/* The game, the cheat menu: its commands and editors: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00411bd7 auto */
void __cdecl cheat_throneroom_upgrade(void)

{
  throneroom_show_upgrade(human_civ[0]);
  return;
}

/* 00554297 auto */
void __cdecl cheat_toggle_mode(void)

{
  ushort was_flag80;
  ushort new_flags;
  int answer;
  int civ;
  int password_count;
  
  password_count = 0;
  for (civ = 0; civ < 8; civ++) {
    if (map_views_block.civ_has_password[civ] != 0) {
      password_count++;
    }
  }
  if (password_count == 0) {
    if (((game.game_options & OPT_CHEAT_MODE) == 0) && ((game.game_flags & GAME_CHEATED) == 0)) {
      answer = popup_game_with_sprite(str_reallycheat_2,(Sprite *)&popup_info_icon_sprite,0);
      if (answer == 0) {
        return;
      }
      new_flags = game.game_flags | GAME_CHEATED;
      was_flag80 = game.game_flags & GAME_SCENARIO;
      game.game_flags = new_flags;
      if (was_flag80 != 0) {
        popup_text((const char *)&str_help_17,(const char *)&str_warning_5);
      }
    }
    game.game_options = game.game_options ^ OPT_CHEAT_MODE;
    if ((game.multiplayer == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) {
      if (game.multiplayer != '\0') {
        mp_turn_timer_stop(1);
        civ_saved_cursor_x_block.mp_turn_time_limit_ms = civ_saved_cursor_x_block.cheat_saved_turn_time_ms;
        mp_turn_timer_resume(1);
      }
    }
    else {
      mp_turn_timer_stop(1);
      civ_saved_cursor_x_block.cheat_saved_turn_time_ms = civ_saved_cursor_x_block.mp_turn_time_limit_ms;
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = 0;
    }
    menu_update_states();
  }
  else {
    text_set_number_arg(0,password_count);
    if (password_count == 1) {
      popup_game_with_sprite(str_passwordnocheat1,(Sprite *)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite(str_passwordnocheat2,(Sprite *)&popup_info_icon_sprite,0);
    }
  }
  return;
}

/* 00554423 auto */
void __cdecl cheat_tech_advance(void)

{
  uptr civ;
  
  civ = dbg_pick_player(0);
  if (-1 < (int)civ) {
    civ_research_complete(civ,0);
  }
  return;
}

/* 00554460 auto */
void __cdecl cheat_toggle_all_advances(uptr civ)

{
  iptr advance;
  
  if (civs[civ].future_techs == '\0') {
    for (advance = 0; advance < ADV_COUNT; advance++) {
      civ_give_advance(civ,advance,0,0,0);
    }
    civs[civ].future_techs = 1;
    popup_debug_text((const char *)&str_gavetech);
  }
  else {
    for (advance = 0; advance < 0xd; advance++) {
      *(i8 *)&civs[civ].advances[advance] = 0;
    }
    for (advance = 0; advance < ADV_COUNT; advance++) {
      game.advance_known_by[advance] =
           game.advance_known_by[advance] & ~(byte)(1 << ((byte)civ & 0x1f));
    }
    civs[civ].future_techs = '\0';
    civs[civ].num_advances = 1;
    popup_debug_text((const char *)&str_tooktech);
  }
  ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
  return;
}

/* 005545d3 auto */
void __cdecl cheat_edit_technologies(void)

{
  uptr civ;
  iptr has_it;
  iptr has_preq1;
  int advance3;
  int already_known;
  byte civ_byte;
  iptr advance;
  int advance2;
  i8 bit_mask[4];
  int last_choice;
  i8 *seh_scope;
  
  last_choice = -1;
  Dialog dialog2(0x4000);
  blink.unit_active = 0;
  civ = dbg_pick_player(0);
  if ((int)civ >= 0) {
    while( true ) {
      dialog2.load_game_section((const char *)&str_edittech,1);
      for (advance = 0; advance < ADV_COUNT; advance++) {
        if (advances[advance].enabled != '\0') {
          text_clear();
          text_append_string_id(advances[advance].name);
          text_append_space();
          has_it = civ_has_advance(civ,advance);
          if (has_it == 0) {
            has_preq1 = civ_has_advance(civ,advances[advance].preq[0]);
            if ((has_preq1 == 0) ||
               (advance3 = civ_has_advance(civ,advances[advance].preq[1]), advance3 == ADV_ADVANCED_FLIGHT)) {
              text_append((const char *)&str_empty_170);
            }
          }
          else {
            text_append((const char *)&str_empty_169);
          }
          dialog2.add_option((const char *)&textbuf.buf,advance,0);
        }
      }
      if (-1 < last_choice) {
        dialog2.set_default_choice(last_choice);
      }
      advance3 = dialog2.show_modal(0);
      if (advance3 < 0) break;
      if (dialog2.result_extra == 0) {
        already_known = civ_has_advance(civ,advance3);
        if (already_known == 0) {
          civ_give_advance(civ,advance3,0,0,0);
        }
        else {
          bit_index_split(advance3,&advance2,(int *)bit_mask);
          civs[civ].advances[advance2] =
               civs[civ].advances[advance2] & ~bit_mask[0];
          civ_byte = (byte)civ;
          game.advance_known_by[advance3] =
               game.advance_known_by[advance3] & ~(byte)(1 << (civ_byte & 0x1f));
        }
        civs[civ].num_advances = '\0';
        last_choice = advance3;
        for (advance = 0; advance < ADV_COUNT; advance++) {
          advance3 = civ_has_advance(civ,advance);
          if (advance3 != ADV_ADVANCED_FLIGHT) {
            civs[civ].num_advances++;
          }
        }
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
      }
      else {
        cheat_toggle_all_advances(civ);
      }
    }
  }
  return;
}

/* 00554962 auto */
void __cdecl cheat_force_government(void)

{
  int civ;
  
  civ = dbg_pick_player(0);
  if (-1 < civ) {
    civ_select_government_dialog(civ,1);
  }
  return;
}

/* 0055499f auto */
void __cdecl cheat_change_terrain(void)

{
  byte old_terrain_byte;
  int result;
  byte *tile;
  uptr terrain;
  iptr x;
  uint old_terrain;
  uptr option_terrain;
  i32 option_flags;
  uptr first_terrain;
  int civ;
  int min_landmass;
  uptr terrain2;
  int y2;
  iptr dir;
  byte improvements2;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  result = map_in_bounds(map_cursor.x[0],map_cursor.y[0]);
  if (result != 0) {
    result = tile_city_owner(map_cursor.x[0],map_cursor.y[0]);
    if (result < 0) {
      tile = (byte *)tile_at(map_cursor.x[0],map_cursor.y[0]);
      old_terrain_byte = tile_terrain(map_cursor.x[0],map_cursor.y[0]);
      old_terrain = (uint)old_terrain_byte;
      terrain2 = old_terrain;
      result = tile_is_ocean((int)map_cursor.x[0],(int)map_cursor.y[0]);
      if (result == 0) {
        dialog2.load_text((const char *)&str_debug_3,(const char *)&str_terrain1,1);
        option_flags = 0;
        first_terrain = terrain2;
        x = (iptr)label_string(terrain_types[terrain2].name);
        dialog2.add_option((const char *)x,first_terrain,option_flags);
        while (-1 < terrain_types[terrain2].irrigate_to) {
          terrain = (uptr)terrain_types[terrain2].irrigate_to;
          option_flags = 0;
          terrain2 = terrain;
          x = (iptr)label_string(terrain_types[terrain].name);
          dialog2.add_option((const char *)x,terrain,option_flags);
        }
        if (old_terrain != terrain2) {
          if ((cheat_terrain_saved_bits < 0) &&
             (terrain2 = dialog2.show_modal(0), (int)terrain2 < 0)) {
            return;
          }
          *tile = *tile & 0x80;
          *tile = *tile | (byte)terrain2;
        }
        improvements2 = tile[1];
        dlg_check_set(0,improvements2 & 4);
        dlg_check_set(1,improvements2 & 8);
        dlg_check_set(2,improvements2 & 0x10);
        dlg_check_set(3,improvements2 & 0x20);
        dlg_check_set(4,(uint)((improvements2 & 0x42) == 0x40));
        dlg_check_set(5,(uint)((improvements2 & 0x42) == 0x42));
        dlg_check_set(6,improvements2 & 0x80);
        if (cheat_terrain_saved_bits < 0) {
          result = popup_text_flags((const char *)&str_debug_4,(const char *)&str_terrain_4,1);
          if (result < 0) {
            return;
          }
          if (dlg_input.help_pressed == 0) {
            cheat_terrain_saved_bits = (*(int *)&dlg_input.check_bits);
          }
        }
        else {
          (*(int *)&dlg_input.check_bits) = cheat_terrain_saved_bits;
        }
        if (dlg_input.help_pressed == 0) {
          if ((improvements2 & 0x80) != 0) {
            game.polluted_tiles--;
          }
          improvements2 = improvements2 & 1;
          result = dlg_check_get(0);
          if (result != 0) {
            improvements2 = improvements2 | 4;
          }
          result = dlg_check_get(1);
          if (result != 0) {
            improvements2 = improvements2 | 8;
          }
          result = dlg_check_get(2);
          if (result != 0) {
            improvements2 = improvements2 | 0x10;
          }
          result = dlg_check_get(3);
          if (result != 0) {
            improvements2 = improvements2 | 0x30;
          }
          result = dlg_check_get(4);
          if (result != 0) {
            improvements2 = improvements2 | 0x40;
          }
          result = dlg_check_get(5);
          if (result != 0) {
            improvements2 = improvements2 | 0x42;
          }
          result = dlg_check_get(6);
          if (result != 0) {
            improvements2 = improvements2 | 0x80;
            game.polluted_tiles++;
          }
          tile[1] = improvements2;
          for (civ = 1; civ < 8; civ++) {
            result = tile_visible_to(map_cursor.x[0],map_cursor.y[0],civ);
            if (result != 0) {
              tile_update_known(map_cursor.x[0],map_cursor.y[0],civ);
            }
          }
          map_redraw_area_all_views((int)map_cursor.x[0],(int)map_cursor.y[0],1,human_civ[0],1);
          return;
        }
      }
      dialog2.close();
      dialog2.init_defaults();
      dialog2.load_text((const char *)&str_debug_5,(const char *)&str_terrain1_2,1);
      for (terrain2 = 0; (int)terrain2 < TERRAIN_COUNT; terrain2++) {
        option_flags = 0;
        option_terrain = terrain2;
        x = (iptr)label_string(terrain_types[terrain2].name);
        dialog2.add_option((const char *)x,option_terrain,option_flags);
        if (old_terrain == terrain2) {
          dialog2.set_default_choice(terrain2);
        }
      }
      terrain2 = dialog2.show_modal(0);
      if ((int)terrain2 < 0) {
        /* cancelled */
      }
      else if (old_terrain != terrain2) {
        if (terrain2 == TERRAIN_OCEAN) {
          *tile = 10;
          min_landmass = 0x3f;
          for (dir = 0; dir < 8; dir++) {
            x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)map_cursor.x[0]);
            y2 = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)map_cursor.y[0];
            result = map_in_bounds(x,y2);
            if (((result != 0) && (result = tile_is_ocean(x,y2), result != 0)) &&
               (result = tile_landmass(x,y2), result < min_landmass)) {
              min_landmass = result;
            }
          }
          tile[3] = (byte)min_landmass;
        }
        else {
          *tile = *tile & 0x80;
          *tile = *tile | (byte)terrain2;
          if (old_terrain == 10) {
            min_landmass = 0x3f;
            for (dir = 0; dir < 8; dir++) {
              x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)map_cursor.x[0]);
              y2 = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)map_cursor.y[0];
              result = map_in_bounds(x,y2);
              if (((result != 0) && (result = tile_is_ocean(x,y2), result == 0)) &&
                 (result = tile_landmass(x,y2), result < min_landmass)) {
                min_landmass = result;
              }
            }
            tile[3] = (byte)min_landmass;
          }
        }
        map_redraw_tile_all_views((int)map_cursor.x[0],(int)map_cursor.y[0]);
      }
    }
  }
  return;
}

