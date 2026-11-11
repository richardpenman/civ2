/*
 * Files, paths, memory handles, profile (INI) files, the registry, modules
 * and other kernel services.
 *
 * Windows paths map to host paths as Wine maps them: drive Z: is the host
 * root, so the game, started in its directory, sees paths like
 * Z:\home\me\civ2\RULES.TXT; C:\WINDOWS is the profile directory; other
 * drives are the game directory.  Name lookup ignores case.
 */
#include "plat.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

char plat_game_dir[1024];       /* host directory of the game files */
char plat_win_dir[1024];        /* host directory standing in for C:\WINDOWS */
char plat_data_dir[1024];       /* host directory of the native build's own data */
static DWORD last_error;

DWORD WINAPI GetLastError(void) { API_TRACE(); return last_error; }
UINT WINAPI SetErrorMode(UINT m) { API_TRACE(); (void)m; return 0; }

/* ---- paths ---- */

/* Find `name` in directory `dir` ignoring case; append the real name. */
static int resolve_component(char *path, size_t n, const char *name)
{
    DIR *d;
    struct dirent *e;
    size_t len = strlen(path);
    struct stat st;
    snprintf(path + len, n - len, "%s%s", len && path[len - 1] != '/' ? "/" : "", name);
    if (stat(path, &st) == 0)
        return 1;
    path[len] = 0;
    d = opendir(len ? path : "/");
    if (d) {
        while ((e = readdir(d))) {
            if (strcasecmp(e->d_name, name) == 0) {
                snprintf(path + len, n - len, "%s%s", len && path[len - 1] != '/' ? "/" : "", e->d_name);
                closedir(d);
                return 1;
            }
        }
        closedir(d);
    }
    snprintf(path + len, n - len, "%s%s", len && path[len - 1] != '/' ? "/" : "", name);
    return 0;
}

void plat_host_path(const char *win, char *host, size_t n, int for_create)
{
    char tmp[1024], *comp, *save = NULL;
    const char *rest = win;
    (void)for_create;
    host[0] = 0;
    if (((win[0] | 0x20) >= 'a' && (win[0] | 0x20) <= 'z') && win[1] == ':') {
        int drive = win[0] | 0x20;
        rest = win + 2;
        if (drive == 'z') {
            snprintf(host, n, "/");
        } else if (drive == 'c' && (strncasecmp(rest, "\\windows", 8) == 0 || strncasecmp(rest, "/windows", 8) == 0) &&
                   (rest[8] == 0 || rest[8] == '\\' || rest[8] == '/')) {
            snprintf(host, n, "%s", plat_win_dir);
            rest += 8;
        } else {
            snprintf(host, n, "%s", plat_game_dir);
        }
        if (*rest != '\\' && *rest != '/') {
            /* "C:name": relative to that drive's current directory (the game's). */
            char cwd[1024];
            if (drive == 'z' && getcwd(cwd, sizeof cwd))
                snprintf(host, n, "%s", cwd);
        }
    } else if (win[0] == '\\' || win[0] == '/') {
        /* Rooted on the current drive (Z:). */
        snprintf(host, n, "/");
    } else {
        if (!getcwd(host, n))
            snprintf(host, n, ".");
    }
    snprintf(tmp, sizeof tmp, "%s", rest);
    for (comp = strtok_r(tmp, "\\/", &save); comp; comp = strtok_r(NULL, "\\/", &save)) {
        if (!strcmp(comp, "."))
            continue;
        if (!strcmp(comp, "..")) {
            char *slash = strrchr(host, '/');
            if (slash && slash != host)
                *slash = 0;
            else
                snprintf(host, n, "/");
            continue;
        }
        resolve_component(host, n, comp);
    }
}

/* The game keeps its paths in 128-byte buffers (sounds, videos ...), enough
   for C:\MPS\CIV2 but not for a game folder deep in the host's file system.
   A long path inside the game folder is given as C:\..., which
   plat_host_path takes back to the game folder; short ones stay Z:\... as
   under Wine, as the recordings have them. */
