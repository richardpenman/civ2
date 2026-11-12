/*
 * GDI: device contexts, bitmaps and DIB sections, palettes, brushes, pens,
 * blits, rectangles, lines, pixels.
 *
 * Window DCs draw on the desktop surface (32 bpp, like the reference X
 * display), clipped to the window's visible client area.  Colours given to
 * 8-bpp surfaces are matched to the nearest colour of their colour table,
 * as Wine's DIB engine does.
 */
#include "plat.h"

Bitmap *screen;                 /* the composed desktop: what is shown */
Bitmap *root;                   /* the desktop background */
int screen_w = 1024, screen_h = 768;
int plat_overlay;               /* a popup menu is drawn on the screen itself */
extern unsigned wnd_generation;

/* ---- bitmaps ---- */

unsigned char *bmp_row(const Bitmap *b, int y)
{
    return b->bits + (size_t)(b->bottom_up ? b->h - 1 - y : y) * (size_t)b->stride;
}

Bitmap *bmp_create(int w, int h, int bpp, int bottom_up)
{
    Bitmap *b = calloc(1, sizeof *b);
    b->w = w < 1 ? 1 : w;
    b->h = h < 1 ? 1 : h;
    b->bpp = bpp;
    b->stride = ((b->w * bpp + 31) / 32) * 4;
    b->bottom_up = bottom_up;
    /* Windows backs a DIB section with whole pages; the game's GIF decoder
       relies on that and writes one byte past the last row. */
    b->bits = calloc((size_t)b->stride * (size_t)b->h + 4, 1);
    b->owns_bits = 1;
    if (bpp == 1) {
        b->ncolors = 2;
        b->colors[1].rgbRed = b->colors[1].rgbGreen = b->colors[1].rgbBlue = 255;
    }
    return b;
}

void bmp_destroy(Bitmap *b)
{
    if (!b)
        return;
    if (b->owns_bits)
        free(b->bits);
    free(b);
}

/* ---- colours ---- */

static int nearest_in(const RGBQUAD *t, int n, COLORREF c)
{
    int i, best = 0;
    long bd = 1L << 30;
    int r = GetRValue(c), g = GetGValue(c), bl = GetBValue(c);
    for (i = 0; i < n; i++) {
        long d = (long)(t[i].rgbRed - r) * (t[i].rgbRed - r) + (long)(t[i].rgbGreen - g) * (t[i].rgbGreen - g) +
                 (long)(t[i].rgbBlue - bl) * (t[i].rgbBlue - bl);
        if (d < bd) {
            bd = d;
            best = i;
            if (!d)
                break;
        }
    }
    return best;
}

int surf_nearest(const Bitmap *b, COLORREF c)
{
    return nearest_in(b->colors, b->ncolors ? b->ncolors : 256, c & 0xffffff);
}

void surf_put(Bitmap *b, int x, int y, COLORREF c)
{
    unsigned char *p;
    if (x < 0 || y < 0 || x >= b->w || y >= b->h)
        return;
    p = bmp_row(b, y);
    switch (b->bpp) {
    case 32:
        ((DWORD *)p)[x] = (DWORD)(GetRValue(c) << 16 | GetGValue(c) << 8 | GetBValue(c));
        break;
    case 24:
        p[x * 3] = GetBValue(c); p[x * 3 + 1] = GetGValue(c); p[x * 3 + 2] = GetRValue(c);
        break;
    case 16:
        ((WORD *)p)[x] = (WORD)((GetRValue(c) >> 3) << 10 | (GetGValue(c) >> 3) << 5 | (GetBValue(c) >> 3));
        break;
    case 8:
        p[x] = (unsigned char)surf_nearest(b, c);
        break;
    case 1:
        if (surf_nearest(b, c))
            p[x >> 3] |= (unsigned char)(0x80 >> (x & 7));
        else
            p[x >> 3] &= (unsigned char)~(0x80 >> (x & 7));
        break;
    }
}

COLORREF surf_get(Bitmap *b, int x, int y)
{
    unsigned char *p;
    if (x < 0 || y < 0 || x >= b->w || y >= b->h)
        return CLR_INVALID;
    p = bmp_row(b, y);
    switch (b->bpp) {
    case 32: { DWORD v = ((DWORD *)p)[x]; return RGB(v >> 16 & 0xff, v >> 8 & 0xff, v & 0xff); }
    case 24: return RGB(p[x * 3 + 2], p[x * 3 + 1], p[x * 3]);
    case 16: { WORD v = ((WORD *)p)[x]; return RGB((v >> 10 & 31) << 3, (v >> 5 & 31) << 3, (v & 31) << 3); }
    case 8: { RGBQUAD q = b->colors[p[x]]; return RGB(q.rgbRed, q.rgbGreen, q.rgbBlue); }
    case 1: { RGBQUAD q = b->colors[(p[x >> 3] >> (7 - (x & 7))) & 1]; return RGB(q.rgbRed, q.rgbGreen, q.rgbBlue); }
    }
    return 0;
}

/* ---- device contexts ---- */

static HBRUSH stock_brush[6];
static HPEN stock_pen[3];
static HFONT stock_font[18];
static HBITMAP stock_bitmap;
static HPALETTE stock_palette;
static HBRUSH sys_brush[31];
static const COLORREF sys_colors[31] = {
    0xffffff, 0x956f25, 0xfa9632, 0x808080, 0xffffff, 0xffffff, 0x9e9e9e, 0x000000, 0x000000, 0x000000,
    0xffffff, 0xffffff, 0x808080, 0xfa9630, 0xffffff, 0xf5f5f5, 0xa6a6a6, 0xa6a6a6, 0x000000, 0xc8c8c8,
    0xffffff, 0x6a6a6a, 0xe3e3e3, 0x000000, 0xffffff, 0xffffff, 0xe0e0e0, 0xfa9632, 0x808080, 0xfa9630,
    0xffffff
};

DWORD WINAPI GetSysColor(int i) { API_TRACE(); return i >= 0 && i < 31 ? sys_colors[i] : 0; }
HBRUSH WINAPI GetSysColorBrush(int i) { API_TRACE(); return i >= 0 && i < 31 ? sys_brush[i] : NULL; }

static HBRUSH new_brush(int style, COLORREF c)
{
    API_TRACE();
    Brush *b = calloc(1, sizeof *b);
    b->style = style;
    b->color = c;
    return obj_new(OBJ_BRUSH, b);
}

static HPEN new_pen(int style, int width, COLORREF c)
{
    Pen *p = calloc(1, sizeof *p);
    p->style = style;
    p->width = width;
    p->color = c;
    return obj_new(OBJ_PEN, p);
}

