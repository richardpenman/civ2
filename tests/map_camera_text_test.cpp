// SOURCE src/platform/text.c camera_text_quad
// SOURCE src/platform/text.c text_camera_labels
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include "map_camera.h"
enum { MINH=6,MAXH=48 };
struct RECT { int left,top,right,bottom; };
struct Bitmap { int w,h,hd_gpu;uint32_t *hd; };
struct AaGlyph { int x,y,w,h; };
struct Glyph { int dadv; };
struct FontFace { int height;int tm[11];Glyph g[256];AaGlyph *aa; };
static FontFace *faces[4][MAXH+1][2][2];
static FontFace font,by_size[MAXH+1];
static int selected_size;
static AaGlyph glyphs[256];
static unsigned char coverage[15],palette[768];
static int hd_density,font_data=1,aa_data=1,fallback,reads,uploads,capabilities=3;
static int host_gpu_available() { return capabilities; }
struct Draw { int x,y,w,h,l,t,r,b;uint32_t color; };
static std::vector<Draw> draws;
static void load_fonts() { assert(false); }
static void load_aa() { assert(false); }
static const unsigned char *hd_palette() { return palette; }
static const unsigned char *aa_coverage(AaGlyph *) { return coverage; }
static int text_width(FontFace *f,const char *,int n) { selected_size=f->height;return n*4; }
static void hd_gpu_text_begin() {}
static void hd_gpu_text_end() {}
static int hd_gpu_text(Bitmap *,const unsigned char *,int sw,int sh,int x,int y,int w,int h,
                       int l,int t,int r,int b,uint32_t color) {
    // At small display sizes use the hinted glyphs directly, without resizing.
    assert((sw==1 && sh==1) || (sw==3 && sh==5 && (selected_size==MAXH || (sw==w && sh==h))));
    draws.push_back({x,y,w,h,l,t,r,b,color});return !fallback;
}
static unsigned aa_blend(unsigned dst,unsigned text,int aa) { return aa==16 ? text : dst; }
static void hd_read_region(Bitmap *,int,int,int,int) { reads++; }
static void hd_upload_region(Bitmap *,int,int,int,int) { uploads++; }
#include "production.inc"
int main()
{
    memset(coverage,16,sizeof coverage);palette[3]=200;palette[4]=150;palette[5]=100;
    font.aa=glyphs;font.tm[0]=6;
    for(int c=0;c<256;c++) { font.g[c].dadv=4;glyphs[c]={0,0,3,5}; }
    for(int h=MINH;h<=MAXH;h++) { by_size[h]=font;by_size[h].height=h;faces[0][h][0][0]=faces[0][h][1][0]=faces[1][h][0][0]=&by_size[h]; }
    MapCameraLabel label={40,30,1,"AB",0,0,0,0,0};RECT clip={49,28,54,36};
    for(hd_density=1;hd_density<=4;hd_density++) {
        std::vector<uint32_t> pixels(64*48*hd_density*hd_density,0xff111111);
        Bitmap dst={64,48,1,pixels.data()};draws.clear();fallback=0;
        text_camera_labels(&dst,&label,1,6,10,0,&clip,0);
        assert(!draws.empty());int old_x=draws.back().x;
        label.x+=1.0/hd_density;draws.clear();
        text_camera_labels(&dst,&label,1,6,10,0,&clip,0);
        assert(draws.back().x==old_x+1); // One physical pixel, even at density 4.
        for(auto &v:draws)assert(v.l>=clip.left*hd_density && v.t>=clip.top*hd_density &&
                                v.r<=clip.right*hd_density && v.b<=clip.bottom*hd_density);
        fallback=1;reads=uploads=0;
        text_camera_labels(&dst,&label,1,6,10,0,&clip,0);
        assert(reads>0 && reads==uploads);
        bool colored=false;
        for(int y=0;y<48*hd_density;y++)for(int x=0;x<64*hd_density;x++) {
            auto p=pixels[y*64*hd_density+x];if(p==0xffc89664)colored=true;
            if(x<clip.left*hd_density || x>=clip.right*hd_density || y<clip.top*hd_density || y>=clip.bottom*hd_density)
                assert(p==0xff111111);
        }
        assert(colored);label.x=40;
        // Population badges use unscaled hinted glyphs and opaque, clipped backgrounds.
        draws.clear();fallback=0;
        text_camera_labels(&dst,&label,1,6.875,10,0,&clip,1);
        assert(draws.size()>=3 && draws[0].color==0 && draws[1].color==0xc89664);
        for(size_t k=2;k<draws.size();k++)assert(draws[k].color==0);
        fallback=1;reads=uploads=0;
        text_camera_labels(&dst,&label,1,6.875,10,0,&clip,1);
        assert(reads>=3 && reads==uploads);
        for(int y=0;y<48*hd_density;y++)for(int x=0;x<64*hd_density;x++)
            if(x<clip.left*hd_density || x>=clip.right*hd_density || y<clip.top*hd_density || y>=clip.bottom*hd_density)
                assert(pixels[y*64*hd_density+x]==0xff111111);
        fallback=0;
        // Unit order glyphs have no shadow and cannot spill into the HP bar.
        label.flags=1;label.left=39;label.right=44;label.top=29;label.bottom=31;
        draws.clear();text_camera_labels(&dst,&label,1,6.875,10,0,&clip,2);
        assert(!draws.empty());
        for(auto &v:draws)assert(v.t>=29*hd_density && v.b<=31*hd_density && v.l>=49*hd_density && v.r<=54*hd_density);
        label.flags=0;
        int previous=MAXH;
        for(int step=1000;step>=625;step--) {
            double size=11*step/1000.0;
            draws.clear();text_camera_labels(&dst,&label,1,size,10,0,&clip,1);
            assert(selected_size==(int)(size*hd_density+.5));
            assert((selected_size<=previous && previous-selected_size<=1) || previous==MAXH);
            previous=selected_size;
        }
        // Notifications retain their independent bold UI size and full text.
        text_camera_labels(&dst,&label,1,14,10,0,&clip,3);
        assert(selected_size==(14*hd_density>MAXH ? MAXH : 14*hd_density));

        // A whole-world label layer needs one transfer on GPUs without gamma
        // blending, including overlapping names, badge backgrounds and shadows.
        MapCameraLabel crowded[100];
        for(int i=0;i<100;i++) {
            crowded[i]=label;crowded[i].x=39+(i%3);crowded[i].y=29+(i%4);
            crowded[i].color=i%2;
        }
        for(int kind=0;kind<4;kind++) {
            std::fill(pixels.begin(),pixels.end(),0xff111111);
            capabilities=3;fallback=1;reads=uploads=0;
            text_camera_labels(&dst,crowded,100,6,10,0,&clip,kind);
            auto reference=pixels;
            assert(reads>100 && reads==uploads);
            std::fill(pixels.begin(),pixels.end(),0xff111111);
            capabilities=1;reads=uploads=0;draws.clear();
            text_camera_labels(&dst,crowded,100,6,10,0,&clip,kind);
            assert(reads==1 && uploads==1 && draws.empty());
            assert(pixels==reference);
        }
        capabilities=3;fallback=0;
    }
    puts("Camera text: native glyph sampling, physical-pixel placement, clipping and fallback at densities 1-4 passed.");
}
