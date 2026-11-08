/*
 * macOS and Linux: the system's part of the set-up (setup_os.h).  The game
 * is found in the usual folders (also in Wine's and CrossOver's), or its CD
 * (or a copy of it) mounted, or a disc image of the CD in the user's
 * folders or on a mounted volume; GameData is in the user's data folder
 * (macOS: ~/Library/Application Support/OpenCiv2, Linux:
 * ~/.local/share/OpenCiv2); the program's own files are beside it, in data.
 * Messages are SDL's; the folder is asked with the system's dialog (macOS:
 * setup_mac.m; Linux: zenity or kdialog, where there is one).  The CD's
 * music is read from its drive with the system's ioctls (Linux: cdrom.h's,
 * macOS: IOKit's IOCDMediaBSDClient.h's).
 */
#include <dirent.h>
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef __APPLE__
#include <sys/mount.h>
#include <IOKit/storage/IOCDMediaBSDClient.h>
#else
#include <linux/cdrom.h>
#endif
#include "SDL.h"
#include "setup.h"
#include "setup_os.h"

#define TITLE "OpenCiv2"

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

/* (writable, though read-only on the CD) */
int os_copy_file(const char *from, const char *to)
{
    char buf[1 << 16];
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
    if (!getcwd(out, size))
        snprintf(out, size, ".");
}

static const char *home(void)
{
    const char *h = getenv("HOME");
    return h && *h ? h : ".";
}

void os_game_data(char *out, size_t size)
{
#ifdef __APPLE__
    snprintf(out, size, "%s/Library/Application Support/OpenCiv2/GameData", home());
#else
    const char *x = getenv("XDG_DATA_HOME");
    if (x && *x == '/')
        snprintf(out, size, "%s/OpenCiv2/GameData", x);
    else
        snprintf(out, size, "%s/.local/share/OpenCiv2/GameData", home());
#endif
}

/* (in data beside the program already) */
int os_program_files(void)
{
    return 1;
}

/* ---- where the game is ---- */

/* The first of the folders pattern (glob's) names for which is(folder). */
static int find(const char *pattern, int (*is)(const char *), char *out, size_t size)
{
    glob_t g;
    size_t i;
    int found = 0;
    if (glob(pattern, 0, NULL, &g))
        return 0;
    for (i = 0; i < g.gl_pathc && !found; i++)
        if (is(g.gl_pathv[i])) {
            snprintf(out, size, "%s", g.gl_pathv[i]);
            found = 1;
        }
    globfree(&g);
    return found;
}

int os_find_installed(char *out, size_t size)
{
    /* where Windows' installer would put it, in a Wine (or CrossOver) drive C: */
    static const char *const in_c[] = {"MPS/CIV2", "Program Files (x86)/MicroProse Software/Civilization II Multiplayer "
                                       "Gold Edition", "Program Files/MicroProse Software/Civilization II Multiplayer Gold "
                                       "Edition", "Civ2", "Games/Civ2"};
    static const char *const drives_c[] = {"%s/.wine/drive_c",
#ifdef __APPLE__
                                           "%s/Library/Application Support/CrossOver/Bottles/*/drive_c",
#endif
                                           "%s/Games/*/drive_c"};
    static const char *const others[] = {"%s/Civ2", "%s/civ2", "%s/Games/Civ2", "%s/Games/civ2"};
    char pattern[SETUP_PATH], c[SETUP_PATH];
    const char *v = getenv("CIV2_GAME");
    size_t i, k;
    if (v && *v && setup_game_folder(v)) {
        snprintf(out, size, "%s", v);
        return 1;
    }
    for (i = 0; i < sizeof others / sizeof others[0]; i++) {
        snprintf(pattern, sizeof pattern, others[i], home());
        if (find(pattern, setup_game_folder, out, size))
            return 1;
    }
    for (i = 0; i <= sizeof drives_c / sizeof drives_c[0]; i++) {
        if (i < sizeof drives_c / sizeof drives_c[0])
            snprintf(c, sizeof c, drives_c[i], home());
        else if ((v = getenv("WINEPREFIX")) && *v)
            snprintf(c, sizeof c, "%s/drive_c", v);
        else
            continue;
        for (k = 0; k < sizeof in_c / sizeof in_c[0]; k++) {
            snprintf(pattern, sizeof pattern, "%s/%s", c, in_c[k]);
            if (find(pattern, setup_game_folder, out, size))
                return 1;
        }
    }
    return 0;
}

