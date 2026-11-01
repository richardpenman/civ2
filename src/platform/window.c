/*
 * Windows: classes, creation, geometry, Z order, visibility, focus, window
 * data, painting (update regions), scroll bar state.
 *
 * As in the reference environment (Wine on an X server without a window
 * manager), top-level windows have their non-client area reserved but not
 * drawn, except the menu bar.
 */
#include "plat.h"

static Window desktop_wnd;
Window *desktop = &desktop_wnd;
static WndClass *classes;
HWND focus_wnd, capture_wnd, active_wnd;
unsigned wnd_generation = 1;            /* bumped on any geometry/Z-order/visibility change */

static int is_managed(Window *w, int activating);
static void nc_dirty_tree(Window *w);
static WINBOOL swp(Window *w, HWND after, int x, int y, int cx, int cy, UINT flags);
static void activate(Window *w);
static void activate_other(Window *gone);

/* GetSystemMetrics, as Wine reports them on the reference desktop. */
static const int sys_metrics[96] = {
    1024, 768, 17, 17, 19, 1, 1, 3, 3, 17, 17, 32, 32, 32, 32, 19, 1024, 779, 0, 1, 17, 17, 0, 0, 0, 0, 0, 0,
    116, 27, 18, 18, 4, 4, 116, 27, 4, 4, 75, 75, 0, 0, 0, 3, 0, 2, 2, 160, 24, 16, 16, 16, 15, 15, 18, 18, 8,
    160, 24, 1036, 780, 1032, 806, 3, 0, 0, 0, 0, 4, 4, 0, 13, 13, 0, 0, 1, 0, 0, 1024, 768, 1, 1, 0, 1, 1, 0,
    0, 0, 0, 0, 90, 0, 0, 0, 0, 0
};

int WINAPI GetSystemMetrics(int i)
{
    API_TRACE();
    if (i == SM_CXSCREEN || i == SM_CXFULLSCREEN || i == SM_CXVIRTUALSCREEN)
        return screen_w;
    if (i == SM_CYSCREEN || i == SM_CYVIRTUALSCREEN)
        return screen_h;
    /* the reference's values are for a 1024 x 768 desktop */
    if (i == SM_CYFULLSCREEN || i == SM_CYMAXTRACK || i == SM_CYMAXIMIZED)
        return sys_metrics[i] + screen_h - 768;
    if (i == SM_CXMAXTRACK || i == SM_CXMAXIMIZED)
        return sys_metrics[i] + screen_w - 1024;
    return i >= 0 && i < 96 ? sys_metrics[i] : 0;
}

Window *wnd_get(HWND h)
{
    Window *w = obj_get(h, OBJ_WINDOW);
    return w && !w->destroyed ? w : NULL;
}

/* ---- classes ---- */

static WndClass *find_class(const char *name)
{
    WndClass *c;
    for (c = classes; c; c = c->next)
        if (strcasecmp(c->name, name) == 0)
            return c;
    return NULL;
}

static void add_builtin(const char *name, WNDPROC proc, int extra, UINT style)
{
    WndClass *c = calloc(1, sizeof *c);
    snprintf(c->name, sizeof c->name, "%s", name);
    c->wc.lpfnWndProc = proc;
    c->wc.cbWndExtra = extra;
    c->wc.style = style;
    c->wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    c->builtin = 1;
    c->next = classes;
    classes = c;
}

void wnd_init(void)
{
    desktop->hwnd = obj_new(OBJ_WINDOW, desktop);
    desktop->style = WS_VISIBLE | WS_CLIPCHILDREN;
    desktop->rect.right = screen_w;
    desktop->rect.bottom = screen_h;
    desktop->client = desktop->rect;
    /* cbWndExtra as Wine's builtin classes report them (GetClassLong). */
    add_builtin("EDIT", ctl_edit_proc, 6, CS_DBLCLKS | CS_PARENTDC);
    add_builtin("LISTBOX", ctl_listbox_proc, 4, CS_DBLCLKS | CS_PARENTDC);
    add_builtin("COMBOBOX", ctl_combobox_proc, 4, CS_DBLCLKS | CS_PARENTDC | CS_HREDRAW | CS_VREDRAW);
    add_builtin("SCROLLBAR", ctl_scrollbar_proc, 4, CS_DBLCLKS | CS_PARENTDC | CS_HREDRAW | CS_VREDRAW);
    add_builtin("BUTTON", ctl_button_proc, 4, CS_DBLCLKS | CS_PARENTDC | CS_HREDRAW | CS_VREDRAW);
    add_builtin("STATIC", ctl_static_proc, 4, CS_DBLCLKS | CS_PARENTDC);
}

ATOM WINAPI RegisterClassA(const WNDCLASSA *wc)
{
    API_TRACE();
    WndClass *c;
    if (find_class(wc->lpszClassName) && !find_class(wc->lpszClassName)->builtin)
        return 0;
    c = calloc(1, sizeof *c);
    snprintf(c->name, sizeof c->name, "%s", wc->lpszClassName);
    c->wc = *wc;
    c->wc.lpszClassName = c->name;
    c->extra = calloc((size_t)(wc->cbClsExtra / 4 + 1), sizeof(LONG_PTR));
    c->next = classes;
    classes = c;
    return (ATOM)(0xc000 + ((uintptr_t)c & 0xfff));
}

WINBOOL WINAPI UnregisterClassA(LPCSTR name, HINSTANCE inst)
{
    API_TRACE();
    WndClass **p;
    (void)inst;
    for (p = &classes; *p; p = &(*p)->next)
        if (!(*p)->builtin && strcasecmp((*p)->name, name) == 0) {
            *p = (*p)->next;           /* windows of the class keep their pointer: leaked deliberately */
            return TRUE;
        }
    return FALSE;
}

ULONG_PTR WINAPI GetClassLongA(HWND h, int i)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w || !w->cls)
        return 0;
    switch (i) {
    case GCL_CBWNDEXTRA: return (ULONG_PTR)w->cls->wc.cbWndExtra;
    case GCL_CBCLSEXTRA: return (ULONG_PTR)w->cls->wc.cbClsExtra;
    case GCL_STYLE: return w->cls->wc.style;
    case GCL_WNDPROC: return (ULONG_PTR)w->cls->wc.lpfnWndProc;
    case GCL_HCURSOR: return (ULONG_PTR)w->cls->wc.hCursor;
    case GCL_HICON: return (ULONG_PTR)w->cls->wc.hIcon;
    case GCL_HBRBACKGROUND: return (ULONG_PTR)w->cls->wc.hbrBackground;
    case GCL_HMODULE: return (ULONG_PTR)w->cls->wc.hInstance;
    }
    return i >= 0 && w->cls->extra ? (ULONG_PTR)w->cls->extra[i / 4] : 0;
}

ULONG_PTR WINAPI SetClassLongA(HWND h, int i, LONG_PTR v)
{
    API_TRACE();
    Window *w = wnd_get(h);
    ULONG_PTR old = GetClassLongA(h, i);
    if (!w || !w->cls)
        return 0;
    switch (i) {
    case GCL_STYLE: w->cls->wc.style = (UINT)v; break;
    case GCL_WNDPROC: w->cls->wc.lpfnWndProc = (WNDPROC)v; break;
    case GCL_HCURSOR: w->cls->wc.hCursor = (HCURSOR)v; break;
    case GCL_HICON: w->cls->wc.hIcon = (HICON)v; break;
    case GCL_HBRBACKGROUND: w->cls->wc.hbrBackground = (HBRUSH)v; break;
    default:
        if (i >= 0 && w->cls->extra)
            w->cls->extra[i / 4] = v;
    }
    return old;
}

int WINAPI GetClassNameA(HWND h, LPSTR buf, int n)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w || !w->cls || n <= 0)
        return 0;
    snprintf(buf, (size_t)n, "%s", w->cls->name);
    return (int)strlen(buf);
}

/* ---- geometry ---- */

/* Non-client insets for a style: left, top, right, bottom. */
static void nc_insets(Window *w, RECT *in)
{
    int b = 0;
    memset(in, 0, sizeof *in);
    if (w->style & WS_THICKFRAME)
        b = GetSystemMetrics(SM_CXFRAME);
    else if ((w->style & WS_DLGFRAME) || (w->exstyle & WS_EX_DLGMODALFRAME))
        b = GetSystemMetrics(SM_CXDLGFRAME);
    else if (w->style & WS_BORDER)
        b = GetSystemMetrics(SM_CXBORDER);
    if (w->exstyle & WS_EX_CLIENTEDGE)
        b += GetSystemMetrics(SM_CXEDGE);
    in->left = in->right = in->top = in->bottom = b;
    if ((w->style & WS_CAPTION) == WS_CAPTION)
        in->top += GetSystemMetrics(SM_CYCAPTION);
    if (!(w->style & WS_CHILD) && w->menu)
        in->top += menu_bar_height(w);
    if (w->style & WS_VSCROLL)
        in->right += GetSystemMetrics(SM_CXVSCROLL);
    if (w->style & WS_HSCROLL)
        in->bottom += GetSystemMetrics(SM_CYHSCROLL);
}

static void calc_client(Window *w)
{
    RECT in;
    nc_insets(w, &in);
    w->client.left = in.left;
    w->client.top = in.top;
    w->client.right = (w->rect.right - w->rect.left) - in.right;
    w->client.bottom = (w->rect.bottom - w->rect.top) - in.bottom;
    if (w->client.right < w->client.left) w->client.right = w->client.left;
    if (w->client.bottom < w->client.top) w->client.bottom = w->client.top;
}

/* Origin of w's client area in screen coordinates (as Windows sees them;
 * the desktop shows a host-clipped window displaced, see wnd_shown_rect). */
void wnd_client_screen(Window *w, POINT *o)
{
    o->x = o->y = 0;
    for (; w && w != desktop; w = w->parent) {
        o->x += w->rect.left + w->client.left;
        o->y += w->rect.top + w->client.top;
    }
}

Window *wnd_top(Window *w)
{
    while (w && w->parent && w->parent != desktop)
        w = w->parent;
    return w;
}

void wnd_screen_rect(Window *w, RECT *r)
{
    POINT o = {0, 0};
    if (w->parent && w->parent != desktop)
        wnd_client_screen(w->parent, &o);
    r->left = w->rect.left + o.x;
    r->top = w->rect.top + o.y;
    r->right = w->rect.right + o.x;
    r->bottom = w->rect.bottom + o.y;
}

int wnd_is_visible(Window *w)
{
    for (; w && w != desktop; w = w->parent)
        if (!(w->style & WS_VISIBLE) || w->minimized)
            return 0;
    return w == desktop;
}

/* The desktop area a top-level window covers.  On the reference desktop
 * (no window manager), Wine leaves the decorations -- frame and caption --
 * of the windows a manager would manage to the manager, so they cover
 * nothing; a maximized window stays in its old X window (host_clip). */
void wnd_shown_rect(Window *w, RECT *r)
{
    if (w->host_clipped) {
        *r = w->host_clip;
        return;
    }
    wnd_screen_rect(w, r);
    if (w->managed) {
        int f = (w->style & WS_THICKFRAME) ? GetSystemMetrics(SM_CXFRAME) :
                ((w->style & WS_DLGFRAME) || (w->exstyle & WS_EX_DLGMODALFRAME)) ? GetSystemMetrics(SM_CXDLGFRAME) :
                (w->style & WS_BORDER) ? GetSystemMetrics(SM_CXBORDER) : 0;
        r->left += f;
        r->right -= f;
        r->top += f + (((w->style & WS_CAPTION) == WS_CAPTION) ? GetSystemMetrics(SM_CYCAPTION) : 0);
        r->bottom -= f;
    }
}

/* After the maximize, any SetWindowPos on the window -- moves, Z order
 * changes on activation, frame changes (DrawMenuBar, SetMenu) -- makes Wine
 * move its X window to where the window really is (x11drv
 * sync_window_position), keeping its size, since resizing a maximized
 * managed window is left to the absent window manager.  The X server then
 * reports the X window's geometry (ConfigureNotify), which Wine applies to
 * the window when it next processes events: same position, the X window's
 * (old) size. */
void wnd_host_sync(Window *w)
{
    if (!w->host_clipped || w->host_frozen)
        return;
    if (!w->host_pending)
        plat_log("host sync requested for %x\n", (unsigned)w->hwnd);
    w->host_pending = 1;
}

