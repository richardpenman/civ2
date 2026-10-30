/*
 * (port) OpenCiv2's interface additions (src/include/extras.h,
 * docs/EXTRAS.md): their settings and the parts that are not in one of the
 * game's functions.  After CIV2UIA (FoxAhead's Civilization II UI
 * Additions), reimplemented from its description.
 */
#include <ctype.h>
#include <math.h>
#include "game.h"
#include "lang.h"

EXTERN_C int plat_replay;

uint64_t extras_enabled;
uint64_t extras_rules;

static void messages_expire(void);

struct KeyItem {
    int item;
    const char *key;
};

/* The items and their keys in CIV.INI's [OpenCiv2] section (the order of
   the options dialog). */
static const KeyItem extra_keys[] = {
    {X_WHEEL, "MouseWheel"}, {X_UNIT_LIST, "UnitListScroll"}, {X_UNIT_LIST_KEYS, "UnitListKeys"},
    {X_WORK_COUNTER, "WorkCounter"}, {X_GAME_TURN, "GameTurn"}, {X_RESEARCH, "ResearchNumbers"},
    {X_ZERO_UPKEEP, "ZeroUpkeep"}, {X_SUPPORT_LIST, "SupportScroll"}, {X_SUPPORT_SORT, "SupportSort"},
    {X_CELEBRATING, "AttitudeColors"}, {X_CITY_ATTITUDE, "CityAttitude"},
    {X_RADIO_KEYS, "RadioHotkeys"}, {X_QUICKINFO, "Quickinfo"},
    {X_PROD_CANCEL, "ProductionCancel"}, {X_PROD_COST, "ProductionCost"},
    {X_CITY_STATUS, "CityStatus"}, {X_MASS_SPECIALISTS, "MassSpecialists"},
    {X_SUPPRESS_POPUPS, "SuppressPopups"}, {X_COLOR, "ColorCorrection"},
    {X_FOREIGN_GOLD, "ForeignGold"}, {X_ARRANGE, "ArrangeWindowsSL"}, {X_PATH_LINES, "PathLines"},
    {X_SENTRY_GREY, "SentryHalftone"}, {X_ZOOM_1, "DefaultZoom1"},
};

/* The rule changes (extra_rule): off unless their key is set (=1; the
   limits: their value, 0 the original's; the options dialog shows the
   original's values while off). */
static const KeyItem rule_keys[] = {
    {X_WORKER_HANDOVER, "WorkerHandover"}, {X_GOTO_ZOC, "GotoNoZocStop"},
    {X_ROTATION, "UnitRotation"}, {X_MASS_MOVE, "MassMove"}, {X_STACK_SENTRY, "StackSentry"},
    {X_RETIREMENT, "RetirementYear"}, {X_POP_LIMIT, "PopulationLimit"},
    {X_GOLD_LIMIT, "GoldLimit"}, {X_MAP_TILES, "MapTilesLimit"},
};

/* The limits' values (with their rule on) and their ranges: the most the
   game's fields and arithmetic hold (docs/EXTRAS.md). */
static int retire_year = 2020, retire_warning = 2000, pop_limit = 32000, gold_limit = 30000,
    map_tiles = 10000;

