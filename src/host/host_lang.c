/*
 * The host's texts, translated (host_lang.h).  The translation is read as
 * the game reads it (src/game/lang.cpp): "@@key" lines, the text on the
 * lines after; only OpenCiv2's own keys ("openciv2.<key>") and the code page
 * ("codepage") are kept here, converted to UTF-8.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <strings.h>
#endif
#include "host_lang.h"
#include "codepages.h"
#ifndef __wasi__
#include "host.h"
#endif

static const struct {
    const char *key, *english;
} english[] = {
#define T(key, text) {key, text},
#include "texts.h"
#undef T
};
#define NTEXTS (sizeof english / sizeof english[0])

static char *translated[NTEXTS];        /* UTF-8 */
static const unsigned short *table = codepage_1252;
static int inited;

static int text_index(const char *key)
{
    size_t i;
    for (i = 0; i < NTEXTS; i++)
        if (!strcmp(english[i].key, key))
            return (int)i;
    return -1;
}

const char *host_lang_utf8(const char *text, char *out, size_t size)
{
    size_t k = 0;
    const unsigned char *p = (const unsigned char *)text;
    if (!size)
        return out;
    for (; *p && k + 4 < size; p++) {
        unsigned u = codepage_to_unicode(table, *p);
        if (!u)
            u = '?';
        if (u < 0x80) {
            out[k++] = (char)u;
        } else if (u < 0x800) {
            out[k++] = (char)(0xc0 | u >> 6);
            out[k++] = (char)(0x80 | (u & 0x3f));
        } else {
            out[k++] = (char)(0xe0 | u >> 12);
            out[k++] = (char)(0x80 | ((u >> 6) & 0x3f));
            out[k++] = (char)(0x80 | (u & 0x3f));
        }
    }
    out[k] = 0;
    return out;
}

static void clear(void)
{
    size_t i;
    for (i = 0; i < NTEXTS; i++) {
        free(translated[i]);
        translated[i] = NULL;
    }
    table = codepage_1252;
}

/* An entry read: kept (as UTF-8) if it is one of ours. */
static void keep(const char *key, const char *text, char **raw)
{
    int i;
    if (!strcmp(key, "codepage")) {
        const unsigned short *t = codepage_table(text);
        if (t)
            table = t;
        return;
    }
    if (strncmp(key, "openciv2.", 9) || (i = text_index(key + 9)) < 0)
        return;
    free(raw[i]);
    raw[i] = strdup(text);
}

void host_lang_load(const char *path)
{
    static char *raw[NTEXTS];
    char line[2048], key[128] = "", *text = NULL;
    size_t len = 0, i;
    int lines = 0;
    FILE *f = path ? fopen(path, "r") : NULL;
    inited = 1;
    clear();
    if (!f)
        return;
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '@' && line[1] == '@') {
            if (key[0])
                keep(key, text ? text : "", raw);
            snprintf(key, sizeof key, "%s", line + 2);
            len = 0;
            lines = 0;
            if (text)
                text[0] = 0;
        } else if (key[0]) {
            const char *l = line[0] == '\\' ? line + 1 : line;
            size_t n = strlen(l);
            char *t = realloc(text, len + n + 2);
            if (!t)
                break;
            text = t;
            if (lines++)
                text[len++] = '\n';
            memcpy(text + len, l, n + 1);
            len += n;
        }
    }
    if (key[0])
        keep(key, text ? text : "", raw);
    fclose(f);
    free(text);
    /* (in UTF-8, now that the code page is known) */
    for (i = 0; i < NTEXTS; i++)
        if (raw[i]) {
            size_t size = strlen(raw[i]) * 3 + 1;
            translated[i] = malloc(size);
            if (translated[i])
                host_lang_utf8(raw[i], translated[i], size);
            free(raw[i]);
            raw[i] = NULL;
        }
}

#ifndef __wasi__
static int file_exists(const char *p)
{
    FILE *f = fopen(p, "r");
    if (f)
        fclose(f);
    return f != NULL;
}