void wnd_host_events(void)
{
    Window *w;
    for (w = desktop->children; w; w = w->next) {
        RECT shown, win;
        int cx, cy;
        if (!w->host_pending)
            continue;
        w->host_pending = 0;
        plat_log("host sync applied for %x\n", (unsigned)w->hwnd);
        /* The window is its X window plus the decorations around it. */
        w->host_clipped = 0;
        wnd_shown_rect(w, &shown);
        wnd_screen_rect(w, &win);
        cx = (w->host_clip.right - w->host_clip.left) + (shown.left - win.left) + (win.right - shown.right);
        cy = (w->host_clip.bottom - w->host_clip.top) + (shown.top - win.top) + (win.bottom - shown.bottom);
        w->host_clipped = 0;
        w->host_dx = w->host_dy = 0;
        wnd_generation++;
        swp(w, 0, w->rect.left, w->rect.top, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
        plat_log("host sync: %dx%d -> rect %d,%d,%d,%d client %dx%d\n", cx, cy, (int)w->rect.left, (int)w->rect.top,
                 (int)w->rect.right, (int)w->rect.bottom, (int)(w->client.right - w->client.left),
                 (int)(w->client.bottom - w->client.top));
    }
}

/* What of w (client area, or whole window) is drawn, in screen
 * coordinates: clipped by its ancestors' client areas, minus siblings above
 * it (and above each ancestor) for those with WS_CLIPSIBLINGS -- without it a
 * child draws over its siblings, as in Wine's get_visible_region -- minus
 * its children when it clips them.
 * Other top-level windows do not clip it: each top-level window draws into
 * a surface of its own, as Wine's do, and the desktop shows them composed. */
void wnd_visible_region(Window *w, int client, Region *out)
{
    RECT r, c;
    Window *a, *s;
    rgn_clear(out);
    if (!wnd_is_visible(w))
        return;
    wnd_screen_rect(w, &r);
    if (client) {
        POINT o;
        wnd_client_screen(w, &o);
        r.left = o.x; r.top = o.y;
        r.right = o.x + (w->client.right - w->client.left);
        r.bottom = o.y + (w->client.bottom - w->client.top);
    }
    rgn_set(out, &r);
    for (a = w; a && a != desktop; a = a->parent) {
        Window *p = a->parent ? a->parent : desktop;
        if (p == desktop) {
            wnd_screen_rect(a, &c);
            rgn_intersect_rect(out, &c);
            break;
        }
        {
            POINT o;
            wnd_client_screen(p, &o);
            c.left = o.x; c.top = o.y;
            c.right = o.x + (p->client.right - p->client.left);
            c.bottom = o.y + (p->client.bottom - p->client.top);
            rgn_intersect_rect(out, &c);
        }
        for (s = (a->style & WS_CLIPSIBLINGS) ? p->children : NULL; s && s != a; s = s->next)
            if ((s->style & WS_VISIBLE) && !s->minimized) {
                RECT sr;
                wnd_screen_rect(s, &sr);
                rgn_subtract_rect(out, &sr);
            }
    }
    if (client && (w->style & WS_CLIPCHILDREN))
        for (s = w->children; s; s = s->next)
            if ((s->style & WS_VISIBLE) && !s->minimized) {
                RECT sr;
                wnd_screen_rect(s, &sr);
                rgn_subtract_rect(out, &sr);
            }
}

/* ---- Z order ---- */

static void unlink_wnd(Window *w)
{
    Window **p;
    if (!w->parent)
        return;
    for (p = &w->parent->children; *p; p = &(*p)->next)
        if (*p == w) {
            *p = w->next;
            break;
        }
    w->next = NULL;
}

static void link_top(Window *w)
{
    w->next = w->parent->children;
    w->parent->children = w;
}

static void link_bottom(Window *w)
{
    Window **p = &w->parent->children;
    while (*p)
        p = &(*p)->next;
    *p = w;
    w->next = NULL;
}

/* Raising a top-level window raises the windows it owns above it. */
static void raise_wnd(Window *w)
{
    Window *s, *next;
    unlink_wnd(w);
    link_top(w);
    if (w->parent == desktop)
        for (s = desktop->children; s; s = next) {
            next = s->next;
            if (s->owner == w && s != w) {
                unlink_wnd(s);
                link_top(s);
            }
        }
    wnd_generation++;
}

/* ---- invalidation ---- */

/* The part of w's client area (client coordinates) that its ancestors'
 * client areas leave: Wine invalidates nothing outside it. */
static void ancestors_clip(Window *w, RECT *cr)
{
    Window *a;
    POINT o, ao;
    wnd_client_screen(w, &o);
    for (a = w->parent; a && a != desktop; a = a->parent) {
        RECT c;
        wnd_client_screen(a, &ao);
        c.left = ao.x - o.x; c.top = ao.y - o.y;
        c.right = c.left + (a->client.right - a->client.left);
        c.bottom = c.top + (a->client.bottom - a->client.top);
        if (!IntersectRect(cr, cr, &c))
            return;
    }
}

static void invalidate_client_(Window *w, const RECT *r, int erase, int clip_children);

static void invalidate_client(Window *w, const RECT *r, int erase)
{
    invalidate_client_(w, r, erase, 1);
}

static void invalidate_client_(Window *w, const RECT *r, int erase, int clip_children)
{
    RECT cr = {0, 0, 0, 0}, x;
    cr.right = w->client.right - w->client.left;
    cr.bottom = w->client.bottom - w->client.top;
    if (w->style & WS_CHILD)
        ancestors_clip(w, &cr);
    if (r) {
        if (!rect_intersect(&x, r, &cr))
            return;
    } else {
        x = cr;
    }
    if (rect_empty(&x))
        return;
    if ((w->style & WS_CLIPCHILDREN) && clip_children) {
        /* The update region of a window that clips its children leaves out
         * what its visible children cover. */
        Region add = {0, 0, NULL};
        Window *c;
        int i;
        rgn_set(&add, &x);
        for (c = w->children; c; c = c->next)
            if ((c->style & WS_VISIBLE) && !c->minimized)
                rgn_subtract_rect(&add, &c->rect);
        for (i = 0; i < add.n; i++)
            rgn_union_rect(&w->update, &add.r[i]);
        if (add.n && erase)
            w->erase |= 1;
        rgn_free(&add);
        return;
    }
    rgn_union_rect(&w->update, &x);
    if (erase)
        w->erase |= 1;
}

/* Something covering area r (screen coordinates) of top-level window top's
 * surface went away -- a child window hidden or moved: repaint what shows
 * there.  Other top-level windows are unaffected: they have surfaces of
 * their own. */
static void expose_tree(Window *w, const RECT *r, int is_top)
{
    RECT sr, x, cl, xc;
    POINT o;
    Window *c;
    if (!wnd_is_visible(w))
        return;
    wnd_screen_rect(w, &sr);
    if (!rect_intersect(&x, &sr, r))
        return;
    wnd_client_screen(w, &o);
    cl.left = o.x; cl.top = o.y;
    cl.right = o.x + (w->client.right - w->client.left);
    cl.bottom = o.y + (w->client.bottom - w->client.top);
    /* children's frames are always invalidated (Wine); the window's own
       only if the area reaches it */
    if (!is_top || !rect_intersect(&xc, &x, &cl) || memcmp(&xc, &x, sizeof x))
        w->erase |= 2;
    if (!rect_intersect(&xc, &x, &cl))
        return;
    OffsetRect(&xc, -o.x, -o.y);
    /* where the exposure starts, the update region takes in what the
       children cover (painted, if not erased, there) */
    invalidate_client_(w, &xc, 1, !is_top);
    OffsetRect(&xc, o.x, o.y);
    for (c = w->children; c; c = c->next)
        expose_tree(c, &xc, 0);
}

void wnd_expose(Window *top, const RECT *r)
{
    if (!top || rect_empty(r))
        return;
    expose_tree(top, r, 1);
}

/* wnd_expose for region rg (screen coordinates) less region minus. */
static void expose_region(Window *top, const Region *rg, const Region *minus)
{
    Region t = {0, 0, NULL};
    int i;
    rgn_copy(&t, rg);
    if (minus)
        for (i = 0; i < minus->n; i++)
            rgn_subtract_rect(&t, &minus->r[i]);
    for (i = 0; i < t.n; i++)
        wnd_expose(top, &t.r[i]);
    rgn_free(&t);
}

/* The surface of top-level window w, the size of its window rect (kept
 * across resizes, as Wine copies the bits that remain valid). */
Bitmap *wnd_surface(Window *w)
{
    int cx = w->rect.right - w->rect.left, cy = w->rect.bottom - w->rect.top;
    if (cx < 1) cx = 1;
    if (cy < 1) cy = 1;
    if (!w->surf || w->surf->w != cx || w->surf->h != cy) {
        Bitmap *n = bmp_create(cx, cy, 32, 0);
        if (w->surf) {
            int y, rows = n->h < w->surf->h ? n->h : w->surf->h, cols = n->w < w->surf->w ? n->w : w->surf->w;
            int x0 = 0;
            if (plat_normal_desktop) {
                /* only the old client area is still valid (the frame is
                   drawn anew); the rest waits for painting */
                RECT k;
                if (!rect_intersect(&k, &w->surf_client, &w->client))
                    memset(&k, 0, sizeof k);
                x0 = k.left;
                cols = k.right < cols ? k.right - x0 : cols - x0;
                for (y = k.top; y < k.bottom && y < rows; y++)
                    if (cols > 0)
                        memcpy((uint32_t *)bmp_row(n, y) + x0, (uint32_t *)bmp_row(w->surf, y) + x0, (size_t)cols * 4);
            } else
                for (y = 0; y < rows; y++)
                    memcpy(bmp_row(n, y), bmp_row(w->surf, y), (size_t)cols * 4);
            bmp_destroy(w->surf);
        }
        w->surf = n;
    }
    w->surf_client = w->client;
    return w->surf;
}

/* InvalidateRect(NULL, ...): every top-level window. */
void wnd_invalidate_desktop(const RECT *r)
{
    Window *w;
    for (w = desktop->children; w; w = w->next)
        wnd_expose(w, r);
}

WINBOOL WINAPI InvalidateRect(HWND h, const RECT *r, WINBOOL erase)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (getenv("CIV2_LOG_NC")) {
        if (r)
            plat_log("InvalidateRect(%x, %d,%d,%d,%d, %d)\n", (unsigned)h, (int)r->left, (int)r->top, (int)r->right,
                     (int)r->bottom, erase);
        else
            plat_log("InvalidateRect(%x, NULL, %d)\n", (unsigned)h, erase);
    }
    if (!h) {
        RECT all = {0, 0, 0, 0};
        all.right = screen_w; all.bottom = screen_h;
        wnd_invalidate_desktop(r ? r : &all);
        return TRUE;
    }
    if (!w)
        return FALSE;
    invalidate_client(w, r, erase);
    /* Children without WS_CLIPCHILDREN on the parent are painted over: they repaint too. */
    if (!(w->style & WS_CLIPCHILDREN)) {
        Window *c;
        for (c = w->children; c; c = c->next) {
            RECT x, cr = c->rect;
            if (!r || rect_intersect(&x, &cr, r)) {
                RECT local;
                if (r) {
                    POINT o;
                    local = x;
                    o.x = c->rect.left + c->client.left;
                    o.y = c->rect.top + c->client.top;
                    local.left -= o.x; local.right -= o.x; local.top -= o.y; local.bottom -= o.y;
                }
                InvalidateRect(c->hwnd, r ? &local : NULL, erase);
            }
        }
    }
    return TRUE;
}

WINBOOL WINAPI ValidateRect(HWND h, const RECT *r)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (getenv("CIV2_LOG_NC"))
        plat_log("ValidateRect(%x)\n", (unsigned)h);
    if (!w)
        return FALSE;
    if (r)
        rgn_subtract_rect(&w->update, r);
    else
        rgn_clear(&w->update);
    if (rgn_empty(&w->update))
        w->erase &= ~1;
    return TRUE;
}

/* The next window needing WM_PAINT (parents before children, top first). */
Window *wnd_next_dirty(Window *root)
{
    Window *c, *d;
    if (root != desktop && !wnd_is_visible(root))
        return NULL;
    if (root != desktop && (!rgn_empty(&root->update) || (root->erase & 2)))
        return root;
    for (c = root->children; c; c = c->next)
        if ((d = wnd_next_dirty(c)))
            return d;
    return NULL;
}

