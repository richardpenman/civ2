/*
 * What the platform layer (src/platform) needs from the machine it runs on:
 * a window to show the desktop in, input, time, cursors, audio, message
 * boxes and network sockets.  Plain C types only: the portable build compiles the game and the
 * platform layer to wasm32 and these become the module's imports (pointers
 * are then offsets into its memory); native builds call them directly.
 * src/host/host_sdl.c implements them with SDL2, host_net.c the sockets.
 */
#ifndef CIV2_HOST_H
#define CIV2_HOST_H

#include <stdint.h>

#ifdef __wasm__
#define HOST_API(name) __attribute__((import_module("host"), import_name(#name))) name
#else
#define HOST_API(name) name
#endif

/* Input events; keys arrive as Windows virtual-key codes. */
enum {
    HOST_EV_NONE, HOST_EV_QUIT, HOST_EV_KEYDOWN, HOST_EV_KEYUP, HOST_EV_MOTION,
    HOST_EV_BUTTONDOWN, HOST_EV_BUTTONUP, HOST_EV_EXPOSED,
    HOST_EV_COMMAND,        /* a native menu's item chosen: vk is its id */
    HOST_EV_WHEEL           /* the mouse wheel turned: button notches, positive away from the user */
};
typedef struct HostEvent {
    int32_t type;
    int32_t vk;             /* keys */
    int32_t x, y;           /* pointer, desktop coordinates */
    int32_t button;         /* 1 left, 2 right, 3 middle */
} HostEvent;

/* Set-up: headless (no window, no audio device) for replays. */
int HOST_API(host_init)(int headless);
void HOST_API(host_quit)(void);
/* The directory the program is in, with a trailing separator ("" if unknown). */
int HOST_API(host_base_path)(char *buf, int size);

/* The window: w x h desktop pixels, scaled; present() shows 32-bit
 * 0xAARRGGBB pixels, rows stride bytes apart.  scale 0: a window without
 * a frame over the display's usable area (the desktop drawn to fill it);
 * negative: full screen, -scale pixels per desktop pixel. */
int HOST_API(host_open_window)(int w, int h, int scale);
/* A window with its frame, maximized; the desktop is its size in pixels
 * over scale (*w x *h; 0: host_desktop_scale).  Returns the scale, 0: none. */
int HOST_API(host_open_maximized_window)(int32_t *w, int32_t *h, int scale);
/* Whole pixels per desktop pixel for a w x h pixel display (the platform
 * layer's too, src/platform/main.c): scale, or (0) as many as leave a
 * 1024x768 desktop; never a desktop smaller than 640x480. */
static inline int host_desktop_scale(int w, int h, int scale)
{
    int most = w / 640 < h / 480 ? w / 640 : h / 480;
    if (scale < 1)
        scale = w / 1024 < h / 768 ? w / 1024 : h / 768;
    if (scale > most)
        scale = most;
    return scale < 1 ? 1 : scale;
}
/* The size of the main display (in the units windows are sized in). */
int HOST_API(host_display_size)(int32_t *w, int32_t *h);
/* ... and of its usable area: without the taskbar, dock or menu bar. */
int HOST_API(host_usable_size)(int32_t *w, int32_t *h);
/* A window of the system's own kind, where there is one (Windows, macOS):
 * mode 0 as is usual there (Windows: maximized; macOS: the screen below
 * the menu bar), 1 full screen, 2 a window.  The desktop is its size in
 * pixels over scale (*w x *h), scale whole pixels per desktop pixel (0: by
 * the display's size: host_desktop_scale); its menu bar is the system's
 * (host_menu_*).  Returns the scale, 0: none (then host_open_window). */
int HOST_API(host_open_native_window)(int32_t *w, int32_t *h, int mode, int scale);

/* The native window's menu bar, described whole: begin, the items in
 * order (level 0: the bar's menus; a popup's items follow it one level
 * deeper), end.  Text as Windows menus have it: '&' mnemonics, a tab
 * before the shortcut. */
