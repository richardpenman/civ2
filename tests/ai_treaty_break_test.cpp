// SOURCE src/auto/diplomacy.cpp diplo_clear_treaty_flags
// SOURCE src/auto/civ.cpp civ_turn_update_diplomacy
// B37: an AI that means to break its treaty with a civ (TREATY_HOSTILE_INTENT,
// set when the AIs gang up on the leader) drops cease-fire and peace at the
// start of its turn.  The original drops them on its own side only and says
// nothing: the player's side still says peace.  Fixed: both sides, and the
// player is told when a peace treaty ends (CANCELPEACE).
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
static const char *names[8] = {"Barbarians", "Romans", "Greeks", "", "", "", "", ""};
static char *civ_name_plural(int civ) { return (char *)names[civ]; }
static std::string args[2];
static void text_set_string_arg(int n, const char *s) { args[n] = s; }
static std::vector<std::string> popups;     // key: STRING0, STRING1
static void popup_game_with_sprite(const char *key, Sprite *, int) {
    popups.push_back(std::string(key) + ": " + args[0] + ", " + args[1]);
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
}

int main() {
    // The original: the AI's side drops peace, the player's still has it, no message.
    plat_fix_bugs = 0;
    setup(TREATY_PEACE);
    civ_turn_update_diplomacy(AI);
    assert((civs[AI].treaties[HUMAN] & (TREATY_PEACE | TREATY_HOSTILE_INTENT)) == 0);
    assert(civs[AI].treaties[HUMAN] & TREATY_BROKEN);
    assert(civs[HUMAN].treaties[AI] & TREATY_PEACE);
    assert(popups.empty());

    // Fixed: both sides drop peace and the player is told.
    plat_fix_bugs = 1;
    setup(TREATY_PEACE);
    civ_turn_update_diplomacy(AI);
    assert((civs[AI].treaties[HUMAN] & (TREATY_PEACE | TREATY_HOSTILE_INTENT)) == 0);
    assert(civs[AI].treaties[HUMAN] & TREATY_BROKEN);
    assert((civs[HUMAN].treaties[AI] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0);
    assert(civs[HUMAN].treaties[AI] & TREATY_CONTACT);
    assert(popups.size() == 1 && popups[0] == "CANCELPEACE: Greeks, Romans");

    // Fixed, a cease-fire only: dropped on both sides, no message (it speaks of peace).
    setup(TREATY_CEASEFIRE);
    civ_turn_update_diplomacy(AI);
    assert((civs[AI].treaties[HUMAN] & TREATY_CEASEFIRE) == 0);
    assert((civs[HUMAN].treaties[AI] & TREATY_CEASEFIRE) == 0);
    assert(popups.empty());

    // Fixed, in a network game: the original (every player must compute the same game).
    setup(TREATY_PEACE);
    game.multiplayer = 3;
    civ_turn_update_diplomacy(AI);
    assert(civs[HUMAN].treaties[AI] & TREATY_PEACE);
    return 0;
}
