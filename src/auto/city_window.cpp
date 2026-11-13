/* The game, cities: the city window (its panels, buttons and menus): functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00500e00 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_font_global_init(void)

{
  city_window_font_static_init();
  city_window_font_reg_atexit();
  return;
}

/* 00500e1a auto */
void __cdecl city_window_font_static_init(void)

{
  ((GameFont *)((i8 *)&city_window_font))->init(0,0x10);
  return;
}

/* 00500e38 auto */
void __cdecl city_window_font_reg_atexit(void)

{
  _atexit(city_window_font_atexit_dtor);
  return;
}

/* 00500e55 auto */
void __cdecl city_window_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&city_window_font))->destroy();
  return;
}

/* 00500e6f auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_small_font_global_init(void)

{
  city_window_small_font_static_init();
  city_window_small_font_reg_atexit();
  return;
}

/* 00500e89 auto */
void __cdecl city_window_small_font_static_init(void)

{
  ((GameFont *)((i8 *)&city_window_small_font))->init(0,10);
  return;
}

/* 00500ea7 auto */
void __cdecl city_window_small_font_reg_atexit(void)

{
  _atexit(city_window_small_font_atexit_dtor);
  return;
}

/* 00500ec4 auto */
void __cdecl city_window_small_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&city_window_small_font))->destroy();
  return;
}

/* 00500ede auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_global_init(void)

{
  city_window_static_init();
  city_window_reg_atexit();
  return;
}

/* 00500ef8 auto */
void __cdecl city_window_static_init(void)

{
  ((CityWindow *)((i8 *)&city_wnd.window))->construct();
  return;
}

/* 00500f12 auto */
void __cdecl city_window_reg_atexit(void)

{
  _atexit(city_window_atexit_dtor);
  return;
}

/* 00500f2f auto */
void __cdecl city_window_atexit_dtor(void)

{
  ((CityWindow *)((i8 *)&city_wnd.window))->destruct();
  return;
}

/* 00500f49 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_window_bg_surface_global_init(void)

{
  city_window_bg_surface_static_init();
  city_window_bg_surface_reg_atexit();
  return;
}

/* 00500f63 auto */
void __cdecl city_window_bg_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&city_window_bg_surface));
  return;
}

/* 00500f7d auto */
void __cdecl city_window_bg_surface_reg_atexit(void)

{
  _atexit(city_window_bg_surface_atexit_dtor);
  return;
}

/* 00500f9a auto */
void __cdecl city_window_bg_surface_atexit_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&city_window_bg_surface));
  return;
}

/* 00500fb4 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl city_tile_pick_surface_global_init(void)

{
  city_tile_pick_surface_static_init();
  city_tile_pick_surface_reg_atexit();
  return;
}

/* 00500fce auto */
void __cdecl city_tile_pick_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&city_tile_pick_surface));
  return;
}

/* 00500fe8 auto */
void __cdecl city_tile_pick_surface_reg_atexit(void)

{
  _atexit(city_tile_pick_surface_atexit_dtor);
  return;
}

/* 00501005 auto */
void __cdecl city_tile_pick_surface_atexit_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&city_tile_pick_surface));
  return;
}

/* 0050101f auto */
void __cdecl city_window_load_background(void)

{
  i32 height;
  i32 width;
  i8 dest_rect[16];
  DibSurface surface;
  GamePalette palette;
  Sprite sprite;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  if (city_wnd.size_level != city_wnd_state.bg_size_level) {
    city_wnd_state.bg_size_level = city_wnd.size_level;
    ((DibSurface *)((i8 *)&city_window_bg_surface))->release();
    surface.load_gif_file((const char *)&str_city_gif,10,0xc0,&palette);
    sprite.grab_from_surface(&surface,7,0,0,0x27c,0x1a5);
    surface.release();
    height = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x1a5);
    width = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x27c);
    ((DibSurface *)((i8 *)&city_window_bg_surface))->DibSurface::create(width,height);
    gfx_set_zoom_scale(city_wnd_state.bg_size_level * 4 - 8);
    sprite.draw((RECT *)dest_rect,&city_window_bg_surface,0,0);
    gfx_reset_zoom_scale();
  }
  return;
}

/* 005013bc auto */
void __cdecl city_window_modal_notice(void)

{
  if (city_wnd_state.modal_notice_active == '\0') {
    city_wnd_state.modal_notice_active = 1;
    text_clear();
    text_append((const char *)&str_citymodal);
    text_append_int((city_wnd.open_mode == 2) + 1);
    popup_game((const char *)&textbuf.buf);
    if (dlg_input.help_pressed != 0) {
      city_window_exit_clicked(0);
    }
    city_wnd_state.modal_notice_active = '\0';
  }
  return;
}

/* 00501440 auto */
void CityWindow::reset_state()

{
  clickmap.clear();
  is_closed_b0[0] = 1;
  is_closed_b1[0] = '\0';
  is_closed_b2[0] = '\0';
  is_closed_b3[0] = '\0';
  redraw_suspended_b0[0] = '\0';
  redraw_suspended_b1[0] = '\0';
  redraw_suspended_b2[0] = '\0';
  redraw_suspended_b3[0] = '\0';
  redraw_deferred_b0[0] = '\0';
  redraw_deferred_b1[0] = '\0';
  redraw_deferred_b2[0] = '\0';
  redraw_deferred_b3[0] = '\0';
  city_index_b0[0] = 0xff;
  city_index_b1[0] = 0xff;
  city_index_b2[0] = 0xff;
  city_index_b3[0] = 0xff;
  improvement_scroll_pos_b0[0] = '\0';
  improvement_scroll_pos_b1[0] = '\0';
  improvement_scroll_pos_b2[0] = '\0';
  improvement_scroll_pos_b3[0] = '\0';
  panel_toggle_flag_b0[0] = 1;
  panel_toggle_flag_b1[0] = '\0';
  panel_toggle_flag_b2[0] = '\0';
  panel_toggle_flag_b3[0] = '\0';
  exit_button_b0[0] = '\0';
  exit_button_byte1[0] = '\0';
  exit_button_byte2[0] = '\0';
  exit_button_byte3[0] = '\0';
  return;
}

/* 005014bf auto */
/* Library Function - Single Match
    public: void __thiscall CMiniDockFrameWnd::OnClose(void)
   
   Library: Visual Studio 1998 Debug */

void CityWindow::remove_citizen_hotspots()

{
  clickmap.remove_by_id(2);
  return;
}

/* 005014e5 auto */
void CityWindow::remove_panel_hotspots()

{
  clickmap.remove_by_id(1);
  clickmap.remove_by_id(3);
  clickmap.remove_by_id(4);
  return;
}

/* 0050152b auto */
void CityWindow::remove_hotspots()

{
  remove_panel_hotspots();
  remove_citizen_hotspots();
  return;
}

/* 00501551 auto */
CityWindow *CityWindow::construct()

{
  
  this->FrameWindow::construct();
  clickmap.construct();
  city_font.init_empty();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 5);
  reset_state();
  return this;
}

/* 0050160a auto */
void CityWindow::destruct()

{
  int seh_state;
  
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 5);
  seh_state = seh_state & 0xff;
  (*((char *)&seh_state + 0)) = 2;
  remove_hotspots();
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 00501673 */ { city_font.destroy(); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 00501682 */ { clickmap.destruct(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00501691 */ { (((FrameWindow *)this))->FrameWindow::destruct(); }
  return;
}

/* 005016b2 auto */
void CityWindow::blit_bg_rect(int *rect)

{
  i32 height;
  i32 width;
  
  height = rect_height((RECT *)rect);
  width = rect_width((RECT *)rect);
  gfx_copy_rect_xywh(&city_window_bg_surface,this,
                     (*rect - margin_x_2) - client_x,
                     (rect[1] - content_offset_y) - client_y,*rect,
                     rect[1],width,height);
  return;
}

/* 00501733 auto */
void CityWindow::begin_draw(RECT *rect)

{
  gfx_set_draw_target(this);
  text_set_surface(this);
  text_set_font(((GameFont *)(u8 *)&city_font));
  blit_bg_rect((int *)rect);
  return;
}

/* 00501780 auto */
void CityWindow::recalc_redraw(i32 flag)

{
  if ((((((u32)is_closed) == 0) && (((u32)redraw_suspended) == 0)) && (((u32)redraw_deferred) == 0)) &&
     (-1 < (int)((u32)city_index))) {
    city_recalc(((u32)city_index),1);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_citizens(1,flag);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_resource_map(1);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
  }
  return;
}

/* 00501819 auto */
void __cdecl city_window_cycle_specialist(int citizen_idx)

{
  iptr city3;
  iptr city;
  iptr city4;
  int city2;
  int spec_idx;
  short spec_slot;
  i32 recalc_flag;
  int spec_type;
  
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
    recalc_flag = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_flag);
    ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    if (cities[city4].size - city_calc.specialist_count <= (int)(short)citizen_idx) {
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      spec_slot = (short)citizen_idx - ((short)cities[city2].size - (short)city_calc.specialist_count);
      if (spec_slot < city_calc.specialist_count) {
        spec_idx = (int)spec_slot;
        city2 = spec_idx;
        city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        spec_type = city_specialist_type(city,city2);
        city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        if (cities[city2].size < 5) {
          if (spec_type == 1) {
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            popup_game_with_city((const char *)&str_elviserr,city);
            return;
          }
          spec_type = 1;
        }
        else {
          spec_type++;
          if (3 < spec_type) {
            spec_type = 1;
          }
        }
        city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        city_set_specialist_type(city,spec_idx,spec_type);
        ((CityWindow *)((i8 *)&city_wnd.window))->recalc_redraw(1);
      }
    }
  }
  return;
}

/* 005019c1 auto */
int CityWindow::draw_citizen_row(int x,int y,i32 width,int happy,int unhappy, int angry)

{
  int arg_y;
  int arg_x;
  int style_era;
  int spec_kind;
  i8 *rect_arg;
  CityWindow *view;
  iptr zero_a;
  iptr zero_b;
  i32 shadow_color;
  i8 spec_rect[16];
  i8 spec_shadow_rect[16];
  i8 unhappy_rect[16];
  i8 unhappy_shadow_rect[16];
  i8 content_rect[16];
  i8 content_shadow_rect[16];
  i8 happy_rect[16];
  i8 happy_shadow_rect[16];
  int mood_idx;
  int spacing;
  int i;
  uint citizen_num;
  
  gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
  zero_b = 0;
  zero_a = 0;
  arg_y = scale(0x1b);
  spacing = ui_calc_item_spacing((int)cities[((u32)city_index)].size,arg_y + 1,width,(int *)zero_a,(int *)zero_b)
  ;
  citizen_num = 0;
  for (i = 0; i < happy; i++) {
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = happy_shadow_rect;
    view = this;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (citizen_num & 1) * 0x3c))->draw_silhouette((RECT *)rect_arg,
               view,arg_x,arg_y,shadow_color);
    rect_arg = happy_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (citizen_num & 1) * 0x3c))->draw((RECT *)rect_arg,
               view,arg_y,arg_x);
    x += spacing;
    citizen_num++;
  }
  for (i = 0;
      i < (int)cities[((u32)city_index)].size - (unhappy + happy + city_calc.specialist_count);
      i = i + 1) {
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = content_shadow_rect;
    view = this;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) * 4 + 8) * 0xf))->draw_silhouette(
               (RECT *)rect_arg,view,arg_x,arg_y,shadow_color);
    rect_arg = content_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) * 4 + 8) * 0xf))->draw(
               (RECT *)rect_arg,view,arg_y,arg_x);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < unhappy; i++) {
    if (i < unhappy - angry) {
      mood_idx = 4;
    }
    else {
      mood_idx = 6;
    }
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = unhappy_shadow_rect;
    view = this;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) + mood_idx) * 0x3c))->draw_silhouette(
               (RECT *)rect_arg,view,arg_x,arg_y,shadow_color);
    rect_arg = unhappy_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + ((citizen_num & 1) + mood_idx) * 0x3c))->draw(
               (RECT *)rect_arg,view,arg_y,arg_x);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < city_calc.specialist_count; i++) {
    shadow_color = 10;
    arg_y = y + 1;
    arg_x = x + 1;
    rect_arg = spec_shadow_rect;
    view = this;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    spec_kind = city_specialist_type(((u32)city_index),i);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (spec_kind * 4 + 0x1c) * 0xf))->draw_silhouette(
               (RECT *)rect_arg,view,arg_x,arg_y,shadow_color);
    rect_arg = spec_rect;
    view = this;
    arg_y = x;
    arg_x = y;
    style_era = civ_city_style_era(cities[((u32)city_index)].owner);
    spec_kind = city_specialist_type(((u32)city_index),i);
    ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + style_era * 0x294 + (spec_kind * 4 + 0x1c) * 0xf))->draw(
               (RECT *)rect_arg,view,arg_y,arg_x);
    x += spacing;
  }
  gfx_reset_zoom_scale();
  return spacing;
}

/* 00501e63 auto */
int CityWindow::happy_draw_citizen_row(int x,i32 y,i32 width,int happy, int unhappy,int angry)

{
  int arg_x;
  int spec_kind;
  i8 *rect_arg;
  CityWindow *view;
  iptr zero_a;
  iptr zero_b;
  i32 arg_y;
  i8 spec_rect[16];
  i8 unhappy_rect[16];
  i8 content_rect[16];
  i8 happy_rect[16];
  int mood_idx;
  int spacing;
  int i;
  uint citizen_num;
  
  gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
  zero_b = 0;
  zero_a = 0;
  arg_x = scale(0xf);
  spacing = ui_calc_item_spacing((int)cities[((u32)city_index)].size,arg_x + 1,width,(int *)zero_a,(int *)zero_b)
  ;
  citizen_num = 0;
  for (i = 0; i < happy; i++) {
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + (citizen_num & 1) * 0x3c))->draw((RECT *)happy_rect,this,
               x,y);
    x += spacing;
    citizen_num++;
  }
  for (i = 0;
      i < (int)cities[((u32)city_index)].size - (unhappy + happy + city_calc.specialist_count);
      i = i + 1) {
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + ((citizen_num & 1) * 4 + 8) * 0xf))->draw((RECT *)content_rect,
               this,x,y);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < unhappy; i++) {
    if (i < unhappy - angry) {
      mood_idx = 4;
    }
    else {
      mood_idx = 6;
    }
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + ((citizen_num & 1) + mood_idx) * 0x3c))->draw((RECT *)unhappy_rect,
               this,x,y);
    x += spacing;
    citizen_num++;
  }
  for (i = 0; i < city_calc.specialist_count; i++) {
    rect_arg = spec_rect;
    view = this;
    arg_x = x;
    arg_y = y;
    spec_kind = city_specialist_type(((u32)city_index),i);
    ((Sprite *)(((i8 *)&government_sprites_block.small_citizen_sprites) + (spec_kind * 4 + 0x1c) * 0xf))->draw((RECT *)rect_arg,view
               ,arg_x,arg_y);
    x += spacing;
  }
  gfx_reset_zoom_scale();
  return spacing;
}

/* 0050207f auto */
void CityWindow::draw_citizens(int blit,i32 flag)

{
  int top;
  i32 value;
  int pos;
  int offset;
  i32 row_width;
  iptr height;
  i32 unhappy;
  i32 angry;
  int x2;
  int i;
  i8 hotspot_rect[16];
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw(((RECT *)&citizens_rect));
    clickmap.remove_by_id(2);
    x2 = citizens_rect;
    top = citizens_rect_top;
    text_set_colors(0x7c,0x12,1,1);
    if (extra_ui(X_CITY_ATTITUDE)) {
      /* X23: coloured as in the Attitude Advisor: yellow celebrating, red
         in disorder */
      text_set_colors(extras_attitude_color(city_index, 0x7c),0x12,1,1);
    }
    text_clear();
    text_append_label(0x193);
    value = scale(0xc6);
    pos = scale(0x2e);
    pos = top + pos;
    offset = scale(2);
    gfx_draw_text_centered((const char *)&textbuf.buf,x2 + offset,pos,value);
    text_clear();
    text_append_label(0x3f);
    value = scale(0xee);
    pos = scale(0x2e);
    pos = top + pos;
    offset = scale(199);
    gfx_draw_text_centered((const char *)&textbuf.buf,x2 + offset,pos,value);
    value = city_calc.happy_count;
    unhappy = city_calc.unhappy_count;
    angry = city_calc.angry_count;
    row_width = scale(0x1a6);
    pos = scale(9);
    pos = top + pos;
    offset = scale(5);
    pos = draw_citizen_row(x2 + offset,pos,row_width,value,unhappy,angry);
    if (bugfix_ui()) {
      /* B10: each hotspot centred on its citizen, not at its sprite's left edge,
         which later sprites cover when they are drawn closer (docs/BUGS.md) */
      offset = scale(5) + scale(0xe) - pos / 2 - 1;
    }
    else {
      offset = scale(5);
    }
    x2 += offset;
    offset = scale(9);
    for (i = 0; i < cities[city_index].size;
        i = i + 1) {
      height = scale(0x1e);
      rect_set_xywh((RECT *)hotspot_rect,x2,top + offset,pos,height);
      clickmap.hotspot_add_rect(i,2,(RECT *)hotspot_rect);
      x2 += pos;
    }
    if (blit != 0) {
      wnd_blit_surface_rect(((RECT *)&citizens_rect));
    }
  }
  return;
}

/* 005022c0 auto */
void __cdecl city_window_toggle_worked_tile(int mouse_x,int mouse_y)

{
  iptr city3;
  iptr city;
  iptr tile_w;
  int city2;
  int rel_x;
  int rel_y;
  iptr city5;
  iptr city4;
  i32 flag_arg;
  int dy;
  iptr tile_idx;
  int dx;
  iptr quadrant;
  iptr i;
  
  tile_idx = -1;
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
    flag_arg = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,flag_arg);
    tile_w = gfx_zoom_scale(0x40,city_wnd.map_zoom);
    city2 = gfx_zoom_scale(0x20,city_wnd.map_zoom);
    rel_x = ((CityWindow *)((i8 *)&city_wnd.window))->scale(5);
    rel_x = city_wnd.map_rect + rel_x;
    rel_y = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0xb);
    rel_x = mouse_x - rel_x;
    rel_y = mouse_y - (city_wnd.map_rect_top + rel_y + (city2 >> 1));
    if ((-1 < rel_x) && (((rel_x < tile_w * 4 && (-1 < rel_y)) && (rel_y < city2 * 4)))) {
      dx = (rel_x / tile_w) * 2 - 3;
      dy = (rel_y / city2) * 2 - 3;
      if ((rel_x % tile_w < 0) || (rel_y % city2 < 0)) {
        quadrant = 0;
      }
      else {
        city2 = ((DibSurface *)((i8 *)&city_tile_pick_surface))->get_pixel(rel_x % tile_w,rel_y % city2);
        quadrant = city2 - 10 >> 4;
      }
      if (quadrant != 0) {
        dx += (((i8 *)&diag_neighbor_bits_block.pick_quadrant_dx_table))[quadrant];
        dy += (((i8 *)&diag_neighbor_bits_block.pick_quadrant_dy_table))[quadrant];
      }
      for (i = 0; i < 0x15; i++) {
        if (((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] == dx) &&
           ((((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] == dy)) {
          tile_idx = i;
          break;
        }
      }
      if (-1 < tile_idx) {
        if (tile_idx == 0x14) {
          city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          cities[city2].worked_tiles = 0;
        }
        else {
          city = tile_idx;
          city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          city2 = city_tile_worked(city5,city);
          if (city2 == 0) {
            if ((((i8 *)&city_calc.tile_flags))[tile_idx] != '\0') {
              return;
            }
            if (city_calc.specialist_count == 0) {
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city2].worked_tiles = 0;
            }
            else {
              flag_arg = 1;
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_set_tile_worked(city,tile_idx,flag_arg);
              flag_arg = 0xffffffff;
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_adjust_specialists(city,flag_arg);
            }
          }
          else {
            flag_arg = 0;
            city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_set_tile_worked(city4,tile_idx,flag_arg);
            flag_arg = 1;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_adjust_specialists(city,flag_arg);
          }
        }
        ((CityWindow *)((i8 *)&city_wnd.window))->recalc_redraw(0);
      }
    }
  }
  return;
}

/* Part of CityWindow::draw_resource_map (005025d5): the terrain, units, cities and huts
   of the 21 tiles of the city radius. */
