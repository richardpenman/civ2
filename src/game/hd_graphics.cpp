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
#include "hd_game.h"
extern "C" void plat_log(const char *fmt, ...);
extern "C" int replay_hold;

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
       follows it: terrains 11 and 12), each only inside its tile's diamond
       (quarters 0 to 3: the tile's top right, bottom right, bottom left,
       top left) */
    for (t = 0; t < 13; t++)
        for (q = 0; q < 4; q++) {
            s = (Sprite *)&map_cursor_sprite_block.terrain_dither_sprites + t * 4 + q;
            snprintf(key, sizeof key, "dither/%d/%d", t, q);
            hd_sprite_bind(s, s->data_handle, key, 3 + q);
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

/* Advances: the pack's picture of each (advance/N: 58 of them, 72x48) where
   the game draws the advance's category icon (36x20, by category and
   epoch), fitted inside the icon's frame (Peter Johnson's idea, hd_advance
   in PR #11; here the game draws its icon as ever, and the plane shows the
   picture instead: hd_sprite_as).  The pack has none for the others: their
   icons stay the original's. */
static Sprite *advance_icon(int advance)
{
    return &government_sprites_block.advance_category_icon_sprites[(char)advances[advance].category][(char)advances[advance].epoch];
}

void hd_advance_begin(int advance)
{
    char key[24];
    Sprite *icon;
    if (!hd_art || advance < 0 || advance >= ADV_COUNT)
        return;
    icon = advance_icon(advance);
    snprintf(key, sizeof key, "advance/%d", advance);
    hd_sprite_as(icon, icon->data_handle, key, 1, -1);
}

void hd_advance_end(void)
{
    if (hd_art)
        hd_sprite_as(NULL, 0, NULL, 0, -1);
}

/* A dialog's entries are drawn after they are added: the entries whose
   sprite is an advance's icon, added between hd_dialog_advance(advance)
   and hd_dialog_advance(-1), with their advance.  Any entry added at the
   same address since is forgotten, so an old entry is never taken for a
   new one. */
static int dialog_pending = -1;
static struct { const void *entry; int advance; } dialog_advances[32];
static int dialog_next;

void hd_dialog_advance(int advance)
{
    dialog_pending = hd_art ? advance : -1;
}

void hd_dialog_sprite_added(const void *entry, const void *sprite)
{
    int i;
    if (!hd_art)
        return;
    for (i = 0; i < 32; i++)
        if (dialog_advances[i].entry == entry)
            dialog_advances[i].entry = NULL;
    if (dialog_pending >= 0 && dialog_pending < ADV_COUNT && sprite == advance_icon(dialog_pending)) {
        dialog_advances[dialog_next].entry = entry;
        dialog_advances[dialog_next].advance = dialog_pending;
        dialog_next = (dialog_next + 1) % 32;
    }
}

void hd_dialog_sprite_begin(const void *entry)
{
    int i;
    if (!hd_art)
        return;
    for (i = 0; i < 32; i++)
        if (dialog_advances[i].entry == entry) {
            hd_advance_begin(dialog_advances[i].advance);
            return;
        }
}

/* The units' shields: the pack's shield is an alpha mask (12x20), drawn in
   the colour the game gives the shield (its owner's, the stack's, the
   shadow's) where the game draws its own and recolours it
   (DibSurface::replace_color: the marker colours 5 and 6), so that the
   recoloured pixels are taken as shown (hd_shield_end); the black bands,
   the hit points and the order's letter are drawn over it as ever (after
   Peter Johnson's gfx_draw_unit_shield and gfx_recolor_unit_shield in
   PR #11, which draw the shield as a silhouette of its owner's colour). */
static int shield_drawn;

void hd_shield_begin(int colour)
{
    Sprite *s = (Sprite *)&unit_shield_sprite;
    shield_drawn = hd_art && hd_sprite_as(s, s->data_handle, "shield", 0, colour);
}

void hd_shield_end(DibSurface *surface, const RECT *recoloured)
{
    if (!shield_drawn)
        return;
    hd_sprite_as(NULL, 0, NULL, 0, -1);
    hd_bits_shown((void *)surface->bits, recoloured->left, recoloured->top, recoloured->right - recoloured->left,
                  recoloured->bottom - recoloured->top);
    shield_drawn = 0;
}

/* A sprite drawn without a picture over the plane's art (the city's
   disorder marker over the city): the game's pixels of its shape, which
   are shown enlarged only where they differ from what was there, are all
   shown, also where the city's pixels underneath happened to be the same
   colour (the city's picture showed through the marker).  drawn: the
   rectangle the game's draw gave; its shape is found as
   extras_draw_grey_sprite finds it (a silhouette in a marker colour, the
   pixels put back after). */
void hd_sprite_pixels_shown(Sprite *sprite, DibSurface *s, int x, int y, const RECT *drawn)
{
    enum { MARK = 0xfe };
    int w = drawn->right - drawn->left, h = drawn->bottom - drawn->top, px, py, any = 0;
    RECT r;
    u8 *keep;
    if (!hd_on || w <= 0 || h <= 0 || w > 1024 || h > 1024 || !(keep = (u8 *)malloc((size_t)w * h * 2)))
        return;
    u8 *shown = keep + (size_t)w * h;   /* the sprite's pixels, put on the plane in one call */
    for (py = 0; py < h; py++)
        for (px = 0; px < w; px++)
            keep[py * w + px] = (u8)(s->in_clip(drawn->left + px, drawn->top + py) ?
                                          s->get_pixel(drawn->left + px, drawn->top + py) : MARK);
    sprite->draw_silhouette(&r, s, x, y, MARK);
    for (py = 0; py < h; py++)
        for (px = 0; px < w; px++) {
            int c = keep[py * w + px], X = drawn->left + px, Y = drawn->top + py;
            shown[py * w + px] = 0;
            if (!s->in_clip(X, Y) || (int)s->get_pixel(X, Y) != MARK)
                continue;
            s->set_pixel(X, Y, c);
            shown[py * w + px] = 1;
            any = 1;
        }
    if (any)
        hd_bits_redrawn_mask((void *)s->bits, drawn->left, drawn->top, w, h, shown);
    free(keep);
}

/* The pack's colours of the civilizations (its tint/KEY/COLOUR variants):
   208 + 3 x the colour's slot (red, white, green, blue, yellow, cyan,
   orange, purple: civ_colors), for a palette colour the game gives one;
   -1 for another. */
static int pack_tint(int colour)
{
    int k;
    for (k = 0; k < 8; k++)
        if ((civ_primary_colors_block.civ_colors[k].primary & 255) == (colour & 255) ||
            (civ_primary_colors_block.civ_colors[k].mid & 255) == (colour & 255))
            return 208 + 3 * k;
    return -1;
}

/* A sprite of the map recoloured by the game after it is drawn (the
   occupied airbase's aircraft: the marker colour 6 in the owner's): the area synced before, the sprite drawn in
   that colour (its pack's variant tint/KEY/COLOUR, or tinted flat), and
   the recoloured pixels taken as shown after.  Only for a sprite drawn
   from a picture of the pack. */
static int recolour_drawn;

void hd_recolour_begin(Sprite *sprite, DibSurface *surface, const RECT *area, int colour, int flat)
{
    recolour_drawn = 0;
    if (!hd_art || !hd_sprite_pictured(sprite, sprite->data_handle))
        return;
    hd_bits_sync((void *)surface->bits, area->left, area->top, area->right - area->left, area->bottom - area->top);
    if (flat)
        recolour_drawn = hd_sprite_as(sprite, sprite->data_handle, NULL, 0, colour);
    else if (pack_tint(colour) >= 0) {
        hd_sprite_recolor(sprite, sprite->data_handle, 8, pack_tint(colour));
        recolour_drawn = 1;
    }
}

void hd_recolour_end(DibSurface *surface, const RECT *area)
{
    if (!recolour_drawn)
        return;
    hd_sprite_as(NULL, 0, NULL, 0, -1);
    hd_bits_shown((void *)surface->bits, area->left, area->top, area->right - area->left, area->bottom - area->top);
    recolour_drawn = 0;
}

/* The diplomacy room: the original's rooms show the civilization's colour
   in a palette entry (0x91) of their banners; the pack's rooms have no
   such banner, so its flag (faction/flag, in the civilization's colour:
   tint/faction/flag/COLOUR) is drawn on the room's plane in its corner, as
   Peter Johnson's hd_diplomacy_flag (PR #11) does. */
void hd_diplomacy_flag(DibSurface *room, int colour)
{
    char key[40];
    if (!hd_art || pack_tint(colour) < 0)
        return;
    snprintf(key, sizeof key, "tint/faction/flag/%d", pack_tint(colour));
    hd_picture((void *)room->bits, key, 24, 16, 28, 44, 1);
}

/* A test (harness/hd/hd-redraw.txt, after Peter Johnson's map redraw
   probe and tools/test_map_redraw.py in PR #11): a trireme, alone and
   stacked (its second shield reaches outside its sprite), slid from the
   map's centre in each of the 8 directions; after each slide the plane
   around the centre must be what a full render gave (no trail, gap or
   seam left), but near the game's own pixels that are not what they were
   (the game itself redraws a little otherwise at some zooms).  Logs its
   result (hook.log): "map redraw probe: zoom Z: N of 16 slides left
   pixels"; the game's state is restored.  The script waits for its end (replay_hold). */
void hd_map_redraw_probe(void)
{
    MapView *m = &map_views_block.map_view_recs[0];
    DummyUnit saved_dummy = dummy_unit;
    u32 saved_options = game.game_options;
    RECT r;
    unsigned int *clean;
    unsigned char *clean8, *now8, *skip;
    int x = m->center_x, y = m->center_y, unit, stacked, dir, bad = 0, worst = 0, n, w, h, i;
    int reach_x = 64 * (8 + m->zoom) / 8, reach_y = 48 * (8 + m->zoom) / 8;
    long skipped = 0;
    BlinkState saved_blink = blink;
    i16 saved_active = game.active_unit;
    if (!hd_on)
        return;
    game.game_options |= OPT_FAST_PIECE_SLIDE;
    replay_hold++;
    /* the map whole, without an active unit (its blinking) */
    blink.unit_active = 0;
    game.active_unit = -1;
    m->draw_all_tiles(human_civ[0]);
    m->tile_radius_rect(x, y, 2, &r);
    if (r.left < 0) r.left = 0;
    if (r.top < 0) r.top = 0;
    if (r.right > (int)m->client_width) r.right = (int)m->client_width;
    if (r.bottom > (int)m->client_height) r.bottom = (int)m->client_height;
    w = r.right - r.left;
    h = r.bottom - r.top;
    clean = hd_debug_capture((void *)m->bits, r.left, r.top, w, h);
    clean8 = hd_debug_capture_pixels((void *)m->bits, r.left, r.top, w, h);
    skip = (unsigned char *)malloc((size_t)w * h);
    unit = unit_make_dummy(UNIT_TRIREME, human_civ[0]);
    for (stacked = 0; stacked <= 1; stacked++)
        for (dir = 0; dir < 8; dir++) {
            const char *how = stacked ? "stacked" : "alone";
            dummy_unit.prev_in_stack = stacked ? 0 : -1;
            map_animate_unit_move(unit, x, y, dir, -1, -1);
            now8 = hd_debug_capture_pixels((void *)m->bits, r.left, r.top, w, h);
            /* where the game's own pixels differ (its scaled sprites drawn
               clipped round otherwise at some zooms; a unit drawn or not),
               a unit's frame round them is not compared */
            memset(skip, 0, (size_t)w * h);
            for (i = 0; clean8 && now8 && i < w * h; i++)
                if (clean8[i] != now8[i]) {
                    int px = i % w, py = i / w, sx, sy;
                    for (sy = py - reach_y; sy <= py + reach_y; sy++)
                        for (sx = px - reach_x; sx <= px + reach_x; sx++)
                            if (sx >= 0 && sy >= 0 && sx < w && sy < h)
                                skip[sy * w + sx] = 1;
                }
            free(now8);
            for (i = 0; i < w * h; i++)
                skipped += skip[i];
            n = hd_debug_compare((void *)m->bits, r.left, r.top, w, h, clean, 8, skip);
            if (n == 0)
                continue;
            if (!bad && getenv("CIV2_HD_PROBE_DUMP")) {
                char path[256];
                unsigned int *now = hd_debug_capture((void *)m->bits, r.left, r.top, w, h);
                snprintf(path, sizeof path, "%s/probe-%d-before.ppm", getenv("CIV2_HD_PROBE_DUMP"), (int)m->zoom);
                hd_debug_dump(path, clean, w * hd_density, h * hd_density);
                snprintf(path, sizeof path, "%s/probe-%d-after-%d-%d.ppm", getenv("CIV2_HD_PROBE_DUMP"), (int)m->zoom,
                         stacked, dir);
                hd_debug_dump(path, now, w * hd_density, h * hd_density);
                free(now);
            }
            bad++;
            if (n > worst)
                worst = n;
            plat_log("map redraw probe: zoom %d, %s, direction %d: %d pixels differ\n", (int)m->zoom, how, dir, n);
        }
    free(clean);
    free(clean8);
    free(skip);
    dummy_unit = saved_dummy;
    game.game_options = saved_options;
    blink = saved_blink;
    game.active_unit = saved_active;
    replay_hold--;
    plat_log("map redraw probe: zoom %d: %d of 16 slides left pixels (most %d; %d%% of the area compared)\n",
             (int)m->zoom, bad, worst, (int)(100 - skipped * 100 / ((long)w * h * 16)));
}

/* A test (harness/hd/hd-disorder.txt): the city at (x, y) in civil
   disorder, its marker drawn in place of the size box; the map centred on
   it at zoom (-7 to 8) and redrawn. */
void hd_test_city_disorder(int x, int y, int zoom)
{
    MapView *m = &map_views_block.map_view_recs[0];
    int city = city_at(x, y);
    if (city < 0)
        return;
    cities[city].flags |= CITYF_DISORDER;
    m->zoom = (i16)zoom;
    replay_hold++;      /* (the redraw idles: the script waits for it) */
    m->center_on(x, y);
    replay_hold--;
}
