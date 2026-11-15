/*
 * (port) The OpenCiv2 Options dialog (docs/EXTRAS.md, "OpenCiv2 Options"):
 * the main menu's last choice, after View Credits, in Standard and Classic
 * mode alike, where the player chooses how to play.  The game's own
 * dialogs, built as it builds those of GAME.TXT: a list of what can be
 * chosen, each opening a dialog of its own with OK and Cancel (as the Game
 * menu's options):
 *
 *   Mode                 Standard (the original's bugs fixed, the interface
 *                        additions) or Classic (the original game, as
 *                        CIV2_CLASSIC=1);
 *   Graphics and screen  sharp text and interface (on by default but in
 *                        Classic), HD graphics (docs/HD-GRAPHICS.md: the
 *                        art downloaded from here where it is missing,
 *                        src/host/host_options.c; in the browser the page
 *                        gets it from its site), full screen;
 *   Rule changes, Limits (EXTRAS.md "Rule changes");
 *   Interface additions, Colours and messages (the colour correction, the
 *                        messages shown on the map).
 *
 * OK writes what changed to CIV.INI's [OpenCiv2] section.  The rule changes
 * and the interface additions take effect at once (extras_load: they are
 * read again); the mode, the graphics and full screen when the game starts
 * again, which it then offers to do (plat_restart: the program started
 * again natively, the page loaded again in the browser).
 *
 * Not in replays or the unit tests (the main menu is compared with the
 * original's), but with CIV2_TEST_OPTIONS=1 in a replay
 * (harness/extras/x-options.txt): then nothing is written to CIV.INI,
 * downloaded or restarted; the choices hold for that run
 * (options_test_setting, which extras_setting reads first).
 */
#include "game.h"

EXTERN_C int plat_replay;

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
    {"HotkeyUnderlines", "Underline options' hotkeys"},
};
#define NEXTRAS ((int)(sizeof extras / sizeof extras[0]))

static const struct { const char *key, *label; int def; } colours[NCOLOR] = {
    {"Gamma", "Gamma (%)", 100}, {"Brightness", "Brightness", 0}, {"Contrast", "Contrast", 0},
    {"SuppressList", "Messages on the map", 0},
};

typedef struct {
    int classic, hd, fullscreen;
    int sharp, sharp_set;           /* sharp text and interface; SharpText= set (else: not in Classic) */
    int rule[NRULES], extra[NEXTRAS];
    char limit[NLIMITS][16], colour[NCOLOR][256];
} Choices;

/* The choices made in a test run (a replay): the game reads them instead of
   CIV.INI (options_test_setting). */
static struct {
    char key[32], value[256];
} set[64];
static int set_count;

int options_test_setting(const char *key, char *out, int n)
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
    if (!options_test_setting(key, out, n) && !plat_replay)
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
    c->classic = !__strcmpi(v, "Classic");
    get("Graphics", v, sizeof v);
    c->hd = !__strcmpi(v, "HD");
    get("SharpText", v, sizeof v);
    c->sharp_set = v[0] != 0;
    c->sharp = c->sharp_set ? atoi(v) != 0 : !c->classic;
    c->fullscreen = get_int("FullScreen", 0) != 0;
    for (i = 0; i < NRULES; i++)
        c->rule[i] = get_int(rules[i].key, 0) != 0;
    for (i = 0; i < NEXTRAS; i++)
        c->extra[i] = get_int(extras[i].key, strcmp(extras[i].key, "HotkeyUnderlines") != 0) != 0;
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
    if (c->sharp_set && (c->sharp != old->sharp || !old->sharp_set))
        put_int("SharpText", c->sharp);
    if (c->fullscreen != old->fullscreen)
        put_int("FullScreen", c->fullscreen);
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

/* ---- the main menu's entry ---- */

int options_in_main_menu(void)
{
    const char *t = getenv("CIV2_TEST_OPTIONS");
    if (plat_replay)
        return t && *t && *t != '0';
    return !plat_unit_test;
}

/* ---- the dialogs ---- */

/* Started with: what needs a restart is compared with it. */
static Choices at_start;
static int at_start_read;

/* What the environment or the command line set wins over the choices
   (src/platform/main.c, hd.c): a line saying so, for which (1 mode, 2 the
   graphics), "" if none. */
