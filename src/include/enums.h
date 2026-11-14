/*
 * The game's enumerations and flag constants (step 6: the magic numbers
 * named).  Included by all game code (auto_common.h).
 *
 * Rules are moddable: a scenario's RULES.TXT replaces the tables, so a unit
 * type, improvement, advance or terrain id means "slot N".  Each slot is
 * named after the original rules' default, as Civ2 modders do (unit type
 * 46 is the Diplomat's slot).
 *
 * The enums are unscoped with the underlying type of the field that holds
 * the value (the 32-bit flag words: i32), so an enumerator promotes,
 * converts and compares exactly as the int literal it replaces did, also
 * where the code reads one byte of a flag word: naming a number never
 * changes the machine code (tools/name_constants.py, checked with
 * tools/cxx_compare.py).  Struct fields keep their integer types.
 *
 * Evidence for the flag bits: the functions that set, clear and test them
 * (named per bit below); "?" marks a meaning that is uncertain.
 */
#ifndef CIV2_ENUMS_H
#define CIV2_ENUMS_H

/* ===== rules: the original RULES.TXT's slots ===== */

/* Unit types (@UNITS; Unit.type, City.building >= 0, unit_types[]). */
enum UnitTypeId : u8 {
    UNIT_SETTLERS = 0,         UNIT_ENGINEERS = 1,        UNIT_WARRIORS = 2,
    UNIT_PHALANX = 3,          UNIT_ARCHERS = 4,          UNIT_LEGION = 5,
    UNIT_PIKEMEN = 6,          UNIT_MUSKETEERS = 7,       UNIT_FANATICS = 8,
    UNIT_PARTISANS = 9,        UNIT_ALPINE_TROOPS = 10,   UNIT_RIFLEMEN = 11,
    UNIT_MARINES = 12,         UNIT_PARATROOPERS = 13,    UNIT_MECH_INF = 14,
    UNIT_HORSEMEN = 15,        UNIT_CHARIOT = 16,         UNIT_ELEPHANT = 17,
    UNIT_CRUSADERS = 18,       UNIT_KNIGHTS = 19,         UNIT_DRAGOONS = 20,
    UNIT_CAVALRY = 21,         UNIT_ARMOR = 22,           UNIT_CATAPULT = 23,
    UNIT_CANNON = 24,          UNIT_ARTILLERY = 25,       UNIT_HOWITZER = 26,
    UNIT_FIGHTER = 27,         UNIT_BOMBER = 28,          UNIT_HELICOPTER = 29,
    UNIT_STEALTH_FIGHTER = 30, UNIT_STEALTH_BOMBER = 31,  UNIT_TRIREME = 32,
    UNIT_CARAVEL = 33,         UNIT_GALLEON = 34,         UNIT_FRIGATE = 35,
    UNIT_IRONCLAD = 36,        UNIT_DESTROYER = 37,       UNIT_CRUISER = 38,
    UNIT_AEGIS_CRUISER = 39,   UNIT_BATTLESHIP = 40,      UNIT_SUBMARINE = 41,
    UNIT_CARRIER = 42,         UNIT_TRANSPORT = 43,       UNIT_CRUISE_MISSILE = 44,
    UNIT_NUCLEAR_MISSILE = 45, UNIT_DIPLOMAT = 46,        UNIT_SPY = 47,
    UNIT_CARAVAN = 48,         UNIT_FREIGHT = 49,         UNIT_EXPLORER = 50,
    UNIT_EXTRA_LAND = 51,      UNIT_EXTRA_SHIP = 52,      UNIT_EXTRA_AIR = 53,
    UNIT_TEST_1 = 54,          UNIT_TEST_2 = 55,          UNIT_TEST_3 = 56,
    UNIT_TEST_4 = 57,          UNIT_TEST_5 = 58,          UNIT_TEST_6 = 59,
    UNIT_TEST_7 = 60,          UNIT_TEST_8 = 61,
    UNIT_TYPE_COUNT = 62,           /* the slots (unit_types[]) */
};

/* City improvements (@IMPROVE; city_has_improvement, improvements[],
 * -City.building): 0..38 improvements, 39..66 the wonders. */
enum ImprovementId : u8 {
    IMP_NOTHING = 0,            IMP_PALACE = 1,             IMP_BARRACKS = 2,
    IMP_GRANARY = 3,            IMP_TEMPLE = 4,             IMP_MARKETPLACE = 5,
    IMP_LIBRARY = 6,            IMP_COURTHOUSE = 7,         IMP_CITY_WALLS = 8,
    IMP_AQUEDUCT = 9,           IMP_BANK = 10,              IMP_CATHEDRAL = 11,
    IMP_UNIVERSITY = 12,        IMP_MASS_TRANSIT = 13,      IMP_COLOSSEUM = 14,
    IMP_FACTORY = 15,           IMP_MFG_PLANT = 16,         IMP_SDI_DEFENSE = 17,
    IMP_RECYCLING_CENTER = 18,  IMP_POWER_PLANT = 19,       IMP_HYDRO_PLANT = 20,
    IMP_NUCLEAR_PLANT = 21,     IMP_STOCK_EXCHANGE = 22,    IMP_SEWER_SYSTEM = 23,
    IMP_SUPERMARKET = 24,       IMP_SUPERHIGHWAYS = 25,     IMP_RESEARCH_LAB = 26,
    IMP_SAM_BATTERY = 27,       IMP_COASTAL_FORTRESS = 28,  IMP_SOLAR_PLANT = 29,
    IMP_HARBOR = 30,            IMP_OFFSHORE_PLATFORM = 31, IMP_AIRPORT = 32,
    IMP_POLICE_STATION = 33,    IMP_PORT_FACILITY = 34,     IMP_SS_STRUCTURAL = 35,
    IMP_SS_COMPONENT = 36,      IMP_SS_MODULE = 37,         IMP_CAPITALIZATION = 38,
    /* wonders */
    IMP_PYRAMIDS = 39,          IMP_HANGING_GARDENS = 40,   IMP_COLOSSUS = 41,
    IMP_LIGHTHOUSE = 42,        IMP_GREAT_LIBRARY = 43,     IMP_ORACLE = 44,
    IMP_GREAT_WALL = 45,        IMP_SUN_TZU = 46,           IMP_KING_RICHARDS = 47,
    IMP_MARCO_POLO = 48,        IMP_MICHELANGELO = 49,      IMP_COPERNICUS = 50,
    IMP_MAGELLAN = 51,          IMP_SHAKESPEARE = 52,       IMP_LEONARDO = 53,
    IMP_BACH = 54,              IMP_NEWTON = 55,            IMP_ADAM_SMITH = 56,
    IMP_DARWIN = 57,            IMP_STATUE_OF_LIBERTY = 58, IMP_EIFFEL_TOWER = 59,
    IMP_WOMENS_SUFFRAGE = 60,   IMP_HOOVER_DAM = 61,        IMP_MANHATTAN = 62,
    IMP_UNITED_NATIONS = 63,    IMP_APOLLO = 64,            IMP_SETI = 65,
    IMP_CURE_FOR_CANCER = 66,
    IMP_FIRST_WONDER = 39,          /* improvement - IMP_FIRST_WONDER: the WonderId; -building <= it: a wonder */
    IMP_COUNT = 67,                 /* improvements and wonders (improvements[]) */
};

