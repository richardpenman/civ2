/*
 * The opening window: shown before the game starts, in a window of its own,
 * where the player chooses how to play (docs/EXTRAS.md, "The opening
 * window"); the choices are kept in CIV.INI's [OpenCiv2] section for the
 * next time.  Its pages:
 *
 *   Game       Mode: Standard (the original's bugs fixed, the interface
 *              additions) or Classic (the original game, as CIV2_CLASSIC=1);
 *              HD graphics and their density (docs/HD-GRAPHICS.md), the art
 *              downloaded from here where it is missing (src/host/
 *              host_launcher.c); full screen.
 *   Rules      the rule changes and limits (EXTRAS.md "Rule changes").
 *   Interface  the interface additions, the colour correction and the
 *              messages shown on the map.
 *
 * Shown at start unless "Don't show this window again" was chosen
 * (Launcher=0); then --launcher, or Shift held down while the game starts,
 * shows it.  Play starts the game, Quit (or closing the window) ends.  Made
 * of the platform's own windows and drawing, as the file dialog
 * (filedlg.c), so it looks the same everywhere; drawn in the colours of the
 * game's own dialogs.
 *
 * Never in replays or the unit tests (nothing they compare changes), but
 * with CIV2_TEST_LAUNCHER=1 in a replay (harness/extras/x-launcher.txt):
 * then it is not shown in a window of its own, nothing is written to
 * CIV.INI and nothing downloaded; the choices hold for that run
 * (launcher_setting).
 */
#include "plat.h"
#include "hd.h"
#include "launcher.h"
#include "../host/host.h"

#define LW 600                  /* its size, in desktop pixels */
#define LH 468

/* ---- the settings ---- */

enum { R_GOTO, R_MASS_MOVE, R_HANDOVER, R_ROTATION, R_SENTRY, NRULES };
enum { L_RETIRE, L_WARN, L_POP, L_GOLD, L_MAP, NLIMITS };
enum { C_GAMMA, C_BRIGHT, C_CONTRAST, C_SUPPRESS, NCOLOR };

/* The rule changes (src/game/extras.cpp: rule_keys), with a line on each. */
static const struct { const char *key, *label, *note; } rules[NRULES] = {
    {"GotoNoZocStop", "Go To past foreign units",
     "A unit on Go To keeps going next to foreign units (zones of control still stop it)."},
    {"MassMove", "Shift+right click moves the type",
     "The active unit goes there with its square's idle units of the same type."},
    {"WorkerHandover", "Workers hand over their work",
     "A settler joining another's terrain work takes it over; the first is freed."},
    {"UnitRotation", "Units of a stack in turn",
     "The other units of a square come next, then those nearest the last one."},
    {"StackSentry", "U sentries the stack",
     "With no land unit on sentry, U puts the stack's idle land units on sentry."},
};

/* The limits: the original's value (shown while off) and the range. */
static const struct { const char *key, *label; int original, lo, hi; } limits[NLIMITS] = {
    {"RetirementYear", "Retirement year", 2020, 1, 9999},
    {"RetirementWarningYear", "Its warning", 2000, 0, 9998},
    {"PopulationLimit", "Population (10,000s)", 32000, 32000, 3275800},
    {"GoldLimit", "Gold", 30000, 30000, 1000000},
    {"MapTilesLimit", "Custom map squares", 10000, 10000, 32767},
};

/* The interface additions (src/game/extras.cpp: extra_keys), in its order. */
static const struct { const char *key, *label; } extras[] = {
    {"MouseWheel", "Mouse wheel and button"}, {"UnitListScroll", "Unit list past 9 units"},
    {"UnitListKeys", "Keys in that unit list"}, {"WorkCounter", "Settlers' work counter"},
    {"GameTurn", "Turn in the status panel"}, {"ResearchNumbers", "Research figures"},
    {"ZeroUpkeep", "Free buildings listed"}, {"SupportScroll", "Supported units: scroll"},
    {"SupportSort", "Supported units sorted"}, {"AttitudeColors", "Attitude Advisor colours"},
    {"CityAttitude", "City window attitude"}, {"RadioHotkeys", "Hotkeys for options"},
    {"Quickinfo", "Quick information with Ctrl"}, {"ProductionCancel", "Cancel in Change Production"},
    {"ProductionCost", "Costs in Change Production"}, {"CityStatus", "City Status: sort, change"},
    {"MassSpecialists", "Shift on specialists"}, {"SuppressPopups", "Messages on the map"},
    {"ColorCorrection", "Colour correction"}, {"ForeignGold", "Gold in Foreign Minister"},
    {"ArrangeWindowsSL", "Arrange Windows S and L"}, {"PathLines", "Path lines with Shift"},
    {"SentryHalftone", "Sentries in grey shades"}, {"DefaultZoom1", "New games at zoom 1:1"},
};
#define NEXTRAS ((int)(sizeof extras / sizeof extras[0]))

