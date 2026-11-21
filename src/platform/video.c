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
#include <math.h>
#include <time.h>
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
    HdAnim *own;                /* OpenCiv2's own herald (open_own_herald): data holds a
                                   record per frame (OwnSample), the codec draws from this */
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

/* ---- OpenCiv2's own heralds (docs/HD-GRAPHICS.md, "Your own heralds") ----
   With HD graphics, where data/native/hd-extra/heralds.txt has one for the
   tribe, the game's KINGS\HRLDxxx.AVI opens it instead, whether the game
   folder has the CD's video or not: an AVI of one video stream at the
   herald's rate, its frames the animated WebP's ('OHRL', each a key frame:
   the game seeks straight to it), which the codec below draws with their
   alpha over the room, sampled at the plane's resolution.  The game plays
   it as it plays the CD's (src/auto/diplomacy.cpp: its segments in the
   game's table, hd_herald in src/game/hd_graphics.cpp). */
#define OWN_FCC FCC('O', 'H', 'R', 'L')
typedef struct { DWORD magic, frame; HdAnim *anim; } OwnSample;

/* The herald's code (xxx) of a path ending in HRLDxxx.AVI, else 0. */
static int herald_code(const char *name, char *code)
{
    const char *b = name, *p;
    for (p = name; *p; p++)
        if (*p == '\\' || *p == '/')
            b = p + 1;
    if (strlen(b) != 11 || (b[0] | 32) != 'h' || (b[1] | 32) != 'r' || (b[2] | 32) != 'l' || (b[3] | 32) != 'd' ||
        b[7] != '.' || (b[8] | 32) != 'a' || (b[9] | 32) != 'v' || (b[10] | 32) != 'i')
        return 0;
    memcpy(code, b + 4, 3);
    code[3] = 0;
    return 1;
}

static void put32(unsigned char *p, DWORD v) { p[0] = (unsigned char)v; p[1] = (unsigned char)(v >> 8); p[2] = (unsigned char)(v >> 16); p[3] = (unsigned char)(v >> 24); }

static HRESULT open_own_herald(PAVIFILE *pf, const char *name, const HdHerald *h, const char *path)
{
    FILE *f = fopen(path, "rb");
    unsigned char *data = NULL;
    long size = 0;
    HdAnim *anim;
    AviFile *a;
    Stream *s;
    BITMAPINFOHEADER *bi;
    int i, n;
    if (f && !fseek(f, 0, SEEK_END) && (size = ftell(f)) > 0 && !fseek(f, 0, SEEK_SET) &&
        (data = malloc((size_t)size)) && fread(data, 1, (size_t)size, f) != (size_t)size) {
        free(data);
        data = NULL;
    }
    if (f)
        fclose(f);
    if (!(anim = hd_anim_open(data, (size_t)size))) {
        plat_log("AVIFileOpen(%s): %s is not an animated WebP\n", name, path);
        return AVIERR_FILEOPEN;
    }
    n = hd_anim_frames(anim);
    a = calloc(1, sizeof *a);
    a->own = anim;
    a->nstreams = 1;
    a->size = (long)(n * sizeof(OwnSample));
    a->data = calloc((size_t)n, sizeof(OwnSample));
    s = &a->streams[0];
    put32(s->strh, FCC('v', 'i', 'd', 's'));
    put32(s->strh + 4, OWN_FCC);
    put32(s->strh + 20, 1);                         /* scale */
    put32(s->strh + 24, (DWORD)h->fps);             /* rate */
    put32(s->strh + 36, sizeof(OwnSample));         /* suggested buffer */
    s->fmt_size = sizeof *bi;
    s->fmt = calloc(1, sizeof *bi);
    bi = (BITMAPINFOHEADER *)s->fmt;
    bi->biSize = sizeof *bi;
    bi->biWidth = h->w;
    bi->biHeight = h->h;
    bi->biPlanes = 1;
    bi->biBitCount = 32;
    bi->biCompression = OWN_FCC;
    bi->biSizeImage = sizeof(OwnSample);
    for (i = 0; i < n; i++) {
        OwnSample r = {OWN_FCC, (DWORD)i, anim};
        memcpy(a->data + (size_t)i * sizeof r, &r, sizeof r);
        add_sample(s, (DWORD)(i * sizeof r), sizeof r);
        s->samples[i].key = 1;
    }
    plat_log("AVIFileOpen(%s): OpenCiv2's own herald %s, %d frames at %d fps\n", name, path, n, h->fps);
    *pf = (PAVIFILE)obj_new(OBJ_AVIFILE, a);
    return AVIERR_OK;
}