/* Wonders (0..27: improvement 39 + n; game.wonder_city[], civ_has_wonder). */
enum WonderId : u8 {
    WONDER_PYRAMIDS = 0,           WONDER_HANGING_GARDENS = 1,    WONDER_COLOSSUS = 2,
    WONDER_LIGHTHOUSE = 3,         WONDER_GREAT_LIBRARY = 4,      WONDER_ORACLE = 5,
    WONDER_GREAT_WALL = 6,         WONDER_SUN_TZU = 7,            WONDER_KING_RICHARDS = 8,
    WONDER_MARCO_POLO = 9,         WONDER_MICHELANGELO = 10,      WONDER_COPERNICUS = 11,
    WONDER_MAGELLAN = 12,          WONDER_SHAKESPEARE = 13,       WONDER_LEONARDO = 14,
    WONDER_BACH = 15,              WONDER_NEWTON = 16,            WONDER_ADAM_SMITH = 17,
    WONDER_DARWIN = 18,            WONDER_STATUE_OF_LIBERTY = 19, WONDER_EIFFEL_TOWER = 20,
    WONDER_WOMENS_SUFFRAGE = 21,   WONDER_HOOVER_DAM = 22,        WONDER_MANHATTAN = 23,
    WONDER_UNITED_NATIONS = 24,    WONDER_APOLLO = 25,            WONDER_SETI = 26,
    WONDER_CURE_FOR_CANCER = 27,
    WONDER_COUNT = 28,
};

/* Advances (@CIVILIZE; civ_has_advance, game.advance_known_by[]). */
enum AdvanceId : u8 {
    ADV_ADVANCED_FLIGHT = 0,      ADV_ALPHABET = 1,             ADV_AMPHIBIOUS_WARFARE = 2,
    ADV_ASTRONOMY = 3,            ADV_ATOMIC_THEORY = 4,        ADV_AUTOMOBILE = 5,
    ADV_BANKING = 6,              ADV_BRIDGE_BUILDING = 7,      ADV_BRONZE_WORKING = 8,
    ADV_CEREMONIAL_BURIAL = 9,    ADV_CHEMISTRY = 10,           ADV_CHIVALRY = 11,
    ADV_CODE_OF_LAWS = 12,        ADV_COMBINED_ARMS = 13,       ADV_COMBUSTION = 14,
    ADV_COMMUNISM = 15,           ADV_COMPUTERS = 16,           ADV_CONSCRIPTION = 17,
    ADV_CONSTRUCTION = 18,        ADV_CORPORATION = 19,         ADV_CURRENCY = 20,
    ADV_DEMOCRACY = 21,           ADV_ECONOMICS = 22,           ADV_ELECTRICITY = 23,
    ADV_ELECTRONICS = 24,         ADV_ENGINEERING = 25,         ADV_ENVIRONMENTALISM = 26,
    ADV_ESPIONAGE = 27,           ADV_EXPLOSIVES = 28,          ADV_FEUDALISM = 29,
    ADV_FLIGHT = 30,              ADV_FUNDAMENTALISM = 31,      ADV_FUSION_POWER = 32,
    ADV_GENETIC_ENGINEERING = 33, ADV_GUERRILLA_WARFARE = 34,   ADV_GUNPOWDER = 35,
    ADV_HORSEBACK_RIDING = 36,    ADV_INDUSTRIALIZATION = 37,   ADV_INVENTION = 38,
    ADV_IRON_WORKING = 39,        ADV_LABOR_UNION = 40,         ADV_LASER = 41,
    ADV_LEADERSHIP = 42,          ADV_LITERACY = 43,            ADV_MACHINE_TOOLS = 44,
    ADV_MAGNETISM = 45,           ADV_MAP_MAKING = 46,          ADV_MASONRY = 47,
    ADV_MASS_PRODUCTION = 48,     ADV_MATHEMATICS = 49,         ADV_MEDICINE = 50,
    ADV_METALLURGY = 51,          ADV_MINIATURIZATION = 52,     ADV_MOBILE_WARFARE = 53,
    ADV_MONARCHY = 54,            ADV_MONOTHEISM = 55,          ADV_MYSTICISM = 56,
    ADV_NAVIGATION = 57,          ADV_NUCLEAR_FISSION = 58,     ADV_NUCLEAR_POWER = 59,
    ADV_PHILOSOPHY = 60,          ADV_PHYSICS = 61,             ADV_PLASTICS = 62,
    ADV_PLUMBING = 63,            ADV_POLYTHEISM = 64,          ADV_POTTERY = 65,
    ADV_RADIO = 66,               ADV_RAILROAD = 67,            ADV_RECYCLING = 68,
    ADV_REFINING = 69,            ADV_REFRIGERATION = 70,       ADV_REPUBLIC = 71,
    ADV_ROBOTICS = 72,            ADV_ROCKETRY = 73,            ADV_SANITATION = 74,
    ADV_SEAFARING = 75,           ADV_SPACE_FLIGHT = 76,        ADV_STEALTH = 77,
    ADV_STEAM_ENGINE = 78,        ADV_STEEL = 79,               ADV_SUPERCONDUCTOR = 80,
    ADV_TACTICS = 81,             ADV_THEOLOGY = 82,            ADV_THEORY_OF_GRAVITY = 83,
    ADV_TRADE = 84,               ADV_UNIVERSITY = 85,          ADV_WARRIOR_CODE = 86,
    ADV_WHEEL = 87,               ADV_WRITING = 88,             ADV_FUTURE_TECH = 89,
    ADV_USER_DEF_A = 90,          ADV_USER_DEF_B = 91,          ADV_USER_DEF_C = 92,
    ADV_EXTRA_1 = 93,             ADV_EXTRA_2 = 94,             ADV_EXTRA_3 = 95,
    ADV_EXTRA_4 = 96,             ADV_EXTRA_5 = 97,             ADV_EXTRA_6 = 98,
    ADV_EXTRA_7 = 99,
    ADV_COUNT = 100,                /* the slots (advances[], game.advance_known_by[]) */
};

