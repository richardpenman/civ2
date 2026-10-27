/* The game, the space race: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004d0160 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl spaceship_wnd_net_idle(void)

{
  int now;
  
  net_process_messages(&net_session_block,1,0);
  if ((net_session_block.net_host_handoff_stage == '\0') && (now = timer_ticks(), now - editor_text_edit_block.spaceship_wnd_open_ticks_i32 < 0x4b1)) {
    return;
  }
  wnd_end_modal_loop((ModalMenuWindow *)spaceship_launch_sprite_xs_block.spaceship_wnd);
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
  i8 *eh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  if (spaceship_launch_sprite_xs_block.spaceship_wnd == 0) {
    if (council_active == '\0') {
      if (civ_arg < 1) {
        civ = ~civ_arg + 1;
      }
      else {
        civ = civ_arg;
      }
      ret_addr = 0x4d0293;
      spaceship_wnd_ctor(window,civ);
      eh_state = 0;
      init_ok = spaceship_wnd_init(window);
      if (init_ok == 0) {
        eh_state = 0xffffffff;
        /* inlined unwind funclet 004d0314 */ { spaceship_wnd_dtor((SpaceshipWindow *)&window); }
        result = 0;
      }
      else {
        if (civ_arg < 0) {
          spaceship_wnd_launch_sequence(window);
        }
        else {
          spaceship_wnd_run_modal(window);
        }
        eh_state = 0xffffffff;
        /* inlined unwind funclet 004d0314 */ { spaceship_wnd_dtor((SpaceshipWindow *)&window); }
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
  i8 *eh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  if (human_civ[0] == civ) {
    if (spaceship_launch_sprite_xs_block.spaceship_wnd == 0) {
      if (council_active == '\0') {
        ret_addr = 0x4d03b3;
        spaceship_wnd_ctor(window,civ);
        eh_state = 0;
        init_ok = spaceship_wnd_init(window);
        if (init_ok == 0) {
          eh_state = 0xffffffff;
          /* inlined unwind funclet 004d041a */ { spaceship_wnd_dtor((SpaceshipWindow *)&window); }
          result = 0;
        }
        else {
          spaceship_wnd_present(window);
          eh_state = 0xffffffff;
          /* inlined unwind funclet 004d041a */ { spaceship_wnd_dtor((SpaceshipWindow *)&window); }
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
  map_views_block.text_buffer_char_1 = 0;
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
  surface_draw_text_in_rect
            ((DibSurface *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xb8),(iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0x8c4,(const char *)&map_views_block.text_buffer,&rect,
             0);
  win_flush_pending_msgs();
  return;
}

/* 004d0517 auto */
SpaceshipWindow * __thiscall spaceship_wnd_ctor(SpaceshipWindow *self,u32 civ)

{
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  wnd_ctor(self);
  eh_state = 0;
  surfwnd_ctor((SurfaceWindow *)&self->control_list);
  (*((char *)&eh_state + 0)) = 1;
  dll_handle_init((DllHandle *)self->ship_dll);
  (*((char *)&eh_state + 0)) = 2;
  palette_init((GamePalette *)self->ship_palette);
  (*((char *)&eh_state + 0)) = 3;
  font_init_empty((GameFont *)self->large_font);
  (*((char *)&eh_state + 0)) = 4;
  font_init_empty((GameFont *)self->medium_font);
  (*((char *)&eh_state + 0)) = 5;
  font_init_empty((GameFont *)self->typing_font);
  (*((char *)&eh_state + 0)) = 6;
  surface_ctor((DibSurface *)self->ship_view_surface);
  (*((char *)&eh_state + 0)) = 7;
  sprite_init((Sprite *)self->ship_image_sprite);
  (*((char *)&eh_state + 0)) = 8;
  sprite_init((Sprite *)self->close_btn_disabled_sprite);
  (*((char *)&eh_state + 0)) = 9;
  sprite_init((Sprite *)self->close_btn_normal_sprite);
  (*((char *)&eh_state + 0)) = 10;
  sprite_init((Sprite *)self->close_btn_pressed_sprite);
  (*((char *)&eh_state + 0)) = 0xb;
  sprite_init((Sprite *)self->launch_btn_disabled_sprite);
  (*((char *)&eh_state + 0)) = 0xc;
  sprite_init((Sprite *)self->launch_btn_normal_sprite);
  (*((char *)&eh_state + 0)) = 0xd;
  sprite_init((Sprite *)self->launch_btn_pressed_sprite);
  (*((char *)&eh_state + 0)) = 0xe;
  clickmap_ctor(self->hotspot_map);
  eh_state = (eh_state & 0xffffff00 | (byte)(0xf));
  self->ship_civ = civ;
  self->anim_timer = 0;
  gfx_get_screen_rect((iptr)&self->frame_left);
  self->type_index = 0;
  spaceship_launch_sprite_xs_block.spaceship_wnd = self;
  font_set_styled((GameFont *)self->large_font,0,0x18,1);
  font_set_styled((GameFont *)self->medium_font,0,0x14,1);
  font_set_styled((GameFont *)self->typing_font,0,0x10,1);
  self->spaceship_state = 0;
  self->launched = 0;
  surface_create((DibSurface *)self->ship_view_surface,0x1c9,0x130);
  surface_fill((DibSurface *)self->ship_view_surface,9);
  self->flight_timer = 0;
  self->anim_timer = 0;
  self->blink_timer = 0;
  SetRect((LPRECT)self->ship_panel_rect,10,0x14,300,0x1df);
  self->launch_video_player = 0;
  self->launch_state = 0;
  self->close_btn_pressed = 0;
  self->launch_btn_pressed = 0;
  self->typing_label_part = 0;
  return self;
}

/* 004d08b0 auto */
i32 __thiscall spaceship_wnd_dtor(SpaceshipWindow *self)

{
  i32 ret_value;
  i8 *seh_handler;
  int eh_state;
  
  eh_state = 0xf;
  dll_handle_free((DllHandle *)self->ship_dll);
  spaceship_launch_sprite_xs_block.spaceship_wnd = 0;
  if (self->flight_timer != 0) {
    game_timer_destroy(self->flight_timer);
    self->flight_timer = 0;
  }
  if (self->anim_timer != 0) {
    game_timer_destroy(self->anim_timer);
    self->anim_timer = 0;
  }
  if (self->blink_timer != 0) {
    game_timer_destroy(self->blink_timer);
    self->blink_timer = 0;
  }
  (*((char *)&eh_state + 0)) = 0xe;
  /* inlined unwind funclet 004d0a56 */ { clickmap_dtor((u8 *)self + 0xae4); }
  (*((char *)&eh_state + 0)) = 0xd;
  /* inlined unwind funclet 004d0a65 */ { sprite_destroy((Sprite *)(*(int *)&self + 0xaa4)); }
  (*((char *)&eh_state + 0)) = 0xc;
  /* inlined unwind funclet 004d0a74 */ { sprite_destroy((Sprite *)(*(int *)&self + 0xa68)); }
  (*((char *)&eh_state + 0)) = 0xb;
  /* inlined unwind funclet 004d0a83 */ { sprite_destroy((Sprite *)(*(int *)&self + 0xa2c)); }
  (*((char *)&eh_state + 0)) = 10;
  /* inlined unwind funclet 004d0a92 */ { sprite_destroy((Sprite *)(*(int *)&self + 0x9f0)); }
  (*((char *)&eh_state + 0)) = 9;
  /* inlined unwind funclet 004d0aa1 */ { sprite_destroy((Sprite *)(*(int *)&self + 0x9b4)); }
  (*((char *)&eh_state + 0)) = 8;
  /* inlined unwind funclet 004d0ab0 */ { sprite_destroy((Sprite *)(*(int *)&self + 0x978)); }
  (*((char *)&eh_state + 0)) = 7;
  /* inlined unwind funclet 004d0abf */ { sprite_destroy((Sprite *)(*(int *)&self + 0x93c)); }
  (*((char *)&eh_state + 0)) = 6;
  /* inlined unwind funclet 004d0ace */ { surface_dtor((DibSurface *)(*(int *)&self + 0x8f4)); }
  (*((char *)&eh_state + 0)) = 5;
  /* inlined unwind funclet 004d0add */ { font_destroy((GameFont *)(*(int *)&self + 0x8d4)); }
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 004d0aec */ { font_destroy((GameFont *)(*(int *)&self + 0x8cc)); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 004d0afb */ { font_destroy((GameFont *)(*(int *)&self + 0x8c4)); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 004d0b0a */ { palette_destroy((GamePalette *)(*(int *)&self + 0x1f8)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 004d0b19 */ { dll_free((DllHandle *)(*(int *)&self + 500)); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 004d0b28 */ { surfwnd_dtor((SurfaceWindow *)(*(int *)&self + 0xb8)); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004d0b37 */ { wnd_dtor(*(WndBase **)&self); }
  return ret_value;
}

/* 004d0b58 auto */
i32 __thiscall spaceship_wnd_init(SpaceshipWindow *self)

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
    dll_handle_load((DllHandle *)self->ship_dll,(const char *)dll_path);
    wnd_set_cursor(self,0x1fc);
    spaceship_calc_stats(self->ship_civ,1);
    spaceship_wnd_build_stats_text(self);
    if ((civs[self->ship_civ].spaceship_parts[3] != 0) && (unit_sound_editor_window_block.spaceship_success_pct != 0)) {
      self->launch_state = 1;
    }
    status = civ_spaceship_launched(self->ship_civ);
    if (status != 0) {
      self->launch_state = 3;
    }
    status = spaceship_wnd_create(self);
    if (status == 0) {
      result = 0;
    }
    else {
      spaceship_wnd_make_button_sprites(self);
      if (self->flight_timer != 0) {
        game_timer_destroy(self->flight_timer);
        self->flight_timer = 0;
      }
      if (self->anim_timer != 0) {
        game_timer_destroy(self->anim_timer);
        self->anim_timer = 0;
      }
      if (self->blink_timer != 0) {
        game_timer_destroy(self->blink_timer);
        self->blink_timer = 0;
      }
      self->type_x = 10;
      self->type_y = 0x28;
      spaceship_wnd_setup_hotspots(self);
      result = 1;
    }
  }
  return result;
}

/* 004d0d64 auto */
uint __thiscall spaceship_wnd_create(SpaceshipWindow *self)

{
  int loaded;
  
  win_flush_pending_msgs();
  wnd_create_with_handlers
            (self,(const char *)&str_empty_125,0x800,self->frame_left,self->frame_top,
             self->frame_right - self->frame_left,(self->frame_bottom - self->frame_top) + 5,(iptr)&game_palette_block.game_palette,
             (iptr)&main_frame_wnd_block.main_frame_wnd);
  wnd_set_bg_color(self,0);
  wnd_create_with_surface
            ((DibSurface *)&self->control_list,(const char *)&str_empty_126,0x800,0,0,0x280,0x1e0,
             (iptr)(self->ship_palette),(iptr)self);
  loaded = surface_load_gif_resource
                    ((DibSurface *)&self->control_list,499,10,0xec,(iptr)(self->ship_palette));
  if (loaded != 0) {
    wnd_center((WndBase *)self->frame_wnd_base);
    wnd_set_keydown_handler((WndHandlers *)self->frame_handlers,spaceship_wnd_on_key);
    wnd_set_mousemove_handler((WndHandlers *)self->frame_handlers,spaceship_wnd_on_mousemove);
    wnd_set_lbuttondown_handler((WndHandlers *)self->frame_handlers,spaceship_wnd_on_lbuttondown);
    wnd_set_lbuttonup_any_handler((WndHandlers *)self->frame_handlers,spaceship_wnd_on_lbuttonup);
  }
  return (uint)(loaded != 0);
}

/* 004d0ea6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall spaceship_wnd_run_modal(SpaceshipWindow *self)

{
  int rendered;
  u32 timer;
  i8 clip_rect[16];
  Sprite launch_sprite;
  DibSurface launch_surface;
  i32 seh_prev;
  i8 *seh_handler;
  int eh_state;
  
  eh_state = 0xffffffff;
  surface_ctor(&launch_surface);
  eh_state = 0;
  sprite_init(&launch_sprite);
  (*((char *)&eh_state + 0)) = 1;
  spaceship_wnd_load_parts(self->ship_civ);
  rendered = spaceship_wnd_render_ship(self);
  if (rendered == 0) {
    eh_state = eh_state & 0xffffff00;
    /* inlined unwind funclet 004d138b */ { sprite_destroy(&launch_sprite); }
    eh_state = 0xffffffff;
    /* inlined unwind funclet 004d1397 */ { surface_dtor(&launch_surface); }
  }
  else {
    if (self->launch_state == 3) {
      surface_load_gif_resource
                (&launch_surface,editor_text_edit_block.spaceship_wnd_component_count + 19999,10,0xec,(iptr)(self->ship_palette));
      sprite_grab_from_surface
                (&launch_sprite,(iptr)&launch_surface,0xff,
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_xs) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_ys) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_widths) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_part_sprite_heights) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10));
      sprite_draw
                (&launch_sprite,(iptr)clip_rect,(iptr)&self->control_list,
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_xs) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10),
                 *(i32 *)(((i8 *)&spaceship_launch_sprite_xs_block.spaceship_launch_sprite_ys) + (editor_text_edit_block.spaceship_wnd_component_count - 1) * 0x10));
    }
    switch(self->launch_state) {
    case 0:
    case 1:
      spaceship_wnd_draw_launch_btn(self,0,1);
      break;
    case 2:
      break;
    case 3:
      spaceship_wnd_init_starfield(self);
      spaceship_wnd_draw_title(self->ship_civ);
    }
    spaceship_wnd_draw_close_btn(self,1,1);
    if (self->anim_timer != 0) {
      game_timer_destroy(self->anim_timer);
    }
    if (self->launch_state == 3) {
      timer = game_timer_create((TimerFn)spaceship_wnd_starfield_tick,5,0xffffffff);
      self->flight_timer = timer;
    }
    else {
      timer = game_timer_create((TimerFn)spaceship_wnd_type_tick,0x3c,0xffffffff);
      self->anim_timer = timer;
    }
    wnd_handle_show_raise(self);
    wnd_flush_paint();
    wnd_set_palette(self,(iptr)(self->ship_palette));
    window_show((SurfaceWindow *)&self->control_list);
    wnd_flush_paint();
    wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)(self->ship_palette));
    spaceship_wnd_setup_hotspots(self);
    spaceship_wnd_draw_buttons(self);
    if (self->launch_state == 3) {
      self->launched = 1;
    }
    else {
      self->launched = 0;
    }
    self->typing_label_part = 1;
    wnd_set_capture((WndBase *)self->frame_wnd_base);
    input_flush_events();
    if (2 < game.multiplayer) {
      wnd_set_modal_idle_callback((ModalMenuWindow *)self->frame_wnd_base,spaceship_wnd_net_idle);
    }
    editor_text_edit_block.spaceship_wnd_open_ticks_i32 = timer_ticks();
    wnd_run_modal_loop((ModalMenuWindow *)self->frame_wnd_base);
    wnd_set_modal_idle_callback((ModalMenuWindow *)self->frame_wnd_base,0);
    wnd_release_capture((WndBase *)self->frame_wnd_base);
    if (self->flight_timer != 0) {
      game_timer_destroy(self->flight_timer);
      self->flight_timer = 0;
    }
    if (self->anim_timer != 0) {
      game_timer_destroy(self->anim_timer);
      self->anim_timer = 0;
    }
    if (self->blink_timer != 0) {
      game_timer_destroy(self->blink_timer);
      self->blink_timer = 0;
    }
    if (self->launch_state == 2) {
      spaceship_play_launch_video(self);
      self->launch_state = 3;
      spaceship_launch(self->ship_civ);
      spaceship_wnd_run_modal(self);
    }
    else {
      wnd_hide((WndBase *)self->frame_wnd_base);
      wnd_flush_paint();
    }
    wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)&game_palette_block.game_palette);
    wnd_set_palette(self,(iptr)&game_palette_block.game_palette);
    wnd_hide(self);
    wnd_flush_paint();
    eh_state = eh_state & 0xffffff00;
    /* inlined unwind funclet 004d138b */ { sprite_destroy(&launch_sprite); }
    eh_state = 0xffffffff;
    /* inlined unwind funclet 004d1397 */ { surface_dtor(&launch_surface); }
  }
  return;
}

