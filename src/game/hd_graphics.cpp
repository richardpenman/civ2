/* The game's unchanged logical sprite and resource contracts over Atlas RGBA. */
#include "../platform/hd.h"
#include "auto_common.h"

EXTERN_C int hd_resource(int id)
{
    int image = hd_module_image((uintptr_t)err_report_ctx_block.app_hinstance, id, "gifs");
    for (unsigned i = 0; !image && i < res_dll_module_count; i++)
        image = hd_module_image((uintptr_t)err_report_ctx_block.res_dll_modules[i], id, "gifs");
    return image;
}

int hd_load_surface(DibSurface *s, int image, GamePalette *pal)
{
    int w, h;
    if (!image)
        return 0;
    hd_size(image, &w, &h);
    const unsigned char *indices = hd_indices(image);
    if (!indices)
        return 0;
    s->create(w, h);
    if (pal) {
        pal->set_rgb(0, 256, (iptr)hd_palette());
        s->set_palette((iptr)pal);
    }
    for (int y = 0; y < h; y++)
        memcpy((void *)s->pixel_ptr(0, y), indices + y * w, w);
    hd_attach((void *)s->bits, image);
    return 1;
}

int hd_encode_sprite(Sprite *s, DibSurface *surface, RECT *r, int key)
{
    RECT clipped;
    if (!IntersectRect(&clipped,r,(RECT *)&surface->clip_left))
        return 0;
    r = &clipped;
    int w = r->right - r->left, h = r->bottom - r->top;
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096 || r->left < 0 || r->top < 0 ||
        r->right > (int)surface->pixel_width || r->bottom > (int)surface->pixel_height)
        return 0;
    s->free();
    SetRect((RECT *)&s->frame_rect, 0, 0, w, h);
    SetRect((RECT *)&s->bounds_rect, 0, 0, w, h);
    if (key == -1) key = surface->get_pixel(r->left,r->top);
    s->key_color = key >= 0 ? key : 0;
    s->offset_x = s->offset_y = 0;
    int handle = mem_handle_alloc((w + 8) * h + 8);
    unsigned char *out = (unsigned char *)mem_handle_lock(handle);
    if (!out)
        return 0;
    hd_sprite_capture(s, (void *)surface->bits, r->left, r->top, w, h, key);
    bool atlas = hd_source((void *)surface->bits) != 0;
    for (int y = 0; y < h; y++) {
        const unsigned char *row = (const unsigned char *)surface->pixel_ptr(r->left,r->top+y);
        int start = 0, end = w;
        if (key >= 0) {
            while (start < end && (row[start] == key || (atlas && row[start] == 7))) start++;
            while (end > start && (row[end-1] == key || (atlas && row[end-1] == 7))) end--;
        }
        int length = end-start;
        memcpy(out,&start,4);
        memcpy(out+4,&length,4);
        memcpy(out+8,row+start,length);
        if (atlas && key >= 0 && key != 7)
            for (int x = 0; x < length; x++)
                if (out[8+x] == 7) out[8+x] = (unsigned char)key;
        out += length+8;
    }
    memset(out, 0, 8);
    mem_handle_unlock(handle);
    return handle;
}

void hd_game_windows_grey(int on)
{
    for (int view = 0; view < 8; view++)
        if (view == 0 || map_views_block.map_view_recs[view].open)
            hd_surface_grey((void *)map_views_block.map_view_recs[view].bits,on);
    hd_surface_grey((void *)((DibSurface *)&status_wnd.window)->bits,on);
    hd_surface_grey((void *)((DibSurface *)&minimap_state.window)->bits,on);
    hd_surface_grey((void *)((DibSurface *)&city_wnd.window)->bits,on);
}

void hd_bind_map_masks()
{
    /* These sprites are manufactured from indexed dither masks by the original
       loader. Bind the authored quarter alpha masks after that manufacturing;
       map topology, ordering and zoom gates remain unchanged. */
    for (int t = 0; t < 13; t++)
        for (int q = 0; q < 4; q++) {
            char key[64];
            snprintf(key, sizeof key, "dither/%d/%d", t, q);
            hd_sprite_bind((Sprite *)&map_cursor_sprite_block.terrain_dither_sprites + t * 4 + q, key);
        }
    for (int q = 0; q < 4; q++) {
        char key[32];
        snprintf(key, sizeof key, "fog/%d", q);
        hd_sprite_bind((Sprite *)&map_cursor_sprite_block.unexplored_dither_sprites + q, key);
    }
}