void WINAPI AVIFileInit(void) { API_TRACE(); }
void WINAPI AVIFileExit(void) { API_TRACE(); }

HRESULT WINAPI AVIFileOpenA(PAVIFILE *pf, LPCSTR name, UINT mode, LPCLSID handler)
{ API_TRACE();
    char host[1024];
    FILE *f;
    AviFile *a;
    long movi = 0;
    char code[4], own[1400];
    HdHerald herald;
    (void)mode; (void)handler;
    *pf = 0;
    if (herald_code(name, code) && hd_herald_find(code, &herald, own, sizeof own))
        return open_own_herald(pf, name, &herald, own);
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
    hd_anim_close(a->own);
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
    int own;                    /* OpenCiv2's own herald's frames (OWN_FCC) */
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

/* ---- the frames on an HD plane (docs/HD-GRAPHICS.md) ----
   Where the destination has a plane (HD graphics, sharp text), the frame is
   enlarged on it smoothly: each plane pixel of a pixel the decoder wrote is
   sampled bilinearly from the decoded frame's own colours (not the
   dithered 8-bit pixels), within the source rectangle and from its opaque
   pixels only (no black of the transparent parts, nor anything outside the
   frame, bleeds in); the pixels it leaves transparent keep what the plane
   shows.  The sprites and pictures are not concerned. */
typedef struct {
    uint32_t *rgb;              /* the frame's colours, 0: transparent (opaque ones have alpha) */
    unsigned char *cov;         /* per source pixel, its opaque neighbourhood (0..255): the
                                   opacity smoothed by a 3 x 3 binomial filter; NULL: a frame
                                   without transparency */
    int w, x0, y0, x1, y1;      /* the source rectangle, inclusive */
    double kx, ky;              /* source pixels per destination pixel */
    int xd, yd, dxd;            /* the destination's origin and width */
    int *col;                   /* per plane column of the destination: two source columns and
                                   the second's weight (0..256) */
} Smooth;

/* The source pixel and the next one (clamped to the rectangle) round
   coordinate u, and the next one's weight. */
static void taps(double u, int lo, int hi, int *a, int *b, int *wb)
{
    int i = (int)floor(u);
    *wb = (int)((u - i) * 256 + .5);
    *a = i < lo ? lo : i > hi ? hi : i;
    *b = i + 1 < lo ? lo : i + 1 > hi ? hi : i + 1;
}

static void smooth_columns(Smooth *m)
{
    int d = hd_density, n = m->dxd * d, k;
    if (!(m->col = malloc((size_t)n * 3 * sizeof *m->col)))
        return;
    for (k = 0; k < n; k++)
        taps(m->x0 + (k + .5) / d * m->kx - .5, m->x0, m->x1, &m->col[3 * k], &m->col[3 * k + 1], &m->col[3 * k + 2]);
}

/* The written pixels tx0..tx1-1 of row ty, without a plane: shown
   enlarged. */
static void frame_run(Bitmap *bm, const Smooth *m, unsigned int *plane, int tx0, int tx1, int ty)
{
    (void)m; (void)plane;
    hd_redrawn(bm, tx0, ty, tx1 - tx0, 1);
}

/* The sharpened coverage (0..256) of a plane pixel whose bilinear share of
   opaque pixels is a (0..256): a smoothstep from 1/4 to 3/4, so that the
   outline of a transparent area is smooth at the plane's resolution and
   about one plane pixel wide at density 2, not blurred over a game pixel. */
static int edge_alpha(int a)
{
    static int lut[257], ready;
    if (!ready) {
        int i;
        for (i = 0; i <= 256; i++) {
            double t = (i / 256.0 - .25) / .5;
            t = t < 0 ? 0 : t > 1 ? 1 : t;
            lut[i] = (int)(t * t * (3 - 2 * t) * 256 + .5);
        }
        ready = 1;
    }
    return lut[a < 0 ? 0 : a > 256 ? 256 : a];
}

/* Row ty of the destination (the frame's columns tx0..tx1-1, source row
   sy) on the plane: each plane pixel sampled bilinearly from the opaque
   pixels round it, its colour from those alone (no transparent black
   bleeds in), and drawn over what the plane shows with the share of
   opaque pixels among its taps as its coverage, sharpened (edge_alpha):
   the outline of a herald smooth, not a staircase of game pixels.  The
   game pixels concerned (opaque, or transparent next to an opaque one)
   are taken as shown. */
static void frame_plane_row(Bitmap *bm, const Smooth *m, unsigned int *plane,
                            const ICDECOMPRESSEX *dc, int dxs, int dxd, int tx0, int tx1, int ty, int sy)
{
    int d = hd_density, pitch = bm->w * d, tx, i, j, n = 0, run;
    int *runs = malloc((size_t)(tx1 - tx0 + 2) * sizeof *runs);  /* start, end, ... */
    if (!runs || sy < m->y0 || sy > m->y1) {
        free(runs);
        return;
    }
    for (tx = tx0, run = -1; tx <= tx1; tx++) {
        int near = 0;
        if (tx < tx1) {
            int sx = dc->xSrc + (tx - dc->xDst) * dxs / dxd;
            near = sx >= m->x0 && sx <= m->x1 && (!m->cov || m->cov[(size_t)sy * m->w + sx]);
        }
        if (near && run < 0)
            run = tx;
        else if (!near && run >= 0) {
            runs[n++] = run;
            runs[n++] = tx;
            run = -1;
        }
    }
    for (j = 0; j < d; j++) {
        int ya, yb, wy, r0;
        const uint32_t *ra, *rb;
        const unsigned char *ca, *cb;
        uint32_t *out = plane + (size_t)(ty * d + j) * pitch;
        taps(m->y0 + ((ty - m->yd) + (j + .5) / d) * m->ky - .5, m->y0, m->y1, &ya, &yb, &wy);
        ra = m->rgb + (size_t)ya * m->w;
        rb = m->rgb + (size_t)yb * m->w;
        ca = m->cov ? m->cov + (size_t)ya * m->w : NULL;
        cb = m->cov ? m->cov + (size_t)yb * m->w : NULL;
        for (r0 = 0; r0 < n; r0 += 2)
          for (tx = runs[r0]; tx < runs[r0 + 1]; tx++) {
            int k;
            for (i = 0, k = ((tx - m->xd) * d) * 3; i < d; i++, k += 3) {
                int xa = m->col[k], xb = m->col[k + 1], wx = m->col[k + 2];
                uint32_t p[4] = {ra[xa], ra[xb], rb[xa], rb[xb]}, bg;
                int w4[4] = {(256 - wx) * (256 - wy), wx * (256 - wy), (256 - wx) * wy, wx * wy}, q, sum = 0, a;
                unsigned r = 0, g = 0, bl = 0;
                if (!ca || (ca[xa] & ca[xb] & cb[xa] & cb[xb]) == 255) {
                    /* inside: all four opaque */
                    out[tx * d + i] =
                        0xff000000u |
                        (((p[0] >> 16 & 255) * w4[0] + (p[1] >> 16 & 255) * w4[1] + (p[2] >> 16 & 255) * w4[2] +
                          (p[3] >> 16 & 255) * w4[3] + 32768) >> 16) << 16 |
                        (((p[0] >> 8 & 255) * w4[0] + (p[1] >> 8 & 255) * w4[1] + (p[2] >> 8 & 255) * w4[2] +
                          (p[3] >> 8 & 255) * w4[3] + 32768) >> 16) << 8 |
                        (((p[0] & 255) * w4[0] + (p[1] & 255) * w4[1] + (p[2] & 255) * w4[2] + (p[3] & 255) * w4[3] +
                          32768) >> 16);
                    continue;
                }
                a = edge_alpha((int)(((unsigned)w4[0] * ca[xa] + (unsigned)w4[1] * ca[xb] + (unsigned)w4[2] * cb[xa] +
                                      (unsigned)w4[3] * cb[xb] + 32768) >> 16) * 256 / 255);
                if (!a)
                    continue;
                for (q = 0; q < 4; q++)
                    if (p[q]) {
                        sum += w4[q];
                        r += (unsigned)w4[q] * ((p[q] >> 16) & 255);
                        g += (unsigned)w4[q] * ((p[q] >> 8) & 255);
                        bl += (unsigned)w4[q] * (p[q] & 255);
                    }
                if (sum <= 0)
                    continue;
                r = (r + sum / 2) / sum;
                g = (g + sum / 2) / sum;
                bl = (bl + sum / 2) / sum;
                if (a < 256) {
                    bg = out[tx * d + i];
                    r = (r * a + ((bg >> 16) & 255) * (256 - a) + 128) >> 8;
                    g = (g * a + ((bg >> 8) & 255) * (256 - a) + 128) >> 8;
                    bl = (bl * a + (bg & 255) * (256 - a) + 128) >> 8;
                }
                out[tx * d + i] = 0xff000000u | r << 16 | g << 8 | bl;
            }
          }
    }
    for (i = 0; i < n; i += 2)
        hd_bitmap_commit(bm, runs[i], ty, runs[i + 1] - runs[i], 1);
    free(runs);
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
    Smooth sm;
    unsigned int *plane = NULL;
    memset(&sm, 0, sizeof sm);
    if (bm && hd_on && (plane = hd_bitmap_want_plane(bm))) {
        sm.w = fr->width;
        sm.dxd = dxd;
        sm.x0 = d->xSrc < 0 ? 0 : d->xSrc;
        sm.y0 = d->ySrc < 0 ? 0 : d->ySrc;
        sm.x1 = (d->xSrc + dxs < fr->width ? d->xSrc + dxs : fr->width) - 1;
        sm.y1 = (d->ySrc + dys < fr->height ? d->ySrc + dys : fr->height) - 1;
        sm.kx = (double)dxs / dxd;
        sm.ky = (double)dys / dyd;
        sm.xd = d->xDst;
        sm.yd = d->yDst;
        if (sm.x0 <= sm.x1 && sm.y0 <= sm.y1 && (sm.rgb = malloc((size_t)fr->width * fr->height * 4)))
            for (y = sm.y0; y <= sm.y1; y++)
                for (x = sm.x0; x <= sm.x1; x++) {
                    int Y = fr->y[y * fr->y_stride + x], U = fr->u[(y >> 2) * fr->uv_stride + (x >> 2)] - 128,
                        V = fr->v[(y >> 2) * fr->uv_stride + (x >> 2)] - 128;
                    uint32_t px = 0;
                    if (!(fr->mask ? fr->mask[y * fr->width + x] :
                          (fr->has_transparency || test_key) && Y <= 16 && U >= -2 && U <= 2 && V >= -2 && V <= 2)) {
                        Y = 298 * (Y - 16);
                        px = 0xff000000u | (uint32_t)clamp8((Y + 409 * V + 128) >> 8) << 16 |
                             (uint32_t)clamp8((Y - 100 * U - 208 * V + 128) >> 8) << 8 |
                             (uint32_t)clamp8((Y + 516 * U + 128) >> 8);
                    }
                    sm.rgb[(size_t)y * fr->width + x] = px;
                }
        smooth_columns(&sm);
        if (sm.rgb && (fr->mask || fr->has_transparency || test_key) &&
            (sm.cov = malloc((size_t)fr->width * fr->height))) {
            /* the opacity's 1-2-1 sums along the rows, then down the columns */
            int cw = sm.x1 - sm.x0 + 1, *hs = malloc((size_t)cw * (sm.y1 - sm.y0 + 1) * sizeof *hs);
            if (!hs) {
                free(sm.cov);
                sm.cov = NULL;
            } else {
                for (y = sm.y0; y <= sm.y1; y++) {
                    const uint32_t *r = sm.rgb + (size_t)y * fr->width;
                    int *o = hs + (size_t)(y - sm.y0) * cw;
                    for (x = sm.x0; x <= sm.x1; x++)
                        o[x - sm.x0] = (r[x > sm.x0 ? x - 1 : x] != 0) + 2 * (r[x] != 0) + (r[x < sm.x1 ? x + 1 : x] != 0);
                }
                for (y = sm.y0; y <= sm.y1; y++) {
                    const int *u = hs + (size_t)((y > sm.y0 ? y - 1 : y) - sm.y0) * cw,
                              *c0 = hs + (size_t)(y - sm.y0) * cw,
                              *dn = hs + (size_t)((y < sm.y1 ? y + 1 : y) - sm.y0) * cw;
                    unsigned char *o = sm.cov + (size_t)y * fr->width;
                    for (x = 0; x < cw; x++)
                        o[sm.x0 + x] = (unsigned char)((u[x] + 2 * c0[x] + dn[x]) * 255 / 16);
                }
                free(hs);
            }
        }
    }
    int smooth = plane && sm.rgb && sm.col;
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
                if (bm && !smooth)
                    frame_run(bm, &sm, plane, run, run_end, ty);
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
                if (run >= 0 && bm && !smooth)
                    frame_run(bm, &sm, plane, run, run_end, ty);
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
        if (run >= 0 && bm && !smooth)
            frame_run(bm, &sm, plane, run, run_end, ty);
        if (smooth) {
            int x0 = d->xDst < 0 ? 0 : d->xDst, x1 = d->xDst + dxd > w ? w : d->xDst + dxd;
            if (x0 < x1)
                frame_plane_row(bm, &sm, plane, d, dxs, dxd, x0, x1, ty, sy);
        }
    }
    free(sm.rgb);
    free(sm.col);
    free(sm.cov);
}

