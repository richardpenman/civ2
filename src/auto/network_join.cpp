/* The game, multiplayer: finding, joining and hosting a game (the game list, the join dialog, the lobby): functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00421f70 auto */
NetGamelistView *NetGamelistView::view_construct()

{
  next_game_id = 1;
  first_option_value = 0;
  refresh_timer = 0;
  return this;
}

/* Part of net_session_init (00421fcd): where it goes on after the scenario
   (net_join_load_scenario). */
enum NetJoinScenario { JOIN_GO_ON, JOIN_FAIL, JOIN_SCENARIO_CANCELLED };

/* Part of net_session_init (00421fcd): Internet: the host's address, typed or
   the lobby's, and the connection to it, tried until it answers. 0: cancelled
   or failed (the caller fails). */
static int net_join_by_address(NetGamelistView *self,int conn_type,int retries,Dialog &dlg)
{
  int result;
  int popup;
  size_t len1;
  size_t len2;
  char address[260];
  
  /* Internet: the host's address typed (or the lobby's) */
  for (;;) {
    retries++;
    if (net.lobby_launch_mode != -1) {
      crt_strcpy(self->server_address,((i8 *)&net.lobby_launch_info));
      crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&net.lobby_launch_info));
    }
    else {
      dlg.load_text_section((const char *)&str_game_7,"IPOFGAME",0x1f,(const char*)((iptr)""),0,0,0,1);
      result = dlg.run_modal(address,0);
      if (result == -1) {
        self->server_address[0] = '\0';
        return 0;
      }
      if (_strlen(address) == 0) continue;
      crt_strcpy(self->server_address,address);
      crt_strcpy(((i8 *)&net.net_server_address),address);
    }
    result = XD_OpenConnection(self->server_address,net.timeout_seconds * 1000);
    if (result < 0) {
      dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
      crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
      popup_game_with_sprite("SERVERCONNECTTIME",(Sprite *)&popup_info_icon_sprite,0);
      if ((conn_type == 2) || (conn_type == 3)) return 0;
      continue;
    }
    if ((conn_type == 0) && (net.lobby_launch_mode != -1)) {
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
    net.wait_start_ticks = timer_ticks();
    dlg_style.pending_idle_callback = net_join_wait_idle_cb;
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(Sprite *)&popup_info_icon_sprite,0);
    if (popup == -1) return 0;
    if (net.server_conn_id != -1) break;
    /* no id from the host: the lobby's game is tried again (3 times) */
    if ((conn_type == 0) && (net.lobby_launch_mode != -1) && (retries <= 3)) {
      XD_CloseConnection();
      continue;
    }
    popup_game_with_sprite("SERVERCONNECTFAIL",(Sprite *)&popup_info_icon_sprite,0);
    return 0;
  }
  /* the host's version */
  diffeng_register_blocks();
  net_reset_msg_counts();
  net_send_message(NM_SERVER_QUERY,0,0,0,0,0,0,0,0,0);
  dlg_style.pending_idle_callback = net_join_wait_answer_idle;
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(Sprite *)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if ((net.nm_server_reject_count != 0) || (net.nm_server_canceled_count != 0)) {
    text_set_string_arg(1,self->server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    if (net.nm_server_reject_count == 0) {
      popup_game_with_sprite("SERVERQUIT",(Sprite *)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("REJECTEDLAN",(Sprite *)&popup_info_icon_sprite,0);
    }
    return 0;
  }
  len1 = _strlen(((i8 *)&net.net_server_version));
  len2 = _strlen(str_ptr_5_4_0f_multiplayer_26_march_99);
  if ((len1 != len2) || (_strcmp(((i8 *)&net.net_server_version),str_ptr_5_4_0f_multiplayer_26_march_99) != 0)) {
    crt_strcpy(((i8 *)&text_args.strings),((i8 *)&net.net_server_version));
    crt_strcpy(((i8 *)&text_args.string1),str_ptr_5_4_0f_multiplayer_26_march_99);
    popup_game_with_sprite("WRONGVERSION",(Sprite *)&popup_info_icon_sprite,0);
    return 0;
  }
  return 1;
}

/* Part of net_session_init (00421fcd): LAN: the games the hosts announce, in
   a list until the player picks one and the connection to it opens. 0:
   cancelled or failed (the caller fails). */
static int net_join_from_list(NetGamelistView *self,int conn_type)
{
  int result;
  NetGameInfo *found;
  int civs_count;
  int players_count;
  uint mask;
  int bit;
  size_t len1;
  size_t len2;
  
  net.free_game_list();
  self->list_dialog = game_new<Dialog>(0x4000);
  if (self->list_dialog == 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)((iptr)"pJoinPopup != NULL"),(const char*)((iptr)"D:\\Ss\\Franklinton\\startup joinbox.cpp"),0xdc);
  }
  self->saved_button_label = dlg_style.button_labels[0];
  str_clear(self->button_label);
  str_append_label(self->button_label,0x126);
  dlg_style.button_labels[0] = (i8 *)self->button_label;
  self->list_dialog->Dialog::reset(0x1081801);
  self->list_dialog->set_width(0x276);
  self->list_dialog->set_height(0x153);
  text_clear();
  text_append_label(0x287);
  self->list_dialog->set_title((const char *)&textbuf.buf);
  self->list_dialog->set_current_pane(0);
  self->list_dialog->set_listbox(0xb,0xd2,0);
  self->list_dialog->set_select_change_fn((DialogSelectFn)net_gamelist_show_game_info);
  self->add_info_lines();
  text_clear();
  text_append_label(0x286);
  self->add_entry((const char *)&textbuf.buf,0);
  self->list_dialog->set_default_choice(0);
  self->update_details();
  self->list_dialog->render();
  self->list_dialog->surf_wnd->window.set_modal_idle_callback(net_gamelist_poll_idle);
  for (;;) {
    net.listing_games = 1;
    self->start_timer((TimerFn)net_gamelist_poll_tick);
    result = self->list_dialog->show_modal(0);
    net.listing_games = 0;
    self->kill_timer();
    if (result < 0) {
      dlg_style.button_labels[0] = self->saved_button_label;
      self->clear();
      if (self->list_dialog != 0) {
        self->list_dialog->scalar_destruct(1);
      }
      self->list_dialog = 0;
      self->server_address[0] = '\0';
      return 0;
    }
    for (found = net.found_games; (found != 0 && (found->list_id != result)); found = found->next) {
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
    if ((players_count >= net.net_max_players) || (players_count >= civs_count) || (found->open_game == 0)) {
      if ((net.net_max_players <= players_count) || (civs_count <= players_count)) {
        popup_game_with_sprite("JOINEDMAX",(Sprite *)&popup_info_icon_sprite,0);
      }
      continue;
    }
    len1 = _strlen(found->version);
    len2 = _strlen(str_ptr_5_4_0f_multiplayer_26_march_99);
    if ((len1 != len2) || (_strcmp(found->version,str_ptr_5_4_0f_multiplayer_26_march_99) != 0)) {
      dlg_style.button_labels[0] = self->saved_button_label;
      crt_strcpy(((i8 *)&text_args.strings),found->version);
      crt_strcpy(((i8 *)&text_args.string1),str_ptr_5_4_0f_multiplayer_26_march_99);
      popup_game_with_sprite("WRONGVERSION",(Sprite *)&popup_info_icon_sprite,0);
      dlg_style.button_labels[0] = (i8 *)self->button_label;
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
      net.net_server_address[0] = 0;
    }
    else {
      crt_strcpy(((i8 *)&net.net_server_address),found->host_address);
    }
    crt_strcpy(((i8 *)&net.net_game_name),found->game_name);
    crt_strcpy(((i8 *)&net.local_player_name),((i8 *)&menu_prefs.net_player_name));
    crt_strcpy(((i8 *)&net.host_player_name),found->host_name);
    result = XD_OpenConnection(self->server_address,net.timeout_seconds * 1000);
    if (-1 < result) break;
    dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    popup_game_with_sprite("SERVERCONNECTFAIL",(Sprite *)&popup_info_icon_sprite,0);
  }
  self->list_dialog->close();
  dlg_style.button_labels[0] = self->saved_button_label;
  self->clear();
  if (self->list_dialog != 0) {
    self->list_dialog->scalar_destruct(1);
  }
  self->list_dialog = 0;
  wnd_flush_paint();
  return 1;
}

