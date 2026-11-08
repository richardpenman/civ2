/* True-color backing surfaces for the indexed game. The indexed planes remain
 * available for legacy color queries; every transfer preserves the HD plane.
 * Only the presentation density changes, never game geometry or input. */
#include "hd.h"
#include "plat.h"
#include "../host/host.h"
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_MAX_DIMENSIONS 16384
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image/stb_image.h"

int hd_enabled, hd_density = 1;
int hd_fade_grey;
int hd_gpu_active;
void hd_start_gpu(void)
{
    hd_gpu_active=hd_enabled && GetPrivateProfileIntA("OpenCiv2","HardwareGraphics",1,"CIV.INI") &&
                  host_gpu_available();
    if (hd_enabled) plat_log("HD compositor: %s\n",hd_gpu_active ? "SDL GPU" : "CPU");
}
static void gpu_check(int result)
{
    if (!result) plat_fatal("SDL HD rendering failed. Restart with [OpenCiv2] HardwareGraphics=0 in CIV.INI.");
}
static HostGpuText text_batch[128];
static int text_count,text_batching;
static void text_flush(void)
{
    if(text_count)gpu_check(host_gpu_text_batch(text_batch,text_count));
    text_count=0;
}
void hd_gpu_text_begin(void) { text_flush();text_batching=1; }
void hd_gpu_text_end(void) { text_flush();text_batching=0; }
void hd_read_region(Bitmap *b,int x,int y,int w,int h)
{
    text_flush();
    int r=x+w,bot=y+h,pitch=b->w*hd_density;
    if (!b->hd_gpu) return;
    if (x<0) x=0;if(y<0)y=0;
    if (r>pitch)r=pitch;if(bot>b->h*hd_density)bot=b->h*hd_density;
    if (x<r && y<bot) gpu_check(host_gpu_read(b->hd_gpu,x,y,r-x,bot-y,b->hd+y*pitch+x,pitch*4));
}
void hd_upload_region(Bitmap *b,int x,int y,int w,int h)
{
    text_flush();
    int r=x+w,bot=y+h,pitch=b->w*hd_density;
    if (!b->hd_gpu) return;
    if (x<0)x=0;if(y<0)y=0;
    if (r>pitch)r=pitch;if(bot>b->h*hd_density)bot=b->h*hd_density;
    if (x<r && y<bot) gpu_check(host_gpu_upload(b->hd_gpu,x,y,r-x,bot-y,b->hd+y*pitch+x,pitch*4,1));
}
typedef struct {
    char key[160];
    uint32_t w, h, offset, length, indices;
    unsigned char *indexed, *rgba;
    int pw, ph;
    unsigned char *small;
    int small_w, small_h; /* one cached minification size; zoom/density may change */
    int cx, cy, cw, ch; /* dynamic crop, in source logical coordinates */
    uint64_t used;
} Image;
static Image *images;
static unsigned count;
static unsigned catalog_count;
static FILE *pack;
static uint64_t tick, cache_bytes;
/* Enlargements cache the rounded premultiplied samples, not straight RGBA:
   this preserves the existing filter/blend rounding and transparent edges. */
typedef struct { uint16_t r, g, b, a; } Sample;
typedef struct Filtered {
    Image *image;
    int w, h, tint, grey;
    Sample *pixels;
    struct Filtered *next;
} Filtered;
static Filtered *filtered;
static uint64_t filtered_bytes;
typedef struct GpuImage {
    const void *key;
    int w,h,kind,color_key,texture;
    struct GpuImage *next;
} GpuImage;
static GpuImage *gpu_images;
static uint64_t gpu_image_bytes;
#define GPU_IMAGE_BUDGET (128u*1024*1024)
static void gpu_image_remove(GpuImage **link)
{
    text_flush(); /* queued glyphs may reference a texture being evicted */
    GpuImage *p=*link;
    *link=p->next;
    gpu_image_bytes-=(uint64_t)p->w*p->h*4;
    host_gpu_destroy(p->texture);
    free(p);
}
static void gpu_image_forget(const void *key)
{
    GpuImage **link=&gpu_images;
    while(*link) {
        if((*link)->key==key)gpu_image_remove(link);
        else link=&(*link)->next;
    }
}
static int gpu_image_find(const void *key,int w,int h,int kind,int color_key)
{
    GpuImage **link=&gpu_images,*p;
    while((p=*link)) {
        if(p->key==key && p->w==w && p->h==h && p->kind==kind && p->color_key==color_key) {
            *link=p->next;p->next=gpu_images;gpu_images=p;
            return p->texture;
        }
        link=&p->next;
    }
    return 0;
}
static int gpu_image_add(const void *key,int w,int h,int kind,int color_key,const void *rgba,int upload_kind)
{
    GpuImage *p,**link;
    uint64_t bytes=(uint64_t)w*h*4;
    int texture;
    if(bytes>GPU_IMAGE_BUDGET)return 0;
    while(gpu_image_bytes+bytes>GPU_IMAGE_BUDGET) {
        link=&gpu_images;while((*link)->next)link=&(*link)->next;
        gpu_image_remove(link);
    }
    texture=host_gpu_create(w,h,upload_kind,rgba,w*4);
    if(!texture)return 0;
    p=malloc(sizeof *p);
    if(!p) { host_gpu_destroy(texture);return 0; }
    *p=(GpuImage){key,w,h,kind,color_key,texture,gpu_images};
    gpu_images=p;gpu_image_bytes+=bytes;
    return texture;
}
#ifndef HD_FILTERED_BUDGET
#define HD_FILTERED_BUDGET (64u * 1024 * 1024)
#endif
static void filtered_remove(Filtered **link)
{
    Filtered *p = *link;
    *link = p->next;
    filtered_bytes -= (uint64_t)p->w*p->h*sizeof(Sample);
    free(p->pixels);
    free(p);
}
static void filtered_forget(Image *image)
{
    Filtered **link = &filtered;
    while (*link) {
        if ((*link)->image == image) filtered_remove(link);
        else link = &(*link)->next;
    }
}
static unsigned char palette[768];
static uint32_t palette32[256];
static uint32_t read32(FILE *f)
{
    unsigned char b[4];
    if (fread(b, 1, 4, f) != 4)
        plat_fatal("Truncated Atlas HD pack");
    return b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
}

