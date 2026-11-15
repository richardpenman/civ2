/* Civilizations and the rules (RULES.TXT): advances, governments, leaders, the rules editors' tables.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_CIV_H
#define CIV2_STATE_CIV_H

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
    union {
        struct {
            i32 tech_editor_edit_x_table[1];  /* 00625150 */
            i32 tech_editor_edit_y_table[1];  /* 00625154 */
            int tech_editor_modifier_edit_x;  /* 00625158 */
            int tech_editor_modifier_edit_y;  /* 0062515c */
        };
        EditorPos tech_editor_edit_pos[2];  /* 00625150 */
    };
};
#pragma pack(pop)
extern struct tech_editor_edit_x_table_block tech_editor_edit_x_table_block;
#pragma pack(push, 1)
struct tech_editor_ctrl_kinds_block {
    union {
        struct {
            u8 tech_editor_ctrl_rows_0_4[40];  /* 00625160 */
            i32 tech_editor_field_min_table[2];  /* 00625188: by field */
            i32 tech_editor_field_max_table[2];  /* 00625190 */
            u8 tech_editor_field_max_table_rest[16];
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
extern RODATA i8 civilize_rules_newline_str[4];
extern RODATA i8 tech_editor_usage_preq1_fmt[4];
extern RODATA i8 tech_editor_usage_preq2_fmt[4];
extern u8 str_empty_11[4];  /* only EditorWindow::tech_editor_create_edit uses it */
extern RODATA i8 tech_editor_prereq1_suffix_str[4];
extern RODATA i8 tech_editor_prereq2_suffix_str[4];
extern Advance advances[100];
extern i32 double_moves_applied;
extern u8 str_empty_110[4];  /* only civ_change_password_dialog uses it */
extern u8 str_empty_111[4];  /* only civ_change_password_dialog uses it */
extern u8 str_empty_112[4];  /* only civ_change_password_dialog uses it */
extern u8 str_empty_113[4];  /* only civ_password_prompt uses it */
extern i8 setup_chosen_leaders[8];
extern RODATA i8 rules_advance_no_str[4];
extern RODATA i8 rules_advance_nil_str[4];
extern RODATA i8 rules_terrain_no_str[4];
extern RODATA i8 rules_terrain_yes_str[8];
extern int research_notice_mode;  /* only civ_research_complete uses it */
extern RODATA i32 cosmic_edit_min_values[22];
extern RODATA i32 cosmic_edit_max_values[22];
extern RODATA i8 cosmic_value_fmt_3digit[4];
#pragma pack(push, 1)
struct civ_rating_title_font_block {
    GameFont civ_rating_title_font;  /* 0063e4e0 */
    i32 mp_meeting_wait_start_i32;  /* 0063e4e8 */
    int civ_rating_title_index;  /* 0063e4ec */
};
#pragma pack(pop)
extern struct civ_rating_title_font_block civ_rating_title_font_block;
extern Cosmic cosmic;
extern u8 cosmic_rest[3];
/* The rules editors' text dialog window. */
#pragma pack(push, 1)
typedef struct EditorTextDialogState {
    DibSurface wnd;  /* 006a18c0 */
    u8 inner[204];  /* 006a1908 */
    i32 style;  /* 006a19d4 */
    int caption_height;  /* 006a19d8 */
    int border_width;  /* 006a19dc */
    int resize_border;  /* 006a19e0 */
    int client_x;  /* 006a19e4 */
    int client_y;  /* 006a19e8 */
    int client_width;  /* 006a19ec */
    int client_height;  /* 006a19f0 */
    i8 title[200];  /* 006a19f4 */
    i32 caption_buttons;  /* 006a1abc */
    u8 caption_buttons_rest[168];
    i32 caption_handler;  /* 006a1b68 */
    u8 caption_handler_rest[16];
    u8 fill_rect[16];  /* 006a1b7c */
    i32 title_font;  /* 006a1b8c */
    i32 title_small_font;  /* 006a1b90 */
    i32 text_font;  /* 006a1b94 */
} EditorTextDialogState;
#pragma pack(pop)
/* The @COSMIC editor's fields. */
#pragma pack(push, 1)
typedef struct CosmicEditValues {
    u8 values[4];  /* 006a2d80 */
    u8 trireme_loss;  /* 006a2d84 */
    u8 trireme_loss_rest[3];
    u8 food_per_citizen;  /* 006a2d88 */
    u8 food_per_citizen_rest[3];
    u8 food_box_rows;  /* 006a2d8c */
    u8 food_box_rows_rest[3];
    u8 shield_box_rows;  /* 006a2d90 */
    u8 shield_box_rows_rest[3];
    u8 settler_food_low;  /* 006a2d94 */
    u8 settler_food_low_rest[3];
    u8 settler_food_high;  /* 006a2d98 */
    u8 settler_food_high_rest[3];
    u8 unhappy_size;  /* 006a2d9c */
    u8 unhappy_size_rest[3];
    u8 riot_factor;  /* 006a2da0 */
    u8 riot_factor_rest[3];
    u8 aqueduct_size;  /* 006a2da4 */
    u8 aqueduct_size_rest[3];
    u8 sewer_size;  /* 006a2da8 */
    u8 sewer_size_rest[3];
    u8 tech_paradigm;  /* 006a2dac */
    u8 tech_paradigm_rest[3];
    u8 transform_time;  /* 006a2db0 */
    u8 transform_time_rest[3];
    u8 monarchy_support;  /* 006a2db4 */
    u8 monarchy_support_rest[3];
    u8 communism_support;  /* 006a2db8 */
    u8 communism_support_rest[3];
    u8 fundamentalism_support;  /* 006a2dbc */
    u8 fundamentalism_support_rest[3];
    u8 communism_palace_dist;  /* 006a2dc0 */
    u8 communism_palace_dist_rest[3];
    u8 fund_science_loss;  /* 006a2dc4 */
    u8 fund_science_loss_rest[3];
    u8 change_penalty;  /* 006a2dc8 */
    u8 change_penalty_rest[3];
    u8 paradrop_range;  /* 006a2dcc */
    u8 paradrop_range_rest[3];
    u8 mass_thrust_paradigm;  /* 006a2dd0 */
    u8 mass_thrust_paradigm_rest[3];
    u8 fund_max_science;  /* 006a2dd4 */
    u8 fund_max_science_rest[8627];
} CosmicEditValues;
#pragma pack(pop)
/* The rules editors' name lists and controls. */
#pragma pack(push, 1)
typedef struct EditorLists {
    i8 govt_names[21][15];  /* 006a1b98: the 7 governments, then their male and female leaders' titles */
    u8 govt_names_rest[5];
    i8 commodity_names[16][10];  /* 006a1cd8 */
    i32 *text_lines_out;  /* 006a1d78 */
    i32 running;  /* 006a1d7c */
    int next_ctrl_id;  /* 006a1d80 */
    u8 next_ctrl_id_rest[4];
} EditorLists;
#pragma pack(pop)
/* The rules editor open. */
#pragma pack(push, 1)
typedef struct EditorActive {
    EditorWindow *wnd;  /* 006a4f88 */
    u8 wnd_rest[4];
} EditorActive;
#pragma pack(pop)
#pragma pack(push, 1)
struct editor_text_dlg_wnd_block {
    EditorTextDialogState editor_text_dlg;  /* 006a18c0 */
    EditorLists editor_lists;  /* 006a1b98 */
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
            CosmicEditValues cosmic_edit;  /* 006a2d80 */
        };
        struct {
            i8 editor_item_name_recs[100][40];  /* 006a1d88 an item's name */
            EditorItem editor_item_recs[100];  /* 006a2d28 an item's values (cosmic editor: record 1) */
        };
    };
    EditorActive editor_active;  /* 006a4f88 */
};
#pragma pack(pop)
extern struct editor_text_dlg_wnd_block editor_text_dlg_wnd_block;
inline constexpr EditorActive &editor_active = editor_text_dlg_wnd_block.editor_active;
inline constexpr EditorLists &editor_lists = editor_text_dlg_wnd_block.editor_lists;
inline constexpr CosmicEditValues &cosmic_edit = editor_text_dlg_wnd_block.cosmic_edit;
inline constexpr EditorTextDialogState &editor_text_dlg = editor_text_dlg_wnd_block.editor_text_dlg;

#endif