#define LONG_WIN_PATH 48

static void win_path_of_host(const char *host, char *win, size_t n)
{
    size_t i, g = strlen(plat_game_dir);
    if (strlen(host) + 2 > LONG_WIN_PATH && g > 1 && strncasecmp(host, plat_game_dir, g) == 0 &&
        (host[g] == 0 || host[g] == '/') && strncasecmp(host + g, "/windows", 8) != 0)
        snprintf(win, n, "C:%s", host[g] ? host + g : "/");
    else
        snprintf(win, n, "Z:%s", host);
    for (i = 0; win[i]; i++)
        if (win[i] == '/')
            win[i] = '\\';
    if (!strcmp(win, "Z:\\") || !win[2])
        snprintf(win, n, "Z:\\");
}

int plat_chdir(const char *win)
{
    char host[1024];
    plat_host_path(win, host, sizeof host, 0);
    return chdir(host) ? -1 : 0;
}

int plat_getcwd(char *win, int n)
{
    char host[1024];
    if (!getcwd(host, sizeof host))
        return 0;
    win_path_of_host(host, win, (size_t)n);
    return 1;
}

DWORD WINAPI GetModuleFileNameA(HMODULE m, LPSTR buf, DWORD n)
{ API_TRACE();
    char host[1100];
    (void)m;
    snprintf(host, sizeof host, "%s/civ2.exe", plat_game_dir);
    win_path_of_host(host, buf, n);
    return (DWORD)strlen(buf);
}

