/*
 * (port) The game's text by key (src/include/lang.h).  The English text is
 * built from the game's text files with the game's own readers
 * (text_open_section, text_read_line: same trimming, the scenario's files
 * first), so it is the text the original shows; what each line of those
 * files is for, and how it differs between their versions, is known here
 * only.  Where the game has no such file, OpenCiv2's own English of it is
 * read in its place (data/text: lang_builtin_text_file).  A translation
 * (lang/<code>.txt) replaces texts by key.
 */
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "game.h"
#include "lang.h"
#include "../host/codepages.h"
#include "rules.h"

EXTERN_C char plat_data_dir[];

/* The main menu bar, as menu_build_main built it from MENU.TXT. */
const MenuRow menu_rows[] = {
    {1, MENU_TITLE, "menu.title.1"},
    {1, CMD_GAME_OPTIONS, "menu.101"}, {1, CMD_GRAPHIC_OPTIONS, "menu.102"},
    {1, CMD_CITY_REPORT_OPTIONS, "menu.103"}, {1, CMD_MULTIPLAYER_OPTIONS, "menu.104"},
    {1, CMD_GAME_PROFILE, "menu.105"}, {1, 0, 0}, {1, CMD_PICK_MUSIC, "menu.106"}, {1, 0, 0},
    {1, CMD_SAVE_GAME, "menu.110"}, {1, CMD_LOAD_GAME, "menu.120"}, {1, CMD_JOIN_GAME, "menu.130"},
    {1, 0, 0}, {1, CMD_SET_PASSWORD, "menu.131"}, {1, CMD_CHANGE_TIMER, "menu.132"}, {1, 0, 0},
    {1, CMD_RETIRE, "menu.1f0"}, {1, CMD_QUIT, "menu.1f1"},
    {2, MENU_TITLE, "menu.title.2"},
    {2, CMD_TAX_RATE, "menu.201"}, {2, 0, 0}, {2, CMD_THRONE_ROOM, "menu.205"},
    {2, CMD_FIND_CITY, "menu.210"}, {2, 0, 0}, {2, CMD_REVOLUTION, "menu.220"},
    {3, MENU_TITLE, "menu.title.3"},
    {3, CMD_MOVE_PIECES, "menu.301"}, {3, CMD_VIEW_PIECES, "menu.302"}, {3, 0, 0},
    {3, CMD_ZOOM_IN, "menu.310"}, {3, CMD_ZOOM_OUT, "menu.311"}, {3, 0, 0},
    {3, CMD_MAX_ZOOM_IN, "menu.320"}, {3, CMD_STANDARD_ZOOM, "menu.321"},
    {3, CMD_MEDIUM_ZOOM_OUT, "menu.322"}, {3, CMD_MAX_ZOOM_OUT, "menu.324"}, {3, 0, 0}, {3, CMD_MAP_GRID, "menu.327"},
    {3, CMD_ARRANGE_WINDOWS, "menu.328"}, {3, CMD_SHOW_HIDDEN_TERRAIN, "menu.330"},
    {3, CMD_CENTER_VIEW, "menu.340"},
    {4, MENU_TITLE, "menu.title.4"},
    {4, CMD_BUILD_CITY, "menu.401", 0x3c}, {4, CMD_BUILD_ROAD, "menu.410", 0x3c},
    {4, CMD_BUILD_IRRIGATION, "menu.411", 0x3c}, {4, CMD_BUILD_MINES, "menu.412", 0x3c},
    {4, CMD_TRANSFORM, "menu.413", 0x3c}, {4, 0, 0}, {4, CMD_BUILD_AIRBASE, "menu.417"},
    {4, CMD_BUILD_FORTRESS, "menu.418"}, {4, CMD_AUTOMATE, "menu.41b"},
    {4, CMD_CLEAN_POLLUTION, "menu.420"}, {4, CMD_PILLAGE, "menu.421"}, {4, 0, 0},
    {4, CMD_UNLOAD, "menu.430"}, {4, CMD_GOTO, "menu.440"}, {4, CMD_PARADROP, "menu.441"},
    {4, CMD_AIRLIFT, "menu.442"}, {4, CMD_HOME_CITY, "menu.445", 0x3c}, {4, 0, 0},
    {4, CMD_FORTIFY, "menu.450"}, {4, CMD_SLEEP, "menu.451"}, {4, 0, 0}, {4, CMD_DISBAND, "menu.460"},
    {4, CMD_ACTIVATE_UNIT, "menu.468"}, {4, CMD_WAIT, "menu.470"}, {4, CMD_SKIP_TURN, "menu.480"},
    {4, 0, 0}, {4, CMD_END_TURN, "menu.490"},
    {5, MENU_TITLE, "menu.title.5"},
    {5, CMD_CHAT_WITH_KINGS, "menu.507"}, {5, CMD_HIGH_COUNCIL, "menu.500"}, {5, 0, 0},
    {5, CMD_CITY_STATUS, "menu.501"}, {5, CMD_DEFENSE_MINISTER, "menu.502"},
    {5, CMD_FOREIGN_MINISTER, "menu.503"}, {5, 0, 0}, {5, CMD_ATTITUDE_ADVISOR, "menu.504"},
    {5, CMD_TRADE_ADVISOR, "menu.505"}, {5, CMD_SCIENCE_ADVISOR, "menu.506"}, {5, 0, 0},
    {5, CMD_CASUALTY_TIMELINE, "menu.508"},
    {6, MENU_TITLE, "menu.title.6"},
    {6, CMD_WONDERS, "menu.601"}, {6, CMD_TOP_CITIES, "menu.602"}, {6, CMD_SCORE, "menu.603"},
    {6, 0, 0}, {6, CMD_DEMOGRAPHICS, "menu.605"}, {6, CMD_SPACESHIPS, "menu.606"},
    {7, MENU_TITLE, "menu.title.7"},
    {7, CMD_CHEAT_MODE, "menu.701"}, {7, 0, 0}, {7, CMD_CHEAT_CREATE_UNIT, "menu.711"},
    {7, CMD_CHEAT_REVEAL_MAP, "menu.712"}, {7, CMD_CHEAT_SET_HUMAN, "menu.713"}, {7, 0, 0},
    {7, CMD_CHEAT_SET_YEAR, "menu.721"}, {7, CMD_CHEAT_KILL_CIV, "menu.722"}, {7, 0, 0},
    {7, CMD_CHEAT_TECH_ADVANCE, "menu.731"}, {7, CMD_CHEAT_EDIT_TECHS, "menu.732"},
    {7, CMD_CHEAT_FORCE_GOVERNMENT, "menu.740"}, {7, CMD_CHEAT_CHANGE_TERRAIN, "menu.748"},
    {7, CMD_CHEAT_DESTROY_UNITS, "menu.750"}, {7, CMD_CHEAT_CHANGE_MONEY, "menu.752"}, {7, 0, 0},
    {7, CMD_CHEAT_EDIT_UNIT, "menu.755"}, {7, CMD_CHEAT_EDIT_CITY, "menu.760"},
    {7, CMD_CHEAT_EDIT_KING, "menu.765"}, {7, 0, 0}, {7, CMD_CHEAT_SCENARIO_PARAMS, "menu.768"},
    {7, CMD_CHEAT_SAVE_SCENARIO, "menu.770"},
    {8, MENU_TITLE, "menu.title.8"},
    {8, CMD_SCENARIO_FLAG, "menu.801"}, {8, 0, 0}, {8, CMD_ADVANCES_EDITOR, "menu.802"},
    {8, CMD_CITIES_EDITOR, "menu.803"}, {8, CMD_EFFECTS_EDITOR, "menu.804"},
    {8, CMD_IMPROVEMENTS_EDITOR, "menu.805"}, {8, CMD_TERRAIN_EDITOR, "menu.806"},
    {8, CMD_TRIBE_EDITOR, "menu.807"}, {8, CMD_UNITS_EDITOR, "menu.808"},
    {8, CMD_EVENTS_EDITOR, "menu.809"},
    {9, MENU_TITLE, "menu.title.9"},
    {9, CMD_PEDIA_ADVANCES, "menu.901"}, {9, CMD_PEDIA_IMPROVEMENTS, "menu.903"},
    {9, CMD_PEDIA_WONDERS, "menu.905"}, {9, CMD_PEDIA_UNITS, "menu.907"}, {9, 0, 0},
    {9, CMD_PEDIA_GOVERNMENTS, "menu.909"}, {9, CMD_PEDIA_TERRAIN, "menu.90b"}, {9, 0, 0},
    {9, CMD_PEDIA_CONCEPTS, "menu.90d"}, {9, 0, 0}, {9, CMD_ABOUT, "menu.9f0"},
};
const int menu_row_count = (int)(sizeof menu_rows / sizeof menu_rows[0]);

