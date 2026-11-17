/*
 * (port) Where the game's rules come from (src/include/rules.h): a
 * scenario's own RULES.TXT, the game folder's, or the built-in tables
 * (rules_default.cpp); and the rules' names by key.
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#ifdef CIV2_UNITTEST
#include <stdlib.h>
#include <unistd.h>
#endif
#include "game.h"
#include "lang.h"
#include "rules.h"

/* Tests (civ2-unittest --rules): a source forced, -1: none */
int rules_forced_source = -1;
static int current_source;

/* Whether the folder has the rules file (rules_txt_path). */
static int rules_file_in(const char *dir)
{
    char name[260];
    char saved_cwd[260];
    uint exists;
    crt_strcpy(name, ((i8 *)&main_frame_wnd_block.rules_txt_path));
    path_default_extension(name, (char *)&lang_file_ext);
    __getcwd(saved_cwd, 0x104);
    __chdir((char *)dir);
    exists = file_exists(name);
    __chdir(saved_cwd);
    return exists != 0;
}

int rules_source(void)
{
    /* (what the game folder's file is looked at and its names read with
       leaves no trace in the last file opened: the original read the
       file there, and the parse opens it again) */
    i8 saved_path[sizeof error_state.last_open_path];
    int saved_failed;
    if (rules_forced_source >= 0) {
        current_source = rules_forced_source;
    } else if (__strcmpi(((i8 *)&net_map_lock_slots_block.scenario_dir_path), ((i8 *)&game_dirs.home)) != 0 &&
               rules_file_in(((i8 *)&net_map_lock_slots_block.scenario_dir_path))) {
        current_source = RULES_SCENARIO_FILE;
        return current_source;
    }
    saved_failed = error_state.last_open_failed;
    memcpy(saved_path, error_state.last_open_path, sizeof saved_path);
    if (rules_forced_source < 0)
        current_source = rules_file_in(((i8 *)&game_dirs.home)) ? RULES_GAME_FILE : RULES_BUILTIN;
    /* the names by key: read before the loader opens the file (once) */
    if (current_source != RULES_SCENARIO_FILE)
        lang_load_rules();
    memcpy(error_state.last_open_path, saved_path, sizeof saved_path);
    error_state.last_open_failed = saved_failed;
    return current_source;
}

i32 rules_name(const char *key, int min_size)
{
    lang_load_rules();
    return strpool_add_padded(lang_text(key), (uint)min_size);
}

i32 rules_name_pooled(const char *key)
{
    lang_load_rules();
    return strpool_add(lang_text(key));
}

i32 rules_read_name(const char *key, int min_size)
{
    if (current_source == RULES_SCENARIO_FILE)
        return text_next_token_str_id(min_size);
    text_next_token();
    return rules_name(key, min_size);
}

i32 rules_read_name_pooled(const char *key)
{
    if (current_source == RULES_SCENARIO_FILE)
        return text_next_token_pooled();
    text_next_token();
    return rules_name_pooled(key);
}

/* An advance as RULES.TXT names it (its abbreviation, nil, no). */
static const char *advance_abbrev(int advance)
{
    if (advance == RULES_NIL)
        return "nil";
    if (advance < 0 || advance >= ADV_COUNT)
        return "no";
    return advances[advance].abbrev;
}

/* A terrain as RULES.TXT names it (its abbreviation, yes, no). */
static const char *terrain_abbrev(int terrain)
{
    if (terrain == TERRAIN_RULE_YES)
        return "yes";
    if (terrain < 0 || terrain >= TERRAIN_COUNT)
        return "no";
    return terrain_types[terrain].abbrev;
}

/* A line of the file: printf's, and the newline. */
static void put_line(FILE *f, const char *format, ...)
{
    char line[256];
    va_list args;
    va_start(args, format);
    vsnprintf(line, sizeof line - 1, format, args);
    va_end(args);
    strcat(line, "\n");
    _fputs(line, f);
}