void CityWindow::draw_resource_tiles(int half_tile_w,int half_tile_h,int origin_x2,int origin_y2)
{
  i8 cursor_rect[16];
  i8 hut_rect[16];
  int x2;
  int tile_y;
  iptr unit;
  int tile_x;
  int city;
  int unit_y;
  iptr tile_idx;
  iptr j;
  iptr y2;
  iptr i;
  i32 drew_city;

  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((int)cities[city_index].x +
                          (int)(((i8 *)&city_resmap_draw_dx_table))[i]);
    y2 = (int)cities[city_index].y + (int)(((i8 *)&city_resmap_draw_dy_table))[i]
    ;
    tile_x = (((i8 *)&city_resmap_draw_dx_table))[i] * half_tile_w + origin_x2;
    tile_y = (((i8 *)&city_resmap_draw_dy_table))[i] * half_tile_h + origin_y2;
    unit_y = tile_y - half_tile_h;
    tile_idx = 0;
    for (j = 0; j < 0x15; j++) {
      if (((((i8 *)&dir_dy_table_block.city_tile_dx_table))[j] == (((i8 *)&city_resmap_draw_dx_table))[i]) &&
         ((((i8 *)&dir_dy_table_block.city_tile_dy_table))[j] == (((i8 *)&city_resmap_draw_dy_table))[i])) {
        tile_idx = j;
        break;
      }
    }
    ((MapView *)((i8 *)&map_views_block.map_views))->draw_tile_terrain(this,tile_x,tile_y,x2,y2,
               (int)cities[city_index].owner,
               map_zoom_level,tile_idx);
    if (i < 0x15) {
      if (((((i8 *)&city_calc.tile_flags))[tile_idx] & 8) == 0) {
        if (((((i8 *)&city_calc.tile_flags))[tile_idx] & 4) == 0) {
          if (((((i8 *)&city_calc.tile_flags))[tile_idx] & 2) != 0) {
            gfx_set_zoom_scale(map_zoom_level);
            ((Sprite *)((i8 *)&hut_sprite))->draw((RECT *)hut_rect,this,tile_x,tile_y
                      );
            gfx_reset_zoom_scale();
          }
        }
        else {
          unit = tile_first_unit(x2,y2);
          while ((-1 < unit && (unit_types[units[unit].type].attack == '\0'))) {
            unit = unit_next_in_stack(unit);
          }
          if (-1 < unit) {
            gfx_draw_unit(this,unit,7,tile_x,unit_y,
                               map_zoom_level,0);
          }
        }
      }
      else {
        city = city_at(x2,y2);
        if (-1 < city) {
          drew_city = 1;
          gfx_draw_city(this,city,1,tile_x,unit_y,
                             map_zoom_level);
        }
      }
      if ((((((i8 *)&city_calc.tile_flags))[tile_idx] & 0x10) != 0) && (((((i8 *)&city_calc.tile_flags))[tile_idx] & 9) == 0)) {
        gfx_set_zoom_scale(map_zoom_level);
        ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->draw((RECT *)cursor_rect,this,tile_x,tile_y);
        gfx_reset_zoom_scale();
      }
    }
  }
}

/* Part of CityWindow::draw_resource_map (005025d5): the food, shield and trade icons
   on each worked tile. */
void CityWindow::draw_resource_yields(int tile_w,int half_tile_w,int half_tile_h,int origin_x2,int origin_y2)
{
  int scaled;
  int scaled2;
  int scaled3;
  iptr zero_a;
  iptr zero_b;
  i8 unhappy_icon_rect[16];
  i8 yield_icon_rect[16];
  int x2;
  int no_yield;
  int tile_y;
  int tile_x;
  iptr yield_kind;
  int unit_y;
  iptr tile_idx;
  int icon_y;
  int per_row;
  int icon_x;
  int min_per_row;
  int spacing;
  int row_count;
  iptr j;
  iptr y2;
  iptr i;

  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((int)cities[city_index].x +
                          (int)(((i8 *)&city_resmap_draw_dx_table))[i]);
    y2 = (int)cities[city_index].y + (int)(((i8 *)&city_resmap_draw_dy_table))[i]
    ;
    tile_x = (((i8 *)&city_resmap_draw_dx_table))[i] * half_tile_w + origin_x2;
    tile_y = (((i8 *)&city_resmap_draw_dy_table))[i] * half_tile_h + origin_y2;
    unit_y = tile_y - half_tile_h;
    tile_idx = 0;
    for (j = 0; j < 0x15; j++) {
      if (((((i8 *)&dir_dy_table_block.city_tile_dx_table))[j] == (((i8 *)&city_resmap_draw_dx_table))[i]) &&
         ((((i8 *)&dir_dy_table_block.city_tile_dy_table))[j] == (((i8 *)&city_resmap_draw_dy_table))[i])) {
        tile_idx = j;
        break;
      }
    }
    scaled = city_tile_worked(city_index,tile_idx);
    if (scaled != 0) {
      gfx_set_zoom_scale(view_scale * 4 - 8);
      no_yield = 0;
      city_calc_tile_yields(city_index,tile_idx,0);
      scaled = tile_w;
      per_row = city_calc.tile_shields + city_calc.tile_trade + city_calc.tile_yields[0];
      if (per_row == 0) {
        per_row = 1;
        no_yield = 1;
      }
      zero_b = 0;
      zero_a = 0;
      scaled2 = scale(8);
      scaled += scaled2 * -2;
      scaled2 = scale(10);
      spacing = ui_calc_item_spacing(per_row,scaled2 + 1,scaled,(int *)zero_a,(int *)zero_b);
      scaled = tile_w;
      scaled2 = scale(8);
      scaled3 = scale(10);
      min_per_row = (scaled + scaled2 * -2) / (scaled3 + 1);
      if (per_row <= min_per_row) {
        per_row = min_per_row;
      }
      yield_kind = 0;
      row_count = 0;
      icon_x = scale(8);
      icon_x = tile_x + icon_x;
      scaled2 = half_tile_h + tile_y;
      scaled = scale(10);
      icon_y = scaled2 - (scaled >> 1);
      for (; yield_kind < 3; yield_kind++) {
        while ((((i32 *)&city_calc.tile_yields))[yield_kind] != 0) {
          ((Sprite *)(((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites) + yield_kind * 0x3c))->draw((RECT *)yield_icon_rect,this
                     ,icon_x,icon_y);
          row_count++;
          if (row_count < per_row) {
            icon_x += spacing;
          }
          else {
            row_count = 0;
            icon_x = scale(8);
            icon_x = tile_x + icon_x;
            scaled = scale(0xb);
            icon_y += scaled;
          }
          (((i32 *)&city_calc.tile_yields))[yield_kind] = (((i32 *)&city_calc.tile_yields))[yield_kind] - 1;
        }
      }
      if (no_yield != 0) {
        ((Sprite *)((i8 *)&small_unhappy_icon_sprite))->draw((RECT *)unhappy_icon_rect,this,icon_x,icon_y);
      }
      gfx_reset_zoom_scale();
    }
  }
}

/* Part of CityWindow::draw_resource_map (005025d5): the food bar: eaten, surplus or
   shortage, and its labels. */
