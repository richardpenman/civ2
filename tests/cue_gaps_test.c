/*
 * The set-up's music from a disc image whose cue sheet is a second or two
 * off (src/host/setup_disc.c's image_encode_music, setup_fix_cue_gaps): a
 * made-up image of four audio tracks after its data track in one .bin, cut where the cue sheet
 * says, then with each boundary moved to the silence nearby.
 *
 *   track 1: the data track (0..299, not read)
 *   track 3: the cue sheet's mark 150 sectors late, the silence before
 *            the track 1000..1049: moved to 1049 (the silent sector nearest
 *            the mark), -151 sectors
 *   track 4: the mark right, silence just before it: kept
 *   track 5: no silence near the mark: kept
 *
 *   tools/dk sh -c 'T=tools/toolchain; V=$T/libvorbis/lib; cc -O2 \
 *     -DHAVE_ALLOCA_H -Isrc/host -I$T/libogg/include -I$T/libvorbis/include \
 *     -I$V -o build/cue_gaps_test tests/cue_gaps_test.c src/host/setup_disc.c \
 *     $T/libogg/src/bitwise.c $T/libogg/src/framing.c $(for f in analysis \
 *     bitrate block codebook envelope floor0 floor1 info lookup lpc lsp \
 *     mapping0 mdct psy registry res0 sharedbook smallft synthesis vorbisenc \
 *     window; do echo $V/$f.c; done) -lm && build/cue_gaps_test build/cue_gaps'
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "setup_os.h"

#define RAW 2352

/* ---- what setup_disc.c asks of the system ---- */

int os_dir_exists(const char *p)
{
    struct stat st;
    return !stat(p, &st) && S_ISDIR(st.st_mode);
}

int os_file_exists(const char *p)
{
    struct stat st;
    return !stat(p, &st) && S_ISREG(st.st_mode);
}

void os_make_dirs(const char *path)
{
    mkdir(path, 0755);
}

int os_entry(const char *dir, const char *name, char *out, size_t size)
{
    snprintf(out, size, "%s/%s", dir, name);
    return !access(out, F_OK);
}

int os_each(const char *dir, int (*fn)(void *ctx, const char *name, int is_dir), void *ctx)
{
    (void)dir, (void)fn, (void)ctx;
    return 1;
}

void os_message(const char *text, int kind)
{
    printf("message (%d): %s\n", kind, text);
}

int os_progress_show(const char *what, const char *detail)
{
    (void)what, (void)detail;
    return 0;
}

int os_cd_open(const char *cd, struct cd_toc *toc, void **h)
{
    (void)cd, (void)toc, (void)h;
    return 0;
}

int os_cd_read(void *h, long lba, int n, unsigned char *buf)
{
    (void)h, (void)lba, (void)n, (void)buf;
    return 0;
}

void os_cd_close(void *h)
{
    (void)h;
}

/* ---- the image ---- */

#define SECTORS 3800L

/* a tone, but in 1000..1049 and 2000..2149 */
static int sounding(long sector)
{
    return !(sector >= 1000 && sector < 1050) && !(sector >= 2000 && sector < 2150);
}

static int make_image(const char *dir)
{
    char path[1024];
    unsigned char sector[RAW];
    long s;
    int i;
    FILE *f;
    mkdir(dir, 0755);
    snprintf(path, sizeof path, "%s/gaps.bin", dir);
    if (!(f = fopen(path, "wb")))
        return 0;
    for (s = 0; s < SECTORS; s++) {
        for (i = 0; i < RAW / 2; i++) {
            short v = sounding(s) ? (short)(8000 * sin((s * (RAW / 4) + i / 2) * 0.0628)) : 0;
            sector[2 * i] = (unsigned char)v;
            sector[2 * i + 1] = (unsigned char)(v >> 8);
        }
        fwrite(sector, RAW, 1, f);
    }
    fclose(f);
    snprintf(path, sizeof path, "%s/gaps.cue", dir);
    if (!(f = fopen(path, "w")))
        return 0;
    fprintf(f, "FILE \"gaps.bin\" BINARY\n"
               "  TRACK 01 MODE1/2352\n    INDEX 01 00:00:00\n"
               "  TRACK 02 AUDIO\n    INDEX 01 00:04:00\n"      /* 300 */
               "  TRACK 03 AUDIO\n    INDEX 01 00:16:00\n"      /* 1200: 150 late */
               "  TRACK 04 AUDIO\n    INDEX 01 00:28:50\n"      /* 2150: right */
               "  TRACK 05 AUDIO\n    INDEX 01 00:40:00\n");    /* 3000: no silence */
    fclose(f);
    return 1;
}

