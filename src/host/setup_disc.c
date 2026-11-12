/*
 * The set-up from a disc image of the game's CD (setup.c): a .cue sheet
 * with its .bin file(s), raw 2352-byte sectors, the data track first
 * (MODE1/2352: an ISO 9660 file system), then the music as AUDIO tracks
 * (44.1 kHz 16-bit stereo).  From the data track the installer's cabinet
 * and the videos, as from the CD; the audio tracks encoded to Ogg Vorbis
 * (libvorbis) as Music/TrackNN.ogg: track N of the CD, which the game
 * plays (src/platform/sound.c).  As tools/extract_disc.py does.  The
 * same encoding for the music of the CD itself, its audio sectors read from
 * its drive (os_cd_open, os_cd_read: setup_win.c, setup_unix.c).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisenc.h>
#include "setup.h"
#include "setup_os.h"

#ifdef _WIN32
#define fseek64 _fseeki64
#define ftell64 _ftelli64
#else
#define fseek64 fseeko
#define ftell64 ftello
#endif

#define RAW 2352                /* a CD sector, as a .bin holds it */
#define ISO_SECTOR 2048         /* a data sector's user data */
#define MAX_FILES 99
#define MUSIC_QUALITY 0.4f      /* libvorbis's VBR quality: about 128 kbit/s */

struct track {
    int number, file;           /* the TRACK's number, the FILE it is in */
    int audio;                  /* AUDIO, else data */
    int size, offset;           /* bytes per sector in the file, of them before the user data */
    long long index0, index1;   /* where its INDEX 00 (-1: none) and 01 are in the file, in sectors */
    long long end;              /* where it ends in the file, in sectors */
};

struct disc {
    int ntracks, nfiles;
    struct track t[99];
    FILE *f[MAX_FILES];
    const struct track *data;   /* the data track (ISO 9660) */
};

/* An entry of the data track's file system. */
struct iso_entry {
    unsigned extent, size;
    int is_dir;
    char name[256];
};

static void disc_close(struct disc *d)
{
    int i;
    for (i = 0; i < d->nfiles; i++)
        fclose(d->f[i]);
    free(d);
}

/* "mm:ss:ff" in sectors (75 a second); -1: not that. */
static long long msf(const char *s)
{
    int m, sec, fr;
    return sscanf(s, "%d:%d:%d", &m, &sec, &fr) == 3 ? ((long long)m * 60 + sec) * 75 + fr : -1;
}

static int has_suffix(const char *s, const char *suffix)
{
    size_t n = strlen(s), k = strlen(suffix);
    size_t i;
    if (n < k)
        return 0;
    for (i = 0; i < k; i++)
        if ((s[n - k + i] | 0x20) != (suffix[i] | 0x20))
            return 0;
    return 1;
}

static void upper(char *s)
{
    for (; *s; s++)
        if (*s >= 'a' && *s <= 'z')
            *s = (char)(*s - 'a' + 'A');
}