int rules_write_default(const char *name)
{
    FILE *f = file_open_path(name, "wt");
    const u8 *c = (const u8 *)&rules_cosmic;
    int i, t, b;
    char flags[17];

    if (!f)
        return 0;
    put_line(f, "@COSMIC");
    for (i = 0; i < RULES_COSMIC_SIZE; i++)
        put_line(f, "%d", c[i]);
    put_line(f, "\n@CIVILIZE");
    for (i = 0; i < rules_advance_count; i++) {
        const RulesAdvance *a = &rules_advances[i];
        put_line(f, "%s, %d, %d, %s, %s, %d, %d", a->name, a->ai_value, a->civilized_mod, advance_abbrev(a->preq[0]),
                 advance_abbrev(a->preq[1]), a->epoch, a->category);
    }
    put_line(f, "\n@IMPROVE");
    for (i = 0; i < IMP_COUNT; i++)
        put_line(f, "%s, %d, %d, %s,", rules_improvements[i].name, rules_improvements[i].cost, rules_improvements[i].upkeep,
                 advance_abbrev(rules_improvements[i].preq));
    put_line(f, "\n@ENDWONDER");
    for (i = 0; i < WONDER_COUNT; i++)
        put_line(f, "%s,", advance_abbrev(rules_wonder_expires[i]));
    put_line(f, "\n@UNITS");
    for (i = 0; i < rules_unit_count; i++) {
        const RulesUnit *u = &rules_units[i];
        for (b = 0; b < 15; b++)
            flags[b] = (u->flags >> (14 - b)) & 1 ? '1' : '0';
        flags[15] = 0;
        put_line(f, "%s, %s, %d, %d., %d, %da, %dd, %dh, %df, %d, %d, %d, %s, %s", u->name, advance_abbrev(u->obsolete_by),
                 u->domain, u->move, u->range, u->attack, u->defense, u->hit_points, u->firepower, u->cost, u->hold, u->role,
                 advance_abbrev(u->prerequisite), flags);
    }
    put_line(f, "\n@TERRAIN");
    for (i = 0; i < TERRAIN_RECORDS; i++) {
        const RulesTerrain *r = &rules_terrain[i];
        if (i >= TERRAIN_COUNT) {
            put_line(f, "%s, %d, %d, %d, %d, %d,", r->name, r->move_cost, r->defense, r->yields[0], r->yields[1], r->yields[2]);
            continue;
        }
        put_line(f, "%s, %d, %d, %d, %d, %d, %s, %d, %d, %d, %s, %d, %d, %d, %s,", r->name, r->move_cost, r->defense,
                 r->yields[0], r->yields[1], r->yields[2], terrain_abbrev(r->improve_to[0]), r->improve_bonus[0],
                 r->improve_turns[0], r->improve_ai_govt[0], terrain_abbrev(r->improve_to[1]), r->improve_bonus[1],
                 r->improve_turns[1], r->improve_ai_govt[1], terrain_abbrev(r->transform_to));
    }
    put_line(f, "\n@GOVERNMENTS");
    for (i = 0; i < 7; i++)
        put_line(f, "%s, %s, %s", rules_governments[i].name, rules_governments[i].male_title, rules_governments[i].female_title);
    put_line(f, "\n@LEADERS");
    for (i = 0; i < 21; i++) {
        const RulesLeader *l = &rules_leaders[i];
        char line[256];
        snprintf(line, sizeof line, "%s, %s, %d, %d, %d, %s, %s, %d, %d, %d,", l->male_name, l->female_name, l->female, l->color,
                 l->city_style, l->plural, l->adjective, l->attack, l->expand, l->civilize);
        for (t = 0; t < 4 && l->titles[t].govt > 0; t++)
            snprintf(line + strlen(line), sizeof line - strlen(line), "%s %d, %s, %s", t ? "," : "", l->titles[t].govt,
                     l->titles[t].male, l->titles[t].female);
        put_line(f, "%s", line);
    }
    put_line(f, "\n@CARAVAN");
    for (i = 0; i < 16; i++)
        put_line(f, "%s,", rules_commodities[i]);
    put_line(f, "\n@ORDERS");
    for (i = 0; i < 12; i++)
        put_line(f, *rules_orders[i].name ? "%s, %s" : "", rules_orders[i].name, rules_orders[i].key);
    put_line(f, "\n@DIFFICULTY");
    for (i = 0; i < 6; i++)
        put_line(f, "%s", rules_difficulties[i]);
    put_line(f, "\n@ATTITUDES");
    for (i = 0; i < 9; i++)
        put_line(f, "%s", rules_attitudes[i]);
    put_line(f, "");
    _fclose(f);
    return 1;
}

#ifdef CIV2_UNITTEST
/* civ2-unittest --rules GAME (make test): the built-in rules against the
   game's RULES.TXT.  The rules are loaded three times from the same state:
   built in, from the game folder's RULES.TXT (names by key) and as the
   original read it (names from the file); everything the loaders set,
   the string pool's bytes included, must be the same.  0: the same. */
struct RulesRegion {
    const char *name;
    void *data;
    size_t size;
};

static std::vector<RulesRegion> rules_regions(void)
{
    std::vector<RulesRegion> r = {
        {"cosmic", &cosmic, sizeof cosmic},
        {"advances", advances, sizeof(Advance) * ADV_COUNT},
        {"game.num_advances", &game.num_advances, sizeof game.num_advances},
        {"improvements", improvements, sizeof(Improvement) * IMP_COUNT},
        {"wonder_expires", wonder_expires, WONDER_COUNT},
        {"unit_types", unit_types, sizeof(UnitType) * UNIT_TYPE_COUNT},
        {"double_moves_applied", &double_moves_applied, sizeof double_moves_applied},
        {"terrain_types", terrain_types, sizeof(Terrain) * TERRAIN_RECORDS},
        {"government_names", government_names, sizeof(i32) * 7},
        {"government_titles", government_titles, sizeof(GovTitle) * 7},
        {"leaders", leaders, sizeof(Leader) * 21},
        {"commodity_names", commodity_names, sizeof(i32) * 16},
        {"order_names", order_names, sizeof(OrderName) * 13},
        {"difficulty_names", difficulty_names, sizeof(i32) * 6},
        {"attitude_names", attitude_names, sizeof(i32) * 9},
        {"strpool.count", &strpool.count, sizeof strpool.count},
    };
    return r;
}

