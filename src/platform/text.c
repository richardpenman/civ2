/*
 * Fonts and text, from the glyphs the reference environment draws: Times
 * New Roman, Arial, System and Courier at 6..48 pixels, regular/bold/italic.
 * Wine draws 1-bit glyphs on surfaces of 8 bits or less
 * (data/native/fonts_win.bin, tools/probe/fontdump.c) and antialiased ones on
 * deeper surfaces (data/native/fonts_win_aa.bin, tools/probe/fontdump_aa.c),
 * blended as its DIB engine does (tools/probe/aaprobe.c checks the model).
 * Characters beyond Windows-1252 (other code pages, UTF-8): made when first
 * drawn (extra_glyph, below).
 */
#include "plat.h"
#include "hd.h"
#include "../include/utf8.h"
#include "../host/codepages.h"
#include <math.h>

DC *dc_get(HDC h);

typedef struct {
    short adv;                      /* for layout (GetTextExtentPoint) */
    short dadv;                     /* for drawing: Wine's may differ by a pixel */
    signed char x, y;
    unsigned char w, h;
    const unsigned char *bits;      /* (w+7)/8 bytes per row */
} Glyph;

typedef struct {
    short adv;                      /* drawn without antialiasing: the drawing advance */
    signed char x, y;
    unsigned char w, h;
    unsigned short n;
    const unsigned char *rle;       /* run-length coded coverage 0..16 */
    unsigned char *cov;             /* decoded on first use */
} AaGlyph;

typedef struct ExtraGlyph ExtraGlyph;

struct FontFace {
    int face, height, bold, italic;
    int tm[11];
    Glyph g[256];
    AaGlyph *aa;                    /* 256 entries, loaded on demand */
    int aa_mono;                    /* drawn without antialiasing (gasp): advances as drawn */
    ExtraGlyph *extra[64];          /* characters beyond Windows-1252 (extra_glyph), by code point % 64 */
};

#define NFACES 4
#define MINH 6
#define MAXH 48
static FontFace *faces[NFACES][MAXH + 1][2][2];
static unsigned char *font_data, *aa_data;
extern char plat_data_dir[];

/* The font files: captured under Wine with the Liberation 2 fonts, which
   have the widths of Times New Roman, Arial and Courier New, so text is laid
   out as on Windows (with the DejaVu fonts Wine substituted before, wider,
   text overflowed: issue 17).  The original's recordings are made with the
   same fonts (tools/docker). */
static FILE *open_fonts(char *path, size_t size, const char *name)
{
    snprintf(path, size, "%s/%s", plat_data_dir, name);
    return fopen(path, "rb");
}

static void load_aa(void)
{
    char path[1024];
    FILE *f;
    long n;
    unsigned char *p, *end;
    unsigned count, i;
    f = open_fonts(path, sizeof path, "fonts_win_aa.bin");
    if (!f)
        plat_fatal("cannot open %s", path);
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    aa_data = malloc((size_t)n);
    if (fread(aa_data, 1, (size_t)n, f) != (size_t)n || memcmp(aa_data, "C2FONTAA", 8))
        plat_fatal("%s: not an antialiased font file", path);
    fclose(f);
    p = aa_data + 12;
    end = aa_data + n;
    memcpy(&count, aa_data + 8, 4);
    for (i = 0; i < count && p < end; i++) {
        int face = p[0], height = p[1], bold = (p[2] | p[3] << 8) >= 600, it = p[4] != 0, c;
        FontFace *ff = face < NFACES && height >= MINH && height <= MAXH ? faces[face][height][bold][it] : NULL;
        AaGlyph *g = calloc(256, sizeof *g);
        int mono = p[5];
        p += 8 + 44;
        for (c = 0; c < 256; c++) {
            g[c].adv = (short)(p[0] | p[1] << 8);
            g[c].x = (signed char)p[2];
            g[c].y = (signed char)p[3];
            g[c].w = p[4];
            g[c].h = p[5];
            g[c].n = (unsigned short)(p[6] | p[7] << 8);
            g[c].rle = p + 8;
            p += 8 + g[c].n;
        }
        if (ff) {
            ff->aa = g;
            ff->aa_mono = mono;
        }
        else
            free(g);
    }
}

static const unsigned char *aa_coverage(AaGlyph *g)
{
    if (!g->cov && g->w && g->h) {
        const unsigned char *p = g->rle, *end = g->rle + g->n;
        int o = 0, total = g->w * g->h;
        g->cov = calloc((size_t)total, 1);
        while (p < end && o < total) {
            int b = *p++;
            if (b < 0x80) {
                int k;
                for (k = 0; k <= b && p < end && o < total; k++)
                    g->cov[o++] = *p++;
            } else {
                int k, v = p < end ? *p++ : 0;
                for (k = 0; k < b - 0x80 + 2 && o < total; k++)
                    g->cov[o++] = (unsigned char)v;
            }
        }
    }
    return g->cov;
}

/* Wine's grayscale text blend (dibdrv): a gamma ramp of 17 coverage levels. */
static const unsigned char aa_ramp[17] = {0, 0x4d, 0x68, 0x7c, 0x8c, 0x9a, 0xa7, 0xb2, 0xbd, 0xc7, 0xd0,
                                          0xd9, 0xe1, 0xe9, 0xf0, 0xf8, 0xff};

static unsigned aa_blend(unsigned dst, unsigned text, int aa)
{
    unsigned mn, mx;
    if (aa >= 16)
        return text;
    if (aa <= 1 || dst == text)     /* the lowest level leaves the pixel unchanged */
        return dst;
    mn = (aa_ramp[aa] * text) / 0xff;
    mx = aa_ramp[16 - aa] + ((0xff - aa_ramp[16 - aa]) * text) / 0xff;
    if (dst > text)
        return text + ((dst - text) * (mx - text)) / (0xff - text);
    return text - ((text - dst) * (text - mn)) / text;
}

static void load_fonts(void)
{
    char path[1024];
    FILE *f;
    long n;
    unsigned char *p, *end;
    unsigned count, i;
    f = open_fonts(path, sizeof path, "fonts_win.bin");
    if (!f)
        plat_fatal("cannot open %s", path);
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    font_data = malloc((size_t)n);
    if (fread(font_data, 1, (size_t)n, f) != (size_t)n)
        plat_fatal("cannot read %s", path);
    fclose(f);
    if (memcmp(font_data, "C2FONTS2", 8))
        plat_fatal("%s: not a font file", path);
    p = font_data + 8;
    end = font_data + n;
    memcpy(&count, p, 4);
    p += 4;
    for (i = 0; i < count && p < end; i++) {
        FontFace *ff = calloc(1, sizeof *ff);
        int c, k;
        ff->face = p[0];
        ff->height = p[1];
        ff->bold = (p[2] | p[3] << 8) >= 600;
        ff->italic = p[4] != 0;
        p += 8;
        for (k = 0; k < 11; k++, p += 4)
            memcpy(&ff->tm[k], p, 4);
        for (c = 0; c < 256; c++) {
            Glyph *g = &ff->g[c];
            memcpy(&g->adv, p, 2);
            memcpy(&g->dadv, p + 2, 2);
            g->x = (signed char)p[4];
            g->y = (signed char)p[5];
            g->w = p[6];
            g->h = p[7];
            p += 8;
            g->bits = p;
            p += (size_t)((g->w + 7) / 8) * g->h;
        }
        if (ff->face < NFACES && ff->height >= MINH && ff->height <= MAXH)
            faces[ff->face][ff->height][ff->bold][ff->italic] = ff;
    }
}