/* 005551b3 auto */
void __cdecl cheat_create_unit(void)

{
  iptr obsolete_known;
  iptr has_prereq;
  int unit;
  iptr civ_name;
  i32 option_flags;
  iptr option_civ;
  iptr unit_type;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  if (human_civ[0] != cheat_unit.last_human_civ) {
    cheat_unit.last_human_civ = human_civ[0];
    cheat_unit.owner_civ = 0xffffffff;
  }
  if ((int)cheat_unit.owner_civ < 0) {
    cheat_unit.owner_civ = human_civ[0];
  }
  while( true ) {
    dialog2.load_text((const char *)&str_debug_6,(const char *)&str_createunit_4,1);
    for (unit_type = 0; unit_type < UNIT_TYPE_COUNT; unit_type++) {
      if ((((unit_types[unit_type].obsolete_by < '\0') ||
           (obsolete_known = civ_has_advance(cheat_unit.owner_civ,unit_types[unit_type].obsolete_by),
           obsolete_known == 0)) || (cheat_unit.show_obsolete != '\0')) &&
         ((cheat_unit.ignore_prereq != '\0' ||
          (has_prereq = civ_has_advance(cheat_unit.owner_civ,unit_types[unit_type].prerequisite),
          has_prereq != 0)))) {
        text_clear();
        if (cheat_unit.veteran != '\0') {
          text_append_label(0xd);
          text_append_space();
        }
        text_append_string_id(unit_types[unit_type].name);
        dialog2.add_option((const char *)&textbuf.buf,unit_type,0);
        if (cheat_unit.last_type < 0) {
          cheat_unit.last_type = unit_type;
        }
      }
    }
    if (-1 < cheat_unit.last_type) {
      dialog2.set_default_choice(cheat_unit.last_type);
    }
    unit = dialog2.show_modal(0);
    cheat_unit.last_type = unit;
    if (unit < 0) break;
    switch(dialog2.result_extra) {
    case 1:
      dialog2.Dialog::reset(1);
      for (cheat_unit.owner_civ = 0; (int)cheat_unit.owner_civ < 8; cheat_unit.owner_civ++) {
        if ((cheat_unit.owner_civ == 0) || ((1 << ((byte)cheat_unit.owner_civ & 0x1f) & (uint)game.civs_alive) != 0)
           ) {
          option_flags = 0;
          option_civ = cheat_unit.owner_civ;
          civ_name = (iptr)(civ_name_plural(cheat_unit.owner_civ));
          dialog2.add_option((const char *)civ_name,option_civ,option_flags);
        }
      }
      cheat_unit.owner_civ = dialog2.show_modal(0);
      if ((int)cheat_unit.owner_civ < 0) {
        return;
      }
      /* fall through */
    default:
      unit = unit_create(unit,cheat_unit.owner_civ,map_cursor.x[0],map_cursor.y[0]);
      if ((-1 < unit) && (cheat_unit.veteran != '\0')) {
        units[unit].flags = units[unit].flags | UNITF_VETERAN;
      }
      map_redraw_tile_all_views((int)map_cursor.x[0],(int)map_cursor.y[0]);
      blink.unit_active = 0;
      return;
    case 2:
      cheat_unit.veteran = cheat_unit.veteran == '\0';
      cheat_unit.last_type = -1;
      break;
    case 3:
      cheat_unit.show_obsolete = cheat_unit.show_obsolete == '\0';
      cheat_unit.last_type = -1;
      break;
    case 4:
      cheat_unit.ignore_prereq = cheat_unit.ignore_prereq == '\0';
      cheat_unit.last_type = -1;
      break;
    }
  }
  return;
}