void hd_init(int classic, int test)
{
    char path[1200], magic[8];
    long size;
    unsigned i;
    hd_enabled = !classic && !test && !GetPrivateProfileIntA("OpenCiv2", "OriginalGraphics", 0, "CIV.INI");
    /* Reference replays remain original by default. Explicit HD replays exercise
       the same executable and platform layer as interactive play. */
    if (getenv("CIV2_TEST_HD"))
        hd_enabled = !classic && atoi(getenv("CIV2_TEST_HD")) != 0 &&
                     !GetPrivateProfileIntA("OpenCiv2", "OriginalGraphics", 0, "CIV.INI");
    if (!hd_enabled)
        return;
    snprintf(path, sizeof path, "%s/atlas-hd.pack", plat_data_dir);
    pack = fopen(path, "rb");
    if (!pack)
        plat_fatal("Atlas HD graphics missing: %s. Rebuild/package the graphics, or set [OpenCiv2] "
                   "OriginalGraphics=1 in CIV.INI.",
                   path);
    fseek(pack, 0, SEEK_END);
    size = ftell(pack);
    rewind(pack);
    if (fread(magic, 1, 8, pack) != 8 || memcmp(magic, "C2ATLAS1", 8))
        plat_fatal("Invalid Atlas HD pack");
    count = read32(pack);
    if (count > 8192 || size < 780 + (long)count * 180)
        plat_fatal("Invalid Atlas HD index");
    if (fread(palette, 1, 768, pack) != 768)
        plat_fatal("Invalid Atlas HD palette");
    for (i = 0; i < 256; i++)
        palette32[i] = 0xff000000u | palette[i * 3] << 16 | palette[i * 3 + 1] << 8 | palette[i * 3 + 2];
    catalog_count = count;
    images = calloc(8192, sizeof *images);
    if (!images)
        plat_fatal("Out of memory loading Atlas HD index");
    for (i = 0; i < count; i++) {
        Image *a = images + i;
        if (fread(a->key, 1, 160, pack) != 160 || !memchr(a->key, 0, 160))
            plat_fatal("Invalid Atlas HD key");
        a->w = read32(pack);
        a->h = read32(pack);
        a->offset = read32(pack);
        a->length = read32(pack);
        a->indices = read32(pack);
        if ((i && strcmp(images[i - 1].key, a->key) >= 0) || !a->w || !a->h || a->w > 4096 || a->h > 4096 ||
            a->offset > (uint32_t)size || a->length > (uint32_t)size - a->offset ||
            a->indices > (uint32_t)size || (a->indices && a->w * a->h > (uint32_t)size - a->indices))
            plat_fatal("Invalid Atlas HD entry: %s", a->key);
    }
    hd_density = GetPrivateProfileIntA("OpenCiv2", "GraphicsDensity", 2, "CIV.INI");
    if (hd_density < 1)
        hd_density = 1;
    if (hd_density > 4)
        hd_density = 4;
    plat_log("Atlas HD: %u bindings, density %d; original image loading disabled\n", count, hd_density);
}

int hd_find(const char *key)
{
    unsigned lo = 0, hi = catalog_count;
    while (lo < hi) {
        unsigned mid = lo + (hi - lo) / 2;
        int c = strcmp(key, images[mid].key);
        if (!c)
            return mid + 1;
        if (c < 0)
            hi = mid;
        else
            lo = mid + 1;
    }
    return 0;
}
void hd_size(int id, int *w, int *h)
{
    int valid = id > 0 && id <= (int)count;
    *w = valid ? images[id - 1].w : 0;
    *h = valid ? images[id - 1].h : 0;
}
int hd_variant(int id, int color)
{
    char key[220];
    if (id <= 0 || id > (int)count)
        return 0;
    snprintf(key, sizeof key, "tint/%s/%d", images[id - 1].key, color & 255);
    return hd_find(key);
}
const unsigned char *hd_palette(void) { return palette; }
const unsigned char *hd_indices(int id)
{
    Image *a;
    if (!id || id > (int)count)
        return NULL;
    a = images + id - 1;
    if (!a->indices)
        return NULL;
    if (!a->indexed) {
        a->indexed = malloc(a->w * a->h);
        if (!a->indexed)
            plat_fatal("Out of memory loading Atlas HD sheet");
        fseek(pack, a->indices, SEEK_SET);
        if (fread(a->indexed, 1, a->w * a->h, pack) != a->w * a->h)
            plat_fatal("Truncated Atlas HD sheet");
    }
    return a->indexed;
}
static Image *decode(int id)
{
    Image *a;
    unsigned i;
    unsigned char *png;
    int channels;
    if (!id || id > (int)count)
        return NULL;
    a = images + id - 1;
    a->used = ++tick;
    if (a->rgba)
        return a;
    /* Bound the decoded-image cache; sprite handles refer to entries, not
       borrowed decoder storage, so eviction cannot invalidate live sprites. */
    while (cache_bytes > 96u * 1024 * 1024) {
        Image *old = NULL;
        for (i = 0; i < count; i++)
            if (images[i].rgba && (!old || images[i].used < old->used))
                old = images + i;
        if (!old)
            break;
        cache_bytes -= (uint64_t)old->pw * old->ph * 4;
        cache_bytes -= (uint64_t)old->small_w * old->small_h * 4;
        filtered_forget(old);
        free(old->rgba);
        free(old->small);
        old->rgba = NULL;
        old->small = NULL;
        old->small_w = old->small_h = 0;
    }
    png = malloc(a->length);
    if (!png)
        plat_fatal("Out of memory decoding Atlas HD image");
    fseek(pack, a->offset, SEEK_SET);
    if (fread(png, 1, a->length, pack) != a->length)
        plat_fatal("Truncated Atlas HD image");
    a->rgba = stbi_load_from_memory(png, a->length, &a->pw, &a->ph, &channels, 4);
    free(png);
    if (!a->rgba)
        plat_fatal("Invalid Atlas HD PNG: %s", a->key);
    if (a->cw) {
        int x = a->cx * a->pw / a->w, y = a->cy * a->ph / a->h;
        int w = a->cw * a->pw / a->w, h = a->ch * a->ph / a->h, j;
        unsigned char *cut;
        if (x < 0 || y < 0 || w < 1 || h < 1 || x + w > a->pw || y + h > a->ph)
            plat_fatal("Atlas crop outside image: %s", a->key);
        cut = malloc((size_t)w * h * 4);
        if (!cut)
            plat_fatal("Out of memory cropping Atlas image");
        for (j = 0; j < h; j++)
            memcpy(cut + j * w * 4, a->rgba + ((y + j) * a->pw + x) * 4, w * 4);
        free(a->rgba);
        a->rgba = cut;
        a->pw = w;
        a->ph = h;
    }
    cache_bytes += (uint64_t)a->pw * a->ph * 4;
    return a;
}
static Image *draw_image(int id)
{
    if(!id || id>(int)count)return NULL;
    /* GPU textures outlive eviction of decoded CPU pixels. A cache hit must
       not decode/upload the image again just to obtain its dimensions. */
    return hd_gpu_active ? images+id-1 : decode(id);
}
int hd_file(const char *name)
{
    char key[180], base[128];
    const char *p = name, *s;
    size_t i;
    for (s = name; *s; s++)
        if (*s == '/' || *s == '\\')
            p = s + 1;
    snprintf(base, sizeof base, "%s", p);
    for (i = 0; base[i]; i++) {
        base[i] = tolower((unsigned char)base[i]);
        if (base[i] == '.') {
            base[i] = 0;
            break;
        }
    }
    snprintf(key, sizeof key, "file/%s", base);
    return hd_find(key);
}
int hd_virtual_file(const char *name)
{
    const char *p = name, *s;
    static const char *const modules[] = {"tiles.dll", "intro.dll", "cv.dll",      "pv.dll",
                                          "mk.dll",    "ss.dll",    "civ2art.dll", "wonder.dll"};
    unsigned i;
    if (!hd_enabled)
        return 0;
    for (s = name; *s; s++)
        if (*s == '/' || *s == '\\')
            p = s + 1;
    for (i = 0; i < sizeof modules / sizeof modules[0]; i++)
        if (!strcasecmp(p, modules[i]))
            return 1;
    return hd_file(p) != 0;
}
const unsigned char *hd_rgba(int id, int *w, int *h)
{
    Image *a = decode(id);
    *w = a ? a->pw : 0;
    *h = a ? a->ph : 0;
    return a ? a->rgba : NULL;
}
/* Resource ids are disambiguated by the current resource-library stack by
   hd_resource(), implemented beside the game's resource loader. */