/* The image of a .cue sheet; NULL: not one (or its files are missing). */
static struct disc *disc_open(const char *cue)
{
    char line[1024], dir[SETUP_PATH], *slash;
    struct disc *d;
    FILE *c;
    int i, file = -1;
    if (!has_suffix(cue, ".cue") || !(c = fopen(cue, "r")))
        return NULL;
    d = calloc(1, sizeof *d);
    snprintf(dir, sizeof dir, "%s", cue);
    slash = strrchr(dir, SEP);
#ifdef _WIN32
    if (strrchr(dir, '/') > slash)
        slash = strrchr(dir, '/');
#endif
    if (slash)
        *slash = 0;
    else
        snprintf(dir, sizeof dir, ".");
    while (fgets(line, sizeof line, c)) {
        char *w = line, word[16], arg[1024];
        if (!strncmp(w, "\xef\xbb\xbf", 3))
            w += 3;
        if (sscanf(w, "%15s", word) != 1)
            continue;
        w = strstr(w, word) + strlen(word);
        upper(word);
        while (*w == ' ' || *w == '\t')
            w++;
        if (!strcmp(word, "FILE")) {
            char path[SETUP_PATH];
            if (*w == '"') {
                char *q = strchr(w + 1, '"');
                if (!q)
                    break;
                *q = 0;
                snprintf(arg, sizeof arg, "%s", w + 1);
            } else if (sscanf(w, "%1023s", arg) != 1) {
                break;
            }
            /* (the name may have a folder, or another case than the file's) */
            for (slash = arg; *slash; slash++)
                if (*slash == '/' || *slash == '\\')
                    *slash = SEP;
            if (d->nfiles == MAX_FILES || !os_entry(dir, arg, path, sizeof path) ||
                !(d->f[d->nfiles] = fopen(path, "rb")))
                goto fail;
            file = d->nfiles++;
        } else if (!strcmp(word, "TRACK")) {
            struct track *t;
            int n;
            if (file < 0 || d->ntracks == 99 || sscanf(w, "%d %1023s", &n, arg) != 2)
                goto fail;
            upper(arg);
            t = &d->t[d->ntracks++];
            t->number = n;
            t->file = file;
            t->index0 = t->index1 = -1;
            t->audio = !strcmp(arg, "AUDIO");
            t->size = RAW;
            if (!strcmp(arg, "MODE1/2048"))
                t->size = ISO_SECTOR;
            else if (!strcmp(arg, "MODE1/2352"))
                t->offset = 16;
            else if (!strcmp(arg, "MODE2/2352"))
                t->offset = 24;
            else if (!t->audio)
                t->size = 0;        /* (not readable here) */
        } else if (!strcmp(word, "INDEX") && d->ntracks) {
            int k;
            if (sscanf(w, "%d %1023s", &k, arg) == 2 && (k == 0 || k == 1))
                *(k ? &d->t[d->ntracks - 1].index1 : &d->t[d->ntracks - 1].index0) = msf(arg);
        }
    }
    fclose(c);
    c = NULL;
    /* each track to the next one in its file (its INDEX 00, the gap
       before it, else 01), the last to the file's end */
    for (i = 0; i < d->ntracks; i++) {
        struct track *t = &d->t[i];
        if (t->index1 < 0 || !t->size)
            goto fail;
        if (i + 1 < d->ntracks && d->t[i + 1].file == t->file) {
            t->end = d->t[i + 1].index0 >= 0 ? d->t[i + 1].index0 : d->t[i + 1].index1;
        } else {
            fseek64(d->f[t->file], 0, SEEK_END);
            t->end = (long long)ftell64(d->f[t->file]) / t->size;
        }
        if (t->end < t->index1)
            goto fail;
        if (!t->audio && !d->data)
            d->data = t;
    }
    if (d->data)
        return d;
fail:
    if (c)
        fclose(c);
    disc_close(d);
    return NULL;
}

/* ---- the data track's ISO 9660 file system ---- */

static unsigned le32(const unsigned char *p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | (unsigned)p[3] << 24;
}

/* n sectors' user data from the data track's sector lba on, into buf. */
static int read_sectors(struct disc *d, unsigned lba, unsigned char *buf, unsigned n)
{
    static unsigned char raw[32 * RAW];
    const struct track *t = d->data;
    FILE *f = d->f[t->file];
    while (n) {
        unsigned k = n < 32 ? n : 32, i;
        if (t->index1 + lba + k > t->end || fseek64(f, (t->index1 + lba) * t->size, SEEK_SET) ||
            fread(raw, (size_t)t->size, k, f) != k)
            return 0;
        for (i = 0; i < k; i++)
            memcpy(buf + (size_t)i * ISO_SECTOR, raw + (size_t)i * t->size + t->offset, ISO_SECTOR);
        buf += (size_t)k * ISO_SECTOR;
        lba += k;
        n -= k;
    }
    return 1;
}

static int same_name(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
        if ((*a >= 'a' && *a <= 'z' ? *a - 32 : *a) != (*b >= 'a' && *b <= 'z' ? *b - 32 : *b))
            return 0;
    return *a == *b;
}

/* The root folder. */
static int iso_root(struct disc *d, struct iso_entry *e)
{
    unsigned char pvd[ISO_SECTOR];
    if (!read_sectors(d, 16, pvd, 1) || pvd[0] != 1 || memcmp(pvd + 1, "CD001", 5))
        return 0;
    memset(e, 0, sizeof *e);
    e->extent = le32(pvd + 156 + 2);
    e->size = le32(pvd + 156 + 10);
    e->is_dir = 1;
    return 1;
}

