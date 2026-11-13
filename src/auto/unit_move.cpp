/* The game, units: a move one square on (unit_move_dir) and its parts: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

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
    unit_relocate(unit2,cities[city4].x,cities[city4].y,1);
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
  Tile *tile;
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
    tile = tile_at(x3,y3);
    arg = 2;
    unit6 = tile_first_unit(x3,y3);
    value = unit_stack_sum(unit6,arg);
    if (value == 1) {
      tile->improvements = tile->improvements & 0xfe;
    }
    else if (1 < value) {
      combat_anim.origin_x = (int)units[unit].x;
      combat_anim.origin_y = (int)units[unit].y;
      value = unit_stack_sum(unit,5);
      if (value == 0) {
        if (units[unit].next_in_stack < 0) {
          if (units[unit].prev_in_stack < 0) {
            tile->improvements = tile->improvements & 0xfe;
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
          tile->improvements = tile->improvements & 0xfe;
        }
        else {
          combat_anim.origin_stack_unit = unit_stack_first(unit);
          while ((combat_anim.origin_stack_unit == unit) ||
                 (unit_types[units[combat_anim.origin_stack_unit].type].domain != DOMAIN_SEA)) {
            combat_anim.origin_stack_unit = (iptr)units[combat_anim.origin_stack_unit].next_in_stack;
            if (combat_anim.origin_stack_unit < 0) {
              tile->improvements = tile->improvements & 0xfe;
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
        tile->improvements = tile->improvements | 1;
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
      popup_game_with_sprite(str_serverconnecttime_7,(Sprite *)&popup_info_icon_sprite,0);
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
      dlg_game_txt_with_unit_flags((const char *)&str_fuel,unit_type,(ui_metrics.large_screen == 0) - 1 & 8);
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
          dlg_game_txt_with_unit_flags((const char *)&str_trireme,unit_type,(ui_metrics.large_screen == 0) - 1 & 8);
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
static int unit_move_dir_land_unit_checks(int &unit,iptr orig_unit,uptr dir,uptr civ,uptr &civ2,
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
        popup_game_with_sprite(str_zoc,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
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
      popup_game_with_sprite(str_alliedrepair,(Sprite *)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))
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
static int unit_move_dir_expel(int &unit,iptr orig_unit,uptr dir,uptr civ,uptr civ2,byte civ3,
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
        popup_game_with_sprite(str_noexpel,(Sprite *)&popup_info_icon_sprite,0);
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
static int unit_move_dir_attack_unit(int &unit,iptr orig_unit,uptr dir,uptr civ,uptr &civ2,
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
      popup_game_with_sprite(str_amphib,(Sprite *)&popup_info_icon_sprite,0);
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
      popup_game_with_sprite(str_noncombat,(Sprite *)&popup_info_icon_sprite,0);
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
      popup_game_with_sprite(str_fighter,(Sprite *)&popup_info_icon_sprite,0);
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
      text_set_number_arg_u8(1,cosmic.road_move_mult);
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
static int unit_move_dir_pick_landing_unit(int &unit,iptr orig_unit,int x3,int y3,int &unit_count,
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
static int unit_move_dir_enemy_city(int &unit,iptr orig_unit,uptr civ,uptr &civ2,byte civ3,
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
        popup_game_with_sprite(str_occupy,(Sprite *)&popup_info_icon_sprite,0);
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

/* Where unit_move_dir goes on after the checks in its loop: on with the
   move (unit_move_dir_step), to its finish (unit_move_dir_finish_move:
   the attack and the no-direction paths too), to the map's unlock, or
   straight to telling the network (the map lock refused). */
enum UnitMoveNext {
  UMD_CHECKS,
  UMD_FINISH_MOVE,
  UMD_UNLOCK_MAP,
  UMD_SEND_UPDATE
};

/* The locals of unit_move_dir that its move and finish share: references to
   the entry point's own. */
struct UnitMoveDir {
  int &unit;
  iptr &orig_unit;
  uptr &dir;
  uptr &civ;
  uptr &civ2;
  byte &civ3;
  iptr &x2;
  int &y2;
  int &x3;
  int &y3;
  uptr &city;
  iptr &unit2;
  iptr &unit3;
  u8 &unit_domain;
  uptr &terrain2;
  uint &src_is_ocean;
  uint &dst_is_ocean;
  uint &result;
  int &has_hut;
  bool &flag;
  bool &map_locked;
  int &lock_id;
  bool &boarding;
  int &attack_done;
  int *civ_sees;
  int *civ_animates;
};

