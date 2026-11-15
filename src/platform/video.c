/*
 * Video for Windows, as much as the game uses: AVI files (AVIFile*,
 * AVIStream*) and an installable decompressor for Intel Indeo 4.1 (ICLocate,
 * ICSendMessage, ICClose) over src/third_party/indeo4.
 *
 * The game asks the decompressor for its palette (entries 10..245 become the
 * window's), hands back the result and has 8-bit frames drawn with it: here
 * Intel's palette, matched with an ordered dither.  (Intel's own dither is
 * not reproduced.)  The herald videos are transparent around the figure:
 * the decoder's mask says where, and there the game's background stays.
 */
#include "plat.h"
#include "plat_files.h"
#include "../third_party/indeo4/iv41.h"
#include "hd.h"

#ifndef DRV_USER
#define DRV_USER 0x4000
#endif
#ifndef FIND_NEXT
#define FIND_NEXT 0x0001
#define FIND_PREV 0x0004
#define FIND_KEY 0x0010
#endif

#define FCC(a, b, c, d) ((DWORD)(a) | (DWORD)(b) << 8 | (DWORD)(c) << 16 | (DWORD)(d) << 24)

static DWORD rd32(const unsigned char *p) { return (DWORD)(p[0] | p[1] << 8 | p[2] << 16 | (DWORD)p[3] << 24); }
static WORD rd16(const unsigned char *p) { return (WORD)(p[0] | p[1] << 8); }

/* ---- AVI files ---- */

typedef struct { DWORD off, size; int key; } Sample;

typedef struct {
    unsigned char strh[56];
    unsigned char *fmt;
    LONG fmt_size;
    Sample *samples;            /* the stream's chunks */
    int n, cap;
    DWORD bytes;                /* all chunks' data */
} Stream;

/* Video streams count chunks, one a frame; streams of fixed-size samples
 * (audio: the format's block) count samples, which run across chunks.
 * Audio counts blocks even where the header gives no sample size (the
 * opening movie, its chunks interleaved in 'rec ' lists), as AVIFile does. */
static DWORD sample_size(const Stream *s)
{
    DWORD ss = rd32(s->strh + 44);
    if ((ss || rd32(s->strh) == FCC('a', 'u', 'd', 's')) && s->fmt && s->fmt_size >= 14 && rd16(s->fmt + 12))
        ss = rd16(s->fmt + 12);         /* WAVEFORMAT nBlockAlign */
    return ss;
}

static LONG stream_length(const Stream *s)
{
    DWORD ss = sample_size(s);
    return ss ? (LONG)(s->bytes / ss) : s->n;
}

#define MAX_STREAMS 4
typedef struct {
    unsigned char *data;
    long size;
    Stream streams[MAX_STREAMS];
    int nstreams;
} AviFile;

typedef struct { AviFile *file; int index; } AviStreamRef;

static void add_sample(Stream *s, DWORD off, DWORD size)
{
    if (s->n == s->cap) {
        s->cap = s->cap ? 2 * s->cap : 256;
        s->samples = realloc(s->samples, sizeof *s->samples * (size_t)s->cap);
    }
    s->samples[s->n].off = off;
    s->samples[s->n].size = size;
    s->samples[s->n].key = s->n == 0;
    s->n++;
    s->bytes += size;
}