/* 0055560f auto */
void __cdecl cheat_reveal_map(void)

{
  iptr label;
  int choice;
  iptr option_id;
  i32 option_flags;
  int civ;
  i8 view_civ_byte;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  dialog2.load_text((const char *)&str_debug_7,(const char *)&str_revealmap,1);
  for (civ = 0; civ < 8; civ++) {
    text_clear();
    label = civ_get_adjective(civ);
    text_append((const char *)label);
    dialog2.add_option((const char *)&textbuf.buf,civ,0);
  }
  option_flags = 0;
  option_id = 0x62;
  label = (iptr)label_string(label_table.string_ids[0x4f]);
  dialog2.add_option((const char *)label,option_id,option_flags);
  option_flags = 0;
  option_id = 99;
  label = (iptr)label_string(label_table.string_ids[0x4e]);
  dialog2.add_option((const char *)label,option_id,option_flags);
  if ((game.reveal_map == '\0') && (game.view_civ < '\0')) {
    dialog2.set_default_choice(0x62);
  }
  else {
    dialog2.set_default_choice(99);
  }
  choice = dialog2.show_modal(0);
  if (choice >= 0) {
    if (choice == 0x62) {
      game.reveal_map = 1;
      game.view_civ = -1;
    }
    else if (choice == 99) {
      game.reveal_map = '\0';
      game.view_civ = -1;
    }
    else {
      game.reveal_map = '\0';
      view_civ_byte = (i8)choice;
      game.view_civ = view_civ_byte;
    }
    if (-1 < game.view_civ) {
      human_civ[0] = (iptr)game.view_civ;
      game.human_players = (u8)(1 << (game.view_civ & 0x1fU));
    }
    ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
    map_redraw_all_views(human_civ[0],1);
  }
  return;
}

/* 0055583f auto */
void __cdecl cheat_set_human_player(void)

