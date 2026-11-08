// SOURCE src/game/map_camera.cpp map_camera_unit_labels
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cctype>
#include "map_camera.h"
using iptr=intptr_t;using uint=unsigned;
enum { UNIT_DIPLOMAT=2,UNITF_AUTOMATED=1,X_WORK_COUNTER=1,ROLE_SETTLE=5 };
static int active=1,map_show_hidden_terrain,human_civ[]={1},work=1,shown=1,picked=1;
static int enabled() { return active; }
static struct { int num_units=2,reveal_map; } game;
static struct { int x=3,y=4,id=1,type,owner=1,flags,counter,prev_in_stack=-1; } units[2];
static struct { int role; } unit_types[3];
static struct { int unit_shield_x_offsets[64],unit_shield_y_offsets[64]; } road_sprites_block;
static struct { int origin_stack_unit=-1,origin_x,origin_y; } combat_anim;
static int tile_lookups;
static int tile_first_unit(int,int) { tile_lookups++;return -1; }
static int map_tile_unit(int,int,int) { return shown; }
static int map_stack_unit(int unit,uint *flags) { *flags=7;return unit; }
static int unit_stack_pick_display_unit(int) { return picked; }
static int gfx_unit_order_char(int) { return 'M'; }
static int gfx_zoom_scale(int n,int zoom) { return n*(zoom+8)/8; }
static int crt_toupper(int c) { return toupper(c); }
static int extra_ui(int) { return work; }
struct MapView {
    int zoom=0,client_x=8,client_y=36,half_tile_height=16;
    int tile_in_view(int,int) { return 1; }
    void tile_to_screen(iptr x,iptr y,int,int) { *(int *)x=client_x+64;*(int *)y=client_y+32; }
};
static struct { MapView *view;MapCameraTransform transform;double scale; } camera;
#include "production.inc"
int main()
{
    MapView view;camera.view=&view;camera.scale=.75;camera.transform={.75,0,0,8,36,320,200};
    units[1].prev_in_stack=0;units[1].flags=UNITF_AUTOMATED;units[1].counter=4;unit_types[0].role=ROLE_SETTLE;
    road_sprites_block.unit_shield_x_offsets[0]=12;road_sprites_block.unit_shield_y_offsets[0]=8;
    MapCameraLabel labels[4]={};double height;
    assert(map_camera_unit_labels(labels,4,&height)==2 && height==8.25);
    assert(tile_lookups==0); // Ordinary frames never search the unit table for each stack.
    assert(labels[0].flags==1 && labels[0].color==0x1d && !strcmp(labels[0].text,"M"));
    assert(labels[0].left==57 && labels[0].right==66 && labels[0].top==22.5 && labels[0].bottom==30);
    assert(labels[1].flags==2 && labels[1].x==72 && labels[1].y==42 && !strcmp(labels[1].text,"4"));
    assert(map_camera_unit_labels(labels,1,&height)==1); // No duplicate stack entries or overflow.
    units[1].owner=2;assert(map_camera_unit_labels(labels,4,&height)==1);
    game.reveal_map=1;assert(map_camera_unit_labels(labels,4,&height)==2);
    shown=-1;assert(!map_camera_unit_labels(labels,4,&height));shown=1;
    picked=-1;assert(!map_camera_unit_labels(labels,4,&height));picked=1;
    combat_anim.origin_stack_unit=1;combat_anim.origin_x=9;combat_anim.origin_y=10;
    assert(map_camera_unit_labels(labels,4,&height)==4 && tile_lookups==1);
    combat_anim.origin_stack_unit=-1;
    for(int z=-7;z<=8;z++) {
        view.zoom=z;camera.scale=.625;
        assert(map_camera_unit_labels(labels,4,&height)==2 && height==6.875);
        camera.scale=.624;assert(!map_camera_unit_labels(labels,4,&height));
    }
    camera.scale=1;map_show_hidden_terrain=1;assert(!map_camera_unit_labels(labels,4,&height));
    map_show_hidden_terrain=0;active=0;assert(!map_camera_unit_labels(labels,4,&height));
    puts("Camera unit labels: stack selection, shield bounds, work privacy, capacity and backing-independent size passed.");
}