/* ---- characters beyond Windows-1252's 256 ----
 *
 * The game's text is drawn with the captured glyphs of Windows-1252
 * (above).  A translation in another code page (its file's "@@codepage":
 * iso-8859-3, src/host/codepages.h) or in UTF-8 (Chinese) also has
 * characters those glyphs lack; they are made when first drawn, per font,
 * and kept (extra_glyph):
 *
 * - Latin letters with a mark (Esperanto's ĉ ĝ ĥ ĵ ŝ ŭ, the rest of
 *   Latin-3: marks[] below) are put together from the captured glyphs, so
 *   that they are hinted as the letters beside them: the base letter as
 *   captured, the mark taken from a captured letter that has it (the
 *   circumflex of â and Â, the cedilla of ç and Ç, the dot of i), moved
 *   over the base as Liberation's composite glyphs place it (by half the
 *   difference of the two letters' advances).  Breve and bar, which no
 *   Windows-1252 glyph has, are drawn in the box the circumflex takes, with
 *   the circumflex's stroke.
 * - Other characters (Chinese) are drawn from the font files in
 *   data/native/fonts (tools/cjk_fonts.py: Noto Sans TC and Noto Serif TC,
 *   only the characters the translations use) with stb_truetype, at the
 *   font's pixel height on its baseline: Noto Serif TC for Times New Roman
 *   from 20 pixels up, else Noto Sans TC (a small serif is not legible
 *   with few pixels), bold made by thickening, upright for italics
 *   (Chinese has none).  On 8-bit surfaces (1-bit glyphs) a pixel is set
 *   where it is more than about half covered, or where a thin stroke is
 *   (the stroke's darker pixel across it), so that strokes neither vanish
 *   nor thicken; on deeper ones antialiased (coverage 0..16), but 1-bit
 *   where the font's Latin letters are (Liberation's "gasp" table).
 * - A character neither has is a box (tofu); bytes that are no character
 *   (UTF-8 cut short) are not drawn.
 */
#define STBTT_STATIC
/* (its rasteriser asserts an edge's slope is not negative after flipping
   it, which with x87's extra precision (the 32-bit build) can fail by a
   rounding for a nearly vertical edge; the coverage is right either way) */
#define STBTT_assert(x) ((void)0)
#define STB_TRUETYPE_IMPLEMENTATION
#include "../third_party/stb_truetype/stb_truetype.h"

static unsigned char *read_file(const char *name, long *size)
{
    char path[1100];
    unsigned char *d;
    FILE *f = open_fonts(path, sizeof path, name);
    long n;
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    d = malloc((size_t)n);
    if (!d || fread(d, 1, (size_t)n, f) != (size_t)n) {
        free(d);
        d = NULL;
    }
    fclose(f);
    *size = n;
    return d;
}

static const unsigned short *cp_table;  /* the translation's code page (non-UTF-8), else NULL */
static char codepage[32];
int plat_text_utf8;

struct ExtraGlyph {
    unsigned cp;
    Glyph g;
    AaGlyph a;
    int has_aa;
    ExtraGlyph *next;
};

void plat_text_codepage(const char *name)
{
    size_t i;
    if (!name || !*name || !strcasecmp(name, "windows-1252") || !strcasecmp(name, "cp1252"))
        return;
    if (codepage_is_utf8(name)) {
        snprintf(codepage, sizeof codepage, "utf-8");
        plat_text_utf8 = 1;
        return;
    }
    if (!(cp_table = codepage_table(name))) {
        plat_log("text: no code page %s\n", name);
        return;
    }
    snprintf(codepage, sizeof codepage, "%s", name);
    for (i = 0; codepage[i]; i++)
        if (codepage[i] >= 'A' && codepage[i] <= 'Z')
            codepage[i] = (char)(codepage[i] + 32);
}

const char *plat_text_codepage_name(void)
{
    return codepage[0] ? codepage : "windows-1252";
}

/* -- glyphs as coverage (0..16) in the cell's coordinates, for building -- */

typedef struct {
    int x, y, w, h;                 /* top left, relative to the pen and the cell's top */
    unsigned char *v;
} Pix;

static void pix_free(Pix *p)
{
    free(p->v);
    p->v = NULL;
    p->w = p->h = 0;
}

static unsigned char pix_at(const Pix *p, int x, int y)
{
    x -= p->x;
    y -= p->y;
    return x >= 0 && y >= 0 && x < p->w && y < p->h ? p->v[y * p->w + x] : 0;
}

static void pix_alloc(Pix *p, int x, int y, int w, int h)
{
    p->x = x;
    p->y = y;
    p->w = w > 0 ? w : 0;
    p->h = h > 0 ? h : 0;
    p->v = calloc((size_t)p->w * p->h + 1, 1);
}

/* a captured glyph of byte c (aa: antialiased, f->aa loaded) */
static void pix_of(FontFace *f, int c, int aa, Pix *p)
{
    int x, y;
    if (aa) {
        AaGlyph *g = &f->aa[c];
        const unsigned char *cov = aa_coverage(g);
        pix_alloc(p, g->x, g->y, g->w, g->h);
        if (cov)
            memcpy(p->v, cov, (size_t)g->w * g->h);
        return;
    }
    {
        const Glyph *g = &f->g[c];
        int stride = (g->w + 7) / 8;
        pix_alloc(p, g->x, g->y, g->w, g->h);
        for (y = 0; y < g->h; y++)
            for (x = 0; x < g->w; x++)
                if (g->bits[y * stride + x / 8] & (0x80 >> (x & 7)))
                    p->v[y * g->w + x] = 16;
    }
}

/* the rows and columns with ink: 0 if none */
static int pix_box(const Pix *p, int *x0, int *y0, int *x1, int *y1)
{
    int x, y, any = 0;
    for (y = 0; y < p->h; y++)
        for (x = 0; x < p->w; x++)
            if (p->v[y * p->w + x]) {
                if (!any || x + p->x < *x0) *x0 = x + p->x;
                if (!any || x + p->x > *x1) *x1 = x + p->x;
                if (!any) *y0 = y + p->y;
                *y1 = y + p->y;
                any = 1;
            }
    return any;
}

/* *dst = *dst with src over it (moved by dx, dy), keeping the larger coverage */
static void pix_merge(Pix *dst, const Pix *src, int dx, int dy)
{
    Pix out;
    int x0, y0, x1, y1, x, y;
    if (!src->w || !src->h)
        return;
    if (!dst->w || !dst->h) {
        x0 = src->x + dx; y0 = src->y + dy; x1 = x0 + src->w; y1 = y0 + src->h;
    } else {
        x0 = dst->x < src->x + dx ? dst->x : src->x + dx;
        y0 = dst->y < src->y + dy ? dst->y : src->y + dy;
        x1 = dst->x + dst->w > src->x + dx + src->w ? dst->x + dst->w : src->x + dx + src->w;
        y1 = dst->y + dst->h > src->y + dy + src->h ? dst->y + dst->h : src->y + dy + src->h;
    }
    pix_alloc(&out, x0, y0, x1 - x0, y1 - y0);
    for (y = y0; y < y1; y++)
        for (x = x0; x < x1; x++) {
            unsigned char a = pix_at(dst, x, y), b = pix_at(src, x - dx, y - dy);
            out.v[(y - y0) * out.w + x - x0] = a > b ? a : b;
        }
    pix_free(dst);
    *dst = out;
}

/* the rows of p from y0 to y1 (inclusive) */
static void pix_rows(const Pix *p, int y0, int y1, Pix *out)
{
    int y;
    if (y0 < p->y) y0 = p->y;
    if (y1 > p->y + p->h - 1) y1 = p->y + p->h - 1;
    pix_alloc(out, p->x, y0, p->w, y1 - y0 + 1);
    for (y = y0; y <= y1; y++)
        memcpy(out->v + (y - y0) * p->w, p->v + (y - p->y) * p->w, (size_t)p->w);
}

static void pix_to_glyph(Pix *p, Glyph *g)
{
    int x, y, stride;
    unsigned char *bits;
    int x0 = 0, y0 = 0, x1 = -1, y1 = -1;
    if (!pix_box(p, &x0, &y0, &x1, &y1)) {
        g->x = g->y = 0;
        g->w = g->h = 0;
        g->bits = NULL;
        return;
    }
    stride = (x1 - x0 + 8) / 8;
    bits = calloc((size_t)stride * (y1 - y0 + 1), 1);
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            if (pix_at(p, x, y) >= 8)
                bits[(y - y0) * stride + (x - x0) / 8] |= (unsigned char)(0x80 >> ((x - x0) & 7));
    g->x = (signed char)x0;
    g->y = (signed char)y0;
    g->w = (unsigned char)(x1 - x0 + 1);
    g->h = (unsigned char)(y1 - y0 + 1);
    g->bits = bits;
}

