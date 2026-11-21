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

/* The leader's portrait in the diplomacy room (DiploLeaderWindow::
   init_portrait: mk.dll 220 + the civilization's leader_portrait, the tribe's
   slot, + 21 for a woman).  The pack's 42 portraits at those ids are regional
   archetypes of its own (its manifest's leaders.leader-00 to -41: mk.dll 220 +
   N), not the tribes' leaders in the original's order, so each tribe and
   gender is given the one that fits it best by region, then era; several
   tribes share one.  0: the original's portrait, enlarged.  The portrait's
   number, not the tribe's name, chooses, as the game's own: the tribe's slot
   (a scenario's tribe in a slot keeps the slot's portrait) or the face a
   custom tribe chose (PortraitDialog).  The pack's crop by its own id is
   never drawn (hd.c's NOT_CROPPED). */
static const struct {
    const char *tribe;
    short male, female;         /* mk.dll id of the pack's portrait (0: none) */
} hd_portraits[21] = {
    {"Romans", 220, 241},        /* Mediterranean civic leaders, ivory mantles (her laurel wreath) */
    {"Babylonians", 228, 249},   /* Near Eastern rulers, blue robes */
    {"Germans", 232, 253},       /* medieval European rulers, circlets, blue velvet */
    {"Egyptians", 223, 244},     /* North African, linen headcloths, ochre robes */
    {"Americans", 236, 257},     /* industrial-era civic leaders, a suit, a jacket */
    {"Greeks", 233, 254},        /* Byzantine: a grey-bearded statesman, purple robes */
    {"Indians", 224, 245},       /* South Asian rulers, saffron turban, sari */
    {"Russians", 235, 256},      /* early modern northern European, navy coats */
    {"Zulus", 0, 0},             /* the original's: the pack's Africans are West African */
    {"French", 235, 256},        /* early modern: grey powdered hair, cravat, lace */
    {"Aztecs", 226, 247},        /* Central American, jade collars, woven cloaks */
    {"Chinese", 222, 243},       /* East Asian scholar-rulers, teal silk robes */
    {"English", 234, 255},       /* Renaissance: black caps, brocade, her ruff */
    {"Mongols", 229, 250},       /* steppe leaders, fur-trimmed coats */
    {"Celts", 221, 242},         /* northern European chieftains, red hair, green cloaks */
    {"Japanese", 237, 258},      /* modern East Asian diplomats */
    {"Vikings", 221, 242},       /* northern European chieftains (the Celts') */
    {"Spanish", 234, 255},       /* Renaissance (the English') */
    {"Persians", 228, 249},      /* Near Eastern (the Babylonians'): her silver circlet */
    {"Carthaginians", 223, 244}, /* North African (the Egyptians') */
    {"Sioux", 239, 260},         /* Indigenous North American, braids, blue wool */
};