/* Governments (@GOVERNMENTS; Civ.government): ordered, the code compares
 * them (`government < GOV_MONARCHY`). */
enum GovernmentId : u8 {
    GOV_ANARCHY = 0, GOV_DESPOTISM = 1, GOV_MONARCHY = 2, GOV_COMMUNISM = 3,
    GOV_FUNDAMENTALISM = 4, GOV_REPUBLIC = 5, GOV_DEMOCRACY = 6,
};

/* Base terrain types (@TERRAIN 0..10; tile_terrain, Tile.terrain & 0xf);
 * 11..32 are the two special resources of each. */
enum TerrainId : u8 {
    TERRAIN_DESERT = 0, TERRAIN_PLAINS = 1, TERRAIN_GRASSLAND = 2,
    TERRAIN_FOREST = 3, TERRAIN_HILLS = 4, TERRAIN_MOUNTAINS = 5,
    TERRAIN_TUNDRA = 6, TERRAIN_GLACIER = 7, TERRAIN_SWAMP = 8,
    TERRAIN_JUNGLE = 9, TERRAIN_OCEAN = 10,
    TERRAIN_COUNT = 11,             /* the base types */
    TERRAIN_SPECIAL_1 = 11,         /* + a base type: its first special resource's record (terrain_types[]) */
    TERRAIN_SPECIAL_2 = 22,         /* + a base type: its second */
    TERRAIN_RECORDS = 33,           /* the base types and both specials of each */
};

/* Tile.terrain: the type in the low nibble and two bits. */
enum TerrainBits : u8 {
    TERRAIN_TYPE_MASK = 0x0f,
    TERRAIN_NO_RESOURCE = 0x40,     /* special resource suppressed (tile_resource) */
    TERRAIN_RIVER = 0x80,
};

/* Unit orders (@ORDERS; Unit.orders).  The low nibble is the order; 0x10
 * marks an AI unit's own state. */
enum UnitOrder : i8 {
    ORDER_NONE = -1,
    ORDER_FORTIFYING = 1, ORDER_FORTIFIED = 2, ORDER_SLEEP = 3,
    ORDER_BUILD_FORTRESS = 4, ORDER_BUILD_ROAD = 5, ORDER_IRRIGATE = 6,
    ORDER_MINE = 7, ORDER_TRANSFORM = 8, ORDER_CLEAN_POLLUTION = 9,
    ORDER_BUILD_AIRBASE = 10, ORDER_GOTO = 11,
    ORDER_NO_ORDERS = 12,           /* the rules' name only; never assigned */
    ORDER_AI_IDLE = 0x10,           /* AI: no orders (ai_assess_unit_goal) */
    ORDER_AI_GOTO = 0x1b,           /* AI: goto one step, territory ignored (unit_goto_step) */
};

/* Unit roles (UnitType.role, CivGoal.role): how the AI uses a unit; 5..7
 * also give abilities. */
enum UnitRole : u8 {
    ROLE_ATTACK = 0, ROLE_DEFEND = 1, ROLE_NAVAL_SUPERIORITY = 2,
    ROLE_AIR_SUPERIORITY = 3, ROLE_SEA_TRANSPORT = 4, ROLE_SETTLE = 5,
    ROLE_DIPLOMACY = 6, ROLE_TRADE = 7,
};

/* Movement domains (UnitType.domain). */
enum UnitDomain : u8 {
    DOMAIN_LAND = 0, DOMAIN_AIR = 1, DOMAIN_SEA = 2,
};

/* Difficulty levels (@DIFFICULTY; game.difficulty): ordered. */
enum Difficulty : u8 {
    DIFF_CHIEFTAIN = 0, DIFF_WARLORD = 1, DIFF_PRINCE = 2, DIFF_KING = 3,
    DIFF_EMPEROR = 4, DIFF_DEITY = 5,
};

/* ===== flag words ===== */

/* Unit type abilities (UnitType.flags; the RULES.TXT flag string, bit 0
 * last). */
enum UnitTypeFlag : i32 {
    UTYPE_SEE_TWO_SPACES = 0x0001,
    UTYPE_IGNORE_ZOC = 0x0002,
    UTYPE_AMPHIBIOUS = 0x0004,          /* can attack from a ship */
    UTYPE_SUBMARINE = 0x0008,
    UTYPE_ATTACK_AIR = 0x0010,          /* fighter */
    UTYPE_COAST_BOUND = 0x0020,         /* must stay near land (trireme) */
    UTYPE_IGNORE_WALLS = 0x0040,        /* howitzer */
    UTYPE_CARRY_AIR = 0x0080,           /* carrier */
    UTYPE_PARADROP = 0x0100,
    UTYPE_ALPINE = 0x0200,              /* every tile as road */
    UTYPE_X2_VS_HORSE = 0x0400,         /* pikemen */
    UTYPE_FUNDAMENTALISM_FREE = 0x0800, /* free support under fundamentalism (fanatics) */
    UTYPE_DESTROYED_AFTER_ATTACK = 0x1000, /* missiles */
    UTYPE_X2_VS_AIR = 0x2000,           /* AEGIS */
    UTYPE_SPOT_SUBMARINES = 0x4000,
};

