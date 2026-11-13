/* The game, windows: the menu bar and its commands: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"
#include "lang.h"

/* 004e01b0 auto */
void __cdecl menu_wonder_movies(void)

{
  iptr wonder_name;
  i32 flags;
  iptr wonder_id;
  Dialog dialog2;
  iptr wonder;
  iptr last_choice;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  last_choice = 0;
  while( true ) {
    dialog2.Dialog::reset(0x1001);
    dialog2.set_width(0x1b8);
    dialog2.set_listbox(0x10,0x1b8,0);
    for (wonder = 0; wonder < 0x1c; wonder++) {
      flags = 0;
      wonder_id = wonder;
      wonder_name = (iptr)label_string(improvements[wonder + 0x27].name);
      dialog2.add_option((const char *)wonder_name,wonder_id,flags);
    }
    dialog2.set_default_choice(last_choice);
    wonder = dialog2.show_modal(0);
    last_choice = wonder;
    if (wonder < 0) break;
    wonder_show_movie(wonder);
  }
  return;
}

/* 004e02ef auto */
void __cdecl ui_reset_window_layout(void)

{
  int left;
  int client_height;
  int chat_font_height;
  int parley_font_height;
  int height2;
  int width2;
  int view;
  
  main_frame_wnd_block.main_frame_wnd.maximize();
  city_window_init_size();
  ((WndBase *)((i8 *)&city_wnd.wnd))->show_restored();
  ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  city_window_close();
  minimap_calc_rect();
  ((WndBase *)((i8 *)&minimap_state.wnd))->show_restored();
  minimap_hidden = 0;
  ((WndBase *)((i8 *)&minimap_state.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
  status_calc_wnd_rect();
  ((WndBase *)((i8 *)&status_wnd.wnd))->show_restored();
  status_bar.wnd_hidden = 0;
  ((WndBase *)((i8 *)&status_wnd.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.status_wnd_rect);
  ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
  if (parley_wnd_block.large_layout == 0) {
    width2 = 0x208;
    height2 = 0x14f;
    parley_font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    width2 = 800;
    height2 = 0x1cc;
    parley_font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  parley_wnd_block.caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + parley_font_height;
  width2 = width2 + (panel_layout.padding_x + panel_layout.margin_x) * 2 + 6;
  height2 = height2 + panel_layout.margin_y * 2 + (int)parley_wnd_block.caption_height * 2 + 0xe;
  left = main_frame_wnd_block.main_frame_wnd.client_width();
  left = (left >> 1) - (width2 >> 1);
  parley_wnd_block.wnd_x = left;
  client_height = main_frame_wnd_block.main_frame_wnd.client_height();
  parley_wnd_block.wnd_y = (client_height >> 1) - (height2 >> 1);
  SetRect((LPRECT)parley_wnd_block.init_rect,left,(int)parley_wnd_block.wnd_y,left + width2,(int)parley_wnd_block.wnd_y + height2);
  ((WndBase *)((i8 *)&parley_wnd_block.window))->move_to_rect((RECT *)parley_wnd_block.init_rect);
  if (chat_wnd_block.large_layout == 0) {
    width2 = 0x208;
    height2 = 0x14f;
    chat_font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    width2 = 800;
    height2 = 0x1cc;
    chat_font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  chat_wnd_block.caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + chat_font_height;
  width2 = width2 + (panel_layout.padding_x + panel_layout.margin_x) * 2 + 6;
  height2 = height2 + panel_layout.margin_y * 2 + (int)chat_wnd_block.caption_height * 2 + 0xe;
  left = main_frame_wnd_block.main_frame_wnd.client_width();
  left = (left >> 1) - (width2 >> 1);
  chat_wnd_block.wnd_x = left;
  client_height = main_frame_wnd_block.main_frame_wnd.client_height();
  chat_wnd_block.wnd_y = (client_height >> 1) - (height2 >> 1);
  SetRect((LPRECT)chat_wnd_block.init_rect,left,(int)chat_wnd_block.wnd_y,left + width2,(int)chat_wnd_block.wnd_y + height2);
  ((WndBase *)((i8 *)&chat_wnd_block.window))->move_to_rect((RECT *)chat_wnd_block.init_rect);
  for (view = 1; view < 8; view++) {
    if (map_views_block.map_view_recs[view].open != 0) {
      map_views_block.map_view_recs[view].open = 0;
      map_views_block.map_view_recs[view].wnd_destroy_with_surface();
    }
  }
  mapview_calc_rect();
  ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->show_restored();
  ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->move_to_rect((RECT *)&map_view_saved_rects_block.map_view_saved_rects);
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  ui_flush_input();
  return;
}

/* 004e068d auto */
void __cdecl menu_load_game(void)

{
  byte music_was_on;
  int result;
  int view;
  
  mp_turn_timer_stop(1);
  city_window_close();
  parley_wnd_close();
  chat_wnd_close();
  pedia_close();
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  music_was_on = (byte)game.game_options & OPT_MUSIC;
  result = game_load_dialog(0,0,0);
  if (result == 0) {
    if (game.multiplayer != '\0') {
      load_game_pick_civ(1);
    }
    (*(byte *)&saved_turn.civ) = (byte)(*(i8 *)&human_civ);
    (*(byte *)&turn_loop.resume_civ) = (byte)(*(i8 *)&human_civ);
    game.multiplayer = '\0';
    game.human_players = (u8)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
    if (((byte)game.game_options & OPT_MUSIC) == 0) {
      snd_cd_stop();
    }
    else {
      if (music_was_on == 0) {
        snd_cd_mci_stop();
        snd_cd_close();
        snd_cd_open();
        snd_cd_detect_layout();
      }
      snd_sync_music_option();
    }
    civ_check_password((*(i8 *)&human_civ));
    if (destroyed_civ_count_block.saved_layout_valid == 0) {
      ui_reset_window_layout();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->show_restored();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->move_to_rect((RECT *)&map_view_saved_rects_block.map_view_saved_rects);
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
      for (view = 1; view < 8; view++) {
        if (map_views_block.map_view_recs[view].open != 0) {
          map_views_block.map_view_recs[view].create();
          ((WndBase *)&map_views_block.map_view_recs[view].window)->move_to_rect(
                     ((RECT *)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + view * 0x10)));
          map_views_block.map_view_recs[view].update_title();
          ((SurfaceWindow *)&map_views_block.map_view_recs[view])->repaint();
        }
      }
    }
    else {
      main_frame_wnd_block.main_frame_wnd.maximize();
      ((WndBase *)((i8 *)&city_wnd.wnd))->show_restored();
      ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
      city_window_close();
      ((WndBase *)((i8 *)&minimap_state.wnd))->show_restored();
      minimap_hidden = 0;
      ((WndBase *)((i8 *)&minimap_state.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
      ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
      ((WndBase *)((i8 *)&status_wnd.wnd))->show_restored();
      status_bar.wnd_hidden = 0;
      ((WndBase *)((i8 *)&status_wnd.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.status_wnd_rect);
      ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->show_restored();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->move_to_rect((RECT *)&map_view_saved_rects_block.map_view_saved_rects);
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
      for (view = 1; view < 8; view++) {
        if (map_views_block.map_view_recs[view].open != 0) {
          map_views_block.map_view_recs[view].create();
          ((WndBase *)&map_views_block.map_view_recs[view].window)->move_to_rect(
                     ((RECT *)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + view * 0x10)));
          map_views_block.map_view_recs[view].update_title();
          ((SurfaceWindow *)&map_views_block.map_view_recs[view])->repaint();
        }
      }
      ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
    }
    moving_units_mode = 0;
    ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
    ui_redraw_all_maps(1);
    ui_focus_active_unit(0);
    menu_update_states();
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->load_indexes();
    ui_flush_input();
    if ((game.multiplayer != '\0') && (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0)) {
      mp_turn_timer_start(1);
    }
  }
  else {
    game.multiplayer = '\0';
    mp_turn_timer_resume(1);
  }
  return;
}

/* 004e0a8c auto */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __cdecl dlg_check_set_bool(i32 index,int value)

{
  dlg_check_set(index,(uint)(value != 0));
  return;
}

/* 004e0ab0 auto */
void __cdecl menu_game_options(void)

{
  uint music_was_on;
  int result;
  
  music_was_on = game.game_options & OPT_MUSIC;
  dlg_checks_clear();
  dlg_check_set_bool(0,game.game_options & OPT_SOUND);
  dlg_check_set_bool(1,game.game_options & OPT_MUSIC);
  dlg_check_set_bool(2,game.game_options & OPT_ALWAYS_WAIT);
  dlg_check_set_bool(3,game.game_options & OPT_AUTOSAVE);
  dlg_check_set_bool(4,game.game_options & OPT_SHOW_ENEMY_MOVES);
  dlg_check_set_bool(5,game.game_options & OPT_NO_PAUSE_ENEMY);
  dlg_check_set_bool(6,game.game_options & OPT_FAST_PIECE_SLIDE);
  dlg_check_set_bool(7,game.game_options & OPT_INSTANT_ADVICE);
  dlg_check_set_bool(8,game.game_options & OPT_TUTORIAL);
  dlg_check_set_bool(9,game.game_options & OPT_MOVE_WITH_MOUSE);
  dlg_check_set_bool(10,game.game_options & OPT_ENTER_CLOSES_CITY);
  text_set_string_arg(0,(const char *)str_ptr_5_4_0f_multiplayer_26_march_99);
  crt_strcat(((i8 *)&text_args.strings),((i8 *)&game_options_version_sep_str));
  crt_strcat(((i8 *)&text_args.strings),(*(i8 * *)str_ptr_5_4_0f_multiplayer_26_march_99_rest));
  result = popup_game_flags((const char *)&str_gameoptions,1);
  if (result == 0) {
    game.game_options = game.game_options & 0xffff8027;
    result = dlg_check_get(0);
    if (result != 0) {
      game.game_options = game.game_options | OPT_SOUND;
    }
    result = dlg_check_get(1);
    if (result == 0) {
      snd_cd_stop();
    }
    else {
      game.game_options = game.game_options | OPT_MUSIC;
      if (music_was_on == 0) {
        snd_cd_mci_stop();
        snd_cd_close();
        snd_cd_open();
        snd_cd_detect_layout();
      }
      snd_sync_music_option();
    }
    result = dlg_check_get(2);
    if (result != 0) {
      game.game_options = game.game_options | OPT_ALWAYS_WAIT;
    }
    result = dlg_check_get(3);
    if (result != 0) {
      game.game_options = game.game_options | OPT_AUTOSAVE;
    }
    result = dlg_check_get(4);
    if (result != 0) {
      game.game_options = game.game_options | OPT_SHOW_ENEMY_MOVES;
    }
    result = dlg_check_get(5);
    if (result != 0) {
      game.game_options = game.game_options | OPT_NO_PAUSE_ENEMY;
    }
    result = dlg_check_get(6);
    if (result != 0) {
      game.game_options = game.game_options | OPT_FAST_PIECE_SLIDE;
    }
    result = dlg_check_get(7);
    if (result != 0) {
      game.game_options = game.game_options | OPT_INSTANT_ADVICE;
    }
    result = dlg_check_get(8);
    if (result != 0) {
      game.game_options = game.game_options | OPT_TUTORIAL;
    }
    result = dlg_check_get(9);
    if (result != 0) {
      game.game_options = game.game_options | OPT_MOVE_WITH_MOUSE;
    }
    result = dlg_check_get(10);
    if (result != 0) {
      game.game_options = game.game_options | OPT_ENTER_CLOSES_CITY;
    }
    prefs.game_options = game.game_options;
    prefs_save_civ2dat();
  }
  return;
}

/* 004e0d71 auto */
void __cdecl menu_graphic_options(void)

{
  uint hires_was_on;
  int result;
  
  hires_was_on = game.game_options & OPT_ANIMATED_HERALDS;
  dlg_checks_clear();
  dlg_check_set_bool(0,game.game_options & OPT_THRONE_ROOM);
  dlg_check_set_bool(1,game.game_options & OPT_DIPLOMACY_SCREEN);
  dlg_check_set_bool(2,game.game_options & OPT_ANIMATED_HERALDS);
  dlg_check_set_bool(3,game.game_options & OPT_PEDIA_ADVANCES);
  dlg_check_set_bool(4,game.game_options & OPT_HIGH_COUNCIL);
  dlg_check_set_bool(5,game.game_options & OPT_WONDER_MOVIES);
  result = popup_game_flags((const char *)&str_graphicoptions,1);
  if (result == 0) {
    game.game_options = game.game_options & 0xffc0ffff;
    result = dlg_check_get(0);
    if (result != 0) {
      game.game_options = game.game_options | OPT_THRONE_ROOM;
    }
    result = dlg_check_get(1);
    if (result != 0) {
      game.game_options = game.game_options | OPT_DIPLOMACY_SCREEN;
    }
    result = dlg_check_get(2);
    if (result != 0) {
      if (hires_was_on == 0) {
        result = mem_check_available(0x800000);
        if (result == 0) {
          popup_game((const char *)&str_lowmemory);
        }
      }
      game.game_options = game.game_options | OPT_ANIMATED_HERALDS;
    }
    result = dlg_check_get(3);
    if (result != 0) {
      game.game_options = game.game_options | OPT_PEDIA_ADVANCES;
    }
    result = dlg_check_get(4);
    if (result != 0) {
      game.game_options = game.game_options | OPT_HIGH_COUNCIL;
    }
    result = dlg_check_get(5);
    if (result != 0) {
      game.game_options = game.game_options | OPT_WONDER_MOVIES;
    }
    prefs.game_options = game.game_options;
    prefs_save_civ2dat();
  }
  return;
}

/* 004e0f18 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl menu_multiplayer_options(void)

{
  int result;
  int choice;
  
  dlg_checks_clear();
  dlg_check_set_bool(0,(int)(short)(*(ushort *)&menu_prefs.clear_chat_on_new_game));
  dlg_check_set_bool(1,(int)(short)(*(ushort *)&menu_prefs.clear_chat_on_boot));
  if (net.host_flag != '\0') {
    dlg_check_set_bool(2,(int)(short)(*(ushort *)&mp_doubles.production));
    dlg_check_set_bool(3,(int)(short)(*(ushort *)&mp_doubles.moves));
  }
  if (((game.multiplayer < 3) || (net.host_flag == '\0')) && (2 < game.multiplayer)) {
    choice = popup_game_flags((const char *)&str_multiplayeroptions2,1);
  }
  else {
    choice = popup_game_flags((const char *)&str_multiplayeroptions,1);
  }
  if (choice == 0) {
    result = dlg_check_get(0);
    (*(ushort *)&menu_prefs.clear_chat_on_new_game) = (ushort)(result != 0);
    result = dlg_check_get(1);
    (*(ushort *)&menu_prefs.clear_chat_on_boot) = (ushort)(result != 0);
    prefs_save_civ2dat();
    if (((2 < game.multiplayer) && (net.host_flag != '\0')) || (game.multiplayer < 3)) {
      result = dlg_check_get(2);
      (*(ushort *)&menu_prefs.mp_double_production) = (ushort)(result != 0);
      result = dlg_check_get(3);
      (*(ushort *)&menu_prefs.mp_double_moves) = (ushort)(result != 0);
      if (((*(ushort *)&mp_doubles.production) != (*(ushort *)&menu_prefs.mp_double_production)) || ((*(ushort *)&mp_doubles.moves) != (*(ushort *)&menu_prefs.mp_double_moves))) {
        if ((2 < game.multiplayer) && (1 < net.net_player_count[0])) {
          crt_strcpy(((i8 *)&text_args.strings),((i8 *)&net.local_player_name));
          str_clear((char *)&text_args.string1);
          str_append_label(text_args.string1,0x350 - (uint)((*(ushort *)&menu_prefs.mp_double_production) == 0));
          crt_strcat(((i8 *)&text_args.string1),((i8 *)&mp_options_vote_sep_str));
          str_append_label(text_args.string1,0x352 - (uint)((*(ushort *)&menu_prefs.mp_double_moves) == 0));
          net.vote_replies_pending = net.net_player_count[0] - 1;
          net.vote_status = 0;
          net_send_text_args(0x43,0xff,2,0,0,0);
          dlg_style.pending_idle_callback = net_vote_wait_idle_cb;
          dialog_state.timeout_start = timer_ticks();
          result = popup_game_sprite_flags(str_pmchangeserver,0x2000001,(Sprite *)&popup_info_icon_sprite,0);
          if (((result < 0) || (net.vote_status == -1)) || (net.vote_replies_pending != 0)) {
            (*(ushort *)&menu_prefs.mp_double_production) = (*(ushort *)&mp_doubles.production);
            (*(ushort *)&menu_prefs.mp_double_moves) = (*(ushort *)&mp_doubles.moves);
            net_send_text_args(0x44,0xff,0,0,0,0);
            popup_game_with_sprite(str_pmchangeno,(Sprite *)&popup_info_icon_sprite,0);
            return;
          }
        }
        (*(ushort *)&mp_doubles.production) = (*(ushort *)&menu_prefs.mp_double_production);
        (*(ushort *)&mp_doubles.moves) = (*(ushort *)&menu_prefs.mp_double_moves);
        rules_apply_double_moves();
        if ((2 < game.multiplayer) && (1 < net.net_player_count[0])) {
          net_send_message(NM_UNITS,0xff,0,0,0,0,0,0,0,0);
          net_send_message(NM_MULTIOPTIONS_CHANGE,0xff,(short)(*(ushort *)&mp_doubles.production),(short)(*(ushort *)&mp_doubles.moves),
                             mp_options.simultaneous_setting,0,0,0,0,0);
          net_send_text_args(0x45,0xff,0,0,0,0);
          popup_game_with_sprite(str_pmchangeyes,(Sprite *)&popup_info_icon_sprite,0);
        }
      }
    }
  }
  return;
}

/* 004e1452 auto */
void __cdecl menu_message_options(void)

{
  int result;
  
  dlg_checks_clear();
  dlg_check_set(0,~(uint)game.message_options & MSGOFF_GROWTH_HALTED);
  dlg_check_set(1,(uint)((game.message_options & MSGOFF_IMPROVEMENT_BUILT) == 0));
  dlg_check_set(2,(uint)((game.message_options & MSGOFF_UNIT_BUILT) == 0));
  dlg_check_set(3,(uint)((game.message_options & MSGOFF_INVALID_BUILD) == 0));
  dlg_check_set(4,(uint)((game.message_options & MSGOFF_DISORDER) == 0));
  dlg_check_set(5,(uint)((game.message_options & MSGOFF_ORDER_RESTORED) == 0));
  dlg_check_set(6,(uint)((game.message_options & MSGOFF_CELEBRATION) == 0));
  dlg_check_set(7,(uint)((game.message_options & MSGOFF_FOOD_LOW) == 0));
  dlg_check_set(8,(uint)((game.message_options & MSGOFF_POLLUTION) == 0));
  dlg_check_set(9,(uint)((game.message_options & MSGOFF_PRODUCTION_PENALTY) == 0));
  dlg_check_set(10,(uint)((game.message_options & MSGOFF_NO_ZOOM_TO_CITY) == 0));
  result = popup_game_flags((const char *)&str_messageoptions,1);
  if (result == 0) {
    game.message_options = 0;
    result = dlg_check_get(0);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_GROWTH_HALTED;
    }
    result = dlg_check_get(1);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_IMPROVEMENT_BUILT;
    }
    result = dlg_check_get(2);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_UNIT_BUILT;
    }
    result = dlg_check_get(3);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_INVALID_BUILD;
    }
    result = dlg_check_get(4);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_DISORDER;
    }
    result = dlg_check_get(5);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_ORDER_RESTORED;
    }
    result = dlg_check_get(6);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_CELEBRATION;
    }
    result = dlg_check_get(7);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_FOOD_LOW;
    }
    result = dlg_check_get(8);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_POLLUTION;
    }
    result = dlg_check_get(9);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_PRODUCTION_PENALTY;
    }
    result = dlg_check_get(10);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_NO_ZOOM_TO_CITY;
    }
    prefs.message_options = game.message_options;
    prefs_save_civ2dat();
  }
  return;
}