static void overrides(int which, char *out, int n)
{
    const char *c = getenv("CIV2_CLASSIC"), *a = getenv("CIV2_HD_ARG"), *e = getenv("CIV2_HD");
    const char *sa = getenv("CIV2_SHARP_ARG"), *se = getenv("CIV2_SHARP");
    const char *what[3];
    int k, count = 0;
    size_t len;
    if (plat_replay) {
        out[0] = 0;             /* (tests: as without) */
        return;
    }
    if (c && *c)
        what[count++] = "CIV2_CLASSIC";
    if (which == 2 && ((a && *a) || (e && *e)))
        what[count++] = a && *a ? "--hd" : "CIV2_HD";
    if (which == 2 && ((sa && *sa) || (se && *se)))
        what[count++] = sa && *sa ? "--sharp" : "CIV2_SHARP";
    out[0] = 0;
    for (k = 0; k < count; k++) {
        len = strlen(out);
        snprintf(out + len, (size_t)n - len, "%s%s", k ? (k == count - 1 ? " and " : ", ") : "", what[k]);
    }
    len = strlen(out);
    if (count)
        snprintf(out + len, (size_t)n - len, " set: for this run %s over these choices.",
                 count > 1 ? "they win" : "it wins");
}

/* A dialog as the game's: its title, its width, Cancel too. */
static void dialog_begin(Dialog &d, const char *title, int width)
{
    d.init(0, 0, 0, 1);
    d.set_width((u32)width);
    d.set_title(title);
}

/* A paragraph (wrapped to the dialog's width), after the one before. */
static void paragraph(Dialog &d, const char *text, int first)
{
    if (!first)
        d.add_line("^", -1, -1, -1, 0);
    d.add_line(text, -1, -1, -1, 0);
}

/* A message, with OK. */
static void message(const char *title, const char *text)
{
    Dialog d(0x4000);
    d.init(0, 0, 0, 0);
    d.set_width(400);
    d.set_title(title);
    d.add_line(text, -1, -1, -1, 0);
    d.show_modal(0);
}

/* Mode: Standard or Classic.  1 if changed. */
static int page_mode(Choices *c)
{
    char note[200];
    int choice;
    Dialog d(0x4000);
    dialog_begin(d, "Mode", 440);
    paragraph(d, "Standard: the original's bugs fixed, with OpenCiv2's interface additions; rule changes as "
                 "chosen.", 1);
    paragraph(d, "Classic: the original game as it was: its bugs, its look, none of OpenCiv2's additions.", 0);
    paragraph(d, "From the next start of the game.", 0);
    overrides(1, note, sizeof note);
    if (note[0])
        paragraph(d, note, 0);
    d.add_option("Standard", 0, 0);
    d.add_option("Classic", 1, 0);
    d.set_default_choice(c->classic);
    choice = d.show_modal(0);
    if (choice < 0 || choice == c->classic)
        return 0;
    c->classic = choice;
    if (!c->sharp_set)
        c->sharp = !c->classic;     /* (by default: not in Classic) */
    return 1;
}

/* The art's download, shown with its progress and Stop. */
static struct {
    int state, info[3];
    char text[96], msg[512];
} fetch;

static void fetch_text(void)
{
    static const char *const doing[3] = {"Downloading the art", "Checking it", "Installing it"};
    int phase = fetch.info[0] < 0 || fetch.info[0] > 2 ? 0 : fetch.info[0];
    int done = fetch.info[1], total = fetch.info[2] > 0 ? fetch.info[2] : 1;
    if (done > total)
        done = total;
    snprintf(fetch.text, sizeof fetch.text, "%s: %d of %d MB", doing[phase], (int)(done * 1024LL / 1000000),
             (int)(total * 1024LL / 1000000));
}

/* (polled while the progress dialog is shown: 1 ends it) */
static i32 fetch_poll(Dialog *d)
{
    static u32 last;
    u32 now;
    if (!win_dispatch_one_message())
        Sleep(10);
    now = GetTickCount();
    if (now - last < 250)
        return 0;
    last = now;
    fetch.state = plat_hd_art(PLAT_HD_POLL, fetch.info, fetch.msg, sizeof fetch.msg);
    if (fetch.state != PLAT_HD_RUNNING)
        return 1;
    fetch_text();
    if (d->set_item_text(1, fetch.text))
        d->render();
    return 0;
}

/* The art downloaded and installed (natively; src/host/host_options.c):
   1 if it is there. */
static int fetch_art(void)
{
    fetch.state = plat_hd_art(PLAT_HD_START, fetch.info, fetch.msg, sizeof fetch.msg);
    if (fetch.state == PLAT_HD_RUNNING) {
        Dialog d(0x4000);
        d.init(0, 0, 0, 0x2000000);
        d.set_width(400);
        d.set_title("HD Graphics");
        fetch_text();
        d.add_line(fetch.text, -1, -1, 80, 0);
        d.add_button("Stop");
        d.on_modal_poll = fetch_poll;
        d.show_modal(0);
        if (fetch.state == PLAT_HD_RUNNING) {
            /* (Stop) */
            plat_hd_art(PLAT_HD_CANCEL, NULL, NULL, 0);
            return 0;
        }
    }
    if (fetch.state == PLAT_HD_DONE)
        return 1;
    {
        char s[600];
        snprintf(s, sizeof s, "%s HD graphics stay off.", fetch.msg[0] ? fetch.msg : "The download failed.");
        message("HD Graphics", s);
    }
    return 0;
}