int hd_crop(int id, int x, int y, int w, int h)
{
    char key[220];
    unsigned i;
    int found;
    if (!id)
        return 0;
    snprintf(key, sizeof key, "crop/%s/%d,%d,%d,%d", images[id - 1].key, x, y, w, h);
    found = hd_find(key);
    if (found)
        return found;
    for (i = catalog_count; i < count; i++)
        if (!strcmp(images[i].key, key))
            return i + 1;
    if (count >= 8192 || strlen(key) >= 160)
        plat_fatal("Too many Atlas crops");
    images[count] = images[id - 1];
    snprintf(images[count].key, 160, "%s", key);
    images[count].rgba = NULL;
    images[count].small = NULL;
    images[count].small_w = images[count].small_h = 0;
    images[count].indexed = NULL;
    images[count].cx = x;
    images[count].cy = y;
    images[count].cw = w;
    images[count].ch = h;
    plat_log("Atlas crop: %s\n", key);
    return ++count;
}

static uint32_t legacy(Bitmap *b, int x, int y)
{
    COLORREF c = surf_get(b, x, y);
    return 0xff000000u | GetRValue(c) << 16 | GetGValue(c) << 8 | GetBValue(c);
}
static uint32_t raw_pixel(Bitmap *b, int x, int y)
{
    const unsigned char *p = bmp_row(b, y);
    if (b->bpp == 8)
        return p[x];
    if (b->bpp == 32)
        return ((const uint32_t *)p)[x];
    return legacy(b, x, y);
}
static void remember_pixel(Bitmap *b,int x,int y)
{
    if(b->bpp==8)((unsigned char *)b->hd_before)[y*b->w+x]=bmp_row(b,y)[x];
    else ((uint32_t *)b->hd_before)[y*b->w+x]=raw_pixel(b,x,y);
}
typedef struct HdIndexedCache {
    int depth,l,t,r,bot,w,h,texture;
    unsigned char *indices;
    RGBQUAD colors[256];
} HdIndexedCache;
static int indexed_note(Bitmap *b,int x,int y,int w,int h)
{
    HdIndexedCache *c=b->hd_indexed_cache;
    int r=x+w,bot=y+h;
    if(!c || !c->depth)return 0;
    if(x<0)x=0;if(y<0)y=0;if(r>b->w)r=b->w;if(bot>b->h)bot=b->h;
    if(x<r && y<bot) {
        if(x<c->l)c->l=x;if(y<c->t)c->t=y;if(r>c->r)c->r=r;if(bot>c->bot)c->bot=bot;
    }
    return 1;
}
static void block(Bitmap *b, int x, int y, uint32_t c)
{
    int i, j, d = hd_density, pitch = b->w * d;
    if (b->hd_gpu) {
        text_flush();
        gpu_check(host_gpu_fill(b->hd_gpu,x*d,y*d,d,d,c));
        return;
    }
    for (j = 0; j < d; j++)
        for (i = 0; i < d; i++)
            b->hd[(y * d + j) * pitch + x * d + i] = c;
}
/* Raw DIB writes outside this rectangle must remain observable. In particular,
   committing a small blit must not acknowledge an unrelated pending write. */
