/*
 * The MSVC C runtime functions the game calls, for native builds, over the
 * platform's C library.  Behaviour the game can observe is MSVC's:
 *   - rand() is MSVC's generator, with its seed at the original address
 *     (the harness forces and traces it there);
 *   - the debug heap fills new memory with 0xCD (the original ran with the
 *     debug runtime, and reads of uninitialized memory must match);
 *   - files opened without "b" are in text mode (CR LF <-> LF, ^Z ends);
 *   - `long` is 32-bit, so printf's "l" size prefix means int;
 *   - paths are Windows paths (src/platform/files.c maps them).
 * Prototypes: CRT_NATIVE_SIGS in tools/gen_c.py.
 */
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include "auto_common.h"
#include "../platform/plat_files.h"

#undef long          /* the platform's own types below */

/* ---- rand: seed at 0x00639e50, as in the original ---- */


void crt_srand(u32 seed)
{
    rand_seed = plat_srand(seed);
}

int crt_rand_at(const char *caller)
{
    rand_seed = rand_seed * 214013u + 2531011u;
    plat_trace_rand((rand_seed >> 16) & 0x7fff, caller);
    return (rand_seed >> 16) & 0x7fff;
}

int (_rand)(void)
{
    return crt_rand_at("_rand");
}

/* ---- heap: a size header, debug-runtime fill patterns ---- */

typedef struct { size_t size; size_t pad; } HeapHeader;

static void *heap_alloc(size_t n)
{
    HeapHeader *h;
    if (n > 64u << 20)
        plat_suspicious("malloc(%lu)", (unsigned long)n);
    h = (HeapHeader *)malloc(sizeof(HeapHeader) + n);
    if (!h)
        return NULL;
    h->size = n;
    memset(h + 1, 0xcd, n);
    return h + 1;
}

static void heap_free(void *p)
{
    HeapHeader *h;
    if (!p)
        return;
    h = (HeapHeader *)p - 1;
    memset(p, 0xdd, h->size);
    free(h);
}

void *operator_new(size_t n)
{
    return heap_alloc(n ? n : 1);
}

void operator_delete(void *p)
{
    heap_free(p);
}

void *crt_realloc(void *p, size_t n)       /* realloc */
{
    void *q;
    size_t old;
    if (!p)
        return heap_alloc(n);
    if (!n) {
        heap_free(p);
        return NULL;
    }
    old = ((HeapHeader *)p - 1)->size;
    q = heap_alloc(n);
    if (q) {
        memcpy(q, p, old < n ? old : n);
        heap_free(p);
    }
    return q;
}

size_t __msize(void *p)
{
    return p ? ((HeapHeader *)p - 1)->size : 0;
}

/* ---- memory and strings ---- */

/* The game shifts strings and buffers within themselves (e.g. strcpy(s, s + 1)).
 * MSVC's memcpy is memmove, and its strcpy/strcat copy forwards, which is
 * well-defined for those; the C library's versions are not. */
static char *copy_forward(char *d, const char *s)
{
    while ((*d++ = *s++) != 0)
        ;
    return d;
}
void *crt_memcpy(void *dst, const void *src, uint n) { return memmove(dst, src, n); }
void *crt_memmove(void *dst, const void *src, uint n) { return memmove(dst, src, n); }
char *crt_strcpy(char *dst, const char *src) { copy_forward(dst, src); return dst; }
char *crt_strcat(char *dst, const char *src) { copy_forward(dst + strlen(dst), src); return dst; }
int _memcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n); }
void *_memset(void *p, int c, size_t n) { return memset(p, c, n); }
size_t _strlen(const char *s) { return strlen(s); }
int _strcmp(const char *a, const char *b) { return strcmp(a, b); }
int _strncmp(const char *a, const char *b, size_t n) { return strncmp(a, b, n); }
char *_strncpy(char *d, const char *s, size_t n)
{
    size_t i = 0;
    for (; i < n && s[i]; i++)
        d[i] = s[i];
    for (; i < n; i++)
        d[i] = 0;
    return d;
}
char *_strncat(char *d, const char *s, size_t n) { return strncat(d, s, n); }
char *_strchr(const char *s, int c) { return (char *)strchr(s, c); }
char *_strrchr(const char *s, int c) { return (char *)strrchr(s, c); }
char *_strstr(const char *s, const char *sub) { return (char *)strstr(s, sub); }
int crt_abs(int v) { return v < 0 ? -v : v; }

