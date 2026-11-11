/*
 * GetOpenFileName / GetSaveFileName: a modal file dialog of our own, made of
 * the platform's windows and controls (a list of the directory's folders and
 * of the files the filter selects, a file name box, OK and Cancel).  Paths
 * are Windows paths, as the game uses them (files.c maps them to the host).
 * Replays answer from the script instead (replay.c).
 *
 * Two conveniences, kept in CIV.INI [OpenCiv2] (not in replays): the files
 * can be sorted newest first (FileSort=Date), and the dialog opens in the
 * folder of the last file loaded or saved (FileFolder=), as Windows' own
 * dialog does.
 */
#include "plat.h"
#include <time.h>

#define ID_LIST 10
#define ID_NAME 11
#define ID_DIR 12
#define ID_SORT 13
#define ID_DATE 14
#define MAX_ITEMS 2048

static struct {
    OPENFILENAMEA *ofn;
    int save, done, ok;
    HWND dlg, list, name, dirlabel, sort, date;
    int by_date;
    time_t times[MAX_ITEMS];            /* the list's files' modified times, in its order */
    char dir[MAX_PATH];
    char pattern[MAX_PATH];             /* ;-separated wildcards */
} fd;

/* Case-insensitive wildcard match (* and ?). */
static int wild(const char *p, const char *s)
{
    for (; *p; p++, s++) {
        if (*p == '*') {
            for (; *s; s++)
                if (wild(p + 1, s))
                    return 1;
            return wild(p + 1, s);
        }
        if (!*s || (*p != '?' && tolower((unsigned char)*p) != tolower((unsigned char)*s)))
            return 0;
    }
    return !*s;
}

static int matches(const char *name)
{
    char pats[MAX_PATH], *p, *save = NULL;
    snprintf(pats, sizeof pats, "%s", fd.pattern[0] ? fd.pattern : "*.*");
    for (p = strtok_r(pats, ";", &save); p; p = strtok_r(NULL, ";", &save)) {
        while (*p == ' ')
            p++;
        if (!strcmp(p, "*.*") || wild(p, name))
            return 1;
    }
    return 0;
}

static void join(char *out, size_t n, const char *dir, const char *name)
{
    size_t k = strlen(dir);
    snprintf(out, n, "%s%s%s", dir, k && dir[k - 1] != '\\' ? "\\" : "", name);
}