enum { HOST_MENU_POPUP = 1, HOST_MENU_SEPARATOR = 2, HOST_MENU_GRAYED = 4, HOST_MENU_CHECKED = 8 };
void HOST_API(host_menu_begin)(void);
void HOST_API(host_menu_add)(int level, int flags, int id, const char *text);
void HOST_API(host_menu_end)(void);
void HOST_API(host_present)(const void *pixels, int stride);
/* Minimize the window (the desktop's main window was minimized). */
void HOST_API(host_minimize)(void);

/* 1 if an event was returned; wait: up to timeout_ms (-1: forever). */
int HOST_API(host_poll_event)(HostEvent *e);
int HOST_API(host_wait_event)(HostEvent *e, int timeout_ms);

/* Monotonic clock. */
uint64_t HOST_API(host_time_us)(void);
void HOST_API(host_delay_ms)(uint32_t ms);

/* Cursors: handles > 0.  System kinds: 0 arrow, 1 I-beam, 2 wait, 3 cross,
 * 4 size NW-SE, 5 size NE-SW, 6 size W-E, 7 size N-S, 8 size all, 9 no, 10 hand. */
int HOST_API(host_cursor_system)(int kind);
int HOST_API(host_cursor_image)(const uint32_t *argb, int w, int h, int hot_x, int hot_y);
void HOST_API(host_cursor_set)(int cursor);
void HOST_API(host_cursor_free)(int cursor);
void HOST_API(host_cursor_show)(int show);

/* Audio: 1 if there is any; devices > 0 play queued PCM (8 or 16-bit). */
int HOST_API(host_audio_init)(void);
int HOST_API(host_audio_open)(int freq, int channels, int bits);
void HOST_API(host_audio_queue)(int dev, const void *data, uint32_t len);
uint32_t HOST_API(host_audio_queued)(int dev);
void HOST_API(host_audio_clear)(int dev);
void HOST_API(host_audio_close)(int dev);

/* A modal message box; returns the chosen button's id, -1 if it failed.
 * icon: 0 information, 1 warning, 2 error; enter/escape: the indices of
 * the buttons those keys choose (-1: none). */
int HOST_API(host_message_box)(int icon, const char *title, const char *text, int nbuttons,
                               const char *const *labels, const int *ids, int enter, int escape);

/* Network play (src/host/host_net.c): non-blocking IPv4 sockets; handles
 * >= 0, -1 on failure.  Addresses in host byte order (a.b.c.d is
 * a << 24 | b << 16 | c << 8 | d); 0xFFFFFFFF: broadcast. */
/* This machine's address as others reach it, "a.b.c.d"; its length. */
int HOST_API(host_net_local_addr)(char *buf, int size);
/* UDP bound to port on every interface, broadcasts allowed, the port shared. */
int HOST_API(host_net_udp_open)(int port);
int HOST_API(host_net_udp_send)(int s, uint32_t ip, int port, const void *p, int n);
/* One datagram's length (cut to n), -1 if none is waiting. */
int HOST_API(host_net_udp_recv)(int s, void *p, int n, uint32_t *from_ip);
/* TCP, bound to port unless it is 0. */
int HOST_API(host_net_tcp_open)(int port);
int HOST_API(host_net_tcp_listen)(int s, int backlog);
/* A new connection's handle, -1 if none is pending. */
int HOST_API(host_net_tcp_accept)(int s, uint32_t *peer_ip);
/* Starts connecting: 0, -1 failed at once. */
int HOST_API(host_net_tcp_connect)(int s, uint32_t ip, int port);
/* 1 connected, 0 connecting, -1 failed. */
int HOST_API(host_net_tcp_state)(int s);
/* Bytes taken (maybe fewer than n), -1 would block, -2 dead. */
int HOST_API(host_net_tcp_send)(int s, const void *p, int n);
/* Bytes read, 0 closed by the peer, -1 nothing to read, -2 error. */
int HOST_API(host_net_tcp_recv)(int s, void *p, int n);
/* Graceful close (FIN, what is still coming dropped). */
void HOST_API(host_net_close)(int s);

#endif
