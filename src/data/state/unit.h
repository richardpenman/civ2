/* Units: moves and combat, the unit editors and unit sounds.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_UNIT_H
#define CIV2_STATE_UNIT_H

/* The combat and move animation: defender and origin. */
#pragma pack(push, 1)
typedef struct CombatAnim {
    int defender_x;  /* 00633e40 */
    int defender_y;  /* 00633e44 */
    int defender;  /* 00633e48 */
    int origin_x;  /* 00633e4c */
    int origin_y;  /* 00633e50 */
    iptr origin_stack_unit;  /* 00633e54 */
} CombatAnim;
#pragma pack(pop)
extern CombatAnim combat_anim;
extern RODATA i32 unit_sound_button_label_ids[6];
extern i8 unit_sound_editor_empty_title[4];  /* only unit_sound_editor_create_wnd uses it */
extern RODATA i8 unit_sound_wav_default_ext[4];
extern RODATA i8 unit_sound_path_sep_str[4];
extern RODATA i8 unit_sound_wav_suffix_str[8];
/* A unit's move in progress. */
#pragma pack(push, 1)
typedef struct UnitMoveState {
    char unanimated_state;  /* 00634c9c */
    u8 unanimated_state_rest[3];
    int in_progress;  /* 00634ca0 */
} UnitMoveState;
#pragma pack(pop)
extern UnitMoveState unit_move;
#pragma pack(push, 1)
struct unit_editor_ctrl_positions_block {
    union {
        struct {
            i8 unit_editor_ctrl_positions[4];  /* 00635df8 */
            i8 unit_editor_ctrl_pos_ys[4];  /* 00635dfc */
            int unit_editor_prereq_x;  /* 00635e00 */
            int unit_editor_prereq_y;  /* 00635e04 */
            int unit_editor_obsolete_x;  /* 00635e08 */
            int unit_editor_obsolete_y;  /* 00635e0c */
            int unit_editor_role_x;  /* 00635e10 */
            int unit_editor_role_y;  /* 00635e14 */
            int unit_editor_domain_x;  /* 00635e18 */
            i32 unit_editor_domain_y_i32;  /* 00635e1c */
        };
        EditorPos unit_editor_combo_pos[5];  /* 00635df8 */
    };
};
#pragma pack(pop)
extern struct unit_editor_ctrl_positions_block unit_editor_ctrl_positions_block;
#pragma pack(push, 1)
struct unit_editor_edit_x_table_block {
    i32 unit_editor_edit_x_table[1];  /* 00635e20 */
    i32 unit_editor_edit_y_table[1];  /* 00635e24 */
    int unit_editor_holds_x;  /* 00635e28 */
    int unit_editor_holds_y;  /* 00635e2c */
    int unit_editor_move_x;  /* 00635e30 */
    i32 unit_editor_edit2_y_i32;  /* 00635e34 */
    int unit_editor_edit3_x;  /* 00635e38 */
    int unit_editor_edit3_y;  /* 00635e3c */
    int unit_editor_edit4_x;  /* 00635e40 */
    int unit_editor_edit4_y;  /* 00635e44 */
    int unit_editor_edit5_x;  /* 00635e48 */
    int unit_editor_edit5_y;  /* 00635e4c */
    int unit_editor_edit6_x;  /* 00635e50 */
    int unit_editor_edit6_y;  /* 00635e54 */
    int unit_editor_edit7_x;  /* 00635e58 */
    int unit_editor_edit7_y;  /* 00635e5c */
};
#pragma pack(pop)
extern struct unit_editor_edit_x_table_block unit_editor_edit_x_table_block;
#pragma pack(push, 1)
struct unit_editor_ctrl_kinds_block {
    union {
        struct {
            i8 unit_editor_ctrl_kinds[4];  /* 00635e60 */
            i8 unit_editor_ctrl_slots[84];  /* 00635e64 */
            i8 unit_editor_field_min_table[32];  /* 00635eb8 */
        };
        EditorCtrlRow unit_editor_ctrl_rows[13];  /* 00635e60 (rows 11-12 overlap the min table) */
    };
    i8 unit_editor_field_max_table[24];  /* 00635ed8 */
    i32 unit_editor_move_max_i32;  /* 00635ef0 */
    u8 unit_editor_move_max_i32_rest[12];
    i32 unit_editor_hp_max_i32;  /* 00635f00 */
    u8 unit_editor_hp_max_i32_rest[4];
    i8 unit_editor_int_fmt[4];  /* 00635f08 */
    i8 units_txt_name_sep[4];  /* 00635f0c */
    i8 units_txt_domain_sep[4];  /* 00635f10 */
    i8 units_txt_move_suffix[4];  /* 00635f14 */
    i8 units_txt_range_sep[4];  /* 00635f18 */
    i8 units_txt_attack_suffix[4];  /* 00635f1c */
    i8 units_txt_defense_suffix[4];  /* 00635f20 */
    i8 units_txt_hp_suffix[4];  /* 00635f24 */
    i8 units_txt_firepower_suffix[4];  /* 00635f28 */
    i8 units_txt_cost_sep[4];  /* 00635f2c */
    i8 units_txt_hold_sep[4];  /* 00635f30 */
    i8 units_txt_role_sep[4];  /* 00635f34 */
    i8 units_txt_flag_digits[8];  /* 00635f38 */
    i8 units_txt_line_end[4];  /* 00635f40 */
};
#pragma pack(pop)
extern struct unit_editor_ctrl_kinds_block unit_editor_ctrl_kinds_block;
extern u8 str_empty_186[4];  /* only EditorWindow::unit_editor_create_edit uses it */
extern i32 unit_move_anim_active;
extern RODATA i8 path_separator_str[8];
#pragma pack(push, 1)
struct pollution_sprite_block {
    u8 pollution_sprite[64];  /* 00641808 */
    Sprite unit_sprites[63];  /* 00641848: by unit type (62) and one more */
    u8 unit_sprites_rest[4];
};
#pragma pack(pop)
extern struct pollution_sprite_block pollution_sprite_block;
#pragma pack(push, 1)
struct unit_types_block {
    UnitType unit_types[62];  /* 0064b1b8 */
    u8 scenario_events[756];  /* 0064b690 */
    u8 str_empty_188[8];  /* 0064b984 */
    char *events_pool_base;  /* 0064b98c */
    u8 events_pool_base_rest[2];
    ushort events_pool_size;  /* 0064b992 */
    ushort events_pool_bytes_free;  /* 0064b994 */
    u8 events_pool_bytes_free_rest[2];
    int scenario_event_count;  /* 0064b998 */
    ScenarioEvent *events_list_head;  /* 0064b99c */
};
#pragma pack(pop)
extern struct unit_types_block unit_types_block;
extern UnitType *const unit_types;  /* = unit_types_block.unit_types */
/* The scratch unit after the unit table (units[2048]): what the code finds past the last unit. */
#pragma pack(push, 1)
typedef struct DummyUnit {
    i16 x;  /* 006660f0 */
    i16 y;  /* 006660f2 */
    i16 flags;  /* 006660f4 */
    i8 type;  /* 006660f6 */
    i8 owner;  /* 006660f7 */
    i8 moves_used;  /* 006660f8 */
    i8 visibility;  /* 006660f9 */
    u8 visibility_rest[3];
    i8 counter;  /* 006660fd */
    i8 byte_0e;  /* 006660fe */
    i8 orders;  /* 006660ff */
    i8 home_city;  /* 00666100 */
    u8 home_city_rest[1];
    i16 goto_x;  /* 00666102 */
    i16 goto_y;  /* 00666104 */
    i16 prev_in_stack;  /* 00666106 */
    i16 next_in_stack;  /* 00666108 */
    u8 next_in_stack_rest[6];
} DummyUnit;
#pragma pack(pop)
#pragma pack(push, 1)
struct power_graph_history_block {
    i8 power_graph_history[151][8];  /* 00655c38: by sample and civilization */
    Unit units[2048];  /* 006560f0 */
    DummyUnit dummy_unit;  /* 006660f0 */
    short stacked_draw_unit_copy[1];  /* 00666110 */
    short stacked_draw_unit_y;  /* 00666112 */
    u8 stacked_draw_unit_y_rest[3];
    char stacked_draw_unit_owner;  /* 00666117 */
    u8 stacked_draw_unit_owner_rest[14];
    short stacked_draw_unit_prev;  /* 00666126 */
    short stacked_draw_unit_next;  /* 00666128 */
    u8 stacked_draw_unit_next_rest[6];
};
#pragma pack(pop)
extern struct power_graph_history_block power_graph_history_block;
inline constexpr DummyUnit &dummy_unit = power_graph_history_block.dummy_unit;
extern Unit *const units;  /* = power_graph_history_block.units */
#pragma pack(push, 1)
struct sprite_picker_sprite_block {
    Sprite sprite_picker_sprite;  /* 006ac8e8 */
    int sprite_sheet_mode;  /* 006ac924 */
    u8 sprite_sheet_mode_rest[236];
    i32 unit_sound_effects_by_ctrl[61];  /* 006aca14 */
    uptr combat_defender_city;  /* 006acb08 */
    int combat_kill_count;  /* 006acb0c */
    u8 combat_kill_count_rest[32];
    uptr unit_defense_terrain;  /* 006acb30 */
    int combat_defense_bonus_halves;  /* 006acb34 */
    int net_can_escape_reply;  /* 006acb38 */
    u8 net_can_escape_reply_rest[28];
    i32 parley_pane_own_cities[4];  /* 006acb58 */
};
#pragma pack(pop)
extern struct sprite_picker_sprite_block sprite_picker_sprite_block;
#pragma pack(push, 1)
struct main_frame_bg_surface_block {
    DibSurface main_frame_bg_surface;  /* 006acb68 */
    int main_frame_nonclient_width;  /* 006acbb0 */
    i32 main_frame_nonclient_height_i32;  /* 006acbb4 */
    u8 main_frame_nonclient_height_i32_rest[24];
    CleanupFn crash_handler_callbacks[10];  /* 006acbd0 */
    i8 crash_debug_buffer[320];  /* 006acbf8 */
    union {
        struct {
            i32 unit_sound_slot_effects[1];  /* 006acd38 */
            int unit_sound_orders_effect;  /* 006acd3c */
            i32 unit_sound_explosion_effect_i32;  /* 006acd40 */
            i32 unit_sound_bomb_effect_i32;  /* 006acd44 */
            i32 unit_sound_crash_effect_i32;  /* 006acd48 */
            i32 unit_sound_sputter_effect_i32;  /* 006acd4c */
        };
        i32 unit_sound_effects[6];  /* 006acd38 by sound slot */
    };
    i32 unit_sound_editor_running;  /* 006acd50 */
    u8 unit_sound_editor_running_rest[4];
};
#pragma pack(pop)
extern struct main_frame_bg_surface_block main_frame_bg_surface_block;
/* The unit sound editor window. */
#pragma pack(push, 1)
typedef struct UnitSoundEditorState {
    DibSurface window;  /* 006acd58 */
    WndBase wnd;  /* 006acda0 */
    u8 wnd_handlers[188];  /* 006acdb0 */
    i32 style_flags;  /* 006ace6c */
    int caption_height;  /* 006ace70 */
    int border_width;  /* 006ace74 */
    int resize_border;  /* 006ace78 */
    int client_x;  /* 006ace7c */
    int client_y;  /* 006ace80 */
    int client_width;  /* 006ace84 */
    i32 client_height;  /* 006ace88 */
    i8 title[200];  /* 006ace8c */
    i32 caption_button_count;  /* 006acf54 */
    u8 caption_button_count_rest[168];
    i32 caption_button_cb;  /* 006ad000 */
    u8 caption_button_cb_rest[16];
    u8 fill_rect[16];  /* 006ad014 */
    i32 title_font;  /* 006ad024 */
    i32 title_small_font;  /* 006ad028 */
    i32 text_font;  /* 006ad02c */
    u8 text_font_rest[156];
} UnitSoundEditorState;
#pragma pack(pop)
/* The spaceship's figures (spaceship_calc_stats). */
#pragma pack(push, 1)
typedef struct SpaceshipStats {
    i32 fuel_pct;  /* 006ad0dc */
    int projected_arrival_year;  /* 006ad0e0 */
    int mass;  /* 006ad0e4 */
    i32 energy_pct;  /* 006ad0e8 */
    int success_pct;  /* 006ad0ec */
    i32 support_pct;  /* 006ad0f0 */
    int flight_time;  /* 006ad0f4 */
    u8 flight_time_rest[16];
} SpaceshipStats;
#pragma pack(pop)
#pragma pack(push, 1)
struct unit_sound_editor_window_block {
    UnitSoundEditorState unit_sound_editor;  /* 006acd58 */
    byte unit_move_flags;  /* 006ad0cc */
    u8 unit_move_flags_rest[3];
    char unit_move_by_local_human;  /* 006ad0d0 */
    u8 unit_move_by_local_human_rest[11];
    SpaceshipStats spaceship_stats;  /* 006ad0dc */
};
#pragma pack(pop)
extern struct unit_sound_editor_window_block unit_sound_editor_window_block;
inline constexpr SpaceshipStats &spaceship_stats = unit_sound_editor_window_block.spaceship_stats;
inline constexpr UnitSoundEditorState &unit_sound_editor = unit_sound_editor_window_block.unit_sound_editor;

#endif