/* Part of net_session_init (00421fcd): the scenario's intro cancelled: out of
   the game (then the palette and the dialog destroyed, as the original did
   on its return). */
static void net_join_cancel_scenario(GamePalette &scen_palette)
{
  net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
  net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
  XD_FlushSendBuffer(180000);
  scen_palette.destroy();
}

/* Part of net_session_init (00421fcd): a scenario: its directory, its intro.
   Returns where the caller goes on. */
static NetJoinScenario net_join_load_scenario(void)
{
  GamePalette scen_palette;
  int civ;
  int gov;
  int sex;
  int label;
  iptr unit;
  i32 year;
  char *tribe_label;
  i8 scen_file[260];
  char subdir_path[260];
  bool paths_fit;
  
  /* (BF-222) the host's folder and file names, after our directory, must fit
     the 260-byte paths (subdir_path, scen_file); a longer one is a folder we
     cannot have */
  paths_fit = _strlen(((i8 *)&net_map_lock_slots_block.scenario_dir_path)) + _strlen(((i8 *)&net.loaded_file_subdir)) +
              _strlen(((i8 *)&net.loaded_file_name)) + 6 < 260;
  if (paths_fit) {
    crt_strcpy(subdir_path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(subdir_path,((i8 *)&net.loaded_file_subdir));
  }
  if (!paths_fit || (__chdir(subdir_path) < 0)) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    text_set_string_arg(0,((i8 *)&game_dirs.home));
    text_set_string_arg(1,((i8 *)&net.loaded_file_subdir));
    popup_game_with_sprite("DATAPATHFAIL",(Sprite *)&popup_info_icon_sprite,0);
    return JOIN_FAIL;
  }
  crt_strcat(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&net.loaded_file_subdir));
  rules_load(0);
  if ((net.loaded_file_kind == 2) || ((game.game_flags & GAME_SCENARIO) != 0)) {
    Dialog scen_dlg(0x4000);
    scen_palette.init();
    for (civ = 1; civ < 8; civ++) {
      if ((civs[civ].flags & CIVF_FEMALE_LEADER) == 0) {
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
    if (cd_music.available == 0) {
      snd_cd_stop();
      snd_play_effect(SND_MENULOOP,0,1,0);
    }
    ui_flush_input();
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    if ((file_exists("TITLE.GIF") != 0) &&
       (((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_file("TITLE.GIF",10,0xc0,&scen_palette) != 0)) {
      main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
      main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
      main_frame_wnd_block.main_frame_wnd.maximize();
      wnd_flush_paint();
      input_flush_events();
    }
    __chdir(((i8 *)&game_dirs.home));
    crt_strcpy(((i8 *)&textbuf.buf),((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(((i8 *)&textbuf.buf),"\\");
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&net.loaded_file_name));
    path_set_extension(scen_file,(char *)(&textbuf.buf),lang_file_ext);
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
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    dialog_state.timeout_start = timer_ticks();
    scen_dlg.init_default();
    if (scen_dlg.load_text((const char *)scen_file,"SCENARIO",0x4008001) == 0) {
      unit = (iptr)game.active_unit;
      if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
         ((int)units[unit].owner == (int)scenario.objective_civ)) {
        scen_dlg.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c)),0,0);
      }
      if (scen_dlg.show_modal(0) < 0) {
        snd_play_effect(SND_MENUOK,0,0,0);
        ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,&scen_palette);
        main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
        main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
        wnd_flush_paint();
        net_join_cancel_scenario(scen_palette);
        return JOIN_SCENARIO_CANCELLED;
      }
    }
    else if (popup_game_flags("SCENARIOLOADED",1) < 0) {
      net_join_cancel_scenario(scen_palette);
      return JOIN_SCENARIO_CANCELLED;
    }
    scen_palette.destroy();
  }
  return JOIN_GO_ON;
}

/* 00421fad auto */
void NetGamelistView::view_destruct()

{
  net.free_game_list();
  return;
}

/* (net_session_init) After a scenario's intro: the set-up backdrop back. */
static void net_join_restore_backdrop(GamePalette *palette)
{
  if (net.loaded_file_kind == 2) {
    snd_play_effect(SND_MENUOK,0,0,0);
    ((DibSurface *)((i8 *)&minimap_window_block.setup_backdrop_surface))->load_gif_resource(0x5a,10,0xc0,palette);
    main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&minimap_window_block.setup_backdrop_surface);
    main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
    wnd_flush_paint();
  }
}

/* 00421fcd (by hand: the decompiler lost the lobby retry count and the stack dialog) */
/* Joining a network game: the game list (LAN) or the host's address
   (Internet; conn_type 0), the connection, the host's answers, the scenario,
   the player's profile and tribe, then the wait for the game.  1: joined. */
i32 NetGamelistView::net_session_init(int conn_type)