static int clamp_setting(int v, int lo, int hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

/* Settings the options dialog changed in this run (they are also written
   to CIV.INI): they come first, in replays too. */
static struct {
    const char *key;
    int value;
} changed[16];
static int changed_count;

static void setting_changed(const char *key, int value)
{
    int i;
    for (i = 0; i < changed_count && strcmp(changed[i].key, key); i++)
        ;
    if (i == changed_count) {
        if (changed_count == (int)(sizeof changed / sizeof changed[0]))
            return;
        changed[changed_count++].key = key;
    }
    changed[i].value = value;
}

int extras_setting(const char *key, int def)
{
    int i;
    for (i = 0; i < changed_count; i++)
        if (!strcmp(changed[i].key, key))
            return changed[i].value;
    if (plat_replay) {
        /* replays: the defaults, whatever CIV.INI says, but those of
           CIV2_TEST_SETTINGS ("Gamma=150,Brightness=10": tests) */
        const char *t = getenv("CIV2_TEST_SETTINGS");
        size_t n = strlen(key);
        while (t && *t) {
            if (!strncmp(t, key, n) && t[n] == '=')
                return atoi(t + n + 1);
            t = strchr(t, ',');
            t = t ? t + 1 : 0;
        }
        return def;
    }
    return (int)GetPrivateProfileIntA("OpenCiv2", key, def, "CIV.INI");
}

/* X35: the colour correction's table, from the settings Gamma (percent,
   100: none), Brightness and Contrast (-100 to 100, 0: none). */
static void color_correction_load(void)
{
    int gamma = extras_setting("Gamma", 100), bright = extras_setting("Brightness", 0),
        contrast = extras_setting("Contrast", 0), i;
    plat_color_lut_on = 0;
    if (!extra_ui(X_COLOR) || (gamma == 100 && bright == 0 && contrast == 0))
        return;
    if (gamma < 10)
        gamma = 10;
    for (i = 0; i < 256; i++) {
        double v = pow(i / 255.0, 100.0 / gamma);
        v = (v - 0.5) * (1.0 + contrast / 100.0) + 0.5 + bright / 200.0;
        plat_color_lut[i] = (unsigned char)(v <= 0 ? 0 : v >= 1 ? 255 : (int)(v * 255.0 + 0.5));
    }
    plat_color_lut_on = 1;
}

/* The limits' values, for the rules that are on.  In a replay: those of
   CIV2_TEST_SETTINGS, else the largest. */
static void limits_load(void)
{
    int v;
    v = extras_setting("RetirementYear", plat_replay ? 2020 : 0);
    retire_year = clamp_setting(v, 1, 9999);
    v = extras_setting("RetirementWarningYear", retire_year - 20);
    retire_warning = clamp_setting(v, 0, retire_year - 1);
    /* PopulationLimit: civ_total_population's cap, in 10,000 people.  At
       most what population_milestone (16 bits, total / 100 + 9) holds. */
    v = extras_setting("PopulationLimit", plat_replay ? 3275800 : 0);
    pop_limit = clamp_setting(v, 32000, 3275800);
    /* GoldLimit: the treasury is 32 bits, but the AI's sums (gold / 50 *
       military strength, gold * 2) overflow past about a million. */
    v = extras_setting("GoldLimit", plat_replay ? 1000000 : 0);
    gold_limit = clamp_setting(v, 30000, 1000000);
    /* MapTilesLimit: a custom map's width x height; map.area is 16 bits. */
    v = extras_setting("MapTilesLimit", plat_replay ? 32767 : 0);
    map_tiles = clamp_setting(v, 10000, 32767);
}

void extras_load(void)
{
    const char *test = getenv("CIV2_TEST_EXTRAS");
    unsigned i;
    uint64_t rule_bits = 0;
    extras_enabled = 0;
    extras_rules = 0;
    plat_color_lut_on = 0;
    if (!plat_extras)
        return;
    for (i = 0; i < sizeof rule_keys / sizeof rule_keys[0]; i++)
        rule_bits |= (uint64_t)1 << rule_keys[i].item;
    if (plat_replay && test && *test && strcmp(test, "1") && strcmp(test, "all")) {
        /* a list of item numbers; "-N": all the additions but item N (the
           rule changes only by number) */
        const char *p = test;
        if (*p == '-')
            extras_enabled = ~rule_bits;
        while (*p) {
            int n = atoi(p);
            if (n < 0 && -n < X_COUNT)
                extras_enabled &= ~((uint64_t)1 << -n);
            if (n > 0 && n < X_COUNT)
                extras_enabled |= (uint64_t)1 << n;
            p += strcspn(p, ",");
            p += *p == ',';
        }
        extras_rules = extras_enabled & rule_bits;
        extras_enabled &= ~rule_bits;
        limits_load();
        color_correction_load();
        return;
    }
    for (i = 0; i < sizeof extra_keys / sizeof extra_keys[0]; i++)
        if (extras_setting(extra_keys[i].key, 1))
            extras_enabled |= (uint64_t)1 << extra_keys[i].item;
    if (plat_replay)    /* "1": the additions; "all": and the rule changes */
        extras_rules = test && !strcmp(test, "all") ? rule_bits : 0;
    else
        for (i = 0; i < sizeof rule_keys / sizeof rule_keys[0]; i++)
            if (extras_setting(rule_keys[i].key, 0))
                extras_rules |= (uint64_t)1 << rule_keys[i].item;
    limits_load();
    color_correction_load();
}

int extras_retirement_year(void) { return extra_rule(X_RETIREMENT) ? retire_year : 2020; }
/* X50: the game's year passed year this turn (0: never). */
int extras_year_reached(int year)
{
    return year != 0 && game.year >= year && (game.turn < 1 || game_turn_to_year(game.turn - 1) < year);
}

int extras_retirement_warning_year(void) { return extra_rule(X_RETIREMENT) ? retire_warning : 2000; }
int extras_population_limit(void) { return extra_rule(X_POP_LIMIT) ? pop_limit : 32000; }
int extras_gold_limit(void) { return extra_rule(X_GOLD_LIMIT) ? gold_limit : 30000; }
int extras_map_tiles_limit(void) { return extra_rule(X_MAP_TILES) ? map_tiles : 10000; }

/* The additions' texts by key: a translation (lang/<code>.txt, src/include/
   lang.h) gives them as "openciv2.<key>". */
static const struct { const char *key, *english; } texts[] = {
    {"turn", "Turn"},                               /* X5: the status panel */
    {"cities", "cities"},                           /* X31: the City Status advisor */
    {"size", "Size"}, {"name", "Name"}, {"production", "Production"},
    {"menu.arrange_s", "Arrange Windows S"},        /* X38 (default: the game's Arrange Windows, S) */
    {"menu.arrange_l", "Arrange Windows L"},
    {"menu.options", "Ope&nCiv2 Options"},          /* the Game menu; & before the hotkey */
    /* the options dialog */
    {"options.title", "OpenCiv2 Options"},
    {"options.interface", "Interface additions"},
    {"options.rules", "Rule changes"},
    {"options.limits", "Limits"},
    {"options.other", "Display and messages"},
    {"options.network", "Rule changes and limits are off in network games."},
    {"options.rules_note", "For the human players' units only."},
    {"options.other_note", "Messages shown on the map: GAME.TXT sections, separated by commas."},
    {"option.MouseWheel", "Mouse wheel and button"},
    {"option.UnitListScroll", "Unit list past 9 units"},
    {"option.UnitListKeys", "Keys in that unit list"},
    {"option.WorkCounter", "Settlers' work counter"},
    {"option.GameTurn", "Turn in the status panel"},
    {"option.ResearchNumbers", "Research figures"},
    {"option.ZeroUpkeep", "Free buildings listed"},
    {"option.SupportScroll", "Supported units: scroll"},
    {"option.SupportSort", "Supported units sorted"},
    {"option.AttitudeColors", "Attitude Advisor colours"},
    {"option.CityAttitude", "City window attitude"},
    {"option.RadioHotkeys", "Hotkeys for options"},
    {"option.Quickinfo", "Quick information with Ctrl"},
    {"option.ProductionCancel", "Cancel in Change Production"},
    {"option.ProductionCost", "Costs in Change Production"},
    {"option.CityStatus", "City Status: sort, change"},
    {"option.MassSpecialists", "Shift on specialists"},
    {"option.SuppressPopups", "Messages on the map"},
    {"option.ColorCorrection", "Colour correction"},
    {"option.ForeignGold", "Gold in Foreign Minister"},
    {"option.ArrangeWindowsSL", "Arrange Windows S and L"},
    {"option.PathLines", "Path lines with Shift"},
    {"option.SentryHalftone", "Sentries in grey shades"},
    {"option.DefaultZoom1", "New games at zoom 1:1"},
    {"option.WorkerHandover", "Workers hand over their work"},
    {"option.GotoNoZocStop", "Go To past foreign units"},
    {"option.UnitRotation", "Units of a stack in turn"},
    {"option.MassMove", "Shift+right click moves the type"},
    {"option.StackSentry", "U sentries the stack"},
    {"option.RetirementYear", "Retirement year"},
    {"option.RetirementWarningYear", "Its warning"},
    {"option.PopulationLimit", "Population (10,000s)"},
    {"option.GoldLimit", "Gold"},
    {"option.MapTilesLimit", "Custom map squares"},
    {"option.Scale", "Screen scale (from restart)"},
    {"option.FullScreen", "Full screen (1: yes, from restart)"},
    {"option.Gamma", "Gamma (%, 100: none)"},
    {"option.Brightness", "Brightness (-100 to 100)"},
    {"option.Contrast", "Contrast (-100 to 100)"},
    {"option.SuppressList", "Messages"},
};

static int text_entry(const char *key)
{
    int i;
    for (i = 0; i < (int)(sizeof texts / sizeof texts[0]); i++)
        if (!strcmp(texts[i].key, key))
            return i;
    return -1;
}

int extras_text_translated(const char *key)
{
    char k[96];
    snprintf(k, sizeof k, "openciv2.%s", key);
    return lang_translated(k);
}

const char *extras_text(const char *key)
{
    char k[96];
    int i = text_entry(key);
    snprintf(k, sizeof k, "openciv2.%s", key);
    if (lang_has(k))
        return lang_text(k);
    return i >= 0 ? texts[i].english : key;
}

void extras_texts_each(void (*add)(const char *key, const char *english))
{
    char k[96];
    int i;
    for (i = 0; i < (int)(sizeof texts / sizeof texts[0]); i++) {
        snprintf(k, sizeof k, "openciv2.%s", texts[i].key);
        add(k, texts[i].english);
    }
}


/* ---- X22, X23: attitude colours ---- */

/* The colour of a city's attitude as the Attitude Advisor shows it, yellow
   for celebrating (no unhappy citizen, half or more happy), red for more
   unhappy than happy citizens, else normal. */
int extras_attitude_color(int city, int normal)
{
    const City *c = &cities[city];
    if (c->unhappy == 0 && (int)c->size - (int)(i8)c->happy <= (int)c->size >> 1)
        return 0x72;
    if ((i8)c->happy < (i8)c->unhappy)
        return 0x6a;
    return normal;
}

/* A city's food surplus (negative: shortage) by its record: the food its
   last recalculation found, less its citizens' and settlers' food.  Reads
   the game's data only. */
int extras_city_food_surplus(int city)
{
    const City *c = &cities[city];
    int civ = c->owner, settlers = 0, unit;
    int cost = civs[civ].government > GOV_MONARCHY ? cosmic.settler_food_high : cosmic.settler_food_low;
    for (unit = 0; unit < game.num_units; unit++)
        if (units[unit].id != 0 && units[unit].owner == civ && (byte)units[unit].home_city == city &&
            unit_types[units[unit].type].role == ROLE_SETTLE)
            settlers++;
    return (int)c->total_food - (int)c->size * (int)cosmic.food_per_citizen - settlers * cost;
}

/* The food icon after a city's name: surplus, or the shortage icon. */
void extras_draw_food_trend(iptr surface, int city, int x, int y)
{
    i8 rect[16];
    int surplus = extras_city_food_surplus(city);
    if (surplus > 0)
        war_icon_sprite_block.resource_icon_sprites.draw((iptr)rect, surface, x, y + 2);
    else if (surplus < 0)
        ((Sprite *)war_icon_sprite_block.resource_shortage_icon_sprites)->draw((iptr)rect, surface, x, y + 2);
}

/* ---- X30, X37: icons in the text of a dialog's list rows ---- */

/* EXTRAS_ICON (src/include/extras.h) and a letter in a row's text: an icon
   there. */
Sprite *extras_icon(int code)
{
    switch (code) {
    case 'g': return &tax_rate_icon_sprites_block.gold_coin_icon_sprite;
    case 's': return &war_icon_sprite_block.shield_icon_sprite;
    case 'f': return &war_icon_sprite_block.resource_icon_sprites;
    }
    return 0;
}

/* The width of text with icons in font. */
int extras_rich_width(iptr font, const char *text)
{
    char part[256];
    int w = 0;
    while (*text) {
        size_t n = strcspn(text, EXTRAS_ICON_S);
        if (n >= sizeof part)
            n = sizeof part - 1;
        memcpy(part, text, n);
        part[n] = 0;
        if (n)
            w += ((GameFont *)font)->text_width(part);
        text += n;
        if (*text == EXTRAS_ICON && text[1]) {
            Sprite *s = extras_icon(text[1]);
            if (s)
                w += s->get_width() + 1;
            text += 2;
        } else if (*text)
            text++;
    }
    return w;
}

/* ---- X3: settlers' work counter ---- */

/* Over a settler's sprite (px, py: its top left, zoom the map's), the work
   it has done on its terrain order so far, for the player's own units. */
void extras_draw_work_counter(iptr surface, int unit, int px, int py, int zoom)
{
    char text[8];
    const Unit *u = &units[unit];
    int x, y;
    if (unit_types[u->type].role != ROLE_SETTLE || u->counter == 0 || zoom < -3)
        return;
    if (u->owner != human_civ[0] && game.reveal_map == '\0')
        return;
    snprintf(text, sizeof text, "%d", u->counter);
    x = px + gfx_zoom_scale(0x20, zoom);
    y = py + gfx_zoom_scale(0x28, zoom);
    gfx_select_zoom_font(zoom);
    gfx_set_text_color_index(10);
    ((DibSurface *)surface)->draw_text((iptr)&status_window_block.map_label_font, text, x + 1, y + 1, 0);
    gfx_set_text_color_index(0x72);
    ((DibSurface *)surface)->draw_text((iptr)&status_window_block.map_label_font, text, x, y, 0);
}

/* ---- X41: sentries in grey halftones ---- */

/* Each palette index's nearest grey (by luminance), from the game's palette. */
static u8 grey_map[256];
static u32 grey_map_sum;

static void grey_map_update(void)
{
    u8 rgb[256 * 3];
    u32 sum = 0;
    int i, j, greys[256], ngreys = 0, lum[256];
    ((GamePalette *)&game_palette_block.game_palette)->get_rgb((iptr)rgb, 0, 256);
    for (i = 0; i < 256 * 3; i++)
        sum = sum * 31 + rgb[i];
    if (sum == grey_map_sum && grey_map[1] | grey_map[2])
        return;
    grey_map_sum = sum;
    for (i = 0; i < 256; i++) {
        int r = rgb[i * 3], g = rgb[i * 3 + 1], b = rgb[i * 3 + 2];
        lum[i] = (30 * r + 59 * g + 11 * b) / 100;
        if (i >= 10 && i < 0xec && abs(r - g) < 10 && abs(g - b) < 10 && abs(r - b) < 10)
            greys[ngreys++] = i;
    }
    for (i = 0; i < 256; i++) {
        int best = i, bestd = 1 << 30;
        for (j = 0; j < ngreys; j++) {
            int d = abs(lum[greys[j]] - lum[i]);
            if (d < bestd) {
                bestd = d;
                best = greys[j];
            }
        }
        grey_map[i] = (u8)best;
    }
}

/* sprite drawn at (x, y) on surface, its colours turned to greys: drawn,
   its colours kept, its shape drawn over in a marker colour, then the kept
   colours put back as greys where the marker is. */
void extras_draw_grey_sprite(Sprite *sprite, iptr surface, int x, int y)
{
    enum { MARK = 0xfe };
    RECT r;
    int px, py, w, h;
    u8 *keep;
    DibSurface *s = (DibSurface *)surface;
    grey_map_update();
    sprite->draw((iptr)&r, surface, x, y);
    w = r.right - r.left;
    h = r.bottom - r.top;
    if (w <= 0 || h <= 0 || w > 1024 || h > 1024)
        return;
    keep = (u8 *)malloc((size_t)(w * h));
    for (py = 0; py < h; py++)
        for (px = 0; px < w; px++)
            keep[py * w + px] = (u8)(s->in_clip(r.left + px, r.top + py) ? s->get_pixel(r.left + px, r.top + py) : 0);
    sprite->draw_silhouette((iptr)&r, surface, x, y, MARK);
    for (py = 0; py < h; py++)
        for (px = 0; px < w; px++) {
            int c = keep[py * w + px];
            if (!s->in_clip(r.left + px, r.top + py))
                continue;
            if ((int)s->get_pixel(r.left + px, r.top + py) == MARK)
                s->set_pixel(r.left + px, r.top + py, grey_map[c]);
            else if (c != (int)s->get_pixel(r.left + px, r.top + py))
                s->set_pixel(r.left + px, r.top + py, c);
        }
    free(keep);
}

/* ---- X30: the shields an item costs, in the Change Production list ---- */

/* cost (the rules' tens of shields, as the city builds) and a shield. */
void extras_append_shield_cost(int cost)
{
    text_append_int(cost * city_calc.shield_rows);
    text_append(EXTRAS_ICON_S "s");
    text_append_space();
}

/* ---- X38: Arrange Windows S and L ---- */

static int panel_size;          /* -1 smaller side panel, 1 larger, 0 the game's */

void extras_set_panel_size(int size)
{
    panel_size = size;
}

/* The side panel's width as Arrange Windows S or L makes it. */
int extras_panel_width(int width)
{
    if (panel_size < 0)
        return width - width / 4;
    if (panel_size > 0)
        return width + width / 3;
    return width;
}

/* ---- X24: hotkeys for radio buttons ---- */

/* The items of a radio group without a hotkey get one: the first letter or
   digit of their label no other item of the group has. */
void extras_radio_hotkeys(RadioItem *items, int count)
{
    char used[256];
    int i, j;
    memset(used, 0, sizeof used);
    for (i = 0; i < count; i++)
        if (items[i].hotkey_pos >= 0)
            used[(u8)items[i].hotkey] = 1;
    for (i = 0; i < count; i++) {
        if (items[i].hotkey_pos >= 0)
            continue;
        for (j = 0; items[i].label[j]; j++) {
            int c = (u8)items[i].label[j];
            if (c < 0x80 && isalnum(c) && !used[tolower(c)]) {
                items[i].hotkey = (char)tolower(c);
                items[i].hotkey_pos = j;
                used[tolower(c)] = 1;
                break;
            }
        }
    }
}

/* ---- X1: the mouse wheel and middle button; X32: Shift on specialists ---- */

static HWND frame_hwnd(void *frame_window)
{
    NativeWindow *w = ((WndBase *)((i8 *)frame_window + 0x48))->native_wnd;
    return w ? w->hwnd : 0;
}

/* h, or a window it is in, is ancestor */
static int within(HWND h, HWND ancestor)
{
    for (; h && ancestor; h = GetParent(h))
        if (h == ancestor)
            return 1;
    return 0;
}

static MapView *main_map(void)
{
    return (MapView *)((i8 *)&map_views_block.map_views);
}

static int city_window_open(void)
{
    return ((CityWindow *)((i8 *)&city_wnd.window))->get_city() != -1 && frame_hwnd(&city_wnd.window) &&
           IsWindowVisible(frame_hwnd(&city_wnd.window));
}

/* The game's scroll bar control of a scroll bar window, or 0. */
static Scrollbar *scrollbar_of(HWND h)
{
    char cls[32];
    if (!h || !GetClassNameA(h, cls, sizeof cls) || __strcmpi(cls, "MSScrollBarClass"))
        return 0;
    return (Scrollbar *)GetWindowLongA(h, GetClassLongA(h, -0x12) - 8);
}

/* A scroll bar the wheel turns: the one under the pointer, else the
   vertical one (else the only one) of the window under the pointer or of a
   window it is in. */
static HWND wheel_scrollbar(HWND under)
{
    HWND h, c, any;
    int count;
    if (scrollbar_of(under))
        return under;
    for (h = under; h; h = GetParent(h)) {
        any = 0;
        count = 0;
        for (c = GetWindow(h, GW_CHILD); c; c = GetWindow(c, GW_HWNDNEXT)) {
            if (!scrollbar_of(c) || !IsWindowVisible(c) || !IsWindowEnabled(c))
                continue;
            if (GetWindowLongA(c, GWL_STYLE) & SBS_VERT)
                return c;
            any = c;
            count++;
        }
        if (count == 1)
            return any;
        if (count > 1)
            return 0;       /* several horizontal ones (the tax rates): only under the pointer */
        if (h == frame_hwnd(&main_frame_wnd_block.main_frame_wnd))
            break;
    }
    return 0;
}

/* The scroll bar moved by lines, as the game moves it (its callback). */
static int scroll_by(HWND bar, int lines)
{
    Scrollbar *sb = scrollbar_of(bar);
    int pos;
    if (!sb)
        return 0;
    SetScrollPos(bar, SB_CTL, (int)sb->cur_pos + lines, TRUE);
    pos = GetScrollPos(bar, SB_CTL);
    if (pos == (int)sb->cur_pos)
        return 1;
    sb->cur_pos = pos;
    wnd_set_active_child(sb->get_parent());
    sb->fire_change(pos);
    return 1;
}

/* X1, X32: a citizen of the city window: its specialist changed (dir +1,
   -1; 0 for a click), or with all: every specialist to the same one. */
static int city_specialists(int citizen, int dir, int all)
{
    int city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(), slot, type, i, count;
    if (city < 0 || (cities[city].owner != human_civ[0] && game.reveal_map == '\0'))
        return 0;
    city_recalc(city, 0);
    count = city_calc.specialist_count;
    slot = citizen - (cities[city].size - count);
    if (slot < 0 || slot >= count)
        return 0;
    if (cities[city].size < 5) {
        if (all && dir == 0)
            return 0;
        type = 1;       /* entertainers only */
    } else {
        type = (int)city_specialist_type(city, all && dir ? 0 : slot);
        type = (type - 1 + dir + 3) % 3 + 1;
    }
    for (i = 0; i < count; i++)
        if (all || i == slot)
            city_set_specialist_type(city, i, type);
    ((CityWindow *)((i8 *)&city_wnd.window))->recalc_redraw(1);
    return 1;
}

static void map_zoom_to(int zoom)
{
    zoom = zoom < -7 ? -7 : zoom > 8 ? 8 : zoom;
    if (zoom == map_views_block.mapview_zoom[0])
        return;
    int keep = map_click_in_progress;
    map_views_block.mapview_zoom[0] = (i16)zoom;
    map_click_in_progress = 1;
    main_map()->redraw(human_civ[0], 1);
    map_click_in_progress = keep;
}

/* The main map centred on (x, y), the input that follows kept (the
   redraw would flush it). */
static void map_center_on(int x, int y)
{
    int keep = map_click_in_progress;
    map_click_in_progress = 1;
    main_map()->center_on(x, y);
    map_click_in_progress = keep;
}

static struct {
    int active, moved;
    POINT start;
    int center_x, center_y;
} drag;

static void map_drag_to(POINT pt)
{
    MapView *m = main_map();
    int dx = pt.x - drag.start.x, dy = pt.y - drag.start.y, tw = m->tile_width, th = m->tile_height;
    int mx, my, x, y;
    if (tw <= 0 || th <= 0)
        return;
    mx = (2 * dx + (dx < 0 ? -tw / 2 : tw / 2)) / tw;
    my = (2 * dy + (dy < 0 ? -th / 2 : th / 2)) / th;
    if ((mx + my) & 1) {
        /* map squares have x + y even: the nearer of the two */
        if (abs(2 * dx * th) - abs(mx * tw * th) > abs(2 * dy * tw) - abs(my * th * tw))
            mx += dx < 0 ? -1 : 1;
        else
            my += dy < 0 ? -1 : 1;
    }
    x = drag.center_x - mx;
    y = drag.center_y - my;
    while (y < 0)
        y += 2;
    while (y >= (int)map.height)
        y -= 2;
    if ((x + y) & 1)
        x++;
    if (map_wrap_x(x) == m->center_x && y == m->center_y)
        return;
    drag.moved += 5;
    map_center_on(x, y);
}

static int wheel_message(const MSG *m, HWND under, POINT pt)
{
    int notches = (short)HIWORD(m->wParam) / 120, keys = LOWORD(m->wParam);
    HWND bar;
    if (!notches || abs(notches) > 10)
        return 0;
    if (city_window_open() && within(under, frame_hwnd(&city_wnd.window))) {
        int id = -1, kind = 0;
        POINT c = pt;
        ScreenToClient(frame_hwnd(&city_wnd.window), &c);
        if (city_wnd.clickmap.hit_test( c.x, c.y, (iptr)&id, (iptr)&kind) >= 0 &&
            kind == 2)
            return city_specialists(id, notches > 0 ? 1 : -1,
                                    (keys & MK_SHIFT) && extra_ui(X_MASS_SPECIALISTS)) || 1;
        if (extras_city_support_wheel(c.x, c.y, notches))
            return 1;
    }
    bar = wheel_scrollbar(under);
    if (bar) {
        int vertical = (GetWindowLongA(bar, GWL_STYLE) & SBS_VERT) != 0;
        int lines = vertical ? 3 : 1;
        if (!vertical && scrollbar_of(under) == 0)
            lines = 1;
        scroll_by(bar, -notches * lines);
        return 1;
    }
    if (keys & MK_CONTROL) {
        map_zoom_to(map_views_block.mapview_zoom[0] + (notches > 0 ? 1 : -1));
        return 1;
    }
    return 0;
}

int extras_input(void *msg)
{
    const MSG *m = (const MSG *)msg;
    POINT pt;
    HWND under, map_hwnd;
    if (!extra_ui(X_WHEEL) && !extra_ui(X_MASS_SPECIALISTS) && !extra_ui(X_QUICKINFO) && !extra_ui(X_PATH_LINES) &&
        !extra_ui(X_SUPPRESS_POPUPS))
        return 0;
    under = plat_window_at_cursor(&pt);
    if (extra_ui(X_SUPPRESS_POPUPS))
        messages_expire();
    if (extra_ui(X_QUICKINFO) || extra_ui(X_PATH_LINES)) {
        if (extras_hover_input(m, under, pt))
            return 1;
    }
    if (m->message == WM_LBUTTONDOWN && (m->wParam & MK_SHIFT) && extra_ui(X_MASS_SPECIALISTS) &&
        city_window_open() && m->hwnd == frame_hwnd(&city_wnd.window)) {
        /* X32: Shift+click on a specialist: all of them to its kind */
        int id = -1, kind = 0;
        if (city_wnd.clickmap.hit_test( (short)LOWORD(m->lParam),
                              (short)HIWORD(m->lParam), (iptr)&id, (iptr)&kind) >= 0 && kind == 2)
            return city_specialists(id, 0, 1);
        return 0;
    }
    if (!extra_ui(X_WHEEL))
        return 0;
    if (m->message == WM_MOUSEWHEEL)
        return wheel_message(m, under, pt);
    map_hwnd = frame_hwnd(main_map());
    switch (m->message) {
    case WM_MBUTTONDOWN:
        drag.active = 0;
        if (m->wParam & MK_CONTROL) {
            map_zoom_to(0);         /* Ctrl+middle button: the normal zoom */
            return 1;
        }
        if (m->hwnd != map_hwnd)
            return 0;
        drag.active = 1;
        drag.moved = 0;
        drag.start = pt;
        drag.center_x = main_map()->center_x;
        drag.center_y = main_map()->center_y;
        return 1;
    case WM_MOUSEMOVE:
        if (!drag.active)
            return 0;
        if (!(m->wParam & MK_MBUTTON) || m->hwnd != map_hwnd) {
            drag.active = 0;
            return 0;
        }
        drag.moved++;
        map_drag_to(pt);
        return 1;
    case WM_MBUTTONUP:
        if (!drag.active)
            return 0;
        drag.active = 0;
        if (drag.moved < 5 && m->hwnd == map_hwnd) {
            /* a click: the map centred there */
            int x, y;
            if (main_map()->screen_to_tile((iptr)&x, (iptr)&y, (short)LOWORD(m->lParam), (short)HIWORD(m->lParam)) == 0 &&
                (x != main_map()->center_x || y != main_map()->center_y))
                map_center_on(x, y);
        }
        return 1;
    }
    return 0;
}



/* ---- X2, X28: the Activate Unit list of more than 9 units ---- */

/* A row's unit (Dialog's option draw callback): its picture in the
   indent, then the text the default way. */
i32 extras_unit_list_draw(Dialog *dialog, iptr surface, i32 unit, i32 flags, i32 x, i32 y, i32 row_height)
{
    int zoom = -4, h = gfx_zoom_scale(0x30, zoom);
    (void)dialog;
    (void)flags;
    gfx_draw_unit(surface, unit, 4, x, y - (h - row_height) / 2, zoom, 0);
    return 0;
}

/* ---- X11, X16: the city window's supported units: scroll bar, total, sorted ---- */

static Scrollbar *support_bar;
static int support_scroll, support_city = -1, support_rows_total;

static void support_on_scroll(i32 pos)
{
    support_scroll = pos;
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_supported_units(1);
}

/* X16: settlers first, then the units that can attack by domain (land, air,
   sea), stronger defence, then attack first, then the rest; by number
   within each. */
static int support_order(const void *a, const void *b)
{
    int u = *(const int *)a, v = *(const int *)b;
    const UnitType *t = &unit_types[units[u].type], *w = &unit_types[units[v].type];
    int ku = t->role == ROLE_SETTLE ? 0 : t->attack ? 1 : 2, kv = w->role == ROLE_SETTLE ? 0 : w->attack ? 1 : 2;
    if (ku != kv)
        return ku - kv;
    if (ku == 1) {
        if (t->domain != w->domain)
            return t->domain - w->domain;
        if (t->defense != w->defense)
            return w->defense - t->defense;
        if (t->attack != w->attack)
            return w->attack - t->attack;
    }
    return u - v;
}

/* The city window closes its controls. */
void extras_city_window_controls_destroyed(void)
{
    if (support_bar && support_bar->hwnd)
        support_bar->destroy();
}

/* CityWindow::draw_supported_units with X11 or X16: the same units and
   upkeep, sorted (X16), with a total and a scroll bar when they do not fit
   (X11). */
void extras_draw_supported_units(CityWindow *w, int blit)
{
    static int list[2048], order[2048];
    static u8 icons[2048], kind[2048], cost[2048];
    int zoom = w->view_scale == 1 ? -6 : w->view_scale == 3 ? 0 : -3;
    int cell_w = gfx_zoom_scale(0x45, zoom), cell_h = gfx_zoom_scale(0x34, zoom);
    int area_w = w->scale(0xc0), area_h = w->scale(0x4e), label_h = 0;
    int left = w->supported_units_rect, top = w->supported_units_rect_top;
    int civ = cities[w->city_index].owner, gov = civs[civ].government;
    int n = 0, i, military_away = 0, cols, rows, need_bar, margin_x, margin_y, first;
    int unit_size = gfx_zoom_scale(0x40, zoom), icon_offset_y = gfx_zoom_scale(0x20, zoom);
    int scroll = extra_ui(X_SUPPORT_LIST);
    if (w->city_index != support_city) {
        support_city = w->city_index;
        support_scroll = 0;
    }
    w->begin_draw((iptr)&w->supported_units_rect);
    /* the upkeep of each in the game's order (the units away from home
       that make citizens unhappy are counted in that order) */
    for (i = 0; i < game.num_units; i++) {
        int u = i, c = 0, k = 0, up = 0;
        if (units[u].id == 0 || (uint)(byte)units[u].home_city != (uint)w->city_index)
            continue;
        if (unit_types[units[u].type].role == ROLE_SETTLE)
            up = city_calc.settler_food_cost;
        if (units[u].flags & UNITF_COSTS_SHIELD)
            up++;
        if ((units[u].flags & UNITF_AGGRESSIVE) && GOV_FUNDAMENTALISM < gov) {
            c = !civ_has_wonder(civ, WONDER_WOMENS_SUFFRAGE) && !city_has_improvement(w->city_index, IMP_POLICE_STATION);
            if (gov == GOV_DEMOCRACY)
                c++;
            else if (c && military_away == 0)
                c = 0;
            if (c == 0) {
                c = 1;
                k = 2;
            } else
                k = 1;
            up += c;
            military_away++;
        }
        list[n] = u;
        icons[n] = (u8)up;
        kind[n] = (u8)k;
        cost[n] = (u8)c;
        order[n] = n;
        n++;
    }
    if (extra_ui(X_SUPPORT_SORT)) {
        static int sorted[2048];
        for (i = 0; i < n; i++)
            sorted[i] = list[i];
        qsort(sorted, n, sizeof sorted[0], support_order);
        for (i = 0; i < n; i++) {
            int j;
            for (j = 0; list[j] != sorted[i]; j++)
                ;
            order[i] = j;
        }
    }
    /* the label, with the total (X11) */
    text_clear();
    text_append_label(0x1bf);
    if (scroll) {
        text_append_colon();
        text_append_int(n);
        label_h = w->scale(0xc);
    }
    cols = area_w / cell_w;
    rows = (area_h - label_h) / cell_h;
    if (rows < 1)
        rows = 1;
    need_bar = scroll && n > cols * rows;
    if (need_bar)
        cols = (area_w - GetSystemMetrics(SM_CXVSCROLL)) / cell_w;
    if (cols < 1)
        cols = 1;
    if (!scroll)
        rows = area_h / cell_h;
    support_rows_total = (n + cols - 1) / cols;
    if (support_scroll > support_rows_total - rows)
        support_scroll = support_rows_total - rows;
    if (support_scroll < 0 || !need_bar)
        support_scroll = 0;
    margin_x = (area_w - (need_bar ? GetSystemMetrics(SM_CXVSCROLL) : 0) - cell_w * cols + gfx_zoom_scale(5, zoom)) >> 1;
    margin_y = (area_h - cell_h * rows + gfx_zoom_scale(4, zoom)) >> 1;
    if (scroll || n <= cols) {
        int lw = rect_width((RECT *)(iptr)((u8 *)&w->supported_units_label_rect));
        text_set_colors(0x7c, 0x12, 1, 1);
        gfx_draw_text_centered((const char *)&textbuf.buf, w->supported_units_label_rect,
                               w->supported_units_label_rect_top + w->scale(1), lw);
        if (scroll)
            margin_y = label_h + ((area_h - label_h - cell_h * (support_rows_total < rows ? support_rows_total : rows)) >> 1);
        else
            margin_y = (area_h - gfx_zoom_scale(0x30, zoom)) >> 1;
    }
    w->clickmap.remove_by_id( 6);
    first = support_scroll * cols;
    for (i = first; i < n && i < first + cols * rows; i++) {
        int j = order[i], u = list[j], k, col = (i - first) % cols, row = (i - first) / cols;
        int x = left + margin_x + col * cell_w, y = top + margin_y + row * cell_h, ix, sp;
        i8 r[16], hot[16];
        gfx_draw_unit((iptr)w, u, 4, x, y, zoom, 0);
        gfx_set_zoom_scale(w->view_scale * 4 - 8);
        sp = ui_calc_item_spacing(icons[j], w->scale(10), unit_size, 0, 0);
        ix = x;
        if (unit_types[units[u].type].role == ROLE_SETTLE)
            for (k = 0; k < city_calc.settler_food_cost; k++, ix += sp)
                small_resource_icon_sprites_block.small_resource_icon_sprites.draw((iptr)r, (iptr)w, ix, y + icon_offset_y);
        if (units[u].flags & UNITF_COSTS_SHIELD) {
            small_resource_icon_sprites_block.small_shield_icon_sprite.draw((iptr)r, (iptr)w, ix, y + icon_offset_y);
            ix += sp;
        }
        for (k = 0; kind[j] && k < cost[j]; k++, ix += sp)
            ((Sprite *)(kind[j] == 1 ? small_unhappy_icon_sprite : unhappy_suppressed_icon_sprite))
                ->draw((iptr)r, (iptr)w, ix, y + icon_offset_y);
        gfx_reset_zoom_scale();
        rect_set_xywh((iptr)hot, x, y, unit_size, unit_size);
        w->clickmap.hotspot_add_rect(u, 6, (RECT *)hot);
    }
    /* the scroll bar (X11) */
    if (need_bar) {
        i8 rect[16];
        int sw = GetSystemMetrics(SM_CXVSCROLL);
        if (!support_bar) {
            support_bar = (Scrollbar *)operator_new(sizeof(Scrollbar));
            support_bar->construct();
        }
        if (!support_bar->hwnd) {
            rect_set_xywh((iptr)rect, left + area_w - sw - 1, top + label_h + 1, sw, area_h - label_h - 2);
            support_bar->create((iptr)&city_wnd.wnd, 98, (iptr)rect, 1);
            support_bar->set_callback(support_on_scroll);
        }
        support_bar->set_range(0, support_rows_total - rows);
        support_bar->set_page_size(1);
        support_bar->set_pos(support_scroll);
    } else if (support_bar && support_bar->hwnd)
        support_bar->destroy();
    if (blit)
        w->wnd_blit_surface_rect((iptr)&w->supported_units_rect);
}

/* X1: the wheel over the supported units (city window coordinates). */
int extras_city_support_wheel(int x, int y, int notches)
{
    CityWindow *w = (CityWindow *)((i8 *)&city_wnd.window);
    RECT *r = (RECT *)&w->supported_units_rect;
    if (!support_bar || !support_bar->hwnd || x < r->left || x >= r->right || y < r->top || y >= r->bottom)
        return 0;
    return scroll_by((HWND)(uptr)support_bar->hwnd, -notches);
}

/* ---- X31: the City Status advisor: total, header, sorting, production ---- */

enum { SORT_NONE, SORT_SIZE, SORT_NAME, SORT_FOOD, SORT_SHIELDS, SORT_TRADE, SORT_PRODUCTION };
static int status_sort, status_cities[256], status_count, status_list_top, status_header_top;
static int status_col_name, status_col_res, status_col_prod;

static int status_compare(const void *a, const void *b)
{
    const City *p = &cities[*(const int *)a], *q = &cities[*(const int *)b];
    int d = 0;
    switch (status_sort) {
    case SORT_SIZE: d = q->size - p->size; break;
    case SORT_NAME: d = __strcmpi(p->name, q->name); break;
    case SORT_FOOD: d = q->total_food - p->total_food; break;
    case SORT_SHIELDS: d = q->total_shields - p->total_shields; break;
    case SORT_TRADE: d = q->trade - p->trade; break;
    case SORT_PRODUCTION: d = q->building - p->building; break;
    }
    return d ? d : *(const int *)a - *(const int *)b;
}

static void status_collect(int civ)
{
    int city;
    status_count = 0;
    for (city = 0; city < game.num_cities && status_count < 256; city++)
        if (cities[city].id != 0 && cities[city].owner == civ)
            status_cities[status_count++] = city;
    if (status_sort != SORT_NONE)
        qsort(status_cities, status_count, sizeof status_cities[0], status_compare);
}

/* The production's picture: a unit's, or an improvement's or wonder's. */
static void status_draw_production_icon(int city, int x, int y)
{
    i8 r[16];
    int b = cities[city].building;
    if (b >= 0) {
        gfx_set_zoom_scale(-4);
        ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + b * 0x3c))->draw((iptr)r, (iptr)&advisor_state.window, x, y - 2);
        gfx_reset_zoom_scale();
    } else if (-b < 0x27)
        ((Sprite *)(caption_close_button_sprite_block.improvement_icon_sprites + -b * 0x3c))
            ->draw((iptr)r, (iptr)&advisor_state.window, x, y);
    else {
        gfx_set_zoom_scale(-4);
        ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (-b * 4 - 0x9c) * 0xf))
            ->draw((iptr)r, (iptr)&advisor_state.window, x, y);
        gfx_reset_zoom_scale();
    }
}

