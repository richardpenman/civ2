/* Exercise the real SDL compositor on a hidden accelerated renderer. */
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "../src/host/host.h"
#include "../src/host/host_gpu.h"
#define main cpu_fixture_main
#include "hd_runtime.c"
#undef main

static void compare(Bitmap *actual,Bitmap *expected,int tolerance)
{
    const uint32_t *pixels=hd_pixels(actual);
    int i,c,n=actual->w*actual->h*hd_density*hd_density;
    for(i=0;i<n;i++)for(c=0;c<24;c+=8) {
        int a=(pixels[i]>>c)&255,b=(expected->hd[i]>>c)&255;
        if(abs(a-b)>tolerance) {
            fprintf(stderr,"GPU difference at pixel %d channel %d: %d vs %d (limit %d)\n",i,c,a,b,tolerance);
            abort();
        }
    }
}
static void check_camera(void)
{
    int d=hd_density,w=64*d,h=48*d,x,y;
    uint32_t *pixels=malloc((size_t)w*h*4),*result=malloc((size_t)w*h*4);
    for(y=0;y<h;y++)for(x=0;x<w;x++)pixels[y*w+x]=0xff000000u|((x/d/8)*29)<<16|((y/d/8)*37)<<8|0x55;
    int src=host_gpu_create(w,h,0,pixels,w*4),dst=host_gpu_create(w,h,0,NULL,0);
    assert(src && dst);
    HostGpuCamera camera={dst,src,0,0,w,h,-7.25f*d,-3.125f*d,w*1.375f,h*1.375f,2*d,3*d,61*d,45*d};
    assert(host_gpu_fill(dst,0,0,w,h,0xffabcdef));
    assert(host_gpu_camera(&camera));
    HostGpuBlit nearest={dst,src,0,0,d,d,0,0,w,h,0,0,d,d,0xffffffffu,0,0};
    assert(host_gpu_blit(&nearest)); /* queued sampler change must not alter the camera */
    assert(host_gpu_read(dst,0,0,w,h,result,w*4));
    int interpolated=0;
    for(y=0;y<h;y++)for(x=0;x<w;x++) {
        if(x<d && y<d)continue;
        if(x<camera.l || x>=camera.r || y<camera.t || y>=camera.b)assert(result[y*w+x]==0xffabcdef);
        else {
            if(((result[y*w+x]>>16)&255)%29)interpolated++;
            double sx=(x+.5-camera.x)/1.375,sy=(y+.5-camera.y)/1.375;
            int ix=(int)sx,iy=(int)sy;
            // Ignore the one-texel interpolation band between ID-colored tiles.
            if(ix>0 && iy>0 && ix<w-1 && iy<h-1 && ix%(8*d)>1 && ix%(8*d)<8*d-2 &&
               iy%(8*d)>1 && iy%(8*d)<8*d-2)assert(result[y*w+x]==pixels[iy*w+ix]);
        }
    }
    assert(interpolated>0);
    host_gpu_destroy(src);host_gpu_destroy(dst);free(pixels);free(result);
}
static int text_channel(int dst,int text,int aa)
{
    static const unsigned char ramp[17]={0,0x4d,0x68,0x7c,0x8c,0x9a,0xa7,0xb2,0xbd,0xc7,0xd0,0xd9,0xe1,0xe9,0xf0,0xf8,0xff};
    int mn,mx;
    if(aa>=16)return text;
    if(aa<=1 || dst==text)return dst;
    mn=ramp[aa]*text/255;mx=ramp[16-aa]+(255-ramp[16-aa])*text/255;
    return dst>text ? text+(dst-text)*(mx-text)/(255-text) : text-(text-dst)*(text-mn)/text;
}
static void check_text(Bitmap *actual,Bitmap *expected)
{
    static const uint32_t colors[]={0,0xffffff,0x33aa77,0xff8000,0x0101fe,0x808080};
    static unsigned char coverage[17];
    int i,v,x,y,c,d=hd_density,stride=actual->w*d,height=actual->h*d;
    for(i=0;i<17;i++)coverage[i]=i;
    for(i=0;i<6;i++)for(v=0;v<256;v+=17) {
        uint32_t bg=0xff000000u|v<<16|(255-v)<<8|v;
        for(x=0;x<stride*height;x++)actual->hd[x]=expected->hd[x]=bg;
        hd_upload_region(actual,0,0,stride,height);
        if(!hd_gpu_text(actual,coverage,17,1,-1,2,17,3,1,2,15,5,colors[i]))continue;
        for(y=2;y<5;y++)for(x=1;x<15;x++) {
            uint32_t p=0xff000000u;
            for(c=0;c<24;c+=8)p|=text_channel((bg>>c)&255,(colors[i]>>c)&255,coverage[x+1])<<c;
            expected->hd[y*stride+x]=p;
        }
        compare(actual,expected,3);
    }
}
static void check_text_batch(Bitmap *actual,Bitmap *reference,Bitmap *expected)
{
    static const unsigned char coverage[]={2,7,11,16};
    int overlap,i,pass,d=hd_density;
    if(!(host_gpu_available()&2)) {
        assert(!hd_gpu_text(actual,coverage,4,1,1,2,4,3,1,2,5,5,0x33aa77));
        return; /* production text_draw supplies the CPU gamma fallback */
    }
    for(overlap=0;overlap<2;overlap++) {
        for(pass=0;pass<2;pass++) {
            Bitmap *b=pass ? actual : reference;
            memset(b->bits,3,192);hd_changed(b->bits,0,0,16,12);
            if(pass)hd_gpu_text_begin();
            for(i=0;i<3;i++) {
                int x=i ? (overlap ? 3 : 8) : 1,y=i==2 ? 6 : 2;
                assert(hd_gpu_text(b,coverage,4,1,x,y,4,3,x,y,x+4,y+3,0x33aa77));
            }
            if(pass)hd_gpu_text_end();
        }
        memcpy(expected->hd,hd_pixels(reference),(size_t)16*12*d*d*4);
        compare(actual,expected,0);
    }
}
static void check_indexed_cache(Bitmap *actual,Bitmap *expected)
{
    int pass,x,y,token=0;
    hd_sprite_bind(&token,"file/test");
    for(pass=0;pass<5;pass++) {
        /* A cached flat background must restore HD artwork painted over it,
           and reflect palette edits and changing cursor/ownership indices. */
        hd_sprite_draw(&token,actual->bits,0,0,16,12,0,0,16,12,-1);
        if(pass==3)actual->colors[2].rgbBlue=expected->colors[2].rgbBlue=127;
        hd_indexed_begin(actual->bits);
        for(y=2;y<9;y++)for(x=1;x<14;x++) {
            bmp_row(actual,y)[x]=bmp_row(expected,y)[x]=(pass==4 ? 3 : 2);
            hd_changed(actual->bits,x,y,1,1);
        }
        hd_indexed_end(actual->bits);
        hd_changed(expected->bits,1,2,13,7);
        { const uint32_t *p=hd_pixels(actual);int d=hd_density;
          for(y=2*d;y<9*d;y++)for(x=d;x<14*d;x++)assert(p[y*16*d+x]==expected->hd[y*16*d+x]); }
    }
    actual->colors[2].rgbBlue=expected->colors[2].rgbBlue=0;
    hd_sprite_forget(&token);
}
static void check_atlas(void)
{
    GpuAtlasStats before,after;
    int images[12],private_images[12],i,j,pass,dst,ref;
    unsigned char rgba[13*7*4];
    uint32_t premul[13*7],actual[32*24],expected[32*24];
    dst=host_gpu_create(32,24,0,NULL,0);ref=host_gpu_create(32,24,0,NULL,0);
    assert(dst && ref);
    for(i=0;i<12;i++) {
        for(j=0;j<13*7;j++) {
            unsigned a=(j%5)*63,r=(j*29+i*21)&255,g=(j*11+i*31)&255,b=(j*37+i*13)&255;
            rgba[j*4]=r;rgba[j*4+1]=g;rgba[j*4+2]=b;rgba[j*4+3]=a;
            premul[j]=a<<24|((r*a+127)/255)<<16|((g*a+127)/255)<<8|(b*a+127)/255;
        }
        images[i]=host_gpu_create(13,7,1,rgba,13*4);
        private_images[i]=host_gpu_create(13,7,0,premul,13*4);
        assert(images[i] && private_images[i]);
    }
    for(pass=0;pass<5;pass++) {
        for(i=0;i<2;i++) {
            int out=i ? dst : ref;
            assert(host_gpu_fill(out,0,0,32,24,0xff225577u));
            for(j=0;j<12;j++) {
                int x=(j*7)%24-3,y=(j*5)%17-2;
                HostGpuBlit draw={out,i ? images[j] : private_images[j],x,y,pass&1 || pass==4 ? 21 : 9,pass&1 || pass==4 ? 11 : 5,
                    pass==3 ? -2 : 0,0,13,7,1,2,31,23,pass==2 ? 0xffa07733u : 0xffffffffu,1,pass==4 ? j&1 : pass&1};
                assert(host_gpu_blit(&draw));
            }
        }
        assert(host_gpu_read(dst,0,0,32,24,actual,32*4));
        assert(host_gpu_read(ref,0,0,32,24,expected,32*4));
        for(i=0;i<32*24;i++)for(j=0;j<32;j+=8)if(abs((int)((actual[i]>>j)&255)-(int)((expected[i]>>j)&255))>3) {
            fprintf(stderr,"Atlas difference: pass %d pixel %d channel %d actual %08x expected %08x\n",pass,i,j,actual[i],expected[i]);abort();
        }
    }
    for(i=0;i<12;i++) { host_gpu_destroy(images[i]);host_gpu_destroy(private_images[i]); }
    gpu_atlas_stats(&before);assert(!before.pages && !before.entries);
    /* Adjacent tinted images share geometry without reordering. Filling the
       batch, deleting a queued source and recycling slots all flush safely. */
    memset(rgba,255,4);images[0]=host_gpu_create(1,1,1,rgba,4);images[1]=host_gpu_create(1,1,1,rgba,4);
    gpu_atlas_stats(&before);
    for(i=0;i<2050;i++) {
        HostGpuBlit draw={dst,images[i&1],0,0,1,1,0,0,1,1,0,0,32,24,(i&1) ? 0xff00ff00u : 0xffff0000u,1,0};
        assert(host_gpu_blit(&draw));
    }
    host_gpu_destroy(images[1]);
    assert(host_gpu_read(dst,0,0,1,1,actual,4));assert(actual[0]==0xff00ff00u);
    gpu_atlas_stats(&after);
    if(before.enabled)assert(after.batches-before.batches==2);
    for(i=0;i<(before.enabled ? 16400 : 64);i++) {
        int image=host_gpu_create(1,1,1,rgba,4);assert(image);host_gpu_destroy(image);
    }
    host_gpu_destroy(images[0]);
    gpu_atlas_stats(&after);assert(!after.entries && !after.pages);
    /* The small-atlas build forces capacity fallback and complete reuse. */
    if(after.enabled && after.page_size<=256) {
        int n=after.page_limit*4+1,w=after.page_size/2-2,handles[32];
        unsigned char *data=malloc((size_t)w*w*4);
        assert(n<=32 && data);memset(data,255,(size_t)w*w*4);
        for(pass=0;pass<3;pass++) {
            for(i=0;i<n;i++) { handles[i]=host_gpu_create(w,w,1,data,w*4);assert(handles[i]); }
            gpu_atlas_stats(&after);assert(after.pages==after.page_limit && after.entries==n-1);
            for(i=n-1;i>=0;i--) {
                HostGpuBlit draw={dst,handles[i],0,0,2,2,0,0,w,w,0,0,32,24,0xffffffffu,1,1};
                assert(host_gpu_blit(&draw));host_gpu_destroy(handles[i]);
            }
            assert(host_gpu_read(dst,0,0,1,1,actual,4));assert(actual[0]==0xffffffffu);
            gpu_atlas_stats(&after);assert(!after.entries && !after.pages);
        }
        free(data);
    }
    host_gpu_destroy(dst);host_gpu_destroy(ref);
}
int main(int argc,char **argv)
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    Bitmap *gpu_bitmap,*cpu_bitmap,*copy;
    int widths[]={1,3,4,5,9,16,64},i,h,tint,grey,x,token=0,sw,sh,d;
    assert(argc==2);
    SDL_SetMainReady();
    assert(!SDL_Init(SDL_INIT_VIDEO));
    SDL_SetHint(SDL_HINT_RENDER_BATCHING,"1");
    window=SDL_CreateWindow("HD GPU test",0,0,64,64,SDL_WINDOW_HIDDEN);
    assert(window);
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
    if(!renderer) { fprintf(stderr,"No accelerated SDL renderer: %s\n",SDL_GetError());return 77; }
    gpu_renderer_init(renderer);
    check_atlas();
    assert(!SDL_RenderSetLogicalSize(renderer,32,32));
    snprintf(plat_data_dir,sizeof plat_data_dir,"%s",argv[1]);
    hd_init(0,0);hd_start_gpu();
    assert(hd_gpu_active);d=hd_density;
    check_camera();
    gpu_bitmap=bitmap(0,0);hd_sync(gpu_bitmap);assert(gpu_bitmap->hd_gpu);
    hd_gpu_active=0;cpu_bitmap=bitmap(1,1);hd_sync(cpu_bitmap);hd_gpu_active=1;
    copy=bitmap(2,1);hd_sync(copy);
    check_text(gpu_bitmap,cpu_bitmap);
    check_text_batch(gpu_bitmap,copy,cpu_bitmap);
    check_indexed_cache(gpu_bitmap,cpu_bitmap);
    hd_sprite_bind(&token,"file/test");
    for(grey=0;grey<2;grey++)for(tint=-1;tint<=3;tint+=4)for(i=0;i<7;i++)for(h=1;h<=9;h+=4)for(x=-3;x<=2;x+=5) {
        memset(gpu_bitmap->bits,3,192);hd_changed(gpu_bitmap->bits,0,0,16,12);
        memset(cpu_bitmap->bits,3,192);hd_changed(cpu_bitmap->bits,0,0,16,12);
        hd_sprite_greyscale(&token,grey);
        hd_sprite_draw(&token,gpu_bitmap->bits,x,-1,widths[i],h,1,0,15,11,tint);
        hd_sprite_draw(&token,cpu_bitmap->bits,x,-1,widths[i],h,1,0,15,11,tint);
        compare(gpu_bitmap,cpu_bitmap,3);
    }
    hd_sprite_forget(&token);
    /* Source/destination clipping and overlap preserve exact opaque pixels. */
    for(i=0;i<16*12;i++)gpu_bitmap->bits[i]=(i&1) ? 2 : 3;
    hd_changed(gpu_bitmap->bits,0,0,16,12);
    hd_copy(copy->bits,gpu_bitmap->bits,0,0,0,0,16,12);
    memcpy(cpu_bitmap->hd,hd_pixels(gpu_bitmap),(size_t)16*12*d*d*4);
    compare(copy,cpu_bitmap,0);
    for(x=-3;x<=18;x+=3) {
        memset(copy->bits,0,192);hd_changed(copy->bits,0,0,16,12);
        memset(cpu_bitmap->bits,0,192);hd_changed(cpu_bitmap->bits,0,0,16,12);
        hd_copy(copy->bits,gpu_bitmap->bits,-2,1,x,-2,16,12);
        hd_copy(cpu_bitmap->bits,gpu_bitmap->bits,-2,1,x,-2,16,12);
        compare(copy,cpu_bitmap,0);
    }
    hd_copy(copy->bits,copy->bits,2,2,0,0,10,8);
    hd_copy(cpu_bitmap->bits,cpu_bitmap->bits,2,2,0,0,10,8);
    compare(copy,cpu_bitmap,0);
    hd_bitmap_invert(copy,3,3,5,4);hd_bitmap_invert(copy,3,3,5,4);
    compare(copy,cpu_bitmap,0);
    /* Capture reads the GPU result; clone/key remapping invalidate textures. */
    hd_sprite_capture(&token,gpu_bitmap->bits,0,0,8,8,-2);
    memset(cpu_bitmap->bits,0,192);
    hd_changed(copy->bits,0,0,16,12);hd_changed(cpu_bitmap->bits,0,0,16,12);
    hd_sprite_draw(&token,copy->bits,0,0,8,8,0,0,16,12,-1);
    hd_sprite_draw(&token,cpu_bitmap->bits,0,0,8,8,0,0,16,12,-1);
    compare(copy,cpu_bitmap,0);
    hd_sprite_tint(&token,2,3);
    hd_sprite_draw(&token,copy->bits,0,0,8,8,0,0,16,12,-1);
    hd_sprite_draw(&token,cpu_bitmap->bits,0,0,8,8,0,0,16,12,-1);
    compare(copy,cpu_bitmap,0);hd_sprite_forget(&token);
    /* The existing alpha-aware minifier remains in the GPU path. */
    memset(gpu_bitmap->bits,0,192);hd_changed(gpu_bitmap->bits,0,0,16,12);
    hd_draw(gpu_bitmap->bits,hd_find("test/pole"),0,0,1,1,0,0,16,12,-1);
    { unsigned sum=0;const uint32_t *p=hd_pixels(gpu_bitmap);
      for(h=0;h<d;h++)for(x=0;x<d;x++)sum+=p[h*16*d+x]&255;
      assert(sum>=3u*d*d && sum<=5u*d*d); }
    /* Rendering to a density-scaled texture must not alter input geometry or
       leave the window viewport active when drawing the next sprite. */
    { SDL_Event event={0};
      event.type=SDL_MOUSEMOTION;event.motion.windowID=SDL_GetWindowID(window);
      event.motion.x=16;event.motion.y=16;
      assert(SDL_PushEvent(&event)==1);
      assert(event.motion.x==8 && event.motion.y==8);
      assert(host_gpu_present(gpu_bitmap->hd_gpu));
      compare(gpu_bitmap,gpu_bitmap,0);
      SDL_SetWindowSize(window,96,96);SDL_PumpEvents();
      gpu_window_target();assert(!SDL_RenderSetLogicalSize(renderer,48,48));
      assert(host_gpu_fill(copy->hd_gpu,0,0,16*d,12*d,0xff123456u));
      for(i=0;i<16*12*d*d;i++)cpu_bitmap->hd[i]=0xff123456u;
      compare(copy,cpu_bitmap,0); }
    gpu_renderer_lost();
    assert(!host_gpu_available());
    assert(!host_gpu_fill(copy->hd_gpu,0,0,1,1,0));
    for(i=0;i<3;i++) { hd_bitmap_destroy(bitmaps[i]);free(bitmaps[i]->bits);free(bitmaps[i]); }
    gpu_renderer_destroy();SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();
    printf("GPU compositor density %d: alpha/tint/grey, text gamma, clipping, copies, inversion, captures, thin details and viewport passed\n",d);
    (void)sw;(void)sh;
    return 0;
}