WINBOOL WINAPI UpdateWindow(HWND h)
{
    API_TRACE();
    Window *w = wnd_get(h);
    Window *d;
    if (getenv("CIV2_LOG_NC"))
        plat_log("UpdateWindow(%x) update %d\n", (unsigned)h, w ? !rgn_empty(&w->update) : -1);
    if (!w)
        return FALSE;
    while ((d = wnd_next_dirty(w))) {
        if (rgn_empty(&d->update)) {
            d->erase &= ~2;         /* only the non-client area needs painting */
            wnd_send(d, WM_NCPAINT, 1, 0);
            continue;
        }
        wnd_send(d, WM_PAINT, 0, 0);
        if (wnd_get(d->hwnd) && !rgn_empty(&d->update)) {
            rgn_clear(&d->update);  /* not validated: do not loop */
            d->erase &= ~1;
        }
    }
    return TRUE;
}

/* ---- creation and destruction ---- */

static LONG_PTR next_ctl_id;

HWND WINAPI CreateWindowExA(DWORD exstyle, LPCSTR cls, LPCSTR title, DWORD style, int x, int y, int cx, int cy,
                            HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
{
    API_TRACE();
    WndClass *c = IS_INTRESOURCE(cls) ? NULL : find_class(cls);
    Window *w, *p = wnd_get(parent);
    CREATESTRUCTA cs;
    MINMAXINFO mmi;
    (void)next_ctl_id;
    if (!c) {
        plat_log("CreateWindowEx: unknown class %s\n", IS_INTRESOURCE(cls) ? "(atom)" : cls);
        return NULL;
    }
    if (!plat_sane_extent(x) || !plat_sane_extent(y) || !plat_sane_extent(cx) || !plat_sane_extent(cy) ||
        (cx < 0 && cx != (int)0x80000000) || (cy < 0 && cy != (int)0x80000000))
        plat_suspicious("CreateWindowEx(%s) at %d,%d size %dx%d", c->name, x, y, cx, cy);
    plat_log("CreateWindowEx(%x, %s, \"%s\", %08x, %d, %d, %d, %d, parent %x)\n", (unsigned)exstyle, c->name,
             title ? title : "", (unsigned)style, x, y, cx, cy, (unsigned)parent);
    w = calloc(1, sizeof *w);
    w->hwnd = obj_new(OBJ_WINDOW, w);
    msg_trace_window_created(w->hwnd, c->name);
    w->cls = c;
    w->proc = c->wc.lpfnWndProc;
    w->text = plat_strdup(title ? title : "");
    w->style = style;
    w->exstyle = exstyle;
    w->menu = menu;
    w->instance = inst;
    w->nextra = c->wc.cbWndExtra / 4 + 1;
    w->extra = calloc((size_t)w->nextra, sizeof(LONG_PTR));
    if (style & WS_CHILD) {
        w->parent = p ? p : desktop;
    } else {
        w->parent = desktop;
        while (p && (p->style & WS_CHILD))
            p = p->parent;
        w->owner = p;
        if (!(style & WS_POPUP))
            w->style |= WS_CLIPSIBLINGS | WS_CAPTION;    /* overlapped windows always have a caption */
    }
    if (x == CW_USEDEFAULT) {
        x = y = 0;
        if (!(style & WS_CHILD) && !(style & WS_POPUP)) {
            x = y = 0;
        }
    }
    if (cx == CW_USEDEFAULT) {
        cx = screen_w * 3 / 4;
        cy = screen_h * 3 / 4;
    }
    /* WM_GETMINMAXINFO limits the initial size of resizable windows. */
    if ((style & WS_THICKFRAME) || !(style & (WS_CHILD | WS_POPUP))) {
        memset(&mmi, 0, sizeof mmi);
        mmi.ptMaxSize.x = screen_w + 8; mmi.ptMaxSize.y = screen_h + 8;
        mmi.ptMaxPosition.x = -4; mmi.ptMaxPosition.y = -4;
        mmi.ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
        mmi.ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
        mmi.ptMaxTrackSize.x = GetSystemMetrics(SM_CXMAXTRACK);
        mmi.ptMaxTrackSize.y = GetSystemMetrics(SM_CYMAXTRACK);
        wnd_send(w, WM_GETMINMAXINFO, 0, (LPARAM)&mmi);
        if (cx > mmi.ptMaxTrackSize.x) cx = mmi.ptMaxTrackSize.x;
        if (cy > mmi.ptMaxTrackSize.y) cy = mmi.ptMaxTrackSize.y;
        if (cx < mmi.ptMinTrackSize.x) cx = mmi.ptMinTrackSize.x;
        if (cy < mmi.ptMinTrackSize.y) cy = mmi.ptMinTrackSize.y;
    }
    w->rect.left = x;
    w->rect.top = y;
    w->rect.right = x + (cx < 0 ? 0 : cx);
    w->rect.bottom = y + (cy < 0 ? 0 : cy);
    w->style &= ~WS_VISIBLE;
    /* New child windows go to the bottom of the Z order, top-level ones to the top. */
    if (style & WS_CHILD)
        link_bottom(w);
    else
        link_top(w);
    calc_client(w);
    wnd_generation++;

    memset(&cs, 0, sizeof cs);
    cs.lpCreateParams = param;
    cs.hInstance = inst;
    cs.hMenu = menu;
    cs.hwndParent = parent;
    cs.x = x; cs.y = y; cs.cx = cx; cs.cy = cy;
    cs.style = (LONG)style;
    cs.lpszName = title;
    cs.lpszClass = cls;
    cs.dwExStyle = exstyle;
    if (!wnd_send(w, WM_NCCREATE, 0, (LPARAM)&cs)) {
        DestroyWindow(w->hwnd);
        return NULL;
    }
    {
        NCCALCSIZE_PARAMS ncp;
        memset(&ncp, 0, sizeof ncp);
        wnd_screen_rect(w, &ncp.rgrc[0]);
        wnd_send(w, WM_NCCALCSIZE, FALSE, (LPARAM)&ncp.rgrc[0]);
    }
    calc_client(w);
    if (wnd_send(w, WM_CREATE, 0, (LPARAM)&cs) == -1) {
        DestroyWindow(w->hwnd);
        return NULL;
    }
    if (!wnd_get(w->hwnd))
        return NULL;
    if (!(style & (WS_CHILD | WS_POPUP))) {
        /* overlapped windows get their size messages on the first ShowWindow */
        w->need_size = 1;
    } else {
        wnd_send(w, WM_SIZE, SIZE_RESTORED,
                 MAKELPARAM(w->client.right - w->client.left, w->client.bottom - w->client.top));
        if (!wnd_get(w->hwnd))
            return NULL;
        wnd_send(w, WM_MOVE, 0, MAKELPARAM(w->rect.left + w->client.left, w->rect.top + w->client.top));
    }
    if ((style & WS_CHILD) && !(exstyle & WS_EX_NOPARENTNOTIFY) && w->parent != desktop)
        wnd_send(w->parent, WM_PARENTNOTIFY, MAKEWPARAM(WM_CREATE, (WORD)(uintptr_t)menu), (LPARAM)w->hwnd);
    if (wnd_get(w->hwnd) && (style & WS_VISIBLE))
        ShowWindow(w->hwnd, (style & WS_MAXIMIZE) ? SW_SHOWMAXIMIZED : SW_SHOW);
    return wnd_get(w->hwnd) ? w->hwnd : 0;
}

/* Wine's send_destroy_message: WM_DESTROY to the window, then to its
 * descendants (parents first). */
static void send_destroy(Window *w)
{
    HWND h = w->hwnd, list[256];
    Window *c;
    int n = 0, i;
    if (h == active_wnd)
        activate_other(w);
    if (!wnd_get(h))
        return;
    wnd_send(w, WM_DESTROY, 0, 0);
    if (!wnd_get(h))
        return;
    for (c = w->children; c && n < 256; c = c->next)
        list[n++] = c->hwnd;
    for (i = 0; i < n; i++)
        if (wnd_get(list[i]))
            send_destroy(wnd_get(list[i]));
}

/* Wine's destroy_window: children first, then WM_NCDESTROY and the window
 * goes.  A window losing the focus this way leaves none (no messages). */
static void destroy_storage(Window *w)
{
    HWND h = w->hwnd;
    while (w->children)
        destroy_storage(w->children);
    wnd_send(w, WM_NCDESTROY, 0, 0);
    if (focus_wnd == h)
        focus_wnd = NULL;
    if (capture_wnd == h)
        capture_wnd = NULL;
    if (active_wnd == h)
        active_wnd = NULL;
    unlink_wnd(w);
    w->destroyed = 1;
    wnd_generation++;
    timers_window_destroyed(h);
    msg_window_destroyed(h);
    obj_free(h);
    rgn_free(&w->update);
    /* The Window struct is not freed: DCs may still point at it. */
}

WINBOOL WINAPI DestroyWindow(HWND h)
{
    API_TRACE();
    Window *w = wnd_get(h), *a;
    int is_child;
    if (!w || w == desktop)
        return FALSE;
    is_child = (w->style & WS_CHILD) != 0;
    /* WM_PARENTNOTIFY up the chain of ancestors that want it */
    for (a = w; a && (a->style & WS_CHILD) && !(a->exstyle & WS_EX_NOPARENTNOTIFY) && a->parent && a->parent != desktop;
         a = a->parent)
        wnd_send(a->parent, WM_PARENTNOTIFY, MAKEWPARAM(WM_DESTROY, (WORD)(uintptr_t)w->menu), (LPARAM)h);
    if (!wnd_get(h))
        return TRUE;
    if (w->style & WS_VISIBLE) {
        /* only child windows get WM_SHOWWINDOW */
        if (is_child)
            ShowWindow(h, SW_HIDE);
        else
            swp(w, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
    }
    if (!wnd_get(h))
        return TRUE;
    /* Owned windows go first. */
    if (!is_child) {
        Window *s, *next;
        for (s = desktop->children; s; s = next) {
            next = s->next;
            if (s->owner == w)
                DestroyWindow(s->hwnd);
        }
    }
    send_destroy(w);
    if (!wnd_get(h))
        return TRUE;
    destroy_storage(w);
    return TRUE;
}

WINBOOL WINAPI IsWindow(HWND h) { API_TRACE(); return wnd_get(h) != NULL; }
WINBOOL WINAPI IsWindowVisible(HWND h) { API_TRACE(); Window *w = wnd_get(h); return w && wnd_is_visible(w); }
WINBOOL WINAPI IsIconic(HWND h) { API_TRACE(); Window *w = wnd_get(h); return w && w->minimized; }
WINBOOL WINAPI IsZoomed(HWND h) { API_TRACE(); Window *w = wnd_get(h); return w && w->maximized; }

WINBOOL WINAPI IsChild(HWND parent, HWND h)
{
    API_TRACE();
    Window *w = wnd_get(h);
    for (; w && (w->style & WS_CHILD); w = w->parent)
        if (w->parent && w->parent->hwnd == parent)
            return TRUE;
    return FALSE;
}

HWND WINAPI GetParent(HWND h)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w)
        return NULL;
    if (w->style & WS_CHILD)
        return w->parent && w->parent != desktop ? w->parent->hwnd : NULL;
    return w->owner ? w->owner->hwnd : NULL;
}

HWND WINAPI GetWindow(HWND h, UINT cmd)
{
    API_TRACE();
    Window *w = wnd_get(h), *s;
    if (!w)
        return NULL;
    switch (cmd) {
    case GW_CHILD: return w->children ? w->children->hwnd : NULL;
    case GW_HWNDNEXT: return w->next ? w->next->hwnd : NULL;
    case GW_HWNDFIRST: return w->parent && w->parent->children ? w->parent->children->hwnd : NULL;
    case GW_HWNDLAST:
        for (s = w->parent ? w->parent->children : NULL; s && s->next; s = s->next)
            ;
        return s ? s->hwnd : NULL;
    case GW_HWNDPREV:
        for (s = w->parent ? w->parent->children : NULL; s && s->next != w; s = s->next)
            ;
        return s ? s->hwnd : NULL;
    case GW_OWNER: return w->owner ? w->owner->hwnd : NULL;
    }
    return NULL;
}

HWND WINAPI FindWindowA(LPCSTR cls, LPCSTR title)
{
    API_TRACE();
    Window *w;
    for (w = desktop->children; w; w = w->next)
        if ((!cls || strcasecmp(w->cls->name, cls) == 0) && (!title || strcmp(w->text, title) == 0))
            return w->hwnd;
    return NULL;
}

WINBOOL WINAPI SetWindowTextA(HWND h, LPCSTR s)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w)
        return FALSE;
    return (WINBOOL)wnd_send(w, WM_SETTEXT, 0, (LPARAM)s);
}

int WINAPI GetWindowTextA(HWND h, LPSTR buf, int n)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w || n <= 0)
        return 0;
    buf[0] = 0;
    return (int)wnd_send(w, WM_GETTEXT, (WPARAM)n, (LPARAM)buf);
}

