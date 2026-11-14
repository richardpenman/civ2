/*
 * HD graphics (src/platform/hd.h, docs/HD-GRAPHICS.md).
 *
 * The art: Peter Johnson's runtime pack (openciv2-assets, MIT), its file
 * data/atlas-hd.pack: "C2ATLAS1", a count, a palette (768 bytes), then per
 * picture a 180-byte entry (key, 160 bytes; its size in the game's pixels;
 * the offset and length of its PNG; an offset of indexed pixels, unused
 * here), the keys in ascending order.  A sprite the game takes from one of
 * its sheets at x,y,w,h has the picture "crop/file/SHEET/x,y,w,h" where the
 * pack has one (4 to 8 times as large); terrain blends are bound by role
 * ("dither/T/Q", "fog/Q").  (The browser version gives the game only the
 * pictures keyed crop/, tint/, dither/, fog/ and file/: src/web/hdpack.js,
 * KEEP; a picture of another kind drawn here is to be added there.)
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
#include "plat.h"
#include "hd.h"
#include "../host/host.h"
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_MAX_DIMENSIONS 16384
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image/stb_image.h"

extern char plat_data_dir[];
extern char plat_game_dir[];
int plat_profile_string(const char *file, const char *section, const char *key, char *out, int n);

int hd_on;
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
    int ch;
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
    a->rgba = stbi_load_from_memory(png, (int)a->len, &a->pw, &a->ph, &ch, 4);
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
            if (opaque) {
                if (!in_diamond(x, y, w, h)) {
                    out[y * w + x] = 0;
                    continue;
                }
                if (al > 0) { r /= al; g /= al; b /= al; }
                else { r = rr / n; g = gg / n; b = bb / n; }
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
    for (j = 0; j < d; j++, p += pitch)
        for (i = 0; i < d; i++)
            p[i] = c;
}

void hd_bitmap_free(Bitmap *b)
{
    free(b->hd);
    free(b->hd_prev);
    b->hd = NULL;
    b->hd_prev = NULL;
}

static int valid(const Bitmap *b)
{
    return b && b->hd && b->hd_w == b->w && b->hd_h == b->h && b->hd_bits == b->bits;
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
    if (!b->hd || !b->hd_prev) {
        hd_bitmap_free(b);
        return 0;
    }
    b->hd_w = b->w;
    b->hd_h = b->h;
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

/* The game's pixels in the rectangle are shown as the plane has them. */
static void commit_rect(Bitmap *b, int l, int t, int r, int bot)
{
    int n, y;
    if (!valid(b) || !clip_rect(b, &l, &t, &r, &bot))
        return;
    n = pixel_bytes(b);
    for (y = t; y < bot; y++)
        memcpy(b->hd_prev + ((size_t)y * b->w + l) * n, bmp_row(b, y) + l * n, (size_t)(r - l) * n);
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
    if (w == sw && h == sh) {
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
    commit_rect(dst, l, t, r, b);
}

unsigned int *hd_bitmap_plane(Bitmap *b)
{
    return hd_on && valid(b) ? b->hd : NULL;
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

/* ---- sheets and sprites ---- */

void hd_sheet_clear(void *bits)
{
    Bitmap *b;
    if (hd_on && (b = bmp_of_bits(bits)))
        b->hd_sheet[0] = 0;
}

void hd_sheet_loaded(void *bits, const char *filename)
{
    Bitmap *b;
    char cwd[1024], key[200];
    const char *base = filename, *s;
    size_t i;
    if (!hd_on || !(b = bmp_of_bits(bits)))
        return;
    b->hd_sheet[0] = 0;
    /* a scenario's own art is kept: only the game's folder's sheets */
    if (!getcwd(cwd, sizeof cwd) || strcmp(cwd, plat_game_dir))
        return;
    for (s = filename; *s; s++)
        if (*s == '/' || *s == '\\' || *s == ':')
            base = s + 1;
    for (i = 0; base[i] && base[i] != '.' && i < sizeof b->hd_sheet - 1; i++)
        b->hd_sheet[i] = (char)tolower((unsigned char)base[i]);
    b->hd_sheet[i] = 0;
    snprintf(key, sizeof key, "file/%s", b->hd_sheet);
    if (!find(key))
        b->hd_sheet[0] = 0;
}

typedef struct Spr {
    const void *sprite;
    unsigned handle;
    int id, opaque;             /* the pack's picture (variant: in a civilization's colour), or */
    int variant;
    uint32_t *snap;             /* the plane's pixels it was taken from, */
    int sw, sh;                 /* sw x sh */
    struct Spr *next;
} Spr;
static Spr *sprites[4096];

static Spr **bucket(const void *s)
{
    uintptr_t k = (uintptr_t)s;
    return &sprites[((k >> 2) ^ (k >> 14)) & 4095];
}

static Spr *lookup(const void *s, unsigned handle)
{
    Spr *p;
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
        snprintf(name, sizeof name, "crop/file/%s/%d,%d,%d,%d", b->hd_sheet, x, y, w, h);
        if ((id = find(name)) && (p = add(s, handle)))
            p->id = id;
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
            p->snap[(size_t)j * w * d + i] = key >= 0 && row[x + i / d] == key ? 0 :
                b->hd[(size_t)(y * d + j) * b->w * d + x * d + i] | 0xff000000u;
    }
}

int hd_sprite_begin(const void *s, unsigned handle, void *bits, int x, int y, int w, int h)
{
    Bitmap *b;
    if (!hd_on || !lookup(s, handle) || !(b = bmp_of_bits(bits)) || !plane(b))
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
    if (silhouette >= 0) {
        RGBQUAD q = b->colors[silhouette & 255];
        tint = (int64_t)q.rgbRed << 16 | q.rgbGreen << 8 | q.rgbBlue;
    }
    if (p->snap)
        paint(b, p->snap, p->sw, p->sh, x * d, y * d, w * d, h * d, clip[0] * d, clip[1] * d, clip[2] * d,
              clip[3] * d, tint);
    else {
        const uint32_t *px = picture(p->variant ? p->variant : p->id, w * d, h * d, p->opaque);
        if (px)
            paint(b, px, w * d, h * d, x * d, y * d, w * d, h * d, clip[0] * d, clip[1] * d, clip[2] * d,
                  clip[3] * d, tint);
    }
    commit_rect(b, drawn[0], drawn[1], drawn[2], drawn[3]);
}

/* ---- the desktop ---- */

static Bitmap *frame_of;        /* the screen, whose plane is the frame */

void hd_compose_begin(void)
{
    int d = hd_density, x, y, i;
    if (!hd_on || !plane(screen))
        return;
    frame_of = screen;
    /* the background (the desktop's own surface), enlarged */
    for (y = 0; y < root->h && y < screen->h; y++) {
        const unsigned char *row = bmp_row(root, y);
        for (x = 0; x < root->w && x < screen->w; x++) {
            uint32_t c = logical(root, row, x), *p = screen->hd + (size_t)y * d * screen->w * d + (size_t)x * d;
            for (i = 0; i < d; i++, p += screen->w * d) {
                int k;
                for (k = 0; k < d; k++)
                    p[k] = c;
            }
        }
    }
}

/* Surface s shows at dx, dy; this piece is screen rectangle l, t, r, b. */
void hd_compose_piece(Bitmap *s, int dx, int dy, int l, int t, int r, int b)
{
    int d = hd_density, x, y, i;
    if (!hd_on || !valid(screen))
        return;
    if (l < dx) l = dx;
    if (t < dy) t = dy;
    if (r > dx + s->w) r = dx + s->w;
    if (b > dy + s->h) b = dy + s->h;
    if (!clip_rect(screen, &l, &t, &r, &b))
        return;
    /* every window shown gets a plane: what it draws next (text) is drawn
       there sharp */
    if (plane(s)) {
        sync_rect(s, l - dx, t - dy, r - dx, b - dy);
        for (y = t * d; y < b * d; y++)
            memcpy(screen->hd + (size_t)y * screen->w * d + l * d,
                   s->hd + (size_t)(y - dy * d) * s->w * d + (l - dx) * d, (size_t)(r - l) * d * 4);
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

void hd_compose_end(const unsigned char *lut)
{
    size_t i, n;
    if (!hd_on || !valid(screen))
        return;
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
   opening window installs it, src/host/host_launcher.c).  1 if found. */
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

void hd_init(int classic, int unit_test, int scale)
{
    char path[1100], value[16];
    const char *test = getenv("CIV2_TEST_HD");
    int want;
    if (classic || unit_test)
        return;
    if (plat_replay) {
        /* replays are compared with the original: only on request */
        if (!test || atoi(test) < 1)
            return;
        want = atoi(test);
    } else {
        /* the command line's --hd[=N] (CIV2_HD_ARG), else CIV2_HD=N, else
           CIV.INI's Graphics=HD: 0 off, 1 on, 2 to 4 on at that density */
        const char *arg = getenv("CIV2_HD_ARG"), *env = getenv("CIV2_HD");
        const char *on = arg && *arg ? arg : env && *env ? env : NULL;
        if (on)
            want = atoi(on) > 1 ? atoi(on) : atoi(on) == 1 || !strcasecmp(on, "on") ? 0 : -1;
        else {
            plat_profile_string("CIV.INI", "OpenCiv2", "Graphics", value, sizeof value);
            want = strcasecmp(value, "HD") ? -1 : 0;
        }
        if (want < 0)
            return;
        if (!want)
            want = (int)GetPrivateProfileIntA("OpenCiv2", "GraphicsDensity", 0, "CIV.INI");
        if (want < 1)
            want = scale;           /* a plane pixel per pixel of the display */
    }
    hd_density = want < 1 ? 1 : want > 4 ? 4 : want;
    if (!hd_pack_path(path, sizeof path))
        plat_log("hd: no art (%s/hd/atlas-hd.pack, nor in the user's folder)\n", plat_data_dir);
    hd_on = open_pack(path);
    if (!hd_on)
        hd_density = 1;
}