void extras_city_status_draw(void)
{
    int civ = advisor_state.report_civ, top_y, list_bottom, line_height, i, row, rows_per_page;
    int max_row, x;
    i8 sprite_rect[16], scroll_rect[16];
    char *text;
    ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
    ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
    ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
    top_y = advisor_state.client_y + 2;
    list_bottom = advisor_state.client_y + 0x174;
    line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
    status_collect(civ);
    text_set_surface((iptr)&advisor_state.window);
    text_set_font((iptr)&advisor_font_block.advisor_font);
    text_set_colors(0x25, 0x12, 2, 1);
    text_clear();
    text_append(label_string(label_table.string_ids[0x14e]));
    gfx_draw_text_centered((const char *)&textbuf.buf, advisor_state.client_x, top_y, advisor_state.client_width);
    top_y += line_height;
    text_clear();
    text_append_label(int_clamp(civs[civ].government - 1, 0, 4) + 0x148);
    text_append_space();
    text_append_label(0x8c);
    text_append_space();
    text_append(civ_name_plural(civ));
    /* the total */
    text_append(", ");
    text_append_int(status_count);
    text_append_space();
    text_append(extras_text("cities"));
    gfx_draw_text_centered((const char *)&textbuf.buf, advisor_state.client_x, top_y, advisor_state.client_width);
    top_y += line_height;
    text_clear();
    text_append((char *)civ_leader_title(civ));
    text_append_space();
    text_append(civ_leader_name(civ));
    text_append_colon();
    text_append_year((int)game.year);
    gfx_draw_text_centered((const char *)&textbuf.buf, advisor_state.client_x, top_y, advisor_state.client_width);
    /* the columns' headings: click one to sort by it */
    status_header_top = top_y + line_height + 2;
    status_col_name = advisor_state.client_x + 0x8c;
    status_col_res = status_col_name + 0x69;
    status_col_prod = status_col_res + 0x78;
    text_set_font((iptr)&advisor_bold_font);
    text_set_colors(status_sort == SORT_SIZE ? 0x72 : 0x29, 0x12, 1, 1);
    gfx_draw_text(extras_text("size"), advisor_state.client_x + 0x2c, status_header_top);
    text_set_colors(status_sort == SORT_NAME ? 0x72 : 0x29, 0x12, 1, 1);
    gfx_draw_text(extras_text("name"), status_col_name, status_header_top);
    for (i = 0; i < 3; i++) {
        ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + i * 0x78))
            ->draw((iptr)sprite_rect, (iptr)&advisor_state.window, status_col_res + 0x10 + i * 0x26, status_header_top + 2);
        if (status_sort == SORT_FOOD + i)
            gfx_draw_text("*", status_col_res + 0x10 + i * 0x26 + 0x10, status_header_top);
    }
    text_set_colors(status_sort == SORT_PRODUCTION ? 0x72 : 0x29, 0x12, 1, 1);
    gfx_draw_text(extras_text("production"), status_col_prod, status_header_top);
    status_list_top = status_header_top + 0x12;
    advisor_state.row_height = 0x18;
    advisor_state.list_top = status_list_top;
    advisor_state.list_height = list_bottom - status_list_top;
    advisor_state.rows_per_page = rows_per_page = int_clamp((list_bottom - status_list_top) / 0x18, 1, 99);
    advisor_state.page_count = int_clamp((rows_per_page - 1 + status_count) / rows_per_page, 1, 99);
    max_row = int_clamp(status_count - 1, 0, 999);
    advisor_state.first_row = int_clamp(advisor_state.first_row, 0, max_row);
    for (row = 0; row < rows_per_page && advisor_state.first_row + row < status_count; row++) {
        int city = status_cities[advisor_state.first_row + row], y = status_list_top + row * 0x18, ty = y + 9;
        uint cost;
        gfx_draw_city((iptr)&advisor_state.window, city, 0, ((row + 1) & 1) * 0x40 + advisor_state.client_x + 2, y, 0);
        text_set_font((iptr)&advisor_bold_font);
        text_set_colors(0x25, 0x12, 1, 1);
        gfx_draw_text((const char *)&cities[city].name, status_col_name, ty);
        x = status_col_res;
        for (i = 0; i < 3; i++) {
            text_clear();
            text_append_int(i == 0 ? (int)cities[city].total_food : i == 1 ? (int)cities[city].total_shields
                                                                            : (int)cities[city].trade);
            x = gfx_draw_text((const char *)&textbuf.buf, x, ty) + 2;
            ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_icon_sprites) + i * 0x78))
                ->draw((iptr)sprite_rect, (iptr)&advisor_state.window, x, ty);
            x += 0x12;
        }
        x = status_col_prod;
        status_draw_production_icon(city, x, ty);
        x += 0x18;
        text_clear();
        if (cities[city].building < 0) {
            int imp = -cities[city].building;
            text_append_string_id(improvements[imp].name);
            if (imp > 0x26)
                text_set_colors(0x5e, 10, 0xffffffff, 0xffffffff);
            cost = improvements[imp].cost;
        } else {
            text_set_colors(0x7a, 10, 0xffffffff, 0xffffffff);
            text_append_string_id(unit_types[cities[city].building].name);
            cost = (uint)(int)(char)unit_types[cities[city].building].cost;
        }
        text_append_space();
        x = gfx_draw_text((const char *)&textbuf.buf, x, ty) + 4;
        text_clear();
        text_append_lparen();
        text_append_int((int)cities[city].shield_box);
        text_append("/");
        text_append_int(cosmic.shield_box_rows * cost);
        text_append_rparen();
        text_set_colors(0x21, 0x12, 0xffffffff, 0xffffffff);
        gfx_draw_text((const char *)&textbuf.buf, x, ty);
    }
    if (advisor_state.controls_created == 0) {
        rect_set((iptr)scroll_rect, (advisor_state.client_right - advisor_state.vscroll_width) - 2, status_list_top,
                 advisor_state.client_right - 2, list_bottom);
        advisor_state.scrollbar.create((iptr)&advisor_state.wnd, 200, (iptr)scroll_rect, 1);
        advisor_state.scrollbar.set_range(0, max_row);
        advisor_state.scrollbar.set_pos(advisor_state.first_row);
        advisor_state.scrollbar.set_page_size(rows_per_page);
        advisor_state.scrollbar.set_callback(city_status_on_scroll);
        advisor_state.scrollbar.show_and_redraw();
        ((WndHandlers *)((i8 *)&advisor_state.wnd_events))->set_lbuttonup_handler(city_status_on_click);
        advisor_state.controls_created = 1;
    }
    advisor_state.window.wnd_blit_surface();
}

