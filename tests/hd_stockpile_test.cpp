// SOURCE src/game/hd_windows.cpp stockpile_label
// SOURCE src/game/hd_windows.cpp hd_stockpile
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
using iptr = intptr_t;
struct RECT { int left, top, right, bottom; };
struct GameFont {
    int size;
    void init_empty() {}
    void set(int, int n) { size = n; }
    int text_width(const char *s) { return (int)strlen(s) * size * 3 / 5; }
    int font_cached_height() { return size; }
    void destroy() {}
};
struct Caption { std::string text; RECT rect; };
static std::vector<Caption> captions;
static std::vector<RECT> boxes;
struct DibSurface {
    void draw_text_in_rect(iptr font, const char *text, RECT *rect, int) {
        auto &f = *(GameFont *)font;
        assert(f.text_width(text) <= rect->right - rect->left);
        assert(f.font_cached_height() <= rect->bottom - rect->top);
        captions.push_back({text, *rect});
    }
};
static void gfx_set_text_color_index(int) {}
static void art(DibSurface *, const char *, int, int, int, int, bool) {}
static void box(DibSurface *, int x, int y, int w, int h, int) { boxes.push_back({x,y,x+w,y+h}); }
static int int_clamp(int x, int lo, int hi) { return x < lo ? lo : x > hi ? hi : x; }
static const char *extras_text(const char *key) {
    if (!strcmp(key,"stockpile.turn")) return "turn";
    if (!strcmp(key,"stockpile.turns")) return "turns";
    if (!strcmp(key,"stockpile.full")) return "Full";
    if (!strcmp(key,"stockpile.no_surplus")) return "No surplus";
    if (!strcmp(key,"stockpile.deficit")) return "Food deficit";
    assert(!strcmp(key,"stockpile.no_production"));
    return "No production";
}
#include "production.inc"
int main() {
    const struct { int value, capacity, surplus; const char *role, *expected; } cases[] = {
        {40,120,6,"food","14 turns"}, {42,120,6,"food","13 turns"},
        {118,120,6,"food","1 turn"}, {120,120,6,"food","Full"},
        {121,120,-1,"food","Full"}, {40,120,0,"food","No surplus"},
        {40,120,-2,"food","Food deficit"}, {3,40,5,"production","8 turns"},
        {35,40,5,"production","1 turn"}, {40,40,0,"production","1 turn"},
        {45,40,-2,"production","1 turn"}, {10,40,0,"production","No production"},
        {10,40,-2,"production","No production"}, {1,32760,1,"production","32759 turns"},
        {32767,32760,1,"production","1 turn"}
    };
    DibSurface surface;
    for (int scale = 1; scale <= 3; scale++) for (const auto &c : cases) for (int granary : {0,1}) {
        RECT r = {17,23,17+(183*scale+1)/2,23+(146*scale+1)/2};
        captions.clear();boxes.clear();
        hd_stockpile(&surface,r,c.value,c.capacity,granary ? c.capacity/2 : 0,c.surplus,c.role);
        assert(captions.size() == 2 && captions[1].text == c.expected);
        const RECT &amount = captions[0].rect, &turns = captions[1].rect;
        assert(amount.bottom <= boxes[0].top && turns.top >= boxes[0].bottom + 2);
        assert(turns.left >= r.left && turns.right <= r.right && turns.bottom <= r.bottom);
        assert(boxes.size() == (unsigned)(2+granary));
    }
    puts("HD stockpiles: rounding, full boxes, stalled output, granaries and all city sizes passed.");
}