/* An own herald's frame (premultiplied RGBA, 0..255) over the canvas's
   area x0..x1, y0..y1: bilinear where it is at most a canvas pixel, else
   the canvas pixels it covers averaged. */
static void own_sample(const unsigned char *px, int cw, int ch, double x0, double y0, double x1, double y1, int *o)
{
    int x, y, n = 0, k;
    long sum[4] = {0, 0, 0, 0};
    if (x1 - x0 <= 1.0 && y1 - y0 <= 1.0) {
        double u = (x0 + x1) / 2 - .5, v = (y0 + y1) / 2 - .5;
        int ia = (int)floor(u), ja = (int)floor(v);
        double fu = u - ia, fv = v - ja, acc[4] = {0, 0, 0, 0};
        for (k = 0; k < 4; k++) {
            int xx = ia + (k & 1), yy = ja + (k >> 1), c;
            double w = ((k & 1) ? fu : 1 - fu) * ((k >> 1) ? fv : 1 - fv);
            const unsigned char *p;
            xx = xx < 0 ? 0 : xx >= cw ? cw - 1 : xx;
            yy = yy < 0 ? 0 : yy >= ch ? ch - 1 : yy;
            p = px + ((size_t)yy * cw + xx) * 4;
            for (c = 0; c < 3; c++)
                acc[c] += w * p[c] * p[3] / 255.0;
            acc[3] += w * p[3];
        }
        for (k = 0; k < 4; k++)
            o[k] = (int)(acc[k] + .5);
        return;
    }
    {
        int xa = (int)floor(x0), xb = (int)ceil(x1), ya = (int)floor(y0), yb = (int)ceil(y1);
        if (xa < 0) xa = 0;
        if (ya < 0) ya = 0;
        if (xb > cw) xb = cw;
        if (yb > ch) yb = ch;
        for (y = ya; y < yb; y++)
            for (x = xa; x < xb; x++) {
                const unsigned char *p = px + ((size_t)y * cw + x) * 4;
                sum[0] += p[0] * p[3];
                sum[1] += p[1] * p[3];
                sum[2] += p[2] * p[3];
                sum[3] += p[3];
                n++;
            }
        for (k = 0; k < 3; k++)
            o[k] = n ? (int)((sum[k] + n * 255 / 2) / ((long)n * 255)) : 0;
        o[3] = n ? (int)((sum[3] + n / 2) / n) : 0;
    }
}

