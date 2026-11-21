// SOURCE src/auto/diplomacy.cpp diplo_attitude_level
// SOURCE src/game/extras.cpp treaty_notice_draw_emblems
// SOURCE src/game/extras.cpp diplo_treaty_break_notice
// B37: the AI's emissary tells the player that it has ended their peace
// treaty or cease-fire: one popup of the game's usual width, titled as the
// game's emissary messages are ("<attitude> <adjective> Emissary"), with the
// emissary's words (OpenCiv2's own text, src/host/texts.h), a blank line,
// "You reply:" and our reply as quoted text: GAME.TXT's @CANCELOURPEACE for a
// peace treaty, @NOPEACE for a cease-fire (their lines joined, so a mod's or
// translation's apply).  Left of the text, the AI's emblem at the top and
// ours beside "You reply:": one placeholder picture as tall as both, drawn
// by a callback that places each emblem where the text puts it (measured
// with the dialog's own wrapping, draw_wrapped_text).
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

// Pictures: an emblem is 64 x 64; a solid one as created; draws recorded.
static std::map<Sprite *, std::pair<int, int>> sizes;
static std::vector<std::string> draws;          // "emblem <civ> at x,y"
static Sprite *sprite_init(Sprite *s) { sizes.erase(s); return s; }
static Sprite *sprite_destroy(Sprite *s) { sizes.erase(s); return s; }
i32 Sprite::create_solid(i32 w, i32 h, uint) { sizes[this] = {w, h}; return 1; }
void Sprite::free() { sizes.erase(this); }
i32 Sprite::width() { return sizes.count(this) ? sizes[this].first : 64; }
i32 Sprite::height() { return sizes.count(this) ? sizes[this].second : 64; }
RECT *Sprite::draw(RECT *dest, DibSurface *, i32 x, i32 y) {
    char s[64];
    int civ = (int)(this - road_sprites_block.civ_portrait_sprites);
    std::snprintf(s, sizeof s, "emblem %d at %d,%d", civ, x, y);
    draws.push_back(s);
    return dest;
}

// The dialog: its text measured as 16 pixels a line (each line added one).
static std::string shown_title;
static std::vector<std::string> shown_lines;
static int shown_options, shown;
static Sprite *shown_sprite;
static DialogSpriteDrawFn shown_draw;
static int placeholder_height;
Dialog::Dialog(i32) {
    std::memset((void *)this, 0, sizeof *this);
    section_gap = 4;
}
Dialog::~Dialog() {}
void Dialog::init(SurfaceWindow *, DibSurface *, RECT *, u32) {}
void Dialog::set_width(u32 w) { content_width = w; }
void Dialog::set_title(const char *title) { shown_title = title; }
DialogLine *Dialog::add_line(const char *text, int, int, int, int) {
    shown_lines.push_back(text);
    return 0;
}
DialogOption *Dialog::add_option(const char *, int, uint) {
    shown_options++;
    return 0;
}
DialogSprite *Dialog::add_sprite(Sprite *sprite, int, const char *) {
    shown_sprite = sprite;
    return 0;
}
void Dialog::set_sprite_draw_fn(DialogSpriteDrawFn fn) { shown_draw = fn; }
int Dialog::draw_wrapped_text(int) { return 16 * (int)shown_lines.size(); }
i32 Dialog::show_modal(int) {
    shown++;
    placeholder_height = shown_sprite ? shown_sprite->height() : 0;
    if (shown_draw && shown_sprite)      // as Dialog::render draws a picture at its place
        shown_draw((i32)(iptr)shown_sprite, 0, 0, 0, 10, 20);
    return 0;
}

#include "production.inc"

enum { HUMAN = 1, AI = 2 };

static void reset(int attitude) {
    memset(civs, 0, sizeof civs);
    civs[AI].attitude[HUMAN] = (u8)attitude;
    shown_title.clear();
    shown_lines.clear();
    draws.clear();
    shown_options = shown = 0;
    shown_sprite = 0;
    shown_draw = 0;
    placeholder_height = 0;
}

static std::string lines() {
    std::string s;
    for (auto &l : shown_lines)
        s += "[" + l + "]";
    return s;
}

int main() {
    // A peace treaty, the French enraged (attitude 100).
    reset(100);
    diplo_treaty_break_notice(AI, HUMAN, 1);
    std::printf("title: %s\nlines: %s\ndraws: %s, %s; placeholder %d high\n", shown_title.c_str(), lines().c_str(),
                draws.size() > 0 ? draws[0].c_str() : "-", draws.size() > 1 ? draws[1].c_str() : "-",
                placeholder_height);
    assert(shown == 1);
    assert(shown_title == "Enraged French Emissary");
    assert(shown_options == 0);         // no choice: the dialog keeps its width
    assert(shown_lines.size() == 4);
    assert(shown_lines[0] == "\"Hear now the words of the French: we no longer consider ourselves bound by our "
                              "peace treaty with you.\"");
    assert(shown_lines[1] == "^");
    assert(shown_lines[2] == "^You reply:");
    assert(shown_lines[3] == "\"Then consider our peace treaty null and void.  But we shall long remember "
                              "this treachery!\"");
    // The emblems: the French at the top, ours beside "You reply:" (2 lines
    // down), the placeholder as tall as both (32 + 64) or the text if taller.
    assert(draws.size() == 2);
    assert(draws[0] == "emblem 2 at 10,20");
    assert(draws[1] == "emblem 1 at 10,52");
    assert(placeholder_height == 96);

    // A cease-fire, the French receptive (attitude 30).
    reset(30);
    diplo_treaty_break_notice(AI, HUMAN, 0);
    assert(shown == 1);
    assert(shown_title == "Receptive French Emissary");
    assert(shown_lines.size() == 4);
    assert(shown_lines[0] == "\"Hear now the words of the French: we no longer consider ourselves bound by our "
                              "cease fire with you.\"");
    assert(shown_lines[3] == "\"So be it.  We warn you, however, that our forces remain vigilant and we will "
                              "brook no aggression.\"");

    // No such section in GAME.TXT (a mod's): the emissary's words, no reply,
    // and only the AI's emblem.
    game_txt.erase("NOPEACE");
    reset(30);
    diplo_treaty_break_notice(AI, HUMAN, 0);
    assert(shown == 1 && shown_lines.size() == 1);
    assert(draws.size() == 1 && draws[0] == "emblem 2 at 10,20");

    // German: the attitude takes "er" (as diplo_set_attitude_arg does).
    game_language = 2;
    reset(100);
    diplo_treaty_break_notice(AI, HUMAN, 1);
    assert(shown_title == "Enrageder French Emissary");
    return 0;
}