{
  int result;
  int popup;
  int retries;
  GamePalette palette;

  Dialog dlg(0x4000);
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  retries = 0;
  net_reset_msg_counts();
  if ((conn_type == 2) || (conn_type == 3)) {
    /* (modem, serial: no address) */
    server_address[0] = '\0';
    result = XD_OpenConnection(server_address,net.timeout_seconds * 1000);
    if (result < 0) {
      if ((net.net_protocol != 2) && (net.net_protocol != 3)) {
        dbg_logf("ConnectToGame: Open connection failed! Err: %d.",result);
        crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
        popup_game_with_sprite("SERVERCONNECTTIME",(Sprite *)&popup_info_icon_sprite,0);
      }
      return 0;
    }
  }
  else if (conn_type == 0) {
    if (net_join_by_address(this,conn_type,retries,dlg) == 0) return 0;
  }
  else {
  /* LAN: the games the hosts announce, in a list */
    if (net_join_from_list(this,conn_type) == 0) return 0;
  }

  /* the host's id */
  dlg_style.pending_idle_callback = net_join_wait_server_idle;
  net.wait_start_ticks = timer_ticks();
  if (conn_type != 0) {
    net_reset_msg_counts();
  }
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGFORSERVER",0x2000001,(Sprite *)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if ((net.nm_server_reject_count != 0) || (net.nm_server_canceled_count != 0) ||
      (net.nm_server_full_count != 0)) {
    text_set_string_arg(1,server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    if (net.nm_server_reject_count != 0) {
      popup_game_with_sprite("REJECTEDLAN",(Sprite *)&popup_info_icon_sprite,0);
    }
    else if (net.nm_server_canceled_count != 0) {
      popup_game_with_sprite("SERVERQUIT",(Sprite *)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("JOINEDMAX",(Sprite *)&popup_info_icon_sprite,0);
    }
    return 0;
  }
  if (popup == -2) {
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    crt_strcpy(((i8 *)&text_args.string1),((i8 *)&net.net_game_name));
    popup_game_with_sprite("SERVERCONNECTTIME",(Sprite *)&popup_info_icon_sprite,0);
    return 0;
  }
  /* the game's state */
  diffeng_register_blocks();
  dlg_style.pending_idle_callback = net_join_wait_gamestate_idle;
  net.wait_start_ticks = timer_ticks();
  if (conn_type != 0) {
    net_reset_msg_counts();
  }
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITTOJOIN",0x2000001,(Sprite *)&popup_info_icon_sprite,0);
  if ((popup == -1) || (net.nm_server_canceled_count != 0)) {
    if (popup == -1) {
      net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    }
    else {
      text_set_string_arg(1,server_game_name);
      crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
      popup_game_with_sprite("SERVERQUIT",(Sprite *)&popup_info_icon_sprite,0);
    }
    XD_FlushSendBuffer(180000);
    return 0;
  }
  /* a scenario: its directory, its intro */
  if ((0 < net.loaded_file_kind) && (_strlen(((i8 *)&net.loaded_file_subdir)) != 0)) {
    switch (net_join_load_scenario()) {
    case JOIN_GO_ON:
      break;
    case JOIN_FAIL:
      return 0;
    case JOIN_SCENARIO_CANCELLED:
      /* the scenario's intro cancelled: out of the game (net_join_cancel_scenario) */
      return 0;
    }
  }

  /* the player: profile, tribe */
  passwords_refresh_flags();
  if (game_profile_popup(1) == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if (((saved_turn.resume_new_game == 0) && (saved_turn.civ == '\0')) &&
     (net.session_phase < 2)) {
    result = setup_choose_tribe(net.local_slot,3,0);
  }
  else {
    result = net_choose_civ_dialog(net.local_slot);
  }
  if (result != 0) {
    net_join_restore_backdrop(&palette);
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  /* in: wait for the player list, then the game */
  net_send_message(NM_PLAYER_INFO,0,0,0,0,0,0,0,0,0);
  diffeng_register_blocks();
  dlg_style.pending_idle_callback = net_join_wait_player_list_idle;
  net.wait_start_ticks = timer_ticks();
  net_reset_msg_counts();
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGONJOIN",0x2000001,(Sprite *)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_join_restore_backdrop(&palette);
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if ((net.nm_server_canceled_count != 0) || (net.nm_lost_server_on_join_count != 0)) {
    net_join_restore_backdrop(&palette);
    text_set_string_arg(1,server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    if (net.nm_server_canceled_count == 0) {
      popup_game_with_sprite("LOSTSERVER",(Sprite *)&popup_info_icon_sprite,0);
    }
    else {
      popup_game_with_sprite("SERVERQUIT",(Sprite *)&popup_info_icon_sprite,0);
    }
    return 0;
  }
  net.nm_start_civ_count = 0;
  dlg_style.pending_idle_callback = net_join_wait_start_civ_idle;
  net.wait_start_ticks = timer_ticks();
  crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
  popup = popup_game_sprite_flags("WAITINGONJOIN",0x2000001,(Sprite *)&popup_info_icon_sprite,0);
  if (popup == -1) {
    net_join_restore_backdrop(&palette);
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    net_send_message(NM_UNLOCK_NATION,0,0,human_civ[0],0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    return 0;
  }
  if (net.nm_server_canceled_count != 0) {
    net_join_restore_backdrop(&palette);
    text_set_string_arg(1,server_game_name);
    crt_strcpy(((i8 *)&text_args.strings),(*(char * *)&net.connection_title));
    popup_game_with_sprite("SERVERQUIT",(Sprite *)&popup_info_icon_sprite,0);
    return 0;
  }
  net_reset_msg_counts();
  text_close_file();
  snd_menu_end();
  return 1;
}

/* 0042415c auto */
void __cdecl net_gamelist_poll_idle(void)

{
  net.process_messages(1,0);
  return;
}

/* 0042417a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_server_idle(void)

{
  timer_ticks();
  net.process_messages(1,0);
  if ((((net.nm_player_list_count != 0) || (net.nm_server_canceled_count != 0)) || (net.nm_server_reject_count != 0)) || (net.nm_server_full_count != 0))
  {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
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
  elapsed -= net.wait_start_ticks;
  net.process_messages(1,0);
  if ((net.nm_join_gamestate_count == 0) && (net.nm_server_canceled_count == 0)) {
    if (2 < elapsed) {
      text = *(char **)((int)(*(Dialog * *)&net.dialog_stack_top)->line_list->next + 8);
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
      (*(Dialog * *)&net.dialog_stack_top)->render();
      net.wait_start_ticks = timer_ticks();
    }
  }
  else {
    (*(Dialog * *)&net.dialog_stack_top)->style_flags = (*(Dialog * *)&net.dialog_stack_top)->style_flags | 0x400;
    (*(Dialog * *)&net.dialog_stack_top)->surf_wnd->window.end_modal_loop();
  }
  return;
}

/* 0042433c auto */
void __cdecl net_join_wait_player_list_idle(void)

{
  net.process_messages(1,0);
  if (((net.nm_player_list_count != 0) || (net.nm_server_canceled_count != 0)) || (net.nm_lost_server_on_join_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0042439c auto */
void __cdecl net_join_wait_start_civ_idle(void)

{
  net.process_messages(1,0);
  if ((net.nm_start_civ_count != 0) || (net.nm_server_canceled_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 004243ef auto */
void __cdecl net_join_wait_answer_idle(void)

{
  net.process_messages(1,0);
  if (((net.nm_server_answer_count != 0) || (net.nm_server_canceled_count != 0)) || (net.nm_server_reject_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0042444f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_join_wait_idle_cb(void)

{
  int elapsed;
  
  elapsed = timer_ticks();
  elapsed -= net.wait_start_ticks;
  net.process_messages(1,0);
  if ((net.server_conn_id == -1) && (0x4d8 < elapsed)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  else if (net.server_conn_id != -1) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 004244e0 auto */
void NetGamelistView::restore_selection()

{
  int option_count;
  DialogOption *option;
  int top_index;
  int selected_index;
  iptr scroll_pos;
  
  option_count = list_dialog->count_group_options(0);
  ((Scrollbar *)list_dialog->list_vscroll[0])->set_range(0,option_count - 1);
  option = find_option((const char*)&selected_game_name,(const char*)&selected_host_name);
  list_dialog->selected_option = option;
  if (option_count < 0xb) {
    list_dialog->list_top_option[0] = first_option_value;
  }
  else {
    option = find_option((const char*)&top_game_name,(const char*)&top_host_name);
    list_dialog->list_top_option[0] = option;
  }
  if (list_dialog->pane_drawn[0] != 0) {
    top_index = list_dialog->option_group_index(list_dialog->list_top_option[0]);
    option_count = (int)list_dialog->list_rows[0];
    selected_index = list_dialog->option_group_index(list_dialog->selected_option);
    if (option_count + top_index <= selected_index) {
      option_count = list_dialog->option_group_index(list_dialog->selected_option);
      option = (list_dialog->option_at(
                         (option_count - (int)list_dialog->list_rows[0]) + 1));
      list_dialog->list_top_option[0] = option;
    }
    option_count = list_dialog->option_group_index(list_dialog->list_top_option[0]);
    top_index = list_dialog->option_group_index(list_dialog->selected_option);
    if (top_index < option_count) {
      list_dialog->list_top_option[0] = list_dialog->selected_option;
    }
  }
  scroll_pos = list_dialog->option_group_index(list_dialog->list_top_option[0]);
  ((Scrollbar *)list_dialog->list_vscroll[0])->set_pos(scroll_pos);
  return;
}

/* 00424688 auto */
DialogOption *NetGamelistView::find_option(const char *game_name,const char *host_name)

{
  int cmp;
  DialogOption *option;
  NetGameInfo *game2;
  
  option = first_option_value;
  game2 = net.found_games;
  while( true ) {
    if (game2 == 0) {
      return first_option_value;
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
void NetGamelistView::update_details()

{
  if (net.found_games == 0) {
    selected_game_name[0] = '\0';
    selected_host_name[0] = '\0';
    top_game_name[0] = '\0';
    top_host_name[0] = '\0';
  }
  else {
    get_entry_names(list_dialog->selected_option,selected_game_name,
                       selected_host_name);
    get_entry_names(list_dialog->list_top_option[0],top_game_name,
                       top_host_name);
  }
  return;
}

/* 004247b2 auto */
void NetGamelistView::get_entry_names(DialogOption *option,char *out_game_name,char *out_host_name)

{
  int remaining;
  NetGameInfo *game2;
  
  for (game2 = net.found_games;
      (game2 != 0 && (option->id != game2->list_id));
      game2 = game2->next) {
  }
  if (game2 == 0) {
    game2 = net.found_games;
    for (remaining = list_dialog->option_group_index(option);
        (game2->next != 0 && (remaining != 0)); remaining--) {
      game2 = game2->next;
    }
  }
  crt_strcpy(out_game_name,game2->game_name);
  crt_strcpy(out_host_name,game2->host_name);
  return;
}

/* 0042486f auto */
void NetGamelistView::add_info_lines()

{
  int line_y;
  DialogLine *line;
  int spacing;
  
  list_dialog->compute_layout();
  line_y = (int)list_dialog->title_height + (int)list_dialog->field_b8 + 2;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_14,0xe6,line_y,0x80,0));
  info_lines[0] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_15,0xe6,line_y,0x80,0));
  info_lines[1] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_16,0xe6,line_y,0x80,0));
  info_lines[2] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_17,0xe6,line_y,0x80,0));
  info_lines[3] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_18,0xe6,line_y,0x80,0));
  info_lines[4] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_19,0xe6,line_y,0x80,0));
  info_lines[5] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_20,0xe6,line_y,0x80,0));
  info_lines[6] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line_y += spacing;
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_21,0xe6,line_y,0x80,0));
  info_lines[7] = (u32)line->number;
  spacing = ((GameFont *)list_dialog->body_font)->line_spacing();
  line = (DialogLine *)((u32 *)list_dialog->add_line((const char *)&str_empty_22,0xe6,line_y + spacing,0x80,0));
  info_lines[8] = (u32)line->number;
  ((GameFont *)list_dialog->body_font)->line_spacing();
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
  
  found_game = net.found_games;
  if (net.found_games == 0) {
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[0],(const char *)&str_empty_23);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[1],(const char *)&str_empty_24);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[2],(const char *)&str_empty_25);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[3],(const char *)&str_empty_26);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[4],(const char *)&str_empty_27);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[5],(const char *)&str_empty_28);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[6],(const char *)&str_empty_29);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[7],(const char *)&str_empty_30);
    ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[8],(const char *)&str_empty_31);
  }
  else {
    for (; (found_game != 0 &&
           (found_game->list_id != net.gamelist_view->list_dialog->selected_option->id));
        found_game = found_game->next) {
    }
    if (found_game != 0) {
      str_clear((char *)text);
      str_append_label(text,0x26f);
      str_append_space(text);
      crt_strcat(text,found_game->game_name);
      str_append_space(text);
      str_append_lparen(text);
      str_append_label(text,(found_game->open_game == 0) + 0x277);
      str_append_rparen(text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[0],(const char *)text);
      str_clear((char *)text);
      str_append_label(text,0x270);
      str_append_space(text);
      crt_strcat(text,found_game->host_name);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[1],(const char *)text);
      str_clear((char *)text);
      text_len = _strlen(found_game->scenario_name);
      if (text_len == 0) {
        str_append_label(text,0x34c);
      }
      else {
        crt_strcpy(text,found_game->scenario_name);
      }
      text_len = _strlen(text);
      if (0x19 < text_len) {
        text[21] = 0;           /* a long scenario name cut */
      }
      str_append_space(text);
      str_append_lparen(text);
      game.game_flags = found_game->game_flags;
      game.first_launch_turn = found_game->first_launch_turn;
      scenario.year_increment[0] = found_game->year_increment;
      scenario.start_year = found_game->start_year;
      game.difficulty = found_game->difficulty;
      year = game_turn_to_year((int)found_game->turn);
      str_append_year(text,year);
      str_append_rparen(text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[2],(const char *)text);
      str_clear((char *)text);
      str_append_label(text,found_game->difficulty + 0x279);
      str_append_space(text);
      str_append_lparen(text);
      str_append_label(text,0x34e - (uint)(found_game->simultaneous == 0));
      str_append_rparen(text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[3],(const char *)text);
      str_clear((char *)text);
      str_append_label(text,0x36c);
      str_append_space(text);
      str_append_int(text,(int)found_game->map_width / 2);
      crt_strcat(text,((i8 *)&gamelist_map_size_sep_str));
      str_append_int(text,(int)found_game->map_height);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[4],(const char *)text);
      str_clear((char *)text);
      str_append_label(text,0x350 - (uint)(found_game->double_production == 0));
      crt_strcat(text,((i8 *)&gamelist_double_opts_sep_str));
      str_append_label(text,0x352 - (uint)(found_game->double_moves == 0));
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[5],(const char *)text);
      str_clear((char *)text);
      str_append_label(text,0x273);
      str_append_space(text);
      str_append_label(text,found_game->barbarian_activity + 0x27f);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[6],(const char *)text);
      str_clear((char *)text);
      str_append_label(text,0x276);
      str_append_space(text);
      if (found_game->turn_time_limit_ms == 0) {
        str_append_label(text,0x285);
      }
      else {
        seconds = found_game->turn_time_limit_ms / 1000;
        minutes = seconds / 0x3c;
        seconds = seconds % 0x3c;
        if (minutes < 10) {
          str_append_int(text,0);
        }
        str_append_int(text,minutes);
        crt_strcat(text,((i8 *)&gamelist_timer_colon_str));
        if (seconds < 10) {
          str_append_int(text,0);
        }
        str_append_int(text,seconds);
      }
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[7],(const char *)text);
      bit = 1;
      civ_count = 0;
      civ_bits = (uint)found_game->civs_alive;
      for (; civ_bits = (int)civ_bits >> 1, bit < 8; bit++) {
        if ((civ_bits & 1) != 0) {
          civ_count++;
        }
      }
      str_clear((char *)text);
      str_append_label(text,0x274);
      str_append_space(text);
      str_append_int(text,civ_count);
      crt_strcat(text,((i8 *)&gamelist_open_paren_str));
      str_append_label(text,0x275);
      str_append_space(text);
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
        str_append_label(text,0x353);
      }
      else {
        str_append_int(text,civ_count);
      }
      str_append_rparen(text);
      ((Dialog *)dialog)->set_item_text(net.gamelist_view->info_lines[8],(const char *)text);
    }
  }
  scroll_pos = net.gamelist_view->list_dialog->option_group_index(net.gamelist_view->list_dialog->list_top_option[0]);
  ((Scrollbar *)net.gamelist_view->list_dialog->list_vscroll[0])->set_pos(scroll_pos);
  net.gamelist_view->list_dialog->render();
  return 1;
}