/* A click: on a heading, the list sorted by it (again: in the game's
   order); on a city's production, its Change Production; else the city. */
void extras_city_status_click(int x, int y)
{
    int row, city;
    if (y >= status_header_top && y < status_list_top) {
        int key = x < status_col_name ? SORT_SIZE : x < status_col_res ? SORT_NAME
                  : x < status_col_prod ? SORT_FOOD + int_clamp((x - status_col_res - 0x10) / 0x26, 0, 2)
                                        : SORT_PRODUCTION;
        status_sort = status_sort == key ? SORT_NONE : key;
        extras_city_status_draw();
        return;
    }
    if (y < status_list_top)
        return;
    row = (y - status_list_top) / 0x18;
    if (row >= advisor_state.rows_per_page || advisor_state.first_row + row >= status_count)
        return;
    city = status_cities[advisor_state.first_row + row];
    ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
    if (x >= status_col_prod && (cities[city].owner == human_civ[0] || game.reveal_map != '\0')) {
        city_button_change(0);
        city_window_close();
        extras_city_status_draw();
    }
}

/* ---- X25: quick information with Ctrl over the map ---- */

static struct {
    int shown, x, y;
} tip;

static void map_redraw_keep_input(void)
{
    int keep = map_click_in_progress;
    map_click_in_progress = 1;
    main_map()->redraw(human_civ[0], 1);
    map_click_in_progress = keep;
}