/* Graphics and screen: sharp text and interface, HD graphics, full screen.
   1 if changed. */
static int page_graphics(Choices *c)
{
    char art[300], note[200];
    int info[3] = {0, 0, 0}, state, mb, browser = plat_restart(0) == 2, result, hd_offered;
    Choices was = *c;
    Dialog d(0x4000);
    state = plat_hd_art(PLAT_HD_POLL, info, art, sizeof art);
    mb = (int)((info[2] > 0 ? info[2] : 228595) * 1024LL / 1000000);
    dialog_begin(d, "Graphics and Screen", 480);
    paragraph(d, "Sharp text and interface: text, menus and controls at the screen's full resolution (the "
                 "pictures enlarged).", 1);
    if (state == PLAT_HD_DONE)
        paragraph(d, "HD graphics: new art for the map, at the screen's full resolution. The art is installed.", 0);
    else if (state == PLAT_HD_LATER) {
        snprintf(note, sizeof note, "HD graphics: new art for the map, at the screen's full resolution. The art "
                 "(%d MB) is downloaded from this site when the game starts again.", mb);
        paragraph(d, note, 0);
    } else if (state == PLAT_HD_FAILED && browser) {
        paragraph(d, "HD graphics: new art for the map, at the screen's full resolution. This site was built "
                     "without the art.", 0);
    } else {
        snprintf(note, sizeof note, "HD graphics: new art for the map, at the screen's full resolution. They "
                 "need the art: %d MB, downloaded once from openciv2-assets on GitHub when chosen.", mb);
        paragraph(d, note, 0);
    }
    if (!browser)
        paragraph(d, "Full screen: instead of a window filling the screen.", 0);
    paragraph(d, "From the next start of the game.", 0);
    overrides(2, note, sizeof note);
    if (note[0])
        paragraph(d, note, 0);
    hd_offered = !c->classic && !(state == PLAT_HD_FAILED && browser);
    d.add_checkbox_option("Sharp text and interface", 0, c->sharp);
    d.add_checkbox_option("HD graphics", 1, c->hd && hd_offered);
    if (!browser)
        d.add_checkbox_option("Full screen", 2, c->fullscreen);
    if (!hd_offered)
        d.set_option_disabled(1, 1);
    result = d.show_modal(0);
    if (result != 0)
        return 0;
    if ((d.option_is_checked(0) != 0) != c->sharp) {
        c->sharp = d.option_is_checked(0) != 0;
        c->sharp_set = 1;
    }
    if (hd_offered) {
        int hd = d.option_is_checked(1) != 0;
        /* (natively the art downloaded now, where it is missing; in the
           browser the page gets it from its site at the restart) */
        if (hd && !c->hd && state != PLAT_HD_DONE && state != PLAT_HD_LATER)
            hd = fetch_art();
        c->hd = hd;
    }
    if (!browser)
        c->fullscreen = d.option_is_checked(2) != 0;
    return memcmp(&was, c, sizeof was) != 0;
}

/* Rule changes. */
static void page_rules(Choices *c)
{
    int i;
    Dialog d(0x4000);
    dialog_begin(d, "Rule Changes", 480);
    paragraph(d, "Changes to the original's rules, for the human players' units; the original's rules in "
                 "network games.", 1);
    for (i = 0; i < NRULES; i++)
        d.add_checkbox_option(rules[i].label, i, c->rule[i]);
    if (d.show_modal(0) != 0)
        return;
    for (i = 0; i < NRULES; i++)
        c->rule[i] = d.option_is_checked(i) != 0;
}

/* Edit fields: their texts as the game's @CUSTOMSIZE has them (in the text
   arguments).  1 if OK. */
static int fields(Dialog &d, char (*values)[256], const int *sizes, int n)
{
    int i;
    if (d.show_modal(0) < 0)
        return 0;
    for (i = 0; i < n; i++)
        snprintf(values[i], (size_t)sizes[i], "%s", text_args.strings[i]);
    return 1;
}

/* Limits. */
static void page_limits(Choices *c)
{
    char values[NLIMITS][256];
    int sizes[NLIMITS], i;
    Dialog d(0x4000);
    dialog_begin(d, "Limits", 400);
    paragraph(d, "At the original's values: as the original.", 1);
    for (i = 0; i < NLIMITS; i++) {
        d.add_edit_field(limits[i].label, c->limit[i], 8);
        sizes[i] = 16;
    }
    if (!fields(d, values, sizes, NLIMITS))
        return;
    for (i = 0; i < NLIMITS; i++)
        snprintf(c->limit[i], 16, "%s", values[i]);
}