/* 004253ef auto */
void NetGamelistView::clear()

{
  DialogOption *next;
  DialogOption *option;
  
  option = (DialogOption *)first_option_value;
  update_details();
  while (option != (DialogOption *)0x0) {
    next = option->next;
    operator_delete(option->text);
    operator_delete(option);
    option = next;
  }
  first_option_value = 0;
  list_dialog->option_count = 0;
  list_dialog->option_list = 0;
  list_dialog->selected_option = 0;
  return;
}

/* 004254a8 auto */
DialogOption *NetGamelistView::add_entry(const char *name2,uint id)

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
  for (option = first_option_value; option != 0; option = option->next) {
    last = option;
  }
  entry = (DialogOption *)(operator_new(0x18));
  entry->next = 0;
  entry->prev = last;
  if (last == 0) {
    first_option_value = entry;
    list_dialog->option_list = entry;
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
  entry_width = ((GameFont *)list_dialog->body_font)->text_width((const char *)entry->text);
  max_width = list_dialog->option_icon_width();
  entry_width = entry_width + max_width + (int)list_dialog->option_extra_width + 5;
  max_width = (int)list_dialog->option_width;
  if (max_width <= entry_width) {
    max_width = entry_width;
  }
  list_dialog->option_width = max_width;
  count_ptr = (int *)&list_dialog->option_count;
  *count_ptr = *count_ptr + 1;
  return entry;
}

