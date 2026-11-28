/*
 * (port) The Settings dialog (docs/EXTRAS.md, "Settings"):
 * the main menu's last choice, after View Credits, where the player chooses
 * how to play.  The game's own dialogs, built as it builds those of
 * GAME.TXT: a list of what can be chosen, each opening a dialog of its own
 * with OK and Cancel (as the Game menu's options):
 *
 *   Bug fixes            "Fix the original game's bugs" (docs/BUGS.md:
 *                        FixBugs=, on by default; each fix's own Fix.B<n>=
 *                        in CIV.INI wins over it);
 *   Graphics and screen  "Classic graphics" (the original's pictures;
 *                        unticked: HD graphics, docs/HD-GRAPHICS.md; the
 *                        default HD_BY_DEFAULT's, src/platform/options.c),
 *                        sharp text and interface (on by default), full
 *                        screen;
 *   Rule changes, Limits (EXTRAS.md "Rule changes");
 *   Interface additions, Colours and messages (the colour correction, the
 *                        messages shown on the map).
 *   Language             of OpenCiv2's own texts (src/include/lang.h,
 *                        docs/TRANSLATING.md): Auto, English, the data
 *                        directory's translations.
 *
 * Where the game folder lacks the original's pictures (docs/NO-INSTALL.md:
 * plat_graphics_forced) Classic graphics are unticked, with a note saying
 * why ("graphics.need_original"); ticked, a dialog offers to find the
 * original's files (plat_add_original: the set-up's, natively; the set-up
 * page's in the browser): found, Classic graphics from the next start (the
 * restart offered); else HD graphics stay, with a note.
 *
 * Its texts are keyed (src/host/texts.h: extras_text).
 *
 * OK writes what changed to CIV.INI's [OpenCiv2] section.  The bug fixes,
 * the rule changes and the interface additions take effect at once
 * (fixes_load, extras_load: they are read again); the graphics, full screen
 * and the language when the game starts again, which it then offers to do
 * (plat_restart: the program started again natively, the page loaded again
 * in the browser).
 *
 * Not in replays or the unit tests (the main menu is compared with the
 * original's), but with CIV2_TEST_OPTIONS=1 in a replay
 * (harness/extras/x-options.txt): then nothing is written to CIV.INI,
 * downloaded or restarted; the choices hold for that run
 * (options_test_setting, which extras_setting reads first).
 */
#include "game.h"
#include "lang.h"

EXTERN_C int plat_replay;
EXTERN_C void test_main_menu_shown(void);       /* src/platform/replay.c */

/* ---- the settings ---- */

enum { R_GOTO, R_MASS_MOVE, R_HANDOVER, R_ROTATION, R_SENTRY, NRULES };
enum { L_RETIRE, L_WARN, L_POP, L_GOLD, L_MAP, NLIMITS };
enum { C_GAMMA, C_BRIGHT, C_CONTRAST, C_SUPPRESS, NCOLOR };

/* The choices by their CIV.INI keys; their labels are the texts
   "rule.<key>", "limit.<key>" ... (src/host/texts.h). */

/* The rule changes (src/game/extras.cpp: rule_keys). */
static const struct { const char *key; } rules[NRULES] = {
    {"GotoNoZocStop"}, {"MassMove"}, {"WorkerHandover"}, {"UnitRotation"}, {"StackSentry"},
};

/* The limits: the original's value (shown while off) and the range. */
static const struct { const char *key; int original, lo, hi; } limits[NLIMITS] = {
    {"RetirementYear", 2020, 1, 9999},
    {"RetirementWarningYear", 2000, 0, 9998},
    {"PopulationLimit", 32000, 32000, 3275800},
    {"GoldLimit", 30000, 30000, 1000000},
    {"MapTilesLimit", 10000, 10000, 32767},
};

