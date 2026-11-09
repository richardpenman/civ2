// SOURCE src/game/map_camera.cpp map_camera_option
// SOURCE src/game/map_camera.cpp enabled
// SOURCE src/game/map_camera.cpp cancel_drag
// SOURCE src/game/map_camera.cpp map_camera_forget
// SOURCE src/game/map_camera.cpp map_camera_set_option
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <initializer_list>

using i16=short;
enum { RULE_FLAT_WORLD=1 };
struct MapView { i16 zoom,center_x,center_y; };
static struct { int rules; } game;
static struct { int width,height; } map={80,50};
static struct { int active; } turn_loop={1};
static struct {
    MapView *view;
    int hwnd,button,moved,animating,dirty;
    double scale,cx,cy;
} camera;
static int camera_option=-1,hd_gpu_active=1,ini_value=-1,reads,clears,capture;
static int GetPrivateProfileIntA(const char *section,const char *key,int fallback,const char *file)
{
    assert(!strcmp(section,"OpenCiv2") && !strcmp(key,"GpuMapCamera") && !strcmp(file,"CIV.INI"));
    ++reads;return ini_value<0 ? fallback : ini_value;
}
static int GetCapture() { return capture; }
static void ReleaseCapture() { capture=0; }
static void map_view_cache_clear() { ++clears; }
static double clamp(double v,double lo,double hi) { return v<lo ? lo : v>hi ? hi : v; }
static int map_wrap_x(int x) { return game.rules & RULE_FLAT_WORLD ? x : (x%map.width+map.width)%map.width; }
#include "production.inc"

int main()
{
    assert(!map_camera_option() && reads==1); // Missing key is off, read once.
    assert(!map_camera_option() && reads==1);
    ini_value=1;camera_option=-1;
    assert(map_camera_option()); // Read a saved preference, even without a GPU.
    hd_gpu_active=0;assert(!enabled() && map_camera_option());
    hd_gpu_active=1;turn_loop.active=0;assert(!enabled());
    turn_loop.active=1;assert(enabled());
    for(int flat: {0,1})for(double scale: {.125,.53,1.0625,1.75,2.0}) {
        game.rules=flat;
        MapView view={-7,12,10};
        camera={&view,7,1,1,1,1,scale,79.8,48.4};capture=7;
        int before=clears;
        map_camera_set_option(1); // Accept unchanged: preserve fractional view/cache.
        assert(camera.view==&view && clears==before && capture==7 && camera.scale==scale);
        map_camera_set_option(0);
        assert(!enabled() && !map_camera_option());
        assert(!camera.view && !camera.animating && !camera.dirty && !capture && clears==before+1);
        assert(view.zoom==(int)floor(scale*8+.5)-8); // Displayed zoom, not backing zoom.
        assert(view.center_x>=0 && view.center_x<map.width && view.center_y==48);
        assert(((view.center_x+view.center_y)&1)==0); // Valid diamond at the fixed right edge.
        map_camera_set_option(1);
        assert(enabled() && !camera.view && clears==before+1); // Next layout starts fresh.
    }
    puts("Camera option: default, saved setting, CPU fallback, live toggle, zoom, bounds and cache/capture cleanup passed.");
}