void gdi_init(void)
{
    static const COLORREF brush_colors[5] = {0xffffff, 0xc0c0c0, 0x808080, 0x404040, 0x000000};
    LOGFONTA lf;
    Palette *pal;
    int i;
    screen = bmp_create(screen_w, screen_h, 32, 0);
    root = bmp_create(screen_w, screen_h, 32, 0);
    for (i = 0; i < 5; i++)
        obj_set_stock(stock_brush[i] = new_brush(BS_SOLID, brush_colors[i]));
    obj_set_stock(stock_brush[5] = new_brush(BS_NULL, 0));
    obj_set_stock(stock_pen[0] = new_pen(PS_SOLID, 1, 0xffffff));
    obj_set_stock(stock_pen[1] = new_pen(PS_SOLID, 1, 0));
    obj_set_stock(stock_pen[2] = new_pen(PS_NULL, 1, 0));
    for (i = 0; i < 31; i++)
        obj_set_stock(sys_brush[i] = new_brush(BS_SOLID, sys_colors[i]));
    memset(&lf, 0, sizeof lf);
    lf.lfHeight = 16;           /* Wine's: a 16-pixel cell */
    lf.lfWeight = 700;
    strcpy(lf.lfFaceName, "System");
    for (i = 0; i < 18; i++) {
        stock_font[i] = CreateFontIndirectA(&lf);
        obj_set_stock(stock_font[i]);
    }
    stock_bitmap = CreateBitmap(1, 1, 1, 1, NULL);
    obj_set_stock(stock_bitmap);
    pal = calloc(1, sizeof *pal);
    /* The default palette: the 20 static colours. */
    {
        static const DWORD sys20[20] = {
            0x000000, 0x000080, 0x008000, 0x008080, 0x800000, 0x800080, 0x808000, 0xc0c0c0, 0xc0dcc0, 0xf0caa6,
            0xfffbf0, 0xa0a0a4, 0x808080, 0x0000ff, 0x00ff00, 0x00ffff, 0xff0000, 0xff00ff, 0xffff00, 0xffffff};
        pal->n = 20;
        for (i = 0; i < 20; i++) {
            pal->e[i].peRed = (BYTE)(sys20[i] >> 16);
            pal->e[i].peGreen = (BYTE)(sys20[i] >> 8);
            pal->e[i].peBlue = (BYTE)sys20[i];
        }
    }
    stock_palette = obj_new(OBJ_PALETTE, pal);
    obj_set_stock(stock_palette);
}

HGDIOBJ WINAPI GetStockObject(int i)
{
    API_TRACE();
    switch (i) {
    case WHITE_BRUSH: case LTGRAY_BRUSH: case GRAY_BRUSH: case DKGRAY_BRUSH: case BLACK_BRUSH: case NULL_BRUSH:
        return stock_brush[i];
    case WHITE_PEN: return stock_pen[0];
    case BLACK_PEN: return stock_pen[1];
    case NULL_PEN: return stock_pen[2];
    case DEFAULT_PALETTE: return stock_palette;
    }
    if (i >= OEM_FIXED_FONT && i <= DEFAULT_GUI_FONT)
        return stock_font[i];
    return NULL;
}

static HDC new_dc(DC **out)
{
    DC *dc = calloc(1, sizeof *dc);
    dc->hbrush = stock_brush[WHITE_BRUSH];
    dc->hpen = stock_pen[1];
    dc->hfont = stock_font[SYSTEM_FONT];
    dc->hpalette = stock_palette;
    dc->text_color = 0;
    dc->bk_color = 0xffffff;
    dc->bk_mode = OPAQUE;
    *out = dc;
    return obj_new(OBJ_DC, dc);
}

static DC *get_dc(HDC h) { return obj_get(h, OBJ_DC); }

/* Window DCs: recompute origin and clipping when windows changed. */
/* Window DCs draw into their top-level window's surface (wnd_surface);
 * coordinates are relative to that window's top left.  whole: GetWindowDC
 * (origin at the window's corner, the non-client area included). */
typedef struct { DC dc; unsigned gen; int whole, overlay; } WindowDC;

void dc_update_window(DC *dc)
{
    WindowDC *wd = (WindowDC *)dc;
    Window *top;
    if (dc->is_memory || !dc->wnd || dc->paint)
        return;
    if (wd->gen == wnd_generation)
        return;
    wd->gen = wnd_generation;
    if (dc->wnd == desktop) {
        RECT all = {0, 0, 0, 0};
        all.right = screen_w; all.bottom = screen_h;
        dc->origin.x = dc->origin.y = 0;
        rgn_set(&dc->clip, &all);
        return;
    }
    top = wnd_top(dc->wnd);
    if (wd->whole) {
        RECT r;
        wnd_screen_rect(dc->wnd, &r);
        dc->origin.x = r.left;
        dc->origin.y = r.top;
    } else {
        wnd_client_screen(dc->wnd, &dc->origin);
    }
    wnd_visible_region(dc->wnd, !wd->whole, &dc->clip);
    dc->origin.x -= top->rect.left;
    dc->origin.y -= top->rect.top;
    rgn_offset(&dc->clip, -top->rect.left, -top->rect.top);
}

unsigned plat_draw_gen;

Bitmap *dc_target(DC *dc)
{
    if (dc->is_memory)
        return dc->bitmap;
    plat_draw_gen++;
    if (((WindowDC *)dc)->overlay)
        return screen;
    dc_update_window(dc);
    if (dc->wnd == desktop)
        return root;
    return wnd_surface(wnd_top(dc->wnd));
}

/* The desktop: the background, then the top-level windows from the bottom
 * up, each where it shows (see wnd_shown_rect). */
int plat_color_lut_on;
unsigned char plat_color_lut[256];