/* As src/platform/main.c's set_dirs: CIV2_DATA, else data beside the
   program, else (a build in the source tree) ../data/native. */
static void data_dir(char *out, size_t size)
{
    char base[4096], probe[4200];
    const char *d = getenv("CIV2_DATA");
    if (d && *d) {
        snprintf(out, size, "%s", d);
        return;
    }
    host_base_path(base, sizeof base);
    snprintf(out, size, "%sdata", *base ? base : "./");
    snprintf(probe, sizeof probe, "%s/fonts_win.bin", out);
    if (!file_exists(probe)) {
        snprintf(probe, sizeof probe, "%s../data/native/fonts_win.bin", *base ? base : "./");
        if (file_exists(probe))
            snprintf(out, size, "%s../data/native", *base ? base : "./");
    }
}

/* CIV.INI's [OpenCiv2] Language= in dir. */
static void ini_language(const char *dir, char *out, size_t size)
{
    char path[4200], line[512];
    FILE *f;
    int in = 0;
    out[0] = 0;
    snprintf(path, sizeof path, "%s/CIV.INI", dir);
    if (!(f = fopen(path, "r")))
        return;
    while (fgets(line, sizeof line, f)) {
        char *p = line, *eq;
        line[strcspn(line, "\r\n")] = 0;
        while (*p == ' ' || *p == '\t')
            p++;
        if (*p == '[') {
            in = !strncasecmp(p, "[OpenCiv2]", 10);
            continue;
        }
        if (in && (eq = strchr(p, '=')) && eq - p >= 8 && !strncasecmp(p, "Language", 8)) {
            char *v = eq + 1;
            while (*v == ' ')
                v++;
            snprintf(out, size, "%s", v);
            out[strcspn(out, " \t")] = 0;
        }
    }
    fclose(f);
}

static int language_file(const char *game_dir, const char *data, const char *code, char *path, size_t size)
{
    if (!*code || strchr(code, '/') || strchr(code, '\\') || strstr(code, ".."))
        return 0;
    if (game_dir) {
        snprintf(path, size, "%s/lang/%s.txt", game_dir, code);
        if (file_exists(path))
            return 1;
    }
    snprintf(path, size, "%s/lang/%s.txt", data, code);
    return file_exists(path);
}

void host_lang_init(const char *game_dir)
{
    char code[64], c[32], data[4096], path[4400];
    const char *e = getenv("CIV2_LANG");
    size_t i;
    code[0] = 0;
    if (e && *e)
        snprintf(code, sizeof code, "%s", e);
    else {
        if (game_dir)
            ini_language(game_dir, code, sizeof code);
        if (!code[0] || !strcasecmp(code, "Auto")) {
            e = getenv("CIV2_SYSTEM_LANG");
            snprintf(code, sizeof code, "%s", e ? e : "");
        }
    }
    /* as lang.cpp's language_path: "de_DE.UTF-8": de-DE.txt, else de.txt */
    snprintf(c, sizeof c, "%s", code);
    c[strcspn(c, ".@")] = 0;
    for (i = 0; c[i]; i++)
        if (c[i] == '_')
            c[i] = '-';
    if (!c[0] || ((c[0] | 0x20) == 'e' && (c[1] | 0x20) == 'n' && (!c[2] || c[2] == '-')) ||
        !strcasecmp(c, "auto") || !strcmp(c, "C") || !strcmp(c, "POSIX")) {
        host_lang_load(NULL);
        return;
    }
    data_dir(data, sizeof data);
    if (language_file(game_dir, data, c, path, sizeof path)) {
        host_lang_load(path);
        return;
    }
    c[strcspn(c, "-")] = 0;
    host_lang_load(language_file(game_dir, data, c, path, sizeof path) ? path : NULL);
}
#endif

const char *host_text(const char *key)
{
    int i = text_index(key);
#ifndef __wasi__
    if (!inited)
        host_lang_init(NULL);
#endif
    if (i < 0)
        return key;
    return translated[i] ? translated[i] : english[i].english;
}
