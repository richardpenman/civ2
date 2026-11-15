/* The AI's working state.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_AI_H
#define CIV2_STATE_AI_H

#pragma pack(push, 1)
struct dir_dx_table_block {
    i8 dir_dx_table[9];  /* 00628350: by direction, then the square itself (8) */
    u8 dir_dx_table_rest[7];
};
#pragma pack(pop)
extern struct dir_dx_table_block dir_dx_table_block;
#pragma pack(push, 1)
struct dir_dy_table_block {
    i8 dir_dy_table[9];  /* 00628360 */
    u8 dir_dy_table_rest[7];
    i8 city_tile_dx_table[48];  /* 00628370 */
    i8 city_tile_dy_table[48];  /* 006283a0 */
    i8 coarse_dir_dx_table[16];  /* 006283d0 */
    i8 coarse_dir_dy_table[44];  /* 006283e0 */
};
#pragma pack(pop)
extern struct dir_dy_table_block dir_dy_table_block;

#endif
