/* Atlas's non-map presentation adapters. Selection, counts, localization,
 * callbacks and random-number use remain owned by the game. */
#include <math.h> /* Before auto_common.h's legacy 32-bit type macros. */
#include "auto_common.h"
#include "hd_windows.h"
#include "hd_layouts.h"

static void art(DibSurface *s, const char *key, int x, int y, int w, int h, bool contain = false)
{
    int id = hd_find(key), iw, ih;
    if (!id)
        return;
    hd_size(id, &iw, &ih);
    if (contain && iw && ih) {
        int nw = w, nh = (int)((double)ih * w / iw);
        if (nh > h) {
            nh = h;
            nw = (int)((double)iw * h / ih);
        }
        x += (w - nw) / 2;
        y += (h - nh) / 2;
        w = nw;
        h = nh;
    }
    hd_draw((void *)s->bits, id, x, y, w, h, s->clip_left, s->clip_top, s->clip_right, s->clip_bottom, -1);
}
static void box(DibSurface *s, int x, int y, int w, int h, int color)
{
    RECT r = {x, y, x + w, y + h};
    s->fill_rect((iptr)&r, color);
}
void hd_diplomacy_flag(DibSurface *s, int color)
{
    /* The original audience room changes a palette entry for its banner.
       RGBA rooms carry the same ownership cue as a separate flag. */
    int id = hd_variant(hd_find("faction/flag"), color);
    if (id)
        hd_draw((void *)s->bits,id,24,16,28,44,
                s->clip_left,s->clip_top,s->clip_right,s->clip_bottom,-1);
}
static void label(DibSurface *s, const char *text, int x, int y, int w, int h, int size = 13)
{
    GameFont f;
    f.init_empty();
    f.set(1, size);
    RECT r = {x, y, x + w, y + h};
    gfx_set_text_color_index(26);
    s->draw_text_in_rect((iptr)&f, text, &r, 0);
    f.destroy();
}
void hd_report_background()
{
    if (!hd_enabled)
        return;
    box((DibSurface *)&advisor_state.window, advisor_state.client_x, advisor_state.client_y,
        advisor_state.client_width, advisor_state.window.clip_bottom - advisor_state.client_y, 26);
}
void hd_report_row(int y, int height, int row)
{
    if (!hd_enabled)
        return;
    /* Bands follow the live row geometry, including scrolling and font size. */
    DibSurface *s = (DibSurface *)&advisor_state.window;
    box(s, advisor_state.client_x + 2, y, advisor_state.client_width - 4, height, row & 1 ? 26 : 35);
}

bool hd_advance(DibSurface *s, int advance, int x, int y, int w, int h)
{
    if (!hd_enabled)
        return false;
    char key[40];
    snprintf(key, sizeof key, "advance/%d", advance);
    if (!hd_find(key))
        return false; /* explicit era/category fallback at caller */
    art(s, key, x, y, w, h, true);
    return true;
}

/* Plain advance dialogs can also contain ordinary sprites (e.g. the obsolete
 * wonder). Tag only advance entries; keep their layout sprite and public ID. */
static void dialog_draw_advance_sprite(i32 sprite,iptr surface,i32,i32 data,i32 x,i32 y)
{
    Sprite *s=(Sprite *)(iptr)sprite;
    if (data>0 && data<=100 && hd_advance((DibSurface *)surface,data-1,
        x+gfx_scale_length(s->offset_x,0,0),y+gfx_scale_length(s->offset_y,0,0),
        gfx_scale_length(s->width(),0,0),gfx_scale_length(s->height(),0,0)))return;
    RECT bounds;
    s->draw((iptr)&bounds,surface,x,y);
}
void Dialog::add_advance(int advance,iptr sprite_id)
{
    Sprite *s=(Sprite *)government_sprites_block.advance_category_icon_sprites+
        (int)advances[advance].category*4+(int)advances[advance].epoch;
    DialogSprite *entry=add_sprite((iptr)s,sprite_id,0);
    if (hd_enabled) {
        entry->field_04=advance+1;
        set_sprite_draw_fn(dialog_draw_advance_sprite);
    }
}

int hd_military(DiploLeaderWindow *v)
{
    DibSurface *s = (DibSurface *)v->leader_surface;
    int rank = int_clamp(game.power_rank[v->leader_civ], 1, 7);
    char key[32];
    snprintf(key, sizeof key, "military/%d", (int)v->style_era);
    box(s, 24, 107, 144, 251, 16);
    art(s, key, 50, 166, 92, 85, true);
    label(s, label_string(label_table.string_ids[0xe3 + rank]), 32, 262, 128, 30, 18);
    for (int i = 0; i < 6; i++)
        box(s, 37 + i * 20, 305, 18, 8, i < rank - 1 ? 118 : 18);
    return 1;
}
void hd_knowledge(DiploLeaderWindow *v, int advance, int y, GameFont &f)
{
    DibSurface *s = (DibSurface *)v->leader_surface;
    box(s, 472, y, 144, 31, 16);
    if (!hd_advance(s, advance, 483, y + 2, 27, 27)) {
        char key[96];
        int category = advances[advance].category, rows[] = {32, 63, 94, 1, 125};
        snprintf(key, sizeof key, "crop/resource/mk.dll/gifs/215/%d,%d,108,30", 1 + 109 * v->style_era,
                 rows[int_clamp(category, 0, 4)]);
        art(s, key, 483, y + 2, 27, 27, true);
    }
    RECT r = {515, y, 609, y + 30};
    gfx_set_text_color_index(26);
    s->draw_text_rect((iptr)&f, label_string(advances[advance].name), (iptr)&r, 0);
}

