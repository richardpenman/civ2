/* SDL render targets for the HD compositor. The platform keeps game geometry
 * and indexed queries; this module owns texture storage, blending and scaling. */
#include "host.h"
#include "host_gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* A guillotine tree gives freed slots back to the page and coalesces empty
 * siblings. Live images never move. Oversize/full-page cases use private
 * textures, so packing cannot evict a source still referenced by a draw. */
#ifndef GPU_ATLAS_SIZE
#define GPU_ATLAS_SIZE 4096
#endif
#ifndef GPU_ATLAS_PAGES
#define GPU_ATLAS_PAGES 1
#endif
typedef struct AtlasNode {
    SDL_Rect rect;
    struct AtlasNode *parent,*child[2];
    int used;
} AtlasNode;
typedef struct AtlasPage AtlasPage;
typedef struct GpuTexture {
    SDL_Texture *texture;
    int w,h,target;
    AtlasPage *page;
    AtlasNode *slot;
    uint32_t *pending;
    struct GpuTexture *next_pending;
} GpuTexture;
struct AtlasPage {
    SDL_Texture *texture;
    AtlasNode *root;
    GpuTexture *pending;
    unsigned refs;
    int linear;
};
static AtlasPage pages[GPU_ATLAS_PAGES];
static int atlas_enabled,atlas_size,rebind_sampler;
static GpuTexture textures[16384];
static SDL_Renderer *gpu;
static SDL_BlendMode premultiplied,maximum;
static SDL_Texture *staging, *snapshot;
static int staging_w, staging_h, snapshot_w, snapshot_h;
static int available, invalid, next_id=1;
static unsigned stat_images,stat_blits,stat_fills,stat_uploads,stat_reads;
static unsigned stat_batches,stat_atlas_uploads;
static unsigned stat_page_breaks,stat_state_breaks,stat_atlas_quads;
#define BATCH_QUADS 2048
static struct {
    SDL_Texture *dst,*image;
    AtlasPage *page;
    SDL_BlendMode blend;
    int linear,count,failed;
    SDL_Vertex vertices[BATCH_QUADS*4];
    int indices[BATCH_QUADS*6];
} batch;
static int batch_flush(void);

