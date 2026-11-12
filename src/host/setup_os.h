/*
 * What the set-up (setup.c) needs of the system: files and folders, where
 * the game usually is, and talking to the user.  setup_win.c on Windows,
 * setup_unix.c on macOS and Linux (with setup_mac.m on macOS).  And what
 * it reads of a disc image of the CD, or of the CD's music in its drive
 * (setup_disc.c).
 */
#ifndef CIV2_SETUP_OS_H
#define CIV2_SETUP_OS_H

#include <stddef.h>

#ifdef _WIN32
#define SEP '\\'
#define SEP_S "\\"
#else
#define SEP '/'
#define SEP_S "/"
#endif

#define SETUP_PATH 4096

/* (setup.c) an installed game's folder; the game's CD */
int setup_game_folder(const char *dir);
int setup_game_cd(const char *root);
/* (setup.c) a disc image of the game's CD in the usual folders of the
   user's home (Downloads, Desktop, the home, Civ2*); 0: none */
int setup_image_in_home(const char *home, char *out, size_t size);

/* (setup_disc.c) a disc image of the game's CD: a .cue with its .bin */
int setup_game_image(const char *path);
/* one in dir (depth: in its folders too, that deep) into out; 0: none */
int setup_image_in(const char *dir, int depth, char *out, size_t size);
/* the installer's cabinet (DATA1.CAB, its other volumes and headers)
   copied out of the image into dir; cab: DATA1.CAB there.  0: failed */
int image_extract_cabinet(const char *cue, const char *dir, char *cab, size_t size);
/* the image's CIV2/KINGS, VIDEO into game_dir, where it has none */
int image_copy_videos(const char *cue, const char *game_dir);
/* its audio tracks as game_dir/Music/TrackNN.ogg (Ogg Vorbis); with
   --CueSheetFuzzyMatch, the boundaries between them moved to the silence
   nearby */
int image_encode_music(const char *cue, const char *game_dir);
/* (setup.c) the command line has this flag (any case) */
int setup_has_flag(const char *flag);
/* the music of the CD (cd: its folder) read from its drive (os_cd_open):
   1: there is (the drive reads its audio tracks), 0: none (not a CD in
   a drive, no audio tracks), -1: the drive could not be read */
int cd_has_music(const char *cd);
/* its audio tracks as game_dir/Music/TrackNN.ogg, as an image's: 1: done
   (or none), 0: failed (said) or stopped, -1: the drive could not be read */
int cd_encode_music(const char *cd, const char *game_dir);

/* A CD's table of contents: its tracks' starts, in sectors from the
   start of the disc (LBA). */
struct cd_toc {
    int ntracks;
    struct {
        int number, audio;
        long lba;
    } t[99];
    long leadout;               /* where the last track ends */
};
/* (setup_win.c, setup_unix.c) the drive the CD (cd: its folder, where it
   is mounted) is in, opened (*h) and its table of contents read: 1;
   0: cd is not on a CD drive; -1: the drive could not be opened or read */
int os_cd_open(const char *cd, struct cd_toc *toc, void **h);
/* n (at most 16) raw audio sectors, 2352 bytes each, from sector lba on
   into buf; 0: failed */
int os_cd_read(void *h, long lba, int n, unsigned char *buf);
void os_cd_close(void *h);

enum { OS_INFO, OS_WARNING, OS_ERROR };

int os_dir_exists(const char *path);
int os_file_exists(const char *path);
void os_make_dirs(const char *path);
/* an empty folder removed */
void os_remove_dir(const char *path);
/* dir's entry called name, whatever its case, into out; 0: none */
int os_entry(const char *dir, const char *name, char *out, size_t size);
/* each of dir's entries (not . and ..) to fn until it returns 0; 0: stopped */
int os_each(const char *dir, int (*fn)(void *ctx, const char *name, int is_dir), void *ctx);
int os_copy_file(const char *from, const char *to);
int os_same_path(const char *a, const char *b);
void os_current_dir(char *out, size_t size);

/* GameData: where the game is set up (and the folder it is in made) */
void os_game_data(char *out, size_t size);
/* the program's own files (Windows: the fonts, from its resources); 0: failed */
int os_program_files(void);
/* where the game usually is: installed, its CD in a drive, a disc image
   of it (setup_image_in_home, mounted volumes...); 0: not found */
int os_find_installed(char *out, size_t size);
int os_find_cd(char *out, size_t size);
int os_find_image(char *out, size_t size);

void os_message(const char *text, int kind);
/* the user's choice of folder: the installed game or the CD (or a disc
   image: its .cue, where the dialog can choose a file); 0: none */
int os_ask_folder(char *out, size_t size);
/* the set-up about to start (text: what it does): 0: the user said no */
int os_confirm(const char *text);
void os_progress_open(void);
/* what is being done (called often while it is long; the same text again
   shown once); 0: go on, 1: the user stopped it */
int os_progress_show(const char *what, const char *detail);
int os_progress_stopped(void);
void os_progress_close(void);

#endif
