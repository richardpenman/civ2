/*
 * The host's part of the opening window (src/platform/launcher.c, host.h):
 * whether Shift is held down at start, the user's own OpenCiv2 folder, and
 * the HD graphics' art (docs/HD-GRAPHICS.md) downloaded, checked and
 * installed there, in the background, as tools/fetch_hd_pack.py does it.
 *
 * The pack is the release pinned in data/hd-pack.json (the build passes its
 * url, sha256 and size: HD_PACK_URL, HD_PACK_SHA256, HD_PACK_SIZE).  The
 * archive is downloaded by the system's curl (Windows 10 and later have
 * curl.exe; on Linux wget where there is no curl) into the cache
 * (${XDG_CACHE_HOME:-~/.cache}/openciv2, %LOCALAPPDATA%\openciv2 on Windows;
 * CIV2_DEPENDENCY_CACHE), named by its SHA-256, so that it is fetched once
 * (and shared with tools/fetch_hd_pack.py); it must have the pinned SHA-256
 * and each file installed the SHA-256 and size its package.json lists:
 * nothing is installed otherwise.
 */
#include "host.h"
#include <SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define SEP "\\"
#else
#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#define SEP "/"
extern char **environ;
#endif
#ifdef __APPLE__
#include "host_menu.h"
#endif
#if defined(__linux__)
#include <dlfcn.h>
#endif

#ifndef HD_PACK_URL         /* (tools/build_port.py and build_native.py pass data/hd-pack.json's) */
#define HD_PACK_TAG ""
#define HD_PACK_URL ""
#define HD_PACK_SHA256 ""
#define HD_PACK_SIZE 0
#endif

/* ---- Shift at start ---- */

#if defined(__linux__)
/* X11 (also XWayland): the keyboard's state, whatever window has the
   focus (SDL knows it only for its own windows). */
static int x11_shift_held(void)
{
    typedef void *(*OpenDisplay)(const char *);
    typedef int (*CloseDisplay)(void *);
    typedef int (*QueryKeymap)(void *, char *);
    typedef unsigned char (*KeysymToKeycode)(void *, unsigned long);
    void *lib = dlopen("libX11.so.6", RTLD_LAZY | RTLD_LOCAL), *d;
    OpenDisplay open_display;
    CloseDisplay close_display;
    QueryKeymap query_keymap;
    KeysymToKeycode to_keycode;
    char keys[32];
    int held = 0, i;
    if (!lib)
        return 0;
    open_display = (OpenDisplay)dlsym(lib, "XOpenDisplay");
    close_display = (CloseDisplay)dlsym(lib, "XCloseDisplay");
    query_keymap = (QueryKeymap)dlsym(lib, "XQueryKeymap");
    to_keycode = (KeysymToKeycode)dlsym(lib, "XKeysymToKeycode");
    if (open_display && close_display && query_keymap && to_keycode && (d = open_display(NULL))) {
        if (query_keymap(d, keys))
            for (i = 0; i < 2; i++) {
                unsigned code = to_keycode(d, i ? 0xffe2 : 0xffe1);     /* XK_Shift_R, XK_Shift_L */
                if (code && (keys[code >> 3] >> (code & 7)) & 1)
                    held = 1;
            }
        close_display(d);
    }
    dlclose(lib);
    return held;
}
#endif

int host_launcher(void)
{
#if defined(_WIN32)
    return GetAsyncKeyState(VK_SHIFT) < 0;
#elif defined(__APPLE__)
    return host_mac_shift_held();
#elif defined(__linux__)
    return x11_shift_held();
#else
    return 0;
#endif
}

/* ---- folders ---- */

static void make_dirs(const char *path)
{
    char p[4096], *s;
    snprintf(p, sizeof p, "%s", path);
    for (s = p + 1; *s; s++)
        if (*s == '/' || *s == '\\') {
            char c = *s;
            *s = 0;
#ifdef _WIN32
            if (!(s == p + 2 && p[1] == ':'))
                _mkdir(p);
#else
            mkdir(p, 0755);
#endif
            *s = c;
        }
#ifdef _WIN32
    _mkdir(p);
#else
    mkdir(p, 0755);
#endif
}

#ifndef _WIN32
static const char *home(void)
{
    const char *h = getenv("HOME");
    return h && *h ? h : ".";
}
#endif

