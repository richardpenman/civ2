// SOURCE src/game/map_camera.cpp map_camera_surface
// SOURCE src/game/map_camera.cpp map_camera_detail_zoom
// SOURCE src/game/map_camera.cpp map_camera_topology_zoom
// SOURCE src/auto/graphics.cpp gfx_show_status_text
// SOURCE src/auto/mapview.cpp MapView::draw_tile_ocean
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>
using iptr=intptr_t;using i8=signed char;using byte=unsigned char;
using u32=uint32_t;using uint=unsigned;
static bool active;
static int hd_enabled=1;
static int enabled() { return active; }
static int map_view_cache_surface(void *) { return 0; }
static struct { void *view; double scale=1; } camera;
struct Draw { void *sprite; int x,y; };
static std::vector<Draw> draws;
struct Sprite {
    char storage[60];
    void draw(iptr,iptr,int x,int y) { draws.push_back({this,x,y}); }
};
static struct { Sprite terrain_ocean_sprite; } terrain_base_sprites_block;
static struct { Sprite coast_quarter_sprites[16][4]; } tutorial_icon_sprite_block;
static struct { byte coast_quarter_land_masks[4]; } scenario_title_surface_block;
static struct { Sprite river_mouth_sprites[4]; } tile_pick_mask_sprite_block;
static struct { i8 diag_dx_table[4],diag_dy_table[4]; } diag_neighbor_bits_block;
static int coast_quarter_dx_table[]={0,1,0,-1},coast_quarter_dy_table[]={0,1,2,1};
static byte neighbor;
static int tile_calc_coast_masks(iptr,int) { return 0; }
static int map_wrap_x(int x) { return x; }
static int map_in_bounds(iptr,iptr) { return 1; }
static byte *tile_at(iptr,iptr) { return &neighbor; }
struct MapView { void draw_tile_ocean(iptr,int,int,iptr,int,int,u32,u32); };
#include "production.inc"
int main()
{
    MapView view,other;camera.view=&view;
    for(int on=0;on<2;on++)for(int target=0;target<2;target++)
    for(int zoom=-7;zoom<=8;zoom++)for(int rivers=0;rivers<2;rivers++) {
        active=on;neighbor=rivers ? 0x80 : 0;
        void *surface=target ? &view : &other;
        int half_w=4*(zoom+8),half_h=2*(zoom+8);
        draws.clear();view.draw_tile_ocean((iptr)surface,100,200,10,10,zoom,half_w,half_h);
        if(!(on && target) && zoom<-3) {
            assert(draws.size()==1 && draws[0].sprite==&terrain_base_sprites_block.terrain_ocean_sprite);
        } else {
            assert(draws.size()==(rivers ? 8u : 4u));
            for(int q=0;q<4;q++) {
                assert(draws[q].sprite==&tutorial_icon_sprite_block.coast_quarter_sprites[0][q]);
                assert(draws[q].x==100+(coast_quarter_dx_table[q]+1)*(half_w/2));
                assert(draws[q].y==200+coast_quarter_dy_table[q]*(half_h/2));
            }
        }
    }
    active=1;
    for(int zoom=-7;zoom<=8;zoom++) {
        camera.scale=.625;assert(map_camera_detail_zoom(&view,zoom)==0);
        assert(gfx_show_status_text((iptr)&view,zoom));
        camera.scale=.624;assert(map_camera_detail_zoom(&view,zoom)==-4);
        assert(!gfx_show_status_text((iptr)&view,zoom));
        camera.scale=.5;assert(map_camera_detail_zoom(&view,zoom)==-4);
        camera.scale=.499;assert(map_camera_detail_zoom(&view,zoom)==-5);
        assert(map_camera_detail_zoom(&other,zoom)==zoom);
    }
    active=0;
    for(int hd=0;hd<2;hd++)for(int zoom=-7;zoom<=8;zoom++) {
        hd_enabled=hd;
        assert(map_camera_detail_zoom(&view,zoom)==zoom);
        assert(gfx_show_status_text((iptr)&view,zoom)==(!hd || zoom>=-3));
    }
    puts("Camera geography/LOD retained; HD status-text cutoffs match CPU fixed zooms; original graphics unchanged.");
}