static void tip_hide(void)
{
    if (!tip.shown)
        return;
    tip.shown = 0;
    map_redraw_keep_input();
}

/* The lines about the city and the units the player sees on (x, y). */
static int tip_lines(int x, int y, char lines[][96], int max)
{
    int n = 0, city = city_at(x, y), unit, viewer = human_civ[0], count = 0;
    int sees = game.reveal_map != '\0' || tile_visible_to(x, y, viewer);
    if (city >= 0 && (cities[city].owner == viewer || game.reveal_map != '\0' ||
                      (cities[city].known_to & (1 << viewer)))) {
        const City *c = &cities[city];
        snprintf(lines[n++], 96, "%s (%d)", c->name, c->size);
        if (c->owner == viewer || game.reveal_map != '\0') {
            text_clear();
            text_append_int(c->total_food);
            text_append(EXTRAS_ICON_S "f ");
            text_append_int(c->total_shields);
            text_append(EXTRAS_ICON_S "s ");
            text_append_int(c->trade);
            text_append(EXTRAS_ICON_S "t  ");
            if (c->building < 0)
                text_append_string_id(improvements[-c->building].name);
            else
                text_append_string_id(unit_types[c->building].name);
            snprintf(lines[n++], 96, "%s", (char *)&textbuf.buf);
        }
    }
    unit = tile_first_unit(x, y);
    if (unit >= 0 && (units[unit].owner == viewer || sees)) {
        for (; unit >= 0 && n < max; unit = unit_next_in_stack(unit), count++) {
            const Unit *u = &units[unit];
            const UnitType *t = &unit_types[u->type];
            text_clear();
            if (u->owner != viewer) {
                text_append_str((const char *)civ_adjective(u->owner));
                text_append_space();
            }
            if (u->flags & UNITF_VETERAN) {
                text_append_label(0xd);
                text_append_space();
            }
            text_append_string_id(t->name);
            text_append("  ");
            text_append_int(t->attack);
            text_append("/");
            text_append_int(t->defense);
            text_append("/");
            text_append_int(t->move / (cosmic.road_move_mult ? cosmic.road_move_mult : 3));
            text_append("  ");
            text_append_int((t->hit_points ? t->hit_points : 10) - u->damage);
            text_append("/");
            text_append_int(t->hit_points ? t->hit_points : 10);
            if (u->owner == viewer && u->orders != ORDER_NONE && (u->orders & 0xf) > 0) {
                text_append("  ");
                status_append_unit_orders(unit);
            }
            snprintf(lines[n++], 96, "%s", (char *)&textbuf.buf);
        }
        if (unit >= 0 && n == max)
            snprintf(lines[n - 1], 96, "...");
    }
    (void)count;
    return n;
}

