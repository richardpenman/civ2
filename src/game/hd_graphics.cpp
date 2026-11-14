/*
 * (port) HD graphics (src/platform/hd.h, docs/HD-GRAPHICS.md): the game's
 * part.  The sprites the game takes from its sheets find their pictures by
 * where they were taken from; the terrain's blends, which the game makes
 * itself from a mask, are given theirs here.  Called at the end of
 * gfx_load_terrain_sprites, only with HD graphics on.
 */
#include "auto_common.h"
#include "../platform/hd.h"

void hd_bind_terrain(void)
{
    Sprite *s;
    char key[32];
    int t, q, i;
    /* the base diamonds tile the map: drawn opaque inside, nothing outside */
    for (t = 0; t < TERRAIN_COUNT; t++) {
        s = (Sprite *)&terrain_base_sprites_block.terrain_base_sprites + t;
        hd_sprite_opaque(s, s->data_handle);
    }
    s = (Sprite *)&unexplored_tile_sprite;
    hd_sprite_opaque(s, s->data_handle);
    for (i = 0; i < 32; i++) {
        s = (Sprite *)&tutorial_icon_sprite_block.coast_quarter_sprites + i;
        hd_sprite_opaque(s, s->data_handle);
    }
    /* the blends of each terrain (11: unexplored, 12: none) into its
       neighbours, by quarter: made from the dither mask, after the 13 x 4
       sprites from terrain_dither_sprites on (unexplored_dither_sprites
       follows it: terrains 11 and 12) */
    for (t = 0; t < 13; t++)
        for (q = 0; q < 4; q++) {
            s = (Sprite *)&map_cursor_sprite_block.terrain_dither_sprites + t * 4 + q;
            snprintf(key, sizeof key, "dither/%d/%d", t, q);
            hd_sprite_bind(s, s->data_handle, key, 0);
        }
}
