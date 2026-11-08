// SOURCE src/auto/mapview.cpp MapView::draw_tile_terrain
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
using iptr=intptr_t;using i8=signed char;using byte=unsigned char;
using uint=unsigned;using u32=uint32_t;
enum { TERRAIN_OCEAN=10,RESOURCE=1,MINE=2 };
static int detail,roads,specials,fog,resources,mines,map_show_hidden_terrain;
static bool camera;
static int map_camera_detail_zoom(void *,int zoom) { return camera ? detail : zoom; }
static int map_camera_topology_zoom(void *,int zoom) { return camera ? 0 : zoom; }
struct Sprite {
    int kind;char padding[56];
    void draw(iptr,iptr,int,int) { if(kind==RESOURCE)resources++;if(kind==MINE)mines++; }
};
static Sprite unexplored_tile_sprite;
static struct { Sprite terrain_resource_sprites[22]; } city_down_arrow_sprite_block;
static struct { Sprite mine_sprite; } land_improvement_sprites_block;
static byte tile[]={0,0x18},known=0x18;
static int map_in_bounds(int,int) { return 1; }
static int tile_visible_to(int,int,int) { return 1; }
static byte *tile_at(int,int) { return tile; }
static byte *known_at(int,int,int) { return &known; }
static int tile_resource(int,int) { return 1; }
static int tile_city_owner(int,int) { return -1; }
static int gfx_zoom_scale(int size,int zoom) { return size*(zoom+8)/8; }
static void gfx_set_zoom_scale(int) {}
static void gfx_reset_zoom_scale() {}
struct MapView {
    int zoom_u,tile_width_u,tile_height_u,half_tile_width_u,half_tile_height_u;
    void draw_tile_terrain(iptr,int,int,iptr,int,int,int,iptr);
    void draw_tile_ocean(iptr,int,int,iptr,int,int,u32,u32) {}
    void draw_tile_land(iptr,int,int,iptr,int,int,uint,uint,uint,u32,u32,uint *) {}
    void draw_tile_roads(iptr,int,int,iptr,int,int,uint) { roads++; }
    void draw_tile_specials(iptr,int,int,iptr,int,uint,u32,u32,u32) { specials++; }
    void draw_tile_unexplored_edges(iptr,int,int,iptr,int,int,iptr,u32,u32) { fog++; }
};
#include "production.inc"
int main()
{
    MapView view={0,64,32,32,16};
    city_down_arrow_sprite_block.terrain_resource_sprites[0].kind=RESOURCE;
    land_improvement_sprites_block.mine_sprite.kind=MINE;
    for(int on=0;on<2;on++)for(int band: {0,-4,-5})for(int zoom=-7;zoom<=8;zoom++) {
        camera=on;detail=band;roads=specials=fog=resources=mines=0;
        view.draw_tile_terrain((iptr)&view,0,0,2,2,1,zoom,-1);
        int lod=on ? band : zoom;
        assert(roads==(lod>=-3) && resources==(lod>=-3) && mines==(lod>=-3));
        assert(specials==(lod>=-4));
        assert(fog==(on || zoom>=-3));
    }
    puts("Camera LOD: small improvements/resources simplify; fog boundaries remain; fixed-zoom thresholds unchanged.");
}
