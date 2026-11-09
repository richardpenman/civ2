/* The CPU compositor tests have no SDL context. Any GPU operation is a test
 * failure; capability probing must keep them on the production CPU fallback. */
#include "../src/host/host.h"
#include <assert.h>
int host_gpu_available(void) { return 0; }
#define UNAVAILABLE() do { assert(!"GPU operation in CPU-only test"); return 0; } while (0)
int host_gpu_create(int w,int h,int kind,const void *p,int pitch) { UNAVAILABLE(); }
void host_gpu_destroy(int id) { assert(!"GPU destruction in CPU-only test"); }
int host_gpu_blit(const HostGpuBlit *d) { UNAVAILABLE(); }
int host_gpu_camera(const HostGpuCamera *d) { UNAVAILABLE(); }
int host_gpu_text(const HostGpuText *d) { UNAVAILABLE(); }
int host_gpu_text_batch(const HostGpuText *d,int count) { UNAVAILABLE(); }
int host_gpu_fill(int id,int x,int y,int w,int h,uint32_t c) { UNAVAILABLE(); }
int host_gpu_upload(int id,int x,int y,int w,int h,const void *p,int pitch,int scale) { UNAVAILABLE(); }
int host_gpu_read(int id,int x,int y,int w,int h,void *p,int pitch) { UNAVAILABLE(); }
int host_gpu_present(int id) { UNAVAILABLE(); }
void host_gpu_finish(void) {}
void host_gpu_fence(void) {}
