/* The game, multiplayer: network sessions, negotiation, lobby and chat: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"


/* 00421f40 auto */
i8 NetSession::is_host()

{
  return host_flag;
}

/* 00482337 auto */
void __cdecl net_game_xmit_wait_idle(void)

{
  net.process_messages(1,0);
  if (net.nm_continue_after_join_count != 0) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
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
  
  net.process_messages(1,0);
  if (((net.players_awaited != 0) &&
      (human_count = bits_count8((uint)(game.human_players == '\0')), human_count == 0)) &&
     (1 < net.net_player_count[0])) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  return;
}

/* 004824e3 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_host_quit_handoff(void)

{
  byte sign;
  int human_count;
  int slot;
  
  net.host_handoff_stage = 1;
  net_drop_flagged_players();
  game.human_players = net.pending_human_players | game.human_players;
  net.pending_human_players = 0;
  if (1 < net.net_player_count[0]) {
    game.human_players = game.human_players & ~(byte)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
    net.players_awaited = net.net_player_count[0] - 1;
    net_send_message(NM_SERVER_QUIT_REQUEST,0xff,0,0,0,0,0,0,0,0);
    net_drop_flagged_players();
    dlg_style.pending_idle_callback = net_server_quit_wait_idle;
    popup_game_sprite_flags(str_serverquitwait,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
    human_count = bits_count8((uint)(game.human_players == '\0'));
    if (human_count == 0) {
      net_send_message(NM_SERVER_QUIT,0xff,0,0,0,0,0,0,0,0);
      sign = (byte)(net.mp_turn_civ + 1 >> 0x1f);
      saved_turn.civ = (((byte)(net.mp_turn_civ + 1) ^ sign) - sign & 7 ^ sign) - sign;
      for (slot = 1; (slot < 7 && ((((i8 *)&net.slot_active_flags))[slot * 0x54] == '\0'));
          slot = slot + 1) {
      }
      if (slot < 7) {
        crt_memcpy(((i8 *)&net.player_slots),((i8 *)&net.player_slots) + slot * 0x54,0x54);
        net.slot_host_flags[0] = 1;
        net.player_slots[0] = 0;
        net.net_civ_slots[0] = net.net_civ_slots[slot];
        net.net_player_count[0]--;
        net.net_civ_slots[slot] = 0xffffffff;
        _memset(((i8 *)&net.player_slots) + slot * 0x54,0,0x54);
        net_send_state_diff(0xff,2,0,1,0);
        net_send_message(NM_START_CIVILIZATION,0xff,0,0,0,0,0,0,0,0);
        net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
    }
  }
  return;
}

/* 00482724 auto */
void __cdecl net_client_quit_wait_idle(void)

{
  net.process_messages(1,0);
  if (net.host_handoff_stage == 2) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0048276d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_start_civ_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  if ((net.nm_start_civ_count == 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net.timeout_seconds * 0x3c)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  return;
}

/* 004827d5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_player_list_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  if ((net.nm_player_list_count == 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net.timeout_seconds * 0x3c)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  return;
}

/* 0048283d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_quit_wait_players_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  if ((net.players_awaited != 0) &&
     (now = timer_ticks(), now - map_views_block.net_handoff_wait_ticks_i32 <= net.timeout_seconds * 0x3c)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
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
  
  net.nm_server_quit_count = 0;
  net.nm_start_civ_count = 0;
  net.nm_player_list_count = 0;
  net_send_message(NM_SERVER_QUIT_ACK,0,0,0,0,0,0,0,0,0);
  dlg_style.pending_idle_callback = net_client_quit_wait_idle;
  popup_game_sprite_flags(str_clientquitwait,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
  net.host_handoff_stage = 0;
  dlg_style.pending_idle_callback = net_quit_wait_start_civ_idle;
  map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
  popup_game_sprite_flags(str_clientquitwait_2,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
  if (net.nm_start_civ_count == 0) {
    dbg_log((const char *)&str_clienttransferserver_waiting_for_nm_star);
    crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
    popup_game_with_sprite(str_lanconnectfail_3,(Sprite *)&popup_info_icon_sprite,0);
    player_turn_active = 0;
    turn_loop.active = 0;
    result = 0;
  }
  else {
    turn_loop.resume_civ = saved_turn.civ;
    dlg_style.pending_idle_callback = net_quit_wait_player_list_idle;
    map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
    popup_game_sprite_flags(str_clientquitwait_3,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
    if (net.nm_player_list_count == 0) {
      dbg_log((const char *)&str_clienttransferserver_waiting_for_nm_play);
      crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
      popup_game_with_sprite(str_lanconnectfail_4,(Sprite *)&popup_info_icon_sprite,0);
      player_turn_active = 0;
      turn_loop.active = 0;
      result = 0;
    }
    else {
      crt_memcpy(&saved,((iptr *)&net.net_player_count),0x270);
      cmp = _strcmp(((i8 *)&menu_prefs.net_ip_address),((i8 *)&net.slot_addresses));
      if (cmp == 0) {
        dbg_log((const char *)&str_clienttransferserver_disconnecting_from);
        net.shutdown(0);
        transport_ok = net.init_transport(net.net_protocol,1);
        status = (int)transport_ok;
        if (status == 0) {
          dbg_log((const char *)&str_clienttransferserver_could_not_reconnect);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite(str_lanconnectfail_5,(Sprite *)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        dbg_log((const char *)&str_reconnected_to_network_as_server);
        crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&menu_prefs.net_ip_address));
        net.server_conn_id = 0;
        net.local_slot = 0;
        diffeng_scan.mirror_end = (*(int *)&diffeng_scan.mirror_size) + diffeng.mirror_buf;
        net.players_awaited = saved.slots[0] - 1;
        net.players_awaited_initial = net.players_awaited;
        if (net.players_awaited < 1) {
          slot_civ = saved.civ_ids[0];
          if ((uint)slot_civ < 8) {   /* (BF-162) a civ of the host's list */
            net.net_civ_slots[saved.civ_ids[0]] = 0;
          }
          net.slot_civs[0] = saved.civ_ids[0];
        }
        else {
          dlg_style.pending_idle_callback = net_quit_wait_players_idle;
          map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
          popup_game_sprite_flags(str_clientquitwait_4,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
          cmp = timer_ticks();
          if ((net.timeout_seconds * 0x3c < cmp - map_views_block.net_handoff_wait_ticks_i32) && (net.players_awaited != 0)) {
            crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
            if (net.players_awaited == net.players_awaited_initial) {
              popup_game_with_sprite(str_nooneshowed,(Sprite *)&popup_info_icon_sprite,0);
              turn_loop.active = 0;
              player_turn_active = 0;
              return 0;
            }
            popup_game_with_sprite(str_someshowed,(Sprite *)&popup_info_icon_sprite,0);
          }
          for (slot = 0; slot < 7; slot++) {
            if ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') {
              old_slot = 0;
              while ((old_slot < 7 &&
                     (cmp = _strcmp(saved.addresses + old_slot * 0x54,
                                      ((i8 *)&net.slot_addresses) + slot * 0x54), cmp != 0))) {
                old_slot++;
              }
              if (6 < old_slot) {
                ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_oldindex_max_net_players,(const char*)&str_d_ss_franklinton_netmgr_poll_cpp_5,0x8a7);
                /* (BF-162) the original stopped at its assertion: a player
                   not in the old list has no civ (civ_ids[7 * 0x15] is past
                   the copy); the transfer fails as when no one came */
                turn_loop.active = 0;
                player_turn_active = 0;
                return 0;
              }
              slot_civ = saved.civ_ids[old_slot * 0x15];
              if ((uint)slot_civ < 8) {   /* a civ of the host's list */
                net.net_civ_slots[slot_civ] = slot;
              }
              (((i32 *)&net.slot_civs))[slot * 0x15] = slot_civ;
            }
          }
          net.mp_simultaneous_moves = *(i8 *)&mp_options.simultaneous_setting;
          net_send_message(NM_MULTIOPTIONS_CHANGE,0xff,mp_doubles.production,mp_doubles.moves,(*(i8 *)&mp_options.simultaneous_setting),0,0,0,0
                             ,0);
          net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
          XD_FlushSendBuffer(60000);
        }
        _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots),-1,0xc0);
        net.session_phase = 3;
        net_reset_error_counts();
        net.nm_server_quit_count = 0;
        net.nm_start_civ_count = 0;
        net.nm_player_list_count = 0;
        if (game_over_reason != 2) {
          popup_game_with_sprite(str_hostchangeserver,(Sprite *)&popup_info_icon_sprite,0);
        }
      }
      else {
        dbg_logf_str((const char *)&str_disconnecting_from_server_s,(const char *)&net.net_server_address);
        status = XD_CloseConnection();
        if (status != 0) {
          dbg_logf_str_int((const char *)&str_clienttransferserver_disconnection_from,(const char *)&net.net_server_address,status);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite(str_lanconnectfail_6,(Sprite *)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        crt_strcpy(((i8 *)&net.net_server_address),((i8 *)&net.slot_addresses));
        map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
        do {
          cmp = timer_ticks();
        } while (cmp - map_views_block.net_handoff_wait_ticks_i32 < 0x3c);
        dbg_logf_str((const char *)&str_attempting_to_reconnect_to_new_server_s,(const char *)&net.net_server_address);
        status = XD_OpenConnection(((i8 *)&net.net_server_address),net.timeout_seconds * 1000);
        if (status < 0) {
          dbg_logf((const char *)&str_connecttogame_open_connection_failed_err,status);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite(str_serverconnectfail_2,(Sprite *)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        dbg_log((const char *)&str_connection_to_new_server_successful);
        net.nm_server_quit_count = 0;
        net.nm_start_civ_count = 0;
        dlg_style.pending_idle_callback = net_quit_wait_player_list_idle;
        map_views_block.net_handoff_wait_ticks_i32 = timer_ticks();
        popup_game_sprite_flags(str_clientquitwait_5,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
        if (net.nm_player_list_count == 0) {
          dbg_log((const char *)&str_clienttransferserver_client_timed_out_wa);
          crt_strcpy(((i8 *)&text_args.strings),(const char *)((const void *)net.connection_title));
          popup_game_with_sprite(str_lanconnectfail_7,(Sprite *)&popup_info_icon_sprite,0);
          turn_loop.active = 0;
          player_turn_active = 0;
          return 0;
        }
        crt_strcpy(((i8 *)&text_args.strings),((i8 *)&net.slot_player_names));
        net_reset_error_counts();
        net.nm_player_list_count = 0;
        popup_game_with_sprite(str_hostchangeclient,(Sprite *)&popup_info_icon_sprite,0);
      }
      result = 1;
    }
  }
  return result;
}

/* 0048bf51 auto */
void __cdecl net_client_hot_wait_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_start_civ_count != 0) || (net.host_handoff_stage != '\0')) || (net.nm_end_sequence_count != 0)) ||
     (((net.player_has_quit != '\0' || (turn_loop.active == '\0')) || (net.lost_conn_total != 0)))) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0048d9ad auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_heartbeat_send(int civ)

{
  if (net.drag_hold == '\0') {
    net.nm_heartbeat_ack_count = 0;
    net_send_message(NM_HEART_BEAT,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,0,
                       0,0,0,0);
  }
  else {
    net.nm_heartbeat_ack_count = 1;
  }
  if (((int)parley_wnd_block.closed == 1) && (net_popups.head == 0)) {
    diplo_meeting.mp_pending = 0;
    net.nm_parley_cancel_count = 0;
  }
  map_views_block.net_heartbeat_last_tick_i32 = timer_ticks();
  return;
}