/* English (from the game's files) and the translation's texts by key; the
   English keys in the order they were first set (for the export). */
static std::unordered_map<std::string, std::string> english, translated;
static std::vector<std::string> english_order;
static int translation_loaded;

static void set_english(const char *key, const char *text)
{
    std::string k(key);
    if (english.find(k) == english.end())
        english_order.push_back(k);
    english[k] = text ? text : "";
}

EXTERN_C int plat_replay;

/* A language's file: lang/<code>.txt in the game's folder (the player's
   own, beside the game), else in the data directory (OpenCiv2's).  1 if
   found (its path in path). */
static int language_file(const char *code, char *path, size_t n)
{
    FILE *f;
    if (!*code || strchr(code, '/') || strchr(code, '\\') || strstr(code, ".."))
        return 0;
    snprintf(path, n, "lang/%s.txt", code);
    if ((f = fopen(path, "r")) == 0) {
        snprintf(path, n, "%s/lang/%s.txt", plat_data_dir, code);
        f = fopen(path, "r");
    }
    if (f)
        fclose(f);
    return f != 0;
}

/* A language code as the system or the player gives it ("de_DE.UTF-8",
   "pt-BR", "eo"): its file's path, the code itself first, then its
   language alone ("pt").  0: none (English: the built-in text). */
static int language_path(const char *code, char *path, size_t n)
{
    char c[32];
    /* ("de_DE.UTF-8": de-DE; zh_TW, zh-HK ...: zh-Hant, codepages.h) */
    language_alias(code, c, sizeof c);
    if (!c[0] || (tolower((unsigned char)c[0]) == 'e' && tolower((unsigned char)c[1]) == 'n' && (!c[2] || c[2] == '-')) || !__strcmpi(c, "auto") || !strcmp(c, "C") ||
        !strcmp(c, "POSIX"))
        return 0;
    if (language_file(c, path, n))
        return 1;
    c[strcspn(c, "-")] = 0;
    return language_file(c, path, n);
}

/* The language chosen: CIV2_LANG, else (not in a test run) the OpenCiv2
   Options' (CIV.INI [OpenCiv2] Language=), else, where that is Auto or
   unset, the system's (CIV2_SYSTEM_LANG: the host's, the browser page's). */
