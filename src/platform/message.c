/*
 * Messages: the queue, sending, posting, default handling, keyboard and
 * mouse input, timers, the clock.
 *
 * Retrieval order is Windows': posted messages (input included, in arrival
 * order), then WM_PAINT for invalid windows.  Window timers post WM_TIMER
 * when due, as the harness's replay layer makes them do in the original.
 * With CIV2_SCRIPT set the clock is virtual (see replay.c).
 */
#include "plat.h"
#ifndef __wasm__
#include <dlfcn.h>
#include <execinfo.h>
#endif
#include "../host/host.h"
#include <time.h>

/* ---- queue ---- */

/* Posted messages pile up when the game busy-waits without retrieving
 * them (its window timers' WM_TIMERs during the long dissolves of the end
 * screens: thousands).  Wine keeps them all; a smaller queue that dropped
 * some changed the order the backlog was retrieved in. */
#define QMAX 65536
static MSG queue[QMAX];
static int qhead, qlen;
static DWORD msg_time;

static void queue_remove(int i)
{
    int j;
    if (i == 0) {
        qhead = (qhead + 1) % QMAX;
        qlen--;
        return;
    }
    for (j = i; j < qlen - 1; j++)
        queue[(qhead + j) % QMAX] = queue[(qhead + j + 1) % QMAX];
    qlen--;
}

/* Keyboard input is queued without a window: as in Wine's server, it goes
 * to the window that has the focus when the message is retrieved. */
#define KBD_PENDING ((HWND)0xfffffffeu)
/* Mouse input likewise: the window under the pointer (or with the capture)
 * when retrieved, after WM_NCHITTEST and WM_SETCURSOR (process_mouse_message). */
#define MOUSE_PENDING ((HWND)0xfffffffdu)
static int process_mouse(MSG *m, HWND h, UINT lo, UINT hi);
static Window *mouse_target(const MSG *m);
static void mouse_message(UINT client_msg, UINT nc_msg);

void msg_post(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    MSG *q;
    if (qlen == QMAX) {
        plat_log("message queue full, dropping %04x\n", m);
        return;
    }
    q = &queue[(qhead + qlen++) % QMAX];
    q->hwnd = h;
    q->message = m;
    q->wParam = wp;
    q->lParam = lp;
    q->time = msg_time = plat_now();
    q->pt.x = q->pt.y = 0;
}

void msg_window_destroyed(HWND h)
{
    int i;
    for (i = 0; i < qlen; i++)
        if (queue[(qhead + i) % QMAX].hwnd == h)
            queue[(qhead + i) % QMAX].message = WM_NULL;    /* skipped on retrieval */
}

void WINAPI PostQuitMessage(int code)
{
    API_TRACE();
    msg_post(0, WM_QUIT, (WPARAM)code, 0);
}

WINBOOL WINAPI PostMessageA(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    API_TRACE();
    if (h && !wnd_get(h) && h != HWND_BROADCAST)
        return FALSE;
    msg_post(h, m, wp, lp);
    return TRUE;
}

static int matches(const MSG *m, HWND h, UINT lo, UINT hi)
{
    if (h && m->hwnd != h && !IsChild(h, m->hwnd))
        return 0;
    if ((lo || hi) && (m->message < lo || m->message > hi))
        return 0;
    return 1;
}

/* The message as it would be retrieved now (keyboard input routed). */
static int resolve(const MSG *m, MSG *out)
{
    *out = *m;
    if (m->hwnd == MOUSE_PENDING) {
        Window *w = mouse_target(m);
        if (!w)
            return 0;
        out->hwnd = w->hwnd;
        return 1;
    }
    if (m->hwnd != KBD_PENDING)
        return 1;
    if (GetFocus()) {
        out->hwnd = GetFocus();
        return 1;
    }
    if (wnd_get(active_wnd)) {
        /* No focus: the active window gets it as a system key. */
        out->hwnd = active_wnd;
        if (m->message == WM_KEYDOWN) out->message = WM_SYSKEYDOWN;
        else if (m->message == WM_KEYUP) out->message = WM_SYSKEYUP;
        return 1;
    }
    return 0;           /* nobody to receive it */
}

static void key_retrieved(const MSG *m);
static void mouse_retrieved(UINT msg);
static int kbd_hook_call(UINT remove, const MSG *m);

/* As Windows: posted messages first, then input (keyboard, mouse), each
 * in order of arrival. */
static int take(MSG *out, HWND h, UINT lo, UINT hi, UINT remove)
{
    int i, input;
    for (input = 0; input < 2; input++)
    for (i = 0; i < qlen; i++) {
        MSG *m = &queue[(qhead + i) % QMAX], r;
        if (m->message == WM_NULL && m->hwnd) {         /* dropped: window destroyed */
            if (i == 0) {
                qhead = (qhead + 1) % QMAX;
                qlen--;
                i--;
            }
            continue;
        }
        if ((m->hwnd == KBD_PENDING || m->hwnd == MOUSE_PENDING) != input)
            continue;
        if (!resolve(m, &r) || !matches(&r, h, lo, hi))
            continue;
        if (m->hwnd == KBD_PENDING && kbd_hook_call(remove, m)) {
            /* the hook swallowed it (Wine: whether or not removing) */
            queue_remove(i);
            i--;
            continue;
        }
        if (m->hwnd == MOUSE_PENDING && (remove & PM_REMOVE)) {
            MSG raw = *m;
            UINT button_msg = m->message;
            int got = process_mouse(&raw, h, lo, hi);
            if (got < 0)
                continue;       /* (Wine) not what the filter asks for once hit-tested: stays queued */
            queue_remove(i);
            mouse_retrieved(button_msg);
            if (!got) {
                i--;            /* swallowed: look further */
                continue;
            }
            *out = raw;
            return 1;
        }
        *out = r;
        if (remove & PM_REMOVE) {
            if (m->hwnd == KBD_PENDING)
                key_retrieved(m);
            queue_remove(i);
        }
        return 1;
    }
    return 0;
}

Window *wnd_next_dirty(Window *root);

static int paint_message(MSG *out, HWND h, UINT lo, UINT hi)
{
    Window *w;
    if ((lo || hi) && (WM_PAINT < lo || WM_PAINT > hi))
        return 0;
    w = wnd_next_dirty(h ? wnd_get(h) : desktop);
    if (!w)
        return 0;
    if (rgn_empty(&w->update)) {
        /* Only the non-client area needs painting (BeginPaint does it otherwise). */
        w->erase &= ~2;
        wnd_send(w, WM_NCPAINT, 1, 0);
        return paint_message(out, h, lo, hi);
    }
    memset(out, 0, sizeof *out);
    out->hwnd = w->hwnd;
    out->message = WM_PAINT;
    out->time = plat_now();
    return 1;
}

/* CIV2_TRACE_MSG: as the hook runtime's (hook/replay.c), for comparison. */
static int trace_msg = -1;

static int trace_on(void)
{
    if (trace_msg < 0)
        trace_msg = getenv("CIV2_TRACE_MSG") != NULL;
    return trace_msg;
}
static HWND seen_hwnd[512];
static int nseen;

static int hwnd_index(HWND h)
{
    int i;
    if (!h)
        return 0;
    for (i = nseen - 1; i >= 0; i--)
        if (seen_hwnd[i] == h)
            return i + 1;
    return -1;
}