/* ---- window data ---- */

LONG_PTR WINAPI GetWindowLongA(HWND h, int i)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w)
        return 0;
    switch (i) {
    case GWL_WNDPROC: return (LONG_PTR)w->proc;
    case GWL_STYLE: return (LONG)w->style;
    case GWL_EXSTYLE: return (LONG)w->exstyle;
    case GWL_ID: return (LONG_PTR)w->menu;
    case GWL_USERDATA: return w->userdata;
    case GWL_HINSTANCE: return (LONG_PTR)w->instance;
    case GWL_HWNDPARENT: return (LONG_PTR)GetParent(h);
    }
    if (i < 0 || i / 4 >= w->nextra)
        return 0;
    return w->extra[i / 4];
}

LONG_PTR WINAPI SetWindowLongA(HWND h, int i, LONG_PTR v)
{
    API_TRACE();
    Window *w = wnd_get(h);
    LONG_PTR old = GetWindowLongA(h, i);
    if (!w)
        return 0;
    switch (i) {
    case GWL_WNDPROC: w->proc = (WNDPROC)v; break;
    case GWL_STYLE:
    case GWL_EXSTYLE: {
        STYLESTRUCT ss;
        ss.styleOld = (DWORD)old;
        ss.styleNew = (DWORD)v;
        wnd_send(w, WM_STYLECHANGING, (WPARAM)i, (LPARAM)&ss);
        if (!wnd_get(h))
            return old;
        if (i == GWL_STYLE)
            w->style = ss.styleNew;
        else
            w->exstyle = ss.styleNew;
        calc_client(w);
        wnd_generation++;
        wnd_send(w, WM_STYLECHANGED, (WPARAM)i, (LPARAM)&ss);
        break;
    }
    case GWL_ID: w->menu = (HMENU)v; break;
    case GWL_USERDATA: w->userdata = v; break;
    case GWL_HINSTANCE: w->instance = (HINSTANCE)v; break;
    default:
        if (i >= 0 && i / 4 < w->nextra)
            w->extra[i / 4] = v;
    }
    return old;
}

/* ---- showing, moving, sizing: Wine's set_window_pos and show_window ----
 * The messages and their order are Wine's (checked against traces of the
 * original game; see harness/msgdiff.sh). */

/* RDW_ERASENOW | RDW_ALLCHILDREN: WM_NCPAINT and WM_ERASEBKGND, now, to w
 * and its visible descendants that need them. */
/* Whether any of w's update region is visible (Wine erases through a DC
 * clipped to it: nothing visible, no WM_ERASEBKGND). */
static int update_shows(Window *w)
{
    Region vis = {0, 0, NULL};
    POINT o;
    int i, j, any = 0;
    wnd_visible_region(w, 1, &vis);
    wnd_client_screen(w, &o);
    for (i = 0; i < w->update.n && !any; i++) {
        RECT u = w->update.r[i], x;
        OffsetRect(&u, o.x, o.y);
        for (j = 0; j < vis.n && !any; j++)
            any = IntersectRect(&x, &u, &vis.r[j]);
    }
    rgn_free(&vis);
    return any;
}

static void erase_now(Window *w)
{
    HWND h = w->hwnd;
    Window *c, *list[256];
    int n = 0, i;
    if (!wnd_is_visible(w))
        return;
    if (w->erase & 2) {
        w->erase &= ~2;
        wnd_send(w, WM_NCPAINT, 1, 0);
        if (!wnd_get(h))
            return;
    }
    if (w->erase & 1) {
        w->erase &= ~1;
        if (update_shows(w)) {
            HDC hdc = GetDC(h);
            wnd_send(w, WM_ERASEBKGND, (WPARAM)hdc, 0);
            ReleaseDC(h, hdc);
            if (!wnd_get(h))
                return;
        }
    }
    for (c = w->children; c && n < 256; c = c->next)
        list[n++] = c;
    for (i = 0; i < n; i++)
        if (wnd_get(list[i]->hwnd))
            erase_now(list[i]);
}

/* RedrawWindow(RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN) of
 * region rg (screen coordinates), as the Wine server does it: the window
 * gets what of rg it covers, its visible children what of rg falls in its
 * client area over them. */
static void invalidate_tree_rgn(Window *w, const Region *rg)
{
    Region t = {0, 0, NULL};
    RECT wr, cr;
    POINT o;
    Window *c;
    int i;
    wnd_screen_rect(w, &wr);
    rgn_copy(&t, rg);
    rgn_intersect_rect(&t, &wr);
    if (rgn_empty(&t)) {
        rgn_free(&t);
        return;
    }
    w->erase |= 2;              /* RDW_FRAME: WM_NCPAINT, whatever the frame */
    wnd_client_screen(w, &o);
    cr.left = o.x; cr.top = o.y;
    cr.right = o.x + (w->client.right - w->client.left);
    cr.bottom = o.y + (w->client.bottom - w->client.top);
    rgn_intersect_rect(&t, &cr);
    for (i = 0; i < t.n; i++) {
        RECT r = t.r[i];
        OffsetRect(&r, -o.x, -o.y);
        invalidate_client(w, &r, 1);
    }
    if (!rgn_empty(&t) && !w->minimized)
        for (c = w->children; c; c = c->next)
            if (c->style & WS_VISIBLE)
                invalidate_tree_rgn(c, &t);
    rgn_free(&t);
}

/* The whole window, or (vis) what of it shows -- the exposure on showing. */
static void invalidate_tree(Window *w, int vis)
{
    Region rg = {0, 0, NULL};
    RECT wr;
    wnd_screen_rect(w, &wr);
    if (vis) {
        /* the reference clips the exposure by the ancestors' client areas
           only: covered children still get WM_NCPAINT (not WM_ERASEBKGND) */
        Window *a;
        for (a = w->parent; a && a != desktop; a = a->parent) {
            POINT o;
            RECT c;
            wnd_client_screen(a, &o);
            c.left = o.x; c.top = o.y;
            c.right = o.x + (a->client.right - a->client.left);
            c.bottom = o.y + (a->client.bottom - a->client.top);
            if (!IntersectRect(&wr, &wr, &c))
                return;
        }
    }
    rgn_set(&rg, &wr);
    invalidate_tree_rgn(w, &rg);
    rgn_free(&rg);
}

#define SWP_NOCLIENTSIZE_ 0x0800
#define SWP_NOCLIENTMOVE_ 0x1000
#define SWP_STATECHANGED_ 0x8000

static void log_geom(const char *what, Window *w, int x, int y, int cx, int cy, UINT f)
{
    RECT wr, cr;
    if (!getenv("CIV2_TRACE_PT"))
        return;
    wnd_screen_rect(w, &wr);
    GetClientRect(w->hwnd, &cr);
    plat_log("geom: %s(%x, %d,%d %dx%d flags %x) -> window %d,%d,%d,%d client %dx%d\n", what, (unsigned)w->hwnd, x, y, cx,
             cy, f, (int)wr.left, (int)wr.top, (int)wr.right, (int)wr.bottom, (int)cr.right, (int)cr.bottom);
}

/* WM_GETMINMAXINFO, with Wine's defaults. */
static void get_minmax(Window *w, MINMAXINFO *mmi)
{
    int fx = (w->style & WS_THICKFRAME) ? GetSystemMetrics(SM_CXFRAME) :
             ((w->style & WS_DLGFRAME) || (w->exstyle & WS_EX_DLGMODALFRAME)) ? GetSystemMetrics(SM_CXDLGFRAME) :
             (w->style & WS_BORDER) ? GetSystemMetrics(SM_CXBORDER) : 0;
    int fy = fx;
    memset(mmi, 0, sizeof *mmi);
    mmi->ptMaxSize.x = screen_w + 2 * fx;
    mmi->ptMaxSize.y = screen_h + 2 * fy;
    mmi->ptMaxPosition.x = -fx;
    mmi->ptMaxPosition.y = -fy;
    mmi->ptMinTrackSize.x = GetSystemMetrics(SM_CXMINTRACK);
    mmi->ptMinTrackSize.y = GetSystemMetrics(SM_CYMINTRACK);
    mmi->ptMaxTrackSize.x = GetSystemMetrics(SM_CXMAXTRACK);
    mmi->ptMaxTrackSize.y = GetSystemMetrics(SM_CYMAXTRACK);
    wnd_send(w, WM_GETMINMAXINFO, 0, (LPARAM)mmi);
}

/* The desktop now w x h (the host's window changed size, as to full screen
 * and back): its surfaces, and the maximized windows maximized anew in it,
 * as Windows does when the display's resolution changes. */
void wnd_resize_desktop(int w, int h)
{
    Bitmap *s = bmp_create(w, h, 32, 0), *r = bmp_create(w, h, 32, 0);
    Window *list[256], *c;
    int n = 0, i, y;
    for (y = 0; y < h && y < root->h; y++)
        memcpy(bmp_row(r, y), bmp_row(root, y), (size_t)(w < root->w ? w : root->w) * 4);
    bmp_destroy(screen);
    bmp_destroy(root);
    screen = s;
    root = r;
    screen_w = w;
    screen_h = h;
    desktop->rect.right = w;
    desktop->rect.bottom = h;
    desktop->client = desktop->rect;
    wnd_generation++;
    for (c = desktop->children; c && n < 256; c = c->next)
        if (c->maximized && !c->minimized)
            list[n++] = c;
    for (i = 0; i < n; i++)
        if (wnd_get(list[i]->hwnd)) {
            MINMAXINFO mmi;
            get_minmax(list[i], &mmi);
            SetWindowPos(list[i]->hwnd, 0, mmi.ptMaxPosition.x, mmi.ptMaxPosition.y, mmi.ptMaxSize.x,
                         mmi.ptMaxSize.y, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    InvalidateRect(0, NULL, TRUE);
}

static int first_focus = 1;

/* Wine's set_focus_window. */
static HWND set_focus_window(HWND h)
{
    HWND prev = focus_wnd;
    Window *w = wnd_get(h), *p = wnd_get(prev);
    if (prev == h)
        return prev;
    focus_wnd = h;
    if (p) {
        wnd_send(p, WM_KILLFOCUS, (WPARAM)h, 0);
        if (wnd_get(prev))
            wnd_send(p, WM_IME_SETCONTEXT, FALSE, ISC_SHOWUIALL);
        if (focus_wnd != h)
            return prev;            /* changed while handling the messages */
    }
    if (w) {
        if (first_focus) {
            first_focus = 0;
            wnd_send(w, WM_IME_SELECT, TRUE, 0);
        }
        wnd_send(w, WM_IME_SETCONTEXT, TRUE, ISC_SHOWUIALL);
        if (focus_wnd == h)
            wnd_send(w, WM_SETFOCUS, (WPARAM)prev, 0);
    }
    return prev;
}

/* Wine's set_active_window (one thread: WM_ACTIVATEAPP only on first activation). */
static int app_active;

static void set_active_window(Window *w, int focus)
{
    HWND prev = active_wnd, h = w ? w->hwnd : 0;
    Window *p = wnd_get(prev), *t;
    if (prev == h)
        return;
    if (p) {
        wnd_send(p, WM_NCACTIVATE, FALSE, (LPARAM)h);
        wnd_send(p, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, p->minimized), (LPARAM)h);
    }
    active_wnd = h;
    if (w) {
        wnd_host_sync(w);           /* activation brings the window to the top: SetWindowPos */
        if (wnd_send(w, WM_QUERYNEWPALETTE, 0, 0))
            for (t = desktop->children; t; t = t->next)
                wnd_send(t, WM_PALETTEISCHANGING, (WPARAM)h, 0);
        if (!wnd_get(h))
            return;
    }
    if (!app_active != !w) {
        Window *list[256];
        int n = 0, i;
        app_active = w != NULL;
        for (t = desktop->children; t && n < 256; t = t->next)
            list[n++] = t;
        for (i = 0; i < n; i++)
            if (wnd_get(list[i]->hwnd))
                wnd_send(list[i], WM_ACTIVATEAPP, app_active, 0);
    }
    if (w && wnd_get(h)) {
        wnd_send(w, WM_NCACTIVATE, TRUE, (LPARAM)prev);
        wnd_send(w, WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, w->minimized), (LPARAM)prev);
    }
    if (focus && w && wnd_get(h) && active_wnd == h) {
        Window *f = wnd_get(focus_wnd);
        while (f && (f->style & WS_CHILD))
            f = f->parent;
        if (f != w)
            set_focus_window(h);
    }
}

