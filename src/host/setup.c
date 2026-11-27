/*
 * The game set up from the user's copy of Civilization II Multiplayer Gold
 * Edition (setup.h).  The game is found where it usually is: installed,
 * its CD in a drive, or a disc image of the CD (a .cue with its .bin) in
 * the usual folders (setup_win.c, setup_unix.c); else the user says where.
 * An installed game's folder is copied; from the CD the game is unpacked
 * from its InstallShield cabinet (DATA1.CAB, with unshield), as its
 * installer would, and its videos copied; from a disc image the same, out
 * of its data track (setup_disc.c).  The music, the CD's audio tracks, from
 * a disc image (with any source, where one is found), else from the CD in
 * its drive (read from the drive: setup_win.c, setup_unix.c): encoded as
 * Ogg Vorbis.  A .zip of an installed game's folder is unpacked.  Into
 * GameData: beside the program on Windows, in the user's data folder on
 * macOS and Linux.  And later, for Settings' Classic graphics in a
 * game folder without the original's files, those it lacks, the same ways
 * (setup_add_original).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libunshield.h>
#include <zlib.h>
#include "setup.h"
#include "setup_os.h"
#include "host_lang.h"

#define DONE_FILE "OpenCiv2-setup.txt"      /* in GameData: written when the set-up is complete */
#define CABINET_DIR "OpenCiv2-cabinet"      /* in GameData: the cabinet out of a disc image, while unpacked */

/* An installed game (or a copy of its folder). */
int setup_game_folder(const char *dir)
{
    char p[SETUP_PATH];
    return *dir && os_entry(dir, "Civ2Art.dll", p, sizeof p) && os_file_exists(p) &&
           os_entry(dir, "RULES.TXT", p, sizeof p) && os_file_exists(p);
}

/* The game's CD (or a copy of it): the cabinet the installer unpacks, the
   folder with the videos. */
int setup_game_cd(const char *root)
{
    char p[SETUP_PATH];
    return *root && os_entry(root, "DATA1.CAB", p, sizeof p) && os_file_exists(p) &&
           os_entry(root, "CIV2", p, sizeof p) && os_dir_exists(p);
}

/* A disc image of the CD: the folders of home it is usually in. */
struct civ2_dirs {
    const char *home;
    char *out;
    size_t size;
};

static int civ2_dir(void *ctx, const char *name, int is_dir)
{
    const struct civ2_dirs *c = ctx;
    char p[SETUP_PATH];
    if (!is_dir || strlen(name) < 4 || (name[0] | 0x20) != 'c' || (name[1] | 0x20) != 'i' || (name[2] | 0x20) != 'v' ||
        name[3] != '2')
        return 1;
    snprintf(p, sizeof p, "%s" SEP_S "%s", c->home, name);
    return !setup_image_in(p, 1, c->out, c->size);
}

int setup_image_in_home(const char *home, char *out, size_t size)
{
    static const char *const in[] = {"Downloads", "Desktop"};
    struct civ2_dirs c = {home, out, size};
    char p[SETUP_PATH];
    size_t i;
    for (i = 0; i < sizeof in / sizeof in[0]; i++)
        if (os_entry(home, in[i], p, sizeof p) && setup_image_in(p, 1, out, size))
            return 1;
    return setup_image_in(home, 0, out, size) || !os_each(home, civ2_dir, &c);
}

/* ---- copying and unpacking ---- */

static const char *copy_dest;           /* GameData: not copied into itself */
static int keep_existing;               /* the destination's own files kept (setup_add_original) */

static int same_flag(const char *a, const char *b);

/* (keep_existing) dir has name, whatever its case: its path in out */
static int existing(const char *dir, const char *name, char *out, size_t size)
{
    char p[SETUP_PATH];
    if (!keep_existing || !os_entry(dir, name, p, sizeof p))
        return 0;
    snprintf(out, size, "%s", p);
    return 1;
}

struct copy {
    const char *src, *dst;
};

static int copy_tree(const char *src, const char *dst);