static void sync_rect(Bitmap *b, int l, int t, int r, int bot, int commit)
{
    int x, y;
    if (l < 0) l = 0;
    if (t < 0) t = 0;
    if (r > b->w) r = b->w;
    if (bot > b->h) bot = b->h;
    if (l >= r || t >= bot) return;
    for (y = t; y < bot; y++) {
        const unsigned char *row = bmp_row(b, y);
        uint32_t *before;
        if(b->bpp==8) {
            unsigned char *indices=(unsigned char *)b->hd_before+y*b->w;
            if(!commit && !host_memory_equal(indices+l,row+l,(uint32_t)(r-l))) {
                for(x=l;x<r;x++)if(indices[x]!=row[x]) {
                    b->hd_image=0;block(b,x,y,legacy(b,x,y));
                }
            }
            memcpy(indices+l,row+l,(size_t)(r-l));
            continue;
        }
        before=(uint32_t *)b->hd_before+y*b->w;
        if (b->bpp == 32) {
            if (commit) {
                memcpy(before+l,row+4*l,(size_t)(r-l)*4);
                continue;
            }
            if (host_memory_equal(before+l,row+4*l,(uint32_t)(r-l)*4)) continue;
        }
        for (x = l; x < r; x++) {
            uint32_t v = b->bpp == 8 ? row[x] : b->bpp == 32 ? ((const uint32_t *)row)[x] : raw_pixel(b,x,y);
            if (!commit && v != before[x]) {
                b->hd_image = 0;
                block(b,x,y,legacy(b,x,y));
            }
            before[x] = v;
        }
    }
}
void hd_sync_rect(Bitmap *b, int x, int y, int w, int h)
{
    int i, j;
    if (!hd_enabled || !b)
        return;
    if (!b->hd) {
        b->hd = malloc((size_t)b->w * b->h * hd_density * hd_density * 4);
        b->hd_before = malloc((size_t)b->w * b->h * (b->bpp==8 ? 1 : 4));
        if (!b->hd || !b->hd_before)
            plat_fatal("Out of memory allocating HD surface");
        if (hd_gpu_active) b->hd_gpu=host_gpu_create(b->w*hd_density,b->h*hd_density,0,NULL,0);
        if (b->hd_gpu) {
            hd_bitmap_changed(b,0,0,b->w,b->h);
            return;
        }
        for (j = 0; j < b->h; j++)
            for (i = 0; i < b->w; i++) {
                remember_pixel(b,i,j);
                block(b, i, j, legacy(b, i, j));
            }
    } else if (w > 0 && h > 0)
        sync_rect(b,x,y,x+w,y+h,0);
}
void hd_sync(Bitmap *b)
{
    if (b) hd_sync_rect(b,0,0,b->w,b->h);
}
void hd_bitmap_destroy(Bitmap *b)
{
    text_flush();
    if(b->hd_indexed_cache) {
        HdIndexedCache *c=b->hd_indexed_cache;
        if(c->texture)host_gpu_destroy(c->texture);
        free(c->indices);free(c);
    }
    if (b->hd_gpu) host_gpu_destroy(b->hd_gpu);
    free(b->hd);
    free(b->hd_before);
}
void hd_bitmap_commit(Bitmap *b)
{
    if (!b || !b->hd)
        return;
    sync_rect(b,0,0,b->w,b->h,1);
}
void hd_commit(void *bits) { hd_bitmap_commit(bmp_of_bits(bits)); }
void hd_bitmap_commit_rect(Bitmap *b, int x, int y, int w, int h)
{
    if (!b || !b->hd || w <= 0 || h <= 0)
        return;
    sync_rect(b,x,y,x+w,y+h,1);
}
void hd_commit_rect(void *bits, int x, int y, int w, int h)
{
    hd_bitmap_commit_rect(bmp_of_bits(bits),x,y,w,h);
}
void hd_changed(void *bits, int x, int y, int w, int h)
{
    if (hd_enabled)
        hd_bitmap_changed(bmp_of_bits(bits), x, y, w, h);
}
void hd_bitmap_changed(Bitmap *b, int x, int y, int w, int h)
{
    int i, j;
    if (!hd_enabled || !b || !b->hd)
        return;
    b->hd_image = 0;
    if(indexed_note(b,x,y,w,h))return;
    if (b->hd_gpu) {
        text_flush();
        static uint32_t *pixels;
        static size_t capacity;
        size_t n;
        int l=x<0 ? 0 : x,t=y<0 ? 0 : y,r=x+w,bot=y+h,uniform=1;
        if(r>b->w)r=b->w;if(bot>b->h)bot=b->h;
        if(l>=r || t>=bot)return;
        n=(size_t)(r-l)*(bot-t);
        if(n>capacity) {
            free(pixels);pixels=malloc(n*4);capacity=n;
            if(!pixels)plat_fatal("Out of memory updating HD surface");
        }
        for(j=t;j<bot;j++)for(i=l;i<r;i++) {
            uint32_t c=legacy(b,i,j);
            pixels[(j-t)*(r-l)+i-l]=c;
            if(c!=pixels[0])uniform=0;
            remember_pixel(b,i,j);
        }
        if(uniform)gpu_check(host_gpu_fill(b->hd_gpu,l*hd_density,t*hd_density,(r-l)*hd_density,(bot-t)*hd_density,pixels[0]));
        else gpu_check(host_gpu_upload(b->hd_gpu,l*hd_density,t*hd_density,r-l,bot-t,pixels,(r-l)*4,hd_density));
        return;
    }
    for (j = y < 0 ? 0 : y; j < y + h && j < b->h; j++)
        for (i = x < 0 ? 0 : x; i < x + w && i < b->w; i++) {
            block(b, i, j, legacy(b, i, j));
            remember_pixel(b,i,j);
        }
}
void hd_pixel(Bitmap *b, int x, int y, uint32_t rgb)
{
    if (!hd_enabled || !b->hd || x < 0 || y < 0 || x >= b->w || y >= b->h)
        return;
    b->hd_image = 0;
    if(indexed_note(b,x,y,1,1))return;
    block(b, x, y, 0xff000000u | rgb);
    remember_pixel(b,x,y);
}
void hd_indexed_begin(void *bits)
{
    Bitmap *b;
    HdIndexedCache *c;
    if(!hd_gpu_active || !(b=bmp_of_bits(bits)) || b->bpp!=8)return;
    hd_sync(b);
    if(!b->hd_gpu)return;
    c=b->hd_indexed_cache;
    if(!c) {
        c=calloc(1,sizeof *c);
        if(!c)return;
        b->hd_indexed_cache=c;
    }
    if(!c->depth++) { c->l=b->w;c->t=b->h;c->r=c->bot=0; }
}
void hd_indexed_end(void *bits)
{
    Bitmap *b=bmp_of_bits(bits);
    HdIndexedCache *c;
    HostGpuBlit draw;
    int w,h,y,same,d=hd_density;
    if(!b || !(c=b->hd_indexed_cache) || !c->depth || --c->depth)return;
    w=c->r-c->l;h=c->bot-c->t;
    if(w<=0 || h<=0)return;
    same=c->texture && c->w==w && c->h==h && !memcmp(c->colors,b->colors,sizeof c->colors);
    for(y=0;same && y<h;y++)
        if(!host_memory_equal(c->indices+y*w,bmp_row(b,c->t+y)+c->l,(uint32_t)w))same=0;
    if(same) {
        draw=(HostGpuBlit){b->hd_gpu,c->texture,c->l*d,c->t*d,w*d,h*d,0,0,w*d,h*d,
                          c->l*d,c->t*d,c->r*d,c->bot*d,0xffffffffu,0,0};
        gpu_check(host_gpu_blit(&draw));
        hd_bitmap_commit_rect(b,c->l,c->t,w,h);
        return;
    }
    hd_bitmap_changed(b,c->l,c->t,w,h);
    if(!c->texture || c->w!=w || c->h!=h) {
        int texture=host_gpu_create(w*d,h*d,0,NULL,0);
        unsigned char *indices=malloc((size_t)w*h);
        if(!texture || !indices) {
            if(texture)host_gpu_destroy(texture);
            free(indices);return;
        }
        if(c->texture)host_gpu_destroy(c->texture);
        free(c->indices);c->indices=indices;c->texture=texture;c->w=w;c->h=h;
    }
    draw=(HostGpuBlit){c->texture,b->hd_gpu,0,0,w*d,h*d,c->l*d,c->t*d,w*d,h*d,0,0,w*d,h*d,
                      0xffffffffu,0,0};
    gpu_check(host_gpu_blit(&draw));
    for(y=0;y<h;y++)memcpy(c->indices+y*w,bmp_row(b,c->t+y)+c->l,(size_t)w);
    memcpy(c->colors,b->colors,sizeof c->colors);
}
void hd_bitmap_invert(Bitmap *b, int x, int y, int w, int h)
{
    int px, py, d = hd_density;
    if (!hd_enabled || !b)
        return;
    hd_sync_rect(b,x,y,w,h);
    hd_read_region(b,x*d,y*d,w*d,h*d);
    b->hd_image = 0;
    for (py = (y < 0 ? 0 : y) * d; py < (y + h) * d && py < b->h * d; py++)
        for (px = (x < 0 ? 0 : x) * d; px < (x + w) * d && px < b->w * d; px++)
            b->hd[py * b->w * d + px] ^= 0x00ffffffu;
    hd_upload_region(b,x*d,y*d,w*d,h*d);
}
void hd_grey_pixels(uint32_t *pixels, uint64_t n, int strength)
{
    uint64_t i;
    if (strength < 0) strength = 0;
    if (strength > 255) strength = 255;
    for (i = 0; i < n; i++) {
        uint32_t c = pixels[i];
        unsigned r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
        unsigned grey = (30 * r + 59 * g + 11 * b + 50) / 100;
        r = (r * (255 - strength) + grey * strength + 127) / 255;
        g = (g * (255 - strength) + grey * strength + 127) / 255;
        b = (b * (255 - strength) + grey * strength + 127) / 255;
        pixels[i] = (c & 0xff000000u) | r << 16 | g << 8 | b;
    }
}
void hd_surface_grey(void *bits, int on)
{
    Bitmap *b = bmp_of_bits(bits);
    if (b) b->hd_grey = on;
}
void hd_attach(void *bits, int id)
{
    Bitmap *b = bmp_of_bits(bits);
    size_t i, n;
    if (!b)
        return;
    hd_sync(b);
    n = (size_t)b->w * b->h * hd_density * hd_density;
    if (b->hd_gpu) gpu_check(host_gpu_fill(b->hd_gpu,0,0,b->w*hd_density,b->h*hd_density,0xff263934u));
    else for (i = 0; i < n; i++) b->hd[i] = 0xff263934u;
    hd_draw(bits, id, 0, 0, b->w, b->h, 0, 0, b->w, b->h, -1);
    b->hd_image = id;
}
int hd_source(void *bits)
{
    Bitmap *b = bmp_of_bits(bits);
    return b ? b->hd_image : 0;
}
static uint32_t blend(uint32_t dst, unsigned r, unsigned g, unsigned b, unsigned a)
{
    unsigned ia = 255 - a;
    return 0xff000000u | ((r + ((dst >> 16) & 255) * ia + 127) / 255) << 16 |
           ((g + ((dst >> 8) & 255) * ia + 127) / 255) << 8 | (b + (dst & 255) * ia + 127) / 255;
}
/* Integrate the source footprint before bilinear sampling. Four point samples
   can miss a flagpole or a thin unit detail entirely at small map zooms. Work
   in premultiplied color so transparent RGB never creates a fringe. The cache
   belongs to the decoded atlas image and is included in its eviction budget. */
