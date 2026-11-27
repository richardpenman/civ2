/*
 * The planes (docs/HD-GRAPHICS.md): the game draws as always, into its
 * 8-bit surfaces; beside them some surfaces keep a true-colour plane, density
 * times larger each way, where text and the platform's controls are drawn
 * again at that resolution, and, with HD graphics, the sprites the game
 * draws from Peter Johnson's replacement art (openciv2-assets).  Whatever
 * else changes a surface's pixels shows in its plane as those pixels
 * enlarged.  On with sharp text and interface (CIV.INI's [OpenCiv2]
 * SharpText=, on by default but with CIV2_CLASSIC=1) on a screen of more than
 * one pixel per game pixel, or with HD graphics (Graphics=HD, the art
 * installed; never with CIV2_CLASSIC=1); never in replays (but CIV2_TEST_SHARP=N,
 * CIV2_TEST_HD=N) or the unit tests.  Nothing here changes what the game
 * computes, nor calls rand().
 */
#ifndef CIV2_HD_H
#define CIV2_HD_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

extern int hd_on;           /* the planes in use (sharp text and interface, HD graphics) */
extern int hd_art;          /* HD graphics: the art in use */
extern int hd_density;      /* plane pixels per surface pixel, each way (1..4) */

/* A picture's pixels as 8-bit RGBA, w x h, from a PNG or a WebP file's bytes
   (by their first bytes; src/platform/hd_image.c); NULL: neither, or bad.
   Freed with free(). */
unsigned char *hd_decode_image(const unsigned char *data, size_t len, int *w, int *h);

/* An animated WebP (hd_image.c): opened from its bytes (taken: freed with
   it; NULL: not one), its frames and canvas size, frame i (0 on) as the
   canvas shows it, RGBA not premultiplied, w x h (NULL: bad; valid until
   the next call), from any frame in any order (decoded from the last key
   frame before it, or on from the one asked last); hd_anim_ms: how long it
   plays (its frames' durations, ms). */
typedef struct HdAnim HdAnim;
HdAnim *hd_anim_open(unsigned char *data, size_t len);
void hd_anim_close(HdAnim *a);
int hd_anim_frames(const HdAnim *a);
void hd_anim_size(const HdAnim *a, int *w, int *h);
long hd_anim_ms(const HdAnim *a);
const unsigned char *hd_anim_frame(HdAnim *a, int i);

/* OpenCiv2's own heralds (data/native/heralds/heralds.txt and hd-extra's, docs/HD-GRAPHICS.md
   "Your own heralds"): a line each, CODE X Y W H FPS G0 G1 I0 I1 P0 P1 D0 D1
   A0 A1 FILE: the three letters of the original's KINGS\HRLDCODE.AVI, where
   it stands in the diplomacy room and its size (the game's pixels), its
   frames per second, the first and last frame of its five segments (the
   game's order: greeting, idle, pleased, refusal, angry), the animated WebP
   beside it.  hd_herald_parse: one line (1: valid); hd_herald_find: with HD
   graphics (hd_art), the tribe's of that code, its file's path in path (1:
   there is one). */
typedef struct {
    char code[4];
    int x, y, w, h, fps;
    int seg[5][2];
    char file[64];
} HdHerald;
int hd_herald_parse(const char *line, HdHerald *h);
int hd_herald_find(const char *code, HdHerald *h, char *path, size_t n);

/* OpenCiv2's own wonder movies (data/native/wonders, made by make
   own-assets; docs/NO-INSTALL.md, "Wonder movies"): wonderNN.webp, the
   picture (an animated WebP), and wonderNN.ogg, its sound, played in the
   place of the game's Video\WONDERNN.AVI (src/platform/video.c:
   open_own_wonder; the game's WonderMovieWindow::play).  hd_wonder_find:
   whether wonder n's plays, as the music and the sound effects choose:
   with HD graphics (hd_art) in place of the game folder's, with Classic
   graphics only where the game folder has none (installed: 0); never in a
   replay but with CIV2_TEST_WONDER=1 (the recordings were made without
   it).  1: it plays, the picture's path in path (the sound's: .ogg for
   .webp), its size in the game's pixels in *w x *h (240 high, as wide as
   the picture's proportions make it). */
int hd_wonder_find(int n, int installed, int *w, int *h, char *path, size_t len);

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
/* The sprite (its data handle) is drawn with this picture of the pack;
   opaque_tile 1: opaque inside its diamond, nothing outside (the base
   terrain); 2: nothing outside its diamond (the terrain's overlays); 3 + Q:
   nothing outside the diamond of the tile whose quarter Q it fills (0 to
   3: top right, bottom right, bottom left, top left: the blends). */
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
/* The sprite's next draws, until hd_sprite_as(NULL, ...): the pack's
   picture KEY (NULL: its own), fitted inside the sprite's frame keeping its
   proportions (contain) or stretched, tinted flat in palette colour tint
   (-1: its own colours).  Only with HD graphics (hd_art): 1 if so. */
int hd_sprite_as(const void *sprite, unsigned handle, const char *key, int contain, int tint);
/* The sprites drawn next: as ever (0), in grey shades (HD_SPRITES_GREY),
   or not on the plane at all (HD_SPRITES_OFF: their pixels are left to
   hd_bits_shown or to show enlarged). */
