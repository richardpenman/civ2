// SOURCE src/platform/main.c hd_redraw_begin
// SOURCE src/platform/main.c hd_redraw_end
// SOURCE src/platform/main.c hd_redraw_poll_due
#include <cassert>
#include <cstdint>
#include <cstdio>
static int hd_redraw_active,plat_replay,benchmark_offscreen,presents;
static uint64_t redraw_next_poll,clock_us;
static uint64_t host_time_us() { return clock_us; }
static void plat_present_now() { presents++; }
#include "production.inc"
int main()
{
    assert(hd_redraw_poll_due());
    clock_us=5000;hd_redraw_begin();
    assert(!hd_redraw_poll_due());
    clock_us=6999;assert(!hd_redraw_poll_due());
    clock_us=7000;assert(hd_redraw_poll_due());
    assert(!hd_redraw_poll_due());
    hd_redraw_begin();clock_us=8999;assert(!hd_redraw_poll_due());
    hd_redraw_end();assert(!presents && hd_redraw_active==1);
    clock_us=9000;assert(hd_redraw_poll_due());
    plat_replay=1;assert(hd_redraw_poll_due());
    benchmark_offscreen=1;assert(!hd_redraw_poll_due());
    clock_us=11000;assert(hd_redraw_poll_due());
    hd_redraw_end();assert(presents==1 && !hd_redraw_active);
    hd_redraw_end();assert(presents==1);
    assert(hd_redraw_poll_due());
    puts("HD redraw scheduling: bounded event polls, nested presentation and replay timing passed");
}
