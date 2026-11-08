/* Atlas runtime boundary. Logical game coordinates never include display density. */
#ifndef CIV2_HD_H
#define CIV2_HD_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
extern int hd_enabled, hd_density;
extern int hd_fade_grey;
extern int hd_gpu_active;
extern int hd_redraw_active;
void hd_redraw_begin(void);
void hd_redraw_end(void);
int hd_redraw_poll_due(void);
/* A flat indexed background: batch writes, reuse its texture if unchanged.
   Complete the batch before any HD sprite, copy, or message dispatch. */
void hd_indexed_begin(void *bits);
void hd_indexed_end(void *bits);
void hd_start_gpu(void);
void hd_init(int classic, int test);
int hd_find(const char *key);
int hd_variant(int image, int color);
void hd_size(int image, int *w, int *h);
const unsigned char *hd_indices(int image);
const unsigned char *hd_palette(void);
const unsigned char *hd_rgba(int image, int *width, int *height);
int hd_gdi_bitmap(uintptr_t dc, uintptr_t bitmap, int x, int y, int w, int h, int sx, int sy);
int hd_sprite_image(const void *sprite);
int hd_resource(int id);
int hd_module_image(uintptr_t module, int id, const char *type);
int hd_file(const char *name);
int hd_virtual_file(const char *name);
void hd_attach(void *bits, int image);
int hd_source(void *bits);
int hd_crop(int image, int x, int y, int w, int h);
void hd_draw(void *bits, int image, int x, int y, int w, int h, int left, int top, int right, int bottom,
             int tint);
void hd_changed(void *bits, int x, int y, int w, int h);
void hd_copy(void *dst, void *src, int x, int y, int sx, int sy, int w, int h);
void hd_commit(void *bits);
void hd_commit_rect(void *bits, int x, int y, int w, int h);
void hd_sprite_forget(const void *sprite);
void hd_sprite_capture(const void *sprite, void *bits, int x, int y, int w, int h, int key);
int hd_sprite_draw(const void *sprite, void *bits, int x, int y, int w, int h, int left, int top, int right,
                   int bottom, int tint);
int hd_sprite_draw_keyed(const void *sprite, void *bits, int x, int y, int w, int h, int left, int top,
                         int right, int bottom, int tint, int key);
void hd_sprite_greyscale(const void *sprite, int on);
void hd_surface_grey(void *bits, int on);
void hd_grey_pixels(uint32_t *pixels, uint64_t count, int strength);
void hd_sprite_tint(const void *sprite, int old_color, int new_color);
void hd_sprite_copy(const void *dst, const void *src);
void hd_sprite_bind(const void *sprite, const char *key);
/* Platform-only helpers (Bitmap is not part of the game's data layout). */
struct Bitmap;
void hd_draw_bitmap(struct Bitmap *bitmap, int image, int x, int y, int w, int h, int left, int top,
                    int right, int bottom, int tint);
void hd_bitmap_destroy(struct Bitmap *b);
void hd_bitmap_commit(struct Bitmap *b);
void hd_bitmap_commit_rect(struct Bitmap *b, int x, int y, int w, int h);
void hd_bitmap_changed(struct Bitmap *b, int x, int y, int w, int h);
void hd_bitmap_invert(struct Bitmap *b, int x, int y, int w, int h);
void hd_sync(struct Bitmap *b);
void hd_sync_rect(struct Bitmap *b, int x, int y, int w, int h);
void hd_pixel(struct Bitmap *b, int x, int y, uint32_t rgb);
void hd_transfer(struct Bitmap *dst, struct Bitmap *src, int x, int y, int w, int h, int sx, int sy, int sw,
                 int sh, int l, int t, int r, int b);
const uint32_t *hd_pixels(struct Bitmap *b);
/* Explicit boundaries for rare CPU effects/captures. Physical pixel rectangles. */
void hd_read_region(struct Bitmap *b, int x, int y, int w, int h);
void hd_upload_region(struct Bitmap *b, int x, int y, int w, int h);
void hd_composite(struct Bitmap *dst, struct Bitmap *src, int x, int y, int l, int t, int r, int bot);
void hd_composite_camera(struct Bitmap *dst, struct Bitmap *src,int sx,int sy,int sw,int sh,
                         double x,double y,double w,double h,int l,int t,int r,int bot);
int hd_gpu_text(struct Bitmap *b, const unsigned char *coverage, int sw, int sh,
                int x, int y, int w, int h, int l, int t, int r, int bot, uint32_t color);
/* Replay diagnostics only: explicitly reads back a logical rectangle.
 * The caller frees the capture; compare borrows it without modification. */
uint32_t *hd_debug_capture_rect(void *bits,int x,int y,int w,int h);
int hd_debug_compare_rect(void *bits,int x,int y,int w,int h,const uint32_t *reference,int tolerance);
void hd_gpu_text_begin(void);
void hd_gpu_text_end(void);
#ifdef __cplusplus
}
#endif
#endif