static const struct { const char *key, *label; int def; } colours[NCOLOR] = {
    {"Gamma", "Gamma (%)", 100}, {"Brightness", "Brightness", 0}, {"Contrast", "Contrast", 0},
    {"SuppressList", "Messages on the map", 0},
};

typedef struct {
    int classic, hd, density, fullscreen, dont_show;
    int rule[NRULES], extra[NEXTRAS];
    char limit[NLIMITS][16], colour[NCOLOR][256];
} Choices;

static Choices now, was;            /* as chosen; as read */

/* The choices set in a test run (CIV2_TEST_LAUNCHER): the game reads them
   instead of CIV.INI (launcher_setting). */
static struct {
    char key[32], value[256];
} set[64];
static int set_count;

int launcher_setting(const char *key, char *out, int n)
{
    int i;
    for (i = 0; i < set_count; i++)
        if (!strcmp(set[i].key, key)) {
            snprintf(out, (size_t)n, "%s", set[i].value);
            return 1;
        }
    return 0;
}

static void get(const char *key, char *out, int n)
{
    out[0] = 0;
    if (!launcher_setting(key, out, n) && !plat_replay)
        plat_profile_string("CIV.INI", "OpenCiv2", key, out, n);
}

static int get_int(const char *key, int def)
{
    char v[64];
    get(key, v, sizeof v);
    return v[0] ? atoi(v) : def;
}

static void put(const char *key, const char *value)
{
    int i;
    if (!plat_replay) {
        WritePrivateProfileStringA("OpenCiv2", key, value, "CIV.INI");
        return;
    }
    for (i = 0; i < set_count && strcmp(set[i].key, key); i++)
        ;
    if (i == (int)(sizeof set / sizeof set[0]))
        return;
    snprintf(set[i].key, sizeof set[i].key, "%s", key);
    snprintf(set[i].value, sizeof set[i].value, "%s", value);
    if (i == set_count)
        set_count++;
}

static void put_int(const char *key, int value)
{
    char v[16];
    snprintf(v, sizeof v, "%d", value);
    put(key, v);
}

static void read_choices(Choices *c)
{
    char v[64];
    int i, year;
    memset(c, 0, sizeof *c);
    get("Mode", v, sizeof v);
    c->classic = !strcasecmp(v, "Classic");
    get("Graphics", v, sizeof v);
    c->hd = !strcasecmp(v, "HD");
    c->density = get_int("GraphicsDensity", 0);
    if (c->density < 0 || c->density > 4)
        c->density = 0;
    c->fullscreen = get_int("FullScreen", 0) != 0;
    c->dont_show = get_int("Launcher", 1) == 0;
    for (i = 0; i < NRULES; i++)
        c->rule[i] = get_int(rules[i].key, 0) != 0;
    for (i = 0; i < NEXTRAS; i++)
        c->extra[i] = get_int(extras[i].key, 1) != 0;
    /* the limits in force: the original's while off (0 or none) */
    year = get_int("RetirementYear", 0);
    if (year <= 0)
        year = limits[L_RETIRE].original;
    snprintf(c->limit[L_RETIRE], 16, "%d", year);
    snprintf(c->limit[L_WARN], 16, "%d",
             get_int("RetirementYear", 0) > 0 ? get_int("RetirementWarningYear", year - 20) : limits[L_WARN].original);
    for (i = L_POP; i < NLIMITS; i++) {
        int n = get_int(limits[i].key, 0);
        snprintf(c->limit[i], 16, "%d", n > 0 ? n : limits[i].original);
    }
    for (i = 0; i < C_SUPPRESS; i++)
        snprintf(c->colour[i], 256, "%d", get_int(colours[i].key, colours[i].def));
    get("SuppressList", c->colour[C_SUPPRESS], 256);
}

/* What changed written (a limit at the original's value is off: =0; a
   warning 20 years before the retirement moves with it). */