void plat_compose(void)
{
    Window *order[512], *w;
    int n = 0, i, y;
    memcpy(screen->bits, root->bits, (size_t)screen->stride * (size_t)screen->h);
    for (w = desktop->children; w && n < 512; w = w->next)
        order[n++] = w;
    for (i = n - 1; i >= 0; i--) {
        RECT shown, r;
        Bitmap *s;
        int dx, dy;
        w = order[i];
        if (!(w->style & WS_VISIBLE) || w->minimized)
            continue;
        wnd_shown_rect(w, &shown);
        if (!rect_intersect(&r, &shown, &desktop->rect))
            continue;
        s = wnd_surface(w);
        /* Surface pixel (x, y) shows at screen (rect.left + x + host_dx, ...). */
        dx = w->rect.left + (w->host_clipped ? w->host_dx : 0);
        dy = w->rect.top + (w->host_clipped ? w->host_dy : 0);
        for (y = r.top; y < r.bottom; y++) {
            int sy = y - dy, x0 = r.left, x1 = r.right;
            if (sy < 0 || sy >= s->h)
                continue;
            if (x0 - dx < 0) x0 = dx;
            if (x1 - dx > s->w) x1 = dx + s->w;
            if (x1 > x0)
                memcpy((DWORD *)bmp_row(screen, y) + x0, (DWORD *)bmp_row(s, sy) + (x0 - dx), (size_t)(x1 - x0) * 4);
        }
    }
    if (plat_color_lut_on) {
        /* X35 (docs/EXTRAS.md): the colour correction, on what is shown */
        for (y = 0; y < screen->h; y++) {
            DWORD *p = (DWORD *)bmp_row(screen, y);
            int x;
            for (x = 0; x < screen->w; x++) {
                DWORD c = p[x];
                p[x] = (c & 0xff000000u) | ((DWORD)plat_color_lut[(c >> 16) & 0xff] << 16) |
                       ((DWORD)plat_color_lut[(c >> 8) & 0xff] << 8) | plat_color_lut[c & 0xff];
            }
        }
    }
}

/* A DC on the composed screen itself (interactive popup menus). */
HDC plat_overlay_dc(void)
{
    HDC h = GetDC(0);
    WindowDC *wd = (WindowDC *)dc_get(h);
    wd->overlay = 1;
    return h;
}

static HDC new_window_dc(HWND h)
{
    WindowDC *wd = calloc(1, sizeof *wd);
    DC *dc = &wd->dc;
    Window *w = h ? wnd_get(h) : desktop;
    HDC hdc;
    if (!w) {
        free(wd);
        return NULL;
    }
    dc->hbrush = stock_brush[WHITE_BRUSH];
    dc->hpen = stock_pen[1];
    dc->hfont = stock_font[SYSTEM_FONT];
    dc->hpalette = stock_palette;
    dc->bk_color = 0xffffff;
    dc->bk_mode = OPAQUE;
    dc->wnd = w;
    dc->is_screen = !h;
    hdc = obj_new(OBJ_DC, dc);
    dc_update_window(dc);
    return hdc;
}

/* A window of a CS_OWNDC class has one DC of its own, made with the window:
 * GetDC and BeginPaint return it, its attributes kept (Wine's owned DCE). */
static int has_own_dc(const Window *w)
{
    return w && w != desktop && w->cls && (w->cls->wc.style & CS_OWNDC);
}

HDC WINAPI GetDC(HWND h)
{
    API_TRACE();
    Window *w = h ? wnd_get(h) : NULL;
    if (has_own_dc(w)) {
        DC *dc = w->own_dc ? get_dc(w->own_dc) : NULL;
        if (dc) {
            dc_update_window(dc);
            return w->own_dc;
        }
        w->own_dc = new_window_dc(h);
        return w->own_dc;
    }
    return new_window_dc(h);
}

HDC WINAPI GetWindowDC(HWND h)
{
    API_TRACE();
    HDC hdc = new_window_dc(h);
    WindowDC *wd = (WindowDC *)dc_get(hdc);
    if (wd && h) {
        wd->whole = 1;
        wd->gen = 0;
        dc_update_window(&wd->dc);
    }
    return hdc;
}

int WINAPI ReleaseDC(HWND h, HDC hdc)
{
    API_TRACE();
    DC *dc = get_dc(hdc);
    (void)h;
    if (!dc || dc->is_memory)
        return 0;
    /* Class and own DCs (CS_OWNDC) keep working after release. */
    if (dc->wnd && dc->wnd != desktop && dc->wnd->cls && (dc->wnd->cls->wc.style & (CS_OWNDC | CS_CLASSDC)))
        return 1;
    rgn_free(&dc->clip);
    obj_free(hdc);
    free(dc);
    return 1;
}

HDC WINAPI BeginPaint(HWND h, LPPAINTSTRUCT ps)
{
    API_TRACE();
    Window *w = wnd_get(h);
    HDC hdc;
    DC *dc;
    Region upd = {0, 0, NULL};
    RECT b;
    memset(ps, 0, sizeof *ps);
    if (!w)
        return NULL;
    if (w->erase & 2) {
        w->erase &= ~2;
        wnd_send(w, WM_NCPAINT, 1, 0);
    }
    hdc = GetDC(h);
    dc = get_dc(hdc);
    dc->paint = 1;
    rgn_copy(&upd, &w->update);
    rgn_offset(&upd, dc->origin.x, dc->origin.y);
    {
        /* clip = visible ∩ update */
        Region out = {0, 0, NULL};
        int i;
        for (i = 0; i < upd.n; i++) {
            Region part = {0, 0, NULL};
            int k;
            rgn_copy(&part, &dc->clip);
            rgn_intersect_rect(&part, &upd.r[i]);
            for (k = 0; k < part.n; k++)
                rgn_union_rect(&out, &part.r[k]);
            rgn_free(&part);
        }
        rgn_free(&dc->clip);
        dc->clip = out;
    }
    /* rcPaint: the part of the update region that is visible (children of a
     * WS_CLIPCHILDREN window and the parent's edges excluded). */
    rgn_bounds(&dc->clip, &b);
    if (!rect_empty(&b)) {
        b.left -= dc->origin.x; b.right -= dc->origin.x;
        b.top -= dc->origin.y; b.bottom -= dc->origin.y;
    }
    rgn_free(&upd);
    ps->hdc = hdc;
    ps->rcPaint = b;
    ps->fErase = 0;
    if (w->erase & 1) {
        w->erase &= ~1;
        ps->fErase = !wnd_send(w, WM_ERASEBKGND, (WPARAM)hdc, 0);
    }
    rgn_clear(&w->update);
    return hdc;
}

WINBOOL WINAPI EndPaint(HWND h, const PAINTSTRUCT *ps)
{
    API_TRACE();
    DC *dc = get_dc(ps->hdc);
    Window *w = wnd_get(h);
    if (dc && w && has_own_dc(w) && ps->hdc == w->own_dc) {
        /* the own DC stays; its clipping is the visible region again
           (Wine's release_dc deletes the paint's clip region) */
        dc->paint = 0;
        ((WindowDC *)dc)->gen = wnd_generation - 1;
        dc_update_window(dc);
        return TRUE;
    }
    if (dc) {
        rgn_free(&dc->clip);
        obj_free(ps->hdc);
        free(dc);
    }
    return TRUE;
}

