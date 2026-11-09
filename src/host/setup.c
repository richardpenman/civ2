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
 * Ogg Vorbis.  Into GameData: beside the program on Windows, in the user's
 * data folder on macOS and Linux.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libunshield.h>
#include "setup.h"
#include "setup_os.h"

#define DONE_FILE "OpenCiv2-setup.txt"      /* in GameData: written when the set-up is complete */
#define CABINET_DIR "OpenCiv2-cabinet"      /* in GameData: the cabinet out of a disc image, while unpacked */

/* An installed game (or a copy of its folder). */
int setup_game_folder(const char *dir)
{
    char p[SETUP_PATH];
    return *dir && os_entry(dir, "GAME.TXT", p, sizeof p) && os_file_exists(p) &&
           os_entry(dir, "RULES.TXT", p, sizeof p) && os_file_exists(p) &&
           os_entry(dir, "LABELS.TXT", p, sizeof p) && os_file_exists(p);
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

struct copy {
    const char *src, *dst;
};

static int copy_tree(const char *src, const char *dst);

static int copy_entry(void *ctx, const char *name, int is_dir)
{
    const struct copy *c = ctx;
    char from[SETUP_PATH], to[SETUP_PATH];
    snprintf(from, sizeof from, "%s" SEP_S "%s", c->src, name);
    snprintf(to, sizeof to, "%s" SEP_S "%s", c->dst, name);
    if (is_dir)
        return (copy_dest && os_same_path(from, copy_dest)) || copy_tree(from, to);
    if (os_progress_show("Copying", from))
        return 0;
    if (!os_copy_file(from, to)) {
        char text[2 * SETUP_PATH + 64];
        snprintf(text, sizeof text, "Could not copy %s to %s.", from, to);
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
        snprintf(text, sizeof text, "Could not read %s.", cab);
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
            if (os_progress_show("Unpacking", path)) {
                ok = 0;
            } else if (!unshield_file_save(u, i, path)) {
                char text[SETUP_PATH + 64];
                snprintf(text, sizeof text, "Could not unpack %s.", name);
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

/* GameData has the music (Music/Track02: the first of the CD's). */
static int has_music(const char *game_dir)
{
    char music[SETUP_PATH], p[SETUP_PATH];
    return os_entry(game_dir, "Music", music, sizeof music) &&
           ((os_entry(music, "Track02.ogg", p, sizeof p) && os_file_exists(p)) ||
            (os_entry(music, "Track02.wav", p, sizeof p) && os_file_exists(p)));
}

/* ---- the set-up ---- */

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
    if (music_only) {
    } else if (setup_game_cd(source)) {
        char cab[SETUP_PATH];
        os_progress_show("Unpacking the game from its CD", source);
        os_entry(source, "DATA1.CAB", cab, sizeof cab);
        ok = unpack_cab(cab, game_dir);
    } else if (setup_game_image(source)) {
        os_progress_show("Unpacking the game from its disc image", source);
        ok = unpack_image(source, game_dir);
    } else {
        os_progress_show("Copying the game", source);
        ok = copy_tree(source, game_dir);
    }
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
        os_message(music_only ? "The music could not be read from the CD's drive." :
                   "The game is set up, but without its music: it could not be read from the CD's drive.",
                   OS_WARNING);
    return 1;
}

int setup_choose_game(int argc, char **argv, char *game, size_t size)
{
    char game_data[SETUP_PATH], done[SETUP_PATH], source[SETUP_PATH], cd[SETUP_PATH] = "", image[SETUP_PATH] = "";
    char text[3 * SETUP_PATH + 256], music[SETUP_PATH + 32] = "";
    int again, ok, cancelled, music_only, cd_music = 0;

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
            snprintf(text, sizeof text, "There is no folder %s.", argv[1]);
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
        char found[SETUP_PATH];
        /* (a folder with a disc image in it: the image) */
        if (!setup_game_folder(source) && !setup_game_cd(source) && setup_image_in(source, 0, found, sizeof found))
            snprintf(source, sizeof source, "%s", found);
        if (setup_game_folder(source) || setup_game_cd(source) || setup_game_image(source))
            break;
        if (source[0]) {
            snprintf(text, sizeof text, "%s has neither an installed Civilization II Multiplayer Gold Edition nor "
                     "its CD, nor a disc image of the CD.", source);
            os_message(text, OS_WARNING);
        } else {
            os_message("Civilization II Multiplayer Gold Edition was found neither where it is usually installed, "
                       "nor in a CD drive, nor as a disc image of its CD (a .cue file with its .bin) in the usual "
                       "folders. Please show where it is: the folder it is installed in, its CD, or the disc image "
                       "(or the folder it is in).", OS_INFO);
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
        snprintf(music, sizeof music, "the disc image %s", image);
    else if (cd_music > 0)
        snprintf(music, sizeof music, "the CD in %s", cd);
    /* (set up already, but without music: only that) */
    music_only = ((*image && !strcmp(image, source)) || (cd_music > 0 && !strcmp(cd, source))) &&
                 os_file_exists(done) && !has_music(game_data);
    if (music_only)
        snprintf(text, sizeof text, "OpenCiv2 adds the music to the game in\n%s\nfrom %s (encoding it as Ogg "
                 "Vorbis).\n\nThis takes a few minutes; then the game starts.", game_data, music);
    else
        snprintf(text, sizeof text, "OpenCiv2 sets up the game from %s, %s it into\n%s%s%s%s\n\nThis takes %s; then "
                 "the game starts.", source, setup_game_folder(source) ? "copying" : "unpacking", game_data,
                 *music ? "\n\nThe music from " : "", music, *music ? " (encoded as Ogg Vorbis)." : "",
                 *music ? "a few minutes" : "a minute or two");
    if (!os_confirm(text))
        return 0;
    copy_dest = game_data;
    os_progress_open();
    ok = os_program_files() && set_up(game_data, source, cd, image, cd_music, music_only);
    cancelled = !ok && os_progress_stopped();
    os_progress_close();
    if (cancelled)
        os_message("The set-up was stopped; it starts again the next time.", OS_INFO);
    return ok;
}
