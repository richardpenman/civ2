/*
 * Civilization II data types, recovered from the original program.
 *
 * This header is the source of truth for the reconstruction's types: it is
 * parsed by Ghidra (tools/ghidra_scripts/ApplyTypes.java) so the decompiler
 * prints field accesses, and included by all C code.  Keep it to plain C that
 * Ghidra's parser understands: no includes, no macros beyond simple ones.
 *
 * Records that the game writes to save files byte-for-byte use only
 * fixed-width integer fields, so their layout is the same on every platform.
 */
#ifndef CIV2_TYPES_H
#define CIV2_TYPES_H

/* plain char: the game keeps text in i8 buffers.  Signed on every target
   (native.h checks), so arithmetic is that of signed char. */
typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;

/* Integers that hold addresses (the original freely mixed the two): exactly
 * int/unsigned in the 32-bit build, pointer-sized in a native 64-bit build,
 * which defines CIV2_PTR_INTS itself (intptr_t / uintptr_t). */
#ifndef CIV2_PTR_INTS
typedef int iptr;
typedef unsigned int uptr;
#endif

/* Records saved to disk are byte-packed (the original used 1-byte packing). */
#pragma pack(push, 1)

/* One unit (0x20 bytes; 2048 slots, saved verbatim). */
typedef struct Unit {
    i16 x;              /* 0x00 map position */
    i16 y;              /* 0x02 */
    u16 flags;          /* 0x04 veteran, etc. */
    u8 type;            /* 0x06 index into the unit-type table */
    i8 owner;           /* 0x07 civilization */
    u8 moves_used;      /* 0x08 movement points spent this turn */
    u8 visibility;      /* 0x09 bit per civilization that has seen it */
    u8 damage;          /* 0x0a hit points lost */
    u8 last_dir;        /* 0x0b direction of the last move */
    union { u8 field_0c; u8 ai_task; };
    u8 counter;         /* 0x0d work counter (terrain improvement) / cargo */
    union { u8 field_0e; u8 aux_timer; };
    i8 orders;          /* 0x0f -1 none, 1 fortifying, 2 fortified, 0x0b goto, ... */
    i8 home_city;       /* 0x10 -1 none */
    u8 pad_11;
    i16 goto_x;         /* 0x12 */
    i16 goto_y;         /* 0x14 */
    i16 prev_in_stack;  /* 0x16 */
    i16 next_in_stack;  /* 0x18 */
    i32 id;             /* 0x1a unique id; 0 = free slot */
    u16 pad_1e;
} Unit;

/* One city (0x58 bytes; 256 slots, saved verbatim). */
typedef struct City {
    i16 x;                  /* 0x00 */
    i16 y;                  /* 0x02 */
    u32 flags;              /* 0x04 disorder, celebrating, coastal, ... */
    i8 owner;               /* 0x08 civilization */
    i8 size;                /* 0x09 */
    i8 founder;             /* 0x0a civilization that founded it */
    u8 turns_captured;      /* 0x0b */
    u8 known_to;            /* 0x0c persistent discovery mask, not current tile visibility */
    u8 revealed_size[8];    /* 0x0d size as last seen by each civilization */
    u8 field_15;
    u32 specialists;        /* 0x16 two bits per specialist */
    i16 food_box;           /* 0x1a */
    i16 shield_box;         /* 0x1c */
    i16 base_trade;         /* 0x1e */
    char name[16];          /* 0x20 */
    u32 worked_tiles;       /* 0x30 worked-square bits 0..20; specialist count in bits 26..31 */
    u8 improvements[5];     /* 0x34 bit per city improvement */
    i8 building;            /* 0x39 production: >= 0 unit type, < 0 improvement */
    u8 trade_routes;        /* 0x3a number of routes */
    u8 supplies[3];         /* 0x3b commodities supplied */
    u8 demands[3];          /* 0x3e commodities demanded */
    u8 route_goods[3];      /* 0x41 commodity carried on each route */
    i16 route_partner[3];   /* 0x44 partner city of each route */
    i16 science;            /* 0x4a */
    i16 tax;                /* 0x4c */
    i16 trade;              /* 0x4e */
    u8 total_food;          /* 0x50 */
    u8 total_shields;       /* 0x51 */
    u8 happy;               /* 0x52 */
    u8 unhappy;             /* 0x53 */
    i32 id;                 /* 0x54 unique id; 0 = free slot */
} City;

#pragma pack(pop)

/* One unit type (0x14 bytes; loaded from RULES.TXT @UNITS). */
typedef struct UnitType {
    i32 name;               /* 0x00 string id (text via the string pool) */
    u32 flags;              /* 0x04 abilities (RULES.TXT flag string) */
    i8 obsolete_by;         /* 0x08 advance that makes it obsolete, -1 never */
    u8 domain;              /* 0x09 0 land, 1 air, 2 sea */
    u8 move;                /* 0x0a movement points x3 */
    u8 range;               /* 0x0b air units: turns aloft */
    u8 attack;              /* 0x0c */
    u8 defense;             /* 0x0d */
    u8 hit_points;          /* 0x0e x10 */
    u8 firepower;           /* 0x0f */
    u8 cost;                /* 0x10 shields x10 */
    u8 hold;                /* 0x11 transport capacity */
    u8 role;                /* 0x12 AI role */
    i8 prerequisite;        /* 0x13 advance required, -1 none */
} UnitType;


/* ===== map ===== */
/*
 * world map (header, tile record, map layers).
 *
 * Evidence:
 *  - map_write 005b8635 / map_read 005b8783 fwrite/fread exactly 0xe bytes at
 *    0x6d1160 (the header), then (non-scenario) 7 per-civ layers of
 *    map.area bytes from 0x6365c0[1..7] (scenario: 2 x 0x2a bytes of start
 *    positions at 0x627fe0/0x628010 instead), then map.area*6 bytes of tiles
 *    from *0x636598.  So the tile record is 6 bytes and map.area = tile count.
 *  - map_alloc 005b7fe0: quarter_w = (width+3)>>2, quarter_h = (height+3)>>2,
 *    area = (width/2)*height; tiles initialised to {10,0,0,0,0,0} (ocean);
 *    allocates handles 0x6d1170 (tiles), 0x6365a0[1..7] (per-civ), 0x6d1174..
 *    0x6d1180 (4 coarse grids of quarter_w*quarter_h bytes), locks each into
 *    the pointer globals 0x636598 / 0x6365c0[] / 0x6365e0..0x6365ec, and sets
 *    0x6365f0 = 1.  map_free 005b8416 undoes it.
 *  - tile_at 005b8931: off-map -> &0x6d1188 (dummy tile), else
 *    tiles + (width&~1)*y*3 + (x&~1)*3, i.e. tiles[(width/2)*y + x/2]:
 *    width counts doubled (isometric) x coordinates, one record per x pair.
 *  - map.flat: set from game flag 0x655ae8 & 0x8000; when that flag is clear
 *    map_wrap_x 005ae052 wraps x around width (round world).
 *  - map.seed: map_new_seed 005b85fe = rand() & 0x7fff (never 0); used by
 *    tile_resource 005b8ee1 and tile_has_hut 005b8ffa to place specials/huts.
 *  - Tile bytes, from the accessors/setters:
 *      +0 terrain: tile_terrain = &0xf (10 = ocean, tile_is_ocean); bit 0x40
 *         suppresses the special resource (tile_resource); bit 0x80 river
 *         (tested all over movement/production code); tile_set_terrain keeps
 *         the high nibble.
 *      +1 improvements: tile_improvements; 0x01 unit present (tile_unit_owner),
 *         0x02 city ((&0x42)==2, tile_city_owner), 0x42 airbase
 *         (tile_airbase_owner), 0x40 fortress, 0x20 railroad, 0x08 mine,
 *         0x04 irrigation, 0x0c farmland, 0x80 pollution (tile_add_pollution
 *         sets 0x80).  Nuclear blast 005b9179 clears 0x40/0x20/0x08/0x04.
 *         0x10 = road per save-format knowledge (only weakly seen in code).
 *      +2 bits 5-7: civ whose city radius covers the tile (city founding
 *         0x43f7a7 writes the owner into the 21 radius tiles via
 *         tile_set_radius_civ 005b9c49); bits 0-4 unknown.
 *      +3 landmass / body of water id (tile_landmass, tile_set_landmass).
 *      +4 visibility: bit per civ (tile_visible_to 005b8b65, setter 005b976d).
 *      +5 high nibble: civ owning the unit/city on the tile, 0xf = none
 *         (tile_owner returns -1); low nibble unknown (setter 005b98b7,
 *         code keeps &7 / sets |8).
 *  - Per-civ layer 0x6365c0[civ] (civ 1..7): one byte per tile, the tile
 *    improvements as last seen by that civ (005b8b1a copies tile byte +1 into
 *    it; known_at 005b898b = layer + (width>>1)*y + (x>>1)).
 *  - Coarse grids 0x6365e0[0..3]: quarter_w x quarter_h bytes (one per 4x4
 *    block), cell = grid + quarter_w*y + x.  0055a980 fills [0] and [1] with
 *    per-direction connectivity bits (domain 0 / 1, likely land / sea path
 *    finding); [0],[1] are saved with the game.  Exact meaning of [2],[3]
 *    not recovered.
 *  - Start positions 0x627fe0 / 0x628010: 21 i16 each (-1 = none), indexed by
 *    tribe, passed as (x, y) to map_in_bounds; loaded only from scenarios
 *    (USESTARTLOC).
 */