/* The CD (or a copy of it, as a disk image) mounted. */
int os_find_cd(char *out, size_t size)
{
    static const char *const mounts[] = {
#ifdef __APPLE__
        "/Volumes/*",
#else
        "/media/*/*", "/run/media/*/*", "/media/*", "/mnt/*", "/cdrom",
#endif
    };
    size_t i;
    for (i = 0; i < sizeof mounts / sizeof mounts[0]; i++)
        if (find(mounts[i], setup_game_cd, out, size))
            return 1;
    return 0;
}

/* A disc image of the CD: in the user's folders (setup_image_in_home), or
   on a mounted volume (a USB stick...), at its top or in a folder there. */
int os_find_image(char *out, size_t size)
{
    static const char *const mounts[] = {
#ifdef __APPLE__
        "/Volumes/*",
#else
        "/media/*/*", "/run/media/*/*", "/mnt/*",
#endif
    };
    glob_t g;
    size_t i, k;
    int found = setup_image_in_home(home(), out, size);
    for (i = 0; !found && i < sizeof mounts / sizeof mounts[0]; i++) {
        if (glob(mounts[i], 0, NULL, &g))
            continue;
        for (k = 0; !found && k < g.gl_pathc; k++)
            found = os_dir_exists(g.gl_pathv[k]) && setup_image_in(g.gl_pathv[k], 1, out, size);
        globfree(&g);
    }
    return found;
}

/* ---- talking to the user ---- */

/* The text for a message box: SDL's on X11 breaks lines only at '\n', so
   a long message was one line wider than the screen; there it is broken
   at spaces, lines of at most 72 characters (macOS' boxes wrap their own). */
static const char *box_text(const char *text)
{
#ifdef __APPLE__
    return text;
#else
    static char out[4 * SETUP_PATH + 512];
    size_t i, n = strlen(text), line = 0, space = 0;
    if (n >= sizeof out)
        n = sizeof out - 1;
    memcpy(out, text, n);
    out[n] = 0;
    for (i = 0; i < n; i++) {
        if (out[i] == '\n') {
            line = i + 1;
            space = 0;
        } else if (out[i] == ' ') {
            space = i;
        }
        if (i - line >= 72 && space > line) {
            out[space] = '\n';
            line = space + 1;
            space = 0;
        }
    }
    return out;
#endif
}

void os_message(const char *text, int kind)
{
    Uint32 flags = kind == OS_ERROR ? SDL_MESSAGEBOX_ERROR : kind == OS_WARNING ? SDL_MESSAGEBOX_WARNING :
                   SDL_MESSAGEBOX_INFORMATION;
    if (SDL_ShowSimpleMessageBox(flags, TITLE, box_text(text), NULL))
        fprintf(stderr, "OpenCiv2: %s\n", text);
}

#ifdef __APPLE__
int setup_mac_ask_folder(const char *prompt, char *out, size_t size);     /* setup_mac.m */
#else
/* The folder chosen with the command's dialog; 0: none (or no command). */
static int ask_with(const char *command, char *out, size_t size)
{
    FILE *p = popen(command, "r");
    size_t n;
    int st;
    if (!p)
        return 0;
    n = fread(out, 1, size - 1, p);
    st = pclose(p);
    out[n] = 0;
    while (n && (out[n - 1] == '\n' || out[n - 1] == '\r'))
        out[--n] = 0;
    return st == 0 && n > 0;
}
#endif

int os_ask_folder(char *out, size_t size)
{
    static const char prompt[] = "Where is Civilization II Multiplayer Gold Edition? Choose the folder it is "
                                 "installed in, its CD, or a disc image of the CD (its .cue file, or the folder it "
                                 "is in).";
#ifdef __APPLE__
    return setup_mac_ask_folder(prompt, out, size);
#else
    if (ask_with("zenity --file-selection --directory --title='Where is Civilization II Multiplayer Gold Edition? "
                 "(the folder it is installed in, its CD, or the folder of a disc image of the CD)' 2>/dev/null",
                 out, size) ||
        ask_with("kdialog --getexistingdirectory ~ --title 'Where is Civilization II Multiplayer Gold Edition? "
                 "(the folder it is installed in, its CD, or the folder of a disc image of the CD)' 2>/dev/null",
                 out, size))
        return 1;
    if (system("command -v zenity >/dev/null 2>&1 || command -v kdialog >/dev/null 2>&1")) {
        char text[512];
        snprintf(text, sizeof text, "%s\n\nStart OpenCiv2 with that folder (or the .cue file): OpenCiv2 --setup "
                 "FOLDER", prompt);
        os_message(text, OS_INFO);
    }
    return 0;
#endif
}

