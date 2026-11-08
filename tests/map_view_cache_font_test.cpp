// SOURCE src/auto/graphics.cpp GameFont::set
// SOURCE src/auto/graphics.cpp GameFont::destroy
// SOURCE src/auto/graphics.cpp gfx_select_zoom_font
// SOURCE src/game/map_view_cache.cpp swap_label_font
// SOURCE src/game/map_view_cache.cpp paint_rect
// SOURCE src/game/map_view_cache.cpp destroy
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
using iptr=intptr_t;using i32=int;using u32=uint32_t;using i8=signed char;
#ifndef __cdecl
#define __cdecl
#endif
#define _fclose fclose
#include "scoped.h"
static std::set<u32> handles;
static u32 created;
static u32 font_handle_create(int,int,int) { handles.insert(++created);return created; }
static iptr font_handle_destroy(u32 handle) { assert(handles.erase(handle)==1);return 0; }
static u32 font_measure_height(u32 handle) { assert(handles.count(handle));return handle+100; }
struct GameFont {
    u32 hfont,cached_height;
    void set(i32,iptr);
    GameFont *destroy();
};
static struct { GameFont map_label_font[2]; } status_window_block;
static int map_label_font_size=-99;
static int gfx_zoom_scale(int size,int zoom) { return size*(zoom+8)/8; }
static void gfx_select_zoom_font(i32);
struct RECT { int left,top,right,bottom; };
static int text_style=7,draw_target=8,gfx_draw_target=9;
static struct { int gfx_text; } ctrl_colors={10};
static struct { int num,den; } gfx_scale={3,2};
static void gfx_set_scale_ratio(int n,int d) { gfx_scale.num=n;gfx_scale.den=d; }
struct MapView {
    GameFont tile_font,tile_small_font;
    int map_surface,overlay_surface,zoom;
    int field_2fc,field_304,margin_y,half_tile_height,tile_height,tile_width;
    int half_view_width,tile_columns,origin_x,origin_y;
    void reset_clip() {}
    void set_clip(RECT *) {}
    void fill(int) {}
    void tile_to_screen(iptr x,iptr y,int,int) { *(int *)x=0;*(int *)y=0; }
    void draw_tile(int,int,int) {
        gfx_select_zoom_font(zoom);
        assert(handles.count(status_window_block.map_label_font[0].hfont));
        text_style=draw_target=gfx_draw_target=ctrl_colors.gfx_text=0;gfx_set_scale_ratio(1,1);
    }
    void draw_all_tiles(int civ) { draw_tile(0,0,civ); }
    void draw_city_names(int,int,int) {}
};
struct CachedView { MapView *view;GameFont label_font;int label_font_size,civ; };
static MapView *painting;
static uint64_t rendered;
static int map_wrap_x(int x) { return x; }
static void surface_dtor(void *) {}
static void hd_gpu_text_begin() {}
static void hd_gpu_text_end() {
    assert(painting && handles.count(status_window_block.map_label_font[0].hfont));
}
#include "production.inc"
int main()
{
    CachedView entries[3]={};
    int zooms[]={-7,0,6};
    RECT rect={0,0,128,128};
    for(int i=0;i<3;i++) {
        CachedView &e=entries[i];e.view=(MapView *)calloc(1,sizeof(MapView));
        e.label_font_size=-99;e.view->zoom=zooms[i];
        e.view->tile_height=32;e.view->tile_width=64;e.view->half_tile_height=16;
        e.view->tile_columns=e.view->half_view_width=1;
    }
    // Even an uninitialized live font must survive a background paint intact.
    paint_rect(entries[0],rect);
    assert(!status_window_block.map_label_font[0].hfont && map_label_font_size==-99);
    gfx_select_zoom_font(0);
    GameFont live=status_window_block.map_label_font[0];
    int live_size=map_label_font_size;
    for(int i=0;i<3000;i++) {
        CachedView &e=entries[i%3];
        // Also exercise the early return in the diagnostic's full traversal.
        paint_rect(e,rect,i%2);
        assert(!painting);
        assert(status_window_block.map_label_font[0].hfont==live.hfont);
        assert(status_window_block.map_label_font[0].cached_height==live.cached_height);
        assert(map_label_font_size==live_size);
        assert(text_style==7 && draw_target==8 && gfx_draw_target==9 && ctrl_colors.gfx_text==10);
        assert(gfx_scale.num==3 && gfx_scale.den==2);
        if(i>=2)assert(created==4 && handles.size()==4);
    }
    // Recycling a slot replaces just its own label font, once.
    assert(rendered>0);
    entries[0].view->zoom=8;paint_rect(entries[0],rect);
    assert(created==5 && handles.size()==4 && handles.count(live.hfont));
    for(auto &e:entries)destroy(e);
    assert(handles.size()==1 && handles.count(live.hfont));
    status_window_block.map_label_font[0].destroy();assert(handles.empty());
    puts("Cached label fonts: 3,000 paints reuse handles, restore live state and release ownership correctly.");
}