/* 004d13b8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall spaceship_wnd_present(SpaceshipWindow *self)

{
  int rendered;
  u32 value;
  tagRECT screen_rect2;
  
  if (self->ship_civ == human_civ[0]) {
    if (self->anim_timer != 0) {
      game_timer_destroy(self->anim_timer);
    }
    if ((self->launch_state != 3) && (rendered = spaceship_wnd_render_ship(self), rendered != 0)) {
      spaceship_wnd_draw_close_btn(self,1,1);
      spaceship_wnd_draw_launch_btn(self,0,1);
      self->typing_label_part = 1;
      wnd_handle_show_raise(self);
      wnd_flush_paint();
      wnd_set_palette(self,(iptr)(self->ship_palette));
      window_show((SurfaceWindow *)&self->control_list);
      wnd_flush_paint();
      wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)(self->ship_palette))
      ;
      spaceship_wnd_setup_hotspots(self);
      spaceship_wnd_draw_buttons(self);
      self->launched = 1;
      self->typing_label_part = 1;
      value = self->launch_state;
      self->launch_state = 0;
      self->launch_state = value;
      spaceship_wnd_load_parts(self->ship_civ);
      rendered = spaceship_wnd_render_ship(self);
      if (rendered != 0) {
        spaceship_wnd_draw_close_btn(self,1,0);
        spaceship_wnd_draw_launch_btn(self,0,1);
        SetRect(&screen_rect2,0,0,0x280,0x1e0);
        gfx_dissolve_blit((iptr)&self->control_list,(iptr)&screen_rect2,5,1);
        spaceship_wnd_draw_close_btn(self,1,1);
        value = game_timer_create((TimerFn)spaceship_wnd_type_tick,10,0xffffffff);
        self->anim_timer = value;
        spaceship_wnd_setup_hotspots(self);
        spaceship_wnd_draw_buttons(self);
        self->launched = 0;
        self->typing_label_part = 1;
        wnd_set_capture((WndBase *)self->frame_wnd_base);
        input_flush_events();
        if (2 < game.multiplayer) {
          wnd_set_modal_idle_callback((ModalMenuWindow *)self->frame_wnd_base,spaceship_wnd_net_idle);
        }
        editor_text_edit_block.spaceship_wnd_open_ticks_i32 = timer_ticks();
        wnd_run_modal_loop((ModalMenuWindow *)self->frame_wnd_base);
        wnd_set_modal_idle_callback((ModalMenuWindow *)self->frame_wnd_base,0);
        wnd_release_capture((WndBase *)self->frame_wnd_base);
        if (self->flight_timer != 0) {
          game_timer_destroy(self->flight_timer);
          self->flight_timer = 0;
        }
        if (self->anim_timer != 0) {
          game_timer_destroy(self->anim_timer);
          self->anim_timer = 0;
        }
        if (self->blink_timer != 0) {
          game_timer_destroy(self->blink_timer);
          self->blink_timer = 0;
        }
        wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)&game_palette_block.game_palette);
        if (self->launch_state == 2) {
          spaceship_play_launch_video(self);
          self->launch_state = 3;
          spaceship_launch(self->ship_civ);
          spaceship_wnd_run_modal(self);
        }
        else {
          wnd_hide((WndBase *)self->frame_wnd_base);
          wnd_flush_paint();
        }
        wnd_set_palette(self,(iptr)&game_palette_block.game_palette);
        wnd_hide(self);
        wnd_flush_paint();
      }
    }
  }
  return;
}

/* 004d1725 auto */
void __thiscall spaceship_wnd_launch_sequence(SpaceshipWindow *self)

