// SOURCE src/auto/text.cpp str_copy_max
// SOURCE src/auto/text.cpp str_append_max
// SOURCE src/auto/text.cpp text_expand_vars_max
// SOURCE src/auto/text.cpp labels_load
// SOURCE src/auto/civ.cpp cosmic_editor_write_section
// SOURCE src/auto/reports.cpp hall_of_fame_clear
// SOURCE src/auto/reports.cpp hall_of_fame_valid
// SOURCE src/auto/reports.cpp hall_of_fame_load
// SOURCE src/auto/support.cpp error_report
// SOURCE src/auto/window.cpp dialog_prepend_title_label
// SOURCE src/auto/game.cpp save_filter_append_label
// SOURCE src/auto/game.cpp save_file_filter
// SOURCE src/auto/game.cpp save_filter_add_entry
// SOURCE src/auto/map.cpp terrain_editor_on_edit_river
// SOURCE src/auto/map.cpp terrain_editor_on_edit_coast
// SOURCE src/auto/map.cpp terrain_editor_on_edit_misc
// SOURCE src/auto/pedia.cpp PediaWindow::load_wonder_index
// SOURCE src/auto/pedia.cpp PediaWindow::append_unit_fact
// SOURCE src/auto/network.cpp parley_desc_append
// Fixed buffers of the original overflowed only by modified game text (Peter
// Johnson's ledger BF-029, 038, 053, 057, 061, 063, 072, 082, 096, 116, 118,
// 198, 212): the actual bodies with small providers. The game's own text keeps
// the original's results; longer text is cut (or a choice refused) within the
// buffer, checked with guard bytes after each one.
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

#define crt_strcpy strcpy
#define crt_strcat strcat
#define crt_memcpy memcpy
#define _strlen strlen
#define _memset memset
#define _strchr strchr
#define _sprintf sprintf
#define _atoi atoi
#define __strnicmp strncasecmp
#define _strncmp strncmp
static const unsigned char GUARD = 0xa5;
static bool guard_ok(const void *p, size_t n) {
    for (size_t i = 0; i < n; i++)
        if (((const unsigned char *)p)[i] != GUARD)
            return false;
    return true;
}

// The game's text: labels by string id (0x10000 + label index), lines of a section.
static std::vector<std::string> label_texts(0x400);
static char *label_string(int id) { return (char *)label_texts[id - 0x10000].c_str(); }
static std::vector<std::string> lines;
static size_t line_pos;
#pragma pack(push, 1)
static struct {
    char buf[0x9a8];
    unsigned char guard[16];
} textbuf;
#pragma pack(pop)
static i8 *text_read_line(int) {
    if (line_pos >= lines.size()) {
        textbuf.buf[0] = 0;
        return nullptr;
    }
    strcpy(textbuf.buf, lines[line_pos++].c_str());
    return textbuf.buf;
}
static i8 *text_read_line_or_end(int) {
    i8 *line = text_read_line(1);
    if (!line) {
        strcpy(textbuf.buf, "@");
        line = textbuf.buf;
    }
    return line;
}
static i32 text_next_int() { return atoi(textbuf.buf); }
static int closed;
static void text_close_file() { closed++; }
static int section_missing;
static int text_open_section(const char *, const char *) {
    line_pos = 0;
    return section_missing;
}

// text_expand_vars_max: the arguments as laid out in the game (24 strings).
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
static char *__ltoa(long value, char *buf, int radix) {
    sprintf(buf, radix == 16 ? "%lx" : "%ld", value);
    return buf;
}
static char *__itoa(int value, char *buf, int radix) { return __ltoa(value, buf, radix); }

// labels_load
static struct {
    int ids_handle;
    i32 *string_ids;
    i32 count;
} label_table;
static char str_labels_2[] = "LABELS", str_labels_3[] = "LABELS", str_labels_4[] = "LABELS",
            str_labels_5[] = "LABELS", str_error_labels_txt[] = "ERROR";
static struct {
    char scenario_dir_path[264];
} net_map_lock_slots_block;
static struct {
    char home[264];
} game_dirs;
static unsigned char *ids_block;
static int mem_handle_alloc_tracked(int size) {
    ids_block = (unsigned char *)malloc(size + 16);
    memset(ids_block + size, GUARD, 16);
    return size;
}
static void *mem_lock_handle(int) { return ids_block; }
static int pooled;
static i32 text_read_line_pooled() {
    text_read_line(1);
    return 0x10000 + pooled++;
}
static i32 strpool_add(const char *) { return 0x10000; }
static int labels_freed, labels_added;
static void labels_free() { labels_freed++; }
static void lang_add_labels(int count) { labels_added = count; }