/* Interface additions. */
static void page_interface(Choices *c)
{
    int i;
    Dialog d(0x4000);
    dialog_begin(d, "Interface Additions", 600);
    paragraph(d, "OpenCiv2's additions to the interface (after FoxAhead's CIV2UIA): what you see and do.", 1);
    d.set_columns(2);
    for (i = 0; i < NEXTRAS; i++)
        d.add_checkbox_option(extras[i].label, i, c->extra[i]);
    if (d.show_modal(0) != 0)
        return;
    for (i = 0; i < NEXTRAS; i++)
        c->extra[i] = d.option_is_checked(i) != 0;
}

/* Colours and messages. */
static void page_colours(Choices *c)
{
    char values[NCOLOR][256];
    int sizes[NCOLOR], i;
    Dialog d(0x4000);
    dialog_begin(d, "Colours and Messages", 440);
    paragraph(d, "Gamma: a percentage, 100 means no change; brightness and contrast: -100 to 100. Messages on "
                 "the map: GAME.TXT sections, e.g. HIDDENTERRAIN,CITYSIZE.", 1);
    for (i = 0; i < NCOLOR; i++) {
        d.add_edit_field(colours[i].label, c->colour[i], i == C_SUPPRESS ? 255 : 7);
        sizes[i] = i == C_SUPPRESS ? 256 : 16;
    }
    if (!fields(d, values, sizes, NCOLOR))
        return;
    for (i = 0; i < NCOLOR; i++)
        snprintf(c->colour[i], 256, "%s", values[i]);
}

/* Whether a choice that needs a restart differs from what the game started
   with: then the restart offered (natively the program started again, in
   the browser the page loaded again). */
static void offer_restart(const Choices *c)
{
    int how, choice;
    if (c->classic == at_start.classic && c->hd == at_start.hd && c->sharp == at_start.sharp &&
        c->fullscreen == at_start.fullscreen)
        return;
    how = plat_restart(0);
    if (!how) {
        message("Restart", "These choices take effect the next time the game starts.");
        return;
    }
    {
        Dialog d(0x4000);
        dialog_begin(d, "Restart", 400);
        paragraph(d, how == 2 ? "These choices take effect when the game starts again: restart it now (the page "
                                "is loaded again)?"
                              : "These choices take effect when the game starts again: restart it now?", 1);
        d.add_option("Restart now", 0, 0);
        d.add_option("Later", 1, 0);
        choice = d.show_modal(0);
    }
    if (choice == 0 && !plat_restart(1) && !plat_replay)
        message("Restart", "The game could not be started again: these choices take effect the next time it starts.");
}

enum { P_MODE, P_GRAPHICS, P_RULES, P_LIMITS, P_INTERFACE, P_COLOURS };

void options_dialog(void)
{
    int choice = P_MODE;
    if (!at_start_read) {
        read_choices(&at_start);
        at_start_read = 1;
    }
    for (;;) {
        Choices now, was;
        read_choices(&now);
        was = now;
        {
            Dialog d(0x4000);
            dialog_begin(d, "OpenCiv2 Options", 360);
            d.add_line("Kept in CIV.INI, under [OpenCiv2].", -1, -1, -1, 0);
            d.add_option("Mode", P_MODE, 0);
            d.add_option("Graphics and screen", P_GRAPHICS, 0);
            d.add_option("Rule changes", P_RULES, 0);
            d.add_option("Limits", P_LIMITS, 0);
            d.add_option("Interface additions", P_INTERFACE, 0);
            d.add_option("Colours and messages", P_COLOURS, 0);
            if (now.classic) {
                /* (the original game: none of these) */
                int i;
                for (i = P_RULES; i <= P_COLOURS; i++)
                    d.set_option_disabled(i, 1);
                if (choice >= P_RULES)
                    choice = P_MODE;
            }
            d.set_default_choice(choice);
            choice = d.show_modal(0);
        }
        if (choice < 0)
            return;
        switch (choice) {
        case P_MODE:
            if (page_mode(&now)) {
                write_choices(&now, &was);
                offer_restart(&now);
            }
            break;
        case P_GRAPHICS:
            if (page_graphics(&now)) {
                write_choices(&now, &was);
                offer_restart(&now);
            }
            break;
        case P_RULES:
            page_rules(&now);
            break;
        case P_LIMITS:
            page_limits(&now);
            break;
        case P_INTERFACE:
            page_interface(&now);
            break;
        case P_COLOURS:
            page_colours(&now);
            break;
        }
        if (choice >= P_RULES) {
            /* at once: the additions and the rules read again */
            write_choices(&now, &was);
            extras_load();
        }
    }
}