void hd_stockpile(DibSurface *s, const RECT &r, int value, int capacity, int threshold, const char *role)
{
    const int w = r.right - r.left, h = r.bottom - r.top, pad = w / 12, bar = h / 7 > 8 ? h / 7 : 8;
    char key[64], text[80];
    snprintf(key, sizeof key, "economy/%s", role);
    int icon = h / 3;
    if (icon > w / 3)
        icon = w / 3;
    art(s, key, r.left + pad, r.top + h / 6, icon, icon, true);
    snprintf(text, sizeof text, "%d / %d", value, capacity);
    label(s, text, r.left + pad + icon + 6, r.top + h / 6, w - pad * 2 - icon - 6, icon, 13);
    int x = r.left + pad, y = r.top + h * 2 / 3, bw = w - 2 * pad;
    box(s, x, y, bw, bar, 18);
    if (capacity > 0)
        box(s, x + 1, y + 1, (int)((int64_t)(bw - 2) * int_clamp(value, 0, capacity) / capacity), bar - 2,
            !strcmp(role, "food") ? 44 : 84);
    if (capacity > 0 && threshold > 0)
        box(s, x + (int)((int64_t)bw * threshold / capacity), y - 2, 2, bar + 4, 118);
}

int hd_spaceship(SpaceshipWindow *v)
{
    DibSurface *s = (DibSurface *)&v->control_list;
    box(s, 175, 99, 457, 304, 16);
    int structure = spaceship_part_count(v->ship_civ, 0);
    for (int lane = 0; lane < 4; lane++) {
        int ready = structure >= ((lane == 3) ? 39 : (lane + 1) * 10), y = 126 + lane * 72;
        box(s, 213, y - 3, 376, 6, ready ? 121 : 18);
        box(s, 403, y - 36, 4, 72, ready ? 121 : 18);
    }
    for (const HdShipSlot &slot : hd_ship_slots) {
        int built = slot.index < spaceship_part_count(v->ship_civ, slot.kind);
        art(s, built ? slot.art : "ship/empty", slot.x, slot.y, slot.w, slot.h, true);
    }
    spaceship_wnd_draw_title(v->ship_civ);
    return 1;
}

static const char *const throne_family[] = {"arch",     "rug",     "dais",   "columns",
                                            "tapestry", "brazier", "throne", "ceiling-medallion"};
static void furnishing(DibSurface *s, int slot, int level, double cx, double base, double w, double h)
{
    if (level < 1)
        return;
    char key[64];
    snprintf(key, sizeof key, "palace/%s-%d", throne_family[slot], int_clamp(level, 1, 4));
    art(s, key, (int)((cx - w / 2) * 640), (int)((base - h) * 480), (int)(w * 640), (int)(h * 480), true);
}
int hd_throne_room(ThroneRoomWindow *v, bool choices)
{
    DibSurface *s = (DibSurface *)v->throne_surface_wnd;
    art(s, "resource/pv.dll/gifs/100", 0, 0, 640, 480);
    const auto &levels = civs[v->throne_civ].throne_room;
    if (choices) {
        ClickMap *map = (ClickMap *)v->choice_clickmap;
        map->clear();
        int count = v->throne_complete ? 7 : 8;
        for (int i = 0; i < count; i++) {
            bool available = v->throne_complete ? !(levels[8] & (1 << i)) : levels[i] < 4;
            if (!available)
                continue;
            int x = i < 4 ? 5 : 565, y = 5 + (i % 4) * 114;
            char key[64];
            box(s, x, y, 70, 106, 16);
            if (v->throne_complete)
                snprintf(key, sizeof key, "palace/bonus-%d", i);
            else
                snprintf(key, sizeof key, "palace/%s-%d", throne_family[i], levels[i] + 1);
            art(s, key, x + 4, y + 4, 62, 98, true);
            map->add(i, v->throne_complete ? i + 10 : i, x, y, 70, 106);
        }
        return 1;
    }
    furnishing(s, 7, levels[7], .5, .19, .18, .16);
    furnishing(s, 0, levels[0], .5, .69, .70, .57);
    furnishing(s, 4, levels[4], .5, .57, .22, .36);
    furnishing(s, 3, levels[3], .22, .77, .21, .55);
    furnishing(s, 3, levels[3], .78, .77, .21, .55);
    furnishing(s, 1, levels[1], .5, .96, .50, .35);
    furnishing(s, 2, levels[2], .5, .84, .44, .27);
    furnishing(s, 6, levels[6], .5, .77, .22, .34);
    furnishing(s, 5, levels[5], .26, .88, .13, .25);
    furnishing(s, 5, levels[5], .74, .88, .13, .25);
    if (v->throne_complete)
        for (int i = 0; i < 7; i++)
            if (levels[8] & (1 << i)) {
                char key[40];
                snprintf(key, sizeof key, "palace/bonus-%d", i);
                art(s, key, 48 + i * 78, 384, 64, 60, true);
            }
    return 1;
}

