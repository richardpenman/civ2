/*
 * (port) HD graphics (src/platform/hd.h, docs/HD-GRAPHICS.md): the game's
 * part.  The sprites the game takes from its sheets find their pictures by
 * where they were taken from; the terrain's blends, which the game makes
 * itself from a mask, are given theirs here.  Called at the end of
 * gfx_load_terrain_sprites, only with HD graphics on.  The city view's
 * houses keep the pack's pictures only in the era they show (below).
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

/* The city view's houses and empty lots (CityViewWindow::load, cv.dll 310),
   once taken.  The pack's houses are all 19th-century buildings (terraced
   houses, a tenement, a clapboard house, a shop row), the same pictures for
   both of the original's rows (y 116 and 429: the cottages of ancient and
   renaissance cities; 231 and 512: the towns from Industrialization on), so
   they are drawn only in the industrial era (civ_city_style_era 2, the
   owner's, the same for the whole view); in the others, and for the empty
   lots (woods in the original, nothing in the pack: its renderer drew its
   own lot pictures), the original's sprites, enlarged. */
void hd_bind_cityview(CityViewWindow *w)
{
    int era = civ_city_style_era(cities[w->city_id].owner), i;
    for (i = 0; i < 4; i++) {
        hd_sprite_forget(&w->large_building_sprites_a[i]);
        hd_sprite_forget(&w->small_building_sprites_a[i]);
        if (era != 2) {
            hd_sprite_forget(&w->large_building_sprites_b[i]);
            hd_sprite_forget(&w->small_building_sprites_b[i]);
        }
    }
}
