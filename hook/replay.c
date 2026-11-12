/*
 * Deterministic replay: makes a game run a pure function of (seed, script).
 *
 * Wall-clock time leaks into Civ2 in several ways: input arrives whenever the
 * harness sends it, animation loops poll timeGetTime(), timeSetEvent()
 * callbacks run on a separate thread, and window timers post WM_TIMER.  Under
 * those conditions even two runs of the original diverge eventually.
 *
 * With CIV2_SCRIPT set, this module takes over all of those through
 * civ2.exe's import table:
 *   - a virtual clock replaces timeGetTime/GetTickCount/Sleep/time/
 *     GetLocalTime/GetSystemTime; it advances by fixed steps on idle
 *     PeekMessage calls and on clock queries, never by itself;
 *   - timeSetEvent callbacks and SetTimer timers fire synchronously, on the
 *     game thread, when the virtual clock passes their deadline;
 *   - the input script is interpreted in virtual time and injected with
 *     SendInput from the game thread, so each input lands at the same point
 *     of the program's execution in every run;
 *   - screenshots are taken in-process at script-defined points.
 * The same table patching applies to the original and the reconstructed
 * build, so both see an identical environment.
 *
 * Script commands (one per line, '#' comments):
 *   wait MS          let MS virtual milliseconds pass
 *   key K [K...]     press and release keys (Return, Escape, F1, a, shift+a, KP_8 ...)
 *   type TEXT        type characters
 *   click X Y [B]    move to screen X,Y and click button B (1 left, 3 right)
 *   shot NAME        save screenshot NAME.bmp in CIV2_SHOT_DIR
 *   file PATH        answer the next Open/Save file dialog with PATH
 *                    (relative to the game directory) instead of showing it
 *   quit             end the run
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hook.h"
#include <commdlg.h>

#define IDLE_STEP_MS 10         /* virtual time per PeekMessage that finds nothing */
#define QUERY_STEP_US 50        /* virtual time per clock query, in microseconds */
#define CLOCK_BASE_MS 1000000u  /* starting value of the virtual tick counter */

static int active;
static unsigned long long vclock_us;   /* virtual microseconds since start */
static char shot_dir[MAX_PATH];

/* ------------------------------------------------------------------ */
/* Import table patching                                              */

static void **find_import(const char *dll, const char *func)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
    IMAGE_DATA_DIRECTORY *dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    IMAGE_IMPORT_DESCRIPTOR *imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir->VirtualAddress);
    for (; imp->Name; imp++) {
        IMAGE_THUNK_DATA *names, *slots;
        if (_stricmp((char *)(base + imp->Name), dll) != 0)
            continue;
        names = (IMAGE_THUNK_DATA *)(base + (imp->OriginalFirstThunk ? imp->OriginalFirstThunk : imp->FirstThunk));
        slots = (IMAGE_THUNK_DATA *)(base + imp->FirstThunk);
        for (; names->u1.AddressOfData; names++, slots++) {
            IMAGE_IMPORT_BY_NAME *ibn;
            if (IMAGE_SNAP_BY_ORDINAL(names->u1.Ordinal))
                continue;
            ibn = (IMAGE_IMPORT_BY_NAME *)(base + names->u1.AddressOfData);
            if (strcmp((char *)ibn->Name, func) == 0)
                return (void **)&slots->u1.Function;
        }
    }
    return NULL;
}

void *hook_patch_import(const char *dll, const char *func, void *repl)
{
    void **slot = find_import(dll, func);
    void *orig;
    DWORD old;
    if (!slot) {
        hook_logf("replay: import %s!%s not found\n", dll, func);
        return NULL;
    }
    orig = *slot;
    VirtualProtect(slot, sizeof *slot, PAGE_READWRITE, &old);
    *slot = repl;
    VirtualProtect(slot, sizeof *slot, old, &old);
    return orig;
}

/* ------------------------------------------------------------------ */
/* Virtual clock and timers                                           */

static DWORD vticks(void)
{
    return CLOCK_BASE_MS + (DWORD)(vclock_us / 1000);
}