/* The chunks of a list from p to end: headers, stream lists, the movi data. */
static void parse_list(AviFile *a, long p, long end, int in_movi, long *movi)
{
    while (p + 8 <= end) {
        DWORD id = rd32(a->data + p), len = rd32(a->data + p + 4);
        long body = p + 8;
        if (body + (long)len > end)
            len = (DWORD)(end - body);
        if (id == FCC('L', 'I', 'S', 'T') && len >= 4) {
            DWORD type = rd32(a->data + body);
            if (type == FCC('m', 'o', 'v', 'i'))
                *movi = body;
            if (type == FCC('s', 't', 'r', 'l') && a->nstreams < MAX_STREAMS)
                a->nstreams++;
            parse_list(a, body + 4, body + (long)len, in_movi || type == FCC('m', 'o', 'v', 'i'), movi);
        } else if (id == FCC('s', 't', 'r', 'h') && a->nstreams) {
            memcpy(a->streams[a->nstreams - 1].strh, a->data + body, len < 56 ? len : 56);
        } else if (id == FCC('s', 't', 'r', 'f') && a->nstreams) {
            Stream *s = &a->streams[a->nstreams - 1];
            s->fmt = malloc(len ? len : 1);
            memcpy(s->fmt, a->data + body, len);
            s->fmt_size = (LONG)len;
        } else if (id == FCC('i', 'd', 'x', '1') && *movi) {
            /* key frames: entries in file order, per stream; offsets from movi */
            int seen[MAX_STREAMS] = {0};
            DWORD i;
            for (i = 0; i + 16 <= len; i += 16) {
                const unsigned char *e = a->data + body + i;
                int st = (e[0] - '0') * 10 + (e[1] - '0');
                if (st < 0 || st >= a->nstreams || seen[st] >= a->streams[st].n)
                    continue;
                a->streams[st].samples[seen[st]++].key = (rd32(e + 4) & AVIIF_KEYFRAME) != 0;
            }
        } else if (in_movi && id != FCC('J', 'U', 'N', 'K')) {
            int st = (int)((id & 0xff) - '0') * 10 + (int)(((id >> 8) & 0xff) - '0');
            if (st >= 0 && st < a->nstreams)
                add_sample(&a->streams[st], (DWORD)body, len);
        }
        p = body + (long)len + (long)(len & 1);
    }
}

void WINAPI AVIFileInit(void) { API_TRACE(); }
void WINAPI AVIFileExit(void) { API_TRACE(); }

HRESULT WINAPI AVIFileOpenA(PAVIFILE *pf, LPCSTR name, UINT mode, LPCLSID handler)
{ API_TRACE();
    char host[1024];
    FILE *f;
    AviFile *a;
    long movi = 0;
    (void)mode; (void)handler;
    *pf = 0;
    plat_host_path(name, host, sizeof host, 0);
    f = fopen(host, "rb");
    if (!f) {
        plat_log("AVIFileOpen(%s): cannot open %s\n", name, host);
        return AVIERR_FILEOPEN;
    }
    a = calloc(1, sizeof *a);
    fseek(f, 0, SEEK_END);
    a->size = ftell(f);
    fseek(f, 0, SEEK_SET);
    a->data = malloc((size_t)a->size + 1);
    if (fread(a->data, 1, (size_t)a->size, f) != (size_t)a->size || a->size < 12 ||
        memcmp(a->data, "RIFF", 4) || memcmp(a->data + 8, "AVI ", 4)) {
        plat_log("AVIFileOpen(%s): not an AVI file\n", name);
        fclose(f);
        free(a->data);
        free(a);
        return AVIERR_FILEOPEN;
    }
    fclose(f);
    parse_list(a, 12, a->size, 0, &movi);
    plat_log("AVIFileOpen(%s): %d streams\n", name, a->nstreams);
    *pf = (PAVIFILE)obj_new(OBJ_AVIFILE, a);
    return AVIERR_OK;
}

ULONG WINAPI AVIFileRelease(PAVIFILE pf)
{ API_TRACE();
    AviFile *a = obj_get((Handle)pf, OBJ_AVIFILE);
    int i;
    if (!a)
        return 0;
    for (i = 0; i < a->nstreams; i++) {
        free(a->streams[i].fmt);
        free(a->streams[i].samples);
    }
    free(a->data);
    free(a);
    obj_free((Handle)pf);
    return 0;
}

HRESULT WINAPI AVIFileGetStream(PAVIFILE pf, PAVISTREAM *ps, DWORD type, LONG n)
{ API_TRACE();
    AviFile *a = obj_get((Handle)pf, OBJ_AVIFILE);
    int i;
    *ps = 0;
    if (!a)
        return AVIERR_NODATA;
    for (i = 0; i < a->nstreams; i++)
        if (!type || rd32(a->streams[i].strh) == type) {
            if (n-- == 0) {
                AviStreamRef *r = calloc(1, sizeof *r);
                r->file = a;
                r->index = i;
                *ps = (PAVISTREAM)obj_new(OBJ_AVISTREAM, r);
                return AVIERR_OK;
            }
        }
    return AVIERR_NODATA;
}

static Stream *stream_of(PAVISTREAM ps)
{
    AviStreamRef *r = obj_get((Handle)ps, OBJ_AVISTREAM);
    return r ? &r->file->streams[r->index] : NULL;
}