// error_report: the 80-byte context before the last open path.
#pragma pack(push, 1)
static struct {
    char context_text[80];
    i8 last_open_path[80];
} error_state;
#pragma pack(pop)
static char str_errors_db[] = "ERRORS", str_modules_db[] = "MODULES";
static void file_read_nth_line(iptr, const char *, int) {}
static int fatal_exits;
static void err_fatal_exit(iptr, iptr, iptr, iptr, i32) { fatal_exits++; }

// cosmic_editor_write_section
static struct {
    struct {
        i32 values[0x20];
    } editor_item_recs[1];
} editor_text_dlg_wnd_block;
static char str_8d_s[] = "%-8d%s";
static std::vector<std::string> cosmic_in;
static size_t cosmic_pos;
static std::string cosmic_out;
static char *_fgets(char *buf, int n, FILE *) {
    if (cosmic_pos >= cosmic_in.size())
        return nullptr;
    snprintf(buf, n, "%s", cosmic_in[cosmic_pos++].c_str());
    return buf;
}
static int _fputs(const char *s, FILE *) {
    cosmic_out += s;
    return 0;
}

// hall_of_fame_load: the six places and the file.
static struct {
    HallOfFameEntry hall_of_fame[6];
} advisor_window_block;
static char str_hallfame_dat[] = "HALLFAME.DAT";
static i8 hall_of_fame_read_mode[] = "rb";
static std::string hof_file;
static size_t hof_pos;
static FILE *_fopen(const char *, const char *) { return hof_file.empty() ? nullptr : (FILE *)&hof_file; }
static size_t _fread(void *buf, size_t size, size_t count, FILE *) {
    if (hof_pos + size * count > hof_file.size())
        return 0;
    memcpy(buf, hof_file.data() + hof_pos, size * count);
    hof_pos += size * count;
    return count;
}
static int _fclose(FILE *) { return 0; }

// dialog_prepend_title_label: the title at +0x134, the pool at +0x254.
static char dialog_title_label_space_str[] = " ";
static void str_append_label(iptr dest, int label_id) { strcat((char *)dest, label_string(0x10000 + label_id)); }
static i32 memblock_alloc(MemBlock *, int size) { return (i32)(iptr)malloc(size); }

// save_file_filter
static struct {
    i8 save_file_filter_buf[264];
    unsigned char guard[16];
} grey_palette_block;
static struct {
    char multiplayer;
} game;
static char str_mp[] = " (*.mp)", str_scn[] = " (*.scn)", str_scn_2[] = "*.scn", str_sav[] = " (*.sav)",
            str_sav_2[] = "*.sav", str_hot[] = " (*.hot)", str_hot_2[] = "*.hot", str_eml[] = " (*.eml)",
            str_eml_2[] = "*.eml", str_net[] = " (*.net)", str_net_2[] = "*.net";
static i8 save_filter_mp_pattern_str[] = "*.mp";

// The terrain editor's icon choices: the sprite each choice opens.
static struct {
    EditorWindow *wnd;
} editor_active;
static struct {
    int help_pressed;
} dlg_input;
static char str_debug_11[] = "DEBUG", str_river[] = "RIVER", str_coast[] = "COAST", str_terrmisc[] = "TERRMISC",
            str_help_9[] = "", str_help_10[] = "", str_help_11[] = "", str_helpriver[] = "", str_helpcoast[] = "",
            str_helpterrmisc[] = "";
static void dlg_set_parent_window(iptr) {}
static int next_choice;
static int popup_text_flags(const char *, const char *, int) { return next_choice; }
static void popup_text(const char *, const char *) {}
static void terrain_editor_redraw() {}
static i8 *edited;
static void sprite_editor_run(iptr sprites, const char *, int, SpriteSheetSaveFn) { edited = (i8 *)sprites; }
static void sprite_sheet_save_terrain1() {}
static void sprite_sheet_save_terrain2() {}
static struct {
    i8 river_sprites[16 * 0x3c];
} icon_misc_sprites_block;
static struct {
    i8 river_mouth_sprites[240];
} tile_pick_mask_sprite_block;
static struct {
    i8 coast_quarter_sprites[60];
    i8 coast_quarter_east_sprites[60];
    i8 coast_quarter_south_sprites[60];
    i8 coast_quarter_west_sprites[1740];
    i8 next_sprite[60];  // the one after them (the original's 0x6442b8)
} tutorial_icon_sprite_block;
static struct {
    i8 land_improvement_sprites[184];
} land_improvement_sprites_block;
static struct {
    i8 pollution_sprite[60];
} pollution_sprite_block;
static i8 grassland_shield_sprite[60], hut_sprite[60];
static struct {
    i8 road_sprites[0x21c * 2];
} road_sprites_block;

