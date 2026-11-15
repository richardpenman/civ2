// SOURCE src/game/extras.cpp extras_food_turns
// SOURCE src/game/extras.cpp extras_production_turns
// The TurnsLeft extra's countdowns (docs/EXTRAS.md) against a turn by turn
// simulation of the game's own order (src/auto/city.cpp, city_process_turn):
// city_process_food first grows the city when its box is full (box >=
// (size + 1) * food rows), or shrinks it (famine) when the box is below 0,
// then the turn's food surplus is added; city_process_production adds the
// turn's shields (none in disorder or below 0), then completes the item
// when the box holds its cost (cost * shield rows).
#include <cassert>
#include <cstdio>

#include "production.inc"

// The turns until the city grows (> 0) or shrinks (< 0), 0: never.
static int simulate_food(int box, int capacity, int surplus)
{
    for (int turn = 1; turn <= 100000; turn++) {
        if (box >= capacity)
            return turn;
        if (box < 0)
            return -turn;
        box += surplus;
    }
    return 0;
}

static int simulate_production(int box, int cost, int net)
{
    if (net < 0)
        net = 0;
    for (int turn = 1; turn <= 100000; turn++) {
        box += net;
        if (box >= cost)
            return turn;
    }
    return 0;
}

int main()
{
    int cases = 0;
    for (int capacity = 2; capacity <= 160; capacity += 3)
        for (int box = 0; box <= capacity + 5; box++)
            for (int surplus = -12; surplus <= 12; surplus++) {
                int got = extras_food_turns(box, capacity, surplus), want = simulate_food(box, capacity, surplus);
                if (got != want) {
                    std::printf("food: box %d of %d, surplus %d: %d, the game %d\n", box, capacity, surplus, got, want);
                    return 1;
                }
                cases++;
            }
    for (int cost = 10; cost <= 6000; cost += 37)
        for (int box = 0; box <= cost + 3; box += 1 + cost / 50)
            for (int net = -3; net <= 40; net++) {
                int got = extras_production_turns(box, cost, net), want = simulate_production(box, cost, net);
                if (got != want) {
                    std::printf("production: box %d of %d, net %d: %d, the game %d\n", box, cost, net, got, want);
                    return 1;
                }
                cases++;
            }
    // a few by hand: a full box grows next turn; 40 of 120 at +6 grows in 15
    assert(extras_food_turns(120, 120, 6) == 1);
    assert(extras_food_turns(40, 120, 6) == 15);
    assert(extras_food_turns(40, 120, 0) == 0);
    assert(extras_food_turns(5, 120, -2) == -4);
    assert(extras_production_turns(3, 40, 5) == 8);
    assert(extras_production_turns(40, 40, 0) == 1);
    assert(extras_production_turns(10, 40, 0) == 0);
    std::printf("TurnsLeft: %d cases as the game counts them\n", cases);
    return 0;
}
