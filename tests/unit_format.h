/*
 * Recorded unit tests of the game's logic: what tests/unit_record.c (the
 * recording build, linux32rec) and tests/unit_run.c (the runner,
 * linux32test) share.  docs/DEVELOPMENT.md "Unit tests".
 *
 * A case is one call of one game function as a replay made it: the
 * arguments, the game's state when it was called (the game's data, the
 * section src/data/gamedata.cpp defines, and the map's blocks) and what the
 * call did (its return value and the bytes of that state it changed; the
 * rand() calls it made are in it, the generator's seed being game data).
 *
 * File (gzip): "CIV2UT01", u32 size of the game's data, then cases:
 *   u32 UT_CASE, u32 name length, name (the function's symbol), u32 n (its
 *   nth call in the replay), u32 flags (UT_F_*),
 *   u32 words, then per argument word: u32 kind (UT_ARG_*), u32 a, u32 b
 *     INT: a = value; MAP: a = region, b = offset; STACK: a = offset in
 *     the stack window,
 *   u32 length of the stack window (what pointers into the caller's stack
 *     point to, and what pointers in that point to), u32 its address, its
 *     bytes before the call and a delta to after (pointers into it are moved
 *     with it),
 *   u32 rand calls (informative), u32 sizes[UT_REGIONS],
 *   the input as a delta from the previous case's input (the first: from
 *   the state the game's initializers leave), per region,
 *   u32 return value (eax), u32 edx, the output as a delta from the input,
 *   per region.
 * A delta: u32 runs, each u32 offset, u32 length, the bytes.
 * Text is not recorded: where the game's data holds text (names and
 * messages from the game's files; tests/unit/textmask.tsv) the input and
 * the output have what the game's initializers left there (ut_mask_text),
 * and the runner compares the same way; on the stack, what looks like text
 * is zeroed (ut_mask_stack_text).
 */
#ifndef UNIT_FORMAT_H
#define UNIT_FORMAT_H

#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define UT_MAGIC "CIV2UT01"
#define UT_CASE 0x45534143u     /* "CASE" */
/* regions: the game's data, the map's tiles, what civilizations 1..7 know
   of it, the four coarse grids */
#define UT_REGIONS 13
enum { UT_ARG_INT, UT_ARG_MAP, UT_ARG_STACK };
/* what the function returns: 32 bits, a float (not compared), 8 or 16 bits */
enum { UT_F_RET = 1, UT_F_FLOATRET = 2, UT_F_RET8 = 4, UT_F_RET16 = 8 };
#define UT_STACKBUF 512         /* bytes kept past where a pointer into the stack points */
#define UT_STACKWINDOW 16384    /* at most, from the lowest such pointer */

extern char __start_civ2data[], __stop_civ2data[];

/* The map's blocks (src/data/state/map.h MapStorage) and its header. */
typedef struct {
    uint8_t *tiles;
    uint8_t tiles_rest[4];
    void *known_handles[8];
    uint8_t *known[8];
    uint8_t *coarse[4];
    uint8_t allocated;
} UtMapStorage;
typedef struct { int16_t width, height, area; uint16_t flat, seed; int16_t quarter_w, quarter_h; } UtMapHeader;
extern UtMapStorage map_data;
extern UtMapHeader map;

static inline uint32_t ut_round4(uint32_t n) { return (n + 3) & ~3u; }

/* Sizes of the regions as the map's header says (map_alloc). */
static inline void ut_region_sizes(uint32_t *size)
{
    int i;
    size[0] = (uint32_t)(__stop_civ2data - __start_civ2data);
    for (i = 1; i < UT_REGIONS; i++)
        size[i] = 0;
    if (!map_data.allocated || map.area <= 0)
        return;
    size[1] = ut_round4((uint32_t)map.area * 6);
    for (i = 2; i <= 8; i++)
        size[i] = ut_round4((uint32_t)map.area);
    for (i = 9; i <= 12; i++)
        size[i] = ut_round4((uint32_t)(map.quarter_w * map.quarter_h));
}

static inline uint8_t *ut_region_ptr(int r)
{
    if (r == 0)
        return (uint8_t *)__start_civ2data;
    if (r == 1)
        return map_data.tiles;
    if (r <= 8)
        return map_data.known[r - 1];
    return map_data.coarse[r - 9];
}

