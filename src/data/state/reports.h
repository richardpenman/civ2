/* Advisors, reports, the High Council and the end-of-game screens.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_REPORTS_H
#define CIV2_STATE_REPORTS_H

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
extern i32 advisor_modal_start_ticks_i32;
extern i32 intel_report_reopen;
/* The scrolling credits screen. */
#pragma pack(push, 1)
typedef struct CreditsScroll {
    int line_count;  /* 00625ec8 */
    int scroll_pos;  /* 00625ecc */
    uint partial_redraw_ok;  /* 00625ed0 */
} CreditsScroll;
#pragma pack(pop)
extern CreditsScroll credits;
extern u8 str_empty_48[4];  /* only demographics_draw uses it */
extern u8 str_empty_51[4];  /* only demographics_draw uses it */
extern RODATA i8 hall_of_fame_read_mode[4];
extern RODATA i8 hall_of_fame_write_mode[4];
extern ThroneRoomWindow *throneroom_wnd;
extern u8 str_empty_67[4];  /* only ThroneRoomWindow::create_windows uses it */
extern u8 str_empty_68[4];  /* only ThroneRoomWindow::create_windows uses it */
extern int game_language;
extern EndgameMovieWindow *endgame_movie_window;
extern u8 str_empty_84[4];  /* only EndgameMovieWindow::defeat_movie_wnd_create uses it */
extern u8 str_empty_85[4];  /* only EndgameMovieWindow::defeat_movie_wnd_create uses it */
extern EndgameMovieWindow *spaceship_arrival_window;
extern u8 str_empty_88[4];  /* only EndgameMovieWindow::arrival_movie_wnd_create uses it */
extern u8 str_empty_89[4];  /* only EndgameMovieWindow::arrival_movie_wnd_create uses it */
extern u8 str_empty_91[4];  /* only endgame_show_centauri_beaten uses it */
extern u8 str_empty_92[4];  /* only endgame_show_centauri_beaten uses it */
extern DestroyedCivsWindow *victory_screen_wnd;
extern i32 destroyed_civs_show_date;
extern u8 str_empty_156[4];  /* only DestroyedCivsWindow::create uses it */
extern u8 str_empty_157[4];  /* only DestroyedCivsWindow::create uses it */
/* The High Council. */
#pragma pack(push, 1)
typedef struct CouncilState {
    i32 video_busy;  /* 00631ac8 */
    CouncilWindow *wnd;  /* 00631acc */
    char active;  /* 00631ad0 */
    u8 active_rest[3];
} CouncilState;
#pragma pack(pop)
extern CouncilState council_state;
extern u8 str_empty_159[4];  /* only CouncilWindow::init uses it */
extern u8 str_empty_160[4];  /* only CouncilWindow::init uses it */
#pragma pack(push, 1)
struct tiles_dll_block {
    DllHandle tiles_dll;  /* 0063e4f0 */
    int unit_ability_list_nonempty;  /* 0063e4f4 */
};
#pragma pack(pop)
extern struct tiles_dll_block tiles_dll_block;
/* The casualty timeline (military advisor) window. */
#pragma pack(push, 1)
typedef struct CasualtyWindowState {
    DibSurface window;  /* 0063e4f8 */
    u8 parent_wnd[16];  /* 0063e540 */
    u8 wnd_events[204];  /* 0063e550 */
    int client_x;  /* 0063e61c */
    int client_y;  /* 0063e620 */
    i32 client_width;  /* 0063e624 */
    u8 client_width_rest[404];
    int client_right;  /* 0063e7bc */
    u8 client_right_rest[88];
    Button close_button[4];  /* 0063e818 */
    Scrollbar scrollbar;  /* 0063e908 */
    int report_id;  /* 0063e948 */
    u8 report_id_rest[4];
    i32 page_count;  /* 0063e950 */
    iptr civ;  /* 0063e954 */
    int scroll_pos;  /* 0063e958 */
    int rows_per_page;  /* 0063e95c */
    int first_row_y;  /* 0063e960 */
    i32 list_height;  /* 0063e964 */
    i32 row_height;  /* 0063e968 */
    u8 row_height_rest[32];
    i32 vscroll_width;  /* 0063e98c */
    int controls_created;  /* 0063e990 */
    i32 report_param;  /* 0063e994 */
    u8 report_param_rest[8];
} CasualtyWindowState;
#pragma pack(pop)
#pragma pack(push, 1)
struct casualty_window_block {
    CasualtyWindowState casualty;  /* 0063e4f8 */
    i32 credits_slot_widths[30];  /* 0063e9a0 */
    int civ_rating_percent;  /* 0063ea18 */
    u8 civ_rating_percent_rest[4];
};
#pragma pack(pop)
extern struct casualty_window_block casualty_window_block;
inline constexpr CasualtyWindowState &casualty = casualty_window_block.casualty;
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
/* The civilizations destroyed: when, by whom. */
#pragma pack(push, 1)
typedef struct DestroyedCivs {
    i16 count[1];  /* 00655128 */
    i16 turns[12];  /* 0065512a */
    i8 killers[12];  /* 00655142 */
    i8 portraits[12];  /* 0065514e */
    i8 names[294];  /* 0065515a */
} DestroyedCivs;
#pragma pack(pop)
#pragma pack(push, 1)
struct destroyed_civ_count_block {
    DestroyedCivs destroyed_civs;  /* 00655128 */
    int saved_layout_valid;  /* 00655280 */
};
#pragma pack(pop)
extern struct destroyed_civ_count_block destroyed_civ_count_block;
inline constexpr DestroyedCivs &destroyed_civs = destroyed_civ_count_block.destroyed_civs;
extern u8 destroyed_civs_text_bg[96];
extern u8 art_palette[1080];

#endif