static void write_choices(const Choices *c, const Choices *old)
{
    int i;
    if (c->classic != old->classic)
        put("Mode", c->classic ? "Classic" : "Standard");
    if (c->hd != old->hd)
        put("Graphics", c->hd ? "HD" : "Standard");
    if (c->density != old->density)
        put_int("GraphicsDensity", c->density);
    if (c->fullscreen != old->fullscreen)
        put_int("FullScreen", c->fullscreen);
    if (c->dont_show != old->dont_show)
        put_int("Launcher", !c->dont_show);
    for (i = 0; i < NRULES; i++)
        if (c->rule[i] != old->rule[i])
            put_int(rules[i].key, c->rule[i]);
    for (i = 0; i < NEXTRAS; i++)
        if (c->extra[i] != old->extra[i])
            put_int(extras[i].key, c->extra[i]);
    if (strcmp(c->limit[L_RETIRE], old->limit[L_RETIRE]) || strcmp(c->limit[L_WARN], old->limit[L_WARN])) {
        int year = atoi(c->limit[L_RETIRE]), warn = atoi(c->limit[L_WARN]);
        if (!strcmp(c->limit[L_WARN], old->limit[L_WARN]) && atoi(old->limit[L_WARN]) == atoi(old->limit[L_RETIRE]) - 20)
            warn = year - 20;       /* a warning 20 years before moves with the year */
        if (year == limits[L_RETIRE].original && warn == limits[L_WARN].original)
            put_int("RetirementYear", 0);
        else {
            put_int("RetirementYear", year < 1 ? 1 : year > 9999 ? 9999 : year);
            put_int("RetirementWarningYear", warn < 0 ? 0 : warn);
        }
    }
    for (i = L_POP; i < NLIMITS; i++)
        if (strcmp(c->limit[i], old->limit[i])) {
            int v = atoi(c->limit[i]);
            put_int(limits[i].key, v <= limits[i].original ? 0 : v > limits[i].hi ? limits[i].hi : v);
        }
    for (i = 0; i < NCOLOR; i++)
        if (strcmp(c->colour[i], old->colour[i])) {
            if (i == C_SUPPRESS)
                put(colours[i].key, c->colour[i]);
            else
                put_int(colours[i].key, atoi(c->colour[i]));
        }
}

/* ---- whether it is shown ---- */

int launcher_wanted(void)
{
    const char *t = getenv("CIV2_TEST_LAUNCHER"), *arg = getenv("CIV2_LAUNCHER_ARG");
    int host;
    if (plat_replay)
        return t && *t && *t != '0';
    host = host_launcher();
    if (host < 0)
        return 0;               /* (the browser: its own set-up page) */
    return host > 0 || (arg && *arg && *arg != '0') || get_int("Launcher", 1) != 0;
}

void launcher_size(int *w, int *h)
{
    *w = LW;
    *h = LH;
}

/* ---- the window ---- */

/* Colours: the game's own dialogs' (its main menu). */
#define C_DESK RGB(143, 123, 99)       /* the brown behind the menu */
#define C_FACE RGB(191, 191, 191)      /* a dialog's face */
#define C_LIGHT RGB(223, 223, 223)     /* its raised edges */
#define C_WHITE RGB(245, 245, 245)
#define C_SHADE RGB(123, 123, 123)
#define C_DARK RGB(67, 67, 67)
#define C_STONE RGB(152, 152, 152)     /* the list of options (the stone) */
#define C_TEXT RGB(32, 32, 32)
#define C_DIM RGB(104, 104, 104)       /* disabled, notes */
#define C_RED RGB(128, 16, 16)

enum { T_TAB, T_CHECK, T_RADIO, T_BUTTON, T_HEAD, T_TEXT, T_FIELD };
enum { PAGE_ALL, PAGE_GAME, PAGE_RULES, PAGE_UI };
enum { B_PLAY = 1, B_QUIT, B_DOWNLOAD, B_STOP };

typedef struct {
    int type, page;
    RECT r;
    const char *text, *note;
    int *value;                 /* checks: 0/1; radios: == choice */
    int choice;                 /* radios: the value; tabs: the page; buttons: B_... */
    int small;                  /* in the smaller font */
    char *field;                /* fields: the text (16 or 256 bytes) */
    int field_size;
    HWND edit;
} Item;

static struct {
    HWND dlg;
    Item items[128];
    int n, page, done, play;
    HFONT title, head, text, note, button;
    int installed;              /* the HD art is there */
    int fetching, fetch_state, fetch_info[3];
    char fetch_msg[512];
    char override_text[128];    /* CIV2_CLASSIC or --hd/CIV2_HD set: they win */
} L;

static HFONT font(const char *face, int px, int bold, int italic)
{
    LOGFONTA lf;
    memset(&lf, 0, sizeof lf);
    lf.lfHeight = -px;
    lf.lfWeight = bold ? 700 : 400;
    lf.lfItalic = (BYTE)italic;
    snprintf(lf.lfFaceName, sizeof lf.lfFaceName, "%s", face);
    return CreateFontIndirectA(&lf);
}

static void fill(HDC hdc, int x, int y, int w, int h, COLORREF c)
{
    RECT r;
    HBRUSH b = CreateSolidBrush(c);
    SetRect(&r, x, y, x + w, y + h);
    FillRect(hdc, &r, b);
    DeleteObject(b);
}