{
  iptr label;
  int choice;
  iptr option_id;
  i32 option_flags;
  int civ;
  byte civ_byte;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  dialog2.load_text((const char *)&str_debug_8,(const char *)&str_sethuman,1);
  for (civ = 1; civ < 8; civ++) {
    if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
      text_clear();
      label = civ_get_adjective(civ);
      text_append((const char *)label);
      dialog2.add_option((const char *)&textbuf.buf,civ,0);
    }
  }
  option_flags = 0;
  option_id = 99;
  label = (iptr)label_string(label_table.string_ids[0x50]);
  dialog2.add_option((const char *)label,option_id,option_flags);
  choice = dialog2.show_modal(0);
  if (choice >= 0) {
    if (choice == 99) {
      game.human_players = '\0';
      player_turn_active = 0;
    }
    else {
      civ_byte = (byte)choice;
      game.human_players = (u8)(1 << (civ_byte & 0x1f));
      game.player_civ = civ_byte;
      game.turn_civ = civ_byte;
      human_civ[0] = choice;
      map_redraw_all_views(choice,1);
      ui_redraw_all_maps(1);
    }
  }
  return;
}

/* 00555a02 auto */
void __cdecl cheat_set_game_year(void)

{
  int result;
  i32 year;
  short turn_value[2];
  
  text_set_number_arg(0,(int)game.turn);
  result = dlg_input_number(str_debug_11,str_gameyear,(int)game.turn,(int *)turn_value);
  if (result == 0) {
    game.turn = turn_value[0];
    year = game_turn_to_year((int)turn_value[0]);
    game.year = (i16)year;
    status_redraw(1);
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
  }
  return;
}

/* 00555a8b auto */
void __cdecl cheat_kill_civ(void)

{
  iptr adjective;
  int civ;
  int civ2;
  uptr city;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  dialog2.load_text((const char *)&str_debug_9,(const char *)&str_killciv,1);
  for (civ2 = 1; civ2 < 8; civ2++) {
    if ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0) {
      text_clear();
      adjective = civ_get_adjective(civ2);
      text_append((const char *)adjective);
      dialog2.add_option((const char *)&textbuf.buf,civ2,0);
    }
  }
  civ = dialog2.show_modal(0);
  if (civ >= 0) {
    (*((char *)&civ2 + 0)) = (byte)civ;
    game.human_players = game.human_players & ~(byte)(1 << ((byte)civ2 & 0x1f));
    if (2 < game.multiplayer) {
      net_send_message(NM_UNLOCK_NATION,0,0,civ,0,0,0,0,0,0);
    }
    city = (uptr)game.num_cities;
    while (city = city - 1, -1 < (int)city) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        city_delete(city,0);
      }
    }
    civ_check_destroyed(civ,human_civ[0]);
  }
  return;
}

/* 00555cb1 auto */
void __cdecl cheat_destroy_units_at_cursor(void)

{
  iptr unit;
  
  unit = tile_first_unit((int)map_cursor.x[0],(int)map_cursor.y[0]);
  unit_stack_delete_all(unit,1);
  return;
}

/* 0055615c auto */
void __cdecl cheat_change_money(void)

{
  iptr civ;
  iptr adjective;
  int result;
  int gold2;
  
  civ = dbg_pick_player(0);
  if (-1 < civ) {
    adjective = civ_get_adjective(civ);
    text_set_string_arg(0,(const char *)adjective);
    text_set_number_arg_v(0,civs[civ].gold);
    result = dlg_input_number(str_debug_11,str_money,civs[civ].gold,&gold2);
    if (result == 0) {
      if (extras_gold_limit() < gold2) {
        gold2 = extras_gold_limit();  /* X52: 30000 unless set */
      }
      if (gold2 < 0) {
        gold2 = 0;
      }
      civs[civ].gold = gold2;
      ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
      status_redraw(1);
    }
  }
  return;
}

/* 0055625b auto */
void __cdecl cheat_edit_unit(void)