ULONG WINAPI AVIStreamRelease(PAVISTREAM ps)
{ API_TRACE();
    AviStreamRef *r = obj_get((Handle)ps, OBJ_AVISTREAM);
    if (r) {
        free(r);
        obj_free((Handle)ps);
    }
    return 0;
}

HRESULT WINAPI AVIStreamInfoA(PAVISTREAM ps, LPAVISTREAMINFOA i, LONG n)
{ API_TRACE();
    Stream *s = stream_of(ps);
    const unsigned char *h;
    AVISTREAMINFOA info;
    if (!s)
        return AVIERR_NODATA;
    h = s->strh;
    memset(&info, 0, sizeof info);
    info.fccType = rd32(h);
    info.fccHandler = rd32(h + 4);
    info.dwFlags = rd32(h + 8);
    info.wPriority = rd16(h + 12);
    info.wLanguage = rd16(h + 14);
    info.dwInitialFrames = rd32(h + 16);
    info.dwScale = rd32(h + 20);
    info.dwRate = rd32(h + 24);
    info.dwStart = rd32(h + 28);
    info.dwLength = (DWORD)stream_length(s);
    info.dwSuggestedBufferSize = rd32(h + 36);
    info.dwQuality = rd32(h + 40);
    info.dwSampleSize = rd32(h + 44);
    info.rcFrame.left = (short)rd16(h + 48);
    info.rcFrame.top = (short)rd16(h + 50);
    info.rcFrame.right = (short)rd16(h + 52);
    info.rcFrame.bottom = (short)rd16(h + 54);
    memcpy(i, &info, (size_t)n < sizeof info ? (size_t)n : sizeof info);
    return AVIERR_OK;
}

LONG WINAPI AVIStreamStart(PAVISTREAM ps) { API_TRACE(); Stream *s = stream_of(ps); return s ? (LONG)rd32(s->strh + 28) : 0; }
LONG WINAPI AVIStreamLength(PAVISTREAM ps) { API_TRACE(); Stream *s = stream_of(ps); return s ? stream_length(s) : 0; }

HRESULT WINAPI AVIStreamReadFormat(PAVISTREAM ps, LONG pos, LPVOID fmt, LONG *n)
{ API_TRACE();
    Stream *s = stream_of(ps);
    (void)pos;
    if (!s || !s->fmt) {
        *n = 0;
        return AVIERR_NODATA;
    }
    if (fmt)
        memcpy(fmt, s->fmt, (size_t)(*n < s->fmt_size ? *n : s->fmt_size));
    *n = s->fmt_size;
    return AVIERR_OK;
}

HRESULT WINAPI AVIStreamRead(PAVISTREAM ps, LONG start, LONG n, LPVOID buf, LONG cb, LONG *bytes, LONG *samples)
{ API_TRACE();
    AviStreamRef *r = obj_get((Handle)ps, OBJ_AVISTREAM);
    Stream *s = r ? &r->file->streams[r->index] : NULL;
    Sample *x;
    DWORD ss;
    if (bytes) *bytes = 0;
    if (samples) *samples = 0;
    if (!s)
        return AVIERR_NODATA;
    if ((ss = sample_size(s)) != 0) {
        /* samples start .. start + n - 1, as many as fit in cb bytes */
        DWORD from = (DWORD)start * ss, want, got = 0, pos = 0;
        int i;
        if (start < 0 || from >= s->bytes)
            return AVIERR_NODATA;
        want = (DWORD)(n > 0 ? n : 1) * ss;
        if (want > s->bytes - from)
            want = s->bytes - from;
        if (buf) {
            if ((DWORD)cb < ss)
                return AVIERR_BUFFERTOOSMALL;
            if (want > (DWORD)cb)
                want = (DWORD)cb / ss * ss;
            for (i = 0; i < s->n && got < want; pos += s->samples[i].size, i++) {
                DWORD a, k;
                if (pos + s->samples[i].size <= from + got)
                    continue;
                a = from + got - pos;
                k = s->samples[i].size - a;
                if (k > want - got)
                    k = want - got;
                memcpy((unsigned char *)buf + got, r->file->data + s->samples[i].off + a, k);
                got += k;
            }
            want = got;
        }
        if (buf && getenv("CIV2_LOG_VIDEO"))
            plat_log("video: read %lu audio bytes at sample %ld\n", (unsigned long)want, (long)start);
        if (bytes) *bytes = (LONG)want;
        if (samples) *samples = (LONG)(want / ss);
        return AVIERR_OK;
    }
    if (start < 0 || start >= s->n)
        return AVIERR_NODATA;
    x = &s->samples[start];
    if (getenv("CIV2_LOG_VIDEO"))
        plat_log("video: read frame %ld of %d at %lu ms\n", (long)start, s->n, (unsigned long)GetTickCount());
    if (buf) {
        if ((DWORD)cb < x->size)
            return AVIERR_BUFFERTOOSMALL;
        memcpy(buf, r->file->data + x->off, x->size);
    }
    if (bytes) *bytes = (LONG)x->size;
    if (samples) *samples = 1;
    return AVIERR_OK;
}

