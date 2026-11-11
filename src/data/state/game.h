/* The game: its settings and the scenario, the turn, setup, saving and loading, events.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_GAME_H
#define CIV2_STATE_GAME_H

/* The world-generation choices (new game setup; the generator reads them). */
#pragma pack(push, 1)
typedef struct MapGenChoices {
    int land_mass;  /* 00624ee8 */
    int land_form;  /* 00624eec */
    int temperature;  /* 00624ef0 */
    int climate;  /* 00624ef4 */
    int age;  /* 00624ef8 */
} MapGenChoices;
#pragma pack(pop)
extern MapGenChoices mapgen;
extern int game_session_active;
extern int simul_tutorial_shown;  /* only game_main uses it */
extern RODATA i8 startup_game_txt_name_str[8];
extern RODATA i8 startup_lang_default_ext_str[4];
extern RODATA i8 startup_lang_english_ext_str[4];
extern RODATA i8 startup_lang_french_ext_str[4];
extern RODATA i8 startup_lang_german_ext_str[4];
extern RODATA i8 startup_lang_italian_ext_str[4];
extern RODATA i8 startup_lang_spanish_ext_str[4];
extern RODATA i8 ini_herald_warning_value_str[4];
extern RODATA i8 menu_game_txt_name_str[8];
extern RODATA i8 xdaemon_req_ver_dot1_str[4];
extern RODATA i8 xdaemon_req_ver_dot2_str[4];
extern RODATA i8 xdaemon_got_ver_dot1_str[4];
extern RODATA i8 xdaemon_got_ver_dot2_str[4];
extern i8 ini_simultaneous_reset_str[4];  /* only game_main uses it */
extern i8 ini_max_players_reset_str[4];  /* only game_main uses it */
extern RODATA i8 game_profile_double_opts_sep_str[4];
extern RODATA i8 game_profile_timer_colon_str[4];
extern RODATA i8 net_title_ip_sep_str[4];
extern RODATA i8 pedia_desc_marker_str[4];
extern RODATA i8 pedia_desc_crlf_str[4];
extern int registry_language_id;
extern i8 registry_key_class_str[28];  /* only registry_write_value uses it */
/* The networking library version the game requires. */
#pragma pack(push, 1)
typedef struct XDaemonVersion {
    int major;  /* 00627664 */
    int minor;  /* 00627668 */
    int build;  /* 0062766c */
} XDaemonVersion;
#pragma pack(pop)
extern XDaemonVersion xdaemon_required;
extern int scenario_events_loaded;
/* Ids given to the next unit and city created. */
#pragma pack(push, 1)
typedef struct NextIds {
    i32 unit;  /* 00627fd8 */
    i32 city;  /* 00627fdc */
} NextIds;
#pragma pack(pop)
extern NextIds next_ids;
/* The main turn loop: running, and the civ to resume with. */
#pragma pack(push, 1)
typedef struct TurnLoop {
    i8 active;  /* 00628044 */
    u8 active_rest[3];
    char resume_civ;  /* 00628048 */
    u8 resume_civ_rest[3];
} TurnLoop;
#pragma pack(pop)
extern TurnLoop turn_loop;
/* The blinking of the active unit and the status bar's End of Turn. */
#pragma pack(push, 1)
typedef struct BlinkState {
    i8 unit_active;  /* 0062804c */
    u8 unit_active_rest[7];
    i8 unit_phase;  /* 00628054 */
    u8 unit_phase_rest[3];
    char end_turn_phase;  /* 00628058 */
    u8 end_turn_phase_rest[3];
    i8 unit_skip_tick;  /* 0062805c */
    u8 unit_skip_tick_rest[3];
} BlinkState;
#pragma pack(pop)
extern BlinkState blink;
/* The opening: theme music and video. */
#pragma pack(push, 1)
typedef struct IntroState {
    i32 theme_started;  /* 0062af10 */
    struct AviPlayer *video_player;  /* 0062af14 */
} IntroState;
#pragma pack(pop)
extern IntroState intro;
extern u8 str_empty_82[4];  /* only intro_play_opening_video uses it */
extern u8 str_empty_83[4];  /* only intro_play_opening_video uses it */
extern RODATA i8 path_default_ext_dot_str[4];
extern RODATA i8 path_set_ext_dot_str[28];
extern i8 *str_ptr_civilize;
extern RODATA i8 save_ext_scn_str[8];
extern RODATA i8 save_ext_sav_str[8];
extern RODATA i8 save_ext_hot_str[8];
extern RODATA i8 save_ext_eml_str[8];
extern RODATA i8 save_ext_net_str[8];
extern RODATA i8 save_filter_mp_pattern_str[8];
extern RODATA i8 save_write_mode[4];
extern RODATA i8 save_events_tag_str[12];
extern RODATA i8 save_read_mode[4];
extern RODATA i8 game_txt_default_name_str[8];
extern RODATA i8 save_events_tag_check_str[8];
extern RODATA i8 save_name_date_sep_str[4];
extern RODATA i8 save_name_month_day_sep_str[4];
extern RODATA i8 save_name_day_year_sep_str[4];
extern RODATA i8 save_name_time_sep_str[4];
extern RODATA i8 save_name_hour_min_sep_str[4];
extern RODATA i8 save_name_min_sec_sep_str[4];
extern RODATA i8 save_name_ext_dot_str[4];
extern RODATA i8 load_dialog_wildcard_str[4];
extern RODATA i8 game_load_scn_ext_str[8];
extern i32 net_meet_pending_flags[8];
extern int turn_resume_new_game;
extern u8 turn_resume_new_game_rest[4];
/* Flags of the turn being played. */
#pragma pack(push, 1)
typedef struct TurnFlags {
    i32 app_shutdown_done;  /* 0062c5b0 */
    iptr spaceship_winner_civ;  /* 0062c5b4 */
    int civ_turn_in_progress;  /* 0062c5b8 */
    int active_unit_lost_in_combat;  /* 0062c5bc */
    int tutorial_hint_shown_this_turn;  /* 0062c5c0 */
} TurnFlags;
#pragma pack(pop)
#pragma pack(push, 1)
struct year_table_start_years_block {
    i8 year_table_start_years[4];  /* 0062c490 */
    i8 year_table_turn_counts[4];  /* 0062c494 */
    i8 year_table_year_steps[52];  /* 0062c498 */
    i8 year_table_last_start_year[4];  /* 0062c4cc */
    i8 year_table_last_turn_count[4];  /* 0062c4d0 */
    i8 year_table_last_year_step[220];  /* 0062c4d4 */
    TurnFlags turn_flags;  /* 0062c5b0 */
};
#pragma pack(pop)
extern struct year_table_start_years_block year_table_start_years_block;
inline constexpr TurnFlags &turn_flags = year_table_start_years_block.turn_flags;
extern YearPeriod *const year_period_table;  /* = year_table_start_years_block: [table * 6 + period], 4 tables of 6 periods (the year loop reads period 6) */
extern i8 lang_file_ext[4];
extern i8 *str_ptr_civ2_dat;
extern u8 str_ptr_civ2_dat_rest[4];
extern i8 prefs_defaults_empty_str[4];  /* only prefs_set_defaults uses it */
extern RODATA i8 prefs_turn_timer_default_str[4];
extern RODATA i8 civ2dat_read_mode_str[4];
extern RODATA i8 civ2dat_write_mode_str[4];
extern int events_parse_verbose;
extern u8 events_parse_verbose_rest[48];
extern RODATA i8 events_kw_if_str[4];
extern RODATA i8 events_kw_then_str[8];
extern RODATA i8 events_kw_turn_str[8];
extern RODATA i8 events_kw_text_str[8];
extern RODATA i8 events_bool_yes_str[4];
extern RODATA i8 events_bool_true_str[8];
extern RODATA i8 events_bool_no_str[4];
extern RODATA i8 events_who_prefix_str[8];
extern RODATA i8 events_numbertomove_all_str[4];
/* The dialog being run: its checkboxes' bits, and whether Help was pressed. */
#pragma pack(push, 1)
typedef struct DialogInput {
    uint check_bits;  /* 00631ed8 */
    int help_pressed;  /* 00631edc */
    u8 help_pressed_rest[4];
} DialogInput;
#pragma pack(pop)
extern DialogInput dlg_input;
/* The new-game setup dialogs. */
#pragma pack(push, 1)
typedef struct SetupDialogState {
    i32 select_opponents;  /* 00631ee4 */
    int accelerated_start;  /* 00631ee8 */
    i32 dialog_pos_state;  /* 00631eec */
    int side_picture_id;  /* 00631ef0 */
} SetupDialogState;
#pragma pack(pop)
extern SetupDialogState setup_dlg;
extern RODATA i8 net_ini_timeout_default_str[4];
extern RODATA i8 net_ini_adapter_default_str[4];
extern i8 net_ini_maxplayers_clear_str[4];  /* only setup_choose_multiplayer_type uses it */
extern RODATA i8 setup_timer_minsec_sep_str[4];
extern i8 setup_input_blank_default[4];  /* only setup_tribe_city_style uses it */
extern iptr events_editor_backup;
extern RODATA i8 events_text_line_newline_str[4];
extern RODATA i8 events_moveunit_rect_sep_str[4];
extern RODATA i8 events_veteran_yes_no_strs[8];
extern RODATA i8 events_wave_file_fmt[4];
extern RODATA i8 events_cd_track_fmt[4];
extern RODATA i8 events_terrain_rect_sep_str[4];
extern RODATA i8 events_terrain_rect_end_str[4];
extern RODATA i8 events_list_end_str[4];
extern RODATA i8 events_editor_text_caption[8];
extern RODATA i8 events_editor_yes_no_strs[8];
extern i8 events_city_default_str;  /* only events_editor_edit_trigger uses it */
extern u8 events_city_default_str_rest[3];
extern i8 events_homecity_default_str[4];  /* only edit_action_create_unit uses it */
extern i8 events_wavfile_default_str[4];  /* only edit_action_play_wave uses it */
extern RODATA i8 events_wav_ext_str[8];
extern RODATA i8 events_pool_free_fmt[688];
extern i32 cheat_terrain_saved_bits;
/* The cheat menu's Create Unit choices. */
#pragma pack(push, 1)
typedef struct CheatUnitChoices {
    int last_human_civ;  /* 0063367c */
    i8 owner_civ;  /* 00633680 */
    u8 owner_civ_rest[3];
    int last_type;  /* 00633684 */
    char veteran;  /* 00633688 */
    u8 veteran_rest[3];
    char show_obsolete;  /* 0063368c */
    u8 show_obsolete_rest[3];
    char ignore_prereq;  /* 00633690 */
    u8 ignore_prereq_rest[3];
} CheatUnitChoices;
#pragma pack(pop)
extern CheatUnitChoices cheat_unit;
/* The status window's fonts, End of Turn and turn timer. */
#pragma pack(push, 1)
typedef struct StatusBarState {
    uint info_font_scale;  /* 00633df4 */
    int font_scale;  /* 00633df8 */
    char end_turn_visible;  /* 00633dfc */
    u8 end_turn_visible_rest[3];
    i8 wnd_hidden;  /* 00633e00 */
    u8 wnd_hidden_rest[3];
    int timer_last_width;  /* 00633e04 */
    int timer_last_color;  /* 00633e08 */
    int timer_last_percent;  /* 00633e0c */
} StatusBarState;
#pragma pack(pop)
extern StatusBarState status_bar;
extern RODATA i8 status_tax_sep_str[4];
extern RODATA i8 status_lux_sep_str[4];
extern RODATA i8 status_moves_frac_sep_str[4];
/* The button under the mouse and the one pressed. */
#pragma pack(push, 1)
typedef struct ButtonMouse {
    int hover;  /* 00637c9c */
    int pressed;  /* 00637ca0 */
} ButtonMouse;
#pragma pack(pop)
extern ButtonMouse button_mouse;
extern int timer_mm_available;
extern void **g_ddraw;
extern i8 *str_ptr_no_error[45];
extern iptr crt_errno;
extern u8 crt_errno_rest[386];
/* The world map window. */
#pragma pack(push, 1)
typedef struct MinimapWindowState {
    DibSurface window;  /* 0063c818 */
    WndBase wnd;  /* 0063c860 */
    i8 wnd_handlers[204];  /* 0063c870 */
    int client_x;  /* 0063c93c */
    int client_y;  /* 0063c940 */
    int client_width;  /* 0063c944 */
    int client_height;  /* 0063c948 */
    u8 client_height_rest[420];
    i32 margin_y;  /* 0063caf0 */
    int margin_x;  /* 0063caf4 */
    u8 margin_x_rest[156];
} MinimapWindowState;
#pragma pack(pop)
/* The world generator's working bounds. */
#pragma pack(push, 1)
typedef struct MapGenBounds {
    int max_y;  /* 0063cb94 */
    int max_x;  /* 0063cb98 */
    i32 min_x;  /* 0063cb9c */
    i32 min_y;  /* 0063cba0 */
    int land_placed;  /* 0063cba4 */
    u8 land_placed_rest[8];
} MapGenBounds;
#pragma pack(pop)
/* The tax rate dialog open. */
#pragma pack(push, 1)
typedef struct TaxRateWindow {
    int active;  /* 0063cbb0 */
    TaxRateDialog *dlg;  /* 0063cbb4 */
    u8 dlg_rest[8];
} TaxRateWindow;
#pragma pack(pop)
/* The timers blinking the End of Turn and the active unit. */
#pragma pack(push, 1)
typedef struct BlinkTimers {
    i32 end_of_turn;  /* 0063cbc0 */
    i32 unit;  /* 0063cbc4 */
    u8 unit_rest[8];
} BlinkTimers;
#pragma pack(pop)
/* The arguments substituted into game texts (%NUMBER0.., %STRING0..; some named for the game profile that fills them). */
#pragma pack(push, 1)
typedef struct TextArgs {
    i32 numbers[6];  /* 0063cc30 */
    i8 strings[1];  /* 0063cc48 */
    i8 strings_plus1;  /* 0063cc49 */
    u8 strings_plus1_rest[258];
    i8 string1[260];  /* 0063cd4c */
    i8 string2[260];  /* 0063ce50 */
    i8 string3[260];  /* 0063cf54 */
    u8 profile_difficulty[260];  /* 0063d058 */
    i8 string5[260];  /* 0063d15c */
    u8 profile_barbarians[260];  /* 0063d260 */
    i8 profile_timer[260];  /* 0063d364 */
    u8 profile_count[260];  /* 0063d468 */
    u8 profile_count2[260];  /* 0063d56c */
    u8 profile_map_width[260];  /* 0063d670 */
    u8 profile_map_height[3396];  /* 0063d774 */
} TextArgs;
#pragma pack(pop)
#pragma pack(push, 1)
struct minimap_window_block {
    MinimapWindowState minimap_state;  /* 0063c818 */
    MapGenBounds mapgen_bounds;  /* 0063cb94 */
    TaxRateWindow taxrate_wnd;  /* 0063cbb0 */
    BlinkTimers blink_timers;  /* 0063cbc0 */
    u8 setup_backdrop_surface[96];  /* 0063cbd0 */
    TextArgs text_args;  /* 0063cc30 */
    i32 strpool_last_string;  /* 0063e4b8 */
    u8 strpool_last_string_rest[4];
};
#pragma pack(pop)
extern struct minimap_window_block minimap_window_block;
inline constexpr TextArgs &text_args = minimap_window_block.text_args;
inline constexpr BlinkTimers &blink_timers = minimap_window_block.blink_timers;
inline constexpr TaxRateWindow &taxrate_wnd = minimap_window_block.taxrate_wnd;
inline constexpr MapGenBounds &mapgen_bounds = minimap_window_block.mapgen_bounds;
inline constexpr MinimapWindowState &minimap_state = minimap_window_block.minimap_state;
extern DibSurface panel_texture_surface;
extern int game_over_reason;
/* The map cursor's tile (the active unit's or the view's). */
#pragma pack(push, 1)
typedef struct MapCursor {
    short y[2];  /* 0064b1b0 */
    short x[2];  /* 0064b1b4 */
} MapCursor;
#pragma pack(pop)
extern MapCursor map_cursor;
extern i8 player_turn_active;
extern u8 player_turn_active_rest[3];
/* The player's game settings, as saved to CIV.INI (the new-game dialogs' choices). */
#pragma pack(push, 1)
typedef struct GamePrefs {
    ushort game_rules[1];  /* 0064bc10 */
    i16 default_level;  /* 0064bc12 */
    short difficulty;  /* 0064bc14 */
    i16 tribe;  /* 0064bc16 */
    short gender;  /* 0064bc18 */
    i16 science_rate;  /* 0064bc1a */
    i16 tax_rate;  /* 0064bc1c */
    uint game_options;  /* 0064bc1e */
    u16 message_options;  /* 0064bc22 */
    short enemies_choice;  /* 0064bc24 */
    short map_size;  /* 0064bc26 */
    short barbarity;  /* 0064bc28 */
    short world_params[1];  /* 0064bc2a */
    short world_land_form;  /* 0064bc2c */
    short world_temperature;  /* 0064bc2e */
    short world_climate;  /* 0064bc30 */
    short world_age;  /* 0064bc32 */
    i8 text_field[32];  /* 0064bc34 */
    short rules_choice;  /* 0064bc54 */
    short accelerated_start;  /* 0064bc56 */
    u8 accelerated_start_rest[8];
} GamePrefs;
#pragma pack(pop)
/* The scenario's parameters (saved in .SCN files): name, years, objectives. */
#pragma pack(push, 1)
typedef struct ScenarioHeader {
    ushort flags[1];  /* 0064bc60 */
    i8 name[80];  /* 0064bc62 */
    short tech_paradigm;  /* 0064bcb2 */
    short year_increment[1];  /* 0064bcb4 */
    i16 start_year;  /* 0064bcb6 */
    short max_turns;  /* 0064bcb8 */
    short objective_civ;  /* 0064bcba */
    i16 victory_thresholds[1];  /* 0064bcbc */
    short marginal_victory_score;  /* 0064bcbe */
    short marginal_defeat_score;  /* 0064bcc0 */
    short decisive_defeat_score;  /* 0064bcc2 */
    u8 decisive_defeat_score_rest[4];
} ScenarioHeader;
#pragma pack(pop)
#pragma pack(push, 1)
struct prefs_game_rules_block {
    GamePrefs prefs;  /* 0064bc10 */
    ScenarioHeader scenario;  /* 0064bc60 */
};
#pragma pack(pop)
extern struct prefs_game_rules_block prefs_game_rules_block;
inline constexpr ScenarioHeader &scenario = prefs_game_rules_block.scenario;
inline constexpr GamePrefs &prefs = prefs_game_rules_block.prefs;
extern CivNames civ_names[8];
/* The multiplayer game's options (saved with the game). */
#pragma pack(push, 1)
typedef struct MpGameOptions {
    short open_game;  /* 00654c74 */
    short kill_civ_on_retire;  /* 00654c76 */
    short show_human_starts;  /* 00654c78 */
    short chat_from_start;  /* 00654c7a */
    short simultaneous_setting;  /* 00654c7c */
} MpGameOptions;
#pragma pack(pop)
/* Per civilization: where its map view was centred, whether its introduction was shown (hot seat). */
#pragma pack(push, 1)
typedef struct CivViewState {
    i16 saved_cursor_x[8];  /* 00654b40 */
    i16 saved_cursor_y[8];  /* 00654b50 */
    i16 intro_shown[8];  /* 00654b60 */
} CivViewState;
#pragma pack(pop)
/* Autoplay (the game playing itself for some turns). */
#pragma pack(push, 1)
typedef struct AutoplayState {
    short active;  /* 00654fa8 */
    short turns_left;  /* 00654faa */
} AutoplayState;
#pragma pack(pop)
/* The turn to resume (saved with the game). */
#pragma pack(push, 1)
typedef struct SavedTurn {
    i8 civ;  /* 00654fa4 */
    u8 civ_rest[1];
    i16 resume_new_game;  /* 00654fa6 */
} SavedTurn;
#pragma pack(pop)
/* The multiplayer options doubling production and moves. */
#pragma pack(push, 1)
typedef struct MpDoubles {
    short production;  /* 00654fac */
    short moves;  /* 00654fae */
} MpDoubles;
#pragma pack(pop)
/* The game's directory. */
#pragma pack(push, 1)
typedef struct GameDirs {
    i8 home[264];  /* 00655020 */
} GameDirs;
#pragma pack(pop)
#pragma pack(push, 1)
struct civ_saved_cursor_x_block {
    CivViewState civ_view;  /* 00654b40 */
    int mp_turn_time_limit_ms;  /* 00654b70 */
    i8 civ_passwords[255];  /* 00654b74 */
    char civ_passwords_last_byte;  /* 00654c73 */
    MpGameOptions mp_options;  /* 00654c74 */
    i32 cheat_saved_turn_time_ms;  /* 00654c7e */
    u8 cheat_saved_turn_time_ms_rest[290];
    i8 civ_email_addresses[256];  /* 00654da4 */
    i8 net_player_setup_table[256];  /* 00654ea4 */
    SavedTurn saved_turn;  /* 00654fa4 */
    AutoplayState autoplay;  /* 00654fa8 */
    MpDoubles mp_doubles;  /* 00654fac */
    i16 net_claimed_civs_mask;  /* 00654fb0 */
    u8 net_claimed_civs_mask_rest[34];
    i8 secondary_menu_handle[4];  /* 00654fd4 */
    i32 game_setup_complete;  /* 00654fd8 */
    u8 game_setup_complete_rest[4];
    GovTitle government_titles[7];  /* 00654fe0 */
    u8 government_titles_rest[8];
    GameDirs game_dirs;  /* 00655020 */
};
#pragma pack(pop)
extern struct civ_saved_cursor_x_block civ_saved_cursor_x_block;
inline constexpr GameDirs &game_dirs = civ_saved_cursor_x_block.game_dirs;
inline constexpr MpDoubles &mp_doubles = civ_saved_cursor_x_block.mp_doubles;
inline constexpr SavedTurn &saved_turn = civ_saved_cursor_x_block.saved_turn;
inline constexpr AutoplayState &autoplay = civ_saved_cursor_x_block.autoplay;
inline constexpr CivViewState &civ_view = civ_saved_cursor_x_block.civ_view;
inline constexpr MpGameOptions &mp_options = civ_saved_cursor_x_block.mp_options;
extern GovTitle *const government_titles;  /* = civ_saved_cursor_x_block.government_titles */
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
/* The main menu's and the multiplayer setup's remembered choices (CIV.INI). */
#pragma pack(push, 1)
typedef struct MenuPrefs {
    i16 main_menu_choice[1];  /* 00666538 */
    i16 multiplayer_type;  /* 0066653a */
    short hotseat_menu_choice;  /* 0066653c */
    short lan_menu_choice;  /* 0066653e */
    i16 mp_prefs_setting_1;  /* 00666540 */
    i16 pbem_menu_choice;  /* 00666542 */
    ushort net_protocol;  /* 00666544 */
    short mp_open_game;  /* 00666546 */
    short mp_kill_on_retire;  /* 00666548 */
    short mp_show_human_starts;  /* 0066654a */
    short mp_chat_from_start;  /* 0066654c */
    short mp_simultaneous_moves;  /* 0066654e */
    i8 net_game_name[32];  /* 00666550 */
    i8 net_player_name[32];  /* 00666570 */
    i8 email_address[32];  /* 00666590 */
    i8 net_ip_address[32];  /* 006665b0 */
    i16 turn_timer_choice;  /* 006665d0 */
    i8 turn_timer_custom[6];  /* 006665d2 */
    i16 hotseat_players_choice;  /* 006665d8 */
    i8 parley_wnd_rect[16];  /* 006665da */
    i8 chat_wnd_rect[16];  /* 006665ea */
    i16 clear_chat_on_new_game;  /* 006665fa */
    i16 clear_chat_on_boot;  /* 006665fc */
    short mp_double_production;  /* 006665fe */
    short mp_double_moves;  /* 00666600 */
    i8 mp_prefs_setting_2;  /* 00666602 */
    u8 mp_prefs_setting_2_rest[23];
    i8 mp_prefs_setting_3;  /* 0066661a */
    u8 mp_prefs_setting_3_rest[23];
    i8 mp_prefs_setting_4;  /* 00666632 */
    u8 mp_prefs_setting_4_rest[23];
    i8 mp_prefs_settings_table[168];  /* 0066664a */
    i16 mp_prefs_setting_5;  /* 006666f2 */
    u8 mp_prefs_setting_5_rest[22404];
} MenuPrefs;
#pragma pack(pop)
#pragma pack(push, 1)
struct prefs_main_menu_choice_block {
    MenuPrefs menu_prefs;  /* 00666538 */
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
inline constexpr MenuPrefs &menu_prefs = prefs_main_menu_choice_block.menu_prefs;
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
struct caption_font_block {
    GameFont caption_font;  /* 006ab190 */
    int screen_width;  /* 006ab198 */
    int screen_height;  /* 006ab19c */
};
#pragma pack(pop)
extern struct caption_font_block caption_font_block;
/* The picture beside the new-game setup dialogs. */
#pragma pack(push, 1)
typedef struct SetupPictureWindow {
    i8 window[72];  /* 006ab1b8 */
    u8 wnd[664];  /* 006ab200 */
    int image_id;  /* 006ab498 */
    u8 image_id_rest[28];
    u8 surface[244];  /* 006ab4b8 */
} SetupPictureWindow;
#pragma pack(pop)
/* Units of other civilizations inside a civ's borders (AI diplomacy). */
#pragma pack(push, 1)
typedef struct BorderIntruders {
    int intruder_units;  /* 006ab5e0 */
    iptr intruder_city;  /* 006ab5e4 */
    int intruder_count;  /* 006ab5e8 */
    int repeat_offenses;  /* 006ab5ec */
    u8 repeat_offenses_rest[16];
} BorderIntruders;
#pragma pack(pop)
#pragma pack(push, 1)
struct setup_picwnd_block {
    SetupPictureWindow setup_picwnd_state;  /* 006ab1b8 */
    i32 g_timer_blink[2];  /* 006ab5ac */
    HHOOK mp_window_drag_hook;  /* 006ab5b4 */
    u8 mp_window_drag_hook_rest[8];
    i32 diplo_recent_peace_table[8];  /* 006ab5c0 */
    BorderIntruders border_intruders;  /* 006ab5e0 */
    i8 exe_dir_path[128];  /* 006ab600 */
    i8 fallback_drive_root[1];  /* 006ab680 */
    i8 fallback_drive_root_end;  /* 006ab681 */
    u8 fallback_drive_root_end_rest[38];
};
#pragma pack(pop)
extern struct setup_picwnd_block setup_picwnd_block;
inline constexpr BorderIntruders &border_intruders = setup_picwnd_block.border_intruders;
inline constexpr SetupPictureWindow &setup_picwnd_state = setup_picwnd_block.setup_picwnd_state;
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
extern iptr human_civ[2];

#endif