/* An own herald's frame into the destination (the game's 8-bit window over
   the room, which it has put back there before): each game pixel at least
   half covered by the figure takes the nearest palette colour of the
   figure's there; on the plane (HD graphics) the frame is drawn over what
   it shows at the plane's resolution, with its alpha (bilinear, from the
   frame premultiplied, where the plane has as many pixels as the frame or
   more; else each plane pixel the frame's it covers averaged), only round
   the figure, and the game pixels it touched are taken as shown. */
static void draw_own(Codec *c, HdAnim *anim, int frame, const ICDECOMPRESSEX *d)
{
    BITMAPINFOHEADER *bi = d->lpbiDst;
    Bitmap *bm = bmp_of_bits(d->lpDst);
    clock_t t0 = clock(), t1, t2;
    const unsigned char *px = hd_anim_frame(anim, frame);
    int cw, ch, W, H, x, y, i, j, dd = hd_density, up;
    unsigned int *plane;
    unsigned char *near = NULL, *pm = NULL;
    int *cols = NULL, *rows = NULL, *rx, *ry;
    double kx, ky;
    if (!px || !bm || bm->bpp != 8 || bi->biBitCount != 8)
        return;
    hd_anim_size(anim, &cw, &ch);
    W = d->dxDst ? d->dxDst : bi->biWidth;
    H = d->dyDst ? d->dyDst : abs((int)bi->biHeight);
    kx = (double)cw / W;
    ky = (double)ch / H;
    if (!c->lut_ok)
        build_lut(c);
    plane = hd_bitmap_want_plane(bm);
    near = calloc((size_t)W * H, 1);
    pm = malloc((size_t)cw * ch * 4);
    rx = malloc((size_t)(W + 1) * sizeof *rx);
    ry = malloc((size_t)(H + 1) * sizeof *ry);
    if (!near || !pm || !rx || !ry) {
        free(near); free(pm); free(rx); free(ry);
        return;
    }
    /* the frame premultiplied */
    for (i = 0; i < cw * ch; i++) {
        const unsigned char *q = px + (size_t)i * 4;
        unsigned char *o = pm + (size_t)i * 4;
        o[0] = (unsigned char)((q[0] * q[3] + 127) / 255);
        o[1] = (unsigned char)((q[1] * q[3] + 127) / 255);
        o[2] = (unsigned char)((q[2] * q[3] + 127) / 255);
        o[3] = q[3];
    }
    /* the frame's pixels each game pixel covers: columns rx[x] .. rx[x + 1] - 1 (at least one) */
    for (x = 0; x <= W; x++)
        rx[x] = x == W ? cw : (int)(x * kx + .5);
    for (y = 0; y <= H; y++)
        ry[y] = y == H ? ch : (int)(y * ky + .5);
    /* the game's pixels, averaged; where the figure is (near: it or a neighbour has some of it) */
    for (y = 0; y < H; y++) {
        int ty = d->yDst + y, y0 = ry[y], y1 = ry[y + 1] > y0 ? ry[y + 1] : y0 + 1;
        unsigned char *row = ty >= 0 && ty < bm->h ? bm->bits + (size_t)(bm->bottom_up ? bm->h - 1 - ty : ty) * bm->stride : NULL;
        if (y1 > ch)
            y0 = (y1 = ch) - 1;
        for (x = 0; x < W; x++) {
            int tx = d->xDst + x, x0 = rx[x], x1 = rx[x + 1] > x0 ? rx[x + 1] : x0 + 1, u, v, n;
            unsigned sr = 0, sg = 0, sb = 0, sa = 0;
            if (x1 > cw)
                x0 = (x1 = cw) - 1;
            for (v = y0; v < y1; v++) {
                const unsigned char *q = pm + ((size_t)v * cw + x0) * 4;
                for (u = x0; u < x1; u++, q += 4) {
                    sr += q[0];
                    sg += q[1];
                    sb += q[2];
                    sa += q[3];
                }
            }
            if (!sa)
                continue;
            for (j = y > 0 ? y - 1 : 0; j <= y + 1 && j < H; j++)
                for (i = x > 0 ? x - 1 : 0; i <= x + 1 && i < W; i++)
                    near[(size_t)j * W + i] = 1;
            n = (x1 - x0) * (y1 - y0);
            if (row && tx >= 0 && tx < bm->w && sa >= 128u * n)
                row[tx] = c->lut[(sr * 255 / sa) >> 3 << 10 | (sg * 255 / sa) >> 3 << 5 | (sb * 255 / sa) >> 3];
        }
    }
    t1 = clock();
    if (!plane) {
        free(near); free(pm); free(rx); free(ry);
        return;
    }
    /* enlarged (or as large): bilinear taps per plane column and row, the frame premultiplied */
    up = kx <= dd + 1e-9 && ky <= dd + 1e-9;
    if (up) {
        int n;
        cols = malloc((size_t)W * dd * 3 * sizeof *cols);
        rows = malloc((size_t)H * dd * 3 * sizeof *rows);
        if (!cols || !rows)
            up = 0;
        for (n = 0; up && n < W * dd; n++)
            taps((n + .5) * kx / dd - .5, 0, cw - 1, &cols[3 * n], &cols[3 * n + 1], &cols[3 * n + 2]);
        for (n = 0; up && n < H * dd; n++)
            taps((n + .5) * ky / dd - .5, 0, ch - 1, &rows[3 * n], &rows[3 * n + 1], &rows[3 * n + 2]);
    }
    for (y = 0; y < H; y++) {
        int ty = d->yDst + y, run = -1;
        if (ty < 0 || ty >= bm->h)
            continue;
        for (x = 0; x <= W; x++) {
            int tx = d->xDst + x, touched = 0;
            if (x < W && tx >= 0 && tx < bm->w && near[(size_t)y * W + x])
                for (j = 0; j < dd; j++) {
                    uint32_t *out = plane + (size_t)(ty * dd + j) * bm->w * dd + (size_t)tx * dd;
                    const int *rt = up ? rows + 3 * (y * dd + j) : NULL;
                    for (i = 0; i < dd; i++) {
                        int q[4];
                        unsigned bg = out[i], r, g, b;
                        if (up) {
                            const int *ct = cols + 3 * (x * dd + i);
                            const unsigned char *p00 = pm + ((size_t)rt[0] * cw + ct[0]) * 4,
                                                *p01 = pm + ((size_t)rt[0] * cw + ct[1]) * 4,
                                                *p10 = pm + ((size_t)rt[1] * cw + ct[0]) * 4,
                                                *p11 = pm + ((size_t)rt[1] * cw + ct[1]) * 4;
                            int wx = ct[2], wy = rt[2], k;
                            for (k = 0; k < 4; k++)
                                q[k] = (((p00[k] * (256 - wx) + p01[k] * wx) * (256 - wy) +
                                         (p10[k] * (256 - wx) + p11[k] * wx) * wy) + 32768) >> 16;
                        } else
                            own_sample(px, cw, ch, (x + (double)i / dd) * kx, (y + (double)j / dd) * ky,
                                       (x + (double)(i + 1) / dd) * kx, (y + (double)(j + 1) / dd) * ky, q);
                        if (!q[3])
                            continue;
                        touched = 1;
                        r = (unsigned)q[0] + ((bg >> 16 & 255) * (255 - q[3]) + 127) / 255;
                        g = (unsigned)q[1] + ((bg >> 8 & 255) * (255 - q[3]) + 127) / 255;
                        b = (unsigned)q[2] + ((bg & 255) * (255 - q[3]) + 127) / 255;
                        out[i] = 0xff000000u | (r > 255 ? 255 : r) << 16 | (g > 255 ? 255 : g) << 8 |
                                 (b > 255 ? 255 : b);
                    }
                }
            if (touched && run < 0)
                run = tx;
            else if (!touched && run >= 0) {
                hd_bitmap_commit(bm, run, ty, tx - run, 1);
                run = -1;
            }
        }
    }
    t2 = clock();
    if (getenv("CIV2_LOG_VIDEO"))
        plat_log("video: own herald frame %d: decoded, the game's pixels %.1f ms, the plane (density %d) %.1f ms\n",
                 frame, (t1 - t0) * 1000.0 / CLOCKS_PER_SEC, hd_density,
                 (t2 - t1) * 1000.0 / CLOCKS_PER_SEC);
    free(near);
    free(pm);
    free(rx);
    free(ry);
    free(cols);
    free(rows);
}