/* 00425607 auto */
void NetGamelistView::start_timer(TimerFn callback)

{
  u32 timer;
  
  if (refresh_timer != 0) {
    kill_timer();
  }
  timer = game_timer_create(callback,0x32,1);
  refresh_timer = timer;
  return;
}

/* 00425650 auto */
void NetGamelistView::kill_timer()

{
  if (refresh_timer != 0) {
    game_timer_destroy(refresh_timer);
    refresh_timer = 0;
  }
  return;
}

/* 00425695 auto */
void __cdecl net_gamelist_poll_tick(void)

{
  i32 timer;
  
  net_send_message(NM_GET_SERVERS,0,0,0,0,0,0,0,0,0);
  net_gamelist_expire_stale();
  if (net.gamelist_view->refresh_timer != 0) {
    game_timer_destroy(net.gamelist_view->refresh_timer);
  }
  timer = game_timer_create((TimerFn)net_gamelist_poll_tick,0xfa,1);
  net.gamelist_view->refresh_timer = timer;
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
  game2 = net.found_games;
  now = timer_ticks();
  while (game2 != 0) {
    if (net.timeout_seconds * 0x3c < now - game2->last_seen) {
      if (net.found_games == game2) {
        net.found_games = game2->next;
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
  
  if (net.gamelist_view != 0) {
    net.gamelist_view->clear();
    if (net.found_games == 0) {
      text_clear();
      text_append_label(0x286);
      net.gamelist_view->add_entry((const char *)&textbuf.buf,0);
      net.gamelist_view->update_details();
    }
    else {
      for (game2 = net.found_games; game2 != 0; game2 = game2->next) {
        if (game2->list_id == 0) {
          game2->list_id = net.gamelist_view->next_game_id;
          net.gamelist_view->next_game_id = net.gamelist_view->next_game_id + 1;
        }
        entry = (net.gamelist_view->add_entry(game2->game_name,
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
        if (((net.net_max_players <= human_count) || (total_civs <= human_count)) ||
           (game2->open_game == 0)) {
          entry->flags = entry->flags | 1;
        }
      }
    }
    net.gamelist_view->restore_selection();
    net_gamelist_show_game_info((void *)net.gamelist_view->list_dialog,0);
  }
  return;
}

/* 00430822 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_meeting_wait_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  now = timer_ticks();
  if (((net.timeout_seconds * 0x3c <= now - civ_rating_title_font_block.mp_meeting_wait_start_i32) || (advisor_window_block.mp_meeting_reply != -1)) ||
     (net.nm_parley_cancel_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
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
  int choice;
  
  Dialog dlg(0x4000);
  if ((net.lobby_launch_mode == -1) || (net.lobby_launch_mode == 1)) {
    proto_addr = XD_GetCurrentProtoAddr(net.adapter_index,net.net_protocol);
    if (proto_addr == (char *)0x0) {
      do {
        dlg.load_text_section((const char *)&str_game_7,(const char *)&str_ipaddress,0x1f,(const char*)&menu_prefs.net_ip_address,0,0,0,1);
        choice = dlg.run_modal(input,0);
        if (choice == -1) {
          result = 0;
          return result;
        }
        input_len = _strlen(input);
      } while (input_len == 0);
      crt_strcpy(((i8 *)&menu_prefs.net_ip_address),input);
    }
    else {
      proto_addr = XD_GetCurrentProtoAddr(net.adapter_index,net.net_protocol);
      crt_strcpy(((i8 *)&menu_prefs.net_ip_address),proto_addr);
    }
  }
  else {
    crt_strcpy(((i8 *)&menu_prefs.net_ip_address),((i8 *)&net.lobby_launch_info));
  }
  result = 1;
  return result;
}

/* 0044542c auto */
i32 __cdecl net_prompt_player_name(void)

{
  size_t input_len;
  i32 result;
  char input[260];
  int choice;
  
  Dialog dlg(0x4000);
  if (net.lobby_launch_mode == -1) {
    do {
      dlg.load_text_section((const char *)&str_game_7,(const char *)&str_netname,0x1f,(const char*)&menu_prefs.net_player_name,0,0,0,1);
      choice = dlg.run_modal(input,0);
      if (choice == -1) {
        result = 0;
        return result;
      }
      input_len = _strlen(input);
    } while (input_len == 0);
    crt_strcpy(((i8 *)&menu_prefs.net_player_name),input);
    prefs_save_civ2dat();
  }
  else {
    _memset(((i8 *)&menu_prefs.net_player_name),0,0x20);
    _strncpy(((i8 *)&menu_prefs.net_player_name),((i8 *)&net.lobby_short_name),0x1f);
  }
  result = 1;
  return result;
}

/* 00445592 auto */
i32 __cdecl net_prompt_game_name(void)

{
  size_t input_len;
  i32 result;
  char input[260];
  int choice;
  
  Dialog dlg(0x4000);
  if (net.lobby_launch_mode == -1) {
    do {
      dlg.load_text_section((const char *)&str_game_7,(const char *)&str_gamename,0x1f,(const char*)&menu_prefs.net_game_name,0,0,0,1);
      choice = dlg.run_modal(input,0);
      if (choice == -1) {
        result = 0;
        return result;
      }
      input_len = _strlen(input);
    } while (input_len == 0);
    crt_strcpy(((i8 *)&menu_prefs.net_game_name),input);
    crt_strcpy(((i8 *)&net.net_game_name),input);
    prefs_save_civ2dat();
  }
  else {
    _memset(((i8 *)&menu_prefs.net_game_name),0,0x20);
    _strncpy(((i8 *)&menu_prefs.net_game_name),((i8 *)&net.lobby_session_name),0x1f);
  }
  result = 1;
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
  DialogOption *option;
  iptr option_civs[8];
  int wait_start;
  
  Dialog dlg(0x4000);
  selected2 = 0;
  net.player_info_has_names = 0;
  net.player_info_has_titles = 0;
  do {
    dlg.Dialog::reset(0x801);
    dialog_prepend_title_label(&dlg,title_offset + 0x25e);
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
        text_append_int(civs[civ].num_cities);
        text_append_space();
        text_append_label(0xc5);
        text_append((const char *)&str_empty_65);
        text_append_int(civs[civ].num_military_units);
        text_append_space();
        text_append_label(0xc4);
        text_append((const char *)&str_empty_66);
        result = index;
        index++;
        option = (DialogOption *)((byte *)dlg.add_option((const char *)&textbuf.buf,result,0));
        option_civs[option->id] = civ;
      }
    }
    option_count2 = index;
    for (;;) {
      do {
        available_count = 0;
        for (index = 0; index < option_count2; index++) {
          option = (DialogOption *)((byte *)dlg.find_option(index));
          if (option != (DialogOption *)0x0) {
            (*((char *)&civ + 0)) = (byte)option_civs[option->id];
            if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
               ((1 << ((byte)civ & 0x1f) & (int)civ_saved_cursor_x_block.net_claimed_civs_mask) == 0)) {
              available_count++;
              dlg.set_option_disabled(option->id,0);
              if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
                ((RadioGroup *)dlg.radio_group)->enable_item(option->id);
              }
            }
            else {
              dlg.set_option_disabled(option->id,1);
              if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
                ((RadioGroup *)dlg.radio_group)->disable_item(option->id);
              }
            }
          }
        }
        if (available_count == 0) {
          popup_game((const char *)&str_joinedmax);
          dlg.close();
          ret = 1;
          return ret;
        }
        for (index = 0; index < option_count2; index++) {
          option = (DialogOption *)((byte *)dlg.find_option(selected2));
          if ((option != (DialogOption *)0x0) && ((option->flags_byte & 1) == 0)) {
            dlg.set_default_choice(option->id);
            if ((RadioGroup *)dlg.radio_group != (RadioGroup *)0x0) {
              ((RadioGroup *)dlg.radio_group)->set_selected(option->id);
            }
            break;
          }
          selected2++;
          if (selected2 == option_count2) {
            selected2 = 0;
          }
        }
        net_reset_msg_counts();
        net.wait_start_ticks = timer_ticks();
        dlg.style_flags = dlg.style_flags & 0xfffffbff;
        dlg.result_id = 0;
        dlg.result_extra = 0;
        dlg_style.pending_idle_callback = net_lock_wait_idle_cb;
        choice = dlg.show_modal(0);
        selected2 = ((RadioGroup *)dlg.radio_group)->get_selected();
        if (choice < 0) {
          ret = 1;
          return ret;
        }
      } while (net.nm_lock_data_count != 0);
      if (net.nm_server_canceled_count != 0) {
        popup_game_with_sprite(str_gamecanceled,(Sprite *)&popup_info_icon_sprite,0);
        ret = 1;
        return ret;
      }
      civ2 = option_civs[choice];
      net.player_info_civ = civ2;
      selected2 = choice;
      net_send_message(NM_LOCK_NATION,0,0,civ2,0,0,0,0,0,0);
      if (net.host_flag == '\0') {
        net_reset_msg_counts();
        wait_start = timer_ticks();
        while (((net.nm_nation_accept_count == 0 && (net.nm_nation_reject_count == 0)) &&
               (result = timer_ticks(), result - wait_start < net.timeout_seconds * 0x3c))) {
          net.process_messages(1,0);
        }
      }
      if ((net.nm_nation_accept_count == 0) && (net.nm_nation_reject_count == 0)) {
        dlg.close();
        popup_game_with_sprite(str_serverconnectfail,(Sprite *)&popup_info_icon_sprite,0);
        ret = 1;
        return ret;
      }
      if (net.nm_nation_reject_count == 0) break;
      popup_game_with_sprite(str_alreadychosen,(Sprite *)&popup_info_icon_sprite,0);
    }
    dlg.close();
    (*((char *)&civ + 0)) = (byte)civ2;
    game.human_players = game.human_players | (byte)(1 << ((byte)civ & 0x1f));
    game.player_civ = (byte)civ;
    game.turn_civ = (byte)civ;
    game.game_options = game.game_options & ~OPT_TUTORIAL;
    human_civ[0] = civ2;
    result = civ_check_password(civ2);
    if (result == 0) {
      game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
      net_send_message(NM_UNLOCK_NATION,0,0,civ2,0,0,0,0,0,0);
    }
    else {
      dlg.load_game_section((const char *)&str_gender_4,1);
      dialog_prepend_title_label(&dlg,title_offset + 0x25e);
      dlg.set_default_choice((uint)leaders[civs[game.player_civ].tribe].female);
      dlg.set_position(0xfffffc19,
                 -(((int)(caption_font_block.screen_height - 0x1e0 + (caption_font_block.screen_height - 0x1e0 >> 0x1f & 7U)) >> 3) + 1));
      dlg_style.pending_idle_callback = net_pump_idle_cb;
      result = dlg.show_modal(0);
      /* BF-113 (Peter Johnson's ledger): a choice past the two genders (a row
         added to GAME.TXT's @GENDER) is no choice (the original indexed the
         leader's names and titles with it) */
      if (1 < result) {
        result = -1;
      }
      net.player_info_gender = result;
      snd_play_effect(SND_MENUOK,0,0,0);
      if (result < 0) {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
        net_send_message(NM_UNLOCK_NATION,0,0,civ2,0,0,0,0,0,0);
      }
      else {
        is_female = (u8)result;
        leaders[civs[game.player_civ].tribe].female = is_female;
        civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
        if (result != 0) {
          civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
        }
        arg8 = 1;
        arg7 = 0;
        arg6 = 0;
        ret = 0;
        leader_str = (iptr)(civ_leader_name(game.player_civ));
        result = dlg.load_text_section((const char *)&str_game_7,(const char *)&str_name_4,0x17,(const char*)leader_str,(SurfaceWindow *)ret,(DibSurface *)arg6,(RECT *)arg7,arg8);
        if (result == 0) {
          dialog_prepend_title_label(&dlg,title_offset + 0x25e);
          dlg_style.pending_idle_callback = net_pump_idle_cb;
          result = dlg.run_modal(input,0);
          if (result == 0) {
            crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),input);
            crt_strcpy(((i8 *)&net.player_info_leader_name),input);
            net.player_info_tribe_name[0] = 0;
            net.player_info_adjective[0] = 0;
            ret = 0;
            return ret;
          }
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          net_send_message(NM_UNLOCK_NATION,0,0,civ2,0,0,0,0,0,0);
        }
      }
    }
  } while( true );
}

