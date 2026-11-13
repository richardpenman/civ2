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
void __cdecl unit_cancel_goto_on_contact(int unit)

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
void __cdecl unit_stack_wake_sentries(int unit)

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
  Tile *tile2;
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
      tile2 = tile_at(x,y4);
      if (civ2 != 0) {
        known = (char *)known_at(x,y4,civ2);
        if (((is_human) && (!map_revealed)) &&
           (((char)tile2->improvements != *known || ((civ_bit & tile2->visible) == 0)))) {
          if (dir < 8) {
            redraw_flags = redraw_flags | 5;
          }
          else {
            redraw_flags = redraw_flags | 10;
          }
        }
        known_set(x,y4,tile2->improvements,
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
void __cdecl unit_reveal_surroundings(int unit,int allow_contact)

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
  revenue = tile_landmass(cities[city].x,cities[city].y);
  other_value = tile_landmass(cities[city2].x,cities[city2].y);
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
void __cdecl unit_caravan_establish_trade(int unit,uptr city)

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
     (city2 = city_find_nearest(units[unit].x,units[unit].y,civ,0xffffffff,-1)
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
i32 __cdecl unit_make_dummy(int unit_type,int owner)

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
void __cdecl units_reset_for_turn(int civ)

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
       (((tile_set_improvements(units[unit].x,units[unit].y,1,1,1), civ == -1 ||
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
      mapviews_ensure_visible((int)units[unit].x,(int)units[unit].y,units[unit].owner);
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
        map_diag_tile_by_ocean(waypoint_x,waypoint_y,&waypoint_x,&waypoint_y,is_sea);
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
    nx = map_diag_tile_by_ocean(cx * 4 + 1,cy * 4 + 1,&x2,&y2,is_sea);
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
           ((ny = map_diag_tile_by_ocean(x2,y2,&x2,&y2,is_sea),
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
uptr __cdecl unit_goto_next_dir(int unit)

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
      result = civ_has_advance(terrain2,ADV_RAILROAD);
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
          result = civ_has_advance(terrain2,ADV_REFRIGERATION);
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
      tile_set_terrain(x,y,terrain_types[terrain].improve_to[order - 6],1);
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
    tile_set_terrain(x,y,terrain_types[terrain].transform_to,1);
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
void __cdecl unit_do_terrain_work(int unit,int order)

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
    tile_update_known(x,y,units[unit2].owner);
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
void __cdecl unit_set_fortified(int unit)

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
    city = city_at((int)units[unit].x,units[unit].y);
    if (city < 0) {
      improvements2 = tile_improvements(units[unit].x,units[unit].y);
      if (((((byte)improvements2 & TILE_AIRBASE) == 0x40) &&
          (city2 = city_find_nearest(units[unit].x,units[unit].y,
                                      units[unit].owner,0xffffffff,-1), -1 < city2)) &&
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
void __cdecl unit_found_city(int unit,int city,const char *city_name)

{
  int x;
  int y;
  int other_civ;
  
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  if ((game.reveal_map != '\0') || (units[unit].owner == human_civ[0])) {
    mapviews_ensure_visible(x,y,units[unit].owner);
  }
  if ((-1 < city) ||
     (city = city_create(x,y,units[unit].owner), -1 < city)) {
    if (city_name != 0) {
      crt_strcpy((char *)(&cities[city].name),city_name);
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
void __cdecl unit_goto_step(int unit)

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
void __cdecl unit_pillage(int unit,int improvement)

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
     (confirmed = diplo_confirm_attack((int)(char)civ,cities[city2].owner,0xe), confirmed == 0)) {
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
      tile_update_known(x,y,units[unit].owner);
      tile_update_known(x,y,cities[city].owner);
      diplo_set_treaty_flags(units[unit].owner,cities[city].owner,0x2000);
      if (cities[city].owner != units[unit].owner) {
        civ_add_ai_goal(units[unit].owner,x,y,ROLE_ATTACK,4);
      }
    }
    map_redraw_area_all_views(x,y,1,human_civ[0],1);
    unit_exhaust_moves(unit);
  }
  return;
}

/* 004c5408 auto */
i32 __cdecl unit_execute_orders(int unit)

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
    unit_do_terrain_work(unit,units[unit].orders);
    break;
  case ORDER_GOTO:
  case ORDER_AI_GOTO:
    unit_goto_step(unit);
  }
  return handled;
}

/* 004c54da auto */
void __cdecl unit_go_home(int unit)

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
         (dist = tile_landmass(cities[city].x,cities[city].y), dist == landmass2)
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

/* 004ca1cd auto */
void __cdecl unit_airlift_perform(int unit,int city2,int city,int risk_rolls,int civ)

{
  int roll;
  iptr civ_name;
  int i;
  
  cities[city2].flags = cities[city2].flags | CITYF_AIRLIFTED;
  cities[city].flags = cities[city].flags | CITYF_AIRLIFTED;
  unit_exhaust_moves(unit);
  units[unit].orders = ORDER_NONE;
  unit_relocate(unit,cities[city].x,cities[city].y,1);
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
void __cdecl unit_paradrop(int unit,int x,int y)

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
  text_set_number_arg_u8(0,cosmic.paradrop_range);
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
void __cdecl unit_pick_caravan_commodity(int city,int unit)

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
void __cdecl unit_stack_wake_by_role(int unit,uint role_mask,int wake)

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
void __cdecl unit_set_goto(int unit,i32 goto_flag,i32 dest_x,i32 dest_y)

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
        (ret = tile_adjacent_nonallied_civ(units[unit].x,(int)units[unit].y,units[unit].owner)
        , ret == 0)))) ||
      ((ret = ai_unit_choose_orders(), ret == 0 && (ret = unit_moves_left(unit), ret != 0))
      )) && (ret = unit_execute_orders(unit), ret == 0)) {
    unit_exhaust_moves(unit);
  }
  return;
}

/* 0056b96e auto */
int __cdecl unit_stack_pick_display_unit(int unit)

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
          ret = dlg_game_text_input(str_namecity,0xf,cities[city].name,city_name);
          if (ret == 0) {
            unit_found_city(unit,city,city_name);
            text_set_string_arg(0,(const char *)&cities[city].name);
            text_clear();
            text_append_year((int)game.year);
            text_set_string_arg(1,(const char *)&textbuf.buf);
            snd_play_effect(SND_BLDCITY,1,0,0);
            popup_city_picture_zoom((const char *)&str_founded,city,0x4c,1,civ);
            if ((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && (civs[civ].num_cities == 1)) {
              popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),str_firstproduct,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
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
        text_set_number_arg_u8(0,cosmic.aqueduct_size);
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
          civ_check_destroyed(civ,0);
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
        city = city_at((int)x,y);
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
void __cdecl unit_cmd_terraform(int order)

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
        popup_game_with_sprite(str_alreadymining,(Sprite *)&land_improvement_sprites_block.mine_sprite,0);
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
    popup_game_with_sprite(str_nopollution,(Sprite *)&pollution_sprite_block.pollution_sprite,8);
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
  new_city = city_at((int)units[unit].x,units[unit].y);
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
  int civ2;
  int choice;
  uint improvements_mask;
  i32 eh_next;
  Tile *map_tile;
  
  choice = 0;
  Dialog dialog2(0x4000);
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
    tile_flags = tile_improvements(units[unit].x,units[unit].y);
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
  int on_ocean;
  int y2;
  uint show_all;
  iptr tile_index;
  iptr city3;
  i32 eh_next;
  
  Dialog dialog2(0x2000);
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
        city_landmass = tile_landmass(cities[city].x,cities[city].y);
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
    for (unit = unit_stack_first(game.active_unit); -1 < unit;
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
  iptr active;
  int civ2;
  iptr city;
  i32 eh_next;
  
  Dialog dialog2(0x2000);
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
      city2 = city_at((int)units[unit2].x,units[unit2].y);
      if (city2 < 0) {
        popup_game_with_sprite(str_noairport,(Sprite *)&popup_info_icon_sprite,0);
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
i32 __cdecl unit_caravan_enter_city(int unit,uptr city)

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
  iptr k;
  int in_demand;
  i32 eh_next;
  
  Dialog dialog2(0x4000);
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
                 ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),0,0);
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

/* 005b2590 auto */
i32 __cdecl unit_stack_validate(int unit)

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
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_id_0_id_max_units_2,(const char*)&str_d_ss_franklinton_unit_cpp,0x11);
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
        unit_stack_link(unit4,x,y,0);
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
int __cdecl unit_max_hp(int unit)

{
  return (int)(char)unit_types[units[unit].type].hit_points;
}

/* 005b29d7 auto */
int __cdecl unit_hp_remaining(int unit)

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
uint __cdecl unit_max_moves(int unit)

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
int __cdecl unit_moves_left(int unit)

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
int __cdecl unit_next_in_stack(int unit)

{
  if (-1 < unit) {
    unit_stack_validate(unit);
    unit = (iptr)units[unit].next_in_stack;
  }
  return unit;
}

/* 005b2cc3 auto */
int __cdecl unit_stack_last(int unit)

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
int __cdecl unit_stack_first(int unit)

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
void __cdecl unit_set_sleep(int unit)

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
void __cdecl unit_stack_unlink(int unit,int sync)

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
       (tile = map_in_bounds(units[unit].x,units[unit].y), tile != 0)) {
      map_tile = tile_at(units[unit].x,units[unit].y);
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
      popup_game_with_sprite(str_serverconnecttime_10,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b345f auto */
void __cdecl unit_stack_link(int unit,int x,int y,int sync)

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
      popup_game_with_sprite(str_serverconnecttime_11,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b36df auto */
void __cdecl unit_relocate(int unit,int x,int y,int sync)

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
      popup_game_with_sprite(str_serverconnecttime_12,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b3863 auto */
void __cdecl unit_relocate_in_place(int unit,i32 sync)

{
  unit_relocate(unit,units[unit].x,units[unit].y,sync);
  return;
}

/* 005b389f auto */
void __cdecl unit_move_to_stack_bottom(int unit,int sync)

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
        popup_game_with_sprite(str_serverconnecttime_13,(Sprite *)&popup_info_icon_sprite,0);
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
void __cdecl unit_stack_relocate_all(int unit,int x,int y,int sync)

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
void __cdecl unit_stack_ships_to_top(int unit,i32 sync)

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
        unit_relocate(unit,x,y,sync);
        unit = cur;
      }
    }
  }
  return;
}

/* 005b3d06 auto */
int __cdecl unit_create(int unit_type,uptr civ,int x,int y)

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
          popup_game_with_sprite(str_toomanyunits,(Sprite *)&popup_info_icon_sprite,0);
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
      popup_game_with_sprite(str_serverconnecttime_14,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (unit = (*(int *)&net.nm_unit_created_result), net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return unit;
}

/* 005b4391 auto */
void __cdecl unit_delete(int unit,int update_ui)

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
        net_send_message(NM_CITYWIN_CHECK_UNIT,0xff,-1,x,y,0,0,0,0,0);
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
        popup_game_with_sprite(str_serverconnecttime_15,(Sprite *)&popup_info_icon_sprite,0);
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
void __cdecl unit_stack_delete_all(int unit,int sync)

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
void __cdecl unit_clear_visibility(int unit)

{
  if (-1 < unit) {
    units[unit].visibility = '\0';
  }
  return;
}

/* 005b48b1 auto */
void __cdecl unit_stack_clear_visibility(int unit)

{
  int unit2;
  
  for (unit2 = unit_stack_first(unit); -1 < unit2; unit2 = unit_next_in_stack(unit2)) {
    unit_clear_visibility(unit2);
  }
  return;
}

/* 005b490e auto */
void __cdecl unit_set_seen_by(int unit,int civ)

{
  if (((-1 < civ) && (units[unit].owner != civ)) && (-1 < unit)) {
    units[unit].visibility = units[unit].visibility | (byte)(1 << ((byte)civ & 0x1f));
  }
  return;
}

/* 005b496e auto */
void __cdecl unit_stack_set_seen_by(int unit,int civ)

{
  int unit2;
  
  for (unit2 = unit_stack_first(unit); -1 < unit2; unit2 = unit_next_in_stack(unit2)) {
    unit_set_seen_by(unit2,civ);
  }
  return;
}

/* 005b4ee2 auto */
void __cdecl unit_stack_add_visibility(int unit,i32 vis_mask)

{
  for (unit = unit_stack_first(unit); -1 < unit; unit = unit_next_in_stack(unit)) {
    units[unit].visibility = units[unit].visibility | (byte)vis_mask;
  }
  return;
}

/* 005b50ad auto */
int __cdecl unit_stack_sum(int unit,int mode)

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
int __cdecl unit_stack_count_role(int unit,int wanted_role)

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
      popup_game_with_sprite(str_serverconnecttime_16,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (capacity2 = (*(int *)&net.nm_ship_stacked_result), net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
}

/* 005b542e auto */
int __cdecl unit_gather_cargo(int unit,int lift_off,int send_update)

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
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_ship_1_ship_max_units_2,(const char*)&str_d_ss_franklinton_unit_cpp_2,0x61d);
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
void __cdecl unit_lift_off_map(int unit,int send_update)

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
        popup_game_with_sprite(str_serverconnecttime_17,(Sprite *)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
      }
      while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
    }
  }
  return;
}

/* B36: a ship lost at sea takes with it its cargo (the units
   unit_gather_cargo has just marked aboard), and the land units of its
   owner that the other ships on the square cannot carry; the other
   ships, the rest of their cargo, aircraft in flight and other civs'
   units stay.  (The original deletes every unit on the square.) */
static void unit_delete_ship_at_sea(int ship)
{
  static int left[0x802];
  int n = 0, capacity = 0, land = 0, unit, next, i;
  char owner = units[ship].owner;
  for (unit = unit_stack_first(ship); -1 < unit; unit = next) {
    next = unit_next_in_stack(unit);
    if (unit == ship) {
      continue;
    }
    if ((units[unit].flags & UNITF_ABOARD) != 0) {
      unit_delete(unit,0);
      continue;
    }
    if (units[unit].owner != owner) {
      continue;
    }
    if (unit_types[units[unit].type].domain == DOMAIN_SEA) {
      if ((unit_types[units[unit].type].flags & UTYPE_CARRY_AIR) == 0 &&
          (unit_types[units[unit].type].hold != 0 ||
           (unit_types[units[unit].type].flags & UTYPE_SUBMARINE) == 0)) {
        capacity += owner == 0 ? 0x14 : (int)(char)unit_types[units[unit].type].hold;
      }
    }
    else if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
      left[n++] = unit;
      land++;
    }
  }
  unit_delete(ship,0);
  /* (those that do not fit: the last on the square) */
  for (i = n - 1; capacity < land && 0 <= i; i--, land--) {
    unit_delete(left[i],0);
  }
}

