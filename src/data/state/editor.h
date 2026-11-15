/* The scenario and events editors.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_EDITOR_H
#define CIV2_STATE_EDITOR_H

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
    union {
        struct {
            i32 tribe_editor_edit_x_table[1];  /* 0062cd60 */
            i32 tribe_editor_edit_y_table[1];  /* 0062cd64 */
        };
        EditorPos tribe_editor_edit_pos[1];  /* 0062cd60 */
    };
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
extern u8 str_empty_114[4];  /* only EditorWindow::tribe_editor_create_edit uses it */
extern RODATA i8 city_file_stop_str[8];
extern i8 editor_text_dlg_empty_title[4];  /* only editor_text_dlg_create uses it */
extern u8 str_empty_168[4];  /* only editor_create_scenario_folder uses it */
extern RODATA i8 scenario_path_sep_str[4];
extern RODATA i8 scenario_file_ext_str[8];
#pragma pack(push, 1)
struct editor_font_block {
    GameFont editor_font;  /* 006a4f90 */
    i32 editor_text_result;  /* 006a4f98 */
    i32 editor_text_active;  /* 006a4f9c */
};
#pragma pack(pop)
extern struct editor_font_block editor_font_block;

#endif
