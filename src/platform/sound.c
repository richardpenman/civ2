/*
 * Sound: waveOut over the host's audio (src/host), sndPlaySound, mmio (RIFF files), MCI.
 *
 * In replay mode there is no audio device, as in the reference environment
 * (an X server in a container, without sound): the game then runs without
 * sound, identically in both.
 */
#include "plat.h"
#include "../host/host.h"
#include "plat_files.h"

static int audio_ok;
static void cd_poll(void);

void sound_init(void)
{
    /* CIV2_TEST_AUDIO: audio in a replay too (tests of the music; with
       SDL_AUDIODRIVER=dummy where there is no sound card) */
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
    if (!audio_ok)
        return;
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
 * (tools/extract_disc.py writes them from a disc image); without those
 * files there is no drive, as before.  Replays have no drive either (as the
 * reference environment).  Other MCI devices (avivideo) are not available. */

#define CD_ID 0x4344            /* the drive's device id */

static struct {
    int tracks;                 /* 0: no drive (no music files) */
    int open, current;
    int playing;                /* a track was started (the host plays it: host_music_play) */
    HWND notify;                /* MCI_NOTIFY on the play: told when it ends */
} cd;

static FILE *cd_track_file(int n, const char *ext)
{
    char win[64], host[1024];
    snprintf(win, sizeof win, "Music\\Track%02d.%s", n, ext);
    plat_host_path(win, host, sizeof host, 0);
    return fopen(host, "rb");
}

static int cd_count_tracks(void)
{
    int n, last = 0;
    if ((plat_replay && !getenv("CIV2_TEST_AUDIO")) || !audio_ok)
        return 0;
    for (n = 2; n < 100; n++) {
        FILE *f = cd_track_file(n, "ogg");
        if (!f && !(f = cd_track_file(n, "wav")))
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
    if (cd.playing)
        plat_log("cdaudio: track %d stopped\n", cd.current);
    cd.playing = 0;
    host_music_stop();
    cd_notify(code);
}

/* Track n (Music\TrackNN.ogg, else .wav) read whole and handed to the host,
   which decodes it as it plays (on a thread of its own: also while a system
   menu holds the message loop); its size, 0 if it did not start. */
static long cd_play(int n, const char **ext)
{
    FILE *f = cd_track_file(n, *ext = "ogg");
    long len = -1;
    void *data = NULL;
    int ok = 0;
    if (!f)
        f = cd_track_file(n, *ext = "wav");
    if (!f)
        return 0;
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
    if (cd.playing && !host_music_playing()) {
        plat_log("cdaudio: track %d ended\n", cd.current);
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
        case MCI_STATUS_MODE: p->dwReturn = cd.playing && host_music_playing() ? MCI_MODE_PLAY : MCI_MODE_STOP; return 0;
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
        long len;
        cd_stop(MCI_NOTIFY_SUPERSEDED);
        if (n < 2 || n > cd.tracks || !(len = cd_play(n, &ext))) {
            plat_log("cdaudio: cannot play track %d (of %d)\n", n, cd.tracks);
            return MCIERR_OUTOFRANGE;
        }
        if (flags & MCI_NOTIFY)
            cd.notify = (HWND)p->dwCallback;
        plat_log("cdaudio: playing track %d%s from Music\\Track%02d.%s (%ld bytes)\n", n,
                 (flags & MCI_NOTIFY) ? " (notify)" : "", n, ext, len);
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
