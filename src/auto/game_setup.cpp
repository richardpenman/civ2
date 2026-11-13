/* The game, setting up a game: the new-game, scenario, hot-seat, network and play-by-email dialogs: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00419170 auto */
i32 __cdecl setup_hotseat_game(void)

{
  int choice;
  i32 result;
  GamePalette palette;
  int i;
  Dialog dlg;
  i32 map_source;
  int num_players;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  for (i = 0; i < 8; i++) {
    civ_view.intro_shown[i] = 0;
  }
  civ_saved_cursor_x_block.mp_turn_time_limit_ms = 0;
  game.multiplayer = 1;
  for (;;) {
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    wnd_flush_paint();
    setup_dlg.dialog_pos_state = 3;
    setup_show_side_picture(0xffffffff,0);
    dlg.load_game_section((const char *)&str_hotseat1,1);
    dlg.set_default_choice((int)menu_prefs.hotseat_menu_choice);
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
    choice = -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1);
    result = setup_next_dialog_x(0);
    dlg.set_position(result,choice);
    choice = dlg.show_modal(0);
    snd_play_effect(SND_MENUOK,0,0,0);
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    if (choice < 0) {
      result = 1;
      return result;
    }
    if (4 < choice) {  /* BF-075 (Peter Johnson's ledger): a choice past the five
                          the game knows (a row added to GAME.TXT's @HOTSEAT1) is
                          asked again (the original went on with no map set) */
      continue;
    }
    menu_prefs.hotseat_menu_choice = (short)choice;
    if ((-1 < choice) && (choice < 3)) {
      __chdir(((i8 *)&game_dirs.home));
      rules_load(1);
    }
    scenario_events_loaded = 0;
    switch(choice) {
    case 0:
      choice = setup_choose_map_size();
      if (choice != 0) continue;
      map_source = 0;
      choice = _rand();
      mapgen.land_mass = choice % 3 - 1;
      mapgen.land_form = 0;
      choice = _rand();
      if (choice % 3 == 0) {
        mapgen.land_form = mapgen.land_mass;
      }
      choice = _rand();
      mapgen.temperature = choice % 3 - 1;
      choice = _rand();
      mapgen.climate = choice % 3 - 1;
      choice = _rand();
      mapgen.age = choice % 3 - 1;
      choice = setup_choose_game_options(map_source,1);
      if (choice != 0) continue;
      break;
    case 1:
      choice = map_load_dialog();
      if (choice != 0) continue;
      map_source = 1;
      if (map.flat != 0) {
        map_source = 2;
      }
      choice = _rand();
      mapgen.land_mass = choice % 3 - 1;
      mapgen.land_form = 0;
      choice = _rand();
      if (choice % 3 == 0) {
        mapgen.land_form = mapgen.land_mass;
      }
      choice = _rand();
      mapgen.temperature = choice % 3 - 1;
      choice = _rand();
      mapgen.climate = choice % 3 - 1;
      choice = _rand();
      mapgen.age = choice % 3 - 1;
      choice = setup_choose_game_options(map_source,1);
      if (choice != 0) continue;
      break;
    case 2:
      choice = setup_choose_map_size();
      if (choice != 0) continue;
      map_source = 0;
      choice = setup_custom_world();
      if (choice != 0) continue;
      choice = setup_choose_game_options(map_source,1);
      if (choice != 0) continue;
      break;
    case 3:
      game_palette_block.event_dont_play_wonders = 0;
      choice = scenario_start_dialog(1);
      if (choice == 0) {
        if (game_palette_block.event_dont_play_wonders != 0) {
          game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
        }
        game.multiplayer = 1;
        num_players = setup_choose_hotseat_players();
        if (num_players == 0) {
          crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
        }
        else {
          choice = scenario_pick_human_civs(num_players);
          if (choice == 0) {
            text_clear();
            text_append_label(0x363);
            ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(1,0x1f2,(const char *)&textbuf.buf,0);
            prefs_save_civ2dat();
            ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
            main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
            main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
            wnd_flush_paint();
            result = 0;
            return result;
          }
          crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
        }
      }
      else {
        crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
      }
      continue;
    case 4:
      game_palette_block.event_dont_play_wonders = 0;
      choice = game_load_dialog(0,1,1);
      if (choice == 0) {
        if (game.multiplayer != 1) {
          popup_game_with_sprite(str_hotseatnot,(Sprite *)&popup_info_icon_sprite,0);
          if (game_palette_block.event_dont_play_wonders != 0) {
            game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
          }
          continue;
        }
        text_clear();
        text_append_label(0x363);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(1,0x1f2,(const char *)&textbuf.buf,0);
        prefs_save_civ2dat();
        ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
        main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
        main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
        wnd_flush_paint();
        result = 0;
        return result;
      }
      if (game_palette_block.event_dont_play_wonders != 0) {
        game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
      }
      continue;
    default:
      break;
    }
    civ_saved_cursor_x_block.net_claimed_civs_mask = 0;
    for (;;) {  /* the number of players, again when the first player backs out */
      game.civs_alive = '\0';
      game.human_players = '\0';
      num_players = setup_choose_hotseat_players();
      if (num_players == 0) break;
      leaders_reset_custom_names();
      for (i = 0; i < num_players; i++) {
        while (choice = setup_choose_tribe(i,1,0), choice != 0) {
          i--;
          if (i < 0) break;
        }
        if (i < 0) break;
      }
      if (i < 0) continue;
      setup_choose_opponents(num_players);
      prefs_save_civ2dat();
      rules_load_advances(1);
      rules_load_units(1);
      rules_load_improvements(1);
      snd_menu_end();
      map_generate(map_source);
      game_init_new();
      if (setup_dlg.accelerated_start != 0) {
        game_accelerated_start(setup_dlg.accelerated_start - 1);
      }
      game.multiplayer = 1;
      if (turn_loop.resume_civ != '\0') {
        game.player_civ = turn_loop.resume_civ;
        human_civ[0] = (iptr)turn_loop.resume_civ;
      }
      text_clear();
      text_append_label(0x363);
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(1,0x1f2,(const char *)&textbuf.buf,0);
      result = 0;
      return result;
    }
  }
}

/* 00419c20 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl setup_backdrop_surface_global_init(void)

{
  setup_backdrop_surface_static_init();
  setup_backdrop_surface_reg_atexit();
  return;
}

/* 00419c3a auto */
void __cdecl setup_backdrop_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface));
  return;
}

/* 00419c54 auto */
void __cdecl setup_backdrop_surface_reg_atexit(void)

{
  _atexit(setup_backdrop_surface_atexit_dtor);
  return;
}

/* 00419c71 auto */
void __cdecl setup_backdrop_surface_atexit_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface));
  return;
}

/* 0041d417 auto */
i32 __cdecl setup_choose_map_size(void)

{
  int choice;
  i32 result;
  Dialog dlg;
  int rc;
  int chosen;
  
  result = 1;
  chosen = 0;
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  game.game_flags = 0;
  do {
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,2);
    dlg.load_game_section((const char *)&str_sizeofmap,1);
    dlg.set_default_choice((int)prefs.map_size);
    dlg.set_position(0xfffffc19,
               -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
    choice = dlg.show_modal(0);
    snd_play_effect(SND_MENUOK,0,0,0);
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    if (choice < 0) break;
    if (dlg.result_extra == 0) {
      prefs.map_size = (short)choice;
      if (choice == 0) {
        map.width = 0x28;
        map.height = 0x32;
      }
      else if (choice == 2) {
        map.width = 0x4b;
        map.height = 0x78;
      }
      else {
        map.width = 0x32;
        map.height = 0x50;
      }
      result = 0;
      break;
    }
    while( true ) {
      text_set_string_arg_b(1,(const char *)&str_80);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,2);
      rc = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_customsize,5,(const char*)&str_50,0,0,0,1)
      ;
      if (rc != 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        chosen = 1;
        break;
      }
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      choice = dlg.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (choice < 0) break;
      choice = _atoi(((i8 *)&text_args.strings));
      map.width = (i16)choice;
      choice = _atoi(((i8 *)&text_args.string1));
      map.height = (i16)choice;
      if ((((0x13 < map.width) && (map.width < 0xfb)) && (0x13 < map.height)) &&
         (((map.height < 0xfb && ((int)map.height * (int)map.width <= extras_map_tiles_limit())) &&  /* X53 */
          (999 < (int)map.height * (int)map.width)))) {
        result = 0;
        chosen = 1;
        break;
      }
    }
  } while (chosen == 0);  /* a cancelled custom size: back to the sizes */
  if (5999 < (int)map.height * (int)map.width) {
    game.game_flags = game.game_flags | GAME_LARGE_MAP;
  }
  if ((int)map.height * (int)map.width < 3000) {
    game.game_flags = game.game_flags | GAME_SMALL_MAP;
  }
  map.width = map.width * 2;
  return result;
}

/* 0041d7ea auto */
int __cdecl setup_custom_world(void)

{
  int choice;
  int result;
  Dialog dlg;
  
  result = 1;
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,2);
  dlg.load_game_section((const char *)&str_customland,1);
  dlg.set_default_choice(prefs.world_params[0] + 1);
  dlg.set_position(0xfffffc19,
             -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
  choice = dlg.show_modal(0);
  mapgen.land_mass = choice - 1;
  snd_play_effect(SND_MENUOK,0,0,0);
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
  if (-2 < mapgen.land_mass) {
    if (dlg.result_extra == 0) {
      prefs.world_params[0] = (short)mapgen.land_mass;
    }
    else {
      choice = _rand();
      mapgen.land_mass = choice % 3 - 1;
    }
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,0xc);
    dlg.load_game_section((const char *)&str_customform,1);
    dlg.set_default_choice(prefs.world_land_form + 1);
    dlg.set_position(0xfffffc19,
               -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
    choice = dlg.show_modal(0);
    mapgen.land_form = choice - 1;
    snd_play_effect(SND_MENUOK,0,0,0);
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    if (-2 < mapgen.land_form) {
      if (dlg.result_extra == 0) {
        prefs.world_land_form = (short)mapgen.land_form;
      }
      else {
        mapgen.land_form = 0;
        choice = _rand();
        if (choice % 3 == 0) {
          mapgen.land_form = mapgen.land_mass;
        }
      }
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,0xb);
      dlg.load_game_section((const char *)&str_customclimate,1);
      dlg.set_default_choice(prefs.world_climate + 1);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      choice = dlg.show_modal(0);
      mapgen.climate = choice - 1;
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (-2 < mapgen.climate) {
        if (dlg.result_extra == 0) {
          prefs.world_climate = (short)mapgen.climate;
        }
        else {
          choice = _rand();
          mapgen.climate = choice % 3 - 1;
        }
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,0xd);
        dlg.load_game_section((const char *)&str_customtemp,1);
        dlg.set_default_choice(prefs.world_temperature + 1);
        dlg.set_position(0xfffffc19,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1)
                  );
        choice = dlg.show_modal(0);
        mapgen.temperature = choice - 1;
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        if (-2 < mapgen.temperature) {
          if (dlg.result_extra == 0) {
            prefs.world_temperature = (short)mapgen.temperature;
          }
          else {
            choice = _rand();
            mapgen.temperature = choice % 3 - 1;
          }
          ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,10);
          dlg.load_game_section((const char *)&str_customage,1);
          dlg.set_default_choice(prefs.world_age + 1);
          dlg.set_position(0xfffffc19,
                     -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) +
                      1));
          choice = dlg.show_modal(0);
          mapgen.age = choice - 1;
          snd_play_effect(SND_MENUOK,0,0,0);
          ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
          if (-2 < mapgen.age) {
            if (dlg.result_extra == 0) {
              prefs.world_age = (short)mapgen.age;
            }
            else {
              choice = _rand();
              mapgen.age = choice % 3 - 1;
            }
            result = 0;
          }
        }
      }
    }
  }
  if (result == 0) {
    prefs_save_civ2dat();
  }
  return result;
}

/* 0041dcc3 auto */
void __cdecl setup_clear_start_positions(void)

{
  iptr i;
  
  for (i = 0; i < 0x15; i++) {
    start_positions.x[i] = -1;
    start_positions.y[i] = -1;
  }
  return;
}

/* 0041dfe1 auto */
int __cdecl setup_start_scenario(void)

{
  i32 year;
  iptr rc;
  iptr unit;
  int choice;
  char *name2;
  int civ;
  u8 is_female;
  iptr gender;
  i8 name_buf[260];
  GamePalette palette;
  int result;
  Dialog dlg;
  DialogOption *option;
  iptr gov;
  i8 dst[256];
  int level;
  int option_civs[8];
  int started;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  started = 0;
  do {
    if (cd_music.available == 0) {
      snd_cd_stop();
      snd_play_effect(SND_MENULOOP,0,1,0);
    }
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    result = game_load_dialog(1,0,1);
    if (result != 0) break;
    path_set_extension(dst,(char *)(&textbuf.buf),lang_file_ext);
    if (cd_music.available != 0) {
      snd_play_effect(SND_MENUOK,0,0,0);
    }
    game.game_options = prefs.game_options & 0xffff7fff;
    game.message_options = prefs.message_options;
    text_set_string_arg(0,(const char *)&scenario.name);
    text_set_number_arg(0,(int)scenario.tech_paradigm);
    text_clear();
    year = game_turn_to_year((int)game.turn);
    text_append_year(year);
    text_set_string_arg(1,(const char *)&textbuf.buf);
    text_clear();
    if (scenario.max_turns == 0) {
      text_append_year(2000);
    }
    else {
      year = game_turn_to_year((int)scenario.max_turns);
      text_append_year(year);
    }
    text_set_string_arg(2,(const char *)&textbuf.buf);
    if (cd_music.available != 0) {
      snd_menu_end();
    }
    dlg.init_default();
    rc = dlg.load_text((const char *)dst,(const char *)&str_scenario,0x4008001);
    if (rc == 0) {
      unit = (iptr)game.active_unit;
      if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
         ((int)units[unit].owner == (int)scenario.objective_civ)) {
        dlg.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),0
                   ,0);
      }
      choice = dlg.show_modal(0);
      if (choice < 0) continue;
    }
    else {
      choice = popup_game_flags((const char *)&str_scenarioloaded,1);
      if (choice < 0) continue;
    }
    dlg.Dialog::reset(1);
    for (civ = 1; civ < 8; civ++) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
        text_clear();
        name2 = civ_name_plural(civ);
        text_append(name2);
        text_append_space();
        text_append_lparen();
        name2 = civ_leader_name(civ);
        text_append(name2);
        text_append_rparen();
        option = (dlg.add_option((const char *)&textbuf.buf,civ,0));
        option_civs[(uint)option->id] = civ;
      }
    }
    choice = dlg.show_modal(0);
    if (-1 < choice) {
      saved_turn.civ = (i8)human_civ[0];
      turn_loop.resume_civ = (i8)human_civ[0];
      (*((char *)&civ + 0)) = (byte)option_civs[choice];
      game.human_players = (u8)(1 << ((byte)civ & 0x1f));
      game.player_civ = (byte)civ;
      game.turn_civ = (byte)civ;
      game.game_options = game.game_options & ~OPT_TUTORIAL;
      human_civ[0] = option_civs[choice];
      while( true ) {
        dlg.load_game_section((const char *)&str_difficulty_3,1);
        dlg.set_default_choice((uint)game.difficulty);
        dlg.set_position(
                   ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1)
                  );
        level = dlg.show_modal(0);
        if (level < 0) break;
        game.difficulty = (u8)level;
        if ((u8)level == '\0') {
          game.game_options = game.game_options | OPT_TUTORIAL;
        }
        else {
          game.game_options = game.game_options & ~OPT_TUTORIAL;
        }
        dlg.load_game_section((const char *)&str_gender_2,1);
        dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female);
        dlg.set_position(0xfffffc19,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1)
                  );
        choice = dlg.show_modal(0);
        /* BF-113 (Peter Johnson's ledger): a choice past the two genders (a row
           added to GAME.TXT's @GENDER) is no choice (the original indexed the
           leader's names and titles with it) */
        if (1 < choice) {
          choice = -1;
        }
        snd_play_effect(SND_MENUOK,0,0,0);
        if (-1 < choice) {
          is_female = (u8)choice;
          leaders[civs[game.player_civ].tribe].female = is_female;
          civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
          if (choice != 0) {
            civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
          }
          for (gov = 0; gov < 7; gov++) {
            for (gender = 0; gender < 2; gender++) {
              leaders[civs[game.player_civ].tribe].titles[gov][gender] = (short)(&government_titles[gov].male)[gender];
            }
          }
          choice = dlg_game_text_input(str_name_2,0x17,civ_names[game.player_civ].leader_name,name_buf
                                    );
          if (choice == 0) {
            crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),name_buf);
            saved_turn.resume_new_game = 1;
            turn_resume_new_game = 1;
            result = 0;
            started = 1;
            break;
          }
        }
      }
    }
  } while (started == 0);  /* cancelled: back to the scenario file */
  if (result != 0) {
    snd_play_effect(SND_MENUOK,0,0,0);
  }
  ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
  main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
  main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
  choice = result;
  return choice;
}