HIC WINAPI ICLocate(DWORD type, DWORD handler, LPBITMAPINFOHEADER in, LPBITMAPINFOHEADER out, WORD mode)
{ API_TRACE();
    Codec *c;
    (void)type; (void)out; (void)mode;
    if ((in && in->biCompression == OWN_FCC) || handler == OWN_FCC) {
        c = calloc(1, sizeof *c);
        c->own = 1;
        video_palette(c->pal);
        return (HIC)(uintptr_t)obj_new(OBJ_CODEC, c);
    }
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
        if (c->own) {
            if (d && d->lpbiDst && d->lpbiDst->biBitCount == 8 && !c->lut_ok)
                memcpy(c->pal, (unsigned char *)d->lpbiDst + d->lpbiDst->biSize, sizeof c->pal);
            return ICERR_OK;
        }
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
        if (c->own) {
            OwnSample r;
            if (!d || !d->lpSrc || !d->lpbiDst)
                return ICERR_ERROR;
            memcpy(&r, d->lpSrc, sizeof r);
            if (r.magic != OWN_FCC)
                return ICERR_ERROR;
            if (!(d->dwFlags & ICDECOMPRESS_PREROLL) && d->lpDst) {
                draw_own(c, r.anim, (int)r.frame, d);
                if (c->view) {
                    c->view[0] = d->xDst;
                    c->view[1] = d->yDst;
                    c->view[2] = d->dxDst ? d->dxDst : d->lpbiDst->biWidth;
                    c->view[3] = d->dyDst ? d->dyDst : abs((int)d->lpbiDst->biHeight);
                }
            }
            return ICERR_OK;
        }
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