HDC WINAPI CreateCompatibleDC(HDC ref)
{
    API_TRACE();
    DC *dc;
    HDC h = new_dc(&dc);
    (void)ref;
    dc->is_memory = 1;
    dc->hbitmap = stock_bitmap;
    dc->default_bitmap = stock_bitmap;
    dc->bitmap = obj_get(stock_bitmap, OBJ_BITMAP);
    return h;
}

WINBOOL WINAPI DeleteDC(HDC h)
{
    API_TRACE();
    DC *dc = get_dc(h);
    if (!dc)
        return FALSE;
    if (dc->bitmap && dc->hbitmap != stock_bitmap)
        dc->bitmap->selected = 0;
    rgn_free(&dc->clip);
    obj_free(h);
    free(dc);
    return TRUE;
}

/* Clipping of memory DCs: the whole bitmap. */
static void dc_clip(DC *dc, Region *out, Bitmap **target)
{
    *target = dc_target(dc);
    if (dc->is_memory) {
        RECT r = {0, 0, 0, 0};
        r.right = (*target)->w;
        r.bottom = (*target)->h;
        rgn_set(out, &r);
    } else {
        rgn_copy(out, &dc->clip);
    }
}

HGDIOBJ WINAPI SelectObject(HDC h, HGDIOBJ obj)
{
    API_TRACE();
    DC *dc = get_dc(h);
    HGDIOBJ old = NULL;
    if (!dc || !obj)
        return NULL;
    switch (obj_type(obj)) {
    case OBJ_BRUSH: old = dc->hbrush; dc->hbrush = obj; break;
    case OBJ_PEN: old = dc->hpen; dc->hpen = obj; break;
    case OBJ_FONT: old = dc->hfont; dc->hfont = obj; break;
    case OBJ_BITMAP:
        if (!dc->is_memory)
            return NULL;
        {
            Bitmap *b = obj_get(obj, OBJ_BITMAP);
            if (b->selected && obj != stock_bitmap && obj != dc->hbitmap)
                return NULL;
            old = dc->hbitmap;
            if (dc->bitmap && old != stock_bitmap)
                dc->bitmap->selected = 0;
            dc->hbitmap = obj;
            dc->bitmap = b;
            if (obj != stock_bitmap)
                b->selected = 1;
        }
        break;
    default:
        return NULL;
    }
    return old;
}

WINBOOL WINAPI DeleteObject(HGDIOBJ h)
{
    API_TRACE();
    ObjType t = obj_type(h);
    if (t == OBJ_NONE)
        return FALSE;
    if (obj_is_stock(h))
        return TRUE;
    switch (t) {
    case OBJ_BITMAP: {
        Bitmap *b = obj_get(h, OBJ_BITMAP);
        if (b->selected)
            return FALSE;
        bmp_destroy(b);
        break;
    }
    case OBJ_BRUSH: case OBJ_PEN: case OBJ_PALETTE: case OBJ_FONT:
        free(obj_get(h, t));
        break;
    default:
        return FALSE;
    }
    obj_free(h);
    return TRUE;
}

int WINAPI GetDeviceCaps(HDC h, int cap)
{
    API_TRACE();
    DC *dc = get_dc(h);
    Bitmap *b = dc && dc->is_memory ? dc->bitmap : NULL;
    switch (cap) {
    case HORZRES: return b ? b->w : screen_w;
    case VERTRES: return b ? b->h : screen_h;
    case BITSPIXEL: return b && b->bpp != 1 ? b->bpp : 32;
    case PLANES: return 1;
    case NUMCOLORS: return -1;
    case RASTERCAPS: return 0xbe99;     /* no RC_PALETTE: a true-colour display */
    case SIZEPALETTE: return 0;
    case LOGPIXELSX: case LOGPIXELSY: return 96;
    case TECHNOLOGY: return DT_RASDISPLAY;
    }
    return 0;
}

/* ---- bitmaps ---- */

HBITMAP WINAPI CreateBitmap(int w, int h, UINT planes, UINT bpp, const void *bits)
{
    API_TRACE();
    Bitmap *b;
    (void)planes;
    b = bmp_create(w, h, bpp == 1 ? 1 : 32, 0);
    if (bits && bpp == 1) {
        /* Device bitmaps rows are WORD aligned. */
        int y, row = ((w + 15) / 16) * 2;
        for (y = 0; y < b->h; y++)
            memcpy(bmp_row(b, y), (const char *)bits + y * row, (size_t)(row < b->stride ? row : b->stride));
    }
    return obj_new(OBJ_BITMAP, b);
}

HBITMAP WINAPI CreateCompatibleBitmap(HDC h, int w, int hgt)
{
    API_TRACE();
    DC *dc = get_dc(h);
    if (dc && dc->is_memory && dc->bitmap && dc->bitmap->is_dib) {
        /* Compatible with a DIB section: a DIB section of the same format. */
        Bitmap *src = dc->bitmap, *b = bmp_create(w, hgt, src->bpp, 0);
        b->is_dib = 1;
        b->ncolors = src->ncolors;
        memcpy(b->colors, src->colors, sizeof b->colors);
        return obj_new(OBJ_BITMAP, b);
    }
    if (dc && dc->is_memory && dc->bitmap && dc->bitmap->bpp == 1)
        return CreateBitmap(w, hgt, 1, 1, NULL);
    return obj_new(OBJ_BITMAP, bmp_create(w, hgt, 32, 0));
}

/* The bitmap whose pixels p points to (start of its rows), or NULL. */
Bitmap *bmp_of_bits(const void *p)
{
    int i;
    for (i = 1; i < 8192; i++) {
        Bitmap *b = obj_get(((Handle)OBJ_BITMAP << 16) | (Handle)i, OBJ_BITMAP);
        if (b && b->bits && (const unsigned char *)p == b->bits)
            return b;
    }
    return NULL;
}

