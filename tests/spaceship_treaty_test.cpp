// SOURCE src/auto/diplomacy.cpp diplo_clear_treaty_flags
// SOURCE src/auto/spaceship.cpp spaceship_launch
// B37: at Emperor and Deity, when a human launches a spaceship that arrives
// before an AI's, the AI ends its cease-fire or peace with the human.  The
// original ends it on the AI's side only and says nothing: the player's side
// still says peace.  Fixed: both sides, and the AI's emissary tells the
// launching player (diplo_treaty_break_notice).
#include <cassert>
#include <cstdio>
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
static_assert(sizeof(Civ) == 0x594, "production layout");

static GameState game;
static Civ civs[8];
static City cities[256];
static iptr human_civ[2];
static struct {
    int active;
} autoplay;
static char str_launched_2[12] = "LAUNCHED";

static int plat_fix_bugs;
static inline int bugfix_state(int n) { return n == 37 && plat_fix_bugs && game.multiplayer < 3; }

static const char *names[8] = {"Barbarians", "Spanish", "French", "", "", "", "", ""};
static char *civ_name_plural(int civ) { return (char *)names[civ]; }
static void text_set_string_arg(int, const char *) {}
static void text_set_number_arg(int, int) {}
static void net_send_text_args(int, int, int, int, int, int) { assert(!"network"); }
static void popup_game(const char *) {}
static int spaceship_wnd_run(int) { return 0; }
static void city_set_production(int, int) {}
static int civ_spaceship_launched(int civ) { return (civs[civ].spaceship_flags & SHIP_LAUNCHED) != 0; }
static void diplo_set_treaty_flags(int, int, uint) {}
static std::vector<std::string> notices;    // ai, human, peace or cease-fire
static void diplo_treaty_break_notice(int ai, int human, int was_peace) {
    char s[64];
    std::snprintf(s, sizeof s, "%d %d %s", ai, human, was_peace ? "peace" : "ceasefire");
    notices.push_back(s);
}
// Who is told, and how, is diplo_treaty_break_tell's (tests/treaty_tell_test.cpp).
static std::vector<std::string> tells;      // ai, other, peace or cease-fire
static void diplo_treaty_break_tell(int ai, int other, int was_peace) {
    char s[64];
    std::snprintf(s, sizeof s, "%d %d %s", ai, other, was_peace ? "peace" : "ceasefire");
    tells.push_back(s);
}

#include "production.inc"

enum { HUMAN = 1, AI = 2 };

// Deity.  Civ 1, a human, launches, arriving in 2100; civ 2, an AI, launched
// and arrives in 2110.  `treaty` both ways, with contact.
static void setup(uint treaty) {
    memset(&game, 0, sizeof game);
    memset(civs, 0, sizeof civs);
    game.difficulty = DIFF_DEITY;
    game.first_launch_turn = -1;
    game.human_players = 1 << HUMAN;
    human_civ[0] = HUMAN;
    civs[HUMAN].spaceship_arrival_year = 2100;
    civs[AI].spaceship_flags = SHIP_LAUNCHED;
    civs[AI].spaceship_arrival_year = 2110;
    civs[HUMAN].treaties[AI] = civs[AI].treaties[HUMAN] = TREATY_CONTACT | treaty;
    notices.clear();
    tells.clear();
}

int main() {
    // The original: the AI's side drops peace, the player's still has it, no message.
    plat_fix_bugs = 0;
    setup(TREATY_PEACE);
    spaceship_launch(HUMAN);
    assert((civs[AI].treaties[HUMAN] & TREATY_PEACE) == 0);
    assert(civs[AI].treaties[HUMAN] & TREATY_BROKEN);
    assert(civs[HUMAN].treaties[AI] & TREATY_PEACE);
    assert(notices.empty() && tells.empty());

    // Fixed: both sides, and diplo_treaty_break_tell tells whom it should.
    plat_fix_bugs = 1;
    setup(TREATY_PEACE);
    spaceship_launch(HUMAN);
    assert((civs[AI].treaties[HUMAN] & TREATY_PEACE) == 0);
    assert(civs[AI].treaties[HUMAN] & TREATY_BROKEN);
    std::printf("peace: player's side %#x, tell %s\n", civs[HUMAN].treaties[AI],
                tells.empty() ? "none" : tells[0].c_str());
    assert((civs[HUMAN].treaties[AI] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0);
    assert(civs[HUMAN].treaties[AI] & TREATY_CONTACT);
    assert(notices.empty());
    assert(tells.size() == 1 && tells[0] == "2 1 peace");

    // Fixed, a cease-fire.
    setup(TREATY_CEASEFIRE);
    spaceship_launch(HUMAN);
    assert((civs[HUMAN].treaties[AI] & TREATY_CEASEFIRE) == 0);
    assert(tells.size() == 1 && tells[0] == "2 1 ceasefire");

    // Fixed, no treaty: nothing to tell.
    setup(0);
    spaceship_launch(HUMAN);
    assert(tells.empty());
    return 0;
}
