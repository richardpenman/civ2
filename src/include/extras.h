/*
 * OpenCiv2's interface additions (docs/EXTRAS.md).  They follow the
 * features of CIV2UIA, the Civilization II UI Additions launcher by FoxAhead
 * (https://github.com/FoxAhead/Civ2-UI-Additions), reimplemented from its
 * description; each one keeps the number of its item in CIV2UIA's README.
 *
 * They are on unless the original game is wanted: CIV2_CLASSIC=1, replays
 * and unit tests (as bugfix_ui(), docs/BUGS.md), and each can be switched
 * off in the [OpenCiv2] section of CIV.INI (its key below, =0).  They change
 * only what this player sees and does with the interface, so they stay on in
 * network games.  CIV2_TEST_EXTRAS turns them on in a replay (tests only):
 * 1 for all, or a list of item numbers ("5,6,43"), or all but some ("-43").  Each one in the code:
 * if (extra_ui(X_...)) <added> else <original>, marked "X<n>".
 *
 * CIV2UIA's rule changes (17, 18, 19, 36, 42 and the limits) are off by
 * default: extra_rule(X_...), switched on in CIV.INI, off in network games
 * too.  In a replay CIV2_TEST_EXTRAS turns them on by number ("17,42") or
 * with "all".
 */
#ifndef CIV2_EXTRAS_H
#define CIV2_EXTRAS_H

#include <stdint.h>

#ifdef __cplusplus
#define EXTRAS_C extern "C"
#else
#define EXTRAS_C extern
#endif

enum {
    X_WHEEL = 1,            /* MouseWheel: wheel and middle button */
    X_UNIT_LIST = 2,        /* UnitListScroll: Activate Unit list scrolls past 9 units */
    X_WORK_COUNTER = 3,     /* WorkCounter: settlers' work counter on their sprite */
    X_GAME_TURN = 5,        /* GameTurn: the turn in the status panel */
    X_RESEARCH = 6,         /* ResearchNumbers: in the Science Advisor */
    X_ZERO_UPKEEP = 10,     /* ZeroUpkeep: free buildings in the Trade Advisor */
    X_SUPPORT_LIST = 11,    /* SupportScroll: supported units list scrolls, total */
    X_SUPPORT_SORT = 16,    /* SupportSort: and is sorted */
    X_CELEBRATING = 22,     /* AttitudeColors: Attitude Advisor */
    X_CITY_ATTITUDE = 23,   /* CityAttitude: colours under the citizens */
    X_RADIO_KEYS = 24,      /* RadioHotkeys */
    X_QUICKINFO = 25,       /* Quickinfo: Ctrl over a city or unit */
    X_RESIZABLE = 26,       /* resizable advisors and lists (not done, docs/STATUS.md) */
    X_CAPTION = 27,         /* taller caption area (not done) */
    X_UNIT_LIST_KEYS = 28,  /* UnitListKeys: keys and page size in that list */
    X_PROD_CANCEL = 29,     /* ProductionCancel */
    X_PROD_COST = 30,       /* ProductionCost: cost and upkeep in the list */
    X_CITY_STATUS = 31,     /* CityStatus: total, icons, sorting, click */
    X_MASS_SPECIALISTS = 32,/* MassSpecialists: Shift+wheel / Shift+click */
    X_SUPPRESS_POPUPS = 33, /* SuppressPopups: listed popups shown on the map */
    X_COLOR = 35,           /* ColorCorrection (Gamma, Brightness, Contrast) */
    X_FOREIGN_GOLD = 37,    /* ForeignGold: coins in the Foreign Minister */
    X_ARRANGE = 38,         /* ArrangeWindowsSL: two more Arrange Windows */
    X_PATH_LINES = 39,      /* PathLines: trade roads and unit paths with Shift */
    X_SENTRY_GREY = 41,     /* SentryHalftone */
    X_ZOOM_1 = 43,          /* DefaultZoom1: new games at zoom 1:1 */
    /* Rule changes (extra_rule): off unless switched on in CIV.INI */
    X_WORKER_HANDOVER = 17, /* WorkerHandover: the worker already there stops */
    X_GOTO_ZOC = 18,        /* GotoNoZocStop: Go To does not stop next to foreign units */
    X_ROTATION = 19,        /* UnitRotation: Wait flags per stack, search from the last start */
    X_MASS_MOVE = 36,       /* MassMove: Shift+right click moves the stack's units of the type */
    X_STACK_SENTRY = 42,    /* StackSentry: U sentries the stack's land units too */
    X_RETIREMENT = 50,      /* RetirementYear= (and RetirementWarningYear=) */
    X_POP_LIMIT = 51,       /* PopulationLimit= (in 10,000s; the original's 32000) */
    X_GOLD_LIMIT = 52,      /* GoldLimit= (the original's 30000) */
    X_MAP_TILES = 53,       /* MapTilesLimit= (a custom map's tiles; the original's 10000) */
    X_COUNT = 64
};