void CityWindow::draw_resource_food(int panel_left,int panel_top2,uint big_zoom)
{
  int scaled;
  int scaled2;
  i32 bar_height;
  i32 color;
  uint food_surplus_text;
  uint food_surplus_count;
  uint abs_food_surplus;
  i8 food_surplus_rect[16];
  i8 food_rect[16];
  int x2;
  int left_width;
  int food_needed;
  int icon_count;
  int icon_w;
  int right_x;
  int food_eaten;
  int label_x;
  int spacing;
  int right_width;
  int label_right;
  iptr y2;
  int produced;
  iptr i;
  int food_need_shown;
  int food_fit_count;
  int avail_width;
  int bar_x;
  uint surplus;

  food_needed = (int)cities[city_index].size * (uint)cosmic.food_per_citizen +
             city_calc.settlers_supported * city_calc.settler_food_cost;
  produced = city_calc.yield_totals[0];
  surplus = city_calc.yield_totals[0] - food_needed;
  icon_count = food_needed;
  if (food_needed <= city_calc.yield_totals[0]) {
    icon_count = city_calc.yield_totals[0];
  }
  food_eaten = food_needed;
  if ((int)surplus < 0) {
    food_eaten = city_calc.yield_totals[0];
  }
  food_need_shown = food_needed;
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  if ((((int)surplus < 0) && (produced != 0)) || ((0 < (int)surplus && (food_needed != 0)))) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    icon_count--;
  }
  spacing = ui_calc_item_spacing(icon_count,icon_w,avail_width,&food_fit_count,0);
  while (food_fit_count < icon_count) {
    icon_count--;
    if (produced < food_need_shown) {
      food_need_shown--;
    }
    else if (food_need_shown == produced) {
      produced--;
      food_need_shown--;
      food_eaten--;
    }
    else {
      produced--;
      food_eaten--;
    }
  }
  if ((surplus == 0) || (produced != 0)) {
    if (surplus == 0) {
      right_width = 0;
    }
    else {
      if ((int)surplus < 1) {
        abs_food_surplus = ~surplus + 1;
      }
      else {
        abs_food_surplus = surplus;
      }
      scaled = scale(2);
      scaled2 = scale(1);
      right_width = scaled + scaled2 + (abs_food_surplus - 1) * spacing + icon_w;
    }
  }
  else {
    right_width = scale(0xe2);
  }
  left_width = scale(0xe2);
  left_width -= right_width;
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  right_x = left_width + bar_x;
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0xe);
  y2 = panel_top2 + y2;
  if (view_scale == 3) {
    y2--;
  }
  if ((int)surplus < 0) {
    left_width += right_width;
    right_width = 0;
  }
  if (left_width != 0) {
    gfx_target_fill_rect(bar_x,y2,left_width,resource_bar_height,0x2d);
  }
  if (right_width != 0) {
    color = 0x30;
    bar_height = resource_bar_height;
    scaled = scale(6);
    gfx_target_fill_rect(right_x,y2,right_width + scaled,bar_height,color);
  }
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < food_eaten) {
    x2 = scale(1);
    x2 = bar_x + x2;
    for (i = 0; i < food_eaten; i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.resource_icon_sprites))->draw((RECT *)food_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  if (surplus != 0) {
    x2 = right_x;
    if (food_eaten < 1) {
      scaled = scale(1);
      x2 += scaled;
    }
    else {
      scaled = scale(4);
      x2 += (scaled >> 1);
    }
    i = 0;
    while( true ) {
      if ((int)surplus < 1) {
        food_surplus_count = ~surplus + 1;
      }
      else {
        food_surplus_count = surplus;
      }
      if ((int)food_surplus_count <= i) break;
      ((Sprite *)(((i8 *)&war_icon_sprite_block.resource_shortage_icon_sprites) + (uint)(0 < (int)surplus) * 0x3c))->draw(
                 (RECT *)food_surplus_rect,this,x2,y2);
      x2 += spacing;
      i++;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0xe);
  scaled = panel_top2 + scaled;
  y2 = scale(0xe);
  y2 = scaled - y2;
  text_clear();
  text_append_label(0x40);
  text_set_colors(0x2a,10,1,1);
  text_append_colon();
  text_append_int(city_calc.food_routes_out + food_eaten);
  if (city_calc.food_routes_in != city_calc.food_routes_out) {
    text_append_space();
    text_append_lparen();
    text_append_int(city_calc.food_routes_in - city_calc.food_routes_out);
    text_append_space();
    text_append_label(0xa9);
    text_append_rparen();
  }
  gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  text_clear();
  if ((int)surplus < 0) {
    text_append_label(0x4a);
    text_set_colors(0xb,0x1d,1,1);
  }
  else {
    text_append_label(0x41);
    text_set_colors(0x2e,10,1,1);
  }
  text_append_colon();
  if ((int)surplus < 1) {
    food_surplus_text = ~surplus + 1;
  }
  else {
    food_surplus_text = surplus;
  }
  text_append_int(food_surplus_text);
  gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
}

/* Part of CityWindow::draw_resource_map (005025d5): the shield bar: support, waste,
   surplus or shortage, and its labels. */
void CityWindow::draw_resource_shields(int panel_left,int panel_top2,uint big_zoom,uint &surplus)
{
  int scaled;
  int scaled2;
  int scaled3;
  i32 bar_height;
  int scaled4;
  i32 color;
  uint shield_surplus_text;
  uint shield_surplus_count;
  uint abs_shield_surplus;
  i8 shortage_rect[16];
  i8 coin_rect[16];
  i8 waste_rect[16];
  i8 shield_rect[16];
  int x2;
  int left_width;
  int mid_width;
  int fit_count;
  int icon_w;
  int mid_x;
  int support_shown;
  int right_x;
  int label_x;
  int spacing;
  int right_width;
  int label_right;
  int total_icons;
  iptr y2;
  int produced;
  iptr i;
  int lux_x;
  int avail_width;
  int shields_shown;
  int bar_x;

  support_shown = city_calc.shield_support;
  produced = city_calc.total_shields + city_calc.shield_waste;
  surplus = city_calc.total_shields - city_calc.shield_support;
  total_icons = city_calc.shield_support;
  if (city_calc.shield_support <= produced) {
    total_icons = produced;
  }
  shields_shown = produced;
  if (-1 < (int)surplus) {
    shields_shown = city_calc.shield_support;
  }
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  if ((((int)surplus < 0) && (produced != 0)) || ((0 < (int)surplus && (city_calc.shield_support != 0)))) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    total_icons--;
  }
  if (city_calc.shield_waste != 0) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    total_icons--;
  }
  spacing = ui_calc_item_spacing(total_icons,icon_w,avail_width,&fit_count,0);
  while (fit_count < total_icons) {
    total_icons--;
    if (produced - city_calc.shield_waste < support_shown) {
      support_shown--;
    }
    else {
      if (produced == city_calc.shield_waste) break;
      if (produced - city_calc.shield_waste == support_shown) {
        produced--;
        support_shown--;
        shields_shown--;
      }
      else {
        produced--;
        shields_shown--;
      }
    }
  }
  mid_width = 0;
  if (city_calc.shield_waste != 0) {
    mid_width = (city_calc.shield_waste - 1) * spacing + icon_w;
    if (surplus != 0) {
      scaled = scale(4);
      mid_width += (scaled >> 1);
    }
    if (produced != city_calc.shield_waste) {
      scaled = scale(4);
      mid_width += (scaled >> 1);
    }
  }
  if ((surplus == 0) || (produced != 0)) {
    if (surplus == 0) {
      right_width = 0;
    }
    else {
      if ((int)surplus < 1) {
        abs_shield_surplus = ~surplus + 1;
      }
      else {
        abs_shield_surplus = surplus;
      }
      scaled = scale(2);
      scaled2 = scale(1);
      right_width = scaled + scaled2 + (abs_shield_surplus - 1) * spacing + icon_w;
    }
  }
  else {
    right_width = scale(0xe2);
  }
  left_width = scale(0xe2);
  left_width -= (mid_width + right_width);
  if (mid_width != 0) {
    if (city_calc.shield_support == 0) {
      left_width = 0;
    }
    else {
      scaled = scale(2);
      scaled2 = scale(1);
      left_width = scaled + scaled2 + (city_calc.shield_support - 1) * spacing + icon_w;
    }
    mid_width = scale(0xe2);
    mid_width -= (right_width + left_width);
  }
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  lux_x = left_width + bar_x;
  right_x = mid_width + lux_x;
  /* B31: also when the waste's part of the bar came out 0 wide: the original
     then draws the waste at an x it never set (every mode: ours would use
     another stale value; docs/BUGS.md) */
  mid_x = ((mid_width >> 1) + lux_x) - ((city_calc.shield_waste - 1) * spacing + icon_w >> 1);
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0x78);
  y2 = panel_top2 + y2;
  if (view_scale == 3) {
    y2--;
  }
  if ((int)surplus < 0) {
    left_width += right_width;
    right_width = 0;
  }
  if (left_width != 0) {
    gfx_target_fill_rect(bar_x,y2,left_width,resource_bar_height,0x54);
  }
  if (mid_width != 0) {
    gfx_target_fill_rect(lux_x,y2,mid_width,resource_bar_height,0xb);
  }
  if (right_width != 0) {
    color = 0x5c;
    bar_height = resource_bar_height;
    scaled = scale(6);
    gfx_target_fill_rect(right_x,y2,right_width + scaled,bar_height,color);
  }
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < shields_shown) {
    x2 = scale(1);
    x2 = bar_x + x2;
    for (i = 0; i < shields_shown; i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->draw((RECT *)shield_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  if (0 < city_calc.shield_waste) {
    x2 = scale(2);
    x2 = mid_x + x2;
    for (i = 0; i < city_calc.shield_waste; i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.shield_shortage_icon_sprite))->draw((RECT *)waste_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  if (surplus != 0) {
    x2 = right_x;
    if (shields_shown < 1) {
      scaled = scale(1);
      x2 += scaled;
    }
    else {
      scaled = scale(4);
      x2 += (scaled >> 1);
    }
    i = 0;
    while( true ) {
      if ((int)surplus < 1) {
        shield_surplus_count = ~surplus + 1;
      }
      else {
        shield_surplus_count = surplus;
      }
      if ((int)shield_surplus_count <= i) break;
      if (cities[city_index].building == -IMP_CAPITALIZATION) {
        ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)coin_rect,this,x2,y2);
      }
      else {
        ((Sprite *)(((i8 *)&war_icon_sprite_block.shield_shortage_icon_sprite) + (uint)(0 < (int)surplus) * 0x3c))->draw(
                   (RECT *)shortage_rect,this,x2,y2);
      }
      x2 += spacing;
      i++;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0xe);
  scaled2 = scale(2);
  scaled3 = scale(0x78);
  scaled4 = scale(0xffffffff);
  y2 = scaled + scaled2 + scaled3 + scaled4 + panel_top2;
  text_clear();
  text_append_label(0xcc);
  text_append_colon();
  scaled = city_calc.shield_support;
  if (produced <= city_calc.shield_support) {
    scaled = produced;
  }
  text_append_int(scaled);
  text_set_colors(0x54,10,1,1);
  gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  if (city_calc.shield_waste != 0) {
    text_clear();
    text_append_label(0x43);
    text_append_colon();
    text_append_int(city_calc.shield_waste);
    text_set_colors(0xb,0x1d,1,1);
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  text_clear();
  if ((int)surplus < 0) {
    text_append_label(0x4b);
    text_set_colors(0xb,0x1d,1,1);
  }
  else {
    text_append_label(0x44);
    text_set_colors(0x5c,10,1,1);
  }
  text_append_colon();
  if ((int)surplus < 1) {
    shield_surplus_text = ~surplus + 1;
  }
  else {
    shield_surplus_text = surplus;
  }
  text_append_int(shield_surplus_text);
  gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
}

/* Part of CityWindow::draw_resource_map (005025d5): the trade bar: trade and
   corruption, and its labels; surplus is the shield surplus
   draw_resource_shields left (the original reads it here). */
void CityWindow::draw_resource_trade(int panel_left,int panel_top2,uint big_zoom,uint surplus)
{
  int scaled;
  int scaled2;
  uint abs_corruption;
  i8 corruption_rect[16];
  i8 trade_rect[16];
  int x2;
  int left_width;
  int fit_count;
  int text_end_x;
  int icon_w;
  int right_x;
  int label_x;
  int spacing;
  int right_width;
  int label_right;
  int total_icons;
  iptr y2;
  int produced;
  iptr i;
  int avail_width;
  int bar_x;

  produced = city_calc.total_trade;
  total_icons = city_calc.total_trade;
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  if ((*(uint *)&city_calc.trade_corruption) != 0) {
    scaled = scale(4);
    avail_width = (avail_width - scaled) - icon_w;
    total_icons--;
  }
  spacing = ui_calc_item_spacing(total_icons,icon_w,avail_width,&fit_count,0);
  for (; fit_count < total_icons; total_icons--) {
    produced--;
  }
  right_width = 0;
  if ((*(uint *)&city_calc.trade_corruption) != 0) {
    if ((int)(*(uint *)&city_calc.trade_corruption) < 1) {
      abs_corruption = ~(*(uint *)&city_calc.trade_corruption) + 1;
    }
    else {
      abs_corruption = (*(uint *)&city_calc.trade_corruption);
    }
    scaled = scale(2);
    scaled2 = scale(1);
    right_width = scaled + scaled2 + (abs_corruption - 1) * spacing + icon_w;
  }
  left_width = scale(0xe2);
  left_width -= right_width;
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  right_x = left_width + bar_x;
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0x37);
  y2 = panel_top2 + y2;
  if ((int)surplus < 0) {
    left_width += right_width;
    right_width = 0;
  }
  if (left_width != 0) {
    gfx_target_fill_rect(bar_x,y2,left_width,resource_bar_height,0x76);
  }
  if (right_width != 0) {
    gfx_target_fill_rect(right_x,y2,right_width,resource_bar_height,0x79);
  }
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < (int)(city_calc.total_trade - (*(uint *)&city_calc.trade_corruption))) {
    x2 = bar_x + 1;
    for (i = 0; i < (int)(city_calc.total_trade - (*(uint *)&city_calc.trade_corruption)); i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.trade_icon_sprite))->draw((RECT *)trade_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  if ((*(uint *)&city_calc.trade_corruption) != 0) {
    x2 = right_x;
    scaled = scale(4);
    x2 += (scaled >> 1);
    for (i = 0; i < (int)(*(uint *)&city_calc.trade_corruption); i++) {
      ((Sprite *)((i8 *)&war_icon_sprite_block.trade_corruption_icon_sprite))->draw((RECT *)corruption_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0x37);
  scaled = panel_top2 + scaled;
  y2 = scale(0xe);
  y2 = scaled - y2;
  text_clear();
  text_append_label(0x45);
  text_set_colors(0x76,10,1,1);
  text_append_colon();
  text_append_int(city_calc.total_trade - (*(uint *)&city_calc.trade_corruption));
  text_end_x = gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  if ((civs[cities[city_index].owner].government == GOV_FUNDAMENTALISM) &&
     (city_calc.building_content != 0)) {
    scaled = scale(4);
    text_end_x += scaled;
    text_clear();
    text_append_label(0x5d);
    text_append_colon();
    text_append((const char *)&str_empty_144);
    text_append_int(city_calc.building_content);
    text_set_colors(0x79,10,1,1);
    gfx_draw_text((const char *)&textbuf.buf,text_end_x,y2);
  }
  text_clear();
  text_append_label(0x46);
  text_set_colors(0x79,10,1,1);
  text_append_colon();
  text_append_int((*(uint *)&city_calc.trade_corruption));
  gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
}

/* Part of CityWindow::draw_resource_map (005025d5): the tax, luxury and science bar
   and its labels. */
void CityWindow::draw_resource_rates(int panel_left,int panel_top2,uint big_zoom)
{
  int scaled;
  int scaled2;
  int scaled3;
  i32 bar_height;
  int scaled4;
  i32 color;
  i32 fill_color;
  i8 science_rect[16];
  i8 lux_rect[16];
  i8 tax_rect[16];
  int x2;
  iptr civ;
  int left_width;
  int mid_width;
  int fit_count;
  int icon_w;
  int mid_x;
  int tax_icons;
  int lux_align;
  int right_x;
  int label_x;
  int spacing;
  int right_width;
  int lux_icons;
  int label_right;
  int total_icons;
  iptr y2;
  int produced;
  int sci_icons;
  iptr i;
  int lux_x;
  int avail_width;
  int nonzero_kinds;
  int bar_x;

  produced = city_calc.total_trade;
  tax_icons = city_calc.tax_total;
  lux_icons = city_calc.lux_total;
  sci_icons = city_calc.science_total;
  total_icons = city_calc.science_total + city_calc.lux_total + city_calc.tax_total;
  nonzero_kinds = (uint)(0 < city_calc.science_total) + (uint)(0 < city_calc.lux_total) + (uint)(0 < city_calc.tax_total);
  scaled = scale(0xe2);
  avail_width = scale(2);
  avail_width = scaled - avail_width;
  icon_w = scale(0xe);
  icon_w++;
  for (; 1 < nonzero_kinds; nonzero_kinds--) {
    scaled = scale(4);
    avail_width -= (icon_w + scaled);
    total_icons--;
  }
  spacing = ui_calc_item_spacing(total_icons,icon_w,avail_width,&fit_count,0);
  for (; fit_count < total_icons; fit_count++) {
    if ((lux_icons < tax_icons) && (sci_icons < tax_icons)) {
      tax_icons--;
    }
    else if (lux_icons < sci_icons) {
      sci_icons--;
    }
    else {
      lux_icons--;
    }
  }
  left_width = 0;
  bar_x = scale(0xcd);
  bar_x = panel_left + bar_x;
  if (tax_icons != 0) {
    scaled = scale(1);
    left_width = scaled + (tax_icons - 1) * spacing + icon_w;
  }
  right_width = 0;
  if (sci_icons != 0) {
    scaled = scale(1);
    right_width = scaled + (sci_icons - 1) * spacing + icon_w;
  }
  scaled = scale(0xe2);
  scaled2 = scale(0xcd);
  right_x = (scaled + scaled2 + panel_left) - right_width;
  mid_width = scale(0xe2);
  mid_width -= (right_width + left_width);
  lux_x = scale(0xcd);
  lux_x = panel_left + lux_x;
  if (left_width != 0) {
    scaled = scale(4);
    mid_width -= scaled;
    scaled = scale(4);
    lux_x = lux_x + left_width + scaled;
  }
  if (right_width != 0) {
    scaled = scale(4);
    mid_width -= scaled;
  }
  mid_x = lux_x;
  lux_x += (mid_width >> 1);
  if (lux_icons != 0) {
    lux_x -= ((lux_icons - 1) * spacing + icon_w >> 1);
  }
  lux_align = 0;
  if ((lux_icons == 0) || (tax_icons != 0)) {
    if ((lux_icons != 0) && (sci_icons == 0)) {
      lux_align = 2;
      scaled = scale(1);
      nonzero_kinds = scaled + (lux_icons - 1) * spacing + icon_w;
      scaled = scale(0xe2);
      scaled2 = scale(0xcd);
      lux_x = (scaled + scaled2 + panel_left) - nonzero_kinds;
    }
  }
  else {
    lux_align = 1;
    lux_x = bar_x;
  }
  scaled = scale(0xe);
  scaled2 = scale(2);
  resource_bar_height = scaled + scaled2;
  y2 = scale(0x4f);
  y2 = panel_top2 + y2;
  fill_color = 0x79;
  bar_height = resource_bar_height;
  color = scale(0xe2);
  gfx_target_fill_rect(bar_x,y2,color,bar_height,fill_color);
  scaled = scale(1);
  y2 = y2 + big_zoom + scaled;
  if (0 < tax_icons) {
    x2 = scale(1);
    x2 = bar_x + x2;
    for (i = 0; i < tax_icons; i++) {
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)tax_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  if (lux_icons != 0) {
    x2 = lux_x;
    for (i = 0; i < lux_icons; i++) {
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites))->draw((RECT *)lux_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  if (sci_icons != 0) {
    x2 = right_x;
    for (i = 0; i < sci_icons; i++) {
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.science_icon_sprite))->draw((RECT *)science_rect,this,x2,y2);
      x2 += spacing;
    }
  }
  label_x = scale(0xcd);
  label_x = panel_left + label_x;
  scaled = scale(0xe2);
  label_right = label_x + scaled - 1;
  scaled = scale(0x4f);
  scaled2 = scale(0xe);
  scaled3 = scale(2);
  scaled4 = scale(0);
  y2 = scaled + scaled2 + scaled3 + scaled4 + panel_top2;
  civ = (iptr)cities[city_index].owner;
  text_clear();
  text_append_int((uint)civs[civ].tax_rate * 10);
  text_append_percent();
  text_append_space();
  text_append_label(0x47);
  text_append_colon();
  text_append_int(city_calc.tax_total);
  text_set_colors(0x76,10,1,1);
  if (lux_align == 1) {
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  else {
    gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  }
  text_clear();
  text_append_int((10 - ((uint)civs[civ].tax_rate + (uint)civs[civ].science_rate)) *
                     10);
  text_append_percent();
  text_append_space();
  text_append_label(0x48);
  text_append_colon();
  text_append_int(city_calc.lux_total);
  text_set_colors(0x29,10,1,1);
  if (lux_align == 1) {
    gfx_draw_text((const char *)&textbuf.buf,label_x,y2);
  }
  else if (lux_align == 2) {
    gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
  }
  else {
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  text_clear();
  text_append_int((uint)civs[civ].science_rate * 10);
  text_append_percent();
  text_append_space();
  text_append_label(0x49);
  text_append_colon();
  text_append_int(city_calc.science_total);
  text_set_colors(0x5e,10,1,1);
  if (lux_align == 2) {
    bar_height = scale(0xe2);
    gfx_draw_text_centered((const char *)&textbuf.buf,label_x,y2,bar_height);
  }
  else {
    gfx_draw_text_right((const char *)&textbuf.buf,label_right,y2,0);
  }
}

/* 005025d5 auto */
void CityWindow::draw_resource_map(int blit)

{
  u32 text_bottom;
  int scaled;
  i32 bar_height;
  int tile_h;
  int tile_w;
  int origin_x2;
  int map_top;
  int panel_top2;
  int panel_left;
  int origin_y2;
  int half_tile_w;
  int map_left;
  int half_tile_h;
  uint big_zoom;
  i8 map_rect[16];
  uint surplus;
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw(((RECT *)&resource_map_rect));
    panel_left = resource_map_rect;
    panel_top2 = resource_map_rect_top;
    tile_w = gfx_zoom_scale(0x40,map_zoom_level);
    tile_h = gfx_zoom_scale(0x20,map_zoom_level);
    half_tile_w = tile_w >> 1;
    half_tile_h = tile_h >> 1;
    map_left = scale(5);
    map_left = panel_left + map_left;
    map_top = scale(0xb);
    map_top = panel_top2 + map_top;
    clickmap.remove_by_id(1);
    rect_set_xywh((RECT *)map_rect,map_left,half_tile_h + map_top,tile_w << 2,tile_h << 2);
    clickmap.hotspot_add_rect(0,1,(RECT *)map_rect);
    origin_x2 = half_tile_w * 3 + map_left;
    origin_y2 = half_tile_h * 4 + map_top;
    draw_resource_tiles(half_tile_w,half_tile_h,origin_x2,origin_y2);
    draw_resource_yields(tile_w,half_tile_w,half_tile_h,origin_x2,origin_y2);
    text_set_font(&city_font);
    gfx_set_zoom_scale(view_scale * 4 - 8);
    text_clear();
    text_append_label(0x1c2);
    text_set_colors(0x7c,0x39,1,1);
    bar_height = rect_width((RECT *)&resource_label_rect);
    text_bottom = resource_label_rect_bottom;
    scaled = scale(0xe);
    gfx_draw_text_centered((const char *)&textbuf.buf,resource_label_rect,text_bottom - scaled,bar_height);
    text_set_colors(0x29,10,1,1);
    big_zoom = (uint)(view_scale == 3);
    draw_resource_food(panel_left,panel_top2,big_zoom);
    draw_resource_shields(panel_left,panel_top2,big_zoom,surplus);
    draw_resource_trade(panel_left,panel_top2,big_zoom,surplus);
    draw_resource_rates(panel_left,panel_top2,big_zoom);
    gfx_reset_zoom_scale();
    if (blit != 0) {
      wnd_blit_surface_rect(((RECT *)&resource_map_rect));
    }
  }
  return;
}

/* 00504c05 auto */
void CityWindow::draw_food_box(int blit)

{
  i32 width;
  iptr box_w;
  iptr box_h;
  i32 arg_y;
  i32 arg_x;
  int icon_x;
  int row_spacing;
  int extent;
  int row_y;
  int food_left;
  int row;
  RECT box_rect;
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw(((RECT *)&food_box_rect));
    text_set_colors(0x2c,10,1,1);
    width = rect_width((RECT *)(iptr)(&food_box_rect));
    arg_y = food_box_rect_top;
    arg_x = food_box_rect;
    box_w = (iptr)label_string(label_table.string_ids[0x2b]);
    gfx_draw_text_centered((const char *)box_w,arg_x,arg_y,width);
    box_w = scale(0xb7);
    box_h = scale(0x93);
    arg_y = scale(0xf);
    arg_x = scale(6);
    rect_set_xywh(&box_rect,arg_x,arg_y,box_w,box_h);
    rect_offset(&box_rect,food_box_rect,
                       food_box_rect_top);
    arg_y = scale(3);
    arg_x = scale(3);
    rect_deflate(&box_rect,arg_x,arg_y);
    box_h = 0;
    box_w = 0;
    arg_y = rect_width(&box_rect);
    arg_x = scale(0x11);
    icon_x = ui_calc_item_spacing(cities[city_index].size + 1,arg_x,arg_y,(int *)box_w
                               ,(int *)box_h);
    box_h = 0;
    box_w = 0;
    arg_y = rect_height(&box_rect);
    arg_x = scale(0xe);
    row_spacing = ui_calc_item_spacing(city_calc.food_box_rows,arg_x,arg_y,(int *)box_w,(int *)box_h);
    icon_x = (cities[city_index].size + 1) * icon_x;
    extent = rect_width(&box_rect);
    if (icon_x < extent) {
      arg_y = 0;
      extent = rect_width(&box_rect);
      rect_deflate(&box_rect,extent - icon_x >> 1,arg_y);
    }
    icon_x = row_spacing * city_calc.food_box_rows;
    extent = rect_height(&box_rect);
    if (icon_x < extent) {
      extent = rect_height(&box_rect);
      box_rect.bottom -= (extent - icon_x);
    }
    rect_inflate(&box_rect,3,3);
    city_draw_bevel_rect(&box_rect,0x2c,0x39);
    rect_deflate(&box_rect,1,1);
    rect_deflate(&box_rect,2,2);
    icon_x = box_rect.left;
    row = 0;
    food_left = (int)cities[city_index].food_box;
    row_y = box_rect.top;
    gfx_set_zoom_scale(view_scale * 4 - 8);
    for (; (0 < food_left && (row < city_calc.food_box_rows)); row++) {
      extent = cities[city_index].size + 1;
      if (food_left <= extent) {
        extent = food_left;
      }
      width = 1;
      arg_y = rect_width(&box_rect);
      arg_x = scale(0x11);
      city_window_draw_icon_row(&war_icon_sprite_block.resource_icon_sprites,icon_x,row_y,extent,
                         cities[city_index].size + 1,arg_x,arg_y,width);
      food_left -= extent;
      row_y += row_spacing;
    }
    gfx_reset_zoom_scale();
    icon_x = city_has_improvement(city_index,IMP_GRANARY);
    if ((icon_x != 0) ||
       (icon_x = civ_has_wonder(cities[city_index].owner,WONDER_PYRAMIDS),
       icon_x != 0)) {
      extent = city_calc.food_box_rows >> 1;
      icon_x = scale(1);
      row_spacing = box_rect.top + (extent * row_spacing - icon_x);
      arg_y = 0x2a;
      icon_x = scale(2);
      icon_x = (box_rect.right - 1) - icon_x;
      extent = scale(2);
      city_draw_hline(box_rect.left + extent,icon_x,row_spacing,arg_y);
    }
    if (blit != 0) {
      wnd_blit_surface_rect(((RECT *)&food_box_rect));
    }
  }
  return;
}

/* 0050503e auto */
void CityWindow::draw_production(int blit)

{
  int scaled;
  int scaled_x;
  i32 arg_y;
  iptr arg_w;
  int *fit_out;
  i32 arg_x;
  iptr arg_h;
  i32 width;
  i8 icon_rect[16];
  int row_spacing;
  int fit_count;
  int col_spacing;
  iptr box_h;
  iptr unit;
  i32 zoom2;
  iptr improvement;
  int pad_x2;
  iptr box_w;
  iptr unit_type;
  uint build_cost;
  int row_y;
  uint per_row;
  i32 row_x;
  uint shields_left;
  uint row_icons;
  int row;
  int row_width;
  struct { i32 left; int top; i32 right; u32 bottom; } box_rect;
  
  improvement = -1;
  unit_type = -1;
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw(((RECT *)&production_rect));
    box_w = scale(0xb7);
    box_h = scale(0x92);
    box_rect.left = production_rect;
    box_rect.top = production_rect_top;
    box_rect.right = production_rect_right;
    box_rect.bottom = production_rect_bottom;
    scaled = scale(0x28);
    box_rect.bottom = box_rect.top + scaled;
    clickmap.remove_by_id(5);
    clickmap.hotspot_add_rect(0,5,(RECT *)&box_rect.left);
    if (cities[city_index].building < '\0') {
      if (cities[city_index].building < 1) {
        improvement = ~(int)cities[city_index].building + 1;
      }
      else {
        improvement = (iptr)cities[city_index].building;
      }
      build_cost = (uint)improvements[improvement].cost;
      text_set_colors(0x54,10,1,1);
      width = rect_width((RECT *)(iptr)(&production_rect));
      arg_y = production_rect_top;
      arg_x = production_rect;
      arg_w = (iptr)label_string(improvements[improvement].name);
      gfx_draw_text_centered((const char *)arg_w,arg_x,arg_y,width);
      gfx_set_zoom_scale(view_scale * 4 - 8);
      scaled = scale(0x10);
      scaled = production_rect_top + scaled;
      scaled_x = scale(0x50);
      ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((RECT *)icon_rect,&city_wnd.window,
                 production_rect + scaled_x,scaled);
      gfx_reset_zoom_scale();
    }
    else {
      unit_type = (iptr)cities[city_index].building;
      build_cost = (uint)(char)unit_types[unit_type].cost;
      unit = unit_make_dummy(unit_type,cities[city_index].owner);
      zoom2 = 0xffffffff;
      if (view_scale == 1) {
        zoom2 = 0xfffffffb;
      }
      if (view_scale == 3) {
        zoom2 = 1;
      }
      arg_x = 0;
      arg_y = zoom2;
      scaled = scale(1);
      scaled = production_rect_top + scaled;
      scaled_x = scale(0x49);
      gfx_draw_unit(&city_wnd.window,unit,0,production_rect + scaled_x,scaled,arg_y,
                         arg_x);
    }
    if (cities[city_index].building != -IMP_CAPITALIZATION) {
      arg_w = box_w;
      arg_h = box_h;
      arg_y = scale(0x28);
      arg_x = scale(6);
      rect_set_xywh((RECT *)&box_rect.left,arg_x,arg_y,arg_w,arg_h);
      rect_offset((RECT *)&box_rect.left,production_rect,
                         production_rect_top);
      arg_y = scale(3);
      arg_x = scale(3);
      rect_deflate((RECT *)&box_rect.left,arg_x,arg_y);
      arg_h = 0;
      arg_w = 0;
      arg_y = rect_height((RECT *)&box_rect.left);
      arg_x = scale(0xe);
      row_spacing = ui_calc_item_spacing(10,arg_x,arg_y,(int *)arg_w,(int *)arg_h);
      per_row = build_cost;
      if ((int)build_cost < 10) {
        per_row = (*(uint *)&city_calc.shield_rows);
        box_rect.bottom -= (10 - build_cost) * row_spacing;
      }
      arg_w = 0;
      fit_out = &fit_count;
      arg_y = rect_width((RECT *)&box_rect.left);
      arg_x = scale(0x11);
      col_spacing = ui_calc_item_spacing((*(uint *)&city_calc.shield_rows),arg_x,arg_y,fit_out,(int *)arg_w);
      row_width = col_spacing * (*(uint *)&city_calc.shield_rows) + fit_count;
      pad_x2 = 0;
      scaled = rect_width((RECT *)&box_rect.left);
      if (row_width < scaled) {
        scaled = rect_width((RECT *)&box_rect.left);
        pad_x2 = scaled - row_width >> 1;
      }
      rect_inflate((RECT *)&box_rect.left,3,3);
      city_draw_bevel_rect((RECT *)&box_rect.left,0x51,0x5d);
      rect_deflate((RECT *)&box_rect.left,1,1);
      rect_deflate((RECT *)&box_rect.left,pad_x2 + 2,2);
      row = 0;
      shields_left = (uint)cities[city_index].shield_box;
      row_x = box_rect.left;
      row_y = box_rect.top;
      gfx_set_zoom_scale(view_scale * 4 - 8);
      for (; (0 < (int)shields_left && (row < 10)); row++) {
        row_icons = shields_left;
        if ((int)per_row <= (int)shields_left) {
          row_icons = per_row;
        }
        width = 1;
        arg_y = rect_width((RECT *)&box_rect.left);
        arg_x = scale(0x11);
        city_window_draw_icon_row(&war_icon_sprite_block.shield_icon_sprite,row_x,row_y,row_icons,per_row,arg_x,arg_y,width);
        shields_left -= row_icons;
        row_y += row_spacing;
      }
      gfx_reset_zoom_scale();
    }
    if (blit != 0) {
      wnd_blit_surface_rect(((RECT *)&production_rect));
    }
  }
  return;
}

/* 005055dd auto */
void CityWindow::draw_button_area(int blit)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) &&
     ((-1 < city_index &&
      (begin_draw((RECT *)button_area_rect), blit != 0)))) {
    wnd_blit_surface_rect((RECT *)button_area_rect);
  }
  return;
}

/* 00505666 auto */
void CityWindow::draw_supported_units(int blit)