int msg_trace_internal;        /* windows our controls make for themselves: not numbered */

/* Windows are numbered in creation order (CreateWindowExA calls this), as
 * the hook numbers those the game creates. */
void msg_trace_window_created(HWND h, const char *cls)
{
    if (msg_trace_internal)
        return;
    if (trace_msg < 0)
        trace_msg = getenv("CIV2_TRACE_MSG") != NULL;
    if (trace_msg > 0)
        plat_log("create w%d %s\n", nseen + 1, cls);
    if (nseen < 512)
        seen_hwnd[nseen++] = h;
}

int msg_count;                  /* messages the game has retrieved (api_trace) */

/* CIV2_CLOCKLOG=FROM-TO: virtual clock events while the game retrieves
 * messages FROM to TO (harness/clockdiff.py; hook/replay.c logs the same). */
static int clk_win(void)
{
    static int lo = -2, hi;
    if (lo == -2) {
        const char *e = getenv("CIV2_CLOCKLOG");
        lo = -1;
        if (e)
            sscanf(e, "%d-%d", &lo, &hi);
    }
    return lo >= 0 && msg_count >= lo && msg_count <= hi;
}
#define CLK(...) do { if (clk_win()) plat_log(__VA_ARGS__); } while (0)

#define WM_KEYF1_ 0x004d

static WINBOOL log_msg(const MSG *m, UINT remove, UINT lo, UINT hi)
{
    if (remove & PM_REMOVE) {
        msg_count++;
        CLK("clk got %04x w%d wp=%x [%x-%x] #%d @%llu\n", m->message, hwnd_index(m->hwnd), (unsigned)m->wParam, lo, hi,
            msg_count, (unsigned long long)vclock_us);
        /* Wine: F1 pressed -> WM_KEYF1 (DefWindowProc makes it WM_HELP) */
        if (m->message == WM_KEYDOWN && m->wParam == VK_F1 && m->hwnd)
            PostMessageA(m->hwnd, WM_KEYF1_, 0, 0);
    }
    if (trace_msg < 0)
        trace_msg = getenv("CIV2_TRACE_MSG") != NULL;
    if (trace_msg && (remove & PM_REMOVE) && m->message != WM_TIMER)
        plat_log("msg %04x w%d wp=%x [%x-%x] #%d @%llu\n", m->message, hwnd_index(m->hwnd), (unsigned)m->wParam, lo, hi,
                 msg_count, (unsigned long long)vclock_us);
    return TRUE;
}

/* What plat_modal_tick may deliver meanwhile: 2 the sound's messages (the
   game waits for any message: no window or range given), 1 MM_WOM_DONE
   (the game dispatches those itself: win_dispatch_custom_msgs), 0 none. */
static int may_deliver;

void sound_poll(void);

static int deliverable(HWND h, UINT lo, UINT hi)
{
    return h ? 0 : !lo && !hi ? 2 : lo <= MM_WOM_DONE && hi >= MM_WOM_DONE ? 1 : 0;
}

/* While a menu of the system's menu bar is open (Windows, macOS: the host's
 * loop holds this one; host_modal_tick): as Windows' menu loop, which goes
 * on dispatching what was posted, the sound's messages delivered, so the
 * sound streamed in parts (the opening screen's MENULOOP) and the music go
 * on.  Only where the game would itself take them (may_deliver). */
void plat_modal_tick(void)
{
    static int busy;
    MSG m;
    if (busy || !may_deliver || plat_replay)
        return;
    busy = 1;
    sound_poll();
    while (take(&m, NULL, MM_WOM_DONE, MM_WOM_DONE, PM_REMOVE) ||
           (may_deliver == 2 && take(&m, NULL, MM_MCINOTIFY, MM_MCINOTIFY, PM_REMOVE)))
        DispatchMessageA(&m);
    busy = 0;
}

static WINBOOL peek_message(LPMSG msg, HWND h, UINT lo, UINT hi, UINT remove)
{
    msg_pump_events(0);
    xd_poll();      /* (the network library's messages, as its windows got them) */
    if (take(msg, h, lo, hi, remove) || paint_message(msg, h, lo, hi)) {
        GetCursorPos(&msg->pt);
        return log_msg(msg, remove, lo, hi);
    }
    /* Like Wine under a busy game, X events (window configuration, pointer
       crossings) are only processed once nothing else is pending. */
    wnd_host_events();
    pointer_check();
    if (take(msg, h, lo, hi, remove) || paint_message(msg, h, lo, hi)) {
        GetCursorPos(&msg->pt);
        return log_msg(msg, remove, lo, hi);
    }
    /* Idle: time passes, timers fire, the script may act. */
    plat_idle();
    plat_present();
    if (take(msg, h, lo, hi, remove) || paint_message(msg, h, lo, hi)) {
        GetCursorPos(&msg->pt);
        return log_msg(msg, remove, lo, hi);
    }
    return FALSE;
}

WINBOOL WINAPI PeekMessageA(LPMSG msg, HWND h, UINT lo, UINT hi, UINT remove)
{
    int was = may_deliver;
    WINBOOL r;
    API_TRACE();
    may_deliver = deliverable(h, lo, hi);
    r = peek_message(msg, h, lo, hi, remove);
    may_deliver = was;
    return r;
}

WINBOOL WINAPI GetMessageA(LPMSG msg, HWND h, UINT lo, UINT hi)
{
    API_TRACE();
    while (!PeekMessageA(msg, h, lo, hi, PM_REMOVE))
        if (!plat_replay) {
            int was = may_deliver;
            may_deliver = deliverable(h, lo, hi);
            msg_pump_events(1);     /* nothing to do: wait for input (at most 10 ms, for timers) */
            may_deliver = was;
        }
    return msg->message != WM_QUIT;
}

LRESULT WINAPI DispatchMessageA(const MSG *m)
{
    API_TRACE();
    Window *w = wnd_get(m->hwnd);
    if (m->message == WM_TIMER && m->lParam) {
        ((TIMERPROC)m->lParam)(m->hwnd, WM_TIMER, m->wParam, plat_now());
        return 0;
    }
    if (!w)
        return 0;
    /* The interface additions (docs/EXTRAS.md) see the mouse first: the
       wheel, the middle button, Shift or Ctrl with the mouse */
    if (plat_extras && ((m->message >= WM_MOUSEFIRST && m->message <= WM_MOUSELAST) ||
                        m->message == WM_KEYDOWN || m->message == WM_KEYUP) &&
        extras_input((void *)m))
        return 0;
    if (m->message == WM_PAINT) {
        LRESULT r = wnd_send(w, WM_PAINT, 0, 0);
        /* A window that does not validate would repaint forever. */
        if (wnd_get(m->hwnd) && !rgn_empty(&w->update)) {
            rgn_clear(&w->update);
            w->erase &= ~1;
        }
        return r;
    }
    return wnd_send(w, m->message, m->wParam, m->lParam);
}

/* ---- sending ---- */

static int send_depth;

static int is_builtin_proc(WNDPROC p)
{
    return p == ctl_edit_proc || p == ctl_listbox_proc || p == ctl_combobox_proc || p == ctl_scrollbar_proc ||
           p == ctl_button_proc || p == ctl_static_proc;
}

