/* The game, the game: start-up, turns, saving, scenarios, events, cheats: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00411bd7 auto */
void __cdecl cheat_throneroom_upgrade(void)

{
  throneroom_show_upgrade(human_civ[0]);
  return;
}

/* 0041325d auto */
void __cdecl eot_blink_timer_tick(void)

{
  if ((blink.unit_active != '\0') && (turn_loop.active != '\0')) {
    blink.end_turn_phase = blink.end_turn_phase == '\0';
    status_draw_end_of_turn(1);
  }
  return;
}

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
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
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
            ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
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
          popup_game_with_sprite((const char *)&str_hotseatnot,(iptr)&popup_info_icon_sprite,0);
          if (game_palette_block.event_dont_play_wonders != 0) {
            game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
          }
          continue;
        }
        text_clear();
        text_append_label(0x363);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(1,0x1f2,(const char *)&textbuf.buf,0);
        prefs_save_civ2dat();
        ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
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

/* 0041b48f auto */
void __cdecl app_fatal_cleanup(void)

{
  if (game_session_active != 0) {
    game_session_end();
  }
  app_shutdown_cleanup();
  turn_flags.app_shutdown_done = 1;
  return;
}

/* 0041b4c0 auto */
i32 __cdecl app_startup_init(void)

{
  i32 result;
  int rc;
  UINT profile_val;
  int i;
  
  crt_strcpy(((i8 *)&main_frame_wnd_block.game_txt_path),((i8 *)&startup_game_txt_name_str));
  crt_strcpy(((i8 *)&main_frame_wnd_block.rules_txt_path),str_rules);
  __getcwd(((i8 *)&game_dirs.home),0x104);
  crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  for (i = 0; i < 8; i++) {
    map_views_block.map_view_recs[i].open = 0;
    (((i32 *)&map_views_block.main_view_timer_shown))[i * 0xfc] = 0;
  }
  timeBeginPeriod(4);
  result = timer_ticks();
  crt_srand(result);
  result = timer_ticks();
  rng_set_seed(result);
  dialog_set_colors(0x10,0x21,0x25,0x1a,0x1d,0x25,0x12,0x1a,10);
  ui_set_radio_text_color(0x10);
  ui_set_radio_text_shadow(0x21,1,1);
  ui_set_radio_disabled_color(0x1a);
  ui_set_ctrl_text_color(0x10);
  ui_set_ctrl_text_shadow(0x21,1,1);
  dialog_set_list_colors(0x23,10,10,0x17,0x29,10,0x12);
  gfx_load_game_sprites();
  registry_load_default_language();
  game_language = registry_language_id - 1;
  if (2 < game_language) {
    game_language = 0;
  }
  crt_strcpy(((i8 *)&lang_file_ext),((i8 *)&startup_lang_default_ext_str));
  switch(registry_language_id) {
  case 1:
    crt_strcpy(((i8 *)&lang_file_ext),((i8 *)&startup_lang_english_ext_str));
    break;
  case 2:
    crt_strcpy(((i8 *)&lang_file_ext),((i8 *)&startup_lang_french_ext_str));
    break;
  case 3:
    crt_strcpy(((i8 *)&lang_file_ext),((i8 *)&startup_lang_german_ext_str));
    break;
  case 4:
    crt_strcpy(((i8 *)&lang_file_ext),((i8 *)&startup_lang_italian_ext_str));
    break;
  case 5:
    crt_strcpy(((i8 *)&lang_file_ext),((i8 *)&startup_lang_spanish_ext_str));
  }
  text_section_index_build((const char *)&main_frame_wnd_block.game_txt_path,0x4000);
  rc = text_open_section((const char *)str_ptr_labels,(const char *)&str_stringheap);
  if (rc == 0) {
    text_read_line(1);
    rc = text_next_int();
    if (strpool_capacity < rc) {
      strpool_capacity = rc;
    }
  }
  strpool_init(strpool_capacity);
  strpool_add((const char *)&str_nil);
  labels_load();
  dialog_load_button_labels();
  prefs_load_civ2dat();
  game.game_options = prefs.game_options;
  profile_val = GetPrivateProfileIntA
                    (str_civilization_gold,str_herald_warning_shown,0,
                     str_civ_ini);
  if (profile_val == 0) {
    rc = popup_game((const char *)&str_heraldwarning);
    if (rc == 0) {
      prefs.game_options = prefs.game_options & 0xffdfffff;
    }
    else {
      prefs.game_options = prefs.game_options | 0x200000;
    }
    game.game_options = prefs.game_options;
    prefs_save_civ2dat();
    WritePrivateProfileStringA
              (str_civilization_gold_2,str_herald_warning_shown_2,((i8 *)&ini_herald_warning_value_str),
               str_civ_ini_2);
  }
  rc = main_frame_create();
  if (rc == 0) {
    result = 1;
  }
  else {
    rc = main_frame_wnd_block.main_frame_wnd.client_width();
    if (1000 < main_frame_bg_surface_block.main_frame_nonclient_width + rc) {
      ((GameFont *)dlg_style.default_text_font)->set(0,0x18);
      ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 4)))->set(0,0x14);
      ((GameFont *)(*(i8 * *)dlg_style.default_text_font_rest))->set(0,0x18);
      dlg_set_scale(3,2);
    }
    ui_init_screen_metrics();
    err_register_cleanup_fn(app_fatal_cleanup);
    rules_parse_sections(1);
    frame_texture_load();
    city_load_citypref();
    passwords_reset();
    autoplay.active = 0;
    saved_turn.civ = 0;
    saved_turn.resume_new_game = 0;
    result = 0;
  }
  return result;
}

/* 0041b8b0 auto */
void __cdecl app_shutdown_cleanup(void)

{
  ui_flush_input();
  snd_shutdown();
  ui_flush_input();
  frame_texture_free();
  ui_flush_input();
  text_section_index_free();
  labels_free();
  strpool_free();
  ui_flush_input();
  main_frame_destroy();
  ui_flush_input();
  timeEndPeriod(4);
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
      rc = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_customsize,5,(iptr)&str_50,0,0,0,1)
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
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
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
        dlg.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),0
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
          choice = dlg_game_text_input((const char *)&str_name_2,0x17,(char *)&civ_names[game.player_civ].leader_name,(iptr)name_buf
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
  ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
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
    dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
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
        text_append_int((int)civs[civ].num_cities);
        text_append_space();
        text_append_label(0xc5);
        text_append((const char *)&str_empty_12);
        text_append_int((int)civs[civ].num_military_units);
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
      dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
      dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      choice = dlg.show_modal(0);
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
        name2 = civ_leader_name((int)game.player_civ);
        choice = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_3,0x17,(iptr)name2,arg4,arg5,arg6,arg7);
        if (choice == 0) {
          dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
          choice = dlg.run_modal((iptr)name_buf,0);
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

/* 0041eeeb auto */
int __cdecl game_main_menu_loop(void)

{
  int rc;
  int civ;
  int choice;
  int xd_major;
  GamePalette palette;
  int result;
  Dialog dlg;
  i32 map_source;
  int xd_minor;
  int retry;
  int xd_build;
  int lobby_failed;
  
  result = 1;
  retry = 1;
  lobby_failed = 0;
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __chdir(((i8 *)&game_dirs.home));
  crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  prefs_load_civ2dat();
  game.game_options = prefs.game_options;
  game.multiplayer = '\0';
  ui_flush_input();
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->load_dll();
  snd_wave_set_buffer_count(4);
  if (intro.theme_started == 0) {
    snd_play_effect(SND_MENULOOP,0,1,0);
  }
  for (;;) {
    crt_strcpy(((i8 *)&main_frame_wnd_block.game_txt_path),((i8 *)&menu_game_txt_name_str));
    crt_strcpy(((i8 *)&main_frame_wnd_block.rules_txt_path),str_rules_4);
    prefs_load_civ2dat();
    setup_clear_start_positions();
    ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
    for (civ = 0; civ < 8; civ++) {
      civ_refresh_ai_goals(civ);
      civ_reset_standing_goals(civ);
    }
    game_over_reason = 0;
    if (net.lobby_launch_mode == -1) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->show(1,6,1);
      dlg.load_game_section((const char *)&str_mainmenu,1);
      dlg.set_default_choice((int)menu_prefs.main_menu_choice[0]);
      dlg.set_position(
                 -(((int)(caption_font_block.screen_width - 0x280 + (caption_font_block.screen_width - 0x280 >> 0x1f & 7U)) >> 3) + 1),
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      choice = dlg.show_modal(0);
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      if (choice < 0) break;
      menu_prefs.main_menu_choice[0] = (short)choice;
      prefs_save_civ2dat();
    }
    else {
      choice = 5;
    }
    snd_play_effect(SND_MENUOK,0,0,0);
    scenario_events_loaded = 0;
    net.clear();
    switch(choice) {
    case 0:
    case 1:
    case 2:
      __chdir(((i8 *)&game_dirs.home));
      rules_load(1);
      if (choice == 1) {
        rc = map_load_dialog();
        if (rc != 0) {
          choice = -1;
        }
        map_source = 1;
        if (map.flat != 0) {
          map_source = 2;
        }
      }
      else {
        rc = setup_choose_map_size();
        if (rc != 0) {
          choice = -1;
        }
        map_source = 0;
      }
      if (choice == 2) {
        rc = setup_custom_world();
        if (rc != 0) {
          choice = -1;
        }
      }
      else {
        rc = _rand();
        mapgen.land_mass = rc % 3 - 1;
        mapgen.land_form = 0;
        rc = _rand();
        if (rc % 3 == 0) {
          mapgen.land_form = mapgen.land_mass;
        }
        rc = _rand();
        mapgen.temperature = rc % 3 - 1;
        rc = _rand();
        mapgen.climate = rc % 3 - 1;
        rc = _rand();
        mapgen.age = rc % 3 - 1;
      }
      if (-1 < choice) {
        retry = setup_new_game_dialogs(map_source);
      }
      break;
    case 3:
      game_palette_block.event_dont_play_wonders = 0;
      retry = setup_start_scenario();
      if (retry == 0) {
        if (game_palette_block.event_dont_play_wonders != 0) {
          game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
        }
      }
      else {
        crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
      }
      break;
    case 4:
      game_palette_block.event_dont_play_wonders = 0;
      retry = game_load_dialog(0,0,1);
      snd_play_effect(SND_MENUOK,0,0,0);
      if ((retry == 0) && (game.multiplayer != '\0')) {
        retry = load_game_pick_civ(1);
      }
      if (retry == 0) {
        (*(byte *)&saved_turn.civ) = (byte)(*(i8 *)&human_civ);
        (*(byte *)&turn_loop.resume_civ) = (byte)(*(i8 *)&human_civ);
        game.multiplayer = '\0';
        game.human_players = (u8)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
      }
      else if (game_palette_block.event_dont_play_wonders != 0) {
        game.game_options = game.game_options & ~OPT_WONDER_MOVIES;
      }
      break;
    case 5:
      XD_GetXDaemonVersion(&xd_major,&xd_minor,&xd_build);
      if (((xdaemon_required.major == xd_major) && (xdaemon_required.minor == xd_minor)) && (xdaemon_required.build == xd_build))
      {
        __chdir(((i8 *)&game_dirs.home));
        rules_load(1);
        retry = setup_choose_multiplayer_type();
        if ((retry != 0) && (net.lobby_launch_mode != -1)) {
          lobby_failed = 1;  /* launched from a lobby: no main menu to go back to */
        }
      }
      else {
        str_clear((char *)&text_args.strings);
        str_append_int((iptr)&text_args.strings,xdaemon_required.major);
        crt_strcat(((i8 *)&text_args.strings),((i8 *)&xdaemon_req_ver_dot1_str));
        str_append_int((iptr)&text_args.strings,xdaemon_required.minor);
        crt_strcat(((i8 *)&text_args.strings),((i8 *)&xdaemon_req_ver_dot2_str));
        str_append_int((iptr)&text_args.strings,xdaemon_required.build);
        str_clear((char *)&text_args.string1);
        str_append_int((iptr)&text_args.string1,xd_major);
        crt_strcat(((i8 *)&text_args.string1),((i8 *)&xdaemon_got_ver_dot1_str));
        str_append_int((iptr)&text_args.string1,xd_minor);
        crt_strcat(((i8 *)&text_args.string1),((i8 *)&xdaemon_got_ver_dot2_str));
        str_append_int((iptr)&text_args.string1,xd_build);
        popup_game_with_sprite((const char *)&str_wrongxdaemonversion,(iptr)&popup_info_icon_sprite,0);
      }
      break;
    case 6:
      hall_of_fame_show(0xffffffff);
      break;
    case 7:
      rc = popup_game_flags((const char *)&str_newcredits,1);
      if (rc < 0) {
        text_close_file();
      }
      else {
        snd_play_effect(-0x6b,0,0,0);
        snd_play_effect(-0x6c,0,0,0);
        snd_cd_detect_layout();
        credits_show(rc);
        snd_cd_stop();
        ui_flush_input();
        snd_play_effect(SND_MENULOOP,0,1,0);
      }
    }
    if (lobby_failed != 0) break;
    if (retry == 0) {
      result = 0;
      break;
    }
  }
  snd_play_effect(-0x6b,0,0,0);
  snd_play_effect(-0x6c,0,0,0);
  snd_wave_set_buffer_count(8);
  if ((result == 0) && (snd_cd_detect_layout(), (game.game_options & OPT_MUSIC) != 0)) {
    snd_sync_music_option();
  }
  ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->free_dll();
  rc = result;
  return rc;
}

/* 0041f69f auto */
void __cdecl game_session_begin(void)

{
  Tile *tile;
  uint y2;
  uint x2;
  int i;
  
  game_session_active = 1;
  if ((((turn_loop.resume_civ == '\0') && (game.turn == 0)) && (game.multiplayer != '\0')) &&
     ((mp_options.show_human_starts != 0 || (game.difficulty == DIFF_CHIEFTAIN)))) {
    y2 = 0;
    x2 = 0;
    net_batch_begin();
    for (i = 0; i < map.area; i++) {
      tile = tile_at(x2,y2);
      if ((game.human_players & tile->visible) != 0) {
        tile_set_visible(x2,y2,(uint)game.human_players,1,1);
      }
      x2 += 2;
      if ((int)map.width <= (int)x2) {
        y2++;
        x2 = y2 & 1;
      }
    }
    net_batch_flush();
  }
  if ((game.multiplayer != '\0') && ((mp_options.show_human_starts != 0 || (game.difficulty == DIFF_CHIEFTAIN)))) {
    for (x2 = 0; (int)x2 < 8; x2++) {
      if ((1 << ((byte)x2 & 0x1f) & (uint)game.human_players) != 0) {
        for (y2 = 0; (int)y2 < 8; y2++) {
          if ((1 << ((byte)y2 & 0x1f) & (uint)game.human_players) != 0) {
            diplo_set_treaty_flags(x2,y2,1);
          }
        }
      }
    }
  }
  if (2 < game.multiplayer) {
    human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
  }
  return;
}

/* 0041f878 auto */
void __cdecl game_session_end(void)

{
  ui_flush_input();
  if (game.multiplayer == 1) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->remove_item(0x1f2);
  }
  mapviews_destroy_all();
  ((CityWindow *)((i8 *)&city_wnd.window))->destroy();
  status_wnd_close();
  minimap_window_destroy();
  map_free();
  ui_flush_input();
  game_session_active = 0;
  return;
}

/* 0041f8d9 auto */
void __cdecl game_main(void)

{
  char is_host;
  iptr rc;
  NativeWindow *hwnd_info;
  int result;
  size_t log_len;
  LobbyLaunchInfo *launch_info;
  int i;
  FileObject log_file;
  int send_rc;
  iptr unit;
  int lobby_rc;
  iptr city;
  UINT ini_value;
  GamePalette palette;
  Dialog dlg;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  dbg_log((const char *)str_ptr_5_4_0f_multiplayer_26_march_99);
  dbg_log((const char *)(*(i8 * *)str_ptr_5_4_0f_multiplayer_26_march_99_rest));
  net.clear();
  rc = app_startup_init();
  if (rc == 0) {
    _memset(((i8 *)&net.lobby_launch_info),0,0xc4);
    launch_info = (LobbyLaunchInfo *)((i8 *)&net.lobby_launch_info);
    hwnd_info = main_frame_wnd_block.main_frame_wnd.get_window();
    lobby_rc = XD_LaunchedByLobby((void *)hwnd_info->hwnd,launch_info);
    if ((lobby_rc == 2) || (lobby_rc == 1)) {
      net.lobby_launch_mode = lobby_rc;
      dbg_log((const char *)&str_lobbylaunchinfo_gnetmgr_igzinfo);
      dbg_logf_str((const char *)&str_ipaddr_32_s,(const char *)&net.lobby_launch_info);
      dbg_logf_str((const char *)&str_longname_64_s,(const char *)&net.startup_log_text);
      dbg_logf_str((const char *)&str_shortname_32_s,(const char *)&net.lobby_short_name);
      dbg_logf_str((const char *)&str_sessionname_64_s,(const char *)&net.lobby_session_name);
      dbg_logf((const char *)&str_bhost_d,net.lobby_is_host);
    }
    else {
      if (((lobby_rc != -3) && (lobby_rc != -4)) && (lobby_rc != -5)) {
        dbg_log((const char *)&str_igz_sending_dplsys_dplayconnectfailed_to);
        send_rc = XD_LobbySendMessage(2);
        if (send_rc != 0) {
          dbg_logf((const char *)&str_igz_xd_lobbysendmessage_returned_error_d,send_rc);
        }
        return;
      }
      dbg_logf((const char *)&str_not_an_igz_game_event_d,lobby_rc);
      net.lobby_launch_mode = -1;
    }
    net.ini_simultaneous_allowed = 0;
    ini_value = GetPrivateProfileIntA
                          (str_civilization_gold_3,str_simultaneous,-1,
                           str_civ_ini_3);
    if (0 < (int)ini_value) {
      net.ini_simultaneous_allowed = 1;
      WritePrivateProfileStringA
                (str_civilization_gold_4,str_simultaneous_2,((i8 *)&ini_simultaneous_reset_str),
                 str_civ_ini_4);
    }
    (*(UINT *)&net.net_max_players) = 7;
    ini_value = GetPrivateProfileIntA
                          (str_civilization_gold_5,str_maxplayers,-1,str_civ_ini_5)
    ;
    if (4 < (int)ini_value) {
      if (7 < (int)ini_value) {
        ini_value = 7;
      }
      (*(UINT *)&net.net_max_players) = ini_value;
      WritePrivateProfileStringA
                (str_civilization_gold_6,str_maxplayers_2,((i8 *)&ini_max_players_reset_str),str_civ_ini_6
                );
    }
    crt_strcpy(((i8 *)&net.net_game_name),((i8 *)&menu_prefs.net_game_name));
    do {
      turn_loop.active = 1;
      net.loaded_file_kind = 0;
      net.loaded_file_name[0] = 0;
      net.loaded_file_subdir[0] = 0;
      for (unit = 0; unit < 8; unit++) {
        net.casualty_counts[unit] = 0;
        net.casualty_reported_counts[unit] = 0;
        net.casualty_log_first[unit] = 0;
        net.casualty_log_next[unit] = 0;
      }
      passwords_reset();
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
      _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots),-1,0xc0);
      for (unit = 0; unit < 0x800; unit++) {
        units[unit].id = 0;
        units[unit].prev_in_stack = -1;
        units[unit].next_in_stack = -1;
      }
      game.num_units = 0;
      for (city = 0; city < 0x100; city++) {
        cities[city].id = 0;
      }
      game.num_cities = 0;
      result = game_main_menu_loop();
      if (result != 0) {
        app_shutdown_cleanup();
        return;
      }
      minimap_window_create();
      status_wnd_open();
      mapviews_create_all();
      ((CityWindow *)((i8 *)&city_wnd.window))->create();
      if (2 < game.multiplayer) {
        chat_wnd_block.create(4);
        parley_wnd_block.create(3);
      }
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->create();
      ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
      main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
      main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
      if (game.multiplayer != '\0') {
        if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
          rules_apply_double_moves();
        }
        if ((2 < game.multiplayer) && (is_host = net.is_host(), is_host != '\0')) {
          net.mp_simultaneous_moves = *(i8 *)&mp_options.simultaneous_setting;
          net_send_message(NM_MULTIOPTIONS_CHANGE,0xff,(int)mp_doubles.production,(int)mp_doubles.moves,(int)(*(i8 *)&mp_options.simultaneous_setting),0,0,0,0
                             ,0);
        }
      }
      if (2 < game.multiplayer) {
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_LOAD_GAME,1);
        dlg_style.default_idle_callback = turn_popup_timeout_idle;
        is_host = net.is_host();
        if (is_host != '\0') {
          net_send_rules_tables(0xff);
        }
        net.tile_sync_enabled = 1;
        XD_FlushSendBuffer(180000);
        mp_mouse_watchdog_start();
      }
      game_session_begin();
      if ((((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && (2 < game.multiplayer)) &&
          (net.mp_simultaneous_moves != '\0')) && (simul_tutorial_shown == 0)) {
        popup_game_with_sprite((const char *)&str_simultut,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
        simul_tutorial_shown = 1;
      }
      ui_set_button_font((iptr)&dialog_plain_font_obj_block.dialog_plain_font_obj);
      civ_saved_cursor_x_block.game_setup_complete = 1;
      if (game.multiplayer < 3) {
        game_play_loop();
      }
      else {
        if (net.host_flag == '\0') {
          XD_FlushSendBuffer(5000);
          while (net.nm_ready_to_start_count == 0) {
            net.process_messages(1,0);
          }
        }
        else {
          net_send_message(NM_READY_TO_START,0xff,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
        }
        if (net.mp_simultaneous_moves == '\0') {
          if (net.host_flag == '\0') {
            result = game_mp_client_turn_loop();
            if (result != 0) {
              game_mp_host_turn_loop(1);
            }
          }
          else {
            game_mp_host_turn_loop(0);
          }
        }
        else if (net.host_flag == '\0') {
          result = net_client_turn_loop();
          if (result != 0) {
            net_host_turn_loop();
          }
        }
        else {
          net_host_turn_loop();
        }
      }
      prefs_save_civ2dat();
      dlg_style.default_idle_callback = 0;
      scenario_events_loaded = 0;
      mp_turn_timer_stop(1);
      mp_mouse_watchdog_stop();
      if (2 < game.multiplayer) {
        log_file.init();
        ON_SCOPE_EXIT(log_file.close());
        diffeng_free_mirror();
        if (net.host_flag != '\0') {
          net.shutdown(1);
        }
        result = log_file.create((const char *)&str_chatlog_txt);
        if (result != 0) {
          log_len = _strlen(chat.log_buffer);
          log_file.write((iptr)chat.log_buffer,log_len);
          log_file.map_close();
        }
        for (i = 0; i < 8; i++) {
          chat_wnd_block.send_to_civs[i] = 0;
          chat_wnd_block.muted_civs[i] = 0;
        }
        chat_wnd_block.remain_anonymous = 0;
        chat_wnd_block.broadcast_all = 0;
        chat_wnd_block.mute_anonymous = 0;
        parley_wnd_close();
        chat_wnd_close();
      }
      pedia_close();
      ((PictureWindow *)((i8 *)&advisor_state.window))->close();
      ((PictureWindow *)((i8 *)&casualty.window))->close();
      if (turn_flags.app_shutdown_done != 0) {
        return;
      }
      game_session_end();
    } while (net.lobby_launch_mode == -1);
  }
  else {
    app_shutdown_cleanup();
  }
  return;
}

/* 00421bb0 auto */
i32 __cdecl timer_ticks(void)

{
  i32 ticks;
  
  ticks = timer_ticks_60hz();
  return ticks;
}

/* 004259a6 auto */
i32 __cdecl game_profile_popup(int is_join)

{
  size_t name_len;
  i32 result;
  int minutes;
  uint slot_masks[10];
  int count;
  int i;
  int seconds;
  uint human_mask;
  
  slot_masks[0] = 0;
  slot_masks[1] = 1;
  slot_masks[2] = 3;
  slot_masks[3] = 7;
  slot_masks[4] = 0xf;
  slot_masks[5] = 0x1f;
  slot_masks[6] = 0x3f;
  slot_masks[7] = 0x7f;
  slot_masks[8] = 0xff;
  str_clear((char *)&text_args.strings);
  crt_strcat(((i8 *)&text_args.strings),((i8 *)&net.net_game_name));
  str_clear((char *)&text_args.string1);
  str_append_label((iptr)&text_args.string1,(mp_options.open_game == 0) + 0x277);
  str_clear((char *)&text_args.string2);
  crt_strcat(((i8 *)&text_args.string2),((i8 *)&net.host_player_name));
  str_clear((char *)&text_args.string3);
  name_len = _strlen(((i8 *)&scenario.name));
  if (name_len == 0) {
    str_append_label((iptr)&text_args.string3,0x34c);
  }
  else {
    crt_strcpy(((i8 *)&text_args.string3),((i8 *)&scenario.name));
  }
  str_append_space((iptr)&text_args.string3);
  str_append_lparen((iptr)&text_args.string3);
  result = game_turn_to_year((int)game.turn);
  str_append_year((iptr)&text_args.string3,result);
  str_append_rparen((iptr)&text_args.string3);
  str_clear((char *)&text_args.profile_difficulty);
  str_append_label((iptr)&text_args.profile_difficulty,game.difficulty + 0x279);
  str_append_space((iptr)&text_args.profile_difficulty);
  str_append_lparen((iptr)&text_args.profile_difficulty);
  str_append_label((iptr)&text_args.profile_difficulty,0x34e - (uint)(mp_options.simultaneous_setting == 0));
  str_append_rparen((iptr)&text_args.profile_difficulty);
  str_clear((char *)&text_args.profile_map_width);
  str_append_int((iptr)&text_args.profile_map_width,(int)map.width / 2);
  str_clear((char *)&text_args.profile_map_height);
  str_append_int((iptr)&text_args.profile_map_height,(int)map.height);
  str_clear((char *)&text_args.string5);
  str_append_label((iptr)&text_args.string5,0x350 - (uint)(mp_doubles.production == 0));
  crt_strcat(((i8 *)&text_args.string5),((i8 *)&game_profile_double_opts_sep_str));
  str_append_label((iptr)&text_args.string5,0x352 - (uint)(mp_doubles.moves == 0));
  str_clear((char *)&text_args.profile_barbarians);
  str_append_label((iptr)&text_args.profile_barbarians,game.barbarian_activity + 0x27f);
  str_clear((char *)&text_args.profile_timer);
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms == 0) {
    str_append_label((iptr)&text_args.profile_timer,0x285);
  }
  else {
    minutes = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) / 0x3c;
    seconds = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) % 0x3c;
    if (minutes < 10) {
      str_append_int((iptr)&text_args.profile_timer,0);
    }
    str_append_int((iptr)&text_args.profile_timer,minutes);
    crt_strcat(((i8 *)&text_args.profile_timer),((i8 *)&game_profile_timer_colon_str));
    if (seconds < 10) {
      str_append_int((iptr)&text_args.profile_timer,0);
    }
    str_append_int((iptr)&text_args.profile_timer,seconds);
  }
  str_clear((char *)&text_args.profile_count);
  count = 0;
  slot_masks[9] = (uint)game.civs_alive;
  for (i = 1; slot_masks[9] = (int)slot_masks[9] >> 1, i < 8; i++) {
    if ((slot_masks[9] & 1) != 0) {
      count++;
    }
  }
  str_append_int((iptr)&text_args.profile_count,count);
  str_clear((char *)&text_args.profile_count2);
  human_mask = 0;
  if (game.multiplayer == '\0') {
    human_mask = 1;
  }
  else if (game.multiplayer == 1) {
    human_mask = (uint)net.hotseat_human_players;
  }
  else {
    human_mask = slot_masks[net.net_player_count[0]];
  }
  count = 0;
  slot_masks[9] = human_mask;
  for (i = 0; i < 8; i++) {
    if ((slot_masks[9] & 1) != 0) {
      count++;
    }
    slot_masks[9] = (int)slot_masks[9] >> 1;
  }
  if (is_join != 0) {
    count--;
  }
  str_append_int((iptr)&text_args.profile_count2,count);
  dlg_style.pending_idle_callback = turn_popup_timeout_idle;
  result = popup_text_with_sprite((iptr)&str_game_7,(const char *)&str_gameprofile,(uint)(is_join != 0),(iptr)&popup_info_icon_sprite,0);
  return result;
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
    popup_game_with_sprite((const char *)&str_lanconnectfail,(iptr)&popup_info_icon_sprite,0);
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
    dlg->load_text_section((const char *)&str_game_7,(const char *)&str_ipverify,0x1f,(iptr)&menu_prefs.net_ip_address,0,0,0,1);
    rc = dlg->run_modal((iptr)ip_buf,0);
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
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_tempstr,(iptr)&str_d_ss_franklinton_startup_lan_cpp_3,0x1b9);
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
        popup_game_with_sprite((const char *)&str_lanconnectfail_2,(iptr)&popup_info_icon_sprite,0);
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
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)palette);
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
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_gnetmgr_pdescription,(iptr)&str_d_ss_franklinton_startup_lan_cpp_2,0x79);
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
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_whichnetwork_internet_whichnetwork_local,(iptr)&str_d_ss_franklinton_startup_lan_cpp,0x21);
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
        popup_game_with_sprite((const char *)&str_lannot,(iptr)&popup_info_icon_sprite,0);
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
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    rc = game_load_dialog(1,3,1);
    if (rc != 0) {
      snd_play_effect(SND_MENUOK,0,0,0);
      ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
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
        dlg.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),0
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

/* 00448de6 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl tutorial_icon_sprite_global_init(void)

{
  tutorial_icon_sprite_static_init();
  tutorial_icon_sprite_reg_atexit();
  return;
}

/* 00448e00 auto */
void __cdecl tutorial_icon_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&tutorial_icon_sprite_block.tutorial_icon_sprite));
  return;
}

/* 00448e1a auto */
void __cdecl tutorial_icon_sprite_reg_atexit(void)

{
  _atexit(tutorial_icon_sprite_static_dtor);
  return;
}

/* 00448e37 auto */
void __cdecl tutorial_icon_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&tutorial_icon_sprite_block.tutorial_icon_sprite));
  return;
}

/* 00453f90 auto */
void __cdecl registry_load_default_language(void)

{
  i8 value_type[4];
  char key_path[1024];
  int value_size;
  int *value_ptr;
  int language;
  
  language = 1;
  _sprintf(key_path,str_software_s_s,throneroom_piece4_rect_x_block.str_ptr_microprose_software,
           (*(i8 * *)throneroom_piece4_rect_x_block.str_ptr_microprose_software_rest));
  value_ptr = (int *)registry_read_value((iptr)key_path,(iptr)&str_defaultlanguage,(iptr)&value_size,(iptr)value_type);
  if ((value_ptr == (int *)0x0) || (value_size == 0)) {
    language = 0;
  }
  else {
    language = *value_ptr;
  }
  if (language == 0) {
    language = 1;
  }
  _sprintf(key_path,str_software_s_s_2,throneroom_piece4_rect_x_block.str_ptr_microprose_software,
           (*(i8 * *)throneroom_piece4_rect_x_block.str_ptr_microprose_software_rest));
  registry_write_value((iptr)key_path,(iptr)&str_defaultlanguage_2,(iptr)&language,4,4);
  registry_language_id = language;
  return;
}

/* 0045406c auto */
bool __cdecl registry_write_value(iptr key_path,iptr value_name,iptr data,DWORD data_size,DWORD value_type)

{
  LSTATUS status;
  bool ok;
  HKEY hkey;
  DWORD disposition;
  int rc;
  
  status = RegCreateKeyExA(0x80000002,(LPCSTR)key_path,0,((i8 *)&registry_key_class_str),0,0x20006,
                          (LPSECURITY_ATTRIBUTES)0x0,&hkey,&disposition);
  if (status == 0) {
    rc = RegSetValueExA(hkey,(LPCSTR)value_name,0,value_type,(BYTE *)data,data_size);
    RegCloseKey(hkey);
    ok = rc == 0;
  }
  else {
    RegCloseKey(hkey);
    ok = false;
  }
  return ok;
}

/* 00454103 auto */
i8 * __cdecl registry_read_value(iptr key_path,iptr value_name,iptr out_size,iptr out_type)

{
  LSTATUS status;
  i8 *result;
  DWORD data_size2;
  HKEY hkey;
  DWORD value_type;
  
  data_size2 = 0x800;
  _memset(((i8 *)&tax_rate_icon_sprites_block.registry_value_buffer),0,0x800);
  *(i32 *)out_size = 0;
  *(i32 *)out_type = 0;
  status = RegOpenKeyExA(0x80000002,(LPCSTR)key_path,0,0x20019,&hkey);
  if (status == 0) {
    status = RegQueryValueExA(hkey,(LPCSTR)value_name,(LPDWORD)0x0,&value_type,((LPBYTE)(i8 *)&tax_rate_icon_sprites_block.registry_value_buffer),&data_size2);
    if (status == 0) {
      *(DWORD *)out_type = value_type;
      *(DWORD *)out_size = data_size2;
      RegCloseKey(hkey);
      result = ((i8 *)&tax_rate_icon_sprites_block.registry_value_buffer);
    }
    else {
      RegCloseKey(hkey);
      result = (i8 *)0x0;
    }
  }
  else {
    RegCloseKey(hkey);
    result = (i8 *)0x0;
  }
  return result;
}

/* 004675b0 auto */
void __cdecl scenario_events_global_init(void)

{
  scenario_events_static_init();
  scenario_events_reg_atexit();
  return;
}

/* 004675ca auto */
void __cdecl scenario_events_static_init(void)

{
  ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->construct(50000);
  return;
}

/* 004675e9 auto */
void __cdecl scenario_events_reg_atexit(void)

{
  _atexit(scenario_events_static_dtor);
  return;
}

/* 00467606 auto */
void __cdecl scenario_events_static_dtor(void)

{
  ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->destruct();
  return;
}

/* 00467620 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl main_frame_wnd_global_init(void)

{
  main_frame_wnd_static_init();
  main_frame_wnd_reg_atexit();
  return;
}

/* 0046763a auto */
void __cdecl main_frame_wnd_static_init(void)

{
  main_frame_wnd_block.main_frame_wnd.ModalMenuWindow::construct();
  return;
}

/* 00467654 auto */
void __cdecl main_frame_wnd_reg_atexit(void)

{
  _atexit(main_frame_wnd_atexit_dtor);
  return;
}

/* 00467671 auto */
void __cdecl main_frame_wnd_atexit_dtor(void)

{
  main_frame_wnd_block.main_frame_wnd.ModalMenuWindow::destruct();
  return;
}

/* 0046da40 auto */
void __cdecl intro_play_opening_video(void)

{
  int found;
  ModalMenuWindow host_wnd;
  GamePalette palette;
  DllHandle dll;
  i8 path[128];
  AviPlayer player;  /* the video player and its window (at 0x48) */
  RECT screen_rect2;
  int open_rc;
  
  player.construct();
  ON_SCOPE_EXIT(player.destruct());
  host_wnd.ModalMenuWindow::construct();
  ON_SCOPE_EXIT(host_wnd.ModalMenuWindow::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  dll.init();
  ON_SCOPE_EXIT(dll.free());
  gfx_get_screen_rect((iptr)((i8 *)&screen_rect2.left));
  (*(i8 * *)&intro.video_player) = (i8 *)&player;
  str_clear((char *)path);
  str_append((char *)path,(const char *)&str_civ2_civ2art_dll);
  found = file_resolve_path((const char *)path);
  if (found != 0) {
    dll.load((const char *)path);
    str_clear((char *)path);
    str_append((char *)path,(const char *)&str_civ2_video_opening_avi);
    found = file_resolve_path((const char *)path);
    if (found != 0) {
      ((WndBase *)&host_wnd)->create_with_cmd_proc((const char *)&str_empty_82,0x800,0,0,screen_rect2.right,screen_rect2.bottom + 5,(iptr)&game_palette_block.game_palette);
      ((WndBase *)&host_wnd)->set_bg_color(0);
      (*(i8 * *)&intro.video_player) = (i8 *)&player;
      player.create((iptr)&str_empty_83,0x800,0,0);
      player.window.wnd_handlers.set_lbuttonup_handler((WndMouseFn)intro_video_on_click);
      player.window.wnd_handlers.set_keydown_handler(intro_video_on_keydown);
      player.set_end_callback(intro_video_on_end);
      player.set_double_size(1);
      open_rc = player.open((const char *)path);
      if (open_rc == 0) {
        ((DibSurface *)&player)->fill(0);
        ((WndBase *)&player.window)->center();
        ((WndBase *)&host_wnd)->handle_show_raise();
        wnd_flush_paint();
        ((SurfaceWindow *)&player)->SurfaceWindow::show();
        wnd_flush_paint();
        player.play();
        ((WndBase *)&player.window)->set_capture();
        if (2 < game.multiplayer) {
          ((ModalMenuWindow *)&player.window)->set_modal_idle_callback(net_modal_idle_poll);
        }
        ((ModalMenuWindow *)&player.window)->run_modal_loop();
        ((ModalMenuWindow *)&player.window)->set_modal_idle_callback(0);
        ((WndBase *)&player.window)->release_capture();
        player.pause();
        ((WndBase *)&player.window)->hide();
        wnd_flush_paint();
        ((WndBase *)&host_wnd)->hide();
        (*(i8 * *)&intro.video_player) = (i8 *)0x0;
        dll.dll_handle_free();
      }
      else {
        if (open_rc == -0x7ffbfeac) {
          popup_game((const char *)&str_vfwnotregistered);
        }
        (*(i8 * *)&intro.video_player) = (i8 *)0x0;
      }
    }
  }
  return;
}

/* 0046dde0 auto */
void __cdecl intro_video_on_end(void)

{
  if (intro.video_player != 0) {
    ((ModalMenuWindow *)(intro.video_player + 0x48))->end_modal_loop();
    intro.theme_started = 1;
    snd_play_effect(SND_MENULOOP,0,1,0);
  }
  return;
}

/* 0046de25 auto */
void __cdecl intro_video_on_keydown(int key)

{
  if ((0xcf < key) && (key < 0xd3)) {
    snd_play_effect(SND_MENULOOP,0,1,0);
    intro.theme_started = 1;
    if (intro.video_player != 0) {
      ((ModalMenuWindow *)(intro.video_player + 0x48))->end_modal_loop();
    }
  }
  return;
}

/* 0046dea1 auto */
void __cdecl intro_video_on_click(void)

{
  if (intro.video_player != 0) {
    ((ModalMenuWindow *)(intro.video_player + 0x48))->end_modal_loop();
  }
  return;
}

/* 00472f10 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl scenario_title_surface_global_init(void)

{
  scenario_title_surface_static_init();
  scenario_title_surface_reg_atexit();
  return;
}

/* 00472f2a auto */
void __cdecl scenario_title_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&scenario_title_surface_block.scenario_title_surface));
  return;
}

/* 00472f44 auto */
void __cdecl scenario_title_surface_reg_atexit(void)

{
  _atexit(scenario_title_surface_atexit_dtor);
  return;
}

/* 00472f61 auto */
void __cdecl scenario_title_surface_atexit_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&scenario_title_surface_block.scenario_title_surface));
  return;
}

/* 00472f7b auto */
void __cdecl save_pack_layout_rects(void)

{
  int i;
  
  grey_palette_block.save_layout_record[0] = (i16)destroyed_civ_count_block.saved_layout_valid;
  rect_pack_to_short((RECT *)&saved_screen_rect_block.saved_screen_rect,(ShortRect *)&grey_palette_block.str_empty_189);
  rect_pack_to_short((RECT *)&saved_screen_rect_block.layout_rect_main,(ShortRect *)&grey_palette_block.saved_layout_rects);
  for (i = 0; i < 8; i++) {
    rect_pack_to_short((RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + i * 0x10),(ShortRect *)(i * 8 + (iptr)(((u8 *)&grey_palette_block.saved_layout_rects + 8))));
  }
  rect_pack_to_short((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect,(ShortRect *)&grey_palette_block.str_empty_190);
  rect_pack_to_short((RECT *)&minimap_wnd_rect_block.status_wnd_rect,(ShortRect *)&grey_palette_block.str_empty_191);
  rect_pack_to_short((RECT *)&minimap_wnd_rect_block.city_wnd_rect,(ShortRect *)&grey_palette_block.saved_layout_rect2);
  return;
}

/* 00473064 auto */
void __cdecl save_unpack_layout_rects(void)

{
  int i;
  
  destroyed_civ_count_block.saved_layout_valid = (int)grey_palette_block.save_layout_record[0];
  rect_unpack_from_short((ShortRect *)&grey_palette_block.str_empty_189,(RECT *)&saved_screen_rect_block.saved_screen_rect);
  rect_unpack_from_short((ShortRect *)&grey_palette_block.saved_layout_rects,(RECT *)&saved_screen_rect_block.layout_rect_main);
  for (i = 0; i < 8; i++) {
    rect_unpack_from_short((ShortRect *)(i * 8 + (iptr)(((u8 *)&grey_palette_block.saved_layout_rects + 8))),(RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + i * 0x10));
  }
  rect_unpack_from_short((ShortRect *)&grey_palette_block.str_empty_190,(RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  rect_unpack_from_short((ShortRect *)&grey_palette_block.str_empty_191,(RECT *)&minimap_wnd_rect_block.status_wnd_rect);
  rect_unpack_from_short((ShortRect *)&grey_palette_block.saved_layout_rect2,(RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  return;
}

/* A NUL inside a saved text field of `size` bytes. */
static int save_text_terminated(const char *text,int size)
{
  return memchr(text,0,size) != (void *)0x0;
}

/* BF-093, BF-123..128, BF-193, BF-232, BF-233 (Peter Johnson's ledger): what
   indexes the game's tables in the units and cities a save brings, as the game
   writes them: an active unit's type, owner, stack links, home city (none or an
   active city), a caravan's commodity (0..15, none) and, with goto orders, its
   square on the map; an active city's owner, trade routes and their partners,
   commodities supplied and demanded (negative: in use), production and name.
   0 refuses the file as one that does not read. */
static int save_records_valid(void)
{
  int unit;
  int city;
  int i;
  Unit *u;
  City *c;

  for (unit = 0; unit < game.num_units; unit++) {
    u = units + unit;
    if (u->id == 0) continue;
    if ((0x3d < u->type) || ((uint)(int)u->owner > 7) ||
       (u->prev_in_stack < -1) || (0x7ff < u->prev_in_stack) ||
       (u->next_in_stack < -1) || (0x7ff < u->next_in_stack)) {
      return 0;
    }
    if ((byte)u->home_city != 0xff) {
      if ((game.num_cities <= (byte)u->home_city) || (cities[(byte)u->home_city].id == 0)) {
        return 0;
      }
    }
    if ((unit_types[u->type].role == ROLE_TRADE) && (0xf < u->counter) && (u->counter != 0xff)) {
      return 0;
    }
    if (((u->orders == ORDER_GOTO) || (u->orders == ORDER_AI_GOTO)) &&
       (map_in_bounds((int)u->x,(int)u->y) == 0)) {
      return 0;
    }
  }
  for (city = 0; city < game.num_cities; city++) {
    c = cities + city;
    if (c->id == 0) continue;
    if (((uint)(int)c->owner > 7) || (3 < c->trade_routes) ||
       (c->building < -0x42) || (0x3d < c->building) ||
       (save_text_terminated(c->name,sizeof c->name) == 0)) {
      return 0;
    }
    for (i = 0; i < 3; i++) {
      if (((char)c->supplies[i] < -0xf) || (0xf < (char)c->supplies[i]) ||
         ((char)c->demands[i] < -0xf) || (0xf < (char)c->demands[i])) {
        return 0;
      }
    }
    for (i = 0; i < c->trade_routes; i++) {
      if ((c->route_partner[i] < 0) || (0xff < c->route_partner[i]) ||
         (0xf < (char)c->route_goods[i])) {
        return 0;
      }
    }
  }
  return 1;
}

/* 004732a6 auto */
i32 __cdecl save_read_units_cities(int version,iptr file)

{
  size_t nread;
  iptr unit;
  
  /* BF-091: reject serialized counts before any record read or ID fixup.
   * Use this port's backing (2048 units, 256 cities), not another build's limits. */
  if (game.num_units < 0 || game.num_units > (int)(sizeof power_graph_history_block.units / sizeof(Unit)) ||
      game.num_cities < 0 || game.num_cities > (int)(sizeof cities / sizeof(City))) {
    return 0;
  }
  if (version < 0x2a) {
    if (version == 0x29) {
      for (unit = 0; unit < game.num_units; unit++) {
        nread = _fread(units + unit,0x1e,1,(FILE *)file);
        if (nread == 0) {
          return 0;
        }
      }
      if ((game.num_cities != 0) &&
         (nread = _fread(cities,game.num_cities * 0x58,1,(FILE *)file), nread == 0)) {
        return 0;
      }
      for (unit = 0; unit < game.num_units; unit++) {
        if (units[unit].id != 0) {
          units[unit].id = next_ids.unit;
          next_ids.unit++;
        }
      }
      for (unit = 0; unit < game.num_cities; unit++) {
        if (cities[unit].id != 0) {
          cities[unit].id = next_ids.city;
          next_ids.city++;
        }
      }
    }
    else {
      for (unit = 0; unit < game.num_units; unit++) {
        nread = _fread(units + unit,0x1a,1,(FILE *)file);
        if (nread == 0) {
          return 0;
        }
      }
      for (unit = 0; unit < game.num_cities; unit++) {
        nread = _fread(cities + unit,0x54,1,(FILE *)file);
        if (nread == 0) {
          return 0;
        }
      }
      for (unit = 0; unit < game.num_units; unit++) {
        units[unit].id = next_ids.unit;
        next_ids.unit++;
      }
      for (unit = 0; unit < game.num_cities; unit++) {
        cities[unit].id = next_ids.city;
        next_ids.city++;
      }
    }
  }
  else {
    if ((game.num_units != 0) &&
       (nread = _fread(units,(int)game.num_units << 5,1,(FILE *)file), nread == 0)) {
      return 0;
    }
    if ((game.num_cities != 0) &&
       (nread = _fread(cities,game.num_cities * 0x58,1,(FILE *)file), nread == 0)) {
      return 0;
    }
    for (unit = 0; unit < game.num_units; unit++) {
      if (units[unit].id != 0) {
        units[unit].id = next_ids.unit;
        next_ids.unit++;
      }
    }
    for (unit = 0; unit < game.num_cities; unit++) {
      if (cities[unit].id != 0) {
        cities[unit].id = next_ids.city;
        next_ids.city++;
      }
    }
  }
  return save_records_valid();
}

/* BF-134..136, BF-172, BF-176, BF-201, BF-230, BF-291 (Peter Johnson's
   ledger): what indexes the game's tables in the game record and the civs a
   save brings, as the game writes them: the difficulty, barbarian activity,
   the civs whose turn it is and who plays here (the local one in a game on
   this computer: hot seat and e-mail games), the power ranks, the AI
   opponents counted from the civs alive, and each civ's tribe, research,
   government and names (each with its NUL). 0 refuses the file. */
static int save_game_record_valid(void)
{
  int civ;
  int i;

  if ((5 < game.difficulty) || (3 < game.barbarian_activity) ||
     ((uint)(int)game.turn_civ > 7) || ((uint)(int)game.player_civ > 7) ||
     ((game.multiplayer < 3) && ((uint)(int)game.saved_current_civ > 7)) ||
     (7 < game.best_human_rank) || (7 < game.worst_human_rank) || (6 < game.num_enemies)) {
    return 0;
  }
  for (civ = 0; civ < 8; civ++) {
    if (((uint)(int)civs[civ].tribe > 0x14) || (civs[civ].researching < -1) ||
       (99 < civs[civ].researching) || (6 < civs[civ].government) ||
       (save_text_terminated(civ_names[civ].leader_name,0x18) == 0) ||
       (save_text_terminated(civ_names[civ].tribe_name,0x18) == 0) ||
       (save_text_terminated(civ_names[civ].adjective,0x18) == 0)) {
      return 0;
    }
    for (i = 0; i < 7; i++) {
      if (save_text_terminated(civ_names[civ].titles[i],0x18) == 0) {
        return 0;
      }
    }
  }
  return 1;
}

/* 00473660 auto */
i32 __cdecl save_load_gpk(int version,iptr file)

{
  size_t nread;
  int alive_count;
  uint enemy_count;
  iptr civ;
  int civ2;
  i8 pad[4];
  
  if (version < 0x28) {
    if ((version != 0x26) && (version != 0x27)) {
      return 0;
    }
    for (civ2 = 0; civ2 < 8; civ2++) {
      for (civ = 0; civ < 0x3e; civ++) {
        *(i8 *)&civs[civ2].units_active[civ] = 0;
        *(i8 *)&civs[civ2].units_lost[civ] = 0;
        *(i8 *)&civs[civ2].units_in_production[civ] = 0;
      }
    }
    for (civ = 0; civ < 100; civ++) {
      game.first_discoverer[civ] = '\0';
      game.advance_known_by[civ] = '\0';
      for (civ2 = 0; civ2 < 8; civ2++) {
        *(i8 *)&civs[civ2].advance_source[civ] = 0;
      }
    }
    for (civ2 = 0; civ2 < 8; civ2++) {
      for (civ = 0; civ < 0xd; civ++) {
        *(i8 *)&civs[civ2].advances[civ] = 0;
      }
    }
    nread = _fread(&game,0x36,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(game.first_discoverer,0x5d,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(game.advance_known_by,0x5d,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(game.wonder_city,0x4c,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
    alive_count = bits_count8((uint)game.civs_alive);
    enemy_count = alive_count - 1;
    if ((game.civs_alive & 1) != 0) {
      enemy_count = alive_count - 2;
    }
    if (game.num_enemies != enemy_count) {
      dbg_logf_int2((const char *)&str_load_gpk_fixing_up_game_enemies_d_incons_2,(uint)game.num_enemies,(uint)game.civs_alive);
      game.num_enemies = (u8)enemy_count;
    }
    nread = _fread(civ_names,0x790,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
    for (civ = 0; civ < 8; civ++) {
      nread = _fread(civs + civ,0x58,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].advances,0xc,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(&civs[civ].num_military_units,0xe,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].advance_source,0x5d,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].units_active,0x36,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].units_lost,0x36,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].units_in_production,0x36,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(pad,1,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].continent_military,0x402,1,(FILE *)file);
      if (nread == 0) {
        return 0;
      }
    }
  }
  else {
    nread = _fread(&game,0x14a,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
    alive_count = bits_count8((uint)game.civs_alive);
    enemy_count = alive_count - 1;
    if ((game.civs_alive & 1) != 0) {
      enemy_count = alive_count - 2;
    }
    if (game.num_enemies != enemy_count) {
      dbg_logf_int2((const char *)&str_load_gpk_fixing_up_game_enemies_d_incons,(uint)game.num_enemies,(uint)game.civs_alive);
      game.num_enemies = (u8)enemy_count;
    }
    nread = _fread(civ_names,0x790,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(civs,0x2ca0,1,(FILE *)file);
    if (nread == 0) {
      return 0;
    }
  }
  return save_game_record_valid();
}

/* 00473c12 auto */
i32 __cdecl save_write_str(char *str,iptr file)

{
  int rc;
  i32 result;
  
  rc = _fputs(str,(FILE *)file);
  if (rc == -1) {
    result = 0xffffffff;
  }
  else {
    rc = _fputc(0,(FILE *)file);
    if (rc == -1) {
      result = 0xffffffff;
    }
    else {
      result = 0;
    }
  }
  return result;
}

/* 00473c68 auto */
/* WARNING: Unable to use type for symbol next_len */

int __cdecl save_read_event_str(iptr file)

{
  char *cur;
  int saved_ch;
  size_t str_len;
  int next_len;
  iptr len;
  char buf[260];
  int ch;
  int result;
  
  len = 0;
  do {
    ch = _fgetc((FILE *)file);
    saved_ch = ch;
    if (ch == -1) {
      return 0;
    }
    if (0xfe < len) break;
    next_len = len + 1;
    buf[len] = (char)ch;
    ch = saved_ch;
    cur = buf + len;
    len = next_len;
  } while (*cur != '\0');
  buf[len] = '\0';
  str_len = _strlen(buf);
  result = memblock_alloc((MemBlock *)&unit_types_block.str_empty_188,str_len + 1);
  if (result == 0) {
    error_report(0xfffffff7,3,0,0,0);
    result = 0;
  }
  else {
    crt_strcpy((char *)((void *)result),buf);
  }
  return result;
}

/* 00473d5e auto */
void __cdecl save_set_file_ext(int is_scenario)

{
  if (is_scenario == 0) {
    if (game.multiplayer == '\0') {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_sav_str));
    }
    else if (game.multiplayer == 1) {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_hot_str));
    }
    else if (game.multiplayer == 2) {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_eml_str));
    }
    else if ((((game.multiplayer == 3) || (game.multiplayer == 4)) ||
             (game.multiplayer == 5)) || (game.multiplayer == 6)) {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_net_str));
    }
  }
  else {
    crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_scn_str));
  }
  return;
}

/* 00473e55 auto */
i8 * __cdecl save_file_filter(int save_only,int is_scenario,int is_mp)

{
  size_t len;
  char *cursor;
  
  if (is_scenario == 0) {
    if (is_mp == 0) {
      _memset(((i8 *)&grey_palette_block.save_file_filter_buf),0,0x104);
      cursor = save_filter_add_entry((uint)game.multiplayer,(char *)&grey_palette_block.save_file_filter_buf);
      if (save_only == 0) {
        if (game.multiplayer != '\0') {
          cursor = save_filter_add_entry(0,cursor);
        }
        if (game.multiplayer < 3) {
          cursor = save_filter_add_entry(3,cursor);
        }
        if (game.multiplayer != 1) {
          save_filter_add_entry(1,cursor);
        }
      }
    }
    else {
      _memset(((i8 *)&grey_palette_block.save_file_filter_buf),0,0x104);
      str_append_label((iptr)&grey_palette_block.save_file_filter_buf,0x105);
      crt_strcat(((i8 *)&grey_palette_block.save_file_filter_buf),str_mp);
      len = _strlen(((i8 *)&grey_palette_block.save_file_filter_buf));
      crt_strcat((char *)((void *)(len + (iptr)(((u8 *)&grey_palette_block.save_file_filter_buf + 1)))),((i8 *)&save_filter_mp_pattern_str));
    }
  }
  else {
    _memset(((i8 *)&grey_palette_block.save_file_filter_buf),0,0x104);
    str_append_label((iptr)&grey_palette_block.save_file_filter_buf,0x19f);
    crt_strcat(((i8 *)&grey_palette_block.save_file_filter_buf),str_scn);
    len = _strlen(((i8 *)&grey_palette_block.save_file_filter_buf));
    crt_strcat((char *)((void *)(len + (iptr)(((u8 *)&grey_palette_block.save_file_filter_buf + 1)))),str_scn_2);
  }
  return &grey_palette_block.save_file_filter_buf[0];
}

/* 00473ff2 auto */
char * __cdecl save_filter_add_entry(i32 kind,char *buf)

{
  size_t len;
  char *next;
  
  switch(kind) {
  case 0:
    str_append_label((iptr)buf,0xee);
    crt_strcat((char *)((void *)buf),str_sav);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_sav_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  case 1:
    str_append_label((iptr)buf,0x357);
    crt_strcat((char *)((void *)buf),str_hot);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_hot_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  case 2:
    str_append_label((iptr)buf,0x35a);
    crt_strcat((char *)((void *)buf),str_eml);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_eml_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  case 3:
  case 4:
  case 5:
  case 6:
    str_append_label((iptr)buf,0x35d);
    crt_strcat((char *)((void *)buf),str_net);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_net_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  default:
    next = (char *)0x0;
  }
  return next;
}

/* 004741be auto */
i32 __cdecl save_write_game(const char *file_name,int is_scenario)

{
  char *dot;
  size_t nwritten;
  int rc;
  int count;
  uint saved_civ;
  iptr civ;
  iptr advance;
  ScenarioEvent *event;
  i32 result;
  u8 saved_humans;
  i16 version[2];
  int leader;
  char path[260];
  
  result = 1;
  if (game.multiplayer == 1) {
    saved_humans = game.human_players;
    game.human_players = net.hotseat_human_players;
  }
  save_set_file_ext(is_scenario);
  crt_strcpy(path,(const char *)((const void *)file_name));
  dot = _strchr(path,0x2e);
  if (dot == (char *)0x0) {
    crt_strcat(path,((i8 *)&grey_palette_block.save_file_path));
  }
  ScopedFile file(_fopen(path,((i8 *)&save_write_mode)));
  ON_SCOPE_EXIT(if (game.multiplayer == 1) { game.human_players = saved_humans; });
  if (file != (FILE *)0x0) {
    save_write_cstr_eof((const char *)str_ptr_civilize,(iptr)file);
    game.saved_current_civ = *(i8 *)&human_civ;
    version[0] = 0x2c;
    nwritten = _fwrite(version,2,1,file);
    if (nwritten != 0) {
      if (is_scenario == 0) {
        game.game_flags = game.game_flags & ~GAME_SAVED_AS_SCENARIO;
      }
      else {
        game.game_flags = game.game_flags | GAME_SAVED_AS_SCENARIO;
        for (advance = 0; advance < 100; advance++) {
          game.advance_known_by[advance] = '\0';
          for (civ = 1; civ < 8; civ++) {
            rc = civ_has_advance(civ,advance);
            if (rc != 0) {
              game.advance_known_by[advance] =
                   game.advance_known_by[advance] | (byte)(1 << ((byte)civ & 0x1f));
            }
          }
          if (game.advance_known_by[advance] != '\0') {
            rc = bits_count8((int)(char)game.advance_known_by[advance]);
            if (rc == 1) {
              for (civ = 1; civ < 8; civ++) {
                rc = civ_has_advance(civ,advance);
                if (rc != 0) {
                  game.first_discoverer[advance] = (byte)civ;
                }
              }
            }
            else {
              game.first_discoverer[advance] = 8;
            }
          }
        }
        for (civ = 1; civ < 8; civ++) {
          civs[civ].flags = civs[civ].flags & ~(CIVF_REVOLUTION | CIVF_GOV_CHANGED | CIVF_FREE_ADVANCE | CIVF_UNUSED_40);
        }
      }
      if ((is_scenario != 0) || ((game.game_flags & GAME_SCENARIO) != 0)) {
        for (civ = 1; civ < 8; civ++) {
          if (-1 < leaders[civs[civ].tribe].adjective) {
            str_clear((char *)(civ * 0xf2 + (iptr)((u8 *)&civ_names + 50)));
            str_append_string_id((iptr)&civ_names[civ].adjective,
                               (int)leaders[civs[civ].tribe].adjective);
          }
          if (-1 < leaders[civs[civ].tribe].plural) {
            str_clear((char *)(civ * 0xf2 + (iptr)((u8 *)&civ_names + 26)));
            str_append_string_id((iptr)&civ_names[civ].tribe_name,
                               (int)leaders[civs[civ].tribe].plural);
          }
          if (-1 < leaders[civs[civ].tribe].leader_name) {
            str_clear((char *)(civ * 0xf2 + (iptr)((u8 *)&civ_names + 2)));
            str_append_string_id((iptr)&civ_names[civ].leader_name,
                               (int)leaders[civs[civ].tribe].leader_name);
          }
        }
      }
      nwritten = _fwrite(&game,0x14a,1,file);
      if (((((((nwritten != 0) && (nwritten = _fwrite(civ_names,0x790,1,file), nwritten != 0)) &&
             (nwritten = _fwrite(civs,0x2ca0,1,file), nwritten != 0)) &&
            ((rc = map_write((iptr)file,0), rc == 0 &&
             (nwritten = _fwrite(map_data.coarse[0],(int)map.quarter_w * (int)map.quarter_h,1,file),
             nwritten != 0)))) &&
           (nwritten = _fwrite(map_data.coarse[1],(int)map.quarter_w * (int)map.quarter_h,1,file),
           nwritten != 0)) &&
          ((nwritten = _fwrite(((i8 *)&landmass_table_block.landmass_table),0x400,1,file), nwritten != 0 &&
           ((game.num_units == 0 ||
            (nwritten = _fwrite(units,(int)game.num_units << 5,1,file), nwritten != 0)))))) &&
         ((game.num_cities == 0 ||
          (nwritten = _fwrite(cities,game.num_cities * 0x58,1,file), nwritten != 0)))) {
        for (leader = 0; leader < 0x15; leader++) {
          nwritten = _fwrite(&leaders[leader].times_used,1,1,file);
          if (((nwritten == 0) ||
              (nwritten = _fwrite(&leaders[leader].city_name_index,1,1,file), nwritten == 0)) ||
             (nwritten = _fwrite(&leaders[leader].female,1,1,file), nwritten == 0))
          return result;
        }
        nwritten = _fwrite(((i16 *)&map_cursor.x),2,1,file);
        if ((nwritten != 0) && (nwritten = _fwrite(((i16 *)&map_cursor.y),2,1,file), nwritten != 0)) {
          if (game.multiplayer == '\0') {
            nwritten = _fwrite(leaders + civs[game.player_civ].tribe,0x30,1,file);
            if (nwritten == 0) return result;
          }
          else {
            civ = (iptr)game.player_civ;
            for (leader = 0; leader < 8; leader++) {
              if ((1 << ((byte)leader & 0x1f) & (uint)game.human_players) != 0) {
                game.player_civ = (byte)leader;
                nwritten = _fwrite(leaders + civs[(char)(byte)leader].tribe,0x30,1,file);
                if (nwritten == 0) {
                  game.player_civ = (byte)civ;
                  return result;
                }
              }
            }
            game.player_civ = (byte)civ;
          }
          nwritten = _fwrite(((i8 *)&power_graph_history_block.power_graph_history),0x4b0,1,file);
          if (nwritten != 0) {
            saved_screen_rect_block.saved_screen_rect[0] = caption_small_font_block.screen_rect[0];
            saved_screen_rect_block.saved_screen_rect_top = caption_small_font_block.screen_rect_top;
            saved_screen_rect_block.saved_screen_rect_right = caption_small_font_block.ui_screen_rect_right;
            saved_screen_rect_block.saved_screen_rect_bottom = caption_small_font_block.ui_screen_rect_bottom;
            save_pack_layout_rects();
            nwritten = _fwrite(((i16 *)&grey_palette_block.save_layout_record),0x6a,1,file);
            if (nwritten != 0) {
              for (leader = 0; leader < 8; leader++) {
                nwritten = _fwrite(&map_views_block.map_view_recs[leader].open,2,1,file);
                if (((nwritten == 0) ||
                    (nwritten = _fwrite(&map_views_block.map_view_recs[leader].track_moves,2,1,file), nwritten == 0))
                   || ((nwritten = _fwrite(((i16 *)&map_views_block.map_view_center_x) + leader * 0x1f8,2,1,file), nwritten == 0
                       || ((nwritten = _fwrite(((i16 *)&map_views_block.mapview_center_y) + leader * 0x1f8,2,1,file),
                           nwritten == 0 ||
                           (nwritten = _fwrite(((i16 *)&map_views_block.mapview_zoom) + leader * 0x1f8,2,1,file),
                           nwritten == 0)))))) return result;
              }
              if ((((is_scenario == 0) && ((game.game_flags & GAME_SCENARIO) == 0)) ||
                  (nwritten = _fwrite(((i16 *)&scenario.flags),100,1,file), nwritten != 0)) &&
                 (nwritten = _fwrite(((i16 *)&destroyed_civs.count),0x152,1,file), nwritten != 0)) {
                if (game.multiplayer != '\0') {
                  saved_civ = (uint)(*(byte *)&saved_turn.civ);
                  (*(byte *)&saved_turn.civ) = (*(byte *)&net.mp_turn_civ);
                  nwritten = _fwrite(((i8 *)&civ_view.saved_cursor_x),0x494,1,file);
                  if (nwritten == 0) return result;
                  (*(byte *)&saved_turn.civ) = (byte)saved_civ;
                }
                if ((scenario_events_loaded != 0) && (is_scenario == 0)) {
                  rc = _fputs(((i8 *)&save_events_tag_str),file);
                  if (rc == -1) return result;
                  count = 0;
                  for (event = (ScenarioEvent *)(void *)unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
                      event = (ScenarioEvent *)(void *)event->next) {
                    count++;
                  }
                  nwritten = _fwrite(&count,4,1,file);
                  if (nwritten == 0) return result;
                  for (event = (ScenarioEvent *)(void *)unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
                      event = (ScenarioEvent *)(void *)event->next) {
                    nwritten = _fwrite(event,0x1bc,1,file);
                    if (nwritten == 0) return result;
                  }
                  for (event = (ScenarioEvent *)(void *)unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
                      event = (ScenarioEvent *)(void *)event->next) {
                    if (((((event->unit_name != 0) &&
                          (rc = save_write_str(event->unit_name,
                                                      (iptr)file), rc == -1)) ||
                         ((event->city_name != 0 &&
                          (rc = save_write_str(event->city_name,
                                                      (iptr)file), rc == -1)))) ||
                        ((event->who_name != 0 &&
                         (rc = save_write_str(event->who_name,
                                                     (iptr)file), rc == -1)))) ||
                       ((event->whom_name != 0 &&
                        (rc = save_write_str(event->whom_name,
                                                    (iptr)file), rc == -1))))
                    return result;
                    count = 0;
                    while ((count < 0x14 &&
                           (event->text[count] != 0))) {
                      rc = save_write_str(event->text[count],
                                                 (iptr)file);
                      if (rc == -1) return result;
                      count++;
                    }
                    if ((((((event->move_owner_name != 0) &&
                           (rc = save_write_str(event->move_owner_name,
                                                       (iptr)file), rc == -1)) ||
                          ((event->move_unit_name != 0 &&
                           (rc = save_write_str(event->move_unit_name,
                                                       (iptr)file), rc == -1)))) ||
                         (((event->aggression_whom_name != 0 &&
                           (rc = save_write_str(event->aggression_whom_name,
                                                       (iptr)file), rc == -1)) ||
                          ((event->aggression_who_name != 0 &&
                           (rc = save_write_str(event->aggression_who_name,
                                                       (iptr)file), rc == -1)))))) ||
                        ((((event->create_owner_name != 0 &&
                           (rc = save_write_str(event->create_owner_name,
                                                       (iptr)file), rc == -1)) ||
                          ((event->create_unit_name != 0 &&
                           (rc = save_write_str(event->create_unit_name,
                                                       (iptr)file), rc == -1)))) ||
                         (((event->create_homecity != 0 &&
                           (rc = save_write_str(event->create_homecity,
                                                       (iptr)file), rc == -1)) ||
                          ((event->field_140 != 0 &&
                           (rc = save_write_str(event->field_140,
                                                       (iptr)file), rc == -1)))))))) ||
                       ((((event->field_148 != 0 &&
                          (rc = save_write_str(event->field_148,
                                                      (iptr)file), rc == -1)) ||
                         ((event->money_receiver_name != 0 &&
                          (rc = save_write_str(event->money_receiver_name,
                                                      (iptr)file), rc == -1)))) ||
                        ((event->wave_file != 0 &&
                         (rc = save_write_str(event->wave_file,
                                                     (iptr)file), rc == -1))))))
                    return result;
                  }
                }
                result = 0;
                if ((file.close() != 0) && bugfix_ui()) {
                  /* B28: the end of the file could not be written (a full disk): a
                     failed save, not a saved game cut short (docs/BUGS.md) */
                  result = 1;
                }
              }
            }
          }
        }
      }
    }
  }
  return result;
}

/* 00475666: written by hand in src/game/retvals.cpp */

/* 0047758c auto */
void __cdecl game_save_dialog(int is_scenario)

{
  uptr desc_len;
  uptr pattern_len;
  i32 leader_name;
  size_t name_len;
  char *title;
  int result;
  char *name_arg;
  i8 *filter_arg;
  i8 *ext_arg;
  i32 flag_arg;
  int abs_year_rem_neg;
  int abs_year_rem_pos;
  int abs_year_div;
  int abs_year;
  _SYSTEMTIME local_time;
  char num_buf[31];
  char file_name_buf[261];
  char filter_text[260];
  char *str_ptr;
  
  if (civ_saved_cursor_x_block.game_setup_complete != 0) {
    mp_turn_timer_stop(1);
    do {
      save_set_file_ext(is_scenario);
      str_clear((char *)filter_text);
      str_append_label((iptr)filter_text,(-(uint)(is_scenario == 0) & 0xffffff4f) + 0x19f);
      str_append_space((iptr)filter_text);
      str_append_lparen((iptr)filter_text);
      str_append((char *)filter_text,(const char *)&str_empty_94);
      str_append((char *)filter_text,(const char *)&grey_palette_block.save_file_ext_bare);
      str_append_rparen((iptr)filter_text);
      desc_len = _strlen(filter_text);
      str_ptr = (filter_text + 1) + desc_len;
      str_clear(str_ptr);
      str_append(str_ptr,(const char *)&str_empty_95);
      str_append(str_ptr,(const char *)&grey_palette_block.save_file_ext_bare);
      pattern_len = _strlen(str_ptr);
      str_ptr = str_ptr + pattern_len + 1;
      str_clear(str_ptr);
      str_clear((file_name_buf + 1));
      leader_name = (i32)(civ_leader_name(human_civ[0]));
      crt_strcat(file_name_buf + 1,(const char *)((const void *)leader_name));
      file_name_buf[3] = 0;
      str_append((file_name_buf + 1),(const char *)&str_empty_96);
      str_ptr = file_name_buf;
      while (str_ptr = str_ptr + 1, *str_ptr != '\0') {
        if ((31 < *str_ptr) && (*str_ptr < 'A')) {
          *str_ptr = '_';
        }
      }
      text_clear();
      if (game.year < 0) {
        text_append((const char *)&str_b);
      }
      else {
        text_append((const char *)&str_a);
      }
      str_append_space((iptr)(file_name_buf + 1));
      name_len = _strlen(file_name_buf + 1);
      file_name_buf[name_len] = textbuf.buf[0];
      if (game.year < 1) {
        abs_year = ~(int)game.year + 1;
      }
      else {
        abs_year = (int)game.year;
      }
      str_append_int((iptr)(file_name_buf + 1),abs_year);
      if ((game.multiplayer == 2) && (is_scenario == 0)) {
        GetLocalTime(&local_time);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_date_sep_str));
        __itoa((uint)(*(i32 *)((char *)&local_time + 0)) >> 0x10,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_month_day_sep_str));
        __itoa((uint)local_time.wDay,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_day_year_sep_str));
        __itoa((*(i32 *)((char *)&local_time + 0)) & 0xffff,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_time_sep_str));
        __itoa((*(i32 *)((char *)&local_time + 8)) & 0xffff,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_hour_min_sep_str));
        __itoa((uint)(*(i32 *)((char *)&local_time + 8)) >> 0x10,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_min_sec_sep_str));
        __itoa((*(i32 *)((char *)&local_time + 12)) & 0xffff,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
      }
      crt_strcat(file_name_buf + 1,((i8 *)&save_name_ext_dot_str));
      crt_strcat(file_name_buf + 1,((i8 *)&grey_palette_block.save_file_ext_bare));
      __strlwr(file_name_buf + 1);
      if (((is_scenario != 0) || ((game.game_flags & GAME_SCENARIO) != 0)) && (scenario.year_increment[0] < 0)) {
        str_clear((file_name_buf + 1));
        if ((int)game.year / 0xc < 1) {
          abs_year_div = ~((int)game.year / 0xc) + 1;
        }
        else {
          abs_year_div = (int)game.year / 0xc;
        }
        str_append_int((iptr)(file_name_buf + 1),abs_year_div);
        if (game.year < 0) {
          if ((int)game.year % 0xc < 1) {
            abs_year_rem_neg = ~((int)game.year % 0xc) + 1;
          }
          else {
            abs_year_rem_neg = (int)game.year % 0xc;
          }
          str_append_label((iptr)(file_name_buf + 1),0x1af - abs_year_rem_neg);
        }
        else {
          if ((int)game.year % 0xc < 1) {
            abs_year_rem_pos = ~((int)game.year % 0xc) + 1;
          }
          else {
            abs_year_rem_pos = (int)game.year % 0xc;
          }
          str_append_label((iptr)(file_name_buf + 1),abs_year_rem_pos + 0x1a4);
        }
        crt_strcat(file_name_buf + 1,((i8 *)&grey_palette_block.save_file_path));
        __strlwr(file_name_buf + 1);
      }
      __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
      save_file_filter(1,is_scenario,0);
      flag_arg = 1;
      leader_name = 0;
      ext_arg = ((i8 *)&grey_palette_block.save_file_ext_bare);
      filter_arg = ((i8 *)&grey_palette_block.save_file_filter_buf);
      name_arg = file_name_buf + 1;
      title = label_string(label_table.string_ids[0x1a0 + (-(uint)(is_scenario == 0) & 0xffffff4f)]
                                );
      result = ui_file_name_dialog((iptr)&main_frame_wnd_block.main_frame_wnd,(const char *)title,(iptr)name_arg,(iptr)filter_arg,(iptr)ext_arg,leader_name,flag_arg);
      if (result != 0) {
        __chdir(((i8 *)&game_dirs.home));
        result = save_write_game((const char *)(file_name_buf + 1),is_scenario);
        if (result == 0) {
          if (is_scenario == 0) {
            if (game.multiplayer == 2) {
              text_set_string_arg(0,(const char *)(file_name_buf + 1));
              popup_game((const char *)&str_emailsaved);
            }
            else {
              title = (char *)civ_leader_title(human_civ[0]);
              text_set_string_arg(0,title);
              title = civ_leader_name(human_civ[0]);
              text_set_string_arg(1,title);
              title = civ_name_plural(human_civ[0]);
              text_set_string_arg(2,title);
              text_clear();
              text_append_year((int)game.year);
              text_set_string_arg(3,(const char *)&textbuf.buf);
              text_set_label_arg(4,difficulty_names[game.difficulty]);
              popup_game((const char *)&str_saveok);
            }
          }
          else {
            text_set_string_arg(0,(const char *)&scenario.name);
            popup_game((const char *)&str_scenok);
          }
        }
        else {
          text_set_string_arg_b(0,(const char *)(((i8 * *)&str_ptr_no_error))[crt_errno]);
          popup_game((const char *)&str_saveerror);
        }
        break;
      }
      __chdir(((i8 *)&game_dirs.home));
    } while (game.multiplayer == 2);
    mp_turn_timer_resume(1);
  }
  return;
}

/* 00477d8c auto */
int __cdecl game_load_dialog(int is_scenario,i32 unused_arg,int show_title)

{
  size_t str_len;
  char *str_arg;
  int result;
  char *ext_match;
  HANDLE find_handle;
  BOOL found_next;
  i8 *name_arg;
  i8 *filter_arg;
  i8 *ext_arg;
  i32 flag_arg;
  i32 flag_arg2;
  i32 cheat_checked;
  uint pick_index;
  _WIN32_FIND_DATAA find_data;
  CHAR alt_pattern[260];
  int alt_count;
  char *slash_ptr;
  iptr unit;
  i8 file_path[260];
  int failed;
  char filter_text[260];
  GamePalette palette;
  i8 events_path[260];
  char *str_ptr;
  int view_index;
  char upper_path[260];
  int load_error;
  
  failed = 1;
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  if (is_scenario == 0) {
    save_set_file_ext(0);
    net.loaded_file_kind = 1;
  }
  else {
    save_set_file_ext(1);
    net.loaded_file_kind = 2;
  }
  str_clear((char *)filter_text);
  str_append_label((iptr)filter_text,(-(uint)(is_scenario == 0) & 0xffffff4f) + 0x19f);
  str_append_space((iptr)filter_text);
  str_append_lparen((iptr)filter_text);
  str_append((char *)filter_text,(const char *)&str_empty_97);
  str_append((char *)filter_text,(const char *)&grey_palette_block.save_file_ext_bare);
  str_append_rparen((iptr)filter_text);
  str_len = _strlen(filter_text);
  str_ptr = (filter_text + 1) + str_len;
  str_clear(str_ptr);
  str_append(str_ptr,(const char *)&str_empty_98);
  str_append(str_ptr,(const char *)&grey_palette_block.save_file_ext_bare);
  str_len = _strlen(str_ptr);
  str_ptr = str_ptr + str_len + 1;
  str_clear(str_ptr);
  crt_strcpy(file_path,((i8 *)&load_dialog_wildcard_str));
  crt_strcat(file_path,((i8 *)&grey_palette_block.save_file_ext_bare));
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  save_file_filter(0,is_scenario,0);
  flag_arg2 = 0;
  flag_arg = 1;
  ext_arg = ((i8 *)&grey_palette_block.save_file_ext_bare);
  filter_arg = ((i8 *)&grey_palette_block.save_file_filter_buf);
  name_arg = file_path;
  str_arg = label_string(label_table.string_ids[0x1a1 + (-(uint)(is_scenario == 0) & 0xffffff4f)]);
  result = ui_file_name_dialog((iptr)&main_frame_wnd_block.main_frame_wnd,(const char *)str_arg,(iptr)name_arg,(iptr)filter_arg,(iptr)ext_arg,flag_arg,flag_arg2);
  if (result == 0) {
    __chdir(((i8 *)&game_dirs.home));
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
    for (view_index = 1; view_index < 8; view_index++) {
      if (map_views_block.map_view_recs[view_index].open != 0) {
        map_views_block.map_view_recs[view_index].open = 0;
        map_views_block.map_view_recs[view_index].wnd_destroy_with_surface();
      }
    }
    crt_strcpy(upper_path,file_path);
    __strupr(upper_path);
    ext_match = _strstr(upper_path,((i8 *)&game_load_scn_ext_str));
    if (ext_match != (char *)0x0) {
      crt_strcpy(upper_path,file_path);
      slash_ptr = _strrchr(upper_path,0x5c);
      slash_ptr++;
      *slash_ptr = '\0';
      crt_strcpy(alt_pattern,upper_path);
      crt_strcat(alt_pattern,str_alt);
      alt_count = 0;
      find_handle = FindFirstFileA(alt_pattern,&find_data);
      if (find_handle != 0xffffffff) {
        do {
          alt_count++;
          found_next = FindNextFileA(find_handle,&find_data);
        } while (found_next == 1);
        FindClose(find_handle);  /* (port) the original kept both searches open */
      }
      if (alt_count != 0) {
        result = _rand();
        pick_index = (uint)(result * (alt_count + 1)) >> 0xf;
        if (pick_index != 0) {
          find_handle = FindFirstFileA(alt_pattern,&find_data);
          while (pick_index = pick_index - 1, pick_index != 0) {
            FindNextFileA(find_handle,&find_data);
          }
          if (find_handle != 0xffffffff) {
            FindClose(find_handle);
          }
          crt_strcat(upper_path,find_data.cFileName);
          crt_strcpy(file_path,upper_path);
        }
      }
    }
    snd_play_effect(-0x6b,0,0,0);
    snd_play_effect(-0x6c,0,0,0);
    snd_play_effect(SND_MENUOK,0,0,0);
    load_error = save_read_game(file_path);
    if (load_error == 0) {
      ui_flush_input();
      if ((scenario_events_loaded == 0) && ((game.game_flags & GAME_SCENARIO) != 0)) {
        scenario_events_loaded = 0;
        __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
        crt_strcpy(events_path,str_events);
        crt_strcat(events_path,((i8 *)&lang_file_ext));
        result = file_exists((const char *)events_path);
        if (result != 0) {
          crt_strcpy(events_path,((i8 *)&textbuf.buf));
          ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
          result = ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->parse_file((const char *)&str_events_2,(const char *)&str_beginevents);
          if (result == 0) {
            events_compact();
            scenario_events_loaded = 1;
          }
          crt_strcpy(((i8 *)&textbuf.buf),events_path);
        }
      }
      if (scenario_events_loaded == 0) {
        ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
      }
      game.game_options = prefs.game_options;
      snd_wave_set_buffer_count(8);
      snd_cd_detect_layout();
      if ((game.game_options & OPT_MUSIC) != 0) {
        if (scenario_events_loaded == 0) {
          snd_sync_music_option();
        }
        else {
          ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_scenario_loaded();
          snd_sync_music_option();
        }
      }
      if (show_title != 0) {
        ui_flush_input();
        result = file_exists((const char *)&str_title_gif);
        if ((result != 0) &&
           (result = ((DibSurface *)((i8 *)&scenario_title_surface_block.scenario_title_surface))->load_gif_file((const char *)&str_title_gif_2,10,0xc0,(iptr)&palette),
           result != 0)) {
          main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&scenario_title_surface_block.scenario_title_surface);
          main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
          main_frame_wnd_block.main_frame_wnd.maximize();
          wnd_flush_paint();
          input_flush_events();
        }
      }
      __chdir(((i8 *)&game_dirs.home));
      failed = 0;
      str_arg = (char *)civ_leader_title(human_civ[0]);
      text_set_string_arg(0,str_arg);
      str_arg = civ_leader_name(human_civ[0]);
      text_set_string_arg(1,str_arg);
      str_arg = civ_name_plural(human_civ[0]);
      text_set_string_arg(2,str_arg);
      text_clear();
      text_append_year((int)game.year);
      text_set_string_arg(3,(const char *)&textbuf.buf);
      text_set_label_arg(4,difficulty_names[game.difficulty]);
      if (((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) {
        cheat_checked = 0;
      }
      else {
        cheat_checked = 1;
      }
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_CHEAT_MODE,cheat_checked);
      if ((is_scenario == 0) && (game.multiplayer == '\0')) {
        popup_game((const char *)&str_loadok);
      }
      net.host_flag = 1;
      for (unit = 0; unit < game.num_units; unit++) {
        if (units[unit].id == 0) {
          units[unit].next_in_stack = -1;
          units[unit].prev_in_stack = -1;
        }
        else {
          unit_stack_validate(unit);
          if ((units[unit].x < 0) || (units[unit].y < 0)) {
            dbg_logf3((const char *)&str_stacked_unit_in_save_unit_d_type_d_whose,unit,(uint)units[unit].type,(int)units[unit].owner)
            ;
            unit_delete(unit,0);
          }
        }
      }
      text_clear();
      text_append((const char *)file_path);
    }
    else {
      switch(load_error) {
      case 1:
        popup_game((const char *)&str_loadnotsave);
        break;
      case 2:
        popup_game((const char *)&str_loadoldsave);
        break;
      case 3:
        text_set_string_arg_b(0,(const char *)(((i8 * *)&str_ptr_no_error))[crt_errno]);
        popup_game((const char *)&str_loadbadsave);
        break;
      case 4:
        text_set_string_arg_b(0,(const char *)(((i8 * *)&str_ptr_no_error))[crt_errno]);
        popup_game((const char *)&str_loadnewsave);
      }
    }
  }
  result = failed;
  if (failed != 0) {
    net.loaded_file_kind = 0;
  }
  return result;
}

/* 0048237d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl turn_popup_timeout_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  now = timer_ticks();
  if (0x4b0 < now - dialog_state.timeout_start) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    dialog_state.timeout_start = timer_ticks();
  }
  return;
}

/* 0048308f auto */
void __cdecl events_relink_pool(void)

{
  int event_count;
  size_t str_len;
  int len_184;
  int len_174;
  int len_148;
  int len_140;
  int len_13c;
  int len_dc;
  int len_d4;
  int len_cc;
  int len_c4;
  int len_90;
  int len_88;
  int len_slot;
  int len_20;
  int len_14;
  int len_10;
  int len_08;
  char *string_ptr;
  char *event;
  int i;
  
  event_count = unit_types_block.scenario_event_count;
  if (scenario_events_loaded != 0) {
    unit_types_block.events_list_head = (ScenarioEvent *)unit_types_block.events_pool_base;
    event = unit_types_block.events_pool_base;
    string_ptr = unit_types_block.events_pool_base + unit_types_block.scenario_event_count * 0x1c4;
    if ((string_ptr <= unit_types_block.events_pool_base) || (unit_types_block.events_pool_base + unit_types_block.events_pool_size <= string_ptr)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_pstrings_charptr_pevents_pstrings_scenar,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_6,0x913);
    }
    for (i = 0; i < event_count; i++) {
      event[0x1bc] = '\0';
      event[0x1bd] = '\0';
      event[0x1be] = '\0';
      event[0x1bf] = '\0';
      event[0x1c0] = '\0';
      event[0x1c1] = '\0';
      event[0x1c2] = '\0';
      event[0x1c3] = '\0';
      if (0 < i) {
        *(char **)(event + 0x1c0) = event;
        *(int *)(event + 0x1c0) = *(int *)(event + 0x1c0) - 0x1c4;
        *(char **)(*(int *)(event + 0x1c0) + 0x1bc) = event;
      }
      event += 0x1c4;
    }
    for (event = (char *)unit_types_block.events_list_head; event != (char *)0x0;
        event = (char *)((ScenarioEvent *)event)->next) {
      if ((int)((ScenarioEvent *)event)->unit_name != 0) {
        ((ScenarioEvent *)event)->unit_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_08 = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_08 = str_len + 1;
        }
        string_ptr += len_08;
      }
      if ((int)((ScenarioEvent *)event)->city_name != 0) {
        ((ScenarioEvent *)event)->city_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_10 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_10 = str_len + 1;
        }
        string_ptr += len_10;
      }
      if ((int)((ScenarioEvent *)event)->who_name != 0) {
        ((ScenarioEvent *)event)->who_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_14 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_14 = str_len + 1;
        }
        string_ptr += len_14;
      }
      if ((int)((ScenarioEvent *)event)->whom_name != 0) {
        ((ScenarioEvent *)event)->whom_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_20 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_20 = str_len + 1;
        }
        string_ptr += len_20;
      }
      for (i = 0; i < 0x14; i++) {
        if ((int)((ScenarioEvent *)event)->text[i] != 0) {
          ((ScenarioEvent *)event)->text[i] = string_ptr;
          str_len = _strlen(string_ptr);
          if (str_len + 1 < 2) {
            len_slot = 1;
          }
          else {
            str_len = _strlen(string_ptr);
            len_slot = str_len + 1;
          }
          string_ptr += len_slot;
        }
      }
      if ((int)((ScenarioEvent *)event)->move_owner_name != 0) {
        ((ScenarioEvent *)event)->move_owner_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_88 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_88 = str_len + 1;
        }
        string_ptr += len_88;
      }
      if ((int)((ScenarioEvent *)event)->move_unit_name != 0) {
        ((ScenarioEvent *)event)->move_unit_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_90 = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_90 = str_len + 1;
        }
        string_ptr += len_90;
      }
      if ((int)((ScenarioEvent *)event)->aggression_whom_name != 0) {
        ((ScenarioEvent *)event)->aggression_whom_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_c4 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_c4 = str_len + 1;
        }
        string_ptr += len_c4;
      }
      if ((int)((ScenarioEvent *)event)->aggression_who_name != 0) {
        ((ScenarioEvent *)event)->aggression_who_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_cc = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_cc = str_len + 1;
        }
        string_ptr += len_cc;
      }
      if ((int)((ScenarioEvent *)event)->create_owner_name != 0) {
        ((ScenarioEvent *)event)->create_owner_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_d4 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_d4 = str_len + 1;
        }
        string_ptr += len_d4;
      }
      if ((int)((ScenarioEvent *)event)->create_unit_name != 0) {
        ((ScenarioEvent *)event)->create_unit_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_dc = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_dc = str_len + 1;
        }
        string_ptr += len_dc;
      }
      if ((int)((ScenarioEvent *)event)->create_homecity != 0) {
        ((ScenarioEvent *)event)->create_homecity = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_13c = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_13c = str_len + 1;
        }
        string_ptr += len_13c;
      }
      if ((int)((ScenarioEvent *)event)->field_140 != 0) {
        ((ScenarioEvent *)event)->field_140 = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_140 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_140 = str_len + 1;
        }
        string_ptr += len_140;
      }
      if ((int)((ScenarioEvent *)event)->field_148 != 0) {
        ((ScenarioEvent *)event)->field_148 = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_148 = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_148 = str_len + 1;
        }
        string_ptr += len_148;
      }
      if ((int)((ScenarioEvent *)event)->money_receiver_name != 0) {
        ((ScenarioEvent *)event)->money_receiver_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_174 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_174 = str_len + 1;
        }
        string_ptr += len_174;
      }
      if ((int)((ScenarioEvent *)event)->wave_file != 0) {
        ((ScenarioEvent *)event)->wave_file = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 2) {
          len_184 = 1;
        }
        else {
          str_len = _strlen(string_ptr);
          len_184 = str_len + 1;
        }
        string_ptr += len_184;
      }
    }
  }
  return;
}

/* 00484cc0 auto */
void __cdecl scenario_reset_params(void)

{
  iptr i;
  
  (*(i16 *)&scenario.flags) = 0;
  scenario.name[0] = 0;
  scenario.tech_paradigm = 10;
  scenario.year_increment[0] = 0;
  scenario.start_year = 0;
  scenario.max_turns = 0;
  scenario.objective_civ = 0;
  for (i = 0; i < 4; i++) {
    (((i16 *)&scenario.victory_thresholds))[i] = 0;
  }
  return;
}

/* 00484d85 auto */
i32 __cdecl main_frame_on_close(void)

{
  int confirmed;
  char *name_str;
  u8 saved_human_players2;
  
  if (((game.multiplayer < 3) || (net.host_handoff_stage == '\0')) &&
     (confirmed = popup_game_with_sprite((const char *)&str_reallyquit_2,(iptr)&popup_info_icon_sprite,0), confirmed != 0)) {
    if (game.multiplayer == '\0') {
      snd_cd_stop();
      map_display_disable();
    }
    else {
      if (game.multiplayer == 1) {
        saved_human_players2 = game.human_players;
        game.human_players = net.hotseat_human_players;
      }
      if (2 < game.multiplayer) {
        if (mp_timer.handle != 0) {
          game_timer_destroy(mp_timer.handle);
          mp_timer.handle = 0;
        }
        parley_wnd_close();
        chat_wnd_close();
        name_str = (char *)civ_get_adjective((*(i8 *)&human_civ));
        text_set_string_arg(0,name_str);
        name_str = (char *)civ_leader_title((*(i8 *)&human_civ));
        text_set_string_arg(1,name_str);
        name_str = (civ_leader_name((*(i8 *)&human_civ)));
        text_set_string_arg(2,name_str);
        name_str = (civ_name_plural((*(i8 *)&human_civ)));
        text_set_string_arg(3,name_str);
        if (mp_options.kill_civ_on_retire == 0) {
          net_send_text_args(1,0xff,4,0,0,0);
        }
        else {
          net_send_text_args(0,0xff,4,0,0,0);
        }
      }
      blink.unit_phase = 0;
      mapviews_redraw_cursor();
      if ((~(1 << ((byte)(*(i8 *)&human_civ) & 0x1f)) & (uint)game.human_players) == 0) {
        if (game.multiplayer < 3) {
          game.human_players = game.human_players & ~(byte)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
        }
        else {
          net_send_message(NM_UNLOCK_NATION,0,0,(*(i8 *)&human_civ),0,0,0,0,0,0);
        }
        map_display_disable();
        snd_cd_stop();
      }
      else {
        civ_eliminate((*(i8 *)&human_civ),0,0);
        if (2 < game.multiplayer) {
          map_display_disable();
        }
      }
      player_turn_active = 0;
      net.player_has_quit = 1;
      if (game.multiplayer == 1) {
        net.hotseat_human_players = game.human_players;
        game.human_players = saved_human_players2;
      }
    }
  }
  return 0;
}

/* 00484fec auto */
int __cdecl game_turn_to_year(int turn)

{
  int table_index;
  int abs_increment;
  int extra_turns;
  int period;
  int year2;
  uint level;
  
  extra_turns = 0;
  level = (uint)game.difficulty;
  if ((game.game_flags & GAME_LARGE_MAP) != 0) {
    level--;
  }
  if ((game.game_flags & GAME_SMALL_MAP) != 0) {
    level++;
  }
  table_index = int_clamp(level - 1,0,3);
  if (-1 < game.first_launch_turn) {
    extra_turns = turn - game.first_launch_turn;
    if (extra_turns < 1) {
      extra_turns = 0;
    }
    turn -= extra_turns;
  }
  if ((((game.game_flags & GAME_SCENARIO) == 0) || (scenario.year_increment[0] == 0)) || (scenario.start_year == 0)) {
    if (turn != 0) {
      turn--;
    }
    for (period = 0;
        (year_period_table[table_index * 6 + period].turn_count < turn && (period < 6));
        period = period + 1) {
      turn -= year_period_table[table_index * 6 + period].turn_count;
    }
    if (period < 6) {
      year2 = year_period_table[table_index * 6 + period].year_step * turn +
                 year_period_table[table_index * 6 + period].start_year;
    }
    else {
      year2 = year_period_table[table_index * 6 + 5].year_step * year_period_table[table_index * 6 + 5].turn_count +
                 year_period_table[table_index * 6 + 5].start_year + turn;
    }
    if (year2 == 0) {
      year2 = 1;
    }
  }
  else {
    if (scenario.year_increment[0] < 1) {
      abs_increment = ~(int)scenario.year_increment[0] + 1;
    }
    else {
      abs_increment = (int)scenario.year_increment[0];
    }
    year2 = (int)scenario.start_year + turn * abs_increment - 1;
  }
  return year2 + extra_turns;
}

/* Part of game_spawn_barbarians (00485c15): an uprising near a city of a
   human player (chance by its treasury share): a horde on a free tile of its
   landmass, its size from the city size (freq_mask: the one for the
   barbarian activity, kept unless the city size replaces it). Calls rand()
   as crt_rand_at("game_spawn_barbarians") for the rand trace. */
static void barbarians_city_uprising(iptr city,uint freq_mask)
{
  short y;
  int unit;
  int share;
  uint horde_size;
  char *label;
  int bonus_roll;
  int surge_roll;
  iptr unit2;
  int horde_count;
  iptr alt_type;
  int attempts;
  iptr unit_type;
  uint y2;
  uint x2;
  iptr main_type;
  uint remaining;

  unit = crt_rand_at("game_spawn_barbarians");
  share = city_treasury_share(city);
  if (unit % 100 < share) {
    attempts = 0;
    for (;;) {
      unit = crt_rand_at("game_spawn_barbarians");
      x2 = map_wrap_x((int)cities[city].x + (unit % 0xd) * 2 - 6);
      y = cities[city].y;
      unit = crt_rand_at("game_spawn_barbarians");
      y2 = ((int)y + unit % 0xd) - 6;
      if ((x2 & 1) != 0) {
        x2--;
      }
      if ((y2 & 1) != 0) {
        x2++;
      }
      unit = map_in_bounds(x2,y2);
      if ((unit != 0) &&
         (city = city_find_nearest(x2,y2,-1,0xffffffff,-1), city < 0)) {
        attempts = 200;
        break;
      }
      attempts++;
      if ((int)((uint)game.barbarian_activity * 0x32) <= attempts) break;
      unit = map_in_bounds(x2,y2);
      if (((unit == 0) || (advisor_window_block.nearest_city_dist < 5)) ||
          ((unit = tile_zoc_unit_adjacent(x2,y2,0), unit != 0 ||
           ((unit = tile_is_ocean(x2,y2), unit != 0 ||
            (unit = tile_unit_owner(x2,y2), -1 < unit)))))) continue;
      unit = tile_landmass((int)cities[city].x,(int)cities[city].y);
      share = tile_landmass(x2,y2);
      if (unit == share) break;
    }
    if (attempts < (int)((uint)game.barbarian_activity * 0x32)) {
      if (((game.barbarian_activity == 1) || (game.barbarian_activity == 2)) ||
         (game.barbarian_activity == 3)) {
        freq_mask = (uint)cities[city].size;
      }
      if (freq_mask == 1 || (int)(freq_mask - 1) < 0) {
        horde_count = 0;
      }
      else {
        horde_count = crt_rand_at("game_spawn_barbarians");
        horde_count = horde_count % (int)freq_mask;
      }
      if ((game.barbarian_activity == 3) && (0x95 < game.turn)) {
        if (-(uint)game.difficulty == -9 || (int)(-(uint)game.difficulty + 9) < 0) {
          surge_roll = 0;
        }
        else {
          surge_roll = crt_rand_at("game_spawn_barbarians");
          surge_roll = surge_roll % (int)(10 - (uint)game.difficulty);
        }
        if (surge_roll == 0) {
          if ((int)freq_mask / 2 == 1 || (int)freq_mask / 2 - 1 < 0) {
            bonus_roll = 0;
          }
          else {
            bonus_roll = crt_rand_at("game_spawn_barbarians");
            bonus_roll = bonus_roll % ((int)freq_mask / 2);
          }
          horde_count = horde_count + bonus_roll + (int)freq_mask / 2;
        }
      }
      horde_size = int_clamp(horde_count,2,99);
      main_type = 0x10;
      alt_type = 0xf;
      unit = crt_rand_at("game_spawn_barbarians");
      if (unit % 3 == 0) {
        int_swap((iptr)&main_type,(iptr)&alt_type);
      }
      label = label_string(label_table.string_ids[0x62]);
      text_set_string_arg(0,label);
      if ((10 < (int)horde_size) && ((int)cities[city].size / 2 < (int)horde_size)) {
        label = label_string(label_table.string_ids[0x63]);
        text_set_string_arg(0,label);
      }
      if (game.advance_known_by[ADV_GUNPOWDER] != '\0') {
        label = label_string(label_table.string_ids[100]);
        text_set_string_arg(0,label);
        if ((game.rules & RULE_BASE_1) != 0) {
          main_type = 4;
          alt_type = 0x11;
        }
      }
      if (game.advance_known_by[ADV_INDUSTRIALIZATION] != '\0') {
        label = label_string(label_table.string_ids[0x65]);
        text_set_string_arg(0,label);
        main_type = 0x18;
        alt_type = 7;
        if (game.advance_known_by[unit_types[UNIT_ARMOR].prerequisite] != '\0') {
          label = label_string(label_table.string_ids[0x66]);
          text_set_string_arg(0,label);
          alt_type = 0xb;
          if (((game.rules & RULE_BASE_1) != 0) && (main_type = 0x19, game.advance_known_by[ADV_GUERRILLA_WARFARE] != '\0'))
          {
            if ((civs[game.best_human_rank].government < GOV_REPUBLIC) ||
               (cities[city].owner != cities[city].founder)) {
              main_type = 0x15;
              alt_type = 9;
            }
            else {
              alt_type = 8;
              label = label_string(label_table.string_ids[0x67]);
              text_set_string_arg(0,label);
            }
          }
        }
      }
      unit2 = -1;
      for (remaining = horde_size; -1 < (int)remaining; remaining--) {
        if ((remaining & 4) == 0) {
          unit_type = alt_type;
        }
        else {
          unit_type = main_type;
        }
        if (remaining == horde_size) {
          unit_type = 0x2e;
        }
        unit2 = unit_create(unit_type,0,x2,y2);
        if (-1 < unit2) {
          if (unit_type == UNIT_FANATICS) {
            units[unit2].flags = units[unit2].flags | UNITF_VETERAN;
          }
          unit = (int)tile_at(x2,y2);
          units[unit2].visibility = *(byte *)(unit + 4) | units[unit2].visibility;
        }
      }
      if ((-1 < unit2) && (autoplay.active == 0)) {
        text_set_string_arg(1,(const char *)&cities[city].name);
        if ((cities[city].owner == human_civ[0]) &&
           (unit = tile_visible_to(x2,y2,human_civ[0]), unit != 0)) {
          dlg_game_txt_with_unit((const char *)&str_barbarians,unit2);
        }
        else if ((((cities[city].owner != human_civ[0]) && (2 < game.multiplayer)) &&
                 (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0)) &&
                (unit = tile_visible_to(x2,y2,(int)cities[city].owner),
                unit != 0)) {
          net_send_message(NM_DRAW_SQUARE,net.net_slots[net.net_civ_slots[cities[city].owner]].id,
                             x2,y2,0,0,0,0,0,0);
          net_send_text_args(2,net.net_slots[net.net_civ_slots[cities[city].owner]].id,2,0
                             ,unit2,0);
        }
      }
      if (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0) {
        map_redraw_tile_all_views(x2,y2);
      }
    }
  }
}

/* Part of game_spawn_barbarians (00485c15): a raid from the sea: an ocean
   tile of a large water body with no units, a ship and its horde (the leader
   and escorts by the advances known). Calls rand() as
   crt_rand_at("game_spawn_barbarians") for the rand trace.  Returns true when no
   tile was found in 1000 tries: the original returns from game_spawn_barbarians
   there (no uprising this turn). */
static bool barbarians_sea_raid(void)
{
  iptr rand_x;
  iptr is_ocean;
  int unit;
  uint horde_size;
  uint sign_mask;
  int rand_y;
  int tries;
  iptr escort_type;
  int horde_count;
  iptr unit_type2;
  iptr unit_type;
  uint y2;
  uint x2;
  int unit3;
  uint remaining;

  tries = 0;
  do {
    if (map.width == 1 || map.width - 1 < 0) {
      x2 = 0;
    }
    else {
      rand_x = crt_rand_at("game_spawn_barbarians");
      x2 = rand_x % (int)map.width;
    }
    if (map.height == 7 || map.height - 7 < 0) {
      rand_y = 0;
    }
    else {
      rand_y = crt_rand_at("game_spawn_barbarians");
      rand_y = rand_y % (map.height - 6);
    }
    y2 = rand_y + 3;
    if ((x2 & 1) != 0) {
      x2--;
    }
    if ((y2 & 1) != 0) {
      x2++;
    }
    tries++;
    if (1000 < tries) {
      return true;
    }
    is_ocean = tile_is_ocean(x2,y2);
  } while (((is_ocean == 0) || (unit = tile_unit_owner(x2,y2), -1 < unit)) ||
          (unit = tile_landmass(x2,y2),
          landmass_table_block.landmass_recs[unit].water_body_size < 0x10));
  unit = int_clamp((int)game.turn /
                             (int)((uint)game.barbarian_activity * -0x32 + 0xfa) + 1,1,5);
  horde_count = unit + 1;
  if (game.barbarian_activity == 3) {
    horde_count = unit + 2;
  }
  unit_type2 = 0x20;
  if (game.advance_known_by[ADV_NAVIGATION] != '\0') {
    unit_type2 = 0x21;
  }
  if (game.advance_known_by[ADV_MAGNETISM] != '\0') {
    unit_type2 = 0x23;
  }
  escort_type = 2;
  if ((game.rules & RULE_BASE_1) != 0) {
    escort_type = 4;
  }
  if ((game.human_players & game.advance_known_by[ADV_IRON_WORKING]) != 0) {
    escort_type = 5;
  }
  if ((game.advance_known_by[ADV_METALLURGY] != '\0') ||
     ((game.barbarian_activity == 3 && (game.advance_known_by[ADV_CHIVALRY] != '\0')))) {
    if (((x2 & 1) == 0) || ((game.rules & RULE_BASE_1) == 0)) {
      escort_type = 0x13;
    }
    else {
      escort_type = 0x12;
    }
  }
  remaining = horde_count;
  if (((game.human_players & game.advance_known_by[ADV_TACTICS]) != 0) && ((game.rules & RULE_BASE_1) != 0)) {
    escort_type = 0x14;
  }
  while( true ) {
    remaining--;
    if ((int)remaining < 0) break;
    unit_type = escort_type;
    if ((remaining != 0) && (horde_count - 1 == remaining)) {
      unit_type = 0x2e;
    }
    unit = unit_create(unit_type,0,x2,y2);
    if (-1 < unit) {
      unit_set_sleep(unit);
    }
  }
  unit = unit_create(unit_type2,0,x2,y2);
  unit3 = unit;
  if (unit < 0) {
    unit = tile_first_unit(x2,y2);
    if (-1 < unit) {
      unit_stack_delete_all(unit,1);
    }
  }
  else {
    horde_size = crt_rand_at("game_spawn_barbarians");
    sign_mask = (int)horde_size >> 0x1f;
    if (((horde_size ^ sign_mask) - sign_mask & 1 ^ sign_mask) == sign_mask) {
      units[unit].flags = units[unit].flags | UNITF_VETERAN;
    }
    unit_relocate_in_place(unit3,1);
  }
  return false;
}

/* 00485c15 auto */
void __cdecl game_spawn_barbarians(void)

{
  int unit;
  iptr city;
  uint freq_mask;
  
  if (game.barbarian_activity != '\0') {
    if (game.difficulty < DIFF_DEITY) {
      unit = ((3 - (uint)game.barbarian_activity) * 3 + 0x1e) * (5 - (uint)game.difficulty);
      if (unit - game.turn != 0 && game.turn <= unit) {
        return;
      }
    }
    else if ((int)game.turn < (int)((3 - (uint)game.barbarian_activity) * 3 + 0xf)) {
      return;
    }
    if (game.barbarian_activity == 1) {
      freq_mask = 0xf;
    }
    else if (game.barbarian_activity == 2) {
      freq_mask = 7;
    }
    else if (game.barbarian_activity == 3) {
      freq_mask = 7;
    }
    if (((freq_mask & (int)game.turn + 1U) == 0) &&
       (game.advance_known_by[unit_types[UNIT_ARMOR].prerequisite] == '\0')) {
      if (barbarians_sea_raid()) return;
    }
    if (game.num_cities == 1 || game.num_cities - 1 < 0) {
      city = 0;
    }
    else {
      unit = _rand();
      city = unit % (int)game.num_cities;
    }
    if (((city < game.num_cities) && (cities[city].id != 0)) &&
       (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0)) {
      barbarians_city_uprising(city,freq_mask);
    }
  }
  return;
}

/* 00486c2e auto */
void __cdecl game_update_global_warming(void)

{
  i8 old_warmings;
  int delta;
  int has_transit;
  i32 clamped;
  int transit_count;
  iptr city;
  int pollution;
  
  delta = bits_count8((uint)game.human_players);
  pollution = ((int)game.polluted_tiles - (int)game.warming_pollution) + (int)game.warming_pollution / 2;
  transit_count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (has_transit = city_has_improvement(city,IMP_SOLAR_PLANT), has_transit != 0)) {
      transit_count++;
    }
  }
  if (1 < delta) {
    pollution = (delta - 1 + pollution) / delta;
  }
  delta = (pollution * 2 + game.global_warmings * -4) - transit_count;
  if (delta == game.warming_counter || delta - game.warming_counter < 0) {
    if (((pollution * 2 + game.global_warmings * -4) - transit_count) - (int)game.warming_counter < 0) {
      game.warming_counter--;
    }
  }
  else {
    game.warming_counter++;
  }
  clamped = int_clamp((int)game.warming_counter,0,99);
  game.warming_counter = (i8)clamped;
  if (((game.warming_counter == 12) && (6 < pollution)) && (autoplay.active == 0)) {
    if (2 < game.multiplayer) {
      net_send_text_args(4,0xff,0,0,0,0);
    }
    popup_game_with_sprite((const char *)&str_fearwarming,(iptr)&popup_info_icon_sprite,0);
  }
  old_warmings = game.global_warmings;
  if (16 < game.warming_counter) {
    game.global_warmings++;
    map_apply_global_warming((int)old_warmings);
    game.warming_counter = '\0';
  }
  game.warming_pollution = 0;
  return;
}

/* 00487371 auto */
void __cdecl game_advance_turn(iptr civ)

{
  i32 value;
  uint level;
  int result;
  iptr adjective;
  iptr civ2;
  int other_civ;
  int civ3;
  int view_index;
  
  if (game.turn != 0) {
    game_spawn_barbarians();
    game_update_global_warming();
    civ_update_power_ranks();
    if (game.multiplayer == '\0') {
      civ_check_throne_room_upgrade();
    }
  }
  game.turn++;
  value = game_turn_to_year((int)game.turn);
  game.year = (i16)value;
  if (199 < game.turn) {
    game.peace_turns++;
  }
  game.transient_flags = game.transient_flags & ~TRANSIENT_END_OF_TURN;
  units_reset_for_turn(civ);
  level = (uint)game.difficulty;
  if ((int)game.turn % (int)((level + 1) * 0xc) == 0) {
    for (civ2 = 0; civ2 < 8; civ2++) {
      if (((((int)game.turn % (int)((level + 1) * 0x18) == 0) ||
           (result = civ_has_wonder(civ2,WONDER_EIFFEL_TOWER), result != 0)) &&
          ((1 < civs[civ2].reputation || (result = civ_has_wonder(civ2,WONDER_EIFFEL_TOWER), result != 0)
           ))) && (civs[civ2].reputation != '\0')) {
        civs[civ2].reputation += 0xff;
        if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
          for (civ3 = 0; civ3 < 8; civ3++) {
            value = int_clamp((int)civs[civ3].reputation_credit[civ2],0,
                                       (uint)civs[civ2].reputation);
            civs[civ3].reputation_credit[civ2] = (char)value;
          }
        }
      }
    }
  }
  if (game.next_history_turn < game.turn) {
    result = _rand();
    game.next_history_turn = game.turn + (short)(result % 0x28) + 0x14;
    if (autoplay.active == 0) {
      if (2 < game.multiplayer) {
        net_send_text_args(5,0xff,0,0,0,0);
      }
      if (game.multiplayer == 1) {
        city_window_close();
        ((WndBase *)((i8 *)&status_wnd.wnd))->hide();
        ((WndBase *)((i8 *)&minimap_state.wnd))->hide();
        for (view_index = 0; view_index < 8; view_index++) {
          if (map_views_block.map_view_recs[view_index].open != 0) {
            ((WndBase *)&map_views_block.map_view_recs[view_index].window)->hide();
          }
        }
      }
      historian_report_show();
      if (game.multiplayer == 1) {
        ((SurfaceWindow *)((i8 *)&status_wnd.window))->SurfaceWindow::show();
        ((SurfaceWindow *)((i8 *)&minimap_state.window))->SurfaceWindow::show();
        for (view_index = 0; view_index < 8; view_index++) {
          if (map_views_block.map_view_recs[view_index].open != 0) {
            ((SurfaceWindow *)&map_views_block.map_view_recs[view_index])->SurfaceWindow::show();
          }
        }
      }
    }
  }
  civ2 = 1;
  while( true ) {
    if (7 < civ2) {
      return;
    }
    result = civ_spaceship_launched(civ2);
    if (((result != 0) && (result = civ_spaceship_arrived(civ2), result != 0)) &&
       ((civs[civ2].spaceship_flags & SHIP_ARRIVED) == 0)) break;
    civ2++;
  }
  civs[civ2].spaceship_flags = civs[civ2].spaceship_flags | SHIP_ARRIVED;
  if ((game.game_flags & GAME_WON) == 0) {
    civs[civ2].spaceship_flags = civs[civ2].spaceship_flags | SHIP_WON;
    turn_flags.spaceship_winner_civ = civ2;
    if (human_civ[0] == civ2) {
      game_over_reason = 1;
    }
    else {
      game_over_reason = 2;
    }
    if (2 < game.multiplayer) {
      adjective = civ_get_adjective(civ2);
      text_set_string_arg(0,(const char *)adjective);
      if ((human_civ[0] == civ2) ||
         ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) {
        net_send_text_args(6,0xff,1,0,0,0);
        net_send_message(NM_END_SEQUENCE,0xff,(uint)game.game_flags,2,turn_flags.spaceship_winner_civ,0,0,0,0,0);
      }
      else {
        net_send_message(NM_END_SEQUENCE,net.net_slots[net.net_civ_slots[civ2]].id,
                           (uint)game.game_flags,1,turn_flags.spaceship_winner_civ,0,0,0,0,0);
        for (other_civ = 1; other_civ < 8; other_civ++) {
          if ((((1 << ((byte)other_civ & 0x1f) &
                (uint)(game.civs_alive & game.human_players) & 1 << ((byte)other_civ & 0x1f)) != 0)
              && (human_civ[0] != other_civ)) && (other_civ != civ2)) {
            net_send_text_args(6,net.net_slots[net.net_civ_slots[other_civ]].id,1,
                               0,0,0);
            net_send_message(NM_END_SEQUENCE,net.net_slots[net.net_civ_slots[other_civ]].id
                               ,(uint)game.game_flags,2,turn_flags.spaceship_winner_civ,0,0,0,0,0);
          }
        }
      }
    }
  }
  if ((autoplay.active == 0) && ((human_civ[0] != civ2 || (game_over_reason == 0)))) {
    adjective = civ_get_adjective(civ2);
    text_set_string_arg(0,(const char *)adjective);
    if ((2 < game.multiplayer) && (game_over_reason == 0)) {
      net_send_text_args(6,0xff,1,0,0,0);
    }
    popup_game_with_sprite((const char *)&str_eaglehaslanded,(iptr)&popup_info_icon_sprite,0);
  }
  game.game_flags = game.game_flags | GAME_WON;
  return;
}

/* 00488937 auto */
void __cdecl game_autosave(iptr civ)

{
  char *leader_name;
  i8 backup_path[80];
  
  if (human_civ[0] == civ) {
    save_set_file_ext(0);
    text_clear();
    leader_name = civ_leader_name(civ);
    text_append(leader_name);
    textbuf.plus2[0] = 0;
    str_trim((char *)&textbuf.buf);
    text_append((const char *)&str_empty_99);
    text_append_label(0x5c);
    crt_strcpy(backup_path,((i8 *)&textbuf.buf));
    str_append((char *)backup_path,(const char *)&str_2);
    text_append((const char *)&str_empty_100);
    text_append((const char *)&grey_palette_block.save_file_ext_bare);
    str_append((char *)backup_path,(const char *)&str_empty_101);
    str_append((char *)backup_path,(const char *)&grey_palette_block.save_file_ext_bare);
    if ((game.turn & 4U) == 0) {
      crt_remove(backup_path);
      crt_rename(((i8 *)&textbuf.buf),backup_path);
    }
    save_write_game((const char *)&textbuf.buf,0);
  }
  return;
}

/* 00489be2 auto */
void __cdecl tutorial_settler_hints(iptr unit)

{
  u8 food_yield;
  u8 shield_yield;
  char dy;
  bool good_site;
  byte terrain_id;
  int x;
  int y;
  iptr civ;
  int city;
  iptr x2;
  int value;
  uint improvements2;
  byte *tile;
  uptr terrain;
  iptr dir;
  
  if (turn_flags.tutorial_hint_shown_this_turn == 0) {
    x = (int)units[unit].x;
    y = (int)units[unit].y;
    civ = (iptr)units[unit].owner;
    terrain_id = tile_terrain(x,y);
    terrain = (uptr)terrain_id;
    food_yield = terrain_types[terrain].food;
    shield_yield = terrain_types[terrain].shields;
    city = tile_owner_or_fertility(x,y);
    good_site = 9 < city && 1 < (int)(char)food_yield + (int)(char)shield_yield;
    for (dir = 0; dir < 8; dir++) {
      x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
      dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
      value = map_in_bounds(x2,dy + y);
      if ((value != 0) && (value = tile_owner_or_fertility(x2,dy + y), city < value)) {
        good_site = false;
      }
    }
    if (good_site) {
      mapview_center_if_moved(x,y);
      popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_buildcity,unit);
      turn_flags.tutorial_hint_shown_this_turn = 1;
    }
    else if (((int)game.turn - (int)civs[civ].last_city_turn < 0x19) ||
            (5 < civs[civ].num_cities)) {
      city = city_find_nearest(x,y,civ,0xffffffff,-1);
      if ((advisor_window_block.nearest_city_dist == 1) ||
         ((advisor_window_block.nearest_city_dist == 2 && (value = tile_resource(x,y), value != 0)))) {
        text_set_string_arg(0,(const char *)&cities[city].name);
        improvements2 = tile_improvements(x,y);
        if ((terrain == TERRAIN_HILLS) && ((improvements2 & TILE_FARMLAND) == 0)) {
          mapview_center_if_moved(x,y);
          popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_mining,unit);
          turn_flags.tutorial_hint_shown_this_turn = 1;
        }
        if (((terrain == TERRAIN_PLAINS) || ((terrain == TERRAIN_GRASSLAND && (civs[civ].government == GOV_MONARCHY)))) &&
           (((improvements2 & TILE_FARMLAND) == 0 && (city = tile_has_irrigation_source(x,y), city != 0)))) {
          mapview_center_if_moved(x,y);
          popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_irrigate,unit);
          turn_flags.tutorial_hint_shown_this_turn = 1;
        }
        else if (((((uint)civs[civ].units_active[UNIT_SETTLERS] + (uint)civs[civ].units_active[UNIT_ENGINEERS] < 2) &&
                  ((improvements2 & TILE_ROAD) == 0)) && (terrain_types[terrain].move_cost == 1)) &&
                (tile = (byte *)tile_at(x,y), (*tile & 0x80) == 0)) {
          mapview_center_if_moved(x,y);
          popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_road,unit);
          turn_flags.tutorial_hint_shown_this_turn = 1;
        }
      }
    }
    else {
      popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_expand1,unit);
      civs[civ].last_city_turn = game.turn;
      turn_flags.tutorial_hint_shown_this_turn = 1;
    }
  }
  return;
}

/* 0048a004 auto */
i32 __cdecl game_run_active_unit(void)

{
  bool resumed_move;
  bool had_unit;
  i16 start_unit_count;
  iptr unit;
  iptr selectable;
  int unit2;
  int result;
  bool show_hint;
  int spin_count;
  i32 waited;
  
  start_unit_count = game.num_units;
  waited = 0;
  had_unit = false;
  resumed_move = false;
  blink.unit_active = 1;
  unit = (iptr)game.active_unit;
  if (((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && ((units[unit].flags & UNITF_AUTOMATED) == 0)) &&
     (units[unit].orders == ORDER_NONE)) {
    if ((unit_types[units[unit].type].role == ROLE_SETTLE) &&
       (selectable = unit_is_selectable(unit), selectable != 0)) {
      tutorial_settler_hints((int)game.active_unit);
    }
    else if ((unit_types[units[unit].type].attack != '\0') && (4 < units[unit].damage)) {
      show_hint = false;
      unit2 = unit_max_hp(unit);
      if ((int)(uint)units[unit].damage < unit2 / 3) {
        if ((game.shown_event_flags & HINT_HEAVY_DAMAGE) == 0) {
          game.shown_event_flags = game.shown_event_flags | HINT_HEAVY_DAMAGE;
          show_hint = true;
        }
      }
      else {
        show_hint = (game.shown_event_flags & HINT_DAMAGE) == 0;
        if (show_hint) {
          game.shown_event_flags = game.shown_event_flags | (HINT_DAMAGE | HINT_HEAVY_DAMAGE);
        }
      }
      if ((show_hint) && (autoplay.active == 0)) {
        popup_text_with_unit_noflags((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_damage,unit);
      }
    }
  }
  unit2 = (int)game.active_unit;
  spin_count = 0;
  while (((result = unit_is_selectable((int)game.active_unit), result != 0 &&
          ((((blink.unit_active != '\0' || (unit_move.unanimated_state == 2)) && (player_turn_active != '\0')) &&
           ((turn_loop.active != '\0' && (game.num_units == start_unit_count)))))) &&
         ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)game.human_players) != 0))) {
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      net.process_messages(1,0);
    }
    if (unit_move.unanimated_state == 2) {
      blink.unit_active = 1;
      unit_move.unanimated_state = '\0';
      resumed_move = true;
    }
    if ((resumed_move) && (units[game.active_unit].orders != ORDER_GOTO)) {
      blink.unit_active = '\0';
      resumed_move = false;
    }
    had_unit = true;
    snd_sync_music_option();
    if ((units[unit2].flags & UNITF_AUTOMATED) == 0) {
      result = unit_execute_orders(unit2);
      if (result == 0) {
        waited = 1;
        win_pump_one_message();
      }
    }
    else {
      unit_continue_active_orders();
      spin_count++;
      if (0x14 < spin_count) {
        unit_exhaust_moves(unit2);
      }
    }
  }
  mapviews_redraw_cursor();
  blink.unit_active = 0;
  if (!had_unit) {
    game.active_unit = -1;
  }
  return waited;
}

/* 0048a374 auto */
void __cdecl game_wait_for_player_input(void)

{
  i16 start_unit_count;
  
  start_unit_count = game.num_units;
  blink.unit_active = 1;
  while ((((blink.unit_active != '\0' && (player_turn_active != '\0')) && (turn_loop.active != '\0')) &&
         ((moving_units_mode == 0 && (game.num_units == start_unit_count))))) {
    snd_sync_music_option();
    win_pump_one_message();
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
  }
  blink.unit_active = 0;
  return;
}

/* 0048a416 auto */
void __cdecl game_player_move_loop(void)

{
  short last_unit_count;
  iptr selectable;
  uint run_result;
  int unit;
  iptr city;
  int slot;
  iptr unit2;
  uint waited;
  int wait_input;
  
  waited = 0;
  last_unit_count = -1;
  game.active_unit = -1;
  ui_focus_active_unit(0);
  player_turn_active = 1;
  turn_flags.tutorial_hint_shown_this_turn = 0;
  if ((2 < game.multiplayer) && (net.host_flag == '\0')) {
    for (slot = 0; slot < 8; slot++) {
      if (((net_meet_pending_flags[slot] != 0) &&
          (net_meet_pending_flags[slot] = 0,
          ((uint)game.civs_alive & 1 << ((i8)net_meet_pending_flags[slot] & 0x1f)) != 0)) &&
         (((uint)game.human_players & 1 << ((i8)net_meet_pending_flags[slot] & 0x1f)) == 0)) {
        diplo_ai_meet_player(net_meet_pending.ai_civ[slot],
                           net_meet_pending.civ[slot],
                           net_meet_pending.x[slot],
                           net_meet_pending.y[slot],0);
      }
    }
  }
  do {
    if (2 < game.multiplayer) {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
      net.process_messages(1,0);
    }
    if (moving_units_mode == 1) {
      unit_is_selectable((int)game.active_unit);
      if (((game.active_unit < 0) || (game.num_units <= game.active_unit)) ||
         ((units[game.active_unit].id == 0 ||
          ((selectable = unit_is_selectable((int)game.active_unit), selectable == 0 ||
           (game.num_units != last_unit_count)))))) {
        unit_activate_next(0);
      }
      else if (map_views_block.unit_focus_done == '\0') {
        unit = (int)game.active_unit;
        map_cursor.x[0] = units[unit].x;
        map_cursor.y[0] = units[unit].y;
        if ((unit_move.unanimated_state == '\0') || (units[unit].orders != ORDER_GOTO)) {
          status_redraw(1);
          mapviews_ensure_visible((int)units[unit].x,(int)units[unit].y,(int)units[unit].owner);
          menu_update_states();
        }
      }
    }
    if ((((2 < game.multiplayer) && (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0)) && (mp_timer.handle == 0)) &&
       (0 < mp_timer.seconds_left)) {
      dbg_log((const char *)&str_primary_cpp_human_turn_timer_seems_to_be);
      mp_turn_timer_resume(0);
    }
    last_unit_count = game.num_units;
    unit_move.unanimated_state = '\0';
    map_views_block.unit_focus_done = '\0';
    wait_input = 1;
    if (game.active_unit < 0) {
      game.transient_flags = game.transient_flags | TRANSIENT_END_OF_TURN;
      if (moving_units_mode == 1) {
        snd_play_effect(SND_ENDOTURN,1,0,0);
      }
      ui_redraw_all_maps(1);
      if (((waited != 0) && (((*((char *)&game.game_options + 1)) & (OPT_ALWAYS_WAIT >> 8)) == 0)) && ((game.transient_flags & TRANSIENT_CITY_WINDOW_OPENED) == 0)) {
        player_turn_active = '\0';  /* the turn ends */
        wait_input = 0;
      }
    }
    if (wait_input != 0) {
      blink.unit_active = 1;
      if (game.active_unit < 0) {
        game.transient_flags = game.transient_flags & ~TRANSIENT_CITY_WINDOW_OPENED;
        game_wait_for_player_input();
      }
      else {
        game.transient_flags = game.transient_flags & ~(TRANSIENT_END_OF_TURN | TRANSIENT_CITY_WINDOW_OPENED);
        run_result = game_run_active_unit();
        waited = waited | run_result;
      }
      unit = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      if (unit == -1) {
        game.transient_flags = game.transient_flags & ~TRANSIENT_CITY_WINDOW_OPENED;
      }
      if (turn_loop.active == '\0') {
        return;
      }
      if ((game.transient_flags & TRANSIENT_CITY_WINDOW_OPENED) != 0) {
        waited = 0;
      }
      if (blink.unit_phase != '\0') {
        mapviews_redraw_cursor();
      }
      blink.unit_active = 0;
    }
    if (((turn_loop.active == '\0') || (player_turn_active == '\0')) ||
       ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)game.human_players) == 0)) {
      if ((city_wnd.closed == 0) &&
         (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
         cities[city].owner != (*(i8 *)&human_civ))) {
        city_window_close();
      }
      input_flush_events();
      player_turn_active = 0;
      if (game.multiplayer != '\0') {
        for (unit2 = 0; unit2 < game.num_units; unit2++) {
          if ((units[unit2].id != 0) && (units[unit2].owner == game.turn_civ)) {
            unit_exhaust_moves(unit2);
          }
        }
      }
      return;
    }
  } while( true );
}

/* 0048a92d auto */
void __cdecl game_calc_human_wonder_range(void)

{
  byte count;
  int civ;
  iptr group;
  
  for (group = 0; group < 4; group++) {
    (((i8 *)&map_views_block.human_wonders_by_group_max))[group] = 0;
    (((i8 *)&map_views_block.human_wonders_by_group_min))[group] = 99;
    for (civ = 1; civ < 8; civ++) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
        count = civs[civ].wonders_by_group[group];
        if (count <= (byte)(((i8 *)&map_views_block.human_wonders_by_group_max))[group]) {
          count = (((i8 *)&map_views_block.human_wonders_by_group_max))[group];
        }
        (((i8 *)&map_views_block.human_wonders_by_group_max))[group] = count;
        count = civs[civ].wonders_by_group[group];
        if ((byte)(((i8 *)&map_views_block.human_wonders_by_group_min))[group] <= count) {
          count = (((i8 *)&map_views_block.human_wonders_by_group_min))[group];
        }
        (((i8 *)&map_views_block.human_wonders_by_group_min))[group] = count;
      }
    }
  }
  return;
}

/* 0048aa24: written by hand in src/game/retvals.cpp */

/* 0048aedc auto */
i32 __cdecl game_check_retirement(void)

{
  char *name_str;
  i32 game_ended;
  
  game_ended = 0;
  if (((game.game_flags & GAME_WON) == 0) && (game_over_reason == 0)) {
    name_str = (char *)civ_leader_title(human_civ[0]);
    text_set_string_arg(0,name_str);
    name_str = civ_leader_name(human_civ[0]);
    text_set_string_arg(1,name_str);
    if (((game.game_flags & GAME_SCENARIO) == 0) || (scenario.max_turns == 0)) {
      if (extra_rule(X_RETIREMENT) ?
          /* X50: the years set; a warning the turn the year is passed */
          (extras_year_reached(extras_retirement_warning_year()) &&
           !extras_year_reached(extras_retirement_year())) : (game.year == 2000)) {
        if (2 < game.multiplayer) {
          net_send_text_args(9,0xff,0,0,0,0);
        }
        popup_game_with_sprite((const char *)&str_planretire,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      }
      else if (extra_rule(X_RETIREMENT) ? extras_year_reached(extras_retirement_year()) :
               (game.year == 0x7e4)) {
        if (2 < game.multiplayer) {
          net_send_text_args(10,0xff,0,0,0,0);
        }
        popup_game_with_sprite((const char *)&str_doretire,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
        game.game_flags = game.game_flags | GAME_WON;
        game_over_reason = 5;
        if (2 < game.multiplayer) {
          net_send_message(NM_END_SEQUENCE,0xff,(uint)game.game_flags,5,0,0,0,0,0,0);
        }
        game_ended = 1;
      }
    }
    else if (scenario.max_turns - 5 == (int)game.turn) {
      if (2 < game.multiplayer) {
        net_send_text_args(7,0xff,0,0,0,0);
      }
      popup_game_with_sprite((const char *)&str_scenarioends,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    else if (scenario.max_turns == game.turn) {
      if (2 < game.multiplayer) {
        net_send_text_args(8,0xff,0,0,0,0);
      }
      popup_game_with_sprite((const char *)&str_scenarioend,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      game.game_flags = game.game_flags | GAME_WON;
      game_over_reason = 5;
      if (2 < game.multiplayer) {
        net_send_message(NM_END_SEQUENCE,0xff,(uint)game.game_flags,5,0,0,0,0,0,0);
      }
      game_ended = 1;
    }
  }
  return game_ended;
}

/* 0048b165 auto */
uint __cdecl game_handle_game_over(void)

{
  int choice;
  uint quit;
  
  quit = 0;
  switch(game_over_reason) {
  case 1:
  case 2:
    if ((turn_flags.spaceship_winner_civ == 0) || (human_civ[0] == turn_flags.spaceship_winner_civ)) {
      endgame_spaceship_arrival(human_civ[0]);
    }
    else {
      endgame_spaceship_arrival(-turn_flags.spaceship_winner_civ);
    }
    break;
  case 3:
    destroyed_civs_show(human_civ[0]);
    break;
  case 4:
    endgame_defeat_movie(human_civ[0]);
    break;
  case 5: ;
  }
  powergraph_show();
  if ((game.game_flags & GAME_OVER_SCORED) == 0) {
    civ_score_window_show(human_civ[0]);
    civ_rating_window_show(human_civ[0]);
    hall_of_fame_add_entry(human_civ[0]);
  }
  if (game.multiplayer == '\0') {
    if ((((game_over_reason == 1) || (game_over_reason == 2)) || (game_over_reason == 4)) ||
       (((game_over_reason == 5 &&
          (extra_rule(X_RETIREMENT) ?  /* X50: retired (not a scenario's end) */
           (((game.game_flags & GAME_SCENARIO) == 0 || (scenario.max_turns == 0)) &&
            extras_retirement_year() <= game.year) : (game.year == 0x7e4))) ||
         (game_over_reason == 3)))) {
      if (game_over_reason == 4) {
        quit = 1;
      }
      else {
        choice = popup_game_with_sprite((const char *)&str_keepplaying,(iptr)&popup_info_icon_sprite,0);
        quit = (uint)(choice == 0);
      }
      game.game_flags = game.game_flags | GAME_OVER_SCORED;
    }
  }
  else {
    quit = 1;
  }
  game_over_reason = 0;
  return quit;
}

/* Part of game_play_loop (0048b340): the casualties of the player's units
   since its last turn, if any (not in autoplay). */
static void play_loop_casualty_popup(Dialog *dialog2)
{
  char *name_str;
  int casualties;

  casualties = net.casualty_counts[(*(i8 *)&human_civ)] -
             net.casualty_reported_counts[(*(i8 *)&human_civ)];
  if (autoplay.active == 0) {
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    name_str = (char *)civ_leader_title((*(i8 *)&human_civ));
    text_set_string_arg(0,name_str);
    name_str = (civ_leader_name((*(i8 *)&human_civ)));
    text_set_string_arg(1,name_str);
    if (0 < casualties) {
      net.casualty_reported_counts[(*(i8 *)&human_civ)] =
           net.casualty_counts[(*(i8 *)&human_civ)];
      text_set_number_arg(0,casualties);
      if (casualties == 1) {
        dialog2->load_text_noflags((const char *)&str_game_7,(const char *)&str_casualty);
      }
      else {
        dialog2->load_text_noflags((const char *)&str_game_7,(const char *)&str_casualties);
      }
      dialog2->add_sprite((iptr)&popup_info_icon_sprite,0,0);
      dialog2->set_image_zoom(0);
      input_flush_events();
      snd_play_effect(SND_NEWONDER,0,0,0);
      dialog2->show_modal(0);
      if (dialog2->result_extra != 0) {
        casualty_timeline_show((*(i8 *)&human_civ));
      }
    }
  }
}

/* Part of game_play_loop (0048b340): the start of the turn of a human
   player: the password, in hotseat the turn popup and the cursor of the
   player, in play-by-email the game saved after the first player (then the
   loop ends: returns 1), the views redrawn and shown again. */
static int play_loop_human_turn_start(uptr civ,bool *pbem_saved,int *views_hidden)
{
  char *name_str;
  iptr prev_human_civ;
  iptr city;
  int view_index;

  game.player_civ = (byte)civ;
  if (game.multiplayer != '\0') {
    civ_check_password(civ);
  }
  if (game.multiplayer == 1) {
    name_str = civ_leader_name(civ);
    text_set_string_arg(0,name_str);
    name_str = civ_name_plural(civ);
    text_set_string_arg(1,name_str);
    popup_game_with_sprite((const char *)&str_hotseatturn,(iptr)&popup_info_icon_sprite,0);
    civ_view.saved_cursor_x[(*(i8 *)&human_civ)] = map_cursor.x[0];
    civ_view.saved_cursor_y[(*(i8 *)&human_civ)] = map_cursor.y[0];
    if (civ_view.intro_shown[civ] == 1) {
      map_cursor.x[0] = civ_view.saved_cursor_x[civ];
      map_cursor.y[0] = civ_view.saved_cursor_y[civ];
      map_views_block.map_view_center_x[0] = map_cursor.x[0];
      map_views_block.mapview_center_y[0] = map_cursor.y[0];
    }
    else {
      civ_intro_popup(civ);
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && ((int)cities[city].owner == civ)) {
          map_cursor.x[0] = cities[city].x;
          map_cursor.y[0] = cities[city].y;
          map_views_block.map_view_center_x[0] = map_cursor.x[0];
          map_views_block.mapview_center_y[0] = map_cursor.y[0];
          break;
        }
      }
      civ_view.intro_shown[civ] = 1;
    }
  }
  prev_human_civ = (*(i8 *)&human_civ);
  (*(i8 *)&human_civ) = civ;
  if (game.multiplayer == 2) {
    if (*pbem_saved) {
      game_save_dialog(0);
      if ((((i8 *)&civ_saved_cursor_x_block.civ_email_addresses))[civ * 0x20] == '\0') {
        name_str = civ_leader_name(civ);
        text_set_string_arg(0,name_str);
        popup_game((const char *)&str_emaildone2);
      }
      else {
        name_str = civ_leader_name(civ);
        text_set_string_arg(0,name_str);
        text_set_string_arg(1,(const char *)(((i8 *)&civ_saved_cursor_x_block.civ_email_addresses) + civ * 0x20));
        popup_game((const char *)&str_emaildone1);
      }
      turn_loop.active = '\0';
      return 1;
    }
    *pbem_saved = true;
  }
  if (((prev_human_civ & 0xffU) != civ) || ((*(byte *)&turn_loop.resume_civ) != 0)) {
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
      mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
      map_views_block.main_view_timer_cache_i32 = 0xffffffff;
    }
    ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
    map_redraw_all_views((*(i8 *)&human_civ),1);
  }
  status_draw_info(1);
  status_draw_civ_color(civ);
  if (*views_hidden != 0) {
    *views_hidden = 0;
    ((SurfaceWindow *)((i8 *)&status_wnd.window))->SurfaceWindow::show();
    ((SurfaceWindow *)((i8 *)&minimap_state.window))->SurfaceWindow::show();
    for (view_index = 0; view_index < 8; view_index++) {
      if (map_views_block.map_view_recs[view_index].open != 0) {
        ((SurfaceWindow *)&map_views_block.map_view_recs[view_index])->SurfaceWindow::show()
        ;
      }
    }
  }
  return 0;
}

/* Part of game_play_loop (0048b340): in hotseat, between two players: the
   city window closed, the status, minimap and map windows hidden. */
static void play_loop_hide_views(void)
{
  int view_index;

  city_window_close();
  ((WndBase *)((i8 *)&status_wnd.wnd))->hide();
  ((WndBase *)((i8 *)&minimap_state.wnd))->hide();
  for (view_index = 0; view_index < 8; view_index++) {
    if (map_views_block.map_view_recs[view_index].open != 0) {
      ((WndBase *)&map_views_block.map_view_recs[view_index].window)->hide()
      ;
    }
  }
}

/* 0048b340 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl game_play_loop(void)

{
  bool pbem_saved;
  bool human_moved;
  int retired;
  uptr civ;
  byte saved_human_civ;
  Dialog dialog2;
  int views_hidden;
  int human_count;
  int quit;
  
  views_hidden = 0;
  pbem_saved = false;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  menu_update_states();
  if (game.multiplayer == 1) {
    net.hotseat_human_players = game.human_players;
  }
  while( true ) {
    human_moved = false;
    game_calc_human_wonder_range();
    if ((*(byte *)&turn_loop.resume_civ) == 0) {
      game_advance_turn(0xffffffff);
    }
    retired = game_check_retirement();
    if ((retired != 0) || (game_over_reason != 0)) {
      human_count = bits_count8((uint)game.human_players);
      quit = game_handle_game_over();
      if ((game.multiplayer == 1) &&
         ((~(1 << ((byte)(*(i8 *)&human_civ) & 0x1f)) & (uint)game.human_players) != 0)) {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
      }
      if (((quit != 0) && (game.multiplayer == '\0')) ||
         ((quit != 0 && ((game.multiplayer == 1 && (human_count < 2)))))) break;
      quit = 0;
    }
    if ((game.multiplayer == 1) && (game.human_players == '\0')) {
      popup_game_with_sprite((const char *)&str_hothumansdead,(iptr)&popup_info_icon_sprite,0);
      break;
    }
    if ((scenario_events_loaded != 0) && (((*(byte *)&turn_loop.resume_civ) == 0 || (turn_resume_new_game != 0)))) {
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_turn((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_turn_interval((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_random_turn((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_received_tech();
    }
    for (civ = 0; (turn_loop.active != '\0' && ((int)civ < 8)); civ++) {
      if (((int)(uint)(*(byte *)&turn_loop.resume_civ) <= (int)civ) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) {
        game.turn_civ = (byte)civ;
        if (game.multiplayer == 1) {
          net.hotseat_human_players = game.human_players;
          net.mp_turn_civ = civ;
          views_hidden = 1;
          play_loop_hide_views();
        }
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
          if (game.multiplayer == 1) {
            dialog2.load_game_section((const char *)&str_hotseataiturn,0x2000008);
            dialog2.show_modal(0);
          }
        }
        else {
          if (play_loop_human_turn_start(civ,&pbem_saved,&views_hidden) != 0) {
            mp_turn_timer_stop(0);
            return;
          }
        }
        status_draw_civ_color(civ);
        if ((game.multiplayer == 1) &&
           ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
          game.human_players = '\0';
          saved_human_civ = (byte)(*(i8 *)&human_civ);
          (*(i8 *)&human_civ) = -1;
        }
        if (((*(byte *)&turn_loop.resume_civ) == 0) || (turn_resume_new_game != 0)) {
          if (((*(byte *)&turn_loop.resume_civ) != 0) && (turn_resume_new_game != 0)) {
            ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
            map_redraw_all_views((*(i8 *)&human_civ),1);
            wnd_flush_paint();
          }
          play_loop_casualty_popup(&dialog2);
          civ_process_turn(civ);
        }
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
          if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
             (ai_process_civ_units(), game.multiplayer == 1)) {
            dialog2.close();
            dialog2.init_defaults();
            (*(i8 *)&human_civ) = (iptr)saved_human_civ;
          }
        }
        else {
          if (game.multiplayer == 1) {
            game.human_players = game.human_players & (byte)(1 << ((byte)civ & 0x1f));
          }
          game_turn_start_popups();
          if ((game.multiplayer == 1) && (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0)) {
            mp_turn_timer_start(1);
            game_player_move_loop();
            mp_turn_timer_stop(1);
          }
          else {
            game_player_move_loop();
          }
          human_moved = true;
        }
        if (game.multiplayer == 1) {
          game.human_players = net.hotseat_human_players;
        }
        saved_turn.civ = 0;
        (*(byte *)&turn_loop.resume_civ) = 0;
        saved_turn.resume_new_game = 0;
        turn_resume_new_game = 0;
        game.transient_flags = game.transient_flags & ~TRANSIENT_END_OF_TURN;
        if (turn_loop.active == '\0') {
          mp_turn_timer_stop(0);
          return;
        }
        if (game_over_reason != 0) {
          human_count = bits_count8((uint)game.human_players);
          quit = game_handle_game_over();
          if ((game.multiplayer == 1) &&
             ((~(1 << ((byte)(*(i8 *)&human_civ) & 0x1f)) & (uint)game.human_players) != 0)) {
            game.human_players = game.human_players & ~(byte)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
          }
          if (((quit != 0) && (game.multiplayer == '\0')) ||
             ((quit != 0 && ((game.multiplayer == 1 && (human_count < 2)))))) {
            mp_turn_timer_stop(0);
            return;
          }
          quit = 0;
        }
      }
    }
    if ((!human_moved) && (game.multiplayer == '\0')) {
      game.turn_civ = (byte)(*(i8 *)&human_civ);
      game_player_move_loop();
      if ((game_over_reason != 0) && (quit = game_handle_game_over(), quit != 0)) break;
    }
    game.human_players = game.civs_alive & game.human_players;
    if (turn_loop.active == '\0') break;
  }
  mp_turn_timer_stop(0);
  return;
}

/* 0048bfec auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl game_mp_client_turn_loop(void)

{
  byte resume_civ;
  SHORT key_state;
  char *name_str;
  int result;
  i32 ret;
  iptr slot_civ;
  int civ;
  iptr unit;
  Dialog dialog2;
  i8 still_playing;
  int found_unit;
  int casualties;
  i32 first_turn;
  
  first_turn = 1;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  still_playing = 1;
  slot_civ = net.slot_civs[0];
  name_str = civ_name_plural(net.slot_civs[0]);
  dbg_logf_str_int((const char *)&str_server_s_d,(const char *)name_str,slot_civ);
  for (civ = 1; civ < 8; civ++) {
    if (((net.net_slots[net.net_civ_slots[civ]].id != 0) &&
        ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      result = civ;
      name_str = civ_name_plural(civ);
      dbg_logf_str_int((const char *)&str_client_s_d,(const char *)name_str,result);
    }
  }
  net_reset_error_counts();
  human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
  (*(byte *)&turn_loop.resume_civ) = (*(byte *)&saved_turn.civ);
  turn_resume_new_game = (int)saved_turn.resume_new_game;
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  menu_update_states();
  game.player_civ = (byte)human_civ[0];
  game.turn_civ = (byte)human_civ[0];
  ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
  found_unit = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit].x,(int)units[unit].y);
      found_unit++;
      break;
    }
  }
  if (found_unit == 0) {
    for (unit = 0; unit < game.num_cities; unit++) {
      if ((cities[unit].id != 0) && (cities[unit].owner == human_civ[0])) {
        ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)cities[unit].x,(int)cities[unit].y
                  );
        break;
      }
    }
  }
  status_draw_info(1);
  status_draw_civ_color(human_civ[0]);
  ui_redraw_all_maps(1);
  resume_civ = (*(byte *)&turn_loop.resume_civ);
  if ((1 << ((byte)human_civ[0] & 0x1f) & (uint)game.human_players) == 0) {
    net_heartbeat_send(net.slot_civs[0]);
    dlg_style.pending_idle_callback = net_client_hot_wait_idle;
    popup_game_sprite_flags((const char *)&str_clienthotwait,0x2000000,(iptr)&popup_info_icon_sprite,0);
    resume_civ = (*(byte *)&turn_loop.resume_civ);
  }
  do {
    (*(byte *)&turn_loop.resume_civ) = resume_civ;
    if ((uint)(*(byte *)&turn_loop.resume_civ) != human_civ[0]) {
      for (unit = 0; unit < game.num_units; unit++) {
        if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
          unit_exhaust_moves(unit);
        }
      }
    }
    net_heartbeat_send(net.slot_civs[0]);
    while ((((net.nm_start_civ_count == 0 && (net.host_handoff_stage == '\0')) && (net.nm_end_sequence_count == 0)) &&
           ((net.player_has_quit == '\0' && (turn_loop.active != '\0'))))) {
      net.process_messages(1,0);
      net_heartbeat_check(net.slot_civs[0]);
      result = net_check_lost_connections();
      if (result != 0) {
        ret = 0;
        return ret;
      }
    }
    if (net.host_handoff_stage != '\0') {
      result = net_client_transfer_server();
      if (result == 0) {
        ret = 0;
        return ret;
      }
      if (net.host_flag != '\0') {
        ret = 1;
        return ret;
      }
      if (((net.nm_start_civ_count == 0) && (net.nm_end_sequence_count == 0)) &&
         ((net.player_has_quit == '\0' && (resume_civ = (*(byte *)&turn_loop.resume_civ), turn_loop.active != '\0'))))
      continue;
    }
    if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
      net.nm_start_civ_count = 0;
      human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
      game.player_civ = (byte)human_civ[0];
      game.turn_civ = (byte)human_civ[0];
      if ((uint)(*(byte *)&turn_loop.resume_civ) != human_civ[0]) {
        units_reset_for_turn(human_civ[0]);
      }
      if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
        mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
        map_views_block.main_view_timer_cache_i32 = 0xffffffff;
      }
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      map_redraw_all_views(human_civ[0],1);
      status_draw_info(1);
      status_draw_civ_color(human_civ[0]);
      casualties = net.casualty_counts[human_civ[0]] -
                 net.casualty_reported_counts[human_civ[0]];
      if (autoplay.active == 0) {
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        name_str = (char *)civ_leader_title(human_civ[0]);
        text_set_string_arg(0,name_str);
        name_str = civ_leader_name(human_civ[0]);
        text_set_string_arg(1,name_str);
        if (casualties < 1) {
          dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_ourturntomove);
          dialog2.add_sprite((iptr)&popup_info_icon_sprite,0,0);
          dialog2.set_image_zoom(0);
          input_flush_events();
          snd_play_effect(SND_NEWONDER,0,0,0);
          dialog2.show_modal(0);
        }
        else {
          net.casualty_reported_counts[human_civ[0]] =
               net.casualty_counts[human_civ[0]];
          text_set_number_arg(0,casualties);
          if (casualties == 1) {
            dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_casualty_2);
          }
          else {
            dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_casualties_2);
          }
          dialog2.add_sprite((iptr)&popup_info_icon_sprite,0,0);
          dialog2.set_image_zoom(0);
          input_flush_events();
          snd_play_effect(SND_NEWONDER,0,0,0);
          dialog2.show_modal(0);
          if (dialog2.result_extra != 0) {
            casualty_timeline_show(human_civ[0]);
          }
        }
      }
      if (autoplay.active == 0) {
        if ((*(byte *)&turn_loop.resume_civ) == 0) {
          civ_check_throne_room_upgrade();
        }
        if (((*(byte *)&turn_loop.resume_civ) == 0) || (turn_resume_new_game != 0)) {
          civ_process_turn(human_civ[0]);
          net_send_state_diff(0xff,2,0,0,0);
        }
        game_turn_start_popups();
        if ((civ_view.intro_shown[human_civ[0]] == 0) && (autoplay.active == 0)) {
          civ_intro_popup(human_civ[0]);
          civ_view.intro_shown[human_civ[0]] = 1;
        }
        if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
          mp_turn_timer_start(1);
        }
        game_player_move_loop();
      }
      else {
        map_views_block.saved_human_players = (uint)game.human_players;
        game.human_players = '\0';
        if (((*(byte *)&turn_loop.resume_civ) == 0) || (turn_resume_new_game != 0)) {
          civ_process_turn(human_civ[0]);
        }
        if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
          mp_turn_timer_start(1);
        }
        ai_process_civ_units();
        game.human_players = (*(u8 *)&map_views_block.saved_human_players);
        key_state = GetAsyncKeyState(0x1b);
        if (((int)key_state & 0x8001U) != 0) {
          autoplay.turns_left = 1;
        }
      }
    }
    else {
      still_playing = 0;
    }
    mp_turn_timer_stop(1);
    status_redraw(1);
    ui_pump_messages();
    if (game_over_reason != 0) {
      turn_loop.active = '\0';
    }
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_TURN_FINISHED,0,human_civ[0],0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    (*(byte *)&saved_turn.civ) = 0;
    (*(byte *)&turn_loop.resume_civ) = 0;
    saved_turn.resume_new_game = 0;
    turn_resume_new_game = 0;
    first_turn = 0;
    resume_civ = 0;
    if (turn_loop.active == '\0') {
      net_send_message(NM_QUIT_REQUEST,0,human_civ[0],0,0,0,0,0,0,0);
      XD_FlushSendBuffer(60000);
      net.shutdown(1);
      if (game_over_reason != 0) {
        game_handle_game_over();
      }
      ret = 0;
      return ret;
    }
  } while( true );
}

/* Part of game_mp_host_turn_loop (0048c9f3): the turn of a human player on
   another machine: tells the client to play and serves its messages until
   its turn ends. */
static void mp_host_remote_turn(uptr civ)
{
  int result;
  RECT rect2;

  result = net.net_civ_slots[civ];
  net.mp_turn_civ = civ;
  menu_update_states();
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
  rect2.left = map_views_block.main_view_frame_left;
  rect2.top = map_views_block.main_view_frame_top;
  rect2.right = map_views_block.main_view_frame_right;
  rect2.bottom = map_views_block.main_view_client_top;
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&rect2);
  net.nm_start_civ_count = 0;
  net.civ_turn_ended[civ] = 0;
  net_send_message(NM_START_WHOSE_TURN,0xff,civ,0,0,0,0,0,0,0);
  map_redraw_all_views(human_civ[0],1);
  status_draw_civ_color(civ);
  net_send_message(NM_START_CIVILIZATION,net.net_slots[result].id,0,0,0,0,0,0,0,0);
  if (net.civ_new_player_flags[civ] != 0) {
    net.civ_new_player_flags[civ] = 0;
    crt_strcpy(((i8 *)&text_args.strings),
                 ((i8 *)&net.slot_player_names) + net.net_civ_slots[civ] * 0x54);
    net_send_text_args(0x3f,0xff,1,0,
                       net.net_slots[net.net_civ_slots[civ]].id,0);
    popup_game_with_sprite((const char *)&str_newplayer,(iptr)&popup_info_icon_sprite,0);
  }
  net_heartbeat_send(civ);
  while ((((net.civ_turn_ended[civ] == 0 && (net.nm_end_sequence_count == 0)) &&
          (net.player_has_quit == '\0')) && (turn_loop.active != '\0'))) {
    net.process_messages(1,0);
    net_heartbeat_check(civ);
  }
  net.nm_start_civ_count = 0;
  net.nm_end_sequence_count = 0;
}

/* Part of game_mp_host_turn_loop (0048c9f3): the turn of a civ the computer
   plays. */
static void mp_host_ai_turn(uptr civ)
{
  RECT rect2;

  net.civ_new_player_flags[civ] = 0;
  net.tile_sync_enabled = 0;
  net.mp_turn_civ = civ;
  menu_update_states();
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
  rect2.left = map_views_block.main_view_frame_left;
  rect2.top = map_views_block.main_view_frame_top;
  rect2.right = map_views_block.main_view_frame_right;
  rect2.bottom = map_views_block.main_view_client_top;
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&rect2);
  net_send_message(NM_START_WHOSE_TURN,0xff,civ,0,0,0,0,0,0,0);
  map_redraw_all_views(human_civ[0],1);
  status_draw_civ_color(civ);
  if (((*(byte *)&turn_loop.resume_civ) == 0) || (turn_resume_new_game != 0)) {
    if (((*(byte *)&turn_loop.resume_civ) != 0) && (turn_resume_new_game != 0)) {
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      wnd_flush_paint();
    }
    civ_process_turn(civ);
  }
  ai_process_civ_units();
  net.tile_sync_enabled = 1;
}

/* Part of game_mp_host_turn_loop (0048c9f3): the turn of the host's own civ:
   the turn popup (casualties), then the turn played (or autoplayed). Returns
   1 when the game loop ended. */
static int mp_host_own_turn(uptr civ,Dialog *dialog2,i32 force_process)
{
  SHORT key_state;
  char *name_str;
  RECT rect2;
  int casualties;

  net.civ_new_player_flags[civ] = 0;
  if (turn_loop.active == '\0') {
    net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
    return 1;
  }
  if ((civ_view.intro_shown[civ] == 0) && (autoplay.active == 0)) {
    civ_intro_popup(civ);
    civ_view.intro_shown[civ] = 1;
  }
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
    mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
    map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  }
  net.mp_turn_civ = civ;
  menu_update_states();
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
  rect2.left = map_views_block.main_view_frame_left;
  rect2.top = map_views_block.main_view_frame_top;
  rect2.right = map_views_block.main_view_frame_right;
  rect2.bottom = map_views_block.main_view_client_top;
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)&rect2);
  net_send_message(NM_START_WHOSE_TURN,0xff,civ,0,0,0,0,0,0,0);
  ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
  map_redraw_all_views(human_civ[0],1);
  status_draw_civ_color(civ);
  casualties = net.casualty_counts[human_civ[0]] -
             net.casualty_reported_counts[human_civ[0]];
  if (autoplay.active == 0) {
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    name_str = (char *)civ_leader_title(human_civ[0]);
    text_set_string_arg(0,name_str);
    name_str = civ_leader_name(human_civ[0]);
    text_set_string_arg(1,name_str);
    if (net.casualty_reported_counts[human_civ[0]] <
        net.casualty_counts[human_civ[0]]) {
      net.casualty_reported_counts[human_civ[0]] =
           net.casualty_counts[human_civ[0]];
      text_set_number_arg(0,casualties);
      if (casualties == 1) {
        dialog2->load_text_noflags((const char *)&str_game_7,(const char *)&str_casualty_3);
      }
      else {
        dialog2->load_text_noflags((const char *)&str_game_7,(const char *)&str_casualties_3);
      }
      dialog2->add_sprite((iptr)&popup_info_icon_sprite,0,0);
      dialog2->set_image_zoom(0);
      input_flush_events();
      snd_play_effect(SND_NEWONDER,0,0,0);
      dialog2->show_modal(0);
      if (dialog2->result_extra != 0) {
        casualty_timeline_show(human_civ[0]);
      }
    }
    else {
      dialog2->load_text_noflags((const char *)&str_game_7,(const char *)&str_ourturntomove_2);
      dialog2->add_sprite((iptr)&popup_info_icon_sprite,0,0);
      dialog2->set_image_zoom(0);
      input_flush_events();
      snd_play_effect(SND_NEWONDER,0,0,0);
      dialog2->show_modal(0);
    }
  }
  if (autoplay.active == 0) {
    if ((*(byte *)&turn_loop.resume_civ) == 0) {
      civ_check_throne_room_upgrade();
    }
    if ((((*(byte *)&turn_loop.resume_civ) == 0) || (turn_resume_new_game != 0)) || ((char)force_process != '\0')) {
      if (((*(byte *)&turn_loop.resume_civ) != 0) && (turn_resume_new_game != 0)) {
        ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
        map_redraw_all_views(human_civ[0],1);
        wnd_flush_paint();
      }
      civ_process_turn(civ);
    }
    game_turn_start_popups();
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
      mp_turn_timer_start(1);
    }
    game_player_move_loop();
  }
  else {
    map_views_block.saved_human_players = (uint)game.human_players;
    game.human_players = '\0';
    if ((((*(byte *)&turn_loop.resume_civ) == 0) || (turn_resume_new_game != 0)) || ((char)force_process != '\0')) {
      if (((*(byte *)&turn_loop.resume_civ) != 0) && (turn_resume_new_game != 0)) {
        ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
        map_redraw_all_views(human_civ[0],1);
        wnd_flush_paint();
      }
      civ_process_turn(civ);
    }
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
      mp_turn_timer_start(1);
    }
    ai_process_civ_units();
    game.human_players = (*(u8 *)&map_views_block.saved_human_players);
    key_state = GetAsyncKeyState(0x1b);
    if (((int)key_state & 0x8001U) != 0) {
      autoplay.turns_left = 1;
    }
    autoplay.turns_left--;
    if (autoplay.turns_left == 0) {
      autoplay.active = 0;
    }
  }
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
    mp_turn_timer_stop(1);
  }
  return 0;
}

/* 0048c9f3 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl game_mp_host_turn_loop(i32 force_process)

{
  byte start_civ;
  char *name_str;
  int result;
  iptr host_civ;
  uptr log_civ;
  uptr civ;
  iptr city;
  iptr unit;
  Dialog dialog2;
  int found_unit;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  host_civ = human_civ[0];
  name_str = civ_name_plural(human_civ[0]);
  dbg_logf_str_int((const char *)&str_server_s_d_2,(const char *)name_str,host_civ);
  for (civ = 1; (int)civ < 8; civ++) {
    if (((human_civ[0] != civ) &&
        ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      log_civ = civ;
      name_str = civ_name_plural(civ);
      dbg_logf_str_int((const char *)&str_client_s_d_2,(const char *)name_str,log_civ);
    }
  }
  net_reset_error_counts();
  map_rebuild_unit_flags();
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  menu_update_states();
  ui_redraw_all_maps(1);
  ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
  found_unit = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit].x,(int)units[unit].y);
      found_unit++;
      break;
    }
  }
  if (found_unit == 0) {
    for (unit = 0; unit < game.num_cities; unit++) {
      if ((cities[unit].id != 0) && (cities[unit].owner == human_civ[0])) {
        ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)cities[unit].x,(int)cities[unit].y
                  );
        break;
      }
    }
  }
  start_civ = (*(byte *)&turn_loop.resume_civ);
  if ((*(byte *)&turn_loop.resume_civ) != 0) {
    while ((1 << ((*(byte *)&turn_loop.resume_civ) & 0x1f) & (uint)game.civs_alive) == 0) {
      (*(byte *)&turn_loop.resume_civ)++;
      if ((*(byte *)&turn_loop.resume_civ) == 8) {
        (*(byte *)&turn_loop.resume_civ) = 0;
      }
      if ((*(byte *)&turn_loop.resume_civ) == start_civ) {
        game_over_reason = 1;
        break;
      }
    }
  }
  do {
    net_send_state_diff(0xff,2,0,0,0);
    XD_FlushSendBuffer(60000);
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
    game_calc_human_wonder_range();
    if ((*(byte *)&turn_loop.resume_civ) == 0) {
      game_advance_turn(0xfffffffd);
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(60000);
    }
    result = game_check_retirement();
    if ((result != 0) || (game_over_reason != 0)) {
      turn_loop.active = '\0';
      break;
    }
    if ((scenario_events_loaded != 0) && (((*(byte *)&turn_loop.resume_civ) == 0 || (turn_resume_new_game != 0)))) {
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_turn((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_turn_interval((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_random_turn((int)game.turn);
      ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_received_tech();
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    for (civ = 0; (turn_loop.active != '\0' && ((int)civ < 8)); civ++) {
      game.human_players = (net.pending_human_players | game.human_players) & (byte)civ_saved_cursor_x_block.net_claimed_civs_mask;
      net.pending_human_players = 0;
      human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
      game.player_civ = (i8)human_civ[0];
      if (((int)(uint)(*(byte *)&turn_loop.resume_civ) <= (int)civ) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) {
        if (((((*(byte *)&turn_loop.resume_civ) != 0) && ((*(byte *)&turn_loop.resume_civ) != civ)) || ((*(byte *)&turn_loop.resume_civ) == 0)) ||
           ((char)force_process != '\0')) {
          units_reset_for_turn(civ);
        }
        net_send_state_diff(0xff,2,0,0,0);
        XD_FlushSendBuffer(5000);
        while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
          net.process_messages(1,0);
        }
        game.turn_civ = (byte)civ;
        if (human_civ[0] == civ) {
          if (mp_host_own_turn(civ,&dialog2,force_process) != 0) break;  /* the loop ended (turn_loop.active clear) */
        }
        else if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
          mp_host_ai_turn(civ);
        }
        else {
          mp_host_remote_turn(civ);
        }
        status_redraw(1);
        net_check_lost_connections();
        if ((net.player_has_quit == '\0') || (net.mp_turn_civ == human_civ[0])) {
          for (unit = 0; unit < game.num_units; unit++) {
            if ((units[unit].id != 0) && ((int)units[unit].owner == civ)) {
              unit_exhaust_moves(unit);
            }
          }
          city = (iptr)game.num_cities;
          while (city = city - 1, -1 < city) {
            if (cities[city].id != 0) {
              cities[city].flags = cities[city].flags & ~CITYF_INVESTIGATED;
            }
          }
        }
        saved_turn.civ = 0;
        (*(byte *)&turn_loop.resume_civ) = 0;
        saved_turn.resume_new_game = 0;
        turn_resume_new_game = 0;
        (*((char *)&force_process + 0)) = '\0';
        game.transient_flags = game.transient_flags & ~TRANSIENT_END_OF_TURN;
        ui_pump_messages();
        if (turn_loop.active == '\0') break;
        if ((turn_loop.active != '\0') && (game_over_reason != 0)) {
          turn_loop.active = '\0';
          break;
        }
      }
    }
    if (turn_loop.active == '\0') break;
  } while( true );
  mp_turn_timer_stop(0);
  net_host_quit_handoff();
  if (game_over_reason != 0) {
    game_handle_game_over();
  }
  return;
}

/* 00497da0 auto */
iptr __cdecl save_read_cstr(char *buf,iptr file)

{
  int ch;
  i8 *dst;
  int i;
  
  dst = buf;
  ch = 1;
  for (i = 0; (ch != 0 && (i < 0x4f)); i++) {
    ch = _fgetc((FILE *)file);
    *dst = (i8)ch;
    dst++;
  }
  _fgetc((FILE *)file);
  return (iptr)buf;
}

/* 00497e0f auto */
void __cdecl save_write_cstr_eof(const char *str,iptr file)

{
  i32 ch;
  
  ch = 1;
  while (ch != 0) {
    ch = (int)*(char *)str;
    str++;
    _fputc(ch,(FILE *)file);
  }
  _fputc(0x1a,(FILE *)file);
  return;
}

/* 00498784 auto */
void __cdecl passwords_reset(void)

{
  byte sign;
  int rnd;
  int civ;
  iptr i;
  
  for (i = 0; i < 0xff; i++) {
    rnd = _rand();
    sign = (byte)(rnd >> 0x1f);
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] = (((byte)rnd ^ sign) - sign ^ sign) - sign;
  }
  for (civ = 0; civ < 8; civ++) {
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[civ * 0x20] = 0;
    map_views_block.civ_password_prompt_pending[civ] = 0;
    map_views_block.civ_has_password[civ] = 0;
  }
  passwords_scramble();
  return;
}

/* 0049882b auto */
void __cdecl passwords_refresh_flags(void)

{
  int civ;
  
  passwords_unscramble();
  for (civ = 0; civ < 8; civ++) {
    if ((((i8 *)&civ_saved_cursor_x_block.civ_passwords))[civ * 0x20] == '\0') {
      map_views_block.civ_password_prompt_pending[civ] = 0;
      map_views_block.civ_has_password[civ] = 0;
    }
    else {
      map_views_block.civ_password_prompt_pending[civ] = 1;
      map_views_block.civ_has_password[civ] = 1;
    }
  }
  passwords_scramble();
  return;
}

/* 004988b8 auto */
void __cdecl passwords_scramble(void)

{
  char prev_byte;
  iptr i;
  byte carry;
  
  prev_byte = civ_saved_cursor_x_block.civ_passwords_last_byte;
  for (i = 0; carry = prev_byte << 5, i < 0x100; i++) {
    prev_byte = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i];
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] =
         ((byte)((uint)(int)(((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] >> 3) & 0x1f | carry) ^ (byte)i
    ;
  }
  return;
}

/* 00498943 auto */
void __cdecl passwords_unscramble(void)

{
  uint carry;
  iptr i;
  char prev_byte;
  
  prev_byte = civ_saved_cursor_x_block.civ_passwords[0];
  for (i = 0xff; -1 < i; i--) {
    carry = (uint)prev_byte;
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] ^ (byte)i;
    prev_byte = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i];
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] << 3 | (byte)(carry >> 5) & 7;
  }
  return;
}

/* 004989d3 auto */
void __cdecl password_dialog_set_title(Dialog *dialog,int civ,i32 label_id)

{
  size_t title_len;
  i32 title_mem;
  char title[128];
  
  crt_strcpy(title,(const char *)(&civ_names[civ].leader_name));
  str_append_label((iptr)title,label_id);
  title_len = _strlen(title);
  title_mem = memblock_alloc((MemBlock *)dialog->string_pool,title_len + 1);
  dialog->title_text = title_mem;
  crt_strcpy((char *)((void *)dialog->title_text),title);
  return;
}

/* 004a70b0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl prefs_set_defaults(void)

{
  int has_memory;
  iptr i;
  
  (*(i16 *)&prefs.game_rules) = 0x3f;
  prefs.default_level = 0;
  if (999 < caption_font_block.screen_width) {
    prefs.default_level = 2;
  }
  prefs.difficulty = 0;
  prefs.tribe = 0;
  prefs.gender = 0;
  prefs.science_rate = 6;
  prefs.tax_rate = 4;
  prefs.enemies_choice = 0;
  prefs.map_size = 1;
  prefs.game_options = 0x3f3258;
  has_memory = mem_check_available(10000000);
  if (has_memory == 0) {
    prefs.game_options = prefs.game_options & 0xffdfffff;
  }
  (*(i16 *)&prefs.message_options) = 0;
  prefs.barbarity = 2;
  for (i = 0; i < 5; i++) {
    (((i16 *)&prefs.world_params))[i] = 0;
  }
  crt_strcpy(((i8 *)&prefs.text_field),((i8 *)&prefs_defaults_empty_str));
  prefs.rules_choice = 0;
  prefs.accelerated_start = 0;
  return;
}

/* 004a71bb auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl prefs_set_mp_defaults(void)

{
  iptr j;
  int i;
  
  menu_prefs.main_menu_choice[0] = 0;
  menu_prefs.multiplayer_type = 0;
  menu_prefs.hotseat_menu_choice = 0;
  menu_prefs.lan_menu_choice = 0;
  menu_prefs.mp_prefs_setting_1 = 0;
  menu_prefs.pbem_menu_choice = 0;
  (*(i16 *)&menu_prefs.net_protocol) = 0;
  menu_prefs.mp_open_game = 0;
  menu_prefs.mp_kill_on_retire = 0;
  menu_prefs.mp_show_human_starts = 0;
  menu_prefs.mp_chat_from_start = 0;
  menu_prefs.mp_simultaneous_moves = 0;
  menu_prefs.net_game_name[0] = 0;
  menu_prefs.net_player_name[0] = 0;
  menu_prefs.email_address[0] = 0;
  menu_prefs.net_ip_address[0] = 0;
  menu_prefs.turn_timer_choice = 0;
  crt_strcpy(((i8 *)&menu_prefs.turn_timer_custom),((i8 *)&prefs_turn_timer_default_str));
  menu_prefs.hotseat_players_choice = 0;
  _memset(((i8 *)&menu_prefs.parley_wnd_rect),0,0x10);
  _memset(((i8 *)&menu_prefs.chat_wnd_rect),0,0x10);
  menu_prefs.clear_chat_on_new_game = 0;
  menu_prefs.clear_chat_on_boot = 0;
  menu_prefs.mp_double_production = 0;
  menu_prefs.mp_double_moves = 0;
  menu_prefs.mp_prefs_setting_2 = 0;
  menu_prefs.mp_prefs_setting_3 = 0;
  menu_prefs.mp_prefs_setting_4 = 0;
  for (i = 0; i < 7; i++) {
    for (j = 0; j < 0x18; j++) {
      (((i8 *)&menu_prefs.mp_prefs_settings_table))[i * 0x18 + j] = 0;
    }
  }
  menu_prefs.mp_prefs_setting_5 = 0;
  return;
}

/* 004a733d auto */
void __cdecl prefs_load_civ2dat(void)

{
  FILE *_File;
  size_t read_count;
  
  _File = _fopen(str_ptr_civ2_dat,((i8 *)&civ2dat_read_mode_str));
  if (_File == (FILE *)0x0) {
    prefs_set_defaults();
    prefs_set_mp_defaults();
  }
  else {
    read_count = _fread(((i16 *)&prefs.game_rules),0x48,1,_File);
    if (read_count == 0) {
      prefs_set_defaults();
    }
    read_count = _fread(((i16 *)&menu_prefs.main_menu_choice),0x1bc,1,_File);
    /* BF-226 (Peter Johnson's ledger): the game, player, e-mail and address
       names end within their 32 bytes, else the defaults as for a short file */
    if ((read_count == 0) || (memchr(menu_prefs.net_game_name,0,0x20) == (void *)0x0) ||
       (memchr(menu_prefs.net_player_name,0,0x20) == (void *)0x0) ||
       (memchr(menu_prefs.email_address,0,0x20) == (void *)0x0) ||
       (memchr(menu_prefs.net_ip_address,0,0x20) == (void *)0x0)) {
      prefs_set_mp_defaults();
    }
    _fclose(_File);
  }
  return;
}

/* 004a73d9 auto */
int __cdecl prefs_save_civ2dat(void)

{
  FILE *_File;
  size_t write_count;
  int failed;
  
  failed = 1;
  _File = _fopen(str_ptr_civ2_dat,((i8 *)&civ2dat_write_mode_str));
  if (((_File != (FILE *)0x0) && (write_count = _fwrite(((i16 *)&prefs.game_rules),0x48,1,_File), write_count != 0)) &&
     (write_count = _fwrite(((i16 *)&menu_prefs.main_menu_choice),0x1bc,1,_File), write_count != 0)) {
    failed = 0;
  }
  if ((_File != (FILE *)0x0) && (_fclose(_File), failed != 0)) {
    crt_remove(str_ptr_civ2_dat);
  }
  return failed;
}

/* 004a7754 auto */
void __cdecl game_balance_start_positions(void)

{
  byte terrain_id;
  iptr found_unit;
  iptr other_landmass;
  int unit;
  byte *tile;
  iptr civ;
  int landmass_starts[64];
  int y2;
  iptr start_units[8];
  iptr landmass2;
  int size_bonus;
  int y3;
  int x2;
  int x3;
  int dist;
  int grass_count;
  uint terrain2;
  iptr other_civ;
  int max_score;
  int min_dist;
  iptr k;
  int score;
  int civ_scores[8];
  int special_count;
  
  for (landmass2 = 0; landmass2 < 0x40; landmass2++) {
    landmass_starts[landmass2] = 0;
  }
  x3 = (int)map.width >> 1;
  y3 = (int)map.height >> 1;
  for (civ = 1; civ < 8; civ++) {
    found_unit = unit_find_nearest(x3,y3,civ,-1);
    start_units[civ] = found_unit;
    if (-1 < found_unit) {
      landmass2 = tile_land_landmass(x3,y3);
      if (-1 < landmass2) {
        landmass_starts[landmass2]++;
      }
    }
  }
  max_score = 0;
  for (civ = 1; civ < 8; civ++) {
    unit = start_units[civ];
    if (-1 < unit) {
      x3 = (int)units[unit].x;
      y3 = (int)units[unit].y;
      landmass2 = tile_land_landmass(x3,y3);
      landmass2 = int_clamp(landmass2,0,0x3f);
      score = 0;
      min_dist = 999;
      for (other_civ = 1; other_civ < 8; other_civ++) {
        unit = start_units[other_civ];
        if ((-1 < unit) && (civ != other_civ)) {
          x2 = (int)units[unit].x;
          y2 = (int)units[unit].y;
          other_landmass = tile_land_landmass(x2,y2);
          if (landmass2 == other_landmass) {
            dist = map_distance_diag(x3,y3,x2,y2);
            if (dist < min_dist) {
              min_dist = dist;
            }
          }
        }
      }
      if (9 < min_dist) {
        if (min_dist < 0x14) {
          score++;
        }
        else if (min_dist == 999) {
          size_bonus = (int)landmass_table_block.landmass_recs[landmass2].tiles / 0x32;
          size_bonus = int_clamp(size_bonus,3,6);
          score += size_bonus;
        }
        else {
          score += 2;
        }
      }
      grass_count = 0;
      special_count = 0;
      for (k = 0; k < 0x15; k++) {
        x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[k] + x3);
        y2 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[k] + y3;
        unit = map_in_bounds(x2,y2);
        if (unit != 0) {
          terrain_id = tile_terrain(x2,y2);
          terrain2 = (uint)terrain_id;
          if (terrain2 == TERRAIN_GRASSLAND) {
            grass_count++;
          }
          tile = (byte *)tile_at(x2,y2);
          if ((*tile & 0x80) != 0) {
            special_count++;
          }
        }
      }
      if (special_count < 4) {
        if (3 < grass_count) {
          score += 2;
        }
      }
      else {
        score += 4;
      }
      switch((int)landmass_table_block.landmass_recs[landmass2].tiles / 0x32) {
      case 0:
        break;
      case 1:
        score++;
        break;
      case 2:
        score += 2;
        break;
      case 3:
        score += 3;
        break;
      default:
        score += 3;
      }
      if (max_score < score) {
        max_score = score;
      }
      civ_scores[civ] = score;
    }
  }
  for (civ = 1; civ < 8; civ++) {
    unit = start_units[civ];
    if (-1 < unit) {
      if (((3 < max_score - civ_scores[civ]) ||
          (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 &&
           (game.difficulty == DIFF_DEITY)))) ||
         ((game.multiplayer != '\0' && (game.difficulty == DIFF_DEITY)))) {
        unit_create(UNIT_SETTLERS,civ,(int)units[unit].x,(int)units[unit].y);
        if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
           (game.difficulty != DIFF_DEITY)) {
          civ_scores[civ] += 3;
        }
        else {
          civ_scores[civ]++;
        }
      }
      for (k = 0; k < max_score - civ_scores[civ]; k++) {
        civ_research_complete(civ,0);
      }
    }
  }
  return;
}

/* 004a9785 auto */
void __cdecl game_accelerated_start(int extra_level)

{
  char civ2;
  short y5;
  iptr x3;
  byte terrain_id;
  iptr in_bounds;
  iptr city;
  iptr rnd;
  iptr unit3;
  iptr stack_count;
  int unit2;
  int result;
  iptr x;
  byte *tile;
  uint coin;
  int y;
  uint sign_mask;
  uptr civ4;
  uptr civ3;
  iptr civ;
  iptr unit;
  iptr best_dir;
  iptr y4;
  i8 home_id;
  int steps;
  iptr unit_type;
  iptr x4;
  int y2;
  int x2;
  int start_y2[8];
  uint terrain2;
  int start_x2[8];
  int y3;
  int dist;
  iptr i;
  iptr city2;
  int best_score;
  int site_score;
  
  for (civ = 0; civ < 8; civ++) {
    start_y2[civ] = -1;
    start_x2[civ] = start_y2[civ];
  }
  for (unit = 0; unit < game.num_units; unit++) {
    if (units[unit].id != 0) {
      y5 = units[unit].y;
      civ2 = units[unit].owner;
      start_x2[civ2] = (int)units[unit].x;
      start_y2[civ2] = (int)y5;
    }
  }
  civ = 1;
  do {
    if (7 < civ) {
      game.turn = ((short)extra_level * 4 + 4) * 5 + 1;
      return;
    }
    x3 = start_x2[civ];
    y = start_y2[civ];
    in_bounds = map_in_bounds(x3,y);
    if (in_bounds != 0) {
      civ_give_advance(civ,ADV_HORSEBACK_RIDING,0,0,1);
      civ_give_advance(civ,ADV_CEREMONIAL_BURIAL,0,0,1);
      civ_give_advance(civ,ADV_ALPHABET,0,0,1);
      civ_give_advance(civ,ADV_BRONZE_WORKING,0,0,1);
      civs[civ].num_advances = 4;
      if (extra_level != 0) {
        for (i = 0; i < 3; i++) {
          civ_research_complete(civ,0);
          civs[civ].num_advances++;
        }
      }
      city = city_create(x3,y,civ);
      if (-1 < city) {
        rnd = _rand();
        civs[civ].gold = (rnd % 0x32 + 0x19) * (extra_level + 1);
        if (extra_level == 0) {
          cities[city].size = 3;
        }
        else {
          cities[city].size = 5;
        }
        unit3 = tile_first_unit(x3,y);
        stack_count = unit_stack_sum(unit3,2);
        if (1 < stack_count) {
          home_id = (i8)city;
          units[unit3].home_city = home_id;
        }
        unit_type = (iptr)cities[city].building;
        _rand();
        if (unit_type != UNIT_PIKEMEN) {
          unit_type = 4;
        }
        unit2 = unit_create(unit_type,civ,x3,y);
        units[unit2].orders = ORDER_FORTIFIED;
        unit2 = unit_create(UNIT_PHALANX,civ,x3,y);
        units[unit2].orders = ORDER_FORTIFIED;
        civ3 = civ;
        x = x3;
        unit2 = y;
        result = civ_has_advance(civ,ADV_WHEEL);
        unit_create(0x10 - (uint)(result == 0),civ3,x,unit2);
        if (extra_level != 0) {
          civ4 = civ;
          x = x3;
          unit2 = y;
          result = civ_has_advance(civ,ADV_POLYTHEISM);
          unit_create(0x11 - (uint)(result == 0),civ4,x,unit2);
        }
        city_set_improvement(city,4,1);
        if (extra_level != 0) {
          unit2 = civ_has_advance(civ,ADV_POTTERY);
          if (unit2 != 0) {
            city_set_improvement(city,3,1);
          }
          unit2 = civ_has_advance(civ,ADV_CURRENCY);
          if (unit2 == 0) {
            city_set_improvement(city,2,1);
          }
          else {
            city_set_improvement(city,5,1);
          }
        }
        accel_start_develop_city(city,(-(uint)(extra_level == 0) & 0xfffffffe) + 4);
        if (extra_level != 0) {
          best_score = -1;
          best_dir = -1;
          unit2 = tile_landmass(x3,y);
          for (i = 0; i < 8; i++) {
            x = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[i] * 5 + x3);
            y3 = (((i8 *)&dir_dy_table_block.dir_dy_table))[i] * 5 + y;
            result = map_in_bounds(x,y3);
            if (((result != 0) && (result = tile_is_ocean(x,y3), result == 0)) &&
               (result = tile_landmass(x,y3), result == unit2)) {
              city_find_nearest(x,y3,-1,0xffffffff,-1);
              for (unit = 0; unit < game.num_units; unit++) {
                if (((units[unit].id != 0) &&
                    (result = tile_city_owner((int)units[unit].x,(int)units[unit].y),
                    result < 0)) &&
                   (dist = map_distance_diag((int)units[unit].x,(int)units[unit].y,
                                                  x,y3), dist < advisor_window_block.nearest_city_dist)) {
                  advisor_window_block.nearest_city_dist = dist;
                }
              }
              if (4 < dist) {
                site_score = 0;
                terrain_id = tile_terrain(x,y3);
                terrain2 = (uint)terrain_id;
                if (terrain2 == TERRAIN_GRASSLAND) {
                  site_score = 4;
                }
                if (terrain2 == TERRAIN_PLAINS) {
                  site_score = 2;
                }
                if (site_score != 0) {
                  tile = (byte *)tile_at(x,y3);
                  if ((*tile & 0x80) != 0) {
                    site_score++;
                  }
                  if (best_score < site_score) {
                    best_score = site_score;
                    best_dir = i;
                    y4 = y3;
                    x4 = x;
                  }
                }
              }
            }
          }
          if (best_dir < 0) {
            civ_give_advance(civ,ADV_CURRENCY,0,0,1);
            civ_give_advance(civ,ADV_MASONRY,0,0,1);
            civ_give_advance(civ,ADV_CONSTRUCTION,0,0,1);
            civ_give_advance(civ,ADV_MYSTICISM,0,0,1);
            civ_give_advance(civ,ADV_CODE_OF_LAWS,0,0,1);
            civ_give_advance(civ,ADV_MONARCHY,0,0,1);
            cities[city].size = 7;
            city_set_improvement(city,0xe,1);
            civs[civ].gold += 0x19;
            accel_start_develop_city(city,2);
            unit_create(UNIT_ARCHERS,civ,(int)cities[city].x,(int)cities[city].y);
            unit_create(UNIT_LEGION,civ,(int)cities[city].x,(int)cities[city].y);
            unit_create(UNIT_CHARIOT,civ,(int)cities[city].x,(int)cities[city].y);
          }
          else {
            city2 = city_create(x4,y4,civ);
            cities[city2].size = 3;
            coin = _rand();
            sign_mask = (int)coin >> 0x1f;
            coin = (coin ^ sign_mask) - sign_mask & 1 ^ sign_mask;
            unit2 = unit_create(3 - (uint)(coin == sign_mask),civ,x4,y4);
            units[unit2].orders = ORDER_FORTIFIED;
            unit2 = unit_create((coin == sign_mask) + 3,civ,x4,y4);
            units[unit2].orders = ORDER_FORTIFIED;
            unit_create(UNIT_HORSEMEN,civ,x4,y4);
            accel_start_develop_city(city2,2);
            x2 = (int)cities[city2].x;
            y2 = (int)cities[city2].y;
            steps = 0;
            unit2 = map_distance(x3,y,x2,y2);
            if (unit2 < 0x17) {
              pathfind.road_planning = 1;
              pathfind.owner_civ = 0xffffffff;
              pathfind.unit_type = 2;
              path_queue.goal_x = x3;
              path_queue.goal_y = y;
              do {
                unit2 = map_path_next_dir(x2,y2,99);
                if ((unit2 < 0) || (unit2 == 8)) break;
                x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[unit2] + x2);
                y2 += (((i8 *)&dir_dy_table_block.dir_dy_table))[unit2];
                if ((x3 == x2) && (y == y2)) break;
                unit2 = tile_city_owner(x2,y2);
                if ((((unit2 < 0) &&
                     (coin = tile_improvements(x2,y2), (coin & 0x10) == 0)) &&
                    ((tile = (byte *)tile_at(x2,y2), (*tile & 0x80) == 0 ||
                     (unit2 = civ_has_advance(civ,ADV_BRIDGE_BUILDING), unit2 != 0)))) &&
                   ((unit2 = tile_city_or_unit_owner(x2,y2), unit2 < 0 &&
                    (unit2 = tile_owner_at_peace(x2,y2,civ), unit2 < 0)))) {
                  tile_set_improvements(x2,y2,0x10,1,1);
                  tile_update_known(x2,y2,civ);
                }
                steps++;
              } while (steps < 0x32);
              pathfind.road_planning = 0;
            }
          }
        }
        y = civ_has_advance(civ,ADV_MONARCHY);
        if (y != 0) {
          civs[civ].government = GOV_MONARCHY;
        }
      }
    }
    civ++;
  } while( true );
}

/* 004aa9c0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl game_init_new(void)

{
  int value;
  iptr civ;
  iptr j;
  iptr leader;
  
  scenario_reset_params();
  destroyed_civs_clear();
  for (leader = 0; leader < 8; leader++) {
    civ_primary_colors_block.civ_colors[leader].flags = 0;
  }
  for (leader = 0; leader < 0x15; leader++) {
    leaders[leader].times_used = '\0';
    if (bugfix_state()) {
      leaders[leader].city_name_index = '\0';  /* B2: a new game names the cities from the list's start (docs/BUGS.md) */
    }
  }
  game.turn = 0;
  game.year = -4000;
  game.first_launch_turn = -1;
  if (game.difficulty == DIFF_CHIEFTAIN) {
    game.game_options = prefs.game_options & 0xffff7fff | 0x300;
  }
  else {
    game.game_options = prefs.game_options & 0xffff7eff;
  }
  game.transient_flags = 0;
  game.message_options = prefs.message_options;
  game.shown_event_flags = 0;
  moving_units_mode = 1;
  game.num_units = 0;
  game.num_cities = 0;
  game.active_unit = 0;
  game.turn_civ = '\0';
  game.unused_34 = 0;
  game.reveal_map = '\0';
  game.view_civ = -1;
  game.polluted_tiles = 0;
  game.warming_counter = '\0';
  game.global_warmings = '\0';
  game.warming_pollution = 0;
  for (leader = 0; leader < 100; leader++) {
    game.first_discoverer[leader] = '\0';
    game.advance_known_by[leader] = '\0';
  }
  for (leader = 0; leader < 0x1c; leader++) {
    game.wonder_city[leader] = -1;
  }
  for (civ = 0; civ < 8; civ++) {
    civ_reset_spaceship(civ);
    civs[civ].num_cities = 0;
    civ_unit_counts_block.civ_unit_totals[civ] = 0;
    civs[civ].num_military_units = 0;
    civs[civ].num_ships = 0;
    civs[civ].total_city_size = 0;
    civs[civ].military_strength = 0;
    civs[civ].total_attack = 0;
    civs[civ].researching = -1;
    for (leader = 0; leader < 0x40; leader++) {
      *(i16 *)&civs[civ].continent_military[leader] = 0;
      *(i16 *)&civs[civ].continent_attack[leader] = 0;
      *(i8 *)&civs[civ].continent_cities[leader] = 0;
      *(i8 *)&civs[civ].continent_city_size[leader] = 0;
    }
  }
  value = _rand();
  game.next_history_turn = (i16)(value % 0x32);
  for (leader = 0; leader < 0x96; leader++) {
    for (j = 0; j < 8; j++) {
      (((i8 *)&power_graph_history_block.power_graph_history))[leader * 8 + j] = 0;
    }
  }
  for (civ = 0; civ < 8; civ++) {
    civ_create(civ);
  }
  game.civs_at_start = game.civs_alive;
  game_balance_start_positions();
  if (((10 < (int)prefs.science_rate + (int)prefs.tax_rate) || (6 < prefs.science_rate)) ||
     (6 < prefs.tax_rate)) {
    prefs.science_rate = 6;
    prefs.tax_rate = 4;
  }
  value = bits_count8((uint)game.human_players);
  if (value == 1) {
    civs[game.player_civ].science_rate = (*(schar *)&prefs.science_rate);
    civs[game.player_civ].tax_rate = (*(schar *)&prefs.tax_rate);
    if (game.difficulty == DIFF_CHIEFTAIN) {
      civs[game.player_civ].gold = 0x32;
    }
  }
  else {
    for (civ = 1; civ < 8; civ++) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
        civs[civ].science_rate = 6;
        civs[civ].tax_rate = 4;
        if (game.difficulty == DIFF_CHIEFTAIN) {
          civs[civ].gold = 0x32;
        }
      }
    }
  }
  return;
}

/* 004b08e0 auto */
/* Library Function - Single Match
    _$E2
   
   Library: Visual Studio 1998 Debug */

void __cdecl diffeng_global_init(void)

{
  diffeng_static_init_stub();
  return;
}

/* 004b08f5 auto */
/* Library Function - Single Match
    _$E1
   
   Library: Visual Studio 1998 Debug */

void __cdecl diffeng_static_init_stub(void)

{
  return;
}

/* 004b0905 auto */
i32 __cdecl diffeng_alloc_mirror(void)

{
  size_t mirror_len;
  i32 result;
  int section;
  
  if (diffeng.mirror_buf != 0) {
    diffeng_free_mirror();
  }
  diffeng_scan.mirror_size[0] = 0;
  for (section = 0; section < 0x17; section++) {
    diffeng_scan.mirror_size[0] += (i32)diffeng_section_table[section].padded_size;
  }
  if ((diffeng_scan.mirror_size[0] & 3) != 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_mirrorlength_sizeof_long_0,(iptr)&str_d_ss_franklinton_difference_engine_cpp,0x22);
  }
  diffeng.mirror_buf = (iptr)operator_new(diffeng_scan.mirror_size[0]);
  if ((void *)diffeng.mirror_buf == (void *)0x0) {
    result = 0;
  }
  else {
    _memset((void *)diffeng.mirror_buf,0,diffeng_scan.mirror_size[0]);
    mirror_len = diffeng_map_known1_section_block.diffeng_client_mirror_size;
    if (net.host_flag != '\0') {
      mirror_len = diffeng_scan.mirror_size[0];
    }
    diffeng_scan.mirror_end = mirror_len + diffeng.mirror_buf;
    diffeng_copy_to_mirror();
    result = 1;
  }
  return result;
}

/* 004b0a0a auto */
void __cdecl diffeng_free_mirror(void)

{
  operator_delete((void *)diffeng.mirror_buf);
  diffeng.mirror_buf = 0;
  return;
}

/* 004b0a41 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl diffeng_copy_to_mirror(void)

{
  int section;
  u8 *dst;
  
  dst = (u8 *)diffeng.mirror_buf;
  for (section = 0; section < 0x17; section++) {
    crt_memcpy(dst,(const void *)diffeng_section_table[section].data_ptr,
                 (i32)diffeng_section_table[section].data_size);
    dst += (i32)diffeng_section_table[section].padded_size;
  }
  diffeng_scan.scan_ptr = diffeng.mirror_buf;
  diffeng_scan.scan_section = 0;
  diffeng_scan.scan_offset = 0;
  return;
}

/* 004b0ad0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl diffeng_invalidate_mirror(void)

{
  uint word_count;
  uint i;
  
  diffeng_copy_to_mirror();
  (*(uint * *)&diffeng_scan.scan_ptr) = (uint *)diffeng.mirror_buf;
  word_count = diffeng_scan.mirror_size[0] >> 2;
  for (i = 0; i < word_count; i++) {
    *(*(uint * *)&diffeng_scan.scan_ptr) = ~*(*(uint * *)&diffeng_scan.scan_ptr);
    (*(uint * *)&diffeng_scan.scan_ptr)++;
  }
  (*(uint * *)&diffeng_scan.scan_ptr) = (uint *)diffeng.mirror_buf;
  diffeng_scan.scan_section = 0;
  diffeng_scan.scan_offset = 0;
  return;
}

/* 004b14a4 auto */
int __cdecl diffeng_total_size(void)

{
  int section;
  int total;
  
  total = 0;
  for (section = 0; section < 0x18; section++) {
    if (section == 5) {
      total = game.num_units * 0x20 + total;
    }
    else if (section == 6) {
      total = game.num_cities * 0x58 + total;
    }
    else {
      total += (int)diffeng_section_table[section].data_size;
    }
  }
  return total + 0x1e0;
}

/* 004b153c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl diffeng_build_civ_image(iptr out_buf)

{
  void *alloc;
  i32 result;
  i32 write_pos[2];
  uint capacity2;
  int buf;
  
  write_pos[0] = 0;
  capacity2 = diffeng_game_section_block.diffeng_game_section[0] + diffeng_leaders_section_block.diffeng_leaders_section[0] + diffeng_civs_section_block.diffeng_civs_section[0] + diffeng_civ_names_section_block.diffeng_civ_names_section[0] + diffeng_scenario_section_block.diffeng_scenario_section[0] + diffeng_civ_view_section_block.diffeng_civ_view_section[0]
            + diffeng_events_section_block.diffeng_events_section[0] + 0x8c;
  if ((int)capacity2 < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_size_0,(iptr)&str_d_ss_franklinton_difference_engine_cpp_4,0x226);
  }
  alloc = operator_new(capacity2);
  *(void **)out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocated_buffer_for_game_stat);
    result = 0;
  }
  else {
    diffeng_game_section_block.diffeng_game_checksum_i32 = mem_checksum(diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_game_section_block.diffeng_game_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_leaders_section_block.diffeng_leaders_checksum_i32 = mem_checksum(diffeng_leaders_section_block.diffeng_leaders_data,diffeng_leaders_section_block.diffeng_leaders_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_leaders_section_block.diffeng_leaders_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_leaders_section_block.diffeng_leaders_data,diffeng_leaders_section_block.diffeng_leaders_section[0]);
    diffeng_civs_section_block.diffeng_civs_checksum_i32 = mem_checksum(diffeng_civs_section_block.diffeng_civs_data,diffeng_civs_section_block.diffeng_civs_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_civs_section_block.diffeng_civs_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_civs_section_block.diffeng_civs_data,diffeng_civs_section_block.diffeng_civs_section[0]);
    diffeng_civ_names_section_block.diffeng_civ_names_checksum_i32 = mem_checksum(diffeng_civ_names_section_block.diffeng_civ_names_data,diffeng_civ_names_section_block.diffeng_civ_names_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_civ_names_section_block.diffeng_civ_names_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_civ_names_section_block.diffeng_civ_names_data,diffeng_civ_names_section_block.diffeng_civ_names_section[0]);
    diffeng_scenario_section_block.diffeng_scenario_checksum_i32 = mem_checksum(diffeng_scenario_section_block.diffeng_scenario_data,diffeng_scenario_section_block.diffeng_scenario_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_scenario_section_block.diffeng_scenario_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_scenario_section_block.diffeng_scenario_data,diffeng_scenario_section_block.diffeng_scenario_section[0]);
    diffeng_civ_view_section_block.diffeng_civ_view_checksum_i32 = mem_checksum(diffeng_civ_view_section_block.diffeng_civ_view_data,diffeng_civ_view_section_block.diffeng_civ_view_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_civ_view_section_block.diffeng_civ_view_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_civ_view_section_block.diffeng_civ_view_data,diffeng_civ_view_section_block.diffeng_civ_view_section[0]);
    diffeng_events_section_block.diffeng_events_checksum_i32 = mem_checksum(diffeng_events_section_block.diffeng_events_data,diffeng_events_section_block.diffeng_events_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_events_section_block.diffeng_events_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_events_section_block.diffeng_events_data,diffeng_events_section_block.diffeng_events_section[0]);
    result = mem_rle_compress(out_buf,write_pos[0]);
  }
  return result;
}

/* 004b187f auto */
void __cdecl diffeng_buf_append(char *buf,iptr pos_ptr,int capacity,iptr src,int len)

{
  if (capacity < *(int *)pos_ptr + len) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_cursize_datsize_bufsize,(iptr)&str_d_ss_franklinton_difference_engine_cpp_5,0x20d);
    return;   /* (BF-083) the original stopped at its assertion: the copy would overrun */
  }
  crt_memcpy((void *)(*(int *)pos_ptr + buf),(const void *)src,len);
  *(int *)pos_ptr = *(int *)pos_ptr + len;
  return;
}

/* 004b18e1 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl diffeng_build_map_header_image(iptr out_buf)

{
  void *alloc;
  i32 result;
  i32 write_pos[2];
  uint capacity2;
  int buf;
  
  write_pos[0] = 0;
  capacity2 = diffeng_game_section_block.diffeng_game_section[0] + 0x28 + diffeng_map_header_section_block.diffeng_map_header_section[0];
  alloc = operator_new(capacity2);
  *(void **)out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocated_buffer_for_game_stat_2);
    result = 0;
  }
  else {
    diffeng_game_section_block.diffeng_game_checksum_i32 = mem_checksum(diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_game_section_block.diffeng_game_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_map_header_section_block.diffeng_map_header_checksum_i32 = mem_checksum(diffeng_map_header_section_block.diffeng_map_header_data,diffeng_map_header_section_block.diffeng_map_header_section[0]);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,(iptr)&diffeng_map_header_section_block.diffeng_map_header_section,0x14);
    diffeng_buf_append((char *)buf,(iptr)write_pos,capacity2,diffeng_map_header_section_block.diffeng_map_header_data,diffeng_map_header_section_block.diffeng_map_header_section[0]);
    result = mem_rle_compress(out_buf,write_pos[0]);
  }
  return result;
}

/* 004b1a15 auto */
i32 __cdecl diffeng_build_full_image(iptr out_buf)

{
  uint capacity;
  void *alloc;
  i32 checksum;
  int encoded;
  RleStream rle;  /* (the original's locals in a row) */
  i32 result;
  int section;
  int header[5];
  i32 error_code;
  uint write_pos;
  int buf;
  
  capacity = diffeng_total_size();
  if ((int)capacity < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_sz_0,(iptr)&str_d_ss_franklinton_difference_engine_cpp_6,0x27f);
  }
  alloc = operator_new(capacity);
  *(void **)out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocated_buffer_for_game_stat_3);
    result = 0;
  }
  else {
    write_pos = 0;
    for (section = 0; section < 0x18; section++) {
      crt_memcpy(header,(i8 *)&diffeng_section_table[section],0x14);
      if (section == 5) {
        header[0] = (int)game.num_units << 5;
      }
      else if (section == 6) {
        header[0] = game.num_cities * 0x58;
      }
      checksum = mem_checksum((iptr)diffeng_section_table[section].data_ptr,header[0]);
      diffeng_section_table[section].checksum = checksum;
      diffeng_buf_append((char *)buf,(iptr)&write_pos,capacity,(iptr)header,0x14);
      diffeng_buf_append((char *)buf,(iptr)&write_pos,capacity,(iptr)diffeng_section_table[section].data_ptr,header[0]);
    }
    rle.src = (u8 *)operator_new(write_pos);
    if (rle.src == 0) {
      dbg_log((const char *)&str_failed_to_allocated_buffer_2_for_game_st);
      result = 0;
    }
    else {
      rle.src_end = rle.src + write_pos;
      crt_memcpy(rle.src,(const void *)buf,write_pos);
      rle.dst = *(u8 **)out_buf;
      rle.dst_end = *(u8 **)out_buf + write_pos;
      encoded = diffeng_rle_encode(&rle);
      result = rle.out_len;
      if (encoded == 0) {
        error_code = 0xffffffff;
        result = error_code;
      }
      else {
        checksum = (i32)crt_realloc((void *)*(i32 *)out_buf,result);
        *(i32 *)out_buf = checksum;
      }
    }
  }
  return result;
}

/* 004b1c11 auto */
i32 __cdecl diffeng_build_delta_image(iptr out_buf)

{
  uint capacity;
  void *alloc;
  i32 result;
  int section;
  int header[5];
  int checksum2;
  i32 write_pos[2];
  int buf;
  
  capacity = diffeng_total_size();
  if ((int)capacity < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_sz_0_2,(iptr)&str_d_ss_franklinton_difference_engine_cpp_7,0x2c6);
  }
  alloc = operator_new(capacity);
  *(void **)out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocate_buffer_for_game_state);
    result = 0;
  }
  else {
    write_pos[0] = 0;
    for (section = 0; section < 0x18; section++) {
      crt_memcpy(header,(i8 *)&diffeng_section_table[section],0x14);
      if (section == 5) {
        header[0] = (int)game.num_units << 5;
      }
      else if (section == 6) {
        header[0] = game.num_cities * 0x58;
      }
      checksum2 = mem_checksum((iptr)diffeng_section_table[section].data_ptr,header[0]);
      if (((int)diffeng_section_table[section].checksum != checksum2) && (header[0] != 0)) {
        diffeng_section_table[section].checksum = checksum2;
        diffeng_buf_append((char *)buf,(iptr)write_pos,capacity,(iptr)header,0x14);
        diffeng_buf_append((char *)buf,(iptr)write_pos,capacity,(iptr)diffeng_section_table[section].data_ptr,header[0]);
      }
    }
    result = (i32)crt_realloc((void *)*(i32 *)out_buf,write_pos[0]);
    *(i32 *)out_buf = result;
    if (*(int *)out_buf == 0) {
      dbg_log((const char *)&str_failed_to_re_allocate_buffer_for_game_st);
      result = 0;
    }
    else {
      result = mem_rle_compress(out_buf,write_pos[0]);
    }
  }
  return result;
}

/* 004b1de3 auto */
void __cdecl diffeng_apply_image(int *image_buf,char compressed)

{
  int *section_id_ptr;
  int *payload;
  size_t src_size;
  int decoded;
  i32 checksum;
  RleStream rle;  /* (the original's locals in a row) */
  u32 saved_game_options;
  u16 saved_msg_options;
  int *header;
  int offset;
  uint total_size;
  int *cursor;
  
  if ((image_buf == (int *)0x0) || (*image_buf == 0)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_phdata_phdata,(iptr)&str_d_ss_franklinton_difference_engine_cpp_8,0x2fc);
  }
  saved_game_options = game.game_options;
  saved_msg_options = game.message_options;
  if (diffeng_sections_block.diffeng_section_ptrs[0] == 0) {
    diffeng_register_blocks();
  }
  if (net.nm_full_gamestate_count == 0) {
    if (compressed != '\0') {
      total_size = mem_rle_decompress((iptr)image_buf);
    }
    cursor = (int *)*image_buf;
    total_size = __msize((void *)*image_buf);
  }
  else {
    rle.src = (u8 *)*image_buf;
    src_size = __msize((void *)*image_buf);
    rle.src_end = rle.src + src_size;
    total_size = diffeng_rle_decoded_size(&rle);
    rle.dst = (u8 *)operator_new(total_size);
    if (rle.dst == 0) {
      dbg_log((const char *)&str_failed_to_allocate_buffer_2_for_game_sta);
      return;
    }
    rle.dst_end = rle.dst + total_size;
    decoded = diffeng_rle_decode(&rle);
    if (decoded == 0) {
      dbg_log((const char *)&str_failed_to_rllbufferdecode_in_setgamestat);
      return;
    }
    operator_delete((void *)*image_buf);
    *image_buf = (int)rle.dst;
    cursor = (int *)*image_buf;
  }
  offset = 0;
  while (offset < (int)total_size) {
    header = cursor;
    payload = cursor + 5;
    offset = *cursor + offset + 0x14;
    if ((cursor[2] < 0) || (section_id_ptr = cursor + 2, cursor = payload, 0x17 < *section_id_ptr)) {
      cursor = payload;
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_pnode_type_0_pnode_type_num_block_types,(iptr)&str_d_ss_franklinton_difference_engine_cpp_9,0x338);
    }
    crt_memcpy((void *)(iptr)diffeng_section_table[header[2]].data_ptr,cursor,*header);
    checksum = mem_checksum((iptr)cursor,*header);
    diffeng_section_table[header[2]].checksum = checksum;
    cursor = (int *)((int)cursor + *header);
  }
  game.message_options = saved_msg_options;
  game.game_options = saved_game_options;
  return;
}

/* 004b2010 auto */
void __cdecl diffeng_apply_map_header_image(int *image_buf,char compressed)

{
  u32 saved_game_options;
  u16 saved_msg_options;
  int section_id;
  int cursor;
  
  if ((image_buf == (int *)0x0) || (*image_buf == 0)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_phdata_phdata_2,(iptr)&str_d_ss_franklinton_difference_engine_cpp_10,0x34b);
  }
  saved_msg_options = game.message_options;
  saved_game_options = game.game_options;
  if (compressed != '\0') {
    mem_rle_decompress((iptr)image_buf);
  }
  cursor = *image_buf;
  map_free();
  section_id = diffeng_read_block((iptr)&cursor);
  if (section_id != 1) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_blocktype_btgame,(iptr)&str_d_ss_franklinton_difference_engine_cpp_11,0x35a);
  }
  section_id = diffeng_read_block((iptr)&cursor);
  if (section_id != 0xc) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_blocktype_btmapstruct,(iptr)&str_d_ss_franklinton_difference_engine_cpp_12,0x35d);
  }
  game.game_options = saved_game_options;
  game.message_options = saved_msg_options;
  map_alloc();
  diffeng_register_blocks();
  diffeng_alloc_mirror();
  return;
}

/* 004b2123 auto */
int __cdecl diffeng_read_block(iptr cursor_ptr)

{
  int *header;
  i32 checksum;
  
  header = *(int **)cursor_ptr;
  *(int *)cursor_ptr = *(int *)cursor_ptr + 0x14;
  if ((header[2] < 0) || (0x17 < header[2])) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_pnode_type_0_pnode_type_num_block_types_2,(iptr)&str_d_ss_franklinton_difference_engine_cpp_13,0x36d);
  }
  crt_memcpy((void *)(iptr)diffeng_section_table[header[2]].data_ptr,(const void *)*(i32 *)cursor_ptr,*header);
  checksum = mem_checksum(*(iptr *)cursor_ptr,*header);
  diffeng_section_table[header[2]].checksum = checksum;
  *(int *)cursor_ptr = *(int *)cursor_ptr + *header;
  return header[2];
}

/* 004b21d7 auto */
void __cdecl diffeng_register_blocks(void)

{
  int offset;
  int section;
  
  memblock_ensure_locked((MemBlock *)&unit_types_block.str_empty_188);
  ((DiffEngSection *)((i8 *)&diffeng_sections_block.diffeng_sections))->init((iptr)&str_empty_115,4,0);
  ((DiffEngSection *)((i32 *)&diffeng_game_section_block.diffeng_game_section))->init((iptr)&game,0x14c,1);
  ((DiffEngSection *)((i32 *)&diffeng_civ_names_section_block.diffeng_civ_names_section))->init((iptr)&civ_names,0x790,2);
  ((DiffEngSection *)((i32 *)&diffeng_civs_section_block.diffeng_civs_section))->init((iptr)&civs,0x2ca0,3);
  ((DiffEngSection *)((i8 *)&diffeng_landmass_section))->init((iptr)&landmass_table_block.landmass_table,0x400,4);
  ((DiffEngSection *)((i8 *)&diffeng_units_section))->init((iptr)((u8 *)units),0x10000,5);
  ((DiffEngSection *)((i8 *)&diffeng_cities_section))->init((iptr)&cities,0x5800,6);
  ((DiffEngSection *)((i32 *)&diffeng_leaders_section_block.diffeng_leaders_section))->init((iptr)((u8 *)leaders),0x3f0,7);
  ((DiffEngSection *)((i8 *)&diffeng_power_graph_section))->init((iptr)&power_graph_history_block.power_graph_history,0x4b0,8);
  ((DiffEngSection *)((i32 *)&diffeng_scenario_section_block.diffeng_scenario_section))->init((iptr)&scenario.flags,100,9);
  ((DiffEngSection *)((i8 *)&diffeng_destroyed_civs_section))->init((iptr)&destroyed_civs.count,0x154,10);
  ((DiffEngSection *)((i32 *)&diffeng_civ_view_section_block.diffeng_civ_view_section))->init((iptr)&civ_view.saved_cursor_x,0x494,0xb);
  ((DiffEngSection *)((i32 *)&diffeng_map_header_section_block.diffeng_map_header_section))->init((iptr)&map,0x10,0xc);
  ((DiffEngSection *)((i8 *)&diffeng_map_coarse0_section))->init((iptr)map_data.coarse[0],(int)map.quarter_w * (int)map.quarter_h
             ,0xd);
  ((DiffEngSection *)((i8 *)&diffeng_map_coarse1_section))->init((iptr)map_data.coarse[1],(int)map.quarter_w * (int)map.quarter_h
             ,0xe);
  ((DiffEngSection *)((i8 *)&diffeng_map_known1_section_block.diffeng_map_known1_section))->init((iptr)map_data.known[1],(int)map.area,0xf);
  ((DiffEngSection *)((i8 *)&diffeng_map_known2_section))->init((iptr)map_data.known[2],(int)map.area,0x10);
  ((DiffEngSection *)((i8 *)&diffeng_map_known3_section))->init((iptr)map_data.known[3],(int)map.area,0x11);
  ((DiffEngSection *)((i8 *)&diffeng_map_known4_section))->init((iptr)map_data.known[4],(int)map.area,0x12);
  ((DiffEngSection *)((i8 *)&diffeng_map_known5_section))->init((iptr)map_data.known[5],(int)map.area,0x13);
  ((DiffEngSection *)((i8 *)&diffeng_map_known6_section))->init((iptr)map_data.known[6],(int)map.area,0x14);
  ((DiffEngSection *)((i8 *)&diffeng_map_known7_section))->init((iptr)map_data.known[7],(int)map.area,0x15);
  ((DiffEngSection *)((i8 *)&diffeng_map_tiles_section))->init((iptr)map_data.tiles,map.area * 6,0x16);
  ((DiffEngSection *)((i32 *)&diffeng_events_section_block.diffeng_events_section))->init((*(iptr *)&unit_types_block.events_pool_base),50000,0x17);
  offset = 0;
  for (section = 0; section < 0x17; section++) {
    diffeng_section_table[section].mirror_offset = offset;
    offset += (i32)diffeng_section_table[section].padded_size;
  }
  return;
}

/* 004b24a2 auto */
/* The decoded size of a run-length coded stream (diffeng_rle_encode): a code
   below 0x8000 is a run of that many copies of the one byte after it, else
   a literal of (code & 0x7fff) bytes. (BF-092) A code cut short at the end
   is not counted: the original read past the stream for it. */
int __cdecl diffeng_rle_decoded_size(RleStream *stream)

{
  uint code;
  int total;
  ushort *src;
  
  src = (ushort *)stream->src;
  total = 0;
  while (2 <= (int)stream->src_end - (int)src) {
    code = (uint)*src;
    if (code < 0x8000) {
      src = (ushort *)((int)src + 3);
    }
    else {
      src = (ushort *)((int)src + (code & 0x7fff) + 2);
    }
    total += (code & 0x7fff);
  }
  return total;
}

/* 004b251a auto */
/* (BF-092) The original took code 0x7fff, the encoder's run of 32767 bytes,
   for a 65535-byte literal, past what the size above counted, and checked no
   code against the stream's ends: decoded here as the size counts it, and a
   code that does not fit the stream or the output fails (0). Valid streams
   decode as before: the encoder writes no literal code 0x7fff (0xffff is the
   32767-byte literal). */
i32 __cdecl diffeng_rle_decode(RleStream *stream)

{
  ushort *payload;
  ushort run_len;
  uint decoded_size;
  i32 result;
  ushort *src;
  void *dst;
  uint src_left;
  
  decoded_size = diffeng_rle_decoded_size(stream);
  if ((uint)((int)stream->dst_end - (int)stream->dst) < decoded_size) {
    result = 0;
  }
  else {
    src = (ushort *)stream->src;
    dst = (void *)stream->dst;
    while (src < (ushort *)stream->src_end) {
      src_left = (uint)((int)stream->src_end - (int)src);
      if (src_left < 2) {
        return 0;
      }
      run_len = *src;
      payload = src + 1;
      if (run_len < 0x8000) {
        if (src_left < 3) {
          return 0;
        }
      }
      else {
        run_len &= 0x7fff;
        if (src_left - 2 < (uint)run_len) {
          return 0;
        }
      }
      if ((uint)((int)stream->dst_end - (int)dst) < (uint)run_len) {
        return 0;
      }
      if (*src < 0x8000) {
        src = (ushort *)((int)src + 3);
        _memset(dst,(int)*(byte *)payload,(uint)run_len);   /* (the run's byte: one byte, not a ushort) */
      }
      else {
        crt_memcpy(dst,payload,(uint)run_len);
        src = (ushort *)((int)payload + (uint)run_len);
      }
      dst = (void *)((int)dst + (uint)run_len);
    }
    stream->out_len = (int)dst - (int)stream->dst;
    result = 1;
  }
  return result;
}

/* 004b263e auto */
i32 __cdecl diffeng_rle_encode(RleStream *stream)

{
  short *code_ptr;
  uint src_len;
  uint dst_capacity;
  char *scan;
  char *run_start;
  short *dst;
  uptr chunk_len;
  uint run_len;
  uint out_len;
  
  dst = (short *)stream->dst;
  run_start = (char *)stream->src;
  scan = run_start + 1;
  src_len = (int)stream->src_end - (int)stream->src;
  out_len = 0;
  dst_capacity = (int)stream->dst_end - (int)stream->dst;
  do {
    if ((char *)stream->src_end <= run_start) {
      stream->out_len = out_len;
      return 1;
    }
    for (; (scan < (char *)stream->src_end && (*scan == *run_start)); scan = scan + 1
        ) {
    }
    run_len = (int)scan - (int)run_start;
    if (3 < run_len) {
      while (code_ptr = dst, run_len != 0) {
        if ((dst_capacity < out_len + 2) || (src_len <= out_len + 2)) {
          return 0;
        }
        if (run_len < 0x8000) {
          *dst = (short)run_len;
          run_len = 0;
        }
        else {
          *dst = 0x7fff;
          run_len -= 0x7fff;
        }
        dst++;
        out_len += 3;
        if ((dst_capacity < out_len) || (src_len <= out_len)) {
          return 0;
        }
        *(char *)dst = *run_start;
        dst = (short *)((int)code_ptr + 3);
      }
      run_start = scan;
    }
    /* (the original compared the last byte with the one after the input;
       the end ends the literal here instead: the same decoded bytes) */
    for (; (scan < (char *)stream->src_end && (scan + 1 == (char *)stream->src_end || scan[1] != *scan));
        scan = scan + 1) {
    }
    run_len = (int)scan - (int)run_start;
    while (run_len != 0) {
      out_len += 2;
      if ((dst_capacity < out_len) || (src_len <= out_len)) {
        return 0;
      }
      if (run_len < 0x8000) {
        *dst = (short)run_len - 0x8000;
        chunk_len = run_len;
        run_len = 0;
      }
      else {
        *dst = -1;
        chunk_len = 0x7fff;
        run_len -= 0x7fff;
      }
      dst++;
      out_len += chunk_len;
      if ((dst_capacity < out_len) || (src_len <= out_len)) {
        return 0;
      }
      crt_memcpy(dst,run_start,chunk_len);
      dst = (short *)((int)dst + chunk_len);
      run_start += chunk_len;
    }
  } while( true );
}

/* 004b3080 auto */
void DiffEngSection::init(iptr data_ptr,u32 data_size,u32 section_id)

{
  this->data_ptr = data_ptr;
  this->data_size = data_size;
  this->section_id = section_id;
  checksum = 0;
  padded_size = this->data_size;
  if ((this->data_size & 3) != 0) {
    padded_size = (4 - (this->data_size & 3)) + padded_size;
  }
  return;
}

/* 004c4280 auto */
void __cdecl game_main_entry(void)

{
  game_main();
  return;
}

/* 004ce903 auto */
int __cdecl events_alloc_string(char *text,iptr events,uint min_len)

{
  size_t len;
  int str;
  i32 alloc_len;
  
  len = _strlen(text);
  if ((int)min_len < (int)(len + 1)) {
    len = _strlen(text);
    alloc_len = len + 1;
  }
  else {
    alloc_len = min_len;
  }
  str = memblock_alloc((MemBlock *)(events + 0x2f4),alloc_len);
  if (str == 0) {
    error_report(0xfffffff7,3,0,0,0);
  }
  crt_strcpy((char *)((void *)str),text);
  return str;
}

/* 004ce98e auto */
void __cdecl events_copy(ScenarioEvents *dst_events,ScenarioEvents *src_events)

{
  ScenarioEvent *new_event;
  i32 str;
  ScenarioEvent *dst_event;
  ScenarioEvent *src_event;
  int i;
  
  for (src_event = src_events->event_list; src_event != 0; src_event = src_event->next) {
    new_event = dst_events->alloc();
    if (new_event == 0) {
      error_report(0xfffffff7,3,0,0,0);
    }
    crt_memcpy((void *)new_event,(const void *)src_event,0x1bc);
  }
  src_event = src_events->event_list;
  dst_event = dst_events->event_list;
  do {
    if (src_event == 0) {
      return;
    }
    if (src_event->unit_name != 0) {
      str = events_alloc_string(src_event->unit_name,(iptr)dst_events,0xf);
      dst_event->unit_name = (char *)str;
      if (dst_event->unit_name == 0) {
        return;
      }
    }
    if (src_event->city_name != 0) {
      str = events_alloc_string(src_event->city_name,(iptr)dst_events,0x18);
      dst_event->city_name = (char *)str;
      if (dst_event->city_name == 0) {
        return;
      }
    }
    if (src_event->who_name != 0) {
      str = events_alloc_string(src_event->who_name,(iptr)dst_events,0x18);
      dst_event->who_name = (char *)str;
      if (dst_event->who_name == 0) {
        return;
      }
    }
    if (src_event->whom_name != 0) {
      str = events_alloc_string(src_event->whom_name,(iptr)dst_events,0x18);
      dst_event->whom_name = (char *)str;
      if (dst_event->whom_name == 0) {
        return;
      }
    }
    for (i = 0; (i < 0x14 && (src_event->text[i] != 0));
        i = i + 1) {
      str = events_alloc_string(src_event->text[i],(iptr)dst_events,1);
      dst_event->text[i] = (char *)str;
      if (dst_event->text[i] == 0) {
        return;
      }
    }
    if (src_event->move_owner_name != 0) {
      str = events_alloc_string(src_event->move_owner_name,(iptr)dst_events,0x18);
      dst_event->move_owner_name = (char *)str;
      if (dst_event->move_owner_name == 0) {
        return;
      }
    }
    if (src_event->move_unit_name != 0) {
      str = events_alloc_string(src_event->move_unit_name,(iptr)dst_events,0xf);
      dst_event->move_unit_name = (char *)str;
      if (dst_event->move_unit_name == 0) {
        return;
      }
    }
    if (src_event->aggression_whom_name != 0) {
      str = events_alloc_string(src_event->aggression_whom_name,(iptr)dst_events,0x18);
      dst_event->aggression_whom_name = (char *)str;
      if (dst_event->aggression_whom_name == 0) {
        return;
      }
    }
    if (src_event->aggression_who_name != 0) {
      str = events_alloc_string(src_event->aggression_who_name,(iptr)dst_events,0x18);
      dst_event->aggression_who_name = (char *)str;
      if (dst_event->aggression_who_name == 0) {
        return;
      }
    }
    if (src_event->create_owner_name != 0) {
      str = events_alloc_string(src_event->create_owner_name,(iptr)dst_events,0x18);
      dst_event->create_owner_name = (char *)str;
      if (dst_event->create_owner_name == 0) {
        return;
      }
    }
    if (src_event->create_unit_name != 0) {
      str = events_alloc_string(src_event->create_unit_name,(iptr)dst_events,0xf);
      dst_event->create_unit_name = (char *)str;
      if (dst_event->create_unit_name == 0) {
        return;
      }
    }
    if (src_event->create_homecity != 0) {
      str = events_alloc_string(src_event->create_homecity,(iptr)dst_events,0x18);
      dst_event->create_homecity = (char *)str;
      if (dst_event->create_homecity == 0) {
        return;
      }
    }
    if (src_event->field_140 != 0) {
      str = events_alloc_string(src_event->field_140,(iptr)dst_events,0x18);
      dst_event->field_140 = (char *)str;
      if (dst_event->field_140 == 0) {
        return;
      }
    }
    if (src_event->field_148 != 0) {
      str = events_alloc_string(src_event->field_148,(iptr)dst_events,0xf);
      dst_event->field_148 = (char *)str;
      if (dst_event->field_148 == 0) {
        return;
      }
    }
    if (src_event->money_receiver_name != 0) {
      str = events_alloc_string(src_event->money_receiver_name,(iptr)dst_events,0x18);
      dst_event->money_receiver_name = (char *)str;
      if (dst_event->money_receiver_name == 0) {
        return;
      }
    }
    if (src_event->wave_file != 0) {
      str = events_alloc_string(src_event->wave_file,(iptr)dst_events,1);
      dst_event->wave_file = (char *)str;
      if (dst_event->wave_file == 0) {
        return;
      }
    }
    src_event = src_event->next;
    dst_event = dst_event->next;
  } while( true );
}

/* (port) ScenarioEvents::reset (004fa5d9) for events_compact: the pool
   frees its block and allocates one of the same size, which under Wine is
   the block just freed, at the same address (seen in the events replay:
   the editor's text lines go there).  The original relies on it: its
   callers keep event pointers across events_compact (editor_text_dlg_on_ok
   writes the text lines through editor_lists.text_lines_out,
   events_editor_edit_action reads event->actions), and they stay valid
   while compacting does not move the event.  So the pool keeps its block
   here, cleared as GMEM_ZEROINIT clears a new one, and unlocked as
   memblock_free leaves it. */
static void events_reset_keep_block(ScenarioEvents *events,uint pool_size)
{
  MemBlock *pool;

  pool = (MemBlock *)events->events_string_pool;
  if ((char)pool->owns_handle == '\0' || pool->mem_handle == 0 || pool->capacity != (pool_size & 0xffff)) {
    events->reset(pool_size);
    return;
  }
  events->event_list = 0;
  events->event_count = 0;
  memblock_ensure_locked(pool);
  _memset((void *)pool->base_ptr,0,pool->capacity);
  memblock_unlock(pool);
  pool->block_id = 0xc;
  pool->used = 0;
  pool->available = (u16)pool_size;
  pool->capacity = (u16)pool_size;
}

/* 004cef35 auto */
void __cdecl events_compact(void)

{
  ScenarioEvents temp_events;
  i32 eh_next;
  
  temp_events.construct(50000);
  ON_SCOPE_EXIT(temp_events.destruct());
  temp_events.reset(50000);
  events_copy(&temp_events,(ScenarioEvents *)&unit_types_block.scenario_events);
  events_reset_keep_block((ScenarioEvents *)&unit_types_block.scenario_events,50000);
  events_copy((ScenarioEvents *)&unit_types_block.scenario_events,&temp_events);
  return;
}

/* 004cefe9 auto */
int __cdecl events_rename_unit_refs(char *old_name,iptr new_name)

{
  int cmp;
  ScenarioEvent *event;
  i32 count;
  
  count = 0;
  for (event = unit_types_block.events_list_head; event != 0; event = event->next) {
    if (event->unit_name != 0) {
      cmp = _strcmp(event->unit_name,old_name);
      if (cmp == 0) {
        _strncpy(event->unit_name,(char *)new_name,0xf);
        count++;
      }
    }
    if (event->move_unit_name != 0) {
      cmp = _strcmp(event->move_unit_name,old_name);
      if (cmp == 0) {
        _strncpy(event->move_unit_name,(char *)new_name,0xf);
        count++;
      }
    }
    if (event->create_unit_name != 0) {
      cmp = _strcmp(event->create_unit_name,old_name);
      if (cmp == 0) {
        _strncpy(event->create_unit_name,(char *)new_name,0xf);
        count++;
      }
    }
    if (event->field_148 != 0) {
      cmp = _strcmp(event->field_148,old_name);
      if (cmp == 0) {
        _strncpy(event->field_148,(char *)new_name,0xf);
        count++;
      }
    }
  }
  return count;
}

/* 004e0140 auto */
void __cdecl main_menu_global_init(void)

{
  main_menu_static_init();
  main_menu_reg_atexit();
  return;
}

/* 004e015a auto */
void __cdecl main_menu_static_init(void)

{
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->construct(0x1000);
  return;
}

/* 004e0179 auto */
void __cdecl main_menu_reg_atexit(void)

{
  _atexit(main_menu_static_dtor);
  return;
}

/* 004e0196 auto */
void __cdecl main_menu_static_dtor(void)

{
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->destruct();
  return;
}

/* (port) A game is being played: it has units or cities.  The intro, the
   main menu and a new game's set-up have none (the main loop clears both
   before its menu). */
extern "C" int game_in_play(void)

{
  return (0 < game.num_units) || (0 < game.num_cities);
}

/* 004e22c9 auto */
void __cdecl game_quit_or_retire(int retire)

{
  int confirmed;
  char *text;
  u8 saved_human_players2;
  
  /* (port) Without a game, Quit ends the program at once and Retire does
     nothing: the original asked "really quit?" even over the intro. */
  if (game_in_play() == 0) {
    if (retire == 0) {
      exit(0);
    }
    return;
  }
  if ((game.multiplayer < 3) || (net.host_handoff_stage == '\0')) {
    if (retire == 0) {
      confirmed = popup_game_with_sprite((const char *)&str_reallyquit_3,(iptr)&popup_info_icon_sprite,0);
      if (confirmed == 0) {
        return;
      }
    }
    else {
      confirmed = popup_game_with_sprite((const char *)&str_reallyretire,(iptr)&popup_info_icon_sprite,0);
      if (confirmed == 0) {
        return;
      }
      powergraph_show();
      if ((game.game_flags & GAME_OVER_SCORED) == 0) {
        civ_score_window_show((*(i8 *)&human_civ));
        civ_rating_window_show((*(i8 *)&human_civ));
        hall_of_fame_add_entry((*(i8 *)&human_civ));
      }
    }
    if (game.multiplayer == '\0') {
      snd_cd_stop();
      map_display_disable();
    }
    else {
      if (game.multiplayer == 1) {
        saved_human_players2 = game.human_players;
        game.human_players = net.hotseat_human_players;
      }
      if (2 < game.multiplayer) {
        if (mp_timer.handle != 0) {
          game_timer_destroy(mp_timer.handle);
          mp_timer.handle = 0;
        }
        parley_wnd_close();
        chat_wnd_close();
        text = (char *)civ_get_adjective((*(i8 *)&human_civ));
        text_set_string_arg(0,text);
        text = (char *)civ_leader_title((*(i8 *)&human_civ));
        text_set_string_arg(1,text);
        text = (civ_leader_name((*(i8 *)&human_civ)));
        text_set_string_arg(2,text);
        text = (civ_name_plural((*(i8 *)&human_civ)));
        text_set_string_arg(3,text);
        if (mp_options.kill_civ_on_retire == 0) {
          net_send_text_args(1,0xff,4,0,0,0);
        }
        else {
          net_send_text_args(0,0xff,4,0,0,0);
        }
      }
      blink.unit_phase = 0;
      mapviews_redraw_cursor();
      if ((~(1 << ((byte)(*(i8 *)&human_civ) & 0x1f)) & (uint)game.human_players) == 0) {
        if (game.multiplayer < 3) {
          game.human_players = game.human_players & ~(byte)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
        }
        else {
          net_send_message(NM_UNLOCK_NATION,0,0,(*(i8 *)&human_civ),0,0,0,0,0,0);
        }
        map_display_disable();
        snd_cd_stop();
      }
      else {
        civ_eliminate((*(i8 *)&human_civ),0,0);
        if (2 < game.multiplayer) {
          map_display_disable();
        }
      }
      player_turn_active = 0;
      net.player_has_quit = 1;
      if (game.multiplayer == 1) {
        net.hotseat_human_players = game.human_players;
        game.human_players = saved_human_players2;
      }
    }
  }
  return;
}

/* 004fa1c0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl game_palette_global_init(void)

{
  game_palette_static_init();
  game_palette_reg_atexit();
  return;
}

/* 004fa1da auto */
void __cdecl game_palette_static_init(void)

{
  ((GamePalette *)((i8 *)&game_palette_block.game_palette))->init();
  return;
}

/* 004fa1f4 auto */
void __cdecl game_palette_reg_atexit(void)

{
  _atexit(game_palette_atexit_dtor);
  return;
}

/* 004fa211 auto */
void __cdecl game_palette_atexit_dtor(void)

{
  ((GamePalette *)((i8 *)&game_palette_block.game_palette))->destroy();
  return;
}

/* 004fa250 auto */
i32 __cdecl events_parse_civ(const char *name2)

{
  int cmp;
  i32 result;
  char *_Str2;
  int civ;
  
  cmp = __strcmpi((char *)name2,str_anybody);
  if (cmp == 0) {
    result = 0xfffffffe;
  }
  else {
    cmp = __strcmpi((char *)name2,str_triggerattacker);
    if (cmp == 0) {
      result = 0xfffffffd;
    }
    else {
      cmp = __strcmpi((char *)name2,str_triggerdefender);
      if (cmp == 0) {
        result = 0xfffffffc;
      }
      else {
        cmp = __strcmpi((char *)name2,str_triggerreceiver);
        if (cmp == 0) {
          result = 0xfffffffc;
        }
        else {
          for (civ = 0; civ < 8; civ++) {
            _Str2 = civ_name_plural(civ);
            cmp = __strcmpi((char *)name2,_Str2);
            if (cmp == 0) {
              return civ;
            }
          }
          if (events_parse_verbose != 0) {
            _printf(str_could_not_find_s_check_spelling,name2);
          }
          result = 0xffffffff;
        }
      }
    }
  }
  return result;
}

/* 004fa359 auto */
i32 __cdecl events_parse_unit_type(const char *name2)

{
  int cmp;
  i32 result;
  char *_Str2;
  iptr unit_type;
  
  cmp = __strcmpi((char *)name2,str_anyunit);
  if (cmp == 0) {
    result = 0xfffffffe;
  }
  else {
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      _Str2 = label_string(unit_types[unit_type].name);
      cmp = __strcmpi((char *)name2,_Str2);
      if (cmp == 0) {
        return unit_type;
      }
    }
    if (events_parse_verbose != 0) {
      _printf(str_could_not_find_unit_s_check_spelling,name2);
    }
    result = 0xffffffff;
  }
  return result;
}

/* 004fa47e auto */
void ScenarioEvents::init_string_pool(uint pool_size)

{
  memblock_free((MemBlock *)events_string_pool);
  memblock_init((MemBlock *)events_string_pool,0xc,pool_size);
  return;
}

/* 004fa4be auto */
ScenarioEvents *ScenarioEvents::construct(i32 pool_size)

{
  i32 eh_next;
  
  Dialog::construct(0x4000);
  ((MemBlock *)events_string_pool)->construct();
  events_pool_valid = '\0';
  event_list = 0;
  event_count = 0;
  reset(pool_size);
  return this;
}

/* 004fa569 auto */
void ScenarioEvents::destruct()

{
  i32 eh_next;
  i32 eh_state;
  
  eh_state = 0;
  memblock_free((MemBlock *)events_string_pool);
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004fa5b8 */ { Dialog::destruct(); }
  return;
}

/* 004fa5d9 auto */
void ScenarioEvents::reset(i32 pool_size)

{
  event_list = 0;
  event_count = 0;
  init_string_pool(pool_size);
  return;
}

/* 004fa617 auto */
ScenarioEvent *ScenarioEvents::alloc()

{
  ScenarioEvent *_Dst;
  ScenarioEvent *last;
  
  last = event_list;
  if (last != 0) {
    for (; (int)last->next != 0; last = last->next) {
    }
  }
  _Dst = (ScenarioEvent *)(void *)memblock_alloc((MemBlock *)events_string_pool,0x1c4);
  if (_Dst == (void *)0x0) {
    _Dst = (ScenarioEvent *)(void *)0x0;
  }
  else {
    _memset(_Dst,0,0x1c4);
    if (last == 0) {
      event_list = _Dst;
      _Dst->next = 0;
      _Dst->prev = 0;
    }
    else {
      last->next = _Dst;
      _Dst->prev = last;
      _Dst->next = 0;
    }
    event_count++;
  }
  return _Dst;
}

/* 004fa707 auto */
i32 ScenarioEvents::action_play_wave(ScenarioEvent *event)

{
  i32 result;
  int exists;
  i8 path[0x208];  /* (port) 240 in the original: scenario_dir_path (264), SOUND\
                      and the event's file name (up to 255, BF-296) */
  
  if (((byte)game.game_options & OPT_SOUND) == 0) {
    result = 0;
  }
  else {
    if (event->wave_file != 0) {
      crt_strcpy(path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
      crt_strcat(path,str_sound_2);
      crt_strcat(path,(const char *)((const void *)event->wave_file));
      exists = file_exists((const char *)path);
      if (exists == 0) {
        crt_strcpy(path,((i8 *)&game_dirs.home));
        crt_strcat(path,str_sound_3);
        crt_strcat(path,(const char *)((const void *)event->wave_file));
        exists = file_exists((const char *)path);
        if (exists != 0) {
          snd_wave_play((const char *)path,0,0,0);
        }
      }
      else {
        snd_wave_play((const char *)path,0,0,0);
      }
    }
    result = 1;
  }
  return result;
}

/* 004fa82d auto */
i32 ScenarioEvents::action_dont_play_wonders(ScenarioEvent *event)

{
  game_palette_block.event_dont_play_wonders = 1;
  return 1;
}

/* 004fa854 auto */
i32 ScenarioEvents::action_play_cd(ScenarioEvent *event)

{
  i32 result;
  int track_count;
  int attempts;
  
  attempts = 0;
  while (attempts < 10) {
    attempts++;
    track_count = snd_cd_get_track_count();
    if ((((track_count == 0x18) || (track_count == 0x12)) || (track_count == 0xc)) || (track_count == 10)) break;
    snd_cd_close();
  }
  if (((track_count == 10) || (track_count == 0x12)) || ((track_count == 0x18 || (track_count == 0xc)))) {
    if ((event->cd_track < 2) || (track_count < event->cd_track)) {
      result = 0;
    }
    else {
      music_play_track(event->cd_track,1);
      result = 1;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004fa944 auto */
i32 ScenarioEvents::action_change_money(ScenarioEvent *event)

{
  i32 result;
  int new_gold;
  iptr civ;
  
  if (event->money_receiver_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->money_receiver_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->money_receiver_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->money_receiver_civ;
  }
  if ((civ < 1) || (7 < civ)) {
    result = 0;
  }
  else if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
    result = 0;
  }
  else {
    new_gold = event->money_amount + civs[civ].gold;
    if ((new_gold < 0) || (extras_gold_limit() < new_gold)) {  /* X52: 30000 unless set */
      if (event->money_amount < 0) {
        civs[civ].gold = 0;
      }
      else {
        civs[civ].gold = extras_gold_limit();
      }
    }
    else {
      civs[civ].gold = new_gold;
    }
    result = 1;
  }
  return result;
}

/* 004faab0 auto */
i32 ScenarioEvents::action_text(ScenarioEvent *event)

{
  int line;
  int arg_count;
  
  arg_count = 0;
  init_default();
  for (line = 0; line < 0x14; line++) {
    if (event->text[line] != 0) {
      add_line((const char *)event->text[line],0xffffffff,0xffffffff,
                 0xffffffff,0);
      if (2 < game.multiplayer) {
        crt_strcpy(((i8 *)&text_args.strings) + arg_count * 0x104,(const char *)event->text[line]);
        arg_count++;
      }
    }
  }
  if (autoplay.active == 0) {
    if (2 < game.multiplayer) {
      net_send_text_args(0x4f,0xff,arg_count,0,0,0);
    }
    show_modal(0);
  }
  return 1;
}

/* 004faba6 auto */
i32 ScenarioEvents::action_make_aggression(ScenarioEvent *event)

{
  i32 result;
  int civ2;
  int civ;
  
  if (event->aggression_who_civ == -3) {
    civ2 = event->who_civ;
  }
  else if (event->aggression_who_civ == -4) {
    civ2 = event->whom_civ;
  }
  else if (event->aggression_who_civ == -4) {
    civ2 = event->whom_civ;
  }
  else {
    civ2 = event->aggression_who_civ;
  }
  if (event->aggression_whom_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->aggression_whom_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->aggression_whom_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->aggression_whom_civ;
  }
  if (((civ < 1) || (7 < civ)) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else if (((civ2 < 1) || (7 < civ2)) ||
          ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else {
    result = diplo_force_war(civ2,civ);
  }
  return result;
}

/* 004fad02 auto */
i32 ScenarioEvents::action_destroy_civ(ScenarioEvent *event)

{
  i32 result;
  int civ;
  
  if (event->destroy_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->destroy_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->destroy_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->destroy_civ;
  }
  if (((civ < 1) || (7 < civ)) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
       (human_civ[0] == civ)) {
      game_over_reason = 4;
    }
    civ_eliminate(civ,1,1);
    result = 1;
  }
  return result;
}

/* 004fadfb auto */
i32 ScenarioEvents::action_give_tech(ScenarioEvent *event)

{
  i32 result;
  int civ;
  
  if (event->give_tech_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->give_tech_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->give_tech_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->give_tech_civ;
  }
  if (((civ < 1) || (7 < civ)) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else {
    civ_give_advance(civ,event->give_tech,0,0,0);
    result = 1;
  }
  return result;
}

/* 004faed4 auto */
i32 ScenarioEvents::action_create_unit(ScenarioEvent *event)

{
  iptr x;
  iptr y;
  i32 result;
  iptr in_bounds;
  iptr is_ocean;
  uptr owner;
  iptr unit;
  int city2;
  int loc;
  int y2;
  int x2;
  uptr civ;
  iptr city;
  
  x2 = 10000;
  y2 = 10000;
  if (event->create_owner_civ == -3) {
    civ = (uptr)event->who_civ;
  }
  else if (event->create_owner_civ == -4) {
    civ = (uptr)event->whom_civ;
  }
  else if (event->create_owner_civ == -4) {
    civ = (uptr)event->whom_civ;
  }
  else {
    civ = (uptr)event->create_owner_civ;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
    result = 0;
  }
  else {
    for (loc = 0; y = y2, x = x2, loc < event->create_location_count;
        loc = loc + 1) {
      x = event->create_locations[loc * 2];
      y = event->create_locations[loc * 2 + 1];
      in_bounds = map_in_bounds(x,y);
      if (in_bounds != 0) {
        if (unit_types[event->create_unit_type].domain == DOMAIN_SEA) {
          is_ocean = tile_is_ocean(x,y);
          if (is_ocean != 0) break;
          city2 = city_at(x,y);
          if ((city2 != -1) &&
             (city2 = city_at(x,y), (cities[city2].flags & CITYF_COASTAL) != 0)) {
            owner = tile_city_owner(x,y);
            if (owner == civ) break;
          }
        }
        else {
          city2 = tile_is_ocean(x,y);
          if (((city2 == 0) || (unit_types[event->create_unit_type].domain == DOMAIN_AIR)) &&
             ((city2 = city_at(x,y), city2 == -1 ||
              (owner = tile_city_owner(x,y), owner == civ)))) {
            city2 = tile_unit_owner(x,y);
            if (city2 == -1) break;
            owner = tile_unit_owner(x,y);
            if (owner == civ) break;
          }
        }
      }
    }
    x2 = x;
    y2 = y;
    if ((x2 == 10000) && (y2 == 10000)) {
      result = 0;
    }
    else {
      unit = unit_create(event->create_unit_type,civ,x2,y2);
      if (unit == -1) {
        result = 0;
      }
      else {
        if (event->create_veteran != 0) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
        units[unit].home_city = -1;
        if (event->create_homecity != 0) {
          for (city = 0; city < game.num_cities; city++) {
            if ((cities[city].id != 0) &&
               (city2 = __strcmpi(cities[city].name,event->create_homecity), city2 == 0)) {
              if ((int)cities[city].owner == civ) {
                units[unit].home_city = (i8)city;
              }
              break;
            }
          }
        }
        result = 1;
      }
    }
  }
  return result;
}

/* 004fb29f auto */
int ScenarioEvents::action_move_unit(ScenarioEvent *event)

{
  int x;
  int y;
  int in_bounds;
  iptr unit;
  int moved;
  iptr i;
  int civ;
  int rect_y[4];
  int rect_x[4];
  
  if (event->move_owner_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->move_owner_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->move_owner_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->move_owner_civ;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
    moved = 0;
  }
  else if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    for (i = 0; i < 4; i++) {
      rect_x[i] = event->move_rect[i * 2];
      rect_y[i] = event->move_rect[i * 2 + 1];
    }
    moved = 0;
    unit = (iptr)game.num_units;
    do {
      do {
        do {
          unit--;
          if (unit < 0) {
            return moved;
          }
        } while ((((units[unit].id == 0) ||
                  ((((event->move_unit_type != (bugfix_state() ? (iptr)units[unit].type : unit) &&  /* B23: the unit's type, not its slot (docs/BUGS.md) */
                     (event->move_unit_type != -2)) ||
                    (units[unit].owner != civ)) ||
                   (('b' < (char)unit_types[units[unit].type].attack ||
                    (units[unit].orders == ORDER_AI_IDLE)))))) || (3 < units[unit].orders)) ||
                (units[unit].orders == ORDER_SLEEP));
        x = (int)units[unit].x;
        y = (int)units[unit].y;
        in_bounds = map_in_bounds(x,y);
      } while ((((in_bounds == 0) || (x < rect_x[0])) ||
               ((rect_x[1] < x ||
                (((rect_x[2] < x || (x < rect_x[3])) || (y < rect_y[0])))))) ||
              (((y < rect_y[1] || (rect_y[2] < y)) || (rect_y[3] < y))));
      units[unit].orders = ORDER_GOTO;
      units[unit].ai_task = '7';
      units[unit].goto_x = (i16)event->move_to_x;
      units[unit].goto_y = (i16)event->move_to_y;
      moved++;
    } while ((moved < event->move_count) || (event->move_count == -2));
  }
  else {
    moved = 0;
  }
  return moved;
}

/* 004fb5b2 auto */
int ScenarioEvents::action_change_terrain(ScenarioEvent *event)

{
  bool has_city;
  iptr in_bounds;
  uptr city2;
  iptr in_bounds2;
  i8 *terrain;
  Tile *tile;
  int civ;
  iptr unit;
  iptr city;
  iptr i;
  int y2;
  int changed;
  int x2;
  int rect_y[4];
  int rect_x[4];
  
  changed = 0;
  for (i = 0; i < 4; i++) {
    rect_x[i] = event->terrain_rect[i * 2];
    rect_y[i] = event->terrain_rect[i * 2 + 1];
  }
  for (x2 = rect_x[0]; x2 <= rect_x[1]; x2++) {
    for (y2 = rect_y[0]; y2 <= rect_y[3]; y2++) {
      in_bounds = map_in_bounds(x2,y2);
      if (in_bounds != 0) {
        city2 = city_at(x2,y2);
        if (-1 < (int)city2) {
          city_delete(city2,0);
        }
        for (unit = tile_first_unit(x2,y2); -1 < unit;
            unit = unit_next_in_stack(unit)) {
          unit_delete(unit,1);
        }
      }
    }
  }
  for (x2 = rect_x[0]; x2 <= rect_x[1]; x2++) {
    for (y2 = rect_y[0]; y2 <= rect_y[3]; y2++) {
      in_bounds2 = map_in_bounds(x2,y2);
      if (in_bounds2 != 0) {
        terrain = (i8 *)tile_at(x2,y2);
        *terrain = (i8)event->terrain_type;
        tile = tile_at(x2,y2);
        tile->improvements = 0;
        tile = tile_at(x2,y2);
        tile->radius = 0;
        tile = tile_at(x2,y2);
        tile->landmass = 0;
        for (civ = 0; civ < 8; civ++) {
          tile_update_known(x2,y2,civ);
        }
        map_redraw_tile_all_views(x2,y2);
        changed++;
      }
    }
  }
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      city_refresh_area_tiles(city);
    }
  }
  for (civ = 0; civ < 8; civ++) {
    has_city = false;
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        has_city = true;
      }
    }
    if (!has_city) {
      unit = (iptr)game.num_units;
      while (unit = unit - 1, -1 < unit) {
        if ((units[unit].id != 0) && (units[unit].owner == civ)) {
          unit_delete(unit,1);
        }
      }
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         (human_civ[0] == civ)) {
        game_over_reason = 4;
      }
      for (x2 = 0; x2 < map.width; x2++) {
        for (y2 = 0; y2 < map.height; y2++) {
          tile = tile_at(x2,y2);
          tile->visible = tile->visible & ~(byte)(1 << ((byte)civ & 0x1f));
        }
      }
      civ_create(civ);
      map_redraw_all_views(human_civ[0],1);
    }
  }
  return changed;
}

/* 004fba0c auto */
i32 ScenarioEvents::check_turn(int turn)

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((event->trigger == 4) && ((event->turn == -1 || (event->turn == turn)))) {
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fba9c auto */
i32 ScenarioEvents::check_turn_interval(int turn)

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((event->trigger == 8) && ((event->turn == 0 || (turn % event->turn == 0)))) {
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbb2f auto */
i32 ScenarioEvents::check_random_turn(i32 turn)

{
  int rnd;
  int roll;
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if (event->trigger == 0x40) {
        if (event->denominator < 2) {
          roll = 1;
        }
        else {
          rnd = crt_rand_at("events_check_random_turn");
          roll = rnd % event->denominator + 1;
        }
        if (event->denominator == roll) {
          triggered = 1;
          run_actions(event);
        }
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbbdd auto */
i32 ScenarioEvents::check_received_tech()

{
  int has_tech;
  int civ;
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if (event->trigger == 0x100) {
        if (event->whom_civ == -2) {
          for (civ = 0; civ < 8; civ++) {
            if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
               (has_tech = civ_has_advance(civ,event->technology), has_tech != 0)) {
              event->whom_civ = civ;
              triggered = 1;
              run_actions(event);
            }
          }
        }
        else if ((((0 < event->whom_civ) || (event->whom_civ < 8)) &&
                 ((1 << ((byte)event->whom_civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
                (has_tech = civ_has_advance(event->whom_civ,event->technology), has_tech != 0)) {
          triggered = 1;
          run_actions(event);
        }
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbd2b auto */
i32 ScenarioEvents::check_scenario_loaded()

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if (event->trigger == 0x20) {
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbd9d auto */
i32 ScenarioEvents::check_unit_killed(int unit_type,int attacker_civ,int defender_civ)

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((((event->trigger == 1) && (event->unit_type == unit_type)) &&
          ((event->whom_civ == defender_civ || (event->whom_civ == -2)))) &&
         ((event->who_civ == attacker_civ || (event->who_civ == -2)))) {
        triggered = 1;
        if (event->who_civ == -2) {
          event->who_civ = attacker_civ;
        }
        if (event->whom_civ == -2) {
          event->whom_civ = defender_civ;
        }
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbe84 auto */
i32 ScenarioEvents::check_negotiation(iptr talker_civ,iptr listener_civ)

{
  ScenarioEvent *event;
  bool talker_match;
  bool listener_match;
  
  for (event = event_list; ; event = event->next) {
    if (event == (ScenarioEvent *)0x0) {
      return 1;
    }
    if (event->trigger == 0x10) {
      if (((event->who_civ == talker_civ) || (event->who_civ == -2)) &&
         ((((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) != 0 &&
           ((event->talker_type == 1 || (event->talker_type == 4)))) ||
          (((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) == 0 &&
           ((event->talker_type == 2 || (event->talker_type == 4)))))))) {
        talker_match = true;
      }
      else {
        talker_match = false;
      }
      if (((event->whom_civ == listener_civ) || (event->whom_civ == -2)) &&
         (((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) != 0 &&
           ((event->listener_type == 1 || (event->listener_type == 4)))) ||
          ((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) == 0 &&
           ((event->listener_type == 2 || (event->listener_type == 4)))))))) {
        listener_match = true;
      }
      else {
        listener_match = false;
      }
      if (!((talker_match) && (listener_match))) {
        if (!((2 < game.multiplayer) &&
           (((1 << ((byte)talker_civ & 0x1f) & (uint)game.human_players) != 0 &&
            ((1 << ((byte)listener_civ & 0x1f) & (uint)game.human_players) != 0))))) continue;
        if (((event->who_civ == listener_civ) || (event->who_civ == -2)) &&
           ((((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) != 0 &&
             ((event->talker_type == 1 || (event->talker_type == 4)))) ||
            (((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) == 0 &&
             ((event->talker_type == 2 || (event->talker_type == 4)))))))) {
          talker_match = true;
        }
        else {
          talker_match = false;
        }
        if (((event->whom_civ == talker_civ) || (event->whom_civ == -2)) &&
           (((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) != 0 &&
             ((event->listener_type == 1 || (event->listener_type == 4)))) ||
            ((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) == 0 &&
             ((event->listener_type == 2 || (event->listener_type == 4)))))))) {
          listener_match = true;
        }
        else {
          listener_match = false;
        }
        if (!((talker_match) && (listener_match))) continue;
      }
      if (event->negotiation_flag == 0x1000) {
        run_actions(event);
        return 0;
      }
      run_actions(event);
      return 1;
    }
  }
}

/* 004fc20d auto */
i32 ScenarioEvents::check_no_schism(iptr civ)

{
  bool triggered;
  i32 result;
  ScenarioEvent *event;
  
  triggered = false;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    result = 1;
  }
  else {
    do {
      if ((event->trigger == 0x80) && ((event->whom_civ == civ || (event->whom_civ == -2)))) {
        run_actions(event);
        triggered = true;
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
    if (triggered) {
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
}

/* 004fc2bb auto */
i32 ScenarioEvents::check_city_taken(iptr city_name,iptr attacker_civ,iptr defender_civ)

{
  int cmp;
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((((event->trigger == 2) && (cmp = __strcmpi(event->city_name,(char *)city_name), cmp == 0))
          && ((event->who_civ == attacker_civ || (event->who_civ == -2)))) &&
         ((event->whom_civ == defender_civ || (event->whom_civ == -2)))) {
        if (event->who_civ == -2) {
          event->who_civ = attacker_civ;
        }
        if (event->whom_civ == -2) {
          event->whom_civ = defender_civ;
        }
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fc3ae auto */
i32 ScenarioEvents::run_actions(ScenarioEvent *event)

{
  i32 result;
  
  if (((event->actions_bytes[0] & 0x40) == 0) || ((event->actions_bytes[1] & 0x20) == 0)) {
    if ((event->actions_bytes[0] & 0x10) != 0) {
      action_play_wave(event);
    }
    if ((event->actions_bytes[0] & 0x80) != 0) {
      action_play_cd(event);
    }
    if ((event->actions_bytes[0] & 4) != 0) {
      action_create_unit(event);
    }
    if ((event->actions_bytes[0] & 2) != 0) {
      action_move_unit(event);
    }
    if ((event->actions_bytes[1] & 2) != 0) {
      action_change_terrain(event);
    }
    if ((event->actions_bytes[0] & 0x20) != 0) {
      action_make_aggression(event);
    }
    if ((event->actions_bytes[0] & 8) != 0) {
      action_change_money(event);
    }
    if ((event->actions_bytes[1] & 4) != 0) {
      action_destroy_civ(event);
    }
    if ((event->actions_bytes[1] & 8) != 0) {
      action_give_tech(event);
    }
    if ((event->actions_bytes[0] & 1) != 0) {
      action_text(event);
    }
    if ((event->actions_bytes[1] & 1) != 0) {
      action_dont_play_wonders(event);
    }
    if ((event->actions_bytes[0] & 0x40) != 0) {
      event->actions = event->actions | 0x2000;
    }
    result = 1;
  }
  else {
    result = 0;
  }
  return result;
}

/* The state of ScenarioEvents::parse_file (004fc516) shared by its parts:
   the event being read, the line just read and which of the event's fields
   were found (reset at each @IF). */
struct EventsParseState {
  int have_owner;
  int have_homecity;
  int have_city;
  int have_unit;
  int have_move_count;
  int have_tech;
  ScenarioEvent *event;
  int have_listener_type;
  int have_unit_killed;
  int have_moveto;
  int have_receiver;
  int have_veteran;
  int have_tech_receiver;
  int have_tech_id;
  char *line;
  int have_defender;
  int have_talker_type;
  int have_destroy_whom;
  /* the last civ read: the verbose messages of Receiver= (RECEIVEDTECHNOLOGY)
     and Attacker= / City= (CITYTAKEN) print it before it is set */
  int parsed_civ;
  int have_terrain;
  int have_amount;
  int have_who;
  int have_attacker;
  int have_whom;
};

/* Part of ScenarioEvents::parse_file (004fc516): the trigger UNITKILLED (its
   Unit=, Attacker= and Defender= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_unit_killed(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;
  int str_buf;

  if (events_parse_verbose != 0) {
    _printf(str_if_unitkilled_found_looking_for_unit_att);
  }
  while (((ps->have_unit_killed == 0 || (ps->have_defender == 0)) || (ps->have_attacker == 0))) {
    ps->line = text_read_line(1);
    if (ps->line == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file);
      }
      return 0;
    }
    if (*ps->line == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_unitkilled_statement);
      }
      return 0;
    }
    cmp = __strnicmp(ps->line,str_unit,5);
    if (cmp == 0) {
      arg = ps->line + 5;
      cmp = events_parse_unit_type((const char *)arg);
      if ((cmp == -1) || (cmp == -2)) {
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        str_buf = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->unit_name = (char *)str_buf;
        if (ps->event->unit_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->unit_name),arg);
        ps->event->unit_type = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_id_d,arg,cmp);
        }
        ps->have_unit_killed = 1;
      }
    }
    else {
      cmp = __strnicmp(ps->line,str_attacker,9);
      if (cmp == 0) {
        arg = ps->line + 9;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_attacker_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->who_name = (char *)cmp;
          if (ps->event->who_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->who_name),arg);
          ps->event->who_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_attacker_s_king_id_d,arg,ps->parsed_civ);
          }
          ps->have_attacker = 1;
        }
      }
      else {
        cmp = __strnicmp(ps->line,str_defender,9);
        if (cmp == 0) {
          arg = ps->line + 9;
          ps->parsed_civ = events_parse_civ((const char *)arg);
          if (ps->parsed_civ == -1) {
            if (events_parse_verbose != 0) {
              _printf(str_found_defender_s_but_s_is_not_valid,arg,arg);
            }
          }
          else {
            len = _strlen(arg);
            cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
            ps->event->whom_name = (char *)cmp;
            if (ps->event->whom_name == 0) return 0;
            crt_strcpy((char *)((void *)ps->event->whom_name),arg);
            ps->event->whom_civ = ps->parsed_civ;
            if (events_parse_verbose != 0) {
              _printf(str_found_defender_s_king_id_d,arg,ps->parsed_civ);
            }
            ps->have_defender = 1;
          }
        }
      }
    }
  }
  ps->event->trigger = 1;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger NEGOTIATION
   (its Talker=, TalkerType=, Listener= and ListenerType= lines). Returns 0
   when the file is bad. */
int ScenarioEvents::parse_trigger_negotiation(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_negotiation_found_looking_for_talker);
  }
  while (((ps->have_defender == 0 || (ps->have_attacker == 0)) || ((ps->have_talker_type == 0 || (ps->have_listener_type == 0))))) {
    ps->line = text_read_line(1);
    if (ps->line == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_2);
      }
      return 0;
    }
    if (*ps->line == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_negotiation_statement);
      }
      return 0;
    }
    cmp = __strnicmp(ps->line,str_talker,7);
    if (cmp == 0) {
      arg = ps->line + 7;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_talker_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->who_name = (char *)cmp;
        if (ps->event->who_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->who_name),arg);
        ps->event->who_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_talker_s_king_id_d,arg,ps->parsed_civ);
        }
        ps->have_attacker = 1;
      }
    }
    else {
      cmp = __strnicmp(ps->line,str_talkertype,0xb);
      if (cmp == 0) {
        arg = ps->line + 0xb;
        cmp = __strcmpi(arg,str_human);
        if (cmp == 0) {
          ps->event->talker_type = 1;
          ps->have_talker_type = 1;
        }
        else {
          cmp = __strcmpi(arg,str_computer);
          if (cmp == 0) {
            ps->event->talker_type = 2;
            ps->have_talker_type = 1;
          }
          else {
            cmp = __strcmpi(arg,str_humanorcomputer);
            if (cmp == 0) {
              ps->event->talker_type = 4;
              ps->have_talker_type = 1;
            }
          }
        }
        if (events_parse_verbose != 0) {
          if (ps->have_talker_type == 0) {
            _printf(str_found_talkertype_s_but_s_is_not_valid,arg,arg);
          }
          else {
            _printf(str_found_talkertype_s,arg);
          }
        }
      }
      else {
        cmp = __strnicmp(ps->line,str_listener,9);
        if (cmp == 0) {
          arg = ps->line + 9;
          ps->parsed_civ = events_parse_civ((const char *)arg);
          if (ps->parsed_civ == -1) {
            if (events_parse_verbose != 0) {
              _printf(str_found_listener_s_but_s_is_not_valid,arg,arg);
            }
          }
          else {
            len = _strlen(arg);
            cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
            ps->event->whom_name = (char *)cmp;
            if (ps->event->whom_name == 0) return 0;
            crt_strcpy((char *)((void *)ps->event->whom_name),arg);
            ps->event->whom_civ = ps->parsed_civ;
            if (events_parse_verbose != 0) {
              _printf(str_found_listener_s_king_id_d,arg,ps->parsed_civ);
            }
            ps->have_defender = 1;
          }
        }
        else {
          cmp = __strnicmp(ps->line,str_listenertype,0xd);
          if (cmp == 0) {
            arg = ps->line + 0xd;
            cmp = __strcmpi(arg,str_human_2);
            if (cmp == 0) {
              ps->event->listener_type = 1;
              ps->have_listener_type = 1;
            }
            else {
              cmp = __strcmpi(arg,str_computer_2);
              if (cmp == 0) {
                ps->event->listener_type = 2;
                ps->have_listener_type = 1;
              }
              else {
                cmp = __strcmpi(arg,str_humanorcomputer_2);
                if (cmp == 0) {
                  ps->event->listener_type = 4;
                  ps->have_listener_type = 1;
                }
              }
            }
            if (events_parse_verbose != 0) {
              if (ps->have_talker_type == 0) {
                _printf(str_found_listenertype_s_but_s_is_not_valid,arg,arg);
              }
              else {
                _printf(str_found_listenertype_s,arg);
              }
            }
          }
        }
      }
    }
  }
  ps->event->trigger = 0x10;
  ps->event->negotiation_flag = 0x1000;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger NOSCHISM (its
   Defender= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_no_schism(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_noschism_found_looking_for_whom);
  }
  while (ps->have_defender == 0) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_3);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_noschism_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_defender_2,9);
    if (cmp == 0) {
      arg += 9;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_defender_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->whom_name = (char *)cmp;
        if (ps->event->whom_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->whom_name),arg);
        ps->event->whom_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_defender_s_king_id_d_2,arg,ps->parsed_civ);
        }
        ps->have_defender = 1;
      }
    }
  }
  ps->event->trigger = 0x80;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger
   RECEIVEDTECHNOLOGY (its Receiver= and Technology= lines). Returns 0 when
   the file is bad. */
int ScenarioEvents::parse_trigger_received_tech(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_receivedtechnology_found_looking_for);
  }
  while ((ps->have_tech == 0 || (ps->have_defender == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_4);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_receivedtechnology_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_receiver,9);
    if (cmp == 0) {
      arg += 9;
      cmp = events_parse_civ((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->whom_name = (char *)cmp;
        if (ps->event->whom_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->whom_name),arg);
        cmp = events_parse_civ((const char *)arg);
        ps->event->whom_civ = cmp;
        if (events_parse_verbose != 0) {
          /* B33: the civilization just read, not the one of an earlier line (docs/BUGS.md) */
          _printf(str_found_receiver_s_king_id_d,arg,bugfix_ui() ? cmp : ps->parsed_civ);
        }
        ps->have_defender = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_technology,0xb);
      if (cmp == 0) {
        arg += 0xb;
        if ((*arg < '0') || ('9' < *arg)) {
          if (events_parse_verbose != 0) {
            _printf(str_found_technology_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          cmp = str_parse_int_prefixed((const char *)arg);
          ps->event->technology = cmp;
          ps->have_tech = 1;
          if (events_parse_verbose != 0) {
            _printf(str_found_technology_s_technology_id_d,arg,ps->event->technology);
          }
        }
      }
    }
  }
  ps->event->trigger = 0x100;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger CITYTAKEN (its
   City=, Attacker= and Defender= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_city_taken(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_citytaken_found_looking_for_city_atta);
  }
  while (((ps->have_city == 0 || (ps->have_attacker == 0)) || (ps->have_defender == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_5);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_citytaken_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_attacker_2,9);
    if (cmp == 0) {
      arg += 9;
      cmp = events_parse_civ((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_attacker_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->who_name = (char *)cmp;
        if (ps->event->who_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->who_name),arg);
        cmp = events_parse_civ((const char *)arg);
        ps->event->who_civ = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_attacker_s_king_id_d_2,arg,bugfix_ui() ? cmp : ps->parsed_civ);  /* B33 */
        }
        ps->have_attacker = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_defender_3,9);
      if (cmp == 0) {
        arg += 9;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_defender_s_but_s_is_not_valid_3,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->whom_name = (char *)cmp;
          if (ps->event->whom_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->whom_name),arg);
          ps->event->whom_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_defender_s_king_id_d_3,arg,ps->parsed_civ);
          }
          ps->have_defender = 1;
        }
      }
      else {
        cmp = __strnicmp(arg,str_city_5,5);
        if (cmp == 0) {
          arg += 5;
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->city_name = (char *)cmp;
          if (ps->event->city_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->city_name),arg);
          if (events_parse_verbose != 0) {
            _printf(str_found_city_s,arg,ps->parsed_civ);
          }
          ps->have_city = 1;
        }
      }
    }
  }
  ps->event->trigger = 2;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger TURN (its Turn=
   line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_turn(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_turn_found_looking_for_turn);
  }
  do {
    do {
      arg = text_read_line(1);
      if (arg == (char *)0x0) {
        if (events_parse_verbose != 0) {
          _printf(str_got_early_end_of_file_6);
        }
        return 0;
      }
      if (*arg == '@') {
        if (events_parse_verbose != 0) {
          _printf(str_illegal_turn_statement);
        }
        return 0;
      }
      cmp = __strnicmp(arg,str_turn,5);
    } while (cmp != 0);
    arg += 5;
    cmp = __strcmpi(arg,str_every);
    if (cmp == 0) {
      ps->event->turn = -1;
      ps->event->trigger = 4;
    }
    else if (((('/' < *arg) && (*arg < ':')) || (*arg == '-')) ||
            (*arg == '+')) {
      cmp = str_parse_int_prefixed((const char *)arg);
      ps->event->turn = cmp;
      ps->event->trigger = 4;
    }
    if (events_parse_verbose != 0) {
      if (ps->event->trigger == 4) {
        _printf(str_found_turn_s,arg);
      }
      else {
        _printf(str_found_turn_s_but_s_is_not_valid,arg,arg);
      }
    }
  } while (ps->event->trigger != 4);
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger TURNINTERVAL
   (its Interval= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_turn_interval(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_turninterval_found_looking_for_interv);
  }
  do {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_7);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_turninterval_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_interval,9);
    if (cmp == 0) {
      arg += 9;
      if ((((*arg < '0') || ('9' < *arg)) && (*arg != '-')) &&
         (*arg != '+')) {
        if (events_parse_verbose != 0) {
          _printf(str_found_interval_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->turn = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_interval_s,arg);
        }
        ps->event->trigger = 8;
      }
    }
  } while (ps->event->trigger != 8);
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger RANDOMTURN (its
   Denominator= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_random_turn(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_randomturn_found_looking_for_denomina);
  }
  do {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_8);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_randomturn_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_denominator,0xc);
    if (cmp == 0) {
      arg += 0xc;
      if ((*arg < '0') || ('9' < *arg)) {
        if (events_parse_verbose != 0) {
          _printf(str_found_denominator_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->denominator = cmp;
        if ((ps->event->denominator < 1) || (1000 < ps->event->denominator)) {
          _printf(str_found_denominator_s_but_s_is_not_valid,arg,arg);
          return 0;
        }
        if (events_parse_verbose != 0) {
          _printf(str_found_denominator_s,arg);
        }
        ps->event->trigger = 0x40;
      }
    }
  } while (ps->event->trigger != 0x40);
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): a line of the trigger after
   @IF (state 2). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger(EventsParseState *ps)
{
  int cmp;

  cmp = __strcmpi(ps->line,str_unitkilled);
  if (cmp == 0) {
    if (parse_trigger_unit_killed(ps) == 0) return 0;
  }
  cmp = __strcmpi(ps->line,str_negotiation);
  if (cmp == 0) {
    if (parse_trigger_negotiation(ps) == 0) return 0;
  }
  cmp = __strcmpi(ps->line,str_noschism);
  if (cmp == 0) {
    if (parse_trigger_no_schism(ps) == 0) return 0;
  }
  else {
    cmp = __strcmpi(ps->line,str_receivedtechnology);
    if (cmp == 0) {
      if (parse_trigger_received_tech(ps) == 0) return 0;
    }
    else {
      cmp = __strcmpi(ps->line,str_citytaken);
      if (cmp == 0) {
        if (parse_trigger_city_taken(ps) == 0) return 0;
      }
      else {
        cmp = __strcmpi(ps->line,((i8 *)&events_kw_turn_str));
        if (cmp == 0) {
          if (parse_trigger_turn(ps) == 0) return 0;
        }
        else {
          cmp = __strcmpi(ps->line,str_turninterval);
          if (cmp == 0) {
            if (parse_trigger_turn_interval(ps) == 0) return 0;
          }
          else {
            cmp = __strcmpi(ps->line,str_randomturn);
            if (cmp == 0) {
              if (parse_trigger_random_turn(ps) == 0) return 0;
            }
            else {
              cmp = __strcmpi(ps->line,str_scenarioloaded_3);
              if (cmp == 0) {
                if (events_parse_verbose != 0) {
                  _printf(str_if_scenarioloaded);
                }
                ps->event->trigger = 0x20;
              }
            }
          }
        }
      }
    }
  }
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action TEXT (its lines
   up to ENDTEXT). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_text(EventsParseState *ps)
{
  int cmp;
  size_t len;
  iptr i;

  if (events_parse_verbose != 0) {
    _printf(str_then_text_found_looking_for_endtext);
  }
  i = 0;
  ps->line = text_read_line(1);
  if (ps->line == (char *)0x0) return 0;
  while (cmp = __strcmpi(ps->line,str_endtext), cmp != 0) {
    if (i != 0x14) {
      len = _strlen(ps->line);
      cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
      ps->event->text[i] = (char *)cmp;
      if (ps->event->text[i] == 0) return 0;
      crt_strcpy(ps->event->text[i],ps->line);
      i++;
    }
    ps->line = text_read_line(1);
    if (ps->line == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_9);
      }
      return 0;
    }
    if (*ps->line == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_text_statement);
      }
      return 0;
    }
  }
  if (events_parse_verbose != 0) {
    _printf(str_found_endtext);
  }
  ps->event->actions = (int)ps->event->actions | 1;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action CHANGETERRAIN
   (its TerrainType= line and MapRect). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_change_terrain(EventsParseState *ps)
{
  int cmp;
  char *arg;
  iptr i;
  int loc_count;

  if (events_parse_verbose != 0) {
    _printf(str_then_changeterrain_found_looking_for_ter);
  }
  loc_count = 0;
  while ((ps->have_terrain == 0 || (loc_count != 4))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_10);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_changeterrain_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_terraintype,0xc);
    if (cmp == 0) {
      arg += 0xc;
      if ((*arg < '0') || ('9' < *arg)) {
        if (events_parse_verbose != 0) {
          _printf(str_found_terraintype_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        cmp = str_parse_int_prefixed((const char *)arg);
        if ((-1 < cmp) || (cmp < 0xb)) {
          ps->event->terrain_type = cmp;
          if (events_parse_verbose != 0) {
            _printf(str_found_terraintype_s_terrainid_d,arg,cmp);
          }
          ps->have_terrain = 1;
        }
      }
    }
    else {
      cmp = __strcmpi(arg,str_maprect);
      if (cmp == 0) {
        if (events_parse_verbose != 0) {
          _printf(str_found_maprect);
        }
        cmp = (int)text_read_line(1);
        if (cmp == 0) {
          if (events_parse_verbose != 0) {
            _printf(str_got_early_end_of_file_11);
          }
          return 0;
        }
        for (i = 0; i < 4; i++) {
          cmp = text_next_int();
          ps->event->terrain_rect[i * 2] = cmp;
          cmp = text_next_int();
          ps->event->terrain_rect[i * 2 + 1] = cmp;
          if (events_parse_verbose != 0) {
            _printf(str_found_location_d_d_d,loc_count,
                    ps->event->terrain_rect[i * 2],ps->event->terrain_rect[i * 2 + 1]);
          }
          loc_count++;
        }
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x200;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action CREATEUNIT (its
   Unit=, Owner=, Veteran=, HomeCity= lines and Locations). Returns 0 when
   the file is bad. */
int ScenarioEvents::parse_action_create_unit(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;
  int str_buf;
  iptr i;
  int loc_count;

  if (events_parse_verbose != 0) {
    _printf(str_then_createunit_found_looking_for_unit_o);
  }
  loc_count = 0;
  while (((ps->have_unit == 0) || (ps->have_owner == 0)) ||
        ((ps->have_homecity == 0 || ((ps->have_veteran == 0 || (loc_count == 0)))))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_12);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_createunit_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_unit_2,5);
    if (cmp == 0) {
      arg += 5;
      cmp = events_parse_unit_type((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        str_buf = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->create_unit_name = (char *)str_buf;
        if (ps->event->create_unit_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->create_unit_name),arg);
        ps->event->create_unit_type = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_unit_id_d,arg,cmp);
        }
        ps->have_unit = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_owner,6);
      if (cmp == 0) {
        arg += 6;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->create_owner_name = (char *)cmp;
          if (ps->event->create_owner_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->create_owner_name),arg);
          ps->event->create_owner_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_king_id_d,arg,ps->parsed_civ);
          }
          ps->have_owner = 1;
        }
      }
      else {
        cmp = __strnicmp(arg,str_veteran,8);
        if (cmp == 0) {
          arg += 8;
          cmp = __strcmpi(arg,((i8 *)&events_bool_yes_str));
          if ((cmp == 0) || (cmp = __strcmpi(arg,((i8 *)&events_bool_true_str)), cmp == 0)) {
            ps->event->create_veteran = 1;
            ps->have_veteran = 1;
          }
          else {
            cmp = __strcmpi(arg,((i8 *)&events_bool_no_str));
            if ((cmp == 0) || (cmp = __strcmpi(arg,str_false), cmp == 0)
               ) {
              ps->event->create_veteran = 0;
              ps->have_veteran = 1;
            }
            else if (events_parse_verbose != 0) {
              _printf(str_found_veteran_s_but_s_is_not_valid,arg,arg);
            }
          }
        }
        else {
          cmp = __strnicmp(arg,str_homecity,9);
          if (cmp == 0) {
            arg += 9;
            len = _strlen(arg);
            cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
            ps->event->create_homecity = (char *)cmp;
            if (ps->event->create_homecity == 0) return 0;
            crt_strcpy((char *)((void *)ps->event->create_homecity),arg);
            if (events_parse_verbose != 0) {
              _printf(str_found_homecity_s,arg);
            }
            ps->have_homecity = 1;
          }
          else {
            cmp = __strcmpi(arg,str_locations);
            if (cmp == 0) {
              if (events_parse_verbose != 0) {
                _printf(str_found_locations);
              }
              for (i = 0; i < 10; i++) {
                arg = text_read_line(1);
                if (arg == (char *)0x0) {
                  if (events_parse_verbose != 0) {
                    _printf(str_got_early_end_of_file_13);
                  }
                  return 0;
                }
                if ((((*arg < '0') || ('9' < *arg)) && (*arg != '-')) &&
                   (*arg != '+')) {
                  cmp = __strcmpi(arg,str_endlocations);
                  if (cmp == 0) {
                    if (events_parse_verbose != 0) {
                      _printf(str_found_endlocations,arg,arg);
                    }
                    break;
                  }
                }
                else {
                  cmp = text_next_int();
                  ps->event->create_locations[i * 2] = cmp;
                  cmp = text_next_int();
                  ps->event->create_locations[i * 2 + 1] = cmp;
                  if (events_parse_verbose != 0) {
                    _printf(str_found_location_d_d_d_2,loc_count,
                            ps->event->create_locations[i * 2],ps->event->create_locations[i * 2 + 1])
                    ;
                  }
                  loc_count++;
                }
              }
            }
          }
        }
      }
    }
  }
  ps->event->create_location_count = loc_count;
  ps->event->actions = (int)ps->event->actions | 4;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action CHANGEMONEY (its
   Receiver= and Amount= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_change_money(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_changemoney_found_looking_for_recei);
  }
  while ((ps->have_receiver == 0 || (ps->have_amount == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_14);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_changemoney_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_receiver_2,9);
    if (cmp == 0) {
      arg += 9;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->money_receiver_name = (char *)cmp;
        if (ps->event->money_receiver_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->money_receiver_name),arg);
        ps->event->money_receiver_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_king_id_d_2,arg,ps->parsed_civ);
        }
        ps->have_receiver = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_amount,7);
      if ((cmp == 0) &&
         ((((arg = arg + 7, '/' < *arg && (*arg < ':')) ||
           (*arg == '-')) || (*arg == '+')))) {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->money_amount = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_amount_s_integer_d,arg,ps->event->money_amount);
        }
        ps->have_amount = 1;
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 8;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action MAKEAGGRESSION
   (its Who= and Whom= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_make_aggression(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_makeaggression_found_looking_for_wh);
  }
  while ((ps->have_who == 0 || (ps->have_whom == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_15);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_makeaggression_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,((i8 *)&events_who_prefix_str),4);
    if (cmp == 0) {
      arg += 4;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_who_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->aggression_who_name = (char *)cmp;
        if (ps->event->aggression_who_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->aggression_who_name),arg);
        ps->event->aggression_who_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_who_s_king_id_d,arg,ps->parsed_civ);
        }
        ps->have_who = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_whom,5);
      if (cmp == 0) {
        arg += 5;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_whom_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->aggression_whom_name = (char *)cmp;
          if (ps->event->aggression_whom_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->aggression_whom_name),arg);
          ps->event->aggression_whom_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_whom_s_king_id_d,arg,ps->parsed_civ);
          }
          ps->have_whom = 1;
        }
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x20;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action
   DESTROYACIVILIZATION (its Whom= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_destroy_civ(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_destroyacivilization_found_looking);
  }
  while (ps->have_destroy_whom == 0) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_16);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_destroyacivilization_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_whom_2,5);
    if (cmp == 0) {
      arg += 5;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_whom_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        ps->event->destroy_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_whom_s_king_id_d_2,arg,ps->parsed_civ);
        }
        ps->have_destroy_whom = 1;
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x400;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action GIVETECHNOLOGY
   (its Technology= and Receiver= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_give_tech(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_givetechnology_found_looking_for_wh);
  }
  while ((ps->have_tech_id == 0 || (ps->have_tech_receiver == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_17);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_givetechnology_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_receiver_3,9);
    if (cmp == 0) {
      arg += 9;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_but_s_is_not_valid_3,arg,arg)
          ;
        }
      }
      else {
        ps->event->give_tech_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_king_id_d_3,arg,ps->parsed_civ)
          ;
        }
        ps->have_tech_receiver = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_technology_2,0xb);
      if (((cmp == 0) && (arg = arg + 0xb, '/' < *arg)) &&
         (*arg < ':')) {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->give_tech = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_technology_s_integer_d,arg,
                  ps->event->give_tech);
        }
        ps->have_tech_id = 1;
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x800;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action MOVEUNIT (its
   Unit=, Owner=, MapRect, MoveTo and NumberToMove= lines). Returns 0 when
   the file is bad. */
int ScenarioEvents::parse_action_move_unit(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;
  int str_buf;
  iptr i;
  int loc_count;

  if (events_parse_verbose != 0) {
    _printf(str_then_moveunit_found_looking_for_unit_own);
  }
  loc_count = 0;
  while ((((ps->have_unit == 0 || (ps->have_owner == 0)) || (loc_count != 4)) ||
         ((ps->have_moveto == 0 || (ps->have_move_count == 0))))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_18);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_moveunit_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_unit_3,5);
    if (cmp == 0) {
      arg += 5;
      cmp = events_parse_unit_type((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_but_s_is_not_valid_3,arg,
                  arg);
        }
      }
      else {
        len = _strlen(arg);
        str_buf = memblock_alloc((MemBlock *)events_string_pool,len + 1)
        ;
        ps->event->move_unit_name = (char *)str_buf;
        if (ps->event->move_unit_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->move_unit_name),arg);
        ps->event->move_unit_type = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_unit_id_d_2,arg,cmp);
        }
        ps->have_unit = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_owner_2,6);
      if (cmp == 0) {
        arg += 6;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_but_s_is_not_valid_2,arg,
                    arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,
                                     len + 1);
          ps->event->move_owner_name = (char *)cmp;
          if (ps->event->move_owner_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->move_owner_name),arg);
          ps->event->move_owner_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_king_id_d_2,arg,ps->parsed_civ
                   );
          }
          ps->have_owner = 1;
        }
      }
      else {
        cmp = __strcmpi(arg,str_maprect_2);
        if (cmp == 0) {
          if (events_parse_verbose != 0) {
            _printf(str_found_maprect_2);
          }
          cmp = (int)text_read_line(1);
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_got_early_end_of_file_19);
            }
            return 0;
          }
          for (i = 0; i < 4; i++) {
            cmp = text_next_int();
            ps->event->move_rect[i * 2] = cmp;
            cmp = text_next_int();
            ps->event->move_rect[i * 2 + 1] = cmp;
            if (events_parse_verbose != 0) {
              _printf(str_found_location_d_d_d_3,loc_count,
                      ps->event->move_rect[i * 2],
                      ps->event->move_rect[i * 2 + 1]);
            }
            loc_count++;
          }
        }
        else {
          cmp = __strcmpi(arg,str_moveto);
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_found_moveto);
            }
            arg = text_read_line(1);
            if (arg == (char *)0x0) {
              if (events_parse_verbose != 0) {
                _printf(str_got_early_end_of_file_20);
              }
              return 0;
            }
            if (((('/' < *arg) && (*arg < ':')) || (*arg == '-'))
               || (*arg == '+')) {
              cmp = text_next_int();
              ps->event->move_to_x = cmp;
              cmp = text_next_int();
              ps->event->move_to_y = cmp;
              if (events_parse_verbose != 0) {
                _printf(str_found_moveto_d_d,ps->event->move_to_x,
                        ps->event->move_to_y);
              }
              ps->have_moveto = 1;
            }
          }
          else {
            cmp = __strnicmp(arg,str_numbertomove,0xd);
            if (cmp == 0) {
              arg += 0xd;
              if ((*arg < '0') || ('9' < *arg)) {
                cmp = __strcmpi(arg,((i8 *)&events_numbertomove_all_str));
                if (cmp == 0) {
                  ps->event->move_count = -2;
                  if (events_parse_verbose != 0) {
                    _printf(str_found_numbertomove_s,arg);
                  }
                  ps->have_move_count = 1;
                }
                else if (events_parse_verbose != 0) {
                  _printf(str_found_numbertomove_s_but_s_is_not_valid,
                          arg,arg);
                }
              }
              else {
                cmp = str_parse_int_prefixed((const char *)arg);
                ps->event->move_count = cmp;
                if (events_parse_verbose != 0) {
                  _printf(str_found_numbertomove_s_integer_d,
                          arg,ps->event->move_count);
                }
                ps->have_move_count = 1;
              }
            }
          }
        }
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 2;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action PLAYCDTRACK (its
   track line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_play_cd(EventsParseState *ps)
{
  int cmp;

  if (events_parse_verbose != 0) {
    _printf(str_then_playcdtrack_found_looking_for_track);
  }
  cmp = (int)text_read_line(1);
  if (cmp == 0) {
    if (events_parse_verbose != 0) {
      _printf(str_got_early_end_of_file_21);
    }
    return 0;
  }
  cmp = text_next_int();
  ps->event->cd_track = cmp;
  if (0 < ps->event->cd_track) {
    if (events_parse_verbose != 0) {
      _printf(str_found_cd_track_d,ps->event->cd_track);
    }
    ps->event->actions = (int)ps->event->actions | 0x80;
  }
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action PLAYWAVEFILE
   (its file line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_play_wave(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_playwavefile_found_looking_for_file);
  }
  arg = text_read_line(1);
  if (arg == (char *)0x0) {
    if (events_parse_verbose != 0) {
      _printf(str_got_early_end_of_file_22);
    }
    return 0;
  }
  len = _strlen(arg);
  cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
  ps->event->wave_file = (char *)cmp;
  if (ps->event->wave_file == 0) return 0;
  crt_strcpy((char *)((void *)ps->event->wave_file),arg);
  if (events_parse_verbose != 0) {
    _printf(str_found_wave_file_name_s,(int)ps->event->wave_file);
  }
  ps->event->actions = (int)ps->event->actions | 0x10;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): a line of an action after
   @THEN (state 3). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action(EventsParseState *ps)
{
  int cmp;

  cmp = __strcmpi(ps->line,((i8 *)&events_kw_text_str));
  if (cmp == 0) {
    if (parse_action_text(ps) == 0) return 0;
  }
  else {
    cmp = __strcmpi(ps->line,str_changeterrain);
    if (cmp == 0) {
      if (parse_action_change_terrain(ps) == 0) return 0;
    }
    else {
      cmp = __strcmpi(ps->line,str_createunit);
      if (cmp == 0) {
        if (parse_action_create_unit(ps) == 0) return 0;
      }
      else {
        cmp = __strcmpi(ps->line,str_changemoney);
        if (cmp == 0) {
          if (parse_action_change_money(ps) == 0) return 0;
        }
        else {
          cmp = __strcmpi(ps->line,str_justonce);
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_then_justonce_found_not_looking_for_anyt);
            }
            ps->event->actions = (int)ps->event->actions | 0x40;
          }
          else {
            cmp = __strcmpi(ps->line,str_dontplaywonders);
            if (cmp == 0) {
              if (events_parse_verbose != 0) {
                _printf(str_then_dontplaywonders_found_not_looking_f);
              }
              ps->event->actions = (int)ps->event->actions | 0x100;
            }
            else {
              cmp = __strcmpi(ps->line,str_makeaggression);
              if (cmp == 0) {
                if (parse_action_make_aggression(ps) == 0) return 0;
              }
              else {
                cmp = __strcmpi(ps->line,str_destroyacivilization);
                if (cmp == 0) {
                  if (parse_action_destroy_civ(ps) == 0) return 0;
                }
                else {
                  cmp = __strcmpi(ps->line,str_givetechnology);
                  if (cmp == 0) {
                    if (parse_action_give_tech(ps) == 0) return 0;
                  }
                  else {
                    cmp = __strcmpi(ps->line,str_moveunit);
                    if (cmp == 0) {
                      if (parse_action_move_unit(ps) == 0) return 0;
                    }
                    else {
                      cmp = __strcmpi(ps->line,str_playcdtrack);
                      if (cmp == 0) {
                        if (parse_action_play_cd(ps) == 0) return 0;
                      }
                      else {
                        cmp = __strcmpi(ps->line,str_playwavefile);
                        if (cmp == 0) {
                          if (parse_action_play_wave(ps) == 0) return 0;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 1;
}

/* 004fc516 auto */
int ScenarioEvents::parse_file(const char *filename,const char *section)

{
  int cmp;
  uint state;
  int failed;
  EventsParseState ps = {};  /* BF-030: no event before the first @IF */
  
  failed = 1;
  state = 1;
  events_parse_verbose = 0;
  cmp = text_open_section(filename,section);
  if (cmp == 0) {
    do {
      ps.line = text_read_line(1);
      if (ps.line == (char *)0x0) {
        state = 10;
      }
      else if (*ps.line == '@') {
        ps.line++;
        cmp = __strcmpi(ps.line,((i8 *)&events_kw_if_str));
        if (cmp == 0) {
          if (events_parse_verbose != 0) {
            _printf(str_if_found_creating_new_event_structure);
          }
          ps.event = alloc();
          if (ps.event == (ScenarioEvent *)0x0) {
            if (events_parse_verbose != 0) {
              _printf(str_failed_to_create_new_event_structure);
            }
            break;
          }
          if (events_parse_verbose != 0) {
            _printf(str_new_event_structure_created);
          }
          ps.have_city = 0;
          ps.have_defender = 0;
          ps.have_attacker = 0;
          ps.have_unit_killed = 0;
          ps.have_terrain = 0;
          ps.have_moveto = 0;
          ps.have_unit = 0;
          ps.have_owner = 0;
          ps.have_move_count = 0;
          ps.have_amount = 0;
          ps.have_receiver = 0;
          ps.have_destroy_whom = 0;
          ps.have_whom = 0;
          ps.have_who = 0;
          ps.have_tech = 0;
          ps.have_listener_type = 0;
          ps.have_talker_type = 0;
          ps.have_veteran = 0;
          ps.have_homecity = 0;
          ps.have_tech_receiver = 0;
          ps.have_tech_id = 0;
          state = 2;
        }
        else if (state != 6) {
          cmp = __strcmpi(ps.line,((i8 *)&events_kw_then_str));
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_then_found);
            }
            state = 3;
          }
          else {
            cmp = __strcmpi(ps.line,str_endif);
            if (cmp == 0) {
              if (events_parse_verbose != 0) {
                _printf(str_endif_found);
              }
              state = 5;
            }
            else {
              cmp = __strcmpi(ps.line,str_debug);
              if (cmp == 0) {
                events_parse_verbose = 1;
                _printf(str_debugging_filename_s,filename);
              }
              else {
                cmp = __strcmpi(ps.line,str_endevents);
                if (cmp == 0) {
                  if (events_parse_verbose != 0) {
                    _printf(str_endevents_found);
                  }
                  state = 10;
                }
              }
            }
          }
        }
      }
      else if (state == 2) {
        if (parse_trigger(&ps) == 0) break;
      }
      else if (state == 3) {
        /* BF-030 (Peter Johnson's ledger): an action after an @THEN with no
           @IF before it had no event to go to (the original wrote through
           an unset pointer): a bad events file */
        if ((ps.event == (ScenarioEvent *)0x0) || (parse_action(&ps) == 0)) break;
      }
    } while (state < 10);
    if (state == 10) {
      failed = 0;  /* read to the end (an error leaves the loop earlier) */
    }
  }
  if (failed != 0) {
    __chdir(((i8 *)&game_dirs.home));
    popup_game((const char *)&str_badeventsfile);
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  }
  return failed;
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
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_event_0_event_3,(iptr)&str_d_ss_franklinton_startup_multiplayer_cpp,0x66);
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
        minutes = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_customgametimer,4,(iptr)&menu_prefs.turn_timer_custom,0,0,0,1);
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
        choice = dlg.run_modal((iptr)custom_text,0);
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
      str_append_label((iptr)&text_args.string1,0x285);
    }
    else {
      minutes = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) / 0x3c;
      seconds = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) % 0x3c;
      if (minutes < 10) {
        str_append_int((iptr)&text_args.string1,0);
      }
      str_append_int((iptr)&text_args.string1,minutes);
      crt_strcat(((i8 *)&text_args.string1),((i8 *)&setup_timer_minsec_sep_str));
      if (seconds < 10) {
        str_append_int((iptr)&text_args.string1,0);
      }
      str_append_int((iptr)&text_args.string1,seconds);
    }
    net.vote_replies_pending = net.net_player_count[0] - 1;
    net.vote_status = 0;
    net_send_text_args(0x40,0xff,2,0,0,0);
    dlg_style.pending_idle_callback = net_vote_wait_idle_cb;
    dialog_state.timeout_start = timer_ticks();
    minutes = popup_game_sprite_flags((const char *)&str_newturntimerserver,0x2000001,(iptr)&popup_info_icon_sprite,0);
    if (((minutes < 0) || (net.vote_status == -1)) || (net.vote_replies_pending != 0)) {
      menu_prefs.turn_timer_choice = saved_choice;
      civ_saved_cursor_x_block.mp_turn_time_limit_ms = saved_limit;
      net_send_text_args(0x41,0xff,0,0,0,0);
      popup_game_with_sprite((const char *)&str_newturntimerno_2,(iptr)&popup_info_icon_sprite,0);
      result = 1;
      return result;
    }
    net_send_text_args(0x42,0xff,0,0,0,0);
    popup_game_with_sprite((const char *)&str_newturntimeryes_2,(iptr)&popup_info_icon_sprite,0);
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
    popup_game_with_sprite(lost_msg,(iptr)&popup_info_icon_sprite,0);
  }
  else {
    popup_game_with_sprite(quit_msg,(iptr)&popup_info_icon_sprite,0);
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
  net_send_message(NM_LOCK_NATION,0,leader2,(int)game.player_civ,0,0,0,0,0,0);
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
    popup_game_with_sprite((const char *)&str_serverconnectfail_3,(iptr)&popup_info_icon_sprite,0);
    if (in_game == 0) {
      ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
    }
    dlg->close();
    return TRIBE_FINISH;
  }
  if (net.nm_nation_reject_count == 0) return TRIBE_NEXT;
  *retry_tribe = leader2;
  popup_game_with_sprite((const char *)&str_alreadychosen_2,(iptr)&popup_info_icon_sprite,0);
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
    dlg->load_text_section((const char *)&str_game_7,(const char *)&str_name_5,0x17,(iptr)&civ_names[game.player_civ].leader_name,0,0,0,1);
    dialog_prepend_title_label((iptr)dlg,player_slot + 0x25e);
    if (in_game == 0) {
      dlg->set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3)
                  + 1));
    }
    if (2 < game_mode) {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
    }
    cancelled = dlg->run_modal((iptr)input_text,0);
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
      net_send_message(NM_UNLOCK_NATION,0,leader2,(int)game.player_civ,0,0,0,0,0,0);
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
    cancelled = dlg->load_text_section((const char *)&str_game_7,(const char *)&str_customtribe_2,0x17,(iptr)&civ_names[game.player_civ].leader_name,
                          0,0,0,1);
    if (cancelled != 0) {
      game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
      if (in_game == 0) {
        ((SetupPicWindow *)((i8 *)&setup_picwnd_state.window))->close();
      }
      if (2 < game_mode) {
        net_send_message(NM_UNLOCK_NATION,0,leader2,(int)game.player_civ,0,0,0,0,0,0);
      }
      return TRIBE_FINISH;
    }
    dialog_prepend_title_label((iptr)dlg,player_slot + 0x25e);
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
        net_send_message(NM_UNLOCK_NATION,0,leader2,(int)game.player_civ,0,0,0,0,0,0);
      }
      return TRIBE_FINISH;
    }
    dialog_prepend_title_label((iptr)dlg,player_slot + 0x25e);
    for (title = 0; title < 7; title++) {
      text_clear();
      text_append_string_id(government_names[title]);
      text_append_colon();
      if (((((i8 *)&net.player_info_govt_titles))[title * 0x18] == '\0') || (game_mode == 1)) {
        dlg->add_edit_field((const char *)&textbuf.buf,(iptr)&civ_names[game.player_civ].titles[title],0x17);
      }
      else {
        dlg->add_edit_field((const char *)&textbuf.buf,
                   (iptr)(((i8 *)&net.player_info_govt_titles) + title * 0x18),0x17);
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
      net_send_message(NM_UNLOCK_NATION,0,leader2,(int)game.player_civ,0,0,0,0,0,0);
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

  if (in_game == 0) {
    setup_show_side_picture(8,0);
  }
  dlg->load_game_section((const char *)&str_customcity_2,0x20001);
  dialog_prepend_title_label((iptr)dlg,player_slot + 0x25e);
  for (style2 = 0; style2 < 4; style2++) {
    label = (iptr)label_string(label_table.string_ids[0xf6 + style2]);
    dlg->add_sprite(style2 * 0x1e0 + (iptr)(((u8 *)&gfx_sheet_surface_block.city_walled_sprites + 360)),style2,(const char *)label);
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
  if (*choice < 0) {
    game.human_players = ~(byte)(1 << (game.player_civ & 0x1fU)) & game.human_players;
    if (2 < game_mode) {
      net_send_message(NM_UNLOCK_NATION,0,leader2,(int)game.player_civ,0,0,0,0,0,0);
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
    dlg->load_text_section((const char *)&str_game_7,(const char *)&str_emailaddress,0x1f,(iptr)email_default,0,0,0,1);
    dialog_prepend_title_label((iptr)dlg,player_slot + 0x25e);
    if (in_game == 0) {
      dlg->set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) +
                  1));
    }
    cancelled = dlg->run_modal((iptr)input_text,0);
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
    dialog_prepend_title_label((iptr)&dlg,player_slot + 0x25e);
    dlg.set_default_choice((int)prefs.gender);
    if (in_game == 0) {
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
    }
    if (2 < game_mode) {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
    }
    choice = dlg.show_modal(0);
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
    dialog_prepend_title_label((iptr)&dlg,player_slot + 0x25e);
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
      net_send_message(NM_UNLOCK_NATION,0,leader2,(int)game.player_civ,0,0,0,0,0,0);
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
             = ~*(ushort *)
                 (game_section_dialog_block.setup_slot_tribes[player_slot] * 0x30 + (iptr)((u8 *)leaders + 20) + title * 4) +
               1;
      }
      if (leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][1]
          < 1) {
        *(ushort *)&leaders[(game_section_dialog_block.setup_slot_tribes[player_slot])].titles[title][1]
             = ~*(ushort *)
                 (game_section_dialog_block.setup_slot_tribes[player_slot] * 0x30 + (iptr)((u8 *)leaders + 22) + title * 4) +
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
      ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
      main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
      main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
      wnd_flush_paint();
      load_result = game_load_dialog(1,game_mode,1);
      if (load_result != 0) {
        snd_play_effect(SND_MENUOK,0,0,0);
        ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
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
          dlg.add_sprite((iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),0
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
          dialog_prepend_title_label((iptr)&dlg,player_idx + 0x25e);
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
              text_append_int((int)civs[civ].num_cities);
              text_append_space();
              text_append_label(0xc5);
              text_append((const char *)&str_empty_163);
              text_append_int((int)civs[civ].num_military_units);
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
          result = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_6,0x17,(iptr)&civ_names[((char)(byte)civ)].leader_name,0,0,0,1);
        } while (result != 0);
        dialog_prepend_title_label((iptr)&dlg,player_idx + 0x25e);
        result = dlg.run_modal((iptr)name_text,0);
        if (result == 0) break;
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
      }
      crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),name_text);
      dlg.load_game_section((const char *)&str_gender_6,1);
      dialog_prepend_title_label((iptr)&dlg,player_idx + 0x25e);
      dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      result = dlg.show_modal(0);
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
    dialog_prepend_title_label((iptr)&dlg,player_idx + 0x25f);
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
        text_append_int((int)civs[civ2].num_cities);
        text_append_space();
        text_append_label(0xc5);
        text_append((const char *)&str_empty_165);
        text_append_int((int)civs[civ2].num_military_units);
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
    result = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_7,0x17,(iptr)str,ret,arg_b,arg_c,arg_d);
    if (result == 0) {
      dialog_prepend_title_label((iptr)&dlg,player_idx + 0x25f);
      result = dlg.run_modal((iptr)name_text,0);
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
          dialog_prepend_title_label((iptr)&dlg,player_idx + 0x25f);
          dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female)
          ;
          dlg.set_position(0xfffffc19,
                     -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) +
                      1));
          civ = dlg.show_modal(0);
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

/* 00548df0 auto */
void __cdecl events_editor_focus_redraw(void)

{
  HWND hWnd;
  
  hWnd = ((Control *)&editor_active.wnd->trigger_listbox)->get_hwnd();
  SetFocus(hWnd);
  editor_active.wnd->events_editor_draw();
  return;
}

/* 00548e23 auto */
/* WARNING: Type propagation algorithm not settling */

void __cdecl events_write_actions(iptr fp,ScenarioEvent *event)

{
  int rc;
  uint action;
  i32 civ_name;
  uint action_bit;
  int i;
  char line[80];
  
  rc = _fputs(str_then,(FILE *)fp);
  if (((rc == -1) || (action_bit = 1, event == 0)) || (event->actions == 0)) {
    return;
  }
  do {
    action = event->actions & action_bit;
    if (action < 9) {
      if (action == 8) {
        rc = _fputs(str_changemoney_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_receiver_s,(i32)event->money_receiver_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_amount_d,event->money_amount);
        rc = _fputs(line,(FILE *)fp);
      }
      else if (action == 1) {
        rc = _fputs(str_text,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        for (i = 0; (i < 0x14 && (event->text[i] != 0));
            i = i + 1) {
          rc = _fputs(event->text[i],(FILE *)fp);
          if (rc == -1) {
            return;
          }
          rc = _fputs(((i8 *)&events_text_line_newline_str),(FILE *)fp);
          if (rc == -1) {
            return;
          }
        }
        rc = _fputs(str_endtext_2,(FILE *)fp);
      }
      else if (action == 2) {
        rc = _fputs(str_moveunit_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_unit_s,(i32)event->move_unit_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_owner_s,(i32)event->move_owner_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        rc = _fputs(str_maprect_3,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        for (i = 0; i < 4; i++) {
          _sprintf(line,str_d_d,event->move_rect[i * 2],
                   event->move_rect[i * 2 + 1]);
          if (i < 3) {
            crt_strcat(line,((i8 *)&events_moveunit_rect_sep_str));
          }
          rc = _fputs(line,(FILE *)fp);
          if (rc == -1) {
            return;
          }
        }
        rc = _fputs(str_moveto_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_d_d_2,event->move_to_x,
                 event->move_to_y);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        if (event->move_count == -2) {
          _sprintf(line,str_numbertomove_all);
        }
        else {
          _sprintf(line,str_numbertomove_d,event->move_count);
        }
        rc = _fputs(line,(FILE *)fp);
      }
      else {
        if (action != 4) continue;
        rc = _fputs(str_createunit_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_unit_s_2,(i32)event->create_unit_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_owner_s_2,(i32)event->create_owner_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_veteran_s,
                 ((i8 *)&events_veteran_yes_no_strs) + ((event->create_veteran != 0) - 1 & 4));
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_homecity_s,(i32)event->create_homecity);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        rc = _fputs(str_locations_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        for (i = 0; i < event->create_location_count; i++) {
          _sprintf(line,str_d_d_3,event->create_locations[i * 2],
                   event->create_locations[i * 2 + 1]);
          rc = _fputs(line,(FILE *)fp);
          if (rc == -1) {
            return;
          }
        }
        rc = _fputs(str_endlocations_2,(FILE *)fp);
      }
    }
    else if (0x20 < action) {
      if (action < 0x81) {
        if (action == 0x80) {
          rc = _fputs(str_playcdtrack_2,(FILE *)fp);
          if (rc == -1) {
            return;
          }
          _sprintf(line,((i8 *)&events_cd_track_fmt),event->cd_track);
          rc = _fputs(line,(FILE *)fp);
        }
        else {
          if (action != 0x40) continue;
          rc = _fputs(str_justonce_2,(FILE *)fp);
        }
      }
      else if (action < 0x201) {
        if (action == 0x200) {
          rc = _fputs(str_changeterrain_2,(FILE *)fp);
          if (rc == -1) {
            return;
          }
          _sprintf(line,str_terraintype_d,event->terrain_type);
          rc = _fputs(line,(FILE *)fp);
          if (rc == -1) {
            return;
          }
          rc = _fputs(str_maprect_4,(FILE *)fp);
          if (rc == -1) {
            return;
          }
          for (i = 0; i < 4; i++) {
            _sprintf(line,str_d_d_4,event->terrain_rect[i * 2],
                     event->terrain_rect[i * 2 + 1]);
            if (i < 3) {
              crt_strcat(line,((i8 *)&events_terrain_rect_sep_str));
            }
            rc = _fputs(line,(FILE *)fp);
            if (rc == -1) {
              return;
            }
          }
          rc = _fputs(((i8 *)&events_terrain_rect_end_str),(FILE *)fp);
        }
        else {
          if (action != 0x100) continue;
          rc = _fputs(str_dontplaywonders_2,(FILE *)fp);
        }
      }
      else if (action == 0x400) {
        rc = _fputs(str_destroyacivilization_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        if (event->destroy_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->destroy_civ == -3) {
              _sprintf(line,str_whom_triggerattacker);
            }
            else {
              _sprintf(line,str_whom_triggerdefender);
            }
          }
          else {
            _sprintf(line,str_whom_triggerreceiver);
          }
        }
        else {
          civ_name = (i32)(civ_name_plural(event->destroy_civ));
          _sprintf(line,str_whom_s_2,civ_name);
        }
        rc = _fputs(line,(FILE *)fp);
      }
      else if (action == 0x800) {
        rc = _fputs(str_givetechnology_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        if (event->give_tech_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->give_tech_civ == -3) {
              _sprintf(line,str_receiver_triggerattacker);
            }
            else {
              _sprintf(line,str_receiver_triggerdefender);
            }
          }
          else {
            _sprintf(line,str_receiver_triggerreceiver);
          }
        }
        else {
          civ_name = (i32)(civ_name_plural(event->give_tech_civ));
          _sprintf(line,str_receiver_s_2,civ_name);
        }
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_technology_d,event->give_tech);
        rc = _fputs(line,(FILE *)fp);
      }
      else {
        if (action != 0x2000) continue;
        rc = _fputs(str_hastriggered,(FILE *)fp);
      }
    }
    else if (action == 0x20) {
      rc = _fputs(str_makeaggression_2,(FILE *)fp);
      if (rc == -1) {
        return;
      }
      _sprintf(line,str_who_s,(i32)event->aggression_who_name);
      rc = _fputs(line,(FILE *)fp);
      if (rc == -1) {
        return;
      }
      _sprintf(line,str_whom_s,(i32)event->aggression_whom_name);
      rc = _fputs(line,(FILE *)fp);
    }
    else if (action == 0x10) {
      rc = _fputs(str_playwavefile_2,(FILE *)fp);
      if (rc == -1) {
        return;
      }
      _sprintf(line,((i8 *)&events_wave_file_fmt),(i32)event->wave_file);
      rc = _fputs(line,(FILE *)fp);
    }
    else {
      continue;
    }
    if (rc == -1) {
      return;
    }
  } while ((action_bit = action_bit << 1) != 0x8000);
}

/* 00549aee auto */
void __cdecl events_write_list(iptr fp)

{
  uint trigger;
  int rc;
  ScenarioEvent *event;
  char line[256];
  
  event = unit_types_block.events_list_head;
  do {
    if (event == (ScenarioEvent *)0x0) {
      _fputs(((i8 *)&events_list_end_str),(FILE *)fp);
      return;
    }
    rc = _fputs(str_if,(FILE *)fp);
    if (rc == -1) {
      return;
    }
    trigger = (uint)event->trigger;
    if (trigger < 9) {
      if (trigger == 8) {
        rc = _fputs(str_turninterval_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_interval_d,(uint)event->turn);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
      else if (trigger == 1) {
        rc = _fputs(str_unitkilled_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_unit_s_3,(uint)event->unit_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_attacker_s,(uint)event->who_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_defender_s,(uint)event->whom_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
      else if (trigger == 2) {
        rc = _fputs(str_citytaken_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_city_s,(uint)event->city_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_attacker_s_2,(uint)event->who_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_defender_s_2,(uint)event->whom_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
      else if (trigger == 4) {
        rc = _fputs(str_turn_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_turn_d,(uint)event->turn);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
    }
    else {
      switch(trigger) {
      case 0x10:
        rc = _fputs(str_negotiation_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_talker_s,(uint)event->who_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_talkertype_2);
        if ((uint)event->talker_type == 1) {
          crt_strcat(line,str_human_3);
        }
        if ((uint)event->talker_type == 2) {
          crt_strcat(line,str_computer_3);
        }
        if ((uint)event->talker_type == 4) {
          crt_strcat(line,str_humanorcomputer_3);
        }
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_listener_s,(uint)event->whom_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_listenertype_2);
        if ((uint)event->listener_type == 1) {
          crt_strcat(line,str_human_4);
        }
        if ((uint)event->listener_type == 2) {
          crt_strcat(line,str_computer_4);
        }
        if ((uint)event->listener_type == 4) {
          crt_strcat(line,str_humanorcomputer_4);
        }
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f:
      case 0x21:
      case 0x22:
      case 0x23:
      case 0x24:
      case 0x25:
      case 0x26:
      case 0x27:
      case 0x28:
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2c:
      case 0x2d:
      case 0x2e:
      case 0x2f:
      case 0x30:
      case 0x31:
      case 0x32:
      case 0x33:
      case 0x34:
      case 0x35:
      case 0x36:
      case 0x37:
      case 0x38:
      case 0x39:
      case 0x3a:
      case 0x3b:
      case 0x3c:
      case 0x3d:
      case 0x3e:
      case 0x3f:
      case 0x41:
      case 0x42:
      case 0x43:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
      case 0x48:
      case 0x49:
      case 0x4a:
      case 0x4b:
      case 0x4c:
      case 0x4d:
      case 0x4e:
      case 0x4f:
      case 0x50:
      case 0x51:
      case 0x52:
      case 0x53:
      case 0x54:
      case 0x55:
      case 0x56:
      case 0x57:
      case 0x58:
      case 0x59:
      case 0x5a:
      case 0x5b:
      case 0x5c:
      case 0x5d:
      case 0x5e:
      case 0x5f:
      case 0x60:
      case 0x61:
      case 0x62:
      case 99:
      case 100:
      case 0x65:
      case 0x66:
      case 0x67:
      case 0x68:
      case 0x69:
      case 0x6a:
      case 0x6b:
      case 0x6c:
      case 0x6d:
      case 0x6e:
      case 0x6f:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x76:
      case 0x77:
      case 0x78:
      case 0x79:
      case 0x7a:
      case 0x7b:
      case 0x7c:
      case 0x7d:
      case 0x7e:
      case 0x7f:
      case 0x81:
      case 0x82:
      case 0x83:
      case 0x84:
      case 0x85:
      case 0x86:
      case 0x87:
      case 0x88:
      case 0x89:
      case 0x8a:
      case 0x8b:
      case 0x8c:
      case 0x8d:
      case 0x8e:
      case 0x8f:
      case 0x90:
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
      case 0x98:
      case 0x99:
      case 0x9a:
      case 0x9b:
      case 0x9c:
      case 0x9d:
      case 0x9e:
      case 0x9f:
      case 0xa0:
      case 0xa1:
      case 0xa2:
      case 0xa3:
      case 0xa4:
      case 0xa5:
      case 0xa6:
      case 0xa7:
      case 0xa8:
      case 0xa9:
      case 0xaa:
      case 0xab:
      case 0xac:
      case 0xad:
      case 0xae:
      case 0xaf:
      case 0xb0:
      case 0xb1:
      case 0xb2:
      case 0xb3:
      case 0xb4:
      case 0xb5:
      case 0xb6:
      case 0xb7:
      case 0xb8:
      case 0xb9:
      case 0xba:
      case 0xbb:
      case 0xbc:
      case 0xbd:
      case 0xbe:
      case 0xbf:
      case 0xc0:
      case 0xc1:
      case 0xc2:
      case 0xc3:
      case 0xc4:
      case 0xc5:
      case 0xc6:
      case 199:
      case 200:
      case 0xc9:
      case 0xca:
      case 0xcb:
      case 0xcc:
      case 0xcd:
      case 0xce:
      case 0xcf:
      case 0xd0:
      case 0xd1:
      case 0xd2:
      case 0xd3:
      case 0xd4:
      case 0xd5:
      case 0xd6:
      case 0xd7:
      case 0xd8:
      case 0xd9:
      case 0xda:
      case 0xdb:
      case 0xdc:
      case 0xdd:
      case 0xde:
      case 0xdf:
      case 0xe0:
      case 0xe1:
      case 0xe2:
      case 0xe3:
      case 0xe4:
      case 0xe5:
      case 0xe6:
      case 0xe7:
      case 0xe8:
      case 0xe9:
      case 0xea:
      case 0xeb:
      case 0xec:
      case 0xed:
      case 0xee:
      case 0xef:
      case 0xf0:
      case 0xf1:
      case 0xf2:
      case 0xf3:
      case 0xf4:
      case 0xf5:
      case 0xf6:
      case 0xf7:
      case 0xf8:
      case 0xf9:
      case 0xfa:
      case 0xfb:
      case 0xfc:
      case 0xfd:
      case 0xfe:
      case 0xff:
        break;
      case 0x20:
        rc = _fputs(str_scenarioloaded_5,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x40:
        rc = _fputs(str_randomturn_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_denominator_d,(uint)event->denominator);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x80:
        rc = _fputs(str_noschism_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_defender_s_3,(uint)event->whom_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x100:
        rc = _fputs(str_receivedtechnology_2,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_technology_d_2,(uint)event->technology);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_receiver_s_3,(uint)event->whom_name);
        rc = _fputs(line,(FILE *)fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
    }
    rc = _fputs(str_endif_2,(FILE *)fp);
    if (rc == -1) {
      return;
    }
    event = event->next;
  } while( true );
}

/* 0054a4c4 auto */
i32 __cdecl events_write_file(void)

{
  int rc;
  char *got;
  i32 ok;
  FILE *src_fp;
  FILE *dst_fp;
  char saved_cwd[260];
  i8 filename[16];
  char s[260];
  
  src_fp = (FILE *)0x0;
  dst_fp = (FILE *)0x0;
  crt_strcpy(filename,str_events_3);
  crt_strcat(filename,((i8 *)&lang_file_ext));
  ok = 0;
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  ON_SCOPE_EXIT(
    if (src_fp != (FILE *)0x0) {
      _fclose(src_fp);
    }
    if (dst_fp != (FILE *)0x0) {
      _fclose(dst_fp);
    }
    __chdir(saved_cwd));
  rc = file_exists((const char *)&str_events_bak);
  if (((rc == 0) || (rc = crt_remove(str_events_bak_2), rc == 0)) &&
     ((rc = file_exists((const char *)filename), rc == 0 ||
      (rc = crt_rename(filename,str_events_bak_3), rc == 0)))) {
    src_fp = file_open_path((const char *)&str_events_bak_4,(iptr)&str_rt_7);
    dst_fp = file_open_path((const char *)filename,(iptr)&str_wt_5);
    if (dst_fp != (FILE *)0x0) {
      if (src_fp == (FILE *)0x0) {
        rc = _fputs(str_beginevents_3,dst_fp);
        if (rc == -1) return ok;
      }
      else {
        do {
          got = _fgets(s,0x100,src_fp);
          if ((got == (char *)0x0) || (rc = _fputs(s,dst_fp), rc == -1))
          return ok;
          str_strip_newline(s);
          str_trim((char *)s);
          rc = __strcmpi(s,str_beginevents_2);
        } while (rc != 0);
      }
      events_write_list((iptr)dst_fp);
      if (src_fp == (FILE *)0x0) {
        rc = _fputs(str_endevents_4,dst_fp);
        if (rc != -1) {
          ok = 1;
        }
      }
      else {
        do {
          got = _fgets(s,0x100,src_fp);
          if (got == (char *)0x0) return ok;
          str_strip_newline(s);
          str_trim((char *)s);
          rc = __strcmpi(s,str_endevents_2);
        } while (rc != 0);
        rc = _fputs(str_endevents_3,dst_fp);
        while (rc != -1) {
          got = _fgets(s,0x100,src_fp);
          if (got == (char *)0x0) {
            ok = 1;
            break;
          }
          rc = _fputs(s,dst_fp);
        }
      }
    }
  }
  return ok;
}

/* 0054a912 auto */
i32 __cdecl events_editor_pick_item(int default_choice,const char *title,i32 kind)

{
  iptr name_str;
  iptr item;
  i32 result;
  iptr terrain_item;
  iptr parent_wnd2;
  iptr advance;
  iptr civ;
  Dialog dlg;
  iptr terrain2;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  dlg.Dialog::reset(0x1001);
  dlg.set_title(title);
  dlg.set_width(0xdc);
  dlg.set_listbox(8,0xdc,0);
  switch(kind) {
  case 0:
    if (default_choice == 0) {
      default_choice = 1;
    }
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 1:
    for (civ = 0; civ < 0x3e; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)label_string(unit_types[civ].name);
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 2:
    for (advance = 0; advance < 100; advance++) {
      result = 0;
      item = advance;
      name_str = (iptr)label_string(advances[advance].name);
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 3:
    ((uint *)dlg.add_option((const char *)&str_anyunit_2,-2,0));
    for (civ = 0; civ < 0x3e; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)label_string(unit_types[civ].name);
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 4:
    if (default_choice == 0) {
      default_choice = -2;
    }
    dlg.add_option((const char *)&str_anybody_2,-2,0);
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 5:
    if (default_choice == 0) {
      default_choice = -3;
    }
    dlg.add_option((const char *)&str_triggerattacker_2,-3,0);
    dlg.add_option((const char *)&str_triggerdefender_2,-4,0);
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 6:
    if (default_choice == 0) {
      default_choice = -4;
    }
    dlg.add_option((const char *)&str_triggerreceiver_2,-4,0);
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 7:
    for (terrain2 = 0; terrain2 < 0xb; terrain2++) {
      result = 0;
      terrain_item = terrain2;
      item = (iptr)label_string(terrain_types[terrain2].name);
      dlg.add_option((const char *)item,terrain_item,result);
    }
  }
  dlg.set_default_choice(default_choice);
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  result = dlg.show_modal(0);
  dlg_set_parent_window(0);
  return result;
}

/* 0054adc6 auto */
int __cdecl events_editor_pick_action_civ(int civ,const char *title,int action,iptr out_name)

{
  i32 name2;
  
  if (action == 0x100) {
    civ = events_editor_pick_item(civ,title,6);
    if (civ != -1) {
      if (civ < 0) {
        *(const char **)out_name = str_triggerreceiver_3;
      }
      else {
        name2 = (i32)(civ_name_plural(civ));
        *(i32 *)out_name = name2;
      }
    }
  }
  else {
    civ = events_editor_pick_item(civ,title,5);
    if (civ != -1) {
      if (civ < 0) {
        if (civ == -3) {
          *(const char **)out_name = str_triggerattacker_3;
        }
        else {
          *(const char **)out_name = str_triggerdefender_3;
        }
      }
      else {
        name2 = (i32)(civ_name_plural(civ));
        *(i32 *)out_name = name2;
      }
    }
  }
  return civ;
}

/* 0054ae93 auto */
void __cdecl events_editor_update_buttons(i32 unused)

{
  int action_sel;
  uint action_bit;
  ScenarioEvent *event;
  int action_count;
  int sel;
  
  editor_active.wnd->add_trigger_button.enable();
  sel = editor_active.wnd->trigger_listbox.get_cur_sel();
  if (sel < 0) {
    editor_active.wnd->edit_trigger_button.disable();
    editor_active.wnd->delete_trigger_button.disable();
    editor_active.wnd->add_action_button.disable();
    editor_active.wnd->edit_action_button.disable();
    editor_active.wnd->delete_action_button.disable();
  }
  else {
    editor_active.wnd->delete_trigger_button.enable();
    event = unit_types_block.events_list_head;
    while ((event != (ScenarioEvent *)0x0 && (sel != 0))) {
      event = (ScenarioEvent *)(byte *)event->next;
      sel--;
    }
    if ((*((byte *)event) & 0x20) == 0) {
      editor_active.wnd->edit_trigger_button.enable();
    }
    else {
      editor_active.wnd->edit_trigger_button.disable();
    }
    if (event->trigger == 0x20) {
      action_count = 0;
      while ((1 << ((byte)action_count & 0x1f) & (event->actions | 0xfffffe6f)) != 0) {
        action_count++;
      }
    }
    else {
      action_count = 0;
      while ((event->actions & 1 << ((byte)action_count & 0x1f)) != 0) {
        action_count++;
      }
    }
    if ((action_count < 0xc) && ((*((byte *)event) & 0x90) == 0)) {
      editor_active.wnd->add_action_button.enable();
    }
    else {
      editor_active.wnd->add_action_button.disable();
    }
    action_sel = editor_active.wnd->action_listbox.get_cur_sel();
    if (action_sel < 0) {
      editor_active.wnd->edit_action_button.disable();
      editor_active.wnd->delete_action_button.disable();
    }
    else {
      editor_active.wnd->delete_action_button.enable();
      action_bit = bits_nth_set(action_sel,(i32)event->actions);
      if ((action_bit & 0x3140) == 0) {
        editor_active.wnd->edit_action_button.enable();
      }
      else {
        editor_active.wnd->edit_action_button.disable();
      }
    }
  }
  return;
}

/* 0054b0ed auto */
void __cdecl events_editor_on_ok(void)

{
  iptr ok;
  NativeWindow *wnd;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  iptr parent_wnd2;
  char msg[32];
  
  if (editor_ui.value_clamped == 0) {
    events_compact();
    ok = events_write_file();
    if (ok == 0) {
      _sprintf(msg,str_error_updating_events_s,((i8 *)&lang_file_ext));
      uType = 0x10;
      lpCaption = str_file_i_o_error_5;
      lpText = msg;
      wnd = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice_14);
    dlg_set_parent_window(0);
    editor_ui.value_clamped = 0;
    events_editor_focus_redraw();
  }
  return;
}

/* 0054b1d5 auto */
void __cdecl events_editor_help_trigger(i32 trigger_idx)

{
  iptr parent_wnd2;
  const char *help_key;
  
  switch(trigger_idx) {
  case 0:
    help_key = str_helpunitkilled;
    break;
  case 1:
    help_key = str_helpcitytaken;
    break;
  case 2:
    help_key = str_helpturn;
    break;
  case 3:
    help_key = str_helpturninterval;
    break;
  case 4:
    help_key = str_helpnegotiation;
    break;
  case 5:
    help_key = str_helpscenarioloaded;
    break;
  case 6:
    help_key = str_helprandomturn;
    break;
  case 7:
    help_key = str_helpnoschism;
    break;
  case 8:
    help_key = str_helpreceivedtechnology;
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_12,(const char *)help_key);
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* 0054b2ec auto */
void __cdecl events_editor_help_action(i32 action_idx)

{
  iptr parent_wnd2;
  const char *help_key;
  
  switch(action_idx) {
  case 0:
    help_key = str_helptext;
    break;
  case 1:
    help_key = str_helpmoveunit;
    break;
  case 2:
    help_key = str_helpcreateunit;
    break;
  case 3:
    help_key = str_helpchangemoney;
    break;
  case 4:
    help_key = str_helpplaywavefile;
    break;
  case 5:
    help_key = str_helpmakeaggression;
    break;
  case 6:
    help_key = str_helpjustonce;
    break;
  case 7:
    help_key = str_helpplaycdtrack;
    break;
  case 8:
    help_key = str_helpdontplaywonders;
    break;
  case 9:
    help_key = str_helpchangeterrain;
    break;
  case 10:
    help_key = str_helpdestroyacivilization;
    break;
  case 0xb:
    help_key = str_helpgivetechnology;
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_13,(const char *)help_key);
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* 0054b433 auto */
void __cdecl events_editor_on_help(void)

{
  int action_sel;
  iptr parent_wnd2;
  ScenarioEvent *event;
  uint bits2;
  int idx;
  
  action_sel = editor_active.wnd->action_listbox.get_cur_sel();
  if (action_sel < 0) {
    idx = editor_active.wnd->trigger_listbox.get_cur_sel();
    if (idx < 0) {
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_help_14,(const char *)&str_events_4);
      dlg_set_parent_window(0);
      events_editor_focus_redraw();
    }
    else {
      event = unit_types_block.events_list_head;
      while ((event != (ScenarioEvent *)0x0 && (idx != 0))) {
        event = event->next;
        idx--;
      }
      bits2 = (uint)event->trigger;
      idx = 0;
      for (; (bits2 & 1) == 0; bits2 = bits2 >> 1) {
        idx++;
      }
      if (event != (ScenarioEvent *)0x0) {
        events_editor_help_trigger(idx);
      }
    }
  }
  else {
    idx = editor_active.wnd->trigger_listbox.get_cur_sel();
    event = unit_types_block.events_list_head;
    while ((event != (ScenarioEvent *)0x0 && (idx != 0))) {
      event = (ScenarioEvent *)(uint *)event->next;
      idx--;
    }
    bits2 = bits_nth_set(action_sel,(i32)event->actions);
    idx = 0;
    for (; (bits2 & 1) == 0; bits2 = bits2 >> 1) {
      idx++;
    }
    if (event != (ScenarioEvent *)0x0) {
      events_editor_help_action(idx);
    }
  }
  return;
}

/* 0054b5eb auto */
void __cdecl events_editor_on_cancel(void)

{
  ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
  events_copy((ScenarioEvents *)&unit_types_block.scenario_events,(ScenarioEvents *)events_editor_backup);
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 0054b635 auto */
void __cdecl events_editor_fill_triggers(void)

{
  i32 tech_name;
  uint trigger;
  const char *talker_type;
  const char *listener_type;
  ScenarioEvent *event;
  int num;
  char line[256];
  
  num = 0;
  editor_active.wnd->trigger_listbox.clear();
  for (event = unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
      event = event->next) {
    num++;
    trigger = (uint)event->trigger;
    if (trigger < 9) {
      if (trigger == 8) {
        _sprintf(line,str_d_turninterval_d,num,(uint)event->turn);
      }
      else if (trigger == 1) {
        if (((event->unit_name == 0) || (event->who_name == 0)) || (event->whom_name == 0)) {
          _sprintf(line,str_d_unitkilled,num);
        }
        else {
          _sprintf(line,str_d_unitkilled_s_attacker_s_defender_s,num,(uint)event->unit_name,
                   (uint)event->who_name,(uint)event->whom_name);
        }
      }
      else if (trigger == 2) {
        if (((event->city_name == 0) || (event->who_name == 0)) || (event->whom_name == 0)) {
          _sprintf(line,str_d_citytaken,num);
        }
        else {
          _sprintf(line,str_d_citytaken_s_attacker_s_defender_s,num,(uint)event->city_name,
                   (uint)event->who_name,(uint)event->whom_name);
        }
      }
      else if (trigger == 4) {
        _sprintf(line,str_d_turn_d,num,(uint)event->turn);
      }
    }
    else {
      switch(trigger) {
      case 0x10:
        if ((event->who_name == 0) || (event->whom_name == 0)) {
          _sprintf(line,str_d_negotiation,num);
        }
        else {
          if ((uint)event->listener_type == 1) {
            listener_type = str_human_5;
          }
          else if ((uint)event->listener_type == 2) {
            listener_type = str_computer_5;
          }
          else {
            listener_type = str_humanorcomputer_5;
          }
          if ((uint)event->talker_type == 1) {
            talker_type = str_human_6;
          }
          else if ((uint)event->talker_type == 2) {
            talker_type = str_computer_6;
          }
          else {
            talker_type = str_humanorcomputer_6;
          }
          _sprintf(line,str_d_negotiation_talker_s_s_listener_s_s,num,(uint)event->who_name,
                   talker_type,(uint)event->whom_name,listener_type);
        }
        break;
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f:
      case 0x21:
      case 0x22:
      case 0x23:
      case 0x24:
      case 0x25:
      case 0x26:
      case 0x27:
      case 0x28:
      case 0x29:
      case 0x2a:
      case 0x2b:
      case 0x2c:
      case 0x2d:
      case 0x2e:
      case 0x2f:
      case 0x30:
      case 0x31:
      case 0x32:
      case 0x33:
      case 0x34:
      case 0x35:
      case 0x36:
      case 0x37:
      case 0x38:
      case 0x39:
      case 0x3a:
      case 0x3b:
      case 0x3c:
      case 0x3d:
      case 0x3e:
      case 0x3f:
      case 0x41:
      case 0x42:
      case 0x43:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
      case 0x48:
      case 0x49:
      case 0x4a:
      case 0x4b:
      case 0x4c:
      case 0x4d:
      case 0x4e:
      case 0x4f:
      case 0x50:
      case 0x51:
      case 0x52:
      case 0x53:
      case 0x54:
      case 0x55:
      case 0x56:
      case 0x57:
      case 0x58:
      case 0x59:
      case 0x5a:
      case 0x5b:
      case 0x5c:
      case 0x5d:
      case 0x5e:
      case 0x5f:
      case 0x60:
      case 0x61:
      case 0x62:
      case 99:
      case 100:
      case 0x65:
      case 0x66:
      case 0x67:
      case 0x68:
      case 0x69:
      case 0x6a:
      case 0x6b:
      case 0x6c:
      case 0x6d:
      case 0x6e:
      case 0x6f:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x76:
      case 0x77:
      case 0x78:
      case 0x79:
      case 0x7a:
      case 0x7b:
      case 0x7c:
      case 0x7d:
      case 0x7e:
      case 0x7f:
      case 0x81:
      case 0x82:
      case 0x83:
      case 0x84:
      case 0x85:
      case 0x86:
      case 0x87:
      case 0x88:
      case 0x89:
      case 0x8a:
      case 0x8b:
      case 0x8c:
      case 0x8d:
      case 0x8e:
      case 0x8f:
      case 0x90:
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97:
      case 0x98:
      case 0x99:
      case 0x9a:
      case 0x9b:
      case 0x9c:
      case 0x9d:
      case 0x9e:
      case 0x9f:
      case 0xa0:
      case 0xa1:
      case 0xa2:
      case 0xa3:
      case 0xa4:
      case 0xa5:
      case 0xa6:
      case 0xa7:
      case 0xa8:
      case 0xa9:
      case 0xaa:
      case 0xab:
      case 0xac:
      case 0xad:
      case 0xae:
      case 0xaf:
      case 0xb0:
      case 0xb1:
      case 0xb2:
      case 0xb3:
      case 0xb4:
      case 0xb5:
      case 0xb6:
      case 0xb7:
      case 0xb8:
      case 0xb9:
      case 0xba:
      case 0xbb:
      case 0xbc:
      case 0xbd:
      case 0xbe:
      case 0xbf:
      case 0xc0:
      case 0xc1:
      case 0xc2:
      case 0xc3:
      case 0xc4:
      case 0xc5:
      case 0xc6:
      case 199:
      case 200:
      case 0xc9:
      case 0xca:
      case 0xcb:
      case 0xcc:
      case 0xcd:
      case 0xce:
      case 0xcf:
      case 0xd0:
      case 0xd1:
      case 0xd2:
      case 0xd3:
      case 0xd4:
      case 0xd5:
      case 0xd6:
      case 0xd7:
      case 0xd8:
      case 0xd9:
      case 0xda:
      case 0xdb:
      case 0xdc:
      case 0xdd:
      case 0xde:
      case 0xdf:
      case 0xe0:
      case 0xe1:
      case 0xe2:
      case 0xe3:
      case 0xe4:
      case 0xe5:
      case 0xe6:
      case 0xe7:
      case 0xe8:
      case 0xe9:
      case 0xea:
      case 0xeb:
      case 0xec:
      case 0xed:
      case 0xee:
      case 0xef:
      case 0xf0:
      case 0xf1:
      case 0xf2:
      case 0xf3:
      case 0xf4:
      case 0xf5:
      case 0xf6:
      case 0xf7:
      case 0xf8:
      case 0xf9:
      case 0xfa:
      case 0xfb:
      case 0xfc:
      case 0xfd:
      case 0xfe:
      case 0xff:
        break;
      case 0x20:
        _sprintf(line,str_d_scenarioloaded,num);
        break;
      case 0x40:
        if (event->denominator == 0) {
          _sprintf(line,str_d_randomturn,num);
        }
        else {
          _sprintf(line,str_d_randomturn_denominator_d,num,(uint)event->denominator);
        }
        break;
      case 0x80:
        if (event->whom_name == 0) {
          _sprintf(line,str_d_noschism,num);
        }
        else {
          _sprintf(line,str_d_noschism_s,num,(uint)event->whom_name);
        }
        break;
      case 0x100:
        if (event->whom_name == 0) {
          _sprintf(line,str_d_receivedtechnology,num);
        }
        else {
          trigger = (uint)event->whom_name;
          tech_name = (i32)label_string(advances[(uint)event->technology].name);
          _sprintf(line,str_d_receivedtechnology_s_whom_s,num,tech_name,trigger);
        }
      }
    }
    editor_active.wnd->trigger_listbox.add_string((const char *)line);
  }
  events_editor_update_buttons(0);
  return;
}

/* 0054bc1a auto */
void __cdecl events_editor_fill_actions(i32 unused)

{
  int next_idx;
  uint action;
  i32 name_str;
  i32 civ_name;
  bool more;
  char who_str[16];
  ScenarioEvent *event;
  uint action_bit;
  char line[256];
  
  action_bit = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  while ((event != 0 &&
         (next_idx = action_bit - 1, more = action_bit != 0, action_bit = next_idx, more))) {
    event = event->next;
  }
  editor_active.wnd->action_listbox.clear();
  action_bit = 1;
  if (event != 0) {
    do {
      line[0] = '\0';
      action = event->actions & action_bit;
      if (action < 9) {
        if (action == 8) {
          if (event->money_receiver_name == 0) {
            _sprintf(line,str_changemoney_3);
          }
          else {
            _sprintf(line,str_changemoney_receiver_s_amount_d,
                     (i32)event->money_receiver_name,event->money_amount);
          }
        }
        else if (action == 1) {
          if (event->text[0] == 0) {
            _sprintf(line,((i8 *)&events_editor_text_caption));
          }
          else {
            _sprintf(line,str_text_s,(i32)event->text[0]);
          }
        }
        else if (action == 2) {
          if ((event->move_unit_name == 0) || (event->move_owner_name == 0)) {
            _sprintf(line,str_moveunit_3);
          }
          else {
            _sprintf(line,str_moveunit_s_owner_s,(i32)event->move_unit_name,
                     (i32)event->move_owner_name);
          }
        }
        else if (action == 4) {
          if (((event->create_unit_name == 0) || (event->create_owner_name == 0)) ||
             (event->create_homecity == 0)) {
            _sprintf(line,str_createunit_3);
          }
          else {
            _sprintf(line,str_createunit_s_owner_s_veteran_s_homecity,
                     (i32)event->create_unit_name,(i32)event->create_owner_name,
                     ((i8 *)&events_editor_yes_no_strs) + ((event->create_veteran != 0) - 1 & 4),
                     (i32)event->create_homecity);
          }
        }
      }
      else if (action < 0x21) {
        if (action == 0x20) {
          if ((event->aggression_who_name == 0) || (event->aggression_whom_name == 0)) {
            _sprintf(line,str_makeaggression_3);
          }
          else {
            _sprintf(line,str_makeaggression_aggressor_s_victim_s,
                     (i32)event->aggression_who_name,(i32)event->aggression_whom_name);
          }
        }
        else if (action == 0x10) {
          if (event->wave_file == 0) {
            _sprintf(line,str_playwavefile_3);
          }
          else {
            _sprintf(line,str_playwavefile_s,(i32)event->wave_file);
          }
        }
      }
      else if (action < 0x81) {
        if (action == 0x80) {
          if (event->cd_track == 0) {
            _sprintf(line,str_playcdtrack_3);
          }
          else {
            _sprintf(line,str_playcdtrack_d,event->cd_track);
          }
        }
        else if (action == 0x40) {
          _sprintf(line,str_justonce_3);
        }
      }
      else if (action < 0x201) {
        if (action == 0x200) {
          name_str = (i32)label_string(terrain_types[event->terrain_type].name);
          _sprintf(line,str_changeterrain_s,name_str);
        }
        else if (action == 0x100) {
          _sprintf(line,str_dontplaywonders_3);
        }
      }
      else if (action == 0x400) {
        if (event->destroy_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->destroy_civ == -3) {
              _sprintf(line,str_destroyacivilization_triggerattacker);
            }
            else {
              _sprintf(line,str_destroyacivilization_triggerdefender);
            }
          }
          else {
            _sprintf(line,str_destroyacivilization_triggerreceiver);
          }
        }
        else {
          name_str = (i32)(civ_name_plural(event->destroy_civ));
          _sprintf(line,str_destroyacivilization_s,name_str);
        }
      }
      else if (action == 0x800) {
        if (event->give_tech_civ == 0) {
          _sprintf(line,str_givetechnology_3);
        }
        else if (event->give_tech_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->give_tech_civ == -3) {
              _sprintf(who_str,str_triggerattacker_4);
            }
            else {
              _sprintf(who_str,str_triggerdefender_4);
            }
          }
          else {
            _sprintf(who_str,str_triggerreceiver_4);
          }
          name_str = (i32)label_string(advances[event->give_tech].name);
          _sprintf(line,str_givetechnology_receiver_s_technology_s_2,who_str,name_str);
        }
        else {
          name_str = (i32)label_string(advances[event->give_tech].name);
          civ_name = (i32)(civ_name_plural(event->give_tech_civ));
          _sprintf(line,str_givetechnology_receiver_s_technology_s,civ_name,name_str);
        }
      }
      else if (action == 0x2000) {
        _sprintf(line,str_hastriggered_2);
      }
      if (line[0] != '\0') {
        editor_active.wnd->action_listbox.add_string((const char *)line);
      }
      action_bit = action_bit << 1;
    } while (action_bit != 0x8000);
  }
  events_editor_update_buttons(0);
  return;
}

/* 0054c36e auto */
void __cdecl events_editor_on_delete_trigger(void)

{
  int sel;
  i32 parent_wnd2;
  ScenarioEvent *event;
  i32 idx;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  sel = popup_game((const char *)&str_deleteitem);
  if (sel != 0) {
    sel = editor_active.wnd->trigger_listbox.get_cur_sel();
    idx = sel;
    for (event = unit_types_block.events_list_head; event != 0; event = event->next) {
      if (idx == 0) break;
      idx--;
    }
    if (event->prev != 0) {
      event->prev->next = event->next;
    }
    if (event->next != 0) {
      event->next->prev = event->prev;
    }
    if (sel == 0) {
      unit_types_block.events_list_head = event->next;
    }
    events_compact();
    events_editor_fill_triggers();
    events_editor_fill_actions(0);
  }
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* Part of events_editor_edit_trigger (0054c4a1): asks for the civ of a
   trigger (the list titled by the label title_label) into *civ, its name (or
   any_name, for any civ) into *civ_name. Returns 0 when cancelled (or out of
   memory). */
static int edit_trigger_pick_civ(int title_label,i32 *civ,char **civ_name,const char *any_name)
{
  size_t len;
  i32 kind;
  i32 title;
  uint value;
  uint choice;
  const char *name2;

  kind = 4;
  title = (i32)label_string(label_table.string_ids[title_label]);
  choice = events_editor_pick_item((uint)*civ,(const char *)title,kind);
  if (choice != 0xffffffff) {
    if ((int)choice < 0) {
      *civ = 0xfffffffe;
      name2 = any_name;
    }
    else {
      *civ = choice;
      name2 = civ_name_plural((uint)*civ);
    }
    len = _strlen(name2);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    *civ_name = (char *)value;
    if (*civ_name != 0) {
      crt_strcpy((char *)((void *)*civ_name),name2);
      return 1;
    }
  }
  return 0;
}

/* Part of events_editor_edit_trigger (0054c4a1): asks whether the talker or
   the listener of a negotiation is a human, the computer or either, into
   *party_type (1, 2, 4). Returns 0 when cancelled. */
static int edit_trigger_pick_party_type(i32 *party_type,const char *title)
{
  iptr parent_wnd;
  uint choice;

  if ((*party_type == 0) || ((uint)*party_type == 1)) {
    choice = 0;
  }
  else if ((uint)*party_type == 2) {
    choice = 1;
  }
  else {
    choice = 2;
  }
  if (editor_active.wnd == 0) {
    parent_wnd = 0;
  }
  else {
    parent_wnd = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd);
  choice = popup_text_run((iptr)&str_debug_11,title,choice,0,0,0,1);
  dlg_set_parent_window(0);
  if (-1 < (int)choice) {
    if (choice == 0) {
      *party_type = 1;
    }
    else if (choice == 1) {
      *party_type = 2;
    }
    else {
      *party_type = 4;
    }
    return 1;
  }
  return 0;
}

/* 0054c4a1 auto */
i32 __cdecl events_editor_edit_trigger(i32 unused)

{
  size_t len;
  int check;
  i32 title;
  uint value;
  bool more;
  i32 kind;
  iptr parent_wnd_h;
  iptr parent_wnd_b;
  iptr parent_wnd_a;
  iptr parent_wnd_d;
  iptr parent_wnd_c;
  iptr parent_wnd2;
  i32 changed;
  ScenarioEvent *event;
  char city_name[260];
  int skip;
  i32 number;
  int sel;
  uint choice;
  const char *name2;
  
  changed = 0;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  sel = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  skip = sel;
  while ((event != (ScenarioEvent *)0x0 &&
         (check = skip - 1, more = skip != 0, skip = check, more))) {
    event = event->next;
  }
  value = (uint)event->trigger;
  if (value < 9) {
    if (value == 8) {
      if (editor_active.wnd == 0) {
        parent_wnd_a = 0;
      }
      else {
        parent_wnd_a = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_a);
      choice = dlg_input_number((iptr)&str_debug_11,(const char *)&str_turninterval_3,(uint)event->turn,(iptr)&number);
      if (editor_active.wnd == 0) {
        parent_wnd_b = 0;
      }
      else {
        parent_wnd_b = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_b);
      if (choice == 0) {
        value = editor_clamp_value(number,0,0x7fff);
        event->turn = value;
        changed = 1;
      }
    }
    else if (value == 1) {
      kind = 1;
      title = (i32)label_string(label_table.string_ids[0x224]);
      choice = events_editor_pick_item((uint)event->unit_type,(const char *)title,kind);
      if (choice != 0xffffffff) {
        if ((int)choice < 0) {
          event->unit_type = 0xfffffffe;
          name2 = str_anyunit_3;
        }
        else {
          event->unit_type = choice;
          name2 = label_string(unit_types[(uint)event->unit_type].name);
        }
        len = _strlen(name2);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->unit_name = (char *)value;
        if (event->unit_name != 0) {
          crt_strcpy((char *)((void *)event->unit_name),name2);
          if (edit_trigger_pick_civ(0x225,&event->who_civ,&event->who_name,str_anybody_3) != 0) {
            if (edit_trigger_pick_civ(0x226,&event->whom_civ,&event->whom_name,str_anybody_4) != 0) {
              changed = 1;
            }
          }
        }
      }
    }
    else if (value == 2) {
      do {
        if (event->city_name == 0) {
          event->city_name = &events_city_default_str;
        }
        if (editor_active.wnd == 0) {
          parent_wnd_c = 0;
        }
        else {
          parent_wnd_c = (iptr)&editor_active.wnd->window;
        }
        dlg_set_parent_window(parent_wnd_c);
        choice = dlg_text_input((iptr)&str_debug_11,(const char *)&str_cityname,0x17,(uint)event->city_name,(iptr)city_name);
        dlg_set_parent_window(0);
      } while ((-1 < (int)choice) &&
              ((len = _strlen(city_name), len == 0 ||
               (check = editor_check_city_name((const char *)city_name), check == 0))));
      if ((-1 < (int)choice) && (len = _strlen(city_name), len != 0)) {
        len = _strlen(city_name);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->city_name = (char *)value;
        if (event->city_name != 0) {
          crt_strcpy((char *)((void *)event->city_name),city_name);
          if (edit_trigger_pick_civ(0x225,&event->who_civ,&event->who_name,str_anybody_5) != 0) {
            if (edit_trigger_pick_civ(0x226,&event->whom_civ,&event->whom_name,str_anybody_6) != 0) {
              changed = 1;
            }
          }
        }
      }
    }
    else if (value == 4) {
      if (editor_active.wnd == 0) {
        parent_wnd_d = 0;
      }
      else {
        parent_wnd_d = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_d);
      choice = dlg_input_number((iptr)&str_debug_11,(const char *)&str_turncount,(uint)event->turn,(iptr)&number);
      dlg_set_parent_window(0);
      if (choice == 0) {
        value = editor_clamp_value(number,0xffffffff,0x7fff);
        event->turn = value;
        changed = 1;
      }
    }
  }
  else {
    switch(value) {
    case 0x10:
      if (edit_trigger_pick_civ(0x227,&event->who_civ,&event->who_name,str_anybody_7) != 0) {
        if (edit_trigger_pick_party_type(&event->talker_type,(const char *)&str_talkertype_3) != 0) {
          if (edit_trigger_pick_civ(0x228,&event->whom_civ,&event->whom_name,str_anybody_8) != 0) {
            if (edit_trigger_pick_party_type(&event->listener_type,(const char *)&str_talkertype_4) != 0) {
              changed = 1;
            }
          }
        }
      }
      break;
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x5d:
    case 0x5e:
    case 0x5f:
    case 0x60:
    case 0x61:
    case 0x62:
    case 99:
    case 100:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x6f:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
    case 0x7b:
    case 0x7c:
    case 0x7d:
    case 0x7e:
    case 0x7f:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8a:
    case 0x8b:
    case 0x8c:
    case 0x8d:
    case 0x8e:
    case 0x8f:
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9a:
    case 0x9b:
    case 0x9c:
    case 0x9d:
    case 0x9e:
    case 0x9f:
    case 0xa0:
    case 0xa1:
    case 0xa2:
    case 0xa3:
    case 0xa4:
    case 0xa5:
    case 0xa6:
    case 0xa7:
    case 0xa8:
    case 0xa9:
    case 0xaa:
    case 0xab:
    case 0xac:
    case 0xad:
    case 0xae:
    case 0xaf:
    case 0xb0:
    case 0xb1:
    case 0xb2:
    case 0xb3:
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
    case 0xbb:
    case 0xbc:
    case 0xbd:
    case 0xbe:
    case 0xbf:
    case 0xc0:
    case 0xc1:
    case 0xc2:
    case 0xc3:
    case 0xc4:
    case 0xc5:
    case 0xc6:
    case 199:
    case 200:
    case 0xc9:
    case 0xca:
    case 0xcb:
    case 0xcc:
    case 0xcd:
    case 0xce:
    case 0xcf:
    case 0xd0:
    case 0xd1:
    case 0xd2:
    case 0xd3:
    case 0xd4:
    case 0xd5:
    case 0xd6:
    case 0xd7:
    case 0xd8:
    case 0xd9:
    case 0xda:
    case 0xdb:
    case 0xdc:
    case 0xdd:
    case 0xde:
    case 0xdf:
    case 0xe0:
    case 0xe1:
    case 0xe2:
    case 0xe3:
    case 0xe4:
    case 0xe5:
    case 0xe6:
    case 0xe7:
    case 0xe8:
    case 0xe9:
    case 0xea:
    case 0xeb:
    case 0xec:
    case 0xed:
    case 0xee:
    case 0xef:
    case 0xf0:
    case 0xf1:
    case 0xf2:
    case 0xf3:
    case 0xf4:
    case 0xf5:
    case 0xf6:
    case 0xf7:
    case 0xf8:
    case 0xf9:
    case 0xfa:
    case 0xfb:
    case 0xfc:
    case 0xfd:
    case 0xfe:
    case 0xff:
      break;
    case 0x20:
      changed = 1;
      break;
    case 0x40:
      value = editor_clamp_value((uint)event->denominator,1,1000);
      event->denominator = value;
      if (editor_active.wnd == 0) {
        parent_wnd_h = 0;
      }
      else {
        parent_wnd_h = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_h);
      choice = dlg_input_number((iptr)&str_debug_11,(const char *)&str_randomdenom,(uint)event->denominator,(iptr)&number);
      dlg_set_parent_window(0);
      if (choice == 0) {
        value = editor_clamp_value(number,1,1000);
        event->denominator = value;
        changed = 1;
      }
      break;
    case 0x80:
      if (edit_trigger_pick_civ(0x229,&event->whom_civ,&event->whom_name,str_anybody_9) != 0) {
        changed = 1;
      }
      break;
    case 0x100:
      if (edit_trigger_pick_civ(0x22a,&event->whom_civ,&event->whom_name,str_anybody_10) != 0) {
        kind = 2;
        title = (i32)label_string(label_table.string_ids[0x22b]);
        choice = events_editor_pick_item((uint)event->technology,(const char *)title,kind);
        if (choice != 0xffffffff) {
          event->technology = choice;
          changed = 1;
        }
      }
    }
  }
  events_compact();
  events_editor_fill_triggers();
  editor_active.wnd->trigger_listbox.set_cur_sel(sel);
  events_editor_update_buttons(0);
  events_editor_focus_redraw();
  dlg_set_parent_window(0);
  return changed;
}

/* 0054d4ca auto */
void __cdecl events_editor_on_edit_trigger(i32 mode)

{
  events_editor_edit_trigger(mode);
  return;
}

/* 0054d4e6 auto */
void __cdecl events_editor_on_add_trigger(void)

{
  int *new_event;
  LPCSTR lpCaption;
  LPCSTR lpText;
  NativeWindow *window;
  int result;
  UINT uType;
  iptr parent_wnd2;
  ScenarioEvent *event;
  int trigger_type;
  
  trigger_type = 0;
  while( true ) {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    dlg_input.help_pressed = 0;
    trigger_type = popup_game_ex((const char *)&str_triggers,trigger_type,1);
    dlg_set_parent_window(0);
    if (dlg_input.help_pressed == 0) break;
    events_editor_help_trigger(trigger_type);
  }
  /* BF-234 (Peter Johnson's ledger): one of the nine triggers (the list is
     from the game's text, which another can make longer); another adds none */
  if ((0 <= trigger_type) && (trigger_type < 9)) {
    if ((unit_types_block.events_pool_bytes_free < 0x1c4) ||
       (new_event = (int *)((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->alloc(),
       new_event == (int *)0x0)) {
      uType = 0;
      lpCaption = (LPCSTR)label_string(label_table.string_ids[0x234]);
      lpText = (LPCSTR)label_string(label_table.string_ids[0x23a]);
      window = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(window->hwnd,lpText,lpCaption,uType);
    }
    else {
      *new_event = 1 << ((byte)trigger_type & 0x1f);
      events_editor_fill_triggers();
      events_editor_fill_actions(0);
      result = editor_active.wnd->trigger_listbox.get_count();
      editor_active.wnd->trigger_listbox.set_cur_sel(result - 1);
      result = events_editor_edit_trigger(0);
      if (result == 0) {
        for (event = unit_types_block.events_list_head; (int)event->next != 0;
            event = event->next) {
        }
        if (event->prev == 0) {
          unit_types_block.events_list_head = event->next;
        }
        else {
          event->prev->next = event->next;
        }
        events_compact();
        events_editor_fill_triggers();
        events_editor_fill_actions(0);
      }
      events_editor_update_buttons(0);
    }
  }
  events_editor_focus_redraw();
  return;
}

/* 0054d6da auto */
void __cdecl events_editor_on_delete_action(void)

{
  int result;
  i32 action_sel;
  uint action_bit;
  i32 parent_wnd2;
  ScenarioEvent *event;
  i32 sel_index;
  
  sel_index = editor_active.wnd->trigger_listbox.get_cur_sel();
  for (event = unit_types_block.events_list_head; event != 0; event = event->next) {
    if (sel_index == 0) break;
    sel_index--;
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  result = popup_game((const char *)&str_deleteitem_2);
  if (result != 0) {
    action_sel = editor_active.wnd->action_listbox.get_cur_sel();
    result = bits_nth_set(action_sel,(i32)event->actions);
    if (result == 1) {
      event->text[0] = 0;
    }
    action_bit = bits_nth_set(action_sel,(i32)event->actions);
    event->actions = event->actions & ~action_bit;
    events_editor_fill_actions(0);
    events_compact();
  }
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* Part of events_editor_edit_action (0054d7ef): the next number of the line
   being read (text_next_token), clamped to 0..0x7fff: a coordinate. */
static i32 edit_action_read_coord(void)
{
  i32 arg2;
  i32 value;
  char *token;
  long parsed;

  arg2 = 0x7fff;
  value = 0;
  token = text_next_token();
  parsed = _atol(token);
  value = editor_clamp_value(parsed,value,arg2);
  return value;
}

/* Part of events_editor_edit_action (0054d7ef): the action CHANGEMONEY: the
   receiver and the amount. */
static void edit_action_change_money(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  char **name_out;
  iptr parent_wnd22;
  int number_value;
  int result;
  char *name_str;

  name_out = &name_str;
  value = event->trigger;
  arg2 = (i32)label_string(label_table.string_ids[0x22a]);
  result = events_editor_pick_action_civ(event->money_receiver_civ,(const char *)arg2,value,(iptr)name_out);
  if (result != -1) {
    event->money_receiver_civ = result;
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->money_receiver_name = (char *)value;
    if (event->money_receiver_name != 0) {
      crt_strcpy((char *)((void *)event->money_receiver_name),name_str);
      if (editor_active.wnd == 0) {
        parent_wnd22 = 0;
      }
      else {
        parent_wnd22 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd22);
      result = editor_input_long((const char *)&str_debug_11,(const char *)&str_amount_2,event->money_amount,(iptr)&number_value);
      dlg_set_parent_window(0);
      if (result == 0) {
        value = editor_clamp_value(number_value,0xffff8ad0,30000);
        event->money_amount = value;
        *changed = 1;
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action MOVEUNIT: the
   unit, its owner, the map rectangle, the destination and the number of
   units. */
static void edit_action_move_unit(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  const char **name_out;
  i32 default_value;
  iptr parent_wnd6;
  iptr parent_wnd5;
  iptr parent_wnd4;
  iptr parent_wnd3;
  char input_buf[260];
  int idx;
  int number_value;
  char default_text[64];
  int result;
  const char *name_str;

  arg2 = 3;
  value = (i32)label_string(label_table.string_ids[0x22d]);
  result = events_editor_pick_item(event->move_unit_type,(const char *)value,arg2);
  if (result != -1) {
    if (result < 0) {
      event->move_unit_type = 0xfffffffe;
      name_str = str_anyunit_4;
    }
    else {
      event->move_unit_type = result;
      name_str = label_string(unit_types[event->move_unit_type].name);
    }
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->move_unit_name = (char *)value;
    if (event->move_unit_name != 0) {
      crt_strcpy((char *)((void *)event->move_unit_name),name_str);
      name_out = &name_str;
      value = event->trigger;
      arg2 = (i32)label_string(label_table.string_ids[0x22e]);
      result = events_editor_pick_action_civ(event->move_owner_civ,(const char *)arg2,value,(iptr)name_out);
      if (result != -1) {
        event->move_owner_civ = result;
        len = _strlen(name_str);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->move_owner_name = (char *)value;
        if (event->move_owner_name != 0) {
          crt_strcpy((char *)((void *)event->move_owner_name),name_str);
          _sprintf(default_text,str_d_d_d_d_d_d_d_d,event->move_rect[0],event->move_rect[1],
                   event->move_rect[2],event->move_rect[3],event->move_rect[4],event->move_rect[5],
                   event->move_rect[6],event->move_rect[7]);
          if (editor_active.wnd == 0) {
            parent_wnd3 = 0;
          }
          else {
            parent_wnd3 = (iptr)&editor_active.wnd->window;
          }
          dlg_set_parent_window(parent_wnd3);
          do {
            result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_maprect_5,0x1f,(iptr)default_text,(iptr)input_buf)
            ;
            if (result == -1) break;
            len = _strlen(input_buf);
          } while (len == 0);
          dlg_set_parent_window(0);
          if (result != -1) {
            len = _strlen(input_buf);
            if (len != 0) {
              crt_strcpy(((i8 *)&textbuf.buf),input_buf);
              for (idx = 0; idx < 4; idx++) {
                event->move_rect[idx * 2] = edit_action_read_coord();
                event->move_rect[idx * 2 + 1] = edit_action_read_coord();
              }
            }
            _sprintf(default_text,str_d_d_5,event->move_to_x,event->move_to_y);
            if (editor_active.wnd == 0) {
              parent_wnd4 = 0;
            }
            else {
              parent_wnd4 = (iptr)&editor_active.wnd->window;
            }
            dlg_set_parent_window(parent_wnd4);
            result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_location,8,(iptr)default_text,(iptr)input_buf);
            if (editor_active.wnd == 0) {
              parent_wnd5 = 0;
            }
            else {
              parent_wnd5 = (iptr)&editor_active.wnd->window;
            }
            dlg_set_parent_window(parent_wnd5);
            if (result != -1) {
              len = _strlen(input_buf);
              if (len != 0) {
                crt_strcpy(((i8 *)&textbuf.buf),input_buf);
                event->move_to_x = edit_action_read_coord();
                event->move_to_y = edit_action_read_coord();
              }
              if (editor_active.wnd == 0) {
                parent_wnd6 = 0;
              }
              else {
                parent_wnd6 = (iptr)&editor_active.wnd->window;
              }
              dlg_set_parent_window(parent_wnd6);
              if (event->move_count == -2) {
                default_value = 0xffffffff;
              }
              else {
                default_value = event->move_count;
              }
              result = dlg_input_number((iptr)&str_debug_11,(const char *)&str_numunits,default_value,(iptr)&number_value);
              dlg_set_parent_window(0);
              if (result == 0) {
                if (number_value == -1) {
                  event->move_count = 0xfffffffe;
                }
                else {
                  value = editor_clamp_value(number_value,0,0x7fff);
                  event->move_count = value;
                }
                *changed = 1;
              }
            }
          }
        }
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action CREATEUNIT: the
   unit, its owner, veteran or not, its home city and up to 10 locations. */
static void edit_action_create_unit(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  int prev_count;
  char **name_out;
  iptr parent_wnd9;
  iptr parent_wnd8;
  iptr parent_wnd7;
  char input_buf[260];
  int idx;
  int choice;
  char default_text[64];
  int result;
  char *name_str;

  arg2 = 1;
  value = (i32)label_string(label_table.string_ids[0x22f]);
  result = events_editor_pick_item(event->create_unit_type,(const char *)value,arg2);
  if (result != -1) {
    event->create_unit_type = result;
    name_str = label_string(unit_types[event->create_unit_type].name);
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->create_unit_name = (char *)value;
    if (event->create_unit_name != 0) {
      crt_strcpy((char *)((void *)event->create_unit_name),name_str);
      name_out = &name_str;
      value = event->trigger;
      arg2 = (i32)label_string(label_table.string_ids[0x22e]);
      result = events_editor_pick_action_civ(event->create_owner_civ,(const char *)arg2,value,(iptr)name_out);
      if (result != -1) {
        event->create_owner_civ = result;
        len = _strlen(name_str);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->create_owner_name = (char *)value;
        if (event->create_owner_name != 0) {
          crt_strcpy((char *)((void *)event->create_owner_name),name_str);
          if (editor_active.wnd == 0) {
            parent_wnd7 = 0;
          }
          else {
            parent_wnd7 = (iptr)&editor_active.wnd->window;
          }
          dlg_set_parent_window(parent_wnd7);
          choice = popup_debug_ex((const char *)&str_veteran_2,event->create_veteran,0);
          dlg_set_parent_window(0);
          if (-1 < choice) {
            event->create_veteran = choice;
            do {
              if (event->create_homecity == 0) {
                event->create_homecity = (char *)(((i32)(i8 *)&events_homecity_default_str));
              }
              if (editor_active.wnd == 0) {
                parent_wnd8 = 0;
              }
              else {
                parent_wnd8 = (iptr)&editor_active.wnd->window;
              }
              dlg_set_parent_window(parent_wnd8);
              result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_homecityname,0x17,(i32)event->create_homecity,
                                            (iptr)input_buf);
              dlg_set_parent_window(0);
            } while ((-1 < result) &&
                    ((len = _strlen(input_buf), len == 0 ||
                     (prev_count = editor_check_city_name((const char *)input_buf), prev_count == 0))));
            if ((-1 < result) && (len = _strlen(input_buf), len != 0)) {
              len = _strlen(input_buf);
              value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
              event->create_homecity = (char *)value;
              if (event->create_homecity != 0) {
                crt_strcpy((char *)((void *)event->create_homecity),input_buf);
                for (idx = 0; idx < 10; idx++) {
                  event->create_location_count = idx;
                  _sprintf(default_text,str_d_d_6,event->create_locations[idx * 2],
                           event->create_locations[idx * 2 + 1]);
                  if (editor_active.wnd == 0) {
                    parent_wnd9 = 0;
                  }
                  else {
                    parent_wnd9 = (iptr)&editor_active.wnd->window;
                  }
                  dlg_set_parent_window(parent_wnd9);
                  text_set_number_arg(0,idx + 1);
                  result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_locationnum,8,(iptr)default_text,
                                                (iptr)input_buf);
                  dlg_set_parent_window(0);
                  if (result == -1) {
                    if (idx == 0) break;
                    do {
                      event->create_locations[idx * 2] = 0;
                      event->create_locations[idx * 2 + 1] = 0;
                      idx++;
                    } while (idx < 10);
                  }
                  else {
                    len = _strlen(input_buf);
                    if (len != 0) {
                      crt_strcpy(((i8 *)&textbuf.buf),input_buf);
                      event->create_locations[idx * 2] = edit_action_read_coord();
                      event->create_locations[idx * 2 + 1] = edit_action_read_coord();
                    }
                  }
                }
                if (event->create_location_count != 0) {
                  *changed = 1;
                }
              }
            }
          }
        }
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action MAKEAGGRESSION:
   who and whom. */
static void edit_action_make_aggression(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  int result;
  char *name_str;

  arg2 = 0;
  value = (i32)label_string(label_table.string_ids[0x230]);
  result = events_editor_pick_item(event->aggression_who_civ,(const char *)value,arg2);
  if (result != -1) {
    event->aggression_who_civ = result;
    name_str = civ_name_plural(event->aggression_who_civ);
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->aggression_who_name = (char *)value;
    if (event->aggression_who_name != 0) {
      crt_strcpy((char *)((void *)event->aggression_who_name),name_str);
      arg2 = 0;
      value = (i32)label_string(label_table.string_ids[0x231]);
      result = events_editor_pick_item(event->aggression_whom_civ,(const char *)value,arg2);
      if (result != -1) {
        event->aggression_whom_civ = result;
        name_str = civ_name_plural(event->aggression_whom_civ);
        len = _strlen(name_str);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->aggression_whom_name = (char *)value;
        if (event->aggression_whom_name != 0) {
          crt_strcpy((char *)((void *)event->aggression_whom_name),name_str);
          *changed = 1;
        }
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action PLAYWAVEFILE: the
   file (.WAV added when it has no extension). */
static void edit_action_play_wave(ScenarioEvent *event,uint *changed)
{
  i32 value;
  size_t len;
  char *token;
  iptr parent_wnd10;
  char input_buf[260];
  int result;

  if (event->wave_file == 0) {
    event->wave_file = (char *)(((i32)(i8 *)&events_wavfile_default_str));
  }
  if (editor_active.wnd == 0) {
    parent_wnd10 = 0;
  }
  else {
    parent_wnd10 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd10);
  do {
    result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_wavfile,10,(i32)event->wave_file,(iptr)input_buf);
    if (result == -1) break;
    len = _strlen(input_buf);
  } while (len == 0);
  dlg_set_parent_window(0);
  if (-1 < result) {
    len = _strlen(input_buf);
    if (len != 0) {
      token = _strchr(input_buf,0x2e);
      if (token == (char *)0x0) {
        crt_strcat(input_buf,((i8 *)&events_wav_ext_str));
      }
      __strupr(input_buf);
      len = _strlen(input_buf);
      value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
      event->wave_file = (char *)value;
      if (event->wave_file != 0) {
        crt_strcpy((char *)((void *)event->wave_file),input_buf);
        *changed = 1;
      }
    }
    else {
      *changed = 1;
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action CHANGETERRAIN:
   the terrain type and the map rectangle. */
static void edit_action_change_terrain(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  iptr parent_wnd12;
  char input_buf[260];
  int idx;
  char default_text[64];
  int result;

  arg2 = 7;
  value = (i32)label_string(label_table.string_ids[0x25c]);
  result = events_editor_pick_item(event->terrain_type,(const char *)value,arg2);
  if (result == -1) {
    result = -1;
  }
  else {
    event->terrain_type = result;
    _sprintf(default_text,str_d_d_d_d_d_d_d_d_2,event->terrain_rect[0],event->terrain_rect[1],
             event->terrain_rect[2],event->terrain_rect[3],event->terrain_rect[4],event->terrain_rect[5],event->terrain_rect[6],
             event->terrain_rect[7]);
    if (editor_active.wnd == 0) {
      parent_wnd12 = 0;
    }
    else {
      parent_wnd12 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd12);
    do {
      result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_maprect_6,0x1f,(iptr)default_text,(iptr)input_buf);
      if (result == -1) break;
      len = _strlen(input_buf);
    } while (len == 0);
    dlg_set_parent_window(0);
    if (result != -1) {
      len = _strlen(input_buf);
      if (len != 0) {
        crt_strcpy(((i8 *)&textbuf.buf),input_buf);
        for (idx = 0; idx < 4; idx++) {
          event->terrain_rect[idx * 2] = edit_action_read_coord();
          event->terrain_rect[idx * 2 + 1] = edit_action_read_coord();
        }
      }
      *changed = 1;
    }
  }
}

/* 0054d7ef auto */
uint __cdecl events_editor_edit_action(i32 mode)

{
  i32 arg2;
  uint action_bit;
  i32 value;
  int prev_count;
  bool nonzero;
  char **name_out;
  iptr parent_wnd11;
  iptr parent_wnd2;
  uint changed;
  ScenarioEvent *event;
  int idx;
  EditControl edit_ctl;
  int number_value;
  int choice;
  int result;
  char *name_str;
  i8 *seh_scope;
  
  edit_ctor(&edit_ctl);
  ON_SCOPE_EXIT(edit_dtor(&edit_ctl));
  changed = 0;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  idx = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  while ((event != (ScenarioEvent *)0x0 &&
         (prev_count = idx - 1, nonzero = idx != 0, idx = prev_count, nonzero))) {
    event = event->next;
  }
  value = (i32)event->actions;
  arg2 = editor_active.wnd->action_listbox.get_cur_sel();
  action_bit = bits_nth_set(arg2,value);
  if (action_bit < 9) {
    if (action_bit == 8) {
      edit_action_change_money(event,&changed);
    }
    else if (action_bit == 1) {
      value = (i32)label_string(label_table.string_ids[0x22c]);
      prev_count = editor_text_edit_dialog((((i32 *)event) + 0xe),(const char *)value);
      changed = (uint)(prev_count != 0);
    }
    else if (action_bit == 2) {
      edit_action_move_unit(event,&changed);
    }
    else if (action_bit == 4) {
      edit_action_create_unit(event,&changed);
    }
  }
  else if (action_bit < 0x21) {
    if (action_bit == 0x20) {
      edit_action_make_aggression(event,&changed);
    }
    else if (action_bit == 0x10) {
      edit_action_play_wave(event,&changed);
    }
  }
  else if (action_bit < 0x81) {
    if (action_bit == 0x80) {
      value = editor_clamp_value(event->cd_track,2,0x18);
      event->cd_track = value;
      text_set_number_arg(0,2);
      text_set_number_arg(1,0x18);
      if (editor_active.wnd == 0) {
        parent_wnd11 = 0;
      }
      else {
        parent_wnd11 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd11);
      result = dlg_input_number((iptr)&str_debug_11,(const char *)&str_cdtrack,event->cd_track,(iptr)&number_value);
      dlg_set_parent_window(0);
      if (result == 0) {
        value = editor_clamp_value(number_value,2,0x18);
        event->cd_track = value;
        changed = 1;
      }
    }
    else if (action_bit == 0x40) {
      changed = 1;
    }
  }
  else if (action_bit < 0x201) {
    if (action_bit == 0x200) {
      edit_action_change_terrain(event,&changed);
    }
    else if (action_bit == 0x100) {
      changed = 1;
    }
  }
  else if (action_bit == 0x400) {
    name_out = &name_str;
    value = event->trigger;
    arg2 = (i32)label_string(label_table.string_ids[0x232]);
    result = events_editor_pick_action_civ(event->destroy_civ,(const char *)arg2,value,(iptr)name_out);
    if (result != -1) {
      event->destroy_civ = result;
      changed = 1;
    }
  }
  else if (action_bit == 0x800) {
    name_out = &name_str;
    value = event->trigger;
    arg2 = (i32)label_string(label_table.string_ids[0x22a]);
    result = events_editor_pick_action_civ(event->give_tech_civ,(const char *)arg2,value,(iptr)name_out);
    if (result == -1) {
      result = -1;
    }
    else {
      event->give_tech_civ = result;
      arg2 = 2;
      value = (i32)label_string(label_table.string_ids[0x22b]);
      result = events_editor_pick_item(event->give_tech,(const char *)value,arg2);
      if (result != -1) {
        event->give_tech = result;
        changed = 1;
      }
    }
  }
  else if (action_bit == 0x2000) {
    changed = 1;
  }
  events_compact();
  events_editor_fill_actions(0);
  choice = bits_index_of(action_bit,(i32)event->actions);
  editor_active.wnd->action_listbox.set_cur_sel(choice);
  events_editor_update_buttons(0);
  events_editor_focus_redraw();
  dlg_set_parent_window(0);
  return changed;
}

/* 0054eeb4 auto */
void __cdecl events_editor_on_edit_action(i32 mode)

{
  events_editor_edit_action(mode);
  return;
}

/* 0054eed0 auto */
void __cdecl events_editor_on_add_action(void)

{
  LPCSTR lpCaption;
  LPCSTR lpText;
  i32 list_pos;
  int result;
  UINT uType;
  iptr parent_wnd2;
  ScenarioEvent *event;
  int action_index;
  int sel_index;
  
  action_index = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  while ((event != (ScenarioEvent *)0x0 && (action_index != 0))) {
    event = event->next;
    action_index--;
  }
  if (event->trigger == 0x20) {
    action_index = 0;
    while ((1 << ((byte)action_index & 0x1f) & ((int)event->actions | 0xfffffe6fU)) != 0) {
      action_index++;
    }
  }
  else {
    action_index = 0;
    while (((int)event->actions & 1 << ((byte)action_index & 0x1f)) != 0) {
      action_index++;
    }
  }
  if (action_index < 0xd) {
    while( true ) {
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      dlg_input.help_pressed = 0;
      if (event->trigger == 0x20) {
        action_index = editor_options_dialog((const char *)&str_actions,(const char *)action_index,(int)event->actions | 0xfffffe6f,1);
      }
      else {
        action_index = editor_options_dialog((const char *)&str_actions_2,(const char *)action_index,(int)event->actions,1);
      }
      dlg_set_parent_window(0);
      if (dlg_input.help_pressed == 0) break;
      events_editor_help_action(action_index);
    }
    if (action_index != -1) {
      event->actions = (int)event->actions | 1 << ((byte)action_index & 0x1f);
      list_pos = bits_index_of(1 << ((byte)action_index & 0x1f),(int)event->actions);
      events_editor_fill_actions(0);
      editor_active.wnd->action_listbox.set_cur_sel(list_pos);
      result = events_editor_edit_action(0);
      if (result == 0) {
        sel_index = editor_active.wnd->trigger_listbox.get_cur_sel();
        event = unit_types_block.events_list_head;
        while ((event != (ScenarioEvent *)0x0 && (sel_index != 0))) {
          event = (ScenarioEvent *)(int *)event->next;
          sel_index--;
        }
        event->actions =
             event->actions & ~(1 << ((byte)action_index & 0x1f));
        events_editor_fill_actions(0);
      }
    }
  }
  else {
    uType = 0;
    lpCaption = (LPCSTR)label_string(label_table.string_ids[0x234]);
    lpText = (LPCSTR)label_string(label_table.string_ids[0x233]);
    MessageBoxA((HWND)0x0,lpText,lpCaption,uType);
  }
  events_editor_focus_redraw();
  return;
}

/* 0054f16b auto */
void EditorWindow::events_editor_draw()

{
  u32 top;
  uptr line_height;
  iptr label;
  int bar_y;
  uint bar_height;
  int x;
  
  this->FrameWindow::draw_caption();
  gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1a);
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  x = client_x + (int)client_width / 2;
  top = client_y;
  line_height = font_height_uptr;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x235]);
  text_append_str((const char *)label);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,x,(top - line_height) + 0x19,0);
  top = client_y;
  line_height = font_height_uptr;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x236]);
  text_append_str((const char *)label);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,x,(top - line_height) + 0xc3,0);
  top = client_x;
  x = top + 0x211;
  bar_y = client_y + 0x1a;
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&events_pool_free_fmt),(uint)unit_types_block.events_pool_bytes_free);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,top + 0x216,(bar_y - font_height_uptr) - 2,0);
  bar_height = ((uint)unit_types_block.events_pool_bytes_free * 0x13b) / (uint)unit_types_block.events_pool_size;
  gfx_fill_rect_xywh((iptr)this,x,bar_y,10,bar_height,0x2a);
  gfx_fill_rect_xywh((iptr)this,x,bar_height + bar_y,10,0x13b - bar_height,0x6a);
  gfx_draw_frame_rect((iptr)this,x,bar_y,10,0x13b,10);
  gfx_draw_frame_rect((iptr)this,x,bar_y,10,bar_height,10);
  wnd_blit_surface();
  return;
}

/* 0054f3b9 auto */
void EditorWindow::events_editor_run()

{
  ScenarioEvents *this_00;
  uptr font_height;
  i32 title;
  int x;
  int y;
  iptr label;
  i32 style;
  i32 arg_x;
  i32 arg_y;
  u32 width;
  u32 height;
  i32 arg_a;
  i32 arg_b;
  i32 arg_c;
  u8 *parent6;
  u8 *parent5;
  u8 *parent4;
  u8 *list_parent2;
  u8 *list_parent;
  u8 *parent9;
  u8 *parent8;
  u8 *parent7;
  u8 *parent3;
  u8 *parent22;
  u8 *parent2;
  ScenarioEvents *backup;
  i8 rect[16];
  GamePalette palette;
  i8 *seh_scope;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (ScenarioEvents *)operator_new(0x310);
  if (this_00 == (ScenarioEvents *)0x0) {
    backup = (ScenarioEvents *)0x0;
  }
  else {
    backup = this_00->construct(50000);
  }
  (*(ScenarioEvents * *)&events_editor_backup) = backup;
  backup->reset(50000);
  events_copy((ScenarioEvents *)(iptr)(*(ScenarioEvents * *)&events_editor_backup),(ScenarioEvents *)&unit_types_block.scenario_events);
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
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  arg_c = 0;
  arg_b = 0;
  arg_a = 0;
  height = wnd_height_u;
  width = wnd_width_u;
  arg_y = 0;
  arg_x = 0;
  style = 0xd;
  title = (i32)label_string(label_table.string_ids[0x1cf]);
  this->FrameWindow::create((const char *)title,style,arg_x,arg_y,width,height,arg_a,arg_b,arg_c);
  x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = x + 8;
  button_width_u = 0x97;
  y = client_y + 0x90;
  x = client_x + 0x32;
  rect_set_xywh((iptr)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x237]);
  add_trigger_button.create((iptr)parent2,0x65,(iptr)rect,(const char *)label);
  add_trigger_button.set_callback((CtrlNotifyFn)events_editor_on_add_trigger);
  x = x + button_width_u + 3;
  rect_set_xywh((iptr)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent22 = (u8 *)0x0;
  }
  else {
    parent22 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x238]);
  edit_trigger_button.create((iptr)parent22,0x65,(iptr)rect,(const char *)label);
  edit_trigger_button.set_callback(events_editor_on_edit_trigger);
  rect_set_xywh((iptr)rect,x + button_width_u + 3,y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent3 = (u8 *)0x0;
  }
  else {
    parent3 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x239]);
  ((Button *)((u8 *)&delete_trigger_button_u))->create((iptr)parent3,0x65,(iptr)rect,(const char *)label);
  ((Button *)((u8 *)&delete_trigger_button_u))->set_callback((CtrlNotifyFn)events_editor_on_delete_trigger);
  y = client_y + 0x13a;
  x = client_x + 0x32;
  rect_set_xywh((iptr)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent7 = (u8 *)0x0;
  }
  else {
    parent7 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x237]);
  add_action_button.create((iptr)parent7,0x65,(iptr)rect,(const char *)label);
  add_action_button.set_callback((CtrlNotifyFn)events_editor_on_add_action);
  x = x + button_width_u + 3;
  rect_set_xywh((iptr)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent8 = (u8 *)0x0;
  }
  else {
    parent8 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x238]);
  edit_action_button.create((iptr)parent8,0x65,(iptr)rect,(const char *)label);
  edit_action_button.set_callback(events_editor_on_edit_action);
  rect_set_xywh((iptr)rect,x + button_width_u + 3,y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent9 = (u8 *)0x0;
  }
  else {
    parent9 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x239]);
  delete_action_button.create((iptr)parent9,0x65,(iptr)rect,(const char *)label);
  delete_action_button.set_callback((CtrlNotifyFn)events_editor_on_delete_action);
  rect_set_xywh((iptr)rect,client_x + 0x32,client_y + 0x1b,0x1cb,0x6e);
  x = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    list_parent = (u8 *)0x0;
  }
  else {
    list_parent = (u8 *)&window;
  }
  trigger_listbox.create((iptr)list_parent,x,(iptr)rect);
  trigger_listbox.set_font((iptr)&editor_font_block.editor_font);
  trigger_listbox.set_select_callback(events_editor_fill_actions);
  trigger_listbox.set_dblclick_callback(events_editor_on_edit_trigger);
  events_editor_fill_triggers();
  trigger_listbox.set_cur_sel(0);
  rect_set_xywh((iptr)rect,client_x + 0x32,client_y + 0xc5,0x1cb,0x6e);
  x = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    list_parent2 = (u8 *)0x0;
  }
  else {
    list_parent2 = (u8 *)&window;
  }
  action_listbox.create((iptr)list_parent2,x,(iptr)rect);
  action_listbox.set_font((iptr)&editor_font_block.editor_font);
  action_listbox.set_select_callback(events_editor_update_buttons);
  action_listbox.set_dblclick_callback(events_editor_on_edit_action);
  events_editor_fill_actions(0);
  events_editor_update_buttons(0);
  button_width_u = (int)(client_width - 8) / 3;
  y = (client_y + client_height) - (button_height_u + 2);
  x = client_x + 2;
  rect_set_xywh((iptr)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent4 = (u8 *)0x0;
  }
  else {
    parent4 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)parent4,0x65,(iptr)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)events_editor_on_ok);
  x = x + button_width_u + 2;
  rect_set_xywh((iptr)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent5 = (u8 *)0x0;
  }
  else {
    parent5 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((iptr)parent5,0x66,(iptr)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)events_editor_on_help);
  rect_set_xywh((iptr)rect,x + button_width_u + 2,y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent6 = (u8 *)0x0;
  }
  else {
    parent6 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)parent6,0x66,(iptr)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)events_editor_on_cancel);
  cancel_button.set_cancel_button();
  window.set_keyboard_nav(0);
  editor_ui.value_clamped = 0;
  set_redraw_callback(events_editor_focus_redraw);
  repaint();
  this->SurfaceWindow::show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (editor_lists.running != 0) {
    win_pump_one_message();
  }
  if ((*(ScenarioEvents * *)&events_editor_backup) != (ScenarioEvents *)0x0) {
    (*(ScenarioEvents * *)&events_editor_backup)->events_delete(1);
  }
  (*(ScenarioEvents * *)&events_editor_backup) = (ScenarioEvents *)0x0;
  return;
}

/* 0054ffc8 auto */
void __cdecl events_editor_open(void)

{
  EditorWindow editor_wnd;
  i8 *seh_scope;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.events_editor_run();
  return;
}

/* 00551cd0 auto */
ScenarioEvents *ScenarioEvents::events_delete(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
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
      answer = popup_game_with_sprite((const char *)&str_reallycheat_2,(iptr)&popup_info_icon_sprite,0);
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
      popup_game_with_sprite((const char *)&str_passwordnocheat1,(iptr)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite((const char *)&str_passwordnocheat2,(iptr)&popup_info_icon_sprite,0);
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
    for (advance = 0; advance < 100; advance++) {
      civ_give_advance(civ,advance,0,0,0);
    }
    civs[civ].future_techs = 1;
    popup_debug_text((const char *)&str_gavetech);
  }
  else {
    for (advance = 0; advance < 0xd; advance++) {
      *(i8 *)&civs[civ].advances[advance] = 0;
    }
    for (advance = 0; advance < 100; advance++) {
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
  iptr advance2;
  i8 bit_mask[4];
  Dialog dialog2;
  int last_choice;
  i8 *seh_scope;
  
  last_choice = -1;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  blink.unit_active = 0;
  civ = dbg_pick_player(0);
  if ((int)civ >= 0) {
    while( true ) {
      dialog2.load_game_section((const char *)&str_edittech,1);
      for (advance = 0; advance < 100; advance++) {
        if (advances[advance].enabled != '\0') {
          text_clear();
          text_append_string_id(advances[advance].name);
          text_append_space();
          has_it = civ_has_advance(civ,advance);
          if (has_it == 0) {
            has_preq1 = civ_has_advance(civ,(int)advances[advance].preq[0]);
            if ((has_preq1 == 0) ||
               (advance3 = civ_has_advance(civ,(int)advances[advance].preq[1]), advance3 == ADV_ADVANCED_FLIGHT)) {
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
          bit_index_split(advance3,(iptr)&advance2,(iptr)bit_mask);
          civs[civ].advances[advance2] =
               civs[civ].advances[advance2] & ~bit_mask[0];
          civ_byte = (byte)civ;
          game.advance_known_by[advance3] =
               game.advance_known_by[advance3] & ~(byte)(1 << (civ_byte & 0x1f));
        }
        civs[civ].num_advances = '\0';
        last_choice = advance3;
        for (advance = 0; advance < 100; advance++) {
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
  Dialog dialog2;
  uptr terrain2;
  int y2;
  iptr dir;
  byte improvements2;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  result = map_in_bounds((int)map_cursor.x[0],(int)map_cursor.y[0]);
  if (result != 0) {
    result = tile_city_owner((int)map_cursor.x[0],(int)map_cursor.y[0]);
    if (result < 0) {
      tile = (byte *)tile_at((int)map_cursor.x[0],(int)map_cursor.y[0]);
      old_terrain_byte = tile_terrain((int)map_cursor.x[0],(int)map_cursor.y[0]);
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
            result = tile_visible_to((int)map_cursor.x[0],(int)map_cursor.y[0],civ);
            if (result != 0) {
              tile_update_known((int)map_cursor.x[0],(int)map_cursor.y[0],civ);
            }
          }
          map_redraw_area_all_views((int)map_cursor.x[0],(int)map_cursor.y[0],1,human_civ[0],1);
          return;
        }
      }
      dialog2.close();
      dialog2.init_defaults();
      dialog2.load_text((const char *)&str_debug_5,(const char *)&str_terrain1_2,1);
      for (terrain2 = 0; (int)terrain2 < 0xb; terrain2++) {
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
  Dialog dialog2;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (human_civ[0] != cheat_unit.last_human_civ) {
    cheat_unit.last_human_civ = human_civ[0];
    cheat_unit.owner_civ = 0xffffffff;
  }
  if ((int)cheat_unit.owner_civ < 0) {
    cheat_unit.owner_civ = human_civ[0];
  }
  while( true ) {
    dialog2.load_text((const char *)&str_debug_6,(const char *)&str_createunit_4,1);
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      if ((((unit_types[unit_type].obsolete_by < '\0') ||
           (obsolete_known = civ_has_advance(cheat_unit.owner_civ,(int)unit_types[unit_type].obsolete_by),
           obsolete_known == 0)) || (cheat_unit.show_obsolete != '\0')) &&
         ((cheat_unit.ignore_prereq != '\0' ||
          (has_prereq = civ_has_advance(cheat_unit.owner_civ,(int)unit_types[unit_type].prerequisite),
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
      unit = unit_create(unit,cheat_unit.owner_civ,(int)map_cursor.x[0],(int)map_cursor.y[0]);
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
  Dialog dialog2;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
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
  Dialog dialog2;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
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
  result = dlg_input_number((iptr)&str_debug_11,(const char *)&str_gameyear,(int)game.turn,(iptr)turn_value);
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
  Dialog dialog2;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
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
    result = dlg_input_number((iptr)&str_debug_11,(const char *)&str_money,civs[civ].gold,(iptr)&gold2);
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
  Dialog dialog2;
  u8 damage_input[4];
  iptr commodity;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  choice = map_in_bounds((int)map_cursor.x[0],(int)map_cursor.y[0]);
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
                  label = civ_get_adjective((int)civ);
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
              choice = dlg_input_number_game((const char *)&str_unithitpoints,(uint)units[unit].damage,(iptr)damage_input);
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
  Dialog dialog2;
  iptr wonder;
  i8 number_input[4];
  iptr improvement;
  iptr city2;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  in_bounds = map_in_bounds((int)map_cursor.x[0],(int)map_cursor.y[0]);
  if (in_bounds != 0) {
    city = city_at((int)map_cursor.x[0],(int)map_cursor.y[0]);
    if (city >= 0) {
      civ = cities[city].owner;
      while( true ) {
        text_set_number_arg(0,(cities[city].flags & CITYF_OBJECTIVE) >> 0x1a);
        text_clear();
        text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
        text_append_space();
        text_append_lparen();
        adjective = civ_get_adjective((int)civ);
        text_append((const char *)adjective);
        text_append_rparen();
        text_set_string_arg(0,(const char *)&textbuf.buf);
        choice = popup_game((const char *)&str_cityedit);
        if (choice == 0) break;
        if (choice == 1) {
          choice = dlg_input_number_game((const char *)&str_setcitysize,(int)cities[city].size,(iptr)number_input);
          if (choice == 0) {
            cities[city].size = number_input[0];
          }
          map_redraw_tile_all_views((int)cities[city].x,(int)cities[city].y);
          status_redraw(1);
        }
        else if (choice == 2) {
          for (wonder = 0; wonder < 0x1c; wonder++) {
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
          choice = dlg_input_number_game((const char *)&str_setcityshields,(int)cities[city].shield_box,(iptr)number_input);
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
  Dialog dialog2;
  u8 number_input[4];
  iptr civ2;
  iptr gov;
  int other_civ;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
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
          choice = dlg_input_number_game((const char *)&str_lastcontact,(int)civs[civ].last_contact_turn[civ2],(iptr)number_input);
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
                                     (iptr)number_input);
          if (choice == 0) {
            civs[civ].attitude[civ2] = number_input[0];
          }
        }
      }
      else if (choice == 4) {
        choice = dlg_input_number_game((const char *)&str_editbetray,(uint)civs[civ].reputation,(iptr)number_input);
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
        choice = dlg_input_number_game((const char *)&str_editprogress,(int)civs[civ].research_progress,(iptr)number_input);
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
        dialog2.load_text_section((const char *)&str_game_7,(const char *)&str_editkingname,0x17,(iptr)&civ_names[civ].leader_name,0,0,0,1);
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
          for (advance = 0; advance < 100; advance++) {
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
  Dialog dialog2;
  i8 number_input[4];
  int total_value;
  int objective_value;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
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
    name2 = (iptr)(civ_name_plural((int)scenario.objective_civ));
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
      result = dlg_input_number_game((const char *)&str_editvictoryobj,(int)(((i16 *)&scenario.year_increment))[choice],(iptr)number_input);
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
      result = dlg_input_number_game((const char *)&str_editparadigm,(int)scenario.tech_paradigm,(iptr)number_input);
      if (result == 0) {
        scenario.tech_paradigm = (*(i16 *)((char *)&number_input + 0));
      }
    }
    else if (choice == 2) {
      value = dlg_input_number_game((const char *)&str_editincrement,(int)scenario.year_increment[0],(iptr)number_input);
      if (value == 0) {
        scenario.year_increment[0] = (*(i16 *)((char *)&number_input + 0));
      }
    }
    else if (choice == 3) {
      value = dlg_input_number_game((const char *)&str_editstartyear,(int)scenario.start_year,(iptr)number_input);
      if (value == 0) {
        scenario.start_year = (*(i16 *)((char *)&number_input + 0));
      }
    }
    else if (choice == 4) {
      value = dlg_input_number_game((const char *)&str_editmaxturns,(int)scenario.max_turns,(iptr)number_input);
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
          value = (int)tile_at((int)cities[city].x,(int)cities[city].y);
          *(i8 *)(value + 4) = 0xff;
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
      value = dlg_game_text_input((const char *)&str_scenname,0x18,(char *)&scenario.name,(iptr)name_buf);
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
  surface.load_gif_resource(setup_picwnd_state.image_id + 900,10,0xc0,(iptr)&palette);
  win_flush_pending_msgs();
  ((DibSurface *)((i8 *)&setup_picwnd_state.surface))->DibSurface::create(width,height);
  gfx_copy_rect_same_pos((iptr)&surface,(iptr)&setup_picwnd_state.surface,0,0,width,height);
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
  /* inlined unwind funclet 00559f30 */ { ((DllHandle *)(((int)this) + 0x2f8))->free(); }
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
  gfx_copy_rect_xywh((iptr)&setup_picwnd_state.surface,(iptr)this,*rect - client_x,rect[1] - client_y,
                     *rect,rect[1],width,height);
  return;
}

/* 00559fcf auto */
void SetupPicWindow::draw_image(iptr rect)

{
  gfx_set_draw_target((iptr)this);
  text_set_surface((iptr)this);
  blit_image_rect((int *)rect);
  return;
}

/* 0055a00b auto */
void SetupPicWindow::setup_picwnd_client_rect(iptr rect,int x,int y,iptr width,iptr height)

{
  rect_set_xywh(rect,client_x + x,client_y + y,width,height);
  return;
}

/* 0055a051 auto */
void SetupPicWindow::redraw()

{
  layout();
  setup_picwnd_load_image(rates_civ,max_rate_value);
  setup_picwnd_client_rect((iptr)&sci_rate_sel,0,0,rates_civ,max_rate_value);
  gfx_set_draw_target((iptr)this);
  draw_caption();
  draw_image((iptr)&sci_rate_sel);
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
setup_picwnd_frame_rect(iptr rect,i32 x,i32 y,int width,int height)

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
    setup_picwnd_frame_rect((iptr)&rect.left,x,y,rates_civ,max_rate_value);
    style = 8;
    y = 8;
    x = 0;
    height = rect_height((RECT *)&rect.left);
    height -= ui_metrics.dlgframe_h2;
    width = rect_width((RECT *)&rect.left);
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

/* 00568b00 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl status_window_global_init(void)

{
  status_window_static_init();
  status_window_reg_atexit();
  return;
}

/* 00568b1a auto */
void __cdecl status_window_static_init(void)

{
  ((FrameWindow *)((i8 *)&status_wnd.window))->FrameWindow::construct();
  return;
}

/* 00568b34 auto */
void __cdecl status_window_reg_atexit(void)

{
  _atexit(status_window_static_dtor);
  return;
}

/* 00568b51 auto */
void __cdecl status_window_static_dtor(void)

{
  ((FrameWindow *)((i8 *)&status_wnd.window))->FrameWindow::destruct();
  return;
}

/* 00568b6b auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl status_info_font_global_init(void)

{
  status_info_font_static_init();
  status_info_font_reg_atexit();
  return;
}

/* 00568b85 auto */
void __cdecl status_info_font_static_init(void)

{
  ((GameFont *)((i8 *)&status_wnd.info_font))->init_empty();
  return;
}

/* 00568b9f auto */
void __cdecl status_info_font_reg_atexit(void)

{
  _atexit(status_info_font_static_dtor);
  return;
}

/* 00568bbc auto */
void __cdecl status_info_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&status_wnd.info_font))->destroy();
  return;
}

/* 00568bd6 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl status_font_global_init(void)

{
  status_font_static_init();
  status_font_reg_atexit();
  return;
}

/* 00568bf0 auto */
void __cdecl status_font_static_init(void)

{
  ((GameFont *)((i8 *)&status_font_block.status_font))->init_empty();
  return;
}

/* 00568c0a auto */
void __cdecl status_font_reg_atexit(void)

{
  _atexit(status_font_static_dtor);
  return;
}

/* 00568c27 auto */
void __cdecl status_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&status_font_block.status_font))->destroy();
  return;
}

/* 00568c41 auto */
void __cdecl status_wnd_on_minimize(void)

{
  status_bar.wnd_hidden = 1;
  return;
}

/* 00568c58 auto */
void __cdecl status_wnd_on_restore(void)

{
  ((CityWindow *)((i8 *)&city_wnd.window))->bring_to_top();
  status_bar.wnd_hidden = 0;
  status_wnd_repaint();
  return;
}

/* 00568c7e auto */
i32 __cdecl status_wnd_on_close(void)

{
  status_bar.wnd_hidden = 1;
  ((WndBase *)((i8 *)&status_wnd.wnd))->minimize();
  return 0;
}

/* 00568ca2 auto */
void __cdecl status_calc_font_sizes(void)

{
  int lower_height;
  uint info_scale;
  int font_scale;
  
  frame_texture_surface_block.status_text_bottom = 0;
  font_scale = 2;
  info_scale = 2;
  status_font_block.status_panel_x = 0;
  status_font_block.status_panel_width = status_wnd.client_width;
  lower_height = status_wnd.client_height - 0x28;
  status_wnd.upper_panel_height = 0x28;
  if ((0xf4 < status_wnd.client_width) && (0x12e < lower_height)) {
    info_scale = 4;
    status_wnd.upper_panel_height = 0x50;
    lower_height = status_wnd.client_height - 0x50;
  }
  if (((info_scale == 2) && (0xb9 < status_wnd.client_width)) && (0xed < lower_height)) {
    info_scale = 3;
    status_wnd.upper_panel_height += (status_wnd.upper_panel_height >> 1);
    lower_height -= 0x14;
  }
  status_wnd.panel_gap = 8;
  if (lower_height < 0xb2) {
    if (lower_height < 0x10) {
      status_font_block.status_lower_panel_height = 0;
    }
    else {
      status_font_block.status_lower_panel_height = lower_height - 8;
    }
  }
  else {
    status_font_block.status_lower_panel_height = lower_height - 8;
  }
  if ((info_scale == 4) && (0x128 < status_font_block.status_lower_panel_height)) {
    font_scale = 4;
  }
  if (((2 < info_scale) && (font_scale == 2)) && (0xfe < status_font_block.status_lower_panel_height)) {
    font_scale = 3;
  }
  if (font_scale != status_bar.font_scale) {
    status_bar.font_scale = font_scale;
    ((GameFont *)((i8 *)&status_font_block.status_font))->set_styled(0,font_scale * 0xb >> 1,1);
  }
  if (info_scale != status_bar.info_font_scale) {
    status_bar.info_font_scale = info_scale;
    ((GameFont *)((i8 *)&status_wnd.info_font))->set_styled(0,(int)(info_scale * 0xb) >> 1,1);
  }
  return;
}

/* 00568e86 auto */
void __cdecl status_draw_civ_color(iptr civ)

{
  i32 color;
  RECT rect;
  
  if ((status_bar.wnd_hidden == '\0') && (status_font_block.status_lower_panel_height != 0)) {
    ((DibSurface *)((i8 *)&status_wnd.window))->set_clip((RECT *)&status_wnd.lower_rect);
    rect.right = status_wnd.client_right;
    rect.bottom = status_wnd.client_bottom;
    rect.left = status_wnd.client_right - 8;
    rect.top = status_wnd.client_bottom - 6;
    color = civ_color_primary(civ);
    gfx_fill_rect((iptr)&status_wnd.window,(RECT *)&rect.left,color);
    win_flush_pending_msgs();
    ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((iptr)&rect.left);
    ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
    win_flush_pending_msgs();
  }
  return;
}

/* 00568f43 auto */
void __cdecl status_draw_end_of_turn(int blit)

{
  int text_y;
  i32 height;
  i32 width;
  int text_x;
  int line_height;
  i32 src_x;
  i32 src_y;
  
  if ((((status_bar.wnd_hidden == '\0') && (blink.unit_active != '\0')) && (status_bar.end_turn_visible != '\0')) &&
     (((game.transient_flags & TRANSIENT_END_OF_TURN) != 0 && (text_y = rect_height((RECT *)&frame_texture_surface_block.status_end_turn_rect), 0 < text_y)))) {
    text_set_surface((iptr)&status_wnd.window);
    ((DibSurface *)((i8 *)&status_wnd.window))->set_clip((RECT *)&frame_texture_surface_block.status_end_turn_rect);
    src_y = 0;
    src_x = 0;
    height = rect_height((RECT *)&frame_texture_surface_block.status_end_turn_rect);
    width = rect_width((RECT *)&frame_texture_surface_block.status_end_turn_rect);
    gfx_tile_fill((iptr)&status_wnd.window,(iptr)&panel_texture_surface,frame_texture_surface_block.status_end_turn_rect,frame_texture_surface_block.status_end_turn_rect_top,width,height,src_x,src_y);
    text_set_colors((-(uint)((moving_units_mode == 0) == (bool)blink.end_turn_phase) & 0xf) + 0x1a,10,1,0);
    text_set_font((iptr)&status_font_block.status_font);
    text_x = ((GameFont *)((i8 *)&status_font_block.status_font))->text_width((const char *)&str_x_4);
    text_x = frame_texture_surface_block.status_end_turn_rect + text_x;
    line_height = ((GameFont *)((i8 *)&status_font_block.status_font))->font_cached_height();
    text_y = frame_texture_surface_block.status_end_turn_rect_top;
    text_clear();
    text_append_label(0x1b);
    gfx_draw_text((const char *)&textbuf.buf,text_x,text_y);
    text_clear();
    text_append_space();
    text_append_lparen();
    text_append_label(0x1c);
    text_append_rparen();
    gfx_draw_text((const char *)&textbuf.buf,text_x,text_y + line_height - 1);
    win_flush_pending_msgs();
    if (blit != 0) {
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((iptr)&frame_texture_surface_block.status_end_turn_rect);
      win_flush_pending_msgs();
    }
    ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
  }
  return;
}

/* 0056911d auto */
i32 __cdecl status_draw_cursor_loc(iptr x,iptr y,i32 draw_x,iptr draw_y)

{
  iptr landmass;
  i32 result;
  
  text_clear();
  text_append_label(0xb);
  text_append_colon();
  text_append_lparen();
  text_append_int(x);
  text_append_comma();
  text_append_int(y);
  text_append_rparen();
  text_append_space();
  landmass = tile_landmass(x,y);
  text_append_int(landmass);
  result = gfx_draw_text((const char *)&textbuf.buf,draw_x,draw_y);
  return result;
}

/* 005691a1 auto */
void __cdecl status_append_unit_orders(iptr unit)

{
  iptr orders;
  uint improvements2;
  iptr city;
  
  orders = (iptr)units[unit].orders;
  if ((orders == ORDER_NONE) || (0xf < orders)) {
    text_append_label(0x1a);
  }
  else if (orders == ORDER_BUILD_ROAD) {
    improvements2 = tile_improvements((int)units[unit].x,(int)units[unit].y);
    if ((improvements2 & TILE_ROAD) == 0) {
      text_append_string_id(order_names[5].name);
    }
    else {
      text_append_label(0xa0);
    }
  }
  else {
    text_append_string_id(order_names[orders].name);
    if (orders == ORDER_GOTO) {
      text_append_space();
      city = city_at((int)units[unit].goto_x,(int)units[unit].goto_y);
      if ((((-1 < city) && (game.reveal_map == '\0')) &&
          ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) == 0)) &&
         ((int)cities[city].owner != (uint)(byte)(*(i8 *)&human_civ))) {
        city = -1;
      }
      if (city < 0) {
        text_append_lparen();
        text_append_int((int)units[unit].goto_x);
        text_append_comma();
        text_append_int((int)units[unit].goto_y);
        text_append_rparen();
      }
      else {
        text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
      }
    }
  }
  return;
}

/* 00569363 auto */
void __cdecl status_draw_info(int full_redraw)

{
  short progress;
  i32 height;
  i32 width;
  int percent;
  uint rect_w;
  i32 src_x;
  i32 src_y;
  int icon_offset;
  i8 warming_rect[16];
  i8 flask_rect[16];
  int text_y2;
  int text_x2;
  int zoom2;
  int panel_width2;
  int icon_size;
  int bar_width;
  int icon_width;
  int icon_y;
  int icon_x;
  int stage;
  int line_x;
  int line_height;
  int year_end = 0;
  
  if (full_redraw != 0) {
    gfx_set_draw_target((iptr)&status_wnd.window);
    src_y = 0;
    src_x = 0;
    height = rect_height((RECT *)&frame_texture_surface_block.status_upper_rect);
    width = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
    gfx_tile_fill((iptr)&status_wnd.window,(iptr)&panel_texture_surface,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect_top,width,height,src_x,src_y);
  }
  text_set_surface((iptr)&status_wnd.window);
  text_set_font((iptr)&status_wnd.info_font);
  panel_width2 = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
  if (1 < game.peace_turns) {
    percent = int_clamp(game.peace_turns * 3,0,100);
    bar_width = (percent * panel_width2) / 100;
    if (bar_width != 0) {
      gfx_draw_hline((iptr)&status_wnd.window,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect + bar_width - 1,frame_texture_surface_block.status_upper_rect_top,0x55);
      gfx_draw_hline((iptr)&status_wnd.window,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect + bar_width - 1,frame_texture_surface_block.status_upper_rect_top + 1,0x55);
    }
  }
  line_x = ((GameFont *)((i8 *)&status_wnd.info_font))->text_width((const char *)&str_x_5);
  line_x = frame_texture_surface_block.status_upper_rect + line_x;
  line_height = ((GameFont *)((i8 *)&status_wnd.info_font))->font_cached_height();
  line_height--;
  text_y2 = frame_texture_surface_block.status_upper_rect_top;
  text_x2 = line_x;
  zoom2 = (*(int *)&status_bar.info_font_scale) * 4 - 8;
  ((DibSurface *)((i8 *)&status_wnd.window))->set_clip((RECT *)&frame_texture_surface_block.status_upper_rect);
  text_set_colors(0x10,0x21,1,1);
  text_clear();
  text_append_population(human_civ[0],0xffffffff);
  text_append_space();
  text_append_label(0x1e);
  gfx_draw_text((const char *)&textbuf.buf,text_x2,text_y2);
  text_x2 = line_x;
  text_y2 += line_height;
  text_clear();
  text_append_year((int)game.year);
  if (extra_ui(X_GAME_TURN)) {
    /* X5: and the turn */
    text_append_space();
    text_append_lparen();
    text_append(extras_text("turn"));
    text_append_space();
    text_append_int((uint)game.turn);
    text_append_rparen();
    year_end = gfx_draw_text((const char *)&textbuf.buf,text_x2,text_y2);
  }
  else
  gfx_draw_text((const char *)&textbuf.buf,text_x2,text_y2);
  text_x2 = line_x;
  text_y2 += line_height;
  text_clear();
  text_append_gold(civs[human_civ[0]].gold);
  text_append_space();
  text_append_space();
  text_append_int((uint)civs[human_civ[0]].tax_rate);
  crt_strcat(((i8 *)&textbuf.buf),((i8 *)&status_tax_sep_str));
  text_append_int(10 - ((uint)civs[human_civ[0]].science_rate +
                          (uint)civs[human_civ[0]].tax_rate));
  crt_strcat(((i8 *)&textbuf.buf),((i8 *)&status_lux_sep_str));
  text_append_int((uint)civs[human_civ[0]].science_rate);
  text_x2 = gfx_draw_text((const char *)&textbuf.buf,text_x2,text_y2);
  icon_size = gfx_zoom_scale(0xe,zoom2);
  icon_width = icon_size;
  percent = rect_height((RECT *)&frame_texture_surface_block.status_upper_rect);
  icon_y = ((percent >> 1) + frame_texture_surface_block.status_upper_rect_top) - (icon_size >> 1);
  rect_w = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
  if ((int)(rect_w & 0xfffffffe) < 200) {
    icon_offset = 100;
  }
  else {
    icon_offset = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
    icon_offset = icon_offset >> 1;
  }
  icon_x = frame_texture_surface_block.status_upper_rect + icon_offset;
  if (extra_ui(X_GAME_TURN) && icon_x < year_end + 4)
    icon_x = year_end + 4;     /* X5: the icons after the turn */
  gfx_set_zoom_scale(zoom2);
  progress = civs[human_civ[0]].research_progress;
  stage = civ_research_cost(human_civ[0]);
  stage = ((int)progress << 2) / stage;
  stage = int_clamp(stage,0,3);
  ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites) + stage * 0x3c))->draw((iptr)flask_rect,(iptr)&status_wnd.window,icon_x,
             icon_y);
  icon_x += (icon_width * 3) / 2;
  if (game.warming_counter != '\0') {
    stage = int_clamp((int)game.warming_counter >> 2,0,3);
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites) + stage * 0x3c))->draw((iptr)warming_rect,(iptr)&status_wnd.window,icon_x,
               icon_y);
  }
  gfx_reset_zoom_scale();
  ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
  if (full_redraw != 0) {
    win_flush_pending_msgs();
    ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((iptr)&frame_texture_surface_block.status_upper_rect);
    win_flush_pending_msgs();
  }
  return;
}

/* Part of status_draw_unit_panel (00569801): the lines of a unit of the
   stack under the cursor (a foreign one: its owner and type; one of the
   player's: its home city, orders and type). */
static void status_draw_stack_unit(iptr unit,bool is_foreign,int info_x,int row_pos,int line_height)
{
  iptr str;

  if (is_foreign) {
    text_clear();
    str = civ_get_adjective((int)units[unit].owner);
    text_append_str((const char *)str);
    if (game.reveal_map != '\0') {
      text_append_space();
      text_append_lparen();
      text_append_int(unit);
      if ((unit_types[units[unit].type].role == ROLE_SETTLE) &&
         ((units[unit].flags & UNITF_AI_COLONIST) != 0)) {
        text_append((const char *)&str_empty_174);
      }
      text_append_rparen();
    }
    gfx_draw_text((const char *)&textbuf.buf,info_x,row_pos);
    if ((units[unit].flags & UNITF_VETERAN) != 0) {
      text_append_space();
      text_append_label(0xd);
    }
    text_clear();
    if ((units[unit].flags & UNITF_AUTOMATED) != 0) {
      text_append_label(0x5c);
      text_append_space();
    }
    text_append_string_id(unit_types[units[unit].type].name);
    gfx_draw_text((const char *)&textbuf.buf,info_x,row_pos + line_height);
    if (game.reveal_map != '\0') {
      text_clear();
      status_append_unit_orders(unit);
      gfx_draw_text((const char *)&textbuf.buf,info_x,row_pos + line_height + line_height);
    }
  }
  else {
    text_clear();
    str = unit_home_city_name(unit);
    text_append_str((const char *)str);
    if (game.reveal_map != '\0') {
      text_append_space();
      text_append_lparen();
      text_append_int(unit);
      if ((unit_types[units[unit].type].role == ROLE_SETTLE) &&
         ((units[unit].flags & UNITF_AI_COLONIST) != 0)) {
        text_append((const char *)&str_empty_175);
      }
      text_append_rparen();
    }
    gfx_draw_text((const char *)&textbuf.buf,info_x,row_pos);
    text_clear();
    status_append_unit_orders(unit);
    gfx_draw_text((const char *)&textbuf.buf,info_x,row_pos + line_height);
    text_clear();
    text_append_string_id(unit_types[units[unit].type].name);
    if (unit_types[units[unit].type].role == ROLE_TRADE) {
      text_append_colon();
      if ((char)units[unit].counter < '\0') {
        text_append_label(0x40);
      }
      else {
        text_append_string_id(commodity_names[(char)units[unit].counter]);
      }
    }
    else if ((units[unit].flags & UNITF_VETERAN) != 0) {
      text_append_space();
      text_append_lparen();
      text_append_label(0xd);
      text_append_rparen();
    }
    gfx_draw_text((const char *)&textbuf.buf,info_x,row_pos + line_height + line_height);
  }
}

/* Part of status_draw_unit_panel (00569801): the lines of the tile under the
   cursor: its terrain (polluted), special resource, hut, and the
   improvements the player knows of. */
static void status_draw_tile_info(iptr x2,iptr y2,int text_x,int *text_y2,int line_height)
{
  byte terrain;
  int value;
  byte *tile_ptr;

  terrain = tile_terrain(x2,y2);
  text_clear();
  text_append_lparen();
  text_append_string_id(terrain_types[terrain].name);
  tile_ptr = (byte *)tile_at(x2,y2);
  if ((*tile_ptr & 0x80) != 0) {
    text_append_comma();
    text_append_label(0x12);
  }
  text_append_rparen();
  gfx_draw_text((const char *)&textbuf.buf,text_x,*text_y2);
  *text_y2 += line_height;
  value = tile_resource(x2,y2);
  if (value != 0) {
    text_clear();
    text_append_lparen();
    text_append_string_id(terrain_types[value * 0xb + (uint)terrain].name);
    text_append_rparen();
    gfx_draw_text((const char *)&textbuf.buf,text_x,*text_y2);
    *text_y2 += line_height;
  }
  value = tile_has_hut(x2,y2);
  if (value != 0) {
    text_clear();
    text_append_lparen();
    text_append_label(0x1d);
    text_append_rparen();
    gfx_draw_text((const char *)&textbuf.buf,text_x,*text_y2);
    *text_y2 += line_height;
  }
  /* B11: between hot-seat players human_civ is -1: the original reads the
     knowledge map of civilization -1, past the table (docs/BUGS.md) */
  if ((game.reveal_map == '\0') &&
      (bugfix_ui() ? 0 < (*(i8 *)&human_civ) : ((*(i8 *)&human_civ) != 0))) {
    tile_ptr = known_at(x2,y2,(*(i8 *)&human_civ));
    terrain = *tile_ptr;
  }
  else {
    value = (int)tile_at(x2,y2);
    terrain = *(byte *)(value + 1);
  }
  if ((terrain & 0x1c) != 0) {
    text_clear();
    text_append_lparen();
    if ((terrain & 0x30) != 0) {
      if ((terrain & 0x20) == 0) {
        text_append_label(0x14);
      }
      else {
        text_append_label(0x13);
      }
      if ((terrain & 0xc) != 0) {
        text_append_comma();
      }
    }
    if ((terrain & 0xc) != 0) {
      if ((terrain & 8) == 0) {
        text_append_label(0x17);
      }
      else if ((terrain & 4) == 0) {
        text_append_label(0x16);
      }
      else {
        text_append_label(0x15);
      }
    }
    text_append_rparen();
    gfx_draw_text((const char *)&textbuf.buf,text_x,*text_y2);
    *text_y2 += line_height;
  }
  if ((terrain & 0x40) != 0) {
    text_clear();
    text_append_lparen();
    text_append_label((-(uint)((terrain & 2) == 0) & 0xffffffcd) + 0x61);
    text_append_rparen();
    gfx_draw_text((const char *)&textbuf.buf,text_x,*text_y2);
    *text_y2 += line_height;
  }
  if ((terrain & 0x80) != 0) {
    text_clear();
    text_append_lparen();
    text_append_label(0x2f);
    text_append_rparen();
    gfx_draw_text((const char *)&textbuf.buf,text_x,*text_y2);
    *text_y2 += line_height;
  }
}

/* Part of status_draw_unit_panel (00569801): the active unit: its picture,
   moves left (and range left, for an air unit), home city, owner and type. */
static void status_draw_active_unit(iptr unit2,int text_x,int *text_y2,int line_height,int zoom)
{
  u8 range;
  u8 turns_used;
  iptr info_y;
  iptr icon_width;
  int value;
  iptr str;
  int row_pos;
  uint move_mult;
  int scaled;

  gfx_draw_unit((iptr)&status_wnd.window,unit2,4,text_x,*text_y2,zoom - 8,0);
  info_y = *text_y2 - 2;
  icon_width = gfx_zoom_scale(0x40,zoom - 8);
  row_pos = text_x + icon_width + 2;
  value = gfx_zoom_scale(0x30,zoom - 8);
  *text_y2 = *text_y2 + value + 2;
  value = unit_max_moves(unit2);
  value -= (uint)units[unit2].moves_used;
  scaled = value % (int)(uint)cosmic.road_move_mult;
  move_mult = (uint)cosmic.road_move_mult;
  text_clear();
  text_append_label(0xc);
  text_append_colon();
  text_append_int(value / (int)move_mult);
  if (scaled != 0) {
    text_append_space();
    text_append_int(scaled);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&status_moves_frac_sep_str));
    text_append_int((uint)cosmic.road_move_mult);
  }
  if ('\0' < (char)unit_types[units[unit2].type].range) {
    text_append_space();
    text_append_lparen();
    range = unit_types[units[unit2].type].range;
    turns_used = units[unit2].counter;
    value = unit_max_moves(unit2);
    text_append_int((int)(((int)(char)range - (int)(char)turns_used) * value -
                            (uint)units[unit2].moves_used) / (int)(uint)cosmic.road_move_mult
                      );
    text_append_rparen();
  }
  gfx_draw_text((const char *)&textbuf.buf,row_pos,info_y);
  text_clear();
  str = unit_home_city_name(unit2);
  text_append_str((const char *)str);
  gfx_draw_text((const char *)&textbuf.buf,row_pos,info_y + line_height);
  text_clear();
  str = civ_get_adjective((int)units[unit2].owner);
  text_append_str((const char *)str);
  gfx_draw_text((const char *)&textbuf.buf,row_pos,info_y + line_height + line_height);
  text_clear();
  if ((units[unit2].flags & UNITF_AUTOMATED) != 0) {
    text_append_label(0x5c);
    text_append_space();
  }
  text_append_string_id(unit_types[units[unit2].type].name);
  if (unit_types[units[unit2].type].role == ROLE_TRADE) {
    text_append_colon();
    if ((char)units[unit2].counter < '\0') {
      text_append_label(0x40);
    }
    else {
      text_append_string_id(commodity_names[(char)units[unit2].counter]);
    }
  }
  else if ((units[unit2].flags & UNITF_VETERAN) != 0) {
    text_append_space();
    text_append_lparen();
    text_append_label(0xd);
    text_append_rparen();
  }
  gfx_draw_text((const char *)&textbuf.buf,text_x,*text_y2);
}

/* 00569801 auto */
void __cdecl status_draw_unit_panel(void)

{
  bool is_foreign;
  int text_x;
  int line_height;
  i32 width;
  int value;
  iptr unit3;
  int zoom;
  int row_pos;
  int info_x;
  int scaled;
  int text_y2;
  iptr unit;
  int shown_count;
  iptr y2;
  int reserved_lines;
  iptr x2;
  iptr unit2;
  
  status_draw_info(0);
  if (status_font_block.status_lower_panel_height != 0) {
    ((DibSurface *)((i8 *)&status_wnd.window))->set_clip((RECT *)&status_wnd.lower_rect);
    text_set_font((iptr)&status_font_block.status_font);
    text_x = ((GameFont *)((i8 *)&status_font_block.status_font))->text_width((const char *)&str_x_6);
    text_x = status_wnd.lower_rect + text_x;
    line_height = ((GameFont *)((i8 *)&status_font_block.status_font))->font_cached_height();
    zoom = status_wnd.lower_rect_top;
    line_height--;
    text_clear();
    text_append_label((moving_units_mode == 0) + 9);
    text_set_colors(0x29,10,1,0);
    width = rect_width((RECT *)&status_wnd.lower_rect);
    gfx_draw_text_centered((const char *)&textbuf.buf,status_wnd.lower_rect,zoom,width);
    text_set_colors(0x10,0x21,1,1);
    text_y2 = zoom + line_height + (line_height >> 1);
    zoom = status_bar.font_scale * 3;
    shown_count = 0;
    if (((moving_units_mode == 1) && (-1 < game.active_unit)) && (player_turn_active != '\0')) {
      shown_count = 1;
      unit2 = (iptr)game.active_unit;
      x2 = (iptr)units[unit2].x;
      y2 = (iptr)units[unit2].y;
      status_draw_active_unit(unit2,text_x,&text_y2,line_height,zoom);
    }
    else {
      x2 = (iptr)map_cursor.x[0];
      y2 = (iptr)map_cursor.y[0];
      unit2 = -1;
      status_draw_cursor_loc(x2,y2,text_x,text_y2);
    }
    text_y2 += line_height;
    value = map_in_bounds(x2,y2);
    if ((value == 0) ||
       ((game.reveal_map == '\0' &&
        (value = tile_visible_to(x2,y2,(*(i8 *)&human_civ)), value == 0)))) {
      text_clear();
      text_append_lparen();
      text_append_label(0xf5);
      text_append_rparen();
      gfx_draw_text((const char *)&textbuf.buf,text_x,text_y2);
      text_y2 += line_height;
    }
    else {
      status_draw_tile_info(x2,y2,text_x,&text_y2,line_height);
      text_y2 += (line_height >> 1);
      if ((game.transient_flags & TRANSIENT_END_OF_TURN) == 0) {
        reserved_lines = 1;
      }
      else {
        reserved_lines = 3;
      }
      unit3 = tile_first_unit(x2,y2);
      value = unit_stack_sum(unit3,2);
      for (unit = unit3; row_pos = status_wnd.lower_rect_bottom, -1 < unit;
          unit = unit_next_in_stack(unit)) {
        if (unit2 != unit) {
          is_foreign = false;
          if ((units[unit].owner != (*(i8 *)&human_civ)) &&
             (((is_foreign = true, unit != unit3 ||
               (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit3].visibility) == 0 &&
                ((int)units[unit3].owner != ((*(i8 *)&human_civ) & 0xffU))))) &&
              (game.reveal_map == '\0')))) break;
          if (value - 1 == shown_count) {
            reserved_lines--;
          }
          scaled = gfx_zoom_scale(0x30,zoom - 8);
          if (row_pos - (scaled + line_height * reserved_lines + 1) <= text_y2) {
            text_clear();
            text_append_lparen();
            text_append_int(value - shown_count);
            text_append_space();
            text_append_label(0x18);
            text_append_space();
            text_append_label((-(uint)(value - shown_count == 1) & 99) + 0x19);
            text_append_rparen();
            gfx_draw_text((const char *)&textbuf.buf,text_x,text_y2);
            text_y2 += line_height;
            break;
          }
          shown_count++;
          gfx_draw_unit((iptr)&status_wnd.window,unit,4,text_x,text_y2,zoom - 8,0);
          row_pos = text_y2 - 2;
          scaled = gfx_zoom_scale(0x40,zoom - 8);
          info_x = text_x + scaled + 2;
          scaled = gfx_zoom_scale(0x30,zoom - 8);
          text_y2 = text_y2 + scaled + 2;
          status_draw_stack_unit(unit,is_foreign,info_x,row_pos,line_height);
        }
      }
    }
    frame_texture_surface_block.status_text_bottom = text_y2;
    if ((game.transient_flags & TRANSIENT_END_OF_TURN) == 0) {
      status_bar.end_turn_visible = 0;
    }
    else {
      frame_texture_surface_block.status_end_turn_rect = status_wnd.lower_rect;
      frame_texture_surface_block.status_end_turn_rect_right = status_wnd.lower_rect_right;
      frame_texture_surface_block.status_end_turn_rect_bottom = status_wnd.lower_rect_bottom;
      frame_texture_surface_block.status_end_turn_rect_top = text_y2;
      while (zoom = rect_height((RECT *)&frame_texture_surface_block.status_end_turn_rect), line_height * 3 < zoom) {
        frame_texture_surface_block.status_end_turn_rect_top += line_height;
      }
      status_bar.end_turn_visible = 1;
      status_draw_end_of_turn(0);
      frame_texture_surface_block.status_text_bottom = frame_texture_surface_block.status_end_turn_rect_bottom;
    }
    ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
  }
  return;
}

/* 0056a65e auto */
void __cdecl status_redraw(int blit)

{
  i32 height;
  i32 width;
  i32 src_x;
  i32 src_y;
  RECT rect2;
  int old_bottom;
  
  if (status_bar.wnd_hidden == '\0') {
    old_bottom = frame_texture_surface_block.status_text_bottom;
    gfx_set_draw_target((iptr)&status_wnd.window);
    src_y = 0;
    src_x = 0;
    height = rect_height((RECT *)&frame_texture_surface_block.status_upper_rect);
    width = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
    gfx_tile_fill((iptr)&status_wnd.window,(iptr)&panel_texture_surface,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect_top,width,height,src_x,src_y);
    if (status_font_block.status_lower_panel_height != 0) {
      src_y = 0;
      src_x = 0;
      height = rect_height((RECT *)&status_wnd.lower_rect);
      width = rect_width((RECT *)&status_wnd.lower_rect);
      gfx_tile_fill((iptr)&status_wnd.window,(iptr)&panel_texture_surface,status_wnd.lower_rect,status_wnd.lower_rect_top,width,height,src_x,src_y);
    }
    status_calc_font_sizes();
    status_draw_unit_panel();
    if (blit != 0) {
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((iptr)&frame_texture_surface_block.status_upper_rect);
      rect2.left = status_wnd.lower_rect;
      rect2.top = status_wnd.lower_rect_top;
      rect2.right = status_wnd.lower_rect_right;
      rect2.bottom = frame_texture_surface_block.status_text_bottom;
      if (frame_texture_surface_block.status_text_bottom <= old_bottom) {
        rect2.bottom = old_bottom;
      }
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((iptr)&rect2);
    }
  }
  return;
}

/* 0056a787 auto */
void __cdecl status_wnd_repaint(void)

{
  i32 height;
  i32 width;
  int inner_height;
  i32 src_x;
  i32 src_y;
  RECT inner_rect;
  RECT bevel_rect;
  
  if ((status_bar.wnd_hidden == '\0') && (turn_loop.active != '\0')) {
    ((FrameWindow *)((i8 *)&status_wnd.window))->layout();
    status_calc_font_sizes();
    gfx_set_draw_target((iptr)&status_wnd.window);
    rect_set_xywh((iptr)&frame_texture_surface_block.status_upper_rect,(uint)(*(byte *)&status_font_block.status_panel_x),0,status_font_block.status_panel_width,status_wnd.upper_panel_height);
    rect_set_xywh((iptr)&status_wnd.lower_rect,(uint)(*(byte *)&status_font_block.status_panel_x),0,status_font_block.status_panel_width,status_font_block.status_lower_panel_height);
    rect_offset((iptr)&frame_texture_surface_block.status_upper_rect,status_wnd.client_x,status_wnd.client_y);
    rect_offset((iptr)&status_wnd.lower_rect,status_wnd.client_x,(uint)(*(byte *)&status_wnd.panel_gap) + frame_texture_surface_block.status_upper_rect_bottom);
    status_redraw(0);
    ((FrameWindow *)((i8 *)&status_wnd.window))->FrameWindow::draw_caption();
    ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
    if (status_font_block.status_lower_panel_height != 0) {
      rect_set((iptr)&bevel_rect.left,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect_bottom,frame_texture_surface_block.status_upper_rect_right,status_wnd.lower_rect_top);
      rect_inflate((iptr)&bevel_rect.left,ui_metrics.bevel_width,0);
      inner_rect.left = bevel_rect.left;
      inner_rect.top = bevel_rect.top;
      inner_rect.right = bevel_rect.right;
      inner_rect.bottom = bevel_rect.bottom;
      rect_deflate((iptr)&inner_rect.left,0,ui_metrics.bevel_width);
      src_y = 0;
      src_x = 0;
      height = rect_height((RECT *)&inner_rect.left);
      width = rect_width((RECT *)&inner_rect.left);
      gfx_tile_fill((iptr)&status_wnd.window,(iptr)&window_border_tile_surface,inner_rect.left,inner_rect.top,width,height,src_x,src_y);
      bevel_rect.bottom = bevel_rect.top + ui_metrics.bevel_width;
      gfx_fill_rect((iptr)&status_wnd.window,(RECT *)&bevel_rect.left,ui_metrics.highlight_color);
      inner_height = rect_height((RECT *)&inner_rect.left);
      rect_offset((iptr)&bevel_rect.left,0,ui_metrics.bevel_width + inner_height);
      gfx_fill_rect((iptr)&status_wnd.window,(RECT *)&bevel_rect.left,ui_metrics.shadow_color);
    }
    ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface();
  }
  return;
}

/* 0056a98b auto */
void __cdecl status_wnd_on_click(void)

{
  if (city_turn.processing == 0) {
    if ((game.transient_flags & TRANSIENT_END_OF_TURN) == 0) {
      if (moving_units_mode == 1) {
        ui_redraw_all_maps(0);
      }
      else {
        ui_focus_active_unit(1);
      }
    }
    else {
      player_turn_active = 0;
    }
  }
  else {
    city_window_modal_notice();
  }
  return;
}

/* 0056a9f4 auto */
void __cdecl status_calc_wnd_rect(void)

{
  i32 left;
  int top;
  iptr width;
  int client_height;
  
  left = minimap_wnd_rect_block.minimap_wnd_rect;
  top = rect_height((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  top = minimap_wnd_rect_block.minimap_wnd_rect_top + top;
  width = rect_width((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  client_height = main_frame_wnd_block.main_frame_wnd.client_height();
  rect_set_xywh((iptr)&minimap_wnd_rect_block.status_wnd_rect,left,top,width,(client_height - top) - 1 + ui_metrics.dlgframe_h2);
  return;
}

/* 0056aa7f auto */
void __cdecl status_wnd_on_size(void)

{
  ((WndBase *)((i8 *)&status_wnd.wnd))->get_rect_in_parent((iptr)&minimap_wnd_rect_block.status_wnd_rect);
  return;
}

/* 0056aaa5 auto */
void __cdecl status_wnd_on_move(void)

{
  ((WndBase *)((i8 *)&status_wnd.wnd))->get_rect_in_parent((iptr)&minimap_wnd_rect_block.status_wnd_rect);
  return;
}

/* 0056aacb auto */
void __cdecl status_wnd_open(void)

{
  int height;
  int width;
  i32 title;
  i32 style;
  i32 x;
  i32 y;
  i32 flags;
  i32 arg_a;
  i32 arg_b;
  
  status_bar.wnd_hidden = 0;
  if ((turn_loop.resume_civ == '\0') || (destroyed_civ_count_block.saved_layout_valid == 0)) {
    status_calc_wnd_rect();
  }
  arg_b = 0;
  arg_a = 0;
  flags = 6;
  height = rect_height((RECT *)&minimap_wnd_rect_block.status_wnd_rect);
  height -= ui_metrics.dlgframe_h2;
  width = rect_width((RECT *)&minimap_wnd_rect_block.status_wnd_rect);
  width -= ui_metrics.dlgframe_w2;
  style = 6;
  x = minimap_wnd_rect_block.status_wnd_rect;
  y = minimap_wnd_rect_block.status_wnd_rect_top;
  title = (i32)label_string(label_table.string_ids[0x5]);
  ((FrameWindow *)((i8 *)&status_wnd.window))->FrameWindow::create((const char *)title,style,x,y,width,height,flags,arg_a,arg_b);
  ((ModalMenuWindow *)((i8 *)&status_wnd.wnd))->set_min_size(0x28,0x4c);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_minimize_handler(status_wnd_on_minimize);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_restore_handler(status_wnd_on_restore);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_close_handler(status_wnd_on_close);
  ((WndBase *)((i8 *)&status_wnd.wnd))->set_icon(1);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_lbuttonup_handler((WndMouseFn)status_wnd_on_click);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_rbuttonup_handler((WndMouseFn)status_wnd_on_click);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_keydown_handler(map_window_key);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_char_handler((WndKeyFn)map_window_ascii);
  ((SurfaceWindow *)((i8 *)&status_wnd.window))->set_redraw_callback(status_wnd_repaint);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_size_handler((WndSizeFn)status_wnd_on_size);
  ((WndHandlers *)((i8 *)&status_wnd.wnd_events))->set_move_handler((WndMouseFn)status_wnd_on_move);
  ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
  if (game.multiplayer != 1) {
    ((SurfaceWindow *)((i8 *)&status_wnd.window))->SurfaceWindow::show();
  }
  return;
}

/* 0056ac46 auto */
void __cdecl status_wnd_close(void)

{
  status_bar.wnd_hidden = 1;
  ((DibSurface *)((i8 *)&status_wnd.window))->wnd_destroy_with_surface();
  return;
}

/* 0056ac67 auto */
void __cdecl status_draw_timer_bar(int percent,int color)

{
  int width;
  i32 height;
  i32 bar_width;
  RECT bar_rect;
  
  if ((status_wnd.timer_surface_init & 1) == 0) {
    status_wnd.timer_surface_init = status_wnd.timer_surface_init | 1;
    surface_ctor((DibSurface *)((i8 *)&status_wnd.timer_bg_surface));
    _atexit(status_timer_bg_surface_static_dtor);
  }
  bar_rect.left = 0;
  bar_rect.top = 0;
  bar_rect.right = 0;
  bar_rect.bottom = 0;
  if (((status_bar.wnd_hidden == '\0') && (status_font_block.status_lower_panel_height != 0)) &&
     ((1 << ((*(byte *)&net.mp_turn_civ) & 0x1f) & (uint)game.human_players) != 0)) {
    ((DibSurface *)((i8 *)&status_wnd.window))->set_clip((RECT *)&status_wnd.lower_rect);
    bar_rect.left = status_wnd.indicator_rect;
    bar_rect.bottom = status_wnd.client_bottom;
    bar_rect.right = status_wnd.client_right - 8;
    bar_rect.top = status_wnd.client_bottom - 6;
    width = rect_width((RECT *)&status_wnd.lower_rect);
    if (width != status_bar.timer_last_width) {
      status_bar.timer_last_width = rect_width((RECT *)&status_wnd.lower_rect);
      status_bar.timer_last_color = -1;
      status_bar.timer_last_percent = -1;
      ((DibSurface *)((i8 *)&status_wnd.timer_bg_surface))->alloc_dib8((iptr)&bar_rect);
      height = rect_height(&bar_rect);
      bar_width = rect_width(&bar_rect);
      gfx_copy_rect_xywh((iptr)&status_wnd.window,(iptr)&status_wnd.timer_bg_surface,bar_rect.left,bar_rect.top,0,0,bar_width,height);
    }
    if (((status_bar.timer_last_color != color) || (width = _memcmp(&bar_rect.left,((i32 *)&status_wnd.timer_last_rect),0x10), width != 0))
       || (status_bar.timer_last_percent != percent)) {
      status_bar.timer_last_color = color;
      status_wnd.timer_last_rect[0] = bar_rect.left;
      status_wnd.timer_last_rect_top = bar_rect.top;
      status_wnd.timer_last_rect_right = bar_rect.right;
      status_wnd.timer_last_rect_bottom = bar_rect.bottom;
      status_bar.timer_last_percent = percent;
      height = rect_height(&bar_rect);
      bar_width = rect_width(&bar_rect);
      gfx_copy_rect_xywh((iptr)&status_wnd.timer_bg_surface,(iptr)&status_wnd.window,0,0,bar_rect.left,bar_rect.top,bar_width,height);
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((iptr)&bar_rect);
      if (-1 < color) {
        width = rect_width(&bar_rect);
        bar_rect.right = bar_rect.left + (percent * width) / 100;
        gfx_fill_rect((iptr)&status_wnd.window,&bar_rect,color);
        win_flush_pending_msgs();
        ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((iptr)&bar_rect);
      }
      ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
      win_flush_pending_msgs();
    }
  }
  return;
}

/* 0056aeed auto */
void __cdecl status_timer_bg_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&status_wnd.timer_bg_surface));
  return;
}

/* 0056b810: written by hand in src/game/regargs.cpp */

/* 005899f0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl main_frame_bg_surface_global_init(void)

{
  main_frame_bg_surface_init();
  main_frame_bg_surface_atexit();
  return;
}

/* 00589a0a auto */
void __cdecl main_frame_bg_surface_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&main_frame_bg_surface_block.main_frame_bg_surface));
  return;
}

/* 00589a24 auto */
void __cdecl main_frame_bg_surface_atexit(void)

{
  _atexit(main_frame_bg_surface_static_dtor);
  return;
}

/* 00589a41 auto */
void __cdecl main_frame_bg_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&main_frame_bg_surface_block.main_frame_bg_surface));
  return;
}

/* 00589a5b auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __cdecl main_frame_create(void)

{
  int cd_ok;
  NativeWindow *wnd_info;
  i32 wave_rate;
  i32 wave_bits;
  i8 saved_rgb2[708];
  GamePalette palette;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  text_clear();
  text_append_label(2);
  WritePrivateProfileStringA
            (str_civilization_gold_16,str_window_name,((i8 *)&textbuf.buf),str_civ_ini_16);
  main_frame_wnd_block.main_frame_wnd.create_with_cmd_proc((const char *)&textbuf.buf,0x7c,0,0,600,400,(iptr)&game_palette_block.game_palette);
  cd_ok = file_require_cdrom((const char *)&str_civ2_civ2_exe_2);
  if (cd_ok != 0) {
    wave_bits = 8;
    wave_rate = 0x10000;
    wnd_info = main_frame_wnd_block.main_frame_wnd.get_window();
    snd_wave_open((i32)wnd_info->hwnd,wave_rate,wave_bits);
    menu_build_main();
    ((DibSurface *)((i8 *)&main_frame_bg_surface_block.main_frame_bg_surface))->load_gif_resource(0x5a,10,0xc0,(iptr)&palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface(&main_frame_bg_surface_block.main_frame_bg_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    main_frame_wnd_block.main_frame_wnd.set_icon(1);
    main_frame_bg_surface_block.main_frame_nonclient_width = main_frame_wnd_block.main_frame_wnd.frame_width();
    main_frame_bg_surface_block.main_frame_nonclient_height_i32 = main_frame_wnd_block.main_frame_wnd.nonclient_height();
    main_frame_wnd_block.main_frame_wnd.maximize();
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_rgb((iptr)saved_rgb2,10,0xec);
    intro_play_opening_video();
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->set_rgb(10,0xec,(iptr)saved_rgb2);
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->commit_range(10,0xec);
    main_frame_wnd_block.main_frame_wnd.wnd_handlers.set_close_handler(main_frame_on_close);
    main_frame_wnd_block.main_frame_wnd.wnd_handlers.set_keydown_handler(map_window_key);
    main_frame_wnd_block.main_frame_wnd.wnd_handlers.set_char_handler((WndKeyFn)map_window_ascii);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    wnd_flush_paint();
    main_frame_created = 1;
  }
  return (uint)(cd_ok != 0);
}

/* 00589c79 auto */
void __cdecl main_frame_destroy(void)

{
  main_frame_wnd_block.main_frame_wnd.destroy_native();
  main_frame_created = 0;
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
        popup_game_with_sprite((const char *)&str_emailnot,(iptr)&popup_info_icon_sprite,0);
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

/* 005d1f50 auto */
i32 __cdecl game_timer_create(TimerFn callback,i32 interval_ms,i32 repeat_count)

{
  TimerManager *mgr;
  i32 slot;
  TimerManager *new_mgr;
  
  if (wnd_modal_stack_below_block.game_timer_mgr == (TimerManager *)0x0) {
    mgr = (TimerManager *)operator_new(0x90);
    if (mgr == (TimerManager *)0x0) {
      new_mgr = (TimerManager *)0x0;
    }
    else {
      new_mgr = mgr->construct();
    }
    wnd_modal_stack_below_block.game_timer_mgr = new_mgr;
  }
  slot = wnd_modal_stack_below_block.game_timer_mgr->add(callback,interval_ms,repeat_count);
  return slot;
}

/* 005d2004 auto */
void __cdecl game_timer_destroy(iptr slot)

{
  if (wnd_modal_stack_below_block.game_timer_mgr == (TimerManager *)0x0) {
    dbg_log((const char *)&str_error_mrtimer_not_initialized_in_resetti);
  }
  else {
    wnd_modal_stack_below_block.game_timer_mgr->game_timer_kill_slot(slot);
  }
  return;
}

/* 005d2042 auto */
iptr TimerManager::add(TimerFn callback,i32 interval_ms,u32 repeat_count)

{
  int started;
  iptr slot;
  
  slot = 1;
  while ((slot < 0x11 && (((TimerManagerExt *)this)->timer_callbacks[slot] != 0))) {
    slot++;
  }
  if (slot < 0x11) {
    started = timer_wnd_start_slot(timer_hwnd,interval_ms,slot);
    if (started == 0) {
      slot = 0;
    }
    else {
      ((TimerManagerExt *)this)->timer_callbacks[slot] = callback;
      ((u32 *)&((TimerManagerExt *)this)->timer_repeat_counts)[slot] = repeat_count;
    }
  }
  else {
    slot = 0;
  }
  return slot;
}

/* 005d20e6 auto */
void TimerManager::game_timer_kill_slot(iptr slot)

{
  ((TimerManagerExt *)this)->timer_callbacks[slot] = 0;
  timer_wnd_kill_slot(timer_hwnd,slot);
  return;
}

/* 005d211e auto */
TimerManager *TimerManager::construct()

{
  u8 *entry;
  u32 hwnd;
  int i;
  
  for (i = 1; i < 0x11; i++) {
    entry = (u8 *)&((TimerManagerExt *)this)->timer_callbacks[i];
    entry[0] = '\0';
    entry[1] = '\0';
    entry[2] = '\0';
    entry[3] = '\0';
    entry = (u8 *)&((u32 *)&((TimerManagerExt *)this)->timer_repeat_counts)[i];
    entry[0] = '\0';
    entry[1] = '\0';
    entry[2] = '\0';
    entry[3] = '\0';
  }
  hwnd = timer_wnd_create((iptr)this);
  timer_hwnd = hwnd;
  return this;
}

/* 005d2182 auto */
void TimerManager::destruct()

{
  iptr slot;
  
  for (slot = 0; slot < 0x10; slot++) {
    if (((TimerManagerExt *)this)->timer_callbacks[slot] != 0) {
      game_timer_kill_slot(slot);
    }
  }
  timer_wnd_destroy(timer_hwnd);
  return;
}

/* 005d41e0 auto */
uint __cdecl timer_ticks_60hz(void)

{
  DWORD ticks;
  
  ticks = GetTickCount();
  return (ticks * 6) / 100;
}

/* 005d4204 auto */
void __cdecl timer_busy_wait_ticks(int ticks)

{
  int start_ticks;
  int now_ticks;
  
  start_ticks = timer_ticks_60hz();
  while( true ) {
    now_ticks = timer_ticks_60hz();
    if (start_ticks + ticks <= now_ticks) break;
    win_flush_pending_msgs();
  }
  return;
}

/* 005d423c auto */
HWND __cdecl timer_wnd_create(iptr timer_mgr)

{
  MMRESULT uTimerID;
  HWND hwnd2;
  
  hwnd2 = (HWND)0x0;
  timerdll.module = LoadLibraryA(str_timerdll_dll);
  if (timerdll.module == (HMODULE)0x0) {
    hwnd2 = CreateWindowExA(0,str_msmrtimerclass_2,str_mrtimer_2,0xcf0000,-0x80000000,
                              -0x80000000,-0x80000000,-0x80000000,(HWND)0x0,(HMENU)0x0,err_report_ctx_block.app_hinstance,
                              (LPVOID)0x0);
    if (hwnd2 != (HWND)0x0) {
      SetWindowLongA(hwnd2,0,timer_mgr);
      timer_mm_available = 0;
    }
  }
  else {
    timerdll.timer_callback = (LPTIMECALLBACK)GetProcAddress(timerdll.module,str_timercallback);
    timerdll.set_timer_id = (TimerDllSetTimerIdFn)GetProcAddress(timerdll.module,str_settimerid);
    timerdll.get_timer_id = (TimerDllGetTimerIdFn)GetProcAddress(timerdll.module,str_gettimerid);
    timerdll.get_timer_index = (TimerDllGetTimerIndexFn)GetProcAddress(timerdll.module,str_gettimerindex);
    timerdll.reset_timer_notified = (TimerDllResetNotifiedFn)GetProcAddress(timerdll.module,str_resettimernotified);
    if ((((timerdll.timer_callback == 0) || (timerdll.set_timer_id == 0)) ||
        (timerdll.get_timer_id == 0)) ||
       ((timerdll.get_timer_index == 0 || (timerdll.reset_timer_notified == 0)))) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(9,(iptr)&str_err_dynamiclinkfailed,(iptr)&str_d_ss_smeds32_pctimer_cpp,0x55);
      FreeLibrary(timerdll.module);
    }
    else {
      hwnd2 = CreateWindowExA(0,str_msmrtimerclass,str_mrtimer,0xcf0000,-0x80000000,
                                -0x80000000,-0x80000000,-0x80000000,(HWND)0x0,(HMENU)0x0,
                                err_report_ctx_block.app_hinstance,(LPVOID)0x0);
      if (hwnd2 == (HWND)0x0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(10,(iptr)&str_err_cantcreatewindow,(iptr)&str_d_ss_smeds32_pctimer_cpp_2,0x71);
        FreeLibrary(timerdll.module);
      }
      else {
        timeBeginPeriod(5);
        SetWindowLongA(hwnd2,0,timer_mgr);
        uTimerID = timeSetEvent(100,5,timerdll.timer_callback,hwnd2,1);
        if (uTimerID == 0) {
          timer_mm_available = 0;
        }
        else {
          timeKillEvent(uTimerID);
        }
      }
    }
  }
  return hwnd2;
}

/* 005d447c auto */
void __cdecl timer_wnd_destroy(HWND hwnd)

{
  if (hwnd != (HWND)0x0) {
    timeEndPeriod(5);
    DestroyWindow(hwnd);
    timerdll.timer_callback = 0;
    FreeLibrary(timerdll.module);
  }
  return;
}

/* 005d44be auto */
i32 __cdecl timer_wnd_start_slot(HWND hwnd,UINT interval_ms,iptr slot)

{
  MMRESULT mm_timer_id;
  i32 result;
  UINT_PTR timer_id;
  
  if (timer_mm_available == 0) {
    if (wnd_modal_stack_below_block.timer_alt_kind_request == 0) {
      timer_id = SetTimer(hwnd,slot,interval_ms,(TIMERPROC)0x0);
      if (timer_id == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(iptr)&str_err_timersetfailed_3,(iptr)&str_d_ss_smeds32_pctimer_cpp_5,0xc3);
        result = 0;
      }
      else {
        result = 1;
      }
    }
    else {
      mm_timer_id = timeSetEvent(interval_ms,5,timerdll.timer_callback,hwnd,1);
      if (mm_timer_id == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(iptr)&str_err_timersetfailed_4,(iptr)&str_d_ss_smeds32_pctimer_cpp_6,0xd7);
        result = 0;
      }
      else {
        timerdll.set_timer_id(slot,mm_timer_id);
        result = 1;
      }
    }
  }
  else if (wnd_modal_stack_below_block.timer_alt_kind_request == 0) {
    mm_timer_id = timeSetEvent(interval_ms,5,timerdll.timer_callback,hwnd,1);
    if (mm_timer_id == 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(iptr)&str_err_timersetfailed,(iptr)&str_d_ss_smeds32_pctimer_cpp_3,0xa8);
      result = 0;
    }
    else {
      timerdll.set_timer_id(slot,mm_timer_id);
      result = 1;
    }
  }
  else {
    timer_id = SetTimer(hwnd,slot,interval_ms,(TIMERPROC)0x0);
    if (timer_id == 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(iptr)&str_err_timersetfailed_2,(iptr)&str_d_ss_smeds32_pctimer_cpp_4,0xb4);
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
}

/* 005d4664 auto */
void __cdecl timer_wnd_kill_slot(HWND hwnd,iptr slot)

{
  UINT mm_timer_id;
  
  if (timer_mm_available == 0) {
    if (wnd_modal_stack_below_block.timer_alt_kind_request == 0) {
      KillTimer(hwnd,slot);
    }
    else {
      mm_timer_id = timerdll.get_timer_id(slot);
      timeKillEvent(mm_timer_id);
      timerdll.set_timer_id(slot,0);
    }
  }
  else if (wnd_modal_stack_below_block.timer_alt_kind_request == 0) {
    mm_timer_id = timerdll.get_timer_id(slot);
    timeKillEvent(mm_timer_id);
    timerdll.set_timer_id(slot,0);
  }
  else {
    KillTimer(hwnd,slot);
  }
  return;
}

/* 005d4700 auto */
LRESULT __stdcall timer_wnd_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)

{
  TimerManager *self;
  LRESULT result;
  
  self = (TimerManager *)GetWindowLongA(hwnd,0);
  if (self == (TimerManager *)0x0) {
    result = DefWindowProcA(hwnd,msg,wparam,lparam);
  }
  else {
    if (msg == 0x113) {
      self->game_timer_fire_slot(wparam);
    }
    else {
      if (msg != 0x52c) {
        result = DefWindowProcA(hwnd,msg,wparam,lparam);
        return result;
      }
      self->game_timer_fire_slot(wparam);
      timerdll.reset_timer_notified(wparam);
    }
    result = 0;
  }
  return result;
}

/* 005d47d0 auto */
void TimerManager::game_timer_fire_slot(iptr slot)

{
  if ((((TimerManagerExt *)this)->timer_callbacks[slot] != 0) && (((u32 *)&((TimerManagerExt *)this)->timer_repeat_counts)[slot] != 0)) {
    if (0 < (int)((u32 *)&((TimerManagerExt *)this)->timer_repeat_counts)[slot]) {
      ((u32 *)&((TimerManagerExt *)this)->timer_repeat_counts)[slot]--;
    }
    ((TimerManagerExt *)this)->timer_callbacks[slot](slot,((u32 *)&((TimerManagerExt *)this)->timer_repeat_counts)[slot]);
    if (((u32 *)&((TimerManagerExt *)this)->timer_repeat_counts)[slot] == 0) {
      game_timer_kill_slot(slot);
    }
  }
  return;
}

/* 005dbb20 auto */
i32 __cdecl app_startup(i32 hinstance,i32 prev_instance)

{
  (*(i32 *)&err_report_ctx_block.app_hinstance) = hinstance;
  err_report_ctx_block.app_prev_instance = prev_instance;
  app_init_graphics();
  app_init_system();
  return 1;
}

/* 005dbb4f auto */
i32 __cdecl app_shutdown(void)

{
  app_shutdown_system();
  dbg_log((const char *)&str_smeds_terminated_normally);
  if (wnd_modal_stack_below_block.game_timer_mgr != (TimerManager *)0x0) {
    if (wnd_modal_stack_below_block.game_timer_mgr != (TimerManager *)0x0) {
      wnd_modal_stack_below_block.game_timer_mgr->scalar_destruct(1);
    }
  }
  GdiFlush();
  return 1;
}

/* 005dbbb3 auto */
void __cdecl app_init_graphics(void)

{
  ((GameFont *)((i32 *)&default_draw_font_obj_block.default_draw_font_obj))->set(2,0xe);
  gfx_scale_init();
  return;
}

/* 005dbbd6 auto */
void __cdecl app_init_system(void)

{
  time_t now;
  
  if (err_report_ctx_block.app_prev_instance == 0) {
    app_register_wnd_classes();
  }
  snd_system_init();
  app_init_hook_nop();
  now = _time((i32 *)(time_t *)0x0);
  crt_srand((int)now);
  Ordinal_17();
  return;
}

/* 005dbc1b auto */
void __cdecl app_shutdown_system(void)

{
  wnd_stack_check_leaks_nop();
  app_shutdown_hook_nop();
  snd_system_shutdown();
  return;
}

/* 005dbc3a auto */
void __cdecl app_init_hook_nop(void)

{
  return;
}

/* 005dbc4a auto */
void __cdecl app_shutdown_hook_nop(void)

{
  return;
}

/* 005dbc5a auto */
void __cdecl app_register_wnd_classes(void)

{
  WNDCLASSA wnd_class;
  
  wnd_class.style = 0x28;
  wnd_class.lpfnWndProc = (WNDPROC)wnd_base_wndproc;
  wnd_class.cbClsExtra = 0;
  wnd_class.cbWndExtra = 0x14;
  wnd_class.hInstance = err_report_ctx_block.app_hinstance;
  wnd_class.hIcon = (HICON)0x0;
  wnd_class.hCursor = (HCURSOR)0x0;
  wnd_class.hbrBackground = (HBRUSH)0x0;
  wnd_class.lpszMenuName = (LPCSTR)0x0;
  wnd_class.lpszClassName = str_mswindowclass_3;
  RegisterClassA(&wnd_class);
  wnd_class.style = 0x28;
  wnd_class.lpfnWndProc = (WNDPROC)moviewnd_wndproc;
  wnd_class.cbClsExtra = 0;
  wnd_class.cbWndExtra = 0x14;
  wnd_class.hInstance = err_report_ctx_block.app_hinstance;
  wnd_class.hIcon = (HICON)0x0;
  wnd_class.hCursor = (HCURSOR)0x0;
  wnd_class.hbrBackground = (HBRUSH)0x0;
  wnd_class.lpszMenuName = (LPCSTR)0x0;
  wnd_class.lpszClassName = str_msmovieclass;
  RegisterClassA(&wnd_class);
  wnd_class.style = 0x88;
  wnd_class.lpfnWndProc = ctrl_default_wndproc;
  wnd_class.cbClsExtra = 0;
  wnd_class.cbWndExtra = 0x10;
  wnd_class.hInstance = err_report_ctx_block.app_hinstance;
  wnd_class.hIcon = (HICON)0x0;
  wnd_class.hCursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f00);
  wnd_class.hbrBackground = (HBRUSH)0x0;
  wnd_class.lpszMenuName = (LPCSTR)0x0;
  wnd_class.lpszClassName = str_mscontrolclass_7;
  RegisterClassA(&wnd_class);
  wnd_class.style = 0;
  wnd_class.lpfnWndProc = timer_wnd_proc;
  wnd_class.cbClsExtra = 0;
  wnd_class.cbWndExtra = 4;
  wnd_class.hInstance = err_report_ctx_block.app_hinstance;
  wnd_class.hIcon = (HICON)0x0;
  wnd_class.hCursor = (HCURSOR)0x0;
  wnd_class.hbrBackground = (HBRUSH)0x0;
  wnd_class.lpszMenuName = (LPCSTR)0x0;
  wnd_class.lpszClassName = str_msmrtimerclass_3;
  RegisterClassA(&wnd_class);
  return;
}

/* 005dbe88: written by hand in src/game/retvals.cpp */

/* 005dcac0 auto */
TimerManager *TimerManager::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}