HBITMAP WINAPI CreateDIBSection(HDC h, const BITMAPINFO *bi, UINT usage, void **bits, HANDLE section, DWORD off)
{
    API_TRACE();
    const BITMAPINFOHEADER *hd = &bi->bmiHeader;
    int bpp = hd->biBitCount, n, i;
    Bitmap *b;
    DC *dc = get_dc(h);
    (void)section; (void)off;
    if (bpp != 1 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32) {
        plat_log("CreateDIBSection: %d bpp unsupported\n", bpp);
        return NULL;
    }
    if (bpp == 4)
        bpp = 8;    /* not used by the game */
    b = bmp_create(hd->biWidth, hd->biHeight < 0 ? -hd->biHeight : hd->biHeight, bpp, hd->biHeight > 0);
    b->is_dib = 1;
    if (bpp <= 8) {
        n = hd->biClrUsed ? (int)hd->biClrUsed : 1 << bpp;
        if (n > 256)
            n = 256;
        b->ncolors = 1 << bpp;
        for (i = 0; i < n; i++) {
            if (usage == DIB_PAL_COLORS) {
                Palette *p = dc ? obj_get(dc->hpalette, OBJ_PALETTE) : NULL;
                WORD idx = ((const WORD *)bi->bmiColors)[i];
                if (p && idx < p->n) {
                    b->colors[i].rgbRed = p->e[idx].peRed;
                    b->colors[i].rgbGreen = p->e[idx].peGreen;
                    b->colors[i].rgbBlue = p->e[idx].peBlue;
                }
            } else {
                b->colors[i] = bi->bmiColors[i];
                b->colors[i].rgbReserved = 0;
            }
        }
    }
    if (bits)
        *bits = b->bits;
    return obj_new(OBJ_BITMAP, b);
}

UINT WINAPI GetDIBColorTable(HDC h, UINT start, UINT n, RGBQUAD *out)
{
    API_TRACE();
    DC *dc = get_dc(h);
    UINT i;
    if (!dc || !dc->is_memory || !dc->bitmap || dc->bitmap->bpp > 8)
        return 0;
    for (i = 0; i < n && start + i < 256; i++)
        out[i] = dc->bitmap->colors[start + i];
    return i;
}

UINT WINAPI SetDIBColorTable(HDC h, UINT start, UINT n, const RGBQUAD *in)
{
    API_TRACE();
    DC *dc = get_dc(h);
    UINT i;
    if (!dc || !dc->is_memory || !dc->bitmap || dc->bitmap->bpp > 8)
        return 0;
    for (i = 0; i < n && start + i < 256; i++) {
        dc->bitmap->colors[start + i] = in[i];
        dc->bitmap->colors[start + i].rgbReserved = 0;
    }
    return i;
}

WINBOOL WINAPI GetBitmapDimensionEx(HBITMAP h, LPSIZE s)
{
    API_TRACE();
    (void)h;
    s->cx = s->cy = 0;      /* never set by the game */
    return TRUE;
}

/* (Also a sign for the idle detection: win_dispatch_all_messages ends with it.) */
WINBOOL WINAPI GdiFlush(void) { API_TRACE(); plat_gdi_flushed(); return TRUE; }

/* ---- palettes ---- */

HPALETTE WINAPI CreatePalette(const LOGPALETTE *lp)
{
    API_TRACE();
    Palette *p = calloc(1, sizeof *p);
    p->n = lp->palNumEntries > 256 ? 256 : lp->palNumEntries;
    memcpy(p->e, lp->palPalEntry, sizeof(PALETTEENTRY) * (size_t)p->n);
    return obj_new(OBJ_PALETTE, p);
}

HPALETTE WINAPI SelectPalette(HDC h, HPALETTE pal, WINBOOL bg)
{
    API_TRACE();
    DC *dc = get_dc(h);
    HPALETTE old;
    (void)bg;
    if (!dc || !obj_get(pal, OBJ_PALETTE))
        return NULL;
    old = dc->hpalette;
    dc->hpalette = pal;
    return old;
}

UINT WINAPI RealizePalette(HDC h) { API_TRACE(); (void)h; return 0; }

UINT WINAPI GetPaletteEntries(HPALETTE h, UINT start, UINT n, LPPALETTEENTRY out)
{
    API_TRACE();
    Palette *p = obj_get(h, OBJ_PALETTE);
    UINT i;
    if (!p)
        return 0;
    if (!out)
        return (UINT)p->n;
    for (i = 0; i < n && start + i < (UINT)p->n; i++)
        out[i] = p->e[start + i];
    return i;
}

UINT WINAPI SetPaletteEntries(HPALETTE h, UINT start, UINT n, const PALETTEENTRY *in)
{
    API_TRACE();
    Palette *p = obj_get(h, OBJ_PALETTE);
    UINT i;
    if (!p)
        return 0;
    for (i = 0; i < n && start + i < (UINT)p->n; i++)
        p->e[start + i] = in[i];
    return i;
}

WINBOOL WINAPI AnimatePalette(HPALETTE h, UINT start, UINT n, const PALETTEENTRY *in)
{
    API_TRACE();
    /* Only PC_RESERVED entries change; no effect on a true-colour display. */
    Palette *p = obj_get(h, OBJ_PALETTE);
    UINT i;
    if (!p)
        return FALSE;
    for (i = 0; i < n && start + i < (UINT)p->n; i++)
        if (p->e[start + i].peFlags & PC_RESERVED)
            p->e[start + i] = in[i];
    return TRUE;
}

UINT WINAPI GetNearestPaletteIndex(HPALETTE h, COLORREF c)
{
    API_TRACE();
    Palette *p = obj_get(h, OBJ_PALETTE);
    RGBQUAD t[256];
    int i;
    if (!p)
        return CLR_INVALID;
    for (i = 0; i < p->n; i++) {
        t[i].rgbRed = p->e[i].peRed;
        t[i].rgbGreen = p->e[i].peGreen;
        t[i].rgbBlue = p->e[i].peBlue;
    }
    return (UINT)nearest_in(t, p->n, c);
}

UINT WINAPI GetSystemPaletteEntries(HDC h, UINT start, UINT n, LPPALETTEENTRY out)
{
    API_TRACE();
    (void)h; (void)start; (void)n; (void)out;
    return 0;           /* not a palette device */
}

COLORREF dc_resolve_color(DC *dc, COLORREF c)
{
    if ((c >> 24) == 1) {
        Palette *p = obj_get(dc->hpalette, OBJ_PALETTE);
        WORD i = (WORD)(c & 0xffff);
        if (p && i < p->n)
            return RGB(p->e[i].peRed, p->e[i].peGreen, p->e[i].peBlue);
        return 0;
    }
    return c & 0xffffff;
}

/* ---- brushes and pens ---- */

HBRUSH WINAPI CreateSolidBrush(COLORREF c) { API_TRACE(); return new_brush(BS_SOLID, c); }

HPEN WINAPI CreatePen(int style, int width, COLORREF c) { API_TRACE(); return new_pen(style, width, c); }

/* ---- filling ---- */

/* Debugging: CIV2_TRACE_PT=x,y logs drawing that touches that desktop pixel. */
static int trace_x = -1, trace_y = -1, trace_init;

