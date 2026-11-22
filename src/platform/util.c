/*
 * Handles, rectangle regions, logging.
 */
#include "plat.h"
#include "../host/host.h"
#ifdef __SANITIZE_ADDRESS__
#include <sanitizer/common_interface_defs.h>     /* plat_fatal: where from */
#endif

/* ---- handles: small integers, (type << 16) | slot ---- */

#define MAX_OBJ 8192

static struct { ObjType type; void *obj; int stock; } objs[MAX_OBJ];
static int next_free = 1;
static int high = 1;            /* 1 + the highest slot ever used */

Handle obj_new(ObjType t, void *obj)
{
    int i, n;
    for (n = 0; n < MAX_OBJ; n++) {
        i = (next_free + n) % MAX_OBJ;
        if (i && objs[i].type == OBJ_NONE) {
            objs[i].type = t;
            objs[i].obj = obj;
            objs[i].stock = 0;
            next_free = i + 1;
            if (high <= i)
                high = i + 1;
            return ((Handle)t << 16) | (Handle)i;
        }
    }
    plat_fatal("out of handles");
    return 0;
}

static int slot_of(Handle v, ObjType *t)
{
    int i = (int)(v & 0xffff);
    if (!v || (v >> 16) >= OBJ_COUNT || i >= MAX_OBJ || objs[i].type != (ObjType)(v >> 16))
        return 0;
    if (t)
        *t = objs[i].type;
    return i;
}

void *obj_get(Handle h, ObjType t)
{
    ObjType have;
    int i = slot_of(h, &have);
    return i && have == t ? objs[i].obj : NULL;
}

/* The slots below this one are all those ever used. */
int obj_slots(void)
{
    return high;
}

ObjType obj_type(Handle h)
{
    ObjType t;
    return slot_of(h, &t) ? t : OBJ_NONE;
}

void obj_free(Handle h)
{
    int i = slot_of(h, NULL);
    if (i) {
        objs[i].type = OBJ_NONE;
        objs[i].obj = NULL;
    }
}

void obj_set_stock(Handle h)
{
    int i = slot_of(h, NULL);
    if (i)
        objs[i].stock = 1;
}

int obj_is_stock(Handle h)
{
    int i = slot_of(h, NULL);
    return i && objs[i].stock;
}

/* ---- rectangles and regions ---- */

int rect_empty(const RECT *r)
{
    return r->left >= r->right || r->top >= r->bottom;
}

int rect_intersect(RECT *out, const RECT *a, const RECT *b)
{
    RECT r;
    r.left = a->left > b->left ? a->left : b->left;
    r.top = a->top > b->top ? a->top : b->top;
    r.right = a->right < b->right ? a->right : b->right;
    r.bottom = a->bottom < b->bottom ? a->bottom : b->bottom;
    if (rect_empty(&r)) {
        memset(out, 0, sizeof *out);
        return 0;
    }
    *out = r;
    return 1;
}

static void rgn_add(Region *g, const RECT *r)
{
    if (rect_empty(r))
        return;
    if (g->n == g->cap) {
        g->cap = g->cap ? g->cap * 2 : 8;
        g->r = realloc(g->r, (size_t)g->cap * sizeof *g->r);
    }
    g->r[g->n++] = *r;
}

void rgn_clear(Region *g) { g->n = 0; }
void rgn_free(Region *g) { free(g->r); memset(g, 0, sizeof *g); }
int rgn_empty(const Region *g) { return g->n == 0; }

void rgn_set(Region *g, const RECT *r)
{
    g->n = 0;
    rgn_add(g, r);
}

void rgn_copy(Region *dst, const Region *src)
{
    int i;
    dst->n = 0;
    for (i = 0; i < src->n; i++)
        rgn_add(dst, &src->r[i]);
}

void rgn_intersect_rect(Region *g, const RECT *r)
{
    int i, n = 0;
    for (i = 0; i < g->n; i++)
        if (rect_intersect(&g->r[n], &g->r[i], r))
            n++;
    g->n = n;
}

/* Rectangles stay disjoint: each is cut into up to four pieces around r. */
void rgn_subtract_rect(Region *g, const RECT *r)
{
    Region out = {0, 0, NULL};
    int i;
    for (i = 0; i < g->n; i++) {
        RECT a = g->r[i], x, piece;
        if (!rect_intersect(&x, &a, r)) {
            rgn_add(&out, &a);
            continue;
        }
        piece = a; piece.bottom = x.top; rgn_add(&out, &piece);                 /* above */
        piece = a; piece.top = x.bottom; rgn_add(&out, &piece);                 /* below */
        piece = x; piece.left = a.left; piece.right = x.left; rgn_add(&out, &piece);    /* left */
        piece = x; piece.left = x.right; piece.right = a.right; rgn_add(&out, &piece);  /* right */
    }
    free(g->r);
    *g = out;
}

void rgn_union_rect(Region *g, const RECT *r)
{
    rgn_subtract_rect(g, r);
    rgn_add(g, r);
}

void rgn_offset(Region *g, int dx, int dy)
{
    int i;
    for (i = 0; i < g->n; i++) {
        g->r[i].left += dx; g->r[i].right += dx;
        g->r[i].top += dy; g->r[i].bottom += dy;
    }
}

