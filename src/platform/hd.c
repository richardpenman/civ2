/*
 * The planes: HD graphics and sharp text and interface (src/platform/hd.h,
 * docs/HD-GRAPHICS.md).  Without the art (sharp text and interface alone)
 * the pack is empty: every sprite keeps its own pixels, enlarged.
 *
 * The art: Peter Johnson's runtime pack (openciv2-assets, MIT), its file
 * data/atlas-hd.pack: "C2ATLAS1", a count, a palette (768 bytes), then per
 * picture a 180-byte entry (key, 160 bytes; its size in the game's pixels;
 * the offset and length of its PNG; an offset of indexed pixels, unused
 * here), the keys in ascending order.  A sprite the game takes from one of
 * its sheets at x,y,w,h has the picture "crop/file/SHEET/x,y,w,h" where the
 * pack has one (4 to 8 times as large); terrain blends are bound by role
 * ("dither/T/Q", "fog/Q").  The pictures of the game's libraries are
 * "resource/MODULE/gifs/ID" and their sprites "crop/resource/..." (below).
 * (The browser version gives the game only the pictures drawn here:
 * src/web/hdpack.js, KEEP; a picture of another kind drawn here is to be
 * added there.)
 *
 * The planes: a surface (Bitmap) gets one when an HD sprite is drawn on it
 * or a surface with one is copied to it: hd, w*d x h*d pixels 0xffRRGGBB,
 * and hd_prev, the surface's own pixels as the plane last took them.
 * Pixels that differ from hd_prev were drawn by the game otherwise (text,
 * fills, lines, sprites without a picture): before the plane is read they
 * are copied into it, enlarged (sync).  An HD sprite or a copy from a
 * plane draws into the plane, then takes the game's pixels it covered as
 * shown (commit).  The idea of the second plane kept by the differences is
 * Peter Johnson's (PR #11 of richardpenman/civ2, src/platform/hd.c); this
 * is a smaller implementation of it.
 */
#include <math.h>
#include <unistd.h>
#include "plat.h"
#include "hd.h"
#include "../host/host.h"

extern char plat_data_dir[];
extern char plat_game_dir[];
int plat_profile_string(const char *file, const char *section, const char *key, char *out, int n);

int hd_on;
int hd_art;
int hd_density = 1;

/* ---- the pack ---- */

typedef struct {
    char key[160];
    uint32_t w, h, off, len;
    unsigned char *rgba;        /* decoded, pw x ph; NULL: not now */
    int pw, ph;
    uint64_t used;
} Entry;
static Entry *entries;
static unsigned nentries;
static FILE *pack;
static uint64_t tick, decoded_bytes;
#define DECODED_BUDGET (160u << 20)

static uint32_t le32(const unsigned char *p)
{
    return p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static int open_pack(const char *path)
{
    unsigned char head[12], e[180];
    long size;
    unsigned i;
    if (!(pack = fopen(path, "rb"))) {
        plat_log("HD graphics: no %s; the original graphics\n", path);
        return 0;
    }
    fseek(pack, 0, SEEK_END);
    size = ftell(pack);
    fseek(pack, 0, SEEK_SET);
    if (fread(head, 1, 12, pack) != 12 || memcmp(head, "C2ATLAS1", 8) || le32(head + 8) > 65536 ||
        fseek(pack, 768, SEEK_CUR))
        goto bad;
    nentries = le32(head + 8);
    entries = calloc(nentries ? nentries : 1, sizeof *entries);
    for (i = 0; i < nentries; i++) {
        Entry *a = entries + i;
        if (fread(e, 1, 180, pack) != 180 || !memchr(e, 0, 160))
            goto bad;
        memcpy(a->key, e, 160);
        a->w = le32(e + 160); a->h = le32(e + 164); a->off = le32(e + 168); a->len = le32(e + 172);
        if (!a->w || !a->h || a->w > 4096 || a->h > 4096 || a->off > (uint32_t)size ||
            a->len > (uint32_t)size - a->off || (i && strcmp(entries[i - 1].key, a->key) >= 0))
            goto bad;
    }
    plat_log("HD graphics: %s, %u pictures, density %d\n", path, nentries, hd_density);
    return 1;
bad:
    plat_log("HD graphics: %s is not a valid pack; the original graphics\n", path);
    fclose(pack);
    pack = NULL;
    free(entries);
    entries = NULL;
    nentries = 0;
    return 0;
}

static int find(const char *key)        /* 1 + index, 0 none */
{
    unsigned lo = 0, hi = nentries;
    while (lo < hi) {
        unsigned mid = lo + (hi - lo) / 2;
        int c = strcmp(key, entries[mid].key);
        if (!c)
            return (int)mid + 1;
        if (c < 0) hi = mid; else lo = mid + 1;
    }
    return 0;
}

static Entry *decode(int id)
{
    Entry *a = entries + id - 1;
    unsigned char *png;
    a->used = ++tick;
    if (a->rgba)
        return a;
    while (decoded_bytes > DECODED_BUDGET) {
        Entry *old = NULL;
        unsigned i;
        for (i = 0; i < nentries; i++)
            if (entries[i].rgba && (!old || entries[i].used < old->used))
                old = entries + i;
        if (!old)
            break;
        decoded_bytes -= (uint64_t)old->pw * old->ph * 4;
        free(old->rgba);
        old->rgba = NULL;
    }
    png = malloc(a->len);
    if (!png || fseek(pack, (long)a->off, SEEK_SET) || fread(png, 1, a->len, pack) != a->len) {
        free(png);
        return NULL;
    }
    a->rgba = hd_decode_image(png, a->len, &a->pw, &a->ph);     /* PNG or WebP: hd_image.c */
    free(png);
    if (!a->rgba) {
        plat_log("HD graphics: cannot decode %s\n", a->key);
        return NULL;
    }
    decoded_bytes += (uint64_t)a->pw * a->ph * 4;
    return a;
}

/* ---- pictures at the size drawn: premultiplied 0xAARRGGBB ---- */

typedef struct Scaled {
    int id, w, h, opaque;
    uint32_t *px;
    uint64_t used;
    struct Scaled *next;
} Scaled;
static Scaled *scaled[1024];
static uint64_t scaled_bytes;
#define SCALED_BUDGET (128u << 20)

/* Inside the diamond the w x h box holds (terrain): its pixels are drawn
   opaque, the others not at all, so neighbouring tiles meet without seams
   or overlap whatever the art's own edges. */
static int in_diamond(int x, int y, int w, int h)
{
    return (int64_t)abs(2 * x + 1 - w) * h + (int64_t)abs(2 * y + 1 - h) * w < (int64_t)w * h;
}

/* A pixel of a diamond drawn opaque where the picture has nothing (its
   antialiased edge falls a little inside the diamond the game's tile
   fills): the colour of the picture's nearest pixel that is there, not
   the transparent pixels' black (which showed as a dotted line along the
   tiles' edges, and differed after a partial redraw). */
static int edge_colour(const Entry *a, double u, double v, double *r, double *g, double *b)
{
    int cx = (int)u, cy = (int)v, rad, i, j, best = -1;
    double bd = 0;
    for (rad = 1; rad <= 16 && best < 0; rad++)
        for (j = cy - rad; j <= cy + rad; j++)
            for (i = cx - rad; i <= cx + rad; i++) {
                double dx = i + .5 - u, dy = j + .5 - v, dd = dx * dx + dy * dy;
                if (i < 0 || j < 0 || i >= a->pw || j >= a->ph || a->rgba[4 * ((size_t)j * a->pw + i) + 3] < 128)
                    continue;
                if (best < 0 || dd < bd) {
                    best = j * a->pw + i;
                    bd = dd;
                }
            }
    if (best < 0)
        return 0;
    *r = a->rgba[4 * (size_t)best];
    *g = a->rgba[4 * (size_t)best + 1];
    *b = a->rgba[4 * (size_t)best + 2];
    return 1;
}

static uint32_t *make_scaled(Entry *a, int w, int h, int opaque)
{
    uint32_t *out = malloc((size_t)w * h * 4);
    int x, y;
    if (!out)
        return NULL;
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            double r = 0, g = 0, b = 0, al = 0, n = 0, rr = 0, gg = 0, bb = 0;
            if (w > a->pw || h > a->ph) {
                /* enlarged: bilinear */
                double u = (x + .5) * a->pw / w - .5, v = (y + .5) * a->ph / h - .5;
                int x0 = (int)floor(u), y0 = (int)floor(v), i, j;
                double fx = u - x0, fy = v - y0;
                for (j = 0; j < 2; j++)
                    for (i = 0; i < 2; i++) {
                        int sx = x0 + i, sy = y0 + j;
                        const unsigned char *p;
                        double k = (i ? fx : 1 - fx) * (j ? fy : 1 - fy);
                        if (sx < 0) sx = 0;
                        if (sy < 0) sy = 0;
                        if (sx >= a->pw) sx = a->pw - 1;
                        if (sy >= a->ph) sy = a->ph - 1;
                        p = a->rgba + 4 * ((size_t)sy * a->pw + sx);
                        al += k * p[3];
                        r += k * p[0] * p[3]; g += k * p[1] * p[3]; b += k * p[2] * p[3];
                        rr += k * p[0]; gg += k * p[1]; bb += k * p[2]; n += k;
                    }
            } else {
                /* reduced: the average of the source pixels it covers */
                int sx0 = x * a->pw / w, sx1 = (x + 1) * a->pw / w, sy0 = y * a->ph / h, sy1 = (y + 1) * a->ph / h, i, j;
                if (sx1 <= sx0) sx1 = sx0 + 1;
                if (sy1 <= sy0) sy1 = sy0 + 1;
                for (j = sy0; j < sy1; j++)
                    for (i = sx0; i < sx1; i++) {
                        const unsigned char *p = a->rgba + 4 * ((size_t)j * a->pw + i);
                        al += p[3];
                        r += p[0] * p[3]; g += p[1] * p[3]; b += p[2] * p[3];
                        rr += p[0]; gg += p[1]; bb += p[2]; n++;
                    }
            }
            if ((opaque == 2 && !in_diamond(x, y, w, h)) ||
                (opaque >= 3 && !in_diamond(x + (opaque == 3 || opaque == 4) * w, y + (opaque == 4 || opaque == 5) * h,
                                            2 * w, 2 * h))) {
                out[y * w + x] = 0;
                continue;
            }
            if (opaque == 1) {
                if (!in_diamond(x, y, w, h)) {
                    out[y * w + x] = 0;
                    continue;
                }
                if (al > 0) { r /= al; g /= al; b /= al; }
                else if (!edge_colour(a, (x + .5) * a->pw / w, (y + .5) * a->ph / h, &r, &g, &b)) {
                    r = rr / n; g = gg / n; b = bb / n;
                }
                out[y * w + x] = 0xff000000u | (uint32_t)(r + .5) << 16 | (uint32_t)(g + .5) << 8 | (uint32_t)(b + .5);
            } else {
                unsigned A = (unsigned)(al / n + .5);
                /* premultiplied: colour sum / n / 255 */
                out[y * w + x] = A << 24 | (uint32_t)(r / n / 255 + .5) << 16 | (uint32_t)(g / n / 255 + .5) << 8 |
                                 (uint32_t)(b / n / 255 + .5);
            }
        }
    return out;
}

