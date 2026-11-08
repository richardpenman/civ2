// SOURCE src/auto/graphics.cpp gfx_draw_city
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#ifndef __cdecl
#define __cdecl
#endif
using iptr = intptr_t;
using i8 = char;
using i32 = int32_t;
using uint = unsigned;
struct RECT { int left, top, right, bottom; };
enum { CITYF_DISORDER = 1 };
static int hd_enabled = 1, camera, status_text = 1, silhouettes, ordinary_disorder, resets;
static char label[8];
static RECT badge;
static struct { int num_cities = 1; } game;
static struct { int map_redraw_busy; } net;
static struct { int x, y, owner = 1, flags, size = 7; } cities[1];
struct DibSurface { void replace_color(iptr, int, int) {} };
static struct { DibSurface window; } advisor_state;
static DibSurface map_surface;
struct Sprite {
    void draw(iptr, iptr, int, int);
    void draw_silhouette(iptr, iptr, int, int, int) { silhouettes++; }
};
static struct { char city_sprites[0x1e0 * 6]; } gfx_sheet_surface_block;
static struct { char civ_flag_sprites[0x78 * 8]; } editor_tile_backdrop_sprite_block;
static struct { Sprite city_disorder_icon_sprite; } fortify_overlay_sprites_block;
void Sprite::draw(iptr, iptr, int, int) {
    if (this == &fortify_overlay_sprites_block.city_disorder_icon_sprite) ordinary_disorder++;
}
static struct { int city_flag_x_offsets[48], city_flag_y_offsets[48]; } peace_treaty_icon_sprite_block;
static struct { int city_label_y_offsets[48] = {17}, city_size_box_x_offsets[48] = {5}; } road_sprites_block;
struct GameFont {
    int text_width(const char *s) { return (int)strlen(s) * 8; }
    int font_cached_height() { return 12; }
};
static struct { GameFont map_label_font; } status_window_block;
static char city_size_digit_str[] = "0", city_size_digit2_str[] = "0";
static int map_camera_owns(void *) { return 0; }
static void map_view_cache_invalidate(void *, int, int, int) {}
static int map_camera_surface(void *s) { return camera && s == &map_surface; }
static int gfx_show_status_text(iptr, int) { return status_text; }
static void gfx_set_zoom_scale(int) {}
static void gfx_reset_zoom_scale() { resets++; }
static int gfx_zoom_scale(int n, int) { return n; }
static void gfx_set_draw_target(iptr) {}
static int civ_color_mid(iptr) { return 2; }
static int civ_color_primary(iptr) { return 3; }
static int tile_unit_owner(int, int) { return -1; }
static int gfx_city_sprite_index(int, uint, int *size) { *size = cities[0].size; return 0; }
static void hd_sprite_tint(Sprite *, int, int) {}
static void gfx_select_zoom_font(int) {}
static void text_set_font(iptr) {}
static void text_set_surface(iptr) {}
static void text_set_colors(int, int, int, int) {}
static void gfx_draw_rect_frame(iptr, RECT *r, int) { badge = *r; }
static void gfx_fill_rect(iptr, RECT *, int) {}
static void gfx_draw_text(const char *s, int, int) { strcpy(label, s); }
static void rect_set_xywh(iptr p, int x, int y, int w, int h) { *(RECT *)p = {x, y, x + w, y + h}; }
static void rect_offset(iptr p, int x, int y) {
    RECT &r = *(RECT *)p; r.left += x; r.right += x; r.top += y; r.bottom += y;
}
static void rect_deflate(iptr p, int x, int y) {
    RECT &r = *(RECT *)p; r.left += x; r.right -= x; r.top += y; r.bottom -= y;
}
static void __itoa(int n, char *s, int) { snprintf(s, 8, "%d", n); }
#define crt_strcpy strcpy
#define crt_strcat strcat
#define _strlen strlen
#include "production.inc"
static void draw(DibSurface *s, uint flags = 0) {
    label[0] = 0; silhouettes = ordinary_disorder = resets = 0;
    gfx_draw_city((iptr)s, 0, flags, 20, 30, 0);
    assert(resets == 1);
}
int main() {
    draw(&advisor_state.window);
    assert(!strcmp(label, "7") && !silhouettes && !ordinary_disorder);
    cities[0].flags = CITYF_DISORDER;
    draw(&advisor_state.window, 0x1000); // Top Five, including unknown enemy sizes.
    assert(!strcmp(label, "7!") && !silhouettes && !ordinary_disorder);
    assert(badge.left == 25 && badge.top == 47 && badge.right - badge.left >= 21);
    cities[0].size = 127;
    draw(&advisor_state.window); // City Status, including three-digit populations.
    assert(!strcmp(label, "127!") && badge.right - badge.left >= 37);
    draw(&advisor_state.window, 1);
    assert(!label[0] && !silhouettes && !ordinary_disorder);
    draw(&map_surface);
    assert(!label[0] && silhouettes == 1 && !ordinary_disorder);
    hd_enabled = 0;
    draw(&advisor_state.window);
    assert(!label[0] && !silhouettes && ordinary_disorder == 1);
    hd_enabled = 1; cities[0].flags = 0; camera = 1;
    draw(&map_surface);
    assert(!label[0]); // Smooth map labels still belong to the camera overlay.
    camera = 0; status_text = 0;
    draw(&map_surface);
    assert(!label[0]); // Overview maps retain the readability cutoff.
    puts("City badges: report population and disorder, bounds, hidden status, original and map rendering passed.");
}
