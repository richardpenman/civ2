// SOURCE src/auto/mapview.cpp MapView::screen_to_tile
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include "map_camera.h"
using iptr=intptr_t;using i32=int;using i8=signed char;
struct { i8 pick_quadrant_dx_table[5]={0,-1,1,-1,1};
         i8 pick_quadrant_dy_table[5]={0,-1,-1,1,1}; } diag_neighbor_bits_block;
struct Mask {
    int w,h;
    int get_pixel(int x,int y) {
        double dx=x/(double)w-.5,dy=y/(double)h-.5;
        return 10+(fabs(dx)+fabs(dy)<=.5 ? 0 : (1+(dx>=0)+2*(dy>=0))*16);
    }
};
struct MapView {
    int client_x,client_y,client_width,client_height,margin_x_u,margin_y_u;
    int tile_width_u,tile_height_u,origin_x_u,origin_y_u;
    Mask map_surface;
    i32 screen_to_tile(iptr,iptr,i32,i32);
};
static MapCameraTransform transform;
int map_camera_pick(void *,int *x,int *y) { return camera_pick(&transform,x,y); }
static int map_wrap_x(int x) { return (x%100+100)%100; }
#include "production.inc"
int main()
{
    MapView view={7,35,160,96,0,0,32,16,98,20,{32,16}};
    for(double scale: {1.0,1.071,1.375,1.999})for(double ox: {0.0,-7.25,-19.5}) {
        transform={scale,ox,-9.75,7,35,160,96};
        for(int y=34;y<=131;y++)for(int x=6;x<=167;x++) {
            double sx=7+(x-7-ox)/scale,sy=35+(y-35+9.75)/scale;
            int tx=-1,ty=-1,result=view.screen_to_tile((iptr)&tx,(iptr)&ty,x,y);
            bool inside=x>=7 && x<167 && y>=35 && y<131 && sx>=7 && sx<167 && sy>=35 && sy<131;
            assert((result==0)==inside);
            if(!inside)continue;
            int px=(int)floor(sx)-7,py=(int)floor(sy)-35;
            // Independent nearest diamond, including the cylinder seam.
            double best=1e10;bool found=false;
            for(int wy=19;wy<=34;wy++)for(int wx=97;wx<=110;wx++)if(!((wx+wy)&1)) {
                double d=fabs(px-(wx-98)*16)/16+fabs(py-(wy-20)*8)/8;
                if(d<best)best=d;
            }
            for(int wy=19;wy<=34;wy++)for(int wx=97;wx<=110;wx++)if(!((wx+wy)&1)) {
                double d=fabs(px-(wx-98)*16)/16+fabs(py-(wy-20)*8)/8;
                if(fabs(d-best)<1e-8 && tx==map_wrap_x(wx) && ty==wy)found=true;
            }
            assert(found);
        }
    }
    puts("Fractional camera picking: viewport bounds, diamonds and wrapping passed.");
}