/* MSVC's ctype functions take the C locale's view of chars 0..255. */
int _isalpha(int c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
int _isdigit(int c) { return c >= '0' && c <= '9'; }
uint crt_toupper(uint c) { return (c >= 'a' && c <= 'z') ? c - 0x20 : c; }   /* toupper */
static int lower(int c) { return (c >= 'A' && c <= 'Z') ? c + 0x20 : c; }

int __strcmpi(const char *a, const char *b)
{
    int ca, cb;
    do {
        ca = lower((unsigned char)*a++);
        cb = lower((unsigned char)*b++);
    } while (ca && ca == cb);
    return ca - cb;
}

int __strnicmp(const char *a, const char *b, size_t n)
{
    int ca = 0, cb = 0;
    while (n--) {
        ca = lower((unsigned char)*a++);
        cb = lower((unsigned char)*b++);
        if (!ca || ca != cb)
            break;
    }
    return ca - cb;
}

char *__strlwr(char *s)
{
    char *p;
    for (p = s; *p; p++)
        *p = (char)lower((unsigned char)*p);
    return s;
}

char *__strupr(char *s)
{
    char *p;
    for (p = s; *p; p++)
        *p = (char)crt_toupper((unsigned char)*p);
    return s;
}

static char *crt_xtoa(u32 v, int neg, char *buf, int radix)
{
    char tmp[40];
    int n = 0;
    char *p = buf;
    if (neg)
        *p++ = '-';
    do {
        int d = (int)(v % (u32)radix);
        tmp[n++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
        v /= (u32)radix;
    } while (v);
    while (n)
        *p++ = tmp[--n];
    *p = 0;
    return buf;
}

char *__itoa(int v, char *buf, int radix)
{
    return radix == 10 && v < 0 ? crt_xtoa((u32)-v, 1, buf, radix) : crt_xtoa((u32)v, 0, buf, radix);
}

char *__ltoa(i32 v, char *buf, int radix) { return __itoa(v, buf, radix); }
char *__ultoa(u32 v, char *buf, int radix) { return crt_xtoa(v, 0, buf, radix); }
int _atoi(const char *s) { return (int)strtol(s, NULL, 10); }
i32 _atol(const char *s) { return (i32)strtol(s, NULL, 10); }

/* ---- printf: MSVC's "l" (32-bit long) is the platform's plain int ---- */

static void msvc_format(const char *in, char *out, size_t n)
{
    size_t o = 0;
    while (*in && o + 2 < n) {
        if (*in != '%') {
            out[o++] = *in++;
            continue;
        }
        out[o++] = *in++;
        while (*in && strchr("-+ #0123456789.*", *in) && o + 2 < n)
            out[o++] = *in++;
        if (*in == 'l' && in[1] && strchr("diouxX", in[1]))
            in++;                                  /* %ld -> %d */
        else if (*in == 'h' || *in == 'l' || *in == 'L')
            out[o++] = *in++;
        if (*in && o + 2 < n)
            out[o++] = *in++;
    }
    out[o] = 0;
}

int _sprintf(char *buf, const char *fmt, ...)
{
    char f[1024];
    va_list ap;
    int r;
    msvc_format(fmt, f, sizeof f);
    va_start(ap, fmt);
    r = vsprintf(buf, f, ap);
    va_end(ap);
    return r;
}

int _printf(const char *fmt, ...)
{
    char f[1024];
    va_list ap;
    int r;
    msvc_format(fmt, f, sizeof f);
    va_start(ap, fmt);
    r = vprintf(f, ap);
    va_end(ap);
    return r;
}

/* ---- files ---- */

#define MAX_TEXT_FILES 64
static FILE *text_files[MAX_TEXT_FILES];

static int is_text(FILE *f)
{
    int i;
    for (i = 0; i < MAX_TEXT_FILES; i++)
        if (text_files[i] == f)
            return 1;
    return 0;
}

/* A host path (not the game's Windows one) opened as _fopen opens a file:
   in binary, a text mode's line ends and ^Z handled by the readers. */
FILE *crt_fopen_host(const char *host, const char *mode)
{
    char m[8];
    int text = !strchr(mode, 'b'), i, o = 0;
    FILE *f;
    for (i = 0; mode[i] && o < 6; i++)
        if (mode[i] != 't' && mode[i] != 'b')
            m[o++] = mode[i];
    m[o++] = 'b';
    m[o] = 0;
    f = fopen(host, m);
    if (f && text)
        for (i = 0; i < MAX_TEXT_FILES; i++)
            if (!text_files[i]) {
                text_files[i] = f;
                break;
            }
    return f;
}

FILE *__fsopen(const char *path, const char *mode, int share)
{
    char host[1024];
    (void)share;
    plat_host_path(path, host, sizeof host, mode[0] != 'r');
    return crt_fopen_host(host, mode);
}

FILE *_fopen(const char *path, const char *mode)
{
    return __fsopen(path, mode, 0);
}

int _fclose(FILE *f)
{
    int i;
    for (i = 0; i < MAX_TEXT_FILES; i++)
        if (text_files[i] == f)
            text_files[i] = NULL;
    return fclose(f);
}

int _fgetc(FILE *f)
{
    int c = getc(f);
    if (!is_text(f))
        return c;
    if (c == 0x1a) {                     /* ^Z: end of a text file */
        ungetc(c, f);
        return EOF;
    }
    if (c == '\r') {
        int d = getc(f);
        if (d == '\n')
            return '\n';
        if (d != EOF)
            ungetc(d, f);
    }
    return c;
}

char *_fgets(char *buf, int n, FILE *f)
{
    int i = 0, c;
    if (!is_text(f))
        return fgets(buf, n, f);
    while (i < n - 1 && (c = _fgetc(f)) != EOF) {
        buf[i++] = (char)c;
        if (c == '\n')
            break;
    }
    if (!i)
        return NULL;
    buf[i] = 0;
    return buf;
}

size_t _fread(void *buf, size_t size, size_t n, FILE *f)
{
    size_t total = size * n, i;
    int c;
    if (!is_text(f) || !size)
        return fread(buf, size, n, f);
    for (i = 0; i < total && (c = _fgetc(f)) != EOF; i++)
        ((char *)buf)[i] = (char)c;
    return i / size;
}

int _fputc(int c, FILE *f)
{
    if (c == '\n' && is_text(f) && putc('\r', f) == EOF)
        return EOF;
    return putc(c, f);
}

int _fputs(const char *s, FILE *f)
{
    for (; *s; s++)
        if (_fputc((unsigned char)*s, f) == EOF)
            return EOF;
    return 0;
}

size_t _fwrite(const void *buf, size_t size, size_t n, FILE *f)
{
    size_t total = size * n, i;
    if (!is_text(f) || !size)
        return fwrite(buf, size, n, f);
    for (i = 0; i < total; i++)
        if (_fputc(((const unsigned char *)buf)[i], f) == EOF)
            break;
    return i / size;
}

int _fseek(FILE *f, i32 off, int whence) { return fseek(f, off, whence); }
i32 _ftell(FILE *f) { return (i32)ftell(f); }
void _rewind(FILE *f) { rewind(f); }
int FUN_006076a0(FILE *f) { return fileno(f); }         /* _fileno */

i32 __filelength(int fd)
{
    struct stat st;
    return fstat(fd, &st) ? -1 : (i32)st.st_size;
}

int crt_rename(const char *from, const char *to)      /* rename */
{
    char a[1024], b[1024];
    plat_host_path(from, a, sizeof a, 0);
    plat_host_path(to, b, sizeof b, 1);
    return rename(a, b) ? -1 : 0;
}

int crt_remove(const char *path)                      /* remove */
{
    char a[1024];
    plat_host_path(path, a, sizeof a, 0);
    return remove(a) ? -1 : 0;
}

int crt_mkdir(const char *path)                      /* _mkdir */
{
    char a[1024];
    plat_host_path(path, a, sizeof a, 1);
    return mkdir(a, 0777) ? -1 : 0;
}

int __chdir(const char *path)
{
    return plat_chdir(path);
}

char *__getcwd(char *buf, int n)
{
    if (!buf)
        buf = (char *)heap_alloc(n > 0 ? (size_t)n : 260);
    return plat_getcwd(buf, n) ? buf : NULL;
}

/* ---- misc ---- */

i32 _time(i32 *t)
{
    i32 v = (i32)plat_time();
    if (t)
        *t = v;
    return v;
}

/* The game's static destructors run on exit(), but not on ExitProcess(),
 * which crt_exit_process() emulates (a replay script ending). */
static int skip_atexit;

static void run_atexit(void (*fn)(void))
{
    if (!skip_atexit)
        fn();
}

#define ATEXIT_MAX 64
static void (*atexit_fns[ATEXIT_MAX])(void);
static int atexit_n;

static void atexit_all(void)
{
    while (atexit_n > 0)
        run_atexit(atexit_fns[--atexit_n]);
}

int _atexit(void (*fn)(void))
{
    if (atexit_n == 0)
        atexit(atexit_all);
    if (atexit_n >= ATEXIT_MAX)
        return -1;
    atexit_fns[atexit_n++] = fn;
    return 0;
}

EXTERN_C void crt_exit_process(int code)
{
    skip_atexit = 1;
    exit(code);
}

void crt_vector_ctor_iterator(void *array, uint size, int count, VecXtorFn ctor, VecXtorFn dtor)
{
    char *p = (char *)array;
    int i;
    (void)dtor;
    for (i = 0; i < count; i++, p += size)
        ctor(p);
}

void crt_vector_dtor_iterator(void *array, uint size, int count, VecXtorFn dtor)
{
    char *p = (char *)array + size * count;
    while (--count >= 0) {
        p -= size;
        dtor(p);
    }
}

int __CxxUnhandledExceptionFilter(EXCEPTION_POINTERS *p)
{
    (void)p;
    return 0;
}

/* ---- not part of native builds ---- */

static void unsupported(const char *what)
{
    plat_fatal("%s is not supported in this build", what);
}

/* 16-bit far-call routines in the ASMGRAF section, unreachable from the
 * 32-bit program. */
void FUN_0061a000(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14) { unsupported("ASMGRAF 0x0061a000"); }
void FUN_0061a759(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16) { unsupported("ASMGRAF 0x0061a759"); }


#ifdef __wasm__
/* The MSVC runtime's assertion failure (glibc happens to have one). */
void __assert(const char *expr, const char *file, int line)
{
    plat_fatal("assertion failed: %s (%s:%d)", expr, file, line);
}
#endif