/* Each entry of folder dir to fn until it returns 0; 0: stopped (or unreadable). */
static int iso_each(struct disc *d, const struct iso_entry *dir, int (*fn)(void *ctx, const struct iso_entry *e),
                    void *ctx)
{
    unsigned char sec[ISO_SECTOR];
    unsigned off;
    for (off = 0; off < dir->size; off += ISO_SECTOR) {
        unsigned i = 0;
        if (!read_sectors(d, dir->extent + off / ISO_SECTOR, sec, 1))
            return 0;
        while (i + 33 < ISO_SECTOR && sec[i]) {
            unsigned len = sec[i], nl = sec[i + 32];
            struct iso_entry e;
            char *c;
            if (i + len > ISO_SECTOR || 33 + nl > len)
                break;
            if (!(nl == 1 && sec[i + 33] <= 1)) {           /* (not . and ..) */
                e.extent = le32(sec + i + 2);
                e.size = le32(sec + i + 10);
                e.is_dir = (sec[i + 25] & 2) != 0;
                memcpy(e.name, sec + i + 33, nl);
                e.name[nl] = 0;
                if ((c = strchr(e.name, ';')))             /* NAME.EXT;1 */
                    *c = 0;
                if ((c = e.name + strlen(e.name)) > e.name && c[-1] == '.')
                    c[-1] = 0;
                if (!fn(ctx, &e))
                    return 0;
            }
            i += len;
        }
    }
    return 1;
}

struct find {
    const char *name;
    struct iso_entry *out;
};

static int find_one(void *ctx, const struct iso_entry *e)
{
    struct find *f = ctx;
    if (!same_name(e->name, f->name))
        return 1;
    *f->out = *e;
    return 0;
}

/* The file or folder at path ("CIV2/KINGS"), whatever its case; 0: none. */
static int iso_find(struct disc *d, const char *path, struct iso_entry *e)
{
    char part[256];
    if (!iso_root(d, e))
        return 0;
    while (*path) {
        size_t n = strcspn(path, "/");
        struct find f = {part, e};
        if (!e->is_dir || n >= sizeof part)
            return 0;
        memcpy(part, path, n);
        part[n] = 0;
        if (iso_each(d, e, find_one, &f))
            return 0;                   /* (not found) */
        path += n + (path[n] == '/');
    }
    return 1;
}

static int iso_extract(struct disc *d, const struct iso_entry *e, const char *dst)
{
    static unsigned char buf[32 * ISO_SECTOR];
    unsigned done = 0;
    FILE *out = fopen(dst, "wb");
    int ok = out != NULL;
    while (ok && done < e->size) {
        unsigned n = e->size - done < sizeof buf ? e->size - done : (unsigned)sizeof buf;
        ok = read_sectors(d, e->extent + done / ISO_SECTOR, buf, (n + ISO_SECTOR - 1) / ISO_SECTOR) &&
             fwrite(buf, 1, n, out) == n;
        done += n;
    }
    if (out && fclose(out) != 0)
        ok = 0;
    if (!ok) {
        char text[SETUP_PATH + 64];
        remove(dst);
        snprintf(text, sizeof text, "Could not copy %s from the disc image.", e->name);
        os_message(text, OS_ERROR);
    }
    return ok;
}

/* ---- what the set-up asks of an image (setup_os.h) ---- */

int setup_game_image(const char *path)
{
    struct disc *d = disc_open(path);
    struct iso_entry e;
    int ok = d && iso_find(d, "DATA1.CAB", &e) && !e.is_dir && iso_find(d, "CIV2", &e) && e.is_dir;
    if (d)
        disc_close(d);
    return ok;
}

struct in_folder {
    const char *dir;
    int depth;
    char *out;
    size_t size;
};

static int image_entry(void *ctx, const char *name, int is_dir)
{
    const struct in_folder *f = ctx;
    char p[SETUP_PATH];
    snprintf(p, sizeof p, "%s" SEP_S "%s", f->dir, name);
    if (is_dir)
        return !(f->depth > 0 && setup_image_in(p, f->depth - 1, f->out, f->size));
    if (has_suffix(name, ".cue") && setup_game_image(p)) {
        snprintf(f->out, f->size, "%s", p);
        return 0;
    }
    return 1;
}

