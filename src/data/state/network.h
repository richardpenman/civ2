/* Network play: the session, its messages, the difference engine, the parley and chat.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_NETWORK_H
#define CIV2_STATE_NETWORK_H

extern RODATA i8 gamelist_map_size_sep_str[4];
extern RODATA i8 gamelist_double_opts_sep_str[4];
extern RODATA i8 gamelist_timer_colon_str[4];
extern RODATA i8 gamelist_open_paren_str[4];
extern u8 str_empty_23[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_24[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_25[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_26[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_27[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_28[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_29[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_30[4];  /* only net_gamelist_show_game_info uses it */
extern u8 str_empty_31[4];  /* only net_gamelist_show_game_info uses it */
extern i8 *str_ptr_5_4_0f_multiplayer_26_march_99;
extern u8 str_ptr_5_4_0f_multiplayer_26_march_99_rest[4];
/* The interface labels (LABELS.TXT): string ids by label number. */
#pragma pack(push, 1)
typedef struct LabelTable {
    int ids_handle;  /* 0062841c */
    i32 *string_ids;  /* 00628420 */
    i32 count;  /* 00628424 */
} LabelTable;
#pragma pack(pop)
extern LabelTable label_table;
extern int net_send_sequence;
extern u8 net_send_sequence_rest[4];
extern u8 str_empty_80[4];  /* only net_send_message uses it */
extern u8 str_empty_81[4];  /* only NetServerInfoMsg::init uses it */
extern char net_deferred_input_busy;
extern u8 net_deferred_input_busy_rest[3];
/* The chat macros (Ctrl+F1..F5): next civ for the name substitutions, file offsets. */
#pragma pack(push, 1)
typedef struct ChatMacros {
    int next_adj_civ;  /* 0062c990 */
    int next_leader_civ;  /* 0062c994 */
    int macro1_offset;  /* 0062c998 */
    int macro2_offset;  /* 0062c99c */
    int macro3_offset;  /* 0062c9a0 */
} ChatMacros;
#pragma pack(pop)
extern ChatMacros chat_macros;
/* The difference engine (network game state sync): mirror buffer and packet counts. */
#pragma pack(push, 1)
typedef struct DiffEngState {
    iptr mirror_buf;  /* 0062d0bc */
    int packet_flush_size;  /* 0062d0c0 */
    i32 send_count;  /* 0062d0c4 */
} DiffEngState;
#pragma pack(pop)
extern DiffEngState diffeng;
/* The parley window: the kinds of list in its left and right panes per mode. */
#pragma pack(push, 1)
typedef struct ParleyPaneKinds {
    i32 mode_left_kinds[1];  /* 0062d7d0 */
    i32 mode_right_kinds[3];  /* 0062d7d4 */
    i32 mode2_left_kind;  /* 0062d7e0 */
    i32 mode2_right_kind;  /* 0062d7e4 */
    u32 mode3_left_kind;  /* 0062d7e8 */
    u32 mode3_right_kind;  /* 0062d7ec */
    u8 mode3_right_kind_rest[8];
    u32 mode5_left_kind;  /* 0062d7f8 */
    u32 mode5_right_kind;  /* 0062d7fc */
    u8 mode5_right_kind_rest[88];
} ParleyPaneKinds;
#pragma pack(pop)
extern ParleyPaneKinds parley_panes;
/* Margins of the network game's panels (parley, chat). */
#pragma pack(push, 1)
typedef struct PanelLayout {
    int margin_x;  /* 0062d858 */
    int margin_y;  /* 0062d85c */
    int padding_x;  /* 0062d860 */
    int bevel_width;  /* 0062d864 */
    int padding_y;  /* 0062d868 */
} PanelLayout;
#pragma pack(pop)
extern PanelLayout panel_layout;
/* The chat window's text. */
#pragma pack(push, 1)
typedef struct ChatBuffers {
    void *input_buffer;  /* 0062d86c */
    char *log_buffer;  /* 0062d870 */
} ChatBuffers;
#pragma pack(pop)
extern ChatBuffers chat;
extern RODATA i8 chat_show_size_default_str[8];
extern i8 parley_wnd_empty_title_str[4];  /* only ParleyWindow::create uses it */
extern RODATA i8 chat_recv_newline_str[4];
extern RODATA i8 chat_recv_private_year_open_str[4];
extern RODATA i8 chat_recv_private_year_close_str[4];
extern RODATA i8 chat_recv_year_open_str[4];
extern RODATA i8 chat_recv_year_close_str[4];
extern RODATA i8 chat_send_newline_str[4];
extern RODATA i8 chat_send_recipient_sep_str[4];
extern RODATA i8 chat_send_year_open_str[4];
extern RODATA i8 chat_send_year_close_str[124];
extern RODATA i32 parley_offer_kinds[16];
extern RODATA i32 parley_item_header_sizes[21];
extern RODATA i8 parley_desc_transmitted_crlf_str[8];
extern RODATA i8 parley_desc_maps_offer_crlf_str[8];
extern RODATA i8 parley_desc_maps_kind_crlf_str[4];
extern RODATA i8 parley_desc_war_offer_crlf_str[8];
extern RODATA i8 parley_desc_war_head_crlf_str[4];
extern RODATA i8 parley_desc_gift_adv_crlf_str[8];
extern RODATA i8 parley_desc_gift_adv_head_crlf_str[4];
extern RODATA i8 parley_desc_gift_units_crlf_str[8];
extern RODATA i8 parley_desc_gift_units_head_crlf_str[4];
extern RODATA i8 parley_desc_gift_gold_crlf_str[8];
extern RODATA i8 parley_desc_gift_gold_head_crlf_str[4];
extern RODATA i8 parley_desc_gift_cities_crlf_str[8];
extern RODATA i8 parley_desc_gift_cities_head_crlf_str[4];
extern RODATA i8 parley_desc_gift_maps_crlf_str[8];
extern RODATA i8 parley_desc_threat_crlf_str[8];
extern RODATA i8 parley_desc_barter_crlf_str[8];
extern RODATA i8 parley_desc_give_adv_crlf_str[4];
extern RODATA i8 parley_desc_give_gold_crlf_str[4];
extern RODATA i8 parley_desc_give_units_crlf_str[4];
extern RODATA i8 parley_desc_give_cities_crlf_str[4];
extern RODATA i8 parley_desc_give_maps_crlf_str[4];
extern RODATA i8 parley_desc_give_war_crlf_str[4];
extern RODATA i8 parley_desc_give_treaty_crlf_str[4];
extern RODATA i8 parley_desc_barter_sep_crlf_str[4];
extern RODATA i8 parley_desc_want_adv_crlf_str[4];
extern RODATA i8 parley_desc_want_gold_crlf_str[4];
extern RODATA i8 parley_desc_want_units_crlf_str[4];
extern RODATA i8 parley_exch_for_cities_crlf_str[4];
extern RODATA i8 parley_exch_for_maps_crlf_str[4];
extern RODATA i8 parley_exch_for_war_targets_crlf_str[4];
extern RODATA i8 parley_exch_for_treaty_crlf_str[4];
extern RODATA i8 parley_advances_comma_str[4];
extern RODATA i8 parley_advances_period_str[4];
extern RODATA i8 parley_advances_crlf_str[4];
extern RODATA i8 parley_gold_crlf_str[4];
extern RODATA i8 parley_units_open_paren_str[4];
extern RODATA i8 parley_units_close_paren_str[4];
extern RODATA i8 parley_units_comma_str[4];
extern RODATA i8 parley_units_period_str[4];
extern RODATA i8 parley_units_crlf_str[4];
extern RODATA i8 parley_cities_open_paren_str[4];
extern RODATA i8 parley_cities_close_paren_str[4];
extern RODATA i8 parley_cities_comma_str[4];
extern RODATA i8 parley_cities_period_str[4];
extern RODATA i8 parley_cities_crlf_str[4];
extern RODATA i8 parley_maps_crlf_str[4];
extern RODATA i8 parley_war_targets_comma_str[4];
extern RODATA i8 parley_war_targets_period_str[4];
extern RODATA i8 parley_war_targets_crlf_str[4];
extern RODATA i8 parley_treaty_crlf_str[212];
/* Network messages waiting to be shown. */
#pragma pack(push, 1)
typedef struct NetPopupQueue {
    int head;  /* 00631130 */
    i32 tail;  /* 00631134 */
} NetPopupQueue;
#pragma pack(pop)
extern NetPopupQueue net_popups;
/* The parley's reveal prompts open. */
#pragma pack(push, 1)
typedef struct ParleyPrompts {
    int units_open;  /* 00631138 */
    int city_open;  /* 0063113c */
} ParleyPrompts;
#pragma pack(pop)
extern ParleyPrompts parley_prompts;
extern u8 str_empty_167[4];  /* only ParleyWindow::add_chat_pane uses it */
/* The network game's turn timer. */
#pragma pack(push, 1)
typedef struct MpTurnTimer {
    int handle;  /* 00633a74 */
    int seconds_left;  /* 00633a78 */
    i32 last_tick;  /* 00633a7c */
    int mouse_watchdog;  /* 00633a80 */
} MpTurnTimer;
#pragma pack(pop)
extern MpTurnTimer mp_timer;
/* Dragging a window during a network game. */
#pragma pack(push, 1)
typedef struct MpWindowDrag {
    char active;  /* 00633a84 */
    u8 active_rest[3];
    i32 start_ticks;  /* 00633a88 */
    i32 flag;  /* 00633a8c */
    i32 wnd;  /* 00633a90 */
} MpWindowDrag;
#pragma pack(pop)
extern MpWindowDrag mp_drag;
extern RODATA i8 lobby_ip_sep_str[4];
extern int net_last_recv_msg_type;  /* only net_on_secure_receive uses it */
extern RODATA i8 net_internet_timeout_default[4];
extern RODATA i8 net_tcpip_timeout_default[4];
extern RODATA i8 net_ipxspx_timeout_default[4];
extern RODATA i8 net_modem_timeout_default[4];
extern RODATA i8 net_direct_timeout_default[4];
extern RODATA i8 net_hexdump_space_str[4];
/* The dialogs' fonts, colors and scale, and the next dialog's callbacks; the dialog stack. */
#pragma pack(push, 1)
typedef struct DialogStyle {
    i8 *default_text_font;  /* 006359e4 */
    u8 default_text_font_rest[12];
    i8 frame_color;  /* 006359f4 */
    u8 frame_color_rest[3];
    i8 text_shadow_color;  /* 006359f8 */
    u8 text_shadow_color_rest[3];
    i8 text_color;  /* 006359fc */
    u8 text_color_rest[3];
    byte highlight_text_color;  /* 00635a00 */
    u8 highlight_text_color_rest[3];
    byte bg_color;  /* 00635a04 */
    u8 bg_color_rest[3];
    byte bevel_light_color;  /* 00635a08 */
    u8 bevel_light_color_rest[3];
    byte bevel_dark_color;  /* 00635a0c */
    u8 bevel_dark_color_rest[3];
    i8 title_text_color;  /* 00635a10 */
    u8 title_text_color_rest[3];
    i8 title_shadow_color;  /* 00635a14 */
    u8 title_shadow_color_rest[3];
    i32 list_bg;  /* 00635a18 */
    i32 list_text;  /* 00635a1c */
    int list_shadow;  /* 00635a20 */
    i32 list_sel_bg;  /* 00635a24 */
    int sel_text_color;  /* 00635a28 */
    int sel_text_shadow_color;  /* 00635a2c */
    i32 default_list_color;  /* 00635a30 */
    i32 scale_num;  /* 00635a34 */
    i32 scale_den;  /* 00635a38 */
    WndNotifyFn pending_idle_callback;  /* 00635a3c the next dialog's modal idle callback */
    WndNotifyFn default_idle_callback;  /* 00635a40 every dialog's, when no pending one */
    WndMouseFn pending_move_handler;  /* 00635a44 the next dialog's move handler */
    i8 *button_labels[4];  /* 00635a48 */
    i32 stack[17];  /* 00635a58 */
    iptr stack_depth;  /* 00635a9c */
    int frame_tile_surface;  /* 00635aa0 */
    int client_tile_surface;  /* 00635aa4 */
} DialogStyle;
#pragma pack(pop)
extern DialogStyle dlg_style;
/* The batch of network records being assembled. */
#pragma pack(push, 1)
typedef struct NetBatch {
    int used_dwords;  /* 006365f4 */
    i32 record_sizes[8];  /* 006365f8 */
} NetBatch;
#pragma pack(pop)
extern NetBatch net_batch;
extern u8 popup_info_icon_sprite[64];
extern i32 attitude_names[9];
extern u8 attitude_names_rest[4];
#pragma pack(push, 1)
struct civ_unit_counts_block {
    union {
        struct {
            i8 civ_unit_counts[25];  /* 0064b9e8 */
            i8 wonder_expires_by_impr[15];  /* 0064ba01 (wonder_expires - 0x27) */
        };
        i32 civ_unit_totals[8];  /* 0064b9e8 units per civ */
    };
    i32 difficulty_names[6];  /* 0064ba10 */
    i8 wonder_expires[28];  /* 0064ba28 */
    u8 wonder_expires_rest[4];
};
#pragma pack(pop)
extern struct civ_unit_counts_block civ_unit_counts_block;
extern i32 *const difficulty_names;  /* = civ_unit_counts_block.difficulty_names */
extern i8 *const wonder_expires;  /* = civ_unit_counts_block.wonder_expires */
#pragma pack(push, 1)
typedef struct net_map_lock_slots_block {
    union {
        struct {
            i8 net_map_lock_slots[4];  /* 0064ba48 */
            i8 net_map_lock_replies[4];  /* 0064ba4c */
            i8 net_map_lock_src_x[4];  /* 0064ba50 */
            i8 net_map_lock_src_y[4];  /* 0064ba54 */
            i8 net_map_lock_dst_x[4];  /* 0064ba58 */
            i8 net_map_lock_dst_y[172];  /* 0064ba5c */
        };
        NetMapLock net_map_locks[8];  /* 0064ba48 per civ */
    };
    i8 scenario_dir_path[264];  /* 0064bb08 */
} NetMapLockBlock;
#pragma pack(pop)
extern struct net_map_lock_slots_block net_map_lock_slots_block;
#pragma pack(push, 1)
struct diffeng_sections_block {
    i8 diffeng_sections[4];  /* 0067a410 */
    i32 diffeng_section_padded_sizes[2];  /* 0067a414 */
    i8 diffeng_section_checksums[4];  /* 0067a41c */
    i8 diffeng_section_offsets[4];  /* 0067a420 */
    iptr diffeng_section_ptrs[1];  /* 0067a424 */
};
#pragma pack(pop)
extern struct diffeng_sections_block diffeng_sections_block;
extern DiffEngSection *const diffeng_section_table;  /* = diffeng_sections_block: the 0x18 sections, one after the other (diffeng_section_init) */
extern GameFont font_size21;
extern GameFont font_size14;
extern ParleyWindow parley_wnd_block;  /* the diplomacy / chat window object */
extern GameFont font_size16;
extern GameFont font_size24;
extern ParleyWindow chat_wnd_block;  /* the diplomacy / chat window object */
extern GameFont font_size30;
extern GameFont font_size20;
extern GameFont font_size24_bold;
#pragma pack(push, 1)
struct font_size16_bold_block {
    u8 font_size16_bold[12];  /* 0069b030 */
    int chat_log_capacity;  /* 0069b03c */
    u8 chat_log_capacity_rest[26660];
    struct WonderMovieWindow *wonder_movie_wnd;  /* 006a1864 */
    u8 wonder_movie_wnd_rest[8];
    uptr net_bribe_answer;  /* 006a1870 */
    u8 net_bribe_answer_rest[12];
    RulesEditSection rules_edit_sections[8];  /* 006a1880 */
};
#pragma pack(pop)
extern struct font_size16_bold_block font_size16_bold_block;
#pragma pack(push, 1)
struct text_section_index_pool_block {
    i8 text_section_index_pool[8];  /* 006a5b38 */
    char *text_section_index_base;  /* 006a5b40 */
    u8 text_section_index_base_rest[4];
    ushort text_section_pool_free_bytes;  /* 006a5b48 */
    u8 text_section_pool_free_bytes_rest[14];
    i8 parley_description[2464];  /* 006a5b58 */
};
#pragma pack(pop)
extern struct text_section_index_pool_block text_section_index_pool_block;
#pragma pack(push, 1)
/* The network session object (the original's global C++ object; its methods
 * take it as `self`). */