// The Civilopedia's wonder index and unit facts.
static Improvement improvements[0x43];
static UnitType unit_types[0x3e];
static char str_describe_3[] = "DESCRIBE", str_wonder_index_2[] = "WONDER_INDEX";
static void *operator_new(size_t n) { return malloc(n); }
void PediaWindow::sort_list(PediaListNode *, i32 *, i32 *, iptr) {}
static i8 append_space_str[] = " ";

// The parley's description: its 2464 bytes.
#pragma pack(push, 1)
static struct {
    i8 parley_description[2464];
    unsigned char guard[16];
} text_section_index_pool_block;
#pragma pack(pop)

#define TEXTBUF_SIZE sizeof textbuf.buf
static void error_report(uint error_code, int module_id, iptr context, iptr data1, iptr data2);
static char *save_filter_add_entry(i32 kind, char *buf);
#include "production.inc"

static void bounded_copies() {
    char buf[8 + 16];
    memset(buf, GUARD, sizeof buf);
    str_copy_max(buf, "abc", 8);
    assert(strcmp(buf, "abc") == 0 && (unsigned char)buf[4] == GUARD); // as strcpy leaves it
    str_append_max(buf, "defghijkl", 8);
    assert(strcmp(buf, "abcdefg") == 0 && guard_ok(buf + 8, 16));
    str_append_max(buf, "x", 8);
    assert(strcmp(buf, "abcdefg") == 0 && guard_ok(buf + 8, 16));
    str_copy_max(buf, "0123456789", 8);
    assert(strcmp(buf, "0123456") == 0 && guard_ok(buf + 8, 16));
    // BF-063: seventeen %STRING0 of a 150-character name into 256 bytes
    memset(&text_args, 0, sizeof text_args);
    memset(text_args.strings, 'r', 150);
    std::string src;
    for (int i = 0; i < 17; i++)
        src += "%STRING0 ";
    char line[256], dest[256 + 16];
    strcpy(line, src.c_str());
    memset(dest, GUARD, sizeof dest);
    text_expand_vars_max((iptr)line, (iptr)dest, 256);
    assert(strlen(dest) == 255 && guard_ok(dest + 256, 16));
    strcpy(line, "in %STRING0, 100%%");
    strcpy((char *)text_args.strings, "Rome");
    text_expand_vars_max((iptr)line, (iptr)dest, 256);
    assert(strcmp(dest, "in Rome, 100%") == 0);
}
static void labels() {
    lines = {"1000"};
    for (int i = 0; i < 1000; i++)
        lines.push_back("label");
    pooled = 0;
    assert(labels_load() == 0x378 && label_table.count == 0x378 && guard_ok(ids_block + 0xde0, 16)); // BF-096
    assert(labels_added == 0x378);
    lines = {"888"}; // the game's own LABELS.TXT
    lines.resize(889, "label");
    pooled = 0;
    assert(labels_load() == 888 && label_table.string_ids[887] == 0x10000 + 887);
}
static void cosmic() {
    for (int i = 0; i < 0x16; i++)
        editor_text_dlg_wnd_block.editor_item_recs[0].values[i] = i + 1;
    cosmic_in = {"3       ; Road movement multiplier\n", "2       ; no comment\n"};
    cosmic_in[1] = "2\n"; // BF-029: a line without a comment, then the end of the file
    cosmic_pos = 0;
    cosmic_out.clear();
    assert(cosmic_editor_write_section(nullptr, nullptr) == 1);
    std::string want = "1       ; Road movement multiplier\n2       \n";
    for (int i = 3; i <= 0x16; i++)
        want += std::to_string(i) + std::string(8 - std::to_string(i).size(), ' ') + "\n";
    assert(cosmic_out == want);
}
static void hall_of_fame() {
    HallOfFameEntry places[6];
    memset(places, 0, sizeof places);
    for (int i = 0; i < 6; i++) {
        places[i].score = -1;
        places[i].rating = -1;
    }
    places[0].score = 500;
    places[0].difficulty = 5 | 0x80; // cheated deity
    places[0].title_index = 23;
    strcpy(places[0].leader_name, "Caesar");
    strcpy(places[0].civ_name, "Romans");
    places[1].score = (i16)40000; // B15: over 32767
    places[1].title_index = 3;
    hof_file.assign((char *)places, sizeof places);
    hof_pos = 0;
    hall_of_fame_load();
    assert(advisor_window_block.hall_of_fame[0].score == 500 && advisor_window_block.hall_of_fame[1].title_index == 3);
    HallOfFameEntry bad[6];
    for (int k = 0; k < 4; k++) { // BF-198: a title, difficulty or name the game does not write
        memcpy(bad, places, sizeof bad);
        if (k == 0)
            bad[0].title_index = 24;
        else if (k == 1)
            bad[0].difficulty = 6;
        else if (k == 2)
            memset(bad[0].leader_name, 'x', 24);
        else
            bad[1].title_index = -1;
        hof_file.assign((char *)bad, sizeof bad);
        hof_pos = 0;
        hall_of_fame_load();
        for (int i = 0; i < 6; i++)
            assert(advisor_window_block.hall_of_fame[i].score == -1);
    }
}
static void error_context() {
    memset(&error_state, GUARD, sizeof error_state);
    std::string line(300, 'w'); // BF-057: a long line of RULES.TXT
    error_report(0xfffffffc, 3, (iptr)line.c_str(), 1, 2);
    assert(strlen(error_state.context_text) == 79 && guard_ok(error_state.last_open_path, 80) && fatal_exits == 1);
}
static void dialog_title() {
    static unsigned char dialog[0x300];
    label_texts[0x25e] = std::string(254, 'p'); // BF-061: a long label before a long title
    *(i32 *)(dialog + 0x134) = (i32)(iptr)strdup(std::string(250, 't').c_str());
    dialog_prepend_title_label((iptr)dialog, 0x25e);
    std::string title = (char *)(iptr) * (i32 *)(dialog + 0x134);
    assert(title == std::string(254, 'p') + " " + std::string(250, 't'));
    dialog_prepend_title_label((iptr)dialog, 0x25e);
    title = (char *)(iptr) * (i32 *)(dialog + 0x134);
    assert(title.size() == 511 && title.compare(0, 255, std::string(254, 'p') + " ") == 0);
    label_texts[0x25e] = "Player 1"; // the game's
    *(i32 *)(dialog + 0x134) = (i32)(iptr)strdup("Choose");
    dialog_prepend_title_label((iptr)dialog, 0x25e);
    assert(strcmp((char *)(iptr) * (i32 *)(dialog + 0x134), "Player 1 Choose") == 0);
}
static std::vector<std::string> filters() {
    std::vector<std::string> out;
    for (const char *p = grey_palette_block.save_file_filter_buf; *p; p += strlen(p) + 1)
        out.push_back(p);
    return out;
}
static void save_filters() {
    label_texts[0xee] = "Saved Games";
    label_texts[0x357] = "Hot Seat Games";
    label_texts[0x35d] = "Network Games";
    game.multiplayer = 0;
    memset(&grey_palette_block, GUARD, sizeof grey_palette_block);
    save_file_filter(0, 0, 0);
    std::vector<std::string> want = {"Saved Games (*.sav)", "*.sav", "Network Games (*.net)", "*.net",
                                     "Hot Seat Games (*.hot)", "*.hot"};
    assert(filters() == want);
    for (int id : {0xee, 0x357, 0x35a, 0x35d, 0x105, 0x19f}) // BF-038: labels of 254 characters
        label_texts[id] = std::string(254, 'L');
    for (int mp : {0, 1, 2, 3}) {
        game.multiplayer = (char)mp;
        memset(&grey_palette_block, GUARD, sizeof grey_palette_block);
        save_file_filter(0, 0, 0);
        assert(guard_ok(grey_palette_block.guard, 16) && filters().size() >= 4);
        assert(filters()[0] == std::string(48, 'L') + " (*.sav)" || filters()[0] == std::string(48, 'L') + " (*.hot)" ||
               filters()[0] == std::string(48, 'L') + " (*.net)" || filters()[0] == std::string(48, 'L') + " (*.eml)");
    }
    memset(&grey_palette_block, GUARD, sizeof grey_palette_block);
    save_file_filter(0, 1, 0);
    assert(guard_ok(grey_palette_block.guard, 16) && filters()[1] == "*.scn");
}
static void terrain_icons() {
    editor_active.wnd = (EditorWindow *)calloc(1, sizeof(EditorWindow));
    label_table.string_ids = (i32 *)calloc(0x378, 4);
    for (int i = 0; i < 0x378; i++)
        label_table.string_ids[i] = 0x10000 + i;
    next_choice = 19;
    edited = nullptr;
    terrain_editor_on_edit_river();
    assert(edited == tile_pick_mask_sprite_block.river_mouth_sprites + 3 * 0x3c);
    next_choice = 20; // BF-072: a 21st row of a scenario's @RIVER
    edited = nullptr;
    terrain_editor_on_edit_river();
    assert(edited == nullptr);
    next_choice = 31;
    terrain_editor_on_edit_coast();
    assert(edited == tutorial_icon_sprite_block.coast_quarter_sprites + 3 * 0x3c + 7 * 0xf0);
    next_choice = 32; // the original's: the sprite after the coast
    terrain_editor_on_edit_coast();
    assert(edited == tutorial_icon_sprite_block.next_sprite);
    for (int choice : {33, 100, -2}) { // BF-118
        next_choice = choice;
        edited = nullptr;
        terrain_editor_on_edit_coast();
        assert(edited == nullptr);
    }
    next_choice = 23;
    terrain_editor_on_edit_misc();
    assert(edited == road_sprites_block.road_sprites + 8 * 0x3c + 0x21c);
    next_choice = 24; // BF-116
    edited = nullptr;
    terrain_editor_on_edit_misc();
    assert(edited == nullptr);
}
static void pedia() {
    PediaWindow *pedia = (PediaWindow *)calloc(1, sizeof(PediaWindow));
    for (int i = 0; i < 0x43; i++) {
        improvements[i].name = 0x10000 + 0x300;
        improvements[i].preq = 0;
    }
    label_texts[0x300] = "Wonder";
    lines = {"@WONDER_INDEX"};
    for (int i = 0; i < 40; i++) // BF-212: a DESCRIBE.TXT index longer than the 28 wonders
        lines.push_back(std::to_string(i));
    lines[0] = "0";
    PediaListNode *prev = nullptr;
    pedia->load_wonder_index(prev);
    assert(pedia->pedia_wonder_count == 28 && pedia->pedia_wonder_ids[27] == 0x42);
    lines = {"5", "3", "-2"}; // ended as the game's
    pedia->pedia_wonder_names = nullptr;
    pedia->load_wonder_index(prev);
    assert(pedia->pedia_wonder_count == 2);

    char desc[1000 + 16];
    memset(desc, GUARD, sizeof desc);
    int has_text = 0;
    unit_types[1].flags = 0x7fff;
    lines.clear();
    for (int i = 0; i < 15; i++) // BF-053: fifteen long facts
        lines.push_back("^" + std::string(200, 'f'));
    line_pos = 0;
    for (int i = 0; i < 15; i++)
        pedia->append_unit_fact(1, 1u << i, desc, has_text);
    assert(strlen(desc) == 999 && guard_ok(desc + 1000, 16));
    lines = {"^Can see units two spaces away.", "^Ignores enemy Zones of Control."};
    line_pos = 0;
    has_text = 0;
    pedia->append_unit_fact(1, 1, desc, has_text);
    pedia->append_unit_fact(1, 2, desc, has_text);
    assert(strcmp(desc, "Can see units two spaces away. Ignores enemy Zones of Control. ") == 0);
}
static void parley() {
    memset(&text_section_index_pool_block, GUARD, sizeof text_section_index_pool_block);
    text_section_index_pool_block.parley_description[0] = 0;
    for (int i = 0; i < 12; i++) // BF-082: twelve advances of 220 characters
        parley_desc_append(std::string(220, 'a').c_str());
    assert(strlen(text_section_index_pool_block.parley_description) == 0x7ff);
    assert(guard_ok(text_section_index_pool_block.parley_description + 0x800, 2464 - 0x800));
}
int main() {
    bounded_copies();
    labels();
    cosmic();
    hall_of_fame();
    error_context();
    dialog_title();
    save_filters();
    terrain_icons();
    pedia();
    parley();
    puts("modified game text: bounded copies and expansions, labels count, cosmic section end, Hall of Fame "
         "places, error context, dialog title, save filters, terrain icon choices, Civilopedia index and "
         "unit facts, parley description passed");
}