/* A frame as the game's: two pixels, raised (light above) or sunken. */
static void bevel(HDC hdc, const RECT *r, int raised)
{
    COLORREF a1 = raised ? C_WHITE : C_SHADE, a2 = raised ? C_LIGHT : C_DARK;
    COLORREF b1 = raised ? C_DARK : C_WHITE, b2 = raised ? C_SHADE : C_LIGHT;
    int w = r->right - r->left, h = r->bottom - r->top;
    fill(hdc, r->left, r->top, w, 1, a1);
    fill(hdc, r->left, r->top, 1, h, a1);
    fill(hdc, r->left + 1, r->top + 1, w - 2, 1, a2);
    fill(hdc, r->left + 1, r->top + 1, 1, h - 2, a2);
    fill(hdc, r->left, r->bottom - 1, w, 1, b1);
    fill(hdc, r->right - 1, r->top, 1, h, b1);
    fill(hdc, r->left + 1, r->bottom - 2, w - 2, 1, b2);
    fill(hdc, r->right - 2, r->top + 1, 1, h - 2, b2);
}

/* Text, embossed as the game's titles (a light copy a pixel down and right). */
static void text(HDC hdc, HFONT f, const char *s, RECT r, COLORREF c, UINT flags, int emboss)
{
    SelectObject(hdc, f);
    SetBkMode(hdc, TRANSPARENT);
    if (emboss) {
        RECT e = r;
        OffsetRect(&e, 1, 1);
        SetTextColor(hdc, C_WHITE);
        DrawTextA(hdc, s, -1, &e, flags | DT_NOPREFIX);
    }
    SetTextColor(hdc, c);
    DrawTextA(hdc, s, -1, &r, flags | DT_NOPREFIX);
}

/* A disc: radius r about (cx, cy). */
static void disc(HDC hdc, int cx, int cy, int r, COLORREF c)
{
    int dy;
    for (dy = -r; dy <= r; dy++) {
        int dx = 0;
        while ((dx + 1) * (dx + 1) + dy * dy <= r * r + r)
            dx++;
        fill(hdc, cx - dx, cy + dy, 2 * dx + 1, 1, c);
    }
}

static int is_on(const Item *it)
{
    if (it->type == T_CHECK)
        return *it->value != 0;
    if (it->type == T_RADIO)
        return *it->value == it->choice;
    return 0;
}

static int fetch_running(void) { return L.fetching && L.fetch_state == HOST_HD_RUNNING; }

static int enabled(const Item *it)
{
    if (it->type == T_TAB)
        return it->choice == PAGE_GAME || !now.classic;
    if (it->value == &now.hd)
        return !now.classic && !fetch_running();
    if (it->value == &now.density)
        return !now.classic && now.hd;
    if (it->type == T_BUTTON && it->choice == B_PLAY)
        return !fetch_running();
    if (it->type == T_BUTTON && it->choice == B_DOWNLOAD)
        return !now.classic;
    return 1;
}

static int visible(const Item *it)
{
    if (it->page != PAGE_ALL && it->page != L.page)
        return 0;
    if (it->type == T_BUTTON && it->choice == B_DOWNLOAD)
        return !L.installed && !fetch_running();
    if (it->type == T_BUTTON && it->choice == B_STOP)
        return fetch_running();
    return 1;
}