static void minify(Image *a, Image *sample, int w, int h)
{
    int x, y;
    if (w > a->pw) w = a->pw;
    if (h > a->ph) h = a->ph;
    *sample = *a;
    if ((w == a->pw && h == a->ph) || !a->length) return;
    if (a->small_w != w || a->small_h != h) {
        cache_bytes -= (uint64_t)a->small_w * a->small_h * 4;
        free(a->small);
        a->small = malloc((size_t)w * h * 4);
        if (!a->small) plat_fatal("Out of memory filtering Atlas image");
        a->small_w = w; a->small_h = h;
        cache_bytes += (uint64_t)w * h * 4;
        for (y = 0; y < h; y++) for (x = 0; x < w; x++) {
            int sx, sy, left = x*a->pw, right = (x+1)*a->pw;
            int top = y*a->ph, bottom = (y+1)*a->ph;
            double rr = 0, gg = 0, bb = 0, aa = 0;
            unsigned char *out = a->small + (y*w+x)*4;
            for (sy = top/h; sy*h < bottom; sy++) {
                int t = sy*h > top ? sy*h : top;
                int b = (sy+1)*h < bottom ? (sy+1)*h : bottom;
                for (sx = left/w; sx*w < right; sx++) {
                    int l = sx*w > left ? sx*w : left;
                    int r = (sx+1)*w < right ? (sx+1)*w : right;
                    const unsigned char *p = a->rgba + (sy*a->pw+sx)*4;
                    double alpha = (double)(r-l)*(b-t)*p[3];
                    aa += alpha;
                    rr += alpha*p[0]; gg += alpha*p[1]; bb += alpha*p[2];
                }
            }
            out[0] = aa ? (unsigned char)(rr/aa+.5) : 0;
            out[1] = aa ? (unsigned char)(gg/aa+.5) : 0;
            out[2] = aa ? (unsigned char)(bb/aa+.5) : 0;
            out[3] = (unsigned char)(aa/((double)a->pw*a->ph)+.5);
        }
    }
    sample->rgba = a->small;
    sample->pw = w; sample->ph = h;
}
void hd_draw(void *bits, int id, int x, int y, int w, int h, int l, int t, int r, int bot, int tint)
{
    hd_draw_bitmap(bmp_of_bits(bits), id, x, y, w, h, l, t, r, bot, tint);
}
static Sample filtered_sample(const Image *a, int x, int y, int w, int h, int tint, int grey, int key)
{
    double u = (x+.5)*a->pw/w-.5, v = (y+.5)*a->ph/h-.5;
    int xx = (int)floor(u), yy = (int)floor(v), i, j;
    double fx = u-xx, fy = v-yy, rr = 0, gg = 0, bb = 0, aa = 0;
    Sample out;
    for (j = 0; j < 2; j++) for (i = 0; i < 2; i++) {
        int sx = xx+i, sy = yy+j;
        const unsigned char *p;
        double k = (i ? fx : 1-fx)*(j ? fy : 1-fy);
        if (sx < 0) sx = 0;
        if (sy < 0) sy = 0;
        if (sx >= a->pw) sx = a->pw-1;
        if (sy >= a->ph) sy = a->ph-1;
        p = a->rgba + 4*(sy*a->pw+sx);
        if (key >= 0 && a->indexed &&
            a->indexed[(sy*a->h/a->ph)*a->w+sx*a->w/a->pw] == (unsigned)key) continue;
        aa += k*p[3]; rr += k*p[0]*p[3]; gg += k*p[1]*p[3]; bb += k*p[2]*p[3];
    }
    if (tint >= 0) {
        uint32_t c = palette32[tint & 255];
        rr = ((c>>16)&255)*aa; gg = ((c>>8)&255)*aa; bb = (c&255)*aa;
    } else if (grey) rr = gg = bb = (30*rr+59*gg+11*bb)/100;
    out.r = (unsigned)(rr+.5); out.g = (unsigned)(gg+.5);
    out.b = (unsigned)(bb+.5); out.a = (unsigned)(aa+.5);
    return out;
}
static const Sample *filtered_pixels(Image *image, const Image *a, int w, int h, int tint, int grey)
{
    Filtered **link = &filtered, *p;
    uint64_t bytes = (uint64_t)w*h*sizeof(Sample);
    int x, y;
    /* Large one-off dialog/background stretches must not evict the map cache. */
    if (bytes > HD_FILTERED_BUDGET/8) return NULL;
    while ((p = *link)) {
        if (p->image == image && p->w == w && p->h == h && p->tint == tint && p->grey == grey) {
            *link = p->next; p->next = filtered; filtered = p;
            return p->pixels;
        }
        link = &p->next;
    }
    while (filtered_bytes+bytes > HD_FILTERED_BUDGET) {
        link = &filtered;
        while ((*link)->next) link = &(*link)->next;
        filtered_remove(link);
    }
    p = malloc(sizeof *p);
    if (!p) plat_fatal("Out of memory caching filtered Atlas image");
    p->pixels = malloc((size_t)bytes);
    if (!p->pixels) plat_fatal("Out of memory caching filtered Atlas pixels");
    p->image = image; p->w = w; p->h = h; p->tint = tint; p->grey = grey;
    p->next = filtered; filtered = p; filtered_bytes += bytes;
    for (y = 0; y < h; y++) for (x = 0; x < w; x++)
        p->pixels[y*w+x] = filtered_sample(a,x,y,w,h,tint,grey,-1);
    return p->pixels;
}
static int gpu_paint(Bitmap *b,Image *a,int x,int y,int w,int h,int l,int t,int r,int bot,int tint,int grey,int key)
{
    int d=hd_density,sw,sh,texture,kind=tint>=0 ? 3 : grey ? 2 : 1;
    Image sample,*source=a;
    HostGpuBlit draw;
    if(!a->pw && a->length)a=decode((int)(a-images)+1);
    sw=a->pw;sh=a->ph;
    if(a->length && key<0) { if(sw>w*d)sw=w*d;if(sh>h*d)sh=h*d; }
    texture=gpu_image_find(source,sw,sh,kind,key);
    if(!texture) {
        unsigned char *masked=NULL;
        const unsigned char *rgba;
        if(a->length) {
            a=decode((int)(source-images)+1);
            minify(a,&sample,sw,sh);a=&sample;
        }
        rgba=a->rgba;
        if(key>=0 && a->indexed) {
            int px,py;
            masked=malloc((size_t)sw*sh*4);
            if(!masked) return 0;
            memcpy(masked,rgba,(size_t)sw*sh*4);
            for(py=0;py<sh;py++)for(px=0;px<sw;px++)
                if(a->indexed[(py*a->h/sh)*a->w+px*a->w/sw]==(unsigned)key)
                    masked[(py*sw+px)*4+3]=0;
            rgba=masked;
        }
        texture=gpu_image_add(source,sw,sh,kind,key,rgba,kind);
        free(masked);
        if(!texture)return 0;
    }
    draw=(HostGpuBlit){b->hd_gpu,texture,x*d,y*d,w*d,h*d,0,0,sw,sh,l*d,t*d,r*d,bot*d,
                      tint>=0 ? palette32[tint & 255] : 0xffffffffu,1,1};
    gpu_check(host_gpu_blit(&draw));
    return 1;
}
int hd_gpu_text(Bitmap *b,const unsigned char *cov,int sw,int sh,int x,int y,int w,int h,
                int l,int t,int r,int bot,uint32_t color)
{
    static const unsigned char ramp[17]={0,0x4d,0x68,0x7c,0x8c,0x9a,0xa7,0xb2,0xbd,0xc7,0xd0,0xd9,0xe1,0xe9,0xf0,0xf8,0xff};
    int kind,texture,px,py;
    HostGpuBlit draw;
    unsigned char *rgba;
    color&=0xffffff;
    if(!b->hd_gpu || w<=0 || h<=0)return 0;
    if(l>=r || t>=bot)return 1;
    if(color && color!=0xffffff) {
        HostGpuText text;
        int layer;
        if(!(host_gpu_available()&2))return 0;
        for(layer=0;layer<2;layer++) {
            kind=((layer+1)<<24)|color;
            texture=gpu_image_find(cov,w,h,kind,-1);
            if(!texture) {
                rgba=malloc((size_t)w*h*4);
                if(!rgba)return 0;
                for(py=0;py<h;py++)for(px=0;px<w;px++) {
                    int c,v=cov[(py*sh/h)*sw+px*sw/w];
                    unsigned char *p=rgba+(py*w+px)*4;
                    for(c=0;c<3;c++) {
                        int t=(color>>(16-c*8))&255,value;
                        if(v<=1)value=0;
                        else if(v>=16)value=t;
                        else {
                            /* RGB is the intercept, 1-alpha is the slope:
                               one premultiplied blend evaluates each branch. */
                            value=layer==0 ? ramp[v]*t/255 : t-(t*ramp[16-v]+127)/255;
                        }
                        p[c]=value;
                    }
                    p[3]=v<=1 ? 0 : v>=16 ? 255 : layer==0 ? ramp[v] : 255-ramp[16-v];
                }
                texture=gpu_image_add(cov,w,h,kind,-1,rgba,4);free(rgba);
                if(!texture)return 0;
            }
            text.layers[layer]=texture;
        }
        text.draw=(HostGpuBlit){b->hd_gpu,0,x,y,w,h,0,0,w,h,l,t,r,bot,color,0,0};
        if(text_batching) {
            int n;
            if(text_count==128)text_flush();
            for(n=0;n<text_count;n++) {
                const HostGpuBlit *p=&text_batch[n].draw;
                if(p->dst!=text.draw.dst || (p->l<r && l<p->r && p->t<bot && t<p->b)) {
                    text_flush();break;
                }
            }
            text_batch[text_count++]=text;
        } else gpu_check(host_gpu_text(&text));
        return 1;
    }
    text_flush();
    kind=color ? 11 : 10;
    texture=gpu_image_find(cov,w,h,kind,-1);
    if(!texture) {
        rgba=malloc((size_t)w*h*4);
        if(!rgba)return 0;
        for(py=0;py<h;py++)for(px=0;px<w;px++) {
            int v=cov[(py*sh/h)*sw+px*sw/w];
            unsigned char *p=rgba+(py*w+px)*4;
            p[0]=p[1]=p[2]=255;
            p[3]=v<=1 ? 0 : color ? ramp[v] : 255-ramp[16-v];
        }
        texture=gpu_image_add(cov,w,h,kind,-1,rgba,1);free(rgba);
        if(!texture)return 0;
    }
    draw=(HostGpuBlit){b->hd_gpu,texture,x,y,w,h,0,0,w,h,l,t,r,bot,color,1,0};
    gpu_check(host_gpu_blit(&draw));
    return 1;
}
static void paint(Bitmap *b, Image *a, int x, int y, int w, int h, int l, int t, int r, int bot, int tint,
                  int grey, int key)
{
    int px, py, d = hd_density;
    Image sample;
    Image *image = a;
    const Sample *cached = NULL;
    if (!b || !a || w <= 0 || h <= 0)
        return;
    if (!b->hd)
        hd_sync(b);
    b->hd_image = 0;
    if (l < x)
        l = x;
    if (t < y)
        t = y;
    if (r > x + w)
        r = x + w;
    if (bot > y + h)
        bot = y + h;
    if (l < 0)
        l = 0;
    if (t < 0)
        t = 0;
    if (r > b->w)
        r = b->w;
    if (bot > b->h)
        bot = b->h;
    if (l >= r || t >= bot) return;
    if(b->hd_gpu) {
        if(gpu_paint(b,a,x,y,w,h,l,t,r,bot,tint,grey,key))return;
        hd_read_region(b,l*d,t*d,(r-l)*d,(bot-t)*d);
    }
    if(!a->rgba && a->length)a=decode((int)(a-images)+1);
    /* Dynamic captures retain their indexed key mask; atlas alpha is already
       authored, so it can be filtered once and reused across clipped draws. */
    if (key < 0) {
        minify(a, &sample, w*d, h*d);
        a = &sample;
    }
    /* The minification cache normally already has the exact output size. */
    if (key < 0 && a->pw == w*d && a->ph == h*d) {
        uint32_t color = tint >= 0 ? palette32[tint & 255] : 0;
        for (py = t*d; py < bot*d; py++) {
            const unsigned char *p = a->rgba + 4*((py-y*d)*a->pw+(l-x)*d);
            uint32_t *out = b->hd + py*b->w*d+l*d;
            for (px = l*d; px < r*d; px++, p += 4, out++) {
                unsigned aa = p[3], rr, gg, bb;
                if (!aa) continue;
                if (tint >= 0) { rr=(color>>16)&255; gg=(color>>8)&255; bb=color&255; }
                else { rr=p[0]; gg=p[1]; bb=p[2]; }
                if (aa == 255 && (!grey || tint >= 0)) {
                    *out = 0xff000000u | rr<<16 | gg<<8 | bb;
                    continue;
                }
                rr *= aa; gg *= aa; bb *= aa;
                if (grey && tint < 0) rr=gg=bb=(30*rr+59*gg+11*bb+50)/100;
                *out = blend(*out,rr,gg,bb,aa);
            }
        }
        hd_upload_region(b,l*d,t*d,(r-l)*d,(bot-t)*d);
        return;
    }
    if (key < 0 && image->length) cached = filtered_pixels(image,a,w*d,h*d,tint,grey);
    /* Bilinear filtering in premultiplied color space: RGB never leaks from
       transparent texels. Tint affects color independently of alpha coverage. */
    for (py = t * d; py < bot * d; py++)
        for (px = l * d; px < r * d; px++) {
            Sample c = cached ? cached[(py-y*d)*w*d+px-x*d] :
                                filtered_sample(a,px-x*d,py-y*d,w*d,h*d,tint,grey,key);
            if (c.a) b->hd[py*b->w*d+px] = blend(b->hd[py*b->w*d+px],c.r,c.g,c.b,c.a);
        }
    hd_upload_region(b,l*d,t*d,(r-l)*d,(bot-t)*d);
}
void hd_draw_bitmap(Bitmap *b, int id, int x, int y, int w, int h, int l, int t, int r, int bot, int tint)
{
    paint(b, draw_image(id), x, y, w, h, l, t, r, bot, tint, 0, -1);
}
void hd_transfer(Bitmap *dst, Bitmap *src, int x, int y, int w, int h, int sx, int sy, int sw, int sh, int l,
                 int t, int r, int bot)
{
    int px, py, d = hd_density;
    uint32_t *copy = NULL, *pixels;
    if (!hd_enabled || !dst || !src || w <= 0 || h <= 0 || sw <= 0 || sh <= 0)
        return;
    if (l < x)
        l = x;
    if (t < y)
        t = y;
    if (r > x + w)
        r = x + w;
    if (bot > y + h)
        bot = y + h;
    if (l < 0)
        l = 0;
    if (t < 0)
        t = 0;
    if (r > dst->w)
        r = dst->w;
    if (bot > dst->h)
        bot = dst->h;
    if (l >= r || t >= bot) return;
    hd_sync_rect(src,sx,sy,sw,sh);
    if (dst != src) hd_sync_rect(dst,l,t,r-l,bot-t);
    dst->hd_image = 0;
    /* SDL clips an out-of-bounds source before scaling. Restrict opaque GPU
       copies to the unscaled case, preserving the legacy sampling origin. */
    if(w==sw && h==sh) {
        if(l<x-sx)l=x-sx;if(t<y-sy)t=y-sy;
        if(r>x+src->w-sx)r=x+src->w-sx;if(bot>y+src->h-sy)bot=y+src->h-sy;
        if(l>=r || t>=bot)return;
    }
    if(dst->hd_gpu && src->hd_gpu && w==sw && h==sh && (!src->hd_grey || dst==src)) {
        HostGpuBlit draw={dst->hd_gpu,src->hd_gpu,l*d,t*d,(r-l)*d,(bot-t)*d,
                         (sx+l-x)*d,(sy+t-y)*d,(r-l)*d,(bot-t)*d,l*d,t*d,r*d,bot*d,0xffffffffu,0,0};
        gpu_check(host_gpu_blit(&draw));
        return;
    }
    hd_read_region(src,sx*d,sy*d,sw*d,sh*d);
    hd_read_region(dst,l*d,t*d,(r-l)*d,(bot-t)*d);
    /* Most transfers are unscaled tile/row copies. memmove plus row direction
       preserves overlapping scrolls without allocating a whole-surface copy. */
    if (w == sw && h == sh) {
        int first, end, step = 1;
        if (l < x-sx) l = x-sx;
        if (t < y-sy) t = y-sy;
        if (r > x+src->w-sx) r = x+src->w-sx;
        if (bot > y+src->h-sy) bot = y+src->h-sy;
        if (l >= r || t >= bot) return;
        first = t*d; end = bot*d;
        if (src == dst && y > sy) { first = bot*d-1; end = t*d-1; step = -1; }
        for (py = first; py != end; py += step) {
            uint32_t *out = dst->hd + py*dst->w*d + l*d;
            const uint32_t *in = src->hd + (py+(sy-y)*d)*src->w*d + (l+sx-x)*d;
            memmove(out,in,(size_t)(r-l)*d*4);
            if (src->hd_grey && dst != src) hd_grey_pixels(out,(uint64_t)(r-l)*d,255);
        }
        hd_upload_region(dst,l*d,t*d,(r-l)*d,(bot-t)*d);
        return;
    }
    pixels = src->hd;
    if (dst == src) {
        size_t n = (size_t)src->w * src->h * d * d * 4;
        copy = malloc(n);
        if (!copy) plat_fatal("Out of memory copying HD surface");
        memcpy(copy,pixels,n);
        pixels = copy;
    }
    for (py = t * d; py < bot * d; py++)
        for (px = l * d; px < r * d; px++) {
            int ax = sx * d + (int)((int64_t)(px - x * d) * sw / w),
                ay = sy * d + (int)((int64_t)(py - y * d) * sh / h);
            if (ax >= 0 && ay >= 0 && ax < src->w * d && ay < src->h * d) {
                uint32_t c = pixels[ay * src->w * d + ax];
                if (src->hd_grey && dst != src) hd_grey_pixels(&c,1,255);
                dst->hd[py * dst->w * d + px] = c;
            }
        }
    free(copy);
    hd_upload_region(dst,l*d,t*d,(r-l)*d,(bot-t)*d);
}
void hd_copy(void *dst, void *src, int x, int y, int sx, int sy, int w, int h)
{
    if (hd_enabled)
        hd_transfer(bmp_of_bits(dst), bmp_of_bits(src), x, y, w, h, sx, sy, w, h, x, y, x + w, y + h);
}
void hd_composite(Bitmap *dst,Bitmap *src,int x,int y,int l,int t,int r,int bot)
{
    int py,d=hd_density;
    if(l<x)l=x;if(t<y)t=y;if(r>x+src->w)r=x+src->w;if(bot>y+src->h)bot=y+src->h;
    if(l<0)l=0;if(t<0)t=0;if(r>dst->w)r=dst->w;if(bot>dst->h)bot=dst->h;
    if(l>=r || t>=bot)return;
    hd_sync_rect(src,l-x,t-y,r-l,bot-t);
    if(!dst->hd)hd_sync(dst);
    if(dst->hd_gpu && src->hd_gpu) {
        HostGpuBlit draw={dst->hd_gpu,src->hd_gpu,x*d,y*d,src->w*d,src->h*d,0,0,src->w*d,src->h*d,
                         l*d,t*d,r*d,bot*d,0xffffffffu,0,0};
        gpu_check(host_gpu_blit(&draw));return;
    }
    hd_read_region(src,(l-x)*d,(t-y)*d,(r-l)*d,(bot-t)*d);
    for(py=t*d;py<bot*d;py++)
        memcpy(dst->hd+py*dst->w*d+l*d,src->hd+(py-y*d)*src->w*d+(l-x)*d,(size_t)(r-l)*d*4);
    hd_upload_region(dst,l*d,t*d,(r-l)*d,(bot-t)*d);
}
const uint32_t *hd_pixels(Bitmap *b)
{
    hd_sync(b);
    hd_read_region(b,0,0,b->w*hd_density,b->h*hd_density);
    return b->hd;
}
uint32_t *hd_debug_capture_rect(void *bits,int x,int y,int w,int h)
{
    Bitmap *b=bmp_of_bits(bits);
    int d=hd_density,py;
    uint32_t *pixels;
    if(!b || x<0 || y<0 || x+w>b->w || y+h>b->h)return 0;
    hd_sync_rect(b,x,y,w,h);hd_read_region(b,x*d,y*d,w*d,h*d);
    pixels=malloc((size_t)w*h*d*d*4);
    if(!pixels)plat_fatal("Out of memory comparing viewport cache");
    for(py=0;py<h*d;py++)memcpy(pixels+py*w*d,b->hd+(y*d+py)*b->w*d+x*d,(size_t)w*d*4);
    return pixels;
}
int hd_debug_compare_rect(void *bits,int x,int y,int w,int h,const uint32_t *reference,int tolerance)
{
    Bitmap *b=bmp_of_bits(bits);
    int d=hd_density,px,py,differences=0,worst=0;
    if(!reference || !b || x<0 || y<0 || x+w>b->w || y+h>b->h)return 0;
    hd_sync_rect(b,x,y,w,h);hd_read_region(b,x*d,y*d,w*d,h*d);
    for(py=y*d;py<(y+h)*d;py++)for(px=x*d;px<(x+w)*d;px++) {
        uint32_t value=b->hd[py*b->w*d+px],old=reference[(py-y*d)*w*d+px-x*d];
        int channel;
        if(value!=old)differences++;
        for(channel=0;channel<32;channel+=8) {
            int error=abs((int)((value>>channel)&255)-(int)((old>>channel)&255));
            if(error>worst)worst=error;
        }
    }
    if(differences)plat_log("camera cache comparison: %d differing pixels, max channel error %d\n",differences,worst);
    return worst<=tolerance;
}
void hd_composite_camera(Bitmap *dst,Bitmap *src,int sx,int sy,int sw,int sh,
                         double x,double y,double w,double h,int l,int t,int r,int bot)
{
    int d=hd_density;
    hd_sync_rect(src,sx,sy,sw,sh);
    if(!dst->hd)hd_sync(dst);
    HostGpuCamera draw={dst->hd_gpu,src->hd_gpu,sx*d,sy*d,sw*d,sh*d,
        (float)(x*d),(float)(y*d),(float)(w*d),(float)(h*d),l*d,t*d,r*d,bot*d};
    gpu_check(host_gpu_camera(&draw));
}

