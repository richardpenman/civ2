// SOURCE src/game/extras.cpp diplo_treaty_break_tell
// B37: who is told when an AI ends its peace treaty or cease-fire with
// another civ, and how, as the game tells of other treaties broken:
// - the other civ is the player at the screen: the AI's emissary first
//   (diplo_treaty_break_notice), then, for a peace treaty, the Diplomats
//   Report CANCELPEACE, as an attack that breaks one shows them
//   (unit_combat.cpp: SNEAK, then CANCELPEACE; a cease-fire: no report);
// - a third party at the screen: the Diplomats Report for a peace treaty,
//   with an embassy with either civ, the United Nations, Marco Polo's or the
//   map revealed, as when two AIs cancel peace (diplomacy.cpp,
//   diplo_civs_meet_war);
// - else (autoplay, no player at the screen: hot seat in an AI's turn): no one.
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
    u8 war_icon_sprite[64];
} war_icon_sprite_block;
static char str_cancelpeace_3[12] = "CANCELPEACE";

static int wonder_of_player;      // the player's wonder, if any
static int civ_has_wonder(int civ, int wonder) { return civ == human_civ[0] && wonder == wonder_of_player; }
static const char *names[8] = {"Barbarians", "Spanish", "French", "Persians", "", "", "", ""};
static char *civ_name_plural(int civ) { return (char *)names[civ]; }

// What is shown, in order.
static std::vector<std::string> shown;
static std::string args[2];
static void text_set_string_arg(int n, const char *s) { args[n] = s; }
static void popup_game_with_sprite(const char *key, Sprite *, int) {
    shown.push_back(std::string(key) + ": " + args[0] + ", " + args[1]);
}
static void diplo_treaty_break_notice(int ai, int human, int was_peace) {
    char s[64];
    std::snprintf(s, sizeof s, "notice %d %d %s", ai, human, was_peace ? "peace" : "ceasefire");
    shown.push_back(s);
}

#include "production.inc"

enum { PLAYER = 1, FRANCE = 2, PERSIA = 3 };

static void reset() {
    memset(&game, 0, sizeof game);
    memset(civs, 0, sizeof civs);
    human_civ[0] = PLAYER;
    autoplay.active = 0;
    wonder_of_player = -1;
    shown.clear();
}

static std::string all() {
    std::string s;
    for (auto &x : shown)
        s += "[" + x + "]";
    return s;
}

int main() {
    // France breaks its peace treaty with the player: the emissary, then the report.
    reset();
    diplo_treaty_break_tell(FRANCE, PLAYER, 1);
    std::printf("player, peace: %s\n", all().c_str());
    assert(shown.size() == 2);
    assert(shown[0] == "notice 2 1 peace");
    assert(shown[1] == "CANCELPEACE: French, Spanish");

    // A cease-fire: the emissary only.
    reset();
    diplo_treaty_break_tell(FRANCE, PLAYER, 0);
    assert(shown.size() == 1 && shown[0] == "notice 2 1 ceasefire");

    // France breaks its peace with Persia; the player has an embassy with France.
    reset();
    civs[PLAYER].treaties[FRANCE] = TREATY_CONTACT | TREATY_EMBASSY;
    diplo_treaty_break_tell(FRANCE, PERSIA, 1);
    std::printf("third party, embassy: %s\n", all().c_str());
    assert(shown.size() == 1 && shown[0] == "CANCELPEACE: French, Persians");

    // ... with Persia only: the same.
    reset();
    civs[PLAYER].treaties[PERSIA] = TREATY_CONTACT | TREATY_EMBASSY;
    diplo_treaty_break_tell(FRANCE, PERSIA, 1);
    assert(shown.size() == 1 && shown[0] == "CANCELPEACE: French, Persians");

    // ... no embassy, but the United Nations, or Marco Polo's, or the map revealed.
    reset();
    wonder_of_player = WONDER_UNITED_NATIONS;
    diplo_treaty_break_tell(FRANCE, PERSIA, 1);
    assert(shown.size() == 1);
    reset();
    wonder_of_player = WONDER_MARCO_POLO;
    diplo_treaty_break_tell(FRANCE, PERSIA, 1);
    assert(shown.size() == 1);
    reset();
    game.reveal_map = 1;
    diplo_treaty_break_tell(FRANCE, PERSIA, 1);
    assert(shown.size() == 1);

    // ... none of those: nothing.
    reset();
    diplo_treaty_break_tell(FRANCE, PERSIA, 1);
    assert(shown.empty());

    // ... a cease-fire: no report, even with an embassy.
    reset();
    civs[PLAYER].treaties[FRANCE] = TREATY_CONTACT | TREATY_EMBASSY;
    diplo_treaty_break_tell(FRANCE, PERSIA, 0);
    assert(shown.empty());

    // Autoplay: nothing.
    reset();
    autoplay.active = 1;
    diplo_treaty_break_tell(FRANCE, PLAYER, 1);
    assert(shown.empty());

    // No player at the screen (hot seat, an AI's turn): nothing.
    reset();
    human_civ[0] = -1;
    diplo_treaty_break_tell(FRANCE, PLAYER, 1);
    diplo_treaty_break_tell(FRANCE, PERSIA, 1);
    assert(shown.empty());
    return 0;
}