/* 004e2597 auto */
void __cdecl menu_show_hidden_terrain(void)

{
  blink.unit_active = 0;
  map_show_hidden_terrain = 1;
  map_redraw_all_views(human_civ[0],1);
  popup_game((const char *)&str_hiddenterrain);
  map_show_hidden_terrain = 0;
  map_redraw_all_views(human_civ[0],1);
  return;
}

/* 004e25ef auto */
void __cdecl menu_select_music(void)

{
  int choice;
  Dialog dialog2;
  int track_count;
  int tries;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  tries = 0;
  while (tries < 5) {
    tries++;
    track_count = snd_cd_get_track_count();
    if ((((track_count == 0x18) || (track_count == 0x12)) || (track_count == 0xc)) || (track_count == 10))
    break;
    if (track_count == -1) {
      snd_cd_close();
    }
    Sleep(200);
  }
  if (track_count == -1) {
    popup_game((const char *)&str_nopickmusicnew);
  }
  else {
    if (track_count == 0x18) {
      dialog2.load_game_section((const char *)&str_pickmusicfanworlds,1);
    }
    else if (track_count == 0x12) {
      dialog2.load_game_section((const char *)&str_pickmusicscenario,1);
    }
    else if (track_count == 0xc) {
      dialog2.load_game_section((const char *)&str_pickmusicgold,1);
    }
    else {
      if (track_count != 10) {
        popup_game((const char *)&str_nopickmusicnew_2);
        return;
      }
      dialog2.load_game_section((const char *)&str_pickmusic,1);
    }
    if (1 < (*(int *)&cd_music.current_track)) {
      dialog2.set_default_choice((*(int *)&cd_music.current_track) - 2);
    }
    choice = dialog2.show_modal(0);
    if (choice >= 0) {
      music_play_track(choice + 2,1);
    }
  }
  return;
}