static void pix_to_aa(Pix *p, AaGlyph *a)
{
    int x, y, x0 = 0, y0 = 0, x1 = -1, y1 = -1;
    a->rle = NULL;
    a->n = 0;
    if (!pix_box(p, &x0, &y0, &x1, &y1)) {
        a->x = a->y = 0;
        a->w = a->h = 0;
        a->cov = NULL;
        return;
    }
    a->x = (signed char)x0;
    a->y = (signed char)y0;
    a->w = (unsigned char)(x1 - x0 + 1);
    a->h = (unsigned char)(y1 - y0 + 1);
    a->cov = malloc((size_t)a->w * a->h);
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            a->cov[(y - y0) * a->w + x - x0] = pix_at(p, x, y);
}

/* -- Latin letters with marks, from the captured glyphs -- */

enum { MARK_CIRCUMFLEX, MARK_BREVE, MARK_DOT, MARK_CEDILLA, MARK_BAR, MARK_NONE };
#define MARK_CAP 0x10               /* the capitals' mark (of Â, Ç): higher */
#define MARK_DOTLESS 0x20           /* the base without its dot (ı, ȷ) */

/* (tests/lang_test.py reads this table: every character a code page has
   beyond Windows-1252 must be here) */
static const struct { unsigned short cp; unsigned char base, mark; } marks[] = {
    /* Latin-3 (iso-8859-3): Esperanto */
    {0x0108, 'C', MARK_CIRCUMFLEX | MARK_CAP}, {0x0109, 'c', MARK_CIRCUMFLEX},
    {0x011c, 'G', MARK_CIRCUMFLEX | MARK_CAP}, {0x011d, 'g', MARK_CIRCUMFLEX},
    {0x0124, 'H', MARK_CIRCUMFLEX | MARK_CAP}, {0x0125, 'h', MARK_CIRCUMFLEX | MARK_CAP},
    {0x0134, 'J', MARK_CIRCUMFLEX | MARK_CAP}, {0x0135, 'j', MARK_CIRCUMFLEX | MARK_DOTLESS},
    {0x015c, 'S', MARK_CIRCUMFLEX | MARK_CAP}, {0x015d, 's', MARK_CIRCUMFLEX},
    {0x016c, 'U', MARK_BREVE | MARK_CAP}, {0x016d, 'u', MARK_BREVE},
    /* Latin-3: Maltese, Turkish */
    {0x010a, 'C', MARK_DOT | MARK_CAP}, {0x010b, 'c', MARK_DOT},
    {0x0120, 'G', MARK_DOT | MARK_CAP}, {0x0121, 'g', MARK_DOT},
    {0x017b, 'Z', MARK_DOT | MARK_CAP}, {0x017c, 'z', MARK_DOT},
    {0x0130, 'I', MARK_DOT | MARK_CAP}, {0x0131, 'i', MARK_NONE | MARK_DOTLESS},
    {0x011e, 'G', MARK_BREVE | MARK_CAP}, {0x011f, 'g', MARK_BREVE},
    {0x015e, 'S', MARK_CEDILLA | MARK_CAP}, {0x015f, 's', MARK_CEDILLA},
    {0x0126, 'H', MARK_BAR | MARK_CAP}, {0x0127, 'h', MARK_BAR},
    {0x02d8, 0, MARK_BREVE}, {0x02d9, 0, MARK_DOT},     /* ˘ ˙ alone: in the place of ˆ */
};

static int floor_half(int d)        /* floor(d / 2 + 0.5) */
{
    return d >= 0 ? (d + 1) / 2 : -((-d) / 2);
}

/* the top row of the ink of byte c (the x-height for 'x') */
static int ink_top(FontFace *f, int c, int aa)
{
    Pix p;
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    pix_of(f, c, aa, &p);
    if (!pix_box(&p, &x0, &y0, &x1, &y1))
        y0 = f->tm[1] / 2;
    pix_free(&p);
    return y0;
}

/* The mark of src over base (the rows above base's ink: â over a), or
   (cedilla) under it (the rows below) */
static void mark_of(FontFace *f, int src, int base, int below, int aa, Pix *mark)
{
    Pix s, b;
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    pix_of(f, src, aa, &s);
    pix_of(f, base, aa, &b);
    if (!pix_box(&b, &x0, &y0, &x1, &y1))
        y0 = y1 = f->tm[1];
    if (below)
        pix_rows(&s, y1 + 1, s.y + s.h - 1, mark);
    else
        pix_rows(&s, s.y, y0 - 1, mark);
    pix_free(&s);
    pix_free(&b);
}

/* A breve in the box (x0, y0)-(x1, y1), drawn as the lower half of an
   ellipse with a stroke `stroke` pixels wide (coverage sampled 4x4). */
static void draw_breve(Pix *p, int x0, int y0, int x1, int y1, double stroke, int aa)
{
    double cx = (x0 + x1 + 1) / 2.0, rx = (x1 - x0 + 1 - stroke) / 2.0, ry = y1 - y0 + 1 - stroke / 2;
    int x, y;
    if (rx < 0.5) rx = 0.5;
    if (ry < 0.5) ry = 0.5;
    pix_alloc(p, x0, y0, x1 - x0 + 1, y1 - y0 + 1);
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++) {
            int sx, sy, n = 0;
            for (sy = 0; sy < 4; sy++)
                for (sx = 0; sx < 4; sx++) {
                    double px = x + (sx + 0.5) / 4 - cx, py = y + (sy + 0.5) / 4 - y0;
                    double r = sqrt(px * px / (rx * rx) + py * py / (ry * ry));
                    /* the distance from the curve, about: |r - 1| times the radius there */
                    double rad = r > 0 ? sqrt(px * px + py * py) / r : rx;
                    if (fabs(r - 1) * rad <= stroke / 2)
                        n++;
                }
            p->v[(y - y0) * p->w + x - x0] = (unsigned char)n;
        }
    if (!aa) {
        /* 1-bit: the darker pixel of each column (a thin stroke) or more than half covered */
        unsigned char *v = p->v;
        int w = p->w, h = p->h;
        unsigned char *o = calloc((size_t)w * h + 1, 1);
        for (x = 0; x < w; x++) {
            int best = -1;
            for (y = 0; y < h; y++) {
                if (v[y * w + x] >= 9)
                    o[y * w + x] = 16;
                if (best < 0 || v[y * w + x] > v[best * w + x])
                    best = y;
            }
            if (best >= 0 && v[best * w + x] >= 4)
                o[best * w + x] = 16;
        }
        free(p->v);
        p->v = o;
    }
}

/* the circumflex's stroke, in pixels: its ink over the length of its two strokes */
static double mark_stroke(const Pix *m, int x0, int y0, int x1, int y1)
{
    int i, sum = 0;
    double len = 2 * sqrt((x1 - x0 + 1) * (x1 - x0 + 1) / 4.0 + (y1 - y0 + 1) * (y1 - y0 + 1));
    for (i = 0; i < m->w * m->h; i++)
        sum += m->v[i];
    return len > 0 && sum / 16.0 / len > 1 ? sum / 16.0 / len : 1;
}

