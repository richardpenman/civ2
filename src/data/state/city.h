/* Cities: their calculation and production, the city window and the city view.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_CITY_H
#define CIV2_STATE_CITY_H

extern pointer ptr_fun;  /* only TaxRateDialog's constructor uses it */
extern u8 ptr_fun_rest[16];
#pragma pack(push, 1)
struct cityview_improvement_art_x_block {
    i8 cityview_improvement_art_x[4];  /* 0061c360 */
    i8 cityview_improvement_art_y[4];  /* 0061c364 */
    i8 cityview_improvement_art_w[4];  /* 0061c368 */
    i8 cityview_improvement_art_h[532];  /* 0061c36c */
    i8 cityview_wonder_art_x[4];  /* 0061c580 */
    i8 cityview_wonder_art_y[4];  /* 0061c584 */
    i8 cityview_wonder_art_w[4];  /* 0061c588 */
    i8 cityview_wonder_art_h[436];  /* 0061c58c */
};
#pragma pack(pop)
extern struct cityview_improvement_art_x_block cityview_improvement_art_x_block;
#pragma pack(push, 1)
struct cityview_slot_x_table_block {
    i8 cityview_slot_x_table[4];  /* 0061c740 */
    i8 cityview_slot_y_table[1436];  /* 0061c744 */
    u8 cityview_slot_tables_init[376];  /* 0061cce0 */
};
#pragma pack(pop)
extern struct cityview_slot_x_table_block cityview_slot_x_table_block;
/* The tax rate dialog. */
#pragma pack(push, 1)
typedef struct TaxRateDialogState {
    i32 lock_flags[3];  /* 00624f10 */
    int bg_surface;  /* 00624f1c */
} TaxRateDialogState;
#pragma pack(pop)
extern TaxRateDialogState taxrate_state;
extern u8 str_empty_9[4];  /* only TaxRateDialog::run uses it */
#pragma pack(push, 1)
struct city_editor_combo_x_table_block {
    i32 city_editor_combo_x_table[1];  /* 00625e70 */
    i32 city_editor_combo_y_table[1];  /* 00625e74 */
    int city_editor_combo2_x;  /* 00625e78 */
    int city_editor_combo2_y;  /* 00625e7c */
    int city_editor_combo3_x;  /* 00625e80 */
    int city_editor_combo3_y;  /* 00625e84 */
};
#pragma pack(pop)
extern struct city_editor_combo_x_table_block city_editor_combo_x_table_block;
/* The city view (the city's picture): its scroll and window. */
#pragma pack(push, 1)
typedef struct CityViewState {
    int scroll_x;  /* 00626a00 */
    CityViewWindow *wnd;  /* 00626a04 */
} CityViewState;
#pragma pack(pop)
extern CityViewState cityview;
extern u8 str_empty_74[4];  /* only CityViewWindow::create_windows uses it */
extern u8 str_empty_75[8];  /* only CityViewWindow::create_windows uses it */
/* City production preferences being chosen. */
#pragma pack(push, 1)
typedef struct CityPrefs {
    iptr count;  /* 0062ccc0 */
    i32 no_defend;  /* 0062ccc4 */
} CityPrefs;
#pragma pack(pop)
extern CityPrefs citypref;
extern u8 str_empty_116[4];  /* only WonderMovieWindow::play uses it */
/* The editors' background and font. */
#pragma pack(push, 1)
typedef struct EditorUi {
    i32 value_clamped;  /* 0062e014 */
    DibSurface *bg_surface;  /* 0062e018 */
} EditorUi;
#pragma pack(pop)
extern EditorUi editor_ui;
extern iptr editor_font_size;
#pragma pack(push, 1)
struct impr_editor_combo_x_table_block {
    union {
        struct {
            i8 impr_editor_combo_x_table[4];  /* 0062e398 */
            i8 impr_editor_combo_y_table[4];  /* 0062e39c */
            int impr_editor_combo1_x;  /* 0062e3a0 */
            int impr_editor_combo1_y;  /* 0062e3a4 */
            int impr_editor_combo2_x;  /* 0062e3a8 */
            int impr_editor_combo2_y;  /* 0062e3ac */
        };
        EditorPos impr_editor_combo_pos[3];  /* 0062e398 */
    };
};
#pragma pack(pop)
extern struct impr_editor_combo_x_table_block impr_editor_combo_x_table_block;
#pragma pack(push, 1)
struct impr_editor_edit_x_table_block {
    i32 impr_editor_edit_x_table[1];  /* 0062e3b0 */
    i32 impr_editor_edit_y_table[1];  /* 0062e3b4 */
    int impr_editor_edit1_x;  /* 0062e3b8 */
    int impr_editor_edit1_y;  /* 0062e3bc */
};
#pragma pack(pop)
extern struct impr_editor_edit_x_table_block impr_editor_edit_x_table_block;
#pragma pack(push, 1)
struct impr_editor_ctrl_kinds_block {
    union {
        struct {
            i8 impr_editor_ctrl_kinds[4];  /* 0062e3c0 */
            i8 impr_editor_ctrl_slots[36];  /* 0062e3c4 */
        };
        EditorCtrlRow impr_editor_ctrl_rows[5];  /* 0062e3c0 */
    };
    i8 impr_editor_field_min_table[8];  /* 0062e3e8 */
    i8 impr_editor_field_max_table[8];  /* 0062e3f0 */
    i8 impr_editor_value_fmt[4];  /* 0062e3f8 */
    i8 rules_improve_name_sep_str[4];  /* 0062e3fc */
    i8 rules_improve_cost_sep_str[4];  /* 0062e400 */
};
#pragma pack(pop)
extern struct impr_editor_ctrl_kinds_block impr_editor_ctrl_kinds_block;
extern RODATA i8 rules_improve_line_end_str[4];
extern RODATA i8 rules_endwonder_comment_sep_str[4];
extern RODATA i8 rules_endwonder_line_end_str[4];
extern u8 str_empty_138[4];  /* only EditorWindow::impr_editor_create_edit uses it */
extern RODATA i8 impr_editor_help_percent_str[4];
extern RODATA i8 impr_editor_help_percent2_str[4];
/* City processing during the turn. */
#pragma pack(push, 1)
typedef struct CityTurnState {
    int yield_tile_worked;  /* 0062edf4 */
    int processing;  /* 0062edf8 */
    int worker_realloc;  /* 0062edfc */
    int window_shown;  /* 0062ee00 */
    i32 window_dirty;  /* 0062ee04 */
    i32 calc_cached_id;  /* 0062ee08 */
    i32 tile_job_found;  /* 0062ee0c */
} CityTurnState;
#pragma pack(pop)
extern CityTurnState city_turn;
/* The city window's pending actions. */
#pragma pack(push, 1)
typedef struct CityWindowState {
    int exit_requested;  /* 00630d18 */
    char modal_notice_active;  /* 00630d1c */
    u8 modal_notice_active_rest[3];
    i8 exit_btn_pending;  /* 00630d20 */
    u8 exit_btn_pending_rest[3];
    char change_button_variant;  /* 00630d24 */
    u8 change_button_variant_rest[3];
    int tile_pick_zoom_cached;  /* 00630d28 */
    int unit_dialog_busy;  /* 00630d2c */
    i8 dlg_close_pending;  /* 00630d30 */
    u8 dlg_close_pending_rest[3];
    int bg_size_level;  /* 00630d34 */
} CityWindowState;
#pragma pack(pop)
extern CityWindowState city_wnd_state;
extern RODATA i8 city_resmap_draw_dx_table[24];
extern RODATA i8 city_resmap_draw_dy_table[24];
extern int city_dlg_close_start_ticks;  /* only city_change_idle_autoclose uses it */
extern i32 dlg_unit_sprite_zoom;
/* The advisors' report window and its layout. */
#pragma pack(push, 1)
typedef struct AdvisorWindowState {
    DibSurface window;  /* 0063eb10 */
    WndBase wnd;  /* 0063eb58 */
    i8 wnd_events[204];  /* 0063eb68 */
    int client_x;  /* 0063ec34 */
    int client_y;  /* 0063ec38 */
    int client_width;  /* 0063ec3c */
    u8 client_width_rest[396];
    int client_rect;  /* 0063edcc */
    u8 client_rect_rest[4];
    int client_right;  /* 0063edd4 */
    u8 client_right_rest[88];
    Button close_button;  /* 0063ee30 */
    Button toggle_button;  /* 0063ee6c */
    Button alt_toggle_button[2];  /* 0063eea8 */
    Scrollbar scrollbar;  /* 0063ef20 */
    int report_id;  /* 0063ef60 */
    u8 report_id_rest[4];
    i32 page_count;  /* 0063ef68 */
    iptr report_civ;  /* 0063ef6c */
    int first_row;  /* 0063ef70 */
    int rows_per_page;  /* 0063ef74 */
    i32 list_top;  /* 0063ef78 */
    i32 list_height;  /* 0063ef7c */
    i32 row_height;  /* 0063ef80 */
    u8 row_height_rest[8];
    int items_per_page;  /* 0063ef8c */
    i32 list_left;  /* 0063ef90 */
    int list_width;  /* 0063ef94 */
    int column_width;  /* 0063ef98 */
    u8 column_width_rest[8];
    int vscroll_width;  /* 0063efa4 */
    int controls_created;  /* 0063efa8 */
    iptr report_param;  /* 0063efac */
    u8 report_param_rest[8];
} AdvisorWindowState;
#pragma pack(pop)
/* Trade: how much a city demands each commodity. */
#pragma pack(push, 1)
typedef struct CommodityDemand {
    int by_commodity[1];  /* 0063f540 */
    i32 wool[1];  /* 0063f544 */
    int beads;  /* 0063f548 */
    i32 cloth;  /* 0063f54c */
    i32 salt;  /* 0063f550 */
    int coal;  /* 0063f554 */
    int copper;  /* 0063f558 */
    int dye;  /* 0063f55c */
    i32 wine;  /* 0063f560 */
    int silk;  /* 0063f564 */
    int silver;  /* 0063f568 */
    int spice;  /* 0063f56c */
    i32 gems;  /* 0063f570 */
    int gold;  /* 0063f574 */
    int oil;  /* 0063f578 */
    int uranium;  /* 0063f57c */
} CommodityDemand;
#pragma pack(pop)
/* Trade: how much a city supplies each commodity. */
#pragma pack(push, 1)
typedef struct CommoditySupply {
    int by_commodity[1];  /* 0063f668 */
    i32 wool[1];  /* 0063f66c */
    int beads;  /* 0063f670 */
    int cloth;  /* 0063f674 */
    int salt;  /* 0063f678 */
    int coal;  /* 0063f67c */
    int copper;  /* 0063f680 */
    int dye;  /* 0063f684 */
    int wine;  /* 0063f688 */
    int silk;  /* 0063f68c */
    int silver;  /* 0063f690 */
    int spice;  /* 0063f694 */
    int gems;  /* 0063f698 */
    int gold;  /* 0063f69c */
    int oil;  /* 0063f6a0 */
    int uranium;  /* 0063f6a4 */
    u8 uranium_rest[72];
} CommoditySupply;
#pragma pack(pop)
#pragma pack(push, 1)
struct advisor_window_block {
    AdvisorWindowState advisor_state;  /* 0063eb10 */
    Sprite *credits_slot_sprites[30];  /* 0063efb8 */
    GameFont hall_of_fame_font;  /* 0063f030 */
    i8 credits_text_block[8];  /* 0063f038 */
    char *credits_text_data;  /* 0063f040 */
    u8 credits_text_data_rest[12];
    i32 credits_slot_x[30];  /* 0063f050 */
    HallOfFameEntry hall_of_fame[6];  /* 0063f0c8 */
    int mp_meeting_reply;  /* 0063f278 */
    u8 mp_meeting_reply_rest[708];
    CommodityDemand commodity_demand;  /* 0063f540 */
    i8 wonder_notice_flags[224];  /* 0063f580 */
    int nearest_city_dist;  /* 0063f660 */
    u8 nearest_city_dist_rest[4];
    CommoditySupply commodity_supply;  /* 0063f668 */
};
#pragma pack(pop)
extern struct advisor_window_block advisor_window_block;
inline constexpr CommoditySupply &commodity_supply = advisor_window_block.commodity_supply;
inline constexpr CommodityDemand &commodity_demand = advisor_window_block.commodity_demand;
inline constexpr AdvisorWindowState &advisor_state = advisor_window_block.advisor_state;
#pragma pack(push, 1)
struct small_resource_icon_sprites_block {
    Sprite small_resource_icon_sprites;  /* 00645068 */
    Sprite small_shield_icon_sprite;  /* 006450a4 */
    u8 small_trade_icon_sprite[64];  /* 006450e0 */
};
#pragma pack(pop)
extern struct small_resource_icon_sprites_block small_resource_icon_sprites_block;
#pragma pack(push, 1)
struct caption_close_button_sprite_block {
    u8 caption_close_button_sprite[64];  /* 00645120 */
    i8 improvement_icon_sprites[120];  /* 00645160 */
    u8 notify_sprites[1320];  /* 006451d8 */
    u8 research_dialog_sprite[480];  /* 00645700 */
    u8 research_popup_sprite3[420];  /* 006458e0 */
    i8 wonder_icon_sprites[1380];  /* 00645a84 */
    i32 city_notify_sprite[76];  /* 00645fe8 */
};
#pragma pack(pop)
extern struct caption_close_button_sprite_block caption_close_button_sprite_block;
extern u8 unhappy_suppressed_icon_sprite[64];
extern i32 commodity_names[16];
extern u8 commodity_names_rest[4];
extern Improvement improvements[67];
extern City cities[256];
/* The city being calculated (city_recalc and the city window): yields, happiness, support. */
#pragma pack(push, 1)
typedef struct CityCalc {
    int saved_shield_box;  /* 006a6528 */
    u8 saved_shield_box_rest[4];
    i8 tile_flags[28];  /* 006a6530 */
    int unassigned_citizens;  /* 006a654c */
    int happy_count;  /* 006a6550 */
    int tax_total;  /* 006a6554 */
    int food_routes_in;  /* 006a6558 */
    int home_attack_units;  /* 006a655c */
    int food_box_rows;  /* 006a6560 */
    uint martial_law_units;  /* 006a6564 */
    int shield_support;  /* 006a6568 */
    int shield_waste;  /* 006a656c */
    int food_route_partner;  /* 006a6570 */
    uint capital_link;  /* 006a6574 */
    int science_total;  /* 006a6578 */
    int shield_rows;  /* 006a657c */
    int trade_corruption;  /* 006a6580 */
    int pollution;  /* 006a6584 */
    int capital_distance;  /* 006a6588 */
    u8 capital_distance_rest[4];
    i32 route_trade_values[3];  /* 006a6590 */
    int angry_count;  /* 006a659c */
    int zoom_requested;  /* 006a65a0 */
    i32 saved_build_category;  /* 006a65a4 */
    int unhappy_count;  /* 006a65a8 */
    int notify_enabled;  /* 006a65ac */
    int food_routes_out;  /* 006a65b0 */
    u8 food_routes_out_rest[4];
    i32 tile_yields[1];  /* 006a65b8 */
    int tile_shields;  /* 006a65bc */
    int tile_trade;  /* 006a65c0 */
    int pop_pollution_factor;  /* 006a65c4 */
    int yield_totals[1];  /* 006a65c8 */
    int total_shields;  /* 006a65cc */
    int total_trade;  /* 006a65d0 */
    int tile_job_priority;  /* 006a65d4 */
    int settlers_supported;  /* 006a65d8 */
    i32 workers_overflow;  /* 006a65dc */
    iptr tile_job_x;  /* 006a65e0 */
    int aggressive_units;  /* 006a65e4 */
    int tile_job_y;  /* 006a65e8 */
    u8 tile_job_y_rest[4];
    i8 unhappy_by_stage[1];  /* 006a65f0 */
    byte unhappy_after_lux;  /* 006a65f1 */
    u8 unhappy_after_lux_rest[1];
    byte unhappy_after_units;  /* 006a65f3 */
    byte unhappy_after_wonders;  /* 006a65f4 */
    u8 unhappy_after_wonders_rest[3];
    int pollution_divisor;  /* 006a65f8 */
    int lux_total;  /* 006a65fc */
    iptr capital_nearest;  /* 006a6600 */
    int specialist_count;  /* 006a6604 */
    int settler_food_cost;  /* 006a6608 */
    int support_unit_count;  /* 006a660c */
    i32 completed_improvement;  /* 006a6610 */
    u8 completed_improvement_rest[4];
    int building_content;  /* 006a6618 */
    int food_surplus;  /* 006a661c */
    i8 happy_by_stage[1];  /* 006a6620 */
    byte happy_after_lux;  /* 006a6621 */
    u8 happy_after_lux_rest[1];
    byte happy_after_units;  /* 006a6623 */
    byte happy_after_wonders;  /* 006a6624 */
    u8 happy_after_wonders_rest[3];
    i8 angry_by_stage[64];  /* 006a6628 */
} CityCalc;
#pragma pack(pop)
#pragma pack(push, 1)
struct main_menu_block {
    u8 main_menu[48];  /* 006a64f8 */
    CityCalc city_calc;  /* 006a6528 */
};
#pragma pack(pop)
extern struct main_menu_block main_menu_block;
inline constexpr CityCalc &city_calc = main_menu_block.city_calc;
extern DibSurface city_tile_pick_surface;
extern GameFont city_window_small_font;
extern DibSurface city_window_bg_surface;
/* The city window and its layout. */
#pragma pack(push, 1)
typedef struct CityWindowLayout {
    DibSurface window;  /* 006a91b8 */
    WndBase wnd[41];  /* 006a9200 */
    ClickMap clickmap;  /* 006a9490 */
    i32 city;  /* 006aa754 */
    i32 minimized;  /* 006aa758 */
    int closed;  /* 006aa75c */
    int refresh_locked;  /* 006aa760 */
    int open_mode;  /* 006aa764 */
    i32 info_panel_mode;  /* 006aa768 */
    i32 improvement_list_top;  /* 006aa76c */
    i32 improvement_list_count;  /* 006aa770 */
    u8 improvement_list_count_rest[16];
    i32 prev_offset_x;  /* 006aa784 */
    u8 prev_offset_x_rest[4];
    int size_level;  /* 006aa78c */
    i32 map_zoom;  /* 006aa790 */
    u8 map_zoom_rest[16];
    int map_rect;  /* 006aa7a4 */
    int map_rect_top;  /* 006aa7a8 */
    u8 map_rect_top_rest[236];
} CityWindowLayout;
#pragma pack(pop)
#pragma pack(push, 1)
struct city_window_block {
    CityWindowLayout city_wnd;  /* 006a91b8 */
};
#pragma pack(pop)
extern struct city_window_block city_window_block;
inline constexpr CityWindowLayout &city_wnd = city_window_block.city_wnd;
extern GameFont city_window_font[152];

#endif
