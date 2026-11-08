// SOURCE src/auto/mapview.cpp MapView::redraw_area
#include <cassert>
#include <cstdint>
#include <cstdio>
using iptr=intptr_t;using i32=int;using i8=signed char;
#define _fclose fclose
#include "scoped.h"
struct RECT { int left,top,right,bottom; };
static struct { int active=1; } turn_loop;
static struct { int multiplayer,reveal_map; } game;
static struct { int map_redraw_busy; } net;
static struct { int unit_phase,unit_active; } blink;
static struct { int x[1],y[1]; } map_cursor;
static int hd_gpu_active=1,moving_units_mode=1,depth,presents,partial_presents,blits,last_civ;
static void present() {
    if(depth)return;
    presents++;
    if(net.map_redraw_busy)partial_presents++;
}
static void hd_redraw_begin() { depth++; }
static void hd_redraw_end() { assert(depth>0);if(!--depth)present(); }
static void map_view_cache_invalidate(void *,int,int,int) {}
static void gfx_reset_zoom_scale() {}
struct Sprite { void draw(iptr,iptr,int,int) {} };
static struct { Sprite map_cursor_sprite; } map_cursor_sprite_block;
struct MapView {
    int map_view_index=0;
    void redraw_area(i32,i32,i32,iptr,int);
    int tile_in_view(int,int) { return 1; }
    void tile_to_screen(iptr x,iptr y,int,int) { *(int *)x=*(int *)y=0; }
    void apply_zoom() {}
    void draw_tile_radius(int,int,int,int civ) {
        last_civ=civ;
        // Model the message polls during a blink's individual tile draws.
        for(int i=0;i<9;i++)present();
    }
    void blit_tile_radius(int,int,int) { blits++; }
};
#include "production.inc"
int main()
{
    MapView view;
    view.redraw_area(50,94,0,2,1);
    assert(presents==1 && partial_presents==0 && blits==1 && last_civ==2);
    assert(depth==0 && net.map_redraw_busy==0);
    // Nested redraw batches must leave presentation to the outer caller.
    hd_redraw_begin();game.reveal_map=1;
    view.redraw_area(50,94,1,2,0);
    assert(presents==1 && blits==1 && last_civ==-1 && depth==1);
    hd_redraw_end();assert(presents==2);
    turn_loop.active=0;view.redraw_area(50,94,0,2,1);
    assert(presents==2 && blits==1 && depth==0);
    // Preserve the existing software/network redraw policy.
    turn_loop.active=1;hd_gpu_active=0;view.redraw_area(50,94,0,2,1);
    assert(presents==11 && partial_presents==9 && depth==0);
    hd_gpu_active=1;game.multiplayer=3;view.redraw_area(50,94,0,2,1);
    assert(presents==20 && partial_presents==18 && depth==0);
    puts("Blink redraw: one completed GPU frame, nested batching and legacy paths passed.");
}
