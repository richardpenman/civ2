/*
 * Pictures for a game folder without the original's graphics
 * (docs/NO-INSTALL.md).  Where the game asks for one of its pictures and
 * the folder has none (TERRAIN1.GIF ..., a library's GIF resource, civ2.exe's
 * cursors and bitmaps), an 8-bit picture is made here at the original's
 * size and layout from the HD art (src/platform/hd.c: the pack and
 * OpenCiv2's own pictures), so that the game's code, which takes its sprites
 * from fixed rectangles and reads some of their pixels, works unchanged:
 *
 *  - each sprite the pack has a picture for (crop/file/SHEET/x,y,w,h,
 *    crop/resource/MODULE/gifs/ID/x,y,w,h) is drawn shrunk into its
 *    rectangle, a whole picture (resource/MODULE/gifs/ID) under them;
 *  - the colours: one palette for all (the game loads the sheets into its
 *    palette's entries 10 to 201, and draws many other pictures in it: CITY.GIF,
 *    the announcements' pictures, the advisors' backgrounds); the colours the
 *    game's code draws with by number kept near the original's (FIXED); the
 *    indices the game gives a meaning kept: 253 transparent, 254 a
 *    cell's border, 255 the background (the game's 7, 8 and 9), the
 *    sprites' marker pixels on the cells' borders (UNITS: the shield's place;
 *    CITIES: the flag's and the size box's), the unit's shield in the colour
 *    the game recolours (252), the civilizations' colours where the game reads
 *    them (CITIES' flags), the map's picking masks (TERRAIN1: the tile's
 *    quarters and the directions by index / 16) and the blends' dither;
 *  - a whole picture (no sprites taken from it) is dithered, its transparent
 *    ground dark grey; what the pack has no picture for is left empty (the
 *    background), and a picture it lacks altogether is a neutral grey.
 *
 * With the game's own files present nothing here is used; without them the
 * game runs with HD graphics, whatever is chosen (synth_missing, main.c:
 * plat_hd_forced), which draw the art over these: the 8-bit pictures serve
 * the game's code (its rectangles, the pixels it reads, the palette), not
 * the eye, and there is no original look without the original's files
 * (docs/NO-INSTALL.md).  Sheets are
 * written once a run into the folder "OpenCiv2Pictures" of the Windows
 * directory (plat_host_path finds them there); resources are kept in memory.
 */
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include "plat.h"
#include "hd.h"

/* ---- the original's pictures: their sizes ---- */

static const struct Res {
    const char *module;
    int id, w, h;
} RESOURCES[] = {
#include "synth_resources.h"
};

/* ---- an RGBA canvas ---- */

typedef struct {
    int w, h;
    unsigned char *px;          /* RGBA, straight alpha */
} Canvas;

static void canvas_init(Canvas *c, int w, int h)
{
    c->w = w;
    c->h = h;
    c->px = calloc((size_t)w * h, 4);
}

/* src (sw x sh) shrunk (a box filter) or enlarged (nearest) into the
   rectangle, in place of what was there. */
static void put_scaled(Canvas *c, const unsigned char *src, int sw, int sh, int x, int y, int w, int h)
{
    int i, j;
    for (j = 0; j < h; j++) {
        int sy0 = (int)((long long)j * sh / h), sy1 = (int)(((long long)j + 1) * sh / h);
        if (sy1 <= sy0)
            sy1 = sy0 + 1;
        if (y + j < 0 || y + j >= c->h)
            continue;
        for (i = 0; i < w; i++) {
            int sx0 = (int)((long long)i * sw / w), sx1 = (int)(((long long)i + 1) * sw / w), sx, sy;
            double r = 0, g = 0, b = 0, a = 0, n = 0;
            unsigned char *d;
            if (sx1 <= sx0)
                sx1 = sx0 + 1;
            if (x + i < 0 || x + i >= c->w)
                continue;
            for (sy = sy0; sy < sy1 && sy < sh; sy++)
                for (sx = sx0; sx < sx1 && sx < sw; sx++) {
                    const unsigned char *s = src + ((size_t)sy * sw + sx) * 4;
                    double al = s[3] / 255.0;
                    r += s[0] * al;
                    g += s[1] * al;
                    b += s[2] * al;
                    a += al;
                    n++;
                }
            d = c->px + ((size_t)(y + j) * c->w + x + i) * 4;
            if (a > 0) {
                d[0] = (unsigned char)(r / a + 0.5);
                d[1] = (unsigned char)(g / a + 0.5);
                d[2] = (unsigned char)(b / a + 0.5);
            } else
                d[0] = d[1] = d[2] = 0;
            d[3] = (unsigned char)(a / (n ? n : 1) * 255 + 0.5);
        }
    }
}

/* The pack's picture of key shrunk into the rectangle; 0: none. */
static int put_art(Canvas *c, const char *key, int x, int y, int w, int h)
{
    int pw, ph;
    unsigned char *px = hd_art_copy(key, &pw, &ph, NULL, NULL);
    if (!px)
        return 0;
    put_scaled(c, px, pw, ph, x, y, w, h);
    free(px);
    return 1;
}