int setup_image_in(const char *dir, int depth, char *out, size_t size)
{
    struct in_folder f = {dir, depth, out, size};
    return *dir && os_dir_exists(dir) && !os_each(dir, image_entry, &f);
}

/* (the files taken out of the image, to remove afterwards) */
struct cabinet {
    struct disc *d;
    const char *dir;
    int ok;
};

/* The cabinet's files (DATA1.CAB, other volumes DATAn.CAB, headers
   DATAn.HDR: unshield opens them by name, beside the first). */
static int cabinet_file(void *ctx, const struct iso_entry *e)
{
    struct cabinet *c = ctx;
    char dst[SETUP_PATH];
    size_t n = strlen(e->name);
    char prefix[5];
    snprintf(prefix, sizeof prefix, "%s", e->name);
    if (e->is_dir || n < 8 || !same_name(prefix, "DATA") ||
        !(same_name(e->name + n - 4, ".CAB") || same_name(e->name + n - 4, ".HDR")))
        return 1;
    snprintf(dst, sizeof dst, "%s" SEP_S "%s", c->dir, e->name);
    if (os_progress_show("Copying from the disc image", dst))
        return c->ok = 0;
    return c->ok = iso_extract(c->d, e, dst);
}

int image_extract_cabinet(const char *cue, const char *dir, char *cab, size_t size)
{
    struct disc *d = disc_open(cue);
    struct iso_entry root;
    struct cabinet c;
    if (!d || !iso_root(d, &root)) {
        char text[SETUP_PATH + 64];
        snprintf(text, sizeof text, "Could not read %s.", cue);
        os_message(text, OS_ERROR);
        if (d)
            disc_close(d);
        return 0;
    }
    c.d = d;
    c.dir = dir;
    c.ok = 1;
    os_make_dirs(dir);
    iso_each(d, &root, cabinet_file, &c);
    disc_close(d);
    snprintf(cab, size, "%s" SEP_S "DATA1.CAB", dir);
    return c.ok && os_file_exists(cab);
}

struct videos {
    struct disc *d;
    char dir[SETUP_PATH];
    int ok;
};

static int video_file(void *ctx, const struct iso_entry *e)
{
    struct videos *v = ctx;
    char dst[SETUP_PATH];
    if (e->is_dir)
        return 1;
    snprintf(dst, sizeof dst, "%s" SEP_S "%s", v->dir, e->name);
    if (os_progress_show("Copying from the disc image", dst))
        return v->ok = 0;
    return v->ok = iso_extract(v->d, e, dst);
}

int image_copy_videos(const char *cue, const char *game_dir)
{
    static const char *const folders[] = {"KINGS", "VIDEO"};
    struct disc *d = disc_open(cue);
    struct iso_entry e;
    size_t i;
    int ok = 1;
    if (!d)
        return 1;
    for (i = 0; ok && i < sizeof folders / sizeof folders[0]; i++) {
        struct videos v;
        char path[64];
        snprintf(path, sizeof path, "CIV2/%s", folders[i]);
        snprintf(v.dir, sizeof v.dir, "%s" SEP_S "%s", game_dir, folders[i]);
        if (!iso_find(d, path, &e) || !e.is_dir || os_entry(game_dir, folders[i], v.dir, sizeof v.dir))
            continue;
        v.d = d;
        v.ok = 1;
        os_make_dirs(v.dir);
        iso_each(d, &e, video_file, &v);
        ok = v.ok;
    }
    disc_close(d);
    return ok;
}

/* ---- the music ---- */

/* Bytes to file: 0: could not write. */
static int put(FILE *f, const void *p, long n)
{
    return n <= 0 || fwrite(p, 1, (size_t)n, f) == (size_t)n;
}

static int write_page(FILE *f, const ogg_page *og)
{
    return put(f, og->header, og->header_len) && put(f, og->body, og->body_len);
}

/* The raw audio sectors (2352 bytes: 588 frames of 16-bit little-endian
   stereo) of a disc image's .bin or of the CD in its drive. */
typedef int (*read_audio_fn)(void *ctx, long long at, int n, unsigned char *buf);