static void activate(Window *w)
{
    if (!w || (w->style & WS_CHILD))
        return;
    set_active_window(w, 1);
}

/* The active window went away: activate its owner, or else the topmost
 * other visible, enabled top-level window (activate_other_window). */
static void activate_other(Window *gone)
{
    Window *t = gone->owner;
    if (!t || t->destroyed || !(t->style & WS_VISIBLE) || (t->style & WS_DISABLED))
        for (t = desktop->children; t; t = t->next)
            if (t != gone && (t->style & WS_VISIBLE) && !(t->style & WS_DISABLED) && !t->minimized)
                break;
    if (t)
        set_active_window(t, 1);
    else
        set_active_window(NULL, 0);
}

/* Wine's set_window_pos.  Returns FALSE if the window went away. */
static WINBOOL swp(Window *w, HWND after, int x, int y, int cx, int cy, UINT flags)
{
    WINDOWPOS wp;
    if ((!(flags & SWP_NOMOVE) && (!plat_sane_extent(x) || !plat_sane_extent(y))) ||
        (!(flags & SWP_NOSIZE) && (!plat_sane_extent(cx) || !plat_sane_extent(cy) || cx < -1 || cy < -1)))
        plat_suspicious("SetWindowPos(%x) to %d,%d size %dx%d flags %x", (unsigned)w->hwnd, x, y, cx, cy, flags);
    RECT old_rect = w->rect, old_client = w->client, old_screen, new_screen;
    Region old_vis = {0, 0, NULL}, old_wvis = {0, 0, NULL};
    int raising = 0;
    POINT old_corigin, new_corigin;
    HWND h = w->hwnd;
    UINT orig = flags;
    int was_visible = (w->style & WS_VISIBLE) != 0;
    Window *parent = w->parent ? w->parent : desktop;

    /* fixup_flags */
    if (was_visible)
        flags &= ~SWP_SHOWWINDOW;
    else {
        if (!(flags & SWP_SHOWWINDOW))
            flags |= SWP_NOREDRAW;
        flags &= ~SWP_HIDEWINDOW;
    }
    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    if (old_rect.right - old_rect.left == cx && old_rect.bottom - old_rect.top == cy)
        flags |= SWP_NOSIZE;
    if (old_rect.left == x && old_rect.top == y)
        flags |= SWP_NOMOVE;
    if ((w->style & (WS_CHILD | WS_POPUP)) == WS_CHILD)
        flags |= SWP_NOACTIVATE;            /* children get WM_CHILDACTIVATE instead */
    else if (!(flags & SWP_NOACTIVATE) && w->hwnd != active_wnd) {
        flags &= ~SWP_NOZORDER;
        after = HWND_TOP;
    }
    if (!(flags & SWP_NOZORDER)) {
        if (after == HWND_TOPMOST || after == HWND_NOTOPMOST)
            after = HWND_TOP;
        if (after == HWND_TOP && parent->children == w)
            flags |= SWP_NOZORDER;
        else if (after == HWND_BOTTOM && !w->next)
            flags |= SWP_NOZORDER;
        else if (after != HWND_TOP && after != HWND_BOTTOM && wnd_get(after) && wnd_get(after)->next == w)
            flags |= SWP_NOZORDER;
    }

    memset(&wp, 0, sizeof wp);
    wp.hwnd = h;
    wp.hwndInsertAfter = after;
    wp.x = (flags & SWP_NOMOVE) ? old_rect.left : x;
    wp.y = (flags & SWP_NOMOVE) ? old_rect.top : y;
    wp.cx = (flags & SWP_NOSIZE) ? old_rect.right - old_rect.left : cx;
    wp.cy = (flags & SWP_NOSIZE) ? old_rect.bottom - old_rect.top : cy;
    wp.flags = flags;
    if (!(flags & SWP_NOSENDCHANGING)) {
        wnd_send(w, WM_WINDOWPOSCHANGING, 0, (LPARAM)&wp);
        if (!wnd_get(h))
            return FALSE;
    }
    flags = wp.flags;
    /* calc_winpos: resizable windows keep within their tracking limits */
    if (!(flags & SWP_NOSIZE) && !w->minimized &&
        ((w->style & WS_THICKFRAME) || !(w->style & (WS_POPUP | WS_CHILD)))) {
        MINMAXINFO mmi;
        get_minmax(w, &mmi);
        if (wp.cx > mmi.ptMaxTrackSize.x) wp.cx = mmi.ptMaxTrackSize.x;
        if (wp.cy > mmi.ptMaxTrackSize.y) wp.cy = mmi.ptMaxTrackSize.y;
        if (wp.cx < mmi.ptMinTrackSize.x) wp.cx = mmi.ptMinTrackSize.x;
        if (wp.cy < mmi.ptMinTrackSize.y) wp.cy = mmi.ptMinTrackSize.y;
    }
    if (!(flags & SWP_NOSIZE) && wp.cx == old_rect.right - old_rect.left && wp.cy == old_rect.bottom - old_rect.top)
        flags |= SWP_NOSIZE;
    if (!(flags & SWP_NOMOVE) && wp.x == old_rect.left && wp.y == old_rect.top)
        flags |= SWP_NOMOVE;

    wnd_screen_rect(w, &old_screen);
    wnd_client_screen(w, &old_corigin);
    if (w->style & WS_CHILD)
        wnd_visible_region(w, 0, &old_wvis);
    /* Z order */
    if (!(flags & SWP_NOZORDER) && (w->style & WS_CHILD) && wnd_is_visible(w) &&
        (flags & (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_HIDEWINDOW)) == (SWP_NOMOVE | SWP_NOSIZE)) {
        wnd_visible_region(w, 0, &old_vis);
        raising = 1;
    }
    if (!(flags & SWP_NOZORDER)) {
        if (wp.hwndInsertAfter == HWND_TOP)
            raise_wnd(w);
        else if (wp.hwndInsertAfter == HWND_BOTTOM) {
            unlink_wnd(w);
            link_bottom(w);
        } else if (wnd_get(wp.hwndInsertAfter) && wnd_get(wp.hwndInsertAfter)->parent == w->parent) {
            Window *a = wnd_get(wp.hwndInsertAfter);
            unlink_wnd(w);
            w->next = a->next;
            a->next = w;
        }
    }
    /* geometry; WM_NCCALCSIZE when the size or the frame changes */
    w->rect.left = wp.x;
    w->rect.top = wp.y;
    w->rect.right = wp.x + wp.cx;
    w->rect.bottom = wp.y + wp.cy;
    if (!(flags & SWP_NOSIZE) || (flags & SWP_FRAMECHANGED)) {
        NCCALCSIZE_PARAMS ncp;
        memset(&ncp, 0, sizeof ncp);
        wnd_screen_rect(w, &ncp.rgrc[0]);
        wnd_send(w, WM_NCCALCSIZE, TRUE, (LPARAM)&ncp);
        if (!wnd_get(h))
            return FALSE;
    }
    calc_client(w);
    {
        /* The pending update region never extends past the client area. */
        RECT cr = {0, 0, 0, 0};
        cr.right = w->client.right - w->client.left;
        cr.bottom = w->client.bottom - w->client.top;
        rgn_intersect_rect(&w->update, &cr);
    }
    if (flags & SWP_SHOWWINDOW) {
        w->style |= WS_VISIBLE;
        if (!(w->style & WS_CHILD)) {
            w->managed = is_managed(w, !(flags & SWP_NOACTIVATE));
            if (w->managed) {
                /* x11drv maps the window: it asks for the icons */
                wnd_send(w, WM_GETICON, ICON_BIG, 0);
                wnd_send(w, WM_GETICON, ICON_SMALL, 0);
            }
        }
    } else if (flags & SWP_HIDEWINDOW) {
        w->style &= ~WS_VISIBLE;
    }
    wnd_generation++;
    wnd_screen_rect(w, &new_screen);
    wnd_client_screen(w, &new_corigin);

    /* activation (top-level windows) */
    if (!(flags & SWP_NOACTIVATE) && !(w->style & WS_CHILD) && (w->style & WS_VISIBLE))
        set_active_window(w, 1);
    if (!wnd_get(h))
        return FALSE;

    /* redraw */
    if (flags & SWP_HIDEWINDOW) {
        if (w->style & WS_CHILD)
            expose_region(w->parent, &old_wvis, NULL);
    } else if (wnd_is_visible(w)) {
        if ((w->style & WS_CHILD) && memcmp(&old_screen, &new_screen, sizeof old_screen)) {
            /* what showed of it and no longer does: the parent shows there */
            Region now = {0, 0, NULL};
            wnd_visible_region(w, 0, &now);
            expose_region(w->parent, &old_wvis, &now);
            rgn_free(&now);
        }
        if (raising) {
            /* raised: what was covered shows now */
            Region now = {0, 0, NULL};
            int i;
            wnd_visible_region(w, 0, &now);
            for (i = 0; i < old_vis.n; i++)
                rgn_subtract_rect(&now, &old_vis.r[i]);
            invalidate_tree_rgn(w, &now);
            rgn_free(&now);
        }
        if (flags & SWP_SHOWWINDOW) {
            /* the parent's pending update stays as it is, even where the
               child covers it now (Wine's server only crops it in
               validate_parents, for parents that do not clip children) */
            invalidate_tree(w, 1);
            erase_now(w);       /* RDW_ERASENOW: frames and backgrounds now */
        } else if (!(flags & SWP_NOSIZE) && !(flags & SWP_NOREDRAW)) {
            UINT cs = w->cls ? w->cls->wc.style : 0;
            int wchg = (old_rect.right - old_rect.left) != wp.cx, hchg = (old_rect.bottom - old_rect.top) != wp.cy;
            if ((wchg && (cs & CS_HREDRAW)) || (hchg && (cs & CS_VREDRAW)))
                invalidate_tree(w, 0);
            else {
                /* only what was not shown before: the Wine server keeps
                   the old client area's bits (calc_ncsize's valid rect, at
                   the top left) and exposes the rest */
                RECT now = {0, 0, 0, 0};
                int ow = old_client.right - old_client.left, oh = old_client.bottom - old_client.top;
                now.right = w->client.right - w->client.left;
                now.bottom = w->client.bottom - w->client.top;
                if (now.right > ow || now.bottom > oh) {
                    if (plat_normal_desktop)
                        invalidate_tree(w, 0);      /* children it clipped show more of themselves too */
                    else {
                        RECT strip = now;
                        if (now.right > ow) {
                            strip.left = ow;
                            InvalidateRect(h, &strip, TRUE);
                        }
                        if (now.bottom > oh) {
                            strip.left = 0;
                            strip.top = oh;
                            InvalidateRect(h, &strip, TRUE);
                        }
                        /* the server flags the window for erasing even if
                           what it exposed does not show; whether it gets
                           WM_ERASEBKGND depends on its whole update region */
                        w->erase |= 1;
                    }
                }
            }
            w->erase |= 2;      /* the frame is redrawn */
        } else if (flags & SWP_FRAMECHANGED) {
            /* Wine redraws everything if the client area moved, or if the
               window had an update region pending (nothing valid to keep) */
            if (!(flags & SWP_NOREDRAW) &&
                (!rgn_empty(&w->update) || new_corigin.x != old_corigin.x || new_corigin.y != old_corigin.y ||
                 old_client.right - old_client.left != w->client.right - w->client.left ||
                 old_client.bottom - old_client.top != w->client.bottom - w->client.top))
                invalidate_tree(w, 0);
            else if (wnd_has_nc(w))
                w->erase |= 2;
        }
    }
    if (!wnd_get(h))
        return FALSE;

    rgn_free(&old_vis);
    rgn_free(&old_wvis);

    /* children are activated before WM_WINDOWPOSCHANGED, as in Wine */
    if (!(orig & SWP_NOACTIVATE) && !(flags & SWP_HIDEWINDOW) && (w->style & (WS_CHILD | WS_POPUP)) == WS_CHILD) {
        wnd_send(w, WM_CHILDACTIVATE, 0, 0);
        if (!wnd_get(h))
            return FALSE;
    }

    /* Wine: erase the parent (a top-level window itself) when hiding or
       when the geometry changed */
    if (new_corigin.x == old_corigin.x && new_corigin.y == old_corigin.y)
        flags |= SWP_NOCLIENTMOVE_;
    if (old_client.right - old_client.left == w->client.right - w->client.left &&
        old_client.bottom - old_client.top == w->client.bottom - w->client.top)
        flags |= SWP_NOCLIENTSIZE_;
    if (!(orig & SWP_DEFERERASE) &&
        ((orig & SWP_HIDEWINDOW) ||
         (!(orig & SWP_SHOWWINDOW) &&
          (flags & (SWP_NOSIZE | SWP_NOMOVE | SWP_NOCLIENTSIZE_ | SWP_NOCLIENTMOVE_ | SWP_NOZORDER |
                    SWP_FRAMECHANGED | SWP_HIDEWINDOW | SWP_SHOWWINDOW)) != (SWP_NOSIZE | SWP_NOCLIENTSIZE_ | SWP_NOZORDER)))) {
        Window *p = (w->style & WS_CHILD) && w->parent && w->parent != desktop ? w->parent : w;
        erase_now(p);
        if (!wnd_get(h))
            return FALSE;
    }

    /* WM_WINDOWPOSCHANGED, unless nothing changed */
    if ((flags & (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW | SWP_HIDEWINDOW | SWP_FRAMECHANGED)) !=
        (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER)) {
        wp.flags = flags;
        wp.x = w->rect.left;
        wp.y = w->rect.top;
        wp.cx = w->rect.right - w->rect.left;
        wp.cy = w->rect.bottom - w->rect.top;
        wnd_send(w, WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
    }
    if (!wnd_get(h))
        return FALSE;
    return TRUE;
}

WINBOOL WINAPI MoveWindow(HWND h, int x, int y, int cx, int cy, WINBOOL repaint)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w)
        return FALSE;
    swp(w, 0, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE | (repaint ? 0 : SWP_NOREDRAW));
    if (wnd_get(h)) {
        wnd_host_sync(w);
        log_geom("MoveWindow", w, x, y, cx, cy, 0);
    }
    return TRUE;
}

