/* The experimental win64 target calls real Win32 directly and is used to
   check pointer portability. Shipping Windows builds use the portable host
   and the HD implementation in platform/hd.c, like Linux and macOS. */
#ifdef _WIN32
#include "../platform/hd.h"
int hd_enabled = 0, hd_density = 1;
int hd_fade_grey = 0;
int hd_find(const char *) { return 0; }
int hd_variant(int, int) { return 0; }
void hd_size(int, int *w, int *h) { *w = *h = 0; }
const unsigned char *hd_indices(int) { return 0; }
const unsigned char *hd_palette() { return 0; }
int hd_module_image(uintptr_t, int, const char *) { return 0; }
int hd_file(const char *) { return 0; }
int hd_virtual_file(const char *) { return 0; }
void hd_attach(void *, int) {}
int hd_source(void *) { return 0; }
void hd_draw(void *, int, int, int, int, int, int, int, int, int, int) {}
void hd_changed(void *, int, int, int, int) {}
void hd_copy(void *, void *, int, int, int, int, int, int) {}
void hd_commit(void *) {}
void hd_commit_rect(void *, int, int, int, int) {}
void hd_sprite_forget(const void *) {}
void hd_sprite_capture(const void *, void *, int, int, int, int, int) {}
int hd_sprite_draw(const void *, void *, int, int, int, int, int, int, int, int, int) { return 0; }
int hd_sprite_draw_keyed(const void *, void *, int, int, int, int, int, int, int, int, int, int) { return 0; }
void hd_sprite_greyscale(const void *, int) {}
void hd_surface_grey(void *, int) {}
void hd_sprite_tint(const void *, int, int) {}
void hd_sprite_copy(const void *, const void *) {}
void hd_sprite_bind(const void *, const char *) {}
int hd_sprite_image(const void *) { return 0; }
int hd_gdi_bitmap(uintptr_t, uintptr_t, int, int, int, int, int, int) { return 0; }
#endif