static void tip_show(int x, int y, POINT at)
{
    char lines[12][96];
    int n = tip_lines(x, y, lines, 12), i, w = 0, lh, bx, by;
    MapView *m = main_map();
    RECT r, clip;
    iptr font = (iptr)&advisor_bold_font;
    if (n == 0)
        return;
    lh = ((GameFont *)font)->line_spacing();
    for (i = 0; i < n; i++) {
        int lw = extras_rich_width(font, lines[i]);
        if (lw > w)
            w = lw;
    }
    bx = at.x + 16;
    by = at.y + 16;
    if (bx + w + 8 > (int)(m->client_x + m->client_width))
        bx = at.x - w - 16;
    if (by + n * lh + 6 > (int)(m->client_y + m->client_height))
        by = at.y - n * lh - 14;
    if (bx < (int)m->client_x)
        bx = m->client_x;
    if (by < (int)m->client_y)
        by = m->client_y;
    SetRect(&r, bx, by, bx + w + 8, by + n * lh + 6);
    SetRect(&clip, m->client_x, m->client_y, m->client_x + m->client_width, m->client_y + m->client_height);
    ((DibSurface *)m)->set_clip(&clip);
    gfx_fill_rect((iptr)m, &r, 10);
    gfx_draw_rect_frame((iptr)m, &r, 0x29);
    text_set_surface((iptr)m);
    text_set_font(font);
    for (i = 0; i < n; i++) {
        char part[96];
        const char *t = lines[i];
        int tx = bx + 4, ty = by + 3 + i * lh;
        text_set_colors(i == 0 ? 0x72 : 0x29, 10, 1, 1);
        while (*t) {
            size_t k = strcspn(t, EXTRAS_ICON_S);
            memcpy(part, t, k);
            part[k] = 0;
            if (k)
                tx = gfx_draw_text(part, tx, ty);
            t += k;
            if (*t == EXTRAS_ICON && t[1]) {
                Sprite *s = t[1] == 't' ? (Sprite *)&war_icon_sprite_block.trade_icon_sprite : extras_icon(t[1]);
                i8 sr[16];
                if (s) {
                    s->draw((iptr)sr, (iptr)m, tx, ty + 1);
                    tx += s->get_width() + 1;
                }
                t += 2;
            } else if (*t)
                t++;
        }
    }
    ((DibSurface *)m)->reset_clip();
    ((DibSurface *)m)->wnd_blit_surface_rect((iptr)&r);
    tip.shown = 1;
    tip.x = x;
    tip.y = y;
}

static int quickinfo_input(const MSG *m, HWND under, POINT pt)
{
    HWND map_hwnd = frame_hwnd(main_map());
    int ctrl = GetKeyState(VK_CONTROL) < 0, x, y;
    POINT c = pt;
    if (m->message == WM_KEYUP && m->wParam == VK_CONTROL) {
        tip_hide();
        return 0;
    }
    if (m->message != WM_MOUSEMOVE && !(m->message == WM_KEYDOWN && m->wParam == VK_CONTROL))
        return 0;
    if (!ctrl || !map_hwnd || !within(under, map_hwnd) || turn_loop.active == '\0') {
        tip_hide();
        return 0;
    }
    ScreenToClient(map_hwnd, &c);
    if (main_map()->screen_to_tile((iptr)&x, (iptr)&y, c.x, c.y) != 0 || !map_in_bounds(x, y)) {
        tip_hide();
        return 0;
    }
    if (tip.shown && tip.x == x && tip.y == y)
        return 0;
    tip_hide();
    tip_show(x, y, c);
    return 0;
}

int extras_hover_input(const MSG *m, HWND under, POINT pt)
{
    if (extra_ui(X_QUICKINFO) && quickinfo_input(m, under, pt))
        return 1;
    if (extra_ui(X_PATH_LINES) && extras_path_input(m, under, pt))
        return 1;
    return 0;
}


/* ---- X33: simple popups of GAME.TXT shown on the map instead ---- */

static Dialog *noted_dialog;
static char noted_file[32], noted_section[64];
static struct {
    char text[160];
    DWORD until;
} messages[4];

void extras_note_dialog(Dialog *d, const char *file, const char *section)
{
    noted_dialog = d;
    snprintf(noted_file, sizeof noted_file, "%s", file ? file : "");
    snprintf(noted_section, sizeof noted_section, "%s", section ? section : "");
}

/* Whether section is in the setting SuppressList (sections of GAME.TXT,
   separated by commas or spaces). */
static char suppress_changed[256];     /* SuppressList as the options dialog set it */
static int suppress_was_changed;

static void suppress_list_get(char *list, int n)
{
    const char *t = getenv("CIV2_TEST_SUPPRESS");
    if (suppress_was_changed)
        snprintf(list, n, "%s", suppress_changed);
    else if (plat_replay)
        snprintf(list, n, "%s", t ? t : "");
    else
        plat_profile_string("CIV.INI", "OpenCiv2", "SuppressList", list, n);
}

static int suppress_listed(const char *section)
{
    char list[1024], *p;
    suppress_list_get(list, sizeof list);
    for (p = list; *p;) {
        size_t n = strcspn(p, ", \t");
        char c = p[n];
        p[n] = 0;
        if (n && !__strcmpi(p, section))
            return 1;
        p += n + (c != 0);
    }
    return 0;
}

static void messages_expire(void)
{
    DWORD now = GetTickCount();
    int i, gone = 0;
    for (i = 0; i < 4; i++)
        if (messages[i].text[0] && (int)(now - messages[i].until) >= 0) {
            messages[i].text[0] = 0;
            gone = 1;
        }
    if (gone && turn_loop.active != '\0')
        map_redraw_keep_input();
}

/* The messages over the top of the map (surface: the main map view). */
void extras_draw_map_messages(iptr surface, int blit)
{
    MapView *m = (MapView *)surface;
    iptr font = (iptr)&advisor_bold_font;
    int i, y = m->client_y + 4, lh = ((GameFont *)font)->line_spacing(), any = 0;
    RECT r, clip;
    SetRect(&clip, m->client_x, m->client_y, m->client_x + m->client_width, m->client_y + m->client_height);
    SetRect(&r, m->client_x, y, m->client_x + m->client_width, y);
    ((DibSurface *)m)->set_clip(&clip);
    text_set_surface(surface);
    text_set_font(font);
    for (i = 0; i < 4; i++) {
        if (!messages[i].text[0])
            continue;
        text_set_colors(0x29, 10, 1, 1);
        gfx_draw_text(messages[i].text, m->client_x + 6, y);
        y += lh;
        any = 1;
    }
    ((DibSurface *)m)->reset_clip();
    r.bottom = y + 2;
    if (blit && any)
        ((DibSurface *)m)->wnd_blit_surface_rect((iptr)&r);
}

int extras_popup_suppressed(Dialog *d)
{
    DialogLine *line;
    char text[160];
    size_t n = 0;
    int i;
    if (d != noted_dialog || __strcmpi(noted_file, "GAME") || !suppress_listed(noted_section))
        return 0;
    /* only a message: no choice, no input, no button but OK */
    if (d->option_count || d->edit_count || d->extra_button_count || (d->style_flags & 0x41) || !d->line_list)
        return 0;
    text[0] = 0;
    for (line = d->line_list; line && n < sizeof text - 2; line = line->next) {
        if (!line->text || !line->text[0])
            continue;
        n += snprintf(text + n, sizeof text - n, "%s%s", n ? " " : "", line->text);
    }
    for (i = 0; i < 3; i++)
        messages[i] = messages[i + 1];
    snprintf(messages[3].text, sizeof messages[3].text, "%s", text);
    messages[3].until = GetTickCount() + 8000;
    noted_dialog = 0;
    if (turn_loop.active != '\0' && frame_hwnd(main_map()))
        map_redraw_keep_input();
    return 1;
}

/* ---- X39: path lines with Shift: a unit's way, a city's trade routes ---- */

static const int nb_dx[8] = {0, 1, 2, 1, 0, -1, -2, -1}, nb_dy[8] = {-2, -1, 0, 1, 2, 1, 0, -1};
static struct {
    int shown, unit, x, y;
} path;

static void path_hide(void)
{
    if (!path.shown)
        return;
    path.shown = 0;
    map_redraw_keep_input();
}

/* The cost (thirds of a move) of a step of unit from (x, y) to (nx, ny) as
   the player knows the map: -1 if it cannot go there. */
static int step_cost(int unit, int x, int y, int nx, int ny)
{
    int civ = units[unit].owner, domain = unit_types[units[unit].type].domain, t, from, to;
    int mult = cosmic.road_move_mult ? cosmic.road_move_mult : 3;
    int seen = game.reveal_map != '\0' || tile_visible_to(nx, ny, civ);
    if (domain == DOMAIN_AIR)
        return mult;
    if (!seen)
        return domain == DOMAIN_SEA ? -1 : mult;    /* unknown land: as plains */
    t = tile_terrain(nx, ny) & 0xf;
    if (domain == DOMAIN_SEA)
        return t == 10 || city_at(nx, ny) >= 0 ? mult : -1;
    if (t == 10)
        return -1;
    from = game.reveal_map != '\0' ? (int)tile_improvements(x, y) : *known_at(x, y, civ);
    to = game.reveal_map != '\0' ? (int)tile_improvements(nx, ny) : *known_at(nx, ny, civ);
    if (city_at(x, y) >= 0)
        from |= TILE_ROAD;
    if (city_at(nx, ny) >= 0)
        to |= TILE_ROAD;
    if ((from & TILE_RAILROAD) && (to & TILE_RAILROAD))
        return 0;
    if ((from & TILE_ROAD) && (to & TILE_ROAD))
        return 1;
    return terrain_types[t].move_cost * mult;
}

/* The cheapest way of unit to (tx, ty) (Dijkstra's, over the map as the
   player knows it): its squares from the target back, n of them. */