/* 0041e8fb auto */
i32 __cdecl load_game_pick_civ(int title_offset)

{
  char *name2;
  iptr sel;
  int choice;
  i32 arg4;
  i32 arg5;
  i32 arg6;
  i32 arg7;
  iptr civ;
  u8 is_female;
  i8 name_buf[260];
  int option_idx;
  int enabled_count;
  int option_count2;
  Dialog dlg;
  DialogOption *option;
  iptr option_civs[8];
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  while( true ) {
    game.human_players = '\0';
    dlg.Dialog::reset(1);
    dialog_prepend_title_label(&dlg,title_offset + 0x25e);
    option_idx = 0;
    for (civ = 1; civ < 8; civ++) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
        text_clear();
        name2 = civ_name_plural(civ);
        text_append(name2);
        text_append_space();
        text_append_lparen();
        name2 = civ_leader_name(civ);
        text_append(name2);
        text_append_rparen();
        text_append_space();
        text_append_lparen();
        text_append_int(civs[civ].num_cities);
        text_append_space();
        text_append_label(0xc5);
        text_append((const char *)&str_empty_12);
        text_append_int(civs[civ].num_military_units);
        text_append_space();
        text_append_label(0xc4);
        text_append((const char *)&str_empty_13);
        choice = option_idx;
        option_idx++;
        option = dlg.add_option((const char *)&textbuf.buf,choice,0)
        ;
        option_civs[(uint)option->id] = civ;
      }
    }
    option_count2 = option_idx;
    enabled_count = 0;
    for (option_idx = 0; option_idx < option_count2; option_idx++) {
      option = dlg.find_option(option_idx);
      if (option != 0) {
        enabled_count++;
        dlg.set_option_disabled((uint)option->id,0);
        if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
          ((RadioGroup *)dlg.radio_group)->enable_item((uint)option->id);
        }
      }
    }
    dlg.result_id = 0;
    dlg.result_extra = 0;
    sel = dlg.show_modal(0);
    if (sel < 0) break;
    human_civ[0] = option_civs[sel];
    (*((char *)&civ + 0)) = (byte)human_civ[0];
    game.human_players = game.human_players | (byte)(1 << ((byte)civ & 0x1f));
    game.player_civ = (byte)civ;
    game.turn_civ = (byte)civ;
    choice = civ_check_password(human_civ[0]);
    if (choice != 0) {
      game.game_options = game.game_options & ~OPT_TUTORIAL;
      dlg.load_game_section((const char *)&str_gender_3,1);
      dialog_prepend_title_label(&dlg,title_offset + 0x25e);
      dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      choice = dlg.show_modal(0);
      /* BF-113 */
      if (1 < choice) {
        choice = -1;
      }
      snd_play_effect(SND_MENUOK,0,0,0);
      if (-1 < choice) {
        is_female = (u8)choice;
        leaders[civs[game.player_civ].tribe].female = is_female;
        civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
        if (choice != 0) {
          civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
        }
        arg7 = 1;
        arg6 = 0;
        arg5 = 0;
        arg4 = 0;
        name2 = civ_leader_name(game.player_civ);
        choice = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_3,0x17,name2,(SurfaceWindow *)arg4,(DibSurface *)arg5,(RECT *)arg6,arg7);
        if (choice == 0) {
          dialog_prepend_title_label(&dlg,title_offset + 0x25e);
          choice = dlg.run_modal(name_buf,0);
          if (choice == 0) {
            crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),name_buf);
            arg4 = 0;
            return arg4;
          }
        }
      }
    }
  }
  arg4 = 1;
  return arg4;
}

/* Part of setup_network_game (00444310): joining a game: the connection, the
   name of the player, the address of the host, then the list of the games.
   Returns 1 when joined, 0 to show the menu again. */
static int setup_net_join(int conn_type,uint *eh_state)
{
  int choice;
  NetGamelistView *self;
  void *this2;
  char transport_ok;
  int rc;

  transport_ok = net.init_transport((*(uint *)&net.net_protocol),0);
  if (transport_ok == '\0') {
    dbg_log((const char *)&str_startup_lan_failure_of_connecttonet_as_c);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    popup_game_with_sprite(str_lanconnectfail,(Sprite *)&popup_info_icon_sprite,0);
    return 0;
  }
  choice = net_prompt_player_name();
  if (choice == 0) {
    net.shutdown(1);
    return 0;
  }
  if (((conn_type != 2) && (conn_type != 3)) && (choice = net_prompt_ip_address(), choice == 0)) {
    net.shutdown(1);
    return 0;
  }
  self = (NetGamelistView *)operator_new(0x178);
  (*((char *)eh_state + 0)) = 2;
  if (self == (NetGamelistView *)0x0) {
    this2 = (void *)0x0;
  }
  else {
    this2 = (void *)self->view_construct();
  }
  *eh_state = (*eh_state & 0xffffff00 | (byte)(1));
  net.gamelist_view = (NetGamelistView *)this2;
  rc = ((NetGamelistView *)this2)->net_session_init(conn_type);
  if (net.gamelist_view != 0) {
    ((NetGamelistView *)(void *)net.gamelist_view)->view_scalar_destruct(1);
  }
  net.gamelist_view = 0;
  if (rc != 0) return 1;
  net.shutdown(1);
  net.loaded_file_subdir[0] = 0;
  scenario_reset_params();
  return 0;
}

/* Part of setup_network_game (00444310): hosting over the LAN (IPX/SPX,
   TCP/IP): confirms the address of this machine and adds it to the
   connection title. Returns 0 when cancelled. */
static int setup_net_verify_ip(Dialog *dlg,char *ip_buf)
{
  size_t len;
  char *title_buf;
  size_t ip_len;
  int rc;

  for (;;) {
    if (net.lobby_launch_mode != -1) {
      crt_strcpy(((i8 *)&menu_prefs.net_ip_address),((i8 *)&net.lobby_launch_info));
      break;
    }
    title_buf = XD_GetCurrentProtoAddr(net.xd_adapter_index,net.active_protocol);
    if (title_buf == (char *)0x0) {
      menu_prefs.net_ip_address[0] = 0;
    }
    else {
      title_buf = XD_GetCurrentProtoAddr(net.adapter_index,(*(uint *)&net.net_protocol));
      crt_strcpy(((i8 *)&menu_prefs.net_ip_address),title_buf);
    }
    dlg->load_text_section((const char *)&str_game_7,(const char *)&str_ipverify,0x1f,(const char*)&menu_prefs.net_ip_address,0,0,0,1);
    rc = dlg->run_modal(ip_buf,0);
    if (rc == -1) {
      net.shutdown(1);
      return 0;
    }
    len = _strlen(ip_buf);
    if (len != 0) {
      crt_strcpy(((i8 *)&menu_prefs.net_ip_address),ip_buf);
      break;
    }
  }
  len = _strlen((*(char * *)&net.connection_title));
  ip_len = _strlen(((i8 *)&menu_prefs.net_ip_address));
  title_buf = (char *)operator_new(len + ip_len + 3);
  if (title_buf == (char *)0x0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_tempstr,(const char*)&str_d_ss_franklinton_startup_lan_cpp_3,0x1b9);
  }
  crt_strcpy(title_buf,(*(char * *)&net.connection_title));
  crt_strcat(title_buf,((i8 *)&net_title_ip_sep_str));
  crt_strcat(title_buf,((i8 *)&menu_prefs.net_ip_address));
  operator_delete((*(char * *)&net.connection_title));
  (*(char * *)&net.connection_title) = title_buf;
  return 1;
}

/* Part of setup_network_game (00444310): hosting a game: the names of the
   player and the game, the connection, then waiting for the players. Returns
   1 when the game starts, 0 to show the menu again. */
static int setup_net_host(Dialog *dlg,char *ip_buf,int conn_type,i32 map_source,uint *eh_state)
{
  int choice;
  LobbyList *this_00;
  LobbyList *lobby_list;
  char transport_ok;

  if (((conn_type != 2) && (conn_type != 3)) ||
     ((choice = net_prompt_player_name(), choice != 0 && (choice = net_prompt_game_name(), choice != 0)))) {
    transport_ok = net.init_transport((*(uint *)&net.net_protocol),1);
    if (transport_ok == '\0') {
      if (((*(uint *)&net.net_protocol) != 2) && ((*(uint *)&net.net_protocol) != 3)) {
        dbg_log((const char *)&str_startup_lan_failure_of_connecttonet_as_s);
        crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
        popup_game_with_sprite(str_lanconnectfail_2,(Sprite *)&popup_info_icon_sprite,0);
      }
      return 0;
    }
    if ((conn_type != 2) && (conn_type != 3)) {
      choice = net_prompt_player_name();
      if (choice == 0) {
        net.shutdown(1);
        return 0;
      }
      choice = net_prompt_game_name();
      if (choice == 0) {
        net.shutdown(1);
        return 0;
      }
    }
    if (conn_type == 0) {
      if (setup_net_verify_ip(dlg,ip_buf) == 0) return 0;
    }
    else if (((conn_type == 1) || (conn_type == 4)) && (choice = net_prompt_ip_address(), choice == 0)) {
      net.shutdown(1);
      return 0;
    }
    this_00 = (LobbyList *)operator_new(0x70);
    (*((char *)eh_state + 0)) = 3;
    if (this_00 == (LobbyList *)0x0) {
      lobby_list = (LobbyList *)0x0;
    }
    else {
      lobby_list = this_00->construct();
    }
    *eh_state = (*eh_state & 0xffffff00 | (byte)(1));
    net.net_lobby_list = lobby_list;
    choice = lobby_list->host_wait_players(map_source,conn_type);
    if (net.net_lobby_list != (LobbyList *)0x0) {
      net.net_lobby_list->scalar_destruct(1);
    }
    net.net_lobby_list = (LobbyList *)0x0;
    if (choice != 0) return 1;
    net.shutdown(1);
    net.loaded_file_subdir[0] = 0;
    scenario_reset_params();
  }
  return 0;
}

/* Part of setup_network_game (00444310): the parameters of a random world
   (land mass, land form, temperature, climate, age), each at random. It calls
   rand() as crt_rand_at("setup_network_game"): the rand trace names the
   original function. */
static void setup_net_random_world(void)
{
  int rc2;

  rc2 = crt_rand_at("setup_network_game");
  mapgen.land_mass = rc2 % 3 - 1;
  mapgen.land_form = 0;
  rc2 = crt_rand_at("setup_network_game");
  if (rc2 % 3 == 0) {
    mapgen.land_form = mapgen.land_mass;
  }
  rc2 = crt_rand_at("setup_network_game");
  mapgen.temperature = rc2 % 3 - 1;
  rc2 = crt_rand_at("setup_network_game");
  mapgen.climate = rc2 % 3 - 1;
  rc2 = crt_rand_at("setup_network_game");
  mapgen.age = rc2 % 3 - 1;
}

/* Part of setup_network_game (00444310): back at the network menu: the
   backdrop again after a game, and the state of the game to start reset
   (rules reloaded but the first time). */
static void setup_net_reset_game(int first_pass,GamePalette *palette)
{
  int i;

  if (turn_resume_new_game == 1) {
    snd_play_effect(SND_MENUOK,0,0,0);
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    wnd_flush_paint();
  }
  for (i = 0; i < 8; i++) {
    civ_view.intro_shown[i] = 0;
    (((i8 *)&civ_saved_cursor_x_block.net_player_setup_table))[i * 0x20] = 0;
  }
  civ_saved_cursor_x_block.mp_turn_time_limit_ms = 0;
  turn_loop.resume_civ = 0;
  saved_turn.civ = 0;
  turn_resume_new_game = 0;
  saved_turn.resume_new_game = 0;
  net.loaded_file_subdir[0] = 0;
  net.loaded_file_kind = 0;
  scenario_reset_params();
  game.turn = 0;
  game.first_launch_turn = -1;
  game.game_flags = 0;
  crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  if (first_pass == 0) {
    rules_load(1);
  }
  scenario_events_loaded = 0;
  setup_dlg.dialog_pos_state = 3;
}

/* Part of setup_network_game (00444310): the connection title (the kind of
   network and protocol), kept in net.connection_title. */
static void setup_net_connection_title(int conn_type)
{
  size_t len;

  text_clear();
  if (conn_type == 0) {
    text_append_label(0x28f);
  }
  else if ((conn_type == 1) || (conn_type == 4)) {
    if (conn_type == 1) {
      text_append_label(0x290);
    }
    else {
      text_append_label(0x362);
    }
    text_append_space();
    if ((*(uint *)&net.net_protocol) == 0) {
      text_append_label(0x291);
    }
    else if ((*(uint *)&net.net_protocol) == 1) {
      text_append_label(0x292);
    }
  }
  else {
    text_append_label(0x349);
  }
  if ((*(char * *)&net.connection_title) != (char *)0x0) {
    operator_delete((*(char * *)&net.connection_title));
    (*(char * *)&net.connection_title) = (char *)0x0;
  }
  len = _strlen(((i8 *)&textbuf.buf));
  (*(char * *)&net.connection_title) = (char *)operator_new(len + 1);
  if ((*(char * *)&net.connection_title) == (char *)0x0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_gnetmgr_pdescription,(const char*)&str_d_ss_franklinton_startup_lan_cpp_2,0x79);
  }
  crt_strcpy((*(char * *)&net.connection_title),((i8 *)&textbuf.buf));
}

/* 00444310 auto */
i32 __cdecl setup_network_game(int conn_type)