{
  char civ;
  int choice;
  iptr label;
  iptr option_id;
  i32 arg;
  iptr commodity_opt;
  iptr unit;
  u8 byte_value;
  iptr city;
  int city_count;
  u8 damage_input[4];
  iptr commodity;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  choice = map_in_bounds(map_cursor.x[0],map_cursor.y[0]);
  if (choice != 0) {
    unit = tile_first_unit((int)map_cursor.x[0],(int)map_cursor.y[0]);
    if (unit >= 0) {
      choice = unit_stack_sum(unit,2);
      if (1 < choice) {
        arg = 1;
        label = (iptr)label_string(label_table.string_ids[0x3e]);
        unit = unit_stack_select_dialog(unit,(const char *)label,arg);
        if (unit < 0) {
          return;
        }
      }
      civ = units[unit].owner;
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                while( true ) {
                  label = civ_get_adjective(civ);
                  text_set_string_arg(0,(const char *)label);
                  text_clear();
                  text_append_string_id(unit_types[units[unit].type].name);
                  if ((units[unit].flags & UNITF_VETERAN) != 0) {
                    text_append_space();
                    text_append_lparen();
                    text_append_label(0xd);
                    text_append_rparen();
                  }
                  text_append_space();
                  text_append_lparen();
                  if (units[unit].home_city == -1) {
                    text_append_label(0xe);
                  }
                  else {
                    text_append((const char *)((uint)(byte)units[unit].home_city * 0x58 + (iptr)((u8 *)&cities + 32)));
                  }
                  text_append_rparen();
                  text_set_string_arg(1,(const char *)&textbuf.buf);
                  choice = popup_game((const char *)&str_unitedit);
                  if (choice == 0) {
                    return;
                  }
                  if (choice != 1) break;
                  units[unit].flags = units[unit].flags ^ UNITF_VETERAN;
                  map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
                  status_redraw(1);
                }
                if (choice != 2) break;
                units[unit].moves_used = '\0';
                map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
                status_redraw(1);
              }
              if (choice != 3) break;
              arg = unit_max_hp(unit);
              text_set_number_arg(0,arg);
              choice = dlg_input_number_game((const char *)&str_unithitpoints,(uint)units[unit].damage,(int *)damage_input);
              if (choice == 0) {
                units[unit].damage = damage_input[0];
              }
              map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
              status_redraw(1);
            }
            if (choice != 4) break;
            city_count = 0;
            dialog2.load_game_section((const char *)&str_edithomecity,1);
            for (city = 0; city < game.num_cities; city++) {
              if ((cities[city].id != 0) && ((int)cities[city].owner == (int)civ)) {
                dialog2.add_option((const char *)&cities[city].name,city,0);
                city_count++;
              }
            }
            if ((city_count != 0) &&
               (choice = dialog2.show_modal(0), -1 < choice)) {
              if (dialog2.result_extra == 0) {
                byte_value = (u8)choice;
                units[unit].home_city = byte_value;
              }
              else {
                units[unit].home_city = -1;
              }
              map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
              status_redraw(1);
            }
          }
          if (choice != 5) break;
          if (unit_types[units[unit].type].domain == DOMAIN_LAND) {
            if (units[unit].orders == ORDER_FORTIFIED) {
              units[unit].orders = ORDER_NONE;
            }
            else {
              units[unit].orders = ORDER_FORTIFIED;
            }
            map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
            status_redraw(1);
          }
        }
        if (choice != 6) break;
        if (unit_types[units[unit].type].role == ROLE_TRADE) {
          dialog2.load_game_section((const char *)&str_supplysearch_2,1);
          for (commodity = 0; commodity < 0x10; commodity++) {
            arg = 0;
            commodity_opt = commodity;
            label = (iptr)label_string(commodity_names[commodity]);
            dialog2.add_option((const char *)label,commodity_opt,arg);
            if ((char)units[unit].counter == commodity) {
              dialog2.set_default_choice(commodity);
            }
          }
          arg = 0;
          option_id = 0x10;
          label = (iptr)label_string(label_table.string_ids[0xc0]);
          dialog2.add_option((const char *)label,option_id,arg);
          if ((char)units[unit].counter < '\0') {
            dialog2.set_default_choice(0x10);
          }
          choice = dialog2.show_modal(0);
          if (-1 < choice) {
            if (choice < 0x10) {
              byte_value = (u8)choice;
              units[unit].counter = byte_value;
            }
            else {
              units[unit].counter = 0xff;
            }
            map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
            status_redraw(1);
          }
        }
      }
    }
  }
  return;
}

/* 005569e3 auto */
void __cdecl cheat_edit_city(void)

{
  char civ;
  bool has_other;
  iptr in_bounds;
  iptr city;
  iptr adjective;
  int choice;
  i32 on;
  iptr wonder;
  i8 number_input[4];
  iptr improvement;
  iptr city2;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  in_bounds = map_in_bounds(map_cursor.x[0],map_cursor.y[0]);
  if (in_bounds != 0) {
    city = city_at((int)map_cursor.x[0],map_cursor.y[0]);
    if (city >= 0) {
      civ = cities[city].owner;
      while( true ) {
        text_set_number_arg(0,(cities[city].flags & CITYF_OBJECTIVE) >> 0x1a);
        text_clear();
        text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
        text_append_space();
        text_append_lparen();
        adjective = civ_get_adjective(civ);
        text_append((const char *)adjective);
        text_append_rparen();
        text_set_string_arg(0,(const char *)&textbuf.buf);
        choice = popup_game((const char *)&str_cityedit);
        if (choice == 0) break;
        if (choice == 1) {
          choice = dlg_input_number_game((const char *)&str_setcitysize,(int)cities[city].size,(int *)number_input);
          if (choice == 0) {
            cities[city].size = number_input[0];
          }
          map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
          status_redraw(1);
        }
        else if (choice == 2) {
          for (wonder = 0; wonder < WONDER_COUNT; wonder++) {
            if (game.wonder_city[wonder] == city) {
              game.wonder_city[wonder] = -1;
            }
          }
          map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
          status_redraw(1);
        }
        else if (choice == 3) {
          cities[city].flags = cities[city].flags & ~(CITYF_DISORDER | CITYF_CELEBRATING);
          map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
        }
        else if (choice == 4) {
          dialog2.load_game_section((const char *)&str_copycity,1);
          has_other = false;
          for (city2 = 0; city2 < game.num_cities; city2++) {
            if ((cities[city2].id != 0) && (city2 != city)) {
              dialog2.add_option((const char *)&cities[city2].name,city2,0);
              has_other = true;
            }
          }
          if ((has_other) &&
             (city2 = dialog2.show_modal(0), -1 < city2)) {
            on = city_has_improvement(city,IMP_PALACE);
            for (improvement = 0; improvement < 5; improvement++) {
              *(i8 *)&cities[city].improvements[improvement] =
                   *(i8 *)&cities[city2].improvements[improvement];
            }
            city_set_improvement(city,1,on);
            ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
          }
        }
        else if (choice == 5) {
          choice = dlg_input_number_game((const char *)&str_setcityshields,(int)cities[city].shield_box,(int *)number_input);
          if (choice == 0) {
            cities[city].shield_box = (*(i16 *)((char *)&number_input + 0));
          }
          map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
          status_redraw(1);
          ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
        }
        else {
          if (choice != 6) {
            return;
          }
          cities[city].flags = cities[city].flags ^ CITYF_OBJECTIVE;
        }
      }
    }
  }
  return;
}

/* 00556f54 auto */
void __cdecl cheat_edit_king(void)