static const uint32_t *picture(int id, int w, int h, int opaque)
{
    Scaled **bucket = &scaled[((unsigned)id * 31u + (unsigned)w * 7u + (unsigned)h) & 1023], *p;
    Entry *a;
    for (p = *bucket; p; p = p->next)
        if (p->id == id && p->w == w && p->h == h && p->opaque == opaque) {
            p->used = ++tick;
            return p->px;
        }
    if (w <= 0 || h <= 0 || (int64_t)w * h > 4096 * 4096 || !(a = decode(id)))
        return NULL;
    while (scaled_bytes + (uint64_t)w * h * 4 > SCALED_BUDGET) {
        Scaled **old = NULL, **q;
        int i;
        for (i = 0; i < 1024; i++)
            for (q = &scaled[i]; *q; q = &(*q)->next)
                if (!old || (*q)->used < (*old)->used)
                    old = q;
        if (!old)
            break;
        p = *old;
        *old = p->next;
        scaled_bytes -= (uint64_t)p->w * p->h * 4;
        free(p->px);
        free(p);
    }
    p = calloc(1, sizeof *p);
    if (!p || !(p->px = make_scaled(a, w, h, opaque))) {
        free(p);
        return NULL;
    }
    p->id = id; p->w = w; p->h = h; p->opaque = opaque; p->used = ++tick;
    p->next = *bucket;
    *bucket = p;
    scaled_bytes += (uint64_t)w * h * 4;
    return p->px;
}

/* ---- planes ---- */

static int pixel_bytes(const Bitmap *b) { return b->bpp == 8 ? 1 : b->bpp == 32 ? 4 : 0; }

static uint32_t logical(const Bitmap *b, const unsigned char *row, int x)
{
    if (b->bpp == 8) {
        RGBQUAD q = b->colors[row[x]];
        return 0xff000000u | (uint32_t)q.rgbRed << 16 | (uint32_t)q.rgbGreen << 8 | q.rgbBlue;
    }
    return 0xff000000u | ((const uint32_t *)row)[x];
}

static void block(Bitmap *b, int x, int y, uint32_t c)
{
    int d = hd_density, pitch = b->w * d, i, j;
    uint32_t *p = b->hd + (size_t)y * d * pitch + (size_t)x * d;
    b->hd_big[(size_t)y * b->w + x] = 1;
    for (j = 0; j < d; j++, p += pitch)
        for (i = 0; i < d; i++)
            p[i] = c;
}

void hd_bitmap_free(Bitmap *b)
{
    free(b->hd);
    free(b->hd_prev);
    free(b->hd_big);
    b->hd = NULL;
    b->hd_prev = NULL;
    b->hd_big = NULL;
    b->hd_own = 0;
}

static int valid(const Bitmap *b)
{
    return b && b->hd && b->hd_w == b->w && b->hd_h == b->h && b->hd_bits == b->bits && b->hd_d == hd_density;
}

/* Gives b a plane, its pixels enlarged.  0: it cannot have one. */
static int plane(Bitmap *b)
{
    int x, y, n;
    if (!hd_on || !b || !(n = pixel_bytes(b)) || b->w <= 0 || b->h <= 0)
        return 0;
    if (valid(b))
        return 1;
    hd_bitmap_free(b);
    b->hd = malloc((size_t)b->w * b->h * hd_density * hd_density * 4);
    b->hd_prev = malloc((size_t)b->w * b->h * n);
    b->hd_big = malloc((size_t)b->w * b->h);
    if (!b->hd || !b->hd_prev || !b->hd_big) {
        hd_bitmap_free(b);
        return 0;
    }
    b->hd_w = b->w;
    b->hd_h = b->h;
    b->hd_d = hd_density;
    b->hd_bits = b->bits;
    for (y = 0; y < b->h; y++) {
        const unsigned char *row = bmp_row(b, y);
        memcpy(b->hd_prev + (size_t)y * b->w * n, row, (size_t)b->w * n);
        for (x = 0; x < b->w; x++)
            block(b, x, y, logical(b, row, x));
    }
    return 1;
}

static int clip_rect(const Bitmap *b, int *l, int *t, int *r, int *bot)
{
    if (*l < 0) *l = 0;
    if (*t < 0) *t = 0;
    if (*r > b->w) *r = b->w;
    if (*bot > b->h) *bot = b->h;
    return *l < *r && *t < *bot;
}

/* The game's own drawing in the rectangle, into the plane. */
static void sync_rect(Bitmap *b, int l, int t, int r, int bot)
{
    int n, x, y;
    if (!valid(b) || !clip_rect(b, &l, &t, &r, &bot))
        return;
    n = pixel_bytes(b);
    for (y = t; y < bot; y++) {
        const unsigned char *row = bmp_row(b, y);
        unsigned char *prev = b->hd_prev + (size_t)y * b->w * n;
        if (!memcmp(prev + l * n, row + l * n, (size_t)(r - l) * n))
            continue;
        for (x = l; x < r; x++)
            if (memcmp(prev + x * n, row + x * n, (size_t)n)) {
                memcpy(prev + x * n, row + x * n, (size_t)n);
                block(b, x, y, logical(b, row, x));
            }
    }
}

/* The game drew the whole rectangle anew (a fill, a copy from a surface
   without a plane): its pixels into the plane, changed or not. */
static void redraw_rect(Bitmap *b, int l, int t, int r, int bot)
{
    int n, x, y;
    if (!valid(b) || !clip_rect(b, &l, &t, &r, &bot))
        return;
    n = pixel_bytes(b);
    for (y = t; y < bot; y++) {
        const unsigned char *row = bmp_row(b, y);
        memcpy(b->hd_prev + ((size_t)y * b->w + l) * n, row + l * n, (size_t)(r - l) * n);
        for (x = l; x < r; x++)
            block(b, x, y, logical(b, row, x));
    }
}

/* The colour table changed (a fade, a palette loaded): where the plane
   shows a pixel enlarged in its old colour (not a picture, nor sharp text),
   the new colour.  The pictures keep theirs (docs/HD-GRAPHICS.md). */
void hd_colors_changed(Bitmap *b, const void *old_table)
{
    const RGBQUAD *old = old_table;
    uint32_t was[256], now[256];
    unsigned char changed[256];
    int d = hd_density, pitch, x, y, i, j, any = 0;
    if (!hd_on || !valid(b) || b->bpp != 8 || b->hd_own)
        return;
    for (i = 0; i < 256; i++) {
        was[i] = 0xff000000u | (uint32_t)old[i].rgbRed << 16 | (uint32_t)old[i].rgbGreen << 8 | old[i].rgbBlue;
        now[i] = 0xff000000u | (uint32_t)b->colors[i].rgbRed << 16 | (uint32_t)b->colors[i].rgbGreen << 8 |
                 b->colors[i].rgbBlue;
        any |= changed[i] = was[i] != now[i];
    }
    if (!any)
        return;
    sync_rect(b, 0, 0, b->w, b->h);
    pitch = b->w * d;
    for (y = 0; y < b->h; y++) {
        const unsigned char *row = b->hd_prev + (size_t)y * b->w;
        for (x = 0; x < b->w; x++) {
            uint32_t *p = b->hd + (size_t)y * d * pitch + (size_t)x * d;
            int same = 1;
            if (!changed[row[x]])
                continue;
            for (j = 0; j < d && same; j++)
                for (i = 0; i < d; i++)
                    if (p[j * pitch + i] != was[row[x]]) {
                        same = 0;
                        break;
                    }
            if (same)
                block(b, x, y, now[row[x]]);
        }
    }
}

void hd_redrawn(Bitmap *b, int x, int y, int w, int h)
{
    if (hd_on)
        redraw_rect(b, x, y, x + w, y + h);
}

void hd_bits_redrawn(void *bits, int x, int y, int w, int h)
{
    if (hd_on)
        redraw_rect(bmp_of_bits(bits), x, y, x + w, y + h);
}