{
  int choice;
  i32 result;
  int rc2;
  char ip_buf[260];
  GamePalette palette;
  int first_pass;
  Dialog dlg;
  i32 map_source;
  int saved_open_game;
  int rc;
  uint eh_state;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  first_pass = 1;
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  if ((((conn_type != 0) && (conn_type != 1)) && (conn_type != 4)) && ((conn_type != 2 && (conn_type != 3))))
  {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_whichnetwork_internet_whichnetwork_local,(const char*)&str_d_ss_franklinton_startup_lan_cpp,0x21);
  }
  if (bugfix_ui()) {
    saved_open_game = (int)mp_options.open_game;  /* B18: joining keeps the option (docs/BUGS.md) */
  }
  net.connection_type = conn_type;
  if ((conn_type == 1) || (conn_type == 4)) {
    dlg.load_game_section((const char *)&str_networktype,1);
    dlg.set_default_choice((int)(short)menu_prefs.net_protocol);
    if (conn_type == 4) {
      dlg.set_option_disabled(1,1);
    }
    rc = dlg.show_modal(0);
    if (rc == -1) {
      result = 1;
      return result;
    }
    (*(uint *)&net.net_protocol) = (uint)(rc != 0);
    menu_prefs.net_protocol = (ushort)(rc != 0);
    if (conn_type == 4) {
      game.multiplayer = 4;
      conn_type = 0;
      net.connection_type = 0;
    }
    else {
      game.multiplayer = 3;
    }
  }
  else if (conn_type == 0) {
    (*(uint *)&net.net_protocol) = 0;
    game.multiplayer = 4;
  }
  else if (conn_type == 2) {
    (*(uint *)&net.net_protocol) = 2;
    game.multiplayer = 5;
  }
  else {
    game.multiplayer = 6;
    (*(uint *)&net.net_protocol) = 3;
  }
  setup_net_connection_title(conn_type);
  turn_resume_new_game = 0;
  saved_turn.resume_new_game = 0;
  for (;;) {
    setup_net_reset_game(first_pass,&palette);
    setup_show_side_picture(0xffffffff,0);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    dlg.load_game_section((const char *)&str_lan1,1);
    choice = -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1);
    result = setup_next_dialog_x(0);
    dlg.set_position(result,choice);
    if (net.lobby_launch_mode == 2) {
      dlg.set_option_disabled(5,1);
      if (menu_prefs.lan_menu_choice == 5) {
        dlg.set_default_choice(0);
      }
      else {
        dlg.set_default_choice((int)menu_prefs.lan_menu_choice);
      }
    }
    else if (net.lobby_launch_mode == 1) {
      dlg.set_option_disabled(0,1);
      dlg.set_option_disabled(1,1);
      dlg.set_option_disabled(2,1);
      dlg.set_option_disabled(3,1);
      dlg.set_option_disabled(4,1);
      dlg.set_default_choice(5);
    }
    else {
      dlg.set_default_choice((int)menu_prefs.lan_menu_choice);
    }
    choice = dlg.show_modal(0);
    snd_play_effect(SND_MENUOK,0,0,0);
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    first_pass = 0;
    if (choice < 0) {
      result = 1;
      return result;
    }
    menu_prefs.lan_menu_choice = (short)choice;
    if ((-1 < choice) && (choice < 3)) {
      __chdir(((i8 *)&game_dirs.home));
    }
    switch(choice) {
    case 0:
      rc2 = setup_choose_map_size();
      if (rc2 != 0) continue;
      map_source = 0;
      setup_net_random_world();
      rc2 = setup_choose_game_options(map_source,3);
      if (rc2 != 0) continue;
      break;
    case 1:
      rc2 = map_load_dialog();
      if (rc2 != 0) continue;
      map_source = 1;
      if (map.flat != 0) {
        map_source = 2;
      }
      setup_net_random_world();
      rc2 = setup_choose_game_options(map_source,3);
      if (rc2 != 0) continue;
      break;
    case 2:
      rc2 = setup_choose_map_size();
      if (rc2 != 0) continue;
      map_source = 0;
      rc2 = setup_custom_world();
      if (rc2 != 0) continue;
      rc2 = setup_choose_game_options(map_source,3);
      if (rc2 != 0) continue;
      break;
    case 3:
      game_palette_block.event_dont_play_wonders = 0;
      rc2 = setup_mp_load_scenario(3);
      if (rc2 != 0) {
        crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
        continue;
      }
      if (game_palette_block.event_dont_play_wonders != 0) {
        game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
      }
      break;
    case 4:
      game_palette_block.event_dont_play_wonders = 0;
      rc2 = game_load_dialog(0,3,1);
      if (rc2 == 0) {
        if (2 < game.multiplayer) break;
        crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
        popup_game_with_sprite(str_lannot,(Sprite *)&popup_info_icon_sprite,0);
      }
      else if (game_palette_block.event_dont_play_wonders != 0) {
        game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
      }
      continue;
    case 5: ;
    }
    civ_saved_cursor_x_block.net_claimed_civs_mask = 0;
    prefs_save_civ2dat();
    civ_saved_cursor_x_block.game_setup_complete = 0;
    game.human_players = '\0';
    if (choice != 5) {
      saved_open_game = (int)mp_options.open_game;
      mp_options.open_game = 1;
      if (setup_net_host(&dlg,ip_buf,conn_type,map_source,&eh_state) != 0) break;
      continue;
    }
    if (setup_net_join(conn_type,&eh_state) != 0) break;
  }
  mp_options.open_game = (short)saved_open_game;
  civ_saved_cursor_x_block.game_setup_complete = 1;
  prefs_save_civ2dat();
  result = 0;
  return result;
}

/* 00445712 auto */
i32 __cdecl setup_mp_load_scenario(i32 mp_mode)

{
  iptr rc;
  i32 value;
  iptr load_rc;
  int unit;
  GamePalette palette;
  Dialog dlg;
  int i;
  i8 dst[260];
  int level;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  for (;;) {
    if (cd_music.available == 0) {
      snd_cd_stop();
      snd_play_effect(SND_MENULOOP,0,1,0);
    }
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    rc = game_load_dialog(1,3,1);
    if (rc != 0) {
      snd_play_effect(SND_MENUOK,0,0,0);
      ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
      main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
      main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
      wnd_flush_paint();
      value = 1;
      return value;
    }
    path_set_extension(dst,(char *)(&textbuf.buf),lang_file_ext);
    if (cd_music.available != 0) {
      snd_play_effect(SND_MENUOK,0,0,0);
    }
    game.game_options = prefs.game_options & 0xffff7fff;
    game.message_options = prefs.message_options;
    game.human_players = '\0';
    text_set_string_arg(0,(const char *)&scenario.name);
    text_set_number_arg(0,(int)scenario.tech_paradigm);
    text_clear();
    value = game_turn_to_year((int)game.turn);
    text_append_year(value);
    text_set_string_arg(1,(const char *)&textbuf.buf);
    text_clear();
    if (scenario.max_turns == 0) {
      text_append_year(2000);
    }
    else {
      value = game_turn_to_year((int)scenario.max_turns);
      text_append_year(value);
    }
    text_set_string_arg(2,(const char *)&textbuf.buf);
    if (cd_music.available != 0) {
      snd_menu_end();
    }
    dlg.init_default();
    load_rc = dlg.load_text((const char *)dst,(const char *)&str_scenario_2,0x4008001);
    if (load_rc == 0) {
      unit = (int)game.active_unit;
      if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
         ((int)units[unit].owner == (int)scenario.objective_civ)) {
        dlg.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),0
                   ,0);
      }
      unit = dlg.show_modal(0);
      if (unit < 0) continue;
    }
    else {
      unit = popup_game_flags((const char *)&str_scenarioloaded_2,1);
      if (unit < 0) continue;
    }
    while( true ) {
      dlg.load_game_section((const char *)&str_difficulty_4,1);
      dlg.set_default_choice((uint)game.difficulty);
      dlg.set_position(
                 ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      level = dlg.show_modal(0);
      if (level < 0) break;
      game.difficulty = (u8)level;
      if ((u8)level == '\0') {
        game.game_options = game.game_options | OPT_TUTORIAL;
      }
      else {
        game.game_options = game.game_options & ~OPT_TUTORIAL;
      }
      while( true ) {
        saved_turn.resume_new_game = 0;
        turn_resume_new_game = 0;
        dlg_checks_clear();
        dlg_check_set(0,(int)menu_prefs.mp_open_game);
        dlg_check_set(1,(int)menu_prefs.mp_kill_on_retire);
        dlg_check_set(2,(int)menu_prefs.mp_show_human_starts);
        dlg_check_set(3,(int)menu_prefs.mp_chat_from_start);
        dlg_check_set(4,(int)menu_prefs.mp_double_production);
        dlg_check_set(5,(int)menu_prefs.mp_double_moves);
        dlg.load_text_section((const char *)&str_game_7,(const char *)&str_advancedmp,0,0,0,0,0,0);
        if (net.ini_simultaneous_allowed == 0) {
          net.mp_simultaneous_moves = 0;
          mp_options.simultaneous_setting = 0;
        }
        else {
          text_clear();
          text_append_label(0x364);
          dlg.add_option((const char *)&textbuf.buf,6,0);
          dlg_check_set(6,(int)menu_prefs.mp_simultaneous_moves);
        }
        if (((byte)dlg.style_flags & 4) != 0) {
          for (i = 0; i < (int)dlg.option_count; i++) {
            dlg.set_option_checked(i,
                       1 << ((byte)i & 0x1f) & dlg_input.check_bits);
          }
        }
        dlg.set_position(
                   ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1)
                  );
        unit = dlg.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        if (unit < 0) break;
        value = dlg.option_is_checked(0);
        mp_options.open_game = (short)value;
        menu_prefs.mp_open_game = mp_options.open_game;
        value = dlg.option_is_checked(1);
        mp_options.kill_civ_on_retire = (short)value;
        menu_prefs.mp_kill_on_retire = mp_options.kill_civ_on_retire;
        value = dlg.option_is_checked(2);
        mp_options.show_human_starts = (short)value;
        menu_prefs.mp_show_human_starts = mp_options.show_human_starts;
        value = dlg.option_is_checked(3);
        mp_options.chat_from_start = (short)value;
        menu_prefs.mp_chat_from_start = mp_options.chat_from_start;
        value = dlg.option_is_checked(4);
        mp_doubles.production = (short)value;
        menu_prefs.mp_double_production = mp_doubles.production;
        value = dlg.option_is_checked(5);
        mp_doubles.moves = (short)value;
        menu_prefs.mp_double_moves = mp_doubles.moves;
        if (net.ini_simultaneous_allowed != 0) {
          value = dlg.option_is_checked(6);
          mp_options.simultaneous_setting = (short)value;
          menu_prefs.mp_simultaneous_moves = mp_options.simultaneous_setting;
        }
        saved_turn.resume_new_game = 1;
        turn_resume_new_game = 1;
        unit = setup_choose_game_timer(0);
        if (unit == 0) {
          turn_loop.resume_civ = *(i8 *)&human_civ;
          saved_turn.civ = *(i8 *)&human_civ;
          value = 0;
          return value;
        }
      }
    }
  }
}

/* 0051d9a0 auto */
i32 __cdecl setup_choose_multiplayer_type(void)

{
  UINT xd_adapter;
  UINT adapter;
  int status;
  i32 result;
  int choice;
  UINT max_players;
  Dialog dialog2;
  i32 eh_next;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  adapter = GetPrivateProfileIntA
                    (str_civilization_gold_10,str_nettimeout,-1,str_civ_ini_10);
  if (adapter == 0xffffffff) {
    WritePrivateProfileStringA
              (str_civilization_gold_11,str_nettimeout_2,((i8 *)&net_ini_timeout_default_str),str_civ_ini_11);
    (*(UINT *)&net.timeout_seconds) = 0x1e;
    adapter = (*(UINT *)&net.timeout_seconds);
  }
  (*(UINT *)&net.timeout_seconds) = adapter;
  adapter = GetPrivateProfileIntA
                    (str_civilization_gold_12,str_adapter,-1,str_civ_ini_12);
  xd_adapter = adapter;
  if (adapter == 0xffffffff) {
    WritePrivateProfileStringA
              (str_civilization_gold_13,str_adapter_2,((i8 *)&net_ini_adapter_default_str),str_civ_ini_13);
    net.xd_adapter_index = 0;
    net.adapter_index = 0;
    adapter = net.adapter_index;
    xd_adapter = net.xd_adapter_index;
  }
  for (;;) {
    net.xd_adapter_index = xd_adapter;
    net.adapter_index = adapter;
    (*(UINT *)&net.net_max_players) = 7;
    max_players = GetPrivateProfileIntA
                          (str_civilization_gold_14,str_maxplayers_3,-1,str_civ_ini_14);
    if (4 < (int)max_players) {
      if (7 < (int)max_players) {
        max_players = 7;
      }
      (*(UINT *)&net.net_max_players) = max_players;
      WritePrivateProfileStringA
                (str_civilization_gold_15,str_maxplayers_4,((i8 *)&net_ini_maxplayers_clear_str),str_civ_ini_15);
    }
    game.multiplayer = '\0';
    game.civs_alive = '\0';
    game.human_players = '\0';
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
    scenario_events_loaded = 0;
    passwords_reset();
    if (net.lobby_launch_mode == -1) {
      setup_show_side_picture(7,1);
      dialog2.load_game_section((const char *)&str_multitype2,1);
      dialog2.set_default_choice((int)menu_prefs.multiplayer_type);
      status = -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1);
      result = setup_next_dialog_x(1);
      dialog2.set_position(result,status);
      choice = dialog2.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (choice < 0) {
        result = 1;
        return result;
      }
      menu_prefs.multiplayer_type = (short)choice;
    }
    else {
      choice = 2;
    }
    if ((choice < 0) || (3 < choice)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_event_0_event_3,(const char*)&str_d_ss_franklinton_startup_multiplayer_cpp,0x66);
    }
    switch(choice) {
    case 0:
      status = setup_hotseat_game();
      break;
    case 1:
      status = setup_network_game(1);
      break;
    case 2:
      status = setup_network_game(0);
      if (status == 0) {
        result = 0;
        return result;
      }
      adapter = net.adapter_index;
      xd_adapter = net.xd_adapter_index;
      if (net.lobby_launch_mode != -1) {
        result = 1;
        return result;
      }
      continue;
    case 3:
      (*(UINT *)&net.net_max_players) = 2;
      status = setup_network_game(4);
      break;
    default:
      result = 0;
      return result;
    }
    adapter = net.adapter_index;
    xd_adapter = net.xd_adapter_index;
    if (status == 0) {
      result = 0;
      return result;
    }
  }
}

/* Part of setup_choose_game_options (0051dd97): the multiplayer options
   checked, kept in the game and the preferences. */
static void game_options_read_mp(Dialog *dialog2,int mp_mode)
{
  i32 value;

  value = dialog2->option_is_checked(0);
  mp_options.open_game = (short)value;
  menu_prefs.mp_open_game = mp_options.open_game;
  value = dialog2->option_is_checked(1);
  mp_options.kill_civ_on_retire = (short)value;
  menu_prefs.mp_kill_on_retire = mp_options.kill_civ_on_retire;
  value = dialog2->option_is_checked(2);
  mp_options.show_human_starts = (short)value;
  menu_prefs.mp_show_human_starts = mp_options.show_human_starts;
  value = dialog2->option_is_checked(3);
  mp_options.chat_from_start = (short)value;
  menu_prefs.mp_chat_from_start = mp_options.chat_from_start;
  value = dialog2->option_is_checked(4);
  mp_doubles.production = (short)value;
  menu_prefs.mp_double_production = mp_doubles.production;
  value = dialog2->option_is_checked(5);
  mp_doubles.moves = (short)value;
  menu_prefs.mp_double_moves = mp_doubles.moves;
  if (net.ini_simultaneous_allowed != 0) {
    value = dialog2->option_is_checked(6);
    mp_options.simultaneous_setting = (short)value;
    menu_prefs.mp_simultaneous_moves = mp_options.simultaneous_setting;
  }
  if (mp_mode == 1) {
    mp_options.chat_from_start = 0;
    mp_options.simultaneous_setting = 0;
  }
}