static void chosen_language(char *code, int n)
{
    const char *e = getenv("CIV2_LANG");
    code[0] = 0;
    if (e && *e) {
        snprintf(code, (size_t)n, "%s", e);
        return;
    }
    if (plat_replay || plat_unit_test)
        return;
    plat_profile_string("CIV.INI", "OpenCiv2", "Language", code, n);
    if (!code[0] || !__strcmpi(code, "Auto")) {
        e = getenv("CIV2_SYSTEM_LANG");
        snprintf(code, (size_t)n, "%s", e ? e : "");
    }
}

/* A language's name (in the code page cp of its file) as the game's text
   is now (plat_text_codepage_name: Windows-1252, a code page, UTF-8): 0 if
   it has a character the game's text cannot have (Chinese in Windows-1252). */
static int name_in_current_page(const char *cp, const char *in, char *out, int n)
{
    const char *now = plat_text_codepage_name();
    const unsigned short *from = codepage_table(cp), *to = codepage_table(now);
    int from_utf8 = codepage_is_utf8(cp), to_utf8 = codepage_is_utf8(now), o = 0, i = 0, len;
    if (!from && !from_utf8)
        from = codepage_1252;
    if (!to && !to_utf8)
        to = codepage_1252;
    while (in[i]) {
        unsigned c;
        if (from_utf8) {
            c = u8_decode(in + i, -1, &len);
            i += len;
            if (c == U8_BAD)
                return 0;
        } else
            c = codepage_to_unicode(from, (unsigned char)in[i++]);
        if (to_utf8) {
            char u[4];
            int k = 0;
            if (c < 0x80) u[k++] = (char)c;
            else if (c < 0x800) { u[k++] = (char)(0xc0 | c >> 6); u[k++] = (char)(0x80 | (c & 0x3f)); }
            else if (c < 0x10000) { u[k++] = (char)(0xe0 | c >> 12); u[k++] = (char)(0x80 | ((c >> 6) & 0x3f));
                                    u[k++] = (char)(0x80 | (c & 0x3f)); }
            else { u[k++] = (char)(0xf0 | c >> 18); u[k++] = (char)(0x80 | ((c >> 12) & 0x3f));
                   u[k++] = (char)(0x80 | ((c >> 6) & 0x3f)); u[k++] = (char)(0x80 | (c & 0x3f)); }
            if (o + k >= n)
                return 0;
            memcpy(out + o, u, (size_t)k);
            o += k;
        } else {
            unsigned char b = c ? codepage_from_unicode(to, c) : 0;
            if (!b || o + 1 >= n)
                return 0;
            out[o++] = (char)b;
        }
    }
    out[o] = 0;
    return 1;
}

/* The file's "openciv2.language.name" (the language's own name), into name,
   as the game's text can show it now (else, a language whose letters it
   cannot have: its English name, or its code). */
static int language_name(const char *path, const char *code, char *name, int n)
{
    static const char *const english[][2] = {{"zh-Hant", "Chinese (traditional)"}, {"el", "Greek"}};
    char line[256], cp[32] = "windows-1252", raw[256] = "";
    FILE *f = fopen(path, "r");
    int next = 0, found = 0;
    size_t i;
    if (!f)
        return 0;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (next == 1) {
            snprintf(raw, sizeof raw, "%s", line);
            found = 1;
            break;      /* (the code page, if any, is the first entry) */
        }
        if (next == 2)
            snprintf(cp, sizeof cp, "%s", line);
        next = !strcmp(line, "@@openciv2.language.name") ? 1 : !strcmp(line, "@@codepage") ? 2 : 0;
    }
    fclose(f);
    if (!found)
        return 0;
    if (name_in_current_page(cp, raw, name, n))
        return 1;
    snprintf(name, (size_t)n, "%s", code);
    for (i = 0; i < sizeof english / sizeof english[0]; i++)
        if (!strcmp(code, english[i][0]))
            snprintf(name, (size_t)n, "%s", english[i][1]);
    return 1;
}

int lang_languages(LangChoice *out, int max)
{
    char dir[1100], path[1200];
    int n = 0, i;
    DIR *d;
    struct dirent *e;
    snprintf(dir, sizeof dir, "%s/lang", plat_data_dir);
    if (!(d = opendir(dir)))
        return 0;
    while ((e = readdir(d)) != 0 && n < max) {
        size_t len = strlen(e->d_name);
        if (len < 5 || len - 4 >= sizeof out[n].code || strcmp(e->d_name + len - 4, ".txt") || !strcmp(e->d_name, "en.txt"))
            continue;
        snprintf(out[n].code, sizeof out[n].code, "%.*s", (int)(len - 4), e->d_name);
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);
        if (!language_name(path, out[n].code, out[n].name, sizeof out[n].name))
            snprintf(out[n].name, sizeof out[n].name, "%s", out[n].code);
        n++;
    }
    closedir(d);
    /* (in order of their codes) */
    for (i = 1; i < n; i++) {
        int k;
        for (k = i; k > 0 && strcmp(out[k - 1].code, out[k].code) > 0; k--) {
            LangChoice t = out[k];
            out[k] = out[k - 1];
            out[k - 1] = t;
        }
    }
    return n;
}

