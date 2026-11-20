/*
 * Modules and resources.  The game's resource libraries (Tiles.dll,
 * Intro.dll, ...) and civ2.exe itself are read as data files: their PE
 * resource directories are parsed here.  timerdll.dll's five functions are
 * built in.
 */
#include "plat.h"
#include "../host/host.h"

typedef struct {
    char name[260];
    unsigned char *image;       /* whole file */
    size_t size;
    unsigned rsrc_off;          /* file offset of the resource directory (0: none) */
    unsigned sec_raw, sec_va;   /* its section: file offset and RVA, to map data RVAs */
    int builtin;                /* timerdll */
    int synth;                  /* a picture library the game folder lacks: its pictures made (synth.c) */
} Module;

static HMODULE exe_module;

static unsigned rd32(const unsigned char *p) { return p[0] | p[1] << 8 | p[2] << 16 | (unsigned)p[3] << 24; }
static unsigned rd16(const unsigned char *p) { return p[0] | p[1] << 8; }

static Module *load_pe(const char *winpath)
{
    char host[1024];
    FILE *f;
    Module *m;
    long n;
    unsigned pe, nsec, i, rsrc_rva, opt;

    plat_host_path(winpath, host, sizeof host, 0);
    f = fopen(host, "rb");
    if (!f)
        return NULL;
    m = calloc(1, sizeof *m);
    snprintf(m->name, sizeof m->name, "%s", winpath);
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    m->image = malloc((size_t)n);
    m->size = (size_t)n;
    if (fread(m->image, 1, (size_t)n, f) != (size_t)n) {
        fclose(f);
        free(m->image);
        free(m);
        return NULL;
    }
    fclose(f);
    if (n < 0x40 || m->image[0] != 'M' || m->image[1] != 'Z')
        return m;
    pe = rd32(m->image + 0x3c);
    if (pe + 0xf8 > (unsigned)n || memcmp(m->image + pe, "PE\0\0", 4))
        return m;
    nsec = rd16(m->image + pe + 6);
    opt = pe + 24;
    rsrc_rva = rd32(m->image + opt + 96 + 2 * 8);        /* data directory 2: resources */
    for (i = 0; i < nsec; i++) {
        const unsigned char *s = m->image + opt + rd16(m->image + pe + 20) + i * 40;
        unsigned va = rd32(s + 12), vsize = rd32(s + 8), raw = rd32(s + 20);
        if (rsrc_rva && rsrc_rva >= va && rsrc_rva < va + (vsize ? vsize : rd32(s + 16))) {
            m->sec_raw = raw;
            m->sec_va = va;
            m->rsrc_off = raw + (rsrc_rva - va);
            break;
        }
    }
    return m;
}

static const unsigned char *rva_ptr(Module *m, unsigned rva)
{
    size_t off = (size_t)m->sec_raw + (rva - m->sec_va);
    return off < m->size ? m->image + off : NULL;
}

/* Directory entry matching an id or a (case-insensitive) name. */
static const unsigned char *dir_find(Module *m, const unsigned char *dir, const char *id, int first)
{
    unsigned nnamed = rd16(dir + 12), nid = rd16(dir + 14), i;
    const unsigned char *e = dir + 16, *base = m->image + m->rsrc_off;
    for (i = 0; i < nnamed + nid; i++, e += 8) {
        unsigned name = rd32(e), off = rd32(e + 4);
        int match = first;
        if (!first) {
            if (IS_INTRESOURCE(id)) {
                match = !(name & 0x80000000u) && name == (unsigned)(uintptr_t)id;
            } else if (id[0] == '#') {
                match = !(name & 0x80000000u) && name == (unsigned)atoi(id + 1);
            } else if (name & 0x80000000u) {
                const unsigned char *s = base + (name & 0x7fffffff);
                unsigned len = rd16(s), k;
                match = strlen(id) == len;
                for (k = 0; match && k < len; k++)
                    if (toupper((unsigned char)id[k]) != toupper(rd16(s + 2 + 2 * k)))
                        match = 0;
            }
        }
        if (match)
            return base + (off & 0x7fffffff);
    }
    return NULL;
}

