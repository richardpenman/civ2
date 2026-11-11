/* Graphics: sprites, surfaces, palettes, fonts and drawing.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_GRAPHICS_H
#define CIV2_STATE_GRAPHICS_H

extern i8 gfx_load_bmp_no_error_str[4];  /* only gfx_load_bmp_640x480 uses it */
extern RODATA i8 gfx_bmp_read_mode[4];
extern i8 gfx_save_bmp_no_error_str[4];
extern RODATA i8 gfx_bmp_write_mode[4];
extern u8 str_empty_109[4];  /* only PortraitDialog::create uses it */
/* Sizes of the window frames and fonts (chosen by the screen size). */
#pragma pack(push, 1)
typedef struct UiMetrics {
    iptr border_tile;  /* 0063357c */
    iptr font_height;  /* 00633580 */
    int large_screen;  /* 00633584 */
    int bevel_width;  /* 00633588 */
    int padding;  /* 0063358c */
    i32 highlight_color;  /* 00633590 */
    i32 shadow_color;  /* 00633594 */
    int caption_height;  /* 00633598 */
    int border_width;  /* 0063359c */
    int dlgframe_w2;  /* 006335a0 */
    int dlgframe_h2;  /* 006335a4 */
} UiMetrics;
#pragma pack(pop)
extern UiMetrics ui_metrics;
extern int map_label_font_size;  /* only gfx_select_zoom_font uses it */
extern RODATA i8 city_size_digit_str[4];
extern RODATA i8 city_size_digit2_str[64];
/* The sprite editor's mouse button colors. */
#pragma pack(push, 1)
typedef struct SpriteEditorColors {
    int right;  /* 00634000 */
    int left;  /* 00634004 */
    int prev_left;  /* 00634008 */
    int prev_right;  /* 0063400c */
} SpriteEditorColors;
#pragma pack(pop)
extern SpriteEditorColors sprite_editor_colors;
extern i8 sprite_editor_default_title[4];  /* only sprite_editor_create_window uses it */
extern i8 sprite_picker_default_title[4];  /* only sprite_picker_create_window uses it */
extern RODATA i8 sprite_import_bmp_ext_str[4];
extern RODATA i8 sprite_import_gif_ext_str[132];
extern DibSurface *gfx_draw_target;
extern RODATA i8 gif_sig_file[4];
extern RODATA i8 gif_sig_resource[4];
/* The drawing scale (zoom) factors. */
#pragma pack(push, 1)
typedef struct GfxScale {
    int num;  /* 00637f98 */
    int den;  /* 00637f9c */
    int x_num;  /* 00637fa0 */
    int x_den;  /* 00637fa4 */
    int y_num;  /* 00637fa8 */
    int y_den;  /* 00637fac */
    u8 y_den_rest[72];
} GfxScale;
#pragma pack(pop)
extern GfxScale gfx_scale;
/* The display's palette: black's index, and whether the display is palettized. */
#pragma pack(push, 1)
typedef struct DisplayPalette {
    i8 black_index;  /* 00638b40 */
    u8 black_index_rest[7];
    int palettized;  /* 00638b48 */
    u8 palettized_rest[4];
} DisplayPalette;
#pragma pack(pop)
extern DisplayPalette display_palette;
extern RODATA i8 logpal_static_system_colors[296];
/* The row order of device-independent bitmaps. */
#pragma pack(push, 1)
typedef struct DibRowOrder {
    int order;  /* 00638e18 */
    int bottom_up_logged;  /* 00638e1c */
} DibRowOrder;
#pragma pack(pop)
extern DibRowOrder dib_rows;
#pragma pack(push, 1)
struct icon_misc_sprites_block {
    Sprite icon_misc_sprites;  /* 0063f6f0 */
    Sprite icon_sprite_1;  /* 0063f72c */
    Sprite icon_sprite_2;  /* 0063f768 */
    Sprite icon_sprite_3;  /* 0063f7a4 */
    Sprite icon_sprite_4;  /* 0063f7e0 */
    Sprite icon_sprite_5;  /* 0063f81c */
    i8 river_sprites[960];  /* 0063f858 */
};
#pragma pack(pop)
extern struct icon_misc_sprites_block icon_misc_sprites_block;
extern u8 grassland_shield_sprite[64];
#pragma pack(push, 1)
struct tile_pick_mask_sprite_block {
    u8 tile_pick_mask_sprite[64];  /* 0063fcd8 */
    i8 river_mouth_sprites[240];  /* 0063fd18 */
};
#pragma pack(pop)
extern struct tile_pick_mask_sprite_block tile_pick_mask_sprite_block;
#pragma pack(push, 1)
struct gfx_sheet_surface_block {
    DibSurface gfx_sheet_surface;  /* 0063fe08 */
    i8 city_sprites[60];  /* 0063fe50 */
    i8 city_walled_sprites[2820];  /* 0063fe8c */
};
#pragma pack(pop)
extern struct gfx_sheet_surface_block gfx_sheet_surface_block;
#pragma pack(push, 1)
struct map_cursor_sprite_block {
    u8 map_cursor_sprite[64];  /* 00640b98 */
    i8 terrain_dither_sprites[2640];  /* 00640bd8 */
    i8 unexplored_dither_sprites[480];  /* 00641628 */
};
#pragma pack(pop)
extern struct map_cursor_sprite_block map_cursor_sprite_block;
#pragma pack(push, 1)
struct road_sprites_block {
    Sprite road_sprites[9];  /* 00642710 */
    Sprite railroad_sprites[9];  /* 0064292c */
    i32 unit_shield_y_offsets[64];  /* 00642b48 */
    i32 unit_shield_x_offsets[64];  /* 00642c48 */
    i8 citizen_sprites[2640];  /* 00642d48 */
    i8 civ_portrait_sprites[480];  /* 00643798 */
    i32 city_size_box_x_offsets[48];  /* 00643978 */
    i32 city_label_y_offsets[48];  /* 00643a38 */
};
#pragma pack(pop)
extern struct road_sprites_block road_sprites_block;
#pragma pack(push, 1)
struct tutorial_icon_sprite_block {
    u8 tutorial_icon_sprite[64];  /* 00643af8 */
    i8 coast_quarter_sprites[60];  /* 00643b38 */
    i8 coast_quarter_east_sprites[60];  /* 00643b74 */
    i8 coast_quarter_south_sprites[60];  /* 00643bb0 */
    i8 coast_quarter_west_sprites[1740];  /* 00643bec */
};
#pragma pack(pop)
extern struct tutorial_icon_sprite_block tutorial_icon_sprite_block;
#pragma pack(push, 1)
struct editor_tile_backdrop_sprite_block {
    u8 editor_tile_backdrop_sprite[64];  /* 006442b8 */
    i8 civ_flag_sprites[60];  /* 006442f8 */
    i8 civ_flag_dark_sprites[900];  /* 00644334 */
};
#pragma pack(pop)
extern struct editor_tile_backdrop_sprite_block editor_tile_backdrop_sprite_block;
#pragma pack(push, 1)
struct land_improvement_sprites_block {
    Sprite land_improvement_sprites;  /* 006446b8 */
    Sprite farmland_sprite;  /* 006446f4 */
    u8 mine_sprite[64];  /* 00644730 */
};
#pragma pack(pop)
extern struct land_improvement_sprites_block land_improvement_sprites_block;
#pragma pack(push, 1)
struct hill_sprites_block {
    Sprite hill_sprites[16];  /* 006447b0 */
    i8 nuke_anim_sprites[664];  /* 00644b70 */
};
#pragma pack(pop)
extern struct hill_sprites_block hill_sprites_block;
extern u8 editor_unit_backdrop_sprite[64];
extern u8 unexplored_tile_sprite[64];
extern u8 hut_sprite[64];
extern u8 map_grid_sprite[64];
extern Sprite forest_sprites[16];
extern u8 small_unhappy_icon_sprite[64];
#pragma pack(push, 1)
struct fortify_overlay_sprites_block {
    Sprite fortify_overlay_sprites;  /* 006465d8 */
    Sprite fortify_overlay_alt_sprite;  /* 00646614 */
    Sprite fortress_sprite;  /* 00646650 */
    Sprite icon_sprite_6;  /* 0064668c */
    Sprite pollution_icon_sprite;  /* 006466c8 */
    Sprite icon_sprite_7;  /* 00646704 */
    Sprite icon_sprite_8;  /* 00646740 */
    u8 city_disorder_icon_sprite[252];  /* 0064677c */
};
#pragma pack(pop)
extern struct fortify_overlay_sprites_block fortify_overlay_sprites_block;
#pragma pack(push, 1)
struct government_sprites_block {
    Sprite government_sprites;  /* 00646878 */
    Sprite government_despotism_sprite;  /* 006468b4 */
    Sprite government_monarchy_sprite;  /* 006468f0 */
    Sprite government_sprite_communism;  /* 0064692c */
    Sprite government_sprite_fundamentalism;  /* 00646968 */
    Sprite government_sprite_republic;  /* 006469a4 */
    u8 government_sprite_democracy[64];  /* 006469e0 */
    i8 small_citizen_sprites[664];  /* 00646a20 */
    i8 advance_category_icon_sprites[1200];  /* 00646cb8 */
    i8 explosion_sprites[480];  /* 00647168 */
};
#pragma pack(pop)
extern struct government_sprites_block government_sprites_block;
extern Sprite mountain_sprites[16];
#pragma pack(push, 1)
struct caption_zoom_out_button_sprite_block {
    u8 caption_zoom_out_button_sprite[64];  /* 00647788 */
    i8 sprite_editor_tool_sprites[904];  /* 006477c8 */
};
#pragma pack(pop)
extern struct caption_zoom_out_button_sprite_block caption_zoom_out_button_sprite_block;
#pragma pack(push, 1)
struct icons_pair_sprites_block {
    Sprite icons_pair_sprites;  /* 00647b50 */
    Sprite icon_sprite_9;  /* 00647b8c */
    Sprite city_radius_grid_sprite[2];  /* 00647bc8 */
};
#pragma pack(pop)
extern struct icons_pair_sprites_block icons_pair_sprites_block;
#pragma pack(push, 1)
struct terrain_base_sprites_block {
    Sprite terrain_base_sprites[10];  /* 00647c40 */
    u8 terrain_ocean_sprite[64];  /* 00647e98 */
};
#pragma pack(pop)
extern struct terrain_base_sprites_block terrain_base_sprites_block;
extern u8 tile_direction_mask_sprite[64];
extern DibSurface window_border_tile_surface;
extern u8 unit_shield_sprite[64];
#pragma pack(push, 1)
struct airbase_sprites_block {
    Sprite airbase_sprites;  /* 00647fa0 */
    Sprite airbase_occupied_sprite;  /* 00647fdc */
};
#pragma pack(pop)
extern struct airbase_sprites_block airbase_sprites_block;
extern u8 icons_unused_tall_sprite[64];
#pragma pack(push, 1)
struct city_down_arrow_sprite_block {
    u8 city_down_arrow_sprite[64];  /* 006480d8 */
    i8 research_flask_sprites[240];  /* 00648118 */
    i8 global_warming_sprites[240];  /* 00648208 */
    i8 terrain_resource_sprites[60];  /* 006482f8 */
    i8 terrain_resource2_sprites[1260];  /* 00648334 */
};
#pragma pack(pop)
extern struct city_down_arrow_sprite_block city_down_arrow_sprite_block;
#pragma pack(push, 1)
struct civ_primary_colors_block {
    union {
        struct {
            i8 civ_primary_colors[4];  /* 00655358 */
            i8 civ_mid_colors[4];  /* 0065535c */
            i8 civ_secondary_colors[4];  /* 00655360 */
            i8 leader_flags_table[116];  /* 00655364 */
        };
        CivColors civ_colors[8];  /* 00655358 */
    };
};
#pragma pack(pop)
extern struct civ_primary_colors_block civ_primary_colors_block;
#pragma pack(push, 1)
struct game_palette_block {
    u8 game_palette[1296];  /* 006a8c00 */
    int event_dont_play_wonders;  /* 006a9110 */
    u8 event_dont_play_wonders_rest[12];
};
#pragma pack(pop)
extern struct game_palette_block game_palette_block;
/* The status window and its layout. */
#pragma pack(push, 1)
typedef struct StatusWindowState {
    DibSurface window;  /* 006abc68 */
    WndBase wnd;  /* 006abcb0 */
    i8 wnd_events[204];  /* 006abcc0 */
    i32 client_x;  /* 006abd8c */
    i32 client_y;  /* 006abd90 */
    int client_width;  /* 006abd94 */
    int client_height;  /* 006abd98 */
    u8 client_height_rest[392];
    i32 indicator_rect;  /* 006abf24 */
    u8 indicator_rect_rest[4];
    int client_right;  /* 006abf2c */
    int client_bottom;  /* 006abf30 */
    u8 client_bottom_rest[12];
    i8 panel_gap;  /* 006abf40 */
    u8 panel_gap_rest[3];
    int upper_panel_height;  /* 006abf44 */
    DibSurface timer_bg_surface;  /* 006abf48 */
    byte timer_surface_init;  /* 006abf90 */
    u8 timer_surface_init_rest[7];
    GameFont info_font[2];  /* 006abf98 */
    int lower_rect;  /* 006abfa8 */
    i32 lower_rect_top;  /* 006abfac */
    i32 lower_rect_right;  /* 006abfb0 */
    int lower_rect_bottom;  /* 006abfb4 */
    i32 timer_last_rect[1];  /* 006abfb8 */
    int timer_last_rect_top;  /* 006abfbc */
    int timer_last_rect_right;  /* 006abfc0 */
    int timer_last_rect_bottom;  /* 006abfc4 */
    u8 timer_last_rect_bottom_rest[200];
} StatusWindowState;
#pragma pack(pop)
/* The sprite (unit/icon) editor and its sprite picker. */
#pragma pack(push, 1)
typedef struct SpriteEditorState {
    i32 undo_flag_y;  /* 006ac0a0 */
    i32 undo_flag_x;  /* 006ac0a4 */
    DibSurface sprite_sheet_surface;  /* 006ac0a8 */
    i32 flag_y;  /* 006ac0f0 */
    i32 flag_x;  /* 006ac0f4 */
    u8 marker_rect[16];  /* 006ac0f8 */
    i8 plot_rect;  /* 006ac108 */
    u8 plot_rect_rest[15];
    int paint_tool;  /* 006ac118 */
    int sprite_picker_dialog;  /* 006ac11c */
    int tool;  /* 006ac120 */
    i32 brush_size;  /* 006ac124 */
    DibSurface surface;  /* 006ac128 */
    u8 orig_sprite[64];  /* 006ac170 */
    DibSurface window;  /* 006ac1b0 */
    WndBase wnd;  /* 006ac1f8 */
    u8 wnd_events[188];  /* 006ac208 */
    i32 style_flags;  /* 006ac2c4 */
    int caption_height;  /* 006ac2c8 */
    int border_width;  /* 006ac2cc */
    int resize_border;  /* 006ac2d0 */
    int origin_x;  /* 006ac2d4 */
    int origin_y;  /* 006ac2d8 */
    int client_width;  /* 006ac2dc */
    i32 client_height;  /* 006ac2e0 */
    i8 title[200];  /* 006ac2e4 */
    i32 caption_button_count;  /* 006ac3ac */
    u8 caption_button_count_rest[168];
    i32 caption_button_cb;  /* 006ac458 */
    u8 caption_button_cb_rest[16];
    u8 fill_rect[16];  /* 006ac46c */
    i32 title_font;  /* 006ac47c */
    i32 title_small_font;  /* 006ac480 */
    i32 text_font;  /* 006ac484 */
    DibSurface sprite_picker_window;  /* 006ac488 */
    u8 sprite_picker_wnd[204];  /* 006ac4d0 */
    i32 sprite_picker_style_flags;  /* 006ac59c */
    int sprite_picker_caption_height;  /* 006ac5a0 */
    int sprite_picker_border_width;  /* 006ac5a4 */
    int sprite_picker_resize_border;  /* 006ac5a8 */
    u8 sprite_picker_resize_border_rest[16];
    i8 sprite_picker_title[200];  /* 006ac5bc */
    i32 sprite_picker_caption_button_count;  /* 006ac684 */
    u8 sprite_picker_caption_button_count_rest[168];
    i32 sprite_picker_caption_button_cb;  /* 006ac730 */
    u8 sprite_picker_caption_button_cb_rest[32];
    i32 sprite_picker_title_font;  /* 006ac754 */
    i32 sprite_picker_title_small_font;  /* 006ac758 */
    i32 sprite_picker_text_font;  /* 006ac75c */
    u8 sprite_picker_text_font_rest[276];
    uint key_color;  /* 006ac874 */
    int width;  /* 006ac878 */
    int height;  /* 006ac87c */
    int last_px;  /* 006ac880 */
    int last_py;  /* 006ac884 */
    SpriteSheetSaveFn ok_callback;  /* 006ac888 */
    i32 running;  /* 006ac88c */
    i32 sprite_picker_running;  /* 006ac890 */
    int paint_color;  /* 006ac894 */
    i32 cursor_id;  /* 006ac898 */
    int sprite_sheet_cell_count;  /* 006ac89c */
    Sprite *target_sprite;  /* 006ac8a0 */
    i32 painting;  /* 006ac8a4 */
    u8 result_sprite[64];  /* 006ac8a8 */
} SpriteEditorState;
#pragma pack(pop)
#pragma pack(push, 1)
struct status_window_block {
    StatusWindowState status_wnd;  /* 006abc68 */
    GameFont map_label_font[2];  /* 006ac090 */
    SpriteEditorState sprite_editor;  /* 006ac0a0 */
};
#pragma pack(pop)
extern struct status_window_block status_window_block;
inline constexpr SpriteEditorState &sprite_editor = status_window_block.sprite_editor;
inline constexpr StatusWindowState &status_wnd = status_window_block.status_wnd;
/* The timer library's entry points. */
#pragma pack(push, 1)
typedef struct TimerDll {
    TimerDllGetTimerIdFn get_timer_id;  /* 006e47fc */
    TimerDllGetTimerIndexFn get_timer_index;  /* 006e4800 */
    HMODULE module;  /* 006e4804 */
    LPTIMECALLBACK timer_callback;  /* 006e4808 */
    TimerDllResetNotifiedFn reset_timer_notified;  /* 006e480c */
    TimerDllSetTimerIdFn set_timer_id;  /* 006e4810 */
    u8 set_timer_id_rest[4];
} TimerDll;
#pragma pack(pop)
/* The scaling tables in use (from gfx_scale_caches). */
#pragma pack(push, 1)
typedef struct GfxScaleTables {
    i8 *x_table;  /* 006e47c0 */
    i8 *y_table;  /* 006e47c4 */
    i32 *table;  /* 006e47c8 */
    u8 table_rest[4];
} GfxScaleTables;
#pragma pack(pop)
#pragma pack(push, 1)
struct default_draw_font_obj_block {
    i32 default_draw_font_obj[2571];  /* 006d1ec8 */
    DWORD scrollbar_base_wnd_extra;  /* 006d46f4 */
    WNDPROC scrollbar_base_wndproc;  /* 006d46f8 */
    u8 scrollbar_base_wndproc_rest[4];
    union {
        struct {
            i8 gfx_scale_cache_num[4];  /* 006d4700 */
            i8 gfx_scale_cache_den[4];  /* 006d4704 */
            i8 gfx_scale_cache_last_used[4];  /* 006d4708 */
            i8 gfx_scale_cache_tables[65716];  /* 006d470c */
        };
        GfxScaleCache gfx_scale_caches[16];  /* 006d4700 */
    };
    GfxScaleTables gfx_scale_tables;  /* 006e47c0 */
    i8 dbg_log_file[8];  /* 006e47d0 */
    DWORD edit_base_wnd_extra;  /* 006e47d8 */
    WNDPROC edit_base_wndproc;  /* 006e47dc */
    u8 edit_base_wndproc_rest[4];
    DWORD combo_base_wnd_extra;  /* 006e47e4 */
    u8 combo_base_wnd_extra_rest[4];
    WNDPROC combo_base_wndproc;  /* 006e47ec */
    WNDPROC listbox_base_wndproc;  /* 006e47f0 */
    DWORD listbox_base_wnd_extra;  /* 006e47f4 */
    u8 listbox_base_wnd_extra_rest[4];
    TimerDll timerdll;  /* 006e47fc */
    i8 snd_wave_dev_caps;  /* 006e4818 */
    u8 snd_wave_dev_caps_rest[39];
    byte snd_wave_dev_formats;  /* 006e4840 */
    u8 snd_wave_dev_formats_rest[15];
};
#pragma pack(pop)
extern struct default_draw_font_obj_block default_draw_font_obj_block;
inline constexpr GfxScaleTables &gfx_scale_tables = default_draw_font_obj_block.gfx_scale_tables;
inline constexpr TimerDll &timerdll = default_draw_font_obj_block.timerdll;

#endif