#pragma pack(push, 1)

/* World map header (0x0e bytes at 0x6d1160, saved verbatim). */
typedef struct MapHeader {
    i16 width;          /* 0x00 in doubled x coordinates (tiles per row * 2) */
    i16 height;         /* 0x02 rows */
    i16 area;           /* 0x04 number of tiles = (width/2)*height */
    u16 flat;           /* 0x06 nonzero: flat world (no x wrap) */
    u16 seed;           /* 0x08 resource/hut placement seed, 1..0x7fff */
    i16 quarter_w;      /* 0x0a (width+3)/4: coarse-grid width */
    i16 quarter_h;      /* 0x0c (height+3)/4: coarse-grid height */
} MapHeader;

/* One map tile (6 bytes; area tiles, saved verbatim). */
typedef struct Tile {
    u8 terrain;         /* 0x00 bits 0-3 type (10 = ocean), 0x40 no special, 0x80 river */
    u8 improvements;    /* 0x01 0x01 unit, 0x02 city, 0x04 irrigation, 0x08 mine, 0x10 road,
                           0x20 railroad, 0x40 fortress, 0x42 airbase, 0x80 pollution */
    u8 radius;          /* 0x02 bits 5-7 civ whose city radius covers it; bits 0-4 unknown */
    u8 landmass;        /* 0x03 continent / body of water id */
    u8 visible;         /* 0x04 bit per civilization that has seen it */
    u8 owner;           /* 0x05 bits 4-7 civ of unit/city on it (0xf none); bits 0-3 unknown */
} Tile;

#pragma pack(pop)


/* ===== rules ===== */
/*
 * RULES.TXT tables (@COSMIC, @CIVILIZE, @IMPROVE, @ENDWONDER,
 * @TERRAIN, @GOVERNMENTS, @LEADERS, @CARAVAN, @ORDERS, @DIFFICULTY,
 * @ATTITUDES).
 *
 * Evidence:
 *  - Loader rules_parse_sections calls the per-section parsers rules_load_cosmic (COSMIC),
 *    rules_load_advances (CIVILIZE), rules_load_improvements (IMPROVE+ENDWONDER), rules_load_units
 *    (UNITS), rules_load_terrain (TERRAIN), rules_load_govts_leaders (GOVERNMENTS+LEADERS) and
 *    then parses CARAVAN/ORDERS/DIFFICULTY/ATTITUDES inline.
 *  - The multiplayer table-sync switch (net_msg_build_packet / crt_memcpy callers,
 *    cases 0x17..0x24) sends each table as one block with its exact size:
 *    cosmic 0x64bcc8/0x29, advances 0x627680/0x640, improvements
 *    0x64c488/0x218, endwonder 0x64ba28/0x1c, unit types 0x64b1b8/0x4d8,
 *    terrain 0x627cc0/0x318, gov names 0x64b9a0/0x1c, gov titles
 *    0x654fe0/0x38, leaders 0x6554f8/0x3f0, commodities 0x64b168/0x40,
 *    orders 0x655490/0x68, difficulty 0x64ba10/0x18, attitudes 0x64b9c0/0x24.
 *    This fixes the bases: advances and terrain start 4 bytes before the
 *    name field, with a static 4-char abbreviation ("AFl", "Drt") that the
 *    prerequisite lookups rules_parse_advance_abbrev / rules_parse_terrain_abbrev strcmp against.
 *  - "Names" are NOT char pointers: text_next_token_str_id -> strpool_add_padded and
 *    text_next_token_pooled -> strpool_add copy the text into a string pool and return
 *    a running string id (strpool_count++); readers turn ids back into text
 *    with label_string(id).  So every name field is a fixed-width integer id
 *    and all these tables have the same layout on 32/64-bit.  (NB: the same
 *    holds for UnitType.name at +0x00 -- it is an i32 string id, not char *;
 *    rules_load_units stores the real pointer separately in unit_type_name_ptrs[62].)
 *  - Leader records are written to save files 0x30 bytes at a time
 *    (FUN at decomp.c:46406 fwrite(&leaders[i], 0x30)) and fields +3/+4/+5
 *    are saved individually, so Leader goes inside the pack(1) block.
 *  - Value meanings from the RULES.TXT comments; ranges are those of the
 *    clamp int_clamp(v, lo, hi) used by the parser.
 *  - Prerequisite/terrain-result codes: advances "nil" = -1 (none),
 *    "no" = -2 (never); terrain "no" = -1 (cannot), "yes" = -2 (improves in
 *    place); -3 = parse error.  Both compared signed -> i8.
 */

#pragma pack(push, 1)

/* One civilization (tribe) definition from @LEADERS (0x30 bytes; 21 slots,
 * saved verbatim).  All "name" fields are string ids (label_string). */
typedef struct Leader {
    i8 attack;              /* 0x00 1 aggressive, -1 rational */
    i8 expand;              /* 0x01 1 expansionist, -1 perfectionist */
    i8 civilize;            /* 0x02 1 civilized, -1 militaristic */
    u8 times_used;          /* 0x03 incremented each time the tribe is picked for a civ */
    u8 female;              /* 0x04 0 male leader, 1 female (clamped 0..1) */
    u8 city_name_index;     /* 0x05 next city name to take from the tribe's list */
    i16 color;              /* 0x06 colour set 1..7 */
    i16 city_style;         /* 0x08 0 bronze age, 1 classical, 2 far east, 3 medieval */
    i16 leader_name;        /* 0x0a current leader name = names[female] */
    i16 plural;             /* 0x0c "Romans" */
    i16 adjective;          /* 0x0e "Roman" */
    i16 names[2];           /* 0x10 leader name: [0] male, [1] female */
    i16 titles[7][2];       /* 0x14 per government: [0] male, [1] female title */
} Leader;

/* One line of the Hall of Fame (HALLFAME, 0x48 bytes): five places and the
   game being retired. */
typedef struct HallOfFameEntry {
    i16 score;              /* 0x00 -1: an empty place */
    u16 difficulty;         /* 0x02 | 0x80 */
    i16 turn;               /* 0x04 */
    i16 year;               /* 0x06 */
    i16 population;         /* 0x08 */
    i16 objective_score;    /* 0x0a -1: none */
    i16 year_increment;     /* 0x0c scenario */
    i16 start_year;         /* 0x0e scenario */
    i16 is_scenario;        /* 0x10 */
    i16 rating;             /* 0x12 civilization rating, % */
    i16 leader_female;      /* 0x14 */
    i16 title_index;        /* 0x16 */
    char leader_name[24];   /* 0x18 */
    char civ_name[24];      /* 0x30 */
} HallOfFameEntry;
static_assert(sizeof(HallOfFameEntry) == 0x48, "");