void hd_bits_redrawn_mask(void *bits, int x, int y, int w, int h, const unsigned char *mask)
{
    Bitmap *b;
    int l = x, t = y, r = x + w, bot = y + h, n, i, j;
    if (!hd_on || !(b = bmp_of_bits(bits)) || !valid(b) || !clip_rect(b, &l, &t, &r, &bot))
        return;
    n = pixel_bytes(b);
    for (j = t; j < bot; j++) {
        const unsigned char *row = bmp_row(b, j), *m = mask + (size_t)(j - y) * w;
        unsigned char *prev = b->hd_prev + (size_t)j * b->w * n;
        for (i = l; i < r; i++)
            if (m[i - x]) {
                memcpy(prev + i * n, row + i * n, (size_t)n);
                block(b, i, j, logical(b, row, i));
            }
    }
}

/* The game's pixels in the rectangle are shown as the plane has them. */
static void commit_pixels(Bitmap *b, int l, int t, int r, int bot, int art)
{
    int n, y;
    if (!valid(b) || !clip_rect(b, &l, &t, &r, &bot))
        return;
    n = pixel_bytes(b);
    for (y = t; y < bot; y++) {
        memcpy(b->hd_prev + ((size_t)y * b->w + l) * n, bmp_row(b, y) + l * n, (size_t)(r - l) * n);
        if (art)
            memset(b->hd_big + (size_t)y * b->w + l, 0, (size_t)(r - l));
    }
}

/* ... and the plane there is art (a picture, sharp text), not the game's
   pixels enlarged. */
static void commit_rect(Bitmap *b, int l, int t, int r, int bot)
{
    commit_pixels(b, l, t, r, bot, 1);
}

/* Draws w x h premultiplied pixels at physical (x, y), clipped to the
   physical rectangle; tint: in that colour (0xRRGGBB), -1 as they are;
   pixels scaled (nearest) when pw x ph is not w x h. */
static void paint(Bitmap *b, const uint32_t *px, int pw, int ph, int x, int y, int w, int h, int l, int t, int r,
                  int bot, int64_t tint)
{
    int d = hd_density, pitch = b->w * d, i, j;
    if (l < x) l = x;
    if (t < y) t = y;
    if (r > x + w) r = x + w;
    if (bot > y + h) bot = y + h;
    if (l < 0) l = 0;
    if (t < 0) t = 0;
    if (r > b->w * d) r = b->w * d;
    if (bot > b->h * d) bot = b->h * d;
    for (j = t; j < bot; j++) {
        uint32_t *out = b->hd + (size_t)j * pitch;
        const uint32_t *in = px + (size_t)(pw == w && ph == h ? j - y : (int64_t)(j - y) * ph / h) * pw;
        for (i = l; i < r; i++) {
            uint32_t s = in[pw == w ? i - x : (int)((int64_t)(i - x) * pw / w)], dst, a = s >> 24, ia;
            if (!a)
                continue;
            if (tint >= 0)
                s = a << 24 | (((uint32_t)(tint >> 16) & 255) * a / 255) << 16 |
                    (((uint32_t)(tint >> 8) & 255) * a / 255) << 8 | ((uint32_t)tint & 255) * a / 255;
            if (a == 255) {
                out[i] = s | 0xff000000u;
                continue;
            }
            dst = out[i];
            ia = 255 - a;
            out[i] = 0xff000000u | (((s >> 16) & 255) + ((dst >> 16) & 255) * ia / 255) << 16 |
                     (((s >> 8) & 255) + ((dst >> 8) & 255) * ia / 255) << 8 | ((s & 255) + (dst & 255) * ia / 255);
        }
    }
}

/* Copies the plane's rectangle (sw x sh at sx, sy of src) to dst's at x, y
   (w x h, scaled nearest), clipped to l, t, r, b (surface pixels). */
/* A copy between 8-bit surfaces whose colour tables differ: where the
   plane showed a pixel enlarged in the source's colour, it takes the
   destination's colour for the same index, as the copied pixels do (the
   herald's backdrop, taken from the diplomacy room while the portrait's
   colours were not yet in the room's table, kept the portrait black in its
   plane under the video). */
static void recolor_copied(Bitmap *dst, const Bitmap *src, int l, int t, int r, int b)
{
    int d = hd_density, pitch = dst->w * d, x, y, i, j;
    if (src->bpp != 8 || dst->bpp != 8 || !memcmp(src->colors, dst->colors, 256 * sizeof *src->colors))
        return;
    for (y = t; y < b; y++) {
        const unsigned char *row = bmp_row(dst, y);
        for (x = l; x < r; x++) {
            uint32_t was = logical(src, row, x), now = logical(dst, row, x), *p;
            int same = 1;
            if (was == now || !dst->hd_big[(size_t)y * dst->w + x])
                continue;
            p = dst->hd + (size_t)y * d * pitch + (size_t)x * d;
            for (j = 0; j < d && same; j++)
                for (i = 0; i < d; i++)
                    if (p[j * pitch + i] != was) {
                        same = 0;
                        break;
                    }
            if (same)
                block(dst, x, y, now);
        }
    }
}

static void transfer(Bitmap *dst, Bitmap *src, int x, int y, int w, int h, int sx, int sy, int sw, int sh, int l,
                     int t, int r, int b)
{
    int d = hd_density, j, i;
    if (!valid(src) || !plane(dst) || w <= 0 || h <= 0 || sw <= 0 || sh <= 0)
        return;
    if (l < x) l = x;
    if (t < y) t = y;
    if (r > x + w) r = x + w;
    if (b > y + h) b = y + h;
    if (w == sw && h == sh) {
        /* within the source */
        if (l < x - sx) l = x - sx;
        if (t < y - sy) t = y - sy;
        if (r > x - sx + src->w) r = x - sx + src->w;
        if (b > y - sy + src->h) b = y - sy + src->h;
    }
    if (!clip_rect(dst, &l, &t, &r, &b))
        return;
    if (src->hd_alpha && src != dst) {
        /* a picture with transparent pixels: over what dst's plane shows
           there (its pixels are the copy's already) */
        for (j = t * d; j < b * d; j++)
            for (i = l * d; i < r * d; i++) {
                int ax = sx * d + (int)((int64_t)(i - x * d) * sw / w), ay = sy * d + (int)((int64_t)(j - y * d) * sh / h);
                uint32_t c, *o, a, ia;
                if (ax < 0 || ay < 0 || ax >= src->w * d || ay >= src->h * d)
                    continue;
                c = src->hd[(size_t)ay * src->w * d + ax];
                o = dst->hd + (size_t)j * dst->w * d + i;
                a = c >> 24;
                ia = 255 - a;
                if (a == 255)
                    *o = c;
                else if (a)
                    *o = 0xff000000u | (((c >> 16) & 255) + ((*o >> 16) & 255) * ia / 255) << 16 |
                         (((c >> 8) & 255) + ((*o >> 8) & 255) * ia / 255) << 8 | ((c & 255) + (*o & 255) * ia / 255);
            }
    } else if (w == sw && h == sh) {
        int first = t * d, end = b * d, step = 1;
        if (src == dst && y > sy) { first = b * d - 1; end = t * d - 1; step = -1; }
        for (j = first; j != end; j += step)
            memmove(dst->hd + (size_t)j * dst->w * d + l * d,
                    src->hd + (size_t)(j + (sy - y) * d) * src->w * d + (l + sx - x) * d, (size_t)(r - l) * d * 4);
    } else if (src != dst) {
        for (j = t * d; j < b * d; j++)
            for (i = l * d; i < r * d; i++) {
                int ax = sx * d + (int)((int64_t)(i - x * d) * sw / w), ay = sy * d + (int)((int64_t)(j - y * d) * sh / h);
                if (ax >= 0 && ay >= 0 && ax < src->w * d && ay < src->h * d)
                    dst->hd[(size_t)j * dst->w * d + i] = src->hd[(size_t)ay * src->w * d + ax];
            }
    } else
        return;     /* a scaled copy within a surface: left to the game's pixels */
    commit_pixels(dst, l, t, r, b, 0);
    /* which pixels the plane shows enlarged: the source's (a picture with
       transparent pixels: where it is opaque; where it is transparent, the
       destination's stay; where it is blended over them, art) */
    if (src->hd_alpha && src != dst)
        for (j = t; j < b; j++)
            for (i = l; i < r; i++) {
                int ax = sx + (int)((int64_t)(i - x) * sw / w), ay = sy + (int)((int64_t)(j - y) * sh / h);
                uint32_t a;
                if (ax < 0 || ay < 0 || ax >= src->w || ay >= src->h)
                    continue;
                a = src->hd[(size_t)ay * d * src->w * d + (size_t)ax * d] >> 24;
                if (a == 255)
                    dst->hd_big[(size_t)j * dst->w + i] = src->hd_big[(size_t)ay * src->w + ax];
                else if (a)
                    dst->hd_big[(size_t)j * dst->w + i] = 0;
            }
    else if (w == sw && h == sh) {
        int first = t, end = b, step = 1;
        if (src == dst && y > sy) { first = b - 1; end = t - 1; step = -1; }
        for (j = first; j != end; j += step)
            memmove(dst->hd_big + (size_t)j * dst->w + l, src->hd_big + (size_t)(j + sy - y) * src->w + l + sx - x,
                    (size_t)(r - l));
    } else
        for (j = t; j < b; j++)
            for (i = l; i < r; i++) {
                int ax = sx + (int)((int64_t)(i - x) * sw / w), ay = sy + (int)((int64_t)(j - y) * sh / h);
                if (ax >= 0 && ay >= 0 && ax < src->w && ay < src->h)
                    dst->hd_big[(size_t)j * dst->w + i] = src->hd_big[(size_t)ay * src->w + ax];
            }
    if (w == sw && h == sh && src != dst)
        recolor_copied(dst, src, l, t, r, b);
}