/* Text in the game's data (tests/unit/textmask.tsv, tools/unit_textmask.py):
   set to what the game's initializers left there (the program's own
   strings; zeros where text from the game's files goes). */
typedef struct { uint32_t off, len; } UtRange;
static UtRange *ut_text;
static int ut_ntext;

static inline int ut_load_textmask(const char *path)
{
    FILE *f = fopen(path, "r");
    char line[512];
    uint32_t gsize = (uint32_t)(__stop_civ2data - __start_civ2data);
    if (!f)
        return 0;
    while (fgets(line, sizeof line, f)) {
        unsigned off, len;
        if (line[0] == '#' || sscanf(line, "%x %u", &off, &len) != 2 || off >= gsize)
            continue;
        ut_text = (UtRange *)realloc(ut_text, (ut_ntext + 1) * sizeof *ut_text);
        ut_text[ut_ntext].off = off;
        ut_text[ut_ntext].len = len < gsize - off ? len : gsize - off;
        ut_ntext++;
    }
    fclose(f);
    return ut_ntext;
}

static inline void ut_mask_text(uint8_t *g, const uint8_t *init)
{
    int i;
    for (i = 0; i < ut_ntext; i++)
        memcpy(g + ut_text[i].off, init + ut_text[i].off, ut_text[i].len);
}

/* Text on the stack (what pointer arguments point to: a caller's buffers):
   no types to go by, so runs of text characters with at least three letters
   ended by a NUL, or any run of eight with five letters, are zeroed. */
static inline int ut_textch(uint8_t c) { return (c >= 0x20 && c < 0x7f) || c == '\t' || c == '\n' || c == '\r'; }
static inline int ut_letter(uint8_t c) { return (c | 0x20) >= 'a' && (c | 0x20) <= 'z'; }

static inline void ut_mask_stack_text(uint8_t *p, uint32_t n)
{
    uint32_t i = 0, j, letters;
    while (i < n) {
        if (!ut_textch(p[i])) {
            i++;
            continue;
        }
        letters = 0;
        for (j = i; j < n && ut_textch(p[j]); j++)
            letters += ut_letter(p[j]);
        if ((letters >= 3 && j < n && p[j] == 0) || (j - i >= 8 && letters >= 5))
            memset(p + i, 0, j - i);
        i = j;
    }
}

/* The map's pointers and handles in the game's data: the runner's own. */
static inline void ut_mask_fixed(uint8_t *g)
{
    extern uint8_t map_handles[32];
    memset(g + ((char *)&map_data - __start_civ2data), 0, sizeof(UtMapStorage));
    memset(g + ((char *)map_handles - __start_civ2data), 0, 32);
}

/* ---- the program's symbols (its ELF symbol table, the static functions
   too) ---- */
typedef struct { uint32_t addr, size; const char *name; } UtSym;

static UtSym *ut_syms;
static int ut_nsyms;

static int ut_sym_by_addr(const void *a, const void *b)
{
    uint32_t x = ((const UtSym *)a)->addr, y = ((const UtSym *)b)->addr;
    return x < y ? -1 : x > y;
}

