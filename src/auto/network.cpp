/* The game, multiplayer: network sessions, negotiation, lobby and chat: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00421f40 auto */
i8 __thiscall net_is_host(NetSession *self)

{
  return self->net_host_flag;
}

/* 00421f70 auto */
NetGamelistView * __thiscall net_gamelist_view_ctor(NetGamelistView *self)

{
  self->next_game_id = 1;
  self->first_option_value = 0;
  self->refresh_timer = 0;
  return self;
}

/* 00421fad auto */
void __thiscall net_gamelist_view_dtor(NetGamelistView *self)

{
  net_free_game_list(&net_session_block);
  return;
}

/* (net_session_init) After a scenario's intro: the set-up backdrop back. */
static void net_join_restore_backdrop(GamePalette *palette)
{
  if (net_session_block.loaded_file_kind == 2) {
    snd_play_effect(0x6a,0,0,0);
    surface_load_gif_resource((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface),0x5a,10,0xc0,(iptr)palette);
    wnd_set_bg_surface(&main_frame_wnd_block.main_frame_wnd,(DibSurface *)&minimap_window_block.setup_backdrop_surface);
    wnd_set_bg_color(&main_frame_wnd_block.main_frame_wnd,0x9e);
    wnd_flush_paint();
  }
}

/* 00421fcd (by hand: the decompiler lost the lobby retry count and the stack dialog) */
/* Joining a network game: the game list (LAN) or the host's address
   (Internet; conn_type 0), the connection, the host's answers, the scenario,
   the player's profile and tribe, then the wait for the game.  1: joined. */
i32 __thiscall net_session_init(void *self_,int conn_type)

{
  NetGamelistView *self = (NetGamelistView *)self_;
  int result;
  int popup;
  int retries;
  NetGameInfo *found;
  int civs_count;
  int players_count;
  uint mask;
  int bit;
  size_t len1;
  size_t len2;
  int civ;
  int gov;
  int sex;
  int label;
  iptr unit;
  i32 year;
  char *tribe_label;
  Dialog *list_dialog;
  Dialog scen_dlg;
  GamePalette scen_palette;
  i8 scen_file[260];
  char subdir_path[260];
  char address[260];
  GamePalette palette;
  Dialog dlg;

  dialog_ctor(&dlg,0x4000);
  palette_init(&palette);
  retries = 0;
  net_reset_msg_counts();
  if ((conn_type == 2) || (conn_type == 3)) {
    /* (modem, serial: no address) */
    self->server_address[0] = '\0';
    result = XD_OpenConnection(self->server_address,net_session_block.net_timeout_seconds * 1000);
    if (result < 0) {
      if ((net_session_block.net_protocol != 2) && (net_session_block.net_protocol != 3)) {
        dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
        crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
        popup_game_with_sprite("SERVERCONNECTTIME",(iptr)&popup_info_icon_sprite,0);
      }
      goto fail;
    }
    goto connected;
  }
  if (conn_type != 0) goto game_list;
  /* Internet: the host's address typed (or the lobby's) */
ask_address:
  retries++;
  if (net_session_block.net_lobby_launch_mode != -1) {
    crt_strcpy(self->server_address,((i8 *)&net_session_block.net_lobby_launch_info));
    crt_strcpy(((i8 *)&net_session_block.net_server_address),((i8 *)&net_session_block.net_lobby_launch_info));
  }
  else {
    dialog_load_text_section(&dlg,(const char *)&str_game_7,"IPOFGAME",0x1f,(iptr)"",0,0,0,1);
    result = dialog_run_modal(&dlg,(iptr)address,0);
    if (result == -1) {
      self->server_address[0] = '\0';
      goto fail;
    }
    if (_strlen(address) == 0) goto ask_address;
    crt_strcpy(self->server_address,address);
    crt_strcpy(((i8 *)&net_session_block.net_server_address),address);
  }
  result = XD_OpenConnection(self->server_address,net_session_block.net_timeout_seconds * 1000);
  if (result < 0) {
    dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
    popup_game_with_sprite("SERVERCONNECTTIME",(iptr)&popup_info_icon_sprite,0);
    if ((conn_type == 2) || (conn_type == 3)) goto fail;
    goto ask_address;
  }
  if ((conn_type == 0) && (net_session_block.net_lobby_launch_mode != -1)) {
    dbg_log("IGZ - Sending DPLSYS_CONNECTIONSETTINGSREAD to lobby");
    result = XD_LobbySendMessage(1);
    if (result != 0) {
      dbg_logf("IGZ - XD_LobbySendMessage returned error - %d",result);
    }
    dbg_log("IGZ - Sending DPLSYS_DPLAYCONNECTSUCCEEDED to lobby");
    result = XD_LobbySendMessage(3);
    if (result != 0) {
      dbg_logf("IGZ - XD_LobbySendMessage returned error - %d",result);
    }
  }
  net_session_block.net_wait_start_ticks_i32 = timer_ticks();
  dlg_pending_idle_callback = net_join_wait_idle_cb;
  crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
  popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) goto fail;
  if (net_session_block.net_server_conn_id == -1) {
    /* no id from the host: the lobby's game is tried again (3 times) */
    if ((conn_type == 0) && (net_session_block.net_lobby_launch_mode != -1) && (retries <= 3)) {
      XD_CloseConnection();
      goto ask_address;
    }
    popup_game_with_sprite("SERVERCONNECTFAIL",(iptr)&popup_info_icon_sprite,0);
    goto fail;
  }
  /* the host's version */
  diffeng_register_blocks();
  net_reset_msg_counts();
  net_send_message(0x12,0,0,0,0,0,0,0,0,0);
  dlg_pending_idle_callback = net_join_wait_answer_idle;
  crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
  popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    goto fail;
  }
  if ((net_session_block.nm_server_reject_count != 0) || (net_session_block.nm_server_canceled_count != 0)) {
    text_set_string_arg(1,self->server_game_name);
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
    if (net_session_block.nm_server_reject_count == 0) {
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("REJECTEDLAN",(iptr)&popup_info_icon_sprite,0);
    }
    goto fail;
  }
  len1 = _strlen(((i8 *)&net_session_block.net_server_version));
  len2 = _strlen(str_ptr_5_4_0f_multiplayer_26_march_99);
  if ((len1 != len2) || (_strcmp(((i8 *)&net_session_block.net_server_version),str_ptr_5_4_0f_multiplayer_26_march_99) != 0)) {
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),((i8 *)&net_session_block.net_server_version));
    crt_strcpy(((i8 *)&minimap_window_block.text_string_arg1),str_ptr_5_4_0f_multiplayer_26_march_99);
    popup_game_with_sprite("WRONGVERSION",(iptr)&popup_info_icon_sprite,0);
    goto fail;
  }
  goto connected;

game_list:
  /* LAN: the games the hosts announce, in a list */
  net_free_game_list(&net_session_block);
  list_dialog = (Dialog *)operator_new(0x2f4);
  if (list_dialog != (Dialog *)0x0) {
    list_dialog = dialog_ctor(list_dialog,0x4000);
  }
  self->list_dialog = list_dialog;
  if (self->list_dialog == 0) {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)"pJoinPopup != NULL",(iptr)"D:\\Ss\\Franklinton\\startup joinbox.cpp",0xdc);
  }
  self->saved_button_label = dialog_button_labels[0];
  str_clear(self->button_label);
  str_append_label((iptr)self->button_label,0x126);
  dialog_button_labels[0] = (i8 *)self->button_label;
  dlg_reset(self->list_dialog,0x1081801);
  dialog_set_width(self->list_dialog,0x276);
  dialog_set_height(self->list_dialog,0x153);
  text_clear();
  text_append_label(0x287);
  dialog_set_title(self->list_dialog,(const char *)&map_views_block.text_buffer);
  dialog_set_current_pane(self->list_dialog,0);
  dialog_set_listbox(self->list_dialog,0xb,0xd2,0);
  dialog_set_select_change_fn(self->list_dialog,(DialogSelectFn)net_gamelist_show_game_info);
  net_gamelist_add_info_lines(self);
  text_clear();
  text_append_label(0x286);
  net_gamelist_add_entry(self,(const char *)&map_views_block.text_buffer,0);
  dialog_set_default_choice(self->list_dialog,0);
  net_gamelist_update_details(self);
  dialog_render(self->list_dialog);
  wnd_set_modal_idle_callback((ModalMenuWindow *)(self->list_dialog->surf_wnd + 0x48),net_gamelist_poll_idle);
  for (;;) {
    net_session_block.net_listing_games = 1;
    net_gamelist_start_timer(self,(TimerFn)net_gamelist_poll_tick);
    result = dlg_show_modal(self->list_dialog,0);
    net_session_block.net_listing_games = 0;
    net_gamelist_kill_timer(self);
    if (result < 0) {
      dialog_button_labels[0] = self->saved_button_label;
      net_gamelist_clear(self);
      if (self->list_dialog != 0) {
        dialog_scalar_dtor(self->list_dialog,1);
      }
      self->list_dialog = 0;
      self->server_address[0] = '\0';
      goto fail;
    }
    for (found = net_session_block.net_found_games; (found != 0 && (found->list_id != result)); found = found->next) {
    }
    if (found == 0) continue;
    civs_count = 0;
    mask = (uint)found->civs_alive;
    for (bit = 0; bit < 8; bit++) {
      if ((mask & 1) != 0) {
        civs_count++;
      }
      mask = (int)mask >> 1;
    }
    players_count = 0;
    mask = (uint)found->player_mask;
    for (bit = 0; bit < 8; bit++) {
      if ((mask & 1) != 0) {
        players_count++;
      }
      mask = (int)mask >> 1;
    }
    if ((players_count >= net_session_block.net_max_players) || (players_count >= civs_count) || (found->open_game == 0)) {
      if ((net_session_block.net_max_players <= players_count) || (civs_count <= players_count)) {
        popup_game_with_sprite("JOINEDMAX",(iptr)&popup_info_icon_sprite,0);
      }
      continue;
    }
    len1 = _strlen(found->version);
    len2 = _strlen(str_ptr_5_4_0f_multiplayer_26_march_99);
    if ((len1 != len2) || (_strcmp(found->version,str_ptr_5_4_0f_multiplayer_26_march_99) != 0)) {
      dialog_button_labels[0] = self->saved_button_label;
      crt_strcpy(((i8 *)&minimap_window_block.text_string_args),found->version);
      crt_strcpy(((i8 *)&minimap_window_block.text_string_arg1),str_ptr_5_4_0f_multiplayer_26_march_99);
      popup_game_with_sprite("WRONGVERSION",(iptr)&popup_info_icon_sprite,0);
      dialog_button_labels[0] = (i8 *)self->button_label;
      continue;
    }
    if ((conn_type == 2) || (conn_type == 3)) {
      self->server_address[0] = '\0';
    }
    else {
      crt_strcpy(self->server_address,found->host_address);
    }
    crt_strcpy(self->server_game_name,found->game_name);
    crt_strcpy(self->server_info_c8,found->host_name);
    map.width = found->map_width;
    map.height = found->map_height;
    if ((conn_type == 2) || (conn_type == 3)) {
      net_session_block.net_server_address[0] = 0;
    }
    else {
      crt_strcpy(((i8 *)&net_session_block.net_server_address),found->host_address);
    }
    crt_strcpy(((i8 *)&net_session_block.net_game_name),found->game_name);
    crt_strcpy(((i8 *)&net_session_block.net_local_player_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
    crt_strcpy(((i8 *)&net_session_block.net_host_player_name),found->host_name);
    result = XD_OpenConnection(self->server_address,net_session_block.net_timeout_seconds * 1000);
    if (-1 < result) break;
    dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
    popup_game_with_sprite("SERVERCONNECTFAIL",(iptr)&popup_info_icon_sprite,0);
  }
  dialog_close(self->list_dialog);
  dialog_button_labels[0] = self->saved_button_label;
  net_gamelist_clear(self);
  if (self->list_dialog != 0) {
    dialog_scalar_dtor(self->list_dialog,1);
  }
  self->list_dialog = 0;
  wnd_flush_paint();

connected:
  /* the host's id */
  dlg_pending_idle_callback = net_join_wait_server_idle;
  net_session_block.net_wait_start_ticks_i32 = timer_ticks();
  if (conn_type != 0) {
    net_reset_msg_counts();
  }
  crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
  popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    goto fail;
  }
  if ((net_session_block.nm_server_reject_count != 0) || (net_session_block.nm_server_canceled_count != 0) ||
      (net_session_block.nm_server_full_count != 0)) {
    text_set_string_arg(1,self->server_game_name);
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
    if (net_session_block.nm_server_reject_count != 0) {
      popup_game_with_sprite("REJECTEDLAN",(iptr)&popup_info_icon_sprite,0);
    }
    else if (net_session_block.nm_server_canceled_count != 0) {
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("JOINEDMAX",(iptr)&popup_info_icon_sprite,0);
    }
    goto fail;
  }
  if (popup == -2) {
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
    crt_strcpy(((i8 *)&minimap_window_block.text_string_arg1),((i8 *)&net_session_block.net_game_name));
    popup_game_with_sprite("SERVERCONNECTTIME",(iptr)&popup_info_icon_sprite,0);
    goto fail;
  }
  /* the game's state */
  diffeng_register_blocks();
  dlg_pending_idle_callback = net_join_wait_gamestate_idle;
  net_session_block.net_wait_start_ticks_i32 = timer_ticks();
  if (conn_type != 0) {
    net_reset_msg_counts();
  }
  crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
  popup = popup_game_sprite_flags("WAITTOJOIN",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if ((popup == -1) || (net_session_block.nm_server_canceled_count != 0)) {
    if (popup == -1) {
      net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    }
    else {
      text_set_string_arg(1,self->server_game_name);
      crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    XD_FlushSendBuffer(180000);
    goto fail;
  }
  /* a scenario: its directory, its intro */
  if ((0 < net_session_block.loaded_file_kind) && (_strlen(((i8 *)&net_session_block.loaded_file_subdir)) != 0)) {
    crt_strcpy(subdir_path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(subdir_path,((i8 *)&net_session_block.loaded_file_subdir));
    if (__chdir(subdir_path) < 0) {
      net_send_message(0x32,0,0,0,0,0,0,0,0,0);
      XD_FlushSendBuffer(180000);
      text_set_string_arg(0,((i8 *)&civ_saved_cursor_x_block.game_home_dir));
      text_set_string_arg(1,((i8 *)&net_session_block.loaded_file_subdir));
      popup_game_with_sprite("DATAPATHFAIL",(iptr)&popup_info_icon_sprite,0);
      goto fail;
    }
    crt_strcat(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&net_session_block.loaded_file_subdir));
    rules_load(0);
    if ((net_session_block.loaded_file_kind == 2) || ((game.game_flags & 0x80) != 0)) {
      dialog_ctor(&scen_dlg,0x4000);
      palette_init(&scen_palette);
      for (civ = 1; civ < 8; civ++) {
        if ((civs[civ].flags & 0x200) == 0) {
          leaders[civs[civ].tribe].female = '\0';
        }
        else {
          leaders[civs[civ].tribe].female = '\x01';
        }
        civs[civ].leader_portrait = (u8)civs[civ].tribe;
        if (leaders[civs[civ].tribe].female != '\0') {
          civs[civ].leader_portrait = civs[civ].leader_portrait + '\x15';
        }
        if (leaders[civs[civ].tribe].plural < 1) {
          label = ~(int)leaders[civs[civ].tribe].plural + 1;
        }
        else {
          label = (int)leaders[civs[civ].tribe].plural;
        }
        tribe_label = (char *)label_string(label);
        if (_strcmp(civ_names[civ].tribe_name,tribe_label) == 0) {
          for (gov = 0; gov < 7; gov++) {
            for (sex = 0; sex < 2; sex++) {
              leaders[civs[civ].tribe].titles[gov][sex] = (short)(&government_titles[gov].male)[sex];
            }
          }
        }
        if (0 < leaders[civs[civ].tribe].adjective) {
          leaders[civs[civ].tribe].leader_name = -leaders[civs[civ].tribe].leader_name;
        }
        if (0 < leaders[civs[civ].tribe].adjective) {
          leaders[civs[civ].tribe].plural = -leaders[civs[civ].tribe].plural;
        }
        if (0 < leaders[civs[civ].tribe].adjective) {
          leaders[civs[civ].tribe].adjective = -leaders[civs[civ].tribe].adjective;
        }
      }
      if (cd_music_available == 0) {
        snd_cd_stop();
        snd_play_effect(0x6b,0,1,0);
      }
      ui_flush_input();
      __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
      if ((file_exists("TITLE.GIF") != 0) &&
         (surface_load_gif_file((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface),"TITLE.GIF",10,0xc0,(iptr)&scen_palette) != 0)) {
        wnd_set_bg_surface(&main_frame_wnd_block.main_frame_wnd,(DibSurface *)&minimap_window_block.setup_backdrop_surface);
        wnd_set_bg_color(&main_frame_wnd_block.main_frame_wnd,0x9e);
        wnd_maximize(&main_frame_wnd_block.main_frame_wnd);
        wnd_flush_paint();
        input_flush_events();
      }
      __chdir(((i8 *)&civ_saved_cursor_x_block.game_home_dir));
      crt_strcpy(((i8 *)&map_views_block.text_buffer),((i8 *)&net_map_lock_slots_block.scenario_dir_path));
      crt_strcat(((i8 *)&map_views_block.text_buffer),"\\");
      crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&net_session_block.loaded_file_name));
      path_set_extension(scen_file,(char *)(&map_views_block.text_buffer),lang_file_ext);
      if (cd_music_available != 0) {
        snd_play_effect(0x6a,0,0,0);
      }
      game.game_options = prefs_game_rules_block.prefs_game_options & 0xffff7fff;
      game.message_options = prefs_game_rules_block.prefs_message_options;
      text_set_string_arg(0,(const char *)&prefs_game_rules_block.scenario_name);
      text_set_number_arg(0,(int)prefs_game_rules_block.scenario_tech_paradigm);
      text_clear();
      year = game_turn_to_year((int)game.turn);
      text_append_year(year);
      text_set_string_arg(1,(const char *)&map_views_block.text_buffer);
      text_clear();
      if (prefs_game_rules_block.scenario_max_turns == 0) {
        text_append_year(2000);
      }
      else {
        year = game_turn_to_year((int)prefs_game_rules_block.scenario_max_turns);
        text_append_year(year);
      }
      text_set_string_arg(2,(const char *)&map_views_block.text_buffer);
      if (cd_music_available != 0) {
        snd_menu_end();
      }
      dlg_pending_idle_callback = turn_popup_timeout_idle;
      dialog_title_font_obj_block.dialog_timeout_start_i32 = timer_ticks();
      dialog_init_default(&scen_dlg);
      if (dialog_load_text(&scen_dlg,(const char *)scen_file,"SCENARIO",0x4008001) == 0) {
        unit = (iptr)game.active_unit;
        if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
           ((int)units[unit].owner == (int)prefs_game_rules_block.scenario_objective_civ)) {
          dialog_add_sprite(&scen_dlg,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c),0,0);
        }
        if (dlg_show_modal(&scen_dlg,0) < 0) {
          snd_play_effect(0x6a,0,0,0);
          surface_load_gif_resource((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface),0x5a,10,0xc0,(iptr)&scen_palette);
          wnd_set_bg_surface(&main_frame_wnd_block.main_frame_wnd,(DibSurface *)&minimap_window_block.setup_backdrop_surface);
          wnd_set_bg_color(&main_frame_wnd_block.main_frame_wnd,0x9e);
          wnd_flush_paint();
          goto scenario_cancelled;
        }
      }
      else if (popup_game_flags("SCENARIOLOADED",1) < 0) {
        goto scenario_cancelled;
      }
      palette_destroy(&scen_palette);
      dialog_dtor(&scen_dlg);
    }
  }
  goto profile;
scenario_cancelled:
  net_send_message(0x32,0,0,0,0,0,0,0,0,0);
  net_send_message(0x31,0,0,human_civ[0],0,0,0,0,0,0);
  XD_FlushSendBuffer(180000);
  palette_destroy(&scen_palette);
  dialog_dtor(&scen_dlg);
  goto fail;

profile:
  /* the player: profile, tribe */
  passwords_refresh_flags();
  if (game_profile_popup(1) == -1) {
    net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    goto fail;
  }
  if (((civ_saved_cursor_x_block.saved_resume_new_game == 0) && (civ_saved_cursor_x_block.saved_turn_civ == '\0')) &&
     (net_session_block.net_session_phase < 2)) {
    result = setup_choose_tribe(net_session_block.net_local_slot,3,0);
  }
  else {
    result = net_choose_civ_dialog(net_session_block.net_local_slot);
  }
  if (result != 0) {
    net_join_restore_backdrop(&palette);
    net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    goto fail;
  }
  /* in: wait for the player list, then the game */
  net_send_message(0x2e,0,0,0,0,0,0,0,0,0);
  diffeng_register_blocks();
  dlg_pending_idle_callback = net_join_wait_player_list_idle;
  net_session_block.net_wait_start_ticks_i32 = timer_ticks();
  net_reset_msg_counts();
  crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
  popup = popup_game_sprite_flags("WAITINGONJOIN",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_join_restore_backdrop(&palette);
    net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    net_send_message(0x31,0,0,human_civ[0],0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    goto fail;
  }
  if ((net_session_block.nm_server_canceled_count != 0) || (net_session_block.nm_lost_server_on_join_count != 0)) {
    net_join_restore_backdrop(&palette);
    text_set_string_arg(1,self->server_game_name);
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
    if (net_session_block.nm_server_canceled_count == 0) {
      popup_game_with_sprite("LOSTSERVER",(iptr)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    }
    goto fail;
  }
  net_session_block.nm_start_civ_count = 0;
  dlg_pending_idle_callback = net_join_wait_start_civ_idle;
  net_session_block.net_wait_start_ticks_i32 = timer_ticks();
  crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
  popup = popup_game_sprite_flags("WAITINGONJOIN",0x2000001,(iptr)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_join_restore_backdrop(&palette);
    net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    net_send_message(0x31,0,0,human_civ[0],0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    goto fail;
  }
  if (net_session_block.nm_server_canceled_count != 0) {
    net_join_restore_backdrop(&palette);
    text_set_string_arg(1,self->server_game_name);
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(*(char * *)&net_session_block.net_connection_title));
    popup_game_with_sprite("SERVERQUIT",(iptr)&popup_info_icon_sprite,0);
    goto fail;
  }
  net_reset_msg_counts();
  text_close_file();
  snd_menu_end();
  palette_destroy(&palette);
  dialog_dtor(&dlg);
  return 1;
fail:
  palette_destroy(&palette);
  dialog_dtor(&dlg);
  return 0;
}

/* 0042415c auto */
void __cdecl net_gamelist_poll_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  return;
}

/* 0042417a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_server_idle(void)

{
  timer_ticks();
  net_process_messages(&net_session_block,1,0);
  if ((((net_session_block.nm_player_list_count != 0) || (net_session_block.nm_server_canceled_count != 0)) || (net_session_block.nm_server_reject_count != 0)) || (net_session_block.nm_server_full_count != 0))
  {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 004241f8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_gamestate_idle(void)

{
  int elapsed;
  size_t text_len;
  char *marker;
  char *text;
  
  elapsed = timer_ticks();
  elapsed -= net_session_block.net_wait_start_ticks_i32;
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.nm_join_gamestate_count == 0) && (net_session_block.nm_server_canceled_count == 0)) {
    if (2 < elapsed) {
      text = *(char **)((int)(*(Dialog * *)&net_session_block.dialog_stack_top)->line_list->next + 8);
      text_len = _strlen(text);
      marker = _strchr(text,0x3e);
      if (text + (text_len - 1) == marker) {
        *marker = '<';
      }
      else {
        *marker = '-';
        marker[1] = '>';
      }
      marker = _strchr(text,0x3c);
      if (marker == (char *)0x0) {
        text = _strchr(text,0x3e);
        text[-1] = '<';
      }
      else if (marker == text) {
        *marker = '>';
      }
      else {
        *marker = '-';
        marker[-1] = '<';
      }
      dialog_render((*(Dialog * *)&net_session_block.dialog_stack_top));
      net_session_block.net_wait_start_ticks_i32 = timer_ticks();
    }
  }
  else {
    (*(Dialog * *)&net_session_block.dialog_stack_top)->style_flags = (*(Dialog * *)&net_session_block.dialog_stack_top)->style_flags | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)((*(Dialog * *)&net_session_block.dialog_stack_top)->surf_wnd + 0x48));
  }
  return;
}

/* 0042433c auto */
void __cdecl net_join_wait_player_list_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  if (((net_session_block.nm_player_list_count != 0) || (net_session_block.nm_server_canceled_count != 0)) || (net_session_block.nm_lost_server_on_join_count != 0)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 0042439c auto */
void __cdecl net_join_wait_start_civ_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.nm_start_civ_count != 0) || (net_session_block.nm_server_canceled_count != 0)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 004243ef auto */
void __cdecl net_join_wait_answer_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  if (((net_session_block.nm_server_answer_count != 0) || (net_session_block.nm_server_canceled_count != 0)) || (net_session_block.nm_server_reject_count != 0)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 0042444f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_idle_cb(void)

{
  int elapsed;
  
  elapsed = timer_ticks();
  elapsed -= net_session_block.net_wait_start_ticks_i32;
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.net_server_conn_id == -1) && (0x4d8 < elapsed)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  else if (net_session_block.net_server_conn_id != -1) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 004244e0 auto */
void __thiscall net_gamelist_restore_selection(NetGamelistView *self)

{
  int option_count;
  DialogOption *option;
  int top_index;
  int selected_index;
  iptr scroll_pos;
  
  option_count = dialog_count_group_options(self->list_dialog,0);
  scrollbar_set_range(*(Scrollbar **)((iptr)self->list_dialog + 0x284),0,option_count - 1);
  option = net_gamelist_find_option(self,(iptr)&self->selected_game_name,(iptr)&self->selected_host_name);
  self->list_dialog->selected_option = option;
  if (option_count < 0xb) {
    self->list_dialog->list_top_option[0] = self->first_option_value;
  }
  else {
    option = net_gamelist_find_option(self,(iptr)&self->top_game_name,(iptr)&self->top_host_name);
    self->list_dialog->list_top_option[0] = option;
  }
  if (*(int *)((iptr)self->list_dialog + 0x1b8) != 0) {
    top_index = dialog_option_group_index
                      (self->list_dialog,self->list_dialog->list_top_option[0]);
    option_count = (int)self->list_dialog->list_rows[0];
    selected_index = dialog_option_group_index
                      (self->list_dialog,self->list_dialog->selected_option);
    if (option_count + top_index <= selected_index) {
      option_count = dialog_option_group_index
                        (self->list_dialog,self->list_dialog->selected_option);
      option = (dialog_option_at
                        (self->list_dialog,
                         (option_count - (int)self->list_dialog->list_rows[0]) + 1));
      self->list_dialog->list_top_option[0] = option;
    }
    option_count = dialog_option_group_index
                      (self->list_dialog,self->list_dialog->list_top_option[0]);
    top_index = dialog_option_group_index
                      (self->list_dialog,self->list_dialog->selected_option);
    if (top_index < option_count) {
      self->list_dialog->list_top_option[0] = self->list_dialog->selected_option;
    }
  }
  scroll_pos = dialog_option_group_index
                    (self->list_dialog,self->list_dialog->list_top_option[0]);
  scrollbar_set_pos(*(Scrollbar **)((iptr)self->list_dialog + 0x284),scroll_pos);
  return;
}

/* 00424688 auto */
DialogOption * __thiscall net_gamelist_find_option(NetGamelistView *self,iptr game_name,iptr host_name)

{
  int cmp;
  DialogOption *option;
  NetGameInfo *game2;
  
  option = self->first_option_value;
  game2 = net_session_block.net_found_games;
  while( true ) {
    if (game2 == 0) {
      return self->first_option_value;
    }
    cmp = _strcmp(game2->game_name,(char *)game_name);
    if ((cmp == 0) && (cmp = _strcmp(game2->host_name,(char *)host_name), cmp == 0))
    break;
    option = option->next;
    game2 = game2->next;
  }
  return option;
}

/* 0042472a auto */
void __thiscall net_gamelist_update_details(NetGamelistView *self)

{
  if (net_session_block.net_found_games == 0) {
    self->selected_game_name[0] = '\0';
    self->selected_host_name[0] = '\0';
    self->top_game_name[0] = '\0';
    self->top_host_name[0] = '\0';
  }
  else {
    net_gamelist_get_entry_names(self,(iptr)self->list_dialog->selected_option,(iptr)&self->selected_game_name,
                       (iptr)&self->selected_host_name);
    net_gamelist_get_entry_names(self,(iptr)self->list_dialog->list_top_option[0],(iptr)&self->top_game_name,
                       (iptr)&self->top_host_name);
  }
  return;
}

/* 004247b2 auto */
void __thiscall net_gamelist_get_entry_names(NetGamelistView *self,iptr option,iptr out_game_name,iptr out_host_name)

{
  int remaining;
  NetGameInfo *game2;
  
  for (game2 = net_session_block.net_found_games;
      (game2 != 0 && (*(int *)(option + 4) != game2->list_id));
      game2 = game2->next) {
  }
  if (game2 == 0) {
    game2 = net_session_block.net_found_games;
    for (remaining = dialog_option_group_index(self->list_dialog,(DialogOption *)option);
        (game2->next != 0 && (remaining != 0)); remaining--) {
      game2 = game2->next;
    }
  }
  crt_strcpy((char *)((void *)out_game_name),game2->game_name);
  crt_strcpy((char *)((void *)out_host_name),game2->host_name);
  return;
}

/* 0042486f auto */
void __thiscall net_gamelist_add_info_lines(NetGamelistView *self)

{
  int line_y;
  DialogLine *line;
  int spacing;
  
  dialog_compute_layout(self->list_dialog);
  line_y = (int)self->list_dialog->title_height + (int)self->list_dialog->field_b8 + 2;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_14,0xe6,line_y,0x80,0));
  self->info_lines[0] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line_y += spacing;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_15,0xe6,line_y,0x80,0));
  self->info_lines[1] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line_y += spacing;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_16,0xe6,line_y,0x80,0));
  self->info_lines[2] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line_y += spacing;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_17,0xe6,line_y,0x80,0));
  self->info_lines[3] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line_y += spacing;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_18,0xe6,line_y,0x80,0));
  self->info_lines[4] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line_y += spacing;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_19,0xe6,line_y,0x80,0));
  self->info_lines[5] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line_y += spacing;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_20,0xe6,line_y,0x80,0));
  self->info_lines[6] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line_y += spacing;
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_21,0xe6,line_y,0x80,0));
  self->info_lines[7] = (u32)line->number;
  spacing = font_line_spacing((GameFont *)self->list_dialog->body_font);
  line = (DialogLine *)((u32 *)dialog_add_line
                            (self->list_dialog,(const char *)&str_empty_22,0xe6,line_y + spacing,0x80,0));
  self->info_lines[8] = (u32)line->number;
  font_line_spacing((GameFont *)self->list_dialog->body_font);
  return;
}

/* 00424ae9 auto */
i32 __cdecl net_gamelist_show_game_info(void *dialog,i32 unused)

{
  size_t text_len;
  i32 year;
  int seconds;
  int minutes;
  iptr scroll_pos;
  uint civ_bits;
  int civ_count;
  int bit;
  char text[256];
  NetGameInfo *found_game;
  
  found_game = net_session_block.net_found_games;
  if (net_session_block.net_found_games == 0) {
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[0],(const char *)&str_empty_23);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[1],(const char *)&str_empty_24);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[2],(const char *)&str_empty_25);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[3],(const char *)&str_empty_26);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[4],(const char *)&str_empty_27);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[5],(const char *)&str_empty_28);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[6],(const char *)&str_empty_29);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[7],(const char *)&str_empty_30);
    dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[8],(const char *)&str_empty_31);
  }
  else {
    for (; (found_game != 0 &&
           (found_game->list_id != *(int *)((int)net_session_block.gamelist_view->list_dialog->selected_option + 4)));
        found_game = found_game->next) {
    }
    if (found_game != 0) {
      str_clear((char *)text);
      str_append_label((iptr)text,0x26f);
      str_append_space((iptr)text);
      crt_strcat(text,found_game->game_name);
      str_append_space((iptr)text);
      str_append_lparen((iptr)text);
      str_append_label((iptr)text,(found_game->open_game == 0) + 0x277);
      str_append_rparen((iptr)text);
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[0],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x270);
      str_append_space((iptr)text);
      crt_strcat(text,found_game->host_name);
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[1],(const char *)text);
      str_clear((char *)text);
      text_len = _strlen(found_game->scenario_name);
      if (text_len == 0) {
        str_append_label((iptr)text,0x34c);
      }
      else {
        crt_strcpy(text,found_game->scenario_name);
      }
      text_len = _strlen(text);
      if (0x19 < text_len) {
        text[21] = 0;           /* a long scenario name cut */
      }
      str_append_space((iptr)text);
      str_append_lparen((iptr)text);
      game.game_flags = found_game->game_flags;
      game.first_launch_turn = found_game->first_launch_turn;
      prefs_game_rules_block.scenario_year_increment[0] = found_game->year_increment;
      prefs_game_rules_block.scenario_start_year = found_game->start_year;
      game.difficulty = found_game->difficulty;
      year = game_turn_to_year((int)found_game->turn);
      str_append_year((iptr)text,year);
      str_append_rparen((iptr)text);
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[2],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,found_game->difficulty + 0x279);
      str_append_space((iptr)text);
      str_append_lparen((iptr)text);
      str_append_label((iptr)text,0x34e - (uint)(found_game->simultaneous == 0));
      str_append_rparen((iptr)text);
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[3],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x36c);
      str_append_space((iptr)text);
      str_append_int((iptr)text,(int)found_game->map_width / 2);
      crt_strcat(text,((i8 *)&gamelist_map_size_sep_str));
      str_append_int((iptr)text,(int)found_game->map_height);
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[4],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x350 - (uint)(found_game->double_production == 0));
      crt_strcat(text,((i8 *)&gamelist_double_opts_sep_str));
      str_append_label((iptr)text,0x352 - (uint)(found_game->double_moves == 0));
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[5],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x273);
      str_append_space((iptr)text);
      str_append_label((iptr)text,found_game->barbarian_activity + 0x27f);
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[6],(const char *)text);
      str_clear((char *)text);
      str_append_label((iptr)text,0x276);
      str_append_space((iptr)text);
      if (found_game->turn_time_limit_ms == 0) {
        str_append_label((iptr)text,0x285);
      }
      else {
        seconds = found_game->turn_time_limit_ms / 1000;
        minutes = seconds / 0x3c;
        seconds = seconds % 0x3c;
        if (minutes < 10) {
          str_append_int((iptr)text,0);
        }
        str_append_int((iptr)text,minutes);
        crt_strcat(text,((i8 *)&gamelist_timer_colon_str));
        if (seconds < 10) {
          str_append_int((iptr)text,0);
        }
        str_append_int((iptr)text,seconds);
      }
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[7],(const char *)text);
      bit = 1;
      civ_count = 0;
      civ_bits = (uint)found_game->civs_alive;
      for (; civ_bits = (int)civ_bits >> 1, bit < 8; bit++) {
        if ((civ_bits & 1) != 0) {
          civ_count++;
        }
      }
      str_clear((char *)text);
      str_append_label((iptr)text,0x274);
      str_append_space((iptr)text);
      str_append_int((iptr)text,civ_count);
      crt_strcat(text,((i8 *)&gamelist_open_paren_str));
      str_append_label((iptr)text,0x275);
      str_append_space((iptr)text);
      bit = 0;
      civ_count = 0;
      civ_bits = (uint)found_game->player_mask;
      for (; bit < 8; bit++) {
        if ((civ_bits & 1) != 0) {
          civ_count++;
        }
        civ_bits = (int)civ_bits >> 1;
      }
      if ((found_game->session_phase < 3) || (civ_count == 0)) {
        str_append_label((iptr)text,0x353);
      }
      else {
        str_append_int((iptr)text,civ_count);
      }
      str_append_rparen((iptr)text);
      dialog_set_item_text((Dialog *)dialog,net_session_block.gamelist_view->info_lines[8],(const char *)text);
    }
  }
  scroll_pos = dialog_option_group_index
                    (net_session_block.gamelist_view->list_dialog,net_session_block.gamelist_view->list_dialog->list_top_option[0]);
  scrollbar_set_pos(*(Scrollbar **)((int)net_session_block.gamelist_view->list_dialog + 0x284),scroll_pos);
  dialog_render(net_session_block.gamelist_view->list_dialog);
  return 1;
}

/* 004253ef auto */
void __thiscall net_gamelist_clear(NetGamelistView *self)

{
  void *next;
  void *option;
  
  option = (void *)self->first_option_value;
  net_gamelist_update_details(self);
  while (option != (void *)0x0) {
    next = *(void **)((int)option + 0x10);
    operator_delete(*(void **)((int)option + 8));
    operator_delete(option);
    option = next;
  }
  self->first_option_value = 0;
  self->list_dialog->option_count = 0;
  self->list_dialog->option_list = 0;
  self->list_dialog->selected_option = 0;
  return;
}

/* 004254a8 auto */
DialogOption * __thiscall net_gamelist_add_entry(NetGamelistView *self,const char *name2,uint id)

{
  int *count_ptr;
  DialogOption *entry;
  size_t name_len;
  void *name_copy;
  int entry_width;
  int max_width;
  DialogOption *last;
  DialogOption *option;
  
  last = 0;
  for (option = self->first_option_value; option != 0; option = option->next) {
    last = option;
  }
  entry = (DialogOption *)(operator_new(0x18));
  entry->next = 0;
  entry->prev = last;
  if (last == 0) {
    self->first_option_value = entry;
    self->list_dialog->option_list = entry;
  }
  else {
    last->next = entry;
  }
  entry->flags = 0;
  name_len = _strlen((char *)name2);
  name_copy = operator_new(name_len + 1);
  entry->text = (char *)name_copy;
  crt_strcpy((char *)((void *)entry->text),(const char *)((const void *)name2));
  if (*(char *)name2 == '\0') {
    entry->flags = entry->flags | 1;
  }
  entry->id = id;
  entry->group = 0;
  entry_width = font_text_width((GameFont *)self->list_dialog->body_font,(const char *)entry->text);
  max_width = dialog_option_icon_width(self->list_dialog);
  entry_width = entry_width + max_width + (int)self->list_dialog->option_extra_width + 5;
  max_width = (int)self->list_dialog->option_width;
  if (max_width <= entry_width) {
    max_width = entry_width;
  }
  self->list_dialog->option_width = max_width;
  count_ptr = (int *)((iptr)self->list_dialog + 0x28);
  *count_ptr = *count_ptr + 1;
  return entry;
}

/* 00425607 auto */
void __thiscall net_gamelist_start_timer(NetGamelistView *self,TimerFn callback)

{
  u32 timer;
  
  if (self->refresh_timer != 0) {
    net_gamelist_kill_timer(self);
  }
  timer = game_timer_create(callback,0x32,1);
  self->refresh_timer = timer;
  return;
}

/* 00425650 auto */
void __thiscall net_gamelist_kill_timer(NetGamelistView *self)

{
  if (self->refresh_timer != 0) {
    game_timer_destroy(self->refresh_timer);
    self->refresh_timer = 0;
  }
  return;
}

/* 00425695 auto */
void __cdecl net_gamelist_poll_tick(void)

{
  i32 timer;
  
  net_send_message(1,0,0,0,0,0,0,0,0,0);
  net_gamelist_expire_stale();
  if (net_session_block.gamelist_view->refresh_timer != 0) {
    game_timer_destroy((i32)net_session_block.gamelist_view->refresh_timer);
  }
  timer = game_timer_create((TimerFn)net_gamelist_poll_tick,0xfa,1);
  net_session_block.gamelist_view->refresh_timer = timer;
  return;
}

/* 0042570c auto */
void __cdecl net_gamelist_expire_stale(void)

{
  bool removed;
  int now;
  NetGameInfo *next;
  NetGameInfo *game2;
  
  removed = false;
  game2 = net_session_block.net_found_games;
  now = timer_ticks();
  while (game2 != 0) {
    if (net_session_block.net_timeout_seconds * 0x3c < now - game2->last_seen) {
      if (net_session_block.net_found_games == game2) {
        net_session_block.net_found_games = game2->next;
      }
      if (game2->prev != 0) {
        game2->prev->next = game2->next;
      }
      if (game2->next != 0) {
        game2->next->prev = game2->prev;
      }
      next = game2->next;
      operator_delete(game2);
      removed = true;
      game2 = next;
    }
    else {
      game2 = game2->next;
    }
  }
  if (removed) {
    net_gamelist_refresh();
  }
  return;
}

/* 004257fe auto */
void __cdecl net_gamelist_refresh(void)

{
  DialogOption *entry;
  i32 civ_bits;
  i32 bit;
  i32 total_civs;
  NetGameInfo *game2;
  i32 human_count;
  
  if (net_session_block.gamelist_view != 0) {
    net_gamelist_clear(net_session_block.gamelist_view);
    if (net_session_block.net_found_games == 0) {
      text_clear();
      text_append_label(0x286);
      net_gamelist_add_entry(net_session_block.gamelist_view,(const char *)&map_views_block.text_buffer,0);
      net_gamelist_update_details(net_session_block.gamelist_view);
    }
    else {
      for (game2 = net_session_block.net_found_games; game2 != 0; game2 = game2->next) {
        if (game2->list_id == 0) {
          game2->list_id = net_session_block.gamelist_view->next_game_id;
          net_session_block.gamelist_view->next_game_id = net_session_block.gamelist_view->next_game_id + 1;
        }
        entry = (net_gamelist_add_entry
                                   (net_session_block.gamelist_view,game2->game_name,
                                    game2->list_id));
        bit = 0;
        total_civs = 0;
        civ_bits = (uint)game2->civs_alive;
        for (; bit < 8; bit++) {
          if ((civ_bits & 1) != 0) {
            total_civs++;
          }
          civ_bits = civ_bits >> 1;
        }
        bit = 0;
        human_count = 0;
        civ_bits = (uint)game2->player_mask;
        for (; bit < 8; bit++) {
          if ((civ_bits & 1) != 0) {
            human_count++;
          }
          civ_bits = civ_bits >> 1;
        }
        if (((net_session_block.net_max_players <= human_count) || (total_civs <= human_count)) ||
           (game2->open_game == 0)) {
          entry->flags = entry->flags | 1;
        }
      }
    }
    net_gamelist_restore_selection(net_session_block.gamelist_view);
    net_gamelist_show_game_info((void *)net_session_block.gamelist_view->list_dialog,0);
  }
  return;
}

/* 00430822 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_meeting_wait_idle(void)

{
  int now;
  
  net_process_messages(&net_session_block,1,0);
  now = timer_ticks();
  if (((net_session_block.net_timeout_seconds * 0x3c <= now - civ_rating_title_font_block.mp_meeting_wait_start_i32) || (advisor_window_block.mp_meeting_reply != -1)) ||
     (net_session_block.nm_parley_cancel_count != 0)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
    if (advisor_window_block.mp_meeting_reply == -1) {
      advisor_window_block.mp_meeting_reply = 0;
    }
  }
  return;
}

/* 0044528e auto */
i32 __cdecl net_prompt_ip_address(void)

{
  char *proto_addr;
  size_t input_len;
  i32 result;
  char input[260];
  Dialog dlg;
  int choice;
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  eh_state = 0;
  if ((net_session_block.net_lobby_launch_mode == -1) || (net_session_block.net_lobby_launch_mode == 1)) {
    proto_addr = XD_GetCurrentProtoAddr(net_session_block.net_adapter_index,net_session_block.net_protocol);
    if (proto_addr == (char *)0x0) {
      do {
        dialog_load_text_section(&dlg,(const char *)&str_game_7,(const char *)&str_ipaddress,0x1f,(iptr)&prefs_main_menu_choice_block.prefs_net_ip_address,0,0,0,1);
        choice = dialog_run_modal(&dlg,(iptr)input,0);
        if (choice == -1) {
          eh_state = 0xffffffff;
          /* inlined unwind funclet 00445407 */ { dialog_dtor(&dlg); }
          result = 0;
          goto return_result;
        }
        input_len = _strlen(input);
      } while (input_len == 0);
      crt_strcpy(((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),input);
    }
    else {
      proto_addr = XD_GetCurrentProtoAddr(net_session_block.net_adapter_index,net_session_block.net_protocol);
      crt_strcpy(((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),proto_addr);
    }
  }
  else {
    crt_strcpy(((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),((i8 *)&net_session_block.net_lobby_launch_info));
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 00445407 */ { dialog_dtor(&dlg); }
  result = 1;
return_result:
  return result;
}

/* 0044542c auto */
i32 __cdecl net_prompt_player_name(void)

{
  size_t input_len;
  i32 result;
  char input[260];
  Dialog dlg;
  int choice;
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  eh_state = 0;
  if (net_session_block.net_lobby_launch_mode == -1) {
    do {
      dialog_load_text_section(&dlg,(const char *)&str_game_7,(const char *)&str_netname,0x1f,(iptr)&prefs_main_menu_choice_block.prefs_net_player_name,0,0,0,1);
      choice = dialog_run_modal(&dlg,(iptr)input,0);
      if (choice == -1) {
        eh_state = 0xffffffff;
        /* inlined unwind funclet 0044556d */ { dialog_dtor(&dlg); }
        result = 0;
        goto return_result;
      }
      input_len = _strlen(input);
    } while (input_len == 0);
    crt_strcpy(((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name),input);
    prefs_save_civ2dat();
  }
  else {
    _memset(((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name),0,0x20);
    _strncpy(((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name),((i8 *)&net_session_block.net_lobby_short_name),0x1f);
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0044556d */ { dialog_dtor(&dlg); }
  result = 1;
return_result:
  return result;
}

/* 00445592 auto */
i32 __cdecl net_prompt_game_name(void)

{
  size_t input_len;
  i32 result;
  char input[260];
  Dialog dlg;
  int choice;
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  eh_state = 0;
  if (net_session_block.net_lobby_launch_mode == -1) {
    do {
      dialog_load_text_section(&dlg,(const char *)&str_game_7,(const char *)&str_gamename,0x1f,(iptr)&prefs_main_menu_choice_block.prefs_net_game_name,0,0,0,1);
      choice = dialog_run_modal(&dlg,(iptr)input,0);
      if (choice == -1) {
        eh_state = 0xffffffff;
        /* inlined unwind funclet 004456ed */ { dialog_dtor(&dlg); }
        result = 0;
        goto return_result;
      }
      input_len = _strlen(input);
    } while (input_len == 0);
    crt_strcpy(((i8 *)&prefs_main_menu_choice_block.prefs_net_game_name),input);
    crt_strcpy(((i8 *)&net_session_block.net_game_name),input);
    prefs_save_civ2dat();
  }
  else {
    _memset(((i8 *)&prefs_main_menu_choice_block.prefs_net_game_name),0,0x20);
    _strncpy(((i8 *)&prefs_main_menu_choice_block.prefs_net_game_name),((i8 *)&net_session_block.net_lobby_session_name),0x1f);
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004456ed */ { dialog_dtor(&dlg); }
  result = 1;
return_result:
  return result;
}

/* 00445e46 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_choose_civ_dialog(int title_offset)

{
  iptr civ2;
  iptr choice;
  int result;
  iptr leader_str;
  i32 ret;
  i32 arg6;
  i32 arg7;
  i32 arg8;
  iptr civ;
  u8 is_female;
  i8 input[260];
  int index;
  int available_count;
  int option_count2;
  iptr selected2;
  Dialog dlg;
  DialogOption *option;
  iptr option_civs[8];
  int wait_start;
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  eh_state = 0;
  selected2 = 0;
  net_session_block.net_player_info_has_names_i32 = 0;
  net_session_block.net_player_info_has_titles_i32 = 0;
  do {
    dlg_reset(&dlg,0x801);
    dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
    index = 0;
    for (civ = 1; civ < 8; civ++) {
      if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
        text_clear();
        civ2 = (iptr)(civ_name_plural(civ));
        text_append((const char *)civ2);
        text_append_space();
        text_append_lparen();
        civ2 = (iptr)(civ_leader_name(civ));
        text_append((const char *)civ2);
        text_append_rparen();
        text_append_space();
        text_append_lparen();
        text_append_int((int)civs[civ].num_cities);
        text_append_space();
        text_append_label(0xc5);
        text_append((const char *)&str_empty_65);
        text_append_int((int)civs[civ].num_military_units);
        text_append_space();
        text_append_label(0xc4);
        text_append((const char *)&str_empty_66);
        result = index;
        index++;
        option = (DialogOption *)((byte *)dialog_add_option
                                     (&dlg,(const char *)&map_views_block.text_buffer,result,0));
        option_civs[option->id] = civ;
      }
    }
    option_count2 = index;
choose_loop:
    do {
      available_count = 0;
      for (index = 0; index < option_count2; index++) {
        option = (DialogOption *)((byte *)dialog_find_option
                                     (&dlg,index));
        if (option != (DialogOption *)0x0) {
          (*((char *)&civ + 0)) = (byte)option_civs[option->id];
          if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
             ((1 << ((byte)civ & 0x1f) & (int)civ_saved_cursor_x_block.net_claimed_civs_mask) == 0)) {
            available_count++;
            dialog_set_option_disabled
                      (&dlg,option->id,0);
            if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
              radiogrp_enable_item
                        ((RadioGroup *)dlg.radio_group,option->id);
            }
          }
          else {
            dialog_set_option_disabled
                      (&dlg,option->id,1);
            if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
              radiogrp_disable_item
                        ((RadioGroup *)dlg.radio_group,option->id);
            }
          }
        }
      }
      if (available_count == 0) {
        popup_game((const char *)&str_joinedmax);
        dialog_close(&dlg);
        eh_state = 0xffffffff;
        /* inlined unwind funclet 004467fa */ { dialog_dtor(&dlg); }
        ret = 1;
        goto return_ret;
      }
      for (index = 0; index < option_count2; index++) {
        option = (DialogOption *)((byte *)dialog_find_option
                                     (&dlg,selected2));
        if ((option != (DialogOption *)0x0) && ((option->flags_byte & 1) == 0)) {
          dialog_set_default_choice(&dlg,option->id);
          if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
            radiogrp_set_selected
                      ((RadioGroup *)dlg.radio_group,option->id);
          }
          break;
        }
        selected2++;
        if (selected2 == option_count2) {
          selected2 = 0;
        }
      }
      net_reset_msg_counts();
      net_session_block.net_wait_start_ticks_i32 = timer_ticks();
      dlg.style_flags = dlg.style_flags & 0xfffffbff;
      dlg.result_id = 0;
      dlg.result_extra = 0;
      dlg_pending_idle_callback = net_lock_wait_idle_cb;
      choice = dlg_show_modal(&dlg,0);
      selected2 = radiogrp_get_selected((RadioGroup *)dlg.radio_group);
      if (choice < 0) {
        eh_state = 0xffffffff;
        /* inlined unwind funclet 004467fa */ { dialog_dtor(&dlg); }
        ret = 1;
        goto return_ret;
      }
    } while (net_session_block.nm_lock_data_count != 0);
    if (net_session_block.nm_server_canceled_count != 0) {
      popup_game_with_sprite((const char *)&str_gamecanceled,(iptr)&popup_info_icon_sprite,0);
      eh_state = 0xffffffff;
      /* inlined unwind funclet 004467fa */ { dialog_dtor(&dlg); }
      ret = 1;
      goto return_ret;
    }
    civ2 = option_civs[choice];
    net_session_block.net_player_info_civ_i32 = civ2;
    selected2 = choice;
    net_send_message(0x30,0,0,civ2,0,0,0,0,0,0);
    if (net_session_block.net_host_flag == '\0') {
      net_reset_msg_counts();
      wait_start = timer_ticks();
      while (((net_session_block.nm_nation_accept_count == 0 && (net_session_block.nm_nation_reject_count == 0)) &&
             (result = timer_ticks(), result - wait_start < net_session_block.net_timeout_seconds * 0x3c))) {
        net_process_messages(&net_session_block,1,0);
      }
    }
    if ((net_session_block.nm_nation_accept_count == 0) && (net_session_block.nm_nation_reject_count == 0)) {
      dialog_close(&dlg);
      popup_game_with_sprite((const char *)&str_serverconnectfail,(iptr)&popup_info_icon_sprite,0);
      eh_state = 0xffffffff;
      /* inlined unwind funclet 004467fa */ { dialog_dtor(&dlg); }
      ret = 1;
      goto return_ret;
    }
    if (net_session_block.nm_nation_reject_count != 0) {
      popup_game_with_sprite((const char *)&str_alreadychosen,(iptr)&popup_info_icon_sprite,0);
      goto choose_loop;
    }
    dialog_close(&dlg);
    (*((char *)&civ + 0)) = (byte)civ2;
    game.human_players = game.human_players | (byte)(1 << ((byte)civ & 0x1f));
    game.player_civ = (byte)civ;
    game.turn_civ = (byte)civ;
    game.game_options = game.game_options & 0xfffffeff;
    human_civ[0] = civ2;
    result = civ_check_password(civ2);
    if (result == 0) {
      game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
      net_send_message(0x31,0,0,civ2,0,0,0,0,0,0);
    }
    else {
      dlg_load_game_section(&dlg,(const char *)&str_gender_4,1);
      dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
      dialog_set_default_choice
                (&dlg,(uint)leaders[civs[game.player_civ].tribe].female);
      dialog_set_position
                (&dlg,0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      dlg_pending_idle_callback = net_pump_idle_cb;
      result = dlg_show_modal(&dlg,0);
      net_session_block.net_player_info_gender_i32 = result;
      snd_play_effect(0x6a,0,0,0);
      if (result < 0) {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
        net_send_message(0x31,0,0,civ2,0,0,0,0,0,0);
      }
      else {
        is_female = (u8)result;
        leaders[civs[game.player_civ].tribe].female = is_female;
        civs[human_civ[0]].flags = civs[human_civ[0]].flags & 0xfdff;
        if (result != 0) {
          civs[human_civ[0]].flags = civs[human_civ[0]].flags | 0x200;
        }
        arg8 = 1;
        arg7 = 0;
        arg6 = 0;
        ret = 0;
        leader_str = (iptr)(civ_leader_name((int)game.player_civ));
        result = dialog_load_text_section
                          (&dlg,(const char *)&str_game_7,(const char *)&str_name_4,0x17,leader_str,ret,arg6,arg7,arg8);
        if (result == 0) {
          dialog_prepend_title_label((iptr)&dlg,title_offset + 0x25e);
          dlg_pending_idle_callback = net_pump_idle_cb;
          result = dialog_run_modal(&dlg,(iptr)input,0);
          if (result == 0) {
            crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),input);
            crt_strcpy(((i8 *)&net_session_block.net_player_info_leader_name),input);
            net_session_block.net_player_info_tribe_name[0] = 0;
            net_session_block.net_player_info_adjective[0] = 0;
            eh_state = 0xffffffff;
            /* inlined unwind funclet 004467fa */ { dialog_dtor(&dlg); }
            ret = 0;
return_ret:
            return ret;
          }
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          net_send_message(0x31,0,0,civ2,0,0,0,0,0,0);
        }
      }
    }
  } while( true );
}

/* 00447170 auto */
void * __thiscall net_gamelist_view_scalar_dtor(NetGamelistView *self,uint flags)

{
  net_gamelist_view_dtor(self);
  if ((flags & 1) != 0) {
    operator_delete(self);
  }
  return self;
}

/* 004471c0 auto */
LobbyList * __thiscall lobby_list_scalar_dtor(LobbyList *self,uint flags)

{
  lobby_list_dtor(self);
  if ((flags & 1) != 0) {
    operator_delete(self);
  }
  return self;
}

/* 00450480 auto */
void __thiscall parley_create_advance_list_pane(ParleyWindow *self,RECT *rect,iptr pane,int their_side)

{
  int result;
  int scroll_width;
  Button *new_ctrl;
  Wnd *parent22;
  Wnd *parent2;
  u32 hotspot;
  u32 scrollbar;
  uptr advance;
  RECT sb_rect;
  int can_see_other;
  iptr civ;
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  can_see_other = 0;
  sb_rect.left = rect->left;
  sb_rect.top = rect->top;
  sb_rect.right = rect->right;
  sb_rect.bottom = rect->bottom;
  result = civ_has_wonder(human_civ[0],0x18);
  if ((((result == 0) && (result = civ_has_wonder(human_civ[0],9), result == 0)) &&
      ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & 0x80) == 0)) &&
     (((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & 8) == 0 &&
      (game.reveal_map == '\0')))) {
    can_see_other = 0;
  }
  else {
    can_see_other = 1;
  }
  self->list_counts[pane] = 0;
  civ = human_civ[0];
  if ((their_side != 0) && (can_see_other != 0)) {
    civ = (int)parley_wnd_block.other_civ;
  }
  for (advance = 0; (int)advance < 100; advance++) {
    if (their_side == 0) {
      result = civ_has_advance(human_civ[0],advance);
      if ((result != 0) &&
         ((can_see_other == 0 || (result = civ_has_advance((int)parley_wnd_block.other_civ,advance), result == 0)))) {
        self->list_items[(u32)self->list_counts[pane] + pane * 0x801] = advance;
        self->list_selected[(u32)self->list_counts[pane] + pane * 0x801] = 0;
        self->list_counts[pane]++;
      }
    }
    else if (((human_civ[0] == civ) ||
             (result = civ_has_advance(human_civ[0],advance), result != 0)) ||
            (result = civ_has_advance(civ,advance), result == 0)) {
      if (((human_civ[0] == civ) &&
          (result = civ_has_advance(human_civ[0],advance), result == 0)) &&
         (((int)advance < 0x59 && (advances[advance].enabled == 1)))) {
        self->list_items[(u32)self->list_counts[pane] + pane * 0x801] = advance;
        self->list_selected[(u32)self->list_counts[pane] + pane * 0x801] = 0;
        self->list_counts[pane]++;
      }
    }
    else {
      self->list_items[(u32)self->list_counts[pane] + pane * 0x801] = advance;
      self->list_selected[(u32)self->list_counts[pane] + pane * 0x801] = 0;
      self->list_counts[pane]++;
    }
  }
  if ((int)(u32)self->list_counts[pane] < 1) {
    self->list_first_row[pane] = 0xffffffff;
  }
  else {
    self->list_first_row[pane] = 0;
  }
  self->list_columns[pane] = 3;
  self->list_rects[pane * 4] = (u32)rect->left;
  self->list_rects[pane * 4 + 1] = (u32)rect->top;
  self->list_rects[pane * 4 + 2] = (u32)rect->right;
  self->list_rects[pane * 4 + 3] = (u32)rect->bottom;
  if ((int)(u32)self->list_rows[pane] < (int)(u32)self->list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    result = sb_rect.right;
    self->list_rects[pane * 4 + 2] -= scroll_width;
    sb_rect.left = GetSystemMetrics(2);
    sb_rect.left = result - sb_rect.left;
    new_ctrl = (Button *)(operator_new(0x40));
    eh_state = 0;
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (u32)(scrollbar_ctor((Scrollbar *)new_ctrl));
    }
    eh_state = 0xffffffff;
    self->list_scrollbars[pane] = scrollbar;
    if (self == (ParleyWindow *)0x0) {
      parent2 = (Wnd *)0x0;
    }
    else {
      parent2 = &self->window;
    }
    scrollbar_create
              ((Scrollbar *)self->list_scrollbars[pane],(iptr)parent2,pane + 0x411,
               (iptr)&sb_rect,1);
    scrollbar_set_range
              ((Scrollbar *)self->list_scrollbars[pane],0,
               ((u32)self->list_counts[pane] - 1) - (u32)self->list_rows[pane]);
    scrollbar_set_pos((Scrollbar *)self->list_scrollbars[pane],0);
    scrollbar_set_page_size
              ((Scrollbar *)self->list_scrollbars[pane],(u32)self->list_rows[pane]);
    scrollbar_set_callback
              ((Scrollbar *)self->list_scrollbars[pane],
               (pane == 0 ? parley_advance_list0_on_scroll : parley_advance_list1_on_scroll));
    scrollbar_set_track_callback
              ((Scrollbar *)self->list_scrollbars[pane],
               (pane == 0 ? parley_advance_list0_on_scroll : parley_advance_list1_on_scroll));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  eh_state = 1;
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (u32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  eh_state = 0xffffffff;
  self->list_hotspots[pane] = hotspot;
  if (self == (ParleyWindow *)0x0) {
    parent22 = (Wnd *)0x0;
  }
  else {
    parent22 = &self->window;
  }
  hotspot_create
            ((Hotspot *)self->list_hotspots[pane],(iptr)parent22,pane + 0x413,
             (iptr)((u8 *)self + pane * 0x10 + 0x3c0));
  hotspot_set_click_callback((Hotspot *)self->list_hotspots[pane],parley_advance_list_on_click);
  hotspot_set_dblclick_callback((Hotspot *)self->list_hotspots[pane],parley_advance_list_on_click);
  parley_draw_advance_list(pane);
  return;
}

/* 00450ae6 auto */
void __cdecl parley_advance_list0_on_scroll(i32 pos)

{
  parley_advance_list_on_scroll(0,pos);
  return;
}

/* 00450b04 auto */
void __cdecl parley_advance_list1_on_scroll(i32 pos)

{
  parley_advance_list_on_scroll(1,pos);
  return;
}

/* 00450b22 auto */
void __cdecl parley_advance_list_on_scroll(int pane,i32 pos)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)window + 0x48));
  window->list_first_row[pane] = pos;
  parley_draw_advance_list(pane);
  return;
}

/* 00450b83 auto */
void __cdecl parley_advance_list_on_click(int control_id)

{
  uint *sel_flag;
  int row;
  int key_down;
  int range_end;
  int i;
  i32 cursor_y;
  i32 cursor_x;
  int pane;
  ParleyWindow *window;
  
  row = wnd_get_active_child();
  if (row == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(row - 0x48);
  }
  wnd_activate((Wnd *)((iptr)window + 0x48));
  pane = control_id - 0x413;
  wnd_get_cursor_pos
            ((WndBase *)((iptr)window + 0x48),(iptr)&cursor_x,(iptr)&cursor_y);
  if (((window->list_counts[pane] != 0) &&
      (row = parley_advance_list_hit_test(cursor_x,cursor_y,pane), -1 < row)) &&
     (row < window->list_counts[pane])) {
    key_down = wnd_is_shift_down((void *)((iptr)window + 0x48));
    if (key_down == 0) {
      key_down = wnd_is_ctrl_down((void *)((iptr)window + 0x48));
      if (key_down == 0) {
        for (i = 0; i < window->list_counts[pane];
            i = i + 1) {
          if (i != row) {
            window->list_selected[i + pane * 0x801] = 0;
          }
        }
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(row * 4 + pane * 0x2004 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(row * 4 + pane * 0x2004 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < window->list_counts[pane]; i++)
      {
        window->list_selected[i + pane * 0x801] = 0;
      }
      if (window->list_anchor_row[pane] < row) {
        i = window->list_anchor_row[pane];
        range_end = row;
      }
      else {
        range_end = window->list_anchor_row[pane];
        i = row;
      }
      for (; i <= range_end; i++) {
        window->list_selected[i + pane * 0x801] = 1;
      }
    }
    parley_draw_advance_list(pane);
  }
  return;
}

/* 00450df6 auto */
int __cdecl parley_advance_list_hit_test(int px,int py,int pane)

{
  iptr child;
  int result;
  int list_height;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (py < (int)window->list_rects[pane * 4 + 1]) {
    result = -1;
  }
  else if (py < (int)window->list_rects[pane * 4 + 3]) {
    if (px < (int)window->list_rects[pane * 4]) {
      result = -3;
    }
    else if (px < (int)window->list_rects[pane * 4 + 2]) {
      result = (int)window->list_rects[pane * 4 + 1];
      list_height = rect_height((RECT *)(pane * 0x10 + (iptr)window + 0x3c0));
      result = (py - result) / (list_height / (window->list_rows[pane] + 1)) +
              window->list_first_row[pane];
    }
    else {
      result = -4;
    }
  }
  else {
    result = -2;
  }
  return result;
}

/* 00450f0b auto */
void __cdecl parley_draw_advance_list(iptr pane)

{
  u8 *has_sel;
  int tmp;
  int tmp2;
  const char *name_label;
  int icon_x;
  i32 bg_color2;
  i8 sprite_rect[16];
  int text_y_offset;
  iptr advance;
  int icon_step;
  int row;
  int row_top;
  int list_left;
  int list_width2;
  int shadow_color2;
  int text_x_offset;
  Sprite *icon_sprite;
  int text_color2;
  ParleyWindow *window;
  int selected2;
  tagRECT row_rect;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  tmp = wnd_get_active_child();
  if (tmp == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(tmp - 0x48);
  }
  surface_get_clip(window,(RECT *)saved_clip);
  surface_set_clip(window,(RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
  surface_fill_rect
            (window,(iptr)(((u8 *)window + 0x3c0) + pane * 0x10),list_bg_color);
  if (window->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  list_left = (int)window->list_rects[pane * 4];
  list_width2 = rect_width((RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
  row_height = rect_height((RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
  row_height = row_height / (window->list_rows[pane] + 1);
  tmp = row_height / 2;
  tmp2 = font_cached_height(font2);
  text_y_offset = tmp - tmp2 / 2;
  has_sel = (u8 *)&window->side_selected[pane];
  has_sel[0] = '\0';
  has_sel[1] = '\0';
  has_sel[2] = '\0';
  has_sel[3] = '\0';
  row = 0;
  do {
    if (window->list_counts[pane] <= row) {
draw_rows:
      for (row = 0; row < window->list_rows[pane] + 1;
          row = row + 1) {
        row_top = (int)window->list_rects[pane * 4 + 1] + row * row_height +
                   panel_margin_y;
        SetRect(&row_rect,list_left,row_top,list_left + list_width2,row_top + row_height);
        if (((row == 0) || (window->list_first_row[pane] != -1)) &&
           (window->list_first_row[pane] + row <
            window->list_counts[pane])) {
          selected2 = (int)window->list_selected[pane * 0x801 + (window->list_first_row[pane] + row)];
          if (selected2 == 0) {
            text_color2 = list_text_color;
            shadow_color2 = list_shadow_color;
          }
          else {
            text_color2 = dlg_sel_text_color;
            shadow_color2 = dlg_sel_text_shadow_color;
          }
          advance = (int)window->list_items[pane * 0x801 + (window->list_first_row[pane] + row)];
          icon_sprite = (Sprite *)
                     (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                     (char)advances[advance].category * 0xf0 +
                     (char)advances[advance].epoch * 0x3c);
          tmp = sprite_get_width(icon_sprite);
          text_x_offset = panel_margin_x * 3 + tmp * 2;
          row_rect.left += text_x_offset;
          if (selected2 == 0) {
            bg_color2 = list_bg_color;
          }
          else {
            bg_color2 = list_sel_bg_color;
          }
          surface_fill_rect(window,(iptr)&row_rect,bg_color2);
          if (window->list_first_row[pane] == -1) {
            text_clear();
            if ((pane == 0) || ((int)parley_wnd_block.mode == 7)) {
              text_append_label(0x36b);
            }
            else {
              text_append_label(0x36a);
            }
            has_sel = (u8 *)&window->side_selected[pane];
            has_sel[0] = '\0';
            has_sel[1] = '\0';
            has_sel[2] = '\0';
            has_sel[3] = '\0';
            tmp = rect_width((RECT *)(iptr)(((u8 *)window + 0x3c0) + pane * 0x10));
            tmp2 = font_text_width(font2,(const char *)&map_views_block.text_buffer);
            text_x_offset = (tmp - tmp2) / 2;
          }
          else {
            icon_step = sprite_get_width(icon_sprite);
            icon_step += panel_margin_x * 2;
            icon_x = icon_step;
            if ((window->list_first_row[pane] + row & 1U) == 0) {
              icon_x = panel_margin_x;
            }
            tmp2 = row_top + row_height / 2;
            tmp = sprite_get_height(icon_sprite);
            sprite_draw
                      (icon_sprite,(iptr)sprite_rect,(iptr)&parley_wnd_block,list_left + icon_x,tmp2 - tmp / 2);
            text_clear();
            name_label = (label_string(advances[advance].name));
            text_append(name_label);
          }
          if (shadow_color2 != text_color2) {
            gfx_set_text_color_index(shadow_color2);
            surface_draw_text
                      (window,(iptr)font2,(const char *)&map_views_block.text_buffer,list_left + text_x_offset + 2,
                       row_top + text_y_offset + 1,5);
            gfx_set_text_color_index(text_color2);
            surface_draw_text
                      (window,(iptr)font2,(const char *)&map_views_block.text_buffer,list_left + text_x_offset + 1,
                       row_top + text_y_offset,5);
          }
          gfx_set_text_color_index(text_color2);
          surface_draw_text
                    (window,(iptr)font2,(const char *)&map_views_block.text_buffer,list_left + text_x_offset,
                     row_top + text_y_offset,5);
        }
      }
      surface_set_clip(window,(RECT *)saved_clip);
      parley_update_buttons(window);
      ctrl_show_and_redraw((Control *)window->list_hotspots[pane])
      ;
      return;
    }
    if (window->list_selected[pane * 0x801 + row] != 0) {
      has_sel = (u8 *)&window->side_selected[pane];
      has_sel[0] = 1;
      has_sel[1] = '\0';
      has_sel[2] = '\0';
      has_sel[3] = '\0';
      goto draw_rows;
    }
    row++;
  } while( true );
}

/* 004683f0 auto */
void __thiscall
parley_create_unit_list_pane(ParleyWindow *self,RECT *rect,int pane,i32 own_side)

{
  u8 *field;
  int sb_right_orig;
  int scroll_width;
  Button *new_ctrl;
  u8 *parent22;
  u8 *parent2;
  i32 hotspot;
  i32 scrollbar;
  RECT sb_rect;
  int window;
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  window = wnd_get_active_child();
  if (window == 0) {
    window = 0;
  }
  else {
    window -= 0x48;
  }
  prefs_main_menu_choice_block.parley_list_own_side[pane] = own_side;
  sb_rect.left = rect->left;
  sb_rect.top = rect->top;
  sb_rect.right = rect->right;
  sb_rect.bottom = rect->bottom;
  prefs_main_menu_choice_block.parley_units_by_city = 0;
  parley_build_unit_list(self,pane,0);
  if (self->list_counts[pane] < 1) {
    field = (u8 *)&self->list_first_row[pane];
    field[0] = 0xff;
    field[1] = 0xff;
    field[2] = 0xff;
    field[3] = 0xff;
  }
  else {
    field = (u8 *)&self->list_first_row[pane];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  field = (u8 *)&self->list_columns[pane];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  self->list_rects[pane * 4] = rect->left;
  self->list_rects[pane * 4 + 1] = rect->top;
  self->list_rects[pane * 4 + 2] = rect->right;
  self->list_rects[pane * 4 + 3] = rect->bottom;
  if (self->list_rows[pane] <
      self->list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    sb_right_orig = sb_rect.right;
    self->list_rects[pane * 4 + 2] =
         (int)self->list_rects[pane * 4 + 2] - scroll_width;
    sb_rect.left = GetSystemMetrics(2);
    sb_rect.left = sb_right_orig - sb_rect.left;
    new_ctrl = (Button *)(operator_new(0x40));
    eh_state = 0;
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(scrollbar_ctor((Scrollbar *)new_ctrl));
    }
    eh_state = 0xffffffff;
    self->list_scrollbars[pane] = scrollbar;
    if (self == (ParleyWindow *)0x0) {
      parent2 = (u8 *)0x0;
    }
    else {
      parent2 = ((u8 *)self + 0x48);
    }
    scrollbar_create
              ((Scrollbar *)self->list_scrollbars[pane],(iptr)parent2,
               pane + 0x3fa,(iptr)&sb_rect,1);
    scrollbar_set_range
              ((Scrollbar *)self->list_scrollbars[pane],0,
               self->list_counts[pane] -
               self->list_rows[pane]);
    scrollbar_set_pos((Scrollbar *)self->list_scrollbars[pane],0);
    scrollbar_set_page_size
              ((Scrollbar *)self->list_scrollbars[pane],
               self->list_rows[pane] - 1);
    scrollbar_set_callback
              ((Scrollbar *)self->list_scrollbars[pane],
               (pane == 0 ? parley_unit_list_scroll_left : parley_unit_list_scroll_right));
    scrollbar_set_track_callback
              ((Scrollbar *)self->list_scrollbars[pane],
               (pane == 0 ? parley_unit_list_scroll_left : parley_unit_list_scroll_right));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  eh_state = 1;
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  eh_state = 0xffffffff;
  self->list_hotspots[pane] = hotspot;
  if (self == (ParleyWindow *)0x0) {
    parent22 = (u8 *)0x0;
  }
  else {
    parent22 = ((u8 *)self + 0x48);
  }
  hotspot_create
            ((Hotspot *)self->list_hotspots[pane],(iptr)parent22,
             pane + 0x3fc,(iptr)(((u8 *)self + 0x39c) + pane * 0x10 + 0x24));
  hotspot_set_click_callback((Hotspot *)self->list_hotspots[pane],parley_unit_list_on_click);
  hotspot_set_dblclick_callback((Hotspot *)self->list_hotspots[pane],parley_unit_list_on_click);
  parley_draw_unit_list(pane);
  return;
}

/* 00468797 auto */
void __cdecl parley_unit_list_scroll_left(i32 pos)

{
  parley_unit_list_on_scroll(0,pos);
  return;
}

/* 004687b5 auto */
void __cdecl parley_unit_list_scroll_right(i32 pos)

{
  parley_unit_list_on_scroll(1,pos);
  return;
}

/* 004687d3 auto */
void __cdecl parley_unit_list_on_scroll(int pane,i32 pos)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)window + 0x48));
  window->list_first_row[pane] = pos;
  parley_draw_unit_list(pane);
  return;
}

/* 00468834 auto */
void __cdecl parley_unit_list_on_click(int control_id)

{
  uint *sel_flag;
  int row;
  int key_down;
  int range_end;
  int i;
  i32 cursor_y;
  i32 cursor_x;
  int pane;
  ParleyWindow *window;
  
  row = wnd_get_active_child();
  if (row == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(row - 0x48);
  }
  wnd_activate((Wnd *)((iptr)window + 0x48));
  pane = control_id - 0x3fc;
  wnd_get_cursor_pos
            ((WndBase *)((iptr)window + 0x48),(iptr)&cursor_x,(iptr)&cursor_y);
  if (((window->list_counts[pane] != 0) &&
      (row = parley_unit_list_hit_test(cursor_x,cursor_y,pane), -1 < row)) &&
     (row < window->list_counts[pane])) {
    key_down = wnd_is_shift_down((void *)((iptr)window + 0x48));
    if (key_down == 0) {
      key_down = wnd_is_ctrl_down((void *)((iptr)window + 0x48));
      if (key_down == 0) {
        for (i = 0; i < window->list_counts[pane];
            i = i + 1) {
          if (i != row) {
            window->list_selected[pane * 0x801 + i] = 0;
          }
        }
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(pane * 0x2004 + row * 4 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        window->list_anchor_row[pane] = row;
        sel_flag = (uint *)(pane * 0x2004 + row * 4 + 0x8400 + (iptr)window);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < window->list_counts[pane]; i++)
      {
        window->list_selected[pane * 0x801 + i] = 0;
      }
      if (window->list_anchor_row[pane] < row) {
        i = window->list_anchor_row[pane];
        range_end = row;
      }
      else {
        range_end = window->list_anchor_row[pane];
        i = row;
      }
      for (; i <= range_end; i++) {
        window->list_selected[pane * 0x801 + i] = 1;
      }
    }
    parley_draw_unit_list(pane);
  }
  return;
}

/* 00468aa7 auto */
int __cdecl parley_unit_list_hit_test(int px,int py,int pane)

{
  iptr child;
  int result;
  int list_height;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (py < (int)window->list_rects[pane * 4 + 1]) {
    result = -1;
  }
  else if (py < (int)window->list_rects[pane * 4 + 3]) {
    if (px < (int)window->list_rects[pane * 4]) {
      result = -3;
    }
    else if (px < (int)window->list_rects[pane * 4 + 2]) {
      result = (int)window->list_rects[pane * 4 + 1];
      list_height = rect_height((RECT *)(pane * 0x10 + (iptr)window + 0x3c0));
      result = (py - result) / (list_height / window->list_rows[pane]) +
              window->list_first_row[pane];
    }
    else {
      result = -4;
    }
  }
  else {
    result = -2;
  }
  return result;
}

/* 00468bb9 auto */
void __cdecl parley_draw_unit_list(iptr pane)

{
  u8 *has_sel;
  int tmp;
  int tmp2;
  iptr str;
  int row_mid;
  i32 denom;
  int icon_x;
  i32 bg_color2;
  int icon_width;
  i8 sprite_rect[16];
  int text_y_offset;
  iptr unit;
  int icon_step;
  int row;
  int row_top;
  int list_left;
  int list_width2;
  int shadow_color2;
  int text_x_offset;
  Sprite *unit_sprite;
  int text_color2;
  ParleyWindow *this2;
  int selected2;
  tagRECT row_rect;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  tmp = wnd_get_active_child();
  if (tmp == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(tmp - 0x48);
  }
  surface_get_clip(this2,(RECT *)saved_clip);
  surface_set_clip(this2,(RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
fill_list_bg:
  surface_fill_rect
            (this2,(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10),list_bg_color);
  if (this2->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  list_left = (int)this2->list_rects[pane * 4];
  list_width2 = rect_width((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
  row_height = rect_height((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
  row_height = row_height / this2->list_rows[pane];
  tmp = row_height / 2;
  tmp2 = font_cached_height(font2);
  text_y_offset = tmp - tmp2 / 2;
  has_sel = (u8 *)&this2->side_selected[pane];
  has_sel[0] = '\0';
  has_sel[1] = '\0';
  has_sel[2] = '\0';
  has_sel[3] = '\0';
  for (row = 0; row < this2->list_counts[pane];
      row = row + 1) {
    if (this2->list_selected[row + pane * 0x801] != 0) {
      has_sel = (u8 *)&this2->side_selected[pane];
      has_sel[0] = 1;
      has_sel[1] = '\0';
      has_sel[2] = '\0';
      has_sel[3] = '\0';
      break;
    }
  }
  row = 0;
  do {
    if (this2->list_rows[pane] <= row) {
restore_clip_and_finish:
      surface_set_clip(this2,(RECT *)saved_clip);
      parley_update_buttons(this2);
      ctrl_show_and_redraw((Control *)this2->list_hotspots[pane])
      ;
      return;
    }
    row_top = (int)this2->list_rects[pane * 4 + 1] + row_height * row;
    SetRect(&row_rect,list_left,row_top,list_width2 + list_left,row_height + row_top);
    if ((row != 0) && (this2->list_first_row[pane] == -1))
    goto restore_clip_and_finish;
    if (this2->list_first_row[pane] + row <
        this2->list_counts[pane]) {
      selected2 = (int)this2->list_selected[pane * 0x801 + (this2->list_first_row[pane] + row)];
      if (selected2 == 0) {
        text_color2 = list_text_color;
        shadow_color2 = list_shadow_color;
      }
      else {
        text_color2 = dlg_sel_text_color;
        shadow_color2 = dlg_sel_text_shadow_color;
      }
      unit = 0;
      if ((0 < this2->list_counts[pane]) &&
         (unit = parley_find_unit_by_id
                               (this2,
                                (i32)this2->list_items_b[pane * 0x801 + (this2->list_first_row[pane] + row)]), unit < 0)) break;
      unit_sprite = (Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c);
      if (this2->large_layout == 0) {
        tmp = sprite_get_width(unit_sprite);
        icon_width = (tmp * 2) / 3;
      }
      else {
        icon_width = sprite_get_width(unit_sprite);
      }
      text_x_offset = (-(uint)(this2->large_layout == 0) & 0xfffffffc) + 10 + icon_width * 2;
      row_rect.left = row_rect.left + text_x_offset - 3;
      if (selected2 == 0) {
        bg_color2 = list_bg_color;
      }
      else {
        bg_color2 = list_sel_bg_color;
      }
      surface_fill_rect(this2,(iptr)&row_rect,bg_color2);
      if (this2->list_first_row[pane] == -1) {
        text_clear();
        str = (iptr)(label_string(label_string_ids[0x2df]));
        text_append((const char *)str);
        has_sel = (u8 *)&this2->side_selected[pane];
        has_sel[0] = '\0';
        has_sel[1] = '\0';
        has_sel[2] = '\0';
        has_sel[3] = '\0';
        tmp = rect_width((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
        tmp2 = font_text_width(font2,(const char *)&map_views_block.text_buffer);
        text_x_offset = (tmp - tmp2) / 2;
      }
      else {
        denom = 0x30;
        tmp = font_cached_height(font2);
        gfx_set_scale_ratio(tmp * 2,denom);
        tmp = font_cached_height(font2);
        icon_step = (tmp << 7) / 0x30;
        icon_x = icon_step;
        if ((this2->list_first_row[pane] + row & 1U) == 0) {
          icon_x = 0;
        }
        row_mid = row_top + row_height / 2;
        tmp = sprite_get_height(unit_sprite);
        tmp2 = font_cached_height(font2);
        sprite_draw
                  (unit_sprite,(iptr)sprite_rect,(iptr)&parley_wnd_block,list_left + icon_x,
                   row_mid - (tmp * tmp2) / 0x30);
        gfx_set_scale_ratio(1,1);
        if (prefs_main_menu_choice_block.parley_units_by_city == 0) {
          text_clear();
          text_append_string_id(unit_types[units[unit].type].name);
          if ((((int)parley_wnd_block.mode == 9) ||
              ((((int)parley_wnd_block.mode == 0xe && (pane == 0)) && (this2->local_units_revealed != '\0')))) ||
             (((((int)parley_wnd_block.mode == 0xe && (pane == 1)) && (this2->remote_units_revealed != '\0')) ||
              (((int)parley_wnd_block.mode == 0xf && (this2->remote_units_revealed != '\0')))))) {
            text_append_str((const char *)&str_empty_76);
            str = unit_home_city_name(unit);
            text_append_str((const char *)str);
            text_append_str((const char *)&str_empty_77);
          }
        }
        else {
          text_clear();
          str = unit_home_city_name(unit);
          text_append_str((const char *)str);
          text_append_str((const char *)&str_empty_78);
          text_append_string_id(unit_types[units[unit].type].name);
          text_append_str((const char *)&str_empty_79);
        }
      }
      if (shadow_color2 != text_color2) {
        gfx_set_text_color_index(shadow_color2);
        surface_draw_text
                  (this2,(iptr)font2,(const char *)&map_views_block.text_buffer,text_x_offset + list_left + 2,
                   text_y_offset + row_top + 1,5);
        gfx_set_text_color_index(text_color2);
        surface_draw_text
                  (this2,(iptr)font2,(const char *)&map_views_block.text_buffer,text_x_offset + list_left + 1,
                   text_y_offset + row_top,5);
      }
      gfx_set_text_color_index(text_color2);
      surface_draw_text
                (this2,(iptr)font2,(const char *)&map_views_block.text_buffer,text_x_offset + list_left,
                 text_y_offset + row_top,5);
    }
    row++;
  } while( true );
  parley_build_unit_list(this2,pane,1);
  if (this2->list_counts[pane] == 0) {
    has_sel = (u8 *)&this2->list_first_row[pane];
    has_sel[0] = 0xff;
    has_sel[1] = 0xff;
    has_sel[2] = 0xff;
    has_sel[3] = 0xff;
  }
  else if (this2->list_counts[pane] <
           this2->list_counts[pane]) {
    this2->list_first_row[pane] =
         this2->list_counts[pane] - 1;
  }
  goto fill_list_bg;
}

/* 004692ea auto */
void __cdecl parley_reveal_city_origin_clicked(void)

{
  iptr child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)window + 0x48));
  ctrl_show_and_redraw((Control *)window->info_hide_buttons[0]);
  control_hide((Control *)window->info_show_buttons[0]);
  window->local_units_revealed = 1;
  ctrl_enable((Button *)window->sort_by_unit_buttons[0]);
  ctrl_enable((Button *)window->sort_by_city_buttons[0]);
  net_send_message(0xa7,net_session_block.net_slots[net_session_block.net_civ_slots[(int)window->other_civ]].id,
                     human_civ[0],(int)(char)window->local_units_revealed,0,0,0,0,0,0);
  parley_draw_unit_list(0);
  return;
}

/* 004693c5 auto */
void __cdecl parley_hide_city_origin_clicked(void)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)window + 0x48));
  ctrl_show_and_redraw((Control *)window->info_show_buttons[0]);
  control_hide((Control *)window->info_hide_buttons[0]);
  window->local_units_revealed = 0;
  ctrl_disable((Button *)window->sort_by_unit_buttons[0]);
  ctrl_disable((Button *)window->sort_by_city_buttons[0]);
  net_send_message(0xa8,net_session_block.net_slots[net_session_block.net_civ_slots[window->other_civ]].id,human_civ[0]
                     ,(int)(char)window->local_units_revealed,0,0,0,0,0,0);
  parley_sort_unit_list(window,0,(u32)window->list_counts[0] - 1,0);
  parley_draw_unit_list(0);
  return;
}

/* 004694b7 auto */
void __cdecl parley_request_city_origin_clicked(void)

{
  iptr child;
  ParleyWindow *window;
  const char *civ_plural;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)window + 0x48));
  civ_plural = (civ_name_plural(human_civ[0]));
  text_set_string_arg(0,civ_plural);
  net_send_text_args(100,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,1,0,
                    human_civ[0],0);
  return;
}

/* 00469547 auto */
void __cdecl parley_sort_by_city_left_clicked(void)

{
  parley_sort_by_city_clicked(0);
  return;
}

/* 00469561 auto */
void __cdecl parley_sort_by_city_right_clicked(void)

{
  parley_sort_by_city_clicked(1);
  return;
}

/* 0046957b auto */
void __cdecl parley_sort_by_city_clicked(iptr pane)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((int)window + 0x48));
  parley_sort_units_by_city(window,0,window->list_counts[pane] - 1,pane);
  parley_draw_unit_list(pane);
  return;
}

/* 004695e9 auto */
void __cdecl parley_sort_by_unit_left_clicked(void)

{
  parley_sort_by_unit_clicked(0);
  return;
}

/* 00469603 auto */
void __cdecl parley_sort_by_unit_right_clicked(void)

{
  parley_sort_by_unit_clicked(1);
  return;
}

/* 0046961d auto */
void __cdecl parley_sort_by_unit_clicked(iptr pane)

{
  int child;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)window + 0x48));
  parley_sort_unit_list(window,0,(u32)window->list_counts[pane] - 1,pane);
  parley_draw_unit_list(pane);
  return;
}

/* 0046968b auto */
void __thiscall parley_sort_unit_list(ParleyWindow *self,int first,int last,iptr pane)

{
  i32 swap;
  iptr child;
  int cmp;
  int j;
  int i;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (1 < window->list_counts[pane]) {
    prefs_main_menu_choice_block.parley_units_by_city = 0;
    for (i = first; j = i, i < last; i++) {
      while (j = j + 1, j <= last) {
        cmp = _strcmp(*(char **)(((i8 *)&prefs_main_menu_choice_block.unit_type_name_ptrs) +
                                  (uint)units[(int)window->list_items[i + pane * 0x801]].type * 4),
                        *(char **)(((i8 *)&prefs_main_menu_choice_block.unit_type_name_ptrs) +
                                  (uint)units[(int)window->list_items[j + pane * 0x801]].type * 4));
        if (0 < cmp) {
          swap = (i32)window->list_items[i + pane * 0x801];
          window->list_items[i + pane * 0x801] =
               (i32)window->list_items[j + pane * 0x801];
          window->list_items[j + pane * 0x801] = swap;
          swap = (i32)window->list_items_b[i + pane * 0x801];
          window->list_items_b[i + pane * 0x801] =
               (i32)window->list_items_b[j + pane * 0x801];
          window->list_items_b[j + pane * 0x801] = swap;
          swap = (i32)window->list_selected[i + pane * 0x801];
          window->list_selected[i + pane * 0x801] =
               (i32)window->list_selected[j + pane * 0x801];
          window->list_selected[j + pane * 0x801] = swap;
        }
      }
    }
  }
  return;
}

/* 0046990a auto */
void __thiscall parley_sort_units_by_city(ParleyWindow *self,int first,int last,iptr pane)

{
  i32 swap;
  iptr child;
  char *_Str2;
  char *_Str1;
  int cmp;
  int j;
  int i;
  ParleyWindow *window;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (window->list_counts[pane] < 2) {
    return;
  }
  prefs_main_menu_choice_block.parley_units_by_city = 1;
  i = first;
outer_loop:
  j = i;
  if (last <= i) {
    return;
  }
inner_loop:
  do {
    j++;
    if (last < j) break;
    if (units[(int)window->list_items[i + pane * 0x801]].home_city != -1) {
      if (units[(int)window->list_items[j + pane * 0x801]].home_city != -1) {
        _Str2 = (char *)unit_home_city_name((iptr)window->list_items[j + pane * 0x801]);
        _Str1 = (char *)unit_home_city_name((iptr)window->list_items[i + pane * 0x801]);
        cmp = _strcmp(_Str1,_Str2);
        if (cmp < 1) goto inner_loop;
      }
      swap = (i32)window->list_items[i + pane * 0x801];
      window->list_items[i + pane * 0x801] =
           (i32)window->list_items[j + pane * 0x801];
      window->list_items[j + pane * 0x801] = swap;
      swap = (i32)window->list_items_b[i + pane * 0x801];
      window->list_items_b[i + pane * 0x801] =
           (i32)window->list_items_b[j + pane * 0x801];
      window->list_items_b[j + pane * 0x801] = swap;
      swap = (i32)window->list_selected[i + pane * 0x801];
      window->list_selected[i + pane * 0x801] =
           (i32)window->list_selected[j + pane * 0x801];
      window->list_selected[j + pane * 0x801] = swap;
    }
  } while( true );
  i++;
  goto outer_loop;
}

/* 00469bdc auto */
void __thiscall parley_build_unit_list(ParleyWindow *self,int pane,int keep_selection)

{
  int *count_ptr;
  short x2;
  short y2;
  bool near_land;
  iptr child;
  iptr x;
  int y;
  int in_bounds;
  iptr unit;
  int old_count;
  int k;
  void *selected_ids;
  iptr dir;
  ParleyWindow *window;
  
  selected_ids = (void *)0x0;
  child = wnd_get_active_child();
  if (child == 0) {
    window = 0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  if (keep_selection != 0) {
    selected_ids = operator_new(window->list_counts[pane] << 2);
    if (selected_ids == (void *)0x0) {
      keep_selection = 0;
    }
    else {
      old_count = window->list_counts[pane];
      for (unit = 0; unit < old_count; unit++) {
        if (window->list_selected[pane * 0x801 + unit] == 0) {
          *(i32 *)((int)selected_ids + unit * 4) = 0xffffffff;
        }
        else {
          *(i32 *)((int)selected_ids + unit * 4) =
               (i32)window->list_items_b[pane * 0x801 + unit];
        }
      }
    }
  }
  window->list_counts[pane] = 0;
  unit = 0;
  do {
    if (game.num_units <= unit) {
      if ((keep_selection != 0) && (selected_ids != (void *)0x0)) {
        operator_delete(selected_ids);
      }
      if (prefs_main_menu_choice_block.parley_units_by_city == 0) {
        parley_sort_unit_list
                  (self,0,window->list_counts[pane] - 1,pane);
      }
      else {
        parley_sort_units_by_city(self,0,window->list_counts[pane] - 1,pane);
      }
      return;
    }
    if ((units[unit].id != 0) &&
       (((prefs_main_menu_choice_block.parley_list_own_side[pane] != 0 && (units[unit].owner == (*(i8 *)&human_civ))) ||
        (((prefs_main_menu_choice_block.parley_list_own_side[pane] == 0 &&
          ((int)units[unit].owner == self->other_civ)) &&
         (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) != 0 ||
          ((int)units[unit].owner == ((*(i8 *)&human_civ) & 0xffU))))))))) {
      x2 = units[unit].x;
      y2 = units[unit].y;
      near_land = false;
      for (dir = 0; dir < 8; dir++) {
        x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
        y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
        in_bounds = map_in_bounds(x,y);
        if ((in_bounds != 0) && (y = tile_is_ocean(x,y), y == 0)) {
          near_land = true;
          break;
        }
      }
      if ((near_land) || (y = unit_stack_sum(unit,2), y == 1)) {
        window->list_items[pane * 0x801 + window->list_counts[pane]] =
             unit;
        window->list_items_b[pane * 0x801 + window->list_counts[pane]] =
             units[unit].id;
        window->list_selected[pane * 0x801 + window->list_counts[pane]] = 0;
        if ((keep_selection == 1) && (selected_ids != (void *)0x0)) {
          for (k = 0; k < old_count; k++) {
            if (units[unit].id == *(int *)((int)selected_ids + k * 4)) {
              window->list_selected[pane * 0x801 + window->list_counts[pane]] =
                   1;
              break;
            }
          }
        }
        count_ptr = (int *)((iptr)window + 1000 + pane * 4);
        *count_ptr = *count_ptr + 1;
      }
    }
    unit++;
  } while( true );
}

/* 0046af70 auto */
i8 __thiscall net_session_send_to(NetSession *self,int player,NetMsg *msg)

{
  i8 ok;
  int sent;
  
  if ((((player < 0) || (7 < player)) && (player != 0xff)) && (player != 0)) {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_sock_0_sock_max_net_players_sock_broadca,(iptr)&str_d_ss_franklinton_netmgr_send_cpp,0x139);
  }
  if (msg->size == 0) {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_pmsg_size_0,(iptr)&str_d_ss_franklinton_netmgr_send_cpp_2,0x13a);
  }
  msg->seq = net_send_sequence;
  net_send_sequence++;
  sent = XD_SendSecureData((ushort)player,(void *)msg,(ulong)msg->size,0);
  self->net_last_send_result = sent;
  if (self->net_last_send_result < 0) {
    dbg_logf3((const char *)&str_sendtoconnections_failed_on_send_to_conn,player,self->net_last_send_result,(i32)msg->hdr.msg_type);
    if ((int)msg->hdr.msg_type == 0x6a) {
      dbg_logf_str((const char *)&str_popup_type_s,(const char *)(((char *)&str_nm_pop_retiredie) + msg->args[0] * 0x20))
      ;
    }
    ok = 0;
  }
  else {
    ok = 1;
  }
  return ok;
}

/* 0046b0a1 auto */
bool __thiscall net_session_send_to_all(NetSession *self,NetMsg *msg)

{
  bool failed;
  int sent;
  
  msg->seq = net_send_sequence;
  net_send_sequence++;
  sent = XD_SendBroadcastData((void *)msg,(ulong)msg->size,0);
  self->net_last_send_result = sent;
  failed = self->net_last_send_result < msg->size;
  if (failed) {
    dbg_logf((const char *)&str_sendtoeveryone_failed_return_val_d,self->net_last_send_result);
  }
  return !failed;
}

/* 0046b14d auto */
/* WARNING: Removing unreachable block (ram,0x0046cdce) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl
net_send_message(int msg_type,i32 dest,iptr arg1,iptr arg2,iptr arg3,iptr arg4,
            iptr arg5,i32 arg6,const char *text,iptr raw_msg)

{
  char sent;
  size_t text_len;
  NetMsgHeader msg_5b[2];
  NetMsgHeader msg_2b[2];
  NetMsgHeader msg_9c[2];
  NetMsgHeader msg_58[2];
  NetMsgHeader msg_8d[2];
  NetMsgHeader msg_8b[2];
  NetMsgHeader msg_26[2];
  NetMsgHeader msg_46[2];
  NetMsgHeader msg_44[2];
  NetMsgHeader msg_42[2];
  NetMsgHeader msg_40[2];
  NetMsgHeader msg_8a[2];
  NetMsgHeader msg_83[2];
  NetMsgHeader msg_35[2];
  NetMsgHeader msg_79[2];
  NetMsgHeader msg_78[2];
  NetMsgHeader msg_74[2];
  NetWhoseTurnMsg whose_turn_msg[1];
  RECT caption_rect;
  NetMsgHeader msg_16[2];
  NetMsgHeader msg_5d[2];
  NetMsgHeader msg_14[2];
  NetServerAnswerMsg answer_msg[1];
  NetMsgHeader msg_12[2];
  NetMsgHeader msg_6f[2];
  NetMsgHeader msg_6e[2];
  NetMsgHeader msg_6d[2];
  NetMsgHeader simple_msg[2];
  uint image_result;
  void *image_buf;
  NetPlayerNodeMsg player_node_msg[1];
  NetPlayerListMsg player_list_msg[1];
  /* (the original's locals lay so: the message whole) */
  struct { NetMsgHeader hdr[2]; i16 kind; i8 name[260]; i8 subdir[262]; } server_path_msg[1];
  NetMsgHeader msg_05[2];
  NetServerInfoMsg server_info_msg[1];
  NetMsgHeader announce_msg[15];
  void *packet;
  int result;
  
  packet = (void *)0x0;
  switch(msg_type) {
  case 1:
    net_msg_init_header((NetNamedMsg *)announce_msg,msg_type,(const char *)&str_empty_80);
    sent = net_session_send_to_all(&net_session_block,(NetMsg *)announce_msg);
    return (int)sent;
  case 2:
    net_msg_server_info_init((NetServerInfoMsg *)server_info_msg);
    server_info_msg->game.session_phase = (i16)net_session_block.net_session_phase;
    sent = net_session_send_to_all(&net_session_block,(NetMsg *)server_info_msg);
    return (int)sent;
  default:
    return 0;
  case 4:
    net_msg_player_list_init((NetPlayerListMsg *)player_list_msg);
    crt_memcpy(&player_list_msg->player_count,((iptr *)&net_session_block.net_player_count),0x270);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)player_list_msg);
    return (int)sent;
  case 5:
    net_msg_header_init(msg_05,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_05);
    return (int)sent;
  case 6:
    net_msg_server_path_init(server_path_msg->hdr);
    server_path_msg->kind = net_session_block.loaded_file_kind;
    crt_strcpy(server_path_msg->name,((i8 *)&net_session_block.loaded_file_name));
    crt_strcpy(server_path_msg->subdir,((i8 *)&net_session_block.loaded_file_subdir));
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)server_path_msg);
    return (int)sent;
  case 7:
  case 8:
  case 9:
  case 10:
    image_buf = (void *)0x0;
    if (msg_type == 7) {
      image_result = diffeng_build_full_image((iptr)&image_buf);
    }
    else if (msg_type == 8) {
      image_result = diffeng_build_delta_image((iptr)&image_buf);
    }
    else if (msg_type == 9) {
      image_result = diffeng_build_civ_image((iptr)&image_buf);
    }
    else if (msg_type == 10) {
      image_result = diffeng_build_map_header_image((iptr)&image_buf);
    }
    image_result = (uint)(image_result != -1);
    if (image_buf == (void *)0x0) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_address_null,(iptr)&str_d_ss_franklinton_netmgr_send_cpp_3,0x22c);
    }
    image_buf = net_msg_build_packet(msg_type,(iptr)image_buf,image_result,0,0,0,0,0,0);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)image_buf);
    image_result = (uint)sent;
    operator_delete(image_buf);
    return image_result;
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
  case 0x10:
  case 0x11:
  case 0x32:
    net_msg_header_init(simple_msg,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)simple_msg);
    return (int)sent;
  case 0x12:
    net_msg_header_init(msg_12,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_12);
    return (int)sent;
  case 0x13:
    net_msg_server_answer_init((NetServerAnswerMsg *)answer_msg);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)answer_msg);
    return (int)sent;
  case 0x14:
    net_msg_header_init(msg_14,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_14);
    return (int)sent;
  case 0x15:
  case 0x5c:
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)raw_msg);
    if (sent == '\0') {
      return 0;
    }
    return *(i32 *)(raw_msg + 8);
  case 0x16:
    net_msg_header_init(msg_16,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_16);
    return (int)sent;
  case 0x17:
    packet = net_msg_build_packet(msg_type,(iptr)&cosmic,0,0,0,0,0,0,0x29);
    break;
  case 0x18:
    packet = net_msg_build_packet(msg_type,(iptr)&advances,0,0,0,0,0,0,0x640);
    break;
  case 0x19:
    packet = net_msg_build_packet(msg_type,(iptr)&improvements,0,0,0,0,0,0,0x218);
    break;
  case 0x1a:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)wonder_expires),0,0,0,0,0,0,0x1c);
    break;
  case 0x1b:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)unit_types),0,0,0,0,0,0,0x4d8);
    break;
  case 0x1c:
    packet = net_msg_build_packet(msg_type,(iptr)&terrain_types,0,0,0,0,0,0,0x318);
    break;
  case 0x1d:
    packet = net_msg_build_packet(msg_type,(iptr)&government_names,0,0,0,0,0,0,0x1c);
    break;
  case 0x1e:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)government_titles),0,0,0,0,0,0,0x38);
    break;
  case 0x1f:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)leaders),0,0,0,0,0,0,0x3f0);
    break;
  case 0x20:
    packet = net_msg_build_packet(msg_type,(iptr)&civs,0,0,0,0,0,0,0x7524);
    break;
  case 0x21:
    packet = net_msg_build_packet(msg_type,(iptr)&commodity_names,0,0,0,0,0,0,0x40);
    break;
  case 0x22:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)order_names),0,0,0,0,0,0,0x68);
    break;
  case 0x23:
    packet = net_msg_build_packet(msg_type,(iptr)((u8 *)difficulty_names),0,0,0,0,0,0,0x18);
    break;
  case 0x24:
    packet = net_msg_build_packet(msg_type,(iptr)&attitude_names,0,0,0,0,0,0,0x24);
    break;
  case 0x25:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x26:
  case 0x27:
    net_msg_header_init(msg_26,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_26);
    return (int)sent;
  case 0x28:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x2a:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    net_session_block.nm_lock_data_count++;
    break;
  case 0x2b:
  case 0x2c:
    net_msg_header_init(msg_2b,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_2b);
    return (int)sent;
  case 0x2d:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x2e:
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)&net_session_block.net_ping_message);
    return (int)sent;
  case 0x2f:
    net_msg_player_list_node_init((NetPlayerNodeMsg *)player_node_msg,arg1);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)player_node_msg);
    return (int)sent;
  case 0x30:
    if (net_session_block.net_host_flag == '\0') {
      packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
      break;
    }
    if ((1 << ((byte)arg2 & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) == 0) {
      (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) | (ushort)(1 << ((byte)arg2 & 0x1f));
      net_send_message(0x2a,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
      net_session_block.nm_nation_accept_count++;
      return 1;
    }
    net_session_block.nm_nation_reject_count++;
  case 0x31:
    if (net_session_block.net_host_flag != '\0') {
      game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
      if ((1 << ((byte)arg2 & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
        (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)arg2 & 0x1f));
        game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
        net_send_message(0x2a,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
        net_session_block.nm_lock_data_count++;
      }
      return 1;
    }
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x33:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x34:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x35:
  case 0x81:
    net_msg_header_init(msg_35,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_35);
    return (int)sent;
  case 0x37:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x38:
    packet = net_msg_build_packet(msg_type,(iptr)(units + arg1),arg1,(int)game.num_units
                                         ,arg3,arg4,arg5,arg6,0x20);
    break;
  case 0x39:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x3a:
    packet = net_msg_build_packet(msg_type,(iptr)(cities + arg1),arg1,
                                         (int)game.num_cities,arg3,arg4,arg5,arg6,0x58);
    break;
  case 0x3b:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x3c:
    if (arg1 == -1) {
      packet = net_msg_build_packet(msg_type,0,0xffffffff,0,0,0,0,0,0);
    }
    else {
      packet = net_msg_build_packet(msg_type,(iptr)(cities + arg1),arg1,
                                           (int)game.num_cities,arg3,arg4,arg5,arg6,0x58
                                          );
    }
    break;
  case 0x3d:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x3e:
    if (arg1 == -1) {
      packet = net_msg_build_packet(msg_type,0,0xffffffff,0,0,0,0,0,0);
    }
    else {
      packet = net_msg_build_packet(msg_type,(iptr)(units + arg1),arg1,
                                           (int)game.num_units,arg3,arg4,arg5,arg6,0x20)
      ;
    }
    break;
  case 0x3f:
  case 0x4b:
  case 0x4d:
  case 0x4f:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x40:
  case 0x4c:
  case 0x4e:
  case 0x50:
    net_msg_header_init(msg_40,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_40);
    return (int)sent;
  case 0x41:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x42:
    net_msg_header_init(msg_42,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_42);
    return (int)sent;
  case 0x43:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x44:
    net_msg_header_init(msg_44,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_44);
    return (int)sent;
  case 0x45:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x46:
    net_msg_header_init(msg_46,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_46);
    return (int)sent;
  case 0x47:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case 0x48:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x49:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x4a:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x51:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case 0x52:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x53:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x54:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x55:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case 0x56:
  case 0x57:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x58:
    net_msg_header_init(msg_58,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_58);
    return (int)sent;
  case 0x59:
    packet = net_msg_build_packet(msg_type,(iptr)&net_batch_buffer,0,0,0,0,0,0,net_batch_used_dwords << 2);
    break;
  case 0x5a:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,arg6,0
                                        );
    break;
  case 0x5b:
  case 0x65:
  case 0x66:
  case 0xa3:
    net_msg_header_init(msg_5b,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_5b);
    return (int)sent;
  case 0x5d:
  case 0x5e:
    net_msg_header_init(msg_5d,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_5d);
    return (int)sent;
  case 0x5f:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x60:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x61:
  case 0x62:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 99:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 100:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case 0x68:
    text_len = _strlen((char *)text);
    packet = net_msg_build_packet(msg_type,(iptr)text,arg1,arg2,arg3,0,0,0,text_len + 1);
    break;
  case 0x69:
    net_msg_whose_turn_init((NetWhoseTurnMsg *)whose_turn_msg,arg1);
    map_views_block.main_view_timer_cache_i32 = 0xffffffff;
    frame_wnd_draw_caption((FrameWindow *)((i8 *)&map_views_block.map_views));
    caption_rect.left = map_views_block.main_view_frame_left;
    caption_rect.top = map_views_block.main_view_frame_top;
    caption_rect.right = map_views_block.main_view_frame_right;
    caption_rect.bottom = map_views_block.main_view_client_top;
    wnd_blit_surface_rect((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&caption_rect);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)whose_turn_msg);
    return (int)sent;
  case 0x6a:
    packet = net_msg_build_packet(msg_type,raw_msg,arg1,arg2,arg3,arg4,arg5,0,
                                         0);
    break;
  case 0x6b:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x6c:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x6d:
    net_msg_header_init(msg_6d,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_6d);
    return (int)sent;
  case 0x6e:
    net_msg_header_init(msg_6e,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_6e);
    return (int)sent;
  case 0x6f:
    net_msg_header_init(msg_6f,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_6f);
    return (int)sent;
  case 0x70:
    packet = net_msg_build_packet(msg_type,(iptr)(units + arg1),arg1,arg2,arg3,
                                         arg4,arg5,arg6,0x20);
    break;
  case 0x71:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x72:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x73:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x74:
    net_msg_header_init(msg_74,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_74);
    return (int)sent;
  case 0x75:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x76:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x77:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x78:
    net_msg_header_init(msg_78,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_78);
    return (int)sent;
  case 0x79:
    net_msg_header_init(msg_79,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_79);
    return (int)sent;
  case 0x7a:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x7b:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x7c:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x7d:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x7e:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x7f:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x80:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x82:
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)raw_msg);
    if (sent == '\0') {
      return 0;
    }
    return *(i32 *)(raw_msg + 8);
  case 0x83:
  case 0x84:
  case 0x85:
    net_msg_header_init(msg_83,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_83);
    return (int)sent;
  case 0x86:
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)raw_msg);
    if (sent == '\0') {
      return 0;
    }
    return *(i32 *)(raw_msg + 8);
  case 0x87:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x88:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x89:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x8a:
    net_msg_header_init(msg_8a,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_8a);
    return (int)sent;
  case 0x8b:
    net_msg_header_init(msg_8b,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_8b);
    return (int)sent;
  case 0x8c:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x8d:
  case 0x8e:
    net_msg_header_init(msg_8d,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_8d);
    return (int)sent;
  case 0x8f:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0x90:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case 0x91:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x92:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,0,0);
    break;
  case 0x93:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x94:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x95:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x96:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x97:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,arg5,arg6,0
                                        );
    break;
  case 0x98:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,0,0,0,0);
    break;
  case 0x99:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x9a:
  case 0xa0:
  case 0xa1:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0x9b:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,arg3,arg4,0,0,0);
    break;
  case 0x9c:
  case 0x9d:
  case 0x9e:
    net_msg_header_init(msg_9c,msg_type);
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)msg_9c);
    return (int)sent;
  case 0x9f:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0xa2:
    packet = net_msg_build_packet(msg_type,0,arg1,0,0,0,0,0,0);
    break;
  case 0xa4:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0xa5:
  case 0xa6:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
    break;
  case 0xa7:
  case 0xa8:
    packet = net_msg_build_packet(msg_type,0,arg1,arg2,0,0,0,0,0);
  }
  if (packet == (void *)0x0) {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_address,(iptr)&str_d_ss_franklinton_netmgr_send_cpp_4,0x523);
    result = 0;
  }
  else {
    sent = net_session_send_to(&net_session_block,dest,(NetMsg *)packet);
    result = (int)sent;
    operator_delete(packet);
  }
  return result;
}

/* 0046d5a0 auto */
NetMsgHeader * __thiscall net_msg_header_init(NetMsgHeader *self,u32 type)

{
  ((NetMsg *)self)->size = 0x10;
  self->msg_magic = 0x66606660;
  self->msg_type = type;
  return self;
}

/* 0046d5f0 auto */
NetMsgHeader * __thiscall net_msg_init_header(NetNamedMsg *self,u32 msg_type,const char *name2)

{
  net_msg_addr_header_init((NetAddrMsg *)self,0);
  self->inner_type = msg_type;
  _memset(self->name,0,0x20);
  _strncpy(self->name,(char *)name2,0x20);
  _memset(self->address2,0,0x20);
  _strncpy(self->address2,((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),0x20);
  self->size = 0x74;
  return &self->hdr;
}

/* 0046d6a0 auto */
NetMsgHeader * __thiscall net_msg_addr_header_init(NetAddrMsg *self,i32 msg_type)

{
  net_msg_header_init(&self->hdr,msg_type);
  _memset(self->address,0,0x20);
  _strncpy(self->address,((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),0x20);
  self->size = 0x30;
  return &self->hdr;
}

/* 0046d720 auto */
NetMsgHeader * __thiscall net_msg_server_info_init(NetServerInfoMsg *self)

{
  net_msg_init_header(&self->named,2,(const char *)&str_empty_81);
  net_fill_game_info(&self->game);
  self->named.size = 0x198;
  return &self->named.hdr;
}

/* 0046d780 auto */
NetMsgHeader * __thiscall net_msg_player_list_node_init(NetPlayerNodeMsg *self,u32 player_id)

{
  net_msg_header_init(&self->hdr,0x2f);
  self->slot.id = player_id;
  _memset(self->slot.address,0,0x20);
  _strncpy(self->slot.address,((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),0x20);
  crt_strcpy(self->slot.name,((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
  self->slot.field_44 = 0;
  self->slot.lobby_id = 0;
  self->slot.is_host = 0;
  self->slot.active = 0;
  self->slot.civ = 0xffffffff;
  self->size = 100;
  return &self->hdr;
}

/* 0046d860 auto */
NetMsgHeader * __thiscall net_msg_player_list_init(NetPlayerListMsg *self)

{
  net_msg_header_init(&self->hdr,4);
  self->size = 0x280;
  return &self->hdr;
}

/* 0046d8a0 auto */
NetMsgHeader * __thiscall net_msg_server_path_init(NetMsgHeader *self)

{
  net_msg_header_init(self,6);
  ((NetMsg *)self)->size = 0x21c;
  return self;
}

/* 0046d8e0 auto */
NetMsgHeader * __thiscall net_msg_server_answer_init(NetServerAnswerMsg *self)

{
  net_msg_header_init(&self->hdr,0x13);
  net_fill_game_info(&self->game);
  self->size = 0x134;
  return &self->hdr;
}

/* 0046d930 auto */
NetMsgHeader * __thiscall net_msg_whose_turn_init(NetWhoseTurnMsg *self,u32 turn_civ)

{
  net_msg_header_init(&self->hdr,0x69);
  self->civ = turn_civ;
  self->size = 0x14;
  return &self->hdr;
}

/* 0047e0e5 auto */
int __cdecl net_stacked_draw_push(int kind,iptr args,iptr unit_data)

{
  int result;
  i8 dummy_args[28];
  int i;
  
  if ((net_session_block.net_stacked_draw_last == net_session_block.net_stacked_draw_first) && (net_session_block.net_stacked_draw_count != 0)) {
    dbg_log((const char *)&str_stacked_draw_stack_full_flushing);
    if (net_session_block.unit_stack_check_enabled != 0) {
      net_session_block.net_stacked_draw_last = (net_session_block.net_stacked_draw_first + 1) % 100;
    }
    else {
      net_session_block.net_stacked_draw_first = 0;
      net_session_block.net_stacked_draw_last = 0;
    }
    net_session_block.net_stacked_draw_count = (uint)(net_session_block.unit_stack_check_enabled != 0);
    net_stacked_draw_push(0xa3,(iptr)dummy_args,0);
    result = net_stacked_draw_push(0x74,(iptr)dummy_args,0);
  }
  else {
    if ((net_session_block.net_stacked_draw_first < 0) || (99 < net_session_block.net_stacked_draw_first)) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_gnetmgr_firststackeddraw_0_gnetmgr_first,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp,0x5a);
    }
    if ((net_session_block.net_stacked_draw_last < 0) || (99 < net_session_block.net_stacked_draw_last)) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_gnetmgr_laststackeddraw_0_gnetmgr_lastst,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_2,0x5b);
    }
    net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_last].kind = kind;
    for (i = 0; i < 7; i++) {
      *(i32 *)(((i8 *)&net_session_block.net_stacked_draw_args) + i * 4 + net_session_block.net_stacked_draw_last * 0x40) =
           *(i32 *)(args + i * 4);
    }
    if (kind == 0x70) {
      crt_memcpy((void *)(net_session_block.net_stacked_draw_last * 0x40 + (iptr)(((u8 *)&net_session_block.net_stacked_draw_arg4 + 12))),(const void *)unit_data,0x20);
    }
    net_session_block.net_stacked_draw_count++;
    if ((int)net_session_block.net_stacked_draw_peak < (int)net_session_block.net_stacked_draw_count) {
      net_session_block.net_stacked_draw_peak = net_session_block.net_stacked_draw_count;
    }
    result = (net_session_block.net_stacked_draw_last + 1) / 100;
    net_session_block.net_stacked_draw_last = (net_session_block.net_stacked_draw_last + 1) % 100;
  }
  return result;
}

/* 0047e2b3 auto */
int __cdecl net_stacked_draw_run_next(void)

{
  int y;
  iptr visible;
  Tile *tile;
  iptr unit;
  iptr unit2;
  int count;
  int x;
  i32 sum_mode;
  int stack_count;
  
  switch(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].kind) {
  case 0x70:
    x = net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1];
    y = net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[2];
    unit2 = net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[3];
    unit_stack_validate(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0]);
    visible = mapviews_tile_visible(x,y);
    if (visible != 0) {
      crt_memcpy(((short *)&power_graph_history_block.stacked_draw_unit_copy),(const void *)(net_session_block.net_stacked_draw_first * 0x40 + (iptr)(((u8 *)&net_session_block.net_stacked_draw_arg4 + 12))),0x20);
      power_graph_history_block.stacked_draw_unit_prev = -1;
      power_graph_history_block.stacked_draw_unit_next = -1;
      mapviews_ensure_visible(x,y,(int)power_graph_history_block.stacked_draw_unit_owner);
      if (net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[4] == -1) {
        snd_play_effect(99,1,0,0);
      }
      tile = tile_at(x,y);
      sum_mode = 2;
      unit = tile_first_unit(x,y);
      stack_count = unit_stack_sum(unit,sum_mode);
      if ((power_graph_history_block.stacked_draw_unit_copy[0] != x) || (power_graph_history_block.stacked_draw_unit_y != y)) {
        stack_count++;
      }
      if (stack_count == 1) {
        tile->improvements = tile->improvements & 0xfe;
      }
      else if (1 < stack_count) {
        anim_origin_x = (int)power_graph_history_block.stacked_draw_unit_copy[0];
        anim_origin_y = (int)power_graph_history_block.stacked_draw_unit_y;
        count = unit_stack_sum(0x801,5);
        if (count == 0) {
          if (power_graph_history_block.stacked_draw_unit_next < 0) {
            if (power_graph_history_block.stacked_draw_unit_prev < 0) {
              tile->improvements = tile->improvements & 0xfe;
            }
            else {
              anim_origin_stack_unit = (iptr)power_graph_history_block.stacked_draw_unit_prev;
            }
          }
          else {
            anim_origin_stack_unit = (iptr)power_graph_history_block.stacked_draw_unit_next;
          }
        }
        else {
          count = unit_stack_sum(0x801,5);
          if (count < 2) {
            tile->improvements = tile->improvements & 0xfe;
          }
          else {
            anim_origin_stack_unit = unit_stack_first(0x801);
            do {
              if ((anim_origin_stack_unit != 0x801) &&
                 (unit_types[units[anim_origin_stack_unit].type].domain == 2)) goto animate_move;
              anim_origin_stack_unit = (iptr)units[anim_origin_stack_unit].next_in_stack;
            } while (-1 < anim_origin_stack_unit);
            tile->improvements = tile->improvements & 0xfe;
          }
        }
      }
animate_move:
      unit_move_anim_active = 1;
      map_animate_unit_move(0x801,x,y,unit2,net_session_block.net_move_anim_sound,net_session_block.net_move_anim_sound_arg);
      unit_move_anim_active = 0;
      sum_mode = 2;
      unit2 = tile_first_unit(x,y);
      count = unit_stack_sum(unit2,sum_mode);
      if (count != 0) {
        if (net_session_block.net_host_flag == '\0') {
          tile->improvements = tile->improvements | 1;
        }
        else {
          tile_set_improvements(x,y,1,1,1);
        }
      }
    }
    break;
  case 0x71:
    x = tile_visible_to(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                            net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1],human_civ[0]);
    if (x != 0) {
      mapviews_ensure_visible(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                         net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1],
                         net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[2]);
    }
    break;
  case 0x72:
    map_redraw_tile_all_views(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1]);
    break;
  case 0x73:
    map_redraw_combat_tiles(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[2],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[3]);
    break;
  case 0x74:
    map_redraw_all_views(human_civ[0],1);
    break;
  case 0x75:
    map_redraw_neighbors_all_views(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1]);
    break;
  case 0x76:
    map_redraw_area_all_views(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[2],
                       net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[3],1);
    break;
  case 0x7c:
    x = mapviews_tile_visible(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                               net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1]);
    if (x != 0) {
      map_animate_explosion(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                         net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1],
                         net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[2]);
    }
    break;
  case 0x7d:
    x = mapviews_tile_visible(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                               net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1]);
    if (x != 0) {
      map_animate_nuke(net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[0],
                         net_session_block.net_stacked_draw[net_session_block.net_stacked_draw_first].args[1]);
    }
    break;
  case 0xa3:
    net_session_block.nm_abort_kaboom_count++;
  }
  (*(int *)&net_session_block.net_stacked_draw_count)--;
  x = net_session_block.net_stacked_draw_first + 1;
  net_session_block.net_stacked_draw_first = x % 100;
  return x / 100;
}

/* 0047e94e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall net_process_messages(NetSession *self,int max_msgs,int wait_mode)

{
  int *drop_counter;
  u8 *civ_flag;
  char *type_count;
  char civ;
  iptr flush_pending;
  iptr flush_pending2;
  u32 now;
  i32 x;
  iptr ret_val;
  int city;
  bool had_pending;
  uptr civ2;
  uptr unit5;
  int *popup_data;
  int dequeue_wait;
  iptr dblclick_wnd;
  iptr click_wnd;
  i8 home_city_id;
  i8 new_owner;
  i8 draw_args[28];
  RECT caption_rect;
  uint section_id2;
  u8 saved_multiplayer;
  iptr unit2;
  RleStream rle;  /* (the original's locals in a row) */
  i16 saved_moving_unit;
  i8 saved_player_civ;
  int decoded_size;
  int section_idx;
  int units_saved;
  int turn_civ_changed;
  uint section_offset;
  int city_changed;
  u16 saved_msg_options;
  i8 saved_field_1c;
  i8 saved_turn_civ_val;
  uptr section_size;
  i16 saved_active_unit;
  u32 saved_game_options;
  int slot;
  Dialog join_dialog;
  int wait_start;
  Dialog wait_dialog;
  u32 saved_busy;
  int *hello_msg;
  int y4;
  int x3;
  RECT timer_rect;
  int input_kind;
  u32 deferred_input;
  uint input_char;
  uint msg_size;
  int sender;
  iptr unit4;
  int image_kind;
  i32 header_size;
  uint *payload;
  i8 unused_flag;
  /* a message's 7 arguments (net_msg_get_args, net_stacked_draw_push): together, as
     the original's locals were */
  struct { uptr unit; uptr civ3; uptr unit3; iptr y2; iptr x2; int y3; int extra_len; } a;
  int *msg_hdr;
  int *msg;
  char gamelist_dirty;
  int image_size;
  iptr result;
  i32 seh_prev;
  i8 *seh_handler;
  i32 unwind_state;
  
  unwind_state = 0xffffffff;
  gamelist_dirty = '\0';
  unused_flag = 0;
  if ((((((((self->unit_create_busy == 0) && (self->unit_delete_busy == 0)) &&
          (self->city_create_busy == 0)) &&
         ((self->city_delete_busy == 0 && (self->net_lock_map_busy == 0)))) &&
        (self->net_unlock_map_busy == 0)) &&
       (((self->unit_stack_unlink_busy == 0 && (self->unit_stack_link_busy == 0)) &&
        ((self->unit_stack_bottom_busy == 0 &&
         (((self->unit_relocate_busy == 0 && (self->city_acquire_wonder_busy == 0)) && (self->civ_eliminate_busy == 0)
          ))))))) && ((self->civ_create_busy == 0 && (self->civ_check_destroyed_busy == 0)))) &&
     ((self->unit_gather_cargo_busy == 0 &&
      (((self->unit_lift_off_map_busy == 0 && (self->unit_delete_cargo_busy == 0)) &&
       ((self->unit_destroy_redraw_busy == 0 && (flush_pending = XD_InFlushSendBuffer(), flush_pending == 0)))))))) {
    if (((self->blocked_input_kind != 0) && (net_deferred_input_busy == '\0')) &&
       (flush_pending2 = XD_InFlushSendBuffer(), flush_pending2 == 0)) {
      deferred_input = (u32)self->blocked_input_kind;
      net_deferred_input_busy = 1;
      dbg_log((const char *)&str_poll_unstacking_xd_flushsendbuffer_block);
      self->blocked_input_kind = 0;
      switch(deferred_input) {
      case 1:
        if (*(short *)((u32)self->blocked_input_window_i32 + 0x2dc) != 0) {
          if (self->blocked_input_window_i32 == 0) {
            click_wnd = 0;
          }
          else {
            click_wnd = (u32)self->blocked_input_window_i32 + 0x48;
          }
          wnd_set_active_child(click_wnd);
          map_window_click(self->blocked_input_arg1_i32,(u32)self->blocked_input_arg2_i32,
                             self->blocked_input_arg3_i32);
        }
        break;
      case 2:
        if (*(short *)((u32)self->blocked_input_window_i32 + 0x2dc) != 0) {
          if (self->blocked_input_window_i32 == 0) {
            dblclick_wnd = 0;
          }
          else {
            dblclick_wnd = (u32)self->blocked_input_window_i32 + 0x48;
          }
          wnd_set_active_child(dblclick_wnd);
          map_window_double_click(self->blocked_input_arg1_i32,(u32)self->blocked_input_arg2_i32);
        }
        break;
      case 4:
        input_kind = net_session_block.blocked_input_char_pending;
        input_char = (input_char & 0xffffff00 | (byte)(net_session_block.blocked_input_char));
        map_window_key(self->blocked_input_arg1_i32);
        if (input_kind == 3) {
          map_window_ascii(input_char);
        }
        break;
      case 5:
        menu_command(self->blocked_input_arg1_i32,(u32)self->blocked_input_arg2_i32);
        break;
      case 6:
        if (((city_window_block.city_window_closed == 0) && (cities[self->blocked_input_arg1_i32].id != 0)) &&
           ((int)cities[self->blocked_input_arg1_i32].owner == (u32)self->blocked_input_arg2_i32)) {
          city_window_on_mouse(self->blocked_input_arg3_i32,(u32)self->blocked_input_arg4_i32,
                             self->blocked_input_arg5_i32);
        }
        break;
      case 7:
        if (((city_window_block.city_window_closed == 0) && (cities[self->blocked_input_arg1_i32].id != 0)) &&
           ((int)cities[self->blocked_input_arg1_i32].owner == (u32)self->blocked_input_arg2_i32)) {
          city_button_buy(self->blocked_input_arg3_i32);
        }
        break;
      case 8:
        if (((city_window_block.city_window_closed == 0) && (cities[self->blocked_input_arg1_i32].id != 0)) &&
           ((int)cities[self->blocked_input_arg1_i32].owner == (u32)self->blocked_input_arg2_i32)) {
          city_button_change(self->blocked_input_arg3_i32);
        }
        break;
      case 9:
        if (((city_window_block.city_window_closed == 0) && (cities[self->blocked_input_arg1_i32].id != 0)) &&
           ((int)cities[self->blocked_input_arg1_i32].owner == (u32)self->blocked_input_arg2_i32)) {
          city_button_view(self->blocked_input_arg3_i32);
        }
        break;
      case 10:
        if (((city_window_block.city_window_closed == 0) && (cities[self->blocked_input_arg1_i32].id != 0)) &&
           ((int)cities[self->blocked_input_arg1_i32].owner == (u32)self->blocked_input_arg2_i32)) {
          city_button_rename(self->blocked_input_arg3_i32);
        }
      }
      net_deferred_input_busy = '\0';
    }
    self->net_poll_depth = self->net_poll_depth + 1;
    if (self->net_poll_depth_peak < self->net_poll_depth) {
      self->net_poll_depth_peak = self->net_poll_depth;
    }
    while (max_msgs != 0) {
      if (((self->unit_stack_check_enabled == 0) && (self->map_redraw_busy == 0)) &&
         (0 < (int)self->net_stacked_draw_count)) {
        ((u8 *)&self->unit_stack_check_enabled)[0] = 1;
        ((u8 *)&self->unit_stack_check_enabled)[1] = '\0';
        ((u8 *)&self->unit_stack_check_enabled)[2] = '\0';
        ((u8 *)&self->unit_stack_check_enabled)[3] = '\0';
        if ((int)self->net_stacked_draw_count < 0x22) {
          unit4 = 1;
        }
        else {
          unit4 = 2;
        }
        for (; 0 < unit4; unit4--) {
          net_stacked_draw_run_next();
        }
        ((u8 *)&self->unit_stack_check_enabled)[0] = '\0';
        ((u8 *)&self->unit_stack_check_enabled)[1] = '\0';
        ((u8 *)&self->unit_stack_check_enabled)[2] = '\0';
        ((u8 *)&self->unit_stack_check_enabled)[3] = '\0';
      }
      if ((net_session_block.net_msgq_normal_count < 0x12d) && (net_session_block.net_msgq_priority_count < 0x22)) {
        ui_pump_messages();
        max_msgs--;
      }
      if (net_session_block.net_msgq_priority_count < 0x22) {
        dequeue_wait = wait_mode;
      }
      else {
        dequeue_wait = 1;
      }
      civ = net_msgq_dequeue
                        (&self->msg_queue,(iptr)&sender,(iptr)&msg,
                         (iptr)&msg_size,dequeue_wait);
      if (civ == '\0') break;
      if (msg == (int *)0x0) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_pdata_null,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_3,0x195);
      }
      msg_hdr = msg;
      if (*msg == 0x66606660) {
        net_session_block.nm_recv_counts_by_type[msg[1]] = net_session_block.nm_recv_counts_by_type[msg[1]] + 1;
        if ((0x35 < msg[1]) && (civ_saved_cursor_x_block.game_setup_complete == 0)) {
          operator_delete(msg);
          break;
        }
        payload = (uint *)(msg + 0xb);
        switch(msg[1]) {
        case 0:
          hello_msg = msg;
          net_session_block.nm_recv_counts_by_type[msg[0x1c]] =
               net_session_block.nm_recv_counts_by_type[msg[0x1c]] + 1;
          if (msg[0x1c] == 1) {
            if (((self->net_host_flag != '\0') && (net_session_block.net_host_handoff_stage == '\0')) &&
               (net_session_block.net_connection_type != 0)) {
              net_send_message(2,0,0,0,0,0,0,0,0,0);
            }
          }
          else if (msg[0x1c] == 2) {
            net_game_list_add(self,(NetServerInfoMsg *)msg);
            gamelist_dirty = 1;
          }
          break;
        default:
          dbg_log((const char *)&str_received_unknown_message_type);
          break;
        case 4:
          crt_memcpy((void *)self->net_player_count,msg + 4,0x270);
          for (unit4 = 0; unit4 < 7; unit4++) {
            if ((self->net_slots[unit4].active != '\0') &&
               (self->net_slots[unit4].id == self->net_server_conn_id)) {
              self->net_local_slot = unit4;
              break;
            }
          }
          break;
        case 5:
          now = timer_ticks();
          self->net_wait_start_ticks_i32 = now;
          break;
        case 6:
          net_session_block.loaded_file_kind = (i16)msg[4];
          crt_strcpy(((i8 *)&net_session_block.loaded_file_name),(const char *)((const void *)((int)msg + 0x12)));
          crt_strcpy(((i8 *)&net_session_block.loaded_file_subdir),(const char *)((const void *)((int)msg_hdr + 0x116)));
          break;
        case 7:
        case 8:
        case 9:
          header_size = 0x2c;
          image_kind = msg[4];
          image_size = msg_size - 0x2c;
          crt_memmove(msg,msg + 0xb,image_size);
          msg = (int *)crt_realloc(msg,image_size);
          diffeng_apply_image((int *)(&msg),image_kind);
          break;
        case 10:
          header_size = 0x2c;
          image_kind = msg[4];
          image_size = msg_size - 0x2c;
          crt_memmove(msg,msg + 0xb,image_size);
          msg = (int *)crt_realloc(msg,image_size);
          diffeng_apply_map_header_image((int *)(&msg),image_kind);
          break;
        case 0xb:
        case 0xc:
        case 0x11:
          break;
        case 0xd:
          break;
        case 0xe:
          self->net_host_handoff_stage++;
          break;
        case 0xf:
          self->net_host_handoff_stage++;
          break;
        case 0x10:
          net_session_block.net_players_awaited--;
          break;
        case 0x12:
          net_send_message(0x13,sender,0,0,0,0,0,0,0,0);
          break;
        case 0x13:
          if (net_session_block.gamelist_view != 0) {
            crt_strcpy(net_session_block.gamelist_view->server_address,(const char *)(msg + 4));
            crt_strcpy(net_session_block.gamelist_view->server_game_name,(const char *)(msg_hdr + 0x20));
            crt_strcpy(net_session_block.gamelist_view->server_info_c8,(const char *)(msg_hdr + 0x28));
            crt_strcpy(net_session_block.gamelist_view->server_version,(const char *)(msg_hdr + 0x10));
          }
          crt_strcpy(((i8 *)&net_session_block.net_server_address),(const char *)(msg_hdr + 4));
          crt_strcpy(((i8 *)&net_session_block.net_game_name),(const char *)(msg_hdr + 0x20));
          crt_strcpy(((i8 *)&net_session_block.net_local_player_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
          crt_strcpy(((i8 *)&net_session_block.net_server_version),(const char *)(msg_hdr + 0x10));
          break;
        case 0x14:
          break;
        case 0x15:
        case 0x5c:
          city_changed = 0;
          units_saved = 0;
          turn_civ_changed = 0;
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if (diffeng_mirror_buf != 0) {
            for (section_idx = 0; section_idx < (int)a.unit; section_idx++) {
              section_id2 = *payload;
              section_offset = payload[1];
              section_size = payload[2];
              payload += 3;
              if ((section_id2 & 0x7fff) == 1) {
                saved_active_unit = game.active_unit;
                saved_moving_unit = game.moving_unit;
                saved_multiplayer = game.multiplayer;
                saved_player_civ = game.player_civ;
                saved_field_1c = game.saved_current_civ;
                saved_turn_civ_val = game.turn_civ;
                saved_game_options = game.game_options;
                saved_msg_options = game.message_options;
              }
              else if (((net_session_block.net_host_flag == '\0') || ((section_id2 & 0x7fff) != 5)) || (units_saved != 0))
              {
                if ((section_id2 & 0x7fff) == 0xb) {
                  turn_civ_changed = 1;
                }
              }
              else {
                units_saved = 1;
                for (unit2 = 0; unit2 < game.num_units; unit2++) {
                  map_views_block.net_saved_unit_x[unit2] = units[unit2].x;
                  map_views_block.net_saved_unit_y[unit2] = units[unit2].y;
                  map_views_block.net_saved_unit_next[unit2] = units[unit2].next_in_stack;
                  map_views_block.net_saved_unit_prev[unit2] = units[unit2].prev_in_stack;
                }
              }
              if ((section_id2 & 0x8000) == 0) {
                crt_memcpy((void *)((iptr)diffeng_section_table[section_id2].data_ptr + section_offset),payload,section_size);
                crt_memcpy((void *)((int)diffeng_section_table[section_id2].mirror_offset + section_offset + diffeng_mirror_buf),
                             payload,section_size);
              }
              else {
                section_id2 = section_id2 & 0x7fff;
                rle.src_end = (u8 *)payload + section_size;
                rle.src = (u8 *)payload;
                decoded_size = diffeng_rle_decoded_size(&rle);
                rle.dst = (u8 *)diffeng_section_table[section_id2].data_ptr + section_offset;
                rle.dst_end = rle.dst + decoded_size;
                diffeng_rle_decode(&rle);
                crt_memcpy((void *)((int)diffeng_section_table[section_id2].mirror_offset + section_offset + diffeng_mirror_buf),
                             (const void *)((iptr)diffeng_section_table[section_id2].data_ptr + section_offset),decoded_size);
              }
              if (section_id2 == 1) {
                game.active_unit = saved_active_unit;
                game.moving_unit = saved_moving_unit;
                game.multiplayer = saved_multiplayer;
                game.player_civ = saved_player_civ;
                game.saved_current_civ = saved_field_1c;
                game.turn_civ = saved_turn_civ_val;
                game.game_options = saved_game_options;
                game.message_options = saved_msg_options;
              }
              if (section_id2 == 6) {
                city_changed = 1;
              }
              payload = (uint *)((int)payload + section_size);
            }
            if (city_changed != 0) {
              city_window_refresh((CityWindow *)((i8 *)&city_window_block.city_window));
            }
            if ((net_session_block.net_host_flag != '\0') && (units_saved != 0)) {
              for (unit2 = 0; unit2 < game.num_units; unit2++) {
                units[unit2].x = map_views_block.net_saved_unit_x[unit2];
                units[unit2].y = map_views_block.net_saved_unit_y[unit2];
                units[unit2].next_in_stack = map_views_block.net_saved_unit_next[unit2];
                units[unit2].prev_in_stack = map_views_block.net_saved_unit_prev[unit2];
              }
            }
            if (turn_civ_changed != 0) {
              turn_resume_civ = civ_saved_cursor_x_block.saved_turn_civ;
            }
          }
          break;
        case 0x16:
          break;
        case 0x17:
          crt_memcpy(&cosmic,payload,0x29);
          break;
        case 0x18:
          crt_memcpy(advances,payload,0x640);
          break;
        case 0x19:
          crt_memcpy(improvements,payload,0x218);
          break;
        case 0x1a:
          crt_memcpy(wonder_expires,payload,0x1c);
          break;
        case 0x1b:
          crt_memcpy(unit_types,payload,0x4d8);
          break;
        case 0x1c:
          crt_memcpy(terrain_types,payload,0x318);
          break;
        case 0x1d:
          crt_memcpy(government_names,payload,0x1c);
          break;
        case 0x1e:
          crt_memcpy(government_titles,payload,0x38);
          break;
        case 0x1f:
          crt_memcpy(leaders,payload,0x3f0);
          break;
        case 0x20:
          crt_memcpy(civs,payload,0x7524);
          break;
        case 0x21:
          crt_memcpy(commodity_names,payload,0x40);
          break;
        case 0x22:
          crt_memcpy(order_names,payload,0x68);
          break;
        case 0x23:
          crt_memcpy(difficulty_names,payload,0x18);
          break;
        case 0x24:
          crt_memcpy(attitude_names,payload,0x24);
          break;
        case 0x25:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&scenario_events_loaded) = a.unit;
          (*(uptr *)&unit_types_block.scenario_event_count) = a.civ3;
          if (a.unit != 0) {
            events_relink_pool();
          }
          break;
        case 0x26:
          break;
        case 0x27:
          break;
        case 0x28:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&net_session_block.net_session_phase) = a.unit;
          break;
        case 0x2a:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (ushort)a.unit;
          game.human_players = game.human_players & (byte)a.unit;
          net_session_block.nm_lock_nation_count_i32++;
          break;
        case 0x2b:
          net_send_message(0x2c,sender,0,0,0,0,0,0,0,0);
          break;
        case 0x2c:
          break;
        case 0x2d:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.net_civ_drop_requested[a.unit] = 1;
          net_drop_flagged_players();
          break;
        case 0x2e:
          if (net_session_block.net_host_flag == '\0') {
            err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_gnetmgr_bserver,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_4,0x22a);
          }
          if (net_session_block.net_host_handoff_stage == '\0') {
            city = net_host_register_player_civ((NetPlayerInfoMsg *)msg_hdr,sender);
            if (city == 0) {
              net_send_message(0xc,sender,0,0,0,0,0,0,0,0);
            }
            else {
              net_session_block.nm_lock_nation_count_i32++;
              if (2 < (int)(*(uptr *)&net_session_block.net_session_phase)) {
                dialog_ctor(&join_dialog,0x4000);
                unwind_state = 1;
                net_send_message(0x65,0xff,0,0,0,0,0,0,0,0);
                mp_turn_timer_stop(0);
                dlg_load_game_section(&join_dialog,(const char *)&str_waitongamexmit_2,0x2000000);
                dlg_show_modal(&join_dialog,0);
                net_send_message(0x33,sender,(int)civ_saved_cursor_x_block.mp_double_production,(int)civ_saved_cursor_x_block.mp_double_moves,
                                   (int)civ_saved_cursor_x_block.mp_simultaneous_setting,0,0,0,0,0);
                net_send_message(4,0xff,0,0,0,0,0,0,0,0);
                XD_FlushSendBuffer(5000);
                net_send_state_diff(0xff,2,0,0,0);
                XD_FlushSendBuffer(5000);
                net_send_message(10,sender,0,0,0,0,0,0,0,0);
                net_send_state_diff(sender,1,0,0,0);
                XD_FlushSendBuffer(5000);
                net_send_message(0x25,sender,(*(uptr *)&scenario_events_loaded),(*(uptr *)&unit_types_block.scenario_event_count),0,0,0,0,0,0);
                net_send_rules_tables(sender);
                XD_FlushSendBuffer(5000);
                net_send_message(0x28,sender,(*(uptr *)&net_session_block.net_session_phase),0,0,0,0,0,0,0);
                net_send_message(0x16,sender,0,0,0,0,0,0,0,0);
                net_send_message(0x9c,0xff,0,0,0,0,0,0,0,0);
                wait_start = timer_ticks();
                do {
                  city = XD_FlushSendBuffer(5000);
                  if (city == 0) break;
                  net_process_messages(&net_session_block,100,0);
                  city = timer_ticks();
                } while (17999 < city - wait_start);
                net_send_message(0x66,0xff,0,0,0,0,0,0,0,0);
                dialog_close(&join_dialog);
                mp_turn_timer_resume(0);
                for (slot = 0;
                    (slot < 7 && (net_session_block.net_slots[slot].id != sender));
                    slot = slot + 1) {
                }
                if (slot < 7) {
                  civ_flag = (u8 *)self->net_civ_new_player_flags + (((i32 *)&net_session_block.net_slot_civs))[slot * 0x15] * 4;
                  civ_flag[0] = 1;
                  civ_flag[1] = '\0';
                  civ_flag[2] = '\0';
                  civ_flag[3] = '\0';
                }
                unwind_state = 0xffffffff;
                /* inlined unwind funclet 00482305 */ { dialog_dtor(&join_dialog); }
              }
            }
          }
          else {
            net_send_message(0xb,sender,0,0,0,0,0,0,0,0);
          }
          break;
        case 0x2f:
          if (net_session_block.net_host_handoff_stage == '\0') {
            if (net_session_block.net_player_count[0] < net_session_block.net_max_players) {
              net_add_player(self,(NetPlayerNodeMsg *)msg);
              if (((*(int * *)&net_session_block.net_lobby_list) == (int *)0x0) || (*(*(int * *)&net_session_block.net_lobby_list) == 0)) {
                if (0 < (int)(*(uptr *)&net_session_block.net_session_phase)) {
                  net_send_message(0x28,sender,(*(uptr *)&net_session_block.net_session_phase),0,0,0,0,0,0,0);
                  net_send_message(4,0xff,0,0,0,0,0,0,0,0);
                  net_send_message(6,sender,0,0,0,0,0,0,0,0);
                  net_send_message(9,sender,0,0,0,0,0,0,0,0);
                }
              }
              else {
                lobby_refresh_player_list();
              }
            }
            else {
              net_send_message(0x11,sender,0,0,0,0,0,0,0,0);
            }
          }
          else {
            net_send_message(0xb,sender,0,0,0,0,0,0,0,0);
          }
          break;
        case 0x30:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if (((int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & 1 << ((byte)a.civ3 & 0x1f)) == 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (ushort)(1 << ((byte)a.civ3 & 0x1f)) | (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
            net_send_message(0x26,sender,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net_send_message(0x2a,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net_session_block.nm_lock_nation_count_i32++;
          }
          else {
            net_send_message(0x27,sender,0,0,0,0,0,0,0,0);
          }
          break;
        case 0x31:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
          if (((int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & 1 << ((byte)a.civ3 & 0x1f)) != 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = ~(ushort)(1 << ((byte)a.civ3 & 0x1f)) & (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
            game.human_players = game.human_players & (byte)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask);
            net_send_message(0x2a,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net_session_block.nm_lock_nation_count_i32++;
          }
          break;
        case 0x32:
          net_session_block.net_lost_conn_by_player[sender] = 0xffffffff;
          net_remove_player(self,sender);
          if ((self->net_lobby_list != 0) &&
             (drop_counter = (int *)&self->net_lobby_list->drop_counts[sender], *drop_counter = *drop_counter + 1,
             *(*(int * *)&net_session_block.net_lobby_list) != 0)) {
            lobby_refresh_player_list();
          }
          break;
        case 0x33:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          civ_saved_cursor_x_block.mp_double_production = (short)a.unit;
          civ_saved_cursor_x_block.mp_double_moves = (short)a.civ3;
          net_session_block.mp_simultaneous_moves = (char)a.unit3;
          civ_saved_cursor_x_block.mp_simultaneous_setting = (short)(char)a.unit3;
          break;
        case 0x34:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          civ_eliminate(a.unit,0,0);
          net_send_message(0x35,sender,0,0,0,0,0,0,0,0);
          break;
        case 0x35:
          net_session_block.net_kill_or_quit_ack = 1;
          break;
        case 0x37:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if ((-1 < (int)a.unit) && (units[a.unit].id != 0)) {
            x3 = (int)units[a.unit].x;
            y4 = (int)units[a.unit].y;
          }
          unit_delete(a.unit,1);
          net_send_message(0x38,sender,a.unit,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x38:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.net_unit_deleted_ack = 1;
          break;
        case 0x39:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city_delete(a.unit,0);
          net_send_message(0x3a,sender,a.unit,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x3a:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.net_city_deleted_ack = 1;
          crt_memcpy(cities + a.unit,payload,0x58);
          break;
        case 0x3b:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          result = city_create(a.unit,a.civ3,a.unit3);
          net_send_message(0x3c,sender,result,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x3c:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.nm_city_created_result = a.unit;
          if (a.unit != 0xffffffff) {
            crt_memcpy(cities + a.unit,payload,0x58);
            game.num_cities = (i16)a.civ3;
          }
          break;
        case 0x3d:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          result = unit_create(a.unit,a.civ3,a.unit3,a.y2);
          net_send_message(0x3e,sender,result,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x3e:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.nm_unit_created_result = a.unit;
          if (a.unit != 0xffffffff) {
            game.num_units = (i16)a.civ3;
          }
          break;
        case 0x3f:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_stack_unlink(a.unit,1);
          net_send_message(0x40,sender,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x40:
          net_session_block.nm_unit_picked_up_count = 1;
          break;
        case 0x41:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_stack_link(a.unit,a.civ3,a.unit3,1);
          net_send_message(0x42,sender,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x42:
          net_session_block.nm_unit_put_down_count = 1;
          break;
        case 0x43:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_move_to_stack_bottom(a.unit,1);
          net_send_message(0x44,sender,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x44:
          net_session_block.nm_bottomed_count = 1;
          break;
        case 0x45:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_relocate(a.unit,a.civ3,a.unit3,1);
          net_send_message(0x46,sender,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x46:
          net_session_block.nm_unit_relocated_count = 1;
          break;
        case 0x47:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          result = city_acquire_wonder(a.unit,a.civ3,a.unit3,a.y2,a.x2);
          net_send_message(0x48,sender,result,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(5000);
          break;
        case 0x48:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&net_session_block.nm_wonder_acquired_result) = a.unit;
          break;
        case 0x49:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          ret_val = unit_gather_cargo(a.unit,a.civ3,1);
          net_send_message(0x4a,sender,ret_val,0,0,0,0,0,0,0);
          break;
        case 0x4a:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.nm_ship_stacked_result = a.unit;
          break;
        case 0x4b:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_lift_off_map(a.unit,1);
          net_send_message(0x4c,sender,0,0,0,0,0,0,0,0);
          break;
        case 0x4c:
          net_session_block.nm_unit_stacked_count = 1;
          break;
        case 0x4d:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_delete_with_cargo(a.unit,1);
          net_send_message(0x4e,sender,0,0,0,0,0,0,0,0);
          break;
        case 0x4e:
          net_session_block.nm_safely_deleted_count = 1;
          break;
        case 0x4f:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_destroy_redraw(a.unit,1);
          net_send_message(0x50,sender,0,0,0,0,0,0,0,0);
          break;
        case 0x50:
          net_session_block.nm_visibly_deleted_count = 1;
          break;
        case 0x51:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_lock_map(a.unit,a.civ3,a.unit3,a.y2,a.x2,0);
          break;
        case 0x52:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_unlock_map(a.unit,0);
          net_send_message(0x54,sender,a.unit,0,0,0,0,0,0,0);
          break;
        case 0x53:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_map_lock_slots_block.net_map_locks[a.unit].replies = a.civ3;
          break;
        case 0x54:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_map_lock_slots_block.net_map_locks[a.unit].state = net_map_lock_slots_block.net_map_locks[a.unit].state + 1;
          break;
        case 0x55:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_map_lock_slots_block.net_map_locks[a.unit].src_x = a.civ3;
          net_map_lock_slots_block.net_map_locks[a.unit].src_y = a.unit3;
          net_map_lock_slots_block.net_map_locks[a.unit].dst_x = a.y2;
          net_map_lock_slots_block.net_map_locks[a.unit].dst_y = a.x2;
          net_map_lock_slots_block.net_map_locks[a.unit].replies = 0;
          net_map_lock_slots_block.net_map_locks[a.unit].state =
               net_map_lock_slots_block.net_map_locks[a.unit].replies;
          break;
        case 0x56:
        case 0x57:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.net_vote_replies_pending--;
          if (a.unit == 2) {
            net_session_block.net_vote_status = 0xffffffff;
          }
          break;
        case 0x58:
          wonder_apollo_reveal_map();
          break;
        case 0x59:
          net_apply_tile_updates((iptr)payload);
          break;
        case 0x5a:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          unit_lift_off_map(a.unit3,0);
          unit_stack_clear_visibility(a.unit3);
          if (a.y2 == 0) {
            unit_stack_set_seen_by(a.unit3,a.civ3);
          }
          unit_stack_relocate_all(a.unit3,a.x2,a.y3,0);
          if ((-1 < units[a.unit3].next_in_stack) &&
             (unit_types[units[a.unit3].type].attack == '\0')) {
            unit_move_to_stack_bottom(a.unit3,0);
          }
          if (net_session_block.mp_simultaneous_moves != '\0') {
            net_unlock_map(a.unit,0);
          }
          net_send_state_diff(0xff,2,0,0,1);
          net_send_message(0x5b,sender,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(60000);
          break;
        case 0x5b:
          net_session_block.nm_stacker_ack_count = 1;
          break;
        case 0x5d:
          net_session_block.net_drag_hold = 1;
          dbg_log((const char *)&str_hold_on);
          break;
        case 0x5e:
          net_session_block.net_drag_hold = 0;
          dbg_log((const char *)&str_hold_off);
          break;
        case 0x5f:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          result = civ_create(a.unit);
          net_send_message(0x61,sender,result,0,0,0,0,0,0,0);
          break;
        case 0x60:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          result = civ_check_destroyed(a.unit,a.civ3);
          net_send_message(0x62,sender,result,0,0,0,0,0,0,0);
          break;
        case 0x61:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&net_session_block.nm_new_civ_ack_result) = a.unit;
          break;
        case 0x62:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&net_session_block.nm_kill_civ_ack_result) = a.unit;
          break;
        case 99:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          civ2 = a.civ3;
          unit5 = a.unit;
          civ = units[a.unit].owner;
          civs[a.civ3].gold -= a.unit3;
          type_count = (char *)(units[a.unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
          *type_count = *type_count - 1;
          type_count = (char *)(units[a.unit].type + (iptr)((u8 *)&civs + 216) + a.civ3 * 0x594);
          *type_count = *type_count + 1;
          civs[a.civ3].subversion_count++;
          new_owner = (i8)a.civ3;
          units[a.unit].owner = new_owner;
          units[a.unit].home_city = -1;
          units[a.unit].moves_used = '\0';
          units[a.unit].orders = -1;
          city = city_find_nearest((int)units[a.unit].x,(int)units[a.unit].y,-1,0xffffffff,-1)
          ;
          if ((-1 < city) && ((int)cities[city].owner == civ2)) {
            home_city_id = (i8)city;
            units[unit5].home_city = home_city_id;
          }
          unit_set_seen_by(unit5,(int)civ);
          tile_set_owner((int)units[unit5].x,(int)units[unit5].y,civ2,1);
          net_send_state_diff(0xff,2,0,0,1);
          break;
        case 100:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          casualty_log_add(a.unit,a.civ3,a.unit3,a.y2,a.x2);
          break;
        case 0x65:
          dialog_ctor(&wait_dialog,0x4000);
          unwind_state = 0;
          saved_busy = (u32)self->net_popup_in_progress_i32;
          self->net_popup_in_progress_i32 = 1;
          mp_turn_timer_stop(0);
          net_session_block.nm_continue_after_join_count = 0;
          dlg_pending_idle_callback = net_game_xmit_wait_idle;
          popup_game_sprite_flags((const char *)&str_waitongamexmit,0x2000000,(iptr)&popup_info_icon_sprite,0);
          dialog_close(&wait_dialog);
          self->net_popup_in_progress_i32 = saved_busy;
          mp_turn_timer_resume(0);
          unwind_state = 0xffffffff;
          /* inlined unwind funclet 00482311 */ { dialog_dtor(&wait_dialog); }
          break;
        case 0x66:
          break;
        case 0x68:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          header_size = 0x2c;
          chat_receive_message
                    (&chat_wnd_block,a.unit,a.civ3,a.unit3,(iptr)(msg + 0xb)
                    );
          break;
        case 0x69:
          had_pending = self->unit_stack_check_enabled != 0;
          if (had_pending) {
            net_session_block.net_stacked_draw_last = (net_session_block.net_stacked_draw_first + 1) % 100;
          }
          else {
            net_session_block.net_stacked_draw_first = 0;
            net_session_block.net_stacked_draw_last = 0;
          }
          net_session_block.net_stacked_draw_count = (uint)had_pending;
          net_stacked_draw_push(0xa3,(iptr)draw_args,0);
          net_stacked_draw_push(0x74,(iptr)draw_args,0);
          game.turn_civ = (i8)msg_hdr[4];
          net_session_block.mp_turn_civ = (int)game.turn_civ;
          mp_turn_timer_stop(1);
          menu_update_states();
          map_views_block.main_view_timer_cache_i32 = 0xffffffff;
          frame_wnd_draw_caption((FrameWindow *)((i8 *)&map_views_block.map_views));
          caption_rect.left = map_views_block.main_view_frame_left;
          caption_rect.top = map_views_block.main_view_frame_top;
          caption_rect.right = map_views_block.main_view_frame_right;
          caption_rect.bottom = map_views_block.main_view_client_top;
          wnd_blit_surface_rect((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&caption_rect);
          break;
        case 0x6a:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          header_size = 0x2c;
          if ((a.unit == 0x3d) &&
             (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)game.human_players) == 0 ||
              (mp_meeting_pending != 0)))) {
            net_send_message(0x80,sender,2,0,0,0,0,0,0,0);
          }
          else {
            if (a.unit == 0x3d) {
              mp_meeting_pending = 1;
            }
            if (((a.unit != 100) || (parley_reveal_units_prompt_open != 1)) &&
               ((a.unit != 0x65 || (parley_reveal_city_prompt_open != 1)))) {
              if (a.extra_len == 0) {
                popup_data = (int *)0x0;
              }
              else {
                popup_data = msg + 0xb;
              }
              net_popup_queue_push(a.unit,a.civ3,a.unit3,a.y2,a.x2,(i32)popup_data,a.extra_len);
            }
          }
          break;
        case 0x6b:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          game.game_flags = (u16)a.unit;
          (*(uptr *)&game_over_reason) = a.civ3;
          (*(uptr *)&year_table_start_years_block.spaceship_winner_civ) = a.unit3;
          break;
        case 0x6c:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&civ_saved_cursor_x_block.mp_turn_time_limit_ms) = a.unit;
          if (a.unit == 0) {
            map_views_block.main_view_timer_shown[0] = 0;
            frame_wnd_draw_caption((FrameWindow *)((i8 *)&map_views_block.map_views));
            timer_rect.left = map_views_block.main_view_frame_left;
            timer_rect.top = map_views_block.main_view_frame_top;
            timer_rect.right = map_views_block.main_view_frame_right;
            timer_rect.bottom = map_views_block.main_view_client_top;
            wnd_blit_surface_rect((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&timer_rect);
          }
          else {
            map_views_block.main_view_timer_shown[0] = 1;
            mp_turn_timer_start(1);
          }
          break;
        case 0x6d:
          if ((*(uptr *)&civ_saved_cursor_x_block.mp_turn_time_limit_ms) != 0) {
            map_views_block.main_view_timer_shown[0] = 1;
          }
          mp_turn_timer_start(0);
          break;
        case 0x6e:
          mp_turn_timer_stop(0);
          break;
        case 0x6f:
          mp_turn_timer_resume(0);
          break;
        case 0x70:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if (a.y3 != 0) {
            x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[a.y2] + a.civ3);
            city = city_at(x,(int)(((i8 *)&dir_dy_table_block.dir_dy_table))[a.y2] + a.unit3);
            if ((city < 0) && ((game.game_options & 0x1000) == 0)) {
              msg_hdr[1] = 0x72;
              a.unit = a.civ3;
              a.civ3 = a.unit3;
            }
          }
          net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,(iptr)payload);
          break;
        case 0x71:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
          break;
        case 0x72:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city = mapviews_tile_visible(a.unit,a.civ3);
          if (city != 0) {
            net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
          }
          break;
        case 0x73:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city = mapviews_tile_visible(a.unit,a.civ3);
          if ((city != 0) || (city = mapviews_tile_visible(a.unit3,a.y2), city != 0)) {
            net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
          }
          break;
        case 0x74:
          net_stacked_draw_push(msg[1],(iptr)&a.unit,0);
          break;
        case 0x75:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city = mapviews_tile_visible(a.unit,a.civ3);
          if (city != 0) {
            net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
          }
          break;
        case 0x76:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city = mapviews_tile_visible(a.unit,a.civ3);
          if (city != 0) {
            net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
          }
          break;
        case 0x77:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&sprite_picker_sprite_block.net_can_escape_reply) = a.unit;
          break;
        case 0x78:
          status_draw_info(1);
          break;
        case 0x79:
          status_redraw(1);
          break;
        case 0x7a:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          snd_play_effect(a.unit,a.civ3,0,0);
          break;
        case 0x7b:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          font_size16_bold_block.net_bribe_answer = a.unit;
          break;
        case 0x7c:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
          break;
        case 0x7d:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_stacked_draw_push(msg_hdr[1],(iptr)&a.unit,0);
          break;
        case 0x7e:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          diplo_ally_demand_help(a.unit,a.civ3);
          break;
        case 0x7f:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          advisor_refresh_if_open(a.unit);
          break;
        case 0x80:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&advisor_window_block.mp_meeting_reply) = a.unit;
          break;
        case 0x81:
          if ((((((i32 *)&net_session_block.net_slot_civs))[sender * 0x15] == parley_wnd_block.other_civ) || (parley_wnd_block.other_civ == 0xfffffffe))
             || (parley_wnd_block.closed == 0)) {
            if (parley_wnd_block.closed == 0) {
              parley_wnd_close();
            }
            else if (parley_wnd_block.other_civ == 0xfffffffe) {
              parley_wnd_block.other_civ = 0xffffffff;
            }
            mp_meeting_pending = 0;
          }
          break;
        case 0x82:
          if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
            operator_delete((void *)parley_wnd_block.received_offer);
            parley_wnd_block.received_offer = 0;
          }
          parley_wnd_block.received_offer = (ParleyOfferMsg *)operator_new(msg_size);
          if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
            crt_memcpy((void *)parley_wnd_block.received_offer,msg,msg_size);
          }
          parley_poll_net_replies();
          break;
        case 0x83:
          parley_poll_net_replies();
          break;
        case 0x84:
          parley_poll_net_replies();
          break;
        case 0x85:
          parley_poll_net_replies();
          break;
        case 0x86:
          parley_execute_transaction((ParleyOfferMsg *)msg);
          break;
        case 0x87:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city_window_on_unit_change
                    ((CityWindow *)((i8 *)&city_window_block.city_window),a.unit,a.civ3,a.unit3);
          break;
        case 0x88:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city_window_refresh_if_city((CityWindow *)((i8 *)&city_window_block.city_window),a.unit);
          break;
        case 0x89:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          city_window_close_if_city((CityWindow *)((i8 *)&city_window_block.city_window),a.unit);
          break;
        case 0x8a:
          if (city_window_block.city_window_refresh_locked == 0) {
            city_window_refresh((CityWindow *)((i8 *)&city_window_block.city_window));
          }
          break;
        case 0x8b:
          break;
        case 0x8c:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          dialog_text_font_obj_block.net_civ_phase_done[a.unit] = dialog_text_font_obj_block.net_civ_phase_done[a.unit] + 1;
          break;
        case 0x8d:
          break;
        case 0x8e:
          break;
        case 0x8f:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          dialog_text_font_obj_block.net_civ_phase_done[a.unit] = dialog_text_font_obj_block.net_civ_phase_done[a.unit] + 1;
          for (unit4 = 0; unit4 < game.num_units; unit4++) {
            if ((units[unit4].id != 0) && ((int)units[unit4].owner == a.unit)) {
              unit_exhaust_moves(unit4);
            }
          }
          break;
        case 0x90:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          tile_set_improvements(a.unit,a.civ3,a.unit3,a.y2,a.x2);
          break;
        case 0x91:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          tile_set_terrain(a.unit,a.civ3,a.unit3,a.y2);
          break;
        case 0x92:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          tile_set_visible(a.unit,a.civ3,a.unit3,a.y2,a.x2);
          break;
        case 0x93:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          tile_set_fertility(a.unit,a.civ3,a.unit3,a.y2);
          break;
        case 0x94:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          tile_set_owner(a.unit,a.civ3,a.unit3,a.y2);
          break;
        case 0x95:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          tile_set_landmass(a.unit,a.civ3,a.unit3,a.y2);
          break;
        case 0x96:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          tile_set_radius_civ(a.unit,a.civ3,a.unit3,a.y2);
          break;
        case 0x97:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          known_set(a.unit,a.civ3,a.unit3,a.y2,a.x2,a.y3);
          break;
        case 0x98:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          *(char *)&civs[a.unit].attitude[a.civ3] = (char)a.unit3;
          break;
        case 0x99:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_meet_pending_flags[a.civ3] = 1;
          map_views_block.net_meet_pending_ai_civ[a.civ3] = a.unit;
          map_views_block.net_meet_pending_civ[a.civ3] = a.civ3;
          map_views_block.net_meet_pending_x[a.civ3] = a.unit3;
          map_views_block.net_meet_pending_y[a.civ3] = a.y2;
          break;
        case 0x9a:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          (*(uptr *)&net_session_block.net_move_anim_sound) = a.unit;
          (*(uptr *)&net_session_block.net_move_anim_sound_arg) = a.civ3;
          break;
        case 0x9b:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          civ_give_advance(a.unit,a.civ3,a.unit3,a.y2,0);
          break;
        case 0x9c:
          break;
        case 0x9d:
          taxrate_dialog_show((*(i8 *)&human_civ));
          break;
        case 0x9e:
          taxrate_enforce_max_rate((*(i8 *)&human_civ));
          break;
        case 0x9f:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          civ_select_government_dialog((*(i8 *)&human_civ),a.unit);
          break;
        case 0xa0:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          diplo_ai_review_relations(a.unit,a.civ3);
          break;
        case 0xa1:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          diplo_ai_seek_war_ally(a.unit,a.civ3);
          break;
        case 0xa2:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          net_session_block.net_civ_turn_ended[a.unit] = net_session_block.net_civ_turn_ended[a.unit] + 1;
          break;
        case 0xa3:
          net_stacked_draw_push(msg[1],(iptr)&a.unit,0);
          break;
        case 0xa4:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          ret_val = (iptr)(civ_name_plural(a.unit));
          text_set_string_arg(0,(const char *)ret_val);
          ret_val = (iptr)(civ_name_plural(a.civ3));
          text_set_string_arg(1,(const char *)ret_val);
          if ((*(byte *)&civs[a.civ3].treaties[a.unit] & 8) == 0) {
            city = popup_game_with_sprite((const char *)&str_pretext,(iptr)&war_icon_sprite_block.war_icon_sprite,0);
            if (city == 1) {
              diplo_set_treaty_flags(a.unit,a.civ3,0x2000);
              civs[a.civ3].last_contact_turn[a.unit] = game.turn;
            }
          }
          else {
            city = popup_game_with_sprite((const char *)&str_pretextallied,(iptr)(((i8 *)&diplo_popup_portrait_sprite) +
                                                      ((game.advance_known_by[0xf] != '\0') - 1 &
                                                      0xffff7c80)),0);
            if (city == 1) {
              diplo_clear_treaty_flags(a.unit,a.civ3,8);
              civs[a.civ3].last_contact_turn[a.unit] = game.turn;
            }
          }
          break;
        case 0xa5:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
            parley_wnd_block.remote_cities_revealed = (byte)a.civ3;
            if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 3)) ||
               (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 3)))) {
              ctrl_disable((Button *)parley_wnd_block.info_hide_buttons[1]);
              ctrl_disable((Button *)parley_wnd_block.info_show_buttons[1]);
              parley_draw_city_list(1);
            }
          }
          break;
        case 0xa6:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
            parley_wnd_block.remote_cities_revealed = (byte)a.civ3;
            if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 3)) ||
               (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 3)))) {
              ctrl_enable((Button *)parley_wnd_block.info_hide_buttons[1]);
              ctrl_enable((Button *)parley_wnd_block.info_show_buttons[1]);
              parley_draw_city_list(1);
            }
          }
          break;
        case 0xa7:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
            parley_wnd_block.remote_units_revealed = (byte)a.civ3;
            if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 2)) ||
               (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 2)))) {
              ctrl_disable((Button *)parley_wnd_block.info_hide_buttons[1]);
              ctrl_disable((Button *)parley_wnd_block.info_show_buttons[1]);
              ctrl_enable((Button *)parley_wnd_block.sort_by_city_buttons[1]);
              ctrl_enable((Button *)parley_wnd_block.sort_by_unit_buttons[1]);
              parley_draw_unit_list(1);
            }
          }
          break;
        case 0xa8:
          net_msg_get_args((NetMsg *)msg,(i32 *)(&a.unit));
          if ((parley_wnd_block.closed == 0) && (parley_wnd_block.other_civ == a.unit)) {
            parley_wnd_block.remote_units_revealed = (byte)a.civ3;
            if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 2)) ||
               (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 2)))) {
              ctrl_enable((Button *)parley_wnd_block.info_hide_buttons[1]);
              ctrl_enable((Button *)parley_wnd_block.info_show_buttons[1]);
              ctrl_disable((Button *)parley_wnd_block.sort_by_city_buttons[1]);
              ctrl_disable((Button *)parley_wnd_block.sort_by_unit_buttons[1]);
              parley_sort_unit_list(&parley_wnd_block,0,parley_wnd_block.list_counts[1] - 1,1);
              parley_draw_unit_list(1);
            }
          }
        }
        operator_delete(msg);
      }
      else {
        dbg_logf_int2((const char *)&str_poll_retrieved_non_civ_message_message_d,msg[1],msg[2]);
      }
    }
    if (((wait_mode == 0) && (self->net_listing_games != '\0')) && (gamelist_dirty != '\0'))
    {
      net_gamelist_refresh();
    }
    if (((wait_mode == 0) && (self->net_popup_in_progress_i32 == 0)) && (net_popup_queue_head != 0)) {
      net_popup_queue_show_next();
    }
    self->net_poll_depth = self->net_poll_depth - 1;
  }
  return;
}

/* 00482337 auto */
void __cdecl net_game_xmit_wait_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  if (net_session_block.nm_continue_after_join_count != 0) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 004823d6 auto */
void __cdecl net_msg_get_args(NetMsg *msg,i32 *args)

{
  *args = msg->args[0];
  args[1] = msg->args[1];
  args[2] = msg->args[2];
  args[3] = msg->args[3];
  args[4] = msg->args[4];
  args[5] = msg->args[5];
  args[6] = msg->args[6];
  return;
}

/* 00482470 auto */
void __cdecl net_server_quit_wait_idle(void)

{
  int human_count;
  
  net_process_messages(&net_session_block,1,0);
  if (((net_session_block.net_players_awaited != 0) &&
      (human_count = bits_count8((uint)(game.human_players == '\0')), human_count == 0)) &&
     (1 < net_session_block.net_player_count[0])) {
    return;
  }
  net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
  wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  return;
}

/* 004824e3 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_host_quit_handoff(void)

{
  byte sign;
  int human_count;
  int slot;
  
  net_session_block.net_host_handoff_stage = 1;
  net_drop_flagged_players();
  game.human_players = net_session_block.pending_human_players | game.human_players;
  net_session_block.pending_human_players = 0;
  if (1 < net_session_block.net_player_count[0]) {
    game.human_players = game.human_players & ~(byte)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
    net_session_block.net_players_awaited = net_session_block.net_player_count[0] - 1;
    net_send_message(0xe,0xff,0,0,0,0,0,0,0,0);
    net_drop_flagged_players();
    dlg_pending_idle_callback = net_server_quit_wait_idle;
    popup_game_sprite_flags((const char *)&str_serverquitwait,0x2000000,(iptr)&popup_info_icon_sprite,0);
    human_count = bits_count8((uint)(game.human_players == '\0'));
    if (human_count == 0) {
      net_send_message(0xf,0xff,0,0,0,0,0,0,0,0);
      sign = (byte)(net_session_block.mp_turn_civ + 1 >> 0x1f);
      civ_saved_cursor_x_block.saved_turn_civ = (((byte)(net_session_block.mp_turn_civ + 1) ^ sign) - sign & 7 ^ sign) - sign;
      for (slot = 1; (slot < 7 && ((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] == '\0'));
          slot = slot + 1) {
      }
      if (slot < 7) {
        crt_memcpy(((i8 *)&net_session_block.net_player_slots),((i8 *)&net_session_block.net_player_slots) + slot * 0x54,0x54);
        net_session_block.net_slot_host_flags[0] = 1;
        net_session_block.net_player_slots[0] = 0;
        net_session_block.net_civ_slots[0] = net_session_block.net_civ_slots[slot];
        net_session_block.net_player_count[0]--;
        net_session_block.net_civ_slots[slot] = 0xffffffff;
        _memset(((i8 *)&net_session_block.net_player_slots) + slot * 0x54,0,0x54);
        net_send_state_diff(0xff,2,0,1,0);
        net_send_message(0x16,0xff,0,0,0,0,0,0,0,0);
        net_send_message(4,0xff,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
    }
  }
  return;
}

/* 00482724 auto */
void __cdecl net_client_quit_wait_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  if (net_session_block.net_host_handoff_stage == 2) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 0048276d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_start_civ_idle(void)

{
  int now;
  
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.nm_start_civ_count == 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net_session_block.net_timeout_seconds * 0x3c)) {
    return;
  }
  net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
  wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  return;
}

/* 004827d5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_player_list_idle(void)

{
  int now;
  
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.nm_player_list_count == 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net_session_block.net_timeout_seconds * 0x3c)) {
    return;
  }
  net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
  wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  return;
}

/* 0048283d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_players_idle(void)

{
  int now;
  
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.net_players_awaited != 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net_session_block.net_timeout_seconds * 0x3c)) {
    return;
  }
  net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
  wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  return;
}

/* 004828a5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_client_transfer_server(void)

{
  char transport_ok;
  i32 result;
  int cmp;
  int slot;
  int old_slot;
  struct { int slots[2]; char addresses[76]; int civ_ids[135]; } saved;  /* a copy of the player table */
  int status;
  int slot_civ;
  
  net_session_block.nm_server_quit_count_i32 = 0;
  net_session_block.nm_start_civ_count = 0;
  net_session_block.nm_player_list_count = 0;
  net_send_message(0x10,0,0,0,0,0,0,0,0,0);
  dlg_pending_idle_callback = net_client_quit_wait_idle;
  popup_game_sprite_flags((const char *)&str_clientquitwait,0x2000000,(iptr)&popup_info_icon_sprite,0);
  net_session_block.net_host_handoff_stage = 0;
  dlg_pending_idle_callback = net_quit_wait_start_civ_idle;
  map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
  popup_game_sprite_flags((const char *)&str_clientquitwait_2,0x2000000,(iptr)&popup_info_icon_sprite,0);
  if (net_session_block.nm_start_civ_count == 0) {
    dbg_log((const char *)&str_clienttransferserver_waiting_for_nm_star);
    crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
    popup_game_with_sprite((const char *)&str_lanconnectfail_3,(iptr)&popup_info_icon_sprite,0);
    player_turn_active = 0;
    game_loop_active = 0;
    result = 0;
  }
  else {
    turn_resume_civ = civ_saved_cursor_x_block.saved_turn_civ;
    dlg_pending_idle_callback = net_quit_wait_player_list_idle;
    map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
    popup_game_sprite_flags((const char *)&str_clientquitwait_3,0x2000000,(iptr)&popup_info_icon_sprite,0);
    if (net_session_block.nm_player_list_count == 0) {
      dbg_log((const char *)&str_clienttransferserver_waiting_for_nm_play);
      crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
      popup_game_with_sprite((const char *)&str_lanconnectfail_4,(iptr)&popup_info_icon_sprite,0);
      player_turn_active = 0;
      game_loop_active = 0;
      result = 0;
    }
    else {
      crt_memcpy(&saved,((iptr *)&net_session_block.net_player_count),0x270);
      cmp = _strcmp(((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),((i8 *)&net_session_block.net_slot_addresses));
      if (cmp == 0) {
        dbg_log((const char *)&str_clienttransferserver_disconnecting_from);
        net_session_shutdown(&net_session_block,0);
        transport_ok = net_init_transport(&net_session_block,net_session_block.net_protocol,1);
        status = (int)transport_ok;
        if (status == 0) {
          dbg_log((const char *)&str_clienttransferserver_could_not_reconnect);
          crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
          popup_game_with_sprite((const char *)&str_lanconnectfail_5,(iptr)&popup_info_icon_sprite,0);
          game_loop_active = 0;
          player_turn_active = 0;
          return 0;
        }
        dbg_log((const char *)&str_reconnected_to_network_as_server);
        crt_strcpy(((i8 *)&net_session_block.net_server_address),((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address));
        net_session_block.net_server_conn_id = 0;
        net_session_block.net_local_slot = 0;
        map_views_block.diffeng_mirror_end = (*(int *)&map_views_block.diffeng_mirror_size) + diffeng_mirror_buf;
        net_session_block.net_players_awaited = saved.slots[0] - 1;
        net_session_block.net_players_awaited_initial = net_session_block.net_players_awaited;
        if (net_session_block.net_players_awaited < 1) {
          slot_civ = saved.civ_ids[0];
          net_session_block.net_civ_slots[saved.civ_ids[0]] = 0;
          net_session_block.net_slot_civs[0] = saved.civ_ids[0];
        }
        else {
          dlg_pending_idle_callback = net_quit_wait_players_idle;
          map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
          popup_game_sprite_flags((const char *)&str_clientquitwait_4,0x2000000,(iptr)&popup_info_icon_sprite,0);
          cmp = timer_ticks();
          if ((net_session_block.net_timeout_seconds * 0x3c < cmp - map_views_block.net_handoff_wait_ticks_i32) && (net_session_block.net_players_awaited != 0)) {
            crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
            if (net_session_block.net_players_awaited == net_session_block.net_players_awaited_initial) {
              popup_game_with_sprite((const char *)&str_nooneshowed,(iptr)&popup_info_icon_sprite,0);
              game_loop_active = 0;
              player_turn_active = 0;
              return 0;
            }
            popup_game_with_sprite((const char *)&str_someshowed,(iptr)&popup_info_icon_sprite,0);
          }
          for (slot = 0; slot < 7; slot++) {
            if ((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] != '\0') {
              old_slot = 0;
              while ((old_slot < 7 &&
                     (cmp = _strcmp(saved.addresses + old_slot * 0x54,
                                      ((i8 *)&net_session_block.net_slot_addresses) + slot * 0x54), cmp != 0))) {
                old_slot++;
              }
              if (6 < old_slot) {
                err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_oldindex_max_net_players,(iptr)&str_d_ss_franklinton_netmgr_poll_cpp_5,0x8a7);
              }
              slot_civ = saved.civ_ids[old_slot * 0x15];
              net_session_block.net_civ_slots[slot_civ] = slot;
              (((i32 *)&net_session_block.net_slot_civs))[slot * 0x15] = slot_civ;
            }
          }
          net_session_block.mp_simultaneous_moves = *(i8 *)&civ_saved_cursor_x_block.mp_simultaneous_setting;
          net_send_message(0x33,0xff,(int)civ_saved_cursor_x_block.mp_double_production,(int)civ_saved_cursor_x_block.mp_double_moves,(int)(*(i8 *)&civ_saved_cursor_x_block.mp_simultaneous_setting),0,0,0,0
                             ,0);
          net_send_message(4,0xff,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(60000);
        }
        _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots),-1,0xc0);
        net_session_block.net_session_phase = 3;
        net_reset_error_counts();
        net_session_block.nm_server_quit_count_i32 = 0;
        net_session_block.nm_start_civ_count = 0;
        net_session_block.nm_player_list_count = 0;
        if (game_over_reason != 2) {
          popup_game_with_sprite((const char *)&str_hostchangeserver,(iptr)&popup_info_icon_sprite,0);
        }
      }
      else {
        dbg_logf_str((const char *)&str_disconnecting_from_server_s,(const char *)&net_session_block.net_server_address);
        status = XD_CloseConnection();
        if (status != 0) {
          dbg_logf_str_int((const char *)&str_clienttransferserver_disconnection_from,(const char *)&net_session_block.net_server_address,status);
          crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
          popup_game_with_sprite((const char *)&str_lanconnectfail_6,(iptr)&popup_info_icon_sprite,0);
          game_loop_active = 0;
          player_turn_active = 0;
          return 0;
        }
        crt_strcpy(((i8 *)&net_session_block.net_server_address),((i8 *)&net_session_block.net_slot_addresses));
        map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
        do {
          cmp = timer_ticks();
        } while (cmp - map_views_block.net_handoff_wait_ticks_i32 < 0x3c);
        dbg_logf_str((const char *)&str_attempting_to_reconnect_to_new_server_s,(const char *)&net_session_block.net_server_address);
        status = XD_OpenConnection(((i8 *)&net_session_block.net_server_address),net_session_block.net_timeout_seconds * 1000);
        if (status < 0) {
          dbg_logf((const char *)&str_connecttogame_open_connection_failed_err,status);
          crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
          popup_game_with_sprite((const char *)&str_serverconnectfail_2,(iptr)&popup_info_icon_sprite,0);
          game_loop_active = 0;
          player_turn_active = 0;
          return 0;
        }
        dbg_log((const char *)&str_connection_to_new_server_successful);
        net_session_block.nm_server_quit_count_i32 = 0;
        net_session_block.nm_start_civ_count = 0;
        dlg_pending_idle_callback = net_quit_wait_player_list_idle;
        map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
        popup_game_sprite_flags((const char *)&str_clientquitwait_5,0x2000000,(iptr)&popup_info_icon_sprite,0);
        if (net_session_block.nm_player_list_count == 0) {
          dbg_log((const char *)&str_clienttransferserver_client_timed_out_wa);
          crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
          popup_game_with_sprite((const char *)&str_lanconnectfail_7,(iptr)&popup_info_icon_sprite,0);
          game_loop_active = 0;
          player_turn_active = 0;
          return 0;
        }
        crt_strcpy(((i8 *)&minimap_window_block.text_string_args),((i8 *)&net_session_block.net_slot_player_names));
        net_reset_error_counts();
        net_session_block.nm_player_list_count = 0;
        popup_game_with_sprite((const char *)&str_hostchangeclient,(iptr)&popup_info_icon_sprite,0);
      }
      result = 1;
    }
  }
  return result;
}

/* 0048bf51 auto */
void __cdecl net_client_hot_wait_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  net_heartbeat_check(net_session_block.net_slot_civs[0]);
  if ((((net_session_block.nm_start_civ_count != 0) || (net_session_block.net_host_handoff_stage != '\0')) || (net_session_block.nm_end_sequence_count != 0)) ||
     (((net_session_block.player_has_quit != '\0' || (game_loop_active == '\0')) || (net_session_block.net_lost_conn_total != 0)))) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 0048d9ad auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_heartbeat_send(iptr civ)

{
  if (net_session_block.net_drag_hold == '\0') {
    net_session_block.nm_heartbeat_ack_count = 0;
    net_send_message(0x2b,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,0,0,0,0,
                       0,0,0,0);
  }
  else {
    net_session_block.nm_heartbeat_ack_count = 1;
  }
  if (((int)parley_wnd_block.closed == 1) && (net_popup_queue_head == 0)) {
    mp_meeting_pending = 0;
    net_session_block.nm_parley_cancel_count = 0;
  }
  map_views_block.net_heartbeat_last_tick_i32 = timer_ticks();
  return;
}

/* 0048da51 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_heartbeat_check(iptr civ)

{
  int now;
  
  now = timer_ticks();
  if (0xe0f < now - map_views_block.net_heartbeat_last_tick_i32) {
    if (net_session_block.nm_heartbeat_ack_count == 0) {
      net_session_block.net_civ_turn_ended[civ] = net_session_block.net_civ_turn_ended[civ] + 1;
      if (net_session_block.net_host_flag == '\0') {
        net_session_block.net_lost_conn_total++;
      }
    }
    else {
      net_heartbeat_send(civ);
    }
  }
  return;
}

/* 0048dab9 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_check_lost_connections(void)

{
  int slot_civ;
  bool no_player;
  int civ;
  int dropped_count;
  int lost_counts[7];
  iptr slot;
  
  if (net_session_block.net_host_flag == '\0') {
    if (net_session_block.net_lost_conn_total != 0) {
      game_loop_active = 0;
      dbg_log((const char *)&str_we_lost_connection_to_the_server_heartbe);
      net_session_shutdown(&net_session_block,1);
      popup_game_with_sprite((const char *)&str_lostserver,(iptr)&popup_info_icon_sprite,0);
      return 1;
    }
  }
  else {
    dropped_count = net_drop_flagged_players();
    for (civ = 1; civ < 8; civ++) {
      no_player = true;
      if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
         ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        for (slot = 0; slot < 7; slot++) {
          if (((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] != '\0') &&
             ((((i32 *)&net_session_block.net_slot_civs))[slot * 0x15] == civ)) {
            no_player = false;
            break;
          }
        }
        if (no_player) {
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          if ((1 << ((byte)civ & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)civ & 0x1f));
            net_send_message(0x2a,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net_session_block.nm_lock_nation_count_i32++;
          }
          dropped_count++;
        }
      }
    }
    if (net_session_block.net_lost_conn_total != 0) {
      crt_memcpy(lost_counts,((i8 *)&net_session_block.net_lost_conn_by_player),0x1c);
      net_reset_error_counts();
      for (slot = 1; slot < 7; slot++) {
        if ((0 < lost_counts[slot]) && ((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] != '\0')) {
          dropped_count++;
          slot_civ = (((i32 *)&net_session_block.net_slot_civs))[slot * 0x15];
          text_set_string_arg(0,(const char *)(((i8 *)&net_session_block.net_slot_player_names) + slot * 0x54));
          net_send_text_args(0x53,0xff,1,0,0,0);
          net_session_block.net_civ_drop_requested[slot_civ] = 0;
          (*((char *)&civ + 0)) = (byte)slot_civ;
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          net_remove_player
                    (&net_session_block,net_session_block.net_slots[slot].id);
          if ((1 << ((byte)civ & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)civ & 0x1f));
            net_send_message(0x2a,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net_session_block.nm_lock_nation_count_i32++;
          }
          popup_game_with_sprite((const char *)&str_lostclient,(iptr)&popup_info_icon_sprite,0);
        }
      }
    }
    if (dropped_count != 0) {
      net_session_block.net_player_count[0] = 0;
      for (slot = 0; slot < 7; slot++) {
        if ((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] != '\0') {
          net_session_block.net_player_count[0]++;
        }
      }
      net_send_message(4,0xff,0,0,0,0,0,0,0,0);
      net_send_state_diff(0xff,2,0,0,1);
    }
  }
  return 0;
}

/* 0048de75 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl net_drop_flagged_players(void)

{
  int slot_civ;
  byte civ_bit;
  int dropped_count;
  int slot;
  
  dropped_count = 0;
  for (slot = 1; slot < 7; slot++) {
    if (((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] != '\0') &&
       (net_session_block.net_civ_drop_requested[(((i32 *)&net_session_block.net_slot_civs))[slot * 0x15]] != 0)) {
      dropped_count++;
      slot_civ = (((i32 *)&net_session_block.net_slot_civs))[slot * 0x15];
      net_session_block.net_civ_drop_requested[slot_civ] = 0;
      net_session_block.net_lost_conn_by_player[net_session_block.net_slots[slot].id] = 0;
      civ_bit = (byte)slot_civ;
      game.human_players = game.human_players & ~(byte)(1 << (civ_bit & 0x1f));
      net_remove_player
                (&net_session_block,net_session_block.net_slots[slot].id);
      if ((1 << (civ_bit & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
        (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << (civ_bit & 0x1f));
        net_session_block.nm_lock_nation_count_i32++;
      }
    }
  }
  if (dropped_count != 0) {
    net_send_message(0x2a,0xff,(int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
    net_send_message(4,0xff,0,0,0,0,0,0,0,0);
  }
  return dropped_count;
}

/* 004923f0 auto */
void __cdecl chat_insert_macro_text(EditControl *edit,i32 macro_id)

{
  bool all_contact;
  int civ;
  iptr name2;
  size_t text_len;
  int tries;
  
  text_clear();
  tries = 0;
  if (((game.wonder_city[0x18] == -1) && (civ = civ_has_wonder(human_civ[0],9), civ == 0))
     && (game.reveal_map == '\0')) {
    all_contact = false;
  }
  else {
    all_contact = true;
  }
  switch(macro_id) {
  case 0x2b0:
    while (((((civ = chat_macro_next_adj_civ, tries < 8 && (human_civ[0] != chat_macro_next_adj_civ)) && (!all_contact))
            && ((civ_saved_cursor_x_block.mp_chat_from_start == 0 ||
                ((1 << ((byte)chat_macro_next_adj_civ & 0x1f) & (uint)game.human_players) == 0)))) &&
           ((*(byte *)&civs[chat_macro_next_adj_civ].treaties[human_civ[0]] & 1) == 0))) {
      tries++;
      chat_macro_next_adj_civ++;
      if (chat_macro_next_adj_civ == 8) {
        chat_macro_next_adj_civ = 1;
      }
    }
    if (tries == 8) {
      return;
    }
    chat_macro_next_adj_civ++;
    name2 = civ_adjective(civ);
    text_append((const char *)name2);
    text_append_space();
    if (chat_macro_next_adj_civ == 8) {
      chat_macro_next_adj_civ = 1;
    }
    goto replace_selection;
  case 0x2b1:
    while ((((tries < 8 && (human_civ[0] != chat_macro_next_leader_civ)) &&
            ((!all_contact &&
             ((civ_saved_cursor_x_block.mp_chat_from_start == 0 ||
              ((1 << ((byte)chat_macro_next_leader_civ & 0x1f) & (uint)game.human_players) == 0)))))) &&
           ((*(byte *)&civs[chat_macro_next_leader_civ].treaties[human_civ[0]] & 1) == 0))) {
      tries++;
      chat_macro_next_leader_civ++;
      if (chat_macro_next_leader_civ == 8) {
        chat_macro_next_leader_civ = 1;
      }
    }
    if (tries == 8) {
      return;
    }
    name2 = civ_leader_title(chat_macro_next_leader_civ);
    text_append((const char *)name2);
    text_append_space();
    civ = chat_macro_next_leader_civ;
    chat_macro_next_leader_civ++;
    name2 = (iptr)(civ_leader_name(civ));
    text_append((const char *)name2);
    text_append_space();
    if (chat_macro_next_leader_civ == 8) {
      chat_macro_next_leader_civ = 1;
    }
    goto replace_selection;
  case 0x2b2:
    chat_read_macro_line((const char *)&str_chatmac1_txt,(iptr)&chat_macro1_file_offset);
    civ = chat_macro1_file_offset;
    break;
  case 0x2b3:
    chat_read_macro_line((const char *)&str_chatmac2_txt,(iptr)&chat_macro2_file_offset);
    civ = chat_macro2_file_offset;
    break;
  case 0x2b4:
    chat_read_macro_line((const char *)&str_chatmac3_txt,(iptr)&chat_macro3_file_offset);
    civ = chat_macro3_file_offset;
    break;
  default:
    goto replace_selection;
  }
  if (civ == 0) {
    return;
  }
replace_selection:
  edit_replace_sel(edit,(const char *)&map_views_block.text_buffer);
  civ = edit_get_sel_end(edit);
  text_len = _strlen(((i8 *)&map_views_block.text_buffer));
  edit_set_sel(edit,civ - text_len,civ);
  return;
}

/* 0049275a auto */
void __cdecl chat_read_macro_line(const char *path,iptr file_offset)

{
  int ok;
  size_t line_len;
  i8 file[160];
  i32 unwind_state;
  
  unwind_state = 0xffffffff;
  file_obj_init((FileObject *)file);
  unwind_state = 0;
  ok = file_obj_open((FileObject *)file,path);
  if (ok == 0) {
    *(i32 *)file_offset = 0;
    unwind_state = 0xffffffff;
    /* inlined unwind funclet 00492868 */ { file_obj_close((FileObject *)&file); }
  }
  else {
    ok = file_obj_get_size((FileObject *)file);
    if (ok <= *(int *)file_offset) {
      *(i32 *)file_offset = 0;
    }
    ok = file_obj_seek_cur((FileSeekView *)file,*(i32 *)file_offset);
    if (ok == 0) {
      *(i32 *)file_offset = 0;
      file_map_close((FileObject *)file);
      unwind_state = 0xffffffff;
      /* inlined unwind funclet 00492868 */ { file_obj_close((FileObject *)&file); }
    }
    else {
      file_obj_read_line((FileReadLineView *)file,(char *)&map_views_block.text_buffer,0x100);
      file_map_close((FileObject *)file);
      line_len = _strlen(((i8 *)&map_views_block.text_buffer));
      *(size_t *)file_offset = *(int *)file_offset + line_len + 2;
      unwind_state = 0xffffffff;
      /* inlined unwind funclet 00492868 */ { file_obj_close((FileObject *)&file); }
    }
  }
  return;
}

/* 004af3e0 auto */
void __thiscall parley_build_civ_list(ParleyWindow *self,RECT *rect,int side)

{
  u8 *field;
  int has_wonder;
  int scroll_width;
  Button *new_ctrl;
  u8 *hotspot_parent;
  u8 *scroll_parent;
  i32 hotspot;
  i32 scrollbar;
  u32 other_civ;
  RECT rect2;
  int all_contact;
  i32 seh_prev;
  i8 *seh_handler;
  i32 unwind_state;
  
  unwind_state = 0xffffffff;
  rect2.left = rect->left;
  rect2.top = rect->top;
  rect2.right = rect->right;
  rect2.bottom = rect->bottom;
  if (game.wonder_city[0x18] == -1) {
    has_wonder = civ_has_wonder(human_civ[0],9);
    if ((has_wonder == 0) && (game.reveal_map == '\0')) {
      all_contact = 0;
      goto reset_contact_field;
    }
  }
  all_contact = 1;
reset_contact_field:
  field = (u8 *)&self->list_counts[side];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  for (other_civ = 1; (int)other_civ < 8; other_civ++) {
    if (((human_civ[0] != other_civ) && (self->other_civ != other_civ)) &&
       ((all_contact != 0 || ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & 1) != 0))))
    {
      self->list_items[side * 0x801 + self->list_counts[side]] = other_civ;
      self->list_selected[side * 0x801 + self->list_counts[side]] =
           0;
      self->list_counts[side] =
           self->list_counts[side] + 1;
    }
  }
  if (self->list_counts[side] < 1) {
    field = (u8 *)&self->list_first_row[side];
    field[0] = 0xff;
    field[1] = 0xff;
    field[2] = 0xff;
    field[3] = 0xff;
  }
  else {
    field = (u8 *)&self->list_first_row[side];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  field = (u8 *)&self->list_columns[side];
  field[0] = 2;
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  self->list_rects[side * 4] = rect->left;
  self->list_rects[side * 4 + 1] = rect->top;
  self->list_rects[side * 4 + 2] = rect->right;
  self->list_rects[side * 4 + 3] = rect->bottom;
  if (self->list_rows[side] <
      self->list_counts[side]) {
    scroll_width = GetSystemMetrics(2);
    has_wonder = rect2.right;
    self->list_rects[side * 4 + 2] =
         (int)self->list_rects[side * 4 + 2] - scroll_width;
    rect2.left = GetSystemMetrics(2);
    rect2.left = has_wonder - rect2.left;
    new_ctrl = (Button *)(operator_new(0x40));
    unwind_state = 0;
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(scrollbar_ctor((Scrollbar *)new_ctrl));
    }
    unwind_state = 0xffffffff;
    self->list_scrollbars[side] = scrollbar;
    if (self == (ParleyWindow *)0x0) {
      scroll_parent = (u8 *)0x0;
    }
    else {
      scroll_parent = ((u8 *)self + 0x48);
    }
    scrollbar_create
              ((Scrollbar *)self->list_scrollbars[side],(iptr)scroll_parent,
               side + 0x40d,(iptr)&rect2,1);
    scrollbar_set_range
              ((Scrollbar *)self->list_scrollbars[side],0,
               self->list_counts[side] -
               self->list_rows[side]);
    scrollbar_set_pos((Scrollbar *)self->list_scrollbars[side],0);
    scrollbar_set_page_size
              ((Scrollbar *)self->list_scrollbars[side],
               self->list_rows[side]);
    scrollbar_set_callback
              ((Scrollbar *)self->list_scrollbars[side],
               (side == 0 ? parley_civ_list_scroll_local : parley_civ_list_scroll_remote));
    scrollbar_set_track_callback
              ((Scrollbar *)self->list_scrollbars[side],
               (side == 0 ? parley_civ_list_scroll_local : parley_civ_list_scroll_remote));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  unwind_state = 1;
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  unwind_state = 0xffffffff;
  self->list_hotspots[side] = hotspot;
  if (self == (ParleyWindow *)0x0) {
    hotspot_parent = (u8 *)0x0;
  }
  else {
    hotspot_parent = ((u8 *)self + 0x48);
  }
  hotspot_create
            ((Hotspot *)self->list_hotspots[side],(iptr)hotspot_parent,
             side + 0x40f,(iptr)(((u8 *)self + 0x39c) + side * 0x10 + 0x24));
  hotspot_set_click_callback((Hotspot *)self->list_hotspots[side],parley_civ_list_click);
  hotspot_set_dblclick_callback((Hotspot *)self->list_hotspots[side],parley_civ_list_click);
  parley_draw_civ_list(side);
  return;
}

/* 004af867 auto */
void __cdecl parley_civ_list_scroll_local(i32 pos)

{
  parley_civ_list_scroll(0,pos);
  return;
}

/* 004af885 auto */
void __cdecl parley_civ_list_scroll_remote(i32 pos)

{
  parley_civ_list_scroll(1,pos);
  return;
}

/* 004af8a3 auto */
void __cdecl parley_civ_list_scroll(int side,i32 pos)

{
  int active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  wnd_activate((Wnd *)((iptr)wnd2 + 0x48));
  wnd2->list_first_row[side] = pos;
  parley_draw_civ_list(side);
  return;
}

/* 004af904 auto */
void __cdecl parley_civ_list_click(int ctrl_id)

{
  uint *sel_flag;
  int row;
  int key_down;
  int range_end;
  int i;
  i32 cursor_y;
  i32 cursor_x;
  int side;
  ParleyWindow *wnd2;
  
  row = wnd_get_active_child();
  if (row == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(row - 0x48);
  }
  wnd_activate((Wnd *)((iptr)wnd2 + 0x48));
  side = ctrl_id - 0x40f;
  wnd_get_cursor_pos
            ((WndBase *)((iptr)wnd2 + 0x48),(iptr)&cursor_x,(iptr)&cursor_y);
  if (((wnd2->list_counts[side] != 0) &&
      (row = parley_civ_list_hit_test(cursor_x,cursor_y,side), -1 < row)) &&
     (row < wnd2->list_counts[side])) {
    key_down = wnd_is_shift_down((void *)((iptr)wnd2 + 0x48));
    if (key_down == 0) {
      key_down = wnd_is_ctrl_down((void *)((iptr)wnd2 + 0x48));
      if (key_down == 0) {
        for (i = 0; i < wnd2->list_counts[side];
            i = i + 1) {
          if (i != row) {
            wnd2->list_selected[i + side * 0x801] = 0;
          }
        }
        wnd2->list_anchor_row[side] = row;
        sel_flag = (uint *)(row * 4 + side * 0x2004 + 0x8400 + (iptr)wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        wnd2->list_anchor_row[side] = row;
        sel_flag = (uint *)(row * 4 + side * 0x2004 + 0x8400 + (iptr)wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < wnd2->list_counts[side]; i++)
      {
        wnd2->list_selected[i + side * 0x801] = 0;
      }
      if (wnd2->list_anchor_row[side] < row) {
        i = wnd2->list_anchor_row[side];
        range_end = row;
      }
      else {
        range_end = wnd2->list_anchor_row[side];
        i = row;
      }
      for (; i <= range_end; i++) {
        wnd2->list_selected[i + side * 0x801] = 1;
      }
    }
    parley_draw_civ_list(side);
  }
  return;
}

/* 004afb77 auto */
int __cdecl parley_civ_list_hit_test(int x,int y,int side)

{
  iptr active;
  int row;
  int list_height;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  if (y < (int)wnd2->list_rects[side * 4 + 1]) {
    row = -1;
  }
  else if (y < (int)wnd2->list_rects[side * 4 + 3]) {
    if (x < (int)wnd2->list_rects[side * 4]) {
      row = -3;
    }
    else if (x < (int)wnd2->list_rects[side * 4 + 2]) {
      row = (int)wnd2->list_rects[side * 4 + 1];
      list_height = rect_height((RECT *)(side * 0x10 + (iptr)wnd2 + 0x3c0));
      row = (y - row) / (list_height / wnd2->list_rows[side]) +
              wnd2->list_first_row[side];
    }
    else {
      row = -4;
    }
  }
  else {
    row = -2;
  }
  return row;
}

/* 004afc89 auto */
void __cdecl parley_draw_civ_list(int side)

{
  u8 *field;
  int xLeft;
  int selected;
  iptr active;
  int list_width;
  int text_dy;
  int row_top;
  int text_dx;
  i32 font_height;
  const char *text;
  i32 bg_color2;
  int row;
  int shadow_color2;
  int text_color2;
  ParleyWindow *wnd2;
  tagRECT row_rect;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  surface_get_clip(wnd2,(RECT *)saved_clip);
  surface_set_clip(wnd2,(RECT *)(iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10));
  surface_fill_rect
            (wnd2,(iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10),list_bg_color);
  if (wnd2->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  xLeft = (int)wnd2->list_rects[side * 4];
  list_width = rect_width((RECT *)(iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10));
  row_height = rect_height((RECT *)(iptr)(((u8 *)wnd2 + 0x3c0) + side * 0x10));
  row_height = row_height / wnd2->list_rows[side];
  text_dy = row_height / 2;
  row_top = font_cached_height(font2);
  text_dy -= row_top / 2;
  field = (u8 *)&wnd2->side_selected[side];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  row = 0;
  do {
    if (wnd2->list_counts[side] <= row) {
draw_rows:
      for (row = 0; row < wnd2->list_rows[side];
          row = row + 1) {
        row_top = (int)wnd2->list_rects[side * 4 + 1] + row_height * row;
        SetRect(&row_rect,xLeft,row_top,list_width + xLeft,row_height + row_top);
        if (((row == 0) || (wnd2->list_first_row[side] != -1)) &&
           (wnd2->list_first_row[side] + row <
            wnd2->list_counts[side])) {
          selected = (int)wnd2->list_selected[side * 0x801 + (wnd2->list_first_row[side] + row)];
          if (selected == 0) {
            text_color2 = list_text_color;
            shadow_color2 = list_shadow_color;
          }
          else {
            text_color2 = dlg_sel_text_color;
            shadow_color2 = dlg_sel_text_shadow_color;
          }
          text_dx = (-(uint)(wnd2->large_layout == 0) & 0xffffffe2) + 0x5a + panel_margin_x;
          row_rect.left = row_rect.left + text_dx - 3;
          if (selected == 0) {
            bg_color2 = list_bg_color;
          }
          else {
            bg_color2 = list_sel_bg_color;
          }
          surface_fill_rect(wnd2,(iptr)&row_rect,bg_color2);
          if (wnd2->list_first_row[side] == -1) {
            text_clear();
            text = (label_string(label_string_ids[0x2e1]));
            text_append(text);
            field = (u8 *)&wnd2->side_selected[side];
            field[0] = '\0';
            field[1] = '\0';
            field[2] = '\0';
            field[3] = '\0';
          }
          else {
            font_height = font_cached_height(font2);
            parley_draw_civ_capital((i32)wnd2->list_items[side * 0x801 + (wnd2->list_first_row[side] + row)],
                               wnd2->list_first_row[side] + row,
                               xLeft + 2,row_top,font_height);
            text_clear();
            text = civ_name_plural((iptr)wnd2->list_items[side * 0x801 + (wnd2->list_first_row[side] + row)]);
            text_append(text);
            text_append_space();
            text_append_lparen();
            text = civ_leader_name((iptr)wnd2->list_items[side * 0x801 + (wnd2->list_first_row[side] + row)]);
            text_append(text);
            text_append_rparen();
          }
          if (shadow_color2 != text_color2) {
            gfx_set_text_color_index(shadow_color2);
            surface_draw_text
                      (wnd2,(iptr)font2,(const char *)&map_views_block.text_buffer,text_dx + xLeft + 2,
                       text_dy + row_top + 1,5);
            gfx_set_text_color_index(text_color2);
            surface_draw_text
                      (wnd2,(iptr)font2,(const char *)&map_views_block.text_buffer,text_dx + xLeft + 1,
                       text_dy + row_top,5);
          }
          gfx_set_text_color_index(text_color2);
          surface_draw_text
                    (wnd2,(iptr)font2,(const char *)&map_views_block.text_buffer,text_dx + xLeft,text_dy + row_top,5)
          ;
        }
      }
      surface_set_clip(wnd2,(RECT *)saved_clip);
      parley_update_buttons(wnd2);
      ctrl_show_and_redraw((Control *)wnd2->list_hotspots[side])
      ;
      return;
    }
    if (wnd2->list_selected[side * 0x801 + row] != 0) {
      field = (u8 *)&wnd2->side_selected[side];
      field[0] = 1;
      field[1] = '\0';
      field[2] = '\0';
      field[3] = '\0';
      goto draw_rows;
    }
    row++;
  } while( true );
}

/* 004b0157 auto */
void __cdecl parley_draw_civ_capital(int civ,uint row,int x,int y,int row_height)

{
  iptr active;
  int has_palace;
  iptr city2;
  i32 zoom2;
  iptr city;
  int draw_x;
  int icon_width;
  ParleyWindow *wnd2;
  int best_score;
  int score;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  if (wnd2->large_layout == 0) {
    zoom2 = 0xfffffffc;
  }
  else {
    zoom2 = 0xfffffffe;
  }
  best_score = 0;
  city2 = -1;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      score = (int)cities[city].size;
      has_palace = city_has_improvement(city,1);
      if (has_palace != 0) {
        score += 200;
      }
      if (cities[city].building == -1) {
        score += 100;
      }
      if (best_score < score) {
        best_score = score;
        city2 = city;
      }
    }
  }
  if (-1 < city2) {
    draw_x = x;
    if (wnd2->large_layout == 0) {
      icon_width = 0x18;
    }
    else {
      icon_width = 0x24;
    }
    if ((row & 1) != 0) {
      draw_x = x + icon_width + 2;
    }
    has_palace = gfx_zoom_scale(0x30,zoom2);
    gfx_draw_city((iptr)&parley_wnd_block,city2,0,draw_x,y - (has_palace - row_height) / 2,zoom2);
  }
  return;
}

/* 004b0b53 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl
net_send_state_diff(i32 dest,uint flags,int limit,i32 force,i32 alt_msg)

{
  int *mirror_start;
  int *run_start;
  int encoded;
  int bytes_changed;
  uint run_section;
  uint run_size;
  int *cur_ptr;
  RleStream rle;  /* (the original's locals in a row) */
  int *start_ptr;
  uint run_offset;
  GameState *game_mirror;
  uint *out_ptr;
  int record_count;
  int runs_left;
  int deadline;
  int *section_end;
  
  if (game.multiplayer < 3) {
    bytes_changed = 0;
  }
  else if (((char)force == '\0') && (net_session_block.net_player_count[0] == 1)) {
    bytes_changed = 0;
  }
  else {
    bytes_changed = 0;
    runs_left = 0;
    deadline = 0;
    start_ptr = (*(int * *)&map_views_block.diffeng_scan_ptr);
    diffeng_send_count_i32++;
    if ((flags & 1) != 0) {
      diffeng_invalidate_mirror();
    }
    if ((flags & 4) == 0) {
      if ((flags & 8) != 0) {
        runs_left = limit;
      }
    }
    else {
      deadline = timer_ticks();
      deadline = limit + deadline;
    }
    cur_ptr = (int *)((iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr + map_views_block.diffeng_scan_offset_i32);
    section_end = (int *)((i32)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].padded_size + (iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr);
    if ((*(uint *)&map_views_block.diffeng_scan_section) == 1) {
      game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng_mirror_buf);
      game_mirror->active_unit = game.active_unit;
      game_mirror->moving_unit = game.moving_unit;
      game_mirror->multiplayer = game.multiplayer;
      game_mirror->player_civ = game.player_civ;
      game_mirror->saved_current_civ = game.saved_current_civ;
      game_mirror->turn_civ = game.turn_civ;
      game_mirror->game_options = game.game_options;
      game_mirror->message_options = game.message_options;
    }
    map_views_block.diffeng_packet_i32 = 0x66606660;
    if ((char)alt_msg == '\0') {
      map_views_block.diffeng_packet_msg_type_i32 = 0x15;
    }
    else {
      map_views_block.diffeng_packet_msg_type_i32 = 0x5c;
    }
    out_ptr = ((uint *)&map_views_block.diffeng_packet_payload);
    record_count = 0;
    do {
      run_start = cur_ptr;
      mirror_start = (*(int * *)&map_views_block.diffeng_scan_ptr);
      if (*cur_ptr == *(*(int * *)&map_views_block.diffeng_scan_ptr)) {
        (*(int * *)&map_views_block.diffeng_scan_ptr)++;
        if ((*(int * *)&map_views_block.diffeng_scan_ptr) < (*(int * *)&map_views_block.diffeng_mirror_end)) {
          cur_ptr++;
          if (section_end <= cur_ptr) {
            (*(uint *)&map_views_block.diffeng_scan_section)++;
            if ((*(uint *)&map_views_block.diffeng_scan_section) == 1) {
              game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng_mirror_buf);
              game_mirror->active_unit = game.active_unit;
              game_mirror->moving_unit = game.moving_unit;
              game_mirror->multiplayer = game.multiplayer;
              game_mirror->player_civ = game.player_civ;
              game_mirror->saved_current_civ = game.saved_current_civ;
              game_mirror->turn_civ = game.turn_civ;
              game_mirror->game_options = game.game_options;
              game_mirror->message_options = game.message_options;
            }
            map_views_block.diffeng_scan_offset_i32 = 0;
            cur_ptr = (int *)(iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr;
            section_end = (int *)((i32)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].padded_size + (iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr
                             );
          }
        }
        else {
          (*(int * *)&map_views_block.diffeng_scan_ptr) = (int *)diffeng_mirror_buf;
          (*(uint *)&map_views_block.diffeng_scan_section) = 0;
          map_views_block.diffeng_scan_offset_i32 = 0;
          cur_ptr = (int *)diffeng_sections_block.diffeng_section_ptrs[0];
          section_end = (int *)(diffeng_sections_block.diffeng_section_padded_sizes[0] + diffeng_sections_block.diffeng_section_ptrs[0]);
        }
      }
      else {
        run_size = 0;
        do {
          (*(int * *)&map_views_block.diffeng_scan_ptr)++;
          if ((*(int * *)&map_views_block.diffeng_scan_ptr) < (*(int * *)&map_views_block.diffeng_mirror_end)) {
            cur_ptr++;
            if (section_end <= cur_ptr) {
              run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr;
              run_size = (int)section_end - (int)run_start;
              run_section = (*(uint *)&map_views_block.diffeng_scan_section);
              (*(uint *)&map_views_block.diffeng_scan_section)++;
              if ((*(uint *)&map_views_block.diffeng_scan_section) == 1) {
                game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng_mirror_buf);
                game_mirror->active_unit = game.active_unit;
                game_mirror->moving_unit = game.moving_unit;
                game_mirror->multiplayer = game.multiplayer;
                game_mirror->player_civ = game.player_civ;
                game_mirror->saved_current_civ = game.saved_current_civ;
                game_mirror->turn_civ = game.turn_civ;
                game_mirror->game_options = game.game_options;
                game_mirror->message_options = game.message_options;
              }
              map_views_block.diffeng_scan_offset_i32 = 0;
              cur_ptr = (int *)(iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr;
              section_end = (int *)((i32)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].padded_size +
                               (iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr);
            }
          }
          else {
            run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr;
            run_size = (int)(*(int * *)&map_views_block.diffeng_mirror_end) - (int)mirror_start;
            run_section = (*(uint *)&map_views_block.diffeng_scan_section);
            (*(int * *)&map_views_block.diffeng_scan_ptr) = (int *)diffeng_mirror_buf;
            (*(uint *)&map_views_block.diffeng_scan_section) = 0;
            map_views_block.diffeng_scan_offset_i32 = 0;
            cur_ptr = (int *)diffeng_sections_block.diffeng_section_ptrs[0];
            section_end = (int *)(diffeng_sections_block.diffeng_section_padded_sizes[0] + diffeng_sections_block.diffeng_section_ptrs[0]);
          }
        } while (((run_size == 0) &&
                 ((int)(*(int * *)&map_views_block.diffeng_scan_ptr) + (-(iptr)&map_views_block.diffeng_packet_i32 - (int)mirror_start) + (int)out_ptr < diffeng_packet_flush_size)) &&
                (*cur_ptr != *(*(int * *)&map_views_block.diffeng_scan_ptr)));
        if (0x400 < run_size) {
          err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_difflength_diff_engine_message_length,(iptr)&str_d_ss_franklinton_difference_engine_cpp_2,0xee);
        }
        if (run_size == 0) {
          run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr;
          run_section = (*(uint *)&map_views_block.diffeng_scan_section);
          run_size = (int)(*(int * *)&map_views_block.diffeng_scan_ptr) - (int)mirror_start;
        }
        if (0x400 < run_size) {
          err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_difflength_diff_engine_message_length_2,(iptr)&str_d_ss_franklinton_difference_engine_cpp_3,0xf7);
        }
        if ((flags & 0x10) == 0) {
          crt_memcpy(mirror_start,run_start,run_size);
        }
        rle.src = (u8 *)run_start;
        rle.src_end = (u8 *)run_start + run_size;
        rle.dst = (u8 *)(out_ptr + 3);
        rle.dst_end = (u8 *)&map_views_block.diffeng_mirror_size;  /* (the packet buffer's end) */
        encoded = diffeng_rle_encode(&rle);
        if (encoded == 0) {
          crt_memcpy(out_ptr + 3,run_start,run_size);
          *out_ptr = run_section;
          out_ptr[1] = run_offset;
          out_ptr[2] = run_size;
          out_ptr = (uint *)((int)out_ptr + run_size + 0xc);
        }
        else {
          *out_ptr = run_section | 0x8000;
          out_ptr[1] = run_offset;
          out_ptr[2] = rle.out_len;
          out_ptr = (uint *)((int)out_ptr + rle.out_len + 0xc);
        }
        record_count++;
        bytes_changed += run_size;
        if (diffeng_packet_flush_size <= (int)((char *)out_ptr - (char *)&map_views_block.diffeng_packet_i32)) {
          map_views_block.diffeng_packet_size_i32 = (i32)((char *)out_ptr - (char *)&map_views_block.diffeng_packet_i32);
          map_views_block.diffeng_packet_record_count_i32 = record_count;
          net_send_message(0x5c,dest,0,0,0,0,0,0,0,(iptr)&map_views_block.diffeng_packet_i32);
          if ((flags & 1) != 0) {
            XD_FlushSendBuffer(5000);
          }
          out_ptr = ((uint *)&map_views_block.diffeng_packet_payload);
          record_count = 0;
        }
        if ((((flags & 4) != 0) && (encoded = timer_ticks(), deadline < encoded)) ||
           (((flags & 8) != 0 && (runs_left = runs_left - 1, runs_left == 0)))) break;
      }
    } while (start_ptr != (*(int * *)&map_views_block.diffeng_scan_ptr));
    if (((uint *)&map_views_block.diffeng_packet_payload) < out_ptr) {
      map_views_block.diffeng_packet_size_i32 = (i32)((char *)out_ptr - (char *)&map_views_block.diffeng_packet_i32);
      map_views_block.diffeng_packet_record_count_i32 = record_count;
      net_send_message(0x5c,dest,0,0,0,0,0,0,0,(iptr)&map_views_block.diffeng_packet_i32);
      if ((flags & 1) != 0) {
        XD_FlushSendBuffer(5000);
      }
    }
    map_views_block.diffeng_scan_offset_i32 = (int)cur_ptr - (iptr)diffeng_section_table[(*(uint *)&map_views_block.diffeng_scan_section)].data_ptr;
  }
  return bytes_changed;
}

/* 004b401c auto */
void __cdecl parley_wnd_global_init(void)

{
  parley_wnd_static_init();
  parley_wnd_reg_atexit();
  return;
}

/* 004b4036 auto */
void __cdecl parley_wnd_static_init(void)

{
  parley_wnd_ctor(&parley_wnd_block,(RECT *)&prefs_main_menu_choice_block.prefs_parley_wnd_rect);
  return;
}

/* 004b405b auto */
void __cdecl parley_wnd_reg_atexit(void)

{
  _atexit(parley_wnd_static_dtor);
  return;
}

/* 004b4078 auto */
void __cdecl parley_wnd_static_dtor(void)

{
  parley_wnd_dtor(&parley_wnd_block);
  return;
}

/* 004b4092 auto */
void __cdecl chat_wnd_global_init(void)

{
  chat_wnd_static_init();
  chat_wnd_reg_atexit();
  return;
}

/* 004b40ac auto */
void __cdecl chat_wnd_static_init(void)

{
  parley_wnd_ctor(&chat_wnd_block,(RECT *)&prefs_main_menu_choice_block.prefs_chat_wnd_rect);
  return;
}

/* 004b40d1 auto */
void __cdecl chat_wnd_reg_atexit(void)

{
  _atexit(chat_wnd_static_dtor);
  return;
}

/* 004b40ee auto */
void __cdecl chat_wnd_static_dtor(void)

{
  parley_wnd_dtor(&chat_wnd_block);
  return;
}

/* 004b4108 auto */
ParleyWindow * __thiscall parley_wnd_ctor(ParleyWindow *self,RECT *rect)

{
  u8 *field;
  int width;
  int i;
  i32 seh_prev;
  i8 *seh_handler;
  i32 unwind_state;
  
  unwind_state = 0xffffffff;
  surfwnd_ctor(self);
  unwind_state = 0;
  self->vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 1);
  self->init_rect = (u32)rect;
  self->closed = 1;
  prefs_load_civ2dat();
  width = rect_width((RECT *)self->init_rect);
  if (width < 1) {
    self->wnd_x = 0;
    self->wnd_y = 0;
  }
  else {
    self->wnd_x = *(u32 *)self->init_rect;
    self->wnd_y = *(u32 *)(self->init_rect + 4);
    if (width < 0x321) {
      self->large_layout = 0;
    }
    else {
      self->large_layout = 1;
    }
  }
  self->close_button = 0;
  self->send_button = 0;
  self->clear_button = 0;
  self->conceal_button = 0;
  self->reveal_button = 0;
  self->mode_button = 0;
  self->intel_button = 0;
  self->mode2_button = 0;
  self->never_mind_button = 0;
  self->accept_button = 0;
  self->no_thanks_button = 0;
  self->counter_button = 0;
  for (i = 0; i < 8; i++) {
    field = (u8 *)&self->send_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->mute_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->send_to_civs[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->muted_civs[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  self->remain_anonymous = 0;
  self->mute_anonymous = 0;
  self->broadcast_all = 0;
  self->broadcast_checkbox = 0;
  self->anonymous_checkbox = 0;
  self->mute_anonymous_checkbox = 0;
  self->desc_edit = 0;
  self->other_civ = 0xffffffff;
  self->received_offer = 0;
  self->exchange_kind[0] = 0;
  self->exchange_kind[1] = 1;
  self->demand_kind = 0;
  self->selected_action = 0xffffffff;
  self->gift_kind = 0;
  self->map_choice[1] = 0;
  self->map_choice[0] = self->map_choice[1];
  self->gold_amount[1] = 0;
  self->gold_amount[0] = self->gold_amount[1];
  self->proposal_kind = 0;
  self->treaty_kind = 0;
  self->side_selected[1] = 0;
  self->side_selected[0] = self->side_selected[1];
  self->caption_buttons[0].hotspot = 0;
  self->caption_buttons[1].hotspot = 0;
  self->caption_buttons[2].hotspot = 0;
  self->chat_input_edit = 0;
  self->chat_log_edit = 0;
  (*(i32 *)&chat_input_buffer) = 0;
  (*(i32 *)&chat_log_buffer) = 0;
  font_size16_bold_block.chat_log_capacity = 0;
  for (i = 0; i < 2; i++) {
    field = (u8 *)&self->side_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->side_radios[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->gold_edits[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->list_scrollbars[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->list_hotspots[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->sort_by_unit_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->sort_by_city_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->info_show_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&self->info_hide_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  self->remote_units_revealed = '\0';
  self->local_units_revealed = '\0';
  self->local_cities_revealed = '\0';
  self->remote_cities_revealed = '\0';
  return self;
}

/* 004b4593 auto */
void __thiscall parley_wnd_dtor(ParleyWindow *self)

{
  int created;
  size_t log_len;
  FileObject log_file;
  i32 seh_prev;
  i8 *seh_handler;
  uint unwind_state;
  
  self->vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 1);
  unwind_state = 0;
  if (self->mode == 4) {
    file_obj_init(&log_file);
    unwind_state = (unwind_state & 0xffffff00 | (byte)(1));
    if ((prefs_main_menu_choice_block.prefs_clear_chat_on_boot != 0) && (chat_log_buffer != (char *)0x0)) {
      *chat_log_buffer = '\0';
      created = file_obj_create(&log_file,(const char *)&str_chatlog_txt_2);
      if (created != 0) {
        log_len = _strlen(chat_log_buffer);
        file_obj_write(&log_file,(iptr)chat_log_buffer,log_len);
        file_map_close(&log_file);
      }
    }
    if (chat_log_buffer != (char *)0x0) {
      operator_delete(chat_log_buffer);
      chat_log_buffer = (char *)0x0;
    }
    if (chat_input_buffer != (void *)0x0) {
      operator_delete(chat_input_buffer);
      chat_input_buffer = (void *)0x0;
    }
    unwind_state = unwind_state & 0xffffff00;
    /* inlined unwind funclet 004b4705 */ { file_obj_close(&log_file); }
  }
  parley_wnd_destroy_controls(self);
  wnd_destroy_with_surface(self);
  unwind_state = 0xffffffff;
  /* inlined unwind funclet 004b4711 */ { surfwnd_dtor(*(SurfaceWindow **)&self); }
  return;
}

/* 004b4735 auto */
void __thiscall parley_wnd_create(ParleyWindow *self,u32 mode)

{
  int file_size;
  i32 height;
  i32 width;
  i8 *palette;
  i8 *parent;
  u8 *wnd2;
  i8 log_file[160];
  i32 unwind_state;
  
  unwind_state = 0xffffffff;
  self->closed = 1;
  if (self == (ParleyWindow *)0x0) {
    wnd2 = (u8 *)0x0;
  }
  else {
    wnd2 = ((u8 *)self + 0x48);
  }
  wnd_set_active_child((iptr)wnd2);
  self->mode = mode;
  if (self->mode != 4) goto pick_size_mode;
  (*(UINT *)&font_size16_bold_block.chat_log_capacity) = GetPrivateProfileIntA
                           (str_civilization_gold_7,str_chatshowsize,-1,
                            str_civ_ini_7);
  if (((*(UINT *)&font_size16_bold_block.chat_log_capacity) == 0xffffffff) || ((int)(*(UINT *)&font_size16_bold_block.chat_log_capacity) < 0x2000)) {
    WritePrivateProfileStringA
              (str_civilization_gold_8,str_chatshowsize_2,((i8 *)&chat_show_size_default_str),str_civ_ini_8
              );
    (*(UINT *)&font_size16_bold_block.chat_log_capacity) = 0x2000;
  }
  else if (0xdfff < (int)(*(UINT *)&font_size16_bold_block.chat_log_capacity)) {
    WritePrivateProfileStringA
              (str_civilization_gold_9,str_chatshowsize_3,str_57344,
               str_civ_ini_9);
    (*(UINT *)&font_size16_bold_block.chat_log_capacity) = 0xe000;
  }
  if ((*&chat_log_buffer) == (i8 *)0x0) {
    (*&chat_log_buffer) = (i8 *)(operator_new((*(UINT *)&font_size16_bold_block.chat_log_capacity) + 1));
  }
  _memset((*&chat_log_buffer),0,(*(UINT *)&font_size16_bold_block.chat_log_capacity) + 1);
  if (chat_input_buffer == (void *)0x0) {
    chat_input_buffer = operator_new(0x101);
  }
  _memset(chat_input_buffer,0,0x101);
  file_obj_init((FileObject *)log_file);
  unwind_state = 0;
  if (prefs_main_menu_choice_block.prefs_clear_chat_on_new_game == 0) {
    file_size = file_obj_open((FileObject *)log_file,(const char *)&str_chatlog_txt_3);
    if (file_size == 0) goto clear_chat_buffer;
    file_size = file_obj_get_size((FileObject *)log_file);
    if ((int)(*(UINT *)&font_size16_bold_block.chat_log_capacity) < file_size) {
      file_obj_seek_cur((FileSeekView *)log_file,file_size - (*(UINT *)&font_size16_bold_block.chat_log_capacity));
    }
    file_obj_read((FileObject *)log_file,*&chat_log_buffer,(*(UINT *)&font_size16_bold_block.chat_log_capacity));
    file_map_close((FileObject *)log_file);
  }
  else {
clear_chat_buffer:
    *(*&chat_log_buffer) = 0;
  }
  unwind_state = 0xffffffff;
  /* inlined unwind funclet 004b4be3 */ { file_obj_close((FileObject *)&log_file); }
pick_size_mode:
  if ((self->wnd_x == 0) && (self->wnd_y == 0)) {
    parley_wnd_pick_size_mode(self);
  }
  crt_strcpy((char *)((u8 *)self + 0x15c),((i8 *)&parley_wnd_empty_title_str));
  if (self->mode == 4) {
    parent = ((i8 *)&main_frame_wnd_block.main_frame_wnd);
    palette = ((i8 *)&game_palette_block.game_palette);
    height = rect_height((RECT *)self->init_rect);
    width = rect_width((RECT *)self->init_rect);
    wnd_create_with_surface
              (self,0,0x602,*(i32 *)self->init_rect,
               *(i32 *)(self->init_rect + 4),width,height,(iptr)palette,(iptr)parent);
  }
  else {
    parent = ((i8 *)&main_frame_wnd_block.main_frame_wnd);
    palette = ((i8 *)&game_palette_block.game_palette);
    height = rect_height((RECT *)self->init_rect);
    width = rect_width((RECT *)self->init_rect);
    wnd_create_with_surface
              (self,0,0x602,*(i32 *)self->init_rect,
               *(i32 *)(self->init_rect + 4),width,height,(iptr)palette,(iptr)parent);
  }
  parley_wnd_calc_rect(self);
  wnd_move_to_rect((WndBase *)((u8 *)self + 0x48),self->init_rect);
  self->built_layout = 0xffffffff;
  self->left_kind = parley_mode3_left_kind;
  self->right_kind = parley_mode3_right_kind;
  self->shown_left_kind = 0x15;
  self->shown_right_kind = 0x15;
  wnd_set_close_handler((WndHandlers *)((u8 *)self + 0x58),diplo_wnd_close_active);
  wnd_set_redraw_callback(self,parley_wnd_redraw);
  wnd_set_move_handler((WndHandlers *)((u8 *)self + 0x58),(WndMouseFn)parley_wnd_on_move);
  wnd_set_keydown_handler((WndHandlers *)((u8 *)self + 0x58),parley_wnd_on_key);
  parley_wnd_calc_layout(self);
  parley_wnd_sync_sides(self);
  if (((self->mode == 3) || (self->mode == 2)) || (self->mode == 1)) {
    ctrl_disable((Button *)self->never_mind_button);
  }
  parley_update_buttons(self);
  return;
}

/* 004b4c09 auto */
void __cdecl parley_wnd_on_move(void)

{
  iptr active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  wnd_get_rect_in_parent((WndBase *)((iptr)wnd2 + 0x48),(iptr)wnd2->init_rect);
  wnd2->wnd_x = *(i32 *)wnd2->init_rect;
  wnd2->wnd_y = *(i32 *)((int)wnd2->init_rect + 4);
  return;
}

/* 004b4c81 auto */
void __thiscall parley_wnd_pick_size_mode(ParleyWindow *self)

{
  iptr active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  wnd2->large_layout = 0;
  if (999 < caption_font_block.screen_width) {
    wnd2->large_layout = 1;
  }
  parley_wnd_calc_rect(self);
  return;
}

/* 004b4cf0 auto */
void __thiscall parley_wnd_calc_rect(ParleyWindow *self)

{
  int size;
  int font_height;
  int height2;
  int width2;
  u32 top;
  u32 left;
  
  if (self->large_layout == 0) {
    width2 = 0x208;
  }
  else {
    width2 = 800;
  }
  if (self->large_layout == 0) {
    height2 = 0x14f;
  }
  else {
    height2 = 0x1cc;
  }
  if (self->large_layout == 0) {
    font_height = font_cached_height((GameFont *)((i8 *)&font_size20));
  }
  else {
    font_height = font_cached_height((GameFont *)((i8 *)&font_size30));
  }
  self->caption_height = panel_margin_y * 2 + panel_bevel_width * 2 + font_height;
  width2 = width2 + (panel_padding_x + panel_margin_x) * 2 + 6;
  height2 = height2 + panel_margin_y * 2 + self->caption_height * 2 + 0xe;
  size = rect_width((RECT *)self->init_rect);
  if (size == 0) {
    size = wnd_client_width(&main_frame_wnd_block.main_frame_wnd);
    self->wnd_x = (size >> 1) - (width2 >> 1);
    left = self->wnd_x;
    size = wnd_client_height(&main_frame_wnd_block.main_frame_wnd);
    self->wnd_y = (size >> 1) - (height2 >> 1);
    top = self->wnd_y;
  }
  else {
    left = self->wnd_x;
    top = self->wnd_y;
  }
  SetRect((LPRECT)self->init_rect,left,top,width2 + left,height2 + top);
  wnd_set_caption_height((WndBase *)((u8 *)self + 0x48),self->caption_height);
  return;
}

/* 004b4e8a auto */
void __thiscall parley_wnd_calc_layout(ParleyWindow *self)

{
  u32 dim;
  int font_height;
  
  dim = surface_get_width(self);
  self->width = dim;
  dim = surface_get_height(self);
  self->height = dim;
  if (self->large_layout == 0) {
    font_height = font_cached_height((GameFont *)((i8 *)&font_size20));
  }
  else {
    font_height = font_cached_height((GameFont *)((i8 *)&font_size30));
  }
  self->caption_height = panel_margin_y * 2 + panel_bevel_width * 2 + font_height;
  SetRect((LPRECT)&self->client_left,0,0,self->width,self->height);
  SetRect((LPRECT)&self->panel_left,0,0,self->width + (panel_padding_x + panel_margin_x) * -2,
          self->height - (panel_margin_y * 2 + self->caption_height * 2 + panel_padding_x));
  OffsetRect((LPRECT)&self->panel_left,panel_padding_x + panel_margin_x,self->caption_height + panel_margin_y);
  return;
}

/* 004b4fb2 auto */
void __thiscall parley_wnd_sync_sides(ParleyWindow *self)

{
  if (self->built_layout == self->large_layout) {
    if (self->shown_left_kind != self->left_kind) {
      parley_add_side_dialog(self,self->left_kind,0);
      self->shown_left_kind = self->left_kind;
    }
    if (self->shown_right_kind != self->right_kind) {
      parley_add_side_dialog(self,self->right_kind,1);
      self->shown_right_kind = self->right_kind;
    }
  }
  else {
    parley_wnd_destroy_controls(self);
    parley_wnd_create_controls(self);
    parley_add_side_dialog(self,self->left_kind,0);
    parley_add_side_dialog(self,self->right_kind,1);
    self->shown_left_kind = self->left_kind;
    self->shown_right_kind = self->right_kind;
    self->built_layout = self->large_layout;
  }
  return;
}

/* 004b50cf auto */
void __thiscall parley_wnd_destroy_controls(ParleyWindow *self)

{
  u8 *slot;
  int i;
  
  if (self->received_offer != 0) {
    operator_delete((void *)self->received_offer);
    self->received_offer = 0;
  }
  for (i = 0; i < 3; i++) {
    if (self->caption_buttons[i].hotspot != 0) {
      if ((Hotspot *)self->caption_buttons[i].hotspot != (Hotspot *)0x0) {
        hotspot_scalar_dtor((Hotspot *)self->caption_buttons[i].hotspot,1);
      }
      slot = (u8 *)&self->caption_buttons[i].hotspot;
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  if (self->close_button != 0) {
    if ((Button *)self->close_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->close_button,1);
    }
    self->close_button = 0;
  }
  if (self->send_button != 0) {
    if ((Button *)self->send_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->send_button,1);
    }
    self->send_button = 0;
  }
  if (self->clear_button != 0) {
    if ((Button *)self->clear_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->clear_button,1);
    }
    self->clear_button = 0;
  }
  if (self->reveal_button != 0) {
    if ((Button *)self->reveal_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->reveal_button,1);
    }
    self->reveal_button = 0;
  }
  if (self->conceal_button != 0) {
    if ((Button *)self->conceal_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->conceal_button,1);
    }
    self->conceal_button = 0;
  }
  if (self->mode_button != 0) {
    if ((Button *)self->mode_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->mode_button,1);
    }
    self->mode_button = 0;
  }
  if (self->intel_button != 0) {
    if ((Button *)self->intel_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->intel_button,1);
    }
    self->intel_button = 0;
  }
  if (self->mode2_button != 0) {
    if ((Button *)self->mode2_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->mode2_button,1);
    }
    self->mode2_button = 0;
  }
  if (self->never_mind_button != 0) {
    if ((Button *)self->never_mind_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->never_mind_button,1);
    }
    self->never_mind_button = 0;
  }
  if (self->accept_button != 0) {
    if ((Button *)self->accept_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->accept_button,1);
    }
    self->accept_button = 0;
  }
  if (self->no_thanks_button != 0) {
    if ((Button *)self->no_thanks_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->no_thanks_button,1);
    }
    self->no_thanks_button = 0;
  }
  if (self->counter_button != 0) {
    if ((Button *)self->counter_button != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->counter_button,1);
    }
    self->counter_button = 0;
  }
  if (self->chat_log_edit != 0) {
    if ((Button *)self->chat_log_edit != (Button *)0x0) {
      edit_scalar_dtor((EditControl *)self->chat_log_edit,1);
    }
    self->chat_log_edit = 0;
  }
  if (self->chat_input_edit != 0) {
    if ((Button *)self->chat_input_edit != (Button *)0x0) {
      edit_scalar_dtor((EditControl *)self->chat_input_edit,1);
    }
    self->chat_input_edit = 0;
  }
  if (self->anonymous_checkbox != 0) {
    if ((Checkbox *)self->anonymous_checkbox != (Checkbox *)0x0) {
      checkbox_scalar_dtor((Checkbox *)self->anonymous_checkbox,1);
    }
    self->anonymous_checkbox = 0;
  }
  if (self->mute_anonymous_checkbox != 0) {
    if ((Checkbox *)self->mute_anonymous_checkbox != (Checkbox *)0x0) {
      checkbox_scalar_dtor((Checkbox *)self->mute_anonymous_checkbox,1);
    }
    self->mute_anonymous_checkbox = 0;
  }
  if (self->desc_edit != 0) {
    if ((Button *)self->desc_edit != (Button *)0x0) {
      edit_scalar_dtor((EditControl *)self->desc_edit,1);
    }
    self->desc_edit = 0;
  }
  if (self->broadcast_checkbox != 0) {
    if ((Checkbox *)self->broadcast_checkbox != (Checkbox *)0x0) {
      checkbox_scalar_dtor((Checkbox *)self->broadcast_checkbox,1);
    }
    self->broadcast_checkbox = 0;
  }
  for (i = 0; i < 8; i++) {
    if (self->send_checkboxes[i] != 0) {
      if ((Checkbox *)self->send_checkboxes[i] != (Checkbox *)0x0) {
        checkbox_scalar_dtor((Checkbox *)self->send_checkboxes[i],1);
      }
      slot = (u8 *)&self->send_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->mute_checkboxes[i] != 0) {
      if ((Checkbox *)self->mute_checkboxes[i] != (Checkbox *)0x0) {
        checkbox_scalar_dtor((Checkbox *)self->mute_checkboxes[i],1);
      }
      slot = (u8 *)&self->mute_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  for (i = 0; i < 2; i++) {
    if (self->side_checkboxes[i] != 0) {
      if ((Checkbox *)self->side_checkboxes[i] != (Checkbox *)0x0) {
        checkbox_scalar_dtor((Checkbox *)self->side_checkboxes[i],1);
      }
      slot = (u8 *)&self->side_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->side_radios[i] != 0) {
      if ((RadioGroup *)self->side_radios[i] != (RadioGroup *)0x0) {
        radiogrp_scalar_dtor((RadioGroup *)self->side_radios[i],1);
      }
      slot = (u8 *)&self->side_radios[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->gold_edits[i] != 0) {
      if ((Button *)self->gold_edits[i] != (Button *)0x0) {
        edit_scalar_dtor((EditControl *)self->gold_edits[i],1)
        ;
      }
      slot = (u8 *)&self->gold_edits[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->list_scrollbars[i] != 0) {
      if ((Button *)self->list_scrollbars[i] != (Button *)0x0) {
        scrollbar_scalar_dtor((Scrollbar *)self->list_scrollbars[i],1)
        ;
      }
      slot = (u8 *)&self->list_scrollbars[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->list_hotspots[i] != 0) {
      if ((Hotspot *)self->list_hotspots[i] != (Hotspot *)0x0) {
        hotspot_scalar_dtor((Hotspot *)self->list_hotspots[i],1);
      }
      slot = (u8 *)&self->list_hotspots[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->sort_by_unit_buttons[i] != 0) {
      if ((Button *)self->sort_by_unit_buttons[i] != (Button *)0x0) {
        ctrl_scalar_dtor((Button *)self->sort_by_unit_buttons[i],1)
        ;
      }
      slot = (u8 *)&self->sort_by_unit_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->sort_by_city_buttons[i] != 0) {
      if ((Button *)self->sort_by_city_buttons[i] != (Button *)0x0) {
        ctrl_scalar_dtor((Button *)self->sort_by_city_buttons[i],1)
        ;
      }
      slot = (u8 *)&self->sort_by_city_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->info_show_buttons[i] != 0) {
      if ((Button *)self->info_show_buttons[i] != (Button *)0x0) {
        ctrl_scalar_dtor
                  ((Button *)self->info_show_buttons[i],1);
      }
      slot = (u8 *)&self->info_show_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (self->info_hide_buttons[i] != 0) {
      if ((Button *)self->info_hide_buttons[i] != (Button *)0x0) {
        ctrl_scalar_dtor
                  ((Button *)self->info_hide_buttons[i],1);
      }
      slot = (u8 *)&self->info_hide_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  return;
}

/* 004b5c93 auto */
void __thiscall parley_wnd_create_controls(ParleyWindow *self)

{
  u8 *slot_ptr;
  u32 wnd_width;
  int y_delta;
  LONG xLeft;
  i32 sprite_dim;
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  int yTop;
  EditControl *new_edit;
  iptr label;
  u8 *counter_parent;
  u8 *no_thanks_parent;
  u8 *accept_parent;
  u8 *never_mind_parent;
  u8 *mode2_parent;
  u8 *mode_parent;
  u8 *intel_parent;
  u8 *reveal_parent;
  u8 *conceal_parent;
  int footer_line_h;
  u8 *clear_parent;
  u8 *send_parent;
  u8 *input_parent;
  int input_font_h4;
  int input_font_h3;
  int input_font_h2;
  int input_font_h1;
  u8 *history_parent;
  int hist_font_h4;
  int hist_font_h3;
  int hist_font_h2;
  int hist_font_h1;
  u8 *close_parent;
  iptr close_label;
  int close_line_h2;
  int close_line_h;
  u8 *caption_parent;
  uptr counter_button;
  uptr no_thanks_button;
  uptr accept_button;
  uptr never_mind_button;
  uptr mode2_button;
  uptr mode_button;
  uptr intel_button;
  uptr reveal_button;
  uptr conceal_button;
  uptr clear_button;
  uptr send_button;
  i8 *input_font;
  uptr input_edit;
  i8 *history_font;
  uptr history_edit;
  uptr close_btn;
  i8 *button_font;
  i32 hotspot;
  int ctrl_top;
  int caption_x;
  int i;
  tagRECT rect;
  int ctrl_height;
  i32 input_style;
  i32 unused_10;
  i8 *eh_prev;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  caption_x = panel_padding_x;
  for (i = 0; i < 3; i++) {
    if (i == 0) {
      self->caption_buttons[0].sprite = (Sprite *)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite);
    }
    else if (i == 1) {
      self->caption_buttons[1].sprite = (Sprite *)((i8 *)&caption_zoom_out_sprite);
    }
    else {
      self->caption_buttons[i].sprite = (Sprite *)((i8 *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite);
    }
    self->caption_buttons[i].id = i + 1;
    sprite_dim = sprite_width
                      (self->caption_buttons[i].sprite);
    self->caption_buttons[i].width = sprite_dim;
    sprite_dim = sprite_height
                      (self->caption_buttons[i].sprite);
    self->caption_buttons[i].height = sprite_dim;
    slot_ptr = (u8 *)&self->caption_buttons[i].hotspot;
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
    ctrl_width = (int)(self->caption_buttons[i].width * self->caption_height) / 0x18;
    ctrl_height = (int)(self->caption_buttons[i].height * self->caption_height) / 0x18;
    ctrl_left = ((int)self->caption_height >> 1) - (ctrl_height >> 1);
    SetRect(&rect,caption_x,ctrl_left,ctrl_width + caption_x,ctrl_left + ctrl_height);
    new_ctrl = (Button *)(operator_new(0x40));
    eh_state = 0;
    if (new_ctrl == (Button *)0x0) {
      hotspot = 0;
    }
    else {
      hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
    }
    eh_state = 0xffffffff;
    self->caption_buttons[i].hotspot = hotspot;
    if (self == (ParleyWindow *)0x0) {
      caption_parent = (u8 *)0x0;
    }
    else {
      caption_parent = ((u8 *)self + 0x48);
    }
    hotspot_create
              ((Hotspot *)self->caption_buttons[i].hotspot,(iptr)caption_parent,
               self->caption_buttons[i].id + 1000,(iptr)&rect);
    hotspot_set_click_callback((Hotspot *)self->caption_buttons[i].hotspot,parley_wnd_on_caption_click);
    caption_x = caption_x + ctrl_width + panel_margin_x;
  }
  if (self->large_layout == 0) {
    button_font = ((i8 *)&font_size16);
  }
  else {
    button_font = ((i8 *)&font_size24);
  }
  ui_set_button_font((iptr)button_font);
  new_ctrl = (Button *)(operator_new(0x3c));
  eh_state = 1;
  if (new_ctrl == (Button *)0x0) {
    close_btn = 0;
  }
  else {
    close_btn = (uptr)(button_ctor(new_ctrl));
  }
  eh_state = 0xffffffff;
  self->close_button = close_btn;
  ctrl_width = panel_bevel_width + panel_margin_x + 2;
  if (self->large_layout == 0) {
    close_line_h = font_line_spacing((GameFont *)((i8 *)&font_size16));
  }
  else {
    close_line_h = font_line_spacing((GameFont *)((i8 *)&font_size24));
  }
  yTop = self->height - ((panel_margin_y + panel_padding_y) * 2 + close_line_h);
  wnd_width = self->width;
  y_delta = panel_margin_x * 2;
  ctrl_left = panel_bevel_width * 2;
  if (self->large_layout == 0) {
    close_line_h2 = font_line_spacing((GameFont *)((i8 *)&font_size16));
  }
  else {
    close_line_h2 = font_line_spacing((GameFont *)((i8 *)&font_size24));
  }
  ctrl_height = panel_padding_y * 2 + close_line_h2;
  SetRect(&rect,ctrl_width,yTop,(wnd_width - (y_delta + ctrl_left + 5)) + ctrl_width,yTop + ctrl_height);
  if (self->mode == 4) {
    close_label = (iptr)(label_string(label_string_ids[0x2cb]));
  }
  else {
    close_label = (iptr)(label_string(label_string_ids[0x2cc]));
  }
  if (self == (ParleyWindow *)0x0) {
    close_parent = (u8 *)0x0;
  }
  else {
    close_parent = ((u8 *)self + 0x48);
  }
  button_create
            ((Button *)self->close_button,(iptr)close_parent,0x3ed,(iptr)&rect,(const char *)close_label);
  button_set_callback((Button *)self->close_button,diplo_wnd_escape_close);
  new_edit = (EditControl *)(operator_new(0x48));
  eh_state = 2;
  if (new_edit == (EditControl *)0x0) {
    history_edit = 0;
  }
  else {
    history_edit = (uptr)(edit_ctor(new_edit));
  }
  eh_state = 0xffffffff;
  self->chat_log_edit = history_edit;
  ctrl_left = self->panel_left + panel_margin_x;
  ctrl_width = rect_width((RECT *)&self->panel_left);
  ctrl_width += panel_margin_x * -2;
  if (self->mode == 4) {
    if (self->large_layout == 0) {
      hist_font_h3 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      hist_font_h3 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    ctrl_top = self->panel_bottom + (panel_margin_y + hist_font_h3) * -9;
    if (self->large_layout == 0) {
      hist_font_h4 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      hist_font_h4 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    ctrl_height = hist_font_h4 * 9 + panel_margin_y * 8;
  }
  else {
    if (self->large_layout == 0) {
      hist_font_h1 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      hist_font_h1 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    ctrl_top = self->panel_bottom + (panel_margin_y + hist_font_h1) * -6;
    if (self->large_layout == 0) {
      hist_font_h2 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      hist_font_h2 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    ctrl_height = panel_margin_y * 5 + hist_font_h2 * 6;
  }
  SetRect(&rect,ctrl_left,ctrl_top,ctrl_width + ctrl_left,ctrl_top + ctrl_height);
  if (self->large_layout == 0) {
    history_font = ((i8 *)&font_size14);
  }
  else {
    history_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((iptr)history_font);
  if (self == (ParleyWindow *)0x0) {
    history_parent = (u8 *)0x0;
  }
  else {
    history_parent = ((u8 *)self + 0x48);
  }
  edit_create_styled
            ((EditControl *)self->chat_log_edit,(iptr)history_parent,0x3ee,(iptr)&rect,((const char *)(*(iptr *)&chat_log_buffer)),
             0x122,0);
  edit_set_max_len((EditControl *)self->chat_log_edit,0x2000);
  edit_set_callback((EditControl *)self->chat_log_edit,parley_edit_escape_filter);
  ctrl_show_and_redraw((Control *)self->chat_log_edit);
  new_edit = (EditControl *)(operator_new(0x48));
  eh_state = 3;
  if (new_edit == (EditControl *)0x0) {
    input_edit = 0;
  }
  else {
    input_edit = (uptr)(edit_ctor(new_edit));
  }
  eh_state = 0xffffffff;
  self->chat_input_edit = input_edit;
  ctrl_width -= ctrl_width / 10;
  if (self->mode == 4) {
    if (self->large_layout == 0) {
      input_font_h3 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      input_font_h3 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    y_delta = panel_margin_y * -4 + input_font_h3 * -3;
    if (self->large_layout == 0) {
      input_font_h4 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      input_font_h4 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    ctrl_height = input_font_h4 * 3 + panel_margin_y * 3;
  }
  else {
    if (self->large_layout == 0) {
      input_font_h1 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      input_font_h1 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    y_delta = panel_margin_y * -3 + input_font_h1 * -2;
    if (self->large_layout == 0) {
      input_font_h2 = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      input_font_h2 = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    ctrl_height = panel_margin_y * 2 + input_font_h2 * 2;
  }
  ctrl_top += y_delta;
  SetRect(&rect,ctrl_left,ctrl_top,ctrl_width + ctrl_left,ctrl_top + ctrl_height);
  if (self->large_layout == 0) {
    input_font = ((i8 *)&font_size14);
  }
  else {
    input_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((iptr)input_font);
  if (self->mode == 4) {
    input_style = 0x200;
  }
  else {
    input_style = 0x220;
  }
  if (self == (ParleyWindow *)0x0) {
    input_parent = (u8 *)0x0;
  }
  else {
    input_parent = ((u8 *)self + 0x48);
  }
  edit_create_styled
            ((EditControl *)self->chat_input_edit,(iptr)input_parent,0x3ef,(iptr)&rect,((const char *)(*(iptr *)&chat_input_buffer)),
             input_style,0);
  edit_set_max_len((EditControl *)self->chat_input_edit,0x100);
  edit_set_callback((EditControl *)self->chat_input_edit,chat_input_on_key);
  ctrl_set_initial_focus((Control *)self->chat_input_edit);
  ctrl_show_and_redraw((Control *)self->chat_input_edit);
  new_ctrl = (Button *)(operator_new(0x3c));
  eh_state = 4;
  if (new_ctrl == (Button *)0x0) {
    send_button = 0;
  }
  else {
    send_button = (uptr)(button_ctor(new_ctrl));
  }
  xLeft = rect.right;
  eh_state = 0xffffffff;
  self->send_button = send_button;
  ctrl_width = ctrl_width / 9;
  ctrl_height = ctrl_height / 2;
  SetRect(&rect,rect.right,ctrl_top,ctrl_width + rect.right,ctrl_top + ctrl_height);
  if (self == (ParleyWindow *)0x0) {
    send_parent = (u8 *)0x0;
  }
  else {
    send_parent = ((u8 *)self + 0x48);
  }
  label = (iptr)(label_string(label_string_ids[0x2cd]));
  button_create
            ((Button *)self->send_button,(iptr)send_parent,0x3f0,(iptr)&rect,(const char *)label);
  button_set_callback((Button *)self->send_button,(CtrlNotifyFn)chat_send_button_on_click);
  new_ctrl = (Button *)(operator_new(0x3c));
  eh_state = 5;
  if (new_ctrl == (Button *)0x0) {
    clear_button = 0;
  }
  else {
    clear_button = (uptr)(button_ctor(new_ctrl));
  }
  eh_state = 0xffffffff;
  self->clear_button = clear_button;
  ctrl_top += ctrl_height;
  SetRect(&rect,xLeft,ctrl_top,ctrl_width + xLeft,ctrl_top + ctrl_height);
  if (self == (ParleyWindow *)0x0) {
    clear_parent = (u8 *)0x0;
  }
  else {
    clear_parent = ((u8 *)self + 0x48);
  }
  label = (iptr)(label_string(label_string_ids[0x2ce]));
  button_create
            ((Button *)self->clear_button,(iptr)clear_parent,0x41e,(iptr)&rect,(const char *)label);
  button_set_callback((Button *)self->clear_button,(CtrlNotifyFn)chat_clear_button_on_click);
  ctrl_top -= ctrl_height;
  if (self->large_layout == 0) {
    footer_line_h = font_line_spacing((GameFont *)((i8 *)&font_size16));
  }
  else {
    footer_line_h = font_line_spacing((GameFont *)((i8 *)&font_size24));
  }
  ctrl_height = panel_padding_y * 2 + footer_line_h;
  ctrl_top -= (panel_margin_y + ctrl_height);
  self->divider_y = ctrl_top - panel_margin_y;
  if (self->mode != 4) {
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 6;
    if (new_ctrl == (Button *)0x0) {
      conceal_button = 0;
    }
    else {
      conceal_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->conceal_button = conceal_button;
    ctrl_width = self->panel_left + panel_margin_x;
    ctrl_left = rect_width((RECT *)&self->panel_left);
    ctrl_left += panel_margin_x * -2;
    SetRect(&rect,ctrl_width,ctrl_top - ctrl_height,ctrl_left + ctrl_width,ctrl_top);
    if (self == (ParleyWindow *)0x0) {
      conceal_parent = (u8 *)0x0;
    }
    else {
      conceal_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x36d]));
    button_create
              ((Button *)self->conceal_button,(iptr)conceal_parent,0x431,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->conceal_button,(CtrlNotifyFn)parley_on_conceal_info);
    control_hide((Control *)self->conceal_button);
    ctrl_disable((Button *)self->conceal_button);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 7;
    if (new_ctrl == (Button *)0x0) {
      reveal_button = 0;
    }
    else {
      reveal_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->reveal_button = reveal_button;
    ctrl_width = self->panel_left + panel_margin_x;
    SetRect(&rect,ctrl_width,ctrl_top - ctrl_height,ctrl_left + ctrl_width,ctrl_top);
    if (self == (ParleyWindow *)0x0) {
      reveal_parent = (u8 *)0x0;
    }
    else {
      reveal_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x36e]));
    button_create
              ((Button *)self->reveal_button,(iptr)reveal_parent,0x432,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->reveal_button,(CtrlNotifyFn)parley_on_reveal_info);
    control_hide((Control *)self->reveal_button);
    ctrl_disable((Button *)self->reveal_button);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 8;
    if (new_ctrl == (Button *)0x0) {
      intel_button = 0;
    }
    else {
      intel_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->intel_button = intel_button;
    ctrl_width = self->panel_left + panel_margin_x;
    ctrl_left = rect_width((RECT *)&self->panel_left);
    ctrl_left = (ctrl_left + panel_margin_x * -2) / 3;
    SetRect(&rect,ctrl_width,ctrl_top,ctrl_left + ctrl_width,ctrl_top + ctrl_height);
    if (self == (ParleyWindow *)0x0) {
      intel_parent = (u8 *)0x0;
    }
    else {
      intel_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x68]));
    button_create
              ((Button *)self->intel_button,(iptr)intel_parent,0x428,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->intel_button,(CtrlNotifyFn)parley_on_check_intel);
    ctrl_enable((Button *)self->intel_button);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 9;
    if (new_ctrl == (Button *)0x0) {
      mode_button = 0;
    }
    else {
      mode_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->mode_button = mode_button;
    SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
    if (self == (ParleyWindow *)0x0) {
      mode_parent = (u8 *)0x0;
    }
    else {
      mode_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x2cf]));
    button_create
              ((Button *)self->mode_button,(iptr)mode_parent,0x3f1,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->mode_button,parley_on_mode_chosen);
    ctrl_enable((Button *)self->mode_button);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 10;
    if (new_ctrl == (Button *)0x0) {
      mode2_button = 0;
    }
    else {
      mode2_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->mode2_button = mode2_button;
    if (self == (ParleyWindow *)0x0) {
      mode2_parent = (u8 *)0x0;
    }
    else {
      mode2_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x344]));
    button_create
              ((Button *)self->mode2_button,(iptr)mode2_parent,0x427,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->mode2_button,parley_on_mode_chosen);
    ctrl_disable((Button *)self->mode2_button);
    control_hide((Control *)self->mode2_button);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 0xb;
    if (new_ctrl == (Button *)0x0) {
      never_mind_button = 0;
    }
    else {
      never_mind_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->never_mind_button = never_mind_button;
    SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
    if (self == (ParleyWindow *)0x0) {
      never_mind_parent = (u8 *)0x0;
    }
    else {
      never_mind_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x2d0]));
    button_create
              ((Button *)self->never_mind_button,(iptr)never_mind_parent,0x3f2,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->never_mind_button,(CtrlNotifyFn)parley_on_never_mind);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 0xc;
    if (new_ctrl == (Button *)0x0) {
      accept_button = 0;
    }
    else {
      accept_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->accept_button = accept_button;
    ctrl_width = self->panel_left + panel_margin_x;
    ctrl_left = rect_width((RECT *)&self->panel_left);
    ctrl_left = (ctrl_left + panel_margin_x * -2) / 3;
    SetRect(&rect,ctrl_width,ctrl_top,ctrl_left + ctrl_width,ctrl_top + ctrl_height);
    if (self == (ParleyWindow *)0x0) {
      accept_parent = (u8 *)0x0;
    }
    else {
      accept_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x2d1]));
    button_create
              ((Button *)self->accept_button,(iptr)accept_parent,0x3f2,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->accept_button,(CtrlNotifyFn)parley_on_accept_offer);
    control_hide((Control *)self->accept_button);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 0xd;
    if (new_ctrl == (Button *)0x0) {
      no_thanks_button = 0;
    }
    else {
      no_thanks_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->no_thanks_button = no_thanks_button;
    SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
    if (self == (ParleyWindow *)0x0) {
      no_thanks_parent = (u8 *)0x0;
    }
    else {
      no_thanks_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x2d2]));
    button_create
              ((Button *)self->no_thanks_button,(iptr)no_thanks_parent,0x3f2,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->no_thanks_button,(CtrlNotifyFn)parley_on_no_thanks);
    control_hide((Control *)self->no_thanks_button);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 0xe;
    if (new_ctrl == (Button *)0x0) {
      counter_button = 0;
    }
    else {
      counter_button = (uptr)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->counter_button = counter_button;
    SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
    if (self == (ParleyWindow *)0x0) {
      counter_parent = (u8 *)0x0;
    }
    else {
      counter_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x2d3]));
    button_create
              ((Button *)self->counter_button,(iptr)counter_parent,0x3f2,(iptr)&rect,(const char *)label);
    button_set_callback((Button *)self->counter_button,(CtrlNotifyFn)parley_on_counter_offer);
    control_hide((Control *)self->counter_button);
  }
  return;
}

/* 004b74c4 auto */
void __cdecl parley_wnd_on_caption_click(int control_id)

{
  iptr child_wnd;
  ParleyWindow *parley_wnd_ptr;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    parley_wnd_ptr = (ParleyWindow *)0x0;
  }
  else {
    parley_wnd_ptr = (ParleyWindow *)(child_wnd - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)parley_wnd_ptr + 0x48));
  if (control_id == 0x3e9) {
    diplo_wnd_close_active();
  }
  else if (control_id == 0x3ea) {
    if (0 < (int)parley_wnd_ptr->large_layout) {
      wnd_hide((WndBase *)((u8 *)parley_wnd_ptr + 0x48));
      parley_wnd_ptr->large_layout--;
      parley_wnd_calc_rect(parley_wnd_ptr);
      wnd_move_to_rect((WndBase *)((u8 *)parley_wnd_ptr + 0x48),parley_wnd_ptr->init_rect);
      window_show(parley_wnd_ptr);
    }
  }
  else if (((control_id == 0x3eb) && ((int)parley_wnd_ptr->large_layout < 1)) && (999 < caption_font_block.screen_width)) {
    wnd_hide((WndBase *)((u8 *)parley_wnd_ptr + 0x48));
    parley_wnd_ptr->large_layout++;
    parley_wnd_calc_rect(parley_wnd_ptr);
    wnd_move_to_rect((WndBase *)((u8 *)parley_wnd_ptr + 0x48),parley_wnd_ptr->init_rect);
    window_show(parley_wnd_ptr);
  }
  return;
}

/* 004b7645 auto */
/* WARNING: Removing unreachable block (ram,0x004b766b) */

void __cdecl parley_wnd_close(void)

{
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  diplo_wnd_close_active();
  return;
}

/* 004b768d auto */
/* WARNING: Removing unreachable block (ram,0x004b76b3) */

void __cdecl chat_wnd_close(void)

{
  wnd_set_active_child((iptr)&chat_wnd_block.window);
  diplo_wnd_close_active();
  return;
}

/* 004b7885 auto */
void __cdecl parley_wnd_redraw(void)

{
  iptr child_wnd;
  i32 caret_pos;
  ParleyWindow *wnd2;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (wnd2->closed == 0) {
    parley_wnd_calc_layout(wnd2);
    parley_wnd_build_title(wnd2);
    gfx_set_draw_target((iptr)wnd2);
    parley_wnd_draw_frame(wnd2);
    parley_wnd_sync_sides(wnd2);
    wnd_blit_surface(wnd2);
    window_show(wnd2);
    caret_pos = edit_get_text_end((EditControl *)wnd2->chat_log_edit);
    edit_set_caret((EditControl *)wnd2->chat_log_edit,caret_pos);
    caret_pos = edit_get_sel_end((EditControl *)wnd2->chat_input_edit);
    edit_set_caret((EditControl *)wnd2->chat_input_edit,caret_pos);
  }
  return;
}

/* 004b794a auto */
void __thiscall parley_wnd_draw_frame(ParleyWindow *self)

{
  int free_width;
  int title_width;
  i8 sprite_rect[16];
  int icon_height;
  int icon_width;
  int y2;
  int x2;
  GameFont *title_font2;
  int i;
  tagRECT rect;
  
  surface_reset_clip(self);
  crt_memcpy(&rect,&self->client_left,0x10);
  for (i = 0; i < panel_bevel_width; i++) {
    gfx_draw_bevel_rect((iptr)self,&rect,(uint)dlg_bevel_light_color,(uint)dlg_bevel_dark_color);
    rect_deflate((iptr)&rect,1,1);
  }
  parley_fill_background(self,&rect,1);
  rect_deflate((iptr)&rect,panel_padding_x + panel_bevel_width * -2,0);
  rect.top = rect.top + self->caption_height + panel_bevel_width * -2;
  free_width = rect_height((RECT *)&self->client_left);
  rect.bottom = (free_width - self->caption_height) + panel_bevel_width * -2;
  for (i = 0; i < panel_bevel_width; i++) {
    gfx_draw_bevel_rect((iptr)self,&rect,(uint)dlg_bevel_dark_color,(uint)dlg_bevel_light_color);
    rect_deflate((iptr)&rect,1,1);
  }
  parley_fill_background(self,&rect,2);
  x2 = panel_padding_x;
  for (i = 0; i < 3; i++) {
    icon_width = (int)(self->caption_buttons[i].width * self->caption_height) / 0x18;
    icon_height = (int)(self->caption_buttons[i].height * self->caption_height) / 0x18;
    y2 = ((int)self->caption_height >> 1) - (icon_height >> 1);
    SetRect(&rect,x2,y2,icon_width + x2,icon_height + y2);
    gfx_set_scale_ratio(self->caption_height,0x18);
    sprite_draw
              (self->caption_buttons[i].sprite,(iptr)sprite_rect,
               (iptr)self,x2,y2);
    gfx_reset_zoom_scale();
    x2 = x2 + panel_margin_x + icon_width;
  }
  if (self->large_layout == 0) {
    title_font2 = (GameFont *)((i8 *)&font_size20);
  }
  else {
    title_font2 = (GameFont *)((i8 *)&font_size30);
  }
  x2 -= panel_margin_x;
  free_width = (self->client_right - x2) - panel_padding_x;
  title_width = font_text_width(title_font2,self->title);
  x2 += (free_width / 2 - (title_width >> 1));
  y2 = panel_bevel_width + panel_margin_y;
  gfx_set_text_color_index(10);
  surface_draw_text
            (self,(iptr)title_font2,self->title,x2 + 2,y2 + 1,5);
  gfx_set_text_color_index(0x1a);
  surface_draw_text
            (self,(iptr)title_font2,self->title,x2 + 1,y2,5);
  surface_draw_text
            (self,(iptr)title_font2,self->title,x2,y2,5);
  surface_set_clip(self,(RECT *)&self->panel_left);
  return;
}

/* 004b7c90 auto */
void __thiscall parley_fill_background(ParleyWindow *self,RECT *rect,int area_kind)

{
  i32 height;
  i32 width;
  i32 tile_x;
  i32 tile_y;
  
  if ((dlg_frame_tile_surface == 0) || (area_kind != 1)) {
    if ((dlg_client_tile_surface == 0) || (area_kind != 2)) {
      gfx_fill_rect((iptr)self,rect,(uint)dlg_bg_color);
    }
    else {
      tile_y = 0;
      tile_x = 0;
      height = rect_height(rect);
      width = rect_width(rect);
      gfx_tile_fill((iptr)self,dlg_client_tile_surface,rect->left,rect->top
                         ,width,height,tile_x,tile_y);
    }
  }
  else {
    tile_y = 0;
    tile_x = 0;
    height = rect_height(rect);
    width = rect_width(rect);
    gfx_tile_fill((iptr)self,dlg_frame_tile_surface,rect->left,rect->top,
                       width,height,tile_x,tile_y);
  }
  return;
}

/* 004b7d72 auto */
void __thiscall parley_wnd_build_title(ParleyWindow *self)

{
  const char *text;
  iptr attitude_level;
  
  text_clear();
  if (self->mode == 4) {
    text = (label_string(label_string_ids[0x2d5]));
    text_append(text);
    crt_strcpy((char *)((u8 *)self + 0x15c),((i8 *)&map_views_block.text_buffer));
  }
  else {
    text = (label_string(label_string_ids[0x2d4]));
    text_append(text);
    text_append_space();
    text = (civ_name_plural(self->other_civ));
    text_append(text);
    text_append_space();
    text_append_lparen();
    if (*(char *)&civs[self->other_civ].attitude[human_civ[0]] == '\0') {
      attitude_level = 0;
    }
    else {
      attitude_level = diplo_attitude_level((uint)civs[self->other_civ].attitude[human_civ[0]]);
    }
    text_append_string_id(attitude_names[attitude_level]);
    text_append_rparen();
    crt_strcpy((char *)((u8 *)self + 0x15c),((i8 *)&map_views_block.text_buffer));
  }
  return;
}

/* 004b7eb6 auto */
/* WARNING: Removing unreachable block (ram,0x004b8008) */
/* WARNING: Removing unreachable block (ram,0x004b8051) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall parley_wnd_open(ParleyWindow *self,iptr other_civ,u32 mode)

{
  bool can_see_all;
  bool has_contact;
  iptr has_wonder;
  iptr child_wnd;
  i32 caret_pos;
  int civ;
  ParleyWindow *wnd2;
  
  if (mode == 4) {
    has_contact = false;
    if (((game.wonder_city[0x18] == -1) && (has_wonder = civ_has_wonder(human_civ[0],9), has_wonder == 0)
        ) && (game.reveal_map == '\0')) {
      can_see_all = false;
    }
    else {
      can_see_all = true;
    }
    for (civ = 1; civ < 8; civ++) {
      if ((((human_civ[0] != civ) &&
           ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (((can_see_all || (civ_saved_cursor_x_block.mp_chat_from_start != 0)) ||
          ((*(byte *)&civs[civ].treaties[human_civ[0]] & 1) != 0)))) {
        has_contact = true;
        break;
      }
    }
    if (!has_contact) {
      popup_game((const char *)&str_noforeignhuman);
      return;
    }
    wnd_set_active_child((iptr)&chat_wnd_block.window);
    net_session_block.chat_unread_count = 0;
    net_session_block.chat_missed_message_count_i32 = 0;
  }
  else {
    wnd_set_active_child((iptr)&parley_wnd_block.window);
    parley_wnd_block.selected_action = 0xffffffff;
    self->remote_units_revealed = '\0';
    self->local_units_revealed = '\0';
    self->local_cities_revealed = '\0';
    self->remote_cities_revealed = '\0';
    if (mode == 3) {
      parley_set_mode_sides(&parley_wnd_block,0);
    }
  }
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (wnd2->closed == 0) {
    window_show(wnd2);
  }
  else {
    ((u8 *)wnd2 + 0x0)[0x114] = '\0';
    ((u8 *)wnd2 + 0x0)[0x115] = '\0';
    ((u8 *)wnd2 + 0x0)[0x116] = '\0';
    ((u8 *)wnd2 + 0x0)[0x117] = '\0';
    wnd2->other_civ = other_civ;
    if (mode != 3) {
      wnd2->mode = mode;
      wnd2->left_kind = (((i32 *)&parley_mode_left_kinds))[mode * 2];
      wnd2->right_kind = (((i32 *)&parley_mode_right_kinds))[mode * 2];
    }
    parley_wnd_build_title(wnd2);
    ((u8 *)wnd2 + 0x1f8)[0] = 21;
    ((u8 *)wnd2 + 0x1f9)[0] = '\0';
    ((u8 *)wnd2 + 0x1fa)[0] = '\0';
    ((u8 *)wnd2 + 0x1fb)[0] = '\0';
    ((u8 *)wnd2 + 0x1fc)[0] = 21;
    ((u8 *)wnd2 + 0x1fd)[0] = '\0';
    ((u8 *)wnd2 + 0x1fe)[0] = '\0';
    ((u8 *)wnd2 + 0x1ff)[0] = '\0';
    wnd_repaint(wnd2);
    window_show(wnd2);
    caret_pos = edit_get_text_end((EditControl *)wnd2->chat_log_edit);
    edit_set_caret((EditControl *)wnd2->chat_log_edit,caret_pos);
    caret_pos = edit_get_sel_end((EditControl *)wnd2->chat_input_edit);
    edit_set_caret((EditControl *)wnd2->chat_input_edit,caret_pos);
  }
  snd_play_effect(((uint)map.seed + other_civ & 7) + 0x53,1,0,0);
  return;
}

/* 004b81dd auto */
/* WARNING: Removing unreachable block (ram,0x004b8211) */

void __cdecl parley_poll_net_replies(void)

{
  const char *civ_name;
  
  net_process_messages(&net_session_block,1,0);
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  if ((net_session_block.nm_parley_transaction_count == 0) || ((int)parley_wnd_block.mode != 2)) {
    if ((net_session_block.nm_parley_accept_offer_count == 0) || ((int)parley_wnd_block.mode != 1)) {
      if ((net_session_block.nm_parley_no_thanks_count == 0) || ((int)parley_wnd_block.mode != 1)) {
        if ((net_session_block.nm_parley_counter_offer_count != 0) && ((int)parley_wnd_block.mode == 1)) {
          net_session_block.nm_parley_counter_offer_count = 0;
          parley_wnd_block.mode = 2;
          parley_wnd_block.left_kind = parley_mode2_left_kind;
          parley_wnd_block.right_kind = parley_mode2_right_kind;
          parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
          parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
          parley_add_side_dialog(&parley_wnd_block,parley_mode2_left_kind,0);
          parley_add_side_dialog(&parley_wnd_block,(i32)parley_wnd_block.right_kind,1);
          civ_name = (civ_name_plural((int)parley_wnd_block.other_civ));
          text_set_string_arg(0,civ_name);
          dlg_pending_idle_callback = parley_poll_net_replies;
          popup_game_with_sprite((const char *)&str_parleycounteroffer,(iptr)&popup_info_icon_sprite,0);
        }
      }
      else {
        net_session_block.nm_parley_no_thanks_count = 0;
        parley_wnd_block.mode = 3;
        parley_wnd_block.selected_action = 0xffffffff;
        parley_set_mode_sides(&parley_wnd_block,1);
        civ_name = (civ_name_plural((int)parley_wnd_block.other_civ));
        text_set_string_arg(0,civ_name);
        dlg_pending_idle_callback = parley_poll_net_replies;
        popup_game_with_sprite((const char *)&str_parleynothanks,(iptr)&popup_info_icon_sprite,0);
      }
    }
    else {
      net_session_block.nm_parley_accept_offer_count = 0;
      if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
        parley_wnd_block.received_offer->hdr.msg_type = 0x86;
        if (net_session_block.net_host_flag == '\0') {
          net_send_message(0x86,0,0,0,0,0,0,0,0,(iptr)(void *)parley_wnd_block.received_offer);
        }
        else {
          parley_execute_transaction(parley_wnd_block.received_offer);
        }
        operator_delete((void *)parley_wnd_block.received_offer);
        parley_wnd_block.received_offer = 0;
      }
      parley_wnd_block.mode = 2;
      parley_wnd_block.left_kind = parley_mode2_left_kind;
      parley_wnd_block.right_kind = parley_mode2_right_kind;
      parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
      parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
      parley_add_side_dialog(&parley_wnd_block,parley_mode2_left_kind,0);
      parley_add_side_dialog(&parley_wnd_block,(i32)parley_wnd_block.right_kind,1);
      dlg_pending_idle_callback = parley_poll_net_replies;
      popup_game_with_sprite((const char *)&str_parleyaccept,(iptr)&popup_info_icon_sprite,0);
    }
  }
  else {
    net_session_block.nm_parley_transaction_count = 0;
    if ((parley_wnd_block.received_offer->mode == 6) && (parley_wnd_block.received_offer->item.value == 3)) {
      civ_name = (civ_name_plural((int)parley_wnd_block.other_civ));
      text_set_string_arg(0,civ_name);
      civ_name = (civ_name_plural(human_civ[0]));
      text_set_string_arg(1,civ_name);
      if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & 8) == 0) {
        if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & 4) == 0) {
          if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & 2) != 0) {
            diplo_clear_treaty_flags(human_civ[0],(int)parley_wnd_block.other_civ,2);
            popup_game((const char *)&str_breakcease);
          }
        }
        else {
          diplo_clear_treaty_flags(human_civ[0],(int)parley_wnd_block.other_civ,4);
          popup_game((const char *)&str_cancelpeace);
        }
      }
      else {
        diplo_clear_treaty_flags(human_civ[0],(int)parley_wnd_block.other_civ,8);
        popup_game((const char *)&str_cancelallied);
      }
    }
    else {
      parley_wnd_block.mode = 0;
      parley_wnd_block.left_kind = parley_mode_left_kinds[0];
      parley_wnd_block.right_kind = parley_mode_right_kinds[0];
      parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
      parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
      parley_add_side_dialog(&parley_wnd_block,parley_mode_left_kinds[0],0);
      parley_add_side_dialog(&parley_wnd_block,(i32)parley_wnd_block.right_kind,1);
    }
  }
  return;
}

/* 004b8676 auto */
/* WARNING: Removing unreachable block (ram,0x004b869f) */

void __thiscall parley_set_mode_sides(ParleyWindow *self,int refresh)

{
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  if ((i32)parley_wnd_block.selected_action == -1) {
    parley_wnd_block.mode = 3;
    ctrl_disable((Button *)parley_wnd_block.never_mind_button);
  }
  else if (parley_wnd_block.selected_action == 0) {
    parley_wnd_block.mode = 6;
    ctrl_enable((Button *)parley_wnd_block.never_mind_button);
  }
  else if ((i32)parley_wnd_block.selected_action == 1) {
    parley_wnd_block.mode = 0xc;
    if (parley_wnd_block.proposal_kind != 0) {
      parley_wnd_block.mode = 0xd;
    }
    ctrl_enable((Button *)parley_wnd_block.never_mind_button);
  }
  else if ((i32)parley_wnd_block.selected_action == 2) {
    if (parley_wnd_block.gift_kind == 0) {
      parley_wnd_block.mode = 7;
    }
    else if ((int)parley_wnd_block.gift_kind == 1) {
      parley_wnd_block.mode = 8;
    }
    else if ((int)parley_wnd_block.gift_kind == 2) {
      parley_wnd_block.mode = 9;
    }
    else if ((int)parley_wnd_block.gift_kind == 3) {
      parley_wnd_block.mode = 10;
    }
    else {
      parley_wnd_block.mode = 0xb;
    }
    ctrl_enable((Button *)parley_wnd_block.never_mind_button);
  }
  else if ((i32)parley_wnd_block.selected_action == 3) {
    parley_wnd_block.mode = 5;
    ctrl_enable((Button *)parley_wnd_block.never_mind_button);
  }
  else if ((i32)parley_wnd_block.selected_action == 4) {
    parley_wnd_block.mode = 0xf;
    ctrl_enable((Button *)parley_wnd_block.never_mind_button);
  }
  parley_wnd_block.left_kind = (((i32 *)&parley_mode_left_kinds))[(int)parley_wnd_block.mode * 2];
  parley_wnd_block.right_kind = (((i32 *)&parley_mode_right_kinds))[(int)parley_wnd_block.mode * 2];
  parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
  parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
  if (refresh != 0) {
    parley_add_side_dialog(&parley_wnd_block,(i32)parley_wnd_block.left_kind,0);
    parley_add_side_dialog(&parley_wnd_block,(i32)parley_wnd_block.right_kind,1);
  }
  return;
}

/* 004b888e auto */
void __thiscall parley_clear_side_controls(ParleyWindow *self,int side)

{
  u8 *slot_ptr;
  int half_width;
  int i;
  
  if ((side != 0) && (side != 1)) {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_leftright_left_leftright_right,(iptr)&str_d_ss_franklinton_parleywin_cpp,0x4ea);
  }
  if (side == 1) {
    if (self->desc_edit != 0) {
      if ((Button *)self->desc_edit != (Button *)0x0) {
        edit_scalar_dtor((EditControl *)self->desc_edit,1);
      }
      self->desc_edit = 0;
    }
    if (self->anonymous_checkbox != 0) {
      if ((Checkbox *)self->anonymous_checkbox != (Checkbox *)0x0) {
        checkbox_scalar_dtor((Checkbox *)self->anonymous_checkbox,1);
      }
      self->anonymous_checkbox = 0;
    }
    if (self->mute_anonymous_checkbox != 0) {
      if ((Checkbox *)self->mute_anonymous_checkbox != (Checkbox *)0x0) {
        checkbox_scalar_dtor((Checkbox *)self->mute_anonymous_checkbox,1);
      }
      self->mute_anonymous_checkbox = 0;
    }
    if (self->broadcast_checkbox != 0) {
      if ((Checkbox *)self->broadcast_checkbox != (Checkbox *)0x0) {
        checkbox_scalar_dtor((Checkbox *)self->broadcast_checkbox,1);
      }
      self->broadcast_checkbox = 0;
    }
    for (i = 0; i < 8; i++) {
      if (self->send_checkboxes[i] != 0) {
        if ((Checkbox *)self->send_checkboxes[i] != (Checkbox *)0x0) {
          checkbox_scalar_dtor((Checkbox *)self->send_checkboxes[i],1);
        }
        slot_ptr = (u8 *)&self->send_checkboxes[i];
        slot_ptr[0] = '\0';
        slot_ptr[1] = '\0';
        slot_ptr[2] = '\0';
        slot_ptr[3] = '\0';
      }
      if (self->mute_checkboxes[i] != 0) {
        if ((Checkbox *)self->mute_checkboxes[i] != (Checkbox *)0x0) {
          checkbox_scalar_dtor((Checkbox *)self->mute_checkboxes[i],1);
        }
        slot_ptr = (u8 *)&self->mute_checkboxes[i];
        slot_ptr[0] = '\0';
        slot_ptr[1] = '\0';
        slot_ptr[2] = '\0';
        slot_ptr[3] = '\0';
      }
    }
  }
  if (self->side_checkboxes[side] != 0) {
    if ((Checkbox *)self->side_checkboxes[side] != (Checkbox *)0x0) {
      checkbox_scalar_dtor((Checkbox *)self->side_checkboxes[side],1);
    }
    slot_ptr = (u8 *)&self->side_checkboxes[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->side_radios[side] != 0) {
    if ((RadioGroup *)self->side_radios[side] != (RadioGroup *)0x0) {
      radiogrp_scalar_dtor((RadioGroup *)self->side_radios[side],1);
    }
    slot_ptr = (u8 *)&self->side_radios[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->gold_edits[side] != 0) {
    if ((Button *)self->gold_edits[side] != (Button *)0x0) {
      edit_scalar_dtor((EditControl *)self->gold_edits[side],1);
    }
    slot_ptr = (u8 *)&self->gold_edits[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->list_scrollbars[side] != 0) {
    if ((Button *)self->list_scrollbars[side] != (Button *)0x0) {
      scrollbar_scalar_dtor((Scrollbar *)self->list_scrollbars[side],1);
    }
    slot_ptr = (u8 *)&self->list_scrollbars[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->list_hotspots[side] != 0) {
    if ((Hotspot *)self->list_hotspots[side] != (Hotspot *)0x0) {
      hotspot_scalar_dtor((Hotspot *)self->list_hotspots[side],1);
    }
    slot_ptr = (u8 *)&self->list_hotspots[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->sort_by_unit_buttons[side] != 0) {
    if ((Button *)self->sort_by_unit_buttons[side] != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->sort_by_unit_buttons[side],1);
    }
    slot_ptr = (u8 *)&self->sort_by_unit_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->sort_by_city_buttons[side] != 0) {
    if ((Button *)self->sort_by_city_buttons[side] != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->sort_by_city_buttons[side],1);
    }
    slot_ptr = (u8 *)&self->sort_by_city_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->info_show_buttons[side] != 0) {
    if ((Button *)self->info_show_buttons[side] != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->info_show_buttons[side],1);
    }
    slot_ptr = (u8 *)&self->info_show_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (self->info_hide_buttons[side] != 0) {
    if ((Button *)self->info_hide_buttons[side] != (Button *)0x0) {
      ctrl_scalar_dtor((Button *)self->info_hide_buttons[side],1);
    }
    slot_ptr = (u8 *)&self->info_hide_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  self->fill_left = self->panel_left;
  self->fill_top = self->panel_top;
  self->fill_right = self->panel_right;
  self->fill_bottom = self->panel_bottom;
  self->fill_bottom = self->divider_y;
  if (side == 0) {
    half_width = rect_width((RECT *)&self->panel_left);
    self->fill_right = half_width / 2;
  }
  else {
    half_width = rect_width((RECT *)&self->panel_left);
    self->fill_left = half_width / 2;
  }
  parley_fill_background(self,(RECT *)&self->fill_left,2);
  return;
}

/* 004b8e5c auto */
void __cdecl parley_gold_edit_on_change(int control_id)

{
  char *src_ptr;
  size_t len;
  int amount;
  size_t text_len;
  uptr i;
  char gold_text[32];
  uint edit_index;
  i32 modified;
  char text[32];
  uint caret;
  
  modified = 0;
  _memset(text,0,0x20);
  _memset(gold_text,0,0x20);
  edit_index = (uint)(control_id != 0x3f8);
  caret = edit_get_sel_end((EditControl *)parley_wnd_block.gold_edits[edit_index]);
  ctrl_get_text((Control *)parley_wnd_block.gold_edits[edit_index],(char *)text);
  __itoa(civs[human_civ[0]].gold,gold_text,10);
  for (i = 0; len = _strlen(text), i < len; i++) {
    if ((text[i] < '0') || ('9' < text[i])) {
      modified = 1;
      crt_strcpy(text + i,text + i + 1);
    }
  }
  amount = _atoi(text);
  if ((((int)parley_wnd_block.mode == 0xe) && (edit_index == 0)) || (((int)parley_wnd_block.mode != 0xf && ((int)parley_wnd_block.mode != 0xe)))
     ) {
    len = _strlen(gold_text);
    text_len = _strlen(text);
    if ((len < text_len) || (civs[human_civ[0]].gold < amount)) {
      modified = 1;
      src_ptr = text + caret;
      amount = caret - 1;
      caret--;
      crt_strcpy(text + amount,src_ptr);
    }
  }
  amount = _atoi(text);
  if (amount < 1) {
    parley_wnd_block.side_selected[edit_index] = 0;
  }
  else {
    parley_wnd_block.side_selected[edit_index] = 1;
  }
  parley_update_buttons(&parley_wnd_block);
  if (modified != 0) {
    len = _strlen(text);
    if (len < caret) {
      caret = _strlen(text);
    }
    ctrl_set_text((Control *)parley_wnd_block.gold_edits[edit_index],(const char *)text);
    edit_set_caret((EditControl *)parley_wnd_block.gold_edits[edit_index],caret);
  }
  return;
}

/* 004b90ad auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
chat_receive_message(ParleyWindow *self,iptr civ,iptr civ2,iptr is_private,iptr message)

{
  u32 muted;
  size_t log_len;
  const void *label;
  int listener;
  
  ctrl_get_text((Control *)chat_wnd_block.chat_log_edit,chat_log_buffer);
  text_clear();
  log_len = _strlen(chat_log_buffer);
  if (log_len != 0) {
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_recv_newline_str));
  }
  if ((char)is_private == '\0') {
    label = (label_string(label_string_ids[0x2d8]));
    crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ);
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_recv_year_open_str));
    text_append_year((int)game.year);
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_recv_year_close_str));
  }
  else {
    label = (label_string(label_string_ids[0x2d6]));
    crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ);
    label = (label_string(label_string_ids[0x2d7]));
    crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ2);
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_recv_private_year_open_str));
    text_append_year((int)game.year);
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_recv_private_year_close_str));
  }
  crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)((const void *)message));
  if ((civ == 0) && (chat_wnd_block.mute_anonymous != 0)) {
    return;
  }
  if ((char)is_private == '\0') {
    if (parley_wnd_block.closed == 0) {
      if ((int)parley_wnd_block.other_civ == civ) goto relay_to_listeners;
      muted = self->muted_civs[civ];
    }
    else {
      muted = self->muted_civs[civ];
    }
    if (muted != 0) {
      return;
    }
  }
relay_to_listeners:
  for (listener = 1; listener < 8; listener++) {
    if ((((listener != civ) && (human_civ[0] != listener)) &&
        ((1 << ((byte)listener & 0x1f) & (uint)game.human_players) != 0)) &&
       ((*(byte *)(listener * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)) {
      net_send_message(0x68,net_session_block.net_slots[net_session_block.net_civ_slots[listener]].id,
                         civ,civ2,1,0,0,0,(const char *)message,0);
    }
  }
  chat_log_append((iptr)&map_views_block.text_buffer);
  if (chat_wnd_block.closed != 0) {
    net_session_block.chat_unread_count++;
    net_session_block.chat_missed_message_count_i32++;
  }
  if ((parley_wnd_block.closed != 0) && (chat_wnd_block.closed != 0)) {
    parley_wnd_open(&chat_wnd_block,0,4);
  }
  return;
}

/* 004b93a2 auto */
void __cdecl chat_log_append(iptr line)

{
  int capacity;
  size_t log_len;
  size_t line_len;
  i32 caret_pos;
  uptr trim_len;
  
  capacity = font_size16_bold_block.chat_log_capacity;
  trim_len = 0;
  log_len = _strlen(chat_log_buffer);
  line_len = _strlen((char *)line);
  if (capacity - (log_len + 1) < line_len + 1) {
    trim_len = _strlen((char *)line);
    do {
      trim_len++;
      if ((chat_log_buffer[trim_len] == '\r') || (chat_log_buffer[trim_len] == '\n')) break;
    } while (chat_log_buffer[trim_len] != '\0');
    if (chat_log_buffer[trim_len] != '\0') {
      for (; (chat_log_buffer[trim_len] == '\r' || (chat_log_buffer[trim_len] == '\n'));
          trim_len = trim_len + 1) {
      }
    }
  }
  crt_strcpy(chat_log_buffer,chat_log_buffer + trim_len);
  crt_strcat(chat_log_buffer,(const char *)((const void *)line));
  ctrl_set_text((Control *)chat_wnd_block.chat_log_edit,(const char *)chat_log_buffer);
  ctrl_set_text((Control *)parley_wnd_block.chat_log_edit,(const char *)chat_log_buffer);
  caret_pos = edit_get_text_end((EditControl *)chat_wnd_block.chat_log_edit);
  edit_set_caret((EditControl *)chat_wnd_block.chat_log_edit,caret_pos);
  caret_pos = edit_get_text_end((EditControl *)chat_wnd_block.chat_log_edit);
  edit_set_caret((EditControl *)parley_wnd_block.chat_log_edit,caret_pos);
  return;
}

/* 004b9504 auto */
void __cdecl chat_append_sender_name(int civ)

{
  i32 name_text;
  
  if (civ == 0) {
    name_text = (i32)(label_string(label_string_ids[0x2d9]));
    crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)((const void *)name_text));
  }
  else {
    name_text = civ_leader_title(civ);
    crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)((const void *)name_text));
    text_append_space();
    name_text = (i32)(civ_leader_name(civ));
    crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)((const void *)name_text));
  }
  return;
}

/* 004b957e auto */
void __cdecl parley_wnd_on_key(int key)

{
  if (key == 0xd2) {
    diplo_wnd_escape_close(0);
  }
  return;
}

/* 004b95c2 auto */
void __cdecl chat_send_button_on_click(void)

{
  chat_input_on_key(0,0xd0);
  return;
}

/* 004b95e1 auto */
void __cdecl chat_clear_button_on_click(void)

{
  _memset((*(void * *)&chat_log_buffer),0,font_size16_bold_block.chat_log_capacity + 1);
  ctrl_set_text((Control *)chat_wnd_block.chat_log_edit,(const char *)(*(void * *)&chat_log_buffer));
  ctrl_set_text((Control *)parley_wnd_block.chat_log_edit,(const char *)(*(void * *)&chat_log_buffer));
  ctrl_set_focus((Control *)parley_wnd_block.chat_input_edit);
  return;
}

/* 004b9635 auto */
i32 __cdecl parley_edit_escape_filter(i32 control,int key)

{
  i32 result;
  
  if ((key == 0x1b) || (key == 0xd2)) {
    diplo_wnd_escape_close(0);
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 004b968a auto */
i32 __cdecl chat_input_on_key(i32 control,int key)

{
  iptr child_wnd;
  size_t len;
  const void *label;
  iptr relay_sender;
  iptr sender;
  int count;
  int civ;
  ParleyWindow *wnd2;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (key < 0x2b5) {
    if (0x2af < key) {
      chat_insert_macro_text((EditControl *)wnd2->chat_input_edit,key);
      return 0;
    }
    if (key == 0xd0) {
      ctrl_get_text((Control *)wnd2->chat_input_edit,(*(char * *)&chat_input_buffer));
      len = _strlen((*(char * *)&chat_input_buffer));
      if (len == 0) {
        return 0;
      }
      ctrl_get_text((Control *)chat_wnd_block.chat_log_edit,chat_log_buffer);
      text_clear();
      len = _strlen(chat_log_buffer);
      if (len != 0) {
        crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_send_newline_str));
      }
      label = (label_string(label_string_ids[0x2da]));
      crt_strcat(((i8 *)&map_views_block.text_buffer),(const char *)label);
      text_append_space();
      if ((int)wnd2->mode == 4) {
        count = 0;
        for (civ = 1; civ < 8; civ++) {
          if (chat_wnd_block.send_to_civs[civ] != 0) {
            if (count != 0) {
              crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_send_recipient_sep_str));
            }
            chat_append_sender_name(civ);
            count++;
          }
        }
      }
      else {
        chat_append_sender_name((i32)wnd2->other_civ);
        count = 1;
      }
      if (count == 0) {
        return 0;
      }
      crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_send_year_open_str));
      text_append_year((int)game.year);
      crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&chat_send_year_close_str));
      crt_strcat(((i8 *)&map_views_block.text_buffer),(*(char * *)&chat_input_buffer));
      chat_log_append((iptr)&map_views_block.text_buffer);
      if ((int)wnd2->mode == 4) {
        for (civ = 1; civ < 8; civ++) {
          if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
              (human_civ[0] != civ)) && (chat_wnd_block.send_to_civs[civ] != 0)) {
            if (chat_wnd_block.remain_anonymous == 0) {
              sender = human_civ[0];
            }
            else {
              sender = 0;
            }
            net_send_message(0x68,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,
                               sender,civ,0,0,0,0,(const char *)(*(char * *)&chat_input_buffer),0);
            for (count = 1; count < 8; count++) {
              if ((((1 << ((byte)count & 0x1f) & (uint)game.human_players) != 0) &&
                  (human_civ[0] != count)) &&
                 ((count != civ &&
                  ((*(byte *)(count * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)))) {
                if (chat_wnd_block.remain_anonymous == 0) {
                  relay_sender = human_civ[0];
                }
                else {
                  relay_sender = 0;
                }
                net_send_message(0x68,net_session_block.net_slots[net_session_block.net_civ_slots[count]].id,relay_sender,
                                   civ,1,0,0,0,(const char *)(*(char * *)&chat_input_buffer),0);
              }
            }
          }
        }
      }
      else {
        net_send_message(0x68,net_session_block.net_slots[net_session_block.net_civ_slots[(int)wnd2->other_civ]].id,
                           human_civ[0],(iptr)wnd2->other_civ,0,0,0,0,(const char *)(*(char * *)&chat_input_buffer),0);
        for (count = 1; count < 8; count++) {
          if ((((1 << ((byte)count & 0x1f) & (uint)game.human_players) != 0) &&
              (human_civ[0] != count)) &&
             (((int)wnd2->other_civ != count &&
              ((*(byte *)(count * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)))) {
            net_send_message(0x68,net_session_block.net_slots[net_session_block.net_civ_slots[count]].id
                               ,human_civ[0],(iptr)wnd2->other_civ,1,0,0,0,(const char *)(*(char * *)&chat_input_buffer),
                               0);
          }
        }
      }
      *(*(char * *)&chat_input_buffer) = '\0';
      ctrl_set_text((Control *)wnd2->chat_input_edit,(const char *)(*(char * *)&chat_input_buffer));
      ctrl_set_focus((Control *)wnd2->chat_input_edit);
      return 0;
    }
    if (key == 0xd2) {
      diplo_wnd_escape_close(0);
      return 0;
    }
  }
  return 1;
}

/* 004db690 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 * __thiscall parley_send_offer(ParleyWindow *self,int mode)

{
  i32 offer_kind;
  i32 *msg;
  int left_items;
  int i;
  uint total_size;
  int right_size;
  int left_size;
  
  offer_kind = parley_offer_kinds[mode];
  parley_wnd_block.offer_counts[0] = 0;
  parley_wnd_block.offer_counts[1] = 0;
  right_size = parley_item_header_sizes[self->side_kind[1]];
  switch(self->side_kind[1]) {
  case 8:
  case 0xc:
    for (i = 0; i < parley_wnd_block.list_counts[1]; i++) {
      if ((int)parley_wnd_block.list_selected[i + 2049] == 1) {
        parley_wnd_block.offer_counts[1]++;
      }
    }
    right_size = self->offer_counts[1] * 4 + right_size;
    break;
  case 10:
  case 0x11:
    for (i = 0; i < parley_wnd_block.list_counts[1]; i++) {
      if ((int)parley_wnd_block.list_selected[i + 2049] == 1) {
        parley_wnd_block.offer_counts[1]++;
      }
    }
    right_size = self->offer_counts[1] * 4 + right_size;
    if (self->side_kind[1] == 0x11) {
      right_size = self->offer_counts[1] * 4 + right_size;
    }
  }
  total_size = right_size + 0x20;
  if (mode == 0xe) {
    left_size = parley_item_header_sizes[self->side_kind[0]];
    switch(self->side_kind[0]) {
    case 8:
    case 0xc:
      for (i = 0; i < parley_wnd_block.list_counts[0]; i++) {
        if ((int)parley_wnd_block.list_selected[i] == 1) {
          parley_wnd_block.offer_counts[0]++;
        }
      }
      left_size = self->offer_counts[0] * 4 + left_size;
      break;
    case 10:
    case 0x11:
      for (i = 0; i < parley_wnd_block.list_counts[0]; i++) {
        if ((int)parley_wnd_block.list_selected[i] == 1) {
          parley_wnd_block.offer_counts[0]++;
        }
      }
      left_size = self->offer_counts[0] * 4 + left_size;
      if (self->side_kind[0] == 0x11) {
        left_size = self->offer_counts[0] * 4 + left_size;
      }
    }
    total_size += left_size;
  }
  msg = (i32 *)(operator_new(total_size));
  if (msg == (i32 *)0x0) {
    dbg_log((const char *)&str_failed_to_allocate_buffer_for_parley_tra);
    msg = (i32 *)0x0;
  }
  else {
    *msg = 0x66606660;
    msg[1] = 0x82;
    msg[2] = total_size;
    msg[4] = self->mode;
    msg[5] = human_civ[0];
    msg[6] = self->other_civ;
    msg[7] = offer_kind;
    if (total_size + (int)msg < (uint)(right_size + (int)(msg + 8))) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_charptr_pchunk_sizechunk0_charptr_pmessa,(iptr)&str_d_ss_franklinton_parleywin_transaction_c,0x9f);
    }
    parley_pack_offer_items(self,(ParleyOfferItem *)(iptr)(msg + 8),self->side_kind[1],right_size,1);
    if (mode == 0xe) {
      left_items = (int)msg + right_size + 0x20;
      if (left_size + left_items != total_size + (int)msg) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_charptr_pchunk_sizechunk1_charptr_pmessa,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_2,0xa4);
      }
      parley_pack_offer_items(self,(ParleyOfferItem *)left_items,self->side_kind[0],left_size,0);
    }
    net_send_message(0x82,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0
                       ,0,0,0,0,0,(iptr)msg);
  }
  return msg;
}

/* 004dbab4 auto */
void __thiscall
parley_pack_offer_items
          (ParleyWindow *self,ParleyOfferItem *dest,i32 item_type,i32 item_size,iptr side)

{
  int gold;
  i32 *item_ptr2;
  int packed_count;
  int i;
  i32 *pair_ptr;
  char gold_text[32];
  
  switch(item_type) {
  case 5:
  case 6:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = self->treaty_kind;
    break;
  case 8:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)self->offer_counts[side];
    item_ptr2 = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *item_ptr2 = (i32)self->list_items[side * 0x801 + i];
        item_ptr2++;
        packed_count++;
        if ((int)self->offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 9:
    dest->type = item_type;
    dest->size = item_size;
    ctrl_get_text((Control *)parley_wnd_block.gold_edits[side],(char *)gold_text);
    gold = _atoi(gold_text);
    dest->value = gold;
    break;
  case 10:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)self->offer_counts[side];
    pair_ptr = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *pair_ptr = (i32)self->list_items_b[side * 0x801 + i];
        pair_ptr++;
        packed_count++;
        if ((int)self->offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 0xc:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)self->offer_counts[side];
    item_ptr2 = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *item_ptr2 = (i32)self->list_items[side * 0x801 + i];
        item_ptr2++;
        packed_count++;
        if ((int)self->offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 0xd:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)self->map_choice[side];
    break;
  case 0x11:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)self->offer_counts[side];
    pair_ptr = (i32 *)((iptr)dest + 0xc);
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *pair_ptr = (i32)self->list_items_b[side * 0x801 + i];
        pair_ptr[1] = (i32)self->list_selected_b[side * 0x801 + i];
        pair_ptr += 2;
        packed_count++;
        if ((int)self->offer_counts[side] == packed_count) {
          return;
        }
      }
    }
  }
  return;
}

/* 004dbee6 auto */
void __thiscall parley_build_description(ParleyWindow *self,ParleyOfferMsg *offer,int received)

{
  int maps_kind;
  int *first_items;
  ParleyOfferMsg *second_offer;
  i32 *second_items;
  size_t desc_len;
  iptr label;
  
  if (offer != 0) {
    first_items = (int *)((iptr)offer + 0x20);
    second_offer = (ParleyOfferMsg *)(offer->item.size + (iptr)offer);
    second_items = (i32 *)((iptr)second_offer + 0x20);
    str_clear((char *)&text_section_index_pool_block.parley_description);
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (desc_len != 0) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_strlen_parleydescription_0,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_3,0x120);
    }
    if (received == 0) {
      label = (iptr)(label_string(label_string_ids[0x31c]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_transmitted_crlf_str));
    }
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (0x7ff < desc_len) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_strlen_parleydescription_2048,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_4,0x126);
    }
    switch(offer->mode) {
    case 6:
      parley_describe_treaty(self,offer->item.value,0);
      break;
    case 7:
      label = (iptr)(label_string(label_string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_adv_crlf_str));
      label = (iptr)(label_string(label_string_ids[0x327]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_adv_head_crlf_str));
      parley_describe_advances((ParleyOfferItem *)first_items);
      break;
    case 8:
      label = (iptr)(label_string(label_string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_gold_crlf_str));
      label = (iptr)(label_string(label_string_ids[0x329]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_gold_head_crlf_str));
      str_concat((iptr)&text_section_index_pool_block.parley_description,(iptr)&str_empty_139);
      parley_describe_gold(offer->item.value);
      break;
    case 9:
      label = (iptr)(label_string(label_string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_units_crlf_str));
      label = (iptr)(label_string(label_string_ids[0x328]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_units_head_crlf_str));
      parley_describe_units((ParleyOfferItem *)first_items);
      break;
    case 10:
      label = (iptr)(label_string(label_string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_cities_crlf_str));
      label = (iptr)(label_string(label_string_ids[0x32a]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_cities_head_crlf_str));
      parley_describe_cities((ParleyOfferItem *)first_items);
      break;
    case 0xb:
      label = (iptr)(label_string(label_string_ids[0x326]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_gift_maps_crlf_str));
      parley_describe_maps(offer->item.value,0);
      break;
    case 0xc:
      label = (iptr)(label_string(label_string_ids[0x321]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_war_offer_crlf_str));
      label = (iptr)(label_string(label_string_ids[0x325]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_war_head_crlf_str));
      parley_describe_war_targets((ParleyOfferItem *)first_items);
      break;
    case 0xd:
      label = (iptr)(label_string(label_string_ids[0x321]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_maps_offer_crlf_str));
      maps_kind = offer->item.value;
      if (maps_kind == 0) {
        label = (iptr)(label_string(label_string_ids[0x322]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      else if (maps_kind == 1) {
        label = (iptr)(label_string(label_string_ids[0x323]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      else if (maps_kind == 2) {
        label = (iptr)(label_string(label_string_ids[0x324]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_maps_kind_crlf_str));
      break;
    case 0xe:
      label = (iptr)(label_string(label_string_ids[0x32d]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_barter_crlf_str));
      switch(*second_items) {
      case 5:
      case 6:
        label = (iptr)(label_string(label_string_ids[0x347]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_treaty_crlf_str));
        parley_describe_treaty(self,second_offer->item.value,1);
        break;
      case 8:
        label = (iptr)(label_string(label_string_ids[0x32e]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_adv_crlf_str));
        parley_describe_advances((ParleyOfferItem *)second_items);
        break;
      case 9:
        label = (iptr)(label_string(label_string_ids[0x32f]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_gold_crlf_str));
        str_concat((iptr)&text_section_index_pool_block.parley_description,(iptr)&str_empty_140);
        parley_describe_gold(second_offer->item.value);
        break;
      case 10:
        label = (iptr)(label_string(label_string_ids[0x330]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_units_crlf_str));
        parley_describe_units((ParleyOfferItem *)second_items);
        break;
      case 0xc:
        label = (iptr)(label_string(label_string_ids[0x333]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_war_crlf_str));
        parley_describe_war_targets((ParleyOfferItem *)second_items);
        break;
      case 0xd:
        label = (iptr)(label_string(label_string_ids[0x332]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_maps_crlf_str));
        parley_describe_maps(second_offer->item.value,0);
        break;
      case 0x11:
        label = (iptr)(label_string(label_string_ids[0x331]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_give_cities_crlf_str));
        parley_describe_cities((ParleyOfferItem *)second_items);
      }
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_barter_sep_crlf_str));
      switch(*first_items) {
      case 5:
      case 6:
        label = (iptr)(label_string(label_string_ids[0x348]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_treaty_crlf_str));
        parley_describe_treaty(self,offer->item.value,1);
        break;
      case 8:
        label = (iptr)(label_string(label_string_ids[0x334]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_want_adv_crlf_str));
        parley_describe_advances((ParleyOfferItem *)first_items);
        break;
      case 9:
        label = (iptr)(label_string(label_string_ids[0x335]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_want_gold_crlf_str));
        str_concat((iptr)&text_section_index_pool_block.parley_description,(iptr)&str_empty_141);
        parley_describe_gold(offer->item.value);
        break;
      case 10:
        label = (iptr)(label_string(label_string_ids[0x336]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_want_units_crlf_str));
        parley_describe_units((ParleyOfferItem *)first_items);
        break;
      case 0xc:
        label = (iptr)(label_string(label_string_ids[0x339]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_war_targets_crlf_str));
        parley_describe_war_targets((ParleyOfferItem *)first_items);
        break;
      case 0xd:
        label = (iptr)(label_string(label_string_ids[0x338]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_maps_crlf_str));
        parley_describe_maps(offer->item.value,1);
        break;
      case 0x11:
        label = (iptr)(label_string(label_string_ids[0x337]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_exch_for_cities_crlf_str));
        parley_describe_cities((ParleyOfferItem *)first_items);
      }
      break;
    case 0xf:
      label = (iptr)(label_string(label_string_ids[0x32b]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      str_append_space((iptr)&text_section_index_pool_block.parley_description);
      if (*first_items == 0xc) {
        label = (iptr)(label_string(label_string_ids[0x346]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      else if ((*first_items == 5) || (*first_items == 6)) {
        label = (iptr)(label_string(label_string_ids[0x345]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      else {
        label = (iptr)(label_string(label_string_ids[0x32c]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      }
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_desc_threat_crlf_str));
      switch(*first_items) {
      case 5:
      case 6:
        parley_describe_treaty(self,offer->item.value,1);
        break;
      case 8:
        parley_describe_advances((ParleyOfferItem *)first_items);
        break;
      case 9:
        parley_describe_gold(offer->item.value);
        break;
      case 10:
        parley_describe_units((ParleyOfferItem *)first_items);
        break;
      case 0xc:
        parley_describe_war_targets((ParleyOfferItem *)first_items);
        break;
      case 0xd:
        parley_describe_maps(offer->item.value,1);
        break;
      case 0x11:
        parley_describe_cities((ParleyOfferItem *)first_items);
      }
    }
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (0x7ff < desc_len) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_strlen_parleydescription_2048_2,(iptr)&str_d_ss_franklinton_parleywin_transaction_c_5,0x1f1);
    }
  }
  return;
}

/* 004dcafa auto */
void __cdecl parley_describe_advances(ParleyOfferItem *items)

{
  int count;
  iptr adv_name;
  int i;
  int *adv_ptr;
  
  count = items->value;
  adv_ptr = (int *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    adv_name = (iptr)(label_string(advances[*adv_ptr].name));
    str_concat((iptr)&text_section_index_pool_block.parley_description,adv_name);
    if (1 < count) {
      if (i < count - 2) {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_advances_comma_str));
      }
      else if (count - 2 == i) {
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
        adv_name = (iptr)(label_string(label_string_ids[0x33a]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,adv_name);
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
      }
      else {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_advances_period_str));
      }
    }
    adv_ptr++;
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_advances_crlf_str));
  return;
}

/* 004dcc0c auto */
void __cdecl parley_describe_gold(int gold)

{
  const void *label;
  char gold_text[16];
  
  __itoa(gold,gold_text,10);
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),gold_text);
  str_append_space((iptr)&text_section_index_pool_block.parley_description);
  label = (label_string(label_string_ids[0x33b]));
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),(const char *)label);
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_gold_crlf_str));
  return;
}

/* 004dcc83 auto */
void __cdecl parley_describe_units(ParleyOfferItem *items)

{
  iptr unit;
  iptr label;
  int type_counts[62];
  int listed;
  iptr unit_ids;
  iptr unit_type;
  int distinct_types;
  int unit_count;
  char count_text[16];
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    type_counts[unit_type] = 0;
  }
  unit_count = items->value;
  unit_ids = (iptr)items + 0xc;
  distinct_types = 0;
  for (unit_type = 0; unit_type < unit_count; unit_type++) {
    unit = parley_find_unit_by_id
                      (&parley_wnd_block,*(i32 *)(unit_ids + unit_type * 4));
    if (-1 < unit) {
      if (type_counts[units[unit].type] == 0) {
        distinct_types++;
      }
      type_counts[units[unit].type] = type_counts[units[unit].type] + 1;
    }
  }
  listed = 0;
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if (type_counts[unit_type] != 0) {
      str_append_string_id((iptr)&text_section_index_pool_block.parley_description,unit_types[unit_type].name);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_open_paren_str));
      __itoa(type_counts[unit_type],count_text,10);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),count_text);
      crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_close_paren_str));
      if (1 < unit_count) {
        if (listed < distinct_types - 2) {
          crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_comma_str));
        }
        else if (distinct_types - 2 == listed) {
          str_append_space((iptr)&text_section_index_pool_block.parley_description);
          label = (iptr)(label_string(label_string_ids[0x33a]));
          str_concat((iptr)&text_section_index_pool_block.parley_description,label);
          str_append_space((iptr)&text_section_index_pool_block.parley_description);
        }
        else {
          crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_period_str));
        }
      }
      listed++;
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_units_crlf_str));
  return;
}

/* 004dcea5 auto */
void __cdecl parley_describe_cities(ParleyOfferItem *items)

{
  int count;
  int city;
  iptr label;
  int i;
  char size_text[16];
  
  count = items->value;
  for (i = 0; i < count; i++) {
    city = parley_find_city_by_id
                      (&parley_wnd_block,*(i32 *)((iptr)items + 0xc + i * 8));
    str_append((char *)&text_section_index_pool_block.parley_description,(const char *)&cities[city].name);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_open_paren_str));
    __itoa(*(int *)((iptr)items + 0x10 + i * 8),size_text,10);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),size_text);
    crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_close_paren_str));
    if (1 < count) {
      if (i < count - 2) {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_comma_str));
      }
      else if (count - 2 == i) {
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
        label = (iptr)(label_string(label_string_ids[0x33a]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,label);
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
      }
      else {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_period_str));
      }
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_cities_crlf_str));
  return;
}

/* 004dd016 auto */
void __cdecl parley_describe_maps(int map_kind,int exchange)

{
  iptr label;
  
  if (map_kind == 0) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_string_ids[0x33c]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else {
      label = (iptr)(label_string(label_string_ids[0x33d]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  else if (map_kind == 1) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_string_ids[0x33e]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else {
      label = (iptr)(label_string(label_string_ids[0x33f]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  else if (map_kind == 2) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_string_ids[0x340]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else {
      label = (iptr)(label_string(label_string_ids[0x341]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_maps_crlf_str));
  return;
}

/* 004dd176 auto */
void __cdecl parley_describe_war_targets(ParleyOfferItem *items)

{
  int count;
  iptr text;
  iptr *civ_ptr;
  int i;
  
  count = items->value;
  civ_ptr = (iptr *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    text = (iptr)(civ_name_plural(*civ_ptr));
    str_concat((iptr)&text_section_index_pool_block.parley_description,text);
    if (1 < count) {
      if (i < count - 2) {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_war_targets_comma_str));
      }
      else if (count - 2 == i) {
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
        text = (iptr)(label_string(label_string_ids[0x33a]));
        str_concat((iptr)&text_section_index_pool_block.parley_description,text);
        str_append_space((iptr)&text_section_index_pool_block.parley_description);
      }
      else {
        crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_war_targets_period_str));
      }
    }
    civ_ptr++;
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_war_targets_crlf_str));
  return;
}

/* 004dd285 auto */
void __cdecl parley_execute_transaction(ParleyOfferMsg *offer)

{
  i32 *first_items;
  ParleyOfferMsg *second_offer;
  i32 *second_items;
  iptr civ;
  iptr civ2;
  
  if (offer != 0) {
    dbg_logf((const char *)&str_start_executeparleytransaction_message_d,net_send_sequence);
    civ = offer->civ;
    civ2 = offer->other_civ;
    first_items = (i32 *)((iptr)offer + 0x20);
    second_offer = (ParleyOfferMsg *)(offer->item.size + (iptr)offer);
    second_items = (i32 *)((iptr)second_offer + 0x20);
    switch(offer->mode) {
    case 6:
      parley_apply_treaty(offer->item.value,civ,civ2);
      break;
    case 7:
      parley_transfer_advances(civ2,civ,(ParleyOfferItem *)first_items);
      break;
    case 8:
      parley_transfer_gold(civ2,civ,offer->item.value);
      break;
    case 9:
      parley_transfer_units(civ2,(ParleyOfferItem *)first_items);
      break;
    case 10:
      parley_transfer_cities(civ2,(ParleyOfferItem *)first_items);
      break;
    case 0xb:
      parley_give_maps(civ2,civ,offer->item.value);
      break;
    case 0xc:
      parley_apply_war_pact(civ,(ParleyOfferItem *)first_items);
      parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
      break;
    case 0xd:
      parley_give_maps(civ,civ2,offer->item.value);
      parley_give_maps(civ2,civ,offer->item.value);
      break;
    case 0xe:
      switch(*second_items) {
      case 5:
      case 6:
        parley_apply_treaty(second_offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ2,civ,(ParleyOfferItem *)second_items);
        break;
      case 9:
        parley_transfer_gold(civ2,civ,second_offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ2,(ParleyOfferItem *)second_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ,(ParleyOfferItem *)second_items);
        break;
      case 0xd:
        parley_give_maps(civ2,civ,second_offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ2,(ParleyOfferItem *)second_items);
      }
      switch(*first_items) {
      case 5:
      case 6:
        parley_apply_treaty(offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ,civ2,(ParleyOfferItem *)first_items);
        break;
      case 9:
        parley_transfer_gold(civ,civ2,offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ,(ParleyOfferItem *)first_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
        break;
      case 0xd:
        parley_give_maps(civ,civ2,offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ,(ParleyOfferItem *)first_items);
      }
      break;
    case 0xf:
      switch(*first_items) {
      case 5:
      case 6:
        parley_apply_treaty(offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ,civ2,(ParleyOfferItem *)first_items);
        break;
      case 9:
        parley_transfer_gold(civ,civ2,offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ,(ParleyOfferItem *)first_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
        break;
      case 0xd:
        parley_give_maps(civ,civ2,offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ,(ParleyOfferItem *)first_items);
      }
    }
    net_send_state_diff(0xff,2,0,0,0);
    if (human_civ[0] == civ2) {
      status_redraw(1);
    }
    else {
      net_send_message(0x79,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,0,0,0,0,
                         0,0,0,0);
    }
    if (human_civ[0] == civ) {
      status_redraw(1);
    }
    else {
      net_send_message(0x79,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,0,0,0,0,
                         0,0,0,0);
    }
    if ((civ_unit_counts_block.civ_unit_totals[civ2] == 0) && (civs[civ2].num_cities == 0)) {
      civ_check_destroyed(civ2,civ);
    }
    if ((civ_unit_counts_block.civ_unit_totals[civ] == 0) && (civs[civ].num_cities == 0)) {
      civ_check_destroyed(civ,civ2);
    }
    dbg_logf((const char *)&str_end_executeparleytransaction_message_d,net_send_sequence);
  }
  return;
}

/* 004dd8ad auto */
void __cdecl parley_give_maps(iptr receiver,iptr civ,int map_kind)

{
  byte recv_bits;
  byte giver_bits;
  uint recv_mask;
  uint giver_mask;
  Tile *tile;
  byte *recv_known;
  byte *giver_known;
  Tile *tile2;
  Tile *tile3;
  int y;
  iptr x;
  int in_bounds;
  i32 set_flag;
  i32 redraw_flag;
  iptr target_civ;
  iptr unit;
  byte known_bits;
  byte recv_bit;
  iptr city;
  uint y2;
  uint x2;
  iptr i;
  
  recv_mask = 1 << ((byte)receiver & 0x1f);
  giver_mask = 1 << ((byte)civ & 0x1f);
  y2 = 0;
  x2 = 0;
  net_batch_begin();
  for (i = 0; i < map.area; i++) {
    tile = tile_at(x2,y2);
    recv_known = known_at(x2,y2,receiver);
    giver_known = known_at(x2,y2,civ);
    if ((giver_mask & tile->visible) != 0) {
      if ((recv_mask & tile->visible) == 0) {
        known_set(x2,y2,0,receiver,0,1);
        tile_set_visible(x2,y2,recv_mask,1,1);
      }
      if (map_kind == 0) {
        if (((*recv_known & 0x80) == 0) || ((*giver_known & 0x80) == 0)) {
          giver_bits = *giver_known;
          recv_bits = *recv_known;
          tile2 = tile_at(x2,y2);
          known_bits = (giver_bits | recv_bits) & tile2->improvements & 0x80;
        }
        else {
          known_bits = 0x80;
        }
        redraw_flag = 1;
        set_flag = 0;
        target_civ = receiver;
        recv_known = known_at(x2,y2,receiver);
        known_set(x2,y2,*recv_known & 0xffffff7f | (uint)known_bits,target_civ,set_flag,redraw_flag);
        redraw_flag = 1;
        set_flag = 1;
        x = receiver;
        tile3 = tile_at(x2,y2);
        giver_bits = tile3->improvements;
        recv_known = known_at(x2,y2,civ);
        known_set(x2,y2,(uint)(giver_bits & *recv_known),x,set_flag,redraw_flag);
      }
    }
    x2 += 2;
    if ((int)map.width <= (int)x2) {
      y2++;
      x2 = y2 & 1;
    }
  }
  recv_bit = (byte)recv_mask;
  if (map_kind == 0) {
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) &&
         (((giver_mask & units[unit].visibility) != 0 || (units[unit].owner == civ)))) {
        units[unit].visibility = units[unit].visibility | recv_bit;
        y = (int)(tile_at((int)units[unit].x,(int)units[unit].y));
        if (((recv_mask & *(byte *)(y + 4)) == 0) || ((giver_mask & *(byte *)(y + 4)) == 0)) {
          for (i = 0; i < 0x15; i++) {
            x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
            y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
            in_bounds = map_in_bounds(x,y);
            if (in_bounds != 0) {
              tile_set_visible(x,y,recv_mask,1,1);
              tile_set_visible(x,y,giver_mask,1,1);
            }
          }
        }
      }
    }
  }
  if ((map_kind == 0) || (map_kind == 1)) {
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) &&
         (((giver_mask & (int)(char)cities[city].known_to) != 0 ||
          (cities[city].owner == civ)))) {
        if (cities[city].owner == civ) {
          *(i8 *)&cities[city].revealed_size[receiver] = cities[city].size;
        }
        else {
          *(i8 *)&cities[city].revealed_size[receiver] =
               *(i8 *)&cities[city].revealed_size[civ];
        }
        cities[city].known_to = cities[city].known_to | recv_bit;
        y = (int)(tile_at((int)cities[city].x,(int)cities[city].y));
        if (((recv_mask & *(byte *)(y + 4)) == 0) || ((giver_mask & *(byte *)(y + 4)) == 0)) {
          for (i = 0; i < 0x15; i++) {
            x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
            y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
            in_bounds = map_in_bounds(x,y);
            if (in_bounds != 0) {
              tile_set_visible(x,y,recv_mask,1,1);
              tile_set_visible(x,y,giver_mask,1,1);
            }
          }
        }
      }
    }
  }
  net_batch_flush();
  net_send_state_diff(0xff,2,0,0,0);
  if (human_civ[0] == receiver) {
    map_redraw_all_views(human_civ[0],1);
  }
  else {
    net_send_message(0x74,net_session_block.net_slots[net_session_block.net_civ_slots[receiver]].id,0,0,0,0,
                       0,0,0,0);
  }
  return;
}

/* 004dde9e auto */
void __cdecl parley_apply_war_pact(iptr civ,ParleyOfferItem *items)

{
  int count;
  iptr *civ_ptr;
  int i;
  
  count = items->value;
  civ_ptr = (iptr *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    diplo_set_treaty_flags(civ,*civ_ptr,0x2000);
    civ_ptr++;
  }
  return;
}

/* 004ddf04 auto */
void __cdecl parley_transfer_gold(iptr civ2,iptr civ,i32 amount)

{
  int attitude_value;
  
  if (civs[civ].gold < amount) {
    amount = civs[civ].gold;
  }
  civs[civ].gold -= amount;
  civs[civ2].gold += amount;
  attitude_value = diplo_gold_attitude_value(amount);
  civ_adjust_attitude(civ2,civ,-((attitude_value * 3) / 2));
  return;
}

/* 004ddfb2 auto */
void __cdecl parley_transfer_advances(iptr civ,iptr civ2,ParleyOfferItem *items)

{
  int count;
  int has_adv;
  iptr *adv_ptr;
  int i;
  
  count = items->value;
  adv_ptr = (iptr *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    has_adv = civ_has_advance(civ2,*adv_ptr);
    if (has_adv != 0) {
      has_adv = civ_has_advance(civ,*adv_ptr);
      if (has_adv == 0) {
        civ_give_advance(civ,*adv_ptr,civ2,1,0);
      }
    }
    adv_ptr++;
  }
  return;
}

/* 004de049 auto */
void __cdecl parley_transfer_cities(iptr civ,ParleyOfferItem *items)

{
  int count;
  iptr city;
  i32 *city_id_ptr;
  int i;
  
  count = items->value;
  city_id_ptr = (i32 *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    city = parley_find_city_by_id(&parley_wnd_block,*city_id_ptr);
    if ((-1 < city) && (cities[city].id != 0)) {
      parley_cede_city(city,civ);
    }
    city_id_ptr += 2;
  }
  return;
}

/* 004de0e2 auto */
void __cdecl parley_cede_city(uptr city,iptr civ)

{
  char *count_ptr;
  char civ3;
  iptr civ2;
  int x;
  int y;
  iptr x2;
  iptr y3;
  iptr in_bounds;
  int y2;
  iptr in_bounds2;
  Tile *tile;
  uptr leonardo_city;
  iptr improvement;
  iptr unit;
  iptr unit_type;
  iptr i;
  int defense_ratio;
  
  civ3 = cities[city].owner;
  civ2 = (iptr)civ3;
  x = (int)cities[city].x;
  y = (int)cities[city].y;
  city_set_improvement(city,1,0);
  city_set_improvement(city,4,0);
  city_set_improvement(city,0xb,0);
  city_set_improvement(city,7,0);
  civs[civ].last_city_turn = game.turn;
  civs[civ].num_cities++;
  if (civs[civ2].num_cities != 0) {
    civs[civ2].num_cities--;
  }
  net_batch_begin();
  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x);
    y3 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y;
    in_bounds = map_in_bounds(x2,y3);
    if (in_bounds != 0) {
      tile_set_radius_civ(x2,y3,civ,1);
      tile_set_visible(x2,y3,1 << ((byte)civ & 0x1f),1,1);
    }
  }
  cities[city].owner = (byte)civ;
  city_reveal_to_civ(city,civ2);
  if (scenario_events_loaded != 0) {
    events_check_city_taken
              ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events),(iptr)&cities[city].name,civ,civ2);
  }
  tile_set_owner(x,y,civ,1);
  unit = (iptr)game.num_units;
  while (unit = unit - 1, -1 < unit) {
    if ((units[unit].id != 0) && (units[unit].owner == civ2)) {
      if (((byte)units[unit].home_city == city) &&
         ((cities[city].x != units[unit].x || (cities[city].y != units[unit].y)))) {
        unit_delete_with_cargo(unit,1);
      }
      else if ((cities[city].x == units[unit].x) && (cities[city].y == units[unit].y))
      {
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *count_ptr = *count_ptr + 1;
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *count_ptr = *count_ptr - 1;
        units[unit].owner = (byte)civ;
        units[unit].home_city = -1;
        units[unit].moves_used = '\0';
        units[unit].orders = -1;
        city = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ,
                                     0xffffffff,-1);
        if ((-1 < (int)city) && (cities[city].owner == civ)) {
          units[unit].home_city = (i8)city;
        }
        unit_set_seen_by(unit,civ);
        if ((char)unit_types[units[unit].type].role < 5) {
          civs[civ].num_military_units++;
        }
        civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
        if ((civs[civ2].num_military_units != 0) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          civs[civ2].num_military_units--;
        }
        if (civ_unit_counts_block.civ_unit_totals[civ2] != 0) {
          civ_unit_counts_block.civ_unit_totals[civ2] = civ_unit_counts_block.civ_unit_totals[civ2] - 1;
        }
      }
    }
  }
  if ((cities[city].building < '\0') ||
     (y2 = civ_can_build_unit(civ,city,(int)cities[city].building), y2 != 0)) {
    if (-1 < cities[city].building) goto redraw_and_notify;
    if (cities[city].building < 1) {
      improvement = ~(int)cities[city].building + 1;
    }
    else {
      improvement = (iptr)cities[city].building;
    }
    y2 = city_can_build_improvement(civ,city,improvement);
    if (y2 != 0) goto redraw_and_notify;
  }
  cities[city].building = '\0';
  for (unit_type = 0x3e; -1 < unit_type; unit_type--) {
    if ((unit_types[unit_type].role == 1) &&
       (y2 = civ_can_build_unit(civ,city,unit_type), y2 != 0)) {
      defense_ratio = ((int)(char)unit_types[unit_type].defense << 3) /
                (int)(char)unit_types[unit_type].cost;
      if ((unit_types[unit_type].flags & 0x400) != 0) {
        defense_ratio++;
      }
      if (0 < defense_ratio) {
        cities[city].building = (i8)unit_type;
      }
      break;
    }
  }
redraw_and_notify:
  map_redraw_neighbors_all_views((int)cities[city].x,(int)cities[city].y);
  net_send_state_diff(0xff,2,0,0,0);
  net_send_message(0x75,0xff,(int)cities[city].x,(int)cities[city].y,0,0,0,0,0,0);
  y2 = (int)(tile_at(x,y));
  if ((1 << ((byte)civ & 0x1f) & (uint)*(byte *)(y2 + 4)) == 0) {
    tile_set_visible(x,y,1 << ((byte)civ & 0x1f),1,1);
    map_redraw_tile_all_views(x,y);
  }
  for (i = 0; i < 8; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[i] + x);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[i] + y;
    in_bounds2 = map_in_bounds(x2,y2);
    if ((in_bounds2 != 0) &&
       (tile = (Tile *)(int)(tile_at(x2,y2)),
       (1 << ((byte)civ & 0x1f) & (uint)tile->visible) == 0)) {
      tile_set_visible(x2,y2,1 << ((byte)civ & 0x1f),1,1);
      map_redraw_tile_all_views(x2,y2);
    }
  }
  net_batch_flush();
  net_send_message(0x75,0xff,x,y,0,0,0,0,0,0);
  cities[city].founder = civ3;
  cities[city].turns_captured = (u8)game.turn;
  leonardo_city = wonder_get_city(0xe);
  if (leonardo_city == city) {
    civ_leonardo_upgrade(civ);
  }
  return;
}

/* 004de990 auto */
void __cdecl parley_transfer_units(iptr civ,ParleyOfferItem *items)

{
  char *count_ptr;
  i32 unit_id;
  int result;
  iptr x3;
  iptr unit;
  iptr city;
  iptr unit2;
  int i;
  iptr y2;
  iptr x2;
  int count;
  iptr civ2;
  iptr y3;
  i32 *id_ptr;
  
  count = items->value;
  id_ptr = (i32 *)((iptr)items + 0xc);
  for (i = 0; i < count; i++) {
    unit_id = *id_ptr;
    id_ptr++;
    unit = parley_find_unit_by_id(&parley_wnd_block,unit_id);
    if ((-1 < unit) && (civ2 = (iptr)units[unit].owner, civ != civ2)) {
      if ((unit_types[units[unit].type].domain == 2) &&
         (result = unit_stack_sum(unit,2), 1 < result)) {
        while (result = unit_stack_sum(unit,2), 1 < result) {
          unit2 = unit_stack_first(unit);
          if (unit2 == unit) {
            unit2 = unit_next_in_stack(unit2);
          }
          result = parley_find_unit_drop_tile(unit2,(iptr)&x3,(iptr)&y3,
                                     (int)units[unit2].owner,1);
          if (result == 0) {
            unit_delete_with_cargo(unit2,1);
          }
          else {
            unit_relocate(unit2,x3,y3,1);
          }
        }
      }
      result = parley_find_unit_drop_tile(unit,(iptr)&x2,(iptr)&y2,civ,0);
      if (result != 0) {
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *count_ptr = *count_ptr + 1;
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *count_ptr = *count_ptr - 1;
        units[unit].owner = (i8)civ;
        units[unit].home_city = -1;
        units[unit].moves_used = '\0';
        units[unit].orders = -1;
        city = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ,
                                      0xffffffff,-1);
        if ((-1 < city) && (cities[city].owner == civ)) {
          units[unit].home_city = (i8)city;
        }
        unit_set_seen_by(unit,civ);
        if ((char)unit_types[units[unit].type].role < 5) {
          civs[civ].num_military_units++;
        }
        civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
        if ((civs[civ2].num_military_units != 0) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          civs[civ2].num_military_units--;
        }
        if (civ_unit_counts_block.civ_unit_totals[civ2] != 0) {
          civ_unit_counts_block.civ_unit_totals[civ2] = civ_unit_counts_block.civ_unit_totals[civ2] - 1;
        }
        unit_relocate(unit,x2,y2,1);
        unit_reveal_surroundings(unit,0);
      }
    }
  }
  XD_FlushSendBuffer(5000);
  return;
}

/* 004ded07 auto */
i32 __cdecl parley_find_unit_drop_tile(iptr unit,iptr x,iptr y,iptr civ,int must_move)

{
  iptr x2;
  int y2;
  int result;
  i32 wrapped_x;
  iptr i;
  i32 found;
  
  *(int *)x = (int)units[unit].x;
  x2 = *(iptr *)x;
  *(int *)y = (int)units[unit].y;
  y2 = *(int *)y;
  found = 1;
  if (((must_move != 0) || (result = tile_city_owner(x2,y2), result != -1)) ||
     (result = unit_stack_sum(unit,2), 1 < result)) {
    found = 0;
    for (i = 0; i < 0x2d; i++) {
      wrapped_x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
      *(i32 *)x = wrapped_x;
      *(int *)y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
      if ((((((must_move == 0) || (*(int *)x != x2)) || (*(int *)y != y2)) &&
           (result = map_in_bounds(*(iptr *)x,*(iptr *)y), result != 0)) &&
          (((result = tile_city_owner(*(iptr *)x,*(i32 *)y), result == civ ||
            (result = tile_unit_owner(*(i32 *)x,*(i32 *)y),
            result == civ)) ||
           ((result = tile_city_owner(*(iptr *)x,*(i32 *)y), result == -1 &&
            (result = tile_unit_owner(*(i32 *)x,*(i32 *)y), result == -1))))
          )) && ((((unit_types[units[unit].type].domain == '\0' &&
                   (result = tile_is_ocean(*(i32 *)x,*(i32 *)y), result == 0
                   )) || ((unit_types[units[unit].type].domain == 2 &&
                          (result = tile_is_ocean(*(i32 *)x,*(i32 *)y),
                          result != 0)))) || (unit_types[units[unit].type].domain == 1)))) {
        return 1;
      }
    }
  }
  return found;
}

/* 004def54 auto */
void __thiscall parley_describe_treaty(ParleyWindow *self,int treaty_kind,int exchange)

{
  iptr label;
  
  if (exchange == 0) {
    switch(treaty_kind) {
    case 0:
      label = (iptr)(label_string(label_string_ids[0x31d]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      break;
    case 1:
      label = (iptr)(label_string(label_string_ids[0x31e]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      break;
    case 2:
      label = (iptr)(label_string(label_string_ids[0x31f]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
      break;
    case 3:
      label = (iptr)(label_string(label_string_ids[0x320]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  else if (exchange == 1) {
    if (treaty_kind == 0) {
      label = (iptr)(label_string(label_string_ids[0x2f3]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else if (treaty_kind == 1) {
      label = (iptr)(label_string(label_string_ids[0x2f4]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
    else if (treaty_kind == 2) {
      label = (iptr)(label_string(label_string_ids[0x2f5]));
      str_concat((iptr)&text_section_index_pool_block.parley_description,label);
    }
  }
  crt_strcat(((i8 *)&text_section_index_pool_block.parley_description),((i8 *)&parley_treaty_crlf_str));
  return;
}

/* 004df10f auto */
void __cdecl parley_apply_treaty(i32 treaty_kind,iptr civ,iptr other_civ)

{
  switch(treaty_kind) {
  case 0:
    diplo_set_treaty_flags(civ,other_civ,2);
    break;
  case 1:
    diplo_set_treaty_flags(civ,other_civ,4);
    break;
  case 2:
    diplo_set_treaty_flags(civ,other_civ,8);
    break;
  case 3:
    if ((*(byte *)&civs[civ].treaties[other_civ] & 2) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,2);
    }
    if ((*(byte *)&civs[civ].treaties[other_civ] & 4) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,4);
    }
    if ((*(byte *)&civs[civ].treaties[other_civ] & 8) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,8);
    }
  }
  return;
}

/* 00511880 auto */
void __cdecl
net_send_text_args(iptr popup_kind,i32 player_id,int string_count,int number_count,iptr arg_a,iptr arg_b)

{
  size_t len;
  void *write_ptr;
  uint payload_size;
  int i;
  void *payload;
  
  payload = (void *)0x0;
  payload_size = 0;
  if (2 < game.multiplayer) {
    if (string_count != 0) {
      for (i = 0; i < string_count; i++) {
        len = _strlen(((i8 *)&minimap_window_block.text_string_args) + i * 0x104);
        payload_size = payload_size + len + 1;
      }
    }
    if (number_count != 0) {
      payload_size = number_count * 4 + payload_size;
    }
    if (payload_size != 0) {
      payload = operator_new(payload_size);
      write_ptr = payload;
      if (string_count != 0) {
        for (i = 0; i < string_count; i++) {
          crt_strcpy((char *)write_ptr,((i8 *)&minimap_window_block.text_string_args) + i * 0x104);
          len = _strlen(((i8 *)&minimap_window_block.text_string_args) + i * 0x104);
          write_ptr = (void *)((int)write_ptr + len + 1);
        }
      }
      if (number_count != 0) {
        for (i = 0; i < number_count; i++) {
          crt_memcpy(write_ptr,((i8 *)&minimap_window_block.text_number_args) + i * 4,4);
          write_ptr = (void *)((int)write_ptr + 4);
        }
      }
    }
    net_send_message(0x6a,player_id,popup_kind,string_count,number_count,arg_a,arg_b,0,0,(iptr)payload);
  }
  return;
}

/* 00511a0e auto */
void __cdecl
net_popup_queue_push(i32 popup_kind,i32 string_count,i32 number_count,i32 arg_a,
            i32 arg_b,i32 payload,int payload_size)

{
  i32 *new_head;
  i32 *entry;
  
  entry = (i32 *)(operator_new(payload_size + 0x1c));
  *entry = 0;
  entry[1] = popup_kind;
  entry[2] = string_count;
  entry[3] = number_count;
  entry[4] = arg_a;
  entry[5] = arg_b;
  entry[6] = payload_size;
  if (payload_size != 0) {
    crt_memcpy(entry + 7,(const void *)payload,payload_size);
  }
  new_head = entry;
  if ((*(i32 * *)&net_popup_queue_head) != (i32 *)0x0) {
    *(*(i32 * *)&net_popup_queue_tail) = (i32)entry;
    new_head = (*(i32 * *)&net_popup_queue_head);
  }
  (*(i32 * *)&net_popup_queue_head) = new_head;
  (*(i32 * *)&net_popup_queue_tail) = entry;
  return;
}

/* 00511acf auto */
void __cdecl net_popup_queue_clear(void)

{
  i32 *entry;
  
  while (entry = (*(i32 * *)&net_popup_queue_head), (*(i32 * *)&net_popup_queue_head) != (i32 *)0x0) {
    (*(i32 * *)&net_popup_queue_head) = (i32 *)*(*(i32 * *)&net_popup_queue_head);
    operator_delete(entry);
  }
  net_popup_queue_tail = 0;
  return;
}

/* 00511b2d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_popup_idle_autoclose(void)

{
  int now;
  
  net_process_messages(&net_session_block,1,0);
  if (((net_session_block.net_host_handoff_stage == '\0') && (net_session_block.nm_parley_cancel_count == 0)) &&
     (now = timer_ticks(), now - dialog_title_font_obj_block.dialog_timeout_start_i32 < 0x4b1)) {
    return;
  }
  net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
  wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  dialog_title_font_obj_block.dialog_timeout_start_i32 = timer_ticks();
  return;
}

/* 00511ba2 auto */
/* WARNING: Removing unreachable block (ram,0x0051343e) */
/* WARNING: Removing unreachable block (ram,0x0051347e) */
/* WARNING: Removing unreachable block (ram,0x0051369c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_popup_queue_show_next(void)

{
  int *entry;
  size_t len;
  int choice;
  iptr text;
  i8 *icon_sprite;
  iptr attitude_level;
  int i;
  Dialog dlg;
  int *arg_ptr;
  i32 eh_handler;
  i8 *eh_prev;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  entry = (*(int * *)&net_popup_queue_head);
  dialog_ctor(&dlg,0x4000);
  eh_state = 0;
  if (entry != (int *)0x0) {
    net_session_block.net_popup_in_progress_i32 = 1;
    (*(int * *)&net_popup_queue_head) = (int *)*entry;
    if ((*(int * *)&net_popup_queue_head) == (int *)0x0) {
      net_popup_queue_tail = 0;
    }
    if ((entry[2] != 0) || (entry[3] != 0)) {
      arg_ptr = entry + 7;
    }
    if (entry[2] != 0) {
      for (i = 0; i < entry[2]; i++) {
        crt_strcpy(((i8 *)&minimap_window_block.text_string_args) + i * 0x104,(const char *)arg_ptr);
        len = _strlen(((i8 *)&minimap_window_block.text_string_args) + i * 0x104);
        arg_ptr = (int *)((int)arg_ptr + len + 1);
      }
    }
    if (entry[3] != 0) {
      for (i = 0; i < entry[3]; i++) {
        crt_memcpy(((i8 *)&minimap_window_block.text_number_args) + i * 4,arg_ptr,4);
        arg_ptr++;
      }
    }
    dlg_pending_idle_callback = turn_popup_timeout_idle;
    switch(entry[1]) {
    case 0:
      popup_game((const char *)&str_retiredie);
      break;
    case 1:
      popup_game((const char *)&str_retireai);
      break;
    case 2:
      dlg_game_txt_with_unit((const char *)&str_barbarians_2,entry[4]);
      break;
    case 3:
      popup_game_with_sprite((const char *)&str_globalwarming_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 4:
      popup_game_with_sprite((const char *)&str_fearwarming_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 5:
      historian_report_show();
      break;
    case 6:
      popup_game_with_sprite((const char *)&str_eaglehaslanded_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 7:
      popup_game_with_sprite((const char *)&str_scenarioends_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      break;
    case 8:
      popup_game_with_sprite((const char *)&str_scenarioend_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      break;
    case 9:
      text = civ_leader_title(human_civ[0]);
      text_set_string_arg(0,(const char *)text);
      text = (iptr)(civ_leader_name(human_civ[0]));
      text_set_string_arg(1,(const char *)text);
      popup_game_with_sprite((const char *)&str_planretire_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      break;
    case 10:
      text = (iptr)(civ_leader_name(human_civ[0]));
      text_set_string_arg(1,(const char *)text);
      popup_game_with_sprite((const char *)&str_doretire_2,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      break;
    case 0xb:
      popup_game((const char *)&str_launched);
      spaceship_wnd_run(-entry[4]);
      break;
    case 0xc:
      popup_game((const char *)&str_spacereturns_2);
      break;
    case 0xd:
      popup_game((const char *)&str_spacedestroyed_2);
      break;
    case 0xe:
      music_play_track(2,1);
      popup_game_with_sprite((const char *)&str_destroyed_2,(iptr)(((i8 *)&road_sprites_block.civ_portrait_sprites) + entry[4] * 0x3c),0);
      break;
    case 0xf:
      snd_play_effect((-(uint)(entry[5] == 0) & 0x14) + 0x16,1,0,0);
      dlg_game_txt_with_unit((const char *)&str_caravan_6,entry[4]);
      break;
    case 0x10:
      snd_play_effect((-(uint)(entry[5] == 0) & 0x14) + 0x16,1,0,0);
      dlg_game_txt_with_unit((const char *)&str_foodcaravan_4,entry[4]);
      break;
    case 0x11:
      dlg_game_txt_with_unit_flags((const char *)&str_caravanother_3,entry[4],entry[5]);
      break;
    case 0x12:
      dialog_load_game_text(&dlg,(const char *)&str_startwonder_2);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,entry[5]);
      dlg_show_modal(&dlg,0);
      break;
    case 0x13:
      dialog_load_game_text(&dlg,(const char *)&str_switchwonder_2);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,entry[5]);
      dlg_show_modal(&dlg,0);
      break;
    case 0x14:
      dialog_load_game_text(&dlg,(const char *)&str_abandonwonder_2);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,entry[5]);
      dlg_show_modal(&dlg,0);
      break;
    case 0x15:
      dlg_game_txt_with_unit((const char *)&str_paradrop_2,entry[4]);
      break;
    case 0x16:
      popup_game((const char *)&str_schism_2);
      break;
    case 0x17:
      text = popup_game_with_sprite((const char *)&str_canescape,(iptr)&popup_info_icon_sprite,0);
      net_send_message(0x77,net_session_block.net_slots[net_session_block.net_civ_slots[entry[4]]].id,
                         text,0,0,0,0,0,0,0);
      break;
    case 0x18:
      popup_game_with_city((const char *)&str_escape,entry[4]);
      break;
    case 0x19:
      dialog_load_game_text(&dlg,(const char *)&str_capturewonder);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,8);
      dlg_show_modal(&dlg,0);
      break;
    case 0x1a:
      dialog_load_game_text(&dlg,(const char *)&str_lostwonder);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,8);
      dlg_show_modal(&dlg,0);
      break;
    case 0x1b:
      snd_play_effect(5,0,0,0);
      popup_city_picture_zoom((const char *)&str_citycapture,entry[5],0x46,1,entry[4]);
      break;
    case 0x1c:
      snd_play_effect(5,0,0,0);
      popup_city_picture_zoom((const char *)&str_citywinally,entry[5],0x46,1,entry[4]);
      break;
    case 0x1d:
      snd_play_effect(5,0,0,0);
      popup_city_picture_zoom((const char *)&str_cityloseally,entry[5],0x46,1,entry[4]);
      break;
    case 0x1e:
      snd_play_effect(5,0,0,0);
      popup_city_picture_zoom((const char *)&str_citycapture2,entry[5],0x46,1,entry[4]);
      break;
    case 0x1f:
      dlg_game_txt_with_unit((const char *)&str_partisans,entry[4]);
      break;
    case 0x20:
      dlg_game_txt_with_unit((const char *)&str_promoted,entry[4]);
      break;
    case 0x21:
      popup_game_with_sprite((const char *)&str_incorruptible_2,(iptr)&government_sprites_block.government_sprite_democracy,0);
      break;
    case 0x22:
      dlg_game_txt_with_unit((const char *)&str_deserted_2,entry[4]);
      break;
    case 0x23:
      text = dlg_game_txt_with_unit((const char *)&str_desert_3,entry[5]);
      net_send_message(0x7b,net_session_block.net_slots[net_session_block.net_civ_slots[entry[4]]].id,
                         text,0,0,0,0,0,0,0);
      break;
    case 0x24:
      text = dlg_game_txt_with_unit((const char *)&str_desert2,entry[5]);
      net_send_message(0x7b,net_session_block.net_slots[net_session_block.net_civ_slots[entry[4]]].id,
                         text,0,0,0,0,0,0,0);
      break;
    case 0x25:
      dlg_game_txt_with_unit((const char *)&str_blewitup_2,entry[4]);
      break;
    case 0x26:
      popup_game_with_sprite((const char *)&str_bond007_2,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c),entry[5]);
      break;
    case 0x27:
      popup_game_with_sprite((const char *)&str_bond_2,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c),entry[5]);
      break;
    case 0x28:
      popup_game_with_sprite((const char *)&str_bondglory_2,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c),entry[5]);
      break;
    case 0x29:
      popup_game_with_improvement((const char *)&str_useweapons,0x3e,8);
      break;
    case 0x2a:
      popup_game_with_improvement((const char *)&str_sdi,0x11,8);
      break;
    case 0x2b:
      popup_game_with_sprite((const char *)&str_changed,(iptr)(((i8 *)&government_sprites_block.government_sprites) + entry[4] * 0x3c),entry[5]);
      break;
    case 0x2c:
      popup_game_with_sprite((const char *)&str_overthrown,(iptr)(((i8 *)&government_sprites_block.government_sprites) + entry[4] * 0x3c),entry[5]);
      break;
    case 0x2d:
      dlg_game_txt_with_unit((const char *)&str_breakcease_2,entry[4]);
      break;
    case 0x2e:
      dlg_game_txt_with_unit((const char *)&str_sneak,entry[4]);
      break;
    case 0x2f:
      dlg_game_txt_with_unit((const char *)&str_missileattack,entry[4]);
      break;
    case 0x30:
      dlg_game_txt_with_unit((const char *)&str_pearlharbor,entry[4]);
      break;
    case 0x31:
      popup_game_with_improvement((const char *)&str_battery2,entry[4],8);
      break;
    case 0x32:
      popup_game_with_improvement((const char *)&str_battery,entry[4],8);
      break;
    case 0x33:
      dlg_game_txt_with_unit((const char *)&str_scramble,entry[4]);
      break;
    case 0x34:
      dlg_game_txt_with_unit((const char *)&str_amphibmotize,entry[4]);
      break;
    case 0x35:
      popup_game((const char *)&str_multiplelose);
      break;
    case 0x36:
      popup_game((const char *)&str_multiplewin);
      break;
    case 0x37:
      dlg_game_txt_with_unit_flags((const char *)&str_ransom,entry[4],0);
      break;
    case 0x38:
      popup_game_with_sprite((const char *)&str_activateally_2,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
      break;
    case 0x39:
      popup_game_with_sprite((const char *)&str_allyhelps_2,(iptr)((game.advance_known_by[0xf] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite)),0);
      break;
    case 0x3a:
      popup_game((const char *)&str_cancelpeace_2);
      break;
    case 0x3b:
      popup_game((const char *)&str_allyunderattack);
      break;
    case 0x3c:
      popup_game((const char *)&str_allyattacking);
      break;
    case 0x3d:
      civs[human_civ[0]].last_contact_turn[entry[4]] = game.turn;
      civs[entry[4]].last_contact_turn[human_civ[0]] = game.turn;
      if ((int)parley_wnd_block.other_civ == -1) {
        parley_wnd_block.other_civ = -2;
        text_clear();
        text = civ_leader_title(entry[4]);
        text_append((const char *)text);
        text_append_space();
        text = (iptr)(civ_leader_name(entry[4]));
        text_append((const char *)text);
        text_append_space();
        text_append_label(0x8c);
        text_append_space();
        text = (iptr)(civ_name_plural(entry[4]));
        text_append((const char *)text);
        text_set_string_arg(0,(const char *)&map_views_block.text_buffer);
        text = (iptr)(civ_leader_name(human_civ[0]));
        text_set_string_arg(1,(const char *)text);
        text = (iptr)(civ_name_plural(human_civ[0]));
        text_set_string_arg(2,(const char *)text);
        if (*(char *)&civs[entry[4]].attitude[human_civ[0]] == '\0') {
          attitude_level = 0;
        }
        else {
          attitude_level = diplo_attitude_level((uint)civs[entry[4]].attitude[human_civ[0]]);
        }
        text = (iptr)(label_string(attitude_names[attitude_level]));
        text_set_string_arg(3,(const char *)text);
        dlg_pending_idle_callback = net_popup_idle_autoclose;
        choice = popup_game_with_sprite((const char *)&str_parleyrequest,(iptr)&popup_info_icon_sprite,0);
        if (net_session_block.net_host_handoff_stage == '\0') {
          if (net_session_block.nm_parley_cancel_count == 0) {
            net_send_message(0x80,net_session_block.net_slots[net_session_block.net_civ_slots[entry[4]]].id,choice,0,0,0,0,0,0,0);
          }
          if (net_session_block.nm_parley_cancel_count == 0) {
            if (choice == 1) {
              text = entry[4];
              operator_delete(entry);
              net_session_block.net_popup_in_progress_i32 = 0;
              parley_wnd_open(&parley_wnd_block,text,2);
              eh_state = 0xffffffff;
              /* inlined unwind funclet 0051399d */ { dialog_dtor(&dlg); }
              goto done;
            }
          }
          else {
            net_session_block.nm_parley_cancel_count = 0;
            text = (iptr)(civ_name_plural(entry[4]));
            text_set_string_arg(0,(const char *)text);
            dlg_pending_idle_callback = turn_popup_timeout_idle;
            popup_game_with_sprite((const char *)&str_parleycancel_4,(iptr)&popup_info_icon_sprite,0);
          }
          parley_wnd_block.other_civ = -1;
          mp_meeting_pending = 0;
        }
        else {
          net_send_message(0x80,net_session_block.net_slots[net_session_block.net_civ_slots[entry[4]]].id,
                             2,0,0,0,0,0,0,0);
          parley_wnd_block.other_civ = -1;
          mp_meeting_pending = 0;
        }
      }
      else {
        net_send_message(0x80,net_session_block.net_slots[net_session_block.net_civ_slots[entry[4]]].id,2,
                           0,0,0,0,0,0,0);
      }
      break;
    case 0x3e:
      dlg_game_txt_with_unit_flags((const char *)&str_upgrade_2,entry[4],(entry[5] == 0) - 1 & 8);
      break;
    case 0x3f:
      if (net_session_block.net_slots[net_session_block.net_civ_slots[human_civ[0]]].id != entry[4])
      {
        popup_game_with_sprite((const char *)&str_newplayer_2,(iptr)&popup_info_icon_sprite,0);
      }
      break;
    case 0x40:
      dlg_load_game_section(&dlg,(const char *)&str_newturntimerclient,0x2000000);
      text = (iptr)(label_string(label_string_ids[0x354]));
      dialog_add_button(&dlg,(const char *)text);
      text = (iptr)(label_string(label_string_ids[0x355]));
      dialog_add_button(&dlg,(const char *)text);
      dlg_show_modal(&dlg,0x28);
      net_send_message(0x56,0,dlg.result_extra,0,0,0,0,0,0,0);
      break;
    case 0x41:
      popup_game_with_sprite((const char *)&str_newturntimerno,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x42:
      popup_game_with_sprite((const char *)&str_newturntimeryes,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x43:
      dlg_load_game_section(&dlg,(const char *)&str_pmchangeclient,0x2000000);
      text = (iptr)(label_string(label_string_ids[0x354]));
      dialog_add_button(&dlg,(const char *)text);
      text = (iptr)(label_string(label_string_ids[0x355]));
      dialog_add_button(&dlg,(const char *)text);
      dlg_show_modal(&dlg,0);
      net_send_message(0x57,0,dlg.result_extra,0,0,0,0,0,0,0);
      break;
    case 0x44:
      popup_game_with_sprite((const char *)&str_pmchangeno_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x45:
      popup_game_with_sprite((const char *)&str_pmchangeyes_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x46:
      snd_play_effect(0x23,1,0,0);
      city_notify_dialog_ex((iptr)&str_manhattan_2,entry[4],1,(iptr)&caption_close_button_sprite_block.city_notify_sprite);
      break;
    case 0x47:
      dialog_load_game_text(&dlg,(const char *)&str_built2_2);
      if (entry[4] < 0) {
        icon_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[5] * 0x3c;
      }
      else {
        icon_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + entry[4] * 0x3c;
      }
      dialog_add_sprite(&dlg,(iptr)icon_sprite,0,0);
      dialog_set_image_zoom(&dlg,8);
      dlg_show_modal(&dlg,0);
      break;
    case 0x48:
      popup_game((const char *)&str_movecapital_2);
      break;
    case 0x49:
      popup_game((const char *)&str_astronauts_2);
      break;
    case 0x4a:
      dialog_load_game_text(&dlg,(const char *)&str_almostwonder_2);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,8);
      dlg_show_modal(&dlg,0);
      break;
    case 0x4b:
      dialog_load_game_text(&dlg,(const char *)&str_stillwonder1);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,8);
      dlg_show_modal(&dlg,0);
      break;
    case 0x4c:
      dialog_load_game_text(&dlg,(const char *)&str_stillwonder2);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,8);
      dlg_show_modal(&dlg,0);
      break;
    case 0x4d:
      dialog_load_game_text(&dlg,(const char *)&str_endwonder_4);
      dialog_add_sprite
                (&dlg,
                 (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                       (char)advances[entry[4]].category * 0xf0 +
                       (char)advances[entry[4]].epoch * 0x3c),0,0);
      dialog_add_sprite
                (&dlg,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[5] * 0x3c),0,0);
      dialog_set_image_zoom(&dlg,8);
      dlg_show_modal(&dlg,0);
      break;
    case 0x4e:
      dlg_game_txt_with_unit((const char *)&str_barbariansland,entry[4]);
      break;
    case 0x4f:
      dialog_init_default(&dlg);
      for (i = 0; i < entry[2]; i++) {
        dialog_add_line
                  (&dlg,(const char *)(((i8 *)&minimap_window_block.text_string_args) + i * 0x104),0xffffffff,
                   0xffffffff,0xffffffff,0);
      }
      dlg_show_modal(&dlg,0);
      break;
    case 0x50:
      popup_game_with_sprite((const char *)&str_warends,(iptr)&ceasefire_icon_sprite,0);
      break;
    case 0x51:
      dlg_game_txt_with_unit((const char *)&str_militaryaid1,entry[4]);
      break;
    case 0x52:
      dlg_game_txt_with_unit((const char *)&str_militaryaid2,entry[4]);
      break;
    case 0x53:
      popup_game_with_sprite((const char *)&str_lostclient_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x54:
      popup_game_with_advance((const char *)&str_goldenage_2,0x3c,8);
      break;
    case 0x55:
      snd_play_effect(0x44,1,0,0);
      popup_game_with_sprite((const char *)&str_enemyembassy_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x56:
      snd_play_effect(0x44,1,0,0);
      popup_game_with_sprite((const char *)&str_enemyinvestigate_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x57:
      choice = civ_has_advance(entry[4],0x23);
      if (choice == 0) {
        snd_play_effect(0x44,1,0,0);
      }
      else {
        snd_play_effect(0x27,1,0,0);
      }
      popup_game_with_sprite((const char *)&str_steal_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x58:
      snd_play_effect(0x44,1,0,0);
      popup_game_with_sprite((const char *)&str_foiledagain,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x59:
      choice = civ_has_advance(entry[5],0x23);
      if (choice == 0) {
        snd_play_effect(0x44,1,0,0);
      }
      else {
        snd_play_effect(0x27,1,0,0);
      }
      popup_game_with_sprite((const char *)&str_sabotageone_2,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c),8);
      break;
    case 0x5a:
      choice = civ_has_advance(entry[4],0x23);
      if (choice == 0) {
        snd_play_effect(0x44,1,0,0);
      }
      else {
        snd_play_effect(0x27,1,0,0);
      }
      popup_game((const char *)&str_sabotagetwo_2);
      break;
    case 0x5b:
      snd_play_effect(0x44,1,0,0);
      popup_game_with_sprite((const char *)&str_watersupply_2,(iptr)&popup_info_icon_sprite,0);
      break;
    case 0x5c:
      text_clear();
      text_append((const char *)&str_plantednuke_2);
      popup_game_with_improvement((const char *)&map_views_block.text_buffer,0x3e,8);
      break;
    case 0x5d:
      text_clear();
      text_append((const char *)&str_plantednuke2);
      popup_game_with_improvement((const char *)&map_views_block.text_buffer,0x3e,8);
      break;
    case 0x5e:
      snd_play_effect(0x44,1,0,0);
      popup_game((const char *)&str_civilwar_2);
      break;
    case 0x5f:
      snd_play_effect(0x5d,0,0,0);
      dlg_game_txt_with_unit((const char *)&str_upmine,entry[4]);
      break;
    case 0x60:
      snd_play_effect(0x5d,0,0,0);
      dlg_game_txt_with_unit((const char *)&str_upyours,entry[4]);
      break;
    case 0x61:
      snd_play_effect(0x5d,0,0,0);
      dlg_game_txt_with_unit((const char *)&str_upyourstoo,entry[4]);
      break;
    case 0x62:
      popup_game((const char *)&str_mercdeclare_2);
      break;
    case 99:
      popup_game_with_advance((const char *)&str_tookciv,entry[4],0);
      break;
    case 100:
      if ((parley_wnd_block.closed == 0) && (entry[4] == (int)parley_wnd_block.other_civ)) {
        parley_reveal_units_prompt_open = 1;
        dlg_load_game_section(&dlg,(const char *)&str_revealunitorigins,0x2000000);
        text = (iptr)(label_string(label_string_ids[0x375]));
        dialog_add_button(&dlg,(const char *)text);
        text = (iptr)(label_string(label_string_ids[0x376]));
        dialog_add_button(&dlg,(const char *)text);
        dlg_show_modal(&dlg,0x14);
        if (dlg.result_extra == 1) {
          parley_wnd_block.local_units_revealed = 1;
          if ((int)parley_wnd_block.mode == 2) {
            ctrl_show_and_redraw((Control *)parley_wnd_block.conceal_button);
            ctrl_enable((Button *)parley_wnd_block.conceal_button);
            control_hide((Control *)parley_wnd_block.reveal_button);
            ctrl_disable((Button *)parley_wnd_block.reveal_button);
          }
          else {
            control_hide((Control *)parley_wnd_block.conceal_button);
            ctrl_disable((Button *)parley_wnd_block.conceal_button);
            control_hide((Control *)parley_wnd_block.reveal_button);
            ctrl_disable((Button *)parley_wnd_block.reveal_button);
          }
          net_send_message(0xa7,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,human_civ[0],
                             (int)(char)parley_wnd_block.local_units_revealed,0,0,0,0,0,0);
          if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 2)) ||
             (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 2)))) {
            ctrl_show_and_redraw((Control *)parley_wnd_block.info_hide_buttons[0]);
            control_hide((Control *)parley_wnd_block.info_show_buttons[0]);
            parley_wnd_block.local_units_revealed = 1;
            ctrl_enable((Button *)parley_wnd_block.sort_by_unit_buttons[0]);
            ctrl_enable((Button *)parley_wnd_block.sort_by_city_buttons[0]);
            parley_draw_unit_list(0);
          }
        }
        parley_reveal_units_prompt_open = 0;
      }
      break;
    case 0x65:
      if ((parley_wnd_block.closed == 0) && (entry[4] == (int)parley_wnd_block.other_civ)) {
        parley_reveal_city_prompt_open = 1;
        dlg_load_game_section(&dlg,(const char *)&str_revealcityinfo,0x2000000);
        text = (iptr)(label_string(label_string_ids[0x377]));
        dialog_add_button(&dlg,(const char *)text);
        text = (iptr)(label_string(label_string_ids[0x376]));
        dialog_add_button(&dlg,(const char *)text);
        dlg_show_modal(&dlg,0x14);
        if (dlg.result_extra == 1) {
          parley_wnd_block.local_cities_revealed = 1;
          if ((int)parley_wnd_block.mode == 2) {
            ctrl_show_and_redraw((Control *)parley_wnd_block.conceal_button);
            ctrl_enable((Button *)parley_wnd_block.conceal_button);
            control_hide((Control *)parley_wnd_block.reveal_button);
            ctrl_disable((Button *)parley_wnd_block.reveal_button);
          }
          else {
            control_hide((Control *)parley_wnd_block.conceal_button);
            ctrl_disable((Button *)parley_wnd_block.conceal_button);
            control_hide((Control *)parley_wnd_block.reveal_button);
            ctrl_disable((Button *)parley_wnd_block.reveal_button);
          }
          net_send_message(0xa5,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,human_civ[0],
                             (int)(char)parley_wnd_block.local_cities_revealed,0,0,0,0,0,0);
          if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 3)) ||
             (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 3)))) {
            ctrl_show_and_redraw((Control *)parley_wnd_block.info_hide_buttons[0]);
            control_hide((Control *)parley_wnd_block.info_show_buttons[0]);
            parley_draw_city_list(0);
          }
        }
        parley_reveal_city_prompt_open = 0;
      }
    }
    operator_delete(entry);
    net_session_block.net_popup_in_progress_i32 = 0;
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0051399d */ { dialog_dtor(&dlg); }
done:
  return;
}

/* 00514220 auto */
NetMsgQueue * __thiscall net_msgq_ctor(NetMsgQueue *self)

{
  _memset(self,0,24000);
  net_msgq_reset(self);
  return self;
}

/* 00514254 auto */
void __thiscall net_msgq_clear(NetMsgQueue *self)

{
  net_msgq_reset(self);
  return;
}

/* 00514272 auto */
void __thiscall net_msgq_reset(NetMsgQueue *self)

{
  u8 *field_ptr;
  int i;
  
  for (i = 0; i < 2000; i++) {
    if (self->slots[i].data != 0) {
      operator_delete((void *)self->slots[i].data);
      field_ptr = (u8 *)&self->slots[i].data;
      field_ptr[0] = '\0';
      field_ptr[1] = '\0';
      field_ptr[2] = '\0';
      field_ptr[3] = '\0';
    }
    field_ptr = (u8 *)&self->slots[i].size;
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
    field_ptr = (u8 *)&self->slots[i].sender;
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
  }
  self->main_head = 400;
  self->main_tail = 400;
  self->main_count = 0;
  self->prio_head = 0;
  self->prio_tail = 0;
  self->prio_count = 0;
  return;
}

/* 0051435f auto */
i32 __cdecl net_msg_is_priority(int msg_type)

{
  i32 is_priority;
  
  if ((msg_type < 0x2a) || (0x66 < msg_type)) {
    is_priority = 0;
  }
  else {
    is_priority = 1;
  }
  return is_priority;
}

/* 0051438f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __thiscall
net_msgq_enqueue(NetMsgQueue *self,int sender,NetMsg *msg,uint msg_size)

{
  int is_priority;
  void *msg_buf;
  i8 lost_msg_text[256];
  iptr msg_copy;
  i8 lost_prio_text[256];
  
  if ((((game.multiplayer == 5) || (game.multiplayer == 6)) && (sender != 0)) &&
     (sender != 0xff)) {
    if (net_session_block.net_host_flag == '\0') {
      sender = 0;
    }
    else {
      sender = 1;
    }
  }
  if ((int)msg->hdr.msg_type == 0x2b) {
    net_session_block.nm_heart_beat_count_i32++;
    net_send_message(0x2c,sender,0,0,0,0,0,0,0,0);
  }
  else if ((int)msg->hdr.msg_type == 0x2c) {
    net_session_block.nm_heartbeat_ack_count++;
  }
  else {
    is_priority = net_msg_is_priority((i32)msg->hdr.msg_type);
    if (is_priority == 0) {
      if ((self->main_head == self->main_tail) && (self->main_count != 0)) {
        crt_strcpy(lost_msg_text,str_message_lost_in_enqueuemessage);
        crt_strcat(lost_msg_text,((char *)&str_nm_datagram) + (int)msg->hdr.msg_type * 0x20);
        crt_strcat(lost_msg_text,str_from_d_size_d_2);
        dbg_logf_int2((const char *)lost_msg_text,sender,msg_size);
        return 0;
      }
      if (((int)self->main_head < 400) || (1999 < (int)self->main_head)) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_head_400_head_max_msgs_in_queue,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_4,0x130);
      }
      if (((int)self->main_tail < 400) || (1999 < (int)self->main_tail)) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_tail_400_tail_max_msgs_in_queue,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_5,0x131);
      }
      msg_buf = operator_new(msg_size);
      self->slots[self->main_tail].data = (i32)msg_buf;
      if (self->slots[self->main_tail].data == 0) {
        dbg_logf_int2((const char *)&str_enqueuemessage_primary_failed_to_allocat,(i32)msg->hdr.msg_type,msg->size);
        return 0;
      }
      if (self->slots[self->main_tail].data == 0) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_msgqueue_tail_pdata,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_6,0x136);
      }
      crt_memcpy((void *)self->slots[self->main_tail].data,(const void *)msg,msg_size);
      self->slots[self->main_tail].sender = sender;
      self->slots[self->main_tail].size = msg_size;
      self->main_tail = (int)(self->main_tail + 1) % 2000;
      if (self->main_tail == 0) {
        self->main_tail = 400;
      }
      self->main_count++;
      if ((int)net_session_block.net_msgq_normal_peak < (int)self->main_count) {
        net_session_block.net_msgq_normal_peak = self->main_count;
      }
    }
    else {
      if ((self->prio_head == self->prio_tail) && (self->prio_count != 0)) {
        crt_strcpy(lost_prio_text,str_message_lost_in_alpha_enqueuemessage);
        crt_strcat(lost_prio_text,((char *)&str_nm_datagram) + (int)msg->hdr.msg_type * 0x20);
        crt_strcat(lost_prio_text,str_from_d_size_d);
        dbg_logf_int2((const char *)lost_prio_text,sender,msg_size);
        return 0;
      }
      if (((int)self->prio_head < 0) || (399 < (int)self->prio_head)) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_alphahead_0_alphahead_400,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp,0x10c);
      }
      if (((int)self->prio_tail < 0) || (399 < (int)self->prio_tail)) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_alphatail_0_alphatail_400,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_2,0x10d);
      }
      msg_buf = operator_new(msg_size);
      self->slots[self->prio_tail].data = (i32)msg_buf;
      if (self->slots[self->prio_tail].data == 0) {
        msg_copy = (iptr)msg;
        dbg_logf_int2((const char *)&str_enqueuemessage_alpha_failed_to_allocate,(i32)msg->hdr.msg_type,msg->size);
        return 0;
      }
      if (self->slots[self->prio_tail].data == 0) {
        err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_msgqueue_alphatail_pdata,(iptr)&str_d_ss_franklinton_netmessagequeue_cpp_3,0x112);
      }
      crt_memcpy((void *)self->slots[self->prio_tail].data,(const void *)msg,msg_size);
      self->slots[self->prio_tail].sender = sender;
      self->slots[self->prio_tail].size = msg_size;
      self->prio_tail = (int)(self->prio_tail + 1) % 400;
      self->prio_count++;
      if ((int)net_session_block.net_msgq_priority_peak < (int)self->prio_count) {
        net_session_block.net_msgq_priority_peak = self->prio_count;
      }
    }
  }
  return 1;
}

/* 005149d6 auto */
i32 __thiscall
net_msgq_dequeue(NetMsgQueue *self,iptr out_sender,iptr out_msg,iptr out_size,int priority_only)

{
  u8 *slot_field;
  i32 result;
  
  if (self->prio_count == 0) {
    if ((priority_only == 0) && (self->main_count != 0)) {
      *(i32 *)out_sender = self->slots[self->main_head].sender;
      slot_field = (u8 *)&self->slots[self->main_head].sender;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      *(i32 *)out_msg = self->slots[self->main_head].data;
      slot_field = (u8 *)&self->slots[self->main_head].data;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      *(i32 *)out_size = self->slots[self->main_head].size;
      slot_field = (u8 *)&self->slots[self->main_head].size;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      self->main_head = (int)(self->main_head + 1) % 2000;
      if (self->main_head == 0) {
        self->main_head = 400;
      }
      self->main_count--;
      result = 1;
    }
    else {
      result = 0;
    }
  }
  else {
    *(i32 *)out_sender = self->slots[self->prio_head].sender;
    slot_field = (u8 *)&self->slots[self->prio_head].sender;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    *(i32 *)out_msg = self->slots[self->prio_head].data;
    slot_field = (u8 *)&self->slots[self->prio_head].data;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    *(i32 *)out_size = self->slots[self->prio_head].size;
    slot_field = (u8 *)&self->slots[self->prio_head].size;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    self->prio_head = (int)(self->prio_head + 1) % 400;
    self->prio_count--;
    result = 1;
  }
  return result;
}

/* 0051ea0c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_vote_wait_idle_cb(void)

{
  int now_ticks;
  
  net_process_messages(&net_session_block,1,0);
  if ((((net_session_block.net_host_handoff_stage == '\0') && (now_ticks = timer_ticks(), now_ticks - dialog_title_font_obj_block.dialog_timeout_start_i32 < 0x961)) &&
      (net_session_block.net_vote_replies_pending != 0)) && (net_session_block.net_vote_status != -1)) {
    return;
  }
  net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
  wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  dialog_title_font_obj_block.dialog_timeout_start_i32 = timer_ticks();
  return;
}

/* 0051f11a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_lock_wait_idle_cb(void)

{
  timer_ticks();
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.nm_lock_data_count != 0) || (net_session_block.nm_server_canceled_count != 0)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 0051f17e auto */
void __cdecl net_pump_idle_cb(void)

{
  net_process_messages(&net_session_block,1,0);
  return;
}

/* 005233fc auto */
i32 __cdecl net_host_register_player_civ(NetPlayerInfoMsg *player_info,int player_id)

{
  int is_female;
  i8 saved_player_civ;
  i8 saved_turn_civ_val;
  i32 result;
  iptr saved_human_civ;
  int title_label_id;
  iptr join_civ;
  int slot;
  u8 female_byte;
  int title_idx;
  u8 portrait;
  
  if (net_session_block.net_host_flag == '\0') {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_gnetmgr_bserver_2,(iptr)&str_d_ss_franklinton_startup_multiplayer_cpp_2,0x778);
  }
  saved_human_civ = human_civ[0];
  saved_turn_civ_val = game.turn_civ;
  saved_player_civ = game.player_civ;
  if (((civ_saved_cursor_x_block.saved_resume_new_game == 0) && (civ_saved_cursor_x_block.saved_turn_civ == '\0')) && (net_session_block.net_session_phase < 2)) {
    is_female = player_info->gender;
    result = player_info->leader_index;
    join_civ = player_info->civ;
    game.player_civ = (byte)join_civ;
    civs[(char)(byte)join_civ].tribe = (i16)result;
    portrait = (u8)result;
    civs[game.player_civ].leader_portrait = portrait;
    if (is_female != 0) {
      civs[game.player_civ].leader_portrait += 21;
    }
    human_civ[0] = (iptr)game.player_civ;
    if (net_session_block.net_session_phase < 3) {
      game.human_players = game.human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net_session_block.pending_human_players = net_session_block.pending_human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    female_byte = (u8)is_female;
    leaders[civs[game.player_civ].tribe].female = female_byte;
    leaders[civs[game.player_civ].tribe].leader_name =
         leaders[civs[game.player_civ].tribe].names[((uint)leaders[civs[game.player_civ].tribe].female)];
    result = (i32)(label_string((int)leaders[civs[game.player_civ].tribe].leader_name));
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),(const char *)((const void *)result));
    if (0 < leaders[civs[game.player_civ].tribe].leader_name) {
      leaders[civs[game.player_civ].tribe].leader_name =
           -leaders[civs[game.player_civ].tribe].leader_name;
    }
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),(const char *)((const void *)((iptr)player_info + 0x20)));
    if (player_info->has_names != 0) {
      leaders[civs[game.player_civ].tribe].plural = -leaders[civs[game.player_civ].tribe].plural;
      leaders[civs[game.player_civ].tribe].adjective =
           -leaders[civs[game.player_civ].tribe].adjective;
      for (title_idx = 0; title_idx < 7; title_idx++) {
        if (leaders[civs[game.player_civ].tribe].titles[title_idx][is_female] < 1)
        {
          title_label_id = ~(int)leaders[civs[game.player_civ].tribe].titles[title_idx][is_female] + 1;
        }
        else {
          title_label_id = (int)leaders[civs[game.player_civ].tribe].titles[title_idx][is_female];
        }
        result = (i32)(label_string(title_label_id));
        crt_strcpy((char *)(&civ_names[game.player_civ].titles[title_idx]),(const char *)((const void *)result));
        leaders[civs[game.player_civ].tribe].titles[title_idx][is_female] =
             -leaders[civs[game.player_civ].tribe].titles[title_idx][is_female];
      }
      crt_strcpy((char *)(&civ_names[game.player_civ].tribe_name),(const char *)((const void *)((iptr)player_info + 0x38)));
      crt_strcpy((char *)(&civ_names[game.player_civ].adjective),(const char *)((const void *)((iptr)player_info + 0x50)));
      if (player_info->has_titles != 0) {
        for (title_idx = 0; title_idx < 7; title_idx++) {
          crt_strcpy((char *)(&civ_names[game.player_civ].titles[title_idx]),
                       (const char *)(title_idx * 0x18 + (iptr)player_info + 0x6c));
        }
      }
    }
    civ_names[game.player_civ].city_style = (i16)player_info->city_style;
  }
  else {
    join_civ = player_info->civ;
    if (net_session_block.net_session_phase < 3) {
      game.human_players = game.human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net_session_block.pending_human_players = net_session_block.pending_human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    game.player_civ = (byte)join_civ;
    game.turn_civ = (byte)join_civ;
    human_civ[0] = join_civ;
    crt_strcpy((char *)(&civ_names[((char)(byte)join_civ)].leader_name),(const char *)((const void *)((iptr)player_info + 0x20)));
    if (0 < leaders[civs[game.player_civ].tribe].leader_name) {
      leaders[civs[game.player_civ].tribe].leader_name =
           -leaders[civs[game.player_civ].tribe].leader_name;
    }
    is_female = player_info->gender;
    civs[human_civ[0]].flags = civs[human_civ[0]].flags & 0xfdff;
    if (is_female != 0) {
      civs[human_civ[0]].flags = civs[human_civ[0]].flags | 0x200;
    }
    female_byte = (u8)is_female;
    leaders[civs[game.player_civ].tribe].female = female_byte;
  }
  for (slot = 0; (slot < 7 && (net_session_block.net_slots[slot].id != player_id));
      slot = slot + 1) {
  }
  if (slot < 7) {
    net_session_block.net_civ_slots[game.player_civ] = slot;
    (((i32 *)&net_session_block.net_slot_civs))[slot * 0x15] = (int)game.player_civ;
    (((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] = 1;
    if (player_id == 0) {
      (((i8 *)&net_session_block.net_slot_host_flags))[slot * 0x54] = 1;
    }
    else {
      (((i8 *)&net_session_block.net_slot_host_flags))[slot * 0x54] = 0;
    }
    result = 1;
  }
  else {
    if (net_session_block.net_session_phase < 3) {
      game.human_players = game.human_players & ~(byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net_session_block.pending_human_players = net_session_block.pending_human_players & ~(byte)(1 << ((byte)join_civ & 0x1f));
    }
    net_send_message(0x31,0,player_info->leader_index,player_info->civ,0,0,0,0,0,0);
    result = 0;
  }
  human_civ[0] = saved_human_civ;
  game.turn_civ = saved_turn_civ_val;
  game.player_civ = saved_player_civ;
  return result;
}

/* 00523d8a auto */
void __cdecl net_send_rules_tables(i32 dest_player)

{
  net_send_message(0x17,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x18,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x19,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x1a,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x1b,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x1c,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x1d,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x1e,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x21,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x22,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x23,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(0x24,dest_player,0,0,0,0,0,0,0,0);
  return;
}

/* 00526ca0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall parley_add_side_dialog(ParleyWindow *self,int side_kind,int side)

{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  int header_x;
  Button *new_ctrl;
  uptr title_x;
  u32 text_x;
  uptr draw_x;
  i32 text_align;
  ParleyOfferMsg *item_buf;
  u8 *city_hide_parent;
  iptr city_hide_label;
  u8 *city_reveal_parent;
  iptr city_reveal_label;
  int city_btn_font_height;
  int city_header_height;
  int city_side_height;
  iptr city_side_title;
  u8 *offer_parent;
  int offer_row_height;
  int offer_header_height;
  int offer_title_height;
  int offer_line_height;
  int civlist_item_height;
  int civlist_box_offset;
  int civlist_header_height;
  int civlist_title_height;
  int civlist_row_height;
  int civlist_side_height;
  iptr civlist_side_title;
  u8 *map_parent;
  int map_box_offset;
  int map_header_height;
  int map_title_height;
  int map_row_height;
  int map_side_height;
  iptr map_side_title;
  u8 *proposal_parent;
  int proposal_row_height;
  int proposal_title_height;
  int proposal_line_height;
  u8 *sort_city_parent;
  u8 *sort_unit_parent;
  int unit_line_spacing;
  int unit_btn_offset;
  u8 *hide_parent;
  iptr hide_label;
  u8 *reveal_parent;
  iptr reveal_label;
  int unit_line_spacing_alt;
  int unit_btn_offset_alt;
  int unit_header_height;
  int unit_side_height;
  iptr unit_side_title;
  u8 *gold_edit_parent;
  int gold_prompt_width;
  int gold_edit_text_width;
  int gold_box_offset;
  int gold_header_height;
  iptr gold_side_title;
  int gold_title_height;
  int gold_line_height;
  int tech_header_height;
  int tech_side_height;
  iptr tech_side_title;
  u8 *gift_parent;
  int gift_row_height;
  int gift_header_height;
  int gift_title_height;
  int gift_line_height;
  int status_box_offset;
  int status_header_height;
  int status_title_height;
  int status_line_height;
  u8 *treaty_parent;
  int treaty_row_height;
  int treaty_row_height_alt;
  int treaty_title_height;
  iptr treaty_title;
  int treaty_line_height;
  int treaty_line_height_alt;
  int treaty_side_height;
  iptr treaty_side_title;
  u8 *exch_parent_r;
  int exch_title_height_r;
  u8 *exch_parent_l;
  int exch_title_height_l;
  u8 *send_parent;
  u8 *mute_parent;
  u8 *broadcast_parent;
  u8 *mute_anon_parent;
  u8 *anon_parent;
  int chat_font_height;
  u32 selected_mode;
  u8 *mode_radio_parent;
  int mode_title_height;
  int waiting_font_height;
  u8 *review_parent;
  u8 *desc_parent;
  i8 *city_header_font;
  i32 city_hide_button;
  i32 city_reveal_button;
  i8 *city_side_font;
  i32 offer_radio;
  i8 *offer_radio_font;
  i8 *offer_header_font;
  i8 *offer_title_font;
  i8 *civlist_header_font;
  i8 *civlist_side_font;
  i32 map_radio;
  i8 *map_radio_font;
  i8 *map_header_font;
  i8 *map_side_font;
  i32 proposal_radio;
  i8 *proposal_radio_font;
  i8 *proposal_title_font;
  i32 sort_city_button;
  i32 sort_unit_button;
  i32 hide_button;
  i32 reveal_button;
  i8 *unit_header_font;
  i8 *unit_side_font;
  i8 *gold_edit_font;
  i32 gold_edit;
  i8 *gold_prompt_font;
  i8 *gold_header_font;
  i8 *gold_side_font;
  i8 *tech_header_font;
  i8 *tech_side_font;
  i32 gift_radio;
  i8 *gift_radio_font;
  i8 *gift_title_font;
  i8 *status_text_font;
  i8 *status_header_font;
  i32 treaty_radio;
  i8 *treaty_radio_font;
  i8 *treaty_title_font;
  i8 *treaty_side_font;
  i32 exch_radio_r;
  i8 *exch_radio_font_r;
  i8 *exch_title_font_r;
  i32 exch_radio_l;
  i8 *exch_radio_font_l;
  i8 *exch_title_font_l;
  i32 send_checkbox;
  i32 mute_checkbox;
  i8 *right_header_font;
  i8 *left_header_font;
  uptr broadcast_checkbox;
  uptr mute_anon_checkbox;
  uptr anon_checkbox;
  i8 *chat_font;
  i32 mode_radio;
  i8 *mode_radio_font;
  i8 *mode_title_font;
  i8 *waiting_font;
  i8 *review_font;
  uptr review_edit;
  i8 *desc_font;
  uptr desc_edit;
  tagRECT city_btn_rect;
  int city_btn_width;
  int city_btn_top;
  iptr city_header_label;
  int city_btn_height;
  i32 city_list_mode;
  i32 offer_labels[7];   /* (the original's locals in a row: the radio group's labels) */
  iptr civlist_title_idx;
  iptr civlist_titles[4];
  uptr map_title_idx;
  iptr map_labels[4];
  i32 map_label_last;
  i32 proposal_labels[2];   /* (the original's locals in a row: the radio group's labels) */
  tagRECT btn_rect;
  int btn_area_bottom;
  int btn_width;
  int btn_top;
  int btn_height;
  i32 unit_list_mode;
  char gold_text[32];
  i32 tech_list_pick_mode;
  i32 gift_labels[5];   /* (the original's locals in a row: the radio group's labels) */
  i32 treaty_labels[4];   /* (the original's locals in a row: the radio group's labels) */
  i32 exch_labels_r[7];   /* (the original's locals in a row: the radio group's labels) */
  i32 exch_labels_l[7];   /* (the original's locals in a row: the radio group's labels) */
  uptr left_col_x;
  uint shown_civ_count;
  int civ;
  u32 right_col_x;
  int has_full_intel;
  int checkbox_indent;
  int max_label_width;
  int label_width;
  iptr label_idx;
  iptr mode_labels[5];
  int waiting_text_width;
  i8 waiting_text[80];
  int *item_kind_ptr;
  iptr *tech_ptr;
  int tech_idx;
  int tech_count;
  int cur_y;
  uptr cur_x;
  int col_width;
  char is_demand_side;
  u32 side_x;
  char is_exchange_side;
  tagRECT ctrl_rect;
  int block_height;
  i32 eh_next;
  i8 *eh_prev;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  if ((side != 0) && (side != 1)) {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_leftright_left_leftright_right_2,(iptr)&str_d_ss_franklinton_parleywin_add_dialog_cp,0xc);
  }
  if (side_kind < 0) {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_thedialog_pdt_null_pdt_null_pdt_count,(iptr)&str_d_ss_franklinton_parleywin_add_dialog_cp_2,0xd);
  }
  is_exchange_side = '\0';
  is_demand_side = '\0';
  flag_ptr = (u8 *)&self->side_selected[side];
  flag_ptr[0] = '\0';
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
  if (side == 0) {
    side_x = self->panel_left + panel_margin_x;
  }
  else if (side == 1) {
    tmp = rect_width((RECT *)&self->panel_left);
    side_x = self->panel_left + tmp / 2 + panel_margin_x;
  }
  if (side_kind == 0xe) {
    is_exchange_side = 1;
    switch((i32)self->exchange_kind[side]) {
    case 0:
      side_kind = 9;
      break;
    case 1:
      side_kind = 8;
      break;
    case 2:
      side_kind = 10;
      break;
    case 3:
      side_kind = 0x11;
      break;
    case 4:
      side_kind = 0xd;
      break;
    case 5:
      side_kind = 0xc;
      break;
    case 6:
      side_kind = 5;
    }
  }
  else if (side_kind == 0x10) {
    is_demand_side = 1;
    switch(self->demand_kind) {
    case 0:
      side_kind = 9;
      break;
    case 1:
      side_kind = 8;
      break;
    case 2:
      side_kind = 10;
      break;
    case 3:
      side_kind = 0x11;
      break;
    case 4:
      side_kind = 0xd;
      break;
    case 5:
      side_kind = 0xc;
      break;
    case 6:
      side_kind = 5;
    }
  }
  parley_clear_side_controls(self,side);
  self->side_kind[side] = side_kind;
  if (((parley_wnd_block.closed == 0) && (side_kind != 1)) && (side_kind != 0)) {
    if (side_kind == 0x14) {
      control_hide((Control *)self->mode_button);
      ctrl_disable((Button *)self->mode_button);
      control_hide((Control *)self->mode2_button);
      ctrl_disable((Button *)self->mode2_button);
      control_hide((Control *)self->never_mind_button);
      ctrl_show_and_redraw((Control *)self->accept_button);
      control_hide((Control *)self->intel_button);
      ctrl_show_and_redraw((Control *)self->no_thanks_button);
      ctrl_show_and_redraw((Control *)self->counter_button);
    }
    else {
      ctrl_show_and_redraw((Control *)self->mode_button);
      ctrl_enable((Button *)self->mode_button);
      control_hide((Control *)self->mode2_button);
      ctrl_disable((Button *)self->mode2_button);
      ctrl_show_and_redraw((Control *)self->never_mind_button);
      control_hide((Control *)self->accept_button);
      ctrl_show_and_redraw((Control *)self->intel_button);
      control_hide((Control *)self->no_thanks_button);
      control_hide((Control *)self->counter_button);
    }
  }
  switch(side_kind) {
  case 0:
    break;
  case 1:
    if (((game.wonder_city[0x18] == -1) && (tmp = civ_has_wonder(human_civ[0],9), tmp == 0)
        ) && (game.reveal_map == '\0')) {
      has_full_intel = 0;
    }
    else {
      has_full_intel = 1;
    }
    right_col_x = side_x;
    left_col_x = self->panel_left + panel_margin_x;
    cur_y = self->panel_top + panel_margin_y;
    cur_x = left_col_x;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 3;
    if (self->large_layout == 0) {
      chat_font_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      chat_font_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = panel_margin_y * 2 + chat_font_height;
    if (self->large_layout == 0) {
      chat_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      chat_font = ((i8 *)&font_size24_bold);
    }
    ui_set_ctrl_font((iptr)chat_font);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 3;
    if (new_ctrl == (Button *)0x0) {
      anon_checkbox = 0;
    }
    else {
      anon_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
    }
    eh_state = 0xffffffff;
    self->anonymous_checkbox = anon_checkbox;
    if (self == (ParleyWindow *)0x0) {
      anon_parent = (u8 *)0x0;
    }
    else {
      anon_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x2e9]));
    checkbox_create_fixed
              ((Checkbox *)self->anonymous_checkbox,(iptr)anon_parent,0x415,cur_x,cur_y,col_width,
               (const char *)label);
    checkbox_set_checked((Checkbox *)self->anonymous_checkbox,(i32)chat_wnd_block.remain_anonymous);
    checkbox_enable((Checkbox *)self->anonymous_checkbox);
    checkbox_set_callback((Checkbox *)self->anonymous_checkbox,(CheckboxToggleFn)chat_on_send_checkbox);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 4;
    if (new_ctrl == (Button *)0x0) {
      mute_anon_checkbox = 0;
    }
    else {
      mute_anon_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
    }
    eh_state = 0xffffffff;
    self->mute_anonymous_checkbox = mute_anon_checkbox;
    if (self == (ParleyWindow *)0x0) {
      mute_anon_parent = (u8 *)0x0;
    }
    else {
      mute_anon_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x361]));
    checkbox_create_fixed
              ((Checkbox *)self->mute_anonymous_checkbox,(iptr)mute_anon_parent,0x430,col_width + cur_x,cur_y,
               col_width,(const char *)label);
    checkbox_set_checked((Checkbox *)self->mute_anonymous_checkbox,(i32)chat_wnd_block.mute_anonymous);
    checkbox_enable((Checkbox *)self->mute_anonymous_checkbox);
    checkbox_set_callback((Checkbox *)self->mute_anonymous_checkbox,(CheckboxToggleFn)chat_on_send_checkbox);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 5;
    if (new_ctrl == (Button *)0x0) {
      broadcast_checkbox = 0;
    }
    else {
      broadcast_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
    }
    eh_state = 0xffffffff;
    self->broadcast_checkbox = broadcast_checkbox;
    if (self == (ParleyWindow *)0x0) {
      broadcast_parent = (u8 *)0x0;
    }
    else {
      broadcast_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x2ea]));
    checkbox_create_fixed
              ((Checkbox *)self->broadcast_checkbox,(iptr)broadcast_parent,0x416,col_width * 2 + cur_x,
               cur_y,col_width,(const char *)label);
    checkbox_set_checked((Checkbox *)self->broadcast_checkbox,(i32)chat_wnd_block.broadcast_all);
    checkbox_enable((Checkbox *)self->broadcast_checkbox);
    checkbox_set_callback((Checkbox *)self->broadcast_checkbox,(CheckboxToggleFn)chat_on_send_checkbox);
    cur_y += block_height;
    gfx_set_text_color_index(10);
    if (self->large_layout == 0) {
      left_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      left_header_font = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    draw_x = left_col_x;
    tmp = cur_y;
    label = (iptr)(label_string(label_string_ids[0x34b]));
    surface_draw_text(self,(iptr)left_header_font,(const char *)label,draw_x,tmp,text_align);
    if (self->large_layout == 0) {
      right_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      right_header_font = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    header_x = right_col_x - ((self->large_layout == 0) - 1 & 0x17);
    tmp = cur_y;
    label = (iptr)(label_string(label_string_ids[0x34b]));
    surface_draw_text(self,(iptr)right_header_font,(const char *)label,header_x,tmp,text_align);
    cur_x = left_col_x;
    if (self->large_layout == 0) {
      checkbox_indent = 5;
    }
    else {
      checkbox_indent = 0x17;
    }
    cur_y += block_height;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2;
    shown_civ_count = 0;
    for (civ = 1; civ < 8; civ++) {
      if (((human_civ[0] == civ) ||
          ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) ||
         (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
          (((has_full_intel == 0 && (civ_saved_cursor_x_block.mp_chat_from_start == 0)) &&
           ((*(byte *)&civs[civ].treaties[human_civ[0]] & 1) == 0)))))) {
        chat_wnd_block.send_to_civs[civ] = 0;
        chat_wnd_block.muted_civs[civ] = 0;
      }
      else {
        new_ctrl = (Button *)(operator_new(0x3c));
        eh_state = 6;
        if (new_ctrl == (Button *)0x0) {
          mute_checkbox = 0;
        }
        else {
          mute_checkbox = (i32)(checkbox_elem_ctor((Checkbox *)new_ctrl));
        }
        eh_state = 0xffffffff;
        self->mute_checkboxes[civ] = mute_checkbox;
        if (self == (ParleyWindow *)0x0) {
          mute_parent = (u8 *)0x0;
        }
        else {
          mute_parent = ((u8 *)self + 0x48);
        }
        checkbox_create_fixed
                  ((Checkbox *)self->mute_checkboxes[civ],(iptr)mute_parent,
                   civ + 0x428,cur_x + checkbox_indent,cur_y,0x23,(const char *)&str_empty_167);
        checkbox_set_checked
                  ((Checkbox *)self->mute_checkboxes[civ],
                   (i32)chat_wnd_block.muted_civs[civ]);
        checkbox_enable((Checkbox *)self->mute_checkboxes[civ]);
        checkbox_set_callback((Checkbox *)self->mute_checkboxes[civ],(CheckboxToggleFn)chat_on_mute_checkbox);
        new_ctrl = (Button *)(operator_new(0x3c));
        eh_state = 7;
        if (new_ctrl == (Button *)0x0) {
          send_checkbox = 0;
        }
        else {
          send_checkbox = (i32)(checkbox_elem_ctor((Checkbox *)new_ctrl));
        }
        eh_state = 0xffffffff;
        self->send_checkboxes[civ] = send_checkbox;
        if (self == (ParleyWindow *)0x0) {
          send_parent = (u8 *)0x0;
        }
        else {
          send_parent = ((u8 *)self + 0x48);
        }
        label = civ_adjective(civ);
        checkbox_create_fixed
                  ((Checkbox *)self->send_checkboxes[civ],(iptr)send_parent,
                   civ + 0x416,cur_x + checkbox_indent + 0x23,cur_y,
                   (col_width - 0x23) - checkbox_indent,(const char *)label);
        checkbox_set_checked
                  ((Checkbox *)self->send_checkboxes[civ],
                   (i32)chat_wnd_block.send_to_civs[civ]);
        checkbox_enable((Checkbox *)self->send_checkboxes[civ]);
        checkbox_set_callback
                  ((Checkbox *)self->send_checkboxes[civ],(CheckboxToggleFn)chat_on_send_checkbox);
        shown_civ_count++;
        if ((shown_civ_count & 1) == 0) {
          cur_x = left_col_x;
        }
        else {
          cur_x = right_col_x;
        }
        if ((shown_civ_count & 1) == 0) {
          if (self->large_layout == 0) {
            checkbox_indent = 5;
          }
          else {
            checkbox_indent = 0x17;
          }
        }
        else if (self->large_layout == 0) {
          checkbox_indent = 5;
        }
        else {
          checkbox_indent = 0;
        }
        if ((shown_civ_count & 1) == 0) {
          cur_y += block_height;
        }
      }
    }
    break;
  case 2:
    mode_labels[0] = (iptr)(label_string(label_string_ids[0x2e4]));
    mode_labels[1] = (iptr)(label_string(label_string_ids[0x2e5]));
    mode_labels[2] = (iptr)(label_string(label_string_ids[0x2e6]));
    mode_labels[3] = (iptr)(label_string(label_string_ids[0x2e7]));
    mode_labels[4] = (iptr)(label_string(label_string_ids[0x2e8]));
    gfx_set_text_color_index(10);
    if (self->large_layout == 0) {
      label = (iptr)(label_string(label_string_ids[0x2e3]));
      max_label_width = font_text_width((GameFont *)((i8 *)&font_size20),(const char *)label);
    }
    else {
      label = (iptr)(label_string(label_string_ids[0x2e3]));
      max_label_width = font_text_width((GameFont *)((i8 *)&font_size30),(const char *)label);
    }
    title_x = (self->panel_left + (int)(self->panel_right - self->panel_left) / 2) - max_label_width / 2;
    tmp = self->panel_top + panel_margin_y;
    if (self->large_layout == 0) {
      mode_title_font = ((i8 *)&font_size20);
    }
    else {
      mode_title_font = ((i8 *)&font_size30);
    }
    text_align = 5;
    cur_y = tmp;
    cur_x = title_x;
    label = (iptr)(label_string(label_string_ids[0x2e3]));
    surface_draw_text(self,(iptr)mode_title_font,(const char *)label,title_x,tmp,text_align);
    if (self->large_layout == 0) {
      max_label_width = font_text_width((GameFont *)((i8 *)&font_size20),(const char *)mode_labels[0]);
    }
    else {
      max_label_width = font_text_width((GameFont *)((i8 *)&font_size30),(const char *)mode_labels[0]);
    }
    for (label_idx = 1; label_idx < 5; label_idx++) {
      if (self->large_layout == 0) {
        label_width = font_text_width
                             ((GameFont *)((i8 *)&font_size20),(const char *)mode_labels[label_idx]);
      }
      else {
        label_width = font_text_width
                             ((GameFont *)((i8 *)&font_size30),(const char *)mode_labels[label_idx]);
      }
      if (max_label_width < label_width) {
        max_label_width = label_width;
      }
    }
    side_x = (self->panel_left + (int)(self->panel_right - self->panel_left) / 2) - max_label_width / 2;
    self->fill_left = side_x;
    if ((int)cur_x < (int)side_x) {
      side_x = cur_x;
      self->fill_left = cur_x;
    }
    if (self->large_layout == 0) {
      mode_radio_font = ((i8 *)&font_size20);
    }
    else {
      mode_radio_font = ((i8 *)&font_size30);
    }
    ctrl_set_pending_font((iptr)mode_radio_font);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 2;
    if (new_input == (EditControl *)0x0) {
      mode_radio = 0;
    }
    else {
      mode_radio = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = mode_radio;
    cur_x = side_x;
    col_width = max_label_width + max_label_width / 2;
    if (self->large_layout == 0) {
      mode_title_height = font_cached_height((GameFont *)((i8 *)&font_size20));
    }
    else {
      mode_title_height = font_cached_height((GameFont *)((i8 *)&font_size30));
    }
    cur_y = self->panel_top + panel_margin_y * 2 + mode_title_height;
    block_height = self->divider_y - cur_y;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    if (self == (ParleyWindow *)0x0) {
      mode_radio_parent = (u8 *)0x0;
    }
    else {
      mode_radio_parent = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)mode_radio_parent,0x3f3,
               (iptr)&ctrl_rect,1,5,(iptr)mode_labels);
    if (self->selected_action == 0xffffffff) {
      selected_mode = 0;
    }
    else {
      selected_mode = self->selected_action;
    }
    radiogrp_set_selected((RadioGroup *)self->side_radios[side],selected_mode);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 3:
    exch_labels_l[0] = (i32)(label_string(label_string_ids[0x2eb]));
    exch_labels_l[1] = (i32)(label_string(label_string_ids[0x2ec]));
    exch_labels_l[2] = (i32)(label_string(label_string_ids[0x2ed]));
    exch_labels_l[3] = (i32)(label_string(label_string_ids[750]));
    exch_labels_l[4] = (i32)(label_string(label_string_ids[0x2ef]));
    exch_labels_l[5] = (i32)(label_string(label_string_ids[0x2f0]));
    exch_labels_l[6] = (i32)(label_string(label_string_ids[0x342]));
    gfx_set_text_color_index(10);
    cur_x = side_x;
    tmp = self->panel_top + panel_margin_y;
    if (self->large_layout == 0) {
      exch_title_font_l = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      exch_title_font_l = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    text_x = side_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x303]));
    surface_draw_text(self,(iptr)exch_title_font_l,(const char *)label,text_x,tmp,text_align);
    if (self->large_layout == 0) {
      exch_radio_font_l = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      exch_radio_font_l = ((i8 *)&font_size24_bold);
    }
    ctrl_set_pending_font((iptr)exch_radio_font_l);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 8;
    if (new_input == (EditControl *)0x0) {
      exch_radio_l = 0;
    }
    else {
      exch_radio_l = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = exch_radio_l;
    cur_x = side_x;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2;
    if (self->large_layout == 0) {
      exch_title_height_l = font_cached_height((GameFont *)((i8 *)&font_size16));
    }
    else {
      exch_title_height_l = font_cached_height((GameFont *)((i8 *)&font_size24));
    }
    cur_y = self->panel_top + panel_margin_y + exch_title_height_l;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,self->divider_y);
    if (self == (ParleyWindow *)0x0) {
      exch_parent_l = (u8 *)0x0;
    }
    else {
      exch_parent_l = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)exch_parent_l,0x3f4,
               (iptr)&ctrl_rect,1,7,(iptr)exch_labels_l);
    if (self->exchange_kind[1] == 0) {
      radiogrp_disable_item((RadioGroup *)self->side_radios[side],0);
    }
    else if (self->exchange_kind[1] == 6) {
      radiogrp_disable_item((RadioGroup *)self->side_radios[side],6);
    }
    radiogrp_set_selected
              ((RadioGroup *)self->side_radios[side],self->exchange_kind[0]);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    radiogrp_set_select_callback((RadioGroup *)self->side_radios[side],parley_on_exchange_kind_select)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 4:
    exch_labels_r[0] = (i32)(label_string(label_string_ids[0x2eb]));
    exch_labels_r[1] = (i32)(label_string(label_string_ids[0x2ec]));
    exch_labels_r[2] = (i32)(label_string(label_string_ids[0x2ed]));
    exch_labels_r[3] = (i32)(label_string(label_string_ids[750]));
    exch_labels_r[4] = (i32)(label_string(label_string_ids[0x2ef]));
    exch_labels_r[5] = (i32)(label_string(label_string_ids[0x2f1]));
    exch_labels_r[6] = (i32)(label_string(label_string_ids[0x342]));
    gfx_set_text_color_index(10);
    cur_x = side_x;
    tmp = self->panel_top + panel_margin_y;
    if (self->large_layout == 0) {
      exch_title_font_r = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      exch_title_font_r = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    text_x = side_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x304]));
    surface_draw_text(self,(iptr)exch_title_font_r,(const char *)label,text_x,tmp,text_align);
    if (self->large_layout == 0) {
      exch_radio_font_r = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      exch_radio_font_r = ((i8 *)&font_size24_bold);
    }
    ctrl_set_pending_font((iptr)exch_radio_font_r);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 9;
    if (new_input == (EditControl *)0x0) {
      exch_radio_r = 0;
    }
    else {
      exch_radio_r = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = exch_radio_r;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2;
    cur_x = side_x;
    if (self->large_layout == 0) {
      exch_title_height_r = font_cached_height((GameFont *)((i8 *)&font_size16));
    }
    else {
      exch_title_height_r = font_cached_height((GameFont *)((i8 *)&font_size24));
    }
    cur_y = self->panel_top + panel_margin_y + exch_title_height_r;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,self->divider_y);
    if (self == (ParleyWindow *)0x0) {
      exch_parent_r = (u8 *)0x0;
    }
    else {
      exch_parent_r = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)exch_parent_r,0x3f5,
               (iptr)&ctrl_rect,1,7,(iptr)exch_labels_r);
    if (self->exchange_kind[0] == 0) {
      radiogrp_disable_item((RadioGroup *)self->side_radios[side],0);
    }
    else if (self->exchange_kind[0] == 6) {
      radiogrp_disable_item((RadioGroup *)self->side_radios[side],6);
    }
    radiogrp_set_selected
              ((RadioGroup *)self->side_radios[side],self->exchange_kind[1]);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    radiogrp_set_select_callback((RadioGroup *)self->side_radios[side],parley_on_exchange_kind_select)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 5:
    treaty_labels[0] = (i32)(label_string(label_string_ids[0x2f3]));
    treaty_labels[1] = (i32)(label_string(label_string_ids[0x2f4]));
    treaty_labels[2] = (i32)(label_string(label_string_ids[0x2f5]));
    treaty_labels[3] = (i32)(label_string(label_string_ids[0x2f6]));
    gfx_set_text_color_index(10);
    cur_x = side_x;
    cur_y = self->panel_top + panel_margin_y;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 + panel_margin_x * -5;
    if ((is_exchange_side != '\0') || (is_demand_side != '\0')) {
      str_clear((char *)&map_views_block.text_buffer);
      if (side == 0) {
        treaty_side_title = (iptr)(label_string(label_string_ids[0x303]));
      }
      else {
        treaty_side_title = (iptr)(label_string(label_string_ids[0x304]));
      }
      text_append_str((const char *)treaty_side_title);
      if (self->large_layout == 0) {
        treaty_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        treaty_side_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)treaty_side_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,5);
      if (self->large_layout == 0) {
        treaty_side_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        treaty_side_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      cur_y = cur_y + panel_margin_y + treaty_side_height;
    }
    if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
      if (self->large_layout == 0) {
        treaty_line_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        treaty_line_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      block_height = treaty_line_height * 5 + panel_margin_y * 4;
    }
    else {
      if (self->large_layout == 0) {
        treaty_line_height_alt = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        treaty_line_height_alt = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      block_height = panel_margin_y * 3 + treaty_line_height_alt * 4;
    }
    cur_y = self->panel_top + (int)((self->divider_y - self->panel_top) - block_height) / 2;
    if (is_demand_side == '\0' && is_exchange_side == '\0') {
      treaty_title = (iptr)(label_string(label_string_ids[0x2e4]));
    }
    else {
      treaty_title = (iptr)(label_string(label_string_ids[0x342]));
    }
    if (self->large_layout == 0) {
      treaty_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      treaty_title_font = ((i8 *)&font_size24_bold);
    }
    surface_draw_text(self,(iptr)treaty_title_font,(const char *)treaty_title,cur_x,cur_y,5)
    ;
    if (self->large_layout == 0) {
      treaty_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      treaty_radio_font = ((i8 *)&font_size24_bold);
    }
    ctrl_set_pending_font((iptr)treaty_radio_font);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 10;
    if (new_input == (EditControl *)0x0) {
      treaty_radio = 0;
    }
    else {
      treaty_radio = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = treaty_radio;
    cur_x = side_x;
    if (self->large_layout == 0) {
      treaty_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      treaty_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y = cur_y + panel_margin_y + treaty_title_height;
    if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
      if (self->large_layout == 0) {
        treaty_row_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        treaty_row_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      block_height = panel_margin_y * 3 + treaty_row_height * 4;
    }
    else {
      if (self->large_layout == 0) {
        treaty_row_height_alt = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        treaty_row_height_alt = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      block_height = treaty_row_height_alt * 3 + panel_margin_y * 2;
    }
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    if (self == (ParleyWindow *)0x0) {
      treaty_parent = (u8 *)0x0;
    }
    else {
      treaty_parent = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)treaty_parent,0x3f6,
               (iptr)&ctrl_rect,1,(is_demand_side == '\0' && is_exchange_side == '\0') + 3,(iptr)treaty_labels);
    if (((*(byte *)&civs[human_civ[0]].treaties[self->other_civ] & 2) != 0) &&
       (radiogrp_disable_item((RadioGroup *)self->side_radios[side],0),
       self->treaty_kind == 0)) {
      self->treaty_kind++;
    }
    if (((*(byte *)&civs[human_civ[0]].treaties[self->other_civ] & 4) != 0) &&
       (radiogrp_disable_item((RadioGroup *)self->side_radios[side],1),
       self->treaty_kind == 1)) {
      self->treaty_kind++;
    }
    if (((*(byte *)&civs[human_civ[0]].treaties[self->other_civ] & 8) != 0) &&
       (radiogrp_disable_item((RadioGroup *)self->side_radios[side],2),
       self->treaty_kind == 2)) {
      if (is_demand_side == '\0' && is_exchange_side == '\0') {
        self->treaty_kind = 3;
      }
      else {
        self->treaty_kind = 0;
      }
    }
    if ((((is_exchange_side == '\0') && (is_demand_side == '\0')) &&
        ((*(byte *)&civs[human_civ[0]].treaties[self->other_civ] & 0xe) == 0)) &&
       (radiogrp_disable_item((RadioGroup *)self->side_radios[side],3),
       self->treaty_kind == 3)) {
      self->treaty_kind = 0;
    }
    radiogrp_set_selected
              ((RadioGroup *)self->side_radios[side],self->treaty_kind);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    radiogrp_set_select_callback((RadioGroup *)self->side_radios[side],parley_on_treaty_type_select)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 6:
    gfx_set_text_color_index(10);
    cur_x = side_x;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 - panel_margin_x;
    if (self->large_layout == 0) {
      status_line_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      status_line_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    if (self->large_layout == 0) {
      status_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      status_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = panel_margin_y * 2 + status_line_height * 4 + status_title_height;
    cur_y = self->panel_top + (int)((self->divider_y - self->panel_top) - block_height) / 2;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    gfx_set_draw_target((iptr)&parley_wnd_block);
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    gfx_target_frame_rect(&ctrl_rect,10);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    if (self->large_layout == 0) {
      status_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      status_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    tmp = cur_y + panel_margin_y + status_header_height / 2;
    if (self->large_layout == 0) {
      status_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      status_header_font = ((i8 *)&font_size24_bold);
    }
    text_align = 0;
    draw_x = cur_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x2f7]));
    surface_draw_text(self,(iptr)status_header_font,(const char *)label,draw_x,tmp,text_align);
    ctrl_rect.bottom += panel_margin_y * -2;
    ctrl_rect.left += panel_margin_x * 2;
    ctrl_rect.right += panel_margin_x * -2;
    if (self->large_layout == 0) {
      status_box_offset = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      status_box_offset = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    ctrl_rect.top = ctrl_rect.top + panel_margin_y + status_box_offset;
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    surface_fill_rect(self,(iptr)&ctrl_rect,list_bg_color);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    tmp = rect_height(&ctrl_rect);
    cur_y = ctrl_rect.top + tmp / 2;
    text_clear();
    if ((*(byte *)&civs[human_civ[0]].treaties[self->other_civ] & 8) == 0) {
      if ((*(byte *)&civs[human_civ[0]].treaties[self->other_civ] & 4) == 0) {
        if ((*(byte *)&civs[human_civ[0]].treaties[self->other_civ] & 2) == 0) {
          if ((*(byte *)(human_civ[0] * 0x594 + (iptr)((u8 *)&civs + 33) + self->other_civ * 4) & 0x20) == 0) {
            label = (iptr)(label_string(label_string_ids[0x2fb]));
            text_append((const char *)label);
          }
          else {
            label = (iptr)(label_string(label_string_ids[0x2fa]));
            text_append((const char *)label);
          }
        }
        else {
          label = (iptr)(label_string(label_string_ids[0x2f3]));
          text_append((const char *)label);
        }
      }
      else {
        label = (iptr)(label_string(label_string_ids[0x2f9]));
        text_append((const char *)label);
      }
    }
    else {
      label = (iptr)(label_string(label_string_ids[0x2f8]));
      text_append((const char *)label);
    }
    if (self->large_layout == 0) {
      status_text_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      status_text_font = ((i8 *)&font_size24_bold);
    }
    surface_draw_text(self,(iptr)status_text_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,0);
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 7:
    gift_labels[0] = (i32)(label_string(label_string_ids[0x2fd]));
    gift_labels[1] = (i32)(label_string(label_string_ids[0x2fe]));
    gift_labels[2] = (i32)(label_string(label_string_ids[0x2ff]));
    gift_labels[3] = (i32)(label_string(label_string_ids[0x300]));
    gift_labels[4] = (i32)(label_string(label_string_ids[0x301]));
    gfx_set_text_color_index(10);
    cur_x = side_x;
    if (self->large_layout == 0) {
      gift_line_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      gift_line_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    if (self->large_layout == 0) {
      gift_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      gift_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = panel_margin_y * 5 + gift_line_height * 5 + gift_title_height;
    tmp = self->panel_top + (int)((self->divider_y - self->panel_top) - block_height) / 2;
    if (self->large_layout == 0) {
      gift_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      gift_title_font = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    draw_x = cur_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x2fc]));
    surface_draw_text(self,(iptr)gift_title_font,(const char *)label,draw_x,tmp,text_align);
    if (self->large_layout == 0) {
      gift_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      gift_radio_font = ((i8 *)&font_size24_bold);
    }
    ctrl_set_pending_font((iptr)gift_radio_font);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 0xb;
    if (new_input == (EditControl *)0x0) {
      gift_radio = 0;
    }
    else {
      gift_radio = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = gift_radio;
    cur_x = side_x;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2;
    if (self->large_layout == 0) {
      gift_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      gift_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y = cur_y + panel_margin_y + gift_header_height;
    if (self->large_layout == 0) {
      gift_row_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      gift_row_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = gift_row_height * 5 + panel_margin_y * 4;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    if (self == (ParleyWindow *)0x0) {
      gift_parent = (u8 *)0x0;
    }
    else {
      gift_parent = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)gift_parent,0x3f7,
               (iptr)&ctrl_rect,1,5,(iptr)gift_labels);
    radiogrp_set_selected
              ((RadioGroup *)self->side_radios[side],self->gift_kind);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    radiogrp_set_select_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_gift_kind_select)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 8:
    if (is_exchange_side == '\0') {
      flag_ptr = (u8 *)&self->list_rows[side];
      flag_ptr[0] = 7;
      flag_ptr[1] = '\0';
      flag_ptr[2] = '\0';
      flag_ptr[3] = '\0';
    }
    else {
      flag_ptr = (u8 *)&self->list_rows[side];
      flag_ptr[0] = 6;
      flag_ptr[1] = '\0';
      flag_ptr[2] = '\0';
      flag_ptr[3] = '\0';
    }
    gfx_set_text_color_index(10);
    cur_x = side_x;
    cur_y = self->panel_top + panel_margin_y;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 + panel_margin_x * -5;
    if (is_exchange_side != '\0') {
      str_clear((char *)&map_views_block.text_buffer);
      if (side == 0) {
        tech_side_title = (iptr)(label_string(label_string_ids[0x303]));
      }
      else {
        tech_side_title = (iptr)(label_string(label_string_ids[0x304]));
      }
      text_append_str((const char *)tech_side_title);
      if (self->large_layout == 0) {
        tech_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        tech_side_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)tech_side_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,5);
      if (self->large_layout == 0) {
        tech_side_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        tech_side_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      cur_y = cur_y + panel_margin_y + tech_side_height;
    }
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,self->divider_y);
    gfx_set_draw_target((iptr)&parley_wnd_block);
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    gfx_target_frame_rect(&ctrl_rect,10);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    if (self->large_layout == 0) {
      tech_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      tech_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    tmp = cur_y + tech_header_height / 2;
    if (self->large_layout == 0) {
      tech_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      tech_header_font = ((i8 *)&font_size24_bold);
    }
    text_align = 0;
    draw_x = cur_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x302]));
    surface_draw_text(self,(iptr)tech_header_font,(const char *)label,draw_x,tmp,text_align);
    if (self->large_layout == 0) {
      tmp = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      ctrl_rect.top += tmp;
    }
    else {
      tmp = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      ctrl_rect.top += tmp;
    }
    ctrl_rect.bottom = self->divider_y + panel_margin_y * -2;
    ctrl_rect.left += panel_margin_x * 2;
    ctrl_rect.right += panel_margin_x * -2;
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    surface_fill_rect(self,(iptr)&ctrl_rect,list_bg_color);
    tech_list_pick_mode = 0;
    if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
      tech_list_pick_mode = 1;
    }
    parley_create_advance_list_pane(self,&ctrl_rect,side,tech_list_pick_mode);
    break;
  case 9:
    gfx_set_text_color_index(10);
    cur_x = side_x;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 + panel_margin_x * -5;
    if (self->large_layout == 0) {
      gold_line_height = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      gold_line_height = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    if (self->large_layout == 0) {
      gold_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      gold_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = panel_margin_y * 2 + gold_line_height * 4 + gold_title_height;
    cur_y = self->panel_top + (int)((self->divider_y - self->panel_top) - block_height) / 2;
    if (is_exchange_side != '\0') {
      str_clear((char *)&map_views_block.text_buffer);
      if (side == 0) {
        gold_side_title = (iptr)(label_string(label_string_ids[0x303]));
      }
      else {
        gold_side_title = (iptr)(label_string(label_string_ids[0x304]));
      }
      text_append_str((const char *)gold_side_title);
      if (self->large_layout == 0) {
        gold_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        gold_side_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)gold_side_font,(const char *)&map_views_block.text_buffer,cur_x,
                 self->panel_top + panel_margin_y,5);
    }
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    gfx_set_draw_target((iptr)&parley_wnd_block);
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    gfx_target_frame_rect(&ctrl_rect,10);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    if (self->large_layout == 0) {
      gold_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      gold_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y = cur_y + panel_margin_y + gold_header_height / 2;
    str_clear((char *)&map_views_block.text_buffer);
    if (is_demand_side == '\0') {
      label = (iptr)(label_string(label_string_ids[0x306]));
      text_append_str((const char *)label);
      text_append_space();
      text_append_int(civs[human_civ[0]].gold);
      text_append_space();
      label = (iptr)(label_string(label_string_ids[0x307]));
      text_append_str((const char *)label);
    }
    else {
      label = (iptr)(label_string(label_string_ids[0x305]));
      text_append_str((const char *)label);
    }
    if (self->large_layout == 0) {
      gold_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      gold_header_font = ((i8 *)&font_size24_bold);
    }
    surface_draw_text(self,(iptr)gold_header_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,0);
    ctrl_rect.bottom += panel_margin_y * -2;
    ctrl_rect.left += panel_margin_x * 2;
    ctrl_rect.right += panel_margin_x * -2;
    if (self->large_layout == 0) {
      gold_box_offset = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      gold_box_offset = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    ctrl_rect.top = ctrl_rect.top + panel_margin_y + gold_box_offset;
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    surface_fill_rect(self,(iptr)&ctrl_rect,list_bg_color);
    str_clear((char *)&map_views_block.text_buffer);
    text_append_str((const char *)&str_000000);
    if (self->large_layout == 0) {
      gold_edit_text_width = font_text_width((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold),(const char *)&map_views_block.text_buffer);
    }
    else {
      gold_edit_text_width = font_text_width((GameFont *)((i8 *)&font_size24_bold),(const char *)&map_views_block.text_buffer);
    }
    col_width = (gold_edit_text_width * 3) / 2;
    str_clear((char *)&map_views_block.text_buffer);
    label = (iptr)(label_string(label_string_ids[0x308]));
    text_append_str((const char *)label);
    text_append_space();
    if (self->large_layout == 0) {
      gold_prompt_width = font_text_width((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold),(const char *)&map_views_block.text_buffer);
    }
    else {
      gold_prompt_width = font_text_width((GameFont *)((i8 *)&font_size24_bold),(const char *)&map_views_block.text_buffer);
    }
    tmp = rect_width(&ctrl_rect);
    cur_x = (ctrl_rect.left + tmp / 2) - (col_width + gold_prompt_width) / 2;
    tmp = rect_height(&ctrl_rect);
    cur_y = ctrl_rect.top + tmp / 2;
    if (self->large_layout == 0) {
      gold_prompt_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      gold_prompt_font = ((i8 *)&font_size24_bold);
    }
    surface_draw_text(self,(iptr)gold_prompt_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,1);
    new_input = (EditControl *)(operator_new(0x48));
    eh_state = 0xc;
    if (new_input == (EditControl *)0x0) {
      gold_edit = 0;
    }
    else {
      gold_edit = (i32)(edit_ctor(new_input));
    }
    eh_state = 0xffffffff;
    self->gold_edits[side] = gold_edit;
    if (self->large_layout == 0) {
      gold_edit_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      gold_edit_font = ((i8 *)&font_size24_bold);
    }
    ui_set_default_font((iptr)gold_edit_font);
    if (self->large_layout == 0) {
      tmp = font_text_width((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold),(const char *)&map_views_block.text_buffer);
      cur_x += tmp;
    }
    else {
      tmp = font_text_width((GameFont *)((i8 *)&font_size24_bold),(const char *)&map_views_block.text_buffer);
      cur_x += tmp;
    }
    if (self->large_layout == 0) {
      block_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      block_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y -= block_height / 2;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    __itoa((int)self->gold_amount[side],gold_text,10);
    if (self == (ParleyWindow *)0x0) {
      gold_edit_parent = (u8 *)0x0;
    }
    else {
      gold_edit_parent = ((u8 *)self + 0x48);
    }
    edit_create
              ((EditControl *)self->gold_edits[side],(iptr)gold_edit_parent,
               side + 0x3f8,(iptr)&ctrl_rect,(const char *)gold_text);
    edit_set_max_len((EditControl *)self->gold_edits[side],6);
    edit_set_change_callback((EditControl *)self->gold_edits[side],parley_gold_edit_on_change);
    ctrl_set_focus((Control *)self->gold_edits[side]);
    break;
  case 10:
    if (is_exchange_side == '\0') {
      flag_ptr = (u8 *)&self->list_rows[side];
      flag_ptr[0] = 6;
      flag_ptr[1] = '\0';
      flag_ptr[2] = '\0';
      flag_ptr[3] = '\0';
    }
    else {
      flag_ptr = (u8 *)&self->list_rows[side];
      flag_ptr[0] = 5;
      flag_ptr[1] = '\0';
      flag_ptr[2] = '\0';
      flag_ptr[3] = '\0';
    }
    gfx_set_text_color_index(10);
    cur_x = side_x;
    cur_y = self->panel_top + panel_margin_y;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 + panel_margin_x * -5;
    if (is_exchange_side != '\0') {
      str_clear((char *)&map_views_block.text_buffer);
      if (side == 0) {
        unit_side_title = (iptr)(label_string(label_string_ids[0x303]));
      }
      else {
        unit_side_title = (iptr)(label_string(label_string_ids[0x304]));
      }
      text_append_str((const char *)unit_side_title);
      if (self->large_layout == 0) {
        unit_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        unit_side_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)unit_side_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,5);
      if (self->large_layout == 0) {
        unit_side_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        unit_side_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      cur_y = cur_y + panel_margin_y + unit_side_height;
    }
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,self->divider_y);
    gfx_set_draw_target((iptr)&parley_wnd_block);
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    gfx_target_frame_rect(&ctrl_rect,10);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    if (self->large_layout == 0) {
      unit_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      unit_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    tmp = cur_y + unit_header_height / 2;
    cur_y = tmp;
    if (is_exchange_side == '\0') {
      if (self->large_layout == 0) {
        unit_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        unit_header_font = ((i8 *)&font_size24_bold);
      }
      text_align = 0;
      draw_x = cur_x;
      label = (iptr)(label_string(label_string_ids[0x309]));
      surface_draw_text(self,(iptr)unit_header_font,(const char *)label,draw_x,tmp,text_align);
    }
    if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
      if (self->large_layout == 0) {
        unit_btn_offset = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        unit_btn_offset = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      btn_top = ctrl_rect.top + unit_btn_offset;
      btn_width = col_width / 2;
      if (self->large_layout == 0) {
        unit_line_spacing = font_line_spacing((GameFont *)((i8 *)&font_size16));
      }
      else {
        unit_line_spacing = font_line_spacing((GameFont *)((i8 *)&font_size24));
      }
      btn_height = panel_padding_y * 2 + unit_line_spacing;
      SetRect(&btn_rect,panel_margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
              btn_top + btn_height);
    }
    else {
      btn_top = panel_margin_y * 2 + ctrl_rect.top - 1;
      if (self->large_layout == 0) {
        unit_btn_offset_alt = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        unit_btn_offset_alt = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      if (self->large_layout == 0) {
        unit_line_spacing_alt = font_line_spacing((GameFont *)((i8 *)&font_size16));
      }
      else {
        unit_line_spacing_alt = font_line_spacing((GameFont *)((i8 *)&font_size24));
      }
      btn_area_bottom = ctrl_rect.top + panel_padding_y * 2 + unit_btn_offset_alt + unit_line_spacing_alt;
      btn_width = col_width + panel_margin_x * -3;
      btn_height = (btn_area_bottom - btn_top) / 2;
      SetRect(&btn_rect,panel_margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
              btn_top + btn_height);
      new_ctrl = (Button *)(operator_new(0x3c));
      eh_state = 0xd;
      if (new_ctrl == (Button *)0x0) {
        reveal_button = 0;
      }
      else {
        reveal_button = (i32)(button_ctor(new_ctrl));
      }
      eh_state = 0xffffffff;
      self->info_show_buttons[side] = reveal_button;
      if (side == 0) {
        reveal_label = (iptr)(label_string(label_string_ids[0x36f]));
      }
      else {
        reveal_label = (iptr)(label_string(label_string_ids[0x370]));
      }
      if (self == (ParleyWindow *)0x0) {
        reveal_parent = (u8 *)0x0;
      }
      else {
        reveal_parent = ((u8 *)self + 0x48);
      }
      button_create
                ((Button *)self->info_show_buttons[side],(iptr)reveal_parent,
                 side + 0x404,(iptr)&btn_rect,(const char *)reveal_label);
      button_set_callback
                ((Button *)self->info_show_buttons[side],
                 (CtrlNotifyFn)(side == 0 ? parley_reveal_city_origin_clicked : parley_request_city_origin_clicked));
      new_ctrl = (Button *)(operator_new(0x3c));
      eh_state = 0xe;
      if (new_ctrl == (Button *)0x0) {
        hide_button = 0;
      }
      else {
        hide_button = (i32)(button_ctor(new_ctrl));
      }
      eh_state = 0xffffffff;
      self->info_hide_buttons[side] = hide_button;
      if (side == 0) {
        hide_label = (iptr)(label_string(label_string_ids[0x371]));
      }
      else {
        hide_label = (iptr)(label_string(label_string_ids[0x370]));
      }
      if (self == (ParleyWindow *)0x0) {
        hide_parent = (u8 *)0x0;
      }
      else {
        hide_parent = ((u8 *)self + 0x48);
      }
      button_create
                ((Button *)self->info_hide_buttons[side],(iptr)hide_parent,
                 side + 0x406,(iptr)&btn_rect,(const char *)hide_label);
      button_set_callback
                ((Button *)self->info_hide_buttons[side],
                 (CtrlNotifyFn)(side == 0 ? parley_hide_city_origin_clicked : parley_request_city_origin_clicked));
      if ((side == 0) && (self->local_units_revealed != '\0')) {
        control_hide((Control *)self->info_show_buttons[0]);
        ctrl_show_and_redraw((Control *)self->info_hide_buttons[0]);
      }
      else if (side == 0) {
        ctrl_show_and_redraw((Control *)self->info_show_buttons[0]);
        control_hide((Control *)self->info_hide_buttons[0]);
      }
      if ((side == 1) && (self->remote_units_revealed != '\0')) {
        control_hide((Control *)self->info_show_buttons[1]);
        ctrl_show_and_redraw((Control *)self->info_hide_buttons[1]);
        ctrl_disable((Button *)self->info_hide_buttons[1]);
      }
      else if (side == 1) {
        ctrl_show_and_redraw((Control *)self->info_show_buttons[1]);
        control_hide((Control *)self->info_hide_buttons[1]);
      }
      btn_top += btn_height;
      btn_width = col_width / 2;
      SetRect(&btn_rect,panel_margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
              btn_top + btn_height);
    }
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 0xf;
    if (new_ctrl == (Button *)0x0) {
      sort_unit_button = 0;
    }
    else {
      sort_unit_button = (i32)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->sort_by_unit_buttons[side] = sort_unit_button;
    if (self == (ParleyWindow *)0x0) {
      sort_unit_parent = (u8 *)0x0;
    }
    else {
      sort_unit_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x30a]));
    button_create
              ((Button *)self->sort_by_unit_buttons[side],(iptr)sort_unit_parent,
               side + 0x400,(iptr)&btn_rect,(const char *)label);
    button_set_callback
              ((Button *)self->sort_by_unit_buttons[side],
               (CtrlNotifyFn)(side == 0 ? parley_sort_by_unit_left_clicked : parley_sort_by_unit_right_clicked));
    if (((int)parley_wnd_block.mode != 9) &&
       (((side == 0 && (self->local_units_revealed == '\0')) ||
        ((side == 1 && (self->remote_units_revealed == '\0')))))) {
      ctrl_disable((Button *)self->sort_by_unit_buttons[side]);
    }
    SetRect(&btn_rect,btn_rect.right,btn_top,btn_rect.right + btn_width + panel_margin_x * -3,
            btn_top + btn_height);
    new_ctrl = (Button *)(operator_new(0x3c));
    eh_state = 0x10;
    if (new_ctrl == (Button *)0x0) {
      sort_city_button = 0;
    }
    else {
      sort_city_button = (i32)(button_ctor(new_ctrl));
    }
    eh_state = 0xffffffff;
    self->sort_by_city_buttons[side] = sort_city_button;
    if (self == (ParleyWindow *)0x0) {
      sort_city_parent = (u8 *)0x0;
    }
    else {
      sort_city_parent = ((u8 *)self + 0x48);
    }
    label = (iptr)(label_string(label_string_ids[0x30b]));
    button_create
              ((Button *)self->sort_by_city_buttons[side],(iptr)sort_city_parent,
               side + 0x402,(iptr)&btn_rect,(const char *)label);
    button_set_callback
              ((Button *)self->sort_by_city_buttons[side],
               (CtrlNotifyFn)(side == 0 ? parley_sort_by_city_left_clicked : parley_sort_by_city_right_clicked));
    if (((int)parley_wnd_block.mode != 9) &&
       (((side == 0 && (self->local_units_revealed == '\0')) ||
        ((side == 1 && (self->remote_units_revealed == '\0')))))) {
      ctrl_disable((Button *)self->sort_by_city_buttons[side]);
    }
    ctrl_rect.top = btn_rect.bottom;
    ctrl_rect.bottom = self->divider_y + panel_margin_y * -2;
    ctrl_rect.left += panel_margin_x * 2;
    ctrl_rect.right += panel_margin_x * -2;
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    surface_fill_rect(self,(iptr)&ctrl_rect,list_bg_color);
    unit_list_mode = 1;
    if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
      unit_list_mode = 0;
    }
    parley_create_unit_list_pane(self,&ctrl_rect,side,unit_list_mode);
    break;
  case 0xb:
    gfx_set_text_color_index(10);
    if (self->large_layout == 0) {
      proposal_line_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      proposal_line_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = proposal_line_height * 3 + panel_margin_y * 2;
    tmp = self->panel_top + (int)((self->divider_y - self->panel_top) - block_height) / 2;
    cur_x = side_x;
    if (self->large_layout == 0) {
      proposal_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      proposal_title_font = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    text_x = side_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x30c]));
    surface_draw_text(self,(iptr)proposal_title_font,(const char *)label,text_x,tmp,text_align);
    proposal_labels[0] = (i32)(label_string(label_string_ids[0x30d]));
    proposal_labels[1] = (i32)(label_string(label_string_ids[0x30e]));
    if (self->large_layout == 0) {
      proposal_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      proposal_radio_font = ((i8 *)&font_size24_bold);
    }
    ctrl_set_pending_font((iptr)proposal_radio_font);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 0x11;
    if (new_input == (EditControl *)0x0) {
      proposal_radio = 0;
    }
    else {
      proposal_radio = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = proposal_radio;
    cur_x = side_x;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2;
    if (self->large_layout == 0) {
      proposal_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      proposal_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y = cur_y + panel_margin_y + proposal_title_height;
    if (self->large_layout == 0) {
      proposal_row_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      proposal_row_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = proposal_row_height * 2 + panel_margin_y;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    if (self == (ParleyWindow *)0x0) {
      proposal_parent = (u8 *)0x0;
    }
    else {
      proposal_parent = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)proposal_parent,0x40c,
               (iptr)&ctrl_rect,1,2,(iptr)proposal_labels);
    radiogrp_set_selected
              ((RadioGroup *)self->side_radios[side],self->proposal_kind);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    radiogrp_set_select_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_proposal_kind_select)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 0xc:
    civlist_titles[0] = (iptr)(label_string(label_string_ids[0x314]));
    civlist_titles[1] = (iptr)(label_string(label_string_ids[0x315]));
    civlist_titles[2] = (iptr)(label_string(label_string_ids[0x316]));
    civlist_titles[3] = (iptr)(label_string(label_string_ids[0x343]));
    if (is_exchange_side == '\0') {
      if (is_demand_side == '\0') {
        civlist_title_idx = 0;
      }
      else {
        civlist_title_idx = 3;
      }
    }
    else if (side == 0) {
      civlist_title_idx = 1;
    }
    else {
      civlist_title_idx = 2;
    }
    flag_ptr = (u8 *)&self->list_rows[side];
    flag_ptr[0] = 5;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    gfx_set_text_color_index(10);
    cur_x = side_x;
    cur_y = self->panel_top + panel_margin_y;
    if (is_exchange_side != '\0') {
      str_clear((char *)&map_views_block.text_buffer);
      if (side == 0) {
        civlist_side_title = (iptr)(label_string(label_string_ids[0x303]));
      }
      else {
        civlist_side_title = (iptr)(label_string(label_string_ids[0x304]));
      }
      text_append_str((const char *)civlist_side_title);
      if (self->large_layout == 0) {
        civlist_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        civlist_side_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)civlist_side_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,5);
      if (self->large_layout == 0) {
        civlist_side_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        civlist_side_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      cur_y = cur_y + panel_margin_y + civlist_side_height;
    }
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 + panel_margin_x * -5;
    if (self->large_layout == 0) {
      civlist_row_height = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      civlist_row_height = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    if (self->large_layout == 0) {
      civlist_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      civlist_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = self->list_rows[side] * civlist_row_height +
               self->list_rows[side] * panel_margin_y + panel_margin_y * 4 +
               civlist_title_height;
    cur_y += (int)((self->divider_y - cur_y) - block_height) / 2;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    gfx_set_draw_target((iptr)&parley_wnd_block);
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    gfx_target_frame_rect(&ctrl_rect,10);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    if (self->large_layout == 0) {
      civlist_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      civlist_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y = cur_y + panel_margin_y + civlist_header_height / 2;
    if (self->large_layout == 0) {
      civlist_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      civlist_header_font = ((i8 *)&font_size24_bold);
    }
    surface_draw_text
              (self,(iptr)civlist_header_font,(const char *)civlist_titles[civlist_title_idx],cur_x,cur_y,0);
    if (self->large_layout == 0) {
      civlist_box_offset = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      civlist_box_offset = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    ctrl_rect.top = ctrl_rect.top + panel_margin_y + civlist_box_offset;
    if (self->large_layout == 0) {
      civlist_item_height = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      civlist_item_height = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    ctrl_rect.bottom =
         self->list_rows[side] * civlist_item_height +
         self->list_rows[side] * panel_margin_y + ctrl_rect.top + 2;
    ctrl_rect.left += panel_margin_x * 2;
    ctrl_rect.right += panel_margin_x * -2;
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    surface_fill_rect(self,(iptr)&ctrl_rect,list_bg_color);
    parley_build_civ_list(self,&ctrl_rect,side);
    break;
  case 0xd:
    map_labels[0] = (iptr)(label_string(label_string_ids[0x30f]));
    map_labels[1] = (iptr)(label_string(label_string_ids[0x310]));
    map_title_idx = 0;
    map_labels[2] = (iptr)(label_string(label_string_ids[0x311]));
    map_labels[3] = (iptr)(label_string(label_string_ids[0x312]));
    map_label_last = (i32)(label_string(label_string_ids[0x313]));
    gfx_set_text_color_index(10);
    cur_x = side_x;
    cur_y = self->panel_top + panel_margin_y;
    if (is_exchange_side != '\0') {
      map_title_idx = (uptr)(side != 0);
      str_clear((char *)&map_views_block.text_buffer);
      if (side == 0) {
        map_side_title = (iptr)(label_string(label_string_ids[0x303]));
      }
      else {
        map_side_title = (iptr)(label_string(label_string_ids[0x304]));
      }
      text_append_str((const char *)map_side_title);
      if (self->large_layout == 0) {
        map_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        map_side_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)map_side_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,5);
      if (self->large_layout == 0) {
        map_side_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        map_side_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      cur_y = cur_y + panel_margin_y + map_side_height;
    }
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 + panel_margin_x * -5;
    if (self->large_layout == 0) {
      map_row_height = font_cached_height((GameFont *)((i8 *)&font_size14));
    }
    else {
      map_row_height = font_cached_height((GameFont *)((i8 *)&font_size21));
    }
    if (self->large_layout == 0) {
      map_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      map_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = map_row_height * 3 + panel_margin_y * 9 + map_title_height;
    cur_y += (int)((self->divider_y - cur_y) - block_height) / 2;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    gfx_set_draw_target((iptr)&parley_wnd_block);
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    gfx_target_frame_rect(&ctrl_rect,10);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    if (self->large_layout == 0) {
      map_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      map_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y = cur_y + panel_margin_y + map_header_height / 2;
    if (self->large_layout == 0) {
      map_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      map_header_font = ((i8 *)&font_size24_bold);
    }
    surface_draw_text
              (self,(iptr)map_header_font,(const char *)map_labels[map_title_idx],cur_x,cur_y,0);
    if (self->large_layout == 0) {
      map_box_offset = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      map_box_offset = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    ctrl_rect.top = ctrl_rect.top + panel_margin_y + map_box_offset;
    ctrl_rect.bottom += panel_margin_y * -2;
    ctrl_rect.left += panel_margin_x * 2;
    ctrl_rect.right += panel_margin_x * -2;
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    surface_fill_rect(self,(iptr)&ctrl_rect,list_bg_color);
    if (self->large_layout == 0) {
      map_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      map_radio_font = ((i8 *)&font_size24_bold);
    }
    ctrl_set_pending_font((iptr)map_radio_font);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 0x12;
    if (new_input == (EditControl *)0x0) {
      map_radio = 0;
    }
    else {
      map_radio = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = map_radio;
    if (self == (ParleyWindow *)0x0) {
      map_parent = (u8 *)0x0;
    }
    else {
      map_parent = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)map_parent,side + 0x424,
               (iptr)&ctrl_rect,1,3,(iptr)(map_labels + 2));
    radiogrp_set_selected
              ((RadioGroup *)self->side_radios[side],
               (i32)self->map_choice[side]);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    radiogrp_set_select_callback((RadioGroup *)self->side_radios[side],parley_on_map_choice_select)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 0xf:
    offer_labels[0] = (i32)(label_string(label_string_ids[0x2eb]));
    offer_labels[1] = (i32)(label_string(label_string_ids[0x2ec]));
    offer_labels[2] = (i32)(label_string(label_string_ids[0x2ed]));
    offer_labels[3] = (i32)(label_string(label_string_ids[750]));
    offer_labels[4] = (i32)(label_string(label_string_ids[0x2ef]));
    offer_labels[5] = (i32)(label_string(label_string_ids[0x2f1]));
    offer_labels[6] = (i32)(label_string(label_string_ids[0x342]));
    gfx_set_text_color_index(10);
    cur_x = side_x;
    if (self->large_layout == 0) {
      offer_line_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      offer_line_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = panel_margin_y * 7 + offer_line_height * 8;
    tmp = self->panel_top + (int)((self->divider_y - self->panel_top) - block_height) / 2;
    if (self->large_layout == 0) {
      offer_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      offer_title_font = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    draw_x = cur_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x317]));
    surface_draw_text(self,(iptr)offer_title_font,(const char *)label,draw_x,tmp,text_align);
    if (self->large_layout == 0) {
      offer_title_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      offer_title_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    tmp = cur_y + panel_margin_y * 2 + offer_title_height;
    if (self->large_layout == 0) {
      offer_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      offer_header_font = ((i8 *)&font_size24_bold);
    }
    text_align = 5;
    draw_x = cur_x;
    cur_y = tmp;
    label = (iptr)(label_string(label_string_ids[0x318]));
    surface_draw_text(self,(iptr)offer_header_font,(const char *)label,draw_x,tmp,text_align);
    if (self->large_layout == 0) {
      offer_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      offer_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y = cur_y + panel_margin_y + offer_header_height;
    if (self->large_layout == 0) {
      offer_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      offer_radio_font = ((i8 *)&font_size24_bold);
    }
    ctrl_set_pending_font((iptr)offer_radio_font);
    new_input = (EditControl *)(operator_new(0x50));
    eh_state = 0x13;
    if (new_input == (EditControl *)0x0) {
      offer_radio = 0;
    }
    else {
      offer_radio = (i32)(radiogrp_ctor((RadioGroup *)new_input));
    }
    eh_state = 0xffffffff;
    self->side_radios[side] = offer_radio;
    cur_x = side_x;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2;
    if (self->large_layout == 0) {
      offer_row_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      offer_row_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    block_height = panel_margin_y * 5 + offer_row_height * 6;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    if (self == (ParleyWindow *)0x0) {
      offer_parent = (u8 *)0x0;
    }
    else {
      offer_parent = ((u8 *)self + 0x48);
    }
    radiogrp_create
              ((RadioGroup *)self->side_radios[side],(iptr)offer_parent,0x41f,
               (iptr)&ctrl_rect,1,7,(iptr)offer_labels);
    radiogrp_set_selected
              ((RadioGroup *)self->side_radios[side],self->demand_kind);
    radiogrp_set_dblclick_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_radio_dblclick)
    ;
    radiogrp_set_select_callback((RadioGroup *)self->side_radios[side],(RadioNotifyFn)parley_on_demand_kind_select)
    ;
    flag_ptr = (u8 *)&self->side_selected[side];
    flag_ptr[0] = 1;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
    break;
  case 0x11:
    if (is_exchange_side == '\0') {
      flag_ptr = (u8 *)&self->list_rows[side];
      flag_ptr[0] = 7;
      flag_ptr[1] = '\0';
      flag_ptr[2] = '\0';
      flag_ptr[3] = '\0';
    }
    else {
      flag_ptr = (u8 *)&self->list_rows[side];
      flag_ptr[0] = 6;
      flag_ptr[1] = '\0';
      flag_ptr[2] = '\0';
      flag_ptr[3] = '\0';
    }
    gfx_set_text_color_index(10);
    cur_x = side_x;
    cur_y = self->panel_top + panel_margin_y;
    tmp = rect_width((RECT *)&self->panel_left);
    col_width = (tmp + panel_margin_x * -2) / 2 + panel_margin_x * -5;
    if (is_exchange_side != '\0') {
      str_clear((char *)&map_views_block.text_buffer);
      if (side == 0) {
        city_side_title = (iptr)(label_string(label_string_ids[0x303]));
      }
      else {
        city_side_title = (iptr)(label_string(label_string_ids[0x304]));
      }
      text_append_str((const char *)city_side_title);
      if (self->large_layout == 0) {
        city_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        city_side_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)city_side_font,(const char *)&map_views_block.text_buffer,cur_x,cur_y,5);
      if (self->large_layout == 0) {
        city_side_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        city_side_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      cur_y = cur_y + panel_margin_y + city_side_height;
    }
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,self->divider_y);
    gfx_set_draw_target((iptr)&parley_wnd_block);
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    gfx_target_frame_rect(&ctrl_rect,10);
    tmp = rect_width(&ctrl_rect);
    cur_x = ctrl_rect.left + tmp / 2;
    if (self->large_layout == 0) {
      city_header_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
    }
    else {
      city_header_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
    }
    cur_y += city_header_height / 2;
    if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
      city_header_label = (iptr)(label_string(label_string_ids[0x31a]));
      if (self->large_layout == 0) {
        city_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
      }
      else {
        city_header_font = ((i8 *)&font_size24_bold);
      }
      surface_draw_text
                (self,(iptr)city_header_font,(const char *)city_header_label,cur_x,cur_y,0);
    }
    else {
      city_btn_top = panel_margin_y * 2 + ctrl_rect.top - 1;
      city_btn_width = col_width + panel_margin_x * -3;
      if (self->large_layout == 0) {
        city_btn_font_height = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      }
      else {
        city_btn_font_height = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      }
      city_btn_height = city_btn_font_height - (panel_margin_y * 2 - 1);
      SetRect(&city_btn_rect,panel_margin_x * 2 + ctrl_rect.left,city_btn_top,ctrl_rect.left + city_btn_width,
              city_btn_height + city_btn_top);
      new_ctrl = (Button *)(operator_new(0x3c));
      eh_state = 0x14;
      if (new_ctrl == (Button *)0x0) {
        city_reveal_button = 0;
      }
      else {
        city_reveal_button = (i32)(button_ctor(new_ctrl));
      }
      eh_state = 0xffffffff;
      self->info_show_buttons[side] = city_reveal_button;
      if (side == 0) {
        city_reveal_label = (iptr)(label_string(label_string_ids[0x372]));
      }
      else {
        city_reveal_label = (iptr)(label_string(label_string_ids[0x373]));
      }
      if (self == (ParleyWindow *)0x0) {
        city_reveal_parent = (u8 *)0x0;
      }
      else {
        city_reveal_parent = ((u8 *)self + 0x48);
      }
      button_create
                ((Button *)self->info_show_buttons[side],(iptr)city_reveal_parent,
                 side + 0x408,(iptr)&city_btn_rect,(const char *)city_reveal_label);
      button_set_callback
                ((Button *)self->info_show_buttons[side],
                 (CtrlNotifyFn)(side == 0 ? parley_reveal_city_info : parley_request_city_info));
      new_ctrl = (Button *)(operator_new(0x3c));
      eh_state = 0x15;
      if (new_ctrl == (Button *)0x0) {
        city_hide_button = 0;
      }
      else {
        city_hide_button = (i32)(button_ctor(new_ctrl));
      }
      eh_state = 0xffffffff;
      self->info_hide_buttons[side] = city_hide_button;
      if (side == 0) {
        city_hide_label = (iptr)(label_string(label_string_ids[0x374]));
      }
      else {
        city_hide_label = (iptr)(label_string(label_string_ids[0x373]));
      }
      if (self == (ParleyWindow *)0x0) {
        city_hide_parent = (u8 *)0x0;
      }
      else {
        city_hide_parent = ((u8 *)self + 0x48);
      }
      button_create
                ((Button *)self->info_hide_buttons[side],(iptr)city_hide_parent,
                 side + 0x40a,(iptr)&city_btn_rect,(const char *)city_hide_label);
      button_set_callback
                ((Button *)self->info_hide_buttons[side],
                 (CtrlNotifyFn)(side == 0 ? parley_hide_city_info : parley_request_city_info));
      if ((side == 0) && (self->local_cities_revealed != '\0')) {
        control_hide((Control *)self->info_show_buttons[0]);
        ctrl_show_and_redraw((Control *)self->info_hide_buttons[0]);
      }
      else if (side == 0) {
        ctrl_show_and_redraw((Control *)self->info_show_buttons[0]);
        control_hide((Control *)self->info_hide_buttons[0]);
      }
      if ((side == 1) && (self->remote_cities_revealed != '\0')) {
        control_hide((Control *)self->info_show_buttons[1]);
        ctrl_show_and_redraw((Control *)self->info_hide_buttons[1]);
        ctrl_disable((Button *)self->info_hide_buttons[1]);
      }
      else if (side == 1) {
        ctrl_show_and_redraw((Control *)self->info_show_buttons[1]);
        control_hide((Control *)self->info_hide_buttons[1]);
      }
    }
    if (self->large_layout == 0) {
      tmp = font_cached_height((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold));
      ctrl_rect.top += tmp;
    }
    else {
      tmp = font_cached_height((GameFont *)((i8 *)&font_size24_bold));
      ctrl_rect.top += tmp;
    }
    ctrl_rect.bottom = self->divider_y + panel_margin_y * -2;
    ctrl_rect.left += panel_margin_x * 2;
    ctrl_rect.right += panel_margin_x * -2;
    gfx_target_frame_rect(&ctrl_rect,10);
    rect_deflate((iptr)&ctrl_rect,1,1);
    surface_fill_rect(self,(iptr)&ctrl_rect,list_bg_color);
    city_list_mode = 1;
    if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
      city_list_mode = 0;
    }
    parley_create_city_list_pane(self,&ctrl_rect,side,city_list_mode);
    break;
  case 0x12:
    label = civ_get_adjective(self->other_civ);
    text_set_string_arg_b(0,(const char *)label);
    text_clear();
    text_append_label(0x2e2);
    text_expand_vars((iptr)&map_views_block.text_buffer,(iptr)waiting_text);
    gfx_set_text_color_index(10);
    if (self->large_layout == 0) {
      waiting_text_width = font_text_width((GameFont *)((i8 *)&font_size20),(const char *)waiting_text);
    }
    else {
      waiting_text_width = font_text_width((GameFont *)((i8 *)&font_size30),(const char *)waiting_text);
    }
    cur_x = (self->panel_left + (int)(self->panel_right - self->panel_left) / 2) - waiting_text_width / 2;
    if (self->large_layout == 0) {
      waiting_font_height = font_cached_height((GameFont *)((i8 *)&font_size20));
    }
    else {
      waiting_font_height = font_cached_height((GameFont *)((i8 *)&font_size30));
    }
    cur_y = (self->panel_top + (int)(self->divider_y - self->panel_top) / 2 + panel_margin_y) -
               waiting_font_height / 2;
    if (self->large_layout == 0) {
      waiting_font = ((i8 *)&font_size20);
    }
    else {
      waiting_font = ((i8 *)&font_size30);
    }
    surface_draw_text
              (self,(iptr)waiting_font,(const char *)waiting_text,cur_x,cur_y,5);
    control_hide((Control *)self->mode2_button);
    ctrl_disable((Button *)self->mode2_button);
    ctrl_show_and_redraw((Control *)self->mode_button);
    ctrl_disable((Button *)self->mode_button);
    ctrl_disable((Button *)self->never_mind_button);
    if ((self->local_units_revealed == '\0') && (self->local_cities_revealed == '\0')) {
      control_hide((Control *)self->conceal_button);
      ctrl_disable((Button *)self->conceal_button);
      ctrl_show_and_redraw((Control *)self->reveal_button);
      ctrl_enable((Button *)self->reveal_button);
    }
    else {
      ctrl_show_and_redraw((Control *)self->conceal_button);
      ctrl_enable((Button *)self->conceal_button);
      control_hide((Control *)self->reveal_button);
      ctrl_disable((Button *)self->reveal_button);
    }
    break;
  case 0x13:
    parley_build_description(self,self->received_offer,0);
    new_input = (EditControl *)(operator_new(0x48));
    eh_state = 0;
    if (new_input == (EditControl *)0x0) {
      desc_edit = 0;
    }
    else {
      desc_edit = (uptr)(edit_ctor(new_input));
    }
    eh_state = 0xffffffff;
    self->desc_edit = desc_edit;
    cur_x = self->panel_left + panel_margin_x;
    col_width = rect_width((RECT *)&self->panel_left);
    col_width += panel_margin_x * -2;
    cur_y = self->panel_top + panel_margin_y;
    block_height = self->divider_y - cur_y;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    if (self->large_layout == 0) {
      desc_font = ((i8 *)&font_size14);
    }
    else {
      desc_font = ((i8 *)&font_size21);
    }
    ui_set_default_font((iptr)desc_font);
    if (self == (ParleyWindow *)0x0) {
      desc_parent = (u8 *)0x0;
    }
    else {
      desc_parent = ((u8 *)self + 0x48);
    }
    edit_create_styled
              ((EditControl *)self->desc_edit,(iptr)desc_parent,0x426,(iptr)&ctrl_rect,(const char *)&text_section_index_pool_block.parley_description,0x122
               ,0);
    edit_set_max_len((EditControl *)self->desc_edit,0x2000);
    edit_set_callback((EditControl *)self->desc_edit,parley_edit_escape_filter);
    ctrl_show_and_redraw((Control *)self->desc_edit);
    break;
  case 0x14:
    window_show((SurfaceWindow *)((i8 *)&parley_wnd_block));
    parley_build_description(self,self->received_offer,1);
    new_input = (EditControl *)(operator_new(0x48));
    eh_state = 1;
    if (new_input == (EditControl *)0x0) {
      review_edit = 0;
    }
    else {
      review_edit = (uptr)(edit_ctor(new_input));
    }
    eh_state = 0xffffffff;
    self->desc_edit = review_edit;
    cur_x = self->panel_left + panel_margin_x;
    col_width = rect_width((RECT *)&self->panel_left);
    col_width += panel_margin_x * -2;
    cur_y = self->panel_top + panel_margin_y;
    block_height = self->divider_y - cur_y;
    SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
    if (self->large_layout == 0) {
      review_font = ((i8 *)&font_size14);
    }
    else {
      review_font = ((i8 *)&font_size21);
    }
    ui_set_default_font((iptr)review_font);
    if (self == (ParleyWindow *)0x0) {
      review_parent = (u8 *)0x0;
    }
    else {
      review_parent = ((u8 *)self + 0x48);
    }
    edit_create_styled
              ((EditControl *)self->desc_edit,(iptr)review_parent,0x426,(iptr)&ctrl_rect,(const char *)&text_section_index_pool_block.parley_description,0x122
               ,0);
    edit_set_max_len((EditControl *)self->desc_edit,0x2000);
    edit_set_callback((EditControl *)self->desc_edit,parley_edit_escape_filter);
    ctrl_show_and_redraw((Control *)self->desc_edit);
    parley_wnd_block.offer_acceptable = 1;
    if (self->received_offer != 0) {
      item_buf = self->received_offer;
      item_kind_ptr = (int *)((uptr)item_buf + 0x20);
      if (((self->received_offer->mode == 0xf) && (*item_kind_ptr == 8)) ||
         ((self->received_offer->mode == 0xe && (*item_kind_ptr == 8)))) {
        tech_count = item_buf->item.value;
        tech_ptr = (iptr *)((uptr)item_buf + 0x2c);
        for (tech_idx = 0; tech_idx < tech_count; tech_idx++) {
          label = *tech_ptr;
          tech_ptr++;
          tmp = civ_has_advance(human_civ[0],label);
          if (tmp == 0) {
            self->offer_acceptable = 0;
          }
        }
      }
    }
    if (self->offer_acceptable == 0) {
      ctrl_disable((Button *)self->accept_button);
    }
    else {
      ctrl_enable((Button *)self->accept_button);
    }
  }
  parley_update_buttons(self);
  wnd_blit_surface_rect(self,(iptr)&self->fill_left);
  return;
}

/* 0052d4d2 auto */
void __cdecl parley_on_treaty_type_select(i32 ctrl_id,i32 treaty_type)

{
  iptr active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)parley + 0x48));
  parley->treaty_kind = treaty_type;
  return;
}

/* 0052d523 auto */
void __cdecl parley_on_demand_kind_select(i32 ctrl_id,u32 demand_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)this2 + 0x48));
  this2->demand_kind = demand_kind;
  parley_add_side_dialog(this2,this2->right_kind,1);
  return;
}

/* 0052d588 auto */
void __cdecl parley_on_map_choice_select(int side,i32 map_choice)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)parley + 0x48));
  *(i32 *)((iptr)parley - 0xe6c + side * 4) = map_choice;
  return;
}

/* 0052d5dd auto */
void __cdecl parley_on_gift_kind_select(i32 ctrl_id,u32 gift_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)this2 + 0x48));
  this2->gift_kind = gift_kind;
  switch(gift_kind) {
  case 0:
    this2->mode = 7;
    break;
  case 1:
    this2->mode = 8;
    break;
  case 2:
    this2->mode = 9;
    break;
  case 3:
    this2->mode = 10;
    break;
  case 4:
    this2->mode = 0xb;
  }
  this2->shown_right_kind = (((i32 *)&parley_mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  parley_add_side_dialog(this2,this2->right_kind,1);
  return;
}

/* 0052d6ff auto */
void __cdecl parley_on_proposal_kind_select(i32 ctrl_id,u32 proposal_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)this2 + 0x48));
  this2->proposal_kind = proposal_kind;
  if (proposal_kind == 0) {
    this2->mode = 0xc;
  }
  else if (proposal_kind == 1) {
    this2->mode = 0xd;
  }
  this2->shown_right_kind = (((i32 *)&parley_mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  parley_add_side_dialog(this2,this2->right_kind,1);
  return;
}

/* 0052d7dc auto */
void __cdecl parley_on_exchange_kind_select(int ctrl_id,i32 exchange_kind)

{
  iptr active_child;
  int selected;
  ParleyWindow *parley;
  uint is_right_side;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)parley + 0x48));
  is_right_side = (uint)(ctrl_id == 0x3f4);
  switch(exchange_kind) {
  case 0:
    selected = radiogrp_get_selected((RadioGroup *)parley->side_radios[is_right_side]);
    radiogrp_disable_item((RadioGroup *)parley->side_radios[is_right_side],0);
    radiogrp_enable_item((RadioGroup *)parley->side_radios[1],6);
    radiogrp_enable_item((RadioGroup *)parley->side_radios[0],6);
    if (selected == 0) {
      radiogrp_set_selected((RadioGroup *)parley->side_radios[is_right_side],1);
    }
    break;
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
    radiogrp_enable_item((RadioGroup *)parley->side_radios[0],0);
    radiogrp_enable_item((RadioGroup *)parley->side_radios[1],0);
    radiogrp_enable_item((RadioGroup *)parley->side_radios[1],6);
    radiogrp_enable_item((RadioGroup *)parley->side_radios[0],6);
    break;
  case 6:
    selected = radiogrp_get_selected((RadioGroup *)parley->side_radios[is_right_side]);
    radiogrp_enable_item((RadioGroup *)parley->side_radios[0],0);
    radiogrp_enable_item((RadioGroup *)parley->side_radios[1],0);
    radiogrp_disable_item((RadioGroup *)parley->side_radios[is_right_side],6);
    if (selected == 6) {
      radiogrp_set_selected((RadioGroup *)parley->side_radios[is_right_side],0);
    }
  }
  return;
}

/* 0052d9a1 auto */
void __cdecl parley_on_radio_dblclick(i32 ctrl_id)

{
  iptr active_child;
  int enabled;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  enabled = ctrl_is_enabled((Button *)parley->mode_button);
  if ((enabled != 0) ||
     (enabled = ctrl_is_enabled((Button *)parley->mode2_button), enabled != 0)) {
    wnd_activate((Wnd *)((u8 *)parley + 0x48));
    parley_on_mode_chosen(ctrl_id);
  }
  return;
}

/* 0052da23 auto */
void __cdecl parley_on_conceal_info(void)

{
  iptr active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->local_units_revealed = 0;
  parley->local_cities_revealed = 0;
  net_send_message(0xa6,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_cities_revealed,0,0,0,0,0,0);
  net_send_message(0xa8,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_units_revealed,0,0,0,0,0,0);
  control_hide((Control *)parley->conceal_button);
  ctrl_disable((Button *)parley->conceal_button);
  ctrl_show_and_redraw((Control *)parley->reveal_button);
  ctrl_enable((Button *)parley->reveal_button);
  return;
}

/* 0052db3d auto */
void __cdecl parley_on_reveal_info(void)

{
  iptr active_child;
  ParleyWindow *parley;
  
  wnd_get_active_child();
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->local_units_revealed = 1;
  parley->local_cities_revealed = 1;
  net_send_message(0xa5,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_cities_revealed,0,0,0,0,0,0);
  net_send_message(0xa7,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_units_revealed,0,0,0,0,0,0);
  ctrl_show_and_redraw((Control *)parley->conceal_button);
  ctrl_enable((Button *)parley->conceal_button);
  control_hide((Control *)parley->reveal_button);
  ctrl_disable((Button *)parley->reveal_button);
  return;
}

/* 0052dc7e auto */
void __cdecl parley_on_check_intel(void)

{
  bool has_intel_wonder;
  iptr active_child;
  int has_wonder;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  has_wonder = civ_has_wonder(human_civ[0],0x18);
  if (has_wonder == 0) {
    has_wonder = civ_has_wonder(human_civ[0],9);
    if (has_wonder == 0) {
      has_intel_wonder = false;
      goto check_intel_access;
    }
  }
  has_intel_wonder = true;
check_intel_access:
  if ((((*(byte *)&civs[human_civ[0]].treaties[(int)parley->other_civ] & 0x80) != 0)
      || (has_intel_wonder)) || (game.reveal_map != '\0')) {
    intel_report_show(human_civ[0],(iptr)parley->other_civ);
  }
  else {
    popup_game_with_sprite((const char *)&str_nointel_2,(iptr)&popup_info_icon_sprite,0);
  }
  return;
}

/* 0052dd73 auto */
void __cdecl parley_on_mode_chosen(i32 ctrl_id)

{
  iptr active_child;
  i32 value;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)this2 + 0x48));
  switch(this2->mode) {
  case 3:
    value = radiogrp_get_selected((RadioGroup *)this2->side_radios[1]);
    this2->selected_action = value;
    if (this2->selected_action == 0) {
      this2->mode = 6;
      ctrl_enable((Button *)this2->never_mind_button);
    }
    else if ((int)this2->selected_action == 1) {
      this2->mode = 0xc;
      if (this2->proposal_kind != 0) {
        this2->mode = 0xd;
      }
      ctrl_enable((Button *)this2->never_mind_button);
    }
    else if ((int)this2->selected_action == 2) {
      if (this2->gift_kind == 0) {
        this2->mode = 7;
      }
      else if ((int)this2->gift_kind == 1) {
        this2->mode = 8;
      }
      else if ((int)this2->gift_kind == 2) {
        this2->mode = 9;
      }
      else if ((int)this2->gift_kind == 3) {
        this2->mode = 10;
      }
      else {
        this2->mode = 0xb;
      }
      ctrl_enable((Button *)this2->never_mind_button);
    }
    else if ((int)this2->selected_action == 3) {
      this2->mode = 5;
      ctrl_enable((Button *)this2->never_mind_button);
    }
    else if ((int)this2->selected_action == 4) {
      this2->mode = 0xf;
      ctrl_enable((Button *)this2->never_mind_button);
    }
    this2->shown_left_kind = (((i32 *)&parley_mode_left_kinds))[this2->mode * 2];
    this2->left_kind = (i32)this2->shown_left_kind;
    this2->shown_right_kind = (((i32 *)&parley_mode_right_kinds))[this2->mode * 2];
    this2->right_kind = (i32)this2->shown_right_kind;
    parley_add_side_dialog(this2,(i32)this2->left_kind,0);
    parley_add_side_dialog
              (this2,(i32)this2->right_kind,1);
    break;
  case 5:
    value = radiogrp_get_selected((RadioGroup *)this2->side_radios[0]);
    this2->exchange_kind[0] = value;
    value = radiogrp_get_selected((RadioGroup *)this2->side_radios[1]);
    this2->exchange_kind[1] = value;
    this2->mode = 0xe;
    this2->shown_left_kind = (((i32 *)&parley_mode_left_kinds))[this2->mode * 2];
    this2->left_kind = (i32)this2->shown_left_kind;
    this2->shown_right_kind = (((i32 *)&parley_mode_right_kinds))[this2->mode * 2];
    this2->right_kind = (i32)this2->shown_right_kind;
    parley_add_side_dialog(this2,(i32)this2->left_kind,0);
    parley_add_side_dialog
              (this2,(i32)this2->right_kind,1);
    break;
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    if (this2->received_offer != 0) {
      operator_delete((void *)this2->received_offer);
      ((u8 *)this2 + 0x21c)[0x18] = '\0';
      ((u8 *)this2 + 0x21c)[0x19] = '\0';
      ((u8 *)this2 + 0x21c)[0x1a] = '\0';
      ((u8 *)this2 + 0x21c)[0x1b] = '\0';
    }
    value = (i32)(parley_send_offer(this2,this2->mode));
    this2->received_offer = (ParleyOfferMsg *)value;
    if ((this2->mode == 6) && (this2->treaty_kind == 3)) {
      if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & 8) == 0) {
        if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & 4) == 0) {
          if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & 2) != 0) {
            diplo_clear_treaty_flags(human_civ[0],this2->other_civ,2);
          }
        }
        else {
          diplo_clear_treaty_flags(human_civ[0],this2->other_civ,4);
        }
      }
      else {
        diplo_clear_treaty_flags(human_civ[0],this2->other_civ,8);
      }
      parley_add_side_dialog
                (this2,(i32)this2->left_kind,0);
      parley_add_side_dialog
                (this2,(i32)this2->right_kind,1);
    }
    else if (this2->received_offer != 0) {
      this2->mode = 1;
      this2->shown_left_kind = (((i32 *)&parley_mode_left_kinds))[this2->mode * 2];
      this2->left_kind = (i32)this2->shown_left_kind;
      this2->shown_right_kind = (((i32 *)&parley_mode_right_kinds))[this2->mode * 2];
      this2->right_kind = (i32)this2->shown_right_kind;
      parley_add_side_dialog
                (this2,(i32)this2->left_kind,0);
      parley_add_side_dialog
                (this2,(i32)this2->right_kind,1);
      ctrl_disable((Button *)this2->never_mind_button);
    }
  }
  return;
}

/* 0052e326 auto */
void __cdecl parley_on_never_mind(void)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)this2 + 0x48));
  switch(this2->mode) {
  case 3:
    break;
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xf:
    this2->mode = 3;
    ctrl_disable((Button *)this2->never_mind_button);
    this2->shown_left_kind = parley_mode3_left_kind;
    this2->left_kind = this2->shown_left_kind;
    this2->shown_right_kind = parley_mode3_right_kind;
    this2->right_kind = this2->shown_right_kind;
    parley_add_side_dialog(this2,this2->left_kind,0);
    parley_add_side_dialog(this2,this2->right_kind,1);
    break;
  case 0xe:
    this2->mode = 5;
    this2->shown_left_kind = parley_mode5_left_kind;
    this2->left_kind = this2->shown_left_kind;
    this2->shown_right_kind = parley_mode5_right_kind;
    this2->right_kind = this2->shown_right_kind;
    parley_add_side_dialog(this2,this2->left_kind,0);
    parley_add_side_dialog(this2,this2->right_kind,1);
  }
  return;
}

/* 0052e4c9 auto */
void __cdecl parley_on_accept_offer(void)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)parley + 0x48));
  net_send_message(0x83,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  dlg_pending_idle_callback = turn_popup_timeout_idle;
  popup_game_with_sprite((const char *)&str_parleyaccept2,(iptr)&popup_info_icon_sprite,0);
  parley->mode = 3;
  parley_set_mode_sides(parley,1);
  return;
}

/* 0052e57c auto */
void __cdecl parley_on_no_thanks(void)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)this2 + 0x48));
  net_send_message(0x84,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  this2->mode = 2;
  this2->shown_left_kind = (((i32 *)&parley_mode_left_kinds))[this2->mode * 2];
  this2->left_kind = this2->shown_left_kind;
  this2->shown_right_kind = (((i32 *)&parley_mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  parley_add_side_dialog(this2,this2->left_kind,0);
  parley_add_side_dialog(this2,this2->right_kind,1);
  return;
}

/* 0052e685 auto */
void __cdecl parley_on_counter_offer(void)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)parley + 0x48));
  net_send_message(0x85,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  parley->mode = 3;
  parley_set_mode_sides(parley,1);
  return;
}

/* 0052e71a auto */
void __cdecl chat_on_mute_checkbox(int ctrl_id)

{
  i32 checked;
  int chat_wnd2;
  
  chat_wnd2 = wnd_get_active_child();
  if (chat_wnd2 == 0) {
    chat_wnd2 = 0;
  }
  else {
    chat_wnd2 -= 0x48;
  }
  wnd_activate((Wnd *)(chat_wnd2 + 0x48));
  if ((0x428 < ctrl_id) && (ctrl_id < 0x430)) {
    checked = checkbox_is_checked
                      ((Checkbox *)chat_wnd_block.mute_checkboxes[(ctrl_id - 0x428)]);
    chat_wnd_block.muted_civs[(ctrl_id - 0x428)] = checked;
  }
  return;
}

/* 0052e7b7 auto */
void __cdecl chat_on_send_checkbox(int ctrl_id)

{
  i32 checked;
  int slot;
  int chat_wnd2;
  
  chat_wnd2 = wnd_get_active_child();
  if (chat_wnd2 == 0) {
    chat_wnd2 = 0;
  }
  else {
    chat_wnd2 -= 0x48;
  }
  wnd_activate((Wnd *)(chat_wnd2 + 0x48));
  switch(ctrl_id) {
  case 0x415:
    chat_wnd_block.remain_anonymous = checkbox_is_checked((Checkbox *)chat_wnd_block.anonymous_checkbox);
    break;
  case 0x416:
    chat_wnd_block.broadcast_all = checkbox_is_checked((Checkbox *)chat_wnd_block.broadcast_checkbox);
    if (chat_wnd_block.broadcast_all != 0) {
      for (slot = 1; slot < 8; slot++) {
        if (chat_wnd_block.send_checkboxes[slot] != 0) {
          checkbox_set_checked((Checkbox *)chat_wnd_block.send_checkboxes[slot],1);
          ctrl_show_and_redraw((Control *)chat_wnd_block.send_checkboxes[slot]);
          chat_wnd_block.send_to_civs[slot] = 1;
        }
      }
    }
    break;
  case 0x417:
  case 0x418:
  case 0x419:
  case 0x41a:
  case 0x41b:
  case 0x41c:
  case 0x41d:
    ctrl_id -= 0x416;
    checked = checkbox_is_checked((Checkbox *)chat_wnd_block.send_checkboxes[ctrl_id]);
    chat_wnd_block.send_to_civs[ctrl_id] = checked;
    if ((chat_wnd_block.send_to_civs[ctrl_id] == 0) && (chat_wnd_block.broadcast_all != 0)) {
      checkbox_set_checked((Checkbox *)chat_wnd_block.broadcast_checkbox,0);
      ctrl_show_and_redraw((Control *)chat_wnd_block.broadcast_checkbox);
      chat_wnd_block.broadcast_all = 0;
    }
    break;
  case 0x430:
    chat_wnd_block.mute_anonymous = checkbox_is_checked((Checkbox *)chat_wnd_block.mute_anonymous_checkbox);
  }
  return;
}

/* 0052e971 auto */
void __thiscall parley_update_buttons(ParleyWindow *self)

{
  if ((self->mode_button != 0) && (self->mode2_button != 0)) {
    if (self->mode != 2) {
      control_hide((Control *)self->conceal_button);
      ctrl_disable((Button *)self->conceal_button);
      control_hide((Control *)self->reveal_button);
      ctrl_disable((Button *)self->reveal_button);
    }
    switch(self->mode) {
    case 1:
    case 2:
      control_hide((Control *)self->mode2_button);
      ctrl_disable((Button *)self->mode2_button);
      ctrl_show_and_redraw((Control *)self->mode_button);
      ctrl_disable((Button *)self->mode_button);
      if (self->mode == 2) {
        if ((self->local_units_revealed == '\0') && (self->local_cities_revealed == '\0')) {
          control_hide((Control *)self->conceal_button);
          ctrl_disable((Button *)self->conceal_button);
          ctrl_show_and_redraw((Control *)self->reveal_button);
          ctrl_enable((Button *)self->reveal_button);
        }
        else {
          ctrl_show_and_redraw((Control *)self->conceal_button);
          ctrl_enable((Button *)self->conceal_button);
          control_hide((Control *)self->reveal_button);
          ctrl_disable((Button *)self->reveal_button);
        }
      }
      break;
    case 3:
    case 5:
      control_hide((Control *)self->mode2_button);
      ctrl_disable((Button *)self->mode2_button);
      ctrl_show_and_redraw((Control *)self->mode_button);
      ctrl_enable((Button *)self->mode_button);
      break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xf:
      ctrl_show_and_redraw((Control *)self->mode2_button);
      control_hide((Control *)self->mode_button);
      ctrl_disable((Button *)self->mode_button);
      if (self->side_selected[1] == 0) {
        ctrl_disable((Button *)self->mode2_button);
      }
      else {
        ctrl_enable((Button *)self->mode2_button);
      }
      break;
    case 0xe:
      ctrl_show_and_redraw((Control *)self->mode2_button);
      control_hide((Control *)self->mode_button);
      ctrl_disable((Button *)self->mode_button);
      if ((self->side_selected[0] == 0) || (self->side_selected[1] == 0)) {
        ctrl_disable((Button *)self->mode2_button);
      }
      else {
        ctrl_enable((Button *)self->mode2_button);
      }
    }
  }
  return;
}

/* 0052ec47 auto */
iptr __thiscall parley_find_unit_by_id(ParleyWindow *self,int unit_id)

{
  short x2;
  short y2;
  bool near_land;
  iptr x;
  int y;
  int in_bounds;
  iptr unit;
  iptr dir;
  
  unit = (iptr)game.num_units;
  do {
    unit--;
    if (unit < 0) {
      return 0xffffffff;
    }
  } while ((units[unit].id == 0) || (units[unit].id != unit_id));
  x2 = units[unit].x;
  y2 = units[unit].y;
  near_land = false;
  dir = 0;
  do {
    if (7 < dir) {
check_stack_result:
      if ((!near_land) && (y = unit_stack_sum(unit,2), y != 1)) {
        unit = -2;
      }
      return unit;
    }
    x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
    y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
    in_bounds = map_in_bounds(x,y);
    if ((in_bounds != 0) && (y = tile_is_ocean(x,y), y == 0)) {
      near_land = true;
      goto check_stack_result;
    }
    dir++;
  } while( true );
}

/* 0052ed95 auto */
iptr __thiscall parley_find_city_by_id(ParleyWindow *self,int city_id)

{
  iptr city;
  
  city = (iptr)game.num_cities;
  do {
    city--;
    if (city < 0) {
      return 0xffffffff;
    }
  } while ((cities[city].id == 0) || (cities[city].id != city_id));
  return city;
}

/* 0055ae80 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_stop(int notify)

{
  if (net_session_block.mp_turn_civ == human_civ[0]) {
    city_window_close();
    intel_report_end_modal();
    throneroom_close();
    pedia_end_modal();
  }
  if (mp_turn_timer_handle != 0) {
    game_timer_destroy(mp_turn_timer_handle);
    mp_turn_timer_handle = 0;
    status_draw_timer_bar(100,0xffffffff);
    if (((notify != 0) && (2 < game.multiplayer)) && (player_turn_active != '\0')) {
      net_send_message(0x6e,0xff,0,0,0,0,0,0,0,0);
    }
  }
  return;
}

/* 0055af2e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_start(int notify)

{
  RECT caption_rect;
  
  if (net_session_block.mp_turn_civ == human_civ[0]) {
    city_window_close();
    intel_report_end_modal();
    throneroom_close();
    pedia_end_modal();
  }
  mp_turn_seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
  setup_picwnd_block.g_timer_blink[0] = 0;
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  frame_wnd_draw_caption((FrameWindow *)((i8 *)&map_views_block.map_views));
  caption_rect.left = map_views_block.main_view_frame_left;
  caption_rect.top = map_views_block.main_view_frame_top;
  caption_rect.right = map_views_block.main_view_frame_right;
  caption_rect.bottom = map_views_block.main_view_client_top;
  wnd_blit_surface_rect((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&caption_rect);
  if (mp_turn_timer_handle != 0) {
    game_timer_destroy(mp_turn_timer_handle);
  }
  mp_turn_timer_handle = game_timer_create((TimerFn)mp_turn_timer_tick,500,1);
  g_timer_last_tick = timer_ticks();
  if ((notify != 0) && (2 < game.multiplayer)) {
    net_send_message(0x6d,0xff,0,0,0,0,0,0,0,0);
  }
  return;
}

/* 0055b046 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_resume(int notify)

{
  city_window_close();
  intel_report_end_modal();
  throneroom_close();
  pedia_end_modal();
  if ((mp_turn_seconds_left != 0) && (map_views_block.main_view_timer_shown[0] != 0)) {
    if (mp_turn_timer_handle != 0) {
      game_timer_destroy(mp_turn_timer_handle);
    }
    mp_turn_timer_handle = game_timer_create((TimerFn)mp_turn_timer_tick,500,1);
    g_timer_last_tick = timer_ticks();
    if ((notify != 0) && (2 < game.multiplayer)) {
      net_send_message(0x6f,0xff,0,0,0,0,0,0,0,0);
    }
  }
  return;
}

/* 0055b0fb: written by hand in src/game/timer_display.cpp */

/* 0055b2c6 auto */
void __cdecl mp_cmd_change_timer(void)

{
  int cancelled;
  RECT caption_rect;
  
  mp_turn_timer_stop(1);
  GetAsyncKeyState(0x1b);
  cancelled = setup_choose_game_timer(1);
  if (cancelled == 0) {
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms == 0) {
      map_views_block.main_view_timer_shown[0] = 0;
      frame_wnd_draw_caption((FrameWindow *)((i8 *)&map_views_block.map_views));
      caption_rect.left = map_views_block.main_view_frame_left;
      caption_rect.top = map_views_block.main_view_frame_top;
      caption_rect.right = map_views_block.main_view_frame_right;
      caption_rect.bottom = map_views_block.main_view_client_top;
      wnd_blit_surface_rect((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&caption_rect);
    }
    else {
      map_views_block.main_view_timer_shown[0] = 1;
      mp_turn_timer_start(1);
    }
    if (2 < game.multiplayer) {
      net_send_message(0x6c,0xff,civ_saved_cursor_x_block.mp_turn_time_limit_ms,0,0,0,0,0,0,0);
    }
  }
  else {
    mp_turn_timer_resume(1);
  }
  if (civ_saved_cursor_x_block.autoplay_active != 0) {
    player_turn_active = 0;
  }
  return;
}

/* 0055b3c8 auto */
void __cdecl mp_mouse_watchdog_stop(void)

{
  if (mp_mouse_watchdog_timer != 0) {
    game_timer_destroy(mp_mouse_watchdog_timer);
    mp_mouse_watchdog_timer = 0;
  }
  return;
}

/* 0055b3fd auto */
void __cdecl mp_mouse_watchdog_start(void)

{
  if (2 < game.multiplayer) {
    if (mp_mouse_watchdog_timer != 0) {
      game_timer_destroy(mp_mouse_watchdog_timer);
    }
    mp_mouse_watchdog_timer = game_timer_create(mp_mouse_watchdog_tick,500,1);
  }
  return;
}

/* 0055b451 auto */
i32 __cdecl mp_window_drag_mouse_hook(void)

{
  if (mp_window_drag_active != '\0') {
    mp_mouse_watchdog_tick(0,0);
  }
  return 0;
}

/* 0055b47e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_window_drag_begin(void)

{
  dbg_log((const char *)&str_enter_window_drag);
  mp_window_drag_active = 1;
  mp_window_drag_start_ticks = timer_ticks();
  mp_window_drag_flag_i32 = 0;
  mp_window_drag_wnd = wnd_get_active_child();
  if (net_session_block.net_host_flag != '\0') {
    net_send_message(0x5d,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  setup_picwnd_block.mp_window_drag_hook = SetWindowsHookExA(7,(HOOKPROC)((void *)mp_window_drag_mouse_hook),err_report_ctx_block.app_hinstance,0);
  return;
}

/* 0055b515 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_window_drag_end(void)

{
  dbg_log((const char *)&str_exit_window_drag);
  mp_window_drag_active = 0;
  mp_window_drag_start_ticks = 0;
  mp_window_drag_flag_i32 = 0;
  mp_window_drag_wnd = 0;
  UnhookWindowsHookEx(setup_picwnd_block.mp_window_drag_hook);
  if (net_session_block.net_host_flag != '\0') {
    net_send_message(0x5e,0xff,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
  }
  return;
}

/* 0055b59e auto */
void __cdecl mp_release_mouse_capture(void)

{
  NativeWindow *wnd;
  UINT Msg;
  WPARAM wParam;
  LPARAM lParam;
  
  if (mp_window_drag_active != '\0') {
    if ((*(WndBase * *)&mp_window_drag_wnd) != (WndBase *)0x0) {
      lParam = 0;
      wParam = 0;
      Msg = 0x202;
      wnd = wnd_get_window((*(WndBase * *)&mp_window_drag_wnd));
      PostMessageA(wnd->hwnd,Msg,wParam,lParam);
      wnd_release_capture((*(WndBase * *)&mp_window_drag_wnd));
    }
    mp_window_drag_active = '\0';
  }
  return;
}

/* 0055b5fa auto */
void __cdecl mp_mouse_watchdog_tick(i32 timer_id,i32 timer_arg)

{
  int now_ticks;
  
  if ((((2 < game.multiplayer) && (net_session_block.net_player_count[0] != 1)) && (mp_window_drag_active != '\0')) &&
     ((now_ticks = timer_ticks(), mp_window_drag_start_ticks + 0x26c < now_ticks && (net_session_block.player_has_quit == '\0')))) {
    mp_release_mouse_capture();
  }
  mp_mouse_watchdog_start();
  return;
}

/* 0056e270 auto */
LobbyList * __thiscall lobby_list_ctor(LobbyList *self)

{
  u8 *slot_ptr;
  int i;
  
  self->next_id = 1;
  self->entries = 0;
  for (i = 0; i < 7; i++) {
    slot_ptr = (u8 *)&self->drop_counts[i];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  return self;
}

/* 0056e2c9 auto */
void __thiscall lobby_list_dtor(void *self)

{
  net_free_game_list(&net_session_block);
  return;
}

/* 0056e2e9 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __thiscall lobby_host_wait_players(LobbyList *self,i32 map_arg,int launch_mode)

{
  uint *flags_ptr;
  Dialog *this_00;
  int result;
  int entry;
  int waiting_entry;
  i32 ret;
  uptr new_dialog;
  i32 seh_frame;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  if ((launch_mode == 2) || (launch_mode == 3)) {
    net_send_message(2,0,0,0,0,0,0,0,0,0);
    self->dialog = 0;
    net_session_block.net_server_address[0] = 0;
    crt_strcpy(((i8 *)&net_session_block.net_game_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_game_name));
    crt_strcpy(((i8 *)&net_session_block.net_local_player_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
    crt_strcpy(((i8 *)&net_session_block.net_host_player_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
  }
  else {
    this_00 = (Dialog *)(operator_new(0x2f4));
    seh_state = 0;
    if (this_00 == (Dialog *)0x0) {
      new_dialog = 0;
    }
    else {
      new_dialog = (uptr)(dialog_ctor(this_00,0x4000));
    }
    seh_state = 0xffffffff;
    self->dialog = (Dialog *)new_dialog;
    if (self->dialog == 0) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_pplayerpopup_null,(iptr)&str_d_ss_franklinton_startup_playerbox_cpp,0x34);
    }
    self->saved_button_label = dialog_button_labels[0];
    str_clear(self->title);
    str_append_label((iptr)self->title,0x28b);
    dialog_button_labels[0] = (i8 *)self->title;
    dlg_reset(self->dialog,0x81801);
    text_clear();
    text_append_label(0x288);
    if (launch_mode == 0) {
      crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&lobby_ip_sep_str));
      crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address));
    }
    dialog_set_title(self->dialog,(const char *)&map_views_block.text_buffer);
    text_clear();
    text_append_label(0x120);
    dialog_add_button(self->dialog,(const char *)&map_views_block.text_buffer);
    lobby_add_waiting_entry(self);
    dialog_set_listbox(self->dialog,7,0x1b7,0);
    dialog_set_default_choice(self->dialog,self->next_id - 1);
    dialog_render(self->dialog);
    wnd_set_modal_idle_callback((ModalMenuWindow *)((int)self->dialog->surf_wnd + 0x48),lobby_dialog_idle);
    if (net_session_block.net_lobby_launch_mode == 2) {
      dbg_log((const char *)&str_igz_sending_dplsys_connectionsettingsrea);
      result = XD_LobbySendMessage(1);
      if (result != 0) {
        dbg_logf((const char *)&str_igz_xd_lobbysendmessage_returned_error_d_2,result);
      }
      dbg_log((const char *)&str_igz_sending_dplsys_dplayconnectsucceeded);
      result = XD_LobbySendMessage(3);
      if (result != 0) {
        dbg_logf((const char *)&str_igz_xd_lobbysendmessage_returned_error_d_3,result);
      }
    }
    while( true ) {
      net_send_message(2,0,0,0,0,0,0,0,0,0);
      flags_ptr = (uint *)((iptr)self->dialog + 0x3c);
      *flags_ptr = *flags_ptr & 0xfffffbff;
      self->dialog->result_id = 0;
      self->dialog->result_extra = (i32)self->dialog->result_id;
      result = dlg_show_modal(self->dialog,0);
      if ((int)self->dialog->result_extra != 1) break;
      entry = lobby_find_entry_by_id(self,result);
      result = lobby_slot_for_entry_id(self,result);
      if ((entry != 0) && (waiting_entry = lobby_find_waiting_entry(self), waiting_entry == 0)) {
        net_send_message(5,net_session_block.net_slots[result].id,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(180000);
        XD_ServerCloseConnection((ushort)net_session_block.net_slots[result].id);
        ret = lobby_entry_neighbor(self,entry);
        self->dialog->selected_option = (DialogOption *)ret;
        lobby_remove_entry(self,(void *)entry);
        net_remove_player
                  (&net_session_block,net_session_block.net_slots[result].id);
        lobby_add_waiting_entry(self);
      }
    }
    if (result < 0) {
      net_send_message(0xb,0xff,0,0,0,0,0,0,0,0);
      dialog_button_labels[0] = self->saved_button_label;
      lobby_clear_player_list(self);
      if (self->dialog != (Dialog *)0x0) {
        dialog_scalar_dtor(self->dialog,1);
      }
      self->dialog = 0;
      XD_StopConnections();
      ret = 0;
      goto return_ret;
    }
    crt_strcpy(((i8 *)&net_session_block.net_server_address),((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address));
    crt_strcpy(((i8 *)&net_session_block.net_game_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_game_name));
    crt_strcpy(((i8 *)&net_session_block.net_local_player_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
    crt_strcpy(((i8 *)&net_session_block.net_host_player_name),((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
    dialog_close(self->dialog);
    dialog_button_labels[0] = self->saved_button_label;
    lobby_clear_player_list(self);
    if (self->dialog != (Dialog *)0x0) {
      dialog_scalar_dtor(self->dialog,1);
    }
    self->dialog = 0;
    wnd_flush_paint();
  }
  if (1 < net_session_block.net_player_count[0]) {
    net_send_message(4,0xff,0,0,0,0,0,0,0,0);
  }
  net_session_block.net_session_phase = 1;
  net_send_message(0x28,0xff,1,0,0,0,0,0,0,0);
  crt_strcpy(((i8 *)&net_session_block.net_slot_player_names),((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name));
  diffeng_register_blocks();
  net_reset_msg_counts();
  net_send_message(6,0xff,0,0,0,0,0,0,0,0);
  net_send_message(9,0xff,0,0,0,0,0,0,0,0);
  net_session_block.net_local_slot = 0;
  result = game_profile_popup(1);
  if (result == -1) {
    net_send_message(0x32,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    ret = 0;
  }
  else {
    if ((civ_saved_cursor_x_block.saved_resume_new_game == 0) && (civ_saved_cursor_x_block.saved_turn_civ == '\0')) {
      leaders_reset_custom_names();
      game.civs_alive = '\0';
      result = setup_choose_tribe(0,3,0);
      if (result != 0) {
        if (1 < net_session_block.net_player_count[0]) {
          net_send_message(0xb,0xff,0,0,0,0,0,0,0,0);
        }
        XD_StopConnections();
        ret = 0;
        goto return_ret;
      }
    }
    else {
      result = net_choose_civ_dialog(0);
      if (result != 0) {
        if (1 < net_session_block.net_player_count[0]) {
          net_send_message(0xb,0xff,0,0,0,0,0,0,0,0);
        }
        XD_StopConnections();
        ret = 0;
        goto return_ret;
      }
    }
    net_session_block.net_civ_slots[game.player_civ] = 0;
    net_session_block.net_slot_civs[0] = (int)game.player_civ;
    net_session_block.net_slot_active_flags[0] = 1;
    net_session_block.net_slot_host_flags[0] = 1;
    if (1 < net_session_block.net_player_count[0]) {
      crt_strcpy(((i8 *)&minimap_window_block.text_string_args),(const char *)((const void *)net_session_block.net_connection_title));
      dlg_pending_idle_callback = lobby_wait_civs_chosen_idle;
      net_session_block.net_wait_start_ticks_i32 = timer_ticks();
      result = popup_game_sprite_flags((const char *)&str_waitingonjoin,0x2000001,(iptr)&popup_info_icon_sprite,0);
      if (result == -1) {
        net_send_message(0xb,0xff,0,0,0,0,0,0,0,0);
        XD_StopConnections();
        ret = 0;
        goto return_ret;
      }
    }
    if (1 < net_session_block.net_player_count[0]) {
      net_send_message(4,0xff,0,0,0,0,0,0,0,0);
    }
    if ((turn_resume_civ == '\0') && (turn_resume_new_game == 0)) {
      ret = setup_choose_hotseat_players();
      setup_choose_opponents(ret);
      prefs_save_civ2dat();
      rules_load_advances(1);
      rules_load_units(1);
      rules_load_improvements(1);
      snd_menu_end();
      map_generate(map_arg);
      diffeng_register_blocks();
      diffeng_alloc_mirror();
      game_init_new();
      if (setup_accelerated_start != 0) {
        game_accelerated_start(setup_accelerated_start - 1);
      }
      if (turn_resume_civ != '\0') {
        game.player_civ = turn_resume_civ;
        human_civ[0] = (iptr)turn_resume_civ;
      }
    }
    else {
      diffeng_register_blocks();
      diffeng_alloc_mirror();
    }
    text_close_file();
    if (1 < net_session_block.net_player_count[0]) {
      net_send_message(10,0xff,0,0,0,0,0,0,0,0);
      net_send_state_diff(0xff,1,0,0,0);
      XD_FlushSendBuffer(5000);
      net_send_message(0x25,0xff,scenario_events_loaded,unit_types_block.scenario_event_count,0,0,0,0,0,0);
      net_send_message(0x16,0xff,0,0,0,0,0,0,0,0);
      XD_FlushSendBuffer(180000);
    }
    net_reset_msg_counts();
    net_session_block.net_session_phase = 3;
    net_send_message(0x28,0xff,3,0,0,0,0,0,0,0);
    ret = 1;
  }
return_ret:
  return ret;
}

/* 0056ec92 auto */
void __cdecl lobby_wait_civs_chosen_idle(void)

{
  int slot;
  int pending_count;
  
  net_process_messages(&net_session_block,1,0);
  pending_count = 0;
  for (slot = 1; slot < 7; slot++) {
    if (((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] == 1) && ((((i32 *)&net_session_block.net_slot_civs))[slot * 0x15] == -1)) {
      pending_count++;
    }
  }
  if (pending_count == 0) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    net_session_block.net_session_phase = 2;
    net_send_message(0x28,0xff,2,0,0,0,0,0,0,0);
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 0056ed62 auto */
void __cdecl lobby_dialog_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  return;
}

/* 0056edd3 auto */
iptr __thiscall lobby_entry_neighbor(LobbyList *self,iptr entry)

{
  iptr neighbor;
  
  neighbor = entry;
  if (*(int *)(entry + 0x10) != 0) {
    neighbor = *(iptr *)(entry + 0x10);
  }
  if ((neighbor == entry) && (*(int *)(entry + 0x14) != 0)) {
    neighbor = *(iptr *)(entry + 0x14);
  }
  return neighbor;
}

/* 0056ee2c auto */
void __thiscall lobby_clear_player_list(LobbyList *self)

{
  void *next;
  void *entry;
  
  entry = (void *)self->entries;
  while (entry != (void *)0x0) {
    next = *(void **)((int)entry + 0x10);
    operator_delete(*(void **)((int)entry + 8));
    operator_delete(entry);
    entry = next;
  }
  self->entries = 0;
  self->dialog->option_count = 0;
  self->dialog->option_list = 0;
  self->dialog->selected_option = 0;
  return;
}

/* 0056eed7 auto */
void __thiscall lobby_remove_entry(LobbyList *self,void *entry)

{
  int *count_ptr;
  
  if (entry != (void *)0x0) {
    if (*(int *)((int)entry + 0x14) == 0) {
      self->dialog->option_list = (DialogOption *)*(i32 *)((int)entry + 0x10);
      self->entries = (uptr)self->dialog->option_list;
    }
    else {
      *(i32 *)(*(int *)((int)entry + 0x14) + 0x10) = *(i32 *)((int)entry + 0x10);
    }
    if (*(int *)((int)entry + 0x10) != 0) {
      *(i32 *)(*(int *)((int)entry + 0x10) + 0x14) = *(i32 *)((int)entry + 0x14);
    }
    operator_delete(*(void **)((int)entry + 8));
    operator_delete(entry);
    count_ptr = (int *)((iptr)self->dialog + 0x28);
    *count_ptr = *count_ptr - 1;
  }
  return;
}

/* 0056ef93 auto */
uint * __thiscall lobby_add_player_entry(LobbyList *self,const char *name2,uint entry_id)

{
  int *count_ptr;
  uint *entry;
  size_t name_len;
  void *name_copy;
  int width;
  int max_width;
  uptr tail2;
  uptr node2;
  
  tail2 = 0;
  for (node2 = self->entries; node2 != 0; node2 = *(uptr *)(node2 + 0x10)) {
    tail2 = node2;
  }
  entry = (uint *)(operator_new(0x18));
  entry[4] = 0;
  entry[5] = tail2;
  if (tail2 == 0) {
    self->entries = (uptr)entry;
    *(uint **)((iptr)self->dialog + 0x228) = entry;
  }
  else {
    *(uint **)(tail2 + 0x10) = entry;
  }
  *entry = 0;
  name_len = _strlen((char *)name2);
  name_copy = operator_new(name_len + 1);
  entry[2] = (uint)name_copy;
  crt_strcpy((char *)((void *)entry[2]),(const char *)((const void *)name2));
  if (*(char *)name2 == '\0') {
    *entry = *entry | 1;
  }
  entry[1] = entry_id;
  entry[3] = 0;
  width = font_text_width((GameFont *)self->dialog->body_font,(const char *)entry[2]);
  max_width = dialog_option_icon_width(self->dialog);
  width = width + max_width + (int)self->dialog->option_extra_width + 5;
  max_width = (int)self->dialog->option_width;
  if (max_width <= width) {
    max_width = width;
  }
  self->dialog->option_width = max_width;
  count_ptr = (int *)((iptr)self->dialog + 0x28);
  *count_ptr = *count_ptr + 1;
  self->dialog->option_list = (DialogOption *)self->entries;
  self->dialog->list_top_option[0] = self->dialog->option_list;
  return entry;
}

/* 0056f113 auto */
void __cdecl lobby_refresh_player_list(void)

{
  int selected_id;
  int slot;
  uptr entry;
  
  if (net_session_block.net_lobby_list != (LobbyList *)0x0) {
    selected_id = *(int *)((int)net_session_block.net_lobby_list->dialog->selected_option + 4);
    lobby_clear_player_list(net_session_block.net_lobby_list);
    if (net_session_block.net_player_count[0] == 0) {
      lobby_add_waiting_entry(net_session_block.net_lobby_list);
    }
    else {
      for (slot = 1; slot < 7; slot++) {
        if ((((i8 *)&net_session_block.net_slot_active_flags))[slot * 0x54] != '\0') {
          if (net_session_block.net_slots[slot].lobby_id == 0) {
            net_session_block.net_slots[slot].lobby_id = net_session_block.net_lobby_list->next_id;
            net_session_block.net_lobby_list->next_id++;
          }
          lobby_add_player_entry
                    (net_session_block.net_lobby_list,(const char *)(((i8 *)&net_session_block.net_slot_player_names) + slot * 0x54),
                     net_session_block.net_slots[slot].lobby_id);
        }
      }
      lobby_add_waiting_entry(net_session_block.net_lobby_list);
    }
    for (entry = net_session_block.net_lobby_list->entries; (entry != 0 && (*(int *)(entry + 4) != selected_id));
        entry = *(uptr *)(entry + 0x10)) {
    }
    if (entry == 0) {
      for (entry = net_session_block.net_lobby_list->entries;
          (*(int *)(entry + 0x10) != 0 && (*(int *)(entry + 4) < selected_id));
          entry = *(uptr *)(entry + 0x10)) {
      }
      if (entry == 0) {
        entry = net_session_block.net_lobby_list->entries;
      }
    }
    net_session_block.net_lobby_list->dialog->option_list = (DialogOption *)net_session_block.net_lobby_list->entries;
    net_session_block.net_lobby_list->dialog->list_top_option[0] = net_session_block.net_lobby_list->dialog->option_list;
    dialog_set_default_choice
              (net_session_block.net_lobby_list->dialog,*(iptr *)(entry + 4));
    dialog_render(net_session_block.net_lobby_list->dialog);
  }
  return;
}

/* 0056f301 auto */
void __thiscall lobby_add_waiting_entry(LobbyList *self)

{
  i32 entry_id;
  
  if (self->entries == 0) {
    text_clear();
    text_append_label(0x28c);
    entry_id = (*(i32 * *)&net_session_block.net_lobby_list)[1];
    (*(i32 * *)&net_session_block.net_lobby_list)[1]++;
    lobby_add_player_entry(self,(const char *)&map_views_block.text_buffer,entry_id);
    dialog_set_default_choice
              ((Dialog *)*(*(i32 * *)&net_session_block.net_lobby_list),*(iptr *)((*(i32 * *)&net_session_block.net_lobby_list)[10] + 4));
  }
  return;
}

/* 0056f372 auto */
i32 __thiscall lobby_find_waiting_entry(LobbyList *self)

{
  int cmp;
  uptr entry;
  
  text_clear();
  text_append_label(0x28c);
  entry = self->entries;
  if (((entry != 0) && (*(int *)(entry + 0x10) == 0)) &&
     (cmp = _strcmp(((i8 *)&map_views_block.text_buffer),*(char **)(entry + 8)), cmp == 0)) {
    return entry;
  }
  return 0;
}

/* 0056f3e0 auto */
uptr __thiscall lobby_find_entry_by_id(LobbyList *self,int entry_id)

{
  uptr entry;
  
  for (entry = self->entries; (entry != 0 && (*(int *)(entry + 4) != entry_id));
      entry = *(uptr *)(entry + 0x10)) {
  }
  return entry;
}

/* 0056f42b auto */
int __thiscall lobby_slot_for_entry_id(LobbyList *self,int entry_id)

{
  int slot;
  
  for (slot = 0; (slot < 7 && (net_session_block.net_slots[slot].lobby_id != entry_id));
      slot = slot + 1) {
  }
  return slot;
}

/* 00587a90 auto */
void __thiscall
parley_create_city_list_pane(ParleyWindow *self,RECT *rect,int pane,i32 own_cities)

{
  u8 *field_ptr;
  int right;
  int scroll_width;
  Button *new_ctrl;
  u8 *hotspot_parent;
  u8 *scroll_parent;
  i32 hotspot;
  i32 scrollbar;
  RECT rect2;
  i32 seh_frame;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  rect2.left = rect->left;
  rect2.top = rect->top;
  rect2.right = rect->right;
  rect2.bottom = rect->bottom;
  sprite_picker_sprite_block.parley_pane_own_cities[pane] = own_cities;
  parley_build_city_list(self,pane,0);
  if (self->list_counts[pane] < 1) {
    field_ptr = (u8 *)&self->list_first_row[pane];
    field_ptr[0] = 0xff;
    field_ptr[1] = 0xff;
    field_ptr[2] = 0xff;
    field_ptr[3] = 0xff;
  }
  else {
    field_ptr = (u8 *)&self->list_first_row[pane];
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
  }
  field_ptr = (u8 *)&self->list_columns[pane];
  field_ptr[0] = 4;
  field_ptr[1] = '\0';
  field_ptr[2] = '\0';
  field_ptr[3] = '\0';
  self->list_rects[pane * 4] = rect->left;
  self->list_rects[pane * 4 + 1] = rect->top;
  self->list_rects[pane * 4 + 2] = rect->right;
  self->list_rects[pane * 4 + 3] = rect->bottom;
  if (self->list_rows[pane] <
      self->list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    right = rect2.right;
    self->list_rects[pane * 4 + 2] =
         (int)self->list_rects[pane * 4 + 2] - scroll_width;
    rect2.left = GetSystemMetrics(2);
    rect2.left = right - rect2.left;
    new_ctrl = (Button *)(operator_new(0x40));
    seh_state = 0;
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(scrollbar_ctor((Scrollbar *)new_ctrl));
    }
    seh_state = 0xffffffff;
    self->list_scrollbars[pane] = scrollbar;
    if (self == (ParleyWindow *)0x0) {
      scroll_parent = (u8 *)0x0;
    }
    else {
      scroll_parent = ((u8 *)self + 0x48);
    }
    scrollbar_create
              ((Scrollbar *)self->list_scrollbars[pane],(iptr)scroll_parent,
               pane + 0x420,(iptr)&rect2,1);
    scrollbar_set_range
              ((Scrollbar *)self->list_scrollbars[pane],0,
               self->list_counts[pane] -
               self->list_rows[pane]);
    scrollbar_set_pos((Scrollbar *)self->list_scrollbars[pane],0);
    scrollbar_set_page_size
              ((Scrollbar *)self->list_scrollbars[pane],
               self->list_rows[pane]);
    scrollbar_set_callback
              ((Scrollbar *)self->list_scrollbars[pane],
               (pane == 0 ? parley_city_list_scroll_local : parley_city_list_scroll_remote));
    scrollbar_set_track_callback
              ((Scrollbar *)self->list_scrollbars[pane],
               (pane == 0 ? parley_city_list_scroll_local : parley_city_list_scroll_remote));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  seh_state = 1;
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  seh_state = 0xffffffff;
  self->list_hotspots[pane] = hotspot;
  if (self == (ParleyWindow *)0x0) {
    hotspot_parent = (u8 *)0x0;
  }
  else {
    hotspot_parent = ((u8 *)self + 0x48);
  }
  hotspot_create
            ((Hotspot *)self->list_hotspots[pane],(iptr)hotspot_parent,
             pane + 0x422,(iptr)(((u8 *)self + 0x39c) + pane * 0x10 + 0x24));
  hotspot_set_click_callback((Hotspot *)self->list_hotspots[pane],parley_city_list_click);
  hotspot_set_dblclick_callback((Hotspot *)self->list_hotspots[pane],parley_city_list_click);
  parley_draw_city_list(pane);
  return;
}

/* 00587e05 auto */
void __cdecl parley_city_list_scroll_local(i32 pos)

{
  parley_city_list_scroll(0,pos);
  return;
}

/* 00587e23 auto */
void __cdecl parley_city_list_scroll_remote(i32 pos)

{
  parley_city_list_scroll(1,pos);
  return;
}

/* 00587e41 auto */
void __cdecl parley_reveal_city_info(void)

{
  iptr child;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)parley_wnd2 + 0x48));
  ctrl_show_and_redraw((Control *)parley_wnd2->info_hide_buttons[0]);
  control_hide((Control *)parley_wnd2->info_show_buttons[0]);
  parley_wnd2->local_cities_revealed = 1;
  net_send_message(0xa5,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd2->other_civ]].id,
                     human_civ[0],(int)(char)parley_wnd2->local_cities_revealed,0,0,0,0,0,0);
  parley_draw_city_list(0);
  return;
}

/* 00587f00 auto */
void __cdecl parley_hide_city_info(void)

{
  iptr child;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)parley_wnd2 + 0x48));
  ctrl_show_and_redraw((Control *)parley_wnd2->info_show_buttons[0]);
  control_hide((Control *)parley_wnd2->info_hide_buttons[0]);
  parley_wnd2->local_cities_revealed = 0;
  net_send_message(0xa6,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd2->other_civ]].id,
                     human_civ[0],(int)(char)parley_wnd2->local_cities_revealed,0,0,0,0,0,0);
  parley_draw_city_list(0);
  return;
}

/* 00587fbf auto */
void __cdecl parley_request_city_info(void)

{
  iptr child;
  ParleyWindow *window;
  const char *civ_plural;
  
  child = wnd_get_active_child();
  if (child == 0) {
    window = (ParleyWindow *)0x0;
  }
  else {
    window = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((u8 *)window + 0x48));
  civ_plural = (civ_name_plural(human_civ[0]));
  text_set_string_arg(0,civ_plural);
  net_send_text_args(0x65,net_session_block.net_slots[net_session_block.net_civ_slots[(int)parley_wnd_block.other_civ]].id,1,0,
                    human_civ[0],0);
  return;
}

/* 0058804f auto */
void __cdecl parley_city_list_scroll(int pane,i32 pos)

{
  int child;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  wnd_activate((Wnd *)((iptr)parley_wnd2 + 0x48));
  parley_wnd2->list_first_row[pane] = pos;
  parley_draw_city_list(pane);
  return;
}

/* 005880b0 auto */
void __cdecl parley_city_list_click(int control_id)

{
  uint *sel_flag;
  int hit_row;
  int key_down;
  int sel_end;
  int row;
  i32 cursor_y;
  i32 cursor_x;
  int pane;
  ParleyWindow *parley_wnd2;
  
  hit_row = wnd_get_active_child();
  if (hit_row == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(hit_row - 0x48);
  }
  wnd_activate((Wnd *)((iptr)parley_wnd2 + 0x48));
  pane = control_id - 0x422;
  wnd_get_cursor_pos
            ((WndBase *)((iptr)parley_wnd2 + 0x48),(iptr)&cursor_x,(iptr)&cursor_y);
  if (((parley_wnd2->list_counts[pane] != 0) &&
      (hit_row = parley_city_list_hit_test(cursor_x,cursor_y,pane), 0 < hit_row)) &&
     (hit_row < parley_wnd2->list_counts[pane])) {
    key_down = wnd_is_shift_down((void *)((iptr)parley_wnd2 + 0x48));
    if (key_down == 0) {
      key_down = wnd_is_ctrl_down((void *)((iptr)parley_wnd2 + 0x48));
      if (key_down == 0) {
        for (row = 0; row < parley_wnd2->list_counts[pane];
            row = row + 1) {
          if (row != hit_row) {
            parley_wnd2->list_selected[pane * 0x801 + row] = 0;
          }
        }
        parley_wnd2->list_anchor_row[pane] = hit_row;
        sel_flag = (uint *)(pane * 0x2004 + hit_row * 4 + 0x8400 + (iptr)parley_wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        parley_wnd2->list_anchor_row[pane] = hit_row;
        sel_flag = (uint *)(pane * 0x2004 + hit_row * 4 + 0x8400 + (iptr)parley_wnd2);
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (row = 0; row < parley_wnd2->list_counts[pane]; row++)
      {
        parley_wnd2->list_selected[pane * 0x801 + row] = 0;
      }
      if (parley_wnd2->list_anchor_row[pane] < hit_row) {
        row = parley_wnd2->list_anchor_row[pane];
        sel_end = hit_row;
      }
      else {
        sel_end = parley_wnd2->list_anchor_row[pane];
        row = hit_row;
      }
      for (; row <= sel_end; row++) {
        if (0 < row) {
          parley_wnd2->list_selected[pane * 0x801 + row] = 1;
        }
      }
    }
    parley_draw_city_list(pane);
  }
  return;
}

/* 0058832d auto */
int __cdecl parley_city_list_hit_test(int x,int y,int pane)

{
  iptr child;
  int result;
  int height;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (y < (int)parley_wnd2->list_rects[pane * 4 + 1]) {
    result = -1;
  }
  else if (y < (int)parley_wnd2->list_rects[pane * 4 + 3]) {
    if (x < (int)parley_wnd2->list_rects[pane * 4]) {
      result = -3;
    }
    else if (x < (int)parley_wnd2->list_rects[pane * 4 + 2]) {
      result = (int)parley_wnd2->list_rects[pane * 4 + 1];
      height = rect_height((RECT *)(pane * 0x10 + (iptr)parley_wnd2 + 0x3c0));
      result = (y - result) / (height / parley_wnd2->list_rows[pane]) +
              parley_wnd2->list_first_row[pane];
    }
    else {
      result = -4;
    }
  }
  else {
    result = -2;
  }
  return result;
}

/* 0058843f auto */
void __thiscall parley_sort_city_list(ParleyWindow *self,int first,int last,int pane)

{
  i32 tmp;
  iptr child;
  int cmp;
  int j;
  int i;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (1 < parley_wnd2->list_counts[pane]) {
    for (i = first; j = i, i < last; i++) {
      while (j = j + 1, j <= last) {
        cmp = city_has_improvement((iptr)parley_wnd2->list_items[j + pane * 0x801],1);
        if ((cmp != 0) ||
           ((cmp = city_has_improvement((iptr)parley_wnd2->list_items[i + pane * 0x801]
                                        ,1), cmp == 0 &&
            (cmp = _strcmp(cities[(int)parley_wnd2->list_items[i + pane * 0x801]].
                             name,cities[(int)parley_wnd2->list_items[j + pane * 0x801]
                                        ].name), 0 < cmp)))) {
          tmp = (i32)parley_wnd2->list_items[i + pane * 0x801];
          parley_wnd2->list_items[i + pane * 0x801] =
               (i32)parley_wnd2->list_items[j + pane * 0x801];
          parley_wnd2->list_items[j + pane * 0x801] = tmp;
          tmp = (i32)parley_wnd2->list_selected[i + pane * 0x801];
          parley_wnd2->list_selected[i + pane * 0x801] =
               (i32)parley_wnd2->list_selected[j + pane * 0x801];
          parley_wnd2->list_selected[j + pane * 0x801] = tmp;
          tmp = (i32)parley_wnd2->list_selected_b[i + pane * 0x801];
          parley_wnd2->list_selected_b[i + pane * 0x801] =
               (i32)parley_wnd2->list_selected_b[j + pane * 0x801];
          parley_wnd2->list_selected_b[j + pane * 0x801] = tmp;
          tmp = (i32)parley_wnd2->list_items_b[i + pane * 0x801];
          parley_wnd2->list_items_b[i + pane * 0x801] =
               (i32)parley_wnd2->list_items_b[j + pane * 0x801];
          parley_wnd2->list_items_b[j + pane * 0x801] = tmp;
        }
      }
    }
  }
  return;
}

/* 0058878e auto */
void __cdecl parley_draw_city_list(int pane)

{
  u8 *field_ptr;
  int xLeft;
  iptr child;
  int width;
  int text_offset_y;
  int row_y;
  int text_offset_x;
  i32 font_height;
  int selected;
  const char *label;
  i32 bg_color2;
  int city;
  int row;
  uint shadow_color2;
  uint text_color2;
  ParleyWindow *this2;
  tagRECT row_rect;
  int row_height;
  int sprite_style;
  i8 saved_clip[16];
  GameFont *font2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(child - 0x48);
  }
  surface_get_clip(this2,(RECT *)saved_clip);
  surface_set_clip(this2,(RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
fill_list_bg:
  surface_fill_rect
            (this2,(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10),list_bg_color);
  if (this2->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  xLeft = (int)this2->list_rects[pane * 4];
  width = rect_width((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
  row_height = rect_height((RECT *)(iptr)(((u8 *)this2 + 0x3c0) + pane * 0x10));
  row_height = row_height / this2->list_rows[pane];
  text_offset_y = row_height / 2;
  row_y = font_cached_height(font2);
  text_offset_y -= row_y / 2;
  field_ptr = (u8 *)&this2->side_selected[pane];
  field_ptr[0] = '\0';
  field_ptr[1] = '\0';
  field_ptr[2] = '\0';
  field_ptr[3] = '\0';
  for (row = 0; row < this2->list_counts[pane];
      row = row + 1) {
    if (this2->list_selected[pane * 0x801 + row] != 0) {
      field_ptr = (u8 *)&this2->side_selected[pane];
      field_ptr[0] = 1;
      field_ptr[1] = '\0';
      field_ptr[2] = '\0';
      field_ptr[3] = '\0';
      break;
    }
  }
  row = 0;
  do {
    if (this2->list_rows[pane] <= row) {
restore_clip_and_finish:
      surface_set_clip(this2,(RECT *)saved_clip);
      parley_update_buttons(this2);
      ctrl_show_and_redraw((Control *)this2->list_hotspots[pane])
      ;
      return;
    }
    row_y = (int)this2->list_rects[pane * 4 + 1] + row * row_height;
    SetRect(&row_rect,xLeft,row_y,width + xLeft,row_y + row_height);
    if ((row != 0) && (this2->list_first_row[pane] == -1))
    goto restore_clip_and_finish;
    if (this2->list_first_row[pane] + row <
        this2->list_counts[pane]) {
      selected = (int)this2->list_selected[pane * 0x801 + (this2->list_first_row[pane] + row)];
      if (selected == 0) {
        if (this2->list_first_row[pane] + row == 0) {
          text_color2 = (uint)dlg_highlight_text_color;
          shadow_color2 = (*(uint *)&dlg_sel_text_shadow_color);
        }
        else {
          text_color2 = (*(uint *)&list_text_color);
          shadow_color2 = (*(uint *)&list_shadow_color);
        }
      }
      else {
        text_color2 = (*(uint *)&dlg_sel_text_color);
        shadow_color2 = (*(uint *)&dlg_sel_text_shadow_color);
      }
      city = 0;
      if ((0 < this2->list_counts[pane]) &&
         (city = parley_find_city_by_id
                               (this2,
                                (i32)this2->list_items_b[pane * 0x801 + (this2->list_first_row[pane] + row)]), city == -1)) break;
      text_offset_x = (-(uint)(this2->large_layout == 0) & 0xffffffe2) + 0x5a + panel_margin_x;
      row_rect.left = row_rect.left + text_offset_x - 3;
      if (selected == 0) {
        bg_color2 = list_bg_color;
      }
      else {
        bg_color2 = list_sel_bg_color;
      }
      surface_fill_rect(this2,(iptr)&row_rect,bg_color2);
      if (pane == 0) {
        if (this2->local_cities_revealed == '\0') {
          sprite_style = 3;
        }
        else {
          sprite_style = 0;
        }
      }
      else if (this2->remote_cities_revealed == '\0') {
        sprite_style = 3;
      }
      else {
        sprite_style = 0;
      }
      if ((int)parley_wnd_block.mode == 10) {
        sprite_style = 0;
      }
      if (this2->list_first_row[pane] == -1) {
        text_clear();
        label = (label_string(label_string_ids[0x2de]));
        text_append(label);
        field_ptr = (u8 *)&this2->side_selected[pane];
        field_ptr[0] = '\0';
        field_ptr[1] = '\0';
        field_ptr[2] = '\0';
        field_ptr[3] = '\0';
      }
      else {
        selected = sprite_style;
        font_height = font_cached_height(font2);
        parley_draw_city_sprite(city,this2->list_first_row[pane] + row,
                           xLeft + 2,row_y,font_height,selected);
        text_clear();
        text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
        text_append_space();
        selected = city_has_improvement(city,1);
        if ((selected == 0) && (sprite_style == 0)) {
          text_append_lparen();
          text_append_int(*(iptr *)(pane * 0x2004 +
                                       (this2->list_first_row[pane] +
                                       row) * 4 + 0xc408 + (int)this2));
          text_append_space();
          if ((int)this2->list_selected_b[pane * 0x801 + (this2->list_first_row[pane] + row)] < 2) {
            label = (label_string(label_string_ids[0x2dc]));
            text_append(label);
          }
          else {
            label = (label_string(label_string_ids[0x2db]));
            text_append(label);
          }
        }
        else {
          selected = city_has_improvement(city,1);
          if (selected != 0) {
            label = (label_string(label_string_ids[0x2dd]));
            text_append(label);
          }
        }
      }
      if (shadow_color2 != text_color2) {
        gfx_set_text_color_index(shadow_color2);
        surface_draw_text
                  (this2,(iptr)font2,(const char *)&map_views_block.text_buffer,xLeft + text_offset_x + 2,
                   row_y + text_offset_y + 1,5);
        gfx_set_text_color_index(text_color2);
        surface_draw_text
                  (this2,(iptr)font2,(const char *)&map_views_block.text_buffer,xLeft + text_offset_x + 1,row_y + text_offset_y,
                   5);
      }
      gfx_set_text_color_index(text_color2);
      surface_draw_text
                (this2,(iptr)font2,(const char *)&map_views_block.text_buffer,xLeft + text_offset_x,row_y + text_offset_y,5);
    }
    row++;
  } while( true );
  parley_build_city_list(this2,pane,1);
  if (this2->list_counts[pane] == 0) {
    field_ptr = (u8 *)&this2->list_first_row[pane];
    field_ptr[0] = 0xff;
    field_ptr[1] = 0xff;
    field_ptr[2] = 0xff;
    field_ptr[3] = 0xff;
  }
  else if (this2->list_counts[pane] <
           this2->list_counts[pane]) {
    this2->list_first_row[pane] =
         this2->list_counts[pane] - 1;
  }
  goto fill_list_bg;
}

/* 00588e47 auto */
void __cdecl
parley_draw_city_sprite(iptr city,uint row_index,int x,int y,int row_height,i32 style)

{
  iptr child;
  int sprite_height2;
  i32 zoom2;
  i32 draw_x;
  i32 sprite_width2;
  ParleyWindow *parley_wnd2;
  
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (parley_wnd2->large_layout == 0) {
    zoom2 = 0xfffffffc;
  }
  else {
    zoom2 = 0xfffffffe;
  }
  draw_x = x;
  if (parley_wnd2->large_layout == 0) {
    sprite_width2 = 0x18;
  }
  else {
    sprite_width2 = 0x24;
  }
  if ((row_index & 1) != 0) {
    draw_x = x + sprite_width2 + 2;
  }
  sprite_height2 = gfx_zoom_scale(0x30,zoom2);
  gfx_draw_city((iptr)&parley_wnd_block,city,style,draw_x,y - (sprite_height2 - row_height) / 2,zoom2);
  return;
}

/* 00588f36 auto */
void __thiscall parley_build_city_list(ParleyWindow *self,int pane,int keep_selection)

{
  int *count_ptr;
  iptr child;
  iptr city;
  int old_count;
  iptr unit;
  void *old_selection;
  ParleyWindow *parley_wnd2;
  
  old_selection = (void *)0x0;
  child = wnd_get_active_child();
  if (child == 0) {
    parley_wnd2 = 0;
  }
  else {
    parley_wnd2 = (ParleyWindow *)(child - 0x48);
  }
  if (keep_selection != 0) {
    old_selection = operator_new(parley_wnd2->list_counts[pane] << 2);
    if (old_selection == (void *)0x0) {
      keep_selection = 0;
    }
    else {
      old_count = parley_wnd2->list_counts[pane];
      for (city = 0; city < old_count; city++) {
        if (parley_wnd2->list_selected[pane * 0x801 + city] == 0) {
          *(i32 *)((int)old_selection + city * 4) = 0xffffffff;
        }
        else {
          *(i32 *)((int)old_selection + city * 4) =
               (i32)parley_wnd2->list_items_b[pane * 0x801 + city];
        }
      }
    }
  }
  parley_wnd2->list_counts[pane] = 0;
  city = 0;
  do {
    if (game.num_cities <= city) {
      if (keep_selection != 0) {
        operator_delete(old_selection);
      }
      parley_sort_city_list(self,0,parley_wnd2->list_counts[pane] - 1,pane);
      return;
    }
    if ((cities[city].id != 0) &&
       (((sprite_picker_sprite_block.parley_pane_own_cities[pane] != 0 && (cities[city].owner == (*(i8 *)&human_civ)))
        || (((sprite_picker_sprite_block.parley_pane_own_cities[pane] == 0 &&
             ((int)cities[city].owner == self->other_civ)) &&
            (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) != 0 ||
             ((int)cities[city].owner == ((*(i8 *)&human_civ) & 0xffU))))))))) {
      parley_wnd2->list_items[pane * 0x801 + parley_wnd2->list_counts[pane]] =
           city;
      parley_wnd2->list_items_b[pane * 0x801 + parley_wnd2->list_counts[pane]] =
           cities[city].id;
      if (keep_selection == 0) {
        parley_wnd2->list_selected[pane * 0x801 + parley_wnd2->list_counts[pane]] = 0;
      }
      else {
        parley_wnd2->list_selected[pane * 0x801 + parley_wnd2->list_counts[pane]] = 0;
        for (unit = 0; unit < old_count; unit++) {
          if (cities[city].id == *(int *)((int)old_selection + unit * 4)) {
            parley_wnd2->list_selected[pane * 0x801 + parley_wnd2->list_counts[pane]] = 1;
            break;
          }
        }
      }
      parley_wnd2->list_selected_b[pane * 0x801 + parley_wnd2->list_counts[pane]] = 0;
      for (unit = 0; unit < game.num_units; unit++) {
        if (((units[unit].id != 0) && (cities[city].x == units[unit].x)) &&
           (cities[city].y == units[unit].y)) {
          count_ptr = (int *)(pane * 0x2004 + parley_wnd2->list_counts[pane] * 4 + 0xc408 +
                          (iptr)parley_wnd2);
          *count_ptr = *count_ptr + 1;
        }
      }
      count_ptr = (int *)((iptr)parley_wnd2 + 1000 + pane * 4);
      *count_ptr = *count_ptr + 1;
    }
    city++;
  } while( true );
}

/* 00594d42 auto */
i32 __cdecl
net_lock_map(iptr civ,iptr src_x,iptr src_y,iptr dst_x,int dst_y,int wait_reply)

{
  i32 result;
  int start_ticks;
  int now_ticks;
  int src_conflict;
  int dst_conflict;
  int slot;
  
  net_session_block.net_lock_map_busy = 1;
  net_map_lock_slots_block.net_map_locks[civ].replies = 0;
  net_map_lock_slots_block.net_map_locks[civ].state = net_map_lock_slots_block.net_map_locks[civ].replies;
  if (net_session_block.net_host_flag == '\0') {
    net_session_block.net_lock_map_busy = 0;
    net_send_message(0x51,0,civ,src_x,src_y,dst_x,dst_y,0,0,0);
    XD_FlushSendBuffer(60000);
    if (wait_reply == 0) {
      result = 1;
    }
    else {
      start_ticks = timer_ticks();
      while( true ) {
        now_ticks = timer_ticks();
        if ((0xe0f < now_ticks - start_ticks) || (net_map_lock_slots_block.net_map_locks[civ].replies != 0)) break;
        net_process_messages(&net_session_block,1,1);
      }
      if (net_map_lock_slots_block.net_map_locks[civ].replies == 0) {
        dbg_log((const char *)&str_lockmap_connection_to_server_timed_out);
        popup_game_with_sprite((const char *)&str_serverconnecttime_8,(iptr)&popup_info_icon_sprite,0);
        game_loop_active = 0;
        result = 0;
      }
      else if (net_map_lock_slots_block.net_map_locks[civ].replies == 1) {
        result = 1;
      }
      else {
        result = 0;
      }
    }
  }
  else {
    dst_conflict = -1;
    src_conflict = -1;
    for (slot = 1; slot < 8; slot++) {
      if (((net_map_lock_slots_block.net_map_locks[slot].src_x == src_x) &&
          (net_map_lock_slots_block.net_map_locks[slot].src_y == src_y)) ||
         ((net_map_lock_slots_block.net_map_locks[slot].dst_x == src_x &&
          (net_map_lock_slots_block.net_map_locks[slot].dst_y == src_y)))) {
        src_conflict = 0;
        break;
      }
      if (((net_map_lock_slots_block.net_map_locks[slot].src_x == dst_x) &&
          (net_map_lock_slots_block.net_map_locks[slot].src_y == dst_y)) ||
         ((net_map_lock_slots_block.net_map_locks[slot].dst_x == dst_x &&
          (net_map_lock_slots_block.net_map_locks[slot].dst_y == dst_y)))) {
        dst_conflict = 0;
        break;
      }
    }
    if ((src_conflict == -1) && (dst_conflict == -1)) {
      net_map_lock_slots_block.net_map_locks[civ].src_x = src_x;
      net_map_lock_slots_block.net_map_locks[civ].src_y = src_y;
      net_map_lock_slots_block.net_map_locks[civ].dst_x = dst_x;
      net_map_lock_slots_block.net_map_locks[civ].dst_y = dst_y;
      if ((human_civ[0] != civ) &&
         (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 &&
          ((((i8 *)&net_session_block.net_slot_active_flags))[net_session_block.net_civ_slots[civ] * 0x54] != '\0')))) {
        net_send_message(0x53,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,
                           civ,1,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
      net_session_block.net_lock_map_busy = 0;
      result = 1;
    }
    else {
      if (((human_civ[0] != civ) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         ((((i8 *)&net_session_block.net_slot_active_flags))[net_session_block.net_civ_slots[civ] * 0x54] != '\0')) {
        net_send_message(0x53,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,
                           civ,-1,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
      net_session_block.net_lock_map_busy = 0;
      result = 0;
    }
  }
  return result;
}

/* 0059511c auto */
void __cdecl net_unlock_map(iptr civ,int wait_reply)

{
  int start_ticks;
  int now_ticks;
  
  net_session_block.net_unlock_map_busy = 1;
  if (net_session_block.net_host_flag == '\0') {
    net_session_block.net_unlock_map_busy = 0;
    net_map_lock_slots_block.net_map_locks[civ].state = 0;
    net_send_message(0x52,0,civ,0,0,0,0,0,0,0);
    if (wait_reply != 0) {
      start_ticks = timer_ticks();
      while( true ) {
        now_ticks = timer_ticks();
        if ((0xe0f < now_ticks - start_ticks) || (net_map_lock_slots_block.net_map_locks[civ].state != 0)) break;
        net_process_messages(&net_session_block,1,1);
      }
      if (net_map_lock_slots_block.net_map_locks[civ].state == 0) {
        dbg_log((const char *)&str_unlockmap_connection_to_server_timed_out);
        popup_game_with_sprite((const char *)&str_serverconnecttime_9,(iptr)&popup_info_icon_sprite,0);
        game_loop_active = 0;
      }
    }
  }
  else {
    _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots) + civ * 0x18,-1,0x18);
    net_send_message(0x55,0xff,civ,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    net_session_block.net_unlock_map_busy = 0;
  }
  return;
}

/* 0059a850 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl net_session_global_init(void)

{
  net_session_static_init();
  net_session_reg_atexit();
  return;
}

/* 0059a86a auto */
void __cdecl net_session_static_init(void)

{
  net_session_ctor(&net_session_block);
  return;
}

/* 0059a884 auto */
void __cdecl net_session_reg_atexit(void)

{
  _atexit(net_session_static_dtor);
  return;
}

/* 0059a8a1 auto */
void __cdecl net_session_static_dtor(void)

{
  net_session_dtor(&net_session_block);
  return;
}

/* 0059a8bb auto */
NetSession * __thiscall net_session_ctor(NetSession *self)

{
  i32 seh_frame;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  net_msg_player_info_init(&self->player_info_msg);
  net_msgq_ctor(&self->msg_queue);
  seh_state = 0;
  ((u8 *)&self->net_connection_title)[0] = '\0';
  ((u8 *)&self->net_connection_title)[1] = '\0';
  ((u8 *)&self->net_connection_title)[2] = '\0';
  ((u8 *)&self->net_connection_title)[3] = '\0';
  XD_SetBroadcastReceive(net_on_broadcast_receive);
  XD_SetSecureReceive(net_on_secure_receive);
  XD_SetOnClientConnectionToServer(net_on_connected_to_server);
  XD_SetNewClientConnection(net_on_new_client);
  XD_SetOnConnectionLost(net_on_connection_lost);
  XD_SetOversizedMessageCB(50000,net_on_oversized_message);
  return self;
}

/* 0059a998 auto */
void __thiscall net_session_clear(NetSession *self)

{
  int i;
  
  self->net_connected = 0;
  self->net_connected_to_server = 0;
  self->net_popup_in_progress_i32 = 0;
  self->net_listing_games = 0;
  self->net_session_phase = 0;
  self->net_found_games = 0;
  self->gamelist_view = 0;
  self->net_lobby_list = 0;
  self->dialog_stack_top = 0;
  self->net_server_conn_id = 0xffffffff;
  (*(i32 *)&net_session_block.net_xd_adapter_index) = 0;
  self->net_adapter_index = 0;
  net_session_block.net_active_protocol = 0xffffffff;
  self->net_protocol = 0xffffffff;
  self->mp_turn_civ = 0;
  self->player_has_quit = 0;
  self->net_poll_depth = 0;
  self->net_poll_depth_peak = 0;
  self->net_msgq_normal_peak = 0;
  self->net_msgq_priority_peak = 0;
  self->net_host_handoff_stage = 0;
  self->mp_simultaneous_moves = 0;
  self->net_tile_sync_enabled = 0;
  self->net_tile_batch_active = 0;
  self->chat_missed_message_count_i32 = 8;
  self->chat_unread_count = 0;
  self->net_timeout_seconds = 0x3c;
  self->net_move_anim_sound = 0xffffffff;
  self->net_move_anim_sound_arg = 0xffffffff;
  self->unit_create_busy = 0;
  self->unit_delete_busy = 0;
  self->city_create_busy = 0;
  self->city_delete_busy = 0;
  self->net_lock_map_busy = 0;
  self->net_unlock_map_busy = 0;
  self->ui_input_busy = 0;
  self->unit_stack_unlink_busy = 0;
  self->unit_stack_link_busy = 0;
  self->unit_stack_bottom_busy = 0;
  self->unit_relocate_busy = 0;
  self->city_acquire_wonder_busy = 0;
  self->civ_eliminate_busy = 0;
  self->civ_create_busy = 0;
  self->civ_check_destroyed_busy = 0;
  self->map_redraw_busy = 0;
  self->net_stacked_draw_count = 0;
  self->net_stacked_draw_first = 0;
  self->net_stacked_draw_last = 0;
  self->unit_stack_check_enabled = 0;
  self->net_stacked_draw_peak = 0;
  for (i = 0; i < 8; i++) {
    self->casualty_counts[i] = 0;
    self->casualty_reported_counts[i] = 0;
    self->casualty_log_first[i] = 0;
    self->casualty_log_next[i] = 0;
  }
  self->hotseat_human_players = 0;
  self->unit_gather_cargo_busy = 0;
  self->unit_lift_off_map_busy = 0;
  self->unit_delete_cargo_busy = 0;
  self->unit_destroy_redraw_busy = 0;
  mp_meeting_pending = 0;
  self->pending_human_players = 0;
  for (i = 0; i < 8; i++) {
    self->net_civ_new_player_flags[i] = 0;
    self->net_civ_turn_ended[i] = 0;
    self->net_civ_drop_requested[i] = 0;
  }
  self->blocked_input_kind = 0;
  self->net_drag_hold = 0;
  net_session_shutdown(self,1);
  net_reset_error_counts();
  return;
}

/* 0059ad40 auto */
void __thiscall net_session_dtor(NetSession *self)

{
  i32 seh_frame;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0;
  if (self->net_connection_title != 0) {
    operator_delete((void *)self->net_connection_title);
    self->net_connection_title = 0;
  }
  net_session_shutdown(self,1);
  net_reset_error_counts();
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0059adc8 */ { net_msgq_clear(&self->msg_queue); }
  return;
}

/* 0059adef auto */
i8 __thiscall net_init_transport(NetSession *self,i32 protocol,int is_server)

{
  i32 saved_protocol;
  i8 result;
  UINT timeout_secs;
  int activate_error;
  int init_error;
  
  if (self->net_connected != '\0') {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_bconnectedtonet,(iptr)&str_d_ss_franklinton_netmgr_cpp,0x92);
  }
  saved_protocol = net_session_block.net_protocol;
  net_msgq_reset(&self->msg_queue);
  net_session_clear(self);
  _memset((void *)self->net_player_count,0,0x270);
  switch(protocol) {
  case 0:
    init_error = XD_InitializeSocketsTCP
                        (3,0x1381,0x1382,(int)(char)is_server,7,NULL
                        );
    game.multiplayer = 3;
    if (self->net_connection_type == 0) {
      timeout_secs = GetPrivateProfileIntA
                        (str_civilization_gold_17,str_internet_timeout,-1,
                         str_civ_ini_17);
      if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0x3c)) {
        WritePrivateProfileStringA
                  (str_civilization_gold_18,str_internet_timeout_2,((i8 *)&net_internet_timeout_default),
                   str_civ_ini_18);
        self->net_timeout_seconds = 0x3c;
      }
      else {
        self->net_timeout_seconds = timeout_secs;
      }
    }
    else {
      timeout_secs = GetPrivateProfileIntA
                        (str_civilization_gold_19,str_tcpip_timeout,-1,str_civ_ini_19
                        );
      if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0xf)) {
        WritePrivateProfileStringA
                  (str_civilization_gold_20,str_tcpip_timeout_2,((i8 *)&net_tcpip_timeout_default),
                   str_civ_ini_20);
        self->net_timeout_seconds = 0xf;
      }
      else {
        self->net_timeout_seconds = timeout_secs;
      }
    }
    break;
  case 1:
    init_error = XD_InitializeSocketsIPXSPX
                        (3,0x1381,0x1382,(int)(char)is_server,7,NULL
                        );
    game.multiplayer = 3;
    timeout_secs = GetPrivateProfileIntA
                      (str_civilization_gold_21,str_ipxspx_timeout,-1,str_civ_ini_21)
    ;
    if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0xf)) {
      WritePrivateProfileStringA
                (str_civilization_gold_22,str_ipxspx_timeout_2,((i8 *)&net_ipxspx_timeout_default),
                 str_civ_ini_22);
      self->net_timeout_seconds = 0xf;
    }
    else {
      self->net_timeout_seconds = timeout_secs;
    }
    break;
  case 2:
    game.multiplayer = 5;
    timeout_secs = GetPrivateProfileIntA
                      (str_civilization_gold_23,str_modem_timeout,-1,str_civ_ini_23);
    if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0x1e)) {
      WritePrivateProfileStringA
                (str_civilization_gold_24,str_modem_timeout_2,((i8 *)&net_modem_timeout_default),
                 str_civ_ini_24);
      self->net_timeout_seconds = 0x1e;
    }
    else {
      self->net_timeout_seconds = timeout_secs;
    }
    init_error = XD_InitializeModem((int)(char)is_server);
    break;
  case 3:
    game.multiplayer = 6;
    timeout_secs = GetPrivateProfileIntA
                      (str_civilization_gold_25,str_direct_timeout,-1,str_civ_ini_25)
    ;
    if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0x1e)) {
      WritePrivateProfileStringA
                (str_civilization_gold_26,str_direct_timeout_2,((i8 *)&net_direct_timeout_default),
                 str_civ_ini_26);
      self->net_timeout_seconds = 0x1e;
    }
    else {
      self->net_timeout_seconds = timeout_secs;
    }
    init_error = XD_InitializeSerial((int)(char)is_server);
    break;
  default:
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_0_6,(iptr)&str_d_ss_franklinton_netmgr_cpp_2,0xf4);
  }
  if (init_error == 0) {
    if (((char)is_server == '\0') || (activate_error = XD_ActivateServer(), activate_error == 0)) {
      self->net_host_flag = (char)is_server;
      self->net_connected = 1;
      net_session_block.net_active_protocol = protocol;
      self->net_protocol = protocol;
      net_reset_error_counts();
      net_reset_msg_counts();
      _memset((void *)self->net_slot_addresses,0,0x20);
      _strncpy((char *)self->net_slot_addresses,((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),0x20);
      _memset((void *)self->net_slot_player_names,0,0x20);
      _strncpy((char *)self->net_slot_player_names,((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name),0x20);
      self->net_player_count[0] = 1;
      self->net_slot_active_flags[0] = 1;
      self->net_slot_host_flags[0] = self->net_host_flag;
      result = 1;
    }
    else {
      dbg_logf((const char *)&str_connecttonet_xd_activateserver_failed_re,activate_error);
      result = 0;
      net_session_block.net_protocol = saved_protocol;
    }
  }
  else {
    dbg_logf((const char *)&str_connecttonet_xd_initialize_failed_return,init_error);
    result = 0;
    net_session_block.net_protocol = saved_protocol;
  }
  return result;
}

/* 0059b293 auto */
/* WARNING: Removing unreachable block (ram,0x0059b3ca) */

void __thiscall net_session_shutdown(NetSession *self,int delay_after)

{
  dbg_log((const char *)&str_disconnecting_from_network);
  if (self->net_host_flag != '\0') {
    delay_after = 0;
  }
  if (self->net_connected == '\0') {
    XD_ResetLibrary();
    dbg_log((const char *)&str_xd_resetlibrary_2);
  }
  else {
    net_free_game_list(self);
    if (self->net_connected_to_server != '\0') {
      XD_FlushSendBuffer(60000);
      dbg_log((const char *)&str_send_buffer_has_flushed);
      net_msgq_reset(&self->msg_queue);
      net_popup_queue_clear();
      XD_CloseConnection();
      net_msgq_reset(&self->msg_queue);
      net_popup_queue_clear();
      dbg_log((const char *)&str_xd_closeconnection);
      net_session_close_hook_nop(self);
    }
    switch(self->net_protocol) {
    case 0:
      XD_ShutdownSockets();
      dbg_log((const char *)&str_xd_shutdownsockets);
      break;
    case 1:
      XD_ShutdownSockets();
      dbg_log((const char *)&str_xd_shutdownsockets_2);
      break;
    case 2:
      XD_ShutdownModem();
      dbg_log((const char *)&str_xd_shutdownmodem);
      break;
    case 3:
      XD_ShutdownTEN();
      dbg_log((const char *)&str_xd_shutdownten);
    }
    net_msgq_reset(&self->msg_queue);
    net_popup_queue_clear();
    if (0 < self->net_poll_depth_peak) {
      dbg_logf((const char *)&str_maximum_gnetmgr_poll_call_depth_d,self->net_poll_depth_peak);
    }
    if (0 < (int)self->net_msgq_priority_peak) {
      dbg_logf((const char *)&str_maximum_alpha_message_queue_load_d,(iptr)self->net_msgq_priority_peak);
    }
    if (0 < (int)self->net_msgq_normal_peak) {
      dbg_logf((const char *)&str_maximum_message_queue_load_d,(iptr)self->net_msgq_normal_peak);
    }
    if (self->net_stacked_draw_peak != 0) {
      dbg_logf((const char *)&str_maximum_draw_stack_load_d,(iptr)self->net_stacked_draw_peak);
    }
    XD_ResetLibrary();
    dbg_log((const char *)&str_xd_resetlibrary);
    if (delay_after != 0) {
      dbg_log((const char *)&str_disconnection_delay_2_seconds);
      ui_delay_ticks(0x78);
    }
  }
  self->net_connected = 0;
  self->net_connected_to_server = 0;
  self->net_listing_games = 0;
  self->net_server_conn_id = 0xffffffff;
  net_reset_error_counts();
  net_msgq_reset(&self->msg_queue);
  net_popup_queue_clear();
  return;
}

/* 0059b55b auto */
void __thiscall net_session_close_hook_nop(NetSession *self)

{
  return;
}

/* 0059b571 auto */
void __thiscall net_game_list_add(NetSession *self,NetServerInfoMsg *msg)

{
  int cmp;
  NetGameInfo *entry;
  i32 now_ticks;
  NetGameInfo *prev;
  NetGameInfo *node2;
  i32 saved_value;
  
  if (self->net_listing_games != '\0') {
    node2 = self->net_found_games;
    saved_value = 0;
    for (; node2 != 0; node2 = node2->next) {
      cmp = _strncmp(node2->host_address,msg->game.host_address,0x20);
      if (cmp == 0) {
        if (self->net_found_games == node2) {
          self->net_found_games = node2->next;
        }
        if (node2->prev != 0) {
          node2->prev->next = node2->next;
        }
        if (node2->next != 0) {
          node2->next->prev = node2->prev;
        }
        saved_value = node2->list_id;
        operator_delete(node2);
        break;
      }
    }
    node2 = self->net_found_games;
    prev = 0;
    while ((node2 != 0 &&
           (cmp = __strcmpi(msg->game.game_name,node2->game_name), 0 < cmp))) {
      prev = node2;
      node2 = node2->next;
    }
    while (((node2 != 0 &&
            (cmp = __strcmpi(msg->game.game_name,node2->game_name), cmp == 0)) &&
           (cmp = __strcmpi(msg->game.host_address,node2->host_address), 0 < cmp))) {
      prev = node2;
      node2 = node2->next;
    }
    entry = (NetGameInfo *)operator_new(0x124);
    if (entry == 0) {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_pcur_null,(iptr)&str_d_ss_franklinton_netmgr_cpp_3,0x1cf);
    }
    crt_memcpy(entry,&msg->game,0x124);
    if (prev == 0) {
      entry->next = self->net_found_games;
      entry->prev = 0;
      if (self->net_found_games != 0) {
        self->net_found_games->prev = entry;
      }
      self->net_found_games = entry;
    }
    else {
      entry->next = prev->next;
      entry->prev = prev;
      prev->next = entry;
      if (entry->next != 0) {
        entry->next->prev = entry;
      }
    }
    now_ticks = timer_ticks();
    entry->last_seen = now_ticks;
    entry->list_id = saved_value;
  }
  return;
}

/* 0059b7fc auto */
void __thiscall net_add_player(NetSession *self,NetPlayerNodeMsg *player_info)

{
  int slot;
  
  slot = 0;
  while( true ) {
    if (6 < slot) {
      return;
    }
    if (self->net_slots[slot].active == '\0') break;
    slot++;
  }
  self->net_slots[slot].id = player_info->slot.id;
  _memset((void *)((int)self + slot * 0x54 + 0x208),0,0x20);
  _strncpy((char *)((int)self + slot * 0x54 + 0x208),(char *)((iptr)player_info + 0x14),0x20);
  self->net_slots[slot].is_host = 0;
  self->net_slots[slot].active = 1;
  self->net_slots[slot].civ = 0xffffffff;
  crt_strcpy((char *)((void *)((int)self + slot * 0x54 + 0x228)),(const char *)((const void *)((iptr)player_info + 0x34)));
  self->net_slots[slot].field_44 = 0;
  self->net_slots[slot].lobby_id = 0;
  self->net_player_count[0] = self->net_player_count[0] + 1;
  self->net_players_awaited--;
  return;
}

/* 0059b96a auto */
void __thiscall net_remove_player(NetSession *self,int player_id)

{
  u8 *civ_slot_ptr;
  int slot;
  
  slot = 0;
  while ((slot < 7 && (self->net_slots[slot].id != player_id))) {
    slot++;
  }
  if ((slot < 7) && (self->net_slots[slot].active != '\0')) {
    if ((0 < self->net_slots[slot].civ) &&
       (self->net_slots[slot].civ < 8)) {
      game.human_players =
           ~(byte)(1 << (*(byte *)((int)self + slot * 0x54 + 0x254) & 0x1f)) & game.human_players
      ;
    }
    self->net_slots[slot].id = 0;
    self->net_slots[slot].address[0] = 0;
    self->net_slots[slot].is_host = 0;
    self->net_slots[slot].active = 0;
    civ_slot_ptr = (u8 *)self->net_civ_slots + self->net_slots[slot].civ * 4;
    civ_slot_ptr[0] = 0xff;
    civ_slot_ptr[1] = 0xff;
    civ_slot_ptr[2] = 0xff;
    civ_slot_ptr[3] = 0xff;
    self->net_slots[slot].civ = 0xffffffff;
    self->net_player_count[0] = self->net_player_count[0] - 1;
  }
  return;
}

/* 0059baf0 auto */
void __thiscall net_free_game_list(NetSession *self)

{
  NetGameInfo *next;
  
  while (self->net_found_games != 0) {
    next = self->net_found_games->next;
    operator_delete(self->net_found_games);
    self->net_found_games = next;
  }
  return;
}

/* 0059bb54 auto */
void __cdecl net_on_broadcast_receive(void *data,ushort size,long header)

{
  int *msg = (int *)data;
  int cmp;
  
  if (size < 0x10) {
    dbg_log((const char *)&str_broadcastreceivefunc_killed_message_tiny);
  }
  else if (*msg == 0x66606660) {
    if ((msg[1] != 0) ||
       ((((char)msg[0xc] == '\0' ||
         (cmp = _strcmp((char *)(msg + 0xc),((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address)), cmp == 0)) &&
        (((char)msg[0x14] == '\0' ||
         (cmp = _strcmp((char *)(msg + 0x14),((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address)), cmp != 0)))))) {
      net_msgq_enqueue(&net_session_block.msg_queue,0,(NetMsg *)msg,(uint)size);
    }
  }
  else {
    dbg_log((const char *)&str_broadcastreceivefunc_killed_message_not);
  }
  return;
}

/* 0059bc41 auto */
void __cdecl net_on_secure_receive(ushort conn_id,void *data,unsigned long size,short flags)

{
  int *msg = (int *)data;
  size_t len;
  int *scan_ptr;
  uptr byte_idx;
  uptr offset;
  char num_buf[32];
  i8 hex_line[64];
  uint byte_val;
  int *header;
  
  if (3 < size) {
    if (*msg == 0x66606660) {
      net_msgq_enqueue
                (&net_session_block.msg_queue,conn_id & 0xffff,(NetMsg *)msg,size);
      net_session_block.net_last_recv_callback_size = size;
      net_session_block.net_last_queued_send_seq = net_send_sequence;
      header = msg;
      net_session_block.net_last_recv_msg_size = msg[2];
      net_last_recv_msg_type = msg[1];
    }
    else {
      dbg_log((const char *)&str_securereceivefunc_killed_message_not_civ);
      if (-1 < net_last_recv_msg_type) {
        __ultoa(net_session_block.net_last_recv_callback_size,num_buf,10);
        dbg_logf_str((const char *)&str_last_callback_size_s,(const char *)num_buf);
        __itoa(net_session_block.net_last_queued_send_seq,num_buf,10);
        dbg_logf_str((const char *)&str_last_legal_sequence_number_s,(const char *)num_buf);
        __itoa(net_last_recv_msg_type,num_buf,10);
        dbg_logf_str((const char *)&str_last_legal_type_s,(const char *)num_buf);
        __ultoa(net_session_block.net_last_recv_msg_size,num_buf,10);
        dbg_logf_str((const char *)&str_last_legal_netmsg_size_s,(const char *)num_buf);
      }
      __itoa(net_send_sequence,num_buf,10);
      dbg_logf_str((const char *)&str_sequence_number_s,(const char *)num_buf);
      __ultoa(size,num_buf,10);
      dbg_logf_str((const char *)&str_message_size_s,(const char *)num_buf);
      for (offset = 0; (offset < 0x20 && (hex_line[0] = 0, offset <= size));
          offset = offset + 8) {
        for (byte_idx = offset; (byte_idx < offset + 8 && (byte_idx < size));
            byte_idx = byte_idx + 1) {
          byte_val = (uint)*(byte *)(byte_idx + (int)msg);
          __itoa(byte_val,num_buf,0x10);
          len = _strlen(num_buf);
          if (len == 1) {
            num_buf[1] = num_buf[0];
            num_buf[2] = 0;
            num_buf[0] = '0';
          }
          crt_strcat(hex_line,num_buf);
          crt_strcat(hex_line,((i8 *)&net_hexdump_space_str));
        }
        dbg_logf_str((const char *)&str_s_3,(const char *)hex_line);
      }
      dbg_log((const char *)&str_empty_184);
      net_last_recv_msg_type = -1;
      scan_ptr = msg;
      for (offset = 0; offset < 0x20; offset++) {
        if (*scan_ptr == 0x66606660) {
          header = scan_ptr;
          dbg_log((const char *)&str_potential_offset_message_found);
          __itoa(offset,num_buf,10);
          dbg_logf_str((const char *)&str_offset_s_bytes,(const char *)num_buf);
          dbg_logf_str((const char *)&str_type_s,(const char *)(((char *)&str_nm_datagram) + header[1] * 0x20));
          __ultoa(header[2],num_buf,10);
          dbg_logf_str((const char *)&str_size_s_bytes,(const char *)num_buf);
          __ultoa(header[3],num_buf,10);
          dbg_logf_str((const char *)&str_sequence_number_s_2,(const char *)num_buf);
          break;
        }
        scan_ptr = (int *)((int)scan_ptr + 1);
      }
    }
  }
  if (((300 < net_session_block.net_msgq_normal_count) || (0x21 < net_session_block.net_msgq_priority_count)) && (mp_window_drag_active != '\0')) {
    mp_release_mouse_capture();
  }
  return;
}

/* 0059bfb5 auto */
void __cdecl net_on_new_client(ushort connected_count,ushort conn_id)

{
  dbg_logf((const char *)&str_newclientreceivefunc_new_client_connecti,conn_id & 0xffff);
  return;
}

/* 0059bfdb auto */
void __cdecl net_on_connected_to_server(short conn_id)

{
  dbg_logf((const char *)&str_connectedtoserverfunc_new_server_connect,(int)conn_id);
  if ((-1 < conn_id) && (conn_id < 8)) {
    net_session_block.net_server_conn_id = (int)conn_id;
    net_session_block.net_connected_to_server = 1;
    net_send_message(0x2f,0,(int)conn_id,0,0,0,0,0,0,0);
  }
  return;
}

/* 0059c04b auto */
void __cdecl net_on_connection_lost(ushort player_id)

{
  dbg_logf((const char *)&str_lostconnectionfunc_connection_to_d_has_s,(uint)player_id);
  net_session_block.net_lost_conn_total++;
  if (player_id < 7) {
    net_session_block.net_lost_conn_by_player[(uint)player_id] = net_session_block.net_lost_conn_by_player[(uint)player_id] + 1;
  }
  return;
}

/* 0059c0a4 auto */
void __cdecl net_on_oversized_message(unsigned long size)

{
  char msg_buf[256];
  
  _sprintf(msg_buf,str_oversized_xdaemon_message_ul,size);
  dbg_log((const char *)msg_buf);
  return;
}

/* 0059c0e1 auto */
void * __cdecl
net_msg_build_packet(i32 msg_type,iptr payload,i32 arg1,i32 arg2,
            i32 arg3,i32 arg4,i32 arg5,i32 arg6,
            size_t payload_len)

{
  void *packet;
  NetMsg header;  /* only the 16-byte header is used */
  size_t payload_size;
  
  net_msg_header_init(&header.hdr,msg_type);
  if (payload_len == 0) {
    if (payload == 0) {
      payload_size = 0;
      packet = operator_new(0x2c);
    }
    else {
      payload_size = __msize((void *)payload);
      packet = crt_realloc((void *)payload,payload_size + 0x2c);
      crt_memmove((void *)((int)packet + 0x2c),packet,payload_size);
    }
  }
  else {
    payload_size = payload_len;
    packet = operator_new(payload_len + 0x2c);
    crt_memcpy((void *)((int)packet + 0x2c),(const void *)payload,payload_size);
  }
  header.size = payload_size + 0x2c;
  crt_memcpy(packet,&header,0x10);
  crt_memcpy((void *)((int)packet + 0x10),&arg1,4);
  crt_memcpy((void *)((int)packet + 0x14),&arg2,4);
  crt_memcpy((void *)((int)packet + 0x18),&arg3,4);
  crt_memcpy((void *)((int)packet + 0x1c),&arg4,4);
  crt_memcpy((void *)((int)packet + 0x20),&arg5,4);
  crt_memcpy((void *)((int)packet + 0x24),&arg6,4);
  crt_memcpy((void *)((int)packet + 0x28),&payload_size,4);
  return packet;
}

/* 0059c276 auto */
void __cdecl net_reset_msg_counts(void)

{
  int i;
  
  for (i = 0; i < 0xa9; i++) {
    net_session_block.nm_recv_counts_by_type[i] = 0;
  }
  return;
}

/* 0059c2b8 auto */
void __cdecl net_reset_error_counts(void)

{
  int i;
  
  net_session_block.net_lost_conn_total = 0;
  for (i = 0; i < 7; i++) {
    net_session_block.net_lost_conn_by_player[i] = 0;
  }
  return;
}

/* 0059c301 auto */
void __cdecl net_modal_idle_poll(void)

{
  net_process_messages(&net_session_block,1,0);
  return;
}

/* 0059c31f auto */
/* WARNING: Type propagation algorithm not settling */

void __cdecl net_fill_game_info(NetGameInfo *info)

{
  i32 bit_masks[9];
  
  bit_masks[0] = 0;
  bit_masks[1] = 1;
  bit_masks[2] = 3;
  bit_masks[3] = 7;
  bit_masks[4] = 0xf;
  bit_masks[5] = 0x1f;
  bit_masks[6] = 0x3f;
  bit_masks[7] = 0x7f;
  bit_masks[8] = 0xff;
  crt_strcpy(info->version,str_ptr_5_4_0f_multiplayer_26_march_99);
  _memset(info->host_address,0,0x20);
  _strncpy(info->host_address,((i8 *)&prefs_main_menu_choice_block.prefs_net_ip_address),0x20);
  _memset(info->game_name,0,0x20);
  _strncpy(info->game_name,((i8 *)&net_session_block.net_game_name),0x20);
  _memset(info->host_name,0,0x20);
  _strncpy(info->host_name,((i8 *)&prefs_main_menu_choice_block.prefs_net_player_name),0x20);
  info->open_game = civ_saved_cursor_x_block.mp_open_game;
  info->difficulty = game.difficulty;
  info->barbarian_activity = game.barbarian_activity;
  if (game.civs_alive == '\0') {
    info->civs_alive = *(i8 *)(bit_masks + (uint)game.num_enemies + 2);
  }
  else {
    info->civs_alive = game.civs_alive;
  }
  info->player_mask = *(i8 *)(bit_masks + net_session_block.net_player_count[0]);
  info->turn_time_limit_ms = civ_saved_cursor_x_block.mp_turn_time_limit_ms;
  crt_strcpy(info->scenario_name,((i8 *)&prefs_game_rules_block.scenario_name));
  info->double_moves = civ_saved_cursor_x_block.mp_double_moves;
  info->double_production = civ_saved_cursor_x_block.mp_double_production;
  info->game_flags = game.game_flags;
  info->first_launch_turn = game.first_launch_turn;
  info->year_increment = prefs_game_rules_block.scenario_year_increment[0];
  info->start_year = prefs_game_rules_block.scenario_start_year;
  info->turn = game.turn;
  info->simultaneous = civ_saved_cursor_x_block.mp_simultaneous_setting;
  info->prev = 0;
  info->next = info->prev;
  info->list_id = 0;
  info->last_seen = info->list_id;
  info->map_width = map.width;
  info->map_height = map.height;
  return;
}

/* 0059d080 auto */
NetMsgHeader * __thiscall net_msg_player_info_init(NetPlayerInfoMsg *self)

{
  iptr j;
  int i;
  
  net_msg_header_init(&self->hdr,0x2e);
  self->civ = 0;
  self->leader_index = 0;
  self->gender = 0;
  self->leader_name[0] = 0;
  self->tribe_name[0] = 0;
  self->adjective[0] = 0;
  self->city_style = 0;
  for (i = 0; i < 7; i++) {
    for (j = 0; j < 0x18; j++) {
      self->govt_titles[i][j] = 0;
    }
  }
  self->has_names = 0;
  self->has_titles = 0;
  self->size = 0x118;
  return &self->hdr;
}

/* 005a9f30 auto */
void __cdecl net_host_wait_production_idle(void)

{
  bool all_done;
  int civ;
  
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.nm_production_complete_count != 0) || (net_session_block.net_player_count[0] < 2)) {
    all_done = true;
    for (civ = 1; civ < 8; civ++) {
      if ((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (dialog_text_font_obj_block.net_civ_phase_done[civ] == 0)) {
        all_done = false;
        break;
      }
    }
    if (all_done) {
      net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
      wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
    }
  }
  return;
}

/* 005aa004 auto */
void __cdecl net_host_wait_human_moves_idle(void)

{
  bool all_done;
  int civ;
  
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.nm_human_move_complete_count != 0) || (net_session_block.net_player_count[0] < 2)) {
    all_done = true;
    for (civ = 1; civ < 8; civ++) {
      if ((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (dialog_text_font_obj_block.net_civ_phase_done[civ] == 0)) {
        all_done = false;
        break;
      }
    }
    if ((all_done) || (net_session_block.net_player_count[0] < 2)) {
      net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
      wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
    }
  }
  return;
}

/* 005aa0e5: written by hand in src/game/bigframes.cpp */

/* 005ab0a3 auto */
void __cdecl net_client_wait_production_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  net_heartbeat_check(net_session_block.net_slot_civs[0]);
  if ((((net_session_block.nm_ai_movement_count != 0) || (net_session_block.nm_end_sequence_count != 0)) || (game_loop_active == '\0')) ||
     (net_session_block.net_lost_conn_total != 0)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 005ab120 auto */
void __cdecl net_client_wait_ai_moves_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  net_heartbeat_check(net_session_block.net_slot_civs[0]);
  if ((((net_session_block.nm_human_movement_count != 0) || (net_session_block.nm_end_sequence_count != 0)) || (game_loop_active == '\0')) ||
     (net_session_block.net_lost_conn_total != 0)) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 005ab19d auto */
void __cdecl net_client_wait_human_moves_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  net_heartbeat_check(net_session_block.net_slot_civs[0]);
  if ((((net_session_block.nm_production_begin_count != 0) || (net_session_block.net_host_handoff_stage == 1)) || (net_session_block.nm_end_sequence_count != 0)) ||
     (((net_session_block.player_has_quit == 1 || (game_loop_active == '\0')) || (net_session_block.net_lost_conn_total != 0)))) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 005ab23a auto */
void __cdecl net_client_hotwait_idle(void)

{
  net_process_messages(&net_session_block,1,0);
  net_heartbeat_check(net_session_block.net_slot_civs[0]);
  if ((((net_session_block.nm_production_begin_count != 0) || (net_session_block.net_host_handoff_stage != '\0')) || (net_session_block.nm_end_sequence_count != 0)) ||
     (((net_session_block.player_has_quit != '\0' || (game_loop_active == '\0')) || (net_session_block.net_lost_conn_total != 0)))) {
    net_session_block.dialog_stack_top[0xf] = net_session_block.dialog_stack_top[0xf] | 0x400;
    wnd_end_modal_loop((ModalMenuWindow *)(*net_session_block.dialog_stack_top + 0x48));
  }
  return;
}

/* 005ab2d5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_client_turn_loop(void)

{
  char resume_civ;
  SHORT esc_state;
  iptr str;
  int status;
  i32 ret;
  iptr slot_civ;
  int civ;
  iptr unit;
  Dialog dialog2;
  i8 turn_ok;
  int centered;
  int new_casualties;
  int is_first_turn;
  i32 seh_frame;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  is_first_turn = 1;
  dialog_ctor(&dialog2,0x4000);
  seh_state = 0;
  turn_ok = 1;
  slot_civ = net_session_block.net_slot_civs[0];
  str = (iptr)(civ_name_plural(net_session_block.net_slot_civs[0]));
  dbg_logf_str_int((const char *)&str_server_s_d_4,(const char *)str,slot_civ);
  for (civ = 1; civ < 8; civ++) {
    if (((net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id != 0) &&
        ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      status = civ;
      str = (iptr)(civ_name_plural(civ));
      dbg_logf_str_int((const char *)&str_client_s_d_4,(const char *)str,status);
    }
  }
  net_reset_error_counts();
  human_civ[0] = (((i32 *)&net_session_block.net_slot_civs))[net_session_block.net_local_slot * 0x15];
  turn_resume_civ = civ_saved_cursor_x_block.saved_turn_civ;
  turn_resume_new_game = (int)civ_saved_cursor_x_block.saved_resume_new_game;
  picwnd_close((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
  picwnd_close((PictureWindow *)((i8 *)&casualty_window_block.casualty_window));
  menu_update_states();
  game.player_civ = (byte)human_civ[0];
  game.turn_civ = (byte)human_civ[0];
  mapview_update_title((MapView *)((i8 *)&map_views_block.map_views));
  centered = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
      mapview_center_on
                ((MapView *)((i8 *)&map_views_block.map_views),(int)units[unit].x,(int)units[unit].y);
      centered++;
      break;
    }
  }
  if (centered == 0) {
    for (unit = 0; unit < game.num_cities; unit++) {
      if ((cities[unit].id != 0) && (cities[unit].owner == human_civ[0])) {
        mapview_center_on
                  ((MapView *)((i8 *)&map_views_block.map_views),(int)cities[unit].x,(int)cities[unit].y
                  );
        break;
      }
    }
  }
  status_draw_info(1);
  status_draw_civ_color(human_civ[0]);
  ui_redraw_all_maps(1);
  resume_civ = turn_resume_civ;
  if ((1 << ((byte)human_civ[0] & 0x1f) & (uint)game.human_players) == 0) {
    net_heartbeat_send(net_session_block.net_slot_civs[0]);
    dlg_pending_idle_callback = net_client_hotwait_idle;
    popup_game_sprite_flags((const char *)&str_clienthotwait_2,0x2000000,(iptr)&popup_info_icon_sprite,0);
    resume_civ = turn_resume_civ;
  }
turn_loop:
  do {
    turn_resume_civ = resume_civ;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
        unit_exhaust_moves(unit);
      }
    }
    if (is_first_turn == 0) {
      net_heartbeat_send(net_session_block.net_slot_civs[0]);
      dlg_pending_idle_callback = net_client_wait_human_moves_idle;
      popup_game_with_sprite((const char *)&str_waithumanmoves_2,(iptr)&popup_info_icon_sprite,0);
    }
    net_heartbeat_send(net_session_block.net_slot_civs[0]);
    while ((((net_session_block.nm_production_begin_count == 0 && (net_session_block.net_host_handoff_stage == '\0')) && (net_session_block.nm_end_sequence_count == 0)) &&
           ((net_session_block.player_has_quit == '\0' && (game_loop_active != '\0'))))) {
      net_process_messages(&net_session_block,1,0);
      net_heartbeat_check(net_session_block.net_slot_civs[0]);
      status = net_check_lost_connections();
      if (status != 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog_dtor(&dialog2); }
        ret = 0;
        goto return_ret;
      }
    }
    if (net_session_block.net_host_handoff_stage != '\0') {
      status = net_client_transfer_server();
      if (status == 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog_dtor(&dialog2); }
        ret = 0;
        goto return_ret;
      }
      if (net_session_block.net_host_flag != '\0') {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog_dtor(&dialog2); }
        ret = 1;
        goto return_ret;
      }
      if (((net_session_block.nm_production_begin_count == 0) && (net_session_block.nm_end_sequence_count == 0)) &&
         ((net_session_block.player_has_quit == '\0' && (resume_civ = turn_resume_civ, game_loop_active != '\0'))))
      goto turn_loop;
    }
    if ((net_session_block.nm_end_sequence_count == 0) && (game_loop_active != '\0')) {
      net_session_block.nm_start_civ_count = 0;
      net_session_block.nm_production_begin_count = 0;
      human_civ[0] = (((i32 *)&net_session_block.net_slot_civs))[net_session_block.net_local_slot * 0x15];
      game.player_civ = (byte)human_civ[0];
      game.turn_civ = (byte)human_civ[0];
      if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
        mp_turn_seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
        map_views_block.main_view_timer_cache_i32 = 0xffffffff;
      }
      while ((net_session_block.net_msgq_priority_count != 0 || (net_session_block.net_msgq_normal_count != 0))) {
        net_process_messages(&net_session_block,1,0);
      }
      net_session_block.mp_turn_civ = human_civ[0];
      mapview_update_title((MapView *)((i8 *)&map_views_block.map_views));
      map_redraw_all_views(human_civ[0],1);
      status_draw_info(1);
      status_draw_civ_color(human_civ[0]);
      if (civ_saved_cursor_x_block.autoplay_active == 0) {
        if (turn_resume_civ == '\0') {
          civ_check_throne_room_upgrade();
        }
        if ((turn_resume_civ == '\0') || (turn_resume_new_game != 0)) {
          civ_process_turn(human_civ[0]);
          net_send_state_diff(0xff,2,0,0,0);
        }
        XD_FlushSendBuffer(60000);
        game_turn_start_popups();
        net_send_message(0x8c,0,human_civ[0],0,0,0,0,0,0,0);
        net_heartbeat_send(net_session_block.net_slot_civs[0]);
        dlg_pending_idle_callback = net_client_wait_production_idle;
        popup_game_with_sprite((const char *)&str_waitproduction_3,(iptr)&popup_info_icon_sprite,0);
        while (((net_session_block.nm_ai_movement_count == 0 && (net_session_block.nm_end_sequence_count == 0)) && (game_loop_active != '\0'))) {
          net_process_messages(&net_session_block,1,0);
          net_heartbeat_check(net_session_block.net_slot_civs[0]);
          status = net_check_lost_connections();
          if (status != 0) {
            seh_state = 0xffffffff;
            /* inlined unwind funclet 005abfdb */ { dialog_dtor(&dialog2); }
            ret = 0;
            goto return_ret;
          }
        }
        if ((net_session_block.nm_end_sequence_count == 0) && (game_loop_active != '\0')) {
          net_session_block.nm_ai_movement_count = 0;
          net_heartbeat_send(net_session_block.net_slot_civs[0]);
          dlg_pending_idle_callback = net_client_wait_ai_moves_idle;
          popup_game_with_sprite((const char *)&str_waitaimoves,(iptr)&popup_info_icon_sprite,0);
          while (((net_session_block.nm_human_movement_count == 0 && (net_session_block.nm_end_sequence_count == 0)) && (game_loop_active != '\0'))) {
            net_process_messages(&net_session_block,1,0);
            net_heartbeat_check(net_session_block.net_slot_civs[0]);
            status = net_check_lost_connections();
            if (status != 0) {
              seh_state = 0xffffffff;
              /* inlined unwind funclet 005abfdb */ { dialog_dtor(&dialog2); }
              ret = 0;
              goto return_ret;
            }
          }
          if ((net_session_block.nm_end_sequence_count == 0) && (game_loop_active != '\0')) {
            net_session_block.nm_human_movement_count = 0;
            if ((civ_saved_cursor_x_block.civ_intro_shown[human_civ[0]] == 0) && (civ_saved_cursor_x_block.autoplay_active == 0)) {
              civ_intro_popup(human_civ[0]);
              civ_saved_cursor_x_block.civ_intro_shown[human_civ[0]] = 1;
            }
            new_casualties = net_session_block.casualty_counts[human_civ[0]] -
                       net_session_block.casualty_reported_counts[human_civ[0]];
            if (civ_saved_cursor_x_block.autoplay_active == 0) {
              dlg_pending_idle_callback = turn_popup_timeout_idle;
              str = civ_leader_title(human_civ[0]);
              text_set_string_arg(0,(const char *)str);
              str = (iptr)(civ_leader_name(human_civ[0]));
              text_set_string_arg(1,(const char *)str);
              if (new_casualties < 1) {
                dialog_load_text_noflags(&dialog2,(const char *)&str_game_7,(const char *)&str_ourturntomove_4);
                dialog_add_sprite(&dialog2,(iptr)&popup_info_icon_sprite,0,0);
                dialog_set_image_zoom(&dialog2,0);
                input_flush_events();
                snd_play_effect(0x30,0,0,0);
                dlg_show_modal(&dialog2,0);
              }
              else {
                net_session_block.casualty_reported_counts[human_civ[0]] =
                     net_session_block.casualty_counts[human_civ[0]];
                text_set_number_arg(0,new_casualties);
                if (new_casualties == 1) {
                  dialog_load_text_noflags(&dialog2,(const char *)&str_game_7,(const char *)&str_casualty_5);
                }
                else {
                  dialog_load_text_noflags(&dialog2,(const char *)&str_game_7,(const char *)&str_casualties_5);
                }
                dialog_add_sprite(&dialog2,(iptr)&popup_info_icon_sprite,0,0);
                dialog_set_image_zoom(&dialog2,0);
                input_flush_events();
                snd_play_effect(0x30,0,0,0);
                dlg_show_modal(&dialog2,0);
                if (dialog2.result_extra != 0) {
                  casualty_timeline_show(human_civ[0]);
                }
              }
            }
            if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
              mp_turn_timer_start(1);
            }
            game_player_move_loop();
          }
          else {
            net_session_block.nm_end_sequence_count = 0;
          }
        }
        else {
          turn_ok = 0;
        }
      }
      else {
        map_views_block.saved_human_players = (uint)game.human_players;
        game.human_players = '\0';
        if ((turn_resume_civ == '\0') || (turn_resume_new_game != 0)) {
          civ_process_turn(human_civ[0]);
        }
        net_send_message(0x8c,0,human_civ[0],0,0,0,0,0,0,0);
        net_heartbeat_send(net_session_block.net_slot_civs[0]);
        dlg_pending_idle_callback = net_client_wait_production_idle;
        popup_game_with_sprite((const char *)&str_waitproduction_2,(iptr)&popup_info_icon_sprite,0);
        while (((net_session_block.nm_ai_movement_count == 0 && (net_session_block.nm_end_sequence_count == 0)) && (game_loop_active != '\0'))) {
          net_process_messages(&net_session_block,1,0);
          net_heartbeat_check(net_session_block.net_slot_civs[0]);
          status = net_check_lost_connections();
          if (status != 0) {
            seh_state = 0xffffffff;
            /* inlined unwind funclet 005abfdb */ { dialog_dtor(&dialog2); }
            ret = 0;
            goto return_ret;
          }
        }
        if ((net_session_block.nm_end_sequence_count == 0) && (game_loop_active != '\0')) {
          net_session_block.nm_ai_movement_count = 0;
          if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
            mp_turn_timer_start(1);
          }
          ai_process_civ_units();
          game.human_players = (*(u8 *)&map_views_block.saved_human_players);
          esc_state = GetAsyncKeyState(0x1b);
          if (((int)esc_state & 0x8001U) != 0) {
            civ_saved_cursor_x_block.autoplay_turns_left = 1;
          }
        }
        else {
          turn_ok = 0;
        }
      }
    }
    else {
      turn_ok = 0;
    }
    mp_turn_timer_stop(1);
    status_redraw(1);
    ui_pump_messages();
    if (game_over_reason != 0) {
      game_loop_active = '\0';
    }
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(0x8f,0,human_civ[0],0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    civ_saved_cursor_x_block.saved_turn_civ = '\0';
    turn_resume_civ = '\0';
    civ_saved_cursor_x_block.saved_resume_new_game = 0;
    turn_resume_new_game = 0;
    is_first_turn = 0;
    resume_civ = '\0';
    if (game_loop_active == '\0') {
      net_send_message(0x2d,0,human_civ[0],0,0,0,0,0,0,0);
      XD_FlushSendBuffer(60000);
      net_session_shutdown(&net_session_block,1);
      if (game_over_reason != 0) {
        game_handle_game_over();
      }
      seh_state = 0xffffffff;
      /* inlined unwind funclet 005abfdb */ { dialog_dtor(&dialog2); }
      ret = 0;
return_ret:
      return ret;
    }
  } while( true );
}

/* 005b9ec6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_batch_begin(void)

{
  if (2 < game.multiplayer) {
    net_session_block.net_tile_sync_enabled = 0;
    net_session_block.net_tile_batch_active = 1;
    _memset(((i8 *)&net_batch_buffer),0,0x400);
    net_batch_buffer[0] = 0;
    net_batch_used_dwords = 1;
  }
  return;
}

/* 005b9f1c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_batch_flush(void)

{
  if (2 < game.multiplayer) {
    net_session_block.net_tile_sync_enabled = 1;
    net_session_block.net_tile_batch_active = 0;
    if (net_session_block.net_host_flag == '\0') {
      if (1 < net_batch_used_dwords) {
        net_send_message(0x59,0,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(5000);
        _memset(((i8 *)&net_batch_buffer),0,0x400);
        net_batch_buffer[0] = 0;
        net_batch_used_dwords = 1;
      }
    }
    else {
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
  }
  return;
}

/* 005b9fde auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
net_batch_add(int record_type,i32 x,i32 y,i32 arg1,iptr arg2,
            i32 arg3)

{
  if (net_session_block.net_host_flag != '\0') {
    err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_gnetmgr_bserver_3,(iptr)&str_d_ss_franklinton_map_cpp,0x3de);
  }
  if (0x100 - net_batch_used_dwords < net_batch_record_sizes[record_type] + 1) {
    net_send_message(0x59,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    _memset(((i8 *)&net_batch_buffer),0,0x400);
    net_batch_buffer[0] = 0;
    net_batch_used_dwords = 1;
  }
  net_batch_buffer[0]++;
  net_batch_buffer[net_batch_used_dwords] = record_type;
  net_batch_used_dwords++;
  switch(record_type) {
  case 0:
  case 2:
    net_batch_buffer[net_batch_used_dwords] = x;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = y;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = arg1;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = arg2;
    net_batch_used_dwords++;
    break;
  case 1:
  case 3:
  case 4:
  case 5:
  case 6:
    net_batch_buffer[net_batch_used_dwords] = x;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = y;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = arg1;
    net_batch_used_dwords++;
    break;
  case 7:
    net_batch_buffer[net_batch_used_dwords] = x;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = y;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = arg1;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = arg2;
    net_batch_used_dwords++;
    net_batch_buffer[net_batch_used_dwords] = arg3;
    net_batch_used_dwords++;
  }
  return;
}

/* 005ba206 auto */
void __cdecl net_apply_tile_updates(iptr batch)

{
  int record_type;
  i8 saved_sync;
  iptr count_offset;
  int records_left;
  iptr arg_idx;
  iptr args[5];
  int pos;
  
  pos = 0;
  if (2 < game.multiplayer) {
    if (net_session_block.net_host_flag == '\0') {
      err_report((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx),7,(iptr)&str_gnetmgr_bserver_4,(iptr)&str_d_ss_franklinton_map_cpp_2,0x416);
    }
    saved_sync = net_session_block.net_tile_sync_enabled;
    net_session_block.net_tile_sync_enabled = 0;
    count_offset = pos * 4;
    pos++;
    for (records_left = *(int *)(batch + count_offset); 0 < records_left; records_left--) {
      record_type = *(int *)(batch + pos * 4);
      for (arg_idx = 0; pos = pos + 1, arg_idx < net_batch_record_sizes[record_type] - 1;
          arg_idx = arg_idx + 1) {
        args[arg_idx] = *(iptr *)(batch + pos * 4);
      }
      switch(record_type) {
      case 0:
        tile_set_improvements(args[0],args[1],args[2],args[3],1);
        break;
      case 1:
        tile_set_terrain(args[0],args[1],args[2],1);
        break;
      case 2:
        tile_set_visible(args[0],args[1],args[2],args[3],1);
        break;
      case 3:
        tile_set_fertility(args[0],args[1],args[2],1);
        break;
      case 4:
        tile_set_owner(args[0],args[1],args[2],1);
        break;
      case 5:
        tile_set_landmass(args[0],args[1],args[2],1);
        break;
      case 6:
        tile_set_radius_civ(args[0],args[1],args[2],1);
        break;
      case 7:
        known_set(args[0],args[1],args[2],args[3],args[4],1);
      }
    }
    net_session_block.net_tile_sync_enabled = saved_sync;
    net_send_state_diff(0xff,2,0,0,0);
  }
  return;
}