/* ---- colours ---- */

typedef struct {
    unsigned char rgb[256][3];
    int lo, hi;                 /* the art's colours: lo .. hi - 1 */
    int greys;                  /* and the greys, 0 .. 31 */
    short cache[32768];         /* 15-bit colour -> index + 1 */
} SynthPalette;

/* The colours of the opaque pixels, counted by 15-bit colour. */
static void count_colours(const Canvas *c, unsigned *hist)
{
    size_t i, n = (size_t)c->w * c->h;
    for (i = 0; i < n; i++) {
        const unsigned char *p = c->px + i * 4;
        if (p[3] >= 128)
            hist[(p[0] >> 3) << 10 | (p[1] >> 3) << 5 | p[2] >> 3]++;
    }
}

typedef struct { int lo[3], hi[3]; unsigned long long n; } Box;

static void box_shrink(Box *b, const unsigned *hist)
{
    int lo[3] = {31, 31, 31}, hi[3] = {0, 0, 0}, r, g, bl;
    unsigned long long n = 0;
    for (r = b->lo[0]; r <= b->hi[0]; r++)
        for (g = b->lo[1]; g <= b->hi[1]; g++)
            for (bl = b->lo[2]; bl <= b->hi[2]; bl++) {
                unsigned k = hist[r << 10 | g << 5 | bl];
                if (!k)
                    continue;
                n += k;
                if (r < lo[0]) lo[0] = r;
                if (r > hi[0]) hi[0] = r;
                if (g < lo[1]) lo[1] = g;
                if (g > hi[1]) hi[1] = g;
                if (bl < lo[2]) lo[2] = bl;
                if (bl > hi[2]) hi[2] = bl;
            }
    b->n = n;
    if (n) {
        memcpy(b->lo, lo, sizeof lo);
        memcpy(b->hi, hi, sizeof hi);
    }
}

/* Median cut: up to k colours for the histogram, into pal->rgb[first ..];
   returns how many. */
static int median_cut(const unsigned *hist, SynthPalette *pal, int first, int k)
{
    Box boxes[256];
    int nb = 1, i;
    boxes[0].lo[0] = boxes[0].lo[1] = boxes[0].lo[2] = 0;
    boxes[0].hi[0] = boxes[0].hi[1] = boxes[0].hi[2] = 31;
    box_shrink(&boxes[0], hist);
    if (!boxes[0].n)
        return 0;
    while (nb < k) {
        int best = -1, axis = 0, a;
        unsigned long long score = 0;
        for (i = 0; i < nb; i++)
            for (a = 0; a < 3; a++) {
                unsigned long long s = boxes[i].n * (unsigned long long)(boxes[i].hi[a] - boxes[i].lo[a]);
                if (boxes[i].hi[a] > boxes[i].lo[a] && s > score) {
                    score = s;
                    best = i;
                    axis = a;
                }
            }
        if (best < 0)
            break;
        {
            Box *b = &boxes[best], nbx = *b;
            unsigned long long half = b->n / 2, sum = 0;
            int cut = b->lo[axis], v, r, g, bl;
            for (v = b->lo[axis]; v < b->hi[axis]; v++) {
                for (r = b->lo[0]; r <= b->hi[0]; r++)
                    for (g = b->lo[1]; g <= b->hi[1]; g++)
                        for (bl = b->lo[2]; bl <= b->hi[2]; bl++)
                            if ((axis == 0 ? r : axis == 1 ? g : bl) == v)
                                sum += hist[r << 10 | g << 5 | bl];
                cut = v;
                if (sum >= half)
                    break;
            }
            nbx.lo[axis] = cut + 1;
            b->hi[axis] = cut;
            box_shrink(b, hist);
            box_shrink(&nbx, hist);
            boxes[nb++] = nbx;
        }
    }
    for (i = 0; i < nb; i++) {
        double r = 0, g = 0, bl = 0, n = 0;
        int x, y, z;
        for (x = boxes[i].lo[0]; x <= boxes[i].hi[0]; x++)
            for (y = boxes[i].lo[1]; y <= boxes[i].hi[1]; y++)
                for (z = boxes[i].lo[2]; z <= boxes[i].hi[2]; z++) {
                    unsigned k = hist[x << 10 | y << 5 | z];
                    r += k * (x * 8 + 4.0);
                    g += k * (y * 8 + 4.0);
                    bl += k * (z * 8 + 4.0);
                    n += k;
                }
        if (!n)
            n = 1;
        pal->rgb[first + i][0] = (unsigned char)(r / n);
        pal->rgb[first + i][1] = (unsigned char)(g / n);
        pal->rgb[first + i][2] = (unsigned char)(bl / n);
    }
    return nb;
}

static int nearest(SynthPalette *pal, const unsigned char *p)
{
    int key = (p[0] >> 3) << 10 | (p[1] >> 3) << 5 | p[2] >> 3, i, best = pal->lo;
    long bd = 1L << 30;
    if (pal->cache[key])
        return pal->cache[key] - 1;
    for (i = pal->greys ? 0 : pal->lo; i < pal->hi; i = pal->greys && i == 31 && pal->lo > 32 ? pal->lo : i + 1) {
        long dr = pal->rgb[i][0] - p[0], dg = pal->rgb[i][1] - p[1], db = pal->rgb[i][2] - p[2];
        long d = dr * dr * 3 + dg * dg * 4 + db * db * 2;
        if (d < bd) {
            bd = d;
            best = i;
        }
    }
    pal->cache[key] = (short)(best + 1);
    return best;
}

