// SOURCE src/game/map_camera.cpp cancel_drag
// SOURCE src/game/map_camera.cpp map_camera_input
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "win32_api.h"
#include "map_camera.h"
static struct {
    void *view; HWND hwnd; MapCameraTransform transform;
    double cx,cy,scale,target,anchor_x,anchor_y,world_x,world_y,drag_x,drag_y;
    int width,height,button,moved,double_click,refreshing,animating,dirty,prefetch;
    DWORD tick; POINT start;
} camera;
static struct { int ui_input_busy; } net;
static struct { int processing; } city_turn;
static HWND capture;
static int enabled() { return 1; }
WINBOOL IsWindowEnabled(HWND) { return 1; }
HWND GetCapture() { return capture; }
WINBOOL ReleaseCapture() { capture=0;return 1; }
HWND SetCapture(HWND hwnd) { HWND previous=capture;capture=hwnd;return previous; }
static int ui_paradrop_cursor_active() { return 0; }
static void mapview_cancel_drag() {}
static void plat_log(const char *,...) {}
static double clamp(double v,double lo,double hi) { return v<lo ? lo : v>hi ? hi : v; }
static HWND plat_window_at_point(int x,int y) { return x==150 && y==160 ? camera.hwnd : 0; }
WINBOOL ScreenToClient(HWND,POINT *pt) { pt->x-=10;pt->y-=20;return 1; }
DWORD timeGetTime() { return 1; }
int map_camera_zoom_step(void *,int) { return 1; }
static std::vector<UINT> sent;
LRESULT SendMessageA(HWND,UINT message,WPARAM,LPARAM) { sent.push_back(message);return 0; }
#include "production.inc"
static int event(UINT message,int x,int y,WPARAM keys=0)
{
    MSG msg={};msg.hwnd=camera.hwnd;msg.message=message;msg.wParam=keys;
    msg.lParam=MAKELPARAM(x,y);msg.pt={999,999}; // Live/MSG.pt coordinates are unrelated.
    return map_camera_input(&msg);
}
int main()
{
    camera.view=&camera;camera.hwnd=1;camera.scale=camera.target=2;
    camera.width=740;camera.height=680;camera.transform={1,0,0,8,36,740,680};
    camera.cx=100;camera.cy=80;
    assert(event(WM_LBUTTONDOWN,100,150,MK_LBUTTON));
    assert(event(WM_MOUSEMOVE,300,250,MK_LBUTTON));
    assert(event(WM_LBUTTONUP,300,250));
    assert(camera.cx==100-200/64.0 && camera.cy==80-100/32.0);
    assert(camera.dirty && !capture && sent.empty());
    double cx=camera.cx,cy=camera.cy;
    // A quick repeat drag starts with DBLCLK, but must not open a city.
    assert(event(WM_LBUTTONDBLCLK,100,150,MK_LBUTTON));
    assert(event(WM_MOUSEMOVE,148,174,MK_LBUTTON));
    assert(event(WM_LBUTTONUP,148,174));
    assert(sent.empty() && camera.cx==cx-.75 && camera.cy==cy-.75);
    assert(event(WM_LBUTTONDOWN,100,150,MK_LBUTTON));
    assert(event(WM_LBUTTONUP,100,150));
    assert((sent==std::vector<UINT>{WM_LBUTTONDOWN,WM_LBUTTONUP}));
    sent.clear();
    assert(event(WM_LBUTTONDBLCLK,100,150,MK_LBUTTON));
    assert(sent.empty());
    assert(event(WM_LBUTTONUP,100,150));
    assert((sent==std::vector<UINT>{WM_LBUTTONDBLCLK,WM_LBUTTONUP}));
    assert(event(WM_MOUSEWHEEL,150,160,MAKEWPARAM(0,60)));
    assert(camera.anchor_x==132 && camera.anchor_y==104);
    assert(!event(WM_MOUSEWHEEL,999,999,MAKEWPARAM(0,60)));
    puts("Camera input: queued coordinates, drag/double-click distinction and recorded wheel anchors passed.");
}
