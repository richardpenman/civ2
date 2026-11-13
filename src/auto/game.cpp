/* The game, the game: start-up, turns, saving, scenarios, events, cheats: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0041325d auto */
void __cdecl eot_blink_timer_tick(void)

{
  if ((blink.unit_active != '\0') && (turn_loop.active != '\0')) {
    blink.end_turn_phase = blink.end_turn_phase == '\0';
    status_draw_end_of_turn(1);
  }
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

/* 0041eeeb auto */
int __cdecl game_main_menu_loop(void)

{
  int rc;
  int civ;
  int choice;
  int xd_major;
  GamePalette palette;
  int result;
  i32 map_source;
  int xd_minor;
  int retry;
  int xd_build;
  int lobby_failed;
  
  result = 1;
  retry = 1;
  lobby_failed = 0;
  Dialog dlg(0x4000);
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
        str_append_int(text_args.strings,xdaemon_required.major);
        crt_strcat(((i8 *)&text_args.strings),((i8 *)&xdaemon_req_ver_dot1_str));
        str_append_int(text_args.strings,xdaemon_required.minor);
        crt_strcat(((i8 *)&text_args.strings),((i8 *)&xdaemon_req_ver_dot2_str));
        str_append_int(text_args.strings,xdaemon_required.build);
        str_clear((char *)&text_args.string1);
        str_append_int(text_args.string1,xd_major);
        crt_strcat(((i8 *)&text_args.string1),((i8 *)&xdaemon_got_ver_dot1_str));
        str_append_int(text_args.string1,xd_minor);
        crt_strcat(((i8 *)&text_args.string1),((i8 *)&xdaemon_got_ver_dot2_str));
        str_append_int(text_args.string1,xd_build);
        popup_game_with_sprite(str_wrongxdaemonversion,(Sprite *)&popup_info_icon_sprite,0);
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
        tile_set_visible(x2,y2,game.human_players,1,1);
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
  
  Dialog dlg(0x4000);
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
      ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
      main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
      main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
      if (game.multiplayer != '\0') {
        if ((game.multiplayer < 3) || (is_host = net.is_host(), is_host != '\0')) {
          rules_apply_double_moves();
        }
        if ((2 < game.multiplayer) && (is_host = net.is_host(), is_host != '\0')) {
          net.mp_simultaneous_moves = *(i8 *)&mp_options.simultaneous_setting;
          net_send_message(NM_MULTIOPTIONS_CHANGE,0xff,mp_doubles.production,mp_doubles.moves,(*(i8 *)&mp_options.simultaneous_setting),0,0,0,0
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
        popup_game_with_sprite(str_simultut,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
        simul_tutorial_shown = 1;
      }
      ui_set_button_font(&dialog_plain_font_obj_block.dialog_plain_font_obj);
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
          log_file.write(chat.log_buffer,log_len);
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
  str_append_label(text_args.string1,(mp_options.open_game == 0) + 0x277);
  str_clear((char *)&text_args.string2);
  crt_strcat(((i8 *)&text_args.string2),((i8 *)&net.host_player_name));
  str_clear((char *)&text_args.string3);
  name_len = _strlen(((i8 *)&scenario.name));
  if (name_len == 0) {
    str_append_label(text_args.string3,0x34c);
  }
  else {
    crt_strcpy(((i8 *)&text_args.string3),((i8 *)&scenario.name));
  }
  str_append_space(text_args.string3);
  str_append_lparen(text_args.string3);
  result = game_turn_to_year((int)game.turn);
  str_append_year(text_args.string3,result);
  str_append_rparen(text_args.string3);
  str_clear((char *)&text_args.profile_difficulty);
  str_append_label((char *)&text_args.profile_difficulty,game.difficulty + 0x279);
  str_append_space((char *)&text_args.profile_difficulty);
  str_append_lparen((char *)&text_args.profile_difficulty);
  str_append_label((char *)&text_args.profile_difficulty,0x34e - (uint)(mp_options.simultaneous_setting == 0));
  str_append_rparen((char *)&text_args.profile_difficulty);
  str_clear((char *)&text_args.profile_map_width);
  str_append_int((char *)&text_args.profile_map_width,(int)map.width / 2);
  str_clear((char *)&text_args.profile_map_height);
  str_append_int((char *)&text_args.profile_map_height,(int)map.height);
  str_clear((char *)&text_args.string5);
  str_append_label(text_args.string5,0x350 - (uint)(mp_doubles.production == 0));
  crt_strcat(((i8 *)&text_args.string5),((i8 *)&game_profile_double_opts_sep_str));
  str_append_label(text_args.string5,0x352 - (uint)(mp_doubles.moves == 0));
  str_clear((char *)&text_args.profile_barbarians);
  str_append_label((char *)&text_args.profile_barbarians,game.barbarian_activity + 0x27f);
  str_clear((char *)&text_args.profile_timer);
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms == 0) {
    str_append_label(text_args.profile_timer,0x285);
  }
  else {
    minutes = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) / 0x3c;
    seconds = (civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000) % 0x3c;
    if (minutes < 10) {
      str_append_int(text_args.profile_timer,0);
    }
    str_append_int(text_args.profile_timer,minutes);
    crt_strcat(((i8 *)&text_args.profile_timer),((i8 *)&game_profile_timer_colon_str));
    if (seconds < 10) {
      str_append_int(text_args.profile_timer,0);
    }
    str_append_int(text_args.profile_timer,seconds);
  }
  str_clear((char *)&text_args.profile_count);
  count = 0;
  slot_masks[9] = (uint)game.civs_alive;
  for (i = 1; slot_masks[9] = (int)slot_masks[9] >> 1, i < 8; i++) {
    if ((slot_masks[9] & 1) != 0) {
      count++;
    }
  }
  str_append_int((char *)&text_args.profile_count,count);
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
  str_append_int((char *)&text_args.profile_count2,count);
  dlg_style.pending_idle_callback = turn_popup_timeout_idle;
  result = popup_text_with_sprite(str_game_7,str_gameprofile,(uint)(is_join != 0),(Sprite *)&popup_info_icon_sprite,0);
  return result;
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
  value_ptr = (int *)registry_read_value(key_path,str_defaultlanguage,(DWORD *)&value_size,(DWORD *)value_type);
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
  registry_write_value(key_path,str_defaultlanguage_2,(BYTE *)&language,4,4);
  registry_language_id = language;
  return;
}

/* 0045406c auto */
bool __cdecl registry_write_value(const char *key_path,const char *value_name,BYTE *data,DWORD data_size,DWORD value_type)

{
  LSTATUS status;
  bool ok;
  HKEY hkey;
  DWORD disposition;
  int rc;
  
  status = RegCreateKeyExA(0x80000002,key_path,0,((i8 *)&registry_key_class_str),0,0x20006,
                          (LPSECURITY_ATTRIBUTES)0x0,&hkey,&disposition);
  if (status == 0) {
    rc = RegSetValueExA(hkey,value_name,0,value_type,data,data_size);
    RegCloseKey(hkey);
    ok = rc == 0;
  }
  else {
    /* BF-301 (Peter Johnson's ledger): no key to close (the original closed
       the handle it had left unset) */
    ok = false;
  }
  return ok;
}

/* 00454103 auto */
i8 * __cdecl registry_read_value(const char *key_path,const char *value_name,DWORD *out_size,DWORD *out_type)

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
  status = RegOpenKeyExA(0x80000002,key_path,0,0x20019,&hkey);
  if (status == 0) {
    status = RegQueryValueExA(hkey,value_name,(LPDWORD)0x0,&value_type,((LPBYTE)(i8 *)&tax_rate_icon_sprites_block.registry_value_buffer),&data_size2);
    if (status == 0) {
      *out_type = value_type;
      *out_size = data_size2;
      RegCloseKey(hkey);
      result = ((i8 *)&tax_rate_icon_sprites_block.registry_value_buffer);
    }
    else {
      RegCloseKey(hkey);
      result = (i8 *)0x0;
    }
  }
  else {
    result = (i8 *)0x0;  /* BF-301: no key to close */
  }
  return result;
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
  /* BF-297 (Peter Johnson's ledger): the audio's flags start clear, as the
     original found them on its stack (AviAudio::construct sets only the
     stream; a heap player keeps the debug heap's fill, as in the original) */
  player.audio.flags = 0;
  ON_SCOPE_EXIT(player.destruct());
  host_wnd.ModalMenuWindow::construct();
  ON_SCOPE_EXIT(host_wnd.ModalMenuWindow::destruct());
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  dll.init();
  ON_SCOPE_EXIT(dll.free());
  gfx_get_screen_rect(&screen_rect2);
  intro.video_player = &player;
  str_clear((char *)path);
  str_append((char *)path,(const char *)&str_civ2_civ2art_dll);
  found = file_resolve_path((const char *)path);
  if (found != 0) {
    dll.load((const char *)path);
    str_clear((char *)path);
    str_append((char *)path,(const char *)&str_civ2_video_opening_avi);
    found = file_resolve_path((const char *)path);
    if (found != 0) {
      ((WndBase *)&host_wnd)->create_with_cmd_proc((const char *)&str_empty_82,0x800,0,0,screen_rect2.right,screen_rect2.bottom + 5,(GamePalette *)&game_palette_block.game_palette);
      ((WndBase *)&host_wnd)->set_bg_color(0);
      intro.video_player = &player;
      player.create((const char*)&str_empty_83,0x800,0,0);
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
        intro.video_player = 0;
        dll.dll_handle_free();
      }
      else {
        if (open_rc == -0x7ffbfeac) {
          popup_game((const char *)&str_vfwnotregistered);
        }
        intro.video_player = 0;
      }
    }
  }
  return;
}