/* A whole picture in the palette's colours, Floyd-Steinberg dithered; its
   transparent pixels (the pack's vignettes) a dark grey, as the original's
   pictures have none. */
static void dither(unsigned char *idx, const Canvas *c, SynthPalette *pal)
{
    int x, y, k;
    float *err = calloc((size_t)(c->w + 2) * 2 * 3, sizeof *err);
    for (y = 0; y < c->h; y++) {
        float *cur = err + (size_t)(y & 1) * (c->w + 2) * 3, *nxt = err + (size_t)((y + 1) & 1) * (c->w + 2) * 3;
        memset(nxt, 0, (size_t)(c->w + 2) * 3 * sizeof *nxt);
        for (x = 0; x < c->w; x++) {
            const unsigned char *p = c->px + ((size_t)y * c->w + x) * 4;
            unsigned char want[3];
            int i;
            if (p[3] < 128) {   /* (a vignette's ground: a dark grey) */
                static const unsigned char ground[3] = {56, 52, 48};
                idx[y * c->w + x] = (unsigned char)nearest(pal, ground);
                continue;
            }
            for (k = 0; k < 3; k++) {
                float v = p[k] + cur[(x + 1) * 3 + k];
                want[k] = (unsigned char)(v < 0 ? 0 : v > 255 ? 255 : v);
            }
            i = nearest(pal, want);
            idx[y * c->w + x] = (unsigned char)i;
            for (k = 0; k < 3; k++) {
                float e = (float)want[k] - pal->rgb[i][k];
                cur[(x + 2) * 3 + k] += e * 7 / 16;
                nxt[x * 3 + k] += e * 3 / 16;
                nxt[(x + 1) * 3 + k] += e * 5 / 16;
                nxt[(x + 2) * 3 + k] += e * 1 / 16;
            }
        }
    }
    free(err);
}

/* The special indices: transparent, a cell's border, the background. */
enum { CLEAR = 253, BORDER = 254, BACK = 255, MARK3 = 249, MARK4 = 250, SHIELD_RED = 252 };

static void special_colours(SynthPalette *pal)
{
    static const unsigned char top[10][3] = {
        {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 160}, {0, 0, 255},      /* 246 .. 250 (249, 250: markers) */
        {128, 0, 0}, {255, 0, 0}, {255, 0, 255}, {0, 255, 0}, {136, 136, 136},  /* 251 .. 255 */
    };
    memcpy(pal->rgb[246], top, sizeof top);
}

/* ---- GIF ---- */

typedef struct { unsigned char *p; size_t n, cap; } Buf;

static void put(Buf *b, const void *d, size_t n)
{
    if (b->n + n > b->cap) {
        b->cap = (b->n + n) * 2 + 1024;
        b->p = realloc(b->p, b->cap);
    }
    memcpy(b->p + b->n, d, n);
    b->n += n;
}

static void put8(Buf *b, int v) { unsigned char c = (unsigned char)v; put(b, &c, 1); }
static void put16(Buf *b, int v) { put8(b, v & 255); put8(b, v >> 8); }

typedef struct {
    Buf *out;
    unsigned char block[256];
    int nblock;
    unsigned acc;
    int bits;
} Bits;

static void bits_flush_block(Bits *s)
{
    if (s->nblock) {
        put8(s->out, s->nblock);
        put(s->out, s->block, (size_t)s->nblock);
        s->nblock = 0;
    }
}

static void bits_put(Bits *s, int code, int width)
{
    s->acc |= (unsigned)code << s->bits;
    s->bits += width;
    while (s->bits >= 8) {
        s->block[s->nblock++] = (unsigned char)(s->acc & 255);
        s->acc >>= 8;
        s->bits -= 8;
        if (s->nblock == 255)
            bits_flush_block(s);
    }
}

/* An 8-bit picture as a GIF (87a, the global colour table only, as the
   game's reader wants it). */