#pragma pack(pop)

/* @COSMIC (0x29 bytes; one block, synced over the network as a whole). */
typedef struct Cosmic {
    u8 road_move_mult;          /* 0x00 road movement multiplier (1..10) */
    u8 trireme_loss_chance;     /* 0x01 1 in x chance trireme lost (1..100) */
    u8 food_per_citizen;        /* 0x02 food eaten per citizen per turn (0..10) */
    u8 food_box_rows;           /* 0x03 rows in food box (4..20, forced even) */
    u8 shield_box_rows;         /* 0x04 rows in shield box (4..20) */
    u8 settler_food_low;        /* 0x05 settlers eat, govt <= Monarchy (0..10) */
    u8 settler_food_high;       /* 0x06 settlers eat, govt >= Communism (0..10) */
    u8 unhappy_size;            /* 0x07 city size for first unhappiness at Chieftain (4..12) */
    u8 riot_factor;             /* 0x08 riot factor based on number of cities (10..100) */
    u8 aqueduct_size;           /* 0x09 aqueduct needed to exceed this size (4..50) */
    u8 sewer_size;              /* 0x0a sewer system needed to exceed this size (4..50) */
    u8 tech_paradigm;           /* 0x0b higher slows research (3..10) */
    u8 transform_time;          /* 0x0c base engineer transform time, x2 (5..100) */
    u8 monarchy_free_support;   /* 0x0d monarchy pays support past this (0..8) */
    u8 communism_free_support;  /* 0x0e (0..8) */
    u8 fundamentalism_free_support; /* 0x0f (0..8) */
    u8 communism_palace_dist;   /* 0x10 communism equivalent palace distance (1..20) */
    u8 fund_science_loss;       /* 0x11 percent science lost under fundamentalism (0..100) */
    u8 change_penalty;          /* 0x12 percent shield penalty for production change (0..100) */
    u8 paradrop_range;          /* 0x13 max paradrop range (4..100) */
    u8 mass_thrust_paradigm;    /* 0x14 spaceship mass/thrust paradigm (25..200) */
    u8 fund_max_science;        /* 0x15 max science rate under fundamentalism, x10 (0..10) */
    u8 reserved[19];            /* 0x16 never accessed; part of the 0x29-byte sync block */
} Cosmic;

/* One civilization advance from @CIVILIZE (0x10 bytes; 100 slots). */
typedef struct Advance {
    char abbrev[4];         /* 0x00 static "AFl", "Alp", ... (not from RULES.TXT) */
    i32 name;               /* 0x04 string id */
    u8 field_08;            /* 0x08 never accessed */
    u8 enabled;             /* 0x09 0 when preq[0] == -2 ("no": never available) */
    i8 ai_value;            /* 0x0a basic AI value */
    i8 civilized_mod;       /* 0x0b AI value modifier for civilized leaders */
    u8 category;            /* 0x0c 0 military, 1 economic, 2 social, 3 academic, 4 applied */
    u8 epoch;               /* 0x0d 0 ancient, 1 renaissance, 2 industrial, 3 modern */
    i8 preq[2];             /* 0x0e prerequisites; -1 nil, -2 no */
} Advance;

/* One city improvement or wonder from @IMPROVE (8 bytes; 67 slots:
 * 0..38 improvements, 39..66 wonders). */
typedef struct Improvement {
    i32 name;               /* 0x00 string id */
    u8 cost;                /* 0x04 shields x10 */
    u8 upkeep;              /* 0x05 gold per turn */
    i8 preq;                /* 0x06 advance required, -1 nil, -2 no */
    u8 pad_07;
} Improvement;

/* One terrain type from @TERRAIN (0x18 bytes; 33 slots: 0..10 base
 * terrains, 11..32 two special resources per base terrain). */
typedef struct Terrain {
    char abbrev[4];         /* 0x00 static "Drt", "Pln", ... */
    i32 name;               /* 0x04 string id */
    u8 move_cost;           /* 0x08 */
    u8 defense;             /* 0x09 x50% */
    union {
        struct {
            u8 food;        /* 0x0a */
            u8 shields;     /* 0x0b */
            u8 trade;       /* 0x0c */
        };
        u8 yields[3];       /* food, shields, trade */
    };
    i8 transform_to;        /* 0x0d terrain engineers transform to, -1 no */
    /* irrigation [0] and mine [1] (orders 6 and 7) */
    union {
        struct {
            i8 irrigate_to; /* 0x0e -2 yes (bonus), -1 no, else terrain it becomes */
            i8 mine_to;     /* 0x0f */
        };
        i8 improve_to[2];
    };
    union {
        struct {
            u8 irrigate_bonus;  /* 0x10 extra production from irrigating */
            u8 mine_bonus;      /* 0x11 */
        };
        u8 improve_bonus[2];
    };
    union {
        struct {
            u8 irrigate_turns;  /* 0x12 settler turns */
            u8 mine_turns;      /* 0x13 */
        };
        u8 improve_turns[2];
    };
    union {
        struct {
            u8 irrigate_ai_govt;    /* 0x14 min govt for AI to irrigate (0 never .. 6 democracy) */
            u8 mine_ai_govt;        /* 0x15 */
        };
        u8 improve_ai_govt[2];
    };
    u8 pad_16[2];
} Terrain;
static_assert(sizeof(Terrain) == 0x18, "");

/* Male/female leader title for one government from @GOVERNMENTS (8 bytes). */
typedef struct GovTitle {
    i32 male;               /* 0x00 string id */
    i32 female;             /* 0x04 string id */
} GovTitle;

/* One unit order from @ORDERS (8 bytes; index = Unit.orders, 0 unused). */
typedef struct OrderName {
    i32 name;               /* 0x00 string id */
    char key;               /* 0x04 first char of the key column */
    u8 pad_05[3];
} OrderName;


/* ===== civs ===== */
/*
 * per-civilization records (civs[8] at 0x64c6a0), their name block
 * (civ_names[8] at 0x64bcf8) and the global game state (game at 0x655ae8).
 *
 * Evidence:
 *  - Save routine save_write_game (writes "CIVILIZE", then version 0x2c) and
 *    loader save_load_gpk move these as single blocks, in this order:
 *      fwrite(&0x655ae8, 0x14a)  game state
 *      fwrite(&0x64bcf8, 0x790)  8 x 0xf2 civ names
 *      fwrite(&0x64c6a0, 0x2ca0) 8 x 0x594 civ records
 *    So all three are saved verbatim -> pack(1), fixed-width fields only.
 *    (Save file offset = 0x0c + offset in GameState; matches the known .SAV
 *    format: difficulty at file 0x2c, human mask 0x2f, unit count 0x3a, ...)
 *  - The pre-0x28 loader branch of save_load_gpk reads the same records
 *    piecewise, which gives the array boundaries exactly:
 *      game:  0x36 header | 0x5d @0x36 | 0x5d @0x9a | 0x4c @0xfe
 *      civ:   0x58 header | 0x0c @0x58 | 0x0e @0x66 | 0x5d @0x74 |
 *             3 x 0x36 @0xd8/0x116/0x154 | (1 byte skipped) | 0x402 @0x192
 *    (old saves had 93 advances / 54 unit types; now 100 / 62).
 *  - Field meanings from the code that uses them (function addresses are
 *    the original FUN_xxxxxxxx), checked against orig/Ca_Auto.SAV (turn 81,
 *    year -2400, Romans/Caesar human civ 1, despotism, rates sum <= 10, ...).
 *  - Key functions: civ_create (initialise a civ), ai_civ_turn_assess (AI
 *    per-turn census of units/cities, per-continent tables), civ_give_advance
 *    (give advance), civ_has_advance (civ knows advance), civ_research_cost
 *    (research cost), city_split_trade (split city trade by rates), civ_process_cities_turn
 *    (AI rates / government scores), ai_choose_government (AI picks government),
 *    civ_update_power_ranks (power ranking), civ_add_ai_goal (add AI goal), spaceship_calc_stats
 *    (spaceship launch), throneroom_show_upgrade/0044db92 (throne room),
 *    setup_new_game_dialogs (new game setup dialogs), game_update_global_warming (global warming),
 *    historian_report_show (historians' report), game_turn_to_year (turn -> year).
 *
 * Uncertain: everything named field_XX; bit meanings of flags words are only
 * listed where the code makes them clear.  GameState.field_138/139 are
 * computed as power RANKS of the best/worst-ranked human civ, but several
 * readers index civs[] with them (probably an original bug or a
 * rank==civ confusion), so they are left unnamed.
 */

