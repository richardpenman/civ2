// SOURCE src/platform/gdi.c xfer_copy_rect
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <initializer_list>
typedef uint32_t DWORD;
struct RECT { int left,top,right,bottom; };
struct Bitmap { int w,h,bpp,bottom_up,stride;unsigned char *bits; };
struct Xfer { Bitmap *src,*dst;int identity;DWORD map32[256]; };
static unsigned char *bmp_row(Bitmap *b,int y) { return b->bits+(b->bottom_up ? b->h-1-y : y)*b->stride; }
#include "production.inc"
int main()
{
    unsigned char src[16*12*4],dst[16*12*4];
    for(int sb: {8,32})for(int db: {8,32})for(int bottom=0;bottom<4;bottom++) {
        Bitmap a={16,12,sb,bottom&1,16*sb/8,src},b={16,12,db,bottom&2,16*db/8,dst};
        Xfer xf={&a,&b,sb==8 && db==8,{}};
        for(int i=0;i<256;i++)xf.map32[i]=i*0x010101;
        for(int y=0;y<12;y++)for(int x=0;x<16;x++) {
            if(sb==8)bmp_row(&a,y)[x]=y*16+x;
            else ((DWORD *)bmp_row(&a,y))[x]=0xaa000000u+(y*16+x)*0x010101;
        }
        memset(dst,0xcc,sizeof dst);
        RECT r={2,3,13,10};
        int ok=xfer_copy_rect(&xf,&r,1,2,3,1);
        assert(ok==(db==32 || sb==8));
        if(ok)for(int y=0;y<12;y++)for(int x=0;x<16;x++) {
            bool copied=x>=2 && x<13 && y>=3 && y<10;
            unsigned v=(y-1)*16+x+2;
            if(db==8)assert(bmp_row(&b,y)[x]==(copied ? v : 0xcc));
            else assert(((DWORD *)bmp_row(&b,y))[x]==(copied ? v*0x010101 : 0xcccccccc));
        }
        assert(!xfer_copy_rect(&xf,&r,1,2,-15,1));
        xf.dst=&a;assert(!xfer_copy_rect(&xf,&r,1,2,3,1));
    }
    puts("HD GDI bulk copies: conversion, clipping, row orientation and fallback passed");
}