static unsigned char *gif_encode(const unsigned char *idx, int w, int h, const SynthPalette *pal, size_t *len)
{
    Buf b = {0};
    Bits s;
    static int dict[4096 * 256];       /* (prefix code, byte) -> code + 1 */
    int next, width, prefix = -1, i;
    size_t n = (size_t)w * h, k;
    put(&b, "GIF87a", 6);
    put16(&b, w);
    put16(&b, h);
    put8(&b, 0xf7);                     /* global table, 256 colours */
    put8(&b, 0);
    put8(&b, 0);
    for (i = 0; i < 256; i++)
        put(&b, pal->rgb[i], 3);
    put8(&b, ',');
    put16(&b, 0);
    put16(&b, 0);
    put16(&b, w);
    put16(&b, h);
    put8(&b, 0);
    put8(&b, 8);                        /* minimum code size */
    memset(&s, 0, sizeof s);
    s.out = &b;
    memset(dict, 0, sizeof dict);
    next = 258;
    width = 9;
    bits_put(&s, 256, width);
    for (k = 0; k < n; k++) {
        int c = idx[k];
        if (prefix < 0) {
            prefix = c;
            continue;
        }
        if (dict[prefix * 256 + c]) {
            prefix = dict[prefix * 256 + c] - 1;
            continue;
        }
        bits_put(&s, prefix, width);
        if (next < 4096) {
            dict[prefix * 256 + c] = next + 1;
            if (next == (1 << width) && width < 12)
                width++;
            next++;
        } else {
            bits_put(&s, 256, width);   /* the table full: start again */
            memset(dict, 0, sizeof dict);
            next = 258;
            width = 9;
        }
        prefix = c;
    }
    if (prefix >= 0)
        bits_put(&s, prefix, width);
    bits_put(&s, 257, width);
    if (s.bits)
        s.block[s.nblock++] = (unsigned char)(s.acc & 255);
    bits_flush_block(&s);
    put8(&b, 0);
    put8(&b, ';');
    *len = b.n;
    return b.p;
}

/* ---- a picture's sprites ---- */

typedef struct { int x, y, w, h; char key[160]; } Crop;
typedef struct { Crop *c; int n; } Crops;

static void add_crop(const char *key, int w, int h, void *ctx)
{
    Crops *cs = ctx;
    const char *r = strrchr(key, '/');
    Crop c;
    (void)w; (void)h;
    if (!r || sscanf(r + 1, "%d,%d,%d,%d", &c.x, &c.y, &c.w, &c.h) != 4 || c.w <= 0 || c.h <= 0)
        return;
    snprintf(c.key, sizeof c.key, "%s", key);
    cs->c = realloc(cs->c, (size_t)(cs->n + 1) * sizeof *cs->c);
    cs->c[cs->n++] = c;
}

static Crops crops_of(const char *base)
{
    Crops cs = {0};
    char prefix[200];
    snprintf(prefix, sizeof prefix, "crop/%s/", base);
    hd_art_each(prefix, add_crop, &cs);
    return cs;
}

/* The canvas with each crop's picture in its rectangle. */
static void draw_crops(Canvas *c, const Crops *cs)
{
    int i;
    for (i = 0; i < cs->n; i++)
        put_art(c, cs->c[i].key, cs->c[i].x, cs->c[i].y, cs->c[i].w, cs->c[i].h);
}

static void border(unsigned char *idx, int w, int h, int x, int y, int cw, int ch, int colour)
{
    int i;
    for (i = x - 1; i <= x + cw; i++) {
        if (i < 0 || i >= w)
            continue;
        if (y - 1 >= 0) idx[(y - 1) * w + i] = (unsigned char)colour;
        if (y + ch < h) idx[(y + ch) * w + i] = (unsigned char)colour;
    }
    for (i = y; i < y + ch; i++) {
        if (i < 0 || i >= h)
            continue;
        if (x - 1 >= 0) idx[i * w + x - 1] = (unsigned char)colour;
        if (x + cw < w) idx[i * w + x + cw] = (unsigned char)colour;
    }
}

/* The windows' textures (ICONS: the dialogs' ground, the frames' border
   tile) in the greys alone, as the original's: windows with palettes of
   their own making (the diplomacy room's) keep the game's greys only. */
static const char *const GREY_CROPS[] = {"crop/file/icons/298,190,32,32", "crop/file/icons/199,322,64,32"};

/* The crops' rectangles in indices: art where opaque, else clear; a border
   round each. */
static void index_crops(unsigned char *idx, const Canvas *c, const Crops *cs, SynthPalette *pal, int clear)
{
    static SynthPalette greys;
    int i, x, y;
    size_t k;
    memcpy(greys.rgb, pal->rgb, sizeof greys.rgb);
    memset(greys.cache, 0, sizeof greys.cache);
    greys.lo = 0;
    greys.hi = 32;
    greys.greys = 0;
    for (i = 0; i < cs->n; i++)
        border(idx, c->w, c->h, cs->c[i].x, cs->c[i].y, cs->c[i].w, cs->c[i].h, BORDER);
    for (i = 0; i < cs->n; i++) {
        SynthPalette *use = pal;
        for (k = 0; k < sizeof GREY_CROPS / sizeof *GREY_CROPS; k++)
            if (!strcmp(cs->c[i].key, GREY_CROPS[k]))
                use = &greys;
        for (y = cs->c[i].y; y < cs->c[i].y + cs->c[i].h && y < c->h; y++)
            for (x = cs->c[i].x; x < cs->c[i].x + cs->c[i].w && x < c->w; x++) {
                const unsigned char *p = c->px + ((size_t)y * c->w + x) * 4;
                idx[y * c->w + x] = (unsigned char)(p[3] >= 128 ? nearest(use, p) : clear);
            }
    }
}

/* ---- the sheets (TERRAIN1.GIF ...) ---- */

/* (CITY.GIF, the credits' and the editors' backdrops are whole pictures: the
   editors' are the art's file/editoras ... where it has them, else left
   dark) */