static const unsigned char *find_res_in(Module *m, const char *name, const char *type, unsigned *size);

/* CIV2_AUDIT: each resource asked for, by module, type and name */
static const unsigned char *find_res(Module *m, const char *name, const char *type, unsigned *size)
{
    const unsigned char *p = find_res_in(m, name, type, size);
    if (plat_audit_on()) {
        const char *base = m ? m->name : "?", *c;
        char t[32], nm[64];
        for (c = base; *c; c++)
            if (*c == '/' || *c == '\\' || *c == ':')
                base = c + 1;
        if (IS_INTRESOURCE(type)) snprintf(t, sizeof t, "%u", (unsigned)(uintptr_t)type); else snprintf(t, sizeof t, "%s", type);
        if (IS_INTRESOURCE(name)) snprintf(nm, sizeof nm, "%u", (unsigned)(uintptr_t)name); else snprintf(nm, sizeof nm, "%s", name);
        plat_log("audit res %s %s %s %s\n", p ? "found" : "missing", base, t, nm);
    }
    return p;
}

static const unsigned char *find_res_in(Module *m, const char *name, const char *type, unsigned *size)
{
    const unsigned char *t, *n, *l, *base;
    if (m && m->synth)
        return !IS_INTRESOURCE(type) && !strcasecmp(type, "GIFS") && IS_INTRESOURCE(name)
                   ? synth_resource(m->name, (int)(uintptr_t)name, size)
                   : NULL;
    if (!m || !m->rsrc_off)
        return NULL;
    base = m->image + m->rsrc_off;
    t = dir_find(m, base, type, 0);
    if (!t)
        return NULL;
    n = dir_find(m, t, name, 0);
    if (!n)
        return NULL;
    l = dir_find(m, n, NULL, 1);            /* first language */
    if (!l)
        return NULL;
    *size = rd32(l + 4);
    return rva_ptr(m, rd32(l));
}

/* ---- modules ---- */

static Module *module_of(HMODULE h)
{
    if (!h)
        h = exe_module;
    return obj_get(h, OBJ_MODULE);
}

void resources_init(void)
{
    char win[1100];
    Module *m;
    GetModuleFileNameA(0, win, sizeof win);
    m = load_pe(win);
    if (!m) {
        m = calloc(1, sizeof *m);
        plat_log("civ2.exe not found: no cursors, icons or bitmaps from it\n");
    }
    exe_module = obj_new(OBJ_MODULE, m);
}

HMODULE plat_exe_module(void) { return exe_module; }

HMODULE WINAPI LoadLibraryExA(LPCSTR name, HANDLE file, DWORD flags)
{ API_TRACE();
    Module *m;
    const char *base = strrchr(name, '\\');
    (void)file; (void)flags;
    base = base ? base + 1 : name;
    if (!strcasecmp(base, "timerdll.dll") || !strcasecmp(base, "timerdll")) {
        m = calloc(1, sizeof *m);
        m->builtin = 1;
        snprintf(m->name, sizeof m->name, "timerdll.dll");
        if (plat_audit_on())
            plat_log("audit lib builtin %s\n", name);
        return obj_new(OBJ_MODULE, m);
    }
    m = load_pe(name);
    if (plat_audit_on())
        plat_log("audit lib %s %s\n", m ? "found" : "missing", name);
    if ((!m || !m->rsrc_off) && synth_module(base)) {
        /* the game folder lacks this picture library: its pictures are made
           from the art (synth.c, docs/NO-INSTALL.md) */
        size_t i;
        if (m) {
            free(m->image);
            free(m);
        }
        m = calloc(1, sizeof *m);
        m->synth = 1;
        for (i = 0; base[i] && i < sizeof m->name - 1; i++)
            m->name[i] = (char)tolower((unsigned char)base[i]);
        plat_log("LoadLibrary(%s): not found; its pictures made from the art\n", name);
        return obj_new(OBJ_MODULE, m);
    }
    if (!m) {
        plat_log("LoadLibrary(%s): not found\n", name);
        return 0;
    }
    return obj_new(OBJ_MODULE, m);
}