{
  wnd_handle_show_raise(self);
  wnd_flush_paint();
  wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)(self->ship_palette));
  wnd_set_palette(self,(iptr)(self->ship_palette));
  self->launch_state = 2;
  spaceship_play_launch_video(self);
  self->launch_state = 3;
  spaceship_wnd_run_modal(self);
  wnd_set_palette(&main_frame_wnd_block.main_frame_wnd,(iptr)&game_palette_block.game_palette);
  wnd_set_palette(self,(iptr)&game_palette_block.game_palette);
  wnd_hide(self);
  wnd_flush_paint();
  return;
}

/* 004d17bf: written by hand in src/game/bigframes.cpp */

/* 004d4a7b auto */
void __thiscall spaceship_wnd_build_stats_text(SpaceshipWindow *self)

{
  short part_count;
  bool over_limit;
  int part_limit;
  int part_index;
  
  over_limit = false;
  str_clear((char *)self->stats_text);
  for (part_index = 0; part_index < 6; part_index++) {
    part_count = civs[(self->ship_civ)].spaceship_parts[part_index];
    part_limit = spaceship_part_limit(self->ship_civ,part_index);
    if (part_limit < part_count) {
      over_limit = true;
    }
  }
  for (part_index = 0; part_index < 6; part_index++) {
    str_append_string_id((iptr)self->stats_text,
                       label_string_ids[spaceship_part_table_block.spaceship_parts[part_index].label_id])
    ;
    str_append_colon((char *)self->stats_text);
    str_append_int((iptr)self->stats_text,
                       (int)civs[(self->ship_civ)].spaceship_parts[part_index]);
    if ((over_limit) && (part_index == 0)) {
      str_append((char *)self->stats_text,(const char *)&str_empty_127);
    }
    str_append((char *)self->stats_text,(const char *)&str_empty_128);
  }
  str_append_label((iptr)self->stats_text,0x22);
  str_append_colon((char *)self->stats_text);
  str_append_int((iptr)self->stats_text,(int)civs[self->ship_civ].spaceship_parts[3]);
  str_append((char *)self->stats_text,(const char *)&str_0_000_6);
  str_append((char *)self->stats_text,(const char *)&str_empty_129);
  str_append_label((iptr)self->stats_text,0x42);
  str_append_colon((char *)self->stats_text);
  str_append_int((iptr)self->stats_text,unit_sound_editor_window_block.spaceship_support_pct);
  str_append_percent((iptr)self->stats_text);
  str_append((char *)self->stats_text,(const char *)&str_empty_130);
  str_append_label((iptr)self->stats_text,0xcd);
  str_append_colon((char *)self->stats_text);
  str_append_int((iptr)self->stats_text,unit_sound_editor_window_block.spaceship_energy_pct);
  str_append_percent((iptr)self->stats_text);
  str_append((char *)self->stats_text,(const char *)&str_empty_131);
  str_append_label((iptr)self->stats_text,0xce);
  str_append_colon((char *)self->stats_text);
  str_append_int((iptr)self->stats_text,unit_sound_editor_window_block.spaceship_mass / 10);
  str_append((char *)self->stats_text,(const char *)&str_empty_132);
  str_append_int((iptr)self->stats_text,unit_sound_editor_window_block.spaceship_mass % 10);
  str_append((char *)self->stats_text,(const char *)&str_00);
  str_append_label((iptr)self->stats_text,0xd1);
  str_append((char *)self->stats_text,(const char *)&str_empty_133);
  str_append_label((iptr)self->stats_text,200);
  str_append_colon((char *)self->stats_text);
  str_append_int((iptr)self->stats_text,unit_sound_editor_window_block.spaceship_fuel_pct);
  str_append_percent((iptr)self->stats_text);
  if ((civs[self->ship_civ].spaceship_flags & 8) != 0) {
    str_append_space((iptr)self->stats_text);
    str_append_lparen((iptr)self->stats_text);
    str_append_label((iptr)self->stats_text,0xeb);
    str_append_rparen((iptr)self->stats_text);
  }
  str_append((char *)self->stats_text,(const char *)&str_empty_134);
  str_append_label((iptr)self->stats_text,0xcf);
  str_append_colon((char *)self->stats_text);
  str_append_int((iptr)self->stats_text,unit_sound_editor_window_block.spaceship_flight_time / 10);
  str_append((char *)self->stats_text,(const char *)&str_empty_135);
  str_append_int((iptr)self->stats_text,unit_sound_editor_window_block.spaceship_flight_time % 10);
  str_append_space((iptr)self->stats_text);
  str_append_label((iptr)self->stats_text,0xd2);
  str_clear((char *)self->success_text);
  str_append_label((iptr)(self->success_text),0xd0);
  str_append_colon((char *)self->success_text);
  str_append_space((iptr)(self->success_text));
  str_append_int((iptr)(self->success_text),unit_sound_editor_window_block.spaceship_success_pct);
  str_append_percent((iptr)(self->success_text));
  win_flush_pending_msgs();
  return;
}