/* Part of setup_choose_game_options (0051dd97): the dialog of the
   multiplayer options, set up from the preferences. */
static void game_options_mp_dialog(Dialog *dialog2,int mp_mode)
{
  int i;

  setup_show_side_picture(9,0);
  dlg_checks_clear();
  dlg_check_set(0,(int)menu_prefs.mp_open_game);
  dlg_check_set(1,(int)menu_prefs.mp_kill_on_retire);
  dlg_check_set(2,(int)menu_prefs.mp_show_human_starts);
  dlg_check_set(3,(int)menu_prefs.mp_chat_from_start);
  dlg_check_set(4,(int)menu_prefs.mp_double_production);
  dlg_check_set(5,(int)menu_prefs.mp_double_moves);
  dialog2->load_text_section((const char *)&str_game_7,(const char *)&str_advancedmp_2,0,0,0,0,0,0);
  if (net.ini_simultaneous_allowed == 0) {
    net.mp_simultaneous_moves = 0;
    mp_options.simultaneous_setting = 0;
  }
  else {
    text_clear();
    text_append_label(0x364);
    dialog2->add_option((const char *)&textbuf.buf,6,0);
    dlg_check_set(6,(int)menu_prefs.mp_simultaneous_moves);
  }
  if (((byte)dialog2->style_flags & 4) != 0) {
    for (i = 0; i < (int)dialog2->option_count; i++) {
      dialog2->set_option_checked(i,
                 1 << ((byte)i & 0x1f) & dlg_input.check_bits);
    }
  }
  if ((mp_mode == 1) &&
     (dialog2->set_option_disabled(3,1), net.ini_simultaneous_allowed != 0))
  {
    dialog2->set_option_disabled(6,1);
  }
  dialog2->set_position(
             ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
             -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
}

/* Part of setup_choose_game_options (0051dd97): the accelerated start: which
   of the three start years. Returns 0 when cancelled. */
static int game_options_accelerated_start(Dialog *dialog2)
{
  int choice;
  uint year;
  int late_year;
  int mid_year;
  int start_year;

  game.first_launch_turn = -1;
  choice = game_turn_to_year(0);
  if (choice < 1) {
    year = game_turn_to_year(0);
    start_year = ~year + 1;
  }
  else {
    start_year = game_turn_to_year(0);
  }
  text_set_number_arg(0,start_year);
  choice = game_turn_to_year(0x15);
  if (choice < 1) {
    year = game_turn_to_year(0x15);
    mid_year = ~year + 1;
  }
  else {
    mid_year = game_turn_to_year(0x15);
  }
  text_set_number_arg(1,mid_year);
  choice = game_turn_to_year(0x29);
  if (choice < 1) {
    year = game_turn_to_year(0x29);
    late_year = ~year + 1;
  }
  else {
    late_year = game_turn_to_year(0x29);
  }
  text_set_number_arg(2,late_year);
  setup_show_side_picture(9,0);
  dialog2->load_game_section((const char *)&str_accelerated_2,1);
  dialog2->set_default_choice((int)prefs.accelerated_start);
  dialog2->set_position(
             -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1),
             -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
  setup_dlg.accelerated_start = dialog2->show_modal(0);
  snd_play_effect(SND_MENUOK,0,0,0);
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
  if (setup_dlg.accelerated_start < 0) {
    setup_dlg.accelerated_start = 0;
    return 0;
  }
  return 1;
}

/* Part of setup_choose_game_options (0051dd97): the advanced rules checked
   (into game.rules and the preferences, the cheat bit kept unless the rules
   are the game's own). */
static void game_options_read_advanced(Dialog *dialog2,int rules_mode)
{
  int choice;
  int i;

  (*(u32 *)&dlg_input.help_pressed) = dialog2->result_extra;
  if (((byte)dialog2->style_flags & 4) != 0) {
    dlg_input.check_bits = 0;
    for (i = 0; i < (int)dialog2->option_count; i++) {
      choice = dialog2->option_is_checked(i);
      if (choice != 0) {
        dlg_input.check_bits = dlg_input.check_bits | 1 << ((byte)i & 0x1f);
      }
    }
  }
  game.rules = 0x2f;
  choice = dlg_check_get(0);
  if (choice == 0) {
    game.rules = game.rules | RULE_NORMAL_COMBAT;
  }
  choice = dlg_check_get(1);
  if (choice != 0) {
    game.rules = game.rules | RULE_FLAT_WORLD;
  }
  setup_dlg.select_opponents = dlg_check_get(2);
  setup_dlg.accelerated_start = dlg_check_get(3);
  choice = dlg_check_get(4);
  if (choice != 0) {
    game.rules = game.rules | RULE_BLOODLUST;
  }
  choice = dlg_check_get(5);
  if (choice != 0) {
    game.rules = game.rules | RULE_NO_RESTART_ELIMINATED;
  }
  if (rules_mode == 0) {
    prefs.game_rules[0] = game.rules;
  }
  else {
    prefs.game_rules[0] = game.rules & ~RULE_FLAT_WORLD | prefs.game_rules[0] & 0x8000;
  }
}

/* 0051dd97 auto */
i32 __cdecl setup_choose_game_options(int rules_mode,int mp_mode)

{
  byte sign;
  int choice;
  i32 value;
  u8 barbarity;
  Dialog dialog2;
  int i;
  int difficulty2;
  i32 eh_next;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  setup_dlg.select_opponents = 0;
  setup_dlg.accelerated_start = 0;
  leaders_fixup_names();
  game.unused_0e = 0;
  for (;;) {
    do {
      do {
        setup_show_side_picture(3,0);
        dialog2.load_game_section((const char *)&str_difficulty_5,1);
        dialog2.set_default_choice((int)prefs.difficulty);
        dialog2.set_position(
                   ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
        difficulty2 = dialog2.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        if (difficulty2 < 0) {
          value = 1;
          return value;
        }
        game.difficulty = (u8)difficulty2;
        prefs.difficulty = (short)difficulty2;
        setup_show_side_picture(4,0);
        dialog2.load_game_section((const char *)&str_enemies2,1);
        dialog2.set_default_choice((int)prefs.enemies_choice);
        dialog2.set_position(
                   -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1),
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
        choice = dialog2.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      } while (choice < 0);
      if (dialog2.result_extra == 0) {
        prefs.enemies_choice = (short)choice;
        game.num_enemies = 6 - (char)choice;
      }
      else {
        choice = _rand();
        game.num_enemies = (char)(choice % 5) + 2;
      }
      setup_show_side_picture(5,0);
      dialog2.load_game_section((const char *)&str_barbarity_2,1);
      dialog2.set_default_choice((int)prefs.barbarity);
      dialog2.set_position(
                 ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      choice = dialog2.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    } while (choice < 0);
    if (dialog2.result_extra == 0) {
      barbarity = (u8)choice;
      game.barbarian_activity = barbarity;
      prefs.barbarity = (short)choice;
    }
    else if (game.difficulty < DIFF_PRINCE) {
      choice = _rand();
      sign = (byte)(choice >> 0x1f);
      game.barbarian_activity =
           ((((byte)choice ^ sign) - sign & 1 ^ sign) - sign) + game.difficulty;
    }
    else if (game.difficulty < DIFF_DEITY) {
      choice = _rand();
      game.barbarian_activity = (char)(choice % 3) + 1;
    }
    else {
      choice = _rand();
      sign = (byte)(choice >> 0x1f);
      game.barbarian_activity = ((((byte)choice ^ sign) - sign & 1 ^ sign) - sign) + 2;
    }
    for (;;) {
      game.rules = 0x3f;
      setup_show_side_picture(9,0);
      dialog2.load_game_section((const char *)&str_rules_6,1);
      dialog2.set_default_choice((int)prefs.rules_choice);
      dialog2.set_position(
                 -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1),
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      choice = dialog2.show_modal(0);
      snd_play_effect(SND_MENUOK,0,0,0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (choice < 0) break;
      prefs.rules_choice = (short)choice;
      if (choice != 0) {
        game.rules = prefs.game_rules[0];
        if (rules_mode == 1) {
          game.rules = prefs.game_rules[0] & 0x7fff;
        }
        else if (rules_mode == 2) {
          game.rules = prefs.game_rules[0] | 0x8000;
        }
        dlg_checks_clear();
        dlg_check_set(0,(uint)((game.rules & RULE_NORMAL_COMBAT) == 0));
        dlg_check_set(1,game.rules & RULE_FLAT_WORLD);
        dlg_check_set(2,0);
        dlg_check_set(3,(int)prefs.accelerated_start);
        dlg_check_set(4,game.rules & RULE_BLOODLUST);
        dlg_check_set(5,game.rules & RULE_NO_RESTART_ELIMINATED);
        setup_show_side_picture(9,0);
        dialog2.load_text_section((const char *)&str_game_7,(const char *)&str_advanced_2,0,0,0,0,0,0);
        if (((byte)dialog2.style_flags & 4) != 0) {
          for (i = 0; i < (int)dialog2.option_count; i++) {
            dialog2.set_option_checked(i,1 << ((byte)i & 0x1f) & dlg_input.check_bits);
          }
        }
        dialog2.set_position(
                   ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
        choice = dialog2.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        if (choice < 0) continue;
        game_options_read_advanced(&dialog2,rules_mode);
        if (setup_dlg.accelerated_start != 0) {
          if (game_options_accelerated_start(&dialog2) == 0) continue;
        }
        prefs.accelerated_start = (short)setup_dlg.accelerated_start;
      }
      while( true ) {
        game_options_mp_dialog(&dialog2,mp_mode);
        choice = dialog2.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        if (choice < 0) break;
        game_options_read_mp(&dialog2,mp_mode);
        if ((mp_mode == 2) || (choice = setup_choose_game_timer(0), choice == 0)) {
          value = 0;
          return value;
        }
      }
    }
  }
}

/* 0051ea8e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl setup_choose_game_timer(int in_game)

{
  int minutes;
  i32 result;
  int seconds;
  int choice;
  char custom_text[16];
  Dialog dlg;
  int custom_seconds;
  short saved_choice;
  int saved_limit;
  
  dlg.Dialog::construct(0x2000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  saved_choice = menu_prefs.turn_timer_choice;
  saved_limit = civ_saved_cursor_x_block.mp_turn_time_limit_ms;
  for (;;) {
    menu_prefs.turn_timer_choice = saved_choice;
    if ((in_game == 0) && (turn_resume_new_game == 0)) {
      setup_show_side_picture(0xffffffff,0);
      dlg.load_game_section((const char *)&str_gametimer,1);
      dlg.set_default_choice((int)menu_prefs.turn_timer_choice);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
    }
    else {
      dlg.load_game_section((const char *)&str_gametimer_2,1);
      dlg.set_default_choice((int)menu_prefs.turn_timer_choice);
      dlg.set_position(0xfffffc19,0xfffffc19);
    }
    choice = dlg.show_modal(0);
    if (choice < 0) {
      if (in_game == 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      }
      result = 1;
      return result;
    }
    menu_prefs.turn_timer_choice = (short)choice;
    switch(choice) {
    case 0:
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = 0;
      break;
    case 1:
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = 30000;
      break;
    case 2:
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = 60000;
      break;
    case 3:
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = 120000;
      break;
    case 4:
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = 180000;
      break;
    case 5:
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = 300000;
      break;
    case 6:
      while( true ) {
        minutes = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_customgametimer,4,(const char*)&menu_prefs.turn_timer_custom,0,0,0,1);
        if (minutes != 0) {
          if (in_game == 0) {
            ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
          }
          result = 1;
          return result;
        }
        if (in_game == 0) {
          dlg.set_position(0xfffffc19,
                     -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
        }
        else {
          dlg.set_position(0xfffffc19,0xfffffc19);
        }
        choice = dlg.run_modal(custom_text,0);
        snd_play_effect(SND_MENUOK,0,0,0);
        if (choice < 0) break;
        custom_seconds = _atoi(custom_text);
        if (custom_seconds < 0) {
          (*(ushort *)&autoplay.active) = (*(ushort *)&autoplay.active) ^ 1;
          autoplay.turns_left = -(short)custom_seconds;
        }
        else if ((custom_seconds < 0xe11) && (9 < custom_seconds)) {
          civ_saved_cursor_x_block.mp_turn_time_limit_ms = custom_seconds * 1000;
          break;
        }
      }
      if (choice < 0) {
        if (in_game == 0) {
          ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        }
        continue;
      }
      break;
    }
    break;
  }
  if (in_game == 0) {
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    if (choice == 6) {
      crt_strcpy(((i8 *)&menu_prefs.turn_timer_custom),custom_text);
    }
    result = 0;
    return result;
  }
  if (1 < net.net_player_count[0]) {
    crt_strcpy(((i8 *)&text_args.strings),((i8 *)&net.local_player_name));
    str_clear((char *)&text_args.string1);
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms == 0) {
      str_append_label(text_args.string1,0x285);
    }
    else {
      minutes = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) / 0x3c;
      seconds = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) % 0x3c;
      if (minutes < 10) {
        str_append_int(text_args.string1,0);
      }
      str_append_int(text_args.string1,minutes);
      crt_strcat(((i8 *)&text_args.string1),((i8 *)&setup_timer_minsec_sep_str));
      if (seconds < 10) {
        str_append_int(text_args.string1,0);
      }
      str_append_int(text_args.string1,seconds);
    }
    net.vote_replies_pending = net.net_player_count[0] - 1;
    net.vote_status = 0;
    net_send_text_args(0x40,0xff,2,0,0,0);
    dlg_style.pending_idle_callback = net_vote_wait_idle_cb;
    dialog_state.timeout_start = timer_ticks();
    minutes = popup_game_sprite_flags(str_newturntimerserver,0x2000001,(Sprite *)&popup_info_icon_sprite,0);
    if (((minutes < 0) || (net.vote_status == -1)) || (net.vote_replies_pending != 0)) {
      menu_prefs.turn_timer_choice = saved_choice;
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = saved_limit;
      net_send_text_args(0x41,0xff,0,0,0,0);
      popup_game_with_sprite(str_newturntimerno_2,(Sprite *)&popup_info_icon_sprite,0);
      result = 1;
      return result;
    }
    net_send_text_args(0x42,0xff,0,0,0,0);
    popup_game_with_sprite(str_newturntimeryes_2,(Sprite *)&popup_info_icon_sprite,0);
  }
  result = 0;
  return result;
}

/* Where setup_choose_tribe (0051f19c) goes on after one of its parts: on with
   the step after it, back to the choice of the tribe (the gender dialog,
   or the tribe list in a network game), or to its end. */
enum TribeStep { TRIBE_NEXT, TRIBE_RETRY, TRIBE_FINISH };

/* Part of setup_choose_tribe (0051f19c): the message that the server quit or
   the connection to it was lost (in a network game). */
static void setup_tribe_server_lost_popup(const char *lost_msg,const char *quit_msg)
{
  text_set_string_arg(1,(const char *)&net.net_game_name);
  crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
  if (net.nm_server_canceled_count == 0) {
    popup_game_with_sprite(lost_msg,(Sprite *)&popup_info_icon_sprite,0);
  }
  else {
    popup_game_with_sprite(quit_msg,(Sprite *)&popup_info_icon_sprite,0);
  }
}

/* Part of setup_choose_tribe (0051f19c): shows the names of a tribe again (a
   label id below 1 marks a name of a tribe a player has taken). */
static void tribe_restore_names(iptr tribe)
{
  iptr title;

  if (leaders[tribe].leader_name < 1) {
    leaders[tribe].leader_name = ~leaders[tribe].leader_name + 1;
  }
  if (leaders[tribe].plural < 1) {
    leaders[tribe].plural = ~leaders[tribe].plural + 1;
  }
  if (leaders[tribe].adjective < 1) {
    leaders[tribe].adjective = ~leaders[tribe].adjective + 1;
  }
  for (title = 0; title < 7; title++) {
    if (leaders[tribe].titles[title][0] < 1) {
      *(ushort *)&leaders[tribe].titles[title][0] =
           ~*(ushort *)&leaders[tribe].titles[title][0] + 1;
    }
    if (leaders[tribe].titles[title][1] < 1) {
      *(ushort *)&leaders[tribe].titles[title][1] =
           ~*(ushort *)&leaders[tribe].titles[title][1] + 1;
    }
  }
}

/* Part of setup_choose_tribe (0051f19c): enables the options of the tribes
   no human player has taken (in a network game, nor claimed), showing their
   names; returns how many. */
static int tribe_dialog_enable_free(Dialog *dlg)
{
  iptr leader;
  int avail_count;

  avail_count = 0;
  for (leader = 0; leader < 0x15; leader++) {
    if ((((uint)game.human_players & 1 << ((byte)leaders[leader].color & 0x1f)) == 0) &&
       (((int)civ_saved_cursor_x_block.net_claimed_civs_mask & 1 << ((byte)leaders[leader].color & 0x1f)) == 0)) {
      tribe_restore_names(leader);
    }
    if ((((uint)game.human_players & 1 << ((byte)leaders[leader].color & 0x1f)) == 0) &&
       (((int)civ_saved_cursor_x_block.net_claimed_civs_mask & 1 << ((byte)leaders[leader].color & 0x1f)) == 0)) {
      avail_count++;
      dlg->set_option_disabled(leader,0);
      if ((RadioGroup *)dlg->radio_group != (RadioGroup *)0x0) {
        ((RadioGroup *)dlg->radio_group)->enable_item(leader);
      }
    }
    else {
      dlg->set_option_disabled(leader,1);
      if ((RadioGroup *)dlg->radio_group != (RadioGroup *)0x0) {
        ((RadioGroup *)dlg->radio_group)->disable_item(leader);
      }
    }
  }
  return avail_count;
}

/* Part of setup_choose_tribe (0051f19c): selects the first enabled tribe
   from *sel_tribe on, wrapping around. */
static void tribe_dialog_preselect(Dialog *dlg,iptr *sel_tribe)
{
  int tries;
  DialogOption *option;

  for (tries = 0; tries < 0x15; tries++) {
    option = dlg->find_option(*sel_tribe);
    if ((option != (DialogOption *)0x0) && ((option->flags_byte & 1) == 0)) {
      dlg->set_default_choice(option->id);
      if ((RadioGroup *)dlg->radio_group != (RadioGroup *)0x0) {
        ((RadioGroup *)dlg->radio_group)->set_selected(option->id);
      }
      break;
    }
    (*sel_tribe)++;
    if (*sel_tribe == 0x15) {
      *sel_tribe = 0;
    }
  }
}

/* Part of setup_choose_tribe (0051f19c): in a network game, claims the tribe
   leader2 from the host and waits for its answer. TRIBE_RETRY: taken or
   locked, choose again. */
static TribeStep setup_tribe_net_claim(Dialog *dlg,iptr leader2,iptr *retry_tribe,int *result,int in_game)
{
  int now_ticks;
  int wait_start;

  net_reset_msg_counts();
  net_send_message(NM_LOCK_NATION,0,leader2,game.player_civ,0,0,0,0,0,0);
  if (net.host_flag == '\0') {
    wait_start = timer_ticks();
    while (((net.nm_nation_accept_count == 0 && (net.nm_nation_reject_count == 0)) &&
           (now_ticks = timer_ticks(), now_ticks - wait_start < net.timeout_seconds * 0x3c))) {
      if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
        setup_tribe_server_lost_popup((const char *)&str_lostserver_5,(const char *)&str_serverquit_4);
        *result = 1;
        return TRIBE_FINISH;
      }
      net.process_messages(1,0);
    }
  }
  if ((net.nm_nation_accept_count == 0) && (net.nm_nation_reject_count == 0)) {
    popup_game_with_sprite(str_serverconnectfail_3,(Sprite *)&popup_info_icon_sprite,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    dlg->close();
    return TRIBE_FINISH;
  }
  if (net.nm_nation_reject_count == 0) return TRIBE_NEXT;
  *retry_tribe = leader2;
  popup_game_with_sprite(str_alreadychosen_2,(Sprite *)&popup_info_icon_sprite,0);
  return TRIBE_RETRY;
}

/* Part of setup_choose_tribe (0051f19c): asks the name of the leader (the
   tribe chosen as it is). */
static TribeStep setup_tribe_leader_name(Dialog *dlg,char *input_text,int *result,iptr leader2,int player_slot,int game_mode,int in_game)
{
  int cancelled;

  do {
    if (in_game == 0) {
      setup_show_side_picture(7,0);
    }
    dlg->load_text_section((const char *)&str_game_7,(const char *)&str_name_5,0x17,(const char*)&civ_names[game.player_civ].leader_name,0,0,0,1);
    dialog_prepend_title_label(dlg,player_slot + 0x25e);
    if (in_game == 0) {
      dlg->set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3)
                  + 1));
    }
    if (2 < game_mode) {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
    }
    cancelled = dlg->run_modal(input_text,0);
  } while (input_text[0] == '\0');
  if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
    setup_tribe_server_lost_popup((const char *)&str_lostserver_9,(const char *)&str_serverquit_8);
    *result = 1;
    return TRIBE_FINISH;
  }
  snd_play_effect(SND_MENUOK,0,0,0);
  if (in_game == 0) {
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
  }
  if (cancelled != 0) {
    game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
    if (2 < game_mode) {
      net_send_message(NM_UNLOCK_NATION,0,leader2,game.player_civ,0,0,0,0,0,0);
    }
    return TRIBE_RETRY;
  }
  crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),input_text);
  crt_strcpy(((i8 *)&net.player_info_leader_name),input_text);
  net.player_info_tribe_name[0] = '\0';
  net.player_info_adjective[0] = '\0';
  return TRIBE_NEXT;
}