HMODULE WINAPI LoadLibraryA(LPCSTR name) { API_TRACE(); return LoadLibraryExA(name, 0, 0); }

WINBOOL WINAPI FreeLibrary(HMODULE h)
{ API_TRACE();
    Module *m = obj_get(h, OBJ_MODULE);
    if (!m || h == exe_module)
        return FALSE;
    free(m->image);
    free(m);
    obj_free(h);
    return TRUE;
}

/* timerdll.dll: multimedia timer callbacks that post message 0x52c once
 * per timer until reset. */
static UINT timer_ids[16];
static int timer_notified[16];

static void WINAPI tdll_SetTimerID(int i, UINT id) { API_TRACE(); if (i >= 0 && i < 16) timer_ids[i] = id; }
static UINT WINAPI tdll_GetTimerID(int i) { API_TRACE(); return i >= 0 && i < 16 ? timer_ids[i] : 0; }
static int WINAPI tdll_GetTimerIndex(UINT id)
{ API_TRACE();
    int i;
    for (i = 0; i < 16; i++)
        if (timer_ids[i] == id)
            return i;
    return 0;
}
static void WINAPI tdll_ResetTimerNotified(int i) { API_TRACE(); if (i >= 0 && i < 16) timer_notified[i] = 0; }
static void WINAPI tdll_TimerCallBack(UINT id, UINT msg, DWORD_PTR user, DWORD_PTR dw1, DWORD_PTR dw2)
{ API_TRACE();
    int i = tdll_GetTimerIndex(id);
    (void)msg; (void)dw1; (void)dw2;
    if (!i || timer_notified[i])
        return;
    PostMessageA((HWND)user, 0x52c, (WPARAM)i, (LPARAM)plat_now());     /* not the game's own clock query */
    timer_notified[i] = 1;
}

FARPROC WINAPI GetProcAddress(HMODULE h, LPCSTR name)
{ API_TRACE();
    Module *m = obj_get(h, OBJ_MODULE);
    if (m && m->builtin && !IS_INTRESOURCE(name)) {
        if (!strcmp(name, "TimerCallBack")) return (FARPROC)tdll_TimerCallBack;
        if (!strcmp(name, "SetTimerID")) return (FARPROC)tdll_SetTimerID;
        if (!strcmp(name, "GetTimerID")) return (FARPROC)tdll_GetTimerID;
        if (!strcmp(name, "GetTimerIndex")) return (FARPROC)tdll_GetTimerIndex;
        if (!strcmp(name, "ResetTimerNotified")) return (FARPROC)tdll_ResetTimerNotified;
    }
    plat_log("GetProcAddress(%s): not available\n", IS_INTRESOURCE(name) ? "#" : name);
    return NULL;
}

/* ---- resources ---- */

typedef struct { const unsigned char *p; unsigned size; } Res;

char plat_res_module[32];       /* the module the last resource found is in */

HRSRC WINAPI FindResourceA(HMODULE h, LPCSTR name, LPCSTR type)
{ API_TRACE();
    Res *r;
    unsigned size;
    Module *m = module_of(h);
    const unsigned char *p = find_res(m, name, type, &size);
    if (!p)
        return 0;
    {   /* the module's file name, lower case (HD graphics: the pack's resource/MODULE/...) */
        const char *base = m->name, *c;
        size_t i;
        for (c = m->name; *c; c++)
            if (*c == '/' || *c == '\\' || *c == ':')
                base = c + 1;
        for (i = 0; base[i] && i < sizeof plat_res_module - 1; i++)
            plat_res_module[i] = (char)tolower((unsigned char)base[i]);
        plat_res_module[i] = 0;
    }
    r = malloc(sizeof *r);
    r->p = p;
    r->size = size;
    return obj_new(OBJ_RSRC, r);
}