unsigned int *hd_bitmap_plane(Bitmap *b)
{
    return hd_on && valid(b) ? b->hd : NULL;
}

/* A surface text (or a control) is drawn on gets a plane: drawn there
   sharp, it stays sharp where the surface is copied (the game draws much of
   its text on surfaces of its own, then copies them to its windows). */
unsigned int *hd_bitmap_want_plane(Bitmap *b)
{
    return hd_on && plane(b) ? b->hd : NULL;
}

void hd_bitmap_sync(Bitmap *b, int x, int y, int w, int h)
{
    if (hd_on)
        sync_rect(b, x, y, x + w, y + h);
}

void hd_bitmap_commit(Bitmap *b, int x, int y, int w, int h)
{
    if (hd_on)
        commit_rect(b, x, y, x + w, y + h);
}

void hd_blit_begin(Bitmap *src, int sx, int sy, int sw, int sh)
{
    if (hd_on && valid(src))
        sync_rect(src, sx, sy, sx + sw, sy + sh);
}

void hd_blit_end(Bitmap *dst, Bitmap *src, int x, int y, int w, int h, int sx, int sy, int sw, int sh, int l, int t,
                 int r, int b)
{
    if (!hd_on)
        return;
    if (valid(src))
        transfer(dst, src, x, y, w, h, sx, sy, sw, sh, l, t, r, b);
    else if (valid(dst)) {
        if (l < x) l = x;
        if (t < y) t = y;
        if (r > x + w) r = x + w;
        if (b > y + h) b = y + h;
        redraw_rect(dst, l, t, r, b);
    }
}

void hd_copy_begin(void *src_bits, int sx, int sy, int w, int h)
{
    if (hd_on)
        hd_blit_begin(bmp_of_bits(src_bits), sx, sy, w, h);
}

void hd_copy_end(void *dst_bits, void *src_bits, int dx, int dy, int sx, int sy, int w, int h)
{
    if (hd_on)
        hd_blit_end(bmp_of_bits(dst_bits), bmp_of_bits(src_bits), dx, dy, w, h, sx, sy, w, h, dx, dy, dx + w, dy + h);
}

/* ---- the platform's shapes, sharp (radio buttons, check marks, arrows) ----

   Drawn again on the plane after the platform drew them in the surface's
   pixels: antialiased, 4 x 4 samples per plane pixel.  Coordinates are the
   DC's (logical pixels, fractional). */

enum { SH_RECT, SH_DISC, SH_LINE, SH_POLY };
typedef struct {
    int kind, n;
    const double *xy;           /* SH_LINE, SH_POLY: n points */
    double a[4], w;             /* SH_RECT l t r b; SH_DISC cx cy r; SH_LINE width w */
} Shape;

static double seg_dist2(double px, double py, double ax, double ay, double bx, double by)
{
    double dx = bx - ax, dy = by - ay, l = dx * dx + dy * dy, t = l > 0 ? ((px - ax) * dx + (py - ay) * dy) / l : 0;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    dx = ax + t * dx - px;
    dy = ay + t * dy - py;
    return dx * dx + dy * dy;
}

static int inside(const Shape *s, double x, double y)
{
    int i, in = 0;
    switch (s->kind) {
    case SH_RECT:
        return x >= s->a[0] && x < s->a[2] && y >= s->a[1] && y < s->a[3];
    case SH_DISC:
        return (x - s->a[0]) * (x - s->a[0]) + (y - s->a[1]) * (y - s->a[1]) <= s->a[2] * s->a[2];
    case SH_LINE:
        for (i = 0; i + 1 < s->n; i++)
            if (seg_dist2(x, y, s->xy[2 * i], s->xy[2 * i + 1], s->xy[2 * i + 2], s->xy[2 * i + 3]) <= s->w * s->w / 4)
                return 1;
        return 0;
    default:                    /* even-odd */
        for (i = 0; i < s->n; i++) {
            const double *p = s->xy + 2 * i, *q = s->xy + 2 * ((i + 1) % s->n);
            if ((p[1] > y) != (q[1] > y) && x < p[0] + (y - p[1]) * (q[0] - p[0]) / (q[1] - p[1]))
                in = !in;
        }
        return in;
    }
}

static void bounds(const Shape *s, double *l, double *t, double *r, double *b)
{
    int i;
    switch (s->kind) {
    case SH_RECT:
        *l = s->a[0]; *t = s->a[1]; *r = s->a[2]; *b = s->a[3];
        return;
    case SH_DISC:
        *l = s->a[0] - s->a[2]; *t = s->a[1] - s->a[2]; *r = s->a[0] + s->a[2]; *b = s->a[1] + s->a[2];
        return;
    default:
        *l = *r = s->xy[0];
        *t = *b = s->xy[1];
        for (i = 1; i < s->n; i++) {
            if (s->xy[2 * i] < *l) *l = s->xy[2 * i];
            if (s->xy[2 * i] > *r) *r = s->xy[2 * i];
            if (s->xy[2 * i + 1] < *t) *t = s->xy[2 * i + 1];
            if (s->xy[2 * i + 1] > *b) *b = s->xy[2 * i + 1];
        }
        if (s->kind == SH_LINE) {
            *l -= s->w / 2; *t -= s->w / 2; *r += s->w / 2; *b += s->w / 2;
        }
    }
}

static void draw_shape(DC *dc, const Shape *s, COLORREF colour)
{
    Bitmap *b;
    Region rg = {0, 0, NULL};
    double l, t, r, bot;
    int d = hd_density, ox, oy, k, L, T, R, B;
    uint32_t c;
    if (!hd_on || !dc || !(b = dc_target(dc)) || !hd_bitmap_want_plane(b))
        return;
    colour = dc_resolve_color(dc, colour);
    c = (uint32_t)GetRValue(colour) << 16 | (uint32_t)GetGValue(colour) << 8 | GetBValue(colour);
    ox = dc->is_memory ? 0 : dc->origin.x;
    oy = dc->is_memory ? 0 : dc->origin.y;
    bounds(s, &l, &t, &r, &bot);
    L = (int)floor(l) + ox; T = (int)floor(t) + oy; R = (int)ceil(r) + ox; B = (int)ceil(bot) + oy;
    if (!clip_rect(b, &L, &T, &R, &B))
        return;
    if (dc->is_memory) {
        RECT all = {0, 0, 0, 0};
        all.right = b->w;
        all.bottom = b->h;
        rgn_set(&rg, &all);
    } else
        rgn_copy(&rg, &dc->clip);
    sync_rect(b, L, T, R, B);
    for (k = 0; k < rg.n; k++) {
        int cl = rg.r[k].left > L ? rg.r[k].left : L, ct = rg.r[k].top > T ? rg.r[k].top : T;
        int cr = rg.r[k].right < R ? rg.r[k].right : R, cb = rg.r[k].bottom < B ? rg.r[k].bottom : B, x, y, i, j;
        for (y = ct * d; y < cb * d; y++)
            for (x = cl * d; x < cr * d; x++) {
                int n = 0;
                uint32_t *p, z;
                for (j = 0; j < 4; j++)
                    for (i = 0; i < 4; i++)
                        n += inside(s, (x + (i + .5) / 4) / d - ox, (y + (j + .5) / 4) / d - oy);
                if (!n)
                    continue;
                p = b->hd + (size_t)y * b->w * d + x;
                z = *p;
                *p = 0xff000000u | (((c >> 16 & 255) * n + (z >> 16 & 255) * (16 - n)) / 16) << 16 |
                     (((c >> 8 & 255) * n + (z >> 8 & 255) * (16 - n)) / 16) << 8 |
                     ((c & 255) * n + (z & 255) * (16 - n)) / 16;
            }
    }
    commit_rect(b, L, T, R, B);
    rgn_free(&rg);
}

void hd_dc_rect(DC *dc, double l, double t, double r, double b, unsigned int c)
{
    Shape s = {SH_RECT, 0, NULL, {0, 0, 0, 0}, 0};
    s.a[0] = l; s.a[1] = t; s.a[2] = r; s.a[3] = b;
    if (hd_on)
        draw_shape(dc, &s, c);
}

void hd_dc_disc(DC *dc, double cx, double cy, double r, unsigned int c)
{
    Shape s = {SH_DISC, 0, NULL, {0, 0, 0, 0}, 0};
    s.a[0] = cx; s.a[1] = cy; s.a[2] = r;
    if (hd_on)
        draw_shape(dc, &s, c);
}

void hd_dc_line(DC *dc, const double *xy, int n, double width, unsigned int c)
{
    Shape s = {SH_LINE, 0, NULL, {0, 0, 0, 0}, 0};
    s.n = n; s.xy = xy; s.w = width;
    if (hd_on && n >= 2)
        draw_shape(dc, &s, c);
}

void hd_dc_polygon(DC *dc, const double *xy, int n, unsigned int c)
{
    Shape s = {SH_POLY, 0, NULL, {0, 0, 0, 0}, 0};
    s.n = n; s.xy = xy;
    if (hd_on && n >= 3)
        draw_shape(dc, &s, c);
}

/* A scroll bar arrow's chevron, centred at (cx, cy), pointing up, down,
   left or right (0 to 3), on the arrow's white (as scroll_theme.h's). */