/* 004d4fd1 auto */
uint __thiscall spaceship_wnd_type_char(SpaceshipWindow *self,int allow_skip)

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
    font_handle = font_get_handle((GameFont *)self->typing_font);
    metric = font_measure_height(font_handle);
    win_flush_pending_msgs();
    char_buf[0] = self->stats_text[self->type_index];
    char_buf[1] = 0;
    gfx_set_text_color_index(0xf9);
    if (self->typing_label_part == 0) {
      gfx_set_text_color_index(0xfe);
    }
    else {
      gfx_set_text_color_index(0xf9);
    }
    surface_draw_text
              ((DibSurface *)&self->control_list,(iptr)(self->typing_font),(const char *)char_buf,
               self->type_x,self->type_y,1);
    yBottom = self->type_y + metric * 2;
    char_ptr = char_buf;
    font_handle = font_get_handle((GameFont *)self->typing_font);
    char_width = font_measure_width(font_handle,(const char *)char_ptr);
    SetRect(&dirty_rect,self->type_x,self->type_y - metric,self->type_x + char_width,yBottom);
    char_ptr = char_buf;
    font_handle = font_get_handle((GameFont *)self->typing_font);
    metric = font_measure_width(font_handle,(const char *)char_ptr);
    self->type_x += metric;
    if (self->stats_text[self->type_index] == ':') {
      self->typing_label_part = 0;
    }
    self->type_index++;
    while (self->stats_text[self->type_index] == '\n') {
      self->typing_label_part = 1;
      self->type_x = 10;
      font_handle = font_get_handle((GameFont *)self->typing_font);
      metric = font_measure_height(font_handle);
      self->type_y += metric;
      self->type_index++;
    }
    if (self->stats_text[self->type_index] == '\0') {
      if (self->anim_timer != 0) {
        game_timer_destroy(self->anim_timer);
        self->anim_timer = 0;
      }
      if (self->launch_state != 3) {
        SetRect((LPRECT)(self->status_line_rect),0,0x1c1,0x27f,0x1df);
        rect_offset((iptr)(self->status_line_rect),1,1);
        gfx_set_text_color_index(10);
        surface_draw_text_in_rect
                  ((DibSurface *)&self->control_list,(iptr)(self->medium_font),
                   (const char *)self->success_text,(RECT *)(iptr)(self->status_line_rect),0);
        rect_offset((iptr)(self->status_line_rect),0xffffffff,0xffffffff);
        if (unit_sound_editor_window_block.spaceship_success_pct < 0x28) {
          gfx_set_text_color_index(0xf9);
        }
        else if (unit_sound_editor_window_block.spaceship_success_pct < 0x4b) {
          gfx_set_text_color_index(0xfb);
        }
        else {
          gfx_set_text_color_index(0xfa);
        }
        surface_draw_text_in_rect
                  ((DibSurface *)&self->control_list,(iptr)(self->medium_font),
                   (const char *)self->success_text,(RECT *)(iptr)(self->status_line_rect),0);
        wnd_blit_surface_rect
                  ((DibSurface *)&self->control_list,(iptr)(self->status_line_rect));
        wnd_flush_paint();
      }
      if (self->launch_state == 1) {
        timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
        self->blink_timer = timer;
      }
      self->launched = 1;
    }
    wnd_blit_surface_rect((DibSurface *)&self->control_list,(iptr)&dirty_rect);
    result = (uint)(self->stats_text[self->type_index] != '\0');
  }
  else {
    result = 1;
  }
  return result;
}

/* 004d53ab: written by hand in src/game/unwind.cpp */