HGLOBAL WINAPI LoadResource(HMODULE h, HRSRC r) { API_TRACE(); (void)h; return obj_get(r, OBJ_RSRC) ? r : 0; }

LPVOID WINAPI LockResource(HGLOBAL h)
{ API_TRACE();
    Res *r = obj_get(h, OBJ_RSRC);
    return r ? (LPVOID)r->p : NULL;
}

WINBOOL WINAPI FreeResource(HGLOBAL h) { API_TRACE(); (void)h; return FALSE; }

DWORD WINAPI SizeofResource(HMODULE h, HRSRC r)
{ API_TRACE();
    Res *res = obj_get(r, OBJ_RSRC);
    (void)h;
    return res ? res->size : 0;
}

/* A packed DIB (header, colour table, bits) as a 32-bpp bitmap; with
 * mask_rows, the bottom half is an AND mask (icons and cursors). */
static Bitmap *dib_to_bitmap(const unsigned char *p, int icon, Bitmap **mask)
{
    const BITMAPINFOHEADER *h = (const BITMAPINFOHEADER *)p;
    int w = h->biWidth, ht = h->biHeight, bpp = h->biBitCount, ncol, x, y, stride, mstride;
    const RGBQUAD *pal = (const RGBQUAD *)(p + h->biSize);
    const unsigned char *bits;
    Bitmap *b;
    if (icon)
        ht /= 2;
    ncol = bpp <= 8 ? (h->biClrUsed ? (int)h->biClrUsed : 1 << bpp) : 0;
    bits = p + h->biSize + ncol * 4;
    stride = ((w * bpp + 31) / 32) * 4;
    b = bmp_create(w, ht < 0 ? -ht : ht, 32, 0);
    for (y = 0; y < b->h; y++) {
        const unsigned char *row = bits + (size_t)stride * (size_t)(ht > 0 ? b->h - 1 - y : y);
        for (x = 0; x < w; x++) {
            COLORREF c = 0;
            int idx;
            switch (bpp) {
            case 1: idx = (row[x >> 3] >> (7 - (x & 7))) & 1; c = RGB(pal[idx].rgbRed, pal[idx].rgbGreen, pal[idx].rgbBlue); break;
            case 4: idx = (row[x >> 1] >> ((x & 1) ? 0 : 4)) & 15; c = RGB(pal[idx].rgbRed, pal[idx].rgbGreen, pal[idx].rgbBlue); break;
            case 8: idx = row[x]; c = RGB(pal[idx].rgbRed, pal[idx].rgbGreen, pal[idx].rgbBlue); break;
            case 24: c = RGB(row[x * 3 + 2], row[x * 3 + 1], row[x * 3]); break;
            case 32: c = RGB(row[x * 4 + 2], row[x * 4 + 1], row[x * 4]); break;
            }
            surf_put(b, x, y, c);
        }
    }
    if (icon && mask) {
        const unsigned char *mbits = bits + (size_t)stride * (size_t)b->h;
        mstride = ((w + 31) / 32) * 4;
        *mask = bmp_create(w, b->h, 1, 0);
        for (y = 0; y < b->h; y++)
            memcpy(bmp_row(*mask, y), mbits + (size_t)mstride * (size_t)(b->h - 1 - y), (size_t)((*mask)->stride));
    }
    return b;
}

/* ---- civ2.exe's pictures where the game folder has no civ2.exe (synth.c) ---- */

static int exe_missing(HINSTANCE inst)
{
    Module *m = module_of(inst);
    return m && module_of(0) == m && !m->rsrc_off;
}

/* The radio buttons' and check boxes' pictures (301 to 306, 32 x 32): the
   art's in the original's place (16 x 16 in the middle), on its
   transparent colour (the game draws them with teal, 0x808000, as
   transparent: gdi_blit_transparent). */