/* n sectors from sector at on of an image's file (ctx). */
static int image_read_audio(void *ctx, long long at, int n, unsigned char *buf)
{
    FILE *f = ctx;
    return !fseek64(f, at * RAW, SEEK_SET) && fread(buf, RAW, (size_t)n, f) == (size_t)n;
}

/* n sectors from the drive (ctx: os_cd_open's), each chunk tried again
   when the drive misreads it. */
static int cd_read_audio(void *ctx, long long at, int n, unsigned char *buf)
{
    int tries;
    for (tries = 0; tries < 3; tries++)
        if (os_cd_read(ctx, (long)at, n, buf))
            return 1;
    return 0;
}

enum { ENC_STOPPED = -1, ENC_UNWRITTEN = 0, ENC_DONE = 1, ENC_UNREAD = 2 };

/* Sectors start to end (read) as Ogg Vorbis into dst (written as dst.part,
   then renamed), the CD's track number; what: the progress shown; from:
   the ENCODER tag.  ENC_*. */
static int encode_track(read_audio_fn read, void *ctx, int number, long long start, long long end, const char *dst,
                        const char *what, const char *from)
{
    enum { CHUNK = 16 };                    /* sectors read at a time: 588 frames each */
    static unsigned char raw[CHUNK * RAW];
    FILE *out;
    char part[SETUP_PATH], detail[SETUP_PATH + 16];
    vorbis_info vi;
    vorbis_comment vc;
    vorbis_dsp_state vd;
    vorbis_block vb;
    ogg_stream_state os;
    ogg_page og;
    ogg_packet op, comm, code;
    long long at = start, total = end - start;
    int ok = ENC_DONE, eos = 0, shown = -1;
    snprintf(part, sizeof part, "%s.part", dst);
    if (!(out = fopen(part, "wb")))
        return ENC_UNWRITTEN;
    vorbis_info_init(&vi);
    if (vorbis_encode_init_vbr(&vi, 2, 44100, MUSIC_QUALITY)) {
        vorbis_info_clear(&vi);
        fclose(out);
        remove(part);
        return ENC_UNWRITTEN;
    }
    vorbis_comment_init(&vc);
    vorbis_comment_add_tag(&vc, "ENCODER", from);
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);
    ogg_stream_init(&os, number);
    vorbis_analysis_headerout(&vd, &vc, &op, &comm, &code);
    ogg_stream_packetin(&os, &op);
    ogg_stream_packetin(&os, &comm);
    ogg_stream_packetin(&os, &code);
    while (ok == ENC_DONE && ogg_stream_flush(&os, &og))
        ok = write_page(out, &og) ? ENC_DONE : ENC_UNWRITTEN;
    while (ok == ENC_DONE && !eos) {
        long long k = end - at < CHUNK ? end - at : CHUNK;
        int pct = (int)(total ? (at - start) * 10 / total : 10) * 10;
        long frames = 0, i;
        if (pct != shown) {
            snprintf(detail, sizeof detail, "%s (%d%%)", dst, pct);
            shown = pct;
        }
        /* (every chunk: the Windows progress window answers, can stop it) */
        if (os_progress_show(what, detail)) {
            ok = ENC_STOPPED;
            break;
        }
        if (k > 0) {
            float **buf;
            if (!read(ctx, at, (int)k, raw)) {
                ok = ENC_UNREAD;
                break;
            }
            at += k;
            frames = (long)k * (RAW / 4);
            buf = vorbis_analysis_buffer(&vd, (int)frames);
            for (i = 0; i < frames; i++) {        /* (16-bit little-endian stereo) */
                const unsigned char *s = raw + i * 4;
                buf[0][i] = (short)(s[0] | s[1] << 8) / 32768.f;
                buf[1][i] = (short)(s[2] | s[3] << 8) / 32768.f;
            }
        }
        vorbis_analysis_wrote(&vd, (int)frames);       /* (0: the end) */
        while (ok == ENC_DONE && vorbis_analysis_blockout(&vd, &vb) == 1) {
            vorbis_analysis(&vb, NULL);
            vorbis_bitrate_addblock(&vb);
            while (ok == ENC_DONE && vorbis_bitrate_flushpacket(&vd, &op)) {
                ogg_stream_packetin(&os, &op);
                while (ok == ENC_DONE && ogg_stream_pageout(&os, &og)) {
                    ok = write_page(out, &og) ? ENC_DONE : ENC_UNWRITTEN;
                    if (ogg_page_eos(&og))
                        eos = 1;
                }
            }
        }
    }
    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
    if (fclose(out) != 0 && ok == ENC_DONE)
        ok = ENC_UNWRITTEN;
    if (ok == ENC_DONE) {
        remove(dst);                /* (Windows: rename does not replace) */
        if (rename(part, dst))
            ok = ENC_UNWRITTEN;
    }
    if (ok != ENC_DONE)
        remove(part);
    return ok;
}

