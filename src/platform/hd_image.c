/*
 * The HD graphics' pictures decoded (src/platform/hd.c): PNG (the art pack's,
 * stb_image) or WebP (OpenCiv2's own, data/native/hd-extra; libwebp's
 * decoder, src/third_party/libwebp), told apart by their first bytes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hd.h"
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_MAX_DIMENSIONS 16384
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image/stb_image.h"
#include "../third_party/libwebp/src/webp/decode.h"

unsigned char *hd_decode_image(const unsigned char *data, size_t len, int *w, int *h)
{
    int ch;
    *w = *h = 0;
    if (len > 8 && !memcmp(data, "\x89PNG\r\n\x1a\n", 8) && len <= 0x7fffffff)
        return stbi_load_from_memory(data, (int)len, w, h, &ch, 4);
    if (len > 12 && !memcmp(data, "RIFF", 4) && !memcmp(data + 8, "WEBP", 4)) {
        /* (libwebp allocates with malloc: freed as stb_image's are) */
        unsigned char *rgba = WebPDecodeRGBA(data, len, w, h);
        if (rgba && (*w > 16384 || *h > 16384)) {
            WebPFree(rgba);
            rgba = NULL;
        }
        return rgba;
    }
    return NULL;
}

/* ---- animated WebP (OpenCiv2's own heralds: data/native/hd-extra/heralds.txt) ----
   The frames composited on the canvas as the WebP container says (each
   frame a rectangle of it, blended or not, its rectangle cleared after it
   or not), as libwebp's WebPAnimDecoder does, but from any frame: from the
   last key frame at or before it (one drawn without the frames before:
   libwebp's own test), or on from the frame shown last. */
#include "../third_party/libwebp/src/webp/demux.h"

struct HdAnim {
    unsigned char *data;
    size_t len;
    WebPDemuxer *dmx;
    int w, h, n;
    unsigned char *canvas;      /* RGBA, not premultiplied: frame cur */
    unsigned char *key;         /* per frame: a key frame */
    int cur;                    /* -1: none yet */
    int dispose_bg, px, py, pw, ph;     /* the frame shown: cleared after it?  its rectangle */
};

HdAnim *hd_anim_open(unsigned char *data, size_t len)
{
    WebPData wd;
    HdAnim *a;
    WebPIterator it, prev;
    int i, prev_key = 0;
    if (!data || len < 12 || memcmp(data, "RIFF", 4) || memcmp(data + 8, "WEBP", 4) || !(a = calloc(1, sizeof *a))) {
        free(data);
        return NULL;
    }
    a->data = data;
    a->len = len;
    wd.bytes = data;
    wd.size = len;
    a->dmx = WebPDemux(&wd);
    if (a->dmx) {
        a->w = (int)WebPDemuxGetI(a->dmx, WEBP_FF_CANVAS_WIDTH);
        a->h = (int)WebPDemuxGetI(a->dmx, WEBP_FF_CANVAS_HEIGHT);
        a->n = (int)WebPDemuxGetI(a->dmx, WEBP_FF_FRAME_COUNT);
    }
    if (!a->dmx || a->w <= 0 || a->h <= 0 || a->w > 8192 || a->h > 8192 || a->n <= 0 ||
        !(a->canvas = calloc((size_t)a->w * a->h, 4)) || !(a->key = calloc((size_t)a->n, 1))) {
        hd_anim_close(a);
        return NULL;
    }
    memset(&prev, 0, sizeof prev);
    for (i = 0; i < a->n; i++) {
        if (!WebPDemuxGetFrame(a->dmx, i + 1, &it)) {
            hd_anim_close(a);
            return NULL;
        }
        /* (anim_decode.c's IsKeyFrame) */
        if (i == 0 || ((!it.has_alpha || it.blend_method == WEBP_MUX_NO_BLEND) && it.width == a->w &&
                       it.height == a->h))
            a->key[i] = 1;
        else
            a->key[i] = prev.dispose_method == WEBP_MUX_DISPOSE_BACKGROUND &&
                        ((prev.width == a->w && prev.height == a->h) || prev_key);
        prev_key = a->key[i];
        WebPDemuxReleaseIterator(&prev);
        prev = it;
    }
    WebPDemuxReleaseIterator(&prev);
    a->cur = -1;
    return a;
}

void hd_anim_close(HdAnim *a)
{
    if (!a)
        return;
    WebPDemuxDelete(a->dmx);
    free(a->canvas);
    free(a->key);
    free(a->data);
    free(a);
}