/* 004d570b auto */
void __thiscall spaceship_wnd_init_starfield(SpaceshipWindow *self)

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
    self->stars[star].x = rand_val;
    rand_val = rand_range(0xffffa240,24000);
    self->stars[star].y = rand_val;
    rand_val = rand_range(1,100);
    self->stars[star].depth = rand_val;
  }
  value = surface_get_height((DibSurface *)&self->control_list);
  xRight = surface_get_width((DibSurface *)&self->control_list);
  SetRect(&fill_rect,0,0x14,xRight,value);
  surface_fill_rect((DibSurface *)&self->control_list,(iptr)&fill_rect,10);
  for (star = 0; star < 0x50; star++) {
    star_x = self->stars[star].x /
               self->stars[star].depth + 0x140;
    star_y = self->stars[star].y /
               self->stars[star].depth + 0xf0;
    if ((((0x280 < star_x) || (star_x < 0)) || (0x1e0 < star_y)) || (star_y < 0)) {
      rand_val = rand_range(0xffff8300,32000);
      self->stars[star].x = rand_val;
      rand_val = rand_range(0xffffa240,24000);
      self->stars[star].y = rand_val;
      self->stars[star].depth = 100;
      star_x = self->stars[star].x /
                 self->stars[star].depth + 0x140;
      star_y = self->stars[star].y /
                 self->stars[star].depth + 0xf0;
    }
    surface_set_pixel((DibSurface *)&self->control_list,star_x,star_y,0x29);
  }
  self->type_index = 0;
  self->type_x = 10;
  self->type_y = 0x73;
  self->typing_label_part = 1;
  do {
    value = spaceship_wnd_type_char(self,0);
  } while (value != 0);
  str_clear((char *)self->success_text);
  label = (iptr)label_string(label_string_ids[0x1c6]);
  str_append((char *)self->success_text,(const char *)label);
  str_append_colon((char *)self->success_text);
  str_append_space((iptr)(self->success_text));
  str_append_year((iptr)(self->success_text),(int)civs[self->ship_civ].spaceship_arrival_year
                    );
  SetRect((LPRECT)(self->status_line_rect),0,0x1c1,0x27f,0x1df);
  rect_offset((iptr)(self->status_line_rect),1,1);
  gfx_set_text_color_index(10);
  surface_draw_text_in_rect
            ((DibSurface *)&self->control_list,(iptr)(self->medium_font),
             (const char *)self->success_text,(RECT *)(iptr)(self->status_line_rect),0);
  rect_offset((iptr)(self->status_line_rect),0xffffffff,0xffffffff);
  gfx_set_text_color_index(0xfa);
  surface_draw_text_in_rect
            ((DibSurface *)&self->control_list,(iptr)(self->medium_font),
             (const char *)self->success_text,(RECT *)(iptr)(self->status_line_rect),0);
  wnd_blit_surface_rect((DibSurface *)&self->control_list,(iptr)(self->status_line_rect));
  sprite_draw
            ((Sprite *)self->ship_image_sprite,(iptr)clip_rect,(iptr)&self->control_list,0xaf,99);
  if (self->launch_state == 3) {
    self->type_y = 0x8c;
  }
  return;
}

/* 004d5b21 auto */
void __thiscall spaceship_play_launch_video(SpaceshipWindow *self)

{
  AviPlayer *this_00;
  int found;
  AviPlayer *player;
  i8 video_path[128];
  int open_result;
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  snd_cd_stop();
  this_00 = (AviPlayer *)operator_new(0xa28);
  eh_state = 0;
  if (this_00 == (AviPlayer *)0x0) {
    player = 0;
  }
  else {
    player = mmwnd_ctor(this_00);
  }
  eh_state = 0xffffffff;
  self->launch_video_player = (u32)player;
  str_clear((char *)video_path);
  str_append((char *)video_path,(const char *)&str_civ2_video_launch_avi);
  found = file_resolve_path((const char *)video_path);
  if (found != 0) {
    avi_player_create((AviPlayer *)self->launch_video_player,(iptr)&str_empty_136,0x800,0,0);
    avi_player_set_double_size((AviPlayer *)self->launch_video_player,1);
    open_result = avi_player_open((AviPlayer *)self->launch_video_player,(const char *)video_path);
    if (open_result != 0) {
      if (open_result == -0x7ffbfeac) {
        popup_game((const char *)&str_vfwnotregistered_6);
      }
      goto done;
    }
    wnd_hide((WndBase *)((u8 *)self + 0x100));
    wnd_flush_paint();
    surface_fill((DibSurface *)self->launch_video_player,0);
    wnd_set_lbuttonup_handler((WndHandlers *)(self->launch_video_player + 0x58),spaceship_video_on_lbuttonup);
    wnd_set_keydown_handler((WndHandlers *)(self->launch_video_player + 0x58),spaceship_wnd_on_key);
    avi_player_set_end_callback((AviPlayer *)self->launch_video_player,spaceship_video_on_end);
    wnd_center((WndBase *)(self->launch_video_player + 0x48));
    wnd_blit_surface((DibSurface *)self->launch_video_player);
    window_show((SurfaceWindow *)self->launch_video_player);
    wnd_flush_paint();
    avi_player_play((AviPlayer *)self->launch_video_player);
    input_flush_events();
    wnd_set_capture((WndBase *)(self->launch_video_player + 0x48));
    if (2 < game.multiplayer) {
      wnd_set_modal_idle_callback((ModalMenuWindow *)(self->launch_video_player + 0x48),net_modal_idle_poll);
    }
    wnd_run_modal_loop((ModalMenuWindow *)(self->launch_video_player + 0x48));
    wnd_set_modal_idle_callback((ModalMenuWindow *)(self->launch_video_player + 0x48),0);
    wnd_release_capture((WndBase *)(self->launch_video_player + 0x48));
    wnd_hide((WndBase *)(self->launch_video_player + 0x48));
    wnd_flush_paint();
  }
  if ((AviPlayer *)self->launch_video_player != (AviPlayer *)0x0) {
    avi_player_scalar_dtor((AviPlayer *)self->launch_video_player,1);
  }
  self->launch_video_player = 0;
  snd_sync_music_option();
done:
  return;
}

/* 004d5e41 auto */
void __thiscall spaceship_wnd_setup_hotspots(SpaceshipWindow *self)

{
  clickmap_clear((ClickMapClearView *)self->hotspot_map);
  if (((self->ship_civ == human_civ[0]) && (self->launch_state != 3)) && (self->launch_state != 0)) {
    clickmap_add_xywh((ClickMap *)self->hotspot_map,0,0x1f5,0x1a1,0x34,0x24);
    clickmap_add_xywh((ClickMap *)self->hotspot_map,1,0x22f,0x1a1,0x43,0x24);
  }
  else {
    clickmap_add_xywh((ClickMap *)self->hotspot_map,0,0x22f,0x1a1,0x43,0x24);
  }
  return;
}

/* 004d5ef9 auto */
void __thiscall spaceship_wnd_draw_buttons(SpaceshipWindow *self)

{
  tagRECT btn_rect;
  
  if ((self->launch_state != 1) && (self->launch_state != 0)) {
    SetRect(&btn_rect,0x1f5,0x1a1,0x229,0x1c5);
    surface_fill_rect((DibSurface *)&self->control_list,(iptr)&btn_rect,0);
  }
  spaceship_wnd_draw_close_btn(self,1,1);
  spaceship_wnd_draw_launch_btn(self,0,1);
  return;
}

/* 004d5f79 auto */
void __thiscall spaceship_wnd_draw_close_btn(SpaceshipWindow *self,int enabled,int raised)

