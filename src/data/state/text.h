/* Text: the game's texts and labels, their arguments, the string pool, text files.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_TEXT_H
#define CIV2_STATE_TEXT_H

extern u8 open_path_prefix[60];  /* only file_build_open_path uses it */
extern char file_path_mode;  /* only file_build_open_path uses it */
extern u8 file_path_mode_rest[3];
extern int strpool_capacity;
extern RODATA i8 text_var_hex_tag_str[4];
extern RODATA i8 text_var_hex_pad_str[4];
extern RODATA i8 text_var_percent_str[8];
/* The string pool's fill. */
#pragma pack(push, 1)
typedef struct StringPoolState {
    int count;  /* 00625e60 */
    int initialized;  /* 00625e64 */
    u8 initialized_rest[8];
} StringPoolState;
#pragma pack(pop)
extern StringPoolState strpool;
extern RODATA i8 pop_thousands_sep_str[4];
extern RODATA i8 pop_zero_pad_str[4];
extern RODATA i8 city_pop_thousands_sep_str[4];
extern RODATA i8 city_pop_zero_pad_str[4];
extern i8 *str_ptr_labels;
extern u8 str_ptr_labels_rest[8];
extern FILE *text_file_handle;
extern RODATA i8 append_space_str[4];
extern RODATA i8 append_comma_str[4];
extern RODATA i8 append_colon_str[4];
extern RODATA i8 append_period_str[4];
extern RODATA i8 append_percent_str[4];
extern RODATA i8 append_lparen_str[4];
extern RODATA i8 append_rparen_str[12];
extern RODATA i8 append_plus_str[28];
extern RODATA i8 int_2d_fmt[4];
extern RODATA i8 advance_abbrev_no_strs[12];
extern RODATA i8 advance_abbrev_nil_strs[12];
extern RODATA i8 advance_abbrev_comma_str[4];
extern RODATA i8 advance_abbrev_err_strs[12];
/* The index of a text file's @sections. */
#pragma pack(push, 1)
typedef struct TextSectionIndex {
    i8 path[260];  /* 0062e508 */
    i32 valid;  /* 0062e60c */
    int count;  /* 0062e610 */
} TextSectionIndex;
#pragma pack(pop)
extern TextSectionIndex text_index;
extern RODATA i8 text_index_path_sep_str[4];
extern RODATA i8 text_lookup_path_sep_str[60];
extern RODATA i8 filename_valid_chars[128];
extern RODATA i8 file_copy_read_mode[4];
extern RODATA i8 file_copy_write_mode[4];
/* How text is drawn: target, font, colors, shadow. */
#pragma pack(push, 1)
typedef struct TextStyle {
    DibSurface *surface;  /* 006366a8 */
    GameFont *font;  /* 006366ac */
    i32 fg_color;  /* 006366b0 */
    int shadow_color;  /* 006366b4 */
    int shadow_dx;  /* 006366b8 */
    int shadow_dy;  /* 006366bc */
    int fake_bold;  /* 006366c0 */
    u8 fake_bold_rest[8];
} TextStyle;
#pragma pack(pop)
extern TextStyle text_style;
extern RODATA i8 file_path_backslash_str[4];
extern uint res_dll_module_count;
#pragma pack(push, 1)
struct strpool_block_block {
    i8 strpool_block[8];  /* 0063e4c0 */
    char *strpool_data;  /* 0063e4c8 */
    u8 strpool_data_rest[20];
};
#pragma pack(pop)
extern struct strpool_block_block strpool_block_block;

#endif