int hd_anim_frames(const HdAnim *a) { return a ? a->n : 0; }
void hd_anim_size(const HdAnim *a, int *w, int *h) { *w = a ? a->w : 0; *h = a ? a->h : 0; }

static int draw_anim_frame(HdAnim *a, int i)
{
    WebPIterator it;
    unsigned char *px;
    int fw, fh, x, y;
    if (!WebPDemuxGetFrame(a->dmx, i + 1, &it))
        return 0;
    if (a->key[i])
        memset(a->canvas, 0, (size_t)a->w * a->h * 4);
    else if (a->dispose_bg)
        for (y = a->py; y < a->py + a->ph && y < a->h; y++)
            memset(a->canvas + ((size_t)y * a->w + a->px) * 4, 0,
                   (size_t)(a->px + a->pw > a->w ? a->w - a->px : a->pw) * 4);
    px = WebPDecodeRGBA(it.fragment.bytes, it.fragment.size, &fw, &fh);
    if (!px) {
        WebPDemuxReleaseIterator(&it);
        return 0;
    }
    for (y = 0; y < fh && it.y_offset + y < a->h; y++) {
        const unsigned char *s = px + (size_t)y * fw * 4;
        unsigned char *d = a->canvas + ((size_t)(it.y_offset + y) * a->w + it.x_offset) * 4;
        int n = it.x_offset + fw > a->w ? a->w - it.x_offset : fw;
        if (it.blend_method == WEBP_MUX_NO_BLEND || !it.has_alpha) {
            memcpy(d, s, (size_t)n * 4);
            continue;
        }
        for (x = 0; x < n; x++, s += 4, d += 4) {
            /* (anim_decode.c's BlendPixelNonPremult) */
            unsigned sa = s[3], da, ba, scale;
            int c;
            if (sa == 255) {
                memcpy(d, s, 4);
                continue;
            }
            if (!sa)
                continue;
            da = (d[3] * (256 - sa)) >> 8;
            ba = sa + da;
            scale = (1u << 24) / ba;
            for (c = 0; c < 3; c++)
                d[c] = (unsigned char)(((s[c] * sa + d[c] * da) * scale) >> 24);
            d[3] = (unsigned char)ba;
        }
    }
    WebPFree(px);
    a->dispose_bg = it.dispose_method == WEBP_MUX_DISPOSE_BACKGROUND;
    a->px = it.x_offset;
    a->py = it.y_offset;
    a->pw = it.width;
    a->ph = it.height;
    WebPDemuxReleaseIterator(&it);
    a->cur = i;
    return 1;
}

const unsigned char *hd_anim_frame(HdAnim *a, int i)
{
    int k, from;
    if (!a || i < 0 || i >= a->n)
        return NULL;
    if (i == a->cur)
        return a->canvas;
    for (k = i; k > 0 && !a->key[k]; k--)
        ;
    from = a->cur >= k && a->cur < i ? a->cur + 1 : k;
    for (; from <= i; from++)
        if (!draw_anim_frame(a, from)) {
            a->cur = -1;
            return NULL;
        }
    return a->canvas;
}

/* A line of heralds.txt: CODE X Y W H FPS, five segments' first and last
   frames, FILE. */
int hd_herald_parse(const char *line, HdHerald *h)
{
    int s[10], i, n = 0;
    memset(h, 0, sizeof *h);
    if (sscanf(line, " %3[A-Za-z] %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %63s%n", h->code, &h->x, &h->y, &h->w,
               &h->h, &h->fps, &s[0], &s[1], &s[2], &s[3], &s[4], &s[5], &s[6], &s[7], &s[8], &s[9], h->file, &n) != 17 ||
        strlen(h->code) != 3 || h->w <= 0 || h->h <= 0 || h->w > 640 || h->h > 480 || h->fps <= 0 || h->fps > 60)
        return 0;
    for (i = 0; i < 3; i++)
        h->code[i] = (char)(h->code[i] >= 'a' && h->code[i] <= 'z' ? h->code[i] - 32 : h->code[i]);
    for (i = 0; i < 5; i++) {
        h->seg[i][0] = s[2 * i];
        h->seg[i][1] = s[2 * i + 1];
        if (s[2 * i] < 0 || s[2 * i + 1] < s[2 * i])
            return 0;
    }
    return 1;
}