void hd_dc_chevron(DC *dc, double cx, double cy, int dir, unsigned int c, unsigned int face)
{
    static const double up[6] = {-3.3, 1.3, 0, -1.9, 3.3, 1.3};
    double xy[6];
    int i;
    if (!hd_on)
        return;
    for (i = 0; i < 3; i++) {
        double u = up[2 * i], v = up[2 * i + 1];
        xy[2 * i] = cx + (dir < 2 ? u : dir == 2 ? v : -v);
        xy[2 * i + 1] = cy + (dir == 0 ? v : dir == 1 ? -v : u);
    }
    hd_dc_rect(dc, cx - 4.5, cy - 3.5, cx + 4.5, cy + 3.5, face);
    hd_dc_line(dc, xy, 3, 1.3, c);
}

/* ---- sheets and sprites ---- */

/* The surface's pixels were replaced: its plane shows them anew (a picture
   painted there before is gone). */
static void reloaded(Bitmap *b)
{
    b->hd_sheet[0] = 0;
    if (valid(b))
        redraw_rect(b, 0, 0, b->w, b->h);
    b->hd_alpha = 0;
    b->hd_own = 0;
}

void hd_sheet_clear(void *bits)
{
    Bitmap *b;
    if (hd_on && (b = bmp_of_bits(bits)))
        reloaded(b);
}

void hd_sheet_loaded(void *bits, const char *filename)
{
    Bitmap *b;
    char cwd[1024];
    const char *base = filename, *s;
    size_t i;
    if (!hd_on || !(b = bmp_of_bits(bits)))
        return;
    reloaded(b);
    /* a scenario's own art is kept: only the game's folder's sheets */
    if (!getcwd(cwd, sizeof cwd) || strcmp(cwd, plat_game_dir))
        return;
    for (s = filename; *s; s++)
        if (*s == '/' || *s == '\\' || *s == ':')
            base = s + 1;
    memcpy(b->hd_sheet, "file/", 5);
    for (i = 0; base[i] && base[i] != '.' && i < sizeof b->hd_sheet - 6; i++)
        b->hd_sheet[5 + i] = (char)tolower((unsigned char)base[i]);
    b->hd_sheet[5 + i] = 0;
    if (!find(b->hd_sheet))
        b->hd_sheet[0] = 0;
}

/* ---- the pictures of the game's resource libraries ----

   A picture the game loads from a library's GIF resource (mk.dll's 200 ...)
   is the pack's "resource/MODULE/gifs/ID", at the original's size; the
   sprites the game takes from it are "crop/resource/MODULE/gifs/ID/x,y,w,h".
   The pack's pictures were made for PR #11's own renderers: many are laid
   out otherwise than the original's (the throne room's pieces, the
   spaceship's parts, the advisors' rooms; the throne room is composed by
   src/game/hd_graphics.cpp, hd_picture below), so only those that are the same
   picture, whole, are painted on the surface's plane (WHOLE, each looked
   at in its window); the others give only their crops. */
static const struct {
    const char *module;
    int lo, hi;
    int inset;                  /* the picture's border left out, in 1/1000ths */
} WHOLE[] = {
    {"mk.dll", 200, 206, 0},    /* diplomacy: the rooms, by government */
    /* (not the advisors' backgrounds, tiles.dll 50 to 59, 65 and 66: the
       pack's are a pale parchment, and the game's text on them, coloured
       for the original's dark photographs, could not be read: docs/HD-GRAPHICS.md) */
    /* the announcements' pictures (tiles.dll 70 to 77: a city captured, in
       disorder, celebrating, founded; + 1 the modern one): the pack's are
       vignettes on a transparent ground, at the original's size; the wide
       ones fill their frame as the original's photographs do, but the tall
       ones (71, 75, 77) are wide pictures shrunk into the middle of a tall
       frame, small, so those are the original's, enlarged
       (harness/hd/hd-pictures.txt) */
    {"tiles.dll", 70, 70, 0},
    {"tiles.dll", 72, 74, 0},
    {"tiles.dll", 76, 76, 0},
    {"tiles.dll", 90, 90, 0},   /* the new game's backdrop */
    {"intro.dll", 901, 913, 0}, /* the new game's pictures */
    /* the city view's landscapes (340 + 5 x the site + civ_city_art_era):
       the pack's are grass with dirt tracks in every era, so only those of
       the eras before the paved roads (0, 1); the modern ones (2, and 3
       with Superhighways) are the original's, enlarged */
    {"cv.dll", 340, 341, 0},
    {"cv.dll", 345, 346, 0},
    {"cv.dll", 350, 351, 0},
    {"ss.dll", 498, 499, 0},    /* the spaceship's window */
};

/* The crops of the pack's pictures that are not the original's sprite:
   the leaders' portraits (mk.dll 220 + the tribe, + 21 for a woman, at
   1,1,225,275 in the diplomacy room) are 42 people of their own, not the
   tribes' leaders in the original's order (the Vikings' is a man in a
   19th-century frock coat), so a portrait's own crop is never drawn: the
   game's side gives each tribe the pack's portrait that fits it
   (hd_diplomacy_portrait in src/game/hd_graphics.cpp), or the original's,
   enlarged; 210 and 211 (the sheets of small faces, which the custom
   tribe's portrait chooser copies whole) are the original's. */
static const struct {
    const char *module;
    int lo, hi;
} NOT_CROPPED[] = {
    {"mk.dll", 210, 211},
    {"mk.dll", 220, 261},
};

/* Single crops of the pack not drawn either: in tiles.dll 86, the pack's
   information seal (the popups' icon, the quit confirmation's) and the
   tutorial's owl are a thin pale "i" and "?" on nothing, the podium in a
   civilization's colour a flat shield; the city's disorder marker (ICONS
   at 166,223) is a thin "!" on nothing, which the game's recolouring for
   the owner (hd_recolour_begin) made a faint stroke, the marker all but
   invisible: the original's are kept, enlarged (the disorder marker's box
   in the owner's colour, as the game recolours it, with its black fist). */
static const char *const NOT_CROPPED_KEYS[] = {
    "crop/file/icons/166,223,30,30",
    "crop/resource/tiles.dll/gifs/86/398,134,64,64",
    "crop/resource/tiles.dll/gifs/86/134,398,64,64",
    "crop/resource/tiles.dll/gifs/86/2,398,64,64",
};

extern char plat_res_module[];

/* The picture without a border of inset/1000 of its size, scaled to w x h. */
static uint32_t *scaled_inside(Entry *a, int inset, int w, int h)
{
    Entry in = *a;
    int l = a->pw * inset / 1000, t = a->ph * inset / 1000, y;
    uint32_t *px;
    in.pw = a->pw - 2 * l;
    in.ph = a->ph - 2 * t;
    if (in.pw <= 0 || in.ph <= 0 || !(in.rgba = malloc((size_t)in.pw * in.ph * 4)))
        return NULL;
    for (y = 0; y < in.ph; y++)
        memcpy(in.rgba + (size_t)y * in.pw * 4, a->rgba + ((size_t)(y + t) * a->pw + l) * 4, (size_t)in.pw * 4);
    px = make_scaled(&in, w, h, 0);
    free(in.rgba);
    return px;
}

void hd_resource_loaded(void *bits, int id)
{
    Bitmap *b;
    Entry *a;
    const uint32_t *px;
    uint32_t *own = NULL;
    int pid, d = hd_density, y, whole = -1;
    size_t i, n;
    if (!hd_on || !(b = bmp_of_bits(bits)))
        return;
    reloaded(b);
    if (!hd_art)
        return;
    snprintf(b->hd_sheet, sizeof b->hd_sheet, "resource/%s/gifs/%d", plat_res_module, id);
    for (i = 0; i < sizeof WHOLE / sizeof *WHOLE; i++)
        if (!strcmp(WHOLE[i].module, plat_res_module) && id >= WHOLE[i].lo && id <= WHOLE[i].hi)
            whole = WHOLE[i].inset;
    /* (the surface's rows are whole dwords: up to 3 pixels wider) */
    if (whole < 0 || !(pid = find(b->hd_sheet)) || (int)entries[pid - 1].h != b->h || (int)entries[pid - 1].w > b->w ||
        b->w - (int)entries[pid - 1].w > 3 || b->bpp != 8 || !plane(b))
        return;
    a = entries + pid - 1;
    /* (scaled once and kept, as the pictures drawn are: the city view
       loads its landscape each time it is opened, 3840 x 1440 at density 3) */
    if (whole)
        px = decode(pid) ? own = scaled_inside(a, whole, (int)a->w * d, b->h * d) : NULL;
    else
        px = picture(pid, (int)a->w * d, b->h * d, 0);
    if (!px)
        return;
    n = (size_t)a->w * d;
    for (y = 0; y < b->h * d; y++)
        memcpy(b->hd + (size_t)y * b->w * d, px + y * n, n * 4);
    for (i = 0; i < n * b->h * d && !b->hd_alpha; i++)
        b->hd_alpha = px[i] >> 24 != 255;
    free(own);
    commit_rect(b, 0, 0, b->w, b->h);
}

typedef struct Spr {
    const void *sprite;
    unsigned handle;
    int id, opaque;             /* the pack's picture (variant: in a civilization's colour), or */
    int variant;
    uint32_t *snap;             /* the plane's pixels it was taken from, */
    int sw, sh;                 /* sw x sh */
    int contain;                /* (hd_sprite_as) fitted inside the frame, */
    int tint;                   /* in this palette colour + 1 (0: its own) */
    struct Spr *next;
} Spr;
static Spr *sprites[4096];

static Spr **bucket(const void *s)
{
    uintptr_t k = (uintptr_t)s;
    return &sprites[((k >> 2) ^ (k >> 14)) & 4095];
}