{
  uint *treaty_ptr;
  iptr civ;
  char *name2;
  int choice;
  i32 value;
  iptr advance;
  iptr gender;
  u8 number_input[4];
  iptr civ2;
  iptr gov;
  int other_civ;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  civ = dbg_pick_player(0);
  if (civ >= 0) {
    while( true ) {
      text_set_number_arg(0,(civs[civ].flags & CIVF_FEMALE_LEADER) >> 9);
      text_clear();
      name2 = civ_leader_name(civ);
      text_append(name2);
      text_set_string_arg(0,(const char *)&textbuf.buf);
      choice = popup_game((const char *)&str_editking);
      if (choice == 0) break;
      if (choice == 1) {
        civ2 = dbg_pick_player(0);
        if (-1 < civ2) {
          name2 = civ_leader_name(civ2);
          text_set_string_arg(1,name2);
          dlg_checks_clear();
          dlg_check_set(0,civs[civ].treaties[civ2] & TREATY_CONTACT);
          dlg_check_set(1,(civs[civ].treaties[civ2] & TREATY_CEASEFIRE) >> 1);
          dlg_check_set(2,(civs[civ].treaties[civ2] & TREATY_PEACE) >> 2);
          dlg_check_set(3,(civs[civ].treaties[civ2] & TREATY_ALLIANCE) >> 3);
          dlg_check_set(4,(civs[civ].treaties[civ2] & TREATY_WAR) >> 0xd)
          ;
          dlg_check_set(5,(civs[civ].treaties[civ2] & TREATY_VENDETTA) >> 4);
          dlg_check_set(6,(civs[civ].treaties[civ2] & TREATY_EMBASSY) >> 7);
          choice = popup_game_flags((const char *)&str_edittreaties,1);
          if (-1 < choice) {
            diplo_clear_treaty_flags(civ,civ2,0x60);
            choice = dlg_check_get(0);
            if (choice == 0) {
              diplo_clear_treaty_flags(civ,civ2,1);
            }
            else {
              diplo_set_treaty_flags(civ,civ2,1);
            }
            choice = dlg_check_get(4);
            if (choice == 0) {
              diplo_clear_treaty_flags(civ,civ2,0x200e);
              choice = dlg_check_get(1);
              if (choice != 0) {
                diplo_set_treaty_flags(civ,civ2,2);
              }
              choice = dlg_check_get(2);
              if (choice != 0) {
                diplo_set_treaty_flags(civ,civ2,4);
              }
              choice = dlg_check_get(3);
              if (choice != 0) {
                diplo_set_treaty_flags(civ,civ2,8);
              }
            }
            else {
              diplo_set_treaty_flags(civ,civ2,0x2000);
            }
            treaty_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
            *treaty_ptr = *treaty_ptr & 0xffffff6f;
            choice = dlg_check_get(5);
            if (choice != 0) {
              treaty_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
              *treaty_ptr = *treaty_ptr | 0x10;
            }
            choice = dlg_check_get(6);
            if (choice != 0) {
              treaty_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
              *treaty_ptr = *treaty_ptr | 0x80;
            }
          }
        }
      }
      else if (choice == 2) {
        civ2 = dbg_pick_player(0);
        if (-1 < civ2) {
          name2 = civ_leader_name(civ2);
          text_set_string_arg(1,name2);
          text_set_number_arg(0,(int)game.turn);
          choice = dlg_input_number_game((const char *)&str_lastcontact,(int)civs[civ].last_contact_turn[civ2],(int *)number_input);
          if (choice == 0) {
            civs[civ].last_contact_turn[civ2] = (*(i16 *)((char *)&number_input + 0));
          }
        }
      }
      else if (choice == 3) {
        civ2 = dbg_pick_player(0);
        if (-1 < civ2) {
          name2 = civ_leader_name(civ2);
          text_set_string_arg(1,name2);
          choice = dlg_input_number_game((const char *)&str_editattitude,(uint)civs[civ].attitude[civ2],
                                     (int *)number_input);
          if (choice == 0) {
            civs[civ].attitude[civ2] = number_input[0];
          }
        }
      }
      else if (choice == 4) {
        choice = dlg_input_number_game((const char *)&str_editbetray,(uint)civs[civ].reputation,(int *)number_input);
        if (choice == 0) {
          civs[civ].reputation = number_input[0];
        }
      }
      else if (choice == 5) {
        civs[civ].diplo_request_count = '\0';
      }
      else if (choice == 6) {
        for (other_civ = 0; other_civ < 8; other_civ++) {
          for (civ2 = 0; civ2 < 8; civ2++) {
            civs[civ2].last_contact_turn[other_civ] = game.turn;
          }
        }
      }
      else if (choice == 7) {
        civ_choose_research_dialog(civ,1);
      }
      else if (choice == 8) {
        value = civ_research_cost(civ);
        text_set_number_arg(0,value);
        choice = dlg_input_number_game((const char *)&str_editprogress,(int)civs[civ].research_progress,(int *)number_input);
        if (choice == 0) {
          civs[civ].research_progress = (*(i16 *)((char *)&number_input + 0));
          status_redraw(1);
        }
      }
      else if (choice == 9) {
        civs[civ].researching = -1;
      }
      else if (choice == 10) {
        text_clear();
        name2 = civ_name_plural(civ);
        text_append(name2);
        text_set_string_arg(0,(const char *)&textbuf.buf);
        if (-1 < leaders[civs[civ].tribe].leader_name) {
          value = (i32)label_string((int)leaders[civs[civ].tribe].leader_name);
          crt_strcpy((char *)(&civ_names[civ].leader_name),(const char *)((const void *)value));
          leaders[civs[civ].tribe].leader_name = -leaders[civs[civ].tribe].leader_name;
        }
        if (-1 < leaders[civs[civ].tribe].plural) {
          value = (i32)label_string((int)leaders[civs[civ].tribe].plural);
          crt_strcpy((char *)(&civ_names[civ].tribe_name),(const char *)((const void *)value));
          leaders[civs[civ].tribe].plural = -leaders[civs[civ].tribe].plural;
        }
        if (-1 < leaders[civs[civ].tribe].adjective) {
          value = (i32)label_string((int)leaders[civs[civ].tribe].adjective);
          crt_strcpy((char *)(&civ_names[civ].adjective),(const char *)((const void *)value));
          leaders[civs[civ].tribe].adjective = -leaders[civs[civ].tribe].adjective;
        }
        for (gov = 0; gov < 7; gov++) {
          for (gender = 0; gender < 2; gender++) {
            leaders[civs[civ].tribe].titles[gov][gender] =
                 (short)(&government_titles[gov].male)[gender];
          }
        }
        text_set_string_arg(1,(const char *)&civ_names[civ].tribe_name);
        text_set_string_arg(2,(const char *)&civ_names[civ].adjective);
        dialog2.load_text_section((const char *)&str_game_7,(const char *)&str_editkingname,0x17,(const char*)&civ_names[civ].leader_name,0,0,0,1);
        choice = dialog2.show_modal(0);
        if (-1 < choice) {
          crt_strcpy((char *)(&civ_names[civ].leader_name),((i8 *)&text_args.strings));
          crt_strcpy((char *)(&civ_names[civ].tribe_name),((i8 *)&text_args.string1));
          crt_strcpy((char *)(&civ_names[civ].adjective),((i8 *)&text_args.string2));
        }
      }
      else if (choice == 0xb) {
        civ2 = dbg_pick_player(0);
        if (0 < civ2) {
          for (advance = 0; advance < 0xd; advance++) {
            *(i8 *)&civs[civ].advances[advance] =
                 *(i8 *)&civs[civ2].advances[advance];
          }
          civs[civ].num_advances = '\0';
          for (advance = 0; advance < ADV_COUNT; advance++) {
            choice = civ_has_advance(civ,advance);
            if (choice != 0) {
              civs[civ].num_advances++;
            }
          }
        }
      }
      else {
        if (choice != 0xc) {
          return;
        }
        civs[civ].flags = civs[civ].flags ^ CIVF_FEMALE_LEADER;
        if ((civs[civ].flags & CIVF_FEMALE_LEADER) == 0) {
          leaders[civs[civ].tribe].female = '\0';
        }
        else {
          leaders[civs[civ].tribe].female = 1;
        }
        civs[civ].leader_portrait = (u8)civs[civ].tribe;
        if (leaders[civs[civ].tribe].female != '\0') {
          civs[civ].leader_portrait += 21;
        }
      }
    }
  }
  return;
}