/* r: on surface b; *sr: the same in (Windows) screen coordinates. */
static int traced_rect(const Bitmap *b, const RECT *r, RECT *sr)
{
    API_TRACE();
    Window *w;
    if (!trace_init) {
        const char *s = getenv("CIV2_TRACE_PT");
        trace_init = 1;
        if (s)
            sscanf(s, "%d,%d", &trace_x, &trace_y);
    }
    if (trace_x < 0)
        return 0;
    *sr = *r;
    if (b != screen && b != root) {
        for (w = desktop->children; w && w->surf != b; w = w->next)
            ;
        if (!w)
            return 0;
        OffsetRect(sr, w->rect.left, w->rect.top);
    }
    return trace_x >= sr->left && trace_x < sr->right && trace_y >= sr->top && trace_y < sr->bottom;
}

static int traced(const Bitmap *b, const RECT *r)
{
    RECT sr;
    return traced_rect(b, r, &sr);
}

/* Fill a device-coordinate rectangle, clipped. */
static void fill_device(Bitmap *b, const Region *clip, const RECT *r, COLORREF rgb, int rop2_xor)
{
    int i, x, y;
    {
        RECT sr;
        if (traced_rect(b, r, &sr))
            plat_log("trace: fill %d,%d,%d,%d color %06x xor %d\n", (int)sr.left, (int)sr.top, (int)sr.right,
                     (int)sr.bottom, (unsigned)rgb, rop2_xor);
    }
    for (i = 0; i < clip->n; i++) {
        RECT a;
        if (!rect_intersect(&a, &clip->r[i], r))
            continue;
        if (a.left < 0) a.left = 0;
        if (a.top < 0) a.top = 0;
        if (a.right > b->w) a.right = b->w;
        if (a.bottom > b->h) a.bottom = b->h;
        if (b->bpp == 8) {
            int idx = surf_nearest(b, rgb);
            for (y = a.top; y < a.bottom; y++) {
                unsigned char *p = bmp_row(b, y);
                if (rop2_xor)
                    for (x = a.left; x < a.right; x++)
                        p[x] ^= 0xff;
                else
                    memset(p + a.left, idx, (size_t)(a.right - a.left));
            }
        } else if (b->bpp == 32) {
            DWORD v = (DWORD)(GetRValue(rgb) << 16 | GetGValue(rgb) << 8 | GetBValue(rgb));
            for (y = a.top; y < a.bottom; y++) {
                DWORD *p = (DWORD *)bmp_row(b, y);
                for (x = a.left; x < a.right; x++)
                    p[x] = rop2_xor ? p[x] ^ 0xffffff : v;
            }
        } else {
            for (y = a.top; y < a.bottom; y++)
                for (x = a.left; x < a.right; x++)
                    surf_put(b, x, y, rop2_xor ? surf_get(b, x, y) ^ 0xffffff : rgb);
        }
    }
}

void dc_fill_rect(DC *dc, const RECT *r, COLORREF c)
{
    Region clip = {0, 0, NULL};
    Bitmap *b;
    RECT d = *r;
    dc_clip(dc, &clip, &b);
    d.left += dc->origin.x; d.right += dc->origin.x;
    d.top += dc->origin.y; d.bottom += dc->origin.y;
    if (dc->is_memory) {
        d = *r;
    }
    fill_device(b, &clip, &d, dc_resolve_color(dc, c), 0);
    rgn_free(&clip);
}

int WINAPI FillRect(HDC h, const RECT *r, HBRUSH hbr)
{
    API_TRACE();
    DC *dc = get_dc(h);
    Brush *br;
    if ((uintptr_t)hbr <= COLOR_MENUBAR + 1 && hbr)
        hbr = GetSysColorBrush((int)(uintptr_t)hbr - 1);
    br = obj_get(hbr, OBJ_BRUSH);
    if (!dc || !br || br->style == BS_NULL)
        return 0;
    dc_fill_rect(dc, r, br->color);
    return 1;
}

int WINAPI FrameRect(HDC h, const RECT *r, HBRUSH hbr)
{
    API_TRACE();
    RECT e;
    if (rect_empty(r))
        return 0;
    e = *r; e.bottom = e.top + 1; FillRect(h, &e, hbr);
    e = *r; e.top = e.bottom - 1; FillRect(h, &e, hbr);
    e = *r; e.right = e.left + 1; FillRect(h, &e, hbr);
    e = *r; e.left = e.right - 1; FillRect(h, &e, hbr);
    return 1;
}

COLORREF WINAPI SetPixel(HDC h, int x, int y, COLORREF c)
{
    API_TRACE();
    DC *dc = get_dc(h);
    RECT r;
    if (!dc)
        return CLR_INVALID;
    r.left = x; r.top = y; r.right = x + 1; r.bottom = y + 1;
    dc_fill_rect(dc, &r, c);
    return dc_resolve_color(dc, c);
}

COLORREF WINAPI GetPixel(HDC h, int x, int y)
{
    API_TRACE();
    DC *dc = get_dc(h);
    Bitmap *b;
    if (!dc)
        return CLR_INVALID;
    b = dc_target(dc);
    return surf_get(b, x + dc->origin.x, y + dc->origin.y);
}

/* ---- lines ---- */

WINBOOL WINAPI MoveToEx(HDC h, int x, int y, LPPOINT old)
{
    API_TRACE();
    DC *dc = get_dc(h);
    if (!dc)
        return FALSE;
    if (old)
        *old = dc->pos;
    dc->pos.x = x;
    dc->pos.y = y;
    return TRUE;
}

/* Bresenham, last point excluded, as GDI draws lines: Wine's dibdrv
 * (bres_line_with_bias, init_bres_params), where a tie in the decision
 * variable steps the minor axis only in octants 3, 5, 6 and 8 (get_bias),
 * so a slope of 1/2 rounds the same way as on Windows. */