typedef struct NetSession {
    NetGamelistView *gamelist_view;  /* 006ad108 */
    LobbyList *net_lobby_list;  /* 006ad10c */
    union {
        struct {
            u8 ping_message[16];  /* 006ad110 */
            i32 player_info_civ;  /* 006ad120 */
            i32 player_info_leader_index;  /* 006ad124 */
            i32 player_info_gender;  /* 006ad128 */
            i32 player_info_has_names;  /* 006ad12c */
            i8 player_info_leader_name[24];  /* 006ad130 */
            i8 player_info_tribe_name[24];  /* 006ad148 */
            i8 player_info_adjective[24];  /* 006ad160 */
            i32 player_info_has_titles;  /* 006ad178 */
            i8 player_info_govt_titles[168];  /* 006ad17c */
            int player_info_city_style;  /* 006ad224 */
        };
        NetPlayerInfoMsg player_info_msg;  /* 006ad110 the local player's */
    };
    int lobby_launch_mode;  /* 006ad228 */
    i8 lobby_launch_info[32];  /* 006ad22c */
    u8 startup_log_text[64];  /* 006ad24c */
    i8 lobby_short_name[32];  /* 006ad28c */
    i8 lobby_session_name[64];  /* 006ad2ac */
    iptr lobby_is_host;  /* 006ad2ec */
    int connection_type;  /* 006ad2f0 */
    i8 connected;  /* 006ad2f4 */
    i8 connected_to_server;  /* 006ad2f5 */
    i8 listing_games;  /* 006ad2f6: game announcements are collected (net_game_list_add) */
    char host_flag;  /* 006ad2f7 */
    i32 net_protocol;  /* 006ad2f8 */
    uint adapter_index;  /* 006ad2fc */
    int server_conn_id;  /* 006ad300 */
    int local_slot;  /* 006ad304 */
    iptr net_player_count[1];  /* 006ad308 */
    union {
        struct {
            i32 player_slots[1];  /* 006ad30c */
            i8 slot_addresses[32];  /* 006ad310 */
            i8 slot_player_names[36];  /* 006ad330 */
            i8 slot_lobby_ids[4];  /* 006ad354 */
            i8 slot_host_flags[1];  /* 006ad358 */
            i8 slot_active_flags[3];  /* 006ad359 */
            i32 slot_civs[127];  /* 006ad35c */
        };
        NetPlayerSlot net_slots[7];  /* 006ad30c */
    };
    i32 net_civ_slots[8];  /* 006ad558 */
    i32 mp_turn_civ;  /* 006ad578 */
    i8 net_server_address[32];  /* 006ad57c */
    i8 net_game_name[32];  /* 006ad59c */
    i8 local_player_name[32];  /* 006ad5bc */
    i8 host_player_name[32];  /* 006ad5dc */
    i8 net_server_version[64];  /* 006ad5fc */
    i32 connection_title;  /* 006ad63c */
    i32 session_phase;  /* 006ad640 */
    i32 civ_new_player_flags[8];  /* 006ad644 */
    int players_awaited;  /* 006ad664 */
    int players_awaited_initial;  /* 006ad668 */
    int vote_replies_pending;  /* 006ad66c */
    int vote_status;  /* 006ad670 */
    i32 wait_start_ticks;  /* 006ad674 */
    int *dialog_stack_top;  /* 006ad678 */
    i32 popup_in_progress;  /* 006ad67c */
    int last_send_result;  /* 006ad680 */
    char mp_simultaneous_moves;  /* 006ad684 */
    char player_has_quit;  /* 006ad685 */
    u8 player_has_quit_rest[2];
    int poll_depth;  /* 006ad688 */
    int poll_depth_peak;  /* 006ad68c */
    u32 msgq_normal_peak;  /* 006ad690 */
    u32 msgq_priority_peak;  /* 006ad694 */
    char host_handoff_stage;  /* 006ad698 */
    i8 tile_sync_enabled;  /* 006ad699 */
    char tile_batch_active;  /* 006ad69a */
    u8 tile_batch_active_rest[1];
    i32 chat_missed_message_count;  /* 006ad69c */
    i32 chat_unread_count;  /* 006ad6a0 */
    i32 move_anim_sound;  /* 006ad6a4 */
    i32 move_anim_sound_arg;  /* 006ad6a8 */
    i16 loaded_file_kind;  /* 006ad6ac */
    i8 loaded_file_name[260];  /* 006ad6ae */
    i8 loaded_file_subdir[262];  /* 006ad7b2 */
    int timeout_seconds;  /* 006ad8b8 */
    int unit_create_busy;  /* 006ad8bc */
    int unit_delete_busy;  /* 006ad8c0 */
    int city_create_busy;  /* 006ad8c4 */
    int city_delete_busy;  /* 006ad8c8 */
    int lock_map_busy;  /* 006ad8cc */
    int unlock_map_busy;  /* 006ad8d0 */
    int ui_input_busy;  /* 006ad8d4 */
    int unit_stack_unlink_busy;  /* 006ad8d8 */
    int unit_stack_link_busy;  /* 006ad8dc */
    int unit_stack_bottom_busy;  /* 006ad8e0 */
    int unit_relocate_busy;  /* 006ad8e4 */
    int city_acquire_wonder_busy;  /* 006ad8e8 */
    int civ_eliminate_busy;  /* 006ad8ec */
    int civ_create_busy;  /* 006ad8f0 */
    int civ_check_destroyed_busy;  /* 006ad8f4 */
    int unit_gather_cargo_busy;  /* 006ad8f8 */
    int unit_lift_off_map_busy;  /* 006ad8fc */
    int unit_delete_cargo_busy;  /* 006ad900 */
    int unit_destroy_redraw_busy;  /* 006ad904 */
    i32 map_redraw_busy;  /* 006ad908 */
    uint stacked_draw_count;  /* 006ad90c */
    int stacked_draw_first;  /* 006ad910 */
    int stacked_draw_last;  /* 006ad914 */
    int unit_stack_check_enabled;  /* 006ad918 */
    uint stacked_draw_peak;  /* 006ad91c */
    union {
        struct {
            i8 stacked_draw_kinds[4];  /* 006ad920 */
            i8 stacked_draw_args[4];  /* 006ad924 */
            i8 stacked_draw_arg1[4];  /* 006ad928 */
            i8 stacked_draw_arg2[4];  /* 006ad92c */
            i8 stacked_draw_arg3[4];  /* 006ad930 */
            i8 stacked_draw_arg4[6380];  /* 006ad934 */
        };
        NetStackedDraw stacked_draw[100];  /* 006ad920 */
    };
    i32 casualty_counts[8];  /* 006af220 */
    i32 casualty_reported_counts[8];  /* 006af240 */
    i32 casualty_log_first[8];  /* 006af260 */
    i32 casualty_log_next[8];  /* 006af280 */
    union {
        struct {
            i8 casualty_log_unit_types[2];  /* 006af2a0 */
            i8 casualty_log_x[2];  /* 006af2a2 */
            i8 casualty_log_y[2];  /* 006af2a4 */
            i8 casualty_log_years[2];  /* 006af2a6 */
            i8 casualty_log_killer_colors[25];  /* 006af2a8 */
            i8 casualty_log_killer_name_ends[81567];  /* 006af2c1 */
        };
        CasualtyEntry casualty_log[2400];  /* 006af2a0 [civ * 300 + i] */
    };
    int ini_simultaneous_allowed;  /* 006c3160 */
    int net_max_players;  /* 006c3164 */
    i32 civ_turn_ended[8];  /* 006c3168 */
    i32 civ_drop_requested[8];  /* 006c3188 */
    byte pending_human_players;  /* 006c31a8 */
    u8 hotseat_human_players;  /* 006c31a9 */
    u8 hotseat_human_players_rest[2];
    i32 blocked_input_kind;  /* 006c31ac */
    i32 blocked_input_arg1;  /* 006c31b0 */
    i32 blocked_input_arg2;  /* 006c31b4 */
    i32 blocked_input_arg3;  /* 006c31b8 */
    i32 blocked_input_arg4;  /* 006c31bc */
    i32 blocked_input_arg5;  /* 006c31c0 */
    i32 blocked_input_char_pending;  /* 006c31c4 */
    char blocked_input_char;  /* 006c31c8 */
    u8 blocked_input_char_rest[3];
    i32 blocked_input_window;  /* 006c31cc */
    i8 drag_hold;  /* 006c31d0 */
    u8 drag_hold_rest[3];
    NetGameInfo *found_games;  /* 006c31d4 */
    union {
        struct {
            i8 net_msg_queue[24008];  /* 006c31d8 */
            int msgq_normal_count;  /* 006c8fa0 */
            u8 msgq_normal_count_rest[8];
            int msgq_priority_count;  /* 006c8fac */
        };
        NetMsgQueue msg_queue;  /* 006c31d8 */
    };
    ulong last_recv_callback_size;  /* 006c8fb0 */
    int lost_conn_total;  /* 006c8fb4 */
    ulong last_recv_msg_size;  /* 006c8fb8 */
    uint xd_adapter_index;  /* 006c8fbc */
    i32 lost_conn_by_player[8];  /* 006c8fc0 */
    union {
        struct {
            i8 nm_recv_counts[16];  /* 006c8fe0 */
            int nm_player_list_count;  /* 006c8ff0 */
            int nm_server_reject_count;  /* 006c8ff4 */
            u8 nm_server_reject_count_rest[4];
            int nm_full_gamestate_count;  /* 006c8ffc */
            u8 nm_full_gamestate_count_rest[4];
            int nm_join_gamestate_count;  /* 006c9004 */
            u8 nm_join_gamestate_count_rest[4];
            int nm_server_canceled_count;  /* 006c900c */
            int nm_lost_server_on_join_count;  /* 006c9010 */
            u8 nm_lost_server_on_join_count_rest[8];
            i32 nm_server_quit_count;  /* 006c901c */
            u8 nm_server_quit_count_rest[4];
            int nm_server_full_count;  /* 006c9024 */
            u8 nm_server_full_count_rest[4];
            int nm_server_answer_count;  /* 006c902c */
            u8 nm_server_answer_count_rest[8];
            int nm_start_civ_count;  /* 006c9038 */
            u8 nm_start_civ_count_rest[60];
            int nm_nation_accept_count;  /* 006c9078 */
            int nm_nation_reject_count;  /* 006c907c */
            u8 nm_nation_reject_count_rest[8];
            int nm_lock_data_count;  /* 006c9088 */
            i32 nm_heart_beat_count;  /* 006c908c */
            int nm_heartbeat_ack_count;  /* 006c9090 */
            u8 nm_heartbeat_ack_count_rest[12];
            i32 nm_lock_nation_count;  /* 006c90a0 */
            u8 nm_lock_nation_count_rest[16];
            int kill_or_quit_ack;  /* 006c90b4 */
            u8 kill_or_quit_ack_rest[8];
            i32 unit_deleted_ack;  /* 006c90c0 */
            u8 unit_deleted_ack_rest[4];
            int city_deleted_ack;  /* 006c90c8 */
            u8 city_deleted_ack_rest[4];
            uptr nm_city_created_result;  /* 006c90d0 */
            u8 nm_city_created_result_rest[4];
            uptr nm_unit_created_result;  /* 006c90d8 */
            u8 nm_unit_created_result_rest[4];
            i32 nm_unit_picked_up_count;  /* 006c90e0 */
            u8 nm_unit_picked_up_count_rest[4];
            i32 nm_unit_put_down_count;  /* 006c90e8 */
            u8 nm_unit_put_down_count_rest[4];
            i32 nm_bottomed_count;  /* 006c90f0 */
            u8 nm_bottomed_count_rest[4];
            i32 nm_unit_relocated_count;  /* 006c90f8 */
            u8 nm_unit_relocated_count_rest[4];
            int nm_wonder_acquired_result;  /* 006c9100 */
            u8 nm_wonder_acquired_result_rest[4];
            uptr nm_ship_stacked_result;  /* 006c9108 */
            u8 nm_ship_stacked_result_rest[4];
            i32 nm_unit_stacked_count;  /* 006c9110 */
            u8 nm_unit_stacked_count_rest[4];
            i32 nm_safely_deleted_count;  /* 006c9118 */
            u8 nm_safely_deleted_count_rest[4];
            i32 nm_visibly_deleted_count;  /* 006c9120 */
            u8 nm_visibly_deleted_count_rest[40];
            i32 nm_stacker_ack_count;  /* 006c914c */
            u8 nm_stacker_ack_count_rest[20];
            int nm_new_civ_ack_result;  /* 006c9164 */
            int nm_kill_civ_ack_result;  /* 006c9168 */
            u8 nm_kill_civ_ack_result_rest[12];
            i32 nm_continue_after_join_count;  /* 006c9178 */
            u8 nm_continue_after_join_count_rest[16];
            int nm_end_sequence_count;  /* 006c918c */
            u8 nm_end_sequence_count_rest[84];
            int nm_parley_cancel_count;  /* 006c91e4 */
            int nm_parley_transaction_count;  /* 006c91e8 */
            int nm_parley_accept_offer_count;  /* 006c91ec */
            int nm_parley_no_thanks_count;  /* 006c91f0 */
            int nm_parley_counter_offer_count;  /* 006c91f4 */
            u8 nm_parley_counter_offer_count_rest[20];
            int nm_production_begin_count;  /* 006c920c */
            int nm_production_complete_count;  /* 006c9210 */
            int nm_ai_movement_count;  /* 006c9214 */
            int nm_human_movement_count;  /* 006c9218 */
            int nm_human_move_complete_count;  /* 006c921c */
            u8 nm_human_move_complete_count_rest[48];
            int nm_ready_to_start_count;  /* 006c9250 */
            u8 nm_ready_to_start_count_rest[24];
            int nm_abort_kaboom_count;  /* 006c926c */
            u8 nm_abort_kaboom_count_rest[20];
        };
        i32 nm_recv_counts_by_type[169];  /* 006c8fe0 messages received, by type */
    };
    int last_queued_send_seq;  /* 006c9284 */
    i32 active_protocol;  /* 006c9288 */
    u8 active_protocol_rest[23020];
    /* methods */
    i8 is_host();    /* 00421f40 */
    i8 send_to(int player,NetMsg *msg);    /* 0046af70 */
    bool send_to_all(NetMsg *msg);    /* 0046b0a1 */
    void process_messages(int max_msgs,int wait_mode);    /* 0047e94e */
    struct MsgArgs;    /* part of process_messages */
    void run_blocked_input();    /* part of process_messages */
    void dispatch_message(int *&msg,int *msg_hdr,uint *payload,int sender,uint msg_size,char &gamelist_dirty,MsgArgs &a);    /* part of process_messages */
    void recv_hello(int *msg,char &gamelist_dirty);    /* part of process_messages */
    void recv_server_info(int *msg,int *msg_hdr);    /* part of process_messages */
    void recv_state_diff(int *msg,uint *payload,MsgArgs &a);    /* part of process_messages */
    void recv_join_request(int *msg_hdr,int sender);    /* part of process_messages */
    void recv_player_node(int *msg,int sender);    /* part of process_messages */
    void recv_claim_civ(int *msg,int sender,MsgArgs &a);    /* part of process_messages */
    void recv_stack_relocate(int *msg,int sender,MsgArgs &a);    /* part of process_messages */
    void recv_unit_bribed(int *msg,MsgArgs &a);    /* part of process_messages */
    void recv_wait_game_xmit();    /* part of process_messages */
    void recv_turn_civ(int *msg_hdr);    /* part of process_messages */
    void recv_popup_request(int *msg,int sender,MsgArgs &a);    /* part of process_messages */
    void recv_turn_time_limit(int *msg,MsgArgs &a);    /* part of process_messages */
    void recv_war_pretext(int *msg,MsgArgs &a);    /* part of process_messages */
    void recv_parley_cities_hidden(int *msg,MsgArgs &a);    /* part of process_messages */
    void recv_parley_cities_shown(int *msg,MsgArgs &a);    /* part of process_messages */
    void recv_parley_units_hidden(int *msg,MsgArgs &a);    /* part of process_messages */
    void recv_parley_units_shown(int *msg,MsgArgs &a);    /* part of process_messages */
    NetSession *construct();    /* 0059a8bb */
    void clear();    /* 0059a998 */
    void destruct();    /* 0059ad40 */
    i8 init_transport(i32 protocol,int is_server);    /* 0059adef */
    void shutdown(int delay_after);    /* 0059b293 */
    void close_hook_nop();    /* 0059b55b */
    void game_list_add(NetServerInfoMsg *msg);    /* 0059b571 */
    void add_player(NetPlayerNodeMsg *player_info);    /* 0059b7fc */
    void remove_player(int player_id);    /* 0059b96a */
    void free_game_list();    /* 0059baf0 */
} NetSession;
#pragma pack(pop)
extern NetSession net;
extern i32 net_batch_buffer[772];

#endif