struct vtimer {
    int used;
    int is_mm;              /* timeSetEvent (callback) vs SetTimer (WM_TIMER) */
    UINT id;
    HWND hwnd;
    UINT_PTR wm_id;
    TIMERPROC proc;
    LPTIMECALLBACK cb;
    DWORD_PTR user;
    UINT period_ms;
    int periodic;
    unsigned long long due_us;
};

#define MAX_TIMERS 64
static struct vtimer timers[MAX_TIMERS];
static UINT next_mm_id = 1;
static int in_timer_dispatch;

extern int replay_msg_count;
static int clk_win(void) { static int lo = -2, hi; if (lo == -2) { const char *e = getenv("CIV2_CLOCKLOG"); lo = -1; if (e) sscanf(e, "%d-%d", &lo, &hi); } return lo >= 0 && replay_msg_count >= lo && replay_msg_count <= hi; }
#define CLK(...) do { if (clk_win()) hook_logf(__VA_ARGS__); } while (0)
static int hwnd_index(HWND h);

static void fire_due_timers(void)
{
    int i, rounds;
    if (in_timer_dispatch)
        return;
    in_timer_dispatch = 1;
    /* Fire every tick that is due, in deadline order, so periodic timers
     * keep their rate however coarse the clock steps are. */
    for (rounds = 0; rounds < 1000; rounds++) {
        struct vtimer *t = NULL;
        for (i = 0; i < MAX_TIMERS; i++)
            if (timers[i].used && timers[i].due_us <= vclock_us && (!t || timers[i].due_us < t->due_us))
                t = &timers[i];
        if (!t)
            break;
        if (t->periodic)
            t->due_us += (unsigned long long)(t->period_ms ? t->period_ms : 1) * 1000;
        else
            t->used = 0;
        CLK("clk timer %s %u @%llu\n", t->is_mm ? "mm" : "wm", t->is_mm ? t->id : (unsigned)t->wm_id, vclock_us);
        if (t->is_mm)
            t->cb(t->id, 0, t->user, 0, 0);
        else if (t->periodic)
            PostMessageA(t->hwnd, WM_TIMER, t->wm_id, (LPARAM)t->proc);
    }
    in_timer_dispatch = 0;
}

static void advance_us(unsigned long long us)
{
    vclock_us += us;
    CLK("clk advance %llu @%llu\n", us, vclock_us);
    fire_due_timers();
}

static DWORD WINAPI v_timeGetTime(void)
{
    vclock_us += QUERY_STEP_US;
    return vticks();
}

static DWORD WINAPI v_GetTickCount(void)
{
    vclock_us += QUERY_STEP_US;
    return vticks();
}

static VOID WINAPI v_Sleep(DWORD ms)
{
    advance_us((unsigned long long)ms * 1000);
}

static MMRESULT WINAPI v_timeSetEvent(UINT delay, UINT res, LPTIMECALLBACK cb, DWORD_PTR user, UINT flags)
{
    int i;
    (void)res;
    for (i = 0; i < MAX_TIMERS; i++) {
        if (!timers[i].used) {
            struct vtimer *t = &timers[i];
            memset(t, 0, sizeof *t);
            t->used = 1;
            t->is_mm = 1;
            t->id = next_mm_id++;
            t->cb = cb;
            t->user = user;
            t->period_ms = delay;
            t->periodic = (flags & TIME_PERIODIC) != 0;
            t->due_us = vclock_us + (unsigned long long)delay * 1000;
            return t->id;
        }
    }
    return 0;
}

static MMRESULT WINAPI v_timeKillEvent(UINT id)
{
    int i;
    for (i = 0; i < MAX_TIMERS; i++)
        if (timers[i].used && timers[i].is_mm && timers[i].id == id) {
            timers[i].used = 0;
            return TIMERR_NOERROR;
        }
    return MMSYSERR_INVALPARAM;
}