void hd_diplomacy_portrait(Sprite *sprite, int portrait)
{
    char key[48];
    int id = portrait >= 0 && portrait < 42 ?
                 (portrait < 21 ? hd_portraits[portrait].male : hd_portraits[portrait - 21].female) : 0;
    if (!hd_art || !id)
        return;
    snprintf(key, sizeof key, "crop/resource/mk.dll/gifs/%d/1,1,225,275", id);
    hd_sprite_bind(sprite, sprite->data_handle, key, 0);
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

/* A test (harness/hd/hd-pictures.txt): the announcement with tiles.dll's
   picture (70 to 77: a city captured, in disorder, celebrating, founded;
   each with a modern one, + 1) for the city, as the game shows it. */
void hd_test_city_picture(int picture, int city)
{
    static const char *const sections[] = {str_citycapture_2, str_disorder, str_weloveking, str_founded};
    if (picture < 70 || picture > 77 || city < 0 || city >= (int)game.num_cities)
        return;
    text_set_string_arg_b(0, (const char *)&cities[city].name);
    text_set_string_arg_b(1, (const char *)&cities[city].name);
    popup_city_picture_zoom(sections[(picture - 70) / 2], city, (uint)picture, 1, cities[city].owner);
}

/* The spaceship (SpaceshipWindow::render_ship, docs/HD-GRAPHICS.md): the
   ship drawn whole from the pack's pieces (ship/..., side views, the ship's
   axis sloping down to the right as they are drawn) in OpenCiv2's own
   layout, over the original's picture, which the game still draws in its
   pixels.  The stern is a cluster of up to eight rows, each a propulsion
   unit (its engine firing once launched) and a fuel tank, filled from the
   middle outwards; the frame grows from the tanks towards the bow, four
   truss rails a bay at a time (four structurals a bay: a pair of
   components needs four more), ending in a docking hub; the habitation
   modules run along above the frame, the life support modules below, the
   solar panels spread above and below them.  A part built but not yet
   supported by the frame (beyond spaceship_part_count, which the original
   draws "unconnected") is a faded grey outline; a part not built is not
   drawn.  The scale is that of the complete ship, so the ship grows in
   place, centred in the picture.  Units: the pack's pieces are 96 x 80
   game pixels at scale 1. */
struct ShipPiece {
    int z;                      /* drawn by z, back to front */
    const char *key;
    double u, v, scale, shear;  /* along the axis, across it (both game pixels) */
    int ghost;
};

enum { SHIP_MAX_PIECES = 128 };
static const double SHIP_SLOPE = .28, SHIP_TUBE = 62, SHIP_ROW = 21, SHIP_BAY = 25.5, SHIP_TRUSS = .44;
static const int SHIP_PIECE_W = 96, SHIP_PIECE_H = 80;
static const int ship_rect[4] = {0xaf, 99, 0xaf + 0x1c9, 99 + 0x130};     /* where the game draws the ship */
static const double ship_box[4] = {236, 104, 626, 398};                  /* the ship's own, clear of the panel */

static int ship_layout(const int *n, const int *built, int launched, ShipPiece *p)
{
    static const int rows[8] = {3, 4, 2, 5, 1, 6, 0, 7};    /* the stern's rows, from the middle */
    static const int rails[4] = {3, 4, 2, 5};               /* the frame's, the four middle rows */
    static const double solar[4][2] = {{1.2, -3.7}, {1.2, 3.7}, {3.0, -3.7}, {3.0, 3.7}};
    const double u0 = SHIP_TUBE * .5 + SHIP_BAY * .5 - 6, front = u0 - SHIP_BAY * .5;
    int k = 0, j;
#define PUT(Z, KEY, U, V, SC, GHOST, SHEAR)                                                         \
    do {                                                                                          \
        ShipPiece q = {(Z), (KEY), (U), (V), (SC), (SHEAR), (GHOST)};                             \
        if (k < SHIP_MAX_PIECES)                                                                  \
            p[k++] = q;                                                                           \
    } while (0)
    for (j = 0; j < n[1] || j < n[2]; j++) {
        double v = (rows[j] - 3.5) * SHIP_ROW;
        if (j < n[1])
            PUT(rows[j] * 10 + 1, launched ? "ship/engine-active" : "ship/propulsion", -SHIP_TUBE * .92, v, 1.0,
                j >= built[1], 0.0);
        if (j < n[2])
            PUT(rows[j] * 10 + 2, "ship/fuel", 0.0, v, 1.0, j >= built[2], 0.0);
    }
    for (j = 0; j < n[0]; j++)
        PUT(rails[j % 4] * 10 + 3, "ship/structural-spine", u0 + j / 4 * SHIP_BAY, (rails[j % 4] - 3.5) * SHIP_ROW,
            SHIP_TRUSS, 0, SHIP_SLOPE - .364);     /* the truss drawn at .364: given the axis' slope */
    if (n[0])
        PUT(47, "ship/structural-dock", u0 + (n[0] + 3) / 4 * SHIP_BAY + SHIP_BAY * .1, 0.0, 1.25, 0, 0.0);
    for (j = 0; j < n[3]; j++)
        PUT(15, "ship/habitation", front + SHIP_TUBE * (j + .5), -2.6 * SHIP_ROW, 1.0, j >= built[3], 0.0);
    for (j = 0; j < n[4]; j++)
        PUT(65, "ship/life-support", front + SHIP_TUBE * (j + .5), 2.6 * SHIP_ROW, 1.0, j >= built[4], 0.0);
    for (j = 0; j < n[5]; j++)
        PUT(solar[j][1] < 0 ? 5 : 85, "ship/solar", front + solar[j][0] * SHIP_TUBE, solar[j][1] * SHIP_ROW, 1.0,
            j >= built[5], 0.0);
#undef PUT
    return k;
}

/* The pieces' extent (game pixels at scale 1, the axis' origin at 0, 0). */
static void ship_bounds(const ShipPiece *p, int k, double *b)
{
    int i;
    b[0] = b[1] = 1e9;
    b[2] = b[3] = -1e9;
    for (i = 0; i < k; i++) {
        double x = p[i].u, y = p[i].u * SHIP_SLOPE + p[i].v, hw = SHIP_PIECE_W * p[i].scale / 2,
               hh = SHIP_PIECE_H * p[i].scale / 2;
        if (x - hw < b[0]) b[0] = x - hw;
        if (y - hh < b[1]) b[1] = y - hh;
        if (x + hw > b[2]) b[2] = x + hw;
        if (y + hh > b[3]) b[3] = y + hh;
    }
}

/* The ship's cover (where it is drawn) for the starfield. */
static unsigned char ship_cover[0x1c9 * 0x130];
static const void *ship_cover_of;

void hd_spaceship(SpaceshipWindow *w)
{
    static const int full[6] = {39, 8, 8, 4, 4, 4};
    ShipPiece p[SHIP_MAX_PIECES];
    void *bits = (void *)w->surface_wnd.bits;
    int n[6], built[6], k, i, pw, ph, launched = w->launch_state == 3;
    double b[4], scale, ox, oy;
    ship_cover_of = NULL;
    if (!hd_art || !hd_picture_size("ship/structural-spine", &pw, &ph))
        return;     /* a pack without the pieces: the original's ship, enlarged */
    for (i = 0; i < 6; i++) {
        n[i] = civs[w->ship_civ].spaceship_parts[i];
        if (n[i] < 0) n[i] = 0;
        if (n[i] > full[i]) n[i] = full[i];
        built[i] = spaceship_part_count(w->ship_civ, i);
    }
    /* the scale of the complete ship, fitted in the box */
    k = ship_layout(full, full, launched, p);
    ship_bounds(p, k, b);
    scale = (ship_box[2] - ship_box[0]) / (b[2] - b[0]);
    if (scale > (ship_box[3] - ship_box[1]) / (b[3] - b[1]))
        scale = (ship_box[3] - ship_box[1]) / (b[3] - b[1]);
    /* the background: the window's picture, or space once launched (the
       starfield's colour, its stars as the game drew them) */
    if (launched) {
        static unsigned char stars[0x1c9 * 0x130];
        const int rw = ship_rect[2] - ship_rect[0];
        memset(stars, 0, sizeof stars);
        hd_picture_fill(bits, ship_rect[0], ship_rect[1], rw, ship_rect[3] - ship_rect[1], hd_bits_colour(bits, 10));
        for (i = 0; i < 80; i++) {
            int x, y;
            if (!w->stars[i].depth)
                continue;
            x = w->stars[i].x / w->stars[i].depth + 0x140;
            y = w->stars[i].y / w->stars[i].depth + 0xf0;
            if (x >= ship_rect[0] && x < ship_rect[2] && y >= ship_rect[1] && y < ship_rect[3] &&
                w->surface_wnd.get_pixel(x, y) == 0x29)
                stars[(y - ship_rect[1]) * rw + x - ship_rect[0]] = 1;
        }
        hd_bits_redrawn_mask(bits, ship_rect[0], ship_rect[1], rw, ship_rect[3] - ship_rect[1], stars);
    } else
        hd_picture_part(bits, "resource/ss.dll/gifs/499", 0, 0, 640, 480, 0, 255, 0, ship_rect, NULL);
    memset(ship_cover, 0, sizeof ship_cover);
    k = ship_layout(n, built, launched, p);
    if (k) {
        /* back to front (a stable sort: few pieces) */
        for (i = 1; i < k; i++) {
            ShipPiece q = p[i];
            int j = i;
            for (; j > 0 && p[j - 1].z > q.z; j--)
                p[j] = p[j - 1];
            p[j] = q;
        }
        ship_bounds(p, k, b);
        ox = (ship_box[0] + ship_box[2]) / 2 - (b[0] + b[2]) / 2 * scale;
        oy = (ship_box[1] + ship_box[3]) / 2 - (b[1] + b[3]) / 2 * scale;
        for (i = 0; i < k; i++) {
            double dw = SHIP_PIECE_W * p[i].scale * scale, dh = SHIP_PIECE_H * p[i].scale * scale;
            hd_picture_part(bits, p[i].key, ox + p[i].u * scale - dw / 2,
                            oy + (p[i].u * SHIP_SLOPE + p[i].v) * scale - dh / 2, dw, dh, p[i].shear,
                            p[i].ghost ? 100 : 255, p[i].ghost, ship_rect, ship_cover);
        }
    }
    hd_bits_shown(bits, ship_rect[0], ship_rect[1], ship_rect[2] - ship_rect[0], ship_rect[3] - ship_rect[1]);
    ship_cover_of = bits;
}

/* The starfield's stars, moved by the game (spaceship_wnd_starfield_tick):
   those under the ship stay behind it. */
void hd_spaceship_stars(SpaceshipWindow *w)
{
    void *bits = (void *)w->surface_wnd.bits;
    if (hd_art && ship_cover_of == bits)
        hd_bits_shown_mask(bits, ship_rect[0], ship_rect[1], ship_rect[2] - ship_rect[0],
                           ship_rect[3] - ship_rect[1], ship_cover);
}

/* A test (harness/hd/hd-spaceship.txt): the player's spaceship with these
   parts built (structurals, the two kinds of components, the three kinds
   of modules), in its window; s negative: -s structurals, the ship
   launched. */
void hd_test_spaceship(int s, int c1, int c2, int m1, int m2, int m3)
{
    const int n[6] = {s < 0 ? -s : s, c1, c2, m1, m2, m3};
    int i;
    for (i = 0; i < 6; i++)
        civs[human_civ[0]].spaceship_parts[i] = (i16)n[i];
    if (s < 0)
        civs[human_civ[0]].spaceship_flags |= SHIP_LAUNCHED;
    spaceship_wnd_run(human_civ[0]);
}

/* OpenCiv2's own heralds (docs/HD-GRAPHICS.md, "Your own heralds";
   data/native/hd-extra/heralds.txt): DiploLeaderWindow::init_video asks
   whether the tribe of this code (KINGS\HRLDxxx.AVI's xxx) has one, with
   HD graphics.  If so (1), the video's window goes where it stands (x, y,
   the game's pixels of the room) and the game's table of the herald's
   segments for that tribe (leader_anim_frames: greeting, idle, pleased,
   refusal, angry) takes its frames, so that the game plays it as it plays
   the CD's video, from the same calls at the same moments; the CD's file
   need not be there (src/platform/video.c opens the own herald in its
   place).  Else (0) the tribe's segments are the original's again. */
int hd_herald(const char *code, int tribe, i32 *x, i32 *y)
{
    static i32 original[26][5][2];
    static int saved;
    HdHerald h;
    char path[1400];
    if (tribe < 0 || tribe >= 26)
        return 0;
    if (!saved) {
        memcpy(original, leader_anim_frame_starts_block.leader_anim_frames, sizeof original);
        saved = 1;
    }
    if (!hd_herald_find(code, &h, path, sizeof path)) {
        memcpy(leader_anim_frame_starts_block.leader_anim_frames[tribe], original[tribe], sizeof original[tribe]);
        return 0;
    }
    for (int s = 0; s < 5; s++) {
        leader_anim_frame_starts_block.leader_anim_frames[tribe][s][0] = h.seg[s][0];
        leader_anim_frame_starts_block.leader_anim_frames[tribe][s][1] = h.seg[s][1];
    }
    *x = h.x;
    *y = h.y;
    return 1;
}