/* f's glyph of marks[k] (aa: antialiased) into *out; 0 if it cannot be made */
static int compose(FontFace *f, int k, int aa, Pix *out)
{
    int base = marks[k].base, mark = marks[k].mark & 15, cap = marks[k].mark & MARK_CAP;
    int src = cap ? 0xc2 : 0xe2, sbase = cap ? 'A' : 'a';     /* Â over A, â over a */
    int dx, mx0 = 0, my0 = 0, mx1 = 0, my1 = 0, have;
    Pix circ, m = {0, 0, 0, 0, NULL};
    if (aa && !f->aa)
        return 0;
    if (base)
        pix_of(f, base, aa, out);
    else
        pix_alloc(out, 0, 0, 0, 0);
    if (marks[k].mark & MARK_DOTLESS) {
        /* without the dot: the rows from the x-height down */
        Pix rows;
        pix_rows(out, ink_top(f, 'x', aa), out->y + out->h - 1, &rows);
        pix_free(out);
        *out = rows;
    }
    if (mark == MARK_NONE)
        return 1;
    /* the circumflex, moved as composite glyphs place it: by half the
       difference of the advances (ˆ alone: as it is) */
    if (base) {
        mark_of(f, src, sbase, 0, aa, &circ);
        dx = floor_half(f->g[base].dadv - f->g[sbase].dadv);
    } else {
        pix_of(f, 0x88, aa, &circ);
        dx = 0;
    }
    have = pix_box(&circ, &mx0, &my0, &mx1, &my1);
    if (!have) {
        /* (too small to have one) */
        mx0 = mx1 = f->g[base].dadv / 2 - dx;
        my0 = my1 = ink_top(f, base ? base : 'x', aa) - 2;
    }
    switch (mark) {
    case MARK_CIRCUMFLEX:
        pix_merge(out, &circ, dx, 0);
        break;
    case MARK_BREVE: {
        int y1 = my1;
        if (my1 - my0 < 1)
            my0 = my1 - 1;
        /* (as wide as the circumflex: at least 4 pixels, for a bowl) */
        if (mx1 - mx0 < 3) {
            mx0 -= (4 - (mx1 - mx0)) / 2;
            mx1 = mx0 + 3;
        }
        draw_breve(&m, mx0, my0, mx1, y1, have ? mark_stroke(&circ, mx0, my0, mx1, my1) : 1, aa);
        pix_merge(out, &m, dx, 0);
        break;
    }
    case MARK_DOT: {
        /* the dot of i, its bottom at the circumflex's, centred under it */
        Pix i;
        int ix0 = 0, iy0 = 0, ix1 = 0, iy1 = 0;
        pix_of(f, 'i', aa, &i);
        pix_rows(&i, i.y, ink_top(f, 'x', aa) - 1, &m);
        pix_free(&i);
        if (pix_box(&m, &ix0, &iy0, &ix1, &iy1))
            pix_merge(out, &m, dx + floor_half(mx0 + mx1 - ix0 - ix1), my1 - iy1);
        break;
    }
    case MARK_CEDILLA:
        mark_of(f, cap ? 0xc7 : 0xe7, cap ? 'C' : 'c', 1, aa, &m);
        pix_merge(out, &m, floor_half(f->g[base].dadv - f->g[cap ? 'C' : 'c'].dadv), 0);
        break;
    case MARK_BAR: {
        /* across the stems, between the top and the crossbar (Ħ) or the
           x-height (ħ), as thick as H's crossbar */
        Pix h;
        int x0 = 0, y0 = 0, x1 = 0, y1 = 0, y, x, t = 0, ycross = -1, w, bar_y, sx0, sx1;
        pix_of(f, 'H', aa, &h);
        if (pix_box(&h, &x0, &y0, &x1, &y1))
            for (y = y0 + 1; y < y1; y++) {
                int n = 0;
                for (x = x0; x <= x1; x++)
                    n += pix_at(&h, x, y) >= 8;
                if (n * 4 >= (x1 - x0 + 1) * 3) {
                    if (ycross < 0)
                        ycross = y;
                    t++;
                } else if (ycross >= 0)
                    break;
            }
        pix_free(&h);
        if (t < 1)
            t = 1;
        if (!pix_box(out, &x0, &y0, &x1, &y1))
            break;
        if (cap) {
            if (ycross < 0)
                ycross = (y0 + y1) / 2;
            bar_y = y0 + (ycross - y0 - t + 1) / 2;
            sx0 = x0 - 1;
            sx1 = x1 + 1;
        } else {
            /* (h: the stem is the ink of its top row) */
            int xh = ink_top(f, 'x', aa);
            bar_y = y0 + (xh - y0 - t + 1) / 2;
            sx0 = x1;
            sx1 = x0;
            for (x = x0; x <= x1; x++)
                if (pix_at(out, x, y0 + 1) >= 8) {
                    if (x < sx0) sx0 = x;
                    if (x > sx1) sx1 = x;
                }
            w = sx1 - sx0 + 1;
            sx0 -= w > 2 ? w / 2 + 1 : 2;
            sx1 += w > 2 ? w / 2 + 1 : 2;
        }
        pix_alloc(&m, sx0, bar_y, sx1 - sx0 + 1, t);
        memset(m.v, 16, (size_t)m.w * m.h);
        pix_merge(out, &m, 0, 0);
        break;
    }
    }
    pix_free(&circ);
    pix_free(&m);
    return 1;
}

/* -- other characters, from the font files (stb_truetype) -- */

typedef struct {
    const char *file;
    int tried;
    unsigned char *data;
    stbtt_fontinfo info;
} TtFont;

static TtFont tt_fonts[] = {{"fonts/NotoSerifTC-Regular.otf", 0, NULL, {0}}, {"fonts/NotoSansTC-Regular.otf", 0, NULL, {0}}};

static TtFont *tt_font(int serif)
{
    TtFont *t = &tt_fonts[serif ? 0 : 1];
    long n;
    if (!t->tried) {
        t->tried = 1;
        t->data = read_file(t->file, &n);
        if (t->data && !stbtt_InitFont(&t->info, t->data, stbtt_GetFontOffsetForIndex(t->data, 0))) {
            free(t->data);
            t->data = NULL;
        }
        if (!t->data)
            plat_log("text: no font %s/%s: characters beyond Windows-1252 drawn as boxes\n", plat_data_dir, t->file);
    }
    return t->data ? t : NULL;
}

/* f's glyph of cp from a font file into *out, its advance in *adv; 0 if none has it */
static int render(FontFace *f, unsigned cp, int aa, Pix *out, int *adv)
{
    TtFont *t = tt_font(f->face == 0 && f->height >= 20);
    int g, a, lsb, x0, y0, x1, y1, w, h, x, y, bold;
    float scale;
    unsigned char *cov;
    if ((!t || !(g = stbtt_FindGlyphIndex(&t->info, (int)cp))) && f->face == 0 && f->height >= 20)
        t = tt_font(0);         /* (the sans has what the serif lacks?) */
    if (!t || !(g = stbtt_FindGlyphIndex(&t->info, (int)cp)))
        return 0;
    scale = stbtt_ScaleForMappingEmToPixels(&t->info, (float)f->height);
    stbtt_GetGlyphHMetrics(&t->info, g, &a, &lsb);
    *adv = (int)(a * scale + 0.5f);
    bold = f->bold ? (f->height >= 32 ? 2 : 1) : 0;
    /* Placed where its strokes fall best on the pixels: of the shifts by
       quarters of a pixel (up to 32 pixels: larger glyphs are clear as
       they are), the one whose pixels are the least partly covered.  (No
       hinting: this is the little of it that helps Chinese, whose strokes
       are mostly level and upright.) */
    cov = NULL;
    {
        int steps = f->height <= 32 ? 4 : 1, sx, sy;
        long best = -1;
        for (sy = 0; sy < steps; sy++)
            for (sx = 0; sx < steps; sx++) {
                int bx0, by0, bx1, by1, i;
                long score = 0;
                unsigned char *c;
                stbtt_GetGlyphBitmapBoxSubpixel(&t->info, g, scale, scale, sx / 4.0f, sy / 4.0f, &bx0, &by0, &bx1,
                                                &by1);
                if (bx1 <= bx0 || by1 <= by0)
                    continue;
                c = calloc((size_t)(bx1 - bx0) * (by1 - by0), 1);
                stbtt_MakeGlyphBitmapSubpixel(&t->info, c, bx1 - bx0, by1 - by0, bx1 - bx0, scale, scale, sx / 4.0f,
                                              sy / 4.0f, g);
                for (i = 0; i < (bx1 - bx0) * (by1 - by0); i++)
                    score += c[i] * (255 - c[i]);
                if (best < 0 || score < best) {
                    best = score;
                    free(cov);
                    cov = c;
                    x0 = bx0; y0 = by0; x1 = bx1; y1 = by1;
                } else
                    free(c);
            }
    }
    if (!cov) {
        pix_alloc(out, 0, f->tm[1], 0, 0);
        return 1;
    }
    w = x1 - x0;
    h = y1 - y0;
    pix_alloc(out, x0, f->tm[1] + y0, w + bold, h);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            int v = cov[y * w + x], set;
            if (aa) {
                /* (contrast raised a little: unhinted, a stroke's ink is
                   spread over more pixels, paler, than the hinted glyphs'
                   beside it, which Wine's blend makes paler still) */
                set = v <= 26 ? 0 : ((v - 26) * 16 + 102) / 204;
                if (set > 16)
                    set = 16;
            } else {
                /* more than about half covered, or a thin stroke's darker
                   pixel across it (a local maximum, down or across) */
#define COV(xx, yy) ((xx) >= 0 && (yy) >= 0 && (xx) < w && (yy) < h ? cov[(yy) * w + (xx)] : 0)
                int up = COV(x, y - 1), down = COV(x, y + 1), left = COV(x - 1, y), right = COV(x + 1, y);
#undef COV
                set = v >= 144 || (v >= 80 && ((v >= up && v > down) || (v > up && v >= down) ||
                                                 (v >= left && v > right) || (v > left && v >= right)))
                          ? 16 : 0;
            }
            if (set)
                out->v[y * out->w + x] = (unsigned char)set;
        }
    free(cov);
    /* bold: 1-bit, each pixel spread right by `bold` pixels, but not into
       a one-pixel gap, which would close it; antialiased, the glyph over
       itself moved right by half as much (more than that blurs a small
       glyph's strokes together) */
    for (y = 0; y < h && bold; y++) {
        unsigned char *row = out->v + y * out->w, *orig = malloc((size_t)out->w);
        memcpy(orig, row, (size_t)out->w);
        for (x = 0; x < out->w; x++) {
            int k;
            if (aa) {
                for (k = 1; k <= bold; k++) {
                    /* moved by k/2: the mean of the two pixels it covers */
                    int a0 = x - k / 2 >= 0 ? orig[x - k / 2] : 0, a1 = x - (k + 1) / 2 >= 0 ? orig[x - (k + 1) / 2] : 0;
                    int v = (a0 + a1 + 1) / 2;
                    if (row[x] < v)
                        row[x] = (unsigned char)v;
                }
                continue;
            }
            if (!orig[x])
                continue;
            for (k = 1; k <= bold && x + k < out->w; k++) {
                if (x + k + 1 < out->w && orig[x + k + 1] && !orig[x + k])
                    break;
                if (row[x + k] < orig[x])
                    row[x + k] = orig[x];
            }
        }
        free(orig);
    }
    return 1;
}