/* Unit state (Unit.flags). */
enum UnitFlag : u16 {
    UNITF_SCRATCH = 0x0001,             /* only ai_assess_scenario_unit, saved and restored */
    UNITF_SPY_ON_MISSION = 0x0002,      /* spy_city_mission_chat; aux_timer counts down to spy_try_escape */
    UNITF_BORDER_INTRUDER = 0x0004,     /* in a foreign city's radius this turn (diplo_count_border_intruders) */
    UNITF_AI_RETREATING = 0x0008,       /* damaged AI unit going home (ai_task 'd') */
    UNITF_PARADROPPED = 0x0010,
    UNITF_BORDER_WARNED = 0x0020,       /* a repeat intruder; ages out every 4 turns */
    UNITF_MOVED = 0x0040,               /* moved this turn: no healing */
    UNITF_GOAL_REACHED = 0x0080,        /* goto destination / AI goal reached */
    UNITF_AI_GARRISON = 0x0100,         /* AI: fortified, fortifies again */
    UNITF_AI_COLONIST = 0x0200,         /* AI: settler that may found a city (ai_unit_role 0x15) */
    UNITF_AGGRESSIVE = 0x0400,          /* away from home: unhappiness under republic/democracy */
    UNITF_COSTS_SHIELD = 0x0800,        /* its home city pays a shield for it */
    UNITF_ABOARD = 0x1000,              /* taken aboard by unit_gather_cargo */
    UNITF_VETERAN = 0x2000,
    UNITF_WAITING = 0x4000,             /* 'W': skipped until the others moved */
    UNITF_AUTOMATED = 0x8000,           /* auto settler: moved by the AI */
};

/* City state (City.flags). */
enum CityFlag : i32 {
    CITYF_DISORDER = 0x1,
    CITYF_CELEBRATING = 0x2,            /* We Love the King Day */
    CITYF_IMPROVEMENT_SOLD = 0x4,       /* this turn */
    CITYF_TECH_STOLEN = 0x8,
    CITYF_AUTOBUILD = 0x10,
    CITYF_DEFENDER_LOST = 0x20,         /* AI: buy a defender */
    CITYF_AI_SETTLER_NEAR = 0x40,
    CITYF_COASTAL = 0x80,               /* next to ocean */
    CITYF_WONDER_BUILT = 0x100,         /* last item completed was a wonder (AI: another scores x3) */
    CITYF_AI_FERRY_PORT = 0x200,
    CITYF_AI_FERRY_PORT_FRESH = 0x400,  /* the port assignment lasts one refresh */
    CITYF_HYDRO_POSSIBLE = 0x800,       /* river or mountains adjacent */
    CITYF_HAD_CARAVAN = 0x1000,         /* built or supported a caravan (never cleared): the AI builds no more */
    CITYF_DISORDER_2ND_TURN = 0x2000,
    CITYF_ON_EDGE = 0x4000,             /* happy == unhappy */
    CITYF_AI_SPECIALIST_HELD = 0x8000,  /* AI kept or added an entertainer against disorder */
    CITYF_AIRLIFTED = 0x10000,          /* airlift used this turn */
    CITYF_TRADE_DIRTY = 0x20000,        /* commodity demand to recompute */
    CITYF_SAM_WANTED = 0x40000,         /* nothing sets it, nor in the original: with it the AI's SAM Battery
                                           score is 3 better (city_choose_production); likely meant, like
                                           CITYF_COASTAL_FORT_USED, to mark a city attacked from the air,
                                           which unit_resolve_attack never records (docs/BUGS.md B5) */
    CITYF_AI_TERRAFORM_GOAL = 0x80000,  /* pollution to clean (city_add_terraform_goals) */
    CITYF_DISORDER_LONG = 0x100000,     /* three turns or more */
    CITYF_OCEAN_ACCESS = 0x200000,      /* water next to it is not a lake */
    CITYF_INVESTIGATED = 0x400000,
    CITYF_AUTO_UNITS_ONLY = 0x1000000,
    CITYF_AUTO_IMPROVEMENTS_ONLY = 0x2000000,
    CITYF_OBJECTIVE = 0x4000000,        /* scenario objective */
    CITYF_COASTAL_FORT_USED = 0x8000000, /* coastal fortress doubled a defence against ships */
};

/* Tile improvements and occupants (Tile.improvements, tile_improvements). */
enum TileFlag : u8 {
    TILE_UNIT = 0x01,                   /* a unit stands on it */
    TILE_CITY = 0x02,
    TILE_IRRIGATION = 0x04,
    TILE_MINE = 0x08,
    TILE_ROAD = 0x10,
    TILE_RAILROAD = 0x20,
    TILE_FORTRESS = 0x40,
    TILE_POLLUTION = 0x80,
    /* combinations with their own meaning */
    TILE_FARMLAND = 0x0c,               /* irrigation | mine */
    TILE_AIRBASE = 0x42,                /* fortress | city without a city */
};

/* Diplomatic state of civ a toward civ b (civs[a].treaties[b]).
 * diplo_set_treaty_flags / diplo_clear_treaty_flags keep both directions
 * (alliance implies peace; war clears the treaties).  The cheat's Edit
 * Treaties dialog names contact, cease-fire, peace, alliance, war, vendetta
 * and embassy; the rest from their uses. */
enum TreatyFlag : i32 {
    TREATY_CONTACT = 0x1,
    TREATY_CEASEFIRE = 0x2,
    TREATY_PEACE = 0x4,
    TREATY_ALLIANCE = 0x8,
    TREATY_VENDETTA = 0x10,             /* a holds a vendetta against b */
    TREATY_HOSTILE_INTENT = 0x20,       /* AI a means to break its treaty with b soon (trespass, spaceship) */
    TREATY_BROKEN = 0x40,               /* a cancelled its treaty with b: war is near */
    TREATY_EMBASSY = 0x80,              /* a has an embassy with b */
    TREATY_NUCLEAR_THREAT = 0x100,      /* a showed b its nukes (or b nuked a): a may nuke b */
    TREATY_FIGHTING = 0x200,            /* their units have fought */
    TREATY_RECENT_CONTACT = 0x400,      /* renewed; lapses every 16 turns */
    TREATY_AGGRESSOR = 0x800,           /* a wages war on b; decays after 32 turns */
    TREATY_SNEAK_ATTACK = 0x1000,       /* a started the war unprovoked */
    TREATY_WAR = 0x2000,
    TREATY_CHANGED_THIS_TURN = 0x4000,  /* cease-fire, peace or first contact this turn */
    TREATY_INCIDENT = 0x10000,          /* a has a reason to summon b to talks */
    TREATY_NUKED = 0x20000,             /* a has nuked b */
    TREATY_TRIBUTE = 0x40000,           /* a paid tribute to b */
    TREATY_AGGRESSOR_LONG = 0x80000,    /* extends TREATY_AGGRESSOR by 32 turns */
    TREATY_MERCENARY = 0x100000,        /* a hired by b against a third civ */
    TREATY_WAR_THIS_TURN = 0x200000,
    TREATY_CITY_CAPTURED = 0x400000,    /* b took a city from a */
    TREATY_MET_THIS_TURN = 0x800000,
    TREATY_SPY_CHAT = 0x1000000,        /* a's spy hears b's multiplayer chat */
};

