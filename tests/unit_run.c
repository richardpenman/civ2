/*
 * The unit tests' runner (build_native.py --target linux32test, make test):
 * for each recorded case (tests/unit_format.h, recorded by
 * tests/unit_record.c from the replays) it sets the game's state as it was
 * at the call, calls the function with the recorded arguments and compares
 * what it returns and changes with what the reference run did, rand() calls
 * included (the generator's seed is game data).  No window, no game files.
 *
 *   civ2-unittest [-v] [-f FUNCTION] [--mask TEXTMASK] FILE.gz...
 *                       (TEXTMASK: default tests/unit/textmask.tsv)
 *   civ2-unittest --bless OUT.gz RAW.gz      keep the cases that pass, twice,
 *                                            on this build (after recording)
 *   civ2-unittest --rules GAME               the built-in rules against GAME's
 *                                            RULES.TXT (src/game/rules.cpp)
 *   civ2-unittest --text GAME OUT            the export of GAME's texts, keyed
 *                                            (src/game/lang.cpp: make test,
 *                                            tests/text_keys_test.py)
 * The program's own start-up (src/platform/main.c, CIV2_UNITTEST) runs the
 * game's initializers first: the first case's input is a delta from what
 * they leave, as when it was recorded.
 */
#include <setjmp.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "unit_format.h"

extern uint32_t rand_seed;

typedef struct {
    char name[256];
    void *fn;
    uint32_t call, flags, nwords, kind[24], a[24], b[24];
    uint32_t win_len, win_lo;           /* the stack window */
    uint8_t win_in[UT_STACKWINDOW], win_out[UT_STACKWINDOW];
    uint32_t size[UT_REGIONS];
    uint32_t ret, edx;
} Case;

static uint8_t *base;                   /* the game's data as the initializers leave it, masked */
static uint8_t *init_data;              /* the same, as it is */
static uint8_t *chain[UT_REGIONS], *expect[UT_REGIONS], *live_map[UT_REGIONS], *got;
static uint32_t chain_size[UT_REGIONS], cap_expect[UT_REGIONS], cap_live[UT_REGIONS];
static uint8_t stackarea[UT_STACKWINDOW + 64] __attribute__((aligned(16)));
static uint8_t *stackwin;

/* Pointers into the stack window, from where it was to where it is (or
   back). */
static void move_window(uint8_t *p, uint32_t len, uint32_t lo, uint32_t from, uint32_t to)
{
    uint32_t a;
    for (a = (lo + 3) & ~3u; a + 4 <= lo + len; a += 4) {
        uint32_t *w = (uint32_t *)(p + (a - lo));
        if (*w >= from && *w < from + len)
            *w = *w - from + to;
    }
}
static int verbose;
static const char *only;

/* The build is instrumented as the recording build is, so that every
   function takes its arguments on the stack (tools/build_native.py). */
void __cyg_profile_func_enter(void *fn, void *site) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *fn, void *site) __attribute__((no_instrument_function));
void __cyg_profile_func_enter(void *fn, void *site) { (void)fn; (void)site; }
void __cyg_profile_func_exit(void *fn, void *site) { (void)fn; (void)site; }

/* ---- calling a function with its argument words ---- */
uint32_t ut_call_args[24], ut_call_ret;
void *ut_call_fn;

/* The function runs on a stack of its own, the program's stack out of
   reach meanwhile: a pointer into the recording's stack left in the state
   must not reach the runner's (where it lands differs from run to run). */
uint32_t ut_case_stack_top;
static uint32_t real_stack_lo, real_stack_hi;

void ut_real_stack(int on) __attribute__((no_instrument_function));
void ut_real_stack(int on)
{
    if (real_stack_hi)
        mprotect((void *)(uintptr_t)real_stack_lo, real_stack_hi - real_stack_lo, on ? PROT_READ | PROT_WRITE : PROT_NONE);
}