static const struct Sheet {
    const char *file, *key;
    int w, h;
} SHEETS[] = {
    {"TERRAIN1.GIF", "file/terrain1", 640, 480},
    {"TERRAIN2.GIF", "file/terrain2", 640, 480},
    {"UNITS.GIF", "file/units", 640, 480},
    {"CITIES.GIF", "file/cities", 640, 480},
    {"ICONS.GIF", "file/icons", 640, 480},
    {"PEOPLE.GIF", "file/people", 640, 480},
    {"EDITORPT.GIF", "file/editorpt", 640, 480},
    {"CITY.GIF", "file/city", 640, 480},
    {"SCREDITS.GIF", "file/scredits", 600, 400},
    {"EDITORAS.GIF", "file/editoras", 640, 480},
    {"EDITORSA.GIF", "file/editorsa", 640, 480},
    {"EDITORSQ.GIF", "file/editorsq", 640, 480},
};
#define NSHEETS (int)(sizeof SHEETS / sizeof *SHEETS)

/* The shared palette's fixed colours: those the game's code draws with by
   index (its palette's entries 10 to 201 are the GIF's 0 to 191; text, lines
   and fills by colour number, as measured in the recorded scenarios): a grey
   ramp (0 to 31) and the others below, near the original's; among them the
   civilizations' colours, which the game reads from CITIES (CIV_INDEX: the
   flag, its darker shade, the text's), in red, white, green, blue, yellow,
   cyan, orange and purple.  The art takes the other entries. */
static const struct { unsigned char i, rgb[3]; } FIXED[] = {
    {32, {88, 168, 40}}, {33, {80, 160, 40}}, {34, {72, 152, 32}}, {35, {72, 144, 32}}, {36, {64, 136, 32}},
    {38, {56, 120, 24}}, {43, {24, 80, 8}}, {45, {8, 64, 8}}, {47, {0, 48, 0}}, {71, {80, 104, 192}},
    {72, {72, 96, 184}}, {74, {64, 80, 168}}, {75, {56, 72, 160}}, {76, {48, 56, 152}}, {82, {8, 8, 104}},
    {83, {0, 0, 96}}, {84, {64, 184, 200}}, {85, {56, 176, 192}}, {89, {32, 136, 152}}, {90, {32, 120, 144}},
    {96, {240, 0, 0}}, {101, {168, 0, 0}}, {104, {255, 255, 0}}, {107, {240, 184, 8}}, {108, {240, 160, 8}},
    {109, {232, 128, 8}}, {111, {224, 80, 16}}, {112, {255, 224, 80}}, {114, {224, 184, 64}}, {119, {136, 96, 24}},
    {125, {152, 88, 88}}, {129, {248, 200, 176}}, {146, {56, 32, 8}}, {162, {48, 88, 184}}, {165, {112, 216, 48}},
    {166, {0, 112, 255}}, {167, {184, 144, 255}}, {173, {128, 104, 176}}, {175, {112, 64, 136}},
    {176, {255, 240, 208}}, {180, {224, 216, 176}}, {188, {168, 152, 120}},
};
static const unsigned char CIV_INDEX[8][3] = {
    {96, 101, 96}, {29, 21, 31}, {32, 43, 165}, {72, 82, 166},
    {104, 107, 104}, {85, 90, 84}, {109, 111, 107}, {173, 175, 167},
};
#define GREYS 32

static SynthPalette shared;
static unsigned char shared_fixed_used[192];
static int shared_made;

static void shared_fixed(SynthPalette *pal)
{
    size_t i;
    memset(pal, 0, sizeof *pal);
    memset(shared_fixed_used, 0, sizeof shared_fixed_used);
    for (i = 0; i < GREYS; i++) {
        pal->rgb[i][0] = pal->rgb[i][1] = pal->rgb[i][2] = (unsigned char)((i * 255 + 15) / 31);
        shared_fixed_used[i] = 1;
    }
    for (i = 0; i < sizeof FIXED / sizeof *FIXED; i++) {
        memcpy(pal->rgb[FIXED[i].i], FIXED[i].rgb, 3);
        shared_fixed_used[FIXED[i].i] = 1;
    }
    special_colours(pal);
}

/* The art's colours into the shared palette's free entries. */
static void shared_art(SynthPalette *pal, const unsigned *hist)
{
    SynthPalette tmp;
    int free_n = 0, i, k = 0, n;
    for (i = 0; i < 192; i++)
        free_n += !shared_fixed_used[i];
    memset(&tmp, 0, sizeof tmp);
    n = median_cut(hist, &tmp, 0, free_n);
    for (i = 0; i < 192 && k < n; i++)
        if (!shared_fixed_used[i])
            memcpy(pal->rgb[i], tmp.rgb[k++], 3);
    pal->lo = 0;
    pal->hi = 192;
}

/* TERRAIN1's masks (row y 447): the blends' dither (x 1: the terrain kept
   where it is index 0, the game's 10, which the game draws transparent),
   the map's picking mask (x 66: the tile's own diamond and its four
   corners, the neighbour by index / 16) and the directions' (x 261: the
   eight directions and the middle, by index / 16). */