{
  i8 pressed_clip[16];
  i8 normal_clip[16];
  i8 disabled_clip[16];
  tagRECT btn_rect;
  
  if (((self->ship_civ == human_civ[0]) && (self->launch_state != 3)) && (self->launch_state != 0)) {
    SetRect(&btn_rect,0x1f5,0x1a1,0x229,0x1c5);
  }
  else {
    SetRect(&btn_rect,0x22f,0x1a1,0x272,0x1c5);
  }
  if (raised == 0) {
    sprite_draw
              ((Sprite *)self->close_btn_disabled_sprite,(iptr)disabled_clip,(iptr)&self->control_list,
               btn_rect.left,btn_rect.top);
  }
  else if (enabled == 0) {
    sprite_draw
              ((Sprite *)self->close_btn_pressed_sprite,(iptr)pressed_clip,(iptr)&self->control_list,
               btn_rect.left,btn_rect.top);
  }
  else {
    sprite_draw
              ((Sprite *)self->close_btn_normal_sprite,(iptr)normal_clip,(iptr)&self->control_list,
               btn_rect.left,btn_rect.top);
  }
  wnd_blit_surface_rect((DibSurface *)&self->control_list,(iptr)&btn_rect);
  wnd_flush_paint();
  return;
}

/* 004d60a5 auto */
void __thiscall spaceship_wnd_draw_launch_btn(SpaceshipWindow *self,int enabled,int raised)

{
  i8 pressed_clip[16];
  i8 normal_clip[16];
  i8 disabled_clip[16];
  tagRECT btn_rect;
  
  if ((((self->ship_civ == human_civ[0]) && (self->launch_state != 3)) && (self->launch_state != 0)) &&
     (self->launch_state != 2)) {
    SetRect(&btn_rect,0x22f,0x1a1,0x272,0x1c5);
    if (raised == 0) {
      sprite_draw
                ((Sprite *)self->launch_btn_disabled_sprite,(iptr)disabled_clip,(iptr)&self->control_list,
                 btn_rect.left,btn_rect.top);
    }
    else if (enabled == 0) {
      sprite_draw
                ((Sprite *)self->launch_btn_pressed_sprite,(iptr)pressed_clip,(iptr)&self->control_list,
                 btn_rect.left,btn_rect.top);
    }
    else {
      sprite_draw
                ((Sprite *)self->launch_btn_normal_sprite,(iptr)normal_clip,(iptr)&self->control_list,
                 btn_rect.left,btn_rect.top);
    }
    wnd_blit_surface_rect((DibSurface *)&self->control_list,(iptr)&btn_rect);
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
    wnd_end_modal_loop((ModalMenuWindow *)((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player + 0x48));
  }
  else {
    hit = clickmap_hit_test
                      ((ClickMapHitTestView *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xae4),x,y,(iptr)&hotspot_id,0);
    if (hit != -1) {
      if (hotspot_id == 0) {
        spaceship_wnd_draw_close_btn(ship_wnd,1,0);
        if (ship_wnd->launched == 0) {
          if (ship_wnd->anim_timer != 0) {
            game_timer_destroy(ship_wnd->anim_timer);
            ship_wnd->anim_timer = 0;
          }
          if (ship_wnd->launch_state != 3) {
            do {
              hit = spaceship_wnd_type_char(ship_wnd,0);
            } while (hit != 0);
            ship_wnd->launched = 1;
            spaceship_wnd_draw_close_btn(ship_wnd,1,1);
          }
        }
        else {
          wnd_end_modal_loop((ModalMenuWindow *)(active_wnd + 0x48));
        }
      }
      else if (((hotspot_id == 1) && (ship_wnd->launch_state == 1)) && (ship_wnd->launched != 0)) {
        spaceship_wnd_draw_launch_btn(ship_wnd,1,0);
        ship_wnd->launch_state = 2;
        wnd_end_modal_loop((ModalMenuWindow *)(active_wnd + 0x48));
      }
    }
  }
  return;
}

/* 004d6367 auto */
void __cdecl spaceship_wnd_type_tick(void)

{
  spaceship_wnd_type_char(spaceship_launch_sprite_xs_block.spaceship_wnd,1);
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
    spaceship_wnd_draw_launch_btn(self,1,1);
  }
  else {
    spaceship_wnd_draw_launch_btn(self,0,1);
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
      pixel = surface_get_pixel((DibSurface *)((iptr)ship_wnd + 0xb8),old_x,old_y);
      if (pixel == 0x29) {
        surface_set_pixel((DibSurface *)((iptr)ship_wnd + 0xb8),old_x,old_y,10);
      }
    }
    if (((new_x < 0x280) && (0 < new_x)) && ((new_y < 0x1e0 && (0 < new_y)))) {
      old_x = surface_get_pixel((DibSurface *)((iptr)ship_wnd + 0xb8),new_x,new_y);
      if (old_x == 10) {
        surface_set_pixel((DibSurface *)((iptr)ship_wnd + 0xb8),new_x,new_y,0x29);
      }
    }
  }
  wnd_blit_surface((DibSurface *)((iptr)ship_wnd + 0xb8));
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
      wnd_end_modal_loop((ModalMenuWindow *)((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player + 0x48));
    }
    else {
      spaceship_wnd_draw_close_btn(spaceship_launch_sprite_xs_block.spaceship_wnd,1,0);
      if (self->launched == 0) {
        if (self->anim_timer != 0) {
          game_timer_destroy((i32)self->anim_timer);
          self->anim_timer = 0;
        }
        do {
          more = spaceship_wnd_type_char(self,0);
        } while (more != 0);
        self->launched = 1;
        spaceship_wnd_draw_close_btn(self,1,1);
      }
      else {
        wnd_end_modal_loop((ModalMenuWindow *)(active_wnd + 0x48));
      }
    }
  }
  return;
}

/* 004d686b auto */
void __cdecl spaceship_video_on_end(void)

