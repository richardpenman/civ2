/*
 * The planes (docs/HD-GRAPHICS.md): the game draws as always, into its
 * 8-bit surfaces; beside them some surfaces keep a true-colour plane, density
 * times larger each way, where text and the platform's controls are drawn
 * again at that resolution, and, with HD graphics, the sprites the game
 * draws from Peter Johnson's replacement art (openciv2-assets).  Whatever
 * else changes a surface's pixels shows in its plane as those pixels
 * enlarged.  On with sharp text and interface (CIV.INI's [OpenCiv2]
 * SharpText=, on by default but in Classic mode) on a screen of more than
 * one pixel per game pixel, or with HD graphics (Graphics=HD, the art
 * installed; never in Classic mode); never in replays (but CIV2_TEST_SHARP=N,
 * CIV2_TEST_HD=N) or the unit tests.  Nothing here changes what the game
 * computes, nor calls rand().
 */
#ifndef CIV2_HD_H
#define CIV2_HD_H
#ifdef __cplusplus
extern "C" {
#endif

extern int hd_on;           /* the planes in use (sharp text and interface, HD graphics) */
extern int hd_art;          /* HD graphics: the art in use */
extern int hd_density;      /* plane pixels per surface pixel, each way (1..4) */

/* The game's side (src/auto/graphics*.cpp).  Surfaces are named by their
   pixels (DibSurface::bits); rectangles are in surface pixels. */

/* A 640x480 sheet (TERRAIN1, UNITS ...) was loaded into this surface from
   the file name (the game's own folder only: a scenario's art is its own). */
void hd_sheet_loaded(void *bits, const char *filename);
/* A picture was loaded into this surface from resource GIFS id of a
   library (the module FindResource last found it in): the pack's picture
   resource/MODULE/gifs/ID, and its crops for the sprites taken from it. */
void hd_resource_loaded(void *bits, int id);
/* The surface's pixels were replaced otherwise (another picture loaded). */
void hd_sheet_clear(void *bits);
/* Sprite::encode_from_surface took sprite (its data handle) from the
   rectangle of the surface, with this transparent colour (-2: none). */
void hd_sprite_captured(const void *sprite, unsigned handle, void *bits, int x, int y, int w, int h, int key);
/* The sprite (its data handle) is drawn with this picture of the pack. */
void hd_sprite_bind(const void *sprite, unsigned handle, const char *key, int opaque_tile);
void hd_sprite_forget(const void *sprite);
/* Sprite::replace_color: a flag in a civilization's colour. */
void hd_sprite_recolor(const void *sprite, unsigned handle, int old_color, int new_color);
/* The sprite (a terrain diamond) covers its diamond exactly: no seams. */
void hd_sprite_opaque(const void *sprite, unsigned handle);
/* Before the game draws the sprite at (x, y), w x h (its frame, scaled),
   clipped: returns nonzero if it has a picture of its own. */
int hd_sprite_begin(const void *sprite, unsigned handle, void *bits, int x, int y, int w, int h);
/* After: draws that picture (silhouette: in that palette colour, -1 not),
   then takes the game's pixels drawn there (drawn) as shown. */
void hd_sprite_end(const void *sprite, unsigned handle, void *bits, int x, int y, int w, int h, const int *clip,
                   int silhouette, const int *drawn);
/* gfx_copy_pixels: before (copy_begin) and after (copy_end). */
void hd_copy_begin(void *src_bits, int sx, int sy, int w, int h);
void hd_copy_end(void *dst_bits, void *src_bits, int dx, int dy, int sx, int sy, int w, int h);

/* The game drew this rectangle anew (DibSurface::fill_rect): it shows as
   the game's pixels even where they did not change. */
void hd_bits_redrawn(void *bits, int x, int y, int w, int h);

/* The platform's side (src/platform). */
struct Bitmap;
void hd_init(int classic, int unit_test, int scale);
/* The art's file (CIV2_HD_PACK, the program's data/hd, the user's folder's
   hd): 1 if it is there. */
int hd_pack_path(char *path, int size);
void hd_bitmap_free(struct Bitmap *b);
void hd_redrawn(struct Bitmap *b, int x, int y, int w, int h);     /* fills */
/* Text (text.c): b's plane (NULL: none); before drawing, the rectangle's
   pixels into it (sync); after, the game's pixels there as shown (commit). */
unsigned int *hd_bitmap_plane(struct Bitmap *b);
/* b's plane, given one if it had none (NULL: it cannot have one). */
unsigned int *hd_bitmap_want_plane(struct Bitmap *b);
void hd_bitmap_sync(struct Bitmap *b, int x, int y, int w, int h);
void hd_bitmap_commit(struct Bitmap *b, int x, int y, int w, int h);
/* Shapes of the platform's controls drawn again on the plane of the DC's
   surface (given one), antialiased: after the same drawn in its pixels.
   Coordinates are the DC's logical ones, fractional; nothing without the
   planes. */
struct DC;
void hd_dc_rect(struct DC *dc, double l, double t, double r, double b, unsigned int c);
void hd_dc_disc(struct DC *dc, double cx, double cy, double r, unsigned int c);
void hd_dc_line(struct DC *dc, const double *xy, int n, double width, unsigned int c);
void hd_dc_polygon(struct DC *dc, const double *xy, int n, unsigned int c);
/* A scroll bar arrow's chevron at (cx, cy), up, down, left, right (0..3),
   on its face colour. */
void hd_dc_chevron(struct DC *dc, double cx, double cy, int dir, unsigned int c, unsigned int face);
/* blit: before (pixels of src about to be read) and after (copied to dst). */
void hd_blit_begin(struct Bitmap *src, int sx, int sy, int sw, int sh);
void hd_blit_end(struct Bitmap *dst, struct Bitmap *src, int x, int y, int w, int h, int sx, int sy, int sw,
                 int sh, int l, int t, int r, int b);
/* plat_compose: the background, then each window's piece (surface s
   shown at dx, dy; the piece l, t, r, b on the screen), then the colour
   correction (lut, or NULL). */
void hd_compose_begin(void);
void hd_compose_piece(struct Bitmap *s, int dx, int dy, int l, int t, int r, int b);
void hd_compose_end(const unsigned char *lut);
/* The HD frame to present (screen_w*d x screen_h*d), or NULL. */
const unsigned int *hd_frame(void);
void hd_screenshot(const char *path);

#ifdef __cplusplus
}
#endif
#endif
