/*
 * Entry of the portable build: runs the game's wasm32 module (translated to
 * C by wasm2c) with its imports -- the host interface (host.h, implemented
 * by host_sdl.c) and WASI (wasi.c).  The host functions take module
 * addresses, translated here.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "port.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"     /* wasm_rt_set_unwind_target (wasm_rt_impl_try) */
#include "host.h"
#include "setup.h"

wasm_rt_memory_t *port_memory;

/* The module's view of a host path: on Windows C:\x\y is /c/x/y (wasi.c
 * maps it back); elsewhere the same path. */
static void module_path(const char *in, char *out, size_t n)
{
#ifdef _WIN32
    if (in[0] && in[1] == ':' && (in[2] == '\\' || in[2] == '/' || !in[2])) {
        size_t i, k = 0;
        if (n < 4)
            return;
        out[k++] = '/';
        out[k++] = (char)(in[0] | 0x20);
        for (i = 2; in[i] && k + 1 < n; i++)
            out[k++] = in[i] == '\\' ? '/' : in[i];
        out[k] = 0;
        return;
    }
#endif
    snprintf(out, n, "%s", in);
}

#define HOST(name) w2c_host_host_##name
static w2c_civ2 module;

void host_modal_tick(void)
{
    w2c_civ2_plat_modal_tick(&module);
}

typedef struct w2c_host Host;

u32 HOST(init)(Host *h, u32 headless) { (void)h; return (u32)host_init((int)headless); }
void HOST(quit)(Host *h) { (void)h; host_quit(); }
u32 HOST(base_path)(Host *h, u32 buf, u32 size)
{
    char p[4096];
    (void)h;
    host_base_path(p, sizeof p);
    module_path(p, MEM(buf), size);
    return (u32)strlen(MEM(buf));
}
u32 HOST(open_window)(Host *h, u32 w, u32 hh, u32 scale) { (void)h; return (u32)host_open_window((int)w, (int)hh, (int)scale); }
u32 HOST(display_size)(Host *h, u32 w, u32 hh)
{
    int32_t a = 0, b = 0;
    u32 r;
    (void)h;
    r = (u32)host_display_size(&a, &b);
    PUT32(w, (u32)a);
    PUT32(hh, (u32)b);
    return r;
}
u32 HOST(usable_size)(Host *h, u32 w, u32 hh)
{
    int32_t a = 0, b = 0;
    u32 r;
    (void)h;
    r = (u32)host_usable_size(&a, &b);
    PUT32(w, (u32)a);
    PUT32(hh, (u32)b);
    return r;
}
u32 HOST(open_maximized_window)(Host *h, u32 w, u32 hh, u32 scale)
{
    int32_t a = 0, b = 0;
    u32 r;
    (void)h;
    r = (u32)host_open_maximized_window(&a, &b, (int)scale);
    PUT32(w, (u32)a);
    PUT32(hh, (u32)b);
    return r;
}
u32 HOST(open_native_window)(Host *h, u32 w, u32 hh, u32 mode, u32 scale)
{
    int32_t a = 0, b = 0;
    u32 r;
    (void)h;
    r = (u32)host_open_native_window(&a, &b, (int)mode, (int)scale);
    PUT32(w, (u32)a);
    PUT32(hh, (u32)b);
    return r;
}
void HOST(menu_begin)(Host *h) { (void)h; host_menu_begin(); }
void HOST(menu_add)(Host *h, u32 level, u32 flags, u32 id, u32 text)
{
    (void)h;
    host_menu_add((int)level, (int)flags, (int)id, MEM(text));
}
void HOST(menu_end)(Host *h) { (void)h; host_menu_end(); }
void HOST(present)(Host *h, u32 pixels, u32 stride) { (void)h; host_present(MEM(pixels), (int)stride); }
void HOST(minimize)(Host *h) { (void)h; host_minimize(); }

static u32 event_out(int got, const HostEvent *e, u32 out)
{
    if (got)
        memcpy(MEM(out), e, sizeof *e);     /* five int32s, same layout */
    return (u32)got;
}

u32 HOST(poll_event)(Host *h, u32 out)
{
    HostEvent e;
    (void)h;
    return event_out(host_poll_event(&e), &e, out);
}

u32 HOST(wait_event)(Host *h, u32 out, u32 timeout)
{
    HostEvent e;
    (void)h;
    return event_out(host_wait_event(&e, (int)timeout), &e, out);
}

u64 HOST(time_us)(Host *h) { (void)h; return host_time_us(); }
void HOST(delay_ms)(Host *h, u32 ms) { (void)h; host_delay_ms(ms); }

u32 HOST(cursor_system)(Host *h, u32 kind) { (void)h; return (u32)host_cursor_system((int)kind); }
u32 HOST(cursor_image)(Host *h, u32 argb, u32 w, u32 hh, u32 x, u32 y)
{
    (void)h;
    return (u32)host_cursor_image(MEM(argb), (int)w, (int)hh, (int)x, (int)y);
}
void HOST(cursor_set)(Host *h, u32 c) { (void)h; host_cursor_set((int)c); }
void HOST(cursor_free)(Host *h, u32 c) { (void)h; host_cursor_free((int)c); }
void HOST(cursor_show)(Host *h, u32 show) { (void)h; host_cursor_show((int)show); }