/* 00447170 auto */
void *NetGamelistView::view_scalar_destruct(uint flags)

{
  view_destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004471c0 auto */
LobbyList *LobbyList::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 0056e270 auto */
LobbyList *LobbyList::construct()

{
  u8 *slot_ptr;
  int i;
  
  next_id = 1;
  this->first_entry = 0;
  for (i = 0; i < 7; i++) {
    slot_ptr = (u8 *)&drop_counts[i];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  return this;
}

/* 0056e2c9 auto */
void LobbyList::destruct()

{
  net.free_game_list();
  return;
}

/* 0056e2e9 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 LobbyList::host_wait_players(i32 map_arg,int launch_mode)

{
  uint *flags_ptr;
  int result;
  DialogOption *entry;
  DialogOption *waiting_entry;
  i32 ret;
  i32 seh_frame;
  
  if ((launch_mode == 2) || (launch_mode == 3)) {
    net_send_message(NM_SERVER_INFO,0,0,0,0,0,0,0,0,0);
    dialog = 0;
    net.net_server_address[0] = 0;
    crt_strcpy(((i8 *)&net.net_game_name),((i8 *)&menu_prefs.net_game_name));
    crt_strcpy(((i8 *)&net.local_player_name),((i8 *)&menu_prefs.net_player_name));
    crt_strcpy(((i8 *)&net.host_player_name),((i8 *)&menu_prefs.net_player_name));
  }
  else {
    dialog = game_new<Dialog>(0x4000);
    if (dialog == 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_pplayerpopup_null,(const char*)&str_d_ss_franklinton_startup_playerbox_cpp,0x34);
    }
    saved_button_label = dlg_style.button_labels[0];
    str_clear(title);
    str_append_label(title,0x28b);
    dlg_style.button_labels[0] = (i8 *)title;
    dialog->Dialog::reset(0x81801);
    text_clear();
    text_append_label(0x288);
    if (launch_mode == 0) {
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&lobby_ip_sep_str));
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&menu_prefs.net_ip_address));
    }
    dialog->set_title((const char *)&textbuf.buf);
    text_clear();
    text_append_label(0x120);
    dialog->add_button((const char *)&textbuf.buf);
    add_waiting_entry();
    dialog->set_listbox(7,0x1b7,0);
    dialog->set_default_choice(next_id - 1);
    dialog->render();
    dialog->surf_wnd->window.set_modal_idle_callback(lobby_dialog_idle);
    if (net.lobby_launch_mode == 2) {
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
      net_send_message(NM_SERVER_INFO,0,0,0,0,0,0,0,0,0);
      flags_ptr = (uint *)&dialog->style_flags;
      *flags_ptr = *flags_ptr & 0xfffffbff;
      dialog->result_id = 0;
      dialog->result_extra = (i32)dialog->result_id;
      result = dialog->show_modal(0);
      if ((int)dialog->result_extra != 1) break;
      entry = find_entry_by_id(result);
      result = slot_for_entry_id(result);
      if ((entry != 0) && (waiting_entry = find_waiting_entry(), waiting_entry == 0)) {
        net_send_message(NM_SERVER_REJECT,net.net_slots[result].id,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(180000);
        XD_ServerCloseConnection((ushort)net.net_slots[result].id);
        dialog->selected_option = entry_neighbor(entry);
        remove_entry(entry);
        net.remove_player(net.net_slots[result].id);
        add_waiting_entry();
      }
    }
    if (result < 0) {
      net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
      dlg_style.button_labels[0] = saved_button_label;
      clear_player_list();
      if (dialog != (Dialog *)0x0) {
        dialog->scalar_destruct(1);
      }
      dialog = 0;
      XD_StopConnections();
      ret = 0;
      return ret;
    }
    crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&menu_prefs.net_ip_address));
    crt_strcpy(((i8 *)&net.net_game_name),((i8 *)&menu_prefs.net_game_name));
    crt_strcpy(((i8 *)&net.local_player_name),((i8 *)&menu_prefs.net_player_name));
    crt_strcpy(((i8 *)&net.host_player_name),((i8 *)&menu_prefs.net_player_name));
    dialog->close();
    dlg_style.button_labels[0] = saved_button_label;
    clear_player_list();
    if (dialog != (Dialog *)0x0) {
      dialog->scalar_destruct(1);
    }
    dialog = 0;
    wnd_flush_paint();
  }
  if (1 < net.net_player_count[0]) {
    net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
  }
  net.session_phase = 1;
  net_send_message(NM_GAME_GOING,0xff,1,0,0,0,0,0,0,0);
  crt_strcpy(((i8 *)&net.slot_player_names),((i8 *)&menu_prefs.net_player_name));
  diffeng_register_blocks();
  net_reset_msg_counts();
  net_send_message(NM_SERVER_PATH,0xff,0,0,0,0,0,0,0,0);
  net_send_message(NM_JOIN_GAMESTATE,0xff,0,0,0,0,0,0,0,0);
  net.local_slot = 0;
  result = game_profile_popup(1);
  if (result == -1) {
    net_send_message(NM_CLIENT_CANCELED_START,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(180000);
    ret = 0;
  }
  else {
    if ((saved_turn.resume_new_game == 0) && (saved_turn.civ == '\0')) {
      leaders_reset_custom_names();
      game.civs_alive = '\0';
      result = setup_choose_tribe(0,3,0);
      if (result != 0) {
        if (1 < net.net_player_count[0]) {
          net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
        }
        XD_StopConnections();
        ret = 0;
        return ret;
      }
    }
    else {
      result = net_choose_civ_dialog(0);
      if (result != 0) {
        if (1 < net.net_player_count[0]) {
          net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
        }
        XD_StopConnections();
        ret = 0;
        return ret;
      }
    }
    net.net_civ_slots[game.player_civ] = 0;
    net.slot_civs[0] = (int)game.player_civ;
    net.slot_active_flags[0] = 1;
    net.slot_host_flags[0] = 1;
    if (1 < net.net_player_count[0]) {
      crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
      dlg_style.pending_idle_callback = lobby_wait_civs_chosen_idle;
      net.wait_start_ticks = timer_ticks();
      result = popup_game_sprite_flags(str_waitingonjoin,0x2000001,(Sprite *)&popup_info_icon_sprite,0);
      if (result == -1) {
        net_send_message(NM_SERVER_CANCELED_START,0xff,0,0,0,0,0,0,0,0);
        XD_StopConnections();
        ret = 0;
        return ret;
      }
    }
    if (1 < net.net_player_count[0]) {
      net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
    }
    if ((turn_loop.resume_civ == '\0') && (turn_resume_new_game == 0)) {
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
      if (setup_dlg.accelerated_start != 0) {
        game_accelerated_start(setup_dlg.accelerated_start - 1);
      }
      if (turn_loop.resume_civ != '\0') {
        game.player_civ = turn_loop.resume_civ;
        human_civ[0] = (iptr)turn_loop.resume_civ;
      }
    }
    else {
      diffeng_register_blocks();
      diffeng_alloc_mirror();
    }
    text_close_file();
    if (1 < net.net_player_count[0]) {
      net_send_message(NM_MAP_STRUCT,0xff,0,0,0,0,0,0,0,0);
      net_send_state_diff(0xff,1,0,0,0);
      XD_FlushSendBuffer(5000);
      net_send_message(NM_EVENTSLOADED,0xff,scenario_events_loaded,unit_types_block.scenario_event_count,0,0,0,0,0,0);
      net_send_message(NM_START_CIVILIZATION,0xff,0,0,0,0,0,0,0,0);
      XD_FlushSendBuffer(180000);
    }
    net_reset_msg_counts();
    net.session_phase = 3;
    net_send_message(NM_GAME_GOING,0xff,3,0,0,0,0,0,0,0);
    ret = 1;
  }
  return ret;
}

/* 0056ec92 auto */
void __cdecl lobby_wait_civs_chosen_idle(void)

{
  int slot;
  int pending_count;
  
  net.process_messages(1,0);
  pending_count = 0;
  for (slot = 1; slot < 7; slot++) {
    if (((((i8 *)&net.slot_active_flags))[slot * 0x54] == 1) && ((((i32 *)&net.slot_civs))[slot * 0x15] == -1)) {
      pending_count++;
    }
  }
  if (pending_count == 0) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    net.session_phase = 2;
    net_send_message(NM_GAME_GOING,0xff,2,0,0,0,0,0,0,0);
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0056ed62 auto */
void __cdecl lobby_dialog_idle(void)

{
  net.process_messages(1,0);
  return;
}

/* 0056edd3 auto */
DialogOption *LobbyList::entry_neighbor(DialogOption *entry)

{
  DialogOption *neighbor;
  
  neighbor = entry;
  if (entry->next != 0) {
    neighbor = entry->next;
  }
  if ((neighbor == entry) && (entry->prev != 0)) {
    neighbor = entry->prev;
  }
  return neighbor;
}

/* 0056ee2c auto */
void LobbyList::clear_player_list()

{
  DialogOption *next;
  DialogOption *entry;
  
  entry = this->first_entry;
  while (entry != (DialogOption *)0x0) {
    next = entry->next;
    operator_delete(entry->text);
    operator_delete(entry);
    entry = next;
  }
  this->first_entry = 0;
  dialog->option_count = 0;
  dialog->option_list = 0;
  dialog->selected_option = 0;
  return;
}

/* 0056eed7 auto */
void LobbyList::remove_entry(DialogOption *entry)

{
  int *count_ptr;
  
  if (entry != (DialogOption *)0x0) {
    if (entry->prev == 0) {
      dialog->option_list = entry->next;
      this->first_entry = (DialogOption *)dialog->option_list;
    }
    else {
      entry->prev->next = entry->next;
    }
    if (entry->next != 0) {
      entry->next->prev = entry->prev;
    }
    operator_delete(entry->text);
    operator_delete(entry);
    count_ptr = (int *)&dialog->option_count;
    *count_ptr = *count_ptr - 1;
  }
  return;
}

/* 0056ef93 auto */
DialogOption *LobbyList::add_player_entry(const char *name2,uint entry_id)

{
  int *count_ptr;
  DialogOption *entry;
  size_t name_len;
  void *name_copy;
  int width;
  int max_width;
  DialogOption *tail2;
  DialogOption *node2;
  
  tail2 = 0;
  for (node2 = first_entry; node2 != 0; node2 = node2->next) {
    tail2 = node2;
  }
  entry = (DialogOption *)(operator_new(0x18));
  entry->next = 0;
  entry->prev = tail2;
  if (tail2 == 0) {
    first_entry = entry;
    dialog->option_list = entry;
  }
  else {
    tail2->next = entry;
  }
  entry->flags = 0;
  name_len = _strlen((char *)name2);
  name_copy = operator_new(name_len + 1);
  entry->text = (char *)name_copy;
  crt_strcpy(entry->text,name2);
  if (*(char *)name2 == '\0') {
    entry->flags = entry->flags | 1;
  }
  entry->id = entry_id;
  entry->group = 0;
  width = ((GameFont *)dialog->body_font)->text_width(entry->text);
  max_width = dialog->option_icon_width();
  width = width + max_width + (int)dialog->option_extra_width + 5;
  max_width = (int)dialog->option_width;
  if (max_width <= width) {
    max_width = width;
  }
  dialog->option_width = max_width;
  count_ptr = (int *)&dialog->option_count;
  *count_ptr = *count_ptr + 1;
  dialog->option_list = first_entry;
  dialog->list_top_option[0] = dialog->option_list;
  return entry;
}

/* 0056f113 auto */
void __cdecl lobby_refresh_player_list(void)

{
  int selected_id;
  int slot;
  DialogOption *entry;
  
  if (net.net_lobby_list != (LobbyList *)0x0) {
    selected_id = net.net_lobby_list->dialog->selected_option->id;
    net.net_lobby_list->clear_player_list();
    if (net.net_player_count[0] == 0) {
      net.net_lobby_list->add_waiting_entry();
    }
    else {
      for (slot = 1; slot < 7; slot++) {
        if ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') {
          if (net.net_slots[slot].lobby_id == 0) {
            net.net_slots[slot].lobby_id = net.net_lobby_list->next_id;
            net.net_lobby_list->next_id++;
          }
          net.net_lobby_list->add_player_entry((const char *)(((i8 *)&net.slot_player_names) + slot * 0x54),
                     net.net_slots[slot].lobby_id);
        }
      }
      net.net_lobby_list->add_waiting_entry();
    }
    for (entry = net.net_lobby_list->first_entry; (entry != 0 && (entry->id != selected_id));
        entry = entry->next) {
    }
    if (entry == 0) {
      for (entry = net.net_lobby_list->first_entry;
          (entry->next != 0 && (entry->id < selected_id));
          entry = entry->next) {
      }
      if (entry == 0) {
        entry = net.net_lobby_list->first_entry;
      }
    }
    net.net_lobby_list->dialog->option_list = net.net_lobby_list->first_entry;
    net.net_lobby_list->dialog->list_top_option[0] = net.net_lobby_list->dialog->option_list;
    net.net_lobby_list->dialog->set_default_choice(entry->id);
    net.net_lobby_list->dialog->render();
  }
  return;
}

/* 0056f301 auto */
void LobbyList::add_waiting_entry()

{
  i32 entry_id;
  
  if (first_entry == 0) {
    text_clear();
    text_append_label(0x28c);
    entry_id = net.net_lobby_list->next_id;
    net.net_lobby_list->next_id++;
    add_player_entry((const char *)&textbuf.buf,entry_id);
    net.net_lobby_list->dialog->set_default_choice(net.net_lobby_list->first_entry->id);
  }
  return;
}

/* 0056f372 auto */
DialogOption *LobbyList::find_waiting_entry()

{
  int cmp;
  DialogOption *entry;
  
  text_clear();
  text_append_label(0x28c);
  entry = first_entry;
  if (((entry != 0) && (entry->next == 0)) &&
     (cmp = _strcmp(((i8 *)&textbuf.buf),entry->text), cmp == 0)) {
    return entry;
  }
  return 0;
}

/* 0056f3e0 auto */
DialogOption *LobbyList::find_entry_by_id(int entry_id)

{
  DialogOption *entry;
  
  for (entry = first_entry; (entry != 0 && (entry->id != entry_id));
      entry = entry->next) {
  }
  return entry;
}

/* 0056f42b auto */
int LobbyList::slot_for_entry_id(int entry_id)

{
  int slot;
  
  for (slot = 0; (slot < 7 && (net.net_slots[slot].lobby_id != entry_id));
      slot = slot + 1) {
  }
  return slot;
}