static UINT_PTR WINAPI v_SetTimer(HWND hwnd, UINT_PTR id, UINT ms, TIMERPROC proc)
{
    int i, free_slot = -1;
    CLK("clk SetTimer w%d id=%u ms=%u @%llu\n", hwnd_index(hwnd), (unsigned)id, ms, vclock_us);
    for (i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].used && !timers[i].is_mm && timers[i].hwnd == hwnd && timers[i].wm_id == id) {
            free_slot = i;      /* replacing an existing timer */
            break;
        }
        if (!timers[i].used && free_slot < 0)
            free_slot = i;
    }
    if (free_slot < 0)
        return 0;
    memset(&timers[free_slot], 0, sizeof timers[free_slot]);
    timers[free_slot].used = 1;
    timers[free_slot].hwnd = hwnd;
    timers[free_slot].wm_id = hwnd ? id : (UINT_PTR)(0x7000 + free_slot);
    timers[free_slot].proc = proc;
    timers[free_slot].period_ms = ms;
    timers[free_slot].periodic = 1;
    timers[free_slot].due_us = vclock_us + (unsigned long long)ms * 1000;
    return timers[free_slot].wm_id;
}

static BOOL WINAPI v_KillTimer(HWND hwnd, UINT_PTR id)
{
    int i;
    CLK("clk KillTimer w%d id=%u @%llu\n", hwnd_index(hwnd), (unsigned)id, vclock_us);
    for (i = 0; i < MAX_TIMERS; i++)
        if (timers[i].used && !timers[i].is_mm && timers[i].hwnd == hwnd && timers[i].wm_id == id) {
            timers[i].used = 0;
            return TRUE;
        }
    return FALSE;
}

/* Fixed calendar: 1999-03-26 12:00:00 plus virtual time. */
static void v_systemtime(SYSTEMTIME *st)
{
    unsigned long long s = vclock_us / 1000000;
    memset(st, 0, sizeof *st);
    st->wYear = 1999;
    st->wMonth = 3;
    st->wDay = 26;
    st->wDayOfWeek = 5;
    st->wHour = (WORD)(12 + (s / 3600) % 12);
    st->wMinute = (WORD)((s / 60) % 60);
    st->wSecond = (WORD)(s % 60);
    st->wMilliseconds = (WORD)((vclock_us / 1000) % 1000);
}

static VOID WINAPI v_GetLocalTime(SYSTEMTIME *st) { v_systemtime(st); }
static VOID WINAPI v_GetSystemTime(SYSTEMTIME *st) { v_systemtime(st); }

/* ------------------------------------------------------------------ */
/* Input                                                              */

static struct { const char *name; WORD vk; } keynames[] = {
    {"Return", VK_RETURN}, {"Enter", VK_RETURN}, {"Escape", VK_ESCAPE}, {"Tab", VK_TAB},
    {"space", VK_SPACE}, {"BackSpace", VK_BACK}, {"Delete", VK_DELETE}, {"Insert", VK_INSERT},
    {"Home", VK_HOME}, {"End", VK_END}, {"Prior", VK_PRIOR}, {"Next", VK_NEXT},
    {"Up", VK_UP}, {"Down", VK_DOWN}, {"Left", VK_LEFT}, {"Right", VK_RIGHT},
    {"shift", VK_SHIFT}, {"ctrl", VK_CONTROL}, {"alt", VK_MENU},
    {"KP_0", VK_NUMPAD0}, {"KP_1", VK_NUMPAD1}, {"KP_2", VK_NUMPAD2}, {"KP_3", VK_NUMPAD3},
    {"KP_4", VK_NUMPAD4}, {"KP_5", VK_NUMPAD5}, {"KP_6", VK_NUMPAD6}, {"KP_7", VK_NUMPAD7},
    {"KP_8", VK_NUMPAD8}, {"KP_9", VK_NUMPAD9}, {"KP_Add", VK_ADD}, {"KP_Subtract", VK_SUBTRACT},
    {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5}, {"F6", VK_F6},
    {"F7", VK_F7}, {"F8", VK_F8}, {"F9", VK_F9}, {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},
    {NULL, 0}
};