/* f's tofu: a box of most of the ascent, half an em wide */
static void make_tofu(FontFace *f, Glyph *g, AaGlyph *a)
{
    int w = f->height / 2 > 3 ? f->height / 2 : 3, h = f->tm[1] * 3 / 4 > 4 ? f->tm[1] * 3 / 4 : 4, x, y;
    Pix p;
    pix_alloc(&p, 1, f->tm[1] - h, w, h);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            if (!x || !y || x == w - 1 || y == h - 1)
                p.v[y * w + x] = 16;
    if (g) {
        pix_to_glyph(&p, g);
        g->adv = g->dadv = (short)(w + 2);
    }
    if (a) {
        pix_to_aa(&p, a);
        a->adv = (short)(w + 2);
    }
    pix_free(&p);
}

/* f's glyph of character cp, beyond Windows-1252: made when first asked
   for, antialiased too if aa (f->aa loaded) */
static ExtraGlyph *extra_glyph(FontFace *f, unsigned cp, int aa)
{
    ExtraGlyph *e;
    unsigned bucket = cp % 64;
    int k, nm = (int)(sizeof marks / sizeof marks[0]);
    for (e = f->extra[bucket]; e && e->cp != cp; e = e->next)
        ;
    if (!e) {
        Pix p;
        int adv;
        e = calloc(1, sizeof *e);
        e->cp = cp;
        e->next = f->extra[bucket];
        f->extra[bucket] = e;
        for (k = 0; k < nm && marks[k].cp != cp; k++)
            ;
        if (k < nm && compose(f, k, 0, &p)) {
            int b = marks[k].base ? marks[k].base : 0x88;
            pix_to_glyph(&p, &e->g);
            e->g.adv = f->g[b].adv;
            e->g.dadv = f->g[b].dadv;
            pix_free(&p);
        } else if (render(f, cp, 0, &p, &adv)) {
            pix_to_glyph(&p, &e->g);
            e->g.adv = e->g.dadv = (short)adv;
            pix_free(&p);
        } else
            make_tofu(f, &e->g, NULL);
    }
    if (aa && !e->has_aa && f->aa) {
        Pix p;
        int adv;
        e->has_aa = 1;
        for (k = 0; k < nm && marks[k].cp != cp; k++)
            ;
        if (k < nm && compose(f, k, 1, &p)) {
            pix_to_aa(&p, &e->a);
            /* (the drawing advance where drawn without antialiasing, as captured) */
            e->a.adv = f->aa[marks[k].base ? marks[k].base : 0x88].adv;
            pix_free(&p);
        } else if (render(f, cp, !f->aa_mono, &p, &adv)) {
            /* (where the font's Latin letters are drawn without
               antialiasing, as Liberation's "gasp" says for small sizes:
               these too, as on 8-bit surfaces) */
            pix_to_aa(&p, &e->a);
            e->a.adv = (short)adv;
            pix_free(&p);
        } else
            make_tofu(f, NULL, &e->a);
    }
    return e;
}

/* The glyphs of the character at s[*i] (s[0..n)), *i moved past it: 1-bit
   (*g) and, if a is not NULL, antialiased (*a, aa_data loaded).  0 for a
   byte that is no character: nothing drawn, no width.  Windows-1252 text:
   the byte's glyphs, as ever. */
static int glyph_at(FontFace *f, const char *s, int n, int *i, const Glyph **g, AaGlyph **a)
{
    unsigned char ch = (unsigned char)s[*i];
    unsigned cp;
    int len;
    ExtraGlyph *e;
    if (ch < 0x80 || (!plat_text_utf8 && (!cp_table || cp_table[ch - 0x80] == codepage_1252[ch - 0x80]))) {
        (*i)++;
        *g = &f->g[ch];
        if (a)
            *a = &f->aa[ch];
        return 1;
    }
    if (plat_text_utf8) {
        cp = u8_decode(s + *i, n - *i, &len);
        *i += len;
        if (cp == U8_BAD)
            return 0;
    } else {
        (*i)++;
        cp = cp_table[ch - 0x80];
        if (!cp)                    /* (a hole of the code page: as Windows-1252's) */
            cp = codepage_1252[ch - 0x80] ? codepage_1252[ch - 0x80] : ch;
    }
    if (cp < 0x100 || (ch = codepage_from_unicode(codepage_1252, cp)) != 0) {
        ch = cp < 0x100 ? (unsigned char)cp : ch;
        if (cp >= 0x80 && cp < 0xa0)
            ch = 0;
        if (ch) {
            *g = &f->g[ch];
            if (a)
                *a = &f->aa[ch];
            return 1;
        }
    }
    e = extra_glyph(f, cp, a != NULL);
    *g = &e->g;
    if (a)
        *a = &e->a;
    return 1;
}

/* Wine's substitutes: Times New Roman -> Liberation Serif, Arial ->
 * Liberation Sans, Courier -> Liberation Mono (the DejaVu names: the faces
 * Wine substituted before, which the game's text can still ask for). */
static int face_index(const LOGFONTA *lf)
{
    const char *n = lf->lfFaceName;
    if (!strcasecmp(n, "Times New Roman") || !strncasecmp(n, "DejaVu Serif", 12) || !strcasecmp(n, "Times"))
        return 0;
    if (!strcasecmp(n, "Arial") || !strcasecmp(n, "Tahoma") || !strcasecmp(n, "MS Sans Serif") ||
        !strcasecmp(n, "MS Shell Dlg") ||
        !strcasecmp(n, "DejaVu Sans"))
        return 1;
    if (!strcasecmp(n, "System"))
        return 2;
    if (!strcasecmp(n, "Courier") || !strcasecmp(n, "Courier New") || !strncasecmp(n, "DejaVu Sans Mono", 16))
        return 3;
    switch (lf->lfPitchAndFamily & 0xf0) {
    case FF_ROMAN: return 0;
    case FF_MODERN: return 3;
    }
    return 1;
}

FontFace *font_face_for(const LOGFONTA *lf)
{
    int face, h, bold = lf->lfWeight >= 600, it = lf->lfItalic != 0;
    if (!font_data)
        load_fonts();
    face = face_index(lf);
    if (lf->lfHeight < 0) {
        h = -lf->lfHeight;
    } else if (lf->lfHeight == 0) {
        h = 16;
    } else {
        /* A cell height: the largest font whose cell fits. */
        int k;
        h = MINH;
        for (k = MINH; k <= MAXH; k++)
            if (faces[face][k][bold][it] && faces[face][k][bold][it]->tm[0] <= lf->lfHeight)
                h = k;
    }
    if (h < MINH) h = MINH;
    if (h > MAXH) h = MAXH;
    return faces[face][h][bold][it];
}