/* Part of menu_command (004e2803): the Game menu's options dialogs (0x101-0x106). */
static void menu_cmd_game_options(int cmd)
{
  switch(cmd) {
  case CMD_GAME_OPTIONS:
    menu_game_options();
    break;
  case CMD_GRAPHIC_OPTIONS:
    menu_graphic_options();
    break;
  case CMD_CITY_REPORT_OPTIONS:
    menu_message_options();
    break;
  case CMD_MULTIPLAYER_OPTIONS:
    menu_multiplayer_options();
    break;
  case CMD_GAME_PROFILE:
    game_profile_popup(0);
    break;
  case CMD_PICK_MUSIC:
    menu_select_music();
    break;
  case CMD_OPENCIV2_OPTIONS:
    extras_options_dialog();
  }
}

/* Part of menu_command (004e2803): the View menu's zoom presets, the
   grid-style toggle and the window layout reset (0x321-0x328). */
static void menu_cmd_view_zoom(int cmd)
{
  switch(cmd) {
  case CMD_STANDARD_ZOOM:
    map_views_block.mapview_zoom[0] = 0;
    map_views_block.map_view_center_x[0] = map_cursor.x[0];
    map_views_block.mapview_center_y[0] = map_cursor.y[0];
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
    break;
  case CMD_MEDIUM_ZOOM_OUT:
    map_views_block.mapview_zoom[0] = -3;
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
    break;
  case CMD_MAX_ZOOM_OUT:
    map_views_block.mapview_zoom[0] = -7;
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
    break;
  case CMD_MAP_GRID:
    game.game_options = game.game_options ^ OPT_MAP_GRID;
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_MAP_GRID,(game.game_options & OPT_MAP_GRID) >> 5);
    map_redraw_all_views(human_civ[0],1);
    prefs.game_options = game.game_options;
    prefs_save_civ2dat();
    break;
  case CMD_ARRANGE_WINDOWS:
    extras_set_panel_size(0);
    ui_reset_window_layout();
    break;
  case CMD_ARRANGE_WINDOWS_S:
  case CMD_ARRANGE_WINDOWS_L:
    /* X38: Arrange Windows with a smaller or larger side panel */
    extras_set_panel_size(cmd == CMD_ARRANGE_WINDOWS_S ? -1 : 1);
    ui_reset_window_layout();
  }
}