/* 0048da51 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_heartbeat_check(int civ)

{
  int now;
  
  now = timer_ticks();
  if (0xe0f < now - map_views_block.net_heartbeat_last_tick_i32) {
    if (net.nm_heartbeat_ack_count == 0) {
      net.civ_turn_ended[civ] = net.civ_turn_ended[civ] + 1;
      if (net.host_flag == '\0') {
        net.lost_conn_total++;
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
  
  if (net.host_flag == '\0') {
    if (net.lost_conn_total != 0) {
      turn_loop.active = 0;
      dbg_log((const char *)&str_we_lost_connection_to_the_server_heartbe);
      net.shutdown(1);
      popup_game_with_sprite(str_lostserver,(Sprite *)&popup_info_icon_sprite,0);
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
          if (((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') &&
             ((((i32 *)&net.slot_civs))[slot * 0x15] == civ)) {
            no_player = false;
            break;
          }
        }
        if (no_player) {
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          if ((1 << ((byte)civ & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)civ & 0x1f));
            net_send_message(NM_LOCK_DATA,0xff,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net.nm_lock_nation_count++;
          }
          dropped_count++;
        }
      }
    }
    if (net.lost_conn_total != 0) {
      crt_memcpy(lost_counts,((i8 *)&net.lost_conn_by_player),0x1c);
      net_reset_error_counts();
      for (slot = 1; slot < 7; slot++) {
        if ((0 < lost_counts[slot]) && ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0')) {
          dropped_count++;
          slot_civ = (((i32 *)&net.slot_civs))[slot * 0x15];
          text_set_string_arg(0,(const char *)(((i8 *)&net.slot_player_names) + slot * 0x54));
          net_send_text_args(0x53,0xff,1,0,0,0);
          net.civ_drop_requested[slot_civ] = 0;
          (*((char *)&civ + 0)) = (byte)slot_civ;
          game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
          net.remove_player(net.net_slots[slot].id);
          if ((1 << ((byte)civ & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
            (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << ((byte)civ & 0x1f));
            net_send_message(NM_LOCK_DATA,0xff,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
            net.nm_lock_nation_count++;
          }
          popup_game_with_sprite(str_lostclient,(Sprite *)&popup_info_icon_sprite,0);
        }
      }
    }
    if (dropped_count != 0) {
      net.net_player_count[0] = 0;
      for (slot = 0; slot < 7; slot++) {
        if ((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') {
          net.net_player_count[0]++;
        }
      }
      net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
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
    if (((((i8 *)&net.slot_active_flags))[slot * 0x54] != '\0') &&
       (net.civ_drop_requested[(((i32 *)&net.slot_civs))[slot * 0x15]] != 0)) {
      dropped_count++;
      slot_civ = (((i32 *)&net.slot_civs))[slot * 0x15];
      net.civ_drop_requested[slot_civ] = 0;
      net.lost_conn_by_player[net.net_slots[slot].id] = 0;
      civ_bit = (byte)slot_civ;
      game.human_players = game.human_players & ~(byte)(1 << (civ_bit & 0x1f));
      net.remove_player(net.net_slots[slot].id);
      if ((1 << (civ_bit & 0x1f) & (int)(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask)) != 0) {
        (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) = (*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask) & ~(ushort)(1 << (civ_bit & 0x1f));
        net.nm_lock_nation_count++;
      }
    }
  }
  if (dropped_count != 0) {
    net_send_message(NM_LOCK_DATA,0xff,(short)(*(ushort *)&civ_saved_cursor_x_block.net_claimed_civs_mask),0,0,0,0,0,0,0);
    net_send_message(NM_PLAYER_LIST,0xff,0,0,0,0,0,0,0,0);
  }
  return dropped_count;
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
  else if (((char)force == '\0') && (net.net_player_count[0] == 1)) {
    bytes_changed = 0;
  }
  else {
    bytes_changed = 0;
    runs_left = 0;
    deadline = 0;
    start_ptr = (*(int * *)&diffeng_scan.scan_ptr);
    diffeng.send_count++;
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
    cur_ptr = (int *)((iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr + diffeng_scan.scan_offset);
    section_end = (int *)((i32)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].padded_size + (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr);
    if ((*(uint *)&diffeng_scan.scan_section) == 1) {
      game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng.mirror_buf);
      game_mirror->active_unit = game.active_unit;
      game_mirror->moving_unit = game.moving_unit;
      game_mirror->multiplayer = game.multiplayer;
      game_mirror->player_civ = game.player_civ;
      game_mirror->saved_current_civ = game.saved_current_civ;
      game_mirror->turn_civ = game.turn_civ;
      game_mirror->game_options = game.game_options;
      game_mirror->message_options = game.message_options;
    }
    diffeng_scan.packet = 0x66606660;
    if ((char)alt_msg == '\0') {
      diffeng_scan.packet_msg_type = 0x15;
    }
    else {
      diffeng_scan.packet_msg_type = 0x5c;
    }
    out_ptr = ((uint *)&diffeng_scan.packet_payload);
    record_count = 0;
    do {
      run_start = cur_ptr;
      mirror_start = (*(int * *)&diffeng_scan.scan_ptr);
      if (*cur_ptr == *(*(int * *)&diffeng_scan.scan_ptr)) {
        (*(int * *)&diffeng_scan.scan_ptr)++;
        if ((*(int * *)&diffeng_scan.scan_ptr) < (*(int * *)&diffeng_scan.mirror_end)) {
          cur_ptr++;
          if (section_end <= cur_ptr) {
            (*(uint *)&diffeng_scan.scan_section)++;
            if ((*(uint *)&diffeng_scan.scan_section) == 1) {
              game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng.mirror_buf);
              game_mirror->active_unit = game.active_unit;
              game_mirror->moving_unit = game.moving_unit;
              game_mirror->multiplayer = game.multiplayer;
              game_mirror->player_civ = game.player_civ;
              game_mirror->saved_current_civ = game.saved_current_civ;
              game_mirror->turn_civ = game.turn_civ;
              game_mirror->game_options = game.game_options;
              game_mirror->message_options = game.message_options;
            }
            diffeng_scan.scan_offset = 0;
            cur_ptr = (int *)(iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
            section_end = (int *)((i32)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].padded_size + (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr
                             );
          }
        }
        else {
          (*(int * *)&diffeng_scan.scan_ptr) = (int *)diffeng.mirror_buf;
          (*(uint *)&diffeng_scan.scan_section) = 0;
          diffeng_scan.scan_offset = 0;
          cur_ptr = (int *)diffeng_sections_block.diffeng_section_ptrs[0];
          section_end = (int *)(diffeng_sections_block.diffeng_section_padded_sizes[0] + diffeng_sections_block.diffeng_section_ptrs[0]);
        }
      }
      else {
        run_size = 0;
        do {
          (*(int * *)&diffeng_scan.scan_ptr)++;
          if ((*(int * *)&diffeng_scan.scan_ptr) < (*(int * *)&diffeng_scan.mirror_end)) {
            cur_ptr++;
            if (section_end <= cur_ptr) {
              run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
              run_size = (int)section_end - (int)run_start;
              run_section = (*(uint *)&diffeng_scan.scan_section);
              (*(uint *)&diffeng_scan.scan_section)++;
              if ((*(uint *)&diffeng_scan.scan_section) == 1) {
                game_mirror = (GameState *)(diffeng_game_section_block.diffeng_game_offset + diffeng.mirror_buf);
                game_mirror->active_unit = game.active_unit;
                game_mirror->moving_unit = game.moving_unit;
                game_mirror->multiplayer = game.multiplayer;
                game_mirror->player_civ = game.player_civ;
                game_mirror->saved_current_civ = game.saved_current_civ;
                game_mirror->turn_civ = game.turn_civ;
                game_mirror->game_options = game.game_options;
                game_mirror->message_options = game.message_options;
              }
              diffeng_scan.scan_offset = 0;
              cur_ptr = (int *)(iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
              section_end = (int *)((i32)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].padded_size +
                               (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr);
            }
          }
          else {
            run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
            run_size = (int)(*(int * *)&diffeng_scan.mirror_end) - (int)mirror_start;
            run_section = (*(uint *)&diffeng_scan.scan_section);
            (*(int * *)&diffeng_scan.scan_ptr) = (int *)diffeng.mirror_buf;
            (*(uint *)&diffeng_scan.scan_section) = 0;
            diffeng_scan.scan_offset = 0;
            cur_ptr = (int *)diffeng_sections_block.diffeng_section_ptrs[0];
            section_end = (int *)(diffeng_sections_block.diffeng_section_padded_sizes[0] + diffeng_sections_block.diffeng_section_ptrs[0]);
          }
        } while (((run_size == 0) &&
                 ((int)(*(int * *)&diffeng_scan.scan_ptr) + (-(iptr)&diffeng_scan.packet - (int)mirror_start) + (int)out_ptr < diffeng.packet_flush_size)) &&
                (*cur_ptr != *(*(int * *)&diffeng_scan.scan_ptr)));
        if (0x400 < run_size) {
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_difflength_diff_engine_message_length,(const char*)&str_d_ss_franklinton_difference_engine_cpp_2,0xee);
        }
        if (run_size == 0) {
          run_offset = (int)run_start - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
          run_section = (*(uint *)&diffeng_scan.scan_section);
          run_size = (int)(*(int * *)&diffeng_scan.scan_ptr) - (int)mirror_start;
        }
        if (0x400 < run_size) {
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_difflength_diff_engine_message_length_2,(const char*)&str_d_ss_franklinton_difference_engine_cpp_3,0xf7);
        }
        if ((flags & 0x10) == 0) {
          crt_memcpy(mirror_start,run_start,run_size);
        }
        rle.src = (u8 *)run_start;
        rle.src_end = (u8 *)run_start + run_size;
        rle.dst = (u8 *)(out_ptr + 3);
        rle.dst_end = (u8 *)&diffeng_scan.mirror_size;  /* (the packet buffer's end) */
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
        if (diffeng.packet_flush_size <= (int)((char *)out_ptr - (char *)&diffeng_scan.packet)) {
          diffeng_scan.packet_size = (i32)((char *)out_ptr - (char *)&diffeng_scan.packet);
          diffeng_scan.packet_record_count = record_count;
          net_send_message(NM_ALPHA_DIFFERENCE_ENGINE,dest,0,0,0,0,0,0,0,(NetMsg *)&diffeng_scan.packet);
          if ((flags & 1) != 0) {
            XD_FlushSendBuffer(5000);
          }
          out_ptr = ((uint *)&diffeng_scan.packet_payload);
          record_count = 0;
        }
        if ((((flags & 4) != 0) && (encoded = timer_ticks(), deadline < encoded)) ||
           (((flags & 8) != 0 && (runs_left = runs_left - 1, runs_left == 0)))) break;
      }
    } while (start_ptr != (*(int * *)&diffeng_scan.scan_ptr));
    if (((uint *)&diffeng_scan.packet_payload) < out_ptr) {
      diffeng_scan.packet_size = (i32)((char *)out_ptr - (char *)&diffeng_scan.packet);
      diffeng_scan.packet_record_count = record_count;
      net_send_message(NM_ALPHA_DIFFERENCE_ENGINE,dest,0,0,0,0,0,0,0,(NetMsg *)&diffeng_scan.packet);
      if ((flags & 1) != 0) {
        XD_FlushSendBuffer(5000);
      }
    }
    diffeng_scan.scan_offset = (int)cur_ptr - (iptr)diffeng_section_table[(*(uint *)&diffeng_scan.scan_section)].data_ptr;
  }
  return bytes_changed;
}

/* 00511880 auto */
void __cdecl
net_send_text_args(int popup_kind,i32 player_id,int string_count,int number_count,i32 arg_a,i32 arg_b)

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
        len = _strlen(((i8 *)&text_args.strings) + i * 0x104);
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
          crt_strcpy((char *)write_ptr,((i8 *)&text_args.strings) + i * 0x104);
          len = _strlen(((i8 *)&text_args.strings) + i * 0x104);
          write_ptr = (void *)((int)write_ptr + len + 1);
        }
      }
      if (number_count != 0) {
        for (i = 0; i < number_count; i++) {
          crt_memcpy(write_ptr,((i8 *)&text_args.numbers) + i * 4,4);
          write_ptr = (void *)((int)write_ptr + 4);
        }
      }
    }
    net_send_message(NM_POPUP,player_id,popup_kind,string_count,number_count,arg_a,arg_b,0,0,(NetMsg *)payload);
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
  if ((*(i32 * *)&net_popups.head) != (i32 *)0x0) {
    *(*(i32 * *)&net_popups.tail) = (i32)entry;
    new_head = (*(i32 * *)&net_popups.head);
  }
  (*(i32 * *)&net_popups.head) = new_head;
  (*(i32 * *)&net_popups.tail) = entry;
  return;
}

/* 00511acf auto */
void __cdecl net_popup_queue_clear(void)

{
  i32 *entry;
  
  while (entry = (*(i32 * *)&net_popups.head), (*(i32 * *)&net_popups.head) != (i32 *)0x0) {
    (*(i32 * *)&net_popups.head) = (i32 *)*(*(i32 * *)&net_popups.head);
    operator_delete(entry);
  }
  net_popups.tail = 0;
  return;
}