/* Civilization state (Civ.flags). */
enum CivFlag : u16 {
    CIVF_REVOLUTION = 0x1,              /* government overthrown, anarchy to announce */
    CIVF_AT_WAR = 0x2,                  /* at war with someone (ai_civ_turn_assess) */
    CIVF_SENATE_ACTIVE = 0x4,           /* the Republic's senate sits (toggled at random each turn) */
    CIVF_GOV_CHANGED = 0x8,             /* this turn */
    CIVF_EIFFEL_APPLIED = 0x10,
    CIVF_FREE_ADVANCE = 0x20,           /* first to Philosophy: an advance to give */
    CIVF_UNUSED_40 = 0x40,              /* only ever cleared */
    CIVF_TRADED_TECH = 0x80,
    CIVF_HAS_ALLIED = 0x100,
    CIVF_FEMALE_LEADER = 0x200,
};

/* Spaceship state (Civ.spaceship_flags). */
enum SpaceshipFlag : u8 {
    SHIP_EXISTS = 0x1, SHIP_LAUNCHED = 0x2, SHIP_ARRIVED = 0x4,
    SHIP_FUSION = 0x8,                  /* Fusion Power known before launch: a faster flight */
    SHIP_WON = 0x10,                    /* arrived first */
};

/* Custom rules (game.rules; the @ADVANCED dialog).  0x2f is the base
 * value: bits 0x1..0x8 and 0x20 are always set; only 0x1 is tested. */
enum GameRuleFlag : u16 {
    RULE_BARBARIAN_FULL_ROSTER = 0x1,   /* always set (no dialog item clears it): barbarians also field
                                           Archers, Elephants, Crusaders, Dragoons, Artillery, Cavalry,
                                           Fanatics and Partisans (game_spawn_barbarians); clear, only
                                           Warriors, Legion, Horsemen, Chariots, Knights, Musketeers,
                                           Cannons and Riflemen.  Why a rule bit: unknown (no other use) */
    RULE_NORMAL_COMBAT = 0x10,          /* clear: Simplified Combat */
    RULE_BLOODLUST = 0x80,
    RULE_NO_RESTART_ELIMINATED = 0x100,
    RULE_FLAT_WORLD = 0x8000,
};

/* Options (game.game_options: @GAMEOPTIONS, @GRAPHICOPTIONS). */
enum GameOption : i32 {
    OPT_MUSIC = 0x8,
    OPT_SOUND = 0x10,
    OPT_MAP_GRID = 0x20,
    OPT_ENTER_CLOSES_CITY = 0x40,
    OPT_MOVE_WITH_MOUSE = 0x80,
    OPT_TUTORIAL = 0x100,
    OPT_INSTANT_ADVICE = 0x200,
    OPT_FAST_PIECE_SLIDE = 0x400,
    OPT_NO_PAUSE_ENEMY = 0x800,
    OPT_SHOW_ENEMY_MOVES = 0x1000,
    OPT_AUTOSAVE = 0x2000,
    OPT_ALWAYS_WAIT = 0x4000,           /* wait at end of turn */
    OPT_CHEAT_MODE = 0x8000,
    OPT_WONDER_MOVIES = 0x10000,
    OPT_THRONE_ROOM = 0x20000,
    OPT_DIPLOMACY_SCREEN = 0x40000,
    OPT_PEDIA_ADVANCES = 0x80000,       /* Civilopedia for advances */
    OPT_HIGH_COUNCIL = 0x100000,
    OPT_ANIMATED_HERALDS = 0x200000,
};

/* Game state (game.game_flags). */
enum GameFlag : u16 {
    GAME_AI_GANG_UP = 0x1,              /* the AIs unite against the leading human (or a human launch) */
    GAME_WON = 0x2,
    GAME_LARGE_MAP = 0x4,
    GAME_SMALL_MAP = 0x8,
    GAME_CHEATED = 0x10,
    GAME_OVER_SCORED = 0x20,            /* game over shown; play goes on */
    GAME_SAVED_AS_SCENARIO = 0x40,
    GAME_SCENARIO = 0x80,
};

/* game.transient_flags. */
enum TransientFlag : u16 {
    TRANSIENT_END_OF_TURN = 0x1,
    TRANSIENT_CITY_WINDOW_OPENED = 0x2, /* a city window was opened: the end of turn waits */
    TRANSIENT_ATTITUDE_DIRTY = 0x4,
};

/* One-shot hints already shown (game.shown_event_flags). */
enum ShownHint : u16 {
    HINT_CITY_STUFF = 0x1, HINT_AIR_UNIT = 0x2, HINT_SHIPS = 0x4,
    HINT_AQUEDUCT = 0x8, HINT_CARAVAN = 0x10, HINT_DAMAGE = 0x40,
    HINT_HEAVY_DAMAGE = 0x80,
};

/* Messages turned off (game.message_options, @MESSAGEOPTIONS). */
enum MessageOption : u16 {
    MSGOFF_GROWTH_HALTED = 0x1, MSGOFF_IMPROVEMENT_BUILT = 0x2,
    MSGOFF_UNIT_BUILT = 0x4, MSGOFF_INVALID_BUILD = 0x8,
    MSGOFF_DISORDER = 0x10, MSGOFF_ORDER_RESTORED = 0x20,
    MSGOFF_CELEBRATION = 0x40, MSGOFF_FOOD_LOW = 0x80,
    MSGOFF_POLLUTION = 0x100, MSGOFF_PRODUCTION_PENALTY = 0x200,
    MSGOFF_NO_ZOOM_TO_CITY = 0x400,
};

/* ===== user interface ===== */

/* Main menu commands (menu_command; the items of MENU.TXT, menu_rows in
 * src/game/lang.cpp): 0xMII, M the menu (1 Game .. 9 Civilopedia). */
