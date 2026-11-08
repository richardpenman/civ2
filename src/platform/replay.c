/*
 * Deterministic replay for native builds: the counterpart of the harness's
 * hook runtime (hook/replay.c, hook/determinism.c) for the original game,
 * with the same script language, virtual clock and outputs, so runs of the
 * two can be compared (harness/compare.py).
 *
 *   CIV2_SCRIPT=FILE     run the input script (see hook/replay.c)
 *   CIV2_SHOT_DIR=DIR    where `shot NAME` saves NAME.bmp
 *   CIV2_SEED=N          force the seed of every srand()
 *   CIV2_RAND_LOG=FILE   log every rand(): calling function and value
 *   CIV2_HOOK_LOG=FILE   log of script actions (plat_log)
 *   CIV2_CALL_LOG=FILE   log of the sounds played and the videos looked for
 *                        (as hook/stackprobe.c logs the original's)
 *   CIV2_BENCHMARK=1    log wall-clock microseconds for finite call_sync actions
 *   CIV2_BENCHMARK_FRAME=1 also compose the desktop before ending that timer
 */
#ifndef __wasm__
#include <dlfcn.h>
#include <execinfo.h>
#endif
#include "plat.h"
#include "hd.h"
#include "../host/host.h"

static char **script;
static int script_len, script_pos;
static unsigned long long next_action_us;
static char shot_dir[1024] = ".";
static char next_file[1024];
static FILE *rand_log;
static FILE *call_log;
static int seed_forced;
static unsigned forced_seed;

/* ---- sounds and videos (CIV2_CALL_LOG) ----
 * The reference has no audio device, no video files and no Indeo codec,
 * so what is compared is what the game asks for: every snd_play_effect
 * (effect, mode, loop) and every video path file_resolve_path is given. */

void replay_log_sound(int effect, int mode, int loop)
{
    if (call_log)
        fprintf(call_log, "sound %d %d %d\n", effect, mode, loop);
}

void replay_log_video(const char *path)
{
    size_t n = path ? strlen(path) : 0;
    if (call_log && n >= 4 && strcasecmp(path + n - 4, ".avi") == 0)
        fprintf(call_log, "video %s\n", path);
}

/* ---- rand trace ---- */

unsigned plat_srand(unsigned seed)
{
    if (rand_log)
        fprintf(rand_log, "srand %u\n", seed_forced ? forced_seed : seed);
    plat_log("srand(%u)%s\n", seed, seed_forced ? " [forced]" : "");
    return seed_forced ? forced_seed : seed;
}

/* caller: the name of the function that called rand(). */
void plat_trace_rand(unsigned value, const char *caller)
{
    static int count, bt_from = -1, bt_to = -1;
    if (!rand_log)
        return;
    /* CIV2_RAND_BT=FROM-TO: call stacks of those calls (1-based) */
    if (bt_from == -1) {
        const char *e = getenv("CIV2_RAND_BT");
        bt_from = 0;
        if (e)
            sscanf(e, "%d-%d", &bt_from, &bt_to);
    }
    if (++count >= bt_from && count <= bt_to) {
        plat_log("rand call %d\n", count);
#ifndef __wasm__
        {
            void *frames[16];
            Dl_info info;
            int n = backtrace(frames, 16), i;
            for (i = 1; i < n; i++)
                if (dladdr(frames[i], &info) && info.dli_sname)
                    fprintf(rand_log, "  #%d %s\n", i, info.dli_sname);
        }
#endif
    }
    fprintf(rand_log, "%s %u\n", caller, value);
}

/* ---- script ---- */

static void append(const char *line, int *cap)
{
    if (script_len == *cap) {
        *cap = *cap ? *cap * 2 : 64;
        script = realloc(script, sizeof *script * (size_t)*cap);
    }
    script[script_len++] = plat_strdup(line);
}

