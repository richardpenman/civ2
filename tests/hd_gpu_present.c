/* Check the physical window border, which is outside game screenshots. */
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void check_present(SDL_Renderer *renderer);
/* Read before swapping: SDL leaves the backbuffer undefined after present. */
#define SDL_RenderPresent check_present
#include "../src/host/host_gpu.c"
#undef SDL_RenderPresent

static void check_present(SDL_Renderer *renderer)
{
    int w,h,lw,lh;
    assert(!SDL_GetRendererOutputSize(renderer,&w,&h));
    SDL_RenderGetLogicalSize(renderer,&lw,&lh);
    uint32_t *pixels=malloc((size_t)w*h*4);
    assert(pixels);
    /* Read the entire physical window, including the integer-scale border. */
    assert(!SDL_RenderSetViewport(renderer,NULL));
    assert(!SDL_RenderReadPixels(renderer,NULL,SDL_PIXELFORMAT_ARGB8888,pixels,w*4));
    assert(pixels[(h/2)*w+w/2]==0xffcccccc);
    uint32_t expected=h%lh ? 0xff000000 : 0xffcccccc;
    for(int x=0;x<w;x++) {
        if(pixels[(h-1)*w+x]!=expected) {
            fprintf(stderr,"Window %dx%d bottom pixel %d: %08x, expected %08x\n",
                    w,h,x,pixels[(h-1)*w+x],expected);
            abort();
        }
    }
    free(pixels);
    /* Recompute the viewport; logical coordinates round odd physical offsets. */
    assert(!SDL_RenderSetLogicalSize(renderer,0,0));
    assert(!SDL_RenderSetLogicalSize(renderer,lw,lh));
    SDL_RenderPresent(renderer);
}

int main(void)
{
    const int sizes[][2]={{64,64},{65,67},{98,100}};
    const uint32_t colors[]={0xffff0000,0xff00ff00,0xff0000ff};
    SDL_SetMainReady();assert(!SDL_Init(SDL_INIT_VIDEO));
    for(int i=0;i<3;i++) {
        SDL_Window *window=SDL_CreateWindow("HD border test",0,0,sizes[i][0],sizes[i][1],SDL_WINDOW_HIDDEN);
        assert(window);
        SDL_Renderer *renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
        assert(renderer);gpu_renderer_init(renderer);assert(host_gpu_available());
        assert(!SDL_RenderSetLogicalSize(renderer,32,32));
        assert(!SDL_RenderSetIntegerScale(renderer,SDL_TRUE));
        int screen=host_gpu_create(32,32,0,NULL,0),scratch=host_gpu_create(1,1,0,NULL,0);
        assert(screen && scratch);
        assert(host_gpu_fill(screen,0,0,32,32,0xffcccccc));
        for(int j=0;j<3;j++) {
            assert(host_gpu_fill(scratch,0,0,1,1,colors[j]));
            assert(host_gpu_present(screen));
        }
        gpu_renderer_destroy();SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);
    }
    SDL_Quit();puts("HD window border: passed");
    return 0;
}