static int copy_entry(void *ctx, const char *name, int is_dir)
{
    const struct copy *c = ctx;
    char from[SETUP_PATH], to[SETUP_PATH];
    snprintf(from, sizeof from, "%s" SEP_S "%s", c->src, name);
    if (!existing(c->dst, name, to, sizeof to))
        snprintf(to, sizeof to, "%s" SEP_S "%s", c->dst, name);
    else if (!is_dir)
        return 1;               /* (the game folder's own) */
    if (is_dir)
        return (copy_dest && os_same_path(from, copy_dest)) || copy_tree(from, to);
    if (os_progress_show(host_text("setup.copying_file"), from))
        return 0;
    if (!os_copy_file(from, to)) {
        char text[2 * SETUP_PATH + 64];
        snprintf(text, sizeof text, host_text("setup.copy_failed"), from, to);
        os_message(text, OS_ERROR);
        return 0;
    }
    return 1;
}

/* src's files and folders into dst (made if need be); 0: stopped */
static int copy_tree(const char *src, const char *dst)
{
    struct copy c = {src, dst};
    os_make_dirs(dst);
    return os_each(src, copy_entry, &c);
}

/* The game from the CD's cabinet into dst, as its installer unpacks it
   (the "Sound Files" group into Sound, the others as their folders say). */
static int unpack_cab(const char *cab, const char *dst)
{
    Unshield *u;
    int g, i, ok = 1;
    unshield_set_log_level(0);
    u = unshield_open(cab);
    if (!u) {
        char text[SETUP_PATH + 64];
        snprintf(text, sizeof text, host_text("setup.read_failed"), cab);
        os_message(text, OS_ERROR);
        return 0;
    }
    for (g = 0; ok && g < unshield_file_group_count(u); g++) {
        UnshieldFileGroup *fg = unshield_file_group_get(u, g);
        const char *sub = !strcmp(fg->name, "Sound Files") ? "Sound" : "";
        for (i = (int)fg->first_file; ok && i <= (int)fg->last_file; i++) {
            const char *dir, *name;
            char folder[SETUP_PATH], path[SETUP_PATH], *c;
            if (!unshield_file_is_valid(u, i))
                continue;
            dir = unshield_directory_name(u, unshield_file_directory(u, i));
            name = unshield_file_name(u, i);
            snprintf(folder, sizeof folder, "%s%s%s%s%s", dst, *sub ? SEP_S : "", sub, dir && *dir ? SEP_S : "",
                     dir ? dir : "");
            for (c = folder; *c; c++)
                if (*c == '\\' || *c == '/')
                    *c = SEP;
            os_make_dirs(folder);
            snprintf(path, sizeof path, "%s" SEP_S "%s", folder, name);
            if (existing(folder, name, path, sizeof path)) {
                /* (the game folder's own) */
            } else if (os_progress_show(host_text("setup.unpacking"), path)) {
                ok = 0;
            } else if (!unshield_file_save(u, i, path)) {
                char text[SETUP_PATH + 64];
                snprintf(text, sizeof text, host_text("setup.unpack_failed"), name);
                os_message(text, OS_ERROR);
                ok = 0;
            }
        }
    }
    unshield_close(u);
    return ok;
}

/* The CD's videos (CIV2's KINGS, VIDEO) into game_dir, where it has none. */
static int copy_videos(const char *cd, const char *game_dir)
{
    static const char *const folders[] = {"KINGS", "VIDEO"};
    char civ2[SETUP_PATH], from[SETUP_PATH], to[SETUP_PATH];
    size_t i;
    if (!os_entry(cd, "CIV2", civ2, sizeof civ2))
        return 1;
    for (i = 0; i < sizeof folders / sizeof folders[0]; i++) {
        snprintf(to, sizeof to, "%s" SEP_S "%s", game_dir, folders[i]);
        if (os_entry(civ2, folders[i], from, sizeof from) && os_dir_exists(from) && !os_dir_exists(to) &&
            !copy_tree(from, to))
            return 0;
    }
    return 1;
}

static int remove_entry(void *ctx, const char *name, int is_dir)
{
    char p[SETUP_PATH];
    (void)is_dir;
    snprintf(p, sizeof p, "%s" SEP_S "%s", (const char *)ctx, name);
    remove(p);
    return 1;
}

/* The game from a disc image's cabinet into dst: the cabinet copied out of
   the image into a folder of dst, unpacked, removed. */