/* 1 a directory, 0 a file, -1 nothing there. */
static int kind_of(const char *path)
{
    WIN32_FIND_DATAA f;
    HANDLE h = FindFirstFileA(path, &f);
    if (h == INVALID_HANDLE_VALUE)
        return -1;
    FindClose(h);
    return (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

typedef struct {
    char *text;
    time_t time;
} Item;

static int by_name(const void *a, const void *b)
{
    const char *x = ((const Item *)a)->text, *y = ((const Item *)b)->text;
    /* folders ("[...]") first */
    if ((*x == '[') != (*y == '['))
        return *x == '[' ? -1 : 1;
    return strcasecmp(x, y);
}

/* Folders first by name, then files newest first. */
static int by_time(const void *a, const void *b)
{
    const Item *x = a, *y = b;
    if (*x->text != '[' && *y->text != '[' && x->time != y->time)
        return x->time > y->time ? -1 : 1;
    return by_name(a, b);
}

static time_t file_time(const FILETIME *t)
{
    unsigned long long v = (unsigned long long)t->dwHighDateTime << 32 | t->dwLowDateTime;
    return v ? (time_t)(v / 10000000ull - 11644473600ull) : 0;
}

/* (docs/EXTRAS.md) The remembered settings; none in replays. */
static void setting_get(const char *key, char *out, int n)
{
    out[0] = 0;
    if (!plat_replay)
        plat_profile_string("CIV.INI", "OpenCiv2", key, out, n);
}

static void setting_set(const char *key, const char *value)
{
    if (!plat_replay)
        WritePrivateProfileStringA("OpenCiv2", key, value, "CIV.INI");
}

/* A label's new text, drawn now (our controls don't repaint on WM_SETTEXT). */
static void set_text(HWND h, const char *text)
{
    SetWindowTextA(h, text);
    InvalidateRect(h, NULL, TRUE);
}

static void show_date(int i)
{
    char text[64] = "";
    if (i >= 0 && i < MAX_ITEMS && fd.times[i]) {
        time_t t = fd.times[i];
        struct tm *tm = localtime(&t);
        if (tm)
            strftime(text, sizeof text, "Modified: %Y-%m-%d %H:%M", tm);
    }
    set_text(fd.date, text);
}

static void refill(void)
{
    char spec[MAX_PATH], label[MAX_PATH + 16];
    static Item items[MAX_ITEMS];
    WIN32_FIND_DATAA f;
    HANDLE h;
    int n = 0, i;
    SendMessageA(fd.list, LB_RESETCONTENT, 0, 0);
    join(spec, sizeof spec, fd.dir, "*.*");
    if ((h = FindFirstFileA(spec, &f)) != INVALID_HANDLE_VALUE) {
        do {
            int is_dir = (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            char item[MAX_PATH + 2];
            if (!strcmp(f.cFileName, ".") || (is_dir && f.cFileName[0] == '.' && strcmp(f.cFileName, "..")))
                continue;
            if (!is_dir && !matches(f.cFileName))
                continue;
            snprintf(item, sizeof item, is_dir ? "[%s]" : "%s", f.cFileName);
            if (n < MAX_ITEMS) {
                items[n].text = plat_strdup(item);
                items[n++].time = is_dir ? 0 : file_time(&f.ftLastWriteTime);
            }
        } while (FindNextFileA(h, &f));
        FindClose(h);
    }
    qsort(items, (size_t)n, sizeof items[0], fd.by_date ? by_time : by_name);
    for (i = 0; i < n; i++) {
        SendMessageA(fd.list, LB_ADDSTRING, 0, (LPARAM)items[i].text);
        fd.times[i] = items[i].time;
        free(items[i].text);
    }
    show_date(-1);
    snprintf(label, sizeof label, "Folder: %s", fd.dir);
    SetWindowTextA(fd.dirlabel, label);
}

static void enter_dir(const char *name)
{
    if (!strcmp(name, "..")) {
        char *s = strrchr(fd.dir, '\\');
        if (s && s > strchr(fd.dir, '\\'))
            *s = 0;
        else if (s)
            s[1] = 0;                   /* "C:\" */
    } else {
        char d[MAX_PATH];
        join(d, sizeof d, fd.dir, name);
        snprintf(fd.dir, sizeof fd.dir, "%s", d);
    }
    refill();
}

/* The name typed (or chosen): a folder to open, a filter, or the answer. */
static void accept(void)
{
    char name[MAX_PATH], path[MAX_PATH];
    OPENFILENAMEA *ofn = fd.ofn;
    int k;
    GetWindowTextA(fd.name, name, sizeof name);
    if (!name[0])
        return;
    if (strchr(name, '*') || strchr(name, '?')) {
        snprintf(fd.pattern, sizeof fd.pattern, "%s", name);
        refill();
        return;
    }
    if (strchr(name, ':') || name[0] == '\\')
        snprintf(path, sizeof path, "%s", name);
    else
        join(path, sizeof path, fd.dir, name);
    k = kind_of(path);
    if (k == 1) {
        snprintf(fd.dir, sizeof fd.dir, "%s", path);
        SetWindowTextA(fd.name, "");
        refill();
        return;
    }
    /* the default extension, if the name has none */
    if (ofn->lpstrDefExt && *ofn->lpstrDefExt && !strchr(strrchr(path, '\\') ? strrchr(path, '\\') : path, '.')) {
        strncat(path, ".", sizeof path - strlen(path) - 1);
        strncat(path, ofn->lpstrDefExt, sizeof path - strlen(path) - 1);
        k = kind_of(path);
    }
    if (!fd.save && k < 0 && (ofn->Flags & OFN_FILEMUSTEXIST)) {
        char msg[MAX_PATH + 64];
        snprintf(msg, sizeof msg, "%s\nFile not found.", path);
        MessageBoxA(fd.dlg, msg, "Open", MB_OK | MB_ICONWARNING);
        return;
    }
    if (fd.save && k == 0 && (ofn->Flags & OFN_OVERWRITEPROMPT)) {
        char msg[MAX_PATH + 64];
        snprintf(msg, sizeof msg, "%s already exists.\nDo you want to replace it?", path);
        if (MessageBoxA(fd.dlg, msg, "Save As", MB_YESNO | MB_ICONWARNING) != IDYES)
            return;
    }
    snprintf(ofn->lpstrFile, ofn->nMaxFile, "%s", path);
    {
        const char *base = strrchr(ofn->lpstrFile, '\\'), *dot = strrchr(ofn->lpstrFile, '.');
        if (ofn->lpstrFileTitle)
            snprintf(ofn->lpstrFileTitle, ofn->nMaxFileTitle, "%s", base ? base + 1 : ofn->lpstrFile);
        ofn->nFileOffset = (WORD)(base ? base - ofn->lpstrFile + 1 : 0);
        ofn->nFileExtension = (WORD)(dot && dot > base ? dot - ofn->lpstrFile + 1 : 0);
    }
    {   /* the next dialog opens in this file's folder */
        char folder[MAX_PATH], *s;
        snprintf(folder, sizeof folder, "%s", path);
        if ((s = strrchr(folder, '\\')) != NULL) {
            s[s == strchr(folder, '\\') ? 1 : 0] = 0;     /* "C:\" keeps its backslash */
            setting_set("FileFolder", folder);
        }
    }
    if (!(ofn->Flags & OFN_NOCHANGEDIR))
        plat_chdir(fd.dir);
    fd.ok = fd.done = 1;
}

static LRESULT CALLBACK dlg_proc(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    switch (m) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDOK:
            accept();
            return 0;
        case IDCANCEL:
            fd.done = 1;
            return 0;
        case ID_SORT:
            fd.by_date = !fd.by_date;
            set_text(fd.sort, fd.by_date ? "Sort: Date" : "Sort: Name");
            setting_set("FileSort", fd.by_date ? "Date" : "Name");
            refill();
            return 0;
        case ID_LIST: {
            char item[MAX_PATH + 2];
            int i = (int)SendMessageA(fd.list, LB_GETCURSEL, 0, 0);
            if (i < 0 || SendMessageA(fd.list, LB_GETTEXT, (WPARAM)i, (LPARAM)item) <= 0)
                return 0;
            show_date(i);
            if (item[0] == '[') {
                if (HIWORD(wp) == LBN_DBLCLK) {
                    item[strlen(item) - 1] = 0;
                    enter_dir(item + 1);
                }
                return 0;
            }
            SetWindowTextA(fd.name, item);
            if (HIWORD(wp) == LBN_DBLCLK)
                accept();
            return 0;
        }
        }
        break;
    case WM_CLOSE:
        fd.done = 1;
        return 0;
    }
    return DefWindowProcA(h, m, wp, lp);
}

static HWND control(const char *cls, const char *text, DWORD style, int x, int y, int w, int h, int id)
{
    HWND c = CreateWindowExA(0, cls, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, fd.dlg,
                             (HMENU)(uintptr_t)id, NULL, NULL);
    SendMessageA(c, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);
    return c;
}

WINBOOL plat_file_dialog(OPENFILENAMEA *ofn, int save)
{
    static int registered;
    const char *title = ofn->lpstrTitle ? ofn->lpstrTitle : save ? "Save As" : "Open";
    HWND owner = ofn->hwndOwner;
    int w = 440, h = 396, owner_enabled = owner && IsWindowEnabled(owner);
    MSG msg;
    if (!registered) {
        WNDCLASSA wc;
        memset(&wc, 0, sizeof wc);
        wc.lpfnWndProc = dlg_proc;
        wc.lpszClassName = "Civ2FileDialog";
        wc.hbrBackground = (HBRUSH)(uintptr_t)(COLOR_BTNFACE + 1);
        wc.hCursor = LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
        RegisterClassA(&wc);
        registered = 1;
    }
    memset(&fd, 0, sizeof fd);
    fd.ofn = ofn;
    fd.save = save;
    /* the starting folder (the last one used, if it is still there) and the filter */
    if (ofn->lpstrInitialDir && *ofn->lpstrInitialDir)
        snprintf(fd.dir, sizeof fd.dir, "%s", ofn->lpstrInitialDir);
    else {
        char last[MAX_PATH];
        setting_get("FileFolder", last, sizeof last);
        if (last[0] && kind_of(last) == 1)
            snprintf(fd.dir, sizeof fd.dir, "%s", last);
        else
            plat_getcwd(fd.dir, sizeof fd.dir);
    }
    {
        char sort[16];
        setting_get("FileSort", sort, sizeof sort);
        fd.by_date = !strcasecmp(sort, "Date");
    }
    if (ofn->lpstrFilter) {
        const char *f = ofn->lpstrFilter;
        DWORD k = ofn->nFilterIndex ? ofn->nFilterIndex : 1;
        while (*f && --k) {
            f += strlen(f) + 1;     /* description */
            f += strlen(f) + 1;     /* pattern */
        }
        if (*f)
            snprintf(fd.pattern, sizeof fd.pattern, "%s", f + strlen(f) + 1);
    }
    fd.dlg = CreateWindowExA(WS_EX_DLGMODALFRAME, "Civ2FileDialog", title, WS_POPUP | WS_CAPTION | WS_SYSMENU,
                             (screen_w - w) / 2, (screen_h - h) / 2, w, h, owner, NULL, NULL, NULL);
    fd.dirlabel = control("STATIC", "", SS_LEFT, 10, 8, 410, 18, ID_DIR);
    fd.list = control("LISTBOX", "", WS_BORDER | WS_VSCROLL | LBS_NOTIFY, 10, 30, 410, 220, ID_LIST);
    control("STATIC", "File name:", SS_LEFT, 10, 264, 70, 20, -1);
    fd.name = control("EDIT", "", WS_BORDER | ES_AUTOHSCROLL, 85, 260, 335, 24, ID_NAME);
    fd.date = control("STATIC", "", SS_LEFT, 85, 292, 335, 18, ID_DATE);
    fd.sort = control("BUTTON", fd.by_date ? "Sort: Date" : "Sort: Name", BS_PUSHBUTTON, 10, 322, 100, 28, ID_SORT);
    control("BUTTON", save ? "Save" : "Open", BS_DEFPUSHBUTTON, 240, 322, 85, 28, IDOK);
    control("BUTTON", "Cancel", BS_PUSHBUTTON, 335, 322, 85, 28, IDCANCEL);
    if (ofn->lpstrFile && ofn->lpstrFile[0] && !strchr(ofn->lpstrFile, '*')) {
        const char *base = strrchr(ofn->lpstrFile, '\\');
        SetWindowTextA(fd.name, base ? base + 1 : ofn->lpstrFile);
    }
    refill();
    if (owner_enabled)
        EnableWindow(owner, FALSE);
    ShowWindow(fd.dlg, SW_SHOW);
    SetFocus(fd.name);
    while (!fd.done) {
        if (!GetMessageA(&msg, NULL, 0, 0)) {
            PostQuitMessage((int)msg.wParam);
            break;
        }
        if (msg.message == WM_KEYDOWN && (msg.hwnd == fd.dlg || IsChild(fd.dlg, msg.hwnd)) &&
            (msg.wParam == VK_RETURN || msg.wParam == VK_ESCAPE)) {
            if (msg.wParam == VK_ESCAPE)
                fd.done = 1;
            else if (msg.hwnd == fd.list)
                SendMessageA(fd.dlg, WM_COMMAND, MAKEWPARAM(ID_LIST, LBN_DBLCLK), (LPARAM)fd.list);
            else
                accept();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    if (owner_enabled)
        EnableWindow(owner, TRUE);
    DestroyWindow(fd.dlg);
    if (owner)
        SetActiveWindow(owner);
    return fd.ok;
}