static int in_diamond(int x, int y)
{
    double dx = fabs(x - 31.5) / 32.0, dy = fabs(y - 15.5) / 16.0;
    return dx + dy <= 1.0;
}

static void terrain1_masks(unsigned char *idx, int w)
{
    int x, y;
    for (y = 0; y < 32; y++)
        for (x = 0; x < 64; x++) {
            unsigned char *d = idx + (447 + y) * w;
            double edge = 1.0 - (fabs(x - 31.5) / 32.0 + fabs(y - 15.5) / 16.0);   /* 0 at the edge */
            unsigned hash = (unsigned)(x * 73856093u ^ y * 19349663u);
            int corner, dir, col, row;
            /* the dither: the terrain kept at scattered pixels near the edge */
            if (!in_diamond(x, y))
                d[1 + x] = BACK;
            else
                d[1 + x] = (unsigned char)(edge < 0.16 && ((x + y) & 1) && (hash >> 7) % 3 ? 0 : CLEAR);
            /* picking: 0 the tile, 1 top right, 2 bottom right, 3 bottom left, 4 top left */
            corner = in_diamond(x, y) ? 0 : y < 16 ? (x < 32 ? 4 : 1) : (x < 32 ? 3 : 2);
            d[66 + x] = (unsigned char)(corner * 16 + 3);
            /* directions: 0 top right, clockwise to 7 top; 8 the middle */
            col = x < 16 ? 0 : x < 48 ? 1 : 2;
            row = y < 8 ? 0 : y < 24 ? 1 : 2;
            {
                static const int DIRS[3][3] = {{6, 7, 0}, {5, 8, 1}, {4, 3, 2}};
                dir = x >= 14 && x < 50 && y >= 8 && y < 26 ? 8 : DIRS[row][col];
            }
            d[261 + x] = (unsigned char)(dir * 16 + 3);
        }
}

/* UNITS: each unit's shield placed by the markers on its cell's top and left
   borders (index 250, the game's 4), the shield (x 586, y 1) in the colour
   the game recolours for each owner (252, the game's 6). */
static void units_marks(unsigned char *idx, const Canvas *c, int w)
{
    int u, x, y;
    for (u = 0; u < 63; u++) {
        int cx = 1 + (u % 9) * 65, cy = 1 + (u / 9) * 49;
        idx[(cy - 1) * w + cx + 12] = MARK4;
        idx[(cy + 8) * w + cx - 1] = MARK4;
    }
    for (y = 1; y < 21; y++)
        for (x = 586; x < 598; x++)
            idx[y * w + x] = (unsigned char)(c->px[((size_t)y * c->w + x) * 4 + 3] >= 128 ? SHIELD_RED : BACK);
}

/* CITIES: the flag's place (250) and the size box's (249) on each city's
   cell's borders; the civilizations' colours where the game reads them
   (beside each flag: x + 3 and x + 6 on row 429, x on row 423). */
static void cities_marks(unsigned char *idx, int w)
{
    int style, size, col, civ;
    for (style = 0; style < 6; style++)
        for (size = 0; size < 4; size++)
            for (col = 0; col < 2; col++) {
                int cx = (col ? 0x14e : 1) + size * 0x41, cy = 39 + style * 0x31;
                idx[(cy - 1) * w + cx + 1] = MARK4;      /* the flag: x 0 */
                idx[(cy - 1) * w + cx + 50] = MARK3;     /* the size box */
                idx[(cy + 15) * w + cx - 1] = MARK4;     /* the flag: y 0 */
                idx[(cy + 20) * w + cx - 1] = MARK3;
            }
    for (civ = 0; civ < 8; civ++) {
        int fx = 1 + civ * 15;
        idx[429 * w + fx + 3] = CIV_INDEX[civ][0];
        idx[429 * w + fx + 6] = CIV_INDEX[civ][1];
        idx[423 * w + fx] = CIV_INDEX[civ][2];
    }
}

static unsigned char *sheet_gif[NSHEETS];
static size_t sheet_len[NSHEETS];
static int sheet_written[NSHEETS];

static unsigned char *grey_picture(int w, int h, SynthPalette *pal, size_t *len);