static int unpack_image(const char *cue, const char *dst)
{
    char tmp[SETUP_PATH], cab[SETUP_PATH];
    int ok;
    snprintf(tmp, sizeof tmp, "%s" SEP_S "%s", dst, CABINET_DIR);
    ok = image_extract_cabinet(cue, tmp, cab, sizeof cab) && unpack_cab(cab, dst);
    os_each(tmp, remove_entry, tmp);
    os_remove_dir(tmp);
    return ok;
}

/* ---- a .zip of an installed game's folder (stored or deflated; no ZIP64) ---- */

struct zip_entry {
    char name[SETUP_PATH];      /* '/' between folders */
    unsigned method;
    unsigned long csize, size, local;
};

static unsigned le16(const unsigned char *p) { return (unsigned)p[0] | (unsigned)p[1] << 8; }
static unsigned long le32(const unsigned char *p) { return (unsigned long)le16(p) | (unsigned long)le16(p + 2) << 16; }

/* Each file of the zip at path to fn (f: the zip, open), until it returns
   0.  1: all; 0: not a zip, or damaged; -1: stopped. */
static int zip_each(const char *path, int (*fn)(void *ctx, FILE *f, const struct zip_entry *e), void *ctx)
{
    FILE *f = fopen(path, "rb");
    unsigned char *tail, h[46];
    long size, start, n, i, end = -1;
    unsigned count, k, nlen, xlen, clen;
    unsigned long at;
    int r = 1;
    if (!f)
        return 0;
    if (fseek(f, 0, SEEK_END) || (size = ftell(f)) < 22 || !(tail = malloc(65557))) {
        fclose(f);
        return 0;
    }
    start = size > 65557 ? size - 65557 : 0;
    n = size - start;
    if (!fseek(f, start, SEEK_SET) && fread(tail, 1, (size_t)n, f) == (size_t)n)
        for (i = n - 22; i >= 0; i--)
            if (le32(tail + i) == 0x06054b50) {
                end = i;
                break;
            }
    if (end < 0) {
        free(tail);
        fclose(f);
        return 0;
    }
    count = le16(tail + end + 10);
    at = le32(tail + end + 16);
    free(tail);
    for (k = 0; k < count && r > 0; k++) {
        struct zip_entry e;
        char *c;
        if (fseek(f, (long)at, SEEK_SET) || fread(h, 1, 46, f) != 46 || le32(h) != 0x02014b50) {
            r = 0;
            break;
        }
        nlen = le16(h + 28);
        xlen = le16(h + 30);
        clen = le16(h + 32);
        e.method = le16(h + 10);
        e.csize = le32(h + 20);
        e.size = le32(h + 24);
        e.local = le32(h + 42);
        if (nlen >= sizeof e.name || fread(e.name, 1, nlen, f) != nlen) {
            r = 0;
            break;
        }
        e.name[nlen] = 0;
        for (c = e.name; *c; c++)
            if (*c == '\\')
                *c = '/';
        at += 46 + nlen + xlen + clen;
        if (nlen && e.name[nlen - 1] != '/' && !strstr(e.name, "../") && !fn(ctx, f, &e))
            r = -1;
    }
    fclose(f);
    return r;
}

/* The folders in the zip with Civ2Art.dll, with RULES.TXT (the first few). */
struct zip_root {
    char art[8][SETUP_PATH], rules[8][SETUP_PATH];
    int na, nr;
};

static int zip_root_entry(void *ctx, FILE *f, const struct zip_entry *e)
{
    struct zip_root *z = ctx;
    const char *base = strrchr(e->name, '/');
    int dir = base ? (int)(base - e->name) + 1 : 0;
    (void)f;
    base = base ? base + 1 : e->name;
    if (same_flag(base, "Civ2Art.dll") && z->na < 8)
        snprintf(z->art[z->na++], SETUP_PATH, "%.*s", dir, e->name);
    else if (same_flag(base, "RULES.TXT") && z->nr < 8)
        snprintf(z->rules[z->nr++], SETUP_PATH, "%.*s", dir, e->name);
    return 1;
}

/* The installed game's folder in the zip at path (prefix: "" or ending
   in '/'), as setup_game_folder: the one nearest the top.  0: none. */
