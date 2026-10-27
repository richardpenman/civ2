/* The game's static data (the original's .rdata and .data): one variable
 * per item, in the original's address order (tools/unglobal.py made them
 * from the single struct that kept the original layout). */
#ifndef CIV2_GAMEDATA_H
#define CIV2_GAMEDATA_H

#include "civ2types.h"

extern pointer ptr_fun;
extern u8 ptr_fun_rest[16];
#pragma pack(push, 1)
struct throneroom_piece_rect_x_block {
    i8 throneroom_piece_rect_x[4];  /* 0061c068 */
    i8 throneroom_piece_rect_y[4];  /* 0061c06c */
    i8 throneroom_piece_rect_w[4];  /* 0061c070 */
    i8 throneroom_piece_rect_h[180];  /* 0061c074 */
    i8 throneroom_piece3_rect_x[4];  /* 0061c128 */
    i8 throneroom_piece3_rect_y[60];  /* 0061c12c */
};
#pragma pack(pop)
extern struct throneroom_piece_rect_x_block throneroom_piece_rect_x_block;
#pragma pack(push, 1)
struct throneroom_piece4_rect_x_block {
    i8 throneroom_piece4_rect_x[4];  /* 0061c168 */
    i8 throneroom_piece4_rect_y[4];  /* 0061c16c */
    i8 throneroom_piece4_rect_w[4];  /* 0061c170 */
    i8 throneroom_piece4_rect_h[180];  /* 0061c174 */
    i8 throneroom_piece7_rect_x[4];  /* 0061c228 */
    i8 throneroom_piece7_rect_y[4];  /* 0061c22c */
    i8 throneroom_piece7_rect_w[4];  /* 0061c230 */
    i8 throneroom_piece7_rect_h[52];  /* 0061c234 */
    i32 throneroom_bonus_rect_x[1];  /* 0061c268 */
    i32 throneroom_bonus_rect_y[1];  /* 0061c26c */
    i32 throneroom_bonus_rect_w[1];  /* 0061c270 */
    i32 throneroom_bonus_rect_h[9];  /* 0061c274 */
    int throneroom_bonus3_rect_x;  /* 0061c298 */
    int throneroom_bonus3_rect_y;  /* 0061c29c */
    i32 throneroom_bonus3_rect_w;  /* 0061c2a0 */
    i32 throneroom_bonus3_rect_h;  /* 0061c2a4 */
    u8 throneroom_bonus3_rect_h_rest[172];
    i8 *str_ptr_microprose_software;  /* 0061c354 */
    u8 str_ptr_microprose_software_rest[8];
};
#pragma pack(pop)
extern struct throneroom_piece4_rect_x_block throneroom_piece4_rect_x_block;
extern ThroneRect (*const throneroom_piece_rects)[4];  /* = throneroom_piece_rect_x_block: [piece][level], pieces 0-7 then the bonus rects (8), across both blocks */
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
extern char str_empty[908];
extern pointer ptr_fun_2;
extern i8 tribe_leader_sound_set[24];
#pragma pack(push, 1)
struct leader_video_x_table_block {
    i32 leader_video_pos[21][2];  /* 0061d200 per tribe: x, y of the leader video */
};
#pragma pack(pop)
extern struct leader_video_x_table_block leader_video_x_table_block;
#pragma pack(push, 1)
struct leader_anim_frame_starts_block {
    i32 leader_anim_frames[26][5][2];  /* 0061d2a8 per tribe, per reaction (1 idle): first, last frame */
    u8 leader_anim_frames_rest[8];
    pointer ptr_fun_3[15];  /* 0061d6c0 */
    pointer ptr_fun_4;  /* 0061d6fc */
    u8 ptr_fun_4_rest[12];
    pointer ptr_fun_5;  /* 0061d70c */
    u8 ptr_fun_5_rest[8];
    pointer ptr_fun_6[93];  /* 0061d718 */
};
#pragma pack(pop)
extern struct leader_anim_frame_starts_block leader_anim_frame_starts_block;
extern char str_illegal_byte_sequence[24];
extern char str_directory_not_empty[20];
extern char str_function_not_implemented[28];
extern char str_no_locks_available[20];
extern char str_filename_too_long[20];
extern char str_resource_deadlock_avoided[28];
extern char str_result_too_large[20];
extern char str_domain_error[16];
extern char str_broken_pipe[12];
extern char str_too_many_links[16];
extern char str_read_only_file_system[24];
extern char str_invalid_seek[16];
extern char str_no_space_left_on_device[24];
extern char str_file_too_large[16];
extern char str_inappropriate_i_o_control_operation[36];
extern char str_too_many_open_files[20];
extern char str_too_many_open_files_in_system[32];
extern char str_invalid_argument[20];
extern char str_is_a_directory[16];
extern char str_not_a_directory[16];
extern char str_no_such_device[16];
extern char str_improper_link[16];
extern char str_file_exists[12];
extern char str_resource_device[16];
extern char str_unknown_error[16];
extern char str_bad_address[12];
extern char str_permission_denied[20];
extern char str_not_enough_space[20];
extern char str_resource_temporarily_unavailable[36];
extern char str_no_child_processes[20];
extern char str_bad_file_descriptor[20];
extern char str_exec_format_error[20];
extern char str_arg_list_too_long[20];
extern char str_no_such_device_or_address[28];
extern char str_input_output_error[20];
extern char str_interrupted_function_call[28];
extern char str_no_such_process[16];
extern char str_no_such_file_or_directory[28];
extern char str_operation_not_permitted[24];
extern char str_no_error[12];
extern char str_client[8];
extern char str_ignore[12];
extern char str_normal[1828];
extern char str_empty_2[4];
extern char str_empty_3[4];
extern char str_empty_4[428];
extern char str_assertion_failed[20];
extern char str_error[8];
extern char str_warning[612];
extern char str_empty_5[16];
extern char str_null[316];
extern char str_runtime_error[20];
extern char str_tloss_error[16];
extern char str_sing_error[16];
extern char str_domain_error_2[16];
extern char str_r6028_unable_to_initialize_heap[40];
extern char str_r6027_not_enough_space_for_lowio_initial[56];
extern char str_r6026_not_enough_space_for_stdio_initial[56];
extern char str_r6025_pure_virtual_function_call[40];
extern char str_r6024_not_enough_space_for_onexit_atexit[56];
extern char str_r6019_unable_to_open_console_device[44];
extern char str_r6018_unexpected_heap_error[36];
extern char str_r6017_unexpected_multithread_lock_error[48];
extern char str_r6016_not_enough_space_for_thread_data[44];
extern char str_abnormal_program_termination[36];
extern char str_r6009_not_enough_space_for_environment[44];
extern char str_r6008_not_enough_space_for_arguments[44];
extern char str_r6002_floating_point_not_loaded[25600];
extern i8 minimap_hidden;
extern u8 minimap_hidden_rest[7];
extern int mapgen_land_mass;
extern int mapgen_land_form;
extern int mapgen_temperature;
extern int mapgen_climate;
extern int mapgen_age;
extern char str_empty_6[4];
extern char str_empty_7[4];
extern char str_empty_8[12];
extern i32 taxrate_lock_flags[3];
extern int taxrate_bg_surface;
extern u8 str_empty_9[4];
extern char str_findcity[12];
extern char str_x[4];
extern char str_revolution[12];
extern char str_startrev[20];
extern int mapview_drag_hold_timer;
extern int mapview_drag_view_index;
extern iptr map_click_tile_x;
extern iptr map_click_tile_y;
extern u8 map_click_tile_y_rest[4];
extern char str_x_2[4];
extern char str_cityinfo[12];
extern char str_map_3_map_window_click_blocked_by_xd_flu[72];
extern char str_map_3_map_double_click_blocked_by_xd_flu[72];
extern char str_map_3_map_ascii_blocked_by_xd_flushsendb[64];
extern char str_map_3_map_key_blocked_by_xd_flushsendbuf[60];
extern char str_reallyquit[12];
extern char str_e[72];
extern u8 open_path_prefix[60];
extern char file_path_mode;
extern u8 file_path_mode_rest[3];
extern char str_rb[16];
#pragma pack(push, 1)
struct tech_editor_combo_x_table_block {
    union {
        struct {
            i8 tech_editor_combo_x_table[4];  /* 00625128 */
            i8 tech_editor_combo_y_table[4];  /* 0062512c */
            int tech_editor_prereq1_ctrl_x;  /* 00625130 */
            int tech_editor_prereq1_ctrl_y;  /* 00625134 */
            int tech_editor_prereq2_ctrl_x;  /* 00625138 */
            int tech_editor_prereq2_ctrl_y;  /* 0062513c */
            int tech_editor_epoch_ctrl_x;  /* 00625140 */
            int tech_editor_epoch_ctrl_y;  /* 00625144 */
            int tech_editor_category_ctrl_x;  /* 00625148 */
            i32 tech_editor_category_ctrl_y_i32;  /* 0062514c */
        };
        EditorPos tech_editor_combo_pos[5];  /* 00625128 */
    };
};
#pragma pack(pop)
extern struct tech_editor_combo_x_table_block tech_editor_combo_x_table_block;
#pragma pack(push, 1)
struct tech_editor_edit_x_table_block {
    i32 tech_editor_edit_x_table[1];  /* 00625150 */
    i32 tech_editor_edit_y_table[1];  /* 00625154 */
    int tech_editor_modifier_edit_x;  /* 00625158 */
    int tech_editor_modifier_edit_y;  /* 0062515c */
};
#pragma pack(pop)
extern struct tech_editor_edit_x_table_block tech_editor_edit_x_table_block;
#pragma pack(push, 1)
struct tech_editor_ctrl_kinds_block {
    union {
        struct {
            u8 tech_editor_ctrl_rows_0_4[40];  /* 00625160 */
            i8 tech_editor_field_min_table[8];  /* 00625188 */
            i8 tech_editor_field_max_table[24];  /* 00625190 */
        };
        EditorCtrlRow tech_editor_ctrl_rows[7];  /* 00625160 (rows 5-6 overlap the min/max tables) */
    };
    i8 tech_editor_int_fmt[4];  /* 006251a8 */
    i8 civilize_rules_name_sep_str[4];  /* 006251ac */
    i8 civilize_rules_ai_sep_str[4];  /* 006251b0 */
    i8 civilize_rules_mod_sep_str[4];  /* 006251b4 */
    i8 civilize_rules_epoch_sep_str[4];  /* 006251b8 */
};
#pragma pack(pop)
extern struct tech_editor_ctrl_kinds_block tech_editor_ctrl_kinds_block;
extern char str_empty_10[8];
extern i8 civilize_rules_newline_str[4];
extern char str_notice[8];
extern char str_civilize[12];
extern char str_error_updating_rules_s[24];
extern char str_file_i_o_error[16];
extern char str_s_s_s[12];
extern i8 tech_editor_usage_preq1_fmt[4];
extern char str_s_s_s_2[12];
extern i8 tech_editor_usage_preq2_fmt[4];
extern char str_techname[12];
extern char str_advances[12];
extern char str_help[8];
extern char str_notice_2[8];
extern u8 str_empty_11[4];
extern i8 tech_editor_prereq1_suffix_str[4];
extern i8 tech_editor_prereq2_suffix_str[4];
extern char str_editorpt_gif[76];
extern char str_hotseat1[12];
extern char str_hotseatnot[16];
extern int strpool_capacity;
extern int game_session_active;
extern int simul_tutorial_shown;
extern char str_cosmic[8];
extern char str_civilize_2[12];
extern char str_advance_d[12];
extern char str_improve[8];
extern char str_endwonder[12];
extern char str_units[8];
extern char str_unit_d[8];
extern char str_terrain[8];
extern char str_governments[12];
extern char str_leaders[8];
extern char str_caravan[8];
extern char str_orders[8];
extern char str_difficulty[12];
extern char str_attitudes[160];
extern i8 startup_game_txt_name_str[8];
extern char str_rules[8];
extern i8 startup_lang_default_ext_str[4];
extern i8 startup_lang_english_ext_str[4];
extern i8 startup_lang_french_ext_str[4];
extern i8 startup_lang_german_ext_str[4];
extern i8 startup_lang_italian_ext_str[4];
extern i8 startup_lang_spanish_ext_str[4];
extern char str_stringheap[12];
extern char str_nil[8];
extern char str_civ_ini[8];
extern char str_herald_warning_shown[24];
extern char str_civilization_gold[20];
extern char str_heraldwarning[16];
extern char str_civ_ini_2[8];
extern i8 ini_herald_warning_value_str[4];
extern char str_herald_warning_shown_2[24];
extern char str_civilization_gold_2[20];
extern char str_init[8];
extern char str_difficulty_2[12];
extern char str_enemies[8];
extern char str_barbarity[12];
extern char str_rules_2[8];
extern char str_advanced[12];
extern char str_accelerated[12];
extern char str_gender[8];
extern char str_tribe[8];
extern char str_game[8];
extern char str_customtribe[12];
extern char str_customtribe2[16];
extern char str_name[8];
extern char str_customcity[12];
extern char str_opponent[12];
extern char str_sizeofmap[12];
extern char str_80[4];
extern char str_50[4];
extern char str_customsize[12];
extern char str_customland[12];
extern char str_customform[12];
extern char str_customclimate[16];
extern char str_customtemp[12];
extern char str_customage[12];
extern i8 map_load_wildcard_str[8];
extern i8 map_load_default_ext_str[4];
extern i8 map_load_open_mode_str[4];
extern char str_failedtoload[16];
extern char str_failedtoload_2[16];
extern char str_useseed[8];
extern char str_usestartloc[12];
extern char str_scenario[12];
extern char str_scenarioloaded[16];
extern char str_difficulty_3[12];
extern char str_gender_2[8];
extern char str_name_2[8];
extern char str_stringheap_2[12];
extern char str_nil_2[8];
extern char str_rules_3[8];
extern char str_empty_12[4];
extern char str_empty_13[4];
extern char str_gender_3[8];
extern char str_name_3[8];
extern i8 menu_game_txt_name_str[8];
extern char str_rules_4[8];
extern char str_mainmenu[12];
extern i8 xdaemon_req_ver_dot1_str[4];
extern i8 xdaemon_req_ver_dot2_str[4];
extern i8 xdaemon_got_ver_dot1_str[4];
extern i8 xdaemon_got_ver_dot2_str[4];
extern char str_wrongxdaemonversion[20];
extern char str_newcredits[12];
extern char str_lobbylaunchinfo_gnetmgr_igzinfo[36];
extern char str_ipaddr_32_s[28];
extern char str_longname_64_s[28];
extern char str_shortname_32_s[28];
extern char str_sessionname_64_s[28];
extern char str_bhost_d[28];
extern char str_not_an_igz_game_event_d[32];
extern char str_igz_sending_dplsys_dplayconnectfailed_to[52];
extern char str_igz_xd_lobbysendmessage_returned_error_d[48];
extern char str_civ_ini_3[8];
extern char str_simultaneous[16];
extern char str_civilization_gold_3[20];
extern char str_civ_ini_4[8];
extern i8 ini_simultaneous_reset_str[4];
extern char str_simultaneous_2[16];
extern char str_civilization_gold_4[20];
extern char str_civ_ini_5[8];
extern char str_maxplayers[12];
extern char str_civilization_gold_5[20];
extern char str_civ_ini_6[8];
extern i8 ini_max_players_reset_str[4];
extern char str_maxplayers_2[12];
extern char str_civilization_gold_6[20];
extern char str_simultut[12];
extern char str_chatlog_txt[1164];
extern char str_empty_14[4];
extern char str_empty_15[4];
extern char str_empty_16[4];
extern char str_empty_17[4];
extern char str_empty_18[4];
extern char str_empty_19[4];
extern char str_empty_20[4];
extern char str_empty_21[4];
extern char str_empty_22[4];
extern i8 gamelist_map_size_sep_str[4];
extern i8 gamelist_double_opts_sep_str[4];
extern i8 gamelist_timer_colon_str[4];
extern i8 gamelist_open_paren_str[4];
extern u8 str_empty_23[4];
extern u8 str_empty_24[4];
extern u8 str_empty_25[4];
extern u8 str_empty_26[4];
extern u8 str_empty_27[4];
extern u8 str_empty_28[4];
extern u8 str_empty_29[4];
extern u8 str_empty_30[4];
extern u8 str_empty_31[4];
extern i8 game_profile_double_opts_sep_str[4];
extern i8 game_profile_timer_colon_str[4];
extern char str_gameprofile[204];
extern char str_string[8];
extern char str_number[8];
extern i8 text_var_hex_tag_str[4];
extern i8 text_var_hex_pad_str[4];
extern i8 text_var_percent_str[8];
extern int strpool_count;
extern int strpool_initialized;
extern u8 strpool_initialized_rest[8];
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
extern char str_cities[8];
extern char str_help_2[8];
extern char str_citymisc[12];
extern char str_editorsq_gif[28];
extern i32 advisor_modal_start_ticks_i32;
extern i32 intel_report_reopen;
extern int credits_line_count;
extern int credits_scroll_pos;
extern uint credits_partial_redraw_ok;
extern char str_tiles_dll[12];
extern char str_scredits_gif[16];
extern char str_supplyshow[12];
extern char str_empty_32[4];
extern char str_supplynone[12];
extern char str_supplysearch[16];
extern char str_empty_33[4];
extern char str_empty_34[4];
extern char str_empty_35[4];
extern char str_empty_36[4];
extern char str_empty_37[4];
extern char str_intellcity[12];
extern char str_reportforeign[16];
extern char str_reportforeignmulti[20];
extern char str_noforeign[12];
extern char str_attitudemulti[16];
extern char str_game_2[8];
extern char str_peacenobetray[16];
extern char str_parleywaiting[16];
extern char str_parleycancel[16];
extern char str_parleygoaway[16];
extern char str_parleyok[12];
extern char str_parleybusy[12];
extern char str_parleycancel_2[16];
extern char str_parleybusy_2[12];
extern char str_nointel[8];
extern char str_empty_38[4];
extern char str_empty_39[4];
extern char str_wonders[8];
extern char str_historians[12];
extern char str_histories[12];
extern char str_history[8];
extern char str_empty_40[4];
extern char str_historyrank[12];
extern char str_empty_41[4];
extern char str_empty_42[4];
extern char str_empty_43[4];
extern char str_0000[8];
extern char str_empty_44[4];
extern char str_m[4];
extern char str_000[8];
extern char str_000_2[8];
extern char str_empty_45[4];
extern char str_empty_46[4];
extern char str_0[4];
extern char str_0_2[4];
extern char str_empty_47[4];
extern u8 str_empty_48[4];
extern char str_empty_49[4];
extern char str_empty_50[4];
extern u8 str_empty_51[4];
extern char str_empty_52[8];
extern char str_empty_53[4];
extern char str_empty_54[8];
extern char str_empty_55[4];
extern char str_empty_56[8];
extern char str_empty_57[4];
extern char str_empty_58[4];
extern char str_femalefame[12];
extern char str_malefame[12];
extern char str_string0[12];
extern char str_error_2[8];
extern char str_femalefame_2[12];
extern char str_malefame_2[12];
extern char str_string0_2[12];
extern char str_error_3[8];
extern char str_empty_59[4];
extern char str_empty_60[4];
extern char str_empty_61[8];
extern i8 hall_of_fame_read_mode[4];
extern char str_hallfame_dat[16];
extern i8 hall_of_fame_write_mode[4];
extern char str_hallfame_dat_2[16];
extern char str_credits[8];
extern char str_credits_2[8];
extern char str_credits_3[8];
extern char str_mpcredits[12];
extern char str_credits_4[8];
extern char str_fcredits[12];
extern char str_credits_5[8];
extern char str_scredits[12];
extern char str_empty_62[4];
extern char str_empty_63[8];
extern char str_empty_64[152];
extern i8 pop_thousands_sep_str[4];
extern i8 pop_zero_pad_str[4];
extern char str_0_000[8];
extern i8 city_pop_thousands_sep_str[4];
extern i8 city_pop_zero_pad_str[4];
extern char str_0_000_2[8];
extern char str_city[8];
extern char str_city_2[8];
extern char str_stop[8];
extern char str_extra[8];
extern char str_stop_2[8];
extern char str_create_city_connection_to_server_timed_o[48];
extern char str_serverconnecttime[20];
extern char str_toomanycities[16];
extern char str_caravan_2[8];
extern char str_caravan_3[8];
extern char str_caravan_4[8];
extern char str_foodcaravan[12];
extern char str_foodcaravan_2[12];
extern char str_foodcaravan_3[12];
extern char str_caravanother[16];
extern char str_caravanother_2[16];
extern char str_delete_city_connection_to_server_timed_o[48];
extern char str_serverconnecttime_2[20];
extern char str_startwonder[12];
extern char str_switchwonder[16];
extern char str_abandonwonder[88];
extern char str_d_ss_franklinton_startup_lan_cpp[36];
extern char str_whichnetwork_internet_whichnetwork_local[152];
extern char str_networktype[12];
extern char str_d_ss_franklinton_startup_lan_cpp_2[36];
extern char str_gnetmgr_pdescription[24];
extern char str_lan1[8];
extern char str_lannot[8];
extern char str_startup_lan_failure_of_connecttonet_as_c[48];
extern char str_lanconnectfail[16];
extern char str_startup_lan_failure_of_connecttonet_as_s[48];
extern char str_lanconnectfail_2[16];
extern char str_ipverify[12];
extern char str_d_ss_franklinton_startup_lan_cpp_3[36];
extern char str_tempstr[8];
extern i8 net_title_ip_sep_str[4];
extern char str_ipaddress[12];
extern char str_netname[8];
extern char str_gamename[12];
extern char str_scenario_2[12];
extern char str_scenarioloaded_2[16];
extern char str_difficulty_4[12];
extern char str_advancedmp[12];
extern char str_empty_65[4];
extern char str_empty_66[4];
extern char str_joinedmax[12];
extern char str_gamecanceled[16];
extern char str_serverconnectfail[20];
extern char str_alreadychosen[16];
extern char str_gender_4[8];
extern char str_name_4[132];
extern char str_terrain1_bmp[16];
extern char str_terrain1_gif[16];
extern char str_terrain1_gif_2[16];
extern char str_terrain2_bmp[16];
extern char str_terrain2_gif[16];
extern char str_terrain2_gif_2[16];
extern char str_icons_bmp[12];
extern char str_icons_gif[12];
extern char str_icons_gif_2[12];
extern char str_editorpt_gif_2[16];
extern char str_people_bmp[12];
extern char str_people_gif[12];
extern char str_people_gif_2[12];
extern char str_cities_bmp[12];
extern char str_cities_gif[12];
extern char str_cities_gif_2[12];
extern char str_units_bmp[12];
extern char str_units_gif[12];
extern char str_units_gif_2[60];
extern ThroneRoomWindow *throneroom_wnd;
extern char str_pv_dll[8];
extern u8 str_empty_67[4];
extern u8 str_empty_68[4];
extern char str_throne[8];
extern char str_empty_69[4];
extern char str_addtothrone[12];
extern char str_empty_70[4];
extern char str_empty_71[4];
extern char str_empty_72[12];
extern int hyperlink_class_registered;
extern u8 str_empty_73[4];
extern char str_advance_index[16];
extern char str_improvement_index[20];
extern char str_wonder_index[16];
extern char str_unit_index[12];
extern char str_terrain_index[16];
extern char str_government_index[20];
extern char str_concept_descriptions[24];
extern char str_unknown_topic_group[20];
extern char str_describe[12];
extern i8 pedia_desc_marker_str[4];
extern i8 pedia_desc_crlf_str[4];
extern char str_describe_2[12];
extern char str_mshypertextclass[20];
extern i8 hyperlink_window_title_str[4];
extern char str_mshypertextclass_2[64];
extern int registry_language_id;
extern char str_microprose_software[20];
extern char str_civilization_ii_multiplayer_gold_edition[44];
extern char str_software_s_s[16];
extern char str_defaultlanguage[16];
extern char str_software_s_s_2[16];
extern char str_defaultlanguage_2[16];
extern i8 registry_key_class_str[28];
extern int cityview_scroll_x;
extern CityViewWindow *cityview_wnd;
extern char str_cv_dll[8];
extern u8 str_empty_74[4];
extern u8 str_empty_75[8];
extern i32 diplo_initial_eval_done;
extern int diplo_opening_eval_active;
extern int diplo_session_active;
extern i32 diplo_ai_meeting_active_i32;
extern i32 mp_meeting_pending;
extern int diplo_emissary_asked;
extern int diplo_emissary_response;
extern char str_er[4];
extern char str_greetings[12];
extern char str_1[4];
extern char str_0_3[4];
extern char str_nuclearweapons[16];
extern char str_yournukes[12];
extern char str_emissaryforce[16];
extern char str_emissary[12];
extern char str_selltech[12];
extern char str_selltech2[12];
extern char str_noexchangemad[16];
extern char str_noexchangemedium[20];
extern char str_noexchangemad_2[16];
extern char str_noexchangemedium_2[20];
extern char str_noexchangewonder[20];
extern char str_noexchangemedium_3[20];
extern char str_noexchangenow[16];
extern char str_exchangegift[16];
extern char str_exchangegift2[16];
extern char str_noexchangenow_2[16];
extern char str_exchange[12];
extern char str_0_4[4];
extern char str_1_2[4];
extern char str_exchangepetty[16];
extern char str_alliance[12];
extern char str_treaty[8];
extern char str_ceasefire[12];
extern char str_activateally[16];
extern char str_allyhelps[12];
extern char str_demandhelp[12];
extern char str_helpbonus[12];
extern char str_didnthelp[12];
extern char str_nobetrayweak[16];
extern char str_alliancenobetray[20];
extern char str_alliancenowinning[20];
extern char str_alliancenodislike[20];
extern char str_alliancenosmall[16];
extern char str_alliancenopatience[20];
extern char str_alliancenothanks[20];
extern char str_perhapssecret[16];
extern char str_perhapsthrowin[16];
extern char str_alliancenopatience_2[20];
extern char str_perhapssolidarity[20];
extern char str_perhapsthanksanyway[20];
extern char str_perhapsbye[12];
extern char str_nobetrayweak_2[16];
extern char str_peacenobetray_2[16];
extern char str_peacenowinning[16];
extern char str_peacenodislike[16];
extern char str_peacenopatience[16];
extern char str_perhapssecret_2[16];
extern char str_perhapsthrowin_2[16];
extern char str_peacenopatience_2[16];
extern char str_perhapssolidarity_2[20];
extern char str_perhapsthanksanyway_2[20];
extern char str_perhapsbye_2[12];
extern char str_provoke[8];
extern char str_apologize[12];
extern char str_givemoreciv[12];
extern char str_givemoreally[16];
extern char str_givemore[12];
extern char str_sympathy[12];
extern char str_tauntally[12];
extern char str_feebleally[12];
extern char str_provoke_2[8];
extern char str_noviolators[12];
extern char str_feeble[8];
extern char str_cancelally[12];
extern char str_favormenu[12];
extern char str_mustattack[12];
extern char str_nocontact[12];
extern char str_atwar[8];
extern char str_hellnowewontgo[16];
extern char str_eternalallies[16];
extern char str_cybercop[12];
extern char str_mercenary[12];
extern char str_unfortunate[12];
extern char str_mercdeclare[12];
extern char str_mercbetrayally[16];
extern char str_mercbetray[12];
extern char str_mapno[8];
extern char str_mapyes[8];
extern char str_giftmenu[12];
extern char str_knowno[8];
extern char str_techgift[12];
extern char str_techgift2[12];
extern char str_wasting[8];
extern char str_accept[8];
extern char str_moneygift[12];
extern char str_wasting_2[8];
extern char str_accept_2[8];
extern char str_militarysource[16];
extern char str_militarynone[16];
extern char str_militarynone_2[16];
extern char str_militaryno[12];
extern char str_militaryno_2[12];
extern char str_accept_3[8];
extern char str_breakthrough[16];
extern char str_continueun[12];
extern char str_continuehawks[16];
extern char str_diplomacy[12];
extern char str_diplomacymenu[16];
extern char str_outahere[12];
extern char str_outahereally[16];
extern char str_outahere_2[12];
extern char str_canceltreaty[16];
extern char str_tribute[8];
extern char str_takeciv[8];
extern char str_overabarrel[12];
extern char str_begone0[8];
extern char str_begone1[8];
extern char str_provoke_3[8];
extern char str_reject0[8];
extern char str_rid0[8];
extern char str_accursedwall[16];
extern char str_accursedun[12];
extern char str_proposealliance[16];
extern char str_perhapssolidarity_3[20];
extern char str_perhapsdidntprove[20];
extern char str_grantcease[12];
extern char str_wallcease[12];
extern char str_proposecease[16];
extern char str_wallovercease[16];
extern char str_unovercease[12];
extern char str_overrulecease[16];
extern char str_giveciv[8];
extern char str_givecash[12];
extern char str_grovel[8];
extern char str_worthless[12];
extern char str_continuewar[12];
extern char str_proposepeace[16];
extern char str_walloverpeace[16];
extern char str_unoverpeace[12];
extern char str_overrulepeace[16];
extern char str_cashforpeace[16];
extern char str_nopeace[8];
extern char str_walloverpeace_2[16];
extern char str_unoverpeace_2[12];
extern char str_wallovercease_2[16];
extern char str_unovercease_2[12];
extern char str_senatepeace[12];
extern char str_senatecease[12];
extern char str_attitudeally[16];
extern char str_attitudepeace[16];
extern char str_attitude[12];
extern char str_howdyally[12];
extern char str_howdypeace[12];
extern char str_howdy[8];
extern char str_doodyally[12];
extern char str_doody[8];
extern char str_welcomeally[12];
extern char str_welcomepeace[16];
extern char str_welcome[8];
extern char str_notorious[12];
extern char str_hate[8];
extern char str_big[4];
extern char str_small[8];
extern char str_pleasecities[16];
extern char str_pleasecity[12];
extern char str_allyplea[12];
extern char str_allybrag[12];
extern char str_patienceally[16];
extern char str_patience[12];
extern char str_ceaseexpire[12];
extern char str_ambassadors[12];
extern char str_schism[960];
extern i8 *str_ptr_5_4_0f_multiplayer_26_march_99;
extern u8 str_ptr_5_4_0f_multiplayer_26_march_99_rest[4];
extern int xdaemon_required_major;
extern int xdaemon_required_minor;
extern int xdaemon_required_build;
extern int scenario_events_loaded;
extern i8 *str_ptr_labels;
extern u8 str_ptr_labels_rest[8];
extern Advance advances[100];
extern Terrain terrain_types[33];
extern i32 next_unit_id;
extern i32 next_city_id;
extern i16 start_x[21];
extern u8 start_x_rest[6];
extern i16 start_y[21];
extern u8 start_y_rest[10];
extern i8 game_loop_active;
extern u8 game_loop_active_rest[3];
extern char turn_resume_civ;
extern u8 turn_resume_civ_rest[3];
extern i8 unit_blink_active;
extern u8 unit_blink_active_rest[7];
extern i8 unit_blink_phase;
extern u8 unit_blink_phase_rest[3];
extern char status_end_turn_blink_phase;
extern u8 status_end_turn_blink_phase_rest[3];
extern i8 unit_blink_skip_tick;
extern u8 unit_blink_skip_tick_rest[3];
extern int map_layout_wide;
extern int game_language;
extern i32 double_moves_applied;
extern char str_5_4_0f_multiplayer_26_march_99[32];
extern char str_patch_3[8];
extern char str_labels[8];
extern char str_tutorial[640];
extern char str_cancelalliance[16];
extern char str_0_5[4];
extern char str_1_3[8];
#pragma pack(push, 1)
struct diag_neighbor_bits_block {
    i8 diag_neighbor_bits[3];  /* 00628338 */
    i8 pick_quadrant_dx_table[1];  /* 0062833b */
    i8 diag_dx_table[7];  /* 0062833c */
    i8 pick_quadrant_dy_table[1];  /* 00628343 */
    i8 diag_dy_table[12];  /* 00628344 */
};
#pragma pack(pop)
extern struct diag_neighbor_bits_block diag_neighbor_bits_block;
#pragma pack(push, 1)
struct dir_dx_table_block {
    i8 dir_dx_table[1];  /* 00628350 */
    i8 mapgen_cardinal_dx_table[15];  /* 00628351 */
};
#pragma pack(pop)
extern struct dir_dx_table_block dir_dx_table_block;
#pragma pack(push, 1)
struct dir_dy_table_block {
    i8 dir_dy_table[1];  /* 00628360 */
    i8 mapgen_cardinal_dy_table[15];  /* 00628361 */
    i8 city_tile_dx_table[48];  /* 00628370 */
    i8 city_tile_dy_table[48];  /* 006283a0 */
    i8 coarse_dir_dx_table[16];  /* 006283d0 */
    i8 coarse_dir_dy_table[44];  /* 006283e0 */
};
#pragma pack(pop)
extern struct dir_dy_table_block dir_dy_table_block;
extern char str_empty_76[4];
extern char str_empty_77[4];
extern char str_empty_78[4];
extern char str_empty_79[4];
extern int label_ids_handle;
extern i32 *label_string_ids;
extern i32 label_count;
extern char str_labels_2[8];
extern char str_labels_3[8];
extern char str_labels_4[8];
extern char str_labels_5[8];
extern char str_error_labels_txt[32];
extern int net_send_sequence;
extern u8 net_send_sequence_rest[4];
extern char str_nm_datagram[5408];
extern char str_nm_pop_retiredie[3272];
extern char str_d_ss_franklinton_netmgr_send_cpp[36];
extern char str_sock_0_sock_max_net_players_sock_broadca[84];
extern char str_d_ss_franklinton_netmgr_send_cpp_2[36];
extern char str_pmsg_size_0[16];
extern char str_sendtoconnections_failed_on_send_to_conn[80];
extern char str_popup_type_s[16];
extern char str_sendtoeveryone_failed_return_val_d[40];
extern u8 str_empty_80[4];
extern char str_d_ss_franklinton_netmgr_send_cpp_3[36];
extern char str_address_null[16];
extern char str_d_ss_franklinton_netmgr_send_cpp_4[36];
extern char str_address[1828];
extern u8 str_empty_81[4];
extern i32 intro_theme_started;
extern int opening_video_player;
extern char str_civ2_civ2art_dll[20];
extern char str_civ2_video_opening_avi[24];
extern u8 str_empty_82[4];
extern u8 str_empty_83[4];
extern char str_vfwnotregistered[36];
extern char str_aircombt[1200];
extern int cd_music_available;
extern i32 cd_music_unknown_disc;
extern uint cd_music_current_track;
extern i32 cd_music_continue;
extern char str_cheers[8];
extern i8 snd_wav_ext_str[8];
extern char str_civ2_sound[12];
extern char str_sound[260];
extern char str_d_ss_franklinton_grey_cpp[28];
extern char str_handle_fullcolors[20];
extern char str_d_ss_franklinton_grey_cpp_2[28];
extern char str_hlocpal[8];
extern char str_d_ss_franklinton_grey_cpp_3[28];
extern char str_handle_palette[16];
extern char str_d_ss_franklinton_grey_cpp_4[28];
extern char str_handle_colors[16];
extern char str_d_ss_franklinton_grey_cpp_5[28];
extern char str_handle_grey[12];
extern char str_d_ss_franklinton_grey_cpp_6[28];
extern char str_p[52];
extern i8 gfx_load_bmp_no_error_str[4];
extern i8 gfx_bmp_read_mode[4];
extern char str_error_reading_file_header[28];
extern char str_invalid_bitmap_file[20];
extern char str_error_reading_info_header[28];
extern char str_error_reading_file_palette[28];
extern char str_image_not_640_x_480_x_256_color[32];
extern char str_error_reading_bitmap_image[28];
extern char str_incompatable_compression_mode_bi_rle4[40];
extern i8 gfx_save_bmp_no_error_str[4];
extern i8 gfx_bmp_write_mode[4];
extern char str_error_writing_file_header[28];
extern char str_error_writing_info_header[28];
extern char str_error_writing_file_palette[28];
extern char str_error_writing_bitmap_image[96];
extern EndgameMovieWindow *endgame_movie_window;
extern char str_loser_avi[12];
extern char str_civ2art_dll[16];
extern u8 str_empty_84[4];
extern u8 str_empty_85[4];
extern char str_vfwnotregistered_2[20];
extern char str_archaeologists[16];
extern char str_empty_86[4];
extern char str_archaeologists3[16];
extern char str_empty_87[24];
extern iptr spaceship_arrival_window;
extern char str_0_000_3[8];
extern char str_centauri[12];
extern char str_civ2_civ2_exe[16];
extern char str_civ2_video_winwin_avi[24];
extern char str_civ2_civ2art_dll_2[20];
extern u8 str_empty_88[4];
extern u8 str_empty_89[4];
extern char str_vfwnotregistered_3[20];
extern char str_0_000_4[8];
extern char str_centauri_2[12];
extern char str_empty_90[4];
extern char str_centauri3[12];
extern char str_civ2_civ2art_dll_3[20];
extern u8 str_empty_91[4];
extern u8 str_empty_92[4];
extern char str_0_000_5[8];
extern char str_centauri_beaten[16];
extern char str_empty_93[44];
extern i8 path_default_ext_dot_str[4];
extern i8 path_set_ext_dot_str[28];
extern i8 *str_ptr_civilize;
extern char str_civilize_3[12];
extern char str_load_gpk_fixing_up_game_enemies_d_incons[76];
extern char str_load_gpk_fixing_up_game_enemies_d_incons_2[76];
extern i8 save_ext_scn_str[8];
extern i8 save_ext_sav_str[8];
extern i8 save_ext_hot_str[8];
extern i8 save_ext_eml_str[8];
extern i8 save_ext_net_str[8];
extern char str_scn[12];
extern char str_scn_2[8];
extern char str_mp[8];
extern i8 save_filter_mp_pattern_str[8];
extern char str_sav[12];
extern char str_sav_2[8];
extern char str_hot[12];
extern char str_hot_2[8];
extern char str_eml[12];
extern char str_eml_2[8];
extern char str_net[12];
extern char str_net_2[8];
extern i8 save_write_mode[4];
extern i8 save_events_tag_str[12];
extern i8 save_read_mode[4];
extern char str_game_3[8];
extern char str_game_4[8];
extern i8 game_txt_default_name_str[8];
extern i8 save_events_tag_check_str[8];
extern char str_empty_94[4];
extern char str_empty_95[4];
extern char str_empty_96[4];
extern char str_a[4];
extern char str_b[4];
extern i8 save_name_date_sep_str[4];
extern i8 save_name_month_day_sep_str[4];
extern i8 save_name_day_year_sep_str[4];
extern i8 save_name_time_sep_str[4];
extern i8 save_name_hour_min_sep_str[4];
extern i8 save_name_min_sec_sep_str[4];
extern i8 save_name_ext_dot_str[4];
extern char str_saveerror[12];
extern char str_scenok[8];
extern char str_emailsaved[12];
extern char str_saveok[8];
extern char str_empty_97[4];
extern char str_empty_98[4];
extern i8 load_dialog_wildcard_str[4];
extern i8 game_load_scn_ext_str[8];
extern char str_alt[8];
extern char str_loadnotsave[12];
extern char str_loadoldsave[12];
extern char str_loadbadsave[12];
extern char str_loadnewsave[12];
extern char str_events[8];
extern char str_beginevents[12];
extern char str_events_2[8];
extern char str_title_gif[12];
extern char str_title_gif_2[12];
extern char str_loadok[8];
extern char str_stacked_unit_in_save_unit_d_type_d_whose[200];
extern i32 map_click_in_progress;
extern u8 map_click_in_progress_rest[4];
extern i32 coast_quarter_dx_table[4];
extern i32 coast_quarter_dy_table[4];
extern int map_show_hidden_terrain;
extern u8 map_show_hidden_terrain_rest[16];
extern char net_deferred_input_busy;
extern u8 net_deferred_input_busy_rest[3];
extern char str_stacked_draw_stack_full_flushing[36];
extern char str_d_ss_franklinton_netmgr_poll_cpp[36];
extern char str_gnetmgr_firststackeddraw_0_gnetmgr_first[80];
extern char str_d_ss_franklinton_netmgr_poll_cpp_2[36];
extern char str_gnetmgr_laststackeddraw_0_gnetmgr_lastst[76];
extern char str_poll_unstacking_xd_flushsendbuffer_block[48];
extern char str_d_ss_franklinton_netmgr_poll_cpp_3[36];
extern char str_pdata_null[16];
extern char str_poll_retrieved_non_civ_message_message_d[56];
extern char str_waitongamexmit[16];
extern char str_d_ss_franklinton_netmgr_poll_cpp_4[36];
extern char str_gnetmgr_bserver[16];
extern char str_waitongamexmit_2[16];
extern char str_hold_on[8];
extern char str_hold_off[12];
extern char str_pretextallied[16];
extern char str_pretext[8];
extern char str_received_unknown_message_type[32];
extern char str_serverquitwait[16];
extern char str_clientquitwait[16];
extern char str_clientquitwait_2[16];
extern char str_clienttransferserver_waiting_for_nm_star[56];
extern char str_lanconnectfail_3[16];
extern char str_clientquitwait_3[16];
extern char str_clienttransferserver_waiting_for_nm_play[52];
extern char str_lanconnectfail_4[16];
extern char str_clienttransferserver_disconnecting_from[52];
extern char str_clienttransferserver_could_not_reconnect[56];
extern char str_lanconnectfail_5[16];
extern char str_reconnected_to_network_as_server[36];
extern char str_clientquitwait_4[16];
extern char str_nooneshowed[12];
extern char str_someshowed[12];
extern char str_d_ss_franklinton_netmgr_poll_cpp_5[36];
extern char str_oldindex_max_net_players[28];
extern char str_hostchangeserver[20];
extern char str_disconnecting_from_server_s[32];
extern char str_clienttransferserver_disconnection_from[72];
extern char str_lanconnectfail_6[16];
extern char str_attempting_to_reconnect_to_new_server_s[44];
extern char str_connecttogame_open_connection_failed_err[48];
extern char str_serverconnectfail_2[20];
extern char str_connection_to_new_server_successful[40];
extern char str_clientquitwait_5[16];
extern char str_clienttransferserver_client_timed_out_wa[64];
extern char str_lanconnectfail_7[16];
extern char str_hostchangeclient[20];
extern char str_d_ss_franklinton_netmgr_poll_cpp_6[36];
extern char str_pstrings_charptr_pevents_pstrings_scenar[420];
extern i32 net_meet_pending_flags[8];
extern int turn_resume_new_game;
extern u8 turn_resume_new_game_rest[4];
#pragma pack(push, 1)
struct year_table_start_years_block {
    i8 year_table_start_years[4];  /* 0062c490 */
    i8 year_table_turn_counts[4];  /* 0062c494 */
    i8 year_table_year_steps[52];  /* 0062c498 */
    i8 year_table_last_start_year[4];  /* 0062c4cc */
    i8 year_table_last_turn_count[4];  /* 0062c4d0 */
    i8 year_table_last_year_step[220];  /* 0062c4d4 */
    i32 app_shutdown_done;  /* 0062c5b0 */
    iptr spaceship_winner_civ;  /* 0062c5b4 */
    int civ_turn_in_progress;  /* 0062c5b8 */
    int active_unit_lost_in_combat;  /* 0062c5bc */
    int tutorial_hint_shown_this_turn;  /* 0062c5c0 */
};
#pragma pack(pop)
extern struct year_table_start_years_block year_table_start_years_block;
extern YearPeriod *const year_period_table;  /* = year_table_start_years_block: [table * 6 + period], 4 tables of 6 periods (the year loop reads period 6) */
extern char str_reallyquit_2[12];
extern char str_barbarians[12];
extern char str_globalwarming[16];
extern char str_fearwarming[12];
extern char str_eaglehaslanded[16];
extern char str_empty_99[4];
extern char str_2[4];
extern char str_empty_100[4];
extern char str_empty_101[4];
extern char str_fertile[8];
extern char str_taxes[8];
extern char str_buildcity[12];
extern char str_expand1[8];
extern char str_mining[8];
extern char str_irrigate[12];
extern char str_road[8];
extern char str_damage[8];
extern char str_primary_cpp_human_turn_timer_seems_to_be[68];
extern char str_counciltime[12];
extern char str_firstmove[12];
extern char str_help1[8];
extern char str_help2[8];
extern char str_onecity[8];
extern char str_expand0[8];
extern char str_scenarioends[16];
extern char str_scenarioend[12];
extern char str_planretire[12];
extern char str_doretire[12];
extern char str_keepplaying[12];
extern char str_hothumansdead[16];
extern char str_hotseatturn[12];
extern char str_emaildone1[12];
extern char str_emaildone2[12];
extern char str_hotseataiturn[16];
extern char str_casualty[12];
extern char str_casualties[12];
extern char str_server_s_d[16];
extern char str_client_s_d[16];
extern char str_clienthotwait[16];
extern char str_casualty_2[12];
extern char str_casualties_2[12];
extern char str_ourturntomove[16];
extern char str_server_s_d_2[16];
extern char str_client_s_d_2[16];
extern char str_casualty_3[12];
extern char str_casualties_3[12];
extern char str_ourturntomove_2[16];
extern char str_newplayer[12];
extern char str_we_lost_connection_to_the_server_heartbe[56];
extern char str_lostserver[12];
extern char str_lostclient[220];
extern i8 pedia_terrain_move_cost_fmt[4];
extern char str_200[8];
extern i8 pedia_terrain_defense_50_str[8];
extern char str_100[8];
extern char str_150[8];
extern i8 pedia_terrain_food_fmt[4];
extern i8 pedia_terrain_shields_fmt[4];
extern i8 pedia_terrain_trade_fmt[4];
extern i8 pedia_terrain_improve_bonus_fmt[8];
extern i8 pedia_terrain_improve_turns_fmt[4];
extern char str_1_4[16];
extern int chat_macro_next_adj_civ;
extern int chat_macro_next_leader_civ;
extern int chat_macro1_file_offset;
extern int chat_macro2_file_offset;
extern int chat_macro3_file_offset;
extern char str_chatmac1_txt[16];
extern char str_chatmac2_txt[16];
extern char str_chatmac3_txt[28];
extern i8 *leader_video_tribe_codes[21];
extern DiploLeaderWindow *diplo_leader_window;
extern char diplo_leader_wnd_open;
extern u8 diplo_leader_wnd_open_rest[3];
extern i8 diplo_leader_wnd_busy;
extern u8 diplo_leader_wnd_busy_rest[3];
extern char diplo_leader_video_active;
extern u8 diplo_leader_video_active_rest[3];
extern int diplo_greeting_sound_id;
extern int diplo_leader_sound_ids[2];
extern int diplo_leader_reaction_sound_ids[3];
extern int tech_prereq_walk_count;
extern u8 tech_prereq_walk_count_rest[84];
extern char str_civ2_mk_dll[12];
extern char str_notice_3[8];
extern char str_init_tile_failed[20];
extern char str_notice_4[8];
extern char str_init_background_failed[24];
extern char str_notice_5[8];
extern char str_init_main_art_failed[24];
extern char str_notice_6[8];
extern char str_init_military_failed[24];
extern char str_notice_7[8];
extern char str_init_science_failed[24];
extern char str_notice_8[8];
extern char str_init_portrait_failed[24];
extern u8 str_empty_102[4];
extern u8 str_empty_103[4];
extern char str_empty_104[4];
extern char str_empty_105[4];
extern char str_empty_106[4];
extern char str_empty_107[4];
extern char str_civ2_kings[12];
extern char str_hrld[8];
extern char str_avi[8];
extern u8 str_empty_108[4];
extern char str_vfwnotregistered_4[20];
extern u8 str_empty_109[4];
extern char str_ok[4];
extern char str_cancel[8];
extern char str_mk_dll[112];
extern u8 str_empty_110[4];
extern char str_password1[12];
extern char str_password0[12];
extern u8 str_empty_111[4];
extern char str_password1_2[12];
extern u8 str_empty_112[4];
extern char str_password1_3[12];
extern char str_password4[12];
extern u8 str_empty_113[4];
extern char str_password1_4[12];
extern char str_password0_2[32];
extern iptr citypref_count;
extern i32 citypref_no_defend;
extern char str_nodefend[12];
extern char str_citypref_txt[16];
extern char str_autobuild[12];
extern char str_citypref_txt_2[16];
extern char str_weshot[8];
extern char str_wesaw[24];
extern FILE *text_file_handle;
extern i8 lang_file_ext[4];
extern char str_rt[4];
extern char str_rt_2[12];
#pragma pack(push, 1)
struct tribe_editor_combo_x_table_block {
    union {
        struct {
            i8 tribe_editor_combo_x_table[4];  /* 0062cd38 */
            i8 tribe_editor_combo_y_table[4];  /* 0062cd3c */
            int tribe_editor_city_style_x;  /* 0062cd40 */
            int tribe_editor_city_style_y;  /* 0062cd44 */
            int tribe_editor_attack_x;  /* 0062cd48 */
            int tribe_editor_attack_y;  /* 0062cd4c */
            int tribe_editor_expand_x;  /* 0062cd50 */
            int tribe_editor_expand_y;  /* 0062cd54 */
            int tribe_editor_civilize_x;  /* 0062cd58 */
            int tribe_editor_civilize_y;  /* 0062cd5c */
        };
        EditorPos tribe_editor_combo_pos[5];  /* 0062cd38 */
    };
    i32 tribe_editor_edit_x_table[1];  /* 0062cd60 */
    i32 tribe_editor_edit_y_table[1];  /* 0062cd64 */
};
#pragma pack(pop)
extern struct tribe_editor_combo_x_table_block tribe_editor_combo_x_table_block;
#pragma pack(push, 1)
struct tribe_editor_ctrl_kinds_block {
    EditorCtrlRow tribe_editor_ctrl_rows[5];  /* 0062cd68 */
    i8 rules_gov_name_sep_str[4];  /* 0062cd90 */
    i8 rules_gov_male_title_sep_str[4];  /* 0062cd94 */
    i8 rules_gov_line_end_str[4];  /* 0062cd98 */
    i8 rules_leader_male_name_sep_str[4];  /* 0062cd9c */
    i8 rules_leader_female_name_sep_str[4];  /* 0062cda0 */
    i8 rules_leader_female_flag_sep_str[4];  /* 0062cda4 */
    i8 rules_leader_color_sep_str[4];  /* 0062cda8 */
    i8 rules_leader_city_style_sep_str[4];  /* 0062cdac */
    i8 rules_leader_plural_sep_str[4];  /* 0062cdb0 */
    i8 rules_leader_adjective_sep_str[4];  /* 0062cdb4 */
    i8 rules_leader_attack_sep_str[4];  /* 0062cdb8 */
    i8 rules_leader_expand_sep_str[4];  /* 0062cdbc */
    i8 rules_leader_civilize_sep_str[4];  /* 0062cdc0 */
    i8 rules_leader_line_end_str[4];  /* 0062cdc4 */
    i8 rules_commodity_line_end_str[4];  /* 0062cdc8 */
};
#pragma pack(pop)
extern struct tribe_editor_ctrl_kinds_block tribe_editor_ctrl_kinds_block;
extern char str_governments_2[12];
extern char str_leaders_2[8];
extern char str_caravan_5[8];
extern char str_error_updating_rules_s_2[24];
extern char str_file_i_o_error_2[16];
extern char str_renamecommo[12];
extern char str_commoname[12];
extern char str_renamegovt[12];
extern char str_govtname[12];
extern char str_renametitle[12];
extern char str_s_s_s_3[12];
extern char str_titlename[12];
extern char str_tribes[8];
extern char str_help_3[8];
extern u8 str_empty_114[4];
extern char str_editorsa_gif[80];
extern i8 *str_ptr_civ2_dat;
extern u8 str_ptr_civ2_dat_rest[4];
extern i8 setup_chosen_leaders[8];
extern char str_civ2_dat[12];
extern i8 prefs_defaults_empty_str[4];
extern i8 prefs_turn_timer_default_str[4];
extern i8 civ2dat_read_mode_str[4];
extern i8 civ2dat_write_mode_str[4];
extern char str_spacereturns[16];
extern char str_spacedestroyed[16];
extern char str_new_civ_connection_to_server_timed_out[44];
extern char str_serverconnecttime_3[20];
extern char str_new_civ_received_nm_new_civ_ack_from_ser[48];
extern char str_kill_civ_connection_to_server_timed_out[44];
extern char str_serverconnecttime_4[20];
extern char str_kill_civ_received_nm_new_civ_ack_from_se[48];
extern char str_destroyed[72];
extern iptr path_unit_type;
extern i32 path_road_planning;
extern i32 path_owner_civ;
extern i32 path_roads_only;
extern i32 path_debug_pending;
extern int path_debug_requested;
extern char str_movedebug[16];
extern i8 append_space_str[4];
extern i8 append_comma_str[4];
extern i8 append_colon_str[4];
extern i8 append_period_str[4];
extern i8 append_percent_str[4];
extern i8 append_lparen_str[4];
extern i8 append_rparen_str[12];
extern i8 append_plus_str[28];
extern i8 rules_advance_no_str[4];
extern i8 rules_advance_nil_str[4];
extern i8 rules_terrain_no_str[4];
extern i8 rules_terrain_yes_str[8];
extern char str_empty_115[4];
extern iptr diffeng_mirror_buf;
extern int diffeng_packet_flush_size;
extern i32 diffeng_send_count_i32;
extern char str_d_ss_franklinton_difference_engine_cpp[40];
extern char str_mirrorlength_sizeof_long_0[36];
extern char str_d_ss_franklinton_difference_engine_cpp_2[40];
extern char str_difflength_diff_engine_message_length[44];
extern char str_d_ss_franklinton_difference_engine_cpp_3[40];
extern char str_difflength_diff_engine_message_length_2[96];
extern char str_d_ss_franklinton_difference_engine_cpp_4[40];
extern char str_size_0[12];
extern char str_failed_to_allocated_buffer_for_game_stat[72];
extern char str_d_ss_franklinton_difference_engine_cpp_5[40];
extern char str_cursize_datsize_bufsize[32];
extern char str_failed_to_allocated_buffer_for_game_stat_2[72];
extern char str_d_ss_franklinton_difference_engine_cpp_6[40];
extern char str_sz_0[8];
extern char str_failed_to_allocated_buffer_for_game_stat_3[72];
extern char str_failed_to_allocated_buffer_2_for_game_st[72];
extern char str_d_ss_franklinton_difference_engine_cpp_7[40];
extern char str_sz_0_2[8];
extern char str_failed_to_allocate_buffer_for_game_state[68];
extern char str_failed_to_re_allocate_buffer_for_game_st[72];
extern char str_d_ss_franklinton_difference_engine_cpp_8[40];
extern char str_phdata_phdata[20];
extern char str_failed_to_allocate_buffer_2_for_game_sta[72];
extern char str_failed_to_rllbufferdecode_in_setgamestat[48];
extern char str_d_ss_franklinton_difference_engine_cpp_9[40];
extern char str_pnode_type_0_pnode_type_num_block_types[52];
extern char str_d_ss_franklinton_difference_engine_cpp_10[40];
extern char str_phdata_phdata_2[20];
extern char str_d_ss_franklinton_difference_engine_cpp_11[40];
extern char str_blocktype_btgame[20];
extern char str_d_ss_franklinton_difference_engine_cpp_12[40];
extern char str_blocktype_btmapstruct[28];
extern char str_d_ss_franklinton_difference_engine_cpp_13[40];
extern char str_pnode_type_0_pnode_type_num_block_types_2[356];
extern i32 parley_mode_left_kinds[1];
extern i32 parley_mode_right_kinds[3];
extern i32 parley_mode2_left_kind;
extern i32 parley_mode2_right_kind;
extern u32 parley_mode3_left_kind;
extern u32 parley_mode3_right_kind;
extern u8 parley_mode3_right_kind_rest[8];
extern u32 parley_mode5_left_kind;
extern u32 parley_mode5_right_kind;
extern u8 parley_mode5_right_kind_rest[88];
extern int panel_margin_x;
extern int panel_margin_y;
extern int panel_padding_x;
extern int panel_bevel_width;
extern int panel_padding_y;
extern void *chat_input_buffer;
extern char *chat_log_buffer;
extern char str_chatlog_txt_2[12];
extern char str_civ_ini_7[8];
extern char str_chatshowsize[16];
extern char str_civilization_gold_7[20];
extern char str_civ_ini_8[8];
extern i8 chat_show_size_default_str[8];
extern char str_chatshowsize_2[16];
extern char str_civilization_gold_8[20];
extern char str_civ_ini_9[8];
extern char str_57344[8];
extern char str_chatshowsize_3[16];
extern char str_civilization_gold_9[20];
extern char str_chatlog_txt_3[12];
extern i8 parley_wnd_empty_title_str[4];
extern char str_parleycancel_3[16];
extern char str_noforeignhuman[16];
extern char str_cancelallied[16];
extern char str_cancelpeace[12];
extern char str_breakcease[12];
extern char str_parleyaccept[16];
extern char str_parleynothanks[16];
extern char str_parleycounteroffer[20];
extern char str_d_ss_franklinton_parleywin_cpp[32];
extern char str_leftright_left_leftright_right[40];
extern i8 chat_recv_newline_str[4];
extern i8 chat_recv_private_year_open_str[4];
extern i8 chat_recv_private_year_close_str[4];
extern i8 chat_recv_year_open_str[4];
extern i8 chat_recv_year_close_str[4];
extern i8 chat_send_newline_str[4];
extern i8 chat_send_recipient_sep_str[4];
extern i8 chat_send_year_open_str[4];
extern i8 chat_send_year_close_str[124];
extern char str_civ2_wonder_dll[16];
extern char str_notice_9[8];
extern char str_failed_to_load_civ2art_gif[28];
extern char str_civ2_video_wonder[20];
extern char str_avi_2[8];
extern u8 str_empty_116[4];
extern char str_vfwnotregistered_5[40];
extern int research_notice_mode;
extern char str_upgrade[8];
extern char str_automonarchy[16];
extern char str_startrev_2[12];
extern char str_republic[12];
extern char str_autorev[8];
extern char str_startrev_3[12];
extern char str_railroads[12];
extern char str_farmland[12];
extern char str_trade[8];
extern char str_seafaring[12];
extern char str_writing[8];
extern char str_goldenage[12];
extern char str_empty_117[4];
extern char str_empty_118[4];
extern char str_erhalten[12];
extern char str_von_den[8];
extern char str_empty_119[4];
extern char str_gape[8];
extern char str_barracks[12];
extern char str_endwonder_2[12];
extern char str_researchgoal[16];
extern char str_empty_120[4];
extern char str_empty_121[4];
extern char str_helpon[8];
extern char str_helpon_2[8];
extern char str_stealnone[12];
extern char str_researchnone[16];
extern char str_stealthese[12];
extern char str_researchthese[16];
extern char str_science[8];
extern char str_advice[8];
extern char str_research[12];
extern char str_civadvance[12];
extern char str_newxform[12];
extern char str_newfortress[12];
extern char str_newairlift[12];
extern char str_newrailroad[12];
extern char str_newfarmland[12];
extern char str_newparadrop[12];
extern char str_firstciv[92];
extern int spy_last_mission;
extern char str_pretextallied_2[16];
extern char str_pretext_2[8];
extern char str_wimpout[8];
extern char str_incidentallied[16];
extern char str_incidentterror[16];
extern char str_incidentwar[12];
extern char str_senatescandal[16];
extern char str_bond007[8];
extern char str_bond[8];
extern char str_bondglory[12];
extern char str_nailed[8];
extern char str_incident[12];
extern char str_civilwar[12];
extern char str_spymenu[8];
extern char str_spyoptions[12];
extern char str_enemyembassy[16];
extern char str_enemyinvestigate[20];
extern char str_nosteal[8];
extern char str_stealhard[12];
extern char str_stealspecific[16];
extern char str_curses[8];
extern char str_steal[8];
extern char str_sabotagespecific[20];
extern char str_sabotage[12];
extern char str_sabotageno[12];
extern char str_sabotagehard[16];
extern char str_sabotagetwo[12];
extern char str_sabotageone[12];
extern char str_watersupply[12];
extern char str_majorincident[16];
extern char str_plantednuke[12];
extern char str_2_2[4];
extern char str_norevolt[12];
extern char str_dissidents[12];
extern char str_dissidentoptions[20];
extern char str_chatspystart[16];
extern char str_incorruptible[16];
extern char str_desert[8];
extern char str_2_3[4];
extern char str_pick_up_unit_connection_to_server_timed[48];
extern char str_serverconnecttime_5[20];
extern char str_deserted[12];
extern char str_sabotageoptions[16];
extern char str_blewitup[12];
extern char str_shotdown[12];
extern char str_airlift[8];
extern char str_paradroptarget[16];
extern char str_paradroptarget2[16];
extern char str_paradroptarget1[16];
extern char str_paradrop[144];
extern i32 editor_value_clamped;
extern DibSurface *editor_bg_surface;
extern iptr editor_font_size;
extern i8 int_2d_fmt[4];
extern i8 advance_abbrev_no_strs[12];
extern i8 advance_abbrev_nil_strs[12];
extern i8 advance_abbrev_comma_str[4];
extern i8 advance_abbrev_err_strs[12];
extern char str_rules_5[8];
extern char str_rules_bak[12];
extern char str_rules_bak_2[12];
extern char str_rules_bak_3[12];
extern char str_rt_3[4];
extern char str_rules_bak_4[12];
extern char str_wt[4];
extern char str_rt_4[4];
extern char str_rules_bak_5[12];
extern char str_rules_bak_6[12];
extern char str_wt_2[4];
extern char str_rules_bak_7[12];
extern char str_warning_2[8];
extern char str_saving_changes_in_file_rules_bak[36];
extern char str_wt_3[136];
extern char str_city_3[8];
extern char str_city_tmp[12];
extern char str_city_tmp_2[12];
extern char str_city_tmp_3[12];
extern char str_rt_5[4];
extern char str_city_tmp_4[12];
extern char str_wt_4[4];
extern i8 city_file_stop_str[8];
extern char str_c_s[8];
extern char str_city_tmp_5[12];
extern char str_city_tmp_6[12];
extern i8 editor_text_dlg_empty_title[4];
extern char str_empty_122[4];
extern char str_city_4[8];
extern char str_nocity[108];
#pragma pack(push, 1)
struct spaceship_launch_sprite_xs_block {
    i8 spaceship_launch_sprite_xs[4];  /* 0062e250 */
    i8 spaceship_launch_sprite_ys[4];  /* 0062e254 */
    i8 spaceship_launch_sprite_widths[4];  /* 0062e258 */
    i8 spaceship_part_sprite_heights[116];  /* 0062e25c */
    SpaceshipWindow *spaceship_wnd;  /* 0062e2d0 */
};
#pragma pack(pop)
extern struct spaceship_launch_sprite_xs_block spaceship_launch_sprite_xs_block;
extern char str_s_s[8];
extern char str_empty_123[4];
extern char str_empty_124[4];
extern char str_ss_dll[8];
extern u8 str_empty_125[4];
extern u8 str_empty_126[4];
extern char str_empty_127[4];
extern char str_empty_128[4];
extern char str_0_000_6[8];
extern char str_empty_129[4];
extern char str_empty_130[4];
extern char str_empty_131[4];
extern char str_empty_132[4];
extern char str_00[4];
extern char str_empty_133[4];
extern char str_empty_134[4];
extern char str_empty_135[4];
extern char str_lancer[8];
extern char str_starten[8];
extern char str_civ2_video_launch_avi[24];
extern u8 str_empty_136[4];
extern char str_vfwnotregistered_6[72];
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
extern char str_empty_137[8];
extern i8 rules_improve_line_end_str[4];
extern i8 rules_endwonder_comment_sep_str[4];
extern i8 rules_endwonder_line_end_str[4];
extern char str_notice_10[8];
extern char str_improve_2[8];
extern char str_endwonder_3[12];
extern char str_error_updating_rules_s_3[24];
extern char str_file_i_o_error_3[16];
extern char str_imprname[12];
extern char str_improvements[16];
extern char str_help_4[8];
extern char str_imprmisc[12];
extern char str_notice_11[8];
extern u8 str_empty_138[4];
extern char str_pediaimprove[16];
extern char str_pedia[8];
extern i8 impr_editor_help_percent_str[4];
extern i8 impr_editor_help_percent2_str[4];
extern char str_editoras_gif[80];
extern i8 text_section_index_path[260];
extern i32 text_section_index_valid;
extern int text_section_index_count;
extern i8 text_index_path_sep_str[4];
extern char str_rt_6[4];
extern i8 text_lookup_path_sep_str[60];
extern i32 parley_offer_kinds[16];
extern i32 parley_item_header_sizes[21];
extern char str_failed_to_allocate_buffer_for_parley_tra[76];
extern char str_d_ss_franklinton_parleywin_transaction_c[44];
extern char str_charptr_pchunk_sizechunk0_charptr_pmessa[60];
extern char str_d_ss_franklinton_parleywin_transaction_c_2[44];
extern char str_charptr_pchunk_sizechunk1_charptr_pmessa[60];
extern char str_d_ss_franklinton_parleywin_transaction_c_3[44];
extern char str_strlen_parleydescription_0[32];
extern i8 parley_desc_transmitted_crlf_str[8];
extern char str_d_ss_franklinton_parleywin_transaction_c_4[44];
extern char str_strlen_parleydescription_2048[36];
extern i8 parley_desc_maps_offer_crlf_str[8];
extern i8 parley_desc_maps_kind_crlf_str[4];
extern i8 parley_desc_war_offer_crlf_str[8];
extern i8 parley_desc_war_head_crlf_str[4];
extern i8 parley_desc_gift_adv_crlf_str[8];
extern i8 parley_desc_gift_adv_head_crlf_str[4];
extern i8 parley_desc_gift_units_crlf_str[8];
extern i8 parley_desc_gift_units_head_crlf_str[4];
extern i8 parley_desc_gift_gold_crlf_str[8];
extern i8 parley_desc_gift_gold_head_crlf_str[4];
extern char str_empty_139[12];
extern i8 parley_desc_gift_cities_crlf_str[8];
extern i8 parley_desc_gift_cities_head_crlf_str[4];
extern i8 parley_desc_gift_maps_crlf_str[8];
extern i8 parley_desc_threat_crlf_str[8];
extern i8 parley_desc_barter_crlf_str[8];
extern i8 parley_desc_give_adv_crlf_str[4];
extern i8 parley_desc_give_gold_crlf_str[4];
extern char str_empty_140[12];
extern i8 parley_desc_give_units_crlf_str[4];
extern i8 parley_desc_give_cities_crlf_str[4];
extern i8 parley_desc_give_maps_crlf_str[4];
extern i8 parley_desc_give_war_crlf_str[4];
extern i8 parley_desc_give_treaty_crlf_str[4];
extern i8 parley_desc_barter_sep_crlf_str[4];
extern i8 parley_desc_want_adv_crlf_str[4];
extern i8 parley_desc_want_gold_crlf_str[4];
extern char str_empty_141[12];
extern i8 parley_desc_want_units_crlf_str[4];
extern i8 parley_exch_for_cities_crlf_str[4];
extern i8 parley_exch_for_maps_crlf_str[4];
extern i8 parley_exch_for_war_targets_crlf_str[4];
extern i8 parley_exch_for_treaty_crlf_str[4];
extern char str_d_ss_franklinton_parleywin_transaction_c_5[44];
extern char str_strlen_parleydescription_2048_2[36];
extern i8 parley_advances_comma_str[4];
extern i8 parley_advances_period_str[4];
extern i8 parley_advances_crlf_str[4];
extern i8 parley_gold_crlf_str[4];
extern i8 parley_units_open_paren_str[4];
extern i8 parley_units_close_paren_str[4];
extern i8 parley_units_comma_str[4];
extern i8 parley_units_period_str[4];
extern i8 parley_units_crlf_str[4];
extern i8 parley_cities_open_paren_str[4];
extern i8 parley_cities_close_paren_str[4];
extern i8 parley_cities_comma_str[4];
extern i8 parley_cities_period_str[4];
extern i8 parley_cities_crlf_str[4];
extern i8 parley_maps_crlf_str[4];
extern i8 parley_war_targets_comma_str[4];
extern i8 parley_war_targets_period_str[4];
extern i8 parley_war_targets_crlf_str[4];
extern char str_start_executeparleytransaction_message_d[44];
extern char str_end_executeparleytransaction_message_d[44];
extern i8 parley_treaty_crlf_str[212];
extern int mp_cheat_menu_allowed[1];
extern i8 game_options_version_sep_str[4];
extern char str_gameoptions[12];
extern char str_graphicoptions[16];
extern char str_lowmemory[12];
extern char str_multiplayeroptions[20];
extern char str_multiplayeroptions2[20];
extern i8 mp_options_vote_sep_str[4];
extern char str_pmchangeserver[16];
extern char str_pmchangeno[12];
extern char str_pmchangeyes[12];
extern char str_messageoptions[16];
extern char str_killorretire_connection_to_server_timed[48];
extern char str_serverconnecttime_6[20];
extern char str_killorretire_received_nm_kill_or_quit_ac[56];
extern char str_reallyretire[16];
extern char str_reallyquit_3[12];
extern char str_hiddenterrain[16];
extern char str_nopickmusicnew[16];
extern char str_pickmusicfanworlds[20];
extern char str_pickmusicscenario[20];
extern char str_pickmusicgold[16];
extern char str_pickmusic[12];
extern char str_nopickmusicnew_2[16];
extern char str_mainmenu_menu_exec_blocked_by_xd_flushse[68];
extern char str_reallyquit_4[12];
extern char str_newcredits_2[12];
extern char str_game_5[8];
extern char str_menu[8];
extern char str_kingdom[8];
extern char str_view[8];
extern char str_orders_2[8];
extern char str_advisors[12];
extern char str_world[8];
extern char str_cheat[8];
extern char str_editor[8];
extern char str_pedia_2[124];
extern int city_yield_tile_worked;
extern int city_turn_processing;
extern int city_turn_worker_realloc;
extern int city_turn_window_shown;
extern i32 city_turn_window_dirty;
extern i32 city_calc_cached_id;
extern i32 city_tile_job_found;
extern char str_acquire_wonder_connection_to_server_time[48];
extern char str_built[8];
extern char str_3[4];
extern char str_famine0[8];
extern char str_aqueduct[12];
extern char str_furthergrowth[16];
extern char str_upgraded[12];
extern char str_ghosttown[12];
extern char str_built_2[8];
extern char str_caravanbuilt[16];
extern char str_nofoodreport[16];
extern char str_caraconfirm[12];
extern char str_built_3[8];
extern char str_badbuild[12];
extern char str_badspace[12];
extern char str_manhattan[12];
extern char str_built_4[8];
extern char str_built2[8];
extern char str_badspace_2[12];
extern char str_movecapital[12];
extern char str_almostwonder[16];
extern char str_support[8];
extern char str_disorder[12];
extern char str_revolt[8];
extern char str_restored[12];
extern char str_weloveking[12];
extern char str_wedontloveking[16];
extern char str_pollution[12];
extern char str_chernobyl[12];
extern char str_inhock[8];
extern char str_decrease[12];
extern char str_foodshortage[16];
extern char str_astronauts[100];
extern int dialog_clamp_overflow;
extern i8 *pedia_missing_text;
extern i32 pedia_going_back;
extern i32 pedia_back_topic;
extern u8 str_empty_142[4];
extern i8 pedia_window_empty_title_str[4];
extern u8 str_empty_143[4];
extern char str_this_should_not_happen[28];
extern char str_this_should_not_happen_2[28];
extern char str_this_should_never_be_seen[32];
extern char str_this_should_not_happen_3[28];
extern char str_describe_3[12];
extern char str_advance_index_2[16];
extern char str_improvement_index_2[20];
extern char str_wonder_index_2[16];
extern char str_unit_index_2[12];
extern char str_government_index_2[20];
extern char str_terrain_index_2[16];
extern i8 pedia_grassland_shield_abbrev_str[4];
extern char str_concept_descriptions_2[24];
extern i8 pedia_index_marker_str[64];
extern int events_parse_verbose;
extern u8 events_parse_verbose_rest[48];
extern char str_desert_2[8];
extern char str_plains[8];
extern char str_grassland[12];
extern char str_forest[8];
extern char str_hills[8];
extern char str_mountains[12];
extern char str_tundra[8];
extern char str_arctic[8];
extern char str_swamp[8];
extern char str_jungle[8];
extern char str_ocean[8];
extern char str_anybody[8];
extern char str_triggerattacker[16];
extern char str_triggerdefender[16];
extern char str_triggerreceiver[16];
extern char str_could_not_find_s_check_spelling[36];
extern char str_anyunit[8];
extern char str_could_not_find_unit_s_check_spelling[88];
extern char str_sound_2[8];
extern char str_sound_3[8];
extern i8 events_kw_if_str[4];
extern char str_if_found_creating_new_event_structure[44];
extern char str_failed_to_create_new_event_structure[40];
extern char str_new_event_structure_created[32];
extern i8 events_kw_then_str[8];
extern char str_then_found[16];
extern char str_endif[8];
extern char str_endif_found[16];
extern char str_debug[8];
extern char str_debugging_filename_s[28];
extern char str_endevents[12];
extern char str_endevents_found[20];
extern char str_unitkilled[12];
extern char str_if_unitkilled_found_looking_for_unit_att[64];
extern char str_got_early_end_of_file[24];
extern char str_illegal_unitkilled_statement[32];
extern char str_unit[8];
extern char str_found_unit_s_id_d[24];
extern char str_found_unit_s_but_s_is_not_valid[36];
extern char str_attacker[12];
extern char str_found_attacker_s_king_id_d[32];
extern char str_found_attacker_s_but_s_is_not_valid[40];
extern char str_defender[12];
extern char str_found_defender_s_king_id_d[32];
extern char str_found_defender_s_but_s_is_not_valid[40];
extern char str_negotiation[12];
extern char str_if_negotiation_found_looking_for_talker[80];
extern char str_got_early_end_of_file_2[24];
extern char str_illegal_negotiation_statement[32];
extern char str_talker[8];
extern char str_found_talker_s_king_id_d[32];
extern char str_found_talker_s_but_s_is_not_valid[40];
extern char str_talkertype[12];
extern char str_human[8];
extern char str_computer[12];
extern char str_humanorcomputer[16];
extern char str_found_talkertype_s[24];
extern char str_found_talkertype_s_but_s_is_not_valid[44];
extern char str_listener[12];
extern char str_found_listener_s_king_id_d[32];
extern char str_found_listener_s_but_s_is_not_valid[40];
extern char str_listenertype[16];
extern char str_human_2[8];
extern char str_computer_2[12];
extern char str_humanorcomputer_2[16];
extern char str_found_listenertype_s[24];
extern char str_found_listenertype_s_but_s_is_not_valid[44];
extern char str_noschism[12];
extern char str_if_noschism_found_looking_for_whom[40];
extern char str_got_early_end_of_file_3[24];
extern char str_illegal_noschism_statement[28];
extern char str_defender_2[12];
extern char str_found_defender_s_king_id_d_2[32];
extern char str_found_defender_s_but_s_is_not_valid_2[40];
extern char str_receivedtechnology[20];
extern char str_if_receivedtechnology_found_looking_for[64];
extern char str_got_early_end_of_file_4[24];
extern char str_illegal_receivedtechnology_statement[40];
extern char str_receiver[12];
extern char str_found_receiver_s_king_id_d[32];
extern char str_found_receiver_s_but_s_is_not_valid[40];
extern char str_technology[12];
extern char str_found_technology_s_technology_id_d[40];
extern char str_found_technology_s_but_s_is_not_valid[44];
extern char str_citytaken[12];
extern char str_if_citytaken_found_looking_for_city_atta[60];
extern char str_got_early_end_of_file_5[24];
extern char str_illegal_citytaken_statement[32];
extern char str_attacker_2[12];
extern char str_found_attacker_s_king_id_d_2[32];
extern char str_found_attacker_s_but_s_is_not_valid_2[40];
extern char str_defender_3[12];
extern char str_found_defender_s_king_id_d_3[32];
extern char str_found_defender_s_but_s_is_not_valid_3[40];
extern char str_city_5[8];
extern char str_found_city_s[16];
extern i8 events_kw_turn_str[8];
extern char str_if_turn_found_looking_for_turn[36];
extern char str_got_early_end_of_file_6[24];
extern char str_illegal_turn_statement[24];
extern char str_turn[8];
extern char str_every[8];
extern char str_found_turn_s[16];
extern char str_found_turn_s_but_s_is_not_valid[36];
extern char str_turninterval[16];
extern char str_if_turninterval_found_looking_for_interv[48];
extern char str_got_early_end_of_file_7[24];
extern char str_illegal_turninterval_statement[32];
extern char str_interval[12];
extern char str_found_interval_s[20];
extern char str_found_interval_s_but_s_is_not_valid[40];
extern char str_randomturn[12];
extern char str_if_randomturn_found_looking_for_denomina[48];
extern char str_got_early_end_of_file_8[24];
extern char str_illegal_randomturn_statement[32];
extern char str_denominator[16];
extern char str_found_denominator_s_but_s_is_not_valid[44];
extern char str_found_denominator_s[24];
extern char str_found_denominator_s_but_s_is_not_valid_2[44];
extern char str_scenarioloaded_3[16];
extern char str_if_scenarioloaded[24];
extern i8 events_kw_text_str[8];
extern char str_then_text_found_looking_for_endtext[40];
extern char str_endtext[8];
extern char str_got_early_end_of_file_9[24];
extern char str_illegal_text_statement[24];
extern char str_found_endtext[16];
extern char str_changeterrain[16];
extern char str_then_changeterrain_found_looking_for_ter[64];
extern char str_got_early_end_of_file_10[24];
extern char str_illegal_changeterrain_statement[36];
extern char str_terraintype[16];
extern char str_found_terraintype_s_terrainid_d[40];
extern char str_found_terraintype_s_but_s_is_not_valid[44];
extern char str_maprect[8];
extern char str_found_maprect[16];
extern char str_got_early_end_of_file_11[24];
extern char str_found_location_d_d_d[28];
extern char str_createunit[12];
extern char str_then_createunit_found_looking_for_unit_o[92];
extern char str_got_early_end_of_file_12[24];
extern char str_illegal_createunit_statement[32];
extern char str_unit_2[8];
extern char str_found_unit_s_unit_id_d[28];
extern char str_found_unit_s_but_s_is_not_valid_2[36];
extern char str_owner[8];
extern char str_found_owner_s_king_id_d[32];
extern char str_found_owner_s_but_s_is_not_valid[40];
extern char str_veteran[12];
extern i8 events_bool_yes_str[4];
extern i8 events_bool_true_str[8];
extern i8 events_bool_no_str[4];
extern char str_false[8];
extern char str_found_veteran_s_but_s_is_not_valid[40];
extern char str_homecity[12];
extern char str_found_homecity_s[20];
extern char str_locations[12];
extern char str_found_locations[20];
extern char str_got_early_end_of_file_13[24];
extern char str_found_location_d_d_d_2[28];
extern char str_endlocations[16];
extern char str_found_endlocations[20];
extern char str_changemoney[12];
extern char str_then_changemoney_found_looking_for_recei[56];
extern char str_got_early_end_of_file_14[24];
extern char str_illegal_changemoney_statement[32];
extern char str_receiver_2[12];
extern char str_found_receiver_s_king_id_d_2[32];
extern char str_found_receiver_s_but_s_is_not_valid_2[40];
extern char str_amount[8];
extern char str_found_amount_s_integer_d[32];
extern char str_justonce[12];
extern char str_then_justonce_found_not_looking_for_anyt[60];
extern char str_dontplaywonders[16];
extern char str_then_dontplaywonders_found_not_looking_f[68];
extern char str_makeaggression[16];
extern char str_then_makeaggression_found_looking_for_wh[52];
extern char str_got_early_end_of_file_15[24];
extern char str_illegal_makeaggression_statement[36];
extern i8 events_who_prefix_str[8];
extern char str_found_who_s_king_id_d[28];
extern char str_found_who_s_but_s_is_not_valid[36];
extern char str_whom[8];
extern char str_found_whom_s_king_id_d[28];
extern char str_found_whom_s_but_s_is_not_valid[36];
extern char str_destroyacivilization[24];
extern char str_then_destroyacivilization_found_looking[56];
extern char str_got_early_end_of_file_16[24];
extern char str_illegal_destroyacivilization_statement[40];
extern char str_whom_2[8];
extern char str_found_whom_s_king_id_d_2[28];
extern char str_found_whom_s_but_s_is_not_valid_2[36];
extern char str_givetechnology[16];
extern char str_then_givetechnology_found_looking_for_wh[48];
extern char str_got_early_end_of_file_17[24];
extern char str_illegal_givetechnology_statement[36];
extern char str_receiver_3[12];
extern char str_found_receiver_s_king_id_d_3[32];
extern char str_found_receiver_s_but_s_is_not_valid_3[40];
extern char str_technology_2[12];
extern char str_found_technology_s_integer_d[36];
extern char str_moveunit[12];
extern char str_then_moveunit_found_looking_for_unit_own[80];
extern char str_got_early_end_of_file_18[24];
extern char str_illegal_moveunit_statement[28];
extern char str_unit_3[8];
extern char str_found_unit_s_unit_id_d_2[28];
extern char str_found_unit_s_but_s_is_not_valid_3[36];
extern char str_owner_2[8];
extern char str_found_owner_s_king_id_d_2[32];
extern char str_found_owner_s_but_s_is_not_valid_2[40];
extern char str_maprect_2[8];
extern char str_found_maprect_2[16];
extern char str_got_early_end_of_file_19[24];
extern char str_found_location_d_d_d_3[28];
extern char str_moveto[8];
extern char str_found_moveto[16];
extern char str_got_early_end_of_file_20[24];
extern char str_found_moveto_d_d[24];
extern char str_numbertomove[16];
extern char str_found_numbertomove_s_integer_d[36];
extern i8 events_numbertomove_all_str[4];
extern char str_found_numbertomove_s[24];
extern char str_found_numbertomove_s_but_s_is_not_valid[44];
extern char str_playcdtrack[12];
extern char str_then_playcdtrack_found_looking_for_track[52];
extern char str_got_early_end_of_file_21[24];
extern char str_found_cd_track_d[20];
extern char str_playwavefile[16];
extern char str_then_playwavefile_found_looking_for_file[52];
extern char str_got_early_end_of_file_22[24];
extern char str_found_wave_file_name_s[28];
extern char str_badeventsfile[1200];
extern int city_window_exit_requested;
extern char city_modal_notice_active;
extern u8 city_modal_notice_active_rest[3];
extern i8 city_window_exit_btn_pending;
extern u8 city_window_exit_btn_pending_rest[3];
extern char city_change_button_variant;
extern u8 city_change_button_variant_rest[3];
extern int city_tile_pick_zoom_cached;
extern int city_unit_dialog_busy;
extern i8 city_dlg_close_pending;
extern u8 city_dlg_close_pending_rest[3];
extern int city_window_bg_size_level;
extern i8 city_resmap_draw_dx_table[24];
extern i8 city_resmap_draw_dy_table[24];
extern int city_dlg_close_start_ticks;
extern char str_city_gif[12];
extern char str_citymodal[12];
extern char str_elviserr[12];
extern char str_empty_144[4];
extern char str_alreadysold[12];
extern char str_canthockthis[16];
extern char str_hockthis[12];
extern char str_childclick[12];
extern char str_unitoptions[12];
extern char str_1_5[4];
extern char str_x_3[4];
extern char str_disorder_2[12];
extern char str_disorder2[12];
extern char str_disorder3[12];
extern char str_citystuff[12];
extern char str_citywin_city_button_buy_blocked_by_xd_fl[72];
extern char str_complete0[12];
extern char str_citywin_city_button_change_blocked_by_xd[76];
extern char str_military[12];
extern char str_advice_2[8];
extern char str_empty_145[4];
extern char str_advice_3[8];
extern char str_default[12];
extern char str_advice_4[8];
extern char str_empty_146[4];
extern char str_domestic[12];
extern char str_advice_5[8];
extern char str_wonder[8];
extern char str_advice_6[8];
extern char str_default_2[12];
extern char str_advice_7[8];
extern char str_production[12];
extern char str_empty_147[4];
extern char str_empty_148[4];
extern char str_empty_149[4];
extern char str_empty_150[4];
extern char str_empty_151[4];
extern char str_empty_152[4];
extern char str_empty_153[4];
extern char str_empty_154[4];
extern char str_automode[12];
extern char str_freebie[8];
extern char str_prodchange[12];
extern char str_citywin_city_button_rename_blocked_by_xd[76];
extern char str_renamecity[12];
extern char str_citywin_city_button_view_blocked_by_xd_f[72];
extern char str_empty_155[28];
extern char str_zzzzzzzzzzzzzzzzzzzzzzzzz[28];
extern char str_citywin_city_mouse_blocked_by_xd_flushse[240];
extern int net_popup_queue_head;
extern i32 net_popup_queue_tail;
extern int parley_reveal_units_prompt_open;
extern int parley_reveal_city_prompt_open;
extern char str_retiredie[12];
extern char str_retireai[12];
extern char str_barbarians_2[12];
extern char str_globalwarming_2[16];
extern char str_fearwarming_2[12];
extern char str_eaglehaslanded_2[16];
extern char str_scenarioends_2[16];
extern char str_scenarioend_2[12];
extern char str_planretire_2[12];
extern char str_doretire_2[12];
extern char str_launched[12];
extern char str_spacereturns_2[16];
extern char str_spacedestroyed_2[16];
extern char str_destroyed_2[12];
extern char str_caravan_6[8];
extern char str_foodcaravan_4[12];
extern char str_caravanother_3[16];
extern char str_startwonder_2[12];
extern char str_switchwonder_2[16];
extern char str_abandonwonder_2[16];
extern char str_paradrop_2[12];
extern char str_schism_2[8];
extern char str_canescape[12];
extern char str_escape[8];
extern char str_capturewonder[16];
extern char str_lostwonder[12];
extern char str_citycapture[12];
extern char str_citywinally[12];
extern char str_cityloseally[16];
extern char str_citycapture2[16];
extern char str_partisans[12];
extern char str_promoted[12];
extern char str_incorruptible_2[16];
extern char str_deserted_2[12];
extern char str_desert_3[8];
extern char str_desert2[8];
extern char str_blewitup_2[12];
extern char str_bond007_2[8];
extern char str_bond_2[8];
extern char str_bondglory_2[12];
extern char str_useweapons[12];
extern char str_sdi[4];
extern char str_changed[8];
extern char str_overthrown[12];
extern char str_breakcease_2[12];
extern char str_sneak[8];
extern char str_missileattack[16];
extern char str_pearlharbor[12];
extern char str_battery2[12];
extern char str_battery[8];
extern char str_scramble[12];
extern char str_amphibmotize[16];
extern char str_multiplelose[16];
extern char str_multiplewin[12];
extern char str_ransom[8];
extern char str_activateally_2[16];
extern char str_allyhelps_2[12];
extern char str_cancelpeace_2[12];
extern char str_allyunderattack[16];
extern char str_allyattacking[16];
extern char str_parleyrequest[16];
extern char str_parleycancel_4[16];
extern char str_upgrade_2[8];
extern char str_newplayer_2[12];
extern char str_newturntimerclient[20];
extern char str_newturntimerno[16];
extern char str_newturntimeryes[16];
extern char str_pmchangeclient[16];
extern char str_pmchangeno_2[12];
extern char str_pmchangeyes_2[12];
extern char str_manhattan_2[12];
extern char str_built2_2[8];
extern char str_movecapital_2[12];
extern char str_astronauts_2[12];
extern char str_almostwonder_2[16];
extern char str_stillwonder1[16];
extern char str_stillwonder2[16];
extern char str_endwonder_4[12];
extern char str_barbariansland[16];
extern char str_warends[8];
extern char str_militaryaid1[16];
extern char str_militaryaid2[16];
extern char str_lostclient_2[12];
extern char str_goldenage_2[12];
extern char str_enemyembassy_2[16];
extern char str_enemyinvestigate_2[20];
extern char str_steal_2[8];
extern char str_foiledagain[12];
extern char str_watersupply_2[12];
extern char str_plantednuke_2[12];
extern char str_plantednuke2[16];
extern char str_civilwar_2[12];
extern char str_sabotageone_2[12];
extern char str_sabotagetwo_2[12];
extern char str_upmine[8];
extern char str_upyours[8];
extern char str_upyourstoo[12];
extern char str_mercdeclare_2[12];
extern char str_tookciv[8];
extern char str_revealunitorigins[20];
extern char str_revealcityinfo[276];
extern char str_message_lost_in_alpha_enqueuemessage[40];
extern char str_from_d_size_d[20];
extern char str_d_ss_franklinton_netmessagequeue_cpp[40];
extern char str_alphahead_0_alphahead_400[36];
extern char str_d_ss_franklinton_netmessagequeue_cpp_2[40];
extern char str_alphatail_0_alphatail_400[36];
extern char str_d_ss_franklinton_netmessagequeue_cpp_3[40];
extern char str_msgqueue_alphatail_pdata[28];
extern char str_enqueuemessage_alpha_failed_to_allocate[72];
extern char str_message_lost_in_enqueuemessage[36];
extern char str_from_d_size_d_2[20];
extern char str_d_ss_franklinton_netmessagequeue_cpp_4[40];
extern char str_head_400_head_max_msgs_in_queue[40];
extern char str_d_ss_franklinton_netmessagequeue_cpp_5[40];
extern char str_tail_400_tail_max_msgs_in_queue[40];
extern char str_d_ss_franklinton_netmessagequeue_cpp_6[40];
extern char str_msgqueue_tail_pdata[24];
extern char str_enqueuemessage_primary_failed_to_allocat[216];
extern int victory_screen_wnd;
extern i32 destroyed_civs_show_date;
extern char str_civ2_mk_dll_2[12];
extern u8 str_empty_156[4];
extern u8 str_empty_157[4];
extern char str_slam[8];
extern char str_empty_158[12];
extern i32 council_video_busy_i32;
extern CouncilWindow *council_wnd;
extern char council_active;
extern u8 council_active_rest[3];
extern u8 str_empty_159[4];
extern char str_councilcheat0[16];
extern char str_councilcheat1[16];
extern char str_councilcheat2[16];
extern char str_m_2[4];
extern char str_s[4];
extern char str_t[4];
extern char str_f[4];
extern char str_a_2[4];
extern char str_council[8];
extern char str_anarchy[8];
extern u8 str_empty_160[4];
extern char str_civ2_video[12];
extern char str_avi_3[8];
extern char str_vfwnotregistered_7[20];
extern char str_er_2[40];
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
extern char str_empty_161[8];
extern i8 terrain_txt_line_end_str[4];
extern char str_notice_12[8];
extern char str_terrain_2[8];
extern char str_error_updating_rules_s_4[24];
extern char str_file_i_o_error_4[16];
extern char str_terrname[12];
extern char str_terrain_3[8];
extern char str_help_5[8];
extern char str_forest_2[8];
extern char str_helpforest[12];
extern char str_help_6[8];
extern char str_hills_2[8];
extern char str_helphills[12];
extern char str_help_7[8];
extern char str_mountains_2[12];
extern char str_helpmountains[16];
extern char str_help_8[8];
extern char str_river[8];
extern char str_helpriver[12];
extern char str_help_9[8];
extern char str_coast[8];
extern char str_helpcoast[12];
extern char str_help_10[8];
extern char str_terrmisc[12];
extern char str_helpterrmisc[16];
extern char str_help_11[8];
extern char str_notice_13[8];
extern u8 str_empty_162[4];
extern char str_editorpt_gif_3[156];
extern uint dlg_check_bits;
extern int dlg_help_pressed;
extern u8 dlg_help_pressed_rest[4];
extern i32 setup_select_opponents;
extern int setup_accelerated_start;
extern i32 setup_dialog_pos_state;
extern int setup_side_picture_id;
extern char str_civ_ini_10[8];
extern char str_nettimeout[12];
extern char str_civilization_gold_10[20];
extern char str_civ_ini_11[8];
extern i8 net_ini_timeout_default_str[4];
extern char str_nettimeout_2[12];
extern char str_civilization_gold_11[20];
extern char str_civ_ini_12[8];
extern char str_adapter[8];
extern char str_civilization_gold_12[20];
extern char str_civ_ini_13[8];
extern i8 net_ini_adapter_default_str[4];
extern char str_adapter_2[8];
extern char str_civilization_gold_13[20];
extern char str_civ_ini_14[8];
extern char str_maxplayers_3[12];
extern char str_civilization_gold_14[20];
extern char str_civ_ini_15[8];
extern i8 net_ini_maxplayers_clear_str[4];
extern char str_maxplayers_4[12];
extern char str_civilization_gold_15[20];
extern char str_multitype2[12];
extern char str_d_ss_franklinton_startup_multiplayer_cpp[44];
extern char str_event_0_event_3[28];
extern char str_difficulty_5[12];
extern char str_enemies2[12];
extern char str_barbarity_2[12];
extern char str_rules_6[8];
extern char str_advanced_2[12];
extern char str_accelerated_2[12];
extern char str_advancedmp_2[12];
extern char str_gametimer[12];
extern char str_gametimer_2[12];
extern char str_customgametimer[16];
extern i8 setup_timer_minsec_sep_str[4];
extern char str_newturntimerserver[20];
extern char str_newturntimerno_2[16];
extern char str_newturntimeryes_2[16];
extern char str_gender_5[8];
extern char str_serverquit[12];
extern char str_lostserver_2[12];
extern char str_tribe_2[8];
extern char str_game_6[8];
extern char str_serverquit_2[12];
extern char str_lostserver_3[12];
extern char str_joinedmax_2[12];
extern char str_serverquit_3[12];
extern char str_lostserver_4[12];
extern char str_serverquit_4[12];
extern char str_lostserver_5[12];
extern char str_serverconnectfail_3[20];
extern char str_alreadychosen_2[16];
extern char str_serverquit_5[12];
extern char str_lostserver_6[12];
extern char str_customtribe_2[12];
extern char str_serverquit_6[12];
extern char str_lostserver_7[12];
extern char str_customtribe2_2[16];
extern char str_serverquit_7[12];
extern char str_lostserver_8[12];
extern char str_name_5[8];
extern char str_serverquit_8[12];
extern char str_lostserver_9[12];
extern char str_customcity_2[12];
extern char str_serverquit_9[12];
extern char str_lostserver_10[12];
extern i8 setup_input_blank_default[4];
extern char str_emailaddress[16];
extern char str_serverquit_10[12];
extern char str_lostserver_11[12];
extern i8 dialog_title_label_space_str[4];
extern char str_scenario_3[12];
extern char str_scenarioloaded_4[16];
extern char str_difficulty_6[12];
extern char str_advancedmp_3[12];
extern char str_empty_163[4];
extern char str_empty_164[4];
extern char str_name_6[8];
extern char str_gender_6[8];
extern char str_hotseat2[12];
extern char str_joingame1[12];
extern char str_joingame0[12];
extern char str_joingame3[12];
extern char str_opponent_2[12];
extern char str_d_ss_franklinton_startup_multiplayer_cpp_2[44];
extern char str_gnetmgr_bserver_2[16];
extern char str_empty_165[4];
extern char str_empty_166[4];
extern char str_name_7[8];
extern char str_gender_7[232];
extern i8 pedia_improvement_cost_fmt[4];
extern char str_d_ss_franklinton_parleywin_add_dialog_cp[44];
extern char str_leftright_left_leftright_right_2[40];
extern char str_d_ss_franklinton_parleywin_add_dialog_cp_2[44];
extern char str_thedialog_pdt_null_pdt_null_pdt_count[48];
extern u8 str_empty_167[4];
extern char str_000000[8];
extern char str_nointel_2[8];
extern char str_parleyaccept2[56];
extern char str_barbariansland_2[16];
extern char str_barbarians_3[12];
extern char str_city_6[8];
extern char str_barbarians_4[12];
extern char str_city_7[8];
extern char str_ransomcity[24];
extern iptr events_editor_backup;
extern char str_then[8];
extern char str_text[8];
extern i8 events_text_line_newline_str[4];
extern char str_endtext_2[12];
extern char str_moveunit_2[12];
extern char str_unit_s[12];
extern char str_owner_s[12];
extern char str_maprect_3[12];
extern char str_d_d[8];
extern i8 events_moveunit_rect_sep_str[4];
extern char str_moveto_2[12];
extern char str_d_d_2[8];
extern char str_numbertomove_all[20];
extern char str_numbertomove_d[20];
extern char str_createunit_2[12];
extern char str_unit_s_2[12];
extern char str_owner_s_2[12];
extern i8 events_veteran_yes_no_strs[8];
extern char str_veteran_s[12];
extern char str_homecity_s[16];
extern char str_locations_2[12];
extern char str_d_d_3[8];
extern char str_endlocations_2[16];
extern char str_changemoney_2[16];
extern char str_receiver_s[16];
extern char str_amount_d[12];
extern char str_playwavefile_2[16];
extern i8 events_wave_file_fmt[4];
extern char str_makeaggression_2[16];
extern char str_who_s[8];
extern char str_whom_s[12];
extern char str_justonce_2[12];
extern char str_hastriggered[16];
extern char str_playcdtrack_2[16];
extern i8 events_cd_track_fmt[4];
extern char str_dontplaywonders_2[20];
extern char str_changeterrain_2[16];
extern char str_terraintype_d[16];
extern char str_maprect_4[12];
extern char str_d_d_4[8];
extern i8 events_terrain_rect_sep_str[4];
extern i8 events_terrain_rect_end_str[4];
extern char str_destroyacivilization_2[24];
extern char str_whom_s_2[12];
extern char str_whom_triggerreceiver[24];
extern char str_whom_triggerattacker[24];
extern char str_whom_triggerdefender[24];
extern char str_givetechnology_2[16];
extern char str_receiver_s_2[16];
extern char str_receiver_triggerreceiver[28];
extern char str_receiver_triggerattacker[28];
extern char str_receiver_triggerdefender[28];
extern char str_technology_d[16];
extern char str_if[8];
extern char str_unitkilled_2[12];
extern char str_unit_s_3[12];
extern char str_attacker_s[16];
extern char str_defender_s[16];
extern char str_citytaken_2[12];
extern char str_city_s[12];
extern char str_attacker_s_2[16];
extern char str_defender_s_2[16];
extern char str_turn_2[8];
extern char str_turn_d[12];
extern char str_turninterval_2[16];
extern char str_interval_d[16];
extern char str_negotiation_2[16];
extern char str_talker_s[12];
extern char str_talkertype_2[12];
extern char str_human_3[8];
extern char str_computer_3[12];
extern char str_humanorcomputer_3[20];
extern char str_listener_s[16];
extern char str_listenertype_2[16];
extern char str_human_4[8];
extern char str_computer_4[12];
extern char str_humanorcomputer_4[20];
extern char str_scenarioloaded_5[16];
extern char str_randomturn_2[12];
extern char str_denominator_d[16];
extern char str_noschism_2[12];
extern char str_defender_s_3[16];
extern char str_receivedtechnology_2[20];
extern char str_technology_d_2[16];
extern char str_receiver_s_3[16];
extern char str_endif_2[8];
extern i8 events_list_end_str[4];
extern char str_events_3[8];
extern char str_events_bak[12];
extern char str_events_bak_2[12];
extern char str_events_bak_3[12];
extern char str_rt_7[4];
extern char str_events_bak_4[12];
extern char str_wt_5[4];
extern char str_beginevents_2[16];
extern char str_beginevents_3[16];
extern char str_endevents_2[12];
extern char str_endevents_3[12];
extern char str_endevents_4[12];
extern char str_anybody_2[8];
extern char str_triggerattacker_2[16];
extern char str_triggerdefender_2[16];
extern char str_triggerreceiver_2[16];
extern char str_anyunit_2[8];
extern char str_triggerreceiver_3[16];
extern char str_triggerattacker_3[16];
extern char str_triggerdefender_3[16];
extern char str_notice_14[8];
extern char str_error_updating_events_s[28];
extern char str_file_i_o_error_5[16];
extern char str_helpunitkilled[16];
extern char str_helpcitytaken[16];
extern char str_helpturn[12];
extern char str_helpturninterval[20];
extern char str_helpnegotiation[16];
extern char str_helpscenarioloaded[20];
extern char str_helprandomturn[16];
extern char str_helpnoschism[16];
extern char str_helpreceivedtechnology[24];
extern char str_help_12[8];
extern char str_helptext[12];
extern char str_helpmoveunit[16];
extern char str_helpcreateunit[16];
extern char str_helpchangemoney[16];
extern char str_helpplaywavefile[20];
extern char str_helpmakeaggression[20];
extern char str_helpjustonce[16];
extern char str_helpplaycdtrack[16];
extern char str_helpdontplaywonders[20];
extern char str_helpchangeterrain[20];
extern char str_helpdestroyacivilization[28];
extern char str_helpgivetechnology[20];
extern char str_help_13[8];
extern char str_events_4[8];
extern char str_help_14[8];
extern char str_d_unitkilled_s_attacker_s_defender_s[44];
extern char str_d_unitkilled[16];
extern char str_d_citytaken_s_attacker_s_defender_s[44];
extern char str_d_citytaken[16];
extern char str_d_turn_d[16];
extern char str_d_turninterval_d[24];
extern char str_human_5[8];
extern char str_computer_5[12];
extern char str_humanorcomputer_5[16];
extern char str_human_6[8];
extern char str_computer_6[12];
extern char str_humanorcomputer_6[16];
extern char str_d_negotiation_talker_s_s_listener_s_s[52];
extern char str_d_negotiation[16];
extern char str_d_scenarioloaded[20];
extern char str_d_randomturn_denominator_d[32];
extern char str_d_randomturn[16];
extern char str_d_noschism_s[20];
extern char str_d_noschism[16];
extern char str_d_receivedtechnology_s_whom_s[36];
extern char str_d_receivedtechnology[24];
extern char str_text_s[12];
extern i8 events_editor_text_caption[8];
extern char str_moveunit_s_owner_s[24];
extern char str_moveunit_3[12];
extern i8 events_editor_yes_no_strs[8];
extern char str_createunit_s_owner_s_veteran_s_homecity[48];
extern char str_createunit_3[12];
extern char str_changemoney_receiver_s_amount_d[36];
extern char str_changemoney_3[12];
extern char str_playwavefile_s[20];
extern char str_playwavefile_3[16];
extern char str_makeaggression_aggressor_s_victim_s[40];
extern char str_makeaggression_3[16];
extern char str_justonce_3[12];
extern char str_hastriggered_2[16];
extern char str_playcdtrack_d[20];
extern char str_playcdtrack_3[12];
extern char str_dontplaywonders_3[16];
extern char str_changeterrain_s[20];
extern char str_destroyacivilization_s[28];
extern char str_destroyacivilization_triggerreceiver[40];
extern char str_destroyacivilization_triggerattacker[40];
extern char str_destroyacivilization_triggerdefender[40];
extern char str_givetechnology_receiver_s_technology_s[44];
extern char str_triggerreceiver_4[16];
extern char str_triggerattacker_4[16];
extern char str_triggerdefender_4[16];
extern char str_givetechnology_receiver_s_technology_s_2[44];
extern char str_givetechnology_3[16];
extern char str_deleteitem[12];
extern char str_anyunit_3[8];
extern char str_anybody_3[8];
extern char str_anybody_4[8];
extern i8 events_city_default_str;
extern u8 events_city_default_str_rest[3];
extern char str_cityname[12];
extern char str_anybody_5[8];
extern char str_anybody_6[8];
extern char str_turncount[12];
extern char str_turninterval_3[16];
extern char str_anybody_7[8];
extern char str_talkertype_3[12];
extern char str_anybody_8[8];
extern char str_talkertype_4[12];
extern char str_randomdenom[12];
extern char str_anybody_9[8];
extern char str_anybody_10[8];
extern char str_triggers[12];
extern char str_deleteitem_2[12];
extern char str_anyunit_4[8];
extern char str_d_d_d_d_d_d_d_d[24];
extern char str_maprect_5[8];
extern char str_d_d_5[8];
extern char str_location[12];
extern char str_numunits[12];
extern char str_veteran_2[8];
extern i8 events_homecity_default_str[4];
extern char str_homecityname[16];
extern char str_d_d_6[8];
extern char str_locationnum[12];
extern char str_amount_2[8];
extern i8 events_wavfile_default_str[4];
extern char str_wavfile[8];
extern i8 events_wav_ext_str[8];
extern char str_cdtrack[8];
extern char str_d_d_d_d_d_d_d_d_2[24];
extern char str_maprect_6[8];
extern char str_actions[8];
extern char str_actions_2[8];
extern i8 events_pool_free_fmt[688];
extern iptr frame_border_tile;
extern iptr ui_font_height;
extern int ui_large_screen;
extern int frame_bevel_width;
extern int frame_padding;
extern i32 frame_highlight_color;
extern i32 frame_shadow_color;
extern int frame_caption_height;
extern int frame_border_width;
extern int sys_dlgframe_w2;
extern int sys_dlgframe_h2;
extern i8 caption_status_open_paren_str[4];
extern i8 caption_ai_fr_str[4];
extern i8 caption_ai_str[4];
extern i8 caption_moves_space_str[4];
extern i8 caption_timer_sep_str[4];
extern i8 caption_close_paren_str[4];
extern i8 caption_simul_open_paren_str[4];
extern i8 caption_simul_ai_fr_str[4];
extern i8 caption_simul_ai_str[4];
extern i8 caption_simul_space_str[4];
extern i8 caption_simul_timer_sep_str[4];
extern i8 caption_simul_close_paren_str[4];
extern i8 caption_timer_open_paren_str[4];
extern i8 caption_timer_close_paren_str[4];
extern i8 frame_default_title_str[24];
extern i8 filename_valid_chars[128];
extern i32 cheat_terrain_saved_bits;
extern int cheat_unit_last_human_civ;
extern i8 cheat_unit_owner_civ;
extern u8 cheat_unit_owner_civ_rest[3];
extern int cheat_unit_last_type;
extern char cheat_unit_veteran;
extern u8 cheat_unit_veteran_rest[3];
extern char cheat_unit_show_obsolete;
extern u8 cheat_unit_show_obsolete_rest[3];
extern char cheat_unit_ignore_prereq;
extern u8 cheat_unit_ignore_prereq_rest[3];
extern u8 str_empty_168[4];
extern char str_newfolder[12];
extern char str_badname[8];
extern char str_scenario_4[12];
extern char str_direxists[12];
extern char str_sound_4[8];
extern i8 scenario_path_sep_str[4];
extern i8 scenario_file_ext_str[8];
extern char str_reallycheat[12];
extern char str_warning_3[8];
extern char str_help_15[8];
extern char str_warning_4[8];
extern char str_help_16[8];
extern char str_pickplayer[12];
extern char str_debug_2[8];
extern char str_passwordnocheat1[20];
extern char str_passwordnocheat2[20];
extern char str_reallycheat_2[12];
extern char str_warning_5[8];
extern char str_help_17[8];
extern char str_gavetech[12];
extern char str_tooktech[12];
extern char str_edittech[12];
extern char str_empty_169[4];
extern char str_empty_170[4];
extern char str_terrain1[12];
extern char str_debug_3[8];
extern char str_terrain_4[8];
extern char str_debug_4[8];
extern char str_terrain1_2[12];
extern char str_debug_5[8];
extern char str_createunit_4[12];
extern char str_debug_6[8];
extern char str_revealmap[12];
extern char str_debug_7[8];
extern char str_sethuman[12];
extern char str_debug_8[8];
extern char str_gameyear[12];
extern char str_killciv[8];
extern char str_debug_9[8];
extern char str_money[8];
extern char str_unitedit[12];
extern char str_unithitpoints[16];
extern char str_edithomecity[16];
extern char str_supplysearch_2[16];
extern char str_cityedit[12];
extern char str_setcitysize[12];
extern char str_copycity[12];
extern char str_setcityshields[16];
extern char str_editking[12];
extern char str_edittreaties[16];
extern char str_lastcontact[12];
extern char str_editattitude[16];
extern char str_editbetray[12];
extern char str_editprogress[16];
extern char str_editkingname[16];
extern char str_empty_171[4];
extern char str_empty_172[4];
extern char str_editvictory[12];
extern char str_editvictoryobj[16];
extern char str_editrules[12];
extern char str_editscen[12];
extern char str_editparadigm[16];
extern char str_editincrement[16];
extern char str_editstartyear[16];
extern char str_editmaxturns[16];
extern char str_scenname[188];
extern char str_civ2_intro_dll[20];
extern char str_civilization_ii_once_only[28];
extern char str_mswindowclass[24];
extern int mp_turn_timer_handle;
extern int mp_turn_seconds_left;
extern i32 g_timer_last_tick;
extern int mp_mouse_watchdog_timer;
extern char mp_window_drag_active;
extern u8 mp_window_drag_active_rest[3];
extern i32 mp_window_drag_start_ticks;
extern i32 mp_window_drag_flag_i32;
extern i32 mp_window_drag_wnd;
extern char str_enter_window_drag[20];
extern char str_exit_window_drag[32];
extern int diplo_border_intruded_cities;
extern char str_pickgovt[12];
extern char str_newgovt[8];
extern char str_democrats[12];
extern char str_changed_2[8];
extern char str_overthrown_2[12];
extern char str_joinwar[8];
extern char str_parleywaiting_2[16];
extern char str_parleybusy_3[12];
extern char str_parleycancel_5[16];
extern char str_parleygoaway_2[16];
extern char str_parleyok_2[12];
extern char str_parleybusy_4[12];
extern char str_parleycancel_6[16];
extern char str_parleybusy_5[12];
extern char str_unforce[8];
extern char str_wallforce[12];
extern char str_unforce_2[8];
extern char str_wallforce_2[12];
extern char str_allymakespeace[16];
extern char str_allymakespeace_2[16];
extern char str_signpeace[12];
extern char str_signnato[12];
extern char str_signallied[12];
extern char str_cancelallied_2[16];
extern char str_allymakeswar[16];
extern char str_allymakeswar_2[16];
extern char str_cancelpeace_3[12];
extern char str_declarewar[12];
extern char str_militaryaid1_2[16];
extern char str_militaryaid2_2[16];
extern char str_warends_2[8];
extern char str_terms[8];
extern char str_intruders[12];
extern char str_intruder[12];
extern char str_violators[12];
extern char str_violator[12];
extern char str_withdrawn[12];
extern char str_1_6[4];
extern char str_senatescandal_2[16];
extern char str_violate[8];
extern char str_admirecity[12];
extern char str_nearcity[12];
extern char str_helpme[8];
extern char str_instead[8];
extern char str_2_4[4];
extern char str_crusade[8];
extern char str_jihad[116];
extern char str_cdromnotfound2[16];
extern char str_empty_173[8];
extern char str_pedia_3[8];
extern char str_describe_4[12];
extern char str_pedia_4[8];
extern char str_pediacivfacts[28];
extern i32 frame_texture_loaded_i32;
extern char str_civ2_civ2art_dll_4[20];
extern char str_notice_15[8];
extern char str_failed_to_load_civ2art_gif_2[40];
extern uint status_info_font_scale;
extern int status_font_scale;
extern char status_end_turn_visible;
extern u8 status_end_turn_visible_rest[3];
extern i8 status_wnd_hidden;
extern u8 status_wnd_hidden_rest[3];
extern int status_timer_last_width;
extern int status_timer_last_color;
extern int status_timer_last_percent;
extern char str_x_4[4];
extern char str_x_5[4];
extern i8 status_tax_sep_str[4];
extern i8 status_lux_sep_str[4];
extern char str_x_6[4];
extern i8 status_moves_frac_sep_str[4];
extern char str_empty_174[4];
extern char str_empty_175[16];
extern int map_label_font_size;
extern int anim_combat_defender_x;
extern int anim_combat_defender_y;
extern int anim_combat_defender;
extern int anim_origin_x;
extern int anim_origin_y;
extern iptr anim_origin_stack_unit;
extern i8 city_size_digit_str[4];
extern i8 city_size_digit2_str[64];
extern char str_d_ss_franklinton_startup_playerbox_cpp[40];
extern char str_pplayerpopup_null[24];
extern i8 lobby_ip_sep_str[4];
extern char str_igz_sending_dplsys_connectionsettingsrea[56];
extern char str_igz_xd_lobbysendmessage_returned_error_d_2[48];
extern char str_igz_sending_dplsys_dplayconnectsucceeded[52];
extern char str_igz_xd_lobbysendmessage_returned_error_d_3[48];
extern char str_waitingonjoin[84];
extern int sprite_editor_right_color;
extern int sprite_editor_left_color;
extern int sprite_editor_prev_left_color;
extern int sprite_editor_prev_right_color;
extern char str_units_bmp_2[12];
extern char str_units_gif_3[12];
extern char str_units_gif_4[12];
extern char str_units_bmp_3[12];
extern char str_units_bmp_4[12];
extern char str_units_bmp_5[12];
extern char str_cities_bmp_2[12];
extern char str_cities_gif_3[12];
extern char str_cities_gif_4[12];
extern char str_cities_bmp_3[12];
extern char str_cities_bmp_4[12];
extern char str_cities_bmp_5[12];
extern char str_terrain1_bmp_2[16];
extern char str_terrain1_gif_3[16];
extern char str_terrain1_gif_4[16];
extern char str_terrain1_bmp_3[16];
extern char str_terrain1_bmp_4[16];
extern char str_terrain1_bmp_5[16];
extern char str_terrain2_bmp_2[16];
extern char str_terrain2_gif_3[16];
extern char str_terrain2_gif_4[16];
extern char str_terrain2_bmp_3[16];
extern char str_terrain2_bmp_4[16];
extern char str_terrain2_bmp_5[16];
extern char str_icons_bmp_2[12];
extern char str_icons_gif_3[12];
extern char str_icons_gif_4[12];
extern char str_icons_bmp_3[12];
extern char str_icons_bmp_4[12];
extern char str_icons_bmp_5[12];
extern char str_icons_bmp_6[12];
extern char str_icons_gif_5[12];
extern char str_icons_gif_6[12];
extern char str_icons_bmp_7[12];
extern char str_icons_bmp_8[12];
extern char str_icons_bmp_9[12];
extern i8 sprite_editor_default_title[4];
extern i8 sprite_picker_default_title[4];
extern char str_rusure[8];
extern char str_units_2[8];
extern char str_terrain_5[8];
extern char str_icons[8];
extern char str_cities_2[8];
extern char str_s_s_c_s_bmp_s_gif_c_c[28];
extern char str_s_bmp_s_gif[16];
extern char str_s_s_2[8];
extern char str_bmp_gif[8];
extern char str_s_s_3[8];
extern char str_oddterrain[12];
extern char str_eventerrain[12];
extern i8 sprite_import_bmp_ext_str[4];
extern i8 sprite_import_gif_ext_str[132];
extern ModalMenuWindow *menu_owner_window;
extern GameMenu *menu_active_object;
extern MenuCommandFn menu_command_callback;
extern i8 menu_title_sep_str[8];
extern char str_annoyallied[12];
extern char str_annoypeace[12];
extern char str_annoyvassal[12];
extern char str_annoycease[12];
extern char str_annoy[8];
extern char str_overrule[12];
extern char str_allowun[8];
extern char str_allowaggressor[16];
extern char str_allowhawks[12];
extern char str_takeciv_2[8];
extern char str_tookciv_2[8];
extern char str_schism_3[8];
extern char str_canescape_2[12];
extern char str_escape_2[8];
extern char str_abandonwonder_3[16];
extern char str_stillwonder[12];
extern char str_capturewonder_2[16];
extern char str_lostwonder_2[12];
extern char str_citycapture_2[12];
extern char str_citywinally_2[12];
extern char str_cityloseally_2[16];
extern char str_citycapture2_2[16];
extern char str_partisans_2[12];
extern char str_promoted_2[12];
extern char str_useweapons_2[12];
extern char str_sdi_2[4];
extern char str_sdi_3[4];
extern char str_breakcease_3[12];
extern char str_sneak_2[8];
extern char str_missileattack_2[16];
extern char str_pearlharbor_2[12];
extern char str_battery2_2[12];
extern char str_battery_2[8];
extern char str_scramble_2[12];
extern char str_amphibmotize_2[16];
extern char str_multiplelose_2[16];
extern char str_multiplewin_2[12];
extern char str_ransom_2[8];
extern char str_cancelpeace_4[12];
extern char str_allyunderattack_2[16];
extern char str_allyattacking_2[16];
extern char str_cancelpeace_5[12];
extern char str_allyunderattack_3[16];
extern char str_allyattacking_3[124];
extern i8 cosmic_edit_min_values[88];
extern i8 cosmic_edit_max_values[88];
extern char str_editcosmic[12];
extern char str_d[8];
extern char str_d_2[8];
extern i8 cosmic_value_fmt_3digit[4];
extern char str_d_3[16];
extern char str_d_4[12];
extern char str_d_5[12];
extern char str_cpedit[8];
extern char str_8d_s[8];
extern char str_cosmic_2[8];
extern char str_error_updating_rules_s_5[24];
extern char str_file_i_o_error_6[16];
extern char str_effects[8];
extern char str_help_18[72];
extern int main_frame_created;
extern u8 main_frame_created_rest[4];
extern char str_civ_ini_16[8];
extern char str_window_name[12];
extern char str_civilization_gold_16[20];
extern char str_civ2_civ2_exe_2[32];
extern int fatal_cleanup_count;
extern u8 fatal_cleanup_count_rest[4];
extern i8 error_context_text[80];
extern i8 error_last_open_path[80];
extern int error_last_open_failed;
extern uint error_last_alloc_failed;
extern i32 mem_last_alloc_size;
extern char str_rt_8[4];
extern i8 err_comment_prefix_str[4];
extern i8 err_dump_line_fmt[4];
extern char str_errors_db[12];
extern char str_modules_db[12];
extern char str_rt_9[4];
extern char str_error_4[8];
extern char str_in_module[16];
extern char str_data[12];
extern i8 err_data_sep_str[4];
extern i8 err_fatal_msg_fmt[4];
extern char str_s_2[8];
extern char str_tried_to_allocate[20];
extern char str_bytes[8];
extern i8 fatal_alloc_line_fmt[4];
extern i8 fatal_alloc_newline_str[4];
extern char str_file_open_failed_s[24];
extern char str_most_recent_dos_error[28];
extern i8 fatal_dos_error_line_fmt[4];
extern i8 fatal_dos_error_newline_str[4];
extern char str_warn0_dat[88];
extern i32 unit_sound_button_label_ids[6];
extern i8 unit_sound_editor_empty_title[4];
extern i8 file_copy_read_mode[4];
extern i8 file_copy_write_mode[4];
extern char str_error_copying_file[20];
extern char str_s_wav_c_wav_c_c[24];
extern char str_wav[8];
extern i8 unit_sound_wav_default_ext[4];
extern char str_playsound[12];
extern char str_debug_10[8];
extern char str_sound_5[8];
extern i8 unit_sound_path_sep_str[4];
extern i8 unit_sound_wav_suffix_str[8];
extern char str_wav_2[32];
extern char str_onlysettlers[16];
extern char str_cityatsea[12];
extern char str_only10[8];
extern char str_adjacentcity[16];
extern char str_namecity[12];
extern char str_founded[8];
extern char str_firstproduct[16];
extern char str_disband[8];
extern char str_disband_2[8];
extern char str_onlysettlers_2[16];
extern char str_radio[8];
extern char str_cantimprove[12];
extern char str_alreadyair[12];
extern char str_construction[16];
extern char str_cantimprove_2[12];
extern char str_alreadyfort[12];
extern char str_cantimprove_3[12];
extern char str_bridgebuilding[16];
extern char str_alreadyroad[12];
extern char str_railroads_2[12];
extern char str_cantimprove_4[12];
extern char str_cantimprove_5[12];
extern char str_alreadymining[16];
extern char str_nowater[8];
extern char str_alreadyfarmland[16];
extern char str_farmland_2[12];
extern char str_cantimprove_6[12];
extern char str_engineers[12];
extern char str_nopollution[12];
extern char str_caravanhome[12];
extern char str_cantdo[8];
extern char str_cantdo_2[8];
extern char str_cantdo_3[8];
extern char str_cantdo_4[8];
extern char str_cantdo_5[8];
extern char str_pillagewhat[12];
extern char str_pillagemenu[12];
extern char str_paradroprules1[16];
extern char str_paradroprules2[16];
extern char str_goto[8];
extern char str_liftship[12];
extern char str_liftplane[12];
extern char str_noairport[12];
extern char str_noairport_2[12];
extern char str_alreadyairlift[16];
extern char str_airliftselect[16];
extern char str_noairport2[12];
extern char str_alreadyairlift_2[16];
extern char str_enemyfighters[132];
extern char unit_move_unanimated_state;
extern u8 unit_move_unanimated_state_rest[3];
extern int unit_move_in_progress;
extern char str_surprisetribe[16];
extern char str_surprisenomads[16];
extern char str_surprisescrolls[16];
extern char str_surprisemetals[16];
extern char str_surprisenothing[16];
extern char str_surprisebarb[16];
extern char str_surprisemercs[16];
extern char str_caravanmenu[12];
extern char str_addtowonder[12];
extern char str_notonmap[12];
extern char str_zoc[4];
extern char str_alliedrepair[16];
extern char str_amphib[8];
extern char str_noncombat[12];
extern char str_fighter[8];
extern char str_expel[8];
extern char str_upmine_2[8];
extern char str_upyours_2[8];
extern char str_upmine_3[8];
extern char str_upyourstoo_2[12];
extern char str_upyourstoo_3[12];
extern char str_upyourstoo_4[12];
extern char str_upyours_3[8];
extern char str_upyours_4[8];
extern char str_upyours_5[8];
extern char str_noexpel[8];
extern char str_fatigue[8];
extern char str_nolandfall[12];
extern char str_landfall[12];
extern char str_occupy[8];
extern char str_landing[8];
extern char str_move_unit_nm_realtime_stacker_connection[64];
extern char str_serverconnecttime_7[20];
extern char str_fuel[8];
extern char str_trireme[8];
extern char str_longmove[12];
extern char str_lockmap_connection_to_server_timed_out[44];
extern char str_serverconnecttime_8[20];
extern char str_unlockmap_connection_to_server_timed_out[44];
extern char str_serverconnecttime_9[136];
#pragma pack(push, 1)
struct spaceship_part_table_block {
    union {
        struct {
            i8 spaceship_part_table[4];  /* 00634f60 */
            int spaceship_part_max_counts[1];  /* 00634f64 */
            i8 spaceship_part_masses[8];  /* 00634f68 */
            int spaceship_propulsion_max;  /* 00634f70 */
            u8 spaceship_propulsion_max_rest[8];
            int spaceship_fuel_max;  /* 00634f7c */
            u8 spaceship_fuel_max_rest[8];
            int spaceship_habitation_max;  /* 00634f88 */
            u8 spaceship_habitation_max_rest[8];
            int spaceship_life_support_max;  /* 00634f94 */
            u8 spaceship_life_support_max_rest[8];
            int spaceship_solar_panel_max;  /* 00634fa0 */
            u8 spaceship_solar_panel_max_rest[4];
        };
        SpaceshipPartKind spaceship_parts[6];  /* 00634f60 */
    };
};
#pragma pack(pop)
extern struct spaceship_part_table_block spaceship_part_table_block;
extern char str_launched_2[12];
extern char str_spaceship[12];
extern char str_empty_176[4];
extern char str_0_000_7[8];
extern char str_empty_177[4];
extern char str_00_2[4];
extern char str_empty_178[4];
extern char str_empty_179[8];
extern char str_empty_180[8];
extern char str_empty_181[8];
extern char str_empty_182[4];
extern char str_empty_183[8];
extern char str_launch[8];
extern char str_component[12];
extern char str_module[8];
extern char str_nofurther[12];
extern char str_spaceships[12];
extern char str_nospaceships[60];
extern i8 pedia_improve_cost_fmt[4];
extern i8 pedia_improve_upkeep_fmt[4];
extern char str_pediaimprove_2[16];
extern char str_pedia_5[8];
extern i8 pedia_improve_text_space_str[8];
extern int g_nr_seed;
extern int net_last_recv_msg_type;
extern char str_d_ss_franklinton_netmgr_cpp[32];
extern char str_bconnectedtonet[20];
extern char str_civ_ini_17[8];
extern char str_internet_timeout[20];
extern char str_civilization_gold_17[20];
extern char str_civ_ini_18[8];
extern i8 net_internet_timeout_default[4];
extern char str_internet_timeout_2[20];
extern char str_civilization_gold_18[20];
extern char str_civ_ini_19[8];
extern char str_tcpip_timeout[16];
extern char str_civilization_gold_19[20];
extern char str_civ_ini_20[8];
extern i8 net_tcpip_timeout_default[4];
extern char str_tcpip_timeout_2[16];
extern char str_civilization_gold_20[20];
extern char str_civ_ini_21[8];
extern char str_ipxspx_timeout[16];
extern char str_civilization_gold_21[20];
extern char str_civ_ini_22[8];
extern i8 net_ipxspx_timeout_default[4];
extern char str_ipxspx_timeout_2[16];
extern char str_civilization_gold_22[20];
extern char str_civ_ini_23[8];
extern char str_modem_timeout[16];
extern char str_civilization_gold_23[20];
extern char str_civ_ini_24[8];
extern i8 net_modem_timeout_default[4];
extern char str_modem_timeout_2[16];
extern char str_civilization_gold_24[20];
extern char str_civ_ini_25[8];
extern char str_direct_timeout[16];
extern char str_civilization_gold_25[20];
extern char str_civ_ini_26[8];
extern i8 net_direct_timeout_default[4];
extern char str_direct_timeout_2[16];
extern char str_civilization_gold_26[20];
extern char str_d_ss_franklinton_netmgr_cpp_2[32];
extern char str_0_6[4];
extern char str_connecttonet_xd_initialize_failed_return[56];
extern char str_connecttonet_xd_activateserver_failed_re[56];
extern char str_disconnecting_from_network[28];
extern char str_send_buffer_has_flushed[28];
extern char str_xd_closeconnection[28];
extern char str_xd_shutdownsockets[28];
extern char str_xd_shutdownsockets_2[28];
extern char str_xd_shutdownmodem[24];
extern char str_xd_shutdownten[140];
extern char str_maximum_gnetmgr_poll_call_depth_d[44];
extern char str_maximum_alpha_message_queue_load_d[40];
extern char str_maximum_message_queue_load_d[36];
extern char str_maximum_draw_stack_load_d[32];
extern char str_xd_resetlibrary[24];
extern char str_disconnection_delay_2_seconds[36];
extern char str_xd_resetlibrary_2[24];
extern char str_d_ss_franklinton_netmgr_cpp_3[32];
extern char str_pcur_null[16];
extern char str_broadcastreceivefunc_killed_message_tiny[72];
extern char str_broadcastreceivefunc_killed_message_not[60];
extern char str_securereceivefunc_killed_message_not_civ[56];
extern char str_last_callback_size_s[24];
extern char str_last_legal_sequence_number_s[32];
extern char str_last_legal_type_s[20];
extern char str_last_legal_netmsg_size_s[28];
extern char str_sequence_number_s[20];
extern char str_message_size_s[20];
extern i8 net_hexdump_space_str[4];
extern char str_s_3[4];
extern char str_empty_184[4];
extern char str_potential_offset_message_found[32];
extern char str_offset_s_bytes[32];
extern char str_type_s[28];
extern char str_size_s_bytes[32];
extern char str_sequence_number_s_2[28];
extern char str_newclientreceivefunc_new_client_connecti[52];
extern char str_connectedtoserverfunc_new_server_connect[56];
extern char str_lostconnectionfunc_connection_to_d_has_s[52];
extern char str_oversized_xdaemon_message_ul[428];
extern iptr dlg_palette;
extern iptr dlg_parent_window;
extern i32 dlg_saved_parent_window;
extern i32 dlg_default_x;
extern i32 dlg_default_y;
extern char str_game_7[8];
extern char str_debug_11[8];
extern i8 *dialog_default_text_font;
extern u8 dialog_default_text_font_rest[12];
extern i8 dlg_frame_color;
extern u8 dlg_frame_color_rest[3];
extern i8 dlg_text_shadow_color;
extern u8 dlg_text_shadow_color_rest[3];
extern i8 dlg_text_color;
extern u8 dlg_text_color_rest[3];
extern byte dlg_highlight_text_color;
extern u8 dlg_highlight_text_color_rest[3];
extern byte dlg_bg_color;
extern u8 dlg_bg_color_rest[3];
extern byte dlg_bevel_light_color;
extern u8 dlg_bevel_light_color_rest[3];
extern byte dlg_bevel_dark_color;
extern u8 dlg_bevel_dark_color_rest[3];
extern i8 dlg_title_text_color;
extern u8 dlg_title_text_color_rest[3];
extern i8 dlg_title_shadow_color;
extern u8 dlg_title_shadow_color_rest[3];
extern i32 list_bg_color;
extern i32 list_text_color;
extern int list_shadow_color;
extern i32 list_sel_bg_color;
extern int dlg_sel_text_color;
extern int dlg_sel_text_shadow_color;
extern i32 dialog_default_list_color;
extern i32 dlg_scale_num;
extern i32 dlg_scale_den;
extern WndNotifyFn dlg_pending_idle_callback;  /* the next dialog's modal idle callback */
extern WndNotifyFn dlg_default_idle_callback;  /* every dialog's, when no pending one */
extern WndMouseFn dlg_pending_move_handler;  /* the next dialog's move handler */
extern i8 *dialog_button_labels[4];
extern i32 dialog_stack[17];
extern iptr dialog_stack_depth;
extern int dlg_frame_tile_surface;
extern int dlg_client_tile_surface;
extern char str_ok_2[12];
extern char str_cancel_2[8];
extern char str_popups[8];
extern char str_labels_6[8];
extern char str_d_ss_franklinton_popup_1_cpp[32];
extern char str_popupstackindex_0_popupstackindex_17[44];
extern char str_w[4];
extern i8 dialog_wrap_space_str[4];
extern char str_d_ss_franklinton_popup_1_cpp_2[32];
extern char str_popupstackindex_0_popupstackindex_16[48];
extern char str_empty_185[4];
extern char str_options[8];
extern char str_prompt[8];
extern i8 dlg_key_text_str[8];
extern char str_title[8];
extern char str_button[8];
extern char str_columns[8];
extern char str_height[8];
extern char str_smallfont[12];
extern i8 dlg_key_y_prefix[4];
extern i8 dlg_key_x_prefix[4];
extern char str_width[8];
extern char str_length[8];
extern char str_checkbox[12];
extern char str_listbox[8];
extern char str_system[8];
extern char str_default_3[120];
extern iptr gfx_draw_target;
extern char str_server_s_d_3[16];
extern char str_client_s_d_3[16];
extern char str_waitproduction[16];
extern char str_casualty_4[12];
extern char str_casualties_4[12];
extern char str_ourturntomove_3[16];
extern char str_waithumanmoves[16];
extern char str_server_s_d_4[16];
extern char str_client_s_d_4[16];
extern char str_clienthotwait_2[16];
extern char str_waithumanmoves_2[16];
extern char str_waitproduction_2[16];
extern char str_waitproduction_3[16];
extern char str_waitaimoves[12];
extern char str_casualty_5[12];
extern char str_casualties_5[12];
extern char str_ourturntomove_4[64];
extern i8 pedia_unit_cost_fmt[4];
extern i8 pedia_unit_attack_fmt[4];
extern i8 pedia_unit_defense_fmt[4];
extern i8 pedia_unit_hp_fmt[4];
extern i8 pedia_unit_firepower_fmt[4];
extern i8 pedia_unit_move_fmt[4];
extern char str_pedia_6[8];
extern char str_pediaunitfacts[24];
extern char str_pbem1[8];
extern char str_emailnot[36];
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
extern char str_notice_16[8];
extern char str_error_updating_events_s_2[28];
extern char str_file_i_o_error_7[16];
extern char str_units_3[8];
extern char str_error_updating_rules_s_6[24];
extern char str_file_i_o_error_8[16];
extern char str_unitname[12];
extern char str_units_4[8];
extern char str_help_19[8];
extern char str_s_s_4[8];
extern char str_abilities[12];
extern char str_notice_17[8];
extern u8 str_empty_186[4];
extern char str_editorsa_gif_2[116];
extern i32 unit_move_anim_active;
extern i32 dlg_unit_sprite_zoom;
extern char str_d_ss_franklinton_unit_cpp[28];
extern char str_id_0_id_max_units_2[32];
extern char str_infinite_unit_stack_id_d_repairing[44];
extern char str_dead_unit_in_unit_stack_id_d_repairing[48];
extern char str_dead_unit_in_unit_stack_id_d_repairing_2[48];
extern char str_crossed_locations_in_unit_stack_id_d_rep[56];
extern char str_crossed_locations_in_unit_stack_id_d_rep_2[56];
extern char str_pick_up_unit_connection_to_server_timed_2[48];
extern char str_serverconnecttime_10[20];
extern char str_put_down_unit_connection_to_server_timed[48];
extern char str_serverconnecttime_11[20];
extern char str_relocate_unit_connection_to_server_timed[48];
extern char str_serverconnecttime_12[20];
extern char str_move_to_bottom_connection_to_server_time[48];
extern char str_serverconnecttime_13[20];
extern char str_create_unit_connection_to_server_timed_o[48];
extern char str_serverconnecttime_14[20];
extern char str_toomanyunits[16];
extern char str_ships[8];
extern char str_airunit[8];
extern char str_caravan_7[8];
extern char str_firstunit1[12];
extern char str_firstunit2[12];
extern char str_delete_unit_connection_to_server_timed_o[48];
extern char str_serverconnecttime_15[20];
extern char str_d_ss_franklinton_unit_cpp_2[28];
extern char str_ship_1_ship_max_units_2[36];
extern char str_stack_ship_connection_to_server_timed_ou[44];
extern char str_serverconnecttime_16[20];
extern char str_stack_unit_connection_to_server_timed_ou[44];
extern char str_serverconnecttime_17[20];
extern char str_delete_safely_connection_to_server_timed[48];
extern char str_serverconnecttime_18[20];
extern char str_delete_visible_connection_to_server_time[48];
extern char str_serverconnecttime_19[244];
extern Tile *map_tiles;
extern u8 map_tiles_rest[4];
extern void *map_known_handles[8];
extern u8 *map_known[8];
extern u8 *map_coarse[4];
extern u8 map_allocated;
extern u8 map_allocated_rest[3];
extern int net_batch_used_dwords;
extern i32 net_batch_record_sizes[8];
extern char str_d_ss_franklinton_map_cpp[28];
extern char str_gnetmgr_bserver_3[20];
extern char str_d_ss_franklinton_map_cpp_2[28];
extern char str_gnetmgr_bserver_4[60];
extern i8 path_separator_str[8];
extern iptr text_draw_surface;
extern GameFont *text_draw_font;
extern i32 text_fg_color;
extern int text_shadow_color;
extern int text_shadow_dx;
extern int text_shadow_dy;
extern int text_fake_bold;
extern u8 text_fake_bold_rest[8];
extern int ms_window_stack_count;
extern char str_mswindowclass_2[16];
extern char str_error_ms_window_stack_inaccurate[36];
extern char str_window_not_a_child_in_mssettopmostchild[40];
extern char str_window_not_sws_attached_in_mssettopmostc[48];
extern char str_d_ss_smeds32_port_cpp[24];
extern char str_err_portallocfailed[1228];
extern char str_error_gif_resource_not_found[36];
extern char str_d_ss_smeds32_port_cpp_2[24];
extern char str_err_resourcenotfound[24];
extern i8 gif_sig_file[4];
extern char str_error_resource_is_not_a_gif[32];
extern char str_d_ss_smeds32_port_cpp_3[24];
extern char str_err_badpicformat[20];
extern char str_error_gif_contains_no_global_color_map[44];
extern char str_d_ss_smeds32_port_cpp_4[24];
extern char str_err_badpicformat_2[20];
extern char str_error_gif_image_block_not_found[36];
extern char str_d_ss_smeds32_port_cpp_5[24];
extern char str_err_badpicformat_3[20];
extern char str_warning_skipping_local_color_table[36];
extern char str_error_gif_resource_not_found_2[36];
extern char str_d_ss_smeds32_port_cpp_6[24];
extern char str_err_resourcenotfound_2[24];
extern i8 gif_sig_resource[4];
extern char str_error_resource_is_not_a_gif_2[32];
extern char str_d_ss_smeds32_port_cpp_7[24];
extern char str_err_badpicformat_4[20];
extern char str_error_gif_contains_no_global_color_map_2[44];
extern char str_d_ss_smeds32_port_cpp_8[24];
extern char str_err_badpicformat_5[20];
extern char str_error_gif_image_block_not_found_2[36];
extern char str_d_ss_smeds32_port_cpp_9[24];
extern char str_err_badpicformat_6[20];
extern char str_warning_skipping_local_color_table_2[456];
extern char str_warning_port_not_locked_in_fillcolor[40];
extern char str_warning_port_not_locked_in_changecolor[40];
extern char str_warning_source_port_not_locked_in_copyto[48];
extern char str_warning_destination_port_not_locked_in_c[100];
extern char str_error_null_port_in_copytoscreen[36];
extern char str_warning_port_not_locked_in_copytoscreen[132];
extern char str_warning_getpixel_out_of_clip_region[40];
extern char str_warning_setpixel_out_of_clip_region[40];
extern char str_error_no_current_font_selected[96];
extern char str_d_ss_smeds32_port_cpp_10[24];
extern char str_err_portallocfailed_2[20];
extern char str_d_ss_smeds32_port_cpp_11[24];
extern char str_err_portallocfailed_3[20];
extern char str_d_ss_smeds32_port_cpp_12[24];
extern char str_err_portallocfailed_4[2172];
extern char str_error_color_resource_not_found[104];
extern char str_color_scale_factor_out_of_range[32];
extern char str_times_new_roman[16];
extern char str_arial[8];
extern char str_courier[8];
extern char str_system_2[8];
extern int g_btn_hover;
extern int g_btn_pressed;
extern i8 hotspot_wnd_title[4];
extern char str_mscontrolclass[16];
extern char str_error_tried_to_dispose_null_region_in_ms[60];
extern char str_error_tried_to_initialize_null_control_i[60];
extern char str_error_tried_to_dispose_null_control_in_d[60];
extern i8 button_wnd_title[4];
extern char str_mscontrolclass_2[16];
extern char str_could_not_create_bitmap_in_msinitbutton[40];
extern char str_could_not_create_bitmap_in_msinitbutton_2[40];
extern char str_could_not_create_bitmap_in_msinitbutton_3[116];
extern i8 spritebtn_wnd_title[4];
extern char str_mscontrolclass_3[16];
extern DibSurface *draw_current_surface;
extern i8 *draw_current_font;
extern u8 draw_current_font_rest[24];
extern byte gfx_text_color_index;
extern u8 gfx_text_color_index_rest[3];
extern byte radio_text_color_index;
extern u8 radio_text_color_index_rest[3];
extern byte radio_shadow_color_index;
extern u8 radio_shadow_color_index_rest[3];
extern byte radio_shadow_dx;
extern u8 radio_shadow_dx_rest[3];
extern byte radio_shadow_dy;
extern u8 radio_shadow_dy_rest[3];
extern byte radio_disabled_color_index;
extern u8 radio_disabled_color_index_rest[3];
extern byte ctrl_text_color_index;
extern u8 ctrl_text_color_index_rest[3];
extern byte ctrl_shadow_color_index;
extern u8 ctrl_shadow_color_index_rest[3];
extern byte ctrl_shadow_dx;
extern u8 ctrl_shadow_dx_rest[3];
extern byte ctrl_shadow_dy;
extern u8 ctrl_shadow_dy_rest[3];
extern byte ctrl_disabled_color_index;
extern u8 ctrl_disabled_color_index_rest[3];
extern i32 wnd_active_child;
extern iptr wnd_modal_depth;
#pragma pack(push, 1)
struct wnd_modal_stack_below_block {
    i32 wnd_modal_stack_below[1];  /* 00637eac */
    i32 wnd_modal_stack[17];  /* 00637eb0 */
    TimerManager *game_timer_mgr;  /* 00637ef4 */
    int timer_alt_kind_request;  /* 00637ef8 */
    int avi_idle_update_enabled;  /* 00637efc */
    i8 ctrl_text_rgb_r;  /* 00637f00 */
    i8 ctrl_text_rgb_g;  /* 00637f01 */
    i8 ctrl_text_rgb_b;  /* 00637f02 */
    char ctrl_text_rgb_set;  /* 00637f03 */
    i8 ctrl_disabled_rgb_r;  /* 00637f04 */
    i8 ctrl_disabled_rgb_g;  /* 00637f05 */
    i8 ctrl_disabled_rgb_b;  /* 00637f06 */
    char ctrl_disabled_rgb_set;  /* 00637f07 */
    int checkbox_mouse_pressed;  /* 00637f08 */
    i8 checkbox_wnd_title[4];  /* 00637f0c */
};
#pragma pack(pop)
extern struct wnd_modal_stack_below_block wnd_modal_stack_below_block;
extern char str_mscontrolclass_4[16];
extern char str_error_tried_to_dispose_null_checkbox_in[56];
extern int scrollbar_class_registered;
extern i8 scrollbar_probe_wnd_title[4];
extern char str_scrollbar[12];
extern char str_msscrollbarclass[20];
extern i8 scrollbar_wnd_title[4];
extern char str_msscrollbarclass_2[20];
extern int gfx_scale_num;
extern int gfx_scale_den;
extern int gfx_scale_x_num;
extern int gfx_scale_x_den;
extern int gfx_scale_y_num;
extern int gfx_scale_y_den;
extern u8 gfx_scale_y_den_rest[72];
extern char str_warning_sprite_not_locked_in_changecolor[88];
extern char str_warning_port_not_locked_on_sprite_extrac[44];
extern char str_error_sprite_extract_illegal_pixel_mappi[52];
extern char str_error_unable_to_allocate_scan_line_table[60];
extern char str_error_couldn_t_allocate_memory_for_sprit[224];
extern char str_error_sprite_size_exceeds_scale_table_si[176];
extern char str_error_sprite_size_exceeds_scale_table_si_2[88];
extern char str_error_mrtimer_not_initialized_in_resetti[48];
extern char dbg_log_to_file;
extern u8 dbg_log_to_file_rest[3];
extern char dbg_log_to_debugger;
extern u8 dbg_log_to_debugger_rest[3];
extern char str_5d[8];
extern int editbox_class_registered;
extern i8 editbox_probe_wnd_title[4];
extern i8 editbox_native_class_name[8];
extern char str_mseditboxclass[16];
extern i8 editbox_wnd_title[4];
extern char str_mseditboxclass_2[16];
extern int combo_class_registered;
extern i8 combo_probe_wnd_title[4];
extern char str_combobox[12];
extern char str_mscomboboxclass[16];
extern i8 combo_wnd_title[4];
extern char str_mscomboboxclass_2[20];
extern int listbox_class_registered;
extern i8 listbox_probe_wnd_title[4];
extern char str_listbox_2[8];
extern char str_mslistboxclass[16];
extern i8 listbox_wnd_title[4];
extern char str_mslistboxclass_2[16];
extern i8 listbox_no_sel_text[4];
extern int timer_mm_available;
extern char str_timerdll_dll[16];
extern char str_timercallback[16];
extern char str_settimerid[12];
extern char str_gettimerid[12];
extern char str_gettimerindex[16];
extern char str_resettimernotified[20];
extern char str_d_ss_smeds32_pctimer_cpp[28];
extern char str_err_dynamiclinkfailed[24];
extern char str_mrtimer[8];
extern char str_msmrtimerclass[16];
extern char str_d_ss_smeds32_pctimer_cpp_2[28];
extern char str_err_cantcreatewindow[24];
extern char str_mrtimer_2[8];
extern char str_msmrtimerclass_2[16];
extern char str_d_ss_smeds32_pctimer_cpp_3[28];
extern char str_err_timersetfailed[20];
extern char str_d_ss_smeds32_pctimer_cpp_4[28];
extern char str_err_timersetfailed_2[20];
extern char str_d_ss_smeds32_pctimer_cpp_5[28];
extern char str_err_timersetfailed_3[20];
extern char str_d_ss_smeds32_pctimer_cpp_6[28];
extern char str_err_timersetfailed_4[24];
extern int snd_wave_out_handle;
extern i32 snd_wave_notify_hwnd;
extern i32 snd_wave_open_buffer_bytes;
extern uint snd_wave_max_buffers;
extern uint snd_wave_buffer_count;
extern short snd_wave_source_count;
extern u8 snd_wave_source_count_rest[2];
extern i32 snd_avi_feed_count_i32;
extern i32 snd_avi_fill_count_i32;
extern i32 snd_avi_feed_samples_i32;
extern u8 snd_avi_feed_samples_i32_rest[4];
extern i32 snd_wave_format[1];
extern i32 snd_wave_samples_per_sec;
extern i32 snd_wave_avg_bytes_per_sec;
extern i8 snd_wave_block_align;
extern u8 snd_wave_block_align_rest[3];
extern int snd_wave_device_open;
extern i32 snd_wave_buffers_ready_i32;
extern i16 snd_wave_stop_pending_i16;
extern u8 snd_wave_stop_pending_i16_rest[2];
extern i32 snd_avi_stream_started;
extern WaveBuffer *snd_wave_buffer_ring;
extern WaveBuffer *snd_wave_spare_buffers;
extern WaveBuffer *snd_wave_cur_buffer;
extern LONG snd_wave_total_buffer_bytes;
extern WaveSource *snd_wave_sources;
extern i32 snd_stop_drained_msg_count_i32;
extern char str_sound_error[12];
extern char str_this_sound_format_is_not_supported[36];
extern char str_sound_error_2[12];
extern char str_undefined_sound_error[24];
extern char str_wave_out_error[16];
extern i8 file_path_backslash_str[4];
extern char str_error_tried_to_write_to_a_read_only_file[92];
extern char str_bad_file_handle_in_msfilesize[88];
extern i8 radio_text_rgb_r;
extern i8 radio_text_rgb_g;
extern i8 radio_text_rgb_b;
extern char radio_text_rgb_set;
extern i8 radio_disabled_rgb_r;
extern i8 radio_disabled_rgb_g;
extern i8 radio_disabled_rgb_b;
extern char radio_disabled_rgb_set;
extern int radio_lbutton_captured;
extern int radio_rbutton_captured;
extern i8 radiogrp_item_wnd_title[4];
extern char str_mscontrolclass_5[36];
extern char str_error_tried_to_dismantle_null_radioset_i[60];
extern char str_mscontrolclass_6[16];
extern char str_fatal_error[20];
extern i8 err_format_empty_str[4];
extern char str_error_s_file_s_line_d[36];
extern char str_smeds_application_error[24];
extern uint res_dll_module_count;
extern char str_error_resource_file[24];
extern char str_not_found[12];
extern char str_smeds_terminated_normally[28];
extern char str_mswindowclass_3[16];
extern char str_msmovieclass[16];
extern char str_mscontrolclass_7[16];
extern char str_msmrtimerclass_3[296];
extern char str_d_ss_smeds32_pcmem_cpp[24];
extern char str_err_memallocfailed[20];
extern char str_error_tried_to_dispose_of_null_handle_in[60];
extern i32 avi_current_player_i32;
extern uint snd_midi_device_id;
extern MCIDEVICEID snd_cd_device_id;
extern u8 snd_cd_device_id_rest[4];
extern i32 snd_cd_track_count;
extern char str_msmmwindow[12];
extern char str_msmmwindow_2[148];
extern char str_failed_to_play_requested_cd_track[36];
extern char str_failed_to_play_requested_cd_track_2[36];
extern char str_cdaudio[8];
extern char str_failed_to_open_cdaudio_device[32];
extern char str_cdaudio_could_not_set_time_format[36];
extern char str_cdaudio_2[40];
extern i8 gfx_black_color_index;
extern u8 gfx_black_color_index_rest[7];
extern int gfx_palettized_display;
extern u8 gfx_palettized_display_rest[4];
extern i8 logpal_static_system_colors[296];
extern char str_rlldecode_could_not_allocate_decompressi[52];
extern char str_rlldecode_could_not_allocate_decompressi_2[52];
extern char str_rlldecode_could_not_allocate_decompressi_3[52];
extern char str_rlldecode_could_not_allocate_decompressi_4[52];
extern char str_rllencode_could_not_allocate_compression[72];
extern char str_ok_3[4];
extern char str_cancel_3[8];
extern HMENU menu_next_item_id;
extern i8 menu_separator_tag[8];
extern LPVOID avi_frame_buffer;
extern i32 avi_frame_buffer_size;
extern i32 avi_decompressor;
extern int avi_frame_width;
extern int avi_frame_height;
extern i32 g_video_start_time;
extern i32 avi_file[1];
extern i32 avi_video_stream[1];
extern i32 avi_audio_stream[1];
extern int avi_file_loaded;
extern int avi_playing;
extern AviPlayer *avi_active_window;
#pragma pack(push, 1)
struct avi_blit_union_left_block {
    i32 avi_blit_union_left[1];  /* 00638dd8 */
    int avi_blit_union_top;  /* 00638ddc */
    int avi_blit_union_right;  /* 00638de0 */
    int avi_blit_union_bottom;  /* 00638de4 */
    i32 avi_frame_needs_blit;  /* 00638de8 */
    i32 g_video_frame_count;  /* 00638dec */
};
#pragma pack(pop)
extern struct avi_blit_union_left_block avi_blit_union_left_block;
extern char str_icdecompressex_returned_ld[28];
extern char str_get_frame[12];
extern int dib_row_order;
extern int dib_bottom_up_logged;
extern char str_bottom_up_orientation_recommended[36];
extern char str_error_unable_to_create_bitmap_in_msinitg[48];
extern char str_error_unable_to_create_dc_in_msinitgraf[44];
extern char str_error_null_font_in_msdrawstring[36];
extern char str_error_null_font_in_msdrawstring_2[36];
extern char str_error_null_font_in_msdrawtext[32];
extern char str_error_unable_to_create_bitmap_in_msinitg_2[48];
extern char str_error_unable_to_create_dc_in_msinitgraf_2[44];
extern char str_error_unable_to_create_bitmap_in_msinitg_3[48];
extern char str_error_unable_to_create_dc_in_msinitgraf_3[44];
extern char str_error_unable_to_create_bitmap_in_msinitg_4[48];
extern char str_error_unable_to_create_dc_in_msinitgraf_4[148];
extern char str_error_cannot_allocate_decompression_tabl[1084];
extern void **g_ddraw;
extern char str_msdirectwindow[16];
extern char str_error_directdrawinitialize_failed[36];
extern char str_direct_draw_doesn_t_supports_hardware_bl[44];
extern char str_direct_draw_doesn_t_supports_colorkeys[40];
extern char str_direct_draw_doesn_t_supports_color_fills[44];
extern char str_direct_draw_device_is_bank_switched[624];
extern i32 input_vkey_map_table[256];
extern char smeds_log_enabled;
extern u8 smeds_log_enabled_rest[3];
extern char str_smeds_log[12];
extern char str_smeds_log_2[56];
extern char str_file_output_to[20];
extern char str_smeds_log_3[12];
extern i8 dbg_log_header_crlf[4];
extern char str_smeds32_version[20];
extern char str_d_d_d[20];
extern i8 dbg_log_header_blank_lines[8];
extern char str_smeds_log_4[12];
extern char str_log_closed[60];
extern char str_smeds_log_5[12];
extern i8 dbg_log_file_crlf[4];
extern i8 dbg_log_debugger_crlf[8];
extern char str_stack_trace_not_yet_available_in_win32[120];
extern char str_potentially_fatal_error[24];
extern uint ddwin_last_activate_app;
extern u8 ddwin_last_activate_app_rest[80];
extern iptr ddctrl_current_target;
extern u8 ddctrl_current_target_rest[60];
extern char str_d_ss_smeds32_ddcntrl_cpp[28];
extern char str_getparentgamewin[44];
extern u32 rand_seed;
extern u8 rand_seed_rest[12];
extern i8 *str_ptr_no_error[45];
extern iptr crt_errno;
extern u8 crt_errno_rest[386];
extern char str_empty_187[926];
extern u8 crt_block_list[3720];
extern char str_pst[64];
extern char str_pdt[5376];
extern int minimap_tile_width;
extern int minimap_tile_height;
extern int minimap_view_rows;
extern int minimap_view_cols;
extern int minimap_origin_y;
extern int minimap_origin_x;
#pragma pack(push, 1)
struct minimap_window_block {
    DibSurface minimap_window;  /* 0063c818 */
    WndBase minimap_wnd;  /* 0063c860 */
    i8 minimap_wnd_handlers[204];  /* 0063c870 */
    int minimap_client_x;  /* 0063c93c */
    int minimap_client_y;  /* 0063c940 */
    int minimap_client_width;  /* 0063c944 */
    int minimap_client_height;  /* 0063c948 */
    u8 minimap_client_height_rest[420];
    i32 minimap_margin_y_i32;  /* 0063caf0 */
    int minimap_margin_x;  /* 0063caf4 */
    u8 minimap_margin_x_rest[156];
    int mapgen_max_y;  /* 0063cb94 */
    int mapgen_max_x;  /* 0063cb98 */
    i32 mapgen_min_x;  /* 0063cb9c */
    i32 mapgen_min_y;  /* 0063cba0 */
    int mapgen_land_placed;  /* 0063cba4 */
    u8 mapgen_land_placed_rest[8];
    int taxrate_dialog_active;  /* 0063cbb0 */
    TaxRateDialog *taxrate_dlg;  /* 0063cbb4 */
    u8 taxrate_dlg_rest[8];
    i32 eot_blink_timer;  /* 0063cbc0 */
    i32 unit_blink_timer;  /* 0063cbc4 */
    u8 unit_blink_timer_rest[8];
    u8 setup_backdrop_surface[96];  /* 0063cbd0 */
    i32 text_number_args[6];  /* 0063cc30 */
    i8 text_string_args[1];  /* 0063cc48 */
    i8 text_string_args_plus1;  /* 0063cc49 */
    u8 text_string_args_plus1_rest[258];
    i8 text_string_arg1[260];  /* 0063cd4c */
    i8 text_string_arg2[260];  /* 0063ce50 */
    i8 text_string_arg3[260];  /* 0063cf54 */
    u8 profile_difficulty_text[260];  /* 0063d058 */
    i8 text_string_arg5[260];  /* 0063d15c */
    u8 profile_barbarians_text[260];  /* 0063d260 */
    i8 game_profile_timer_text[260];  /* 0063d364 */
    u8 profile_count_text[260];  /* 0063d468 */
    u8 profile_count_text2[260];  /* 0063d56c */
    u8 profile_map_width_text[260];  /* 0063d670 */
    u8 profile_map_height_text[3396];  /* 0063d774 */
    i32 strpool_last_string;  /* 0063e4b8 */
    u8 strpool_last_string_rest[4];
};
#pragma pack(pop)
extern struct minimap_window_block minimap_window_block;
#pragma pack(push, 1)
struct strpool_block_block {
    i8 strpool_block[8];  /* 0063e4c0 */
    char *strpool_data;  /* 0063e4c8 */
    u8 strpool_data_rest[20];
};
#pragma pack(pop)
extern struct strpool_block_block strpool_block_block;
#pragma pack(push, 1)
struct civ_rating_title_font_block {
    GameFont civ_rating_title_font;  /* 0063e4e0 */
    i32 mp_meeting_wait_start_i32;  /* 0063e4e8 */
    int civ_rating_title_index;  /* 0063e4ec */
};
#pragma pack(pop)
extern struct civ_rating_title_font_block civ_rating_title_font_block;
#pragma pack(push, 1)
struct tiles_dll_block {
    DllHandle tiles_dll;  /* 0063e4f0 */
    int unit_ability_list_nonempty;  /* 0063e4f4 */
};
#pragma pack(pop)
extern struct tiles_dll_block tiles_dll_block;
#pragma pack(push, 1)
struct casualty_window_block {
    DibSurface casualty_window;  /* 0063e4f8 */
    u8 casualty_parent_wnd[16];  /* 0063e540 */
    u8 casualty_wnd_events[204];  /* 0063e550 */
    int casualty_client_x;  /* 0063e61c */
    int casualty_client_y;  /* 0063e620 */
    i32 casualty_client_width;  /* 0063e624 */
    u8 casualty_client_width_rest[404];
    int casualty_client_right;  /* 0063e7bc */
    u8 casualty_client_right_rest[88];
    Button casualty_close_button[4];  /* 0063e818 */
    Scrollbar casualty_scrollbar;  /* 0063e908 */
    int casualty_window_report_id;  /* 0063e948 */
    u8 casualty_window_report_id_rest[4];
    i32 casualty_page_count_i32;  /* 0063e950 */
    iptr casualty_window_civ;  /* 0063e954 */
    int casualty_scroll_pos;  /* 0063e958 */
    int casualty_rows_per_page;  /* 0063e95c */
    int casualty_first_row_y;  /* 0063e960 */
    i32 casualty_list_height_i32;  /* 0063e964 */
    i32 casualty_row_height;  /* 0063e968 */
    u8 casualty_row_height_rest[32];
    i32 casualty_vscroll_width_i32;  /* 0063e98c */
    int casualty_controls_created;  /* 0063e990 */
    i32 casualty_report_param_i32;  /* 0063e994 */
    u8 casualty_report_param_i32_rest[8];
    i32 credits_slot_widths[30];  /* 0063e9a0 */
    int civ_rating_percent;  /* 0063ea18 */
    u8 civ_rating_percent_rest[4];
};
#pragma pack(pop)
extern struct casualty_window_block casualty_window_block;
#pragma pack(push, 1)
struct credits_font_block {
    GameFont credits_font;  /* 0063ea20 */
    i32 credits_slot_lines[30];  /* 0063ea28 */
};
#pragma pack(pop)
extern struct credits_font_block credits_font_block;
#pragma pack(push, 1)
struct advisor_button_font_block {
    GameFont advisor_button_font;  /* 0063eaa0 */
    int credits_drawn_rect;  /* 0063eaa8 */
    int credits_drawn_rect_top;  /* 0063eaac */
    i32 credits_drawn_rect_right;  /* 0063eab0 */
    i32 credits_drawn_rect_bottom;  /* 0063eab4 */
};
#pragma pack(pop)
extern struct advisor_button_font_block advisor_button_font_block;
extern GameFont advisor_bold_font;
#pragma pack(push, 1)
struct advisor_font_block {
    GameFont advisor_font;  /* 0063eac0 */
    HallOfFameEntry hof_new_entry;  /* 0063eac8 */
};
#pragma pack(pop)
extern struct advisor_font_block advisor_font_block;
#pragma pack(push, 1)
struct advisor_window_block {
    DibSurface advisor_window;  /* 0063eb10 */
    WndBase advisor_wnd;  /* 0063eb58 */
    i8 advisor_wnd_events[204];  /* 0063eb68 */
    int advisor_client_x;  /* 0063ec34 */
    int advisor_client_y;  /* 0063ec38 */
    int advisor_client_width;  /* 0063ec3c */
    u8 advisor_client_width_rest[396];
    int advisor_client_rect;  /* 0063edcc */
    u8 advisor_client_rect_rest[4];
    int advisor_client_right;  /* 0063edd4 */
    u8 advisor_client_right_rest[88];
    Button advisor_close_button;  /* 0063ee30 */
    Button advisor_toggle_button;  /* 0063ee6c */
    Button advisor_alt_toggle_button[2];  /* 0063eea8 */
    Scrollbar advisor_scrollbar;  /* 0063ef20 */
    int advisor_report_id;  /* 0063ef60 */
    u8 advisor_report_id_rest[4];
    i32 advisor_page_count_i32;  /* 0063ef68 */
    iptr advisor_report_civ;  /* 0063ef6c */
    int advisor_first_row;  /* 0063ef70 */
    int advisor_rows_per_page;  /* 0063ef74 */
    i32 advisor_list_top_i32;  /* 0063ef78 */
    i32 advisor_list_height_i32;  /* 0063ef7c */
    i32 advisor_row_height;  /* 0063ef80 */
    u8 advisor_row_height_rest[8];
    int advisor_items_per_page;  /* 0063ef8c */
    i32 advisor_list_left_i32;  /* 0063ef90 */
    int advisor_list_width;  /* 0063ef94 */
    int advisor_column_width;  /* 0063ef98 */
    u8 advisor_column_width_rest[8];
    int advisor_vscroll_width;  /* 0063efa4 */
    int advisor_controls_created;  /* 0063efa8 */
    iptr advisor_report_param;  /* 0063efac */
    u8 advisor_report_param_rest[8];
    Sprite *credits_slot_sprites[30];  /* 0063efb8 */
    GameFont hall_of_fame_font;  /* 0063f030 */
    i8 credits_text_block[8];  /* 0063f038 */
    char *credits_text_data;  /* 0063f040 */
    u8 credits_text_data_rest[12];
    i32 credits_slot_x[30];  /* 0063f050 */
    HallOfFameEntry hall_of_fame[6];  /* 0063f0c8 */
    int mp_meeting_reply;  /* 0063f278 */
    u8 mp_meeting_reply_rest[708];
    int city_commodity_demand_scores[1];  /* 0063f540 */
    i32 city_wool_demand_score[1];  /* 0063f544 */
    int demand_score_beads;  /* 0063f548 */
    i32 demand_score_cloth_i32;  /* 0063f54c */
    i32 demand_score_salt_i32;  /* 0063f550 */
    int demand_score_coal;  /* 0063f554 */
    int demand_score_copper;  /* 0063f558 */
    int demand_score_dye;  /* 0063f55c */
    i32 demand_score_wine_i32;  /* 0063f560 */
    int demand_score_silk;  /* 0063f564 */
    int demand_score_silver;  /* 0063f568 */
    int demand_score_spice;  /* 0063f56c */
    i32 demand_score_gems_i32;  /* 0063f570 */
    int demand_score_gold;  /* 0063f574 */
    int demand_score_oil;  /* 0063f578 */
    int demand_score_uranium;  /* 0063f57c */
    i8 wonder_notice_flags[224];  /* 0063f580 */
    int nearest_city_dist;  /* 0063f660 */
    u8 nearest_city_dist_rest[4];
    int city_commodity_supply_scores[1];  /* 0063f668 */
    i32 supply_score_wool[1];  /* 0063f66c */
    int supply_score_beads;  /* 0063f670 */
    int supply_score_cloth;  /* 0063f674 */
    int supply_score_salt;  /* 0063f678 */
    int supply_score_coal;  /* 0063f67c */
    int supply_score_copper;  /* 0063f680 */
    int supply_score_dye;  /* 0063f684 */
    int supply_score_wine;  /* 0063f688 */
    int supply_score_silk;  /* 0063f68c */
    int supply_score_silver;  /* 0063f690 */
    int supply_score_spice;  /* 0063f694 */
    int supply_score_gems;  /* 0063f698 */
    int supply_score_gold;  /* 0063f69c */
    int supply_score_oil;  /* 0063f6a0 */
    int supply_score_uranium;  /* 0063f6a4 */
    u8 supply_score_uranium_rest[72];
};
#pragma pack(pop)
extern struct advisor_window_block advisor_window_block;
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
extern u8 popup_info_icon_sprite[64];
extern u8 signal_allied_icon_sprite[64];
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
extern DibSurface panel_texture_surface;
#pragma pack(push, 1)
struct peace_treaty_icon_sprite_block {
    u8 peace_treaty_icon_sprite[64];  /* 006409d8 */
    i32 city_flag_x_offsets[48];  /* 00640a18 */
    i32 city_flag_y_offsets[48];  /* 00640ad8 */
};
#pragma pack(pop)
extern struct peace_treaty_icon_sprite_block peace_treaty_icon_sprite_block;
#pragma pack(push, 1)
struct map_cursor_sprite_block {
    u8 map_cursor_sprite[64];  /* 00640b98 */
    i8 terrain_dither_sprites[2640];  /* 00640bd8 */
    i8 unexplored_dither_sprites[480];  /* 00641628 */
};
#pragma pack(pop)
extern struct map_cursor_sprite_block map_cursor_sprite_block;
#pragma pack(push, 1)
struct pollution_sprite_block {
    u8 pollution_sprite[64];  /* 00641808 */
    i8 unit_sprites[60];  /* 00641848 */
    u8 research_popup_sprite[720];  /* 00641884 */
    u8 research_popup_sprite2[3004];  /* 00641b54 */
};
#pragma pack(pop)
extern struct pollution_sprite_block pollution_sprite_block;
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
extern u8 scroll_up_button_sprite[64];
#pragma pack(push, 1)
struct hill_sprites_block {
    Sprite hill_sprites[16];  /* 006447b0 */
    i8 nuke_anim_sprites[664];  /* 00644b70 */
};
#pragma pack(pop)
extern struct hill_sprites_block hill_sprites_block;
extern u8 editor_unit_backdrop_sprite[64];
#pragma pack(push, 1)
struct war_icon_sprite_block {
    u8 war_icon_sprite[64];  /* 00644e48 */
    i8 unused_sprite_pair[120];  /* 00644e88 */
    i8 resource_shortage_icon_sprites[60];  /* 00644f00 */
    Sprite resource_icon_sprites;  /* 00644f3c */
    Sprite shield_shortage_icon_sprite;  /* 00644f78 */
    Sprite shield_icon_sprite;  /* 00644fb4 */
    Sprite trade_corruption_icon_sprite;  /* 00644ff0 */
    Sprite trade_icon_sprite;  /* 0064502c */
};
#pragma pack(pop)
extern struct war_icon_sprite_block war_icon_sprite_block;
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
extern u8 caption_view_mode_button_sprite[64];
extern Sprite mountain_sprites[16];
extern u8 ceasefire_icon_sprite[64];
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
extern u8 diplo_popup_portrait_sprite[64];
extern u8 unhappy_suppressed_icon_sprite[64];
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
extern u8 caption_zoom_out_sprite[64];
#pragma pack(push, 1)
struct tax_rate_icon_sprites_block {
    Sprite tax_rate_icon_sprites;  /* 00648860 */
    Sprite gold_coin_icon_sprite;  /* 0064889c */
    u8 science_icon_sprite[64];  /* 006488d8 */
    i8 lux_tax_sci_icon_sprites[7704];  /* 00648918 */
    i8 registry_value_buffer[2464];  /* 0064a730 */
    int cityview_screen_rect;  /* 0064b0d0 */
    int cityview_screen_rect_top;  /* 0064b0d4 */
    int screen_rect_right;  /* 0064b0d8 */
    int cityview_screen_rect_bottom;  /* 0064b0dc */
    u8 cityview_screen_rect_bottom_rest[8];
    int diplo_ai_declare_war;  /* 0064b0e8 */
    int diplo_ai_fear_level;  /* 0064b0ec */
    int diplo_gifts_this_meeting;  /* 0064b0f0 */
    int diplo_captured_city_count;  /* 0064b0f4 */
    int diplo_ai_aggressive;  /* 0064b0f8 */
    iptr diplo_strongest_enemy;  /* 0064b0fc */
    iptr diplo_weakest_enemy_civ;  /* 0064b100 */
    iptr diplo_war_target_civ;  /* 0064b104 */
    int diplo_last_contact_turn;  /* 0064b108 */
    int diplo_offer_tech_2nd;  /* 0064b10c */
    int diplo_attitude_civ;  /* 0064b110 */
    int diplo_cur_attitude;  /* 0064b114 */
    int diplo_tribute_demand;  /* 0064b118 */
    int diplo_target_is_leader;  /* 0064b11c */
    int diplo_attitude_target;  /* 0064b120 */
    int diplo_offer_tech_best;  /* 0064b124 */
    int diplo_suppressed_aggression;  /* 0064b128 */
    uint diplo_ai_outranks_target;  /* 0064b12c */
    int diplo_space_race_threat;  /* 0064b130 */
    int diplo_ai_units_near_cities;  /* 0064b134 */
    int diplo_had_ceasefire;  /* 0064b138 */
    int diplo_ai_initial_aggressive;  /* 0064b13c */
    int diplo_peace_inclination;  /* 0064b140 */
    iptr diplo_wanted_tech;  /* 0064b144 */
    int diplo_ai_request_mode;  /* 0064b148 */
    u8 diplo_ai_request_mode_rest[28];
};
#pragma pack(pop)
extern struct tax_rate_icon_sprites_block tax_rate_icon_sprites_block;
extern i32 commodity_names[16];
extern u8 commodity_names_rest[4];
extern int game_over_reason;
extern short map_cursor_y[2];
extern short map_cursor_x[2];
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
extern i32 government_names[7];
extern i8 player_turn_active;
extern u8 player_turn_active_rest[3];
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
struct prefs_game_rules_block {
    ushort prefs_game_rules[1];  /* 0064bc10 */
    i16 prefs_default_level_i16;  /* 0064bc12 */
    short prefs_difficulty;  /* 0064bc14 */
    i16 prefs_tribe;  /* 0064bc16 */
    short prefs_gender;  /* 0064bc18 */
    i16 prefs_science_rate;  /* 0064bc1a */
    i16 prefs_tax_rate;  /* 0064bc1c */
    uint prefs_game_options;  /* 0064bc1e */
    u16 prefs_message_options;  /* 0064bc22 */
    short prefs_enemies_choice;  /* 0064bc24 */
    short prefs_map_size;  /* 0064bc26 */
    short prefs_barbarity;  /* 0064bc28 */
    short prefs_world_params[1];  /* 0064bc2a */
    short prefs_world_land_form;  /* 0064bc2c */
    short prefs_world_temperature;  /* 0064bc2e */
    short prefs_world_climate;  /* 0064bc30 */
    short prefs_world_age;  /* 0064bc32 */
    i8 prefs_text_field[32];  /* 0064bc34 */
    short prefs_rules_choice;  /* 0064bc54 */
    short prefs_accelerated_start;  /* 0064bc56 */
    u8 prefs_accelerated_start_rest[8];
    ushort scenario_flags[1];  /* 0064bc60 */
    i8 scenario_name[80];  /* 0064bc62 */
    short scenario_tech_paradigm;  /* 0064bcb2 */
    short scenario_year_increment[1];  /* 0064bcb4 */
    i16 scenario_start_year;  /* 0064bcb6 */
    short scenario_max_turns;  /* 0064bcb8 */
    short scenario_objective_civ;  /* 0064bcba */
    i16 scenario_victory_thresholds[1];  /* 0064bcbc */
    short scenario_marginal_victory_score;  /* 0064bcbe */
    short scenario_marginal_defeat_score;  /* 0064bcc0 */
    short scenario_decisive_defeat_score;  /* 0064bcc2 */
    u8 scenario_decisive_defeat_score_rest[4];
};
#pragma pack(pop)
extern struct prefs_game_rules_block prefs_game_rules_block;
extern Cosmic cosmic;
extern u8 cosmic_rest[3];
extern int minimap_margin;
extern CivNames civ_names[8];
extern Improvement improvements[67];
extern Civ civs[8];
extern City cities[256];
#pragma pack(push, 1)
struct civ_saved_cursor_x_block {
    i16 civ_saved_cursor_x[8];  /* 00654b40 */
    i16 civ_saved_cursor_y[8];  /* 00654b50 */
    i16 civ_intro_shown[8];  /* 00654b60 */
    int mp_turn_time_limit_ms;  /* 00654b70 */
    i8 civ_passwords[255];  /* 00654b74 */
    char civ_passwords_last_byte;  /* 00654c73 */
    short mp_open_game;  /* 00654c74 */
    short mp_kill_civ_on_retire;  /* 00654c76 */
    short mp_show_human_starts;  /* 00654c78 */
    short mp_chat_from_start;  /* 00654c7a */
    short mp_simultaneous_setting;  /* 00654c7c */
    i32 cheat_saved_turn_time_ms;  /* 00654c7e */
    u8 cheat_saved_turn_time_ms_rest[290];
    i8 civ_email_addresses[256];  /* 00654da4 */
    i8 net_player_setup_table[256];  /* 00654ea4 */
    i8 saved_turn_civ;  /* 00654fa4 */
    u8 saved_turn_civ_rest[1];
    i16 saved_resume_new_game;  /* 00654fa6 */
    short autoplay_active;  /* 00654fa8 */
    short autoplay_turns_left;  /* 00654faa */
    short mp_double_production;  /* 00654fac */
    short mp_double_moves;  /* 00654fae */
    i16 net_claimed_civs_mask;  /* 00654fb0 */
    u8 net_claimed_civs_mask_rest[34];
    i8 secondary_menu_handle[4];  /* 00654fd4 */
    i32 game_setup_complete;  /* 00654fd8 */
    u8 game_setup_complete_rest[4];
    GovTitle government_titles[7];  /* 00654fe0 */
    u8 government_titles_rest[8];
    i8 game_home_dir[264];  /* 00655020 */
};
#pragma pack(pop)
extern struct civ_saved_cursor_x_block civ_saved_cursor_x_block;
extern GovTitle *const government_titles;  /* = civ_saved_cursor_x_block.government_titles */
#pragma pack(push, 1)
struct destroyed_civ_count_block {
    i16 destroyed_civ_count[1];  /* 00655128 */
    i16 destroyed_civ_turns[12];  /* 0065512a */
    i8 destroyed_civ_killers[12];  /* 00655142 */
    i8 destroyed_civ_portraits[12];  /* 0065514e */
    i8 destroyed_civ_names[294];  /* 0065515a */
    int saved_layout_valid;  /* 00655280 */
};
#pragma pack(pop)
extern struct destroyed_civ_count_block destroyed_civ_count_block;
#pragma pack(push, 1)
struct saved_screen_rect_block {
    i32 saved_screen_rect[1];  /* 00655284 */
    i32 saved_screen_rect_top;  /* 00655288 */
    i32 saved_screen_rect_right;  /* 0065528c */
    i32 saved_screen_rect_bottom;  /* 00655290 */
    u8 layout_rect_main[16];  /* 00655294 */
};
#pragma pack(pop)
extern struct saved_screen_rect_block saved_screen_rect_block;
#pragma pack(push, 1)
struct map_view_saved_rects_block {
    i8 map_view_saved_rects[4];  /* 006552a4 */
    i8 map_view_saved_rect_tops[124];  /* 006552a8 */
};
#pragma pack(pop)
extern struct map_view_saved_rects_block map_view_saved_rects_block;
#pragma pack(push, 1)
struct minimap_wnd_rect_block {
    i32 minimap_wnd_rect;  /* 00655324 */
    int minimap_wnd_rect_top;  /* 00655328 */
    u8 minimap_wnd_rect_top_rest[8];
    i32 status_wnd_rect;  /* 00655334 */
    i32 status_wnd_rect_top;  /* 00655338 */
    u8 status_wnd_rect_top_rest[8];
    i32 city_wnd_rect;  /* 00655344 */
    i32 city_wnd_rect_top;  /* 00655348 */
    u8 city_wnd_rect_top_rest[12];
};
#pragma pack(pop)
extern struct minimap_wnd_rect_block minimap_wnd_rect_block;
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
struct main_frame_wnd_block {
    ModalMenuWindow main_frame_wnd;  /* 006553d8 */
    OrderName order_names[13];  /* 00655490 */
    Leader leaders[21];  /* 006554f8 */
    i8 game_txt_path[256];  /* 006558e8 */
    i8 rules_txt_path[256];  /* 006559e8 */
};
#pragma pack(pop)
extern struct main_frame_wnd_block main_frame_wnd_block;
extern OrderName *const order_names;  /* = main_frame_wnd_block.order_names */
extern Leader *const leaders;  /* = main_frame_wnd_block.leaders */
extern GameState game;
extern u8 game_rest[6];
#pragma pack(push, 1)
struct power_graph_history_block {
    i8 power_graph_history[1208];  /* 00655c38 */
    Unit units[2048];  /* 006560f0 */
    i16 dummy_unit_x;  /* 006660f0 */
    i16 dummy_unit_y;  /* 006660f2 */
    i16 dummy_unit_flags;  /* 006660f4 */
    i8 dummy_unit_type;  /* 006660f6 */
    i8 dummy_unit_owner;  /* 006660f7 */
    i8 dummy_unit_moves_used;  /* 006660f8 */
    i8 dummy_unit_visibility;  /* 006660f9 */
    u8 dummy_unit_visibility_rest[3];
    i8 dummy_unit_counter;  /* 006660fd */
    i8 dummy_unit_byte_0e;  /* 006660fe */
    i8 dummy_unit_orders;  /* 006660ff */
    i8 dummy_unit_home_city;  /* 00666100 */
    u8 dummy_unit_home_city_rest[1];
    i16 dummy_unit_goto_x;  /* 00666102 */
    i16 dummy_unit_goto_y;  /* 00666104 */
    i16 dummy_unit_prev_in_stack;  /* 00666106 */
    i16 dummy_unit_next_in_stack;  /* 00666108 */
    u8 dummy_unit_next_in_stack_rest[6];
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
extern Unit *const units;  /* = power_graph_history_block.units */
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
struct prefs_main_menu_choice_block {
    i16 prefs_main_menu_choice[1];  /* 00666538 */
    i16 prefs_multiplayer_type;  /* 0066653a */
    short prefs_hotseat_menu_choice;  /* 0066653c */
    short prefs_lan_menu_choice;  /* 0066653e */
    i16 mp_prefs_setting_1_i16;  /* 00666540 */
    i16 prefs_pbem_menu_choice;  /* 00666542 */
    ushort prefs_net_protocol;  /* 00666544 */
    short prefs_mp_open_game;  /* 00666546 */
    short prefs_mp_kill_on_retire;  /* 00666548 */
    short prefs_mp_show_human_starts;  /* 0066654a */
    short prefs_mp_chat_from_start;  /* 0066654c */
    short prefs_mp_simultaneous_moves;  /* 0066654e */
    i8 prefs_net_game_name[32];  /* 00666550 */
    i8 prefs_net_player_name[32];  /* 00666570 */
    i8 prefs_email_address[32];  /* 00666590 */
    i8 prefs_net_ip_address[32];  /* 006665b0 */
    i16 prefs_turn_timer_choice;  /* 006665d0 */
    i8 prefs_turn_timer_custom[6];  /* 006665d2 */
    i16 prefs_hotseat_players_choice;  /* 006665d8 */
    i8 prefs_parley_wnd_rect[16];  /* 006665da */
    i8 prefs_chat_wnd_rect[16];  /* 006665ea */
    i16 prefs_clear_chat_on_new_game;  /* 006665fa */
    i16 prefs_clear_chat_on_boot;  /* 006665fc */
    short prefs_mp_double_production;  /* 006665fe */
    short prefs_mp_double_moves;  /* 00666600 */
    i8 mp_prefs_setting_2;  /* 00666602 */
    u8 mp_prefs_setting_2_rest[23];
    i8 mp_prefs_setting_3;  /* 0066661a */
    u8 mp_prefs_setting_3_rest[23];
    i8 mp_prefs_setting_4;  /* 00666632 */
    u8 mp_prefs_setting_4_rest[23];
    i8 mp_prefs_settings_table[168];  /* 0066664a */
    i16 mp_prefs_setting_5_i16;  /* 006666f2 */
    u8 mp_prefs_setting_5_i16_rest[22404];
    i32 parley_units_by_city;  /* 0066be78 */
    u8 parley_units_by_city_rest[4];
    i32 parley_list_own_side[4];  /* 0066be80 */
    i32 unit_type_name_ptrs[76];  /* 0066be90 */
    iptr snd_last_effect_arg;  /* 0066bfc0 */
    iptr snd_last_effect_id;  /* 0066bfc4 */
    u8 snd_last_effect_id_rest[8];
};
#pragma pack(pop)
extern struct prefs_main_menu_choice_block prefs_main_menu_choice_block;
#pragma pack(push, 1)
struct grey_palette_block {
    u8 grey_palette[1080];  /* 0066bfd0 */
    int grey_remap_handle;  /* 0066c408 */
    u8 grey_remap_handle_rest[220];
    i8 save_file_path[1];  /* 0066c4e8 */
    i8 save_file_ext_bare[15];  /* 0066c4e9 */
    i8 save_file_filter_buf[264];  /* 0066c4f8 */
    i16 save_layout_record[1];  /* 0066c600 */
    u8 str_empty_189[8];  /* 0066c602 */
    u8 saved_layout_rects[72];  /* 0066c60a */
    u8 str_empty_190[8];  /* 0066c652 */
    u8 str_empty_191[8];  /* 0066c65a */
    u8 saved_layout_rect2[14];  /* 0066c662 */
};
#pragma pack(pop)
extern struct grey_palette_block grey_palette_block;
#pragma pack(push, 1)
struct scenario_title_surface_block {
    u8 scenario_title_surface[176];  /* 0066c670 */
    i8 coast_quarter_land_masks[8];  /* 0066c720 */
};
#pragma pack(pop)
extern struct scenario_title_surface_block scenario_title_surface_block;
extern u8 static_scrollbar_1[64];
extern u8 static_scrollbar_2[64];
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
    i32 net_meet_pending_x[8];  /* 00673a78 */
    i32 net_meet_pending_y[8];  /* 00673a98 */
    i32 net_meet_pending_civ[8];  /* 00673ab8 */
    i32 net_meet_pending_ai_civ[8];  /* 00673ad8 */
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
    i8 text_token_buf[260];  /* 00673e10 */
    iptr text_line_cursor;  /* 00673f14 */
    u8 text_line_cursor_rest[60];
    int score_objective_outcome;  /* 00673f54 */
    int score_pollution;  /* 00673f58 */
    iptr score_wonders;  /* 00673f5c */
    int score_spaceship_arrived;  /* 00673f60 */
    iptr score_objective_civ_value;  /* 00673f64 */
    int score_scenario_points;  /* 00673f68 */
    int score_future_techs;  /* 00673f6c */
    iptr score_total_objective_value;  /* 00673f70 */
    int score_barbarian_bonus;  /* 00673f74 */
    iptr score_citizens;  /* 00673f78 */
    int score_conquest_bonus;  /* 00673f7c */
    iptr score_civ_objective_value;  /* 00673f80 */
    int score_spaceship_in_flight;  /* 00673f84 */
    int score_total;  /* 00673f88 */
    int score_peace;  /* 00673f8c */
    u8 score_peace_rest[16];
    int path_goal_x;  /* 00673fa0 */
    int path_goal_y;  /* 00673fa4 */
    int path_coarse_start_x;  /* 00673fa8 */
    int path_coarse_start_y;  /* 00673fac */
    int path_grid_center_x;  /* 00673fb0 */
    int path_grid_center_y;  /* 00673fb4 */
    int path_flood_goal_y;  /* 00673fb8 */
    iptr path_queue_tail;  /* 00673fbc */
    int path_result_cost;  /* 00673fc0 */
    iptr path_queue_head;  /* 00673fc4 */
    i32 path_queue_x[2304];  /* 00673fc8 */
    i32 path_queue_y[2305];  /* 006763c8 */
    int path_flood_goal_x;  /* 006787cc */
    u8 path_flood_goal_x_rest[3695];
    i8 text_buffer_minus1[1];  /* 0067963f */
    i8 text_buffer[1];  /* 00679640 */
    i8 text_buffer_char_1;  /* 00679641 */
    i8 text_buffer_plus2[1];  /* 00679642 */
    i8 text_buffer_char_3;  /* 00679643 */
    u8 text_buffer_char_3_rest[2468];
    iptr diffeng_scan_ptr;  /* 00679fe8 */
    i32 diffeng_scan_offset_i32;  /* 00679fec */
    i32 diffeng_packet_i32;  /* 00679ff0 */
    i32 diffeng_packet_msg_type_i32;  /* 00679ff4 */
    i32 diffeng_packet_size_i32;  /* 00679ff8 */
    u8 diffeng_packet_size_i32_rest[4];
    i32 diffeng_packet_record_count_i32;  /* 0067a000 */
    u8 diffeng_packet_record_count_i32_rest[24];
    i32 diffeng_packet_payload[249];  /* 0067a01c */
    uint diffeng_mirror_size[1];  /* 0067a400 */
    i32 diffeng_scan_section;  /* 0067a404 */
    int diffeng_mirror_end;  /* 0067a408 */
    u8 diffeng_mirror_end_rest[4];
};
#pragma pack(pop)
extern struct map_views_block map_views_block;
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
#pragma pack(push, 1)
struct diffeng_game_section_block {
    int diffeng_game_section[3];  /* 0067a428 */
    i32 diffeng_game_checksum_i32;  /* 0067a434 */
    iptr diffeng_game_offset;  /* 0067a438 */
    iptr diffeng_game_data;  /* 0067a43c */
};
#pragma pack(pop)
extern struct diffeng_game_section_block diffeng_game_section_block;
#pragma pack(push, 1)
struct diffeng_civ_names_section_block {
    int diffeng_civ_names_section[3];  /* 0067a440 */
    i32 diffeng_civ_names_checksum_i32;  /* 0067a44c */
    u8 diffeng_civ_names_checksum_i32_rest[4];
    iptr diffeng_civ_names_data;  /* 0067a454 */
};
#pragma pack(pop)
extern struct diffeng_civ_names_section_block diffeng_civ_names_section_block;
#pragma pack(push, 1)
struct diffeng_civs_section_block {
    int diffeng_civs_section[3];  /* 0067a458 */
    i32 diffeng_civs_checksum_i32;  /* 0067a464 */
    u8 diffeng_civs_checksum_i32_rest[4];
    iptr diffeng_civs_data;  /* 0067a46c */
};
#pragma pack(pop)
extern struct diffeng_civs_section_block diffeng_civs_section_block;
extern DiffEngSection diffeng_landmass_section;
extern DiffEngSection diffeng_units_section;
extern DiffEngSection diffeng_cities_section;
#pragma pack(push, 1)
struct diffeng_leaders_section_block {
    int diffeng_leaders_section[3];  /* 0067a4b8 */
    i32 diffeng_leaders_checksum_i32;  /* 0067a4c4 */
    u8 diffeng_leaders_checksum_i32_rest[4];
    iptr diffeng_leaders_data;  /* 0067a4cc */
};
#pragma pack(pop)
extern struct diffeng_leaders_section_block diffeng_leaders_section_block;
extern DiffEngSection diffeng_power_graph_section;
#pragma pack(push, 1)
struct diffeng_scenario_section_block {
    int diffeng_scenario_section[3];  /* 0067a4e8 */
    i32 diffeng_scenario_checksum_i32;  /* 0067a4f4 */
    u8 diffeng_scenario_checksum_i32_rest[4];
    iptr diffeng_scenario_data;  /* 0067a4fc */
};
#pragma pack(pop)
extern struct diffeng_scenario_section_block diffeng_scenario_section_block;
extern DiffEngSection diffeng_destroyed_civs_section;
#pragma pack(push, 1)
struct diffeng_civ_view_section_block {
    int diffeng_civ_view_section[3];  /* 0067a518 */
    i32 diffeng_civ_view_checksum_i32;  /* 0067a524 */
    u8 diffeng_civ_view_checksum_i32_rest[4];
    iptr diffeng_civ_view_data;  /* 0067a52c */
};
#pragma pack(pop)
extern struct diffeng_civ_view_section_block diffeng_civ_view_section_block;
#pragma pack(push, 1)
struct diffeng_map_header_section_block {
    int diffeng_map_header_section[3];  /* 0067a530 */
    i32 diffeng_map_header_checksum_i32;  /* 0067a53c */
    u8 diffeng_map_header_checksum_i32_rest[4];
    iptr diffeng_map_header_data;  /* 0067a544 */
};
#pragma pack(pop)
extern struct diffeng_map_header_section_block diffeng_map_header_section_block;
extern DiffEngSection diffeng_map_coarse0_section;
extern DiffEngSection diffeng_map_coarse1_section;
#pragma pack(push, 1)
struct diffeng_map_known1_section_block {
    i8 diffeng_map_known1_section[16];  /* 0067a578 */
    size_t diffeng_client_mirror_size;  /* 0067a588 */
    u8 diffeng_client_mirror_size_rest[4];
};
#pragma pack(pop)
extern struct diffeng_map_known1_section_block diffeng_map_known1_section_block;
extern DiffEngSection diffeng_map_known2_section;
extern DiffEngSection diffeng_map_known3_section;
extern DiffEngSection diffeng_map_known4_section;
extern DiffEngSection diffeng_map_known5_section;
extern DiffEngSection diffeng_map_known6_section;
extern DiffEngSection diffeng_map_known7_section;
extern DiffEngSection diffeng_map_tiles_section;
#pragma pack(push, 1)
struct diffeng_events_section_block {
    int diffeng_events_section[3];  /* 0067a638 */
    i32 diffeng_events_checksum_i32;  /* 0067a644 */
    u8 diffeng_events_checksum_i32_rest[4];
    iptr diffeng_events_data;  /* 0067a64c */
    u8 diffeng_events_data_rest[328];
};
#pragma pack(pop)
extern struct diffeng_events_section_block diffeng_events_section_block;
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
    int wonder_movie_wnd;  /* 006a1864 */
    u8 wonder_movie_wnd_rest[8];
    uptr net_bribe_answer;  /* 006a1870 */
    u8 net_bribe_answer_rest[12];
    RulesEditSection rules_edit_sections[8];  /* 006a1880 */
};
#pragma pack(pop)
extern struct font_size16_bold_block font_size16_bold_block;
#pragma pack(push, 1)
struct editor_text_dlg_wnd_block {
    DibSurface editor_text_dlg_wnd;  /* 006a18c0 */
    u8 editor_text_dlg_inner[204];  /* 006a1908 */
    i32 editor_text_dlg_style_i32;  /* 006a19d4 */
    int editor_text_dlg_caption_height;  /* 006a19d8 */
    int editor_text_dlg_border_width;  /* 006a19dc */
    int editor_text_dlg_resize_border;  /* 006a19e0 */
    int editor_text_dlg_client_x;  /* 006a19e4 */
    int editor_text_dlg_client_y;  /* 006a19e8 */
    int editor_text_dlg_client_width;  /* 006a19ec */
    int editor_text_dlg_client_height;  /* 006a19f0 */
    i8 editor_text_dlg_title[200];  /* 006a19f4 */
    i32 editor_text_dlg_caption_buttons_i32;  /* 006a1abc */
    u8 editor_text_dlg_caption_buttons_i32_rest[168];
    i32 editor_text_dlg_caption_handler_i32;  /* 006a1b68 */
    u8 editor_text_dlg_caption_handler_i32_rest[16];
    u8 editor_text_fill_rect[16];  /* 006a1b7c */
    i32 editor_text_dlg_title_font_i32;  /* 006a1b8c */
    i32 editor_text_dlg_title_small_font_i32;  /* 006a1b90 */
    i32 editor_text_dlg_text_font_i32;  /* 006a1b94 */
    i8 editor_govt_names[14];  /* 006a1b98 */
    i8 editor_govt_name_ends[306];  /* 006a1ba6 */
    i8 editor_commodity_names[9];  /* 006a1cd8 */
    i8 editor_commodity_name_ends[151];  /* 006a1ce1 */
    i32 *editor_text_lines_out;  /* 006a1d78 */
    i32 editor_running;  /* 006a1d7c */
    int editor_next_ctrl_id;  /* 006a1d80 */
    u8 editor_next_ctrl_id_rest[4];
    union {
        struct {
            i8 editor_item_names[23];  /* 006a1d88 */
            i8 editor_item_name24_ends[16];  /* 006a1d9f */
            i8 editor_item_name_ends[3153];  /* 006a1daf */
            i8 editor_ctrl_values[808];  /* 006a2a00 */
            i32 editor_items[1];  /* 006a2d28 */
            i32 editor_item_field1[1];  /* 006a2d2c */
            i32 editor_item_field2[1];  /* 006a2d30 */
            i32 editor_item_field3[1];  /* 006a2d34 */
            i32 editor_item_field4[1];  /* 006a2d38 */
            i32 editor_item_field5[1];  /* 006a2d3c */
            i32 editor_item_field6[1];  /* 006a2d40 */
            i32 editor_item_field7[1];  /* 006a2d44 */
            i32 editor_item_field8[1];  /* 006a2d48 */
            i32 editor_item_field9[1];  /* 006a2d4c */
            i32 editor_item_field10[1];  /* 006a2d50 */
            i32 editor_item_field11[1];  /* 006a2d54 */
            u8 editor_item_flags[1];  /* 006a2d58 */
            i8 editor_item_flags_byte1[3];  /* 006a2d59 */
            i32 editor_item_field13[1];  /* 006a2d5c */
            i32 editor_item_field14_i32;  /* 006a2d60 */
            i32 editor_item_field15_i32;  /* 006a2d64 */
            i32 editor_item_field16_i32;  /* 006a2d68 */
            i32 editor_item_field17_i32;  /* 006a2d6c */
            i32 editor_item_field18_i32;  /* 006a2d70 */
            i32 editor_item_field19_i32;  /* 006a2d74 */
            i32 editor_item_field20_i32;  /* 006a2d78 */
            i32 editor_item_field21_i32;  /* 006a2d7c */
            u8 cosmic_edit_values[4];  /* 006a2d80 */
            u8 cosmic_edit_trireme_loss;  /* 006a2d84 */
            u8 cosmic_edit_trireme_loss_rest[3];
            u8 cosmic_edit_food_per_citizen;  /* 006a2d88 */
            u8 cosmic_edit_food_per_citizen_rest[3];
            u8 cosmic_edit_food_box_rows;  /* 006a2d8c */
            u8 cosmic_edit_food_box_rows_rest[3];
            u8 cosmic_edit_shield_box_rows;  /* 006a2d90 */
            u8 cosmic_edit_shield_box_rows_rest[3];
            u8 cosmic_edit_settler_food_low;  /* 006a2d94 */
            u8 cosmic_edit_settler_food_low_rest[3];
            u8 cosmic_edit_settler_food_high;  /* 006a2d98 */
            u8 cosmic_edit_settler_food_high_rest[3];
            u8 cosmic_edit_unhappy_size;  /* 006a2d9c */
            u8 cosmic_edit_unhappy_size_rest[3];
            u8 cosmic_edit_riot_factor;  /* 006a2da0 */
            u8 cosmic_edit_riot_factor_rest[3];
            u8 cosmic_edit_aqueduct_size;  /* 006a2da4 */
            u8 cosmic_edit_aqueduct_size_rest[3];
            u8 cosmic_edit_sewer_size;  /* 006a2da8 */
            u8 cosmic_edit_sewer_size_rest[3];
            u8 cosmic_edit_tech_paradigm;  /* 006a2dac */
            u8 cosmic_edit_tech_paradigm_rest[3];
            u8 cosmic_edit_transform_time;  /* 006a2db0 */
            u8 cosmic_edit_transform_time_rest[3];
            u8 cosmic_edit_monarchy_support;  /* 006a2db4 */
            u8 cosmic_edit_monarchy_support_rest[3];
            u8 cosmic_edit_communism_support;  /* 006a2db8 */
            u8 cosmic_edit_communism_support_rest[3];
            u8 cosmic_edit_fundamentalism_support;  /* 006a2dbc */
            u8 cosmic_edit_fundamentalism_support_rest[3];
            u8 cosmic_edit_communism_palace_dist;  /* 006a2dc0 */
            u8 cosmic_edit_communism_palace_dist_rest[3];
            u8 cosmic_edit_fund_science_loss;  /* 006a2dc4 */
            u8 cosmic_edit_fund_science_loss_rest[3];
            u8 cosmic_edit_change_penalty;  /* 006a2dc8 */
            u8 cosmic_edit_change_penalty_rest[3];
            u8 cosmic_edit_paradrop_range;  /* 006a2dcc */
            u8 cosmic_edit_paradrop_range_rest[3];
            u8 cosmic_edit_mass_thrust_paradigm;  /* 006a2dd0 */
            u8 cosmic_edit_mass_thrust_paradigm_rest[3];
            u8 cosmic_edit_fund_max_science;  /* 006a2dd4 */
            u8 cosmic_edit_fund_max_science_rest[8627];
        };
        struct {
            i8 editor_item_name_recs[100][40];  /* 006a1d88 an item's name */
            EditorItem editor_item_recs[100];  /* 006a2d28 an item's values (cosmic editor: record 1) */
        };
    };
    EditorWindow *active_editor_wnd;  /* 006a4f88 */
    u8 active_editor_wnd_rest[4];
};
#pragma pack(pop)
extern struct editor_text_dlg_wnd_block editor_text_dlg_wnd_block;
#pragma pack(push, 1)
struct editor_font_block {
    GameFont editor_font;  /* 006a4f90 */
    i32 editor_text_result;  /* 006a4f98 */
    i32 editor_text_active;  /* 006a4f9c */
};
#pragma pack(pop)
extern struct editor_font_block editor_font_block;
#pragma pack(push, 1)
struct editor_text_edit_block {
    u8 editor_text_edit[2920];  /* 006a4fa0 */
    i32 spaceship_wnd_open_ticks_i32;  /* 006a5b08 */
    u8 spaceship_wnd_open_ticks_i32_rest[4];
    union {
        struct {
            i8 spaceship_wnd_part_counts[4];  /* 006a5b10 */
            int spaceship_wnd_component_count;  /* 006a5b14 */
            u8 spaceship_wnd_component_count_rest[32];
        };
        i32 spaceship_wnd_part_count_list[10];  /* 006a5b10 */
    };
};
#pragma pack(pop)
extern struct editor_text_edit_block editor_text_edit_block;
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
struct main_menu_block {
    u8 main_menu[48];  /* 006a64f8 */
    int city_saved_shield_box;  /* 006a6528 */
    u8 city_saved_shield_box_rest[4];
    i8 city_tile_flags[28];  /* 006a6530 */
    int city_unassigned_citizens;  /* 006a654c */
    int city_happy_count;  /* 006a6550 */
    int city_tax_total;  /* 006a6554 */
    int city_food_routes_in;  /* 006a6558 */
    int city_home_attack_units;  /* 006a655c */
    int city_food_box_rows;  /* 006a6560 */
    uint city_martial_law_units;  /* 006a6564 */
    int city_shield_support;  /* 006a6568 */
    int city_shield_waste;  /* 006a656c */
    int city_food_route_partner;  /* 006a6570 */
    uint city_capital_link;  /* 006a6574 */
    int city_science_total;  /* 006a6578 */
    int city_shield_rows;  /* 006a657c */
    int city_trade_corruption;  /* 006a6580 */
    int city_pollution;  /* 006a6584 */
    int city_capital_distance;  /* 006a6588 */
    u8 city_capital_distance_rest[4];
    i32 city_route_trade_values[3];  /* 006a6590 */
    int city_angry_count;  /* 006a659c */
    int city_zoom_requested;  /* 006a65a0 */
    i32 city_saved_build_category;  /* 006a65a4 */
    int city_unhappy_count;  /* 006a65a8 */
    int city_notify_enabled;  /* 006a65ac */
    int city_food_routes_out;  /* 006a65b0 */
    u8 city_food_routes_out_rest[4];
    i32 city_tile_yields[1];  /* 006a65b8 */
    int city_tile_shields;  /* 006a65bc */
    int city_tile_trade;  /* 006a65c0 */
    int city_pop_pollution_factor;  /* 006a65c4 */
    int city_yield_totals[1];  /* 006a65c8 */
    int city_total_shields;  /* 006a65cc */
    int city_total_trade;  /* 006a65d0 */
    int city_tile_job_priority;  /* 006a65d4 */
    int city_settlers_supported;  /* 006a65d8 */
    i32 city_workers_overflow;  /* 006a65dc */
    iptr city_tile_job_x;  /* 006a65e0 */
    int city_aggressive_units;  /* 006a65e4 */
    int city_tile_job_y;  /* 006a65e8 */
    u8 city_tile_job_y_rest[4];
    i8 city_unhappy_by_stage[1];  /* 006a65f0 */
    byte city_unhappy_after_lux;  /* 006a65f1 */
    u8 city_unhappy_after_lux_rest[1];
    byte city_unhappy_after_units;  /* 006a65f3 */
    byte city_unhappy_after_wonders;  /* 006a65f4 */
    u8 city_unhappy_after_wonders_rest[3];
    int city_pollution_divisor;  /* 006a65f8 */
    int city_lux_total;  /* 006a65fc */
    iptr city_capital_nearest;  /* 006a6600 */
    int city_specialist_count;  /* 006a6604 */
    int settler_food_cost;  /* 006a6608 */
    int city_support_unit_count;  /* 006a660c */
    i32 city_completed_improvement_i32;  /* 006a6610 */
    u8 city_completed_improvement_i32_rest[4];
    int city_building_content;  /* 006a6618 */
    int city_food_surplus;  /* 006a661c */
    i8 city_happy_by_stage[1];  /* 006a6620 */
    byte city_happy_after_lux;  /* 006a6621 */
    u8 city_happy_after_lux_rest[1];
    byte city_happy_after_units;  /* 006a6623 */
    byte city_happy_after_wonders;  /* 006a6624 */
    u8 city_happy_after_wonders_rest[3];
    i8 city_angry_by_stage[64];  /* 006a6628 */
};
#pragma pack(pop)
extern struct main_menu_block main_menu_block;
#pragma pack(push, 1)
struct pedia_window_block {
    SurfaceWindow pedia_window;  /* 006a6668 the PediaWindow's SurfaceWindow; its members follow */
    int pedia_closed;  /* 006a677c */
    i32 pedia_category;  /* 006a6780 */
    i32 pedia_page_mode;  /* 006a6784 */
    int pedia_topic;  /* 006a6788 */
    int pedia_modal_active;  /* 006a678c */
    i32 pedia_history_count;  /* 006a6790 */
    i32 pedia_history_topics[100];  /* 006a6794 */
    i32 pedia_history_page_modes[100];  /* 006a6924 */
    i32 pedia_history_categories[100];  /* 006a6ab4 */
    u8 pedia_history_categories_rest[28];
    u8 pedia_describe_rect[32];  /* 006a6c60 */
    i8 pedia_title_text[64];  /* 006a6c80 */
    int pedia_advance_entry_count;  /* 006a6cc0 */
    u8 pedia_advance_entry_count_rest[800];
    int pedia_improvement_entry_count;  /* 006a6fe4 */
    u8 pedia_improvement_entry_count_rest[312];
    int pedia_wonder_entry_count;  /* 006a7120 */
    u8 pedia_wonder_entry_count_rest[224];
    int pedia_unit_entry_count;  /* 006a7204 */
    u8 pedia_unit_entry_count_rest[496];
    int pedia_government_entry_count;  /* 006a73f8 */
    u8 pedia_government_entry_count_rest[56];
    int pedia_terrain_count;  /* 006a7434 */
    u8 pedia_terrain_count_rest[132];
    i32 pedia_terrain_ids[33];  /* 006a74bc */
    int pedia_concept_entry_count;  /* 006a7540 */
    u8 pedia_concept_entry_count_rest[2048];
    PediaListNode *pedia_advance_names;  /* 006a7d44 */
    PediaListNode *pedia_improvement_names;  /* 006a7d48 */
    PediaListNode *pedia_wonder_names;  /* 006a7d4c */
    PediaListNode *pedia_unit_names;  /* 006a7d50 */
    PediaListNode *pedia_government_names;  /* 006a7d54 */
    PediaListNode *pedia_terrain_names;  /* 006a7d58 */
    PediaListNode *pedia_concept_names;  /* 006a7d5c */
    u8 pedia_concept_names_rest[1000];
    Scrollbar pedia_list_scrollbar;  /* 006a8148 */
    Hotspot *pedia_close_hotspot;  /* 006a8188 */
    int pedia_list_rect;  /* 006a818c */
    int pedia_list_rect_top;  /* 006a8190 */
    int pedia_list_rect_right;  /* 006a8194 */
    int pedia_list_rect_bottom;  /* 006a8198 */
    int pedia_list_count;  /* 006a819c */
    u8 pedia_list_count_rest[1024];
    i32 pedia_selected_item;  /* 006a85a0 */
    int pedia_list_top;  /* 006a85a4 */
    PediaLink *pedia_link_list;  /* 006a85a8 */
    i32 pedia_popped_category;  /* 006a85ac */
    i32 pedia_popped_page_mode;  /* 006a85b0 */
    u8 pedia_popped_page_mode_rest[1612];
};
#pragma pack(pop)
extern struct pedia_window_block pedia_window_block;
#pragma pack(push, 1)
struct game_palette_block {
    u8 game_palette[1296];  /* 006a8c00 */
    int event_dont_play_wonders;  /* 006a9110 */
    u8 event_dont_play_wonders_rest[12];
};
#pragma pack(pop)
extern struct game_palette_block game_palette_block;
extern DibSurface city_tile_pick_surface;
extern GameFont city_window_small_font;
extern DibSurface city_window_bg_surface;
#pragma pack(push, 1)
struct city_window_block {
    DibSurface city_window;  /* 006a91b8 */
    WndBase city_window_wnd[41];  /* 006a9200 */
    ClickMapHitTestView city_window_clickmap;  /* 006a9490 */
    i32 city_window_city_i32;  /* 006aa754 */
    i32 city_window_minimized_i32;  /* 006aa758 */
    int city_window_closed;  /* 006aa75c */
    int city_window_refresh_locked;  /* 006aa760 */
    int city_window_open_mode;  /* 006aa764 */
    i32 city_info_panel_mode;  /* 006aa768 */
    i32 city_improvement_list_top;  /* 006aa76c */
    i32 city_improvement_list_count_i32;  /* 006aa770 */
    u8 city_improvement_list_count_i32_rest[16];
    i32 city_window_prev_offset_x_i32;  /* 006aa784 */
    u8 city_window_prev_offset_x_i32_rest[4];
    int city_window_size_level;  /* 006aa78c */
    i32 city_window_map_zoom;  /* 006aa790 */
    u8 city_window_map_zoom_rest[16];
    int city_window_map_rect;  /* 006aa7a4 */
    int city_window_map_rect_top;  /* 006aa7a8 */
    u8 city_window_map_rect_top_rest[236];
};
#pragma pack(pop)
extern struct city_window_block city_window_block;
extern GameFont city_window_font[152];
extern u8 destroyed_civs_text_bg[96];
#pragma pack(push, 1)
struct game_section_dialog_block {
    u8 game_section_dialog[908];  /* 006aadb8 */
    union {
        struct {
            i8 setup_prev_player_tribe[4];  /* 006ab144 */
            i8 setup_player_tribes[48];  /* 006ab148 */
        };
        i32 setup_slot_tribes[13];  /* 006ab144 */
    };
};
#pragma pack(pop)
extern struct game_section_dialog_block game_section_dialog_block;
#pragma pack(push, 1)
struct caption_small_font_block {
    GameFont caption_small_font;  /* 006ab178 */
    i32 screen_rect[1];  /* 006ab180 */
    i32 screen_rect_top;  /* 006ab184 */
    i32 ui_screen_rect_right;  /* 006ab188 */
    i32 ui_screen_rect_bottom;  /* 006ab18c */
};
#pragma pack(pop)
extern struct caption_small_font_block caption_small_font_block;
#pragma pack(push, 1)
struct caption_font_block {
    GameFont caption_font;  /* 006ab190 */
    int screen_width;  /* 006ab198 */
    int screen_height;  /* 006ab19c */
};
#pragma pack(pop)
extern struct caption_font_block caption_font_block;
extern GameFont ui_text_font[3];
#pragma pack(push, 1)
struct setup_picwnd_block {
    i8 setup_picwnd[72];  /* 006ab1b8 */
    u8 setup_picwnd_wnd[664];  /* 006ab200 */
    int setup_picwnd_image_id;  /* 006ab498 */
    u8 setup_picwnd_image_id_rest[28];
    u8 setup_picwnd_surface[244];  /* 006ab4b8 */
    i32 g_timer_blink[2];  /* 006ab5ac */
    HHOOK mp_window_drag_hook;  /* 006ab5b4 */
    u8 mp_window_drag_hook_rest[8];
    i32 diplo_recent_peace_table[8];  /* 006ab5c0 */
    int diplo_border_intruder_units;  /* 006ab5e0 */
    iptr border_intruder_city;  /* 006ab5e4 */
    int diplo_border_intruder_count;  /* 006ab5e8 */
    int diplo_border_repeat_offenses;  /* 006ab5ec */
    u8 diplo_border_repeat_offenses_rest[16];
    i8 exe_dir_path[128];  /* 006ab600 */
    i8 fallback_drive_root[1];  /* 006ab680 */
    i8 fallback_drive_root_end;  /* 006ab681 */
    u8 fallback_drive_root_end_rest[38];
};
#pragma pack(pop)
extern struct setup_picwnd_block setup_picwnd_block;
extern u8 art_palette[1080];
#pragma pack(push, 1)
struct frame_texture_surface_block {
    u8 frame_texture_surface[312];  /* 006abae0 */
    int status_end_turn_rect;  /* 006abc18 */
    int status_end_turn_rect_top;  /* 006abc1c */
    i32 status_end_turn_rect_right;  /* 006abc20 */
    int status_end_turn_rect_bottom;  /* 006abc24 */
    i32 status_upper_rect;  /* 006abc28 */
    int status_upper_rect_top;  /* 006abc2c */
    i32 status_upper_rect_right;  /* 006abc30 */
    int status_upper_rect_bottom;  /* 006abc34 */
    int status_text_bottom;  /* 006abc38 */
    u8 status_text_bottom_rest[20];
};
#pragma pack(pop)
extern struct frame_texture_surface_block frame_texture_surface_block;
#pragma pack(push, 1)
struct status_font_block {
    GameFont status_font;  /* 006abc50 */
    int status_panel_width;  /* 006abc58 */
    int status_lower_panel_height;  /* 006abc5c */
    i8 status_panel_x;  /* 006abc60 */
    u8 status_panel_x_rest[7];
};
#pragma pack(pop)
extern struct status_font_block status_font_block;
#pragma pack(push, 1)
struct status_window_block {
    DibSurface status_window;  /* 006abc68 */
    WndBase status_window_wnd;  /* 006abcb0 */
    i8 status_wnd_events[204];  /* 006abcc0 */
    i32 status_client_x;  /* 006abd8c */
    i32 status_client_y;  /* 006abd90 */
    int status_client_width;  /* 006abd94 */
    int status_client_height;  /* 006abd98 */
    u8 status_client_height_rest[392];
    i32 status_indicator_rect;  /* 006abf24 */
    u8 status_indicator_rect_rest[4];
    int status_client_right;  /* 006abf2c */
    int status_client_bottom;  /* 006abf30 */
    u8 status_client_bottom_rest[12];
    i8 status_panel_gap;  /* 006abf40 */
    u8 status_panel_gap_rest[3];
    int status_upper_panel_height;  /* 006abf44 */
    DibSurface status_timer_bg_surface;  /* 006abf48 */
    byte status_timer_surface_init;  /* 006abf90 */
    u8 status_timer_surface_init_rest[7];
    GameFont status_info_font[2];  /* 006abf98 */
    int status_lower_rect;  /* 006abfa8 */
    i32 status_lower_rect_top;  /* 006abfac */
    i32 status_lower_rect_right;  /* 006abfb0 */
    int status_lower_rect_bottom;  /* 006abfb4 */
    i32 status_timer_last_rect[1];  /* 006abfb8 */
    int status_timer_last_rect_top;  /* 006abfbc */
    int status_timer_last_rect_right;  /* 006abfc0 */
    int status_timer_last_rect_bottom;  /* 006abfc4 */
    u8 status_timer_last_rect_bottom_rest[200];
    GameFont map_label_font[2];  /* 006ac090 */
    i32 sprite_editor_undo_flag_y;  /* 006ac0a0 */
    i32 sprite_editor_undo_flag_x;  /* 006ac0a4 */
    DibSurface sprite_sheet_surface;  /* 006ac0a8 */
    i32 sprite_editor_flag_y;  /* 006ac0f0 */
    i32 sprite_editor_flag_x;  /* 006ac0f4 */
    u8 sprite_editor_marker_rect[16];  /* 006ac0f8 */
    i8 sprite_editor_plot_rect;  /* 006ac108 */
    u8 sprite_editor_plot_rect_rest[15];
    int sprite_editor_paint_tool;  /* 006ac118 */
    int sprite_picker_dialog;  /* 006ac11c */
    int sprite_editor_tool;  /* 006ac120 */
    i32 sprite_editor_brush_size;  /* 006ac124 */
    DibSurface sprite_editor_surface;  /* 006ac128 */
    u8 sprite_editor_orig_sprite[64];  /* 006ac170 */
    DibSurface sprite_editor_window;  /* 006ac1b0 */
    WndBase sprite_editor_wnd;  /* 006ac1f8 */
    u8 sprite_editor_wnd_events[188];  /* 006ac208 */
    i32 sprite_editor_style_flags_i32;  /* 006ac2c4 */
    int sprite_editor_caption_height;  /* 006ac2c8 */
    int sprite_editor_border_width;  /* 006ac2cc */
    int sprite_editor_resize_border;  /* 006ac2d0 */
    int sprite_editor_origin_x;  /* 006ac2d4 */
    int sprite_editor_origin_y;  /* 006ac2d8 */
    int sprite_editor_client_width;  /* 006ac2dc */
    i32 sprite_editor_client_height_i32;  /* 006ac2e0 */
    i8 sprite_editor_title[200];  /* 006ac2e4 */
    i32 sprite_editor_caption_button_count_i32;  /* 006ac3ac */
    u8 sprite_editor_caption_button_count_i32_rest[168];
    i32 sprite_editor_caption_button_cb_i32;  /* 006ac458 */
    u8 sprite_editor_caption_button_cb_i32_rest[16];
    u8 sprite_editor_fill_rect[16];  /* 006ac46c */
    i32 sprite_editor_title_font_i32;  /* 006ac47c */
    i32 sprite_editor_title_small_font_i32;  /* 006ac480 */
    i32 sprite_editor_text_font_i32;  /* 006ac484 */
    DibSurface sprite_picker_window;  /* 006ac488 */
    u8 sprite_picker_wnd[204];  /* 006ac4d0 */
    i32 sprite_picker_style_flags_i32;  /* 006ac59c */
    int sprite_picker_caption_height;  /* 006ac5a0 */
    int sprite_picker_border_width;  /* 006ac5a4 */
    int sprite_picker_resize_border;  /* 006ac5a8 */
    u8 sprite_picker_resize_border_rest[16];
    i8 sprite_picker_title[200];  /* 006ac5bc */
    i32 sprite_picker_caption_button_count_i32;  /* 006ac684 */
    u8 sprite_picker_caption_button_count_i32_rest[168];
    i32 sprite_picker_caption_button_cb_i32;  /* 006ac730 */
    u8 sprite_picker_caption_button_cb_i32_rest[32];
    i32 sprite_picker_title_font_i32;  /* 006ac754 */
    i32 sprite_picker_title_small_font_i32;  /* 006ac758 */
    i32 sprite_picker_text_font_i32;  /* 006ac75c */
    u8 sprite_picker_text_font_i32_rest[276];
    uint sprite_editor_key_color;  /* 006ac874 */
    int sprite_editor_width;  /* 006ac878 */
    int sprite_editor_height;  /* 006ac87c */
    int sprite_editor_last_px;  /* 006ac880 */
    int sprite_editor_last_py;  /* 006ac884 */
    SpriteSheetSaveFn sprite_editor_ok_callback;  /* 006ac888 */
    i32 sprite_editor_running;  /* 006ac88c */
    i32 sprite_picker_running;  /* 006ac890 */
    int sprite_editor_paint_color;  /* 006ac894 */
    i32 sprite_editor_cursor_id;  /* 006ac898 */
    int sprite_sheet_cell_count;  /* 006ac89c */
    Sprite *sprite_editor_target_sprite;  /* 006ac8a0 */
    i32 sprite_editor_painting;  /* 006ac8a4 */
    u8 sprite_editor_result_sprite[64];  /* 006ac8a8 */
};
#pragma pack(pop)
extern struct status_window_block status_window_block;
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
#pragma pack(push, 1)
struct unit_sound_editor_window_block {
    DibSurface unit_sound_editor_window;  /* 006acd58 */
    WndBase unit_sound_editor_wnd;  /* 006acda0 */
    u8 unit_sound_editor_wnd_handlers[188];  /* 006acdb0 */
    i32 unit_sound_editor_style_flags_i32;  /* 006ace6c */
    int unit_sound_editor_caption_height;  /* 006ace70 */
    int unit_sound_editor_border_width;  /* 006ace74 */
    int unit_sound_editor_resize_border;  /* 006ace78 */
    int unit_sound_editor_client_x;  /* 006ace7c */
    int unit_sound_editor_client_y;  /* 006ace80 */
    int unit_sound_editor_client_width;  /* 006ace84 */
    i32 unit_sound_editor_client_height_i32;  /* 006ace88 */
    i8 unit_sound_editor_title[200];  /* 006ace8c */
    i32 unit_sound_editor_caption_button_count_i32;  /* 006acf54 */
    u8 unit_sound_editor_caption_button_count_i32_rest[168];
    i32 unit_sound_editor_caption_button_cb_i32;  /* 006ad000 */
    u8 unit_sound_editor_caption_button_cb_i32_rest[16];
    u8 unit_sound_editor_fill_rect[16];  /* 006ad014 */
    i32 unit_sound_editor_title_font_i32;  /* 006ad024 */
    i32 unit_sound_editor_title_small_font_i32;  /* 006ad028 */
    i32 unit_sound_editor_text_font_i32;  /* 006ad02c */
    u8 unit_sound_editor_text_font_i32_rest[156];
    byte unit_move_flags;  /* 006ad0cc */
    u8 unit_move_flags_rest[3];
    char unit_move_by_local_human;  /* 006ad0d0 */
    u8 unit_move_by_local_human_rest[11];
    i32 spaceship_fuel_pct;  /* 006ad0dc */
    int spaceship_projected_arrival_year;  /* 006ad0e0 */
    int spaceship_mass;  /* 006ad0e4 */
    i32 spaceship_energy_pct;  /* 006ad0e8 */
    int spaceship_success_pct;  /* 006ad0ec */
    i32 spaceship_support_pct;  /* 006ad0f0 */
    int spaceship_flight_time;  /* 006ad0f4 */
    u8 spaceship_flight_time_rest[16];
};
#pragma pack(pop)
extern struct unit_sound_editor_window_block unit_sound_editor_window_block;
#pragma pack(push, 1)
/* The network session object (the original's global C++ object; its methods
 * take it as `self`). */