/* Part of setup_choose_tribe (0051f19c): the custom tribe: the names of the
   leader, the tribe and its adjective, then the titles of the leader under
   each government. */
static TribeStep setup_tribe_custom_names(Dialog *dlg,int *choice,int *result,iptr leader2,int player_slot,int game_mode,int in_game)
{
  char *str;
  iptr title;
  int cancelled;

  if (((net.player_info_tribe_name[0] == '\0') || (net.player_info_adjective[0] == '\0')) || (game_mode == 1)) {
    str = label_string((int)leaders[civs[game.player_civ].tribe].plural);
    crt_strcpy((char *)(&civ_names[game.player_civ].tribe_name),str);
    str = label_string((int)leaders[civs[game.player_civ].tribe].adjective);
    crt_strcpy((char *)(&civ_names[game.player_civ].adjective),str);
  }
  else {
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),((i8 *)&net.player_info_leader_name));
    crt_strcpy((char *)(&civ_names[game.player_civ].tribe_name),((i8 *)&net.player_info_tribe_name));
    crt_strcpy((char *)(&civ_names[game.player_civ].adjective),((i8 *)&net.player_info_adjective));
  }
  leaders[civs[game.player_civ].tribe].plural = -leaders[civs[game.player_civ].tribe].plural;
  leaders[civs[game.player_civ].tribe].adjective =
       -leaders[civs[game.player_civ].tribe].adjective;
  for (title = 0; title < 7; title++) {
    str = label_string((int)leaders[civs[game.player_civ].tribe].titles[title][*choice]);
    crt_strcpy((char *)(&civ_names[game.player_civ].titles[title]),str);
    leaders[civs[game.player_civ].tribe].titles[title][*choice] =
         -leaders[civs[game.player_civ].tribe].titles[title][*choice]
    ;
  }
  while( true ) {
    text_set_string_arg_b(1,(const char *)&civ_names[game.player_civ].tribe_name);
    text_set_string_arg_b(2,(const char *)&civ_names[game.player_civ].adjective);
    if (in_game == 0) {
      setup_show_side_picture(7,0);
    }
    cancelled = dlg->load_text_section((const char *)&str_game_7,(const char *)&str_customtribe_2,0x17,(const char*)&civ_names[game.player_civ].leader_name,
                          0,0,0,1);
    if (cancelled != 0) {
      game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
      if (in_game == 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      }
      if (2 < game_mode) {
        net_send_message(NM_UNLOCK_NATION,0,leader2,game.player_civ,0,0,0,0,0,0);
      }
      return TRIBE_FINISH;
    }
    dialog_prepend_title_label(dlg,player_slot + 0x25e);
    if (in_game == 0) {
      dlg->set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3)
                  + 1));
    }
    if (2 < game_mode) {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
    }
    *choice = dlg->show_modal(0);
    snd_play_effect(SND_MENUOK,0,0,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
      setup_tribe_server_lost_popup((const char *)&str_lostserver_7,(const char *)&str_serverquit_6);
      *result = 1;
      return TRIBE_FINISH;
    }
    if (*choice < 0) break;
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),((i8 *)&text_args.strings));
    crt_strcpy((char *)(&civ_names[game.player_civ].tribe_name),((i8 *)&text_args.string1));
    crt_strcpy((char *)(&civ_names[game.player_civ].adjective),((i8 *)&text_args.string2));
    crt_strcpy(((i8 *)&net.player_info_leader_name),((i8 *)&text_args.strings));
    crt_strcpy(((i8 *)&net.player_info_tribe_name),((i8 *)&text_args.string1));
    crt_strcpy(((i8 *)&net.player_info_adjective),((i8 *)&text_args.string2));
    net.player_info_has_names = 1;
    if (dlg->result_extra != 1) break;
    if (in_game == 0) {
      setup_show_side_picture(7,0);
    }
    cancelled = dlg->load_game_section((const char *)&str_customtribe2_2,1);
    if (cancelled != 0) {
      game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
      if (in_game == 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      }
      if (2 < game_mode) {
        net_send_message(NM_UNLOCK_NATION,0,leader2,game.player_civ,0,0,0,0,0,0);
      }
      return TRIBE_FINISH;
    }
    dialog_prepend_title_label(dlg,player_slot + 0x25e);
    for (title = 0; title < 7; title++) {
      text_clear();
      text_append_string_id(government_names[title]);
      text_append_colon();
      if (((((i8 *)&net.player_info_govt_titles))[title * 0x18] == '\0') || (game_mode == 1)) {
        dlg->add_edit_field((const char *)&textbuf.buf,(const char*)&civ_names[game.player_civ].titles[title],0x17);
      }
      else {
        dlg->add_edit_field((const char *)&textbuf.buf,
                   (((i8 *)&net.player_info_govt_titles) + title * 0x18),0x17);
      }
    }
    if (in_game == 0) {
      dlg->set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3)
                  + 1));
    }
    if (2 < game_mode) {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
    }
    *choice = dlg->show_modal(0);
    if (*choice == 0) {
      for (title = 0; title < 7; title++) {
        crt_strcpy((char *)(&civ_names[game.player_civ].titles[title]),
                     ((i8 *)&text_args.strings) + title * 0x104);
        crt_strcpy(((i8 *)&net.player_info_govt_titles) + title * 0x18,((i8 *)&text_args.strings) + title * 0x104);
      }
      net.player_info_has_titles = 1;
    }
    snd_play_effect(SND_MENUOK,0,0,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
  }
  if (*choice < 0) {
    game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
    if (2 < game_mode) {
      net_send_message(NM_UNLOCK_NATION,0,leader2,game.player_civ,0,0,0,0,0,0);
    }
    return TRIBE_RETRY;
  }
  if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
    setup_tribe_server_lost_popup((const char *)&str_lostserver_8,(const char *)&str_serverquit_7);
    *result = 1;
    return TRIBE_FINISH;
  }
  return TRIBE_NEXT;
}

/* Part of setup_choose_tribe (0051f19c): the city style of the tribe, then
   in a play-by-email game the e-mail address of the player. TRIBE_FINISH
   when the tribe is chosen (or the server lost). */
static TribeStep setup_tribe_city_style(Dialog *dlg,char *input_text,int *choice,int *result,iptr leader2,int player_slot,int game_mode,int in_game)
{
  iptr label;
  i8 *email_default;
  int style2;
  int cancelled;

  /* BF-102 (Peter Johnson's ledger): a choice past the four styles (a row
     added to GAME.TXT's @CUSTOMCITY) is asked again (the original stored it
     as the style, past the CITIES graphics) */
  do {
    if (in_game == 0) {
      setup_show_side_picture(8,0);
    }
    dlg->load_game_section((const char *)&str_customcity_2,0x20001);
    dialog_prepend_title_label(dlg,player_slot + 0x25e);
    for (style2 = 0; style2 < 4; style2++) {
      label = (iptr)label_string(label_table.string_ids[0xf6 + style2]);
      dlg->add_sprite((Sprite *)(style2 * 0x1e0 + (iptr)(((u8 *)&gfx_sheet_surface_block.city_walled_sprites + 360))),style2,(const char *)label);
    }
    dlg->set_default_choice((int)leaders[civs[game.player_civ].tribe].city_style);
    if (in_game == 0) {
      dlg->set_position(
                 -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1)
                 ,-(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1
                   ));
    }
    if (2 < game_mode) {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
    }
    *choice = dlg->show_modal(0);
    if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
      setup_tribe_server_lost_popup((const char *)&str_lostserver_10,(const char *)&str_serverquit_9);
      *result = 1;
      return TRIBE_FINISH;
    }
    snd_play_effect(SND_MENUOK,0,0,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
  } while ((-1 < *choice) && (3 < (*choice & 0xffff)));
  if (*choice < 0) {
    game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
    if (2 < game_mode) {
      net_send_message(NM_UNLOCK_NATION,0,leader2,game.player_civ,0,0,0,0,0,0);
    }
  }
  else {
    net.player_info_city_style = *choice;
    civ_names[game.player_civ].city_style = (i16)*choice;
    *result = 0;
    game_section_dialog_block.setup_slot_tribes[player_slot + 1] = leader2;
    if (game_mode != 2) return TRIBE_FINISH;
    if (in_game == 0) {
      setup_show_side_picture(7,0);
    }
    if (player_slot == 0) {
      email_default = ((i8 *)&menu_prefs.email_address);
    }
    else {
      email_default = ((i8 *)&setup_input_blank_default);
    }
    dlg->load_text_section((const char *)&str_game_7,(const char *)&str_emailaddress,0x1f,email_default,0,0,0,1);
    dialog_prepend_title_label(dlg,player_slot + 0x25e);
    if (in_game == 0) {
      dlg->set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) +
                  1));
    }
    cancelled = dlg->run_modal(input_text,0);
    if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
      setup_tribe_server_lost_popup((const char *)&str_lostserver_11,(const char *)&str_serverquit_10);
      *result = 1;
      return TRIBE_FINISH;
    }
    snd_play_effect(SND_MENUOK,0,0,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    if (cancelled == 0) {
      crt_strcpy(((i8 *)&civ_saved_cursor_x_block.civ_email_addresses) + game.player_civ * 0x20,input_text);
      if (player_slot == 0) {
        crt_strcpy(((i8 *)&menu_prefs.email_address),input_text);
        saved_turn.civ = *(i8 *)&human_civ;
        turn_loop.resume_civ = *(i8 *)&human_civ;
      }
      return TRIBE_FINISH;
    }
    game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
  }
  return TRIBE_RETRY;
}

