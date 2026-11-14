/*
 * Native menu bars for host_sdl.c: host_menu_win.c (Windows) and
 * host_menu_mac.m (macOS).
 */
#ifndef CIV2_HOST_MENU_H
#define CIV2_HOST_MENU_H

#include <SDL.h>

typedef struct HostMenuItem {
    int level, flags, id;       /* as host_menu_add() */
    char *text;
} HostMenuItem;

/* Make items the window's menu bar (none: an empty bar). */
void host_menu_apply(SDL_Window *window, const HostMenuItem *items, int n);
/* Windows: the window over the whole display, without its title bar and
   frame, its menu bar kept. */
void host_win_full_screen(SDL_Window *window);
/* Windows: the item id of a WM_COMMAND from the menu bar (an SDL_SysWMmsg), else 0. */
int host_menu_syswm_command(const void *msg);
/* macOS: an item was chosen (host_sdl.c turns it into HOST_EV_COMMAND). */
void host_menu_picked(int id);
/* macOS: the program made the active one (its menu bar the one shown). */
void host_mac_activate(void);
/* macOS: the window to the system's full screen and back (as View > Full
   Screen, ctrl-cmd-F). */
void host_mac_full_screen(SDL_Window *window);
/* macOS: the window over the screen but the menu bar and the Dock (maximized). */
void host_mac_fill_screen(SDL_Window *window);
/* macOS: Shift held down now (host_launcher.c). */
int host_mac_shift_held(void);

#endif