static void load_translation(void)
{
    char code[64], path[1200];
    static char line[0x1000];   /* (a description's paragraph is one line: the
                                   game reads 0xfff bytes of a description) */
    std::string key, text;
    int lines = 0;
    FILE *f;

    translation_loaded = 1;
    chosen_language(code, sizeof code);
    if (!language_path(code, path, sizeof path))
        return;
    f = fopen(path, "r");
    if (!f)
        return;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '@' && line[1] == '@') {
            if (!key.empty())
                translated[key] = text;
            key = line + 2;
            text.clear();
            lines = 0;
        } else if (!key.empty()) {
            if (lines++)
                text += '\n';
            text += line[0] == '\\' ? line + 1 : line;
        }
    }
    if (!key.empty())
        translated[key] = text;
    fclose(f);
    /* its code page (Windows-1252 unless it says): its glyphs, its input */
    if (translated.count("codepage"))
        plat_text_codepage(translated["codepage"].c_str());
}

/* A line that starts a section: "@NAME", NAME in capitals, digits and '_'
   (the lines in a section that start with '@' are in small letters:
   @title=, @width=, @options, ...).  name: NAME. */
static int section_header(const char *line, std::string *name)
{
    const char *p = line, *end;
    while (*p == ' ' || *p == '\t')
        p++;
    if (*p++ != '@')
        return 0;
    for (end = p; (*end >= 'A' && *end <= 'Z') || (*end >= '0' && *end <= '9') || *end == '_'; end++)
        ;
    if (end == p)
        return 0;
    if (name)
        name->assign(p, end);
    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;
    return *end == 0;
}

/* The game's text files whose sections are keyed by file and section, in
   lower case ("game.MAINMENU"): their popups and other texts.  (MENU and
   LABELS have keys of their own; RULES is the game's rules; DESCRIBE, the
   Civilopedia's descriptions, has its own reader: add_descriptions.) */
static const char *const section_files[] = {
    "GAME", "CITY", "ADVICE", "HELP", "TUTORIAL", "COUNCIL0", "COUNCIL1", "COUNCIL2", "HERALD", "PEDIA",
    "MAPMENU", "DEBUG", "CREDITS", "SCREDITS", "FCREDITS", "MPCREDITS",
};

/* A line "@" alone (ADVICE.TXT's blocks end with one). */
static int bare_at(const char *line)
{
    const char *p = line + strspn(line, " \t");
    if (*p++ != '@')
        return 0;
    return p[strspn(p, " \t\r\n")] == 0;
}

/* A line "@@name" (trimmed): a block's name (ADVICE.TXT: "@@Temple", the
   advice for a Temple, which the game finds by the improvement's name). */
static int block_name(const char *line, std::string *name)
{
    const char *p = line + strspn(line, " \t");
    std::string n;
    if (p[0] != '@' || p[1] != '@')
        return 0;
    n = p + 2;
    n.erase(n.find_last_not_of(" \t\r\n") + 1);
    if (n.empty())
        return 0;
    if (name)
        *name = n;
    return 1;
}

/* file's sections, as English texts: each one's lines (as the game's reader
   reads them, without the newline) up to the next section.  A name used
   twice: the first, the one the game finds.  @STOP and @END end lists (city
   names), they are no sections.  only: that section alone (0: all).
   Blocks: after a line "@" alone, lines "@@name" start a block that the
   game finds by any of its names (ADVICE.TXT's advice by improvement or
   unit), up to the next "@" alone: keyed by its first name as in the file
   ("advice.@@Coastal Fortress"), its other names its first lines
   ("@@SAM Missile Battery", which a translation keeps). */
static void add_file_sections(const char *file, const char *only)
{
    char line[0x100];
    std::string prefix(file), name, text;
    int in_section = 0, in_block = 0, lines = 0, pending_at = 0;
    size_t i;

    if (text_open_section(file, 0) != 0)
        return;
    for (i = 0; i < prefix.size(); i++)
        prefix[i] = (char)tolower((unsigned char)prefix[i]);
    prefix += '.';
    for (;;) {
        char *got = _fgets(line, 0xff, text_file_handle);
        int header, block = 0;
        if (got)
            line[strcspn(line, "\r\n")] = 0;
        header = got && section_header(line, 0);
        if (got && !header && (pending_at || (!in_section && !in_block)) && block_name(line, 0))
            block = 1;
        if (pending_at && !block) {     /* (the "@" was the section's) */
            if (lines++)
                text += '\n';
            text += "@";
        }
        pending_at = 0;
        if (!got || header || block || (in_block && bare_at(line))) {
            if ((in_section || in_block) && !english.count(prefix + name))
                set_english((prefix + name).c_str(), text.c_str());
            if (!got)
                break;
            in_section = in_block = 0;
            text.clear();
            lines = 0;
            if (header) {
                section_header(line, &name);
                in_section = name != "STOP" && name != "END" && (!only || name == only);
            } else if (block) {
                block_name(line, &name);
                name = "@@" + name;
                in_block = !only;
            }
            continue;
        }
        if (in_section && bare_at(line)) {
            pending_at = 1;     /* a block's start, if a "@@" line follows */
            continue;
        }
        if (!in_section && !in_block)
            continue;
        if (lines++)
            text += '\n';
        text += line;
    }
    text_close_file();
}

/* RULES.TXT, its names: per section the lines the rules loaders read
   (rules_load_*, rules_parse_sections), their names the tokens they put in
   the string pool.  Read once, from the game's RULES.TXT (the rules of a
   scenario with its own are read from it, names and all); without one, the
   built-in rules' (rules.h). */
