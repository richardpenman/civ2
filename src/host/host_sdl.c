/*
 * The host interface (host.h) over SDL2.
 */
#include "host.h"
#include "host_menu.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../platform/win32_vk.h"
#define STB_VORBIS_HEADER_ONLY
#include "../third_party/stb_vorbis/stb_vorbis.c"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static int screen_w, screen_h;
static Uint64 perf_start;
static Uint32 menu_event;           /* an SDL user event: a native menu's item chosen */

int host_init(int headless)
{
    if (headless && !getenv("SDL_VIDEODRIVER"))
        SDL_setenv("SDL_VIDEODRIVER", "dummy", 0);
    /* macOS: full screen in a Space of its own, the menu bar shown at the
       top edge (read when the video subsystem starts). */
    SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "1");
    /* Windows: the display's own pixels, scaled by the game in whole steps
       (host_desktop_scale), not stretched blurred by the system. */
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "civ2: SDL: %s\n", SDL_GetError());
        return 0;
    }
    perf_start = SDL_GetPerformanceCounter();
    menu_event = SDL_RegisterEvents(1);
    atexit(SDL_Quit);       /* the game ends with exit() */
    return 1;
}

static void music_end_thread(void);

void host_quit(void)
{
    music_end_thread();
    SDL_Quit();
}

int host_base_path(char *buf, int size)
{
    char *base = SDL_GetBasePath();
    snprintf(buf, (size_t)size, "%s", base ? base : "");
    SDL_free(base);
    return (int)strlen(buf);
}

static int create_renderer(void)
{
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    return renderer != NULL;
}

/* Whole pixels per desktop pixel (sharp; any pixels left over a border),
   unless the window is smaller than the desktop: then shrunk to fit. */
static void fit_desktop(void)
{
    int w = 0, h = 0;
    if (renderer && !SDL_GetRendererOutputSize(renderer, &w, &h))
        SDL_RenderSetIntegerScale(renderer, w >= screen_w && h >= screen_h ? SDL_TRUE : SDL_FALSE);
}

/* The w x h desktop shown in the window (fit_desktop). */
static int make_texture(int w, int h)
{
    SDL_RenderSetLogicalSize(renderer, w, h);
    fit_desktop();
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    return texture != NULL;
}