LONG WINAPI AVIStreamSampleToTime(PAVISTREAM ps, LONG n)
{ API_TRACE();
    Stream *s = stream_of(ps);
    DWORD scale = s ? rd32(s->strh + 20) : 0, rate = s ? rd32(s->strh + 24) : 0;
    return rate ? (LONG)((int64_t)n * scale * 1000 / rate) : 0;
}

LONG WINAPI AVIStreamTimeToSample(PAVISTREAM ps, LONG t)
{ API_TRACE();
    Stream *s = stream_of(ps);
    DWORD scale = s ? rd32(s->strh + 20) : 0, rate = s ? rd32(s->strh + 24) : 0;
    return scale ? (LONG)((int64_t)t * rate / ((int64_t)scale * 1000)) : 0;
}

LONG WINAPI AVIStreamFindSample(PAVISTREAM ps, LONG pos, LONG flags)
{ API_TRACE();
    Stream *s = stream_of(ps);
    int step = (flags & FIND_NEXT) ? 1 : -1;
    if (!s || !s->n)
        return -1;
    if (sample_size(s))         /* every sample of an audio stream is a key */
        return pos < 0 ? 0 : pos >= stream_length(s) ? stream_length(s) - 1 : pos;
    if (pos < 0) pos = 0;
    if (pos >= s->n) pos = s->n - 1;
    if (!(flags & FIND_KEY))
        return pos;
    for (; pos >= 0 && pos < s->n; pos += step)
        if (s->samples[pos].key)
            return pos;
    return -1;
}

/* ---- the IV41 decompressor ---- */

typedef struct {
    Iv41 *dec;
    int width, height;
    RGBQUAD pal[256];
    unsigned char lut[32768];   /* RGB 5:5:5 -> palette index */
    int lut_ok;
    int32_t *view;              /* where to report the area drawn: x, y, width, height */
} Codec;

/* The palette offered: the one Intel's decompressor (ir41_32.dll, from the
 * Indeo installer on the game's disc) gives an 8-bit decompression --
 * Windows' 20 static colours around 234 of its own (tools/probe/ivpalprobe.c
 * prints it).  The game puts entries 10..245 into its palette and draws the
 * leader's room with them, so they decide how the room looks. */
