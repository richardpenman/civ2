/* Tests the production HD compositor with a tiny authored alpha fixture.
   No original game, window system or replacement-art collection is required. */
#include "../src/platform/hd.h"
#include "../src/platform/plat.h"
#include <assert.h>

char plat_data_dir[1024];
static Bitmap *bitmaps[8];
void plat_log(const char *fmt, ...) { (void)fmt; }
void plat_fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(2);
}
UINT WINAPI GetPrivateProfileIntA(LPCSTR section, LPCSTR key, INT value, LPCSTR file)
{
    const char *s = getenv(key);
    (void)section;
    (void)file;
    return s ? (UINT)atoi(s) : value;
}
unsigned char *bmp_row(const Bitmap *b, int y)
{
    return b->bits + (b->bottom_up ? b->h - 1 - y : y) * b->stride;
}
Bitmap *bmp_of_bits(const void *bits)
{
    int i;
    for (i = 0; i < 8; i++)
        if (bitmaps[i] && bitmaps[i]->bits == bits)
            return bitmaps[i];
    return NULL;
}
COLORREF surf_get(Bitmap *b, int x, int y)
{
    unsigned n = bmp_row(b, y)[x];
    RGBQUAD c = b->colors[n];
    return RGB(c.rgbRed, c.rgbGreen, c.rgbBlue);
}
static Bitmap *bitmap(int index, int bottom)
{
    Bitmap *b = calloc(1, sizeof *b);
    int i;
    const unsigned char *pal = hd_palette();
    b->w = 16;
    b->h = 12;
    b->stride = 16;
    b->bpp = 8;
    b->bottom_up = bottom;
    b->bits = calloc(192, 1);
    for (i = 0; i < 256; i++) {
        b->colors[i].rgbRed = pal[i * 3];
        b->colors[i].rgbGreen = pal[i * 3 + 1];
        b->colors[i].rgbBlue = pal[i * 3 + 2];
    }
    bitmaps[index] = b;
    return b;
}
static void zoom_clips(Bitmap *a, Bitmap *b, int id)
{
    static const int sizes[][2] = {{64,48},{14,22},{12,20}};
    static const int positions[][2] = {{-5,-3},{0,0},{2,3},{13,9},{18,14}};
    static const int clips[][4] = {{0,0,16,12},{2,3,13,9},{7,5,7,5}};
    int z, s, p, c, tint, x, y, d = hd_density;
    for (z = -7; z <= 8; z++) for (s = 0; s < 3; s++)
        for (p = 0; p < 5; p++) for (tint = -1; tint <= 3; tint += 4) {
            int w = sizes[s][0]*(z+8)/8, h = sizes[s][1]*(z+8)/8;
            memset(a->bits,0,192);hd_changed(a->bits,0,0,16,12);
            hd_draw(a->bits,id,positions[p][0],positions[p][1],w,h,0,0,16,12,tint);
            for (c = 0; c < 3; c++) {
                memset(b->bits,0,192);hd_changed(b->bits,0,0,16,12);
                hd_draw(b->bits,id,positions[p][0],positions[p][1],w,h,
                        clips[c][0],clips[c][1],clips[c][2],clips[c][3],tint);
                for (y = 0; y < 12*d; y++) for (x = 0; x < 16*d; x++) {
                    int inside = x >= clips[c][0]*d && x < clips[c][2]*d &&
                                 y >= clips[c][1]*d && y < clips[c][3]*d;
                    assert(b->hd[y*16*d+x] == (inside ? a->hd[y*16*d+x] : 0xff000000u));
                }
            }
        }
}
static void copy_clips(Bitmap *a, Bitmap *b)
{
    static const int moves[][4] = {{0,0,2,2},{2,2,0,0},{2,0,0,0},{0,2,0,0},
                                  {-3,-2,1,0},{0,0,-3,-2},{13,9,1,1},{1,1,13,9}};
    int d=hd_density, i, j, self, x, y;
    size_t bytes=(size_t)a->w*a->h*d*d*4;
    uint32_t *before=malloc(bytes), *expected=malloc(bytes);
    for (self=0;self<2;self++) for (i=0;i<8;i++) {
        Bitmap *dst=self ? a : b;
        int dx=moves[i][0],dy=moves[i][1],sx=moves[i][2],sy=moves[i][3];
        memset(a->bits,0,192);memset(b->bits,0,192);
        hd_sync(a);hd_sync(b);hd_bitmap_commit(a);hd_bitmap_commit(b);
        /* Unique physical pixels expose accidental logical-pixel replication,
           wrong overlap direction and bottom-up indexed-row confusion. */
        for (j=0;j<16*12*d*d;j++) { a->hd[j]=0xff000000u+j; b->hd[j]=0xffabcdefu; }
        memcpy(before,a->hd,bytes);memcpy(expected,dst->hd,bytes);
        for (y=2*d;y<10*d;y++) for (x=1*d;x<14*d;x++) {
            int ax=sx*d+x-dx*d,ay=sy*d+y-dy*d;
            if (x>=dx*d && y>=dy*d && x<(dx+12)*d && y<(dy+9)*d &&
                ax>=0 && ay>=0 && ax<16*d && ay<12*d)
                expected[y*16*d+x]=before[ay*16*d+ax];
        }
        hd_transfer(dst,a,dx,dy,12,9,sx,sy,12,9,1,2,14,10);
        assert(!memcmp(dst->hd,expected,bytes));
    }
    free(before);free(expected);
    /* A copy's commit must not hide an unrelated pending indexed write. This
       occurs when GDI borders and HD sprites update the same destination. */
    memset(a->bits,0,192);memset(b->bits,0,192);
    hd_changed(a->bits,0,0,16,12);hd_changed(b->bits,0,0,16,12);
    bmp_row(b,11)[15]=3;
    hd_copy(b->bits,a->bits,0,0,0,0,2,2);
    hd_commit_rect(b->bits,0,0,2,2);
    assert(b->hd[11*d*16*d+15*d]==0xff000000u);
    hd_sync(b);
    assert(b->hd[11*d*16*d+15*d]==0xffff0000u);
    /* Source writes inside the copy must be visible immediately; outside
       writes are deferred but remain observable by the next whole sync. */
    bmp_row(a,1)[1]=2;bmp_row(a,11)[15]=3;
    hd_copy(b->bits,a->bits,0,0,0,0,2,2);
    assert(b->hd[d*16*d+d]==0xff00ff00u);
    assert(a->hd[11*d*16*d+15*d]==0xff000000u);
    hd_sync(a);
    assert(a->hd[11*d*16*d+15*d]==0xffff0000u);
    /* Window surfaces have no GDI bitmap handle. Their commits must use the
       Bitmap directly, or a later sync overwrites HD art with indexed pixels. */
    bitmaps[1]=NULL;
    bmp_row(b,0)[0]=3;
    b->hd[0]=0xffabcdefu;
    hd_bitmap_commit_rect(b,0,0,1,1);
    hd_sync(b);
    assert(b->hd[0]==0xffabcdefu);
    bitmaps[1]=b;
}
static void filter_samples(Bitmap *b)
{
    static const int widths[] = {1,3,4,5,9,16,64};
    uint64_t hash=14695981039346656037ull;
    int sprite=0, pass, w, h, grey, tint, i, d=hd_density;
    hd_sprite_bind(&sprite,"file/test");
    for (pass=0;pass<2;pass++) for (grey=0;grey<2;grey++) {
        hd_sprite_greyscale(&sprite,grey);
        for (tint=-1;tint<=3;tint+=4) for (w=0;w<7;w++) for (h=1;h<=9;h+=4) {
            memset(b->bits,3,192);hd_changed(b->bits,0,0,16,12);
            hd_sprite_draw(&sprite,b->bits,-2,-1,widths[w],h,1,0,15,11,tint);
            for (i=0;i<16*12*d*d;i++) { hash^=b->hd[i];hash*=1099511628211ull; }
        }
    }
    hd_sprite_forget(&sprite);
    /* Recorded with the uncached premultiplied compositor. Covers cold/warm
       caches, reduction/enlargement, clipping, tint and grayscale rounding. */
    {
        static const uint64_t expected[] = {1622634468927623133ull,10195902450889624357ull,
                                           14289954145285747453ull,1436514619647450621ull};
        assert(hash == expected[d-1]);
    }
}
int main(int argc, char **argv)
{
    Bitmap *a, *b;
    int id, w, h, d, x, y;
    uint32_t *before;
    int token = 0, clone = 0;
    assert(argc == 2);
    snprintf(plat_data_dir, sizeof plat_data_dir, "%s", argv[1]);
    hd_init(0, 0);
    if (getenv("OriginalGraphics")) {
        assert(!hd_enabled);
        puts("original mode: atlas not opened");
        return 0;
    }
    assert(hd_enabled);
    d = hd_density;
    assert(hd_virtual_file("C:/Game/WONDER.DLL"));
    assert(!hd_virtual_file("RULES.TXT"));
    id = hd_file("C:\\Game\\TEST.GIF");
    assert(id);
    hd_size(id, &w, &h);
    assert(w == 4 && h == 4);
    assert(hd_indices(id)[5] == 2);
    a = bitmap(0, 0);
    b = bitmap(1, 1);
    hd_sync(a);
    /* Thin flags must retain their alpha coverage when reduced to a tiny
       map symbol, including when their pole falls between bilinear taps. */
    hd_draw(a->bits,hd_find("test/pole"),0,0,1,1,0,0,16,12,-1);
    {
        unsigned coverage = 0;
        for (y = 0; y < d; y++) for (x = 0; x < d; x++) {
            uint32_t c = a->hd[y*16*d+x];
            assert((c & 255) == ((c >> 8) & 255) && (c & 255) == ((c >> 16) & 255));
            coverage += c & 255;
        }
        assert(coverage >= 3u*d*d && coverage <= 5u*d*d);
    }
    hd_changed(a->bits,0,0,16,12);
    /* Cropping a filtered source must not borrow its resized-pixel cache. */
    {
        int crop = hd_crop(hd_find("test/pole"),2,0,2,4);
        hd_draw(a->bits,crop,0,0,1,1,0,0,16,12,-1);
        assert(a->hd[0] == 0xff000000u);
        hd_draw(a->bits,hd_find("test/pole"),0,0,1,1,0,0,16,12,-1);
        assert((a->hd[0]&255) > 0);
        hd_changed(a->bits,0,0,16,12);
    }
    hd_draw(a->bits, id, 2, 2, 8, 8, 4, 4, 9, 9, -1);
    for (y = 0; y < 12 * d; y++)
        for (x = 0; x < 16 * d; x++) {
            uint32_t c = a->hd[y * 16 * d + x];
            if (x < 4 * d || x >= 9 * d || y < 4 * d || y >= 9 * d)
                assert(c == 0xff000000u);
            /* Transparent magenta RGB must not bleed into the green silhouette. */
            assert((c & 0xff00ffu) == 0);
        }
    assert((a->hd[6 * d * 16 * d + 6 * d] & 0xff00u) > 0);
    hd_copy(b->bits, a->bits, 0, 0, 2, 2, 10, 10);
    assert(b->hd[4 * d * 16 * d + 4 * d] == a->hd[6 * d * 16 * d + 6 * d]);
    before = malloc((size_t)16 * 12 * d * d * 4);
    memcpy(before, b->hd, (size_t)16 * 12 * d * d * 4);
    hd_copy(b->bits, b->bits, 2, 2, 0, 0, 10, 10);
    assert(b->hd[6 * d * 16 * d + 6 * d] == before[4 * d * 16 * d + 4 * d]);
    free(before);
    /* A write of the same indexed color still clears previously drawn art. */
    bmp_row(a, 6)[6] = 0;
    hd_changed(a->bits, 6, 6, 1, 1);
    for (y = 6 * d; y < 7 * d; y++)
        for (x = 6 * d; x < 7 * d; x++)
            assert(a->hd[y * 16 * d + x] == 0xff000000u);
    bmp_row(b, 0)[0] = 2;
    hd_sync(b);
    assert(b->hd[0] == 0xff00ff00u);
    hd_sprite_capture(&token, a->bits, 2, 2, 8, 8, -2);
    hd_sprite_copy(&clone, &token);
    hd_sprite_forget(&token);
    assert(hd_sprite_draw(&clone, b->bits, 0, 0, 8, 8, 0, 0, 16, 12, -1));
    hd_sprite_forget(&clone);
    assert(!hd_sprite_draw(&clone, b->bits, 0, 0, 8, 8, 0, 0, 16, 12, -1));
    /* Selective faction variants retain contrasting detail and alpha. They
       follow cloned sprites; explicit silhouettes override the variant. */
    memset(a->bits, 0, 192);
    memset(b->bits, 0, 192);
    hd_changed(a->bits, 0, 0, 16, 12);
    hd_changed(b->bits, 0, 0, 16, 12);
    assert(hd_variant(id, 2) && !hd_variant(id, 3));
    hd_draw(a->bits, hd_variant(id, 2), 0, 0, 4, 4, 0, 0, 16, 12, -1);
    hd_sprite_bind(&token, "file/test");
    hd_sprite_tint(&token, 8, 2);
    hd_sprite_copy(&clone, &token);
    hd_sprite_forget(&token);
    assert(hd_sprite_draw(&clone, b->bits, 0, 0, 4, 4, 0, 0, 16, 12, -1));
    assert(!memcmp(a->hd, b->hd, (size_t)16 * 12 * d * d * 4));
    hd_changed(a->bits, 0, 0, 16, 12);
    hd_changed(b->bits, 0, 0, 16, 12);
    hd_draw(a->bits, id, 0, 0, 4, 4, 0, 0, 16, 12, 3);
    assert(hd_sprite_draw(&clone, b->bits, 0, 0, 4, 4, 0, 0, 16, 12, 3));
    assert(!memcmp(a->hd, b->hd, (size_t)16 * 12 * d * d * 4));
    hd_sprite_tint(&clone, 8, 3);
    hd_changed(b->bits, 0, 0, 16, 12);
    assert(hd_sprite_draw(&clone, b->bits, 0, 0, 4, 4, 0, 0, 16, 12, -1));
    assert(!memcmp(a->hd, b->hd, (size_t)16 * 12 * d * d * 4));
    hd_sprite_forget(&clone);
    /* Greyscale is applied before alpha composition: the colored background
       stays colored, and transparent magenta never contributes luminance. */
    memset(b->bits, 3, 192);
    hd_changed(b->bits, 0, 0, 16, 12);
    hd_sprite_bind(&token, "file/test");
    hd_sprite_greyscale(&token, 1);
    assert(hd_sprite_draw(&token,b->bits,2,2,8,8,0,0,16,12,-1));
    assert(b->hd[0] == 0xffff0000u);
    assert(b->hd[6*d*16*d+6*d] == 0xff969696u);
    hd_sprite_greyscale(&token, 0);
    assert(hd_sprite_draw(&token,b->bits,2,2,8,8,0,0,16,12,-1));
    assert(b->hd[6*d*16*d+6*d] == 0xff00ff00u);
    hd_sprite_forget(&token);
    /* Inverting a focus region twice must restore every physical pixel. */
    before = malloc((size_t)16*12*d*d*4);
    memcpy(before,b->hd,(size_t)16*12*d*d*4);
    hd_bitmap_invert(b,2,2,8,8);
    assert(b->hd[6*d*16*d+6*d] == 0xffff00ffu);
    assert(b->hd[0] == before[0]);
    hd_bitmap_invert(b,2,2,8,8);
    assert(!memcmp(before,b->hd,(size_t)16*12*d*d*4));
    free(before);
    /* Backdrop greying must survive redraws without destroying source art. */
    memset(a->bits,3,192);
    hd_changed(a->bits,0,0,16,12);
    hd_surface_grey(a->bits,1);
    hd_copy(a->bits,a->bits,2,2,0,0,10,10);
    assert(a->hd[2*d*16*d+2*d] == 0xffff0000u);
    hd_copy(b->bits,a->bits,0,0,0,0,16,12);
    assert(a->hd[0] == 0xffff0000u && b->hd[0] == 0xff4d4d4du);
    hd_surface_grey(a->bits,0);
    hd_copy(b->bits,a->bits,0,0,0,0,16,12);
    assert(b->hd[0] == 0xffff0000u);
    {
        uint32_t ramp[] = {0x80112233u, 0xff00ff00u};
        hd_grey_pixels(ramp,2,0);
        assert(ramp[0] == 0x80112233u && ramp[1] == 0xff00ff00u);
        hd_grey_pixels(ramp,2,255);
        assert(ramp[0] == 0x801f1f1fu && ramp[1] == 0xff969696u);
    }
    /* Custom and automatic capture keys, interior holes, alternate draw keys
       and key remaps must agree with the indexed row-trimming contract. */
    memset(a->bits,3,192);
    bmp_row(a,1)[1] = bmp_row(a,1)[3] = 2;
    hd_changed(a->bits,0,0,16,12);
    hd_sprite_capture(&token,a->bits,0,0,5,3,-1);
    hd_sprite_copy(&clone,&token);
    hd_sprite_forget(&token);
    memset(b->bits,0,192);
    hd_changed(b->bits,0,0,16,12);
    assert(hd_sprite_draw(&clone,b->bits,0,0,5,3,0,0,16,12,-1));
    assert(b->hd[0] == 0xff000000u);
    assert((b->hd[d*16*d+d] & 0xff00u) > 0);
    hd_changed(b->bits,0,0,16,12);
    assert(hd_sprite_draw_keyed(&clone,b->bits,0,0,5,3,0,0,16,12,-1,2));
    assert(b->hd[0] == 0xff000000u); /* discarded outside stays transparent */
    assert((b->hd[d*16*d+2*d] & 0xff0000u) > 0); /* interior key hole revealed */
    hd_sprite_tint(&clone,2,3);
    hd_changed(b->bits,0,0,16,12);
    assert(hd_sprite_draw(&clone,b->bits,0,0,5,3,0,0,16,12,-1));
    for (x = 0; x < 16*12*d*d; x++) assert(b->hd[x] == 0xff000000u);
    hd_sprite_forget(&clone);
    zoom_clips(a,b,id);
    copy_clips(a,b);
    filter_samples(b);
    for (x = 0; x < 2; x++) {
        hd_bitmap_destroy(bitmaps[x]);
        free(bitmaps[x]->bits);
        free(bitmaps[x]);
    }
    printf("HD compositor: density %d passed\n", d);
    return 0;
}
