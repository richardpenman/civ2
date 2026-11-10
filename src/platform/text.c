/*
 * Fonts and text, from the glyphs the reference environment draws: Times
 * New Roman, Arial, System and Courier at 6..48 pixels, regular/bold/italic.
 * Wine draws 1-bit glyphs on surfaces of 8 bits or less
 * (data/native/fonts_win.bin, tools/probe/fontdump.c) and antialiased ones on
 * deeper surfaces (data/native/fonts_win_aa.bin, tools/probe/fontdump_aa.c),
 * blended as its DIB engine does (tools/probe/aaprobe.c checks the model).
 */
#include "plat.h"

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

struct FontFace {
    int face, height, bold, italic;
    int tm[11];
    Glyph g[256];
    AaGlyph *aa;                    /* 256 entries, loaded on demand */
    int aa_mono;                    /* drawn without antialiasing (gasp): advances as drawn */
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
int text_width(FontFace *f, const char *s, int n)
{
    static int wine = -1;
    int i, w = 0;
    if (wine < 0)
        wine = getenv("CIV2_WINE_TEXT") != NULL;
    for (i = 0; i < n; i++)
        w += wine ? f->g[(unsigned char)s[i]].adv : f->g[(unsigned char)s[i]].dadv;
    return w;
}

int text_height(FontFace *f) { return f->tm[0]; }

static FontFace *dc_face(DC *dc)
{
    Font *font = obj_get(dc->hfont, OBJ_FONT);
    return font ? font->face : NULL;
}

/* Glyphs placed at their measured advances (text_draw_measured), not their
   drawing ones: as Uniscribe places them (Wine's EDIT control). */
static int measured_advances;

void text_draw_measured(DC *dc, int x, int y, const char *s, int n, const RECT *clip)
{
    measured_advances = 1;
    text_draw(dc, x, y, s, n, clip);
    measured_advances = 0;
}

/* Glyphs at logical (x, y) = top left of the text cell, clipped to `clip`
 * (logical) and the DC's clip region. */
void text_draw(DC *dc, int x, int y, const char *s, int n, const RECT *clip)
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
        for (i = 0; i < n && f->aa; i++) {
            AaGlyph *g = &f->aa[(unsigned char)s[i]];
            const unsigned char *cov = aa_coverage(g);
            int gx = x + ox + g->x, gy = y + oy + g->y, row, col, k;
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
            /* antialiased glyphs advance as measured, the others as drawn */
            x += f->aa_mono && !measured_advances ? g->adv : f->g[(unsigned char)s[i]].adv;
        }
        rgn_free(&rg);
        return;
    }
    {
        int idx = b->bpp == 8 ? surf_nearest(b, c) : -1;
        DWORD v32 = (DWORD)(GetRValue(c) << 16 | GetGValue(c) << 8 | GetBValue(c));
        for (i = 0; i < n; i++) {
            const Glyph *g = &f->g[(unsigned char)s[i]];
            int gx = x + ox + g->x, gy = y + oy + g->y, row, col, stride = (g->w + 7) / 8, k;
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
            int fit = 0, last_space = -1, i;
            for (i = 0; i < n; i++) {
                if (text_width(f, line, i + 1) > width)
                    break;
                if (line[i] == ' ')
                    last_space = i;
                fit = i + 1;
            }
            if (i < n) {
                int brk = last_space >= 0 ? last_space : (fit ? fit : 1);
                int consumed = brk, skip = last_space >= 0 ? 1 : 0;
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
                        underline(dc, f, x, ty, line, ul, 1, (fmt & DT_NOCLIP) ? NULL : rect);
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
                underline(dc, f, x, ty, line, ul, 1, (fmt & DT_NOCLIP) ? NULL : rect);
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
