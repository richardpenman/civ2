// SOURCE src/game/map_camera.cpp map_camera_labels
// SOURCE src/game/extras.cpp extras_camera_messages
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "map_camera.h"
using iptr=intptr_t;
static int active=1,map_show_hidden_terrain,human_civ[]={1};
static int enabled() { return active; }
static double clamp(double v,double lo,double hi) { return v<lo ? lo : v>hi ? hi : v; }
static struct { int num_cities=5,reveal_map,game_flags; } game;
struct City { int id,x,y,owner,known_to,flags;int revealed_size[8];char name[32]; };
static City cities[5];
static int tile_visible_to(int x,int,int) { return x!=3; }
static int civ_color_secondary(int owner) { return owner+10; }
enum { GAME_SCENARIO=1,CITYF_DISORDER=1 };
static struct { int flags[1]; } scenario;
static struct { int city_size_box_x_offsets[1]={3},city_label_y_offsets[1]={5}; } road_sprites_block;
static int civ_color_primary(int owner) { return owner+20; }
static int gfx_city_sprite_index(int city,unsigned,int *size) { *size=city+7;return 0; }
enum { X_SUPPRESS_POPUPS=1 };
static int popups=1;
static int extra_ui(int) { return popups; }
static struct { int line_spacing() { return 17; } } advisor_bold_font;
static struct { char text[160]; } messages[4];
struct MapView {
    int client_x=8,client_y=36,half_tile_width=8,tile_height=8,half_tile_height=4;
    int tile_in_view(int,int y) { return y!=99; }
    void tile_to_screen(iptr x,iptr y,int tx,int ty) {
        *(int *)x=client_x+tx*8;*(int *)y=client_y+ty*4;
    }
};
static struct { MapView *view;MapCameraTransform transform;double scale; } camera;
#include "production.inc"
int main()
{
    MapView view;camera.view=&view;camera.scale=.375;
    camera.transform={1.5,-4.25,-2.75,8,36,320,200};
    for(int i=0;i<5;i++) {
        cities[i].id=1;cities[i].x=i;cities[i].y=2;cities[i].owner=i ? 2 : 1;
        cities[i].known_to=2;cities[i].revealed_size[1]=3;
        snprintf(cities[i].name,32,"City %d",i);
    }
    cities[1].known_to=0;cities[2].revealed_size[1]=0;cities[4].y=99;
    MapCameraLabel labels[6]={};double height=0;
    assert(map_camera_labels(labels,5,&height,0)==1);
    assert(fabs(height-7.2)<1e-9 && labels[0].color==11);
    assert(labels[0].x==7.75 && labels[0].y==18.25);
    assert(!strcmp(labels[0].text,"City 0"));
    game.reveal_map=1;assert(map_camera_labels(labels,5,&height,0)==4);
    labels[1].color=99;assert(map_camera_labels(labels,1,&height,0)==1 && labels[1].color==99);
    camera.scale=.125;map_camera_labels(labels,5,&height,0);assert(height==6);
    camera.scale=2;map_camera_labels(labels,5,&height,0);assert(fabs(height-38.4)<1e-9);
    map_show_hidden_terrain=1;assert(!map_camera_labels(labels,5,&height,0));
    map_show_hidden_terrain=0;active=0;assert(!map_camera_labels(labels,5,&height,0));
    active=1;camera.scale=.625;
    assert(map_camera_labels(labels,5,&height,1)==4 && height==6.875);
    assert(!strcmp(labels[0].text,"7") && labels[0].color==21);
    assert(labels[0].x==-2.375 && labels[0].y==6.375);
    camera.scale=.624;assert(!map_camera_labels(labels,5,&height,1));
    camera.scale=.75;cities[0].flags=CITYF_DISORDER;
    assert(map_camera_labels(labels,5,&height,1)==3);
    game.reveal_map=0;assert(map_camera_labels(labels,5,&height,1)==0);
    game.game_flags=GAME_SCENARIO;scenario.flags[0]=8;
    assert(map_camera_labels(labels,5,&height,1)==3); // Scenario size visibility.
    strcpy(messages[0].text,"An optional map message longer than a city name.");
    strcpy(messages[2].text,"Second message");
    assert(extras_camera_messages(labels,5,&height)==2 && height==14);
    assert(labels[0].x==6 && labels[0].y==4 && !strcmp(labels[0].text,messages[0].text));
    assert(labels[1].y==21 && extras_camera_messages(labels,1,&height)==1);
    popups=0;assert(!extras_camera_messages(labels,5,&height));
    puts("Camera labels: display-space placement, minimum size, visibility and capacity passed.");
}
