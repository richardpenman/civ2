/*
 * The native platform layer: the part of the Windows API the game uses,
 * implemented over the host interface (src/host).  Internal declarations
 * shared by the platform's files.
 *
 * Model: one desktop (a host window of the screen size the game asks the
 * system metrics for) holding the game's top-level windows, drawn in
 * software into a 32-bit desktop surface.  Window, GDI and other objects are
 * referred to by small integer handles.  Reference behaviour is Wine's (the
 * environment the original game is recorded in by the harness).
 */
#ifndef CIV2_PLAT_H
#define CIV2_PLAT_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "win32_api.h"
#include "../include/map_camera.h"
#include "plat_files.h"

/* ---- handles ---- */

typedef enum {
    OBJ_NONE, OBJ_WINDOW, OBJ_DC, OBJ_BITMAP, OBJ_PALETTE, OBJ_BRUSH, OBJ_PEN, OBJ_FONT, OBJ_MENU, OBJ_FILE,
    OBJ_FIND, OBJ_MEM, OBJ_CURSOR, OBJ_MODULE, OBJ_RSRC, OBJ_MMIO, OBJ_WAVEOUT, OBJ_KEY, OBJ_MISC, OBJ_HOOK,
    OBJ_AVIFILE, OBJ_AVISTREAM, OBJ_CODEC,
    OBJ_COUNT
} ObjType;

typedef uint32_t Handle;                        /* any handle: (type << 16) | slot */
void *obj_get(Handle h, ObjType t);             /* NULL if h is not a live object of type t */
ObjType obj_type(Handle h);
Handle obj_new(ObjType t, void *obj);           /* returns the new handle */
void obj_free(Handle h);
void obj_set_stock(Handle h);                   /* stock objects survive DeleteObject */
int obj_is_stock(Handle h);

/* ---- clock (virtual in replay mode) ---- */

DWORD plat_ticks(void);                 /* timeGetTime / GetTickCount */
DWORD plat_now(void);                   /* the platform's own clock reads */
void plat_idle(void);                   /* nothing to do: let time pass (replay) or poll events */
void plat_suspicious(const char *fmt, ...);  /* impossible request: logged, fatal with CIV2_STRICT */
int plat_sane_extent(int v);
void crt_exit_process(int code);       /* ExitProcess: exit without the game's atexit list (native/crt.c) */
extern int plat_replay;                 /* deterministic replay active */
extern int plat_normal_desktop;         /* windows framed and captioned as on Windows; 0: as on the
                                           reference (Wine, no window manager), for comparisons */
extern int plat_host_frame;             /* the host owns the main window's caption and frame */
extern int plat_native_menu;            /* the main window's menu bar is the host's (host_menu_*) */
extern int plat_lock_panels;            /* the game's panels (child windows: map, world, status) not
                                           moved or sized by the mouse, as the original allows */
extern int plat_fix_bugs;               /* the original's bugs fixed (docs/BUGS.md): not with
                                           CIV2_CLASSIC=1, in replays or the unit tests */
#include "../include/extras.h"              /* plat_extras: the interface additions (docs/EXTRAS.md),
                                           on or off as plat_fix_bugs */

/* ---- regions: lists of rectangles ---- */

typedef struct { int n, cap; RECT *r; } Region;
void rgn_set(Region *g, const RECT *r);
void rgn_clear(Region *g);
void rgn_free(Region *g);
void rgn_intersect_rect(Region *g, const RECT *r);
void rgn_subtract_rect(Region *g, const RECT *r);
void rgn_union_rect(Region *g, const RECT *r);
void rgn_offset(Region *g, int dx, int dy);
void rgn_copy(Region *dst, const Region *src);
int rgn_empty(const Region *g);
void rgn_bounds(const Region *g, RECT *out);
int rect_empty(const RECT *r);
int rect_intersect(RECT *out, const RECT *a, const RECT *b);

/* ---- bitmaps (surfaces) ---- */

typedef struct Bitmap {
    int w, h, bpp;              /* 1, 8, 16, 24 or 32 */
    int stride;                 /* bytes per row */
    int bottom_up;              /* DIB rows stored bottom row first */
    unsigned char *bits;        /* first stored row */
    int owns_bits;
    RGBQUAD colors[256];        /* colour table for bpp <= 8 */
    int ncolors;
    int is_dib;
    int selected;               /* selected into a DC */
    uint32_t *hd;
    void *hd_before;            /* packed 8-bit indices, otherwise 32-bit raw pixels */
    int hd_image;               /* Atlas source sheet; never stored in saves */
    int hd_grey;                /* grey backdrop presentation, pixels retained */
    int hd_gpu;                 /* optional host render target; HD CPU pixels may be stale */
    struct HdIndexedCache *hd_indexed_cache;
} Bitmap;

unsigned char *bmp_row(const Bitmap *b, int y);     /* row y, top = 0 */
Bitmap *bmp_create(int w, int h, int bpp, int bottom_up);
void bmp_destroy(Bitmap *b);
Bitmap *bmp_of_bits(const void *p);             /* the bitmap with these pixels, or NULL */