DWORD WINAPI GetFileAttributesA(LPCSTR win)
{ API_TRACE();
    char host[1024];
    struct stat st;
    plat_host_path(win, host, sizeof host, 0);
    if (stat(host, &st)) {
        last_error = ERROR_FILE_NOT_FOUND;
        return INVALID_FILE_ATTRIBUTES;
    }
    return S_ISDIR(st.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
}

/* Drives as Wine's default configuration has them: C: and Z:. */
DWORD WINAPI GetLogicalDrives(void) { API_TRACE(); return (1u << 2) | (1u << 25); }

DWORD WINAPI GetLogicalDriveStringsA(DWORD n, LPSTR buf)
{ API_TRACE();
    static const char s[] = "C:\\\0Z:\\\0";
    if (n < sizeof s)
        return sizeof s;
    memcpy(buf, s, sizeof s);
    return sizeof s - 1;
}

UINT WINAPI GetDriveTypeA(LPCSTR root)
{ API_TRACE();
    int d = root ? (root[0] | 0x20) : 'z';
    return d == 'c' || d == 'z' ? DRIVE_FIXED : DRIVE_NO_ROOT_DIR;
}

/* ---- file handles ---- */

typedef struct { int fd; } File;

static HANDLE new_file(int fd)
{
    File *f = malloc(sizeof *f);
    f->fd = fd;
    return obj_new(OBJ_FILE, f);
}

static File *get_file(HANDLE h) { return obj_get(h, OBJ_FILE); }

HANDLE WINAPI CreateFileA(LPCSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa, DWORD disp, DWORD flags,
                          HANDLE tmpl)
{ API_TRACE();
    char host[1024];
    int oflags = 0, fd;
    (void)share; (void)sa; (void)flags; (void)tmpl;
    plat_host_path(name, host, sizeof host, disp != OPEN_EXISTING);
    if ((access & GENERIC_READ) && (access & GENERIC_WRITE))
        oflags = O_RDWR;
    else if (access & GENERIC_WRITE)
        oflags = O_WRONLY;
    else
        oflags = O_RDONLY;
    switch (disp) {
    case CREATE_NEW: oflags |= O_CREAT | O_EXCL; break;
    case CREATE_ALWAYS: oflags |= O_CREAT | O_TRUNC; break;
    case OPEN_ALWAYS: oflags |= O_CREAT; break;
    case TRUNCATE_EXISTING: oflags |= O_TRUNC; break;
    }
    fd = open(host, oflags, 0666);
    if (fd < 0) {
        last_error = errno == ENOENT ? ERROR_FILE_NOT_FOUND : ERROR_ACCESS_DENIED;
        return INVALID_HANDLE_VALUE;
    }
    last_error = 0;
    return new_file(fd);
}

WINBOOL WINAPI ReadFile(HANDLE h, LPVOID buf, DWORD n, LPDWORD got, LPOVERLAPPED ov)
{ API_TRACE();
    File *f = get_file(h);
    ssize_t r;
    (void)ov;
    if (!f)
        return FALSE;
    r = read(f->fd, buf, n);
    if (got)
        *got = r < 0 ? 0 : (DWORD)r;
    return r >= 0;
}

WINBOOL WINAPI WriteFile(HANDLE h, LPCVOID buf, DWORD n, LPDWORD put, LPOVERLAPPED ov)
{ API_TRACE();
    File *f = get_file(h);
    ssize_t r;
    (void)ov;
    if (!f)
        return FALSE;
    r = write(f->fd, buf, n);
    if (put)
        *put = r < 0 ? 0 : (DWORD)r;
    return r >= 0;
}

DWORD WINAPI SetFilePointer(HANDLE h, LONG off, PLONG high, DWORD method)
{ API_TRACE();
    File *f = get_file(h);
    off_t r;
    long long o = off;
    if (!f)
        return INVALID_SET_FILE_POINTER;
    if (high)
        o = ((long long)*high << 32) | (unsigned)off;
    r = lseek(f->fd, (off_t)o, method == FILE_BEGIN ? SEEK_SET : method == FILE_CURRENT ? SEEK_CUR : SEEK_END);
    if (r < 0)
        return INVALID_SET_FILE_POINTER;
    if (high)
        *high = (LONG)((long long)r >> 32);
    return (DWORD)r;
}

DWORD WINAPI GetFileSize(HANDLE h, LPDWORD high)
{ API_TRACE();
    File *f = get_file(h);
    struct stat st;
    if (!f || fstat(f->fd, &st))
        return INVALID_FILE_SIZE;
    if (high)
        *high = (DWORD)((long long)st.st_size >> 32);
    return (DWORD)st.st_size;
}

typedef struct { unsigned char *data; size_t size; } Mapping;

WINBOOL WINAPI CloseHandle(HANDLE h)
{ API_TRACE();
    File *f = get_file(h);
    if (f) {
        close(f->fd);
        free(f);
        obj_free(h);
        return TRUE;
    }
    if (obj_type(h) == OBJ_MISC) {
        Mapping *m = obj_get(h, OBJ_MISC);
        if (m) {
            free(m->data);
            free(m);
        }
        obj_free(h);
        return TRUE;
    }
    return FALSE;
}

HFILE WINAPI OpenFile(LPCSTR name, LPOFSTRUCT of, UINT style)
{ API_TRACE();
    char host[1024];
    struct stat st;
    int fd, mode = O_RDONLY;
    plat_host_path(name, host, sizeof host, (style & OF_CREATE) != 0);
    if (of) {
        memset(of, 0, sizeof *of);
        of->cBytes = sizeof *of;
        snprintf(of->szPathName, sizeof of->szPathName, "%s", name);
    }
    if (style & OF_EXIST) {
        if (stat(host, &st)) {
            if (of)
                of->nErrCode = ERROR_FILE_NOT_FOUND;
            return HFILE_ERROR;
        }
        return 1;
    }
    if (style & OF_DELETE)
        return unlink(host) ? HFILE_ERROR : 1;
    if (style & OF_READWRITE)
        mode = O_RDWR;
    else if (style & OF_WRITE)
        mode = O_WRONLY;
    if (style & OF_CREATE)
        mode |= O_CREAT | O_TRUNC;
    fd = open(host, mode, 0666);
    if (fd < 0) {
        if (of)
            of->nErrCode = ERROR_FILE_NOT_FOUND;
        return HFILE_ERROR;
    }
    return (HFILE)(uintptr_t)new_file(fd);
}

long WINAPI _hread(HFILE h, LPVOID buf, long n)
{ API_TRACE();
    File *f = get_file((HANDLE)(uintptr_t)(unsigned)h);
    ssize_t r;
    if (!f)
        return HFILE_ERROR;
    r = read(f->fd, buf, (size_t)n);
    return r < 0 ? HFILE_ERROR : (long)r;
}

LONG WINAPI _llseek(HFILE h, LONG off, int origin)
{ API_TRACE();
    File *f = get_file((HANDLE)(uintptr_t)(unsigned)h);
    off_t r;
    if (!f)
        return HFILE_ERROR;
    r = lseek(f->fd, off, origin == 0 ? SEEK_SET : origin == 1 ? SEEK_CUR : SEEK_END);
    return r < 0 ? HFILE_ERROR : (LONG)r;
}

HANDLE WINAPI CreateFileMappingA(HANDLE h, LPSECURITY_ATTRIBUTES sa, DWORD prot, DWORD hi, DWORD lo, LPCSTR name)
{ API_TRACE();
    File *f = get_file(h);
    Mapping *m;
    struct stat st;
    (void)sa; (void)prot; (void)hi; (void)name;
    if (!f || fstat(f->fd, &st))
        return NULL;
    m = calloc(1, sizeof *m);
    m->size = lo ? lo : (size_t)st.st_size;
    m->data = calloc(m->size + 1, 1);
    if (pread(f->fd, m->data, m->size, 0) < 0)
        plat_log("CreateFileMapping: read failed\n");
    return obj_new(OBJ_MISC, m);
}

LPVOID WINAPI MapViewOfFile(HANDLE h, DWORD access, DWORD hi, DWORD lo, SIZE_T n)
{ API_TRACE();
    Mapping *m = obj_get(h, OBJ_MISC);
    (void)access; (void)hi; (void)n;
    return m && lo < m->size ? m->data + lo : NULL;
}

WINBOOL WINAPI UnmapViewOfFile(LPCVOID p) { API_TRACE(); (void)p; return TRUE; }

/* ---- directory search ---- */

typedef struct { DIR *d; char dir[1024]; char pattern[260]; } Find;

static int wild_match(const char *pat, const char *s)
{
    if (!*pat)
        return !*s;
    if (*pat == '*') {
        /* "*.*" matches names without a dot too, as on Windows. */
        if (!strcmp(pat, "*.*"))
            return 1;
        for (; ; s++) {
            if (wild_match(pat + 1, s))
                return 1;
            if (!*s)
                return 0;
        }
    }
    if (!*s)
        return 0;
    if (*pat == '?' || tolower((unsigned char)*pat) == tolower((unsigned char)*s))
        return wild_match(pat + 1, s + 1);
    return 0;
}

static int find_next(Find *f, LPWIN32_FIND_DATAA fd)
{
    struct dirent *e;
    while ((e = readdir(f->d))) {
        char full[1400];
        struct stat st;
        if (!wild_match(f->pattern, e->d_name))
            continue;
        snprintf(full, sizeof full, "%s/%s", f->dir, e->d_name);
        if (stat(full, &st))
            continue;
        memset(fd, 0, sizeof *fd);
        fd->dwFileAttributes = S_ISDIR(st.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_ARCHIVE;
        fd->nFileSizeLow = (DWORD)st.st_size;
        {   /* 100 ns units since 1601 */
            unsigned long long t = ((unsigned long long)st.st_mtime + 11644473600ull) * 10000000ull;
            fd->ftLastWriteTime.dwLowDateTime = (DWORD)t;
            fd->ftLastWriteTime.dwHighDateTime = (DWORD)(t >> 32);
        }
        snprintf(fd->cFileName, sizeof fd->cFileName, "%s", e->d_name);
        return 1;
    }
    return 0;
}

HANDLE WINAPI FindFirstFileA(LPCSTR spec, LPWIN32_FIND_DATAA fd)
{ API_TRACE();
    char win[1024], *slash;
    Find *f = calloc(1, sizeof *f);
    snprintf(win, sizeof win, "%s", spec);
    slash = strrchr(win, '\\');
    if (!slash)
        slash = strrchr(win, '/');
    if (slash) {
        snprintf(f->pattern, sizeof f->pattern, "%s", slash + 1);
        *slash = 0;
        plat_host_path(*win ? win : "\\", f->dir, sizeof f->dir, 0);
    } else {
        snprintf(f->pattern, sizeof f->pattern, "%s", win);
        if (!getcwd(f->dir, sizeof f->dir))
            snprintf(f->dir, sizeof f->dir, ".");
    }
    f->d = opendir(f->dir);
    if (!f->d || !find_next(f, fd)) {
        if (f->d)
            closedir(f->d);
        free(f);
        last_error = ERROR_FILE_NOT_FOUND;
        return INVALID_HANDLE_VALUE;
    }
    return obj_new(OBJ_FIND, f);
}

WINBOOL WINAPI FindNextFileA(HANDLE h, LPWIN32_FIND_DATAA fd)
{ API_TRACE();
    Find *f = obj_get(h, OBJ_FIND);
    if (!f || !find_next(f, fd)) {
        last_error = ERROR_NO_MORE_FILES;
        return FALSE;
    }
    return TRUE;
}

WINBOOL WINAPI FindClose(HANDLE h)
{ API_TRACE();
    Find *f = obj_get(h, OBJ_FIND);
    if (!f)
        return FALSE;
    closedir(f->d);
    free(f);
    obj_free(h);
    return TRUE;
}

/* ---- memory handles ---- */

typedef struct { void *p; size_t n; int locks; } Mem;

static HGLOBAL mem_alloc(UINT flags, SIZE_T n)
{
    Mem *m = calloc(1, sizeof *m);
    (void)flags;
    if (n > 64u << 20)
        plat_suspicious("GlobalAlloc(%lu)", (unsigned long)n);
    m->p = calloc(n ? n : 1, 1);
    m->n = n;
    return obj_new(OBJ_MEM, m);
}

HGLOBAL WINAPI GlobalAlloc(UINT flags, SIZE_T n) { API_TRACE(); return mem_alloc(flags, n); }

LPVOID WINAPI GlobalLock(HGLOBAL h)
{ API_TRACE();
    Mem *m = obj_get(h, OBJ_MEM);
    if (!m)
        return NULL;
    m->locks++;
    return m->p;
}

WINBOOL WINAPI GlobalUnlock(HGLOBAL h)
{ API_TRACE();
    Mem *m = obj_get(h, OBJ_MEM);
    if (!m || !m->locks)
        return FALSE;
    return --m->locks != 0;
}

HGLOBAL WINAPI GlobalFree(HGLOBAL h)
{ API_TRACE();
    Mem *m = obj_get(h, OBJ_MEM);
    if (!m)
        return h;
    free(m->p);
    free(m);
    obj_free(h);
    return NULL;
}

SIZE_T WINAPI GlobalSize(HGLOBAL h)
{ API_TRACE();
    Mem *m = obj_get(h, OBJ_MEM);
    return m ? m->n : 0;
}

/* The handle of a block from its address: a linear search (rarely used). */
HGLOBAL WINAPI GlobalHandle(LPCVOID p)
{ API_TRACE();
    uintptr_t i;
    for (i = 1; i < 8192; i++) {
        HGLOBAL h = (HGLOBAL)(((uintptr_t)OBJ_MEM << 16) | i);
        Mem *m = obj_get(h, OBJ_MEM);
        if (m && m->p == p)
            return h;
    }
    return NULL;
}

SIZE_T WINAPI GlobalCompact(DWORD min) { API_TRACE(); (void)min; return 64u << 20; }

HLOCAL WINAPI LocalAlloc(UINT flags, SIZE_T n) { API_TRACE(); return mem_alloc(flags, n); }
LPVOID WINAPI LocalLock(HLOCAL h) { API_TRACE(); return GlobalLock(h); }
WINBOOL WINAPI LocalUnlock(HLOCAL h) { API_TRACE(); return GlobalUnlock(h); }
HLOCAL WINAPI LocalFree(HLOCAL h) { API_TRACE(); return GlobalFree(h); }

WINBOOL WINAPI IsBadHugeReadPtr(const void *p, UINT_PTR n) { API_TRACE(); (void)n; return p == NULL; }

/* ---- profile (INI) files ---- */

static void profile_path(LPCSTR file, char *host, size_t n)
{
    if (strchr(file, '\\') || strchr(file, '/') || strchr(file, ':'))
        plat_host_path(file, host, n, 1);
    else {
        char win[300];
        snprintf(win, sizeof win, "C:\\WINDOWS\\%s", file);
        plat_host_path(win, host, n, 1);
    }
}

/* Find key in [section]; copy its value.  Returns 1 if found. */
static int profile_get(LPCSTR file, LPCSTR section, LPCSTR key, char *out, size_t n)
{
    char host[1024], line[1024];
    FILE *f;
    int in = 0;
    profile_path(file, host, sizeof host);
    f = fopen(host, "r");
    if (!f)
        return 0;
    while (fgets(line, sizeof line, f)) {
        char *e = line + strlen(line), *s = line, *eq;
        while (e > line && (e[-1] == '\n' || e[-1] == '\r' || e[-1] == ' ' || e[-1] == '\t'))
            *--e = 0;
        while (*s == ' ' || *s == '\t')
            s++;
        if (*s == '[') {
            char *close = strchr(s, ']');
            if (close)
                *close = 0;
            in = !strcasecmp(s + 1, section);
            continue;
        }
        if (!in || !(eq = strchr(s, '=')))
            continue;
        *eq = 0;
        {
            char *ke = eq;
            while (ke > s && (ke[-1] == ' ' || ke[-1] == '\t'))
                *--ke = 0;
        }
        if (!strcasecmp(s, key)) {
            char *v = eq + 1;
            while (*v == ' ' || *v == '\t')
                v++;
            snprintf(out, n, "%s", v);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

/* (docs/EXTRAS.md) A key's text in an INI file, as GetPrivateProfileString:
   1 if found. */
int plat_profile_string(const char *file, const char *section, const char *key, char *out, int n)
{
    if (n > 0)
        out[0] = 0;
    return profile_get(file, section, key, out, (size_t)n);
}

UINT WINAPI GetPrivateProfileIntA(LPCSTR section, LPCSTR key, INT def, LPCSTR file)
{ API_TRACE();
    char v[256];
    if (!profile_get(file, section, key, v, sizeof v))
        return (UINT)def;
    return (UINT)strtol(v, NULL, 10);
}

WINBOOL WINAPI WritePrivateProfileStringA(LPCSTR section, LPCSTR key, LPCSTR value, LPCSTR file)
{ API_TRACE();
    char host[1024], line[1024];
    char *out = NULL;
    size_t outn = 0, cap = 0;
    FILE *f;
    int in = 0, done = 0, have_section = 0;
#define EMIT(str) do { size_t l_ = strlen(str); if (outn + l_ + 1 > cap) { cap = (outn + l_ + 1) * 2; out = realloc(out, cap); } \
                       memcpy(out + outn, str, l_); outn += l_; out[outn] = 0; } while (0)
    profile_path(file, host, sizeof host);
    f = fopen(host, "r");
    EMIT("");
    if (f) {
        while (fgets(line, sizeof line, f)) {
            char tmp[1024], *s = tmp, *eq;
            snprintf(tmp, sizeof tmp, "%s", line);
            while (*s == ' ' || *s == '\t')
                s++;
            if (*s == '[') {
                char *close = strchr(s, ']');
                if (in && !done && key && value) {
                    char kv[1024];
                    snprintf(kv, sizeof kv, "%s=%s\r\n", key, value);
                    EMIT(kv);
                    done = 1;
                }
                if (close)
                    *close = 0;
                in = !strcasecmp(s + 1, section);
                if (in)
                    have_section = 1;
                if (in && !key)
                    continue;           /* deleting the section */
                EMIT(line);
                continue;
            }
            if (in && !key)
                continue;
            if (in && key && (eq = strchr(s, '='))) {
                char *ke = eq;
                *eq = 0;
                while (ke > s && (ke[-1] == ' ' || ke[-1] == '\t'))
                    *--ke = 0;
                if (!strcasecmp(s, key)) {
                    if (value && !done) {
                        char kv[1024];
                        snprintf(kv, sizeof kv, "%s=%s\r\n", key, value);
                        EMIT(kv);
                    }
                    done = 1;
                    continue;
                }
            }
            EMIT(line);
        }
        fclose(f);
    }
    if (!done && key && value) {
        char kv[1100];
        if (!have_section) {
            snprintf(kv, sizeof kv, "[%s]\r\n", section);
            EMIT(kv);
        } else if (!in) {
            /* The section exists but is not the last one: append it again. */
            snprintf(kv, sizeof kv, "[%s]\r\n", section);
            EMIT(kv);
        }
        snprintf(kv, sizeof kv, "%s=%s\r\n", key, value);
        EMIT(kv);
    }
#undef EMIT
    f = fopen(host, "wb");
    if (f) {
        fwrite(out, 1, outn, f);
        fclose(f);
    }
    free(out);
    return f != NULL;
}

/* ---- registry: kept in memory for the run ---- */

typedef struct RegValue { char key[256], name[128]; DWORD type; unsigned char *data; DWORD n; struct RegValue *next; } RegValue;
static RegValue *reg_values;
typedef struct { char path[256]; } RegKey;

static HKEY reg_open(HKEY parent, LPCSTR sub)
{
    RegKey *k = calloc(1, sizeof *k), *p = obj_get(parent, OBJ_KEY);
    const char *base = p ? p->path : parent == HKEY_LOCAL_MACHINE ? "HKLM" : parent == HKEY_CURRENT_USER ? "HKCU" : "HK";
    snprintf(k->path, sizeof k->path, "%s\\%s", base, sub ? sub : "");
    return obj_new(OBJ_KEY, k);
}

LSTATUS WINAPI RegOpenKeyExA(HKEY parent, LPCSTR sub, DWORD opt, REGSAM sam, PHKEY out)
{ API_TRACE();
    RegValue *v;
    char path[256];
    RegKey *p = obj_get(parent, OBJ_KEY);
    (void)opt; (void)sam;
    snprintf(path, sizeof path, "%s\\%s",
             p ? p->path : parent == HKEY_LOCAL_MACHINE ? "HKLM" : parent == HKEY_CURRENT_USER ? "HKCU" : "HK", sub);
    for (v = reg_values; v; v = v->next)
        if (!strncasecmp(v->key, path, strlen(path))) {
            *out = reg_open(parent, sub);
            return ERROR_SUCCESS;
        }
    return ERROR_FILE_NOT_FOUND;
}

LSTATUS WINAPI RegCreateKeyExA(HKEY parent, LPCSTR sub, DWORD res, LPSTR cls, DWORD opt, REGSAM sam,
                               LPSECURITY_ATTRIBUTES sa, PHKEY out, LPDWORD disp)
{ API_TRACE();
    (void)res; (void)cls; (void)opt; (void)sam; (void)sa;
    *out = reg_open(parent, sub);
    if (disp)
        *disp = REG_CREATED_NEW_KEY;
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegCloseKey(HKEY k)
{ API_TRACE();
    RegKey *r = obj_get(k, OBJ_KEY);
    if (r) {
        free(r);
        obj_free(k);
    }
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegQueryValueExA(HKEY k, LPCSTR name, LPDWORD res, LPDWORD type, LPBYTE data, LPDWORD n)
{ API_TRACE();
    RegKey *r = obj_get(k, OBJ_KEY);
    RegValue *v;
    (void)res;
    if (!r)
        return ERROR_INVALID_HANDLE;
    for (v = reg_values; v; v = v->next)
        if (!strcasecmp(v->key, r->path) && !strcasecmp(v->name, name ? name : "")) {
            if (type)
                *type = v->type;
            if (data && n && *n < v->n) {
                *n = v->n;
                return ERROR_MORE_DATA;
            }
            if (data)
                memcpy(data, v->data, v->n);
            if (n)
                *n = v->n;
            return ERROR_SUCCESS;
        }
    return ERROR_FILE_NOT_FOUND;
}

LSTATUS WINAPI RegSetValueExA(HKEY k, LPCSTR name, DWORD res, DWORD type, const BYTE *data, DWORD n)
{ API_TRACE();
    RegKey *r = obj_get(k, OBJ_KEY);
    RegValue *v;
    (void)res;
    if (!r)
        return ERROR_INVALID_HANDLE;
    for (v = reg_values; v; v = v->next)
        if (!strcasecmp(v->key, r->path) && !strcasecmp(v->name, name ? name : ""))
            break;
    if (!v) {
        v = calloc(1, sizeof *v);
        snprintf(v->key, sizeof v->key, "%s", r->path);
        snprintf(v->name, sizeof v->name, "%s", name ? name : "");
        v->next = reg_values;
        reg_values = v;
    }
    free(v->data);
    v->data = malloc(n ? n : 1);
    memcpy(v->data, data, n);
    v->n = n;
    v->type = type;
    return ERROR_SUCCESS;
}

/* ---- synchronisation (single-threaded) ---- */

HANDLE WINAPI CreateMutexA(LPSECURITY_ATTRIBUTES sa, WINBOOL owner, LPCSTR name)
{ API_TRACE();
    (void)sa; (void)owner; (void)name;
    last_error = 0;
    return obj_new(OBJ_MISC, NULL);
}

/* ---- strings ---- */

int WINAPI lstrlenA(LPCSTR s) { API_TRACE(); return s ? (int)strlen(s) : 0; }
LPSTR WINAPI lstrcpyA(LPSTR d, LPCSTR s) { API_TRACE(); return strcpy(d, s); }

LPSTR WINAPI CharUpperA(LPSTR s)
{ API_TRACE();
    if (IS_INTRESOURCE(s))
        return (LPSTR)(uintptr_t)toupper((int)(uintptr_t)s);
    for (char *p = s; *p; p++)
        *p = (char)toupper((unsigned char)*p);
    return s;
}

LPSTR WINAPI CharLowerA(LPSTR s)
{ API_TRACE();
    if (IS_INTRESOURCE(s))
        return (LPSTR)(uintptr_t)tolower((int)(uintptr_t)s);
    for (char *p = s; *p; p++)
        *p = (char)tolower((unsigned char)*p);
    return s;
}

/* wsprintf: the C library's, with MSVC's 32-bit "l" sizes. */
int wsprintfA(LPSTR buf, LPCSTR fmt, ...)
{
    char f[512];
    size_t i, o = 0;
    va_list ap;
    int r;
    for (i = 0; fmt[i] && o < sizeof f - 1; i++) {
        if (fmt[i] == 'l' && i && strchr("diuxX", fmt[i + 1]))
            continue;
        f[o++] = fmt[i];
    }
    f[o] = 0;
    va_start(ap, fmt);
    r = vsnprintf(buf, 1024, f, ap);
    va_end(ap);
    return r;
}

/* ---- processes ---- */

UINT WINAPI WinExec(LPCSTR cmd, UINT show)
{ API_TRACE();
    (void)show;
    plat_log("WinExec(%s) not supported\n", cmd);
    return 0;
}

void WINAPI OutputDebugStringA(LPCSTR s) { API_TRACE(); plat_log("debug: %s", s); }
void WINAPI DebugBreak(void) { API_TRACE(); plat_log("DebugBreak\n"); }