/* hd_sprite_as: one sprite drawn otherwise for a while */
static const void *as_sprite;
static Spr as_spr;
static int sprite_mode;         /* hd_sprite_mode */

static Spr *lookup(const void *s, unsigned handle)
{
    Spr *p;
    if (as_sprite && s == as_sprite)
        return &as_spr;
    for (p = *bucket(s); p; p = p->next)
        if (p->sprite == s)
            return p->handle == handle ? p : NULL;
    return NULL;
}

void hd_sprite_forget(const void *s)
{
    Spr **q, *p;
    if (!hd_on)
        return;
    for (q = bucket(s); (p = *q); q = &p->next)
        if (p->sprite == s) {
            *q = p->next;
            free(p->snap);
            free(p);
            return;
        }
}

static Spr *add(const void *s, unsigned handle)
{
    Spr *p = calloc(1, sizeof *p);
    hd_sprite_forget(s);
    if (!p)
        return NULL;
    p->sprite = s;
    p->handle = handle;
    p->next = *bucket(s);
    *bucket(s) = p;
    return p;
}

void hd_sprite_bind(const void *s, unsigned handle, const char *key, int opaque_tile)
{
    int id;
    Spr *p;
    if (!hd_on)
        return;
    if (!(id = find(key))) {
        hd_sprite_forget(s);
        return;
    }
    if ((p = add(s, handle))) {
        p->id = id;
        p->opaque = opaque_tile;
    }
}

void hd_sprite_opaque(const void *s, unsigned handle)
{
    Spr *p;
    if (hd_on && (p = lookup(s, handle)) && p->id)
        p->opaque = 1;
}

/* Sprite::replace_color: colour 8 (a flag's cloth) in a civilization's
   colour is a picture of its own in the pack ("tint/KEY/COLOUR"). */
void hd_sprite_recolor(const void *s, unsigned handle, int old_color, int new_color)
{
    Spr *p;
    char key[220];
    if (!hd_on || !(p = lookup(s, handle)) || !p->id || old_color != 8)
        return;
    snprintf(key, sizeof key, "tint/%s/%d", entries[p->id - 1].key, new_color & 255);
    p->variant = find(key);
}

void hd_sprite_captured(const void *s, unsigned handle, void *bits, int x, int y, int w, int h, int key)
{
    Bitmap *b;
    char name[220];
    int id, d = hd_density, i, j;
    Spr *p;
    if (!hd_on)
        return;
    hd_sprite_forget(s);
    if (!(b = bmp_of_bits(bits)) || w <= 0 || h <= 0 || x < 0 || y < 0 || x + w > b->w || y + h > b->h)
        return;
    if (b->hd_sheet[0]) {
        for (i = 0; i < (int)(sizeof NOT_CROPPED / sizeof *NOT_CROPPED); i++) {
            size_t n = strlen(NOT_CROPPED[i].module);
            if (!strncmp(b->hd_sheet, "resource/", 9) && !strncmp(b->hd_sheet + 9, NOT_CROPPED[i].module, n) &&
                !strncmp(b->hd_sheet + 9 + n, "/gifs/", 6) && (id = atoi(b->hd_sheet + 15 + n)) >= NOT_CROPPED[i].lo &&
                id <= NOT_CROPPED[i].hi)
                return;         /* (drawn as the game's pixels, enlarged) */
        }
        snprintf(name, sizeof name, "crop/%s/%d,%d,%d,%d", b->hd_sheet, x, y, w, h);
        for (i = 0; i < (int)(sizeof NOT_CROPPED_KEYS / sizeof *NOT_CROPPED_KEYS); i++)
            if (!strcmp(name, NOT_CROPPED_KEYS[i]))
                return;
        if ((id = find(name)) && (p = add(s, handle))) {
            p->id = id;
            /* the terrain's tiles and quarters: inside their diamond only */
            if ((!strcmp(b->hd_sheet, "file/terrain1") || !strcmp(b->hd_sheet, "file/terrain2")) &&
                ((w == 64 && h == 32) || (w == 32 && h == 16)))
                p->opaque = 2;
            return;
        }
        /* a sheet's sprite without a picture keeps its pixels; a resource
           picture's is taken from its plane (below) */
        if (!strncmp(b->hd_sheet, "file/", 5))
            return;
    }
    /* taken from what is drawn (a unit's background, kept to restore it):
       the plane's pixels, where the game's are not the transparent colour */
    if (!valid(b) || b->bpp != 8 || !(p = add(s, handle)))
        return;
    sync_rect(b, x, y, x + w, y + h);
    if (!(p->snap = malloc((size_t)w * h * d * d * 4))) {
        hd_sprite_forget(s);
        return;
    }
    p->sw = w * d;
    p->sh = h * d;
    for (j = 0; j < h * d; j++) {
        const unsigned char *row = bmp_row(b, y + j / d);
        for (i = 0; i < w * d; i++)
            /* (a picture with its own transparency: that, not the key's) */
            p->snap[(size_t)j * w * d + i] = key >= 0 && !b->hd_alpha && row[x + i / d] == key ? 0 :
                b->hd[(size_t)(y * d + j) * b->w * d + x * d + i] | (b->hd_alpha ? 0 : 0xff000000u);
    }
}

int hd_sprite_as(const void *s, unsigned handle, const char *key, int contain, int tint)
{
    Spr *p;
    int id;
    as_sprite = NULL;
    if (!hd_art || !s)
        return 0;
    if (key) {
        if (!(id = find(key)))
            return 0;
        memset(&as_spr, 0, sizeof as_spr);
        as_spr.id = id;
    } else {
        if (!(p = lookup(s, handle)) || (!p->id && !p->snap))
            return 0;
        as_spr = *p;
        as_spr.next = NULL;
    }
    as_spr.sprite = s;
    as_spr.handle = handle;
    as_spr.contain = contain;
    as_spr.tint = tint >= 0 ? (tint & 255) + 1 : 0;
    as_sprite = s;
    return 1;
}

void hd_sprite_mode(int mode)
{
    sprite_mode = mode;
}

int hd_sprite_pictured(const void *s, unsigned handle)
{
    Spr *p;
    return hd_art && (p = lookup(s, handle)) && p->id;
}

void hd_bits_sync(void *bits, int x, int y, int w, int h)
{
    if (hd_on)
        sync_rect(bmp_of_bits(bits), x, y, x + w, y + h);
}

void hd_bits_shown(void *bits, int x, int y, int w, int h)
{
    if (hd_on)
        commit_rect(bmp_of_bits(bits), x, y, x + w, y + h);
}

void hd_bits_shown_mask(void *bits, int x, int y, int w, int h, const unsigned char *mask)
{
    Bitmap *b;
    int l = x, t = y, r = x + w, bot = y + h, n, i, j;
    if (!hd_on || !(b = bmp_of_bits(bits)) || !valid(b) || !clip_rect(b, &l, &t, &r, &bot))
        return;
    n = pixel_bytes(b);
    for (j = t; j < bot; j++) {
        const unsigned char *row = bmp_row(b, j), *m = mask + (size_t)(j - y) * w;
        unsigned char *prev = b->hd_prev + (size_t)j * b->w * n;
        for (i = l; i < r; i++)
            if (m[i - x]) {
                memcpy(prev + i * n, row + i * n, (size_t)n);
                b->hd_big[(size_t)j * b->w + i] = 0;
            }
    }
}

/* The picture in grey shades (SentryHalftone): a copy, premultiplied. */
static uint32_t *greyed(const uint32_t *px, int n)
{
    uint32_t *g = malloc((size_t)n * 4);
    int i;
    if (!g)
        return NULL;
    for (i = 0; i < n; i++) {
        uint32_t c = px[i], v = (((c >> 16) & 255) * 77 + ((c >> 8) & 255) * 150 + (c & 255) * 29) >> 8;
        g[i] = (c & 0xff000000u) | v << 16 | v << 8 | v;
    }
    return g;
}

int hd_sprite_begin(const void *s, unsigned handle, void *bits, int x, int y, int w, int h)
{
    Bitmap *b;
    if (!hd_on || sprite_mode == HD_SPRITES_OFF || !lookup(s, handle) || !(b = bmp_of_bits(bits)) || !plane(b))
        return 0;
    sync_rect(b, x, y, x + w, y + h);
    return 1;
}

void hd_sprite_end(const void *s, unsigned handle, void *bits, int x, int y, int w, int h, const int *clip, int silhouette,
                   const int *drawn)
{
    Bitmap *b = bmp_of_bits(bits);
    Spr *p;
    int d = hd_density;
    int64_t tint = -1;
    if (!(p = lookup(s, handle)) || !valid(b))
        return;
    if (silhouette >= 0 || p->tint) {
        RGBQUAD q = b->colors[(silhouette >= 0 ? silhouette : p->tint - 1) & 255];
        tint = (int64_t)q.rgbRed << 16 | q.rgbGreen << 8 | q.rgbBlue;
    }
    if (p->snap)
        paint(b, p->snap, p->sw, p->sh, x * d, y * d, w * d, h * d, clip[0] * d, clip[1] * d, clip[2] * d,
              clip[3] * d, tint);
    else {
        int pw = w * d, ph = h * d, px0 = x * d, py0 = y * d, id = p->variant ? p->variant : p->id;
        const uint32_t *px;
        uint32_t *grey = NULL;
        if (p->contain && entries[id - 1].w && entries[id - 1].h) {
            /* inside the frame, keeping the picture's proportions */
            double k = (double)pw / entries[id - 1].w < (double)ph / entries[id - 1].h ?
                (double)pw / entries[id - 1].w : (double)ph / entries[id - 1].h;
            int fw = (int)(entries[id - 1].w * k + .5), fh = (int)(entries[id - 1].h * k + .5);
            px0 += (pw - fw) / 2;
            py0 += (ph - fh) / 2;
            pw = fw;
            ph = fh;
        }
        if (pw > 0 && ph > 0 && (px = picture(id, pw, ph, p->opaque))) {
            if (sprite_mode == HD_SPRITES_GREY && tint < 0 && (grey = greyed(px, pw * ph)))
                px = grey;
            paint(b, px, pw, ph, px0, py0, pw, ph, clip[0] * d, clip[1] * d, clip[2] * d, clip[3] * d, tint);
            free(grey);
        }
    }
    commit_rect(b, drawn[0], drawn[1], drawn[2], drawn[3]);
}

