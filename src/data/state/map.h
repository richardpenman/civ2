/* The map, world generation and path finding.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_MAP_H
#define CIV2_STATE_MAP_H

extern RODATA i8 map_load_wildcard_str[8];
extern RODATA i8 map_load_default_ext_str[4];
extern RODATA i8 map_load_open_mode_str[4];
extern Terrain terrain_types[33];
/* Scenario start positions per tribe. */
#pragma pack(push, 1)
typedef struct StartPositions {
    i16 x[21];  /* 00627fe0 */
    u8 x_rest[6];
    i16 y[21];  /* 00628010 */
    u8 y_rest[10];
} StartPositions;
#pragma pack(pop)
extern StartPositions start_positions;
extern i32 map_click_in_progress;
extern u8 map_click_in_progress_rest[4];
/* The path finder's current query. */
#pragma pack(push, 1)
typedef struct PathSearch {
    iptr unit_type;  /* 0062d03c */
    i32 road_planning;  /* 0062d040 */
    i32 owner_civ;  /* 0062d044 */
    i32 roads_only;  /* 0062d048 */
    i32 debug_pending;  /* 0062d04c */
    int debug_requested;  /* 0062d050 */
} PathSearch;
#pragma pack(pop)
extern PathSearch pathfind;
#pragma pack(push, 1)
struct terrain_editor_combo_pos_x_block {
    union {
        struct {
            i8 terrain_editor_combo_pos_x[4];  /* 00631b80 */
            i8 terrain_editor_combo_pos_y[4];  /* 00631b84 */
            int terrain_editor_irrig_combo_x;  /* 00631b88 */
            int terrain_editor_irrig_combo_y;  /* 00631b8c */
            int terrain_editor_irrig_ai_combo_x;  /* 00631b90 */
            int terrain_editor_irrig_ai_combo_y;  /* 00631b94 */
            int terrain_editor_mine_combo_x;  /* 00631b98 */
            int terrain_editor_mine_combo_y;  /* 00631b9c */
            u8 terrain_editor_mine_combo_y_rest[8];
            int terrain_editor_transform_combo_x;  /* 00631ba8 */
            int terrain_editor_transform_combo_y;  /* 00631bac */
        };
        EditorPos terrain_editor_combo_pos[6];  /* 00631b80 */
    };
};
#pragma pack(pop)
extern struct terrain_editor_combo_pos_x_block terrain_editor_combo_pos_x_block;
#pragma pack(push, 1)
struct terrain_editor_field_pos_x_block {
    i32 terrain_editor_field_pos_x[1];  /* 00631bb0 */
    i32 terrain_editor_field_pos_y[1];  /* 00631bb4 */
    int terrain_editor_defense_field_x;  /* 00631bb8 */
    int terrain_editor_defense_field_y;  /* 00631bbc */
    int terrain_editor_food_field_x;  /* 00631bc0 */
    int terrain_editor_food_field_y;  /* 00631bc4 */
    int terrain_editor_shield_field_x;  /* 00631bc8 */
    int terrain_editor_shield_field_y;  /* 00631bcc */
    int terrain_editor_trade_field_x;  /* 00631bd0 */
    int terrain_editor_trade_field_y;  /* 00631bd4 */
    int terrain_editor_irrigate_bonus_x;  /* 00631bd8 */
    int terrain_editor_irrigate_bonus_y;  /* 00631bdc */
    int terrain_editor_irrigate_turns_x;  /* 00631be0 */
    int terrain_editor_irrigate_turns_y;  /* 00631be4 */
    int terrain_editor_mine_bonus_x;  /* 00631be8 */
    int terrain_editor_mine_bonus_y;  /* 00631bec */
    u8 terrain_editor_mine_bonus_y_rest[8];
};
#pragma pack(pop)
extern struct terrain_editor_field_pos_x_block terrain_editor_field_pos_x_block;
#pragma pack(push, 1)
struct terrain_editor_ctrl_kinds_block {
    union {
        struct {
            i8 terrain_editor_ctrl_kinds[4];  /* 00631bf8 */
            i8 terrain_editor_ctrl_slots[116];  /* 00631bfc */
        };
        EditorCtrlRow terrain_editor_ctrl_rows[15];  /* 00631bf8 */
    };
    i8 terrain_editor_field_min_table[48];  /* 00631c70 */
    i8 terrain_editor_field_max_table[48];  /* 00631ca0 */
    i8 terrain_editor_int_fmt[4];  /* 00631cd0 */
    i8 terrain_txt_ref_no_strs[12];  /* 00631cd4 */
    i8 terrain_txt_ref_yes_strs[12];  /* 00631ce0 */
    i8 terrain_txt_ref_comma_str[4];  /* 00631cec */
    i8 terrain_txt_ref_err_strs[12];  /* 00631cf0 */
    i8 terrain_txt_name_sep[4];  /* 00631cfc */
    i8 terrain_txt_move_sep[4];  /* 00631d00 */
    i8 terrain_txt_defense_sep[4];  /* 00631d04 */
    i8 terrain_txt_yield_sep[4];  /* 00631d08 */
    i8 terrain_txt_bonus_sep[4];  /* 00631d0c */
    i8 terrain_txt_turns_sep[4];  /* 00631d10 */
    i8 terrain_txt_ai_gov_sep[4];  /* 00631d14 */
};
#pragma pack(pop)
extern struct terrain_editor_ctrl_kinds_block terrain_editor_ctrl_kinds_block;
extern RODATA i8 terrain_txt_line_end_str[4];
extern u8 str_empty_162[4];  /* only EditorWindow::terrain_editor_create_edit uses it */
/* The map's tiles and per-civ knowledge, allocated for its size. */
#pragma pack(push, 1)
typedef struct MapStorage {
    Tile *tiles;  /* 00636598 */
    u8 tiles_rest[4];
    void *known_handles[8];  /* 006365a0 */
    u8 *known[8];  /* 006365c0 */
    u8 *coarse[4];  /* 006365e0 */
    u8 allocated;  /* 006365f0 */
    u8 allocated_rest[3];
} MapStorage;
#pragma pack(pop)
extern MapStorage map_data;
#pragma pack(push, 1)
struct landmass_table_block {
    union {
        struct {
            i8 landmass_table[2];  /* 00666130 */
            i8 landmass_fertile_tiles[2];  /* 00666132 */
            i8 water_body_sizes[3];  /* 00666134 */
            i8 landmass_border_bits[1025];  /* 00666137 */
        };
        Landmass landmass_recs[64];  /* 00666130 */
    };
};
#pragma pack(pop)
extern struct landmass_table_block landmass_table_block;
#pragma pack(push, 1)
struct scenario_title_surface_block {
    u8 scenario_title_surface[176];  /* 0066c670 */
    i8 coast_quarter_land_masks[8];  /* 0066c720 */
};
#pragma pack(pop)
extern struct scenario_title_surface_block scenario_title_surface_block;
/* Meetings between civilizations waiting to be held (network game). */
#pragma pack(push, 1)
typedef struct NetMeetQueue {
    i32 x[8];  /* 00673a78 */
    i32 y[8];  /* 00673a98 */
    i32 civ[8];  /* 00673ab8 */
    i32 ai_civ[8];  /* 00673ad8 */
} NetMeetQueue;
#pragma pack(pop)
/* The parts of the civilization score (civ_calc_score) and the scenario objectives. */
#pragma pack(push, 1)
typedef struct ScoreParts {
    int objective_outcome;  /* 00673f54 */
    int pollution;  /* 00673f58 */
    iptr wonders;  /* 00673f5c */
    int spaceship_arrived;  /* 00673f60 */
    iptr objective_civ_value;  /* 00673f64 */
    int scenario_points;  /* 00673f68 */
    int future_techs;  /* 00673f6c */
    iptr total_objective_value;  /* 00673f70 */
    int barbarian_bonus;  /* 00673f74 */
    iptr citizens;  /* 00673f78 */
    int conquest_bonus;  /* 00673f7c */
    iptr civ_objective_value;  /* 00673f80 */
    int spaceship_in_flight;  /* 00673f84 */
    int total;  /* 00673f88 */
    int peace;  /* 00673f8c */
    u8 peace_rest[16];
} ScoreParts;
#pragma pack(pop)
/* The path finder's search: goal, coarse grid, flood-fill queue. */
#pragma pack(push, 1)
typedef struct PathQueue {
    int goal_x;  /* 00673fa0 */
    int goal_y;  /* 00673fa4 */
    int coarse_start_x;  /* 00673fa8 */
    int coarse_start_y;  /* 00673fac */
    int grid_center_x;  /* 00673fb0 */
    int grid_center_y;  /* 00673fb4 */
    int flood_goal_y;  /* 00673fb8 */
    iptr queue_tail;  /* 00673fbc */
    int result_cost;  /* 00673fc0 */
    iptr queue_head;  /* 00673fc4 */
    i32 queue_x[2304];  /* 00673fc8 */
    i32 queue_y[2305];  /* 006763c8 */
    int flood_goal_x;  /* 006787cc */
    u8 flood_goal_x_rest[3695];
} PathQueue;
#pragma pack(pop)
/* The difference engine's scan of the game state and the packet being built. */
#pragma pack(push, 1)
typedef struct DiffEngScan {
    iptr scan_ptr;  /* 00679fe8 */
    i32 scan_offset;  /* 00679fec */
    i32 packet;  /* 00679ff0 */
    i32 packet_msg_type;  /* 00679ff4 */
    i32 packet_size;  /* 00679ff8 */
    u8 packet_size_rest[4];
    i32 packet_record_count;  /* 0067a000 */
    u8 packet_record_count_rest[24];
    i32 packet_payload[249];  /* 0067a01c */
    uint mirror_size[1];  /* 0067a400 */
    i32 scan_section;  /* 0067a404 */
    int mirror_end;  /* 0067a408 */
    u8 mirror_end_rest[4];
} DiffEngScan;
#pragma pack(pop)
/* The text file reader's current token and line. */
#pragma pack(push, 1)
typedef struct TextParse {
    i8 token_buf[260];  /* 00673e10 */
    iptr line_cursor;  /* 00673f14 */
    u8 line_cursor_rest[60];
} TextParse;
#pragma pack(pop)
/* The text being composed (messages, dialogs); the bytes before and after it are read too. */
#pragma pack(push, 1)
typedef struct TextBuffer {
    i8 before[1];  /* 0067963f */
    i8 buf[1];  /* 00679640 */
    i8 char_1;  /* 00679641 */
    i8 plus2[1];  /* 00679642 */
    i8 char_3;  /* 00679643 */
    u8 char_3_rest[2468];
} TextBuffer;
#pragma pack(pop)
#pragma pack(push, 1)
struct map_views_block {
    union {
        struct {
            DibSurface map_views;  /* 0066c7a8 */
            u8 map_view_wnds[220];  /* 0066c7f0 */
            i32 mapview_client_origin_x[1];  /* 0066c8cc */
            i32 mapview_client_origin_y[46];  /* 0066c8d0 */
            i32 main_view_timer_shown[1];  /* 0066c988 */
            i32 main_view_status_shown_i32;  /* 0066c98c */
            i32 main_view_timer_cache_i32;  /* 0066c990 */
            u8 main_view_timer_cache_i32_rest[192];
            i32 main_view_frame_left;  /* 0066ca54 */
            i32 main_view_frame_top;  /* 0066ca58 */
            i32 main_view_frame_right;  /* 0066ca5c */
            u8 main_view_frame_right_rest[8];
            i32 main_view_client_top;  /* 0066ca68 */
            u8 main_view_client_top_rest[24];
            u8 map_view_open_flags[2];  /* 0066ca84 */
            i8 mapview_track_moves[2];  /* 0066ca86 */
            i16 map_view_center_x[1];  /* 0066ca88 */
            i16 mapview_center_y[1];  /* 0066ca8a */
            short mapview_zoom[2];  /* 0066ca8c */
            i32 mapview_origin_x[1];  /* 0066ca90 */
            i32 mapview_origin_y[1];  /* 0066ca94 */
            i32 mapview_end_x;  /* 0066ca98 */
            i32 mapview_end_y;  /* 0066ca9c */
            u8 mapview_end_y_rest[16];
            i8 mapview_tile_width[4];  /* 0066cab0 */
            i8 mapview_tile_height[4];  /* 0066cab4 */
            int mapview_half_tile_width;  /* 0066cab8 */
            i32 mapview_half_tile_height[1];  /* 0066cabc */
            i8 mapview_quarter_tile_width[12];  /* 0066cac0 */
            i8 mapview_unit_sprite_height[4];  /* 0066cacc */
            int mapview_layout_width;  /* 0066cad0 */
            int mapview_layout_height;  /* 0066cad4 */
            int mapview_margin_x;  /* 0066cad8 */
            int mapview_margin_y;  /* 0066cadc */
            u8 tile_number_font[32];  /* 0066cae0 */
            i32 mapview_cursor_id[2214];  /* 0066cb00 */
        };
        MapView map_view_recs[8];  /* 0066c7a8 */
    };
    i16 net_saved_unit_next[2048];  /* 0066ed98 */
    i16 net_saved_unit_x[2048];  /* 0066fd98 */
    i32 net_handoff_wait_ticks_i32;  /* 00670d98 */
    u8 net_handoff_wait_ticks_i32_rest[4];
    i16 net_saved_unit_y[2048];  /* 00670da0 */
    i16 net_saved_unit_prev[3692];  /* 00671da0 */
    NetMeetQueue net_meet_pending;  /* 00673a78 */
    i8 human_wonders_by_group_min[4];  /* 00673af8 */
    i8 human_wonders_by_group_max[4];  /* 00673afc */
    i32 net_heartbeat_last_tick_i32;  /* 00673b00 */
    char unit_focus_done;  /* 00673b04 */
    u8 unit_focus_done_rest[3];
    i32 saved_human_players;  /* 00673b08 */
    u8 saved_human_players_rest[36];
    i32 tech_prereq_depth_cache[122];  /* 00673b30 */
    i32 civ_password_prompt_pending[8];  /* 00673d18 */
    i32 civ_has_password[14];  /* 00673d38 */
    i32 citypref_improvements[40];  /* 00673d70 */
    TextParse text_parse;  /* 00673e10 */
    ScoreParts civ_score;  /* 00673f54 */
    PathQueue path_queue;  /* 00673fa0 */
    TextBuffer textbuf;  /* 0067963f */
    DiffEngScan diffeng_scan;  /* 00679fe8 */
};
#pragma pack(pop)
extern struct map_views_block map_views_block;
inline constexpr TextBuffer &textbuf = map_views_block.textbuf;
/* (port) the bytes from textbuf.buf to the end of the text buffer (2472) */
#define TEXTBUF_SIZE (sizeof(TextBuffer) - 1)
inline constexpr TextParse &text_parse = map_views_block.text_parse;
inline constexpr DiffEngScan &diffeng_scan = map_views_block.diffeng_scan;
inline constexpr PathQueue &path_queue = map_views_block.path_queue;
inline constexpr ScoreParts &civ_score = map_views_block.civ_score;
inline constexpr NetMeetQueue &net_meet_pending = map_views_block.net_meet_pending;
#pragma pack(push, 1)
struct dialog_text_font_obj_block {
    GameFont dialog_text_font_obj;  /* 006ceca8 */
    i8 dialog_section_name[112];  /* 006cecb0 */
    i32 net_civ_phase_done[11];  /* 006ced20 */
    iptr adjacent_foreign_civ;  /* 006ced4c */
    int unit_nearest_dist;  /* 006ced50 */
    u8 unit_nearest_dist_rest[12];
    i8 path_cost_grid[96];  /* 006ced60 */
    u8 path_grid_costs[9120];  /* 006cedc0 */
};
#pragma pack(pop)
extern struct dialog_text_font_obj_block dialog_text_font_obj_block;
extern MapHeader map;
extern u8 map_rest[2];
/* The memory handles of the map's tile and coarse arrays, and the tile returned for coordinates off the map. */
#pragma pack(push, 1)
typedef struct MapHandles {
    void *tiles;  /* 006d1170 */
    void *coarse[4];  /* 006d1174 */
    u8 coarse_rest[4];
    Tile offmap_tile;  /* 006d1188 */
    u8 offmap_tile_rest[2];
} MapHandles;
#pragma pack(pop)
extern MapHandles map_handles;

#endif
