// SOURCE src/auto/mapview.cpp map_stack_unit
// SOURCE src/auto/mapview.cpp map_tile_unit
// SOURCE src/auto/graphics.cpp gfx_unit_order_char
#include <cassert>
#include <cstdint>
#include <cstdio>
using iptr=intptr_t;using uptr=uintptr_t;using uint=unsigned;
using i8=signed char;using u8=unsigned char;using byte=unsigned char;
enum { ORDER_AI_IDLE=-1,ORDER_FORTIFYING=1,ORDER_FORTIFIED=2,UNITF_AUTOMATED=1 };
static struct { int active_unit,human_players=2,reveal_map; } game;
static struct { int defender=-1,origin_stack_unit=-1,defender_x,defender_y,origin_x,origin_y; } combat_anim;
static struct { int x,y,owner=1,visibility=2,orders,flags,ai_task; } units[4];
static struct { int key; } order_names[16];
static struct { int unit_active,unit_phase; } blink;
static i8 human_civ[]={1};
static int unit_move_anim_active;
static int tile_unit_owner(int,int) { return 1; }
static int moving_units_mode,visible=1,first=0,city_owner=-1,selectable=1;
static int map_in_bounds(int x,int y) { return x>=0 && y>=0; }
static int tile_first_unit(int,int) { return first; }
static int tile_city_owner(int,int) { return city_owner; }
static int tile_visible_to(int,int,int) { return visible; }
static int unit_next_in_stack(int unit) { return unit<1 ? unit+1 : -1; }
static int unit_is_selectable(int) { return selectable; }
#include "production.inc"
int main()
{
    uint flags;units[0].x=units[1].x=3;units[0].y=units[1].y=4;
    game.active_unit=1;
    assert(map_stack_unit(map_tile_unit(3,4,-2),&flags)==1 && flags==5);
    moving_units_mode=1;blink.unit_active=blink.unit_phase=1;
    assert(map_stack_unit(0,&flags)==-1);
    blink.unit_phase=0;assert(map_stack_unit(0,&flags)==1);
    selectable=0;assert(map_stack_unit(0,&flags)==0 && flags==7);
    visible=0;assert(map_tile_unit(3,4,-2)==-1);
    game.reveal_map=1;assert(map_tile_unit(3,4,-2)==0);
    game.reveal_map=0;visible=1;units[0].owner=2;units[0].visibility=0;
    assert(map_tile_unit(3,4,-2)==-1);units[0].visibility=2;
    assert(map_tile_unit(3,4,-2)==0);
    city_owner=1;assert(map_tile_unit(3,4,-2)==-1);selectable=1;
    assert(map_tile_unit(3,4,-2)==1);
    units[1].orders=ORDER_FORTIFIED;assert(map_tile_unit(3,4,-2)==-1);
    units[1].orders=0;blink.unit_active=0;assert(map_tile_unit(3,4,-2)==-1);
    city_owner=-1;combat_anim.defender=0;combat_anim.defender_x=3;combat_anim.defender_y=4;
    assert(map_tile_unit(3,4,-2)==0 && map_stack_unit(0,&flags)==0 && flags==5);
    blink.unit_active=blink.unit_phase=1;assert(map_stack_unit(0,&flags)==-1);
    combat_anim.defender=-1;combat_anim.origin_stack_unit=1;combat_anim.origin_x=3;combat_anim.origin_y=4;
    assert(map_tile_unit(3,4,-2)==1 && map_stack_unit(1,&flags)==1 && flags==5);
    first=-1;assert(map_tile_unit(3,4,-2)==1); // Retained combat origin, even with no unit on its tile.
    combat_anim.origin_stack_unit=-1;
    assert(map_tile_unit(3,4,-2)==-1 && map_stack_unit(-1,&flags)==-1);
    units[0].owner=1;city_owner=-1;
    assert(map_tile_unit(3,4,0)==0); // The supplied stack head avoids the ordinary tile lookup.
    units[0].owner=2;
    assert(map_tile_unit(-1,4,-2)==-1);
    order_names[2].key='F';order_names[11].key='G';
    units[0].orders=2;assert(gfx_unit_order_char(0)=='F');
    units[0].orders=ORDER_AI_IDLE;assert(gfx_unit_order_char(0)=='-');
    units[0].flags=UNITF_AUTOMATED;assert(gfx_unit_order_char(0)=='*');
    units[0].orders=11;units[0].ai_task='A';assert(gfx_unit_order_char(0)=='-');
    game.reveal_map=1;assert(gfx_unit_order_char(0)=='A');
    units[0].owner=1;assert(gfx_unit_order_char(0)=='G');
    puts("Shared map status selection: fog, cities, stacks, blinking, combat and order privacy passed.");
}