enum MenuCommand : i32 {
    CMD_GAME_OPTIONS = 0x101, CMD_GRAPHIC_OPTIONS = 0x102,
    CMD_CITY_REPORT_OPTIONS = 0x103, CMD_MULTIPLAYER_OPTIONS = 0x104,
    CMD_GAME_PROFILE = 0x105, CMD_PICK_MUSIC = 0x106,
    CMD_SAVE_GAME = 0x110,
    CMD_LOAD_GAME = 0x120, CMD_JOIN_GAME = 0x130, CMD_SET_PASSWORD = 0x131,
    CMD_CHANGE_TIMER = 0x132, CMD_RETIRE = 0x1f0, CMD_QUIT = 0x1f1,
    CMD_TAX_RATE = 0x201, CMD_THRONE_ROOM = 0x205, CMD_FIND_CITY = 0x210,
    CMD_REVOLUTION = 0x220,
    CMD_MOVE_PIECES = 0x301, CMD_VIEW_PIECES = 0x302, CMD_ZOOM_IN = 0x310,
    CMD_ZOOM_OUT = 0x311, CMD_MAX_ZOOM_IN = 0x320, CMD_STANDARD_ZOOM = 0x321,
    CMD_MEDIUM_ZOOM_OUT = 0x322, CMD_MAX_ZOOM_OUT = 0x324, CMD_MAP_GRID = 0x327,
    CMD_ARRANGE_WINDOWS = 0x328, CMD_ARRANGE_WINDOWS_S = 0x329, CMD_ARRANGE_WINDOWS_L = 0x32a,   /* X38 (OpenCiv2) */
    CMD_SHOW_HIDDEN_TERRAIN = 0x330,
    CMD_CENTER_VIEW = 0x340,
    CMD_BUILD_CITY = 0x401, CMD_BUILD_ROAD = 0x410, CMD_BUILD_IRRIGATION = 0x411,
    CMD_BUILD_MINES = 0x412, CMD_TRANSFORM = 0x413, CMD_BUILD_AIRBASE = 0x417,
    CMD_BUILD_FORTRESS = 0x418, CMD_AUTOMATE = 0x41b,
    CMD_CLEAN_POLLUTION = 0x420, CMD_PILLAGE = 0x421, CMD_UNLOAD = 0x430,
    CMD_GOTO = 0x440, CMD_PARADROP = 0x441, CMD_AIRLIFT = 0x442,
    CMD_HOME_CITY = 0x445, CMD_FORTIFY = 0x450, CMD_SLEEP = 0x451,
    CMD_DISBAND = 0x460, CMD_ACTIVATE_UNIT = 0x468, CMD_WAIT = 0x470,
    CMD_SKIP_TURN = 0x480, CMD_END_TURN = 0x490,
    CMD_HIGH_COUNCIL = 0x500, CMD_CITY_STATUS = 0x501,
    CMD_DEFENSE_MINISTER = 0x502, CMD_FOREIGN_MINISTER = 0x503,
    CMD_ATTITUDE_ADVISOR = 0x504, CMD_TRADE_ADVISOR = 0x505,
    CMD_SCIENCE_ADVISOR = 0x506, CMD_CHAT_WITH_KINGS = 0x507,
    CMD_CASUALTY_TIMELINE = 0x508,
    CMD_WONDERS = 0x601, CMD_TOP_CITIES = 0x602, CMD_SCORE = 0x603,
    CMD_DEMOGRAPHICS = 0x605, CMD_SPACESHIPS = 0x606,
    CMD_CHEAT_MODE = 0x701, CMD_CHEAT_CREATE_UNIT = 0x711,
    CMD_CHEAT_REVEAL_MAP = 0x712, CMD_CHEAT_SET_HUMAN = 0x713,
    CMD_CHEAT_SET_YEAR = 0x721, CMD_CHEAT_KILL_CIV = 0x722,
    CMD_CHEAT_TECH_ADVANCE = 0x731, CMD_CHEAT_EDIT_TECHS = 0x732,
    CMD_CHEAT_FORCE_GOVERNMENT = 0x740, CMD_CHEAT_CHANGE_TERRAIN = 0x748,
    CMD_CHEAT_DESTROY_UNITS = 0x750, CMD_CHEAT_CHANGE_MONEY = 0x752,
    CMD_CHEAT_EDIT_UNIT = 0x755, CMD_CHEAT_EDIT_CITY = 0x760,
    CMD_CHEAT_EDIT_KING = 0x765, CMD_CHEAT_SCENARIO_PARAMS = 0x768,
    CMD_CHEAT_SAVE_SCENARIO = 0x770,
    CMD_SCENARIO_FLAG = 0x801, CMD_ADVANCES_EDITOR = 0x802,
    CMD_CITIES_EDITOR = 0x803, CMD_EFFECTS_EDITOR = 0x804,
    CMD_IMPROVEMENTS_EDITOR = 0x805, CMD_TERRAIN_EDITOR = 0x806,
    CMD_TRIBE_EDITOR = 0x807, CMD_UNITS_EDITOR = 0x808,
    CMD_EVENTS_EDITOR = 0x809,
    CMD_PEDIA_ADVANCES = 0x901, CMD_PEDIA_IMPROVEMENTS = 0x903,
    CMD_PEDIA_WONDERS = 0x905, CMD_PEDIA_UNITS = 0x907,
    CMD_PEDIA_GOVERNMENTS = 0x909, CMD_PEDIA_TERRAIN = 0x90b,
    CMD_PEDIA_CONCEPTS = 0x90d, CMD_ABOUT = 0x9f0,
};

/* A dialog's standard buttons (DialogButton.kind; their labels
 * dlg_style.button_labels[kind], POPUPS @LABELS): OK unless the style has
 * none, Help with style 0x40 (pressed, it sets style 0x80), Cancel with
 * style 1. */
enum DialogButtonKind : i32 {
    DLG_BUTTON_OK = 0, DLG_BUTTON_HELP = 1, DLG_BUTTON_CANCEL = 2,
};

/* Sound effects (snd_play_effect): the slot's WAV file in the original's
 * table (str_aircombt; the scenario's SOUND folder first).  Slots sharing a
 * file carry their number. */