void hd_city_panorama(CityViewWindow *v)
{
    struct Object {
        const HdCityArt *meta;
        int image, index;
        float px, py, x, y, w, h, base;
    };
    Object objects[60];
    int n = 0, gate = -1;
    const int site = int_clamp(v->terrain_kind, 0, 2);
    const HdCitySite &bounds = hd_city_sites[site];
    DibSurface *s = (DibSurface *)&v->ship_civ;
    for (unsigned i = 0; i < v->slot_count && i < 60; i++) {
        Sprite *sprite = *(Sprite **)(v->building_art_block + i * 4 + 0x463);
        if (!sprite)
            continue;
        int image = hd_sprite_image(sprite);
        for (const HdCityArt &meta : hd_city_art)
            if (hd_find(meta.crop) == image) {
                int x =
                    *(i32 *)((i8 *)&cityview_slot_x_table_block.cityview_slot_x_table + i * 8 + site * 0x1e0);
                int y =
                    *(i32 *)((i8 *)&cityview_slot_x_table_block.cityview_slot_y_table + i * 8 + site * 0x1e0);
                objects[n++] = {&meta,
                                hd_find(meta.art),
                                (int)i,
                                (float)((.06 + (x + sprite->width() / 2.) / 1280 * .87) * 1280),
                                (float)((.21 + (y + sprite->height()) / 480. * .70) * 480),
                                0,
                                0,
                                0,
                                0,
                                0};
                break;
            }
    }
    /* Reserve landmarks before housing; never consume the simulation's RNG. */
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (objects[j].meta->priority > objects[i].meta->priority ||
                (objects[j].meta->priority == objects[i].meta->priority &&
                 objects[j].meta->units > objects[i].meta->units)) {
                Object t = objects[i];
                objects[i] = objects[j];
                objects[j] = t;
            }
    for (int i = 0; i < n; i++) {
        Object &o = objects[i];
        int iw, ih;
        hd_size(o.image, &iw, &ih);
        if (!iw)
            continue;
        const float *b = site == 0 && o.meta->waterfront ? bounds.water : bounds.land;
        float best = 1e30f;
        for (int candidate = -1; candidate < 27 * 11; candidate++) {
            float cx = candidate < 0 ? o.px : (b[0] + (b[2] - b[0]) * (candidate % 27) / 26) * 1280;
            float base = candidate < 0 ? o.py : (b[1] + (b[3] - b[1]) * (candidate / 27) / 10) * 480;
            if (o.meta->wall) {
                cx = bounds.gate[0] * 1280;
                base = bounds.gate[1] * 480;
            }
            if (!o.meta->wall &&
                (cx < b[0] * 1280 || cx > b[2] * 1280 || base < b[1] * 480 || base > b[3] * 480))
                continue;
            float w = 40 * o.meta->units * (.76f + .24f * base / 480), h = w * ih / iw, x = cx - w / 2,
                  y = base - h;
            if (x < 15 || x + w > 1265 || y < 12)
                continue;
            float overlap = 0;
            for (int j = 0; j < i; j++) {
                Object &q = objects[j];
                float a = fminf(x + w, q.x + q.w) - fmaxf(x, q.x), c = fminf(base, q.base) - fmaxf(y, q.y);
                if (a > 0 && c > 0)
                    overlap += a * c / (w * h);
            }
            float score = overlap * overlap * 40 + (cx - o.px) * (cx - o.px) / (1280 * 1280) +
                          (base - o.py) * (base - o.py) / (480 * 480);
            if (score < best) {
                best = score;
                o.x = x;
                o.y = y;
                o.w = w;
                o.h = h;
                o.base = base;
            }
            if (o.meta->wall) {
                gate = i;
                break;
            }
        }
    }
    char key[64];
    const char *sites[] = {"coastal", "river", "inland"};
    if (gate >= 0) {
        snprintf(key, sizeof key, "city/wall-%s-rear", sites[site]);
        art(s, key, 0, 0, 1280, 480);
    }
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (objects[j].base < objects[i].base) {
                Object t = objects[i];
                objects[i] = objects[j];
                objects[j] = t;
            }
    for (int i = 0; i < n; i++)
        if (!objects[i].meta->wall) {
            Object &o = objects[i];
            art(s, o.meta->art, (int)o.x, (int)o.y, (int)o.w, (int)o.h);
        }
    if (gate >= 0) {
        snprintf(key, sizeof key, "city/wall-%s-front", sites[site]);
        art(s, key, 0, 0, 1280, 480);
        for (int i = 0; i < n; i++)
            if (objects[i].meta->wall) {
                Object &o = objects[i];
                art(s, o.meta->art, (int)o.x, (int)o.y, (int)o.w, (int)o.h);
            }
    }
}