static int zip_game_root(const char *path, char *prefix, size_t size)
{
    static struct zip_root z;
    size_t n = strlen(path);
    int i, k, best = -1;
    if (n < 5 || !same_flag(path + n - 4, ".zip") || !os_file_exists(path))
        return 0;
    memset(&z, 0, sizeof z);
    if (zip_each(path, zip_root_entry, &z) != 1)
        return 0;
    for (i = 0; i < z.na; i++)
        for (k = 0; k < z.nr; k++)
            if (same_flag(z.art[i], z.rules[k]) && (best < 0 || strlen(z.art[i]) < strlen(z.art[best])))
                best = i;
    if (best < 0)
        return 0;
    if (prefix)
        snprintf(prefix, size, "%s", z.art[best]);
    return 1;
}

int setup_game_zip(const char *path)
{
    return zip_game_root(path, NULL, 0);
}

/* A zip of the game in dir (the first), into out; 0: none. */
struct zip_in {
    const char *dir;
    char *out;
    size_t size;
};

static int zip_in_entry(void *ctx, const char *name, int is_dir)
{
    struct zip_in *z = ctx;
    char p[SETUP_PATH];
    if (is_dir)
        return 1;
    snprintf(p, sizeof p, "%s" SEP_S "%s", z->dir, name);
    if (!setup_game_zip(p))
        return 1;
    snprintf(z->out, z->size, "%s", p);
    return 0;
}

static int setup_zip_in(const char *dir, char *out, size_t size)
{
    struct zip_in z = {dir, out, size};
    return os_dir_exists(dir) && !os_each(dir, zip_in_entry, &z);
}

struct zip_out {
    const char *prefix, *dst;
    int failed;
};

/* One file of the zip, under its game's folder, into dst (its folders
   made); 0: stopped or failed. */
static int zip_unpack_entry(void *ctx, FILE *f, const struct zip_entry *e)
{
    struct zip_out *z = ctx;
    size_t np = strlen(z->prefix);
    char to[SETUP_PATH], folder[SETUP_PATH];
    const char *c, *base;
    unsigned char lh[30], in[16384], out[16384];
    unsigned long left = e->csize;
    FILE *o;
    int ok = 1;
    z_stream zs;
    if (strncmp(e->name, z->prefix, np))
        return 1;
    /* (keep_existing: the folders there whatever their case) */
    snprintf(folder, sizeof folder, "%s", z->dst);
    for (c = e->name + np; (base = strchr(c, '/')); c = base + 1) {
        char name[SETUP_PATH], p[SETUP_PATH];
        snprintf(name, sizeof name, "%.*s", (int)(base - c), c);
        if (!existing(folder, name, p, sizeof p))
            snprintf(p, sizeof p, "%s" SEP_S "%s", folder, name);
        snprintf(folder, sizeof folder, "%s", p);
    }
    os_make_dirs(folder);
    if (existing(folder, c, to, sizeof to))
        return 1;               /* (the game folder's own) */
    snprintf(to, sizeof to, "%s" SEP_S "%s", folder, c);
    if (os_progress_show(host_text("setup.unpacking"), to))
        return 0;
    if (fseek(f, (long)e->local, SEEK_SET) || fread(lh, 1, 30, f) != 30 || le32(lh) != 0x04034b50 ||
        fseek(f, (long)(e->local + 30 + le16(lh + 26) + le16(lh + 28)), SEEK_SET) ||
        (e->method != 0 && e->method != 8) || !(o = fopen(to, "wb")))
        ok = 0;
    else {
        memset(&zs, 0, sizeof zs);
        if (e->method == 8 && inflateInit2(&zs, -MAX_WBITS) != Z_OK)
            ok = 0;
        while (ok && left) {
            size_t n = fread(in, 1, left < sizeof in ? (size_t)left : sizeof in, f);
            if (!n) {
                ok = 0;
                break;
            }
            left -= n;
            if (e->method == 0) {
                ok = fwrite(in, 1, n, o) == n;
                continue;
            }
            zs.next_in = in;
            zs.avail_in = (uInt)n;
            do {
                int r;
                zs.next_out = out;
                zs.avail_out = sizeof out;
                r = inflate(&zs, Z_NO_FLUSH);
                if (r != Z_OK && r != Z_STREAM_END) {
                    ok = 0;
                    break;
                }
                if (fwrite(out, 1, sizeof out - zs.avail_out, o) != sizeof out - zs.avail_out)
                    ok = 0;
                if (r == Z_STREAM_END)
                    break;
            } while (ok && zs.avail_out == 0);
        }
        if (e->method == 8)
            inflateEnd(&zs);
        if (fclose(o))
            ok = 0;
        if (!ok)
            remove(to);
    }
    if (!ok) {
        char text[SETUP_PATH + 64];
        snprintf(text, sizeof text, host_text("setup.unpack_failed"), e->name);
        os_message(text, OS_ERROR);
        z->failed = 1;
    }
    return ok;
}