static int unit_path(int unit, int tx, int ty, int *px, int *py, int max)
{
    int w = map.width, h = map.height, size = w * h, n = 0, i;
    int *dist, *prev, *heap, hn = 0, start;
    if (size <= 0)
        return 0;
    dist = (int *)malloc(sizeof(int) * size * 3);
    prev = dist + size;
    heap = prev + size;
    for (i = 0; i < size; i++)
        dist[i] = 1 << 30, prev[i] = -1;
    start = units[unit].y * w + units[unit].x;
    dist[start] = 0;
    heap[hn++] = start;
    while (hn) {
        /* the nearest square: a linear search, the frontier is small */
        int best = 0, cur, cx, cy, d;
        for (i = 1; i < hn; i++)
            if (dist[heap[i]] < dist[heap[best]])
                best = i;
        cur = heap[best];
        heap[best] = heap[--hn];
        cx = cur % w;
        cy = cur / w;
        if (cx == tx && cy == ty)
            break;
        for (d = 0; d < 8; d++) {
            int nx = map_wrap_x(cx + nb_dx[d]), ny = cy + nb_dy[d], c, ni;
            if (!map_in_bounds(nx, ny))
                continue;
            c = step_cost(unit, cx, cy, nx, ny);
            if (c < 0)
                continue;
            ni = ny * w + nx;
            if (dist[cur] + c + 1 < dist[ni]) {    /* + 1: fewer steps at the same cost */
                if (dist[ni] == 1 << 30 && hn < size)
                    heap[hn++] = ni;
                dist[ni] = dist[cur] + c + 1;
                prev[ni] = cur;
            }
        }
    }
    for (i = ty * w + tx; i >= 0 && prev[i] != -1 && n < max; i = prev[i]) {
        px[n] = i % w;
        py[n] = i / w;
        n++;
    }
    if (n && n < max) {
        px[n] = units[unit].x;
        py[n] = units[unit].y;
        n++;
    }
    free(dist);
    return n;
}

static void tile_center(MapView *m, int x, int y, int *sx, int *sy)
{
    m->tile_to_screen((iptr)sx, (iptr)sy, x, y);
    *sx += m->half_tile_width;
    *sy += m->half_tile_height;
}

/* Lines through the squares (x[i], y[i]) on the main map, in colour. */
static void draw_path(const int *x, const int *y, int n, int color)
{
    MapView *m = main_map();
    RECT clip;
    int i;
    SetRect(&clip, m->client_x, m->client_y, m->client_x + m->client_width, m->client_y + m->client_height);
    ((DibSurface *)m)->set_clip(&clip);
    for (i = 0; i + 1 < n; i++) {
        int x1, y1, x2, y2;
        tile_center(m, x[i], y[i], &x1, &y1);
        tile_center(m, x[i + 1], y[i + 1], &x2, &y2);
        if (abs(x2 - x1) > m->client_width / 2)
            continue;               /* across the map's wrap */
        gfx_draw_line_color((iptr)m, x1, y1, x2, y2, 10);
        gfx_draw_line_color((iptr)m, x1, y1 - 1, x2, y2 - 1, color);
    }
    ((DibSurface *)m)->reset_clip();
    ((DibSurface *)m)->wnd_blit_surface_rect((iptr)&clip);
}

/* The squares from city to city2 where a road gives their trade route the
   bonus: the way the game follows when it looks for that road
   (map_road_connection: the path finder's, for road planning, from city
   toward city2, up to city2 or a city of city's owner; on one landmass, less
   than 23 squares apart).  n of them, 0 if no road can connect the two.
   *connected: whether the road is there (map_road_connection's answer).
   The path finder's state is the game's: put back as it was. */
static int trade_road_path(int city, int city2, int *px, int *py, int max, int *connected)
{
    static PathQueue saved_queue;
    static i8 saved_grid[0x2400];
    PathSearch saved_find = pathfind;
    int civ = cities[city].owner, x = cities[city].x, y = cities[city].y;
    int tx = cities[city2].x, ty = cities[city2].y, n = 0, reached = 0, dir;
    i8 *grid = (i8 *)&dialog_text_font_obj_block.path_cost_grid;
    *connected = 0;
    if (tile_land_landmass(x, y) != tile_land_landmass(tx, ty) || map_distance(tx, ty, x, y) >= 0x17)
        return 0;
    saved_queue = path_queue;
    memcpy(saved_grid, grid, sizeof saved_grid);
    pathfind.debug_pending = 0;
    *connected = map_road_connection(civ, x, y, tx, ty) != 0;
    pathfind.road_planning = 1;
    pathfind.owner_civ = -1;
    pathfind.unit_type = 2;
    pathfind.roads_only = 1;
    path_queue.goal_x = tx;
    path_queue.goal_y = ty;
    px[n] = x;
    py[n] = y;
    n++;
    while (n < max && n <= 0x34) {
        dir = map_path_next_dir(x, y, 99);
        if (dir < 0 || dir == 8)
            break;
        x = map_wrap_x(dir_dx_table_block.dir_dx_table[dir] + x);
        y += dir_dy_table_block.dir_dy_table[dir];
        if (!map_in_bounds(x, y))
            break;
        px[n] = x;
        py[n] = y;
        n++;
        if ((x == tx && y == ty) || tile_city_owner(x, y) == civ) {
            reached = 1;
            break;
        }
    }
    path_queue = saved_queue;
    memcpy(grid, saved_grid, sizeof saved_grid);
    pathfind = saved_find;
    return reached ? n : 0;
}

int extras_path_input(const MSG *m, HWND under, POINT pt)
{
    HWND map_hwnd = frame_hwnd(main_map());
    int x, y, unit = game.active_unit, city, target;
    POINT c = pt;
    static int px[512], py[512];
    if (m->message == WM_KEYUP && m->wParam == VK_SHIFT) {
        path_hide();
        return 0;
    }
    if (m->message != WM_MOUSEMOVE && !(m->message == WM_KEYDOWN && m->wParam == VK_SHIFT))
        return 0;
    if (GetKeyState(VK_SHIFT) >= 0 || !map_hwnd || !within(under, map_hwnd) || turn_loop.active == '\0') {
        path_hide();
        return 0;
    }
    ScreenToClient(map_hwnd, &c);
    if (main_map()->screen_to_tile((iptr)&x, (iptr)&y, c.x, c.y) != 0 || !map_in_bounds(x, y)) {
        path_hide();
        return 0;
    }
    if (path.shown && path.x == x && path.y == y)
        return 0;
    path_hide();
    city = city_at(map_cursor.x[0], map_cursor.y[0]);
    target = city_at(x, y);
    if (moving_units_mode != 1 && city >= 0 && cities[city].owner == human_civ[0] && target >= 0 &&
        target != city && (cities[target].owner == human_civ[0] || game.reveal_map != '\0' ||
                           (cities[target].known_to & (1 << human_civ[0])))) {
        /* the city under the cursor (View Pieces), the pointer over another:
           where a road between them gives their trade routes the bonus */
        int connected, n = trade_road_path(city, target, px, py, 512, &connected);
        if (n < 2)
            return 0;
        draw_path(px, py, n, connected ? 0x72 : 0x29);
    } else if (moving_units_mode != 1 && city >= 0 && cities[city].owner == human_civ[0] &&
        x == cities[city].x && y == cities[city].y) {
        /* the city under the cursor (View Pieces): its trade routes */
        int r;
        for (r = 0; r < cities[city].trade_routes && r < 3; r++) {
            int p = cities[city].route_partner[r], lx[2], ly[2];
            if (p < 0 || cities[p].id == 0)
                continue;
            lx[0] = cities[city].x;
            ly[0] = cities[city].y;
            lx[1] = cities[p].x;
            ly[1] = cities[p].y;
            draw_path(lx, ly, 2, 0x72);
        }
    } else if (moving_units_mode == 1 && unit >= 0 && units[unit].id != 0 && units[unit].owner == human_civ[0]) {
        int n = unit_path(unit, x, y, px, py, 512);
        if (n < 2)
            return 0;
        draw_path(px, py, n, 0x29);
    } else
        return 0;
    path.shown = 1;
    path.x = x;
    path.y = y;
    return 0;
}

/* ---- The rule changes (extra_rule) ---- */

static int human_unit(int unit)
{
    return unit >= 0 && unit < game.num_units && units[unit].id != 0 &&
        ((1 << (units[unit].owner & 0x1f)) & (int)game.human_players) != 0;
}

/* X19: where the unit last activated stood when it was (by the player or
   as the next unit): the next unit is looked for near there, not where the
   unit has moved to, so that the turn goes round the units near each other. */
static struct { int x, y, civ, turn; } rotation = {0, 0, -1, -1};

static void rotation_note(int unit)
{
    rotation.x = units[unit].x;
    rotation.y = units[unit].y;
    rotation.civ = units[unit].owner;
    rotation.turn = game.turn;
}

int extras_rotation_origin(int unit, int *x, int *y)
{
    if (!human_unit(unit) || rotation.civ != units[unit].owner || rotation.turn != game.turn)
        return 0;
    *x = rotation.x;
    *y = rotation.y;
    return 1;
}

void extras_rotation_found(int unit)
{
    if (human_unit(unit))
        rotation_note(unit);
}

/* The player activated a unit: the units of its stack come next (their
   Wait flags cleared), the civ's others wait. */
void extras_rotation_activated(int unit)
{
    int i;
    if (!human_unit(unit))
        return;
    for (i = 0; i < game.num_units; i++) {
        Unit *u = &units[i];
        if (u->id == 0 || u->owner != units[unit].owner)
            continue;
        if (u->x == units[unit].x && u->y == units[unit].y)
            u->flags &= ~UNITF_WAITING;
        else
            u->flags |= UNITF_WAITING;
    }
    rotation_note(unit);
}

/* X36: Shift+right click on (x, y): the active unit and the units of its
   type on its square that can move (no orders: not fortified, on sentry or
   working) go there, as with Go To; the others first, each as far as it
   gets this turn, then the active unit as by a Go To.  1 if done. */