WINBOOL WINAPI LineTo(HDC h, int x1, int y1)
{
    API_TRACE();
    DC *dc = get_dc(h);
    Pen *pen;
    Region clip = {0, 0, NULL};
    Bitmap *b;
    int x0, y0, dx, dy, adx, ady, sx, sy, err, add1, add2, len, bias, xmajor, oct;
    COLORREF c;
    if (!dc)
        return FALSE;
    pen = obj_get(dc->hpen, OBJ_PEN);
    x0 = dc->pos.x;
    y0 = dc->pos.y;
    dc->pos.x = x1;
    dc->pos.y = y1;
    if (!pen || pen->style == PS_NULL)
        return TRUE;
    c = dc_resolve_color(dc, pen->color);
    dc_clip(dc, &clip, &b);
    dx = x1 - x0; dy = y1 - y0;
    adx = abs(dx); ady = abs(dy);
    sx = dx > 0 ? 1 : -1; sy = dy > 0 ? 1 : -1;
    if (dy > 0)
        oct = dx > 0 ? (dx > dy ? 1 : 2) : (-dx > dy ? 4 : 3);
    else
        oct = dx < 0 ? (-dx > -dy ? 5 : 6) : (dx > -dy ? 8 : 7);
    bias = (1 << (oct - 1)) & 0xb4 ? 1 : 0;
    xmajor = adx > ady;
    if (xmajor) {
        err = 2 * ady - adx; add1 = 2 * ady - 2 * adx; add2 = 2 * ady; len = adx;
    } else {
        err = 2 * adx - ady; add1 = 2 * adx - 2 * ady; add2 = 2 * adx; len = ady;
    }
    while (len--) {
        int px = x0 + dc->origin.x, py = y0 + dc->origin.y, i;
        for (i = 0; i < clip.n; i++)
            if (px >= clip.r[i].left && px < clip.r[i].right && py >= clip.r[i].top && py < clip.r[i].bottom) {
                surf_put(b, px, py, c);
                break;
            }
        if (err + bias > 0) {
            if (xmajor)
                y0 += sy;
            else
                x0 += sx;
            err += add1;
        } else {
            err += add2;
        }
        if (xmajor)
            x0 += sx;
        else
            y0 += sy;
    }
    rgn_free(&clip);
    return TRUE;
}

/* ---- blits ---- */

/* Pixel transfer from src to dst for one blit.  Colours go through a table
 * built once per blit.  Monochrome conversions follow GDI: colour -> mono
 * gives 1 where the source is the source DC's background colour; mono ->
 * colour gives the destination DC's text colour for 0 and its background
 * colour for 1. */
typedef struct {
    Bitmap *src, *dst;
    int identity;                   /* 8 -> 8 with equal colour tables */
    unsigned char map8[256];        /* source index -> 8-bpp destination index */
    DWORD map32[256];               /* source index -> 0x00RRGGBB */
    COLORREF rgb[256];              /* source index -> COLORREF */
    int to_mono;
    COLORREF key;
} Xfer;

static void xfer_init(Xfer *x, Bitmap *src, Bitmap *dst, DC *sd, DC *dd)
{
    int i, n = src->ncolors ? src->ncolors : 256;
    memset(x, 0, sizeof *x);
    x->src = src;
    x->dst = dst;
    x->to_mono = dst->bpp == 1 && src->bpp != 1;
    x->key = dc_resolve_color(sd, sd->bk_color);
    if (src->bpp > 8)
        return;
    for (i = 0; i < 256; i++) {
        RGBQUAD q = src->colors[i < n ? i : 0];
        x->rgb[i] = RGB(q.rgbRed, q.rgbGreen, q.rgbBlue);
    }
    if (src->bpp == 1 && dst->bpp != 1) {
        x->rgb[0] = dc_resolve_color(dd, dd->text_color);
        x->rgb[1] = dc_resolve_color(dd, dd->bk_color);
    }
    for (i = 0; i < 256; i++)
        x->map32[i] = (DWORD)(GetRValue(x->rgb[i]) << 16 | GetGValue(x->rgb[i]) << 8 | GetBValue(x->rgb[i]));
    if (dst->bpp == 8) {
        if (src->bpp == 8 && !memcmp(src->colors, dst->colors, sizeof src->colors))
            x->identity = 1;
        else
            for (i = 0; i < 256; i++)
                x->map8[i] = (unsigned char)surf_nearest(dst, x->rgb[i]);
    }
}

static int rop_apply(DWORD rop, int s, int d)
{
    switch (rop) {
    case SRCAND: return s & d;
    case SRCPAINT: return s | d;
    case SRCINVERT: return s ^ d;
    case NOTSRCCOPY: return ~s;
    case SRCERASE: return s & ~d;
    case NOTSRCERASE: return ~(s | d);
    case MERGEPAINT: return ~s | d;
    default: return s;      /* SRCCOPY */
    }
}

/* Source pixel: an index for <= 8 bpp, else 0x00RRGGBB. */
static DWORD src_pixel(const Bitmap *s, const unsigned char *row, int x)
{
    switch (s->bpp) {
    case 1: return (row[x >> 3] >> (7 - (x & 7))) & 1;
    case 8: return row[x];
    case 16: { WORD v = ((const WORD *)row)[x]; return (DWORD)((v >> 10 & 31) << 19 | (v >> 5 & 31) << 11 | (v & 31) << 3); }
    case 24: return (DWORD)(row[x * 3 + 2] << 16 | row[x * 3 + 1] << 8 | row[x * 3]);
    default: return ((const DWORD *)row)[x] & 0xffffff;
    }
}

static void xfer_row(Xfer *x, int sy, int dy, int dx0, int n, const int *sxs, DWORD rop)
{
    Bitmap *s = x->src, *d = x->dst;
    const unsigned char *sp = bmp_row(s, sy);
    unsigned char *dp = bmp_row(d, dy);
    int i;
    for (i = 0; i < n; i++) {
        int dxx = dx0 + i;
        DWORD v = src_pixel(s, sp, sxs[i]);
        if (x->to_mono) {
            COLORREF c = s->bpp <= 8 ? x->rgb[v] : RGB(v >> 16, v >> 8 & 0xff, v & 0xff);
            unsigned char *b = dp + (dxx >> 3), mask = (unsigned char)(0x80 >> (dxx & 7));
            int r = rop_apply(rop, c == x->key, (*b & mask) != 0) & 1;
            if (r) *b |= mask; else *b &= (unsigned char)~mask;
            continue;
        }
        switch (d->bpp) {
        case 8: {
            unsigned char sv = s->bpp <= 8 ? (x->identity ? (unsigned char)v : x->map8[v])
                                            : (unsigned char)surf_nearest(d, RGB(v >> 16, v >> 8 & 0xff, v & 0xff));
            dp[dxx] = (unsigned char)rop_apply(rop, sv, dp[dxx]);
            break;
        }
        case 32: {
            DWORD sv = s->bpp <= 8 ? x->map32[v] : v;
            ((DWORD *)dp)[dxx] = (DWORD)rop_apply(rop, (int)sv, (int)((DWORD *)dp)[dxx]) & 0xffffff;
            break;
        }
        case 1: {
            unsigned char *b = dp + (dxx >> 3), mask = (unsigned char)(0x80 >> (dxx & 7));
            int r = rop_apply(rop, (int)v, (*b & mask) != 0) & 1;
            if (r) *b |= mask; else *b &= (unsigned char)~mask;
            break;
        }
        default: {
            COLORREF sc = s->bpp <= 8 ? x->rgb[v] : RGB(v >> 16, v >> 8 & 0xff, v & 0xff);
            COLORREF dc = surf_get(d, dxx, dy);
            surf_put(d, dxx, dy, (COLORREF)rop_apply(rop, (int)sc, (int)dc) & 0xffffff);
        }
        }
    }
}

