// SOURCE src/auto/text.cpp str_copy_max
// SOURCE src/auto/text.cpp str_append_max
// SOURCE src/auto/window_dialog.cpp Dialog::draw_wrapped_text
// The game's dialogs' word wrapping (Dialog::draw_wrapped_text) in UTF-8 text
// (a translation's "@@codepage utf-8": Chinese; src/include/utf8.h).  Text
// right of a picture (an advisor's portrait) starts sprite_gap further right
// than compute_layout widens the window for it, so a line wrapped to the
// whole content_width ran into the window's border; Chinese, which breaks
// next to any character, fills a line to the pixel.  In UTF-8 text a line is
// as wide as the window has room for; in the game's code page the original's
// width, unchanged.
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
#include "utf8.h"
static_assert(sizeof(void *) == 4, "run with a 32-bit compiler");

extern "C" int plat_text_utf8;
int plat_text_utf8 = 1;
static i8 dialog_wrap_space_str[4] = {0x20};
#define __cdecl
#define _strlen strlen
#define _strchr strchr
#define crt_strcat strcat
#define crt_memcpy memcpy
#define crt_strcpy(d, s) memmove(d, s, strlen(s) + 1)

// The font: an ASCII character 8 pixels wide, a wide (Chinese) one 16.
i32 GameFont::line_spacing() { return 16; }
i32 GameFont::text_width(const char *s) {
    int w = 0, n = (int)strlen(s);
    for (int i = 0; i < n;) {
        int len;
        unsigned c = u8_decode(s + i, n - i, &len);
        if (c != U8_BAD) w += u8_wide(c) ? 16 : 8;
        i += len;
    }
    return w;
}
static std::vector<std::string> drawn;
void Dialog::draw_text_line(const char *text, int, i32) { drawn.push_back(text); }
void Dialog::draw_text_default_font(const char *, i32, i32, i32) {}

#include "production.inc"

// An advisor's popup: a portrait, 225 pixels of text right of it (the window
// has room for 221: sprite_gap 4 less), shadowed text.
static std::vector<std::string> wrap(const char *text, int with_sprite) {
    alignas(Dialog) static char mem[sizeof(Dialog)];
    static GameFont font;
    static DialogSprite sprite;
    static DialogLine line;
    static char buf[512];
    Dialog *d = (Dialog *)mem;
    std::memset(mem, 0, sizeof mem);
    std::memset(&sprite, 0, sizeof sprite);
    std::memset(&line, 0, sizeof line);
    std::strcpy(buf, text);
    line.text = buf;
    line.y = -1;
    d->body_font = (uptr)&font;
    d->text_color = 1;
    d->shadow_color = 2;
    d->content_width = 225;
    d->sprite_gap = 4;
    d->sprite_list = with_sprite ? &sprite : 0;
    d->line_list = &line;
    drawn.clear();
    d->draw_wrapped_text(1);
    return drawn;
}

int main() {
    std::string zh;                       // 15 Chinese characters (一 repeated)
    for (int i = 0; i < 15; i++) zh += "\xe4\xb8\x80";
    // beside a portrait: 13 a line (208 + 1 of shadow fits 221; 14, 225, did
    // not fit the window)
    std::vector<std::string> lines = wrap(zh.c_str(), 1);
    assert(lines.size() == 2 && lines[0].size() == 13 * 3 && lines[1].size() == 2 * 3);
    // no portrait: the whole content width, 14 a line
    lines = wrap(zh.c_str(), 0);
    assert(lines.size() == 2 && lines[0].size() == 14 * 3 && lines[1].size() == 3);

    // the game's code page: the original's width beside a portrait too
    // ("aaaaaaa" seven of 56: four words and three spaces, 4*56+3*8 = 248 >
    // 225: three a line, 3*56+2*8+1 = 185)
    plat_text_utf8 = 0;
    lines = wrap("aaaaaaa aaaaaaa aaaaaaa aaaa aaaaaaa", 1);
    // 3 words (184+1) and "aaaa" (+40 = 225: fits content_width exactly, as
    // in the original, though it reaches the border by sprite_gap)
    assert(lines.size() == 2 && lines[0] == "aaaaaaa aaaaaaa aaaaaaa aaaa" && lines[1] == "aaaaaaa");
    plat_text_utf8 = 1;
    lines = wrap("aaaaaaa aaaaaaa aaaaaaa aaaa aaaaaaa", 1);
    assert(lines.size() == 2 && lines[0] == "aaaaaaa aaaaaaa aaaaaaa" && lines[1] == "aaaa aaaaaaa");
    std::printf("dialog wrapping beside a picture: UTF-8 within the window, the game's code page as the original\n");
    return 0;
}