enum { HD_SPRITES_GREY = 1, HD_SPRITES_OFF = 2 };
void hd_sprite_mode(int mode);
/* The sprite (its data handle) is drawn from a picture of the pack. */
int hd_sprite_pictured(const void *sprite, unsigned handle);
/* The game's pixels in the rectangle: into the plane, enlarged where they
   changed (sync); taken as shown as the plane has them (shown: what the
   game drew there afterwards, recolouring a sprite drawn from a picture). */
void hd_bits_sync(void *bits, int x, int y, int w, int h);
void hd_bits_shown(void *bits, int x, int y, int w, int h);
/* Tests (hd_map_redraw_probe): a copy of the plane's rectangle, the game's
   pixels synced first (free it); the plane pixels of the rectangle that
   differ from such a copy by more than tolerance in a channel, but where
   skip (w x h, NULL: none) is set; a copy of the game's own pixels. */
unsigned int *hd_debug_capture(void *bits, int x, int y, int w, int h);
int hd_debug_compare(void *bits, int x, int y, int w, int h, const unsigned int *before, int tolerance,
                     const unsigned char *skip);
unsigned char *hd_debug_capture_pixels(void *bits, int x, int y, int w, int h);
void hd_debug_dump(const char *path, const unsigned int *px, int w, int h);     /* a PPM */
/* gfx_copy_pixels: before (copy_begin) and after (copy_end). */
void hd_copy_begin(void *src_bits, int sx, int sy, int w, int h);
void hd_copy_end(void *dst_bits, void *src_bits, int dx, int dy, int sx, int sy, int w, int h);

/* Pictures the game's side composes on a surface's plane itself (the
   throne room, src/game/hd_graphics.cpp), over the game's own picture: the
   pack's picture KEY in the rectangle x, y, w x h (surface pixels,
   fractional), fitted inside it keeping its proportions (contain) or
   stretched; 0 if the pack has none (nothing drawn). */
int hd_picture(void *bits, const char *key, double x, double y, double w, double h, int contain);
/* The pack's picture's proportions (its size in the game's pixels): 0 none. */
int hd_picture_size(const char *key, int *w, int *h);
/* A rectangle of colour 0xAARRGGBB (AA the opacity) over the plane. */
void hd_picture_fill(void *bits, double x, double y, double w, double h, unsigned int argb);
/* The same, clipped to clip (l, t, r, b, surface pixels), each column of
   the picture moved down by shear times its distance from the middle (a
   slope given to a straight piece), its opacity times alpha/255, in grey
   shades (grey); where it is mostly opaque, cover (the clip's size, row by
   row, NULL: none) is set.  0 if the pack has none (nothing drawn). */
int hd_picture_part(void *bits, const char *key, double x, double y, double w, double h, double shear, int alpha,
                    int grey, const int *clip, unsigned char *cover);
/* The colour 0xffRRGGBB of index in the surface's colour table. */
unsigned int hd_bits_colour(void *bits, int index);
/* The game's pixels where mask (w x h) is set shown as the plane has them
   (hd_bits_shown, those pixels only). */
void hd_bits_shown_mask(void *bits, int x, int y, int w, int h, const unsigned char *mask);
/* Done: the game's pixels as they are now are shown as the plane has them,
   and the plane is a picture of its own, which a change of the surface's
   colour table leaves as it is (until another picture is loaded there). */
void hd_picture_done(void *bits);

/* The game drew this rectangle anew (DibSurface::fill_rect): it shows as
   the game's pixels even where they did not change. */
void hd_bits_redrawn(void *bits, int x, int y, int w, int h);
/* ... only its pixels where mask (w x h, row by row) is not 0, in one call
   (the surface found once, not for each pixel). */
void hd_bits_redrawn_mask(void *bits, int x, int y, int w, int h, const unsigned char *mask);

/* The platform's side (src/platform). */
struct Bitmap;
void hd_init(int classic, int unit_test, int scale);
/* The art read for the pictures a game folder lacks (synth.c; HD graphics
   are on then, plat_hd_forced): hd_art_open whether the art is loaded (0:
   no); hd_art_copy a picture's
   RGBA pixels, pw x ph (free()), its size in the game's pixels w x h;
   hd_art_each the keys beginning with prefix. */
int hd_art_open(void);
unsigned char *hd_art_copy(const char *key, int *pw, int *ph, int *w, int *h);
void hd_art_each(const char *prefix, void (*fn)(const char *key, int w, int h, void *ctx), void *ctx);
void hd_bitmap_free(struct Bitmap *b);
void hd_redrawn(struct Bitmap *b, int x, int y, int w, int h);     /* fills */
/* b's colour table changed (it was old): its pixels enlarged in the plane
   take their new colours (a palette's fade). */
void hd_colors_changed(struct Bitmap *b, const void *old);
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
/* The window's background picture (the surface bits: the new game's
   backdrop, tiles.dll 90) with HD graphics: the pack's covering the
   rectangle l, t, r, b of the DC (its client, cropped evenly, its
   proportions kept), within the DC's clip; 1 if drawn (nothing without HD
   graphics or another picture).  plat_hd_cover: the same for a window's
   DC by its handle (src/platform/gdi.c). */
int hd_dc_cover(struct DC *dc, void *bits, int l, int t, int r, int b);
int plat_hd_cover(void *hdc, void *bits, int l, int t, int r, int b);
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