static void draw_item(HDC hdc, const Item *it)
{
    int on = enabled(it);
    COLORREF tc = on ? C_TEXT : C_DIM;
    RECT r = it->r;
    switch (it->type) {
    case T_TAB: {
        int cur = it->choice == L.page;
        RECT t = r;
        if (cur)
            t.bottom += 3;      /* (joined to the page) */
        fill(hdc, t.left, t.top, t.right - t.left, t.bottom - t.top, cur ? C_FACE : C_STONE);
        fill(hdc, t.left, t.top, t.right - t.left, 1, C_WHITE);
        fill(hdc, t.left, t.top, 1, t.bottom - t.top - (cur ? 3 : 0), C_WHITE);
        fill(hdc, t.right - 1, t.top, 1, t.bottom - t.top - (cur ? 3 : 0), C_DARK);
        fill(hdc, t.right - 2, t.top + 1, 1, t.bottom - t.top - 1 - (cur ? 3 : 0), C_SHADE);
        text(hdc, L.head, it->text, r, tc, DT_SINGLELINE | DT_CENTER | DT_VCENTER, on);
        break;
    }
    case T_CHECK: case T_RADIO: {
        int y = r.top + 2, x = r.left;
        RECT tr = r;
        if (it->type == T_CHECK) {
            RECT box;
            SetRect(&box, x, y, x + 15, y + 15);
            fill(hdc, x, y, 15, 15, on ? C_WHITE : C_LIGHT);
            bevel(hdc, &box, 0);
            if (is_on(it)) {
                int k;
                /* a check mark, two pixels thick */
                for (k = 0; k < 3; k++)
                    fill(hdc, x + 4 + k, y + 7 + k, 1, 2, on ? C_TEXT : C_DIM);
                for (k = 0; k < 5; k++)
                    fill(hdc, x + 7 + k, y + 8 - k, 1, 2, on ? C_TEXT : C_DIM);
            }
        } else {
            disc(hdc, x + 7, y + 7, 7, C_SHADE);
            disc(hdc, x + 8, y + 8, 6, C_WHITE);
            disc(hdc, x + 7, y + 7, 6, on ? RGB(232, 232, 232) : C_LIGHT);
            if (is_on(it))
                disc(hdc, x + 7, y + 7, 3, on ? C_TEXT : C_DIM);
        }
        tr.left += 22;
        tr.bottom = tr.top + 19;
        text(hdc, it->small ? L.note : L.text, it->text, tr, tc, DT_SINGLELINE | DT_VCENTER, 0);
        if (it->note) {
            tr = r;
            tr.left += 22;
            tr.top += 19;
            text(hdc, L.note, it->note, tr, C_DIM, DT_WORDBREAK, 0);
        }
        break;
    }
    case T_BUTTON: {
        RECT t = r;
        fill(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, C_FACE);
        bevel(hdc, &r, 1);
        text(hdc, L.button, it->text, t, tc, DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0);
        break;
    }
    case T_HEAD:
        text(hdc, L.head, it->text, r, C_TEXT, DT_SINGLELINE | DT_VCENTER, 1);
        break;
    case T_TEXT:
        text(hdc, it->small ? L.note : L.text, it->text, r, it->choice ? C_RED : C_TEXT, DT_WORDBREAK, 0);
        break;
    case T_FIELD: {
        RECT tr = r;
        tr.right = tr.left + it->choice;
        text(hdc, L.text, it->text, tr, tc, DT_SINGLELINE | DT_VCENTER, 0);
        break;
    }
    }
}

/* The HD art's state, in the line under the HD graphics' choice. */
static void draw_status(HDC hdc, RECT r)
{
    char s[600];
    COLORREF c = C_TEXT;
    if (fetch_running()) {
        static const char *const doing[3] = {"Downloading the art", "Checking it", "Installing it"};
        int ph = L.fetch_info[0] < 0 || L.fetch_info[0] > 2 ? 0 : L.fetch_info[0];
        int done = L.fetch_info[1], total = L.fetch_info[2] > 0 ? L.fetch_info[2] : 1;
        RECT bar;
        if (done > total)
            done = total;
        snprintf(s, sizeof s, "%s: %d of %d MB", doing[ph], (int)(done * 1024LL / 1000000),
                 (int)(total * 1024LL / 1000000));
        SetRect(&bar, r.left, r.top + 20, r.left + 300, r.top + 34);
        fill(hdc, bar.left, bar.top, 300, 14, C_STONE);
        fill(hdc, bar.left + 2, bar.top + 2, (int)((long long)296 * done / total), 10, RGB(48, 64, 128));
        bevel(hdc, &bar, 0);
    } else if (L.installed)
        snprintf(s, sizeof s, "The art is installed.");
    else if (L.fetching && L.fetch_state == HOST_HD_FAILED) {
        snprintf(s, sizeof s, "%s HD graphics stay off.", L.fetch_msg[0] ? L.fetch_msg : "The download failed.");
        c = C_RED;
    } else
        snprintf(s, sizeof s, "HD graphics need the art: %d MB, downloaded once from openciv2-assets on GitHub.",
                 (int)((L.fetch_info[2] > 0 ? L.fetch_info[2] : 228595) * 1024LL / 1000000));
    r.right -= fetch_running() ? 0 : 110;
    text(hdc, L.note, s, r, c, fetch_running() ? DT_SINGLELINE : DT_WORDBREAK, 0);
}

static RECT status_rect;

static void paint(HDC hdc)
{
    RECT r;
    int i;
    fill(hdc, 0, 0, LW, LH, C_DESK);
    /* the title */
    SetRect(&r, 20, 8, LW - 20, 44);
    text(hdc, L.title, "OpenCiv2", r, C_TEXT, DT_SINGLELINE | DT_VCENTER, 1);
    SetRect(&r, 170, 8, LW - 20, 44);
    text(hdc, L.head, "Civilization II Multiplayer Gold Edition", r, RGB(236, 226, 200),
         DT_SINGLELINE | DT_VCENTER | DT_RIGHT, 0);
    /* the page */
    SetRect(&r, 14, 80, LW - 14, LH - 52);
    fill(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, C_FACE);
    bevel(hdc, &r, 1);
    for (i = 0; i < L.n; i++)
        if (visible(&L.items[i]))
            draw_item(hdc, &L.items[i]);
    if (L.page == PAGE_GAME)
        draw_status(hdc, status_rect);
}