static void rules_section_names(const char *section, int count, const char *format, int stop_at_blank)
{
    char key[64];
    int i;
    if (text_open_section(0, section) != 0)
        return;
    for (i = 0; i < count; i++) {
        i8 *line = text_read_line(1);
        if (stop_at_blank && (!line || !*line || *line == ';'))
            break;
        snprintf(key, sizeof key, format, i);
        set_english(key, (const char *)text_next_token());
    }
}

/* The built-in rules' names (rules.h), under the same keys: the English
   when there is no RULES.TXT. */
static void rules_default_names(void)
{
    char key[64];
    int i, t;
    for (i = 0; i < rules_advance_count; i++) {
        snprintf(key, sizeof key, "rules.advance.%d", i);
        set_english(key, rules_advances[i].name);
    }
    for (i = 0; i < IMP_COUNT; i++) {
        snprintf(key, sizeof key, "rules.improvement.%d", i);
        set_english(key, rules_improvements[i].name);
    }
    for (i = 0; i < rules_unit_count; i++) {
        snprintf(key, sizeof key, "rules.unit.%d", i);
        set_english(key, rules_units[i].name);
    }
    for (i = 0; i < TERRAIN_RECORDS; i++) {
        snprintf(key, sizeof key, "rules.terrain.%d", i);
        set_english(key, rules_terrain[i].name);
    }
    for (i = 0; i < 7; i++) {
        snprintf(key, sizeof key, "rules.government.%d", i);
        set_english(key, rules_governments[i].name);
        snprintf(key, sizeof key, "rules.government.%d.male", i);
        set_english(key, rules_governments[i].male_title);
        snprintf(key, sizeof key, "rules.government.%d.female", i);
        set_english(key, rules_governments[i].female_title);
    }
    for (i = 0; i < 21; i++) {
        const RulesLeader *l = &rules_leaders[i];
        snprintf(key, sizeof key, "rules.leader.%d.male", i);
        set_english(key, l->male_name);
        snprintf(key, sizeof key, "rules.leader.%d.female", i);
        set_english(key, l->female_name);
        snprintf(key, sizeof key, "rules.leader.%d.plural", i);
        set_english(key, l->plural);
        snprintf(key, sizeof key, "rules.leader.%d.adjective", i);
        set_english(key, l->adjective);
        for (t = 0; t < 4 && l->titles[t].govt > 0; t++) {
            snprintf(key, sizeof key, "rules.leader.%d.title.%d.male", i, l->titles[t].govt);
            set_english(key, l->titles[t].male);
            snprintf(key, sizeof key, "rules.leader.%d.title.%d.female", i, l->titles[t].govt);
            set_english(key, l->titles[t].female);
        }
    }
    for (i = 0; i < 16; i++) {
        snprintf(key, sizeof key, "rules.commodity.%d", i);
        set_english(key, rules_commodities[i]);
    }
    for (i = 1; i < 13; i++) {
        snprintf(key, sizeof key, "rules.order.%d", i);
        set_english(key, rules_orders[i - 1].name);
        snprintf(key, sizeof key, "rules.order.%d.key", i);
        set_english(key, rules_orders[i - 1].key);
    }
    for (i = 0; i < 6; i++) {
        snprintf(key, sizeof key, "rules.difficulty.%d", i);
        set_english(key, rules_difficulties[i]);
    }
    for (i = 0; i < 9; i++) {
        snprintf(key, sizeof key, "rules.attitude.%d", i);
        set_english(key, rules_attitudes[i]);
    }
}

void lang_load_rules(void)
{
    static int loaded;
    char key[64];
    int i, g;
    i8 *line;

    if (loaded)
        return;
    loaded = 1;
    if (rules_forced_source == RULES_BUILTIN || text_open_section("RULES", 0) != 0) {
        rules_default_names();
        return;
    }
    rules_section_names("CIVILIZE", 100, "rules.advance.%d", 1);
    rules_section_names("IMPROVE", 67, "rules.improvement.%d", 0);
    rules_section_names("UNITS", 62, "rules.unit.%d", 1);
    rules_section_names("TERRAIN", 33, "rules.terrain.%d", 0);
    if (text_open_section(0, "GOVERNMENTS") == 0)
        for (i = 0; i < 7; i++) {
            text_read_line(1);
            snprintf(key, sizeof key, "rules.government.%d", i);
            set_english(key, (const char *)text_next_token());
            snprintf(key, sizeof key, "rules.government.%d.male", i);
            set_english(key, (const char *)text_next_token());
            snprintf(key, sizeof key, "rules.government.%d.female", i);
            set_english(key, (const char *)text_next_token());
        }
    if (text_open_section(0, "LEADERS") == 0)
        for (i = 0; i < 21; i++) {
            static const char *const fields[] = {"male", "female", 0, 0, 0, "plural", "adjective", 0, 0, 0};
            size_t f;
            text_read_line(1);
            for (f = 0; f < sizeof fields / sizeof fields[0]; f++) {
                if (!fields[f]) {
                    text_next_int();
                    continue;
                }
                snprintf(key, sizeof key, "rules.leader.%d.%s", i, fields[f]);
                set_english(key, (const char *)text_next_token());
            }
            while ((g = text_next_int()) > 0) {
                snprintf(key, sizeof key, "rules.leader.%d.title.%d.male", i, g);
                set_english(key, (const char *)text_next_token());
                snprintf(key, sizeof key, "rules.leader.%d.title.%d.female", i, g);
                set_english(key, (const char *)text_next_token());
            }
        }
    rules_section_names("CARAVAN", 16, "rules.commodity.%d", 0);
    if (text_open_section(0, "ORDERS") == 0)
        for (i = 1; i < 13; i++) {
            text_read_line(1);
            snprintf(key, sizeof key, "rules.order.%d", i);
            set_english(key, (const char *)text_next_token());
            snprintf(key, sizeof key, "rules.order.%d.key", i);
            set_english(key, (const char *)text_next_token());
        }
    if (text_open_section(0, "DIFFICULTY") == 0)
        for (i = 0; i < 6; i++) {
            line = text_read_line(1);
            snprintf(key, sizeof key, "rules.difficulty.%d", i);
            set_english(key, line ? (const char *)line : "");
        }
    if (text_open_section(0, "ATTITUDES") == 0)
        for (i = 0; i < 9; i++) {
            line = text_read_line(1);
            snprintf(key, sizeof key, "rules.attitude.%d", i);
            set_english(key, line ? (const char *)line : "");
        }
    text_close_file();
}