/* 0046dde0 auto */
void __cdecl intro_video_on_end(void)

{
  if (intro.video_player != 0) {
    intro.video_player->window.end_modal_loop();
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
      intro.video_player->window.end_modal_loop();
    }
  }
  return;
}

/* 0046dea1 auto */
void __cdecl intro_video_on_click(void)

{
  if (intro.video_player != 0) {
    intro.video_player->window.end_modal_loop();
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
     (confirmed = popup_game_with_sprite(str_reallyquit_2,(Sprite *)&popup_info_icon_sprite,0), confirmed != 0)) {
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
  int alt_type;
  int attempts;
  iptr unit_type;
  uint y2;
  uint x2;
  int main_type;
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
      unit = tile_landmass(cities[city].x,cities[city].y);
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
      main_type = UNIT_CHARIOT;
      alt_type = UNIT_HORSEMEN;
      unit = crt_rand_at("game_spawn_barbarians");
      if (unit % 3 == 0) {
        int_swap(&main_type,&alt_type);
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
        if ((game.rules & RULE_BARBARIAN_FULL_ROSTER) != 0) {
          main_type = UNIT_ARCHERS;
          alt_type = UNIT_ELEPHANT;
        }
      }
      if (game.advance_known_by[ADV_INDUSTRIALIZATION] != '\0') {
        label = label_string(label_table.string_ids[0x65]);
        text_set_string_arg(0,label);
        main_type = UNIT_CANNON;
        alt_type = UNIT_MUSKETEERS;
        if (game.advance_known_by[unit_types[UNIT_ARMOR].prerequisite] != '\0') {
          label = label_string(label_table.string_ids[0x66]);
          text_set_string_arg(0,label);
          alt_type = UNIT_RIFLEMEN;
          if (((game.rules & RULE_BARBARIAN_FULL_ROSTER) != 0) && (main_type = UNIT_ARTILLERY, game.advance_known_by[ADV_GUERRILLA_WARFARE] != '\0'))
          {
            if ((civs[game.best_human_rank].government < GOV_REPUBLIC) ||
               (cities[city].owner != cities[city].founder)) {
              main_type = UNIT_CAVALRY;
              alt_type = UNIT_PARTISANS;
            }
            else {
              alt_type = UNIT_FANATICS;
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
          unit_type = UNIT_DIPLOMAT;
        }
        unit2 = unit_create(unit_type,0,x2,y2);
        if (-1 < unit2) {
          if (unit_type == UNIT_FANATICS) {
            units[unit2].flags = units[unit2].flags | UNITF_VETERAN;
          }
          unit = (int)tile_at(x2,y2);
          units[unit2].visibility = ((Tile *)unit)->visible | units[unit2].visibility;
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
                (unit = tile_visible_to(x2,y2,cities[city].owner),
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
  unit_type2 = UNIT_TRIREME;
  if (game.advance_known_by[ADV_NAVIGATION] != '\0') {
    unit_type2 = UNIT_CARAVEL;
  }
  if (game.advance_known_by[ADV_MAGNETISM] != '\0') {
    unit_type2 = UNIT_FRIGATE;
  }
  escort_type = UNIT_WARRIORS;
  if ((game.rules & RULE_BARBARIAN_FULL_ROSTER) != 0) {
    escort_type = UNIT_ARCHERS;
  }
  if ((game.human_players & game.advance_known_by[ADV_IRON_WORKING]) != 0) {
    escort_type = UNIT_LEGION;
  }
  if ((game.advance_known_by[ADV_METALLURGY] != '\0') ||
     ((game.barbarian_activity == 3 && (game.advance_known_by[ADV_CHIVALRY] != '\0')))) {
    if (((x2 & 1) == 0) || ((game.rules & RULE_BARBARIAN_FULL_ROSTER) == 0)) {
      escort_type = UNIT_KNIGHTS;
    }
    else {
      escort_type = UNIT_CRUSADERS;
    }
  }
  remaining = horde_count;
  if (((game.human_players & game.advance_known_by[ADV_TACTICS]) != 0) && ((game.rules & RULE_BARBARIAN_FULL_ROSTER) != 0)) {
    escort_type = UNIT_DRAGOONS;
  }
  while( true ) {
    remaining--;
    if ((int)remaining < 0) break;
    unit_type = escort_type;
    if ((remaining != 0) && (horde_count - 1 == remaining)) {
      unit_type = UNIT_DIPLOMAT;
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
  clamped = int_clamp(game.warming_counter,0,99);
  game.warming_counter = (i8)clamped;
  if (((game.warming_counter == 12) && (6 < pollution)) && (autoplay.active == 0)) {
    if (2 < game.multiplayer) {
      net_send_text_args(4,0xff,0,0,0,0);
    }
    popup_game_with_sprite(str_fearwarming,(Sprite *)&popup_info_icon_sprite,0);
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
void __cdecl game_advance_turn(int civ)

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
            value = int_clamp(civs[civ3].reputation_credit[civ2],0,
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
        net_send_message(NM_END_SEQUENCE,0xff,game.game_flags,2,turn_flags.spaceship_winner_civ,0,0,0,0,0);
      }
      else {
        net_send_message(NM_END_SEQUENCE,net.net_slots[net.net_civ_slots[civ2]].id,
                           game.game_flags,1,turn_flags.spaceship_winner_civ,0,0,0,0,0);
        for (other_civ = 1; other_civ < 8; other_civ++) {
          if ((((1 << ((byte)other_civ & 0x1f) &
                (uint)(game.civs_alive & game.human_players) & 1 << ((byte)other_civ & 0x1f)) != 0)
              && (human_civ[0] != other_civ)) && (other_civ != civ2)) {
            net_send_text_args(6,net.net_slots[net.net_civ_slots[other_civ]].id,1,
                               0,0,0);
            net_send_message(NM_END_SEQUENCE,net.net_slots[net.net_civ_slots[other_civ]].id
                               ,game.game_flags,2,turn_flags.spaceship_winner_civ,0,0,0,0,0);
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
    popup_game_with_sprite(str_eaglehaslanded,(Sprite *)&popup_info_icon_sprite,0);
  }
  game.game_flags = game.game_flags | GAME_WON;
  return;
}

/* 00488937 auto */
void __cdecl game_autosave(int civ)

{
  char *leader_name;
  i8 backup_path[0x200];  /* BF-046 (Peter Johnson's ledger): the original's
                             80, less than its label */
  
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
void __cdecl tutorial_settler_hints(int unit)

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
      tutorial_settler_hints(game.active_unit);
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
  while (((result = unit_is_selectable(game.active_unit), result != 0 &&
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
      unit_is_selectable(game.active_unit);
      if (((game.active_unit < 0) || (game.num_units <= game.active_unit)) ||
         ((units[game.active_unit].id == 0 ||
          ((selectable = unit_is_selectable(game.active_unit), selectable == 0 ||
           (game.num_units != last_unit_count)))))) {
        unit_activate_next(0);
      }
      else if (map_views_block.unit_focus_done == '\0') {
        unit = (int)game.active_unit;
        map_cursor.x[0] = units[unit].x;
        map_cursor.y[0] = units[unit].y;
        if ((unit_move.unanimated_state == '\0') || (units[unit].orders != ORDER_GOTO)) {
          status_redraw(1);
          mapviews_ensure_visible((int)units[unit].x,(int)units[unit].y,units[unit].owner);
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
        popup_game_with_sprite(str_planretire,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      }
      else if (extra_rule(X_RETIREMENT) ? extras_year_reached(extras_retirement_year()) :
               (game.year == 0x7e4)) {
        if (2 < game.multiplayer) {
          net_send_text_args(10,0xff,0,0,0,0);
        }
        popup_game_with_sprite(str_doretire,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
        game.game_flags = game.game_flags | GAME_WON;
        game_over_reason = 5;
        if (2 < game.multiplayer) {
          net_send_message(NM_END_SEQUENCE,0xff,game.game_flags,5,0,0,0,0,0,0);
        }
        game_ended = 1;
      }
    }
    else if (scenario.max_turns - 5 == (int)game.turn) {
      if (2 < game.multiplayer) {
        net_send_text_args(7,0xff,0,0,0,0);
      }
      popup_game_with_sprite(str_scenarioends,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    else if (scenario.max_turns == game.turn) {
      if (2 < game.multiplayer) {
        net_send_text_args(8,0xff,0,0,0,0);
      }
      popup_game_with_sprite(str_scenarioend,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      game.game_flags = game.game_flags | GAME_WON;
      game_over_reason = 5;
      if (2 < game.multiplayer) {
        net_send_message(NM_END_SEQUENCE,0xff,game.game_flags,5,0,0,0,0,0,0);
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
        choice = popup_game_with_sprite(str_keepplaying,(Sprite *)&popup_info_icon_sprite,0);
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
      dialog2->add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
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
    popup_game_with_sprite(str_hotseatturn,(Sprite *)&popup_info_icon_sprite,0);
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
  int human_civ_saved;  /* B34: 0 never, 1 in an earlier civ's turn, 2 in this one */
  int views_hidden;
  int human_count;
  int quit;
  
  views_hidden = 0;
  pbem_saved = false;
  saved_human_civ = 0;
  human_civ_saved = 0;
  Dialog dialog2(0x4000);
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
      popup_game_with_sprite(str_hothumansdead,(Sprite *)&popup_info_icon_sprite,0);
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
        if (human_civ_saved == 2) {
          human_civ_saved = 1;
        }
        if ((game.multiplayer == 1) &&
           ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
          game.human_players = '\0';
          saved_human_civ = (byte)(*(i8 *)&human_civ);
          (*(i8 *)&human_civ) = -1;
          human_civ_saved = 2;
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
            /* B34: a hot-seat player whose civ was destroyed in its own turn
               (its last city starved) keeps the player: the original took
               the player saved at an earlier AI civ's turn (or its stack) */
            if (bugfix_state() ? (human_civ_saved == 2) : (human_civ_saved != 0)) {
              (*(i8 *)&human_civ) = (iptr)saved_human_civ;
            }
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
  i8 still_playing;
  int found_unit;
  int casualties;
  i32 first_turn;
  
  first_turn = 1;
  Dialog dialog2(0x4000);
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
    popup_game_sprite_flags(str_clienthotwait,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
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
          dialog2.add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
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
          dialog2.add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
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
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&rect2);
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
    popup_game_with_sprite(str_newplayer,(Sprite *)&popup_info_icon_sprite,0);
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
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&rect2);
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
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&rect2);
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
      dialog2->add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
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
      dialog2->add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
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
  int found_unit;
  
  Dialog dialog2(0x4000);
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
        unit_create(UNIT_SETTLERS,civ,units[unit].x,units[unit].y);
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
                    (result = tile_city_owner(units[unit].x,units[unit].y),
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
            unit_create(UNIT_ARCHERS,civ,cities[city].x,cities[city].y);
            unit_create(UNIT_LEGION,civ,cities[city].x,cities[city].y);
            unit_create(UNIT_CHARIOT,civ,cities[city].x,cities[city].y);
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

/* 004c4280 auto */
void __cdecl game_main_entry(void)

{
  game_main();
  return;
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
      confirmed = popup_game_with_sprite(str_reallyquit_3,(Sprite *)&popup_info_icon_sprite,0);
      if (confirmed == 0) {
        return;
      }
    }
    else {
      confirmed = popup_game_with_sprite(str_reallyretire,(Sprite *)&popup_info_icon_sprite,0);
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
  main_frame_wnd_block.main_frame_wnd.create_with_cmd_proc((const char *)&textbuf.buf,0x7c,0,0,600,400,(GamePalette *)&game_palette_block.game_palette);
  cd_ok = file_require_cdrom((const char *)&str_civ2_civ2_exe_2);
  if (cd_ok != 0) {
    wave_bits = 8;
    wave_rate = 0x10000;
    wnd_info = main_frame_wnd_block.main_frame_wnd.get_window();
    snd_wave_open((i32)wnd_info->hwnd,wave_rate,wave_bits);
    menu_build_main();
    ((DibSurface *)((i8 *)&main_frame_bg_surface_block.main_frame_bg_surface))->load_gif_resource(0x5a,10,0xc0,&palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface(&main_frame_bg_surface_block.main_frame_bg_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    main_frame_wnd_block.main_frame_wnd.set_icon(1);
    main_frame_bg_surface_block.main_frame_nonclient_width = main_frame_wnd_block.main_frame_wnd.frame_width();
    main_frame_bg_surface_block.main_frame_nonclient_height_i32 = main_frame_wnd_block.main_frame_wnd.nonclient_height();
    main_frame_wnd_block.main_frame_wnd.maximize();
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->get_rgb((u8 *)saved_rgb2,10,0xec);
    intro_play_opening_video();
    ((GamePalette *)((i8 *)&game_palette_block.game_palette))->set_rgb(10,0xec,(u8 *)saved_rgb2);
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
void __cdecl game_timer_destroy(int slot)

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
int TimerManager::add(TimerFn callback,i32 interval_ms,u32 repeat_count)

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
void TimerManager::game_timer_kill_slot(int slot)

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
  hwnd = timer_wnd_create(this);
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
HWND __cdecl timer_wnd_create(TimerManager *timer_mgr)

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
      SetWindowLongA(hwnd2,0,(LONG_PTR)timer_mgr);
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
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(9,(const char*)&str_err_dynamiclinkfailed,(const char*)&str_d_ss_smeds32_pctimer_cpp,0x55);
      FreeLibrary(timerdll.module);
    }
    else {
      hwnd2 = CreateWindowExA(0,str_msmrtimerclass,str_mrtimer,0xcf0000,-0x80000000,
                                -0x80000000,-0x80000000,-0x80000000,(HWND)0x0,(HMENU)0x0,
                                err_report_ctx_block.app_hinstance,(LPVOID)0x0);
      if (hwnd2 == (HWND)0x0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(10,(const char*)&str_err_cantcreatewindow,(const char*)&str_d_ss_smeds32_pctimer_cpp_2,0x71);
        FreeLibrary(timerdll.module);
      }
      else {
        timeBeginPeriod(5);
        SetWindowLongA(hwnd2,0,(LONG_PTR)timer_mgr);
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
i32 __cdecl timer_wnd_start_slot(HWND hwnd,UINT interval_ms,int slot)

{
  MMRESULT mm_timer_id;
  i32 result;
  UINT_PTR timer_id;
  
  if (timer_mm_available == 0) {
    if (wnd_modal_stack_below_block.timer_alt_kind_request == 0) {
      timer_id = SetTimer(hwnd,slot,interval_ms,(TIMERPROC)0x0);
      if (timer_id == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(const char*)&str_err_timersetfailed_3,(const char*)&str_d_ss_smeds32_pctimer_cpp_5,0xc3);
        result = 0;
      }
      else {
        result = 1;
      }
    }
    else {
      mm_timer_id = timeSetEvent(interval_ms,5,timerdll.timer_callback,hwnd,1);
      if (mm_timer_id == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(const char*)&str_err_timersetfailed_4,(const char*)&str_d_ss_smeds32_pctimer_cpp_6,0xd7);
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
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(const char*)&str_err_timersetfailed,(const char*)&str_d_ss_smeds32_pctimer_cpp_3,0xa8);
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
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(0xb,(const char*)&str_err_timersetfailed_2,(const char*)&str_d_ss_smeds32_pctimer_cpp_4,0xb4);
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
}

/* 005d4664 auto */
void __cdecl timer_wnd_kill_slot(HWND hwnd,int slot)

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
void TimerManager::game_timer_fire_slot(int slot)

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