EXTRAS_C int plat_extras;                       /* src/platform/main.c */
EXTRAS_C int plat_scale;                        /* src/platform/main.c: pixels per desktop pixel */
EXTRAS_C int plat_fullscreen;                   /* src/platform/main.c: full screen, not a window */
EXTRAS_C uint64_t extras_enabled;     /* src/game/extras.cpp: bit per item */
static inline int extra_ui(int item) { return plat_extras && (int)((extras_enabled >> item) & 1); }
EXTRAS_C uint64_t extras_rules;       /* src/game/extras.cpp: bit per rule change */

EXTRAS_C int plat_color_lut_on;                 /* src/platform/gdi.c: X35, the screen's */
EXTRAS_C unsigned char plat_color_lut[256];     /* colours through this table */

EXTRAS_C HWND plat_window_at_cursor(POINT *pt);  /* src/platform/message.c */
EXTRAS_C int plat_profile_string(const char *file, const char *section, const char *key, char *out, int n);

/* src/game/extras.cpp */
EXTRAS_C void extras_load(void);                /* the settings, once at start */
EXTRAS_C int extras_input(void *msg);           /* a mouse message (MSG *) before dispatch: 1 if used */
EXTRAS_C int extras_setting(const char *key, int def);   /* [OpenCiv2] key=number */
EXTRAS_C const char *extras_text(const char *key);   /* an addition's text (translatable) */
EXTRAS_C int extras_text_translated(const char *key);  /* whether a translation gives it */
/* each of the additions' texts: add("openciv2.<key>", English) (lang.cpp's export) */
EXTRAS_C void extras_texts_each(void (*add)(const char *key, const char *english));
EXTRAS_C void extras_options_dialog(void);      /* Game > OpenCiv2 Options */

/* In a dialog list row's text (X30, X37): EXTRAS_ICON then 'g' (a gold coin),
   's' (a shield) or 'f' (food): that icon. */
#define EXTRAS_ICON '\x1b'
#define EXTRAS_ICON_S "\x1b"

#ifdef __cplusplus
/* A rule change (CIV2UIA's 17, 18, 19, 36, 42 and the limits): off by
   default, switched on in CIV.INI (its key =1, or the limit); as
   bugfix_state(), also off in network games, where every player must
   compute the same game.  Each one: if (extra_rule(X_...)) <changed> else
   <original>, marked X<n>. */
static inline int extra_rule(int item)
{
    return plat_extras && (int)((extras_rules >> item) & 1) && game.multiplayer < 3;
}

/* src/game/extras.cpp: helpers of the additions in the game's code */
int extras_attitude_color(int city, int normal);
int extras_city_food_surplus(int city);
void extras_draw_food_trend(iptr surface, int city, int x, int y);
struct Sprite *extras_icon(int code);
int extras_rich_width(iptr font, const char *text);
void extras_append_shield_cost(int cost);
void extras_set_panel_size(int size);
void extras_radio_hotkeys(RadioItem *items, int count);
int extras_city_support_wheel(int x, int y, int notches);
void extras_draw_supported_units(struct CityWindow *w, int blit);
void extras_city_window_controls_destroyed(void);
void extras_city_status_draw(void);
void extras_city_status_click(int x, int y);
void extras_note_dialog(struct Dialog *d, const char *file, const char *section);
int extras_popup_suppressed(struct Dialog *d);
void extras_draw_map_messages(iptr surface, int blit);
i32 extras_unit_list_draw(struct Dialog *dialog, iptr surface, i32 unit, i32 flags, i32 x, i32 y, i32 row_height);
int extras_hover_input(const MSG *m, HWND under, POINT pt);
int extras_path_input(const MSG *m, HWND under, POINT pt);
int extras_panel_width(int width);
void extras_draw_work_counter(iptr surface, int unit, int px, int py, int zoom);
void extras_draw_grey_sprite(struct Sprite *sprite, iptr surface, int x, int y);
/* the rule changes' */
int extras_rotation_origin(int unit, int *x, int *y);
void extras_rotation_found(int unit);
void extras_rotation_activated(int unit);
int extras_mass_move(int x, int y);
int extras_stack_sentry(void);
int extras_retirement_year(void);
int extras_year_reached(int year);
int extras_retirement_warning_year(void);
int extras_population_limit(void);
int extras_gold_limit(void);
int extras_map_tiles_limit(void);
#endif

#endif