typedef struct NetSession {
    NetGamelistView *gamelist_view;  /* 006ad108 */
    LobbyList *net_lobby_list;  /* 006ad10c */
    union {
        struct {
            u8 net_ping_message[16];  /* 006ad110 */
            i32 net_player_info_civ_i32;  /* 006ad120 */
            i32 net_player_info_leader_index_i32;  /* 006ad124 */
            i32 net_player_info_gender_i32;  /* 006ad128 */
            i32 net_player_info_has_names_i32;  /* 006ad12c */
            i8 net_player_info_leader_name[24];  /* 006ad130 */
            i8 net_player_info_tribe_name[24];  /* 006ad148 */
            i8 net_player_info_adjective[24];  /* 006ad160 */
            i32 net_player_info_has_titles_i32;  /* 006ad178 */
            i8 net_player_info_govt_titles[168];  /* 006ad17c */
            int net_player_info_city_style;  /* 006ad224 */
        };
        NetPlayerInfoMsg player_info_msg;  /* 006ad110 the local player's */
    };
    int net_lobby_launch_mode;  /* 006ad228 */
    i8 net_lobby_launch_info[32];  /* 006ad22c */
    u8 startup_log_text[64];  /* 006ad24c */
    i8 net_lobby_short_name[32];  /* 006ad28c */
    i8 net_lobby_session_name[64];  /* 006ad2ac */
    iptr net_lobby_is_host;  /* 006ad2ec */
    int net_connection_type;  /* 006ad2f0 */
    i8 net_connected;  /* 006ad2f4 */
    i8 net_connected_to_server;  /* 006ad2f5 */
    i8 net_listing_games;  /* 006ad2f6: game announcements are collected (net_game_list_add) */
    char net_host_flag;  /* 006ad2f7 */
    i32 net_protocol;  /* 006ad2f8 */
    uint net_adapter_index;  /* 006ad2fc */
    int net_server_conn_id;  /* 006ad300 */
    int net_local_slot;  /* 006ad304 */
    iptr net_player_count[1];  /* 006ad308 */
    union {
        struct {
            i32 net_player_slots[1];  /* 006ad30c */
            i8 net_slot_addresses[32];  /* 006ad310 */
            i8 net_slot_player_names[36];  /* 006ad330 */
            i8 net_slot_lobby_ids[4];  /* 006ad354 */
            i8 net_slot_host_flags[1];  /* 006ad358 */
            i8 net_slot_active_flags[3];  /* 006ad359 */
            i32 net_slot_civs[127];  /* 006ad35c */
        };
        NetPlayerSlot net_slots[7];  /* 006ad30c */
    };
    i32 net_civ_slots[8];  /* 006ad558 */
    i32 mp_turn_civ;  /* 006ad578 */
    i8 net_server_address[32];  /* 006ad57c */
    i8 net_game_name[32];  /* 006ad59c */
    i8 net_local_player_name[32];  /* 006ad5bc */
    i8 net_host_player_name[32];  /* 006ad5dc */
    i8 net_server_version[64];  /* 006ad5fc */
    i32 net_connection_title;  /* 006ad63c */
    i32 net_session_phase;  /* 006ad640 */
    i32 net_civ_new_player_flags[8];  /* 006ad644 */
    int net_players_awaited;  /* 006ad664 */
    int net_players_awaited_initial;  /* 006ad668 */
    int net_vote_replies_pending;  /* 006ad66c */
    int net_vote_status;  /* 006ad670 */
    i32 net_wait_start_ticks_i32;  /* 006ad674 */
    int *dialog_stack_top;  /* 006ad678 */
    i32 net_popup_in_progress_i32;  /* 006ad67c */
    int net_last_send_result;  /* 006ad680 */
    char mp_simultaneous_moves;  /* 006ad684 */
    char player_has_quit;  /* 006ad685 */
    u8 player_has_quit_rest[2];
    int net_poll_depth;  /* 006ad688 */
    int net_poll_depth_peak;  /* 006ad68c */
    u32 net_msgq_normal_peak;  /* 006ad690 */
    u32 net_msgq_priority_peak;  /* 006ad694 */
    char net_host_handoff_stage;  /* 006ad698 */
    i8 net_tile_sync_enabled;  /* 006ad699 */
    char net_tile_batch_active;  /* 006ad69a */
    u8 net_tile_batch_active_rest[1];
    i32 chat_missed_message_count_i32;  /* 006ad69c */
    i32 chat_unread_count;  /* 006ad6a0 */
    i32 net_move_anim_sound;  /* 006ad6a4 */
    i32 net_move_anim_sound_arg;  /* 006ad6a8 */
    i16 loaded_file_kind;  /* 006ad6ac */
    i8 loaded_file_name[260];  /* 006ad6ae */
    i8 loaded_file_subdir[262];  /* 006ad7b2 */
    int net_timeout_seconds;  /* 006ad8b8 */
    int unit_create_busy;  /* 006ad8bc */
    int unit_delete_busy;  /* 006ad8c0 */
    int city_create_busy;  /* 006ad8c4 */
    int city_delete_busy;  /* 006ad8c8 */
    int net_lock_map_busy;  /* 006ad8cc */
    int net_unlock_map_busy;  /* 006ad8d0 */
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
    uint net_stacked_draw_count;  /* 006ad90c */
    int net_stacked_draw_first;  /* 006ad910 */
    int net_stacked_draw_last;  /* 006ad914 */
    int unit_stack_check_enabled;  /* 006ad918 */
    uint net_stacked_draw_peak;  /* 006ad91c */
    union {
        struct {
            i8 net_stacked_draw_kinds[4];  /* 006ad920 */
            i8 net_stacked_draw_args[4];  /* 006ad924 */
            i8 net_stacked_draw_arg1[4];  /* 006ad928 */
            i8 net_stacked_draw_arg2[4];  /* 006ad92c */
            i8 net_stacked_draw_arg3[4];  /* 006ad930 */
            i8 net_stacked_draw_arg4[6380];  /* 006ad934 */
        };
        NetStackedDraw net_stacked_draw[100];  /* 006ad920 */
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
    i32 net_civ_turn_ended[8];  /* 006c3168 */
    i32 net_civ_drop_requested[8];  /* 006c3188 */
    byte pending_human_players;  /* 006c31a8 */
    u8 hotseat_human_players;  /* 006c31a9 */
    u8 hotseat_human_players_rest[2];
    i32 blocked_input_kind;  /* 006c31ac */
    i32 blocked_input_arg1_i32;  /* 006c31b0 */
    i32 blocked_input_arg2_i32;  /* 006c31b4 */
    i32 blocked_input_arg3_i32;  /* 006c31b8 */
    i32 blocked_input_arg4_i32;  /* 006c31bc */
    i32 blocked_input_arg5_i32;  /* 006c31c0 */
    i32 blocked_input_char_pending;  /* 006c31c4 */
    char blocked_input_char;  /* 006c31c8 */
    u8 blocked_input_char_rest[3];
    i32 blocked_input_window_i32;  /* 006c31cc */
    i8 net_drag_hold;  /* 006c31d0 */
    u8 net_drag_hold_rest[3];
    NetGameInfo *net_found_games;  /* 006c31d4 */
    union {
        struct {
            i8 net_msg_queue[24008];  /* 006c31d8 */
            int net_msgq_normal_count;  /* 006c8fa0 */
            u8 net_msgq_normal_count_rest[8];
            int net_msgq_priority_count;  /* 006c8fac */
        };
        NetMsgQueue msg_queue;  /* 006c31d8 */
    };
    ulong net_last_recv_callback_size;  /* 006c8fb0 */
    int net_lost_conn_total;  /* 006c8fb4 */
    ulong net_last_recv_msg_size;  /* 006c8fb8 */
    uint net_xd_adapter_index;  /* 006c8fbc */
    i32 net_lost_conn_by_player[8];  /* 006c8fc0 */
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
            i32 nm_server_quit_count_i32;  /* 006c901c */
            u8 nm_server_quit_count_i32_rest[4];
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
            i32 nm_heart_beat_count_i32;  /* 006c908c */
            int nm_heartbeat_ack_count;  /* 006c9090 */
            u8 nm_heartbeat_ack_count_rest[12];
            i32 nm_lock_nation_count_i32;  /* 006c90a0 */
            u8 nm_lock_nation_count_i32_rest[16];
            int net_kill_or_quit_ack;  /* 006c90b4 */
            u8 net_kill_or_quit_ack_rest[8];
            i32 net_unit_deleted_ack;  /* 006c90c0 */
            u8 net_unit_deleted_ack_rest[4];
            int net_city_deleted_ack;  /* 006c90c8 */
            u8 net_city_deleted_ack_rest[4];
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
    int net_last_queued_send_seq;  /* 006c9284 */
    i32 net_active_protocol;  /* 006c9288 */
    u8 net_active_protocol_rest[23020];
} NetSession;
#pragma pack(pop)
extern NetSession net_session_block;
#pragma pack(push, 1)
struct dialog_title_font_obj_block {
    GameFont dialog_title_font_obj;  /* 006cec78 */
    i32 dialog_timeout_start_i32;  /* 006cec80 */
    Dialog *active_dialog;  /* 006cec84 */
};
#pragma pack(pop)
extern struct dialog_title_font_obj_block dialog_title_font_obj_block;
extern GameFont dialog_list_font_obj;
#pragma pack(push, 1)
struct dialog_plain_font_obj_block {
    GameFont dialog_plain_font_obj;  /* 006cec90 */
    i32 dialog_button_label_ids[4];  /* 006cec98 */
};
#pragma pack(pop)
extern struct dialog_plain_font_obj_block dialog_plain_font_obj_block;
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
extern void *map_tiles_handle;
extern void *map_coarse_handles[4];
extern u8 map_coarse_handles_rest[4];
extern Tile map_offmap_tile;
extern u8 map_offmap_tile_rest[2];
extern i32 net_batch_buffer[772];
extern iptr human_civ[2];
extern int moving_units_mode;
extern u8 moving_units_mode_rest[12];
#pragma pack(push, 1)
struct ms_window_stack_block {
    union {
        struct {
            i8 ms_window_stack[4];  /* 006d1db8 */
            i8 ms_window_stack_elem1[268];  /* 006d1dbc */
        };
        i32 ms_window_stack_entries[68];  /* 006d1db8 */
    };
};
#pragma pack(pop)
extern struct ms_window_stack_block ms_window_stack_block;
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
    i8 *gfx_scale_x_table;  /* 006e47c0 */
    i8 *gfx_scale_y_table;  /* 006e47c4 */
    i32 *gfx_scale_table;  /* 006e47c8 */
    u8 gfx_scale_table_rest[4];
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
    TimerDllGetTimerIdFn timerdll_get_timer_id;  /* 006e47fc */
    TimerDllGetTimerIndexFn timerdll_get_timer_index;  /* 006e4800 */
    HMODULE timerdll_module;  /* 006e4804 */
    LPTIMECALLBACK timerdll_timer_callback;  /* 006e4808 */
    TimerDllResetNotifiedFn timerdll_reset_timer_notified;  /* 006e480c */
    TimerDllSetTimerIdFn timerdll_set_timer_id;  /* 006e4810 */
    u8 timerdll_set_timer_id_rest[4];
    i8 snd_wave_dev_caps;  /* 006e4818 */
    u8 snd_wave_dev_caps_rest[39];
    byte snd_wave_dev_formats;  /* 006e4840 */
    u8 snd_wave_dev_formats_rest[15];
};
#pragma pack(pop)
extern struct default_draw_font_obj_block default_draw_font_obj_block;
#pragma pack(push, 1)
struct err_report_ctx_block {
    u8 err_report_ctx[1808];  /* 006e4850 */
    u32 res_dll_modules[35];  /* 006e4f60 HMODULE */
    i32 app_prev_instance;  /* 006e4fec */
    HINSTANCE app_hinstance;  /* 006e4ff0 */
    u8 app_hinstance_rest[4];
    HWND snd_mci_notify_hwnd;  /* 006e4ff8 */
    u8 snd_mci_notify_hwnd_rest[4];
    MusicNotifyFn snd_cd_notify_callback;  /* 006e5000 */
    MusicNotifyFn snd_midi_notify_callback;  /* 006e5004 */
    u8 snd_midi_notify_callback_rest[4];
    int palette_static_half_count;  /* 006e500c */
    u8 palette_static_half_count_rest[8];
    i32 choice_dlg_selection;  /* 006e5018 */
    u8 choice_dlg_selection_rest[4];
    int choice_dlg_window;  /* 006e5020 */
    u8 choice_dlg_window_rest[4];
    i32 avi_indeo_frame_opts[1];  /* 006e5028 */
    i32 avi_indeo_frame_fourcc_i32;  /* 006e502c */
    i32 avi_indeo_frame_version_i32;  /* 006e5030 */
    i32 avi_indeo_frame_type_i32;  /* 006e5034 */
    i32 avi_indeo_frame_environment_i32;  /* 006e5038 */
    i32 g_video_flags_b;  /* 006e503c */
    u8 g_video_flags_b_rest[36];
    i32 avi_indeo_view_rect_ptr_i32;  /* 006e5064 */
    u8 avi_indeo_view_rect_ptr_i32_rest[48];
    i32 avi_indeo_seq_opts[1];  /* 006e5098 */
    i32 avi_indeo_seq_fourcc_i32;  /* 006e509c */
    i32 avi_indeo_seq_version_i32;  /* 006e50a0 */
    i32 avi_indeo_seq_type_i32;  /* 006e50a4 */
    i32 avi_indeo_seq_environment_i32;  /* 006e50a8 */
    i32 g_video_flags_a;  /* 006e50ac */
    u8 g_video_flags_a_rest[12];
    i32 avi_indeo_seq_extra_i32;  /* 006e50bc */
    u8 avi_indeo_seq_extra_i32_rest[4];
    int avi_vfw_initialized;  /* 006e50c4 */
    i32 avi_view_rect_x[1];  /* 006e50c8 */
    i32 avi_view_rect_y;  /* 006e50cc */
    i32 avi_view_rect_width_i32;  /* 006e50d0 */
    i32 avi_view_rect_height_i32;  /* 006e50d4 */
    i32 avi_frames_per_second_i32;  /* 006e50d8 */
    i32 avi_indeo_frame_extra_i32;  /* 006e50dc */
    u8 avi_indeo_frame_extra_i32_rest[8];
    DWORD g_ddcaps_driver;  /* 006e50e8 */
    byte ddraw_hw_caps_byte0;  /* 006e50ec */
    u8 ddraw_hw_caps_byte0_rest[1];
    byte ddraw_hw_caps_byte2;  /* 006e50ee */
    byte ddraw_hw_caps_byte3;  /* 006e50ef */
    u8 ddraw_hw_caps_byte3_rest[312];
    DWORD g_ddcaps_hel;  /* 006e5228 */
    u8 g_ddcaps_hel_rest[316];
    i32 menu_search_pos;  /* 006e5368 */
    u8 menu_search_pos_rest[6144];
};
#pragma pack(pop)
extern struct err_report_ctx_block err_report_ctx_block;

/* Sets the addresses the data holds (functions, other data). */
EXTERN_C void gamedata_init(void);

#endif