/* image_encode_music's report on stderr, in log */
static int encode(const char *dir, const char *out, int fix, char *log, size_t size)
{
    char cue[1024], err[1024];
    int ok, saved = dup(2);
    size_t n;
    FILE *f;
    snprintf(cue, sizeof cue, "%s/gaps.cue", dir);
    snprintf(err, sizeof err, "%s/stderr.txt", dir);
    mkdir(out, 0755);
    fflush(stderr);
    if (!freopen(err, "w", stderr))
        return 0;
    setup_fix_cue_gaps = fix;
    ok = image_encode_music(cue, out);
    fflush(stderr);
    dup2(saved, 2);
    close(saved);
    if (!(f = fopen(err, "r")))
        return 0;
    n = fread(log, 1, size - 1, f);
    log[n] = 0;
    fclose(f);
    return ok;
}

static long file_size(const char *dir, int track)
{
    char path[1024];
    struct stat st;
    snprintf(path, sizeof path, "%s/Music/Track%02d.ogg", dir, track);
    return stat(path, &st) ? -1 : (long)st.st_size;
}

int main(int argc, char **argv)
{
    static const char *const want[] = {
        "setup: fix-cue-gaps: tracks 2/3: boundary moved -151 sectors (-2.01 s)\n",
        "setup: fix-cue-gaps: tracks 3/4: boundary kept (silent already)\n",
        "setup: fix-cue-gaps: tracks 4/5: boundary kept (no silence near it)\n",
    };
    char dir[900], fixed[1000], exact[1000], log[4096];
    int i, fail = 0;
    snprintf(dir, sizeof dir, "%s", argc > 1 ? argv[1] : "build/cue_gaps");
    snprintf(fixed, sizeof fixed, "%s/fixed", dir);
    snprintf(exact, sizeof exact, "%s/exact", dir);
    if (!make_image(dir)) {
        printf("FAIL: could not write the image in %s\n", dir);
        return 1;
    }

    if (!encode(dir, fixed, 1, log, sizeof log)) {
        printf("FAIL: image_encode_music (fixed)\n");
        return 1;
    }
    for (i = 0; i < 3; i++)
        if (!strstr(log, want[i])) {
            printf("FAIL: no \"%.*s\" in:\n%s", (int)strlen(want[i]) - 1, want[i], log);
            fail = 1;
        }

    if (!encode(dir, exact, 0, log, sizeof log)) {
        printf("FAIL: image_encode_music (exact)\n");
        return 1;
    }
    if (strstr(log, "fix-cue-gaps")) {
        printf("FAIL: --no-fix-cue-gaps still moved boundaries:\n%s", log);
        fail = 1;
    }

    /* track 2 shorter by 151 sectors, track 3 longer; 4 and 5 as the cue sheet says */
    for (i = 2; i <= 5; i++) {
        long a = file_size(fixed, i), b = file_size(exact, i);
        printf("Track%02d.ogg: %ld bytes fixed, %ld exact\n", i, a, b);
        if (a <= 0 || b <= 0 || (i == 2 && a >= b) || (i == 3 && a <= b) || (i > 3 && a != b))
            fail = 1;
    }
    printf(fail ? "FAIL\n" : "PASS\n");
    return fail;
}
