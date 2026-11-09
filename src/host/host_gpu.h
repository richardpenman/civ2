#ifndef CIV2_HOST_GPU_H
#define CIV2_HOST_GPU_H
#include <SDL.h>
void gpu_renderer_init(SDL_Renderer *renderer);
void gpu_renderer_destroy(void);
void gpu_window_target(void);
void gpu_renderer_lost(void);
typedef struct GpuAtlasStats {
    int enabled,page_size,page_limit,pages,entries;
    unsigned batches,uploads;
} GpuAtlasStats;
/* Native diagnostics; does not flush pending rendering. */
void gpu_atlas_stats(GpuAtlasStats *out);
#endif
