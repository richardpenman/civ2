/* The map views, the world map window and the animations on the map.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_MAPVIEW_H
#define CIV2_STATE_MAPVIEW_H

extern pointer ptr_fun_2;  /* only mapview_ctor uses it */
extern i8 minimap_hidden;
extern u8 minimap_hidden_rest[7];
/* Dragging the map view and the tile a click is on. */
#pragma pack(push, 1)
typedef struct MapClickState {
    int drag_hold_timer;  /* 00624f54 */
    int drag_view_index;  /* 00624f58 */
    iptr tile_x;  /* 00624f5c */
    iptr tile_y;  /* 00624f60 */
    u8 tile_y_rest[4];
} MapClickState;
#pragma pack(pop)
extern MapClickState map_click;
extern int map_layout_wide;
#pragma pack(push, 1)
struct diag_neighbor_bits_block {
    i8 diag_neighbor_bits[4];  /* 00628338: by diagonal direction */
    i8 diag_dx_table[5];  /* 0062833c: by diagonal direction, then the square itself (4) */
    u8 diag_dx_table_rest[3];
    i8 diag_dy_table[5];  /* 00628344 */
    u8 diag_dy_table_rest[7];
};
#pragma pack(pop)
extern struct diag_neighbor_bits_block diag_neighbor_bits_block;
extern RODATA i32 coast_quarter_dx_table[4];
extern RODATA i32 coast_quarter_dy_table[4];
extern int map_show_hidden_terrain;
extern u8 map_show_hidden_terrain_rest[16];
/* The world map window's scale and origin. */
#pragma pack(push, 1)
typedef struct MinimapLayout {
    int tile_width;  /* 0063c800 */
    int tile_height;  /* 0063c804 */
    int view_rows;  /* 0063c808 */
    int view_cols;  /* 0063c80c */
    int origin_y;  /* 0063c810 */
    int origin_x;  /* 0063c814 */
} MinimapLayout;
#pragma pack(pop)
extern MinimapLayout minimap_layout;
extern int minimap_margin;
#pragma pack(push, 1)
struct map_view_saved_rects_block {
    RECT map_view_saved_rects[8];  /* 006552a4: by map view */
};
#pragma pack(pop)
extern struct map_view_saved_rects_block map_view_saved_rects_block;

#endif
