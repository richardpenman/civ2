// SOURCE src/auto/text.cpp file_build_open_path
// SOURCE src/auto/text.cpp file_open_path
// SOURCE src/auto/text.cpp text_expand_vars
// SOURCE src/auto/text.cpp text_read_line
// SOURCE src/auto/text.cpp text_read_line_or_end
// SOURCE src/auto/civ.cpp advance_has_prereq_seen
// SOURCE src/auto/civ.cpp advance_has_prereq
// SOURCE src/auto/city.cpp city_add_trade_route
// SOURCE src/auto/game.cpp ScenarioEvents::parse_file
// Memory errors in the game's own code reached from its files (Peter Johnson's
// ledger BF-058/296, BF-197, BF-121, BF-173, BF-026, BF-030): the actual
// bodies with small providers; valid inputs keep the original's results.
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "native.h"
#include "win32_types.h"
#include "civ2types.h"
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#include "civ2classes.h"
#include "enums.h"
static_assert(sizeof(void *) == 4, "run with a 32-bit compiler");

// file_open_path: the 80-byte error path between its neighbours.
#pragma pack(push, 1)
static struct {
    char context_text[80];
    i8 last_open_path[80];
    int last_open_failed;
    uint last_alloc_failed;
} error_state;
#pragma pack(pop)
static char file_path_mode, open_path_prefix[60];
static std::string opened;
static char *path_join_upper(char *, iptr, const char *) { abort(); }
#define crt_strcpy strcpy
#define crt_strcat strcat
#define crt_memcpy memcpy
#define _strlen strlen
#define _memset memset
static FILE *_fopen(const char *path, char *) {
    opened = path;
    return nullptr;
}

// text_expand_vars: the arguments as laid out in the game (24 strings).
#pragma pack(push, 1)
struct TextArgs {
    i32 numbers[6];
    i8 strings[1];
    u8 rest[0x1888 - 25];
};
#pragma pack(pop)
static TextArgs text_args;
static char str_string[] = "STRING", str_number[] = "NUMBER";
static i8 text_var_hex_tag_str[] = "HEX", text_var_hex_pad_str[] = "0", text_var_percent_str[] = "%";
#define _strchr strchr
#define __strnicmp strncasecmp
#define _strncmp strncmp
#define _atoi atoi
static char *__ltoa(long value, char *buf, int radix) {
    sprintf(buf, radix == 16 ? "%lx" : "%ld", value);
    return buf;
}

// text_read_line and the events parser: lines from a string.
static struct {
    char buf[0x800];
} textbuf;
static struct {
    char *line_cursor;
} text_parse;
static FILE *text_file_handle;
static std::vector<std::string> lines;
static size_t line_pos;
static int lang_section_line(char *) { return 0; }
#define CRT_FEOF(f) (line_pos >= lines.size())
static char *_fgets(char *buf, int, FILE *) {
    if (line_pos >= lines.size())
        return nullptr;
    strcpy(buf, lines[line_pos++].c_str());
    return buf;
}
static void str_strip_newline(char *) {}
static void str_trim(char *) {}
static int closed;
static void text_close_file() { closed++; }
static int text_open_section(const char *, const char *) {
    line_pos = 0;
    return 0;
}
#define __strcmpi strcasecmp
static i8 events_kw_if_str[] = "IF", events_kw_then_str[] = "THEN";
static char str_endif[] = "ENDIF", str_debug[] = "DEBUG", str_endevents[] = "ENDEVENTS";
static char str_badeventsfile[] = "BADEVENTSFILE";
static char events_parse_verbose;
static int popups;
static void popup_game(const char *) { popups++; }
static void __chdir(const char *) {}
static struct {
    char home[264];
} game_dirs;
static struct {
    char scenario_dir_path[264];
} net_map_lock_slots_block;
#define _printf printf
static char str_if_found_creating_new_event_structure[] = "", str_failed_to_create_new_event_structure[] = "",
            str_new_event_structure_created[] = "", str_then_found[] = "", str_endif_found[] = "",
            str_debugging_filename_s[] = "", str_endevents_found[] = "";
struct EventsParseState {
    int have_owner;
    int have_homecity;
    int have_city;
    int have_unit;
    int have_move_count;
    int have_tech;
    ScenarioEvent *event;
    int have_listener_type;
    int have_unit_killed;
    int have_moveto;
    int have_receiver;
    int have_veteran;
    int have_tech_receiver;
    int have_tech_id;
    char *line;
    int have_defender;
    int have_talker_type;
    int have_destroy_whom;
    int parsed_civ;
    int have_terrain;
    int have_amount;
    int have_who;
    int have_attacker;
    int have_whom;
};
static ScenarioEvent pool[4];
static int allocated, actions;
ScenarioEvent *ScenarioEvents::alloc() { return &pool[allocated++]; }
int ScenarioEvents::parse_trigger(EventsParseState *) { return 1; }
int ScenarioEvents::parse_action(EventsParseState *ps) {
    actions++;
    ps->event->actions |= 1; // the write the original made through its unset pointer
    return 1;
}

// advance_has_prereq and city_add_trade_route.
static Advance advances[100];
static City cities[8];
static int slot_set;
static int map_road_connection(int, int, int, int, int) { return 0; }
static i32 city_has_improvement(iptr, iptr) { return 0; }
static void city_set_trade_route_slot(uptr, iptr slot, uptr, iptr) { slot_set = (int)slot; }
static void city_calc_commodity_demand(uptr) {}