static void video_palette(RGBQUAD *pal)
{
    static const unsigned char intel[256][3] = {
        {0, 0, 0}, {128, 0, 0}, {0, 128, 0}, {128, 128, 0}, {0, 0, 128}, {128, 0, 128}, {0, 128, 128}, {192, 192, 192},
        {192, 220, 192}, {166, 202, 240}, {25, 25, 25}, {43, 43, 43}, {60, 60, 60}, {77, 77, 77}, {95, 95, 95}, {112, 112, 112},
        {130, 130, 130}, {147, 147, 147}, {165, 165, 165}, {182, 182, 182}, {200, 200, 200}, {217, 217, 217}, {235, 235, 235}, {0, 51, 25},
        {0, 69, 43}, {9, 86, 60}, {26, 104, 77}, {44, 121, 95}, {61, 138, 112}, {79, 156, 130}, {96, 173, 147}, {114, 191, 165},
        {131, 208, 182}, {149, 226, 200}, {166, 243, 217}, {184, 255, 235}, {25, 38, 0}, {43, 55, 0}, {60, 73, 0}, {78, 90, 13},
        {95, 107, 30}, {112, 125, 48}, {130, 142, 65}, {147, 160, 83}, {165, 177, 100}, {182, 195, 118}, {200, 212, 135}, {217, 230, 153},
        {235, 247, 170}, {76, 0, 25}, {94, 17, 43}, {111, 34, 60}, {129, 51, 78}, {146, 69, 95}, {163, 86, 112}, {181, 104, 130},
        {198, 121, 147}, {216, 139, 165}, {233, 156, 182}, {251, 174, 200}, {255, 191, 217}, {255, 209, 235}, {25, 13, 90}, {43, 30, 107},
        {60, 48, 125}, {77, 65, 142}, {95, 82, 159}, {112, 100, 177}, {130, 117, 194}, {147, 135, 212}, {165, 152, 229}, {182, 170, 247},
        {200, 187, 255}, {217, 205, 255}, {235, 222, 255}, {0, 64, 0}, {0, 81, 0}, {9, 99, 0}, {26, 116, 13}, {44, 133, 30},
        {61, 151, 48}, {79, 168, 65}, {96, 186, 83}, {114, 203, 100}, {131, 221, 118}, {149, 238, 135}, {166, 255, 153}, {184, 255, 170},
        {0, 39, 90}, {0, 56, 107}, {9, 74, 125}, {26, 91, 142}, {44, 108, 159}, {61, 126, 177}, {79, 143, 194}, {96, 161, 212},
        {114, 178, 229}, {131, 196, 247}, {149, 213, 255}, {166, 231, 255}, {184, 248, 255}, {76, 0, 90}, {94, 4, 107}, {111, 22, 125},
        {129, 39, 142}, {146, 56, 160}, {163, 74, 177}, {181, 91, 194}, {198, 109, 212}, {216, 126, 229}, {233, 144, 247}, {251, 161, 255},
        {255, 179, 255}, {255, 196, 255}, {76, 12, 0}, {94, 29, 0}, {111, 47, 0}, {129, 64, 13}, {146, 81, 30}, {164, 99, 48},
        {181, 116, 65}, {198, 134, 83}, {216, 151, 100}, {233, 169, 118}, {251, 186, 135}, {255, 204, 153}, {255, 221, 170}, {25, 50, 0},
        {43, 68, 0}, {60, 85, 0}, {78, 103, 0}, {95, 120, 0}, {112, 137, 0}, {130, 155, 1}, {147, 172, 18}, {165, 190, 36},
        {182, 207, 53}, {200, 225, 71}, {217, 242, 88}, {235, 255, 106}, {127, 0, 25}, {145, 0, 43}, {162, 8, 60}, {180, 25, 78},
        {197, 43, 95}, {215, 60, 112}, {232, 78, 130}, {249, 95, 147}, {255, 113, 165}, {255, 130, 182}, {255, 148, 200}, {255, 165, 217},
        {255, 183, 235}, {0, 77, 25}, {0, 95, 43}, {0, 112, 60}, {0, 130, 77}, {0, 147, 95}, {10, 164, 112}, {28, 182, 130},
        {45, 199, 147}, {63, 217, 165}, {80, 234, 182}, {98, 252, 200}, {115, 255, 217}, {132, 255, 235}, {25, 0, 154}, {43, 18, 172},
        {60, 35, 189}, {77, 52, 207}, {95, 70, 224}, {112, 87, 241}, {130, 105, 255}, {147, 122, 255}, {165, 140, 255}, {182, 157, 255},
        {200, 175, 255}, {217, 192, 255}, {235, 210, 255}, {76, 24, 0}, {94, 42, 0}, {111, 59, 0}, {129, 76, 0}, {146, 94, 0},
        {164, 111, 0}, {181, 129, 1}, {198, 146, 18}, {216, 164, 36}, {233, 181, 53}, {251, 199, 71}, {255, 216, 88}, {255, 234, 106},
        {127, 0, 0}, {145, 3, 0}, {162, 21, 0}, {180, 38, 13}, {197, 55, 30}, {215, 73, 48}, {232, 90, 65}, {250, 108, 83},
        {255, 125, 100}, {255, 143, 118}, {255, 160, 135}, {255, 178, 153}, {255, 195, 170}, {0, 76, 0}, {0, 94, 0}, {9, 111, 0},
        {26, 129, 0}, {44, 146, 0}, {61, 163, 0}, {79, 181, 1}, {96, 198, 18}, {114, 216, 36}, {131, 233, 53}, {149, 251, 71},
        {166, 255, 88}, {184, 255, 106}, {0, 65, 90}, {0, 82, 107}, {0, 100, 125}, {0, 117, 142}, {0, 134, 159}, {10, 152, 177},
        {28, 169, 194}, {45, 187, 212}, {63, 204, 229}, {80, 222, 247}, {98, 239, 255}, {115, 255, 255}, {132, 255, 255}, {0, 26, 154},
        {0, 44, 172}, {9, 61, 189}, {26, 78, 207}, {44, 96, 224}, {61, 113, 241}, {79, 131, 255}, {96, 148, 255}, {114, 166, 255},
        {131, 183, 255}, {149, 201, 255}, {166, 218, 255}, {183, 236, 255}, {0, 0, 0}, {0, 0, 0}, {255, 251, 240}, {160, 160, 164},
        {128, 128, 128}, {255, 0, 0}, {0, 255, 0}, {255, 255, 0}, {0, 0, 255}, {255, 0, 255}, {0, 255, 255}, {255, 255, 255},
    };
    int i;
    for (i = 0; i < 256; i++) {
        pal[i].rgbRed = intel[i][0];
        pal[i].rgbGreen = intel[i][1];
        pal[i].rgbBlue = intel[i][2];
        pal[i].rgbReserved = 0;
    }
}