u32 HOST(audio_init)(Host *h) { (void)h; return (u32)host_audio_init(); }
u32 HOST(audio_open)(Host *h, u32 freq, u32 ch, u32 bits) { (void)h; return (u32)host_audio_open((int)freq, (int)ch, (int)bits); }
void HOST(audio_queue)(Host *h, u32 dev, u32 data, u32 len) { (void)h; host_audio_queue((int)dev, MEM(data), len); }
u32 HOST(audio_queued)(Host *h, u32 dev) { (void)h; return host_audio_queued((int)dev); }
void HOST(audio_clear)(Host *h, u32 dev) { (void)h; host_audio_clear((int)dev); }
void HOST(audio_close)(Host *h, u32 dev) { (void)h; host_audio_close((int)dev); }
u32 HOST(music_play)(Host *h, u32 file, u32 len) { (void)h; return (u32)host_music_play(MEM(file), len); }
void HOST(music_stop)(Host *h) { (void)h; host_music_stop(); }
u32 HOST(music_playing)(Host *h) { (void)h; return (u32)host_music_playing(); }

u32 HOST(message_box)(Host *h, u32 icon, u32 title, u32 text, u32 n, u32 labels, u32 ids, u32 enter, u32 escape)
{
    const char *l[8];
    int id[8];
    u32 i;
    (void)h;
    if (n > 8)
        n = 8;
    for (i = 0; i < n; i++) {
        l[i] = MEM(GET32(labels + 4 * i));
        id[i] = (int)GET32(ids + 4 * i);
    }
    return (u32)host_message_box((int)icon, MEM(title), MEM(text), (int)n, l, id, (int)enter, (int)escape);
}

/* Network sockets (host_net.c) */
u32 HOST(net_local_addr)(Host *h, u32 buf, u32 size) { (void)h; return (u32)host_net_local_addr(MEM(buf), (int)size); }
u32 HOST(net_udp_open)(Host *h, u32 port) { (void)h; return (u32)host_net_udp_open((int)port); }
u32 HOST(net_udp_send)(Host *h, u32 s, u32 ip, u32 port, u32 p, u32 n)
{
    (void)h;
    return (u32)host_net_udp_send((int)s, ip, (int)port, MEM(p), (int)n);
}
u32 HOST(net_udp_recv)(Host *h, u32 s, u32 p, u32 n, u32 from)
{
    uint32_t ip = 0;
    int r;
    (void)h;
    r = host_net_udp_recv((int)s, MEM(p), (int)n, &ip);
    if (r >= 0 && from)
        PUT32(from, ip);
    return (u32)r;
}
u32 HOST(net_tcp_open)(Host *h, u32 port) { (void)h; return (u32)host_net_tcp_open((int)port); }
u32 HOST(net_tcp_listen)(Host *h, u32 s, u32 backlog) { (void)h; return (u32)host_net_tcp_listen((int)s, (int)backlog); }
u32 HOST(net_tcp_accept)(Host *h, u32 s, u32 peer)
{
    uint32_t ip = 0;
    int r;
    (void)h;
    r = host_net_tcp_accept((int)s, &ip);
    if (r >= 0 && peer)
        PUT32(peer, ip);
    return (u32)r;
}
u32 HOST(net_tcp_connect)(Host *h, u32 s, u32 ip, u32 port) { (void)h; return (u32)host_net_tcp_connect((int)s, ip, (int)port); }
u32 HOST(net_tcp_state)(Host *h, u32 s) { (void)h; return (u32)host_net_tcp_state((int)s); }
u32 HOST(net_tcp_send)(Host *h, u32 s, u32 p, u32 n) { (void)h; return (u32)host_net_tcp_send((int)s, MEM(p), (int)n); }
u32 HOST(net_tcp_recv)(Host *h, u32 s, u32 p, u32 n) { (void)h; return (u32)host_net_tcp_recv((int)s, MEM(p), (int)n); }
void HOST(net_close)(Host *h, u32 s) { (void)h; host_net_close((int)s); }

int main(int argc, char **argv)
{
    static Host host = {&module};
    static struct w2c_wasi__snapshot__preview1 wasi = {&module};
    wasm_rt_trap_t trap;
    char cwd[4096], game[4096], *args[2];
    /* The game: GameData, set up from the user's copy first if need be,
       or the folder named (setup.h); the module's working directory
       starts at "/", so it is named (main.c's argument). */
    if (!setup_choose_game(argc, argv, cwd, sizeof cwd))
        return 0;
    module_path(cwd, game, sizeof game);
    args[0] = argv[0];
    args[1] = game;
    argc = 2;
    argv = args;
#ifdef _WIN32
    {
        /* directories in the environment (CIV2_DATA ...), as the module sees them */
        static const char *const vars[] = {"CIV2_DATA", "CIV2_WINDIR", "CIV2_SHOT_DIR", "CIV2_SCRIPT",
                                           "CIV2_RAND_LOG", "CIV2_HOOK_LOG", "CIV2_CALL_LOG"};
        size_t i;
        for (i = 0; i < sizeof vars / sizeof vars[0]; i++) {
            const char *v = getenv(vars[i]);
            char p[4096], kv[4200];
            if (v && *v) {
                module_path(v, p, sizeof p);
                snprintf(kv, sizeof kv, "%s=%s", vars[i], p);
                _putenv(kv);
            }
        }
    }
#endif
    wasi_init(argc, argv);
    wasm_rt_init();
    wasm2c_civ2_instantiate(&module, &host, &wasi);
    port_memory = w2c_civ2_memory(&module);
    trap = wasm_rt_impl_try();
    if (trap == WASM_RT_TRAP_NONE)
        w2c_civ2_0x5Fstart(&module);
    else
        fprintf(stderr, "civ2: the game stopped: %s\n", wasm_rt_strerror(trap));
    wasm2c_civ2_free(&module);
    wasm_rt_free();
    return trap == WASM_RT_TRAP_NONE ? 0 : 1;
}
