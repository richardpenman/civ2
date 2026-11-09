// SOURCE src/auto/graphics.cpp gfx_city_sprite_index
#include <cassert>
#include <cstdio>
using uint=unsigned;using byte=unsigned char;
enum { IMP_CITY_WALLS=1,IMP_PALACE=2,WONDER_GREAT_WALL=3,ADV_INDUSTRIALIZATION=4,
       ADV_AUTOMOBILE=5,ADV_ELECTRONICS=6,GAME_SCENARIO=1,DIFF_KING=3 };
static int human_civ[]={1},wall,palace,wonder,industrial,auto_advance,electronics;
static struct { int owner=1,size=12;char revealed_size[8]={0,3}; } cities[1];
static struct { int tribe; } civs[8];
static struct { int city_style=2; } leaders[8];
static struct { int city_style=1; } civ_names[8];
static struct { int reveal_map,game_flags,human_players=2,difficulty=DIFF_KING; } game;
static struct { int flags[1]; } scenario;
static int city_has_improvement(int,int improvement) { return improvement==IMP_PALACE ? palace : wall; }
static int civ_has_wonder(int,int) { return wonder; }
static int civ_has_advance(int,int advance) { return advance==ADV_INDUSTRIALIZATION ? industrial : advance==ADV_AUTOMOBILE ? auto_advance : electronics; }
#include "production.inc"
int main()
{
    int size;
    assert(gfx_city_sprite_index(0,0,&size)==14 && size==12);
    cities[0].owner=2;
    assert(gfx_city_sprite_index(0,0,&size)==16 && size==3); // Hidden enemy growth stays hidden.
    assert(gfx_city_sprite_index(0,0x1000,&size)==22 && size==12);
    game.reveal_map=1;assert(gfx_city_sprite_index(0,0,&size)==22 && size==12);
    game.reveal_map=0;game.game_flags=GAME_SCENARIO;scenario.flags[0]=8;
    assert(gfx_city_sprite_index(0,0,&size)==22 && size==12);
    game.game_flags=0;palace=1;
    assert(gfx_city_sprite_index(0,0,&size)==16);
    game.difficulty=DIFF_KING-1;assert(gfx_city_sprite_index(0,0,&size)==18);
    wall=1;assert(gfx_city_sprite_index(0,0,&size)==19);
    wall=0;wonder=1;assert(gfx_city_sprite_index(0,0,&size)==19);
    industrial=1;assert(gfx_city_sprite_index(0,0,&size)==35);
    auto_advance=1;electronics=1;assert(gfx_city_sprite_index(0,0,&size)==43);
    cities[0].owner=1;cities[0].size=19;
    assert(gfx_city_sprite_index(0,0,&size)==47 && size==19); // Palace cannot overflow the last sprite.
    puts("City appearance: revealed size, civilization style, eras, walls and palace selection passed.");
}
