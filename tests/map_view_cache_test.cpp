// SOURCE src/game/map_view_cache.cpp copy_pixels
// SOURCE src/game/map_view_cache.cpp cell_rect
// SOURCE src/game/map_view_cache.cpp valid_rect
// SOURCE src/game/map_view_cache.cpp crop
// SOURCE src/game/map_view_cache.cpp recenter
// SOURCE src/game/map_view_cache.cpp fill
// SOURCE src/game/map_view_cache.cpp map_view_cache_invalidate
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
using iptr=intptr_t;
struct RECT { int left,top,right,bottom; };
static int minimum(int a,int b) { return std::min(a,b); }
static int maximum(int a,int b) { return std::max(a,b); }
static int IntersectRect(RECT *r,const RECT *a,const RECT *b) {
    *r={maximum(a->left,b->left),maximum(a->top,b->top),minimum(a->right,b->right),minimum(a->bottom,b->bottom)};
    return r->left<r->right && r->top<r->bottom;
}
static void OffsetRect(RECT *r,int x,int y) { r->left+=x;r->right+=x;r->top+=y;r->bottom+=y; }
enum { CELL=128,MAX_CELLS=1024,SLOTS=3 };
static int flat;
static struct { int width=256,height=256; } map;
static int wrap_delta(int x) { if(!flat) { if(x>map.width/2)x-=map.width;if(x<-map.width/2)x+=map.width; }return x; }
struct DibSurface {
    uintptr_t bits;int width,height;
    std::vector<unsigned char> pixels;
    void init(int w,int h) { width=w;height=h;pixels.resize(w*h);bits=(uintptr_t)pixels.data(); }
    intptr_t pixel_ptr(int x,int y) { assert(x>=0 && x<width && y>=0 && y<height);return bits+y*width+x; }
};
struct MapView: DibSurface {
    int client_width,client_height,center_x,center_y,origin_x,origin_y,margin_x=0,margin_y=0;
    int half_tile_width=16,half_tile_height=8,tile_width=32;
    void update_layout() {
        int width=std::min(client_width,map.width*half_tile_width);
        int height=std::min(client_height,map.height*half_tile_height);
        margin_x=(client_width-width)/2;margin_y=(client_height-height)/2;
        origin_x=center_x-width/(2*half_tile_width);
        if(flat)origin_x=std::max(0,std::min(origin_x,map.width-width/half_tile_width));
        else origin_x=(origin_x%map.width+map.width)%map.width;
        origin_y=center_y-height/(2*half_tile_height);
        origin_y=std::max(0,std::min(origin_y,map.height-height/half_tile_height));
    }
    void tile_radius_rect(int x,int y,int radius,iptr out) {
        int px=wrap_delta(x-origin_x-1)*half_tile_width,py=(y-origin_y-1)*half_tile_height;
        *(RECT *)out={px-(2*radius+1)*half_tile_width,py-(2*radius+1)*half_tile_height,
                     px+(2*radius+3)*half_tile_width,py+(2*radius+3)*half_tile_height};
    }
};
struct CachedView { MapView *view;int cols,rows,civ;unsigned used;unsigned char valid[MAX_CELLS]; };
static CachedView entries[SLOTS];
static MapView *painting,*owner;
static int busy;
static int map_camera_busy() { return busy; }
static int map_camera_owns(void *v) { return v==owner; }
static uint64_t clock_us;
static uint64_t host_time_us() { return clock_us; }
static void hd_copy(void *,void *,int,int,int,int,int,int) {}
static void hd_commit_rect(void *,int,int,int,int) {}
static void host_memory_move(void *dst,const void *src,uint32_t size) { memmove(dst,src,size); }
static int changed;
static unsigned char expected(MapView &v,int x,int y) {
    if(x<v.margin_x || x>=v.margin_x+map.width*16 || y<v.margin_y || y>=v.margin_y+map.height*8)return 0;
    int wy=v.origin_y*8+y-v.margin_y;
    if(wy<0 || wy>=map.height*8)return 0;
    wy+=8;
    if(flat && v.origin_x*16+x-v.margin_x>=map.width*16)return 0;
    int wx=((v.origin_x+1)*16+x-v.margin_x)%(map.width*16);
    return (unsigned char)((wx*17+wy*31+(changed && wx>=2000 && wx<2016 && wy>=800 && wy<808 ? 71:0))%251);
}
static void paint_rect(CachedView &e,RECT r) {
    for(int y=r.top;y<r.bottom;y++)for(int x=r.left;x<r.right;x++)
        e.view->pixels[y*e.view->width+x]=expected(*e.view,x,y);
    clock_us+=1000;
}
#include "production.inc"
int main()
{
    MapView view,cache;owner=&view;
    view.client_width=420;view.client_height=300;view.center_x=128;view.center_y=100;view.update_layout();
    cache.client_width=676;cache.client_height=556;cache.init(676,556);
    cache.center_x=128;cache.center_y=100;cache.update_layout();
    CachedView &e=entries[0];e.view=&cache;e.cols=6;e.rows=5;
    RECT all={0,0,676,556};
    assert(fill(e,all,2000)==2); // Stop after the budget, leaving work pending.
    fill(e,all,0);
    unsigned seed=73;
    for(int n=0;n<1000;n++) {
        seed=1664525*seed+1013904223;
        view.center_x=(view.center_x+((seed>>24)%13)-6+256)%256;
        view.center_y+=((seed>>16)%9)-4;
        view.update_layout();recenter(e,&view);
        if(n%7==0) { changed=!changed;map_view_cache_invalidate(&view,125,100,0); }
        RECT r;crop(&view,&cache,r);
        assert(r.left>=0 && r.top>=0 && r.right<=cache.width && r.bottom<=cache.height);
        fill(e,r,0);
        for(int y=r.top;y<r.bottom;y++)for(int x=r.left;x<r.right;x++)
            assert(cache.pixels[y*cache.width+x]==expected(cache,x,y));
        if(n%11==0)fill(e,all,clock_us+2000);
    }
    fill(e,all,0);busy=1;map_view_cache_invalidate(&view,0,0,-1);assert(valid_rect(e,all));
    busy=0;map_view_cache_invalidate(&view,0,0,-1);assert(!valid_rect(e,all));
    // Whole-world cache images have stationary margins. Reusing those pixels
    // as wrapped terrain used to leave black cells behind the live city labels.
    for(int bounded=0;bounded<2;bounded++)for(int width: {20,32,48}) {
        flat=bounded;map.width=width;map.height=40;
        memset(e.valid,0,sizeof e.valid);changed=0;
        cache.center_x=view.center_x=width/2;cache.center_y=view.center_y=20;
        view.update_layout();cache.update_layout();fill(e,all,0);
        for(int n=0;n<160;n++) {
            view.center_x=(n*7)%width;view.center_y=(n*3)%40;
            view.update_layout();recenter(e,&view);
            RECT r;crop(&view,&cache,r);fill(e,r,0);
            assert(r.left>=0 && r.top>=0 && r.right<=cache.width && r.bottom<=cache.height);
            for(int y=0;y<view.client_height;y++)for(int x=0;x<view.client_width;x++)
                if(cache.pixels[(r.top+y)*cache.width+r.left+x]!=expected(view,x,y)) {
                    printf("bounded=%d width=%d n=%d pixel=%d,%d crop=%d,%d origin=%d,%d cacheorigin=%d,%d margin=%d,%d cachemargin=%d,%d got=%d expected=%d\n",flat,width,n,x,y,r.left,r.top,view.origin_x,view.origin_y,cache.origin_x,cache.origin_y,view.margin_x,view.margin_y,cache.margin_x,cache.margin_y,cache.pixels[(r.top+y)*cache.width+r.left+x],expected(view,x,y));return 1;
                }
        }
    }
    puts("Viewport cache: 1,000 scrolls and 960 bounded/wrapped whole-world views match fresh pixels.");
}
