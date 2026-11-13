/* The game, units: diplomats and spies (their missions in cities, bribery, escape): functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004c5fae auto */
i32 __cdecl spy_try_escape(int unit,int mission_mode,int civ)

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
        unit_relocate(unit,cities[city].x,cities[city].y,1);
        unit_exhaust_moves(unit);
        map_redraw_tile_all_views(x,y);
        map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
        if (2 < game.multiplayer) {
          net_send_state_diff(0xff,2,0,0,0);
          net_send_message(NM_DRAW_SQUARE,0xff,x,y,0,0,0,0,0,0);
          net_send_message(NM_DRAW_SQUARE,0xff,cities[city].x,cities[city].y,0,0,0,0,0,0);
        }
        text_set_string_arg(1,(const char *)&cities[city].name);
      }
      if (-1 < mission_mode) {
        if ((autoplay.active == 0) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) {
          if ((units[unit].flags & UNITF_VETERAN) == 0) {
            if (human_civ[0] == civ3) {
              popup_game_with_sprite(str_bond007,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),8
                                );
            }
            else if (2 < game.multiplayer) {
              net_send_text_args(0x26,net.net_slots[net.net_civ_slots[civ3]].id,
                                 2,0,units[unit].type,8);
            }
          }
          else if (human_civ[0] == civ3) {
            popup_game_with_sprite(str_bond,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),8);
          }
          else if (2 < game.multiplayer) {
            net_send_text_args(0x27,net.net_slots[net.net_civ_slots[civ3]].id,2,
                               0,units[unit].type,8);
          }
        }
        units[unit].flags = units[unit].flags | UNITF_VETERAN;
      }
      return 0;
    }
    if (((autoplay.active == 0) && ((1 << (civ2 & 0x1f) & (uint)game.human_players) != 0)) &&
       (-1 < mission_mode)) {
      if (human_civ[0] == civ3) {
        popup_game_with_sprite(str_bondglory,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),8);
      }
      else if (game.multiplayer == 3) {
        net_send_text_args(0x28,net.net_slots[net.net_civ_slots[civ3]].id,0,0,
                           units[unit].type,8);
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
i32 __cdecl spy_check_caught(int unit,int civ)

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
      popup_game_with_sprite(str_nailed,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)unit_type * 0x3c)),8);
    }
    result = 1;
  }
  return result;
}

/* 004c654d auto */
uint __cdecl spy_confirm_treaty_breach(int civ,int target_civ)

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
  dlg.add_sprite((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)
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
      str_append_space(textbuf.buf);
      str_append_lparen(textbuf.buf);
      text_append_label(0xed);
      str_append_rparen(textbuf.buf);
    }
    dlg.add_option((const char *)&textbuf.buf,0,0);
  }
  text_read_line(1);
  if (is_spy != 0) {
    str_append_space(textbuf.buf);
    str_append_lparen(textbuf.buf);
    text_append_label(0xed);
    str_append_rparen(textbuf.buf);
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
        popup_game_with_sprite(str_enemyembassy,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8);
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
      popup_game_with_sprite(str_enemyinvestigate,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8);
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
        choice = popup_game_sprite_flags(str_stealspecific,1,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8);
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
        popup_game_with_sprite(str_steal,(Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
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
        popup_game_with_sprite(str_curses,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c)),8);
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
      popup_game_with_sprite(str_nosteal,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8);
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
    str_append_string_id(build_name,improvements[-(int)cities[city].building].name);
  }
  else {
    str_append_string_id(build_name,unit_types[cities[city].building].name);
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
    choice = popup_game_sprite_flags(str_sabotagespecific,1,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8);
    if (choice < 0) return;
    if (choice == 1) {
      dlg.load_game_section((const char *)&str_sabotage,1);
      dlg.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),0,0);
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
           (choice = popup_game_with_sprite(str_sabotagehard,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8),
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
                           cities[city].x,cities[city].y,0,0,0,0,0,0);
        net_send_message(NM_CITYWIN_REDRAW,net.net_slots[net.net_civ_slots[civ2]].id,0,0
                           ,0,0,0,0,0,0);
        net_send_text_args(0x59,net.net_slots[net.net_civ_slots[civ2]].id,1,0
                           ,improvement2,civ);
      }
      if ((autoplay.active == 0) && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0))
      {
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh_if_city(city);
        popup_game_with_sprite(str_sabotageone,((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c)),8);
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
      popup_game_with_sprite(str_watersupply,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8);
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
        (ret = popup_game_with_sprite(str_majorincident,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8),
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
      popup_game_with_sprite(str_norevolt,(Sprite *)&government_sprites_block.government_sprite_democracy,0);
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
  popup_game_with_sprite(str_chatspystart,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type3 * 0x3c)),8);
}

/* 004c6bf5 auto */
void __cdecl spy_city_mission(int unit,int city)

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
    city = city_find_nearest(units[unit].x,units[unit].y,-1,0xffffffff,-1);
    if ((-1 < city) && (cities[city].owner == civ)) {
      new_home = (i8)city;
      units[unit].home_city = new_home;
    }
    unit_set_seen_by(unit,civ2);
    tile_set_owner(units[unit].x,units[unit].y,civ,1);
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
      popup_game_with_sprite(str_serverconnecttime_5,(Sprite *)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
  }
}

/* 004c9528 auto */
void __cdecl unit_bribe(int unit,int civ)

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
    popup_game_with_sprite(str_incorruptible,(Sprite *)&government_sprites_block.government_sprite_democracy,0);
    return;
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
     (has_adv = civ_has_advance(civ,unit_types[UNIT_DIPLOMAT].prerequisite), has_adv == 0)) {
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
                         units[unit].x,units[unit].y,civ2,0,0,0,0,0);
      net_send_text_args(0x22,net.net_slots[net.net_civ_slots[civ2]].id,3,0,
                         unit,0);
    }
  }
  map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
  if (2 < game.multiplayer) {
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_DRAW_SQUARE,0xff,units[unit].x,units[unit].y,0,0,0,0,0,0);
  }
  if (((1 << (civ3 & 0x1f) & (uint)game.human_players) != 0) && (human_civ[0] == civ2)) {
    ui_delay_ticks(0x1e);
  }
  return;
}

/* 004c9ebd auto */
i32 __cdecl spy_unit_mission(int unit2,int unit,int civ)

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
        adjective = civ_get_adjective(units[unit].owner);
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