static WORD vk_of(const char *name)
{
    int i;
    for (i = 0; keynames[i].name; i++)
        if (_stricmp(keynames[i].name, name) == 0)
            return keynames[i].vk;
    if (strlen(name) == 1) {
        SHORT r = VkKeyScanA(name[0]);
        return r == -1 ? 0 : (WORD)(r & 0xff);
    }
    return 0;
}

static void send_key(WORD vk, int up)
{
    INPUT in;
    memset(&in, 0, sizeof in);
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.wScan = (WORD)MapVirtualKeyA(vk, 0);
    in.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
    if (vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT || vk == VK_HOME ||
        vk == VK_END || vk == VK_PRIOR || vk == VK_NEXT || vk == VK_INSERT || vk == VK_DELETE)
        in.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    SendInput(1, &in, sizeof in);
}

/* "shift+a" -> press modifiers then key (down), or release in reverse (up).
 * Presses and releases are separate script steps with idle time between, as
 * with a human typist: the game reacts to a key-down by creating windows that
 * must be initialised before the matching key-up reaches them. */
static void press_combo(char *combo, int up)
{
    WORD vks[4];
    int n = 0, i;
    char *tok = strtok(combo, "+");
    while (tok && n < 4) {
        WORD vk = vk_of(tok);
        if (!vk)
            hook_logf("replay: unknown key '%s'\n", tok);
        else
            vks[n++] = vk;
        tok = strtok(NULL, "+");
    }
    if (!up)
        for (i = 0; i < n; i++)
            send_key(vks[i], 0);
    else
        for (i = n - 1; i >= 0; i--)
            send_key(vks[i], 1);
}

/* mousedown/mousemove/mouseup X Y: the pointer moved there, the left
 * button pressed or released (drags), as src/platform/replay.c. */
static void mouse_drag(int x, int y, int down_up)
{
    INPUT in[2];
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN), n = 1;
    memset(in, 0, sizeof in);
    in[0].type = INPUT_MOUSE;
    in[0].mi.dx = (x * 65535 + w / 2) / (w - 1);
    in[0].mi.dy = (y * 65535 + h / 2) / (h - 1);
    in[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    if (down_up) {
        in[1].type = INPUT_MOUSE;
        in[1].mi.dwFlags = down_up > 0 ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
        n = 2;
    }
    SendInput(n, in, sizeof in[0]);
}

