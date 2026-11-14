/*
 * The browser version's set-up from the game's CD or a disc image of it:
 * src/host/setup.c and setup_disc.c (with unshield, zlib, libogg and
 * libvorbis) compiled to their own WebAssembly module, setup.wasm
 * (tools/build_web.py), run in a Web Worker (src/web/setup_worker.js)
 * over WASI: the files the player chose under /src (read from the disc
 * image or the CD as the set-up asks for them), the game set up into
 * /game, each file handed to the page as it is written.  This is the
 * system's part (setup_os.h) for that: plain files, no CD drive (the music
 * comes from a disc image only), no dialogs (the page asked already);
 * messages and progress go to the page (the "setup" imports).
 *
 *   setup.wasm SOURCE GAME_DIR   (SOURCE: a .cue, or the CD's folder)
 */
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include "setup.h"
#include "setup_os.h"

#define IMPORT(name) __attribute__((import_module("setup"), import_name(name)))

/* The page: what is being done (0: go on, else stop); a message (OS_*). */
IMPORT("progress") int web_progress(const char *what, const char *detail);
IMPORT("message") void web_message(const char *text, int kind);

static const char *game_dir = "/game";
static int stopped;

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
    char p[SETUP_PATH];
    size_t i;
    snprintf(p, sizeof p, "%s", path);
    for (i = 1; p[i]; i++)
        if (p[i] == '/') {
            p[i] = 0;
            mkdir(p, 0755);
            p[i] = '/';
        }
    mkdir(p, 0755);
}

void os_remove_dir(const char *path)
{
    rmdir(path);
}

int os_entry(const char *dir, const char *name, char *out, size_t size)
{
    DIR *d;
    struct dirent *e;
    int found = 0;
    snprintf(out, size, "%s/%s", dir, name);
    if (!access(out, F_OK))
        return 1;
    if (!(d = opendir(dir)))
        return 0;
    while (!found && (e = readdir(d)))
        if (!strcasecmp(e->d_name, name)) {
            snprintf(out, size, "%s/%s", dir, e->d_name);
            found = 1;
        }
    closedir(d);
    return found;
}

int os_each(const char *dir, int (*fn)(void *ctx, const char *name, int is_dir), void *ctx)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    int ok = 1;
    if (!d)
        return 1;
    while (ok && (e = readdir(d))) {
        char p[SETUP_PATH];
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
            continue;
        snprintf(p, sizeof p, "%s/%s", dir, e->d_name);
        ok = fn(ctx, e->d_name, os_dir_exists(p));
    }
    closedir(d);
    return ok;
}

int os_copy_file(const char *from, const char *to)
{
    static char buf[1 << 20];
    size_t n;
    FILE *in = fopen(from, "rb"), *out;
    int ok = 1;
    if (!in)
        return 0;
    if (!(out = fopen(to, "wb"))) {
        fclose(in);
        return 0;
    }
    while (ok && (n = fread(buf, 1, sizeof buf, in)) > 0)
        ok = fwrite(buf, 1, n, out) == n;
    ok = ok && !ferror(in);
    fclose(in);
    if (fclose(out) != 0)
        ok = 0;
    if (!ok)
        remove(to);
    return ok;
}

int os_same_path(const char *a, const char *b)
{
    return !strcmp(a, b);
}

void os_current_dir(char *out, size_t size)
{
    snprintf(out, size, "/");
}

void os_game_data(char *out, size_t size)
{
    snprintf(out, size, "%s", game_dir);
}

int os_program_files(void)
{
    return 1;
}

/* (the page says where the game is: nothing is looked for) */
int os_find_installed(char *out, size_t size)
{
    (void)out, (void)size;
    return 0;
}

int os_find_cd(char *out, size_t size)
{
    (void)out, (void)size;
    return 0;
}

int os_find_image(char *out, size_t size)
{
    (void)out, (void)size;
    return 0;
}

void os_message(const char *text, int kind)
{
    web_message(text, kind);
}

int os_ask_folder(char *out, size_t size)
{
    (void)out, (void)size;
    return 0;
}

int os_confirm(const char *text)
{
    (void)text;
    return 1;
}

void os_progress_open(void)
{
}

int os_progress_show(const char *what, const char *detail)
{
    if (!stopped && web_progress(what, detail ? detail : ""))
        stopped = 1;
    return stopped;
}

int os_progress_stopped(void)
{
    return stopped;
}

void os_progress_close(void)
{
}

/* (no CD drive) */
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

/* 0: set up; 1: not (said). */
int main(int argc, char **argv)
{
    char game[SETUP_PATH];
    char *args[3];
    if (argc < 3) {
        fprintf(stderr, "usage: setup.wasm SOURCE GAME_DIR\n");
        return 2;
    }
    game_dir = argv[2];
    args[0] = argv[0];
    args[1] = "--setup";
    args[2] = argv[1];
    return setup_choose_game(3, args, game, sizeof game) ? 0 : 1;
}
