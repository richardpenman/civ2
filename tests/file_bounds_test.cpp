// SOURCE src/auto/game.cpp save_text_terminated
// SOURCE src/auto/game.cpp save_records_valid
// SOURCE src/auto/game.cpp save_read_units_cities
// SOURCE src/auto/game.cpp save_game_record_valid
// SOURCE src/auto/map.cpp map_header_valid
// SOURCE src/auto/map.cpp map_tiles_valid
// SOURCE src/game/retvals.cpp save_event_trigger_valid
// SOURCE src/auto/sound.cpp snd_read_pcm_format
// SOURCE src/auto/sound.cpp snd_wave_load_file
// SOURCE src/auto/sound.cpp snd_wave_open_stream
#include <cassert>
#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>
#include "native.h"
#include "win32_types.h"
#include "civ2types.h"
typedef unsigned int uint;
typedef unsigned char byte;
#include "enums.h"
static_assert(sizeof(void *) == 4, "run with a 32-bit compiler");
static_assert(sizeof(Unit) == 32 && sizeof(City) == 88 && sizeof(WaveSource) == 188,
              "production layouts");
static GameState game;
static struct {
    Unit units[2048];
} power_graph_history_block;
static Unit *const units = power_graph_history_block.units;
static City cities[256];
static struct {
    short unit, city;
} next_ids;
static UnitType unit_types[62];
static Civ civs[8];
static CivNames civ_names[8];
static MapHeader map;
static struct {
    Tile *tiles;
} map_data;
static i32 map_in_bounds(iptr x, iptr y) { return x >= 0 && x < 40 && y >= 0 && y < 20; }
static int reads, fail_read;
static std::vector<unsigned char> payload; // what the reads return (zeros past it)
static size_t payload_pos;
static size_t _fread(void *dst, size_t n, size_t count, FILE *) {
    reads++;
    if (fail_read == reads)
        return 0;
    for (size_t i = 0; i < n * count; i++, payload_pos++)
        ((unsigned char *)dst)[i] = payload_pos < payload.size() ? payload[payload_pos] : 0;
    return count;
}
static struct {
    int total_buffer_bytes;
    WaveSource *sources;
} snd_wave;
static std::vector<unsigned char> fmt;
static DWORD declared_size;
static size_t fmt_pos;
static int closed, shown_error, allocated, format_reads, max_read;
static WaveSource record;
static HMMIO mmioOpenA(LPSTR, LPMMIOINFO, DWORD) { return 1; }
static MMRESULT mmioDescend(HMMIO, MMCKINFO *chunk, MMCKINFO *, UINT flags) {
    if (flags == 0x10 && chunk->ckid == 0x20746d66)
        chunk->cksize = declared_size;
    else
        chunk->cksize = 4;
    return 0;
}
static LONG mmioRead(HMMIO, HPSTR dst, LONG n) {
    assert(n >= 0);
    format_reads++;
    if (n > max_read)
        max_read = n;
    size_t count = (size_t)n < fmt.size() - fmt_pos ? (size_t)n : fmt.size() - fmt_pos;
    if (count != 0) {
        memcpy(dst, fmt.data() + fmt_pos, count);
    }
    fmt_pos += count;
    return (LONG)count;
}
static MMRESULT mmioAscend(HMMIO, MMCKINFO *, UINT) {
    assert(fmt_pos == declared_size);
    return 0;
}
static MMRESULT mmioClose(HMMIO, UINT) {
    closed++;
    return 0;
}
static MMRESULT mmioGetInfo(HMMIO, MMIOINFO *info, UINT) {
    memset(info, 0, sizeof *info);
    return 0;
}
static MMRESULT mmioAdvance(HMMIO, MMIOINFO *, UINT) { return 0; }
static HGLOBAL GlobalAlloc(UINT, size_t n) {
    assert(n == sizeof record);
    allocated++;
    return 1;
}
static void *GlobalLock(HGLOBAL) { return &record; }
static HGLOBAL GlobalFree(HGLOBAL) { return 0; }
static void snd_show_error(int code) { shown_error = code; }
static void snd_wave_stream_refill(WaveSource *) {}
#define _memset memset
#include "production.inc"
static void wave(size_t actual, DWORD declared) {
    fmt.assign(actual, 0x5a);
    declared_size = declared;
    fmt_pos = 0;
    WAVEFORMATEX pcm = {1, 1, 22050, 22050, 1, 8, 0};
    if (actual != 0) {
        memcpy(fmt.data(), &pcm, actual < 16 ? actual : 16);
    }
    closed = shown_error = allocated = format_reads = max_read = 0;
    snd_wave.sources = nullptr;
}
static int load(int which) {
    char path[] = "test.wav";
    return which ? snd_wave_open_stream(path, 0, 3) : snd_wave_load_file(path, 0, 3, nullptr);
}
// A small valid game: two cities, three units (one a caravan on goto), as
// the game writes them; each case breaks one field.
static void valid_records() {
    memset(units, 0, sizeof power_graph_history_block);
    memset(cities, 0, sizeof cities);
    memset(unit_types, 0, sizeof unit_types);
    unit_types[48].role = ROLE_TRADE;
    game.num_units = 3;
    game.num_cities = 2;
    for (int c = 0; c < 2; c++) {
        City &city = cities[c];
        city.id = c + 1;
        city.owner = (i8)(c + 1);
        strcpy(city.name, "Fifteen letters");
        city.building = c ? -66 : 61;
        city.trade_routes = 1;
        city.route_partner[0] = (i16)(1 - c);
        city.route_goods[0] = c ? 15 : 0xff;
        city.supplies[0] = (u8)-15;
        city.demands[2] = 15;
    }
    for (int u = 0; u < 3; u++) {
        Unit &unit = units[u];
        unit.id = u + 1;
        unit.owner = 7;
        unit.type = u ? 61 : 48;
        unit.counter = u ? 200 : 15;
        unit.home_city = (i8)(u == 2 ? 0xff : u);
        unit.prev_in_stack = (i16)(u ? u - 1 : -1);
        unit.next_in_stack = (i16)(u < 2 ? 2047 : -1);
        unit.orders = ORDER_GOTO;
        unit.x = 39;
        unit.y = 19;
    }
    units[3].type = 255; // free slots past the count and inactive records are not checked
    units[3].owner = 9;
}
template <class T, class V> static void refused(T &field, V value) {
    valid_records();
    T keep = field;
    field = (T)value;
    assert(save_records_valid() == 0);
    field = keep;
    assert(save_records_valid() == 1);
}
static void save_records() {
    valid_records();
    assert(save_records_valid() == 1);
    units[1].id = 0; // an inactive record keeps whatever it has
    units[1].type = 200;
    units[1].owner = -3;
    units[1].next_in_stack = 5000;
    assert(save_records_valid() == 1);
    refused(units[0].type, 62);
    refused(units[0].owner, -1);
    refused(units[0].owner, 8);
    refused(units[1].prev_in_stack, -2);
    refused(units[1].next_in_stack, 2048);
    refused(units[0].home_city, 2);    // past the cities in use
    refused(units[0].counter, 16);     // a caravan's commodity
    refused(units[2].x, 40);           // goto from off the map
    valid_records();
    units[2].orders = ORDER_AI_GOTO;
    units[2].x = -1;
    assert(save_records_valid() == 0);
    units[2].orders = ORDER_FORTIFIED; // other orders may be off the map
    assert(save_records_valid() == 1);
    valid_records();
    cities[1].id = 0; // a home city that is not active
    assert(save_records_valid() == 0);
    refused(cities[1].owner, 8);
    refused(cities[0].trade_routes, 4);
    refused(cities[0].route_partner[0], 256);
    refused(cities[0].route_partner[0], -1);
    refused(cities[1].route_goods[0], 16);
    refused(cities[0].supplies[0], -16);
    refused(cities[0].demands[2], 16);
    refused(cities[0].building, 62);
    refused(cities[1].building, -67);
    valid_records();
    memset(cities[0].name, 'x', sizeof cities[0].name);
    assert(save_records_valid() == 0);
    // Through the reader: the records read, then refused (current format).
    valid_records();
    payload.assign((unsigned char *)units, (unsigned char *)(units + 3));
    payload.insert(payload.end(), (unsigned char *)cities, (unsigned char *)(cities + 2));
    for (int bad = 0; bad < 2; bad++) {
        if (bad)
            payload[6] = 62; // the first unit's type
        memset(units, 0, 3 * sizeof(Unit));
        payload_pos = 0;
        reads = fail_read = 0;
        game.num_units = 3;
        game.num_cities = 2;
        assert(save_read_units_cities(0x2c, 0) == !bad && payload_pos == payload.size());
    }
    payload.clear();
}
static void valid_game() {
    memset(&game, 0, sizeof game);
    game.difficulty = 5;
    game.barbarian_activity = 3;
    game.turn_civ = 7;
    game.player_civ = 1;
    game.saved_current_civ = 1;
    game.best_human_rank = 7;
    game.num_enemies = 6;
    for (int c = 0; c < 8; c++) {
        civs[c].tribe = (i16)(c ? 20 : 0);
        civs[c].researching = (i16)(c ? 99 : -1);
        civs[c].government = 6;
        memset(&civ_names[c], 0, sizeof civ_names[c]);
        memset(civ_names[c].leader_name, 'L', 23);
        memset(civ_names[c].titles[6], 'T', 23);
    }
}
template <class T, class V> static void game_refused(T &field, V value) {
    valid_game();
    T keep = field;
    field = (T)value;
    assert(save_game_record_valid() == 0);
    field = keep;
    assert(save_game_record_valid() == 1);
}
static void game_record() {
    game_refused(game.difficulty, 6);
    game_refused(game.barbarian_activity, 4);
    game_refused(game.turn_civ, 8);
    game_refused(game.turn_civ, -1);
    game_refused(game.player_civ, -1);
    game_refused(game.saved_current_civ, 8);
    game_refused(game.best_human_rank, 8);
    game_refused(game.worst_human_rank, 8);
    game_refused(game.num_enemies, 255); // the load's count from no civ alive
    game_refused(civs[3].tribe, 21);
    game_refused(civs[3].tribe, -1);
    game_refused(civs[4].researching, 100);
    game_refused(civs[4].researching, -2);
    game_refused(civs[5].government, 7);
    valid_game();
    game.multiplayer = 3; // a network game's local civ comes from the session
    game.saved_current_civ = -1;
    assert(save_game_record_valid() == 1);
    valid_game();
    civ_names[2].tribe_name[23] = 'x';
    memset(civ_names[2].tribe_name, 'x', 24);
    assert(save_game_record_valid() == 0);
    valid_game();
    memset(civ_names[7].titles[3], 'x', 24);
    assert(save_game_record_valid() == 0);
}
static void map_and_events() {
    MapHeader header = {};
    for (auto dims : {std::make_pair(2, 1), std::make_pair(150, 120), std::make_pair(2, 32767),
                      std::make_pair(130, 504)}) {
        header.width = (i16)dims.first;
        header.height = (i16)dims.second;
        assert(map_header_valid(&header) == 1);
    }
    for (auto dims : {std::make_pair(0, 10), std::make_pair(3, 10), std::make_pair(-2, 10),
                      std::make_pair(100, 0), std::make_pair(100, -80), std::make_pair(132, 504)}) {
        header.width = (i16)dims.first;
        header.height = (i16)dims.second;
        assert(map_header_valid(&header) == 0);
    }
    std::vector<Tile> tiles(64);
    map_data.tiles = tiles.data();
    map.area = 64;
    for (Tile &t : tiles) {
        t.terrain = 0xca; // river, no special, ocean
        t.landmass = 63;
        t.owner = 0xf3;
    }
    tiles[1].owner = 0x7f;
    assert(map_tiles_valid() == 1);
    tiles[63].terrain = 0x0b;
    assert(map_tiles_valid() == 0);
    tiles[63].terrain = 0x0a;
    tiles[63].landmass = 64;
    assert(map_tiles_valid() == 0);
    tiles[63].landmass = 0;
    for (int owner = 8; owner < 15; owner++) {
        tiles[63].owner = (u8)(owner << 4);
        assert(map_tiles_valid() == 0);
    }
    map.area = 63; // only the map's squares
    assert(map_tiles_valid() == 1);
    ScenarioEvent event = {};
    char whom[] = "Romans";
    for (int k = -1; k < 9; k++) {
        event.trigger = k < 0 ? 0 : 1 << k;
        event.technology = 1000;
        assert(save_event_trigger_valid(&event) == 1); // the advance only with a civ
    }
    for (int bad : {3, 0x200, 0x101, -1})
        event.trigger = bad, assert(save_event_trigger_valid(&event) == 0);
    event.trigger = 0x100;
    event.whom_name = whom;
    for (int tech : {0, 99})
        event.technology = tech, assert(save_event_trigger_valid(&event) == 1);
    for (int tech : {-1, 100})
        event.technology = tech, assert(save_event_trigger_valid(&event) == 0);
}
int main() {
    for (int version : {0x26, 0x29, 0x2c}) {
        for (int count : {-1, 2049, 32767}) {
            game.num_units = (short)count;
            game.num_cities = 0;
            reads = 0;
            next_ids.unit = 7;
            assert(save_read_units_cities(version, 0) == 0 && reads == 0 && next_ids.unit == 7);
        }
        for (int count : {-1, 257, 32767}) {
            game.num_units = 0;
            game.num_cities = (short)count;
            reads = 0;
            next_ids.city = 9;
            assert(save_read_units_cities(version, 0) == 0 && reads == 0 && next_ids.city == 9);
        }
        game.num_units = game.num_cities = 0;
        reads = 0;
        assert(save_read_units_cities(version, 0) == 1 && reads == 0);
        game.num_units = 2048;
        game.num_cities = 256;
        reads = fail_read = 0;
        assert(save_read_units_cities(version, 0) == 1 && reads > 0);
        game.num_units = game.num_cities = 1;
        reads = 0;
        fail_read = 1;
        assert(save_read_units_cities(version, 0) == 0 && reads == 1);
        fail_read = 0;
    }
    for (int which = 0; which < 2; which++) {
        for (int n = 0; n < 16; n++) {
            wave(n, n);
            assert(load(which) == 10);
            assert(closed == 1 && shown_error == 10 && allocated == 0);
        }
        wave(7, 8);
        assert(load(which) == 8 && closed == 1 && allocated == 0 && shown_error == 0);
        for (int n : {16, 18, 1024}) {
            wave(n, n);
            assert(load(which) == 0);
            assert(fmt_pos == (size_t)n && allocated == 1);
            assert(record.samples_per_sec == 22050 && record.bits_per_sample == 8 &&
                   record.channels == 1);
            assert(max_read <= 256 && shown_error == 0 && snd_wave.sources == &record);
        }
        for (int n : {0, 8, 15, 17, 280}) {
            wave(n, n < 16 ? 16 : 1024);
            assert(load(which) == 8);
            assert(closed == 1 && allocated == 0 && shown_error == 0);
        }
        wave(16, 0xffffffffu);
        assert(load(which) == 8 && max_read <= 256 && closed == 1);
        wave(16, 16);
        fmt[14] = 16;
        assert(load(which) == 10 && closed == 1 && shown_error == 10);
    }
    save_records();
    game_record();
    map_and_events();
    puts("files: count bounds in three save versions, both WAV loaders, "
         "extensions/truncation/errors, unit/city/game/map/event domains passed");
}