/* ---- the items ---- */

static Item *add(int type, int page, int x, int y, int w, int h, const char *s)
{
    Item *it = &L.items[L.n < 127 ? L.n++ : L.n];
    memset(it, 0, sizeof *it);
    it->type = type;
    it->page = page;
    SetRect(&it->r, x, y, x + w, y + h);
    it->text = s;
    return it;
}

static void add_check(int page, int x, int y, int w, const char *s, const char *note, int *value)
{
    Item *it = add(T_CHECK, page, x, y, w, note ? 36 : 19, s);
    it->note = note;
    it->value = value;
}

static void add_radio(int page, int x, int y, int w, const char *s, const char *note, int *value, int choice)
{
    Item *it = add(T_RADIO, page, x, y, w, note ? 36 : 19, s);
    it->note = note;
    it->value = value;
    it->choice = choice;
}

static void add_field(int page, int x, int y, int label_w, int field_w, const char *s, char *field, int size)
{
    Item *it = add(T_FIELD, page, x, y, label_w + field_w, 22, s);
    it->choice = label_w;
    it->field = field;
    it->field_size = size;
    it->edit = CreateWindowExA(0, "EDIT", field, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, x + label_w, y,
                               field_w, 22, L.dlg, 0, 0, NULL);
    SendMessageA(it->edit, WM_SETFONT, (WPARAM)L.text, FALSE);
    SendMessageA(it->edit, EM_LIMITTEXT, (WPARAM)(size - 1), 0);
}

static void add_button(int page, int x, int y, int w, const char *s, int id)
{
    add(T_BUTTON, page, x, y, w, 28, s)->choice = id;
}

static void build(void)
{
    int i, x = 30, y;
    Item *it;
    L.n = 0;
    add(T_TAB, PAGE_ALL, 14, 52, 120, 28, "Game")->choice = PAGE_GAME;
    add(T_TAB, PAGE_ALL, 136, 52, 120, 28, "Rules")->choice = PAGE_RULES;
    add(T_TAB, PAGE_ALL, 258, 52, 120, 28, "Interface")->choice = PAGE_UI;

    /* Game */
    y = 92;
    add(T_HEAD, PAGE_GAME, x, y, 300, 22, "Mode");
    add_radio(PAGE_GAME, x, y + 26, 520, "Standard",
              "The original's bugs fixed, with OpenCiv2's interface additions; rule changes as chosen.",
              &now.classic, 0);
    add_radio(PAGE_GAME, x, y + 66, 520, "Classic",
              "The original game as it was: its bugs, its look, none of OpenCiv2's additions.", &now.classic, 1);
    y += 112;
    add(T_HEAD, PAGE_GAME, x, y, 300, 22, "Graphics");
    add_check(PAGE_GAME, x, y + 26, 520, "HD graphics",
              "New art for the map, at the screen's full resolution.", &now.hd);
    add(T_TEXT, PAGE_GAME, x + 22, y + 68, 70, 20, "Density:")->small = 1;
    for (i = 0; i <= 4; i++) {
        static const char *const names[5] = {"Auto", "1", "2", "3", "4"};
        it = add(T_RADIO, PAGE_GAME, x + 84 + (i ? 60 + (i - 1) * 46 : 0), y + 66, i ? 40 : 56, 19, names[i]);
        it->value = &now.density;
        it->choice = i;
        it->small = 1;
    }
    SetRect(&status_rect, x + 22, y + 92, LW - 30, y + 126);
    add_button(PAGE_GAME, LW - 130, y + 92, 100, "Download", B_DOWNLOAD);
    add_button(PAGE_GAME, x + 340, y + 106, 80, "Stop", B_STOP);
    y += 136;
    add_check(PAGE_GAME, x, y, 520, "Full screen", "Instead of a window filling the screen.", &now.fullscreen);
    if (L.override_text[0])
        add(T_TEXT, PAGE_GAME, x, y + 42, LW - 60, 18, L.override_text)->small = 1;
    add(T_TEXT, PAGE_GAME, x, LH - 74, LW - 60, 18,
        "The choices are kept in CIV.INI, in the game's folder, under [OpenCiv2].")->small = 1;

    /* Rules */
    y = 90;
    add(T_TEXT, PAGE_RULES, x, y, LW - 60, 32,
        "Changes to the original's rules, for the human players' units; the original's rules in network games.")->small = 1;
    y += 30;
    for (i = 0; i < NRULES; i++, y += 38)
        add_check(PAGE_RULES, x, y, LW - 60, rules[i].label, rules[i].note, &now.rule[i]);
    add(T_HEAD, PAGE_RULES, x, y + 2, 300, 22, "Limits");
    add(T_TEXT, PAGE_RULES, x + 80, y + 5, LW - 140, 20, "(at the original's values: as the original)")->small = 1;
    y += 28;
    for (i = 0; i < NLIMITS; i++)
        add_field(PAGE_RULES, i < 3 ? x : x + 290, y + (i < 3 ? i : i - 3) * 26, 170, 80, limits[i].label,
                  now.limit[i], 16);

    /* Interface */
    y = 90;
    add(T_TEXT, PAGE_UI, x, y, LW - 60, 20,
        "OpenCiv2's additions to the interface (after FoxAhead's CIV2UIA): what you see and do.")->small = 1;
    y += 24;
    for (i = 0; i < NEXTRAS; i++)
        add_check(PAGE_UI, i < 12 ? x : x + 270, y + (i % 12) * 18, 260, extras[i].label, NULL, &now.extra[i]);
    y += 12 * 18 + 6;
    for (i = 0; i < C_SUPPRESS; i++)
        add_field(PAGE_UI, x + i * 180, y, i ? 80 : 76, 60, colours[i].label, now.colour[i], 16);
    add_field(PAGE_UI, x, y + 26, 180, 356, colours[C_SUPPRESS].label, now.colour[C_SUPPRESS], 256);
    add(T_TEXT, PAGE_UI, x + 180, y + 50, 360, 16,
        "GAME.TXT sections, e.g. HIDDENTERRAIN,CITYSIZE")->small = 1;

    /* always */
    add_check(PAGE_ALL, 18, LH - 40, 330, "Don't show this window again",
              NULL, &now.dont_show);
    add(T_TEXT, PAGE_ALL, 40, LH - 21, 300, 18, "(--launcher or Shift at start shows it)")->small = 1;
    add_button(PAGE_ALL, LW - 228, LH - 42, 100, "Quit", B_QUIT);
    add_button(PAGE_ALL, LW - 116, LH - 42, 100, "Play", B_PLAY);
}

