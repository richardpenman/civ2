/* Optional presentation camera. Nothing here is part of a save or game struct. */
#ifndef CIV2_MAP_CAMERA_H
#define CIV2_MAP_CAMERA_H
#include <stdint.h>
typedef struct MapCameraTransform {
    double scale, x, y; /* viewport-local: displayed = source * scale + offset */
    int left, top, width, height;
} MapCameraTransform;
typedef struct MapCameraLabel {
    double x,y; /* viewport-local display coordinates, before density scaling */
    int color;
    char text[160];
    int flags; /* 1: clip to shield, 2: shadow (unit work counter) */
    double left,top,right,bottom;
} MapCameraLabel;
static inline double camera_source_x(const MapCameraTransform *c,double x)
{ return c->left+(x-c->left-c->x)/c->scale; }
static inline double camera_source_y(const MapCameraTransform *c,double y)
{ return c->top+(y-c->top-c->y)/c->scale; }
/* Inverse-map a pointer, rejecting the viewport chrome before transforming it.
 * floor matters at negative offsets and diamond boundaries. */
static inline int camera_pick(const MapCameraTransform *c,int *x,int *y)
{
    double sx,sy;
    if(*x<c->left || *y<c->top || *x>=c->left+c->width || *y>=c->top+c->height)return 0;
    sx=camera_source_x(c,*x);sy=camera_source_y(c,*y);
    if(sx<c->left || sy<c->top || sx>=c->left+c->width || sy>=c->top+c->height)return 0;
    *x=(int)sx;*y=(int)sy;
    if(sx<*x)--*x;if(sy<*y)--*y;return 1;
}
#ifdef __cplusplus
extern "C" {
#endif
void map_camera_layout(void *view);
void map_camera_forget(void *view);
int map_camera_pick(void *view,int *x,int *y);
int map_camera_input(void *message);
int map_camera_zoom_step(void *view,int step); /* 0: normal zoom, otherwise +/-1 */
void map_camera_tick(void);
int map_camera_busy(void);
int map_camera_active(void);
int map_camera_option(void);
void map_camera_set_option(int on);
int map_camera_owns(void *view);
int map_camera_surface(void *surface);
int map_camera_detail_zoom(void *surface,int zoom);
int map_camera_topology_zoom(void *surface,int zoom);
/* sizes selects population badges instead of city names. */
int map_camera_labels(MapCameraLabel *labels,int capacity,double *height,int sizes);
int map_camera_unit_labels(MapCameraLabel *labels,int capacity,double *height);
int extras_camera_messages(MapCameraLabel *labels,int capacity,double *height);
int map_camera_backing_zoom(double scale,int width,int height);
void map_camera_cancel(void);
void map_camera_probe(int x,int y);
void map_camera_boundary_test(int flat);
/* Native window containing the map; the transform is in its client coordinates. */
uintptr_t map_camera_get(MapCameraTransform *out);
/* Unoccluded indexed/HD backing; child windows share the desktop surface. */
void *map_camera_pixels(void);
/* Persistent rendered viewport regions; owned outside serialized MapView. */
int map_view_cache_draw(void *view,int civ);
void map_view_cache_invalidate(void *view,int x,int y,int radius);
void map_view_cache_idle(void *view,int zoom,int zooming);
void map_view_cache_clear(void);
int map_view_cache_rendering(void);
int map_view_cache_surface(void *surface);
void map_view_cache_probe(void);
#ifdef __cplusplus
}
#endif
#endif
