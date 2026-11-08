// SOURCE src/game/map_camera.cpp map_camera_recenter
// SOURCE src/game/map_camera.cpp horizontal_center
// SOURCE src/game/map_camera.cpp source_x
// SOURCE src/game/map_camera.cpp source_y
// SOURCE src/game/map_camera.cpp focus_position
// SOURCE src/game/map_camera.cpp map_camera_near_edge
// SOURCE src/game/map_camera.cpp map_camera_layout
// SOURCE src/auto/mapview.cpp MapView::scroll_if_near_edge
// SOURCE src/auto/mapview.cpp mapviews_ensure_visible
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include "map_camera.h"
#ifndef _MSC_VER
#define __cdecl
#endif
using iptr=intptr_t;using uptr=uint32_t;using u32=uint32_t;
using i32=int;using uint=unsigned;using ushort=unsigned short;using HWND=int;
enum { RULE_FLAT_WORLD=1 };
static struct { int rules; } game;
static struct { int width=200,height=160; } map;
static struct { int unit_skip_tick,unit_phase; } blink;
static struct { int map_redraw_busy; } net;
static int human_civ[]={1},moving_units_mode=1,active=1,centers,clears;
struct MapView {
    int map_view_index=0,open=0,track_moves=1,zoom=4;
    int center_x=100,center_y=100,client_x=8,client_y=36,client_width=740,client_height=680;
    int half_tile_width=48,half_tile_height=24;
    int origin_x=92,origin_y=85,margin_x=0,margin_y=0;
    uptr origin_x_uptr=92,origin_y_uptr=85;
    u32 half_view_width=8,tile_columns=9,end_y_u=113,margin_x_u=0,margin_y_u=0;
    int field_2fc=15;
    int scroll_if_near_edge(int,int);
    void center_on(int,int);
};
static struct { MapView map_view_recs[8]; } map_views_block;
static struct {
    MapView *view=nullptr;HWND hwnd=1;
    int refreshing=0,recenter=0,center_x=100,center_y=100,zoom=4;
    int width=740,height=680,map_w=200,map_h=160,civ=1,animating=0,dirty=0,prefetch=0;
    double scale=2,target=2,cx=99.5,cy=100.25;
} camera;
static int enabled() { return active; }
int map_camera_owns(void *view) { return enabled() && camera.view==view; }
static int view_hwnd(MapView *) { return 1; }
static double clamp(double v,double lo,double hi) { return std::max(lo,std::min(v,hi)); }
static int int_clamp(int v,int lo,int hi) { return std::max(lo,std::min(v,hi)); }
static int map_wrap_x(int x) { return game.rules & RULE_FLAT_WORLD ? x : (x%map.width+map.width)%map.width; }
static void cancel_drag() {}
void map_view_cache_clear() { clears++; }
static void remember() {
    MapView *m=camera.view;
    camera.center_x=m->center_x;camera.center_y=m->center_y;camera.zoom=m->zoom;
}
static void transform() {}
#include "production.inc"
void MapView::center_on(int x,int y)
{
    centers++;center_x=map_wrap_x(x);center_y=y;
    map_camera_recenter(this);
    origin_x=map_wrap_x(center_x-8);
    origin_y=int_clamp(center_y-field_2fc,-1,map.height+1-2*field_2fc);
    origin_x_uptr=origin_x;origin_y_uptr=origin_y;end_y_u=origin_y+28;
    map_camera_layout(this);
}
int main()
{
    MapView &view=map_views_block.map_view_recs[0];camera.view=&view;
    // The tile at x=106 is in the backing image but outside the displayed
    // viewport after a fractional drag. This is the activation call path.
    assert((107-camera.cx)*64+camera.width/2.0>camera.width);
    assert(mapviews_ensure_visible(106,100,1)==1);
    assert(centers==1 && view.center_x==106 && blink.unit_skip_tick && !blink.unit_phase);
    assert(camera.scale==2 && !map_camera_near_edge(&view,106,100));
    assert(!net.map_redraw_busy && !clears);
    // An already-visible unit must not reset fractional pan or zoom.
    camera.cx=106.25;camera.cy=100.5;camera.scale=1.7;
    assert(!mapviews_ensure_visible(106,100,1));
    assert(centers==1 && camera.cx==106.25 && camera.cy==100.5 && camera.scale==1.7);
    // A requested center may equal the old backing center after a drag.
    // The explicit centering hook must still discard the displayed offset.
    camera.scale=2;camera.cx=100;camera.cy=100;camera.animating=1;
    assert(mapviews_ensure_visible(106,100,1));
    assert(centers==2 && !camera.animating && !camera.recenter && camera.scale==2);
    assert(!map_camera_near_edge(&view,106,100));
    // Honor each map window's own/enemy movement tracking mask.
    view.track_moves=0;camera.cx=100;camera.cy=100;
    assert(!mapviews_ensure_visible(106,100,1) && centers==2 && camera.cx==100);
    view.track_moves=1;
    assert(!mapviews_ensure_visible(106,100,2) && centers==2);
    // Near-edge checks in all four directions, fractional scales and wrapping.
    for(double scale: {.5,1.0625,1.7,2.0}) {
        camera.scale=scale;camera.cx=100.25;camera.cy=80.25;
        double hw=camera.width/(64*scale),hh=camera.height/(32*scale);
        assert(!map_camera_near_edge(&view,100,80));
        assert(map_camera_near_edge(&view,(int)ceil(camera.cx+hw),80));
        assert(map_camera_near_edge(&view,(int)floor(camera.cx-hw),80));
        assert(map_camera_near_edge(&view,100,(int)ceil(camera.cy+hh)));
        assert(map_camera_near_edge(&view,100,(int)floor(camera.cy-hh)));
    }
    camera.scale=2;camera.cx=199.5;camera.cy=80;
    assert(!map_camera_near_edge(&view,2,80));
    assert(map_camera_near_edge(&view,10,80));
    camera.cx=.5;assert(!map_camera_near_edge(&view,198,80));
    // Clamped poles and flat edges do not continually request impossible pans.
    game.rules=RULE_FLAT_WORLD;camera.cx=5;camera.cy=10;
    assert(!map_camera_near_edge(&view,0,0));
    camera.cx=195;camera.cy=150;
    assert(map_camera_near_edge(&view,199,159)); // A small remaining pan still matters.
    camera.cx=201-camera.width/(64*camera.scale);
    camera.cy=161-camera.height/(32*camera.scale);
    assert(!map_camera_near_edge(&view,199,159));
    // No tracking scroll when the complete map fits in the visible viewport.
    map.width=20;map.height=20;camera.scale=.125;camera.cx=10;camera.cy=10;
    assert(!map_camera_near_edge(&view,0,0) && !map_camera_near_edge(&view,19,19));
    // Fixed zoom, secondary windows and disabled cameras use original bounds.
    map.width=200;map.height=160;game.rules=0;
    MapView other;assert(map_camera_near_edge(&other,106,100)==-1);
    assert(!other.scroll_if_near_edge(106,100));
    active=0;assert(map_camera_near_edge(&view,106,100)==-1);
    assert(!view.scroll_if_near_edge(106,100));
    assert(view.scroll_if_near_edge(150,100));
    // With an aligned viewport and no overscan, camera thresholds must agree
    // with the original check, including the diamond-center offset.
    MapView aligned;
    aligned.client_width=640;aligned.client_height=480;
    aligned.origin_x=aligned.origin_x_uptr=80;aligned.origin_y=aligned.origin_y_uptr=60;
    aligned.half_tile_width=32;aligned.half_tile_height=16;
    aligned.half_view_width=10;aligned.tile_columns=11;aligned.end_y_u=90;
    camera.view=&aligned;camera.cx=91;camera.cy=76;camera.width=640;camera.height=480;camera.scale=1;
    for(int y=58;y<=94;y++)for(int x=78;x<=104;x++)if(!((x+y)&1)) {
        MapView legacy=aligned;active=0;
        int expected=legacy.scroll_if_near_edge(x,y);
        active=1;assert(map_camera_near_edge(&aligned,x,y)==expected);
    }
    // Last-row units must be checked at their real location, then brought into
    // the visible crop even though the backing image cannot recenter farther.
    MapView pole;camera.view=&pole;camera.width=740;camera.height=680;camera.scale=2;
    camera.cx=102;camera.cy=149.1;pole.origin_y=131;pole.origin_y_uptr=131;
    assert((160-camera.cy)*32+camera.height/2.0>camera.height);
    assert(pole.scroll_if_near_edge(101,159));
    assert(pole.center_y==159);
    double unit_y=(160-camera.cy)*32+camera.height/2.0;
    assert(unit_y>=0 && unit_y<camera.height);
    double cy=camera.cy;
    assert(!pole.scroll_if_near_edge(101,159) && camera.cy==cy);
    // A zoomed-out backing can have blank margins. Even when the world could
    // fit in the viewport, a pan can put its last row outside the visible crop.
    map.height=80;camera.map_h=80;camera.scale=.24;camera.cy=80-680/(32*.24)-.25;
    pole.half_tile_height=2;pole.field_2fc=40;pole.margin_y=260;pole.origin_y=0;
    assert(pole.scroll_if_near_edge(101,79));
    assert(camera.cy==40.5 && !pole.scroll_if_near_edge(101,79));
    puts("Camera focus: offscreen activation, map-boundary units, same backing center, tracking settings, fractional zoom, wrapping and legacy fallback passed.");
}