/* "@ADVANCE_INDEX", "ADVANCE_DESCRIPTIONS", "TERRAIN_INDEX": the group of
   descriptions, "ADVANCE" ... (DESCRIBE.TXT's terrain is in the section
   TERRAIN_AND_RESOURCE_DESCRIPTIONS, after its @@TERRAIN_INDEX). */
static std::string description_group(const char *name)
{
    std::string g(name + strspn(name, "@"));
    size_t n;
    if ((n = g.rfind("_INDEX")) != std::string::npos && n + 6 == g.size())
        g.erase(n);
    else if ((n = g.rfind("_DESCRIPTIONS")) != std::string::npos && n + 13 == g.size())
        g.erase(n);
    return g;
}

/* DESCRIBE.TXT: per group an index (@@ADVANCE_INDEX, the rules' entries'
   descriptions: structure, no text) and the descriptions, each a line
   "@@name" and its paragraphs, one per line, up to the next line starting
   with '@' (pedia_load_description; it passes over ';' lines). */
static void add_descriptions(void)
{
    static char line[0x800];
    std::string group, key, text;
    int in_entry = 0, lines = 0;

    if (text_open_section("DESCRIBE", 0) != 0)
        return;
    for (;;) {
        char *got = _fgets(line, 0x800, text_file_handle);
        if (got)
            line[strcspn(line, "\r\n")] = 0;
        if (!got || line[0] == '@') {
            if (in_entry && !english.count(key))
                set_english(key.c_str(), text.c_str());
            if (!got)
                break;
            in_entry = 0;
            if (line[1] != '@') {
                group = description_group(line + 1);
            } else if (strstr(line, "_INDEX")) {
                group = description_group(line + 2);
            } else {
                std::string name(line + 2);
                name.erase(name.find_last_not_of(" \t") + 1);
                key = "describe." + group + "." + name;
                in_entry = 1;
                text.clear();
                lines = 0;
            }
            continue;
        }
        if (!in_entry || line[0] == ';')
            continue;
        if (lines++)
            text += '\n';
        text += line;
    }
    text_close_file();
}

FILE *lang_builtin_text_file(const char *name)
{
    char path[1200], file[64];
    const char *base = name;
    size_t i;
    for (i = 0; name[i]; i++)
        if (name[i] == '\\' || name[i] == '/' || name[i] == ':')
            base = name + i + 1;
    if (!*base || strlen(base) >= sizeof file)
        return 0;
    for (i = 0; base[i]; i++)
        file[i] = (char)toupper((unsigned char)base[i]);
    file[i] = 0;
    snprintf(path, sizeof path, "%s/text/%s", plat_data_dir, file);
    return crt_fopen_host(path, "rt");
}

const char *lang_description(const char *group, const char *entry)
{
    std::unordered_map<std::string, std::string>::const_iterator it;
    if (!translation_loaded)
        load_translation();
    if (translated.empty())
        return 0;
    it = translated.find("describe." + description_group(group) + "." + entry);
    return it != translated.end() ? it->second.c_str() : 0;
}

/* (the export of OpenCiv2's own texts: their keys, in the table's order) */
static std::vector<std::string> own_keys;
static void add_own(const char *key, const char *text)
{
    set_english(key, text);
    own_keys.push_back(key);
}

static void export_english(void)
{
    static int sections_added;
    const char *path = getenv("CIV2_EXPORT_TEXT");
    FILE *f;
    size_t i, start, end;
    const char *own_env = getenv("CIV2_EXPORT_OWN");
    int own = own_env && *own_env && *own_env != '0';
    if (!path || !*path)
        return;
    if (own) {
        /* OpenCiv2's own texts only (the template of a translation of
           them: data/native/lang/en.txt), none of the game's */
        own_keys.clear();
        extras_texts_each(add_own);
    } else if (!sections_added) {
        sections_added = 1;
        for (i = 0; i < sizeof section_files / sizeof section_files[0]; i++)
            add_file_sections(section_files[i], 0);
        add_file_sections("LABELS", "POPUPS");  /* the dialogs' OK, Help, Cancel */
        add_descriptions();
        lang_load_rules();
        extras_texts_each(set_english);     /* OpenCiv2's additions: openciv2.<key> */
    }
    if (!(f = fopen(path, "w")))
        return;
    fprintf(f, "; OpenCiv2 text (English), keyed: \"@@key\" and its text on the lines after it\n"
               "; (a line of text that starts with \"@@\" or \"\\\" has a \"\\\" put before it).\n");
    if (own)
        fprintf(f, "; OpenCiv2's own texts only: the template of a translation (docs/TRANSLATING.md).\n"
                   "; Encoding: Windows-1252, as the game's fonts; placeholders (%%s, %%d) stay, in order.\n");
    for (i = 0; i < (own ? own_keys.size() : english_order.size()); i++) {
        const std::string &k = own ? own_keys[i] : english_order[i];
        const std::string &text = english[k];
        fprintf(f, "@@%s\n", k.c_str());
        for (start = 0;; start = end + 1) {
            end = text.find('\n', start);
            if (end == std::string::npos)
                end = text.size();
            if (text.compare(start, 2, "@@") == 0 || text.compare(start, 1, "\\") == 0)
                fputc('\\', f);
            fprintf(f, "%.*s\n", (int)(end - start), text.c_str() + start);
            if (end == text.size())
                break;
        }
    }
    fclose(f);
}