static void show_page(int page)
{
    int i;
    L.page = page;
    for (i = 0; i < L.n; i++)
        if (L.items[i].edit) {
            ShowWindow(L.items[i].edit, L.items[i].page == page ? SW_SHOW : SW_HIDE);
            EnableWindow(L.items[i].edit, !now.classic);
        }
    InvalidateRect(L.dlg, NULL, TRUE);
}

static void fields_read(void)
{
    int i;
    for (i = 0; i < L.n; i++)
        if (L.items[i].edit)
            GetWindowTextA(L.items[i].edit, L.items[i].field, L.items[i].field_size);
}

/* ---- the HD art ---- */

static void fetch_poll(void)
{
    L.fetch_state = host_hd_fetch(HOST_HD_POLL, L.fetch_info, L.fetch_msg, sizeof L.fetch_msg);
    if (L.fetch_state != HOST_HD_RUNNING) {
        KillTimer(L.dlg, 1);
        if (L.fetch_state == HOST_HD_DONE) {
            char path[1100];
            L.installed = hd_pack_path(path, sizeof path);
            now.hd = L.installed;
        } else
            now.hd = 0;
    }
    InvalidateRect(L.dlg, NULL, TRUE);
}

static void fetch_start(void)
{
    if (plat_replay)
        return;                 /* (tests: nothing downloaded) */
    L.fetching = 1;
    L.fetch_state = host_hd_fetch(HOST_HD_START, L.fetch_info, L.fetch_msg, sizeof L.fetch_msg);
    SetTimer(L.dlg, 1, 250, NULL);
    fetch_poll();
}

/* ---- input ---- */

static void click(int x, int y)
{
    POINT p;
    int i;
    p.x = x;
    p.y = y;
    for (i = 0; i < L.n; i++) {
        Item *it = &L.items[i];
        if (!visible(it) || !enabled(it) || !PtInRect(&it->r, p) || it->type == T_HEAD || it->type == T_TEXT ||
            it->type == T_FIELD)
            continue;
        switch (it->type) {
        case T_TAB:
            show_page(it->choice);
            return;
        case T_CHECK:
            *it->value = !*it->value;
            if (it->value == &now.hd && now.hd && !L.installed) {
                now.hd = 0;             /* (on once the art is there) */
                fetch_start();
            }
            break;
        case T_RADIO:
            *it->value = it->choice;
            if (it->value == &now.classic)
                show_page(PAGE_GAME);   /* (the other pages: not in Classic) */
            break;
        case T_BUTTON:
            if (it->choice == B_PLAY)
                L.play = L.done = 1;
            else if (it->choice == B_QUIT)
                L.done = 1;
            else if (it->choice == B_DOWNLOAD)
                fetch_start();
            else if (it->choice == B_STOP) {
                host_hd_fetch(HOST_HD_CANCEL, L.fetch_info, L.fetch_msg, sizeof L.fetch_msg);
                L.fetching = 0;
                KillTimer(L.dlg, 1);
            }
            break;
        }
        InvalidateRect(L.dlg, NULL, TRUE);
        return;
    }
}

