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

/* The throne room, drawn whole from the pack's art on the plane of its
   surface, over the original's picture (which the game still draws, from
   the same libraries, with the same palettes and rand() calls: only the
   plane changes).  The pack's room is an empty hall
   (resource/pv.dll/gifs/100: in the original, the hall the game colours to
   choose a section), furnished with palace/FAMILY-LEVEL by the
   civilization's levels (throne_room[slot], 0: none; the families in the
   slots' order) and, the room complete, the bonus pieces palace/bonus-I
   (the bits of throne_room[8]).  Their placement and order are Peter
   Johnson's, from hd_throne_room in PR #11's src/game/hd_windows.cpp. */
static const char *const throne_family[8] = {"arch",     "rug",     "dais",   "columns",
                                             "tapestry", "brazier", "throne", "ceiling-medallion"};

static void throne_furnishing(void *bits, int slot, int level, double cx, double base, double w, double h)
{
    char key[64];
    if (level < 1)
        return;
    snprintf(key, sizeof key, "palace/%s-%d", throne_family[slot], level > 4 ? 4 : level);
    hd_picture(bits, key, (cx - w / 2) * 640, (base - h) * 480, w * 640, h * 480, 1);
}

static int throne_hall(ThroneRoomWindow *v)
{
    void *bits = (void *)v->throne_surface_wnd.bits;
    const u8 *levels = (const u8 *)civs[v->throne_civ].throne_room;
    int i;
    if (!hd_picture(bits, "resource/pv.dll/gifs/100", 0, 0, 640, 480, 0))
        return 0;
    throne_furnishing(bits, 7, levels[7], .5, .19, .18, .16);
    throne_furnishing(bits, 0, levels[0], .5, .69, .70, .57);
    throne_furnishing(bits, 4, levels[4], .5, .57, .22, .36);
    throne_furnishing(bits, 3, levels[3], .22, .77, .21, .55);
    throne_furnishing(bits, 3, levels[3], .78, .77, .21, .55);
    throne_furnishing(bits, 1, levels[1], .5, .96, .50, .35);
    throne_furnishing(bits, 2, levels[2], .5, .84, .44, .27);
    throne_furnishing(bits, 6, levels[6], .5, .77, .22, .34);
    throne_furnishing(bits, 5, levels[5], .26, .88, .13, .25);
    throne_furnishing(bits, 5, levels[5], .74, .88, .13, .25);
    if (v->throne_complete)
        for (i = 0; i < 7; i++)
            if (levels[8] & (1 << i)) {
                char key[40];
                snprintf(key, sizeof key, "palace/bonus-%d", i);
                hd_picture(bits, key, 48 + i * 78, 384, 64, 60, 1);
            }
    return 1;
}

/* The choice ("Which section shall we improve?"): the game keeps its own
   choosing, by the regions of its click map over its colour-coded hall
   (add_choice_regions: the first region holding the point gives the
   section, or 10 + the bonus piece), so the plane shows the pack's room,
   darkened, and for each section that can be chosen the piece to be built,
   framed, in the largest rectangle of the part of the screen whose clicks
   choose it: a click on a piece chooses it (PR #11 drew its pieces in
   boxes at the screen's sides, with a click map of its own). */
static unsigned char *throne_owner;     /* 640 x 480: the click map's value there, 255 none */

static void throne_choice(ThroneRoomWindow *v, int value, const char *key)
{
    void *bits = (void *)v->throne_surface_wnd.bits;
    static int heights[641];
    int aw, ah, x, y, bx = 0, by = 0, bw = 0, bh = 0;
    double best = 0, k, pw, ph;
    const double pad = 3, most = 150;
    if (!hd_picture_size(key, &aw, &ah))
        return;
    /* its largest rectangle for the piece (as large as it fits): the
       largest rectangle under each row's histogram */
    for (x = 0; x <= 640; x++)
        heights[x] = 0;
    for (y = 0; y < 480; y++) {
        int stack_x[641], stack_h[641], n = 0;
        for (x = 0; x < 640; x++)
            heights[x] = throne_owner[y * 640 + x] == value ? heights[x] + 1 : 0;
        for (x = 0; x <= 640; x++) {
            int h = x < 640 ? heights[x] : 0, start = x;
            while (n && stack_h[n - 1] >= h) {
                int sx = stack_x[--n], sh = stack_h[n];
                double fit = (x - sx - 2 * pad) / aw < (sh - 2 * pad) / ah ? (x - sx - 2 * pad) / aw : (sh - 2 * pad) / ah;
                if (fit > best) {
                    best = fit;
                    bx = sx; by = y - sh + 1; bw = x - sx; bh = sh;
                }
                start = sx;
            }
            stack_x[n] = start;
            stack_h[n++] = h;
        }
    }
    if (best <= 0)
        return;
    k = best;
    if (aw * k > most || ah * k > most)
        k = most / (aw > ah ? aw : ah);
    pw = aw * k;
    ph = ah * k;
    hd_picture_fill(bits, bx + (bw - pw) / 2 - pad, by + (bh - ph) / 2 - pad, pw + 2 * pad, ph + 2 * pad, 0xffc8a050);
    hd_picture_fill(bits, bx + (bw - pw) / 2 - pad + 1, by + (bh - ph) / 2 - pad + 1, pw + 2 * pad - 2,
                    ph + 2 * pad - 2, 0xff2a2018);
    hd_picture(bits, key, bx + (bw - pw) / 2, by + (bh - ph) / 2, pw, ph, 1);
}

/* Called at the end of ThroneRoomWindow::render (choices 0) and
   render_choices (1), with HD graphics on. */
void hd_throne_room(ThroneRoomWindow *v, int choices)
{
    const u8 *levels = (const u8 *)civs[v->throne_civ].throne_room;
    char key[64];
    int i, r, x, y;
    if (!throne_hall(v))
        return;
    if (choices && (throne_owner || (throne_owner = (unsigned char *)malloc(640 * 480)))) {
        hd_picture_fill((void *)v->throne_surface_wnd.bits, 0, 0, 640, 480, 0x70000000);
        /* the click map, first region first */
        memset(throne_owner, 255, 640 * 480);
        for (r = (int)v->choice_clickmap.entry_count - 1; r >= 0; r--) {
            const ClickRegion *g = &v->choice_clickmap.hit_regions[r];
            for (y = g->top < 0 ? 0 : g->top; y < g->bottom && y < 480; y++)
                for (x = g->left < 0 ? 0 : g->left; x < g->right && x < 640; x++)
                    throne_owner[y * 640 + x] = (unsigned char)g->value;
        }
        if (!v->throne_complete) {
            for (i = 0; i < 8; i++)
                if (levels[i] < 4) {
                    snprintf(key, sizeof key, "palace/%s-%d", throne_family[i], levels[i] + 1);
                    throne_choice(v, i, key);
                }
        } else
            for (i = 0; i < 7; i++) {
                /* (bonus pieces 4 and 5 share their regions, those of 15:
                   the game gives the one not yet built, 5 first) */
                int piece = i == 5 && (levels[8] & 0x20) ? 4 : i;
                if (i != 4 && !(levels[8] & (1 << piece))) {
                    snprintf(key, sizeof key, "palace/bonus-%d", piece);
                    throne_choice(v, 10 + i, key);
                }
            }
    }
    hd_picture_done((void *)v->throne_surface_wnd.bits);
}