const char *lang_text(const char *key)
{
    std::unordered_map<std::string, std::string>::const_iterator it;
    if (!translation_loaded)
        load_translation();
    it = translated.find(key);
    if (it != translated.end())
        return it->second.c_str();
    it = english.find(key);
    return it != english.end() ? it->second.c_str() : "";
}

int lang_translated(const char *key)
{
    if (!translation_loaded)
        load_translation();
    return translated.count(key) != 0;
}

int lang_has(const char *key)
{
    if (!translation_loaded)
        load_translation();
    return translated.count(key) || english.count(key);
}

/* MENU.TXT: per menu a section (@GAME ...), its title, then one line per
   item in the menu's order (menu_rows; separators have none).  Its
   versions: the 5.4.0f patch's @GAME has 13 items (with Game Profile,
   0x105), the CD's 12, without it (a 13th line that is no label: blank,
   the next @section, a comment).  The sections are read in one pass as
   the original did, which stopped at the first one missing. */
void lang_load_menus(void)
{
    static const char *const sections[9] = {
        str_game_5, str_kingdom, str_view, str_orders_2, str_advisors, str_world, str_cheat, str_editor, str_pedia_2,
    };
    char game_lines[13][0x100];
    int menu, row, n, has_profile;
    i32 pos = 0;
    i8 *line;

    if (!translation_loaded)
        load_translation();     /* (its code page before any text is drawn) */

    for (menu = 1; menu <= 9; menu++) {
        if (text_open_section(menu == 1 ? str_menu : 0, sections[menu - 1]) != 0)
            break;
        for (row = 0; row < menu_row_count; row++)
            if (menu_rows[row].menu == menu && menu_rows[row].cmd == MENU_TITLE)
                set_english(menu_rows[row].key, text_read_line(1));
        if (menu == 1) {
            for (n = 0; n < 13; n++) {
                if (n == 12)
                    pos = _ftell(text_file_handle);
                line = text_read_line(1);
                snprintf(game_lines[n], sizeof game_lines[n], "%s", line ? line : "");
            }
            has_profile = game_lines[12][0] != 0 && game_lines[12][0] != '@' && game_lines[12][0] != ';';
            if (!has_profile && line)
                _fseek(text_file_handle, pos, 0);
            n = 0;
            for (row = 0; row < menu_row_count; row++)
                if (menu_rows[row].menu == 1 && menu_rows[row].cmd > 0 && (menu_rows[row].cmd != CMD_GAME_PROFILE || has_profile))
                    set_english(menu_rows[row].key, game_lines[n++]);
            continue;
        }
        for (row = 0; row < menu_row_count; row++)
            if (menu_rows[row].menu == menu && menu_rows[row].cmd > 0)
                set_english(menu_rows[row].key, text_read_line(1));
    }
    text_close_file();
    export_english();
}

/* LABELS.TXT is a numbered list (labels_load: label_table.string_ids[n] is the
   n-th line's string in the string pool); n is the structure, "label.<n>"
   the key.  A translated label is given by label_string in place of its
   pool string (the pool has a fixed size: translations are kept here). */
static std::unordered_map<int, std::string> pool_translated;

void lang_add_labels(int count)
{
    char key[32];
    int i;
    pool_translated.clear();        /* (a new pool: labels reloaded) */
    for (i = 0; i < count; i++) {
        snprintf(key, sizeof key, "label.%d", i);
        set_english(key, label_string(label_table.string_ids[i]));
    }
    if (!translation_loaded)
        load_translation();
    for (i = 0; i < count; i++) {
        snprintf(key, sizeof key, "label.%d", i);
        std::unordered_map<std::string, std::string>::const_iterator it = translated.find(key);
        if (it != translated.end())     /* (no longer than a LABELS.TXT line) */
            pool_translated[label_table.string_ids[i]] = it->second.substr(0, 0xfe);
    }
    export_english();
}

char *lang_pool_text(int string_id)
{
    std::unordered_map<int, std::string>::iterator it;
    if (pool_translated.empty())
        return 0;
    it = pool_translated.find(string_id);
    return it != pool_translated.end() ? &it->second[0] : 0;
}

/* The translated section text_read_line reads: its lines, the next one, in
   which file; the file of the last section opened by name. */
static std::vector<std::string> section_lines;
static size_t section_next;
static FILE *section_file;
static std::string section_last_file;

/* The translation's blocks ("advice.@@Coastal Fortress", add_file_sections)
   by each of their names, in capitals ("advice.@@SAM MISSILE BATTERY"), as
   the game finds them: the block's key. */
static std::unordered_map<std::string, std::string> block_keys;
static int block_keys_made;