/* What the loaders made: the regions, then the pool's bytes, then the
   unit names' pointers as offsets into the pool. */
static std::vector<std::vector<u8> > rules_snapshot(void)
{
    std::vector<std::vector<u8> > s;
    MemBlock *pool = (MemBlock *)&strpool_block_block.strpool_block;
    for (const RulesRegion &r : rules_regions())
        s.push_back(std::vector<u8>((u8 *)r.data, (u8 *)r.data + r.size));
    s.push_back(std::vector<u8>((u8 *)(uintptr_t)pool->base_ptr, (u8 *)(uintptr_t)pool->base_ptr + pool->used));
    std::vector<u8> ptrs;
    for (int i = 0; i < UNIT_TYPE_COUNT; i++) {
        u32 off = (u32)prefs_main_menu_choice_block.unit_type_name_ptrs[i] - pool->base_ptr;
        ptrs.insert(ptrs.end(), (u8 *)&off, (u8 *)&off + 4);
    }
    s.push_back(ptrs);
    return s;
}

EXTERN_C int rules_check(const char *dir)
{
    static const char *const names[] = {"built in", "the game's RULES.TXT, names by key", "RULES.TXT as the original read it",
                                        "the built-in rules written out (rules_write_default)"};
    static const int sources[] = {RULES_BUILTIN, RULES_GAME_FILE, RULES_SCENARIO_FILE};
    std::vector<RulesRegion> regions = rules_regions();
    std::vector<std::vector<u8> > before, got[4];
    int k, fails = 0;
    size_t i, b;

    __chdir((char *)dir);
    __getcwd(((i8 *)&game_dirs.home), 0x104);
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path), ((i8 *)&game_dirs.home));
    crt_strcpy(((i8 *)&main_frame_wnd_block.rules_txt_path), str_rules);
    if (!rules_file_in(((i8 *)&game_dirs.home))) {
        printf("rules: no RULES.TXT in %s: skipped\n", dir);
        return 0;
    }
    for (const RulesRegion &r : regions)
        before.push_back(std::vector<u8>((u8 *)r.data, (u8 *)r.data + r.size));
    for (k = 0; k < 3; k++) {
        for (i = 0; i < regions.size(); i++)
            memcpy(regions[i].data, before[i].data(), regions[i].size);
        strpool_init(0x8000);
        rules_forced_source = sources[k];
        rules_parse_sections(1);
        got[k] = rules_snapshot();
    }
    /* and the built-in rules written out (rules_write_default), read back */
    {
        char tmp[] = "/tmp/civ2-rulesXXXXXX";
        if (!mkdtemp(tmp)) {
            printf("rules: no temporary folder\n");
            return 1;
        }
        __chdir(tmp);
        __getcwd(((i8 *)&game_dirs.home), 0x104);
        crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path), ((i8 *)&game_dirs.home));
        if (!rules_write_default("RULES.TXT")) {
            printf("rules: RULES.TXT not written in %s\n", tmp);
            return 1;
        }
        for (i = 0; i < regions.size(); i++)
            memcpy(regions[i].data, before[i].data(), regions[i].size);
        strpool_init(0x8000);
        rules_forced_source = RULES_SCENARIO_FILE;
        rules_parse_sections(1);
        got[3] = rules_snapshot();
        remove("RULES.TXT");
        __chdir((char *)dir);
        rmdir(tmp);
    }
    rules_forced_source = -1;
    for (k = 1; k < 4; k++)
        for (i = 0; i < got[0].size(); i++) {
            const char *what = i < regions.size() ? regions[i].name : i == regions.size() ? "the string pool" : "unit_type_name_ptrs";
            if (got[0][i] == got[k][i])
                continue;
            for (b = 0; b < got[0][i].size() && b < got[k][i].size() && got[0][i][b] == got[k][i][b]; b++)
                ;
            printf("rules: %s differs from %s: %s, at byte %u (of %u, %u)\n", names[0], names[k], what, (unsigned)b,
                   (unsigned)got[0][i].size(), (unsigned)got[k][i].size());
            fails++;
        }
    if (!fails)
        printf("rules: built in = the game's RULES.TXT = written out (%u string pool bytes, %d names)\n",
               (unsigned)got[0][regions.size()].size(), strpool.count);
    return fails != 0;
}
#endif