enum SoundId : i32 {
    SND_AIRCOMBT = 0,     SND_AIRPLANE = 1,     SND_CHEERS_2 = 2,
    SND_CRWDBUGL_3 = 3,   SND_AQUEDUCT = 4,     SND_BARRACKS = 5,
    SND_BIGGUN = 6,       SND_BLDCITY = 7,      SND_BLDSPCSH = 8,
    SND_BOATSINK = 9,     SND_CATAPULT = 10,    SND_CATHEDRL = 11,
    SND_CAVALRY = 12,     SND_CHEERS_13 = 13,   SND_CIVDISOR = 14,
    SND_CHEERS_15 = 15,   SND_CHEERS_16 = 16,   SND_NEWGOVT_17 = 17,
    SND_CHEERS_18 = 18,   SND_CRWDBUGL_19 = 19, SND_NEWGOVT_20 = 20,
    SND_NEWGOVT_21 = 21,  SND_DIESEL = 22,      SND_DIVCRASH = 23,
    SND_DIVEBOMB = 24,    SND_ELEPHANT = 25,    SND_ENGNSPUT = 26,
    SND_CHEERS_27 = 27,   SND_FIRE = 28,        SND_NEWGOVT_29 = 29,
    SND_CHEERS_30 = 30,   SND_CHEERS_31 = 31,   SND_HELICPTR = 32,
    SND_HELISHOT = 33,    SND_INFANTRY = 34,    SND_LARGEXPL = 35,
    SND_CHEERS_36 = 36,   SND_CHEERS_37 = 37,   SND_MCHNGUNS = 38,
    SND_MEDEXPL = 39,     SND_MEDGUN = 40,      SND_MISSILE = 41,
    SND_MRKTPLCE = 42,    SND_NEWGOVT_43 = 43,  SND_CHEERS_44 = 44,
    SND_CHEERS_45 = 45,   SND_NAVBTTLE = 46,    SND_NEWBANK = 47,
    SND_NEWONDER = 48,    SND_CHEERS_49 = 49,   SND_NUKEXPLO = 50,
    SND_CHEERS_51 = 51,   SND_CHEERS_52 = 52,   SND_CRWDBUGL_53 = 53,
    SND_CHEERS_54 = 54,   SND_CHEERS_55 = 55,   SND_CHEERS_56 = 56,
    SND_CHEERS_57 = 57,   SND_CHEERS_58 = 58,   SND_CHEERS_59 = 59,
    SND_CHEERS_60 = 60,   SND_NEWGOVT_61 = 61,  SND_REVOLT = 62,
    SND_RIFLE = 63,       SND_CHEERS_64 = 64,   SND_CHEERS_65 = 65,
    SND_CHEERS_66 = 66,   SND_SMALLEXP = 67,    SND_SPYSOUND = 68,
    SND_STKMARKT = 69,    SND_SUBMRINE = 70,    SND_CHEERS_71 = 71,
    SND_CHEERS_72 = 72,   SND_SWORDFGT = 73,    SND_SWRDHORS = 74,
    SND_TANKMOTR = 75,    SND_CHEERS_76 = 76,   SND_TORPEDOS = 77,
    SND_JETSPUTR = 78,    SND_JETCRASH = 79,    SND_JETBOMB = 80,
    SND_JETPLANE = 81,    SND_JETCOMBT = 82,    SND_FANFARE1 = 83,
    SND_FANFARE2 = 84,    SND_FANFARE3 = 85,    SND_FANFARE4 = 86,
    SND_FANFARE5 = 87,    SND_FANFARE6 = 88,    SND_FANFARE7 = 89,
    SND_FANFARE8 = 90,    SND_FEEDBK01 = 91,    SND_FEEDBK02 = 92,
    SND_FEEDBK03 = 93,    SND_FEEDBK04 = 94,    SND_FEEDBK05 = 95,
    SND_FEEDBK06 = 96,    SND_FEEDBK07 = 97,    SND_FEEDBK08 = 98,
    SND_MOVPIECE = 99,    SND_ENDOTURN = 100,   SND_CUSTOM1 = 101,
    SND_CUSTOM2 = 102,    SND_CUSTOM3 = 103,    SND_POS1 = 104,
    SND_NEG1 = 105,       SND_MENUOK = 106,     SND_MENULOOP = 107,
    SND_MENUEND = 108,    SND_BUY = 109,        SND_SELL = 110,
    SND_GUILLOTN = 111,   SND_DRUMAL = 112,     SND_DRUMAY = 113,
    SND_DRUMA0 = 114,     SND_DRUMAN = 115,     SND_DRUMBL = 116,
    SND_DRUMBY = 117,     SND_DRUMB0 = 118,     SND_DRUMBN = 119,
    SND_DRUMCL = 120,     SND_DRUMCY = 121,     SND_DRUMC0 = 122,
    SND_DRUMCN = 123,     SND_LETTER = 124,     SND_EXTRA1 = 125,
    SND_EXTRA2 = 126,     SND_EXTRA3 = 127,     SND_EXTRA4 = 128,
    SND_EXTRA5 = 129,     SND_EXTRA6 = 130,     SND_EXTRA7 = 131,
    SND_EXTRA8 = 132,
};

/* ===== network ===== */

/* Game message types (NetMsgHeader.msg_type): the original's NM_* name
 * table (str_nm_datagram), in its order; docs/xdaemon-protocol.md. */