void text_metrics(FontFace *f, TEXTMETRICA *tm)
{
    memset(tm, 0, sizeof *tm);
    tm->tmHeight = f->tm[0];
    tm->tmAscent = f->tm[1];
    tm->tmDescent = f->tm[2];
    tm->tmInternalLeading = f->tm[3];
    tm->tmExternalLeading = f->tm[4];
    tm->tmAveCharWidth = f->tm[5];
    tm->tmMaxCharWidth = f->tm[6];
    tm->tmWeight = f->tm[7];
    tm->tmOverhang = f->tm[8];
    tm->tmFirstChar = (BYTE)f->tm[9];
    tm->tmLastChar = (BYTE)(f->tm[9] >> 8);
    tm->tmDefaultChar = (BYTE)(f->tm[9] >> 16);
    tm->tmBreakChar = (BYTE)(f->tm[9] >> 24);
    tm->tmPitchAndFamily = (BYTE)f->tm[10];
    tm->tmCharSet = (BYTE)(f->tm[10] >> 8);
    tm->tmItalic = (BYTE)(f->italic ? 255 : 0);
    tm->tmDigitizedAspectX = tm->tmDigitizedAspectY = 96;
}

/* (port) Text is measured as it is drawn (dadv): Wine measures a bold line
   a few pixels narrower than it draws it, so the game's word wrapping ran
   lines past their boxes (the heralds question).  CIV2_WINE_TEXT=1 keeps
   Wine's measures, for comparing with the original's recordings
   (tools/native-run.sh). */
static int wine_text(void)
{
    static int wine = -1;
    if (wine < 0)
        wine = getenv("CIV2_WINE_TEXT") != NULL;
    return wine;
}

int text_width(FontFace *f, const char *s, int n)
{
    int i, w = 0, wine = wine_text();
    if (plat_text_utf8 || cp_table) {
        const Glyph *g;
        for (i = 0; i < n;)
            if (glyph_at(f, s, n, &i, &g, NULL))
                w += wine ? g->adv : g->dadv;
        return w;
    }
    for (i = 0; i < n; i++)
        w += wine ? f->g[(unsigned char)s[i]].adv : f->g[(unsigned char)s[i]].dadv;
    return w;
}

int text_height(FontFace *f) { return f->tm[0]; }
int text_ave_width(FontFace *f) { return f->tm[5]; }

static FontFace *dc_face(DC *dc)
{
    Font *font = obj_get(dc->hfont, OBJ_FONT);
    return font ? font->face : NULL;
}

/* Glyphs placed at their measured advances (text_draw_measured), not their
   drawing ones: as Uniscribe places them (Wine's EDIT control).  Only with
   CIV2_WINE_TEXT=1, where text_width measures so too: in play text is
   measured as drawn, and a line drawn in runs (an edit's selection) must
   place its letters where one run would. */
static int measured_advances;

void text_draw_measured(DC *dc, int x, int y, const char *s, int n, const RECT *clip)
{
    measured_advances = wine_text();
    text_draw(dc, x, y, s, n, clip);
    measured_advances = 0;
}

static void text_draw_game(DC *dc, int x, int y, const char *s, int n, const RECT *clip);

/* The advance of a glyph (g, a: its 1-bit and antialiased forms) as
   text_draw_game places it on b. */
static int drawn_advance(const FontFace *f, const Bitmap *b, const Glyph *g, const AaGlyph *a)
{
    if (b->bpp > 8) {
        if (!wine_text())
            return g->dadv;
        return f->aa_mono && !measured_advances && f->aa ? a->adv : g->adv;
    }
    return measured_advances ? g->adv : g->dadv;
}

/* HD graphics (hd.c): the text again on the surface's plane, from the
   glyphs of the same font density times as tall (to 48 pixels), each where
   the game's own glyph is, so that the text keeps its layout. */
static void hd_text(DC *dc, Bitmap *b, const Region *rg, FontFace *f, int x, int y, const char *s, int n, COLORREF c)
{
    unsigned int *plane = hd_bitmap_plane(b);
    int d = hd_density, fh = f->height * d, i, k;
    FontFace *fine;
    double scale;
    if (fh > MAXH)
        fh = MAXH;
    fine = faces[f->face][fh][f->bold][f->italic];
    if (!aa_data)
        load_aa();
    if (!fine || !fine->aa)
        return;
    scale = (double)f->height * d / fine->height;
    if (dc->bk_mode == OPAQUE) {
        COLORREF bk = dc_resolve_color(dc, dc->bk_color);
        unsigned v = 0xff000000u | GetRValue(bk) << 16 | GetGValue(bk) << 8 | GetBValue(bk);
        RECT bg, a;
        bg.left = x; bg.top = y; bg.right = x + text_width(f, s, n); bg.bottom = y + f->tm[0];
        for (k = 0; k < rg->n; k++)
            if (rect_intersect(&a, &rg->r[k], &bg)) {
                int px, py;
                if (a.left < 0) a.left = 0;
                if (a.top < 0) a.top = 0;
                if (a.right > b->w) a.right = b->w;
                if (a.bottom > b->h) a.bottom = b->h;
                for (py = a.top * d; py < a.bottom * d; py++)
                    for (px = a.left * d; px < a.right * d; px++)
                        plane[(size_t)py * b->w * d + px] = v;
            }
    }
    for (i = 0; i < n;) {
        const Glyph *fg, *gg;
        AaGlyph *g, *ga;
        const unsigned char *cov;
        double xs;
        int at = i;
        if (!glyph_at(fine, s, n, &i, &fg, &g))
            continue;
        glyph_at(f, s, n, &at, &gg, f->aa ? &ga : NULL);
        cov = aa_coverage(g);
        /* not wider than the game's glyph: its advance, density times */
        xs = fg->adv ? (double)gg->dadv * d / fg->adv : scale;
        int gx = x * d + (int)(g->x * xs), gy = y * d + (int)(g->y * scale);
        int gw = (int)(g->w * xs + .5), gh = (int)(g->h * scale + .5);
        if (fg < fine->g || fg >= fine->g + 256) {
            /* (a glyph made for a character beyond Windows-1252
               (extra_glyph), at the fine font's size: scaled to the game
               glyph's advance alike both ways, on the baseline) */
            double base = y * d + fine->tm[1] * scale;
            gy = (int)(base + (g->y - fine->tm[1]) * xs);
            gh = (int)(g->h * xs + .5);
        }
        for (k = 0; cov && gw > 0 && gh > 0 && k < rg->n; k++) {
            int l = rg->r[k].left * d, t = rg->r[k].top * d, r = rg->r[k].right * d, bot = rg->r[k].bottom * d, px, py;
            if (l < gx) l = gx;
            if (t < gy) t = gy;
            if (r > gx + gw) r = gx + gw;
            if (bot > gy + gh) bot = gy + gh;
            if (l < 0) l = 0;
            if (t < 0) t = 0;
            if (r > b->w * d) r = b->w * d;
            if (bot > b->h * d) bot = b->h * d;
            for (py = t; py < bot; py++)
                for (px = l; px < r; px++) {
                    int v = cov[((py - gy) * g->h / gh) * g->w + (px - gx) * g->w / gw];
                    unsigned *p = plane + (size_t)py * b->w * d + px, z = *p;
                    if (v > 1)
                        *p = 0xff000000u | aa_blend((z >> 16) & 255, GetRValue(c), v) << 16 |
                             aa_blend((z >> 8) & 255, GetGValue(c), v) << 8 | aa_blend(z & 255, GetBValue(c), v);
                }
        }
        x += drawn_advance(f, b, gg, f->aa ? ga : g);
    }
}

/* Glyphs at logical (x, y) = top left of the text cell, clipped to `clip`
 * (logical) and the DC's clip region; with HD graphics on a surface with a
 * plane, there too (hd_text). */
