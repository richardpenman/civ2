// SOURCE src/auto/mapview.cpp MapView::tile_to_screen
// SOURCE src/auto/mapview.cpp MapView::tile_radius_rect
// SOURCE src/auto/mapview.cpp MapView::draw_tile_radius
// SOURCE src/auto/mapview.cpp MapView::draw_all_tiles
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
using iptr=intptr_t;using uptr=uintptr_t;using i32=int;using u32=uint32_t;
using i8=signed char;using uint=unsigned;
struct RECT { int left,top,right,bottom; };
using LPRECT=RECT *;
static int hd_enabled=1,world_width=96;
static struct { int map_redraw_busy; } net;
static int map_wrap_x(int x) { return (x%world_width+world_width)%world_width; }
static void map_view_cache_invalidate(void *,int,int,int) {}
static int map_view_cache_draw(void *,int) { return 0; }
static void rect_set_xywh(iptr out,int x,int y,int w,int h) { *(RECT *)out={x,y,x+w,y+h}; }
static void rect_intersect_into(LPRECT out,iptr a,iptr b) {
    RECT &r=*(RECT *)a,&s=*(RECT *)b;
    *out={std::max(r.left,s.left),std::max(r.top,s.top),std::min(r.right,s.right),std::min(r.bottom,s.bottom)};
}
struct MapView {
    int client_x=0,client_y=0,margin_x_u=0,margin_y_u=0;
    uptr origin_x_uptr=0,origin_y_uptr=0;
    int tile_width_u=64,tile_height_u=32,half_tile_width_u=32,half_tile_height_u=16;
    int tile_columns,half_view_width,field_2fc,field_304=0;
    RECT client_rect={0,0,320,240},clip=client_rect;
    std::vector<uint32_t> pixels=std::vector<uint32_t>(320*240);
    int fills=0,labels=0;
    void tile_to_screen(iptr,iptr,int,int);
    void tile_radius_rect(i32,i32,int,iptr);
    void draw_tile_radius(int,int,int,i32);
    void draw_all_tiles(i32);
    int tile_in_view(int x,int y) {
        return map_wrap_x(x-(int)origin_x_uptr)<tile_columns+half_view_width &&
            y>=(int)origin_y_uptr && y<(int)origin_y_uptr+field_2fc+1;
    }
    void set_clip(RECT *r) { clip=*r; }
    void fill(int color) {
        fills++;
        for(int y=clip.top;y<clip.bottom;y++)for(int x=clip.left;x<clip.right;x++)pixels[y*320+x]=color;
    }
    void draw_city_names(int,int,int) { labels++; }
    void draw_tile(int x,int y,int) {
        int sx,sy;tile_to_screen((iptr)&sx,(iptr)&sy,x,y);
        int w=tile_width_u,h=tile_height_u;
        // Unexplored art is deliberately distinct from the cleared background.
        uint32_t color=((x/4+y/4)&1) ? 0x10121d : 0x408030;
        for(int py=std::max(clip.top,sy);py<std::min(clip.bottom,sy+h);py++)
            for(int px=std::max(clip.left,sx);px<std::min(clip.right,sx+w);px++)
                if(abs(2*(px-sx)+1-w)*h+abs(2*(py-sy)+1-h)*w<w*h)
                    pixels[py*320+px]=color;
        // A translucent unit extending above the diamond also checks ordering
        // and that repeated repainting starts from clean terrain.
        if((x+3*y)%7==0) {
            for(int py=std::max(clip.top,sy-h/2);py<std::min(clip.bottom,sy+h/2);py++)
                for(int px=std::max(clip.left,sx+w/4);px<std::min(clip.right,sx+3*w/4);px++)
                    pixels[py*320+px]=(pixels[py*320+px]>>1 & 0x7f7f7f)+0x703020;
            // A transport's stacked banner/shadow extends left of the sprite.
            for(int py=std::max(clip.top,sy-h/2);py<std::min(clip.bottom,sy+h/8);py++)
                for(int px=std::max(clip.left,sx-w/32-1);px<std::min(clip.right,sx+w/8);px++)
                    pixels[py*320+px]=(pixels[py*320+px]>>1 & 0x7f7f7f)+0x204050;
        }
    }
};
static void gfx_fill_surface(iptr surface,int color) { ((MapView *)surface)->fill(color); }
#include "production.inc"
int main()
{
    int cases=0;
    for(int zoom=-7;zoom<=8;zoom++)for(int origin: {0,90})for(int radius: {0,1,2,3}) {
        MapView view;
        view.origin_x_uptr=origin;
        view.tile_width_u=8*(zoom+8);view.tile_height_u=4*(zoom+8);
        view.half_tile_width_u=view.tile_width_u/2;view.half_tile_height_u=view.tile_height_u/2;
        view.tile_columns=(320+view.tile_width_u-1)/view.tile_width_u+1;
        view.half_view_width=view.tile_columns;view.field_2fc=240/view.half_tile_height_u+1;
        for(int edge: {0,1,2}) {
            int dx=edge==0 ? 0 : edge==1 ? 160/view.half_tile_width_u : 320/view.half_tile_width_u;
            int y=edge==0 ? 0 : edge==1 ? 120/view.half_tile_height_u : 240/view.half_tile_height_u;
            dx+=(dx+y)&1;
            int x=map_wrap_x(origin+dx);
            view.draw_all_tiles(1);
            auto clean=view.pixels;
            RECT dirty;view.tile_radius_rect(x,y,radius,(iptr)&dirty);
            for(int repeat=0;repeat<3;repeat++) {
                // Model an old selection/animation image within the patch.
                view.set_clip(&dirty);view.fill(0xff00ff);view.set_clip(&view.client_rect);
                view.draw_tile_radius(x,y,radius,1);
                if(view.pixels!=clean) {
                    fprintf(stderr,"Partial redraw differs: zoom=%d origin=%d radius=%d edge=%d repeat=%d\n",
                            zoom,origin,radius,edge,repeat);
                    return 1;
                }
                assert(view.clip.left==0 && view.clip.top==0 && view.clip.right==320 && view.clip.bottom==240);
                cases++;
            }
        }
    }
    // Original graphics retain their incremental redraw without clearing.
    hd_enabled=0;MapView legacy;legacy.tile_columns=legacy.half_view_width=6;legacy.field_2fc=16;
    legacy.draw_tile_radius(4,4,1,1);assert(!legacy.fills && legacy.labels==1);
    printf("Partial redraw: %d fog/terrain/unit patches match full renders at every zoom, including wrap and viewport edges.\n",cases);
}