static void mouse_click(int x, int y, int button)
{
    INPUT in[3];
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
    memset(in, 0, sizeof in);
    in[0].type = INPUT_MOUSE;
    in[0].mi.dx = (x * 65535 + w / 2) / (w - 1);
    in[0].mi.dy = (y * 65535 + h / 2) / (h - 1);
    in[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    in[1].type = INPUT_MOUSE;
    in[1].mi.dwFlags = button == 3 ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    in[2].type = INPUT_MOUSE;
    in[2].mi.dwFlags = button == 3 ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
    SendInput(3, in, sizeof in[0]);
}

/* Input goes to the foreground window.  Without a window manager, closing a
 * window can leave nothing active; give focus back to the game's topmost
 * visible window, as a desktop window manager would. */
static BOOL CALLBACK find_top(HWND w, LPARAM out)
{
    DWORD pid;
    GetWindowThreadProcessId(w, &pid);
    if (pid == GetCurrentProcessId() && IsWindowVisible(w) && IsWindowEnabled(w)) {
        *(HWND *)out = w;
        return FALSE;      /* EnumWindows goes top to bottom in Z order */
    }
    return TRUE;
}

static void ensure_focus(void)
{
    HWND fg = GetForegroundWindow(), top = NULL;
    DWORD pid = 0;
    GUITHREADINFO gti;
    if (fg)
        GetWindowThreadProcessId(fg, &pid);
    memset(&gti, 0, sizeof gti);
    gti.cbSize = sizeof gti;
    GetGUIThreadInfo(GetCurrentThreadId(), &gti);

    if (fg && pid == GetCurrentProcessId() && IsWindowVisible(fg)) {
        /* Destroying the focused child leaves no focus; under X the window
         * system's next FocusIn event would restore it to the active window. */
        if (!gti.hwndFocus && gti.hwndActive) {
            hook_logf("replay: restoring focus to %p\n", (void *)gti.hwndActive);
            SetFocus(gti.hwndActive);
        }
        return;
    }
    EnumWindows(find_top, (LPARAM)&top);
    if (top) {
        hook_logf("replay: focusing window %p\n", (void *)top);
        SetForegroundWindow(top);
        SetFocus(top);
    }
}

/* ------------------------------------------------------------------ */
/* Screenshots                                                        */

static void screenshot(const char *name)
{
    char path[MAX_PATH + 64];
    int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
    HDC screen = GetDC(NULL), mem = CreateCompatibleDC(screen);
    BITMAPINFO bi;
    void *bits;
    HBITMAP bmp;
    FILE *f;
    BITMAPFILEHEADER fh;

    GdiFlush();
    memset(&bi, 0, sizeof bi);
    bi.bmiHeader.biSize = sizeof bi.bmiHeader;
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 24;
    bmp = CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, screen, 0, 0, SRCCOPY);
    GdiFlush();
    _snprintf(path, sizeof path, "%s\\%s.bmp", shot_dir, name);
    f = fopen(path, "wb");
    if (f) {
        DWORD stride = ((DWORD)w * 3 + 3) & ~3u;
        memset(&fh, 0, sizeof fh);
        fh.bfType = 0x4d42;
        fh.bfOffBits = sizeof fh + sizeof bi.bmiHeader;
        fh.bfSize = fh.bfOffBits + stride * h;
        fwrite(&fh, sizeof fh, 1, f);
        fwrite(&bi.bmiHeader, sizeof bi.bmiHeader, 1, f);
        fwrite(bits, stride, h, f);
        fclose(f);
    }
    DeleteDC(mem);
    DeleteObject(bmp);
    ReleaseDC(NULL, screen);
    hook_logf("shot %s at t=%lu ms\n", name, (unsigned long)(vclock_us / 1000));
}

/* ------------------------------------------------------------------ */
/* File dialogs: answered from the script (their modal loop would      */
/* otherwise run outside the game's message pump).                     */

static char next_file[MAX_PATH];

static BOOL file_dialog(OPENFILENAMEA *ofn, const char *kind)
{
    if (!next_file[0]) {
        hook_logf("replay: %s dialog with no scripted answer; cancelling\n", kind);
        return FALSE;
    }
    hook_logf("replay: %s dialog -> %s\n", kind, next_file);
    lstrcpynA(ofn->lpstrFile, next_file, ofn->nMaxFile);
    if (ofn->lpstrFileTitle) {
        const char *base = strrchr(next_file, '\\');
        lstrcpynA(ofn->lpstrFileTitle, base ? base + 1 : next_file, ofn->nMaxFileTitle);
    }
    {
        const char *base = strrchr(ofn->lpstrFile, '\\');
        const char *dot = strrchr(ofn->lpstrFile, '.');
        ofn->nFileOffset = (WORD)(base ? base - ofn->lpstrFile + 1 : 0);
        ofn->nFileExtension = (WORD)(dot ? dot - ofn->lpstrFile + 1 : 0);
    }
    next_file[0] = 0;
    return TRUE;
}

/* A message box (a file saved over, an error): answered at once with its
 * first button, as the native build's replays do (src/platform/misc.c);
 * Wine's own box would wait for input the script cannot give it. */
static int WINAPI v_MessageBoxA(HWND owner, LPCSTR text, LPCSTR caption, UINT type)
{
    (void)owner;
    hook_logf("MessageBox: %s: %s\n", caption ? caption : "", text ? text : "");
    switch (type & MB_TYPEMASK) {
    case MB_YESNO: case MB_YESNOCANCEL: return IDYES;
    default: return IDOK;
    }
}

static BOOL WINAPI v_GetOpenFileNameA(OPENFILENAMEA *ofn) { return file_dialog(ofn, "open"); }
static BOOL WINAPI v_GetSaveFileNameA(OPENFILENAMEA *ofn) { return file_dialog(ofn, "save"); }

/* ------------------------------------------------------------------ */
/* Script interpreter                                                 */

