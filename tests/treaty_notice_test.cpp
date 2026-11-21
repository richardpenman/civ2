// SOURCE src/auto/diplomacy.cpp diplo_attitude_level
// SOURCE src/game/extras.cpp diplo_treaty_break_notice
// B37: the AI's emissary tells the player that it has ended their peace
// treaty or cease-fire: one popup, titled as the game's emissary messages are
// ("<attitude> <adjective> Emissary"), with the AI's civ emblem in its colour,
// the emissary's words (OpenCiv2's own text, src/host/texts.h) and our reply
// as the one choice: GAME.TXT's @CANCELOURPEACE for a peace treaty, @NOPEACE
// for a cease-fire (their lines joined, so a mod's or translation's apply).
#include <cassert>
#include <cstdio>
#include <cstring>
#include <map>
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

static Civ civs[8];
static int game_language;
static char str_er[4] = "er";
static char str_game_7[8] = "GAME";
static struct {
    Sprite civ_portrait_sprites[8];
} road_sprites_block;

// RULES.TXT @ATTITUDES, as pooled strings (attitude_names holds their ids).
static const char *attitudes[9] = {"Worshipful", "Enthusiastic", "Cordial", "Receptive", "Neutral",
                                   "Uncooperative", "Icy", "Hostile", "Enraged"};
static i32 attitude_names[9] = {100, 101, 102, 103, 104, 105, 106, 107, 108};
static char *label_string(int id) { return (char *)attitudes[id - 100]; }
static const char *adjectives[8] = {"Barbarian", "Spanish", "French", "", "", "", "", ""};
static const char *plurals[8] = {"Barbarians", "Spanish", "French", "", "", "", "", ""};
static i32 civ_get_adjective(int civ) { return (i32)(iptr)adjectives[civ]; }
static char *civ_name_plural(int civ) { return (char *)plurals[civ]; }

// OpenCiv2's texts, as the game has them.
static const char *extras_text(const char *key) {
    static const struct { const char *key, *english; } texts[] = {
#define T(key, english) {key, english},
#include "../src/host/texts.h"
#undef T
    };
    for (auto &t : texts)
        if (!strcmp(t.key, key))
            return t.english;
    return key;
}

// GAME.TXT's sections, as the game's text reader gives them.
static std::map<std::string, std::vector<std::string>> game_txt = {
    {"CANCELOURPEACE", {"@width=320", "@title=%STRING0 Emissary",
                        "\"Then consider our peace treaty null and void.  But",
                        "we shall long remember this treachery!\"", "", "@NEXT"}},
    {"NOPEACE", {"@width=320", "@title=%STRING0 Emissary",
                 "\"So be it.  We warn you, however, that our",
                 "forces remain vigilant and we will brook no", "aggression.\"", "", "@NEXT"}},
};
static std::vector<std::string> *section;
static size_t section_line;
static char line_buf[256];
static int text_open_section(const char *file, const char *name) {
    auto it = game_txt.find(name);
    if (strcmp(file, "GAME") || it == game_txt.end())
        return 1;
    section = &it->second;
    section_line = 0;
    return 0;
}
static i8 *text_read_line(int) {
    if (!section || section_line >= section->size())
        return 0;
    snprintf(line_buf, sizeof line_buf, "%s", (*section)[section_line++].c_str());
    return line_buf;
}
static void text_close_file(void) { section = 0; }

// The dialog, as shown.
static std::string shown_title, shown_body, shown_reply;
static Sprite *shown_sprite;
static int shown;
Dialog::Dialog(i32) {}
Dialog::~Dialog() {}
void Dialog::init(SurfaceWindow *, DibSurface *, RECT *, u32) {}
void Dialog::set_width(u32) {}
void Dialog::set_title(const char *title) { shown_title = title; }
DialogLine *Dialog::add_line(const char *text, int, int, int, int) {
    shown_body += text;
    return 0;
}
DialogOption *Dialog::add_option(const char *text, int, uint) {
    shown_reply = text;
    return 0;
}
DialogSprite *Dialog::add_sprite(Sprite *sprite, int, const char *) {
    shown_sprite = sprite;
    return 0;
}
i32 Dialog::show_modal(int) {
    shown++;
    return 0;
}

#include "production.inc"

enum { HUMAN = 1, AI = 2 };

static void reset(int attitude) {
    memset(civs, 0, sizeof civs);
    civs[AI].attitude[HUMAN] = (u8)attitude;
    shown_title.clear();
    shown_body.clear();
    shown_reply.clear();
    shown_sprite = 0;
    shown = 0;
}

int main() {
    // A peace treaty, the French enraged (attitude 100).
    reset(100);
    diplo_treaty_break_notice(AI, HUMAN, 1);
    std::printf("title: %s\nbody: %s\nreply: %s\n", shown_title.c_str(), shown_body.c_str(), shown_reply.c_str());
    assert(shown == 1);
    assert(shown_title == "Enraged French Emissary");
    assert(shown_body == "\"Hear now the words of the French: we no longer consider ourselves bound by our "
                         "peace treaty with you.\"");
    assert(shown_reply == "\"Then consider our peace treaty null and void.  But we shall long remember "
                          "this treachery!\"");
    assert(shown_sprite == &road_sprites_block.civ_portrait_sprites[AI]);

    // A cease-fire, the French receptive (attitude 30).
    reset(30);
    diplo_treaty_break_notice(AI, HUMAN, 0);
    assert(shown == 1);
    assert(shown_title == "Receptive French Emissary");
    assert(shown_body == "\"Hear now the words of the French: we no longer consider ourselves bound by our "
                         "cease fire with you.\"");
    assert(shown_reply == "\"So be it.  We warn you, however, that our forces remain vigilant and we will "
                          "brook no aggression.\"");

    // No such section in GAME.TXT (a mod's): the emissary's words, no reply.
    game_txt.erase("NOPEACE");
    reset(30);
    diplo_treaty_break_notice(AI, HUMAN, 0);
    assert(shown == 1 && shown_reply.empty());

    // German: the attitude takes "er" (as diplo_set_attitude_arg does).
    game_language = 2;
    reset(100);
    diplo_treaty_break_notice(AI, HUMAN, 1);
    assert(shown_title == "Enrageder French Emissary");
    return 0;
}