/* 00557e2c auto */
void __cdecl cheat_edit_victory(void)

{
  iptr value;
  iptr owned_value;
  iptr name2;
  int choice;
  int result;
  iptr option_id;
  i32 option_flags;
  int civ;
  iptr city;
  i8 number_input[4];
  int total_value;
  int objective_value;
  i8 *seh_scope;
  
  Dialog dialog2(0x4000);
  while( true ) {
    total_value = 0;
    objective_value = 0;
    for (city = 0; city < game.num_cities; city++) {
      if (cities[city].id != 0) {
        value = city_objective_value(city);
        total_value += value;
        if ((int)cities[city].owner == (int)scenario.objective_civ) {
          owned_value = city_objective_value(city);
          objective_value += owned_value;
        }
      }
    }
    text_clear();
    text_append_int(total_value);
    text_append((const char *)&str_empty_171);
    text_append_int(objective_value);
    text_append((const char *)&str_empty_172);
    text_set_string_arg(1,(const char *)&textbuf.buf);
    name2 = (iptr)(civ_name_plural(scenario.objective_civ));
    text_set_string_arg(0,(const char *)name2);
    text_set_number_arg(0,((int)(short)scenario.flags[0] & 2U) >> 1);
    text_set_number_arg(1,((int)(short)scenario.flags[0] & 4U) >> 2);
    text_set_number_arg(2,(int)scenario.victory_thresholds[0]);
    text_set_number_arg(3,(int)scenario.marginal_victory_score);
    text_set_number_arg(4,(int)scenario.marginal_defeat_score);
    text_set_number_arg(5,(int)scenario.decisive_defeat_score);
    choice = popup_game((const char *)&str_editvictory);
    if (choice < 1) break;
    if (choice == 1) {
      scenario.flags[0] = scenario.flags[0] ^ 2;
    }
    else if (choice == 2) {
      scenario.flags[0] = scenario.flags[0] ^ 4;
    }
    else if (choice == 3) {
      dialog2.Dialog::reset(1);
      option_flags = 0;
      option_id = 0;
      name2 = (iptr)label_string(label_table.string_ids[0xe]);
      dialog2.add_option((const char *)name2,option_id,option_flags);
      for (civ = 1; civ < 8; civ++) {
        option_flags = 0;
        choice = civ;
        name2 = (iptr)(civ_name_plural(civ));
        dialog2.add_option((const char *)name2,choice,option_flags);
      }
      choice = dialog2.show_modal(0);
      if (-1 < choice) {
        scenario.objective_civ = (short)choice;
      }
    }
    else {
      if ((choice < 4) || (7 < choice)) {
        return;
      }
      result = dlg_input_number_game((const char *)&str_editvictoryobj,(int)(((i16 *)&scenario.year_increment))[choice],(int *)number_input);
      if (result == 0) {
        (((i16 *)&scenario.year_increment))[choice] = (*(i16 *)((char *)&number_input + 0));
      }
    }
  }
  return;
}

/* 0055819b auto */
void __cdecl cheat_edit_scenario_rules(void)

{
  int choice;
  
  while( true ) {
    while( true ) {
      while( true ) {
        while( true ) {
          text_set_number_arg(0,((int)(short)scenario.flags[0] & 0x10U) >> 4);
          text_set_number_arg(1,((int)(short)scenario.flags[0] & 0x20U) >> 5);
          text_set_number_arg(2,((int)(short)scenario.flags[0] & 0x40U) >> 6);
          text_set_number_arg(3,((int)(short)scenario.flags[0] & 0x8000U) >> 0xf);
          choice = popup_game((const char *)&str_editrules);
          if (choice == 0) {
            return;
          }
          if (choice != 1) break;
          scenario.flags[0] = scenario.flags[0] ^ 0x10;
        }
        if (choice != 2) break;
        scenario.flags[0] = scenario.flags[0] ^ 0x20;
      }
      if (choice != 3) break;
      scenario.flags[0] = scenario.flags[0] ^ 0x40;
    }
    if (choice != 4) break;
    scenario.flags[0] = scenario.flags[0] ^ 0x8000;
  }
  return;
}