{
  int scaled;
  int used;
  int pad;
  i32 width;
  iptr size_arg;
  iptr zero_a;
  iptr zero_b;
  i8 suppressed_rect[16];
  i8 unhappy_rect[16];
  i8 shield_rect[16];
  i8 food_rect[16];
  int icon_x;
  iptr civ;
  int cell_h;
  int cell_y;
  iptr unit;
  int cell_x;
  int upkeep_icons;
  uint gov_type;
  int col;
  int margin_x;
  int margin_y;
  u32 area_top;
  int icon_spacing;
  u32 area_left;
  int cell_w;
  int unhappy_cost;
  int unit_count;
  int icon_offset_y;
  int icon_y;
  int row;
  int military_away;
  int unhappy_kind;
  int max_rows;
  int i;
  i32 zoom2;
  iptr unit_size;
  int max_cols;
  i8 hotspot_rect[16];
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index) &&
      (extra_ui(X_SUPPORT_LIST) || extra_ui(X_SUPPORT_SORT))) {
    extras_draw_supported_units(this,blit);   /* X11, X16 */
    return;
  }
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((RECT *)&supported_units_rect);
    area_left = supported_units_rect;
    area_top = supported_units_rect_top;
    zoom2 = 0xfffffffd;
    if (view_scale == 1) {
      zoom2 = 0xfffffffa;
    }
    if (view_scale == 3) {
      zoom2 = 0;
    }
    cell_w = gfx_zoom_scale(0x45,zoom2);
    cell_h = gfx_zoom_scale(0x34,zoom2);
    max_cols = scale(0xc0);
    max_cols = max_cols / cell_w;
    max_rows = scale(0x4e);
    max_rows = max_rows / cell_h;
    scaled = scale(0xc0);
    used = cell_w * max_cols;
    pad = gfx_zoom_scale(5,zoom2);
    margin_x = (scaled - used) + pad >> 1;
    scaled = scale(0x4e);
    used = cell_h * max_rows;
    pad = gfx_zoom_scale(4,zoom2);
    margin_y = (scaled - used) + pad >> 1;
    icon_offset_y = gfx_zoom_scale(0x20,zoom2);
    unit_count = 0;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) &&
         ((uint)(byte)units[unit].home_city == (uint)city_index)) {
        unit_count++;
      }
    }
    if (unit_count <= max_cols) {
      scaled = scale(0x4e);
      used = gfx_zoom_scale(0x30,zoom2);
      margin_y = scaled - used >> 1;
      text_clear();
      text_append_label(0x1bf);
      text_set_colors(0x7c,0x12,1,1);
      width = rect_width((RECT *)(iptr)((u8 *)&supported_units_label_rect));
      scaled = scale(1);
      gfx_draw_text_centered((const char *)&textbuf.buf,supported_units_label_rect,
                         supported_units_label_rect_top + scaled,width);
    }
    unit_size = gfx_zoom_scale(0x40,zoom2);
    cell_x = margin_x + area_left;
    cell_y = margin_y + area_top;
    row = 0;
    col = 0;
    civ = (iptr)cities[city_index].owner;
    gov_type = (uint)civs[civ].government;
    military_away = 0;
    clickmap.remove_by_id(6);
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) &&
         ((uint)(byte)units[unit].home_city == (uint)city_index)) {
        icon_y = icon_offset_y + cell_y;
        gfx_draw_unit(this,unit,4,cell_x,cell_y,zoom2,0);
        upkeep_icons = 0;
        unhappy_kind = 0;
        if (unit_types[units[unit].type].role == ROLE_SETTLE) {
          upkeep_icons = city_calc.settler_food_cost;
        }
        if ((units[unit].flags & UNITF_COSTS_SHIELD) != 0) {
          upkeep_icons++;
        }
        if (((units[unit].flags & UNITF_AGGRESSIVE) != 0) && (GOV_FUNDAMENTALISM < (int)gov_type)) {
          scaled = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
          if ((scaled == 0) &&
             (scaled = city_has_improvement(city_index,IMP_POLICE_STATION), scaled == 0)) {
            unhappy_cost = 1;
          }
          else {
            unhappy_cost = 0;
          }
          if (gov_type == GOV_DEMOCRACY) {
            unhappy_cost++;
          }
          else if ((unhappy_cost != 0) && (military_away == 0)) {
            unhappy_cost = 0;
          }
          if (unhappy_cost == 0) {
            unhappy_cost = 1;
            unhappy_kind = 2;
          }
          else {
            unhappy_kind = 1;
          }
          upkeep_icons += unhappy_cost;
          military_away++;
        }
        gfx_set_zoom_scale(view_scale * 4 - 8);
        zero_b = 0;
        zero_a = 0;
        size_arg = unit_size;
        width = scale(10);
        icon_spacing = ui_calc_item_spacing(upkeep_icons,width,size_arg,(int *)zero_a,(int *)zero_b);
        icon_x = cell_x;
        if (unit_types[units[unit].type].role == ROLE_SETTLE) {
          for (i = 0; i < city_calc.settler_food_cost; i++) {
            ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites))->draw((RECT *)food_rect,this,icon_x,icon_y);
            icon_x += icon_spacing;
          }
        }
        if ((units[unit].flags & UNITF_COSTS_SHIELD) != 0) {
          ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_shield_icon_sprite))->draw((RECT *)shield_rect,this,icon_x,icon_y);
          icon_x += icon_spacing;
        }
        if (unhappy_kind != 0) {
          for (i = 0; i < unhappy_cost; i++) {
            if (unhappy_kind == 1) {
              ((Sprite *)((i8 *)&small_unhappy_icon_sprite))->draw((RECT *)unhappy_rect,this,icon_x,icon_y)
              ;
            }
            else {
              ((Sprite *)((i8 *)&unhappy_suppressed_icon_sprite))->draw((RECT *)suppressed_rect,this,icon_x,icon_y)
              ;
            }
            icon_x += icon_spacing;
          }
        }
        gfx_reset_zoom_scale();
        rect_set_xywh((RECT *)hotspot_rect,cell_x,cell_y,unit_size,unit_size);
        clickmap.hotspot_add_rect(unit,6,(RECT *)hotspot_rect);
        col++;
        cell_x += cell_w;
        if (max_cols <= col) {
          col = 0;
          cell_x = margin_x + area_left;
          row++;
          cell_y += cell_h;
          if (max_rows <= row) break;
        }
      }
    }
    if (blit != 0) {
      wnd_blit_surface_rect((RECT *)&supported_units_rect);
    }
  }
  return;
}

/* 00505d3d auto */
void __cdecl city_window_sell_improvement(i32 list_row)

{
  short list_top;
  iptr city3;
  iptr city4;
  iptr city2;
  int city;
  int sale_price;
  i32 on;
  iptr imp_arg;
  iptr imp_idx;
  iptr improvement;
  int owned_count;
  
  owned_count = 0;
  improvement = -1;
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    if (((cities[city4].flags & CITYF_IMPROVEMENT_SOLD) == 0) ||
       ((game.reveal_map != '\0' && ((game.game_flags & GAME_SCENARIO) != 0)))) {
      on = 0;
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city2,on);
      list_top = (short)city_wnd.improvement_list_top;
      for (imp_idx = 1; imp_idx < 0x27; imp_idx++) {
        imp_arg = imp_idx;
        city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        city = city_has_improvement(city2,imp_arg);
        if ((city != 0) && (owned_count = owned_count + 1, (short)(list_top + (short)list_row + 1) == owned_count))
        {
          improvement = imp_idx;
          break;
        }
      }
      if (-1 < improvement) {
        if ((imp_idx == 1) && ((game.reveal_map == '\0' || ((game.game_flags & GAME_SCENARIO) == 0)))) {
          snd_play_effect(SND_NEG1,0,0,0);
          popup_game_with_improvement((const char *)&str_canthockthis,IMP_PALACE,8);
        }
        else {
          sale_price = (uint)improvements[improvement].cost * (uint)cosmic.shield_box_rows;
          text_set_label_arg(0,improvements[improvement].name);
          text_set_number_arg(0,sale_price);
          city = popup_game_with_improvement((const char *)&str_hockthis,improvement,8);
          if (city == 0) {
            snd_play_effect(SND_SELL,0,0,0);
            on = 0;
            city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_set_improvement(city2,improvement,on);
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            civs[cities[city].owner].gold = civs[cities[city].owner].gold + sale_price;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            cities[city].flags = cities[city].flags | CITYF_IMPROVEMENT_SOLD;
            ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
            status_draw_info(1);
            advisor_refresh_if_open(5);
            advisor_refresh_if_open(4);
            advisor_refresh_if_open(6);
          }
        }
      }
    }
    else {
      snd_play_effect(SND_NEG1,0,0,0);
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      popup_game_with_city((const char *)&str_alreadysold,city2);
    }
  }
  return;
}

/* 00505ffa auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void CityWindow::draw_improvements(int blit)

{
  u32 list_right;
  i32 width;
  int scaled;
  int scaled2;
  iptr row_h;
  i8 coin_rect[16];
  i8 imp_rect[16];
  i8 wonder_rect[16];
  int has_it;
  int row_y;
  iptr improvement;
  int row_x;
  int shown_count;
  uptr list_top;
  u32 list_left;
  int name_y;
  int text_x2;
  int wonder_idx;
  int visible_rows;
  int row;
  int zoom2;
  i8 row_rect[16];
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((RECT *)(u8 *)&improvements_rect);
    clickmap.remove_by_id(4);
    list_left = improvement_list_rect;
    list_top = improvement_list_rect_top;
    text_clear();
    text_append_label(0x1c0);
    text_set_colors(0x7c,0x12,1,1);
    width = rect_width((RECT *)(iptr)(u8 *)&improvements_rect);
    scaled2 = improvements_rect_top;
    scaled = scale(1);
    gfx_draw_text_centered((const char *)&textbuf.buf,*(i32 *)(u8 *)&improvements_rect,scaled2 - scaled,width);
    row_x = scale(2);
    row_x = list_left + row_x;
    row_y = scale(1);
    row_y = list_top + row_y;
    scaled2 = rect_height((RECT *)&improvement_list_rect);
    scaled = scale(1);
    visible_rows = scale(0xc);
    visible_rows = (scaled2 - (scaled >> 1)) / visible_rows;
    zoom2 = view_scale * 2 - 8;
    for (row = 0; row < visible_rows; row++) {
      row_h = scale(0xc);
      scaled2 = scale(0xc0);
      scaled = scale(4);
      rect_set_xywh((RECT *)row_rect,row_x,row_y,scaled2 - scaled,row_h);
      clickmap.hotspot_add_rect(row,4,(RECT *)row_rect);
      scaled2 = scale(0xc);
      row_y += scaled2;
    }
    row_y = list_top + 1;
    shown_count = 0;
    for (improvement = 0; improvement < 0x43; improvement++) {
      if (improvement < 0x27) {
        has_it = city_has_improvement(city_index,improvement);
        wonder_idx = -1;
      }
      else {
        wonder_idx = improvement - 0x27;
        if ((int)game.wonder_city[improvement - 0x27] == city_index) {
          has_it = 1;
        }
        else {
          has_it = 0;
        }
      }
      if (((has_it != 0) &&
          (shown_count = shown_count + 1, improvement_scroll_pos + 1 <= shown_count)) &&
         (shown_count <= improvement_scroll_pos + visible_rows)) {
        gfx_set_zoom_scale(zoom2);
        if (wonder_idx < 0) {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((RECT *)imp_rect,this,
                     row_x,row_y);
        }
        else {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder_idx * 0x3c))->draw((RECT *)wonder_rect,this,
                     row_x,row_y);
        }
        gfx_reset_zoom_scale();
        text_x2 = scale(0x18);
        text_x2 = row_x + text_x2;
        name_y = row_y - 1;
        text_clear();
        text_append_string_id(improvements[improvement].name);
        text_set_colors(0x29,10,1,0);
        gfx_draw_text((const char *)&textbuf.buf,text_x2,name_y - 1);
        list_right = improvement_list_rect_right;
        scaled2 = scale(0xe);
        text_x2 = scale(4);
        text_x2 = (list_right - scaled2) - text_x2;
        if (wonder_idx < 0) {
          gfx_set_zoom_scale(view_scale * 3 - 7);
          ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)coin_rect,this,text_x2,name_y);
          gfx_reset_zoom_scale();
        }
        scaled2 = scale(0xc);
        row_y += scaled2;
      }
    }
    city_wnd.improvement_list_count = shown_count;
    improvements_scrollbar->set_range(0,shown_count - 1);
    improvements_scrollbar->set_page_size(visible_rows - 1);
    improvements_scrollbar->set_pos(improvement_scroll_pos);
    if (blit != 0) {
      wnd_blit_surface_rect((RECT *)(u8 *)&improvements_rect);
    }
  }
  return;
}

/* 00506637 auto */
void __cdecl city_window_supported_unit_menu(int unit)

{
  iptr city3;
  iptr city2;
  iptr choice;
  int city;
  int window_city;
  iptr option_id;
  i32 arg;
  Sprite sprite;
  
  Dialog dialog2(0x4000);
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  if (city_wnd_state.unit_dialog_busy == 0) {
    city_wnd_state.unit_dialog_busy = 1;
    city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    if ((cities[city3].owner == human_civ[0]) || (game.reveal_map != '\0')) {
      arg = 0;
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city2,arg);
      dlg_unit_sprite_zoom = 8;
      sprite.create_solid(0x40,0x20,0);
      text_set_unit_info_args(unit);
      dialog2.load_game_section((const char *)&str_childclick,1);
      dialog2.set_sprite_draw_fn((DialogSpriteDrawFn)unit_dlg_draw_sprite_cb);
      dialog2.set_image_zoom(8);
      dialog2.add_sprite(&sprite,unit,0);
      arg = 0;
      option_id = 4;
      city2 = (iptr)label_string(label_table.string_ids[0x132]);
      dialog2.add_option((const char *)city2,option_id,arg);
      choice = dialog2.show_modal(0);
      if (0 < choice) {
        if (choice == 3) {
          units[unit].orders = ORDER_GOTO;
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          units[unit].goto_x = cities[city].x;
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          units[unit].goto_y = cities[city].y;
          if (bugfix_state()) {
            units[unit].aux_timer = '\0';  /* B9: the long move count starts again (docs/BUGS.md) */
          }
        }
        else if (choice == 4) {
          city = city_at((int)units[unit].x,units[unit].y);
          if (-1 < city) {
            cities[city].shield_box =
                 (short)((int)((int)(char)unit_types[units[unit].type].cost *
                              (uint)cosmic.shield_box_rows) / 2) + cities[city].shield_box;
            window_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            if (window_city == city) {
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_save_build_state(city2);
            }
          }
          unit_destroy_redraw(unit,1);
          ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
        }
        else {
          city = map_in_bounds(units[unit].x,units[unit].y);
          if (city != 0) {
            city = unit_is_selectable(unit);
            if (city == 0) {
              ui_redraw_all_maps(0);
              map_set_cursor_pos(units[unit].x,units[unit].y);
              ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)units[unit].x,(int)units[unit].y
                        );
            }
            else {
              game.active_unit = (i16)unit;
              units[unit].orders = ORDER_NONE;
              moving_units_mode = 0;
              ui_focus_active_unit(0);
            }
            if (choice == 2) {
              city_window_close();
            }
          }
        }
      }
    }
    city_wnd_state.unit_dialog_busy = 0;
  }
  return;
}

/* 00506a42 auto */
void __cdecl city_window_present_unit_menu(int stack_idx)

{
  iptr city3;
  iptr city;
  iptr nth;
  iptr city4;
  iptr y;
  iptr city5;
  int unit;
  uint city6;
  int city2;
  iptr option_id;
  i32 unit2;
  Sprite sprite;
  
  Dialog dialog2(0x4000);
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  if (city_wnd_state.unit_dialog_busy != 0) {
    return;
  }
  city_wnd_state.unit_dialog_busy = 1;
  ON_SCOPE_EXIT(city_wnd_state.unit_dialog_busy = 0);
  sprite.create_solid(0x40,0x20,0);
  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cities[city3].owner != human_civ[0]) && (game.reveal_map == '\0')) return;
  unit2 = 0;
  city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  city_recalc(city,unit2);
  if (city_wnd.info_panel_mode != 0) return;
  nth = (iptr)(short)stack_idx;
  city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  y = (iptr)cities[city4].y;
  city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  unit2 = tile_first_unit((int)cities[city5].x,y);
  unit = unit_stack_nth(unit2,nth);
  if (unit < 0) return;
  text_set_unit_info_args(unit);
  dialog2.load_game_section((const char *)&str_unitoptions,1);
  dlg_unit_sprite_zoom = 8;
  dialog2.set_sprite_draw_fn((DialogSpriteDrawFn)unit_dlg_draw_sprite_cb);
  dialog2.set_image_zoom(8);
  dialog2.add_sprite(&sprite,unit,0);
  unit2 = 0;
  option_id = 0;
  city = (iptr)label_string(label_table.string_ids[0x51]);
  dialog2.add_option((const char *)city,option_id,unit2);
  if (units[unit].orders != ORDER_NONE) {
    unit2 = 0;
    option_id = 1;
    city = (iptr)label_string(label_table.string_ids[0x52]);
    dialog2.add_option((const char *)city,option_id,unit2);
    dialog2.set_default_choice(1);
  }
  if (units[unit].orders != ORDER_SLEEP) {
    unit2 = 0;
    option_id = 2;
    city = (iptr)label_string(label_table.string_ids[0x53]);
    dialog2.add_option((const char *)city,option_id,unit2);
  }
  if ((units[unit].orders != ORDER_FORTIFYING) && (units[unit].orders != ORDER_FORTIFIED)) {
    unit2 = 0;
    option_id = 3;
    city = (iptr)label_string(label_table.string_ids[0x54]);
    dialog2.add_option((const char *)city,option_id,unit2);
  }
  city6 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (city6 != (byte)units[unit].home_city) {
    unit2 = 0;
    option_id = 5;
    city = (iptr)label_string(label_table.string_ids[0xb3]);
    dialog2.add_option((const char *)city,option_id,unit2);
    if (units[unit].home_city != -1) {
      unit2 = 0;
      option_id = 6;
      city = (iptr)label_string(label_table.string_ids[0xbd]);
      dialog2.add_option((const char *)city,option_id,unit2);
    }
  }
  unit2 = 0;
  option_id = 7;
  city = (iptr)label_string(label_table.string_ids[0xb4]);
  dialog2.add_option((const char *)city,option_id,unit2);
  if (((((uint)game.human_players & 1 << (units[unit].owner & 0x1fU)) != 0) &&
      (game.turn_civ != '\0')) && (city2 = unit_moves_left(unit), city2 != 0)) {
    unit2 = 0;
    option_id = 8;
    city = (iptr)label_string(label_table.string_ids[0xbe]);
    dialog2.add_option((const char *)city,option_id,unit2);
    unit2 = 0;
    option_id = 9;
    city = (iptr)label_string(label_table.string_ids[0xbf]);
    dialog2.add_option((const char *)city,option_id,unit2);
  }
  unit2 = dialog2.show_modal(0);
  switch(unit2) {
  case 1:
    units[unit].orders = ORDER_NONE;
    break;
  case 2:
    unit_set_sleep(unit);
    break;
  case 3:
    units[unit].orders = ORDER_FORTIFYING;
    unit_exhaust_moves(unit);
    break;
  case 4:
    unit_relocate_in_place(unit,1);
    break;
  case 5:
    unit2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    units[unit].home_city = (i8)unit2;
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    return;
  case 6:
    ((CityWindow *)((i8 *)&city_wnd.window))->open((uint)(byte)units[unit].home_city);
    return;
  case 7:
    city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    cities[city2].shield_box =
         (short)((int)((int)(char)unit_types[units[unit].type].cost * (uint)cosmic.shield_box_rows)
                / 2) + cities[city2].shield_box;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_save_build_state(city);
    unit_destroy_redraw(unit,1);
    ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
    return;
  case 8:
    game.active_unit = (i16)unit;
    units[unit].orders = ORDER_NONE;
    moving_units_mode = 0;
    ui_focus_active_unit(0);
    break;
  case 9:
    game.active_unit = (i16)unit;
    units[unit].orders = ORDER_NONE;
    moving_units_mode = 0;
    ui_focus_active_unit(0);
    city_window_close();
    return;
  }
  unit2 = 0;
  city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  city_recalc(city,unit2);
  ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
  return;
}

/* 005070e5 auto */
void CityWindow::draw_units_present()

