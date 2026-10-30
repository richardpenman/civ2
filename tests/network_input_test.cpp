// SOURCE src/auto/game.cpp diffeng_rle_decoded_size
// SOURCE src/auto/game.cpp diffeng_rle_decode
// SOURCE src/auto/game.cpp diffeng_rle_encode
// SOURCE src/auto/network.cpp net_state_diff_record_fits
// SOURCE src/auto/network.cpp net_message_min_size
// SOURCE src/auto/network.cpp net_parley_item_valid
// SOURCE src/auto/network.cpp net_parley_offer_valid
// SOURCE src/auto/network.cpp net_popup_args_valid
// SOURCE src/auto/network.cpp net_rule_table_valid
// SOURCE src/auto/network.cpp net_message_body_valid
// SOURCE src/auto/network.cpp net_received_message_valid
// SOURCE src/auto/network.cpp net_on_broadcast_receive
// SOURCE src/auto/network.cpp net_on_secure_receive
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <tuple>
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
static_assert(sizeof(NetMsg) == 44 && sizeof(NetNamedMsg) == 116 && sizeof(NetServerInfoMsg) == 408,
              "wire layouts");
static int enqueued;
static std::vector<unsigned char> queued;
i32 NetMsgQueue::enqueue(int, NetMsg *msg, uint size) {
    enqueued++;
    queued.assign((unsigned char *)msg, (unsigned char *)msg + size);
    return 1;
}
static struct {
    NetMsgQueue msg_queue;
    int nm_recv_counts_by_type[169];
    uint last_recv_callback_size, last_queued_send_seq, last_recv_msg_size;
    int msgq_normal_count, msgq_priority_count;
} net;
static struct {
    char net_ip_address[32];
} menu_prefs;
static struct {
    char active;
} mp_drag;
static int net_last_recv_msg_type = -1, net_send_sequence = 7;
static struct {
    int used_dwords;
    int record_sizes[8];
} net_batch = {0, {5, 4, 5, 4, 4, 4, 4, 6}};
static DiffEngSection diffeng_sections[0x18];
static DiffEngSection *const diffeng_section_table = diffeng_sections;
#define _memset memset
#define crt_memcpy memcpy
static void mp_release_mouse_capture() {}
#define LOG_TEXT(name) static char name[] = #name
LOG_TEXT(str_broadcastreceivefunc_killed_message_tiny);
LOG_TEXT(str_broadcastreceivefunc_killed_message_not);
LOG_TEXT(str_securereceivefunc_killed_message_not_civ);
LOG_TEXT(str_last_callback_size_s);
LOG_TEXT(str_last_legal_sequence_number_s);
LOG_TEXT(str_last_legal_type_s);
LOG_TEXT(str_last_legal_netmsg_size_s);
LOG_TEXT(str_sequence_number_s);
LOG_TEXT(str_message_size_s);
LOG_TEXT(str_s_3);
LOG_TEXT(str_empty_184);
LOG_TEXT(str_potential_offset_message_found);
LOG_TEXT(str_offset_s_bytes);
LOG_TEXT(str_type_s);
LOG_TEXT(str_size_s_bytes);
LOG_TEXT(str_sequence_number_s_2);
static char str_nm_datagram[5408], net_hexdump_space_str[] = " ";
static std::string diagnostic_type;
static void dbg_log(const char *) {}
static int refused;
static void dbg_logf_int2(const char *, int, int) {
    refused++;
}
static void dbg_logf_str(const char *label, const char *text) {
    if (label == str_type_s)
        diagnostic_type = text;
}
static char *__itoa(int n, char *out, int radix) {
    if (radix == 16)
        sprintf(out, "%x", (unsigned)n);
    else
        sprintf(out, "%d", n);
    return out;
}
static char *__ultoa(unsigned long n, char *out, int) {
    sprintf(out, "%lu", n);
    return out;
}
#define _strlen strlen
#define _strcmp strcmp
#define crt_strcat strcat
static const unsigned char *packet;
static size_t captured, backing;
static void check_read(const void *p, size_t n) {
    uintptr_t off = (uintptr_t)p - (uintptr_t)packet;
    if (off <= backing)
        assert(off <= captured && n <= captured - off);
}
static void *checked_copy(void *dst, const void *src, size_t n) {
    check_read(src, n);
    return memcpy(dst, src, n);
}
static const void *checked_find(const void *p, int c, size_t n) {
    check_read(p, n);
    return memchr(p, c, n);
}
#define memcpy checked_copy
#define memchr checked_find
#include "production.inc"
#undef memcpy
#undef memchr
static void put(std::vector<unsigned char> &p, size_t off, int value) {
    memcpy(p.data() + off, &value, 4);
}
static std::vector<unsigned char> message(size_t n, int type) {
    std::vector<unsigned char> p(n);
    put(p, 0, 0x66606660);
    put(p, 4, type);
    put(p, 8, (int)n);
    return p;
}
static int valid(const std::vector<unsigned char> &p, size_t size) {
    packet = p.data();
    captured = size;
    backing = p.size();
    return net_received_message_valid(packet, (uint)size);
}
static void secure(std::vector<unsigned char> &p, size_t size, int expected) {
    packet = p.data();
    captured = size;
    backing = p.size();
    enqueued = 0;
    queued.clear();
    net_on_secure_receive(3, p.data(), (uint)size, 0);
    assert(enqueued == expected);
    if (expected)
        assert(queued == std::vector<unsigned char>(p.begin(), p.begin() + size));
}
static void broadcast(std::vector<unsigned char> &p, size_t size, int expected) {
    packet = p.data();
    captured = size;
    backing = p.size();
    enqueued = 0;
    net_on_broadcast_receive(p.data(), (ushort)size, 0);
    assert(enqueued == expected);
}
static std::vector<unsigned char> args(int type, std::initializer_list<int> values, size_t extra = 0) {
    std::vector<unsigned char> p = message(44 + extra, type);
    int i = 0;
    for (int v : values)
        put(p, 16 + 4 * i++, v);
    put(p, 40, (int)extra);
    return p;
}
static int ok(const std::vector<unsigned char> &p) {
    return valid(p, p.size());
}
// A parley item: kind, its size (header and ids), value, then the ids.
static void item(std::vector<unsigned char> &p, int kind, int value, int ids) {
    size_t at = p.size();
    p.resize(at + 12 + 4 * ids);
    put(p, at, kind);
    put(p, at + 4, 12 + 4 * ids);
    put(p, at + 8, value);
}
static std::vector<unsigned char> offer(int mode) {
    std::vector<unsigned char> p = message(0x20, NM_PARLEY_TRANSACTION);
    put(p, 16, mode);
    return p;
}
static void message_bodies() {
    // Each type needs what its handler reads; one byte less is refused.
    const std::pair<int, size_t> sizes[] = {
        {NM_PLAYER_LIST, 0x280}, {NM_SERVER_PATH, 0x21c}, {NM_SERVER_ANSWER, 0x134},
        {NM_PLAYER_INFO, 0x118}, {NM_PLAYER_LIST_NODE, 0x64}, {NM_START_WHOSE_TURN, 0x14},
        {NM_COSMIC, 44 + 0x29}, {NM_CIVILIZE, 44 + 0x640}, {NM_IMPROVE, 44 + 0x218},
        {NM_ENDWONDER, 44 + 0x1c}, {NM_UNITS, 44 + 0x4d8}, {NM_TERRAIN, 44 + 0x318},
        {NM_GOVT, 44 + 0x1c}, {NM_GOVT_TITLE, 44 + 0x38}, {NM_LEADERS, 44 + 0x3f0},
        {NM_KINGS, 44 + 0x7524}, {NM_CARAVAN, 44 + 0x40}, {NM_ORDERS, 44 + 0x68},
        {NM_DIFFICULTY, 44 + 0x18}, {NM_ATTITUDES, 44 + 0x24}, {NM_CITY_DELETED, 44 + 0x58},
        {NM_SLIDE_PIECE, 44 + 0x20}, {NM_JOIN_GAMESTATE, 44}, {NM_MAP_STRUCT, 44},
        {NM_DIFFERENCE_ENGINE, 44}, {NM_ALPHA_DIFFERENCE_ENGINE, 44}, {NM_EVENTSLOADED, 44},
        {NM_LOCK_MAP_SYNC, 44}, {NM_KILL_OR_QUIT, 44}, {NM_QUIT_REQUEST, 44}, {NM_POPUP, 44},
        {NM_SET_ATTITUDE, 44}, {NM_HIDE_CITY_ORIGIN, 44}, {NM_HEART_BEAT, 16},
        {NM_KILL_OR_QUIT_ACK, 16}, {NM_PARLEYREQUEST_CANCEL, 16}, {NM_DRAW_MAP, 16},
        {NM_ABORT_KABOOM, 16}};
    for (auto s : sizes) {
        assert(net_message_min_size(s.first) == s.second);
        std::vector<unsigned char> p = message(s.second, s.first);
        if (s.first == NM_POPUP)
            put(p, 40, 0);
        assert(ok(p));
        assert(!valid(p, s.second - 1) || s.second == 16);
    }
    // Peer selectors: the map-lock rows, civ slots, city and unit tables (BF-263, BF-233, ...).
    for (int type : {NM_LOCK_MAP, NM_UNLOCK_MAP, NM_LOCK_MAP_ACK, NM_UNLOCK_MAP_ACK, NM_LOCK_MAP_SYNC,
                     NM_QUIT_REQUEST, NM_KILL_OR_QUIT, NM_NEW_CIV, NM_TURN_FINISHED,
                     NM_PRODUCTION_COMPLETE, NM_HUMAN_MOVE_COMPLETE}) {
        assert(ok(args(type, {7})));
        assert(!ok(args(type, {8})) && !ok(args(type, {-1})));
    }
    for (int type : {NM_SET_ATTITUDE, NM_DO_AIPARLEY, NM_POLITICAL_VIOLATIONS, NM_POLITICAL_ATTITUDE,
                     NM_PRETEXT}) {
        assert(ok(args(type, {7, 0})));
        assert(!ok(args(type, {0, 8})) && !ok(args(type, {8, 0})));
    }
    assert(ok(args(NM_KILL_CIV, {3, -1})) && !ok(args(NM_KILL_CIV, {3, -2})) && !ok(args(NM_KILL_CIV, {8, 1})));
    assert(ok(args(NM_CITY_DELETED, {255}, 0x58)));
    assert(!ok(args(NM_CITY_DELETED, {256}, 0x58)) && !ok(args(NM_CITY_DELETED, {-1}, 0x58)));
    assert(ok(args(NM_CITY_CREATED, {-1})) && !ok(args(NM_CITY_CREATED, {4, 5})));
    assert(ok(args(NM_CITY_CREATED, {4, 256}, 0x58)) && !ok(args(NM_CITY_CREATED, {4, 257}, 0x58)));
    assert(!ok(args(NM_CITY_CREATED, {256, 257}, 0x58)));
    assert(ok(args(NM_DELETE_CITY, {-5})) && ok(args(NM_DELETE_CITY, {255})) && !ok(args(NM_DELETE_CITY, {256})));
    assert(ok(args(NM_UNIT_CREATED, {-1, 99999})) && ok(args(NM_UNIT_CREATED, {2047, 2048})));
    assert(!ok(args(NM_UNIT_CREATED, {5, 2049})) && !ok(args(NM_UNIT_CREATED, {2048, 2048})));
    assert(ok(args(NM_DELETE_UNIT, {-1})) && ok(args(NM_STACK_UNIT, {2047})) && !ok(args(NM_DELETE_UNIT, {2048})));
    assert(ok(args(NM_PICK_UP_UNIT, {2047})) && !ok(args(NM_PICK_UP_UNIT, {-1})));
    assert(ok(args(NM_CREATE_UNIT, {61, 7})) && !ok(args(NM_CREATE_UNIT, {62, 7})) && !ok(args(NM_CREATE_UNIT, {0, 8})));
    assert(ok(args(NM_CREATE_CITY, {-3, 900, 7})) && !ok(args(NM_CREATE_CITY, {0, 0, 8})));
    assert(ok(args(NM_ACQUIRE_WONDER, {7, 255, 27, 999, 66})));
    assert(!ok(args(NM_ACQUIRE_WONDER, {7, 255, 28, 999, 66})) && !ok(args(NM_ACQUIRE_WONDER, {7, 256, 0, 0, 0})));
    assert(!ok(args(NM_ACQUIRE_WONDER, {7, 0, 0, 0, 67})));
    assert(ok(args(NM_REALTIME_STACKER, {7, -1, 2047, 0, 5, 5})) && !ok(args(NM_REALTIME_STACKER, {8, 0, 0})));
    assert(!ok(args(NM_REALTIME_STACKER, {7, 0, 2048})));
    assert(ok(args(NM_BRIBED_UNIT, {2047, 7, 300})) && !ok(args(NM_BRIBED_UNIT, {2048, 7})));
    assert(ok(args(NM_DEATH_STACK, {0, 2047})) && !ok(args(NM_DEATH_STACK, {0, 2048})));
    assert(ok(args(NM_EVENTSLOADED, {1, 110})) && !ok(args(NM_EVENTSLOADED, {1, 111})) &&
           !ok(args(NM_EVENTSLOADED, {2, 0})));
    std::vector<unsigned char> p = message(20, NM_START_WHOSE_TURN);
    put(p, 16, 7);
    assert(ok(p));
    put(p, 16, 8);
    assert(!ok(p));
    // Strings the handlers copy (BF-222, BF-247, BF-248, BF-171/249).
    p = message(0x21c, NM_SERVER_PATH);
    memset(p.data() + 0x12, 'x', 0x103);
    memset(p.data() + 0x116, 'x', 0x105);
    assert(ok(p));
    p[0x12 + 0x103] = 'x';
    assert(!ok(p));
    p = message(0x21c, NM_SERVER_PATH);
    memset(p.data() + 0x116, 'x', 0x106);
    assert(!ok(p));
    p = message(0x134, NM_SERVER_ANSWER);
    memset(p.data() + 16 + 0x30, 'x', 31);
    assert(ok(p));
    p[16 + 0x30 + 31] = 'x';
    assert(!ok(p));
    for (int field : {0, 0x70, 0x90}) {
        p = message(0x134, NM_SERVER_ANSWER);
        memset(p.data() + 16 + field, 'x', 32);
        assert(!ok(p));
    }
    p = message(0x64, NM_PLAYER_LIST_NODE);
    memset(p.data() + 0x34, 'x', 31);
    assert(ok(p));
    p[0x34 + 31] = 'x';
    assert(!ok(p));
    p = message(0x118, NM_PLAYER_INFO);
    put(p, 0x10, 7);
    put(p, 0x14, 20);
    put(p, 0x18, 1);
    put(p, 0x114, 3);
    assert(ok(p));
    for (auto bad : {std::pair<int, int>{0x10, 8}, {0x14, 21}, {0x18, 2}, {0x114, 4}, {0x10, -1}}) {
        std::vector<unsigned char> q = p;
        put(q, bad.first, bad.second);
        assert(!ok(q));
    }
    for (int field : {0x20, 0x38, 0x50, 0x6c, 0x6c + 6 * 24}) {
        std::vector<unsigned char> q = p;
        memset(q.data() + field, 'x', 24);
        assert(!ok(q));
    }
    // Rule tables (BF-213/216/218/221): advance, category, epoch and terrain indices.
    p = message(44 + 0x640, NM_CIVILIZE);
    p[44 + 99 * 16 + 0xe] = (unsigned char)-2;
    p[44 + 99 * 16 + 0xf] = 99;
    p[44 + 5 * 16 + 0xc] = 4;
    p[44 + 5 * 16 + 0xd] = 3;
    assert(ok(p));
    for (auto bad : {std::pair<int, int>{0xe, 100}, {0xf, -3}, {0xc, 5}, {0xd, 4}}) {
        std::vector<unsigned char> q = p;
        q[44 + 7 * 16 + bad.first] = (unsigned char)bad.second;
        assert(!ok(q));
    }
    p = message(44 + 0x218, NM_IMPROVE);
    p[44 + 66 * 8 + 6] = (unsigned char)-1;
    assert(ok(p));
    p[44 + 66 * 8 + 6] = 100;
    assert(!ok(p));
    p = message(44 + 0x1c, NM_ENDWONDER);
    p[44 + 27] = (unsigned char)-1;
    assert(ok(p));
    p[44 + 27] = 0x80;
    assert(!ok(p));
    p = message(44 + 0x4d8, NM_UNITS);
    p[44 + 61 * 0x14 + 8] = (unsigned char)-1;
    p[44 + 61 * 0x14 + 0x13] = 99;
    assert(ok(p));
    p[44 + 61 * 0x14 + 0x13] = 100;
    assert(!ok(p));
    p = message(44 + 0x318, NM_TERRAIN);
    p[44 + 10 * 0x18 + 0xd] = 10;
    p[44 + 10 * 0x18 + 0xe] = (unsigned char)-2;
    p[44 + 11 * 0x18 + 0xd] = 50; // special terrains carry no changes
    assert(ok(p));
    p[44 + 10 * 0x18 + 0xf] = 11;
    assert(!ok(p));
    // Popups (BF-130..133, BF-241, BF-275): payload length, strings, numbers, indices.
    const char text[] = "Rome\0Babylon";
    p = args(NM_POPUP, {99, 2, 1, 87, 0}, sizeof text + 4);
    memcpy(p.data() + 44, text, sizeof text);
    assert(ok(p));
    std::vector<unsigned char> q = p;
    put(q, 40, (int)sizeof text + 3);
    assert(!ok(q));
    q = p;
    put(q, 24, 2); // two numbers do not fit
    assert(!ok(q));
    q = p;
    put(q, 20, 3); // a third string is not there
    assert(!ok(q));
    q = p;
    put(q, 28, 100); // advance 100
    assert(!ok(q));
    q = args(NM_POPUP, {0x4f, 25, 0, 0, 0}, 25);
    assert(!ok(q));
    q = args(NM_POPUP, {0x4f, 1, 0, 0, 0}, 0x105);
    memset(q.data() + 44, 'x', 0x104);
    assert(!ok(q)); // a string longer than a text_args slot
    q[44 + 0x103] = 0;
    assert(ok(q));
    const std::tuple<int, int, int, bool> popups[] = {
        {0x37, 62, 0, true},    {0x37, 63, 0, false},   {2, 2047, 0, true},      {2, 2048, 0, false},
        {0x23, 7, 2047, true},  {0x23, 8, 0, false},    {0x23, 0, 2048, false},  {0x1b, 7, 255, true},
        {0x1b, 7, 256, false},  {0x47, -1, 66, true},   {0x47, -1, 67, false},   {0x47, 27, 0, true},
        {0x47, 28, 0, false},   {0x4d, 99, 66, true},   {0x4d, 100, 0, false},   {0x2b, 6, 0, true},
        {0x2b, 7, 0, false},    {0xe, 7, 0, true},      {0xe, 8, 0, false},      {0x12, 66, 0, true},
        {0x12, 67, 0, false},   {0x59, 66, 7, true},    {0x59, 66, 8, false},    {0x46, 255, 0, true},
        {0x46, -1, 0, false},   {0x3d, -1, 0, false},   {0x41, 12345, -9, true}};
    for (auto c : popups)
        assert(ok(args(NM_POPUP, {std::get<0>(c), 0, 0, std::get<1>(c), std::get<2>(c)})) == std::get<3>(c));
    // Parley offers (BF-178): both items inside the message, their ids too.
    p = offer(0xe);
    item(p, 8, 2, 2);
    item(p, 9, 500, 0);
    put(p, 8, (int)p.size());
    assert(ok(p));
    for (size_t n = 0x20; n < p.size(); n++)
        assert(!valid(p, n));
    put(p, 0x28, 3); // three advances in room for two
    assert(!ok(p));
    put(p, 0x28, -1);
    assert(!ok(p));
    p = offer(0xe);
    item(p, 0x11, 2, 4); // cities: two ids each
    item(p, 10, 1, 1);
    assert(ok(p));
    put(p, 0x24, 0x1000); // first item past the end
    assert(!ok(p));
    put(p, 0x24, 8); // smaller than an item
    assert(!ok(p));
    p = offer(7);
    item(p, 8, 1, 1);
    assert(ok(p));
    put(p, 4, NM_PARLEY_EXECUTE);
    assert(ok(p));
    p = offer(0xe);
    item(p, 5, 3, 0);
    assert(!ok(p)); // an exchange without its second item
    // Map batches (BF-243): record types and sizes.
    p = args(NM_MAP_PACKETS, {}, 4 + 5 * 4 + 6 * 4);
    put(p, 44, 2);
    put(p, 48, 0);
    put(p, 48 + 20, 7);
    assert(ok(p));
    assert(!valid(p, p.size() - 1) && !valid(p, 47));
    put(p, 48 + 20, 8);
    assert(!ok(p));
    put(p, 48 + 20, 7);
    put(p, 44, 3);
    assert(!ok(p));
    put(p, 44, -1);
    assert(ok(p));
    // State diffs (BF-277): records inside the message.
    p = args(NM_ALPHA_DIFFERENCE_ENGINE, {2}, 12 + 8 + 12 + 3);
    put(p, 44 + 8, 8);
    put(p, 44 + 20 + 8, 3);
    assert(ok(p));
    assert(!valid(p, p.size() - 1));
    put(p, 16, 3);
    assert(!ok(p));
    put(p, 16, 2);
    put(p, 44 + 8, -1);
    assert(!ok(p));
}
static std::vector<unsigned char> decode(std::vector<unsigned char> src, size_t room, int &ok_out) {
    RleStream rle;
    std::vector<unsigned char> out(room + 1, 0xee);
    rle.src = src.data();
    rle.src_end = src.data() + src.size();
    rle.dst = out.data();
    rle.dst_end = out.data() + room;
    ok_out = diffeng_rle_decode(&rle);
    assert(out[room] == 0xee);
    out.resize(ok_out ? rle.out_len : 0);
    return out;
}
static void run_length_streams() {
    // BF-092: the encoder's long run (code 0x7fff) and long literal (0xffff), round trip.
    std::vector<unsigned char> raw(40000, 0);
    for (int i = 0; i < 33000; i++)
        raw.push_back((unsigned char)(i * 7 + i / 255));
    raw.insert(raw.end(), 5, 9);
    std::vector<unsigned char> coded(raw.size());
    RleStream rle;
    rle.src = raw.data();
    rle.src_end = raw.data() + raw.size();
    rle.dst = coded.data();
    rle.dst_end = coded.data() + coded.size();
    assert(diffeng_rle_encode(&rle));
    coded.resize(rle.out_len);
    assert(coded[0] == 0xff && coded[1] == 0x7f && coded[6] == 0xff && coded[7] == 0xff); // run 32767, literal 32767
    rle.src = coded.data();
    rle.src_end = coded.data() + coded.size();
    assert(diffeng_rle_decoded_size(&rle) == (int)raw.size());
    int good;
    assert(decode(coded, raw.size(), good) == raw && good);
    decode(coded, raw.size() - 1, good);
    assert(!good);
    // Malformed streams: cut codes, literals past the end, runs past the room.
    for (size_t cut = 1; cut < 6; cut++) {
        std::vector<unsigned char> s(coded.begin(), coded.end() - cut);
        decode(s, raw.size(), good);
        assert(good == (cut == 3)); // 3: without the last run (5 x 9)
    }
    decode({0xff, 0x7f, 1}, 32767, good);
    assert(good);
    decode({0xff, 0x7f, 1}, 32766, good);
    assert(!good);
    decode({0xff, 0x7f}, 40000, good);
    assert(!good);
    decode({5, 0x80, 1, 2, 3, 4}, 40000, good);
    assert(!good);
    decode({5}, 40000, good);
    assert(!good);
    assert(decode({3, 0x80, 1, 2, 3, 2, 0, 7}, 5, good) == std::vector<unsigned char>({1, 2, 3, 7, 7}) && good);
    // A state diff record inside its section (BF-092, BF-277).
    diffeng_sections[5].padded_size = 0x10000;
    diffeng_sections[0x17].padded_size = 50000;
    unsigned char run[] = {0xff, 0x7f, 0};
    assert(net_state_diff_record_fits(5, 0x10000 - 8, 8, run));
    assert(!net_state_diff_record_fits(5, 0x10000 - 7, 8, run));
    assert(!net_state_diff_record_fits(5, 0x10001, 0, run));
    assert(net_state_diff_record_fits(0x8005, 0x10000 - 32767, 3, run));
    assert(!net_state_diff_record_fits(0x8005, 0x10000 - 32766, 3, run));
    assert(net_state_diff_record_fits(0x17, 0, 50000, run));
    assert(!net_state_diff_record_fits(0x18, 0, 0, run));
    assert(!net_state_diff_record_fits(0x10005, 0, 0, run));
    assert(!net_state_diff_record_fits(0x18005, 0, 0, run));
}
int main() {
    std::vector<unsigned char> p = message(16, NM_HEART_BEAT);
    for (int n = 0; n < 16; n++) {
        assert(!valid(p, n));
        secure(p, n, 0);
        broadcast(p, n, 0);
    }
    secure(p, 16, 1);
    assert(net.last_recv_callback_size == 16 && net_last_recv_msg_type == NM_HEART_BEAT);
    assert(!net_received_message_valid(nullptr, 16));
    net_on_secure_receive(0, nullptr, 16, 0);
    for (int type : {-1, 169, 0x7fffffff}) {
        put(p, 4, type);
        secure(p, 16, 0);
        broadcast(p, 16, 0);
    }
    p = message(116, NM_DATAGRAM);
    put(p, 112, NM_GET_SERVERS);
    strcpy(menu_prefs.net_ip_address, "local");
    for (int n = 16; n < 116; n++) {
        assert(!valid(p, n));
        broadcast(p, n, 0);
    }
    broadcast(p, 116, 1);
    strcpy((char *)p.data() + 48, "local");
    broadcast(p, 116, 1);
    strcpy((char *)p.data() + 80, "local");
    broadcast(p, 116, 0); // self announcement
    p[80] = 0;
    strcpy((char *)p.data() + 48, "other");
    broadcast(p, 116, 0); // wrong target
    for (int offset : {16, 48, 80}) {
        p = message(116, NM_DATAGRAM);
        put(p, 112, NM_GET_SERVERS);
        memset(p.data() + offset, 'x', 32);
        secure(p, 116, 0);
        broadcast(p, 116, 0);
    }
    p = message(408, NM_DATAGRAM);
    put(p, 112, NM_SERVER_INFO);
    for (int n = 116; n < 408; n++)
        assert(!valid(p, n));
    secure(p, 408, 1);
    broadcast(p, 408, 1);
    for (auto field : {std::pair<int, int>{0, 32}, {48, 64}, {112, 32}, {144, 32}, {188, 80}}) {
        p = message(408, NM_DATAGRAM);
        put(p, 112, NM_SERVER_INFO);
        memset(p.data() + 116 + field.first, 'x', field.second);
        assert(!valid(p, 408));
    }
    p = message(116, NM_DATAGRAM);
    put(p, 112, -1);
    assert(!valid(p, 116));
    put(p, 112, 169);
    assert(!valid(p, 116));
    p = message(44 + 6, NM_CHAT);
    put(p, 16, 1);
    put(p, 20, 7);
    memcpy(p.data() + 44, "hello", 6);
    secure(p, p.size(), 1);
    broadcast(p, p.size(), 1);
    for (int n = 16; n <= 44; n++)
        assert(!valid(p, n));
    p.back() = 'x';
    secure(p, p.size(), 0);
    p = message(44 + 257, NM_CHAT);
    memset(p.data() + 44, 'x', 256);
    secure(p, p.size(), 1);
    p.push_back(0);
    secure(p, p.size(), 0);
    p = message(45, NM_CHAT);
    secure(p, p.size(), 1); // empty, terminated chat
    for (int offset : {16, 20})
        for (int civ : {-1, 8}) {
            p = message(45, NM_CHAT);
            put(p, offset, civ);
            secure(p, p.size(), 0);
        }
    // Invalid-magic diagnostics: a full header at an unaligned offset may be logged.
    strcpy(str_nm_datagram + 32, "HELLO");
    p.assign(48, 0);
    put(p, 1, 0x66606660);
    put(p, 5, 1);
    put(p, 9, 16);
    diagnostic_type.clear();
    secure(p, 17, 0);
    assert(diagnostic_type == "HELLO");
    diagnostic_type.clear();
    secure(p, 16, 0);
    assert(diagnostic_type.empty());
    put(p, 5, 0x7fffffff);
    secure(p, 17, 0);
    assert(diagnostic_type == "2147483647");
    message_bodies();
    run_length_streams();
    puts("network: captured headers, discovery fields/filtering, chat bounds, diagnostic scans, "
         "message sizes and indices, popups, parley offers, records and run-length streams passed");
}
