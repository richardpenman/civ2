// SOURCE src/platform/main.c plat_present
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
static uint64_t clock_us=100000,last_present,present_cost,tick_cost,compose_cost=5000;
static int window_open=1,benchmark_offscreen,hd_redraw_active,busy,active=1;
static int plat_overlay,hd_gpu_active=1,hd_enabled,hd_density=1,screen_w=1024,presents;
static struct { int hd_gpu=1;void *hd,*bits;int stride; } buffer,*screen=&buffer;
static uint64_t host_time_us() { return clock_us; }
static int map_camera_busy() { return busy; }
static int map_camera_active() { return active; }
static void map_camera_tick() { clock_us+=tick_cost;tick_cost=0; }
static void plat_compose() { clock_us+=compose_cost; }
static int host_gpu_present(int) { presents++;return 1; }
static void host_present_hd(void *,int,int) { presents++; }
static void host_present(void *,int) { presents++; }
static void plat_fatal(const char *) { abort(); }
#include "production.inc"
int main()
{
    tick_cost=80000;plat_present();
    assert(presents==1 && present_cost==5000);
    plat_present();assert(presents==2); // Slow refresh must not impose a cooldown.
    clock_us=last_present+14999;plat_present();assert(presents==2);
    clock_us=last_present+15000;plat_present();assert(presents==3);
    present_cost=30000;clock_us=last_present+15000;
    plat_present();assert(presents==4); // Camera remains responsive after a slow frame.
    active=0;present_cost=30000;clock_us=last_present+15000;
    plat_present();assert(presents==4); // Legacy software throttle retained.
    clock_us=last_present+60000;plat_present();assert(presents==5);
    active=1;clock_us+=100000;busy=1;plat_present();assert(presents==5);
    busy=0;hd_redraw_active=1;plat_present();assert(presents==5);
    hd_redraw_active=0;plat_present();assert(presents==6);
    puts("Camera pacing: refresh excluded from frame cost, 15 ms cadence and redraw guards passed.");
}