LRESULT wnd_send(Window *w, UINT m, WPARAM wp, LPARAM lp)
{
    LRESULT r;
    if (!w || !w->proc)
        return 0;
    if (trace_on() && m != WM_TIMER && !(w->cls && w->cls->builtin && is_builtin_proc(w->proc)))
        plat_log("deliver %04x w%d\n", m, hwnd_index(w->hwnd));
    if (++send_depth > 400)
        plat_fatal("message recursion (message %04x)", m);
    r = w->proc(w->hwnd, m, wp, lp);
    send_depth--;
    return r;
}

LRESULT WINAPI SendMessageA(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    API_TRACE();
    Window *w;
    if (h == HWND_BROADCAST) {
        for (w = desktop->children; w; w = w->next)
            wnd_send(w, m, wp, lp);
        return 0;
    }
    w = wnd_get(h);
    return w ? wnd_send(w, m, wp, lp) : 0;
}

LRESULT WINAPI CallWindowProcA(WNDPROC proc, HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    API_TRACE();
    return proc ? proc(h, m, wp, lp) : 0;
}

/* ---- default handling ---- */

typedef struct { UINT cbSize; int iContextType, iCtrlId; HANDLE hItemHandle; DWORD dwContextId; POINT MousePos; } HelpInfo;

LRESULT WINAPI DefWindowProcA(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    API_TRACE();
    Window *w = wnd_get(h);
    if (!w)
        return 0;
    switch (m) {
    case WM_NCCREATE:
        return TRUE;
    case WM_KEYF1_: {
        HelpInfo hi;
        memset(&hi, 0, sizeof hi);
        hi.cbSize = sizeof hi;
        hi.iContextType = 1;            /* HELPINFO_WINDOW */
        hi.iCtrlId = (int)(uintptr_t)w->menu;
        hi.hItemHandle = h;
        GetCursorPos(&hi.MousePos);
        return SendMessageA(h, WM_HELP, 0, (LPARAM)&hi);
    }
    case WM_HELP:
        if ((w->style & WS_CHILD) && w->parent && w->parent != desktop)
            return SendMessageA(w->parent->hwnd, WM_HELP, wp, lp);
        return 0;
    case WM_SETTEXT:
        free(w->text);
        w->text = plat_strdup((const char *)lp);
        return TRUE;
    case WM_GETTEXT:
        if (wp) {
            snprintf((char *)lp, wp, "%s", w->text);
            return (LRESULT)strlen((char *)lp);
        }
        return 0;
    case WM_GETTEXTLENGTH:
        return (LRESULT)strlen(w->text);
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(h, &ps);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_ERASEBKGND: {
        HBRUSH br = w->cls ? w->cls->wc.hbrBackground : NULL;
        RECT r;
        if (!br)
            return 0;
        if ((uintptr_t)br <= COLOR_MENUBAR + 1)
            br = GetSysColorBrush((int)(uintptr_t)br - 1);
        GetClientRect(h, &r);
        FillRect((HDC)wp, &r, br);
        return 1;
    }
    case WM_CLOSE:
        DestroyWindow(h);
        return 0;
    case WM_CANCELMODE:
        /* Wine's DefWindowProc: a window disabled (EnableWindow sends
           this) gives up the mouse capture */
        if (GetCapture() == h)
            ReleaseCapture();
        return 0;
    case WM_SETCURSOR: {
        /* Wine: the parent first (outside the sizing border), then the
           class cursor of the window under the pointer (wParam) */
        Window *u = wnd_get((HWND)wp);
        if ((w->style & WS_CHILD) && (LOWORD(lp) < HTSIZEFIRST || LOWORD(lp) > HTSIZELAST) &&
            w->parent && w->parent != desktop && wnd_send(w->parent, WM_SETCURSOR, wp, lp))
            return TRUE;
        if ((short)LOWORD(lp) == HTCLIENT) {
            if (!u || !u->cls || !u->cls->wc.hCursor)
                return FALSE;
            SetCursor(u->cls->wc.hCursor);
            return TRUE;
        }
        switch ((short)LOWORD(lp)) {
        case HTLEFT: case HTRIGHT: SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_SIZEWE)); break;
        case HTTOP: case HTBOTTOM: SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_SIZENS)); break;
        case HTTOPLEFT: case HTBOTTOMRIGHT: SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_SIZENWSE)); break;
        case HTTOPRIGHT: case HTBOTTOMLEFT: SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_SIZENESW)); break;
        default: SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
        }
        return TRUE;
    }
    case WM_MOUSEACTIVATE:
        if ((w->style & WS_CHILD) && w->parent != desktop)
            return wnd_send(w->parent, WM_MOUSEACTIVATE, wp, lp);
        return MA_ACTIVATE;
    case WM_ACTIVATE:
        if (LOWORD(wp) != WA_INACTIVE && !w->minimized)
            SetFocus(h);
        return 0;
    case WM_SYSKEYDOWN:
        if (wp == VK_F4 && (HIWORD(lp) & KF_ALTDOWN))
            PostMessageA(h, WM_SYSCOMMAND, SC_CLOSE, 0);
        else if (!(w->style & WS_CHILD) || 1)
            menu_key(w, (int)wp);
        return 0;
    case WM_SYSKEYUP:
        return 0;
    case WM_SYSCHAR:
        return 0;
    case WM_SYSCOMMAND:
        switch (wp & 0xfff0) {
        case SC_CLOSE: wnd_send(w, WM_CLOSE, 0, 0); break;
        case SC_MINIMIZE:
            /* A normal desktop holds this program only, with no task bar
               to restore the main window from: the host window minimizes. */
            if (plat_normal_desktop && !(w->style & WS_CHILD) && !w->owner)
                host_minimize();
            else
                ShowWindow(h, SW_MINIMIZE);
            break;
        case SC_MAXIMIZE: ShowWindow(h, SW_MAXIMIZE); break;
        case SC_MOUSEMENU: menu_bar_click(w, (short)LOWORD(lp), (short)HIWORD(lp)); break;
        case SC_RESTORE: ShowWindow(h, SW_RESTORE); break;
        }
        return 0;
    case WM_CTLCOLORMSGBOX: case WM_CTLCOLOREDIT: case WM_CTLCOLORLISTBOX: case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG: case WM_CTLCOLORSCROLLBAR: case WM_CTLCOLORSTATIC:
        SetTextColor((HDC)wp, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor((HDC)wp, GetSysColor(m == WM_CTLCOLOREDIT || m == WM_CTLCOLORLISTBOX ? COLOR_WINDOW : COLOR_3DFACE));
        return (LRESULT)GetSysColorBrush(m == WM_CTLCOLOREDIT || m == WM_CTLCOLORLISTBOX ? COLOR_WINDOW : COLOR_3DFACE);
    case WM_WINDOWPOSCHANGED: {
        /* WM_MOVE and WM_SIZE for what changed */
        const WINDOWPOS *wp = (const WINDOWPOS *)lp;
        if (!(wp->flags & 0x1000))
            wnd_send(w, WM_MOVE, 0, MAKELPARAM(w->rect.left + w->client.left, w->rect.top + w->client.top));
        if (wnd_get(h) && (!(wp->flags & 0x0800) || (wp->flags & 0x8000)))
            wnd_send(w, WM_SIZE, w->maximized ? SIZE_MAXIMIZED : w->minimized ? SIZE_MINIMIZED : SIZE_RESTORED,
                     MAKELPARAM(w->client.right - w->client.left, w->client.bottom - w->client.top));
        return 0;
    }
    case WM_NCPAINT:
        wnd_nc_paint(w);
        return 0;
    case WM_NCLBUTTONDBLCLK:
        if (wp == HTVSCROLL || wp == HTHSCROLL) {
            scroll_track_nc(w, (int)wp);
            return 0;
        }
        if (plat_normal_desktop && wp == HTCAPTION && (w->style & WS_MAXIMIZEBOX))
            SendMessageA(h, WM_SYSCOMMAND, w->maximized ? SC_RESTORE : SC_MAXIMIZE, 0);
        else if (plat_normal_desktop)
            return DefWindowProcA(h, WM_NCLBUTTONDOWN, wp, lp);
        return 0;
    case WM_NCLBUTTONDOWN:
        if (wp == HTMENU) {
            /* NC_HandleNCLButtonDown: the menu bar tracks the mouse */
            SendMessageA(h, WM_SYSCOMMAND, SC_MOUSEMENU + HTMENU, lp);
            return 0;
        }
        if (wp == HTVSCROLL || wp == HTHSCROLL) {
            scroll_track_nc(w, (int)wp);
            return 0;
        }
        if (plat_normal_desktop) {
            /* drag the caption or a border, press a caption button;
               a double click on the caption maximizes or restores */
            static DWORD last_time;
            static HWND last_wnd;
            DWORD now = plat_now();
            int hit = (int)wp;
            if (hit == HTCAPTION && last_wnd == h && now - last_time <= 500 && (w->style & WS_MAXIMIZEBOX)) {
                last_wnd = 0;
                SendMessageA(h, WM_SYSCOMMAND, w->maximized ? SC_RESTORE : SC_MAXIMIZE, 0);
                return 0;
            }
            last_wnd = hit == HTCAPTION ? h : 0;
            last_time = now;
            if ((hit == HTCAPTION && !w->maximized) || (hit >= HTLEFT && hit <= HTBOTTOMRIGHT) ||
                hit == HTCLOSE || hit == HTMAXBUTTON || hit == HTMINBUTTON)
                wnd_nc_track(w, hit);
        }
        return 0;
    case WM_NCHITTEST: {
        /* screen point -> part of the window */
        POINT pt, o;
        RECT r;
        pt.x = (short)LOWORD(lp);
        pt.y = (short)HIWORD(lp);
        wnd_screen_rect(w, &r);
        if (pt.x < r.left || pt.x >= r.right || pt.y < r.top || pt.y >= r.bottom)
            return HTNOWHERE;
        wnd_client_screen(w, &o);
        if (pt.x >= o.x && pt.x < o.x + (w->client.right - w->client.left) && pt.y >= o.y &&
            pt.y < o.y + (w->client.bottom - w->client.top))
            return HTCLIENT;
        if ((w->style & WS_VSCROLL) && pt.x >= o.x + (w->client.right - w->client.left) && pt.y >= o.y &&
            pt.y < o.y + (w->client.bottom - w->client.top))
            return HTVSCROLL;
        if ((w->style & WS_HSCROLL) && pt.y >= o.y + (w->client.bottom - w->client.top) && pt.x >= o.x &&
            pt.x < o.x + (w->client.right - w->client.left))
            return HTHSCROLL;
        if (plat_normal_desktop && !w->minimized) {
            /* the sizing border (corners: a caption's height along each
               edge), then the caption buttons */
            int hb = wnd_nc_button(w, pt);
            if ((w->style & WS_THICKFRAME) && !w->maximized) {
                int fx = GetSystemMetrics(SM_CXFRAME), fy = GetSystemMetrics(SM_CYFRAME);
                int k = GetSystemMetrics(SM_CYCAPTION);
                int left = pt.x < r.left + fx, right = pt.x >= r.right - fx;
                int top = pt.y < r.top + fy, bottom = pt.y >= r.bottom - fy;
                if (left || right || top || bottom) {
                    if (top || bottom) {
                        left = pt.x < r.left + k;
                        right = pt.x >= r.right - k;
                    } else {
                        top = pt.y < r.top + k;
                        bottom = pt.y >= r.bottom - k;
                    }
                    return top ? (left ? HTTOPLEFT : right ? HTTOPRIGHT : HTTOP) :
                           bottom ? (left ? HTBOTTOMLEFT : right ? HTBOTTOMRIGHT : HTBOTTOM) :
                           left ? HTLEFT : HTRIGHT;
                }
            }
            if (hb)
                return hb;
        }
        if (!(w->style & WS_CHILD) && w->menu && pt.y < o.y && pt.y >= o.y - menu_bar_height(w))
            return HTMENU;
        if ((w->style & WS_CAPTION) == WS_CAPTION && pt.y < o.y)
            return HTCAPTION;
        return HTBORDER;
    }
    case WM_NCACTIVATE:
        if (plat_normal_desktop) {
            /* the caption shows whether the window is active */
            w->nc_active = wp != 0;
            if ((w->style & WS_CAPTION) == WS_CAPTION && wnd_is_visible(w))
                wnd_nc_paint(w);
        }
        return TRUE;
    case WM_GETMINMAXINFO:
    case WM_WINDOWPOSCHANGING:
    case WM_NCCALCSIZE:
    case WM_GETICON:
    case WM_PALETTEISCHANGING:
    case WM_IME_SETCONTEXT:
    case WM_IME_SELECT:
    case WM_CHILDACTIVATE:
    case WM_SHOWWINDOW:
        return 0;
    case WM_QUERYNEWPALETTE:
        return FALSE;
    }
    return 0;
}