{
  int scaled;
  i32 width;
  int row_width;
  int row_y;
  int pad_x;
  int max_icons;
  i32 flag;
  iptr demand_good;
  iptr supply_good;
  iptr cell_h;
  i8 food_icon_rect[16];
  i8 trade_icon_rect[16];
  iptr unit;
  int cell_y;
  int cell_x;
  iptr cell_w;
  int col;
  int row_h;
  uint is_negative;
  int stack_count;
  u32 area_top;
  u32 area_left;
  int per_row;
  int icon_x;
  int saved_per_row;
  int row_start_x;
  int slot;
  int slot_count2;
  int row;
  int label_y;
  iptr i;
  i32 zoom2;
  int unit_h;
  int line_h;
  int offset_y;
  i8 hotspot_rect[16];
  int col_w;
  int margin_x;
  
  area_left = info_panel_rect;
  area_top = info_panel_rect_top;
  zoom2 = map_zoom_level;
  cell_w = gfx_zoom_scale(0x40,zoom2);
  unit_h = gfx_zoom_scale(0x30,zoom2);
  col_w = gfx_zoom_scale(0x40,zoom2);
  row_h = gfx_zoom_scale(0x34,zoom2);
  per_row = scale(0xf4);
  per_row = per_row / col_w;
  scaled = scale(0xf4);
  margin_x = (scaled - col_w * per_row) / 2;
  offset_y = gfx_zoom_scale(4,zoom2);
  slot_count2 = per_row << 2;
  clickmap.remove_by_id(3);
  unit = tile_first_unit((int)cities[city_index].x,
                                (int)cities[city_index].y);
  col = 0;
  row = 0;
  row_start_x = area_left + margin_x;
  stack_count = unit_stack_sum(unit,2);
  if (stack_count <= per_row) {
    offset_y += (row_h >> 1);
    text_clear();
    text_append_label(0x1c3);
    text_set_colors(0x7c,0x12,1,1);
    width = rect_width((RECT *)(iptr)((u8 *)&info_title_rect));
    scaled = scale(1);
    gfx_draw_text_centered((const char *)&textbuf.buf,info_title_rect,info_title_rect_top + scaled,
                       width);
  }
  cell_x = row_start_x;
  cell_y = area_top + offset_y;
  text_set_surface(this);
  text_set_font(&city_font);
  saved_per_row = per_row;
  for (; -1 < unit; unit = unit_next_in_stack(unit)) {
    gfx_draw_unit(this,unit,4,cell_x,cell_y,zoom2,0);
    if (row < 2) {
      scaled = scale(0);
      label_y = cell_y + scaled + unit_h;
      text_clear();
      if (units[unit].home_city == -1) {
        text_append_label(0xe);
      }
      else {
        text_append((const char *)((uint)(byte)units[unit].home_city * 0x58 + (iptr)((u8 *)&cities + 32)));
      }
      textbuf.char_3 = 0;
      text_set_colors(10,0x1a,1,1);
      gfx_draw_text_centered((const char *)&textbuf.buf,cell_x,label_y,cell_w);
    }
    col++;
    cell_x += col_w;
    if (per_row <= col) {
      cell_x = row_start_x;
      cell_y += row_h;
      row++;
      col = 0;
      if (row == 2) {
        cell_x = (col_w >> 1) + area_left + margin_x;
        cell_y = (row_h >> 1) + area_top + offset_y;
        per_row--;
        row_start_x = cell_x;
      }
      else if (row == 4) break;
    }
  }
  col = 0;
  row = 0;
  per_row = saved_per_row;
  cell_x = area_left + margin_x;
  cell_y = area_top + offset_y;
  row_start_x = cell_x;
  for (slot = 0; slot < slot_count2; slot++) {
    if (row == 0) {
      cell_h = unit_h - 3;
    }
    else {
      cell_h = unit_h;
    }
    rect_set_xywh((RECT *)hotspot_rect,cell_x,cell_y,cell_w,cell_h);
    clickmap.hotspot_add_rect(slot,3,(RECT *)hotspot_rect);
    col++;
    cell_x += col_w;
    if (per_row <= col) {
      col = 0;
      row++;
      cell_x = row_start_x;
      cell_y += row_h;
      if (row == 2) {
        cell_x = (col_w >> 1) + area_left + margin_x;
        cell_y = (row_h >> 1) + area_top + offset_y;
        per_row--;
        row_start_x = cell_x;
      }
      else if (row == 4) break;
    }
  }
  if (city_calc.pollution != 0) {
    gfx_set_zoom_scale(view_scale * 4 - 8);
    flag = 0;
    row_width = rect_width((RECT *)&info_panel_rect);
    scaled = scale(0xe);
    row_width -= scaled;
    width = scale(0x40);
    scaled = city_calc.pollution;
    max_icons = city_calc.pollution;
    row_y = scale(100);
    row_y = area_top + row_y;
    pad_x = scale(7);
    city_window_draw_icon_row(&fortify_overlay_sprites_block.pollution_icon_sprite,area_left + pad_x,row_y,scaled,max_icons,width,row_width,flag);
    gfx_reset_zoom_scale();
  }
  cell_x = scale(7);
  cell_x = area_left + cell_x;
  row_start_x = cell_x;
  cell_y = scale(0x85);
  cell_y = area_top + cell_y;
  line_h = city_font.line_spacing();
  line_h -= 2;
  text_set_colors(0x79,0x12,1,1);
  scaled = civ_has_advance(cities[city_index].owner,ADV_TRADE);
  if (scaled != 0) {
    text_clear();
    text_append_label(0x56);
    text_append_colon();
    for (i = 0; i < 3; i++) {
      is_negative = (uint)(*(char *)&cities[(city_index)].supplies[i] < '\0'
                       );
      if (is_negative != 0) {
        text_append_lparen();
      }
      if (*(char *)&cities[(city_index)].supplies[i] < 1) {
        supply_good = ~(int)*(char *)&cities[(city_index)].supplies[i] + 1;
      }
      else {
        supply_good = (iptr)*(char *)&cities[(city_index)].supplies[i];
      }
      text_append_string_id(commodity_names[supply_good]);
      if (is_negative != 0) {
        text_append_rparen();
      }
      if (i < 2) {
        text_append_comma();
      }
    }
    gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
    cell_y += line_h;
    text_clear();
    text_append_label(0x57);
    text_append_colon();
    for (i = 0; i < 3; i++) {
      is_negative = (uint)(*(char *)&cities[(city_index)].demands[i] < '\0'
                       );
      if (is_negative != 0) {
        text_append_lparen();
      }
      if (*(char *)&cities[(city_index)].demands[i] < 1) {
        demand_good = ~(int)*(char *)&cities[(city_index)].demands[i] + 1;
      }
      else {
        demand_good = (iptr)*(char *)&cities[(city_index)].demands[i];
      }
      text_append_string_id(commodity_names[demand_good]);
      if (is_negative != 0) {
        text_append_rparen();
      }
      if (i < 2) {
        text_append_comma();
      }
    }
    gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
    cell_y += line_h;
  }
  scaled = scale(2);
  cell_y += scaled;
  for (i = 0; i < (char)cities[city_index].trade_routes;
      i = i + 1) {
    text_clear();
    text_append((const char *)(cities[(city_index)].route_partner[i] *
                       0x58 + (iptr)((u8 *)&cities + 32)));
    text_append_space();
    if (*(char *)&cities[(city_index)].route_goods[i] < '\0') {
      text_append_label(0xc0);
    }
    else {
      text_append_string_id(commodity_names
                         [*(char *)&cities[(city_index)].route_goods[i]]);
    }
    text_append_colon();
    if (*(char *)&cities[(city_index)].route_goods[i] < '\0') {
      text_append((const char *)&str_1_5);
      scaled = gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
      icon_x = scale(2);
      icon_x = scaled + icon_x;
      gfx_set_zoom_scale(view_scale * 4 - 8);
      scaled = scale(3);
      ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_resource_icon_sprites))->draw((RECT *)food_icon_rect,this,icon_x,cell_y + scaled)
      ;
      gfx_reset_zoom_scale();
    }
    else {
      text_append_plus();
      text_append_int(city_calc.route_trade_values[i]);
      scaled = gfx_draw_text((const char *)&textbuf.buf,cell_x,cell_y);
      icon_x = scale(2);
      icon_x = scaled + icon_x;
      gfx_set_zoom_scale(view_scale * 4 - 8);
      scaled = scale(3);
      ((Sprite *)((i8 *)&small_resource_icon_sprites_block.small_trade_icon_sprite))->draw((RECT *)trade_icon_rect,this,icon_x,cell_y + scaled)
      ;
      gfx_reset_zoom_scale();
    }
    cell_y += line_h;
  }
  return;
}

/* 00507b69 auto */
void CityWindow::draw_support_map()

{
  char civ;
  i32 title_width;
  int origin_x;
  int origin_y;
  int pixel_size;
  iptr x;
  int value;
  iptr unit;
  int y2;
  int map_x;
  int x_offset;
  
  civ = cities[((u32)city_index)].owner;
  text_clear();
  text_append_label(0x1c4);
  text_set_colors(0x7c,0x12,1,1);
  title_width = rect_width((RECT *)&info_title_rect);
  origin_x = scale(1);
  gfx_draw_text_centered((const char *)&textbuf.buf,info_title_rect,info_title_rect_top + origin_x,title_width);
  origin_x = rect_width((RECT *)&info_panel_rect);
  origin_y = rect_height((RECT *)&info_panel_rect);
  pixel_size = int_clamp((origin_x - 4) / (int)map.width,1,(origin_y - 4) / (int)map.height);
  origin_x = ((origin_x - 4) - map.width * pixel_size >> 1) + 2 + info_panel_rect;
  origin_y = ((origin_y - 4) - map.height * pixel_size >> 1) + 2 + info_panel_rect_top;
  x_offset = (int)civs[civ].capital_x - ((int)map.width >> 1);
  if ((game.rules & RULE_FLAT_WORLD) != 0) {
    x_offset = 0;
  }
  for (y2 = 0; y2 < map.height; y2++) {
    for (map_x = 0; map_x < map.width; map_x++) {
      x = map_wrap_x(map_x + x_offset);
      value = map_in_bounds(x,y2);
      if ((value != 0) &&
         ((value = tile_visible_to(x,y2,civ), value != 0 ||
          (game.reveal_map != '\0')))) {
        value = tile_is_ocean(x,y2);
        gfx_target_fill_rect(map_x * pixel_size + origin_x,y2 * pixel_size + origin_y,pixel_size,pixel_size,
                           (-(uint)(value == 0) & 0xffffffd3) + 0x5d);
      }
    }
  }
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && ((uint)(byte)units[unit].home_city == ((u32)city_index)))
    {
      value = map_wrap_x(units[unit].x - x_offset);
      gfx_target_fill_rect(value * pixel_size + origin_x,units[unit].y * pixel_size + origin_y,pixel_size,pixel_size,0x1d);
    }
  }
  value = map_wrap_x(cities[((u32)city_index)].x - x_offset);
  gfx_target_fill_rect(value * pixel_size + origin_x,cities[((u32)city_index)].y * pixel_size + origin_y,pixel_size,pixel_size
                     ,0x29);
  return;
}

/* 00507f31 auto */
void CityWindow::happy_draw_building_icons(i32 *x_ptr,int row_top,int row_h)

{
  int scaled;
  i8 icon_rect[16];
  iptr improvement_id;
  int icon_y;
  int drawn_count;
  int slot;
  int forced;
  
  drawn_count = 0;
  scaled = scale(0x14);
  icon_y = ((row_h >> 1) + row_top) - (scaled >> 1);
  for (slot = 0; slot < 4; slot++) {
    forced = 0;
    switch(slot) {
    case 0:
      improvement_id = 0xe;
      break;
    case 1:
      improvement_id = 0xb;
      scaled = civ_has_advance(cities[((u32)city_index)].owner,ADV_MONOTHEISM);
      if (scaled == 0) continue;
      scaled = civ_has_wonder(cities[((u32)city_index)].owner,WONDER_MICHELANGELO);
      if (scaled != 0) {
        forced = 1;
        improvement_id = 0x31;
      }
      break;
    case 2:
      improvement_id = 4;
      break;
    case 3:
      scaled = city_has_improvement(((u32)city_index),IMP_PALACE);
      if (scaled == 0) {
        improvement_id = 7;
      }
      else {
        improvement_id = 1;
      }
      if (civs[cities[((u32)city_index)].owner].government != GOV_DEMOCRACY) continue;
    }
    scaled = city_has_improvement(((u32)city_index),improvement_id);
    if ((scaled != 0) || (forced != 0)) {
      if (2 < drawn_count) {
        return;
      }
      if (drawn_count != 0) {
        scaled = scale(2);
        *x_ptr = *x_ptr - scaled;
      }
      drawn_count++;
      scaled = scale(0x24);
      *x_ptr = *x_ptr - scaled;
      gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
      ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement_id * 0x3c))->draw((RECT *)icon_rect,this,
                 *x_ptr,icon_y);
      gfx_reset_zoom_scale();
    }
  }
  return;
}

/* 00508177 auto */
void CityWindow::happy_draw_unit_effects(i32 *x_ptr,int row_top,int row_h,int max_width)

{
  byte gov_type;
  bool drew_any;
  iptr civ;
  int unit_w;
  iptr text;
  i32 text_x;
  int scaled;
  int half_h;
  int half_row_h;
  int *count_out;
  i32 flag;
  iptr unit;
  int unit_count;
  int aggressive;
  int unhappy_per_unit;
  int icon_count;
  int icons_width;
  u32 zoom2;
  int unit_h;
  
  drew_any = false;
  civ = (iptr)cities[((u32)city_index)].owner;
  gov_type = civs[civ].government;
  zoom2 = ((u32)map_zoom_level);
  unit_w = gfx_zoom_scale(0x40,zoom2);
  unit_h = gfx_zoom_scale(0x30,zoom2);
  half_row_h = row_h >> 1;
  if (gov_type == GOV_FUNDAMENTALISM) {
    unit_w = city_font.line_spacing();
    unit_w = (half_row_h + row_top) - (unit_w >> 1);
    text_set_colors(0x29,10,1,0);
    flag = 0;
    text_x = *x_ptr;
    text = (iptr)label_string(government_names[4]);
    text_x = gfx_draw_text_right((const char *)text,text_x,unit_w,flag);
    *x_ptr = text_x;
  }
  else if (gov_type < GOV_REPUBLIC) {
    unit_count = 0;
    for (unit = 0; unit < game.num_units; unit++) {
      if ((((units[unit].id != 0) &&
           (half_h = unit_h >> 1, cities[((u32)city_index)].x == units[unit].x)) &&
          (cities[((u32)city_index)].y == units[unit].y)) &&
         (unit_types[units[unit].type].attack != '\0')) {
        unit_count++;
        if (3 < unit_count) {
          return;
        }
        *x_ptr = *x_ptr - unit_w;
        if (drew_any) {
          scaled = scale(2);
          *x_ptr = *x_ptr - scaled;
        }
        else {
          drew_any = true;
        }
        gfx_draw_unit(this,unit,4,*x_ptr,(half_row_h + row_top) - half_h,
                           zoom2,0);
      }
    }
  }
  else {
    unit_w = scale(10);
    half_h = civ_has_wonder(civ,WONDER_WOMENS_SUFFRAGE);
    if ((half_h == 0) && (half_h = city_has_improvement(((u32)city_index),IMP_POLICE_STATION), half_h == 0)) {
      unhappy_per_unit = 1;
    }
    else {
      unhappy_per_unit = 0;
    }
    if (civs[civ].government == GOV_DEMOCRACY) {
      unhappy_per_unit++;
    }
    if (unhappy_per_unit != 0) {
      aggressive = city_calc.aggressive_units;
      if ((city_calc.aggressive_units != 0) && (civs[civ].government == GOV_REPUBLIC)) {
        aggressive = city_calc.aggressive_units - 1;
      }
      icon_count = aggressive * unhappy_per_unit;
      if (icon_count != 0) {
        half_h = scale(2);
        half_h = max_width - half_h;
        text = 0;
        count_out = &icon_count;
        icons_width = half_h;
        scaled = scale(10);
        half_h = ui_calc_item_spacing(icon_count,scaled + 1,half_h,count_out,(int *)text);
        icons_width = scale(10);
        icons_width += (icon_count - 1) * half_h;
        *x_ptr = *x_ptr - icons_width;
        gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
        text_x = 0;
        half_h = icons_width;
        scaled = scale(10);
        city_window_draw_icon_row((Sprite *)&small_unhappy_icon_sprite,*x_ptr,(half_row_h + row_top) - (unit_w >> 1),icon_count
                           ,icon_count,scaled + 1,half_h,text_x);
        gfx_reset_zoom_scale();
      }
    }
  }
  return;
}

/* 00508552 auto */
void CityWindow::draw_happiness()

{
  i32 width;
  int scaled;
  iptr text;
  int scaled2;
  int mid_y;
  i8 coin_rect[16];
  i8 lux_rect[16];
  iptr civ;
  int row_y;
  int area_h;
  int area_x;
  int area_y;
  int area_w;
  int right_x;
  int avail_w;
  int row_offset;
  int area_left;
  int icon_y;
  iptr stage;
  iptr prev_stage;
  int row_h;
  int row_w;
  int icon_w;
  
  civ = (iptr)cities[((u32)city_index)].owner;
  text_clear();
  text_append_label(0x1c5);
  text_set_colors(0x7c,0x12,1,1);
  width = rect_width((RECT *)&info_title_rect);
  scaled = scale(1);
  gfx_draw_text_centered((const char *)&textbuf.buf,info_title_rect,info_title_rect_top + scaled,width);
  area_left = info_title_rect + 2;
  scaled = scale(0xe);
  area_y = info_title_rect_top + scaled + 2;
  area_w = rect_width((RECT *)&info_title_rect);
  area_w -= 4;
  area_h = rect_height((RECT *)&info_title_rect);
  area_h -= 4;
  row_h = area_h / 5;
  area_x = area_left;
  row_y = area_y;
  mid_y = row_h >> 1;
  scaled = scale(0x1e);
  row_offset = mid_y - (scaled >> 1);
  prev_stage = 0;
  for (stage = 0; stage < 5; stage++) {
    if (stage != 0) {
      city_draw_hline(info_title_rect,info_title_rect_right - 1,row_y,0x7c);
    }
    row_w = area_w >> 1;
    right_x = area_x + area_w;
    switch(stage) {
    case 0:
      break;
    case 1:
      if ((uint)(byte)(((i8 *)&city_calc.happy_by_stage))[prev_stage] - (uint)(byte)(((i8 *)&city_calc.unhappy_by_stage))[prev_stage] ==
          (uint)city_calc.happy_after_lux - (uint)city_calc.unhappy_after_lux) continue;
      prev_stage = 1;
      icon_w = scale(0xe);
      right_x -= icon_w;
      mid_y = (row_h >> 1) + row_y;
      scaled = scale(0xe);
      icon_y = mid_y - (scaled >> 1);
      gfx_set_zoom_scale(((u32)view_scale) * 4 - 8);
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.tax_rate_icon_sprites))->draw((RECT *)lux_rect,this,right_x,icon_y);
      gfx_reset_zoom_scale();
      break;
    case 2:
      scaled = city_has_improvement(((u32)city_index),IMP_TEMPLE);
      if (((((scaled == 0) && (scaled = city_has_improvement(((u32)city_index),IMP_COLOSSEUM), scaled == 0)) &&
           (scaled = city_has_improvement(((u32)city_index),IMP_CATHEDRAL), scaled == 0)) &&
          (scaled = civ_has_wonder(civ,WONDER_MICHELANGELO), scaled == 0)) &&
         (((scaled = city_has_improvement(((u32)city_index),IMP_COURTHOUSE), scaled == 0 &&
           (scaled = city_has_improvement(((u32)city_index),IMP_PALACE), scaled == 0)) ||
          (civs[civ].government != GOV_DEMOCRACY)))) continue;
      prev_stage = 2;
      happy_draw_building_icons(&right_x,row_y,row_h);
      break;
    case 3:
      if ((uint)(byte)(((i8 *)&city_calc.happy_by_stage))[prev_stage] - (uint)(byte)(((i8 *)&city_calc.unhappy_by_stage))[prev_stage] ==
          (uint)city_calc.happy_after_units - (uint)city_calc.unhappy_after_units) continue;
      happy_draw_unit_effects(&right_x,row_y,row_h,area_w);
      break;
    case 4:
      if ((uint)city_calc.happy_after_units - (uint)city_calc.unhappy_after_units == (uint)city_calc.happy_after_wonders - (uint)city_calc.unhappy_after_wonders)
      continue;
      mid_y = (row_h >> 1) + row_y;
      scaled = city_font.line_spacing();
      icon_y = mid_y - (scaled >> 1);
      text_set_colors(0x29,10,1,0);
      width = 0;
      scaled = right_x;
      mid_y = icon_y;
      text = (iptr)label_string(label_table.string_ids[0x58]);
      right_x = gfx_draw_text_right((const char *)text,scaled,mid_y,width);
    }
    scaled = scale(2);
    right_x -= scaled;
    avail_w = right_x - area_x;
    scaled = avail_w;
    if (row_w <= avail_w) {
      scaled = row_w;
    }
    row_w = scaled;
    if ((stage == 2) && (civs[civ].government == GOV_FUNDAMENTALISM)) {
      mid_y = (row_h >> 1) + row_y;
      scaled = city_font.line_spacing();
      icon_y = mid_y - (scaled >> 1);
      text_set_colors(0x29,10,1,0);
      text_clear();
      text_append_label(0x5d);
      text_append_colon();
      text_append_int(city_calc.building_content);
      mid_y = (row_h >> 1) + row_y;
      scaled = scale(0xe);
      icon_y = mid_y - (scaled >> 1);
      ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((RECT *)coin_rect,this,area_x,icon_y);
      scaled = icon_y;
      mid_y = scale(0xe);
      scaled2 = scale(3);
      gfx_draw_text((const char *)&textbuf.buf,mid_y + scaled2 + area_x,scaled);
    }
    else {
      happy_draw_citizen_row(area_x,row_y + row_offset,scaled,
                         (uint)(byte)(((i8 *)&city_calc.happy_by_stage))[stage],(uint)(byte)(((i8 *)&city_calc.unhappy_by_stage))[stage]
                         ,(uint)(byte)(((i8 *)&city_calc.angry_by_stage))[stage]);
    }
    row_y += row_h;
  }
  return;
}

/* 00508adc auto */
void CityWindow::draw_info_panel(int blit)

{
  int panel_mode;
  
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    begin_draw((RECT *)&info_panel_rect);
    panel_mode = info_panel_mode;
    if (panel_mode == 0) {
      draw_units_present();
    }
    else if (panel_mode == 1) {
      draw_support_map();
    }
    else if (panel_mode == 2) {
      draw_happiness();
    }
    if (blit != 0) {
      wnd_blit_surface_rect((RECT *)&info_panel_rect);
    }
  }
  return;
}

/* 00508bc5 auto */
void CityWindow::draw_all(i32 blit)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    draw_citizens(blit,0);
    draw_resource_map(blit);
    draw_food_box(blit);
    draw_production(blit);
    draw_button_area(blit);
    draw_supported_units(blit);
    draw_improvements(blit);
    draw_info_panel(blit);
  }
  return;
}

/* 00508c84 auto */
void CityWindow::set_scaled_rect(RECT *rect,int x,int y,int width,int height)

{
  rect_set_xywh(rect,margin_x_2 +
                             (view_scale * x + 1) / 2 +
                             client_x,
                     client_y + (view_scale * y + 1) / 2 +
                     content_offset_y,
                     (view_scale * width + 1) / 2,
                     (view_scale * height + 1) / 2);
  return;
}

/* 00508d24 auto */
void CityWindow::layout_rects()

{
  set_scaled_rect(((RECT *)&citizens_rect),0,0,0x1b4,0x3d);
  set_scaled_rect(((RECT *)&resource_map_rect),0,0x3d,0x1b4,0x99);
  set_scaled_rect((RECT *)&resource_label_rect,7,0x41,0xbc,0x89);
  set_scaled_rect(((RECT *)&food_box_rect),0x1b4,0,200,0xa7);
  set_scaled_rect(((RECT *)&production_rect),0x1b4,0xa7,200,0xbd);
  set_scaled_rect((RECT *)button_area_rect,0x1b4,0x164,200,0x41);
  set_scaled_rect((RECT *)&supported_units_rect,0,0xd4,0xc0,0x4e);
  set_scaled_rect((RECT *)&supported_units_label_rect,7,0xd8,0xb5,0x45);
  set_scaled_rect((RECT *)&improvements_rect,0,0x122,0xc0,0x83);
  set_scaled_rect((RECT *)&improvement_list_rect,6,0x132,0xa6,0x6c);
  set_scaled_rect((RECT *)&info_panel_rect,0xc0,0xd4,0xf4,0xd1);
  set_scaled_rect((RECT *)(u8 *)&info_title_rect,0xc5,0xd8,0xe9,0xc6);
  return;
}

/* 00508ec6 auto */
void CityWindow::fill_margins()

