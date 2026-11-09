// SOURCE src/platform/util.c rect_empty
// SOURCE src/platform/util.c rect_intersect
// SOURCE src/platform/util.c rgn_add
// SOURCE src/platform/util.c rgn_set
// SOURCE src/platform/util.c rgn_copy
// SOURCE src/platform/util.c rgn_intersect_rect
// SOURCE src/platform/util.c rgn_subtract_rect
// SOURCE src/platform/window.c wnd_client_screen
// SOURCE src/platform/window.c wnd_top
// SOURCE src/platform/window.c wnd_screen_rect
// SOURCE src/platform/window.c wnd_is_visible
// SOURCE src/platform/window.c wnd_visible_region
// SOURCE src/platform/gdi.c compose_gpu_piece
// SOURCE src/platform/gdi.c compose_camera_piece
// SOURCE src/platform/gdi.c compose_gpu_windows
// Exercise the real visibility/composition code with independent map backing,
// overlapping child windows, ancestor clipping, and a separate modal dialog.
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <initializer_list>
#include "map_camera.h"
using DWORD=uint32_t;
struct RECT { int left,top,right,bottom; };
struct POINT { int x,y; };
struct RGBQUAD { unsigned char rgbBlue,rgbGreen,rgbRed,rgbReserved; };
struct Region { int n,cap;RECT *r; };
static void rgn_clear(Region *r) { r->n=0; }
static void rgn_free(Region *r) { free(r->r);*r={}; }
static constexpr int screen_w=64,screen_h=48;
struct Bitmap { int w=64,h=48,bpp=32,bottom_up=0;RGBQUAD colors[256];unsigned char bits[64*48*4]; };
struct Window {
    DWORD style=0;int minimized=0,host_clipped=0,host_dx=0,host_dy=0;
    RECT rect{},client{};Window *parent=nullptr,*children=nullptr,*next=nullptr;Bitmap *surface=nullptr;
};
using HWND=Window *;
static constexpr DWORD WS_VISIBLE=1,WS_CLIPSIBLINGS=2,WS_CLIPCHILDREN=4;
static Window desktop_storage,main_wnd,map_wnd,city_wnd,control_wnd,modal_wnd;
static Window *desktop=&desktop_storage;
static Bitmap output,background,shared,map_pixels,modal;
static Bitmap *screen=&output,*root=&background;
static DWORD gpu[64*48],reference[64*48],logical[64*48];
static unsigned copies[64*48];
static MapCameraTransform transform;
static unsigned char *bmp_row(const Bitmap *b,int y) {
    assert(y>=0 && y<b->h);
    return (unsigned char *)(b->bits+(b->bottom_up ? b->h-1-y : y)*b->w*(b->bpp/8));
}
static DWORD pixel(Bitmap *b,int x,int y) {
    assert(x>=0 && x<b->w);
    if(b->bpp==32)return ((DWORD *)bmp_row(b,y))[x];
    RGBQUAD c=b->colors[((unsigned char *)bmp_row(b,y))[x]];
    return (DWORD)c.rgbRed<<16|(DWORD)c.rgbGreen<<8|c.rgbBlue;
}
static Bitmap *bmp_of_bits(void *p) { return p==map_pixels.bits ? &map_pixels : nullptr; }
void *map_camera_pixels() { return map_pixels.bits; }
uintptr_t map_camera_get(MapCameraTransform *c) { *c=transform;return (uintptr_t)&map_wnd; }
static Window *wnd_get(HWND h) { return h; }
static Bitmap *wnd_surface(Window *w) { return w->surface; }
static void wnd_shown_rect(Window *w,RECT *r) { *r=w->rect; }
static void hd_composite(Bitmap *,Bitmap *src,int dx,int dy,int l,int t,int r,int b) {
    for(int y=t;y<b;y++)for(int x=l;x<r;x++) { gpu[y*64+x]=pixel(src,x-dx,y-dy);copies[y*64+x]++; }
}
static void hd_composite_camera(Bitmap *,Bitmap *src,int sx,int sy,int sw,int sh,
                                 double dx,double dy,double w,double h,int l,int t,int r,int b) {
    assert(src==&map_pixels); // Never stretch a shared surface containing dialogs.
    for(int y=t;y<b;y++)for(int x=l;x<r;x++) {
        gpu[y*64+x]=pixel(src,sx+(int)((x-dx)*sw/w),sy+(int)((y-dy)*sh/h));copies[y*64+x]++;
    }
}
int map_camera_labels(MapCameraLabel *,int,double *h,int) { *h=8;return 1; }
int map_camera_unit_labels(MapCameraLabel *,int,double *h) { *h=8;return 1; }
int extras_camera_messages(MapCameraLabel *,int,double *h) { *h=8;return 1; }
static void text_camera_labels(Bitmap *,const MapCameraLabel *,int,double,int,int,const RECT *r,int kind) {
    for(int y=r->top;y<r->bottom;y++)for(int x=r->left;x<r->right;x++)
        if((x+y)%7==kind)gpu[y*64+x]=0xf00000+kind;
}
static RECT *region_realloc(void *p,size_t n) { return (RECT *)realloc(p,n); }
#define realloc region_realloc
#include "production.inc"
#undef realloc
static bool contains(RECT r,int x,int y) { return x>=r.left && x<r.right && y>=r.top && y<r.bottom; }
int main()
{
    desktop->rect={0,0,64,48};desktop->client=desktop->rect;
    main_wnd.style=WS_VISIBLE;main_wnd.parent=desktop;main_wnd.rect=desktop->rect;main_wnd.surface=&shared;
    map_wnd.style=WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;map_wnd.parent=&main_wnd;
    map_wnd.rect={4,6,54,42};map_wnd.client={0,0,50,36};map_wnd.children=&control_wnd;
    city_wnd.parent=&main_wnd;city_wnd.rect={16,12,40,29};city_wnd.next=&map_wnd;main_wnd.children=&city_wnd;
    control_wnd.parent=&map_wnd;control_wnd.style=WS_VISIBLE;control_wnd.rect={10,5,16,10};
    modal_wnd.parent=desktop;modal_wnd.rect={24,20,50,36};modal_wnd.surface=&modal;modal.w=26;modal.h=16;
    Window *order[]={&modal_wnd,&main_wnd};
    for(int i=0;i<64*48;i++)((DWORD *)background.bits)[i]=0x111111;
    for(int i=0;i<26*16;i++)((DWORD *)modal.bits)[i]=0xabcdef;
    map_pixels.w=50;map_pixels.h=36;
    for(int i=0;i<256;i++)map_pixels.colors[i]={(unsigned char)i,(unsigned char)(255-i),(unsigned char)(i/2),0};
    int scenes=0;
    for(int bpp: {8,32})for(int bottom_up: {0,1})for(double scale: {1.0,1.25,1.7,2.0})
    for(int mode=0;mode<4;mode++)for(int popup=0;popup<2;popup++)for(int clip=0;clip<2;clip++) {
        map_pixels.bpp=bpp;map_pixels.bottom_up=bottom_up;
        for(int y=0;y<36;y++)for(int x=0;x<50;x++) {
            if(bpp==8)((unsigned char *)bmp_row(&map_pixels,y))[x]=(x+3*y)%256;
            else ((DWORD *)bmp_row(&map_pixels,y))[x]=0x210000+y*50+x;
        }
        main_wnd.client={0,0,clip ? 44 : 64,clip ? 32 : 48};
        city_wnd.style=mode==0 ? 0 : WS_VISIBLE;city_wnd.minimized=mode==2;
        map_wnd.minimized=mode==3;modal_wnd.style=popup ? WS_VISIBLE : 0;
        transform={scale,-44*(scale-1)*.33,-28*(scale-1)*.67,2,3,44,28};
        memset(copies,0,sizeof copies);
        for(int y=0;y<48;y++)for(int x=0;x<64;x++) {
            bool city=(mode==1 || mode==3) && contains(city_wnd.rect,x,y);
            bool child=contains({14,11,20,16},x,y),pop=popup && contains(modal_wnd.rect,x,y);
            DWORD normal=city ? 0xdd4400 : child ? 0xccbb00 : 0x246810;
            ((DWORD *)shared.bits)[y*64+x]=normal;
            bool visible=mode!=3 && contains({6,9,50,37},x,y) && contains(main_wnd.client,x,y) && !city && !child && !pop;
            DWORD value=pop ? 0xabcdef : normal;
            if(visible)value=pixel(&map_pixels,2+(int)((x-6-transform.x)/scale),3+(int)((y-9-transform.y)/scale));
            logical[y*64+x]=value;
            if(visible && (x+y)%7<4)value=0xf00000+(x+y)%7;
            reference[y*64+x]=value;
        }
        compose_gpu_windows(order,2);
        assert(!memcmp(reference,gpu,sizeof gpu));assert(!memcmp(logical,screen->bits,sizeof logical));
        for(unsigned n:copies)assert(n==1);
        scenes++;
    }
    printf("Camera composition: %d sibling/modal/ancestor/zoom/backing scenes passed.\n",scenes);
}