/* Part of menu_command (004e2803): the Orders menu's terrain orders,
   automate and pillage (0x410-0x421). */
static void menu_cmd_terraform(int cmd)
{
  switch(cmd) {
  case CMD_BUILD_ROAD:
    unit_cmd_terraform(ORDER_BUILD_ROAD);
    break;
  case CMD_BUILD_IRRIGATION:
    unit_cmd_terraform(ORDER_IRRIGATE);
    break;
  case CMD_BUILD_MINES:
    unit_cmd_terraform(ORDER_MINE);
    break;
  case CMD_TRANSFORM:
    unit_cmd_terraform(ORDER_TRANSFORM);
    break;
  case CMD_BUILD_AIRBASE:
    unit_cmd_terraform(ORDER_BUILD_AIRBASE);
    break;
  case CMD_BUILD_FORTRESS:
    unit_cmd_terraform(ORDER_BUILD_FORTRESS);
    break;
  case CMD_AUTOMATE:
    unit_cmd_automate();
    break;
  case CMD_CLEAN_POLLUTION:
    unit_cmd_terraform(ORDER_CLEAN_POLLUTION);
    break;
  case CMD_PILLAGE:
    unit_cmd_pillage();
  }
}

/* Part of menu_command (004e2803): the Orders menu's go to, paradrop,
   airlift and home city (0x440-0x445). */
static void menu_cmd_unit_moves(int cmd)
{
  switch(cmd) {
  case CMD_GOTO:
    unit_cmd_goto_city();
    break;
  case CMD_PARADROP:
    unit_cmd_paradrop();
    break;
  case CMD_AIRLIFT:
    unit_cmd_airlift();
    break;
  case CMD_HOME_CITY:
    unit_cmd_home_city();
  }
}

/* Part of menu_command (004e2803): the Advisors menu (0x500-0x508). */
static void menu_cmd_advisors(int cmd)
{
  SHORT shift_state;

  switch(cmd) {
  case CMD_HIGH_COUNCIL:
    if ((((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) ||
       (shift_state = GetAsyncKeyState(0x10), (char)((ushort)shift_state >> 8) == '\0')) {
      council_show(human_civ[0],0);
    }
    else {
      council_show(human_civ[0],1);
    }
    break;
  case CMD_CITY_STATUS:
    city_status_show(human_civ[0]);
    break;
  case CMD_DEFENSE_MINISTER:
    defense_minister_show(human_civ[0]);
    break;
  case CMD_FOREIGN_MINISTER:
    foreign_minister_show(human_civ[0]);
    break;
  case CMD_ATTITUDE_ADVISOR:
    attitude_advisor_show(human_civ[0]);
    break;
  case CMD_TRADE_ADVISOR:
    trade_advisor_show(human_civ[0]);
    break;
  case CMD_SCIENCE_ADVISOR:
    science_advisor_show(human_civ[0]);
    break;
  case CMD_CHAT_WITH_KINGS:
    chat_wnd_block.open(0,4);
    break;
  case CMD_CASUALTY_TIMELINE:
    casualty_timeline_show(human_civ[0]);
  }
}

/* Part of menu_command (004e2803): the World menu's reports (0x602-0x606;
   with shift in cheat mode: the endgame movies, the destroyed civs). */
static void menu_cmd_world_reports(int cmd)
{
  SHORT shift_state;

  switch(cmd) {
  case CMD_TOP_CITIES:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      top_cities_show(human_civ[0]);
    }
    else {
      endgame_defeat_movie(human_civ[0]);
    }
    break;
  case CMD_SCORE:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      civ_score_window_show(human_civ[0]);
      if (game.reveal_map != '\0') {
        civ_rating_window_show(human_civ[0]);
        powergraph_show();
      }
    }
    else {
      destroyed_civs_show(human_civ[0]);
    }
    break;
  case CMD_DEMOGRAPHICS:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      demographics_show(human_civ[0]);
    }
    else {
      endgame_spaceship_arrival(-(human_civ[0] + 1U & 7));
    }
    break;
  case CMD_SPACESHIPS:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      spaceship_select_view_dialog(human_civ[0]);
    }
    else {
      endgame_spaceship_arrival(human_civ[0]);
    }
  }
}