/* The audio tracks of a disc (an image's, the CD's): its track numbers,
   their sectors (start to end), where they are read from. */
struct audio {
    int n;
    struct {
        int number;
        long long start, end;
        void *ctx;
    } t[99];
    read_audio_fn read;
    const char *from;           /* the ENCODER tag */
};

/* Its tracks as game_dir/Music/TrackNN.ogg.  ENC_*: on ENC_UNWRITTEN, said. */
static int encode_music(const struct audio *a, const char *game_dir)
{
    char music[SETUP_PATH], dst[SETUP_PATH], what[64];
    int i, ok = ENC_DONE;
    if (!a->n)
        return ENC_DONE;
    if (!os_entry(game_dir, "Music", music, sizeof music))
        snprintf(music, sizeof music, "%s" SEP_S "Music", game_dir);
    os_make_dirs(music);
    for (i = 0; ok == ENC_DONE && i < a->n; i++) {
        snprintf(what, sizeof what, "Encoding the music (track %d of %d)", i + 1, a->n);
        snprintf(dst, sizeof dst, "%s" SEP_S "Track%02d.ogg", music, a->t[i].number);
        ok = encode_track(a->read, a->t[i].ctx, a->t[i].number, a->t[i].start, a->t[i].end, dst, what, a->from);
        if (ok == ENC_UNWRITTEN) {
            char text[SETUP_PATH + 64];
            snprintf(text, sizeof text, "Could not write %s.", dst);
            os_message(text, OS_ERROR);
        }
    }
    return ok;
}

/* --CueSheetFuzzyMatch: a cue sheet's INDEX 01 marks can be off by a
 * couple of seconds (e.g. late by the 2 s gap between tracks, with no
 * INDEX 00), so that a track's file ends with the next one's opening.  Each
 * boundary between two audio tracks that follow each other in one file is
 * moved to the quietest sector within FUZZY_WINDOW of it (of equally quiet
 * ones, the nearest), if that is silent (FUZZY_SILENT) and the sector
 * before the mark is not already: a cue sheet that is right is read as it
 * is.  Each boundary's outcome is told on stderr. */
#define FUZZY_WINDOW 225        /* sectors: 3 s */
#define FUZZY_SILENT 1074LL     /* mean square of a sector's samples: -60 dBFS (32.77^2) */

/* Mean square of a sector's 1176 samples (16-bit little-endian). */
static long long sector_power(const unsigned char *s)
{
    long long sum = 0;
    int i;
    for (i = 0; i < RAW / 2; i++) {
        long v = (short)(s[2 * i] | s[2 * i + 1] << 8);
        sum += (long long)v * v;
    }
    return sum / (RAW / 2);
}