static char **script;
static int script_len, script_pos;
static unsigned long long next_action_us;

static void append_line(const char *line, int *cap)
{
    if (script_len == *cap) {
        *cap = *cap ? *cap * 2 : 64;
        script = realloc(script, *cap * sizeof *script);
    }
    script[script_len++] = _strdup(line);
}

static void load_script(const char *path)
{
    FILE *f = fopen(path, "r");
    char line[512];
    int cap = 0;
    if (!f) {
        hook_logf("replay: cannot open script %s\n", path);
        return;
    }
    while (fgets(line, sizeof line, f)) {
        char *hash = strchr(line, '#');
        char *e;
        if (hash)
            *hash = 0;
        e = line + strlen(line);
        while (e > line && (e[-1] == '\n' || e[-1] == '\r' || e[-1] == ' ' || e[-1] == '\t'))
            *--e = 0;
        if (!line[0])
            continue;
        if (strncmp(line, "key ", 4) == 0) {
            /* One key per step, press and release separately: a burst of
             * presses is not processed reliably. */
            char *k = strtok(line + 4, " \t");
            while (k) {
                char one[80];
                _snprintf(one, sizeof one, "keydown %s", k);
                one[sizeof one - 1] = 0;
                append_line(one, &cap);
                one[3] = 'u'; one[4] = 'p';
                memmove(one + 5, one + 7, strlen(one + 7) + 1);   /* "keyup %s" */
                append_line(one, &cap);
                k = strtok(NULL, " \t");
            }
            continue;
        }
        append_line(line, &cap);
    }
    fclose(f);
}