static LRESULT CALLBACK proc(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    switch (m) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(h, &ps);
        paint(hdc);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_LBUTTONDOWN:
        SetFocus(h);
        click((short)LOWORD(lp), (short)HIWORD(lp));
        return 0;
    case WM_TIMER:
        fetch_poll();
        return 0;
    case WM_CLOSE:
        L.done = 1;
        return 0;
    }
    return DefWindowProcA(h, m, wp, lp);
}

/* Tab: the page's next field (Shift+Tab: the one before). */
static void next_field(int back)
{
    HWND f = GetFocus(), list[32];
    int n = 0, i, k = -1;
    for (i = 0; i < L.n && n < 32; i++)
        if (L.items[i].edit && L.items[i].page == L.page && !now.classic)
            list[n++] = L.items[i].edit;
    if (!n)
        return;
    for (i = 0; i < n; i++)
        if (list[i] == f)
            k = i;
    k = k < 0 ? (back ? n - 1 : 0) : (k + (back ? n - 1 : 1)) % n;
    SetFocus(list[k]);
    SendMessageA(list[k], EM_SETSEL, 0, -1);
}

int launcher_run(void)
{
    static int registered;
    MSG msg;
    if (!registered) {
        WNDCLASSA wc;
        memset(&wc, 0, sizeof wc);
        wc.lpfnWndProc = proc;
        wc.lpszClassName = "OpenCiv2Launcher";
        wc.hCursor = LoadCursorA(0, (LPCSTR)IDC_ARROW);
        RegisterClassA(&wc);
        registered = 1;
    }
    memset(&L, 0, sizeof L);
    read_choices(&now);
    was = now;
    {
        char path[1100];
        L.installed = !plat_replay && hd_pack_path(path, sizeof path);
    }
    if (!L.installed)
        now.hd = 0;                     /* (shown off until the art is there) */
    if (!plat_replay)
        host_hd_fetch(HOST_HD_POLL, L.fetch_info, NULL, 0);    /* (the art's size) */
    {
        /* what the environment or the command line say wins (main.c, hd.c) */
        const char *c = getenv("CIV2_CLASSIC"), *a = getenv("CIV2_HD_ARG"), *e = getenv("CIV2_HD");
        snprintf(L.override_text, sizeof L.override_text, "%s%s%s%s",
                 c && *c ? "CIV2_CLASSIC" : "", c && *c && ((a && *a) || (e && *e)) ? " and " : "",
                 a && *a ? "--hd" : e && *e ? "CIV2_HD" : "",
                 (c && *c) || (a && *a) || (e && *e) ? " set: for this run they win over these choices." : "");
    }
    L.title = font("Times New Roman", 34, 1, 0);
    L.head = font("Times New Roman", 18, 1, 0);
    L.text = font("Times New Roman", 16, 0, 0);
    L.note = font("Arial", 12, 0, 0);
    L.button = font("Times New Roman", 17, 0, 0);
    L.dlg = CreateWindowExA(0, "OpenCiv2Launcher", "OpenCiv2", WS_POPUP, (screen_w - LW) / 2, (screen_h - LH) / 2,
                            LW, LH, 0, 0, 0, NULL);
    build();
    show_page(PAGE_GAME);
    ShowWindow(L.dlg, SW_SHOW);
    SetFocus(L.dlg);
    while (!L.done) {
        if (!GetMessageA(&msg, 0, 0, 0))
            break;
        if (msg.message == WM_KEYDOWN) {
            if (msg.wParam == VK_RETURN) {
                L.play = L.done = 1;
                continue;
            }
            if (msg.wParam == VK_ESCAPE) {
                L.done = 1;
                continue;
            }
            if (msg.wParam == VK_TAB) {
                next_field(GetKeyState(VK_SHIFT) < 0);
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    if (L.fetching && L.fetch_state == HOST_HD_RUNNING) {
        host_hd_fetch(HOST_HD_CANCEL, NULL, NULL, 0);
        now.hd = 0;
    }
    KillTimer(L.dlg, 1);
    if (L.play) {
        fields_read();
        if (!L.installed)
            now.hd = was.hd;        /* (as it was: shown off only for want of the art) */
        write_choices(&now, &was);
    }
    DestroyWindow(L.dlg);
    DeleteObject(L.title);
    DeleteObject(L.head);
    DeleteObject(L.text);
    DeleteObject(L.note);
    DeleteObject(L.button);
    return L.play;
}