int extras_mass_move(int x, int y)
{
    int active = game.active_unit, ux, uy, i, n = 0;
    static short movers[2048];
    if (!human_unit(active) || units[active].owner != human_civ[0] || moving_units_mode == 0)
        return 0;
    ux = units[active].x;
    uy = units[active].y;
    if (ux == x && uy == y)
        return 0;
    for (i = 0; i < game.num_units && n < 2048; i++)
        if (i != active && units[i].id != 0 && units[i].x == ux && units[i].y == uy &&
            units[i].owner == units[active].owner && units[i].type == units[active].type &&
            units[i].orders == ORDER_NONE && unit_is_selectable(i))
            movers[n++] = (short)i;
    for (i = 0; i < n; i++) {
        int unit = movers[i], id = units[unit].id, steps;
        units[unit].goto_x = (i16)x;
        units[unit].goto_y = (i16)y;
        units[unit].orders = ORDER_GOTO;
        units[unit].aux_timer = 0;
        for (steps = 0; steps < 64; steps++) {
            int px = units[unit].x, py = units[unit].y, used = units[unit].moves_used;
            game.active_unit = (i16)unit;
            unit_goto_step(unit);
            if (units[unit].id != id || units[unit].orders != ORDER_GOTO || !unit_is_selectable(unit))
                break;
            if (units[unit].x == px && units[unit].y == py && units[unit].moves_used == used)
                break;      /* stuck: it waits with its order */
        }
    }
    game.active_unit = (i16)active;
    if (units[active].id == 0)
        return 1;
    units[active].goto_x = (i16)x;
    units[active].goto_y = (i16)y;
    units[active].orders = ORDER_GOTO;
    units[active].aux_timer = 0;
    unit_goto_step(active);
    return 1;
}

/* X42: U with no land unit on sentry in the stack: its land units that can
   move (no orders) go on sentry.  1 if any did. */
int extras_stack_sentry(void)
{
    int unit, any = 0;
    if (game.active_unit < 0)
        return 0;
    for (unit = unit_stack_first(game.active_unit); unit >= 0; unit = unit_next_in_stack(unit))
        if (unit_types[units[unit].type].domain == DOMAIN_LAND && units[unit].orders == ORDER_NONE &&
            unit_is_selectable(unit)) {
            unit_set_sleep(unit);
            any = 1;
        }
    if (any)
        map_redraw_tile_all_views(units[game.active_unit].x, units[game.active_unit].y);
    return any;
}


/* ---- The options dialog (Game > OpenCiv2 Options) ---- */

/* The game's dialogs, built as the game builds those of GAME.TXT (a
   @checkbox section: checkboxes; text fields as @CUSTOMSIZE's), with our
   texts.  A change is written to CIV.INI and kept for this run
   (setting_changed: in replays too, where CIV.INI is not read). */

static int network_game(void)
{
    return game.multiplayer >= 3;
}

static void setting_write(const char *key, int value)
{
    char v[16];
    snprintf(v, sizeof v, "%d", value);
    WritePrivateProfileStringA("OpenCiv2", key, v, "CIV.INI");
    setting_changed(key, value);
}

/* A page of checkboxes, keys[n] by their bits in *bits: 1 if OK (*bits
   changed and the changes written). */
static int options_checks(const char *title, const char *note, const KeyItem *keys, int n, uint64_t *bits,
                          int columns)
{
    Dialog d;
    char k[64];
    int i, result;
    uint64_t now = *bits;
    d.Dialog::construct(0x4000);
    ON_SCOPE_EXIT(d.Dialog::destruct());
    d.init(0, 0, 0, 1);
    d.set_width(columns > 1 ? 600 : 400);
    d.set_title(title);
    if (note)
        d.add_line(note, -1, -1, -1, 0);
    d.set_columns(columns);
    for (i = 0; i < n; i++) {
        snprintf(k, sizeof k, "option.%s", keys[i].key);
        d.add_checkbox_option(extras_text(k), i, (int)((*bits >> keys[i].item) & 1));
    }
    result = d.show_modal(0);
    if (result != 0)
        return 0;
    for (i = 0; i < n; i++) {
        uint64_t bit = (uint64_t)1 << keys[i].item;
        int on = d.option_is_checked(i) != 0;
        if (on == ((now & bit) != 0))
            continue;
        now ^= bit;
        setting_write(keys[i].key, on);
    }
    *bits = now;
    return 1;
}

/* A page of numbers (and text): keys[n], each shown as shown[i]; those
   changed written.  Returns 1 if OK (value[i]: the numbers, text: the last
   field's text if is_text). */
static int options_fields(const char *title, const char *note, const char *const *keys, int n,
                          char shown[][256], int is_text, int *value, int *changed_field)
{
    Dialog d;
    char k[64];
    int i, result;
    d.Dialog::construct(0x4000);
    ON_SCOPE_EXIT(d.Dialog::destruct());
    d.init(0, 0, 0, 1);
    d.set_width(400);
    d.set_title(title);
    if (note)
        d.add_line(note, -1, -1, -1, 0);
    for (i = 0; i < n; i++) {
        snprintf(k, sizeof k, "option.%s", keys[i]);
        d.add_edit_field(extras_text(k), (iptr)shown[i], is_text && i == n - 1 ? 255 : 7);
    }
    result = d.show_modal(0);
    if (result < 0)
        return 0;
    /* the fields' texts: as the game's @CUSTOMSIZE, in the text arguments */
    for (i = 0; i < n; i++) {
        const char *t = (const char *)&text_args.strings + i * 0x104;
        changed_field[i] = strcmp(t, shown[i]) != 0;
        value[i] = atoi(t);
        snprintf(shown[i], 256, "%s", t);
    }
    return 1;
}

static void options_limits(void)
{
    static const char *const keys[5] = {
        "RetirementYear", "RetirementWarningYear", "PopulationLimit", "GoldLimit", "MapTilesLimit",
    };
    static const int items[5] = {X_RETIREMENT, -1, X_POP_LIMIT, X_GOLD_LIMIT, X_MAP_TILES};
    static const int original[5] = {2020, 2000, 32000, 30000, 10000};
    char shown[5][256];
    int value[5], changed_field[5], i, year, warn;
    /* the limits in force: the original's while a rule is off */
    int now[5] = {extras_retirement_year(), extras_retirement_warning_year(), extras_population_limit(),
                  extras_gold_limit(), extras_map_tiles_limit()};
    for (i = 0; i < 5; i++)
        snprintf(shown[i], 256, "%d", now[i]);
    if (!options_fields(extras_text("options.limits"), NULL, keys, 5, shown, 0, value, changed_field))
        return;
    if (changed_field[0] || changed_field[1]) {
        year = value[0];
        warn = value[1];
        if (!changed_field[1] && now[1] == now[0] - 20)
            warn = year - 20;  /* a warning 20 years before moves with the year */
        if (year == original[0] && warn == original[1]) {
            setting_write(keys[0], 0);
            extras_rules &= ~((uint64_t)1 << X_RETIREMENT);
        } else {
            setting_write(keys[0], year);
            setting_write(keys[1], warn);
            extras_rules |= (uint64_t)1 << X_RETIREMENT;
        }
    }
    /* a limit set back to the original's (or 0) turns its rule off */
    for (i = 2; i < 5; i++) {
        if (!changed_field[i])
            continue;
        if (value[i] == original[i])
            value[i] = 0;
        setting_write(keys[i], value[i]);
        if (value[i])
            extras_rules |= (uint64_t)1 << items[i];
        else
            extras_rules &= ~((uint64_t)1 << items[i]);
    }
    limits_load();
}

static void options_other(void)
{
    static const char *const keys[6] = {"Scale", "FullScreen", "Gamma", "Brightness", "Contrast", "SuppressList"};
    char shown[6][256];
    int value[6], changed_field[6], i;
    /* Scale and FullScreen: what is in force (src/platform/main.c), read at
       start; Scale 0: by the size */
    snprintf(shown[0], 256, "%d", plat_scale);
    snprintf(shown[1], 256, "%d", plat_fullscreen);
    snprintf(shown[2], 256, "%d", extras_setting("Gamma", 100));
    snprintf(shown[3], 256, "%d", extras_setting("Brightness", 0));
    snprintf(shown[4], 256, "%d", extras_setting("Contrast", 0));
    suppress_list_get(shown[5], 256);
    if (!options_fields(extras_text("options.other"), extras_text("options.other_note"), keys, 6, shown, 1,
                        value, changed_field))
        return;
    if (changed_field[0])
        setting_write(keys[0], value[0] < 0 || value[0] > 8 ? 0 : value[0]);
    if (changed_field[1])
        setting_write(keys[1], value[1] != 0);
    for (i = 2; i < 5; i++)
        if (changed_field[i])
            setting_write(keys[i], value[i]);
    if (changed_field[5]) {
        WritePrivateProfileStringA("OpenCiv2", "SuppressList", shown[5], "CIV.INI");
        snprintf(suppress_changed, sizeof suppress_changed, "%s", shown[5]);
        suppress_was_changed = 1;
    }
}

/* What the interface additions changed shows at once: the menu (X38), the
   colours (X35), the map and the status panel (X3, X5, X41 ...). */
static void options_apply(uint64_t before)
{
    GameMenu *menu = (GameMenu *)((i8 *)&main_menu_block.main_menu);
    color_correction_load();
    if (((before ^ extras_enabled) >> X_ARRANGE) & 1) {
        if (!extra_ui(X_ARRANGE))
            extras_set_panel_size(0);
        menu->set_item_hidden(CMD_ARRANGE_WINDOWS_S, !extra_ui(X_ARRANGE));
        menu->set_item_hidden(CMD_ARRANGE_WINDOWS_L, !extra_ui(X_ARRANGE));
        menu->rebuild(0);
    }
    tip_hide();
    path_hide();
    if (turn_loop.active != '\0') {
        map_redraw_all_views(human_civ[0], 1);
        status_draw_info(1);
    }
}

void extras_options_dialog(void)
{
    int choice = 0;
    if (!plat_extras)
        return;
    for (;;) {
        uint64_t before = extras_enabled;
        {
            Dialog d;
            d.Dialog::construct(0x4000);
            ON_SCOPE_EXIT(d.Dialog::destruct());
            d.init(0, 0, 0, 1);
            d.set_width(320);
            d.set_title(extras_text("options.title"));
            if (network_game())
                d.add_line(extras_text("options.network"), -1, -1, -1, 0);
            d.add_option(extras_text("options.interface"), 0, 0);
            d.add_option(extras_text("options.rules"), 1, 0);
            d.add_option(extras_text("options.limits"), 2, 0);
            d.add_option(extras_text("options.other"), 3, 0);
            if (network_game()) {
                /* the rules are the original's in network games (extra_rule) */
                d.set_option_disabled(1, 1);
                d.set_option_disabled(2, 1);
                if (choice == 1 || choice == 2)
                    choice = 0;
            }
            d.set_default_choice(choice);
            choice = d.show_modal(0);
        }
        if (choice < 0)
            return;
        if (choice == 0) {
            if (options_checks(extras_text("options.interface"), 0, extra_keys,
                               (int)(sizeof extra_keys / sizeof extra_keys[0]), &extras_enabled, 2))
                options_apply(before);
        } else if (choice == 1 && !network_game()) {
            options_checks(extras_text("options.rules"), extras_text("options.rules_note"), rule_keys, 5,
                           &extras_rules, 1);
        } else if (choice == 2 && !network_game()) {
            options_limits();
        } else if (choice == 3) {
            options_other();
            options_apply(before);
        }
    }
}