/* ---- timers ---- */

typedef struct {
    int used, is_mm, periodic;
    UINT id;
    HWND hwnd;
    UINT_PTR wm_id;
    TIMERPROC proc;
    LPTIMECALLBACK cb;
    DWORD_PTR user;
    UINT period;
    unsigned long long due_us;
} Timer;

#define MAX_TIMERS 64
static Timer timers[MAX_TIMERS];
static UINT next_mm_id = 1;
static int in_timers;
unsigned long long vclock_us;          /* the clock, in microseconds since start */

static int timer_pending(HWND h, UINT_PTR id)
{
    int i;
    for (i = 0; i < qlen; i++) {
        const MSG *m = &queue[(qhead + i) % QMAX];
        if (m->message == WM_TIMER && m->hwnd == h && m->wParam == id)
            return 1;
    }
    return 0;
}

void timers_fire(void)
{
    int rounds;
    if (in_timers)
        return;
    in_timers = 1;
    /* Every tick that is due, in deadline order. */
    for (rounds = 0; rounds < 1000; rounds++) {
        Timer *t = NULL;
        int i;
        for (i = 0; i < MAX_TIMERS; i++)
            if (timers[i].used && timers[i].due_us <= vclock_us && (!t || timers[i].due_us < t->due_us))
                t = &timers[i];
        if (!t)
            break;
        if (!t->periodic)
            t->used = 0;
        else if (!plat_replay && !t->is_mm) {
            /* Normal play: Windows' WM_TIMER.  At most one is pending per
               timer and the next is a period after this one (missed ticks
               are dropped): a handler slower than its period that pumps
               messages (the unit blink's map redraw, in a network game)
               would otherwise find the backlog's next WM_TIMER at once and
               nest without end. */
            t->due_us = vclock_us + (unsigned long long)(t->period ? t->period : 1) * 1000;
            if (timer_pending(t->hwnd, t->wm_id))
                continue;
        }
        else
            t->due_us += (unsigned long long)(t->period ? t->period : 1) * 1000;
        CLK("clk timer %s %u @%llu\n", t->is_mm ? "mm" : "wm", t->is_mm ? t->id : (unsigned)t->wm_id, (unsigned long long)vclock_us);
        if (t->is_mm)
            t->cb(t->id, 0, t->user, 0, 0);
        else
            msg_post(t->hwnd, WM_TIMER, t->wm_id, (LPARAM)t->proc);
    }
    in_timers = 0;
}