/* The game from a zip of its installed folder into dst. */
static int unpack_zip(const char *zip, const char *dst)
{
    char prefix[SETUP_PATH];
    struct zip_out z = {prefix, dst, 0};
    if (!zip_game_root(zip, prefix, sizeof prefix))
        return 0;
    return zip_each(zip, zip_unpack_entry, &z) == 1 && !z.failed;
}

/* GameData has the music (Music/Track02: the first of the CD's). */
static int has_music(const char *game_dir)
{
    char music[SETUP_PATH], p[SETUP_PATH];
    return os_entry(game_dir, "Music", music, sizeof music) &&
           ((os_entry(music, "Track02.ogg", p, sizeof p) && os_file_exists(p)) ||
            (os_entry(music, "Track02.wav", p, sizeof p) && os_file_exists(p)));
}

/* ---- the set-up ---- */

/* The game from source (an installed game's folder, a .zip of it, the CD,
   a disc image) into game_dir.  1: done. */
static int game_from(const char *source, const char *game_dir)
{
    if (setup_game_cd(source)) {
        char cab[SETUP_PATH];
        os_progress_show(host_text("setup.unpacking_cd"), source);
        os_entry(source, "DATA1.CAB", cab, sizeof cab);
        return unpack_cab(cab, game_dir);
    }
    if (setup_game_image(source)) {
        os_progress_show(host_text("setup.unpacking_image"), source);
        return unpack_image(source, game_dir);
    }
    if (setup_game_zip(source)) {
        os_progress_show(host_text("setup.unpacking_zip"), source);
        return unpack_zip(source, game_dir);
    }
    os_progress_show(host_text("setup.copying"), source);
    return copy_tree(source, game_dir);
}

/* What source names, as the game: a folder with a disc image or a zip of
   the game in it, that (source changed).  1: the game is there. */
static int game_source(char *source, size_t size)
{
    char found[SETUP_PATH];
    if (!setup_game_folder(source) && !setup_game_cd(source) &&
        (setup_image_in(source, 0, found, sizeof found) || setup_zip_in(source, found, sizeof found)))
        snprintf(source, size, "%s", found);
    return setup_game_folder(source) || setup_game_cd(source) || setup_game_image(source) ||
           setup_game_zip(source);
}

/* GameData from source (an installed game's folder, the CD or a disc
   image), from the CD (cd, may be "") its videos, from a disc image
   (image, may be "") the videos it lacks and the music; else cd_music
   (cd_has_music's): the music from the CD's drive; music_only: the game is
   there already (set up without music).  1: done. */
static int set_up(const char *game_dir, const char *source, const char *cd, const char *image, int cd_music,
                  int music_only)
{
    char done[SETUP_PATH];
    FILE *f;
    int ok = 1, unread = cd_music < 0;
    os_make_dirs(game_dir);
    if (!music_only)
        ok = game_from(source, game_dir);
    if (ok && *cd)
        ok = copy_videos(cd, game_dir);
    if (ok && *image) {
        ok = image_copy_videos(image, game_dir) && image_encode_music(image, game_dir);
    } else if (ok && cd_music > 0) {
        int r = cd_encode_music(cd, game_dir);
        unread = r < 0;
        ok = r != 0;
        if (unread) {
            /* (the tracks read before the drive failed: gone, so that
               has_music says no and a later --setup tries again) */
            char music[SETUP_PATH];
            snprintf(music, sizeof music, "%s" SEP_S "Music", game_dir);
            os_remove_dir(music);
        }
    }
    if (!ok)
        return 0;
    snprintf(done, sizeof done, "%s" SEP_S "%s", game_dir, DONE_FILE);
    if (music_only) {
        if (!unread && (f = fopen(done, "a"))) {
            fprintf(f, "OpenCiv2: the music from %s%s.\n", *image ? "" : "the CD in ", *image ? image : cd);
            fclose(f);
        }
    } else if ((f = fopen(done, "w"))) {
        int other = *image && strcmp(image, source);
        fprintf(f, "OpenCiv2: the game set up from %s%s%s%s%s.\n", source, *cd && strcmp(cd, source) ? ", videos from " : "",
                *cd && strcmp(cd, source) ? cd : "", !*image ? "" : other ? ", music from " : ", with its music",
                other ? image : "");
        if (cd_music > 0 && !unread)
            fprintf(f, "OpenCiv2: the music from the CD in %s.\n", cd);
        fclose(f);
    }
    if (unread)
        os_message(host_text(music_only ? "setup.music_failed_only" : "setup.music_failed"), OS_WARNING);
    return 1;
}

