// SOURCE src/auto/unit.cpp unit_delete_ship_at_sea
// B36: a ship lost at sea (unit_delete_with_cargo, fixed) takes its cargo,
// the units unit_gather_cargo marked aboard, and the land units of its
// owner the other ships cannot carry; nothing else on the square.
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include "native.h"
#include "win32_types.h"
#include "civ2types.h"
#include "enums.h"
static_assert(sizeof(void *) == 4, "run with a 32-bit compiler");

static Unit units[2048];
static UnitType unit_types[62];
static std::vector<int> square;     // the units on the square, in stack order
static std::vector<int> deleted;

static int unit_stack_first(int) { return square.empty() ? -1 : square[0]; }
static int unit_next_in_stack(int unit) {
    auto i = std::find(square.begin(), square.end(), unit);
    assert(i != square.end());
    return i + 1 == square.end() ? -1 : *(i + 1);
}
static void unit_delete(int unit, int) {
    auto i = std::find(square.begin(), square.end(), unit);
    assert(i != square.end());
    square.erase(i);
    deleted.push_back(unit);
}

#include "production.inc"

enum { TRANSPORT, TRIREME, CARRIER, ARMOR, FIGHTER, SUBMARINE };

static void add(int unit, int type, int owner, bool aboard) {
    memset(&units[unit], 0, sizeof units[unit]);
    units[unit].type = (u8)type;
    units[unit].owner = (i8)owner;
    units[unit].flags = aboard ? UNITF_ABOARD : 0;
    square.push_back(unit);
}

static bool gone(int unit) { return std::find(deleted.begin(), deleted.end(), unit) != deleted.end(); }

static void reset() { square.clear(); deleted.clear(); }

int main() {
    unit_types[TRANSPORT].domain = DOMAIN_SEA, unit_types[TRANSPORT].hold = 8;
    unit_types[TRIREME].domain = DOMAIN_SEA, unit_types[TRIREME].hold = 2;
    unit_types[CARRIER].domain = DOMAIN_SEA, unit_types[CARRIER].flags = UTYPE_CARRY_AIR;
    unit_types[SUBMARINE].domain = DOMAIN_SEA, unit_types[SUBMARINE].flags = UTYPE_SUBMARINE;
    unit_types[ARMOR].domain = DOMAIN_LAND;
    unit_types[FIGHTER].domain = DOMAIN_AIR;

    // Two transports: the lost one's two passengers go, the other ship and
    // its passenger stay (the original: all five gone).
    reset();
    add(10, TRANSPORT, 3, false);
    add(11, ARMOR, 3, true);
    add(12, TRANSPORT, 3, false);
    add(13, ARMOR, 3, false);
    add(14, ARMOR, 3, true);
    unit_delete_ship_at_sea(10);
    assert(gone(10) && gone(11) && gone(14));
    assert(!gone(12) && !gone(13));

    // The other ship (a trireme, 2) can't take all three left: the last goes.
    reset();
    add(20, TRANSPORT, 2, false);
    add(21, TRIREME, 2, false);
    for (int u = 22; u < 30; u++)
        add(u, ARMOR, 2, true);         // 8 aboard the transport
    add(30, ARMOR, 2, false);
    add(31, ARMOR, 2, false);
    add(32, ARMOR, 2, false);
    unit_delete_ship_at_sea(20);
    assert(gone(20));
    for (int u = 22; u < 30; u++)
        assert(gone(u));
    assert(!gone(21) && !gone(30) && !gone(31) && gone(32));

    // A carrier, a submarine (no hold) and aircraft in flight: no room for
    // the land unit; the aircraft not aboard, and another civ's unit, stay.
    reset();
    add(40, TRANSPORT, 1, false);
    add(41, CARRIER, 1, false);
    add(42, SUBMARINE, 1, false);
    add(43, FIGHTER, 1, false);
    add(44, ARMOR, 1, false);
    add(45, ARMOR, 4, false);
    unit_delete_ship_at_sea(40);
    assert(gone(40) && gone(44));
    assert(!gone(41) && !gone(42) && !gone(43) && !gone(45));

    // Barbarians: any ship carries 20.
    reset();
    add(50, TRANSPORT, 0, false);
    add(51, TRIREME, 0, false);
    for (int u = 52; u < 55; u++)
        add(u, ARMOR, 0, false);
    unit_delete_ship_at_sea(50);
    assert(gone(50) && !gone(51) && !gone(52) && !gone(53) && !gone(54));
    return 0;
}