void timers_window_destroyed(HWND h)
{
    int i;
    for (i = 0; i < MAX_TIMERS; i++)
        if (timers[i].used && !timers[i].is_mm && timers[i].hwnd == h)
            timers[i].used = 0;
}

UINT_PTR WINAPI SetTimer(HWND h, UINT_PTR id, UINT ms, TIMERPROC proc)
{
    API_TRACE();
    CLK("clk SetTimer w%d id=%u ms=%u @%llu\n", hwnd_index(h), (unsigned)id, ms, (unsigned long long)vclock_us);
    int i, slot = -1;
    for (i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].used && !timers[i].is_mm && timers[i].hwnd == h && timers[i].wm_id == id) {
            slot = i;
            break;
        }
        if (!timers[i].used && slot < 0)
            slot = i;
    }
    if (slot < 0)
        return 0;
    memset(&timers[slot], 0, sizeof timers[slot]);
    timers[slot].used = 1;
    timers[slot].hwnd = h;
    timers[slot].wm_id = h ? id : (UINT_PTR)(0x7000 + slot);
    timers[slot].proc = proc;
    timers[slot].period = ms;
    timers[slot].periodic = 1;
    timers[slot].due_us = vclock_us + (unsigned long long)ms * 1000;
    return timers[slot].wm_id;
}

WINBOOL WINAPI KillTimer(HWND h, UINT_PTR id)
{
    API_TRACE();
    int i;
    CLK("clk KillTimer w%d id=%u @%llu\n", hwnd_index(h), (unsigned)id, (unsigned long long)vclock_us);
    for (i = 0; i < MAX_TIMERS; i++)
        if (timers[i].used && !timers[i].is_mm && timers[i].hwnd == h && timers[i].wm_id == id) {
            timers[i].used = 0;
            return TRUE;
        }
    return FALSE;
}

MMRESULT WINAPI timeSetEvent(UINT delay, UINT res, LPTIMECALLBACK cb, DWORD_PTR user, UINT flags)
{
    API_TRACE();
    int i;
    (void)res;
    for (i = 0; i < MAX_TIMERS; i++)
        if (!timers[i].used) {
            Timer *t = &timers[i];
            memset(t, 0, sizeof *t);
            t->used = 1;
            t->is_mm = 1;
            t->id = next_mm_id++;
            t->cb = cb;
            t->user = user;
            t->period = delay;
            t->periodic = (flags & TIME_PERIODIC) != 0;
            t->due_us = vclock_us + (unsigned long long)delay * 1000;
            return t->id;
        }
    return 0;
}

MMRESULT WINAPI timeKillEvent(UINT id)
{
    API_TRACE();
    int i;
    for (i = 0; i < MAX_TIMERS; i++)
        if (timers[i].used && timers[i].is_mm && timers[i].id == id) {
            timers[i].used = 0;
            return TIMERR_NOERROR;
        }
    return MMSYSERR_INVALPARAM;
}

MMRESULT WINAPI timeBeginPeriod(UINT p) { API_TRACE(); (void)p; return TIMERR_NOERROR; }
MMRESULT WINAPI timeEndPeriod(UINT p) { API_TRACE(); (void)p; return TIMERR_NOERROR; }

/* ---- clock ---- */

#define IDLE_STEP_MS 10         /* virtual time per idle message check */
#define QUERY_STEP_US 50        /* virtual time per clock query */
#define CLOCK_BASE_MS 1000000u

int plat_replay;
int plat_normal_desktop = 1;
int plat_lock_panels;
int plat_fix_bugs;
int plat_extras;
int plat_scale = 1;
int plat_fullscreen;
static uint64_t real_start;

/* CIV2_REALTIME: a replay script on the real clock (its waits are real
 * milliseconds): tests of what only happens in real time.  Not
 * deterministic; the virtual clock is the default. */
static int virtual_clock(void)
{
    static int realtime = -1;
    if (realtime < 0)
        realtime = getenv("CIV2_REALTIME") && *getenv("CIV2_REALTIME");
    return plat_replay && !realtime;
}

static void sync_real_clock(void)
{
    API_TRACE();
    if (!virtual_clock()) {
        unsigned long long now = host_time_us() - real_start;
        if (now > vclock_us)
            vclock_us = now;
    }
}

/* The game's clock queries (timeGetTime, GetTickCount).  In replay mode
 * each one lets a little virtual time pass (the game busy-waits on them)
 * without firing timers, exactly as the harness's hook runtime does for
 * the original. */
DWORD plat_ticks(void)
{
    if (virtual_clock())
        vclock_us += QUERY_STEP_US;
    else {
        sync_real_clock();
        timers_fire();
    }
    return CLOCK_BASE_MS + (DWORD)(vclock_us / 1000);
}

/* The platform's own reads: no virtual time passes. */
DWORD plat_now(void)
{
    if (!virtual_clock())
        sync_real_clock();
    return CLOCK_BASE_MS + (DWORD)(vclock_us / 1000);
}

DWORD WINAPI timeGetTime(void) { API_TRACE(); return plat_ticks(); }
DWORD WINAPI GetTickCount(void)
{
    API_TRACE();
#if !defined(__wasm__)
    if (clk_win() && getenv("CIV2_TICKCALLERS")) {
        Dl_info d;
        void *f[24];
        char line[1024];
        int n = backtrace(f, 24), x, len = 0;
        for (x = 3; x < n && x < 20; x++)
            if (dladdr(f[x], &d))
                len += snprintf(line + len, sizeof line - (size_t)len, " < %s@%p", d.dli_sname ? d.dli_sname : "?", f[x]);
        plat_log("tick @%llu%s\n", (unsigned long long)vclock_us, line);
    }
#endif
    return plat_ticks();
}

void WINAPI Sleep(DWORD ms)
{
    API_TRACE();
    if (virtual_clock()) {
        vclock_us += (unsigned long long)ms * 1000;
        CLK("clk advance %llu @%llu\n", (unsigned long long)ms * 1000, (unsigned long long)vclock_us);
    } else {
        host_delay_ms(ms);
        sync_real_clock();
    }
    timers_fire();
}

void plat_idle(void)
{
    if (virtual_clock()) {
        vclock_us += IDLE_STEP_MS * 1000;
        CLK("clk advance %llu @%llu\n", (unsigned long long)IDLE_STEP_MS * 1000, (unsigned long long)vclock_us);
        timers_fire();
        replay_run_script();
    } else {
        /* PeekMessage does not wait, as on Windows: the game runs work such
         * as building the world as its idle task, a small step per empty
         * peek.  A short sleep now and then keeps its idle loop from taking
         * a whole core. */
        static int empty;
        if (++empty >= 100) {
            empty = 0;
            host_delay_ms(1);
        }
        msg_pump_events(0);
        sync_real_clock();
        timers_fire();
        if (plat_replay)
            replay_run_script();
    }
}