WINBOOL WINAPI SetWindowPos(HWND h, HWND after, int x, int y, int cx, int cy, UINT flags)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w)
        return FALSE;
    swp(w, after, x, y, cx, cy, flags);
    if (wnd_get(h)) {
        wnd_host_sync(w);
        log_geom("SetWindowPos", w, x, y, cx, cy, flags);
    }
    return TRUE;
}

WINBOOL WINAPI BringWindowToTop(HWND h)
{
    API_TRACE();
    return SetWindowPos(h, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

/* WINPOS_MinMaximize: the new placement; returns SWP flags. */
static UINT min_maximize(Window *w, int cmd, RECT *pos)
{
    MINMAXINFO mmi;
    UINT f = 0;
    *pos = w->rect;
    switch (cmd) {
    case SW_MINIMIZE: case SW_SHOWMINIMIZED: case SW_SHOWMINNOACTIVE: case SW_FORCEMINIMIZE:
        if (w->minimized)
            return 0;
        if (w->maximized)
            w->restore_max = 1;
        else
            w->normal = w->rect;
        w->minimized = 1;
        w->maximized = 0;
        w->style = (w->style | WS_MINIMIZE) & ~WS_MAXIMIZE;
        f |= SWP_STATECHANGED_ | SWP_NOCOPYBITS;
        break;
    case SW_MAXIMIZE:
        /* Wine's window_min_maximize: a visible maximized window is left
           alone, before WM_GETMINMAXINFO */
        if (w->maximized && !w->minimized && (w->style & WS_VISIBLE))
            return 0;
        get_minmax(w, &mmi);
        if (w->maximized && !w->minimized)
            return 0;
        if (!w->minimized)
            w->normal = w->rect;
        w->minimized = 0;
        w->maximized = 1;
        w->style = (w->style | WS_MAXIMIZE) & ~WS_MINIMIZE;
        pos->left = mmi.ptMaxPosition.x;
        pos->top = mmi.ptMaxPosition.y;
        pos->right = pos->left + mmi.ptMaxSize.x;
        pos->bottom = pos->top + mmi.ptMaxSize.y;
        f |= SWP_STATECHANGED_;
        break;
    case SW_RESTORE: case SW_SHOWNORMAL: case SW_SHOWDEFAULT: case SW_SHOWNOACTIVATE:
        if (w->minimized && w->restore_max) {
            w->restore_max = 0;
            w->minimized = 0;
            w->style &= ~WS_MINIMIZE;
            return min_maximize(w, SW_MAXIMIZE, pos);
        }
        if (!w->minimized && !w->maximized)
            return 0;
        w->minimized = w->maximized = 0;
        w->style &= ~(WS_MINIMIZE | WS_MAXIMIZE);
        if (!rect_empty(&w->normal))
            *pos = w->normal;
        f |= SWP_STATECHANGED_;
        break;
    }
    return f;
}

static WINBOOL show_window(HWND h, int cmd)
{
    Window *w = wnd_get(h), *parent;
    int was_visible, show_flag = cmd != SW_HIDE;
    UINT f = 0;
    RECT pos;
    if (!w)
        return FALSE;
    was_visible = (w->style & WS_VISIBLE) != 0;
    plat_log("ShowWindow(%x, %d)\n", (unsigned)h, cmd);
    pos = w->rect;
    switch (cmd) {
    case SW_HIDE:
        if (!was_visible)
            return FALSE;
        f |= SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE;
        if (h != active_wnd)
            f |= SWP_NOACTIVATE | SWP_NOZORDER;
        break;
    case SW_SHOWMINNOACTIVE: case SW_MINIMIZE: case SW_FORCEMINIMIZE:
        f |= SWP_NOACTIVATE | SWP_NOZORDER;
        /* fall through */
    case SW_SHOWMINIMIZED:
        f |= SWP_SHOWWINDOW | SWP_FRAMECHANGED;
        f |= min_maximize(w, SW_MINIMIZE, &pos);
        if (!(f & SWP_STATECHANGED_) && was_visible)
            return TRUE;
        break;
    case SW_SHOWMAXIMIZED:
        if (!was_visible)
            f |= SWP_SHOWWINDOW;
        f |= SWP_FRAMECHANGED;
        f |= min_maximize(w, SW_MAXIMIZE, &pos);
        if (!(f & SWP_STATECHANGED_) && was_visible)
            return TRUE;
        break;
    case SW_SHOWNA:
        f |= SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE;
        if (w->style & WS_CHILD)
            f |= SWP_NOZORDER;
        break;
    case SW_SHOW:
        if (was_visible)
            return TRUE;
        f |= SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE;
        if (w->style & WS_CHILD)
            f |= SWP_NOACTIVATE | SWP_NOZORDER;
        break;
    case SW_SHOWNOACTIVATE:
        f |= SWP_NOACTIVATE | SWP_NOZORDER;
        /* fall through */
    case SW_RESTORE: case SW_SHOWNORMAL: case SW_SHOWDEFAULT:
        f |= SWP_SHOWWINDOW;
        if (w->minimized || w->maximized) {
            f |= SWP_FRAMECHANGED;
            f |= min_maximize(w, SW_RESTORE, &pos);
        } else {
            if (was_visible)
                return TRUE;
            f |= SWP_NOSIZE | SWP_NOMOVE;
        }
        if ((w->style & WS_CHILD) && !(f & SWP_STATECHANGED_))
            f |= SWP_NOACTIVATE | SWP_NOZORDER;
        break;
    default:
        return was_visible;
    }
    if ((show_flag != was_visible || cmd == SW_SHOWNA) && cmd != SW_SHOWMAXIMIZED && !(f & SWP_STATECHANGED_)) {
        wnd_send(w, WM_SHOWWINDOW, show_flag, 0);
        if (!wnd_get(h))
            return was_visible;
    }
    parent = w->parent ? w->parent : desktop;
    if (parent != desktop && !wnd_is_visible(parent) && !(f & SWP_STATECHANGED_)) {
        /* parent not visible: simply toggle WS_VISIBLE */
        if (show_flag)
            w->style |= WS_VISIBLE;
        else
            w->style &= ~WS_VISIBLE;
        wnd_generation++;
    } else if (cmd == SW_SHOWMAXIMIZED && !plat_normal_desktop && !(w->style & WS_CHILD) &&
               !(getenv("CIV2_TEST_FULL") && *getenv("CIV2_TEST_FULL"))) {
        /* (CIV2_TEST_FULL=1, tests of OpenCiv2's additions, which have no
           reference: the window maximized, all of the desktop shown) */
        /* The reference desktop has no window manager.  Wine makes the
         * window maximized, but leaves resizing its X window to the
         * (absent) manager: the window keeps showing in its old X window --
         * the old window rect without the frame and caption (the manager's
         * decorations) -- drawn from the old position. */
        RECT old = w->rect, in;
        nc_insets(w, &in);
        swp(w, HWND_TOP, pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top, f & ~SWP_STATECHANGED_);
        if (!wnd_get(h))
            return was_visible;
        w->host_dx = old.left - w->rect.left;
        w->host_dy = old.top - w->rect.top;
        w->host_clip.left = old.left + in.left;
        w->host_clip.right = old.right - in.right + ((w->style & WS_VSCROLL) ? GetSystemMetrics(SM_CXVSCROLL) : 0);
        w->host_clip.top = old.top + in.left + (((w->style & WS_CAPTION) == WS_CAPTION) ? GetSystemMetrics(SM_CYCAPTION) : 0);
        w->host_clip.bottom = old.bottom - in.left;
        w->host_clipped = 1;
        w->host_frozen = 1;     /* until this ShowWindow returns */
        wnd_generation++;
    } else {
        swp(w, HWND_TOP, pos.left, pos.top, pos.right - pos.left, pos.bottom - pos.top, f & ~SWP_STATECHANGED_);
    }
    if (!wnd_get(h))
        return was_visible;
    if (cmd == SW_HIDE) {
        HWND f2;
        if (h == active_wnd)
            activate_other(w);
        f2 = focus_wnd;
        if (h == f2) {          /* Wine 8: not when a descendant has it */
            Window *pp = w->parent;
            SetFocus(pp && pp != desktop ? pp->hwnd : 0);
        }
        return was_visible;
    }
    if (w->need_size) {
        /* the size messages CreateWindowEx left for the first ShowWindow */
        w->need_size = 0;
        wnd_send(w, WM_SIZE, w->maximized ? SIZE_MAXIMIZED : w->minimized ? SIZE_MINIMIZED : SIZE_RESTORED,
                 MAKELPARAM(w->client.right - w->client.left, w->client.bottom - w->client.top));
        if (wnd_get(h))
            wnd_send(w, WM_MOVE, 0, MAKELPARAM(w->rect.left + w->client.left, w->rect.top + w->client.top));
    }
    return was_visible;
}

WINBOOL WINAPI ShowWindow(HWND h, int cmd)
{
    API_TRACE();
    WINBOOL r = show_window(h, cmd);
    Window *w = wnd_get(h);
    if (w)
        w->host_frozen = 0;
    return r;
}

WINBOOL WINAPI EnableWindow(HWND h, WINBOOL enable)
{
    API_TRACE();
    Window *w = wnd_get(h);
    int was;
    if (!w)
        return FALSE;
    was = (w->style & WS_DISABLED) != 0;
    if (!enable) {
        wnd_send(w, WM_CANCELMODE, 0, 0);
        if (!wnd_get(h))
            return was;
    }
    if (enable)
        w->style &= ~WS_DISABLED;
    else
        w->style |= WS_DISABLED;
    if (was != !enable)
        wnd_send(w, WM_ENABLE, enable, 0);
    if (!enable && (focus_wnd == h || IsChild(h, focus_wnd)))
        SetFocus(NULL);
    return was;
}

WINBOOL WINAPI GetWindowRect(HWND h, LPRECT r)
{
    API_TRACE();
    Window *w = h ? wnd_get(h) : desktop;
    if (!w)
        return FALSE;
    wnd_screen_rect(w, r);
    return TRUE;
}

WINBOOL WINAPI GetClientRect(HWND h, LPRECT r)
{
    API_TRACE();
    Window *w = h ? wnd_get(h) : desktop;
    if (!w)
        return FALSE;
    r->left = r->top = 0;
    r->right = w->client.right - w->client.left;
    r->bottom = w->client.bottom - w->client.top;
    return TRUE;
}

WINBOOL WINAPI ClientToScreen(HWND h, LPPOINT p)
{
    API_TRACE();
    Window *w = wnd_get(h);
    POINT o;
    if (!w)
        return FALSE;
    wnd_client_screen(w, &o);
    p->x += o.x;
    p->y += o.y;
    return TRUE;
}

WINBOOL WINAPI ScreenToClient(HWND h, LPPOINT p)
{
    API_TRACE();
    Window *w = wnd_get(h);
    POINT o;
    if (!w)
        return FALSE;
    wnd_client_screen(w, &o);
    p->x -= o.x;
    p->y -= o.y;
    return TRUE;
}

int WINAPI MapWindowPoints(HWND from, HWND to, LPPOINT pts, UINT n)
{
    API_TRACE();
    POINT a = {0, 0}, b = {0, 0};
    UINT i;
    if (from && wnd_get(from))
        wnd_client_screen(wnd_get(from), &a);
    if (to && wnd_get(to))
        wnd_client_screen(wnd_get(to), &b);
    for (i = 0; i < n; i++) {
        pts[i].x += a.x - b.x;
        pts[i].y += a.y - b.y;
    }
    return MAKELONG(a.x - b.x, a.y - b.y);
}

/* ---- focus and capture ---- */

WINBOOL WINAPI IsWindowEnabled(HWND h)
{
    API_TRACE();
    Window *w = wnd_get(h);
    return w && !(w->style & WS_DISABLED);
}

HWND WINAPI SetActiveWindow(HWND h)
{
    API_TRACE();
    HWND prev = active_wnd;
    Window *w = wnd_get(h);
    if (!w || (w->style & WS_CHILD))
        return 0;
    set_active_window(w, 1);
    return prev;
}

HWND WINAPI SetFocus(HWND h)
{
    API_TRACE();
    HWND prev = focus_wnd;
    Window *w = wnd_get(h), *top;
    if (h && !w)
        return 0;
    if (w) {
        if (w->style & (WS_MINIMIZE | WS_DISABLED))
            return 0;
        for (top = w; top->style & WS_CHILD; top = top->parent)
            ;
        if (top->hwnd != active_wnd && top != desktop) {
            set_active_window(top, 0);
            if (!wnd_get(h))
                return 0;
        }
    }
    set_focus_window(h);
    return wnd_get(prev) ? prev : 0;
}

HWND WINAPI GetFocus(void) { API_TRACE(); return wnd_get(focus_wnd) ? focus_wnd : NULL; }

HWND WINAPI SetCapture(HWND h)
{
    API_TRACE();
    HWND prev = capture_wnd;
    capture_wnd = h;
    return prev;
}

WINBOOL WINAPI ReleaseCapture(void)
{
    API_TRACE();
    HWND prev = capture_wnd;
    capture_wnd = NULL;
    if (wnd_get(prev))
        wnd_send(wnd_get(prev), WM_CAPTURECHANGED, 0, 0);
    return TRUE;
}

/* The window at a desktop point, as Wine's server finds it (deepest visible,
 * not into a disabled one); *hit: HTCLIENT or other. */
Window *wnd_from_point(POINT pt, int *hit)
{
    Window *w = desktop, *c;
    *hit = HTNOWHERE;
    for (;;) {
        Window *found = NULL;
        for (c = w->children; c; c = c->next) {
            RECT r;
            if (!(c->style & WS_VISIBLE) || c->minimized)
                continue;
            if ((c->style & (WS_POPUP | WS_CHILD | WS_DISABLED)) == (WS_CHILD | WS_DISABLED))
                continue;       /* (Wine's is_point_in_window) a disabled child is not hit: its parent is */
            if (w == desktop)
                wnd_shown_rect(c, &r);
            else
                wnd_screen_rect(c, &r);
            if (pt.x >= r.left && pt.x < r.right && pt.y >= r.top && pt.y < r.bottom) {
                found = c;
                break;
            }
        }
        if (!found)
            break;
        if (w == desktop && found->host_clipped) {
            /* The window shows displaced: back to the coordinates it has. */
            pt.x -= found->host_dx;
            pt.y -= found->host_dy;
        }
        w = found;
        {
            POINT o;
            wnd_client_screen(w, &o);
            if (pt.x < o.x || pt.y < o.y || pt.x >= o.x + w->client.right - w->client.left ||
                pt.y >= o.y + w->client.bottom - w->client.top) {
                *hit = HTCAPTION;
                if (!(w->style & WS_CHILD) && w->menu) {
                    RECT r;
                    wnd_screen_rect(w, &r);
                    if (pt.y >= o.y - menu_bar_height(w) && pt.y < o.y)
                        *hit = HTMENU;
                }
                if ((w->style & WS_VSCROLL) && pt.x >= o.x + w->client.right - w->client.left)
                    *hit = HTVSCROLL;
                if ((w->style & WS_HSCROLL) && pt.y >= o.y + w->client.bottom - w->client.top)
                    *hit = HTHSCROLL;
                return w;
            }
        }
        *hit = HTCLIENT;
        if (w->style & WS_DISABLED)
            break;      /* (Wine's child_window_from_point) a disabled window: not its children */
    }
    return w == desktop ? NULL : w;
}

/* ---- scroll bars (window scroll bars and SCROLLBAR controls) ---- */

static Scroll *scroll_of(HWND h, int bar)
{
    Window *w = wnd_get(h);
    if (!w)
        return NULL;
    if (bar == SB_CTL)
        return &w->hscroll;            /* a SCROLLBAR control keeps its state here */
    return bar == SB_VERT ? &w->vscroll : &w->hscroll;
}

static void scroll_changed(HWND h, int bar)
{
    Window *w = wnd_get(h);
    if (!w)
        return;
    if (bar == SB_CTL)
        InvalidateRect(h, NULL, FALSE);
    else
        wnd_nc_paint(w);
}

/* SB_CTL: the control is asked with SBM_* messages, as in Wine. */
static int ctl_set_info(HWND h, UINT mask, int min, int max, int pos, WINBOOL redraw)
{
    SCROLLINFO si;
    memset(&si, 0, sizeof si);
    si.cbSize = sizeof si;
    si.fMask = mask;
    si.nMin = min;
    si.nMax = max;
    si.nPos = pos;
    return (int)SendMessageA(h, SBM_SETSCROLLINFO, (WPARAM)redraw, (LPARAM)&si);
}

int WINAPI SetScrollPos(HWND h, int bar, int pos, WINBOOL redraw)
{
    if (bar == SB_CTL) {
        Window *w = wnd_get(h);
        int old = w ? w->hscroll.pos : 0;
        ctl_set_info(h, SIF_POS, 0, 0, pos, redraw);
        return old;
    }
    API_TRACE();
    Scroll *s = scroll_of(h, bar);
    int old;
    if (!s)
        return 0;
    old = s->pos;
    if (pos < s->min) pos = s->min;
    if (pos > s->max) pos = s->max;
    s->pos = pos;
    if (redraw)
        scroll_changed(h, bar);
    return old;
}

int WINAPI GetScrollPos(HWND h, int bar)
{
    if (bar == SB_CTL)
        return (int)SendMessageA(h, SBM_GETPOS, 0, 0);
    API_TRACE();
    Scroll *s = scroll_of(h, bar);
    return s ? s->pos : 0;
}

WINBOOL WINAPI SetScrollRange(HWND h, int bar, int lo, int hi, WINBOOL redraw)
{
    if (bar == SB_CTL) {
        ctl_set_info(h, SIF_RANGE, lo, hi, 0, redraw);
        return TRUE;
    }
    API_TRACE();
    Scroll *s = scroll_of(h, bar);
    if (!s)
        return FALSE;
    s->min = lo;
    s->max = hi;
    if (s->pos < lo) s->pos = lo;
    if (s->pos > hi) s->pos = hi;
    if (redraw)
        scroll_changed(h, bar);
    return TRUE;
}

WINBOOL WINAPI GetScrollRange(HWND h, int bar, LPINT lo, LPINT hi)
{
    if (bar == SB_CTL) {
        SendMessageA(h, SBM_GETRANGE, (WPARAM)lo, (LPARAM)hi);
        return TRUE;
    }
    API_TRACE();
    Scroll *s = scroll_of(h, bar);
    if (!s)
        return FALSE;
    *lo = s->min;
    *hi = s->max;
    return TRUE;
}

WINBOOL WINAPI ShowScrollBar(HWND h, int bar, WINBOOL show)
{
    API_TRACE();
    Window *w = wnd_get(h);
    DWORD bits = 0;
    if (!w)
        return FALSE;
    if (bar == SB_CTL)
        return ShowWindow(h, show ? SW_SHOW : SW_HIDE), TRUE;
    if (bar == SB_VERT || bar == SB_BOTH) bits |= WS_VSCROLL;
    if (bar == SB_HORZ || bar == SB_BOTH) bits |= WS_HSCROLL;
    if (show)
        w->style |= bits;
    else
        w->style &= ~bits;
    swp(w, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    return TRUE;
}

/* ---- non-client painting ----
 * As Wine on a desktop without a window manager: the frame and caption of
 * windows the manager would decorate (top-level windows with a caption) are
 * not drawn; Wine draws the frame of the others (child windows, popups
 * without a caption) itself, and the menu bar and scroll bars of all. */

static int frame_drawn(Window *w)
{
    return (w->style & WS_CHILD) || !w->managed;
}

/* Wine's x11drv is_window_managed(), for a window being shown. */
static int is_managed(Window *w, int activating)
{
    Window *s;
    if ((w->style & WS_CHILD) || plat_normal_desktop)
        return 0;           /* (a normal desktop: the platform decorates every window) */
    if (activating || w->hwnd == active_wnd)
        return 1;
    if ((w->style & WS_CAPTION) == WS_CAPTION || (w->style & WS_THICKFRAME))
        return 1;
    if ((w->style & WS_POPUP) && (w->style & WS_SYSMENU))
        return 1;
    if (w->exstyle & WS_EX_APPWINDOW)
        return 1;
    for (s = desktop->children; s; s = s->next)
        if (s->owner == w && (s->style & WS_POPUP))
            return 1;
    return 0;
}

/* Mark w and its descendants for non-client repaint. */
static void nc_dirty_tree(Window *w)
{
    Window *c;
    if (wnd_has_nc(w))
        w->erase |= 2;
    for (c = w->children; c; c = c->next)
        nc_dirty_tree(c);
}

int wnd_has_nc(Window *w)
{
    return (!(w->style & WS_CHILD) && w->menu && !plat_native_menu) || (w->style & (WS_VSCROLL | WS_HSCROLL)) ||
           (frame_drawn(w) && ((w->style & (WS_BORDER | WS_DLGFRAME | WS_THICKFRAME)) ||
                               (w->exstyle & (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE))));
}

static void px_fill(DC *dc, int l, int t, int r, int b, COLORREF c)
{
    RECT x;
    x.left = l; x.top = t; x.right = r; x.bottom = b;
    if (!rect_empty(&x))
        dc_fill_rect(dc, &x, c);
}

/* DrawEdge(BF_RECT | BF_ADJUST) of one 1-pixel border, Wine's way: left and
 * top in the light colour, then bottom and right (over the corners) dark. */
static void edge_line(DC *dc, RECT *r, int lt, int rb)
{
    px_fill(dc, r->left, r->top + 1, r->left + 1, r->bottom, GetSysColor(lt));
    px_fill(dc, r->left, r->top, r->right, r->top + 1, GetSysColor(lt));
    px_fill(dc, r->left, r->bottom - 1, r->right, r->bottom, GetSysColor(rb));
    px_fill(dc, r->right - 1, r->top, r->right, r->bottom, GetSysColor(rb));
    InflateRect(r, -1, -1);
}

static void frame_band(DC *dc, RECT *r, int w, int h, COLORREF c)
{
    px_fill(dc, r->left, r->top, r->right, r->top + h, c);
    px_fill(dc, r->left, r->bottom - h, r->right, r->bottom, c);
    px_fill(dc, r->left, r->top + h, r->left + w, r->bottom - h, c);
    px_fill(dc, r->right - w, r->top + h, r->right, r->bottom - h, c);
    InflateRect(r, -w, -h);
}

/* A caption button: raised, or sunken while pressed. */
static void nc_button(DC *dc, RECT b, int pressed)
{
    px_fill(dc, b.left, b.top, b.right, b.bottom, GetSysColor(COLOR_3DFACE));
    if (pressed) {
        edge_line(dc, &b, COLOR_3DDKSHADOW, COLOR_3DLIGHT);
        edge_line(dc, &b, COLOR_3DSHADOW, COLOR_3DHILIGHT);
    } else {
        edge_line(dc, &b, COLOR_3DLIGHT, COLOR_3DDKSHADOW);
        edge_line(dc, &b, COLOR_3DHILIGHT, COLOR_3DSHADOW);
    }
}

/* A normal desktop's caption: the title on the caption colour; close,
 * maximize/restore and minimize buttons as the style asks. */
static void nc_caption(Window *w, HDC hdc, DC *dc, RECT *r)
{
    int h = GetSystemMetrics(SM_CYCAPTION) - 1, bw = GetSystemMetrics(SM_CXSIZE) - 2, i;
    Font *f = obj_get(plat_menu_font(), OBJ_FONT);
    RECT c = *r;
    c.bottom = c.top + h;
    memset(w->nc_btn, 0, sizeof w->nc_btn);
    px_fill(dc, c.left, c.top, c.right, c.bottom, GetSysColor(w->nc_active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
    px_fill(dc, c.left, c.bottom, c.right, c.bottom + 1, GetSysColor(COLOR_3DFACE));
    if (w->style & WS_SYSMENU) {
        RECT b;
        int cx, cy, d = w->nc_pressed == HTCLOSE;
        b.right = c.right - 2;
        b.left = b.right - bw;
        b.top = c.top + 2;
        b.bottom = c.bottom - 2;
        c.right = b.left - 2;
        w->nc_btn[0] = b;
        nc_button(dc, b, d);
        cx = (b.left + b.right) / 2 + d;
        cy = (b.top + b.bottom) / 2 + d;
        for (i = -3; i <= 3; i++) {         /* the X */
            px_fill(dc, cx + i - 1, cy + i, cx + i + 1, cy + i + 1, 0);
            px_fill(dc, cx + i - 1, cy - i, cx + i + 1, cy - i + 1, 0);
        }
        if (w->style & (WS_MAXIMIZEBOX | WS_MINIMIZEBOX)) {
            /* maximize (restore when maximized), then minimize to its left;
               the one the style lacks is drawn grey */
            int x, y, gmax = GetSysColor(w->style & WS_MAXIMIZEBOX ? COLOR_BTNTEXT : COLOR_GRAYTEXT);
            int gmin = GetSysColor(w->style & WS_MINIMIZEBOX ? COLOR_BTNTEXT : COLOR_GRAYTEXT);
            b.right = c.right;
            b.left = b.right - bw;
            c.right = b.left;
            w->nc_btn[1] = b;
            d = w->nc_pressed == HTMAXBUTTON;
            nc_button(dc, b, d);
            x = b.left + (bw - 9) / 2 + d;
            y = b.top + 2 + d;
            if (w->maximized) {             /* two overlapping windows */
                px_fill(dc, x + 2, y, x + 9, y + 2, gmax);
                px_fill(dc, x + 8, y + 2, x + 9, y + 5, gmax);
                px_fill(dc, x, y + 3, x + 7, y + 5, gmax);
                px_fill(dc, x, y + 5, x + 1, y + 9, gmax);
                px_fill(dc, x + 6, y + 5, x + 7, y + 9, gmax);
                px_fill(dc, x, y + 8, x + 7, y + 9, gmax);
            } else {                        /* a window */
                px_fill(dc, x, y, x + 9, y + 2, gmax);
                px_fill(dc, x, y + 2, x + 1, y + 9, gmax);
                px_fill(dc, x + 8, y + 2, x + 9, y + 9, gmax);
                px_fill(dc, x, y + 8, x + 9, y + 9, gmax);
            }
            b.right = c.right;
            b.left = b.right - bw;
            c.right = b.left - 2;
            w->nc_btn[2] = b;
            d = w->nc_pressed == HTMINBUTTON;
            nc_button(dc, b, d);
            x = b.left + (bw - 9) / 2 + d;
            px_fill(dc, x + 1, b.bottom - 5 + d, x + 7, b.bottom - 3 + d, gmin);
        }
    }
    if (w->text && f) {
        SelectObject(hdc, plat_menu_font());
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, GetSysColor(w->nc_active ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT));
        text_draw(dc, c.left + 4, c.top + (h - text_height(f->face)) / 2, w->text, (int)strlen(w->text), &c);
    }
    r->top += h + 1;
}

static void nc_frame(Window *w)
{
    HDC hdc = GetWindowDC(w->hwnd);     /* window coordinates */
    DC *dc = dc_get(hdc);
    RECT r;
    DWORD st = w->style, ex = w->exstyle;
    r.left = r.top = 0;
    r.right = w->rect.right - w->rect.left;
    r.bottom = w->rect.bottom - w->rect.top;
    if ((ex & WS_EX_STATICEDGE) && !(st & (WS_DLGFRAME | WS_THICKFRAME)))
        edge_line(dc, &r, COLOR_3DSHADOW, COLOR_3DHILIGHT);
    else if ((st & (WS_THICKFRAME | WS_DLGFRAME)) || (ex & WS_EX_DLGMODALFRAME)) {
        edge_line(dc, &r, COLOR_3DLIGHT, COLOR_3DDKSHADOW);      /* EDGE_RAISED */
        edge_line(dc, &r, COLOR_3DHILIGHT, COLOR_3DSHADOW);
    }
    if (st & WS_THICKFRAME)
        frame_band(dc, &r, GetSystemMetrics(SM_CXFRAME) - GetSystemMetrics(SM_CXDLGFRAME),
                   GetSystemMetrics(SM_CYFRAME) - GetSystemMetrics(SM_CYDLGFRAME),
                   GetSysColor(w->hwnd == active_wnd ? COLOR_ACTIVEBORDER : COLOR_INACTIVEBORDER));
    if ((st & (WS_BORDER | WS_DLGFRAME)) || (ex & WS_EX_DLGMODALFRAME)) {
        int c = (ex & (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE)) ? COLOR_3DFACE :
                (ex & WS_EX_STATICEDGE) ? COLOR_WINDOWFRAME :
                (st & (WS_DLGFRAME | WS_THICKFRAME)) ? COLOR_3DFACE : COLOR_WINDOWFRAME;
        frame_band(dc, &r, GetSystemMetrics(SM_CXDLGFRAME) - GetSystemMetrics(SM_CXEDGE),
                   GetSystemMetrics(SM_CYDLGFRAME) - GetSystemMetrics(SM_CYEDGE), GetSysColor(c));
    }
    if ((st & WS_CAPTION) == WS_CAPTION && plat_normal_desktop)
        nc_caption(w, hdc, dc, &r);
    if (ex & WS_EX_CLIENTEDGE) {
        RECT c;
        POINT o;
        o.x = w->client.left;
        o.y = w->client.top;
        c.left = o.x - GetSystemMetrics(SM_CXEDGE);
        c.top = o.y - GetSystemMetrics(SM_CYEDGE);
        c.right = o.x + (w->client.right - w->client.left) + GetSystemMetrics(SM_CXEDGE) +
                  ((st & WS_VSCROLL) ? GetSystemMetrics(SM_CXVSCROLL) : 0);
        c.bottom = o.y + (w->client.bottom - w->client.top) + GetSystemMetrics(SM_CYEDGE) +
                   ((st & WS_HSCROLL) ? GetSystemMetrics(SM_CYHSCROLL) : 0);
        edge_line(dc, &c, COLOR_3DSHADOW, COLOR_3DHILIGHT);       /* EDGE_SUNKEN */
        edge_line(dc, &c, COLOR_3DDKSHADOW, COLOR_3DLIGHT);
    }
    ReleaseDC(w->hwnd, hdc);
}

void wnd_nc_paint(Window *w)
{
    if (getenv("CIV2_LOG_NC"))
        plat_log("nc_paint %x style %08x visible %d frame %d\n", (unsigned)w->hwnd, (unsigned)w->style, wnd_is_visible(w),
                 frame_drawn(w));
    if (!wnd_is_visible(w))
        return;
    if (frame_drawn(w))
        nc_frame(w);
    if (!(w->style & WS_CHILD) && w->menu && !plat_native_menu)
        menu_bar_paint(w);
    if (w->style & (WS_VSCROLL | WS_HSCROLL))
        scroll_paint_nc(w);
}

/* ---- normal desktop: the caption buttons, moving and sizing ---- */

static int lesser(int a, int b) { return a < b ? a : b; }
static int greater(int a, int b) { return a > b ? a : b; }

int wnd_nc_button(Window *w, POINT pt)
{
    static const int hits[3] = {HTCLOSE, HTMAXBUTTON, HTMINBUTTON};
    RECT r;
    int i;
    wnd_screen_rect(w, &r);
    pt.x -= r.left;
    pt.y -= r.top;
    for (i = 0; i < 3; i++)
        if (w->nc_btn[i].right > w->nc_btn[i].left && PtInRect(&w->nc_btn[i], pt))
            return hits[i];
    return 0;
}

static int nc_button_enabled(Window *w, int hit)
{
    return hit == HTCLOSE || (hit == HTMAXBUTTON && (w->style & WS_MAXIMIZEBOX)) ||
           (hit == HTMINBUTTON && (w->style & WS_MINIMIZEBOX));
}

/* The modal loop of DefWindowProc's WM_NCLBUTTONDOWN (Windows' move/size
 * loop, with the window following the pointer): runs until the button is
 * released; other messages are dispatched meanwhile, so the windows keep
 * painting.  Escape puts the window back. */
void wnd_nc_track(Window *w, int hit)
{
    HWND h = w->hwnd;
    POINT start, pt;
    RECT r0, r;
    MSG msg;
    int pressed = 0, done = 0;
    MINMAXINFO mmi;
    if ((hit == HTMINBUTTON || hit == HTMAXBUTTON || hit == HTCLOSE) && !nc_button_enabled(w, hit))
        return;
    GetCursorPos(&start);
    r0 = w->rect;
    get_minmax(w, &mmi);
    if (hit == HTMINBUTTON || hit == HTMAXBUTTON || hit == HTCLOSE) {
        w->nc_pressed = hit;
        pressed = 1;
        wnd_nc_paint(w);
    }
    SetCapture(h);
    while (!done && wnd_get(h)) {
        if (!GetMessageA(&msg, NULL, 0, 0)) {
            PostQuitMessage((int)msg.wParam);
            break;
        }
        switch (msg.message) {
        case WM_MOUSEMOVE: case WM_NCMOUSEMOVE:
            GetCursorPos(&pt);
            if (w->nc_pressed || pressed) {
                int over = wnd_nc_button(w, pt) == hit ? hit : 0;
                if (over != w->nc_pressed) {
                    w->nc_pressed = over;
                    wnd_nc_paint(w);
                }
                break;
            }
            r = r0;
            if (hit == HTCAPTION)
                OffsetRect(&r, pt.x - start.x, pt.y - start.y);
            else {
                int dx = pt.x - start.x, dy = pt.y - start.y;
                if (hit == HTLEFT || hit == HTTOPLEFT || hit == HTBOTTOMLEFT)
                    r.left = lesser(r0.left + dx, r0.right - mmi.ptMinTrackSize.x);
                if (hit == HTRIGHT || hit == HTTOPRIGHT || hit == HTBOTTOMRIGHT)
                    r.right = greater(r0.right + dx, r0.left + mmi.ptMinTrackSize.x);
                if (hit == HTTOP || hit == HTTOPLEFT || hit == HTTOPRIGHT)
                    r.top = lesser(r0.top + dy, r0.bottom - mmi.ptMinTrackSize.y);
                if (hit == HTBOTTOM || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT)
                    r.bottom = greater(r0.bottom + dy, r0.top + mmi.ptMinTrackSize.y);
            }
            if (memcmp(&r, &w->rect, sizeof r))
                SetWindowPos(h, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top,
                             SWP_NOZORDER | SWP_NOACTIVATE | (hit == HTCAPTION ? SWP_NOSIZE : 0));
            break;
        case WM_LBUTTONUP: case WM_NCLBUTTONUP:
            done = 1;
            break;
        case WM_KEYDOWN: case WM_SYSKEYDOWN:
            if (msg.wParam == VK_ESCAPE) {
                if (!pressed)
                    SetWindowPos(h, NULL, r0.left, r0.top, r0.right - r0.left, r0.bottom - r0.top,
                                 SWP_NOZORDER | SWP_NOACTIVATE);
                w->nc_pressed = 0;
                done = 1;
            }
            break;
        default:
            if ((msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) ||
                (msg.message >= WM_NCMOUSEMOVE && msg.message <= WM_NCMBUTTONDBLCLK) ||
                (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST))
                break;      /* input belongs to the loop */
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
    ReleaseCapture();
    if (!wnd_get(h) || !pressed)
        return;
    hit = w->nc_pressed;
    w->nc_pressed = 0;
    wnd_nc_paint(w);
    if (hit == HTCLOSE)
        SendMessageA(h, WM_SYSCOMMAND, SC_CLOSE, 0);
    else if (hit == HTMAXBUTTON)
        SendMessageA(h, WM_SYSCOMMAND, w->maximized ? SC_RESTORE : SC_MAXIMIZE, 0);
    else if (hit == HTMINBUTTON)
        SendMessageA(h, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}