void text_draw(DC *dc, int x, int y, const char *s, int n, const RECT *clip)
{
    FontFace *f = dc_face(dc);
    Bitmap *b;
    Region rg = {0, 0, NULL};
    int m, w;
    if (!hd_on || !f || n <= 0 || !(b = dc_target(dc)) || !hd_bitmap_want_plane(b)) {
        text_draw_game(dc, x, y, s, n, clip);
        return;
    }
    if (dc->is_memory) {
        RECT all = {0, 0, 0, 0};
        all.right = b->w;
        all.bottom = b->h;
        rgn_set(&rg, &all);
    } else
        rgn_copy(&rg, &dc->clip);
    if (clip) {
        RECT dclip = *clip;
        dclip.left += dc->origin.x; dclip.right += dc->origin.x;
        dclip.top += dc->origin.y; dclip.bottom += dc->origin.y;
        rgn_intersect_rect(&rg, &dclip);
    }
    /* the cell, with room for glyphs that reach out of it */
    m = f->height / 2 + 2;
    w = text_width(f, s, n);
    hd_bitmap_sync(b, x + dc->origin.x - m, y + dc->origin.y - m, w + 2 * m, f->tm[0] + 2 * m);
    text_draw_game(dc, x, y, s, n, clip);
    hd_text(dc, b, &rg, f, x + dc->origin.x, y + dc->origin.y, s, n, dc_resolve_color(dc, dc->text_color));
    hd_bitmap_commit(b, x + dc->origin.x - m, y + dc->origin.y - m, w + 2 * m, f->tm[0] + 2 * m);
    rgn_free(&rg);
}

static void text_draw_game(DC *dc, int x, int y, const char *s, int n, const RECT *clip)
{
    FontFace *f = dc_face(dc);
    Bitmap *b;
    Region rg = {0, 0, NULL};
    COLORREF c;
    int i, ox, oy;
    if (!f || n <= 0)
        return;
    b = dc_target(dc);
    if (dc->is_memory) {
        RECT all = {0, 0, 0, 0};
        all.right = b->w;
        all.bottom = b->h;
        rgn_set(&rg, &all);
    } else {
        rgn_copy(&rg, &dc->clip);
    }
    ox = dc->origin.x;
    oy = dc->origin.y;
    if (clip) {
        RECT dclip = *clip;
        dclip.left += ox; dclip.right += ox; dclip.top += oy; dclip.bottom += oy;
        rgn_intersect_rect(&rg, &dclip);
    }
    if (dc->bk_mode == OPAQUE) {
        RECT bg, d;
        int k;
        bg.left = x + ox; bg.top = y + oy;
        bg.right = bg.left + text_width(f, s, n);
        bg.bottom = bg.top + f->tm[0];
        for (k = 0; k < rg.n; k++)
            if (rect_intersect(&d, &rg.r[k], &bg)) {
                int px, py;
                COLORREF bk = dc_resolve_color(dc, dc->bk_color);
                for (py = d.top; py < d.bottom; py++)
                    for (px = d.left; px < d.right; px++)
                        surf_put(b, px, py, bk);
            }
    }
    c = dc_resolve_color(dc, dc->text_color);
    if (b->bpp > 8) {
        /* Antialiased, as Wine draws on deep surfaces (but for the sizes the
           font's "gasp" table says not to: those glyphs as Wine draws them,
           coverage 0 or 16; tools/probe/fontdump_aa.c). */
        unsigned tr = GetRValue(c), tg = GetGValue(c), tb = GetBValue(c);
        if (!aa_data)
            load_aa();
        for (i = 0; i < n && f->aa;) {
            const Glyph *g1;
            AaGlyph *g;
            const unsigned char *cov;
            int gx, gy, row, col, k;
            if (!glyph_at(f, s, n, &i, &g1, &g))
                continue;
            cov = aa_coverage(g);
            gx = x + ox + g->x;
            gy = y + oy + g->y;
            for (k = 0; cov && k < rg.n; k++) {
                RECT gr, a;
                gr.left = gx; gr.top = gy; gr.right = gx + g->w; gr.bottom = gy + g->h;
                if (!rect_intersect(&a, &gr, &rg.r[k]))
                    continue;
                if (a.left < 0) a.left = 0;
                if (a.top < 0) a.top = 0;
                if (a.right > b->w) a.right = b->w;
                if (a.bottom > b->h) a.bottom = b->h;
                for (row = a.top; row < a.bottom; row++)
                    for (col = a.left; col < a.right; col++) {
                        int v = cov[(row - gy) * g->w + (col - gx)];
                        COLORREF d;
                        if (v <= 1)
                            continue;
                        d = surf_get(b, col, row);
                        surf_put(b, col, row, RGB(aa_blend(GetRValue(d), tr, v), aa_blend(GetGValue(d), tg, v),
                                                  aa_blend(GetBValue(d), tb, v)));
                    }
            }
            /* Wine: antialiased glyphs advance as measured, the others as
               drawn; in play every glyph as text_width counts it */
            if (!wine_text())
                x += g1->dadv;
            else
                x += f->aa_mono && !measured_advances ? g->adv : g1->adv;
        }
        rgn_free(&rg);
        return;
    }
    {
        int idx = b->bpp == 8 ? surf_nearest(b, c) : -1;
        DWORD v32 = (DWORD)(GetRValue(c) << 16 | GetGValue(c) << 8 | GetBValue(c));
        for (i = 0; i < n;) {
            const Glyph *g;
            int gx, gy, row, col, stride, k;
            if (!glyph_at(f, s, n, &i, &g, NULL))
                continue;
            gx = x + ox + g->x;
            gy = y + oy + g->y;
            stride = (g->w + 7) / 8;
            for (k = 0; k < rg.n; k++) {
                RECT gr, a;
                gr.left = gx; gr.top = gy; gr.right = gx + g->w; gr.bottom = gy + g->h;
                if (!rect_intersect(&a, &gr, &rg.r[k]))
                    continue;
                if (a.left < 0) a.left = 0;
                if (a.top < 0) a.top = 0;
                if (a.right > b->w) a.right = b->w;
                if (a.bottom > b->h) a.bottom = b->h;
                for (row = a.top; row < a.bottom; row++) {
                    const unsigned char *gb = g->bits + (row - gy) * stride;
                    unsigned char *p = bmp_row(b, row);
                    for (col = a.left; col < a.right; col++) {
                        int bx = col - gx;
                        if (!(gb[bx >> 3] & (0x80 >> (bx & 7))))
                            continue;
                        if (idx >= 0)
                            p[col] = (unsigned char)idx;
                        else if (b->bpp == 32)
                            ((DWORD *)p)[col] = v32;
                        else
                            surf_put(b, col, row, c);
                    }
                }
            }
            x += measured_advances ? g->adv : g->dadv;
        }
    }
    rgn_free(&rg);
}

/* An underline (for & prefixes) under characters [from, from+len) of s. */
static void underline(DC *dc, FontFace *f, int x, int y, const char *s, int from, int len, const RECT *clip)
{
    RECT r;
    Pen pen;
    (void)pen;
    r.left = x + text_width(f, s, from);
    r.right = r.left + text_width(f, s + from, len) - 1;     /* Wine stops one pixel short */
    r.top = y + f->tm[1] + 1;
    r.bottom = r.top + 1;
    if (clip) {
        RECT a;
        if (!rect_intersect(&a, &r, clip))
            return;
        r = a;
    }
    dc_fill_rect(dc, &r, dc->text_color);
}

/* ---- GDI entry points ---- */

HFONT WINAPI CreateFontIndirectA(const LOGFONTA *lf)
{ API_TRACE();
    Font *f = calloc(1, sizeof *f);
    f->lf = *lf;
    f->face = font_face_for(lf);
    plat_log("CreateFont(%s, %d, weight %d, italic %d) -> face %d height %d\n", lf->lfFaceName, (int)lf->lfHeight,
             (int)lf->lfWeight, lf->lfItalic, f->face ? f->face->face : -1, f->face ? f->face->height : -1);
    return obj_new(OBJ_FONT, f);
}

WINBOOL WINAPI GetTextMetricsA(HDC h, LPTEXTMETRICA tm)
{ API_TRACE();
    DC *dc = dc_get(h);
    FontFace *f = dc ? dc_face(dc) : NULL;
    if (!f)
        return FALSE;
    text_metrics(f, tm);
    return TRUE;
}

WINBOOL WINAPI GetTextExtentPointA(HDC h, LPCSTR s, int n, LPSIZE sz)
{ API_TRACE();
    DC *dc = dc_get(h);
    FontFace *f = dc ? dc_face(dc) : NULL;
    if (!f)
        return FALSE;
    sz->cx = text_width(f, s, n);
    sz->cy = f->tm[0];
    if (getenv("CIV2_LOG_TEXT"))
        plat_log("extent \"%.*s\" -> %ld x %ld\n", n, s, (long)sz->cx, (long)sz->cy);
    return TRUE;
}