/* Run script commands that are due; called only when the game is idle. */
static void run_script(void)
{
    while (script_pos < script_len && vclock_us >= next_action_us) {
        char buf[512], *cmd, *arg;
        strcpy(buf, script[script_pos++]);
        cmd = strtok(buf, " \t");
        arg = strtok(NULL, "");
        hook_logf("t=%lu: %s %s\n", (unsigned long)(vclock_us / 1000), cmd, arg ? arg : "");
        if (strcmp(cmd, "wait") == 0) {
            next_action_us = vclock_us + (unsigned long long)atoi(arg) * 1000;
        } else if (strcmp(cmd, "keydown") == 0 || strcmp(cmd, "keyup") == 0) {
            char combo[64];
            ensure_focus();
            strncpy(combo, arg, sizeof combo - 1);
            combo[sizeof combo - 1] = 0;
            press_combo(combo, cmd[3] == 'u');
            next_action_us = vclock_us + 50 * 1000;    /* let it be processed */
            return;
        } else if (strcmp(cmd, "type") == 0) {
            const char *p;
            ensure_focus();
            for (p = arg; p && *p; p++) {
                SHORT r = VkKeyScanA(*p);
                if (r == -1)
                    continue;
                if (r & 0x100)
                    send_key(VK_SHIFT, 0);
                send_key((WORD)(r & 0xff), 0);
                send_key((WORD)(r & 0xff), 1);
                if (r & 0x100)
                    send_key(VK_SHIFT, 1);
            }
            next_action_us = vclock_us + 100 * 1000;
            return;
        } else if (strcmp(cmd, "click") == 0) {
            int x = 0, y = 0, b = 1;
            sscanf(arg, "%d %d %d", &x, &y, &b);
            mouse_click(x, y, b);
            next_action_us = vclock_us + 100 * 1000;
            return;
        } else if (strcmp(cmd, "mousedown") == 0 || strcmp(cmd, "mouseup") == 0 || strcmp(cmd, "mousemove") == 0) {
            int x = 0, y = 0;
            sscanf(arg, "%d %d", &x, &y);
            mouse_drag(x, y, cmd[5] == 'd' ? 1 : cmd[5] == 'u' ? -1 : 0);
            next_action_us = vclock_us + 100 * 1000;
            return;
        } else if (strcmp(cmd, "call") == 0) {
            /* call FUNCTION ADDR [ARGS]: run the original's function at ADDR
               (cdecl) directly; the native replay calls FUNCTION by name */
            char name[128];
            unsigned addr = 0;
            int a[6] = {0, 0, 0, 0, 0, 0};
            sscanf(arg, "%127s %x %d %d %d %d %d %d", name, &addr, &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
            next_action_us = vclock_us + 100 * 1000;
            /* (six words: before, four, a fifth argument was what lay on the
               stack: recordings made then got that) */
            if (addr)
                ((int (__cdecl *)(int, int, int, int, int, int))addr)(a[0], a[1], a[2], a[3], a[4], a[5]);
            return;
        } else if (strcmp(cmd, "shot") == 0) {
            screenshot(arg);
        } else if (strcmp(cmd, "file") == 0) {
            /* Absolute path inside the game directory. */
            GetCurrentDirectoryA(sizeof next_file, next_file);
            strncat(next_file, "\\", sizeof next_file - strlen(next_file) - 1);
            strncat(next_file, arg, sizeof next_file - strlen(next_file) - 1);
        } else if (strcmp(cmd, "quit") == 0) {
            hook_logf("replay: script finished\n");
            ExitProcess(0);
        } else {
            hook_logf("replay: unknown command %s\n", cmd);
        }
    }
    if (script_pos >= script_len && vclock_us >= next_action_us) {
        hook_logf("replay: script finished\n");
        ExitProcess(0);
    }
}

/* ------------------------------------------------------------------ */
/* Message pump hook: the scheduler's heartbeat                       */

static BOOL (WINAPI *real_PeekMessageA)(LPMSG, HWND, UINT, UINT, UINT);

/* CIV2_TRACE_MSG: log every message the game retrieves, windows numbered
 * in order of first appearance (comparable across builds). */
static int trace_msg = -1;
static HWND seen_hwnd[512];
static int nseen;

int replay_hwnd_index(HWND h);

static int hwnd_index(HWND h)
{
    return replay_hwnd_index(h);
}

int replay_hwnd_index(HWND h)
{
    int i;
    if (!h)
        return 0;
    for (i = nseen - 1; i >= 0; i--)
        if (seen_hwnd[i] == h)
            return i + 1;
    /* A window still being created: the innermost creation in progress. */
    for (i = nseen - 1; i >= 0; i--)
        if (seen_hwnd[i] == (HWND)-1) {
            seen_hwnd[i] = h;
            return i + 1;
        }
    return -1;
}

/* Windows are numbered in creation order. */
static HWND (WINAPI *real_CreateWindowExA)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);

static HWND WINAPI v_CreateWindowExA(DWORD ex, LPCSTR cls, LPCSTR title, DWORD style, int x, int y, int cx, int cy,
                                     HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
    int slot = nseen < 512 ? nseen++ : 511;
    HWND h;
    seen_hwnd[slot] = (HWND)-1;
    hook_logf("create w%d %s\n", slot + 1, IS_INTRESOURCE(cls) ? "#" : cls);
    h = real_CreateWindowExA(ex, cls, title, style, x, y, cx, cy, parent, menu, inst, param);
    seen_hwnd[slot] = h;
    return h;
}

int replay_msg_count;           /* messages the game has retrieved (apitrace.c) */

static void log_msg(const MSG *m, UINT lo, UINT hi)
{
    replay_msg_count++;
    if (trace_msg < 0)
        trace_msg = getenv("CIV2_TRACE_MSG") != NULL;
    CLK("clk got %04x w%d wp=%x [%x-%x] #%d @%llu\n", m->message, hwnd_index(m->hwnd), (unsigned)m->wParam, lo, hi,
        replay_msg_count, vclock_us);
    if (trace_msg && m->message != WM_TIMER)
        hook_logf("msg %04x w%d wp=%x [%x-%x] #%d @%llu\n", m->message, hwnd_index(m->hwnd), (unsigned)m->wParam, lo, hi,
                  replay_msg_count, vclock_us);
}

