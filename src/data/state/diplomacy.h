/* Diplomacy: meetings, the AI's evaluation, the leaders' screens.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_DIPLOMACY_H
#define CIV2_STATE_DIPLOMACY_H

extern RODATA i8 tribe_leader_sound_set[24];
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
/* The diplomatic meeting in progress (AI evaluation, emissaries). */
#pragma pack(push, 1)
typedef struct DiploMeeting {
    i32 initial_eval_done;  /* 00626a1c */
    int opening_eval_active;  /* 00626a20 */
    int session_active;  /* 00626a24 */
    i32 ai_meeting_active;  /* 00626a28 */
    i32 mp_pending;  /* 00626a2c */
    int emissary_asked;  /* 00626a30 */
    int emissary_response;  /* 00626a34 */
} DiploMeeting;
#pragma pack(pop)
extern DiploMeeting diplo_meeting;
extern i8 *leader_video_tribe_codes[21];  /* only DiploLeaderWindow::init_video uses it */
/* The foreign leader's window in a diplomatic meeting: video and sounds. */
#pragma pack(push, 1)
typedef struct DiploLeaderScreen {
    DiploLeaderWindow *window;  /* 0062ca34 */
    char wnd_open;  /* 0062ca38 */
    u8 wnd_open_rest[3];
    i8 wnd_busy;  /* 0062ca3c */
    u8 wnd_busy_rest[3];
    char video_active;  /* 0062ca40 */
    u8 video_active_rest[3];
    int greeting_sound_id;  /* 0062ca44 */
    int sound_ids[2];  /* 0062ca48 */
    int reaction_sound_ids[3];  /* 0062ca50 */
} DiploLeaderScreen;
#pragma pack(pop)
extern DiploLeaderScreen diplo_leader;
extern int tech_prereq_walk_count;
extern u8 str_empty_102[4];  /* only DiploLeaderWindow::init_background uses it */
extern u8 str_empty_103[4];  /* only DiploLeaderWindow::init_main_art uses it */
extern u8 str_empty_108[4];  /* only DiploLeaderWindow::init_video uses it */
extern int spy_last_mission;
extern int diplo_border_intruded_cities;
extern u8 signal_allied_icon_sprite[64];
#pragma pack(push, 1)
struct peace_treaty_icon_sprite_block {
    u8 peace_treaty_icon_sprite[64];  /* 006409d8 */
    i32 city_flag_x_offsets[48];  /* 00640a18 */
    i32 city_flag_y_offsets[48];  /* 00640ad8 */
};
#pragma pack(pop)
extern struct peace_treaty_icon_sprite_block peace_treaty_icon_sprite_block;
extern u8 ceasefire_icon_sprite[64];
extern u8 diplo_popup_portrait_sprite[64];
/* The AI's evaluation of a diplomatic meeting (attitudes, threats, what to offer or demand). */
#pragma pack(push, 1)
typedef struct DiploEval {
    int ai_declare_war;  /* 0064b0e8 */
    int ai_fear_level;  /* 0064b0ec */
    int gifts_this_meeting;  /* 0064b0f0 */
    int captured_city_count;  /* 0064b0f4 */
    int ai_aggressive;  /* 0064b0f8 */
    iptr strongest_enemy;  /* 0064b0fc */
    iptr weakest_enemy_civ;  /* 0064b100 */
    iptr war_target_civ;  /* 0064b104 */
    int last_contact_turn;  /* 0064b108 */
    int offer_tech_2nd;  /* 0064b10c */
    int attitude_civ;  /* 0064b110 */
    int cur_attitude;  /* 0064b114 */
    int tribute_demand;  /* 0064b118 */
    int target_is_leader;  /* 0064b11c */
    int attitude_target;  /* 0064b120 */
    int offer_tech_best;  /* 0064b124 */
    int suppressed_aggression;  /* 0064b128 */
    uint ai_outranks_target;  /* 0064b12c */
    int space_race_threat;  /* 0064b130 */
    int ai_units_near_cities;  /* 0064b134 */
    int had_ceasefire;  /* 0064b138 */
    int ai_initial_aggressive;  /* 0064b13c */
    int peace_inclination;  /* 0064b140 */
    iptr wanted_tech;  /* 0064b144 */
    int ai_request_mode;  /* 0064b148 */
    u8 ai_request_mode_rest[28];
} DiploEval;
#pragma pack(pop)
#pragma pack(push, 1)
struct tax_rate_icon_sprites_block {
    union {
        Sprite tax_rate_icon_sprites[3];  /* 00648860 */
        struct {
            Sprite tax_rate_icon_sprite;
            Sprite gold_coin_icon_sprite;
            Sprite science_icon_sprite;
        };
    };
    u8 tax_rate_icon_sprites_rest[4];
    Sprite lux_tax_sci_icon_sprites[3];  /* 00648918 */
    u8 lux_tax_sci_icon_sprites_rest[7524];
    i8 registry_value_buffer[2464];  /* 0064a730 */
    int cityview_screen_rect;  /* 0064b0d0 */
    int cityview_screen_rect_top;  /* 0064b0d4 */
    int screen_rect_right;  /* 0064b0d8 */
    int cityview_screen_rect_bottom;  /* 0064b0dc */
    u8 cityview_screen_rect_bottom_rest[8];
    DiploEval diplo_eval;  /* 0064b0e8 */
};
#pragma pack(pop)
extern struct tax_rate_icon_sprites_block tax_rate_icon_sprites_block;
inline constexpr DiploEval &diplo_eval = tax_rate_icon_sprites_block.diplo_eval;
extern Civ civs[8];

#endif
