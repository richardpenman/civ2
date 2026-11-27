/*
 * Sound: waveOut over the host's audio (src/host), sndPlaySound, mmio (RIFF files), MCI.
 *
 * In replay mode there is no audio device, as in the reference environment
 * (an X server in a container, without sound): the game then runs without
 * sound, identically in both.
 */
#include <sys/stat.h>
#include "plat.h"
#include "../host/host.h"
#include "plat_files.h"
#include "hd.h"
#define STB_VORBIS_HEADER_ONLY
#include "../third_party/stb_vorbis/stb_vorbis.c"

static int audio_ok;
static int cd_virtual;          /* CIV2_TEST_MUSIC in a replay: the drive plays on the virtual clock */
static void cd_poll(void);

void sound_init(void)
{
    /* CIV2_TEST_AUDIO: audio in a replay too (tests of the music; with
       SDL_AUDIODRIVER=dummy where there is no sound card) */
    cd_virtual = plat_replay && getenv("CIV2_TEST_MUSIC") && !getenv("CIV2_TEST_AUDIO");
    if ((plat_replay && !getenv("CIV2_TEST_AUDIO")) || getenv("CIV2_NOSOUND"))
        return;
    audio_ok = host_audio_init();
}

/* ---- waveOut ---- */

#define MAX_HDR 64
typedef struct {
    int dev;
    WAVEFORMATEX fmt;
    DWORD_PTR cb, inst;
    DWORD flags;
    LPWAVEHDR pending[MAX_HDR];         /* written, not yet played */
    uint64_t end_bytes[MAX_HDR];          /* total queued when each ends */
    int npending;
    uint64_t queued_total;
} WaveOut;

static WaveOut *wave_get(HWAVEOUT h) { return obj_get(h, OBJ_WAVEOUT); }

/* The open waveOut devices, in order of opening: what sound_poll looks at
   (scanning all 8192 handle slots on every message check made busy loops
   such as building the world some 40 times slower). */
#define MAX_WAVE 64
static HWAVEOUT wave_open[MAX_WAVE];
static int nwave_open;

UINT WINAPI waveOutGetNumDevs(void) { API_TRACE(); return audio_ok ? 1 : 0; }