static BOOL WINAPI v_PeekMessageA(LPMSG msg, HWND hwnd, UINT lo, UINT hi, UINT remove)
{
    BOOL r = real_PeekMessageA(msg, hwnd, lo, hi, remove);
    if (r && (remove & PM_REMOVE))
        log_msg(msg, lo, hi);
    if (r) {
        if ((remove & PM_REMOVE) && msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)
            hook_logf("  msg %04x wp=%x lp=%08lx hwnd=%p\n", msg->message, (unsigned)msg->wParam,
                      (unsigned long)msg->lParam, (void *)msg->hwnd);
        return r;
    }
    if (!active)
        return r;
    /* Idle: time passes, timers fire, the script may act. */
    advance_us(IDLE_STEP_MS * 1000);
    run_script();
    r = real_PeekMessageA(msg, hwnd, lo, hi, remove);
    if (r && (remove & PM_REMOVE))
        log_msg(msg, lo, hi);
    return r;
}

static void run_script(void);

/* Polling the keyboard is a scheduling point: the game busy-waits on it
 * (e.g. for a key to be released), so time must pass and the script must be
 * able to act.  The only input is ours, injected synchronously, so the real
 * asynchronous key state is deterministic. */
static SHORT (WINAPI *real_GetAsyncKeyState)(int);

static SHORT WINAPI v_GetAsyncKeyState(int vk)
{
    advance_us(QUERY_STEP_US);
    run_script();
    return real_GetAsyncKeyState(vk);
}

static time_t __cdecl v_time(time_t *t)
{
    time_t v = 922449600 + (time_t)(vclock_us / 1000000);   /* 1999-03-26 12:00 UTC */
    if (t)
        *t = v;
    return v;
}

#define CRT_TIME 0x005F7320      /* statically linked CRT time() */

void replay_init(void)
{
    const char *s = getenv("CIV2_SCRIPT");
    const char *d = getenv("CIV2_SHOT_DIR");
    if (!s || !*s) {
        const char *k = getenv("CIV2_LOG_KEYS");
        if (k && *k)   /* diagnostics only: log keyboard messages, no replay */
            real_PeekMessageA = hook_patch_import("USER32.dll", "PeekMessageA", (void *)v_PeekMessageA);
        return;
    }
    active = 1;
    strncpy(shot_dir, d && *d ? d : ".", sizeof shot_dir - 1);
    load_script(s);
    hook_logf("replay: %d script lines\n", script_len);
    real_PeekMessageA = hook_patch_import("USER32.dll", "PeekMessageA", (void *)v_PeekMessageA);
    if (getenv("CIV2_TRACE_MSG"))
        real_CreateWindowExA = hook_patch_import("USER32.dll", "CreateWindowExA", (void *)v_CreateWindowExA);
    hook_patch_import("WINMM.dll", "timeGetTime", (void *)v_timeGetTime);
    hook_patch_import("KERNEL32.dll", "GetTickCount", (void *)v_GetTickCount);
    hook_patch_import("KERNEL32.dll", "Sleep", (void *)v_Sleep);
    hook_patch_import("WINMM.dll", "timeSetEvent", (void *)v_timeSetEvent);
    hook_patch_import("WINMM.dll", "timeKillEvent", (void *)v_timeKillEvent);
    hook_patch_import("USER32.dll", "SetTimer", (void *)v_SetTimer);
    hook_patch_import("USER32.dll", "KillTimer", (void *)v_KillTimer);
    hook_patch_import("KERNEL32.dll", "GetLocalTime", (void *)v_GetLocalTime);
    hook_patch_import("KERNEL32.dll", "GetSystemTime", (void *)v_GetSystemTime);
    real_GetAsyncKeyState = hook_patch_import("USER32.dll", "GetAsyncKeyState", (void *)v_GetAsyncKeyState);
    hook_patch_import("USER32.dll", "MessageBoxA", (void *)v_MessageBoxA);
    hook_patch_import("comdlg32.dll", "GetOpenFileNameA", (void *)v_GetOpenFileNameA);
    hook_patch_import("comdlg32.dll", "GetSaveFileNameA", (void *)v_GetSaveFileNameA);
    hook_patch_jmp(CRT_TIME, (void *)v_time);
}