#pragma pack(push, 1)

/* An AI goal: go to / act at (x, y) with units of a role (6 bytes). */
typedef struct CivGoal {
    i16 x;                  /* 0x00 */
    i16 y;                  /* 0x02 */
    i8 role;                /* 0x04 unit role (UnitType.role), -1 = free slot */
    i8 priority;            /* 0x05 (negative = expires at end of turn) */
} CivGoal;

/* One civilization (0x594 bytes; 8 slots, index = civ id, 0 = barbarians;
 * saved verbatim). */
typedef struct Civ {
    u16 flags;                      /* 0x000 status bits (0x20 set on getting advance 60, 0x100 diplomacy, 0x200 ...) */
    i32 gold;                       /* 0x002 treasury */
    i16 tribe;                      /* 0x006 index into leaders[] (RULES.TXT @LEADERS) */
    i16 research_progress;          /* 0x008 beakers accumulated */
    i16 researching;                /* 0x00a advance being researched, -1 none */
    i16 capital_x;                  /* 0x00c x of the capital (palace) / start position; centres the world map */
    i16 last_city_turn;             /* 0x00e turn the last city was founded */
    u8 num_advances;                /* 0x010 advances known */
    u8 future_techs;                /* 0x011 future technologies discovered */
    union { u8 field_12; u8 research_cost_advances; };                    /* 0x012 added to num_advances in the research cost; only ever zeroed */
    u8 science_rate;                /* 0x013 in tenths; luxury = 10 - science - tax */
    u8 tax_rate;                    /* 0x014 in tenths */
    u8 government;                  /* 0x015 0 anarchy .. 6 democracy */
    union { u8 field_16; u8 diplo_random; };                    /* 0x016 rand() % 100, re-rolled every turn (civ_turn_update_diplomacy); AI diplomacy threshold */
    u8 wonders_by_group[4];         /* 0x017 wonders owned, counted per group of 7 (wonder index / 7) */
    u8 pad_1b;                      /* 0x01b */
    union { i16 field_1c; i16 subversion_count; };                   /* 0x01c +1 when a unit changes owner for gold (net handler net_process_messages); AI gold logic */
    u8 reputation;                  /* 0x01e 0 spotless .. 7; incremented when a treaty is broken */
    union { i8 field_1f; i8 diplo_request_count; };                    /* 0x01f diplomacy counter: +1/+2 per request granted, AI refuses above 4 */
    u32 treaties[8];                /* 0x020 relationship flags toward each civ (contact, peace, alliance, war, ...) */
    u8 attitude[8];                 /* 0x040 AI attitude toward each civ (0..100) */
    union { i8 field_48[8]; i8 reputation_credit[8]; };                 /* 0x048 per-civ incident counter (diplo_declare_war), signed */
    union { u8 field_50[8]; u8 aggression_counter[8]; };                 /* 0x050 per-civ counter updated by combat (unit_resolve_attack), reset at > 9 */
    u8 advances[13];                /* 0x058 bit per advance (100 bits), see civ_has_advance */
    u8 pad_65;                      /* 0x065 */
    i16 num_military_units;         /* 0x066 units with role < 5 (census ai_civ_turn_assess) */
    i16 num_cities;                 /* 0x068 */
    i16 num_ships;                  /* 0x06a units of domain 2 (sea) */
    i16 total_city_size;            /* 0x06c sum of city sizes */
    u16 military_strength;          /* 0x06e sum of attack + defence of all units */
    i16 total_attack;               /* 0x070 sum of attack of all units */
    i16 population_milestone;       /* 0x072 last population milestone announced (civ_check_pop_and_funds) */
    u8 advance_source[100];         /* 0x074 per advance: civ it was obtained from (0xff = none/initial) */
    u8 units_active[62];            /* 0x0d8 units alive, per unit type */
    u8 units_lost[62];              /* 0x116 units killed, per unit type (saturates at 255) */
    u8 units_in_production[62];     /* 0x154 cities building each unit type */
    u16 continent_military[64];     /* 0x192 military units per landmass */
    u16 continent_attack[64];       /* 0x212 total attack strength per landmass */
    u8 continent_cities[64];        /* 0x292 cities per landmass */
    u8 continent_city_size[64];     /* 0x2d2 sum of city sizes per landmass */
    u8 continent_trade_units[64];   /* 0x312 role-7 (trade) units per landmass */
    u8 continent_flags[64];         /* 0x352 AI: other civs present (1/2 cities, 4/8 units, 0x10 threat) */
    u8 continent_policy[64];        /* 0x392 AI posture per landmass (0..5) */
    union { i16 field_3d2; i16 best_shield_surplus; };                  /* 0x3d2 zeroed each turn (civ_process_cities_turn), compared in city_choose_production */
    i16 government_score[7];        /* 0x3d4 AI preference for each government, accumulated over cities */
    i16 last_contact_turn[8];       /* 0x3e2 directed contact/cooldown turn; -1 unset, may be advanced into the future */
    u8 leader_portrait;             /* 0x3f2 tribe index, +21 if the leader is female */
    u8 throne_room[10];             /* 0x3f3 throne-room piece levels; [8] is a bitmask */
    u8 pad_3fd;                     /* 0x3fd */
    i16 throne_room_upgrades;       /* 0x3fe throne-room improvements earned */
    u8 spaceship_flags;             /* 0x400 bit 0x02 launched, 0x04, 0x08, 0x10 ... */
    u8 pad_401;                     /* 0x401 */
    i16 spaceship_arrival_year;     /* 0x402 */
    i16 spaceship_launch_year;      /* 0x404 */
    i16 spaceship_population;       /* 0x406 colonists, thousands */
    i16 spaceship_parts[6];         /* 0x408 structurals, 2 component kinds, 3 module kinds (costs at 0x634f68) */
    CivGoal ai_goals[48];           /* 0x414 current AI goals, sorted by priority */
    CivGoal ai_standing_goals[16];  /* 0x534 goals re-posted into ai_goals every turn (civ_refresh_ai_goals) */
} Civ;

/* Names of one civilization (0xf2 bytes; 8 slots, saved verbatim).  Empty
 * strings mean "use the leaders[] defaults". */
typedef struct CivNames {
    i16 city_style;                 /* 0x00 overrides leaders[tribe].city_style for human civs */
    char leader_name[24];           /* 0x02 */
    char tribe_name[24];            /* 0x1a plural noun ("Romans") */
    char adjective[24];             /* 0x32 */
    char titles[7][24];             /* 0x4a leader title per government */
} CivNames;