/* Part of menu_command (004e2803): the Cheat menu's scenario editors
   (0x801-0x809). */
static void menu_cmd_editors(int cmd)
{
  switch(cmd) {
  case CMD_SCENARIO_FLAG:
    editor_toggle_scenario_flag();
    break;
  case CMD_ADVANCES_EDITOR:
    tech_editor_run();
    break;
  case CMD_CITIES_EDITOR:
    city_editor_run();
    break;
  case CMD_EFFECTS_EDITOR:
    cosmic_editor_run();
    break;
  case CMD_IMPROVEMENTS_EDITOR:
    impr_editor_run();
    break;
  case CMD_TERRAIN_EDITOR:
    terrain_editor_open();
    break;
  case CMD_TRIBE_EDITOR:
    tribe_editor_run();
    break;
  case CMD_UNITS_EDITOR:
    unit_editor_run();
    break;
  case CMD_EVENTS_EDITOR:
    events_editor_open();
  }
}

/* Part of menu_command (004e2803): the Civilopedia pages and the credits
   (0x903-0x9f0). */
static void menu_cmd_pedia(int cmd)
{
  SHORT shift_state;
  int result;

  switch(cmd) {
  case CMD_PEDIA_IMPROVEMENTS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(2,1);
    break;
  case CMD_PEDIA_WONDERS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(3,1);
    break;
  case CMD_PEDIA_UNITS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(4,1);
    break;
  case CMD_PEDIA_GOVERNMENTS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(6,1);
    break;
  case CMD_PEDIA_TERRAIN:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(5,1);
    break;
  case CMD_PEDIA_CONCEPTS:
    if ((cmd == CMD_PEDIA_WONDERS) &&
       (shift_state = GetAsyncKeyState(0x10), (char)((ushort)shift_state >> 8) != '\0')) {
      menu_wonder_movies();
    }
    else {
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(7,1);
    }
    break;
  case CMD_ABOUT:
    if (game.multiplayer < 3) {
      result = popup_game_flags((const char *)&str_newcredits_2,1);
      if (-1 < result) {
        credits_show(result);
      }
      text_close_file();
    }
  }
}

