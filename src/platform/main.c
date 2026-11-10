/*
 * Program entry for native builds: host set-up (src/host), the game's static data and
 * initializers, then the game's WinMain.  Also presentation (desktop
 * surface -> host window) and host input events.
 *
 * Usage: civ2 [GAMEDIR]   (default: the current directory)
 * Environment: CIV2_DATA (native data directory, default: next to the
 * executable), CIV2_SCALE (window scale), CIV2_SCRIPT etc. (replay.c).
 */
#include "plat.h"
#include "../host/host.h"
#include <unistd.h>

void gamedata_init(void);
int gamedata_check_layout(void);
extern void (*const civ2_c_inits[])(void);
extern void (*const civ2_cpp_inits[])(void);
int __stdcall win_main(int inst, int prev, int cmdline, int show);   /* WinMain */
void replay_init(void);
void resources_init(void);
void sound_poll(void);

static int window_open;
static uint64_t last_present, present_cost;

void plat_present(void)
{
    uint64_t now;
    if (!window_open)
        return;
    now = host_time_us();
    /* at most ~66 a second, and no more than a third of the time: where
       presenting is slow (a software renderer) the game, which retrieves
       messages between the tiles it draws, would spend its time here */
    if (now - last_present < 15000 || now - last_present < 2 * present_cost)
        return;
    last_present = now;
    if (!plat_overlay)
        plat_compose();
    host_present(screen->bits, screen->stride);
    present_cost = host_time_us() - now;
}

/* Closing the window during a game: asked first (Cancel the default:
   Enter does not end a game by accident); 1 quit, 0 not, -1 no message
   box on this host. */
static int confirm_quit(void)
{
    static const char *const labels[2] = {"Quit", "Cancel"};
    static const int ids[2] = {1, 0};
    return host_message_box(1, "Civilization II",
                            "Quit Civilization II?\n\nThe game in progress ends here; whatever was not saved is lost.",
                            2, labels, ids, 1, 1);
}

/* Now, whatever the frame rate: before waiting for input (menus). */
void plat_present_now(void)
{
    last_present = 0;
    plat_present();
}

/* The host's window changed size (full screen and back, or resized by
   hand): the desktop follows it, at the scale it opened with where that
   leaves 640x480 (src/host/host.h: host_desktop_scale), never smaller
   (then shrunk to fit).  Not while a popup menu is drawn on the screen
   itself: once it is gone.  In a game the windows are then arranged anew,
   as View > Arrange Windows does (where the game would take the command). */
static int want_scale, resize_w, resize_h;

static void resize_desktop(void)
{
    int k = host_desktop_scale(resize_w, resize_h, want_scale), w = resize_w / k, h = resize_h / k, id;
    HWND main_wnd = 0;
    Window *c;
    if (!resize_w || plat_overlay)
        return;
    resize_w = resize_h = 0;
    if (w < 640) w = 640;
    if (h < 480) h = 480;
    if (w == screen_w && h == screen_h)
        return;
    plat_scale = k;
    host_resize_desktop(w, h);
    wnd_resize_desktop(w, h);
    last_present = 0;
    for (c = desktop->children; c && !main_wnd; c = c->next)
        if (c->menu && (c->style & WS_VISIBLE))
            main_wnd = c->hwnd;
    if (main_wnd && game_in_play() && IsWindowEnabled(main_wnd) &&
        (id = menu_find_enabled(GetMenu(main_wnd), "Arrange Windows")))
        PostMessageA(main_wnd, WM_COMMAND, (WPARAM)id, 0);
}