/* 005582ad auto */
void __cdecl cheat_scenario_params(void)

{
  i32 year;
  char *label;
  iptr choice;
  iptr result;
  int value;
  uint owner;
  iptr player;
  iptr city;
  i8 name_buf[260];
  Tile *tile_ptr;
  int known_ptrs[8];
  Tile *tile;
  i8 number_input[4];
  int tile_index;
  
  while( true ) {
    year = game_turn_to_year((int)game.turn);
    game.year = (i16)year;
    status_redraw(1);
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    text_set_number_arg(0,(int)scenario.tech_paradigm);
    text_set_number_arg(1,(int)scenario.year_increment[0]);
    text_set_number_arg(2,(int)scenario.start_year);
    text_set_number_arg(3,(int)scenario.max_turns);
    text_set_number_arg(4,(int)(short)scenario.flags[0] & 1);
    if ((game.game_flags & GAME_SCENARIO) == 0) {
      label = label_string(label_table.string_ids[0x1a3]);
      text_set_string_arg(0,label);
    }
    else {
      label = label_string(label_table.string_ids[0x1a2]);
      text_set_string_arg(0,label);
    }
    text_set_string_arg(1,(const char *)&scenario.name);
    choice = popup_game((const char *)&str_editscen);
    if (choice == 0) break;
    if (choice == 1) {
      result = dlg_input_number_game((const char *)&str_editparadigm,(int)scenario.tech_paradigm,(int *)number_input);
      if (result == 0) {
        scenario.tech_paradigm = (*(i16 *)((char *)&number_input + 0));
      }
    }
    else if (choice == 2) {
      value = dlg_input_number_game((const char *)&str_editincrement,(int)scenario.year_increment[0],(int *)number_input);
      if (value == 0) {
        scenario.year_increment[0] = (*(i16 *)((char *)&number_input + 0));
      }
    }
    else if (choice == 3) {
      value = dlg_input_number_game((const char *)&str_editstartyear,(int)scenario.start_year,(int *)number_input);
      if (value == 0) {
        scenario.start_year = (*(i16 *)((char *)&number_input + 0));
      }
    }
    else if (choice == 4) {
      value = dlg_input_number_game((const char *)&str_editmaxturns,(int)scenario.max_turns,(int *)number_input);
      if (value == 0) {
        scenario.max_turns = (*(i16 *)((char *)&number_input + 0));
      }
    }
    else if (choice == 5) {
      tile = map_data.tiles;
      for (tile_index = 0; tile_index < map.area; tile_index++) {
        if ((tile->owner & 0xf0) == 0xf0) {
          tile->owner = tile->owner & 0xf;
        }
        tile++;
      }
      map_redraw_all_views(human_civ[0],1);
    }
    else if (choice == 6) {
      tile = map_data.tiles;
      for (tile_index = 0; tile_index < map.area; tile_index++) {
        if (((tile->owner & 0xf0) != 0xf0) && ((tile->improvements & (TILE_UNIT | TILE_CITY)) == 0)) {
          tile->owner = tile->owner | 0xf0;
        }
        tile++;
      }
      map_redraw_all_views(human_civ[0],1);
    }
    else if (choice == 7) {
      tile_ptr = tile_at(0,0);
      for (player = 1; player < 8; player++) {
        value = (int)known_at(0,0,player);
        known_ptrs[player] = value;
      }
      for (tile_index = 0; tile_index < map.area; tile_index++) {
        tile_ptr->visible = 0xff;
        for (player = 1; player < 8; player++) {
          *(i8 *)known_ptrs[player] = tile_ptr->improvements;
          known_ptrs[player]++;
        }
        tile_ptr++;
      }
      for (city = 0; city < game.num_cities; city++) {
        if (cities[city].id != 0) {
          cities[city].known_to = 0xff;
          for (player = 0; player < 8; player++) {
            *(i8 *)&cities[city].revealed_size[player] = cities[city].size;
          }
          value = (int)tile_at(cities[city].x,cities[city].y);
          ((Tile *)value)->visible = 0xff;
        }
      }
      scenario.flags[0] = scenario.flags[0] | 8;
      map_redraw_all_views(human_civ[0],1);
    }
    else if (choice == 8) {
      tile_ptr = tile_at(0,0);
      for (tile_index = 0; tile_index < map.area; tile_index++) {
        tile_ptr->visible = 0;
        if (((tile_ptr->improvements & (TILE_UNIT | TILE_CITY)) != 0) &&
           (owner = (int)(uint)tile_ptr->owner >> 4, owner < 8)) {
          (*((char *)&player + 0)) = (sbyte)owner;
          tile_ptr->visible = (char)(1 << (sbyte)player);
        }
        tile_ptr++;
      }
      scenario.flags[0] = scenario.flags[0] & 0xfff7;
      map_redraw_all_views(human_civ[0],1);
    }
    else if (choice == 9) {
      value = dlg_game_text_input(str_scenname,0x18,scenario.name,name_buf);
      if (value == 0) {
        crt_strcpy(((i8 *)&scenario.name),name_buf);
      }
    }
    else if (choice == 10) {
      scenario.flags[0] = scenario.flags[0] ^ 1;
    }
    else if (choice == 0xb) {
      cheat_edit_victory();
    }
    else {
      if (choice != 0xc) {
        return;
      }
      cheat_edit_scenario_rules();
    }
  }
  return;
}

/* 0055891d auto */
void __cdecl cheat_save_as_scenario(void)

{
  game_save_dialog(1);
  return;
}
