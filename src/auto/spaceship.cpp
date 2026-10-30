/* The game, the space race: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004d0160 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl spaceship_wnd_net_idle(void)

{
  int now;
  
  net.process_messages(1,0);
  if ((net.host_handoff_stage == '\0') && (now = timer_ticks(), now - editor_text_edit_block.spaceship_wnd_open_ticks_i32 < 0x4b1)) {
    return;
  }
  ((ModalMenuWindow *)spaceship_launch_sprite_xs_block.spaceship_wnd)->end_modal_loop();
  return;
}

/* 004d01ae auto */
void __cdecl spaceship_wnd_load_parts(iptr civ)

{
  int i;
  
  for (i = 0; i < 6; i++) {
    editor_text_edit_block.spaceship_wnd_part_count_list[i] =
         (int)civs[civ].spaceship_parts[i];
  }
  return;
}

/* 004d0208 auto */
i32 __cdecl spaceship_wnd_run(iptr civ_arg)

{
  i32 result;
  int init_ok;
  i32 ret_addr;
  int civ;
  SpaceshipWindow window[1];   /* 0x2180 bytes */
  i32 eh_next;
  
  if (spaceship_launch_sprite_xs_block.spaceship_wnd == 0) {
    if (council_state.active == '\0') {
      if (civ_arg < 1) {
        civ = ~civ_arg + 1;
      }
      else {
        civ = civ_arg;
      }
      ret_addr = 0x4d0293;
      window->construct(civ);
      ON_SCOPE_EXIT(((SpaceshipWindow *)&window)->destruct());
      init_ok = window->init();
      if (init_ok == 0) {
        result = 0;
      }
      else {
        if (civ_arg < 0) {
          window->launch_sequence();
        }
        else {
          window->run_modal();
        }
        result = 1;
      }
    }
    else {
      result = 1;
    }
  }
  else {
    result = 1;
  }
  return result;
}

/* 004d0339 auto */
i32 __cdecl spaceship_wnd_show_for_civ(iptr civ)

