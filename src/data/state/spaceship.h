/* The spaceship.
 * Declarations only, in the original's address order; the definitions are in
 * src/data/gamedata.cpp (see src/data/gamedata.h). */
#ifndef CIV2_STATE_SPACESHIP_H
#define CIV2_STATE_SPACESHIP_H

#pragma pack(push, 1)
struct spaceship_launch_sprite_xs_block {
    PicPlace spaceship_launch_places[8];  /* 0062e250: by number of components - 1 */
    SpaceshipWindow *spaceship_wnd;  /* 0062e2d0 */
};
#pragma pack(pop)
extern struct spaceship_launch_sprite_xs_block spaceship_launch_sprite_xs_block;
extern u8 str_empty_125[4];  /* only SpaceshipWindow::create uses it */
extern u8 str_empty_126[4];  /* only SpaceshipWindow::create uses it */
extern u8 str_empty_136[4];  /* only SpaceshipWindow::play_launch_video uses it */
#pragma pack(push, 1)
struct spaceship_part_table_block {
    SpaceshipPartKind spaceship_parts[6];  /* 00634f60: structurals, components (propulsion, fuel), modules (habitation, life support, solar panels) */
};
#pragma pack(pop)
extern struct spaceship_part_table_block spaceship_part_table_block;
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

#endif