/* Global game state (0x14a bytes, saved verbatim right after the header). */
typedef struct GameState {
    u16 rules;                      /* 0x000 custom features: 0x10 set unless Simplified Combat, 0x80 Bloodlust,
                                       0x100 Don't Restart Eliminated, 0x8000 Flat World */
    u32 game_options;               /* 0x002 @GAMEOPTIONS / @GRAPHICOPTIONS bits (saved to CIV.INI as 0x64bc1e) */
    union { u16 field_06; u16 transient_flags; };                   /* 0x006 transient flag bits (1, 2, 4) */
    u16 game_flags;                 /* 0x008 0x01 AI ganging up, 0x04 large map, 0x08 small map, 0x40 saved as
                                       scenario, 0x80 scenario */
    u16 message_options;            /* 0x00a @MESSAGEOPTIONS, bit set = message off */
    union { u16 field_0c; u16 shown_event_flags; };                   /* 0x00c one-shot event bits (each set once when its message is shown) */
    union { u16 field_0e; u16 unused_0e; };                   /* 0x00e only ever zeroed */
    i16 turn;                       /* 0x010 */
    i16 year;                       /* 0x012 negative = BC */
    i16 first_launch_turn;          /* 0x014 turn the first spaceship was launched, -1 none (years then +1/turn) */
    i16 active_unit;                /* 0x016 selected unit, -1 none */
    i16 moving_unit;                /* 0x018 unit being moved (kept across combat) */
    u8 multiplayer;                 /* 0x01a 0 single player, 1 hot seat, > 2 network */
    i8 player_civ;                  /* 0x01b the human player's civ */
    union { i8 field_1c; i8 saved_current_civ; };                    /* 0x01c copy of the current player civ (0x6d1da0) at save; restored on load */
    i8 turn_civ;                    /* 0x01d civ whose turn is being played */
    i8 view_civ;                    /* 0x01e cheat "view as" civ, -1 none */
    u8 reveal_map;                  /* 0x01f cheat: whole map visible */
    u8 difficulty;                  /* 0x020 0 chieftain .. 5 deity */
    u8 barbarian_activity;          /* 0x021 */
    u8 civs_alive;                  /* 0x022 bit per civilization in play */
    u8 human_players;               /* 0x023 bit per civilization */
    u8 civs_at_start;               /* 0x024 civs_alive when the game began */
    u8 num_enemies;                 /* 0x025 AI opponents (recomputed on load from civs_alive) */
    i8 warming_counter;             /* 0x026 global warming pressure 0..99, event above 16 */
    i8 global_warmings;             /* 0x027 global warming events so far */
    union { i16 field_28; i16 warming_pollution; };                   /* 0x028 polluted-tile count weighted 1/2 in the warming check; reset after it */
    i16 polluted_tiles;             /* 0x02a */
    union { i16 field_2c; i16 peace_turns; };                   /* 0x02c +1 per turn after turn 200, reset in combat; AI chance factor */
    i16 num_units;                  /* 0x02e unit slots in use */
    i16 num_cities;                 /* 0x030 city slots in use */
    i16 num_advances;               /* 0x032 advances defined in RULES.TXT */
    union { i16 field_34; i16 unused_34; };                   /* 0x034 only ever zeroed */
    u8 first_discoverer[100];       /* 0x036 per advance: civ that discovered it first (8 = several, scenarios) */
    u8 advance_known_by[100];       /* 0x09a per advance: bit per civ that knows it */
    i16 wonder_city[28];            /* 0x0fe city holding each wonder, -1 not built */
    i16 next_history_turn;          /* 0x136 turn of the next historians' report */
    union { u8 field_138; u8 best_human_rank; };                   /* 0x138 power rank of the best-ranked human civ (see header note) */
    union { u8 field_139; u8 worst_human_rank; };                   /* 0x139 power rank of the worst-ranked human civ (see header note) */
    u8 power_rank[8];               /* 0x13a rank of each civ (7 = strongest) */
    u8 civ_by_rank[8];              /* 0x142 civ at each rank; [7] = strongest civ */
} GameState;

/* Sound mixer (sound.cpp).  32-bit layout: the original keeps pointers and
 * Win32 handles (HMMIO, HGLOBAL) in 4-byte slots. */
/* A waveOut header (WAVEHDR, 0x20 bytes) at the start of a buffer's sample
 * block; the samples follow it. */
typedef struct WaveHeader {
    i32 data;                       /* 0x00 lpData: the samples */
    u32 length;                     /* 0x04 dwBufferLength */
    u32 bytes_recorded;             /* 0x08 */
    struct WaveBuffer *owner;       /* 0x0c dwUser: the buffer it belongs to */
    u32 flags;                      /* 0x10 dwFlags (WHDR_DONE = 1) */
    u32 loops;                      /* 0x14 */
    i32 next;                       /* 0x18 lpNext */
    i32 reserved;                   /* 0x1c */
} WaveHeader;

/* One output buffer (0x20 bytes, GlobalAlloc'd), a ring linked by next
 * (snd_wave_buffer_ring; unused ones in snd_wave_spare_buffers). */
typedef struct WaveBuffer {
    WaveHeader *hdr;                /* 0x00 header + samples (bytes + 0x20) */
    u32 data_handle;                /* 0x04 HGLOBAL of hdr */
    u32 handle;                     /* 0x08 HGLOBAL of this buffer */
    u32 flags;                      /* 0x0c 1 spare, 2 stream ended, 8 filled, 0x10 end played,
                                       0x20 queued, 0x40 mixed; bits 7..14 index in the ring */
    i32 data;                       /* 0x10 the samples (= hdr->data) */
    i32 data_end;                   /* 0x14 */
    i32 length;                     /* 0x18 bytes */
    struct WaveBuffer *next;        /* 0x1c */
} WaveBuffer;

/* Called with the sound's id when a sound ends (snd_play_effect). */
typedef void (*SoundDoneFn)(i32 sound_id);
/* Called when the CD audio / MIDI music notifies that it stopped. */
typedef void (*MusicNotifyFn)(void);

/* One playing sound (0xbc bytes, GlobalAlloc'd): a wave file read through
 * mmio, a streamed wave file, or an AVI audio stream (kind 4); a list linked
 * by next (snd_wave_sources). */
typedef struct WaveSource {
    u32 mmio;                       /* 0x00 HMMIO the samples are read from */
    u32 data_ckid;                  /* 0x04 the 'data' chunk (MMCKINFO) */
    i32 data_size;                  /* 0x08 bytes of samples */
    u32 data_fcc_type;              /* 0x0c */
    i32 data_offset;                /* 0x10 file offset of the samples */
    u32 data_ck_flags;              /* 0x14 */
    u32 io_flags;                   /* 0x18 MMIOINFO of mmio */
    u32 io_fcc_proc;                /* 0x1c */
    u32 io_proc;                    /* 0x20 */
    u32 io_error;                   /* 0x24 */
    u32 io_task;                    /* 0x28 */
    u32 buf_size;                   /* 0x2c cchBuffer */
    i32 buf_start;                  /* 0x30 pchBuffer */
    i32 buf_next;                   /* 0x34 pchNext: next sample to play */
    i32 buf_end_read;               /* 0x38 */
    i32 buf_end;                    /* 0x3c pchEndWrite */
    i32 io_buf_offset;              /* 0x40 */
    i32 io_disk_offset;             /* 0x44 */
    u32 io_info[3];                 /* 0x48 */
    u32 io_reserved[2];             /* 0x54 */
    i32 io_hmmio;                   /* 0x5c */
    u32 file_mmio;                  /* 0x60 HMMIO of the file (streamed) */
    i32 play_pos;                   /* 0x64 bytes played */
    i32 buffer_fill;                /* 0x68 bytes in the buffer being filled / read */
    u32 mem_handle;                 /* 0x6c HGLOBAL of this record */
    u32 flags;                      /* 0x70 1 ended, 2 stopped, 4 streamed, 8 needs data,
                                       0x10 started */
    union { u32 format_tag_channels; struct {
        u16 format_tag;             /* 0x74 WAVEFORMAT */
        u16 channels;               /* 0x76 */
    }; };
    u32 samples_per_sec;            /* 0x78 */
    u32 avg_bytes_per_sec;          /* 0x7c */
    union { u32 block_align_bits; struct {
        u16 block_align;            /* 0x80 bytes per sample */
        u16 bits_per_sample;        /* 0x82 */
    }; };
    i32 avi_stream;                 /* 0x84 PAVISTREAM */
    i16 kind;                       /* 0x88 1 wave, 4 AVI stream */
    i16 sound_id;                   /* 0x8a */
    iptr status_ptr;                /* 0x8c u16 * of the AVI player's status (4 = playing) */
    i32 stream_pos;                 /* 0x90 AVI: next byte to read */
    i32 stream_start;               /* 0x94 */
    WaveBuffer *fill_buffer;        /* 0x98 AVI: the buffer being filled */
    i32 buffer_bytes;               /* 0x9c AVI: bytes per buffer */
    u8 pad_a0[8];
    i32 size_mask;                  /* 0xa8 */
    u8 pad_ac[8];
    SoundDoneFn done_callback;      /* 0xb4 called with sound_id when it ends */
    struct WaveSource *next;        /* 0xb8 */
} WaveSource;
/* A BITMAPINFOHEADER (0x28 bytes): the AVI player's frame formats. */
typedef struct BitmapInfoHeader {
    u32 size;                       /* 0x00 */
    i32 width;                      /* 0x04 */
    i32 height;                     /* 0x08 negative: top-down */
    u16 planes;                     /* 0x0c */
    u16 bit_count;                  /* 0x0e */
    u32 compression;                /* 0x10 */
    i32 size_image;                 /* 0x14 */
    i32 x_pels_per_meter;           /* 0x18 */
    i32 y_pels_per_meter;           /* 0x1c */
    u32 clr_used;                   /* 0x20 */
    u32 clr_important;              /* 0x24 */
} BitmapInfoHeader;
#ifdef __cplusplus
static_assert(sizeof(WaveHeader) == 0x20, "");
static_assert(sizeof(WaveBuffer) == 0x20, "");
static_assert(sizeof(WaveSource) == 0xbc, "");
static_assert(sizeof(BitmapInfoHeader) == 0x28, "");
#endif