/* 005b5d93 auto */
void __cdecl unit_delete_with_cargo(int unit,int send_update)

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
      popup_game_with_sprite(str_serverconnecttime_18,(Sprite *)&popup_info_icon_sprite,0);
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
    if (bugfix_state()) {  /* B36 */
      unit_delete_ship_at_sea(unit);
    }
    else {
      unit_stack_delete_all(unit,0);
    }
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
void __cdecl unit_destroy_redraw(int unit,int send_update)

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
      popup_game_with_sprite(str_serverconnecttime_19,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 005b633f auto */
i32 __cdecl unit_is_selectable(int unit)

{
  int result;
  i32 selectable;
  
  selectable = 0;
  if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
     (result = map_in_bounds(units[unit].x,units[unit].y), result != 0)) {
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
i32 __cdecl unit_is_awake_with_moves(int unit)

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
int __cdecl unit_find_nearest_selectable(int unit,int human_only)

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
void __cdecl unit_exhaust_moves(int unit)

{
  i32 max_moves;
  
  max_moves = unit_max_moves(unit);
  units[unit].moves_used = (u8)max_moves;
  return;
}

/* 005b67af auto */
int __cdecl unit_find_nearest(i32 target_x,i32 target_y,int owner_civ,int exclude_unit)

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
int __cdecl unit_home_city_name(int unit)

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
i32 __cdecl unit_dlg_wake_unit_cb(int unit)

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
unit_dlg_draw_sprite_cb(i32 dlg,DibSurface *surface,int unit,i32 left,int x,
            i32 y)

{
  gfx_draw_unit(surface,unit,4,x + 2,y,dlg_unit_sprite_zoom,0);
  return;
}

/* Part of unit_stack_select_dialog (005b6aea): a unit's line: its owner,
   veteran, type, home city (and a caravan's commodity). */
static void unit_stack_select_text(iptr unit)
{
  iptr adjective;
  uint city;

  text_clear();
  adjective = civ_adjective(units[unit].owner);
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
i32 __cdecl unit_stack_select_dialog(int unit,const char *title,int allow_cancel)

{
  i32 choice;
  int count;
  Sprite sprite;
  i8 *seh_scope;
  
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  Dialog dialog2(0x4000);
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
      dialog2.add_sprite(&sprite,unit,(const char *)&textbuf.buf);
    }
  }
  choice = dialog2.show_modal(0);
  return choice;
}

/* 005b9179 auto */
void __cdecl nuclear_blast(int center_x,int center_y)

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
char * __cdecl path_join_upper(char *dest,const char *dir,const char *filename)

{
  char *end;
  char buf[80];
  
  crt_strcpy(buf,dir);
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