static void case_stack_init(void)
{
    FILE *maps = fopen("/proc/self/maps", "r");
    char line[512];
    void *p = mmap(0, 16 << 20, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ut_case_stack_top = (uint32_t)(uintptr_t)p + (16 << 20) - 64;
    while (maps && fgets(line, sizeof line, maps)) {
        unsigned lo, hi;
        if (strstr(line, "[stack]") && sscanf(line, "%x-%x", &lo, &hi) == 2) {
            real_stack_lo = lo;
            real_stack_hi = hi;
        }
    }
    if (maps)
        fclose(maps);
}

static void call_fn(void)
{
    __asm__ volatile("push %%ebp\n\t"
                     "push %%ebx\n\t"
                     "push %%esi\n\t"
                     "push %%edi\n\t"
                     "mov %%esp, %%esi\n\t"
                     "mov ut_case_stack_top, %%esp\n\t"
                     "sub $16, %%esp\n\t"
                     "and $-16, %%esp\n\t"
                     "movl $0, (%%esp)\n\t"
                     "call ut_real_stack\n\t"
                     "sub $128, %%esp\n\t"
                     "and $-16, %%esp\n\t"
                     "xor %%ecx, %%ecx\n"
                     "1:\n\t"
                     "mov ut_call_args(,%%ecx,4), %%eax\n\t"
                     "mov %%eax, (%%esp,%%ecx,4)\n\t"
                     "inc %%ecx\n\t"
                     "cmp $24, %%ecx\n\t"
                     "jb 1b\n\t"
                     "call *ut_call_fn\n\t"
                     "mov %%eax, ut_call_ret\n\t"
                     "and $-16, %%esp\n\t"
                     "movl $1, (%%esp)\n\t"
                     "call ut_real_stack\n\t"
                     "mov %%esi, %%esp\n\t"
                     "pop %%edi\n\t"
                     "pop %%esi\n\t"
                     "pop %%ebx\n\t"
                     "pop %%ebp\n\t"
                     ::: "eax", "ecx", "edx", "memory", "cc");
}

static void pop_x87(void)
{
    __asm__ volatile("fstp %%st(0)" ::: "memory");
}

static sigjmp_buf guard;
static volatile int guarded;
static const char *sig_name;

static void on_signal(int sig)
{
    ut_real_stack(1);
    if (!guarded)
        _exit(3);
    sig_name = sig == SIGALRM ? "timed out" : sig == SIGSEGV ? "crashed (SIGSEGV)" : sig == SIGFPE ? "crashed (SIGFPE)"
             : sig == SIGBUS ? "crashed (SIGBUS)" : "crashed";
    siglongjmp(guard, 1);
}

static void guard_signals(void)
{
    static char altstack[1 << 16];
    stack_t ss;
    struct sigaction sa;
    int sigs[] = {SIGSEGV, SIGFPE, SIGBUS, SIGILL, SIGALRM, SIGABRT}, i;
    ss.ss_sp = altstack;
    ss.ss_size = sizeof altstack;
    ss.ss_flags = 0;
    sigaltstack(&ss, 0);
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = on_signal;
    sa.sa_flags = SA_ONSTACK | SA_NODEFER;
    for (i = 0; i < 6; i++)
        sigaction(sigs[i], &sa, 0);
}

/* ---- names of places in the game's data, for reports ---- */
static const char *where(uint32_t off, uint32_t *rel)
{
    uint32_t a = (uint32_t)(uintptr_t)__start_civ2data + off;
    const char *best = "?";
    uint32_t best_addr = 0;
    int i;
    for (i = 0; i < ut_nsyms; i++)
        if (ut_syms[i].addr <= a && ut_syms[i].addr >= best_addr && ut_syms[i].addr >= (uint32_t)(uintptr_t)__start_civ2data) {
            best = ut_syms[i].name;
            best_addr = ut_syms[i].addr;
        }
    *rel = a - best_addr;
    return best;
}

static const char *region_name(int r)
{
    static const char *names[UT_REGIONS] = {"data", "map tiles", "known 1", "known 2", "known 3", "known 4", "known 5",
                                            "known 6", "known 7", "coarse 0", "coarse 1", "coarse 2", "coarse 3"};
    return names[r];
}

/* ---- reading a case ---- */
static void grow(uint8_t **p, uint32_t *cap, uint32_t n)
{
    if (*cap < n) {
        *p = (uint8_t *)realloc(*p, n);
        *cap = n;
    }
}

static void *find_fn(const char *name)
{
    static char last[256];
    static void *last_fn;
    if (strcmp(last, name)) {
        snprintf(last, sizeof last, "%s", name);
        last_fn = ut_lookup(name);
    }
    return last_fn;
}

static int read_case(gzFile f, Case *c)
{
    uint32_t tag, len, i, r, rnd;
    static uint32_t chain_cap[UT_REGIONS];
    if (!ut_get(f, &tag))
        return 0;
    if (tag != UT_CASE || !ut_get(f, &len) || len >= sizeof c->name)
        return -1;
    if (gzread(f, c->name, len) != (int)len)
        return -1;
    c->name[len] = 0;
    c->fn = find_fn(c->name);
    if (!ut_get(f, &c->call) || !ut_get(f, &c->flags) || !ut_get(f, &c->nwords) || c->nwords > 24)
        return -1;
    for (i = 0; i < c->nwords; i++) {
        if (!ut_get(f, &c->kind[i]) || !ut_get(f, &c->a[i]) || !ut_get(f, &c->b[i]))
            return -1;
    }
    if (!ut_get(f, &c->win_len) || c->win_len > UT_STACKWINDOW || !ut_get(f, &c->win_lo))
        return -1;
    if (c->win_len && gzread(f, c->win_in, c->win_len) != (int)c->win_len)
        return -1;
    memcpy(c->win_out, c->win_in, c->win_len);
    if (c->win_len && !ut_read_delta(f, c->win_out, c->win_len))
        return -1;
    for (i = 0; i < c->nwords; i++)
        if (c->kind[i] == UT_ARG_STACK && c->a[i] >= c->win_len)
            return -1;
    if (!ut_get(f, &rnd))
        return -1;
    for (r = 0; r < UT_REGIONS; r++)
        if (!ut_get(f, &c->size[r]))
            return -1;
    if (c->size[0] != chain_size[0])
        return -1;
    for (r = 0; r < UT_REGIONS; r++) {
        if (chain_size[r] != c->size[r]) {
            grow(&chain[r], &chain_cap[r], c->size[r] ? c->size[r] : 1);
            memset(chain[r], 0, c->size[r]);
            chain_size[r] = c->size[r];
        }
        if (!ut_read_delta(f, chain[r], c->size[r]))
            return -1;
    }
    /* (the text mask applies to what was recorded too: a mask made wider
       after a recording takes effect when the cases are blessed again) */
    ut_mask_text(chain[0], init_data);
    if (!ut_get(f, &c->ret) || !ut_get(f, &c->edx))
        return -1;
    for (r = 0; r < UT_REGIONS; r++) {
        grow(&expect[r], &cap_expect[r], c->size[r] ? c->size[r] : 1);
        memcpy(expect[r], chain[r], c->size[r]);
        if (!ut_read_delta(f, expect[r], c->size[r]))
            return -1;
    }
    ut_mask_text(expect[0], init_data);
    return 1;
}

/* ---- running one ---- */
static uint32_t rand_steps(uint32_t from, uint32_t to)
{
    uint32_t n;
    for (n = 0; n < 1000000 && from != to; n++)
        from = from * 214013u + 2531011u;
    return from == to ? n : 0xffffffffu;
}

/* Runs the case; returns 1 if it did what was recorded, else 0 and why in msg. */
static int run_case(Case *c, char *msg, size_t msgsize, uint32_t *rands)
{
    uint32_t r, i, seed_in;
    uint32_t sz[UT_REGIONS];
    *msg = 0;
    if (!c->fn) {
        snprintf(msg, msgsize, "no function %s in this build", c->name);
        return 0;
    }
    /* the state */
    memcpy(__start_civ2data, chain[0], c->size[0]);
    for (r = 1; r < UT_REGIONS; r++) {
        grow(&live_map[r], &cap_live[r], c->size[r] + 64);
        memcpy(live_map[r], chain[r], c->size[r]);
    }
    map_data.tiles = c->size[1] ? live_map[1] : 0;
    for (r = 2; r <= 8; r++)
        map_data.known[r - 1] = c->size[r] ? live_map[r] : 0;
    for (r = 9; r <= 12; r++)
        map_data.coarse[r - 9] = c->size[r] ? live_map[r] : 0;
    map_data.allocated = c->size[1] != 0;
    /* the arguments */
    stackwin = stackarea + (c->win_lo & 15);
    memset(ut_call_args, 0, sizeof ut_call_args);
    for (i = 0; i < c->nwords; i++) {
        if (c->kind[i] == UT_ARG_INT)
            ut_call_args[i] = c->a[i];
        else if (c->kind[i] == UT_ARG_MAP)
            ut_call_args[i] = (uint32_t)(uintptr_t)(live_map[c->a[i]] + c->b[i]);
        else
            ut_call_args[i] = (uint32_t)(uintptr_t)(stackwin + c->a[i]);
    }
    memset(stackarea, 0, sizeof stackarea);
    stackwin = stackarea + (c->win_lo & 15);      /* aligned as it was */
    memcpy(stackwin, c->win_in, c->win_len);
    move_window(stackwin, c->win_len, c->win_lo, c->win_lo, (uint32_t)(uintptr_t)stackwin);
    seed_in = rand_seed;
    ut_call_fn = c->fn;
    sig_name = 0;
    if (sigsetjmp(guard, 1) == 0) {
        guarded = 1;
        alarm(10);
        call_fn();
        if (c->flags & UT_F_FLOATRET)
            pop_x87();
        alarm(0);
        guarded = 0;
    } else {
        alarm(0);
        guarded = 0;
        snprintf(msg, msgsize, "%s", sig_name);
        return 0;
    }
    *rands = rand_steps(seed_in, rand_seed);
    /* what it did */
    if (c->flags & (UT_F_RET | UT_F_RET8 | UT_F_RET16)) {
        uint32_t mask = c->flags & UT_F_RET8 ? 0xff : c->flags & UT_F_RET16 ? 0xffff : 0xffffffffu;
        if ((ut_call_ret & mask) != (c->ret & mask)) {
            snprintf(msg, msgsize, "returned %d (0x%x), expected %d (0x%x)", (int)(ut_call_ret & mask), ut_call_ret & mask,
                     (int)(c->ret & mask), c->ret & mask);
            return 0;
        }
    }
    ut_region_sizes(sz);
    memcpy(got, __start_civ2data, c->size[0]);
    ut_mask_text(got, init_data);
    ut_mask_fixed(got);
    for (r = 0; r < UT_REGIONS; r++) {
        const uint8_t *g = r ? live_map[r] : got;
        if (r && sz[r] != c->size[r] && (sz[r] || c->size[r])) {
            snprintf(msg, msgsize, "the map's size changed");
            return 0;
        }
        if (memcmp(g, expect[r], c->size[r])) {
            uint32_t off = 0, n = 0, rel;
            while (g[off] == expect[r][off])
                off++;
            for (i = 0; i < c->size[r]; i++)
                n += g[i] != expect[r][i];
            if (r == 0) {
                const char *v = where(off, &rel);
                if (v == (const char *)0 || !strcmp(v, "rand_seed"))
                    snprintf(msg, msgsize, "rand(): %u calls, expected %u", *rands, rand_steps(seed_in, *(uint32_t *)(expect[0] + off - rel)));
                else
                    snprintf(msg, msgsize, "%u bytes differ, first at %s+0x%x: 0x%02x, expected 0x%02x", n, v, rel, g[off], expect[r][off]);
            } else
                snprintf(msg, msgsize, "%u bytes differ in %s, first at +0x%x: 0x%02x, expected 0x%02x", n, region_name(r), off,
                         g[off], expect[r][off]);
            return 0;
        }
    }
    move_window(stackwin, c->win_len, c->win_lo, (uint32_t)(uintptr_t)stackwin, c->win_lo);
    ut_mask_stack_text(stackwin, c->win_len);
    if (memcmp(stackwin, c->win_out, c->win_len)) {
        for (i = 0; stackwin[i] == c->win_out[i]; i++)
            ;
        snprintf(msg, msgsize, "what the pointer arguments point to differs (stack window +0x%x)", i);
        return 0;
    }
    return 1;
}

/* ---- writing a case (--bless) ---- */
static uint8_t *bchain[UT_REGIONS];
static uint32_t bchain_size[UT_REGIONS], bchain_cap[UT_REGIONS];

static void write_case(gzFile o, Case *c, uint32_t rands)
{
    uint32_t i, r;
    ut_put(o, UT_CASE);
    ut_put(o, (uint32_t)strlen(c->name));
    gzwrite(o, c->name, (unsigned)strlen(c->name));
    ut_put(o, c->call);
    ut_put(o, c->flags);
    ut_put(o, c->nwords);
    for (i = 0; i < c->nwords; i++) {
        ut_put(o, c->kind[i]);
        ut_put(o, c->a[i]);
        ut_put(o, c->b[i]);
    }
    ut_put(o, c->win_len);
    ut_put(o, c->win_lo);
    if (c->win_len) {
        gzwrite(o, c->win_in, c->win_len);
        ut_write_delta(o, c->win_in, c->win_out, c->win_len);
    }
    ut_put(o, rands);
    for (r = 0; r < UT_REGIONS; r++)
        ut_put(o, c->size[r]);
    for (r = 0; r < UT_REGIONS; r++) {
        if (bchain_size[r] != c->size[r]) {
            grow(&bchain[r], &bchain_cap[r], c->size[r] ? c->size[r] : 1);
            memset(bchain[r], 0, c->size[r]);
            bchain_size[r] = c->size[r];
        }
        ut_write_delta(o, bchain[r], chain[r], c->size[r]);
        memcpy(bchain[r], chain[r], c->size[r]);
    }
    ut_put(o, c->ret);
    ut_put(o, 0);
    for (r = 0; r < UT_REGIONS; r++)
        ut_write_delta(o, chain[r], expect[r], c->size[r]);
}

/* --bless: the stack window cut to the shortest that does the same (what
   lies past the objects the arguments point to is mostly stale stack). */
static void shrink_window(Case *c)
{
    uint32_t full = c->win_len, amax = 0, len, i, r;
    char msg[512];
    for (i = 0; i < c->nwords; i++)
        if (c->kind[i] == UT_ARG_STACK && c->a[i] > amax)
            amax = c->a[i];
    for (len = (amax + 64) & ~15u; len < full; len *= 2) {
        uint32_t at;
        int cut = 0;
        /* not where a pointer in what is kept points to what would go */
        for (at = (c->win_lo + 3) & ~3u; at + 4 <= c->win_lo + len; at += 4) {
            uint32_t v;
            memcpy(&v, c->win_in + (at - c->win_lo), 4);
            cut |= v >= c->win_lo + len && v < c->win_lo + full;
        }
        if (cut)
            continue;
        c->win_len = len;
        if (run_case(c, msg, sizeof msg, &r) && run_case(c, msg, sizeof msg, &r))
            return;
    }
    c->win_len = full;
}

typedef struct { const char *name; uint32_t pass, fail; } FnStat;
static FnStat *stats;
static int nstats;

static FnStat *stat_of(const char *name)
{
    int i;
    for (i = nstats - 1; i >= 0; i--)
        if (!strcmp(stats[i].name, name))
            return &stats[i];
    stats = (FnStat *)realloc(stats, (nstats + 1) * sizeof *stats);
    stats[nstats].name = strdup(name);
    stats[nstats].pass = stats[nstats].fail = 0;
    return &stats[nstats++];
}

static int run_file(const char *path, gzFile bless, uint32_t *npass, uint32_t *nfail, uint32_t *nskip)
{
    gzFile f = gzopen(path, "rb");
    char magic[8], msg[512], msg2[512];
    uint32_t gsize, r, rands, rands2;
    static Case c;
    int k, shown = 0;
    if (!f) {
        fprintf(stderr, "%s: cannot open\n", path);
        return 0;
    }
    if (gzread(f, magic, 8) != 8 || memcmp(magic, UT_MAGIC, 8) || !ut_get(f, &gsize)) {
        fprintf(stderr, "%s: not a file of cases\n", path);
        return 0;
    }
    if (gsize != (uint32_t)(__stop_civ2data - __start_civ2data)) {
        fprintf(stderr, "%s: recorded with %u bytes of game data, this build has %u: the data's layout changed; "
                "record the cases again (docs/DEVELOPMENT.md)\n", path, gsize, (uint32_t)(__stop_civ2data - __start_civ2data));
        return 0;
    }
    /* each file starts from the initializers' state */
    for (r = 0; r < UT_REGIONS; r++)
        chain_size[r] = 0;
    chain_size[0] = gsize;
    chain[0] = (uint8_t *)realloc(chain[0], gsize);
    memcpy(chain[0], base, gsize);
    for (r = 0; r < UT_REGIONS; r++)
        bchain_size[r] = 0;
    if (bless) {
        bchain_size[0] = gsize;
        bchain[0] = (uint8_t *)realloc(bchain[0], gsize);
        memcpy(bchain[0], base, gsize);
    }
    while ((k = read_case(f, &c)) > 0) {
        int ok;
        if (only && strcmp(only, c.name)) {
            continue;
        }
        ok = run_case(&c, msg, sizeof msg, &rands);
        if (bless) {
            /* twice: a case must not depend on what the first run left */
            if (ok && !run_case(&c, msg2, sizeof msg2, &rands2))
                ok = 0;
            if (ok && c.win_len)
                shrink_window(&c);
            if (ok) {
                write_case(bless, &c, rands);
                (*npass)++;
            } else {
                (*nfail)++;
                if (verbose)
                    printf("drop %s (call %u): %s\n", c.name, c.call, msg);
            }
            stat_of(c.name)->pass += ok;
            continue;
        }
        if (!c.fn) {
            (*nskip)++;
            if (verbose)
                printf("SKIP %s: %s\n", c.name, msg);
            continue;
        }
        if (ok) {
            (*npass)++;
            stat_of(c.name)->pass++;
        } else {
            (*nfail)++;
            stat_of(c.name)->fail++;
            if (verbose || shown++ < 10)
                printf("FAIL %s (%s, call %u): %s\n", c.name, path, c.call, msg);
        }
    }
    if (k < 0)
        fprintf(stderr, "%s: truncated or damaged after %u cases\n", path, *npass + *nfail + *nskip);
    gzclose(f);
    return 1;
}

int unit_run_main(int argc, char **argv)
{
    const char *bless_path = 0, *mask = "tests/unit/textmask.tsv";
    gzFile bless = 0;
    uint32_t pass = 0, fail = 0, skip = 0, gsize = (uint32_t)(__stop_civ2data - __start_civ2data);
    int i, files = 0, fns_failed = 0;
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    if (argc == 3 && !strcmp(argv[1], "--rules")) {
        /* the built-in rules against the game's RULES.TXT (src/game/rules.cpp) */
        int rules_check(const char *dir);
        return rules_check(argv[2]);
    }
    if (argc == 4 && !strcmp(argv[1], "--text")) {
        /* the export of the game's texts, keyed (src/game/lang.cpp) */
        int lang_export_check(const char *dir, const char *path);
        return lang_export_check(argv[2], argv[3]);
    }
    if (!ut_load_symbols()) {
        fprintf(stderr, "civ2-unittest: no symbol table\n");
        return 2;
    }
    base = (uint8_t *)malloc(gsize);
    got = (uint8_t *)malloc(gsize);
    init_data = (uint8_t *)malloc(gsize);
    memcpy(init_data, __start_civ2data, gsize);
    memcpy(base, __start_civ2data, gsize);
    ut_mask_fixed(base);
    if (!getenv("CIV2_UT_NOGUARD"))     /* debugging: crash where it crashes */
        guard_signals();
    case_stack_init();
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v"))
            verbose = 1;
        else if (!strcmp(argv[i], "-f") && i + 1 < argc)
            only = argv[++i];
        else if (!strcmp(argv[i], "--mask") && i + 1 < argc)
            mask = argv[++i];
        else if (!strcmp(argv[i], "--bless") && i + 1 < argc)
            bless_path = argv[++i];
        else {
            if (!ut_ntext && !ut_load_textmask(mask)) {
                fprintf(stderr, "civ2-unittest: no %s\n", mask);
                return 2;
            }
            if (bless_path && !bless) {
                bless = gzopen(bless_path, "wb9");
                if (!bless) {
                    fprintf(stderr, "cannot write %s\n", bless_path);
                    return 2;
                }
                gzwrite(bless, UT_MAGIC, 8);
                ut_put(bless, gsize);
            }
            if (!run_file(argv[i], bless, &pass, &fail, &skip))
                return 2;
            files++;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    if (bless) {
        int kept_fns = 0;
        gzclose(bless);
        for (i = 0; i < nstats; i++)
            kept_fns += stats[i].pass > 0;
        printf("kept %u cases of %d functions, dropped %u\n", pass, kept_fns, fail);
        fflush(stdout);
        _exit(0);
    }
    for (i = 0; i < nstats; i++)
        fns_failed += stats[i].fail > 0;
    printf("%u cases of %d functions from %d files: %u passed, %u failed (%d functions), %.1f s\n", pass + fail, nstats,
           files, pass, fail, fns_failed, (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9);
    if (skip)
        printf("%u cases skipped: their function is not in this build (renamed or removed?)\n", skip);
    /* not through exit(): the game's atexit destructors would free what the
       last case's state points to */
    fflush(stdout);
    _exit(fail ? 1 : 0);
}