/* All the sheets at once: they share a palette. */
static void make_sheets(void)
{
    Canvas cv[NSHEETS];
    Crops cs[NSHEETS];
    int i, art = hd_art_open();
    unsigned *hist = calloc(32768, sizeof *hist);
    clock_t t0 = clock();
    memset(cs, 0, sizeof cs);
    for (i = 0; i < NSHEETS; i++) {
        canvas_init(&cv[i], SHEETS[i].w, SHEETS[i].h);
        if (!art)
            continue;
        cs[i] = crops_of(SHEETS[i].key);
        if (!cs[i].n) {         /* a whole picture: CITY.GIF's, the credits' (at its own size if it fits) */
            int pw, ph, aw, ah;
            unsigned char *px = hd_art_copy(SHEETS[i].key, &pw, &ph, &aw, &ah);
            if (px) {
                if (aw > SHEETS[i].w || ah > SHEETS[i].h)
                    aw = SHEETS[i].w, ah = SHEETS[i].h;
                put_scaled(&cv[i], px, pw, ph, 0, 0, aw, ah);
                free(px);
            }
        } else
            draw_crops(&cv[i], &cs[i]);
        /* (the editors' backdrops, dark slate, are left out of the palette's
           art colours: in its greys, the others' colours as without them) */
        if (strncasecmp(SHEETS[i].file, "EDITORS", 7) && strcasecmp(SHEETS[i].file, "EDITORAS.GIF"))
            count_colours(&cv[i], hist);
    }
    shared_fixed(&shared);
    shared_art(&shared, hist);
    shared_made = 1;
    for (i = 0; i < NSHEETS; i++) {
        size_t n = (size_t)cv[i].w * cv[i].h;
        unsigned char *idx = malloc(n);
        memset(idx, BACK, n);
        if (cs[i].n)
            index_crops(idx, &cv[i], &cs[i], &shared, CLEAR);
        else
            dither(idx, &cv[i], &shared);
        if (!strcmp(SHEETS[i].file, "TERRAIN1.GIF"))
            terrain1_masks(idx, cv[i].w);
        else if (!strcmp(SHEETS[i].file, "UNITS.GIF"))
            units_marks(idx, &cv[i], cv[i].w);
        else if (!strcmp(SHEETS[i].file, "CITIES.GIF"))
            cities_marks(idx, cv[i].w);
        sheet_gif[i] = gif_encode(idx, cv[i].w, cv[i].h, &shared, &sheet_len[i]);
        free(idx);
        free(cv[i].px);
        free(cs[i].c);
    }
    free(hist);
    plat_log("synth: the sheets made in %.1f s\n", (double)(clock() - t0) / CLOCKS_PER_SEC);
}

/* A picture the art has nothing for: neutral grey. */
static unsigned char *grey_picture(int w, int h, SynthPalette *pal, size_t *len)
{
    static const unsigned char grey[3] = {96, 96, 96};
    unsigned char *idx = malloc((size_t)w * h), *g;
    memset(idx, nearest(pal, grey), (size_t)w * h);
    g = gif_encode(idx, w, h, pal, len);
    free(idx);
    return g;
}

/* The game folder lacks name (a file name, no folder): if it is one of the
   sheets, it is written to the pictures' folder; 1 and its host path, if so. */
int synth_file(const char *name, char *host, size_t n)
{
    int i;
    char dir[1100];
    FILE *f;
    snprintf(dir, sizeof dir, "%s/OpenCiv2Pictures", plat_win_dir);
    if (synth_module(name) || !strcasecmp(name, "civ2.exe")) {
        /* a picture library, civ2.exe: there, empty (the game looks for
           them before it loads them; resources.c makes their pictures) */
        snprintf(host, n, "%s/%s", dir, name);
        mkdir(dir, 0777);
        if (!(f = fopen(host, "ab")))
            return 0;
        fclose(f);
        return 1;
    }
    for (i = 0; i < NSHEETS && strcasecmp(name, SHEETS[i].file); i++) {}
    if (i == NSHEETS)
        return 0;
    snprintf(host, n, "%s/%s", dir, SHEETS[i].file);
    if (sheet_written[i])
        return 1;
    if (!sheet_gif[i])
        make_sheets();
    if (!sheet_gif[i])
        return 0;
    mkdir(dir, 0777);
    if (!(f = fopen(host, "wb")) || fwrite(sheet_gif[i], 1, sheet_len[i], f) != sheet_len[i]) {
        if (f)
            fclose(f);
        plat_log("synth: cannot write %s\n", host);
        return 0;
    }
    fclose(f);
    sheet_written[i] = 1;
    plat_log("synth: %s made from the art\n", SHEETS[i].file);
    return 1;
}

/* The first of the original's pictures the game folder lacks (a sheet, a
   picture library, civ2.exe), NULL if it has them all: then nothing here is
   used.  Without them the game runs with HD graphics, whatever is chosen
   (main.c: plat_hd_forced; docs/NO-INSTALL.md): the 8-bit pictures made here
   are what the game's code reads, the art what is shown. */
const char *synth_missing(void)
{
    static const char *const OTHERS[] = {"civ2.exe", "Civ2Art.dll", "Intro.dll", "cv.dll", "mk.dll",
                                         "pv.dll", "ss.dll", "Tiles.dll", "Wonder.dll"};
    int i;
    for (i = 0; i < NSHEETS + (int)(sizeof OTHERS / sizeof *OTHERS); i++) {
        const char *name = i < NSHEETS ? SHEETS[i].file : OTHERS[i - NSHEETS];
        if (!plat_game_file_exists(name))
            return name;
    }
    return NULL;
}

/* ---- the libraries' pictures (Tiles.dll ...) ---- */

/* Whether the module (a file name, lower case) is one of the original's
   picture libraries. */
int synth_module(const char *module)
{
    size_t i;
    for (i = 0; i < sizeof RESOURCES / sizeof *RESOURCES; i++)
        if (!strcasecmp(RESOURCES[i].module, module))
            return 1;
    return 0;
}