int msg_pump_events(int wait)
{
    HostEvent e;
    int n = 0;
    sound_poll();               /* (a replay has audio only with CIV2_TEST_AUDIO) */
    if (plat_replay) {
        /* Input comes from the script only; keep the window responsive. */
        while (window_open && host_poll_event(&e))
            ;
        return 0;
    }
    menu_native_sync();
    while (wait ? host_wait_event(&e, 10) : host_poll_event(&e)) {
        wait = 0;
        n++;
        switch (e.type) {
        case HOST_EV_QUIT:
            /* The window closed (its close button, Alt+F4, its menu's
               Close): at once without a game, else once confirmed (or as
               the game closes its window, without a message box). */
            if (!game_in_play())
                exit(0);
            switch (confirm_quit()) {
            case 1:
                exit(0);
            case -1:
                if (active_wnd)
                    PostMessageA(active_wnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                else
                    exit(0);
            }
            break;
        case HOST_EV_KEYDOWN:
            /* Ctrl+Q (Game > Quit) with no game in progress: exits at once,
               as the menu's Quit does, also while a video plays, when the
               game reads no keys. */
            if (e.vk == 'Q' && GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_MENU) >= 0 &&
                GetKeyState(VK_SHIFT) >= 0 && !game_in_play())
                exit(0);
            /* fall through */
        case HOST_EV_KEYUP:
            input_key(e.vk, e.type == HOST_EV_KEYDOWN);
            break;
        case HOST_EV_MOTION:
            input_mouse_move(e.x, e.y);
            break;
        case HOST_EV_BUTTONDOWN:
        case HOST_EV_BUTTONUP:
            input_mouse_move(e.x, e.y);
            input_mouse_button(e.button, e.type == HOST_EV_BUTTONDOWN);
            break;
        case HOST_EV_WHEEL:
            input_mouse_wheel(e.button);
            break;
        case HOST_EV_EXPOSED:
            last_present = 0;
            break;
        case HOST_EV_COMMAND:
            menu_native_command(e.vk);
            break;
        case HOST_EV_RESIZE:
            resize_w = e.x;
            resize_h = e.y;
            break;
        }
    }
    resize_desktop();
    return n;
}

/* ---- start-up ---- */

static void set_dirs(const char *argv0, const char *gamedir)
{
    const char *d;
    char base[1024];
    host_base_path(base, sizeof base);
    if (gamedir && chdir(gamedir))
        plat_fatal("cannot enter the game directory %s", gamedir);
    if (!getcwd(plat_game_dir, 1024))
        plat_fatal("no current directory");
    d = getenv("CIV2_WINDIR");
    snprintf(plat_win_dir, 1024, "%s", d && *d ? d : plat_game_dir);
    d = getenv("CIV2_DATA");
    if (d && *d)
        snprintf(plat_data_dir, 1024, "%s", d);
    else {
        /* A release: data beside the program. A build in the source tree
           (build/OpenCiv2): the tree's data/native. */
        char probe[1100];
        FILE *f;
        snprintf(plat_data_dir, 1024, "%sdata", *base ? base : "./");
        snprintf(probe, sizeof probe, "%s/fonts_win.bin", plat_data_dir);
        if (!(f = fopen(probe, "rb"))) {
            snprintf(probe, sizeof probe, "%s../data/native/fonts_win.bin", *base ? base : "./");
            if ((f = fopen(probe, "rb")))
                snprintf(plat_data_dir, 1024, "%s../data/native", *base ? base : "./");
        }
        if (f)
            fclose(f);
    }
    (void)argv0;
}