static void fuzzy_boundaries(struct audio *a)
{
    int i;
    for (i = 0; i + 1 < a->n; i++) {
        long long b = a->t[i].end, lo = b - FUZZY_WINDOW, hi = b + FUZZY_WINDOW, at, best = -1, best_p = 0;
        unsigned char *buf;
        if (a->t[i + 1].ctx != a->t[i].ctx || a->t[i + 1].start != b)
            continue;
        if (lo < a->t[i].start + 1)
            lo = a->t[i].start + 1;             /* (each track keeps a sector) */
        if (hi > a->t[i + 1].end - 1)
            hi = a->t[i + 1].end - 1;
        if (hi <= lo || !(buf = malloc((size_t)(hi - lo) * RAW)))
            continue;
        if (!a->read(a->t[i].ctx, lo, (int)(hi - lo), buf)) {
            free(buf);
            continue;
        }
        if (b - 1 >= lo && sector_power(buf + (b - 1 - lo) * RAW) < FUZZY_SILENT) {
            fprintf(stderr, "setup: CueSheetFuzzyMatch: tracks %d/%d: boundary kept (silent already)\n",
                    a->t[i].number, a->t[i + 1].number);
            free(buf);
            continue;
        }
        for (at = lo; at < hi; at++) {
            long long p = sector_power(buf + (at - lo) * RAW);
            long long dist = at < b ? b - at : at - b, best_dist = best < b ? b - best : best - b;
            if (best < 0 || p < best_p || (p == best_p && dist < best_dist)) {
                best = at;
                best_p = p;
            }
        }
        free(buf);
        if (best < 0 || best_p >= FUZZY_SILENT) {
            fprintf(stderr, "setup: CueSheetFuzzyMatch: tracks %d/%d: boundary kept (no silence near it)\n",
                    a->t[i].number, a->t[i + 1].number);
            continue;
        }
        fprintf(stderr, "setup: CueSheetFuzzyMatch: tracks %d/%d: boundary moved %+lld sectors (%+.2f s)\n",
                a->t[i].number, a->t[i + 1].number, best - b, (best - b) / 75.0);
        a->t[i].end = a->t[i + 1].start = best;
    }
}

int image_encode_music(const char *cue, const char *game_dir)
{
    struct disc *d = disc_open(cue);
    static struct audio a;
    int i, ok;
    if (!d)
        return 1;
    a.n = 0;
    a.read = image_read_audio;
    a.from = "OpenCiv2 set-up, from the player's disc image";
    for (i = 0; i < d->ntracks; i++) {
        const struct track *t = &d->t[i];
        if (!t->audio)
            continue;
        a.t[a.n].number = t->number;
        a.t[a.n].start = t->index1;
        a.t[a.n].end = t->end;
        a.t[a.n].ctx = d->f[t->file];
        a.n++;
    }
    if (setup_has_flag("--CueSheetFuzzyMatch"))
        fuzzy_boundaries(&a);
    ok = encode_music(&a, game_dir);
    if (ok == ENC_UNREAD) {
        char text[SETUP_PATH + 64];
        snprintf(text, sizeof text, "Could not read %s.", cue);
        os_message(text, OS_ERROR);
    }
    disc_close(d);
    return ok == ENC_DONE;
}

/* ---- the music from the CD in its drive ---- */

/* The CD's audio tracks, each to the next track (the last to the lead-out). */
static void cd_audio(const struct cd_toc *toc, void *h, struct audio *a)
{
    int i;
    a->n = 0;
    a->read = cd_read_audio;
    a->from = "OpenCiv2 set-up, from the player's CD";
    for (i = 0; i < toc->ntracks; i++) {
        long end = i + 1 < toc->ntracks ? toc->t[i + 1].lba : toc->leadout;
        if (!toc->t[i].audio || end <= toc->t[i].lba)
            continue;
        a->t[a->n].number = toc->t[i].number;
        a->t[a->n].start = toc->t[i].lba;
        a->t[a->n].end = end;
        a->t[a->n].ctx = h;
        a->n++;
    }
}

int cd_has_music(const char *cd)
{
    static struct cd_toc toc;
    static struct audio a;
    void *h;
    unsigned char *probe;
    int r = os_cd_open(cd, &toc, &h);
    if (r <= 0)
        return r;
    cd_audio(&toc, h, &a);
    r = a.n > 0;
    /* (the drive reads audio: its first sector) */
    if (r && (probe = malloc(RAW))) {
        if (!cd_read_audio(h, a.t[0].start, 1, probe))
            r = -1;
        free(probe);
    }
    os_cd_close(h);
    return r;
}

int cd_encode_music(const char *cd, const char *game_dir)
{
    static struct cd_toc toc;
    static struct audio a;
    void *h;
    int ok, r = os_cd_open(cd, &toc, &h);
    if (r <= 0)
        return r ? -1 : 1;
    cd_audio(&toc, h, &a);
    ok = encode_music(&a, game_dir);
    os_cd_close(h);
    return ok == ENC_UNREAD ? -1 : ok == ENC_DONE;
}