typedef struct Made {
    const struct Res *res;
    unsigned char *gif;
    size_t len;
    struct Made *next;
} Made;
static Made *made;

/* The pictures whose colours the game takes, entries first .. first +
   count - 1 (the GIF's: the palette's are 10 more), into a palette of its
   own making, beside the game's greys (0 .. 31): the diplomacy room's
   (DiploLeaderWindow: the leader's portrait 32 .. 95, the room and its
   panels 96 .. 134, 135 the civilization's colour; the destroyed
   civilizations' window the same portraits, its backgrounds 96 .. 235);
   and two loaded into the game's own palette, their 32 colours in place of
   its greys (mk.dll 299, the diplomacy's tile; cv.dll 399, the city view's
   icons): in those greys (count 0). */
static const struct { const char *module; int lo, hi, first, count; } OWN[] = {
    {"mk.dll", 200, 215, 96, 39},
    {"mk.dll", 220, 261, 32, 64},
    {"mk.dll", 299, 299, 0, 0},
    {"mk.dll", 10000, 10002, 96, 140},
    {"cv.dll", 399, 399, 0, 0},
};

/* The picture resource GIFS id of the module, made from the art, if the
   original has it: the GIF's bytes (kept), else NULL. */
const unsigned char *synth_resource(const char *module, int id, unsigned *len)
{
    const struct Res *r = NULL;
    Made *m;
    Canvas c;
    Crops cs = {0};
    SynthPalette *pal;
    char key[200];
    unsigned char *idx;
    size_t i, n;
    int whole = 0, art;
    for (i = 0; i < sizeof RESOURCES / sizeof *RESOURCES; i++)
        if (RESOURCES[i].id == id && !strcasecmp(RESOURCES[i].module, module))
            r = &RESOURCES[i];
    if (!r)
        return NULL;
    for (m = made; m; m = m->next)
        if (m->res == r) {
            *len = (unsigned)m->len;
            return m->gif;
        }
    m = calloc(1, sizeof *m);
    m->res = r;
    canvas_init(&c, r->w, r->h);
    art = hd_art_open();
    snprintf(key, sizeof key, "resource/%s/gifs/%d", r->module, id);
    if (art) {
        /* (not wonder.dll's: the game takes only the movie window's border
           tile from them, 1,1,64,32, and the pack's are each wonder's
           painting, whose corner is no border: the tile is the art's crop
           of that key where it has one, else plain grey) */
        if (strcasecmp(r->module, "wonder.dll"))
            whole = put_art(&c, key, 0, 0, r->w, r->h);
        cs = crops_of(key);
        draw_crops(&c, &cs);
    }
    pal = calloc(1, sizeof *pal);
    n = (size_t)r->w * r->h;
    idx = malloc(n);
    memset(idx, BACK, n);
    /* in the sheets' colours (the game draws many of these pictures, or
       sprites taken from them, in its own palette: the announcements'
       pictures, the advisors' backgrounds), but those whose colours the game
       takes into a palette of its own making (OWN) */
    if (!shared_made)
        make_sheets();
    *pal = shared;
    for (i = 0; i < sizeof OWN / sizeof *OWN; i++)
        if (!strcmp(OWN[i].module, r->module) && id >= OWN[i].lo && id <= OWN[i].hi) {
            /* colours of its own, in the entries the game takes from it */
            unsigned *hist = calloc(32768, sizeof *hist);
            count_colours(&c, hist);
            memset(pal->cache, 0, sizeof pal->cache);
            pal->lo = OWN[i].first;
            pal->hi = OWN[i].first + (OWN[i].count ? median_cut(hist, pal, OWN[i].first, OWN[i].count) : 32);
            pal->greys = OWN[i].first >= 32;
            if (pal->hi == pal->lo)
                pal->hi++;
            free(hist);
        }
    if (!whole && !cs.n) {
        m->gif = grey_picture(r->w, r->h, pal, &m->len);
        plat_log("synth: %s has no art: grey\n", key);
    } else {
        if (cs.n)
            index_crops(idx, &c, &cs, pal, BACK);
        else
            dither(idx, &c, pal);
        m->gif = gif_encode(idx, r->w, r->h, pal, &m->len);
        plat_log("synth: %s made from the art (%s%d sprites)\n", key, whole ? "whole, " : "", cs.n);
    }
    free(idx);
    free(pal);
    free(c.px);
    free(cs.c);
    m->next = made;
    made = m;
    *len = (unsigned)m->len;
    return m->gif;
}

/* ---- civ2.exe's bitmaps, cursors and icons ---- */

/* The pack's picture of civ2.exe's resource TYPE/ID at w x h, RGBA (free()),
   drawn into the rectangle x, y, cw, ch of it; NULL: none. */
unsigned char *synth_exe_rgba(int type, int id, int w, int h, int x, int y, int cw, int ch)
{
    Canvas c;
    char key[64];
    snprintf(key, sizeof key, "resource/civ2.exe/%d/%d", type, id);
    if (!hd_art_open())
        return NULL;
    canvas_init(&c, w, h);
    if (!put_art(&c, key, x, y, cw, ch)) {
        free(c.px);
        return NULL;
    }
    return c.px;
}