/* The interface additions (src/game/extras.cpp: extra_keys), in its order. */
static const struct { const char *key; } extras[] = {
    {"MouseWheel"}, {"UnitListScroll"}, {"UnitListKeys"}, {"WorkCounter"}, {"GameTurn"},
    {"ResearchNumbers"}, {"ZeroUpkeep"}, {"SupportScroll"}, {"SupportSort"}, {"AttitudeColors"},
    {"CityAttitude"}, {"RadioHotkeys"}, {"Quickinfo"}, {"ProductionCancel"}, {"ProductionCost"},
    {"CityStatus"}, {"MassSpecialists"}, {"SuppressPopups"}, {"ColorCorrection"}, {"ForeignGold"},
    {"ArrangeWindowsSL"}, {"PathLines"}, {"SentryHalftone"}, {"DefaultZoom1"}, {"HotkeyUnderlines"},
};
#define NEXTRAS ((int)(sizeof extras / sizeof extras[0]))

static const struct { const char *key; int def; } colours[NCOLOR] = {
    {"Gamma", 100}, {"Brightness", 0}, {"Contrast", 0}, {"SuppressList", 0},
};

typedef struct {
    int fix;                        /* FixBugs=: the original's bugs fixed */
    int hd, fullscreen;             /* hd: Classic graphics unticked */
    int sharp, sharp_set;           /* sharp text and interface; SharpText= set (else: on) */
    int rule[NRULES], extra[NEXTRAS];
    char limit[NLIMITS][16], colour[NCOLOR][256];
    char language[16];              /* Language=: a code, "" (or Auto) the system's */
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
    c->fix = get_int("FixBugs", 1) != 0;
    /* (without the original's pictures HD graphics whatever is chosen;
       plat_graphics_hd: the default where Graphics= is unset) */
    get("Graphics", v, sizeof v);
    c->hd = plat_graphics_forced() || plat_graphics_hd(v);
    get("SharpText", v, sizeof v);
    c->sharp_set = v[0] != 0;
    c->sharp = c->sharp_set ? atoi(v) != 0 : 1;
    c->fullscreen = get_int("FullScreen", 0) != 0;
    get("Language", c->language, sizeof c->language);
    if (!__strcmpi(c->language, "Auto"))
        c->language[0] = 0;
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
    if (c->fix != old->fix)
        put_int("FixBugs", c->fix);
    if (c->hd != old->hd)
        put("Graphics", c->hd ? "HD" : "Classic");
    if (c->sharp_set && (c->sharp != old->sharp || !old->sharp_set))
        put_int("SharpText", c->sharp);
    if (c->fullscreen != old->fullscreen)
        put_int("FullScreen", c->fullscreen);
    if (strcmp(c->language, old->language))
        put("Language", c->language[0] ? c->language : "Auto");
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
    if (!plat_unit_test)
        test_main_menu_shown();     /* (CIV2_TEST_QUIT_AT_MENU: the release's smoke test) */
    return !plat_unit_test;
}

/* ---- the dialogs ---- */

/* Started with: what needs a restart is compared with it. */
static Choices at_start;
static int at_start_read;

/* What the environment or the command line set wins over the choices
   (src/platform/main.c, hd.c): a line saying so, for which (1 the bug
   fixes, 2 the graphics), "" if none. */
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
        snprintf(out + len, (size_t)n - len, "%s%s", k ? (k == count - 1 ? extras_text("options.list_and") : ", ") : "",
                 what[k]);
    }
    if (count) {
        char list[100];
        snprintf(list, sizeof list, "%s", out);
        snprintf(out, (size_t)n, extras_text(count > 1 ? "options.override_many" : "options.override_one"), list);
    }
}