/* Part of unit_move_dir (0059062c): the move itself, once the loop's checks
   let it through: an enemy city entered (unit_move_dir_enemy_city), the
   moves it costs (a land unit short of them may still make it, at random),
   an AI unit holding back, who sees it, the unit moved (by the host in a
   network game), a landing counted, a city captured, a ship's sleeping
   cargo woken.  Returns UMD_FINISH_MOVE, or UMD_UNLOCK_MAP when the unit
   does not move. */
static int unit_move_dir_step(UnitMoveDir *m)
{
  int &unit = m->unit; iptr &orig_unit = m->orig_unit; uptr &dir = m->dir;
  uptr &civ = m->civ; uptr &civ2 = m->civ2; byte &civ3 = m->civ3;
  iptr &x2 = m->x2; int &y2 = m->y2; int &x3 = m->x3; int &y3 = m->y3;
  uptr &city = m->city; iptr &unit2 = m->unit2; iptr &unit3 = m->unit3;
  u8 &unit_domain = m->unit_domain; uptr &terrain2 = m->terrain2;
  uint &src_is_ocean = m->src_is_ocean; uint &dst_is_ocean = m->dst_is_ocean;
  uint &result = m->result; int &has_hut = m->has_hut; bool &flag = m->flag;
  bool &map_locked = m->map_locked; int &lock_id = m->lock_id;
  int *civ_sees = m->civ_sees; int *civ_animates = m->civ_animates;
  short *continent_count;
  byte unit_type;
  int city3;
  int move_roll;
  uint step_cost;
  uint prev_moves_used;

  if ((-1 < (int)city) && ((int)cities[city].owner != civ)) {
    if (unit_move_dir_enemy_city(unit,orig_unit,civ,civ2,civ3,x2,y2,x3,y3,city,unit_domain,result) != 0)
    return UMD_UNLOCK_MAP;
  }
  city3 = unit_moves_left(unit);
  unit = orig_unit;
  if (city3 == 0) return UMD_UNLOCK_MAP;
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
      move_roll = crt_rand_at("unit_move_dir");
      move_roll = move_roll % (int)step_cost;
    }
    city3 = unit_moves_left(unit);
    if (city3 <= move_roll) {
      unit_exhaust_moves(unit);
      result = 1;
      return UMD_UNLOCK_MAP;
    }
  }
  if ((unit_sound_editor_window_block.unit_move_flags & 2) == 0) {
    result = 1;
    return UMD_UNLOCK_MAP;
  }
  unit_type = units[unit].moves_used;
  prev_moves_used = (uint)unit_type;
  units[unit].moves_used += (char)step_cost;
  if ((1 << (civ3 & 0x1f) & (uint)game.human_players) == 0) {
    if (unit_move_dir_ai_holds_back(unit,dir,civ,civ2,unit2,city,x2,y2,x3,y3,unit_domain,step_cost,
                                    unit_type,prev_moves_used) != 0) return UMD_UNLOCK_MAP;
  }
  if (((civ == 0) && (dst_is_ocean == 0)) &&
     (((city3 = tile_visible_to(x3,y3,(*(i8 *)&human_civ)), city3 != 0 &&
       ((((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) == 0 &&
         ((int)units[unit].owner != ((*(i8 *)&human_civ) & 0xffU))) &&
        (city3 = city_find_nearest(x3,y3,-1,0xffffffff,-1), -1 < city3)))) &&
      (cities[city3].owner == (*(i8 *)&human_civ))))) {
    units[unit].visibility = tile_at(x3,y3)->visible | units[unit].visibility;
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
    unit_stack_add_visibility(unit,(uint)tile_at(x2,y2)->visible);
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
  return UMD_FINISH_MOVE;
}

/* Part of unit_move_dir (0059062c): a move's finish (the attack and the
   no-direction paths too): the unit to its stack's bottom, the view
   around it, a pause where the player sees an enemy move, air units' fuel,
   triremes lost at sea, huts, settlers woken, boarding, a long move's
   continuation.  The move is made (RESULT 1) unless the unit was lost. */
