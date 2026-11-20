/*
 * The HD graphics' pictures decoded (src/platform/hd.c): PNG (the art pack's,
 * stb_image) or WebP (OpenCiv2's own, data/native/hd-extra; libwebp's
 * decoder, src/third_party/libwebp), told apart by their first bytes.
 */
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
