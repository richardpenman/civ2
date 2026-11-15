/* The Civilopedia.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_PEDIA_H
#define CIV2_STATE_PEDIA_H

extern int hyperlink_class_registered;  /* only hyperlink_register_class uses it */
extern u8 str_empty_73[4];  /* only PediaWindow::update_controls uses it */
extern i8 hyperlink_window_title_str[4];  /* only hyperlink_create_hwnd uses it */
extern RODATA i8 pedia_terrain_move_cost_fmt[4];
extern RODATA i8 pedia_terrain_defense_50_str[8];
extern RODATA i8 pedia_terrain_food_fmt[4];
extern RODATA i8 pedia_terrain_shields_fmt[4];
extern RODATA i8 pedia_terrain_trade_fmt[4];
extern RODATA i8 pedia_terrain_improve_bonus_fmt[8];
extern RODATA i8 pedia_terrain_improve_turns_fmt[4];
/* The Civilopedia: missing-text placeholder and the Back button. */
#pragma pack(push, 1)
typedef struct PediaState {
    i8 *missing_text;  /* 0062f008 */
    i32 going_back;  /* 0062f00c */
    i32 back_topic;  /* 0062f010 */
} PediaState;
#pragma pack(pop)
extern PediaState pedia;
extern i8 pedia_window_empty_title_str[4];  /* only PediaWindow::create uses it */
extern u8 str_empty_143[4];  /* only PediaWindow::create uses it */
extern RODATA i8 pedia_grassland_shield_abbrev_str[4];
extern RODATA i8 pedia_index_marker_str[64];
extern RODATA i8 pedia_improvement_cost_fmt[4];
extern RODATA i8 pedia_improve_cost_fmt[4];
extern RODATA i8 pedia_improve_upkeep_fmt[4];
extern RODATA i8 pedia_improve_text_space_str[8];
extern RODATA i8 pedia_unit_cost_fmt[4];
extern RODATA i8 pedia_unit_attack_fmt[4];
extern RODATA i8 pedia_unit_defense_fmt[4];
extern RODATA i8 pedia_unit_hp_fmt[4];
extern RODATA i8 pedia_unit_firepower_fmt[4];
extern RODATA i8 pedia_unit_move_fmt[4];
#pragma pack(push, 1)
struct war_icon_sprite_block {
    u8 war_icon_sprite[64];  /* 00644e48 */
    Sprite unused_sprite_pair[2];  /* 00644e88 */
    /* food, shields, trade: [0] the shortage's (the corruption's), [1] the yield's */
    union {
        Sprite resource_icon_sprites[3][2];  /* 00644f00 */
        struct {
            Sprite food_shortage_icon_sprite;
            Sprite food_icon_sprite;
            Sprite shield_shortage_icon_sprite;
            Sprite shield_icon_sprite;
            Sprite trade_corruption_icon_sprite;
            Sprite trade_icon_sprite;
        };
    };
};
#pragma pack(pop)
extern struct war_icon_sprite_block war_icon_sprite_block;
extern i32 government_names[7];
/* The Civilopedia window: the page shown, its history and lists. */
#pragma pack(push, 1)
typedef struct PediaWindowState {
    SurfaceWindow window;  /* 006a6668 the PediaWindow's SurfaceWindow; its members follow */
    int closed;  /* 006a677c */
    i32 category;  /* 006a6780 */
    i32 page_mode;  /* 006a6784 */
    int topic;  /* 006a6788 */
    int modal_active;  /* 006a678c */
    i32 history_count;  /* 006a6790 */
    i32 history_topics[100];  /* 006a6794 */
    i32 history_page_modes[100];  /* 006a6924 */
    i32 history_categories[100];  /* 006a6ab4 */
    u8 history_categories_rest[28];
    u8 describe_rect[32];  /* 006a6c60 */
    i8 title_text[64];  /* 006a6c80 */
    int advance_entry_count;  /* 006a6cc0 */
    u8 advance_entry_count_rest[800];
    int improvement_entry_count;  /* 006a6fe4 */
    u8 improvement_entry_count_rest[312];
    int wonder_entry_count;  /* 006a7120 */
    u8 wonder_entry_count_rest[224];
    int unit_entry_count;  /* 006a7204 */
    u8 unit_entry_count_rest[496];
    int government_entry_count;  /* 006a73f8 */
    u8 government_entry_count_rest[56];
    int terrain_count;  /* 006a7434 */
    u8 terrain_count_rest[132];
    i32 terrain_ids[33];  /* 006a74bc */
    int concept_entry_count;  /* 006a7540 */
    u8 concept_entry_count_rest[2048];
    PediaListNode *advance_names;  /* 006a7d44 */
    PediaListNode *improvement_names;  /* 006a7d48 */
    PediaListNode *wonder_names;  /* 006a7d4c */
    PediaListNode *unit_names;  /* 006a7d50 */
    PediaListNode *government_names;  /* 006a7d54 */
    PediaListNode *terrain_names;  /* 006a7d58 */
    PediaListNode *concept_names;  /* 006a7d5c */
    u8 concept_names_rest[1000];
    Scrollbar list_scrollbar;  /* 006a8148 */
    Hotspot *close_hotspot;  /* 006a8188 */
    int list_rect;  /* 006a818c */
    int list_rect_top;  /* 006a8190 */
    int list_rect_right;  /* 006a8194 */
    int list_rect_bottom;  /* 006a8198 */
    int list_count;  /* 006a819c */
    u8 list_count_rest[1024];
    i32 selected_item;  /* 006a85a0 */
    int list_top;  /* 006a85a4 */
    PediaLink *link_list;  /* 006a85a8 */
    i32 popped_category;  /* 006a85ac */
    i32 popped_page_mode;  /* 006a85b0 */
    u8 popped_page_mode_rest[1612];
} PediaWindowState;
#pragma pack(pop)
#pragma pack(push, 1)
struct pedia_window_block {
    PediaWindowState pedia_wnd;  /* 006a6668 */
};
#pragma pack(pop)
extern struct pedia_window_block pedia_window_block;
inline constexpr PediaWindowState &pedia_wnd = pedia_window_block.pedia_wnd;

#endif