static void build_lut(Codec *c)
{
    int i, best, bd, d, k;
    for (i = 0; i < 32768; i++) {
        int r = (i >> 10 & 31) * 255 / 31, g = (i >> 5 & 31) * 255 / 31, b = (i & 31) * 255 / 31;
        best = 10;
        bd = 1 << 30;
        for (k = 10; k < 244; k++) {       /* (244, 245: unused black) */
            int dr = r - c->pal[k].rgbRed, dg = g - c->pal[k].rgbGreen, db = b - c->pal[k].rgbBlue;
            d = 3 * dr * dr + 4 * dg * dg + 2 * db * db;
            if (d < bd) {
                bd = d;
                best = k;
            }
        }
        c->lut[i] = (unsigned char)best;
    }
    c->lut_ok = 1;
}

static int clamp8(int v) { return v < 0 ? 0 : v > 255 ? 255 : v; }

/* Draw the decoded frame into the destination DIB (8, 16, 24 or 32 bits;
 * bottom-up unless the height is negative), scaling the source rectangle. */
static void draw_frame(Codec *c, const Iv41Frame *fr, const ICDECOMPRESSEX *d)
{
    static const int bayer[4][4] = {{-24, 8, -16, 16}, {20, -8, 28, 0}, {-12, 20, -20, 12}, {24, 0, 16, -4}};
    BITMAPINFOHEADER *bi = d->lpbiDst;
    int bpp = bi->biBitCount, w = bi->biWidth, h = bi->biHeight < 0 ? -bi->biHeight : bi->biHeight;
    int stride = ((w * bpp + 31) / 32) * 4, top_down = bi->biHeight < 0, x, y, run, run_end = 0;
    /* The destination is a DIB of ours: its real size, which can be smaller
       than the header says (a window's client area clamped to its maximum). */
    Bitmap *bm = bmp_of_bits(d->lpDst);
    if (getenv("CIV2_LOG_VIDEO"))
        plat_log("video: dst bitmap %dx%d/%d\n", bm ? bm->w : -1, bm ? bm->h : -1, bm ? bm->bpp : -1);
    if (bm && bm->bpp == bpp) {
        w = bm->w;
        h = bm->h;
        stride = bm->stride;
        top_down = !bm->bottom_up;
    } else
        bm = NULL;
    int dxs = d->dxSrc ? d->dxSrc : fr->width, dys = d->dySrc ? d->dySrc : fr->height;
    int dxd = d->dxDst ? d->dxDst : dxs, dyd = d->dyDst ? d->dyDst : dys;
    unsigned char *base = d->lpDst;
    /* CIV2_TEST_VIDEO_KEY=1 (tests): a video without transparency (LOSER.AVI
       standing for a herald) drawn as if its black were transparent */
    static int test_key = -1;
    if (test_key < 0)
        test_key = getenv("CIV2_TEST_VIDEO_KEY") != NULL;
    if (bpp == 8 && !c->lut_ok)
        build_lut(c);
    for (y = 0; y < dyd; y++) {
        int ty = d->yDst + y, sy = d->ySrc + y * dys / dyd;
        unsigned char *row;
        if (ty < 0 || ty >= h || sy < 0 || sy >= fr->height)
            continue;
        row = base + (size_t)(top_down ? ty : h - 1 - ty) * stride;
        run = -1;
        for (x = 0; x < dxd; x++) {
            int tx = d->xDst + x, sx = d->xSrc + x * dxs / dxd;
            int Y, U, V, r, g, b;
            /* (HD planes: the pixels written shown as they are, changed or
               not, in runs; those left transparent keep what is there) */
            if (run >= 0 && (tx < 0 || tx >= w || sx < 0 || sx >= fr->width || tx != run_end)) {
                if (bm)
                    hd_redrawn(bm, run, ty, run_end - run, 1);
                run = -1;
            }
            if (tx < 0 || tx >= w || sx < 0 || sx >= fr->width)
                continue;
            Y = fr->y[sy * fr->y_stride + sx];
            U = fr->u[(sy >> 2) * fr->uv_stride + (sx >> 2)] - 128;
            V = fr->v[(sy >> 2) * fr->uv_stride + (sx >> 2)] - 128;
            /* Transparent areas (the heralds: a figure over the throne room)
               leave the background the game put there; they are encoded
               black, which stands for them should a frame's mask be missing. */
            if (fr->mask ? fr->mask[sy * fr->width + sx] :
                (fr->has_transparency || test_key) && Y <= 16 && U >= -2 && U <= 2 && V >= -2 && V <= 2) {
                if (run >= 0 && bm)
                    hd_redrawn(bm, run, ty, run_end - run, 1);
                run = -1;
                continue;
            }
            if (run < 0)
                run = run_end = tx;
            run_end = tx + 1;
            Y = 298 * (Y - 16);
            r = clamp8((Y + 409 * V + 128) >> 8);
            g = clamp8((Y - 100 * U - 208 * V + 128) >> 8);
            b = clamp8((Y + 516 * U + 128) >> 8);
            switch (bpp) {
            case 8: {
                int e = bayer[ty & 3][tx & 3];
                r = clamp8(r + e); g = clamp8(g + e); b = clamp8(b + e);
                row[tx] = c->lut[(r >> 3) << 10 | (g >> 3) << 5 | (b >> 3)];
                break;
            }
            case 16: {
                WORD p = (WORD)((r >> 3) << 10 | (g >> 3) << 5 | (b >> 3));
                row[2 * tx] = (unsigned char)p;
                row[2 * tx + 1] = (unsigned char)(p >> 8);
                break;
            }
            case 24:
                row[3 * tx] = (unsigned char)b; row[3 * tx + 1] = (unsigned char)g; row[3 * tx + 2] = (unsigned char)r;
                break;
            case 32:
                row[4 * tx] = (unsigned char)b; row[4 * tx + 1] = (unsigned char)g; row[4 * tx + 2] = (unsigned char)r;
                row[4 * tx + 3] = 0;
                break;
            }
        }
        if (run >= 0 && bm)
            hd_redrawn(bm, run, ty, run_end - run, 1);
    }
}