/* Network (network.cpp). */
/* A game announced on the network (0x124 bytes): the payload of the server's
 * info message (net_fill_game_info), copied into a node of the found games
 * list (net_session_block.net_found_games, sorted by game name). */
typedef struct NetGameInfo {
    char host_address[32];          /* 0x00 the server's address: the list's key */
    struct NetGameInfo *next;       /* 0x20 found games list */
    struct NetGameInfo *prev;       /* 0x24 */
    i32 last_seen;                  /* 0x28 timer_ticks() when last announced */
    i32 list_id;                    /* 0x2c id in the game list window */
    char version[64];               /* 0x30 */
    char game_name[32];             /* 0x70 */
    char host_name[32];             /* 0x90 the hosting player */
    i16 open_game;                  /* 0xb0 */
    u8 difficulty;                  /* 0xb2 */
    u8 barbarian_activity;          /* 0xb3 */
    u8 civs_alive;                  /* 0xb4 bit per civ */
    u8 player_mask;                 /* 0xb5 bit per player */
    u8 pad_b6[2];
    i32 turn_time_limit_ms;         /* 0xb8 0: none */
    char scenario_name[80];         /* 0xbc */
    i16 double_moves;               /* 0x10c */
    i16 double_production;          /* 0x10e */
    u16 game_flags;                 /* 0x110 */
    i16 first_launch_turn;          /* 0x112 */
    i16 year_increment;             /* 0x114 */
    i16 start_year;                 /* 0x116 */
    i16 turn;                       /* 0x118 */
    i16 map_width;                  /* 0x11a */
    i16 map_height;                 /* 0x11c */
    i16 simultaneous;               /* 0x11e */
    i16 session_phase;              /* 0x120 announced net_session_phase; below 3 hides the player count */
    u8 pad_122[2];
} NetGameInfo;
#ifdef __cplusplus
static_assert(sizeof(NetGameInfo) == 0x124, "");
#endif
/* A player of the network game (0x54 bytes; net_session_block.net_slots,
 * slot 0 is the local player). */
typedef struct NetPlayerSlot {
    i32 id;                         /* 0x00 the player's connection id */
    char address[32];               /* 0x04 */
    char name[32];                  /* 0x24 */
    i32 field_44;                   /* 0x44 */
    i32 lobby_id;                   /* 0x48 entry in the lobby list */
    i8 is_host;                     /* 0x4c */
    i8 active;                      /* 0x4d */
    u8 pad_4e[2];
    i32 civ;                        /* 0x50 */
} NetPlayerSlot;
/* A queued draw request of the network stacked-draw queue (0x40 bytes). */
typedef struct NetStackedDraw {
    i32 kind;                       /* 0x00 */
    i32 args[15];                   /* 0x04 */
} NetStackedDraw;
/* A unit lost, in a civ's casualty log (0x22 bytes;
 * net_session_block.casualty_log, 300 per civ). */
typedef struct CasualtyEntry {
    i16 unit_type;                  /* 0x00 */
    i16 x;                          /* 0x02 */
    i16 y;                          /* 0x04 */
    i16 year;                       /* 0x06 */
    i16 killer_color;               /* 0x08 */
    char killer_name[24];           /* 0x0a */
} CasualtyEntry;
/* A civ's lock on map tiles during a network move (0x18 bytes;
 * net_map_lock_slots_block.net_map_locks). */
typedef struct NetMapLock {
    i32 state;                      /* 0x00 */
    i32 replies;                    /* 0x04 */
    i32 src_x;                      /* 0x08 */
    i32 src_y;                      /* 0x0c */
    i32 dst_x;                      /* 0x10 */
    i32 dst_y;                      /* 0x14 */
} NetMapLock;
#ifdef __cplusplus
static_assert(sizeof(NetPlayerSlot) == 0x54, "");
static_assert(sizeof(NetMapLock) == 0x18, "");
static_assert(sizeof(CasualtyEntry) == 0x22, "");
static_assert(sizeof(NetStackedDraw) == 0x40, "");
#endif


/* Windows (window.cpp). */
/* The native window record (0x4c bytes, a locked mem_handle block) that
 * wnd_obj_create makes for a window object: WndBase.native_wnd,
 * wnd_get_window(). */
typedef struct NativeWindow {
    i32 mem_handle;                 /* 0x00 handle of this record */
    HWND hwnd;                      /* 0x04 */
    HDC hdc;                        /* 0x08 GetDC(hwnd), kept for the window's life */
    HWND active_child;              /* 0x0c the attached child drawn active (WM_NCACTIVATE) */
    HWND topmost_child;             /* 0x10 the attached child kept above its siblings */
    HBRUSH bg_brush;                /* 0x14 background colour */
    HPALETTE palette;               /* 0x18 selected into hdc */
    HCURSOR cursor;                 /* 0x1c */
    HICON icon;                     /* 0x20 drawn when minimized */
    struct DibSurface *bg_tile;     /* 0x24 tiled background */
    i32 field_28;                   /* 0x28 */
    struct DibSurface *bg_surface;  /* 0x2c centred background */
    i32 flag_400;                   /* 0x30 created with flags & 0x400 */
    i32 flag_1000;                  /* 0x34 created with flags & 0x1000 */
    i32 caption_height;             /* 0x38 */
    i32 resize_border;              /* 0x3c */
    i32 field_40;                   /* 0x40 set to 8 */
    i32 destroyed;                  /* 0x44 WM_DESTROY seen: hwnd is gone */
    union { u32 flags; u8 flags_bytes[4]; };  /* 0x48 wnd_obj_create flags
                                       (0x200 attached child, 0x2000 sizing ...) */
} NativeWindow;
#ifdef __cplusplus
static_assert(sizeof(NativeWindow) == 0x4c, "");
#endif


/* Dialogs (window.cpp): the lists a dialog builds, each node allocated from
 * the dialog's string pool (memblock_alloc). */