/* Strip & prefixes; *ul = index of the underlined character or -1. */
static int strip_prefix(const char *s, int n, char *out, int *ul)
{
    int i, o = 0;
    *ul = -1;
    for (i = 0; i < n; i++) {
        if (s[i] == '&' && i + 1 < n) {
            i++;
            if (s[i] != '&')
                *ul = o;
        }
        out[o++] = s[i];
    }
    return o;
}

/* TA_UPDATECP: the text goes at the current position, whatever the rect
 * and the alignment flags say (still clipped to the rect), which then
 * moves past it (tools/probe/cpprobe.c). */
static void at_current_position(DC *dc, int *x, int *y, int w)
{
    if (!(dc->text_align & TA_UPDATECP))
        return;
    *x = dc->pos.x;
    *y = dc->pos.y;
    dc->pos.x += w;
}

int WINAPI DrawTextA(HDC h, LPCSTR str, int len, LPRECT rect, UINT fmt)
{ API_TRACE();
    DC *dc = dc_get(h);
    FontFace *f = dc ? dc_face(dc) : NULL;
    int lh, y, maxw = 0, width;
    const char *p, *end;
    char line[1024];
    if (!f || !str)
        return 0;
    if (len < 0)
        len = (int)strlen(str);
    if (getenv("CIV2_LOG_TEXT"))
        plat_log("DrawText(\"%.*s\", face %d h %d bold %d, fmt %x, rect %d,%d,%d,%d)\n", len, str, f->face, f->height,
                 f->bold, fmt, (int)rect->left, (int)rect->top, (int)rect->right, (int)rect->bottom);
    lh = f->tm[0] + ((fmt & DT_EXTERNALLEADING) ? f->tm[4] : 0);
    width = rect->right - rect->left;
    y = rect->top;
    p = str;
    end = str + len;
    if ((fmt & DT_SINGLELINE) && (fmt & (DT_VCENTER | DT_BOTTOM))) {
        if (fmt & DT_VCENTER)
            y = rect->top + (rect->bottom - rect->top) / 2 - lh / 2;     /* as Wine computes it */
        else
            y = rect->bottom - lh;
    }
    do {
        int n = 0, ul = -1, w, x;
        const char *q = p;
        /* One line: up to a newline, or word-wrapped to the width. */
        if (fmt & DT_SINGLELINE) {
            q = end;
        } else {
            while (q < end && *q != '\n' && *q != '\r')
                q++;
        }
        {
            char raw[1024];
            int rn = (int)(q - p) < 1023 ? (int)(q - p) : 1023, i, o = 0;
            for (i = 0; i < rn; i++) {
                if (p[i] == '\t' && (fmt & DT_EXPANDTABS)) {
                    int tabw = 8 * f->tm[5], cur = text_width(f, raw, o);
                    while (o < 1000 && text_width(f, raw, o) < (cur / tabw + 1) * tabw)
                        raw[o++] = ' ';
                    continue;
                }
                raw[o++] = p[i];
            }
            if (fmt & DT_NOPREFIX) {
                memcpy(line, raw, (size_t)o);
                n = o;
            } else {
                n = strip_prefix(raw, o, line, &ul);
            }
        }
        if ((fmt & DT_WORDBREAK) && !(fmt & DT_SINGLELINE) && text_width(f, line, n) > width) {
            /* Break after the last space that fits (or mid-word if none). */
            int fit = 0, last_space = -1, i, no_space = 0;
            if (plat_text_utf8) {
                /* (UTF-8 text: by character, and also next to a wide one:
                   no_space, a break with no space to skip) */
                unsigned prev = U8_BAD;
                for (i = 0; i < n;) {
                    int len;
                    unsigned c = u8_decode(line + i, n - i, &len);
                    int brk_here = i > 0 && line[i] != ' ' && u8_break_between(prev, c);
                    if (text_width(f, line, i + len) > width) {
                        if (brk_here) {
                            last_space = i;
                            no_space = 1;
                        }
                        break;
                    }
                    if (line[i] == ' ') {
                        last_space = i;
                        no_space = 0;
                    } else if (brk_here) {
                        last_space = i;
                        no_space = 1;
                    }
                    prev = c;
                    i += len;
                    fit = i;
                }
            } else
            for (i = 0; i < n; i++) {
                if (text_width(f, line, i + 1) > width)
                    break;
                if (line[i] == ' ')
                    last_space = i;
                fit = i + 1;
            }
            if (i < n) {
                int brk = last_space >= 0 ? last_space : (fit ? fit : u8_len_at(line, n, 0));
                int consumed = brk, skip = last_space >= 0 && !no_space ? 1 : 0;
                /* Map back to the source: count source chars giving `consumed` output chars. */
                const char *s = p;
                int o = 0;
                while (s < end && o < consumed) {
                    if (!(fmt & DT_NOPREFIX) && *s == '&' && s + 1 < end)
                        s++;
                    s++;
                    o++;
                }
                n = consumed;
                q = s;
                if (skip)
                    while (q < end && *q == ' ')
                        q++;
                if (ul >= n)
                    ul = -1;
                /* q is the start of the next line: no newline to skip. */
                w = text_width(f, line, n);
                if (w > maxw) maxw = w;
                if (!(fmt & DT_CALCRECT)) {
                    int ty = y;
                    x = rect->left;
                    if (fmt & DT_CENTER) x = (rect->left + rect->right - w) / 2;
                    else if (fmt & DT_RIGHT) x = rect->right - w;
                    at_current_position(dc, &x, &ty, w);
                    text_draw(dc, x, ty, line, n, (fmt & DT_NOCLIP) ? NULL : rect);
                    if (ul >= 0)
                        underline(dc, f, x, ty, line, ul, u8_len_at(line, n, ul), (fmt & DT_NOCLIP) ? NULL : rect);
                }
                y += lh;
                p = q;
                continue;
            }
        }
        w = text_width(f, line, n);
        if (w > maxw)
            maxw = w;
        if (!(fmt & DT_CALCRECT)) {
            int ty = y;
            x = rect->left;
            if (fmt & DT_CENTER)
                x = (rect->left + rect->right - w) / 2;
            else if (fmt & DT_RIGHT)
                x = rect->right - w;
            at_current_position(dc, &x, &ty, w);
            text_draw(dc, x, ty, line, n, (fmt & DT_NOCLIP) ? NULL : rect);
            if (ul >= 0)
                underline(dc, f, x, ty, line, ul, u8_len_at(line, n, ul), (fmt & DT_NOCLIP) ? NULL : rect);
        }
        y += lh;
        p = q;
        if (p < end && *p == '\r') p++;
        if (p < end && *p == '\n') p++;
    } while (p < end && !(fmt & DT_SINGLELINE));
    if (fmt & DT_CALCRECT) {
        rect->right = rect->left + maxw;
        rect->bottom = y;
    }
    return y - rect->top;
}

int WINAPI EnumFontFamiliesA(HDC h, LPCSTR family, FONTENUMPROCA proc, LPARAM lp)
{ API_TRACE();
    static const char *names[] = {"DejaVu Sans", "DejaVu Sans Mono", "DejaVu Serif"};
    int i, r = 1;
    (void)h;
    for (i = 0; i < 3 && r; i++) {
        LOGFONTA lf;
        TEXTMETRICA tm;
        if (family && strcasecmp(family, names[i]))
            continue;
        memset(&lf, 0, sizeof lf);
        strcpy(lf.lfFaceName, names[i]);
        lf.lfHeight = -16;
        lf.lfWeight = 400;
        text_metrics(font_face_for(&lf), &tm);
        r = proc(&lf, &tm, TRUETYPE_FONTTYPE, lp);
    }
    return r;
}

int WINAPI AddFontResourceA(LPCSTR file)
{ API_TRACE();
    char host[1024];
    FILE *f;
    plat_host_path(file, host, sizeof host, 0);
    plat_log("AddFontResource(%s)\n", file);
    f = fopen(host, "rb");
    if (!f)
        return 0;
    fclose(f);
    plat_log("AddFontResource(%s): font files are not supported\n", file);
    return 0;
}

WINBOOL WINAPI RemoveFontResourceA(LPCSTR file)
{ API_TRACE();
    (void)file;
    return FALSE;
}