MMRESULT WINAPI waveOutGetDevCapsA(UINT_PTR id, LPWAVEOUTCAPSA caps, UINT n)
{ API_TRACE();
    (void)id;
    if (!audio_ok)
        return MMSYSERR_BADDEVICEID;
    memset(caps, 0, n < sizeof *caps ? n : sizeof *caps);
    snprintf(caps->szPname, sizeof caps->szPname, "Audio");
    caps->dwFormats = 0xfff;
    caps->wChannels = 2;
    caps->dwSupport = WAVECAPS_VOLUME;
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI waveOutGetErrorTextA(MMRESULT err, LPSTR buf, UINT n)
{ API_TRACE();
    snprintf(buf, n, "wave output error %u", (unsigned)err);
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI waveOutOpen(LPHWAVEOUT out, UINT id, LPCWAVEFORMATEX f, DWORD_PTR cb, DWORD_PTR inst, DWORD flags)
{ API_TRACE();
    WaveOut *w;
    (void)id;
    if (!audio_ok)
        return MMSYSERR_NODRIVER;
    if (f->wFormatTag != WAVE_FORMAT_PCM)
        return WAVERR_BADFORMAT;
    if (flags & WAVE_FORMAT_QUERY)
        return MMSYSERR_NOERROR;
    if (nwave_open >= MAX_WAVE)                 /* (one sound_poll would not see) */
        return MMSYSERR_ALLOCATED;
    w = calloc(1, sizeof *w);
    w->dev = host_audio_open((int)f->nSamplesPerSec, f->nChannels, f->wBitsPerSample == 8 ? 8 : 16);
    if (!w->dev) {
        free(w);
        return MMSYSERR_ALLOCATED;
    }
    w->fmt = *f;
    w->cb = cb;
    w->inst = inst;
    w->flags = flags;
    *out = obj_new(OBJ_WAVEOUT, w);
    wave_open[nwave_open++] = *out;
    if ((flags & CALLBACK_TYPEMASK) == CALLBACK_WINDOW)
        PostMessageA((HWND)cb, MM_WOM_OPEN, (WPARAM)*out, 0);
    return MMSYSERR_NOERROR;
}

static void wave_done(HWAVEOUT h, WaveOut *w, LPWAVEHDR hdr)
{
    hdr->dwFlags = (hdr->dwFlags & ~WHDR_INQUEUE) | WHDR_DONE;
    switch (w->flags & CALLBACK_TYPEMASK) {
    case CALLBACK_WINDOW:
        PostMessageA((HWND)w->cb, MM_WOM_DONE, (WPARAM)h, (LPARAM)hdr);
        break;
    case CALLBACK_FUNCTION:
        ((void (*)(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR))w->cb)(h, WOM_DONE, w->inst, (DWORD_PTR)hdr, 0);
        break;
    }
}

static uint64_t played_bytes(WaveOut *w)
{
    return w->queued_total - host_audio_queued(w->dev);
}

/* Called from the message loop: finish buffers that have played. */
void sound_poll(void)
{
    HWAVEOUT open[MAX_WAVE];
    int i, n;
    if (!audio_ok) {
        if (cd_virtual)
            cd_poll();
        return;
    }
    cd_poll();
    n = nwave_open;
    memcpy(open, wave_open, sizeof open[0] * (size_t)n);       /* (a callback may close one) */
    for (i = 0; i < n; i++) {
        HWAVEOUT h = open[i];
        WaveOut *w = wave_get(h);
        if (!w)
            continue;
        while (w->npending && played_bytes(w) >= w->end_bytes[0]) {
            LPWAVEHDR hdr = w->pending[0];
            memmove(w->pending, w->pending + 1, sizeof w->pending[0] * (size_t)(w->npending - 1));
            memmove(w->end_bytes, w->end_bytes + 1, sizeof w->end_bytes[0] * (size_t)(w->npending - 1));
            w->npending--;
            wave_done(h, w, hdr);
        }
    }
}

MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT h, LPWAVEHDR hdr, UINT n)
{ API_TRACE();
    (void)h; (void)n;
    hdr->dwFlags |= WHDR_PREPARED;
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT h, LPWAVEHDR hdr, UINT n)
{ API_TRACE();
    (void)h; (void)n;
    if (hdr->dwFlags & WHDR_INQUEUE)
        return WAVERR_STILLPLAYING;
    hdr->dwFlags &= ~WHDR_PREPARED;
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI waveOutWrite(HWAVEOUT h, LPWAVEHDR hdr, UINT n)
{ API_TRACE();
    WaveOut *w = wave_get(h);
    (void)n;
    if (!w)
        return MMSYSERR_INVALHANDLE;
    if (w->npending == MAX_HDR)
        return MMSYSERR_NOMEM;
    hdr->dwFlags = (hdr->dwFlags & ~WHDR_DONE) | WHDR_INQUEUE;
    host_audio_queue(w->dev, hdr->lpData, hdr->dwBufferLength);
    w->queued_total += hdr->dwBufferLength;
    w->pending[w->npending] = hdr;
    w->end_bytes[w->npending++] = w->queued_total;
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI waveOutReset(HWAVEOUT h)
{ API_TRACE();
    WaveOut *w = wave_get(h);
    if (!w)
        return MMSYSERR_INVALHANDLE;
    host_audio_clear(w->dev);
    w->queued_total = 0;
    while (w->npending) {
        LPWAVEHDR hdr = w->pending[0];
        memmove(w->pending, w->pending + 1, sizeof w->pending[0] * (size_t)(w->npending - 1));
        w->npending--;
        wave_done(h, w, hdr);
    }
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI waveOutClose(HWAVEOUT h)
{ API_TRACE();
    WaveOut *w = wave_get(h);
    int i;
    if (!w)
        return MMSYSERR_INVALHANDLE;
    if (w->npending)
        return WAVERR_STILLPLAYING;
    host_audio_close(w->dev);
    if ((w->flags & CALLBACK_TYPEMASK) == CALLBACK_WINDOW)
        PostMessageA((HWND)w->cb, MM_WOM_CLOSE, (WPARAM)h, 0);
    free(w);
    obj_free(h);
    for (i = 0; i < nwave_open; i++)
        if (wave_open[i] == h) {
            memmove(wave_open + i, wave_open + i + 1, sizeof wave_open[0] * (size_t)(--nwave_open - i));
            break;
        }
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI waveOutGetPosition(HWAVEOUT h, LPMMTIME t, UINT n)
{ API_TRACE();
    WaveOut *w = wave_get(h);
    uint64_t bytes;
    (void)n;
    if (!w)
        return MMSYSERR_INVALHANDLE;
    bytes = played_bytes(w);
    if (t->wType == TIME_SAMPLES) {
        t->u.sample = (DWORD)(bytes / (w->fmt.nBlockAlign ? w->fmt.nBlockAlign : 1));
    } else if (t->wType == TIME_MS) {
        t->u.ms = (DWORD)(bytes * 1000 / (w->fmt.nAvgBytesPerSec ? w->fmt.nAvgBytesPerSec : 1));
    } else {
        t->wType = TIME_BYTES;
        t->u.cb = (DWORD)bytes;
    }
    return MMSYSERR_NOERROR;
}

/* ---- sndPlaySound: whole WAV files ---- */

static int snd_dev;

static DWORD rd32le(const unsigned char *p) { return (DWORD)(p[0] | p[1] << 8 | p[2] << 16 | (DWORD)p[3] << 24); }

WINBOOL WINAPI sndPlaySoundA(LPCSTR name, UINT flags)
{ API_TRACE();
    char host[1024];
    unsigned char *buf;
    long size;
    unsigned pos, rate = 0, chans = 0, bits = 0, data = 0, len = 0;
    FILE *f;
    if (snd_dev) {
        host_audio_close(snd_dev);
        snd_dev = 0;
    }
    if (!name || !audio_ok)
        return name == NULL;
    if (flags & SND_MEMORY)
        return FALSE;
    plat_host_path(name, host, sizeof host, 0);
    if (!(f = fopen(host, "rb")))
        return FALSE;
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf = malloc((size_t)(size > 0 ? size : 1));
    if (size < 12 || fread(buf, 1, (size_t)size, f) != (size_t)size || memcmp(buf, "RIFF", 4) || memcmp(buf + 8, "WAVE", 4)) {
        fclose(f);
        free(buf);
        return FALSE;
    }
    fclose(f);
    /* the PCM format and the samples */
    for (pos = 12; pos + 8 <= (unsigned)size; pos += 8 + ((rd32le(buf + pos + 4) + 1) & ~1u)) {
        unsigned n = rd32le(buf + pos + 4);
        if (!memcmp(buf + pos, "fmt ", 4) && n >= 16 && buf[pos + 8] == 1 && buf[pos + 9] == 0) {
            chans = buf[pos + 10] | buf[pos + 11] << 8;
            rate = rd32le(buf + pos + 12);
            bits = buf[pos + 22] | buf[pos + 23] << 8;
        } else if (!memcmp(buf + pos, "data", 4)) {
            data = pos + 8;
            len = n < (unsigned)size - data ? n : (unsigned)size - data;
        }
    }
    if (!rate || !data || (bits != 8 && bits != 16) || !(snd_dev = host_audio_open((int)rate, (int)chans, (int)bits))) {
        free(buf);
        return FALSE;
    }
    host_audio_queue(snd_dev, buf + data, len);
    free(buf);
    if (!(flags & SND_ASYNC))
        while (host_audio_queued(snd_dev))
            host_delay_ms(10);
    return TRUE;
}

/* ---- the sound effects without the install (docs/NO-INSTALL.md) ----
 *
 * Where the game folder lacks one of the game's sound effects
 * (Sound\NAME.WAV), OpenCiv2's own stands in for it: data/native/sound/
 * name.ogg (CC0: data/native/sound/SOURCES.md), decoded (stb_vorbis) into a
 * WAV as the game reads them (snd_wave_load_file: 22050 Hz, 8 bits; mono),
 * written once a run into the folder "OpenCiv2Sounds" of the Windows
 * directory, where plat_host_path finds it (files.c: synth_path).  Only
 * with sound: a replay without it finds no file, as before (the game then
 * plays nothing, and so takes the same steps). */

#define FALLBACK_RATE 22050

/* the WAV made from the samples (16-bit, interleaved), 0 if not written */
static int write_wav8(const char *path, const short *pcm, int frames, int chans, int rate)
{
    long out = rate == FALLBACK_RATE ? frames : (long)((long long)frames * FALLBACK_RATE / rate), i;
    unsigned char h[44], *data;
    FILE *f;
    int ok;
    if (out <= 0 || !(data = malloc((size_t)out)))
        return 0;
    for (i = 0; i < out; i++) {
        /* linear interpolation to 22050 Hz, the channels mixed */
        double pos = (double)i * rate / FALLBACK_RATE, v = 0;
        long a = (long)pos, b = a + 1 < frames ? a + 1 : a;
        double t = pos - (double)a;
        int c, q;
        for (c = 0; c < chans; c++)
            v += pcm[(size_t)a * chans + c] * (1 - t) + pcm[(size_t)b * chans + c] * t;
        q = (int)(v / chans / 256.0 + 128.5);     /* (positive: truncation rounds) */
        data[i] = (unsigned char)(q < 0 ? 0 : q > 255 ? 255 : q);
    }
    memcpy(h, "RIFF", 4);
    h[4] = (unsigned char)(36 + out), h[5] = (unsigned char)((36 + out) >> 8);
    h[6] = (unsigned char)((36 + out) >> 16), h[7] = (unsigned char)((36 + out) >> 24);
    memcpy(h + 8, "WAVEfmt ", 8);
    memcpy(h + 16, "\x10\0\0\0\1\0\1\0", 8);          /* 16 bytes, PCM, mono */
    h[24] = FALLBACK_RATE & 0xff, h[25] = FALLBACK_RATE >> 8, h[26] = h[27] = 0;
    h[28] = FALLBACK_RATE & 0xff, h[29] = FALLBACK_RATE >> 8, h[30] = h[31] = 0;   /* bytes a second */
    memcpy(h + 32, "\1\0\x08\0data", 8);                /* block 1, 8 bits */
    h[40] = (unsigned char)out, h[41] = (unsigned char)(out >> 8);
    h[42] = (unsigned char)(out >> 16), h[43] = (unsigned char)(out >> 24);
    ok = (f = fopen(path, "wb")) && fwrite(h, 1, 44, f) == 44 && fwrite(data, 1, (size_t)out, f) == (size_t)out;
    if (f && fclose(f))
        ok = 0;
    free(data);
    return ok;
}

int sound_fallback(const char *name, char *host, size_t n)
{
    static char made[128][16];
    static int nmade;
    char ogg[1200], dir[1100], lower[16];
    const char *dot = strrchr(name, '.');
    unsigned char *file;
    short *pcm = NULL;
    int i, chans, rate, frames;
    long len;
    FILE *f;
    if (!audio_ok || !dot || strcasecmp(dot, ".wav") || dot - name > 12 || strchr(name, '/'))
        return 0;
    snprintf(dir, sizeof dir, "%s/OpenCiv2Sounds", plat_win_dir);
    snprintf(host, n, "%s/%.*s.WAV", dir, (int)(dot - name), name);
    for (i = 0; i < (int)(dot - name); i++)
        lower[i] = (char)tolower((unsigned char)name[i]);
    lower[i] = 0;
    for (i = 0; i < nmade; i++)
        if (!strcmp(made[i], lower))
            return 1;
    snprintf(ogg, sizeof ogg, "%s/sound/%s.ogg", plat_data_dir, lower);
    if (!(f = fopen(ogg, "rb")))
        return 0;
    file = fseek(f, 0, SEEK_END) == 0 && (len = ftell(f)) > 0 && fseek(f, 0, SEEK_SET) == 0 ? malloc((size_t)len) : NULL;
    if (file && fread(file, 1, (size_t)len, f) != (size_t)len) {
        free(file);
        file = NULL;
    }
    fclose(f);
    if (!file)
        return 0;
    frames = stb_vorbis_decode_memory(file, (int)len, &chans, &rate, &pcm);
    free(file);
    mkdir(dir, 0777);
    if (frames <= 0 || chans <= 0 || rate <= 0 || !write_wav8(host, pcm, frames, chans, rate)) {
        plat_log("sound: cannot make %s from %s\n", host, ogg);
        free(pcm);
        return 0;
    }
    free(pcm);
    if (nmade < 128)
        snprintf(made[nmade++], sizeof made[0], "%s", lower);
    plat_log("sound: %s made from %s\n", name, ogg);
    return 1;
}

/* ---- mmio: a file read into memory, with RIFF chunk navigation ---- */

typedef struct { unsigned char *data; LONG size, pos; int memory; } Mmio;
#define MMIO_BUF 8192

static Mmio *mmio_get(HMMIO h) { return obj_get(h, OBJ_MMIO); }

HMMIO WINAPI mmioOpenA(LPSTR name, LPMMIOINFO info, DWORD flags)
{ API_TRACE();
    char host[1024];
    FILE *f;
    Mmio *m;
    (void)flags;
    if (info)
        info->wErrorRet = 0;
    if (!name) {
        /* A memory file (fccIOProc 'MEM '): the caller's buffer, or one of
           cchBuffer bytes allocated here (MMIO_ALLOCBUF). */
        if (!info || info->fccIOProc != mmioFOURCC('M', 'E', 'M', ' ')) {
            if (info)
                info->wErrorRet = MMIOERR_CANNOTOPEN;
            return 0;
        }
        m = calloc(1, sizeof *m);
        m->memory = 1;
        m->size = (LONG)info->cchBuffer;
        m->data = calloc(1, (size_t)m->size + MMIO_BUF);
        if (info->pchBuffer)
            memcpy(m->data, info->pchBuffer, (size_t)m->size);
        return obj_new(OBJ_MMIO, m);
    }
    plat_host_path(name, host, sizeof host, 0);
    f = fopen(host, "rb");
    if (!f) {
        if (info)
            info->wErrorRet = MMIOERR_CANNOTOPEN;
        return 0;
    }
    m = calloc(1, sizeof *m);
    fseek(f, 0, SEEK_END);
    m->size = (LONG)ftell(f);
    fseek(f, 0, SEEK_SET);
    m->data = malloc((size_t)m->size + MMIO_BUF);
    if (fread(m->data, 1, (size_t)m->size, f) != (size_t)m->size)
        plat_log("mmioOpen: short read\n");
    fclose(f);
    return obj_new(OBJ_MMIO, m);
}

MMRESULT WINAPI mmioClose(HMMIO h, UINT flags)
{ API_TRACE();
    Mmio *m = mmio_get(h);
    (void)flags;
    if (!m)
        return MMSYSERR_INVALHANDLE;
    free(m->data);
    free(m);
    obj_free(h);
    return MMSYSERR_NOERROR;
}

LONG WINAPI mmioRead(HMMIO h, HPSTR buf, LONG n)
{ API_TRACE();
    Mmio *m = mmio_get(h);
    if (!m)
        return -1;
    if (n > m->size - m->pos)
        n = m->size - m->pos;
    if (n < 0)
        n = 0;
    memcpy(buf, m->data + m->pos, (size_t)n);
    m->pos += n;
    return n;
}

LONG WINAPI mmioSeek(HMMIO h, LONG off, int origin)
{ API_TRACE();
    Mmio *m = mmio_get(h);
    LONG p;
    if (!m)
        return -1;
    p = origin == SEEK_SET ? off : origin == SEEK_CUR ? m->pos + off : m->size + off;
    if (p < 0 || p > m->size)
        return -1;
    return m->pos = p;
}

MMRESULT WINAPI mmioDescend(HMMIO h, LPMMCKINFO ck, const MMCKINFO *parent, UINT flags)
{ API_TRACE();
    Mmio *m = mmio_get(h);
    LONG end;
    if (!m)
        return MMSYSERR_INVALHANDLE;
    end = parent ? (LONG)(parent->dwDataOffset + parent->cksize) : m->size;
    for (;;) {
        DWORD id, size, type = 0;
        if (m->pos + 8 > end)
            return MMIOERR_CHUNKNOTFOUND;
        id = rd32le(m->data + m->pos);
        size = rd32le(m->data + m->pos + 4);
        if (id == FOURCC_RIFF || id == FOURCC_LIST)
            type = m->pos + 12 <= m->size ? rd32le(m->data + m->pos + 8) : 0;
        if ((flags & MMIO_FINDCHUNK) && id != ck->ckid) {
            m->pos += 8 + (LONG)((size + 1) & ~1u);
            continue;
        }
        if ((flags & MMIO_FINDRIFF) && (id != FOURCC_RIFF || type != ck->fccType)) {
            m->pos += 8 + (LONG)((size + 1) & ~1u);
            continue;
        }
        if ((flags & MMIO_FINDLIST) && (id != FOURCC_LIST || type != ck->fccType)) {
            m->pos += 8 + (LONG)((size + 1) & ~1u);
            continue;
        }
        ck->ckid = id;
        ck->cksize = size;
        ck->dwDataOffset = (DWORD)(m->pos + 8);
        ck->dwFlags = 0;
        if (id == FOURCC_RIFF || id == FOURCC_LIST) {
            ck->fccType = type;
            m->pos += 12;
        } else {
            m->pos += 8;
        }
        return MMSYSERR_NOERROR;
    }
}

MMRESULT WINAPI mmioAscend(HMMIO h, LPMMCKINFO ck, UINT flags)
{ API_TRACE();
    Mmio *m = mmio_get(h);
    (void)flags;
    if (!m)
        return MMSYSERR_INVALHANDLE;
    m->pos = (LONG)(ck->dwDataOffset + ((ck->cksize + 1) & ~1u));
    return MMSYSERR_NOERROR;
}

/* Direct buffer access: the buffer is the file image from the position. */
MMRESULT WINAPI mmioGetInfo(HMMIO h, LPMMIOINFO info, UINT flags)
{ API_TRACE();
    Mmio *m = mmio_get(h);
    (void)flags;
    if (!m)
        return MMSYSERR_INVALHANDLE;
    memset(info, 0, sizeof *info);
    info->hmmio = h;
    if (m->memory) {
        /* a memory file's buffer is the whole file (the movies' sound
           sources stream through it as a ring) */
        info->fccIOProc = mmioFOURCC('M', 'E', 'M', ' ');
        info->cchBuffer = m->size;
        info->pchBuffer = (HPSTR)m->data;
        info->pchNext = info->pchBuffer + m->pos;
        info->pchEndRead = info->pchEndWrite = info->pchBuffer + m->size;
        return MMSYSERR_NOERROR;
    }
    info->cchBuffer = MMIO_BUF;
    info->pchBuffer = (HPSTR)m->data + m->pos;
    info->pchNext = info->pchBuffer;
    info->pchEndRead = (HPSTR)m->data + (m->pos + MMIO_BUF < m->size ? m->pos + MMIO_BUF : m->size);
    info->pchEndWrite = info->pchBuffer + MMIO_BUF;
    info->lBufOffset = m->pos;
    info->lDiskOffset = m->pos;
    return MMSYSERR_NOERROR;
}

MMRESULT WINAPI mmioAdvance(HMMIO h, LPMMIOINFO info, UINT flags)
{ API_TRACE();
    Mmio *m = mmio_get(h);
    (void)flags;
    if (!m)
        return MMSYSERR_INVALHANDLE;
    m->pos = (LONG)(info->pchNext - (HPSTR)m->data);
    info->pchBuffer = (HPSTR)m->data + m->pos;
    info->pchNext = info->pchBuffer;
    info->pchEndRead = (HPSTR)m->data + (m->pos + MMIO_BUF < m->size ? m->pos + MMIO_BUF : m->size);
    info->lBufOffset = m->pos;
    info->lDiskOffset = m->pos;
    return MMSYSERR_NOERROR;
}

/* ---- MCI: the CD drive ("cdaudio") as files ----
 *
 * The game plays its music from the CD's audio tracks (data track 1, music
 * from track 2).  Here track N is Music\TrackNN.ogg in the game directory
 * (the set-up encodes them from a disc image, src/host/setup_disc.c),
 * decoded as it plays (stb_vorbis), else Music\TrackNN.wav
 * (tools/extract_disc.py writes them from a disc image), else OpenCiv2's
 * own music (docs/NO-INSTALL.md): data/native/music/trackNN.ogg (with HD
 * graphics OpenCiv2's first, the game folder's only for a track it lacks:
 * cd_open_track; made by
 * tools/build_own_assets.py from openciv2/openciv2-assets; read and played
 * as an install's .ogg, so only its 2 to 5 MB are in memory, never a WAV),
 * only with sound, as the sound effects' (sound_fallback); without any of
 * those there is no drive, as before.  The tracks are counted from 2 to the
 * first that none of them has.  Replays have no drive either (as the
 * reference environment), but with CIV2_TEST_MUSIC=1 (the media tests,
 * harness/media): a drive whose tracks play silently on the virtual clock,
 * each as long as its file says (a .wav's data over its byte rate), so that
 * which track the game plays when, and when it ends, is the same in every
 * run; each play, stop and end is logged in calls.log ("music ...").
 * Other MCI devices (avivideo) are not available. */

#define CD_ID 0x4344            /* the drive's device id */

static struct {
    int tracks;                 /* 0: no drive (no music files) */
    int open, current;
    int playing;                /* a track was started (the host plays it: host_music_play) */
    HWND notify;                /* MCI_NOTIFY on the play: told when it ends */
    DWORD end_ms;               /* cd_virtual: when the track ends (plat_now) */
} cd;

/* cd_virtual: the length of a track's file in ms (a .wav's data over its
   byte rate; an .ogg's is not read: a minute). */
static DWORD cd_track_ms(FILE *f, const char *ext)
{
    unsigned char h[12], c[8], fmt[16];
    unsigned long rate = 0, size;
    if (strcmp(ext, "wav") != 0 || fread(h, 1, 12, f) != 12 || memcmp(h, "RIFF", 4) || memcmp(h + 8, "WAVE", 4))
        return 60000;
    while (fread(c, 1, 8, f) == 8) {
        size = c[4] | c[5] << 8 | (unsigned long)c[6] << 16 | (unsigned long)c[7] << 24;
        if (!memcmp(c, "fmt ", 4) && size >= 16 && fread(fmt, 1, 16, f) == 16) {
            rate = fmt[8] | fmt[9] << 8 | (unsigned long)fmt[10] << 16 | (unsigned long)fmt[11] << 24;
            size -= 16;
        } else if (!memcmp(c, "data", 4))
            return rate ? (DWORD)((unsigned long long)size * 1000 / rate) : 60000;
        if (fseek(f, (long)(size + (size & 1)), SEEK_CUR) != 0)
            break;
    }
    return 60000;
}

static int cd_sounding(void)
{
    return cd.playing && (cd_virtual ? (long)(plat_now() - cd.end_ms) < 0 : host_music_playing());
}

static FILE *cd_track_file(int n, const char *ext)
{
    char win[64], host[1024];
    snprintf(win, sizeof win, "Music\\Track%02d.%s", n, ext);
    plat_host_path(win, host, sizeof host, 0);
    return fopen(host, "rb");
}

/* OpenCiv2's own track n (data/native/music/trackNN.ogg; only with sound). */
static FILE *cd_open_own(int n, const char **ext, char *from, size_t size)
{
    char own[1100];
    *ext = "ogg";
    if (!audio_ok)
        return NULL;
    snprintf(own, sizeof own, "%s/music/track%02d.ogg", plat_data_dir, n);
    if (from)
        snprintf(from, size, "%s", own);
    return fopen(own, "rb");
}

/* Track n's file; *ext its kind, from (if given) where it is.  With HD
   graphics (hd_art: Settings' Classic graphics unticked, or the game
   folder without the original's pictures; never with CIV2_CLASSIC=1, nor
   in a replay but with CIV2_TEST_HD or without the pictures) OpenCiv2's own
   first (data/native/music/trackNN.ogg; only with sound), else the game
   folder's Music\TrackNN.ogg, else .wav; with Classic graphics the game
   folder's first, OpenCiv2's only for a track it lacks. */
static FILE *cd_open_track(int n, const char **ext, char *from, size_t size)
{
    FILE *f = hd_art ? cd_open_own(n, ext, from, size) : NULL;
    if (f)
        return f;
    if ((f = cd_track_file(n, *ext = "ogg")) || (f = cd_track_file(n, *ext = "wav"))) {
        if (from)
            snprintf(from, size, "Music\\Track%02d.%s", n, *ext);
        return f;
    }
    return hd_art ? NULL : cd_open_own(n, ext, from, size);
}

static int cd_count_tracks(void)
{
    int n, last = 0;
    if (!cd_virtual && ((plat_replay && !getenv("CIV2_TEST_AUDIO")) || !audio_ok))
        return 0;
    for (n = 2; n < 100; n++) {
        const char *ext;
        FILE *f = cd_open_track(n, &ext, NULL, 0);
        if (!f)
            break;
        fclose(f);
        last = n;
    }
    return last;
}

static void cd_notify(UINT code)
{
    HWND h = cd.notify;
    cd.notify = 0;
    if (h)
        PostMessageA(h, MM_MCINOTIFY, code, CD_ID);
}

static void cd_stop(UINT code)
{
    if (cd.playing) {
        plat_log("cdaudio: track %d stopped\n", cd.current);
        if (cd_virtual)
            replay_log_music("stop", cd.current);
    }
    cd.playing = 0;
    if (!cd_virtual)
        host_music_stop();
    cd_notify(code);
}

/* Track n (Music\TrackNN.ogg, else .wav, else OpenCiv2's own: cd_open_track)
   read whole and handed to the host, which decodes it as it plays (on a
   thread of its own: also while a system menu holds the message loop); its
   size, 0 if it did not start; from: where it is. */
static long cd_play(int n, const char **ext, char *from, size_t size)
{
    FILE *f = cd_open_track(n, ext, from, size);
    long len = -1;
    void *data = NULL;
    int ok = 0;
    if (!f)
        return 0;
    if (cd_virtual) {
        DWORD ms = cd_track_ms(f, *ext);
        fseek(f, 0, SEEK_END);
        len = ftell(f);
        fclose(f);
        cd.playing = 1;
        cd.current = n;
        cd.end_ms = plat_now() + ms;
        replay_log_music("play", n);
        return len;
    }
    if (fseek(f, 0, SEEK_END) == 0 && (len = ftell(f)) > 0 && fseek(f, 0, SEEK_SET) == 0 &&
        (data = malloc((size_t)len)) && fread(data, 1, (size_t)len, f) == (size_t)len)
        ok = host_music_play(data, (uint32_t)len);
    free(data);
    fclose(f);
    if (!ok)
        return 0;
    cd.playing = 1;
    cd.current = n;
    return len;
}

/* From the message loop: at the track's end, tell the window that asked
 * (MCI_NOTIFY_SUCCESSFUL). */
static void cd_poll(void)
{
    if (cd.playing && !cd_sounding()) {
        plat_log("cdaudio: track %d ended\n", cd.current);
        if (cd_virtual)
            replay_log_music("end", cd.current);
        cd.playing = 0;
        cd_notify(MCI_NOTIFY_SUCCESSFUL);
    }
}

static MCIERROR cd_command(UINT msg, DWORD_PTR flags, DWORD_PTR param)
{
    switch (msg) {
    case MCI_CLOSE:
        cd_stop(MCI_NOTIFY_ABORTED);
        cd.open = 0;
        return 0;
    case MCI_SET:                       /* time format (tracks), door: nothing to do */
        return 0;
    case MCI_STATUS: {
        MCI_STATUS_PARMS *p = (MCI_STATUS_PARMS *)param;
        if (!(flags & MCI_STATUS_ITEM))
            return MCIERR_MISSING_PARAMETER;
        switch (p->dwItem) {
        case MCI_STATUS_NUMBER_OF_TRACKS: p->dwReturn = (DWORD_PTR)cd.tracks; return 0;
        case MCI_STATUS_CURRENT_TRACK: p->dwReturn = (DWORD_PTR)(cd.current ? cd.current : 1); return 0;
        case MCI_STATUS_MODE: p->dwReturn = cd_sounding() ? MCI_MODE_PLAY : MCI_MODE_STOP; return 0;
        case MCI_STATUS_READY: p->dwReturn = TRUE; return 0;
        case MCI_STATUS_MEDIA_PRESENT: p->dwReturn = TRUE; return 0;
        }
        return MCIERR_UNSUPPORTED_FUNCTION;
    }
    case MCI_SEEK: {
        MCI_SEEK_PARMS *p = (MCI_SEEK_PARMS *)param;
        cd_stop(MCI_NOTIFY_ABORTED);
        if (flags & MCI_TO)
            cd.current = (int)(p->dwTo & 0xff);
        return 0;
    }
    case MCI_STOP:
    case MCI_PAUSE:
        cd_stop(MCI_NOTIFY_ABORTED);
        return 0;
    case MCI_PLAY: {
        MCI_PLAY_PARMS *p = (MCI_PLAY_PARMS *)param;
        int n = (flags & MCI_FROM) ? (int)(p->dwFrom & 0xff) : (cd.current ? cd.current : 2);
        const char *ext;
        char from[1100];
        long len;
        cd_stop(MCI_NOTIFY_SUPERSEDED);
        if (n < 2 || n > cd.tracks || !(len = cd_play(n, &ext, from, sizeof from))) {
            plat_log("cdaudio: cannot play track %d (of %d)\n", n, cd.tracks);
            return MCIERR_OUTOFRANGE;
        }
        if (flags & MCI_NOTIFY)
            cd.notify = (HWND)p->dwCallback;
        plat_log("cdaudio: playing track %d%s from %s (%ld bytes)\n", n,
                 (flags & MCI_NOTIFY) ? " (notify)" : "", from, len);
        return 0;
    }
    }
    return MCIERR_UNSUPPORTED_FUNCTION;
}

MCIERROR WINAPI mciSendCommandA(MCIDEVICEID id, UINT msg, DWORD_PTR flags, DWORD_PTR param)
{ API_TRACE();
    if (msg == MCI_OPEN) {
        MCI_OPEN_PARMSA *p = (MCI_OPEN_PARMSA *)param;
        if ((flags & MCI_OPEN_TYPE) && !(flags & MCI_OPEN_TYPE_ID) && p && p->lpstrDeviceType &&
            !strcasecmp(p->lpstrDeviceType, "cdaudio")) {
            if (!cd.tracks)
                cd.tracks = cd_count_tracks();
            if (!cd.tracks)
                return MCIERR_DEVICE_NOT_INSTALLED;
            cd.open = 1;
            p->wDeviceID = CD_ID;
            return 0;
        }
        return MCIERR_DEVICE_NOT_INSTALLED;
    }
    if (id == CD_ID && cd.open)
        return cd_command(msg, flags, param);
    if (msg == MCI_CLOSE)
        return 0;
    return MCIERR_INVALID_DEVICE_ID;
}

MCIDEVICEID WINAPI mciGetDeviceIDA(LPCSTR name) { API_TRACE(); (void)name; return 0; }