static inline int ut_load_symbols(void)
{
    FILE *f = fopen("/proc/self/exe", "rb");
    Elf32_Ehdr eh;
    Elf32_Shdr *sh;
    int i;
    if (!f || fread(&eh, sizeof eh, 1, f) != 1)
        return 0;
    sh = (Elf32_Shdr *)malloc(eh.e_shnum * sizeof *sh);
    fseek(f, eh.e_shoff, SEEK_SET);
    if (fread(sh, sizeof *sh, eh.e_shnum, f) != eh.e_shnum)
        return 0;
    for (i = 0; i < eh.e_shnum; i++) {
        Elf32_Sym *sym;
        char *str;
        uint32_t k, n;
        if (sh[i].sh_type != SHT_SYMTAB)
            continue;
        n = sh[i].sh_size / sizeof(Elf32_Sym);
        sym = (Elf32_Sym *)malloc(sh[i].sh_size);
        str = (char *)malloc(sh[sh[i].sh_link].sh_size);
        fseek(f, sh[i].sh_offset, SEEK_SET);
        if (fread(sym, sizeof *sym, n, f) != n)
            return 0;
        fseek(f, sh[sh[i].sh_link].sh_offset, SEEK_SET);
        if (fread(str, 1, sh[sh[i].sh_link].sh_size, f) != sh[sh[i].sh_link].sh_size)
            return 0;
        ut_syms = (UtSym *)malloc(n * sizeof *ut_syms);
        for (k = 0; k < n; k++)
            if ((ELF32_ST_TYPE(sym[k].st_info) == STT_FUNC || ELF32_ST_TYPE(sym[k].st_info) == STT_OBJECT) &&
                sym[k].st_value && sym[k].st_shndx != SHN_UNDEF) {
                ut_syms[ut_nsyms].addr = sym[k].st_value;
                ut_syms[ut_nsyms].size = sym[k].st_size;
                ut_syms[ut_nsyms].name = str + sym[k].st_name;
                ut_nsyms++;
            }
        free(sym);
        break;
    }
    fclose(f);
    free(sh);
    qsort(ut_syms, ut_nsyms, sizeof *ut_syms, ut_sym_by_addr);
    return ut_nsyms;
}

/* The address of a function by its symbol; 0 if there is none or several. */
static inline void *ut_lookup(const char *name)
{
    int i, found = 0;
    void *r = 0;
    for (i = 0; i < ut_nsyms; i++)
        if (!strcmp(ut_syms[i].name, name)) {
            r = (void *)(uintptr_t)ut_syms[i].addr;
            found++;
        }
    if (!found && !strncmp(name, "_Z", 2) && name[2] != 'N') {
        /* a free function whose parameter types changed (int -> RECT *: the
           same argument words): its mangled name up to the parameters */
        const char *p = name + 2 + (name[2] == 'L');
        size_t len = 0;
        while (*p >= '0' && *p <= '9')
            len = len * 10 + (size_t)(*p++ - '0');
        if (len && strlen(p) >= len) {
            size_t pre = (size_t)(p - name) + len;
            for (i = 0; i < ut_nsyms; i++)
                if (!strncmp(ut_syms[i].name, name, pre) && ut_syms[i].name[pre]
                        && !(ut_syms[i].name[pre] >= '0' && ut_syms[i].name[pre] <= '9')) {
                    r = (void *)(uintptr_t)ut_syms[i].addr;
                    found++;
                }
        }
    }
    return found == 1 ? r : 0;
}

/* ---- deltas ---- */
static inline void ut_put(gzFile f, uint32_t v) { gzwrite(f, &v, 4); }

/* The runs where b differs from a (n bytes), gaps under 8 bytes merged. */
static inline void ut_write_delta(gzFile f, const uint8_t *a, const uint8_t *b, uint32_t n)
{
    uint32_t i = 0, runs = 0, start, end;
    /* count first */
    while (i < n) {
        if (a[i] == b[i]) {
            i++;
            continue;
        }
        end = i + 1;
        for (start = i, i++; i < n && i < end + 8; i++)
            if (a[i] != b[i])
                end = i + 1;
        i = end;
        runs++;
        (void)start;
    }
    ut_put(f, runs);
    for (i = 0; i < n;) {
        if (a[i] == b[i]) {
            i++;
            continue;
        }
        end = i + 1;
        for (start = i, i++; i < n && i < end + 8; i++)
            if (a[i] != b[i])
                end = i + 1;
        i = end;
        ut_put(f, start);
        ut_put(f, end - start);
        gzwrite(f, b + start, end - start);
    }
}

static inline int ut_get(gzFile f, uint32_t *v) { return gzread(f, v, 4) == 4; }

/* Applies a delta to p (n bytes); 0 if it does not fit. */
static inline int ut_read_delta(gzFile f, uint8_t *p, uint32_t n)
{
    uint32_t runs, off, len;
    if (!ut_get(f, &runs))
        return 0;
    while (runs--) {
        if (!ut_get(f, &off) || !ut_get(f, &len) || off > n || len > n - off)
            return 0;
        if (gzread(f, p + off, len) != (int)len)
            return 0;
    }
    return 1;
}

#endif