/* 004e2803 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl menu_command(i32 menu,int cmd)

{
  SHORT shift_state;
  int result;
  
  result = XD_InFlushSendBuffer();
  if ((((((((result == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
         ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) && (net.lock_map_busy == 0)) &&
       (((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)) &&
        ((net.unit_stack_link_busy == 0 && (((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)))
         ))))) && ((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)))) &&
     ((net.civ_check_destroyed_busy == 0 &&
      (((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)) && ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))))
      ))) {
    if (city_turn.processing == 0) {
      result = ui_paradrop_cursor_active();
      if ((result == 0) || (mapviews_reset_cursor(), cmd != CMD_PARADROP)) {
        if (cmd < 0x111) {
          if (cmd == CMD_SAVE_GAME) {
            game_save_dialog(0);
          }
          else {
            menu_cmd_game_options(cmd);
          }
        }
        else if (cmd < 0x131) {
          if (cmd == CMD_JOIN_GAME) {
            game_join_dialog();
          }
          else if (cmd == CMD_LOAD_GAME) {
            menu_load_game();
          }
        }
        else if (cmd < 0x1f1) {
          if (cmd == CMD_RETIRE) {
            game_quit_or_retire(1);
          }
          else if (cmd == CMD_SET_PASSWORD) {
            civ_change_password_dialog(human_civ[0]);
          }
          else if (cmd == CMD_CHANGE_TIMER) {
            mp_cmd_change_timer();
          }
        }
        else if (cmd < 0x202) {
          if (cmd == CMD_TAX_RATE) {
            taxrate_dialog_show(human_civ[0]);
          }
          else if (cmd == CMD_QUIT) {
            game_quit_or_retire(0);
          }
          else if ((cmd == 0x1f2) && (game.multiplayer == 1)) {
            result = popup_game_with_sprite(str_reallyquit_4,(Sprite *)&popup_info_icon_sprite,0);
            if (result == 0) {
              return;
            }
            snd_cd_stop();
            map_display_disable();
            return;
          }
        }
        else if (cmd < 0x211) {
          if (cmd == CMD_FIND_CITY) {
            menu_find_city();
          }
          else if (cmd == CMD_THRONE_ROOM) {
            throneroom_show(human_civ[0]);
          }
        }
        else if (cmd < 0x302) {
          if (cmd == CMD_MOVE_PIECES) {
            ui_focus_active_unit(1);
          }
          else if (cmd == CMD_REVOLUTION) {
            menu_revolution();
          }
        }
        else if (cmd < 0x311) {
          if (cmd == CMD_ZOOM_IN) {
            if (map_views_block.mapview_zoom[0] < 8) {
              map_views_block.mapview_zoom[0]++;
              ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
            }
          }
          else if (cmd == CMD_VIEW_PIECES) {
            ui_redraw_all_maps(0);
          }
        }
        else if (cmd < 0x321) {
          if (cmd == CMD_MAX_ZOOM_IN) {
            map_views_block.mapview_zoom[0] = 8;
            ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
          }
          else if ((cmd == CMD_ZOOM_OUT) && (-7 < map_views_block.mapview_zoom[0])) {
            map_views_block.mapview_zoom[0]--;
            ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
          }
        }
        else if (cmd < 0x331) {
          if (cmd == CMD_SHOW_HIDDEN_TERRAIN) {
            menu_show_hidden_terrain();
          }
          else {
            menu_cmd_view_zoom(cmd);
          }
        }
        else if (cmd < 0x402) {
          if (cmd == CMD_BUILD_CITY) {
            unit_cmd_build_city();
          }
          else if (cmd == CMD_CENTER_VIEW) {
            ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)map_cursor.x[0],(int)map_cursor.y[0]);
          }
        }
        else if (cmd < 0x431) {
          if (cmd == CMD_UNLOAD) {
            unit_cmd_unload();
          }
          else {
            menu_cmd_terraform(cmd);
          }
        }
        else if (cmd < 0x451) {
          if (cmd == CMD_FORTIFY) {
            unit_cmd_fortify();
          }
          else {
            menu_cmd_unit_moves(cmd);
          }
        }
        else if (cmd < 0x461) {
          if (cmd == CMD_DISBAND) {
            unit_cmd_disband();
          }
          else if (cmd == CMD_SLEEP) {
            unit_cmd_sleep();
          }
        }
        else if (cmd < 0x471) {
          if (cmd == CMD_WAIT) {
            unit_cmd_wait();
          }
          else if (cmd == CMD_ACTIVATE_UNIT) {
            ui_pick_unit_from_stack();
          }
        }
        else if (cmd < 0x491) {
          if (cmd == CMD_END_TURN) {
            cmd_end_player_turn();
          }
          else if (cmd == CMD_SKIP_TURN) {
            cmd_unit_skip_turn();
          }
        }
        else if (cmd < 0x602) {
          if (cmd == CMD_WONDERS) {
            shift_state = GetAsyncKeyState(0x10);
            if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
               (game.multiplayer != '\0')) {
              wonders_report_show(human_civ[0]);
            }
            else {
              menu_wonder_movies();
            }
          }
          else {
            menu_cmd_advisors(cmd);
          }
        }
        else if (cmd < 0x702) {
          if (cmd == CMD_CHEAT_MODE) {
            if ((game.multiplayer == '\0') || (mp_cheat_menu_allowed[0] != 0)) {
              cheat_toggle_mode();
            }
          }
          else {
            menu_cmd_world_reports(cmd);
          }
        }
        else if (cmd < 0x722) {
          if (cmd == CMD_CHEAT_SET_YEAR) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_set_game_year();
            }
          }
          else if (cmd == CMD_CHEAT_CREATE_UNIT) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_create_unit();
            }
          }
          else if (cmd == CMD_CHEAT_REVEAL_MAP) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_reveal_map();
            }
          }
          else if (((cmd == CMD_CHEAT_SET_HUMAN) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_set_human_player();
          }
        }
        else if (cmd < 0x732) {
          if (cmd == CMD_CHEAT_TECH_ADVANCE) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_tech_advance();
            }
          }
          else if (((cmd == CMD_CHEAT_KILL_CIV) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_kill_civ();
          }
        }
        else if (cmd < 0x741) {
          if (cmd == CMD_CHEAT_FORCE_GOVERNMENT) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_force_government();
            }
          }
          else if (((cmd == CMD_CHEAT_EDIT_TECHS) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_edit_technologies();
          }
        }
        else if (cmd < 0x751) {
          if (cmd == CMD_CHEAT_DESTROY_UNITS) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_destroy_units_at_cursor();
              ui_focus_active_unit(0);
            }
          }
          else if (((cmd == CMD_CHEAT_CHANGE_TERRAIN) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_terrain_saved_bits = 0xffffffff;
            cheat_change_terrain();
          }
        }
        else if (cmd < 0x756) {
          if (cmd == CMD_CHEAT_EDIT_UNIT) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_edit_unit();
            }
          }
          else if (((cmd == CMD_CHEAT_CHANGE_MONEY) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_change_money();
          }
        }
        else if (cmd < 0x766) {
          if (cmd == CMD_CHEAT_EDIT_KING) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_edit_king();
            }
          }
          else if (((cmd == CMD_CHEAT_EDIT_CITY) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_edit_city();
          }
        }
        else if (cmd < 0x771) {
          if (cmd == CMD_CHEAT_SAVE_SCENARIO) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_save_as_scenario();
            }
          }
          else if (((cmd == CMD_CHEAT_SCENARIO_PARAMS) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_scenario_params();
          }
        }
        else if (cmd < 0x902) {
          if (cmd == CMD_PEDIA_ADVANCES) {
            ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(1,1);
          }
          else {
            menu_cmd_editors(cmd);
          }
        }
        else {
          menu_cmd_pedia(cmd);
        }
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  else {
    dbg_log((const char *)&str_mainmenu_menu_exec_blocked_by_xd_flushse);
    net.blocked_input_kind = 5;
    net.blocked_input_arg1 = menu;
    net.blocked_input_arg2 = cmd;
  }
  return;
}

/* 004e3a86 auto */
void __cdecl menu_build_main(void)

