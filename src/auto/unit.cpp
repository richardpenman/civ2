/* The game, units: movement, orders, combat, pathfinding, diplomats and spies: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004131c0 auto */
void __cdecl unit_blink_timer_tick(void)

{
  if (2 < game.multiplayer) {
    net.process_messages(1,1);
  }
  if (((blink.unit_skip_tick == '\0') && (player_turn_active != '\0')) && (turn_loop.active != '\0')) {
    if (blink.unit_active != '\0') {
      blink.unit_phase = blink.unit_phase == '\0';
      mapviews_redraw_cursor();
    }
  }
  else {
    blink.unit_skip_tick = '\0';
  }
  return;
}

/* 0042738c auto */
void __cdecl unit_cancel_goto_on_contact(iptr unit)

{
  if (extra_rule(X_GOTO_ZOC) && ((1 << (units[unit].owner & 0x1f) & (uint)game.human_players) != 0)) {
    return;  /* X18: a human player's Go To goes on next to foreign units, as a caravan's */
  }
  if (((units[unit].orders & 0xfU) == 0xb) && (unit_types[units[unit].type].role != ROLE_TRADE)) {
    units[unit].orders = ORDER_NONE;
  }
  return;
}

/* 004273e6 auto */
void __cdecl unit_stack_wake_sentries(iptr unit)

{
  int is_ocean;
  
  for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit)) {
    if ((units[unit].orders == ORDER_SLEEP) &&
       ((unit_types[units[unit].type].domain != DOMAIN_LAND ||
        (is_ocean = tile_is_ocean((int)units[unit].x,(int)units[unit].y), is_ocean == 0)))) {
      units[unit].orders = ORDER_NONE;
    }
  }
  return;
}

/* Part of unit_reveal_surroundings (004274a6): the tiles around the unit (two away for
   a unit that sees two) become known and visible to its civ; redraw_flags
   notes what the human player had not seen. */
static void unit_reveal_surroundings_tiles(int x2,int y,uptr civ2,uint civ_bit,uint sees_two,
                                                      u8 unit_domain,bool is_human,bool map_revealed,
                                                      uint &redraw_flags)
{
  iptr x;
  iptr y4;
  iptr in_bounds;
  uint is_ocean;
  iptr tile2;
  char *known;
  iptr dir;
  for (dir = 0; dir < 0x19; dir++) {
    x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[dir] + x2);
    y4 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[dir] + y;
    in_bounds = map_in_bounds(x,y4);
    if ((in_bounds != 0) &&
       (((dir < 8 || (dir == 0x14)) ||
        ((sees_two != 0 &&
         ((unit_domain == 1 || (is_ocean = tile_is_ocean(x,y4), is_ocean == (unit_domain == 2))))
         ))))) {
      tile2 = (iptr)tile_at(x,y4);
      if (civ2 != 0) {
        known = (char *)known_at(x,y4,civ2);
        if (((is_human) && (!map_revealed)) &&
           ((*(char *)(tile2 + 1) != *known || ((civ_bit & *(byte *)(tile2 + 4)) == 0)))) {
          if (dir < 8) {
            redraw_flags = redraw_flags | 5;
          }
          else {
            redraw_flags = redraw_flags | 10;
          }
        }
        known_set(x,y4,CONCAT31((int3)((uint)tile2 >> 8),*(i8 *)(tile2 + 1)),
                  civ2,0,1);
      }
      tile_set_visible(x,y4,civ_bit,1,1);
    }
  }
}

/* Part of unit_reveal_surroundings (004274a6): the adjacent cities and units: revealed,
   seeing the unit, contact made, AI goals, gotos cancelled. */
static void unit_reveal_surroundings_adjacent(iptr unit,int allow_contact,int x2,int y,uptr civ2,
                                                         byte civ3,uint civ_bit,uint is_stealth,
                                                         u8 unit_domain,int y3,bool is_human,
                                                         bool map_revealed,u8 reveal_all,
                                                         uint &redraw_flags,bool &redrawn)
{
  byte unit_type2;
  byte unit_type;
  iptr x;
  int y2;
  int city;
  uptr civ;
  int other_ocean;
  iptr unit2;
  int tmp;
  iptr dir;
  int goal_priority;
  for (dir = 0; dir < 8; dir++) {
    x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    city = map_in_bounds(x,y2);
    if (city != 0) {
      city = city_at(x,y2);
      if ((-1 < city) && (civ = (uptr)cities[city].owner, civ != civ2)) {
        if ((is_human) && (!map_revealed)) {
          if ((civ_bit & (int)(char)cities[city].known_to) == 0) {
            redraw_flags = redraw_flags | 5;
          }
          else if (*(char *)&cities[city].revealed_size[civ2] != cities[city].size) {
            redraw_flags = redraw_flags | 1;
          }
        }
        city_reveal_to_civ(city,civ2);
        if (is_stealth == 0) {
          unit_stack_set_seen_by(unit,civ);
        }
        if ((unit_domain == '\0') && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0)) {
          civ_add_ai_goal(civ,x,y2,ROLE_ATTACK,4);
          civ_add_ai_goal(civ,x,y2,ROLE_DEFEND,2);
        }
      }
      other_ocean = tile_is_ocean(x,y2);
      if ((is_stealth == 0) || (other_ocean != 0)) {
        unit2 = tile_first_unit(x,y2);
        if (unit2 < 0) {
          city = tile_is_ocean(x2,y);
          if ((((city == 0) && (civ = tile_city_owner(x,y2), -1 < (int)civ)) &&
              (civ != civ2)) && ((*(byte *)&civs[civ2].treaties[civ] & TREATY_CONTACT) == 0))
          {
            if (!redrawn) {
              map_redraw_tile_all_views(x2,y);
              if (2 < game.multiplayer) {
                net_send_state_diff(0xff,2,0,0,0);
                net_send_message(NM_DRAW_SQUARE,0xff,x2,y,0,0,0,0,0,0);
              }
              redrawn = true;
            }
            if (allow_contact != 0) {
              diplo_civs_meet(civ2,civ,x,y2);
            }
          }
        }
        else {
          unit_type2 = units[unit2].owner;
          civ = (uptr)(char)unit_type2;
          if (civ != civ2) {
            unit_type = units[unit2].type;
            if ((is_stealth == 0) || ((unit_types[unit_type].flags & UTYPE_SPOT_SUBMARINES) != 0)) {
              tmp = tile_city_owner(x2,y);
              if (tmp < 0) {
                unit_stack_set_seen_by(unit,civ);
              }
              if ((*(byte *)&civs[civ2].treaties[civ] & TREATY_ALLIANCE) == 0) {
                unit_stack_wake_sentries(unit2);
              }
              if ((1 << (unit_type2 & 0x1f) & (uint)game.human_players) == 0) {
                if ((other_ocean != 0) || (y3 == 0)) {
                  unit_cancel_goto_on_contact(unit2);
                }
              }
              else if (other_ocean == y3) {
                unit_cancel_goto_on_contact(unit2);
              }
            }
            if ((((unit_types[unit_type].flags & UTYPE_SUBMARINE) == 0) ||
                (tmp = unit_stack_sum(unit2,2), 1 < tmp)) ||
               ((unit_types[units[unit].type].flags & UTYPE_SPOT_SUBMARINES) != 0)) {
              if (city < 0) {
                if (((is_human) && (reveal_all == '\0')) && ((civ_bit & units[unit2].visibility) == 0)) {
                  redraw_flags = redraw_flags | 1;
                }
                unit_stack_set_seen_by(unit2,civ2);
              }
              if ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) {
                if (((y3 != 0) || (other_ocean == 0)) || (unit_domain == 1)) {
                  unit_cancel_goto_on_contact(unit);
                }
              }
              else if ((other_ocean == y3) || (unit_domain == 1)) {
                unit_cancel_goto_on_contact(unit);
              }
              if (((other_ocean != 0) && (civ != 0)) &&
                 ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) == 0)) {
                civ_add_ai_goal(civ2,x,y2,ROLE_NAVAL_SUPERIORITY,
                                   3 - (uint)((unit_types[unit_type].flags & UTYPE_SUBMARINE) == 0));
              }
            }
            if (((unit_types[units[unit].type].domain == DOMAIN_LAND) && (y3 == 0)) &&
               ((other_ocean == 0 && ((unit_types[unit_type].domain != DOMAIN_AIR || (-1 < city)))))) {
              if ((!redrawn) && (reveal_all == '\0')) {
                map_redraw_tile_all_views(x2,y);
                if (2 < game.multiplayer) {
                  net_send_state_diff(0xff,2,0,0,0);
                  net_send_message(NM_DRAW_SQUARE,0xff,x2,y,0,0,0,0,0,0);
                }
                redrawn = true;
              }
              if ((allow_contact != 0) && (units[unit].type != UNIT_PARTISANS)) {
                diplo_civs_meet(civ2,civ,x,y2);
              }
              if ((char)unit_types[units[unit].type].role < 6) {
                civ_add_ai_goal(civ,x,y2,ROLE_DEFEND,2 - (uint)(civ2 == 0));
              }
              if ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) == 0) {
                goal_priority = 1;
                if (((civ != 0) && (unit_types[unit_type].role != ROLE_DIPLOMACY)) &&
                   ((units[unit2].orders != ORDER_FORTIFIED || (unit_types[unit_type].role != ROLE_DEFEND)))) {
                  goal_priority = 3;
                }
                civ_add_ai_goal(civ2,x,y2,ROLE_ATTACK,goal_priority);
              }
            }
          }
        }
      }
    }
  }
}

/* Part of unit_reveal_surroundings (004274a6): the cities and units two tiles away, for a
   unit that sees two (and units that see two seeing it). */
static void unit_reveal_surroundings_far(iptr unit,int x2,int y,uptr civ2,uint civ_bit,
                                                    uint sees_two,uint is_stealth,u8 unit_domain,
                                                    bool is_human,bool map_revealed,u8 reveal_all,
                                                    uint &redraw_flags)
{
  u8 other_domain;
  byte unit_type2;
  uptr civ;
  int y3;
  iptr x;
  uint is_ocean;
  int y2;
  int city;
  int other_ocean;
  iptr dir;
  for (dir = 8; dir < 0x19; dir++) {
    if (dir != 0x14) {
      x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[dir] + x2);
      y3 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[dir] + y;
      y2 = map_in_bounds(x,y3);
      if (y2 != 0) {
        y2 = tile_first_unit(x,y3);
        civ = tile_city_or_unit_owner(x,y3);
        city = city_at(x,y3);
        if (civ != civ2) {
          if ((sees_two != 0) &&
             ((unit_domain == 1 ||
              (is_ocean = tile_is_ocean(x,y3), is_ocean == (unit_domain == 2))))) {
            if (-1 < city) {
              if ((is_human) && (!map_revealed)) {
                if ((civ_bit & (int)(char)cities[city].known_to) == 0) {
                  redraw_flags = redraw_flags | 10;
                }
                else if (*(char *)&cities[city].revealed_size[civ2] != cities[city].size) {
                  redraw_flags = redraw_flags | 2;
                }
              }
              city_reveal_to_civ(city,civ2);
            }
            if ((-1 < y2) && ((unit_types[units[y2].type].flags & UTYPE_SUBMARINE) == 0)) {
              if (city < 0) {
                if (((is_human) && (reveal_all == '\0')) && ((civ_bit & units[unit].visibility) == 0)) {
                  redraw_flags = redraw_flags | 2;
                }
                unit_stack_set_seen_by(y2,civ2);
              }
              other_ocean = tile_is_ocean(x,y3);
              if (((other_ocean != 0) && (civ != 0)) &&
                 ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) == 0)) {
                civ_add_ai_goal(civ2,x,y3,ROLE_NAVAL_SUPERIORITY,2);
              }
            }
          }
          if (((-1 < y2) && (is_stealth == 0)) &&
             ((unit_type2 = units[y2].type, (unit_types[unit_type2].flags & UTYPE_SEE_TWO_SPACES) != 0 &&
              ((unit_types[unit_type2].domain == DOMAIN_AIR ||
               (other_domain = unit_types[unit_type2].domain, is_ocean = tile_is_ocean(x2,y),
               (other_domain == DOMAIN_SEA) == is_ocean)))))) {
            unit_cancel_goto_on_contact(y2);
            unit_stack_wake_sentries(y2);
            y2 = tile_city_owner(x2,y);
            if (y2 < 0) {
              unit_stack_set_seen_by(unit,civ);
            }
            y3 = tile_is_ocean(x,y3);
            if (((y3 == 0) && (civ != 0)) &&
               ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0)) {
              civ_add_ai_goal(civ,x2,y,ROLE_NAVAL_SUPERIORITY,2);
            }
          }
          if (((-1 < city) && (y3 = tile_visible_to(x2,y,civ), y3 != 0)) &&
             ((unit_types[units[unit].type].attack != '\0' && (is_stealth == 0)))) {
            unit_stack_set_seen_by(unit,civ);
          }
        }
      }
    }
  }
}

/* Part of unit_reveal_surroundings (004274a6): the map redrawn for those who see the
   unit (as much of it as redraw_flags asks for). */
static void unit_reveal_surroundings_redraw(iptr unit,int x2,int y,uptr civ2,byte civ3,
                                                       uint sees_two,uint redraw_flags,bool redrawn)
{
  iptr redraw_radius;
  uint player;
  if (game.turn == 0) {
    net_batch_flush();
  }
  else {
    net_send_state_diff(0xff,2,0,0,0);
    if (((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) && (human_civ[0] != civ2)) {
      for (player = 1; (int)player < 8; player++) {
        if (((1 << ((byte)player & 0x1f) & (uint)game.human_players) != 0) &&
           (((game.reveal_map != '\0' ||
             ((1 << ((byte)player & 0x1f) & (uint)units[unit].visibility) != 0)) ||
            ((int)units[unit].owner == (player & 0xff))))) {
          if (human_civ[0] == player) {
            map_redraw_neighbors_all_views(x2,y);
          }
          else if (2 < game.multiplayer) {
            net_send_message(NM_DRAW_AREA,net.net_slots[net.net_civ_slots[player]].id
                               ,x2,y,0,0,0,0,0,0);
          }
        }
      }
      if ((!redrawn) && (map_redraw_tile_all_views(x2,y), 2 < game.multiplayer)) {
        net_send_message(NM_DRAW_SQUARE,0xff,x2,y,0,0,0,0,0,0);
      }
    }
    else {
      redraw_radius = 2;
      if ((sees_two == 0) || ((redraw_flags & 2) == 0)) {
        redraw_radius = 1;
        if ((redraw_flags & 4) != 0) {
          redraw_radius = 2;
        }
      }
      else if ((redraw_flags & 8) != 0) {
        redraw_radius = 3;
      }
      if ((redraw_flags == 0) && (redraw_radius = 0, redrawn)) {
        net_batch_flush();
        return;
      }
      if (human_civ[0] == civ2) {
        map_redraw_area_all_views(x2,y,redraw_radius,civ2,1);
      }
      else if (2 < game.multiplayer) {
        net_send_message(NM_DRAW_MAP_REGION,net.net_slots[net.net_civ_slots[civ2]].id,x2
                           ,y,redraw_radius,civ2,0,0,0,0);
      }
    }
    net_batch_flush();
  }
}

/* 004274a6 auto */
void __cdecl unit_reveal_surroundings(iptr unit,int allow_contact)

{
  byte civ3;
  u8 unit_domain;
  bool map_revealed;
  bool redrawn;
  u8 reveal_all;
  int x2;
  int y;
  uptr civ2;
  uint sees_two;
  uint is_stealth;
  int y3;
  Tile *tile;
  uint civ_bit;
  bool is_human;
  uint redraw_flags;
  
  redrawn = false;
  x2 = (int)units[unit].x;
  y = (int)units[unit].y;
  civ3 = units[unit].owner;
  civ2 = (uptr)(char)civ3;
  sees_two = unit_types[units[unit].type].flags & UTYPE_SEE_TWO_SPACES;
  is_stealth = unit_types[units[unit].type].flags & UTYPE_SUBMARINE;
  unit_domain = unit_types[units[unit].type].domain;
  y3 = tile_is_ocean(x2,y);
  civ_bit = 1 << (civ3 & 0x1f) & 0xff;
  redraw_flags = 0;
  is_human = human_civ[0] == civ2;
  if (civ2 == 0) {
    tile = tile_at(x2,y);
    units[unit].visibility = tile->visible | units[unit].visibility;
  }
  reveal_all = game.reveal_map;
  if ((game.reveal_map == '\0') && (((game.game_flags & GAME_SCENARIO) == 0 || ((scenario.flags[0] & 8) == 0)))) {
    map_revealed = false;
  }
  else {
    map_revealed = true;
  }
  net_batch_begin();
  unit_reveal_surroundings_tiles(x2,y,civ2,civ_bit,sees_two,unit_domain,is_human,map_revealed,redraw_flags);
  unit_reveal_surroundings_adjacent(unit,allow_contact,x2,y,civ2,civ3,civ_bit,is_stealth,unit_domain,y3,
                                    is_human,map_revealed,reveal_all,redraw_flags,redrawn);
  unit_reveal_surroundings_far(unit,x2,y,civ2,civ_bit,sees_two,is_stealth,unit_domain,is_human,map_revealed,
                               reveal_all,redraw_flags);
  unit_reveal_surroundings_redraw(unit,x2,y,civ2,civ3,sees_two,redraw_flags,redrawn);
  return;
}

/* Part of unit_caravan_establish_trade (00440750): the trade revenue (distance, trade,
   continents, roads, harbours and superhighways, demand, advances; capped).
   Also reads the commodity carried (commodity) and whether the unit is freight
   (is_freight). */
static int unit_caravan_trade_revenue(iptr unit,uptr city,uptr city2,iptr civ,int civ4,
                                                 uptr &commodity,uint &is_freight)
{
  int revenue;
  int other_value;
  int has_building;
  bool unit_type;
  i32 max_revenue;
  int bonus_count;
  int demand_bonus;
  int base_revenue;
  iptr i;
  base_revenue = map_distance_diag((int)cities[city].x,(int)cities[city].y,
                                (int)cities[city2].x,(int)cities[city2].y);
  if ((game.game_flags & GAME_LARGE_MAP) != 0) {
    base_revenue = (base_revenue << 2) / 5;
  }
  if ((game.game_flags & GAME_SMALL_MAP) != 0) {
    base_revenue = (int)(base_revenue * 5 + (base_revenue * 5 >> 0x1f & 3U)) >> 2;
  }
  base_revenue = (((int)cities[city2].base_trade + (int)cities[city].base_trade) * (base_revenue + 10)
             ) / 0x18;
  revenue = tile_landmass((int)cities[city].x,(int)cities[city].y);
  other_value = tile_landmass((int)cities[city2].x,(int)cities[city2].y);
  if (other_value != revenue) {
    base_revenue = base_revenue << 1;
  }
  if (civ == civ4) {
    base_revenue = base_revenue >> 1;
  }
  commodity = (uptr)(char)units[unit].counter;
  unit_type = units[unit].type == UNIT_FREIGHT;
  if (unit_type) {
    base_revenue += (base_revenue >> 1);
  }
  is_freight = (uint)unit_type;
  bonus_count = map_road_connection(civ,(int)cities[city].x,(int)cities[city].y,
                                (int)cities[city2].x,(int)cities[city2].y);
  has_building = city_has_improvement(city,IMP_AIRPORT);
  if ((has_building != 0) && (has_building = city_has_improvement(city2,IMP_AIRPORT), has_building != 0)) {
    if (other_value == revenue) {
      bonus_count++;
    }
    else {
      bonus_count += 2;
    }
  }
  revenue = city_has_improvement(city,IMP_SUPERHIGHWAYS);
  if (revenue != 0) {
    bonus_count++;
  }
  revenue = city_has_improvement(city2,IMP_SUPERHIGHWAYS);
  if (revenue != 0) {
    bonus_count++;
  }
  base_revenue += (bonus_count * base_revenue >> 1);
  demand_bonus = 0;
  switch(commodity) {
  case 3:
  case 5:
  case 8:
  case 10:
    demand_bonus = base_revenue / 2;
    break;
  case 9:
  case 0xb:
  case 0xc:
  case 0xd:
    demand_bonus = base_revenue;
    break;
  case 0xe:
    demand_bonus = (base_revenue * 3) / 2;
    break;
  case 0xf:
    demand_bonus = base_revenue * 2;
  }
  for (i = 0; i < 3; i++) {
    if ((int)*(char *)&cities[city].demands[i] == commodity) {
      if (units[unit].owner == civ) {
        base_revenue = base_revenue * 2 + demand_bonus;
      }
      else {
        base_revenue = (base_revenue + demand_bonus) * 2;
      }
    }
  }
  if (((game.turn < 200) && (revenue = civ_has_advance(civ,ADV_INVENTION), revenue == 0)) &&
     (revenue = civ_has_advance(civ,ADV_NAVIGATION), revenue == 0)) {
    base_revenue = base_revenue << 1;
  }
  revenue = civ_has_advance(civ,ADV_RAILROAD);
  if (revenue != 0) {
    base_revenue -= base_revenue / 3;
  }
  revenue = civ_has_advance(civ,ADV_FLIGHT);
  if (revenue != 0) {
    base_revenue -= base_revenue / 3;
  }
  max_revenue = 30000;
  revenue = crt_rand_at("unit_caravan_establish_trade");
  revenue = revenue % 10 + 200;
  other_value = civ_research_cost(civ);
  max_revenue = int_clamp((other_value * 2) / 3,revenue,max_revenue);
  revenue = int_clamp(base_revenue,0,max_revenue);
  return revenue;
}

/* Part of unit_caravan_establish_trade (00440750): the trade told to the human players
   involved. */
static void unit_caravan_trade_messages(iptr unit,iptr civ,byte civ2,byte civ3,int civ4,
                                                  uptr commodity,uptr city,uptr city2,int revenue,
                                                  uint is_freight)
{
  if ((int)commodity < 0) {
    text_set_label_arg(0,label_table.string_ids[0x40]);
  }
  else {
    text_set_label_arg(0,commodity_names[commodity]);
  }
  text_set_string_arg(1,(const char *)&cities[city2].name);
  text_set_string_arg(2,(const char *)&cities[city].name);
  text_set_number_arg(0,revenue);
  snd_play_effect((-(uint)(is_freight == 0) & 0x14) + 0x16,1,0,0);
  if ((int)commodity < 0) {
    if (game.multiplayer < 3) {
      dlg_game_txt_with_unit((const char *)&str_foodcaravan,unit);
    }
    else {
      if (human_civ[0] == civ) {
        dlg_game_txt_with_unit((const char *)&str_foodcaravan_2,unit);
      }
      else if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
        net_send_text_args(0x10,net.net_slots[net.net_civ_slots[civ]].id,3,1,
                           unit,is_freight);
      }
      if (civ != civ4) {
        if (human_civ[0] == civ4) {
          dlg_game_txt_with_unit((const char *)&str_foodcaravan_3,unit);
        }
        else if ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) {
          net_send_text_args(0x10,net.net_slots[net.net_civ_slots[civ4]].id,3,
                             1,unit,is_freight);
        }
      }
    }
  }
  else if (game.multiplayer < 3) {
    dlg_game_txt_with_unit((const char *)&str_caravan_2,unit);
  }
  else {
    if (human_civ[0] == civ) {
      dlg_game_txt_with_unit((const char *)&str_caravan_3,unit);
    }
    else if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
      net_send_text_args(0xf,net.net_slots[net.net_civ_slots[civ]].id,3,1,
                         unit,is_freight);
    }
    if (civ != civ4) {
      if (human_civ[0] == civ4) {
        dlg_game_txt_with_unit((const char *)&str_caravan_4,unit);
      }
      else if ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) {
        net_send_text_args(0xf,net.net_slots[net.net_civ_slots[civ4]].id,3,1,
                           unit,is_freight);
      }
    }
  }
}

/* Part of unit_caravan_establish_trade (00440750): the route back, with the commodity the
   other city supplies that the home city wants most. */
static void unit_caravan_trade_return_route(iptr civ,byte civ2,byte civ3,int civ4,uptr commodity,
                                                      uptr city,uptr city2,uint unit_type2,uint is_freight)
{
  uint rand_val;
  iptr adjective;
  uint sign;
  uptr best_commodity;
  uptr supply;
  iptr j;
  iptr i;
  int best_score;
  int score;
  best_score = 0;
  best_commodity = 0xffffffff;
  for (i = 0; i < 3; i++) {
    rand_val = crt_rand_at("unit_caravan_establish_trade");
    sign = (int)rand_val >> 0x1f;
    score = ((rand_val ^ sign) - sign & 7 ^ sign) - sign;
    supply = (uptr)*(char *)&cities[city].supplies[i];
    if ((int)supply < 0) {
      if ((int)supply < 1) {
        supply = ~supply + 1;
      }
    }
    else {
      score += 10;
    }
    if (supply != commodity) {
      if (9 < score) {
        for (j = 0; j < 3; j++) {
          if ((int)*(char *)&cities[city2].demands[j] == supply) {
            score += 10;
          }
        }
      }
      if (best_score <= score) {
        best_score = score;
        best_commodity = supply;
      }
    }
  }
  city_add_trade_route(city,city2,best_commodity);
  if (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) &&
     ((civ != civ4 || (2 < game.multiplayer)))) {
    snd_play_effect((-(uint)(is_freight == 0) & 0x14) + 0x16,1,0,0);
    adjective = civ_get_adjective(civ);
    text_set_string_arg(0,(const char *)adjective);
    text_set_label_arg(1,commodity_names[commodity]);
    text_set_string_arg(2,(const char *)&cities[city2].name);
    text_set_string_arg(3,(const char *)&cities[city].name);
    text_set_label_arg(4,commodity_names[best_commodity]);
    if ((human_civ[0] == civ4) && (civ != civ4)) {
      dlg_game_txt_with_unit_flags((const char *)&str_caravanother,unit_type2,(ui_metrics.large_screen == 0) - 1 & 8);
    }
    else if ((2 < game.multiplayer) && (human_civ[0] != civ4)) {
      net_send_text_args(0x11,net.net_slots[net.net_civ_slots[civ4]].id,5,0,
                         unit_type2,(ui_metrics.large_screen == 0) - 1 & 8);
    }
    if ((2 < game.multiplayer) && (civ != civ4)) {
      if (human_civ[0] == civ) {
        dlg_game_txt_with_unit_flags((const char *)&str_caravanother_2,unit_type2,(ui_metrics.large_screen == 0) - 1 & 8);
      }
      else if (((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) && (human_civ[0] != civ)
              ) {
        net_send_text_args(0x11,net.net_slots[net.net_civ_slots[civ]].id,5,0,
                           unit_type2,(ui_metrics.large_screen == 0) - 1 & 8);
      }
    }
  }
}

/* 00440750 auto */
void __cdecl unit_caravan_establish_trade(iptr unit,uptr city)

{
  byte civ2;
  byte civ3;
  iptr civ;
  int civ4;
  int revenue;
  uptr commodity;
  uint unit_type2;
  uptr city2;
  uint is_freight;
  
  civ2 = units[unit].owner;
  civ = (iptr)(char)civ2;
  civ3 = cities[city].owner;
  civ4 = (int)(char)civ3;
  if (units[unit].home_city == -1) {
    city2 = 0xffffffff;
  }
  else {
    city2 = (uptr)(byte)units[unit].home_city;
  }
  if (((int)city2 < 0) &&
     (city2 = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ,0xffffffff,-1)
     , (int)city2 < 0)) {
    city2 = 0;
  }
  revenue = unit_caravan_trade_revenue(unit,city,city2,civ,civ4,commodity,is_freight);
  if ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) {
    unit_caravan_trade_messages(unit,civ,civ2,civ3,civ4,commodity,city,city2,revenue,is_freight);
  }
  civ_adjust_attitude(civ4,civ,0xfffffff6);
  civ_adjust_attitude(civ,civ4,0xfffffff6);
  if ((int)commodity < 0) {
    city_recalc(city,0);
    cities[city].food_box =
         cities[city].food_box + (short)(((cities[city].size + 1) * city_calc.food_box_rows) / 2);
  }
  else {
    civs[civ].gold += revenue;
    civs[civ].research_progress += (short)revenue;
    if (human_civ[0] == civ) {
      status_draw_info(1);
    }
  }
  unit_type2 = (uint)units[unit].type;
  unit_destroy_redraw(unit,1);
  city_add_trade_route(city2,city,commodity);
  if (-1 < (int)commodity) {
    unit_caravan_trade_return_route(civ,civ2,civ3,civ4,commodity,city,city2,unit_type2,is_freight);
  }
  return;
}

/* 00447ebe auto */
void __cdecl nuke_anim_sprites_global_init(void)

{
  nuke_anim_sprites_static_init();
  nuke_anim_sprites_reg_atexit();
  return;
}

/* 00447ed8 auto */
void __cdecl nuke_anim_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&hill_sprites_block.nuke_anim_sprites),0x3c,0xb,sprite_init,sprite_destroy);
  return;
}

/* 00447f00 auto */
void __cdecl nuke_anim_sprites_reg_atexit(void)

{
  _atexit(nuke_anim_sprites_static_dtor);
  return;
}

/* 00447f1d auto */
void __cdecl nuke_anim_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&hill_sprites_block.nuke_anim_sprites),0x3c,0xb,sprite_destroy);
  return;
}

/* 00448103 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl unit_shield_sprite_global_init(void)

{
  unit_shield_sprite_static_init();
  unit_shield_sprite_reg_atexit();
  return;
}

/* 0044811d auto */
void __cdecl unit_shield_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&unit_shield_sprite));
  return;
}

/* 00448137 auto */
void __cdecl unit_shield_sprite_reg_atexit(void)

{
  _atexit(unit_shield_sprite_static_dtor);
  return;
}

/* 00448154 auto */
void __cdecl unit_shield_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&unit_shield_sprite));
  return;
}

/* 00448272 auto */
void __cdecl unit_sprites_global_init(void)

{
  unit_sprites_static_init();
  unit_sprites_reg_atexit();
  return;
}

/* 0044828c auto */
void __cdecl unit_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&pollution_sprite_block.unit_sprites),0x3c,0x3f,sprite_init,sprite_destroy);
  return;
}

/* 004482b4 auto */
void __cdecl unit_sprites_reg_atexit(void)

{
  _atexit(unit_sprites_static_dtor);
  return;
}

/* 004482d1 auto */
void __cdecl unit_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&pollution_sprite_block.unit_sprites),0x3c,0x3f,sprite_destroy);
  return;
}

/* 004482f4 auto */
void __cdecl fortify_overlay_sprites_global_init(void)

{
  fortify_overlay_sprites_static_init();
  fortify_overlay_sprites_reg_atexit();
  return;
}

/* 0044830e auto */
void __cdecl fortify_overlay_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites),0x3c,8,sprite_init,sprite_destroy);
  return;
}

/* 00448336 auto */
void __cdecl fortify_overlay_sprites_reg_atexit(void)

{
  _atexit(fortify_overlay_sprites_static_dtor);
  return;
}

/* 00448353 auto */
void __cdecl fortify_overlay_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites),0x3c,8,sprite_destroy);
  return;
}

/* 00448a4d auto */
void __cdecl explosion_sprites_global_init(void)

{
  explosion_sprites_static_init();
  explosion_sprites_reg_atexit();
  return;
}

/* 00448a67 auto */
void __cdecl explosion_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&government_sprites_block.explosion_sprites),0x3c,8,sprite_init,sprite_destroy);
  return;
}

/* 00448a8f auto */
void __cdecl explosion_sprites_reg_atexit(void)

{
  _atexit(explosion_sprites_static_dtor);
  return;
}

/* 00448aac auto */
void __cdecl explosion_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&government_sprites_block.explosion_sprites),0x3c,8,sprite_destroy);
  return;
}

/* 0044b239 auto */
void __cdecl unit_sprite_scan_shield_offset(int unit_type,int x,int y)

{
  int pixel;
  int dy;
  int dx;
  
  road_sprites_block.unit_shield_x_offsets[unit_type] = 0;
  road_sprites_block.unit_shield_y_offsets[unit_type] = 0;
  for (dx = 0; dx < 0x40; dx++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(x - 1 + dx + 1,y - 1);
    if (pixel == 4) {
      road_sprites_block.unit_shield_x_offsets[unit_type] = dx;
    }
  }
  for (dy = 0; dy < 0x30; dy++) {
    pixel = ((DibSurface *)((i8 *)&gfx_sheet_surface_block.gfx_sheet_surface))->get_pixel(x - 1,y - 1 + dy + 1);
    if (pixel == 4) {
      road_sprites_block.unit_shield_y_offsets[unit_type] = dy;
    }
  }
  return;
}

/* 00472d20 auto */
i32 __cdecl unit_make_dummy(iptr unit_type,iptr owner)

{
  dummy_unit.type = (i8)unit_type;
  dummy_unit.owner = (i8)owner;
  dummy_unit.x = 0xffce;
  dummy_unit.y = 0xffce;
  dummy_unit.flags = 0;
  dummy_unit.home_city = 0xff;
  dummy_unit.moves_used = 0;
  dummy_unit.visibility = 0;
  dummy_unit.byte_0e = 0;
  dummy_unit.counter = 0;
  dummy_unit.prev_in_stack = 0xffff;
  dummy_unit.next_in_stack = 0xffff;
  dummy_unit.orders = 0xff;
  dummy_unit.goto_x = 0xffff;
  dummy_unit.goto_y = 0xffff;
  return 0x800;
}

/* 0048710a auto */
void __cdecl units_reset_for_turn(iptr civ)

{
  uint *spy_flags;
  uptr unit;
  
  map_rebuild_unit_flags();
  if (civ == -3) {
    for (unit = 0; (int)unit < (int)game.num_units; unit++) {
      if (units[unit].id != 0) {
        unit_exhaust_moves(unit);
      }
    }
  }
  for (unit = 0; (int)unit < (int)game.num_units; unit++) {
    if ((units[unit].id != 0) &&
       (((tile_set_improvements((int)units[unit].x,(int)units[unit].y,1,1,1), civ == -1 ||
         (civ == -2)) || (units[unit].owner == civ)))) {
      if ((units[unit].flags & UNITF_SPY_ON_MISSION) == 0) {
        units[unit].moves_used = '\0';
        units[unit].aux_timer = '\0';
        units[unit].flags = units[unit].flags & ~UNITF_WAITING;
        if ((((units[unit].flags & UNITF_BORDER_WARNED) != 0) && ((units[unit].flags & UNITF_BORDER_INTRUDER) == 0)) &&
           ((((int)game.turn ^ unit & 3) & 3) == 0)) {
          units[unit].flags = units[unit].flags & ~UNITF_BORDER_WARNED;
        }
        units[unit].flags = units[unit].flags & ~UNITF_BORDER_INTRUDER;
      }
      else {
        units[unit].aux_timer += 0xff;
        if (units[unit].aux_timer == '\0') {
          units[unit].flags = units[unit].flags & ~UNITF_SPY_ON_MISSION;
          spy_flags = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + (char)units[unit].counter * 4);
          *spy_flags = *spy_flags & 0xfeffffff;
          spy_try_escape(unit,0,(int)(char)units[unit].counter);
        }
      }
    }
  }
  return;
}

/* 00489859 auto */
void __cdecl unit_activate_next(i32 search_mode)

{
  iptr unit;
  iptr unit2;
  
  if (turn_flags.active_unit_lost_in_combat == 0) {
    unit2 = (iptr)game.active_unit;
  }
  else {
    unit2 = -1;
  }
  unit = unit_find_nearest_selectable(unit2,search_mode);
  turn_flags.active_unit_lost_in_combat = 0;
  game.active_unit = (i16)unit;
  if (((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) {
    if (((units[unit].orders != ORDER_FORTIFYING) && (units[unit].orders != ORDER_FORTIFIED)) &&
       ((units[unit].orders == ORDER_NONE || ('\n' < units[unit].orders)))) {
      units[unit].aux_timer = '\0';
      map_cursor.x[0] = units[unit].x;
      map_cursor.y[0] = units[unit].y;
      status_redraw(1);
      mapviews_ensure_visible((int)units[unit].x,(int)units[unit].y,(int)units[unit].owner);
      menu_update_states();
    }
    if (units[unit].moves_used == '\0') {
      unit_reveal_surroundings(unit,1);
    }
    if (net.mp_simultaneous_moves == '\0') {
      input_flush_events();
    }
  }
  if ((unit < 0) || (units[unit].id == 0)) {
    ui_redraw_all_maps(0);
  }
  return;
}

/* 004ad01e auto */
i32 __cdecl path_grid_get_cost(int x,int y)

{
  return *(i32 *)
          ((x - path_queue.grid_center_x) * 0xc0 +
           ((x - path_queue.grid_center_x) + (y - path_queue.grid_center_y) >> 1) * -0xbc + (iptr)(((u8 *)&dialog_text_font_obj_block.path_grid_costs + 4608)));
}

/* 004ad076 auto */
void __cdecl path_grid_set_cost(int x,int y,i32 cost)

{
  *(i32 *)
   ((x - path_queue.grid_center_x) * 0xc0 +
    ((y - path_queue.grid_center_y) + (x - path_queue.grid_center_x) >> 1) * -0xbc + (iptr)(((u8 *)&dialog_text_font_obj_block.path_grid_costs + 4608))) = cost;
  return;
}

/* 004ad20f auto */
int __cdecl path_coarse_find_waypoint(i32 from_x,i32 from_y,int to_x,int to_y)

{
  byte dir_bits;
  int start_cy;
  int snapped;
  i8 *cell;
  char *visited;
  byte *cell_ptr;
  int start_cx;
  uint start_dirs;
  uint cur_dirs;
  int waypoint_y;
  int waypoint_x;
  uint step;
  int raw_wp_x;
  int found;
  int cy;
  int has_waypoint;
  int cx;
  iptr best_dir;
  uint dist;
  int ny;
  uint is_sea;
  uint cur_step;
  int nx;
  uint dir_mask;
  iptr dir;
  int raw_wp_y;
  uint best_step;
  int debug_flag;
  
  found = 0;
  debug_flag = pathfind.debug_requested;
  path_queue.goal_x = to_x;
  path_queue.goal_y = to_y;
  is_sea = (uint)(unit_types[pathfind.unit_type].domain == DOMAIN_SEA);
  snapped = path_coarse_snap_cell(from_x,from_y,is_sea);
  start_cy = path_queue.goal_y;
  start_cx = path_queue.goal_x;
  if (snapped == 0) {
    path_queue.goal_x = to_x;
    path_queue.goal_y = to_y;
  }
  else {
    path_queue.coarse_start_x = path_queue.goal_x;
    path_queue.coarse_start_y = path_queue.goal_y;
    path_coarse_snap_cell(to_x,to_y,is_sea);
    _memset(map_data.coarse[2],0,(int)map.quarter_w * (int)map.quarter_h);
    path_queue.queue_head = 0;
    path_queue.queue_x[0] = path_queue.goal_x;
    path_queue.queue_y[0] = path_queue.goal_y;
    path_queue.queue_tail = 1;
    cell = (i8 *)map_coarse2_at(path_queue.goal_x,path_queue.goal_y);
    *cell = 1;
    while( true ) {
      cx = (((i32 *)&path_queue.queue_x))[path_queue.queue_head];
      cy = (((i32 *)&path_queue.queue_y))[path_queue.queue_head];
      path_queue.queue_head = path_queue.queue_head + 1U & 0xff;
      if ((cx == start_cx) && (cy == start_cy)) {
        found = 1;
        break;
      }
      cell_ptr = map_coarse2_at(cx,cy);
      cur_step = (uint)*cell_ptr;
      if (is_sea == 0) {
        cell_ptr = map_coarse0_at(cx,cy);
        dir_bits = *cell_ptr;
      }
      else {
        cell_ptr = map_coarse1_at(cx,cy);
        dir_bits = *cell_ptr;
      }
      cur_dirs = (uint)dir_bits;
      dir_mask = cur_dirs;
      for (dir = 0; dir < 8; dir++) {
        if ((dir_mask & 1 << ((byte)dir & 0x1f)) != 0) {
          nx = coarse_wrap_x((((i8 *)&dir_dy_table_block.coarse_dir_dx_table))[dir] + cx);
          ny = (((i8 *)&dir_dy_table_block.coarse_dir_dy_table))[dir] + cy;
          if ((((-1 < nx) && (nx < map.quarter_w)) && (-1 < ny)) &&
             ((ny < map.quarter_h &&
              (visited = (char *)map_coarse2_at(nx,ny), *visited == '\0')))) {
            visited = (char *)map_coarse2_at(nx,ny);
            *visited = (char)cur_step + 1;
            (((i32 *)&path_queue.queue_x))[path_queue.queue_tail] = nx;
            (((i32 *)&path_queue.queue_y))[path_queue.queue_tail] = ny;
            path_queue.queue_tail = path_queue.queue_tail + 1U & 0xff;
          }
        }
      }
      if ((found != 0) || (path_queue.queue_head == path_queue.queue_tail)) break;
    }
    has_waypoint = 0;
    if (found != 0) {
      best_step = 99;
      best_dir = -1;
      if (is_sea == 0) {
        cell_ptr = map_coarse0_at(start_cx,start_cy);
        dir_bits = *cell_ptr;
      }
      else {
        cell_ptr = map_coarse1_at(start_cx,start_cy);
        dir_bits = *cell_ptr;
      }
      start_dirs = (uint)dir_bits;
      dir_mask = start_dirs;
      for (dir = 0; dir < 8; dir++) {
        if ((dir_mask & 1 << ((byte)dir & 0x1f)) != 0) {
          nx = coarse_wrap_x((((i8 *)&dir_dy_table_block.coarse_dir_dx_table))[dir] + start_cx);
          ny = (((i8 *)&dir_dy_table_block.coarse_dir_dy_table))[dir] + start_cy;
          if ((((-1 < nx) && (nx < map.quarter_w)) && (-1 < ny)) &&
             (ny < map.quarter_h)) {
            cell_ptr = map_coarse2_at(nx,ny);
            step = (uint)*cell_ptr;
            if (step != 0) {
              if ((int)step < (int)best_step) {
                best_dir = dir;
                best_step = step;
                cur_step = map_distance_diag(to_x,to_y,nx * 4 + 1,ny * 4 + 1);
              }
              else if ((step == best_step) &&
                      (dist = map_distance_diag(to_x,to_y,nx * 4 + 1,
                                                     ny * 4 + 1),
                      (int)dist < (int)cur_step)) {
                best_dir = dir;
                cur_step = dist;
              }
            }
          }
        }
      }
      if (-1 < best_dir) {
        start_cx = coarse_wrap_x((((i8 *)&dir_dy_table_block.coarse_dir_dx_table))[best_dir] + start_cx);
        waypoint_x = start_cx * 4 + 1;
        waypoint_y = ((((i8 *)&dir_dy_table_block.coarse_dir_dy_table))[best_dir] + start_cy) * 4 + 1;
        has_waypoint = 1;
        raw_wp_x = waypoint_x;
        raw_wp_y = waypoint_y;
        map_diag_tile_by_ocean(waypoint_x,waypoint_y,(iptr)&waypoint_x,(iptr)&waypoint_y,is_sea);
      }
    }
    if (has_waypoint == 0) {
      path_queue.goal_x = to_x;
      path_queue.goal_y = to_y;
    }
    else {
      path_queue.goal_x = waypoint_x;
      path_queue.goal_y = waypoint_y;
    }
  }
  if (debug_flag != 0) {
    pathfind.debug_pending = 1;
  }
  return found;
}

/* 004ad822 auto */
uint __cdecl path_coarse_snap_cell(int x,int y,int is_sea)

{
  int cx;
  int cy;
  char *cell;
  int nx;
  int ny;
  iptr best_dir;
  int y2;
  iptr dir;
  int x2;
  int best_dist;
  
  best_dir = -1;
  cx = x >> 2;
  cy = y >> 2;
  if (is_sea == 0) {
    cell = (char *)map_coarse0_at(cx,cy);
    if (*cell != '\0') {
      best_dir = 8;
    }
  }
  else {
    cell = (char *)map_coarse1_at(cx,cy);
    if (*cell != '\0') {
      best_dir = 8;
    }
  }
  if (best_dir == 8) {
    nx = map_diag_tile_by_ocean(cx * 4 + 1,cy * 4 + 1,(iptr)&x2,(iptr)&y2,is_sea);
    if (nx == 0) {
      best_dir = -1;
    }
    else {
      nx = map_path_length(x2,y2,x,y,is_sea,0x12);
      if (nx < 0) {
        best_dir = -1;
      }
    }
  }
  if (best_dir < 0) {
    best_dist = 99;
    for (dir = 0; dir < 8; dir++) {
      nx = coarse_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + cx);
      ny = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + cy;
      if (((((-1 < nx) && (nx < map.quarter_w)) && (-1 < ny)) && (ny < map.quarter_h))
         && (((is_sea != 0 && (cell = (char *)map_coarse1_at(nx,ny), *cell != '\0')) ||
             ((is_sea == 0 && (cell = (char *)map_coarse0_at(nx,ny), *cell != '\0'))))))
      {
        x2 = nx * 4 + 1;
        y2 = ny * 4 + 1;
        nx = map_in_bounds(x2,y2);
        if (((nx != 0) &&
            (nx = map_distance_diag(x,y,x2,y2), nx < best_dist)) &&
           ((ny = map_diag_tile_by_ocean(x2,y2,(iptr)&x2,(iptr)&y2,is_sea),
            ny != 0 &&
            (ny = map_path_length(x2,y2,x,y,is_sea,0x12), -1 < ny))
           )) {
          best_dir = dir;
          best_dist = nx;
        }
      }
    }
  }
  dir = best_dir;
  if (best_dir < 0) {
    dir = 8;
  }
  path_queue.goal_x = coarse_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + cx);
  path_queue.goal_y = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + cy;
  return (uint)(-1 < best_dir);
}

/* Part of unit_goto_next_dir (004adafc): the cost of a step to (x,y2)
   (roads, rivers, railroads, terrain). */
static int unit_goto_step_cost(iptr unit,int x2,int y,int x,int y2,byte terrain,byte cur_tile_flags,
                                          uint cur_improvements,uint dx)
{
  byte *tile;
  uint improvements2;
  int cost2;
  if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
    if (((cur_improvements & 0x20) == 0) ||
       (improvements2 = tile_improvements(x,y2), (improvements2 & TILE_RAILROAD) == 0)) {
      if (((dx & 0x10) == 0) ||
         (improvements2 = tile_improvements(x,y2), (improvements2 & TILE_ROAD) == 0)) {
        if (((((cur_tile_flags & 0x80) != 0) &&
             (tile = (byte *)tile_at(x,y2), (*tile & 0x80) != 0)) &&
            (x = map_dx_wrapped(x2,x), x == 1)) &&
           (y2 = (y == y2 || y - y2 < 0) ? ~(y - y2) + 1 : y - y2, y2 == 1)) {
          cost2 = 4;
        }
        else {
          x = unit_max_moves(unit);
          if ((int)(uint)cosmic.road_move_mult < x) {
            cost2 = (int)(char)terrain_types[terrain].move_cost *
                       (uint)cosmic.road_move_mult * 4;
          }
          else {
            cost2 = (uint)cosmic.road_move_mult << 2;
          }
        }
      }
      else {
        cost2 = 4;
      }
    }
    else {
      cost2 = 1;
    }
  }
  else {
    cost2 = (uint)cosmic.road_move_mult << 2;
  }
  return cost2;
}

/* Part of unit_goto_next_dir (004adafc): the greedy step toward the
   waypoint: the cheapest adjacent tile the unit may enter that does not lead
   away (for a human player), not straight back. Returns the direction, or -1
   (the goto given up). */
static uptr unit_goto_greedy_dir(iptr unit,int x2,int y,int civ,uint is_human,uint delta_x,
                                            uint dy,int cur_dist,int goal_x,int goal_y,u8 unit_domain)
{
  byte cur_tile_flags;
  byte terrain;
  int dist;
  uint dx;
  byte *tile;
  uint cur_improvements;
  int x;
  int tmp;
  int tmp2;
  int y2;
  int cost2;
  uptr result_dir;
  int new_dist;
  uint ady;
  uptr dir;
  uint adx;
  int best_cost;
  best_cost = 9999;
  result_dir = 8;
  dx = tile_improvements(x2,y);
  tile = (byte *)tile_at(x2,y);
  cur_tile_flags = *tile;
  cur_improvements = tile_improvements(x2,y);
  dist = tile_in_enemy_zoc(x2,y,civ);
  for (dir = 0; (int)dir < 8; dir++) {
    x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    adx = delta_x - (int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir];
    ady = dy - (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    if ((int)adx < 1) {
      adx = ~adx + 1;
    }
    if ((int)ady < 1) {
      ady = ~ady + 1;
    }
    tmp = (int)(adx + ady) >> 1;
    if ((int)ady < (int)adx) {
      new_dist = adx * 2 - ((tmp - ady) + 1);
    }
    else {
      new_dist = ady * 2 - ((tmp - adx) + 1);
    }
    if ((((is_human == 0) || (new_dist <= cur_dist)) &&
        (tmp = map_in_bounds(x,y2), tmp != 0)) &&
       (((unit_types[units[unit].type].domain != DOMAIN_AIR ||
         (tmp = tile_airbase_owner(x,y2), tmp < 0)) ||
        ((x == path_queue.goal_x && (y2 == path_queue.goal_y)))))) {
      terrain = tile_terrain(x,y2);
      tmp = tile_unit_owner(x,y2);
      if ((((tmp < 0) || (civ == tmp)) &&
          ((((terrain == TERRAIN_OCEAN) == (unit_domain == 2) &&
            (((dist == 0 ||
              (tmp2 = tile_in_enemy_zoc(x,y2,civ), tmp2 == 0)) ||
             ((unit_types[units[unit].type].flags & UTYPE_IGNORE_ZOC) != 0)))) ||
           (unit_types[units[unit].type].domain == DOMAIN_AIR)))) ||
         (((tmp2 = tile_city_owner(x,y2), tmp2 == civ && (x == goal_x))
          && (y2 == goal_y)))) {
        cost2 = unit_goto_step_cost(unit,x2,y,x,y2,terrain,cur_tile_flags,cur_improvements,dx);
        cost2 = (new_dist * 4 + cost2 + adx + ady) * 4;
        if ((new_dist != 0) && (-1 < tmp)) {
          cost2++;
        }
        if (cost2 < best_cost) {
          result_dir = dir;
          best_cost = cost2;
        }
      }
    }
  }
  if (((-1 < (char)units[unit].last_dir) &&
      (((int)(char)units[unit].last_dir ^ 4U) == result_dir)) &&
     ((unit_types[units[unit].type].domain != DOMAIN_AIR && (units[unit].moves_used != '\0')
      ))) {
    result_dir = 0xffffffff;
    units[unit].orders = ORDER_NONE;
    if ((is_human == 0) || ((units[unit].flags & UNITF_AUTOMATED) != 0)) {
      if (is_human != 0) {
        units[unit].flags = units[unit].flags & ~UNITF_AUTOMATED;
      }
      unit_exhaust_moves(unit);
    }
  }
  if (result_dir == 8) {
    units[unit].orders = ORDER_NONE;
    if ((is_human == 0) || ((units[unit].flags & UNITF_AUTOMATED) != 0)) {
      if (is_human != 0) {
        units[unit].flags = units[unit].flags & ~UNITF_AUTOMATED;
      }
      unit_exhaust_moves(unit);
    }
    result_dir = 0xffffffff;
  }
  return result_dir;
}

/* 004adafc auto */
uptr __cdecl unit_goto_next_dir(iptr unit)

{
  u8 unit_domain;
  int goal_x;
  int goal_y;
  int x2;
  int y;
  int civ;
  uint is_human;
  int dist;
  uint dx;
  uint dy;
  int x;
  int tmp;
  uint delta_x;
  uptr result_dir;
  int cur_dist;
  uint abs_dy;
  uint abs_dx;
  
  result_dir = 0xffffffff;
  goal_x = (int)units[unit].goto_x;
  goal_y = (int)units[unit].goto_y;
  x2 = (int)units[unit].x;
  y = (int)units[unit].y;
  pathfind.unit_type = (iptr)units[unit].type;
  unit_domain = unit_types[units[unit].type].domain;
  civ = map_in_bounds(goal_x,goal_y);
  if ((civ != 0) && ((goal_x != x2 || (goal_y != y)))) {
    civ = (int)units[unit].owner;
    is_human = (uint)game.human_players & 1 << (units[unit].owner & 0x1fU);
    delta_x = goal_x - x2;
    if ((game.rules & RULE_FLAT_WORLD) == 0) {
      if ((map.width - 2 <= goal_x) && (x2 < 2)) {
        delta_x = -1;
      }
      if ((goal_x < 2) && (map.width - 2 <= x2)) {
        delta_x = 1;
      }
    }
    dist = map_distance(x2,y,goal_x,goal_y);
    if (dist < 2) {
      result_dir = map_dir_from_delta(delta_x,goal_y - y);
    }
    else {
      dist = goal_x;
      x = goal_y;
      if (unit_types[units[unit].type].domain != DOMAIN_AIR) {
        dist = map_distance(x2,y,goal_x,goal_y);
        if ((dist < 0x17) &&
           (path_queue.goal_x = goal_x, path_queue.goal_y = goal_y,
           result_dir = map_path_next_dir(x2,y,999), -1 < (int)result_dir)) {
          pathfind.debug_pending = 0;
          pathfind.debug_requested = 0;
          return result_dir;
        }
        tmp = path_coarse_find_waypoint(x2,y,goal_x,goal_y);
        dist = path_queue.goal_x;
        x = path_queue.goal_y;
        if (tmp != 0) {
          dist = map_distance(x2,y,path_queue.goal_x,path_queue.goal_y);
          if (dist < 0x17) {
            result_dir = map_path_next_dir(x2,y,0x3e6);
          }
          else {
            result_dir = 0xffffffff;
          }
          dist = path_queue.goal_x;
          x = path_queue.goal_y;
          if (-1 < (int)result_dir) {
            pathfind.debug_pending = 0;
            pathfind.debug_requested = 0;
            return result_dir;
          }
        }
      }
      path_queue.goal_y = x;
      path_queue.goal_x = dist;
      dx = path_queue.goal_x - x2;
      dy = path_queue.goal_y - y;
      abs_dx = dx;
      if ((int)dx < 1) {
        abs_dx = ~dx + 1;
      }
      abs_dy = dy;
      if ((int)dy < 1) {
        abs_dy = ~dy + 1;
      }
      delta_x = dx;
      if ((((game.rules & RULE_FLAT_WORLD) == 0) && ((int)map.width >> 1 < (int)abs_dx)) &&
         (abs_dx = (int)map.width - abs_dx, delta_x = abs_dx, -1 < (int)dx)) {
        delta_x = -abs_dx;
      }
      dist = (int)(abs_dy + abs_dx) >> 1;
      if ((int)abs_dy < (int)abs_dx) {
        cur_dist = abs_dx * 2 - ((dist - abs_dy) + 1);
      }
      else {
        cur_dist = abs_dy * 2 - ((dist - abs_dx) + 1);
      }
      if ((delta_x == 0) && (dy == 0)) {
        units[unit].orders = ORDER_NONE;
        unit_exhaust_moves(unit);
      }
      else {
        result_dir = unit_goto_greedy_dir(unit,x2,y,civ,is_human,delta_x,dy,cur_dist,goal_x,goal_y,unit_domain);
      }
    }
  }
  pathfind.debug_pending = 0;
  pathfind.debug_requested = 0;
  return result_dir;
}

/* Part of unit_do_terrain_work (004c42a0): the turns the work takes on this terrain
   (turns_needed for the other orders). */
static int unit_terrain_work_turns(iptr order,int x,int y,uint terrain,int turns_needed)
{
  uint improvements2;
  byte *tile;
  switch(order) {
  case 4:
    turns_needed = (int)(char)terrain_types[terrain].move_cost / 2 + 3;
    break;
  case 5:
    improvements2 = tile_improvements(x,y);
    turns_needed = ((-(uint)((improvements2 & TILE_ROAD) == 0) & 0xfffffffe) + 4) *
              (int)(char)terrain_types[terrain].move_cost;
    tile = (byte *)tile_at(x,y);
    if ((*tile & 0x80) != 0) {
      turns_needed += 2;
    }
    break;
  case 6:
    turns_needed = (int)(char)terrain_types[terrain].irrigate_turns;
    break;
  case 7:
    turns_needed = (int)(char)terrain_types[terrain].mine_turns;
    break;
  case 8:
    turns_needed = (int)(char)terrain_types[terrain].move_cost * (uint)cosmic.transform_time;
    break;
  case 9:
    turns_needed = 4;
    break;
  case 10:
    turns_needed = 4;
  }
  return turns_needed;
}

/* Part of unit_do_terrain_work (004c42a0): the finished work changes the tile. */
static void unit_terrain_work_finish(iptr order,int x,int y,uint terrain,char terrain2)
{
  int result;
  uptr imp_bits;
  i32 imp_low;
  uptr imp_pollution;
  switch(order) {
  case 4:
    result = tile_city_owner(x,y);
    if (result < 0) {
      tile_set_improvements(x,y,0x40,1,1);
      tile_set_improvements(x,y,2,0,1);
    }
    break;
  case 5:
    terrain = tile_improvements(x,y);
    if (((terrain & TILE_ROAD) == 0) && (result = tile_city_owner(x,y), result < 0)) {
      tile_set_improvements(x,y,0x10,1,1);
    }
    else {
      result = civ_has_advance((int)terrain2,ADV_RAILROAD);
      if (result != 0) {
        tile_set_improvements(x,y,0x30,1,1);
      }
    }
    break;
  case 6:
  case 7:
    if (terrain_types[terrain].improve_to[order - 6] < '\0') {
      if (order == 6) {
        terrain = tile_improvements(x,y);
        if ((terrain & TILE_IRRIGATION) == 0) {
          tile_set_improvements(x,y,8,0,1);
          tile_set_improvements(x,y,4,1,1);
        }
        else {
          result = civ_has_advance((int)terrain2,ADV_REFRIGERATION);
          if (result != 0) {
            tile_set_improvements(x,y,0xc,1,1);
          }
        }
      }
      else {
        tile_set_improvements(x,y,4,0,1);
        tile_set_improvements(x,y,8,1,1);
      }
    }
    else {
      tile_set_terrain(x,y,(int)terrain_types[terrain].improve_to[order - 6],1);
      terrain2 = terrain_types[terrain].improve_to[order - 6];
      if ((terrain_types[terrain2].irrigate_to != -2) &&
         (terrain = tile_improvements(x,y), (terrain & TILE_IRRIGATION) != 0)) {
        tile_set_improvements(x,y,0xc,0,1);
      }
      if ((terrain_types[terrain2].mine_to != -2) &&
         (imp_low = tile_improvements(x,y), ((byte)imp_low & TILE_FARMLAND) == 8)) {
        tile_set_improvements(x,y,8,0,1);
      }
    }
    break;
  case 8:
    tile_set_terrain(x,y,(int)terrain_types[terrain].transform_to,1);
    terrain2 = terrain_types[terrain].transform_to;
    if ((terrain_types[terrain2].irrigate_to != -2) &&
       (imp_bits = tile_improvements(x,y), (imp_bits & TILE_IRRIGATION) != 0)) {
      tile_set_improvements(x,y,0xc,0,1);
    }
    if ((terrain_types[terrain2].mine_to != -2) &&
       (imp_low = tile_improvements(x,y), ((byte)imp_low & TILE_FARMLAND) == 8)) {
      tile_set_improvements(x,y,8,0,1);
    }
    break;
  case 9:
    imp_pollution = tile_improvements(x,y);
    if ((imp_pollution & TILE_POLLUTION) != 0) {
      game.polluted_tiles--;
    }
    tile_set_improvements(x,y,0x80,0,1);
    break;
  case 10:
    result = tile_city_owner(x,y);
    if (result < 0) {
      tile_set_improvements(x,y,0x42,1,1);
    }
  }
}

/* 004c42a0 auto */
void __cdecl unit_do_terrain_work(iptr unit,iptr order)

{
  char terrain2;
  char old_orders;
  byte terrain_id;
  int x;
  int y;
  uint terrain;
  iptr unit2;
  iptr unit3;
  int turns_needed;
  
  unit2 = unit;
  turns_needed = 1;
  terrain2 = units[unit].owner;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  terrain_id = tile_terrain(x,y);
  terrain = (uint)terrain_id;
  old_orders = units[unit].orders;
  units[unit].orders = (i8)order;
  turns_needed = unit_terrain_work_turns(order,x,y,terrain,turns_needed);
  blink.unit_active = 0;
  unit_exhaust_moves(unit);
  unit3 = -1;
  for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit)) {
    if (((unit_types[units[unit].type].role == ROLE_SETTLE) && (units[unit].orders == order)) &&
       (unit != unit2)) {
      unit3 = unit;
      break;
    }
  }
  if (-1 < unit3) {
    units[unit2].counter += units[unit3].counter;
    units[unit3].counter = '\0';
    if (extra_rule(X_WORKER_HANDOVER) && ((1 << (units[unit3].owner & 0x1f) & (uint)game.human_players) != 0)) {
      /* X17: the worker that was there stops; one worker with the order on the tile */
      units[unit3].orders = ORDER_NONE;
      ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit3,-99,-99);
    }
  }
  units[unit2].counter++;
  if (units[unit2].type == UNIT_ENGINEERS) {
    units[unit2].counter++;
  }
  if ((char)units[unit2].counter < turns_needed) {
    map_redraw_area_all_views(x,y,0,human_civ[0],1);
    if (units[unit2].orders != old_orders) {
      ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit2,-99,-99);
    }
  }
  else {
    for (unit = unit_stack_first(unit2); -1 < unit;
        unit = unit_next_in_stack(unit)) {
      if ((unit_types[units[unit].type].role == ROLE_SETTLE) && (units[unit].orders == order)
         ) {
        units[unit].counter = '\0';
        units[unit].orders = ORDER_NONE;
      }
    }
    unit_terrain_work_finish(order,x,y,terrain,terrain2);
    tile_update_known(x,y,(int)units[unit2].owner);
    map_redraw_area_all_views(x,y,1,human_civ[0],1);
    if (units[unit2].owner == human_civ[0]) {
      status_redraw(1);
    }
    if (units[unit2].orders != old_orders) {
      ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit2,-99,-99);
    }
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_near(x,y);
  }
}

/* 004c4ada auto */
void __cdecl unit_set_fortified(iptr unit)

{
  iptr city;
  i32 improvements2;
  iptr city2;
  i8 home_byte;
  uint old_home;
  
  blink.unit_active = 0;
  units[unit].orders = ORDER_FORTIFIED;
  if ((((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) == 0) &&
     (-1 < units[unit].home_city)) {
    city = city_at((int)units[unit].x,(int)units[unit].y);
    if (city < 0) {
      improvements2 = tile_improvements((int)units[unit].x,(int)units[unit].y);
      if (((((byte)improvements2 & TILE_AIRBASE) == 0x40) &&
          (city2 = city_find_nearest((int)units[unit].x,(int)units[unit].y,
                                      (int)units[unit].owner,0xffffffff,-1), -1 < city2)) &&
         (2 < cities[city2].size)) {
        if (units[unit].home_city == -1) {
          old_home = 0xffffffff;
        }
        else {
          old_home = (uint)(byte)units[unit].home_city;
        }
        home_byte = (i8)city2;
        units[unit].home_city = home_byte;
        if (-1 < (int)old_home) {
          ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(old_home);
        }
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city2);
      }
    }
    else if (2 < cities[city].size) {
      if (units[unit].home_city == -1) {
        old_home = 0xffffffff;
      }
      else {
        old_home = (uint)(byte)units[unit].home_city;
      }
      home_byte = (i8)city;
      units[unit].home_city = home_byte;
      if (-1 < (int)old_home) {
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(old_home);
      }
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city);
    }
  }
  map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
  ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit,-99,-99);
  return;
}

/* 004c4d1e auto */
void __cdecl unit_found_city(iptr unit,iptr city,iptr city_name)

{
  int x;
  int y;
  int other_civ;
  
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  if ((game.reveal_map != '\0') || (units[unit].owner == human_civ[0])) {
    mapviews_ensure_visible(x,y,(int)units[unit].owner);
  }
  if ((-1 < city) ||
     (city = city_create(x,y,(int)units[unit].owner), -1 < city)) {
    if (city_name != 0) {
      crt_strcpy((char *)(&cities[city].name),(const char *)city_name);
    }
    unit_delete(unit,1);
    tile_set_improvements(x,y,0x7c,0,1);
    for (other_civ = 1; other_civ < 8; other_civ++) {
      tile_update_known(x,y,other_civ);
    }
    map_redraw_area_all_views(x,y,1,human_civ[0],1);
  }
  return;
}

/* 004c4e6d auto */
void __cdecl unit_goto_step(iptr unit)

{
  byte civ;
  byte old_orders;
  uptr dir;
  int result;
  
  blink.unit_active = 0;
  civ = units[unit].owner;
  old_orders = units[unit].orders;
  pathfind.owner_civ = (int)(char)civ;
  if ((old_orders & 0x10) != 0) {
    pathfind.owner_civ = -1;
  }
  dir = unit_goto_next_dir(unit);
  if (((int)dir < 0) || (dir == 8)) {
    if ((dir != 8) && (units[unit].orders = ORDER_NONE, unit_types[units[unit].type].role == ROLE_SEA_TRANSPORT)
       ) {
      units[unit].counter = (byte)game.turn & 7;
    }
  }
  else {
    result = unit_move_dir(unit,dir,3);
    if (result == 0) {
      return;
    }
    units[unit].flags = units[unit].flags & ~UNITF_GOAL_REACHED;
    if (((units[unit].x == units[unit].goto_x) && (units[unit].y == units[unit].goto_y))
       && ((old_orders & 0x10) == 0)) {
      units[unit].orders = ORDER_NONE;
      units[unit].flags = units[unit].flags | UNITF_GOAL_REACHED;
      if (((1 << (civ & 0x1f) & (uint)game.human_players) == 0) &&
         (((unit_types[units[unit].type].domain == DOMAIN_SEA ||
           (('\t' < (char)units[unit].aux_timer &&
            (result = tile_adjacent_nonallied_unit((int)units[unit].x,(int)units[unit].y,(int)(char)civ
                                       ), result == 0)))) &&
          (unit_exhaust_moves(unit), unit_types[units[unit].type].role == ROLE_SEA_TRANSPORT)))) {
        units[unit].counter = (byte)game.turn & 7;
      }
    }
  }
  pathfind.owner_civ = 0xffffffff;
  return;
}

/* 004c50d0 auto */
void __cdecl unit_pillage(iptr unit,int improvement)

{
  byte civ;
  int x;
  int y;
  iptr city2;
  iptr confirmed;
  int city;
  byte *tile_flags;
  Tile *map_tile;
  
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  civ = units[unit].owner;
  city2 = city_find_nearest(x,y,-1,0xffffffff,-1);
  if ((((city2 < 0) || ((int)cities[city2].owner == (int)(char)civ)) ||
      ((1 << (civ & 0x1f) & (uint)game.human_players) == 0)) ||
     (confirmed = diplo_confirm_attack((int)(char)civ,(int)cities[city2].owner,0xe), confirmed == 0)) {
    map_tile = tile_at(x,y);
    tile_flags = &map_tile->improvements;
    if (improvement < 1) {
      if ((units[unit].type == UNIT_PARTISANS) && ((*tile_flags & 0x10) != 0)) {
        if ((*tile_flags & 0x20) == 0) {
          tile_set_improvements(x,y,0x10,0,1);
        }
        else {
          tile_set_improvements(x,y,0x20,0,1);
        }
      }
      else if ((*tile_flags & 0xc) == 0) {
        if ((*tile_flags & 0x40) == 0) {
          if ((*tile_flags & 0x20) == 0) {
            tile_set_improvements(x,y,0x10,0,1);
          }
          else {
            tile_set_improvements(x,y,0x20,0,1);
          }
        }
        else {
          tile_set_improvements(x,y,0x42,0,1);
        }
      }
      else if ((*tile_flags & 8) == 0) {
        tile_set_improvements(x,y,4,0,1);
      }
      else {
        tile_set_improvements(x,y,8,0,1);
      }
    }
    else {
      tile_set_improvements(x,y,improvement,0,1);
    }
    city = city_find_nearest(x,y,-1,0xffffffff,-1);
    if (-1 < city) {
      tile_update_known(x,y,(int)units[unit].owner);
      tile_update_known(x,y,(int)cities[city].owner);
      diplo_set_treaty_flags((int)units[unit].owner,(int)cities[city].owner,0x2000);
      if (cities[city].owner != units[unit].owner) {
        civ_add_ai_goal((int)units[unit].owner,x,y,ROLE_ATTACK,4);
      }
    }
    map_redraw_area_all_views(x,y,1,human_civ[0],1);
    unit_exhaust_moves(unit);
  }
  return;
}

/* 004c5408 auto */
i32 __cdecl unit_execute_orders(iptr unit)

{
  i32 handled;
  
  handled = 1;
  switch(units[unit].orders) {
  case ORDER_FORTIFYING:
    unit_set_fortified(unit);
    break;
  default:
    handled = 0;
    break;
  case ORDER_BUILD_FORTRESS:
  case ORDER_BUILD_ROAD:
  case ORDER_IRRIGATE:
  case ORDER_MINE:
  case ORDER_TRANSFORM:
  case ORDER_CLEAN_POLLUTION:
  case ORDER_BUILD_AIRBASE:
    unit_do_terrain_work(unit,(int)units[unit].orders);
    break;
  case ORDER_GOTO:
  case ORDER_AI_GOTO:
    unit_goto_step(unit);
  }
  return handled;
}

/* 004c54da auto */
void __cdecl unit_go_home(iptr unit)

{
  byte unit_type;
  byte civ;
  u8 max_range;
  u8 turns_used;
  int x;
  int y;
  int dist;
  int moves_left;
  int home_dist;
  uint road_mult;
  iptr unit2;
  int y3;
  int y2;
  int x3;
  int x2;
  iptr city;
  int landmass2;
  iptr city2;
  int best_dist;
  int carrier_dist;
  
  unit2 = unit;
  carrier_dist = 999;
  unit_type = units[unit].type;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  civ = units[unit].owner;
  landmass2 = -1;
  if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
    landmass2 = tile_landmass(x,y);
  }
  advisor_window_block.nearest_city_dist = 9999;
  city2 = -1;
  for (city = 0; city < game.num_cities; city++) {
    if ((((cities[city].id != 0) && (cities[city].owner == civ)) &&
        ((landmass2 < 0 ||
         (dist = tile_landmass((int)cities[city].x,(int)cities[city].y), dist == landmass2)
         ))) && (dist = map_distance(x,y,(int)cities[city].x,
                                            (int)cities[city].y), dist < advisor_window_block.nearest_city_dist)) {
      city2 = city;
      advisor_window_block.nearest_city_dist = dist;
    }
  }
  if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
    for (unit = 0; unit < game.num_units; unit++) {
      if (((units[unit].id != 0) && (units[unit].owner == civ)) &&
         ((((unit_types[units[unit].type].flags & UTYPE_CARRY_AIR) != 0 ||
           (((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0 &&
            ((unit_types[units[unit].type].flags & UTYPE_SUBMARINE) != 0)))) &&
          (dist = map_distance(x,y,(int)units[unit].x,(int)units[unit].y),
          dist < carrier_dist)))) {
        x3 = (int)units[unit].x;
        y3 = (int)units[unit].y;
        carrier_dist = dist;
      }
    }
    dist = tile_airbase_owner(x,y);
    if (-1 < dist) {
      if ((1 << (civ & 0x1f) & (uint)game.human_players) != 0) {
        return;
      }
      unit_exhaust_moves(unit2);
      return;
    }
  }
  best_dist = 999;
  y2 = y;
  x2 = x;
  if (advisor_window_block.nearest_city_dist < 999) {
    x2 = (int)cities[city2].x;
    y2 = (int)cities[city2].y;
    best_dist = advisor_window_block.nearest_city_dist;
  }
  if (carrier_dist < best_dist) {
    x2 = x3;
    y2 = y3;
    best_dist = carrier_dist;
  }
  if (best_dist < 999) {
    if (((1 << (civ & 0x1f) & (uint)game.human_players) != 0) &&
       (unit_types[units[unit2].type].range != '\0')) {
      max_range = unit_types[units[unit2].type].range;
      turns_used = units[unit2].counter;
      dist = unit_max_moves(unit2);
      moves_left = unit_moves_left(unit2);
      road_mult = (uint)cosmic.road_move_mult;
      home_dist = map_distance(x,y,x2,y2);
      if ((((int)(char)max_range - ((char)turns_used + 1)) * dist + moves_left) / (int)road_mult < home_dist) {
        return;
      }
    }
    if ((x == x2) && (y == y2)) {
      unit_exhaust_moves(unit2);
    }
    else {
      units[unit2].orders = ORDER_GOTO;
      units[unit2].goto_x = (i16)x2;
      units[unit2].goto_y = (i16)y2;
    }
  }
  return;
}

/* 004c5fae auto */
i32 __cdecl spy_try_escape(iptr unit,int mission_mode,iptr civ)

{
  byte civ2;
  int civ3;
  uint rnd;
  int x;
  int y;
  iptr city;
  uint rnd_sign;
  int escape_roll;
  uint escape_odds;
  
  civ2 = units[unit].owner;
  civ3 = (int)(char)civ2;
  if (-1 < mission_mode) {
    units[unit].moves_used += cosmic.road_move_mult;
  }
  if (units[unit].type == UNIT_SPY) {
    escape_odds = (mission_mode < 0) + 2;
    if ((units[unit].flags & UNITF_VETERAN) != 0) {
      escape_odds = escape_odds * 2;
    }
    if (0 < mission_mode) {
      escape_odds = escape_odds / 2;
    }
    if (escape_odds < 2) {
      rnd = _rand();
      rnd_sign = (int)rnd >> 0x1f;
      if (((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) == rnd_sign) {
        escape_odds++;
      }
    }
    if (escape_odds == 1 || (int)(escape_odds - 1) < 0) {
      escape_roll = 0;
    }
    else {
      escape_roll = _rand();
      escape_roll = escape_roll % (int)escape_odds;
    }
    if (escape_roll != 0) {
      x = (int)units[unit].x;
      y = (int)units[unit].y;
      city = city_find_nearest(x,y,civ3,0xffffffff,-1);
      if (-1 < city) {
        unit_relocate(unit,(int)cities[city].x,(int)cities[city].y,1);
        unit_exhaust_moves(unit);
        map_redraw_tile_all_views(x,y);
        map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
        if (2 < game.multiplayer) {
          net_send_state_diff(0xff,2,0,0,0);
          net_send_message(NM_DRAW_SQUARE,0xff,x,y,0,0,0,0,0,0);
          net_send_message(NM_DRAW_SQUARE,0xff,(int)cities[city].x,(int)cities[city].y,0,0,0,0,0,0);
        }
        text_set_string_arg(1,(const char *)&cities[city].name);
      }
      if (-1 < mission_mode) {
        if ((autoplay.active == 0) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
          if ((units[unit].flags & UNITF_VETERAN) == 0) {
            if (human_civ[0] == civ3) {
              popup_game_with_sprite((const char *)&str_bond007,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),8
                                );
            }
            else if (2 < game.multiplayer) {
              net_send_text_args(0x26,net.net_slots[net.net_civ_slots[civ3]].id,
                                 2,0,(uint)units[unit].type,8);
            }
          }
          else if (human_civ[0] == civ3) {
            popup_game_with_sprite((const char *)&str_bond,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),8);
          }
          else if (2 < game.multiplayer) {
            net_send_text_args(0x27,net.net_slots[net.net_civ_slots[civ3]].id,2,
                               0,(uint)units[unit].type,8);
          }
        }
        units[unit].flags = units[unit].flags | UNITF_VETERAN;
      }
      return 0;
    }
    if (((autoplay.active == 0) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
       (-1 < mission_mode)) {
      if (human_civ[0] == civ3) {
        popup_game_with_sprite((const char *)&str_bondglory,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),8);
      }
      else if (game.multiplayer == 3) {
        net_send_text_args(0x28,net.net_slots[net.net_civ_slots[civ3]].id,0,0,
                           (uint)units[unit].type,8);
      }
    }
  }
  unit_destroy_redraw(unit,1);
  if ((0 < civ) && (-1 < mission_mode)) {
    diplo_espionage_incident(civ3,civ);
  }
  return 1;
}

/* 004c64aa auto */
i32 __cdecl spy_check_caught(iptr unit,iptr civ)

{
  byte civ2;
  byte unit_type;
  int escape_result;
  i32 result;
  
  civ2 = units[unit].owner;
  unit_type = units[unit].type;
  escape_result = spy_try_escape(unit,0xffffffff,civ);
  if (escape_result == 0) {
    result = 0;
  }
  else {
    if ((autoplay.active == 0) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
      popup_game_with_sprite((const char *)&str_nailed,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)unit_type * 0x3c),8);
    }
    result = 1;
  }
  return result;
}

/* 004c654d auto */
uint __cdecl spy_confirm_treaty_breach(iptr civ,iptr target_civ)

{
  uint result;
  int choice;
  
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    result = 0;
  }
  else if ((*(byte *)&civs[civ].treaties[target_civ] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0) {
    result = 0;
  }
  else {
    choice = popup_game((const char *)&str_incident);
    result = (uint)(choice != 1);
  }
  return result;
}

/* Part of spy_city_mission (004c6bf5): a human player's choice of mission, from
   the menu. Returns 1 when cancelled. */
static int spy_city_mission_menu(Dialog &dlg,iptr unit,iptr city,uptr civ,uptr civ2,uint is_spy,
                                            int &choice)
{
  iptr unit2;
  int ret;
  iptr option_id;
  i32 option_flags;
  unit2 = civ_get_adjective(civ);
  text_set_string_arg(0,(const char *)unit2);
  text_set_label_arg(1,unit_types[units[unit].type].name);
  text_set_string_arg(2,(const char *)&cities[city].name);
  dlg.load_game_section((const char *)&str_spymenu,1);
  dlg.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)
             ,0,0);
  dlg.set_image_zoom(8);
  ret = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_spyoptions);
  if (ret != 0) {
    return 1;
  }
  text_read_line(1);
  if (((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) == 0) &&
     (ret = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), ret == 0)) {
    if (is_spy != 0) {
      str_append_space((iptr)&textbuf.buf);
      str_append_lparen((iptr)&textbuf.buf);
      text_append_label(0xed);
      str_append_rparen((iptr)&textbuf.buf);
    }
    dlg.add_option((const char *)&textbuf.buf,0,0);
  }
  text_read_line(1);
  if (is_spy != 0) {
    str_append_space((iptr)&textbuf.buf);
    str_append_lparen((iptr)&textbuf.buf);
    text_append_label(0xed);
    str_append_rparen((iptr)&textbuf.buf);
  }
  dlg.add_option((const char *)&textbuf.buf,1,0);
  text_read_line(1);
  if (((cities[city].flags & CITYF_TECH_STOLEN) == 0) || (is_spy != 0)) {
    dlg.add_option((const char *)&textbuf.buf,2,0);
  }
  text_read_line(1);
  dlg.add_option((const char *)&textbuf.buf,3,0);
  text_read_line(1);
  if ((is_spy != 0) && (1 < cities[city].size)) {
    dlg.add_option((const char *)&textbuf.buf,4,0);
  }
  text_read_line(1);
  if ((((is_spy != 0) && (ret = civ_has_advance(civ,ADV_ROCKETRY), ret != 0)) &&
      (ret = civ_has_advance(civ,ADV_NUCLEAR_FISSION), ret != 0)) &&
     (ret = wonder_get_city(WONDER_MANHATTAN), ret != -1)) {
    dlg.add_option((const char *)&textbuf.buf,5,0);
  }
  text_read_line(1);
  ret = city_has_improvement(city,IMP_PALACE);
  if (ret == 0) {
    dlg.add_option((const char *)&textbuf.buf,6,0);
  }
  if (((2 < game.multiplayer) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
     && ((ret = city_has_improvement(city,IMP_PALACE), ret != 0 &&
         ((*(byte *)(civ * 0x594 + (iptr)((u8 *)&civs + 35) + civ2 * 4) & 1) == 0)))) {
    option_flags = 0;
    option_id = 7;
    unit2 = (iptr)label_string(label_table.string_ids[0x34a]);
    dlg.add_option((const char *)unit2,option_id,option_flags);
  }
  text_close_file();
  choice = dlg.show_modal(0);
  if (choice < 0) {
    return 1;
  }
  return 0;
}

/* Part of spy_city_mission (004c6bf5): an AI's choice of mission. */
static int spy_city_mission_ai_choice(iptr city,uptr civ,uptr civ2,uint is_spy)
{
  int ret;
  int choice;
  choice = 2;
  if ((((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0) &&
      (ret = crt_rand_at("spy_city_mission"), ret % 3 == 0)) && (is_spy != 0)) {
    choice = 4;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) {
    choice = 2;
  }
  if ((((DIFF_KING < game.difficulty) || ((game.difficulty == DIFF_KING && (5 < civs[civ2].subversion_count))))
      && (choice == 2)) &&
     (((civs[civ].treaties[civ2] & (TREATY_VENDETTA | TREATY_WAR)) != 0 &&
      (civs[civ2].government != GOV_DEMOCRACY)))) {
    choice = 6;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    choice = 2;
  }
  if (((((cities[city].flags & CITYF_TECH_STOLEN) != 0) && (is_spy == 0)) && (choice == 2)) &&
     (choice = 3, (*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) != 0)) {
    choice = 0;
  }
  return choice;
}

/* Part of spy_city_mission (004c6bf5): establish an embassy (once; otherwise the
   city is investigated). Returns 1 to go on with the mission in choice. */
static int spy_city_mission_embassy(iptr unit,uptr civ,uptr civ2,uint is_spy,uint unit_type3,
                                               int &choice)
{
  uint *treaty_ptr;
  iptr unit2;
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_EMBASSY) == 0) {
    treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
    *treaty_ptr = *treaty_ptr | 0x80;
    if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
      snd_play_effect(SND_SPYSOUND,0,0,0);
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      unit_destroy_redraw(unit,1);
    }
    else {
      intel_report_show(civ,civ2);
      if (is_spy == 0) {
        unit_delete(unit,1);
      }
      else {
        units[unit].moves_used++;
      }
    }
    if ((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
      unit2 = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)unit2);
      if ((game.multiplayer < 3) || (human_civ[0] == civ2)) {
        popup_game_with_sprite((const char *)&str_enemyembassy,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8);
      }
      else {
        net_send_text_args(0x55,net.net_slots[net.net_civ_slots[civ2]].id,1
                           ,0,0,0);
      }
    }
    return 0;
  }
  choice = 1;
  return 1;
}

/* Part of spy_city_mission (004c6bf5): investigate the city. */
static void spy_city_mission_investigate(iptr unit,iptr city,uptr civ,uptr civ2,uint is_spy,
                                                    uint unit_type3)
{
  iptr unit2;
  if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
    snd_play_effect(SND_SPYSOUND,1,0,0);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    unit_destroy_redraw(unit,1);
  }
  else {
    ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
    if (is_spy == 0) {
      spy_try_escape(unit,0,-1);
    }
    else if ((cities[city].flags & CITYF_INVESTIGATED) == 0) {
      units[unit].moves_used++;
    }
    cities[city].flags = cities[city].flags | CITYF_INVESTIGATED;
  }
  if ((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
    unit2 = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)unit2);
    text_set_string_arg(1,(const char *)&cities[city].name);
    if ((game.multiplayer < 3) || (human_civ[0] == civ2)) {
      popup_game_with_sprite((const char *)&str_enemyinvestigate,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8);
    }
    else {
      net_send_text_args(0x56,net.net_slots[net.net_civ_slots[civ2]].id,2,0
                         ,0,0);
    }
  }
}

/* Part of spy_city_mission (004c6bf5): steal an advance (a city once, a spy may
   choose). Returns 1 to go on with the mission in choice (an AI with nothing
   to steal sabotages or establishes an embassy instead). */
static int spy_city_mission_steal_tech(iptr unit,iptr city,uptr civ,uptr civ2,uint is_spy,
                                                  uint unit_type3,int &choice)
{
  iptr unit2;
  int ret;
  iptr option_id;
  int i;
  int detect_chance;
  uptr unit_type;
  iptr advance2;
  int rand_start;
  int escape_mode;
  iptr unit3;
  iptr advance;
  int detected;
  ret = spy_confirm_treaty_breach(civ,civ2);
  if (ret != 0) return 0;
  advance = -1;
  ret = crt_rand_at("spy_city_mission");
  rand_start = ret % 100;
  for (i = 0; i < 100; i++) {
    advance2 = (rand_start + i) % 100;
    if ((((advances[advance2].preq[0] != -2) || (advances[advance2].preq[1] != -2)) &&
        (ret = civ_has_advance(civ,advance2), ret == 0)) &&
       (ret = civ_has_advance(civ2,advance2), ret != 0)) {
      advance = advance2;
      ret = spaceship_race_active();
      if ((((ret == 0) ||
           ((game.difficulty < DIFF_PRINCE &&
            ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)))) ||
          (improvements[IMP_SS_STRUCTURAL].preq == advance2)) ||
         (((improvements[IMP_SS_COMPONENT].preq == advance2 || (improvements[IMP_SS_MODULE].preq == advance2)) ||
          (advance2 == ADV_FUSION_POWER)))) break;
    }
  }
  if (-1 < advance) {
    escape_mode = 0;
    if ((cities[city].flags & CITYF_TECH_STOLEN) == 0) {
      if (((autoplay.active == 0) && (is_spy != 0)) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        choice = popup_game_sprite_flags((const char *)&str_stealspecific,1,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8);
        if (choice < 0) return 0;
        if (choice == 1) {
          ret = spy_check_caught(unit,civ2);
          if (ret == 0) {
            if (human_civ[0] == civ) {
              snd_play_effect(SND_SPYSOUND,1,0,0);
            }
            civ_steal_advance(civ,civ2);
            spy_try_escape(unit,1,civ2);
            cities[city].flags = cities[city].flags | CITYF_TECH_STOLEN;
          }
          return 0;
        }
      }
    }
    else {
      if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)
         ) {
        unit2 = (iptr)(civ_name_plural(civ2));
        text_set_string_arg(0,(const char *)unit2);
        text_set_string_arg(1,(const char *)&cities[city].name);
        choice = popup_game_with_city((const char *)&str_stealhard,city);
        if (choice != 1) return 0;
      }
      ret = spy_check_caught(unit,civ2);
      if (ret != 0) return 0;
      escape_mode = 1;
    }
    unit3 = unit;
    detected = 0;
    for (unit = tile_first_unit((int)cities[city].x,(int)cities[city].y); -1 < unit;
        unit = unit_next_in_stack(unit)) {
      if (unit_types[units[unit].type].role == ROLE_DIPLOMACY) {
        detect_chance = 0x14;
        if ((units[unit].type == UNIT_SPY) && (detect_chance = 0x28, (units[unit].flags & UNITF_VETERAN) != 0)) {
          detect_chance = 0x3c;
        }
        ret = crt_rand_at("spy_city_mission");
        if (ret % 100 < detect_chance) {
          detected = 1;
          unit_type = (uptr)units[unit].type;
          break;
        }
      }
    }
    unit2 = unit3;
    if (detected == 0) {
      option_id = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)option_id);
      text_set_label_arg(1,advances[advance].name);
      if ((((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
          && (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
        net_send_text_args(0x57,net.net_slots[net.net_civ_slots[civ2]].id,2,0,0
                           ,0);
      }
      if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        snd_play_effect(SND_SPYSOUND,1,0,0);
        popup_game_with_sprite((const char *)&str_steal,(iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                                          (char)advances[advance].epoch * 0x3c +
                                          (char)advances[advance].category * 0xf0),8);
      }
      civ_give_advance(civ,advance,civ2,0,0);
      cities[city].flags = cities[city].flags | CITYF_TECH_STOLEN;
      spy_try_escape(unit2,escape_mode,civ2);
    }
    else {
      unit_exhaust_moves(unit3);
      map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
      text_set_label_arg(1,unit_types[unit_type].name);
      unit2 = civ_get_adjective(civ);
      text_set_string_arg(2,(const char *)unit2);
      if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        snd_play_effect(SND_SPYSOUND,1,0,0);
        popup_game_with_sprite((const char *)&str_curses,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c),8);
      }
      if ((((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
          && (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
        net_send_text_args(0x58,net.net_slots[net.net_civ_slots[civ2]].id,3,0,0
                           ,0);
      }
    }
    return 0;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    units[unit].moves_used += cosmic.road_move_mult;
    if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
    {
      popup_game_with_sprite((const char *)&str_nosteal,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8);
    }
    return 0;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0) {
    choice = 3;
  }
  else {
    choice = 0;
  }
  return 1;
}

/* Part of spy_city_mission (004c6bf5): sabotage production or a building (a spy
   may choose which). */
static void spy_city_mission_sabotage(Dialog &dlg,iptr unit,iptr city,uptr civ,uptr civ2,uint is_spy,
                                                 uint unit_type3,int &choice)
{
  iptr unit2;
  int ret;
  i32 option_flags;
  iptr impr_option;
  int sabotage_roll;
  iptr improvement;
  int rand_start;
  int escape_mode;
  int candidate;
  iptr improvement2;
  i8 build_name[80];
  int odds;
  escape_mode = 0;
  ret = spy_confirm_treaty_breach(civ,civ2);
  if (ret != 0) return;
  str_clear((char *)build_name);
  if (cities[city].building < '\0') {
    str_append_string_id((iptr)build_name,improvements[-(int)cities[city].building].name);
  }
  else {
    str_append_string_id((iptr)build_name,unit_types[cities[city].building].name);
  }
  improvement2 = 0;
  rand_start = crt_rand_at("spy_city_mission");
  rand_start = rand_start % 0x27;
  for (improvement = 0; improvement < 0x27; improvement++) {
    candidate = (improvement + rand_start) % 0x27;
    if (((candidate != 0) && (candidate != 1)) &&
       ((candidate != 0x11 && (ret = city_has_improvement(city,candidate), ret != 0)))) {
      improvement2 = candidate;
      break;
    }
  }
  if ((((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
      (is_spy != 0)) && (0 < improvement2)) {
    choice = popup_game_sprite_flags((const char *)&str_sabotagespecific,1,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8);
    if (choice < 0) return;
    if (choice == 1) {
      dlg.load_game_section((const char *)&str_sabotage,1);
      dlg.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),0,0);
      dlg.set_image_zoom(8);
      for (improvement = 1; improvement < 0x27; improvement++) {
        if ((improvement != IMP_PALACE) && (ret = city_has_improvement(city,improvement), ret != 0)) {
          option_flags = 0;
          impr_option = improvement;
          unit2 = (iptr)label_string(improvements[improvement].name);
          dlg.add_option((const char *)unit2,impr_option,option_flags);
        }
      }
      text_clear();
      text_append((const char *)build_name);
      text_append_space();
      text_append_lparen();
      text_append_label(0x78);
      text_append_rparen();
      dlg.add_option((const char *)&textbuf.buf,0,0);
      improvement2 = dlg.show_modal(0);
      /* BF-231 (Peter Johnson's ledger): a choice past the improvements (a
         row added to GAME.TXT's @SABOTAGE) is no choice (the original took
         it as an improvement past the 39) */
      if ((improvement2 < 0) || (0x26 < improvement2)) return;
      if (improvement2 == IMP_SDI_DEFENSE) {
        popup_game_with_improvement((const char *)&str_sabotageno,IMP_SDI_DEFENSE,8);
        return;
      }
      ret = city_has_improvement(city,IMP_PALACE);
      if (((((ret != 0) || (improvement2 == IMP_CITY_WALLS)) &&
           (choice = popup_game_with_sprite((const char *)&str_sabotagehard,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8),
           choice != 1)) ||
          ((ret = spy_check_caught(unit,civ2), ret != 0 ||
           ((ret = city_has_improvement(city,IMP_PALACE), ret != 0 &&
            (ret = spy_check_caught(unit,civ2), ret != 0)))))) ||
         (((improvement2 == IMP_CITY_WALLS && (ret = spy_check_caught(unit,civ2), ret != 0)) ||
          ((improvement2 == IMP_SDI_DEFENSE &&
           ((ret = spy_check_caught(unit,civ2), ret != 0 ||
            (ret = spy_check_caught(unit,civ2), ret != 0))))))))
      return;
      escape_mode = 1;
    }
  }
  if ((escape_mode == 0) && (cities[city].shield_box != 0)) {
    odds = 2;
    if (is_spy != 0) {
      odds = 3;
    }
    if ((units[unit].flags & UNITF_VETERAN) != 0) {
      odds++;
    }
    if (odds - 1 < 1) {
      sabotage_roll = 0;
    }
    else {
      sabotage_roll = crt_rand_at("spy_city_mission");
      sabotage_roll = sabotage_roll % odds;
    }
    if (sabotage_roll == 0) {
      improvement2 = 0;
    }
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
     ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
    if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
      ret = civ_has_advance(civ,ADV_GUNPOWDER);
      if ((ret == 0) || (human_civ[0] != civ)) {
        snd_play_effect(SND_SPYSOUND,1,0,0);
      }
      else {
        snd_play_effect(SND_MEDEXPL,1,0,0);
      }
    }
    if (improvement2 == IMP_NOTHING) {
      cities[city].shield_box = 0;
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city);
      text_set_string_arg(0,(const char *)build_name);
      if (((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
         && ((2 < game.multiplayer && (human_civ[0] != civ2)))) {
        net_send_text_args(0x5a,net.net_slots[net.net_civ_slots[civ2]].id,1,0
                           ,civ,0);
      }
      if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
      {
        popup_game((const char *)&str_sabotagetwo);
      }
    }
    else {
      city_set_improvement(city,improvement2,0);
      map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
      text_set_label_arg(0,improvements[improvement2].name);
      net_send_state_diff(0xff,2,0,0,0);
      if ((((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
          && (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
        net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ2]].id,
                           (int)cities[city].x,(int)cities[city].y,0,0,0,0,0,0);
        net_send_message(NM_CITYWIN_REDRAW,net.net_slots[net.net_civ_slots[civ2]].id,0,0
                           ,0,0,0,0,0,0);
        net_send_text_args(0x59,net.net_slots[net.net_civ_slots[civ2]].id,1,0
                           ,improvement2,civ);
      }
      if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
      {
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city);
        popup_game_with_sprite((const char *)&str_sabotageone,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c),8);
      }
    }
  }
  spy_try_escape(unit,escape_mode,civ2);
}

/* Part of spy_city_mission (004c6bf5): poison the water supply. */
static void spy_city_mission_poison_water(iptr unit,iptr city,uptr civ,uptr civ2,uint unit_type3)
{
  iptr unit2;
  int ret;
  uint rnd;
  uint rnd_sign;
  uptr civ4;
  ret = spy_confirm_treaty_breach(civ,civ2);
  if (ret == 0) {
    if (cities[city].size < 2) {
      cities[city].food_box = 0;
    }
    else {
      cities[city].size--;
    }
    city_reveal_to_civ(city,civ);
    map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
    text_set_string_arg(0,(const char *)&cities[city].name);
    unit2 = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)unit2);
    if ((((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0))
        && (2 < game.multiplayer)) && (human_civ[0] != civ2)) {
      net_send_text_args(0x5b,net.net_slots[net.net_civ_slots[civ2]].id,2,0
                         ,0,0);
    }
    if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
    {
      snd_play_effect(SND_SPYSOUND,1,0,0);
      popup_game_with_sprite((const char *)&str_watersupply,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8);
    }
    civ4 = civ2;
    rnd = crt_rand_at("spy_city_mission");
    rnd_sign = (int)rnd >> 0x1f;
    spy_try_escape(unit,((rnd ^ rnd_sign) - rnd_sign & 1 ^ rnd_sign) - rnd_sign,civ4);
  }
}

/* Part of spy_city_mission (004c6bf5): plant a nuclear device (the other civs may
   find out). modifier_table[15] notes whether they did. */
static void spy_city_mission_plant_nuke(iptr unit,iptr city,uptr civ,uptr civ2,uint unit_type3,
                                                   int *modifier_table)
{
  iptr unit2;
  int ret;
  int nuke_roll;
  uptr civ3;
  int odds;
  if (((((autoplay.active == 0) &&
        (ret = popup_game_with_sprite((const char *)&str_majorincident,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8),
        ret == 1)) && (ret = spy_check_caught(unit,civ2), ret == 0)) &&
      ((ret = spy_check_caught(unit,civ2), ret == 0 &&
       (ret = spy_check_caught(unit,civ2), ret == 0)))) &&
     ((ret = city_has_improvement(city,IMP_PALACE), ret == 0 ||
      (ret = spy_check_caught(unit,civ2), ret == 0)))) {
    unit_nuclear_attack(civ,(int)cities[city].x,(int)cities[city].y,0);
    text_set_string_arg(0,(const char *)&cities[city].name);
    unit2 = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)unit2);
    unit2 = (iptr)(civ_name_plural(civ));
    text_set_string_arg(2,(const char *)unit2);
    text_clear();
    text_append((const char *)&str_plantednuke);
    modifier_table[0xf] = 0;
    if (civs[civ].government != GOV_FUNDAMENTALISM) {
      odds = 2;
      if ((units[unit].flags & UNITF_VETERAN) != 0) {
        odds = 4;
      }
      if (odds == 1 || odds - 1 < 0) {
        nuke_roll = 0;
      }
      else {
        nuke_roll = crt_rand_at("spy_city_mission");
        nuke_roll = nuke_roll % odds;
      }
      if (nuke_roll == 0) {
        text_append((const char *)&str_2_2);
        modifier_table[0xf] = 1;
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if (civ != civ3) {
            civ_adjust_attitude(civ3,civ,100);
            diplo_set_treaty_flags(civ,civ3,0x2000);
          }
        }
      }
    }
    popup_game_with_improvement((const char *)&textbuf.buf,IMP_MANHATTAN,8);
    if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) && (2 < game.multiplayer)
       ) {
      if (modifier_table[0xf] == 0) {
        net_send_text_args(0x5c,net.net_slots[net.net_civ_slots[civ2]].id,3
                           ,0,0,0);
      }
      else {
        net_send_text_args(0x5d,net.net_slots[net.net_civ_slots[civ2]].id,3
                           ,0,0,0);
      }
    }
  }
}

/* Part of spy_city_mission (004c6bf5): incite a revolt (the price asked, paid).
   Returns 1 to go on with the mission in choice (an AI that cannot afford it
   steals an advance instead). */
static int spy_city_mission_incite_revolt(Dialog &dlg,iptr unit,iptr city,uptr civ,uptr civ2,
                                                     uint is_spy,int &choice)
{
  uint *treaty_ptr;
  int ret;
  bool unit_type2;
  int capital_dist;
  int incite_cost;
  int double_cost;
  uint has_treaty;
  if (civs[civ2].government == GOV_DEMOCRACY) {
    if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
    {
      popup_game_with_sprite((const char *)&str_norevolt,(iptr)&government_sprites_block.government_sprite_democracy,0);
    }
    return 0;
  }
  capital_dist = civ_distance_to_capital(civ2,(int)units[unit].x,(int)units[unit].y);
  if (capital_dist < 2) {
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) return 0;
    choice = 2;
  }
  else {
    if ((civs[civ2].government == GOV_COMMUNISM) && (9 < capital_dist)) {
      capital_dist = 10;
    }
    ret = city_has_improvement(city,IMP_COURTHOUSE);
    if (ret != 0) {
      capital_dist = capital_dist / 2;
    }
    incite_cost = (int)cities[city].size * ((civs[civ2].gold + 1000) / (capital_dist + 3));
    if (incite_cost < 0) {
      incite_cost = 30000;
    }
    if ((cities[city].flags & CITYF_DISORDER) != 0) {
      incite_cost = incite_cost / 2;
    }
    ret = tile_unit_owner((int)cities[city].x,(int)cities[city].y);
    if (ret < 0) {
      incite_cost = incite_cost / 2;
    }
    if ((int)cities[city].founder == civ) {
      incite_cost = incite_cost / 2;
    }
    if (is_spy != 0) {
      if ((units[unit].flags & UNITF_VETERAN) == 0) {
        incite_cost -= incite_cost / 6;
      }
      else {
        incite_cost -= incite_cost / 3;
      }
    }
    unit_type2 = (*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0;
    if (unit_type2) {
      has_treaty = 0;
    }
    else {
      has_treaty = 1;
    }
    has_treaty = (uint)!unit_type2;
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) ||
       (double_cost = incite_cost * 2,
        ((double_cost <= civs[civ].gold) && (has_treaty == 0)) && (-1 < double_cost))) {
      if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)
         ) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        text_set_number_arg(0,incite_cost);
        dlg.load_game_text((const char *)&str_dissidents);
        if (incite_cost <= civs[civ].gold) {
          ret = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_dissidentoptions);
          if (ret != 0) return 0;
          text_read_line(1);
          dlg.add_option((const char *)&textbuf.buf,0,0);
          text_read_line(1);
          if (has_treaty == 0) {
            dlg.add_option((const char *)&textbuf.buf,1,0);
          }
          text_read_line(1);
          if (has_treaty != 0) {
            dlg.add_option((const char *)&textbuf.buf,1,0);
          }
          text_read_line(1);
          if (((has_treaty != 0) && (incite_cost * 2 <= civs[civ].gold)) && (incite_cost < 0x3a99)
             ) {
            dlg.add_option((const char *)&textbuf.buf,2,0);
          }
          text_close_file();
        }
        choice = dlg.show_modal(0);
      }
      else {
        choice = 1;
      }
      if (0 < choice) {
        civs[civ].gold -= incite_cost * choice;
        civs[civ].subversion_count += 2;
        if (human_civ[0] == civ) {
          status_draw_info(1);
        }
        spy_try_escape(unit,0,0);
        if (choice == 1) {
          diplo_espionage_incident(civ,civ2);
        }
        if (choice == 2) {
          treaty_ptr = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
          *treaty_ptr = *treaty_ptr | 0x10;
          cities[city].founder = (byte)civ;
        }
        if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
          snd_play_effect(SND_SPYSOUND,1,0,0);
        }
        city_incite_transfer(city,civ,choice);
      }
      return 0;
    }
    choice = 2;
  }
  return 1;
}

/* Part of spy_city_mission (004c6bf5): a multiplayer spy's talk with the city's
   owner: its morale (veteran, difficulty, attitude) sets how long. */
static void spy_city_mission_chat(iptr unit,uptr civ,uptr civ2,uint is_spy,uint unit_type3,
                                             int *modifier_table)
{
  uint *treaty_ptr;
  u8 morale;
  int ret;
  if ((human_civ[0] == civ) || (human_civ[0] == civ2)) {
    snd_play_effect(SND_SPYSOUND,0,0,0);
  }
  modifier_table[9] = 5;
  modifier_table[10] = 4;
  modifier_table[0xb] = 3;
  modifier_table[0xc] = 2;
  modifier_table[0xd] = 1;
  modifier_table[0xe] = 0;
  modifier_table[0] = 0;
  modifier_table[1] = 0;
  modifier_table[2] = 0;
  modifier_table[3] = 0;
  modifier_table[4] = 1;
  modifier_table[5] = 2;
  modifier_table[6] = 2;
  modifier_table[7] = 3;
  modifier_table[8] = 4;
  if (is_spy == 0) {
    units[unit].aux_timer = 5;
  }
  else {
    units[unit].aux_timer = '\n';
  }
  units[unit].aux_timer = (char)modifier_table[game.difficulty + 9] + units[unit].aux_timer;
  if ((units[unit].flags & UNITF_VETERAN) != 0) {
    units[unit].aux_timer += 2;
  }
  /* (BF-289) the table has 15 entries and attitudes go to 100: the original
     read its caller's frame beyond them; 0, the table's last value, here */
  units[unit].aux_timer =
       units[unit].aux_timer -
       (char)((uint)civs[civ2].attitude[civ] < 0xf ? modifier_table[civs[civ2].attitude[civ]] : 0);
  morale = units[unit].aux_timer;
  ret = crt_rand_at("spy_city_mission");
  units[unit].aux_timer = morale + (char)(ret % 6);
  units[unit].counter = (byte)civ2;
  units[unit].flags = units[unit].flags | UNITF_SPY_ON_MISSION;
  treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
  *treaty_ptr = *treaty_ptr | 0x1000000;
  popup_game_with_sprite((const char *)&str_chatspystart,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c),8);
}

/* 004c6bf5 auto */
void __cdecl spy_city_mission(iptr unit,iptr city)

{
  bool unit_type2;
  int modifier_table[16];
  uptr civ;
  int choice;
  uint unit_type3;
  uint is_spy;
  Dialog dlg;
  uptr civ2;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  blink.unit_active = 0;
  civ = (uptr)units[unit].owner;
  civ2 = (uptr)cities[city].owner;
  unit_type3 = (uint)units[unit].type;
  diplo_set_treaty_flags(civ,civ2,1);
  unit_type2 = units[unit].type != UNIT_SPY;
  if (unit_type2) {
    is_spy = 0;
  }
  else {
    is_spy = 1;
  }
  is_spy = (uint)!unit_type2;
  if (((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
     (human_civ[0] == civ)) {
    if (spy_city_mission_menu(dlg,unit,city,civ,civ2,is_spy,choice) != 0) {
      return;
    }
  }
  else {
    choice = spy_city_mission_ai_choice(city,civ,civ2,is_spy);
  }
  for (;;) {
    spy_last_mission = choice;
    switch(choice) {
    case 0:
      if (spy_city_mission_embassy(unit,civ,civ2,is_spy,unit_type3,choice) != 0) continue;
      break;
    case 1:
      spy_city_mission_investigate(unit,city,civ,civ2,is_spy,unit_type3);
      break;
    case 2:
      if (spy_city_mission_steal_tech(unit,city,civ,civ2,is_spy,unit_type3,choice) != 0) continue;
      break;
    case 3:
      spy_city_mission_sabotage(dlg,unit,city,civ,civ2,is_spy,unit_type3,choice);
      break;
    case 4:
      spy_city_mission_poison_water(unit,city,civ,civ2,unit_type3);
      break;
    case 5:
      spy_city_mission_plant_nuke(unit,city,civ,civ2,unit_type3,modifier_table);
      break;
    case 6:
      if (spy_city_mission_incite_revolt(dlg,unit,city,civ,civ2,is_spy,choice) != 0) continue;
      break;
    case 7:
      spy_city_mission_chat(unit,civ,civ2,is_spy,unit_type3,modifier_table);
    default:
      break;
    }
    break;
  }
  return;
}

/* Part of unit_bribe (004c9528): the briber accepts the price (a human
   player asked, here or over the network; an AI pays at most half its
   gold). Returns 0 when it does not. */
static int unit_bribe_accept(iptr unit,iptr civ,int bribe_cost)
{
  int now;
  int city;
  int answer;
  if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
    text_set_number_arg(0,bribe_cost);
    text_clear();
    text_append((const char *)&str_desert);
    if (human_civ[0] == civ) {
      if (bribe_cost <= civs[civ].gold) {
        text_append((const char *)&str_2_3);
      }
      answer = dlg_game_txt_with_unit((const char *)&textbuf.buf,unit);
      if (answer != 1) {
        return 0;
      }
    }
    else if (!(game.multiplayer < 3)) {
      if (civs[civ].gold < bribe_cost) {
        net_send_text_args(0x23,net.net_slots[net.net_civ_slots[civ]].id,2,1,
                           human_civ[0],unit);
      }
      else {
        net_send_text_args(0x24,net.net_slots[net.net_civ_slots[civ]].id,2,1,
                           human_civ[0],unit);
      }
      (*(int *)&font_size16_bold_block.net_bribe_answer) = -1;
      city = timer_ticks();
      while ((now = timer_ticks(), answer = (*(int *)&font_size16_bold_block.net_bribe_answer),
             now - city < net.timeout_seconds * 0x3c && ((*(int *)&font_size16_bold_block.net_bribe_answer) == -1))) {
        net.process_messages(1,0);
      }
      if (answer != 1) {
        return 0;
      }
    }
  }
  else if (civs[civ].gold / 2 < bribe_cost) {
    return 0;
  }
  return 1;
}

/* Part of unit_bribe (004c9528): the unit changes sides (here, or on the
   host for a multiplayer client, waited for). */
static void unit_bribe_transfer(iptr unit,iptr civ,iptr civ2,int bribe_cost)
{
  char *unit_count_ptr;
  int city;
  int answer;
  i8 new_home;
  if ((game.multiplayer < 3) || (net.host_flag != '\0')) {
    civs[civ].gold -= bribe_cost;
    unit_count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
    *unit_count_ptr = *unit_count_ptr - 1;
    unit_count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
    *unit_count_ptr = *unit_count_ptr + 1;
    civs[civ].subversion_count++;
    units[unit].owner = (byte)civ;
    units[unit].home_city = -1;
    units[unit].moves_used = '\0';
    units[unit].orders = ORDER_NONE;
    city = city_find_nearest((int)units[unit].x,(int)units[unit].y,-1,0xffffffff,-1);
    if ((-1 < city) && (cities[city].owner == civ)) {
      new_home = (i8)city;
      units[unit].home_city = new_home;
    }
    unit_set_seen_by(unit,civ2);
    tile_set_owner((int)units[unit].x,(int)units[unit].y,civ,1);
    net_send_state_diff(0xff,2,0,0,1);
  }
  else {
    net_send_message(NM_BRIBED_UNIT,0,unit,civ,bribe_cost,0,0,0,0,0);
    city = timer_ticks();
    while ((units[unit].owner != civ && (answer = timer_ticks(), answer - city < 0xe10)
           )) {
      net.process_messages(1,1);
    }
    if (units[unit].owner != civ) {
      dbg_log((const char *)&str_pick_up_unit_connection_to_server_timed);
      popup_game_with_sprite((const char *)&str_serverconnecttime_5,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
  }
}

/* 004c9528 auto */
void __cdecl unit_bribe(iptr unit,iptr civ)

{
  byte civ3;
  iptr civ2;
  iptr stack_count;
  iptr has_adv;
  iptr name_str;
  int bribe_cost;
  int capital_dist;
  
  civ3 = units[unit].owner;
  civ2 = (iptr)(char)civ3;
  stack_count = unit_stack_sum(unit,2);
  if (1 < stack_count) {
    return;
  }
  if (civs[civ2].government == GOV_DEMOCRACY) {
    if (autoplay.active != 0) {
      return;
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      return;
    }
    if ((2 < game.multiplayer) && (human_civ[0] != civ)) {
      net_send_text_args(0x21,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,
                         0);
      return;
    }
    popup_game_with_sprite((const char *)&str_incorruptible,(iptr)&government_sprites_block.government_sprite_democracy,0);
    return;
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
     (has_adv = civ_has_advance(civ,(int)unit_types[UNIT_DIPLOMAT].prerequisite), has_adv == 0)) {
    return;
  }
  capital_dist = civ_distance_to_capital(civ2,(int)units[unit].x,(int)units[unit].y);
  if ((civs[civ].government == GOV_COMMUNISM) && (9 < capital_dist)) {
    capital_dist = 10;
  }
  bribe_cost = (int)(char)unit_types[units[unit].type].cost *
             ((civs[civ2].gold + 0x2ee) / (capital_dist + 2));
  if (bribe_cost < 0) {
    bribe_cost = 30000;
  }
  if (unit_types[units[unit].type].role != ROLE_SETTLE) {
    bribe_cost = bribe_cost / 2;
  }
  if ((unit_types[units[unit].type].role == ROLE_TRADE) &&
     ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
    return;
  }
  name_str = civ_get_adjective(civ2);
  text_set_string_arg(0,(const char *)name_str);
  text_set_label_arg(1,unit_types[units[unit].type].name);
  if (unit_bribe_accept(unit,civ,bribe_cost) == 0) {
    return;
  }
  unit_bribe_transfer(unit,civ,civ2,bribe_cost);
  if ((human_civ[0] == civ2) || (human_civ[0] == civ)) {
    snd_play_effect(SND_SPYSOUND,1,0,0);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    if (human_civ[0] == civ) {
      status_draw_info(1);
    }
    else if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      net_send_message(NM_SOUND_PLAY,net.net_slots[net.net_civ_slots[civ]].id,0x44,1
                         ,0,0,0,0,0,0);
      net_send_message(NM_INFO_STATUS,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,
                         0,0,0,0,0);
    }
  }
  if ((autoplay.active == 0) && ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) {
    name_str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(2,(const char *)name_str);
    if (human_civ[0] == civ2) {
      mapviews_ensure_visible((int)units[unit].x,(int)units[unit].y,civ2);
      dlg_game_txt_with_unit((const char *)&str_deserted,unit);
    }
    else if ((2 < game.multiplayer) && (human_civ[0] != civ2)) {
      net_send_state_diff(0xff,2,0,0,0);
      net_send_message(NM_POSSIBLY_CENTER,net.net_slots[net.net_civ_slots[civ2]].id,
                         (int)units[unit].x,(int)units[unit].y,civ2,0,0,0,0,0);
      net_send_text_args(0x22,net.net_slots[net.net_civ_slots[civ2]].id,3,0,
                         unit,0);
    }
  }
  map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_DRAW_SQUARE,0xff,(int)units[unit].x,(int)units[unit].y,0,0,0,0,0,0);
  }
  if (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) && (human_civ[0] == civ2)) {
    ui_delay_ticks(0x1e);
  }
  return;
}

/* 004c9ebd auto */
i32 __cdecl spy_unit_mission(iptr unit2,iptr unit,iptr civ)

{
  int x;
  i32 result;
  iptr adjective;
  int y;
  int hp_left;
  
  x = unit_stack_sum(unit,2);
  if (x < 2) {
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
        (units[unit2].type != UNIT_SPY)) || ((game.rules & RULE_NORMAL_COMBAT) == 0)) {
      unit_bribe(unit,civ);
      result = 0;
    }
    else {
      x = popup_game_with_unit((const char *)&str_sabotageoptions,1,unit2);
      if (x < 0) {
        result = 0;
      }
      else if (x == 0) {
        unit_bribe(unit,civ);
        result = 0;
      }
      else {
        if ((human_civ[0] == units[unit].owner) || (human_civ[0] == civ)) {
          snd_play_effect(SND_SMALLEXP,1,0,0);
        }
        adjective = civ_get_adjective((int)units[unit].owner);
        text_set_string_arg(0,(const char *)adjective);
        text_set_label_arg(1,unit_types[units[unit].type].name);
        adjective = civ_get_adjective(civ);
        text_set_string_arg(2,(const char *)adjective);
        text_set_label_arg(3,unit_types[units[unit2].type].name);
        dlg_game_txt_with_unit((const char *)&str_blewitup,unit2);
        if ((2 < game.multiplayer) &&
           (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) {
          net_send_text_args(0x25,net.net_slots[net.net_civ_slots[units[unit].owner]].id,4,0,
                             unit2,0);
        }
        x = (int)units[unit].x;
        y = (int)units[unit].y;
        map_animate_explosion(x,y,0);
        if (2 < game.multiplayer) {
          net_send_message(NM_KABOOM,0xff,x,y,0,0,0,0,0,0);
        }
        hp_left = unit_hp_remaining(unit);
        units[unit].damage = (char)(hp_left / 2) + units[unit].damage;
        map_redraw_tile_all_views(x,y);
        if (2 < game.multiplayer) {
          net_send_state_diff(0xff,2,0,0,1);
          net_send_message(NM_DRAW_SQUARE,0xff,x,y,0,0,0,0,0,0);
        }
        spy_try_escape(unit2,0,-1);
        result = 1;
      }
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004ca1cd auto */
void __cdecl unit_airlift_perform(iptr unit,iptr city2,iptr city,int risk_rolls,iptr civ)

{
  int roll;
  iptr civ_name;
  int i;
  
  cities[city2].flags = cities[city2].flags | CITYF_AIRLIFTED;
  cities[city].flags = cities[city].flags | CITYF_AIRLIFTED;
  unit_exhaust_moves(unit);
  units[unit].orders = ORDER_NONE;
  unit_relocate(unit,(int)cities[city].x,(int)cities[city].y,1);
  ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit,-1,-1);
  if (risk_rolls != 0) {
    for (i = 0; i < risk_rolls; i++) {
      roll = _rand();
      if (roll % 6 == 0) {
        if ((units[unit].owner == human_civ[0]) && (autoplay.active == 0)) {
          civ_name = (iptr)(civ_name_plural(civ));
          text_set_string_arg(0,(const char *)civ_name);
          dlg_game_txt_with_unit_flags((const char *)&str_shotdown,0x1b,(ui_metrics.large_screen == 0) - 1 & 8);
        }
        unit_delete(unit,1);
        return;
      }
    }
  }
  if ((units[unit].owner == human_civ[0]) && (autoplay.active == 0)) {
    text_set_string_arg(0,(const char *)&cities[city2].name);
    text_set_string_arg(1,(const char *)&cities[city].name);
    dlg_game_txt_with_unit((const char *)&str_airlift,unit);
  }
  return;
}

/* Part of unit_paradrop (004ca39e): whether this player sees the drop (view:
   civ_sees) or sees it animated (civ_animates): view[human_civ].  B22: with
   no player here (-1 during a hot-seat AI turn, 0 after Reveal Map's
   Barbarians) nobody here sees it; the original reads locals it has not set
   there (every mode: ours would read others; docs/BUGS.md). */
static int unit_paradrop_local_view(const int *view)
{
  if ((*(i8 *)&human_civ) <= 0) {
    return 0;
  }
  return view[(*(i8 *)&human_civ)];
}

/* Part of unit_paradrop (004ca39e): which human players see the drop
   (civ_sees) and see it animated (civ_animates). */
static void unit_paradrop_viewers(int x,int y,uptr civ,uptr civ4,uptr city2,int *civ_sees,
                                             int *civ_animates)
{
  int ret;
  iptr x2;
  uptr tile_civ;
  int y2;
  uptr civ2;
  uptr dir;
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    if (((2 < game.multiplayer) || ((*(i8 *)&human_civ) == civ2)) &&
       (((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0 &&
        ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)))) {
      civ_sees[civ2] = 0;
      civ_animates[civ2] = civ_sees[civ2];
      if ((civ == civ2) || (game.reveal_map != '\0')) {
        civ_sees[civ2] = 1;
      }
      if ((((-1 < (int)city2) && (civ4 != civ)) &&
          (ret = tile_visible_to(x,y,civ2), ret != 0)) &&
         (((*(byte *)&civs[civ2].treaties[civ] & TREATY_EMBASSY) != 0 ||
          ((*(byte *)&civs[civ2].treaties[civ4] & TREATY_EMBASSY) != 0)))) {
        civ_sees[civ2] = 1;
      }
      for (dir = 0; (int)dir < 9; dir++) {
        x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
        y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
        ret = map_in_bounds(x2,y2);
        if ((ret != 0) &&
           (tile_civ = tile_city_or_unit_owner(x2,y2), tile_civ == civ2)) {
          civ_sees[civ2] = 1;
        }
      }
      if ((civ_sees[civ2] != 0) &&
         (((civ == civ2 || (((*((char *)&game.game_options + 1)) & (OPT_SHOW_ENEMY_MOVES >> 8)) != 0)) || (-1 < (int)city2)
          ))) {
        civ_animates[civ2] = 1;
      }
    }
  }
}

/* Part of unit_paradrop (004ca39e): the drop announced to the human players who
   see it. */
static void unit_paradrop_announce(iptr unit,int x,int y,uptr civ,byte civ3,int *civ_sees)
{
  iptr x2;
  uptr civ2;
  int city;
  x2 = civ_get_adjective(civ);
  text_set_string_arg(0,(const char *)x2);
  text_set_label_arg(1,unit_types[units[unit].type].name);
  city = city_find_nearest(x,y,-1,0xffffffff,-1);
  text_set_string_arg(2,(const char *)&cities[city].name);
  if (((unit_paradrop_local_view(civ_sees) != 0) &&
      (mapviews_ensure_visible(x,y,civ),
      (1 << (civ3 & 0x1f) & (uint)game.human_players) == 0)) && (autoplay.active == 0)) {
    dlg_game_txt_with_unit((const char *)&str_paradrop,unit);
  }
  if (2 < game.multiplayer) {
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0) &&
          ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
         (((*(i8 *)&human_civ) != civ2 &&
          (((civ_sees[civ2] != 0 &&
            ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0)) && (autoplay.active == 0))))
         )) {
        net_send_text_args(0x15,net.net_slots[net.net_civ_slots[civ2]].id,3,0,unit,0)
        ;
      }
    }
  }
}

/* Part of unit_paradrop (004ca39e): the direction the unit is shown flying in
   from (preferring an empty tile, a diagonal, at random). Returns it, or -1;
   (from_x,from_y) is where it starts. */
static uint unit_paradrop_approach(int x,int y,iptr &from_x,int &from_y)
{
  int ret;
  iptr x2;
  uint anim_dir;
  int y2;
  uptr dir;
  int best_score;
  int score;
  best_score = 0;
  anim_dir = 0xffffffff;
  for (dir = 0; (int)dir < 8; dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
    ret = map_in_bounds(x2,y2);
    if (ret != 0) {
      score = crt_rand_at("unit_paradrop");
      score = score % 6;
      if (((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] != '\0') && ((((i8 *)&dir_dy_table_block.dir_dy_table))[dir] != '\0')) {
        score += 3;
      }
      ret = tile_city_or_unit_owner(x2,y2);
      if (ret < 0) {
        score += 200;
      }
      if (best_score < score) {
        best_score = score;
        anim_dir = dir ^ 4;
        from_y = y2;
        from_x = x2;
      }
    }
  }
  return anim_dir;
}

/* Part of unit_paradrop (004ca39e): the drop relayed to the other human
   players. */
static void unit_paradrop_relay(iptr unit,int x,int y,uint anim_dir,int *civ_animates)
{
  uptr civ2;
  for (civ2 = 1; (int)civ2 < 8; civ2++) {
    if ((((*(i8 *)&human_civ) != civ2) &&
        ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
      if (((game.reveal_map != '\0') ||
          ((1 << ((byte)civ2 & 0x1f) & (uint)units[unit].visibility) != 0)) ||
         ((int)units[unit].owner == (civ2 & 0xff))) {
        net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ2]].id,x,
                           y,0,0,0,0,0,0);
      }
      if (civ_animates[civ2] != 0) {
        net_send_message(NM_SLIDE_PIECE,net.net_slots[net.net_civ_slots[civ2]].id,unit,
                           x,y,anim_dir,-1,0,0,0);
      }
    }
  }
}

/* 004ca39e auto */
void __cdecl unit_paradrop(iptr unit,int x,int y)

{
  byte civ3;
  short x3;
  short y3;
  uptr civ;
  int ret;
  uptr city2;
  int from_y;
  iptr from_x;
  uint anim_dir;
  int civ_sees[8];
  int civ_animates[8];
  uptr civ4;
  uptr civ2;
  
  civ3 = units[unit].owner;
  civ = (uptr)(char)civ3;
  x3 = units[unit].x;
  y3 = units[unit].y;
  text_set_number_arg_u8(0,CONCAT31((int3)(char)((ushort)y3 >> 8),cosmic.paradrop_range));
  ret = tile_is_ocean(x,y);
  if (ret == 0) {
    civ4 = tile_unit_owner(x,y);
    if (((int)civ4 < 0) || (civ4 == civ)) {
      ret = map_distance((int)units[unit].x,(int)units[unit].y,x,y);
      if ((int)(uint)cosmic.paradrop_range < ret) {
        if ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) {
          dlg_game_txt_with_unit((const char *)&str_paradroptarget1,unit);
        }
      }
      else {
        civ4 = tile_owner(x,y);
        city2 = city_at(x,y);
        if (((-1 < (int)city2) && (civ4 != civ)) &&
           ((*(byte *)&civs[civ].treaties[civ4] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) != 0)) {
          if ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) {
            unit_exhaust_moves(unit);
            return;
          }
          ret = diplo_confirm_attack(civ,civ4,0xe);
          if (ret != 0) {
            return;
          }
        }
        unit_paradrop_viewers(x,y,civ,civ4,city2,civ_sees,civ_animates);
        unit_paradrop_announce(unit,x,y,civ,civ3,civ_sees);
        anim_dir = unit_paradrop_approach(x,y,from_x,from_y);
        if ((int)anim_dir < 0) {
          for (civ2 = 1; (int)civ2 < 8; civ2++) {
            civ_animates[civ2] = 0;
          }
        }
        units[unit].orders = ORDER_NONE;
        unit_lift_off_map(unit,0);
        if ((((*(i8 *)&human_civ) == civ) || (game.reveal_map != '\0')) ||
           (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) != 0 ||
            ((int)units[unit].owner == ((*(i8 *)&human_civ) & 0xffU))))) {
          map_redraw_tile_all_views((int)x3,(int)y3);
        }
        if (unit_paradrop_local_view(civ_animates) != 0) {
          map_animate_unit_move(unit,from_x,from_y,anim_dir,0xffffffff,0xffffffff);
        }
        unit_stack_clear_visibility(unit);
        if ((game.active_unit == unit) && ((*(i8 *)&human_civ) == civ)) {
          map_cursor.x[0] = (i16)x;
          map_cursor.y[0] = (i16)y;
        }
        unit_stack_relocate_all(unit,x,y,0);
        if (2 < game.multiplayer) {
          net_send_state_diff(0xff,2,0,0,0);
          unit_paradrop_relay(unit,x,y,anim_dir,civ_animates);
        }
        units[unit].flags = units[unit].flags | UNITF_PARADROPPED;
        if ((-1 < (int)city2) && (civ4 != civ)) {
          if ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) {
            unit_exhaust_moves(unit);
          }
          units[unit].damage = '\0';
          game.moving_unit = (i16)unit;
          city_capture_aftermath(city2,civ,0);
          unit = (iptr)game.moving_unit;
        }
        unit_reveal_surroundings(unit,1);
        ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit,(int)x3,(int)y3);
        units[unit].flags = units[unit].flags & ~UNITF_AI_GARRISON;
        status_redraw(1);
      }
    }
    else if ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) {
      dlg_game_txt_with_unit((const char *)&str_paradroptarget2,unit);
    }
  }
  else if ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) {
    dlg_game_txt_with_unit((const char *)&str_paradroptarget,unit);
  }
  return;
}

/* 004ec1c6 auto */
void __cdecl unit_pick_caravan_commodity(iptr city,iptr unit)

{
  iptr rand_start;
  iptr rnd;
  iptr slot;
  int rand_slot;
  int i;
  
  units[unit].counter = 0xff;
  rand_start = _rand();
  for (i = 0; i <= 2; i++) {
    slot = (rand_start % 3 + i) % 3;
    if (-1 < *(char *)&cities[city].supplies[slot]) {
      units[unit].counter = cities[city].supplies[slot];
      break;
    }
  }
  if ((char)units[unit].counter < '\0') {
    rnd = _rand();
    rand_slot = rnd % 3;
    if (*(char *)&cities[city].supplies[rand_slot] < 1) {
      units[unit].counter = ~cities[city].supplies[rand_slot] + 1;
    }
    else {
      units[unit].counter = cities[city].supplies[rand_slot];
    }
  }
  cities[city].flags = cities[city].flags | CITYF_HAD_CARAVAN;
}

/* 00531567 auto */
void __cdecl unit_stack_wake_by_role(iptr unit,uint role_mask,int wake)

{
  unit = unit_stack_first(unit);
  while ((-1 < unit &&
         (((units[unit].orders != ORDER_SLEEP ||
           ((role_mask & 1 << (unit_types[units[unit].type].role & 0x1f)) == 0)) ||
          (units[unit].orders = ORDER_NONE, wake == 0))))) {
    unit = unit_next_in_stack(unit);
  }
  return;
}

/* 00531607 auto */
void __cdecl unit_set_goto(iptr unit,i32 goto_flag,i32 dest_x,i32 dest_y)

{
  units[unit].orders = ORDER_GOTO;
  units[unit].ai_task = (u8)goto_flag;
  units[unit].goto_x = (i16)dest_x;
  units[unit].goto_y = (i16)dest_y;
  return;
}

/* 00543b80 auto */
void __cdecl unit_continue_active_orders(void)

{
  iptr unit;
  int ret;
  
  ui_flush_input();
  unit = (iptr)game.active_unit;
  if ((((((units[unit].moves_used != '\0') && (units[unit].orders == ORDER_GOTO)) &&
        (unit_types[units[unit].type].hold == '\0')) &&
       ((unit_types[units[unit].type].role != ROLE_NAVAL_SUPERIORITY ||
        (ret = tile_adjacent_nonallied_civ((int)units[unit].x,(int)units[unit].y,(int)units[unit].owner)
        , ret == 0)))) ||
      ((ret = ai_unit_choose_orders(), ret == 0 && (ret = unit_moves_left(unit), ret != 0))
      )) && (ret = unit_execute_orders(unit), ret == 0)) {
    unit_exhaust_moves(unit);
  }
  return;
}

/* 0056b96e auto */
iptr __cdecl unit_stack_pick_display_unit(iptr unit)

{
  int x;
  int y;
  int ret;
  int best_priority;
  int priority2;
  iptr best_unit;
  
  best_unit = -1;
  best_priority = -1;
  if (-1 < unit) {
    x = (int)units[unit].x;
    y = (int)units[unit].y;
    ret = map_in_bounds(x,y);
    if (ret != 0) {
      tile_is_ocean(x,y);
    }
    for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit))
    {
      priority2 = 1;
      if (unit_types[units[unit].type].domain != DOMAIN_LAND) {
        priority2 = 2;
      }
      if ((game.active_unit == unit) && ((int)units[unit].owner == (uint)(byte)(*(i8 *)&human_civ)))
      {
        priority2 = 4;
      }
      if ((unit_types[units[unit].type].domain == DOMAIN_AIR) &&
         (ret = tile_airbase_owner(x,y), -1 < ret)) {
        priority2 = -1;
      }
      if (best_priority < priority2) {
        best_priority = priority2;
        best_unit = unit;
      }
    }
  }
  return best_unit;
}

/* 0057e2c3 auto */
int __cdecl unit_attack_strength(iptr unit)

{
  int strength;
  
  strength = (char)unit_types[units[unit].type].attack * 8;
  if ((units[unit].flags & UNITF_VETERAN) != 0) {
    strength += (strength >> 1);
  }
  if ((units[unit].flags & UNITF_PARADROPPED) != 0) {
    strength += (strength >> 1);
  }
  return strength;
}

/* 0057e33a auto */
int __cdecl unit_defense_strength(iptr unit,int recalc_tile,iptr unit2)

{
  u8 base_defense;
  byte unit_type;
  int x;
  int y;
  byte *tile;
  i32 tile_impr;
  int strength;
  
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  if (recalc_tile != 0) {
    unit_type = tile_terrain(x,y);
    sprite_picker_sprite_block.unit_defense_terrain = (uptr)unit_type;
    sprite_picker_sprite_block.combat_defender_city = city_at(x,y);
  }
  base_defense = unit_types[units[unit].type].defense;
  tile = (byte *)tile_at(x,y);
  strength = ((uint)(*tile >> 7) + (int)(char)terrain_types[sprite_picker_sprite_block.unit_defense_terrain].defense) *
            (char)base_defense * 4;
  sprite_picker_sprite_block.combat_defense_bonus_halves = 2;
  if ((units[unit].orders == ORDER_FORTIFIED) && (unit_types[units[unit].type].domain == DOMAIN_LAND)) {
    sprite_picker_sprite_block.combat_defense_bonus_halves = 3;
  }
  tile_impr = tile_improvements(x,y);
  if (((byte)tile_impr & TILE_AIRBASE) == 0x40) {
    if (unit2 < 0) {
      sprite_picker_sprite_block.combat_defense_bonus_halves = 4;
    }
    else if ((unit_types[units[unit2].type].domain != DOMAIN_AIR) &&
            ((unit_types[units[unit2].type].flags & UTYPE_IGNORE_WALLS) == 0)) {
      sprite_picker_sprite_block.combat_defense_bonus_halves = 4;
    }
  }
  if (-1 < (int)sprite_picker_sprite_block.combat_defender_city) {
    if ((((unit_types[units[unit].type].domain == DOMAIN_AIR) && (-1 < unit2)) &&
        ((unit_types[units[unit].type].flags & UTYPE_ATTACK_AIR) != 0)) &&
       ((unit_type = units[unit2].type, unit_types[unit_type].domain == DOMAIN_AIR &&
        ((unit_types[unit_type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)))) {
      if ((unit_types[unit_type].flags & UTYPE_ATTACK_AIR) == 0) {
        sprite_picker_sprite_block.combat_defense_bonus_halves = sprite_picker_sprite_block.combat_defense_bonus_halves << 2;
      }
      else {
        sprite_picker_sprite_block.combat_defense_bonus_halves = sprite_picker_sprite_block.combat_defense_bonus_halves << 1;
      }
    }
    else {
      x = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_CITY_WALLS);
      if (((x != 0) || (x = civ_has_wonder((int)units[unit].owner,WONDER_GREAT_WALL), x != 0)) &&
         (unit_types[units[unit].type].domain == DOMAIN_LAND)) {
        if (unit2 < 0) {
          sprite_picker_sprite_block.combat_defense_bonus_halves = 6;
        }
        else if ((unit_types[units[unit2].type].domain == DOMAIN_LAND) &&
                ((unit_types[units[unit2].type].flags & UTYPE_IGNORE_WALLS) == 0)) {
          sprite_picker_sprite_block.combat_defense_bonus_halves = 6;
        }
      }
      if (unit_types[units[unit].type].domain != DOMAIN_LAND) {
        sprite_picker_sprite_block.combat_defense_bonus_halves = 2;
      }
    }
  }
  else if (unit_types[units[unit].type].domain != DOMAIN_LAND) {
    sprite_picker_sprite_block.combat_defense_bonus_halves = 2;
  }
  if (sprite_picker_sprite_block.combat_defense_bonus_halves != 2) {
    strength = strength * sprite_picker_sprite_block.combat_defense_bonus_halves >> 1;
  }
  if ((units[unit].flags & UNITF_VETERAN) != 0) {
    strength += (strength >> 1);
  }
  return strength;
}

/* 0057e6e2 auto */
iptr __cdecl unit_stack_best_defender(iptr unit,iptr unit2)

{
  short x;
  short y;
  byte terrain;
  int hp_left;
  int max_hp;
  int strength;
  iptr best_unit;
  int best_strength;
  
  best_strength = 0;
  best_unit = unit;
  if (-1 < unit) {
    x = units[unit].x;
    y = units[unit].y;
    terrain = tile_terrain((int)x,(int)y);
    sprite_picker_sprite_block.unit_defense_terrain = (uptr)terrain;
    sprite_picker_sprite_block.combat_defender_city = city_at((int)x,(int)y);
    for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit))
    {
      if ((sprite_picker_sprite_block.unit_defense_terrain != 10) || (unit_types[units[unit].type].domain != DOMAIN_LAND)) {
        strength = unit_defense_strength(unit,0,unit2);
        if ((game.rules & RULE_NORMAL_COMBAT) != 0) {
          hp_left = unit_hp_remaining(unit);
          max_hp = unit_max_hp(unit);
          strength = (hp_left * strength) / max_hp;
        }
        if ((unit_types[units[unit].type].flags & UTYPE_X2_VS_HORSE) != 0) {
          strength++;
        }
        if ((unit_types[units[unit].type].flags & UTYPE_X2_VS_AIR) != 0) {
          if (unit2 == -1) {
            strength++;
          }
          else if (unit_types[units[unit2].type].domain == DOMAIN_AIR) {
            if ((unit_types[units[unit2].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
              strength = strength * 3;
            }
            else {
              strength = strength * 5;
            }
          }
        }
        if (((((unit_types[units[unit].type].flags & UTYPE_ATTACK_AIR) != 0) && (unit2 != -1)) &&
            (unit_types[units[unit2].type].domain == DOMAIN_AIR)) &&
           ((unit_types[units[unit2].type].flags & UTYPE_ATTACK_AIR) != 0)) {
          strength = strength << 1;
        }
        if ((unit_types[units[unit].type].domain == DOMAIN_SEA) && (-1 < (int)sprite_picker_sprite_block.combat_defender_city)) {
          if (unit2 == -1) {
            strength = strength / 2;
          }
          else if ((unit_types[units[unit2].type].domain == DOMAIN_AIR) &&
                  (hp_left = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_SAM_BATTERY), hp_left == 0)) {
            strength = strength << 1;
          }
          else {
            strength = strength / 2;
          }
        }
        if (best_strength <= strength) {
          best_strength = strength;
          best_unit = unit;
        }
      }
    }
  }
  return best_unit;
}

/* 0057e9f9 auto */
void __cdecl unit_kill_in_combat(iptr unit,iptr unit2,iptr x,iptr y)

{
  char *lost_count_ptr;
  
  if (*(char *)&civs[units[unit].owner].units_lost[units[unit].type] != -1) {
    lost_count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 278) + units[unit].owner * 0x594);
    *lost_count_ptr = *lost_count_ptr + 1;
  }
  if ((units[unit].owner == human_civ[0]) || (game.multiplayer < 3)) {
    casualty_log_add(unit,unit2,(int)game.year,x,y);
  }
  else if ((2 < game.multiplayer) &&
          (((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) {
    net_send_message(NM_DEATH_STACK,net.net_slots[net.net_civ_slots[units[unit].owner]].id,unit,
                       unit2,(int)game.year,x,y,0,0,0);
  }
  sprite_picker_sprite_block.combat_kill_count++;
  if (scenario_events_loaded != 0) {
    ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_unit_killed((uint)units[unit].type,(int)units[unit2].owner,
               (int)units[unit].owner);
  }
  unit_delete(unit,1);
  return;
}

/* 0057eb94 auto */
void __cdecl unit_kill_stack_in_combat(iptr unit,iptr unit2,i32 x,i32 y)

{
  short next_unit;
  
  sprite_picker_sprite_block.combat_kill_count = 0;
  unit = unit_stack_first(unit);
  while (-1 < unit) {
    next_unit = units[unit].next_in_stack;
    unit_kill_in_combat(unit,unit2,x,y);
    unit = (iptr)next_unit;
  }
  return;
}

/* 0057ebfd auto */
void __cdecl unit_promote_veteran(iptr unit)

{
  if (((((units[unit].flags & UNITF_VETERAN) == 0) &&
       ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) &&
      (units[unit].flags = units[unit].flags | UNITF_VETERAN,
      ((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0)) && (autoplay.active == 0)
     ) {
    text_set_label_arg(0,unit_types[units[unit].type].name);
    if (units[unit].owner == human_civ[0]) {
      dlg_game_txt_with_unit((const char *)&str_promoted_2,unit);
    }
    else if (2 < game.multiplayer) {
      net_send_text_args(0x20,net.net_slots[net.net_civ_slots[units[unit].owner]].id,1,0,
                         unit,0);
    }
  }
  return;
}

/* 0057f9e3 auto */
i32 __cdecl unit_nuclear_attack(iptr civ,int x,int y,int check_sdi)

{
  uint *treaty_ptr;
  char dy;
  iptr target_civ;
  iptr x2;
  iptr has_sdi;
  int unit;
  iptr city;
  byte target_civ_byte;
  iptr dir;
  
  target_civ = tile_city_or_unit_owner(x,y);
  x2 = (iptr)(civ_name_plural(civ));
  text_set_string_arg(0,(const char *)x2);
  target_civ_byte = (byte)target_civ;
  if (((game.multiplayer < 3) || ((1 << (target_civ_byte & 0x1f) & (uint)game.human_players) == 0)) ||
     (human_civ[0] == target_civ)) {
    if ((1 << (target_civ_byte & 0x1f) & (uint)game.human_players) != 0) {
      popup_game_with_improvement((const char *)&str_useweapons_2,IMP_MANHATTAN,8);
    }
  }
  else {
    net_send_text_args(0x29,net.net_slots[net.net_civ_slots[target_civ]].id,1,0,0,0);
  }
  if (check_sdi != 0) {
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].owner != civ)) &&
         ((has_sdi = city_has_improvement(city,IMP_SDI_DEFENSE), has_sdi != 0 &&
          (unit = map_distance((int)cities[city].x,(int)cities[city].y,x,
                                      y), unit < 4)))) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        if (((game.multiplayer < 3) ||
            (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) == 0)) ||
           (cities[city].owner == human_civ[0])) {
          if (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0) {
            popup_game_with_improvement((const char *)&str_sdi_2,IMP_SDI_DEFENSE,8);
          }
        }
        else {
          net_send_text_args(0x2a,net.net_slots[net.net_civ_slots[cities[city].owner]].id,1,0,
                             0,0);
        }
        if (((game.multiplayer < 3) ||
            ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) ||
           (human_civ[0] == civ)) {
          if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
            popup_game_with_improvement((const char *)&str_sdi_3,IMP_SDI_DEFENSE,8);
          }
        }
        else {
          net_send_text_args(0x2a,net.net_slots[net.net_civ_slots[civ]].id,1,
                             0,0,0);
        }
        return 0;
      }
    }
  }
  if (((2 < game.multiplayer) && ((1 << (target_civ_byte & 0x1f) & (uint)game.human_players) != 0)) &&
     (human_civ[0] != target_civ)) {
    net_send_message(NM_SHOW_NUKE,net.net_slots[net.net_civ_slots[target_civ]].id,x,
                       y,0,0,0,0,0,0);
  }
  map_animate_nuke(x,y);
  for (dir = 0; dir < 9; dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    unit = map_in_bounds(x2,dy + y);
    if ((unit != 0) && (unit = tile_first_unit(x2,dy + y), -1 < unit)) {
      if (units[unit].owner != civ) {
        treaty_ptr = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + units[unit].owner * 0x594);
        *treaty_ptr = *treaty_ptr | 0x110;
        treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + units[unit].owner * 4);
        *treaty_ptr = *treaty_ptr | 0x20000;
        civ_adjust_attitude((int)units[unit].owner,civ,100);
      }
      unit_stack_delete_all(unit,1);
    }
  }
  nuclear_blast(x,y);
  return 1;
}

/* Part of unit_resolve_attack (00580341): a defending city's Coastal Defense,
   SAM battery and SDI, or fighters scrambling against bombers. */
static void unit_resolve_attack_city_defenses(iptr unit,iptr unit2,int execute,int &defense2,
                                                         iptr &improvement,int &sdi_bonus,int &scramble_msg)
{
  iptr has_coastal;
  int unit3;
  if ((((0 < (int)sprite_picker_sprite_block.combat_defender_city) && (unit_types[units[unit].type].domain == DOMAIN_SEA)) &&
      (unit_types[units[unit2].type].domain != DOMAIN_SEA)) &&
     ((has_coastal = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_COASTAL_FORTRESS), has_coastal != 0 &&
      (defense2 = defense2 << 1, execute != 0)))) {
    improvement = 0x1c;
    cities[sprite_picker_sprite_block.combat_defender_city].flags = cities[sprite_picker_sprite_block.combat_defender_city].flags | CITYF_COASTAL_FORT_USED;
  }
  if ((0 < (int)sprite_picker_sprite_block.combat_defender_city) && (unit_types[units[unit].type].domain == DOMAIN_AIR)) {
    if (((unit_types[units[unit2].type].flags & UTYPE_ATTACK_AIR) == 0) ||
       ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)) {
      unit3 = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_SDI_DEFENSE);
      if (((unit3 != 0) && ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)) &&
         ((char)unit_types[units[unit].type].attack < 'c')) {
        defense2 = defense2 << 1;
        sdi_bonus = 1;
        if (execute != 0) {
          improvement = 0x11;
        }
      }
      unit3 = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_SAM_BATTERY);
      if ((unit3 != 0) && (defense2 = defense2 << 1, execute != 0)) {
        improvement = 0x1b;
      }
    }
    else if (execute != 0) {
      scramble_msg = 1;
    }
  }
}

/* Part of unit_resolve_attack (00580341): the odds of barbarian attacks, and of
   attacks on barbarians (difficulty, Great Wall). */
static void unit_resolve_attack_barbarian_odds(iptr unit2,uptr civ,uptr civ2,byte civ5,
                                                          int &attack_power,int &defense2,
                                                          bool &defender_reroll)
{
  int unit3;
  if (civ == 0) {
    if ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0) {
      attack_power = attack_power / 2;
    }
    else {
      attack_power = (game.difficulty + 1) * attack_power;
      attack_power = (int)(attack_power + (attack_power >> 0x1f & 3U)) >> 2;
    }
    if (-1 < (int)sprite_picker_sprite_block.combat_defender_city) {
      if (civs[civ2].num_cities < 2) {
        attack_power = 0;
      }
      unit3 = city_has_improvement(sprite_picker_sprite_block.combat_defender_city,IMP_PALACE);
      defender_reroll = unit3 != 0;
      if (defender_reroll) {
        attack_power = attack_power >> 1;
      }
      if (civs[civ2].num_cities < 8) {
        defender_reroll = true;
      }
    }
    unit3 = civ_has_wonder(civ2,WONDER_GREAT_WALL);
    if (unit3 != 0) {
      attack_power = attack_power / 2;
    }
  }
  if (civ2 == 0) {
    if (((units[unit2].type == UNIT_ARCHERS) ||
        ((units[unit2].type == UNIT_LEGION && ((game.human_players & game.advance_known_by[ADV_IRON_WORKING]) == 0)
         ))) && (defense2 = defense2 / 2, defense2 < 2)) {
      defense2 = 1;
    }
    unit3 = civ_has_wonder(civ,WONDER_GREAT_WALL);
    if (unit3 != 0) {
      attack_power = attack_power << 1;
    }
  }
}

/* Part of unit_resolve_attack (00580341): attacking breaks a treaty: who is told
   (treaty and ally warnings), the senate, the victim's message. */
static void unit_resolve_attack_break_treaty(iptr unit,uptr civ,uptr civ2,byte civ4,byte civ5,
                                                        int &attack_power,int &show_treaty_msg,
                                                        int &ally_warning)
{
  int unit3;
  iptr name_str;
  int senate_roll;
  uptr civ3;
  if (((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0) ||
     (((*(byte *)&civs[civ2].treaties[civ] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0 &&
      ((((*(byte *)&civs[civ2].treaties[civ] & TREATY_CONTACT) == 0 &&
        ((*(byte *)&civs[civ].treaties[civ2] & TREATY_BROKEN) == 0)) ||
       ((*(byte *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 33) + civ * 4) & 0x20) != 0)))))) {
    if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) != 0) ||
       ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) != 0)) {
      if (game.multiplayer < 3) {
        if (((1 << (civ4 & 0x1f) & (uint)game.human_players) == 0) &&
           ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0)) {
          if ((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_ALLIANCE) == 0) {
            if ((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_ALLIANCE) == 0) {
              if (game.reveal_map != '\0') {
                show_treaty_msg = 1;
              }
            }
            else {
              ally_warning = 2;
            }
          }
          else {
            ally_warning = 1;
          }
        }
        else {
          show_treaty_msg = 1;
        }
      }
      else {
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
            if (civ == civ3) {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 1;
            }
            else if (civ2 == civ3) {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 1;
            }
            else if (game.reveal_map != '\0') {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 1;
            }
            if (*(int *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) != 0) {
              show_treaty_msg = 1;
            }
          }
        }
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) {
            if ((civ2 == civ3) ||
               ((*(byte *)&civs[civ3].treaties[civ2] & TREATY_ALLIANCE) == 0)) {
              if ((civ != civ3) &&
                 ((*(byte *)&civs[civ3].treaties[civ] & TREATY_ALLIANCE) != 0)) {
                *(i32 *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) = 2;
                ally_warning = 2;
              }
            }
            else {
              *(i32 *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) = 1;
              ally_warning = 1;
            }
          }
        }
      }
    }
  }
  else {
    if (GOV_FUNDAMENTALISM < civs[civ].government) {
      senate_roll = crt_rand_at("unit_resolve_attack");
      senate_roll = senate_roll % 100;
      if ((civs[civ].government == GOV_REPUBLIC) ||
         (unit3 = civ_has_wonder(civ,WONDER_UNITED_NATIONS), unit3 != 0)) {
        senate_roll -= 0x32;
      }
      if ((-1 < (int)(senate_roll + (uint)civs[civ2].reputation * -10)) &&
         ((1 << (civ4 & 0x1f) & (uint)game.human_players) == 0)) {
        ai_choose_government(civ,1);
      }
    }
    if (((*(byte *)&civs[civ2].treaties[civ] & TREATY_CEASEFIRE) == 0) ||
       ((*(byte *)&civs[civ2].treaties[civ] & TREATY_PEACE) != 0)) {
      if ((autoplay.active == 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
        name_str = civ_get_adjective(civ);
        text_set_string_arg(0,(const char *)name_str);
        if (human_civ[0] == civ2) {
          dlg_game_txt_with_unit((const char *)&str_sneak_2,unit);
        }
        else if (2 < game.multiplayer) {
          net_send_text_args(0x2e,net.net_slots[net.net_civ_slots[civ2]].id,1,0
                             ,unit,0);
        }
      }
    }
    else if ((autoplay.active == 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
      name_str = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)name_str);
      if (human_civ[0] == civ2) {
        dlg_game_txt_with_unit((const char *)&str_breakcease_3,unit);
      }
      else if (2 < game.multiplayer) {
        net_send_text_args(0x2d,net.net_slots[net.net_civ_slots[civ2]].id,1,0,
                           unit,0);
      }
    }
    attack_power = attack_power << 1;
    civs[civ2].last_contact_turn[civ] = game.turn;
  }
}

/* Part of unit_resolve_attack (00580341): which human players see the combat
   (civ_sees). Returns whether any does. */
static bool unit_resolve_attack_viewers(iptr unit,iptr unit2,uptr civ,uptr civ2,int *civ_sees)
{
  bool any_viewer;
  uptr civ3;
  any_viewer = false;
  for (civ3 = 1; (int)civ3 < 8; civ3++) {
    civ_sees[civ3] = 0;
    if ((((2 < game.multiplayer) || (human_civ[0] == civ3)) &&
        ((1 << ((byte)civ3 & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0)) {
      if (game.multiplayer < 3) {
        if (((((civ == civ3) || (civ2 == civ3)) ||
             ((game.reveal_map != '\0' ||
              ((((1 << ((byte)civ3 & 0x1f) & (uint)units[unit].visibility) != 0 ||
                ((int)units[unit].owner == (civ3 & 0xff))) ||
               ((1 << ((byte)civ3 & 0x1f) & (uint)units[unit2].visibility) != 0)))))) ||
            ((int)units[unit2].owner == (civ3 & 0xff))) ||
           (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))) {
          civ_sees[civ3] = 1;
          any_viewer = true;
        }
      }
      else if ((civ == civ3) || (civ2 == civ3)) {
        civ_sees[civ3] = 1;
        any_viewer = true;
      }
    }
  }
  return any_viewer;
}

/* Part of unit_resolve_attack (00580341): the messages before the combat
   (missile attack, sneak attack from the sea, city defenses, scrambled
   fighters, attack from a ship). */
static void unit_resolve_attack_announce(iptr unit,iptr unit2,uptr civ,uptr civ2,byte civ5,iptr x,
                                                    int y,int x2,int y2,iptr improvement,int sdi_bonus,
                                                    int scramble_msg,int shore_bombard,int *civ_sees)
{
  iptr name_str;
  int city2;
  uptr civ3;
  if (((((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0) &&
       ((char)unit_types[units[unit].type].attack < 'c')) &&
      ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) &&
     ((city2 = city_find_nearest(x,y,-1,0xffffffff,-1), -1 < city2 &&
      (text_set_string_arg(0,(const char *)&cities[city2].name), autoplay.active == 0)))) {
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          dlg_game_txt_with_unit((const char *)&str_missileattack_2,unit);
        }
        else {
          net_send_text_args(0x2f,net.net_slots[net.net_civ_slots[civ3]].id
                             ,1,0,unit,0);
        }
      }
    }
  }
  if (shore_bombard != 0) {
    name_str = civ_get_adjective(civ);
    text_set_string_arg(0,(const char *)name_str);
    text_set_label_arg(1,unit_types[units[unit].type].name);
    name_str = civ_get_adjective(civ2);
    text_set_string_arg(2,(const char *)name_str);
    if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (civ_sees[civ3] != 0) {
          if (human_civ[0] == civ3) {
            dlg_game_txt_with_unit((const char *)&str_pearlharbor_2,unit);
          }
          else {
            net_send_text_args(0x30,net.net_slots[net.net_civ_slots[civ3]].id,3,0,unit,0)
            ;
          }
        }
      }
    }
  }
  if (improvement != IMP_NOTHING) {
    text_set_label_arg(1,improvements[improvement].name);
    text_set_string_arg(2,(const char *)&cities[sprite_picker_sprite_block.combat_defender_city].name);
    if ((sdi_bonus == 0) || (improvement == IMP_SDI_DEFENSE)) {
      if (autoplay.active == 0) {
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if (civ_sees[civ3] != 0) {
            if (human_civ[0] == civ3) {
              popup_game_with_improvement((const char *)&str_battery_2,improvement,8);
            }
            else {
              net_send_text_args(0x32,net.net_slots[net.net_civ_slots[civ3]].id,3,0,improvement
                                 ,0);
            }
          }
        }
      }
    }
    else {
      text_set_label_arg(3,improvements[IMP_SDI_DEFENSE].name);
      if (autoplay.active == 0) {
        for (civ3 = 1; (int)civ3 < 8; civ3++) {
          if (civ_sees[civ3] != 0) {
            if (human_civ[0] == civ3) {
              popup_game_with_improvement((const char *)&str_battery2_2,improvement,8);
            }
            else {
              net_send_text_args(0x31,net.net_slots[net.net_civ_slots[civ3]].id,4,0,improvement
                                 ,0);
            }
          }
        }
      }
    }
  }
  if (scramble_msg != 0) {
    name_str = civ_get_adjective(civ2);
    text_set_string_arg(0,(const char *)name_str);
    text_set_label_arg(1,unit_types[units[unit2].type].name);
    text_set_string_arg(2,(const char *)&cities[sprite_picker_sprite_block.combat_defender_city].name);
    if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (civ_sees[civ3] != 0) {
          if (human_civ[0] == civ3) {
            dlg_game_txt_with_unit((const char *)&str_scramble_2,unit2);
          }
          else {
            net_send_text_args(0x33,net.net_slots[net.net_civ_slots[civ3]].id,3,0,unit2,0)
            ;
          }
        }
      }
    }
  }
  if ((unit_types[units[unit].type].domain == DOMAIN_LAND) &&
     (city2 = tile_is_ocean(x2,y2), city2 != 0)) {
    name_str = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)name_str);
    text_set_label_arg(2,unit_types[units[unit].type].name);
    if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (civ_sees[civ3] != 0) {
          if (human_civ[0] == civ3) {
            dlg_game_txt_with_unit((const char *)&str_amphibmotize_2,unit);
          }
          else {
            net_send_text_args(0x34,net.net_slots[net.net_civ_slots[civ3]].id,3,0,unit,0)
            ;
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the attacker's sound. Returns the kind of
   explosion shown (0: the default). */
static int unit_resolve_attack_sound(iptr unit,iptr unit2)
{
  byte unit_type;
  int explosion_kind;
  explosion_kind = 0;
  unit_type = units[unit].type;
  prefs_main_menu_choice_block.snd_last_effect_id = -1;
  prefs_main_menu_choice_block.snd_last_effect_arg = -1;
  if (unit_type == UNIT_TEST_1) {
    snd_play_effect(SND_EXTRA1,1,0,0);
  }
  else if (unit_type == UNIT_TEST_2) {
    snd_play_effect(SND_EXTRA2,1,0,0);
  }
  else if (unit_type == UNIT_TEST_3) {
    snd_play_effect(SND_EXTRA3,1,0,0);
  }
  else if (unit_type == UNIT_TEST_4) {
    snd_play_effect(SND_EXTRA4,1,0,0);
  }
  else if (unit_type == UNIT_TEST_5) {
    snd_play_effect(SND_EXTRA5,1,0,0);
  }
  else if (unit_type == UNIT_TEST_6) {
    snd_play_effect(SND_EXTRA6,1,0,0);
  }
  else if (unit_type == UNIT_TEST_7) {
    snd_play_effect(SND_EXTRA7,1,0,0);
  }
  else if (unit_type == UNIT_TEST_8) {
    snd_play_effect(SND_EXTRA8,1,0,0);
  }
  else if (unit_type == UNIT_EXTRA_LAND) {
    snd_play_effect(SND_CUSTOM1,1,0,0);
  }
  else if (unit_type == UNIT_EXTRA_SHIP) {
    snd_play_effect(SND_CUSTOM2,1,0,0);
  }
  else if (unit_type == UNIT_EXTRA_AIR) {
    snd_play_effect(SND_CUSTOM3,1,0,0);
  }
  else if ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
    if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
      if (unit_types[units[unit2].type].domain == DOMAIN_AIR) {
        if (units[unit].type < 0x1e) {
          snd_play_effect(SND_AIRCOMBT,0,0,0);
        }
        else {
          snd_play_effect(SND_JETCOMBT,0,0,0);
        }
      }
      else if (unit_types[units[unit].type].range == '\0') {
        snd_play_effect(SND_HELISHOT,1,0,0);
      }
      else if (units[unit].type < 0x1e) {
        snd_play_effect(SND_DIVEBOMB,1,0,0);
      }
      else {
        snd_play_effect(SND_JETBOMB,1,0,0);
      }
    }
    else if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
      if ((unit_types[units[unit].type].flags & UTYPE_SUBMARINE) == 0) {
        explosion_kind = 6;
        if (((unit_type == UNIT_BATTLESHIP) || (unit_type == UNIT_CRUISER)) || ((unit_type == UNIT_AEGIS_CRUISER || (unit_type == UNIT_DESTROYER)))) {
          explosion_kind = 0x2e;
        }
      }
      else {
        snd_play_effect(SND_TORPEDOS,1,0,0);
      }
    }
    else if (unit_type == UNIT_ELEPHANT) {
      snd_play_effect(SND_ELEPHANT,1,0,0);
    }
    else if ((((unit_type == UNIT_HORSEMEN) || (unit_type == UNIT_CHARIOT)) || (unit_type == UNIT_KNIGHTS)) || (unit_type == UNIT_CRUSADERS)) {
      snd_play_effect(SND_SWRDHORS,1,0,0);
    }
    else if ((unit_type == UNIT_DRAGOONS) || (unit_type == UNIT_CAVALRY)) {
      snd_play_effect(SND_CAVALRY,1,0,0);
    }
    else if (((unit_type == UNIT_MUSKETEERS) || (unit_type == UNIT_RIFLEMEN)) || ((unit_type == UNIT_ALPINE_TROOPS || (unit_type == UNIT_PARTISANS)))) {
      snd_play_effect(SND_INFANTRY,1,0,0);
    }
    else if ((((unit_type == UNIT_FANATICS) || (unit_type == UNIT_PARATROOPERS)) || (unit_type == UNIT_MARINES)) || (unit_type == UNIT_MECH_INF)) {
      snd_play_effect(SND_MCHNGUNS,1,0,0);
    }
    else if ((unit_type < 0x16) || (0x1a < unit_type)) {
      snd_play_effect(SND_SWORDFGT,1,0,0);
    }
    else if (unit_type == UNIT_CATAPULT) {
      snd_play_effect(SND_CATAPULT,1,0,0);
    }
    else {
      explosion_kind = 0x28;
      if (0x17 < unit_type) {
        snd_play_effect(SND_FIRE,1,0,0);
        ui_delay_ticks(0x14);
      }
    }
  }
  else if ((char)unit_types[units[unit].type].attack < 'c') {
    snd_play_effect(SND_MISSILE,1,0,0);
  }
  return explosion_kind;
}

/* Part of unit_resolve_attack (00580341): a nuclear missile strikes instead. */
static void unit_resolve_attack_nuclear(iptr unit,uptr civ,uptr civ2,byte civ4,iptr x,int y,int x2,
                                                   int y2,int *civ_sees)
{
  int unit3;
  uptr civ3;
  combat_anim.defender = 0xffffffff;
  unit3 = unit_nuclear_attack(civ,x,y,1);
  if (unit3 == 0) {
    unit_delete(unit,1);
    map_redraw_tile_all_views(x2,y2);
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ3]].id
                             ,x2,y2,0,0,0,0,0,0);
        }
      }
    }
  }
  else {
    *(i8 *)&civs[civ].aggression_counter[civ2] = 0;
  }
  if (((unit3 != 0) && ((1 << (civ4 & 0x1f) & (uint)game.human_players) == 0)) &&
     (-1 < (int)sprite_picker_sprite_block.combat_defender_city)) {
    ai_respond_to_attack(civ,x,y);
  }
}

/* Part of unit_resolve_attack (00580341): a combat round's hit: the victim's
   damage, and an explosion shown at (ex,ey) when its health bar drops a step. */
static void unit_resolve_attack_hit(iptr victim,int firepower,iptr ex,int ey,iptr x,int y,int x2,
                                               int y2,bool any_viewer,int anim_step,int explosion_kind,
                                               int *civ_sees,bool &explosion_shown)
{
  byte unit_type;
  int city2;
  uptr civ3;

  unit_type = units[victim].damage;
  units[victim].damage += (char)firepower;
  if (((any_viewer) && (anim_step != 0)) &&
     ((int)((longlong)(ulonglong)units[victim].damage / (longlong)anim_step) !=
      (int)(uint)unit_type / anim_step)) {
    explosion_shown = true;
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          map_animate_explosion(ex,ey,explosion_kind);
          city2 = unit_hp_remaining(victim);
          if (city2 != 0) {
            map_redraw_combat_tiles(x,y,x2,y2);
          }
        }
        else {
          net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id,ex,ey,
                             explosion_kind,0,0,0,0,0);
          city2 = unit_hp_remaining(victim);
          if (city2 != 0) {
            net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,x,
                               y,x2,y2,0,0,0,0);
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the combat rounds. Returns whether the
   attacker won. */
static uint unit_resolve_attack_fight(iptr unit,iptr unit2,iptr x,int y,int x2,int y2,
                                                 int attack_power,int defense2,int att_firepower,
                                                 int def_firepower,bool defender_reroll,bool any_viewer,
                                                 int explosion_kind,int *civ_sees,bool &explosion_shown)
{
  int city2;
  int def_roll2;
  int att_roll2;
  int def_roll;
  int att_roll;
  sbyte small_hp;
  uint attacker_won;
  int anim_step;
  uptr civ3;
  if (((char)unit_types[units[unit2].type].hit_points < 30) &&
     ((char)unit_types[units[unit].type].hit_points < 30)) {
    small_hp = 1;
  }
  else {
    small_hp = 0;
  }
  anim_step = 10 >> small_hp;
  if ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0) {
    anim_step = 0;
    explosion_shown = false;
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          map_animate_explosion(x,y,explosion_kind);
        }
        else {
          net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id
                             ,x,y,explosion_kind,0,0,0,0,0);
        }
      }
    }
  }
  for (;;) {
    city2 = unit_hp_remaining(unit);
    if ((city2 == 0) || (city2 = unit_hp_remaining(unit2), city2 == 0)) {
      attacker_won = unit_hp_remaining(unit);
      break;
    }
    if (attack_power == 1 || attack_power - 1 < 0) {
      att_roll = 0;
    }
    else {
      att_roll = crt_rand_at("unit_resolve_attack");
      att_roll = att_roll % attack_power;
    }
    if (defense2 == 1 || defense2 - 1 < 0) {
      def_roll = 0;
    }
    else {
      def_roll = crt_rand_at("unit_resolve_attack");
      def_roll = def_roll % defense2;
    }
    attacker_won = (uint)(def_roll < att_roll);
    if ((defender_reroll) && (attacker_won != 0)) {
      if (attack_power == 1 || attack_power - 1 < 0) {
        att_roll2 = 0;
      }
      else {
        att_roll2 = crt_rand_at("unit_resolve_attack");
        att_roll2 = att_roll2 % attack_power;
      }
      if (defense2 == 1 || defense2 - 1 < 0) {
        def_roll2 = 0;
      }
      else {
        def_roll2 = crt_rand_at("unit_resolve_attack");
        def_roll2 = def_roll2 % defense2;
      }
      if (att_roll2 < def_roll2) {
        attacker_won = 0;
      }
    }
    if (attacker_won != 0) {
      unit_resolve_attack_hit(unit2,att_firepower,x,y,x,y,x2,y2,any_viewer,anim_step,explosion_kind,civ_sees,
                              explosion_shown);
    }
    else {
      unit_resolve_attack_hit(unit,def_firepower,x2,y2,x,y,x2,y2,any_viewer,anim_step,explosion_kind,civ_sees,
                              explosion_shown);
    }
    if ((game.rules & RULE_NORMAL_COMBAT) == 0) break;
  }
  return attacker_won;
}

/* Part of unit_resolve_attack (00580341): the end of the combat shown: its sound,
   the loser's explosion. */
static void unit_resolve_attack_show_result(iptr unit,iptr unit2,iptr x,int y,int x2,int y2,
                                                       uint attacker_won,bool any_viewer,
                                                       bool explosion_shown,int explosion_kind,int *civ_sees)
{
  uptr civ3;
  if (any_viewer) {
    prefs_main_menu_choice_block.snd_last_effect_id = -1;
    if ((((attacker_won == 0) || (unit_types[units[unit2].type].domain != DOMAIN_AIR)) ||
        (-1 < (int)sprite_picker_sprite_block.combat_defender_city)) &&
       (((attacker_won != 0 || (unit_types[units[unit].type].domain != DOMAIN_AIR)) ||
        ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)))) {
      if (explosion_kind != 0) {
        snd_play_effect(SND_LARGEXPL,1,0,0);
      }
    }
    else if (attacker_won == 0) {
      if (units[unit].type < 0x1e) {
        snd_play_effect(SND_DIVCRASH,1,0,0);
      }
      else {
        snd_play_effect(SND_JETCRASH,1,0,0);
      }
    }
    else if (units[unit2].type < 0x1e) {
      snd_play_effect(SND_DIVCRASH,1,0,0);
    }
    else {
      snd_play_effect(SND_JETCRASH,1,0,0);
    }
    if ((2 < game.multiplayer) && (prefs_main_menu_choice_block.snd_last_effect_id != -1)) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_SOUND_PLAY,net.net_slots[net.net_civ_slots[civ3]].id,
                             prefs_main_menu_choice_block.snd_last_effect_id,prefs_main_menu_choice_block.snd_last_effect_arg,0,0,0,0,0,0);
        }
      }
    }
  }
  net_send_state_diff(0xff,2,0,0,0);
  if (any_viewer) {
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          map_redraw_combat_tiles(x,y,x2,y2);
          if (attacker_won == 0) {
            if (!explosion_shown) {
              map_animate_explosion(x2,y2,0);
            }
          }
          else if (!explosion_shown) {
            map_animate_explosion(x,y,0);
          }
        }
        else {
          net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,x2,y2,0,0,0,0);
          if (attacker_won == 0) {
            if (!explosion_shown) {
              net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id,x2,y2,0,
                                 0,0,0,0,0);
            }
          }
          else if (!explosion_shown) {
            net_send_message(NM_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id
                               ,x,y,0,0,0,0,0,0);
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the kill counted, and the winner may
   become a veteran. */
static void unit_resolve_attack_veterans(iptr unit,iptr unit2,uptr civ,uptr civ2,uint attacker_won,
                                                    int attack_power,int defense2)
{
  char *kill_count_ptr;
  int city2;
  int def_vet_roll;
  int att_vet_roll;
  if (attacker_won == 0) {
    kill_count_ptr = (char *)(civ2 + (iptr)((u8 *)&civs + 80) + civ * 0x594);
    *kill_count_ptr = *kill_count_ptr + 1;
    if (defense2 + attack_power == 1 || defense2 + attack_power - 1 < 0) {
      def_vet_roll = 0;
    }
    else {
      def_vet_roll = crt_rand_at("unit_resolve_attack");
      def_vet_roll = def_vet_roll % (defense2 + attack_power);
    }
    if ((def_vet_roll <= attack_power) || (city2 = civ_has_wonder(civ2,WONDER_SUN_TZU), city2 != 0)) {
      unit_promote_veteran(unit2);
    }
  }
  else {
    *(i8 *)&civs[civ].aggression_counter[civ2] = 0;
    if (defense2 + attack_power == 1 || defense2 + attack_power - 1 < 0) {
      att_vet_roll = 0;
    }
    else {
      att_vet_roll = crt_rand_at("unit_resolve_attack");
      att_vet_roll = att_vet_roll % (defense2 + attack_power);
    }
    if ((att_vet_roll <= defense2) || (city2 = civ_has_wonder(civ,WONDER_SUN_TZU), city2 != 0)) {
      unit_promote_veteran(unit);
    }
  }
}

/* Part of unit_resolve_attack (00580341): the loser (and its stack) is killed; a
   city that lost its defender shrinks or is destroyed. */
static void unit_resolve_attack_remove_loser(iptr unit,iptr unit2,uptr civ,uptr civ2,byte civ5,
                                                        iptr x,int y,int x2,int y2,iptr target_x,
                                                        int target_y,uint attacker_won,uptr &winner_civ,
                                                        uptr &loser_civ,bool &barbarian_ransom,
                                                        int &show_treaty_msg)
{
  iptr name_str;
  int city2;
  i32 tile_impr;
  uptr city;
  int view_civ2;
  uptr civ3;
  if (attacker_won == 0) {
    winner_civ = civ2;
    unit_lift_off_map(unit,1);
    unit_kill_stack_in_combat(unit,unit2,target_x,target_y);
    loser_civ = civ;
  }
  else {
    if ((civ2 == 0) && (4 < (char)unit_types[units[unit2].type].role)) {
      barbarian_ransom = true;
    }
    winner_civ = civ;
    if ((((int)sprite_picker_sprite_block.combat_defender_city < 0) &&
        (tile_impr = tile_improvements(x,y), ((byte)tile_impr & TILE_AIRBASE) != 0x40)) &&
       (city2 = tile_airbase_owner(x,y), city2 < 0)) {
      unit_kill_stack_in_combat(unit2,unit,target_x,target_y);
    }
    else {
      unit_kill_in_combat(unit2,unit,target_x,target_y);
    }
    city = sprite_picker_sprite_block.combat_defender_city;
    loser_civ = civ2;
    if (-1 < (int)sprite_picker_sprite_block.combat_defender_city) {
      cities[sprite_picker_sprite_block.combat_defender_city].flags = cities[sprite_picker_sprite_block.combat_defender_city].flags | CITYF_DEFENDER_LOST;
      city2 = tile_is_ocean(x2,y2);
      if (((city2 == 0) && (city2 = city_has_improvement(city,IMP_CITY_WALLS), city2 == 0)) &&
         ((city2 = civ_has_wonder(civ2,WONDER_GREAT_WALL), city2 == 0 &&
          ((game.difficulty != DIFF_CHIEFTAIN || ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0))))))
      {
        cities[city].size--;
        if (cities[city].size == '\0') {
          city_delete(city,0);
          for (view_civ2 = 1; view_civ2 < 8; view_civ2++) {
            tile_update_known(x,y,view_civ2);
          }
          map_redraw_neighbors_all_views(x,y);
          city2 = civ_check_destroyed(civ2,civ);
          if (city2 != 0) {
            show_treaty_msg = 0;
            for (civ3 = 1; (int)civ3 < 8; civ3++) {
              *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 0;
            }
          }
        }
        else {
          city_reveal_to_civ(city,civ);
          city2 = tile_unit_owner(x,y);
          if ((city2 < 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0)) {
            name_str = tile_landmass(x,y);
            civ_reset_city_production(civ2,name_str);
          }
        }
      }
      city2 = tile_unit_owner(x,y);
      if ((city2 < 0) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
        ai_transports_raid_empty_city(city);
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): several units killed at once. */
static void unit_resolve_attack_multiple_kills(uptr winner_civ,uptr loser_civ)
{
  if ((autoplay.active == 0) && (1 < sprite_picker_sprite_block.combat_kill_count)) {
    text_set_number_arg(0,sprite_picker_sprite_block.combat_kill_count);
    if (human_civ[0] == loser_civ) {
      popup_game((const char *)&str_multiplelose_2);
    }
    else if (human_civ[0] == winner_civ) {
      popup_game((const char *)&str_multiplewin_2);
    }
    if (((game.multiplayer < 3) || ((1 << ((byte)loser_civ & 0x1f) & (uint)game.human_players) == 0))
       || (human_civ[0] == loser_civ)) {
      if (((2 < game.multiplayer) &&
          ((1 << ((byte)winner_civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (human_civ[0] != winner_civ)) {
        net_send_text_args(0x36,net.net_slots[net.net_civ_slots[winner_civ]].id,0,1
                           ,0,0);
      }
    }
    else {
      net_send_text_args(0x35,net.net_slots[net.net_civ_slots[loser_civ]].id,0,1,0
                         ,0);
    }
  }
}

/* Part of unit_resolve_attack (00580341): the ransom for a barbarian leader. */
static void unit_resolve_attack_ransom(uptr civ,int *civ_sees)
{
  uint ransom;
  uptr civ3;
  ransom = ((uint)game.barbarian_activity * 100) / 2;
  civs[civ].gold += ransom;
  text_set_number_arg(0,ransom);
  if ((human_civ[0] == civ) && (autoplay.active == 0)) {
    dlg_game_txt_with_unit_flags((const char *)&str_ransom_2,0x3e,0);
    status_draw_info(1);
  }
  else if ((2 < game.multiplayer) && (autoplay.active == 0)) {
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if ((civ_sees[civ3] != 0) && (human_civ[0] != civ3)) {
        net_send_text_args(0x37,net.net_slots[net.net_civ_slots[civ3]].id,1
                           ,0,0x3e,0);
        net_send_message(NM_INFO_STATUS,net.net_slots[net.net_civ_slots[civ3]].id,0
                           ,0,0,0,0,0,0,0);
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the treaty broken and the allies at war
   told. */
static void unit_resolve_attack_treaty_messages(uptr civ,uptr civ2,byte civ5,int show_treaty_msg,
                                                           int ally_warning)
{
  iptr name_str;
  uptr civ3;
  if ((autoplay.active == 0) && ((show_treaty_msg != 0 || (ally_warning != 0)))) {
    name_str = (iptr)(civ_name_plural(civ2));
    text_set_string_arg(0,(const char *)name_str);
    name_str = (iptr)(civ_name_plural(civ));
    text_set_string_arg(1,(const char *)name_str);
    if (game.multiplayer < 3) {
      if (show_treaty_msg == 0) {
        if (ally_warning == 1) {
          popup_game((const char *)&str_allyunderattack_2);
          diplo_ally_demand_help(civ2,civ);
        }
        else {
          popup_game((const char *)&str_allyattacking_2);
        }
      }
      else {
        popup_game((const char *)&str_cancelpeace_4);
      }
    }
    else if (autoplay.active == 0) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if (*(int *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) != 0) {
          if (human_civ[0] == civ3) {
            popup_game((const char *)&str_cancelpeace_5);
          }
          else {
            net_send_text_args(0x3a,net.net_slots[net.net_civ_slots[civ3]].id
                               ,2,0,0,0);
          }
        }
        if ((*(int *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) != 0) && (autoplay.active == 0)) {
          if (human_civ[0] == civ3) {
            if (*(int *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) == 1) {
              popup_game((const char *)&str_allyunderattack_3);
              diplo_ally_demand_help(civ2,civ);
            }
            else {
              popup_game((const char *)&str_allyattacking_3);
            }
          }
          else if (*(int *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) == 1) {
            net_send_text_args(0x3b,net.net_slots[net.net_civ_slots[civ3]].id
                               ,2,0,0,0);
            if ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0) {
              net_send_message(NM_ASK_ALLY,net.net_slots[net.net_civ_slots[civ3]].id,civ2,civ,0
                                 ,0,0,0,0,0);
            }
          }
          else {
            net_send_text_args(0x3c,net.net_slots[net.net_civ_slots[civ3]].id
                               ,2,0,0,0);
          }
        }
      }
    }
  }
}

/* Part of unit_resolve_attack (00580341): the viewers are shown the two units
   (the attacker drawn on its tile, the defender on top of its stack). */
static void unit_resolve_attack_show_start(iptr unit,iptr unit2,uptr civ,uptr civ2,iptr x,int y,
                                                      int x2,int y2,int *civ_sees)
{
  int city2;
  uptr msg_civ;
  int view;
  uptr civ3;
    unit_set_seen_by(unit,civ2);
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if (civ_sees[civ3] != 0) {
        if (human_civ[0] == civ3) {
          city2 = mapviews_ensure_visible(x,y,civ);
          if (city2 == 0) {
            map_redraw_tile_all_views(x2,y2);
          }
        }
        else {
          msg_civ = civ2;
          if (civ == civ3) {
            msg_civ = civ;
          }
          net_send_message(NM_POSSIBLY_CENTER,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,msg_civ,0,0,0,0,0);
          net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[civ3]].id,
                             x2,y2,0,0,0,0,0,0);
        }
      }
    }
    unit_lift_off_map(unit,1);
    for (view = 0; view < 8; view++) {
      if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
        map_views_block.map_view_recs[view].draw_unit_at(unit2);
        map_views_block.map_view_recs[view].blit_tile(x,y);
      }
    }
    if ((unit_types[units[unit].type].flags & (UTYPE_SUBMARINE | UTYPE_DESTROYED_AFTER_ATTACK)) != 0) {
      ui_delay_ticks(10);
    }
}

/* Part of unit_resolve_attack (00580341): the attack animated (and relayed to the
   other human players). */
static void unit_resolve_attack_animate(iptr unit,iptr dir,iptr unit2,iptr x,int y,int x2,int y2,
                                                   int *civ_sees)
{
  uptr civ3;
    combat_anim.defender = unit2;
    combat_anim.defender_x = x;
    combat_anim.defender_y = y;
    if (2 < game.multiplayer) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_SET_SLIDE_SOUND,net.net_slots[net.net_civ_slots[civ3]].id,
                             prefs_main_menu_choice_block.snd_last_effect_id,prefs_main_menu_choice_block.snd_last_effect_arg,0,0,0,0,0,0);
          net_send_message(NM_SLIDE_PIECE,net.net_slots[net.net_civ_slots[civ3]].id,
                             unit,x2,y2,dir,combat_anim.defender,0,0,0);
        }
      }
    }
    map_animate_unit_move(unit,x2,y2,dir,0xffffffff,0xffffffff);
    unit_stack_relocate_all(unit,x2,y2,0);
    map_redraw_combat_tiles(x,y,x2,y2);
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,x2,y2,0,0,0,0);
        }
      }
    }
}

/* Part of unit_resolve_attack (00580341): after a won attack: an aircraft's moves
   used, a missile spent, an AI's attack on an emptied city answered. */
static void unit_resolve_attack_after_win(uptr civ,byte civ4,iptr x,int y,int x2,int y2,
                                                     uint attacker_won,i32 &result)
{
  int unit3;
  int city2;
    unit3 = (int)game.moving_unit;
    if ((unit_types[units[unit3].type].domain == DOMAIN_AIR) &&
       (unit_types[units[unit3].type].range != 1)) {
      unit_exhaust_moves(unit3);
    }
    if ((((attacker_won == 0) || ((1 << (civ4 & 0x1f) & (uint)game.human_players) != 0)) ||
        ((int)sprite_picker_sprite_block.combat_defender_city < 0)) || (city2 = tile_first_unit(x,y), -1 < city2)) {
      if ((attacker_won != 0) && ((unit_types[units[unit3].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)) {
        unit_delete(unit3,1);
        result = 0;
      }
    }
    else {
      if ((unit_types[units[unit3].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
        if (!(((unit_types[units[unit3].type].domain == DOMAIN_AIR) &&
              (unit_types[units[unit3].type].range != '\0')) &&
             (units[unit3].counter = units[unit3].counter + 1,
             (char)unit_types[units[unit3].type].range <= (char)units[unit3].counter))) {
          ai_respond_to_attack(civ,x,y);
          result = 0;
        }
      }
      else {
        unit_delete(unit3,1);
        map_redraw_neighbors_all_views(x2,y2);
        ai_respond_to_attack(civ,x,y);
        result = 0;
      }
    }
}

/* 00580341 auto */
i32 __cdecl unit_resolve_attack(iptr unit,iptr dir,int execute)

{
  byte civ4;
  byte civ5;
  bool any_viewer;
  bool explosion_shown;
  bool barbarian_ransom;
  uptr civ;
  int x2;
  int y2;
  int y;
  iptr x;
  uptr civ2;
  iptr max_hp;
  int unit3;
  bool defender_reroll;
  int max_moves;
  uint attacker_won;
  iptr improvement;
  int ally_warning;
  int explosion_kind;
  int attack_power;
  uint moves_avail;
  int att_firepower;
  int defense2;
  uptr loser_civ;
  int civ_sees[8];
  i32 result;
  int def_firepower;
  int show_treaty_msg;
  int scramble_msg;
  int sdi_bonus;
  uptr winner_civ;
  uptr civ3;
  int target_y;
  int shore_bombard;
  iptr unit2;
  iptr target_x;
  
  scramble_msg = 0;
  improvement = 0;
  defender_reroll = false;
  barbarian_ransom = false;
  show_treaty_msg = 0;
  ally_warning = 0;
  shore_bombard = 0;
  sdi_bonus = 0;
  explosion_kind = 0;
  result = 1;
  for (civ3 = 1; (int)civ3 < 8; civ3++) {
    *(i32 *)(civ3 * 4 + (iptr)(((u8 *)&sprite_picker_sprite_block.unit_sound_effects_by_ctrl + 212))) = 0;
    *(i32 *)(civ3 * 4 + (iptr)((u8 *)sprite_picker_sprite_block.combat_kill_count_rest)) = 0;
  }
  civ4 = units[unit].owner;
  civ = (uptr)(char)civ4;
  x2 = (int)units[unit].x;
  y2 = (int)units[unit].y;
  attack_power = unit_attack_strength(unit);
  att_firepower = (int)(char)unit_types[units[unit].type].firepower;
  y = unit_moves_left(unit);
  if (y < (int)(uint)cosmic.road_move_mult) {
    moves_avail = unit_moves_left(unit);
  }
  else {
    moves_avail = (uint)cosmic.road_move_mult;
  }
  if (execute != 0) {
    attack_power = (int)(attack_power * moves_avail) / (int)(uint)cosmic.road_move_mult;
  }
  x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
  y = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y2;
  target_y = y;
  target_x = x;
  unit2 = tile_first_unit(x,y);
  if (unit2 < 0) {
    return 999;
  }
  unit2 = unit_stack_best_defender(unit2,unit);
  civ5 = units[unit2].owner;
  civ2 = (uptr)(char)civ5;
  defense2 = unit_defense_strength(unit2,0,unit);
  def_firepower = (int)(char)unit_types[units[unit2].type].firepower;
  if (((unit_types[units[unit].type].role == ROLE_AIR_SUPERIORITY) &&
      (unit_types[units[unit2].type].domain == DOMAIN_AIR)) &&
     (unit_types[units[unit2].type].range == '\0')) {
    defense2 -= (defense2 >> 1);
    def_firepower = 1;
  }
  if ((unit_types[units[unit2].type].flags & UTYPE_X2_VS_HORSE) != 0) {
    max_moves = unit_max_moves(unit);
    if (mp_doubles.moves != 0) {
      max_moves -= (int)(char)unit_types[units[unit].type].move / 2;
    }
    if ((((uint)cosmic.road_move_mult * 2 == max_moves) &&
        (unit_types[units[unit].type].domain == DOMAIN_LAND)) &&
       (max_hp = unit_max_hp(unit), max_hp == 10)) {
      defense2 += (defense2 >> 1);
    }
  }
  if (((unit_types[units[unit2].type].flags & UTYPE_X2_VS_AIR) != 0) &&
     (unit_types[units[unit].type].domain == DOMAIN_AIR)) {
    if ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0) {
      defense2 = defense2 * 3;
    }
    else {
      defense2 = defense2 * 5;
    }
  }
  unit_resolve_attack_city_defenses(unit,unit2,execute,defense2,improvement,sdi_bonus,scramble_msg);
  if ((units[unit].type == UNIT_PARTISANS) && (unit_types[units[unit2].type].attack == '\0')) {
    attack_power = attack_power << 3;
  }
  if ((unit_types[units[unit].type].domain == DOMAIN_SEA) &&
     (unit_types[units[unit2].type].domain == DOMAIN_LAND)) {
    att_firepower = 1;
    def_firepower = 1;
  }
  if ((unit_types[units[unit2].type].flags & UTYPE_SUBMARINE) != 0) {
    def_firepower = 1;
  }
  if (((unit_types[units[unit].type].flags & UTYPE_SUBMARINE) != 0) &&
     ((1 << (civ5 & 0x1f) & (uint)game.human_players) == 0)) {
    civ_clear_standing_goals_near(civ2,0x12,x,y,3);
    civ_add_standing_goal(civ2,x,y,0x12,2);
  }
  if (((unit_types[units[unit2].type].domain == DOMAIN_SEA) &&
      (unit3 = tile_is_ocean(x,y), unit3 == 0)) &&
     (unit_types[units[unit].type].domain != DOMAIN_SEA)) {
    def_firepower = 1;
    if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
      shore_bombard = 1;
      att_firepower = att_firepower << 1;
    }
    else {
      shore_bombard = 1;
      att_firepower = att_firepower << 1;
    }
  }
  unit_resolve_attack_barbarian_odds(unit2,civ,civ2,civ5,attack_power,defense2,defender_reroll);
  if (execute == 0) {
    if ((game.rules & RULE_NORMAL_COMBAT) != 0) {
      x2 = unit_hp_remaining(unit);
      attack_power = (x2 + att_firepower * 8) * attack_power;
      x2 = unit_hp_remaining(unit2);
      defense2 = (x2 + def_firepower * 8) * defense2;
    }
    for (; 3999 < attack_power; attack_power = attack_power >> 1) {
      defense2 = defense2 >> 1;
    }
    x2 = (attack_power << 3) / (defense2 + 1);
    if (x2 < 0x401) {
      return x2;
    }
    return 0x400;
  }
  if ((unit_sound_editor_window_block.unit_move_flags & 2) != 0) {
    units[unit].moves_used += cosmic.road_move_mult;
  }
  if (((1 << (civ4 & 0x1f) & (uint)game.human_players) != 0) &&
     (unit3 = diplo_confirm_attack(civ,civ2,0xd), unit3 != 0)) {
    return 1;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0) {
    return 1;
  }
  if ((civ != 0) && (civ2 != 0)) {
    game.peace_turns = 0;
  }
  diplo_set_treaty_flags(civ,civ2,0x200);
  unit_resolve_attack_break_treaty(unit,civ,civ2,civ4,civ5,attack_power,show_treaty_msg,ally_warning);
  unit3 = diplo_force_war(civ,civ2);
  if (civ != 0) {
    if ((game.difficulty < DIFF_PRINCE) && ((1 << (civ5 & 0x1f) & (uint)game.human_players) != 0)) {
      attack_power = attack_power >> 1;
    }
    if ((game.difficulty == DIFF_CHIEFTAIN) && ((1 << (civ4 & 0x1f) & (uint)game.human_players) != 0)) {
      attack_power = attack_power << 1;
    }
  }
  any_viewer = unit_resolve_attack_viewers(unit,unit2,civ,civ2,civ_sees);
  if (any_viewer) {
    unit_resolve_attack_show_start(unit,unit2,civ,civ2,x,y,x2,y2,civ_sees);
    unit_resolve_attack_announce(unit,unit2,civ,civ2,civ5,x,y,x2,y2,improvement,sdi_bonus,scramble_msg,
                                 shore_bombard,civ_sees);
    explosion_kind = unit_resolve_attack_sound(unit,unit2);
    unit_resolve_attack_animate(unit,dir,unit2,x,y,x2,y2,civ_sees);
  }
  if ('b' < (char)unit_types[units[unit].type].attack) {
    unit_resolve_attack_nuclear(unit,civ,civ2,civ4,x,y,x2,y2,civ_sees);
    return 0;
  }
  explosion_shown = false;
  if ((att_firepower == 0) && (def_firepower == 0)) {
    attacker_won = 0;
  }
  else {
    attacker_won = unit_resolve_attack_fight(unit,unit2,x,y,x2,y2,attack_power,defense2,att_firepower,
                                             def_firepower,defender_reroll,any_viewer,explosion_kind,
                                             civ_sees,explosion_shown);
  }
  if (attacker_won == 0) {
    turn_flags.active_unit_lost_in_combat = 1;
  }
  unit_resolve_attack_show_result(unit,unit2,x,y,x2,y2,attacker_won,any_viewer,explosion_shown,
                                  explosion_kind,civ_sees);
  combat_anim.defender = 0xffffffff;
  sprite_picker_sprite_block.combat_kill_count = 0;
  unit_resolve_attack_veterans(unit,unit2,civ,civ2,attacker_won,attack_power,defense2);
  unit_resolve_attack_remove_loser(unit,unit2,civ,civ2,civ5,x,y,x2,y2,target_x,target_y,attacker_won,
                                   winner_civ,loser_civ,barbarian_ransom,show_treaty_msg);
  net_send_state_diff(0xff,2,0,0,0);
  if (any_viewer) {
    if (2 < game.multiplayer) {
      for (civ3 = 1; (int)civ3 < 8; civ3++) {
        if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
          net_send_message(NM_DRAW_TWO,net.net_slots[net.net_civ_slots[civ3]].id,
                             x,y,x2,y2,0,0,0,0);
        }
      }
    }
    map_redraw_combat_tiles(x,y,x2,y2);
  }
  unit_resolve_attack_multiple_kills(winner_civ,loser_civ);
  if (barbarian_ransom) {
    unit_resolve_attack_ransom(civ,civ_sees);
  }
  if (unit3 != 0) {
    diplo_allies_join_war(civ2,civ);
  }
  unit_resolve_attack_treaty_messages(civ,civ2,civ5,show_treaty_msg,ally_warning);
  advisor_refresh_if_open(2);
  if (2 < game.multiplayer) {
    net_send_message(NM_REPORTS_UPDATE,0xff,2,0,0,0,0,0,0,0);
  }
  if (attacker_won == 0) {
    result = 0;
  }
  else {
    unit_resolve_attack_after_win(civ,civ4,x,y,x2,y2,attacker_won,result);
  }
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    for (civ3 = 1; (int)civ3 < 8; civ3++) {
      if ((human_civ[0] != civ3) && (civ_sees[civ3] != 0)) {
        net_send_message(NM_ABORT_KABOOM,net.net_slots[net.net_civ_slots[civ3]].id,0,0
                           ,0,0,0,0,0,0);
        net_send_message(NM_DRAW_AREA,net.net_slots[net.net_civ_slots[civ3]].id,
                           x2,y2,0,0,0,0,0,0);
      }
    }
  }
  return result;
}

/* 0058a5b0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl unit_sound_editor_global_init(void)

{
  unit_sound_editor_static_init();
  unit_sound_editor_reg_atexit();
  return;
}

/* 0058a5ca auto */
void __cdecl unit_sound_editor_static_init(void)

{
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::construct();
  return;
}

/* 0058a5e4 auto */
void __cdecl unit_sound_editor_reg_atexit(void)

{
  _atexit(unit_sound_editor_static_dtor);
  return;
}

/* 0058a601 auto */
void __cdecl unit_sound_editor_static_dtor(void)

{
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::destruct();
  return;
}

/* 0058a61b auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
unit_sound_editor_create_wnd(const char *title,uint style,int x,int y,int width,int height,int resize_border,
            int caption_height,int border_width)

{
  iptr parent2;
  uint wnd_style;
  
  if (title == 0) {
    crt_strcpy(((i8 *)&unit_sound_editor.title),((i8 *)&unit_sound_editor_empty_title));
  }
  else {
    crt_strcpy(((i8 *)&unit_sound_editor.title),title);
  }
  unit_sound_editor.text_font = (iptr)&ui_text_font;
  unit_sound_editor.title_font = (iptr)&caption_font_block.caption_font;
  unit_sound_editor.title_small_font = (iptr)&caption_small_font_block.caption_small_font;
  unit_sound_editor.style_flags = style;
  unit_sound_editor.caption_height = caption_height;
  unit_sound_editor.border_width = border_width;
  unit_sound_editor.resize_border = resize_border;
  unit_sound_editor.caption_button_count = 0;
  unit_sound_editor.caption_button_cb = 0;
  if ((style & 4) != 0) {
    unit_sound_editor.caption_height = ui_metrics.caption_height;
    unit_sound_editor.border_width = ui_metrics.border_width;
  }
  if ((style & 8) == 0) {
    wnd_style = 0x202;
  }
  else {
    wnd_style = 0x802;
  }
  if (unit_sound_editor.caption_height != 0) {
    wnd_style = wnd_style | 0x400;
  }
  if (resize_border != 0) {
    wnd_style = wnd_style | 0x1000;
  }
  if ((style & 2) == 0) {
    width += unit_sound_editor.border_width * 2;
    height = height + unit_sound_editor.caption_height + unit_sound_editor.border_width;
  }
  if ((style & 1) != 0) {
    x = (caption_font_block.screen_width >> 1) - (width >> 1);
    y = (caption_font_block.screen_height >> 1) - (height >> 1);
  }
  if (editor_active.wnd == 0) {
    parent2 = 0;
  }
  else {
    parent2 = (iptr)&editor_active.wnd->window;
  }
  ((DibSurface *)((i8 *)&unit_sound_editor.window))->wnd_create_with_surface(0,wnd_style,x,y,width,height,(iptr)&game_palette_block.game_palette,
             parent2);
  if (unit_sound_editor.caption_height != 0) {
    ((WndBase *)((i8 *)&unit_sound_editor.wnd))->set_caption_height(unit_sound_editor.caption_height);
  }
  if (unit_sound_editor.resize_border != 0) {
    ((WndBase *)((i8 *)&unit_sound_editor.wnd))->set_resize_border(unit_sound_editor.resize_border);
  }
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->layout();
  return;
}

/* 0058a905 auto */
/* WARNING: Removing unreachable block (ram,0x0058a97f) */
/* WARNING: Removing unreachable block (ram,0x0058a9fb) */

void __cdecl unit_sound_editor_pick_wav(int slot)

{
  i32 filter_arg;
  iptr title;
  int ret;
  DWORD attrs;
  i8 *file_buf;
  char *filter;
  i32 dlg_arg;
  i8 *default_ext;
  i32 fmt_arg2;
  i32 fmt_arg3;
  i8 wav_path[256];
  char dest_path[0x140];  /* (port) 256 in the original: scenario_dir_path (264),
                             SOUND\ and the name (BF-296) */
  Dialog dlg;
  int choice;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if (sprite_picker_sprite_block.unit_sound_effects_by_ctrl[slot] >= 0) {
    dlg_set_parent_window((iptr)&unit_sound_editor.wnd);
    do {
      fmt_arg3 = 0;
      fmt_arg2 = 0;
      dlg_arg = 0;
      filter_arg = (iptr)label_string(label_table.string_ids[0x222]);
      _sprintf(dest_path,str_s_wav_c_wav_c_c,filter_arg,dlg_arg,fmt_arg2,fmt_arg3);
      crt_strcpy(wav_path,str_wav);
      dlg_arg = 0;
      filter_arg = 1;
      default_ext = ((i8 *)&unit_sound_wav_default_ext);
      filter = dest_path;
      file_buf = wav_path;
      title = (iptr)label_string(label_table.string_ids[0x223]);
      ret = ui_file_name_dialog((iptr)&unit_sound_editor.wnd,(const char *)title,(iptr)file_buf,(iptr)filter,(iptr)default_ext,filter_arg,dlg_arg);
      if (ret == 0) {
        return;
      }
      dlg.load_text((const char *)&str_debug_10,(const char *)&str_playsound,0x801);
      do {
        dlg.result_extra = 0;
        choice = dlg.show_modal(0);
        if (dlg.result_extra != 0) {
          snd_wave_play((const char *)wav_path,0,0,0);
        }
      } while (dlg.result_extra != 0);
      dlg.close();
      dlg.init_defaults();
    } while (choice != 0);
    crt_strcpy(dest_path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(dest_path,str_sound_5);
    attrs = GetFileAttributesA(dest_path);
    if (attrs != 0xffffffff) {
      crt_strcat(dest_path,((i8 *)&unit_sound_path_sep_str));
      crt_strcat(dest_path,((char *)&str_aircombt) + sprite_picker_sprite_block.unit_sound_effects_by_ctrl[slot] * 9);
      crt_strcat(dest_path,((i8 *)&unit_sound_wav_suffix_str));
      ret = file_exists((const char *)dest_path);
      if ((ret == 0) || (ret = crt_remove(dest_path), ret == 0)) {
        file_copy((iptr)wav_path,(iptr)dest_path);
      }
    }
  }
  return;
}

/* 0058abee auto */
void __cdecl unit_sound_editor_ok_clicked(void)

{
  main_frame_bg_surface_block.unit_sound_editor_running = 0;
  ((ModalMenuWindow *)((i8 *)&unit_sound_editor.wnd))->end_modal_loop();
  return;
}

/* 0058ac13 auto */
void __cdecl unit_sound_editor_cancel_clicked(void)

{
  main_frame_bg_surface_block.unit_sound_editor_running = 0;
  ((ModalMenuWindow *)((i8 *)&unit_sound_editor.wnd))->end_modal_loop();
  return;
}

/* 0058ac38 auto */
void __cdecl unit_sound_editor_redraw(void)

{
  int row_height;
  int width_adj;
  int col_width;
  int top;
  int left;
  int slot;
  i8 rect[16];
  
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::draw_caption();
  gfx_fill_rect((iptr)&unit_sound_editor.window,(RECT *)&unit_sound_editor.fill_rect,0x1d);
  text_set_surface((iptr)&unit_sound_editor.window);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(10,10,0,0);
  row_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  row_height += 8;
  width_adj = unit_sound_editor.client_width + (unit_sound_editor.client_width >> 0x1f & 3U);
  col_width = width_adj >> 2;
  for (slot = 0; slot < 6; slot++) {
    top = (row_height * 3) / 2 + (slot / 3) * row_height * 4 + unit_sound_editor.client_y;
    left = (slot % 3) * col_width * 5;
    left = ((int)(left + (left >> 0x1f & 3U)) >> 2) + ((int)(col_width + (width_adj >> 0x1f & 3U)) >> 2)
            + unit_sound_editor.client_x;
    rect_set_xywh((iptr)rect,left,top,col_width,row_height);
    gfx_draw_bevel_rect((iptr)&unit_sound_editor.window,(RECT *)rect,10,10);
    if (-1 < main_frame_bg_surface_block.unit_sound_effects[slot]) {
      text_clear();
      text_append_str((const char *)(((char *)&str_aircombt) + main_frame_bg_surface_block.unit_sound_effects[slot] * 9));
      text_append_str((const char *)&str_wav_2);
      text_draw_centered((iptr)&unit_sound_editor.window,(const char *)&textbuf.buf,left + col_width / 2,top + 4,0);
    }
  }
  text_clear();
  text_append_label(0x25b);
  text_draw_centered((iptr)&unit_sound_editor.window,(const char *)&textbuf.buf,unit_sound_editor.client_x + unit_sound_editor.client_width / 2,unit_sound_editor.client_y + 10,0);
  ((DibSurface *)((i8 *)&unit_sound_editor.window))->wnd_blit_surface();
  return;
}

/* 0058ae6c auto */
void __cdecl unit_sound_editor_on_dblclick(int x,int y)

{
  int row_height;
  int width_adj;
  int col_width;
  int hit;
  int slot;
  RECT rect2;
  u32 saved_options;
  
  y -= unit_sound_editor.client_y;
  x -= unit_sound_editor.client_x;
  row_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  row_height += 8;
  width_adj = unit_sound_editor.client_width + (unit_sound_editor.client_width >> 0x1f & 3U);
  col_width = width_adj >> 2;
  slot = 0;
  while( true ) {
    if (5 < slot) {
      return;
    }
    hit = (slot % 3) * col_width * 5;
    rect_set_xywh((iptr)&rect2,
                       ((int)(hit + (hit >> 0x1f & 3U)) >> 2) +
                       ((int)(col_width + (width_adj >> 0x1f & 3U)) >> 2),
                       (row_height * 3) / 2 + (slot / 3) * row_height * 4,col_width,row_height);
    if ((-1 < main_frame_bg_surface_block.unit_sound_effects[slot]) &&
       (hit = point_in_rect_ltrb(x,y,rect2.left,rect2.top,rect2.right,rect2.bottom), hit != 0))
    break;
    slot++;
  }
  saved_options = game.game_options;
  game.game_options = game.game_options | OPT_SOUND;
  snd_play_effect(main_frame_bg_surface_block.unit_sound_effects[slot],0,0,0);
  game.game_options = saved_options;
  return;
}

/* 0058afb6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl unit_sound_editor_set_slots(int unit_type,iptr buttons)

{
  int orders_effect;
  int i;
  
  orders_effect = -1;
  for (i = 0; i < 6; i++) {
    main_frame_bg_surface_block.unit_sound_effects[i] = 0xffffffff;
  }
  if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8] != 0) {
    if (unit_type == UNIT_TEST_1) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x7d;
    }
    else if (unit_type == UNIT_TEST_2) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x7e;
    }
    else if (unit_type == UNIT_TEST_3) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x7f;
    }
    else if (unit_type == UNIT_TEST_4) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x80;
    }
    else if (unit_type == UNIT_TEST_5) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x81;
    }
    else if (unit_type == UNIT_TEST_6) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x82;
    }
    else if (unit_type == UNIT_TEST_7) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x83;
    }
    else if (unit_type == UNIT_TEST_8) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x84;
    }
    else if (unit_type == UNIT_EXTRA_LAND) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x65;
    }
    else if (unit_type == UNIT_EXTRA_SHIP) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x66;
    }
    else if (unit_type == UNIT_EXTRA_AIR) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x67;
    }
    else if (((((i8 *)&editor_text_dlg_wnd_block.editor_item_flags_byte1))[unit_type * 0x58] & 0x10) == 0) {
      if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3] == 1) {
        if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[7] == 0) {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x21;
          if (unit_type < 0x1e) {
            main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x17;
          }
          else {
            main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x4f;
          }
        }
        else if (unit_type < 0x1e) {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0;
          main_frame_bg_surface_block.unit_sound_bomb_effect_i32 = 0x18;
          main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x17;
          main_frame_bg_surface_block.unit_sound_sputter_effect_i32 = 0x1a;
        }
        else {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x52;
          main_frame_bg_surface_block.unit_sound_bomb_effect_i32 = 0x50;
          main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x4f;
          main_frame_bg_surface_block.unit_sound_sputter_effect_i32 = 0x4e;
        }
      }
      else if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3] == 2) {
        if (((i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[12] & 8) == 0) {
          orders_effect = 6;
          if (((unit_type == UNIT_BATTLESHIP) || (unit_type == UNIT_CRUISER)) || ((unit_type == UNIT_AEGIS_CRUISER || (unit_type == UNIT_DESTROYER))))
          {
            orders_effect = 0x2e;
          }
        }
        else {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x4d;
        }
      }
      else if (unit_type == UNIT_ELEPHANT) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x19;
      }
      else if ((((unit_type == UNIT_HORSEMEN) || (unit_type == UNIT_CHARIOT)) || (unit_type == UNIT_KNIGHTS)) || (unit_type == UNIT_CRUSADERS))
      {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x4a;
      }
      else if ((unit_type == UNIT_DRAGOONS) || (unit_type == UNIT_CAVALRY)) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0xc;
      }
      else if (((unit_type == UNIT_MUSKETEERS) || (unit_type == UNIT_RIFLEMEN)) || ((unit_type == UNIT_ALPINE_TROOPS || (unit_type == UNIT_PARTISANS)))) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x22;
      }
      else if ((((unit_type == UNIT_FANATICS) || (unit_type == UNIT_PARATROOPERS)) || (unit_type == UNIT_MARINES)) || (unit_type == UNIT_MECH_INF)) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x26;
      }
      else if ((unit_type < 0x16) || (0x1a < unit_type)) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x49;
      }
      else if (unit_type == UNIT_CATAPULT) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 10;
      }
      else {
        orders_effect = 0x28;
        if (0x17 < unit_type) {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x1c;
        }
      }
    }
    else if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8] < 99) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x29;
    }
    else {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x32;
    }
  }
  main_frame_bg_surface_block.unit_sound_orders_effect = orders_effect;
  if (-1 < orders_effect) {
    main_frame_bg_surface_block.unit_sound_explosion_effect_i32 = 0x23;
  }
  for (i = 0; i < 6; i++) {
    if (main_frame_bg_surface_block.unit_sound_effects[i] < 0) {
      ((Button *)(i * 0x3c + buttons))->disable();
    }
    else {
      ((Button *)(i * 0x3c + buttons))->enable();
    }
  }
  return;
}

/* 0058b47e auto */
/* WARNING: Removing unreachable block (ram,0x0058b6f2) */
/* WARNING: Removing unreachable block (ram,0x0058b77b) */
/* WARNING: Removing unreachable block (ram,0x0058b5ea) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl unit_sound_editor_run(const char *title,i32 unit_type)

{
  int col_offset;
  iptr label;
  int i;
  Button cancel_btn;
  int btn_y;
  Button slot_btns[6];
  int btn_height;
  int btn_x;
  int btn_width;
  i8 rect[16];
  Button ok_btn;
  
  button_ctor(&ok_btn);
  ON_SCOPE_EXIT(ctrl_dtor(&ok_btn));
  button_ctor(&cancel_btn);
  ON_SCOPE_EXIT(ctrl_dtor(&cancel_btn));
  crt_vector_ctor_iterator(slot_btns,0x3c,6,button_ctor,ctrl_dtor);
  ON_SCOPE_EXIT(crt_vector_dtor_iterator(&slot_btns,0x3c,6,ctrl_dtor));
  main_frame_bg_surface_block.unit_sound_editor_running = 1;
  unit_sound_editor_create_wnd(title,0xd,0,0,0x21c,0x118,0,0,0);
  btn_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  btn_height += 8;
  btn_width = (int)(unit_sound_editor.client_width + (unit_sound_editor.client_width >> 0x1f & 3U)) >> 2;
  for (i = 0; i < 6; i++) {
    btn_y = btn_height * 3 + (i / 3) * btn_height * 4 + unit_sound_editor.client_y;
    col_offset = (i % 3) * btn_width * 5;
    btn_x = ((int)(col_offset + (col_offset >> 0x1f & 3U)) >> 2) +
               ((int)(btn_width + (btn_width >> 0x1f & 3U)) >> 2) + unit_sound_editor.client_x;
    rect_set_xywh((iptr)rect,btn_x,btn_y,btn_width,btn_height);
    label = (iptr)label_string(label_table.string_ids[unit_sound_button_label_ids[i]]);
    (slot_btns + i)->create((iptr)&unit_sound_editor.wnd,i + 0xc9,(iptr)rect,(const char *)label);
    (slot_btns + i)->set_callback(unit_sound_editor_pick_wav);
  }
  unit_sound_editor_set_slots(unit_type,(iptr)slot_btns);
  btn_width = (unit_sound_editor.client_width - 6) / 2;
  btn_y = (unit_sound_editor.client_y + unit_sound_editor.client_height) - (btn_height + 2);
  btn_x = unit_sound_editor.client_x + 2;
  rect_set_xywh((iptr)rect,btn_x,btn_y,btn_width,btn_height);
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_btn.create((iptr)&unit_sound_editor.wnd,0x65,(iptr)rect,(const char *)label);
  ok_btn.set_callback((CtrlNotifyFn)unit_sound_editor_ok_clicked);
  btn_x = btn_x + btn_width + 2;
  rect_set_xywh((iptr)rect,btn_x,btn_y,btn_width,btn_height);
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_btn.create((iptr)&unit_sound_editor.wnd,0x66,(iptr)rect,(const char *)label);
  cancel_btn.set_callback((CtrlNotifyFn)unit_sound_editor_cancel_clicked);
  cancel_btn.set_cancel_button();
  ((WndHandlers *)((i8 *)&unit_sound_editor.wnd_handlers))->set_dblclick_handler(unit_sound_editor_on_dblclick);
  ((SurfaceWindow *)((i8 *)&unit_sound_editor.window))->set_redraw_callback(unit_sound_editor_redraw);
  ((SurfaceWindow *)((i8 *)&unit_sound_editor.window))->repaint();
  ((SurfaceWindow *)((i8 *)&unit_sound_editor.window))->SurfaceWindow::show();
  ((ModalMenuWindow *)((i8 *)&unit_sound_editor.wnd))->run_modal_loop();
  while (main_frame_bg_surface_block.unit_sound_editor_running != 0) {
    win_pump_one_message();
  }
  dlg_set_parent_window(0);
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::close();
  return;
}

/* 0058bdfd auto */
void __cdecl unit_cmd_wait(void)

{
  units[game.active_unit].flags = units[game.active_unit].flags | UNITF_WAITING;
  blink.unit_active = 0;
  blink.unit_phase = 0;
  mapviews_redraw_cursor();
  unit_activate_next(0);
  return;
}

/* 0058be56 auto */
void __cdecl unit_cmd_build_city(void)

{
  iptr unit;
  iptr civ;
  iptr is_ocean;
  iptr city2;
  iptr x;
  int ret;
  uptr city;
  i8 city_name[260];
  int y2;
  int x2;
  int y3;
  iptr dir;
  
  blink.unit_active = 0;
  unit = (iptr)game.active_unit;
  x2 = (int)units[unit].x;
  y2 = (int)units[unit].y;
  civ = (iptr)units[unit].owner;
  if (unit_types[units[unit].type].role == ROLE_SETTLE) {
    is_ocean = tile_is_ocean(x2,y2);
    if (is_ocean == 0) {
      city2 = city_at(x2,y2);
      if (city2 < 0) {
        for (dir = 0; dir < 8; dir++) {
          x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x2);
          y3 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y2;
          ret = map_in_bounds(x,y3);
          if ((ret != 0) && (ret = tile_city_owner(x,y3), -1 < ret)) {
            x = city_at(x,y3);
            popup_game_with_city((const char *)&str_adjacentcity,x);
            return;
          }
        }
        city = city_create(x2,y2,civ);
        if (-1 < (int)city) {
          ret = dlg_game_text_input((const char *)&str_namecity,0xf,(char *)&cities[city].name,(iptr)city_name);
          if (ret == 0) {
            unit_found_city(unit,city,(iptr)city_name);
            text_set_string_arg(0,(const char *)&cities[city].name);
            text_clear();
            text_append_year((int)game.year);
            text_set_string_arg(1,(const char *)&textbuf.buf);
            snd_play_effect(SND_BLDCITY,1,0,0);
            popup_city_picture_zoom((const char *)&str_founded,city,0x4c,1,civ);
            if ((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && (civs[civ].num_cities == 1)) {
              popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_firstproduct,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
            }
            city_turn.processing = 1;
            ((CityWindow *)((i8 *)&city_wnd.window))->begin_notice();
            ((CityWindow *)((i8 *)&city_wnd.window))->open(city);
            ((CityWindow *)((i8 *)&city_wnd.window))->end_notice();
            city_turn.processing = 0;
          }
          else {
            city_delete(city,0);
            leaders[civs[civ].tribe].city_name_index =
                 leaders[civs[civ].tribe].city_name_index + 0xff;
          }
        }
      }
      else if ((int)cities[city2].size < (int)(uint)cosmic.aqueduct_size) {
        cities[city2].size++;
        unit_delete(unit,1);
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city2);
        map_redraw_area_all_views(x2,y2,1,human_civ[0],1);
      }
      else {
        text_set_number_arg_u8(0,CONCAT31(cities[city2].size >> 7,cosmic.aqueduct_size));
        popup_game_with_city((const char *)&str_only10,city2);
      }
    }
    else {
      popup_game((const char *)&str_cityatsea);
    }
  }
  else {
    dlg_game_txt_with_unit_flags((const char *)&str_onlysettlers,0,(ui_metrics.large_screen == 0) - 1 & 8);
  }
  return;
}

/* 0058c295 auto */
void __cdecl unit_cmd_disband(void)

{
  char civ;
  short x;
  short y;
  iptr choice;
  int city;
  int wnd_city;
  uptr city2;
  iptr confirm;
  iptr unit;
  iptr y2;
  int x2;
  
  if (moving_units_mode == 1) {
    unit = (iptr)game.active_unit;
    if (unit < 0) {
      /* B27: no active unit: the original goes on with a square it never set
         (every mode: ours would read another one; docs/BUGS.md) */
      return;
    }
  }
  else {
    x2 = (int)map_cursor.x[0];
    y2 = (iptr)map_cursor.y[0];
    unit = tile_first_unit(x2,y2);
  }
  if ((game.multiplayer < 3) || (units[unit].owner == human_civ[0])) {
    if (unit < 0) {
      if (((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) != 0) && (game.multiplayer == '\0')) &&
         (city2 = city_at(x2,y2), -1 < (int)city2)) {
        civ = cities[city2].owner;
        text_set_string_arg_b(0,(const char *)&cities[city2].name);
        confirm = popup_game_with_city((const char *)&str_disband_2,city2);
        if (confirm == 1) {
          city_delete(city2,0);
          civ_check_destroyed((int)civ,0);
          map_redraw_all_views(human_civ[0],1);
        }
      }
    }
    else if ((units[unit].owner == human_civ[0]) || (game.reveal_map != '\0')) {
      blink.unit_active = 0;
      text_set_label_arg(0,unit_types[units[unit].type].name);
      choice = dlg_game_txt_with_unit((const char *)&str_disband,unit);
      if (choice == 1) {
        x = units[unit].x;
        y = units[unit].y;
        city = city_at((int)x,(int)y);
        if (-1 < city) {
          cities[city].shield_box =
               (short)((int)((int)(char)unit_types[units[unit].type].cost *
                            (uint)cosmic.shield_box_rows) / 2) + cities[city].shield_box;
          wnd_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (wnd_city == city) {
            city_save_build_state(city);
          }
          wnd_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (wnd_city == city) {
            ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
          }
        }
        unit_delete_with_cargo(unit,1);
        map_redraw_area_all_views((int)x,(int)y,0,human_civ[0],1);
      }
    }
  }
  return;
}

/* 0058c65e auto */
void __cdecl unit_cmd_terraform(iptr order)

{
  char transform;
  byte terrain;
  iptr unit;
  int x;
  int y;
  int civ;
  int ret;
  i32 tile_impr;
  byte *tile;
  uint impr_bits;
  
  unit = (iptr)game.active_unit;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  civ = (int)units[unit].owner;
  terrain = tile_terrain(x,y);
  blink.unit_active = 0;
  if (unit_types[units[unit].type].role != ROLE_SETTLE) {
    popup_game_with_improvement((const char *)&str_onlysettlers_2,IMP_NOTHING,8);
    return;
  }
  if (order == 10) {
    ret = civ_has_advance(civ,ADV_RADIO);
    if (ret == 0) {
      popup_game_with_advance((const char *)&str_radio,ADV_RADIO,8);
      return;
    }
    ret = tile_city_owner(x,y);
    if (((-1 < ret) || (tile_impr = tile_improvements(x,y), ((byte)tile_impr & TILE_AIRBASE) == 0x40)) ||
       (ret = tile_is_ocean(x,y), ret != 0)) {
      dlg_game_txt_with_unit((const char *)&str_cantimprove,unit);
      return;
    }
    ret = tile_airbase_owner(x,y);
    if (-1 < ret) {
      dlg_game_txt_with_unit((const char *)&str_alreadyair,unit);
      return;
    }
  }
  if (order == 4) {
    ret = civ_has_advance(civ,ADV_CONSTRUCTION);
    if (ret == 0) {
      popup_game_with_advance((const char *)&str_construction,ADV_CONSTRUCTION,8);
      return;
    }
    ret = tile_city_owner(x,y);
    if (((-1 < ret) || (ret = tile_is_ocean(x,y), ret != 0)) ||
       (tile_impr = tile_improvements(x,y), ((byte)tile_impr & TILE_AIRBASE) == 0x40)) {
      dlg_game_txt_with_unit((const char *)&str_cantimprove_2,unit);
      return;
    }
    tile_impr = tile_improvements(x,y);
    if (((byte)tile_impr & TILE_AIRBASE) == 0x40) {
      dlg_game_txt_with_unit((const char *)&str_alreadyfort,unit);
      return;
    }
  }
  if (order == 5) {
    ret = tile_city_owner(x,y);
    if ((-1 < ret) || (ret = tile_is_ocean(x,y), ret != 0)) {
      dlg_game_txt_with_unit((const char *)&str_cantimprove_3,unit);
      return;
    }
    tile = (byte *)tile_at(x,y);
    if (((*tile & 0x80) != 0) && (ret = civ_has_advance(civ,ADV_BRIDGE_BUILDING), ret == 0)) {
      popup_game_with_advance((const char *)&str_bridgebuilding,ADV_BRIDGE_BUILDING,8);
      return;
    }
    impr_bits = tile_improvements(x,y);
    if ((impr_bits & TILE_RAILROAD) != 0) {
      dlg_game_txt_with_unit((const char *)&str_alreadyroad,unit);
      return;
    }
    impr_bits = tile_improvements(x,y);
    if (((impr_bits & TILE_ROAD) != 0) && (ret = civ_has_advance(civ,ADV_RAILROAD), ret == 0)) {
      popup_game_with_advance((const char *)&str_railroads_2,ADV_RAILROAD,8);
      return;
    }
  }
  if (((order == 6) || (order == 7)) &&
     (transform = terrain_types[(uint)terrain].improve_to[order - 6], transform < '\0')) {
    if (transform == -1) {
      dlg_game_txt_with_unit((const char *)&str_cantimprove_4,unit);
      return;
    }
    ret = tile_city_owner(x,y);
    if (-1 < ret) {
      dlg_game_txt_with_unit((const char *)&str_cantimprove_5,unit);
      return;
    }
    if (order == 7) {
      tile_impr = tile_improvements(x,y);
      if (((byte)tile_impr & TILE_FARMLAND) == 8) {
        popup_game_with_sprite((const char *)&str_alreadymining,(iptr)&land_improvement_sprites_block.mine_sprite,0);
        return;
      }
    }
    else {
      ret = tile_has_irrigation_source(x,y);
      if (ret == 0) {
        dlg_game_txt_with_unit((const char *)&str_nowater,unit);
        return;
      }
      tile_impr = tile_improvements(x,y);
      if (((byte)tile_impr & TILE_FARMLAND) == 0xc) {
        dlg_game_txt_with_unit((const char *)&str_alreadyfarmland,unit);
        return;
      }
      impr_bits = tile_improvements(x,y);
      if (((impr_bits & TILE_IRRIGATION) != 0) && (civ = civ_has_advance(civ,ADV_REFRIGERATION), civ == 0)) {
        popup_game_with_advance((const char *)&str_farmland_2,ADV_REFRIGERATION,8);
        return;
      }
    }
  }
  if (order == 8) {
    civ = tile_is_ocean(x,y);
    if (civ != 0) {
      dlg_game_txt_with_unit((const char *)&str_cantimprove_6,unit);
      return;
    }
    if (units[unit].type != UNIT_ENGINEERS) {
      dlg_game_txt_with_unit_flags((const char *)&str_engineers,1,(ui_metrics.large_screen == 0) - 1 & 8);
      return;
    }
  }
  if ((order == 9) && (impr_bits = tile_improvements(x,y), (impr_bits & TILE_POLLUTION) == 0)) {
    popup_game_with_sprite((const char *)&str_nopollution,(iptr)&pollution_sprite_block.pollution_sprite,8);
  }
  else {
    map_redraw_tile_all_views(x,y);
    unit_do_terrain_work(unit,order);
  }
  return;
}

/* 0058cbe1 auto */
void __cdecl unit_cmd_home_city(void)

{
  byte city;
  iptr unit;
  int new_city;
  i8 new_home;
  
  blink.unit_active = 0;
  unit = (iptr)game.active_unit;
  new_city = city_at((int)units[unit].x,(int)units[unit].y);
  if (new_city < 0) {
    unit_go_home(unit);
  }
  else if ((unit_types[units[unit].type].role == ROLE_TRADE) && (units[unit].home_city != -1)) {
    dlg_game_txt_with_unit((const char *)&str_caravanhome,unit);
  }
  else {
    city = units[unit].home_city;
    new_home = (i8)new_city;
    units[unit].home_city = new_home;
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city((uint)city);
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(new_city);
  }
  return;
}

/* 0058cce6 auto */
void __cdecl unit_cmd_fortify(void)

{
  iptr unit;
  int x;
  int y;
  int result;
  
  blink.unit_active = 0;
  unit = (iptr)game.active_unit;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  result = tile_is_ocean(x,y);
  if ((result == 0) &&
     (((unit_types[units[unit].type].domain != DOMAIN_AIR) ||
       (result = tile_city_owner(x,y), -1 < result)) ||
      (result = tile_airbase_owner(x,y), -1 < result))) {
    units[unit].orders = ORDER_FORTIFYING;
    unit_exhaust_moves(unit);
    map_redraw_tile_all_views(x,y);
    ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit,-99,-99);
  }
  else {
    popup_game((const char *)&str_cantdo);
  }
  return;
}

/* 0058cde5 auto */
void __cdecl unit_cmd_sleep(void)

{
  char dy;
  bool ok;
  iptr unit;
  int x;
  int y;
  int result;
  iptr x2;
  iptr dir;
  
  blink.unit_active = 0;
  unit = (iptr)game.active_unit;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
    ok = false;
    result = tile_is_ocean(x,y);
    if ((result != 0) &&
       ((result = unit_stack_sum(unit,9), result != 0 ||
        (((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0 &&
         (result = unit_stack_sum(unit,10), result != 0)))))) {
      ok = true;
    }
    if ((!ok) && (result = tile_city_owner(x,y), result < 0)) {
      popup_game((const char *)&str_cantdo_2);
      return;
    }
  }
  if ((unit_types[units[unit].type].flags & UTYPE_COAST_BOUND) != 0) {
    ok = false;
    for (dir = 0; dir < 9; dir++) {
      x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
      dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
      result = map_in_bounds(x2,dy + y);
      if ((result != 0) && (result = tile_is_ocean(x2,dy + y), result == 0)) {
        ok = true;
      }
    }
    if (!ok) {
      popup_game((const char *)&str_cantdo_3);
      return;
    }
  }
  unit_set_sleep(unit);
  return;
}

/* 0058cfcd auto */
void __cdecl unit_cmd_pillage(void)

{
  iptr unit;
  int civ;
  iptr x;
  iptr y;
  iptr city_owner;
  iptr tile;
  int city;
  byte option_count;
  Dialog dialog2;
  int civ2;
  int choice;
  uint improvements_mask;
  i32 eh_next;
  Tile *map_tile;
  
  choice = 0;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  unit = (iptr)game.active_unit;
  civ = (int)units[unit].owner;
  x = (iptr)units[unit].x;
  y = (iptr)units[unit].y;
  if (unit_types[units[unit].type].attack == '\0') {
    /* a unit that cannot attack: nothing */
  }
  else if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
    city_owner = tile_city_owner(x,y);
    if (city_owner < 0) {
      map_tile = tile_at(x,y);
      improvements_mask = (uint)map_tile->improvements;
      if ((map_tile->improvements & (TILE_IRRIGATION | TILE_MINE | TILE_ROAD | TILE_FORTRESS)) == 0) {
        popup_game((const char *)&str_cantdo_5);
      }
      else {
        dialog2.load_game_section((const char *)&str_pillagewhat,1);
        city = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)&str_pillagemenu);
        if (city == 0) {
          text_read_line(1);
          option_count = ((byte)improvements_mask & TILE_FARMLAND) == 0xc;
          if (option_count) {
            dialog2.add_option((const char *)&textbuf.buf,0xc,0);
          }
          text_read_line(1);
          if (((byte)improvements_mask & TILE_FARMLAND) == 4) {
            dialog2.add_option((const char *)&textbuf.buf,4,0);
            option_count++;
          }
          text_read_line(1);
          if (((byte)improvements_mask & TILE_FARMLAND) == 8) {
            dialog2.add_option((const char *)&textbuf.buf,8,0);
            option_count++;
          }
          text_read_line(1);
          if (((byte)improvements_mask & TILE_AIRBASE) == 0x42) {
            dialog2.add_option((const char *)&textbuf.buf,0x42,0);
            option_count++;
          }
          text_read_line(1);
          if (((byte)improvements_mask & TILE_AIRBASE) == 0x40) {
            dialog2.add_option((const char *)&textbuf.buf,0x40,0);
            option_count++;
          }
          text_read_line(1);
          if ((improvements_mask & TILE_RAILROAD) == 0) {
            text_read_line(1);
            if ((improvements_mask & TILE_ROAD) != 0) {
              dialog2.add_option((const char *)&textbuf.buf,0x10,0);
              option_count++;
            }
          }
          else {
            dialog2.add_option((const char *)&textbuf.buf,0x20,0);
            option_count++;
          }
          if ((1 < option_count) &&
             (choice = dialog2.show_modal(0), choice < 0)) {
            return;
          }
        }
        city = city_find_nearest(x,y,-1,0xffffffff,-1);
        if (((0 < city) && (civ2 = (int)cities[city].owner, -1 < civ2)) &&
           (civ2 != civ)) {
          (*(i8 *)&unit_sound_editor_window_block.unit_move_flags) = 1;
          city = diplo_confirm_attack(civ,civ2,0xe);
          if (city != 0) {
            return;
          }
          if ((*(byte *)(civ * 0x594 + (iptr)((u8 *)&civs + 33) + civ2 * 4) & 0x20) == 0) {
            diplo_declare_war(civ,civ2,-1);
          }
        }
        unit_pillage(unit,choice);
      }
    }
    else {
      popup_game((const char *)&str_cantdo_4);
    }
  }
  return;
}

/* 0058d60a auto */
void __cdecl unit_cmd_paradrop(void)

{
  iptr unit;
  uint tile_flags;
  
  unit = (iptr)game.active_unit;
  if ((units[unit].moves_used == '\0') && ((units[unit].flags & UNITF_PARADROPPED) == 0)) {
    tile_flags = tile_improvements((int)units[unit].x,(int)units[unit].y);
    if ((tile_flags & TILE_CITY) == 0) {
      dlg_game_txt_with_unit((const char *)&str_paradroprules2,unit);
    }
    else {
      mapviews_set_paradrop_cursor();
    }
  }
  else {
    dlg_game_txt_with_unit((const char *)&str_paradroprules1,unit);
  }
  return;
}

/* 0058d6af auto */
void __cdecl unit_cmd_goto_city(void)

{
  u8 unit_role;
  u8 unit_range;
  u8 turns_aloft;
  iptr unit;
  iptr civ;
  int x;
  int y;
  int landmass;
  iptr city_landmass;
  iptr reachable;
  int moves_left;
  int distance;
  iptr x2;
  int city2;
  uint move_mult;
  iptr city;
  int option_count2;
  int improved_tiles;
  Dialog dialog2;
  int on_ocean;
  int y2;
  uint show_all;
  iptr tile_index;
  iptr city3;
  i32 eh_next;
  
  dialog2.Dialog::construct(0x2000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  unit = (iptr)game.active_unit;
  civ = (iptr)units[unit].owner;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  landmass = tile_landmass(x,y);
  on_ocean = tile_is_ocean(x,y);
  unit_role = unit_types[units[unit].type].role;
  show_all = 0;
  do {
    text_set_label_arg(0,unit_types[units[unit].type].name);
    dialog2.load_game_section((const char *)&str_goto,0x800001);
    option_count2 = 0;
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && ((cities[city].owner == civ || (show_all != 0)))) {
        city_landmass = tile_landmass((int)cities[city].x,(int)cities[city].y);
        if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
          if (on_ocean == 0) {
            city3 = city_at(x,y);
            if (!(city < 0)) {
              reachable = cities_share_ocean(city3,city);
              if (reachable == 0) continue;
            }
          }
          else {
            reachable = city_adjacent_to_body(city,landmass);
            if (reachable == 0) continue;
          }
        }
        else if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
          if (city_landmass != landmass) continue;
        }
        else if ((unit_types[units[unit].type].domain == DOMAIN_AIR) &&
                (unit_types[units[unit].type].range != '\0')) {
          unit_range = unit_types[units[unit].type].range;
          turns_aloft = units[unit].counter;
          city2 = unit_max_moves(unit);
          moves_left = unit_moves_left(unit);
          move_mult = (uint)cosmic.road_move_mult;
          distance = map_distance(x,y,(int)cities[city].x,(int)cities[city].y)
          ;
          if ((moves_left + ((int)(char)unit_range - ((char)turns_aloft + 1)) * city2) / (int)move_mult < distance)
          continue;
        }
        if (((cities[city].owner == civ) ||
            (*(char *)&cities[city].revealed_size[civ] != '\0')) || (game.reveal_map != '\0'))
        {
          text_clear();
          text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
          if (unit_role == ROLE_SETTLE) {
            improved_tiles = 0;
            for (tile_index = 0; tile_index < 0x14; tile_index++) {
              x2 = map_wrap_x((int)cities[city].x + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile_index]);
              y2 = (int)cities[city].y + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile_index];
              city2 = map_in_bounds(x2,y2);
              if ((city2 != 0) &&
                 (move_mult = tile_improvements(x2,y2), (move_mult & 0x80) != 0)) {
                improved_tiles++;
              }
            }
            if (improved_tiles != 0) {
              text_append_space();
              text_append_lparen();
              text_append_int(improved_tiles);
              text_append_space();
              text_append_label(0x2f);
              text_append_rparen();
            }
          }
          if ((cities[city].owner == civ) &&
             (city2 = city_has_improvement(city,IMP_AIRPORT), city2 != 0)) {
            text_append_space();
            text_append_lparen();
            text_append_string_id(improvements[IMP_AIRPORT].name);
            text_append_rparen();
          }
          dialog2.add_option((const char *)&textbuf.buf,city,0);
          option_count2++;
        }
      }
    }
    if (option_count2 != 0) {
      city2 = dialog2.show_modal(0);
      if (city2 < 0) {
        return;
      }
      if (dialog2.result_extra == 0) {
        units[unit].orders = ORDER_GOTO;
        units[unit].goto_x = cities[city2].x;
        units[unit].goto_y = cities[city2].y;
        if (bugfix_state()) {
          units[unit].aux_timer = '\0';  /* B9: the long move count starts again (docs/BUGS.md) */
        }
        return;
      }
      show_all = (uint)(show_all == 0);
      continue;
    }
    if (show_all != 0) {
      return;
    }
    show_all = 1;
  } while( true );
}

/* 0058ddce auto */
void __cdecl unit_cmd_unload(void)

{
  iptr unit2;
  int selectable;
  iptr new_active;
  iptr unit;
  bool woken = false;
  
  new_active = -1;
  if (-1 < game.active_unit) {
    unit2 = (iptr)game.active_unit;
    if (unit_types[units[unit2].type].domain == DOMAIN_SEA) {
      units[unit2].flags = units[unit2].flags | UNITF_WAITING;
    }
    for (unit = unit_stack_first((int)game.active_unit); -1 < unit;
        unit = unit_next_in_stack(unit)) {
      if ((unit_types[units[unit].type].domain == DOMAIN_LAND) && (units[unit].orders == ORDER_SLEEP)) {
        units[unit].orders = ORDER_NONE;
        woken = true;
        selectable = unit_is_selectable(unit);
        if (selectable != 0) {
          new_active = unit;
        }
      }
    }
    if (-1 < new_active) {
      game.active_unit = (i16)new_active;
      moving_units_mode = 0;
      ui_focus_active_unit(0);
    }
    else if (extra_rule(X_STACK_SENTRY) && !woken) {
      extras_stack_sentry();  /* X42: none to unload: the stack's land units sentry */
    }
  }
  return;
}

/* 0058df14 auto */
void __cdecl unit_cmd_automate(void)

{
  iptr unit;
  
  unit = (iptr)game.active_unit;
  if (unit_types[units[unit].type].role == ROLE_SETTLE) {
    units[unit].flags = units[unit].flags | UNITF_AUTOMATED;
  }
  return;
}

/* 0058df7b auto */
void __cdecl unit_cmd_airlift(void)

{
  iptr unit2;
  int civ;
  iptr city2;
  int threat;
  int max_moves;
  int distance;
  uint move_mult;
  iptr unit3;
  iptr unit;
  int threat_count;
  int option_count2;
  Dialog dialog2;
  iptr active;
  int civ2;
  iptr city;
  i32 eh_next;
  
  dialog2.Dialog::construct(0x2000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  unit2 = (iptr)game.active_unit;
  if (unit2 >= 0) {
    civ = (int)units[unit2].owner;
    if (unit_types[units[unit2].type].domain == DOMAIN_SEA) {
      dlg_game_txt_with_unit((const char *)&str_liftship,unit2);
    }
    else if (unit_types[units[unit2].type].domain == DOMAIN_AIR) {
      dlg_game_txt_with_unit((const char *)&str_liftplane,unit2);
    }
    else {
      city2 = city_at((int)units[unit2].x,(int)units[unit2].y);
      if (city2 < 0) {
        popup_game_with_sprite((const char *)&str_noairport,(iptr)&popup_info_icon_sprite,0);
      }
      else {
        threat = city_has_improvement(city2,IMP_AIRPORT);
        if (threat == 0) {
          popup_game_with_city((const char *)&str_noairport_2,city2);
        }
        else if ((cities[city2].flags & CITYF_AIRLIFTED) == 0) {
          dialog2.load_game_section((const char *)&str_airliftselect,1);
          option_count2 = 0;
          for (city = 0; city < game.num_cities; city++) {
            if ((((cities[city].id != 0) && (cities[city].owner == civ)) &&
                (city != city2)) && (threat = city_has_improvement(city,IMP_AIRPORT), threat != 0)) {
              option_count2++;
              dialog2.add_option((const char *)&cities[city].name,city,0);
            }
          }
          if (option_count2 == 0) {
            popup_game_with_improvement((const char *)&str_noairport2,IMP_AIRPORT,8);
          }
          else {
            city = dialog2.show_modal(0);
            if (city < 0) {
              /* cancelled */
            }
            else if ((cities[city].flags & CITYF_AIRLIFTED) == 0) {
              threat_count = 0;
              active = unit2;
              for (unit = 0; unit3 = active, unit < game.num_units;
                  unit = unit + 1) {
                threat = threat_count;
                if (((units[unit].id != 0) && (units[unit].owner != civ)) &&
                   ((unit_types[units[unit].type].role == ROLE_AIR_SUPERIORITY &&
                    ((*(byte *)(civ * 0x594 + (iptr)((u8 *)&civs + 33) + units[unit].owner * 4) & 0x20) != 0)
                    ))) {
                  max_moves = unit_max_moves(unit);
                  move_mult = (uint)cosmic.road_move_mult;
                  distance = map_distance((int)cities[city2].x,(int)cities[city2].y,
                                             (int)units[unit].x,(int)units[unit].y);
                  if (!(max_moves / (int)move_mult < distance) ||
                     (max_moves = unit_max_moves(unit), move_mult = (uint)cosmic.road_move_mult,
                      distance = map_distance((int)cities[city].x,(int)cities[city].y,
                                              (int)units[unit].x,(int)units[unit].y),
                      !(max_moves / (int)move_mult < distance))) {
                    civ2 = (int)units[unit].owner;
                    threat = threat_count + 1;
                    if ((units[unit].flags & UNITF_VETERAN) != 0) {
                      threat = threat_count + 2;
                    }
                  }
                }
                threat_count = threat;
              }
              if ((threat_count == 0) ||
                 (civ = dlg_game_txt_with_unit_flags((const char *)&str_enemyfighters,0x1b,(ui_metrics.large_screen == 0) - 1 & 8), civ != 0)
                 ) {
                unit_airlift_perform(unit3,city2,city,threat_count,civ2);
              }
            }
            else {
              text_set_string_arg(0,(const char *)&cities[city].name);
              popup_game_with_city((const char *)&str_alreadyairlift_2,city2);
            }
          }
        }
        else {
          text_set_string_arg(0,(const char *)&cities[city2].name);
          popup_game_with_city((const char *)&str_alreadyairlift,city2);
        }
      }
    }
  }
  return;
}

/* 0058f040: written by hand in src/game/retvals.cpp */

/* 0058fedb auto */
i32 __cdecl unit_caravan_enter_city(iptr unit,uptr city)

{
  byte civ2;
  int civ;
  iptr label;
  char city2;
  iptr option_id;
  i32 result;
  int choice;
  iptr improvement;
  int shields_left;
  Dialog dialog2;
  iptr k;
  int in_demand;
  i32 eh_next;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (autoplay.active == 0) {
    civ2 = units[unit].owner;
    civ = (int)(char)civ2;
    choice = 1;
    if (((int)units[unit].home_city == city) &&
       ((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0)) {
      choice = 0;
    }
    if (cities[city].owner == civ) {
      if (((1 << (civ2 & 0x1f) & (uint)game.human_players) == 0) &&
         ((cities[city].flags & CITYF_AI_FERRY_PORT) != 0)) {
        choice = 0;
      }
      if ((units[unit].orders == ORDER_GOTO) &&
         ((cities[city].x != units[unit].goto_x ||
          (cities[city].y != units[unit].goto_y)))) {
        choice = 0;
      }
    }
    if (((((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0) && (human_civ[0] == civ)) &&
        (cities[city].owner == civ)) && (choice != 0)) {
      if ((char)units[unit].counter < '\0') {
        text_set_label_arg(0,label_table.string_ids[0x40]);
      }
      else {
        text_set_label_arg(0,commodity_names[(char)units[unit].counter]);
      }
      dialog2.load_game_text((const char *)&str_caravanmenu);
      dialog2.add_sprite(
                 (iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),0,0);
      if (-1 < (char)units[unit].counter) {
        text_clear();
        text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
        text_append_space();
        in_demand = 0;
        for (k = 0; k < 3; k++) {
          if (cities[city].demands[k] == units[unit].counter) {
            in_demand = 1;
          }
        }
        if (in_demand == 0) {
          text_append_label(0x134);
        }
        else {
          text_append_label(0x133);
        }
        text_append_space();
        text_append_string_id(commodity_names[(char)units[unit].counter]);
        text_append_period();
        dialog2.add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
      }
      result = 0;
      option_id = 0;
      label = (iptr)label_string(label_table.string_ids[0xa1]);
      dialog2.add_option((const char *)label,option_id,result);
      city2 = (int)units[unit].home_city != city;
      if ((bool)city2) {
        result = 0;
        option_id = 1;
        label = (iptr)label_string(label_table.string_ids[0xa2]);
        dialog2.add_option((const char *)label,option_id,result);
      }
      if (cities[city].building < -0x26) {
        result = 0;
        option_id = 2;
        label = (iptr)label_string(label_table.string_ids[0xa3]);
        dialog2.add_option((const char *)label,option_id,result);
        city2++;
      }
      choice = 0;
      if (city2 != '\0') {
        choice = dialog2.show_modal(0);
      }
    }
    if (choice == 1) {
      unit_caravan_establish_trade(unit,city);
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
      }
      result = 1;
    }
    else if (choice == 2) {
      if (units[unit].type == UNIT_FREIGHT) {
        snd_play_effect(SND_DIESEL,1,0,0);
      }
      else {
        snd_play_effect(SND_CHEERS_44,1,0,0);
      }
      cities[city].shield_box =
           (short)(char)unit_types[units[unit].type].cost * (ushort)cosmic.shield_box_rows +
           cities[city].shield_box;
      text_set_string_arg(0,(const char *)&cities[city].name);
      text_set_number_arg(0,(int)(char)unit_types[units[unit].type].cost *
                           (uint)cosmic.shield_box_rows);
      if (cities[city].building < 1) {
        improvement = ~(int)cities[city].building + 1;
      }
      else {
        improvement = (iptr)cities[city].building;
      }
      shields_left = (uint)improvements[improvement].cost * (uint)cosmic.shield_box_rows -
                  (int)cities[city].shield_box;
      if (shields_left < 0) {
        shields_left = 0;
      }
      text_set_number_arg(1,shields_left);
      popup_game((const char *)&str_addtowonder);
      if (units[unit].home_city != -1) {
        cities[(byte)units[unit].home_city].flags =
             cities[(byte)units[unit].home_city].flags | CITYF_TRADE_DIRTY;
      }
      unit_destroy_redraw(unit,1);
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
      }
      result = 1;
    }
    else {
      result = 0;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* Part of unit_move_dir (0059062c): a caravan or diplomat entering a human player's
   city is first shown stepping in and back. */
static void unit_move_dir_show_entry(iptr unit,int x3,int y3,iptr x2,int y2,uptr civ,uptr dir,
                                                uint ticks)
{
  if (2 < game.multiplayer) {
    net_send_message(NM_POSSIBLY_CENTER,0xff,x3,y3,civ,0,0,0,0,0);
  }
  mapviews_ensure_visible(x3,y3,civ);
  ui_delay_ticks(ticks);
  unit_lift_off_map(unit,1);
  map_animate_unit_move(unit,x3,y3,dir,0xffffffff,0xffffffff);
  unit_relocate(unit,x3,y3,1);
  if (2 < game.multiplayer) {
    net_send_message(NM_DRAW_AREA,0xff,x2,y2,0,0,0,0,0,0);
  }
  map_redraw_tile_all_views(x2,y2);
}

/* Part of unit_move_dir (0059062c): the expelled unit goes to a city of its owner
   (a diplomat to the biggest, the courthouse counting 50). */
static void unit_move_dir_expel_to_city(iptr unit2,uptr civ2,uptr city,int x3,int y3)
{
  iptr city2;
  iptr city4;
  int best_score;
  int score;
  int city3;
  city4 = -1;
  best_score = -1;
  if (unit_types[units[unit2].type].role != ROLE_DIPLOMACY) {
    best_score = 9999;
  }
  for (city2 = 0; city2 < game.num_cities; city2++) {
    if ((cities[city2].id != 0) && ((int)cities[city2].owner == civ2)) {
      if (unit_types[units[unit2].type].role == ROLE_DIPLOMACY) {
        score = (int)cities[city2].size;
        city3 = city_has_improvement(city2,IMP_PALACE);
        if (city3 != 0) {
          score += 0x32;
        }
        if (best_score < score) {
          best_score = score;
          city4 = city2;
        }
      }
      else {
        score = map_distance_diag(x3,y3,(int)cities[city].x,
                                      (int)cities[city].y);
        if (score < best_score) {
          city4 = city2;
          best_score = score;
        }
      }
    }
  }
  if (-1 < city4) {
    city2 = city4;
    unit_relocate(unit2,(int)cities[city4].x,(int)cities[city4].y,1);
    units[unit2].visibility = '\0';
    text_set_string_arg(3,(const char *)&cities[city2].name);
  }
}

/* Part of unit_move_dir (0059062c): the messages after an expulsion, and the
   expelled civ's attitude. */
static void unit_move_dir_expel_notify(iptr unit2,uptr civ,uptr civ2,byte civ3)
{
  uint *treaty;
  int city3;
  uint rnd;
  uint sign;
  if ((autoplay.active == 0) &&
     ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
    if (game.multiplayer < 3) {
      snd_play_effect(SND_FEEDBK03,0,0,0);
      dlg_game_txt_with_unit((const char *)&str_upmine_3,unit2);
    }
    else if ((*(i8 *)&human_civ) == civ2) {
      snd_play_effect(SND_FEEDBK03,0,0,0);
      dlg_game_txt_with_unit((const char *)&str_upmine_2,unit2);
      if ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) {
        net_send_text_args(0x60,net.net_slots[net.net_civ_slots[civ]].id,4,0,unit2
                           ,0);
      }
    }
    else {
      net_send_text_args(0x5f,net.net_slots[net.net_civ_slots[civ2]].id,4,0,unit2
                         ,0);
      if ((*(i8 *)&human_civ) == civ) {
        snd_play_effect(SND_FEEDBK03,0,0,0);
        dlg_game_txt_with_unit((const char *)&str_upyours_2,unit2);
      }
    }
  }
  else if ((autoplay.active == 0) &&
          (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0 &&
           ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0)))) {
    if (game.multiplayer < 3) {
      snd_play_effect(SND_FEEDBK03,0,0,0);
      dlg_game_txt_with_unit((const char *)&str_upyourstoo_4,unit2);
    }
    else if ((*(i8 *)&human_civ) == civ) {
      snd_play_effect(SND_FEEDBK03,0,0,0);
      dlg_game_txt_with_unit((const char *)&str_upyourstoo_2,unit2);
      if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
        net_send_text_args(0x61,net.net_slots[net.net_civ_slots[civ2]].id,4,0,
                           unit2,0);
      }
    }
    else {
      net_send_text_args(0x61,net.net_slots[net.net_civ_slots[civ]].id,4,0,unit2,0
                        );
      if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
         ((*(i8 *)&human_civ) == civ2)) {
        snd_play_effect(SND_FEEDBK03,0,0,0);
        dlg_game_txt_with_unit((const char *)&str_upyourstoo_3,unit2);
      }
    }
  }
  else if ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) {
    if (game.multiplayer < 3) {
      if (autoplay.active == 0) {
        snd_play_effect(SND_FEEDBK03,0,0,0);
        dlg_game_txt_with_unit((const char *)&str_upyours_5,unit2);
      }
    }
    else if ((*(i8 *)&human_civ) == civ) {
      if (autoplay.active == 0) {
        snd_play_effect(SND_FEEDBK03,0,0,0);
        dlg_game_txt_with_unit((const char *)&str_upyours_3,unit2);
        if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
          net_send_text_args(0x60,net.net_slots[net.net_civ_slots[civ2]].id,4,0,
                             unit2,0);
        }
      }
    }
    else if (autoplay.active == 0) {
      net_send_text_args(0x60,net.net_slots[net.net_civ_slots[civ]].id,4,0,unit2,0
                        );
      if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
         ((*(i8 *)&human_civ) == civ2)) {
        snd_play_effect(SND_FEEDBK03,0,0,0);
        dlg_game_txt_with_unit((const char *)&str_upyours_4,unit2);
      }
    }
    city3 = ai_spaceship_race_behind(civ2);
    if (city3 == 0) {
      rnd = crt_rand_at("unit_move_dir");
      sign = (int)rnd >> 0x1f;
      civ_adjust_attitude(civ2,civ,
                         ((rnd ^ sign) - sign & 3 ^ sign) - sign);
    }
    else if ((*(byte *)&civs[civ2].treaties[civ] & TREATY_HOSTILE_INTENT) == 0) {
      treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
      *treaty = *treaty | 0x20;
    }
    else {
      treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
      *treaty = *treaty & 0xffffffd9;
      treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
      *treaty = *treaty | 0x40;
    }
  }
}

/* Part of unit_move_dir (0059062c): the move points a step costs (rivers, roads,
   railroads, terrain). */
static uint unit_move_dir_step_cost(iptr unit,int x3,int y3,iptr x2,int y2,uptr terrain2,
                                               u8 unit_domain)
{
  byte *src_tile;
  byte *dst_tile;
  int abs_dy;
  int city3;
  uint step_cost;
  if (unit_domain == '\0') {
    src_tile = (byte *)tile_at(x3,y3);
    dst_tile = (byte *)tile_at(x2,y2);
    if (((src_tile[1] & 0x22) == 0) || ((dst_tile[1] & 0x22) == 0)) {
      if ((unit_types[units[unit].type].flags & UTYPE_ALPINE) == 0) {
        if (((src_tile[1] & 0x12) == 0) || ((dst_tile[1] & 0x12) == 0)) {
          if ((((*dst_tile & *src_tile & 0x80) != 0) &&
              (city3 = map_dx_wrapped(x3,x2), city3 == 1)) &&
             (abs_dy = (y3 == y2 || y3 - y2 < 0) ? ~(y3 - y2) + 1 : y3 - y2, abs_dy == 1)) {
            step_cost = 1;
          }
          else {
            step_cost = (int)(char)terrain_types[terrain2].move_cost * (uint)cosmic.road_move_mult;
          }
        }
        else {
          step_cost = 1;
        }
      }
      else {
        step_cost = 1;
      }
    }
    else {
      step_cost = 0;
    }
  }
  else {
    step_cost = (uint)cosmic.road_move_mult;
  }
  return step_cost;
}

/* Part of unit_move_dir (0059062c): an AI unit holds back (a diplomat or attacker
   near its goal, not stepping back, not into a full own stack). Returns 1 to
   stop the move. */
static int unit_move_dir_ai_holds_back(iptr unit,uptr dir,uptr civ,uptr civ2,iptr unit2,uptr city,
                                                  iptr x2,int y2,int x3,int y3,u8 unit_domain,
                                                  uint step_cost,byte unit_type,uint prev_moves_used)
{
  u8 unit_role;
  int city3;
  int value;
  if (((((unit_types[units[unit].type].role == ROLE_DIPLOMACY) ||
        ((unit_types[units[unit].type].role == ROLE_ATTACK &&
         ((char)unit_types[units[unit].type].defense * 2 <
          (int)(char)unit_types[units[unit].type].attack)))) && (units[unit].orders == ORDER_GOTO)
       ) && (((unit_type != UNIT_SETTLERS &&
              (unit_role = unit_types[units[unit].type].role, city3 = unit_moves_left(unit),
              city3 <= (int)(uint)(unit_role != ROLE_DIPLOMACY))) &&
             (city3 = unit_max_moves(unit), (int)step_cost < city3)))) &&
     ((city3 = map_distance(x2,y2,(int)units[unit].goto_x,
                                   (int)units[unit].goto_y), city3 == 1 ||
      (((city3 = tile_adjacent_nonallied_civ(x2,y2,civ), city3 != 0 ||
        ((game.human_players & units[unit].visibility) != 0)) &&
       (city3 = tile_adjacent_nonallied_civ(x3,y3,civ), city3 == 0))))))
    return 1;
  if (((((int)(char)units[unit].last_dir ^ 4U) == dir) && (prev_moves_used != 0)) &&
     ((unit_types[units[unit].type].domain != DOMAIN_AIR &&
      (city3 = unit_stack_sum(unit,2), city3 < 3)))) {
    unit_exhaust_moves(unit);
    return 1;
  }
  if ((((-1 < unit2) && (civ2 == civ)) && ((int)city < 0)) &&
     ((unit_domain == '\0' && (unit_types[units[unit].type].role != ROLE_DIPLOMACY)))) {
    city3 = unit_moves_left(unit);
    value = unit_stack_sum(unit2,2);
    if ((int)((-(uint)(city3 == 0) & 0xfffffffe) + 4) <= value) {
      unit_exhaust_moves(unit);
      return 1;
    }
  }
  return 0;
}

/* Part of unit_move_dir (0059062c): whether this player sees the move (view:
   civ_sees) or sees it animated (civ_animates): view[human_civ].  During a
   hot-seat AI turn human_civ is -1 (B11): the original then reads the local
   before the array, which unit_move_dir keeps there (views).  B22: with no
   player here (-1, or 0 after Reveal Map's Barbarians) nobody here sees the
   move; the original pauses, animates, and counts and announces a landing
   by what those locals hold (docs/BUGS.md). */
static int unit_move_dir_local_view(const int *view)
{
  if (bugfix_state() && ((*(i8 *)&human_civ) <= 0)) {
    return 0;
  }
  return view[(*(i8 *)&human_civ)];
}

/* Part of unit_move_dir (0059062c): which civs see the move (civ_sees) and which of
   them see it animated (civ_animates). */
static void unit_move_dir_viewers(iptr unit,uptr civ,iptr x2,int y2,uptr city,uint step_cost,
                                             u8 unit_domain,int *civ_sees,int *civ_animates)
{
  SHORT shift_state;
  uptr adj_owner;
  int city3;
  int y5;
  iptr adj_dir;
  uptr other_civ;
  iptr x5;
  uint shift_down;
  for (other_civ = 1; (int)other_civ < 8; other_civ++) {
    civ_sees[other_civ] = 0;
    civ_animates[other_civ] = civ_sees[other_civ];
    if ((((1 << ((byte)other_civ & 0x1f) & (uint)game.civs_alive) != 0) &&
        ((2 < game.multiplayer || ((*(i8 *)&human_civ) == other_civ)))) &&
       (((autoplay.active != 0 && ((map_views_block.saved_human_players & 1 << ((byte)other_civ & 0x1f)) != 0)) ||
        ((1 << ((byte)other_civ & 0x1f) & (uint)game.human_players) != 0)))) {
      if (((((1 << ((byte)other_civ & 0x1f) & (uint)units[unit].visibility) == 0) &&
           ((int)units[unit].owner != (other_civ & 0xff))) && (civ != other_civ)) &&
         (game.reveal_map == '\0')) {
        for (adj_dir = 0; adj_dir < 8; adj_dir++) {
          x5 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[adj_dir] + x2);
          y5 = (((i8 *)&dir_dy_table_block.dir_dy_table))[adj_dir] + y2;
          city3 = map_in_bounds(x5,y5);
          if ((city3 != 0) &&
             (adj_owner = tile_city_or_unit_owner(x5,y5), adj_owner == other_civ)) {
            civ_sees[other_civ] = 1;
            break;
          }
        }
      }
      else {
        civ_sees[other_civ] = 1;
      }
      if (((*(i8 *)&human_civ) == other_civ) && (civ_sees[other_civ] != 0)) {
        if ((((*(i8 *)&human_civ) == civ) || (((*((char *)&game.game_options + 1)) & (OPT_SHOW_ENEMY_MOVES >> 8)) != 0)) ||
           (-1 < (int)city)) {
          civ_animates[(*(i8 *)&human_civ)] = 1;
          shift_state = GetAsyncKeyState(0x10);
          shift_down = (uint)(byte)((ushort)shift_state >> 8);
          if (shift_down != 0) {
            civ_animates[(*(i8 *)&human_civ)] = 0;
          }
          if ((((step_cost == 0) || (shift_down != 0)) ||
              (((*(i8 *)&human_civ) == civ && (unit_domain == 1)))) &&
             (((*(i8 *)&human_civ) != civ || (units[unit].orders == ORDER_GOTO)))) {
            civ_animates[(*(i8 *)&human_civ)] = 0;
            unit_move.unanimated_state = 1;
            if ((*(i8 *)&human_civ) == civ) {
              unit_move.unanimated_state = 2;
              map_cursor.x[0] = (i16)x2;
              map_cursor.y[0] = (i16)y2;
            }
          }
        }
      }
      else if ((((*(i8 *)&human_civ) != other_civ) && (civ_sees[other_civ] != 0)) &&
              (((civ_animates[other_civ] = 1, step_cost == 0 ||
                ((civ == other_civ && (unit_types[units[unit].type].domain == DOMAIN_AIR)))) &&
               ((civ != other_civ || (units[unit].orders == ORDER_GOTO)))))) {
        civ_animates[other_civ] = 0;
      }
    }
  }
}

/* Part of unit_move_dir (0059062c): an AI landing on a continent near the human
   player's city: counted, and the player warned. */
static void unit_move_dir_landing_alert(iptr unit,uptr civ,uptr civ2,iptr x2,int y2,int x3,int y3,
                                                   uint src_is_ocean,uint dst_is_ocean,int *civ_sees)
{
  short *continent_count;
  iptr unit3;
  int city3;
  if (((((((unit_move_dir_local_view(civ_sees) != 0) &&
          (mapviews_ensure_visible(x3,y3,civ), src_is_ocean != 0)) && (dst_is_ocean == 0)) &&
        (((*(i8 *)&human_civ) != civ && (civ != 0)))) &&
       (unit_types[units[unit].type].domain == DOMAIN_LAND)) &&
      ((city3 = tile_landmass(x2,y2),
       *(short *)&civs[civ].continent_military[city3] == 0 &&
       (continent_count = (short *)(city3 * 2 + (iptr)((u8 *)&civs + 402) + civ * 0x594), *continent_count = *continent_count + 1,
       (*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0)))) &&
     ((city3 = city_find_nearest(x3,y3,-1,0xffffffff,-1), 0 < city3 &&
      ((cities[city3].owner == (*(i8 *)&human_civ) && (autoplay.active == 0)))))) {
    unit3 = (iptr)(civ_name_plural(civ));
    text_set_string_arg(0,(const char *)unit3);
    text_set_string_arg(1,(const char *)&cities[city3].name);
    dlg_game_txt_with_unit((const char *)&str_landing,unit);
  }
}

/* Part of unit_move_dir (0059062c): the move done here (single player, the host,
   an AI): animated, the stack moved. Returns whether the tile has a hut. */
static int unit_move_dir_relocate(iptr unit,uptr civ2,iptr x2,int y2,int x3,int y3,uptr dir,
                                             uint dst_is_ocean,int *civ_sees,int *civ_animates)
{
  uptr other_civ;
  int has_hut;
  unit_lift_off_map(unit,0);
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,1);
    for (other_civ = 1; (int)other_civ < 8; other_civ++) {
      if (((*(i8 *)&human_civ) == other_civ) || (civ_animates[other_civ] == 0)) {
        if (((*(i8 *)&human_civ) != other_civ) && (civ_sees[other_civ] != 0)) {
          net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[other_civ]].id
                             ,x3,y3,0,0,0,0,0,0);
        }
      }
      else {
        net_send_message(NM_SLIDE_PIECE,net.net_slots[net.net_civ_slots[other_civ]].id,
                           unit,x3,y3,dir,-1,1,0,0);
      }
    }
    XD_FlushSendBuffer(5000);
  }
  if (unit_move_dir_local_view(civ_animates) == 0) {
    if (unit_move_dir_local_view(civ_sees) != 0) {
      map_redraw_tile_all_views(x3,y3);
    }
  }
  else {
    snd_play_effect(SND_MOVPIECE,1,0,0);
    map_animate_unit_move(unit,x3,y3,dir,0xffffffff,0xffffffff);
  }
  unit_stack_clear_visibility(unit);
  if (dst_is_ocean == 0) {
    unit_stack_set_seen_by(unit,civ2);
  }
  has_hut = tile_has_hut(x2,y2);
  unit_stack_relocate_all(unit,x2,y2,0);
  net_send_state_diff(0xff,2,0,0,1);
  return has_hut;
}

/* Part of unit_move_dir (0059062c): a multiplayer client's move, done by the host:
   asked for, animated here, then waited for. Returns whether the tile has a
   hut. */
static int unit_move_dir_relocate_via_host(iptr unit,uptr civ,uptr civ2,iptr x2,int y2,int x3,
                                                      int y3,uptr dir,uint dst_is_ocean,int lock_id,
                                                      int *civ_sees,int *civ_animates,bool &flag,
                                                      bool &map_locked)
{
  iptr unit3;
  i32 arg;
  iptr unit6;
  int city3;
  int value;
  uptr other_civ;
  int has_hut;
  has_hut = tile_has_hut(x2,y2);
  net.nm_stacker_ack_count = -2;
  net_send_message(NM_REALTIME_STACKER,0,civ,civ2,unit,dst_is_ocean,x2,y2,0,0);
  flag = true;
  XD_FlushSendBuffer(5000);
  if (unit_move_dir_local_view(civ_animates) == 0) {
    if (unit_move_dir_local_view(civ_sees) != 0) {
      map_redraw_tile_all_views(x3,y3);
    }
  }
  else {
    snd_play_effect(SND_MOVPIECE,1,0,0);
    city3 = (iptr)tile_at(x3,y3);
    arg = 2;
    unit6 = tile_first_unit(x3,y3);
    value = unit_stack_sum(unit6,arg);
    if (value == 1) {
      *(byte *)(city3 + 1) = *(byte *)(city3 + 1) & 0xfe;
    }
    else if (1 < value) {
      combat_anim.origin_x = (int)units[unit].x;
      combat_anim.origin_y = (int)units[unit].y;
      value = unit_stack_sum(unit,5);
      if (value == 0) {
        if (units[unit].next_in_stack < 0) {
          if (units[unit].prev_in_stack < 0) {
            *(byte *)(city3 + 1) = *(byte *)(city3 + 1) & 0xfe;
          }
          else {
            combat_anim.origin_stack_unit = (iptr)units[unit].prev_in_stack;
          }
        }
        else {
          combat_anim.origin_stack_unit = (iptr)units[unit].next_in_stack;
        }
      }
      else {
        value = unit_stack_sum(unit,5);
        if (value < 2) {
          *(byte *)(city3 + 1) = *(byte *)(city3 + 1) & 0xfe;
        }
        else {
          combat_anim.origin_stack_unit = unit_stack_first(unit);
          while ((combat_anim.origin_stack_unit == unit) ||
                 (unit_types[units[combat_anim.origin_stack_unit].type].domain != DOMAIN_SEA)) {
            combat_anim.origin_stack_unit = (iptr)units[combat_anim.origin_stack_unit].next_in_stack;
            if (combat_anim.origin_stack_unit < 0) {
              *(byte *)(city3 + 1) = *(byte *)(city3 + 1) & 0xfe;
              break;
            }
          }
        }
      }
    }
    unit_move_anim_active = 1;
    map_animate_unit_move(unit,x3,y3,dir,0xffffffff,0xffffffff);
    unit_move_anim_active = 0;
    arg = 2;
    unit3 = tile_first_unit(x3,y3);
    value = unit_stack_sum(unit3,arg);
    if (value != 0) {
      if (net.host_flag == '\0') {
        *(byte *)(city3 + 1) = *(byte *)(city3 + 1) | 1;
      }
      else {
        tile_set_improvements(x3,y3,1,1,1);
      }
    }
  }
  for (other_civ = 1; (int)other_civ < 8; other_civ++) {
    if (((*(i8 *)&human_civ) == other_civ) || (civ_animates[other_civ] == 0)) {
      if ((*(i8 *)&human_civ) != other_civ) {
        net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[other_civ]].id,
                           x3,y3,0,0,0,0,0,0);
      }
    }
    else {
      net_send_message(NM_SLIDE_PIECE,net.net_slots[net.net_civ_slots[other_civ]].id,
                         unit,x3,y3,dir,-1,1,0,0);
    }
  }
  if (lock_id != 0) {
    city3 = timer_ticks();
    while ((value = timer_ticks(), value - city3 < 0xe10 && (net.nm_stacker_ack_count != 1))) {
      net.process_messages(1,1);
    }
    if (net.nm_stacker_ack_count != 1) {
      dbg_log((const char *)&str_move_unit_nm_realtime_stacker_connection);
      popup_game_with_sprite((const char *)&str_serverconnecttime_7,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
  }
  while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
    net.process_messages(1,0);
  }
  map_locked = false;
  return has_hut;
}

/* Part of unit_move_dir (0059062c): an aircraft lands (city, airbase, carrier) or
   uses fuel, and crashes when out. Returns 1 when it crashed. */
static int unit_move_dir_air_fuel(iptr unit,iptr x2,int y2,int attack_done,bool &flag,int &x3,
                                             int &y3)
{
  int city3;
  iptr unit5;
  byte unit_type;
  city3 = tile_city_owner(x2,y2);
  if ((city3 < 0) && (city3 = tile_airbase_owner(x2,y2), city3 < 0)) {
    flag = false;
  }
  else {
    flag = true;
  }
  if (!flag) {
    for (unit5 = unit_stack_first(unit); -1 < unit5;
        unit5 = unit_next_in_stack(unit5)) {
      if ((unit_types[units[unit5].type].flags & UTYPE_CARRY_AIR) != 0) {
        flag = true;
        break;
      }
      if (((unit_types[units[unit5].type].flags & UTYPE_SUBMARINE) != 0) &&
         ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0)) {
        flag = true;
        break;
      }
    }
  }
  if ((((flag) && (unit_types[units[unit].type].range == 1)) &&
      (city3 = unit_moves_left(unit), city3 != 0)) && (attack_done != 0)) {
    flag = false;
  }
  if (flag) {
    units[unit].counter = '\0';
    unit_exhaust_moves(unit);
    unit_move_to_stack_bottom(unit,1);
    map_redraw_tile_all_views(x2,y2);
    net_send_message(NM_DRAW_SQUARE,0xff,x2,y2,0,0,0,0,0,0);
  }
  else if (!(((unit_types[units[unit].type].range == '\0') ||
             (city3 = unit_moves_left(unit), city3 != 0)) ||
            (units[unit].counter = units[unit].counter + 1,
            (char)units[unit].counter < (char)unit_types[units[unit].type].range))) {
    if (unit_sound_editor_window_block.unit_move_by_local_human != '\0') {
      if (units[unit].type < 0x1e) {
        snd_play_effect(SND_ENGNSPUT,0,0,0);
      }
      else {
        snd_play_effect(SND_JETSPUTR,0,0,0);
      }
    }
    x3 = (int)units[unit].x;
    y3 = (int)units[unit].y;
    unit_type = units[unit].type;
    unit_delete(unit,1);
    map_redraw_area_all_views(x3,y3,0,(*(i8 *)&human_civ),1);
    net_send_message(NM_DRAW_AREA,0xff,x3,y3,0,0,0,0,0,0);
    if (unit_sound_editor_window_block.unit_move_by_local_human != '\0') {
      dlg_game_txt_with_unit_flags((const char *)&str_fuel,(uint)unit_type,(ui_metrics.large_screen == 0) - 1 & 8);
    }
    return 1;
  }
  return 0;
}

/* Part of unit_move_dir (0059062c): a trireme ending its move away from the coast
   may be lost at sea. Returns 1 when it was. */
static int unit_move_dir_trireme_loss(iptr unit,uptr civ,iptr x2,int y2,bool &flag,int &x3,
                                                 int &y3)
{
  uint trireme_odds;
  int trireme_roll;
  iptr adj_dir;
  iptr x;
  int y4;
  int city3;
  byte unit_type;
  if ((((unit_types[units[unit].type].flags & UTYPE_COAST_BOUND) != 0) && (unit_sound_editor_window_block.unit_move_by_local_human != '\0')) &&
     ((city3 = unit_moves_left(unit), city3 == 0 &&
      (city3 = civ_has_wonder(civ,WONDER_LIGHTHOUSE), city3 == 0)))) {
    trireme_odds = (uint)cosmic.trireme_loss_chance;
    city3 = civ_has_advance(civ,ADV_SEAFARING);
    if ((city3 != 0) && (trireme_odds = trireme_odds * 2, trireme_odds < 3)) {
      trireme_odds = 2;
    }
    city3 = civ_has_advance(civ,ADV_NAVIGATION);
    if (city3 != 0) {
      trireme_odds = trireme_odds << 1;
    }
    if ((int)(trireme_odds - 1) < 1) {
      trireme_roll = 0;
    }
    else {
      trireme_roll = crt_rand_at("unit_move_dir");
      trireme_roll = trireme_roll % (int)trireme_odds;
    }
    if (trireme_roll == 0) {
      flag = false;
      for (adj_dir = 0; adj_dir < 9; adj_dir++) {
        x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[adj_dir] + x2);
        y4 = (((i8 *)&dir_dy_table_block.dir_dy_table))[adj_dir] + y2;
        city3 = map_in_bounds(x,y4);
        if ((city3 != 0) && (city3 = tile_is_ocean(x,y4), city3 == 0)) {
          flag = true;
          break;
        }
      }
      if (!flag) {
        if (unit_sound_editor_window_block.unit_move_by_local_human != '\0') {
          snd_play_effect(SND_BOATSINK,1,0,0);
        }
        x3 = (int)units[unit].x;
        y3 = (int)units[unit].y;
        unit_type = units[unit].type;
        unit_delete_with_cargo(unit,1);
        map_redraw_area_all_views(x3,y3,0,(*(i8 *)&human_civ),1);
        net_send_message(NM_DRAW_AREA,0xff,x3,y3,0,0,0,0,0,0);
        if (unit_sound_editor_window_block.unit_move_by_local_human != '\0') {
          dlg_game_txt_with_unit_flags((const char *)&str_trireme,(uint)unit_type,(ui_metrics.large_screen == 0) - 1 & 8);
        }
        return 1;
      }
    }
  }
  return 0;
}

/* Part of unit_move_dir (0059062c): a human player's transport out of moves next to
   unexplored land wakes its sleeping settlers. */
static void unit_move_dir_wake_settlers(iptr unit,uptr civ,byte civ3,iptr x2,int y2,
                                                   u8 unit_domain,bool &flag)
{
  iptr adj_dir;
  iptr x;
  int y4;
  iptr adj_in_bounds;
  iptr adj_ocean;
  iptr dir2;
  iptr x4;
  int city3;
  int value;
  if ((((unit_domain == 2) && (unit_types[units[unit].type].hold != '\0')) &&
      ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) &&
     ((city3 = unit_moves_left(unit), city3 == 0 &&
      (city3 = unit_stack_count_role(unit,ROLE_ATTACK), city3 != 0)))) {
    flag = false;
    for (adj_dir = 0; adj_dir < 8; adj_dir++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[adj_dir] + x2);
      y4 = (((i8 *)&dir_dy_table_block.dir_dy_table))[adj_dir] + y2;
      adj_in_bounds = map_in_bounds(x,y4);
      if (((adj_in_bounds != 0) && (adj_ocean = tile_is_ocean(x,y4), adj_ocean == 0)) &&
         (city3 = tile_landmass(x,y4),
         *(char *)&civs[civ].continent_policy[city3] != 4)) {
        for (dir2 = 0; dir2 < 8; dir2++) {
          x4 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir2] + x);
          city3 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir2] + y4;
          value = map_in_bounds(x4,city3);
          if ((value != 0) && (city3 = tile_visible_to(x4,city3,civ), city3 == 0)) {
            flag = true;
            break;
          }
        }
        if (flag) break;
      }
    }
    if (flag) {
      for (unit = unit_stack_first(unit); -1 < unit;
          unit = unit_next_in_stack(unit)) {
        if (((unit_types[units[unit].type].role == ROLE_ATTACK) &&
            (unit_types[units[unit].type].attack == '\0')) &&
           (units[unit].moves_used == '\0')) {
          units[unit].orders = ORDER_NONE;
        }
      }
    }
  }
}

/* Part of unit_move_dir (0059062c): a land unit boarded a ship: it sleeps, and a
   ship that can still move gathers its cargo. */
static void unit_move_dir_board_ship(iptr unit3,iptr x2,int y2)
{
  iptr unit4;
  byte unit_type;
  int city3;
  unit_set_sleep(unit3);
  unit_exhaust_moves(unit3);
  for (unit4 = unit_stack_first(unit3); -1 < unit4;
      unit4 = unit_next_in_stack(unit4)) {
    if (((unit_types[units[unit4].type].domain == DOMAIN_SEA) &&
        (unit_type = units[unit4].moves_used, city3 = unit_max_moves(unit4),
        (int)(uint)unit_type <= city3)) &&
       (unit_gather_cargo(unit4,0,0), units[unit3].goto_x == unit4)) {
      units[unit4].orders = ORDER_NONE;
      game.active_unit = (i16)unit4;
      break;
    }
  }
  map_redraw_tile_all_views(x2,y2);
  net_send_state_diff(0xff,2,0,0,0);
  net_send_message(NM_DRAW_SQUARE,0xff,x2,y2,0,0,0,0,0,0);
}

/* Part of unit_move_dir (0059062c): a human player's long goto asks whether to go on. */
static void unit_move_dir_long_move(iptr unit3,uptr dir)
{
  int city3;
  if (unit_sound_editor_window_block.unit_move_by_local_human != '\0') {
    if ((units[unit3].orders == ORDER_GOTO) || ((units[unit3].flags & UNITF_AUTOMATED) != 0)) {
      units[unit3].aux_timer++;
      if (((int)(char)units[unit3].last_dir ^ 4U) == dir) {
        units[unit3].aux_timer += 15;
      }
      if ('/' < (char)units[unit3].aux_timer) {
        city3 = dlg_game_txt_with_unit((const char *)&str_longmove,unit3);
        if (city3 != 0) {
          units[unit3].orders = ORDER_NONE;
          units[unit3].flags = units[unit3].flags & ~UNITF_AUTOMATED;
        }
        units[unit3].aux_timer = '\0';
      }
    }
    else {
      units[unit3].aux_timer = '\0';
    }
  }
}

/* Part of unit_move_dir (0059062c): a land unit's special moves: blocked by a zone
   of control, a caravan or diplomat entering a city, a diplomat meeting a
   unit, repair in an allied city. Returns 1 to stop the move. */
static int unit_move_dir_land_unit_checks(iptr &unit,iptr orig_unit,uptr dir,uptr civ,uptr &civ2,
                                                     byte civ3,iptr x2,int y2,int x3,int y3,uptr city,
                                                     iptr unit2,uint src_is_ocean)
{
  int city3;
  iptr unit3;
  int heal;
  if ((((unit2 < 0) && (city3 = tile_in_enemy_zoc(x3,y3,civ), city3 != 0))
      && (city3 = tile_in_enemy_zoc(x2,y2,civ), city3 != 0)) &&
     ((src_is_ocean == 0 && ((unit_types[units[unit].type].flags & UTYPE_IGNORE_ZOC) == 0)))) {
    if ((unit_sound_editor_window_block.unit_move_by_local_human != '\0') && ((unit_sound_editor_window_block.unit_move_flags & 1) != 0)) {
      snd_play_effect(SND_NEG1,1,0,0);
      if ((unit_sound_editor_window_block.unit_move_by_local_human != '\0') && (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0)) {
        popup_game_with_sprite((const char *)&str_zoc,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      }
      map_reveal_adjacent_units(x2,y2,civ);
    }
    unit = orig_unit;
    if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
      unit_exhaust_moves(unit);
    }
    return 1;
  }
  if ((unit_types[units[unit].type].role == ROLE_TRADE) && (-1 < (int)city)) {
    if ((civ2 != civ) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)
       ) {
      unit_move_dir_show_entry(unit,x3,y3,x2,y2,civ,dir,10);
    }
    city3 = unit_caravan_enter_city(unit,city);
    unit = orig_unit;
    if (city3 != 0) return 1;
  }
  if (((unit_types[units[unit].type].role == ROLE_DIPLOMACY) && (-1 < (int)civ2)) &&
     (civ2 != civ)) {
    if (-1 < (int)city) {
      if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
        unit_move_dir_show_entry(unit,x3,y3,x2,y2,civ,dir,0x14);
      }
      spy_city_mission(unit,city);
      unit = orig_unit;
      return 1;
    }
    city3 = spy_unit_mission(unit,unit2,civ);
    unit = orig_unit;
    if (city3 != 0) return 1;
    civ2 = tile_city_or_unit_owner(x2,y2);
  }
  if (((-1 < (int)city) && (-1 < (int)civ2)) &&
     ((civ2 != civ && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0))))
  {
    heal = unit_max_hp(unit);
    heal = heal / 10;
    city3 = city_has_improvement(city,IMP_BARRACKS);
    if (city3 != 0) {
      heal = heal << 1;
    }
    heal = int_clamp(heal,0,(uint)units[unit].damage);
    units[unit].damage -= (char)heal;
    unit_exhaust_moves(unit);
    units[unit].flags = units[unit].flags & ~UNITF_MOVED;
    if (((autoplay.active == 0) && ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) &&
       ((*(i8 *)&human_civ) == civ)) {
      snd_play_effect(SND_POS1,1,0,0);
      unit3 = (iptr)(civ_name_plural(civ2));
      text_set_string_arg(1,(const char *)unit3);
      text_set_label_arg(2,unit_types[units[unit].type].name);
      popup_game_with_sprite((const char *)&str_alliedrepair,(iptr)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
                         ,0);
    }
    unit = orig_unit;
    if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
      unit_exhaust_moves(unit);
    }
    return 1;
  }
  return 0;
}

/* Part of unit_move_dir (0059062c): a diplomat alone on the tile, of a civ at peace,
   is expelled (asking a human player first). Returns 1 to stop the move.
   answer keeps the value of an earlier step when not asked (the original
   reads it as it is). */
static int unit_move_dir_expel(iptr &unit,iptr orig_unit,uptr dir,uptr civ,uptr civ2,byte civ3,
                                          iptr x2,int y2,int x3,int y3,uptr city,iptr unit2,int &answer)
{
  int city3;
  iptr city2;
  iptr unit3;
  if (((unit_types[units[unit2].type].role == ROLE_DIPLOMACY) &&
      (city3 = unit_stack_sum(unit2,2), city3 == 1)) &&
     ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) != 0)) {
    city2 = city_find_nearest(x2,y2,-1,0xffffffff,-1);
    if ((city2 < 0) || ((int)cities[city2].owner != civ)) {
      if ((autoplay.active == 0) && ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) {
        text_set_label_arg(1,unit_types[units[unit2].type].name);
        if (unit_types[units[unit2].type].role != ROLE_DIPLOMACY) {
          text_set_label_arg(1,label_table.string_ids[0x8d]);
        }
        popup_game_with_sprite((const char *)&str_noexpel,(iptr)&popup_info_icon_sprite,0);
      }
    }
    else {
      unit3 = civ_get_adjective(civ2);
      text_set_string_arg(0,(const char *)unit3);
      text_set_label_arg(1,unit_types[units[unit2].type].name);
      unit3 = (iptr)(civ_name_plural(civ));
      text_set_string_arg(2,(const char *)unit3);
      if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) &&
         ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) {
        if (autoplay.active == 0) {
          answer = dlg_game_txt_with_unit((const char *)&str_expel,unit2);
        }
        else if (bugfix_state()) {
          answer = 0;  /* B25: not asked under autoplay: expelled, as for the AI (docs/BUGS.md) */
        }
      }
      else {
        answer = 0;
      }
      unit = orig_unit;
      if (answer == 2) return 1;
      if (answer == 0) {
        if (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) ||
           ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
          if (2 < game.multiplayer) {
            net_send_message(NM_POSSIBLY_CENTER,0xff,x3,y3,civ,0,0,0,0,0);
          }
          mapviews_ensure_visible(x3,y3,civ);
          ui_delay_ticks(10);
          unit_lift_off_map(unit,1);
          map_animate_unit_move(unit,x3,y3,dir,0xffffffff,0xffffffff);
          unit_relocate(unit,x3,y3,1);
          if (2 < game.multiplayer) {
            net_send_message(NM_DRAW_AREA,0xff,x3,y3,civ,0,0,0,0,0);
          }
          map_redraw_tile_all_views(x2,y2);
        }
        unit_move_dir_expel_to_city(unit2,civ2,city,x3,y3);
        map_redraw_tile_all_views(x2,y2);
        net_send_message(NM_DRAW_SQUARE,0xff,x2,y2,0,0,0,0,0,0);
        unit_move_dir_expel_notify(unit2,civ,civ2,civ3);
        return 1;
      }
    }
  }
  return 0;
}

/* Part of unit_move_dir (0059062c): moving onto a foreign unit: the checks (amphibious
   attack, submarine, diplomat, non-combat unit, fighters, expulsion,
   fatigue), then the attack. Returns 1 to finish the move (the attack won and
   the unit moved in), 0 to stop it. */
static int unit_move_dir_attack_unit(iptr &unit,iptr orig_unit,uptr dir,uptr civ,uptr &civ2,
                                                byte civ3,iptr &x2,int &y2,int x3,int y3,uptr city,
                                                iptr unit2,u8 unit_domain,uint src_is_ocean,
                                                uint dst_is_ocean,uint &result,int &attack_done,
                                                iptr &unit3,int &answer)
{
  int city3;
  i32 arg;
  civ2 = (uptr)units[unit2].owner;
  if ((src_is_ocean != 0) && ((unit_domain == '\0' && ((unit_types[units[unit].type].flags & UTYPE_AMPHIBIOUS) == 0)))
     ) {
    unit = orig_unit;
    if (((unit_sound_editor_window_block.unit_move_flags & 1) != 0) && (unit_sound_editor_window_block.unit_move_by_local_human != '\0')) {
      popup_game_with_sprite((const char *)&str_amphib,(iptr)&popup_info_icon_sprite,0);
    }
    return 0;
  }
  if ((dst_is_ocean == 0) && (unit = orig_unit, (unit_types[units[unit].type].flags & UTYPE_SUBMARINE) != 0))
  return 0;
  if (((unit_types[units[unit2].type].flags & UTYPE_SUBMARINE) != 0) &&
     ((((city3 = unit_stack_sum(unit2,2), city3 == 1 &&
        ((1 << (civ3 & 0x1f) & (uint)units[unit2].visibility) == 0)) &&
       ((int)units[unit2].owner != (uint)civ3)) &&
      ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)))) {
    unit = orig_unit;
    if ((unit_sound_editor_window_block.unit_move_flags & 1) != 0) {
      units[unit].orders = ORDER_NONE;
      unit_set_seen_by(unit2,civ);
      map_redraw_tile_all_views(x2,y2);
      net_send_message(NM_DRAW_SQUARE,0xff,x2,y2,0,0,0,0,0,0);
    }
    return 0;
  }
  if (unit_types[units[unit].type].role == ROLE_DIPLOMACY) {
    units[unit].orders = ORDER_NONE;
    result = (uint)game.human_players & 1 << (civ3 & 0x1f);
    unit = orig_unit;
    return 0;
  }
  if (unit_types[units[unit].type].attack == '\0') {
    if ((((unit_sound_editor_window_block.unit_move_flags & 1) != 0) && (unit_sound_editor_window_block.unit_move_by_local_human != '\0')) &&
       ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0)) {
      snd_play_effect(SND_NEG1,0,0,0);
      popup_game_with_sprite((const char *)&str_noncombat,(iptr)&popup_info_icon_sprite,0);
    }
    unit = orig_unit;
    if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
      unit_exhaust_moves(unit);
    }
    return 0;
  }
  if ((((int)city < 0) && (city3 = tile_airbase_owner(x2,y2), city3 < 0)) &&
     ((city3 = unit_stack_sum(unit2,7), city3 != 0 &&
      (((unit_types[units[unit].type].flags & UTYPE_ATTACK_AIR) == 0 &&
       (city3 = unit_stack_sum(unit2,9), city3 == 0)))))) {
    if (((unit_sound_editor_window_block.unit_move_flags & 1) != 0) && (unit_sound_editor_window_block.unit_move_by_local_human != '\0')) {
      popup_game_with_sprite((const char *)&str_fighter,(iptr)&popup_info_icon_sprite,0);
    }
    unit = orig_unit;
    if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
      unit_exhaust_moves(unit);
    }
    return 0;
  }
  if (unit_move_dir_expel(unit,orig_unit,dir,civ,civ2,civ3,x2,y2,x3,y3,city,unit2,answer) != 0)
  return 0;
  city3 = unit_moves_left(unit);
  if (city3 < (int)(uint)cosmic.road_move_mult) {
    if ((unit_sound_editor_window_block.unit_move_by_local_human == '\0') || ((unit_sound_editor_window_block.unit_move_flags & 1) == 0)) {
      if (((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) &&
         (city3 = unit_moves_left(unit), unit = orig_unit,
         city3 < (cosmic.road_move_mult - 1))) return 0;
    }
    else {
      arg = unit_moves_left(unit);
      arg = text_set_number_arg(0,arg);
      text_set_number_arg_u8(1,CONCAT31((int3)((uint)arg >> 8),cosmic.road_move_mult));
      city3 = dlg_game_txt_with_unit((const char *)&str_fatigue,unit);
      unit = orig_unit;
      if (city3 != 0) return 0;
    }
  }
  game.moving_unit = (i16)unit;
  units[unit].flags = units[unit].flags & ~UNITF_AI_GARRISON;
  if ((unit_sound_editor_window_block.unit_move_flags & 2) == 0) {
    city3 = unit_resolve_attack(unit,dir,1);
    if (city3 == 0) {
      result = 1;
      unit = orig_unit;
    }
    else {
      result = 0;
      unit = orig_unit;
    }
  }
  else {
    city3 = unit_resolve_attack(unit,dir,1);
    unit = orig_unit;
    if (city3 != 0) {
      attack_done = 1;
      unit3 = (int)game.moving_unit;
      y2 = y3;
      x2 = x3;
      return 1;
    }
  }
  return 0;
}

/* Part of unit_move_dir (0059062c): a ship moving onto land: a land unit aboard that
   can still move (asking a human player) makes landfall instead. Returns 1
   to stop the move, 0 to move that unit (unit). */
static int unit_move_dir_pick_landing_unit(iptr &unit,iptr orig_unit,int x3,int y3,int &unit_count,
                                                      int &answer)
{
  int city3;
  iptr unit7;
  int has_land_unit;
  unit = orig_unit;
  if ((unit_sound_editor_window_block.unit_move_by_local_human == '\0') || (city3 = tile_city_owner(x3,y3), -1 < city3))
  return 1;
  if (game.num_units == unit_count) {
    units[unit].orders = ORDER_NONE;
  }
  unit_count = 0;
  unit7 = -1;
  has_land_unit = 0;
  for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit))
  {
    if ((unit_types[units[unit].type].domain == DOMAIN_LAND) &&
       (((units[unit].orders != ORDER_SLEEP || (units[unit].goto_x < 0)) ||
        (units[unit].goto_x == orig_unit)))) {
      has_land_unit = 1;
      city3 = unit_moves_left(unit);
      if ((int)(uint)cosmic.road_move_mult <= city3) {
        unit7 = unit;
        break;
      }
    }
  }
  if (unit7 < 0) {
    unit = orig_unit;
    if (((autoplay.active == 0) && (has_land_unit != 0)) && ((unit_sound_editor_window_block.unit_move_flags & 1) != 0)) {
      dlg_game_txt_with_unit((const char *)&str_nolandfall,orig_unit);
    }
    return 1;
  }
  if (((autoplay.active == 0) && ((unit_sound_editor_window_block.unit_move_flags & 1) != 0)) &&
     (answer = dlg_game_txt_with_unit((const char *)&str_landfall,orig_unit), unit = orig_unit, answer == 0))
  return 1;
  unit = unit7;
  units[unit7].orders = ORDER_NONE;
  return 0;
}

/* Part of unit_move_dir (0059062c): moving into a foreign city: ships and aircraft
   cannot occupy it (a nuclear missile strikes it), and attacking a civ at
   peace needs a declaration. Returns 1 to stop the move. */
static int unit_move_dir_enemy_city(iptr &unit,iptr orig_unit,uptr civ,uptr &civ2,byte civ3,
                                               iptr x2,int y2,int x3,int y3,uptr city,u8 unit_domain,
                                               uint &result)
{
  int city3;
  civ2 = (uptr)cities[city].owner;
  if ((unit_domain != '\0') && ((unit_domain != 1 || (unit_types[units[unit].type].range != '\0')))) {
    if ((char)unit_types[units[unit].type].attack < 'c') {
      unit = orig_unit;
      if ((((unit_sound_editor_window_block.unit_move_flags & 1) != 0) && ((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0))
         && (unit_sound_editor_window_block.unit_move_by_local_human != '\0')) {
        snd_play_effect(SND_NEG1,0,0,0);
        popup_game_with_sprite((const char *)&str_occupy,(iptr)&popup_info_icon_sprite,0);
      }
    }
    else if ((unit_sound_editor_window_block.unit_move_flags & 2) == 0) {
      result = 1;
      unit = orig_unit;
    }
    else {
      city3 = unit_nuclear_attack(civ,x2,y2,1);
      if (city3 == 0) {
        unit_delete_with_cargo(unit,1);
        map_redraw_tile_all_views(x3,y3);
        net_send_message(NM_DRAW_SQUARE,0xff,x3,y3,0,0,0,0,0,0);
        unit = orig_unit;
      }
      else {
        *(i8 *)&civs[civ].aggression_counter[civ2] = 0;
        unit = orig_unit;
      }
    }
    return 1;
  }
  if ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) != 0) {
    if ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) {
      units[unit].orders = ORDER_NONE;
      unit = orig_unit;
      return 1;
    }
    if ((((unit_sound_editor_window_block.unit_move_flags & 1) != 0) && (unit_sound_editor_window_block.unit_move_by_local_human != '\0')) &&
       (city3 = diplo_confirm_attack(civ,civ2,0xe), unit = orig_unit, city3 != 0))
    return 1;
  }
  return 0;
}

/* Part of unit_move_dir (0059062c): a move that did not happen: the unit's
   orders are cleared, and an AI unit stuck too long gives up for the turn. */
static void unit_move_dir_not_moved(iptr unit,byte civ3)
{
  units[unit].orders = ORDER_NONE;
  if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
    unit_exhaust_moves(unit);
  }
  if (unit_types[units[unit].type].role == ROLE_SEA_TRANSPORT) {
    units[unit].counter = (byte)game.turn & 7;
  }
  if ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) {
    if ((units[unit].flags & UNITF_AI_GARRISON) != 0) {
      units[unit].orders = ORDER_FORTIFIED;
    }
    units[unit].aux_timer++;
    if (19 < (char)units[unit].aux_timer) {
      units[unit].aux_timer = '\0';
      unit_exhaust_moves(unit);
    }
  }
}

/* 0059062c auto */
/* WARNING: Removing unreachable block (ram,0x0059103e) */
/* WARNING: Removing unreachable block (ram,0x0059104b) */
/* WARNING: Removing unreachable block (ram,0x00590f5a) */
/* WARNING: Removing unreachable block (ram,0x0059100a) */
/* WARNING: Removing unreachable block (ram,0x00591023) */
/* WARNING: Removing unreachable block (ram,0x00591027) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl unit_move_dir(iptr unit,uptr dir,i32 move_flags)

{
  short *continent_count;
  u8 unit_domain;
  byte civ3;
  bool flag;
  bool boarding;
  bool map_locked;
  char src_terrain;
  SHORT shift_state;
  uptr civ;
  iptr src_city;
  iptr in_bounds;
  byte unit_type;
  int city3;
  uptr city;
  iptr unit3;
  iptr orig_unit;
  int move_roll;
  int y2;
  uint result;
  iptr x2;
  int answer;
  int y3;
  int x3;
  /* has_hut, civ_sees[8], civ_animates[8] in a row, as in the original's
     frame: with no player here (human_civ -1) civ_sees[-1] is has_hut and
     civ_animates[-1] is civ_sees[7] (unit_move_dir_local_view) */
  int views[17];
  int &has_hut = views[0];
  int *civ_sees = views + 1;
  int *civ_animates = views + 9;
  uptr terrain2;
  uint src_is_ocean;
  int attack_done;
  uptr civ2;
  uint dst_is_ocean;
  int lock_id;
  int unit_count;
  iptr unit2;
  uint step_cost;
  uint prev_moves_used;
  i32 unused_c;
  /* the Barbarians' entries (human_civ 0 after Reveal Map), which the
     original leaves as its stack held them (B22) */
  civ_sees[0] = civ_animates[0] = 0;
  if (bugfix_ui()) {
    /* B14: a unit that does not move pauses as if the player saw it (docs/BUGS.md) */
    memset(civ_sees,0,8 * sizeof civ_sees[0]);
  }
  else {
    /* stale stack the original reads (FRAME_PRESETS; CIV2_STACKPROBE=59062c:a0:20
       in the Rome scenario: entries 1 to 5 non-zero, 6 and 7 zero) */
    civ_sees[1] = civ_sees[2] = civ_sees[3] = civ_sees[4] = civ_sees[5] = 1;
    civ_sees[6] = civ_sees[7] = 0;
  }
  
  attack_done = 0;
  map_locked = false;
  unused_c = 0;
  lock_id = 1;
  flag = false;
  if ((((unit_move.in_progress != 0) || (units[unit].id == 0)) || (units[unit].x < 0)) ||
     (units[unit].y < 0)) {
    return 0;
  }
  unit_move.in_progress = 1;
  unit_sound_editor_window_block.unit_move_flags = (byte)move_flags;
  win_flush_pending_msgs();
  while( true ) {
    orig_unit = unit;
    unit_move.unanimated_state = '\0';
    boarding = false;
    has_hut = 0;
    result = 0;
    if ((unit_sound_editor_window_block.unit_move_flags & 1) != 0) {
      blink.unit_active = 0;
    }
    unit_count = (int)game.num_units;
    game.transient_flags = game.transient_flags | TRANSIENT_ATTITUDE_DIRTY;
    x3 = (int)units[unit].x;
    y3 = (int)units[unit].y;
    y2 = y3;
    x2 = x3;
    if (-1 < (int)dir) {
      x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x3);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y3;
    }
    unit_domain = unit_types[units[unit].type].domain;
    civ3 = units[unit].owner;
    civ = (uptr)(char)civ3;
    src_city = city_at(x3,y3);
    if (src_city < 0) {
      tile_set_owner(x3,y3,civ,1);
    }
    unit_sound_editor_window_block.unit_move_by_local_human = autoplay.active == 0 && human_civ[0] == civ;
    in_bounds = map_in_bounds(x2,y2);
    if (in_bounds == 0) {
      if (((unit_sound_editor_window_block.unit_move_flags & 1) != 0) && (unit_sound_editor_window_block.unit_move_by_local_human != '\0')) {
        popup_game_with_sprite((const char *)&str_notonmap,(iptr)&popup_info_icon_sprite,0);
      }
      unit = orig_unit;
      if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
        unit_exhaust_moves(unit);
      }
      goto unlock_map;
    }
    if (((2 < game.multiplayer) && (net.mp_simultaneous_moves != '\0')) &&
       (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0 && (-1 < (int)dir)))) {
      city3 = net_lock_map(civ,x3,y3,x2,y2,lock_id);
      if (city3 == 0) {
        if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
          unit_exhaust_moves(unit);
        }
        goto send_net_update;
      }
      map_locked = true;
    }
    src_terrain = tile_terrain(x3,y3);
    unit_type = tile_terrain(x2,y2);
    terrain2 = (uptr)unit_type;
    src_is_ocean = (uint)(src_terrain == TERRAIN_OCEAN);
    dst_is_ocean = (uint)(terrain2 == UNIT_ALPINE_TROOPS);
    civ2 = tile_city_or_unit_owner(x2,y2);
    unit2 = tile_first_unit(x2,y2);
    if (-1 < unit2) {
      civ2 = (uptr)units[unit2].owner;
    }
    city = city_at(x2,y2);
    if (-1 < (int)city) {
      civ2 = (uptr)cities[city].owner;
    }
    if ((int)dir < 0) {
      unit_exhaust_moves(unit);
      unit3 = unit;
      goto finish_move;
    }
    units[unit].flags = units[unit].flags | UNITF_MOVED;
    if (unit_domain == DOMAIN_LAND) {
      if (unit_move_dir_land_unit_checks(unit,orig_unit,dir,civ,civ2,civ3,x2,y2,x3,y3,city,unit2,
                                         src_is_ocean) != 0) goto unlock_map;
    }
    if ((unit_domain == DOMAIN_LAND) && (dst_is_ocean != 0)) {
      unit = orig_unit;
      if ((civ2 != civ) || (city3 = unit_stack_sum(unit2,6), city3 < 1))
      goto unlock_map;
      boarding = true;
    }
    if ((-1 < unit2) && ((int)units[unit2].owner != civ)) {
      if (unit_move_dir_attack_unit(unit,orig_unit,dir,civ,civ2,civ3,x2,y2,x3,y3,city,unit2,unit_domain,
                                    src_is_ocean,dst_is_ocean,result,attack_done,unit3,answer) != 0)
      goto finish_move;
      goto unlock_map;
    }
    if (((dst_is_ocean != 0) || (-1 < (int)city)) || (unit_domain != DOMAIN_SEA)) break;
    if (unit_move_dir_pick_landing_unit(unit,orig_unit,x3,y3,unit_count,answer) != 0) goto unlock_map;
  }
  if ((-1 < (int)city) && ((int)cities[city].owner != civ)) {
    if (unit_move_dir_enemy_city(unit,orig_unit,civ,civ2,civ3,x2,y2,x3,y3,city,unit_domain,result) != 0)
    goto unlock_map;
  }
  city3 = unit_moves_left(unit);
  unit = orig_unit;
  if (city3 == 0) goto unlock_map;
  step_cost = unit_move_dir_step_cost(unit,x3,y3,x2,y2,terrain2,unit_domain);
  city3 = unit_moves_left(unit);
  if ((((city3 < (int)step_cost) && (unit_domain == DOMAIN_LAND)) && (units[unit].moves_used != '\0')) &&
     ((((unit_sound_editor_window_block.unit_move_by_local_human != '\0' && ((unit_sound_editor_window_block.unit_move_flags & 1) != 0)) ||
       (((*(i8 *)&human_civ) == civ && ((unit_sound_editor_window_block.unit_move_flags & 1) != 0)))) ||
      (((unit_sound_editor_window_block.unit_move_flags & 2) != 0 && ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0)))))) {
    if (step_cost == 1 || (int)(step_cost - 1) < 0) {
      move_roll = 0;
    }
    else {
      move_roll = _rand();
      move_roll = move_roll % (int)step_cost;
    }
    city3 = unit_moves_left(unit);
    if (city3 <= move_roll) {
      unit_exhaust_moves(unit);
      result = 1;
      goto unlock_map;
    }
  }
  if ((unit_sound_editor_window_block.unit_move_flags & 2) == 0) {
    result = 1;
    goto unlock_map;
  }
  unit_type = units[unit].moves_used;
  prev_moves_used = (uint)unit_type;
  units[unit].moves_used += (char)step_cost;
  if ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) {
    if (unit_move_dir_ai_holds_back(unit,dir,civ,civ2,unit2,city,x2,y2,x3,y3,unit_domain,step_cost,
                                    unit_type,prev_moves_used) != 0) goto unlock_map;
  }
  if (((civ == 0) && (dst_is_ocean == 0)) &&
     (((city3 = tile_visible_to(x3,y3,(*(i8 *)&human_civ)), city3 != 0 &&
       ((((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) == 0 &&
         ((int)units[unit].owner != ((*(i8 *)&human_civ) & 0xffU))) &&
        (city3 = city_find_nearest(x3,y3,-1,0xffffffff,-1), -1 < city3)))) &&
      (cities[city3].owner == (*(i8 *)&human_civ))))) {
    city3 = (iptr)tile_at(x3,y3);
    units[unit].visibility = *(byte *)(city3 + 4) | units[unit].visibility;
  }
  if ((((-1 < (int)city) && (civ2 != civ)) &&
      (city3 = tile_visible_to(x2,y2,(*(i8 *)&human_civ)), city3 != 0)) &&
     ((((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & TREATY_EMBASSY) != 0 ||
       ((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ2] & TREATY_EMBASSY) != 0)) ||
      ((*(i8 *)&human_civ) == civ2)))) {
    unit_set_seen_by(unit,(*(i8 *)&human_civ));
  }
  unit_move_dir_viewers(unit,civ,x2,y2,city,step_cost,unit_domain,civ_sees,civ_animates);
  unit_move_dir_landing_alert(unit,civ,civ2,x2,y2,x3,y3,src_is_ocean,dst_is_ocean,civ_sees);
  net_send_state_diff(0xff,2,0,0,1);
  if (((game.multiplayer < 3) || (net.host_flag != '\0')) ||
     ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0)) {
    has_hut = unit_move_dir_relocate(unit,civ2,x2,y2,x3,y3,dir,dst_is_ocean,civ_sees,civ_animates);
  }
  else {
    has_hut = unit_move_dir_relocate_via_host(unit,civ,civ2,x2,y2,x3,y3,dir,dst_is_ocean,lock_id,civ_sees,
                                              civ_animates,flag,map_locked);
  }
  if (((unit_domain == DOMAIN_LAND) && (src_is_ocean != 0)) && (dst_is_ocean == 0)) {
    city3 = tile_landmass(x2,y2);
    continent_count = (short *)(city3 * 2 + (iptr)((u8 *)&civs + 402) + civ * 0x594);
    *continent_count = *continent_count + 1;
  }
  if ((-1 < (int)city) && (civ2 != civ)) {
    units[unit].damage = '\0';
    game.moving_unit = (i16)unit;
    city_capture_aftermath(city,civ,0);
    unit = (iptr)game.moving_unit;
  }
  unit3 = unit;
  if (((civ == 0) && (dst_is_ocean == 0)) &&
     ((city3 = tile_visible_to(x2,y2,(*(i8 *)&human_civ)), city3 != 0 &&
      (((((1 << (civ3 & 0x1f) & (uint)units[unit].visibility) == 0 &&
         ((int)units[unit].owner != (uint)civ3)) &&
        (city3 = city_find_nearest(x2,y2,-1,0xffffffff,-1), -1 < city3)) &&
       (cities[city3].owner == (*(i8 *)&human_civ))))))) {
    city3 = (iptr)tile_at(x2,y2);
    unit_stack_add_visibility(unit,(uint)*(byte *)(city3 + 4));
  }
  if (((src_is_ocean != 0) && (-1 < (int)city)) &&
     (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0 &&
      (unit_types[units[unit].type].hold != '\0')))) {
    for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit))
    {
      if (units[unit].orders == ORDER_SLEEP) {
        units[unit].orders = ORDER_NONE;
      }
    }
  }
finish_move:
  unit = unit3;
  unit3 = unit;
  if ((((game.multiplayer < 3) || (net.host_flag != '\0')) || (!flag)) &&
     ((-1 < units[unit].next_in_stack && (unit_types[units[unit].type].attack == '\0')))) {
    unit_move_to_stack_bottom(unit,0);
  }
  unit_reveal_surroundings(unit,1);
  ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(unit,x3,y3);
  if (((((*(i8 *)&human_civ) != civ) && (unit_move_dir_local_view(civ_sees) != 0)) &&
      (((*((char *)&game.game_options + 1)) & (OPT_NO_PAUSE_ENEMY >> 8)) == 0)) &&
     (shift_state = GetAsyncKeyState(0x10), (char)((ushort)shift_state >> 8) == '\0')) {
    if ((((*((char *)&game.game_options + 1)) & (OPT_SHOW_ENEMY_MOVES >> 8)) == 0) || (unit_move.unanimated_state != '\0')) {
      ui_delay_ticks(10);
    }
    else {
      ui_delay_ticks(0xf);
    }
  }
  if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
    if (unit_move_dir_air_fuel(unit,x2,y2,attack_done,flag,x3,y3) != 0) goto unlock_map;
  }
  if (unit_move_dir_trireme_loss(unit,civ,x2,y2,flag,x3,y3) != 0) goto unlock_map;
  if ((has_hut != 0) && (unit_types[units[unit].type].domain == DOMAIN_LAND)) {
    unit_enter_hut(unit);
  }
  unit_move_dir_wake_settlers(unit,civ,civ3,x2,y2,unit_domain,flag);
  if (boarding) {
    unit_move_dir_board_ship(unit3,x2,y2);
  }
  unit_move_dir_long_move(unit3,dir);
  units[unit3].last_dir = (u8)dir;
  result = 1;
  unit = unit3;
unlock_map:
  if (map_locked) {
    net_unlock_map(civ,lock_id);
  }
send_net_update:
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_DRAW_AREA,0xff,x3,y3,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  if (game.num_units != unit_count) {
    unit_move.in_progress = 0;
    return 0;
  }
  if (result == 0) {
    unit_move_dir_not_moved(unit,civ3);
  }
  else {
    units[unit].flags = units[unit].flags & ~UNITF_AI_GARRISON;
  }
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
  }
  unit_move.in_progress = 0;
  return result;
}

/* 005aef20 auto */
void __cdecl unit_editor_load_rules(void)

{
  char *_Source;
  size_t _Count;
  iptr unit_type;
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    _Count = 0x28;
    _Source = label_string(unit_types[unit_type].name);
    _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[unit_type],_Source,_Count);
    editor_text_dlg_wnd_block.editor_item_name_recs[unit_type][0x27] = 0;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[0] = (int)unit_types[unit_type].prerequisite;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[1] = (int)unit_types[unit_type].obsolete_by;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[2] = (int)(char)unit_types[unit_type].role;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3] = (int)(char)unit_types[unit_type].domain;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[4] = (int)(char)unit_types[unit_type].cost;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[5] = (int)(char)unit_types[unit_type].hold;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[6] =
         (int)(char)unit_types[unit_type].move / (int)(uint)cosmic.road_move_mult;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[7] = (int)(char)unit_types[unit_type].range;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8] = (int)(char)unit_types[unit_type].attack;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[9] = (int)(char)unit_types[unit_type].defense;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[10] = (int)(char)unit_types[unit_type].hit_points / 10;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[11] = (int)(char)unit_types[unit_type].firepower;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[12] = unit_types[unit_type].flags;
  }
  return;
}

/* 005af140 auto */
void __cdecl unit_editor_store_rules(void)

{
  char *_Dest;
  char *_Source;
  size_t _Count;
  iptr unit_type;
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    _Count = 0xf;
    _Source = editor_text_dlg_wnd_block.editor_item_name_recs[unit_type];
    _Dest = label_string(unit_types[unit_type].name);
    editor_copy_pool_name(_Dest,_Source,_Count);  /* B32 (docs/BUGS.md) */
    unit_types[unit_type].prerequisite = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[0];
    unit_types[unit_type].obsolete_by = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[1];
    unit_types[unit_type].role = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[2];
    unit_types[unit_type].domain = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3];
    unit_types[unit_type].cost = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[4];
    unit_types[unit_type].hold = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[5];
    unit_types[unit_type].move =
         (char)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[6] * cosmic.road_move_mult;
    unit_types[unit_type].range = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[7];
    unit_types[unit_type].attack = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8];
    unit_types[unit_type].defense = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[9];
    unit_types[unit_type].hit_points = (char)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[10] * '\n';
    unit_types[unit_type].firepower = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[11];
    unit_types[unit_type].flags = (u32)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[12];
  }
  return;
}

/* 005af343 auto */
void __cdecl unit_editor_load_controls(void)

{
  int ctrl_id;
  int i;
  char num_buf[8];
  iptr sel;
  
  for (i = 1; i < 0xd; i++) {
    if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 9) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      _sprintf(num_buf,((i8 *)&unit_editor_ctrl_kinds_block.unit_editor_int_fmt),
               *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58));
      ((Control *)
                 &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->set_text(
                 (const char *)num_buf);
    }
    else if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 0xc) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      sel = *(iptr *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58);
      if ((i == 1) || (i == 2)) {
        sel += 2;
      }
      editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot].set_cur_sel(sel);
    }
  }
  return;
}

/* 005af4ae auto */
int __cdecl unit_editor_store_controls(void)

{
  int field;
  i32 clamped;
  int i;
  int changed_count;
  char num_buf[8];
  int value;
  
  changed_count = 0;
  for (i = 1; i < 0xd; i++) {
    if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 9) {
      field = ((Control *)
                         &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      field -= 0xca;
      ((Control *)
                 &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->Control::get_text(
                 (char *)num_buf);
      value = _atoi(num_buf);
      clamped = int_clamp(value,*(i32 *)(((i8 *)&unit_editor_ctrl_kinds_block.unit_editor_field_min_table) + field * 4),
                                 *(i32 *)(((i8 *)&unit_editor_ctrl_kinds_block.unit_editor_field_max_table) + field * 4));
      editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] = clamped;
      if (editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] != value) {
        changed_count++;
      }
    }
    else if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 0xc) {
      value = editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot].get_cur_sel();
      if ((i == 1) || (i == 2)) {
        value -= 2;
      }
      field = ((Control *)
                         &editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_active.wnd->item * 0x58 + field * 4) = value;
    }
  }
  return changed_count;
}

/* 005af682 auto */
void __cdecl unit_editor_redraw(void)

{
  editor_active.wnd->unit_editor_paint();
  return;
}

/* 005af69d auto */
i32 __cdecl unit_editor_write_units(FILE *file)

{
  char *type_name;
  size_t name_len;
  size_t pad_len;
  iptr unit_type;
  int bit;
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    text_clear();
    text_append_string_id(unit_types[unit_type].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_name_sep));
    type_name = label_string(unit_types[unit_type].name);
    name_len = _strlen(type_name);
    if (name_len < 0xd) {
      type_name = label_string(unit_types[unit_type].name);
      pad_len = _strlen(type_name);
    }
    else {
      pad_len = 0xd;
    }
    text_append_spaces(0xd - pad_len);
    text_append_advance_abbrev((int)unit_types[unit_type].obsolete_by,1);
    text_append_int_2d((int)(char)unit_types[unit_type].domain);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_domain_sep));
    text_append_int_2d((int)(char)unit_types[unit_type].move / (int)(uint)cosmic.road_move_mult);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_move_suffix));
    text_append_int((int)(char)unit_types[unit_type].range);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_range_sep));
    text_append_int_2d((int)(char)unit_types[unit_type].attack);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_attack_suffix));
    text_append_int((int)(char)unit_types[unit_type].defense);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_defense_suffix));
    if ((char)unit_types[unit_type].defense < '\n') {
      text_append_space();
    }
    text_append_int_2d((int)(char)unit_types[unit_type].hit_points / 10);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_hp_suffix));
    text_append_int((int)(char)unit_types[unit_type].firepower);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_firepower_suffix));
    text_append_int_2d((int)(char)unit_types[unit_type].cost);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_cost_sep));
    text_append_int((int)(char)unit_types[unit_type].hold);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_hold_sep));
    text_append_int_2d((int)(char)unit_types[unit_type].role);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_role_sep));
    text_append_advance_abbrev((int)unit_types[unit_type].prerequisite,1);
    text_append_space();
    for (bit = 0xe; -1 < bit; bit--) {
      crt_strcat(((i8 *)&textbuf.buf),
                   ((i8 *)&unit_editor_ctrl_kinds_block.units_txt_flag_digits) +
                   (((unit_types[unit_type].flags & 1 << ((byte)bit & 0x1f)) != 0) - 1 & 4));
    }
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_line_end));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 005af9e3 auto */
void __cdecl unit_editor_on_ok(void)

{
  iptr invalid_count;
  HWND hWnd;
  const char *str;
  iptr cmp;
  i32 old_name;
  int result;
  char *new_name;
  i8 *edited_name;
  UINT box_flags;
  iptr parent_wnd2;
  char rules_err_msg[32];
  char events_err_msg[32];
  iptr unit_type;
  int renamed_count;
  
  invalid_count = unit_editor_store_controls();
  if (invalid_count == 0) {
    renamed_count = 0;
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      new_name = editor_text_dlg_wnd_block.editor_item_name_recs[unit_type];
      str = label_string(unit_types[unit_type].name);
      cmp = _strcmp(str,new_name);
      if (cmp != 0) {
        edited_name = editor_text_dlg_wnd_block.editor_item_name_recs[unit_type];
        old_name = (iptr)label_string(unit_types[unit_type].name);
        result = events_rename_unit_refs((char *)old_name,(iptr)edited_name);
        renamed_count += result;
      }
    }
    if (renamed_count != 0) {
      result = events_write_file();
      if (result == 0) {
        _sprintf(events_err_msg,str_error_updating_events_s_2,((i8 *)&lang_file_ext));
        box_flags = 0x10;
        str = str_file_i_o_error_7;
        new_name = events_err_msg;
        result = (int)((WndBase *)&editor_active.wnd->window)->get_window();
        MessageBoxA(((NativeWindow *)result)->hwnd,new_name,str,box_flags);
      }
    }
    unit_editor_store_rules();
    rules_edit_set_section((iptr)&str_units_3,(RulesSectionWriterFn)unit_editor_write_units);
    result = rules_write_sections();
    if (result == 0) {
      _sprintf(rules_err_msg,str_error_updating_rules_s_6,((i8 *)&lang_file_ext));
      box_flags = 0x10;
      str = str_file_i_o_error_8;
      new_name = rules_err_msg;
      result = (int)((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(((NativeWindow *)result)->hwnd,new_name,str,box_flags);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    unit_editor_load_controls();
    unit_editor_redraw();
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice_16);
    dlg_set_parent_window(0);
    hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
    SetFocus(hWnd);
  }
  return;
}

/* 005afbca auto */
void __cdecl unit_editor_on_rename(void)

{
  size_t input_len;
  iptr parent_wnd2;
  char old_name[15];
  char new_name[260];
  int item;
  iptr sel;
  int result;
  
  item = editor_active.wnd->item;
  _strncpy(old_name,editor_text_dlg_wnd_block.editor_item_name_recs[item],0xf);
  old_name[14] = 0;
  do {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_push_parent_window(parent_wnd2);
    result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_unitname,0xe,(iptr)old_name,(iptr)new_name);
    dlg_pop_parent_window();
    if (result == -1) break;
    input_len = _strlen(new_name);
  } while (input_len == 0);
  if (-1 < result) {
    crt_strcpy(editor_text_dlg_wnd_block.editor_item_name_recs[item],new_name);
    sel = editor_active.wnd->combos[0].get_cur_sel();
    editor_active.wnd->combos[0].clear();
    for (item = 0; item < 0x3e; item++) {
      editor_active.wnd->combos[0].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[item]);
    }
    editor_active.wnd->combos[0].set_cur_sel(sel);
    unit_editor_redraw();
  }
  return;
}

/* 005afd3b auto */
void __cdecl unit_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_19,(const char *)&str_units_4);
  dlg_set_parent_window(0);
  return;
}

/* 005afd9a auto */
void __cdecl unit_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 005afdc2 auto */
void __cdecl unit_editor_edit_sprite(void)

{
  i32 title;
  i32 count;
  SpriteSheetSaveFn save_cb;
  
  save_cb = sprite_sheet_save_units;
  count = 0xb;
  title = (iptr)label_string(label_table.string_ids[0x1f6]);
  sprite_editor_run((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + editor_active.wnd->item * 0x3c),(const char *)title,count,
                     save_cb);
  unit_editor_redraw();
  return;
}

/* 005afe15 auto */
void __cdecl unit_editor_edit_sound(void)

{
  i32 label;
  i8 *item_name;
  char title[128];
  
  unit_editor_store_controls();
  item_name = editor_text_dlg_wnd_block.editor_item_name_recs[editor_active.wnd->item];
  label = (iptr)label_string(label_table.string_ids[0x1f7]);
  /* BF-049 (Peter Johnson's ledger): a longer label is cut at 127 characters */
  snprintf(title,sizeof title,str_s_s_4,(char *)label,item_name);
  unit_sound_editor_run((const char *)title,editor_active.wnd->item);
  return;
}

/* 005afe84 auto */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __cdecl unit_editor_set_flag_check(i32 index,int checked)

{
  dlg_check_set(index,(uint)(checked != 0));
  return;
}

/* 005afea8 auto */
void __cdecl unit_editor_edit_flags(void)

{
  int item;
  int result;
  iptr parent_wnd2;
  uint flags2;
  
  item = editor_active.wnd->item;
  dlg_checks_clear();
  unit_editor_set_flag_check(0,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 1);
  unit_editor_set_flag_check(1,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 2);
  unit_editor_set_flag_check(2,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 4);
  unit_editor_set_flag_check(3,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 8);
  unit_editor_set_flag_check(4,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x10);
  unit_editor_set_flag_check(5,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x20);
  unit_editor_set_flag_check(6,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x40);
  unit_editor_set_flag_check(7,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x80);
  unit_editor_set_flag_check(8,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x100);
  unit_editor_set_flag_check(9,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x200);
  unit_editor_set_flag_check(10,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x400);
  unit_editor_set_flag_check(0xb,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x800);
  unit_editor_set_flag_check(0xc,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x1000);
  unit_editor_set_flag_check(0xd,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x2000);
  unit_editor_set_flag_check(0xe,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x4000);
  text_set_string_arg(0,(const char *)editor_text_dlg_wnd_block.editor_item_name_recs[item]);
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  result = popup_game_flags((const char *)&str_abilities,1);
  if (result == 0) {
    result = dlg_check_get(0);
    flags2 = (uint)(result != 0);
    result = dlg_check_get(1);
    if (result != 0) {
      flags2 = flags2 | 2;
    }
    result = dlg_check_get(2);
    if (result != 0) {
      flags2 = flags2 | 4;
    }
    result = dlg_check_get(3);
    if (result != 0) {
      flags2 = flags2 | 8;
    }
    result = dlg_check_get(4);
    if (result != 0) {
      flags2 = flags2 | 0x10;
    }
    result = dlg_check_get(5);
    if (result != 0) {
      flags2 = flags2 | 0x20;
    }
    result = dlg_check_get(6);
    if (result != 0) {
      flags2 = flags2 | 0x40;
    }
    result = dlg_check_get(7);
    if (result != 0) {
      flags2 = flags2 | 0x80;
    }
    result = dlg_check_get(8);
    if (result != 0) {
      flags2 = flags2 | 0x100;
    }
    result = dlg_check_get(9);
    if (result != 0) {
      flags2 = flags2 | 0x200;
    }
    result = dlg_check_get(10);
    if (result != 0) {
      flags2 = flags2 | 0x400;
    }
    result = dlg_check_get(0xb);
    if (result != 0) {
      flags2 = flags2 | 0x800;
    }
    result = dlg_check_get(0xc);
    if (result != 0) {
      flags2 = flags2 | 0x1000;
    }
    result = dlg_check_get(0xd);
    if (result != 0) {
      flags2 = flags2 | 0x2000;
    }
    result = dlg_check_get(0xe);
    if (result != 0) {
      flags2 = flags2 | 0x4000;
    }
    editor_text_dlg_wnd_block.editor_item_recs[item].values[12] = flags2;
  }
  dlg_set_parent_window(0);
  return;
}

/* 005b02a5 auto */
void __cdecl unit_editor_update_domain_ctrls(void)

{
  int domain;
  
  domain = editor_active.wnd->combos[4].get_cur_sel();
  if (domain == 0) {
    ((Control *)&editor_active.wnd->edits[1])->hide();
    ((Control *)&editor_active.wnd->edits[3])->hide();
  }
  else if (domain == 1) {
    ((Control *)&editor_active.wnd->edits[1])->hide();
    ((Control *)&editor_active.wnd->edits[3])->show_and_redraw();
  }
  else if (domain == 2) {
    ((Control *)&editor_active.wnd->edits[1])->show_and_redraw();
    ((Control *)&editor_active.wnd->edits[3])->hide();
  }
  return;
}

/* 005b0373 auto */
void __cdecl unit_editor_on_combo(int ctrl_id)

{
  int invalid_count;
  HWND hWnd;
  i32 sel;
  i32 parent_wnd2;
  
  if (ctrl_id == 0xc9) {
    invalid_count = unit_editor_store_controls();
    if (invalid_count == 0) {
      sel = editor_active.wnd->combos[0].get_cur_sel();
      editor_active.wnd->item = sel;
      unit_editor_load_controls();
      unit_editor_update_domain_ctrls();
      unit_editor_redraw();
    }
    else {
      editor_active.wnd->combos[0].set_cur_sel(editor_active.wnd->item);
      unit_editor_load_controls();
      unit_editor_redraw();
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_debug_11,(const char *)&str_notice_17);
      dlg_set_parent_window(0);
      hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
      SetFocus(hWnd);
    }
  }
  else if (ctrl_id == 0xcd) {
    unit_editor_update_domain_ctrls();
  }
  return;
}

/* 005b0473 auto */
void EditorWindow::unit_editor_create_combo(int index)

{
  int x;
  int y;
  iptr label;
  u8 *parent2;
  iptr advance;
  iptr unit_type;
  i8 rect[16];
  
  x = unit_editor_ctrl_positions_block.unit_editor_combo_pos[index].x + client_x;
  y = unit_editor_ctrl_positions_block.unit_editor_combo_pos[index].y + client_y;
  if (index == 0) {
    rect_set_xywh((iptr)rect,x - 0xf,y,0x82,font_height_uptr << 3);
  }
  else {
    rect_set_xywh((iptr)rect,x - 0x1e,y,0xa0,font_height_uptr << 3);
  }
  x = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + index * 0x38 + 0x28))->create((iptr)parent2,x,
             (iptr)rect);
  ((ComboBox *)(text_font + index * 0x38 + 0x28))->set_font((iptr)&editor_font_block.editor_font);
  ((ComboBox *)(text_font + index * 0x38 + 0x28))->set_callback(unit_editor_on_combo);
  switch(index) {
  case 0:
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      label = (iptr)label_string(unit_types[unit_type].name);
      ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 1:
  case 2:
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f1]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    for (advance = 0; advance < 100; advance++) {
      label = (iptr)label_string(advances[advance].name);
      ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 3:
    label = (iptr)label_string(label_table.string_ids[0x1e5]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1e6]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f8]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f9]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fa]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fb]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fc]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fd]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    break;
  case 4:
    label = (iptr)label_string(label_table.string_ids[0x1fe]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1ff]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x200]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
  }
  return;
}

/* 005b08e8 auto */
void EditorWindow::unit_editor_create_edit(int index)

{
  int ctrl_id;
  u8 *parent2;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,(((i32 *)&unit_editor_edit_x_table_block.unit_editor_edit_x_table))[index * 2] + client_x,
                     (((i32 *)&unit_editor_edit_x_table_block.unit_editor_edit_y_table))[index * 2] + client_y,0x30,font_height_uptr + 6);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  ((EditControl *)(numeric_edit_controls_area + index * 0x48 - 0x2c))->create((iptr)parent2,ctrl_id,
             (iptr)rect,(const char *)&str_empty_186);
  ((EditControl *)(numeric_edit_controls_area + index * 0x48 - 0x2c))->set_max_len(3);
  ((EditControl *)(numeric_edit_controls_area + index * 0x48 - 0x2c))->set_callback(editor_numeric_edit_filter);
  return;
}

/* 005b09dc auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void EditorWindow::unit_editor_paint()

{
  byte key_color;
  i8 sprite_rect[16];
  Sprite *sprite;
  int text_y2;
  int text_x2;
  int pic_y;
  int pic_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((*(int *)&editor_ui.bg_surface),(iptr)this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  pic_x = client_x + 0x20;
  pic_y = client_y + 0x20;
  sprite = (Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + item_uptr * 0x3c);
  key_color = sprite->get_key_color();
  gfx_fill_rect_xywh((iptr)this,pic_x,pic_y,0x40,0x40,(uint)key_color);
  sprite->draw_keyed((iptr)sprite_rect,(iptr)this,0,pic_x,pic_y + 8);
  gfx_draw_tiled_frame((iptr)this,pic_x,pic_y,0x40,0x40,6);
  gfx_draw_tiled_frame((iptr)this,client_x + 0x79,client_y + 0x4f,0x14d,0x47,6);
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  text_x2 = client_x + (int)wnd_width_u / 2 + 5;
  text_y2 = unit_editor_ctrl_positions_block.unit_editor_prereq_y + client_y + font_height_uptr * -2 - 2;
  text_clear();
  text_append_label(0x1e9);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_prereq_x + client_x + 0x32;
  text_y2 = ((unit_editor_ctrl_positions_block.unit_editor_prereq_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_obsolete_x + client_x + 0x32;
  text_y2 = ((unit_editor_ctrl_positions_block.unit_editor_obsolete_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1de);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_role_x + client_x + 0x32;
  text_y2 = ((unit_editor_ctrl_positions_block.unit_editor_role_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1df);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_domain_x + client_x + 0x32;
  text_y2 = ((client_y + unit_editor_ctrl_positions_block.unit_editor_domain_y_i32) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e0);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit_x_table[0] + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e1);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_holds_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_holds_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e2);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_move_x + client_x + 0x18;
  text_y2 = ((client_y + unit_editor_edit_x_table_block.unit_editor_edit2_y_i32) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e3);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit3_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit3_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e4);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit4_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit4_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e5);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit5_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit5_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e6);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit6_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit6_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e7);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit7_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit7_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e8);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,text_x2,text_y2,0);
  wnd_blit_surface();
  return;
}

/* 005b1037 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void EditorWindow::unit_editor_window_run()

{
  DibSurface *this_00;
  uptr font_height;
  i32 title;
  int btn_x;
  int btn_y;
  iptr label;
  i32 wnd_style;
  i32 wnd_x;
  i32 wnd_y;
  u32 wnd_width;
  u32 wnd_height;
  i32 wnd_arg7;
  i32 wnd_arg8;
  i32 wnd_arg9;
  u8 *flags_parent;
  u8 *sound_parent;
  u8 *sprite_parent;
  u8 *cancel_parent;
  u8 *help_parent;
  u8 *rename_parent;
  u8 *ok_parent;
  DibSurface *bg_surface2;
  int i;
  i8 rect[16];
  GamePalette palette;
  i32 eh_next;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    bg_surface2 = (DibSurface *)0x0;
  }
  else {
    bg_surface2 = surface_ctor(this_00);
  }
  editor_ui.bg_surface = bg_surface2;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  wnd_width_u = 0x230;
  wnd_height_u = 0x17c;
  item_uptr = 0;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editorsa_gif_2,10,0xc0,(iptr)&palette);
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  wnd_arg9 = 0;
  wnd_arg8 = 0;
  wnd_arg7 = 0;
  wnd_height = wnd_height_u;
  wnd_width = wnd_width_u;
  wnd_y = 0;
  wnd_x = 0;
  wnd_style = 0xd;
  title = (iptr)label_string(label_table.string_ids[0x1cb]);
  this->FrameWindow::create((const char *)title,wnd_style,wnd_x,wnd_y,wnd_width,wnd_height,wnd_arg7,wnd_arg8,wnd_arg9);
  for (i = 0; i < 0xd; i++) {
    if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 9) {
      unit_editor_create_edit(unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot);
    }
    else if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 0xc) {
      unit_editor_create_combo(unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot);
    }
  }
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_x = client_width - 10;
  button_width_u = (int)(btn_x + (btn_x >> 0x1f & 3U)) >> 2;
  btn_y = (client_y + client_height) - (button_height_u + 2);
  btn_x = client_x + 2;
  rect_set_xywh((iptr)rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)ok_parent,0x65,(iptr)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)unit_editor_on_ok);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((iptr)rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    rename_parent = (u8 *)0x0;
  }
  else {
    rename_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x2a]);
  rename_button.create((iptr)rename_parent,0x66,(iptr)rect,(const char *)label);
  rename_button.set_callback((CtrlNotifyFn)unit_editor_on_rename);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((iptr)rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((iptr)help_parent,0x66,(iptr)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)unit_editor_on_help);
  rect_set_xywh((iptr)rect,btn_x + button_width_u + 2,btn_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)cancel_parent,0x66,(iptr)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)unit_editor_on_cancel);
  cancel_button.set_cancel_button();
  btn_x = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c))->width();
  button_width_u = btn_x + 0xd;
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_y = client_x + 0x19;
  btn_x = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c))->height();
  btn_x = btn_x * 2 + client_y + 0x28;
  rect_set_xywh((iptr)rect,btn_y,btn_x,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    sprite_parent = (u8 *)0x0;
  }
  else {
    sprite_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((iptr)sprite_parent,0x65,(iptr)rect,(const char *)label);
  icon_button.set_callback((CtrlNotifyFn)unit_editor_edit_sprite);
  btn_x = btn_x + button_height_u + 2;
  rect_set_xywh((iptr)rect,btn_y,btn_x,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    sound_parent = (u8 *)0x0;
  }
  else {
    sound_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[500]);
  unit_sound_button.create((iptr)sound_parent,0x65,(iptr)rect,(const char *)label);
  unit_sound_button.set_callback((CtrlNotifyFn)unit_editor_edit_sound);
  rect_set_xywh((iptr)rect,btn_y,btn_x + button_height_u + 2,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    flags_parent = (u8 *)0x0;
  }
  else {
    flags_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f5]);
  unit_flags_button.create((iptr)flags_parent,0x65,(iptr)rect,(const char *)label);
  unit_flags_button.set_callback((CtrlNotifyFn)unit_editor_edit_flags);
  window.set_keyboard_nav(0);
  unit_editor_ctrl_kinds_block.unit_editor_move_max_i32 = (i32)(0x7f / (ulonglong)(longlong)(int)(uint)cosmic.road_move_mult);
  unit_editor_ctrl_kinds_block.unit_editor_hp_max_i32 = 0xc;
  unit_editor_load_rules();
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  unit_editor_load_controls();
  unit_editor_update_domain_ctrls();
  combo_count_u = 5;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(unit_editor_redraw);
  repaint();
  this->SurfaceWindow::show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (editor_lists.running != 0) {
    win_pump_one_message();
  }
  if (editor_ui.bg_surface != (DibSurface *)0x0) {
    editor_ui.bg_surface->surface_delete(1);
  }
  editor_ui.bg_surface = (DibSurface *)0x0;
  combo_count_u = 0;
  return;
}

/* 005b1a29 auto */
void __cdecl unit_editor_run(void)

{
  EditorWindow editor_wnd;
  i32 eh_next;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.unit_editor_window_run();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 005b2590 auto */
i32 __cdecl unit_stack_validate(iptr unit)

{
  short x;
  short y;
  iptr stack_count;
  iptr unit5;
  iptr unit3;
  iptr unit2;
  iptr unit4;
  i32 valid;
  
  if ((unit < 0) || (0x801 < unit)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_id_0_id_max_units_2,(iptr)&str_d_ss_franklinton_unit_cpp,0x11);
  }
  valid = 1;
  if (net.unit_stack_check_enabled == 0) {
    stack_count = unit_stack_sum(unit,0xb);
    if (0x7ff < stack_count) {
      dbg_logf((const char *)&str_infinite_unit_stack_id_d_repairing,unit);
      valid = 0;
      unit4 = unit;
      x = units[unit].next_in_stack;
      do {
        unit2 = (iptr)x;
        if (unit4 < 0) break;
        x = units[unit4].x;
        y = units[unit4].y;
        unit_stack_unlink(unit4,0);
        unit_stack_link(unit4,(int)x,(int)y,0);
        unit4 = unit2;
        x = units[unit2].next_in_stack;
      } while (unit2 != unit);
    }
    unit2 = unit;
    unit3 = unit2;
    while (unit2 = unit3, -1 < units[unit2].prev_in_stack) {
      unit5 = (iptr)units[unit2].prev_in_stack;
      unit3 = unit5;
      if (units[units[unit2].prev_in_stack].id == 0) {
        dbg_logf((const char *)&str_dead_unit_in_unit_stack_id_d_repairing,unit2);
        valid = 0;
        units[unit2].prev_in_stack = -1;
        unit3 = unit5;
        if (units[unit5].next_in_stack == unit2) {
          units[unit5].next_in_stack = -1;
          unit3 = unit5;
        }
      }
    }
    unit2 = unit;
    unit3 = unit2;
    while (unit2 = unit3, -1 < units[unit2].next_in_stack) {
      unit3 = (iptr)units[unit2].next_in_stack;
      if (units[unit2].id == 0) {
        dbg_logf((const char *)&str_dead_unit_in_unit_stack_id_d_repairing_2,unit2);
        valid = 0;
        units[unit2].next_in_stack = -1;
        if (units[unit3].prev_in_stack == unit2) {
          units[unit3].prev_in_stack = -1;
        }
      }
    }
    unit2 = unit;
    unit3 = unit2;
    while (unit2 = unit3, -1 < units[unit2].prev_in_stack) {
      unit3 = (iptr)units[unit2].prev_in_stack;
      if ((units[units[unit2].prev_in_stack].x != units[unit2].x) ||
         (units[units[unit2].prev_in_stack].y != units[unit2].y)) {
        dbg_logf((const char *)&str_crossed_locations_in_unit_stack_id_d_rep,unit2);
        valid = 0;
        units[unit2].prev_in_stack = -1;
        if (units[unit3].next_in_stack == unit2) {
          units[unit3].next_in_stack = -1;
        }
      }
    }
    unit2 = unit;
    unit3 = unit2;
    while (unit2 = unit3, -1 < units[unit2].next_in_stack) {
      unit3 = (iptr)units[unit2].next_in_stack;
      if ((units[units[unit2].next_in_stack].x != units[unit2].x) ||
         (units[units[unit2].next_in_stack].y != units[unit2].y)) {
        dbg_logf((const char *)&str_crossed_locations_in_unit_stack_id_d_rep_2,unit2);
        valid = 0;
        units[unit2].next_in_stack = -1;
        if (units[unit3].prev_in_stack == unit2) {
          units[unit3].prev_in_stack = -1;
        }
      }
    }
  }
  else {
    valid = 1;
  }
  return valid;
}

/* 005b29aa auto */
int __cdecl unit_max_hp(iptr unit)

{
  return (int)(char)unit_types[units[unit].type].hit_points;
}

/* 005b29d7 auto */
int __cdecl unit_hp_remaining(iptr unit)

{
  int hp;
  
  if ((game.rules & RULE_NORMAL_COMBAT) == 0) {
    units[unit].damage = '\0';
  }
  hp = unit_max_hp(unit);
  hp -= (uint)units[unit].damage;
  if (hp < 1) {
    hp = 0;
  }
  return hp;
}

/* 005b2a39 auto */
uint __cdecl unit_max_moves(iptr unit)

{
  int civ;
  int has;
  uint result;
  uint min_moves;
  uint moves;
  
  moves = (uint)(char)unit_types[units[unit].type].move;
  if (moves == 0) {
    result = 0;
  }
  else {
    if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
      civ = (int)units[unit].owner;
      has = civ_has_advance(civ,ADV_NUCLEAR_POWER);
      if (has != 0) {
        moves += cosmic.road_move_mult;
      }
      has = civ_has_wonder(civ,WONDER_MAGELLAN);
      if (has != 0) {
        moves += (uint)cosmic.road_move_mult * 2;
      }
      civ = civ_has_wonder(civ,WONDER_LIGHTHOUSE);
      if ((civ != 0) && ((unit_types[units[unit].type].flags & UTYPE_COAST_BOUND) == 0)) {
        moves += cosmic.road_move_mult;
      }
    }
    result = moves;
    if (((units[unit].damage != '\0') && ((game.rules & RULE_NORMAL_COMBAT) != 0)) &&
       (unit_types[units[unit].type].domain != DOMAIN_AIR)) {
      civ = unit_max_hp(unit);
      if (civ < 2) {
        civ = 1;
      }
      has = unit_hp_remaining(unit);
      moves = (int)(has * moves) / civ;
      if ((int)moves % (int)(uint)cosmic.road_move_mult != 0) {
        moves = moves +
                   ((uint)cosmic.road_move_mult - (int)moves % (int)(uint)cosmic.road_move_mult);
      }
      if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
        min_moves = (uint)cosmic.road_move_mult * 2;
      }
      else {
        min_moves = (uint)cosmic.road_move_mult;
      }
      result = min_moves;
      if ((int)min_moves <= (int)moves) {
        result = moves;
      }
    }
  }
  return result;
}

/* 005b2c3d auto */
int __cdecl unit_moves_left(iptr unit)

{
  int moves;
  
  moves = unit_max_moves(unit);
  moves -= (uint)units[unit].moves_used;
  if (moves < 1) {
    moves = 0;
  }
  return moves;
}

/* 005b2c82 auto */
iptr __cdecl unit_next_in_stack(iptr unit)

{
  if (-1 < unit) {
    unit_stack_validate(unit);
    unit = (iptr)units[unit].next_in_stack;
  }
  return unit;
}

/* 005b2cc3 auto */
iptr __cdecl unit_stack_last(iptr unit)

{
  if (-1 < unit) {
    unit_stack_validate(unit);
    for (; (-1 < units[unit].next_in_stack && (units[unit].next_in_stack != unit));
        unit = (iptr)units[unit].next_in_stack) {
    }
  }
  return unit;
}

/* 005b2d39 auto */
iptr __cdecl unit_stack_first(iptr unit)

{
  if (-1 < unit) {
    unit_stack_validate(unit);
    for (; (-1 < units[unit].prev_in_stack && (units[unit].prev_in_stack != unit));
        unit = (iptr)units[unit].prev_in_stack) {
    }
  }
  return unit;
}

/* 005b2daf auto */
i32 __cdecl unit_stack_first_at(int civ,int x,int y)

{
  i32 first;
  iptr unit;
  iptr unit2;
  
  unit2 = -1;
  for (unit = 0; (unit2 < 0 && (unit < game.num_units)); unit++) {
    if ((units[unit].id != 0) &&
       (((units[unit].x == x && (units[unit].y == y)) &&
        (units[unit].owner == civ)))) {
      unit2 = unit;
    }
  }
  first = unit_stack_first(unit2);
  return first;
}

/* 005b2f50 auto */
void __cdecl unit_set_sleep(iptr unit)

{
  if (units[unit].orders != ORDER_SLEEP) {
    units[unit].goto_x = -1;
  }
  units[unit].orders = ORDER_SLEEP;
  return;
}

/* 005b2f92 auto */
int __cdecl unit_stack_nth(int unit,int n)

{
  int found;
  i32 pos;
  
  pos = -1;
  found = -1;
  for (unit = unit_stack_first(unit); (found < 0 && (-1 < unit));
      unit = unit_next_in_stack(unit)) {
    pos++;
    if (pos == n) {
      found = unit;
    }
  }
  return found;
}

/* 005b319e auto */
void __cdecl unit_stack_unlink(iptr unit,int sync)

{
  char is_host;
  iptr start_ticks;
  iptr now_ticks;
  int tile;
  bool has_prev;
  bool has_next;
  Tile *map_tile;
  
  net.unit_stack_unlink_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    has_prev = -1 < units[unit].prev_in_stack;
    if (has_prev) {
      units[units[unit].prev_in_stack].next_in_stack = units[unit].next_in_stack;
    }
    has_next = -1 < units[unit].next_in_stack;
    if (has_next) {
      units[units[unit].next_in_stack].prev_in_stack = units[unit].prev_in_stack;
    }
    units[unit].prev_in_stack = -1;
    units[unit].next_in_stack = -1;
    if ((!has_next && !has_prev) &&
       (tile = map_in_bounds((int)units[unit].x,(int)units[unit].y), tile != 0)) {
      map_tile = tile_at((int)units[unit].x,(int)units[unit].y);
      map_tile->improvements = map_tile->improvements & ~TILE_UNIT;
    }
    units[unit].x = (units[unit].owner * 4 + 4) * -0x19;
    units[unit].y = (units[unit].owner * 4 + 4) * -0x19;
    if ((2 < game.multiplayer) && (sync != 0)) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.unit_stack_unlink_busy = 0;
  }
  else {
    net.unit_stack_unlink_busy = 0;
    net.nm_unit_picked_up_count = -2;
    net_send_message(NM_PICK_UP_UNIT,0,unit,0,0,0,0,0,0,0);
    start_ticks = timer_ticks();
    while ((net.nm_unit_picked_up_count == -2 && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_unit_picked_up_count == -2) {
      dbg_log((const char *)&str_pick_up_unit_connection_to_server_timed_2);
      popup_game_with_sprite((const char *)&str_serverconnecttime_10,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b345f auto */
void __cdecl unit_stack_link(iptr unit,iptr x,iptr y,int sync)

{
  char is_host;
  iptr start_ticks;
  iptr now_ticks;
  int unit2;
  int civ;
  Tile *map_tile;
  
  net.unit_stack_link_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    unit2 = unit_stack_first_at((int)units[unit].owner,x,y);
    units[unit].x = (i16)x;
    units[unit].y = (i16)y;
    units[unit].prev_in_stack = -1;
    units[unit].next_in_stack = (i16)unit2;
    if (unit2 < 0) {
      unit2 = map_in_bounds(x,y);
      if (unit2 != 0) {
        map_tile = tile_at(x,y);
        map_tile->improvements = map_tile->improvements | TILE_UNIT;
        civ = (int)units[unit].owner;
        if ((civ < 0) || (8 < civ)) {
          civ = 0xf;
        }
        map_tile = tile_at(x,y);
        map_tile->owner = map_tile->owner & 0xf;
        map_tile->owner = map_tile->owner | (byte)(civ << 4);
      }
    }
    else {
      units[unit2].prev_in_stack = (i16)unit;
    }
    if ((2 < game.multiplayer) && (sync != 0)) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.unit_stack_link_busy = 0;
  }
  else {
    net.unit_stack_link_busy = 0;
    net.nm_unit_put_down_count = -2;
    net_send_message(NM_PUT_DOWN_UNIT,0,unit,x,y,0,0,0,0,0);
    start_ticks = timer_ticks();
    while ((net.nm_unit_put_down_count == -2 && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_unit_put_down_count == -2) {
      dbg_log((const char *)&str_put_down_unit_connection_to_server_timed);
      popup_game_with_sprite((const char *)&str_serverconnecttime_11,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b36df auto */
void __cdecl unit_relocate(iptr unit,iptr x,iptr y,int sync)

{
  char is_host;
  int start_ticks;
  int now_ticks;
  
  net.unit_relocate_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    unit_stack_unlink(unit,0);
    unit_stack_link(unit,x,y,0);
    if ((2 < game.multiplayer) && (sync != 0)) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.unit_relocate_busy = 0;
  }
  else {
    net.unit_relocate_busy = 0;
    net.nm_unit_relocated_count = -2;
    net_send_message(NM_RELOCATE_UNIT,0,unit,x,y,0,0,0,0,0);
    start_ticks = timer_ticks();
    while ((net.nm_unit_relocated_count == -2 && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_unit_relocated_count == -2) {
      dbg_log((const char *)&str_relocate_unit_connection_to_server_timed);
      popup_game_with_sprite((const char *)&str_serverconnecttime_12,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b3863 auto */
void __cdecl unit_relocate_in_place(iptr unit,i32 sync)

{
  unit_relocate(unit,(int)units[unit].x,(int)units[unit].y,sync);
  return;
}

/* 005b389f auto */
void __cdecl unit_move_to_stack_bottom(iptr unit,int sync)

{
  i16 x;
  i16 y;
  char is_host;
  iptr start_ticks;
  iptr now_ticks;
  int unit2;
  iptr unit3;
  
  if (-1 < units[unit].next_in_stack) {
    net.unit_stack_bottom_busy = 1;
    if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
      if (-1 < units[unit].next_in_stack) {
        x = units[unit].x;
        y = units[unit].y;
        unit3 = unit_stack_first(unit);
        if (unit3 == unit) {
          unit3 = unit_next_in_stack(unit);
        }
        unit_stack_unlink(unit,0);
        unit2 = unit_stack_last(unit3);
        units[unit2].next_in_stack = (i16)unit;
        units[unit].prev_in_stack = (i16)unit2;
        units[unit].next_in_stack = -1;
        units[unit].x = x;
        units[unit].y = y;
      }
      if ((2 < game.multiplayer) && (sync != 0)) {
        net_send_state_diff(0xff,2,0,0,0);
        XD_FlushSendBuffer(5000);
      }
      net.unit_stack_bottom_busy = 0;
    }
    else {
      net.unit_stack_bottom_busy = 0;
      net.nm_bottomed_count = -2;
      net_send_message(NM_MOVE_TO_BOTTOM,0,unit,0,0,0,0,0,0,0);
      start_ticks = timer_ticks();
      while ((net.nm_bottomed_count == -2 && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
        net.process_messages(1,1);
      }
      if (net.nm_bottomed_count == -2) {
        dbg_log((const char *)&str_move_to_bottom_connection_to_server_time);
        popup_game_with_sprite((const char *)&str_serverconnecttime_13,(iptr)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
      }
      while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
    }
  }
  return;
}

/* 005b3ae0 auto */
void __cdecl unit_stack_relocate_all(iptr unit,iptr x,iptr y,int sync)

{
  int next;
  int unit2;
  
  unit2 = unit_stack_first(unit);
  while (-1 < unit2) {
    next = unit_next_in_stack(unit2);
    unit_relocate(unit2,x,y,0);
    unit2 = next;
  }
  if ((2 < game.multiplayer) && (sync != 0)) {
    net_send_state_diff(0xff,2,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  return;
}

/* 005b3b78 auto */
void __cdecl unit_stack_ships_to_top(iptr unit,i32 sync)

{
  short x;
  short y;
  iptr first;
  iptr next_unit;
  iptr cur;
  iptr unit2;
  
  unit2 = -1;
  if (-1 < unit) {
    x = units[unit].x;
    y = units[unit].y;
    first = unit_stack_first(unit);
    cur = first;
    while (unit = cur, -1 < unit) {
      next_unit = unit_next_in_stack(unit);
      cur = next_unit;
      if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
        unit2 = unit;
        unit_relocate(unit,(units[unit].owner * 4 + 4) * -0x4b,
                           (units[unit].owner * 4 + 4) * -0x4b,sync);
        cur = next_unit;
      }
    }
    if (-1 < unit2) {
      unit = unit_stack_first(unit2);
      while (-1 < unit) {
        cur = unit_next_in_stack(unit);
        unit_relocate(unit,(int)x,(int)y,sync);
        unit = cur;
      }
    }
  }
  return;
}

/* 005b3d06 auto */
iptr __cdecl unit_create(iptr unit_type,uptr civ,iptr x,iptr y)

{
  char *type_count;
  char is_host;
  iptr start_ticks;
  iptr now_ticks;
  i32 max_moves;
  int city;
  iptr unit;
  i8 home;
  
  net.unit_create_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    for (unit = 0; (unit < game.num_units && (units[unit].id != 0));
        unit = unit + 1) {
    }
    if (game.num_units == unit) {
      if (((0x7ff < game.num_units) || (0x79c < civs[civ].num_military_units)) ||
         (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 && (0x7f7 < game.num_units)
          ))) {
        if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
           (game.multiplayer < 3)) {
          popup_game_with_sprite((const char *)&str_toomanyunits,(iptr)&popup_info_icon_sprite,0);
        }
        net.unit_create_busy = 0;
        return 0xffffffff;
      }
      game.num_units++;
    }
    if ((char)unit_types[unit_type].role < 5) {
      civs[civ].num_military_units++;
    }
    type_count = (char *)(unit_type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
    *type_count = *type_count + 1;
    civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
    units[unit].type = (u8)unit_type;
    units[unit].owner = (byte)civ;
    units[unit].id = next_ids.unit;
    next_ids.unit++;
    if (game.multiplayer < 3) {
      units[unit].moves_used = '\0';
    }
    else if (net.mp_simultaneous_moves == '\0') {
      units[unit].moves_used = '\0';
    }
    else {
      max_moves = unit_max_moves(unit);
      units[unit].moves_used = (u8)max_moves;
    }
    units[unit].damage = '\0';
    units[unit].ai_task = 'X';
    units[unit].flags = 0;
    units[unit].visibility = '\0';
    units[unit].orders = ORDER_NONE;
    units[unit].home_city = -1;
    city = city_find_nearest(x,y,-1,0xffffffff,-1);
    if (((civ != 0) && (-1 < city)) && ((int)cities[city].owner == (civ & 0xff))) {
      home = (i8)city;
      units[unit].home_city = home;
    }
    units[unit].counter = '\0';
    units[unit].aux_timer = '\0';
    units[unit].last_dir = 0xff;
    units[unit].prev_in_stack = -1;
    units[unit].next_in_stack = -1;
    units[unit].x = -1;
    units[unit].y = -1;
    units[unit].goto_x = -1;
    units[unit].goto_y = -1;
    unit_stack_link(unit,x,y,0);
    unit_reveal_surroundings(unit,1);
    if (((game.turn != 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
       (game.multiplayer == '\0')) {
      if (unit_types[unit_type].domain == DOMAIN_SEA) {
        if ((game.shown_event_flags & HINT_SHIPS) == 0) {
          if (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) {
            popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_ships,unit);
          }
          game.shown_event_flags = game.shown_event_flags | HINT_SHIPS;
        }
      }
      else if (unit_types[unit_type].domain == DOMAIN_AIR) {
        if ((game.shown_event_flags & HINT_AIR_UNIT) == 0) {
          if (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) {
            popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_airunit,unit);
          }
          game.shown_event_flags = game.shown_event_flags | HINT_AIR_UNIT;
        }
      }
      else if (unit_types[unit_type].role == ROLE_TRADE) {
        if ((game.shown_event_flags & HINT_CARAVAN) == 0) {
          if (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) {
            popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_caravan_7,unit);
          }
          game.shown_event_flags = game.shown_event_flags | HINT_CARAVAN;
        }
      }
      else if ((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && ((char)unit_types[unit_type].role < 5)) {
        if (civs[civ].num_military_units == 1) {
          popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_firstunit1,unit);
        }
        if (civs[civ].num_military_units == 2) {
          popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_firstunit2,unit);
        }
      }
    }
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.unit_create_busy = 0;
  }
  else {
    net.unit_create_busy = 0;
    (*(int *)&net.nm_unit_created_result) = -2;
    net_send_message(NM_CREATE_UNIT,0,unit_type,civ,x,y,0,0,0,0);
    start_ticks = timer_ticks();
    while (((*(int *)&net.nm_unit_created_result) == -2 && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
      net.process_messages(1,1);
    }
    if ((*(int *)&net.nm_unit_created_result) == -2) {
      dbg_log((const char *)&str_create_unit_connection_to_server_timed_o);
      popup_game_with_sprite((const char *)&str_serverconnecttime_14,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (unit = (*(int *)&net.nm_unit_created_result), net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return unit;
}

/* 005b4391 auto */
void __cdecl unit_delete(iptr unit,int update_ui)

{
  char *type_count;
  byte unit_type;
  short x;
  short y;
  char is_host;
  iptr start_ticks;
  iptr now_ticks;
  int civ;
  uint home;
  iptr unit2;
  
  if ((-1 < unit) && (units[unit].id != 0)) {
    net.unit_delete_busy = 1;
    if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
      x = units[unit].x;
      y = units[unit].y;
      unit_type = units[unit].type;
      if (units[unit].home_city == -1) {
        home = 0xffffffff;
      }
      else {
        home = (uint)(byte)units[unit].home_city;
      }
      civ = (int)units[unit].owner;
      if ((-1 < civ) && (unit < 0x800)) {
        if ((civs[civ].num_military_units != 0) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          civs[civ].num_military_units--;
        }
        if (*(char *)&civs[civ].units_active[units[unit].type] != '\0') {
          type_count = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
          *type_count = *type_count - 1;
        }
        if (civ_unit_counts_block.civ_unit_totals[civ] != 0) {
          civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] - 1;
        }
      }
      unit_stack_unlink(unit,0);
      units[unit].id = 0;
      if (game.num_units - 1 == unit) {
        game.num_units--;
      }
      for (unit2 = 0; unit2 < game.num_units; unit2++) {
        if ((units[unit2].orders == ORDER_SLEEP) && (units[unit2].goto_x == unit)) {
          units[unit2].orders = ORDER_NONE;
        }
      }
      if ((update_ui != 0) &&
         (((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(-1,(int)x,(int)y), -1 < (int)home))
      {
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(home);
      }
      if (((unit_types[unit_type].role == ROLE_SETTLE) && (civs[civ].num_cities == 0)) &&
         (*(char *)&civs[civ].units_active[unit_type] == '\0')) {
        civ_check_destroyed(civ,-1);
      }
      if ((2 < game.multiplayer) && (update_ui != 0)) {
        net_send_state_diff(0xff,2,0,0,0);
        net_send_message(NM_CITYWIN_CHECK_UNIT,0xff,-1,(int)x,(int)y,0,0,0,0,0);
        if (0 < (int)home) {
          net_send_message(NM_CITYWIN_CHECK_CITY,0xff,home,0,0,0,0,0,0,0);
        }
        XD_FlushSendBuffer(5000);
      }
      net.unit_delete_busy = 0;
    }
    else {
      net.unit_delete_busy = 0;
      net.unit_deleted_ack = -2;
      net_send_message(NM_DELETE_UNIT,0,unit,0,0,0,0,0,0,0);
      start_ticks = timer_ticks();
      while ((net.unit_deleted_ack == -2 && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
        net.process_messages(1,1);
      }
      if (net.unit_deleted_ack == -2) {
        dbg_log((const char *)&str_delete_unit_connection_to_server_timed_o);
        popup_game_with_sprite((const char *)&str_serverconnecttime_15,(iptr)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
      }
      while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
    }
  }
  return;
}

/* 005b47fa auto */
void __cdecl unit_stack_delete_all(iptr unit,int sync)

{
  iptr next;
  
  unit = unit_stack_first(unit);
  while (-1 < unit) {
    next = unit_next_in_stack(unit);
    unit_delete(unit,0);
    unit = next;
  }
  if ((2 < game.multiplayer) && (sync != 0)) {
    net_send_state_diff(0xff,2,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  return;
}

/* 005b488a auto */
void __cdecl unit_clear_visibility(iptr unit)

{
  if (-1 < unit) {
    units[unit].visibility = '\0';
  }
  return;
}

/* 005b48b1 auto */
void __cdecl unit_stack_clear_visibility(iptr unit)

{
  int unit2;
  
  for (unit2 = unit_stack_first(unit); -1 < unit2; unit2 = unit_next_in_stack(unit2)) {
    unit_clear_visibility(unit2);
  }
  return;
}

/* 005b490e auto */
void __cdecl unit_set_seen_by(iptr unit,iptr civ)

{
  if (((-1 < civ) && (units[unit].owner != civ)) && (-1 < unit)) {
    units[unit].visibility = units[unit].visibility | (byte)(1 << ((byte)civ & 0x1f));
  }
  return;
}

/* 005b496e auto */
void __cdecl unit_stack_set_seen_by(iptr unit,iptr civ)

{
  int unit2;
  
  for (unit2 = unit_stack_first(unit); -1 < unit2; unit2 = unit_next_in_stack(unit2)) {
    unit_set_seen_by(unit2,civ);
  }
  return;
}

/* 005b4ee2 auto */
void __cdecl unit_stack_add_visibility(iptr unit,i32 vis_mask)

{
  for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit)) {
    units[unit].visibility = units[unit].visibility | (byte)vis_mask;
  }
  return;
}

/* 005b50ad auto */
int __cdecl unit_stack_sum(iptr unit,int mode)

{
  int sum;
  
  sum = 0;
  if (mode != 0xb) {
    unit = unit_stack_first(unit);
  }
  while ((-1 < unit && (sum < 0x800))) {
    switch(mode) {
    case 0:
      sum += (char)unit_types[units[unit].type].cost;
      break;
    case 1:
      sum += (char)unit_types[units[unit].type].defense;
      break;
    case 2:
    case 0xb:
      sum++;
      break;
    case 3:
      sum += (char)unit_types[units[unit].type].attack;
      break;
    case 4:
      if ((-1 < units[unit].prev_in_stack) && (unit_types[units[unit].type].role == ROLE_DEFEND)) {
        sum++;
      }
      break;
    case 5:
      if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
        sum++;
      }
      break;
    case 6:
      if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
        sum += (char)unit_types[units[unit].type].hold;
      }
      else if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
        sum--;
      }
      break;
    case 7:
      if ((unit_types[units[unit].type].domain == DOMAIN_AIR) &&
         (1 < (char)unit_types[units[unit].type].range)) {
        sum++;
      }
      break;
    case 8:
      if ((unit_types[units[unit].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) != 0) {
        sum++;
      }
      break;
    case 9:
      if ((unit_types[units[unit].type].flags & UTYPE_CARRY_AIR) != 0) {
        sum++;
      }
      break;
    case 10:
      if ((unit_types[units[unit].type].flags & UTYPE_SUBMARINE) != 0) {
        sum++;
      }
    }
    if (mode == 0xb) {
      unit = (iptr)units[unit].next_in_stack;
    }
    else {
      unit = unit_next_in_stack(unit);
    }
  }
  return sum;
}

/* 005b53b6 auto */
int __cdecl unit_stack_count_role(iptr unit,int wanted_role)

{
  int count;
  
  count = 0;
  for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit)) {
    if ((char)unit_types[units[unit].type].role == wanted_role) {
      count++;
    }
  }
  return count;
}

/* Part of unit_gather_cargo (005b542e): whether this unit goes aboard in this
   pass (first those bound for the ship, then the AI's and the idle). */
static void unit_gather_cargo_take_aboard(iptr unit,iptr unit2,int pass,uint is_carrier,
                                                     bool is_missile_sub,int in_bounds,int on_ocean,int x,
                                                     int y,bool &take_aboard)
{
  char civ;
  iptr landmass;
  if (pass == 0) {
    if ((is_carrier != 0) || (is_missile_sub)) {
      take_aboard = true;
    }
    else if ((units[unit2].orders == ORDER_SLEEP) && (units[unit2].goto_x == unit)) {
      take_aboard = true;
    }
  }
  else if (pass == 1) {
    if (((uint)game.human_players & 1 << (units[unit2].owner & 0x1fU)) == 0) {
      if (((units[unit2].orders != ORDER_FORTIFYING) && (units[unit2].orders != ORDER_FORTIFIED)) ||
         (on_ocean != 0)) {
        if ((in_bounds != 0) && (on_ocean == 0)) {
          civ = units[unit2].owner;
          landmass = tile_landmass(x,y);
          if ((*(char *)&civs[civ].continent_policy[landmass] != 1) ||
             (4 < (char)unit_types[units[unit2].type].role)) {
            take_aboard = true;
          }
        }
        else {
          take_aboard = true;
        }
      }
    }
    else if (units[unit2].orders == ORDER_SLEEP) {
      take_aboard = units[unit2].goto_x < 0;
    }
    else if (on_ocean != 0) {
      take_aboard = true;
    }
  }
}

/* Part of unit_gather_cargo (005b542e): a multiplayer client asks the host to
   do it and waits for the result (capacity2: what is left aboard; the
   original reads it as it is when no message is pending). */
static void unit_gather_cargo_via_host(iptr unit,iptr lift_off,int &capacity2)
{
  int x;
  int y;
    net.unit_gather_cargo_busy = 0;
    (*(int *)&net.nm_ship_stacked_result) = -2;
    net_send_message(NM_STACK_SHIP,0,unit,lift_off,0,0,0,0,0,0);
    x = timer_ticks();
    while (((*(int *)&net.nm_ship_stacked_result) == -2 && (y = timer_ticks(), y - x < 0xe10))) {
      net.process_messages(1,1);
    }
    if ((*(int *)&net.nm_ship_stacked_result) == -2) {
      dbg_log((const char *)&str_stack_ship_connection_to_server_timed_ou);
      popup_game_with_sprite((const char *)&str_serverconnecttime_16,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (capacity2 = (*(int *)&net.nm_ship_stacked_result), net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
}

/* 005b542e auto */
int __cdecl unit_gather_cargo(iptr unit,iptr lift_off,int send_update)

{
  bool is_missile_sub;
  u8 cargo_domain;
  char civ;
  int x;
  int y;
  int in_bounds;
  uint is_carrier;
  iptr next_unit;
  bool take_aboard;
  iptr cur_unit;
  int num_passes;
  iptr unit2;
  iptr first_unit;
  int capacity2;
  int pass;
  int on_ocean;
  
  on_ocean = 0;
  if ((unit < -1) || (0x801 < unit)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_ship_1_ship_max_units_2,(iptr)&str_d_ss_franklinton_unit_cpp_2,0x61d);
  }
  net.unit_gather_cargo_busy = 1;
  if ((game.multiplayer < 3) || (civ = net.is_host(), civ != '\0')) {
    if (lift_off != 0) {
      unit_stack_ships_to_top(unit,0);
    }
    first_unit = unit_stack_first(unit);
    if (first_unit == unit) {
      first_unit = unit_next_in_stack(unit);
    }
    x = (int)units[unit].x;
    y = (int)units[unit].y;
    in_bounds = map_in_bounds(x,y);
    if (in_bounds != 0) {
      on_ocean = tile_is_ocean(x,y);
    }
    if (lift_off == 0) {
      for (unit2 = first_unit; -1 < unit2; unit2 = unit_next_in_stack(unit2)) {
        units[unit2].flags = units[unit2].flags & ~UNITF_ABOARD;
      }
    }
    else {
      unit_relocate(unit,(units[unit].owner * 5 + 5) * -0x28,
                         (units[unit].owner * 5 + 5) * -0x28,0);
    }
    capacity2 = (int)(char)unit_types[units[unit].type].hold;
    is_carrier = unit_types[units[unit].type].flags & UTYPE_CARRY_AIR;
    if (((capacity2 == 0) && (is_carrier == 0)) && ((unit_types[units[unit].type].flags & UTYPE_SUBMARINE) != 0)) {
      is_missile_sub = true;
    }
    else {
      is_missile_sub = false;
    }
    if ((is_carrier != 0) || (is_missile_sub)) {
      capacity2 = 0x14;
    }
    if (units[unit].owner == '\0') {
      capacity2 = 0x14;
    }
    pass = 0;
    while( true ) {
      if ((is_carrier != 0) || (is_missile_sub)) {
        num_passes = 1;
      }
      else {
        num_passes = 2;
      }
      if ((num_passes <= pass) || (unit2 = first_unit, cur_unit = unit2, capacity2 == 0)) break;
      for (;;) {
        unit2 = cur_unit;
        if ((unit2 < 0) || (capacity2 == 0)) break;
        next_unit = unit_next_in_stack(unit2);
        take_aboard = false;
        if ((is_carrier != 0) || (is_missile_sub)) {
          cargo_domain = 1;
        }
        else {
          cargo_domain = '\0';
        }
        if (unit_types[units[unit2].type].domain == cargo_domain) {
          if ((is_missile_sub) &&
             (cur_unit = next_unit, (unit_types[units[unit2].type].flags & UTYPE_DESTROYED_AFTER_ATTACK) == 0)) continue;
          unit_gather_cargo_take_aboard(unit,unit2,pass,is_carrier,is_missile_sub,in_bounds,on_ocean,x,y,
                                        take_aboard);
        }
        if ((lift_off == 0) && ((units[unit2].flags & UNITF_ABOARD) != 0)) {
          take_aboard = false;
        }
        cur_unit = next_unit;
        if (take_aboard) {
          if (first_unit == unit2) {
            first_unit = next_unit;
          }
          if ((is_carrier != 0) || (is_missile_sub)) {
            if (units[unit2].orders != ORDER_SLEEP) {
              units[unit2].orders = ORDER_NONE;
            }
          }
          else {
            unit_set_sleep(unit2);
            units[unit2].goto_x = (i16)unit;
          }
          if (lift_off == 0) {
            units[unit2].flags = units[unit2].flags | UNITF_ABOARD;
          }
          else {
            unit_relocate(unit2,(units[unit2].owner * 5 + 5) * -0x28,
                               (units[unit2].owner * 5 + 5) * -0x28,0);
          }
          capacity2--;
          cur_unit = next_unit;
        }
      }
      pass++;
    }
    if ((2 < game.multiplayer) && (send_update != 0)) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    net.unit_gather_cargo_busy = 0;
  }
  else {
    unit_gather_cargo_via_host(unit,lift_off,capacity2);
  }
  return capacity2;
}

/* 005b5bab auto */
void __cdecl unit_lift_off_map(iptr unit,int send_update)

{
  char is_host;
  int start_ticks;
  int now_ticks;
  
  if (-1 < unit) {
    net.unit_lift_off_map_busy = 1;
    if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
      if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
        unit_gather_cargo(unit,1,0);
      }
      else {
        unit_relocate(unit,(units[unit].owner * 5 + 5) * -0x28,
                           (units[unit].owner * 5 + 5) * -0x28,0);
      }
      if ((2 < game.multiplayer) && (send_update != 0)) {
        net_send_state_diff(0xff,2,0,0,0);
        XD_FlushSendBuffer(5000);
      }
      net.unit_lift_off_map_busy = 0;
    }
    else {
      net.unit_lift_off_map_busy = 0;
      net.nm_unit_stacked_count = -2;
      net_send_message(NM_STACK_UNIT,0,unit,0,0,0,0,0,0,0);
      start_ticks = timer_ticks();
      while ((net.nm_unit_stacked_count == -2 && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
        net.process_messages(1,1);
      }
      if (net.nm_unit_stacked_count == -2) {
        dbg_log((const char *)&str_stack_unit_connection_to_server_timed_ou);
        popup_game_with_sprite((const char *)&str_serverconnecttime_17,(iptr)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
      }
      while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
    }
  }
  return;
}

/* 005b5d93 auto */
void __cdecl unit_delete_with_cargo(iptr unit,int send_update)

{
  char is_host;
  int result;
  int now_ticks;
  bool at_sea;
  bool deleted;
  iptr y2;
  iptr x2;
  
  at_sea = false;
  deleted = false;
  net.unit_delete_cargo_busy = 1;
  if ((2 < game.multiplayer) && (is_host = net.is_host(), is_host == '\0')) {
    net.unit_delete_cargo_busy = 0;
    net.nm_safely_deleted_count = -2;
    net_send_message(NM_DELETE_SAFELY,0,unit,0,0,0,0,0,0,0);
    result = timer_ticks();
    while ((net.nm_safely_deleted_count == -2 && (now_ticks = timer_ticks(), now_ticks - result < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_safely_deleted_count == -2) {
      dbg_log((const char *)&str_delete_safely_connection_to_server_timed);
      popup_game_with_sprite((const char *)&str_serverconnecttime_18,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
    return;
  }
  x2 = (iptr)units[unit].x;
  y2 = (iptr)units[unit].y;
  if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
    x2 = (iptr)units[unit].x;
    y2 = (iptr)units[unit].y;
    result = map_in_bounds(x2,y2);
    if (result == 0) {
      if (-x2 == (units[unit].owner * 5 + 5) * 0x28) {
        unit_stack_delete_all(unit,0);
        deleted = true;
      }
      else {
        at_sea = true;
      }
    }
    else {
      result = tile_is_ocean(x2,y2);
      at_sea = result != 0;
    }
  }
  if (at_sea) {
    unit_gather_cargo(unit,0,0);
    unit_stack_delete_all(unit,0);
    ((CityWindow *)((i8 *)&city_wnd.window))->on_unit_change(-1,x2,y2);
  }
  else if (!deleted) {
    unit_delete(unit,0);
  }
  if ((2 < game.multiplayer) && (send_update != 0)) {
    net_send_state_diff(0xff,2,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  net.unit_delete_cargo_busy = 0;
  return;
}

/* 005b6042 auto */
void __cdecl unit_destroy_redraw(iptr unit,int send_update)

{
  char is_host;
  int x;
  int y;
  int in_bounds;
  
  net.unit_destroy_redraw_busy = 1;
  if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
    x = (int)units[unit].x;
    y = (int)units[unit].y;
    unit_delete_with_cargo(unit,0);
    in_bounds = map_in_bounds(x,y);
    if (in_bounds != 0) {
      map_redraw_tile_all_views(x,y);
    }
    if ((2 < game.multiplayer) && (send_update != 0)) {
      net_send_state_diff(0xff,2,0,0,0);
      net_send_message(NM_DRAW_SQUARE,0xff,x,y,0,0,0,0,0,0);
    }
    net.unit_destroy_redraw_busy = 0;
  }
  else {
    net.unit_destroy_redraw_busy = 0;
    net.nm_visibly_deleted_count = -2;
    net_send_message(NM_DELETE_VISIBLY,0,unit,0,0,0,0,0,0,0);
    x = timer_ticks();
    while ((net.nm_visibly_deleted_count == -2 && (y = timer_ticks(), y - x < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.nm_visibly_deleted_count == -2) {
      dbg_log((const char *)&str_delete_visible_connection_to_server_time);
      popup_game_with_sprite((const char *)&str_serverconnecttime_19,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b633f auto */
i32 __cdecl unit_is_selectable(iptr unit)

{
  int result;
  i32 selectable;
  
  selectable = 0;
  if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
     (result = map_in_bounds((int)units[unit].x,(int)units[unit].y), result != 0)) {
    if ((((int)units[unit].owner == (uint)(byte)game.turn_civ) &&
        (units[unit].orders != ORDER_SLEEP)) &&
       ((units[unit].orders != ORDER_FORTIFIED &&
        ((result = unit_moves_left(unit), result != 0 && ((units[unit].flags & UNITF_SPY_ON_MISSION) == 0))))))
    {
      selectable = 1;
    }
    else {
      selectable = 0;
    }
  }
  return selectable;
}

/* 005b6458 auto */
i32 __cdecl unit_is_awake_with_moves(iptr unit)

{
  int moves_left;
  i32 awake;
  
  awake = 0;
  if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
     (-1 < units[unit].x)) {
    if ((units[unit].orders == ORDER_SLEEP) || (moves_left = unit_moves_left(unit), moves_left == 0)) {
      awake = 0;
    }
    else {
      awake = 1;
    }
  }
  return awake;
}

/* 005b6512 auto */
iptr __cdecl unit_find_nearest_selectable(iptr unit,int human_only)

{
  int result;
  iptr best_unit;
  int y2;
  int x2;
  int pass;
  int dist;
  int prev_active;
  int best_dist;
  
  best_dist = 9999;
  best_unit = -1;
  prev_active = -1;
  x2 = (int)map_cursor.x[0];
  y2 = (int)map_cursor.y[0];
  if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
     (game.turn_civ != '\0')) {
    x2 = (int)units[unit].x;
    y2 = (int)units[unit].y;
    prev_active = (int)game.active_unit;
    if (extra_rule(X_ROTATION)) {
      extras_rotation_origin(unit,&x2,&y2);  /* X19: from where the last unit started */
    }
  }
  pass = 0;
  while( true ) {
    if (2 < pass) {
      return best_unit;
    }
    for (unit = 0; unit < game.num_units; unit++) {
      if (((units[unit].id != 0) && (result = unit_is_selectable(unit), result != 0)) &&
         (((units[unit].flags & UNITF_WAITING) == 0 &&
          ((human_only != 1 || (units[unit].owner == human_civ[0])))))) {
        result = map_distance_diag(x2,y2,(int)units[unit].x,(int)units[unit].y);
        dist = result * 2 + 1;
        if (unit_types[units[unit].type].domain != DOMAIN_SEA) {
          dist = result * 2;
        }
        if (dist < best_dist) {
          best_dist = dist;
          best_unit = unit;
        }
        else if ((dist == best_dist) && (game.active_unit == unit)) {
          best_unit = unit;
        }
      }
    }
    if (-1 < best_unit) break;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) && ((pass != 0 || (unit != prev_active)))) {
        units[unit].flags = units[unit].flags & ~UNITF_WAITING;
      }
    }
    pass++;
  }
  if (extra_rule(X_ROTATION)) {
    extras_rotation_found(best_unit);  /* X19 */
  }
  return best_unit;
}

/* 005b6787 auto */
void __cdecl unit_exhaust_moves(iptr unit)

{
  i32 max_moves;
  
  max_moves = unit_max_moves(unit);
  units[unit].moves_used = (u8)max_moves;
  return;
}

/* 005b67af auto */
iptr __cdecl unit_find_nearest(i32 target_x,i32 target_y,iptr owner_civ,iptr exclude_unit)

{
  int dist;
  iptr unit;
  iptr nearest;
  
  nearest = -1;
  dialog_text_font_obj_block.unit_nearest_dist = 9999;
  for (unit = 0; unit < game.num_units; unit++) {
    if (((units[unit].id != 0) &&
        (((owner_civ < 0 || ((int)units[unit].owner == (owner_civ & 0xffU))) &&
         (unit != exclude_unit)))) &&
       (dist = map_distance_diag(target_x,target_y,(int)units[unit].x,(int)units[unit].y),
       dist <= dialog_text_font_obj_block.unit_nearest_dist)) {
      nearest = unit;
      dialog_text_font_obj_block.unit_nearest_dist = dist;
    }
  }
  return nearest;
}

/* 005b6898 auto */
int __cdecl unit_home_city_name(iptr unit)

{
  int name2;
  
  if (units[unit].home_city == -1) {
    name2 = (iptr)label_string(label_table.string_ids[0xe]);
  }
  else {
    name2 = (uint)(byte)units[unit].home_city * 0x58 + (iptr)((u8 *)&cities + 32);
  }
  return name2;
}

/* 005b6a58 auto */
i32 __cdecl unit_dlg_wake_unit_cb(iptr unit)

{
  if (-1 < unit) {
    units[unit].orders = ORDER_NONE;
    units[unit].flags = units[unit].flags & ~UNITF_AUTOMATED;
  }
  return 0;
}

/* 005b6aa0 auto */
i32 __cdecl unit_dlg_dblclick_accept_cb(void)

{
  return 1;
}

/* 005b6ab5 auto */
void __cdecl
unit_dlg_draw_sprite_cb(i32 dlg,iptr sprite,iptr unit,i32 left,int x,
            i32 y)

{
  gfx_draw_unit(sprite,unit,4,x + 2,y,dlg_unit_sprite_zoom,0);
  return;
}

/* Part of unit_stack_select_dialog (005b6aea): a unit's line: its owner,
   veteran, type, home city (and a caravan's commodity). */
static void unit_stack_select_text(iptr unit)
{
  iptr adjective;
  uint city;

  text_clear();
  adjective = civ_adjective((int)units[unit].owner);
  text_append_str((const char *)adjective);
  text_append_space();
  if ((units[unit].flags & UNITF_VETERAN) != 0) {
    text_append_label(0xd);
    text_append_space();
  }
  text_append_string_id(unit_types[units[unit].type].name);
  text_append_space();
  text_append_lparen();
  if (units[unit].home_city == -1) {
    city = 0xffffffff;
  }
  else {
    city = (uint)(byte)units[unit].home_city;
  }
  text_append_city_name(city);
  if (unit_types[units[unit].type].role == ROLE_TRADE) {
    text_append_comma();
    if ((char)units[unit].counter < '\0') {
      text_append_string_id(label_table.string_ids[0xc0]);
    }
    else {
      text_append_string_id(commodity_names[(char)units[unit].counter]);
    }
  }
  text_append_rparen();
}

/* 005b6aea auto */
i32 __cdecl unit_stack_select_dialog(iptr unit,const char *title,int allow_cancel)

{
  i32 choice;
  int count;
  Dialog dialog2;
  Sprite sprite;
  i8 *seh_scope;
  
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (extra_ui(X_UNIT_LIST) && unit_stack_sum(unit,2) > 9) {
    /* X2, X28: more than 9 units: all of them, in a list that scrolls */
    dialog2.Dialog::reset(allow_cancel != 0);
    dialog2.set_title(title);
    dialog2.set_width(0x154);
    dialog2.set_option_draw_fn((DialogOptionDrawFn)extras_unit_list_draw);
    dialog2.set_list_indent(0x2c,0);
    for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit)) {
      unit_stack_select_text(unit);
      dialog2.add_option((const char *)&textbuf.buf,unit,0);
    }
    dialog2.set_listbox(9,0x14c,0);
    return dialog2.show_modal(0);
  }
  sprite.create_solid(0x42,0x30,0);
  dlg_unit_sprite_zoom = 0xffffffff;
  dialog2.Dialog::reset(0x20001 - (uint)(allow_cancel == 0));
  dialog2.set_title(title);
  dialog2.set_width(0x154);
  dialog2.set_sprite_click_fn((DialogSpriteFn)unit_dlg_wake_unit_cb);
  dialog2.set_sprite_dblclick_fn((DialogSpriteFn)unit_dlg_dblclick_accept_cb);
  dialog2.set_sprite_draw_fn((DialogSpriteDrawFn)unit_dlg_draw_sprite_cb);
  dialog2.set_image_zoom(0xffffffff);
  count = 0;
  for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit)) {
    count++;
    if (count < 10) {
      unit_stack_select_text(unit);
      dialog2.add_sprite((iptr)&sprite,unit,(const char *)&textbuf.buf);
    }
  }
  choice = dialog2.show_modal(0);
  return choice;
}

/* 005b9179 auto */
void __cdecl nuclear_blast(iptr center_x,int center_y)

{
  int y;
  iptr x;
  iptr in_bounds;
  iptr city;
  int tile;
  i32 improvements2;
  uint rnd;
  uint sign;
  int civ;
  iptr dir;
  Tile *map_tile;
  
  y = map_in_bounds(center_x,center_y);
  if (y != 0) {
    net_batch_begin();
    for (dir = 0; dir < 9; dir++) {
      x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + center_x);
      y = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + center_y;
      in_bounds = map_in_bounds(x,y);
      if (in_bounds != 0) {
        city = city_at(x,y);
        if (city < 0) {
          tile = tile_is_ocean(x,y);
          if (tile == 0) {
            map_tile = tile_at(x,y);
            improvements2 = tile_improvements(x,y);
            if (((byte)improvements2 & TILE_AIRBASE) == 0x40) {
              tile_set_improvements(x,y,0x40,0,1);
            }
            rnd = _rand();
            sign = (int)rnd >> 0x1f;
            if (((rnd ^ sign) - sign & 1 ^ sign) != sign) {
              tile_set_improvements(x,y,0x20,0,1);
            }
            if (((map_tile->improvements & TILE_MINE) == 0) || ((map_tile->improvements & TILE_IRRIGATION) == 0)) {
              rnd = _rand();
              sign = (int)rnd >> 0x1f;
              if (((rnd ^ sign) - sign & 1 ^ sign) != sign) {
                tile_set_improvements(x,y,8,0,1);
              }
              rnd = _rand();
              sign = (int)rnd >> 0x1f;
              if (((rnd ^ sign) - sign & 1 ^ sign) != sign) {
                tile_set_improvements(x,y,4,0,1);
              }
            }
            else {
              rnd = _rand();
              sign = (int)rnd >> 0x1f;
              if (((rnd ^ sign) - sign & 1 ^ sign) != sign) {
                tile_set_improvements(x,y,8,0,1);
              }
            }
            for (civ = 1; civ < 8; civ++) {
              tile_update_known(x,y,civ);
            }
            tile = _rand();
            if (tile % 3 != 0) {
              tile_add_pollution(x,y);
            }
            map_redraw_tile_all_views(x,y);
          }
        }
        else {
          cities[city].size -= (cities[city].size >> 1);
          map_redraw_area_all_views(x,y,0,human_civ[0],1);
        }
      }
    }
    net_batch_flush();
  }
  return;
}

/* 005badf0 auto */
char * __cdecl path_join_upper(char *dest,iptr dir,const char *filename)

{
  char *end;
  char buf[80];
  
  crt_strcpy(buf,(const char *)dir);
  for (end = buf; *end != '\0'; end++) {
  }
  if (end[-1] != '\\') {
    crt_strcat(buf,((i8 *)&path_separator_str));
  }
  crt_strcpy(dest,buf);
  crt_strcat(dest,filename);
  __strupr(dest);
  return dest;
}