{
  int margin_y;
  int scaled;
  int fill_size;
  int margin_x;
  u32 top;
  i32 extent;
  i32 color;
  
  margin_x = margin_x_2;
  margin_y = content_offset_y;
  if (0 < margin_x) {
    fill_size = layout_width;
    scaled = scale(0x27c);
    fill_size -= (margin_x + scaled);
    gfx_target_fill_rect(client_x,client_y,margin_x_2,
                       resource_bar_height,10);
    color = 10;
    extent = resource_bar_height;
    top = client_y;
    scaled = scale(0x27c);
    gfx_target_fill_rect(client_x + scaled + margin_x,top,fill_size,extent,color);
  }
  if (0 < margin_y) {
    margin_x = resource_bar_height;
    fill_size = scale(0x1a5);
    margin_x -= (margin_y + fill_size);
    gfx_target_fill_rect(client_x,client_y,layout_width,
                       content_offset_y,10);
    color = 10;
    extent = layout_width;
    fill_size = scale(0x1a5);
    gfx_target_fill_rect(client_x,client_y + fill_size + margin_y,extent,margin_x,color);
  }
  return;
}

/* 00509028 auto */
void CityWindow::apply_size()

{
  int height;
  int width;
  u32 border;
  i32 font_h;
  uint small_style;
  int font_size;
  
  border = border_width;
  height = get_height();
  width = get_width();
  view_scale_b0[0] = 2;
  view_scale_byte1[0] = '\0';
  view_scale_byte2[0] = '\0';
  view_scale_byte3[0] = '\0';
  if (((int)(border + 0x297) <= height) && ((int)(border * 2 + 0x3b6) <= width)) {
    view_scale_b0[0] = 3;
    view_scale_byte1[0] = '\0';
    view_scale_byte2[0] = '\0';
    view_scale_byte3[0] = '\0';
  }
  if ((height < (int)(border + 0x1b9)) || (width < (int)(border * 2 + 0x278))) {
    view_scale_b0[0] = 1;
    view_scale_byte1[0] = '\0';
    view_scale_byte2[0] = '\0';
    view_scale_byte3[0] = '\0';
  }
  if (view_scale < 2) {
    font_size = 0xc;
    caption_height = 0x12;
  }
  else {
    font_size = scale(0x10);
    border = scale(0x18);
    caption_height = border;
  }
  ((GameFont *)((i8 *)&city_window_font))->set(0,font_size);
  ((GameFont *)((i8 *)&city_window_small_font))->set(0,(font_size * 2) / 3);
  layout();
  layout_width = client_width;
  resource_bar_height = client_height;
  city_window_load_background();
  height = scale(0x27c);
  width = scale(0x1a5);
  if (height < layout_width) {
    margin_x_2 = layout_width - height >> 1;
  }
  else {
    margin_x_2_b0[0] = '\0';
    margin_x_b1[0] = '\0';
    content_offset_x_byte2[0] = '\0';
    content_offset_x_byte3[0] = '\0';
  }
  if (width < resource_bar_height) {
    content_offset_y = resource_bar_height - width >> 1;
  }
  else {
    content_offset_y_b0[0] = '\0';
    content_offset_y_byte1[0] = '\0';
    content_offset_y_byte2[0] = '\0';
    content_offset_y_byte3[0] = '\0';
  }
  small_style = (uint)(view_scale < 3);
  font_h = scale(0xc);
  city_font.set_styled(1,font_h,small_style);
  if ((margin_x_2 == prev_content_offset_x) &&
     (content_offset_y == prev_content_offset_y)) {
    if (prev_content_offset_x < 0) {
      create_controls();
    }
  }
  else {
    create_controls();
  }
  prev_content_offset_x = margin_x_2;
  prev_content_offset_y = content_offset_y;
  return;
}

/* 005092af auto */
void CityWindow::update_title()

{
  int objective_value;
  
  text_clear();
  text_append_label(0x1f);
  text_append_space();
  text_append_str((const char *)(city_index * 0x58 + (iptr)((u8 *)&cities + 32)));
  if (1 < city_wnd.size_level) {
    text_append_comma();
    text_append_year((int)game.year);
    if ((game.game_flags & GAME_SCENARIO) == 0) {
      text_append_comma();
      text_append_label(0x22);
      text_append_space();
      text_append_city_population(city_index);
    }
    else if (((scenario.flags[0] & 2) != 0) &&
            (objective_value = city_objective_value(city_index), objective_value != 0)) {
      text_append_comma();
      text_append_label(0x1b0);
      if (1 < objective_value) {
        text_append((const char *)&str_x_3);
        text_append_int(objective_value);
      }
    }
    text_append_space();
    text_append_lparen();
    text_append_label(0x9a);
    text_append_colon();
    text_append_gold(civs[cities[city_index].owner].gold);
    text_append_rparen();
  }
  set_caption((const char *)&textbuf.buf);
  return;
}

/* 00509429 auto */
void CityWindow::refresh()

{
  if ((((-1 < city_index) &&
       (city_index < (int)game.num_cities)) &&
      (cities[city_index].id != 0)) && (turn_loop.active != '\0')) {
    if ((redraw_suspended == 0) && (is_closed == 0)) {
      city_recalc(city_index,1);
      apply_size();
      update_title();
      gfx_set_draw_target(this);
      this->FrameWindow::draw_caption();
      fill_margins();
      layout_rects();
      draw_all(0);
      wnd_blit_surface();
    }
    else {
      update_title();
    }
  }
  return;
}

/* 00509545 auto */
void CityWindow::bring_to_top()

{
  NativeWindow *window;
  
  if ((((u32)is_closed) == 0) && (((u32)redraw_suspended) == 0)) {
    window = this->window.get_window();
    BringWindowToTop(window->hwnd);
  }
  return;
}

/* 00509590 auto */
void CityWindow::open(int city)

{
  int flushing;
  
  if ((city_turn.processing == 0) ||
     ((((((flushing = XD_InFlushSendBuffer(), flushing == 0 && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)
         ) && (((net.city_create_busy == 0 && (net.city_delete_busy == 0)) &&
               ((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))))))) &&
       ((net.unit_stack_link_busy == 0 && (((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0))))
       )) && (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
              ((((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))) &&
                (net.unit_delete_cargo_busy == 0)) && (net.unit_destroy_redraw_busy == 0)))))))) {
    game.transient_flags = game.transient_flags | TRANSIENT_CITY_WINDOW_OPENED;
    city_index = city;
    city_save_build_state(city_index);
    if (is_closed != 0) {
      info_panel_mode_b0[0] = '\0';
      info_panel_mode_b1[0] = '\0';
      info_panel_mode_b2[0] = '\0';
      info_panel_mode_b3[0] = '\0';
      improvement_scroll_pos_b0[0] = '\0';
      improvement_scroll_pos_b1[0] = '\0';
      improvement_scroll_pos_b2[0] = '\0';
      improvement_scroll_pos_b3[0] = '\0';
      panel_toggle_flag_b0[0] = 1;
      panel_toggle_flag_b1[0] = '\0';
      panel_toggle_flag_b2[0] = '\0';
      panel_toggle_flag_b3[0] = '\0';
      if (redraw_suspended != 0) {
        this->SurfaceWindow::show();
      }
    }
    is_closed_b0[0] = '\0';
    is_closed_b1[0] = '\0';
    is_closed_b2[0] = '\0';
    is_closed_b3[0] = '\0';
    redraw_deferred_b0[0] = '\0';
    redraw_deferred_b1[0] = '\0';
    redraw_deferred_b2[0] = '\0';
    redraw_deferred_b3[0] = '\0';
    if (redraw_suspended == 0) {
      repaint();
      this->SurfaceWindow::show();
    }
    else {
      ((WndBase *)&window)->show_restored();
    }
    if (bugfix_ui()) {
      ((WndBase *)&window)->set_focus();  /* B12: the keys go to the city window, not an advisor that opened it (docs/BUGS.md) */
    }
    if (city_wnd_state.exit_btn_pending != '\0') {
      create_exit_button(1);
      city_wnd_state.exit_btn_pending = '\0';
    }
    create_change_button(1);
    city_wnd_state.exit_requested = 0;
    if (city_turn.processing == 0) {
      if ((((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && (3 < cities[city].size)) &&
          ((game.shown_event_flags & HINT_CITY_STUFF) == 0)) && (cities[city].owner != human_civ[0])) {
        popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_citystuff);
        game.shown_event_flags = game.shown_event_flags | HINT_CITY_STUFF;
      }
    }
    else {
      if (((((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && ((cities[city].flags & CITYF_DISORDER) != 0)) ||
           (((cities[city].flags & CITYF_DISORDER_LONG) != 0 && (game.difficulty < DIFF_KING)))) &&
          ((cities[city].owner == human_civ[0] && (autoplay.active == 0)))) &&
         (popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_disorder_2), dlg_input.help_pressed != 0)) {
        popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_disorder2);
        popup_text((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_disorder3);
      }
      while (((redraw_suspended == 0 && (is_closed == 0))
             && (city_wnd_state.exit_requested == 0))) {
        win_pump_one_message();
      }
    }
  }
  return;
}

/* 00509935 auto */
void __cdecl city_window_on_redraw(void)

{
  ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
  return;
}

/* 0050994f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl city_window_close(void)

{
  city_wnd.closed = 1;
  ((WndBase *)((i8 *)&city_wnd.wnd))->hide();
  city_wnd.city = 0xffffffff;
  if (bugfix_ui() && (-1 < (int)advisor_state.report_id)) {
    advisor_state.wnd.focus_and_raise();  /* B12: back to the advisor that opened the city (docs/BUGS.md) */
  }
  else {
    main_frame_wnd_block.main_frame_wnd.focus_and_raise();
  }
  ui_flush_input();
  return 0;
}

/* 0050998f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_calc_rect(void)

{
  int width;
  int height;
  int screen_w;
  int screen_h;
  
  width = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x27c);
  height = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x1a5);
  screen_w = ((CityWindow *)((i8 *)&city_wnd.window))->scale(0x18);
  height = height + screen_w + 8;
  screen_w = main_frame_wnd_block.main_frame_wnd.client_width();
  screen_h = main_frame_wnd_block.main_frame_wnd.client_height();
  city_wnd.prev_offset_x = 0xffffffff;
  rect_set_xywh((RECT *)&minimap_wnd_rect_block.city_wnd_rect,(screen_w >> 1) - (width + 0x10 >> 1),(screen_h >> 1) - (height >> 1),
                     width + 0x10 + ui_metrics.dlgframe_w2,height + ui_metrics.dlgframe_h2);
  return;
}

/* 00509a49 auto */
void __cdecl city_window_init_size(void)

{
  city_wnd.size_level = 2;
  if (999 < caption_font_block.screen_width) {
    city_wnd.size_level = 3;
  }
  city_window_calc_rect();
  return;
}

/* 00509a82 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_on_minimize(void)

{
  city_wnd.minimized = 1;
  return;
}

/* 00509a9c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_on_restore(void)

{
  city_wnd.minimized = 0;
  ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
  return;
}

/* 00509ac0 auto */
void __cdecl city_text_append_build_turns(int cost,int shields)

{
  int turns;
  int total_needed;
  i32 min_turns;
  i32 max_turns;
  
  max_turns = 999;
  min_turns = 1;
  total_needed = cost * city_calc.shield_rows;
  turns = int_clamp_c(city_calc.total_shields - city_calc.shield_support,1,99);
  turns = int_clamp_c(((total_needed - 1) - shields) / turns + 1,min_turns,max_turns);
  text_append_int(turns);
  text_append_space();
  if (turns == 1) {
    text_append_label(0x2d);
  }
  else {
    text_append_label(0x2c);
  }
  return;
}

/* 00509b48 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_buy(i32 control_id)

{
  bool can_afford;
  iptr cur_city;
  iptr flushing;
  int civ;
  iptr city2;
  iptr city3;
  iptr unit_type;
  iptr city4;
  int building;
  iptr city5;
  iptr city6;
  int city;
  uint shield_rows;
  i32 arg_lo;
  i32 clamp_max;
  uint cost2;
  int price;
  i8 text_key[80];
  i32 eh_next;
  
  Dialog dialog2(0x4000);
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    flushing = XD_InFlushSendBuffer();
    if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
          ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
         (((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))) &&
          (net.unit_stack_link_busy == 0)))) &&
        ((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
         (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
          ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))))) &&
       ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))) {
      arg_lo = 0;
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city2,arg_lo);
      civ = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      civ = (int)cities[civ].owner;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      unit_type = (iptr)cities[city3].building;
      city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      building = (int)cities[city4].building;
      if ((human_civ[0] == civ) || (game.reveal_map != '\0')) {
        if (-building != 0x26) {
          city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (cities[city5].building < '\0') {
            cost2 = (uint)improvements[-building].cost;
            clamp_max = 999;
            arg_lo = 0;
            shield_rows = (uint)cosmic.shield_box_rows;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city = int_clamp(shield_rows * cost2 - (int)cities[city].shield_box,arg_lo,
                                        clamp_max);
            price = city * 2;
            if (0x22 < -building) {
              price = city << 2;
            }
            text_set_label_arg(0,improvements[-building].name);
          }
          else {
            cost2 = (uint)(char)unit_types[unit_type].cost;
            clamp_max = 999;
            arg_lo = 0;
            shield_rows = (uint)cosmic.shield_box_rows;
            city6 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city = int_clamp(shield_rows * cost2 - (int)cities[city6].shield_box,arg_lo,
                                        clamp_max);
            price = (city * city) / 0x14 + city * 2;
            text_set_label_arg(0,unit_types[unit_type].name);
          }
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (cities[city].shield_box == 0) {
            price = price << 1;
          }
          text_set_number_arg(0,price);
          text_set_number_arg_v(1,civs[civ].gold);
          crt_strcpy(text_key,str_complete0);
          can_afford = price <= civs[civ].gold;
          if (can_afford) {
            text_key[8] = 0x31;
          }
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if ((-1 < cities[city].building) &&
             (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
             (cities[city].flags & CITYF_DISORDER) != 0)) {
            text_key[8] = 0x32;
            can_afford = false;
          }
          dialog2.load_game_text((const char *)text_key);
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          if (cities[city].building < '\0') {
            dialog2.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + building * -0x3c)),0,0);
          }
          else {
            dialog2.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c)),0,0);
          }
          dialog2.set_image_zoom(8);
          if (can_afford) {
            dialog2.set_default_choice(1);
          }
          building = dialog2.show_modal(0);
          if (can_afford) {
            if ((building == 0) && (city_wnd.closed != 1)) {
              civs[civ].gold -= price;
              civ = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[civ].shield_box = (ushort)cosmic.shield_box_rows * (short)cost2;
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_save_build_state(city2);
              snd_play_effect(SND_POS1,1,0,0);
              arg_lo = 1;
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              city_recalc(city2,arg_lo);
              ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
              ((CityWindow *)((i8 *)&city_wnd.window))->update_title();
              status_draw_info(1);
            }
          }
        }
      }
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_buy_blocked_by_xd_fl);
      net.blocked_input_kind = 7;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      civ = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[civ].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050a1d6 auto */
i32 __cdecl
city_change_draw_option_icon(i32 dialog,DibSurface *surface,int option,byte flags,int x,int y,
            int row_height)

{
  i8 improvement_rect[16];
  i8 unit_rect[16];
  int improvement;
  i32 zoom2;
  int unit_type;
  int draw_y;
  int draw_x;
  int y_offset;
  int sprite_height2;
  
  unit_type = -1;
  improvement = -1;
  zoom2 = 0xfffffffe;
  if (option < 0x3e) {
    unit_type = option;
  }
  else {
    improvement = option - 0x3e;
  }
  draw_x = x;
  if ((flags & 1) != 0) {
    draw_x = x + 0x26;
  }
  if (-1 < unit_type) {
    draw_y = y;
    sprite_height2 = gfx_zoom_scale(0x30,0xfffffffe);
    y_offset = (sprite_height2 - row_height) / 2;
    draw_y -= y_offset;
    gfx_set_zoom_scale(zoom2);
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->draw((RECT *)unit_rect,surface,draw_x,
               draw_y);
    gfx_reset_zoom_scale();
  }
  if (-1 < improvement) {
    y_offset = (0x14 - row_height) / 2;
    draw_y = y - y_offset;
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((RECT *)improvement_rect,surface,draw_x,
               draw_y);
  }
  return 0;
}

/* Part of city_button_change (0050a473): with the advisors option, the
   build the advisors suggest, in two dialogs beside the list. */
static void city_button_change_advice(Dialog &unit_dialog,Dialog &improvement_dialog)
{
  int city;
  iptr city6;
  i32 *unit_out;
  i32 *improvement_out;
  i32 improvement;
  i32 unit_type;
  i8 caption[80];  /* BF-051 (Peter Johnson's ledger): a longer name is cut
                      (and its advice section not found) */

  if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
    improvement_out = &improvement;
    unit_out = &unit_type;
    city6 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_choose_production(city6,unit_out,improvement_out);
    if (unit_type != 999) {
      text_set_label_arg(0,unit_types[unit_type].name);
      unit_dialog.load_text((const char *)&str_advice_2,(const char *)&str_military,8);
      str_clear((char *)caption);
      str_append((char *)caption,(const char *)&str_empty_145);
      str_append_max((char *)caption,label_string(unit_types[unit_type].name),sizeof caption);
      unit_dialog.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c)),0,0);
      city = unit_dialog.load_text((const char *)&str_advice_3,(const char *)caption,0x8000)
      ;
      if (city != 0) {
        unit_dialog.load_text((const char *)&str_advice_4,(const char *)&str_default,0x8000);
      }
      unit_dialog.run_modal(0,300);
    }
    if (improvement != 999) {
      str_clear((char *)caption);
      str_append((char *)caption,(const char *)&str_empty_146);
      improvement_dialog.load_text((const char *)&str_advice_5,(const char *)&str_domestic,8);
      if (improvement < 0) {
        city = -improvement;
        improvement = -improvement;
        text_set_label_arg(0,improvements[city].name);
        if (improvement < 0x27) {
          str_append_max((char *)caption,label_string(improvements[improvement].name),sizeof caption);
          improvement_dialog.add_sprite(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c)),
                     (ui_metrics.large_screen == 0) - 1 & 8,0);
        }
        else {
          str_append((char *)caption,(const char *)&str_wonder);
          improvement_dialog.add_sprite(
                     ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (improvement * 4 - 0x9c) * 0xf)),
                     (ui_metrics.large_screen == 0) - 1 & 8,0);
        }
      }
      else {
        unit_type = improvement;
        text_set_label_arg(0,unit_types[improvement].name);
        str_append_max((char *)caption,label_string(unit_types[unit_type].name),sizeof caption);
        improvement_dialog.add_sprite(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c)),0,0
                  );
      }
      city = improvement_dialog.load_text((const char *)&str_advice_6,(const char *)caption,0x8000)
      ;
      if (city != 0) {
        improvement_dialog.load_text((const char *)&str_advice_7,(const char *)&str_default_2,0x8000);
      }
      improvement_dialog.run_modal(0,300);
    }
  }
}

/* Part of city_button_change (0050a473): the units, improvements and
   wonders the city can build, the options of the dialog. */
static void city_button_change_add_options(int city2,int &option_count2,iptr current_build,Dialog &change_dialog)
{
  iptr city3;
  int city;
  iptr city4;
  i32 on;
  iptr improvement;
  iptr unit_type;
  int wonder;

  option_count2 = 0;
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    city3 = unit_type;
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city = civ_can_build_unit(city2,city4,city3);
    if (city != 0) {
      text_clear();
      text_append_string_id(unit_types[unit_type].name);
      text_append_space();
      text_append((const char *)&str_empty_147);
      if (extra_ui(X_PROD_COST)) {
        extras_append_shield_cost((int)(char)unit_types[unit_type].cost);   /* X30 */
      }
      text_append_lparen();
      city3 = unit_type;
      city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      on = city_shields_after_change(city4,city3);
      city_text_append_build_turns((int)(char)unit_types[unit_type].cost,on);
      text_append_comma();
      text_append_label(0xaa);
      text_append_colon();
      text_append_int((int)(char)unit_types[unit_type].attack);
      text_append((const char *)&str_empty_148);
      text_append_int((int)(char)unit_types[unit_type].defense);
      if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
        text_append((const char *)&str_empty_149);
      }
      text_append((const char *)&str_empty_150);
      text_append_int((int)(char)unit_types[unit_type].move /
                         (int)(uint)cosmic.road_move_mult);
      text_append_space();
      text_append_label(0xab);
      text_append_colon();
      text_append_int((int)(char)unit_types[unit_type].hit_points / 10);
      text_append((const char *)&str_empty_151);
      text_append_int((int)(char)unit_types[unit_type].firepower);
      text_append_rparen();
      change_dialog.add_option((const char *)&textbuf.buf,unit_type,0);
      if (current_build == unit_type) {
        change_dialog.set_default_choice(unit_type);
      }
      option_count2++;
    }
  }
  for (improvement = 1; improvement < 0x27; improvement++) {
    city3 = improvement;
    city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city = city_can_build_improvement(city2,city4,city3);
    if (city != 0) {
      text_clear();
      text_append_string_id(improvements[improvement].name);
      text_append_space();
      text_append((const char *)&str_empty_152);
      if (extra_ui(X_PROD_COST)) {
        extras_append_shield_cost((uint)improvements[improvement].cost);   /* X30 */
      }
      text_append_lparen();
      city = -improvement;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      on = city_shields_after_change(city3,city);
      city_text_append_build_turns((uint)improvements[improvement].cost,on);
      if (extra_ui(X_PROD_COST)) {
        /* X30: and its upkeep */
        text_append_comma();
        text_append_int(civ_improvement_upkeep(city2,improvement));
        text_append(EXTRAS_ICON_S "g");
      }
      text_append_rparen();
      change_dialog.add_option((const char *)&textbuf.buf,improvement + 0x3e,0);
      if (-current_build == improvement) {
        change_dialog.set_default_choice(improvement + 0x3e);
      }
      option_count2++;
    }
  }
  for (wonder = 0; wonder < 0x1c; wonder++) {
    city = civ_can_build_wonder(city2,wonder);
    if (city != 0) {
      improvement = wonder + 0x27;
      text_clear();
      city = wonder_is_expired(wonder);
      if (city != 0) {
        text_append_str((const char *)&str_empty_153);
      }
      text_append_string_id(improvements[improvement].name);
      text_append_space();
      text_append((const char *)&str_empty_154);
      if (extra_ui(X_PROD_COST)) {
        extras_append_shield_cost((uint)improvements[improvement].cost);   /* X30 */
      }
      text_append_lparen();
      city = -improvement;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      on = city_shields_after_change(city3,city);
      city_text_append_build_turns((uint)improvements[improvement].cost,on);
      text_append_rparen();
      change_dialog.add_option((const char *)&textbuf.buf,wonder + 0x65,0);
      if (-current_build == improvement) {
        change_dialog.set_default_choice(improvement + 0x3e);
      }
      option_count2++;
    }
  }
}