static WINBOOL blit(HDC hd, int x, int y, int w, int h, HDC hs, int sx, int sy, int sw, int sh, DWORD rop)
{
    DC *dd = get_dc(hd), *sd = get_dc(hs);
    Region clip = {0, 0, NULL};
    Bitmap *dst, *src;
    Xfer xf;
    int i, flipx = 0, flipy = 0, *sxs, row;
    if (!dd)
        return FALSE;
    if (rop == BLACKNESS || rop == WHITENESS || rop == PATCOPY || rop == DSTINVERT) {
        RECT r;
        Brush *br = obj_get(dd->hbrush, OBJ_BRUSH);
        r.left = x; r.top = y; r.right = x + w; r.bottom = y + h;
        if (rop == DSTINVERT) {
            Region c2 = {0, 0, NULL};
            Bitmap *b;
            dc_clip(dd, &c2, &b);
            if (!dd->is_memory) {
                r.left += dd->origin.x; r.right += dd->origin.x; r.top += dd->origin.y; r.bottom += dd->origin.y;
            }
            fill_device(b, &c2, &r, 0, 1);
            rgn_free(&c2);
            return TRUE;
        }
        dc_fill_rect(dd, &r, rop == BLACKNESS ? 0 : rop == WHITENESS ? 0xffffff : br ? br->color : 0);
        return TRUE;
    }
    if (!sd)
        return FALSE;
    if (w < 0) { x += w; w = -w; flipx = 1; }
    if (h < 0) { y += h; h = -h; flipy = 1; }
    if (sw < 0) { sx += sw; sw = -sw; flipx ^= 1; }
    if (sh < 0) { sy += sh; sh = -sh; flipy ^= 1; }
    if (!w || !h || !sw || !sh)
        return TRUE;
    dc_clip(dd, &clip, &dst);
    src = dc_target(sd);
    sx += sd->origin.x;
    sy += sd->origin.y;
    x += dd->origin.x;
    y += dd->origin.y;
    xfer_init(&xf, src, dst, sd, dd);
    {
        RECT tr, sr;
        tr.left = x; tr.top = y; tr.right = x + w; tr.bottom = y + h;
        if (traced_rect(dst, &tr, &sr))
            plat_log("trace: blit to %d,%d %dx%d from %s %d,%d rop %06x\n", (int)sr.left, (int)sr.top, w, h,
                     sd->is_memory ? "memory" : "window", sx, sy, (unsigned)rop);
    }
    sxs = malloc(sizeof(int) * (size_t)w);
    for (i = 0; i < clip.n; i++) {
        RECT a, want;
        want.left = x; want.top = y; want.right = x + w; want.bottom = y + h;
        if (!rect_intersect(&a, &clip.r[i], &want))
            continue;
        if (a.left < 0) a.left = 0;
        if (a.top < 0) a.top = 0;
        if (a.right > dst->w) a.right = dst->w;
        if (a.bottom > dst->h) a.bottom = dst->h;
        for (row = a.top; row < a.bottom; row++) {
            int ry = row - y, srow, n = 0, col;
            if (flipy)
                ry = h - 1 - ry;
            srow = sy + (int)((long long)ry * sh / h);
            if (srow < 0 || srow >= src->h)
                continue;
            for (col = a.left; col < a.right; col++) {
                int rx = col - x, scol;
                if (flipx)
                    rx = w - 1 - rx;
                scol = sx + (int)((long long)rx * sw / w);
                if (scol < 0)
                    scol = 0;
                if (scol >= src->w)
                    scol = src->w - 1;
                sxs[n++] = scol;
            }
            xfer_row(&xf, srow, row, a.left, n, sxs, rop);
        }
    }
    free(sxs);
    rgn_free(&clip);
    return TRUE;
}

WINBOOL WINAPI BitBlt(HDC hd, int x, int y, int w, int h, HDC hs, int sx, int sy, DWORD rop)
{
    API_TRACE();
    return blit(hd, x, y, w, h, hs, sx, sy, w, h, rop);
}

/* Only the source-less operations blit() fills itself. */
WINBOOL WINAPI PatBlt(HDC hd, int x, int y, int w, int h, DWORD rop)
{
    API_TRACE();
    if (rop != BLACKNESS && rop != WHITENESS && rop != PATCOPY && rop != DSTINVERT)
        plat_fatal("PatBlt: rop %lx", (unsigned long)rop);
    return blit(hd, x, y, w, h, 0, 0, 0, w, h, rop);
}

WINBOOL WINAPI StretchBlt(HDC hd, int x, int y, int w, int h, HDC hs, int sx, int sy, int sw, int sh, DWORD rop)
{
    API_TRACE();
    return blit(hd, x, y, w, h, hs, sx, sy, sw, sh, rop);
}

/* ---- text state ---- */

COLORREF WINAPI SetTextColor(HDC h, COLORREF c)
{
    API_TRACE();
    DC *dc = get_dc(h);
    COLORREF old;
    if (!dc)
        return CLR_INVALID;
    old = dc->text_color;
    dc->text_color = c;
    return old;
}

COLORREF WINAPI SetBkColor(HDC h, COLORREF c)
{
    API_TRACE();
    DC *dc = get_dc(h);
    COLORREF old;
    if (!dc)
        return CLR_INVALID;
    old = dc->bk_color;
    dc->bk_color = c;
    return old;
}

int WINAPI SetBkMode(HDC h, int mode)
{
    API_TRACE();
    DC *dc = get_dc(h);
    int old;
    if (!dc)
        return 0;
    old = dc->bk_mode;
    dc->bk_mode = mode;
    return old;
}

UINT WINAPI SetTextAlign(HDC h, UINT a)
{
    API_TRACE();
    DC *dc = get_dc(h);
    UINT old;
    if (!dc)
        return GDI_ERROR;
    old = dc->text_align;
    dc->text_align = a;
    return old;
}

UINT WINAPI GetTextAlign(HDC h)
{
    API_TRACE();
    DC *dc = get_dc(h);
    return dc ? dc->text_align : GDI_ERROR;
}

DC *dc_get(HDC h) { return get_dc(h); }