int os_confirm(const char *text)
{
    static const SDL_MessageBoxButtonData buttons[] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Set up"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Quit"},
    };
    SDL_MessageBoxData box;
    int id;
    memset(&box, 0, sizeof box);
    box.flags = SDL_MESSAGEBOX_INFORMATION;
    box.title = TITLE;
    box.message = box_text(text);
    box.numbuttons = 2;
    box.buttons = buttons;
    if (SDL_ShowMessageBox(&box, &id)) {
        fprintf(stderr, "OpenCiv2: %s\n", text);       /* (no display for it: go on) */
        return 1;
    }
    return id == 1;
}

/* (no window: what is done, on the terminal) */
void os_progress_open(void)
{
}

int os_progress_show(const char *what, const char *detail)
{
    static char last[2 * SETUP_PATH];
    char text[sizeof last];
    snprintf(text, sizeof text, "%s %s", what, detail ? detail : "");
    if (strcmp(text, last)) {
        fprintf(stderr, "%s\n", text);
        snprintf(last, sizeof last, "%s", text);
    }
    return 0;
}

int os_progress_stopped(void)
{
    return 0;
}

void os_progress_close(void)
{
}

/* ---- the CD's music, from its drive ---- */

#define CD_SECTOR 2352

struct cd {
    int fd;
};

/* The drive's device (Linux: /dev/srN; macOS: the whole disc's raw
   /dev/rdiskN) for the CD mounted at cd; 0: not on a CD drive. */
static int cd_device(const char *cd, char *out, size_t size)
{
#ifdef __APPLE__
    struct statfs st;
    const char *p;
    size_t n;
    if (statfs(cd, &st) || strncmp(st.f_mntfromname, "/dev/disk", 9))
        return 0;
    p = st.f_mntfromname + 9;
    n = strspn(p, "0123456789");
    if (!n)
        return 0;
    snprintf(out, size, "/dev/rdisk%.*s", (int)n, p);
    return 1;
#else
    /* (the mount point in /proc/mounts: its spaces... as \040...) */
    char line[2 * SETUP_PATH], dev[SETUP_PATH], dir[SETUP_PATH], real[PATH_MAX], want[PATH_MAX];
    FILE *f = fopen("/proc/mounts", "r");
    int found = 0;
    if (!f)
        return 0;
    if (!realpath(cd, want))
        snprintf(want, sizeof want, "%s", cd);
    while (!found && fgets(line, sizeof line, f)) {
        char *r, *w;
        if (sscanf(line, "%4095s %4095s", dev, dir) != 2)
            continue;
        for (r = w = dir; *r; w++)
            if (r[0] == '\\' && r[1] >= '0' && r[1] <= '3' && r[2] >= '0' && r[2] <= '7' && r[3] >= '0' &&
                r[3] <= '7') {
                *w = (char)((r[1] - '0') << 6 | (r[2] - '0') << 3 | (r[3] - '0'));
                r += 4;
            } else {
                *w = *r++;
            }
        *w = 0;
        found = !strcmp(dir, want);
    }
    fclose(f);
    /* (a CD drive: /dev/srN, the name /dev/cdrom... may point to) */
    if (!found || !realpath(dev, real) || strncmp(real, "/dev/sr", 7))
        return 0;
    snprintf(out, size, "%s", real);
    return 1;
#endif
}