/* 0050a473 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_change(i32 control_id)

{
  char *build_count;
  byte civ;
  iptr cur_city;
  iptr flushing;
  int city2;
  iptr city3;
  iptr city5;
  int city;
  iptr city4;
  iptr city7;
  iptr city8;
  i32 on;
  iptr unit_type3;
  int option_count2;
  iptr current_build;
  int new_shields;
  iptr unit_type2;
  int confirmed;
  
  Dialog change_dialog(0x8000);
  Dialog unit_dialog(0x4000);
  Dialog improvement_dialog(0x4000);
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    flushing = XD_InFlushSendBuffer();
    if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
          ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
         (((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))) &&
          (net.unit_stack_link_busy == 0)))) &&
        ((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
         (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
          ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))))) &&
       ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))) {
      on = 0;
      city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city_recalc(city3,on);
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      civ = cities[city2].owner;
      city2 = (int)(char)civ;
      if ((human_civ[0] == city2) || (game.reveal_map != '\0')) {
        city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        current_build = (iptr)cities[city5].building;
        while (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
              city != -1) {
          city_button_change_advice(unit_dialog,improvement_dialog);
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          text_set_string_arg(0,(const char *)&cities[city].name);
          if (extra_ui(X_PROD_CANCEL)) {
            /* X29: with a Cancel button (Escape) */
            change_dialog.load_text((const char *)&str_game_7,(const char *)&str_production,1);
          }
          else
          change_dialog.load_game_text((const char *)&str_production);
          change_dialog.set_option_draw_fn((DialogOptionDrawFn)city_change_draw_option_icon);
          change_dialog.set_list_indent(0x4b,0);
          city_button_change_add_options(city2,option_count2,current_build,change_dialog);
          if ((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) != 0) && (game.multiplayer == '\0')) {
            city3 = (iptr)label_string(label_table.string_ids[0x4c]);
            change_dialog.add_button((const char *)city3);
          }
          if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
            if (0xb < option_count2) {
              option_count2 = 0xc;
            }
            change_dialog.set_position(0xfffffc19,0xfffffffb);
          }
          if (option_count2 < 0x10) {
            city = option_count2 + 1;
            if (city < 9) {
              city = 8;
            }
            change_dialog.set_list_rows(city);
          }
          city_wnd_state.dlg_close_pending = 0;
          dlg_style.pending_idle_callback = city_change_idle_autoclose;
          unit_type3 = change_dialog.run_modal(0,300);
          city_wnd_state.dlg_close_pending = 0;
          if ((unit_type3 < 0) ||
             (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(), city == -1
             )) {
            ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
            return;
          }
          if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
            improvement_dialog.close();
            unit_dialog.close();
            ui_flush_input();
          }
          city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          cities[city].flags = cities[city].flags & ~(CITYF_AUTOBUILD | CITYF_AUTO_UNITS_ONLY | CITYF_AUTO_IMPROVEMENTS_ONLY);
          if (change_dialog.result_extra == 0) {
            if (unit_type3 < 0x3e) {
              unit_type2 = unit_type3;
            }
            else {
              unit_type2 = -(unit_type3 - 0x3e);
            }
            city3 = unit_type2;
            city4 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            new_shields = city_shields_after_change(city4,city3);
            confirmed = 1;
            city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            if ((new_shields < cities[city].shield_box) && ((game.message_options & MSGOFF_PRODUCTION_PENALTY) == 0)) {
              text_set_number_arg_u8(0,cosmic.change_penalty);
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              if (cities[city].building < '\0') {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                text_set_label_arg(0,improvements[-(int)cities[city].building].name);
              }
              else {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                text_set_label_arg(0,unit_types[cities[city].building].name);
              }
              if (unit_type2 < 0) {
                text_set_label_arg(1,improvements[-unit_type2].name);
              }
              else {
                text_set_label_arg(1,unit_types[unit_type2].name);
              }
              city = popup_game((const char *)&str_prodchange);
              if (city == 0) {
                confirmed = 0;  /* the production kept */
              }
            }
            if (confirmed != 0) {
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              if (-1 < cities[city].building) {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + city2 * 0x594);
                *build_count = *build_count - 1;
              }
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city].building = (i8)unit_type2;
              city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              if (-1 < cities[city].building) {
                city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                build_count = (char *)(cities[city].building + (iptr)((u8 *)&civs + 340) + city2 * 0x594);
                *build_count = *build_count + 1;
              }
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city2].shield_box = (i16)new_shields;
              advisor_refresh_if_open(1);
              advisor_refresh_if_open(2);
            }
          }
          else if (change_dialog.result_extra == 1) {
            if ((1 << (civ & 0x1f) & (uint)game.human_players) != 0) {
              city_load_citypref();
              on = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              unit_type3 = popup_game_with_city_flags((const char *)&str_automode,1,on);
              if (unit_type3 < 0) {
                ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
                return;
              }
              city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
              cities[city2].flags = cities[city2].flags | CITYF_AUTOBUILD;
              if (unit_type3 == UNIT_ENGINEERS) {
                city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                cities[city2].flags = cities[city2].flags | CITYF_AUTO_UNITS_ONLY;
              }
              if (unit_type3 == UNIT_WARRIORS) {
                city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                cities[city2].flags = cities[city2].flags | CITYF_AUTO_IMPROVEMENTS_ONLY;
              }
            }
            on = 99;
            city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
            city_set_production(city3,on);
            ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
            ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
            return;
          }
          else if (change_dialog.result_extra != 2) {
            if (change_dialog.result_extra == 3) {
              if (unit_type3 < 0x3e) {
                city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                city2 = (int)cities[city2].y;
                city7 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                city = (int)cities[city7].x;
                city8 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                unit_create(unit_type3,(int)cities[city8].owner,city,city2);
                city2 = unit_type3;
              }
              else {
                city2 = unit_type3 - 0x3e;
                if (city2 < 0x23) {
                  on = 1;
                  unit_type3 = city2;
                  city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                  city_set_improvement(city3,city2,on);
                  city2 = unit_type3;
                }
                else if (city2 < 0x27) {
                  if ((0x22 < city2) && (city2 < 0x26)) {
                    city = unit_type3 - 0x61;
                    unit_type3 = city2;
                    city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                    spaceship_add_part(cities[city2].owner,city);
                    city2 = unit_type3;
                  }
                }
                else {
                  unit_type3 -= 0x65;
                  on = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
                  game.wonder_city[unit_type3] = (i16)on;
                  city2 = unit_type3;
                }
              }
              unit_type3 = city2;
              ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
              popup_debug_text((const char *)&str_freebie);
            }
          }
          else {
            if (unit_type3 < 0x3e) {
              ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_unit_modal(unit_type3);
              current_build = unit_type3;
            }
            else {
              unit_type3 -= 0x3e;
              ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_improvement_modal(unit_type3);
              current_build = -unit_type3;
            }
            continue;
          }
          on = 1;
          city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
          city_recalc(city3,on);
          ((CityWindow *)((i8 *)&city_wnd.window))->draw_production(1);
          ((CityWindow *)((i8 *)&city_wnd.window))->draw_resource_map(1);
          ((CityWindow *)((i8 *)&city_wnd.window))->create_change_button(1);
          return;
        }
        change_dialog.close();
        unit_dialog.close();
        improvement_dialog.close();
      }
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_change_blocked_by_xd);
      net.blocked_input_kind = 8;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[city2].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050b674 auto */
void __cdecl city_change_idle_autoclose(void)

{
  int result;
  
  if (((mp_timer.handle != 0) && (mp_timer.seconds_left < 1)) ||
     (result = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(), result == -1)) {
    if (net.host_flag == '\0') {
      net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
      ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
    }
    else if (city_wnd_state.dlg_close_pending == '\0') {
      city_wnd_state.dlg_close_pending = 1;
      city_dlg_close_start_ticks = timer_ticks();
    }
    else {
      result = timer_ticks();
      if (city_dlg_close_start_ticks + 0x4b0 < result) {
        city_wnd_state.dlg_close_pending = '\0';
        net.dialog_stack_top[0xf] = net.dialog_stack_top[0xf] | 0x400;
        ((ModalMenuWindow *)(*net.dialog_stack_top + 0x48))->end_modal_loop();
      }
    }
  }
  net.process_messages(1,0);
  return;
}

/* 0050b74e auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_rename(i32 control_id)

{
  iptr cur_city;
  iptr city2;
  int city;
  i8 *name_buf;
  i8 new_name[260];
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cur_city != -1) &&
     (city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
     cities[city2].owner == human_civ[0])) {
    city = XD_InFlushSendBuffer();
    if (((((city == 0) &&
          ((((net.unit_create_busy == 0 && (net.unit_delete_busy == 0)) && (net.city_create_busy == 0)) &&
           ((net.city_delete_busy == 0 && (net.lock_map_busy == 0)))))) &&
         ((net.unlock_map_busy == 0 &&
          ((((net.unit_stack_unlink_busy == 0 && (net.unit_stack_link_busy == 0)) &&
            ((net.unit_stack_bottom_busy == 0 &&
             (((net.unit_relocate_busy == 0 && (net.city_acquire_wonder_busy == 0)) && (net.civ_eliminate_busy == 0)))))) &&
           ((net.civ_create_busy == 0 && (net.civ_check_destroyed_busy == 0)))))))) && (net.unit_gather_cargo_busy == 0)) &&
       (((net.unit_lift_off_map_busy == 0 && (net.unit_delete_cargo_busy == 0)) && (net.unit_destroy_redraw_busy == 0)))) {
      city_wnd_state.dlg_close_pending = 0;
      dlg_style.pending_idle_callback = city_change_idle_autoclose;
      name_buf = new_name;
      city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city = dlg_game_text_input(str_renamecity,0xf,cities[city].name,name_buf);
      if ((city == 0) &&
         (city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(), city != -1)) {
        snd_play_effect(SND_POS1,0,0,0);
        name_buf = new_name;
        city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        crt_strcpy((char *)(&cities[city].name),name_buf);
        ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
        map_redraw_all_views(human_civ[0],1);
      }
      city_wnd_state.dlg_close_pending = 0;
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_rename_blocked_by_xd);
      net.blocked_input_kind = 10;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[city].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050b9a4 auto */
void __cdecl city_button_info(i32 control_id)

{
  int cur_city;
  iptr city;
  i32 recalc_mode;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    city_wnd.info_panel_mode = 0;
    recalc_mode = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_mode);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    snd_play_effect(SND_MOVPIECE,0,0,0);
  }
  return;
}

/* 0050ba07 auto */
void __cdecl city_button_map(i32 control_id)

{
  int cur_city;
  iptr city;
  i32 recalc_mode;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    city_wnd.info_panel_mode = 1;
    recalc_mode = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_mode);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    snd_play_effect(SND_MOVPIECE,0,0,0);
  }
  return;
}

/* 0050ba6a auto */
void __cdecl city_button_happy(i32 control_id)

{
  int cur_city;
  iptr city;
  i32 recalc_mode;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    city_wnd.info_panel_mode = 2;
    recalc_mode = 0;
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city_recalc(city,recalc_mode);
    ((CityWindow *)((i8 *)&city_wnd.window))->draw_info_panel(1);
    snd_play_effect(SND_MOVPIECE,0,0,0);
  }
  return;
}

/* 0050bacd auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_button_view(i32 control_id)

{
  iptr cur_city;
  iptr flushing;
  int city;
  i32 city2;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    flushing = XD_InFlushSendBuffer();
    if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
          ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
         ((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))))) &&
        (net.unit_stack_link_busy == 0)) &&
       (((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
         (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
          ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))) &&
        ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))))) {
      city2 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      cityview_open(city2);
    }
    else {
      dbg_log((const char *)&str_citywin_city_button_view_blocked_by_xd_f);
      net.blocked_input_kind = 9;
      net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      net.blocked_input_arg2 = (int)cities[city].owner;
      net.blocked_input_arg3 = control_id;
    }
  }
  return;
}

/* 0050bc4f auto */
void __cdecl city_window_exit_clicked(i32 control_id)

{
  int cur_city;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if (cur_city != -1) {
    snd_play_effect(SND_MOVPIECE,0,0,0);
    if (city_wnd.open_mode == 2) {
      city_wnd_state.exit_requested = 1;
    }
    else {
      city_window_close();
    }
    advisor_refresh_if_open(1);
    advisor_refresh_if_open(2);
    advisor_refresh_if_open(4);
    advisor_refresh_if_open(5);
    advisor_refresh_if_open(6);
  }
  return;
}

/* 0050bcda auto */
void __cdecl city_window_on_improvement_scroll(i32 scroll_pos)

{
  iptr city;
  i32 recalc_mode;
  
  city_wnd.improvement_list_top = scroll_pos;
  recalc_mode = 0;
  city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  city_recalc(city,recalc_mode);
  ((CityWindow *)((i8 *)&city_wnd.window))->draw_improvements(1);
  return;
}

/* 0050bd13 auto */
void __cdecl city_button_next_city(i32 control_id)

{
  iptr cur_city;
  iptr city3;
  int cmp;
  char first_name[80];
  iptr city;
  char next_name[80];
  iptr city4;
  iptr city5;
  iptr city2;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cur_city != -1) &&
     ((city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
      cities[city3].owner == human_civ[0] || (game.reveal_map != '\0')))) {
    if (city_turn.processing == 0) {
      city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city4 = -1;
      city2 = city5;
      crt_strcpy(next_name,str_empty_155);
      crt_strcpy(first_name,(const char *)(&cities[city2].name));
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city2].owner == cities[city].owner)) {
          cmp = __strcmpi(cities[city].name,next_name);
          if ((0 < cmp) &&
             (cmp = __strcmpi(cities[city].name,cities[city2].name), cmp < 0)) {
            city4 = city;
            crt_strcpy(next_name,(const char *)(&cities[city].name));
          }
          cmp = __strcmpi(cities[city].name,first_name);
          if (0 < cmp) {
            city5 = city;
            crt_strcpy(first_name,(const char *)(&cities[city].name));
          }
        }
      }
      if (city4 < 0) {
        if (city2 != city5) {
          ((CityWindow *)((i8 *)&city_wnd.window))->open(city5);
        }
      }
      else {
        ((CityWindow *)((i8 *)&city_wnd.window))->open(city4);
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  return;
}

/* 0050bf72 auto */
void __cdecl city_button_prev_city(i32 control_id)

{
  iptr cur_city;
  iptr city3;
  int cmp;
  char last_name[80];
  iptr city;
  char prev_name[80];
  iptr city4;
  iptr city5;
  iptr city2;
  
  cur_city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
  if ((cur_city != -1) &&
     ((city3 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city(),
      cities[city3].owner == human_civ[0] || (game.reveal_map != '\0')))) {
    if (city_turn.processing == 0) {
      city5 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      city4 = -1;
      city2 = city5;
      crt_strcpy(prev_name,str_zzzzzzzzzzzzzzzzzzzzzzzzz);
      crt_strcpy(last_name,(const char *)(&cities[city2].name));
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city2].owner == cities[city].owner)) {
          cmp = __strcmpi(cities[city].name,prev_name);
          if ((cmp < 0) &&
             (cmp = __strcmpi(cities[city].name,cities[city2].name), 0 < cmp)) {
            city4 = city;
            crt_strcpy(prev_name,(const char *)(&cities[city].name));
          }
          cmp = __strcmpi(cities[city].name,last_name);
          if (cmp < 0) {
            city5 = city;
            crt_strcpy(last_name,(const char *)(&cities[city].name));
          }
        }
      }
      if (city4 < 0) {
        if (city5 != city2) {
          ((CityWindow *)((i8 *)&city_wnd.window))->open(city5);
        }
      }
      else {
        ((CityWindow *)((i8 *)&city_wnd.window))->open(city4);
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  return;
}

/* 0050c1d1 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl city_window_on_mouse(i32 x,i32 y,i32 button)

{
  iptr flushing;
  iptr city;
  int unit;
  int hit;
  i32 hit_kind;
  
  flushing = XD_InFlushSendBuffer();
  if (((((((flushing == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
        ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) &&
       ((net.lock_map_busy == 0 && ((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)))))) && (net.unit_stack_link_busy == 0)
      ) && (((((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)) &&
             (((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)) &&
              ((net.civ_check_destroyed_busy == 0 && ((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)))))))) &&
            ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))))) {
    hit = city_wnd.clickmap.hit_test(x,y,&unit,&hit_kind)
    ;
    if (-1 < hit) {
      switch(hit_kind) {
      case 1:
        city_window_toggle_worked_tile(x,y);
        break;
      case 2:
        city_window_cycle_specialist(unit);
        break;
      case 3:
        city_window_present_unit_menu(unit);
        break;
      case 4:
        city_window_sell_improvement(unit);
        break;
      case 5:
        break;
      case 6:
        city_window_supported_unit_menu(unit);
      }
    }
  }
  else {
    dbg_log((const char *)&str_citywin_city_mouse_blocked_by_xd_flushse);
    net.blocked_input_kind = 6;
    net.blocked_input_arg1 = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
    net.blocked_input_arg2 = (int)cities[city].owner;
    net.blocked_input_arg3 = x;
    net.blocked_input_arg4 = y;
    net.blocked_input_arg5 = button;
  }
  return;
}

/* 0050c405 auto */
void __cdecl city_window_on_lbuttonup(i32 x,i32 y)

{
  city_window_on_mouse(x,y,0);
  return;
}

/* 0050c427 auto */
void __cdecl city_window_on_rbuttonup(i32 x,i32 y)

{
  city_window_on_mouse(x,y,1);
  return;
}

/* 0050c449 auto */
void CityWindow::close_if_city(int city)

{
  if (((int)((u32)city_index) <= city) && (city <= (int)((u32)city_index))) {
    city_window_close();
  }
  return;
}

/* 0050c494 auto */
void CityWindow::on_unit_change(int unit,int x,int y)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) && (-1 < city_index)) {
    if (unit < 0) {
      if ((cities[city_index].x == x) &&
         (cities[city_index].y == y)) {
        city_recalc(city_index,0);
        draw_info_panel(1);
        draw_production(1);
      }
    }
    else if (((cities[city_index].x == units[unit].x) &&
             (cities[city_index].y == units[unit].y)) ||
            ((cities[city_index].x == x &&
             (cities[city_index].y == y)))) {
      city_recalc(city_index,0);
      draw_info_panel(1);
      draw_production(1);
      if ((uint)(byte)units[unit].home_city == (uint)city_index) {
        city_recalc(city_index,1);
        draw_supported_units(1);
        recalc_redraw(0);
      }
    }
  }
  return;
}

/* 0050c679 auto */
void CityWindow::refresh_if_city(int city)

{
  if ((((is_closed == 0) && (redraw_suspended == 0)) &&
      (redraw_deferred == 0)) &&
     ((city_index == city && (-1 < city)))) {
    refresh();
  }
  return;
}

/* 0050c6ef auto */
void CityWindow::refresh_if_near(i32 x,i32 y)

{
  int dist;
  
  if ((((((u32)is_closed) == 0) && (((u32)redraw_suspended) == 0)) && (((u32)redraw_deferred) == 0)) &&
     ((-1 < (int)((u32)city_index) &&
      (dist = map_distance_diag(x,y,(int)cities[((u32)city_index)].x,
                                  (int)cities[((u32)city_index)].y), dist < 3)))) {
    recalc_redraw(0);
  }
  return;
}

/* 0050c7a3 auto */
void CityWindow::clear_controls()

{
  buy_button = 0;
  (change_button) = 0;
  (exit_button) = 0;
  view_button = ((u32)(iptr)exit_button);
  happy_button = view_button;
  rename_button = happy_button;
  map_button = rename_button;
  info_button = map_button;
  next_city_button = 0;
  prev_city_button = next_city_button;
  return;
}

/* 0050c859 auto */
void CityWindow::destroy_controls()