/* 0051f19c auto */
/* WARNING: Removing unreachable block (ram,0x005212c5) */
/* WARNING: Removing unreachable block (ram,0x005213cc) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl setup_choose_tribe(int player_slot,int game_mode,int in_game)

{
  int choice;
  iptr label;
  int now_ticks;
  char *str;
  iptr tribe_arg;
  iptr leader;
  u8 is_female;
  char input_text[260];
  int avail_count;
  int result;
  iptr sel_tribe;
  Dialog dlg;
  iptr retry_tribe;
  iptr leader2;
  iptr title;
  int cancelled;
  TribeStep step;
  int stop;
  
  stop = 0;
  leader2 = -1;
  result = 1;
  retry_tribe = -1;
  dlg.Dialog::construct(0x2000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  sel_tribe = 0;
  do {
    if (-1 < leader2) {
      tribe_restore_names(leader2);
    }
    if (in_game == 0) {
      setup_show_side_picture(6,0);
    }
    dlg.load_game_section((const char *)&str_gender_5,1);
    dialog_prepend_title_label(&dlg,player_slot + 0x25e);
    dlg.set_default_choice((int)prefs.gender);
    if (in_game == 0) {
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
    }
    if (2 < game_mode) {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
    }
    choice = dlg.show_modal(0);
    /* BF-113 */
    if (1 < choice) {
      choice = -1;
    }
    snd_play_effect(SND_MENUOK,0,0,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
      setup_tribe_server_lost_popup((const char *)&str_lostserver_2,(const char *)&str_serverquit);
      result = 1;
      break;
    }
    if (choice < 0) break;
    prefs.gender = (short)choice;
    net.player_info_gender = (int)prefs.gender;
    if (in_game == 0) {
      setup_show_side_picture(0xffffffff,0);
    }
    cancelled = dlg.load_text((const char *)&str_game_6,(const char *)&str_tribe_2,0x801);
    if (cancelled != 0) {
      if (in_game == 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      }
      break;
    }
    if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
      setup_tribe_server_lost_popup((const char *)&str_lostserver_3,(const char *)&str_serverquit_2);
      result = 1;
      break;
    }
    dialog_prepend_title_label(&dlg,player_slot + 0x25e);
    dlg.set_columns(3);
    avail_count = 0;
    for (leader = 0; leader < 0x15; leader++) {
      if ((((uint)game.human_players & 1 << ((byte)leaders[leader].color & 0x1f)) == 0) &&
         (((int)civ_saved_cursor_x_block.net_claimed_civs_mask & 1 << ((byte)leaders[leader].color & 0x1f)) == 0)) {
        tribe_restore_names(leader);
      }
      if ((((uint)game.human_players & 1 << ((byte)leaders[leader].color & 0x1f)) == 0) ||
         (-1 < leaders[leader].plural)) {
        str = 0;
        tribe_arg = leader;
        label = (iptr)label_string((int)leaders[leader].plural);
        dlg.add_option((const char *)label,tribe_arg,(uint)str);
      }
      else {
        dlg.add_option((const char *)&civ_names[leaders[leader].color].tribe_name,leader
                   ,0);
      }
    }
    for (;;) {
      avail_count = tribe_dialog_enable_free(&dlg);
      if (avail_count == 0) {
        popup_game((const char *)&str_joinedmax_2);
        dlg.close();
        stop = 1;
        break;
      }
      tribe_dialog_preselect(&dlg,&sel_tribe);
      if (in_game == 0) {
        dlg.set_position(0xfffffc19,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      }
      if (2 < game_mode) {
        net_reset_msg_counts();
        net.wait_start_ticks = timer_ticks();
      }
      dlg.style_flags = dlg.style_flags & 0xfffffbff;
      dlg.result_id = 0;
      dlg.result_extra = 0;
      if (2 < game_mode) {
        dlg_style.pending_idle_callback = net_lock_wait_idle_cb;
      }
      leader2 = dlg.show_modal(0);
      sel_tribe = ((RadioGroup *)dlg.radio_group)->get_selected();
      if (2 < game_mode) {
        if (net.nm_lock_data_count != 0) {
          retry_tribe = leader2;
          continue;
        }
        if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
          setup_tribe_server_lost_popup((const char *)&str_lostserver_4,(const char *)&str_serverquit_3);
          if (in_game == 0) {
            ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
          }
          dlg.close();
          stop = 1;
          break;
        }
      }
      snd_play_effect(SND_MENUOK,0,0,0);
      if (leader2 < 0) break;
      sel_tribe = leader2;
      net.player_info_leader_index = leader2;
      game.player_civ = (i8)leaders[leader2].color;
      net.player_info_civ = (int)game.player_civ;
      if (2 < game_mode) {
        step = setup_tribe_net_claim(&dlg,leader2,&retry_tribe,&result,in_game);
        if (step == TRIBE_FINISH) {
          stop = 1;
          break;
        }
        if (step == TRIBE_RETRY) continue;
      }
      break;
    }
    if (stop != 0) break;
    if (leader2 < 0) {
      if (in_game == 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      }
      dlg.close();
      continue;
    }
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    prefs.tribe = (i16)leader2;
    civs[game.player_civ].tribe = prefs.tribe;
    civs[game.player_civ].leader_portrait = (u8)leader2;
    if (choice != 0) {
      civs[game.player_civ].leader_portrait += 21;
    }
    (*(i8 *)&human_civ) = (iptr)game.player_civ;
    game.human_players = (byte)(1 << (game.player_civ & 0x1fU)) | game.human_players;
    is_female = (u8)choice;
    leaders[civs[game.player_civ].tribe].female = is_female;
    leaders[civs[game.player_civ].tribe].leader_name =
         leaders[civs[game.player_civ].tribe].names[((uint)leaders[civs[game.player_civ].tribe].female)];
    str = label_string((int)leaders[civs[game.player_civ].tribe].leader_name);
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),str);
    leaders[civs[game.player_civ].tribe].leader_name =
         -leaders[civs[game.player_civ].tribe].leader_name;
    now_ticks = civ_check_password((*(i8 *)&human_civ));
    if (now_ticks != 0) {
      net.player_info_has_names = 0;
      net.player_info_has_titles = 0;
      if (dlg.result_extra == 0) {
        step = setup_tribe_leader_name(&dlg,input_text,&result,leader2,player_slot,game_mode,in_game);
      }
      else {
        step = setup_tribe_custom_names(&dlg,&choice,&result,leader2,player_slot,game_mode,in_game);
      }
      if (step == TRIBE_FINISH) break;
      if (step == TRIBE_RETRY) continue;
      step = setup_tribe_city_style(&dlg,input_text,&choice,&result,leader2,player_slot,game_mode,in_game);
      if (step == TRIBE_FINISH) break;
      continue;
    }
    if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
      setup_tribe_server_lost_popup((const char *)&str_lostserver_6,(const char *)&str_serverquit_5);
      result = 1;
      break;
    }
    snd_play_effect(SND_MENUOK,0,0,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
    if (2 < game_mode) {
      net_send_message(NM_UNLOCK_NATION,0,leader2,game.player_civ,0,0,0,0,0,0);
    }
  } while( true );
  choice = result;
  if ((result == 1) && (-1 < leader2)) {
    tribe_restore_names(leader2);
  }
  if (((in_game == 0) && (result == 1)) && (0 < player_slot)) {
    if (leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].leader_name < 1) {
      leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].leader_name =
           ~leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].leader_name + 1;
    }
    if (leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].plural < 1) {
      leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].plural =
           ~leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].plural + 1;
    }
    if (leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].adjective < 1) {
      leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].adjective =
           ~leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].adjective + 1;
    }
    for (title = 0; title < 7; title++) {
      if (leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][0]
          < 1) {
        *(ushort *)&leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][0]
             = ~*(ushort *)&leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][0] +
               1;
      }
      if (leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][1]
          < 1) {
        *(ushort *)&leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][1]
             = ~*(ushort *)&leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][1] +
               1;
      }
    }
    game.human_players =
         ~(byte)(1 << ((byte)leaders[game_section_dialog_block.setup_slot_tribes[player_slot]].color & 0x1f)) &
         game.human_players;
  }
  return choice;
}

/* 005218cb auto */
i32 __cdecl scenario_start_dialog(int game_mode)

{
  iptr load_result;
  i32 value;
  iptr text_result;
  int unit;
  GamePalette palette;
  Dialog dlg;
  int opt;
  i8 dst[260];
  int difficulty2;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  for (;;) {
    do {
      if (cd_music.available == 0) {
        snd_cd_stop();
        snd_play_effect(SND_MENULOOP,0,1,0);
      }
      ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
      main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
      main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
      wnd_flush_paint();
      load_result = game_load_dialog(1,game_mode,1);
      if (load_result != 0) {
        snd_play_effect(SND_MENUOK,0,0,0);
        ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
        main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
        main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
        wnd_flush_paint();
        value = 1;
        return value;
      }
      path_set_extension(dst,(char *)(&textbuf.buf),lang_file_ext);
      if (cd_music.available != 0) {
        snd_play_effect(SND_MENUOK,0,0,0);
      }
      game.game_options = prefs.game_options & 0xffff7fff;
      game.message_options = prefs.message_options;
      text_set_string_arg(0,(const char *)&scenario.name);
      text_set_number_arg(0,(int)scenario.tech_paradigm);
      text_clear();
      value = game_turn_to_year((int)game.turn);
      text_append_year(value);
      text_set_string_arg(1,(const char *)&textbuf.buf);
      text_clear();
      if (scenario.max_turns == 0) {
        text_append_year(2000);
      }
      else {
        value = game_turn_to_year((int)scenario.max_turns);
        text_append_year(value);
      }
      text_set_string_arg(2,(const char *)&textbuf.buf);
      if (cd_music.available != 0) {
        snd_menu_end();
      }
      dlg.init_default();
      text_result = dlg.load_text((const char *)dst,(const char *)&str_scenario_3,0x4008001);
      if (text_result == 0) {
        unit = (int)game.active_unit;
        if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
           ((int)units[unit].owner == (int)scenario.objective_civ)) {
          dlg.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),0
                     ,0);
        }
        unit = dlg.show_modal(0);
      }
      else {
        unit = popup_game_flags((const char *)&str_scenarioloaded_4,1);
      }
    } while (unit < 0);
    while( true ) {
      dlg.load_game_section((const char *)&str_difficulty_6,1);
      dlg.set_default_choice((uint)game.difficulty);
      dlg.set_position(
                 ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      difficulty2 = dlg.show_modal(0);
      if (difficulty2 < 0) break;
      game.difficulty = (u8)difficulty2;
      if ((u8)difficulty2 == '\0') {
        game.game_options = game.game_options | OPT_TUTORIAL;
      }
      else {
        game.game_options = game.game_options & ~OPT_TUTORIAL;
      }
      while( true ) {
        saved_turn.resume_new_game = 0;
        turn_resume_new_game = 0;
        dlg_checks_clear();
        dlg_check_set(0,(int)menu_prefs.mp_open_game);
        dlg_check_set(1,(int)menu_prefs.mp_kill_on_retire);
        dlg_check_set(2,(int)menu_prefs.mp_show_human_starts);
        dlg_check_set(3,(int)menu_prefs.mp_chat_from_start);
        dlg_check_set(4,(int)menu_prefs.mp_double_production);
        dlg_check_set(5,(int)menu_prefs.mp_double_moves);
        dlg.load_text_section((const char *)&str_game_7,(const char *)&str_advancedmp_3,0,0,0,0,0,0);
        if (net.ini_simultaneous_allowed == 0) {
          net.mp_simultaneous_moves = 0;
          mp_options.simultaneous_setting = 0;
        }
        else {
          text_clear();
          text_append_label(0x364);
          dlg.add_option((const char *)&textbuf.buf,6,0);
          dlg_check_set(6,(int)menu_prefs.mp_simultaneous_moves);
        }
        if (((byte)dlg.style_flags & 4) != 0) {
          for (opt = 0; opt < (int)dlg.option_count; opt++) {
            dlg.set_option_checked(opt,
                       1 << ((byte)opt & 0x1f) & dlg_input.check_bits);
          }
        }
        dlg.set_position(
                   ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1,
                   -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1)
                  );
        unit = dlg.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        if (unit < 0) break;
        value = dlg.option_is_checked(0);
        mp_options.open_game = (short)value;
        menu_prefs.mp_open_game = mp_options.open_game;
        value = dlg.option_is_checked(1);
        mp_options.kill_civ_on_retire = (short)value;
        menu_prefs.mp_kill_on_retire = mp_options.kill_civ_on_retire;
        value = dlg.option_is_checked(2);
        mp_options.show_human_starts = (short)value;
        menu_prefs.mp_show_human_starts = mp_options.show_human_starts;
        value = dlg.option_is_checked(3);
        mp_options.chat_from_start = (short)value;
        menu_prefs.mp_chat_from_start = mp_options.chat_from_start;
        value = dlg.option_is_checked(4);
        mp_doubles.production = (short)value;
        menu_prefs.mp_double_production = mp_doubles.production;
        value = dlg.option_is_checked(5);
        mp_doubles.moves = (short)value;
        menu_prefs.mp_double_moves = mp_doubles.moves;
        if (net.ini_simultaneous_allowed != 0) {
          value = dlg.option_is_checked(6);
          mp_options.simultaneous_setting = (short)value;
          menu_prefs.mp_simultaneous_moves = mp_options.simultaneous_setting;
        }
        if ((game_mode == 2) || (unit = setup_choose_game_timer(0), unit == 0)) {
          value = 0;
          return value;
        }
      }
    }
  }
}

/* 00521fe0 auto */
i32 __cdecl scenario_pick_human_civs(int num_players)

