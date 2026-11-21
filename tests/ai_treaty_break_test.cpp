// SOURCE src/auto/diplomacy.cpp diplo_clear_treaty_flags
// SOURCE src/auto/civ.cpp civ_turn_update_diplomacy
// B37: an AI that means to break its treaty with a civ (TREATY_HOSTILE_INTENT,
// set after a meeting where it decided on war, or when the AIs gang up on the
// leader) drops cease-fire and peace at the start of its turn.  The original
// drops them on its own side only and says nothing: the player's side still
// says peace.  Fixed: both sides, and the AI's emissary tells the player
// (diplo_treaty_break_notice), for a peace treaty and for a cease-fire.
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
static iptr human_civ[2];
static struct {
    int active;
} autoplay;
static struct {
    i32 diplo_recent_peace_table[8];
} setup_picwnd_block;
static struct {
    struct {
        int id;
    } net_slots[8];
    int net_civ_slots[8];
    void process_messages(int, int) {}
} net;
static struct {
    u8 war_icon_sprite[64];
} war_icon_sprite_block;
static u8 ceasefire_icon_sprite[64];
static char str_warends_2[8] = "WARENDS";
static char str_cancelpeace_3[12] = "CANCELPEACE";

static int plat_fix_bugs;
static inline int bugfix_state(void) { return plat_fix_bugs && game.multiplayer < 3; }

static int _rand() { return 0; }       // every roll hits
static int int_clamp(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }
static int civ_has_wonder(int, int) { return 0; }
static void civ_change_government(int, int) {}
static void civ_adjust_attitude(int, int, int) {}
static const char *names[8] = {"Barbarians", "Romans", "Greeks", "Persians", "", "", "", ""};
static char *civ_name_plural(int civ) { return (char *)names[civ]; }
static std::string args[2];
static void text_set_string_arg(int n, const char *s) { args[n] = s; }
static std::vector<std::string> popups;     // key: STRING0, STRING1
static void popup_game_with_sprite(const char *key, Sprite *, int) {
    popups.push_back(std::string(key) + ": " + args[0] + ", " + args[1]);
}
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
static void diplo_civs_meet(int, int, int, int) {}
static void diplo_ai_swap_advances(int, int) {}
static void diplo_ai_review_relations(int, int) {}
static void diplo_ai_military_aid(int) {}
static void diplo_ai_seek_war_ally(int, int) {}
static void net_send_text_args(int, int, int, int, int, int) { assert(!"network"); }
static int net_send_message(int, int, int, int, int, int, int, int, const char *, void *) {
    assert(!"network");
    return 0;
}

#include "production.inc"

enum { HUMAN = 1, AI = 2 };

// Civ 1 a human, civ 2 an AI with contact, embassies and `treaty` both ways;
// the AI means to break it.  Turn 1: the 16- and 32-turn checks don't run.
static void setup(uint treaty) {
    memset(&game, 0, sizeof game);
    memset(civs, 0, sizeof civs);
    game.turn = 1;
    game.civs_alive = 1 << HUMAN | 1 << AI;
    game.human_players = 1 << HUMAN;
    human_civ[0] = HUMAN;
    civs[HUMAN].government = civs[AI].government = GOV_MONARCHY;
    civs[HUMAN].treaties[AI] = civs[AI].treaties[HUMAN] = TREATY_CONTACT | TREATY_EMBASSY | treaty;
    civs[AI].treaties[HUMAN] |= TREATY_HOSTILE_INTENT;
    popups.clear();
    notices.clear();
    tells.clear();
}

int main() {
    // The original: the AI's side drops peace, the player's still has it, no message.
    plat_fix_bugs = 0;
    setup(TREATY_PEACE);
    civ_turn_update_diplomacy(AI);
    assert((civs[AI].treaties[HUMAN] & (TREATY_PEACE | TREATY_HOSTILE_INTENT)) == 0);
    assert(civs[AI].treaties[HUMAN] & TREATY_BROKEN);
    assert(civs[HUMAN].treaties[AI] & TREATY_PEACE);
    assert(popups.empty() && notices.empty() && tells.empty());

    // Fixed: both sides drop peace, and diplo_treaty_break_tell tells whom it
    // should (the emissary's notice, then the Diplomats Report).
    plat_fix_bugs = 1;
    setup(TREATY_PEACE);
    civ_turn_update_diplomacy(AI);
    assert((civs[AI].treaties[HUMAN] & (TREATY_PEACE | TREATY_HOSTILE_INTENT)) == 0);
    assert(civs[AI].treaties[HUMAN] & TREATY_BROKEN);
    assert((civs[HUMAN].treaties[AI] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0);
    assert(civs[HUMAN].treaties[AI] & TREATY_CONTACT);
    std::printf("peace: tell %s\n", tells.empty() ? "none" : tells[0].c_str());
    assert(popups.empty() && notices.empty());
    assert(tells.size() == 1 && tells[0] == "2 1 peace");

    // Fixed, a cease-fire: dropped on both sides, and told too.
    setup(TREATY_CEASEFIRE);
    civ_turn_update_diplomacy(AI);
    assert((civs[AI].treaties[HUMAN] & TREATY_CEASEFIRE) == 0);
    assert((civs[HUMAN].treaties[AI] & TREATY_CEASEFIRE) == 0);
    assert(tells.size() == 1 && tells[0] == "2 1 ceasefire");

    // Fixed, one AI dropping its peace with another (civ 3): told as well
    // (a third party may see the Diplomats Report).
    setup(0);
    game.civs_alive |= 1 << 3;
    civs[3].government = GOV_MONARCHY;
    civs[AI].treaties[3] = civs[3].treaties[AI] = TREATY_CONTACT | TREATY_PEACE | TREATY_EMBASSY;
    civs[AI].treaties[3] |= TREATY_HOSTILE_INTENT;
    civs[AI].treaties[HUMAN] &= ~TREATY_HOSTILE_INTENT;
    civ_turn_update_diplomacy(AI);
    assert((civs[3].treaties[AI] & TREATY_PEACE) == 0);
    assert(tells.size() == 1 && tells[0] == "2 3 peace");

    // Fixed, in a network game: the original (every player must compute the same game).
    setup(TREATY_PEACE);
    game.multiplayer = 3;
    civ_turn_update_diplomacy(AI);
    assert(civs[HUMAN].treaties[AI] & TREATY_PEACE);
    assert(notices.empty() && tells.empty());
    return 0;
}