static int same_flag(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
    return *a == *b;
}

int setup_choose_game(int argc, char **argv, char *game, size_t size)
{
    char game_data[SETUP_PATH], done[SETUP_PATH], source[SETUP_PATH], cd[SETUP_PATH] = "", image[SETUP_PATH] = "";
    char text[3 * SETUP_PATH + 256], music[SETUP_PATH + 32] = "";
    int again, ok, cancelled, music_only, cd_music = 0;
    int i, n;

    /* (--fix-cue-gaps, --no-fix-cue-gaps, --hd, --hd=N, --sharp, --sharp=N, any
       case: noted and left out of the arguments read here) */
    for (i = n = argc > 0; i < argc; i++)
        if (same_flag(argv[i], "--no-fix-cue-gaps"))
            setup_fix_cue_gaps = 0;
        else if (same_flag(argv[i], "--fix-cue-gaps"))
            setup_fix_cue_gaps = 1;
        else if (same_flag(argv[i], "--hd") || !strncmp(argv[i], "--hd=", 5) || !strncmp(argv[i], "--HD=", 5)) {
            /* HD graphics (docs/HD-GRAPHICS.md), for the game: --hd on,
               --hd=N density N (2 to 4; 1 on, 0 off), as CIV2_HD_ARG */
            char kv[64];
            snprintf(kv, sizeof kv, "CIV2_HD_ARG=%s", argv[i][4] == '=' ? argv[i] + 5 : "1");
#ifdef _WIN32
            _putenv(kv);
#else
            putenv(strdup(kv));
#endif
        } else if (same_flag(argv[i], "--sharp") || !strncmp(argv[i], "--sharp=", 8) || !strncmp(argv[i], "--SHARP=", 8)) {
            /* sharp text and interface (docs/EXTRAS.md), for the game:
               --sharp on, --sharp=0 off, as CIV2_SHARP_ARG */
            char kv[64];
            snprintf(kv, sizeof kv, "CIV2_SHARP_ARG=%s", argv[i][7] == '=' ? argv[i] + 8 : "1");
#ifdef _WIN32
            _putenv(kv);
#else
            putenv(strdup(kv));
#endif
        } else
            argv[n++] = argv[i];
    argc = n;
#ifdef _WIN32
    /* (the translations beside the program first: the set-up's texts) */
    if (os_program_files())
        host_lang_init(NULL);
#endif

#ifdef __APPLE__
    /* (started from the Finder by an older macOS: its process serial number) */
    if (argc > 1 && !strncmp(argv[1], "-psn_", 5))
        argc = 1;
#endif
    again = argc > 1 && !strcmp(argv[1], "--setup");
    if (argc > 1 && !again) {
        /* a folder: the game, played where it is */
        if (!os_dir_exists(argv[1])) {
            char text[SETUP_PATH + 64];
            snprintf(text, sizeof text, host_text("setup.no_folder"), argv[1]);
            os_message(text, OS_ERROR);
            return 0;
        }
        snprintf(game, size, "%s", argv[1]);
        return os_program_files();
    }
    os_current_dir(game, size);
    if (argc < 2 && ((getenv("CIV2_SCRIPT") && *getenv("CIV2_SCRIPT"))
#ifndef _WIN32
                     || setup_game_folder(game)
#endif
                     ))
        /* a replay (the test harness), or (macOS, Linux) started in the
           game's folder: the game where it is run */
        return os_program_files();
    os_game_data(game_data, sizeof game_data);
#ifndef __wasi__
    host_lang_init(game_data);      /* (its CIV.INI's Language=, set up before) */
#endif
    snprintf(done, sizeof done, "%s" SEP_S "%s", game_data, DONE_FILE);
    snprintf(game, size, "%s", game_data);
    if (os_file_exists(done) && !again)
        return os_program_files();

    /* where the game is: --setup FOLDER (or IMAGE.cue), the installed
       game, the CD, a disc image of it, else asked; a disc image for the
       music in any case */
    if (!os_find_cd(cd, sizeof cd))
        cd[0] = 0;
    if (!os_find_image(image, sizeof image))
        image[0] = 0;
    if (again && argc > 2)
        snprintf(source, sizeof source, "%s", argv[2]);
    else if (!os_find_installed(source, sizeof source))
        snprintf(source, sizeof source, "%s", *cd ? cd : image);
    for (;;) {
        /* (a folder with a disc image or a zip of the game in it: that) */
        if (game_source(source, sizeof source))
            break;
        if (source[0]) {
            snprintf(text, sizeof text, host_text("setup.not_game"), source);
            os_message(text, OS_WARNING);
        } else {
            os_message(host_text("setup.not_found"), OS_INFO);
        }
        if (!os_ask_folder(source, sizeof source))
            return 0;
    }
    if (setup_game_cd(source))
        snprintf(cd, sizeof cd, "%s", source);
    if (setup_game_image(source))
        snprintf(image, sizeof image, "%s", source);
    /* no disc image: the music from the CD in its drive, if it reads */
    if (!*image && *cd)
        cd_music = cd_has_music(cd);
    if (*image)
        snprintf(music, sizeof music, host_text("setup.music_image"), image);
    else if (cd_music > 0)
        snprintf(music, sizeof music, host_text("setup.music_cd"), cd);
    /* (set up already, but without music: only that) */
    music_only = ((*image && !strcmp(image, source)) || (cd_music > 0 && !strcmp(cd, source))) &&
                 os_file_exists(done) && !has_music(game_data);
    if (music_only)
        snprintf(text, sizeof text, host_text("setup.confirm_music"), game_data, music);
    else if (*music)
        snprintf(text, sizeof text,
                 host_text(setup_game_folder(source) ? "setup.confirm_copy_music" : "setup.confirm_unpack_music"),
                 source, game_data, music);
    else
        snprintf(text, sizeof text, host_text(setup_game_folder(source) ? "setup.confirm_copy" : "setup.confirm_unpack"),
                 source, game_data);
    if (!os_confirm(text))
        return 0;
    copy_dest = game_data;
    os_progress_open();
    ok = os_program_files() && set_up(game_data, source, cd, image, cd_music, music_only);
    cancelled = !ok && os_progress_stopped();
    os_progress_close();
    if (cancelled)
        os_message(host_text("setup.stopped"), OS_INFO);
    return ok;
}