{
  char *str;
  iptr choice;
  int result;
  i32 ret;
  uptr civ;
  u8 is_female;
  iptr sex;
  i8 name_text[260];
  int has_default;
  int player_idx;
  Dialog dlg;
  DialogOption *option;
  iptr govt;
  uptr option_civs[8];
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  game.human_players = '\0';
  (*(byte *)&turn_loop.resume_civ) = 0;
  player_idx = 0;
  do {
    if (num_players <= player_idx) {
      (*(byte *)&saved_turn.civ) = (*(byte *)&turn_loop.resume_civ);
      human_civ[0] = (iptr)(*(byte *)&turn_loop.resume_civ);
      game.turn_civ = (*(byte *)&turn_loop.resume_civ);
      saved_turn.resume_new_game = 1;
      turn_resume_new_game = 1;
      ret = 0;
      return ret;
    }
    while( true ) {
      while( true ) {
        do {
          dlg.Dialog::reset(1);
          dialog_prepend_title_label(&dlg,player_idx + 0x25e);
          has_default = 0;
          for (civ = 1; (int)civ < 8; civ++) {
            if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
              text_clear();
              str = civ_name_plural(civ);
              text_append(str);
              text_append_space();
              text_append_lparen();
              str = civ_leader_name(civ);
              text_append(str);
              text_append_rparen();
              text_append_space();
              text_append_lparen();
              text_append_int(civs[civ].num_cities);
              text_append_space();
              text_append_label(0xc5);
              text_append((const char *)&str_empty_163);
              text_append_int(civs[civ].num_military_units);
              text_append_space();
              text_append_label(0xc4);
              text_append((const char *)&str_empty_164);
              option = (dlg.add_option((const char *)&textbuf.buf,civ,0));
              option_civs[(uint)option->id] = civ;
              if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
                if (has_default == 0) {
                  has_default = 1;
                  dlg.set_default_choice((uint)option->id);
                }
              }
              else {
                dlg.set_option_disabled((uint)option->id,1);
              }
            }
          }
          choice = dlg.show_modal(0);
          if (choice < 0) {
            ret = 1;
            return ret;
          }
          human_civ[0] = option_civs[choice];
          (*((char *)&civ + 0)) = (byte)human_civ[0];
          if (player_idx == 0) {
            (*(byte *)&turn_loop.resume_civ) = (byte)civ;
          }
          game.human_players = game.human_players | (byte)(1 << ((byte)civ & 0x1f));
          game.player_civ = (byte)civ;
          game.turn_civ = (byte)civ;
          game.game_options = game.game_options & ~OPT_TUTORIAL;
          result = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_6,0x17,(const char*)&civ_names[((char)(byte)civ)].leader_name,0,0,0,1);
        } while (result != 0);
        dialog_prepend_title_label(&dlg,player_idx + 0x25e);
        result = dlg.run_modal(name_text,0);
        if (result == 0) break;
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
      }
      crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),name_text);
      dlg.load_game_section((const char *)&str_gender_6,1);
      dialog_prepend_title_label(&dlg,player_idx + 0x25e);
      dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      result = dlg.show_modal(0);
      /* BF-113 */
      if (1 < result) {
        result = -1;
      }
      snd_play_effect(SND_MENUOK,0,0,0);
      if (-1 < result) break;
      game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
    }
    is_female = (u8)result;
    leaders[civs[game.player_civ].tribe].female = is_female;
    civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
    if (result != 0) {
      civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
    }
    for (govt = 0; govt < 7; govt++) {
      for (sex = 0; sex < 2; sex++) {
        leaders[civs[game.player_civ].tribe].titles[govt][sex] =
             (short)(&government_titles[govt].male)[sex];
      }
    }
    player_idx++;
  } while( true );
}

/* 0052263c auto */
void __cdecl setup_show_side_picture(int picture_id,int reset)

{
  if (reset != 0) {
    setup_dlg.side_picture_id = 5;
  }
  if (picture_id == -1) {
    if (setup_dlg.side_picture_id == 5) {
      setup_dlg.side_picture_id = 7;
    }
    else if (setup_dlg.side_picture_id == 6) {
      setup_dlg.side_picture_id = 5;
    }
    else if (setup_dlg.side_picture_id == 7) {
      setup_dlg.side_picture_id = 6;
    }
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,setup_dlg.side_picture_id);
  }
  else {
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(2,6,picture_id);
  }
  return;
}

/* 005226fa auto */
int __cdecl setup_next_dialog_x(int reset)

{
  int x;
  
  if (reset != 0) {
    setup_dlg.dialog_pos_state = 1;
  }
  switch(setup_dlg.dialog_pos_state) {
  case 1:
    setup_dlg.dialog_pos_state = 3;
    x = -999;
    break;
  case 2:
    setup_dlg.dialog_pos_state = 1;
    x = ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1;
    break;
  case 3:
    setup_dlg.dialog_pos_state = 4;
    x = -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1);
    break;
  case 4:
    setup_dlg.dialog_pos_state = 2;
    x = -999;
    break;
  default:
    x = -999;
  }
  return x;
}

/* 005227e3 auto */
int __cdecl setup_choose_hotseat_players(void)

{
  int result;
  int civ;
  Dialog dlg;
  int num_players;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if (game.multiplayer < 3) {
    setup_show_side_picture(0xffffffff,0);
    dlg.load_game_section((const char *)&str_hotseat2,1);
    if (4 < net.net_max_players) {
      text_clear();
      text_append_label(0x365);
      dlg.add_option((const char *)&textbuf.buf,3,0);
    }
    if (5 < net.net_max_players) {
      text_clear();
      text_append_label(0x366);
      dlg.add_option((const char *)&textbuf.buf,4,0);
    }
    if (6 < net.net_max_players) {
      text_clear();
      text_append_label(0x367);
      dlg.add_option((const char *)&textbuf.buf,5,0);
    }
    dlg.set_default_choice((int)menu_prefs.hotseat_players_choice);
    dlg.set_position(
               -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1),
               -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
    switch(game.num_enemies) {
    case 1:
      dlg.set_option_disabled(1,1);
    case 2:
      dlg.set_option_disabled(2,1);
    case 3:
      if (4 < net.net_max_players) {
        dlg.set_option_disabled(3,1);
      }
    case 4:
      if (5 < net.net_max_players) {
        dlg.set_option_disabled(4,1);
      }
    case 5:
      if (6 < net.net_max_players) {
        dlg.set_option_disabled(5,1);
      }
    }
    if (menu_prefs.hotseat_players_choice + 1 < (int)(uint)game.num_enemies) {
      dlg.set_default_choice((int)menu_prefs.hotseat_players_choice);
    }
    num_players = dlg.show_modal(0);
    if (num_players < 0) {
      result = 0;
      return result;
    }
    menu_prefs.hotseat_players_choice = (short)num_players;
    num_players += 2;
    snd_play_effect(SND_MENUOK,0,0,0);
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
  }
  else {
    num_players = 0;
    for (civ = 1; civ < 8; civ++) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
        num_players++;
      }
    }
  }
  result = num_players;
  return result;
}

/* 00522b2b auto */
void __cdecl game_join_dialog(void)

{
  int picked;
  iptr civ_name;
  iptr saved_human_civ;
  int human_count;
  int civ;
  Dialog dlg;
  uint saved_humans;
  int saved_player_civ;
  int ai_count;
  int alive_count;
  int view_idx;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  saved_human_civ = human_civ[0];
  saved_player_civ = (int)game.player_civ;
  saved_humans = (uint)game.human_players;
  game.human_players = net.hotseat_human_players;
  mp_turn_timer_stop(1);
  city_window_close();
  ((WndBase *)((i8 *)&status_wnd.wnd))->hide();
  ((WndBase *)((i8 *)&minimap_state.wnd))->hide();
  for (view_idx = 0; view_idx < 8; view_idx++) {
    if (map_views_block.map_view_recs[view_idx].open != 0) {
      ((WndBase *)&map_views_block.map_view_recs[view_idx].window)->hide();
    }
  }
  wnd_flush_paint();
  alive_count = 0;
  ai_count = 0;
  human_count = 0;
  for (civ = 1; civ < 8; civ++) {
    if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
      alive_count++;
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
        ai_count++;
      }
      else {
        human_count++;
      }
    }
  }
  if (net.net_max_players == human_count) {
    text_set_number_arg(0,net.net_max_players);
    popup_game((const char *)&str_joingame1);
  }
  else if (ai_count == 0) {
    popup_game((const char *)&str_joingame0);
  }
  else {
    picked = setup_join_pick_civ(human_count - 1);
    if (picked == 0) {
      civ_name = (iptr)(civ_name_plural(human_civ[0]));
      text_set_string_arg_b(0,(const char *)civ_name);
      popup_game((const char *)&str_joingame3);
    }
  }
  game.player_civ = (i8)saved_player_civ;
  net.hotseat_human_players = game.human_players;
  game.human_players = (u8)saved_humans;
  human_civ[0] = saved_human_civ;
  ((SurfaceWindow *)((i8 *)&status_wnd.window))->SurfaceWindow::show();
  ((SurfaceWindow *)((i8 *)&minimap_state.window))->SurfaceWindow::show();
  for (view_idx = 0; view_idx < 8; view_idx++) {
    if (map_views_block.map_view_recs[view_idx].open != 0) {
      ((SurfaceWindow *)&map_views_block.map_view_recs[view_idx])->SurfaceWindow::show();
    }
  }
  mp_turn_timer_resume(1);
  return;
}

/* 00522f8f auto */
void __cdecl setup_choose_opponents(int num_humans)

{
  byte sign;
  int rnd;
  i32 dlg_x;
  iptr civ;
  Dialog dlg;
  iptr leader;
  int i;
  int opponent_num;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  game.civs_alive = game.human_players | 1;
  for (leader = 1; leader < 8; leader++) {
    if ((1 << ((byte)leader & 0x1f) & (uint)game.human_players) == 0) {
      if (leaders[leader].leader_name < 1) {
        leaders[leader].leader_name = ~leaders[leader].leader_name + 1;
      }
      if (leaders[leader].plural < 1) {
        leaders[leader].plural = ~leaders[leader].plural + 1;
      }
      if (leaders[leader].adjective < 1) {
        leaders[leader].adjective = ~leaders[leader].adjective + 1;
      }
      for (i = 0; i < 7; i++) {
        if (leaders[leader].titles[i][0] < 1) {
          *(ushort *)&leaders[leader].titles[i][0] =
               ~*(ushort *)&leaders[leader].titles[i][0] + 1;
        }
        if (leaders[leader].titles[i][1] < 1) {
          *(ushort *)&leaders[leader].titles[i][1] =
               ~*(ushort *)&leaders[leader].titles[i][1] + 1;
        }
      }
    }
  }
  for (i = 0; i < ((game.num_enemies + 1) - num_humans); i++) {
    rnd = _rand();
    sign = (byte)(rnd >> 0x1f);
    (*((char *)&civ + 0)) = (((byte)rnd ^ sign) - sign & 7 ^ sign) - sign;
    if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
      game.civs_alive = game.civs_alive | (byte)(1 << ((byte)civ & 0x1f));
    }
    else {
      i--;
    }
  }
  if (setup_dlg.select_opponents != 0) {
    opponent_num = 0;
    for (civ = 1; civ < 8; civ++) {
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) {
        opponent_num++;
        text_set_number_arg(0,opponent_num);
        setup_show_side_picture(9,0);
        dlg.load_game_section((const char *)&str_opponent_2,1);
        dlg.option_list->id = 0x15;
        for (leader = 0; leader < 0x15; leader++) {
          if (leaders[leader].color == civ) {
            text_clear();
            text_append_string_id((int)leaders[leader].plural);
            text_append_space();
            text_append_lparen();
            text_append_string_id((int)leaders[leader].leader_name);
            text_append_rparen();
            dlg.add_option((const char *)&textbuf.buf,leader,0);
          }
        }
        rnd = -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1);
        dlg_x = setup_next_dialog_x(0);
        dlg.set_position(dlg_x,rnd);
        leader = dlg.show_modal(0);
        snd_play_effect(SND_MENUOK,0,0,0);
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
        if (leader < 0) {
          return;
        }
        if (leader < 0x15) {
          (((i8 *)&setup_chosen_leaders))[civ] = (byte)leader;
        }
      }
    }
  }
  return;
}

/* 00523f02 auto */
i32 __cdecl setup_join_pick_civ(int player_idx)

{
  char *str;
  iptr choice;
  int result;
  iptr pw_ok;
  int civ;
  i32 ret;
  i32 arg_b;
  i32 arg_c;
  i32 arg_d;
  iptr civ2;
  u8 is_female;
  iptr sex;
  i8 name_text[260];
  int opt_idx;
  int avail_count;
  int opt_count;
  int sel_idx;
  Dialog dlg;
  DialogOption *option;
  iptr govt;
  iptr option_civs[8];
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  sel_idx = 0;
  do {
    dlg.Dialog::reset(1);
    dialog_prepend_title_label(&dlg,player_idx + 0x25f);
    opt_idx = 0;
    for (civ2 = 1; civ2 < 8; civ2++) {
      if ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0) {
        text_clear();
        str = civ_name_plural(civ2);
        text_append(str);
        text_append_space();
        text_append_lparen();
        str = civ_leader_name(civ2);
        text_append(str);
        text_append_rparen();
        text_append_space();
        text_append_lparen();
        text_append_int(civs[civ2].num_cities);
        text_append_space();
        text_append_label(0xc5);
        text_append((const char *)&str_empty_165);
        text_append_int(civs[civ2].num_military_units);
        text_append_space();
        text_append_label(0xc4);
        text_append((const char *)&str_empty_166);
        civ = opt_idx;
        opt_idx++;
        option = (DialogOption *)((byte *)dlg.add_option((const char *)&textbuf.buf,civ,0));
        option_civs[option->id] = civ2;
      }
    }
    opt_count = opt_idx;
    avail_count = 0;
    for (opt_idx = 0; opt_idx < opt_count; opt_idx++) {
      option = dlg.find_option(opt_idx);
      if (option != (DialogOption *)0x0) {
        (*((char *)&civ2 + 0)) = (byte)option_civs[option->id];
        if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
          avail_count++;
          dlg.set_option_disabled(option->id,0)
          ;
          if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
            ((RadioGroup *)dlg.radio_group)->enable_item(option->id);
          }
        }
        else {
          dlg.set_option_disabled(option->id,1)
          ;
          if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
            ((RadioGroup *)dlg.radio_group)->disable_item(option->id);
          }
        }
      }
    }
    sel_idx = 0;
    for (opt_idx = 0; opt_idx < opt_count; opt_idx++) {
      option = dlg.find_option(sel_idx);
      if ((option != (DialogOption *)0x0) && ((option->flags_byte & 1) == 0)) {
        dlg.set_default_choice(option->id);
        if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
          ((RadioGroup *)dlg.radio_group)->set_selected(option->id);
        }
        break;
      }
      sel_idx++;
      if (sel_idx == opt_count) {
        sel_idx = 0;
      }
    }
    choice = dlg.show_modal(0);
    if (choice < 0) {
      ret = 1;
      return ret;
    }
    civ = option_civs[choice];
    (*((char *)&civ2 + 0)) = (byte)civ;
    game.human_players = game.human_players | (byte)(1 << ((byte)civ2 & 0x1f));
    arg_d = 1;
    arg_c = 0;
    arg_b = 0;
    ret = 0;
    human_civ[0] = civ;
    str = civ_leader_name(civ);
    result = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_7,0x17,str,(SurfaceWindow *)ret,(DibSurface *)arg_b,(RECT *)arg_c,arg_d);
    if (result == 0) {
      dialog_prepend_title_label(&dlg,player_idx + 0x25f);
      result = dlg.run_modal(name_text,0);
      if (result == 0) {
        crt_strcpy((char *)(&civ_names[civ].leader_name),name_text);
        if (0 < leaders[civs[civ].tribe].leader_name) {
          leaders[civs[civ].tribe].leader_name = -leaders[civs[civ].tribe].leader_name;
        }
        pw_ok = civ_check_password(human_civ[0]);
        if (pw_ok == 0) {
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ2 & 0x1f));
        }
        else {
          dlg.load_game_section((const char *)&str_gender_7,1);
          dialog_prepend_title_label(&dlg,player_idx + 0x25f);
          dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female)
          ;
          dlg.set_position(0xfffffc19,
                     -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) +
                      1));
          civ = dlg.show_modal(0);
          /* BF-113 */
          if (1 < civ) {
            civ = -1;
          }
          snd_play_effect(SND_MENUOK,0,0,0);
          if (-1 < civ) {
            is_female = (u8)civ;
            leaders[civs[game.player_civ].tribe].female = is_female;
            civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
            if (civ != 0) {
              civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
            }
            for (govt = 0; govt < 7; govt++) {
              for (sex = 0; sex < 2; sex++) {
                leaders[civs[game.player_civ].tribe].titles[govt][sex] = (short)(&government_titles[govt].male)[sex];
              }
            }
            ret = 0;
            return ret;
          }
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ2 & 0x1f));
        }
      }
      else {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ2 & 0x1f));
      }
    }
  } while( true );
}