static void load_script(const char *path)
{
    FILE *f = fopen(path, "r");
    char line[512];
    int cap = 0;
    if (!f)
        plat_fatal("cannot open script %s", path);
    while (fgets(line, sizeof line, f)) {
        char *hash = strchr(line, '#'), *e;
        if (hash)
            *hash = 0;
        e = line + strlen(line);
        while (e > line && (e[-1] == '\n' || e[-1] == '\r' || e[-1] == ' ' || e[-1] == '\t'))
            *--e = 0;
        if (!line[0])
            continue;
        if (!strncmp(line, "key ", 4)) {
            /* One key per step, pressed and released separately. */
            char *save = NULL, *k;
            for (k = strtok_r(line + 4, " \t", &save); k; k = strtok_r(NULL, " \t", &save)) {
                char one[96];
                snprintf(one, sizeof one, "keydown %s", k);
                append(one, &cap);
                snprintf(one, sizeof one, "keyup %s", k);
                append(one, &cap);
            }
            continue;
        }
        append(line, &cap);
    }
    fclose(f);
}

static const struct { const char *name; int vk; } keynames[] = {
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

/* VkKeyScan for a US layout: virtual key, 0x100 if shift is needed. */
static int vk_scan(int c)
{
    static const char *shifted = ")!@#$%^&*(", *punct = ";=,-./`[\\]'", *punct_sh = ":+<_>?~{|}\"";
    static const int punct_vk[] = {VK_OEM_1, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_2,
                                   VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7};
    const char *p;
    if (c >= 'a' && c <= 'z') return toupper(c);
    if (c >= 'A' && c <= 'Z') return c | 0x100;
    if (c >= '0' && c <= '9') return c;
    if (c == ' ') return VK_SPACE;
    if ((p = strchr(shifted, c)) && c) return ('0' + (int)(p - shifted)) | 0x100;
    if ((p = strchr(punct, c)) && c) return punct_vk[p - punct];
    if ((p = strchr(punct_sh, c)) && c) return punct_vk[p - punct_sh] | 0x100;
    return -1;
}

static int vk_of(const char *name)
{
    int i;
    for (i = 0; keynames[i].name; i++)
        if (!strcasecmp(keynames[i].name, name))
            return keynames[i].vk;
    if (strlen(name) == 1) {
        int r = vk_scan(name[0]);
        return r < 0 ? 0 : r & 0xff;
    }
    return 0;
}

static void press_combo(char *combo, int up)
{
    int vks[4], n = 0, i;
    char *save = NULL, *tok;
    for (tok = strtok_r(combo, "+", &save); tok && n < 4; tok = strtok_r(NULL, "+", &save)) {
        int vk = vk_of(tok);
        if (!vk)
            plat_log("replay: unknown key '%s'\n", tok);
        else
            vks[n++] = vk;
    }
    if (!up)
        for (i = 0; i < n; i++)
            input_key(vks[i], 1);
    else
        for (i = n - 1; i >= 0; i--)
            input_key(vks[i], 0);
}

/* Input goes to the focus; with none, to the topmost usable window (what
 * the reference X server's focus handling amounts to). */
static void ensure_focus(void)
{
    Window *w;
    if (GetFocus())
        return;
    if (active_wnd && wnd_get(active_wnd)) {
        plat_log("replay: restoring focus to %x\n", (unsigned)active_wnd);
        SetFocus(active_wnd);
        return;
    }
    for (w = desktop->children; w; w = w->next)
        if ((w->style & WS_VISIBLE) && !(w->style & WS_DISABLED)) {
            plat_log("replay: focusing window %x\n", (unsigned)w->hwnd);
            SetFocus(w->hwnd);
            return;
        }
}

/* Screenshot of the desktop as a 24-bit BMP. */
void plat_screenshot(const char *path)
{
    FILE *f = fopen(path, "wb");
    int w=screen_w*(hd_enabled?hd_density:1), h=screen_h*(hd_enabled?hd_density:1);
    int y, x, stride = (w * 3 + 3) & ~3;
    unsigned char hdr[54] = {'B', 'M'}, *row;
    unsigned size = 54 + (unsigned)(stride * h);
    if (!f)
        return;
    if (!plat_overlay)          /* (as presenting) a popup menu is on the screen itself */
        plat_compose();
    if(hd_enabled)hd_read_region(screen,0,0,w,h);
    hdr[2] = (unsigned char)size; hdr[3] = (unsigned char)(size >> 8); hdr[4] = (unsigned char)(size >> 16);
    hdr[5] = (unsigned char)(size >> 24);
    hdr[10] = 54; hdr[14] = 40;
    hdr[18] = (unsigned char)w; hdr[19] = (unsigned char)(w >> 8);
    hdr[22] = (unsigned char)h; hdr[23] = (unsigned char)(h >> 8);
    hdr[26] = 1; hdr[28] = 24;
    fwrite(hdr, 1, 54, f);
    row = calloc((size_t)stride, 1);
    for (y = h - 1; y >= 0; y--) {
        const DWORD *p = hd_enabled ? screen->hd+y*w : (const DWORD *)bmp_row(screen, y);
        for (x = 0; x < w; x++) {
            row[x * 3] = (unsigned char)p[x];
            row[x * 3 + 1] = (unsigned char)(p[x] >> 8);
            row[x * 3 + 2] = (unsigned char)(p[x] >> 16);
        }
        fwrite(row, 1, (size_t)stride, f);
    }
    free(row);
    fclose(f);
}

int replay_file_dialog(OPENFILENAMEA *ofn, const char *kind)
{
    const char *base, *dot;
    if (!next_file[0]) {
        plat_log("replay: %s dialog with no scripted answer; cancelling\n", kind);
        return FALSE;
    }
    plat_log("replay: %s dialog -> %s\n", kind, next_file);
    snprintf(ofn->lpstrFile, ofn->nMaxFile, "%s", next_file);
    base = strrchr(next_file, '\\');
    if (ofn->lpstrFileTitle)
        snprintf(ofn->lpstrFileTitle, ofn->nMaxFileTitle, "%s", base ? base + 1 : next_file);
    base = strrchr(ofn->lpstrFile, '\\');
    dot = strrchr(ofn->lpstrFile, '.');
    ofn->nFileOffset = (WORD)(base ? base - ofn->lpstrFile + 1 : 0);
    ofn->nFileExtension = (WORD)(dot ? dot - ofn->lpstrFile + 1 : 0);
    next_file[0] = 0;
    return TRUE;
}

static void finish(void)
{
    plat_log("replay: script finished\n");
    if (rand_log)
        fclose(rand_log);
    if (call_log)
        fclose(call_log);
    crt_exit_process(0);
}

/* Run due script commands; called when the game is idle. */
void replay_run_script(void)
{
    static int running;
    if (!plat_replay || running)
        return;
    running = 1;
    while (script_pos < script_len && vclock_us >= next_action_us) {
        char buf[512], *cmd, *arg, *save = NULL;
        snprintf(buf, sizeof buf, "%s", script[script_pos++]);
        cmd = strtok_r(buf, " \t", &save);
        arg = strtok_r(NULL, "", &save);
        plat_log("t=%lu: %s %s\n", (unsigned long)(vclock_us / 1000), cmd, arg ? arg : "");
        if (!strcmp(cmd, "wait")) {
            next_action_us = vclock_us + (unsigned long long)atoi(arg) * 1000;
        } else if (!strcmp(cmd, "keydown") || !strcmp(cmd, "keyup")) {
            char combo[64];
            ensure_focus();
            snprintf(combo, sizeof combo, "%s", arg);
            press_combo(combo, cmd[3] == 'u');
            next_action_us = vclock_us + 50 * 1000;
            break;
        } else if (!strcmp(cmd, "type")) {
            const char *p;
            ensure_focus();
            for (p = arg; p && *p; p++) {
                int r = vk_scan((unsigned char)*p);
                if (r < 0)
                    continue;
                if (r & 0x100)
                    input_key(VK_SHIFT, 1);
                input_key(r & 0xff, 1);
                input_key(r & 0xff, 0);
                if (r & 0x100)
                    input_key(VK_SHIFT, 0);
            }
            next_action_us = vclock_us + 100 * 1000;
            break;
        } else if (!strcmp(cmd, "click")) {
            /* click X Y [B]: B 1 left, 2 middle, 3 right */
            int x = 0, y = 0, b = 1;
            sscanf(arg, "%d %d %d", &x, &y, &b);
            input_mouse_move(x, y);
            input_mouse_button(b == 3 ? 2 : b == 2 ? 3 : 1, 1);
            input_mouse_button(b == 3 ? 2 : b == 2 ? 3 : 1, 0);
            next_action_us = vclock_us + 100 * 1000;
            break;
        } else if (!strcmp(cmd, "call") || !strcmp(cmd, "call_sync")) {
            /* call FUNCTION ADDR [ARGS]: run a game function directly (tests
               of screens hard to reach by input); the original's address is
               for hook/replay.c, which runs the same scripts.  The functions
               are those named in the scripts (build/native/gen/calltable.c). */
            char name[128] = "";
            unsigned addr = 0;
            int args[4] = {0, 0, 0, 0};
            sscanf(arg, "%127s %x %d %d %d %d", name, &addr, &args[0], &args[1], &args[2], &args[3]);
            (void)addr;
            {
                extern const struct { const char *name; void (*fn)(void); } replay_calltable[];
                int i;
                for (i = 0; replay_calltable[i].name && strcmp(replay_calltable[i].name, name); i++)
                    ;
                if (replay_calltable[i].name) {
                    uint64_t started = 0;
                    next_action_us = vclock_us + 100 * 1000;
                    /* Modal calls need input from subsequent script commands.
                       Finite redraws use call_sync so a shot cannot interrupt
                       them at an internal message-pump/idle boundary. */
                    if (!strcmp(cmd, "call")) running = 0;
                    if (!strcmp(cmd, "call_sync") && getenv("CIV2_BENCHMARK")) {
                        /* Do not charge previously queued GPU work to this call. */
                        if(hd_gpu_active)host_gpu_fence();
                        started = host_time_us();
                    }
                    ((int (*)(int, int, int, int))replay_calltable[i].fn)(args[0], args[1], args[2], args[3]);
                    if(started && getenv("CIV2_BENCHMARK_FRAME"))plat_compose();
                    if(started && hd_gpu_active)host_gpu_finish();
                    if (started) plat_log("benchmark: %s %llu us\n",name,
                                          (unsigned long long)(host_time_us()-started));
                    running = 1;
                } else
                    plat_log("replay: no function %s (tools/build_native.py gen_calltable)\n", name);
            }
            break;
        } else if (!strcmp(cmd, "mousedown") || !strcmp(cmd, "mouseup") || !strcmp(cmd, "mousemove")) {
            /* mousedown/mousemove/mouseup X Y [B]: drags (B as for click) */
            int x = 0, y = 0, b = 1;
            sscanf(arg, "%d %d %d", &x, &y, &b);
            input_mouse_move(x, y);
            if (cmd[5] != 'm')
                input_mouse_button(b == 3 ? 2 : b == 2 ? 3 : 1, cmd[5] == 'd');
            next_action_us = vclock_us + 100 * 1000;
            break;
        } else if (!strcmp(cmd,"drag_burst")) {
            /* Queue a complete gesture before dispatch, as when input piles
             * up during a redraw. Event positions must survive the burst. */
            int x=0,y=0,dx=0,dy=0;
            sscanf(arg,"%d %d %d %d",&x,&y,&dx,&dy);
            input_mouse_move(x,y);input_mouse_button(1,1);
            for(int i=1;i<=8;i++)input_mouse_move(x+dx*i/8,y+dy*i/8);
            input_mouse_button(1,0);
            next_action_us=vclock_us+100*1000;break;
        } else if (!strcmp(cmd, "camera_probe")) {
            int x=0,y=0;sscanf(arg,"%d %d",&x,&y);
            map_camera_probe(x,y);
        } else if (!strcmp(cmd,"camera_benchmark")) {
            int count=atoi(arg);
            if(count<1)count=1;if(count>200)count=200;
            for(int i=0;i<count+1;i++) {
                uint64_t start;
                if(hd_gpu_active)host_gpu_fence();
                start=host_time_us();plat_compose();
                if(hd_gpu_active)host_gpu_fence();
                if(i)plat_log("camera compose: %llu us\n",(unsigned long long)(host_time_us()-start));
            }
        } else if (!strcmp(cmd, "wheel")) {
            /* wheel X Y N: the wheel turned N notches (positive: away) */
            int x = 0, y = 0, n = 0;
            sscanf(arg, "%d %d %d", &x, &y, &n);
            input_mouse_move(x, y);
            input_mouse_wheel(n);
            next_action_us = vclock_us + 100 * 1000;
            break;
        } else if (!strcmp(cmd,"wheel_precise")) {
            int x=0,y=0;double n=0;sscanf(arg,"%d %d %lf",&x,&y,&n);
            input_mouse_move(x,y);input_mouse_wheel_precise(n);
            next_action_us=vclock_us+100*1000;break;
        } else if (!strcmp(cmd, "menu")) {
            /* menu LABEL: the host's menu bar's item so labelled chosen
               (CIV2_TEST_NATIVE), as with the mouse */
            int id = menu_native_find(arg);
            if (id)
                menu_native_command(id);
            else
                plat_log("replay: no menu item %s\n", arg);
            next_action_us = vclock_us + 100 * 1000;
            break;
        } else if (!strcmp(cmd, "shot")) {
            char path[1300];
            snprintf(path, sizeof path, "%s/%s.bmp", shot_dir, arg);
            plat_screenshot(path);
            plat_log("shot %s at t=%lu ms\n", arg, (unsigned long)(vclock_us / 1000));
        } else if (!strcmp(cmd, "file")) {
            plat_getcwd(next_file, sizeof next_file);
            snprintf(next_file + strlen(next_file), sizeof next_file - strlen(next_file), "\\%s", arg);
        } else if (!strcmp(cmd, "quit")) {
            finish();
        } else {
            plat_log("replay: unknown command %s\n", cmd);
        }
    }
    if (script_pos >= script_len && vclock_us >= next_action_us)
        finish();
    running = 0;
}

void replay_init(void)
{
    const char *s = getenv("CIV2_SEED"), *d = getenv("CIV2_SHOT_DIR"), *r = getenv("CIV2_RAND_LOG");
    if (s && *s) {
        seed_forced = 1;
        forced_seed = (unsigned)strtoul(s, NULL, 0);
    }
    if (r && *r) {
        rand_log = fopen(r, "w");
        if (rand_log)
            setvbuf(rand_log, NULL, _IOLBF, 0);   /* complete even if the game crashes */
    }
    r = getenv("CIV2_CALL_LOG");
    if (r && *r && plat_replay) {
        call_log = fopen(r, "w");
        if (call_log)
            setvbuf(call_log, NULL, _IOLBF, 0);
    }
    if (!plat_replay)
        return;
    if (d && *d)
        snprintf(shot_dir, sizeof shot_dir, "%s", d);
    load_script(getenv("CIV2_SCRIPT"));
    plat_log("replay: %d script lines\n", script_len);
    if(getenv("CIV2_BENCHMARK") && getenv("CIV2_BENCHMARK_FRAME"))
        plat_log("benchmark: includes desktop composition\n");
}