{
  int status;
  i32 editor_disabled;
  i32 editor_checked;
  i32 cheat_checked;
  i32 cheat_disabled;
  int row;
  int options_added = 0;
  
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->reset(0x1bbc);
  /* (port) The menus' structure is menu_rows; their texts are looked up by
     key (lang.h), not read from MENU.TXT line by line.  An item without a
     text is left out (the CD's MENU.TXT has no Game Profile). */
  lang_load_menus();
  for (row = 0; row < menu_row_count; row++) {
    if (menu_rows[row].cmd == MENU_TITLE) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_menu(menu_rows[row].menu,
                    (char *)lang_text(menu_rows[row].key));
    }
    else if (menu_rows[row].cmd == 0) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,0,0,0);
    }
    else if (lang_has(menu_rows[row].key) != 0) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,menu_rows[row].cmd,
                    lang_text(menu_rows[row].key),menu_rows[row].room);
      if (menu_rows[row].cmd == CMD_ARRANGE_WINDOWS && plat_extras) {
        /* X38: Arrange Windows S and L (hidden while X38 is off: the options
           dialog can switch it) */
        static char label_s[64], label_l[64];
        const char *label = lang_text(menu_rows[row].key);
        int n = (int)strcspn(label, "|");
        snprintf(label_s, sizeof label_s, "%.*s S", n, label);
        snprintf(label_l, sizeof label_l, "%.*s L", n, label);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,CMD_ARRANGE_WINDOWS_S,
                      extras_text_translated("menu.arrange_s") ? extras_text("menu.arrange_s") : label_s,0);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,CMD_ARRANGE_WINDOWS_L,
                      extras_text_translated("menu.arrange_l") ? extras_text("menu.arrange_l") : label_l,0);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(CMD_ARRANGE_WINDOWS_S,!extra_ui(X_ARRANGE));
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(CMD_ARRANGE_WINDOWS_L,!extra_ui(X_ARRANGE));
      }
    }
    if (plat_extras && menu_rows[row].menu == 1 && menu_rows[row + 1].menu == 1 &&
        menu_rows[row + 1].cmd == 0 && !options_added) {
      /* OpenCiv2's options, after the Game menu's options dialogs */
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(1,CMD_OPENCIV2_OPTIONS,
                    extras_text("menu.options"),0);
      options_added = 1;
    }
  }
  if (((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) {
    cheat_disabled = 1;
  }
  else {
    cheat_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(7,cheat_disabled);
  if (((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) {
    cheat_checked = 0;
  }
  else {
    cheat_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_CHEAT_MODE,cheat_checked);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHEAT_MODE,
             (game.multiplayer != '\0' &((int *)&mp_cheat_menu_allowed) == 0));
  if (((((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) ||
      ((game.game_flags & GAME_SCENARIO) == 0)) ||
     (status = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), status == 0)) {
    editor_checked = 0;
  }
  else {
    editor_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_SCENARIO_FLAG,editor_checked);
  if ((((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) ||
     (((game.game_flags & GAME_SCENARIO) == 0 ||
      (status = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), status == 0)))) {
    editor_disabled = 1;
  }
  else {
    editor_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(8,editor_disabled);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SCENARIO_FLAG,(game.multiplayer != '\0'));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x441,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x413,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x442,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x417,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x418,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x41b,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->rebuild(&main_frame_wnd_block.main_frame_wnd);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_command_handler(menu_command);
  return;
}

/* 004e4c92 auto */
void __cdecl menu_set_item_label(i32 item_id,i32 label_id,i32 enabled)

{
  i8 label_text[80];
  
  text_clear();
  text_append_label(label_id);
  /* BF-200 (Peter Johnson's ledger): a label is cut at 60 characters, the
     room its menu item was made with (lang.cpp menu_rows: the original
     expanded it whole into these 80 bytes and copied it into the item's) */
  text_expand_vars_max(textbuf.buf,label_text,0x3d);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_text(item_id,(const char *)label_text);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(item_id,enabled);
  return;
}

/* Part of menu_update_states (004e4ceb): the settlers' orders (build city / join city, road
   and railroad, irrigation and farmland, mine, transform, fortress, airbase,
   pillage) for the unit's tile. */
static void menu_update_settler_orders(int unit,int x,int y,int civ,uptr terrain,bool is_ocean)
{
  char dy;
  bool no_adjacent_city;
  int city;
  iptr x2;
  uint improvements2;
  i32 improvements22;
  iptr airbase_owner;
  iptr airbase_owner2;
  int terrain2;
  byte *tile;
  i32 mine_disabled;
  i32 irrigate_disabled;
  i32 farmland_disabled;
  i32 road_disabled;
  i32 railroad_disabled;
  i32 build_city_disabled;
  iptr dir;

  no_adjacent_city = true;
  for (dir = 0; dir < 8; dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    city = map_in_bounds(x2,dy + y);
    if ((city != 0) && (city = tile_city_owner(x2,dy + y), -1 < city)) {
      no_adjacent_city = false;
    }
  }
  city = city_at(x,y);
  if (city < 0) {
    terrain2 = tile_is_ocean(x,y);
    if ((terrain2 == TERRAIN_DESERT) && (no_adjacent_city)) {
      build_city_disabled = 0;
    }
    else {
      build_city_disabled = 1;
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_CITY,build_city_disabled);
  }
  else {
    menu_set_item_label(CMD_BUILD_CITY,0x33,(uint)('\t' < cities[city].size));
  }
  improvements2 = tile_improvements(x,y);
  if ((improvements2 & TILE_POLLUTION) == 0) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CLEAN_POLLUTION,1);
  }
  improvements22 = tile_improvements(x,y);
  if ((((((byte)improvements22 & TILE_AIRBASE) == 0x40) ||
       (airbase_owner = tile_airbase_owner(x,y), -1 < airbase_owner)) || (is_ocean)) || (-1 < city)) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_FORTRESS,1);
  }
  improvements22 = tile_improvements(x,y);
  if (((((byte)improvements22 & TILE_AIRBASE) == 0x40) ||
      (airbase_owner2 = tile_airbase_owner(x,y), -1 < airbase_owner2)) || ((is_ocean || (-1 < city)))) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_AIRBASE,1);
  }
  improvements2 = tile_improvements(x,y);
  if (((improvements2 & TILE_ROAD) == 0) || (terrain2 = civ_has_advance(civ,ADV_RAILROAD), terrain2 == TERRAIN_DESERT)) {
    if (((is_ocean) ||
        ((-1 < city || (improvements2 = tile_improvements(x,y), (improvements2 & TILE_ROAD) != 0)))) ||
       ((tile = (byte *)tile_at(x,y), (*tile & 0x80) != 0 &&
        (terrain2 = civ_has_advance(civ,ADV_BRIDGE_BUILDING), terrain2 == TERRAIN_DESERT)))) {
      road_disabled = 1;
    }
    else {
      road_disabled = 0;
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_ROAD,road_disabled);
  }
  else {
    improvements2 = tile_improvements(x,y);
    if ((((improvements2 & TILE_RAILROAD) != 0) || (-1 < city)) || (is_ocean)) {
      railroad_disabled = 1;
    }
    else {
      railroad_disabled = 0;
    }
    menu_set_item_label(CMD_BUILD_ROAD,0x35,railroad_disabled);
  }
  terrain2 = (int)terrain_types[terrain].irrigate_to;
  if (terrain2 < 0) {
    improvements2 = tile_improvements(x,y);
    if ((improvements2 & TILE_IRRIGATION) == 0) {
      if (((terrain2 == -1) || (is_ocean)) || (-1 < city)) {
        irrigate_disabled = 1;
      }
      else {
        irrigate_disabled = 0;
      }
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_IRRIGATION,irrigate_disabled);
    }
    else {
      terrain2 = civ_has_advance(civ,ADV_REFRIGERATION);
      if (((terrain2 == TERRAIN_DESERT) || (is_ocean)) || (-1 < city)) {
        farmland_disabled = 1;
      }
      else {
        farmland_disabled = 0;
      }
      menu_set_item_label(CMD_BUILD_IRRIGATION,0x3d,farmland_disabled);
    }
  }
  else {
    text_set_label_arg(0,terrain_types[terrain2].name);
    menu_set_item_label(CMD_BUILD_IRRIGATION,0x37,0);
  }
  terrain2 = (int)terrain_types[terrain].mine_to;
  if (terrain2 < 0) {
    if (((terrain2 == -1) || (is_ocean)) || (-1 < city)) {
      mine_disabled = 1;
    }
    else {
      mine_disabled = 0;
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_MINES,mine_disabled);
  }
  else {
    text_set_label_arg(0,terrain_types[terrain2].name);
    menu_set_item_label(CMD_BUILD_MINES,0x39,0);
  }
  civ = civ_can_build_unit(civ,-1,UNIT_ENGINEERS);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x413,(uint)(civ == 0));
  if (terrain_types[terrain].transform_to < 0) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_TRANSFORM,1);
  }
  else {
    text_set_label_arg(0,terrain_types[terrain_types[terrain].transform_to].name);
    menu_set_item_label(CMD_TRANSFORM,0x3c,(uint)(units[unit].type != UNIT_ENGINEERS));
  }
}