/* A choice of a dialog's option list (0x18 bytes; Dialog.option_list,
 * dialog_add_option); a list sorted by text when style_flags & 0x800000. */
typedef struct DialogOption {
    union { u32 flags; u8 flags_byte; };  /* 0x00 1 disabled (also an empty text), 4 checked */
    i32 id;                         /* 0x04 */
    char *text;                     /* 0x08 */
    i32 group;                      /* 0x0c list pane (dialog_add_option data); compared with cur_pane */
    struct DialogOption *next;      /* 0x10 */
    struct DialogOption *prev;      /* 0x14 */
} DialogOption;
/* A picture of a dialog's sprite list (0x14 bytes; Dialog.sprite_list,
 * dialog_add_sprite). */
typedef struct DialogSprite {
    i32 id;                         /* 0x00 */
    i32 field_04;                   /* 0x04 */
    char *label;                    /* 0x08 0: none */
    struct Sprite *sprite;          /* 0x0c */
    struct DialogSprite *next;      /* 0x10 */
} DialogSprite;
/* A text field of a dialog (0x1c bytes; Dialog.edit_field_list,
 * dialog_add_edit_field). */
typedef struct DialogEditField {
    i32 field_00;                   /* 0x00 */
    i32 width;                      /* 0x04 pixels */
    i32 max_len;                    /* 0x08 */
    i32 index;                      /* 0x0c its edit control */
    char *label;                    /* 0x10 */
    char *buffer;                   /* 0x14 the text (0x100 bytes) */
    struct DialogEditField *next;   /* 0x18 */
} DialogEditField;
/* A line of a dialog's text (0x20 bytes; Dialog.line_list, dialog_add_line). */
typedef struct DialogLine {
    i32 number;                     /* 0x00 1, 2, ... */
    union { u32 flags; u8 flags_byte; };  /* 0x04 1 the text began with ^^, 2 with ^ (centred) */
    char *text;                     /* 0x08 */
    i32 capacity;                   /* 0x0c bytes of text */
    i32 x;                          /* 0x10 */
    i32 y;                          /* 0x14 */
    i32 color;                      /* 0x18 */
    struct DialogLine *next;        /* 0x1c */
} DialogLine;
/* A menu of a GameMenu, or an item of a menu (0x1c bytes; GameMenu.menus,
 * menu_add_menu / menu_add_item). */
typedef struct MenuNode {
    char *text;                     /* 0x00 */
    i32 id;                         /* 0x04 */
    union { u32 flags; u8 flags_byte; };  /* 0x08 1 disabled, 2 hidden, 4 checked */
    i32 field_0c;                   /* 0x0c */
    struct MenuNode *next;          /* 0x10 */
    struct MenuNode *prev;          /* 0x14 */
    struct MenuNode *link;          /* 0x18 a menu: its first item; an item: its menu */
} MenuNode;
#ifdef __cplusplus
static_assert(sizeof(DialogOption) == 0x18, "");
static_assert(sizeof(DialogSprite) == 0x14, "");
static_assert(sizeof(DialogEditField) == 0x1c, "");
static_assert(sizeof(DialogLine) == 0x20, "");
static_assert(sizeof(MenuNode) == 0x1c, "");
#endif


/* The buffers of the diff engine's run-length coder (0x14 bytes;
 * diffeng_rle_encode / diffeng_rle_decode, callers' consecutive locals). */
typedef struct RleStream {
    u8 *src;                        /* 0x00 input */
    u8 *src_end;                    /* 0x04 */
    u8 *dst;                        /* 0x08 output */
    u8 *dst_end;                    /* 0x0c its capacity */
    i32 out_len;                    /* 0x10 bytes written */
} RleStream;
#ifdef __cplusplus
static_assert(sizeof(RleStream) == 0x14, "");
#endif


/* Scenario events (game.cpp). */
/* One @IF/@THEN event of EVENTS.TXT (0x1c4 bytes; ScenarioEvents.event_list,
 * events_alloc, events_parse_file).  The first 0x1bc bytes are saved with the
 * game; the strings live in the events' string pool. */
typedef struct ScenarioEvent {
    union { i32 trigger; u8 trigger_bytes[4]; };  /* 0x000 1 UnitKilled, 2 CityTaken, 4 Turn, 8 TurnInterval,
                                       0x10 Negotiation, 0x20 ScenarioLoaded, 0x40 RandomTurn,
                                       0x80 NoSchism, 0x100 ReceivedTechnology */
    union { u32 actions; u8 actions_bytes[4]; };  /* 0x004 1 Text, 2 MoveUnit, 4 CreateUnit,
                                       8 ChangeMoney, 0x10 PlayWaveFile, 0x20 MakeAggression,
                                       0x40 JustOnce, 0x80 PlayCDTrack, 0x100 DontPlayWonders,
                                       0x200 ChangeTerrain, 0x400 DestroyACivilization,
                                       0x800 GiveTechnology */
    char *unit_name;                /* 0x008 UnitKilled unit */
    i32 unit_type;                  /* 0x00c */
    char *city_name;                /* 0x010 CityTaken city */
    char *who_name;                 /* 0x014 attacker / Negotiation talker */
    i32 who_civ;                    /* 0x018 */
    i32 talker_type;                /* 0x01c 1 human, 2 computer, 4 either */
    char *whom_name;                /* 0x020 defender / listener / NoSchism, ReceivedTechnology civ */
    i32 whom_civ;                   /* 0x024 */
    i32 listener_type;              /* 0x028 1 human, 2 computer, 4 either */
    i32 turn;                       /* 0x02c Turn (-1 every turn) / TurnInterval interval */
    i32 denominator;                /* 0x030 RandomTurn */
    i32 technology;                 /* 0x034 ReceivedTechnology */
    char *text[20];                 /* 0x038 Text lines */
    char *move_owner_name;          /* 0x088 MoveUnit */
    i32 move_owner_civ;             /* 0x08c */
    char *move_unit_name;           /* 0x090 */
    i32 move_unit_type;             /* 0x094 */
    i32 move_count;                 /* 0x098 NumberToMove, -2 all */
    i32 move_rect[8];               /* 0x09c MapRect: 4 x, y pairs */
    i32 move_to_x;                  /* 0x0bc */
    i32 move_to_y;                  /* 0x0c0 */
    char *aggression_whom_name;     /* 0x0c4 MakeAggression */
    i32 aggression_whom_civ;        /* 0x0c8 */
    char *aggression_who_name;      /* 0x0cc */
    i32 aggression_who_civ;         /* 0x0d0 */
    char *create_owner_name;        /* 0x0d4 CreateUnit */
    i32 create_owner_civ;           /* 0x0d8 */
    char *create_unit_name;         /* 0x0dc */
    i32 create_unit_type;           /* 0x0e0 */
    i32 create_locations[20];       /* 0x0e4 10 x, y pairs */
    i32 create_location_count;      /* 0x134 */
    i32 create_veteran;             /* 0x138 */
    char *create_homecity;          /* 0x13c */
    char *field_140;                /* 0x140 a civ name (saved, copied) */
    i32 field_144;                  /* 0x144 */
    char *field_148;                /* 0x148 a unit type name (renamed with the type) */
    u8 pad_14c[0x28];               /* 0x14c */
    char *money_receiver_name;      /* 0x174 ChangeMoney */
    i32 money_receiver_civ;         /* 0x178 */
    i32 money_amount;               /* 0x17c */
    i32 negotiation_flag;           /* 0x180 0x1000 for a Negotiation trigger */
    char *wave_file;                /* 0x184 PlayWaveFile */
    i32 cd_track;                   /* 0x188 PlayCDTrack */
    i32 terrain_type;               /* 0x18c ChangeTerrain */
    i32 terrain_rect[8];            /* 0x190 MapRect: 4 x, y pairs */
    i32 destroy_civ;                /* 0x1b0 DestroyACivilization */
    i32 give_tech;                  /* 0x1b4 GiveTechnology */
    i32 give_tech_civ;              /* 0x1b8 */
    struct ScenarioEvent *next;     /* 0x1bc */
    struct ScenarioEvent *prev;     /* 0x1c0 */
} ScenarioEvent;
#ifdef __cplusplus
static_assert(sizeof(ScenarioEvent) == 0x1c4, "");
#endif