int host_user_dir(char *buf, int size)
{
#ifdef _WIN32
    const char *l = getenv("LOCALAPPDATA");
    if (!l || !*l)
        return 0;
    snprintf(buf, (size_t)size, "%s\\OpenCiv2", l);
#elif defined(__APPLE__)
    snprintf(buf, (size_t)size, "%s/Library/Application Support/OpenCiv2", home());
#else
    const char *x = getenv("XDG_DATA_HOME");
    if (x && *x == '/')
        snprintf(buf, (size_t)size, "%s/OpenCiv2", x);
    else
        snprintf(buf, (size_t)size, "%s/.local/share/OpenCiv2", home());
#endif
    make_dirs(buf);
    return (int)strlen(buf);
}

/* As tools/fetch_hd_pack.py's cache_dir. */
static void cache_dir(char *out, size_t size)
{
    const char *d = getenv("CIV2_DEPENDENCY_CACHE");
    if (d && *d)
        snprintf(out, size, "%s", d);
    else {
#ifdef _WIN32
        const char *l = getenv("LOCALAPPDATA");
        snprintf(out, size, "%s\\openciv2", l && *l ? l : ".");
#else
        const char *x = getenv("XDG_CACHE_HOME");
        if (x && *x)
            snprintf(out, size, "%s/openciv2", x);
        else
            snprintf(out, size, "%s/.cache/openciv2", home());
#endif
    }
    make_dirs(out);
}

static long file_size(const char *path)
{
    struct stat st;
    return stat(path, &st) ? -1 : (long)st.st_size;
}

static int replace_file(const char *from, const char *to)
{
#ifdef _WIN32
    remove(to);
#endif
    return rename(from, to) == 0;
}

/* ---- SHA-256 (FIPS 180-4) ---- */

typedef struct {
    uint32_t h[8];
    uint64_t len;
    unsigned char buf[64];
    int n;
} Sha256;

static const uint32_t sha_k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98,
    0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8,
    0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
    0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
    0xc67178f2};

#define ROR(x, n) ((x) >> (n) | (x) << (32 - (n)))