/* ---- palettes, brushes, pens, fonts ---- */

typedef struct { int n; PALETTEENTRY e[256]; } Palette;
typedef struct { int style; COLORREF color; Bitmap *pattern; } Brush;
typedef struct { int style, width; COLORREF color; } Pen;

typedef struct FontFace FontFace;       /* loaded glyphs (text.c) */
typedef struct { LOGFONTA lf; FontFace *face; } Font;

/* ---- device contexts ---- */

typedef struct Window Window;

typedef struct DC {
    Window *wnd;                /* window DC: drawing goes to the desktop */
    int is_screen;              /* GetDC(NULL) */
    int is_memory;              /* CreateCompatibleDC */
    Bitmap *bitmap;             /* memory DC: selected bitmap */
    HBITMAP hbitmap, default_bitmap;
    HPALETTE hpalette;
    HBRUSH hbrush;
    HPEN hpen;
    HFONT hfont;
    COLORREF text_color, bk_color;
    int bk_mode;
    UINT text_align;
    POINT pos;                  /* current position (MoveToEx) */
    POINT origin;               /* device origin of the window's client area on the desktop */
    Region clip;                /* in device coordinates of the target surface */
    int paint;                  /* BeginPaint DC */
} DC;

Bitmap *dc_target(DC *dc);              /* surface drawn on */
void dc_update_window(DC *dc);          /* window moved/clipping changed */
COLORREF dc_resolve_color(DC *dc, COLORREF c);
void dc_fill_rect(DC *dc, const RECT *r, COLORREF c);  /* logical coords */
void surf_put(Bitmap *b, int x, int y, COLORREF rgb);
COLORREF surf_get(Bitmap *b, int x, int y);
int surf_nearest(const Bitmap *b, COLORREF rgb);       /* palette index for an 8-bpp surface */

/* ---- windows ---- */

typedef struct WndClass {
    char name[64];
    WNDCLASSA wc;
    int builtin;                /* EDIT, LISTBOX, ... */
    LONG_PTR *extra;            /* class extra bytes (SetClassLong) */
    struct WndClass *next;
} WndClass;

typedef struct Scroll { int min, max, pos, page, shown, enabled; } Scroll;

struct Window {
    HWND hwnd;
    WndClass *cls;
    WNDPROC proc;
    char *text;
    DWORD style, exstyle;
    HMENU menu;                 /* top-level: menu bar; child: control id */
    HINSTANCE instance;
    Window *parent, *owner;
    Window *children;           /* topmost first */
    Window *next;               /* next sibling below */
    RECT rect;                  /* window rect, in parent client (or screen) coordinates */
    RECT client;                /* client rect, relative to the window rect's top left */
    LONG_PTR *extra;            /* cbWndExtra bytes, pointer-sized slots */
    int nextra;
    LONG_PTR userdata;
    Region update;              /* invalid area, client coordinates */
    HDC own_dc;                 /* CS_OWNDC: the window's one DC (GetDC, BeginPaint) */
    int erase;                  /* WM_ERASEBKGND pending */
    int destroyed;
    Scroll hscroll, vscroll;
    void *control;              /* builtin control state */
    int minimized, maximized;
    int restore_max;            /* minimized from maximized */
    RECT normal;                /* placement before minimizing/maximizing */
    int need_size;              /* WM_SIZE/WM_MOVE left for the first ShowWindow */
    int managed;                /* top-level window a window manager would decorate (see wnd_shown_rect) */
    int nc_active;              /* caption drawn active (WM_NCACTIVATE) */
    RECT nc_btn[3];             /* normal desktop: close, maximize, minimize buttons (window coords; empty if none) */
    int nc_pressed;             /* the caption button shown pressed: HTCLOSE etc., 0 none */
    int host_clipped;           /* replay: shown within host_clip, moved by host_dx/dy (see ShowWindow) */
    RECT host_clip;
    int host_dx, host_dy;
    int host_frozen;            /* during the maximizing call itself: no re-sync */
    int host_pending;           /* X window moved: its ConfigureNotify is still to come */
    Bitmap *surf;               /* top-level windows: what they draw (see wnd_surface) */
    RECT surf_client;           /* the client rect the surface was last made for */
};

Window *wnd_get(HWND h);
extern Window *desktop;
void wnd_screen_rect(Window *w, RECT *out);     /* window rect on the desktop */
void wnd_client_screen(Window *w, POINT *origin);
void wnd_visible_region(Window *w, int client, Region *out);   /* desktop coordinates */
int wnd_is_visible(Window *w);
LRESULT wnd_send(Window *w, UINT msg, WPARAM wp, LPARAM lp);
void wnd_invalidate_desktop(const RECT *r);     /* InvalidateRect(NULL) */
void wnd_resize_desktop(int w, int h);          /* the desktop now w x h */
void wnd_expose(Window *top, const RECT *r);    /* repaint an area of a top-level window's surface */
Window *wnd_top(Window *w);
int wnd_nc_button(Window *w, POINT screen);      /* HTCLOSE/HTMAXBUTTON/HTMINBUTTON at a point, or 0 */
void scroll_track_nc(Window *w, int hit);
void wnd_nc_track(Window *w, int hit);           /* DefWindowProc: drag the caption, a border or a button */
Bitmap *wnd_surface(Window *top);
void plat_compose(void);                /* the desktop: background and windows -> screen */
extern Bitmap *root;                    /* the desktop background (GetDC(NULL) draws here) */
extern int plat_overlay;
HDC plat_overlay_dc(void);
HDC WINAPI GetWindowDC(HWND h);
void wnd_nc_paint(Window *w);
int wnd_has_nc(Window *w);
void wnd_shown_rect(Window *w, RECT *r);
void wnd_host_sync(Window *w);
void wnd_host_events(void);
Window *wnd_from_point(POINT pt, int *hit);
extern HWND focus_wnd, capture_wnd, active_wnd;