HIC WINAPI ICLocate(DWORD type, DWORD handler, LPBITMAPINFOHEADER in, LPBITMAPINFOHEADER out, WORD mode)
{ API_TRACE();
    Codec *c;
    (void)type; (void)out; (void)mode;
    if (!(in && (in->biCompression == FCC('I', 'V', '4', '1') || in->biCompression == FCC('i', 'v', '4', '1'))) &&
        handler != FCC('I', 'V', '4', '1') && handler != FCC('i', 'v', '4', '1')) {
        DWORD k = in ? in->biCompression : handler;
        plat_log("ICLocate: no decompressor for %c%c%c%c (only Indeo 4.1, IV41)\n", (char)k, (char)(k >> 8),
                 (char)(k >> 16), (char)(k >> 24));
        return 0;
    }
    c = calloc(1, sizeof *c);
    video_palette(c->pal);
    if (in) {
        c->width = in->biWidth;
        c->height = in->biHeight < 0 ? -in->biHeight : in->biHeight;
    }
    return (HIC)(uintptr_t)obj_new(OBJ_CODEC, c);
}

LRESULT WINAPI ICSendMessage(HIC h, UINT msg, DWORD_PTR a, DWORD_PTR b)
{ API_TRACE();
    Codec *c = obj_get((Handle)(uintptr_t)h, OBJ_CODEC);
    if (!c)
        return ICERR_BADHANDLE;
    switch (msg) {
    case ICM_DECOMPRESS_GET_PALETTE: {
        BITMAPINFOHEADER *bo = (BITMAPINFOHEADER *)b;
        if (bo) {
            memcpy((unsigned char *)bo + bo->biSize, c->pal, sizeof c->pal);
            bo->biClrUsed = 256;
        }
        return ICERR_OK;
    }
    case ICM_DECOMPRESS_SET_PALETTE: {
        BITMAPINFOHEADER *bo = (BITMAPINFOHEADER *)a;
        if (bo && bo->biBitCount == 8) {
            memcpy(c->pal, (unsigned char *)bo + bo->biSize, sizeof c->pal);
            c->lut_ok = 0;
        }
        return ICERR_OK;
    }
    case ICM_DECOMPRESSEX_QUERY:
    case ICM_DECOMPRESS_QUERY: {
        const ICDECOMPRESSEX *d = (const ICDECOMPRESSEX *)a;
        if (msg == ICM_DECOMPRESSEX_QUERY && d && d->lpbiDst) {
            int bpp = d->lpbiDst->biBitCount;
            if (d->lpbiDst->biCompression != BI_RGB || (bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32))
                return ICERR_BADFORMAT;
        }
        return ICERR_OK;
    }
    case ICM_DECOMPRESSEX_BEGIN: {
        const ICDECOMPRESSEX *d = (const ICDECOMPRESSEX *)a;
        int w = d && d->lpbiSrc ? d->lpbiSrc->biWidth : c->width;
        int hh = d && d->lpbiSrc ? (d->lpbiSrc->biHeight < 0 ? -d->lpbiSrc->biHeight : d->lpbiSrc->biHeight) : c->height;
        if (!c->dec || w != c->width || hh != c->height) {
            iv41_close(c->dec);
            c->width = w;
            c->height = hh;
            c->dec = iv41_open(w, hh);
        }
        if (d && d->lpbiDst && d->lpbiDst->biBitCount == 8 && !c->lut_ok)
            memcpy(c->pal, (unsigned char *)d->lpbiDst + d->lpbiDst->biSize, sizeof c->pal);
        return c->dec ? ICERR_OK : ICERR_ERROR;
    }
    case ICM_DECOMPRESSEX: {
        const ICDECOMPRESSEX *d = (const ICDECOMPRESSEX *)a;
        Iv41Frame fr;
        if (!c->dec || !d || !d->lpbiSrc || !d->lpSrc)
            return ICERR_ERROR;
        if (!iv41_decode(c->dec, d->lpSrc, (int)d->lpbiSrc->biSizeImage, &fr))
            return ICERR_OK;            /* a frame that only updates the decoder's state */
        if (getenv("CIV2_LOG_VIDEO"))
            plat_log("video: frame %dx%d -> dst %ldx%ld/%d at %p (%d,%d %dx%d) flags %lx\n", fr.width, fr.height,
                     (long)d->lpbiDst->biWidth, (long)d->lpbiDst->biHeight, d->lpbiDst->biBitCount, d->lpDst, d->xDst,
                     d->yDst, d->dxDst, d->dyDst, (unsigned long)d->dwFlags);
        if (!(d->dwFlags & ICDECOMPRESS_PREROLL) && d->lpDst && d->lpbiDst) {
            draw_frame(c, &fr, d);
            if (c->view) {              /* the game copies this area to the screen */
                c->view[0] = d->xDst;
                c->view[1] = d->yDst;
                c->view[2] = d->dxDst ? d->dxDst : fr.width;
                c->view[3] = d->dyDst ? d->dyDst : fr.height;
            }
        }
        return ICERR_OK;
    }
    case ICM_DECOMPRESSEX_END:
        return ICERR_OK;
    case 0x5001:
        /* Indeo's private settings: the 0x70-byte frame settings hold, at
           0x3c, where the decoder reports each frame's drawn area (the game
           copies only that to the screen in its keyed mode, the heralds) */
        if (b == 0x70 && a)
            c->view = (int32_t *)(uintptr_t)*(const uint32_t *)((const unsigned char *)a + 0x3c);
        return ICERR_OK;
    }
    return ICERR_UNSUPPORTED;
}

LRESULT WINAPI ICClose(HIC h)
{ API_TRACE();
    Codec *c = obj_get((Handle)(uintptr_t)h, OBJ_CODEC);
    if (c) {
        iv41_close(c->dec);
        free(c);
        obj_free((Handle)(uintptr_t)h);
    }
    return ICERR_OK;
}