enum NetMsgType : i32 {
    NM_DATAGRAM = 0,                 NM_GET_SERVERS = 1,
    NM_SERVER_INFO = 2,              NM_CLIENT_SERVER = 3,
    NM_PLAYER_LIST = 4,              NM_SERVER_REJECT = 5,
    NM_SERVER_PATH = 6,              NM_FULL_GAMESTATE = 7,
    NM_DIFF_GAMESTATE = 8,           NM_JOIN_GAMESTATE = 9,
    NM_MAP_STRUCT = 10,              NM_SERVER_CANCELED_START = 11,
    NM_LOST_SERVER_ON_JOIN = 12,     NM_CLIENT_QUIT = 13,
    NM_SERVER_QUIT_REQUEST = 14,     NM_SERVER_QUIT = 15,
    NM_SERVER_QUIT_ACK = 16,         NM_SERVER_FULL = 17,
    NM_SERVER_QUERY = 18,            NM_SERVER_ANSWER = 19,
    NM_CIVILIZATION_PING = 20,       NM_DIFFERENCE_ENGINE = 21,
    NM_START_CIVILIZATION = 22,      NM_COSMIC = 23,
    NM_CIVILIZE = 24,                NM_IMPROVE = 25,
    NM_ENDWONDER = 26,               NM_UNITS = 27,
    NM_TERRAIN = 28,                 NM_GOVT = 29,
    NM_GOVT_TITLE = 30,              NM_LEADERS = 31,
    NM_KINGS = 32,                   NM_CARAVAN = 33,
    NM_ORDERS = 34,                  NM_DIFFICULTY = 35,
    NM_ATTITUDES = 36,               NM_EVENTSLOADED = 37,
    NM_NATION_ACCEPT = 38,           NM_NATION_REJECT = 39,
    NM_GAME_GOING = 40,              NM_BEFORE_PRIORITY_ALPHA = 41,
    NM_LOCK_DATA = 42,               NM_HEART_BEAT = 43,
    NM_HEART_BEAT_ACK = 44,          NM_QUIT_REQUEST = 45,
    NM_PLAYER_INFO = 46,             NM_PLAYER_LIST_NODE = 47,
    NM_LOCK_NATION = 48,             NM_UNLOCK_NATION = 49,
    NM_CLIENT_CANCELED_START = 50,   NM_MULTIOPTIONS_CHANGE = 51,
    NM_KILL_OR_QUIT = 52,            NM_KILL_OR_QUIT_ACK = 53,
    NM_BEFORE_ALLOCATION = 54,       NM_DELETE_UNIT = 55,
    NM_UNIT_DELETED = 56,            NM_DELETE_CITY = 57,
    NM_CITY_DELETED = 58,            NM_CREATE_CITY = 59,
    NM_CITY_CREATED = 60,            NM_CREATE_UNIT = 61,
    NM_UNIT_CREATED = 62,            NM_PICK_UP_UNIT = 63,
    NM_UNIT_PICKED_UP = 64,          NM_PUT_DOWN_UNIT = 65,
    NM_UNIT_PUT_DOWN = 66,           NM_MOVE_TO_BOTTOM = 67,
    NM_BOTTOMED = 68,                NM_RELOCATE_UNIT = 69,
    NM_UNIT_RELOCATED = 70,          NM_ACQUIRE_WONDER = 71,
    NM_WONDER_ACQUIRED = 72,         NM_STACK_SHIP = 73,
    NM_SHIP_STACKED = 74,            NM_STACK_UNIT = 75,
    NM_UNIT_STACKED = 76,            NM_DELETE_SAFELY = 77,
    NM_SAFELY_DELETED = 78,          NM_DELETE_VISIBLY = 79,
    NM_VISIBLY_DELETED = 80,         NM_LOCK_MAP = 81,
    NM_UNLOCK_MAP = 82,              NM_LOCK_MAP_ACK = 83,
    NM_UNLOCK_MAP_ACK = 84,          NM_LOCK_MAP_SYNC = 85,
    NM_NEWTURNTIMER_REPLY = 86,      NM_PMCHANGE_REPLY = 87,
    NM_DO_APOLLO = 88,               NM_MAP_PACKETS = 89,
    NM_REALTIME_STACKER = 90,        NM_REALTIME_STACKER_ACKER = 91,
    NM_ALPHA_DIFFERENCE_ENGINE = 92, NM_DRAG_HOLD_ON = 93,
    NM_DRAG_HOLD_OFF = 94,           NM_NEW_CIV = 95,
    NM_KILL_CIV = 96,                NM_NEW_CIV_ACK = 97,
    NM_KILL_CIV_ACK = 98,            NM_BRIBED_UNIT = 99,
    NM_DEATH_STACK = 100,            NM_HOLD_FOR_JOIN = 101,
    NM_CONTINUE_AFTER_JOIN = 102,    NM_AFTER_PRIORITY_ALPHA = 103,
    NM_CHAT = 104,                   NM_START_WHOSE_TURN = 105,
    NM_POPUP = 106,                  NM_END_SEQUENCE = 107,
    NM_TIMER_CHANGE = 108,           NM_START_TURN_TIMER = 109,
    NM_STOP_TURN_TIMER = 110,        NM_RESTART_TURN_TIMER = 111,
    NM_SLIDE_PIECE = 112,            NM_POSSIBLY_CENTER = 113,
    NM_DRAW_SQUARE = 114,            NM_DRAW_TWO = 115,
    NM_DRAW_MAP = 116,               NM_DRAW_AREA = 117,
    NM_DRAW_MAP_REGION = 118,        NM_CANESCAPE_REPLY = 119,
    NM_INFO_STATUS = 120,            NM_INFO_UPDATE = 121,
    NM_SOUND_PLAY = 122,             NM_DESERT_REPLY = 123,
    NM_KABOOM = 124,                 NM_SHOW_NUKE = 125,
    NM_ASK_ALLY = 126,               NM_REPORTS_UPDATE = 127,
    NM_PARLEYREQUEST_REPLY = 128,    NM_PARLEYREQUEST_CANCEL = 129,
    NM_PARLEY_TRANSACTION = 130,     NM_PARLEY_ACCEPT_OFFER = 131,
    NM_PARLEY_NO_THANKS = 132,       NM_PARLEY_COUNTER_OFFER = 133,
    NM_PARLEY_EXECUTE = 134,         NM_CITYWIN_CHECK_UNIT = 135,
    NM_CITYWIN_CHECK_CITY = 136,     NM_CITYWIN_DELETED = 137,
    NM_CITYWIN_REDRAW = 138,         NM_PRODUCTION_BEGIN = 139,
    NM_PRODUCTION_COMPLETE = 140,    NM_AI_MOVEMENT = 141,
    NM_HUMAN_MOVEMENT = 142,         NM_HUMAN_MOVE_COMPLETE = 143,
    NM_FEATURE_SET = 144,            NM_TERRAIN_SET = 145,
    NM_SEEN_SET = 146,               NM_SITE_SET = 147,
    NM_OWNER_SET = 148,              NM_REGION_SET = 149,
    NM_CITY_USING_SET = 150,         NM_FEAT_LOC_SET = 151,
    NM_SET_ATTITUDE = 152,           NM_DO_AIPARLEY = 153,
    NM_SET_SLIDE_SOUND = 154,        NM_SET_CIV = 155,
    NM_READY_TO_START = 156,         NM_KINGDOM_RATES = 157,
    NM_TAX_CHECK = 158,              NM_PICK_GOVT = 159,
    NM_POLITICAL_VIOLATIONS = 160,   NM_POLITICAL_ATTITUDE = 161,
    NM_TURN_FINISHED = 162,          NM_ABORT_KABOOM = 163,
    NM_PRETEXT = 164,                NM_REVEAL_CITY_INFO = 165,
    NM_HIDE_CITY_INFO = 166,         NM_REVEAL_CITY_ORIGIN = 167,
    NM_HIDE_CITY_ORIGIN = 168,
};

#endif