static void sha_block(Sha256 *s, const unsigned char *p)
{
    uint32_t w[64], a, b, c, d, e, f, g, h, t1, t2;
    int i;
    for (i = 0; i < 16; i++)
        w[i] = (uint32_t)p[4 * i] << 24 | (uint32_t)p[4 * i + 1] << 16 | (uint32_t)p[4 * i + 2] << 8 | p[4 * i + 3];
    for (i = 16; i < 64; i++)
        w[i] = (ROR(w[i - 2], 17) ^ ROR(w[i - 2], 19) ^ (w[i - 2] >> 10)) + w[i - 7] +
               (ROR(w[i - 15], 7) ^ ROR(w[i - 15], 18) ^ (w[i - 15] >> 3)) + w[i - 16];
    a = s->h[0]; b = s->h[1]; c = s->h[2]; d = s->h[3]; e = s->h[4]; f = s->h[5]; g = s->h[6]; h = s->h[7];
    for (i = 0; i < 64; i++) {
        t1 = h + (ROR(e, 6) ^ ROR(e, 11) ^ ROR(e, 25)) + ((e & f) ^ (~e & g)) + sha_k[i] + w[i];
        t2 = (ROR(a, 2) ^ ROR(a, 13) ^ ROR(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
    }
    s->h[0] += a; s->h[1] += b; s->h[2] += c; s->h[3] += d; s->h[4] += e; s->h[5] += f; s->h[6] += g; s->h[7] += h;
}

static void sha_init(Sha256 *s)
{
    static const uint32_t h0[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                   0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    memcpy(s->h, h0, sizeof h0);
    s->len = 0;
    s->n = 0;
}

static void sha_update(Sha256 *s, const void *data, size_t n)
{
    const unsigned char *p = data;
    s->len += n;
    while (n) {
        size_t k = 64 - (size_t)s->n < n ? 64 - (size_t)s->n : n;
        memcpy(s->buf + s->n, p, k);
        s->n += (int)k;
        p += k;
        n -= k;
        if (s->n == 64) {
            sha_block(s, s->buf);
            s->n = 0;
        }
    }
}

/* The digest as 64 lower-case hex digits. */
static void sha_hex(Sha256 *s, char out[65])
{
    uint64_t bits = s->len * 8;
    unsigned char pad = 0x80, z = 0, len[8];
    int i;
    sha_update(s, &pad, 1);
    while (s->n != 56)
        sha_update(s, &z, 1);
    for (i = 0; i < 8; i++)
        len[i] = (unsigned char)(bits >> (56 - 8 * i));
    sha_update(s, len, 8);
    for (i = 0; i < 32; i++)
        snprintf(out + 2 * i, 3, "%02x", (unsigned)(s->h[i >> 2] >> (24 - 8 * (i & 3))) & 0xff);
}

/* ---- the background work ---- */

static SDL_mutex *lock;
static SDL_Thread *thread;
static volatile int cancel_wanted;
static struct {
    int state, phase;
    long done, total;           /* bytes */
    char msg[512];
} job = {HOST_HD_IDLE, 0, 0, 0, ""};

static void progress(int phase, long done, long total)
{
    SDL_LockMutex(lock);
    job.phase = phase;
    job.done = done;
    job.total = total;
    SDL_UnlockMutex(lock);
}

static int fail(const char *fmt, const char *arg)
{
    SDL_LockMutex(lock);
    snprintf(job.msg, sizeof job.msg, fmt, arg ? arg : "");
    SDL_UnlockMutex(lock);
    return HOST_HD_FAILED;
}

/* The SHA-256 of a file as it is read (progress: HOST_HD_CHECKING); 0: stopped or unreadable. */
static int sha_file(const char *path, char hex[65])
{
    static unsigned char buf[1 << 16];
    FILE *f = fopen(path, "rb");
    long total = file_size(path), done = 0;
    size_t n;
    Sha256 s;
    if (!f)
        return 0;
    sha_init(&s);
    while ((n = fread(buf, 1, sizeof buf, f)) > 0) {
        sha_update(&s, buf, n);
        done += (long)n;
        if (!(done & 0xfffff))
            progress(HOST_HD_CHECKING, done, total);
        if (cancel_wanted) {
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    sha_hex(&s, hex);
    return 1;
}

/* url downloaded into path (progress by the file's size); 1 done, 0 failed or stopped. */
static int download(const char *url, const char *path, long size)
{
#ifdef _WIN32
    char cmd[8192];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    DWORD code = 1;
    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    snprintf(cmd, sizeof cmd, "curl.exe -fsSL --retry 3 -o \"%s\" \"%s\"", path, url);
    if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
        return fail("Windows' curl.exe, which downloads the art, was not found (it comes with Windows 10 and "
                    "later).%s", NULL) * 0;
    while (WaitForSingleObject(pi.hProcess, 200) == WAIT_TIMEOUT) {
        progress(HOST_HD_DOWNLOADING, file_size(path), size);
        if (cancel_wanted)
            TerminateProcess(pi.hProcess, 1);
    }
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    if (cancel_wanted)
        return 0;
    if (code != 0) {
        char n[32];
        snprintf(n, sizeof n, "%lu", (unsigned long)code);
        return fail("The download failed (curl's error %s). Is the computer connected to the internet?", n) * 0;
    }
    return 1;
#else
    char *const curl[] = {"curl", "-fsSL", "--retry", "3", "-o", (char *)path, (char *)url, NULL};
    char *const wget[] = {"wget", "-q", "-O", (char *)path, (char *)url, NULL};
    char *const *tools[2] = {curl, wget};
    int t, status = 0;
    for (t = 0; t < 2; t++) {
        pid_t pid;
        if (posix_spawnp(&pid, tools[t][0], NULL, NULL, tools[t], environ) != 0)
            continue;
        for (;;) {
            pid_t r = waitpid(pid, &status, WNOHANG);
            if (r == pid || (r < 0 && errno != EINTR))
                break;
            progress(HOST_HD_DOWNLOADING, file_size(path), size);
            if (cancel_wanted)
                kill(pid, SIGTERM);
            SDL_Delay(200);
        }
        if (cancel_wanted)
            return 0;
        if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
            continue;           /* (not found by the child) */
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            return 1;
        {
            char n[64];
            snprintf(n, sizeof n, "%s's error %d", tools[t][0], WIFEXITED(status) ? WEXITSTATUS(status) : -1);
            return fail("The download failed (%s). Is the computer connected to the internet?", n) * 0;
        }
    }
    return fail("Neither curl nor wget, which download the art, was found: please install curl.%s", NULL) * 0;
#endif
}

/* ---- the archive ---- */

typedef struct {
    char name[256];
    unsigned method;
    unsigned long csize, size, offset;     /* (offset: of its local header) */
} ZipEntry;

static unsigned get16(const unsigned char *p) { return p[0] | p[1] << 8; }
static unsigned long get32(const unsigned char *p)
{
    return (unsigned long)p[0] | (unsigned long)p[1] << 8 | (unsigned long)p[2] << 16 | (unsigned long)p[3] << 24;
}

/* The archive's entries from its central directory; their number, -1: not a zip. */
static int zip_entries(FILE *f, ZipEntry *e, int max)
{
    unsigned char tail[65536 + 22], *p;
    long size, start, n, i, k;
    unsigned long dir_at, count;
    int found = 0;
    if (fseek(f, 0, SEEK_END) || (size = ftell(f)) < 22)
        return -1;
    start = size > (long)sizeof tail ? size - (long)sizeof tail : 0;
    n = size - start;
    if (fseek(f, start, SEEK_SET) || fread(tail, 1, (size_t)n, f) != (size_t)n)
        return -1;
    for (i = n - 22; i >= 0; i--)
        if (get32(tail + i) == 0x06054b50)
            break;
    if (i < 0)
        return -1;
    count = get16(tail + i + 10);
    dir_at = get32(tail + i + 16);
    if (fseek(f, (long)dir_at, SEEK_SET))
        return -1;
    for (k = 0; k < (long)count; k++) {
        unsigned char h[46];
        unsigned nl, xl, cl;
        if (fread(h, 1, 46, f) != 46 || get32(h) != 0x02014b50)
            return -1;
        nl = get16(h + 28);
        xl = get16(h + 30);
        cl = get16(h + 32);
        if (found < max && nl < sizeof e->name) {
            p = (unsigned char *)e[found].name;
            if (fread(p, 1, nl, f) != nl)
                return -1;
            e[found].name[nl] = 0;
            e[found].method = get16(h + 10);
            e[found].csize = get32(h + 20);
            e[found].size = get32(h + 24);
            e[found].offset = get32(h + 42);
            found++;
            nl = 0;
        }
        if (fseek(f, (long)(nl + xl + cl), SEEK_CUR))
            return -1;
    }
    return found;
}

static const ZipEntry *zip_find(const ZipEntry *e, int n, const char *name)
{
    int i;
    for (i = 0; i < n; i++)
        if (!strcmp(e[i].name, name))
            return &e[i];
    return NULL;
}

/* An entry unpacked into out (a file, or into mem: at most cap bytes);
   its SHA-256 (hex) too.  1 done, 0 failed or stopped. */
static int zip_extract(FILE *f, const ZipEntry *e, const char *out, char *mem, size_t cap, char hex[65],
                       long *done, long total)
{
    static unsigned char in[1 << 16], buf[1 << 16];
    unsigned char h[30];
    unsigned long left = e->csize, written = 0;
    FILE *o = NULL;
    z_stream z;
    Sha256 s;
    int r = Z_OK, ok = 0;
    if (fseek(f, (long)e->offset, SEEK_SET) || fread(h, 1, 30, f) != 30 || get32(h) != 0x04034b50 ||
        fseek(f, (long)(get16(h + 26) + get16(h + 28)), SEEK_CUR))
        return 0;
    if (e->method != 0 && e->method != 8)
        return 0;
    if (out && !(o = fopen(out, "wb")))
        return 0;
    memset(&z, 0, sizeof z);
    if (e->method == 8 && inflateInit2(&z, -MAX_WBITS) != Z_OK) {
        if (o)
            fclose(o);
        return 0;
    }
    sha_init(&s);
    for (;;) {
        size_t got = 0;
        if (e->method == 0) {
            size_t want = left < sizeof buf ? (size_t)left : sizeof buf;
            if (!want)
                break;
            got = fread(buf, 1, want, f);
            if (got != want)
                break;
            left -= (unsigned long)got;
        } else {
            if (!z.avail_in && left) {
                size_t want = left < sizeof in ? (size_t)left : sizeof in;
                if (fread(in, 1, want, f) != want)
                    break;
                left -= (unsigned long)want;
                z.next_in = in;
                z.avail_in = (uInt)want;
            }
            z.next_out = buf;
            z.avail_out = sizeof buf;
            r = inflate(&z, Z_NO_FLUSH);
            if (r != Z_OK && r != Z_STREAM_END)
                break;
            got = sizeof buf - z.avail_out;
        }
        sha_update(&s, buf, got);
        if (o && fwrite(buf, 1, got, o) != got)
            break;
        if (mem) {
            if (written + got >= cap)
                break;
            memcpy(mem + written, buf, got);
        }
        written += (unsigned long)got;
        *done += (long)got;
        progress(HOST_HD_INSTALLING, *done, total);
        if (cancel_wanted)
            break;
        if (e->method == 8 && r == Z_STREAM_END) {
            ok = 1;
            break;
        }
        if (e->method == 0 && !left) {
            ok = 1;
            break;
        }
        if (e->method == 8 && !got && !z.avail_in && !left)
            break;          /* (truncated) */
    }
    if (e->method == 8)
        inflateEnd(&z);
    if (o && fclose(o))
        ok = 0;
    if (mem && ok)
        mem[written] = 0;
    sha_hex(&s, hex);
    return ok && written == e->size;
}

/* In package.json: the "sha256" and "size" listed for member. */
static int manifest_file(const char *json, const char *member, char sha[65], unsigned long *size)
{
    char key[300];
    const char *p, *q, *end;
    snprintf(key, sizeof key, "\"%s\"", member);
    if (!(p = strstr(json, key)) || !(end = strchr(p, '}')))
        return 0;
    if (!(q = strstr(p, "\"sha256\"")) || q > end || !(q = strchr(q + 8, '"')) || strlen(q) < 66 || q[65] != '"')
        return 0;
    memcpy(sha, q + 1, 64);
    sha[64] = 0;
    if (!(q = strstr(p, "\"size\"")) || q > end || !(q = strchr(q, ':')))
        return 0;
    *size = strtoul(q + 1, NULL, 10);
    return 1;
}

/* The archive's game files installed into dest; HOST_HD_DONE or _FAILED. */
static int install(const char *zip, const char *dest)
{
    static const char *const files[4][2] = {
        {"data/atlas-hd.pack", "atlas-hd.pack"}, {"LICENSE", "LICENSE"},
        {"THIRD-PARTY.txt", "THIRD-PARTY.txt"}, {"data/ATLAS-PROVENANCE.md", "ATLAS-PROVENANCE.md"}};
    static ZipEntry entries[64];
    static char manifest[16384];
    char hex[65], want[65], staged[4][4200], path[4200], version[64] = "";
    const ZipEntry *pkg;
    unsigned long size;
    long done = 0, total = 0;
    FILE *f = fopen(zip, "rb"), *o;
    int n, i, ok = 1;
    const char *v;
    if (!f)
        return fail("Could not read %s.", zip);
    n = zip_entries(f, entries, 64);
    if (n < 0 || !(pkg = zip_find(entries, n, "package.json")) ||
        !zip_extract(f, pkg, NULL, manifest, sizeof manifest, hex, &done, 1) ||
        !strstr(manifest, "\"openciv2-assets\"") || !strstr(manifest, "\"schema\": 1")) {
        fclose(f);
        return fail("%s is not an openciv2-assets pack.", zip);
    }
    if ((v = strstr(manifest, "\"version\"")) && (v = strchr(v + 9, '"')))
        sscanf(v + 1, "%63[^\"]", version);
    for (i = 0; i < 4; i++)
        if (zip_find(entries, n, files[i][0]))
            total += (long)zip_find(entries, n, files[i][0])->size;
    make_dirs(dest);
    done = 0;
    for (i = 0; i < 4 && ok; i++) {
        const ZipEntry *e = zip_find(entries, n, files[i][0]);
        snprintf(staged[i], sizeof staged[i], "%s" SEP "%s.part", dest, files[i][1]);
        if (!e || !manifest_file(manifest, files[i][0], want, &size) ||
            !zip_extract(f, e, staged[i], NULL, 0, hex, &done, total) || strcmp(hex, want) ||
            file_size(staged[i]) != (long)size) {
            if (!cancel_wanted)
                fail("%s in the pack does not match its package.json.", files[i][0]);
            ok = 0;
        }
    }
    fclose(f);
    for (i = 0; i < 4; i++) {
        snprintf(path, sizeof path, "%s" SEP "%s", dest, files[i][1]);
        if (ok && !replace_file(staged[i], path))
            ok = 0, fail("Could not write %s.", path);
        remove(staged[i]);
    }
    if (!ok)
        return HOST_HD_FAILED;
    snprintf(path, sizeof path, "%s" SEP "installed.json", dest);
    if ((o = fopen(path, "w"))) {
        fprintf(o, "{\n  \"tag\": \"%s\",\n  \"sha256\": \"%s\",\n  \"version\": \"%s\"\n}", HD_PACK_TAG,
                HD_PACK_SHA256, version);
        fclose(o);
    }
    return HOST_HD_DONE;
}

static int fetch(void *unused)
{
    char cache[4096], zip[4200], part[4300], dest[4200], hex[65];
    int r;
    (void)unused;
    if (!*HD_PACK_URL)
        r = fail("This build does not know which art to download.%s", NULL);
    else if (!host_user_dir(dest, (int)sizeof dest - 4))
        r = fail("There is no folder to install the art in.%s", NULL);
    else {
        strcat(dest, SEP "hd");
        cache_dir(cache, sizeof cache);
        snprintf(zip, sizeof zip, "%s" SEP "%s.zip", cache, HD_PACK_SHA256);
        snprintf(part, sizeof part, "%s.part", zip);
        r = HOST_HD_FAILED;
        /* the archive: in the cache already, else downloaded and checked */
        if (file_size(zip) == HD_PACK_SIZE && sha_file(zip, hex) && !strcmp(hex, HD_PACK_SHA256))
            r = HOST_HD_DONE;
        else if (!cancel_wanted && download(HD_PACK_URL, part, HD_PACK_SIZE)) {
            if (!sha_file(part, hex))
                ;
            else if (strcmp(hex, HD_PACK_SHA256))
                fail("The download is not the pinned art (its SHA-256 differs): nothing was installed.%s", NULL);
            else if (!replace_file(part, zip))
                fail("Could not write %s.", zip);
            else
                r = HOST_HD_DONE;
            remove(part);
        } else
            remove(part);
        if (r == HOST_HD_DONE && !cancel_wanted)
            r = install(zip, dest);
        if (cancel_wanted)
            r = HOST_HD_IDLE;
    }
    SDL_LockMutex(lock);
    job.state = r;
    SDL_UnlockMutex(lock);
    return 0;
}

int host_hd_fetch(int op, int32_t *info, char *msg, int size)
{
    int state;
    if (!lock)
        lock = SDL_CreateMutex();
    SDL_LockMutex(lock);
    state = job.state;
    SDL_UnlockMutex(lock);
    if (op == HOST_HD_START && state != HOST_HD_RUNNING) {
        if (thread)
            SDL_WaitThread(thread, NULL);       /* (the last one, finished) */
        cancel_wanted = 0;
        job.state = state = HOST_HD_RUNNING;
        job.phase = HOST_HD_DOWNLOADING;
        job.done = 0;
        job.total = HD_PACK_SIZE;
        job.msg[0] = 0;
        thread = SDL_CreateThread(fetch, "hd-fetch", NULL);
        if (!thread) {
            job.state = state = HOST_HD_FAILED;
            snprintf(job.msg, sizeof job.msg, "Could not start the download.");
        }
    } else if (op == HOST_HD_CANCEL && state == HOST_HD_RUNNING) {
        cancel_wanted = 1;
        SDL_WaitThread(thread, NULL);           /* (curl stopped, the part removed) */
        thread = NULL;
        SDL_LockMutex(lock);
        job.state = state = HOST_HD_IDLE;
        SDL_UnlockMutex(lock);
    }
    SDL_LockMutex(lock);
    if (info) {
        info[0] = job.phase;
        info[1] = (int32_t)(job.done / 1024);
        info[2] = (int32_t)((job.total ? job.total : HD_PACK_SIZE) / 1024);
    }
    if (msg && size > 0)
        snprintf(msg, (size_t)size, "%s", job.msg);
    SDL_UnlockMutex(lock);
    return state;
}
