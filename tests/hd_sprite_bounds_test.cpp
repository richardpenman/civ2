// SOURCE src/auto/graphics.cpp gfx_scale_length
// SOURCE src/auto/graphics.cpp Sprite::blit_to_surface
// SOURCE src/auto/graphics.cpp Sprite::blit_silhouette_to_surface
// Exercise the actual sprite bridge, replacing only raster writes. Its HD
// synchronization must cover exactly the scaled, offset, clipped indexed draw.
#include <cassert>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include "native.h"
#include "win32_types.h"
#define _fclose fclose
#include "scoped.h"
using i32 = int;
using i8 = char;
using u8 = unsigned char;
using uint = unsigned;
struct GdiDib {};
struct DibSurface {
    iptr bits = 1;
    int clip_left = 2, clip_top = 3, clip_right = 80, clip_bottom = 60;
    int surface_height() { return 64; }
    GdiDib *get_dib() { return nullptr; }
    int get_pitch() { return 96; }
    int get_bits() { return (int)bits; }
};
struct Sprite {
    int frame_rect = 0, frame_top = 0, fw = 64, fh = 48;
    int bounds_rect = 0, bounds_top = 0, bw = 64, bh = 48;
    int offset_x = 0, offset_y = 0, key_color = 7;
    iptr data_ptr = 1;
    int width() { return fw; }
    int height() { return fh; }
    int get_width() { return bw; }
    int get_height() { return bh; }
    int *blit_to_surface(int *, DibSurface *, int, int, int);
    int *blit_silhouette_to_surface(int *, DibSurface *, int, int, int, i32);
};
static struct { int num, den; } gfx_scale;
static int table[1024];
static struct { int *table; } gfx_scale_tables = {table};
static int hd_enabled = 1, bound, commits, changes;
static RECT synchronized;
static char str_error_sprite_size_exceeds_scale_table_si[] = "oversized";
static char str_error_sprite_size_exceeds_scale_table_si_2[] = "oversized";
static void dbg_log(const char *) {}
static int dib_is_top_down(GdiDib *) { return 1; }
static int rect_width(const RECT *r) { return r->right-r->left; }
static int rect_height(const RECT *r) { return r->bottom-r->top; }
static BOOL SetRect(RECT *r,int l,int t,int rr,int b) { *r={l,t,rr,b};return 1; }
static BOOL OffsetRect(RECT *r,int x,int y) { r->left+=x;r->right+=x;r->top+=y;r->bottom+=y;return 1; }
static BOOL IntersectRect(RECT *out,const RECT *a,const RECT *b) {
    RECT r={std::max(a->left,b->left),std::max(a->top,b->top),
            std::min(a->right,b->right),std::min(a->bottom,b->bottom)};
    if (r.left>=r.right || r.top>=r.bottom) { *out={};return 0; }
    *out=r;return 1;
}
static int hd_sprite_draw_keyed(const void *,void *,int,int,int,int,int,int,int,int,int,int) { return bound; }
static void hd_commit_rect(void *,int x,int y,int w,int h) { commits++;synchronized={x,y,x+w,y+h}; }
static void hd_changed(void *,int x,int y,int w,int h) { changes++;synchronized={x,y,x+w,y+h}; }
static void sprite_blit_rows_keyed(iptr,int,iptr,i32,int,int,int,int,int,int,int,int,i32,int,int,int) {}
static void sprite_blit_rows_solid(iptr,int,iptr,i32,int,int,int,int,int,int,int,int,i32,int,int,int,i32) {}
#include "production.inc"

int main() {
    DibSurface surface;
    unsigned cases=0;
    for (int zoom=-7;zoom<=8;zoom++) {
        gfx_scale={zoom+8,8};
        for (int i=0;i<1024;i++)
            table[i]=gfx_scale.num<8 ? (i*8+gfx_scale.num-1)/gfx_scale.num : ((i+1)*8-1)/gfx_scale.num;
        // The game's status rectangles use truncation; sprite sizes agree.
        for (int size : {12,14,20,22,48,64}) assert(gfx_scale_length(size,0,0)==size*(zoom+8)/8);
        for (bound=0;bound<=1;bound++) for (int silhouette=0;silhouette<2;silhouette++)
            for (int trim : {0,3}) for (int offset : {-5,0,7})
                for (int x : {-130,-5,0,17,79,100}) for (int y : {-100,-4,0,13,59,80}) {
                    Sprite s; s.bounds_rect=trim;s.bounds_top=trim;s.bw-=trim;s.bh-=trim;
                    s.offset_x=offset;s.offset_y=-offset;
                    RECT result;
                    commits=changes=0;
                    if (silhouette) s.blit_silhouette_to_surface((int *)&result,&surface,-1,x,y,23);
                    else s.blit_to_surface((int *)&result,&surface,-1,x,y);
                    assert(commits==bound && changes==!bound);
                    assert(!memcmp(&result,&synchronized,sizeof result));
                    if (result.right>result.left) {
                        assert(result.left>=surface.clip_left && result.right<=surface.clip_right);
                        assert(result.top>=surface.clip_top && result.bottom<=surface.clip_bottom);
                    }
                    cases++;
                }
    }
    printf("Sprite scaling/clipping: %u cases passed across zoom -7..8.\n",cases);
}