/* ---- pictures the game's side composes (hd.h: hd_picture) ---- */

int hd_picture_size(const char *key, int *w, int *h)
{
    int id;
    if (!hd_art || !(id = find(key)))
        return 0;
    *w = (int)entries[id - 1].w;
    *h = (int)entries[id - 1].h;
    return 1;
}

int hd_picture(void *bits, const char *key, double x, double y, double w, double h, int contain)
{
    Bitmap *b;
    const uint32_t *px;
    int id, d = hd_density, l, t, pw, ph;
    if (!hd_art || !(id = find(key)) || !(b = bmp_of_bits(bits)) || b->bpp != 8 || !plane(b) || w <= 0 || h <= 0)
        return 0;
    if (contain) {
        double k = w / entries[id - 1].w < h / entries[id - 1].h ? w / entries[id - 1].w : h / entries[id - 1].h;
        x += (w - entries[id - 1].w * k) / 2;
        y += (h - entries[id - 1].h * k) / 2;
        w = entries[id - 1].w * k;
        h = entries[id - 1].h * k;
    }
    l = (int)floor(x * d + .5);
    t = (int)floor(y * d + .5);
    pw = (int)floor((x + w) * d + .5) - l;
    ph = (int)floor((y + h) * d + .5) - t;
    if (pw <= 0 || ph <= 0 || !(px = picture(id, pw, ph, 0)))
        return 0;
    paint(b, px, pw, ph, l, t, pw, ph, 0, 0, b->w * d, b->h * d, -1);
    return 1;
}

int hd_picture_part(void *bits, const char *key, double x, double y, double w, double h, double shear, int alpha,
                    int grey, const int *clip, unsigned char *cover)
{
    Bitmap *b;
    const uint32_t *px;
    int id, d = hd_density, l, t, pw, ph, cl, ct, cr, cb, pitch, i, j;
    if (!hd_art || !(id = find(key)) || !(b = bmp_of_bits(bits)) || b->bpp != 8 || !plane(b) || w <= 0 || h <= 0)
        return 0;
    l = (int)floor(x * d + .5);
    t = (int)floor(y * d + .5);
    pw = (int)floor((x + w) * d + .5) - l;
    ph = (int)floor((y + h) * d + .5) - t;
    if (pw <= 0 || ph <= 0 || !(px = picture(id, pw, ph, 0)))
        return 0;
    cl = clip[0] < 0 ? 0 : clip[0] * d;
    ct = clip[1] < 0 ? 0 : clip[1] * d;
    cr = (clip[2] > b->w ? b->w : clip[2]) * d;
    cb = (clip[3] > b->h ? b->h : clip[3]) * d;
    pitch = b->w * d;
    for (i = 0; i < pw; i++) {
        int X = l + i, off = (int)floor(shear * (i - pw / 2.0) + .5);
        if (X < cl || X >= cr)
            continue;
        for (j = 0; j < ph; j++) {
            int Y = t + j + off;
            uint32_t s = px[(size_t)j * pw + i], a, dst, ia, *out;
            if (Y < ct || Y >= cb || !(s >> 24))
                continue;
            if (grey) {
                uint32_t v = (((s >> 16) & 255) * 77 + ((s >> 8) & 255) * 150 + (s & 255) * 29) >> 8;
                s = (s & 0xff000000u) | v << 16 | v << 8 | v;
            }
            if (alpha < 255)
                s = ((s >> 24) * alpha / 255) << 24 | (((s >> 16) & 255) * alpha / 255) << 16 |
                    (((s >> 8) & 255) * alpha / 255) << 8 | (s & 255) * alpha / 255;
            if (!(a = s >> 24))
                continue;
            if (cover && a >= 128)
                cover[(size_t)(Y / d - clip[1]) * (clip[2] - clip[0]) + X / d - clip[0]] = 1;
            out = b->hd + (size_t)Y * pitch + X;
            dst = *out;
            ia = 255 - a;
            *out = 0xff000000u | (((s >> 16) & 255) + ((dst >> 16) & 255) * ia / 255) << 16 |
                   (((s >> 8) & 255) + ((dst >> 8) & 255) * ia / 255) << 8 | ((s & 255) + (dst & 255) * ia / 255);
        }
    }
    return 1;
}

unsigned int hd_bits_colour(void *bits, int index)
{
    Bitmap *b = bmp_of_bits(bits);
    if (!b || b->bpp != 8)
        return 0xff000000u;
    return 0xff000000u | (uint32_t)b->colors[index & 255].rgbRed << 16 | (uint32_t)b->colors[index & 255].rgbGreen << 8 |
           b->colors[index & 255].rgbBlue;
}

void hd_picture_fill(void *bits, double x, double y, double w, double h, unsigned int argb)
{
    Bitmap *b;
    int d = hd_density, l, t;
    unsigned a = argb >> 24;
    /* premultiplied */
    uint32_t px = a << 24 | (((argb >> 16) & 255) * a / 255) << 16 | (((argb >> 8) & 255) * a / 255) << 8 |
                  (argb & 255) * a / 255;
    if (!hd_on || !(b = bmp_of_bits(bits)) || !plane(b))
        return;
    l = (int)floor(x * d + .5);
    t = (int)floor(y * d + .5);
    paint(b, &px, 1, 1, l, t, (int)floor((x + w) * d + .5) - l, (int)floor((y + h) * d + .5) - t, 0, 0, b->w * d,
          b->h * d, -1);
}

void hd_picture_done(void *bits)
{
    Bitmap *b;
    if (!hd_on || !(b = bmp_of_bits(bits)) || !valid(b))
        return;
    commit_rect(b, 0, 0, b->w, b->h);
    b->hd_own = 1;
}

/* ---- the desktop ---- */

static Bitmap *frame_of;        /* the screen, whose plane is the frame */
static Region bare;             /* what no window covers: the background */

void hd_compose_begin(void)
{
    RECT all = {0, 0, 0, 0};
    if (!hd_on || !plane(screen))
        return;
    frame_of = screen;
    all.right = screen->w;
    all.bottom = screen->h;
    rgn_set(&bare, &all);
}

/* Surface s shows at dx, dy; this piece is screen rectangle l, t, r, b. */
void hd_compose_piece(Bitmap *s, int dx, int dy, int l, int t, int r, int b)
{
    int d = hd_density, x, y, i;
    RECT piece;
    if (!hd_on || !valid(screen))
        return;
    if (l < dx) l = dx;
    if (t < dy) t = dy;
    if (r > dx + s->w) r = dx + s->w;
    if (b > dy + s->h) b = dy + s->h;
    if (!clip_rect(screen, &l, &t, &r, &b))
        return;
    SetRect(&piece, l, t, r, b);
    rgn_subtract_rect(&bare, &piece);
    /* every window shown gets a plane: what it draws next (text) is drawn
       there sharp */
    if (plane(s)) {
        sync_rect(s, l - dx, t - dy, r - dx, b - dy);
        for (y = t * d; y < b * d; y++) {
            uint32_t *o = screen->hd + (size_t)y * screen->w * d + l * d;
            memcpy(o, s->hd + (size_t)(y - dy * d) * s->w * d + (l - dx) * d, (size_t)(r - l) * d * 4);
            if (s->hd_alpha)
                for (x = 0; x < (r - l) * d; x++)
                    o[x] |= 0xff000000u;
        }
        return;
    }
    if (!pixel_bytes(s))
        return;
    for (y = t; y < b; y++) {
        const unsigned char *row = bmp_row(s, y - dy);
        for (x = l; x < r; x++) {
            uint32_t c = logical(s, row, x - dx), *p = screen->hd + (size_t)y * d * screen->w * d + (size_t)x * d;
            for (i = 0; i < d; i++, p += screen->w * d) {
                int k;
                for (k = 0; k < d; k++)
                    p[k] = c;
            }
        }
    }
}

/* The background (the desktop's own surface, with a plane of its own, so
   that it is enlarged once) where no window covers it. */
static void compose_background(void)
{
    int d = hd_density, k, y;
    if (rgn_empty(&bare) || !plane(root))
        return;
    for (k = 0; k < bare.n; k++) {
        int l = bare.r[k].left, t = bare.r[k].top, r = bare.r[k].right, b = bare.r[k].bottom;
        if (r > root->w) r = root->w;
        if (b > root->h) b = root->h;
        if (l >= r || t >= b)
            continue;
        sync_rect(root, l, t, r, b);
        for (y = t * d; y < b * d; y++)
            memcpy(screen->hd + (size_t)y * screen->w * d + l * d, root->hd + (size_t)y * root->w * d + l * d,
                   (size_t)(r - l) * d * 4);
    }
}

