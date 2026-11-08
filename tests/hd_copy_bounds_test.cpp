// SOURCE src/auto/graphics.cpp gfx_copy_pixels
// The HD commit must describe logical destination rows before the indexed
// copy converts its local coordinates for a bottom-up DIB.
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
using i8 = char;
using i32 = int;
using uint = unsigned;
static int hd_enabled=1, copied, committed;
static int bounds[4], copy_bounds[4];
static void hd_copy(void *,void *,int x,int y,int,int,int w,int h) {
    copied++;copy_bounds[0]=x;copy_bounds[1]=y;copy_bounds[2]=w;copy_bounds[3]=h;
}
static void hd_commit_rect(void *,int x,int y,int w,int h) {
    committed++;bounds[0]=x;bounds[1]=y;bounds[2]=w;bounds[3]=h;
}
void // the extracted definition's return type is on its preceding source line
#include "production.inc"
int main() {
    static_assert(sizeof(void *)==4,"run with the 32-bit source-probe compiler");
    unsigned char src[16*12],dst[16*12];
    for (int sb=0;sb<2;sb++) for (int db=0;db<2;db++) for (hd_enabled=0;hd_enabled<2;hd_enabled++) {
        for (int y=0;y<12;y++) for (int x=0;x<16;x++) src[(sb ? 11-y : y)*16+x]=y*16+x;
        memset(dst,255,sizeof dst);copied=committed=0;
        gfx_copy_pixels((int)(intptr_t)src,(int)(intptr_t)dst,2,3,5,4,7,5,12,12,sb ? -16 : 16,db ? -16 : 16);
        assert(copied==hd_enabled && committed==hd_enabled);
        if (hd_enabled) {
            const int want[]={5,4,7,5};
            assert(!memcmp(want,bounds,sizeof want) && !memcmp(want,copy_bounds,sizeof want));
        }
        for (int y=0;y<12;y++) for (int x=0;x<16;x++)
            assert(dst[(db ? 11-y : y)*16+x] == (x>=5 && x<12 && y>=4 && y<9 ? (y-1)*16+x-3 : 255));
    }
    puts("HD copy bounds: indexed rows and logical commit agree");
}