#include "production.inc"

static void open_paths() {
    memset(&error_state, 0x5a, sizeof error_state);
    error_state.last_open_failed = 0;
    std::string shortp = "C:\\GAME\\SOUND\\X.WAV";
    file_open_path(shortp.c_str(), 0);
    assert(opened == shortp && strcmp((char *)error_state.last_open_path, shortp.c_str()) == 0);
    assert((unsigned char)error_state.last_open_path[shortp.size() + 1] == 0x5a); // as strcpy left it
    assert(error_state.last_open_failed == 1 && error_state.last_alloc_failed == 0x5a5a5a5a);
    std::string longp = "Z:\\" + std::string(300, 'd') + "\\SOUND\\X.WAV"; // a save outside the game
    file_open_path(longp.c_str(), 0);
    assert(opened == longp);
    assert(strlen((char *)error_state.last_open_path) == 79 &&
           memcmp(error_state.last_open_path, longp.c_str(), 79) == 0);
    assert(error_state.last_open_failed == 1 && error_state.last_alloc_failed == 0x5a5a5a5a);
}
static std::string expand(const char *text) {
    char src[256], dest[0x1000];
    strcpy(src, text);
    text_expand_vars((iptr)src, (iptr)dest);
    return dest;
}
static void text_vars() {
    memset(&text_args, 0, sizeof text_args);
    strcpy((char *)text_args.strings + 3 * 0x104, "Rome");
    strcpy((char *)text_args.strings + 23 * 0x104, "last");
    text_args.numbers[2] = 42;
    text_args.numbers[5] = 0xab;
    text_args.numbers[0] = 0x12345;
    assert(expand("in %STRING3 %NUMBER2 %HEX5 100%%") == "in Rome 42 00ab 100%");
    assert(expand("%STRING23.") == "last.");
    assert(expand("a%STRING24b%NUMBER6c%HEX9d") == "abcd"); // out of the tables: nothing
    assert(expand("%HEX0") == "12345"); // longer than 4 digits: no padding (was endless)
}
static void section_lines() {
    lines = {"one", "two"};
    line_pos = 0;
    assert(strcmp(text_read_line_or_end(1), "one") == 0 && strcmp(text_read_line_or_end(1), "two") == 0);
    closed = 0;
    i8 *end = text_read_line_or_end(1); // no @ line before the end of the file
    assert(strcmp(end, "@") == 0 && text_parse.line_cursor == textbuf.buf && closed == 1);
    assert(text_read_line(1) == nullptr && textbuf.buf[0] == 0); // the plain reader still says none
}
static void prereqs() {
    for (int a = 0; a < 100; a++)
        advances[a].preq[0] = advances[a].preq[1] = -1;
    advances[10].preq[0] = 5;
    advances[10].preq[1] = 7;
    advances[7].preq[1] = 3;
    assert(advance_has_prereq(10, 3) && advance_has_prereq(10, 10) && !advance_has_prereq(10, 4));
    assert(!advance_has_prereq(-1, 3) && !advance_has_prereq(3, 10));
    advances[20].preq[0] = 21; // a cycle (Bonaparte's rules have one): an answer, not a stack overflow
    advances[21].preq[0] = 20;
    advances[21].preq[1] = 10;
    assert(advance_has_prereq(20, 3) && !advance_has_prereq(20, 4) && advance_has_prereq(21, 20));
    advances[30].preq[0] = advances[30].preq[1] = 30;
    assert(!advance_has_prereq(30, 1));
}
static void trade_routes() {
    for (int c = 0; c < 8; c++)
        cities[c].owner = (i8)c;
    cities[0].trade_routes = 3;
    for (int r = 0; r < 3; r++) {
        cities[0].route_partner[r] = (i16)(r + 1);
        cities[0].route_goods[r] = 2;
        cities[r + 1].base_trade = (i16)(100 - r);
    }
    cities[4].base_trade = 200;
    slot_set = -5;
    city_add_trade_route(0, 4, 3);
    assert(slot_set == 2); // the weakest route replaced, as before
    for (int r = 0; r < 3; r++)
        cities[r + 1].base_trade = 9999;
    cities[4].base_trade = 9999;
    slot_set = -5;
    city_add_trade_route(0, 4, 3);
    assert(slot_set == -5); // no route under 9999: none replaced (the slot was never set)
}
static int parse(std::vector<std::string> file) {
    lines = file;
    allocated = actions = popups = 0;
    memset(pool, 0, sizeof pool);
    ScenarioEvents *events = (ScenarioEvents *)calloc(1, sizeof(ScenarioEvents));
    int failed = events->parse_file("EVENTS.TXT", "BEGINEVENTS");
    free(events);
    return failed;
}
static void events_file() {
    assert(parse({"@IF", "TURN", "@THEN", "TEXT", "@ENDIF", "@ENDEVENTS"}) == 0);
    assert(allocated == 1 && actions == 1 && pool[0].actions == 1 && popups == 0);
    assert(parse({"@THEN", "TEXT", "@ENDIF", "@IF", "@ENDEVENTS"}) == 1); // an action before any @IF
    assert(actions == 0 && popups == 1);
}
int main() {
    open_paths();
    text_vars();
    section_lines();
    prereqs();
    trade_routes();
    events_file();
    puts("text and game code: long open paths, text arguments, unterminated sections, cyclic "
         "prerequisites, unset trade slot, events action before @IF passed");
}