/* 00511b2d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_popup_idle_autoclose(void)

{
  int now;
  
  net.process_messages(1,0);
  if (((net.host_handoff_stage == '\0') && (net.nm_parley_cancel_count == 0)) &&
     (now = timer_ticks(), now - dialog_state.timeout_start < 0x4b1)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  dialog_state.timeout_start = timer_ticks();
  return;
}

/* Part of net_popup_queue_show_next (00511ba2): a request to meet (0x3d):
   accepted, the parley window opens (returns 1: the entry is freed and the
   dialog destroyed, the caller returns); refused or cancelled, the answer is
   sent. */
static int net_popup_parley_request(int *entry,Dialog &dlg)
{
  int choice;
  iptr text;
  iptr attitude_level;
  i32 eh_state;
  
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
    text_set_string_arg(0,(const char *)&textbuf.buf);
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
    dlg_style.pending_idle_callback = net_popup_idle_autoclose;
    choice = popup_game_with_sprite(str_parleyrequest,(Sprite *)&popup_info_icon_sprite,0);
    if (net.host_handoff_stage == '\0') {
      if (net.nm_parley_cancel_count == 0) {
        net_send_message(NM_PARLEYREQUEST_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,choice,0,0,0,0,0,0,0);
      }
      if (net.nm_parley_cancel_count == 0) {
        if (choice == 1) {
          text = entry[4];
          operator_delete(entry);
          net.popup_in_progress = 0;
          parley_wnd_block.open(text,2);
          eh_state = 0xffffffff;
          /* inlined unwind funclet 0051399d */ { dlg.Dialog::destruct(); }
          return 1;
        }
      }
      else {
        net.nm_parley_cancel_count = 0;
        text = (iptr)(civ_name_plural(entry[4]));
        text_set_string_arg(0,(const char *)text);
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        popup_game_with_sprite(str_parleycancel_4,(Sprite *)&popup_info_icon_sprite,0);
      }
      parley_wnd_block.other_civ = -1;
      diplo_meeting.mp_pending = 0;
    }
    else {
      net_send_message(NM_PARLEYREQUEST_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                         2,0,0,0,0,0,0,0);
      parley_wnd_block.other_civ = -1;
      diplo_meeting.mp_pending = 0;
    }
  }
  else {
    net_send_message(NM_PARLEYREQUEST_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,2,
                       0,0,0,0,0,0,0);
  }
  return 0;
}

/* Part of net_popup_queue_show_next (00511ba2): the other side asks to see
   where our units come from (100): our answer, sent, and the parley window's
   unit pane updated. */
static void net_popup_reveal_units(int *entry,Dialog &dlg)
{
  iptr text;
  
  if ((parley_wnd_block.closed == 0) && (entry[4] == (int)parley_wnd_block.other_civ)) {
    parley_prompts.units_open = 1;
    dlg.load_game_section((const char *)&str_revealunitorigins,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x375]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x376]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0x14);
    if (dlg.result_extra == 1) {
      parley_wnd_block.local_units_revealed = 1;
      if ((int)parley_wnd_block.mode == 2) {
        ((Control *)parley_wnd_block.conceal_button)->show_and_redraw();
        ((Button *)parley_wnd_block.conceal_button)->enable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      else {
        ((Control *)parley_wnd_block.conceal_button)->hide();
        ((Button *)parley_wnd_block.conceal_button)->disable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      net_send_message(NM_REVEAL_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,human_civ[0],
                         (int)(char)parley_wnd_block.local_units_revealed,0,0,0,0,0,0);
      if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 2)) ||
         (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 2)))) {
        ((Control *)parley_wnd_block.info_hide_buttons[0])->show_and_redraw();
        ((Control *)parley_wnd_block.info_show_buttons[0])->hide();
        parley_wnd_block.local_units_revealed = 1;
        ((Button *)parley_wnd_block.sort_by_unit_buttons[0])->enable();
        ((Button *)parley_wnd_block.sort_by_city_buttons[0])->enable();
        parley_draw_unit_list(0);
      }
    }
    parley_prompts.units_open = 0;
  }
}

/* Part of net_popup_queue_show_next (00511ba2): the other side asks to see
   our city information (0x65): our answer, sent, and the parley window's city
   pane updated. */
static void net_popup_reveal_cities(int *entry,Dialog &dlg)
{
  iptr text;
  
  if ((parley_wnd_block.closed == 0) && (entry[4] == (int)parley_wnd_block.other_civ)) {
    parley_prompts.city_open = 1;
    dlg.load_game_section((const char *)&str_revealcityinfo,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x377]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x376]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0x14);
    if (dlg.result_extra == 1) {
      parley_wnd_block.local_cities_revealed = 1;
      if ((int)parley_wnd_block.mode == 2) {
        ((Control *)parley_wnd_block.conceal_button)->show_and_redraw();
        ((Button *)parley_wnd_block.conceal_button)->enable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      else {
        ((Control *)parley_wnd_block.conceal_button)->hide();
        ((Button *)parley_wnd_block.conceal_button)->disable();
        ((Control *)parley_wnd_block.reveal_button)->hide();
        ((Button *)parley_wnd_block.reveal_button)->disable();
      }
      net_send_message(NM_REVEAL_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,human_civ[0],
                         (int)(char)parley_wnd_block.local_cities_revealed,0,0,0,0,0,0);
      if ((((int)parley_wnd_block.mode == 0xe) && ((int)parley_wnd_block.exchange_kind[1] == 3)) ||
         (((int)parley_wnd_block.mode == 0xf && ((int)parley_wnd_block.demand_kind == 3)))) {
        ((Control *)parley_wnd_block.info_hide_buttons[0])->show_and_redraw();
        ((Control *)parley_wnd_block.info_show_buttons[0])->hide();
        parley_draw_city_list(0);
      }
    }
    parley_prompts.city_open = 0;
  }
}

/* Part of net_popup_queue_show_next (00511ba2): the entry's arguments
   (entry[2] strings, then entry[3] numbers) into the text arguments. */
static void net_popup_load_args(int *entry)
{
  size_t len;
  int i;
  int *arg_ptr;
  
  if ((entry[2] != 0) || (entry[3] != 0)) {
    arg_ptr = entry + 7;
  }
  if (entry[2] != 0) {
    for (i = 0; i < entry[2]; i++) {
      crt_strcpy(((i8 *)&text_args.strings) + i * 0x104,(const char *)arg_ptr);
      len = _strlen(((i8 *)&text_args.strings) + i * 0x104);
      arg_ptr = (int *)((int)arg_ptr + len + 1);
    }
  }
  if (entry[3] != 0) {
    for (i = 0; i < entry[3]; i++) {
      crt_memcpy(((i8 *)&text_args.numbers) + i * 4,arg_ptr,4);
      arg_ptr++;
    }
  }
}

/* Part of net_popup_queue_show_next (00511ba2), of net_popup_show: the popups that are a dialog of their own: wonders and improvements
   started, switched, abandoned, captured, lost, built and soon obsolete (with
   their pictures), the turn timer's and the government change's votes, a
   list of lines.
   Returns 0 for a popup of another kind. */
static int net_popup_show_dialog(int *entry,Dialog &dlg)
{
  iptr text;
  i8 *icon_sprite;
  int i;

  switch(entry[1]) {
  case 0x12:
    dlg.load_game_text((const char *)&str_startwonder_2);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(entry[5]);
    dlg.show_modal(0);
    break;
  case 0x13:
    dlg.load_game_text((const char *)&str_switchwonder_2);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(entry[5]);
    dlg.show_modal(0);
    break;
  case 0x14:
    dlg.load_game_text((const char *)&str_abandonwonder_2);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(entry[5]);
    dlg.show_modal(0);
    break;
  case 0x19:
    dlg.load_game_text((const char *)&str_capturewonder);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x1a:
    dlg.load_game_text((const char *)&str_lostwonder);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x40:
    dlg.load_game_section((const char *)&str_newturntimerclient,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x354]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x355]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0x28);
    net_send_message(NM_NEWTURNTIMER_REPLY,0,dlg.result_extra,0,0,0,0,0,0,0);
    break;
  case 0x43:
    dlg.load_game_section((const char *)&str_pmchangeclient,0x2000000);
    text = (iptr)(label_string(label_table.string_ids[0x354]));
    dlg.add_button((const char *)text);
    text = (iptr)(label_string(label_table.string_ids[0x355]));
    dlg.add_button((const char *)text);
    dlg.show_modal(0);
    net_send_message(NM_PMCHANGE_REPLY,0,dlg.result_extra,0,0,0,0,0,0,0);
    break;
  case 0x47:
    dlg.load_game_text((const char *)&str_built2_2);
    if (entry[4] < 0) {
      icon_sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[5] * 0x3c;
    }
    else {
      icon_sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + entry[4] * 0x3c;
    }
    dlg.add_sprite((Sprite *)icon_sprite,0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4a:
    dlg.load_game_text((const char *)&str_almostwonder_2);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4b:
    dlg.load_game_text((const char *)&str_stillwonder1);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4c:
    dlg.load_game_text((const char *)&str_stillwonder2);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4d:
    dlg.load_game_text((const char *)&str_endwonder_4);
    dlg.add_sprite(
               (Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                     (char)advances[entry[4]].category * 0xf0 +
                     (char)advances[entry[4]].epoch * 0x3c),0,0);
    dlg.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[5] * 0x3c)),0,0);
    dlg.set_image_zoom(8);
    dlg.show_modal(0);
    break;
  case 0x4f:
    dlg.init_default();
    for (i = 0; i < entry[2]; i++) {
      dlg.add_line((const char *)(((i8 *)&text_args.strings) + i * 0x104),0xffffffff,
                 0xffffffff,0xffffffff,0);
    }
    dlg.show_modal(0);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of net_popup_queue_show_next (00511ba2), of net_popup_show: espionage and bribery: units deserting or bribed (the answer sent
   back), sabotage, embassies, stolen advances, poisoned water, planted
   nukes, incited revolt, the spy's promotions, ransom.
   Returns 0 for a popup of another kind. */