static void unit_move_dir_finish_move(UnitMoveDir *m)
{
  int &unit = m->unit; uptr &dir = m->dir; uptr &civ = m->civ; byte &civ3 = m->civ3;
  iptr &x2 = m->x2; int &y2 = m->y2; int &x3 = m->x3; int &y3 = m->y3;
  iptr &unit3 = m->unit3; u8 &unit_domain = m->unit_domain; uint &result = m->result;
  int &has_hut = m->has_hut; bool &flag = m->flag; bool &boarding = m->boarding;
  int &attack_done = m->attack_done; int *civ_sees = m->civ_sees;
  SHORT shift_state;

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
    if (unit_move_dir_air_fuel(unit,x2,y2,attack_done,flag,x3,y3) != 0) return;
  }
  if (unit_move_dir_trireme_loss(unit,civ,x2,y2,flag,x3,y3) != 0) return;
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
}


/* 0059062c auto */
/* WARNING: Removing unreachable block (ram,0x0059103e) */
/* WARNING: Removing unreachable block (ram,0x0059104b) */
/* WARNING: Removing unreachable block (ram,0x00590f5a) */
/* WARNING: Removing unreachable block (ram,0x0059100a) */
/* WARNING: Removing unreachable block (ram,0x00591023) */
/* WARNING: Removing unreachable block (ram,0x00591027) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl unit_move_dir(int unit,uptr dir,i32 move_flags)

{
  u8 unit_domain;
  byte civ3;
  bool flag;
  bool boarding;
  bool map_locked;
  char src_terrain;
  uptr civ;
  iptr src_city;
  iptr in_bounds;
  byte unit_type;
  int city3;
  uptr city;
  iptr unit3;
  iptr orig_unit;
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
  i32 unused_c;
  int next;  /* where the move goes on after its checks */
  UnitMoveDir m = {unit, orig_unit, dir, civ, civ2, civ3, x2, y2, x3, y3, city, unit2, unit3,
    unit_domain, terrain2, src_is_ocean, dst_is_ocean, result, has_hut, flag, map_locked, lock_id,
    boarding, attack_done, civ_sees, civ_animates};
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
  next = UMD_CHECKS;
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
        popup_game_with_sprite(str_notonmap,(Sprite *)&popup_info_icon_sprite,0);
      }
      unit = orig_unit;
      if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
        unit_exhaust_moves(unit);
      }
      next = UMD_UNLOCK_MAP;
      break;
    }
    if (((2 < game.multiplayer) && (net.mp_simultaneous_moves != '\0')) &&
       (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0 && (-1 < (int)dir)))) {
      city3 = net_lock_map(civ,x3,y3,x2,y2,lock_id);
      if (city3 == 0) {
        if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
          unit_exhaust_moves(unit);
        }
        next = UMD_SEND_UPDATE;
        break;
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
      next = UMD_FINISH_MOVE;
      break;
    }
    units[unit].flags = units[unit].flags | UNITF_MOVED;
    if (unit_domain == DOMAIN_LAND) {
      if (unit_move_dir_land_unit_checks(unit,orig_unit,dir,civ,civ2,civ3,x2,y2,x3,y3,city,unit2,
                                         src_is_ocean) != 0) {
        next = UMD_UNLOCK_MAP;
        break;
      }
    }
    if ((unit_domain == DOMAIN_LAND) && (dst_is_ocean != 0)) {
      unit = orig_unit;
      if ((civ2 != civ) || (city3 = unit_stack_sum(unit2,6), city3 < 1)) {
        next = UMD_UNLOCK_MAP;
        break;
      }
      boarding = true;
    }
    if ((-1 < unit2) && ((int)units[unit2].owner != civ)) {
      if (unit_move_dir_attack_unit(unit,orig_unit,dir,civ,civ2,civ3,x2,y2,x3,y3,city,unit2,unit_domain,
                                    src_is_ocean,dst_is_ocean,result,attack_done,unit3,answer) != 0) {
        next = UMD_FINISH_MOVE;
      }
      else {
        next = UMD_UNLOCK_MAP;
      }
      break;
    }
    if (((dst_is_ocean != 0) || (-1 < (int)city)) || (unit_domain != DOMAIN_SEA)) break;
    if (unit_move_dir_pick_landing_unit(unit,orig_unit,x3,y3,unit_count,answer) != 0) {
      next = UMD_UNLOCK_MAP;
      break;
    }
  }
  if (next == UMD_CHECKS) {
    next = unit_move_dir_step(&m);
  }
  if (next == UMD_FINISH_MOVE) {
    unit_move_dir_finish_move(&m);
  }
  if ((next != UMD_SEND_UPDATE) && (map_locked)) {
    net_unlock_map(civ,lock_id);
  }
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