int64_t plat_time(void)
{
    if (plat_replay)
        return 922449600LL + (long long)(vclock_us / 1000000);     /* 1999-03-26 12:00 UTC */
    return (long long)time(NULL);
}

void WINAPI GetLocalTime(LPSYSTEMTIME st)
{
    API_TRACE();
    memset(st, 0, sizeof *st);
    if (plat_replay) {
        unsigned long long s = vclock_us / 1000000;
        st->wYear = 1999; st->wMonth = 3; st->wDay = 26; st->wDayOfWeek = 5;
        st->wHour = (WORD)(12 + (s / 3600) % 12);
        st->wMinute = (WORD)((s / 60) % 60);
        st->wSecond = (WORD)(s % 60);
        st->wMilliseconds = (WORD)((vclock_us / 1000) % 1000);
    } else {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        st->wYear = (WORD)(tm->tm_year + 1900); st->wMonth = (WORD)(tm->tm_mon + 1); st->wDay = (WORD)tm->tm_mday;
        st->wDayOfWeek = (WORD)tm->tm_wday; st->wHour = (WORD)tm->tm_hour; st->wMinute = (WORD)tm->tm_min;
        st->wSecond = (WORD)tm->tm_sec;
    }
}

void clock_init(void)
{
    real_start = host_time_us();
}

/* ---- keyboard ---- */

/* As in Wine, two states: the keyboard's (async_state, as input arrives)
 * and the thread's (key_state, as the key messages are retrieved). */
BYTE key_state[256];            /* 0x80 down, 0x01 toggled */
static BYTE async_state[256];
static BYTE async_pressed[256];

static void key_set(BYTE *st, int vk, int down)
{
    if (down) {
        if (!(st[vk] & 0x80))
            st[vk] ^= 1;
        st[vk] |= 0x80;
    } else {
        st[vk] &= ~0x80;
    }
}

/* A key message leaves the queue: the thread's key state follows it. */
/* A mouse button message retrieved: as Wine's server, the thread's key
 * state (GetKeyState) follows the input it has taken, so a click injected
 * at once (down and up) still shows the button down while the game handles
 * WM_LBUTTONDOWN. */
static void mouse_retrieved(UINT msg)
{
    switch (msg) {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: key_state[VK_LBUTTON] |= 0x80; break;
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK: key_state[VK_RBUTTON] |= 0x80; break;
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK: key_state[VK_MBUTTON] |= 0x80; break;
    case WM_LBUTTONUP: key_state[VK_LBUTTON] &= ~0x80; break;
    case WM_RBUTTONUP: key_state[VK_RBUTTON] &= ~0x80; break;
    case WM_MBUTTONUP: key_state[VK_MBUTTON] &= ~0x80; break;
    }
}

static void key_retrieved(const MSG *m)
{
    int vk = (int)m->wParam & 0xff, ext = (m->lParam >> 24) & 1;
    int down = m->message == WM_KEYDOWN || m->message == WM_SYSKEYDOWN;
    if (m->message != WM_KEYDOWN && m->message != WM_SYSKEYDOWN && m->message != WM_KEYUP && m->message != WM_SYSKEYUP)
        return;
    key_set(key_state, vk, down);
    if (vk == VK_SHIFT)
        key_set(key_state, VK_LSHIFT, down);
    if (vk == VK_CONTROL)
        key_set(key_state, ext ? VK_RCONTROL : VK_LCONTROL, down);
    if (vk == VK_MENU)
        key_set(key_state, ext ? VK_RMENU : VK_LMENU, down);
}

SHORT WINAPI GetKeyState(int vk)
{
    API_TRACE();
    return (SHORT)(((key_state[vk & 0xff] & 0x80) ? 0x8000 : 0) | (key_state[vk & 0xff] & 1));
}

SHORT WINAPI GetAsyncKeyState(int vk)
{
    API_TRACE();
#if !defined(__wasm__)
    if (clk_win() && getenv("CIV2_TICKCALLERS")) {
        Dl_info d;
        void *f[24];
        char line[1024];
        int n = backtrace(f, 24), x, len = 0;
        for (x = 1; x < n && x < 14; x++)
            if (dladdr(f[x], &d))
                len += snprintf(line + len, sizeof line - (size_t)len, " < %s@%p", d.dli_sname ? d.dli_sname : "?", f[x]);
        plat_log("async @%llu%s\n", (unsigned long long)vclock_us, line);
    }
#endif
    SHORT r;
    if (plat_replay) {
        /* Polling the keyboard is a scheduling point (the game busy-waits on it). */
        vclock_us += QUERY_STEP_US;
        CLK("clk advance %llu @%llu\n", (unsigned long long)QUERY_STEP_US, (unsigned long long)vclock_us);
        timers_fire();
        replay_run_script();
    } else {
        msg_pump_events(0);
    }
    r = (SHORT)(((async_state[vk & 0xff] & 0x80) ? 0x8000 : 0) | (async_pressed[vk & 0xff] ? 1 : 0));
    async_pressed[vk & 0xff] = 0;
    return r;
}

static HHOOK kbd_hook_handle;
static HOOKPROC kbd_hook;

HHOOK WINAPI SetWindowsHookExA(int id, HOOKPROC proc, HINSTANCE mod, DWORD thread)
{
    API_TRACE();
    (void)mod; (void)thread;
    if (id != WH_KEYBOARD) {
        plat_log("SetWindowsHookEx(%d) ignored\n", id);
        return obj_new(OBJ_HOOK, NULL);
    }
    kbd_hook = proc;
    kbd_hook_handle = obj_new(OBJ_HOOK, NULL);
    return kbd_hook_handle;
}

WINBOOL WINAPI UnhookWindowsHookEx(HHOOK h)
{
    API_TRACE();
    if (h == kbd_hook_handle)
        kbd_hook = NULL;
    obj_free(h);
    return TRUE;
}

/* WH_KEYBOARD hooks run as the key message is retrieved. */
static int kbd_hook_call(UINT remove, const MSG *m)
{
    if (!kbd_hook)
        return 0;
    return kbd_hook((remove & PM_REMOVE) ? HC_ACTION : HC_NOREMOVE, m->wParam, m->lParam) != 0;
}

static int is_extended(int vk)
{
    return vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT || vk == VK_HOME || vk == VK_END ||
           vk == VK_PRIOR || vk == VK_NEXT || vk == VK_INSERT || vk == VK_DELETE || vk == VK_DIVIDE ||
           vk == VK_RCONTROL || vk == VK_RMENU;
}