static HBITMAP synth_bitmap(int id)
{
    unsigned char *px = synth_exe_rgba(2, id, 32, 32, 8, 8, 16, 16);
    Bitmap *b;
    int x, y;
    if (!px)
        return 0;
    b = bmp_create(32, 32, 32, 0);
    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++) {
            const unsigned char *p = px + (y * 32 + x) * 4;
            surf_put(b, x, y, p[3] >= 128 ? RGB(p[0], p[1], p[2]) : RGB(0, 128, 128));
        }
    free(px);
    return obj_new(OBJ_BITMAP, b);
}

typedef struct { Bitmap *image, *mask; int hx, hy; int host; } Cursor;

/* A cursor or the icon: the art's, 32 x 32; the cursors' hot spot is their
   arrow's tip, at the top left. */
static Cursor *synth_cursor(int id, int cursor)
{
    unsigned char *px = synth_exe_rgba(cursor ? 12 : 14, id, 32, 32, 0, 0, 32, 32);
    Cursor *c;
    int x, y;
    if (!px)
        return NULL;
    c = calloc(1, sizeof *c);
    c->image = bmp_create(32, 32, 32, 0);
    c->mask = bmp_create(32, 32, 1, 0);
    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++) {
            const unsigned char *p = px + (y * 32 + x) * 4;
            surf_put(c->image, x, y, RGB(p[0], p[1], p[2]));
            if (p[3] < 128)
                bmp_row(c->mask, y)[x >> 3] |= (unsigned char)(0x80 >> (x & 7));
        }
    c->hx = c->hy = cursor ? 1 : 0;
    free(px);
    return c;
}

HBITMAP WINAPI LoadBitmapA(HINSTANCE inst, LPCSTR name)
{ API_TRACE();
    unsigned size;
    const unsigned char *p = find_res(module_of(inst), name, (LPCSTR)RT_BITMAP, &size);
    if (!p)
        return exe_missing(inst) && IS_INTRESOURCE(name) ? synth_bitmap((int)(uintptr_t)name) : 0;
    return obj_new(OBJ_BITMAP, dib_to_bitmap(p, 0, NULL));
}

/* ---- icons and cursors ---- */

/* The best image of an icon/cursor group: the largest. */
static const unsigned char *group_image(Module *m, LPCSTR name, int cursor, int *hx, int *hy)
{
    unsigned size, n, i, best = 0, best_area = 0;
    const unsigned char *g = find_res(m, name, (LPCSTR)(uintptr_t)(cursor ? 12 : 14), &size), *img;
    if (!g)
        return NULL;
    n = rd16(g + 4);
    for (i = 0; i < n; i++) {
        const unsigned char *e = g + 6 + i * 14;
        unsigned w = cursor ? rd16(e) : e[0], h = cursor ? rd16(e + 2) / 2 : e[1];
        if (!w) w = 256;
        if (!h) h = 256;
        if (w * h > best_area) {
            best_area = w * h;
            best = rd16(e + 12);
        }
    }
    img = find_res(m, (LPCSTR)(uintptr_t)best, (LPCSTR)(uintptr_t)(cursor ? 1 : 3), &size);
    if (img && cursor) {
        *hx = rd16(img);
        *hy = rd16(img + 2);
        img += 4;
    }
    return img;
}

static HCURSOR system_cursor(int id)
{
    static HCURSOR cache[32];
    /* IDC_ARROW, IBEAM, WAIT, CROSS, UPARROW (32512..), then IDC_SIZENWSE,
     * SIZENESW, SIZEWE, SIZENS, SIZEALL, (32647), NO, HAND (32642..) */
    static const int map[] = {0, 1, 2, 3, 0, 4, 5, 6, 7, 8, 0, 9, 10};
    int k = id >= 32512 && id <= 32516 ? id - 32512 : id >= 32642 && id <= 32649 ? id - 32642 + 5 : 0;
    if (!cache[k]) {
        Cursor *c = calloc(1, sizeof *c);
        c->host = host_cursor_system(map[k]);
        cache[k] = obj_new(OBJ_CURSOR, c);
        obj_set_stock(cache[k]);
    }
    return cache[k];
}