{
  if (spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player != 0) {
    wnd_end_modal_loop((ModalMenuWindow *)((int)spaceship_launch_sprite_xs_block.spaceship_wnd->launch_video_player + 0x48));
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
     (hit = clickmap_hit_test
                        ((ClickMapHitTestView *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xae4),x,y,(iptr)&hotspot_id,0),
     hit != -1)) {
    if (hotspot_id == 0) {
      (ship_wnd)->close_btn_pressed = 1;
      spaceship_wnd_draw_close_btn(ship_wnd,1,0);
      if ((ship_wnd->launch_state == 1) && (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
      wnd_blit_surface((DibSurface *)&ship_wnd->control_list);
    }
    else if (((hotspot_id == 1) && (ship_wnd->launch_state == 1)) && (ship_wnd->launched != 0)) {
      (ship_wnd)->launch_btn_pressed = 1;
      spaceship_wnd_draw_launch_btn(ship_wnd,1,0);
      if ((ship_wnd->launch_state == 1) && (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
      wnd_blit_surface((DibSurface *)&ship_wnd->control_list);
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
    wnd_end_modal_loop((ModalMenuWindow *)((int)(ship_wnd)->launch_video_player + 0x48));
  }
  else if (((ship_wnd)->launch_btn_pressed != 0) ||
          ((ship_wnd)->close_btn_pressed != 0)) {
    if ((ship_wnd)->close_btn_pressed != 0) {
      spaceship_wnd_draw_close_btn(ship_wnd,1,1);
    }
    if ((ship_wnd)->launch_btn_pressed != 0) {
      spaceship_wnd_draw_launch_btn(ship_wnd,1,1);
    }
    hit = clickmap_hit_test
                      ((ClickMapHitTestView *)ship_wnd->hotspot_map,x,y,(iptr)&hotspot_id,0);
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
                hit = spaceship_wnd_type_char(ship_wnd,0);
              } while (hit != 0);
              ship_wnd->launched = 1;
            }
          }
          else {
            wnd_end_modal_loop((ModalMenuWindow *)(active_wnd + 0x48));
          }
        }
      }
      else if ((hotspot_id == 1) && ((ship_wnd)->launch_btn_pressed != 0)) {
        ship_wnd->launch_state = 2;
        wnd_end_modal_loop((ModalMenuWindow *)(active_wnd + 0x48));
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
  hit = clickmap_hit_test
                    ((ClickMapHitTestView *)((iptr)spaceship_launch_sprite_xs_block.spaceship_wnd + 0xae4),x,y,(iptr)&hotspot_id,0);
  if (hit == -1) {
    if ((((ship_wnd)->close_btn_pressed != 0) &&
        (spaceship_wnd_draw_close_btn(ship_wnd,1,1), ship_wnd->launch_state == 1)) &&
       (ship_wnd->blink_timer == 0)) {
      timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
      ship_wnd->blink_timer = timer;
    }
    if ((((ship_wnd)->launch_btn_pressed != 0) &&
        (spaceship_wnd_draw_launch_btn(ship_wnd,1,1), ship_wnd->launch_state == 1)) &&
       (ship_wnd->blink_timer == 0)) {
      timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
      ship_wnd->blink_timer = timer;
    }
  }
  else if (hotspot_id == 0) {
    if ((ship_wnd)->launch_btn_pressed == 0) {
      if ((((ship_wnd)->close_btn_pressed != 0) &&
          (spaceship_wnd_draw_close_btn(ship_wnd,1,0), ship_wnd->launch_state == 1)) &&
         (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
    }
    else {
      spaceship_wnd_draw_launch_btn(ship_wnd,1,1);
      if ((ship_wnd->launch_state == 1) && (ship_wnd->blink_timer == 0)) {
        timer = game_timer_create((TimerFn)spaceship_wnd_blink_launch_btn,500,0xffffffff);
        ship_wnd->blink_timer = timer;
      }
    }
  }
  else if (hotspot_id == 1) {
    if ((ship_wnd)->close_btn_pressed == 0) {
      if ((((ship_wnd)->launch_btn_pressed != 0) &&
          (spaceship_wnd_draw_launch_btn(ship_wnd,1,0), ship_wnd->launch_state == 1)) &&
         (ship_wnd->blink_timer != 0)) {
        game_timer_destroy(ship_wnd->blink_timer);
        ship_wnd->blink_timer = 0;
      }
    }
    else {
      spaceship_wnd_draw_close_btn(ship_wnd,1,1);
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
    value = civ_has_advance(civ,0x20);
    if (value != 0) {
      civs[civ].spaceship_flags = civs[civ].spaceship_flags | 8;
    }
  }
  flags = civs[civ].spaceship_flags;
  unit_sound_editor_window_block.spaceship_mass = 0;
  for (part = 0; part < 6; part++) {
    unit_sound_editor_window_block.spaceship_mass = unit_sound_editor_window_block.spaceship_mass +
                   (int)civs[civ].spaceship_parts[part] *
                   spaceship_part_table_block.spaceship_parts[part].mass;
  }
  full_mass = unit_sound_editor_window_block.spaceship_mass;
  for (part = 0; part < 6; part++) {
    if (civs[civ].spaceship_parts[part] == 0) {
      full_mass += spaceship_part_table_block.spaceship_parts[part].mass;
    }
  }
  unit_sound_editor_window_block.spaceship_success_pct = 100;
  value = spaceship_part_count(civ,4);
  arg_b = 99;
  arg_a = 1;
  part_count = spaceship_part_count(civ,3);
  divisor = int_clamp(part_count,arg_a,arg_b);
  unit_sound_editor_window_block.spaceship_support_pct = (value * 100) / divisor;
  if (apply_support != 0) {
    value = int_clamp(unit_sound_editor_window_block.spaceship_support_pct,0,100);
    unit_sound_editor_window_block.spaceship_success_pct = (value * unit_sound_editor_window_block.spaceship_success_pct) / 100;
  }
  value = spaceship_part_count(civ,5);
  arg_b = 99;
  arg_a = 1;
  divisor = spaceship_part_count(civ,3);
  temp = spaceship_part_count(civ,4);
  divisor = int_clamp(divisor + temp,arg_a,arg_b);
  unit_sound_editor_window_block.spaceship_energy_pct = (value * 200) / divisor;
  value = int_clamp(unit_sound_editor_window_block.spaceship_energy_pct,0,100);
  unit_sound_editor_window_block.spaceship_success_pct = (value * unit_sound_editor_window_block.spaceship_success_pct) / 100;
  value = spaceship_part_count(civ,2);
  arg_b = 99;
  arg_a = 1;
  part_count = spaceship_part_count(civ,1);
  divisor = int_clamp(part_count,arg_a,arg_b);
  unit_sound_editor_window_block.spaceship_fuel_pct = (value * 100) / divisor;
  paradigm = (uint)cosmic.mass_thrust_paradigm;
  if ((flags & 8) != 0) {
    paradigm = (int)(paradigm * 3) >> 2;
  }
  scale = 1;
  for (; 100 < paradigm; paradigm = (int)paradigm >> 1) {
    scale = scale << 1;
  }
  temp = unit_sound_editor_window_block.spaceship_mass * paradigm;
  arg_a = spaceship_part_count(civ,2);
  value = spaceship_component_thrust(arg_a);
  value = value * 10;
  arg_b = 0;
  arg_a = spaceship_part_count(civ,1);
  divisor = spaceship_component_thrust(arg_a);
  value = int_clamp(divisor * 10,arg_b,value);
  unit_sound_editor_window_block.spaceship_flight_time = temp / (value + 1);
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
    unit_sound_editor_window_block.spaceship_flight_time = scale * unit_sound_editor_window_block.spaceship_flight_time;
    flight_time = scale * flight_time;
    full_flight_time = scale * full_flight_time;
  }
  if (0x96 < unit_sound_editor_window_block.spaceship_flight_time) {
    unit_sound_editor_window_block.spaceship_success_pct -= (unit_sound_editor_window_block.spaceship_flight_time - 0x96) / 10;
  }
  unit_sound_editor_window_block.spaceship_success_pct = int_clamp(unit_sound_editor_window_block.spaceship_success_pct,0,100);
  value = civ_spaceship_launched(civ);
  if (value == 0) {
    arg_a = spaceship_part_count(civ,3);
    civs[civ].spaceship_population = (i16)arg_a;
    arg_a = game_turn_to_year((int)game.turn);
    civs[civ].spaceship_launch_year = (i16)arg_a;
    arg_a = game_turn_to_year((int)game.turn);
    civs[civ].spaceship_arrival_year = (short)arg_a + (short)(flight_time / 10);
    value = game_turn_to_year((int)game.turn);
    unit_sound_editor_window_block.spaceship_projected_arrival_year = value + full_flight_time / 10;
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
  
  civs[civ].spaceship_flags = civs[civ].spaceship_flags | 2;
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
        if (game.difficulty < 4) {
          if ((game.difficulty < 2) || (civs[civ].reputation == '\0')) {
            diplo_set_treaty_flags(civ,civ2,0x10000);
          }
          else {
            treaty = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
            *treaty = *treaty | 0x20;
          }
        }
        else if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
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
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  dialog_ctor(&dlg,0x2000);
  eh_state = 0;
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
  minimap_window_block.text_string_args_plus1 = 0;
  __strupr(((i8 *)&minimap_window_block.text_string_args));
  dialog_load_game_text(&dlg,(const char *)&str_spaceship);
  for (part = 0; part < 6; part++) {
    part_count = civs[civ].spaceship_parts[part];
    result = spaceship_part_limit(civ,part);
    if (result < part_count) {
      over_limit = 1;
    }
  }
  for (part = 0; part < 6; part++) {
    text_clear();
    text_append_string_id(label_string_ids[spaceship_part_table_block.spaceship_parts[part].label_id]
                      );
    text_append_colon();
    text_append_int((int)civs[civ].spaceship_parts[part]);
    if ((over_limit != 0) && (part == 0)) {
      text_append((const char *)&str_empty_176);
    }
    dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  }
  text_clear();
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  text_clear();
  text_append_label(0x22);
  text_append_colon();
  text_append_int((int)civs[civ].spaceship_parts[3]);
  text_append((const char *)&str_0_000_7);
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  text_clear();
  text_append_label(0x42);
  text_append_colon();
  text_append_int(unit_sound_editor_window_block.spaceship_support_pct);
  text_append_percent();
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  text_clear();
  text_append_label(0xcd);
  text_append_colon();
  text_append_int(unit_sound_editor_window_block.spaceship_energy_pct);
  text_append_percent();
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  text_clear();
  text_append_label(0xce);
  text_append_colon();
  text_append_int(unit_sound_editor_window_block.spaceship_mass / 10);
  text_append((const char *)&str_empty_177);
  text_append_int(unit_sound_editor_window_block.spaceship_mass % 10);
  text_append((const char *)&str_00_2);
  text_append_label(0xd1);
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  text_clear();
  text_append_label(200);
  text_append_colon();
  text_append_int(unit_sound_editor_window_block.spaceship_fuel_pct);
  text_append_percent();
  if ((civs[civ].spaceship_flags & 8) != 0) {
    text_append_space();
    text_append_lparen();
    text_append_label(0xeb);
    text_append_rparen();
  }
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  text_clear();
  text_append_label(0xcf);
  text_append_colon();
  text_append_int(unit_sound_editor_window_block.spaceship_flight_time / 10);
  text_append((const char *)&str_empty_178);
  text_append_int(unit_sound_editor_window_block.spaceship_flight_time % 10);
  text_append_space();
  text_append_label(0xd2);
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  text_clear();
  text_append_label(0xd0);
  text_append_colon();
  text_append((const char *)&str_empty_179);
  text_append_int(unit_sound_editor_window_block.spaceship_success_pct);
  text_append_percent();
  text_append((const char *)&str_empty_180);
  dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  result = civ_spaceship_launched(civ);
  if (result == 0) {
    if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (allow_launch != 0)) &&
       (unit_sound_editor_window_block.spaceship_success_pct != 0)) {
      str = (iptr)label_string(label_string_ids[0xd5]);
      dialog_add_button(&dlg,(const char *)str);
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
    dialog_add_option(&dlg,(const char *)&map_views_block.text_buffer,0,0);
  }
  dlg_show_modal(&dlg,0);
  if (dlg.result_extra != 0) {
    text_set_number_arg(0,unit_sound_editor_window_block.spaceship_success_pct);
    result = popup_game((const char *)&str_launch);
    if (result != 0) {
      spaceship_launch(civ);
    }
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 00597d4b */ { dialog_dtor(&dlg); }
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
  if ((!chosen) && ((group != 1 || (!race_close)))) {
    count = spaceship_group_count_clamped(civ,group);
    limit = spaceship_group_limit(civ,group);
    if ((count < limit) && (count = spaceship_group_can_build(civ,group), count != 0))
    goto check_group_zero;
  }
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
check_group_zero:
  if (group == 0) {
    for (civ2 = 1; civ2 < 8; civ2++) {
      if (((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
           (count = civ_spaceship_launched(civ2), count != 0)) &&
          (civs[civ2].spaceship_arrival_year <= unit_sound_editor_window_block.spaceship_projected_arrival_year)) &&
         ((count = spaceship_group_full(civ,1), count == 0 &&
          (count = civ_has_advance(civ,(int)improvements[0x24].preq), count != 0)))) {
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
      text_set_label_arg(0,label_string_ids[spaceship_part_table_block.spaceship_parts[part].label_id]);
      popup_game((const char *)&str_nofurther);
    }
    result = civ_spaceship_exists(civ);
    if (result == 0) {
      civs[civ].spaceship_flags = civs[civ].spaceship_flags | 1;
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
      if ((result == 0) && (0x27 < unit_sound_editor_window_block.spaceship_success_pct)) {
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
          if ((civ4 == 0) && (3 < game.difficulty)) {
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
            should_launch = 0x4b < unit_sound_editor_window_block.spaceship_success_pct;
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
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  dialog_ctor(&dlg,0x4000);
  eh_state = 0;
  dlg_load_game_section(&dlg,(const char *)&str_spaceships,1);
  any_ship = false;
  for (civ2 = 1; civ2 < 8; civ2++) {
    civ = civ_spaceship_exists(civ2);
    if (civ != 0) {
      flags = 0;
      civ = civ2;
      name2 = (iptr)(civ_name_plural(civ2));
      dialog_add_option(&dlg,(const char *)name2,civ,flags);
      any_ship = true;
    }
  }
  if (any_ship) {
    civ = dlg_show_modal(&dlg,0);
    if (civ < 0) {
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00598cc7 */ { dialog_dtor(&dlg); }
    }
    else {
      spaceship_wnd_load_parts(civ);
      shown = spaceship_wnd_run(civ);
      if (shown == 0) {
        (*((char *)&civ2 + 0)) = (byte)civ;
        spaceship_status_dialog(civ,(uint)game.human_players & 1 << ((byte)civ2 & 0x1f));
      }
      eh_state = 0xffffffff;
      /* inlined unwind funclet 00598cc7 */ { dialog_dtor(&dlg); }
    }
  }
  else {
    popup_game((const char *)&str_nospaceships);
    eh_state = 0xffffffff;
    /* inlined unwind funclet 00598cc7 */ { dialog_dtor(&dlg); }
  }
  return;
}

/* 00598ceb auto */
i32 __cdecl spaceship_race_active(void)

{
  i32 active;
  
  if ((game.rules & 0x80) == 0) {
    if ((game.wonder_city[0x19] == -1) && ((game.human_players & game.advance_known_by[0x4c]) == 0))
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
