// SOURCE src/platform/window.c IsChild
// SOURCE src/platform/window.c wnd_is_visible
// SOURCE src/platform/window.c unlink_wnd
// SOURCE src/platform/window.c set_focus_window
// SOURCE src/platform/window.c set_active_window
// SOURCE src/platform/window.c activate_other
// SOURCE src/platform/window.c SetFocus
// SOURCE src/platform/window.c send_destroy
// SOURCE src/platform/window.c destroy_storage
// SOURCE src/platform/window.c DestroyWindow
// SOURCE src/platform/message.c resolve
// Exercise actual destruction, focus messages and keyboard routing after the
// advisor has been hidden, without replay.c's automatic ensure_focus repair.
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
// This fixture uses only the C platform layer, not extras.h's C++ game helpers.
#define CIV2_EXTRAS_H
#include "plat.h"

static Window windows[8];
Window *desktop = &windows[0];
HWND focus_wnd, capture_wnd, active_wnd;
unsigned wnd_generation;
static int first_focus, app_active;
static HWND redirect_focus, destroy_on_killfocus;
struct Event { HWND window; UINT message; };
static std::vector<Event> events;

Window *wnd_get(HWND h) {
    return h > 0 && h < 8 && windows[h].hwnd && !windows[h].destroyed ? &windows[h] : NULL;
}
HWND WINAPI GetFocus(void) { return wnd_get(focus_wnd) ? focus_wnd : 0; }
void api_trace(const char *, void *) {}
void wnd_host_sync(Window *) {}
void timers_window_destroyed(HWND) {}
void msg_window_destroyed(HWND) {}
void obj_free(Handle) {}
void rgn_free(Region *) {}

LRESULT wnd_send(Window *w, UINT msg, WPARAM, LPARAM) {
    assert(!w->destroyed);
    events.push_back({w->hwnd, msg});
    if (msg == WM_KILLFOCUS) {
        HWND redirect = redirect_focus, destroy = destroy_on_killfocus;
        redirect_focus = destroy_on_killfocus = 0;
        if (redirect) SetFocus(redirect);
        if (destroy) DestroyWindow(destroy);
    }
    return 0;
}

// These services are outside the scenario: the game's wnd_obj_destroy has
// already called ShowWindow(SW_HIDE) before it calls DestroyWindow.
WINBOOL WINAPI ShowWindow(HWND, int) { assert(false); return FALSE; }
static WINBOOL swp(Window *, HWND, int, int, int, int, UINT) { assert(false); return FALSE; }
#define KBD_PENDING ((HWND)0xfffffffeu)
#define MOUSE_PENDING ((HWND)0xfffffffdu)
static Window *mouse_target(const MSG *) { assert(false); return NULL; }

#include "production.inc"

static void add_window(HWND h, HWND parent, DWORD style, HWND owner = 0) {
    Window *w = &windows[h];
    w->hwnd = h;
    w->style = style;
    w->parent = &windows[parent];
    w->owner = owner ? &windows[owner] : NULL;
    w->next = w->parent->children;
    w->parent->children = w;
}

static void setup(HWND focused = 3) {
    memset(windows, 0, sizeof windows);
    focus_wnd = capture_wnd = active_wnd = 0;
    first_focus = 1;
    app_active = 0;
    redirect_focus = destroy_on_killfocus = 0;
    add_window(1, 0, WS_VISIBLE);                 // main game window
    add_window(2, 1, WS_CHILD);                   // hidden Trade Advisor
    add_window(3, 2, WS_CHILD | WS_VISIBLE);      // its Close button
    add_window(4, 1, WS_CHILD | WS_VISIBLE);      // another map/view
    SetFocus(focused);
    assert(GetFocus() == focused);
    events.clear();
}

static void expect_shortcuts(HWND target) {
    // The game consumes ordinary WM_KEYDOWN/WM_CHAR. With no focus, resolve
    // converts keydown to WM_SYSKEYDOWN, so asserting only the HWND is insufficient.
    const UINT messages[] = {WM_KEYDOWN, WM_CHAR, WM_KEYUP};
    for (UINT message : messages) {
        MSG input = {}, output = {};
        input.hwnd = KBD_PENDING;
        input.message = message;
        input.wParam = 'F';
        assert(resolve(&input, &output));
        assert(output.hwnd == target && output.message == message && output.wParam == 'F');
    }
}

static size_t event_index(HWND window, UINT message) {
    for (size_t i = 0; i < events.size(); ++i)
        if (events[i].window == window && events[i].message == message) return i;
    return events.size();
}

int main() {
    // Mouse Close: focus remains on the button after the advisor is hidden.
    setup();
    assert(DestroyWindow(2));
    assert(!wnd_get(2) && !wnd_get(3));
    assert(GetFocus() == 1 && active_wnd == 1);
    assert(event_index(3, WM_KILLFOCUS) < event_index(1, WM_SETFOCUS));
    assert(event_index(1, WM_SETFOCUS) < event_index(2, WM_DESTROY));
    expect_shortcuts(1);

    // Keyboard Close, including an already hidden focused window.
    setup(2);
    assert(DestroyWindow(2));
    assert(GetFocus() == 1);
    expect_shortcuts(1);

    // Closing a background advisor must preserve focus in another view.
    setup(4);
    assert(DestroyWindow(2));
    assert(GetFocus() == 4);
    assert(event_index(4, WM_KILLFOCUS) == events.size());
    expect_shortcuts(4);

    // A nested child returns focus to its immediate parent.
    setup();
    add_window(5, 2, WS_CHILD);
    add_window(6, 5, WS_CHILD | WS_VISIBLE);
    SetFocus(6);
    assert(DestroyWindow(5));
    assert(GetFocus() == 2);
    expect_shortcuts(2);

    // Focus handlers may redirect focus or destroy the advisor reentrantly.
    setup();
    redirect_focus = 4;
    assert(DestroyWindow(2));
    assert(GetFocus() == 4);
    expect_shortcuts(4);
    setup();
    destroy_on_killfocus = 2;
    assert(DestroyWindow(2));
    assert(GetFocus() == 1 && !wnd_get(2) && !wnd_get(3));
    int destroyed = 0;
    for (const Event &event : events)
        if (event.window == 2 && event.message == WM_DESTROY) ++destroyed;
    assert(destroyed == 1);
    expect_shortcuts(1);

    // Owned top-level windows still use the normal activation path.
    setup();
    add_window(5, 0, WS_POPUP, 1);
    add_window(6, 5, WS_CHILD | WS_VISIBLE);
    SetFocus(6);
    assert(DestroyWindow(5));
    assert(GetFocus() == 1 && active_wnd == 1);
    expect_shortcuts(1);
    assert(!DestroyWindow(7));
    assert(GetFocus() == 1);
    puts("window focus and shortcut routing passed");
}