void rgn_bounds(const Region *g, RECT *out)
{
    int i;
    memset(out, 0, sizeof *out);
    for (i = 0; i < g->n; i++) {
        if (!i) {
            *out = g->r[0];
            continue;
        }
        if (g->r[i].left < out->left) out->left = g->r[i].left;
        if (g->r[i].top < out->top) out->top = g->r[i].top;
        if (g->r[i].right > out->right) out->right = g->r[i].right;
        if (g->r[i].bottom > out->bottom) out->bottom = g->r[i].bottom;
    }
}

/* ---- diagnostics ---- */

/* CIV2_APITRACE=FROM-TO: log the Windows API calls made while the game
 * retrieves messages FROM to TO (as hook/apitrace.c does for the original). */
extern int msg_count;
static int api_from = -2, api_to = -1;

void plat_text_begin(void);

void api_trace(const char *name, void *caller)
{
    if (caller && (const char *)caller >= (const char *)plat_text_begin)
        return;                 /* the platform's own call */
    if (api_from == -2) {
        const char *s = getenv("CIV2_APITRACE");
        api_from = -1;
        if (s)
            sscanf(s, "%d-%d", &api_from, &api_to);
    }
    if (api_from >= 0 && msg_count >= api_from && msg_count <= api_to)
        plat_log("api %s @%llu\n", name, vclock_us);
}

static FILE *log_file;

void plat_log(const char *fmt, ...)
{
    va_list ap;
    if (!log_file) {
        const char *p = getenv("CIV2_HOOK_LOG");
        log_file = p && *p ? fopen(p, "w") : stderr;
        if (!log_file)
            log_file = stderr;
    }
    va_start(ap, fmt);
    vfprintf(log_file, fmt, ap);
    va_end(ap);
    fflush(log_file);
}

/* A request no working program makes (a window 700000 pixels tall, a
 * gigabyte allocation): logged as SUSPICIOUS, where test runs look for it;
 * with CIV2_STRICT set the program stops there (and dumps core). */
void plat_suspicious(const char *fmt, ...)
{
    static int strict = -1;
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    plat_log("SUSPICIOUS: %s\n", buf);
    fprintf(stderr, "civ2: SUSPICIOUS: %s\n", buf);
    if (strict < 0)
        strict = getenv("CIV2_STRICT") && *getenv("CIV2_STRICT");
    if (strict)
        abort();
}

/* A coordinate or size a window can have (CW_USEDEFAULT aside). */
int plat_sane_extent(int v)
{
    return v == (int)0x80000000 || (v >= -32768 && v <= 32767);
}

void plat_fatal(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    plat_log("fatal: %s\n", buf);
    fprintf(stderr, "civ2: %s\n", buf);
#ifdef __SANITIZE_ADDRESS__
    __sanitizer_print_stack_trace();
#endif
    if (!getenv("CIV2_SCRIPT")) {
        static const char *const ok[1] = {"OK"};
        static const int id[1] = {1};
        host_message_box(2, "Civilization II", buf, 1, ok, id, 0, 0);
    }
    exit(1);
}

char *plat_strdup(const char *s)
{
    size_t n = strlen(s ? s : "") + 1;
    char *p = malloc(n);
    memcpy(p, s ? s : "", n);
    return p;
}

/* ---- rectangle functions ---- */

WINBOOL WINAPI SetRect(LPRECT r, int l, int t, int rt, int b)
{
    API_TRACE();
    if (!r)
        return FALSE;
    r->left = l; r->top = t; r->right = rt; r->bottom = b;
    return TRUE;
}

WINBOOL WINAPI CopyRect(LPRECT d, const RECT *s)
{
    API_TRACE();
    if (!d || !s)
        return FALSE;
    *d = *s;
    return TRUE;
}

WINBOOL WINAPI OffsetRect(LPRECT r, int dx, int dy)
{
    API_TRACE();
    if (!r)
        return FALSE;
    r->left += dx; r->right += dx; r->top += dy; r->bottom += dy;
    return TRUE;
}

WINBOOL WINAPI InflateRect(LPRECT r, int dx, int dy)
{
    API_TRACE();
    if (!r)
        return FALSE;
    r->left -= dx; r->right += dx; r->top -= dy; r->bottom += dy;
    return TRUE;
}

WINBOOL WINAPI IntersectRect(LPRECT d, const RECT *a, const RECT *b)
{
    API_TRACE();
    return rect_intersect(d, a, b);
}

WINBOOL WINAPI UnionRect(LPRECT d, const RECT *a, const RECT *b)
{
    API_TRACE();
    if (rect_empty(a)) {
        if (rect_empty(b)) {
            memset(d, 0, sizeof *d);
            return FALSE;
        }
        *d = *b;
        return TRUE;
    }
    if (rect_empty(b)) {
        *d = *a;
        return TRUE;
    }
    d->left = a->left < b->left ? a->left : b->left;
    d->top = a->top < b->top ? a->top : b->top;
    d->right = a->right > b->right ? a->right : b->right;
    d->bottom = a->bottom > b->bottom ? a->bottom : b->bottom;
    return TRUE;
}

WINBOOL WINAPI PtInRect(const RECT *r, POINT p)
{
    API_TRACE();
    return p.x >= r->left && p.x < r->right && p.y >= r->top && p.y < r->bottom;
}

HWND WINAPI GetCapture(void)
{
    API_TRACE();
    return capture_wnd;
}

/* a * b / c rounded half away from zero; -1 when c is 0 (as Windows). */
int WINAPI MulDiv(int a, int b, int c)
{
    long long p;
    API_TRACE();
    if (!c)
        return -1;
    p = (long long)a * b;
    if ((p < 0) != (c < 0))
        return (int)((p - c / 2) / c);
    return (int)((p + c / 2) / c);
}
