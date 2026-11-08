// SOURCE src/platform/util.c rect_empty
// SOURCE src/platform/util.c rect_intersect
// SOURCE src/platform/util.c rgn_add
// SOURCE src/platform/util.c rgn_set
// SOURCE src/platform/util.c rgn_copy
// SOURCE src/platform/util.c rgn_intersect_rect
// SOURCE src/platform/util.c rgn_subtract_rect
// SOURCE src/platform/gdi.c compose_gpu_piece
// SOURCE src/platform/gdi.c compose_gpu_windows
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
typedef uint32_t DWORD;
struct RECT { int left,top,right,bottom; };
struct Region { int n,cap;RECT *r; };
static void rgn_free(Region *r) { free(r->r);memset(r,0,sizeof *r); }
struct Bitmap { int w,h;DWORD pixels[48*32]; };
struct Window { DWORD style;int minimized,host_clipped,host_dx,host_dy;RECT rect,shown;Bitmap *surface; };
static const DWORD WS_VISIBLE=1;
static const int screen_w=48,screen_h=32;
static Bitmap output={48,32,{}},background={48,32,{}};
static Bitmap *screen=&output,*root=&background;
static DWORD gpu_pixels[48*32],reference[48*32];
static unsigned copied[48*32];
static void *bmp_row(Bitmap *b,int y) { return b->pixels+y*b->w; }
static void wnd_shown_rect(Window *w,RECT *r) { *r=w->shown; }
static Bitmap *wnd_surface(Window *w) { return w->surface; }
static int compose_camera_piece(Window *,Bitmap *,int,int,const RECT *) { return 0; }
static void hd_composite(Bitmap *dst,Bitmap *src,int x,int y,int l,int t,int r,int b)
{
    assert(dst==screen);
    for(int py=t;py<b;py++)for(int px=l;px<r;px++) {
        assert(px>=0 && px<48 && py>=0 && py<32 && px-x>=0 && px-x<src->w && py-y>=0 && py-y<src->h);
        gpu_pixels[py*48+px]=src->pixels[(py-y)*src->w+px-x];
        copied[py*48+px]++;
    }
}
static RECT *region_realloc(void *p,size_t n) { return (RECT *)realloc(p,n); }
#define realloc region_realloc
#include "production.inc"
#undef realloc
static unsigned seed=123;
static int random(int n) { seed=seed*1664525u+1013904223u;return (seed>>8)%n; }
int main()
{
    Bitmap surfaces[6];Window windows[6],*order[6];
    for(int i=0;i<48*32;i++)root->pixels[i]=0x12340000+i;
    for(int scene=0;scene<2000;scene++) {
        memset(copied,0,sizeof copied);
        for(int i=0;i<6;i++) {
            Bitmap &b=surfaces[i];Window &w=windows[i];
            b.w=1+random(48);b.h=1+random(32);
            for(int p=0;p<b.w*b.h;p++)b.pixels[p]=(i+1)*0x10000+p;
            int x=random(80)-20,y=random(50)-10;
            w={random(4) ? WS_VISIBLE : 0,random(5)==0,random(2),random(9)-4,random(9)-4,
                {x,y,x+b.w,y+b.h},{x,y,x+b.w,y+b.h},&b};
            if(w.host_clipped) {
                w.shown.left+=random(8);w.shown.top+=random(8);
                w.shown.right+=random(12)-6;w.shown.bottom+=random(12)-6;
            }
            order[i]=&w;
        }
        memcpy(reference,root->pixels,sizeof reference);
        for(int i=5;i>=0;i--) {
            Window &w=windows[i];Bitmap &b=*w.surface;
            if(!(w.style&WS_VISIBLE) || w.minimized)continue;
            int dx=w.rect.left+(w.host_clipped ? w.host_dx : 0),dy=w.rect.top+(w.host_clipped ? w.host_dy : 0);
            for(int y=0;y<32;y++)for(int x=0;x<48;x++)
                if(x>=w.shown.left && x<w.shown.right && y>=w.shown.top && y<w.shown.bottom &&
                   x>=dx && x<dx+b.w && y>=dy && y<dy+b.h)
                    reference[y*48+x]=b.pixels[(y-dy)*b.w+x-dx];
        }
        compose_gpu_windows(order,6);
        assert(!memcmp(reference,gpu_pixels,sizeof reference));
        assert(!memcmp(reference,screen->pixels,sizeof reference));
        for(unsigned count:copied)assert(count==1);
    }
    puts("HD composition: 2,000 overlapping/clipped window scenes match, with each pixel copied once");
}