static HCURSOR load_cursor_or_icon(HINSTANCE inst, LPCSTR name, int cursor)
{
    Cursor *c;
    int hx = 0, hy = 0;
    const unsigned char *img;
    if (!inst && IS_INTRESOURCE(name))
        return system_cursor((int)(uintptr_t)name);
    img = group_image(module_of(inst), name, cursor, &hx, &hy);
    if (!img && exe_missing(inst) && IS_INTRESOURCE(name) && (c = synth_cursor((int)(uintptr_t)name, cursor)))
        return obj_new(OBJ_CURSOR, c);
    if (!img)
        return cursor ? system_cursor(32512) : 0;
    c = calloc(1, sizeof *c);
    c->image = dib_to_bitmap(img, 1, &c->mask);
    c->hx = hx;
    c->hy = hy;
    return obj_new(OBJ_CURSOR, c);
}

HCURSOR WINAPI LoadCursorA(HINSTANCE inst, LPCSTR name) { API_TRACE(); return load_cursor_or_icon(inst, name, 1); }
HICON WINAPI LoadIconA(HINSTANCE inst, LPCSTR name) { API_TRACE(); return load_cursor_or_icon(inst, name, 0); }

static WINBOOL destroy_cursor(HCURSOR h)
{
    Cursor *c = obj_get(h, OBJ_CURSOR);
    if (!c || obj_is_stock(h))
        return FALSE;
    bmp_destroy(c->image);
    bmp_destroy(c->mask);
    if (c->host)
        host_cursor_free(c->host);
    free(c);
    obj_free(h);
    return TRUE;
}

WINBOOL WINAPI DestroyCursor(HCURSOR h) { API_TRACE(); return destroy_cursor(h); }
WINBOOL WINAPI DestroyIcon(HICON h) { API_TRACE(); return destroy_cursor(h); }

WINBOOL WINAPI DrawIcon(HDC hdc, int x, int y, HICON h)
{ API_TRACE();
    Cursor *c = obj_get(h, OBJ_CURSOR);
    DC *dc = dc_get(hdc);
    Bitmap *b;
    int i, j;
    if (!c || !dc || !c->image)
        return FALSE;
    b = dc_target(dc);
    for (j = 0; j < c->image->h; j++)
        for (i = 0; i < c->image->w; i++) {
            int px = x + i + dc->origin.x, py = y + j + dc->origin.y, k, inside = dc->is_memory;
            if (c->mask && (bmp_row(c->mask, j)[i >> 3] & (0x80 >> (i & 7))))
                continue;       /* transparent */
            for (k = 0; !inside && k < dc->clip.n; k++)
                inside = px >= dc->clip.r[k].left && px < dc->clip.r[k].right && py >= dc->clip.r[k].top &&
                         py < dc->clip.r[k].bottom;
            if (inside)
                surf_put(b, px, py, surf_get(c->image, i, j));
        }
    return TRUE;
}

void cursor_set_image(HCURSOR h)
{
    Cursor *c = obj_get(h, OBJ_CURSOR);
    if (!c)
        return;
    if (!c->host && c->image) {
        uint32_t *px = malloc((size_t)c->image->w * c->image->h * 4);
        int x, y;
        for (y = 0; y < c->image->h; y++)
            for (x = 0; x < c->image->w; x++) {
                COLORREF v = surf_get(c->image, x, y);
                int transparent = c->mask && (bmp_row(c->mask, y)[x >> 3] & (0x80 >> (x & 7)));
                px[y * c->image->w + x] =
                    transparent ? 0 : 0xff000000u | (uint32_t)(GetRValue(v) << 16 | GetGValue(v) << 8 | GetBValue(v));
            }
        c->host = host_cursor_image(px, c->image->w, c->image->h, c->hx, c->hy);
        free(px);
    }
    if (c->host)
        host_cursor_set(c->host);
}

HMENU WINAPI LoadMenuA(HINSTANCE inst, LPCSTR name)
{ API_TRACE();
    (void)inst; (void)name;
    return 0;           /* civ2.exe has no menu resources */
}