static int net_popup_show_spy(int *entry,Dialog &dlg)
{
  int choice;
  iptr text;

  switch(entry[1]) {
  case 0x22:
    dlg_game_txt_with_unit((const char *)&str_deserted_2,entry[4]);
    break;
  case 0x23:
    text = dlg_game_txt_with_unit((const char *)&str_desert_3,entry[5]);
    net_send_message(NM_DESERT_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                       text,0,0,0,0,0,0,0);
    break;
  case 0x24:
    text = dlg_game_txt_with_unit((const char *)&str_desert2,entry[5]);
    net_send_message(NM_DESERT_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                       text,0,0,0,0,0,0,0);
    break;
  case 0x25:
    dlg_game_txt_with_unit((const char *)&str_blewitup_2,entry[4]);
    break;
  case 0x26:
    popup_game_with_sprite(str_bond007_2,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c)),entry[5]);
    break;
  case 0x27:
    popup_game_with_sprite(str_bond_2,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c)),entry[5]);
    break;
  case 0x28:
    popup_game_with_sprite(str_bondglory_2,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + entry[4] * 0x3c)),entry[5]);
    break;
  case 0x37:
    dlg_game_txt_with_unit_flags((const char *)&str_ransom,entry[4],0);
    break;
  case 0x55:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite(str_enemyembassy_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x56:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite(str_enemyinvestigate_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x57:
    choice = civ_has_advance(entry[4],ADV_GUNPOWDER);
    if (choice == 0) {
      snd_play_effect(SND_SPYSOUND,1,0,0);
    }
    else {
      snd_play_effect(SND_MEDEXPL,1,0,0);
    }
    popup_game_with_sprite(str_steal_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x58:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite(str_foiledagain,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x59:
    choice = civ_has_advance(entry[5],ADV_GUNPOWDER);
    if (choice == 0) {
      snd_play_effect(SND_SPYSOUND,1,0,0);
    }
    else {
      snd_play_effect(SND_MEDEXPL,1,0,0);
    }
    popup_game_with_sprite(str_sabotageone_2,((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + entry[4] * 0x3c)),8);
    break;
  case 0x5a:
    choice = civ_has_advance(entry[4],ADV_GUNPOWDER);
    if (choice == 0) {
      snd_play_effect(SND_SPYSOUND,1,0,0);
    }
    else {
      snd_play_effect(SND_MEDEXPL,1,0,0);
    }
    popup_game((const char *)&str_sabotagetwo_2);
    break;
  case 0x5b:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game_with_sprite(str_watersupply_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x5c:
    text_clear();
    text_append((const char *)&str_plantednuke_2);
    popup_game_with_improvement((const char *)&textbuf.buf,IMP_MANHATTAN,8);
    break;
  case 0x5d:
    text_clear();
    text_append((const char *)&str_plantednuke2);
    popup_game_with_improvement((const char *)&textbuf.buf,IMP_MANHATTAN,8);
    break;
  case 0x5e:
    snd_play_effect(SND_SPYSOUND,1,0,0);
    popup_game((const char *)&str_civilwar_2);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of net_popup_queue_show_next (00511ba2), of net_popup_show: diplomacy: schism, broken ceasefires, sneak attacks, allies called
   and helping, peace cancelled, wars ended, military aid, mercenaries,
   a civ taken over, the units and cities a parley reveals.
   Returns 0 for a popup of another kind. */
static int net_popup_show_diplomacy(int *entry,Dialog &dlg)
{
  switch(entry[1]) {
  case 0x16:
    popup_game((const char *)&str_schism_2);
    break;
  case 0x2d:
    dlg_game_txt_with_unit((const char *)&str_breakcease_2,entry[4]);
    break;
  case 0x2e:
    dlg_game_txt_with_unit((const char *)&str_sneak,entry[4]);
    break;
  case 0x30:
    dlg_game_txt_with_unit((const char *)&str_pearlharbor,entry[4]);
    break;
  case 0x38:
    popup_game_with_sprite(str_activateally_2,((Sprite *)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))),0);
    break;
  case 0x39:
    popup_game_with_sprite(str_allyhelps_2,((Sprite *)((game.advance_known_by[ADV_COMMUNISM] != '\0') ? ((i8 *)&diplo_popup_portrait_sprite) : ((i8 *)&signal_allied_icon_sprite))),0);
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
  case 0x50:
    popup_game_with_sprite(str_warends,(Sprite *)&ceasefire_icon_sprite,0);
    break;
  case 0x51:
    dlg_game_txt_with_unit((const char *)&str_militaryaid1,entry[4]);
    break;
  case 0x52:
    dlg_game_txt_with_unit((const char *)&str_militaryaid2,entry[4]);
    break;
  case 0x62:
    popup_game((const char *)&str_mercdeclare_2);
    break;
  case 99:
    popup_game_with_advance((const char *)&str_tookciv,entry[4],0);
    break;
  case 100:
    net_popup_reveal_units(entry,dlg);
    break;
  case 0x65:
    net_popup_reveal_cities(entry,dlg);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of net_popup_queue_show_next (00511ba2), of net_popup_show: the game's events: retirement, barbarians, global warming,
   the historians, the spaceship, civs destroyed, caravans, paradrops,
   cities captured, promotions, governments, nuclear attacks, ....
   Returns 0 for a popup of another kind. */
static int net_popup_show_event(int *entry,Dialog &dlg)
{
  iptr text;

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
    popup_game_with_sprite(str_globalwarming_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 4:
    popup_game_with_sprite(str_fearwarming_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 5:
    historian_report_show();
    break;
  case 6:
    popup_game_with_sprite(str_eaglehaslanded_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 7:
    popup_game_with_sprite(str_scenarioends_2,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    break;
  case 8:
    popup_game_with_sprite(str_scenarioend_2,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    break;
  case 9:
    text = civ_leader_title(human_civ[0]);
    text_set_string_arg(0,(const char *)text);
    text = (iptr)(civ_leader_name(human_civ[0]));
    text_set_string_arg(1,(const char *)text);
    popup_game_with_sprite(str_planretire_2,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    break;
  case 10:
    text = (iptr)(civ_leader_name(human_civ[0]));
    text_set_string_arg(1,(const char *)text);
    popup_game_with_sprite(str_doretire_2,(Sprite *)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
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
    popup_game_with_sprite(str_destroyed_2,((Sprite *)(((i8 *)&road_sprites_block.civ_portrait_sprites) + entry[4] * 0x3c)),0);
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
  case 0x15:
    dlg_game_txt_with_unit((const char *)&str_paradrop_2,entry[4]);
    break;
  case 0x17:
    text = popup_game_with_sprite(str_canescape,(Sprite *)&popup_info_icon_sprite,0);
    net_send_message(NM_CANESCAPE_REPLY,net.net_slots[net.net_civ_slots[entry[4]]].id,
                       text,0,0,0,0,0,0,0);
    break;
  case 0x18:
    popup_game_with_city((const char *)&str_escape,entry[4]);
    break;
  case 0x1b:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_citycapture,entry[5],0x46,1,entry[4]);
    break;
  case 0x1c:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_citywinally,entry[5],0x46,1,entry[4]);
    break;
  case 0x1d:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_cityloseally,entry[5],0x46,1,entry[4]);
    break;
  case 0x1e:
    snd_play_effect(SND_BARRACKS,0,0,0);
    popup_city_picture_zoom((const char *)&str_citycapture2,entry[5],0x46,1,entry[4]);
    break;
  case 0x1f:
    dlg_game_txt_with_unit((const char *)&str_partisans,entry[4]);
    break;
  case 0x20:
    dlg_game_txt_with_unit((const char *)&str_promoted,entry[4]);
    break;
  case 0x21:
    popup_game_with_sprite(str_incorruptible_2,(Sprite *)&government_sprites_block.government_sprite_democracy,0);
    break;
  case 0x29:
    popup_game_with_improvement((const char *)&str_useweapons,IMP_MANHATTAN,8);
    break;
  case 0x2a:
    popup_game_with_improvement((const char *)&str_sdi,IMP_SDI_DEFENSE,8);
    break;
  case 0x2b:
    popup_game_with_sprite(str_changed,((Sprite *)(((i8 *)&government_sprites_block.government_sprites) + entry[4] * 0x3c)),entry[5]);
    break;
  case 0x2c:
    popup_game_with_sprite(str_overthrown,((Sprite *)(((i8 *)&government_sprites_block.government_sprites) + entry[4] * 0x3c)),entry[5]);
    break;
  case 0x2f:
    dlg_game_txt_with_unit((const char *)&str_missileattack,entry[4]);
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
  case 0x3e:
    dlg_game_txt_with_unit_flags((const char *)&str_upgrade_2,entry[4],(entry[5] == 0) - 1 & 8);
    break;
  case 0x3f:
    if (net.net_slots[net.net_civ_slots[human_civ[0]]].id != entry[4])
    {
      popup_game_with_sprite(str_newplayer_2,(Sprite *)&popup_info_icon_sprite,0);
    }
    break;
  case 0x41:
    popup_game_with_sprite(str_newturntimerno,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x42:
    popup_game_with_sprite(str_newturntimeryes,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x44:
    popup_game_with_sprite(str_pmchangeno_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x45:
    popup_game_with_sprite(str_pmchangeyes_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x46:
    snd_play_effect(SND_LARGEXPL,1,0,0);
    city_notify_dialog_ex(str_manhattan_2,entry[4],1,(Sprite *)&caption_close_button_sprite_block.city_notify_sprite);
    break;
  case 0x48:
    popup_game((const char *)&str_movecapital_2);
    break;
  case 0x49:
    popup_game((const char *)&str_astronauts_2);
    break;
  case 0x4e:
    dlg_game_txt_with_unit((const char *)&str_barbariansland,entry[4]);
    break;
  case 0x53:
    popup_game_with_sprite(str_lostclient_2,(Sprite *)&popup_info_icon_sprite,0);
    break;
  case 0x54:
    popup_game_with_advance((const char *)&str_goldenage_2,ADV_PHILOSOPHY,8);
    break;
  case 0x5f:
    snd_play_effect(SND_FEEDBK03,0,0,0);
    dlg_game_txt_with_unit((const char *)&str_upmine,entry[4]);
    break;
  case 0x60:
    snd_play_effect(SND_FEEDBK03,0,0,0);
    dlg_game_txt_with_unit((const char *)&str_upyours,entry[4]);
    break;
  case 0x61:
    snd_play_effect(SND_FEEDBK03,0,0,0);
    dlg_game_txt_with_unit((const char *)&str_upyourstoo,entry[4]);
    break;
  default:
    return 0;
  }
  return 1;
}

/* Part of net_popup_queue_show_next (00511ba2): the popup, by kind
   (entry[1]). Returns 1 when the caller is to return at once (a meeting
   accepted: the entry is freed and the dialog destroyed). */
static int net_popup_show(int *entry,Dialog &dlg)
{
  if (entry[1] == 0x3d) {
    /* a parley request */
    if (net_popup_parley_request(entry,dlg) != 0) {
      return 1;
    }
    return 0;
  }
  if (net_popup_show_dialog(entry,dlg) != 0) return 0;
  if (net_popup_show_spy(entry,dlg) != 0) return 0;
  if (net_popup_show_diplomacy(entry,dlg) != 0) return 0;
  net_popup_show_event(entry,dlg);
  return 0;
}

/* 00511ba2 auto */
/* WARNING: Removing unreachable block (ram,0x0051343e) */
/* WARNING: Removing unreachable block (ram,0x0051347e) */
/* WARNING: Removing unreachable block (ram,0x0051369c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_popup_queue_show_next(void)

{
  int *entry;
  Dialog dlg;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  entry = (*(int * *)&net_popups.head);
  dlg.Dialog::construct(0x4000);
  eh_state = 0;
  if (entry != (int *)0x0) {
    net.popup_in_progress = 1;
    (*(int * *)&net_popups.head) = (int *)*entry;
    if ((*(int * *)&net_popups.head) == (int *)0x0) {
      net_popups.tail = 0;
    }
    net_popup_load_args(entry);
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
    if (net_popup_show(entry,dlg) != 0) {
      return;
    }
    operator_delete(entry);
    net.popup_in_progress = 0;
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0051399d */ { dlg.Dialog::destruct(); }
  return;
}

/* 00514220 auto */
NetMsgQueue *NetMsgQueue::construct()

{
  _memset(this,0,24000);
  reset();
  return this;
}

/* 00514254 auto */
void NetMsgQueue::clear()

{
  reset();
  return;
}

/* 00514272 auto */
void NetMsgQueue::reset()

{
  u8 *field_ptr;
  int i;
  
  for (i = 0; i < 2000; i++) {
    if (slots[i].data != 0) {
      operator_delete((void *)slots[i].data);
      field_ptr = (u8 *)&slots[i].data;
      field_ptr[0] = '\0';
      field_ptr[1] = '\0';
      field_ptr[2] = '\0';
      field_ptr[3] = '\0';
    }
    field_ptr = (u8 *)&slots[i].size;
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
    field_ptr = (u8 *)&slots[i].sender;
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
  }
  main_head = 400;
  main_tail = 400;
  main_count = 0;
  prio_head = 0;
  prio_tail = 0;
  prio_count = 0;
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

i32 NetMsgQueue::enqueue(int sender,NetMsg *msg,uint msg_size)

{
  int is_priority;
  void *msg_buf;
  i8 lost_msg_text[256];
  iptr msg_copy;
  i8 lost_prio_text[256];
  
  if ((((game.multiplayer == 5) || (game.multiplayer == 6)) && (sender != 0)) &&
     (sender != 0xff)) {
    if (net.host_flag == '\0') {
      sender = 0;
    }
    else {
      sender = 1;
    }
  }
  if ((int)msg->hdr.msg_type == NM_HEART_BEAT) {
    net.nm_heart_beat_count++;
    net_send_message(NM_HEART_BEAT_ACK,sender,0,0,0,0,0,0,0,0);
  }
  else if ((int)msg->hdr.msg_type == NM_HEART_BEAT_ACK) {
    net.nm_heartbeat_ack_count++;
  }
  else {
    is_priority = net_msg_is_priority((i32)msg->hdr.msg_type);
    if (is_priority == 0) {
      if ((main_head == main_tail) && (main_count != 0)) {
        crt_strcpy(lost_msg_text,str_message_lost_in_enqueuemessage);
        crt_strcat(lost_msg_text,((char *)&str_nm_datagram) + (int)msg->hdr.msg_type * 0x20);
        crt_strcat(lost_msg_text,str_from_d_size_d_2);
        dbg_logf_int2((const char *)lost_msg_text,sender,msg_size);
        return 0;
      }
      if (((int)main_head < 400) || (1999 < (int)main_head)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_head_400_head_max_msgs_in_queue,(const char*)&str_d_ss_franklinton_netmessagequeue_cpp_4,0x130);
      }
      if (((int)main_tail < 400) || (1999 < (int)main_tail)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_tail_400_tail_max_msgs_in_queue,(const char*)&str_d_ss_franklinton_netmessagequeue_cpp_5,0x131);
      }
      msg_buf = operator_new(msg_size);
      slots[main_tail].data = (i32)msg_buf;
      if (slots[main_tail].data == 0) {
        dbg_logf_int2((const char *)&str_enqueuemessage_primary_failed_to_allocat,(i32)msg->hdr.msg_type,msg->size);
        return 0;
      }
      if (slots[main_tail].data == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_msgqueue_tail_pdata,(const char*)&str_d_ss_franklinton_netmessagequeue_cpp_6,0x136);
      }
      crt_memcpy((void *)slots[main_tail].data,(const void *)msg,msg_size);
      slots[main_tail].sender = sender;
      slots[main_tail].size = msg_size;
      main_tail = (int)(main_tail + 1) % 2000;
      if (main_tail == 0) {
        main_tail = 400;
      }
      main_count++;
      if ((int)net.msgq_normal_peak < (int)main_count) {
        net.msgq_normal_peak = main_count;
      }
    }
    else {
      if ((prio_head == prio_tail) && (prio_count != 0)) {
        crt_strcpy(lost_prio_text,str_message_lost_in_alpha_enqueuemessage);
        crt_strcat(lost_prio_text,((char *)&str_nm_datagram) + (int)msg->hdr.msg_type * 0x20);
        crt_strcat(lost_prio_text,str_from_d_size_d);
        dbg_logf_int2((const char *)lost_prio_text,sender,msg_size);
        return 0;
      }
      if (((int)prio_head < 0) || (399 < (int)prio_head)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_alphahead_0_alphahead_400,(const char*)&str_d_ss_franklinton_netmessagequeue_cpp,0x10c);
      }
      if (((int)prio_tail < 0) || (399 < (int)prio_tail)) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_alphatail_0_alphatail_400,(const char*)&str_d_ss_franklinton_netmessagequeue_cpp_2,0x10d);
      }
      msg_buf = operator_new(msg_size);
      slots[prio_tail].data = (i32)msg_buf;
      if (slots[prio_tail].data == 0) {
        msg_copy = (iptr)msg;
        dbg_logf_int2((const char *)&str_enqueuemessage_alpha_failed_to_allocate,(i32)msg->hdr.msg_type,msg->size);
        return 0;
      }
      if (slots[prio_tail].data == 0) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_msgqueue_alphatail_pdata,(const char*)&str_d_ss_franklinton_netmessagequeue_cpp_3,0x112);
      }
      crt_memcpy((void *)slots[prio_tail].data,(const void *)msg,msg_size);
      slots[prio_tail].sender = sender;
      slots[prio_tail].size = msg_size;
      prio_tail = (int)(prio_tail + 1) % 400;
      prio_count++;
      if ((int)net.msgq_priority_peak < (int)prio_count) {
        net.msgq_priority_peak = prio_count;
      }
    }
  }
  return 1;
}

/* 005149d6 auto */
i32 NetMsgQueue::dequeue(i32 *out_sender,i32 *out_msg,i32 *out_size,int priority_only)

{
  u8 *slot_field;
  i32 result;
  
  if (prio_count == 0) {
    if ((priority_only == 0) && (main_count != 0)) {
      *out_sender = slots[main_head].sender;
      slot_field = (u8 *)&slots[main_head].sender;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      *out_msg = slots[main_head].data;
      slot_field = (u8 *)&slots[main_head].data;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      *out_size = slots[main_head].size;
      slot_field = (u8 *)&slots[main_head].size;
      slot_field[0] = '\0';
      slot_field[1] = '\0';
      slot_field[2] = '\0';
      slot_field[3] = '\0';
      main_head = (int)(main_head + 1) % 2000;
      if (main_head == 0) {
        main_head = 400;
      }
      main_count--;
      result = 1;
    }
    else {
      result = 0;
    }
  }
  else {
    *out_sender = slots[prio_head].sender;
    slot_field = (u8 *)&slots[prio_head].sender;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    *out_msg = slots[prio_head].data;
    slot_field = (u8 *)&slots[prio_head].data;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    *out_size = slots[prio_head].size;
    slot_field = (u8 *)&slots[prio_head].size;
    slot_field[0] = '\0';
    slot_field[1] = '\0';
    slot_field[2] = '\0';
    slot_field[3] = '\0';
    prio_head = (int)(prio_head + 1) % 400;
    prio_count--;
    result = 1;
  }
  return result;
}

/* 0051ea0c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_vote_wait_idle_cb(void)

{
  int now_ticks;
  
  net.process_messages(1,0);
  if ((((net.host_handoff_stage == '\0') && (now_ticks = timer_ticks(), now_ticks - dialog_state.timeout_start < 0x961)) &&
      (net.vote_replies_pending != 0)) && (net.vote_status != -1)) {
    return;
  }
  net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
  ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  dialog_state.timeout_start = timer_ticks();
  return;
}

/* 0051f11a auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_lock_wait_idle_cb(void)

{
  timer_ticks();
  net.process_messages(1,0);
  if ((net.nm_lock_data_count != 0) || (net.nm_server_canceled_count != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 0051f17e auto */
void __cdecl net_pump_idle_cb(void)

{
  net.process_messages(1,0);
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
  
  if (net.host_flag == '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_gnetmgr_bserver_2,(const char*)&str_d_ss_franklinton_startup_multiplayer_cpp_2,0x778);
  }
  saved_human_civ = human_civ[0];
  saved_turn_civ_val = game.turn_civ;
  saved_player_civ = game.player_civ;
  if (((saved_turn.resume_new_game == 0) && (saved_turn.civ == '\0')) && (net.session_phase < 2)) {
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
    if (net.session_phase < 3) {
      game.human_players = game.human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net.pending_human_players = net.pending_human_players | (byte)(1 << ((byte)join_civ & 0x1f));
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
    crt_strcpy((char *)(&civ_names[game.player_civ].leader_name),(const char *)((const void *)player_info->leader_name));
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
      crt_strcpy((char *)(&civ_names[game.player_civ].tribe_name),(const char *)((const void *)player_info->tribe_name));
      crt_strcpy((char *)(&civ_names[game.player_civ].adjective),(const char *)((const void *)player_info->adjective));
      if (player_info->has_titles != 0) {
        for (title_idx = 0; title_idx < 7; title_idx++) {
          crt_strcpy((char *)(&civ_names[game.player_civ].titles[title_idx]),
                       (const char *)player_info->govt_titles[title_idx]);
        }
      }
    }
    civ_names[game.player_civ].city_style = (i16)player_info->city_style;
  }
  else {
    join_civ = player_info->civ;
    if (net.session_phase < 3) {
      game.human_players = game.human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net.pending_human_players = net.pending_human_players | (byte)(1 << ((byte)join_civ & 0x1f));
    }
    game.player_civ = (byte)join_civ;
    game.turn_civ = (byte)join_civ;
    human_civ[0] = join_civ;
    crt_strcpy((char *)(&civ_names[((char)(byte)join_civ)].leader_name),(const char *)((const void *)player_info->leader_name));
    if (0 < leaders[civs[game.player_civ].tribe].leader_name) {
      leaders[civs[game.player_civ].tribe].leader_name =
           -leaders[civs[game.player_civ].tribe].leader_name;
    }
    is_female = player_info->gender;
    civs[human_civ[0]].flags = civs[human_civ[0]].flags & ~CIVF_FEMALE_LEADER;
    if (is_female != 0) {
      civs[human_civ[0]].flags = civs[human_civ[0]].flags | CIVF_FEMALE_LEADER;
    }
    female_byte = (u8)is_female;
    leaders[civs[game.player_civ].tribe].female = female_byte;
  }
  for (slot = 0; (slot < 7 && (net.net_slots[slot].id != player_id));
      slot = slot + 1) {
  }
  if (slot < 7) {
    net.net_civ_slots[game.player_civ] = slot;
    (((i32 *)&net.slot_civs))[slot * 0x15] = (int)game.player_civ;
    (((i8 *)&net.slot_active_flags))[slot * 0x54] = 1;
    if (player_id == 0) {
      (((i8 *)&net.slot_host_flags))[slot * 0x54] = 1;
    }
    else {
      (((i8 *)&net.slot_host_flags))[slot * 0x54] = 0;
    }
    result = 1;
  }
  else {
    if (net.session_phase < 3) {
      game.human_players = game.human_players & ~(byte)(1 << ((byte)join_civ & 0x1f));
    }
    else {
      net.pending_human_players = net.pending_human_players & ~(byte)(1 << ((byte)join_civ & 0x1f));
    }
    net_send_message(NM_UNLOCK_NATION,0,player_info->leader_index,player_info->civ,0,0,0,0,0,0);
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
  net_send_message(NM_COSMIC,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_CIVILIZE,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_IMPROVE,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_ENDWONDER,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_UNITS,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_TERRAIN,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_GOVT,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_GOVT_TITLE,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_CARAVAN,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_ORDERS,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_DIFFICULTY,dest_player,0,0,0,0,0,0,0,0);
  net_send_message(NM_ATTITUDES,dest_player,0,0,0,0,0,0,0,0);
  return;
}

/* 0055ae80 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_stop(int notify)

{
  if (net.mp_turn_civ == human_civ[0]) {
    city_window_close();
    intel_report_end_modal();
    throneroom_close();
    pedia_end_modal();
  }
  if (mp_timer.handle != 0) {
    game_timer_destroy(mp_timer.handle);
    mp_timer.handle = 0;
    status_draw_timer_bar(100,0xffffffff);
    if (((notify != 0) && (2 < game.multiplayer)) && (player_turn_active != '\0')) {
      net_send_message(NM_STOP_TURN_TIMER,0xff,0,0,0,0,0,0,0,0);
    }
  }
  return;
}

/* 0055af2e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_turn_timer_start(int notify)

{
  RECT caption_rect;
  
  if (net.mp_turn_civ == human_civ[0]) {
    city_window_close();
    intel_report_end_modal();
    throneroom_close();
    pedia_end_modal();
  }
  mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
  setup_picwnd_block.g_timer_blink[0] = 0;
  map_views_block.main_view_timer_cache_i32 = 0xffffffff;
  ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
  caption_rect.left = map_views_block.main_view_frame_left;
  caption_rect.top = map_views_block.main_view_frame_top;
  caption_rect.right = map_views_block.main_view_frame_right;
  caption_rect.bottom = map_views_block.main_view_client_top;
  ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&caption_rect);
  if (mp_timer.handle != 0) {
    game_timer_destroy(mp_timer.handle);
  }
  mp_timer.handle = game_timer_create((TimerFn)mp_turn_timer_tick,500,1);
  mp_timer.last_tick = timer_ticks();
  if ((notify != 0) && (2 < game.multiplayer)) {
    net_send_message(NM_START_TURN_TIMER,0xff,0,0,0,0,0,0,0,0);
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
  if ((mp_timer.seconds_left != 0) && (map_views_block.main_view_timer_shown[0] != 0)) {
    if (mp_timer.handle != 0) {
      game_timer_destroy(mp_timer.handle);
    }
    mp_timer.handle = game_timer_create((TimerFn)mp_turn_timer_tick,500,1);
    mp_timer.last_tick = timer_ticks();
    if ((notify != 0) && (2 < game.multiplayer)) {
      net_send_message(NM_RESTART_TURN_TIMER,0xff,0,0,0,0,0,0,0,0);
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
      ((FrameWindow *)((i8 *)&map_views_block.map_views))->FrameWindow::draw_caption();
      caption_rect.left = map_views_block.main_view_frame_left;
      caption_rect.top = map_views_block.main_view_frame_top;
      caption_rect.right = map_views_block.main_view_frame_right;
      caption_rect.bottom = map_views_block.main_view_client_top;
      ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect(&caption_rect);
    }
    else {
      map_views_block.main_view_timer_shown[0] = 1;
      mp_turn_timer_start(1);
    }
    if (2 < game.multiplayer) {
      net_send_message(NM_TIMER_CHANGE,0xff,civ_saved_cursor_x_block.mp_turn_time_limit_ms,0,0,0,0,0,0,0);
    }
  }
  else {
    mp_turn_timer_resume(1);
  }
  if (autoplay.active != 0) {
    player_turn_active = 0;
  }
  return;
}

/* 0055b3c8 auto */
void __cdecl mp_mouse_watchdog_stop(void)

{
  if (mp_timer.mouse_watchdog != 0) {
    game_timer_destroy(mp_timer.mouse_watchdog);
    mp_timer.mouse_watchdog = 0;
  }
  return;
}

/* 0055b3fd auto */
void __cdecl mp_mouse_watchdog_start(void)

{
  if (2 < game.multiplayer) {
    if (mp_timer.mouse_watchdog != 0) {
      game_timer_destroy(mp_timer.mouse_watchdog);
    }
    mp_timer.mouse_watchdog = game_timer_create(mp_mouse_watchdog_tick,500,1);
  }
  return;
}

/* 0055b451 auto */
i32 __cdecl mp_window_drag_mouse_hook(void)

{
  if (mp_drag.active != '\0') {
    mp_mouse_watchdog_tick(0,0);
  }
  return 0;
}

/* 0055b47e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mp_window_drag_begin(void)

{
  dbg_log((const char *)&str_enter_window_drag);
  mp_drag.active = 1;
  mp_drag.start_ticks = timer_ticks();
  mp_drag.flag = 0;
  mp_drag.wnd = wnd_get_active_child();
  if (net.host_flag != '\0') {
    net_send_message(NM_DRAG_HOLD_ON,0xff,0,0,0,0,0,0,0,0);
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
  mp_drag.active = 0;
  mp_drag.start_ticks = 0;
  mp_drag.flag = 0;
  mp_drag.wnd = 0;
  UnhookWindowsHookEx(setup_picwnd_block.mp_window_drag_hook);
  if (net.host_flag != '\0') {
    net_send_message(NM_DRAG_HOLD_OFF,0xff,0,0,0,0,0,0,0,0);
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
  
  if (mp_drag.active != '\0') {
    if ((*(WndBase * *)&mp_drag.wnd) != (WndBase *)0x0) {
      lParam = 0;
      wParam = 0;
      Msg = 0x202;
      wnd = (*(WndBase * *)&mp_drag.wnd)->get_window();
      PostMessageA(wnd->hwnd,Msg,wParam,lParam);
      (*(WndBase * *)&mp_drag.wnd)->release_capture();
    }
    mp_drag.active = '\0';
  }
  return;
}

/* 0055b5fa auto */
void __cdecl mp_mouse_watchdog_tick(i32 timer_id,i32 timer_arg)

{
  int now_ticks;
  
  if ((((2 < game.multiplayer) && (net.net_player_count[0] != 1)) && (mp_drag.active != '\0')) &&
     ((now_ticks = timer_ticks(), mp_drag.start_ticks + 0x26c < now_ticks && (net.player_has_quit == '\0')))) {
    mp_release_mouse_capture();
  }
  mp_mouse_watchdog_start();
  return;
}

/* 00594d42 auto */
i32 __cdecl
net_lock_map(int civ,int src_x,int src_y,int dst_x,int dst_y,int wait_reply)

{
  i32 result;
  int start_ticks;
  int now_ticks;
  int src_conflict;
  int dst_conflict;
  int slot;
  
  net.lock_map_busy = 1;
  net_map_lock_slots_block.net_map_locks[civ].replies = 0;
  net_map_lock_slots_block.net_map_locks[civ].state = net_map_lock_slots_block.net_map_locks[civ].replies;
  if (net.host_flag == '\0') {
    net.lock_map_busy = 0;
    net_send_message(NM_LOCK_MAP,0,civ,src_x,src_y,dst_x,dst_y,0,0,0);
    XD_FlushSendBuffer(60000);
    if (wait_reply == 0) {
      result = 1;
    }
    else {
      start_ticks = timer_ticks();
      while( true ) {
        now_ticks = timer_ticks();
        if ((0xe0f < now_ticks - start_ticks) || (net_map_lock_slots_block.net_map_locks[civ].replies != 0)) break;
        net.process_messages(1,1);
      }
      if (net_map_lock_slots_block.net_map_locks[civ].replies == 0) {
        dbg_log((const char *)&str_lockmap_connection_to_server_timed_out);
        popup_game_with_sprite(str_serverconnecttime_8,(Sprite *)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
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
          ((((i8 *)&net.slot_active_flags))[net.net_civ_slots[civ] * 0x54] != '\0')))) {
        net_send_message(NM_LOCK_MAP_ACK,net.net_slots[net.net_civ_slots[civ]].id,
                           civ,1,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
      net.lock_map_busy = 0;
      result = 1;
    }
    else {
      if (((human_civ[0] != civ) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         ((((i8 *)&net.slot_active_flags))[net.net_civ_slots[civ] * 0x54] != '\0')) {
        net_send_message(NM_LOCK_MAP_ACK,net.net_slots[net.net_civ_slots[civ]].id,
                           civ,-1,0,0,0,0,0,0);
        XD_FlushSendBuffer(60000);
      }
      net.lock_map_busy = 0;
      result = 0;
    }
  }
  return result;
}

/* 0059511c auto */
void __cdecl net_unlock_map(int civ,int wait_reply)

{
  int start_ticks;
  int now_ticks;
  
  net.unlock_map_busy = 1;
  if (net.host_flag == '\0') {
    net.unlock_map_busy = 0;
    net_map_lock_slots_block.net_map_locks[civ].state = 0;
    net_send_message(NM_UNLOCK_MAP,0,civ,0,0,0,0,0,0,0);
    if (wait_reply != 0) {
      start_ticks = timer_ticks();
      while( true ) {
        now_ticks = timer_ticks();
        if ((0xe0f < now_ticks - start_ticks) || (net_map_lock_slots_block.net_map_locks[civ].state != 0)) break;
        net.process_messages(1,1);
      }
      if (net_map_lock_slots_block.net_map_locks[civ].state == 0) {
        dbg_log((const char *)&str_unlockmap_connection_to_server_timed_out);
        popup_game_with_sprite(str_serverconnecttime_9,(Sprite *)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
      }
    }
  }
  else {
    _memset(((i8 *)&net_map_lock_slots_block.net_map_lock_slots) + civ * 0x18,-1,0x18);
    net_send_message(NM_LOCK_MAP_SYNC,0xff,civ,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    net.unlock_map_busy = 0;
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
  net.construct();
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
  net.destruct();
  return;
}

/* 0059a8bb auto */
NetSession *NetSession::construct()

{
  i32 seh_frame;
  
  player_info_msg.init();
  msg_queue.construct();
  ((u8 *)&connection_title)[0] = '\0';
  ((u8 *)&connection_title)[1] = '\0';
  ((u8 *)&connection_title)[2] = '\0';
  ((u8 *)&connection_title)[3] = '\0';
  XD_SetBroadcastReceive(net_on_broadcast_receive);
  XD_SetSecureReceive(net_on_secure_receive);
  XD_SetOnClientConnectionToServer(net_on_connected_to_server);
  XD_SetNewClientConnection(net_on_new_client);
  XD_SetOnConnectionLost(net_on_connection_lost);
  XD_SetOversizedMessageCB(50000,net_on_oversized_message);
  return this;
}

/* 0059a998 auto */
void NetSession::clear()

{
  int i;
  
  connected = 0;
  connected_to_server = 0;
  popup_in_progress = 0;
  listing_games = 0;
  session_phase = 0;
  found_games = 0;
  gamelist_view = 0;
  net_lobby_list = 0;
  dialog_stack_top = 0;
  server_conn_id = 0xffffffff;
  (*(i32 *)&net.xd_adapter_index) = 0;
  adapter_index = 0;
  net.active_protocol = 0xffffffff;
  net_protocol = 0xffffffff;
  mp_turn_civ = 0;
  player_has_quit = 0;
  poll_depth = 0;
  poll_depth_peak = 0;
  msgq_normal_peak = 0;
  msgq_priority_peak = 0;
  host_handoff_stage = 0;
  mp_simultaneous_moves = 0;
  tile_sync_enabled = 0;
  tile_batch_active = 0;
  chat_missed_message_count = 8;
  chat_unread_count = 0;
  timeout_seconds = 0x3c;
  move_anim_sound = 0xffffffff;
  move_anim_sound_arg = 0xffffffff;
  unit_create_busy = 0;
  unit_delete_busy = 0;
  city_create_busy = 0;
  city_delete_busy = 0;
  lock_map_busy = 0;
  unlock_map_busy = 0;
  ui_input_busy = 0;
  unit_stack_unlink_busy = 0;
  unit_stack_link_busy = 0;
  unit_stack_bottom_busy = 0;
  unit_relocate_busy = 0;
  city_acquire_wonder_busy = 0;
  civ_eliminate_busy = 0;
  civ_create_busy = 0;
  civ_check_destroyed_busy = 0;
  map_redraw_busy = 0;
  stacked_draw_count = 0;
  stacked_draw_first = 0;
  stacked_draw_last = 0;
  unit_stack_check_enabled = 0;
  stacked_draw_peak = 0;
  for (i = 0; i < 8; i++) {
    casualty_counts[i] = 0;
    casualty_reported_counts[i] = 0;
    casualty_log_first[i] = 0;
    casualty_log_next[i] = 0;
  }
  hotseat_human_players = 0;
  unit_gather_cargo_busy = 0;
  unit_lift_off_map_busy = 0;
  unit_delete_cargo_busy = 0;
  unit_destroy_redraw_busy = 0;
  diplo_meeting.mp_pending = 0;
  pending_human_players = 0;
  for (i = 0; i < 8; i++) {
    civ_new_player_flags[i] = 0;
    civ_turn_ended[i] = 0;
    civ_drop_requested[i] = 0;
  }
  blocked_input_kind = 0;
  drag_hold = 0;
  shutdown(1);
  net_reset_error_counts();
  return;
}

/* 0059ad40 auto */
void NetSession::destruct()

{
  i32 seh_frame;
  i32 seh_state;
  
  seh_state = 0;
  if (connection_title != 0) {
    operator_delete((void *)connection_title);
    connection_title = 0;
  }
  shutdown(1);
  net_reset_error_counts();
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0059adc8 */ { msg_queue.clear(); }
  return;
}

/* 0059adef auto */
i8 NetSession::init_transport(i32 protocol,int is_server)

{
  i32 saved_protocol;
  i8 result;
  UINT timeout_secs;
  int activate_error;
  int init_error;
  
  if (connected != '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_bconnectedtonet,(const char*)&str_d_ss_franklinton_netmgr_cpp,0x92);
  }
  saved_protocol = net.net_protocol;
  msg_queue.reset();
  clear();
  _memset((void *)net_player_count,0,0x270);
  switch(protocol) {
  case 0:
    init_error = XD_InitializeSocketsTCP
                        (3,0x1381,0x1382,(int)(char)is_server,7,NULL
                        );
    game.multiplayer = 3;
    if (connection_type == 0) {
      timeout_secs = GetPrivateProfileIntA
                        (str_civilization_gold_17,str_internet_timeout,-1,
                         str_civ_ini_17);
      if ((timeout_secs == 0xffffffff) || ((int)timeout_secs < 0x3c)) {
        WritePrivateProfileStringA
                  (str_civilization_gold_18,str_internet_timeout_2,((i8 *)&net_internet_timeout_default),
                   str_civ_ini_18);
        timeout_seconds = 0x3c;
      }
      else {
        timeout_seconds = timeout_secs;
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
        timeout_seconds = 0xf;
      }
      else {
        timeout_seconds = timeout_secs;
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
      timeout_seconds = 0xf;
    }
    else {
      timeout_seconds = timeout_secs;
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
      timeout_seconds = 0x1e;
    }
    else {
      timeout_seconds = timeout_secs;
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
      timeout_seconds = 0x1e;
    }
    else {
      timeout_seconds = timeout_secs;
    }
    init_error = XD_InitializeSerial((int)(char)is_server);
    break;
  default:
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_0_6,(const char*)&str_d_ss_franklinton_netmgr_cpp_2,0xf4);
  }
  if (init_error == 0) {
    if (((char)is_server == '\0') || (activate_error = XD_ActivateServer(), activate_error == 0)) {
      host_flag = (char)is_server;
      connected = 1;
      net.active_protocol = protocol;
      net_protocol = protocol;
      net_reset_error_counts();
      net_reset_msg_counts();
      _memset((void *)slot_addresses,0,0x20);
      _strncpy((char *)slot_addresses,((i8 *)&menu_prefs.net_ip_address),0x20);
      _memset((void *)slot_player_names,0,0x20);
      _strncpy((char *)slot_player_names,((i8 *)&menu_prefs.net_player_name),0x20);
      net_player_count[0] = 1;
      slot_active_flags[0] = 1;
      slot_host_flags[0] = host_flag;
      result = 1;
    }
    else {
      dbg_logf((const char *)&str_connecttonet_xd_activateserver_failed_re,activate_error);
      result = 0;
      net.net_protocol = saved_protocol;
    }
  }
  else {
    dbg_logf((const char *)&str_connecttonet_xd_initialize_failed_return,init_error);
    result = 0;
    net.net_protocol = saved_protocol;
  }
  return result;
}

/* 0059b293 auto */
/* WARNING: Removing unreachable block (ram,0x0059b3ca) */

void NetSession::shutdown(int delay_after)

{
  dbg_log((const char *)&str_disconnecting_from_network);
  if (host_flag != '\0') {
    delay_after = 0;
  }
  if (connected == '\0') {
    XD_ResetLibrary();
    dbg_log((const char *)&str_xd_resetlibrary_2);
  }
  else {
    free_game_list();
    if (connected_to_server != '\0') {
      XD_FlushSendBuffer(60000);
      dbg_log((const char *)&str_send_buffer_has_flushed);
      msg_queue.reset();
      net_popup_queue_clear();
      XD_CloseConnection();
      msg_queue.reset();
      net_popup_queue_clear();
      dbg_log((const char *)&str_xd_closeconnection);
      close_hook_nop();
    }
    switch(net_protocol) {
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
    msg_queue.reset();
    net_popup_queue_clear();
    if (0 < poll_depth_peak) {
      dbg_logf((const char *)&str_maximum_gnetmgr_poll_call_depth_d,poll_depth_peak);
    }
    if (0 < (int)msgq_priority_peak) {
      dbg_logf((const char *)&str_maximum_alpha_message_queue_load_d,(iptr)msgq_priority_peak);
    }
    if (0 < (int)msgq_normal_peak) {
      dbg_logf((const char *)&str_maximum_message_queue_load_d,(iptr)msgq_normal_peak);
    }
    if (stacked_draw_peak != 0) {
      dbg_logf((const char *)&str_maximum_draw_stack_load_d,(iptr)stacked_draw_peak);
    }
    XD_ResetLibrary();
    dbg_log((const char *)&str_xd_resetlibrary);
    if (delay_after != 0) {
      dbg_log((const char *)&str_disconnection_delay_2_seconds);
      ui_delay_ticks(0x78);
    }
  }
  connected = 0;
  connected_to_server = 0;
  listing_games = 0;
  server_conn_id = 0xffffffff;
  net_reset_error_counts();
  msg_queue.reset();
  net_popup_queue_clear();
  return;
}

/* 0059b55b auto */
void NetSession::close_hook_nop()

{
  return;
}

/* 0059b571 auto */
void NetSession::game_list_add(NetServerInfoMsg *msg)

{
  int cmp;
  NetGameInfo *entry;
  i32 now_ticks;
  NetGameInfo *prev;
  NetGameInfo *node2;
  i32 saved_value;
  
  if (listing_games != '\0') {
    node2 = found_games;
    saved_value = 0;
    for (; node2 != 0; node2 = node2->next) {
      cmp = _strncmp(node2->host_address,msg->game.host_address,0x20);
      if (cmp == 0) {
        if (found_games == node2) {
          found_games = node2->next;
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
    node2 = found_games;
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
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_pcur_null,(const char*)&str_d_ss_franklinton_netmgr_cpp_3,0x1cf);
    }
    crt_memcpy(entry,&msg->game,0x124);
    if (prev == 0) {
      entry->next = found_games;
      entry->prev = 0;
      if (found_games != 0) {
        found_games->prev = entry;
      }
      found_games = entry;
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
void NetSession::add_player(NetPlayerNodeMsg *player_info)

{
  int slot;
  
  slot = 0;
  while( true ) {
    if (6 < slot) {
      return;
    }
    if (net_slots[slot].active == '\0') break;
    slot++;
  }
  net_slots[slot].id = player_info->slot.id;
  _memset(net_slots[slot].address,0,0x20);
  _strncpy(net_slots[slot].address,&player_info->slot.address[0],0x20);
  net_slots[slot].is_host = 0;
  net_slots[slot].active = 1;
  net_slots[slot].civ = 0xffffffff;
  crt_strcpy(net_slots[slot].name,(const char *)((const void *)player_info->slot.name));
  net_slots[slot].field_44 = 0;
  net_slots[slot].lobby_id = 0;
  net_player_count[0] = net_player_count[0] + 1;
  players_awaited--;
  return;
}

/* 0059b96a auto */
void NetSession::remove_player(int player_id)

{
  u8 *civ_slot_ptr;
  int slot;
  
  slot = 0;
  while ((slot < 7 && (net_slots[slot].id != player_id))) {
    slot++;
  }
  if ((slot < 7) && (net_slots[slot].active != '\0')) {
    if ((0 < net_slots[slot].civ) &&
       (net_slots[slot].civ < 8)) {
      game.human_players =
           ~(byte)(1 << ((byte)net_slots[slot].civ & 0x1f)) & game.human_players
      ;
    }
    net_slots[slot].id = 0;
    net_slots[slot].address[0] = 0;
    net_slots[slot].is_host = 0;
    net_slots[slot].active = 0;
    civ_slot_ptr = (u8 *)net_civ_slots + net_slots[slot].civ * 4;
    civ_slot_ptr[0] = 0xff;
    civ_slot_ptr[1] = 0xff;
    civ_slot_ptr[2] = 0xff;
    civ_slot_ptr[3] = 0xff;
    net_slots[slot].civ = 0xffffffff;
    net_player_count[0] = net_player_count[0] - 1;
  }
  return;
}

/* 0059baf0 auto */
void NetSession::free_game_list()

{
  NetGameInfo *next;
  
  while (found_games != 0) {
    next = found_games->next;
    operator_delete(found_games);
    found_games = next;
  }
  return;
}

/* 0059c276 auto */
void __cdecl net_reset_msg_counts(void)

{
  int i;
  
  for (i = 0; i < 0xa9; i++) {
    net.nm_recv_counts_by_type[i] = 0;
  }
  return;
}

/* 0059c2b8 auto */
void __cdecl net_reset_error_counts(void)

{
  int i;
  
  net.lost_conn_total = 0;
  for (i = 0; i < 7; i++) {
    net.lost_conn_by_player[i] = 0;
  }
  return;
}

/* 0059c301 auto */
void __cdecl net_modal_idle_poll(void)

{
  net.process_messages(1,0);
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
  _strncpy(info->host_address,((i8 *)&menu_prefs.net_ip_address),0x20);
  _memset(info->game_name,0,0x20);
  _strncpy(info->game_name,((i8 *)&net.net_game_name),0x20);
  _memset(info->host_name,0,0x20);
  _strncpy(info->host_name,((i8 *)&menu_prefs.net_player_name),0x20);
  info->open_game = mp_options.open_game;
  info->difficulty = game.difficulty;
  info->barbarian_activity = game.barbarian_activity;
  if (game.civs_alive == '\0') {
    info->civs_alive = *(i8 *)(bit_masks + (uint)game.num_enemies + 2);
  }
  else {
    info->civs_alive = game.civs_alive;
  }
  info->player_mask = *(i8 *)(bit_masks + net.net_player_count[0]);
  info->turn_time_limit_ms = civ_saved_cursor_x_block.mp_turn_time_limit_ms;
  crt_strcpy(info->scenario_name,((i8 *)&scenario.name));
  info->double_moves = mp_doubles.moves;
  info->double_production = mp_doubles.production;
  info->game_flags = game.game_flags;
  info->first_launch_turn = game.first_launch_turn;
  info->year_increment = scenario.year_increment[0];
  info->start_year = scenario.start_year;
  info->turn = game.turn;
  info->simultaneous = mp_options.simultaneous_setting;
  info->prev = 0;
  info->next = info->prev;
  info->list_id = 0;
  info->last_seen = info->list_id;
  info->map_width = map.width;
  info->map_height = map.height;
  return;
}

/* 0059d080 auto */
NetMsgHeader *NetPlayerInfoMsg::init()

{
  iptr j;
  int i;
  
  _memset(this,0,sizeof *this);   /* (BF-286) nothing of the last names after the new ones */
  hdr.header_init(0x2e);
  civ = 0;
  leader_index = 0;
  gender = 0;
  leader_name[0] = 0;
  tribe_name[0] = 0;
  adjective[0] = 0;
  city_style = 0;
  for (i = 0; i < 7; i++) {
    for (j = 0; j < 0x18; j++) {
      govt_titles[i][j] = 0;
    }
  }
  has_names = 0;
  has_titles = 0;
  size = 0x118;
  return &hdr;
}

/* 005a9f30 auto */
void __cdecl net_host_wait_production_idle(void)

{
  bool all_done;
  int civ;
  
  net.process_messages(1,0);
  if ((net.nm_production_complete_count != 0) || (net.net_player_count[0] < 2)) {
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
      net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
      ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    }
  }
  return;
}

/* 005aa004 auto */
void __cdecl net_host_wait_human_moves_idle(void)

{
  bool all_done;
  int civ;
  
  net.process_messages(1,0);
  if ((net.nm_human_move_complete_count != 0) || (net.net_player_count[0] < 2)) {
    all_done = true;
    for (civ = 1; civ < 8; civ++) {
      if ((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (dialog_text_font_obj_block.net_civ_phase_done[civ] == 0)) {
        all_done = false;
        break;
      }
    }
    if ((all_done) || (net.net_player_count[0] < 2)) {
      net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
      ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    }
  }
  return;
}

/* 005aa0e5: written by hand in src/game/bigframes.cpp */

/* 005ab0a3 auto */
void __cdecl net_client_wait_production_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_ai_movement_count != 0) || (net.nm_end_sequence_count != 0)) || (turn_loop.active == '\0')) ||
     (net.lost_conn_total != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 005ab120 auto */
void __cdecl net_client_wait_ai_moves_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_human_movement_count != 0) || (net.nm_end_sequence_count != 0)) || (turn_loop.active == '\0')) ||
     (net.lost_conn_total != 0)) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 005ab19d auto */
void __cdecl net_client_wait_human_moves_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_production_begin_count != 0) || (net.host_handoff_stage == 1)) || (net.nm_end_sequence_count != 0)) ||
     (((net.player_has_quit == 1 || (turn_loop.active == '\0')) || (net.lost_conn_total != 0)))) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* 005ab23a auto */
void __cdecl net_client_hotwait_idle(void)

{
  net.process_messages(1,0);
  net_heartbeat_check(net.slot_civs[0]);
  if ((((net.nm_production_begin_count != 0) || (net.host_handoff_stage != '\0')) || (net.nm_end_sequence_count != 0)) ||
     (((net.player_has_quit != '\0' || (turn_loop.active == '\0')) || (net.lost_conn_total != 0)))) {
    net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
    ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
  }
  return;
}

/* Part of net_client_turn_loop (005ab2d5): our turn played by the player:
   production, the wait for the AI's and the other humans' moves, the
   our-turn-to-move popup (with the casualties), then the moves. 1: a
   connection lost (dialog2 destroyed; the caller returns 0). */
static int net_client_human_turn(Dialog &dialog2,i8 &turn_ok)
{
  iptr str;
  int status;
  int new_casualties;
  i32 seh_state;
  
  if (turn_loop.resume_civ == '\0') {
    civ_check_throne_room_upgrade();
  }
  if ((turn_loop.resume_civ == '\0') || (turn_resume_new_game != 0)) {
    civ_process_turn(human_civ[0]);
    net_send_state_diff(0xff,2,0,0,0);
  }
  XD_FlushSendBuffer(60000);
  game_turn_start_popups();
  net_send_message(NM_PRODUCTION_COMPLETE,0,human_civ[0],0,0,0,0,0,0,0);
  net_heartbeat_send(net.slot_civs[0]);
  dlg_style.pending_idle_callback = net_client_wait_production_idle;
  popup_game_with_sprite(str_waitproduction_3,(Sprite *)&popup_info_icon_sprite,0);
  while (((net.nm_ai_movement_count == 0 && (net.nm_end_sequence_count == 0)) && (turn_loop.active != '\0'))) {
    net.process_messages(1,0);
    net_heartbeat_check(net.slot_civs[0]);
    status = net_check_lost_connections();
    if (status != 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
      return 1;
    }
  }
  if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
    net.nm_ai_movement_count = 0;
    net_heartbeat_send(net.slot_civs[0]);
    dlg_style.pending_idle_callback = net_client_wait_ai_moves_idle;
    popup_game_with_sprite(str_waitaimoves,(Sprite *)&popup_info_icon_sprite,0);
    while (((net.nm_human_movement_count == 0 && (net.nm_end_sequence_count == 0)) && (turn_loop.active != '\0'))) {
      net.process_messages(1,0);
      net_heartbeat_check(net.slot_civs[0]);
      status = net_check_lost_connections();
      if (status != 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        return 1;
      }
    }
    if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
      net.nm_human_movement_count = 0;
      if ((civ_view.intro_shown[human_civ[0]] == 0) && (autoplay.active == 0)) {
        civ_intro_popup(human_civ[0]);
        civ_view.intro_shown[human_civ[0]] = 1;
      }
      new_casualties = net.casualty_counts[human_civ[0]] -
                 net.casualty_reported_counts[human_civ[0]];
      if (autoplay.active == 0) {
        dlg_style.pending_idle_callback = turn_popup_timeout_idle;
        str = civ_leader_title(human_civ[0]);
        text_set_string_arg(0,(const char *)str);
        str = (iptr)(civ_leader_name(human_civ[0]));
        text_set_string_arg(1,(const char *)str);
        if (new_casualties < 1) {
          dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_ourturntomove_4);
          dialog2.add_sprite((Sprite *)&popup_info_icon_sprite,0,0);
          dialog2.set_image_zoom(0);
          input_flush_events();
          snd_play_effect(SND_NEWONDER,0,0,0);
          dialog2.show_modal(0);
        }
        else {
          net.casualty_reported_counts[human_civ[0]] =
               net.casualty_counts[human_civ[0]];
          text_set_number_arg(0,new_casualties);
          if (new_casualties == 1) {
            dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_casualty_5);
          }
          else {
            dialog2.load_text_noflags((const char *)&str_game_7,(const char *)&str_casualties_5);
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
      if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
        mp_turn_timer_start(1);
      }
      game_player_move_loop();
    }
    else {
      net.nm_end_sequence_count = 0;
    }
  }
  else {
    turn_ok = 0;
  }
  return 0;
}

/* Part of net_client_turn_loop (005ab2d5): our turn played by the AI
   (autoplay). 1: a connection lost (dialog2 destroyed; the caller returns 0).
   */
static int net_client_autoplay_turn(Dialog &dialog2,i8 &turn_ok)
{
  SHORT esc_state;
  int status;
  i32 seh_state;
  
  map_views_block.saved_human_players = (uint)game.human_players;
  game.human_players = '\0';
  if ((turn_loop.resume_civ == '\0') || (turn_resume_new_game != 0)) {
    civ_process_turn(human_civ[0]);
  }
  net_send_message(NM_PRODUCTION_COMPLETE,0,human_civ[0],0,0,0,0,0,0,0);
  net_heartbeat_send(net.slot_civs[0]);
  dlg_style.pending_idle_callback = net_client_wait_production_idle;
  popup_game_with_sprite(str_waitproduction_2,(Sprite *)&popup_info_icon_sprite,0);
  while (((net.nm_ai_movement_count == 0 && (net.nm_end_sequence_count == 0)) && (turn_loop.active != '\0'))) {
    net.process_messages(1,0);
    net_heartbeat_check(net.slot_civs[0]);
    status = net_check_lost_connections();
    if (status != 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
      return 1;
    }
  }
  if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
    net.nm_ai_movement_count = 0;
    if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
      mp_turn_timer_start(1);
    }
    ai_process_civ_units();
    game.human_players = (*(u8 *)&map_views_block.saved_human_players);
    esc_state = GetAsyncKeyState(0x1b);
    if (((int)esc_state & 0x8001U) != 0) {
      autoplay.turns_left = 1;
    }
  }
  else {
    turn_ok = 0;
  }
  return 0;
}

/* 005ab2d5 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl net_client_turn_loop(void)

{
  char resume_civ;
  iptr str;
  int status;
  i32 ret;
  iptr slot_civ;
  int civ;
  iptr unit;
  Dialog dialog2;
  i8 turn_ok;
  int centered;
  int is_first_turn;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  is_first_turn = 1;
  dialog2.Dialog::construct(0x4000);
  seh_state = 0;
  turn_ok = 1;
  slot_civ = net.slot_civs[0];
  str = (iptr)(civ_name_plural(net.slot_civs[0]));
  dbg_logf_str_int((const char *)&str_server_s_d_4,(const char *)str,slot_civ);
  for (civ = 1; civ < 8; civ++) {
    if (((net.net_slots[net.net_civ_slots[civ]].id != 0) &&
        ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
      status = civ;
      str = (iptr)(civ_name_plural(civ));
      dbg_logf_str_int((const char *)&str_client_s_d_4,(const char *)str,status);
    }
  }
  net_reset_error_counts();
  human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
  turn_loop.resume_civ = saved_turn.civ;
  turn_resume_new_game = (int)saved_turn.resume_new_game;
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  menu_update_states();
  game.player_civ = (byte)human_civ[0];
  game.turn_civ = (byte)human_civ[0];
  ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
  centered = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit].x,(int)units[unit].y);
      centered++;
      break;
    }
  }
  if (centered == 0) {
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
  resume_civ = turn_loop.resume_civ;
  if ((1 << ((byte)human_civ[0] & 0x1f) & (uint)game.human_players) == 0) {
    net_heartbeat_send(net.slot_civs[0]);
    dlg_style.pending_idle_callback = net_client_hotwait_idle;
    popup_game_sprite_flags(str_clienthotwait_2,0x2000000,(Sprite *)&popup_info_icon_sprite,0);
    resume_civ = turn_loop.resume_civ;
  }
  do {
    turn_loop.resume_civ = resume_civ;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
        unit_exhaust_moves(unit);
      }
    }
    if (is_first_turn == 0) {
      net_heartbeat_send(net.slot_civs[0]);
      dlg_style.pending_idle_callback = net_client_wait_human_moves_idle;
      popup_game_with_sprite(str_waithumanmoves_2,(Sprite *)&popup_info_icon_sprite,0);
    }
    net_heartbeat_send(net.slot_civs[0]);
    while ((((net.nm_production_begin_count == 0 && (net.host_handoff_stage == '\0')) && (net.nm_end_sequence_count == 0)) &&
           ((net.player_has_quit == '\0' && (turn_loop.active != '\0'))))) {
      net.process_messages(1,0);
      net_heartbeat_check(net.slot_civs[0]);
      status = net_check_lost_connections();
      if (status != 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        ret = 0;
        return ret;
      }
    }
    if (net.host_handoff_stage != '\0') {
      status = net_client_transfer_server();
      if (status == 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        ret = 0;
        return ret;
      }
      if (net.host_flag != '\0') {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
        ret = 1;
        return ret;
      }
      if (((net.nm_production_begin_count == 0) && (net.nm_end_sequence_count == 0)) &&
         ((net.player_has_quit == '\0' && (resume_civ = turn_loop.resume_civ, turn_loop.active != '\0'))))
      continue;
    }
    if ((net.nm_end_sequence_count == 0) && (turn_loop.active != '\0')) {
      net.nm_start_civ_count = 0;
      net.nm_production_begin_count = 0;
      human_civ[0] = (((i32 *)&net.slot_civs))[net.local_slot * 0x15];
      game.player_civ = (byte)human_civ[0];
      game.turn_civ = (byte)human_civ[0];
      if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
        mp_timer.seconds_left = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
        map_views_block.main_view_timer_cache_i32 = 0xffffffff;
      }
      while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
      net.mp_turn_civ = human_civ[0];
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      map_redraw_all_views(human_civ[0],1);
      status_draw_info(1);
      status_draw_civ_color(human_civ[0]);
      if (autoplay.active == 0) {
        if (net_client_human_turn(dialog2,turn_ok) != 0) {
          ret = 0;
          return ret;
        }
      }
      else {
        if (net_client_autoplay_turn(dialog2,turn_ok) != 0) {
          ret = 0;
          return ret;
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
      turn_loop.active = '\0';
    }
    net_send_state_diff(0xff,2,0,0,0);
    net_send_message(NM_HUMAN_MOVE_COMPLETE,0,human_civ[0],0,0,0,0,0,0,0);
    XD_FlushSendBuffer(60000);
    saved_turn.civ = '\0';
    turn_loop.resume_civ = '\0';
    saved_turn.resume_new_game = 0;
    turn_resume_new_game = 0;
    is_first_turn = 0;
    resume_civ = '\0';
    if (turn_loop.active == '\0') {
      net_send_message(NM_QUIT_REQUEST,0,human_civ[0],0,0,0,0,0,0,0);
      XD_FlushSendBuffer(60000);
      net.shutdown(1);
      if (game_over_reason != 0) {
        game_handle_game_over();
      }
      seh_state = 0xffffffff;
      /* inlined unwind funclet 005abfdb */ { dialog2.Dialog::destruct(); }
      ret = 0;
      return ret;
    }
  } while( true );
}