int host_open_window(int w, int h, int scale)
{
    SDL_Rect r;
    int full = scale < 0 || (getenv("CIV2_FULLSCREEN") && *getenv("CIV2_FULLSCREEN"));
    if (scale < 0)
        scale = -scale;
    screen_w = w;
    screen_h = h;
    if (scale < 1 && SDL_GetDisplayUsableBounds(0, &r) == 0)
        /* The desktop's main window has its own caption and borders. */
        window = SDL_CreateWindow("Civilization II", r.x, r.y, r.w, r.h, SDL_WINDOW_BORDERLESS);
    else
        window = SDL_CreateWindow("Civilization II", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  w * (scale < 1 ? 1 : scale), h * (scale < 1 ? 1 : scale),
                                  SDL_WINDOW_RESIZABLE | (full ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
    return window && create_renderer() && make_texture(w, h);
}

/* Once the window manager is done (maximizing): the desktop in its pixels
   (macOS: its size is in points) over scale (host_desktop_scale).  Returns
   the scale, 0: none. */
static int size_desktop(int32_t *w, int32_t *h, int scale)
{
    int cw = 0, ch = 0;
    if (!create_renderer())
        return 0;
    if (SDL_GetRendererOutputSize(renderer, &cw, &ch) || cw < 640 || ch < 480) {
        cw = 1024;
        ch = 768;
    }
    scale = host_desktop_scale(cw, ch, scale);
    *w = screen_w = cw / scale;
    *h = screen_h = ch / scale;
    return make_texture(screen_w, screen_h) ? scale : 0;
}

int host_open_maximized_window(int32_t *w, int32_t *h, int scale)
{
    int i, cw = 0, ch = 0, lw = -1, lh = -1, same = 0;
    window = SDL_CreateWindow("Civilization II", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window)
        return 0;
    /* The size: maximized (changed from 1024x768, the window manager can
       take a while to get to it), then steady a while; at most 3 seconds.
       The desktop's size is fixed from it: one sized before the window
       grew was drawn small in the middle of it. */
    SDL_MaximizeWindow(window);
    for (i = 0; i < 300; i++) {
        SDL_PumpEvents();
        SDL_GetWindowSize(window, &cw, &ch);
        same = cw == lw && ch == lh ? same + 1 : 0;
        if ((cw != 1024 || ch != 768) && same >= 20)
            break;
        lw = cw;
        lh = ch;
        SDL_Delay(10);
    }
    return size_desktop(w, h, scale);
}

#if defined(_WIN32) || defined(__APPLE__)
int host_open_native_window(int32_t *w, int32_t *h, int mode, int scale)
{
    Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;     /* macOS: the Retina pixels */
    SDL_Rect full;
    int i, cw = 0, ch = 0, lw = -1, lh = -1, same = 0;
#ifdef __APPLE__
    /* As is usual: a window without a frame over the screen below the
       menu bar (which stays shown); full screen once the window is up (as
       the green button does: the menu bar shows only at the top edge). */
    SDL_Rect r;
    if (mode == 0 && SDL_GetDisplayUsableBounds(0, &r) == 0)
        window = SDL_CreateWindow("Civilization II", r.x, r.y, r.w, r.h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI);
    else
        window = SDL_CreateWindow("Civilization II", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768,
                                  flags | (mode == 1 ? 0 : SDL_WINDOW_MAXIMIZED));
#else
    /* Full screen (also the default): the window without its title bar
       and frame over the whole display, its menu bar at the top
       (host_win_full_screen; SDL's full screen and frameless windows have
       none); mode 2 (CIV2_WINDOW=1): a window, maximized. */
    window = SDL_CreateWindow("Civilization II", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768,
                              flags | (mode == 2 ? SDL_WINDOW_MAXIMIZED : 0));
    mode = mode == 2 ? 2 : 0;           /* (the size: steady, as a window's) */
#endif
    if (!window)
        return 0;
#ifdef __APPLE__
    host_mac_activate();
    for (i = 0; i < 20; i++) {
        SDL_PumpEvents();
        SDL_Delay(10);
    }
    if (mode == 1)
        host_mac_full_screen(window);
#endif
#ifdef _WIN32
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);     /* WM_COMMAND from the menu bar */
#endif
    host_menu_apply(window, NULL, 0);               /* the client area: what the menu bar leaves */
#ifdef _WIN32
    if (mode != 2)
        host_win_full_screen(window);
#endif
    if (SDL_GetDisplayBounds(0, &full))
        full.w = full.h = 0;
    if (mode == 2)
        SDL_MaximizeWindow(window);
    /* The size once the window manager is done (maximizing, the full
       screen animation): full screen, the display's (or, where it is not
       quite that, as with a camera notch, changed and then steady);
       maximized: changed from 1024x768 and steady (at most 3 seconds:
       a desktop sized before the window grew was drawn small in it);
       else steady a while. */
    for (i = 0; i < 500; i++) {
        SDL_PumpEvents();
        SDL_GetWindowSize(window, &cw, &ch);
        same = cw == lw && ch == lh ? same + 1 : 0;
        if (mode == 1 ? (cw == full.w && ch == full.h) || ((cw != 1024 || ch != 768) && same >= 50) :
            mode == 2 ? ((cw != 1024 || ch != 768) && same >= 20) || i >= 300 : same >= 20)
            break;
        lw = cw;
        lh = ch;
        SDL_Delay(10);
    }
    return size_desktop(w, h, scale);
}
#else
int host_open_native_window(int32_t *w, int32_t *h, int mode, int scale)
{
    (void)w; (void)h; (void)mode; (void)scale;
    return 0;
}
#endif

/* ---- the native menu bar ---- */

static HostMenuItem menu_items[1024];
static int menu_n;

void host_menu_begin(void)
{
    while (menu_n > 0)
        SDL_free(menu_items[--menu_n].text);       /* SDL's heap: SDL2.dll may have another C runtime */
}

void host_menu_add(int level, int flags, int id, const char *text)
{
    if (menu_n < (int)(sizeof menu_items / sizeof menu_items[0])) {
        HostMenuItem *it = &menu_items[menu_n++];
        it->level = level;
        it->flags = flags;
        it->id = id;
        it->text = SDL_strdup(text ? text : "");
    }
}

void host_menu_end(void)
{
#if defined(_WIN32) || defined(__APPLE__)
    if (window)
        host_menu_apply(window, menu_items, menu_n);
#endif
}

void host_menu_picked(int id)
{
    SDL_Event e;
    SDL_zero(e);
    e.type = menu_event;
    e.user.code = id;
    SDL_PushEvent(&e);
}

int host_display_size(int32_t *w, int32_t *h)
{
    SDL_DisplayMode m;
    if (SDL_GetDesktopDisplayMode(0, &m))
        return 0;
    *w = m.w;
    *h = m.h;
    return 1;
}

int host_usable_size(int32_t *w, int32_t *h)
{
    SDL_Rect r;
    if (SDL_GetDisplayUsableBounds(0, &r))
        return 0;
    *w = r.w;
    *h = r.h;
    return 1;
}

void host_present(const void *pixels, int stride)
{
    if (!texture)
        return;
    SDL_UpdateTexture(texture, NULL, pixels, stride);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void host_minimize(void)
{
    if (window)
        SDL_MinimizeWindow(window);
}

/* ---- input ---- */

static int vk_of_key(const SDL_Keysym *k)
{
    SDL_Keycode c = k->sym;
    if (c >= SDLK_a && c <= SDLK_z)
        return 'A' + (int)(c - SDLK_a);
    if (c >= SDLK_0 && c <= SDLK_9)
        return '0' + (int)(c - SDLK_0);
    if (c >= SDLK_F1 && c <= SDLK_F12)
        return VK_F1 + (int)(c - SDLK_F1);
    switch (c) {
    case SDLK_RETURN: return VK_RETURN;
    case SDLK_ESCAPE: return VK_ESCAPE;
    case SDLK_BACKSPACE: return VK_BACK;
    case SDLK_TAB: return VK_TAB;
    case SDLK_SPACE: return VK_SPACE;
    case SDLK_UP: return VK_UP;
    case SDLK_DOWN: return VK_DOWN;
    case SDLK_LEFT: return VK_LEFT;
    case SDLK_RIGHT: return VK_RIGHT;
    case SDLK_HOME: return VK_HOME;
    case SDLK_END: return VK_END;
    case SDLK_PAGEUP: return VK_PRIOR;
    case SDLK_PAGEDOWN: return VK_NEXT;
    case SDLK_INSERT: return VK_INSERT;
    case SDLK_DELETE: return VK_DELETE;
    case SDLK_LSHIFT: return VK_LSHIFT;
    case SDLK_RSHIFT: return VK_RSHIFT;
    case SDLK_LCTRL: return VK_LCONTROL;
    case SDLK_RCTRL: return VK_RCONTROL;
    case SDLK_LALT: return VK_LMENU;
    case SDLK_RALT: return VK_RMENU;
    case SDLK_CAPSLOCK: return VK_CAPITAL;
    case SDLK_KP_0: return (k->mod & KMOD_NUM) ? VK_NUMPAD0 : VK_INSERT;
    case SDLK_KP_1: return (k->mod & KMOD_NUM) ? VK_NUMPAD1 : VK_END;
    case SDLK_KP_2: return (k->mod & KMOD_NUM) ? VK_NUMPAD2 : VK_DOWN;
    case SDLK_KP_3: return (k->mod & KMOD_NUM) ? VK_NUMPAD3 : VK_NEXT;
    case SDLK_KP_4: return (k->mod & KMOD_NUM) ? VK_NUMPAD4 : VK_LEFT;
    case SDLK_KP_5: return (k->mod & KMOD_NUM) ? VK_NUMPAD5 : VK_CLEAR;
    case SDLK_KP_6: return (k->mod & KMOD_NUM) ? VK_NUMPAD6 : VK_RIGHT;
    case SDLK_KP_7: return (k->mod & KMOD_NUM) ? VK_NUMPAD7 : VK_HOME;
    case SDLK_KP_8: return (k->mod & KMOD_NUM) ? VK_NUMPAD8 : VK_UP;
    case SDLK_KP_9: return (k->mod & KMOD_NUM) ? VK_NUMPAD9 : VK_PRIOR;
    case SDLK_KP_PERIOD: return (k->mod & KMOD_NUM) ? VK_DECIMAL : VK_DELETE;
    case SDLK_KP_ENTER: return VK_RETURN;
    case SDLK_KP_PLUS: return VK_ADD;
    case SDLK_KP_MINUS: return VK_SUBTRACT;
    case SDLK_KP_MULTIPLY: return VK_MULTIPLY;
    case SDLK_KP_DIVIDE: return VK_DIVIDE;
    case SDLK_MINUS: return VK_OEM_MINUS;
    case SDLK_EQUALS: return VK_OEM_PLUS;
    case SDLK_LEFTBRACKET: return VK_OEM_4;
    case SDLK_RIGHTBRACKET: return VK_OEM_6;
    case SDLK_SEMICOLON: return VK_OEM_1;
    case SDLK_QUOTE: return VK_OEM_7;
    case SDLK_BACKQUOTE: return VK_OEM_3;
    case SDLK_BACKSLASH: return VK_OEM_5;
    case SDLK_COMMA: return VK_OEM_COMMA;
    case SDLK_PERIOD: return VK_OEM_PERIOD;
    case SDLK_SLASH: return VK_OEM_2;
    }
    return 0;
}

/* SDL event -> host event; 0 for those of no interest. */
static int convert(const SDL_Event *s, HostEvent *e)
{
    memset(e, 0, sizeof *e);
    switch (s->type) {
    case SDL_QUIT:
        e->type = HOST_EV_QUIT;
        return 1;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
#ifdef __APPLE__
        /* Command shortcuts are the system's, not the game's: ctrl-cmd-F
           switches full screen, the rest (cmd-Q, cmd-H ...) the menu bar
           has. */
        if (s->key.keysym.mod & KMOD_GUI) {
            if (s->type == SDL_KEYDOWN && !s->key.repeat && s->key.keysym.sym == SDLK_f &&
                (s->key.keysym.mod & KMOD_CTRL) && window)
                host_mac_full_screen(window);
            return 0;
        }
#endif
        e->vk = vk_of_key(&s->key.keysym);
        if (!e->vk)
            return 0;
        e->type = s->type == SDL_KEYDOWN ? HOST_EV_KEYDOWN : HOST_EV_KEYUP;
        return 1;
    case SDL_MOUSEMOTION:
        e->type = HOST_EV_MOTION;
        e->x = s->motion.x;
        e->y = s->motion.y;
        return 1;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        e->type = s->type == SDL_MOUSEBUTTONDOWN ? HOST_EV_BUTTONDOWN : HOST_EV_BUTTONUP;
        e->x = s->button.x;
        e->y = s->button.y;
        e->button = s->button.button == SDL_BUTTON_LEFT ? 1 : s->button.button == SDL_BUTTON_RIGHT ? 2 : 3;
        return 1;
    case SDL_MOUSEWHEEL:
        if (s->wheel.y == 0)
            return 0;
        e->type = HOST_EV_WHEEL;
        e->button = s->wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -s->wheel.y : s->wheel.y;
        return 1;
    case SDL_WINDOWEVENT:
        if (s->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            fit_desktop();
        if (s->window.event != SDL_WINDOWEVENT_EXPOSED)
            return 0;
        e->type = HOST_EV_EXPOSED;
        return 1;
#ifdef _WIN32
    case SDL_SYSWMEVENT:
        if (!(e->vk = host_menu_syswm_command(s->syswm.msg)))
            return 0;
        e->type = HOST_EV_COMMAND;
        return 1;
#endif
    }
    if (s->type == menu_event && menu_event != (Uint32)-1) {
        e->type = HOST_EV_COMMAND;
        e->vk = s->user.code;
        return 1;
    }
    return 0;
}

int host_poll_event(HostEvent *e)
{
    SDL_Event s;
    while (SDL_PollEvent(&s))
        if (convert(&s, e))
            return 1;
    return 0;
}

int host_wait_event(HostEvent *e, int timeout_ms)
{
    SDL_Event s;
    Uint32 end = SDL_GetTicks() + (Uint32)(timeout_ms < 0 ? 0 : timeout_ms);
    for (;;) {
        int left = timeout_ms < 0 ? -1 : (int)(end - SDL_GetTicks());
        if (timeout_ms >= 0 && left <= 0)
            return host_poll_event(e);
        if (!(timeout_ms < 0 ? SDL_WaitEvent(&s) : SDL_WaitEventTimeout(&s, left)))
            return 0;
        if (convert(&s, e))
            return 1;
    }
}

/* ---- time ---- */

uint64_t host_time_us(void)
{
    return (SDL_GetPerformanceCounter() - perf_start) * 1000000ull / SDL_GetPerformanceFrequency();
}

void host_delay_ms(uint32_t ms)
{
    SDL_Delay(ms);
}

/* ---- cursors ---- */

#define MAX_CURSORS 256
static SDL_Cursor *cursors[MAX_CURSORS];

static int cursor_slot(SDL_Cursor *c)
{
    int i;
    if (!c)
        return 0;
    for (i = 1; i < MAX_CURSORS; i++)
        if (!cursors[i]) {
            cursors[i] = c;
            return i;
        }
    SDL_FreeCursor(c);
    return 0;
}

int host_cursor_system(int kind)
{
    static const SDL_SystemCursor map[] = {SDL_SYSTEM_CURSOR_ARROW, SDL_SYSTEM_CURSOR_IBEAM, SDL_SYSTEM_CURSOR_WAIT,
                                           SDL_SYSTEM_CURSOR_CROSSHAIR, SDL_SYSTEM_CURSOR_SIZENWSE,
                                           SDL_SYSTEM_CURSOR_SIZENESW, SDL_SYSTEM_CURSOR_SIZEWE, SDL_SYSTEM_CURSOR_SIZENS,
                                           SDL_SYSTEM_CURSOR_SIZEALL, SDL_SYSTEM_CURSOR_NO, SDL_SYSTEM_CURSOR_HAND};
    int n = (int)(sizeof map / sizeof map[0]);
    return cursor_slot(SDL_CreateSystemCursor(map[kind >= 0 && kind < n ? kind : 0]));
}

int host_cursor_image(const uint32_t *argb, int w, int h, int hot_x, int hot_y)
{
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
    SDL_Cursor *c;
    int y;
    if (!s)
        return 0;
    for (y = 0; y < h; y++)
        memcpy((char *)s->pixels + y * s->pitch, argb + y * w, (size_t)w * 4);
    c = SDL_CreateColorCursor(s, hot_x, hot_y);
    SDL_FreeSurface(s);
    return cursor_slot(c);
}

void host_cursor_set(int cursor)
{
    if (cursor > 0 && cursor < MAX_CURSORS && cursors[cursor])
        SDL_SetCursor(cursors[cursor]);
}

void host_cursor_free(int cursor)
{
    if (cursor > 0 && cursor < MAX_CURSORS && cursors[cursor]) {
        SDL_FreeCursor(cursors[cursor]);
        cursors[cursor] = NULL;
    }
}

void host_cursor_show(int show)
{
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

/* ---- audio ---- */

int host_audio_init(void)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0)
        return 1;
    fprintf(stderr, "civ2: no audio: %s\n", SDL_GetError());
    return 0;
}

int host_audio_open(int freq, int channels, int bits)
{
    SDL_AudioSpec want, have;
    memset(&want, 0, sizeof want);
    want.freq = freq;
    want.channels = (Uint8)channels;
    want.format = bits == 8 ? AUDIO_U8 : AUDIO_S16LSB;
    want.samples = 1024;
    {
        SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
        if (dev)
            SDL_PauseAudioDevice(dev, 0);
        return (int)dev;
    }
}

void host_audio_queue(int dev, const void *data, uint32_t len)
{
    SDL_QueueAudio((SDL_AudioDeviceID)dev, data, len);
}

uint32_t host_audio_queued(int dev)
{
    return SDL_GetQueuedAudioSize((SDL_AudioDeviceID)dev);
}

void host_audio_clear(int dev)
{
    SDL_ClearQueuedAudio((SDL_AudioDeviceID)dev);
}

void host_audio_close(int dev)
{
    SDL_CloseAudioDevice((SDL_AudioDeviceID)dev);
}

/* ---- music: decoded on a thread of its own ----
 *
 * The program's loop fed the music before; a system menu bar (Windows,
 * macOS) runs its own loop while a menu is open, and the music stopped once
 * what was queued had played.  Here a thread keeps about a second queued. */

static struct {
    SDL_mutex *lock;
    SDL_Thread *thread;
    int quit;
    unsigned char *file;        /* the file's bytes */
    stb_vorbis *ogg;            /* its decoder (Ogg Vorbis), else WAV samples: */
    const unsigned char *pcm;
    uint32_t pcm_left;
    int playing, ended;         /* playing: until all of it has played */
    int dev, rate, chans;
} music;

static uint32_t rd32(const unsigned char *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }

static void music_close(void)
{
    if (music.ogg)
        stb_vorbis_close(music.ogg);
    music.ogg = NULL;
    free(music.file);
    music.file = NULL;
    music.pcm = NULL;
    music.pcm_left = 0;
    music.playing = 0;
}

static int music_thread(void *arg)
{
    static short buf[22050];
    (void)arg;
    SDL_LockMutex(music.lock);
    while (!music.quit) {
        uint32_t want = (uint32_t)(music.rate * music.chans * 2);
        while (music.playing && !music.ended && SDL_GetQueuedAudioSize((SDL_AudioDeviceID)music.dev) < want) {
            uint32_t n;
            if (music.ogg) {
                int got = stb_vorbis_get_samples_short_interleaved(music.ogg, music.chans, buf,
                                                                   (int)(sizeof buf / sizeof buf[0]));
                n = got > 0 ? (uint32_t)got * (uint32_t)music.chans * 2 : 0;
                if (n)
                    SDL_QueueAudio((SDL_AudioDeviceID)music.dev, buf, n);
            } else {
                n = music.pcm_left < sizeof buf ? music.pcm_left : (uint32_t)sizeof buf;
                if (n)
                    SDL_QueueAudio((SDL_AudioDeviceID)music.dev, music.pcm, n);
                music.pcm += n;
                music.pcm_left -= n;
            }
            if (!n)
                music.ended = 1;
        }
        if (music.playing && music.ended && !SDL_GetQueuedAudioSize((SDL_AudioDeviceID)music.dev))
            music_close();
        SDL_UnlockMutex(music.lock);
        SDL_Delay(20);
        SDL_LockMutex(music.lock);
    }
    SDL_UnlockMutex(music.lock);
    return 0;
}

static void music_end_thread(void)
{
    if (!music.thread)
        return;
    SDL_LockMutex(music.lock);
    music.quit = 1;
    SDL_UnlockMutex(music.lock);
    SDL_WaitThread(music.thread, NULL);
    music.thread = NULL;
}

/* The music's output at rate, chans (16-bit samples); 0: none. */
static int music_device(int rate, int chans)
{
    if (music.dev && (music.rate != rate || music.chans != chans)) {
        SDL_CloseAudioDevice((SDL_AudioDeviceID)music.dev);
        music.dev = 0;
    }
    if (!music.dev) {
        music.dev = host_audio_open(rate, chans, 16);
        music.rate = rate;
        music.chans = chans;
    }
    return music.dev != 0;
}

static int music_start(const unsigned char *file, uint32_t len)
{
    if (!(music.file = malloc(len ? len : 1)))
        return 0;
    memcpy(music.file, file, len);
    if (len >= 4 && !memcmp(file, "OggS", 4)) {
        int err;
        stb_vorbis_info info;
        if (!(music.ogg = stb_vorbis_open_memory(music.file, (int)len, &err, NULL)))
            return 0;
        info = stb_vorbis_get_info(music.ogg);
        return info.channels >= 1 && info.channels <= 2 && music_device((int)info.sample_rate, info.channels);
    }
    if (len >= 12 && !memcmp(file, "RIFF", 4) && !memcmp(file + 8, "WAVE", 4)) {
        uint32_t at = 12, rate = 0, chans = 0, bits = 0;
        while (len - at >= 8) {
            uint32_t size = rd32(music.file + at + 4);
            at += 8;
            if (size > len - at)
                size = len - at;
            if (!memcmp(music.file + at - 8, "fmt ", 4) && size >= 16) {
                chans = music.file[at + 2] | music.file[at + 3] << 8;
                rate = rd32(music.file + at + 4);
                bits = music.file[at + 14] | music.file[at + 15] << 8;
            } else if (!memcmp(music.file + at - 8, "data", 4)) {
                if (bits != 16 || !rate || chans < 1 || chans > 2 || !music_device((int)rate, (int)chans))
                    return 0;
                music.pcm = music.file + at;
                music.pcm_left = size & ~(chans * 2 - 1);
                return 1;
            }
            at += size + (size & 1);
            if (at > len)
                break;
        }
    }
    return 0;
}

int host_music_play(const void *file, uint32_t len)
{
    int ok;
    if (!music.lock && !(music.lock = SDL_CreateMutex()))
        return 0;
    SDL_LockMutex(music.lock);
    music_close();
    if (music.dev)
        SDL_ClearQueuedAudio((SDL_AudioDeviceID)music.dev);
    ok = music_start(file, len);
    if (ok) {
        music.playing = 1;
        music.ended = 0;
    } else
        music_close();
    SDL_UnlockMutex(music.lock);
    if (ok && !music.thread && !(music.thread = SDL_CreateThread(music_thread, "music", NULL))) {
        host_music_stop();
        return 0;
    }
    return ok;
}

void host_music_stop(void)
{
    if (!music.lock)
        return;
    SDL_LockMutex(music.lock);
    music_close();
    if (music.dev)
        SDL_ClearQueuedAudio((SDL_AudioDeviceID)music.dev);
    SDL_UnlockMutex(music.lock);
}

int host_music_playing(void)
{
    int playing;
    if (!music.lock)
        return 0;
    SDL_LockMutex(music.lock);
    playing = music.playing;
    SDL_UnlockMutex(music.lock);
    return playing;
}

/* ---- message boxes ---- */

int host_message_box(int icon, const char *title, const char *text, int nbuttons, const char *const *labels,
                     const int *ids, int enter, int escape)
{
    SDL_MessageBoxButtonData buttons[8];
    SDL_MessageBoxData data;
    int i, hit = -1;
    if (nbuttons > 8)
        nbuttons = 8;
    for (i = 0; i < nbuttons; i++) {
        buttons[i].flags = (i == enter ? SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT : 0) |
                           (i == escape ? SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT : 0);
        buttons[i].buttonid = ids[i];
        buttons[i].text = labels[i];
    }
    memset(&data, 0, sizeof data);
    data.flags = icon == 2 ? SDL_MESSAGEBOX_ERROR : icon == 1 ? SDL_MESSAGEBOX_WARNING : SDL_MESSAGEBOX_INFORMATION;
    data.window = window;
    data.title = title;
    data.message = text;
    data.numbuttons = nbuttons;
    data.buttons = buttons;
    if (SDL_ShowMessageBox(&data, &hit))
        return -1;
    return hit;
}