int main(int argc, char **argv)
{
    const char *scale = getenv("CIV2_SCALE");
    int i, full = 0;
    char cmdline[1] = "";
#ifdef CIV2_UNITTEST
    /* The unit tests' runner (tests/unit_run.c): the game's data and
       initializers, then recorded cases instead of the game; no window. */
    const int unit_test = 1;
    setenv("CIV2_HOOK_LOG", "/dev/null", 0);   /* the platform's log: not with the results */
#else
    const int unit_test = 0;
#endif
    int classic = getenv("CIV2_CLASSIC") && *getenv("CIV2_CLASSIC");
    plat_replay = !unit_test && getenv("CIV2_SCRIPT") && *getenv("CIV2_SCRIPT");
    /* The original's bugs are fixed (docs/BUGS.md) except with CIV2_CLASSIC=1
       (the original game) and in replays and unit tests, which are compared
       with the original; CIV2_TEST_FIXES=1 fixes them in a replay (tests). */
    plat_fix_bugs = !classic && !unit_test &&
        (!plat_replay || (getenv("CIV2_TEST_FIXES") && *getenv("CIV2_TEST_FIXES")));
    /* The interface additions (docs/EXTRAS.md) likewise; CIV2_TEST_EXTRAS
       turns them on in a replay (tests). */
    /* Text laid out as on Windows (src/platform/text.c: open_fonts), not as
       in the reference environment: in play, classic mode included (the
       original on Windows), not in replays or the interface additions'
       tests, which are compared with recordings (CIV2_TEST_WIN_FONTS=1: in
       a replay too, tests). */
    plat_win_fonts = !unit_test &&
        (!plat_replay || (getenv("CIV2_TEST_WIN_FONTS") && *getenv("CIV2_TEST_WIN_FONTS")));
    plat_extras = !classic && !unit_test &&
        (!plat_replay || (getenv("CIV2_TEST_EXTRAS") && *getenv("CIV2_TEST_EXTRAS")));
    /* Replays reproduce the reference desktop (Wine without a window
       manager) unless CIV2_NORMAL_DESKTOP is set. */
    plat_normal_desktop = !plat_replay || (getenv("CIV2_NORMAL_DESKTOP") && *getenv("CIV2_NORMAL_DESKTOP"));
    if (!host_init(unit_test || (plat_replay && !getenv("CIV2_SHOW"))))
        return 1;
    set_dirs(argv[0], argc > 1 && !unit_test ? argv[1] : NULL);
    clock_init();
    if (!plat_replay && !unit_test) {
        /* A window with its frame, maximized; CIV2_FULLSCREEN=1 or CIV.INI's
         * [OpenCiv2] FullScreen=1: full screen, as the original on Windows.
         * Where the system has its own (Windows, macOS): its window and menu
         * bar (full screen: Windows' with the menu bar shown, macOS' own).
         * Elsewhere the menu bar is drawn inside the host's frame. With
         * CIV2_CLASSIC=1 the caption and frame are drawn as the original's
         * too. The desktop is the window's size (full screen: the display's),
         * in steps of whole pixels per desktop pixel:
         * CIV.INI's Scale=, or (0) Windows' display scale, elsewhere by the
         * size (host_desktop_scale).
         * CIV2_SCALE=N: the 1024x768 desktop in a window, N times. */
        int fullscreen = (getenv("CIV2_FULLSCREEN") && *getenv("CIV2_FULLSCREEN")) ||
                         GetPrivateProfileIntA("OpenCiv2", "FullScreen", 0, "CIV.INI") != 0;
        int want = (int)GetPrivateProfileIntA("OpenCiv2", "Scale", 0, "CIV.INI"), k;
        int32_t dw, dh;
        plat_host_frame = !classic;
        plat_lock_panels = !classic;
        plat_fullscreen = fullscreen;
        if (!classic && !scale && (k = host_open_native_window(&dw, &dh, fullscreen ? 1 : 2, want))) {
            screen_w = dw;
            screen_h = dh;
            plat_scale = want_scale = k;
            window_open = plat_native_menu = 1;
        } else if (scale) {
            plat_fullscreen = 0;
        } else if (fullscreen) {
            if (host_display_size(&dw, &dh) && dw >= 800 && dh >= 600) {
                full = k = host_desktop_scale(dw, dh, want);
                screen_w = dw / k;
                screen_h = dh / k;
                plat_scale = k;
            }
        } else if ((k = host_open_maximized_window(&dw, &dh, want))) {
            screen_w = dw;
            screen_h = dh;
            plat_scale = want_scale = k;
            window_open = 1;
        }
    }
    if (plat_replay && getenv("CIV2_TEST_FULL") && *getenv("CIV2_TEST_FULL")) {
        /* Tests of OpenCiv2's additions, which have no reference: the
           game's window maximized on a smaller desktop, all of it shown
           (CIV2_TEST_FULL=WxH, or 800x600) */
        int w = 0, h = 0;
        if (sscanf(getenv("CIV2_TEST_FULL"), "%dx%d", &w, &h) != 2 || w < 640 || h < 480)
            w = 800, h = 600;
        screen_w = w;
        screen_h = h;
    }
    if (plat_replay && getenv("CIV2_TEST_NATIVE") && *getenv("CIV2_TEST_NATIVE"))
        /* Tests: the game as on Windows and macOS (the host's menu bar, no
           caption of its own, panels locked), the host's part left out. */
        plat_host_frame = plat_native_menu = plat_lock_panels = 1;
    gdi_init();
    wnd_init();
    resources_init();
    sound_init();
    if (!window_open && !unit_test && (!plat_replay || getenv("CIV2_SHOW"))) {
        window_open = host_open_window(screen_w, screen_h, full ? -full : scale ? atoi(scale) : 1);
        if (scale && atoi(scale) > 1)
            plat_scale = atoi(scale);
    }
    replay_init();
    extras_load();
    pointer_init(screen_w / 2, screen_h / 2);      /* where the X server puts it */
    /* The game's static data, then the C and C++ initializers the MSVC
     * runtime ran before WinMain. */
    gamedata_init();
    gamedata_check_layout();
    for (i = 0; civ2_c_inits[i]; i++)
        civ2_c_inits[i]();
    for (i = 0; civ2_cpp_inits[i]; i++)
        civ2_cpp_inits[i]();
#ifdef CIV2_UNITTEST
    {
        int unit_run_main(int argc, char **argv);
        int r = unit_run_main(argc, argv);
        host_quit();
        return r;
    }
#endif
    win_main((int)(uintptr_t)plat_exe_module(), 0, (int)(uintptr_t)cmdline, SW_SHOWNORMAL);
    host_quit();
    return 0;
}
