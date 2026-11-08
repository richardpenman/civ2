// SOURCE src/auto/graphics.cpp GamePalette::new_version
// SOURCE src/auto/diplomacy.cpp DiploLeaderWindow::init_military
// SOURCE src/auto/diplomacy.cpp DiploLeaderWindow::init_video
// SOURCE src/auto/reports.cpp ThroneRoomWindow::render
// SOURCE src/auto/reports.cpp ThroneRoomWindow::render_choices
// SOURCE src/game/bigframes.cpp SpaceshipWindow::render_ship_load_surfaces
// SOURCE src/game/bigframes.cpp SpaceshipWindow::render_ship_draw_part
// SOURCE src/game/bigframes.cpp SpaceshipWindow::render_ship
// SOURCE src/auto/graphics.cpp screen_fade_out
// SOURCE src/auto/graphics.cpp screen_fade_in
// Run both production branches from the same seed. Only drawing, image decoding
// and OS services are replaced; the original branches determine the RNG budget.
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
using i8 = char;
using u8 = unsigned char;
using byte = unsigned char;
using i32 = int;
using u32 = unsigned;
using uint = unsigned;
using iptr = intptr_t;
#define _fclose fclose
#include "scoped.h"
static_assert(sizeof(void *) == 4, "the ship renderer uses serialized offsets");
static u32 seed;
static std::vector<int> trace;
static int crt_rand_at(const char *caller) {
    assert(!strcmp(caller, "palette_new_version"));
    seed = seed * 214013u + 2531011u;
    int value = (seed >> 16) & 0x7fff;
    trace.push_back(value);
    return value;
}
struct GamePalette {
    u32 pal_version;
    void new_version();
    void init() { new_version(); }
    void destroy() {}
    void set_rgb(int, int, iptr) { new_version(); }
    void load_ctab_res(int) { new_version(); }
    void get_rgb(iptr, int, int) {}
    void fade_begin(int, int, int, iptr) { new_version(); }
    void fade_end() { new_version(); }
    void fade_step(int) {} // palette_animate does not change the version
};
static int video_windows_destroyed;
struct DibSurface {
    int load_gif_resource(iptr, uint first, uint count, iptr palette) {
        assert(first == 10 && count == 236 && palette);
        ((GamePalette *)palette)->set_rgb(first, count, 0);
        return 1;
    }
    void fill(int) {}
    void avi_player_set_palette(iptr) {}
    void wnd_destroy_with_surface() { ++video_windows_destroyed; }
};
struct Sprite {
    void grab_from_surface(iptr, int, int, int, int, int) {}
    void replace_color(int, int) {}
    void draw(iptr, iptr, int, int) {}
};
static void sprite_init(Sprite *) {}
static void sprite_destroy(Sprite *) {}
static void surface_ctor(DibSurface *) {}
static void surface_dtor(DibSurface *) {}
static void win_flush_pending_msgs() {}
static int hd_enabled;
static struct { int power_rank[8]; } game;
static struct { u8 throne_room[9], tribe; } civs[8];
static GamePalette leader_palette;
static int video_found, video_error, video_palette_update, video_opens, video_closes;
struct AviPlayer : DibSurface {
    void create_child(iptr, int, int, int, iptr) {}
    int open(const char *) {
        ++video_opens;
        if (video_palette_update) leader_palette.new_version();
        return video_error;
    }
    void set_end_callback(void (*)()) {}
    void set_transparent(int) {}
    void show_frame(int) {}
};
static void avi_close(AviPlayer *) { ++video_closes; }
struct WndBase { void set_palette(iptr) {} };
static AviPlayer player;
static WndBase video_window;
static void diplo_leader_video_end_cb() {}
static int file_resolve_path(const char *) { return video_found; }
static void str_clear(char *s) { *s = 0; }
static void str_append(char *, const char *) {}
static void popup_game(const char *) {}
static const char str_civ2_kings[] = "kings", str_hrld[] = "hrld";
static const char str_avi[] = ".avi", str_empty_108[] = "", str_vfwnotregistered_4[] = "vfw";
static const char *leader_video_tribe_codes[21];
static struct { int leader_video_pos[21][2]; } leader_video_x_table_block;
struct DiploLeaderWindow {
    int leader_civ = 1, style_era = 0, leader_tribe = 0;
    iptr leader_surface = 0;
    iptr leader_palette = (iptr)&::leader_palette;
    iptr video_player = (iptr)&player, video_window = (iptr)&::video_window;
    i32 init_military();
    i8 init_video();
};
static int hd_military(DiploLeaderWindow *) { return 1; }
struct Piece { int x, y, w, h; };
static Piece throneroom_piece_rects[9][7];
static struct {
    int throneroom_bonus_rect_x[1], throneroom_bonus_rect_y[1];
    int throneroom_bonus_rect_w[1], throneroom_bonus_rect_h[1];
    int throneroom_bonus3_rect_x, throneroom_bonus3_rect_y;
    int throneroom_bonus3_rect_w, throneroom_bonus3_rect_h;
} throneroom_piece4_rect_x_block;
static DibSurface throne_surface;
struct ThroneRoomWindow {
    GamePalette control_list;
    int throne_civ = 1, throne_complete = 0;
    iptr throne_surface_wnd = (iptr)&throne_surface;
    void add_choice_regions() {}
    i32 render();
    i32 render_choices();
};
static int hd_throne_room(ThroneRoomWindow *, bool) { return 1; }
struct ShipPartRow { u8 kind, count; int v[6]; };
struct ShipSurfaces {
    i8 *surf_part, *surf_1b9, *surf_1e0, *surf_1c7, *surf_1ea, *surf_1d6;
    i8 *surf_1eb, *surf_1ba, *surf_1e1, *surf_1c8, *surf_1e9, *surf_1d7;
};
struct SpaceshipWindow {
    u8 gap[0x1f8];
    GamePalette ship_palette[1];
    u8 tail[0x1000];
    int ship_civ = 1, launch_state = 0;
    void render_ship_construct_surfaces(ShipSurfaces &, int &state) { state = 0; }
    void render_ship_free_surfaces(ShipSurfaces &, int &) {}
    void render_ship_init_rows(ShipPartRow (&rows)[88]) {
        for (int i = 0; i < 88; ++i) rows[i] = {(u8)(i % 6), (u8)(i / 6), {}};
    }
    int render_ship_load_surfaces(ShipSurfaces &);
    void render_ship_draw_part(ShipSurfaces &, ShipPartRow *, i8 *);
    i32 render_ship();
};
static struct { int spaceship_wnd_part_count_list[6]; } editor_text_edit_block;
static int spaceship_part_count(int, int) { return 5; }
static void spaceship_wnd_draw_title(int) {}
static int hd_spaceship(SpaceshipWindow *) { return 1; }
static struct { int unit_active; } blink;
static struct { int palettized; } display_palette;
static struct { GamePalette game_palette; } game_palette_block;
static struct { GamePalette grey_palette; } grey_palette_block;
static int hd_fade_grey;
static void ui_delay_ticks(int) {}
static void ui_blit_all_windows() {}
static void gfx_grey_out_windows() {}
static void gfx_repaint_main_windows() {}
static void hd_game_windows_grey(int) {}
static void palette_set_nocollapse_all() { game_palette_block.game_palette.new_version(); }
#include "production.inc"