{
  extras_city_window_controls_destroyed();   /* X11 */
  if (buy_button != 0) {
    window.remove_all_controls();
    if ((Button *)buy_button != (Button *)0x0) {
      ((Button *)buy_button)->scalar_destruct(1);
    }
    if (change_button != (Button *)0x0) {
      change_button->scalar_destruct(1);
    }
    if ((Button *)info_button != (Button *)0x0) {
      ((Button *)info_button)->scalar_destruct(1);
    }
    if ((Button *)map_button != (Button *)0x0) {
      ((Button *)map_button)->scalar_destruct(1);
    }
    if ((Button *)rename_button != (Button *)0x0) {
      ((Button *)rename_button)->scalar_destruct(1);
    }
    if ((Button *)happy_button != (Button *)0x0) {
      ((Button *)happy_button)->scalar_destruct(1);
    }
    if ((Button *)view_button != (Button *)0x0) {
      ((Button *)view_button)->scalar_destruct(1);
    }
    if (((u32)(iptr)exit_button) != 0) {
      if (exit_button != (Button *)0x0) {
        exit_button->scalar_destruct(1);
      }
      (exit_button) = 0;
    }
    if ((SpriteButton *)prev_city_button != (SpriteButton *)0x0) {
      ((SpriteButton *)prev_city_button)->scalar_destruct(1);
    }
    if ((SpriteButton *)next_city_button != (SpriteButton *)0x0) {
      ((SpriteButton *)next_city_button)->scalar_destruct(1);
    }
    if (improvements_scrollbar != (Scrollbar *)0x0) {
      improvements_scrollbar->scalar_destruct(1);
    }
    clear_controls();
  }
  return;
}

/* 0050ca8d auto */
/* WARNING: Removing unreachable block (ram,0x0050cc02) */

void CityWindow::create_exit_button(int show)

{
  Button *this_00;
  iptr height;
  iptr width;
  int scaled;
  i32 dy;
  int top;
  Button *button;
  i8 rect[16];
  i32 eh_next;
  
  if (exit_button != 0) {
    if (exit_button != (Button *)0x0) {
      exit_button->scalar_destruct(1);
    }
    exit_button_b0[0] = '\0';
    exit_button_byte1[0] = '\0';
    exit_button_byte2[0] = '\0';
    exit_button_byte3[0] = '\0';
  }
  this_00 = (Button *)operator_new(0x3c);
  if (this_00 == (Button *)0x0) {
    button = 0;
  }
  else {
    button = button_ctor(this_00);
  }
  exit_button = button;
  ui_set_button_font(((GameFont *)(u8 *)&city_font));
  height = scale(0x18);
  width = scale(0x39);
  scaled = scale(0x16c);
  top = content_offset_y + scaled + client_y;
  scaled = scale(0x1cb);
  rect_set_xywh((RECT *)rect,client_x + scaled + margin_x_2,
                     top,width,height);
  dy = scale(0x19);
  scaled = scale(0x3a);
  top = scale(0x3a);
  rect_offset((RECT *)rect,scaled + top,dy);
  height = (iptr)label_string(label_table.string_ids[0x25 + (-(uint)(notice_mode == 2) & 0xcc)]);
  exit_button->create((Wnd *)&city_wnd.wnd,8,(RECT *)rect,(const char *)height);
  exit_button->set_cancel_button();
  exit_button->set_callback(city_window_exit_clicked);
  if (show != 0) {
    exit_button->show_and_redraw();
  }
  return;
}

/* 0050ccb3 auto */
/* WARNING: Removing unreachable block (ram,0x0050ce6a) */

void CityWindow::create_change_button(int show)

{
  int origin_x;
  u32 cx;
  u32 cy;
  Button *this_00;
  iptr height;
  iptr width;
  int top;
  int origin_y;
  i32 dx;
  i32 dy;
  Button *button;
  i8 rect[16];
  char auto_mode;
  i32 eh_next;
  
  auto_mode = (cities[city_index].flags & CITYF_AUTOBUILD) != 0;
  if (city_wnd_state.change_button_variant != auto_mode) {
    city_wnd_state.change_button_variant = auto_mode;
    if (change_button != 0) {
      if (change_button != (Button *)0x0) {
        change_button->scalar_destruct(1);
      }
      change_button_b0[0] = '\0';
      change_button_byte1[0] = '\0';
      change_button_byte2[0] = '\0';
      change_button_byte3[0] = '\0';
    }
    this_00 = (Button *)operator_new(0x3c);
    if (this_00 == (Button *)0x0) {
      button = 0;
    }
    else {
      button = button_ctor(this_00);
    }
    change_button = button;
    ui_set_button_font(((GameFont *)(u8 *)&city_font));
    origin_x = margin_x_2;
    cx = client_x;
    cy = client_y;
    origin_y = content_offset_y;
    height = scale(0x18);
    width = scale(0x44);
    top = scale(0xb5);
    top = cy + origin_y + top;
    origin_y = scale(0x1ba);
    rect_set_xywh((RECT *)rect,origin_x + cx + origin_y,top,width,height);
    dy = 0;
    dx = scale(0x73);
    rect_offset((RECT *)rect,dx,dy);
    height = (iptr)label_string(label_table.string_ids[0x144 + (-(uint)(auto_mode == '\0') & 0xfffffee0)]);
    change_button->create((Wnd *)&city_wnd.wnd,2,(RECT *)rect,(const char *)height);
    change_button->set_callback(city_button_change);
    if (show != 0) {
      change_button->show_and_redraw();
    }
  }
  return;
}

/* 0050cf06 auto */
/* WARNING: Removing unreachable block (ram,0x0050d785) */
/* WARNING: Removing unreachable block (ram,0x0050d64a) */
/* WARNING: Removing unreachable block (ram,0x0050d52c) */
/* WARNING: Removing unreachable block (ram,0x0050d3ea) */
/* WARNING: Removing unreachable block (ram,0x0050d307) */
/* WARNING: Removing unreachable block (ram,0x0050d471) */
/* WARNING: Removing unreachable block (ram,0x0050d5bb) */
/* WARNING: Removing unreachable block (ram,0x0050d6f6) */
/* WARNING: Removing unreachable block (ram,0x0050d871) */

void CityWindow::create_controls()

{
  iptr height;
  iptr width;
  int top;
  int left_offset;
  i32 offset;
  int scaled;
  int scaled2;
  int scroll_top;
  i32 offset2;
  i8 mask_rect[16];
  Scrollbar *scrollbar;
  Scrollbar *scrollbar_mem;
  SpriteButton *next_btn;
  SpriteButton *next_btn_mem;
  SpriteButton *prev_btn;
  SpriteButton *prev_btn_mem;
  Button *view_btn;
  Button *view_btn_mem;
  Button *happy_btn;
  Button *happy_btn_mem;
  Button *rename_btn;
  Button *rename_btn_mem;
  Button *map_btn;
  Button *map_btn_mem;
  Button *info_btn;
  Button *info_btn_mem;
  Button *change_btn;
  Button *change_btn_mem;
  Button *buy_btn;
  Button *buy_btn_mem;
  int vscroll_w;
  int origin_y2;
  int origin_x2;
  i8 icon_char;
  i8 icon_char_end;
  i8 rect[16];
  i32 eh_next;
  
  window.set_keyboard_nav(0);
  destroy_controls();
  buy_btn_mem = (Button *)operator_new(0x3c);
  if (buy_btn_mem == (Button *)0x0) {
    buy_btn = 0;
  }
  else {
    buy_btn = button_ctor(buy_btn_mem);
  }
  buy_button = (i32)buy_btn;
  change_btn_mem = (Button *)operator_new(0x3c);
  if (change_btn_mem == (Button *)0x0) {
    change_btn = 0;
  }
  else {
    change_btn = button_ctor(change_btn_mem);
  }
  change_button = change_btn;
  info_btn_mem = (Button *)operator_new(0x3c);
  if (info_btn_mem == (Button *)0x0) {
    info_btn = 0;
  }
  else {
    info_btn = button_ctor(info_btn_mem);
  }
  info_button = (i32)info_btn;
  map_btn_mem = (Button *)operator_new(0x3c);
  if (map_btn_mem == (Button *)0x0) {
    map_btn = 0;
  }
  else {
    map_btn = button_ctor(map_btn_mem);
  }
  map_button = (i32)map_btn;
  rename_btn_mem = (Button *)operator_new(0x3c);
  if (rename_btn_mem == (Button *)0x0) {
    rename_btn = 0;
  }
  else {
    rename_btn = button_ctor(rename_btn_mem);
  }
  rename_button = (i32)rename_btn;
  happy_btn_mem = (Button *)operator_new(0x3c);
  if (happy_btn_mem == (Button *)0x0) {
    happy_btn = 0;
  }
  else {
    happy_btn = button_ctor(happy_btn_mem);
  }
  happy_button = (i32)happy_btn;
  view_btn_mem = (Button *)operator_new(0x3c);
  if (view_btn_mem == (Button *)0x0) {
    view_btn = 0;
  }
  else {
    view_btn = button_ctor(view_btn_mem);
  }
  view_button = (i32)view_btn;
  prev_btn_mem = (SpriteButton *)operator_new(0xcc);
  if (prev_btn_mem == (SpriteButton *)0x0) {
    prev_btn = 0;
  }
  else {
    prev_btn = prev_btn_mem->construct();
  }
  prev_city_button = (i32)prev_btn;
  next_btn_mem = (SpriteButton *)operator_new(0xcc);
  if (next_btn_mem == (SpriteButton *)0x0) {
    next_btn = 0;
  }
  else {
    next_btn = next_btn_mem->construct();
  }
  next_city_button = (i32)next_btn;
  scrollbar_mem = (Scrollbar *)operator_new(0x40);
  if (scrollbar_mem == (Scrollbar *)0x0) {
    scrollbar = 0;
  }
  else {
    scrollbar = scrollbar_mem->construct();
  }
  improvements_scrollbar = scrollbar;
  origin_x2 = margin_x_2 + client_x;
  origin_y2 = client_y + content_offset_y;
  ui_set_button_font(((GameFont *)(u8 *)&city_font));
  create_exit_button(0);
  height = scale(0x18);
  width = scale(0x44);
  top = scale(0xb5);
  top = origin_y2 + top;
  left_offset = scale(0x1ba);
  rect_set_xywh((RECT *)rect,origin_x2 + left_offset,top,width,height);
  height = (iptr)label_string(label_table.string_ids[0x23]);
  ((Button *)buy_button)->create((Wnd *)&city_wnd.wnd,1,(RECT *)rect,(const char *)height);
  ((Button *)buy_button)->set_callback(city_button_buy);
  city_wnd_state.change_button_variant = 0xff;
  create_change_button(0);
  height = scale(0x18);
  width = scale(0x15);
  top = scale(0x16c);
  top = origin_y2 + top;
  left_offset = scale(0x1b5);
  rect_set_xywh((RECT *)rect,origin_x2 + left_offset,top,width,height);
  icon_char_end = 0;
  icon_char = 0x5e;
  spritebtn_create_icon_button((WndBase *)&city_wnd.wnd,9,(SpriteButton *)prev_city_button,(Sprite *)&scroll_up_button_sprite,(RECT *)rect);
  ((SpriteButton *)prev_city_button)->set_click_handler(city_button_prev_city);
  offset = scale(0x19);
  rect_offset((RECT *)rect,0,offset);
  icon_char = 0x76;
  spritebtn_create_icon_button((WndBase *)&city_wnd.wnd,10,(SpriteButton *)next_city_button,(Sprite *)&city_down_arrow_sprite_block.city_down_arrow_sprite,(RECT *)rect);
  ((SpriteButton *)next_city_button)->set_click_handler(city_button_next_city);
  height = scale(0x18);
  width = scale(0x39);
  top = scale(0x16c);
  top = origin_y2 + top;
  left_offset = scale(0x1cb);
  rect_set_xywh((RECT *)rect,origin_x2 + left_offset,top,width,height);
  height = (iptr)label_string(label_table.string_ids[0x27]);
  ((Button *)info_button)->create((Wnd *)&city_wnd.wnd,3,(RECT *)rect,(const char *)height);
  ((Button *)info_button)->set_callback(city_button_info);
  offset2 = 0;
  offset = scale(0x3a);
  rect_offset((RECT *)rect,offset,offset2);
  height = (iptr)label_string(label_table.string_ids[0x29]);
  ((Button *)map_button)->create((Wnd *)&city_wnd.wnd,4,(RECT *)rect,(const char *)height);
  ((Button *)map_button)->set_callback(city_button_map);
  offset2 = 0;
  offset = scale(0x3a);
  rect_offset((RECT *)rect,offset,offset2);
  height = (iptr)label_string(label_table.string_ids[0x2a]);
  ((Button *)rename_button)->create((Wnd *)&city_wnd.wnd,5,(RECT *)rect,(const char *)height);
  ((Button *)rename_button)->set_callback(city_button_rename);
  offset = scale(0x19);
  top = scale(0xffffffc6);
  left_offset = scale(0xffffffc6);
  rect_offset((RECT *)rect,top + left_offset,offset);
  height = (iptr)label_string(label_table.string_ids[0x28]);
  ((Button *)happy_button)->create((Wnd *)&city_wnd.wnd,6,(RECT *)rect,(const char *)height);
  ((Button *)happy_button)->set_callback(city_button_happy);
  offset2 = 0;
  offset = scale(0x3a);
  rect_offset((RECT *)rect,offset,offset2);
  height = (iptr)label_string(label_table.string_ids[0x26]);
  ((Button *)view_button)->create((Wnd *)&city_wnd.wnd,7,(RECT *)rect,(const char *)height);
  ((Button *)view_button)->set_callback(city_button_view);
  vscroll_w = GetSystemMetrics(2);
  left_offset = scale(0x83);
  left_offset -= 2;
  top = vscroll_w;
  scaled = scale(0x122);
  scroll_top = origin_y2 + scaled + 1;
  scaled = scale(0xc0);
  scaled2 = scale(0);
  rect_set_xywh(((RECT *)improvements_scrollbar_rect),(scaled + scaled2 + origin_x2) - (vscroll_w + 1),
                     scroll_top,top,left_offset);
  improvements_scrollbar->create((Wnd *)&city_wnd.wnd,99,
             ((RECT *)improvements_scrollbar_rect),1);
  top = rect_height((RECT *)&improvement_list_rect);
  left_offset = scale(1);
  scaled = scale(0xc);
  improvements_scrollbar->set_page_size((top - (left_offset >> 1)) / scaled);
  improvements_scrollbar->set_callback(city_window_on_improvement_scroll);
  improvements_scrollbar->set_track_callback(city_window_on_improvement_scroll);
  top = view_scale;
  if (top == 1) {
    map_zoom_level_b0[0] = 0xfb;
    map_zoom_level_byte1[0] = 0xff;
    map_zoom_level_byte2[0] = 0xff;
    map_zoom_level_byte3[0] = 0xff;
  }
  else if (top == 2) {
    map_zoom_level_b0[0] = 0xfe;
    map_zoom_level_byte1[0] = 0xff;
    map_zoom_level_byte2[0] = 0xff;
    map_zoom_level_byte3[0] = 0xff;
  }
  else if (top == 3) {
    map_zoom_level_b0[0] = 1;
    map_zoom_level_byte1[0] = '\0';
    map_zoom_level_byte2[0] = '\0';
    map_zoom_level_byte3[0] = '\0';
  }
  if (map_zoom_level != city_wnd_state.tile_pick_zoom_cached) {
    city_wnd_state.tile_pick_zoom_cached = map_zoom_level;
    offset = gfx_zoom_scale(0x20,map_zoom_level);
    offset2 = gfx_zoom_scale(0x40,map_zoom_level);
    ((DibSurface *)((i8 *)&city_tile_pick_surface))->DibSurface::create(offset2,offset);
    gfx_set_zoom_scale(map_zoom_level);
    ((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite))->draw((RECT *)mask_rect,&city_tile_pick_surface,0,0);
    gfx_reset_zoom_scale();
  }
  return;
}

/* 0050dada auto */
void CityWindow::begin_notice()

{
  if (((u32)is_closed) == 0) {
    (notice_mode) = 2;
  }
  else {
    (notice_mode) = 1;
  }
  if (((u32)is_closed) == 0) {
    city_wnd_state.exit_btn_pending = 1;
  }
  return;
}

/* 0050db36 auto */
void CityWindow::end_notice()

{
  int notice_mode;
  
  notice_mode = this->notice_mode;
  notice_mode_b0[0] = '\0';
  notice_mode_b1[0] = '\0';
  notice_mode_b2[0] = '\0';
  notice_mode_b3[0] = '\0';
  if (notice_mode == 2) {
    create_exit_button(1);
  }
  city_wnd_state.exit_btn_pending = 0;
  return;
}

/* 0050db92 auto */
void __cdecl city_window_on_size(void)

{
  ((WndBase *)((i8 *)&city_wnd.wnd))->get_rect_in_parent((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  return;
}

/* 0050dbb8 auto */
void __cdecl city_window_on_move(void)

{
  ((WndBase *)((i8 *)&city_wnd.wnd))->get_rect_in_parent((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  return;
}

/* 0050dbde auto */
void __cdecl city_window_on_caption_button(int button)

{
  if (button == 1) {
    city_window_exit_clicked(0);
  }
  else if (button == 2) {
    if (1 < city_wnd.size_level) {
      city_wnd.size_level--;
      city_window_calc_rect();
      ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
    }
  }
  else if ((button == 3) && ((city_wnd.size_level < 2 || ((city_wnd.size_level < 3 && (999 < caption_font_block.screen_width)))))) {
    city_wnd.size_level++;
    city_window_calc_rect();
    ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  }
  return;
}

/* 0050dcb6 auto */
void CityWindow::create()

{
  int height;
  int width;
  i32 title;
  i32 style;
  i32 left;
  i32 top;
  i32 extra_arg1;
  i32 extra_arg2;
  i32 extra_arg3;
  
  reset_state();
  notice_mode_b0[0] = '\0';
  notice_mode_b1[0] = '\0';
  notice_mode_b2[0] = '\0';
  notice_mode_b3[0] = '\0';
  content_offset_y_b0[0] = '\0';
  content_offset_y_byte1[0] = '\0';
  content_offset_y_byte2[0] = '\0';
  content_offset_y_byte3[0] = '\0';
  margin_x_2 = content_offset_y;
  prev_content_offset_y_b0[0] = 0xff;
  prev_content_offset_y_byte1[0] = 0xff;
  prev_content_offset_y_byte2[0] = 0xff;
  prev_content_offset_y_byte3[0] = 0xff;
  prev_content_offset_x = prev_content_offset_y;
  redraw_suspended_b0[0] = '\0';
  redraw_suspended_b1[0] = '\0';
  redraw_suspended_b2[0] = '\0';
  redraw_suspended_b3[0] = '\0';
  if ((turn_loop.resume_civ == '\0') || (destroyed_civ_count_block.saved_layout_valid == 0)) {
    city_window_init_size();
  }
  extra_arg3 = 8;
  extra_arg2 = 0x18;
  extra_arg1 = 0;
  height = rect_height((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  height -= ui_metrics.dlgframe_h2;
  width = rect_width((RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  width -= ui_metrics.dlgframe_w2;
  style = 2;
  left = minimap_wnd_rect_block.city_wnd_rect;
  top = minimap_wnd_rect_block.city_wnd_rect_top;
  title = (iptr)label_string(label_table.string_ids[0x7]);
  this->FrameWindow::create((const char *)title,style,left,top,width,height,extra_arg1,extra_arg2,extra_arg3);
  set_title_font((GameFont *)&city_window_font);
  set_title_small_font(&city_window_small_font);
  add_caption_button((Sprite *)&caption_close_button_sprite_block.caption_close_button_sprite,1);
  add_caption_button((Sprite *)&caption_zoom_out_sprite,2);
  add_caption_button((Sprite *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite,3);
  set_caption_button_handler(city_window_on_caption_button);
  clear_controls();
  window.wnd_handlers.set_close_handler(city_window_close);
  set_redraw_callback(city_window_on_redraw);
  window.wnd_handlers.set_minimize_handler(city_window_on_minimize);
  window.wnd_handlers.set_restore_handler(city_window_on_restore);
  window.wnd_handlers.set_lbuttonup_handler(city_window_on_lbuttonup);
  window.wnd_handlers.set_rbuttonup_handler(city_window_on_rbuttonup);
  window.wnd_handlers.set_keydown_handler(map_window_key);
  window.wnd_handlers.set_char_handler((WndKeyFn)map_window_ascii);
  window.wnd_handlers.set_size_handler((WndSizeFn)city_window_on_size);
  window.wnd_handlers.set_move_handler((WndMouseFn)city_window_on_move);
  return;
}

/* 0050dea8 auto */
void CityWindow::destroy()

{
  (redraw_suspended) = 1;
  destroy_controls();
  wnd_destroy_with_surface();
  return;
}

/* 005113b0 auto */
void __cdecl
city_draw_hline(i32 x,i32 y,i32 length,i32 color)

{
  gfx_draw_hline(gfx_draw_target,x,y,length,color);
  return;
}

/* 005113f0 auto */
void __cdecl city_draw_bevel_rect(RECT *rect,i32 light_color,i32 dark_color)

{
  gfx_draw_bevel_rect(gfx_draw_target,rect,light_color,dark_color);
  return;
}

/* 005114d0 auto */
void __cdecl
city_window_draw_icon_row(Sprite *sprite,i32 count,i32 x,i32 y,
            i32 width,i32 spacing,i32 max_width,i32 flags)

{
  sprite_draw_spaced_row(gfx_draw_target,sprite,count,x,y,width,spacing,max_width,flags);
  return;
}

/* 00511690 auto */
int CityWindow::scale(int value)

{
  if (((u32)view_scale) != 2) {
    value = (int)(((u32)view_scale) * value) / 2;
  }
  return value;
}
