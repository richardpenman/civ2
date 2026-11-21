/*
 * The host's part of the OpenCiv2 Options dialog (src/game/options.cpp,
 * host.h): the program started again (its choices that need a restart).
 */
#include "host.h"
#include <SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

/* ---- the restart ---- */

/* The program's arguments as it was started (port_main.c: before the
   set-up takes its own out); without them (the native test build) Linux's
   /proc/self/cmdline. */
static char **saved_argv;
static int saved_argc;

void host_save_args(int argc, char **argv)
{
    int i;
    saved_argv = (char **)calloc((size_t)argc + 1, sizeof *saved_argv);
    if (!saved_argv)
        return;
    for (i = 0; i < argc; i++)
        saved_argv[i] = strdup(argv[i]);
    saved_argc = argc;
}

/* The arguments that choose what the options dialog chooses (--hd,
   --sharp: they would win over it again) or the set-up (--setup): left
   out of the restart's. */
static int restart_drops(const char *a)
{
    static const char *const flags[] = {"--hd", "--sharp", "--setup"};
    unsigned i;
    for (i = 0; i < sizeof flags / sizeof flags[0]; i++) {
        size_t n = strlen(flags[i]);
        if (!SDL_strncasecmp(a, flags[i], n) && (!a[n] || (a[n] == '=' && i < 2)))
            return 1;
    }
    return 0;
}

/* args: the arguments to start again with (argv[0] first), NULL-terminated,
   in a block of size bytes; their count. */
static int restart_args(char **args, int max, char *block, size_t size)
{
    int n = 0, i;
    if (saved_argv) {
        for (i = 0; i < saved_argc && n < max - 1; i++)
            if (!i || !restart_drops(saved_argv[i]))
                args[n++] = saved_argv[i];
    } else {
#if defined(__linux__)
        FILE *f = fopen("/proc/self/cmdline", "rb");
        size_t got = 0, k = 0;
        if (f) {
            got = fread(block, 1, size - 1, f);
            fclose(f);
        }
        block[got] = 0;
        while (k < got && n < max - 1) {
            char *a = block + k;
            if (!n || !restart_drops(a))
                args[n++] = a;
            k += strlen(a) + 1;
        }
#else
        (void)block;
        (void)size;
#endif
    }
    args[n] = NULL;
    return n;
}

#ifdef _WIN32
/* An argument as Windows' C runtime parses it back */
static void quote_arg(char *out, size_t size, const char *a)
{
    size_t k = 0;
    int slashes = 0;
    if (*a && !strpbrk(a, " \t\"")) {
        snprintf(out, size, "%s", a);
        return;
    }
    if (k + 1 < size)
        out[k++] = '"';
    for (; *a && k + 4 < size; a++) {
        if (*a == '\\') {
            slashes++;
        } else {
            if (*a == '"')
                while (slashes-- >= 0 && k + 4 < size)
                    out[k++] = '\\';
            slashes = 0;
        }
        out[k++] = *a;
    }
    while (slashes-- > 0 && k + 3 < size)
        out[k++] = '\\';
    out[k++] = '"';
    out[k] = 0;
}
#endif

int host_restart(int now)
{
    char *args[64], block[8192];
    int n;
    if (!now)
        return 1;
    n = restart_args(args, 64, block, sizeof block);
    if (!n)
        return 0;
    /* what the set-up took from the command line (--hd, --sharp) */
#ifdef _WIN32
    _putenv("CIV2_HD_ARG=");
    _putenv("CIV2_SHARP_ARG=");
    {
        char exe[MAX_PATH], line[8192], q[1100];
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        int i;
        if (!GetModuleFileNameA(NULL, exe, sizeof exe))
            return 0;
        quote_arg(line, sizeof line, exe);
        for (i = 1; i < n; i++) {
            quote_arg(q, sizeof q, args[i]);
            if (strlen(line) + strlen(q) + 2 < sizeof line) {
                strcat(line, " ");
                strcat(line, q);
            }
        }
        memset(&si, 0, sizeof si);
        si.cb = sizeof si;
        if (!CreateProcessA(exe, line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            return 0;
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        host_quit();
        ExitProcess(0);
    }
#else
    unsetenv("CIV2_HD_ARG");
    unsetenv("CIV2_SHARP_ARG");
    {
        char exe[4096];
#if defined(__APPLE__)
        uint32_t len = sizeof exe;
        if (_NSGetExecutablePath(exe, &len))
            return 0;
#elif defined(__linux__)
        snprintf(exe, sizeof exe, "/proc/self/exe");
#else
        snprintf(exe, sizeof exe, "%s", args[0]);
#endif
        if (access(exe, X_OK))
            return 0;
        host_quit();
        execv(exe, args);
        fprintf(stderr, "OpenCiv2: could not start again (%s)\n", strerror(errno));
        exit(1);
    }
#endif
    return 0;
}

/* ---- the original's files ---- */

/* (the native test build, which has no set-up: src/host/setup.c is the
   portable build's, whose port_main.c gives the module setup_add_original) */
int host_add_original(void)
{
    return -1;
}