/* ---- messages and input ---- */

void msg_post(HWND h, UINT msg, WPARAM wp, LPARAM lp);
int msg_pump_events(int wait);         /* host events -> input messages */
void xd_poll(void);                    /* network play's events and callbacks (xdaemon.c) */
void input_key(int vk, int down);
void input_mouse_move(int x, int y);
void input_mouse_button(int button, int down);
void input_mouse_wheel(int notches);       /* WM_MOUSEWHEEL to the focus window */
void input_mouse_wheel_precise(double notches);
void pointer_init(int x, int y);
void pointer_check(void);              /* the pointer's window changed: WM_MOUSEMOVE */
extern BYTE key_state[256];

/* ---- menus ---- */

int menu_bar_height(Window *w);
void menu_bar_paint(Window *w);
int menu_bar_click(Window *w, int x, int y);
int menu_key(Window *w, int vk);
void menu_native_sync(void);            /* the menu bar to the host, if it changed */
void menu_native_command(int id);       /* the host's menu bar: an item chosen */
int menu_find_enabled(HMENU menu, const char *text);  /* its enabled item labelled text: the id, or 0 */
int menu_native_find(const char *text); /* its item labelled text: the id, or 0 */

/* ---- builtin controls ---- */

LRESULT ctl_edit_proc(HWND, UINT, WPARAM, LPARAM);
LRESULT ctl_listbox_proc(HWND, UINT, WPARAM, LPARAM);
LRESULT ctl_combobox_proc(HWND, UINT, WPARAM, LPARAM);
LRESULT ctl_scrollbar_proc(HWND, UINT, WPARAM, LPARAM);
LRESULT ctl_button_proc(HWND, UINT, WPARAM, LPARAM);
LRESULT ctl_static_proc(HWND, UINT, WPARAM, LPARAM);

/* ---- text ---- */

FontFace *font_face_for(const LOGFONTA *lf);
void text_metrics(FontFace *f, TEXTMETRICA *tm);
int text_width(FontFace *f, const char *s, int n);
int text_height(FontFace *f);
void text_draw(DC *dc, int x, int y, const char *s, int n, const RECT *clip);   /* logical coords, top left */
/* kind: 0 city names, 1 population badges, 2 unit text, 3 fixed map messages. */
void text_camera_labels(Bitmap *dst,const MapCameraLabel *labels,int count,double height,
                        int origin_x,int origin_y,const RECT *clip,int kind);

/* ---- presentation ---- */

extern Bitmap *screen;                  /* the desktop surface (32 bpp) */
extern int screen_w, screen_h;
WINBOOL plat_file_dialog(OPENFILENAMEA *ofn, int save);     /* filedlg.c */
HFONT plat_menu_font(void);             /* menu.c: menu bars and captions */
void plat_present(void);
void plat_present_now(void);            /* unthrottled, before blocking on input */                /* copy the desktop to the host window */
void plat_screenshot(const char *path);

/* ---- misc ---- */

void plat_log(const char *fmt, ...);
void api_trace(const char *name, void *caller);
#ifdef __wasm__
#define API_TRACE() api_trace(__func__, 0)       /* (no return addresses: every call counts) */
#else
#define API_TRACE() api_trace(__func__, __builtin_return_address(0))
#endif
void plat_fatal(const char *fmt, ...);
/* A game is being played (src/auto/game.cpp): it has units or cities. */
#ifdef __cplusplus
extern "C"
#endif
int game_in_play(void);
void plat_init(int argc, char **argv);
char *plat_strdup(const char *s);
void timers_fire(void);
void timers_window_destroyed(HWND h);
void msg_window_destroyed(HWND h);
void msg_trace_window_created(HWND h, const char *cls);
extern int msg_trace_internal;
Window *wnd_next_dirty(Window *root);
void scroll_paint_nc(Window *w);
void wnd_init(void);
void gdi_init(void);
void clock_init(void);
void cursor_set_image(HCURSOR c);
DC *dc_get(HDC h);
extern unsigned wnd_generation;
extern unsigned long long vclock_us;
extern char plat_game_dir[], plat_win_dir[], plat_data_dir[];
void replay_run_script(void);
int replay_file_dialog(OPENFILENAMEA *ofn, const char *kind);
void sound_init(void);
HMODULE plat_exe_module(void);

#endif