static void make_block_keys(void)
{
    std::unordered_map<std::string, std::string>::const_iterator it;
    size_t at, i, start, end;
    block_keys_made = 1;
    for (it = translated.begin(); it != translated.end(); ++it) {
        std::string upper;
        if ((at = it->first.find(".@@")) == std::string::npos)
            continue;
        upper = it->first;
        for (i = at; i < upper.size(); i++)
            upper[i] = (char)toupper((unsigned char)upper[i]);
        block_keys[upper] = it->first;
        /* its other names: its first lines */
        for (start = 0; start < it->second.size(); start = end + 1) {
            std::string name;
            end = it->second.find('\n', start);
            if (end == std::string::npos)
                end = it->second.size();
            if (!block_name(it->second.substr(start, end - start).c_str(), &name))
                break;
            for (i = 0; i < name.size(); i++)
                name[i] = (char)toupper((unsigned char)name[i]);
            block_keys[it->first.substr(0, at) + ".@@" + name] = it->first;
        }
    }
}

const char *lang_english_name(const char *key, const char *name)
{
    std::unordered_map<std::string, std::string>::const_iterator t, e;
    if (!translation_loaded)
        load_translation();
    if (translated.empty() || (t = translated.find(key)) == translated.end() || (e = english.find(key)) == english.end() ||
        strcmp(t->second.c_str(), name) != 0)
        return name;
    return e->second.c_str();
}

void lang_section_opened(const char *filename, const char *section, int in_scenario)
{
    std::unordered_map<std::string, std::string>::const_iterator it;
    std::string key;
    const char *base;
    char line[0x100];
    i32 pos, at_pos;
    size_t i, start, end;
    int block, names = 1;

    lang_section_closed();
    if (filename) {
        base = filename;
        for (i = 0; filename[i]; i++)
            if (filename[i] == '\\' || filename[i] == '/' || filename[i] == ':')
                base = filename + i + 1;
        section_last_file.assign(base, strcspn(base, "."));
        for (i = 0; i < section_last_file.size(); i++)
            section_last_file[i] = (char)tolower((unsigned char)section_last_file[i]);
    }
    if (!translation_loaded)
        load_translation();
    if (translated.empty() || in_scenario || !section)
        return;
    /* "@Temple": the block "@@Temple" (add_file_sections) */
    block = section[0] == '@';
    key = section_last_file + '.';
    if (block)
        key += '@';
    for (i = 0; section[i]; i++)
        key += (char)toupper((unsigned char)section[i]);
    if (block) {
        std::unordered_map<std::string, std::string>::const_iterator b;
        if (!block_keys_made)
            make_block_keys();
        if ((b = block_keys.find(key)) == block_keys.end())
            return;
        key = b->second;
    }
    it = translated.find(key);
    if (it == translated.end())
        return;
    for (start = 0;; start = end + 1) {
        end = it->second.find('\n', start);
        if (end == std::string::npos)
            end = it->second.size();
        /* (a block's other names, its first lines: passed over) */
        if (!(block && names && (names = block_name(it->second.substr(start, end - start).c_str(), 0)) != 0))
            section_lines.push_back(it->second.substr(start, end - start));
        if (end == it->second.size())
            break;
    }
    /* the file's own lines of the section passed over: what comes after
       the translation is the file's next section (a block's: its "@" line,
       which ends it; a section's: a "@" line that blocks follow) */
    for (;;) {
        pos = _ftell(text_file_handle);
        if (!_fgets(line, 0xff, text_file_handle))
            break;
        if (section_header(line, 0)) {
            _fseek(text_file_handle, pos, 0);
            break;
        }
        if (bare_at(line)) {
            if (block) {
                _fseek(text_file_handle, pos, 0);
                break;
            }
            at_pos = pos;
            pos = _ftell(text_file_handle);
            if (_fgets(line, 0xff, text_file_handle) && block_name(line, 0)) {
                _fseek(text_file_handle, at_pos, 0);
                break;
            }
            _fseek(text_file_handle, pos, 0);
        }
    }
    section_next = 0;
    section_file = text_file_handle;
}

int lang_section_line(char *buf)
{
    if (!section_file || section_file != text_file_handle)
        return 0;
    if (section_next >= section_lines.size()) {
        lang_section_closed();
        return 0;
    }
    snprintf(buf, 0xff, "%s", section_lines[section_next++].c_str());
    return 1;
}

void lang_section_closed(void)
{
    section_file = 0;
    section_lines.clear();
    section_next = 0;
}

/* (make test: civ2-unittest --text DIR PATH, tests/text_keys_test.py) The
   export (CIV2_EXPORT_TEXT) of the game in dir, written to path: its text
   files' keys, from its files or, without them, OpenCiv2's own English.
   0, or 1 if it was not written. */
EXTERN_C int lang_export_check(const char *dir, const char *path)
{
    FILE *f;
    if (__chdir((char *)dir) != 0)
        return 1;
    __getcwd(((i8 *)&game_dirs.home), 0x104);
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path), ((i8 *)&game_dirs.home));
    crt_strcpy(((i8 *)&main_frame_wnd_block.rules_txt_path), str_rules);
    setenv("CIV2_EXPORT_TEXT", path, 1);
    unsetenv("CIV2_EXPORT_OWN");
    unsetenv("CIV2_LANG");
    remove(path);
    strpool_init(0x8000);
    lang_load_menus();      /* (the export, the menus') */
    labels_load();          /* (and again with the labels) */
    if (!(f = fopen(path, "r")))
        return 1;
    fclose(f);
    return 0;
}