void hd_compose_end(const unsigned char *lut)
{
    size_t i, n;
    if (!hd_on || !valid(screen))
        return;
    compose_background();
    rgn_clear(&bare);
    if (lut) {
        n = (size_t)screen->w * screen->h * hd_density * hd_density;
        for (i = 0; i < n; i++) {
            uint32_t c = screen->hd[i];
            screen->hd[i] = 0xff000000u | (uint32_t)lut[(c >> 16) & 255] << 16 | (uint32_t)lut[(c >> 8) & 255] << 8 |
                            lut[c & 255];
        }
    }
    /* what the screen shows now is in the frame; drawing on the screen
       itself afterwards (popup menus) shows enlarged (hd_frame) */
    commit_rect(screen, 0, 0, screen->w, screen->h);
}

const unsigned int *hd_frame(void)
{
    if (!hd_on || frame_of != screen || !valid(screen))
        return NULL;
    sync_rect(screen, 0, 0, screen->w, screen->h);
    return screen->hd;
}

/* The frame as a 24-bit BMP (replays' `shot`, CIV2_TEST_HD). */
void hd_screenshot(const char *path)
{
    const unsigned int *px = hd_frame();
    int w = screen_w * hd_density, h = screen_h * hd_density, stride = (w * 3 + 3) & ~3, x, y;
    unsigned char hdr[54] = {'B', 'M'}, *row;
    unsigned size = 54 + (unsigned)(stride * h);
    FILE *f;
    if (!px || !(f = fopen(path, "wb")))
        return;
    hdr[2] = (unsigned char)size; hdr[3] = (unsigned char)(size >> 8); hdr[4] = (unsigned char)(size >> 16);
    hdr[5] = (unsigned char)(size >> 24);
    hdr[10] = 54; hdr[14] = 40;
    hdr[18] = (unsigned char)w; hdr[19] = (unsigned char)(w >> 8);
    hdr[22] = (unsigned char)h; hdr[23] = (unsigned char)(h >> 8);
    hdr[26] = 1; hdr[28] = 24;
    fwrite(hdr, 1, 54, f);
    row = calloc((size_t)stride, 1);
    for (y = h - 1; y >= 0; y--) {
        for (x = 0; x < w; x++) {
            uint32_t c = px[(size_t)y * w + x];
            row[x * 3] = (unsigned char)c;
            row[x * 3 + 1] = (unsigned char)(c >> 8);
            row[x * 3 + 2] = (unsigned char)(c >> 16);
        }
        fwrite(row, 1, (size_t)stride, f);
    }
    free(row);
    fclose(f);
}

/* ---- start ---- */

/* Where the art is: CIV2_HD_PACK, else the program's data (a source
   checkout: data/native/hd, make hd), else the user's own folder (where the
   OpenCiv2 Options dialog installs it, src/host/host_options.c).  1 if found. */
int hd_pack_path(char *path, int size)
{
    const char *p = getenv("CIV2_HD_PACK");
    char user[1024];
    FILE *f;
    if (p && *p) {
        snprintf(path, (size_t)size, "%s", p);
        return 1;
    }
    snprintf(path, (size_t)size, "%s/hd/atlas-hd.pack", plat_data_dir);
    if ((f = fopen(path, "rb"))) {
        fclose(f);
        return 1;
    }
    if (host_user_dir(user, sizeof user) > 0) {
        char other[1100];
        snprintf(other, sizeof other, "%s/hd/atlas-hd.pack", user);
        if ((f = fopen(other, "rb"))) {
            fclose(f);
            snprintf(path, (size_t)size, "%s", other);
            return 1;
        }
    }
    return 0;
}

/* A setting's value: "0" off, "1" (or "on") on at the screen's density, "N"
   (2 to 4) at density N; -1 off, 0 the screen's, N. */
static int wanted(const char *v)
{
    if (atoi(v) > 1)
        return atoi(v);
    return atoi(v) == 1 || !strcasecmp(v, "on") ? 0 : -1;
}

/* Sharp text and interface (docs/EXTRAS.md): the planes without the art.
   The command line's --sharp[=N] (CIV2_SHARP_ARG), else CIV2_SHARP=N, else
   CIV.INI's SharpText= (by default on, but in Classic mode).  -1 off, 0 at
   the screen's density, N. */
static int sharp_wanted(int classic)
{
    const char *arg = getenv("CIV2_SHARP_ARG"), *env = getenv("CIV2_SHARP");
    char value[16] = "";
    if (arg && *arg)
        return wanted(arg);
    if (env && *env)
        return wanted(env);
    plat_profile_string("CIV.INI", "OpenCiv2", "SharpText", value, sizeof value);
    return value[0] ? wanted(value) : classic ? -1 : 0;
}

/* (the pack closed: the planes alone, or nothing) */
static void close_pack(void)
{
    unsigned i;
    if (pack)
        fclose(pack);
    pack = NULL;
    for (i = 0; i < nentries; i++)
        free(entries[i].rgba);
    free(entries);
    entries = NULL;
    nentries = 0;
    decoded_bytes = 0;
    hd_art = 0;
}

void hd_init(int classic, int unit_test, int scale)
{
    char path[1100], value[16];
    const char *test = getenv("CIV2_TEST_HD"), *test_sharp = getenv("CIV2_TEST_SHARP");
    int art, sharp;
    close_pack();
    hd_on = 0;
    hd_density = 1;
    if (unit_test)
        return;
    if (plat_replay) {
        /* replays are compared with the original: only on request */
        art = test && atoi(test) >= 1 ? atoi(test) : -1;
        sharp = test_sharp && atoi(test_sharp) >= 1 ? atoi(test_sharp) : -1;
    } else {
        /* the art: the command line's --hd[=N] (CIV2_HD_ARG), else
           CIV2_HD=N, else CIV.INI's Graphics=HD; never in Classic mode */
        const char *arg = getenv("CIV2_HD_ARG"), *env = getenv("CIV2_HD");
        const char *on = arg && *arg ? arg : env && *env ? env : NULL;
        if (classic)
            art = -1;
        else if (on)
            art = wanted(on);
        else {
            plat_profile_string("CIV.INI", "OpenCiv2", "Graphics", value, sizeof value);
            art = strcasecmp(value, "HD") ? -1 : 0;
        }
        if (!art)
            art = (int)GetPrivateProfileIntA("OpenCiv2", "GraphicsDensity", 0, "CIV.INI");
        if (!art)
            art = scale;            /* a plane pixel per pixel of the display */
        sharp = sharp_wanted(classic);
        if (!sharp)
            sharp = scale;
    }
    /* the art at its density (1 too: the new art at the original's size);
       sharp text and interface only where the screen has more pixels */
    if (art >= 1 && hd_pack_path(path, sizeof path)) {
        hd_density = art > 4 ? 4 : art;
        hd_art = open_pack(path);
    } else if (art >= 1)
        plat_log("hd: no art (%s/hd/atlas-hd.pack, nor in the user's folder)\n", plat_data_dir);
    if (!hd_art)
        hd_density = sharp > 4 ? 4 : sharp > 1 ? sharp : 1;
    hd_on = hd_art || hd_density > 1;
    if (hd_on && !hd_art)
        plat_log("Sharp text and interface: density %d\n", hd_density);
}

/* ---- tests ---- */

unsigned int *hd_debug_capture(void *bits, int x, int y, int w, int h)
{
    Bitmap *b = bmp_of_bits(bits);
    int d = hd_density, j;
    uint32_t *out;
    if (!hd_on || !valid(b) || w <= 0 || h <= 0 || x < 0 || y < 0 || x + w > b->w || y + h > b->h)
        return NULL;
    sync_rect(b, x, y, x + w, y + h);
    if (!(out = malloc((size_t)w * h * d * d * 4)))
        return NULL;
    for (j = 0; j < h * d; j++)
        memcpy(out + (size_t)j * w * d, b->hd + (size_t)(y * d + j) * b->w * d + x * d, (size_t)w * d * 4);
    return out;
}

unsigned char *hd_debug_capture_pixels(void *bits, int x, int y, int w, int h)
{
    Bitmap *b = bmp_of_bits(bits);
    unsigned char *out;
    int j;
    if (!b || b->bpp != 8 || w <= 0 || h <= 0 || x < 0 || y < 0 || x + w > b->w || y + h > b->h ||
        !(out = malloc((size_t)w * h)))
        return NULL;
    for (j = 0; j < h; j++)
        memcpy(out + (size_t)j * w, bmp_row(b, y + j) + x, (size_t)w);
    return out;
}

int hd_debug_compare(void *bits, int x, int y, int w, int h, const unsigned int *before, int tolerance,
                     const unsigned char *skip)
{
    Bitmap *b = bmp_of_bits(bits);
    int d = hd_density, i, j, k, n = 0;
    if (!before || !valid(b))
        return -1;
    sync_rect(b, x, y, x + w, y + h);
    for (j = 0; j < h * d; j++)
        for (i = 0; i < w * d; i++) {
            uint32_t p = b->hd[(size_t)(y * d + j) * b->w * d + x * d + i], q = before[(size_t)j * w * d + i];
            if (skip && skip[(size_t)(j / d) * w + i / d])
                continue;
            for (k = 0; k < 24; k += 8)
                if (abs((int)((p >> k) & 255) - (int)((q >> k) & 255)) > tolerance) {
                    n++;
                    break;
                }
        }
    return n;
}

void hd_debug_dump(const char *path, const unsigned int *px, int w, int h)
{
    FILE *f;
    int i;
    if (!px || !(f = fopen(path, "wb")))
        return;
    fprintf(f, "P6 %d %d 255\n", w, h);
    for (i = 0; i < w * h; i++) {
        unsigned char c[3] = {(unsigned char)(px[i] >> 16), (unsigned char)(px[i] >> 8), (unsigned char)px[i]};
        fwrite(c, 1, 3, f);
    }
    fclose(f);
}