/* Scan codes of a US keyboard (set 1) for the lParam of key messages. */
static int scan_code(int vk)
{
    static const char *row1 = "QWERTYUIOP", *row2 = "ASDFGHJKL", *row3 = "ZXCVBNM";
    const char *p;
    if (vk >= '1' && vk <= '9') return 2 + vk - '1';
    if (vk == '0') return 11;
    if ((p = strchr(row1, vk)) && vk) return 0x10 + (int)(p - row1);
    if ((p = strchr(row2, vk)) && vk) return 0x1e + (int)(p - row2);
    if ((p = strchr(row3, vk)) && vk) return 0x2c + (int)(p - row3);
    if (vk >= VK_F1 && vk <= VK_F10) return 0x3b + vk - VK_F1;
    switch (vk) {
    case VK_ESCAPE: return 1; case VK_BACK: return 14; case VK_TAB: return 15; case VK_RETURN: return 28;
    case VK_CONTROL: return 29; case VK_SHIFT: return 42; case VK_MENU: return 56; case VK_SPACE: return 57;
    case VK_F11: return 87; case VK_F12: return 88; case VK_HOME: case VK_NUMPAD7: return 71;
    case VK_UP: case VK_NUMPAD8: return 72; case VK_PRIOR: case VK_NUMPAD9: return 73;
    case VK_SUBTRACT: return 74; case VK_LEFT: case VK_NUMPAD4: return 75; case VK_NUMPAD5: return 76;
    case VK_RIGHT: case VK_NUMPAD6: return 77; case VK_ADD: return 78; case VK_END: case VK_NUMPAD1: return 79;
    case VK_DOWN: case VK_NUMPAD2: return 80; case VK_NEXT: case VK_NUMPAD3: return 81;
    case VK_INSERT: case VK_NUMPAD0: return 82; case VK_DELETE: case VK_DECIMAL: return 83;
    case VK_OEM_MINUS: return 12; case VK_OEM_PLUS: return 13; case VK_OEM_4: return 26; case VK_OEM_6: return 27;
    case VK_OEM_1: return 39; case VK_OEM_7: return 40; case VK_OEM_3: return 41; case VK_OEM_5: return 43;
    case VK_OEM_COMMA: return 51; case VK_OEM_PERIOD: return 52; case VK_OEM_2: return 53; case VK_MULTIPLY: return 55;
    }
    return 0;
}

void input_key(int vk, int down)
{
    int was = async_state[vk] & 0x80, alt;
    HWND target = GetFocus() ? GetFocus() : active_wnd;
    LPARAM lp;
    UINT msg;
    key_set(async_state, vk, down);
    if (down)
        async_pressed[vk] = 1;
    /* The generic modifier keys follow their left/right variants. */
    if (vk == VK_LSHIFT || vk == VK_RSHIFT) async_state[VK_SHIFT] = async_state[vk];
    if (vk == VK_LCONTROL || vk == VK_RCONTROL) async_state[VK_CONTROL] = async_state[vk];
    if (vk == VK_LMENU || vk == VK_RMENU) async_state[VK_MENU] = async_state[vk];
    if (vk == VK_LSHIFT || vk == VK_RSHIFT) vk = VK_SHIFT;
    if (vk == VK_LCONTROL || vk == VK_RCONTROL) vk = VK_CONTROL;
    if (vk == VK_LMENU || vk == VK_RMENU) vk = VK_MENU;
    alt = (async_state[VK_MENU] & 0x80) && !(async_state[VK_CONTROL] & 0x80);
    lp = 1 | ((LPARAM)scan_code(vk) << 16) | (is_extended(vk) ? 1 << 24 : 0);
    if (alt || vk == VK_MENU || vk == VK_F10)
        lp |= (LPARAM)(alt ? 1 : 0) << 29;
    if (!down)
        lp |= (LPARAM)0xC0000000u & 0xC0000000u;
    else if (was)
        lp |= 1 << 30;
    if (alt || vk == VK_F10 || (vk == VK_MENU && !(async_state[VK_CONTROL] & 0x80)))
        msg = down ? WM_SYSKEYDOWN : WM_SYSKEYUP;
    else
        msg = down ? WM_KEYDOWN : WM_KEYUP;
    (void)target;
    msg_post(KBD_PENDING, msg, (WPARAM)vk, lp);
}

/* Characters for TranslateMessage: a US keyboard layout. */
static int vk_to_char(int vk, int shift)
{
    static const char *digits_shift = ")!@#$%^&*(";
    if (vk >= 'A' && vk <= 'Z')
        return shift ^ ((key_state[VK_CAPITAL] & 1) != 0) ? vk : vk + 32;
    if (vk >= '0' && vk <= '9')
        return shift ? digits_shift[vk - '0'] : vk;
    if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9)
        return '0' + vk - VK_NUMPAD0;
    switch (vk) {
    case VK_SPACE: return ' ';
    case VK_RETURN: return '\r';
    case VK_BACK: return '\b';
    case VK_TAB: return '\t';
    case VK_ESCAPE: return 27;
    case VK_MULTIPLY: return '*';
    case VK_ADD: return '+';
    case VK_SUBTRACT: return '-';
    case VK_DECIMAL: return '.';
    case VK_DIVIDE: return '/';
    case VK_OEM_1: return shift ? ':' : ';';
    case VK_OEM_PLUS: return shift ? '+' : '=';
    case VK_OEM_COMMA: return shift ? '<' : ',';
    case VK_OEM_MINUS: return shift ? '_' : '-';
    case VK_OEM_PERIOD: return shift ? '>' : '.';
    case VK_OEM_2: return shift ? '?' : '/';
    case VK_OEM_3: return shift ? '~' : '`';
    case VK_OEM_4: return shift ? '{' : '[';
    case VK_OEM_5: return shift ? '|' : '\\';
    case VK_OEM_6: return shift ? '}' : ']';
    case VK_OEM_7: return shift ? '"' : '\'';
    }
    return 0;
}

WINBOOL WINAPI TranslateMessage(const MSG *m)
{
    API_TRACE();
    int c, shift = (key_state[VK_SHIFT] & 0x80) != 0, ctrl = (key_state[VK_CONTROL] & 0x80) != 0;
    if (m->message != WM_KEYDOWN && m->message != WM_SYSKEYDOWN)
        return FALSE;
    c = vk_to_char((int)m->wParam, shift);
    if (ctrl && c) {
        if ((c | 0x20) >= 'a' && (c | 0x20) <= 'z')
            c = (c | 0x20) - 'a' + 1;
        else if (c != '\r' && c != '\b' && c != 27)
            c = 0;
    }
    if (!c)
        return FALSE;
    /* The character comes next, ahead of messages already queued. */
    if (qlen < QMAX) {
        qhead = (qhead + QMAX - 1) % QMAX;
        qlen++;
        queue[qhead] = *m;
        queue[qhead].message = m->message == WM_SYSKEYDOWN ? WM_SYSCHAR : WM_CHAR;
        queue[qhead].wParam = (WPARAM)c;
    }
    return TRUE;
}

/* ---- mouse ---- */

static POINT cursor;
static int buttons;
static HWND pointer_top;            /* top-level window last reported under the pointer */

/* The X server reports the pointer entering a window when windows appear,
 * disappear or move under it, and Wine turns that into WM_MOUSEMOVE: do the
 * same when the top-level window under the pointer changes. */
void pointer_check(void)
{
    int hit;
    Window *w = wnd_from_point(cursor, &hit), *top = w;
    while (top && (top->style & WS_CHILD))
        top = top->parent;
    if ((top ? top->hwnd : 0) == pointer_top)
        return;
    pointer_top = top ? top->hwnd : 0;
    if (w)
        mouse_message(WM_MOUSEMOVE, WM_NCMOUSEMOVE);
}

void pointer_init(int x, int y)
{
    cursor.x = x;
    cursor.y = y;
}

WINBOOL WINAPI GetCursorPos(LPPOINT p)
{
    API_TRACE();
    *p = cursor;
    return TRUE;
}