/* 00559c20 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl setup_picwnd_global_init(void)

{
  setup_picwnd_static_init();
  setup_picwnd_reg_atexit();
  return;
}

/* 00559c3a auto */
void __cdecl setup_picwnd_static_init(void)

{
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->construct();
  return;
}

/* 00559c54 auto */
void __cdecl setup_picwnd_reg_atexit(void)

{
  _atexit(setup_picwnd_static_dtor);
  return;
}

/* 00559c71 auto */
void __cdecl setup_picwnd_static_dtor(void)

{
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->destruct();
  return;
}

/* 00559c8b auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl setup_picwnd_surface_global_init(void)

{
  setup_picwnd_surface_static_init();
  setup_picwnd_surface_reg_atexit();
  return;
}

/* 00559ca5 auto */
void __cdecl setup_picwnd_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&setup_picwnd_state.surface));
  return;
}

/* 00559cbf auto */
void __cdecl setup_picwnd_surface_reg_atexit(void)

{
  _atexit(setup_picwnd_surface_static_dtor);
  return;
}

/* 00559cdc auto */
void __cdecl setup_picwnd_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&setup_picwnd_state.surface));
  return;
}

/* 00559cf6 auto */
void __cdecl setup_picwnd_load_image(i32 width,i32 height)

{
  DibSurface surface;
  GamePalette palette;
  i8 *seh_scope;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  win_flush_pending_msgs();
  surface.load_gif_resource(setup_picwnd_state.image_id + 900,10,0xc0,&palette);
  win_flush_pending_msgs();
  ((DibSurface *)((i8 *)&setup_picwnd_state.surface))->DibSurface::create(width,height);
  gfx_copy_rect_same_pos(&surface,(DibSurface *)&setup_picwnd_state.surface,0,0,width,height);
  surface.release();
  return;
}

/* 00559ded auto */
void SetupPicWindow::reset_fields()

{
  max_rate_value = 100;
  rates_civ = max_rate_value;
  tax_rate_sel = 1;
  lux_rate_sel = 0;
  return;
}

/* 00559e3c auto */
SetupPicWindow *SetupPicWindow::construct()

{
  i8 *seh_scope;
  
  FrameWindow::construct();
  dll.init();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 8);
  reset_fields();
  return this;
}

/* 00559ed4 auto */
/* Library Function - Single Match
    public: virtual __thiscall CMiniFrameWnd::~CMiniFrameWnd(void)
   
   Library: Visual Studio 1998 Debug */

void SetupPicWindow::destruct()

{
  i8 *seh_scope;
  int seh_state;
  
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 8);
  (*((char *)&seh_state + 0)) = 1;
  seh_state = seh_state & 0xff;
  reset_fields();
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 00559f30 */ { dll.free(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00559f3f */ { (((FrameWindow *)this))->FrameWindow::destruct(); }
  return;
}

/* 00559f60 auto */
void SetupPicWindow::blit_image_rect(int *rect)

{
  i32 height;
  i32 width;
  
  height = rect_height((RECT *)rect);
  width = rect_width((RECT *)rect);
  gfx_copy_rect_xywh((DibSurface *)&setup_picwnd_state.surface,this,*rect - client_x,rect[1] - client_y,
                     *rect,rect[1],width,height);
  return;
}

/* 00559fcf auto */
void SetupPicWindow::draw_image(RECT *rect)

{
  gfx_set_draw_target(this);
  text_set_surface(this);
  blit_image_rect((int *)rect);
  return;
}

/* 0055a00b auto */
void SetupPicWindow::setup_picwnd_client_rect(RECT *rect,int x,int y,int width,int height)

{
  rect_set_xywh(rect,client_x + x,client_y + y,width,height);
  return;
}

/* 0055a051 auto */
void SetupPicWindow::redraw()

{
  layout();
  setup_picwnd_load_image(rates_civ,max_rate_value);
  setup_picwnd_client_rect((RECT *)&sci_rate_sel,0,0,rates_civ,max_rate_value);
  gfx_set_draw_target(this);
  draw_caption();
  draw_image((RECT *)&sci_rate_sel);
  wnd_blit_surface();
  return;
}

/* 0055a0e1 auto */
i32 __cdecl setup_picwnd_on_close(void)

{
  ((WndBase *)((i8 *)&setup_picwnd_state.wnd))->hide();
  main_frame_wnd_block.main_frame_wnd.focus_and_raise();
  ui_flush_input();
  return 0;
}

/* 0055a10d auto */
void __cdecl
setup_picwnd_frame_rect(RECT *rect,i32 x,i32 y,int width,int height)

{
  rect_set_xywh(rect,x,y,width + 0x10 + ui_metrics.dlgframe_w2,
                     height + 0x10 + ui_metrics.dlgframe_h2);
  return;
}

/* 0055a15e auto */
void __cdecl setup_picwnd_on_redraw(void)

{
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->redraw();
  return;
}

/* 0055a178 auto */
void __cdecl setup_picwnd_on_restore(void)

{
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->redraw();
  return;
}

/* 0055a192 auto */
int __cdecl setup_picwnd_position_coord(int position,int size)

{
  int client_size;
  
  switch(position) {
  case 0:
  case 5:
    position = 0;
    break;
  case 1:
    position = ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1;
    break;
  case 2:
    client_size = main_frame_wnd_block.main_frame_wnd.client_width();
    position = (client_size - (size + 0x10)) / 2;
    break;
  case 3:
    client_size = main_frame_wnd_block.main_frame_wnd.client_width();
    position = ((client_size - (size + 0x10)) -
              ((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3)) - 1;
    break;
  case 4:
    client_size = main_frame_wnd_block.main_frame_wnd.client_width();
    position = client_size - (size + 0x10);
    if (position < 0) {
      position = 0;
    }
    break;
  case 6:
    position = ((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1;
    break;
  case 7:
    client_size = main_frame_wnd_block.main_frame_wnd.client_height();
    position = (client_size - (size + 0x10)) / 2;
    break;
  case 8:
    client_size = main_frame_wnd_block.main_frame_wnd.client_height();
    position = ((client_size - (size + 0x10)) -
              ((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3)) - 1;
    break;
  case 9:
    client_size = main_frame_wnd_block.main_frame_wnd.client_height();
    position = client_size - (size + 0x10);
    if (position < 0) {
      position = 0;
    }
  }
  return position;
}

/* 0055a329 auto */
i32 __cdecl setup_picwnd_image_size(int is_height,i32 image_id)

{
  i32 size;
  
  switch(image_id) {
  case 1:
    if (is_height == 0) {
      size = 0x178;
    }
    else {
      size = 0xe3;
    }
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    if (is_height == 0) {
      size = 0x247;
    }
    else {
      size = 0x101;
    }
    break;
  case 6:
  case 7:
  case 8:
  case 9:
    if (is_height == 0) {
      size = 0x248;
    }
    else {
      size = 0x102;
    }
    break;
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
    if (is_height == 0) {
      size = 0x196;
    }
    else {
      size = 0x102;
    }
    break;
  default:
    size = 100;
  }
  return size;
}

/* 0055a41d auto */
void SetupPicWindow::show(i32 x_pos,i32 y_pos,u32 image_id)

{
  u32 size;
  i32 x;
  i32 y;
  int height;
  int width;
  i32 style;
  RECT rect;
  
  if (lux_rate_sel != 0) {
    tax_rate_sel = image_id;
    size = setup_picwnd_image_size(0,tax_rate_sel);
    rates_civ = size;
    size = setup_picwnd_image_size(1,tax_rate_sel);
    max_rate_value = size;
    x = setup_picwnd_position_coord(x_pos,rates_civ);
    y = setup_picwnd_position_coord(y_pos,max_rate_value);
    setup_picwnd_frame_rect(&rect,x,y,rates_civ,max_rate_value);
    style = 8;
    y = 8;
    x = 0;
    height = rect_height(&rect);
    height -= ui_metrics.dlgframe_h2;
    width = rect_width(&rect);
    create(0,2,rect.left,rect.top,width - ui_metrics.dlgframe_w2,height,x,y,
               style);
    window.wnd_handlers.set_close_handler(setup_picwnd_on_close);
    set_redraw_callback(setup_picwnd_on_redraw);
    window.wnd_handlers.set_restore_handler(setup_picwnd_on_restore);
    repaint();
    SurfaceWindow::show();
  }
  return;
}

/* 0055a567 auto */
void SetupPicWindow::close()

{
  if (lux_rate_sel != 0) {
    wnd_destroy_with_surface();
    ((DibSurface *)((i8 *)&setup_picwnd_state.surface))->release();
    ui_flush_input();
  }
  return;
}

/* 0055a5e4 auto */
void SetupPicWindow::load_dll()

{
  int found;
  i8 dll_path[128];
  
  crt_strcpy(dll_path,str_civ2_intro_dll);
  found = file_resolve_path((const char *)dll_path);
  if (found != 0) {
    dll.load((const char *)dll_path);
    lux_rate_sel = 1;
  }
  return;
}

/* 0055a64a auto */
void SetupPicWindow::free_dll()

{
  if (lux_rate_sel != 0) {
    dll.dll_handle_free();
    lux_rate_sel = 0;
  }
  return;
}

/* 005ae580 auto */
i32 __cdecl setup_pbem_game(void)

{
  int choice;
  i32 result;
  int i;
  Dialog dialog2;
  i32 map_mode;
  int num_players;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  for (i = 0; i < 8; i++) {
    civ_view.intro_shown[i] = 0;
    (((i8 *)&civ_saved_cursor_x_block.civ_email_addresses))[i * 0x20] = 0;
  }
  civ_saved_cursor_x_block.mp_turn_time_limit_ms = 0;
  game.multiplayer = 2;
  for (;;) {
    setup_dlg.dialog_pos_state = 3;
    setup_show_side_picture(0xffffffff,0);
    dialog2.load_game_section((const char *)&str_pbem1,1);
    dialog2.set_default_choice((int)menu_prefs.pbem_menu_choice);
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
    choice = -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1);
    result = setup_next_dialog_x(0);
    dialog2.set_position(result,choice);
    choice = dialog2.show_modal(0);
    snd_play_effect(SND_MENUOK,0,0,0);
    ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    if (choice < 0) {
      result = 1;
      return result;
    }
    if (4 < choice) {  /* BF-079 (Peter Johnson's ledger): a choice past the five
                          the game knows (a row added to GAME.TXT's @PBEM1) is
                          asked again (the original went on with no map set) */
      continue;
    }
    menu_prefs.pbem_menu_choice = (short)choice;
    if ((-1 < choice) && (choice < 3)) {
      __chdir(((i8 *)&game_dirs.home));
      rules_load(1);
    }
    scenario_events_loaded = 0;
    switch(choice) {
    case 0:
      choice = setup_choose_map_size();
      if (choice != 0) continue;
      map_mode = 0;
      choice = _rand();
      mapgen.land_mass = choice % 3 - 1;
      mapgen.land_form = 0;
      choice = _rand();
      if (choice % 3 == 0) {
        mapgen.land_form = mapgen.land_mass;
      }
      choice = _rand();
      mapgen.temperature = choice % 3 - 1;
      choice = _rand();
      mapgen.climate = choice % 3 - 1;
      choice = _rand();
      mapgen.age = choice % 3 - 1;
      choice = setup_choose_game_options(map_mode,2);
      if (choice != 0) continue;
      break;
    case 1:
      choice = map_load_dialog();
      if (choice != 0) continue;
      map_mode = 1;
      if (map.flat != 0) {
        map_mode = 2;
      }
      choice = _rand();
      mapgen.land_mass = choice % 3 - 1;
      mapgen.land_form = 0;
      choice = _rand();
      if (choice % 3 == 0) {
        mapgen.land_form = mapgen.land_mass;
      }
      choice = _rand();
      mapgen.temperature = choice % 3 - 1;
      choice = _rand();
      mapgen.climate = choice % 3 - 1;
      choice = _rand();
      mapgen.age = choice % 3 - 1;
      choice = setup_choose_game_options(map_mode,2);
      if (choice != 0) continue;
      break;
    case 2:
      choice = setup_choose_map_size();
      if (choice != 0) continue;
      map_mode = 0;
      choice = setup_custom_world();
      if (choice != 0) continue;
      choice = setup_choose_game_options(map_mode,2);
      if (choice != 0) continue;
      break;
    case 3:
      game_palette_block.event_dont_play_wonders = 0;
      choice = scenario_start_dialog(2);
      if (choice == 0) {
        if (game_palette_block.event_dont_play_wonders != 0) {
          game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
        }
        game.multiplayer = 2;
        num_players = setup_choose_hotseat_players();
        if (num_players == 0) {
          crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
        }
        else {
          choice = scenario_pick_human_civs(num_players);
          if (choice == 0) {
            prefs_save_civ2dat();
            result = 0;
            return result;
          }
          crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
        }
      }
      else {
        crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
      }
      continue;
    case 4:
      game_palette_block.event_dont_play_wonders = 0;
      choice = game_load_dialog(0,2,1);
      if (choice == 0) {
        if (game.multiplayer == 2) {
          prefs_save_civ2dat();
          result = 0;
          return result;
        }
        popup_game_with_sprite(str_emailnot,(Sprite *)&popup_info_icon_sprite,0);
      }
      if (game_palette_block.event_dont_play_wonders != 0) {
        game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
      }
      continue;
    default:
      break;
    }
    for (;;) {  /* the number of players, again when the first player backs out */
      game.civs_alive = '\0';
      game.human_players = '\0';
      num_players = setup_choose_hotseat_players();
      if (num_players == 0) break;
      leaders_reset_custom_names();
      for (i = 0; i < num_players; i++) {
        while (choice = setup_choose_tribe(i,2,0), choice != 0) {
          i--;
          if (i < 0) break;
        }
        if (i < 0) break;
      }
      if (i < 0) continue;
      setup_choose_opponents(num_players);
      prefs_save_civ2dat();
      rules_load_advances(1);
      rules_load_units(1);
      rules_load_improvements(1);
      snd_menu_end();
      map_generate(map_mode);
      game_init_new();
      if (setup_dlg.accelerated_start != 0) {
        game_accelerated_start(setup_dlg.accelerated_start - 1);
      }
      game.multiplayer = 2;
      if (turn_loop.resume_civ != '\0') {
        game.player_civ = turn_loop.resume_civ;
        human_civ[0] = (iptr)turn_loop.resume_civ;
      }
      result = 0;
      return result;
    }
  }
}