typedef struct SpriteImage {
    const void *sprite;
    int image, tint, key, grey;
    Image snapshot;
    struct SpriteImage *next;
} SpriteImage;
static SpriteImage *sprites[1024];
static SpriteImage **sprite_bucket(const void *s)
{
    uintptr_t key = (uintptr_t)s;
    return &sprites[((key>>4) ^ (key>>14)) & 1023];
}
static SpriteImage *sprite_image(const void *s)
{
    SpriteImage *p;
    for (p = *sprite_bucket(s); p; p = p->next)
        if (p->sprite == s)
            return p;
    return NULL;
}
int hd_sprite_image(const void *s)
{
    SpriteImage *p = sprite_image(s);
    return p ? p->image : 0;
}
void hd_sprite_forget(const void *s)
{
    SpriteImage **p = sprite_bucket(s);
    while (*p) {
        if ((*p)->sprite == s) {
            SpriteImage *old = *p;
            *p = old->next;
            gpu_image_forget(&old->snapshot);
            free(old->snapshot.rgba);
            free(old->snapshot.indexed);
            free(old);
            return;
        }
        p = &(*p)->next;
    }
}
void hd_sprite_capture(const void *s, void *bits, int x, int y, int w, int h, int key)
{
    Bitmap *b = bmp_of_bits(bits);
    int id, crop, px, py, d = hd_density;
    SpriteImage *p;
    hd_sprite_forget(s);
    if (!b || w <= 0 || h <= 0 || x < 0 || y < 0 || x + w > b->w || y + h > b->h)
        return;
    hd_sync(b);
    id = hd_source(bits);
    crop = hd_crop(id, x, y, w, h);
    p = calloc(1, sizeof *p);
    if (!p)
        plat_fatal("Out of memory capturing Atlas sprite");
    p->sprite = s;
    p->key = key == -1 ? (int)raw_pixel(b, x, y) : key;
    p->image = crop;
    p->tint = -1;
    p->next = *sprite_bucket(s);
    *sprite_bucket(s) = p;
    if (crop)
        return;
    p->snapshot.w = w;
    p->snapshot.h = h;
    p->snapshot.pw = w * d;
    p->snapshot.ph = h * d;
    p->snapshot.rgba = malloc((size_t)w * h * d * d * 4);
    p->snapshot.indexed = malloc((size_t)w * h);
    if (!p->snapshot.rgba || !p->snapshot.indexed)
        plat_fatal("Out of memory capturing Atlas surface");
    key = p->key;
    hd_read_region(b,x*d,y*d,w*d,h*d);
    for (py = 0; py < h; py++)
        for (px = 0; px < w; px++)
            p->snapshot.indexed[py * w + px] = raw_pixel(b, x + px, y + py);
    for (py = 0; py < h * d; py++)
    {
        int first = 0, last = w;
        /* The indexed encoder trims each row. Alternate draw keys can reveal
           holes inside a row, but must not reveal the discarded outside. */
        if (key >= 0) {
            while (first < last && p->snapshot.indexed[(py / d) * w + first] == key) first++;
            while (last > first && p->snapshot.indexed[(py / d) * w + last - 1] == key) last--;
        }
        for (px = 0; px < w * d; px++) {
            uint32_t c = b->hd[(y * d + py) * b->w * d + x * d + px];
            unsigned char *q = p->snapshot.rgba + (py * w * d + px) * 4;
            q[0] = c >> 16;
            q[1] = c >> 8;
            q[2] = c;
            q[3] = px < first * d || px >= last * d ? 0 : 255;
        }
    }
}
int hd_sprite_draw(const void *s, void *bits, int x, int y, int w, int h, int l, int t, int r, int b,
                   int tint)
{
    return hd_sprite_draw_keyed(s,bits,x,y,w,h,l,t,r,b,tint,-1);
}
int hd_sprite_draw_keyed(const void *s, void *bits, int x, int y, int w, int h, int l, int t, int r, int b,
                         int tint, int key)
{
    SpriteImage *p = sprite_image(s);
    int variant;
    if (!p)
        return 0;
    /* Authored color variants preserve borders and interior detail. Explicit
       silhouette draws (shadows, sleeping units) still tint the entire mask. */
    if (tint < 0 && p->tint >= 0 && (variant = hd_variant(p->image, p->tint))) {
        paint(bmp_of_bits(bits), draw_image(variant), x, y, w, h, l, t, r, b, -1, p->grey, -1);
        return 1;
    }
    paint(bmp_of_bits(bits), p->image ? draw_image(p->image) : &p->snapshot, x, y, w, h, l, t, r, b,
          tint < 0 ? p->tint : tint, p->grey, p->image ? -1 : key < 0 ? p->key : key & 255);
    return 1;
}
void hd_sprite_greyscale(const void *s, int on)
{
    SpriteImage *p = sprite_image(s);
    if (p) p->grey = on;
}
void hd_sprite_tint(const void *s, int old_color, int new_color)
{
    SpriteImage *p = sprite_image(s);
    if (p && p->snapshot.indexed) {
        int x, y;
        Image *a = &p->snapshot;
        gpu_image_forget(a);
        for (y = 0; y < a->ph; y++)
            for (x = 0; x < a->pw; x++)
                if (a->indexed[(y * a->h / a->ph) * a->w + x * a->w / a->pw] == (old_color & 255)) {
                    unsigned char *c = a->rgba + (y * a->pw + x) * 4;
                    memcpy(c, palette + (new_color & 255) * 3, 3);
                }
        for (x = 0; x < (int)(a->w * a->h); x++)
            if (a->indexed[x] == (old_color & 255)) a->indexed[x] = new_color;
    }
    else if (p && old_color == 8)
        p->tint = new_color;
}
void hd_sprite_copy(const void *dst, const void *src)
{
    SpriteImage *p = sprite_image(src), *q;
    if (!p || dst == src)
        return;
    q = malloc(sizeof *q);
    if (!q)
        plat_fatal("Out of memory cloning Atlas sprite");
    *q = *p;
    if (p->snapshot.rgba) {
        size_t n = (size_t)p->snapshot.pw * p->snapshot.ph * 4;
        q->snapshot.rgba = malloc(n);
        if (!q->snapshot.rgba)
            plat_fatal("Out of memory cloning Atlas pixels");
        memcpy(q->snapshot.rgba, p->snapshot.rgba, n);
    }
    if (p->snapshot.indexed) {
        size_t n = (size_t)p->snapshot.w * p->snapshot.h;
        q->snapshot.indexed = malloc(n);
        if (!q->snapshot.indexed) plat_fatal("Out of memory cloning Atlas indices");
        memcpy(q->snapshot.indexed, p->snapshot.indexed, n);
    }
    hd_sprite_forget(dst);
    q->sprite = dst;
    q->next = *sprite_bucket(dst);
    *sprite_bucket(dst) = q;
}
void hd_sprite_bind(const void *s, const char *key)
{
    int id = hd_find(key);
    SpriteImage *p;
    if (!id)
        plat_fatal("Missing Atlas sprite: %s", key);
    hd_sprite_forget(s);
    p = calloc(1, sizeof *p);
    p->sprite = s;
    p->image = id;
    p->tint = -1;
    p->next = *sprite_bucket(s);
    *sprite_bucket(s) = p;
}