/* Part of menu_update_states (004e4ceb): the Game and Kingdom items that depend on single
   or multiplayer (join, load, chat, timer, save, quit, retire). */
static void menu_update_multiplayer_items(void)
{
  if (game.multiplayer == '\0') {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_JOIN_GAME,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHAT_WITH_KINGS,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_MULTIPLAYER_OPTIONS,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_GAME_PROFILE,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_LOAD_GAME,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_ABOUT,0);
  }
  else {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_ABOUT,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_MULTIPLAYER_OPTIONS,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_GAME_PROFILE,0);
    if (mp_cheat_menu_allowed[0] == 0) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHEAT_MODE,1);
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_LOAD_GAME,1);
    if ((mp_options.open_game == 0) || (game.multiplayer != 1)) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_JOIN_GAME,1);
    }
    else {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_JOIN_GAME,0);
    }
    if (game.multiplayer == 1) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_THRONE_ROOM,1);
    }
    else {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_THRONE_ROOM,0);
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHAT_WITH_KINGS,(game.multiplayer < 3));
    if (game.multiplayer == 2) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,1);
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SAVE_GAME,1);
    }
    else {
      if (net.host_flag == '\0') {
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,1);
      }
      else {
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,0);
      }
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SAVE_GAME,0);
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_QUIT,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_RETIRE,0);
  }
}

/* Part of menu_update_states (004e4ceb): the Orders menu for the active unit. */
static void menu_update_unit_orders(void)
{
  u8 unit_role;
  byte terrain_byte;
  int unit;
  int x;
  int y;
  int civ;
  iptr has_airbase_tech;
  iptr has_fortress_tech;
  int city;
  uptr terrain;
  bool is_ocean;
  i32 fortress_hidden;
  i32 airbase_hidden;
  i32 pillage_hidden;

  unit = (int)game.active_unit;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  civ = (int)units[unit].owner;
  terrain_byte = tile_terrain(x,y);
  terrain = (uptr)terrain_byte;
  is_ocean = terrain == TERRAIN_OCEAN;
  unit_role = unit_types[units[unit].type].role;
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(4,0);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x401,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_CITY,0x32,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_ROAD,0x34,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_IRRIGATION,0x36,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_MINES,0x38,(uint)(unit_role != ROLE_SETTLE));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x450,(uint)(unit_role == ROLE_SETTLE));
  if ((unit_role == ROLE_SETTLE) && (has_airbase_tech = civ_has_advance(civ,ADV_CONSTRUCTION), has_airbase_tech != 0)) {
    pillage_hidden = 0;
  }
  else {
    pillage_hidden = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x418,pillage_hidden);
  if ((unit_role == ROLE_SETTLE) && (has_fortress_tech = civ_has_advance(civ,ADV_RADIO), has_fortress_tech != 0)) {
    airbase_hidden = 0;
  }
  else {
    airbase_hidden = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x417,airbase_hidden);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x430,(uint)(unit_role == ROLE_SETTLE));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x41b,(uint)(unit_role != ROLE_SETTLE));
  city = civ_has_advance(civ,ADV_COMBINED_ARMS);
  if ((city == 0) && ((unit_types[units[unit].type].flags & UTYPE_PARADROP) == 0)) {
    fortress_hidden = 1;
  }
  else {
    fortress_hidden = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x441,fortress_hidden);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_PARADROP,
             ((unit_types[units[unit].type].flags & UTYPE_PARADROP) == 0));
  city = civ_has_advance(civ,ADV_RADIO);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x442,(uint)(city == 0));
  city = tile_city_owner(x,y);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_AIRLIFT,(city < 0));
  if (unit_role == ROLE_SETTLE) {
    menu_update_settler_orders(unit,x,y,civ,terrain,is_ocean);
  }
  else {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_TRANSFORM,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_FORTRESS,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CLEAN_POLLUTION,1);
  }
  civ = tile_city_owner(x,y);
  if (civ < 0) {
    menu_set_item_label(CMD_HOME_CITY,0x3b,0);
  }
  else {
    x = tile_city_owner(x,y);
    menu_set_item_label(CMD_HOME_CITY,0x3a,(uint)(x < 0));
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_UNLOAD,(unit_types[units[unit].type].hold == '\0'))
  ;
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x421,(uint)(unit_role == ROLE_SETTLE));
  if ((unit_types[units[unit].type].attack == '\0') ||
     (unit_types[units[unit].type].domain != DOMAIN_LAND)) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_PILLAGE,1);
  }
}

/* Part of menu_update_states (004e4ceb): the Cheat and Scenario menus. */
static void menu_update_cheat_items(void)
{
  iptr path_cmp;
  iptr path_cmp2;
  i32 scenario_menu_disabled;
  i32 scenario_checked;
  i32 cheat_checked;
  i32 cheat_menu_disabled;

  if ((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) {
    cheat_menu_disabled = 1;
  }
  else {
    cheat_menu_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(7,cheat_menu_disabled);
  if ((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) {
    cheat_checked = 0;
  }
  else {
    cheat_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_CHEAT_MODE,cheat_checked);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHEAT_MODE,(uint)(game.multiplayer != '\0' &((int *)&mp_cheat_menu_allowed) == 0)
            );
  if ((2 < game.multiplayer) && (net.mp_turn_civ != human_civ[0])) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(4,1);
  }
  if ((((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) ||
      ((game.game_flags & GAME_SCENARIO) == 0)) ||
     (path_cmp = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), path_cmp == 0)) {
    scenario_checked = 0;
  }
  else {
    scenario_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_SCENARIO_FLAG,scenario_checked);
  if (((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) ||
     (((game.game_flags & GAME_SCENARIO) == 0 || (path_cmp2 = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), path_cmp2 == 0)
      ))) {
    scenario_menu_disabled = 1;
  }
  else {
    scenario_menu_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(8,scenario_menu_disabled);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SCENARIO_FLAG,(game.multiplayer != '\0'))
  ;
}

/* 004e4ceb auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl menu_update_states(void)

{
  i32 revolution_disabled;
  
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SPACESHIPS,game.rules & RULE_BLOODLUST);
  menu_update_multiplayer_items();
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_VIEW_PIECES,(moving_units_mode == 0));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_MOVE_PIECES,(moving_units_mode == 1));
  if ((((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 0x10) == 0)) &&
     (civs[human_civ[0]].government != GOV_ANARCHY)) {
    revolution_disabled = 0;
  }
  else {
    revolution_disabled = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_REVOLUTION,revolution_disabled);
  if (((moving_units_mode == 0) || (game.num_units == 0)) ||
     ((game.active_unit < 0 ||
      ((game.num_units < game.active_unit || (units[game.active_unit].id == 0)))))) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(4,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_DISBAND,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_ACTIVATE_UNIT,0);
  }
  else {
    menu_update_unit_orders();
  }
  menu_update_cheat_items();
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->rebuild(0);
  return;
}