/* Graphics (graphics.cpp). */
/* A GDI DIB section and its memory DC (0x28 bytes, a locked mem_handle block;
 * dib_create*; DibSurface.dib). */
typedef struct GdiDib {
    i32 mem_handle;                 /* 0x00 handle of this record */
    HDC hdc;                        /* 0x04 memory DC the bitmap is selected into */
    HGDIOBJ palette;                /* 0x08 deleted with the DIB when set */
    HBITMAP bitmap;                 /* 0x0c the DIB section */
    HGDIOBJ old_bitmap;             /* 0x10 hdc's bitmap before */
    i32 top_down;                   /* 0x14 1: row 0 at the top */
    i32 width;                      /* 0x18 */
    i32 height;                     /* 0x1c */
    i32 stride;                     /* 0x20 bytes per row */
    i32 bits;                       /* 0x24 the pixels */
} GdiDib;
#ifdef __cplusplus
static_assert(sizeof(GdiDib) == 0x28, "");
#endif


/* Controls (controls.cpp). */
/* The record a control's HWND keeps in its window long 0 (0x30 bytes, a locked
 * mem_handle block; ctrl_init_block, wnd_from_hwnd). */
typedef struct ControlBlock {
    i32 mem_handle;                 /* 0x00 handle of this record */
    iptr owner;                     /* 0x04 the Control */
    i32 shared_bitmaps;             /* 0x08 nonzero: the bitmaps are not deleted with it */
    HBITMAP bitmaps[3];             /* 0x0c a button's faces */
    i32 pressed;                    /* 0x18 drawn pressed / checked */
    i32 tracking;                   /* 0x1c the mouse is captured */
    i32 field_20;                   /* 0x20 */
    iptr data;                      /* 0x24 kind 2: an owned buffer, kind 3: an i32 * cleared on destroy */
    i32 field_28;                   /* 0x28 */
    i32 kind;                       /* 0x2c */
} ControlBlock;
/* An item of a radio group (0xa4 bytes; RadioGroup.items). */
typedef struct RadioItem {
    HWND hwnd;                      /* 0x00 */
    RECT rect;                      /* 0x04 */
    i32 index;                      /* 0x14 */
    i32 enabled;                    /* 0x18 */
    char label[128];                /* 0x1c */
    char hotkey;                    /* 0x9c */
    u8 pad_9d[3];
    i32 hotkey_pos;                 /* 0xa0 index of the hotkey in label, -1: none */
} RadioItem;
#ifdef __cplusplus
static_assert(sizeof(ControlBlock) == 0x30, "");
static_assert(sizeof(RadioItem) == 0xa4, "");
#endif

/* Writes a section of RULES.TXT (after its @NAME line) to out_file; in_file
   is the old RULES.TXT. */
typedef i32 (*RulesSectionWriterFn)(FILE *out_file, FILE *in_file);

/* A section of RULES.TXT the rules editors rewrite (rules_edit_add_section). */
typedef struct RulesEditSection {
    iptr name;                      /* 0x0 "@NAME" without the @, 0 ends the list */
    RulesSectionWriterFn writer;    /* 0x4 */
} RulesEditSection;
/* A row of a rules editor's control table: what is on that row. */
typedef struct EditorCtrlRow {
    i32 kind;                       /* 0x0 9 edit box, 0xc combo box */
    i32 slot;                       /* 0x4 index of the edit box / combo box */
} EditorCtrlRow;
/* A 256-colour LOGPALETTE (the start of a GamePalette; palette_get_logpalette),
 * entries read as signed bytes. */
typedef struct LogPalEntry {
    i8 red;
    i8 green;
    i8 blue;
    i8 flags;                       /* PC_ flags: 1 reserved, 4 nocollapse */
} LogPalEntry;
typedef struct GameLogPalette {
    i16 version;                    /* 0x0 0x300 */
    i16 count;                      /* 0x2 0x100 */
    LogPalEntry entries[256];       /* 0x4 */
} GameLogPalette;
#ifdef __cplusplus
static_assert(sizeof(GameLogPalette) == 0x404, "");
#endif
/* Where a throne room piece is drawn (throneroom_piece_rects[piece][level]). */
typedef struct ThroneRect {
    i32 x;
    i32 y;
    i32 w;
    i32 h;
} ThroneRect;
#ifdef __cplusplus
static_assert(sizeof(ThroneRect) == 0x10, "");
#endif
/* A cached scaling table (default_draw_font_obj_block.gfx_scale_caches[16]):
 * table[i] = i * num / den, the least recently used is replaced. */
typedef struct GfxScaleCache {
    i32 num;                        /* 0x0 */
    i32 den;                        /* 0x4 */
    i32 last_used;                  /* 0x8 tick */
    i32 table[0x400];               /* 0xc */
} GfxScaleCache;
#ifdef __cplusplus
static_assert(sizeof(GfxScaleCache) == 0x100c, "");
#endif
/* A civ's colours (civ_primary_colors_block.civ_colors[8], by colour index). */
typedef struct CivColors {
    i32 primary;                    /* 0x0 */
    i32 mid;                        /* 0x4 */
    i32 secondary;                  /* 0x8 */
    i32 flags;                      /* 0xc cleared for a new game */
} CivColors;
#ifdef __cplusplus
static_assert(sizeof(CivColors) == 0x10, "");
#endif
/* A landmass or body of water (landmass_table_block.landmass_recs[64]). */
typedef struct Landmass {
    i16 tiles;                      /* 0x0 */
    i16 fertile_tiles;              /* 0x2 */
    i16 water_body_size;            /* 0x4 */
    i8 field_6;
    i8 border_bits[8];              /* 0x7 a bit per body it borders */
    i8 field_f;
} Landmass;
#ifdef __cplusplus
static_assert(sizeof(Landmass) == 0x10, "");
static_assert(__builtin_offsetof(Landmass, border_bits) == 7, "");
#endif
/* A RECT stored as shorts (the saved window layout; rect_pack_to_short). */
typedef struct ShortRect {
    i16 left;
    i16 top;
    i16 right;
    i16 bottom;
} ShortRect;
#ifdef __cplusplus
static_assert(sizeof(ShortRect) == 8, "");
#endif
/* Where a rules editor puts a control (x, y in the client area). */
typedef struct EditorPos {
    i32 x;
    i32 y;
} EditorPos;
/* A spaceship part kind (spaceship_part_table_block.spaceship_parts[6]). */
typedef struct SpaceshipPartKind {
    i32 label_id;                   /* 0x0 index in label_string_ids */
    i32 max_count;                  /* 0x4 */
    i32 mass;                       /* 0x8 */
} SpaceshipPartKind;
/* A period of the calendar (year_period_table, 4 x 6):
 * turn_count turns of year_step years from start_year. */
typedef struct YearPeriod {
    i32 start_year;                 /* 0x0 */
    i32 turn_count;                 /* 0x4 */
    i32 year_step;                  /* 0x8 */
} YearPeriod;
#ifdef __cplusplus
static_assert(sizeof(EditorPos) == 8 && sizeof(SpaceshipPartKind) == 0xc && sizeof(YearPeriod) == 0xc, "");
#endif
/* A rules editor's values for one item (unit type, improvement, advance...):
 * value i is the control with id 0xca + i (editor_text_dlg_wnd_block). */
typedef struct EditorItem {
    i32 values[22];
} EditorItem;
#ifdef __cplusplus
static_assert(sizeof(EditorItem) == 0x58, "");
static_assert(sizeof(RulesEditSection) == 8, "");
static_assert(sizeof(EditorCtrlRow) == 8, "");
#endif

#pragma pack(pop)

#endif