template<class F> static void parity(u32 initial_seed, F draw) {
    hd_enabled = 0; seed = initial_seed; trace.clear();
    int original = draw();
    const auto expected_trace = trace;
    const auto expected_seed = seed;
    hd_enabled = 1; seed = initial_seed; trace.clear();
    int hd = draw();
    assert(hd == original && trace == expected_trace && seed == expected_seed);
}
int main() {
    // A palette version of zero must retry, rather than merely discarding one
    // rand(). Find a seed hitting zero after an earlier palette update as well.
    u32 retry_seed = 0;
    for (;; ++retry_seed) {
        seed = retry_seed * 214013u + 2531011u;
        seed = seed * 214013u + 2531011u;
        if (((seed >> 16) & 0x7fff) == 0) break;
    }
    for (u32 initial : {0u, 7u, retry_seed, 0xffffffffu}) {
        DiploLeaderWindow leader;
        for (int rank = 1; rank <= 7; ++rank) {
            game.power_rank[1] = rank;
            parity(initial, [&] { return leader.init_military(); });
        }
        for (video_found = 0; video_found <= 1; ++video_found)
            for (int error : {0, 1, -0x7ffbfeac})
                for (video_palette_update = 0; video_palette_update <= 1; ++video_palette_update) {
                    video_error = error;
                    // HD deliberately closes a successfully opened movie and
                    // shows its replacement. Compare RNG, not that UI result.
                    video_opens = video_closes = video_windows_destroyed = 0;
                    parity(initial, [&] { leader.init_video(); return 0; });
                    assert(video_opens == video_found * 2);
                    assert(video_closes == video_found);
                    assert(video_windows_destroyed == video_found);
                }
        ThroneRoomWindow throne;
        for (int complete = 0; complete <= 1; ++complete) {
            throne.throne_complete = complete;
            for (int slots = 0; slots < 256; ++slots)
                for (int bonuses = 0; bonuses < 128; ++bonuses) {
                    for (int i = 0; i < 8; ++i)
                        civs[1].throne_room[i] = slots & (1 << i) ? i % 4 + 1 : 0;
                    civs[1].throne_room[8] = bonuses;
                    parity(initial, [&] { return throne.render(); });
                    parity(initial, [&] { return throne.render_choices(); });
                    if (!complete) {
                        // Every subset of slots can be fully upgraded, including
                        // a choices screen with only its background left to load.
                        for (int i = 0; i < 8; ++i)
                            if (civs[1].throne_room[i]) civs[1].throne_room[i] = 4;
                        parity(initial, [&] { return throne.render_choices(); });
                    }
                }
        }
        SpaceshipWindow ship;
        for (int parts : {0, 1, 6, 40}) {
            for (int &count : editor_text_edit_block.spaceship_wnd_part_count_list) count = parts;
            for (int launch : {0, 3}) {
                ship.launch_state = launch;
                parity(initial, [&] { return ship.render_ship(); });
            }
        }
        for (int palettized : {0, 1}) {
            display_palette.palettized = palettized;
            parity(initial, [] { screen_fade_out(); return 0; });
            parity(initial, [] { screen_fade_in(); return 0; });
        }
    }
    puts("HD RNG parity: diplomacy, optional video, all throne slot/bonus masks, ships, fades and zero-version retries passed.");
}