int os_cd_open(const char *cd, struct cd_toc *toc, void **h)
{
    char dev[SETUP_PATH];
    struct cd *c;
    int fd, ok = 1;
    if (!cd_device(cd, dev, sizeof dev))
        return 0;
#ifdef __APPLE__
    fd = open(dev, O_RDONLY);
#else
    fd = open(dev, O_RDONLY | O_NONBLOCK);
#endif
    if (fd < 0)
        return -1;
    memset(toc, 0, sizeof *toc);
    {
#ifdef __APPLE__
        /* the full TOC (kCDTOCFormatTOC): a header (length, sessions), then
           11-byte descriptors: session, ADR (high 4 bits) and control (low 4,
           4: data), TNO, POINT (1-99: a track, 0xA2: the lead-out), ...,
           PMIN, PSEC, PFRAME (where it starts) */
        static unsigned char buf[4 + 11 * 256];
        dk_cd_read_toc_t r;
        unsigned len, i, k;
        long leadout = -1;
        memset(&r, 0, sizeof r);
        r.format = kCDTOCFormatTOC;
        r.formatAsTime = 1;
        r.address.session = 1;
        r.bufferLength = sizeof buf;
        r.buffer = buf;
        if (ioctl(fd, DKIOCCDREADTOC, &r) < 0 || r.bufferLength < 4) {
            ok = 0;
        } else {
            len = (unsigned)(buf[0] << 8 | buf[1]) + 2;
            if (len > r.bufferLength)
                len = r.bufferLength;
            for (i = 4; i + 11 <= len; i += 11) {
                const unsigned char *d = buf + i;
                long lba = ((long)d[8] * 60 + d[9]) * 75 + d[10] - 150;
                if (d[1] >> 4 != 1)
                    continue;
                if (d[3] >= 1 && d[3] <= 99 && toc->ntracks < 99) {
                    /* (in the order of their numbers) */
                    for (k = (unsigned)toc->ntracks; k > 0 && toc->t[k - 1].number > d[3]; k--)
                        toc->t[k] = toc->t[k - 1];
                    toc->t[k].number = d[3];
                    toc->t[k].audio = !(d[1] & 4);
                    toc->t[k].lba = lba;
                    toc->ntracks++;
                } else if (d[3] == 0xA2 && lba > leadout) {
                    leadout = lba;
                }
            }
            toc->leadout = leadout;
            ok = toc->ntracks > 0 && leadout > 0;
        }
#else
        struct cdrom_tochdr hdr;
        struct cdrom_tocentry e;
        int t;
        if (ioctl(fd, CDROMREADTOCHDR, &hdr) < 0) {
            ok = 0;
        } else {
            for (t = hdr.cdth_trk0; ok && t <= hdr.cdth_trk1 && toc->ntracks < 99; t++) {
                memset(&e, 0, sizeof e);
                e.cdte_track = (unsigned char)t;
                e.cdte_format = CDROM_LBA;
                if (ioctl(fd, CDROMREADTOCENTRY, &e) < 0) {
                    ok = 0;
                } else {
                    toc->t[toc->ntracks].number = t;
                    toc->t[toc->ntracks].audio = !(e.cdte_ctrl & CDROM_DATA_TRACK);
                    toc->t[toc->ntracks].lba = e.cdte_addr.lba;
                    toc->ntracks++;
                }
            }
            memset(&e, 0, sizeof e);
            e.cdte_track = CDROM_LEADOUT;
            e.cdte_format = CDROM_LBA;
            if (ok && ioctl(fd, CDROMREADTOCENTRY, &e) < 0)
                ok = 0;
            toc->leadout = e.cdte_addr.lba;
        }
#endif
    }
    if (!ok || !(c = malloc(sizeof *c))) {
        close(fd);
        return -1;
    }
    c->fd = fd;
    *h = c;
    return 1;
}

int os_cd_read(void *h, long lba, int n, unsigned char *buf)
{
    const struct cd *c = h;
#ifdef __APPLE__
    dk_cd_read_t r;
    memset(&r, 0, sizeof r);
    r.offset = (uint64_t)lba * CD_SECTOR;
    r.sectorArea = kCDSectorAreaUser;
    r.sectorType = kCDSectorTypeCDDA;
    r.bufferLength = (uint32_t)n * CD_SECTOR;
    r.buffer = buf;
    return ioctl(c->fd, DKIOCCDREAD, &r) == 0 && r.bufferLength == (uint32_t)n * CD_SECTOR;
#else
    struct cdrom_read_audio r;
    memset(&r, 0, sizeof r);
    r.addr.lba = (int)lba;
    r.addr_format = CDROM_LBA;
    r.nframes = n;
    r.buf = buf;
    return ioctl(c->fd, CDROMREADAUDIO, &r) == 0;
#endif
}

void os_cd_close(void *h)
{
    struct cd *c = h;
    close(c->fd);
    free(c);
}