/* ---- the original's files, later (Settings' Classic graphics) ---- */

int setup_add_original(const char *game)
{
    char source[SETUP_PATH] = "", text[3 * SETUP_PATH + 256];
    int ok, cancelled;
    /* where it usually is (not the game folder itself), else asked */
    if (!(os_find_installed(source, sizeof source) && !os_same_path(source, game)) &&
        !os_find_cd(source, sizeof source) && !os_find_image(source, sizeof source))
        source[0] = 0;
    for (;;) {
        if (source[0] && !os_same_path(source, game) && game_source(source, sizeof source))
            break;
        if (source[0]) {
            snprintf(text, sizeof text, host_text("setup.not_game"), source);
            os_message(text, OS_WARNING);
        } else {
            os_message(host_text("setup.original_not_found"), OS_INFO);
        }
        if (!os_ask_folder(source, sizeof source))
            return 0;
    }
    snprintf(text, sizeof text, host_text("setup.confirm_original"), source, game);
    if (!os_ask(text, host_text("setup.copy"), host_text("setup.cancel")))
        return 0;
    copy_dest = game;
    keep_existing = 1;
    os_progress_open();
    ok = game_from(source, game);
    cancelled = !ok && os_progress_stopped();
    os_progress_close();
    keep_existing = 0;
    if (cancelled)
        os_message(host_text("setup.original_stopped"), OS_INFO);
    return ok;
}