static AtlasNode *atlas_node(AtlasNode *parent,SDL_Rect rect)
{
    AtlasNode *p=calloc(1,sizeof *p);
    if(p) { p->parent=parent;p->rect=rect; }
    return p;
}
static void atlas_free_tree(AtlasNode *p)
{
    if(!p)return;
    atlas_free_tree(p->child[0]);atlas_free_tree(p->child[1]);free(p);
}
static AtlasNode *atlas_alloc(AtlasNode *p,int w,int h)
{
    AtlasNode *found;
    SDL_Rect a,b;
    int dw,dh;
    if(!p || p->used || w>p->rect.w || h>p->rect.h)return NULL;
    if(p->child[0]) {
        found=atlas_alloc(p->child[0],w,h);
        return found ? found : atlas_alloc(p->child[1],w,h);
    }
    dw=p->rect.w-w;dh=p->rect.h-h;
    if(!dw && !dh) { p->used=1;return p; }
    a=b=p->rect;
    if(dw>dh) { a.w=w;b.x+=w;b.w-=w; }
    else { a.h=h;b.y+=h;b.h-=h; }
    p->child[0]=atlas_node(p,a);p->child[1]=atlas_node(p,b);
    if(!p->child[0] || !p->child[1]) {
        free(p->child[0]);free(p->child[1]);p->child[0]=p->child[1]=NULL;return NULL;
    }
    return atlas_alloc(p->child[0],w,h);
}
static void atlas_release(AtlasNode *p)
{
    p->used=0;
    while(p->parent) {
        AtlasNode *parent=p->parent,*a=parent->child[0],*b=parent->child[1];
        if(a->used || b->used || a->child[0] || b->child[0])break;
        free(a);free(b);parent->child[0]=parent->child[1]=NULL;p=parent;
    }
}
static AtlasPage *atlas_reserve(int w,int h,AtlasNode **slot)
{
    int i;
    if(!atlas_enabled || w>atlas_size-2 || h>atlas_size-2 ||
       (uint64_t)(w+2)*(h+2)>(uint64_t)atlas_size*atlas_size/4 ||
       (uint64_t)(w+2)*(h+2)>256u*1024)return NULL;
    for(i=0;i<GPU_ATLAS_PAGES;i++)if(pages[i].texture) {
        *slot=atlas_alloc(pages[i].root,w+2,h+2);
        if(*slot) { pages[i].refs++;return pages+i; }
    }
    for(i=0;i<GPU_ATLAS_PAGES;i++)if(!pages[i].texture) {
        AtlasPage *p=pages+i;
        p->root=atlas_node(NULL,(SDL_Rect){0,0,atlas_size,atlas_size});
        if(!p->root)return NULL;
        p->texture=SDL_CreateTexture(gpu,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC,atlas_size,atlas_size);
        if(!p->texture) { free(p->root);p->root=NULL;atlas_enabled=0;return NULL; }
        p->linear=-1;
        *slot=atlas_alloc(p->root,w+2,h+2);
        if(*slot) { p->refs=1;return p; }
        SDL_DestroyTexture(p->texture);atlas_free_tree(p->root);memset(p,0,sizeof *p);return NULL;
    }
    return NULL;
}
static int atlas_upload(AtlasPage *p)
{
    while(p->pending) {
        GpuTexture *t=p->pending;
        /* All new slots are uploaded before their first geometry submission.
           No page-sized CPU shadow or per-zoom enlarged image is needed. */
        if(SDL_UpdateTexture(p->texture,&t->slot->rect,t->pending,(t->w+2)*4))return 0;
        p->pending=t->next_pending;t->next_pending=NULL;
        free(t->pending);t->pending=NULL;stat_atlas_uploads++;
    }
    return 1;
}
static int batch_flush(void)
{
    int n=batch.count;
    if(!n)return !batch.failed;
    batch.count=0;
    stat_atlas_quads+=n;
    /* SDL's D3D backends read scale mode from the texture while executing
       their queue, rather than storing it in each command. Drain old users
       before changing a shared page's sampler. Vertex tint/blend are queued. */
    if(batch.page->linear!=batch.linear) {
        if((batch.page->linear>=0 && SDL_RenderFlush(gpu)) ||
           SDL_SetTextureScaleMode(batch.image,batch.linear ? SDL_ScaleModeLinear : SDL_ScaleModeNearest))batch.failed=1;
        if(rebind_sampler && batch.page->linear>=0) {
            /* SDL2's D3D9 backend refreshes the sampler only when binding a
               different texture. A zero-area untextured primitive invalidates
               that binding without touching any destination pixel. */
            SDL_Vertex empty[3]={0};
            if(SDL_RenderGeometry(gpu,NULL,empty,3,NULL,0))batch.failed=1;
        }
        batch.page->linear=batch.linear;
    }
    if(!atlas_upload(batch.page) ||
       (SDL_GetRenderTarget(gpu)!=batch.dst && SDL_SetRenderTarget(gpu,batch.dst)) ||
       SDL_RenderSetClipRect(gpu,NULL) ||
       SDL_SetTextureBlendMode(batch.image,batch.blend) ||
       SDL_RenderGeometry(gpu,batch.image,batch.vertices,n*4,batch.indices,n*6))batch.failed=1;
    stat_batches++;
    return !batch.failed;
}
static int atlas_draw(SDL_Texture *dst,GpuTexture *src,SDL_Rect sr,SDL_Rect dr,
                      SDL_Rect clip,uint32_t color,SDL_BlendMode blend,int linear)
{
    SDL_Rect visible;
    int n,j,base;
    float u0,v0,u1,v1;
    double sx,sy;
    SDL_Vertex *v;
    SDL_Color c={color>>16,color>>8,color,255};
    if(!SDL_IntersectRect(&dr,&clip,&visible))return 1;
    if(batch.count && (batch.dst!=dst || batch.image!=src->texture || batch.blend!=blend ||
       batch.linear!=linear || batch.count==BATCH_QUADS)) {
        if(batch.image!=src->texture)stat_page_breaks++;
        else stat_state_breaks++;
        if(!batch_flush())return 0;
    }
    batch.dst=dst;batch.image=src->texture;batch.page=src->page;batch.blend=blend;
    batch.linear=linear;
    n=batch.count++;v=batch.vertices+n*4;base=n*4;
    /* Clip geometry, preserving the original sampling origin, instead of
       changing the scissor for each fog edge/glyph. Adjacent quads can then
       share one draw even when their individual clipping rectangles differ. */
    sx=(double)sr.w/dr.w;sy=(double)sr.h/dr.h;
    u0=(float)((src->slot->rect.x+1+sr.x+(visible.x-dr.x)*sx)/atlas_size);
    v0=(float)((src->slot->rect.y+1+sr.y+(visible.y-dr.y)*sy)/atlas_size);
    u1=(float)((src->slot->rect.x+1+sr.x+(visible.x+visible.w-dr.x)*sx)/atlas_size);
    v1=(float)((src->slot->rect.y+1+sr.y+(visible.y+visible.h-dr.y)*sy)/atlas_size);
    v[0]=(SDL_Vertex){{visible.x,visible.y},c,{u0,v0}};
    v[1]=(SDL_Vertex){{visible.x+visible.w,visible.y},c,{u1,v0}};
    v[2]=(SDL_Vertex){{visible.x+visible.w,visible.y+visible.h},c,{u1,v1}};
    v[3]=(SDL_Vertex){{visible.x,visible.y+visible.h},c,{u0,v1}};
    { static const int order[6]={0,1,2,0,2,3};
      for(j=0;j<6;j++)batch.indices[n*6+j]=base+order[j]; }
    return !batch.failed;
}
void gpu_window_target(void)
{
    batch_flush();
    if (gpu && SDL_GetRenderTarget(gpu)) SDL_SetRenderTarget(gpu,NULL);
}
void gpu_renderer_destroy(void)
{
    int i;
    gpu_window_target();
    for (i=1;i<16384;i++) {
        if (textures[i].texture) host_gpu_destroy(i);
    }
    if (staging) SDL_DestroyTexture(staging);
    if (snapshot) SDL_DestroyTexture(snapshot);
    staging=snapshot=NULL;
    staging_w=staging_h=snapshot_w=snapshot_h=0;
    available=invalid=0;gpu=NULL;next_id=1;
    memset(&batch,0,sizeof batch);atlas_enabled=0;
}
void gpu_renderer_lost(void)
{
    /* CPU shadows are deliberately not kept current. Reject further use of
       lost contents instead of silently displaying corrupt/empty surfaces. */
    batch.count=0;invalid=1;available=0;
    fprintf(stderr,"SDL graphics device reset; HD surfaces require a restart\n");
}
void gpu_renderer_init(SDL_Renderer *renderer)
{
    SDL_RendererInfo info;
    SDL_Texture *probe;
    gpu=renderer;available=invalid=0;
    if (!gpu || SDL_GetRendererInfo(gpu,&info)) return;
    fprintf(stderr,"SDL renderer: %s (flags 0x%x)\n",info.name,info.flags);
    rebind_sampler=!strcmp(info.name,"direct3d");
    if (!(info.flags & SDL_RENDERER_ACCELERATED) || !(info.flags & SDL_RENDERER_TARGETTEXTURE)) return;
    premultiplied=SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE,SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        SDL_BLENDOPERATION_ADD,SDL_BLENDFACTOR_ONE,SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,SDL_BLENDOPERATION_ADD);
    probe=SDL_CreateTexture(gpu,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_TARGET,2,2);
    if (!probe) return;
    available=SDL_SetTextureBlendMode(probe,premultiplied)==0 && SDL_SetRenderTarget(gpu,probe)==0;
    maximum=SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE,SDL_BLENDFACTOR_ONE,SDL_BLENDOPERATION_MAXIMUM,
        SDL_BLENDFACTOR_ONE,SDL_BLENDFACTOR_ZERO,SDL_BLENDOPERATION_ADD);
    if(available && !SDL_SetTextureBlendMode(probe,maximum))available|=2;
    atlas_size=GPU_ATLAS_SIZE;
    if(info.max_texture_width && atlas_size>info.max_texture_width)atlas_size=info.max_texture_width;
    if(info.max_texture_height && atlas_size>info.max_texture_height)atlas_size=info.max_texture_height;
    { SDL_Vertex v[3]={{{0,0},{255,255,255,255},{0,0}},{{1,0},{255,255,255,255},{0,0}},{{0,1},{255,255,255,255},{0,0}}};
      atlas_enabled=available && atlas_size>=64 && !getenv("CIV2_GPU_NO_ATLAS") && !SDL_RenderGeometry(gpu,NULL,v,3,NULL,0); }
    SDL_SetRenderTarget(gpu,NULL);
    SDL_DestroyTexture(probe);
    fprintf(stderr,"HD GPU compositor: %s\n",available ? "available" : "unsupported; using CPU");
    fprintf(stderr,"HD sprite atlas: %s (%dx%d, page limit %d)\n",
            atlas_enabled ? "enabled" : "disabled",atlas_size,atlas_size,GPU_ATLAS_PAGES);
}
int host_gpu_available(void) { return available; }
static GpuTexture *get(int id)
{
    return !invalid && id>0 && id<16384 && textures[id].texture ? textures+id : NULL;
}
static int target(GpuTexture *t)
{
    if (!t || !t->target) return 0;
    if (SDL_GetRenderTarget(gpu)!=t->texture && SDL_SetRenderTarget(gpu,t->texture)) return 0;
    return 1;
}
static int temporary(SDL_Texture **tex,int *tw,int *th,int w,int h,int access)
{
    if (*tex && *tw>=w && *th>=h) return 1;
    if (*tex) SDL_DestroyTexture(*tex);
    *tw=w;*th=h;
    *tex=SDL_CreateTexture(gpu,SDL_PIXELFORMAT_ARGB8888,access,w,h);
    if (!*tex) return 0;
    SDL_SetTextureBlendMode(*tex,SDL_BLENDMODE_NONE);
    SDL_SetTextureScaleMode(*tex,SDL_ScaleModeNearest);
    return 1;
}
int host_gpu_create(int w,int h,int kind,const void *pixels,int pitch)
{
    int id,n,x,y,cw,ch,pad;
    SDL_Texture *tex;
    AtlasNode *slot=NULL;
    AtlasPage *page=NULL;
    uint32_t *converted=NULL;
    if (!available || w<=0 || h<=0) return 0;
    for (n=0;n<16383;n++) {
        id=next_id++;if (next_id==16384) next_id=1;
        if (!textures[id].texture) break;
    }
    if (n==16383) return 0;
    if(kind)page=atlas_reserve(w,h,&slot);
    tex=page ? page->texture : SDL_CreateTexture(gpu,SDL_PIXELFORMAT_ARGB8888,
                                               kind ? SDL_TEXTUREACCESS_STATIC : SDL_TEXTUREACCESS_TARGET,w,h);
    if(!tex)return 0;
    textures[id]=(GpuTexture){tex,w,h,!kind,page,slot,NULL,NULL};
    stat_images++;
    if(!page)SDL_SetTextureScaleMode(tex,SDL_ScaleModeNearest);
    if (kind) {
        pad=page ? 1 : 0;cw=w+2*pad;ch=h+2*pad;
        converted=malloc((size_t)cw*ch*4);
        if (!converted) { host_gpu_destroy(id);return 0; }
        for (y=0;y<h;y++) for (x=0;x<w;x++) {
            const unsigned char *p=(const unsigned char *)pixels+y*pitch+x*4;
            unsigned a=p[3],r=p[0],g=p[1],b=p[2];
            if (kind==2) r=g=b=(30*r+59*g+11*b+50)/100;
            if (kind==3) r=g=b=255;
            if(kind!=4) { r=(r*a+127)/255;g=(g*a+127)/255;b=(b*a+127)/255; }
            converted[(y+pad)*cw+x+pad]=a<<24|r<<16|g<<8|b;
        }
        if(page) {
            /* Duplicate the outer texels: bilinear magnification must sample
               this sprite's edge, never a neighbor or uninitialized padding. */
            for(y=1;y<=h;y++) { converted[y*cw]=converted[y*cw+1];converted[y*cw+w+1]=converted[y*cw+w]; }
            memcpy(converted,converted+cw,(size_t)cw*4);
            memcpy(converted+(h+1)*cw,converted+h*cw,(size_t)cw*4);
            textures[id].pending=converted;textures[id].next_pending=page->pending;page->pending=textures+id;
            n=0;
        } else {
            n=SDL_UpdateTexture(tex,NULL,converted,w*4);free(converted);
            if(!n)n=SDL_SetTextureBlendMode(tex,premultiplied);
        }
    } else if (pixels) n=!host_gpu_upload(id,0,0,w,h,pixels,pitch,1);
    else n=!host_gpu_fill(id,0,0,w,h,0xff000000u);
    if (n) { host_gpu_destroy(id);return 0; }
    return id;
}
void host_gpu_destroy(int id)
{
    GpuTexture *t=id>0 && id<16384 && textures[id].texture ? textures+id : NULL;
    if (!t) return;
    batch_flush();
    if(t->page) {
        AtlasPage *p=t->page;
        GpuTexture **link=&p->pending;
        while(*link && *link!=t)link=&(*link)->next_pending;
        if(*link)*link=t->next_pending;
        free(t->pending);
        atlas_release(t->slot);
        if(!--p->refs) {
            SDL_DestroyTexture(p->texture);atlas_free_tree(p->root);memset(p,0,sizeof *p);
        }
        memset(t,0,sizeof *t);return;
    }
    if (SDL_GetRenderTarget(gpu)==t->texture) gpu_window_target();
    SDL_DestroyTexture(t->texture);
    memset(t,0,sizeof *t);
}
int host_gpu_fill(int id,int x,int y,int w,int h,uint32_t color)
{
    stat_fills++;
    SDL_Rect r={x,y,w,h};
    if (!batch_flush() || !target(get(id))) return 0;
    SDL_RenderSetClipRect(gpu,NULL);
    SDL_SetRenderDrawBlendMode(gpu,SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(gpu,color>>16,color>>8,color,255);
    return SDL_RenderFillRect(gpu,&r)==0;
}
int host_gpu_upload(int id,int x,int y,int w,int h,const void *pixels,int pitch,int scale)
{
    stat_uploads++;
    SDL_Rect sr={0,0,w,h},dr={x,y,w*scale,h*scale};
    if (!batch_flush() || !target(get(id)) || !temporary(&staging,&staging_w,&staging_h,w,h,SDL_TEXTUREACCESS_STREAMING)) return 0;
    if (SDL_UpdateTexture(staging,&sr,pixels,pitch)) return 0;
    SDL_RenderSetClipRect(gpu,NULL);
    return SDL_RenderCopy(gpu,staging,&sr,&dr)==0;
}
static int private_sampling(SDL_Texture *texture,SDL_ScaleMode mode)
{
    SDL_ScaleMode previous;
    if(SDL_GetTextureScaleMode(texture,&previous))return 0;
    if(previous==mode)return 1;
    if(SDL_RenderFlush(gpu) || SDL_SetTextureScaleMode(texture,mode))return 0;
    if(rebind_sampler) {
        SDL_Vertex v[3]={0};
        if(SDL_RenderGeometry(gpu,NULL,v,3,NULL,0))return 0;
    }
    return 1;
}
int host_gpu_blit(const HostGpuBlit *d)
{
    stat_blits++;
    GpuTexture *dst=get(d->dst),*src=get(d->src);
    SDL_Rect sr={d->sx,d->sy,d->sw,d->sh},dr={d->x,d->y,d->w,d->h};
    SDL_Rect clip={d->l,d->t,d->r-d->l,d->b-d->t};
    SDL_Texture *image;
    if (!src || !dst || dr.w<=0 || dr.h<=0 || sr.w<=0 || sr.h<=0) return 0;
    { SDL_Rect all={0,0,src->w,src->h};if(!SDL_IntersectRect(&sr,&all,&sr))return 1; }
    if(src->page) {
        if(!dst->target)return 0;
        return atlas_draw(dst->texture,src,sr,dr,clip,d->color,d->blend ? premultiplied : SDL_BLENDMODE_NONE,d->linear);
    }
    if(!batch_flush())return 0;
    image=src->texture;
    if (dst==src) {
        /* GPU-only snapshot; no per-scroll readback to system memory. */
        if (!temporary(&snapshot,&snapshot_w,&snapshot_h,src->w,src->h,SDL_TEXTUREACCESS_TARGET)) return 0;
        if (SDL_SetRenderTarget(gpu,snapshot)) return 0;
        SDL_RenderSetClipRect(gpu,NULL);
        SDL_SetTextureBlendMode(image,SDL_BLENDMODE_NONE);
        SDL_SetTextureColorMod(image,255,255,255);
        { SDL_Rect all={0,0,src->w,src->h};if (SDL_RenderCopy(gpu,image,&all,&all)) return 0; }
        image=snapshot;
    }
    if (!target(dst)) return 0;
    /* Most sprites are fully inside the clip. A different scissor for every
       sprite breaks SDL's command batches even when it clips no pixels. */
    SDL_RenderSetClipRect(gpu,clip.x<=dr.x && clip.y<=dr.y &&
        clip.x+clip.w>=dr.x+dr.w && clip.y+clip.h>=dr.y+dr.h ? NULL : &clip);
    if (SDL_SetTextureBlendMode(image,d->blend ? premultiplied : SDL_BLENDMODE_NONE)) return 0;
    SDL_SetTextureColorMod(image,d->color>>16,d->color>>8,d->color);
    /* At 1:1, integer copies sample texel centers in either mode. Keep the
     * sampler stable when copying chrome from the camera's backing surface. */
    if((sr.w!=dr.w || sr.h!=dr.h) &&
       !private_sampling(image,d->linear ? SDL_ScaleModeLinear : SDL_ScaleModeNearest))return 0;
    return SDL_RenderCopy(gpu,image,&sr,&dr)==0;
}
int host_gpu_read(int id,int x,int y,int w,int h,void *pixels,int pitch)
{
    stat_reads++;
    SDL_Rect r={x,y,w,h};
    if (!batch_flush() || !target(get(id))) return 0;
    return SDL_RenderReadPixels(gpu,&r,SDL_PIXELFORMAT_ARGB8888,pixels,pitch)==0;
}
int host_gpu_text(const HostGpuText *text)
{
    return host_gpu_text_batch(text,1);
}
int host_gpu_text_batch(const HostGpuText *texts,int count)
{
    GpuTexture *dst;
    SDL_Rect bounds,zero;
    int n,i,branch;
    if(!(available&2) || count<=0 || count>128 || !(dst=get(texts[0].draw.dst)))return 0;
    bounds=(SDL_Rect){texts[0].draw.l,texts[0].draw.t,texts[0].draw.r-texts[0].draw.l,texts[0].draw.b-texts[0].draw.t};
    for(n=0;n<count;n++) {
        const HostGpuBlit *d=&texts[n].draw;
        SDL_Rect clip={d->l,d->t,d->r-d->l,d->b-d->t};
        SDL_Rect united;
        if(get(d->dst)!=dst || clip.w<=0 || clip.h<=0)return 0;
        for(i=0;i<2;i++)if(!get(texts[n].layers[i]))return 0;
        SDL_UnionRect(&bounds,&clip,&united);bounds=united;
    }
    zero=(SDL_Rect){0,0,bounds.w,bounds.h};
    if(!batch_flush())return 0;
    if(!temporary(&snapshot,&snapshot_w,&snapshot_h,bounds.w,bounds.h,SDL_TEXTUREACCESS_TARGET))return 0;
    if(SDL_SetRenderTarget(gpu,snapshot))return 0;
    SDL_RenderSetClipRect(gpu,NULL);
    SDL_SetTextureBlendMode(dst->texture,SDL_BLENDMODE_NONE);
    SDL_SetTextureColorMod(dst->texture,255,255,255);
    if(SDL_RenderCopy(gpu,dst->texture,&bounds,&zero))return 0;
    /* Wine's colored gamma ramp is max(mn+b*dst, intercept+a*dst).
       Evaluate each affine branch with one premultiplied blend, then MAX.
       Everything stays in GPU textures; no destination readback is needed. */
    for(branch=0;branch<2;branch++) {
        for(n=0;n<count;n++) {
            const HostGpuBlit *d=&texts[n].draw;
            SDL_Rect draw={d->x,d->y,d->w,d->h},clip={d->l,d->t,d->r-d->l,d->b-d->t};
            GpuTexture *layer=get(texts[n].layers[branch]);
            SDL_Texture *out=branch ? snapshot : dst->texture;
            if(branch) { draw.x-=bounds.x;draw.y-=bounds.y;clip.x-=bounds.x;clip.y-=bounds.y; }
            if(layer->page) {
                SDL_Rect sr={0,0,layer->w,layer->h};
                if(!atlas_draw(out,layer,sr,draw,clip,0xffffffffu,premultiplied,0))return 0;
            } else {
                if(!batch_flush() || SDL_SetRenderTarget(gpu,out))return 0;
                SDL_RenderSetClipRect(gpu,&clip);
                if(SDL_SetTextureBlendMode(layer->texture,premultiplied))return 0;
                SDL_SetTextureColorMod(layer->texture,255,255,255);
                if(SDL_RenderCopy(gpu,layer->texture,NULL,&draw))return 0;
            }
        }
    }
    if(!batch_flush() || !target(dst))return 0;
    SDL_RenderSetClipRect(gpu,&bounds);
    if(SDL_SetTextureBlendMode(snapshot,maximum))return 0;
    SDL_SetTextureColorMod(snapshot,255,255,255);
    stat_blits+=2+2*count;
    return SDL_RenderCopy(gpu,snapshot,&zero,&bounds)==0;
}
int host_gpu_camera(const HostGpuCamera *d)
{
    GpuTexture *dst=get(d->dst),*src=get(d->src);
    SDL_Rect sr={d->sx,d->sy,d->sw,d->sh},clip={d->l,d->t,d->r-d->l,d->b-d->t};
    SDL_FRect dr={d->x,d->y,d->w,d->h};
    if(!src || !dst || src==dst || src->page || d->sw<=0 || d->sh<=0 ||
       !(d->w>0) || !(d->h>0) || !isfinite(d->x) || !isfinite(d->y) ||
       !isfinite(d->w) || !isfinite(d->h))return 0;
    if(clip.w<=0 || clip.h<=0)return 1;
    if(d->sx<0 || d->sy<0 || d->sx>src->w-d->sw || d->sy>src->h-d->sh)return 0;
    if(!batch_flush() || !target(dst))return 0;
    SDL_RenderSetClipRect(gpu,&clip);
    SDL_SetTextureBlendMode(src->texture,SDL_BLENDMODE_NONE);
    SDL_SetTextureColorMod(src->texture,255,255,255);
    /* Private render targets can also have queued integer copies. Changing
     * their sampler requires the same ordering/rebind rules as an atlas. */
    if(!private_sampling(src->texture,SDL_ScaleModeLinear))return 0;
    stat_blits++;
    return SDL_RenderCopyF(gpu,src->texture,&sr,&dr)==0;
}
int host_gpu_present(int id)
{
    GpuTexture *t=get(id);
    if (!t || !batch_flush()) return 0;
    gpu_window_target();
    SDL_RenderSetClipRect(gpu,NULL);
    SDL_SetTextureBlendMode(t->texture,SDL_BLENDMODE_NONE);
    SDL_SetTextureColorMod(t->texture,255,255,255);
    SDL_SetTextureScaleMode(t->texture,SDL_ScaleModeLinear);
    if (SDL_RenderClear(gpu) || SDL_RenderCopy(gpu,t->texture,NULL,NULL)) return 0;
    SDL_RenderPresent(gpu);
    return 1;
}
void host_gpu_fence(void)
{
    SDL_Rect r={0,0,1,1};
    uint32_t pixel;
    if (available && (!batch_flush() || SDL_RenderReadPixels(gpu,&r,SDL_PIXELFORMAT_ARGB8888,&pixel,4))) {
        fprintf(stderr,"GPU benchmark fence failed: %s\n",SDL_GetError());
        abort();
    }
}
void host_gpu_finish(void)
{
    host_gpu_fence();
    if(getenv("CIV2_GPU_STATS"))fprintf(stderr,"GPU: images=%u blits=%u fills=%u uploads=%u readbacks=%u\n",
        stat_images,stat_blits,stat_fills,stat_uploads,stat_reads);
    if(getenv("CIV2_GPU_STATS")) {
        unsigned i,entries=0,active=0;
        for(i=0;i<GPU_ATLAS_PAGES;i++)if(pages[i].texture) { active++;entries+=pages[i].refs; }
        fprintf(stderr,"GPU atlas: pages=%u entries=%u geometry_batches=%u quads=%u uploads=%u breaks=%u/%u\n",
            active,entries,stat_batches,stat_atlas_quads,stat_atlas_uploads,stat_page_breaks,stat_state_breaks);
    }
    stat_images=stat_blits=stat_fills=stat_uploads=stat_reads=0;
    stat_batches=stat_atlas_uploads=0;
    stat_atlas_quads=stat_page_breaks=stat_state_breaks=0;
}
void gpu_atlas_stats(GpuAtlasStats *out)
{
    int i;
    *out=(GpuAtlasStats){atlas_enabled,atlas_size,GPU_ATLAS_PAGES,0,0,stat_batches,stat_atlas_uploads};
    for(i=0;i<GPU_ATLAS_PAGES;i++)if(pages[i].texture) { out->pages++;out->entries+=pages[i].refs; }
}