{
  i32 result;
  int init_ok;
  i32 ret_addr;
  SpaceshipWindow window[1];   /* 0x2180 bytes */
  i32 eh_next;
  
  if (human_civ[0] == civ) {
    if (spaceship_launch_sprite_xs_block.spaceship_wnd == 0) {
      if (council_state.active == '\0') {
        ret_addr = 0x4d03b3;
        window->construct(civ);
        ON_SCOPE_EXIT(((SpaceshipWindow *)&window)->destruct());
        init_ok = window->init();
        if (init_ok == 0) {
          result = 0;
        }
        else {
          window->present();
          result = 1;
        }
      }
      else {
        result = 1;
      }
    }
    else {
      result = 1;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004d043f auto */
void __cdecl spaceship_wnd_draw_title(iptr civ)

{
  iptr text;
  tagRECT rect;
  
  text_clear();
  text = (iptr)(civ_name_plural(civ));
  text_append((const char *)text);
  textbuf.char_1 = 0;
  text_append((const char *)&str_s_s);
  text = (iptr)(civ_leader_name(civ));
  text_append((const char *)text);
  text_append_space();
  text_append((const char *)&str_empty_123);
  text = civ_get_adjective(civ);
  text_append((const char *)text);
  text_append((const char *)&str_empty_124);
  gfx_set_text_color_index(0xfa);
  SetRect(&rect,0,0,0x27f,0x28);
  ((DibSurface *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xb8))->draw_text_in_rect((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0x8c4,(const char *)&textbuf.buf,&rect,
             0);
  win_flush_pending_msgs();
  return;
}

/* 004d0517 auto */
SpaceshipWindow *SpaceshipWindow::construct(u32 civ)

{
  
  this->ModalMenuWindow::construct();
  ((SurfaceWindow *)&control_list)->SurfaceWindow::construct();
  ((DllHandle *)ship_dll)->init();
  ((GamePalette *)ship_palette)->init();
  ((GameFont *)large_font)->init_empty();
  ((GameFont *)medium_font)->init_empty();
  ((GameFont *)typing_font)->init_empty();
  surface_ctor((DibSurface *)ship_view_surface);
  sprite_init((Sprite *)ship_image_sprite);
  sprite_init((Sprite *)close_btn_disabled_sprite);
  sprite_init((Sprite *)close_btn_normal_sprite);
  sprite_init((Sprite *)close_btn_pressed_sprite);
  sprite_init((Sprite *)launch_btn_disabled_sprite);
  sprite_init((Sprite *)launch_btn_normal_sprite);
  sprite_init((Sprite *)launch_btn_pressed_sprite);
  ((ClickMap *)hotspot_map)->construct();
  ship_civ = civ;
  anim_timer = 0;
  gfx_get_screen_rect((iptr)&frame_left);
  type_index = 0;
  spaceship_launch_sprite_xs_block.spaceship_wnd = this;
  ((GameFont *)large_font)->set_styled(0,0x18,1);
  ((GameFont *)medium_font)->set_styled(0,0x14,1);
  ((GameFont *)typing_font)->set_styled(0,0x10,1);
  spaceship_state = 0;
  launched = 0;
  ((DibSurface *)ship_view_surface)->DibSurface::create(0x1c9,0x130);
  ((DibSurface *)ship_view_surface)->fill(9);
  flight_timer = 0;
  anim_timer = 0;
  blink_timer = 0;
  SetRect((LPRECT)ship_panel_rect,10,0x14,300,0x1df);
  launch_video_player = 0;
  launch_state = 0;
  close_btn_pressed = 0;
  launch_btn_pressed = 0;
  typing_label_part = 0;
  return this;
}

/* 004d08b0 auto */
i32 SpaceshipWindow::destruct()

{
  i32 ret_value;
  int eh_state;
  
  eh_state = 0xf;
  ((DllHandle *)ship_dll)->dll_handle_free();
  spaceship_launch_sprite_xs_block.spaceship_wnd = 0;
  if (flight_timer != 0) {
    game_timer_destroy(flight_timer);
    flight_timer = 0;
  }
  if (anim_timer != 0) {
    game_timer_destroy(anim_timer);
    anim_timer = 0;
  }
  if (blink_timer != 0) {
    game_timer_destroy(blink_timer);
    blink_timer = 0;
  }
  (*((char *)&eh_state + 0)) = 0xe;
  /* inlined unwind funclet 004d0a56 */ { ((ClickMap *)((u8 *)this + 0xae4))->destruct(); }
  (*((char *)&eh_state + 0)) = 0xd;
  /* inlined unwind funclet 004d0a65 */ { sprite_destroy((Sprite *)(((int)this) + 0xaa4)); }
  (*((char *)&eh_state + 0)) = 0xc;
  /* inlined unwind funclet 004d0a74 */ { sprite_destroy((Sprite *)(((int)this) + 0xa68)); }
  (*((char *)&eh_state + 0)) = 0xb;
  /* inlined unwind funclet 004d0a83 */ { sprite_destroy((Sprite *)(((int)this) + 0xa2c)); }
  (*((char *)&eh_state + 0)) = 10;
  /* inlined unwind funclet 004d0a92 */ { sprite_destroy((Sprite *)(((int)this) + 0x9f0)); }
  (*((char *)&eh_state + 0)) = 9;
  /* inlined unwind funclet 004d0aa1 */ { sprite_destroy((Sprite *)(((int)this) + 0x9b4)); }
  (*((char *)&eh_state + 0)) = 8;
  /* inlined unwind funclet 004d0ab0 */ { sprite_destroy((Sprite *)(((int)this) + 0x978)); }
  (*((char *)&eh_state + 0)) = 7;
  /* inlined unwind funclet 004d0abf */ { sprite_destroy((Sprite *)(((int)this) + 0x93c)); }
  (*((char *)&eh_state + 0)) = 6;
  /* inlined unwind funclet 004d0ace */ { surface_dtor((DibSurface *)(((int)this) + 0x8f4)); }
  (*((char *)&eh_state + 0)) = 5;
  /* inlined unwind funclet 004d0add */ { ((GameFont *)(((int)this) + 0x8d4))->destroy(); }
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 004d0aec */ { ((GameFont *)(((int)this) + 0x8cc))->destroy(); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 004d0afb */ { ((GameFont *)(((int)this) + 0x8c4))->destroy(); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 004d0b0a */ { ((GamePalette *)(((int)this) + 0x1f8))->destroy(); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 004d0b19 */ { ((DllHandle *)(((int)this) + 500))->free(); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 004d0b28 */ { ((SurfaceWindow *)(((int)this) + 0xb8))->SurfaceWindow::destruct(); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004d0b37 */ { ModalMenuWindow::destruct(); }
  return ret_value;
}

/* 004d0b58 auto */
i32 SpaceshipWindow::init()

{
  int status;
  i32 result;
  i8 dll_path[128];
  
  str_clear((char *)dll_path);
  str_append((char *)dll_path,(const char *)&str_ss_dll);
  status = file_resolve_path((const char *)dll_path);
  if (status == 0) {
    result = 0;
  }
  else {
    ((DllHandle *)ship_dll)->load((const char *)dll_path);
    set_cursor(0x1fc);
    spaceship_calc_stats(ship_civ,1);
    build_stats_text();
    if ((civs[ship_civ].spaceship_parts[3] != 0) && (spaceship_stats.success_pct != 0)) {
      launch_state = 1;
    }
    status = civ_spaceship_launched(ship_civ);
    if (status != 0) {
      launch_state = 3;
    }
    status = create();
    if (status == 0) {
      result = 0;
    }
    else {
      make_button_sprites();
      if (flight_timer != 0) {
        game_timer_destroy(flight_timer);
        flight_timer = 0;
      }
      if (anim_timer != 0) {
        game_timer_destroy(anim_timer);
        anim_timer = 0;
      }
      if (blink_timer != 0) {
        game_timer_destroy(blink_timer);
        blink_timer = 0;
      }
      type_x = 10;
      type_y = 0x28;
      setup_hotspots();
      result = 1;
    }
  }
  return result;
}

/* 004d0d64 auto */
uint SpaceshipWindow::create()

{
  int loaded;
  
  win_flush_pending_msgs();
  create_with_handlers((const char *)&str_empty_125,0x800,frame_left,frame_top,
             frame_right - frame_left,(frame_bottom - frame_top) + 5,(iptr)&game_palette_block.game_palette,
             (iptr)&main_frame_wnd_block.main_frame_wnd);
  set_bg_color(0);
  ((DibSurface *)&control_list)->wnd_create_with_surface((const char *)&str_empty_126,0x800,0,0,0x280,0x1e0,
             (iptr)(ship_palette),(iptr)this);
  loaded = ((DibSurface *)&control_list)->load_gif_resource(499,10,0xec,(iptr)(ship_palette));
  if (loaded != 0) {
    ((WndBase *)frame_wnd_base)->center();
    ((WndHandlers *)frame_handlers)->set_keydown_handler(spaceship_wnd_on_key);
    ((WndHandlers *)frame_handlers)->set_mousemove_handler(spaceship_wnd_on_mousemove);
    ((WndHandlers *)frame_handlers)->set_lbuttondown_handler(spaceship_wnd_on_lbuttondown);
    ((WndHandlers *)frame_handlers)->set_lbuttonup_any_handler(spaceship_wnd_on_lbuttonup);
  }
  return (uint)(loaded != 0);
}

/* 004d0ea6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void SpaceshipWindow::run_modal()

{
  int rendered;
  u32 timer;
  i8 clip_rect[16];
  Sprite launch_sprite;
  DibSurface launch_surface;
  
  surface_ctor(&launch_surface);
  ON_SCOPE_EXIT(surface_dtor(&launch_surface));
  sprite_init(&launch_sprite);
  ON_SCOPE_EXIT(sprite_destroy(&launch_sprite));
  spaceship_wnd_load_parts(ship_civ);
  rendered = render_ship();
  if (rendered != 0) {
    if (launch_state == 3) {
      launch_surface.load_gif_resource(editor_text_edit_block.spaceship_wnd_component_count + 19999,10,0xec,(iptr)(ship_palette));
      launch_sprite.grab_from_surface((iptr)&launch_surface,0xff,
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_xs) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_ys) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_widths) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_part_sprite_heights) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10));
      launch_sprite.draw((iptr)clip_rect,(iptr)&control_list,
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_xs) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_ys) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10));
    }
    switch(launch_state) {
    case 0:
    case 1:
      draw_launch_btn(0,1);
      break;
    case 2:
      break;
    case 3:
      init_starfield();
      spaceship_wnd_draw_title(ship_civ);
    }
    draw_close_btn(1,1);
    if (anim_timer != 0) {
      game_timer_destroy(anim_timer);
    }
    if (launch_state == 3) {
      timer = game_timer_create((TimerFn)spaceship_wnd_starfield_tick,5,0xffffffff);
      flight_timer = timer;
    }
    else {
      timer = game_timer_create((TimerFn)spaceship_wnd_type_tick,0x3c,0xffffffff);
      anim_timer = timer;
    }
    handle_show_raise();
    wnd_flush_paint();
    set_palette((iptr)(ship_palette));
    ((SurfaceWindow *)&control_list)->SurfaceWindow::show();
    wnd_flush_paint();
    main_frame_wnd_block.main_frame_wnd.set_palette((iptr)(ship_palette));
    setup_hotspots();
    draw_buttons();
    if (launch_state == 3) {
      launched = 1;
    }
    else {
      launched = 0;
    }
    typing_label_part = 1;
    ((WndBase *)frame_wnd_base)->set_capture();
    input_flush_events();
    if (2 < game.multiplayer) {
      ((ModalMenuWindow *)frame_wnd_base)->set_modal_idle_callback(spaceship_wnd_net_idle);
    }
    editor_text_edit_block.spaceship_wnd_open_ticks_i32 = timer_ticks();
    ((ModalMenuWindow *)frame_wnd_base)->run_modal_loop();
    ((ModalMenuWindow *)frame_wnd_base)->set_modal_idle_callback(0);
    ((WndBase *)frame_wnd_base)->release_capture();
    if (flight_timer != 0) {
      game_timer_destroy(flight_timer);
      flight_timer = 0;
    }
    if (anim_timer != 0) {
      game_timer_destroy(anim_timer);
      anim_timer = 0;
    }
    if (blink_timer != 0) {
      game_timer_destroy(blink_timer);
      blink_timer = 0;
    }
    if (launch_state == 2) {
      play_launch_video();
      launch_state = 3;
      spaceship_launch(ship_civ);
      run_modal();
    }
    else {
      ((WndBase *)frame_wnd_base)->hide();
      wnd_flush_paint();
    }
    main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
    set_palette((iptr)&game_palette_block.game_palette);
    hide();
    wnd_flush_paint();
  }
  return;
}

/* 004d13b8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void SpaceshipWindow::present()

{
  int rendered;
  u32 value;
  tagRECT screen_rect2;
  
  if (ship_civ == human_civ[0]) {
    if (anim_timer != 0) {
      game_timer_destroy(anim_timer);
    }
    if ((launch_state != 3) && (rendered = render_ship(), rendered != 0)) {
      draw_close_btn(1,1);
      draw_launch_btn(0,1);
      typing_label_part = 1;
      handle_show_raise();
      wnd_flush_paint();
      set_palette((iptr)(ship_palette));
      ((SurfaceWindow *)&control_list)->SurfaceWindow::show();
      wnd_flush_paint();
      main_frame_wnd_block.main_frame_wnd.set_palette((iptr)(ship_palette))
      ;
      setup_hotspots();
      draw_buttons();
      launched = 1;
      typing_label_part = 1;
      value = launch_state;
      launch_state = 0;
      launch_state = value;
      spaceship_wnd_load_parts(ship_civ);
      rendered = render_ship();
      if (rendered != 0) {
        draw_close_btn(1,0);
        draw_launch_btn(0,1);
        SetRect(&screen_rect2,0,0,0x280,0x1e0);
        gfx_dissolve_blit((iptr)&control_list,(iptr)&screen_rect2,5,1);
        draw_close_btn(1,1);
        value = game_timer_create((TimerFn)spaceship_wnd_type_tick,10,0xffffffff);
        anim_timer = value;
        setup_hotspots();
        draw_buttons();
        launched = 0;
        typing_label_part = 1;
        ((WndBase *)frame_wnd_base)->set_capture();
        input_flush_events();
        if (2 < game.multiplayer) {
          ((ModalMenuWindow *)frame_wnd_base)->set_modal_idle_callback(spaceship_wnd_net_idle);
        }
        editor_text_edit_block.spaceship_wnd_open_ticks_i32 = timer_ticks();
        ((ModalMenuWindow *)frame_wnd_base)->run_modal_loop();
        ((ModalMenuWindow *)frame_wnd_base)->set_modal_idle_callback(0);
        ((WndBase *)frame_wnd_base)->release_capture();
        if (flight_timer != 0) {
          game_timer_destroy(flight_timer);
          flight_timer = 0;
        }
        if (anim_timer != 0) {
          game_timer_destroy(anim_timer);
          anim_timer = 0;
        }
        if (blink_timer != 0) {
          game_timer_destroy(blink_timer);
          blink_timer = 0;
        }
        main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
        if (launch_state == 2) {
          play_launch_video();
          launch_state = 3;
          spaceship_launch(ship_civ);
          run_modal();
        }
        else {
          ((WndBase *)frame_wnd_base)->hide();
          wnd_flush_paint();
        }
        set_palette((iptr)&game_palette_block.game_palette);
        hide();
        wnd_flush_paint();
      }
    }
  }
  return;
}

/* 004d1725 auto */
void SpaceshipWindow::launch_sequence()

{
  handle_show_raise();
  wnd_flush_paint();
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)(ship_palette));
  set_palette((iptr)(ship_palette));
  launch_state = 2;
  play_launch_video();
  launch_state = 3;
  run_modal();
  main_frame_wnd_block.main_frame_wnd.set_palette((iptr)&game_palette_block.game_palette);
  set_palette((iptr)&game_palette_block.game_palette);
  hide();
  wnd_flush_paint();
  return;
}

/* 004d17bf: written by hand in src/game/bigframes.cpp */

/* 004d4a7b auto */
void SpaceshipWindow::build_stats_text()

{
  short part_count;
  bool over_limit;
  int part_limit;
  int part_index;
  char text[0x1400];
  char text2[0x200];
  
  over_limit = false;
  str_clear((char *)text);
  for (part_index = 0; part_index < 6; part_index++) {
    part_count = civs[(ship_civ)].spaceship_parts[part_index];
    part_limit = spaceship_part_limit(ship_civ,part_index);
    if (part_limit < part_count) {
      over_limit = true;
    }
  }
  for (part_index = 0; part_index < 6; part_index++) {
    str_append_string_id((iptr)text,
                       label_table.string_ids[spaceship_part_table_block.spaceship_parts[part_index].label_id])
    ;
    str_append_colon((char *)text);
    str_append_int((iptr)text,
                       (int)civs[(ship_civ)].spaceship_parts[part_index]);
    if ((over_limit) && (part_index == 0)) {
      str_append((char *)text,(const char *)&str_empty_127);
    }
    str_append((char *)text,(const char *)&str_empty_128);
  }
  str_append_label((iptr)text,0x22);
  str_append_colon((char *)text);
  str_append_int((iptr)text,(int)civs[ship_civ].spaceship_parts[3]);
  str_append((char *)text,(const char *)&str_0_000_6);
  str_append((char *)text,(const char *)&str_empty_129);
  str_append_label((iptr)text,0x42);
  str_append_colon((char *)text);
  str_append_int((iptr)text,spaceship_stats.support_pct);
  str_append_percent((iptr)text);
  str_append((char *)text,(const char *)&str_empty_130);
  str_append_label((iptr)text,0xcd);
  str_append_colon((char *)text);
  str_append_int((iptr)text,spaceship_stats.energy_pct);
  str_append_percent((iptr)text);
  str_append((char *)text,(const char *)&str_empty_131);
  str_append_label((iptr)text,0xce);
  str_append_colon((char *)text);
  str_append_int((iptr)text,spaceship_stats.mass / 10);
  str_append((char *)text,(const char *)&str_empty_132);
  str_append_int((iptr)text,spaceship_stats.mass % 10);
  str_append((char *)text,(const char *)&str_00);
  str_append_label((iptr)text,0xd1);
  str_append((char *)text,(const char *)&str_empty_133);
  str_append_label((iptr)text,200);
  str_append_colon((char *)text);
  str_append_int((iptr)text,spaceship_stats.fuel_pct);
  str_append_percent((iptr)text);
  if ((civs[ship_civ].spaceship_flags & SHIP_FUSION) != 0) {
    str_append_space((iptr)text);
    str_append_lparen((iptr)text);
    str_append_label((iptr)text,0xeb);
    str_append_rparen((iptr)text);
  }
  str_append((char *)text,(const char *)&str_empty_134);
  str_append_label((iptr)text,0xcf);
  str_append_colon((char *)text);
  str_append_int((iptr)text,spaceship_stats.flight_time / 10);
  str_append((char *)text,(const char *)&str_empty_135);
  str_append_int((iptr)text,spaceship_stats.flight_time % 10);
  str_append_space((iptr)text);
  str_append_label((iptr)text,0xd2);
  str_clear((char *)text2);
  str_append_label((iptr)text2,0xd0);
  str_append_colon((char *)text2);
  str_append_space((iptr)text2);
  str_append_int((iptr)text2,spaceship_stats.success_pct);
  str_append_percent((iptr)text2);
  /* BF-190 (Peter Johnson's ledger): the texts composed apart and cut to the
     512 bytes the statistics have (to success_text) and success_text's 128
     (the original appended the labels in place without a bound) */
  str_copy_max((char *)stats_text,text,(size_t)(success_text - stats_text));
  str_copy_max((char *)success_text,text2,sizeof success_text);
  win_flush_pending_msgs();
  return;
}

/* 004d4fd1 auto */
uint SpaceshipWindow::type_char(int allow_skip)

{
  int metric;
  uint result;
  i32 font_handle;
  int char_width;
  u32 timer;
  u8 *char_ptr;
  int yBottom;
  u8 char_buf[4];
  tagRECT dirty_rect;
  
  if ((allow_skip == 0) || (metric = rand_range(0,1), metric == 0)) {
    font_handle = ((GameFont *)typing_font)->get_handle();
    metric = font_measure_height(font_handle);
    win_flush_pending_msgs();
    char_buf[0] = stats_text[type_index];
    char_buf[1] = 0;
    gfx_set_text_color_index(0xf9);
    if (typing_label_part == 0) {
      gfx_set_text_color_index(0xfe);
    }
    else {
      gfx_set_text_color_index(0xf9);
    }
    ((DibSurface *)&control_list)->draw_text((iptr)(typing_font),(const char *)char_buf,
               type_x,type_y,1);
    yBottom = type_y + metric * 2;
    char_ptr = char_buf;
    font_handle = ((GameFont *)typing_font)->get_handle();
    char_width = font_measure_width(font_handle,(const char *)char_ptr);
    SetRect(&dirty_rect,type_x,type_y - metric,type_x + char_width,yBottom);
    char_ptr = char_buf;
    font_handle = ((GameFont *)typing_font)->get_handle();
    metric = font_measure_width(font_handle,(const char *)char_ptr);
    type_x += metric;
    if (stats_text[type_index] == ':') {
      typing_label_part = 0;
    }
    type_index++;
    while (stats_text[type_index] == '\n') {
      typing_label_part = 1;
      type_x = 10;
      font_handle = ((GameFont *)typing_font)->get_handle();
      metric = font_measure_height(font_handle);
      type_y += metric;
      type_index++;
    }
    if (stats_text[type_index] == '\0') {
      if (anim_timer != 0) {
        game_timer_destroy(anim_timer);
        anim_timer = 0;
      }
      if (launch_state != 3) {
        SetRect((LPRECT)(status_line_rect),0,0x1c1,0x27f,0x1df);
        rect_offset((iptr)(status_line_rect),1,1);
        gfx_set_text_color_index(10);
        ((DibSurface *)&control_list)->draw_text_in_rect((iptr)(medium_font),
                   (const char *)success_text,(RECT *)(iptr)(status_line_rect),0);
        rect_offset((iptr)(status_line_rect),0xffffffff,0xffffffff);
        if (spaceship_stats.success_pct < 0x28) {
          gfx_set_text_color_index(0xf9);
        }
        else if (spaceship_stats.success_pct < 0x4b) {
          gfx_set_text_color_index(0xfb);
        }
        else {
          gfx_set_text_color_index(0xfa);
        }
        ((DibSurface *)&control_list)->draw_text_in_rect((iptr)(medium_font),
                   (const char *)success_text,(RECT *)(iptr)(status_line_rect),0);
        ((DibSurface *)&control_list)->wnd_blit_surface_rect((iptr)(status_line_rect));
        wnd_flush_paint();
      }
      if (launch_state == 1) {
        timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
        blink_timer = timer;
      }
      launched = 1;
    }
    ((DibSurface *)&control_list)->wnd_blit_surface_rect((iptr)&dirty_rect);
    result = (uint)(stats_text[type_index] != '\0');
  }
  else {
    result = 1;
  }
  return result;
}

/* 004d53ab: written by hand in src/game/unwind.cpp */

/* 004d570b auto */
void SpaceshipWindow::init_starfield()

{
  i32 rand_val;
  int value;
  int xRight;
  iptr label;
  i8 clip_rect[16];
  int star;
  int star_y;
  int star_x;
  tagRECT fill_rect;
  
  for (star = 0; star < 0x50; star++) {
    rand_val = rand_range(0xffff8300,32000);
    stars[star].x = rand_val;
    rand_val = rand_range(0xffffa240,24000);
    stars[star].y = rand_val;
    rand_val = rand_range(1,100);
    stars[star].depth = rand_val;
  }
  value = ((DibSurface *)&control_list)->get_height();
  xRight = ((DibSurface *)&control_list)->get_width();
  SetRect(&fill_rect,0,0x14,xRight,value);
  ((DibSurface *)&control_list)->fill_rect((iptr)&fill_rect,10);
  for (star = 0; star < 0x50; star++) {
    star_x = stars[star].x /
               stars[star].depth + 0x140;
    star_y = stars[star].y /
               stars[star].depth + 0xf0;
    if ((((0x280 < star_x) || (star_x < 0)) || (0x1e0 < star_y)) || (star_y < 0)) {
      rand_val = rand_range(0xffff8300,32000);
      stars[star].x = rand_val;
      rand_val = rand_range(0xffffa240,24000);
      stars[star].y = rand_val;
      stars[star].depth = 100;
      star_x = stars[star].x /
                 stars[star].depth + 0x140;
      star_y = stars[star].y /
                 stars[star].depth + 0xf0;
    }
    ((DibSurface *)&control_list)->set_pixel(star_x,star_y,0x29);
  }
  type_index = 0;
  type_x = 10;
  type_y = 0x73;
  typing_label_part = 1;
  do {
    value = type_char(0);
  } while (value != 0);
  {
    /* BF-190: composed apart and cut to success_text's 128 bytes */
    char text[0x400];

    str_clear(text);
    label = (iptr)label_string(label_table.string_ids[0x1c6]);
    str_append(text,(const char *)label);
    str_append_colon(text);
    str_append_space((iptr)text);
    str_append_year((iptr)text,(int)civs[ship_civ].spaceship_arrival_year);
    str_copy_max((char *)success_text,text,sizeof success_text);
  }
  SetRect((LPRECT)(status_line_rect),0,0x1c1,0x27f,0x1df);
  rect_offset((iptr)(status_line_rect),1,1);
  gfx_set_text_color_index(10);
  ((DibSurface *)&control_list)->draw_text_in_rect((iptr)(medium_font),
             (const char *)success_text,(RECT *)(iptr)(status_line_rect),0);
  rect_offset((iptr)(status_line_rect),0xffffffff,0xffffffff);
  gfx_set_text_color_index(0xfa);
  ((DibSurface *)&control_list)->draw_text_in_rect((iptr)(medium_font),
             (const char *)success_text,(RECT *)(iptr)(status_line_rect),0);
  ((DibSurface *)&control_list)->wnd_blit_surface_rect((iptr)(status_line_rect));
  ((Sprite *)ship_image_sprite)->draw((iptr)clip_rect,(iptr)&control_list,0xaf,99);
  if (launch_state == 3) {
    type_y = 0x8c;
  }
  return;
}

/* 004d5b21 auto */
void SpaceshipWindow::play_launch_video()

{
  AviPlayer *this_00;
  int found;
  AviPlayer *player;
  i8 video_path[128];
  int open_result;
  
  snd_cd_stop();
  this_00 = (AviPlayer *)operator_new(0xa28);
  if (this_00 == (AviPlayer *)0x0) {
    player = 0;
  }
  else {
    player = this_00->construct();
  }
  launch_video_player = (u32)player;
  str_clear((char *)video_path);
  str_append((char *)video_path,(const char *)&str_civ2_video_launch_avi);
  found = file_resolve_path((const char *)video_path);
  if (found != 0) {
    ((AviPlayer *)launch_video_player)->create((iptr)&str_empty_136,0x800,0,0);
    ((AviPlayer *)launch_video_player)->set_double_size(1);
    open_result = ((AviPlayer *)launch_video_player)->open((const char *)video_path);
    if (open_result != 0) {
      if (open_result == -0x7ffbfeac) {
        popup_game((const char *)&str_vfwnotregistered_6);
      }
      return;
    }
    ((WndBase *)((u8 *)this + 0x100))->hide();
    wnd_flush_paint();
    ((DibSurface *)launch_video_player)->fill(0);
    ((WndHandlers *)(launch_video_player + 0x58))->set_lbuttonup_handler(spaceship_video_on_lbuttonup);
    ((WndHandlers *)(launch_video_player + 0x58))->set_keydown_handler(spaceship_wnd_on_key);
    ((AviPlayer *)launch_video_player)->set_end_callback(spaceship_video_on_end);
    ((WndBase *)(launch_video_player + 0x48))->center();
    ((DibSurface *)launch_video_player)->wnd_blit_surface();
    ((SurfaceWindow *)launch_video_player)->SurfaceWindow::show();
    wnd_flush_paint();
    ((AviPlayer *)launch_video_player)->play();
    input_flush_events();
    ((WndBase *)(launch_video_player + 0x48))->set_capture();
    if (2 < game.multiplayer) {
      ((ModalMenuWindow *)(launch_video_player + 0x48))->set_modal_idle_callback(net_modal_idle_poll);
    }
    ((ModalMenuWindow *)(launch_video_player + 0x48))->run_modal_loop();
    ((ModalMenuWindow *)(launch_video_player + 0x48))->set_modal_idle_callback(0);
    ((WndBase *)(launch_video_player + 0x48))->release_capture();
    ((WndBase *)(launch_video_player + 0x48))->hide();
    wnd_flush_paint();
  }
  if ((AviPlayer *)launch_video_player != (AviPlayer *)0x0) {
    ((AviPlayer *)launch_video_player)->scalar_destruct(1);
  }
  launch_video_player = 0;
  snd_sync_music_option();
  return;
}

/* 004d5e41 auto */
void SpaceshipWindow::setup_hotspots()

{
  ((ClickMap *)hotspot_map)->clear();
  if (((ship_civ == human_civ[0]) && (launch_state != 3)) && (launch_state != 0)) {
    ((ClickMap *)hotspot_map)->add_xywh(0,0x1f5,0x1a1,0x34,0x24);
    ((ClickMap *)hotspot_map)->add_xywh(1,0x22f,0x1a1,0x43,0x24);
  }
  else {
    ((ClickMap *)hotspot_map)->add_xywh(0,0x22f,0x1a1,0x43,0x24);
  }
  return;
}

/* 004d5ef9 auto */
void SpaceshipWindow::draw_buttons()

{
  tagRECT btn_rect;
  
  if ((launch_state != 1) && (launch_state != 0)) {
    SetRect(&btn_rect,0x1f5,0x1a1,0x229,0x1c5);
    ((DibSurface *)&control_list)->fill_rect((iptr)&btn_rect,0);
  }
  draw_close_btn(1,1);
  draw_launch_btn(0,1);
  return;
}

/* 004d5f79 auto */
void SpaceshipWindow::draw_close_btn(int enabled,int raised)

{
  i8 pressed_clip[16];
  i8 normal_clip[16];
  i8 disabled_clip[16];
  tagRECT btn_rect;
  
  if (((ship_civ == human_civ[0]) && (launch_state != 3)) && (launch_state != 0)) {
    SetRect(&btn_rect,0x1f5,0x1a1,0x229,0x1c5);
  }
  else {
    SetRect(&btn_rect,0x22f,0x1a1,0x272,0x1c5);
  }
  if (raised == 0) {
    ((Sprite *)close_btn_disabled_sprite)->draw((iptr)disabled_clip,(iptr)&control_list,
               btn_rect.left,btn_rect.top);
  }
  else if (enabled == 0) {
    ((Sprite *)close_btn_pressed_sprite)->draw((iptr)pressed_clip,(iptr)&control_list,
               btn_rect.left,btn_rect.top);
  }
  else {
    ((Sprite *)close_btn_normal_sprite)->draw((iptr)normal_clip,(iptr)&control_list,
               btn_rect.left,btn_rect.top);
  }
  ((DibSurface *)&control_list)->wnd_blit_surface_rect((iptr)&btn_rect);
  wnd_flush_paint();
  return;
}

/* 004d60a5 auto */
void SpaceshipWindow::draw_launch_btn(int enabled,int raised)

{
  i8 pressed_clip[16];
  i8 normal_clip[16];
  i8 disabled_clip[16];
  tagRECT btn_rect;
  
  if ((((ship_civ == human_civ[0]) && (launch_state != 3)) && (launch_state != 0)) &&
     (launch_state != 2)) {
    SetRect(&btn_rect,0x22f,0x1a1,0x272,0x1c5);
    if (raised == 0) {
      ((Sprite *)launch_btn_disabled_sprite)->draw((iptr)disabled_clip,(iptr)&control_list,
                 btn_rect.left,btn_rect.top);
    }
    else if (enabled == 0) {
      ((Sprite *)launch_btn_pressed_sprite)->draw((iptr)pressed_clip,(iptr)&control_list,
                 btn_rect.left,btn_rect.top);
    }
    else {
      ((Sprite *)launch_btn_normal_sprite)->draw((iptr)normal_clip,(iptr)&control_list,
                 btn_rect.left,btn_rect.top);
    }
    ((DibSurface *)&control_list)->wnd_blit_surface_rect((iptr)&btn_rect);
    wnd_flush_paint();
  }
  return;
}

/* 004d61c3 auto */
void __cdecl spaceship_video_on_lbuttonup(i32 x,i32 y)

{
  int hit;
  int hotspot_id;
  SpaceshipWindow *ship_wnd;
  int active_wnd;
  
  active_wnd = wnd_get_active_child();
  if (active_wnd == 0) {
    active_wnd = 0;
  }
  else {
    active_wnd -= 0x48;
  }
  ship_wnd = spaceship_launch_sprite_xs_block.spaceship_wnd;
  if ((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_state == 2) {
    ((ModalMenuWindow *)((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player + 0x48))->end_modal_loop();
  }
  else {
    hit = ((ClickMap *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xae4))->hit_test(x,y,(iptr)&hotspot_id,0);
    if (hit != -1) {
      if (hotspot_id == 0) {
        ship_wnd->draw_close_btn(1,0);
        if (ship_wnd->launched == 0) {
          if (ship_wnd->anim_timer != 0) {
            game_timer_destroy(ship_wnd->anim_timer);
            ship_wnd->anim_timer = 0;
          }
          if (ship_wnd->launch_state != 3) {
            do {
              hit = ship_wnd->type_char(0);
            } while (hit != 0);
            ship_wnd->launched = 1;
            ship_wnd->draw_close_btn(1,1);
          }
        }
        else {
          ((ModalMenuWindow *)(active_wnd + 0x48))->end_modal_loop();
        }
      }
      else if (((hotspot_id == 1) && (ship_wnd->launch_state == 1)) && (ship_wnd->launched != 0)) {
        ship_wnd->draw_launch_btn(1,0);
        ship_wnd->launch_state = 2;
        ((ModalMenuWindow *)(active_wnd + 0x48))->end_modal_loop();
      }
    }
  }
  return;
}

/* 004d6367 auto */
void __cdecl spaceship_wnd_type_tick(void)

{
  spaceship_launch_sprite_xs_block.spaceship_wnd->type_char(1);
  return;
}

/* 004d6384 auto */
void __cdecl spaceship_wnd_blink_launch_btn(void)

{
  SpaceshipWindow *self;
  
  self = spaceship_launch_sprite_xs_block.spaceship_wnd;
  if (spaceship_launch_sprite_xs_block.spaceship_wnd->spaceship_state == 0) {
    spaceship_launch_sprite_xs_block.spaceship_wnd->spaceship_state = 1;
  }
  else {
    spaceship_launch_sprite_xs_block.spaceship_wnd->spaceship_state = 0;
  }
  if (self->spaceship_state == 0) {
    self->draw_launch_btn(1,1);
  }
  else {
    self->draw_launch_btn(0,1);
  }
  return;
}

/* 004d63fb auto */
void __cdecl spaceship_wnd_starfield_tick(void)

{
  int old_x;
  int old_y;
  i32 rand_val;
  int pixel;
  SpaceshipWindow *ship_wnd;
  int star;
  int new_y;
  int new_x;
  
  ship_wnd = spaceship_launch_sprite_xs_block.spaceship_wnd;
  win_flush_pending_msgs();
  for (star = 0; star < 0x50; star++) {
    old_x = ship_wnd->stars[star].x / ship_wnd->stars[star].depth +
            0x140;
    old_y = ship_wnd->stars[star].y / ship_wnd->stars[star].depth +
            0xf0;
    ship_wnd->stars[star].x += 0xfa;
    ship_wnd->stars[star].y += 300;
    ship_wnd->stars[star].depth--;
    if (ship_wnd->stars[star].depth == 0) {
      rand_val = rand_range(0xffff8300,32000);
      ship_wnd->stars[star].x = rand_val;
      rand_val = rand_range(0xffffa240,24000);
      ship_wnd->stars[star].y = rand_val;
      rand_val = rand_range(0x32,100);
      ship_wnd->stars[star].depth = rand_val;
    }
    new_x = ship_wnd->stars[star].x / ship_wnd->stars[star].depth
               + 0x140;
    new_y = ship_wnd->stars[star].y / ship_wnd->stars[star].depth
               + 0xf0;
    if ((((0x280 < new_x) || (new_x < 0)) || (0x1e0 < new_y)) || (new_y < 0)) {
      rand_val = rand_range(0xffff8300,32000);
      ship_wnd->stars[star].x = rand_val;
      rand_val = rand_range(0xffffa240,24000);
      ship_wnd->stars[star].y = rand_val;
      rand_val = rand_range(0x32,100);
      ship_wnd->stars[star].depth = rand_val;
      new_x = ship_wnd->stars[star].x /
                 ship_wnd->stars[star].depth + 0x140;
      new_y = ship_wnd->stars[star].y /
                 ship_wnd->stars[star].depth + 0xf0;
    }
    if (((old_x < 0x280) && (0 < old_x)) && ((old_y < 0x1e0 && (0 < old_y)))) {
      pixel = ((DibSurface *)((iptr)ship_wnd + 0xb8))->get_pixel(old_x,old_y);
      if (pixel == 0x29) {
        ((DibSurface *)((iptr)ship_wnd + 0xb8))->set_pixel(old_x,old_y,10);
      }
    }
    if (((new_x < 0x280) && (0 < new_x)) && ((new_y < 0x1e0 && (0 < new_y)))) {
      old_x = ((DibSurface *)((iptr)ship_wnd + 0xb8))->get_pixel(new_x,new_y);
      if (old_x == 10) {
        ((DibSurface *)((iptr)ship_wnd + 0xb8))->set_pixel(new_x,new_y,0x29);
      }
    }
  }
  ((DibSurface *)((iptr)ship_wnd + 0xb8))->wnd_blit_surface();
  return;
}

/* 004d6744 auto */
void __cdecl spaceship_wnd_on_key(int key)

{
  SpaceshipWindow *self;
  int more;
  int active_wnd;
  
  active_wnd = wnd_get_active_child();
  self = spaceship_launch_sprite_xs_block.spaceship_wnd;
  if (active_wnd == 0) {
    active_wnd = 0;
  }
  else {
    active_wnd -= 0x48;
  }
  if ((0xcf < key) && (key < 0xd3)) {
    if ((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_state == 2) {
      ((ModalMenuWindow *)((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player + 0x48))->end_modal_loop();
    }
    else {
      spaceship_launch_sprite_xs_block.spaceship_wnd->draw_close_btn(1,0);
      if (self->launched == 0) {
        if (self->anim_timer != 0) {
          game_timer_destroy((i32)self->anim_timer);
          self->anim_timer = 0;
        }
        do {
          more = self->type_char(0);
        } while (more != 0);
        self->launched = 1;
        self->draw_close_btn(1,1);
      }
      else {
        ((ModalMenuWindow *)(active_wnd + 0x48))->end_modal_loop();
      }
    }
  }
  return;
}

/* 004d686b auto */
void __cdecl spaceship_video_on_end(void)

{
  if (spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player != 0) {
    ((ModalMenuWindow *)((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player + 0x48))->end_modal_loop();
  }
  return;
}

/* 004d68a7 auto */
void __cdecl spaceship_wnd_on_lbuttondown(i32 x,i32 y)

{
  int hit;
  int hotspot_id;
  SpaceshipWindow *ship_wnd;
  
  ship_wnd = spaceship_launch_sprite_xs_block.spaceship_wnd;
  if (((spaceship_launch_sprite_xs_block.spaceship_wnd->launch_btn_pressed == 0) && (spaceship_launch_sprite_xs_block.spaceship_wnd->close_btn_pressed == 0)) &&
     (hit = ((ClickMap *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xae4))->hit_test(x,y,(iptr)&hotspot_id,0),
     hit != -1)) {
    if (hotspot_id == 0) {
      (ship_wnd)->close_btn_pressed = 1;
      ship_wnd->draw_close_btn(1,0);
      if ((ship_wnd->launch_state == 1) && (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
      ((DibSurface *)&ship_wnd->control_list)->wnd_blit_surface();
    }
    else if (((hotspot_id == 1) && (ship_wnd->launch_state == 1)) && (ship_wnd->launched != 0)) {
      (ship_wnd)->launch_btn_pressed = 1;
      ship_wnd->draw_launch_btn(1,0);
      if ((ship_wnd->launch_state == 1) && (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
      ((DibSurface *)&ship_wnd->control_list)->wnd_blit_surface();
    }
  }
  return;
}

/* 004d6a30 auto */
void __cdecl spaceship_wnd_on_lbuttonup(i32 x,i32 y)

{
  int hit;
  u32 timer;
  int hotspot_id;
  SpaceshipWindow *ship_wnd;
  int active_wnd;
  
  ship_wnd = spaceship_launch_sprite_xs_block.spaceship_wnd;
  active_wnd = wnd_get_active_child();
  if (active_wnd == 0) {
    active_wnd = 0;
  }
  else {
    active_wnd -= 0x48;
  }
  if (ship_wnd->launch_state == 2) {
    ((ModalMenuWindow *)((int)(ship_wnd)->launch_video_player + 0x48))->end_modal_loop();
  }
  else if (((ship_wnd)->launch_btn_pressed != 0) ||
          ((ship_wnd)->close_btn_pressed != 0)) {
    if ((ship_wnd)->close_btn_pressed != 0) {
      ship_wnd->draw_close_btn(1,1);
    }
    if ((ship_wnd)->launch_btn_pressed != 0) {
      ship_wnd->draw_launch_btn(1,1);
    }
    hit = ((ClickMap *)ship_wnd->hotspot_map)->hit_test(x,y,(iptr)&hotspot_id,0);
    if (hit != -1) {
      if (hotspot_id == 0) {
        if ((ship_wnd)->close_btn_pressed != 0) {
          if (ship_wnd->launched == 0) {
            if (ship_wnd->anim_timer != 0) {
              game_timer_destroy(ship_wnd->anim_timer);
              ship_wnd->anim_timer = 0;
            }
            if (ship_wnd->launch_state != 3) {
              do {
                hit = ship_wnd->type_char(0);
              } while (hit != 0);
              ship_wnd->launched = 1;
            }
          }
          else {
            ((ModalMenuWindow *)(active_wnd + 0x48))->end_modal_loop();
          }
        }
      }
      else if ((hotspot_id == 1) && ((ship_wnd)->launch_btn_pressed != 0)) {
        ship_wnd->launch_state = 2;
        ((ModalMenuWindow *)(active_wnd + 0x48))->end_modal_loop();
      }
    }
    if ((((ship_wnd)->close_btn_pressed != 0) &&
        ((ship_wnd)->close_btn_pressed = 0, ship_wnd->launch_state == 1)
        ) && (ship_wnd->blink_timer == 0)) {
      timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
      ship_wnd->blink_timer = timer;
    }
    if ((((ship_wnd)->launch_btn_pressed != 0) &&
        ((ship_wnd)->launch_btn_pressed = 0, ship_wnd->launch_state == 1)
        ) && (ship_wnd->blink_timer == 0)) {
      timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
      ship_wnd->blink_timer = timer;
    }
  }
  return;
}

/* 004d6cbc auto */
void __cdecl spaceship_wnd_on_mousemove(i32 x,i32 y)

{
  int hit;
  u32 timer;
  int hotspot_id;
  SpaceshipWindow *ship_wnd;
  
  ship_wnd = spaceship_launch_sprite_xs_block.spaceship_wnd;
  hit = ((ClickMap *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xae4))->hit_test(x,y,(iptr)&hotspot_id,0);
  if (hit == -1) {
    if ((((ship_wnd)->close_btn_pressed != 0) &&
        (ship_wnd->draw_close_btn(1,1), ship_wnd->launch_state == 1)) &&
       (ship_wnd->blink_timer == 0)) {
      timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
      ship_wnd->blink_timer = timer;
    }
    if ((((ship_wnd)->launch_btn_pressed != 0) &&
        (ship_wnd->draw_launch_btn(1,1), ship_wnd->launch_state == 1)) &&
       (ship_wnd->blink_timer == 0)) {
      timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
      ship_wnd->blink_timer = timer;
    }
  }
  else if (hotspot_id == 0) {
    if ((ship_wnd)->launch_btn_pressed == 0) {
      if ((((ship_wnd)->close_btn_pressed != 0) &&
          (ship_wnd->draw_close_btn(1,0), ship_wnd->launch_state == 1)) &&
         (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
    }
    else {
      ship_wnd->draw_launch_btn(1,1);
      if ((ship_wnd->launch_state == 1) && (ship_wnd->blink_timer == 0)) {
        timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
        ship_wnd->blink_timer = timer;
      }
    }
  }
  else if (hotspot_id == 1) {
    if ((ship_wnd)->close_btn_pressed == 0) {
      if ((((ship_wnd)->launch_btn_pressed != 0) &&
          (ship_wnd->draw_launch_btn(1,0), ship_wnd->launch_state == 1)) &&
         (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
    }
    else {
      ship_wnd->draw_close_btn(1,1);
      if ((ship_wnd->launch_state == 1) && (ship_wnd->blink_timer == 0)) {
        timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
        ship_wnd->blink_timer = timer;
      }
    }
  }
  return;
}

/* 00596b00 auto */
iptr __cdecl spaceship_part_limit(iptr civ,int part)

{
  int structurals;
  iptr limit;
  
  limit = (((int *)&spaceship_part_table_block.spaceship_part_max_counts))[part * 3];
  if ((part == 1) || (part == 2)) {
    structurals = (civs[civ].spaceship_parts[0] + 1) - (part * 2 - 2);
    limit = int_clamp(limit,0,(int)(structurals + (structurals >> 0x1f & 3U)) >> 2);
  }
  else if (part != 0) {
    structurals = (civs[civ].spaceship_parts[0] + 1) - (part * 4 - 0xc);
    limit = int_clamp(limit,0,(int)(structurals + (structurals >> 0x1f & 7U)) >> 3);
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      limit = int_clamp(limit,0,1);
    }
    if (limit < 0) {
      limit = 0;
    }
  }
  return limit;
}

/* 00596c08 auto */
i32 __cdecl spaceship_part_count(iptr civ,int part)

{
  short built;
  i32 result;
  
  built = civs[civ].spaceship_parts[part];
  result = spaceship_part_limit(civ,part);
  result = int_clamp((int)built,0,result);
  return result;
}

/* 00596c61 auto */
int __cdecl spaceship_group_limit(iptr civ,int group)

{
  int total;
  int limit_a;
  int limit_b;
  
  if (group == 0) {
    total = spaceship_part_limit(civ,0);
  }
  else if (group == 1) {
    limit_a = spaceship_part_limit(civ,2);
    total = spaceship_part_limit(civ,1);
    total += limit_a;
  }
  else {
    limit_a = spaceship_part_limit(civ,3);
    limit_b = spaceship_part_limit(civ,4);
    total = spaceship_part_limit(civ,5);
    total = total + limit_a + limit_b;
  }
  return total;
}

/* 00596ced auto */
int __cdecl spaceship_group_max(int group)

{
  int max_count;
  
  max_count = spaceship_part_table_block.spaceship_part_max_counts[0];
  if (group != 0) {
    if (group == 1) {
      max_count = spaceship_part_table_block.spaceship_fuel_max + spaceship_part_table_block.spaceship_propulsion_max;
    }
    else {
      max_count = spaceship_part_table_block.spaceship_solar_panel_max + spaceship_part_table_block.spaceship_habitation_max + spaceship_part_table_block.spaceship_life_support_max;
    }
  }
  return max_count;
}

/* 00596d3c auto */
int __cdecl spaceship_group_count(iptr civ,iptr group)

{
  int count;
  
  if (group == 0) {
    count = (int)civs[civ].spaceship_parts[0];
  }
  else if (group == 1) {
    count = (int)civs[civ].spaceship_parts[1] + (int)civs[civ].spaceship_parts[2];
  }
  else {
    count = (int)civs[civ].spaceship_parts[3] + (int)civs[civ].spaceship_parts[4] +
            (int)civs[civ].spaceship_parts[5];
  }
  return count;
}

/* 00596e06 auto */
int __cdecl spaceship_group_count_clamped(iptr civ,int group)

{
  int total;
  int count_a;
  int count_b;
  
  if (group == 0) {
    total = spaceship_part_count(civ,0);
  }
  else if (group == 1) {
    count_a = spaceship_part_count(civ,2);
    total = spaceship_part_count(civ,1);
    total += count_a;
  }
  else {
    count_a = spaceship_part_count(civ,3);
    count_b = spaceship_part_count(civ,4);
    total = spaceship_part_count(civ,5);
    total = total + count_a + count_b;
  }
  return total;
}

/* 00596e92 auto */
int __cdecl spaceship_component_thrust(int count)

{
  int next_thrust;
  int i;
  i32 thrust;
  
  thrust = 0;
  for (i = 0; i < count; i++) {
    next_thrust = thrust + 1;
    if (3 < i) {
      next_thrust = thrust + 2;
    }
    thrust = next_thrust;
    if (5 < i) {
      thrust++;
    }
  }
  return thrust;
}

/* 00596eec auto */
void __cdecl spaceship_calc_stats(iptr civ,int apply_support)

{
  byte flags;
  int value;
  iptr part_count;
  int divisor;
  int temp;
  i32 arg_a;
  i32 arg_b;
  int part;
  int full_mass;
  int full_flight_time;
  int flight_time;
  uint paradigm;
  int scale;
  
  value = civ_spaceship_launched(civ);
  if (value == 0) {
    value = civ_has_advance(civ,ADV_FUSION_POWER);
    if (value != 0) {
      civs[civ].spaceship_flags = civs[civ].spaceship_flags | SHIP_FUSION;
    }
  }
  flags = civs[civ].spaceship_flags;
  spaceship_stats.mass = 0;
  for (part = 0; part < 6; part++) {
    spaceship_stats.mass = spaceship_stats.mass +
                   (int)civs[civ].spaceship_parts[part] *
                   spaceship_part_table_block.spaceship_parts[part].mass;
  }
  full_mass = spaceship_stats.mass;
  for (part = 0; part < 6; part++) {
    if (civs[civ].spaceship_parts[part] == 0) {
      full_mass += spaceship_part_table_block.spaceship_parts[part].mass;
    }
  }
  spaceship_stats.success_pct = 100;
  value = spaceship_part_count(civ,4);
  arg_b = 99;
  arg_a = 1;
  part_count = spaceship_part_count(civ,3);
  divisor = int_clamp(part_count,arg_a,arg_b);
  spaceship_stats.support_pct = (value * 100) / divisor;
  if (apply_support != 0) {
    value = int_clamp(spaceship_stats.support_pct,0,100);
    spaceship_stats.success_pct = (value * spaceship_stats.success_pct) / 100;
  }
  value = spaceship_part_count(civ,5);
  arg_b = 99;
  arg_a = 1;
  divisor = spaceship_part_count(civ,3);
  temp = spaceship_part_count(civ,4);
  divisor = int_clamp(divisor + temp,arg_a,arg_b);
  spaceship_stats.energy_pct = (value * 200) / divisor;
  value = int_clamp(spaceship_stats.energy_pct,0,100);
  spaceship_stats.success_pct = (value * spaceship_stats.success_pct) / 100;
  value = spaceship_part_count(civ,2);
  arg_b = 99;
  arg_a = 1;
  part_count = spaceship_part_count(civ,1);
  divisor = int_clamp(part_count,arg_a,arg_b);
  spaceship_stats.fuel_pct = (value * 100) / divisor;
  paradigm = (uint)cosmic.mass_thrust_paradigm;
  if ((flags & SHIP_FUSION) != 0) {
    paradigm = (int)(paradigm * 3) >> 2;
  }
  scale = 1;
  for (; 100 < paradigm; paradigm = (int)paradigm >> 1) {
    scale = scale << 1;
  }
  temp = spaceship_stats.mass * paradigm;
  arg_a = spaceship_part_count(civ,2);
  value = spaceship_component_thrust(arg_a);
  value = value * 10;
  arg_b = 0;
  arg_a = spaceship_part_count(civ,1);
  divisor = spaceship_component_thrust(arg_a);
  value = int_clamp(divisor * 10,arg_b,value);
  spaceship_stats.flight_time = temp / (value + 1);
  arg_a = spaceship_part_count(civ,2);
  value = spaceship_component_thrust(arg_a);
  value = value * 10;
  arg_b = 0;
  arg_a = spaceship_part_count(civ,1);
  divisor = spaceship_component_thrust(arg_a);
  value = int_clamp(divisor * 10,arg_b,value);
  flight_time = (int)(paradigm * full_mass) / (value + 1);
  value = spaceship_component_thrust((int)civs[civ].spaceship_parts[2]);
  value = value * 10;
  arg_a = 0;
  divisor = spaceship_component_thrust((int)civs[civ].spaceship_parts[1]);
  value = int_clamp(divisor * 10,arg_a,value);
  full_flight_time = (int)(paradigm * full_mass) / (value + 1);
  if (1 < scale) {
    spaceship_stats.flight_time = scale * spaceship_stats.flight_time;
    flight_time = scale * flight_time;
    full_flight_time = scale * full_flight_time;
  }
  if (0x96 < spaceship_stats.flight_time) {
    spaceship_stats.success_pct -= (spaceship_stats.flight_time - 0x96) / 10;
  }
  spaceship_stats.success_pct = int_clamp(spaceship_stats.success_pct,0,100);
  value = civ_spaceship_launched(civ);
  if (value == 0) {
    arg_a = spaceship_part_count(civ,3);
    civs[civ].spaceship_population = (i16)arg_a;
    arg_a = game_turn_to_year((int)game.turn);
    civs[civ].spaceship_launch_year = (i16)arg_a;
    arg_a = game_turn_to_year((int)game.turn);
    civs[civ].spaceship_arrival_year = (short)arg_a + (short)(flight_time / 10);
    value = game_turn_to_year((int)game.turn);
    spaceship_stats.projected_arrival_year = value + full_flight_time / 10;
  }
  return;
}

/* 005973fd auto */
void __cdecl spaceship_launch(iptr civ)

{
  uint *treaty;
  iptr name2;
  int launched;
  iptr city;
  iptr civ2;
  
  civs[civ].spaceship_flags = civs[civ].spaceship_flags | SHIP_LAUNCHED;
  if (game.first_launch_turn < 0) {
    game.first_launch_turn = game.turn;
  }
  name2 = (iptr)(civ_name_plural(civ));
  text_set_string_arg(0,(const char *)name2);
  text_set_number_arg(0,(int)civs[civ].spaceship_arrival_year);
  if (2 < game.multiplayer) {
    net_send_text_args(0xb,0xff,1,1,civ,0);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    popup_game((const char *)&str_launched_2);
    spaceship_wnd_run(-civ);
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        city_set_production(city,99);
      }
    }
  }
  else {
    for (civ2 = 1; civ2 < 8; civ2++) {
      if ((((civ2 != civ) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0))
          && (launched = civ_spaceship_launched(civ2), launched != 0)) &&
         (civs[civ].spaceship_arrival_year < civs[civ2].spaceship_arrival_year)) {
        if (game.difficulty < DIFF_EMPEROR) {
          if ((game.difficulty < DIFF_PRINCE) || (civs[civ].reputation == '\0')) {
            diplo_set_treaty_flags(civ,civ2,0x10000);
          }
          else {
            treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
            *treaty = *treaty | 0x20;
          }
        }
        else if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
          treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
          *treaty = *treaty & 0xffffffd9;
          treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
          *treaty = *treaty | 0x80840;
        }
        else {
          treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
          *treaty = *treaty | 0x20;
        }
      }
    }
  }
  return;
}

/* 0059772c auto */
void __cdecl spaceship_status_dialog(iptr civ,int allow_launch)

{
  short part_count;
  iptr str;
  int result;
  Dialog dlg;
  int part;
  int over_limit;
  
  dlg.Dialog::construct(0x2000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  over_limit = 0;
  spaceship_calc_stats(civ,1);
  if (civs[civ].spaceship_parts[3] == 0) {
    allow_launch = 0;
  }
  str = civ_get_adjective(civ);
  text_set_string_arg(2,(const char *)str);
  str = (iptr)(civ_leader_name(civ));
  text_set_string_arg(1,(const char *)str);
  str = (iptr)(civ_name_plural(civ));
  text_set_string_arg(0,(const char *)str);
  text_args.strings_plus1 = 0;
  __strupr(((i8 *)&text_args.strings));
  dlg.load_game_text((const char *)&str_spaceship);
  for (part = 0; part < 6; part++) {
    part_count = civs[civ].spaceship_parts[part];
    result = spaceship_part_limit(civ,part);
    if (result < part_count) {
      over_limit = 1;
    }
  }
  for (part = 0; part < 6; part++) {
    text_clear();
    text_append_string_id(label_table.string_ids[spaceship_part_table_block.spaceship_parts[part].label_id]
                      );
    text_append_colon();
    text_append_int((int)civs[civ].spaceship_parts[part]);
    if ((over_limit != 0) && (part == 0)) {
      text_append((const char *)&str_empty_176);
    }
    dlg.add_option((const char *)&textbuf.buf,0,0);
  }
  text_clear();
  dlg.add_option((const char *)&textbuf.buf,0,0);
  text_clear();
  text_append_label(0x22);
  text_append_colon();
  text_append_int((int)civs[civ].spaceship_parts[3]);
  text_append((const char *)&str_0_000_7);
  dlg.add_option((const char *)&textbuf.buf,0,0);
  text_clear();
  text_append_label(0x42);
  text_append_colon();
  text_append_int(spaceship_stats.support_pct);
  text_append_percent();
  dlg.add_option((const char *)&textbuf.buf,0,0);
  text_clear();
  text_append_label(0xcd);
  text_append_colon();
  text_append_int(spaceship_stats.energy_pct);
  text_append_percent();
  dlg.add_option((const char *)&textbuf.buf,0,0);
  text_clear();
  text_append_label(0xce);
  text_append_colon();
  text_append_int(spaceship_stats.mass / 10);
  text_append((const char *)&str_empty_177);
  text_append_int(spaceship_stats.mass % 10);
  text_append((const char *)&str_00_2);
  text_append_label(0xd1);
  dlg.add_option((const char *)&textbuf.buf,0,0);
  text_clear();
  text_append_label(200);
  text_append_colon();
  text_append_int(spaceship_stats.fuel_pct);
  text_append_percent();
  if ((civs[civ].spaceship_flags & SHIP_FUSION) != 0) {
    text_append_space();
    text_append_lparen();
    text_append_label(0xeb);
    text_append_rparen();
  }
  dlg.add_option((const char *)&textbuf.buf,0,0);
  text_clear();
  text_append_label(0xcf);
  text_append_colon();
  text_append_int(spaceship_stats.flight_time / 10);
  text_append((const char *)&str_empty_178);
  text_append_int(spaceship_stats.flight_time % 10);
  text_append_space();
  text_append_label(0xd2);
  dlg.add_option((const char *)&textbuf.buf,0,0);
  text_clear();
  text_append_label(0xd0);
  text_append_colon();
  text_append((const char *)&str_empty_179);
  text_append_int(spaceship_stats.success_pct);
  text_append_percent();
  text_append((const char *)&str_empty_180);
  dlg.add_option((const char *)&textbuf.buf,0,0);
  result = civ_spaceship_launched(civ);
  if (result == 0) {
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (allow_launch != 0)) &&
       (spaceship_stats.success_pct != 0)) {
      str = (iptr)label_string(label_table.string_ids[0xd5]);
      dlg.add_button((const char *)str);
    }
  }
  else {
    text_clear();
    text_append((const char *)&str_empty_181);
    if (civs[civ].spaceship_arrival_year < game.year) {
      text_append_label(0xd4);
    }
    else {
      text_append_label(0xd3);
      text_append((const char *)&str_empty_182);
      text_append_label(0xda);
      text_append_space();
      text_append_int((int)civs[civ].spaceship_arrival_year);
    }
    text_append((const char *)&str_empty_183);
    dlg.add_option((const char *)&textbuf.buf,0,0);
  }
  dlg.show_modal(0);
  if (dlg.result_extra != 0) {
    text_set_number_arg(0,spaceship_stats.success_pct);
    result = popup_game((const char *)&str_launch);
    if (result != 0) {
      spaceship_launch(civ);
    }
  }
  return;
}

/* 00597d6f auto */
iptr __cdecl spaceship_choose_part_group(iptr civ,iptr group)

{
  short earliest_arrival;
  bool race_close;
  bool chosen;
  int count;
  int limit;
  iptr civ2;
  int grp;
  
  spaceship_calc_stats(civ,0);
  count = civ_spaceship_launched(civ);
  if (count != 0) {
    return 0xffffffff;
  }
  earliest_arrival = 9999;
  for (civ2 = 1; civ2 < 8; civ2++) {
    if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
        (0 < civs[civ2].spaceship_arrival_year)) &&
       (civs[civ2].spaceship_arrival_year < earliest_arrival)) {
      earliest_arrival = civs[civ2].spaceship_arrival_year;
    }
  }
  if (((civs[civ].spaceship_arrival_year == 0) ||
      (0xe < (int)civs[civ].spaceship_arrival_year - (int)game.year)) ||
     (earliest_arrival <= civs[civ].spaceship_arrival_year)) {
    race_close = false;
  }
  else {
    race_close = true;
  }
  chosen = false;
  if (group == 0) {
    count = spaceship_group_count_clamped(civ,2);
    limit = spaceship_group_limit(civ,2);
    if (count < limit) {
      chosen = true;
    }
    else if (!race_close) {
      count = spaceship_group_count_clamped(civ,1);
      limit = spaceship_group_limit(civ,1);
      if (count < limit) {
        chosen = true;
      }
    }
  }
  if (!(((!chosen) && ((group != 1 || (!race_close)))) &&
        (count = spaceship_group_count_clamped(civ,group),
         limit = spaceship_group_limit(civ,group), count < limit) &&
        (count = spaceship_group_can_build(civ,group), count != 0))) {
    for (grp = 2; (int)(uint)(group == 0) <= grp; grp--) {
      count = spaceship_group_can_build(civ,grp);
      if ((count != 0) && ((grp != 1 || (!race_close)))) {
        count = spaceship_group_count_clamped(civ,grp);
        limit = spaceship_group_limit(civ,grp);
        if (count < limit) {
          group = grp;
          break;
        }
      }
    }
  }
  if (group == 0) {
    for (civ2 = 1; civ2 < 8; civ2++) {
      if (((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
           (count = civ_spaceship_launched(civ2), count != 0)) &&
          (civs[civ2].spaceship_arrival_year <= spaceship_stats.projected_arrival_year)) &&
         ((count = spaceship_group_full(civ,1), count == 0 &&
          (count = civ_has_advance(civ,(int)improvements[IMP_SS_COMPONENT].preq), count != 0)))) {
        earliest_arrival = civs[civ].spaceship_parts[1];
        count = spaceship_part_limit(civ,1);
        if (earliest_arrival <= count) {
          return 1;
        }
        earliest_arrival = civs[civ].spaceship_parts[2];
        count = spaceship_part_limit(civ,2);
        if (earliest_arrival <= count) {
          return 1;
        }
      }
    }
  }
  count = spaceship_group_can_build(civ,group);
  if (count == 0) {
    group = -2;
  }
  return group;
}

/* 00598197 auto */
int __cdecl spaceship_add_part(iptr civ,iptr group)

{
  short *part_ptr;
  short min_count;
  bool should_launch;
  int result;
  iptr civ2;
  int part;
  iptr civ3;
  int p;
  iptr civ4;
  
  part = -1;
  spaceship_calc_stats(civ,0);
  result = civ_spaceship_launched(civ);
  if (result == 0) {
    while( true ) {
      if (group == 0) {
        if (spaceship_part_table_block.spaceship_part_max_counts[0] <= civs[civ].spaceship_parts[0]) {
          return part;
        }
        part = 0;
      }
      else if (group == 1) {
        if ((spaceship_part_table_block.spaceship_propulsion_max <= civs[civ].spaceship_parts[1]) &&
           (spaceship_part_table_block.spaceship_fuel_max <= civs[civ].spaceship_parts[2])) {
          return part;
        }
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
          if (civs[civ].spaceship_parts[1] < civs[civ].spaceship_parts[2]) {
            part = 1;
          }
          else {
            part = 2;
          }
        }
        else {
          text_set_number_arg(0,(int)civs[civ].spaceship_parts[1]);
          text_set_number_arg(1,(int)civs[civ].spaceship_parts[2]);
          part = popup_game((const char *)&str_component);
          part++;
        }
      }
      else {
        if (((spaceship_part_table_block.spaceship_habitation_max <= civs[civ].spaceship_parts[3]) &&
            (spaceship_part_table_block.spaceship_life_support_max <= civs[civ].spaceship_parts[4])) &&
           (spaceship_part_table_block.spaceship_solar_panel_max <= civs[civ].spaceship_parts[5])) {
          return part;
        }
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
          min_count = 999;
          for (p = 3; p < 6; p++) {
            if (civs[civ].spaceship_parts[p] < min_count) {
              min_count = civs[civ].spaceship_parts[p];
              part = p;
            }
          }
        }
        else {
          text_set_number_arg(0,(int)civs[civ].spaceship_parts[3]);
          text_set_number_arg(1,(int)civs[civ].spaceship_parts[4]);
          text_set_number_arg(2,(int)civs[civ].spaceship_parts[5]);
          part = popup_game((const char *)&str_module);
          part += 3;
        }
      }
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
         ((int)civs[civ].spaceship_parts[part] < (((int *)&spaceship_part_table_block.spaceship_part_max_counts))[part * 3]
         )) break;
      text_set_label_arg(0,label_table.string_ids[spaceship_part_table_block.spaceship_parts[part].label_id]);
      popup_game((const char *)&str_nofurther);
    }
    result = civ_spaceship_exists(civ);
    if (result == 0) {
      civs[civ].spaceship_flags = civs[civ].spaceship_flags | SHIP_EXISTS;
      for (civ2 = 1; civ2 < 8; civ2++) {
        if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
           ((civ2 == civ || (result = civ_spaceship_exists(civ2), result == 0)))) {
          for (civ3 = 1; civ3 < 8; civ3++) {
            civs[civ2].last_contact_turn[civ3] = 0;
          }
        }
      }
    }
    part_ptr = (short *)(civ * 0x594 + (iptr)((u8 *)&civs + 1032) + part * 2);
    *part_ptr = *part_ptr + 1;
    spaceship_calc_stats(civ,0);
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      should_launch = true;
      for (part = 0; part < 3; part++) {
        if ((int)civs[civ].spaceship_parts[part] <
            (((int *)&spaceship_part_table_block.spaceship_part_max_counts))[part * 3]) {
          should_launch = false;
        }
      }
      result = civ_spaceship_launched(civ);
      if ((result == 0) && (0x27 < spaceship_stats.success_pct)) {
        if (should_launch) {
          spaceship_launch(civ);
        }
        else {
          civ4 = 0;
          for (civ2 = 1; civ2 < 8; civ2++) {
            if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
               (result = civ_spaceship_launched(civ2), result != 0)) {
              if (civ4 == 0) {
                civ4 = civ2;
              }
              else if (civs[civ2].spaceship_arrival_year < civs[civ4].spaceship_arrival_year)
              {
                civ4 = civ2;
              }
            }
          }
          if ((civ4 == 0) && (DIFF_KING < game.difficulty)) {
            for (civ3 = 1; civ3 < 8; civ3++) {
              if ((((civ3 != civ) &&
                   ((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) == 0)) &&
                  (result = civ_spaceship_launched(civ3), result != 0)) &&
                 (civs[civ3].spaceship_arrival_year < civs[0].spaceship_arrival_year)) {
                return part;
              }
            }
          }
          if (civ4 == 0) {
            should_launch = 0x4b < spaceship_stats.success_pct;
            if (!should_launch) {
              for (civ2 = 1; civ2 < 8; civ2++) {
                if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
                  if (game.power_rank[civ] < game.power_rank[civ2]) {
                    should_launch = true;
                    break;
                  }
                  result = civ_spaceship_exists(civ2);
                  if ((result != 0) && (999 < civs[civ2].gold)) {
                    should_launch = true;
                    break;
                  }
                }
              }
            }
            if (should_launch) {
              spaceship_launch(civ);
            }
          }
          else if (civs[civ].spaceship_arrival_year < civs[civ4].spaceship_arrival_year) {
            spaceship_launch(civ);
          }
        }
      }
    }
  }
  return part;
}

/* 00598b4e auto */
void __cdecl spaceship_select_view_dialog(iptr unused)

{
  bool any_ship;
  int civ;
  iptr name2;
  int shown;
  i32 flags;
  int civ2;
  Dialog dlg;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  dlg.load_game_section((const char *)&str_spaceships,1);
  any_ship = false;
  for (civ2 = 1; civ2 < 8; civ2++) {
    civ = civ_spaceship_exists(civ2);
    if (civ != 0) {
      flags = 0;
      civ = civ2;
      name2 = (iptr)(civ_name_plural(civ2));
      dlg.add_option((const char *)name2,civ,flags);
      any_ship = true;
    }
  }
  if (any_ship) {
    civ = dlg.show_modal(0);
    if (civ >= 0) {
      spaceship_wnd_load_parts(civ);
      shown = spaceship_wnd_run(civ);
      if (shown == 0) {
        (*((char *)&civ2 + 0)) = (byte)civ;
        spaceship_status_dialog(civ,(uint)game.human_players & 1 << ((byte)civ2 & 0x1f));
      }
    }
  }
  else {
    popup_game((const char *)&str_nospaceships);
  }
  return;
}

/* 00598ceb auto */
i32 __cdecl spaceship_race_active(void)

{
  i32 active;
  
  if ((game.rules & RULE_BLOODLUST) == 0) {
    if ((game.wonder_city[WONDER_APOLLO] == -1) && ((game.human_players & game.advance_known_by[ADV_SPACE_FLIGHT]) == 0))
    {
      active = 0;
    }
    else {
      active = 1;
    }
  }
  else {
    active = 0;
  }
  return active;
}

/* 005998b0 auto */
uint __cdecl spaceship_group_full(iptr civ,iptr group)

{
  int max_count;
  int count;
  
  max_count = spaceship_group_max(group);
  count = spaceship_group_count(civ,group);
  return (uint)(max_count <= count);
}

/* 00599910 auto */
i32 __cdecl spaceship_group_can_build(iptr civ,iptr group)

{
  int result;
  
  result = spaceship_group_at_limit(civ,group);
  if ((((result == 0) || (result = spaceship_group_count(civ,group), result == 0)) &&
      (result = spaceship_group_full(civ,group), result == 0)) &&
     (result = civ_has_advance(civ,(int)improvements[group + 0x23].preq), result != 0)) {
    return 1;
  }
  return 0;
}

/* 005999c0 auto */
uint __cdecl spaceship_group_at_limit(iptr civ,iptr group)

{
  int count;
  int limit;
  
  count = spaceship_group_count(civ,group);
  limit = spaceship_group_limit(civ,group);
  return (uint)(limit <= count);
}