static WPARAM mouse_keys(void)
{
    return (WPARAM)((buttons & 1 ? MK_LBUTTON : 0) | (buttons & 2 ? MK_RBUTTON : 0) | (buttons & 4 ? MK_MBUTTON : 0) |
                    (async_state[VK_SHIFT] & 0x80 ? MK_SHIFT : 0) | (async_state[VK_CONTROL] & 0x80 ? MK_CONTROL : 0));
}

static Window *mouse_target(const MSG *m)
{
    int hit;
    Window *w = wnd_get(capture_wnd);
    if (!w)
        w = wnd_from_point(m->pt, &hit);
    return w;
}

/* The hardware message m (pt: screen position) becomes the message the
 * game gets; returns 0 if it is swallowed, -1 if the message it becomes
 * does not pass the retrieval filter (h, lo..hi) and is left queued. */
static int process_mouse(MSG *m, HWND h, UINT lo, UINT hi)
{
    Window *w = mouse_target(m), *top;
    UINT client_msg = m->message;
    POINT pt = m->pt;
    int hit;
    if (!w)
        return 0;
    top = wnd_top(w);
    if (top && top->host_clipped) {
        pt.x -= top->host_dx;       /* the window shows displaced */
        pt.y -= top->host_dy;
    }
    if (wnd_get(capture_wnd))
        hit = HTCLIENT;
    else {
        hit = (int)wnd_send(w, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y));
        if (!wnd_get(w->hwnd))
            return 0;
    }
    if (plat_lock_panels && (w->style & WS_CHILD) && (hit == HTCAPTION || (hit >= HTLEFT && hit <= HTBOTTOMRIGHT)))
        hit = HTBORDER;     /* the game's panels stay where they are */
    if (plat_normal_desktop || hit == HTMENU) {
        /* Wine's process_mouse_message: the filter applies to the message
           as hit-testing made it (replays keep the earlier behaviour,
           which the recordings were matched with, but for the menu bar,
           which they never click) */
        MSG t = *m;
        t.hwnd = w->hwnd;
        t.message = hit == HTCLIENT ? client_msg : client_msg - WM_MOUSEMOVE + WM_NCMOUSEMOVE;
        if (!matches(&t, h, lo, hi))
            return -1;
    }
    if (client_msg == WM_LBUTTONDOWN || client_msg == WM_RBUTTONDOWN || client_msg == WM_MBUTTONDOWN) {
        /* Clicking a window activates its top-level window. */
        Window *t = w;
        while (t->style & WS_CHILD)
            t = t->parent;
        if (t->hwnd != active_wnd && !(t->style & WS_DISABLED)) {
            LRESULT ma = wnd_send(w, WM_MOUSEACTIVATE, (WPARAM)t->hwnd, MAKELPARAM(hit, client_msg));
            if (wnd_get(t->hwnd) && ma != MA_NOACTIVATE && ma != MA_NOACTIVATEANDEAT)
                SetWindowPos(t->hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            if (ma == MA_ACTIVATEANDEAT || ma == MA_NOACTIVATEANDEAT)
                return 0;
        }
        if (!wnd_get(w->hwnd) || (w->style & WS_DISABLED))
            return 0;
    }
    wnd_send(w, WM_SETCURSOR, (WPARAM)w->hwnd, MAKELPARAM(hit, client_msg));
    if (!wnd_get(w->hwnd))
        return 0;
    m->hwnd = w->hwnd;
    if (hit != HTCLIENT) {
        m->message = client_msg - WM_MOUSEMOVE + WM_NCMOUSEMOVE;
        m->wParam = (WPARAM)hit;
        m->lParam = MAKELPARAM(pt.x, pt.y);
        return 1;
    }
    ScreenToClient(w->hwnd, &pt);
    m->lParam = MAKELPARAM(pt.x, pt.y);
    return 1;
}

static void mouse_message(UINT client_msg, UINT nc_msg)
{
    (void)nc_msg;
    msg_post(MOUSE_PENDING, client_msg, mouse_keys(), 0);
    queue[(qhead + qlen - 1) % QMAX].pt = cursor;
}

static DWORD last_click_time;
static POINT last_click_pos;
static int last_click_button;

void input_mouse_move(int x, int y)
{
    int hit;
    Window *top;
    if (x == cursor.x && y == cursor.y)
        return;
    cursor.x = x;
    cursor.y = y;
    for (top = wnd_from_point(cursor, &hit); top && (top->style & WS_CHILD); top = top->parent)
        ;
    pointer_top = top ? top->hwnd : 0;
    mouse_message(WM_MOUSEMOVE, WM_NCMOUSEMOVE);
}

void input_mouse_button(int button, int down)
{
    static const UINT downs[3] = {WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN};
    static const UINT ups[3] = {WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP};
    static const UINT dbls[3] = {WM_LBUTTONDBLCLK, WM_RBUTTONDBLCLK, WM_MBUTTONDBLCLK};
    static const int vks[3] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON};
    int i = button - 1;
    if (i < 0 || i > 2)
        return;
    if (down) {
        DWORD now = plat_now();
        int hit;
        Window *w = wnd_from_point(cursor, &hit);
        buttons |= 1 << i;
        async_state[vks[i]] |= 0x80;       /* (key_state: when retrieved) */
        if (w && (w->cls->wc.style & CS_DBLCLKS) && last_click_button == button &&
            now - last_click_time <= 500 && abs(cursor.x - last_click_pos.x) <= 2 &&
            abs(cursor.y - last_click_pos.y) <= 2) {
            last_click_time = 0;
            mouse_message(dbls[i], dbls[i] - WM_LBUTTONDBLCLK + WM_NCLBUTTONDBLCLK);
            return;
        }
        last_click_time = now;
        last_click_pos = cursor;
        last_click_button = button;
        mouse_message(downs[i], downs[i] - WM_LBUTTONDOWN + WM_NCLBUTTONDOWN);
    } else {
        buttons &= ~(1 << i);
        async_state[vks[i]] &= ~0x80;
        mouse_message(ups[i], ups[i] - WM_LBUTTONUP + WM_NCLBUTTONUP);
    }
}

/* The wheel (Windows: WM_MOUSEWHEEL, WHEEL_DELTA 120 a notch) to the window
 * with the focus, else the active one; the pointer in screen coordinates. */
void input_mouse_wheel(int notches)
{
    HWND h = GetFocus() ? GetFocus() : (wnd_get(active_wnd) ? active_wnd : 0);
    if (!h || !notches)
        return;
    msg_post(h, WM_MOUSEWHEEL, MAKEWPARAM(mouse_keys(), (WORD)(short)(notches * 120)),
             MAKELPARAM(cursor.x, cursor.y));
}

/* ---- cursor ---- */

static HCURSOR current_cursor;
static int cursor_count;

HCURSOR WINAPI SetCursor(HCURSOR c)
{
    API_TRACE();
    HCURSOR old = current_cursor;
    current_cursor = c;
    cursor_set_image(c);
    return old;
}

int WINAPI ShowCursor(WINBOOL show)
{
    API_TRACE();
    cursor_count += show ? 1 : -1;
    host_cursor_show(cursor_count >= 0);
    return cursor_count;
}

/* (docs/EXTRAS.md) The window under the pointer, and the pointer, in desktop
   coordinates. */
HWND plat_window_at_cursor(POINT *pt)
{
    int hit;
    Window *w = wnd_from_point(cursor, &hit);
    if (pt)
        *pt = cursor;
    return w ? w->hwnd : 0;
}