/* An option's label: the text of prefix + key ("rule.MassMove"). */
static const char *label(const char *prefix, const char *key)
{
    char k[64];
    snprintf(k, sizeof k, "%s%s", prefix, key);
    return extras_text(k);
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

/* Bug fixes: the original's bugs fixed or not.  1 if changed. */
static int page_fixes(Choices *c)
{
    char note[200];
    Dialog d(0x4000);
    dialog_begin(d, extras_text("fixes.title"), 480);
    paragraph(d, extras_text("fixes.intro"), 1);
    paragraph(d, extras_text("fixes.each"), 0);
    overrides(1, note, sizeof note);
    if (note[0])
        paragraph(d, note, 0);
    d.add_checkbox_option(extras_text("fixes.fix"), 0, c->fix);
    if (d.show_modal(0) != 0 || (d.option_is_checked(0) != 0) == c->fix)
        return 0;
    c->fix = d.option_is_checked(0) != 0;
    return 1;
}

/* Classic graphics ticked where the game folder lacks the original's
   pictures: the original's files offered to be found (plat_add_original).
   1: the folder has them now. */
static int find_original(void)
{
    int choice, r;
    {
        Dialog d(0x4000);
        dialog_begin(d, extras_text("locate.title"), 480);
        paragraph(d, extras_text("locate.intro"), 1);
        d.add_option(extras_text("locate.find"), 0, 0);
        d.add_option(extras_text("locate.keep"), 1, 0);
        choice = d.show_modal(0);
    }
    r = choice == 0 ? plat_add_original() : 0;
    if (r <= 0)
        message(extras_text("locate.title"), extras_text(r < 0 ? "locate.cannot" : "locate.kept"));
    return r > 0;
}

/* Graphics and screen: Classic graphics, sharp text and interface, full
   screen.  1 if changed. */
static int page_graphics(Choices *c)
{
    char note[200];
    int browser = plat_restart(0) == 2, result;
    Choices was = *c;
    Dialog d(0x4000);
    dialog_begin(d, extras_text("graphics.title"), 480);
    paragraph(d, extras_text("graphics.classic_note"), 1);
    paragraph(d, extras_text("graphics.sharp_note"), 0);
    if (!browser)
        paragraph(d, extras_text("graphics.fullscreen_note"), 0);
    if (plat_graphics_forced())
        paragraph(d, extras_text("graphics.need_original"), 0);
    paragraph(d, extras_text("options.next_start"), 0);
    overrides(2, note, sizeof note);
    if (note[0])
        paragraph(d, note, 0);
    d.add_checkbox_option(extras_text("graphics.classic"), 0, !c->hd);
    d.add_checkbox_option(extras_text("graphics.sharp"), 1, c->sharp);
    if (!browser)
        d.add_checkbox_option(extras_text("graphics.fullscreen"), 2, c->fullscreen);
    result = d.show_modal(0);
    if (result != 0)
        return 0;
    if ((d.option_is_checked(1) != 0) != c->sharp) {
        c->sharp = d.option_is_checked(1) != 0;
        c->sharp_set = 1;
    }
    if (!browser)
        c->fullscreen = d.option_is_checked(2) != 0;
    /* (without the original's pictures: Classic graphics once they are
       found; HD graphics until then) */
    if (d.option_is_checked(0) && c->hd && (!plat_graphics_forced() || find_original()))
        c->hd = 0;
    else if (!d.option_is_checked(0))
        c->hd = 1;
    return memcmp(&was, c, sizeof was) != 0;
}

/* Rule changes. */
static void page_rules(Choices *c)
{
    int i;
    Dialog d(0x4000);
    dialog_begin(d, extras_text("rules.title"), 480);
    paragraph(d, extras_text("rules.intro"), 1);
    for (i = 0; i < NRULES; i++)
        d.add_checkbox_option(label("rule.", rules[i].key), i, c->rule[i]);
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
    dialog_begin(d, extras_text("limits.title"), 400);
    paragraph(d, extras_text("limits.intro"), 1);
    for (i = 0; i < NLIMITS; i++) {
        d.add_edit_field(label("limit.", limits[i].key), c->limit[i], 8);
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
    dialog_begin(d, extras_text("interface.title"), 600);
    paragraph(d, extras_text("interface.intro"), 1);
    d.set_columns(2);
    for (i = 0; i < NEXTRAS; i++)
        d.add_checkbox_option(label("extra.", extras[i].key), i, c->extra[i]);
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
    dialog_begin(d, extras_text("colours.title"), 440);
    paragraph(d, extras_text("colours.intro"), 1);
    for (i = 0; i < NCOLOR; i++) {
        d.add_edit_field(label("colour.", colours[i].key), c->colour[i], i == C_SUPPRESS ? 255 : 7);
        sizes[i] = i == C_SUPPRESS ? 256 : 16;
    }
    if (!fields(d, values, sizes, NCOLOR))
        return;
    for (i = 0; i < NCOLOR; i++)
        snprintf(c->colour[i], 256, "%s", values[i]);
}

/* Language: Auto (the system's), English, the languages there are files
   of (data directory's lang/: lang_languages).  1 if changed. */
static int page_language(Choices *c)
{
    static LangChoice langs[32];
    const char *env = getenv("CIV2_LANG");
    int n = lang_languages(langs, 32), i, choice, current = 0;
    Dialog d(0x4000);
    dialog_begin(d, extras_text("language.title"), 440);
    paragraph(d, extras_text("language.intro"), 1);
    paragraph(d, extras_text("options.next_start"), 0);
    if (env && *env && !plat_replay)
        paragraph(d, extras_text("language.override"), 0);
    d.add_option(extras_text("language.auto"), 0, 0);
    d.add_option("English", 1, 0);
    if (!__strcmpi(c->language, "en"))
        current = 1;
    for (i = 0; i < n; i++) {
        d.add_option(langs[i].name, i + 2, 0);
        if (!__strcmpi(c->language, langs[i].code))
            current = i + 2;
    }
    d.set_default_choice(current);
    choice = d.show_modal(0);
    if (choice < 0 || choice == current)
        return 0;
    snprintf(c->language, sizeof c->language, "%s", choice == 0 ? "" : choice == 1 ? "en" : langs[choice - 2].code);
    return 1;
}

/* Whether a choice that needs a restart differs from what the game started
   with: then the restart offered (natively the program started again, in
   the browser the page loaded again). */
static void offer_restart(const Choices *c)
{
    int how, choice;
    if (c->hd == at_start.hd && c->sharp == at_start.sharp &&
        c->fullscreen == at_start.fullscreen && !strcmp(c->language, at_start.language))
        return;
    how = plat_restart(0);
    if (!how) {
        message(extras_text("restart.title"), extras_text("restart.next_time"));
        return;
    }
    {
        Dialog d(0x4000);
        dialog_begin(d, extras_text("restart.title"), 400);
        paragraph(d, extras_text(how == 2 ? "restart.ask_page" : "restart.ask"), 1);
        d.add_option(extras_text("restart.now"), 0, 0);
        d.add_option(extras_text("restart.later"), 1, 0);
        choice = d.show_modal(0);
    }
    if (choice == 0 && !plat_restart(1) && !plat_replay)
        message(extras_text("restart.title"), extras_text("restart.failed"));
}

enum { P_FIXES, P_GRAPHICS, P_RULES, P_LIMITS, P_INTERFACE, P_COLOURS, P_LANGUAGE };

void options_dialog(void)
{
    int choice = P_FIXES;
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
            dialog_begin(d, extras_text("options.title"), 360);
            d.add_line(extras_text("options.kept"), -1, -1, -1, 0);
            d.add_option(extras_text("options.fixes"), P_FIXES, 0);
            d.add_option(extras_text("options.graphics"), P_GRAPHICS, 0);
            d.add_option(extras_text("options.rules"), P_RULES, 0);
            d.add_option(extras_text("options.limits"), P_LIMITS, 0);
            d.add_option(extras_text("options.interface"), P_INTERFACE, 0);
            d.add_option(extras_text("options.colours"), P_COLOURS, 0);
            d.add_option(extras_text("options.language"), P_LANGUAGE, 0);
            d.set_default_choice(choice);
            choice = d.show_modal(0);
        }
        if (choice < 0)
            return;
        switch (choice) {
        case P_FIXES:
            if (page_fixes(&now)) {
                write_choices(&now, &was);
                fixes_load();       /* at once */
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
        case P_LANGUAGE:
            if (page_language(&now)) {
                write_choices(&now, &was);
                offer_restart(&now);
            }
            break;
        }
        if (choice >= P_RULES && choice <= P_COLOURS) {
            /* at once: the additions and the rules read again */
            write_choices(&now, &was);
            extras_load();
        }
    }
}