/* 005b9ec6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_batch_begin(void)

{
  if (2 < game.multiplayer) {
    net.tile_sync_enabled = 0;
    net.tile_batch_active = 1;
    _memset(((i8 *)&net_batch_buffer),0,0x400);
    net_batch_buffer[0] = 0;
    net_batch.used_dwords = 1;
  }
  return;
}

/* 005b9f1c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl net_batch_flush(void)

{
  if (2 < game.multiplayer) {
    net.tile_sync_enabled = 1;
    net.tile_batch_active = 0;
    if (net.host_flag == '\0') {
      if (1 < net_batch.used_dwords) {
        net_send_message(NM_MAP_PACKETS,0,0,0,0,0,0,0,0,0);
        XD_FlushSendBuffer(5000);
        _memset(((i8 *)&net_batch_buffer),0,0x400);
        net_batch_buffer[0] = 0;
        net_batch.used_dwords = 1;
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
net_batch_add(int record_type,i32 x,i32 y,i32 arg1,i32 arg2,
            i32 arg3)

{
  if (net.host_flag != '\0') {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_gnetmgr_bserver_3,(const char*)&str_d_ss_franklinton_map_cpp,0x3de);
  }
  if (0x100 - net_batch.used_dwords < net_batch.record_sizes[record_type] + 1) {
    net_send_message(NM_MAP_PACKETS,0,0,0,0,0,0,0,0,0);
    XD_FlushSendBuffer(5000);
    _memset(((i8 *)&net_batch_buffer),0,0x400);
    net_batch_buffer[0] = 0;
    net_batch.used_dwords = 1;
  }
  net_batch_buffer[0]++;
  net_batch_buffer[net_batch.used_dwords] = record_type;
  net_batch.used_dwords++;
  switch(record_type) {
  case 0:
  case 2:
    net_batch_buffer[net_batch.used_dwords] = x;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = y;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg1;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg2;
    net_batch.used_dwords++;
    break;
  case 1:
  case 3:
  case 4:
  case 5:
  case 6:
    net_batch_buffer[net_batch.used_dwords] = x;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = y;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg1;
    net_batch.used_dwords++;
    break;
  case 7:
    net_batch_buffer[net_batch.used_dwords] = x;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = y;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg1;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg2;
    net_batch.used_dwords++;
    net_batch_buffer[net_batch.used_dwords] = arg3;
    net_batch.used_dwords++;
  }
  return;
}

/* 005ba206 auto */
void __cdecl net_apply_tile_updates(u8 *batch)

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
    if (net.host_flag == '\0') {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_gnetmgr_bserver_4,(const char*)&str_d_ss_franklinton_map_cpp_2,0x416);
    }
    saved_sync = net.tile_sync_enabled;
    net.tile_sync_enabled = 0;
    count_offset = pos * 4;
    pos++;
    for (records_left = *(int *)(batch + count_offset); 0 < records_left; records_left--) {
      record_type = *(int *)(batch + pos * 4);
      for (arg_idx = 0; pos = pos + 1, arg_idx < net_batch.record_sizes[record_type] - 1;
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
        /* (port) as NM_FEAT_LOC_SET: a square off the map is ignored */
        if (map_in_bounds(args[0],args[1]) != 0) {
          known_set(args[0],args[1],args[2],args[3],args[4],1);
        }
      }
    }
    net.tile_sync_enabled = saved_sync;
    net_send_state_diff(0xff,2,0,0,0);
  }
  return;
}
