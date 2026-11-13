/* The game, the status window: the date, gold, research and the active unit or tile: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

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
void __cdecl status_draw_civ_color(int civ)

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
    gfx_fill_rect(&status_wnd.window,&rect,color);
    win_flush_pending_msgs();
    ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((RECT *)&rect.left);
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
    text_set_surface(&status_wnd.window);
    ((DibSurface *)((i8 *)&status_wnd.window))->set_clip((RECT *)&frame_texture_surface_block.status_end_turn_rect);
    src_y = 0;
    src_x = 0;
    height = rect_height((RECT *)&frame_texture_surface_block.status_end_turn_rect);
    width = rect_width((RECT *)&frame_texture_surface_block.status_end_turn_rect);
    gfx_tile_fill(&status_wnd.window,&panel_texture_surface,frame_texture_surface_block.status_end_turn_rect,frame_texture_surface_block.status_end_turn_rect_top,width,height,src_x,src_y);
    text_set_colors((-(uint)((moving_units_mode == 0) == (bool)blink.end_turn_phase) & 0xf) + 0x1a,10,1,0);
    text_set_font(&status_font_block.status_font);
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
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((RECT *)&frame_texture_surface_block.status_end_turn_rect);
      win_flush_pending_msgs();
    }
    ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
  }
  return;
}

/* 0056911d auto */
i32 __cdecl status_draw_cursor_loc(int x,int y,i32 draw_x,int draw_y)

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
void __cdecl status_append_unit_orders(int unit)

{
  iptr orders;
  uint improvements2;
  iptr city;
  
  orders = (iptr)units[unit].orders;
  if ((orders == ORDER_NONE) || (0xf < orders)) {
    text_append_label(0x1a);
  }
  else if (orders == ORDER_BUILD_ROAD) {
    improvements2 = tile_improvements(units[unit].x,units[unit].y);
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
      city = city_at((int)units[unit].goto_x,units[unit].goto_y);
      if ((((-1 < city) && (game.reveal_map == '\0')) &&
          ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) == 0)) &&
         ((int)cities[city].owner != (uint)(byte)(*(i8 *)&human_civ))) {
        city = -1;
      }
      if (city < 0) {
        text_append_lparen();
        text_append_int(units[unit].goto_x);
        text_append_comma();
        text_append_int(units[unit].goto_y);
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
    gfx_set_draw_target(&status_wnd.window);
    src_y = 0;
    src_x = 0;
    height = rect_height((RECT *)&frame_texture_surface_block.status_upper_rect);
    width = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
    gfx_tile_fill(&status_wnd.window,&panel_texture_surface,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect_top,width,height,src_x,src_y);
  }
  text_set_surface(&status_wnd.window);
  text_set_font((GameFont *)&status_wnd.info_font);
  panel_width2 = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
  if (1 < game.peace_turns) {
    percent = int_clamp(game.peace_turns * 3,0,100);
    bar_width = (percent * panel_width2) / 100;
    if (bar_width != 0) {
      gfx_draw_hline(&status_wnd.window,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect + bar_width - 1,frame_texture_surface_block.status_upper_rect_top,0x55);
      gfx_draw_hline(&status_wnd.window,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect + bar_width - 1,frame_texture_surface_block.status_upper_rect_top + 1,0x55);
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
    text_append_int(game.turn);
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
  text_append_int(civs[human_civ[0]].tax_rate);
  crt_strcat(((i8 *)&textbuf.buf),((i8 *)&status_tax_sep_str));
  text_append_int(10 - ((uint)civs[human_civ[0]].science_rate +
                          (uint)civs[human_civ[0]].tax_rate));
  crt_strcat(((i8 *)&textbuf.buf),((i8 *)&status_lux_sep_str));
  text_append_int(civs[human_civ[0]].science_rate);
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
  ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites) + stage * 0x3c))->draw((RECT *)flask_rect,&status_wnd.window,icon_x,
             icon_y);
  icon_x += (icon_width * 3) / 2;
  if (game.warming_counter != '\0') {
    stage = int_clamp((int)game.warming_counter >> 2,0,3);
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites) + stage * 0x3c))->draw((RECT *)warming_rect,&status_wnd.window,icon_x,
               icon_y);
  }
  gfx_reset_zoom_scale();
  ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
  if (full_redraw != 0) {
    win_flush_pending_msgs();
    ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((RECT *)&frame_texture_surface_block.status_upper_rect);
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
    str = civ_get_adjective(units[unit].owner);
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
    terrain = ((Tile *)value)->improvements;
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

  gfx_draw_unit(&status_wnd.window,unit2,4,text_x,*text_y2,zoom - 8,0);
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
    text_append_int(cosmic.road_move_mult);
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
  str = civ_get_adjective(units[unit2].owner);
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
    text_set_font(&status_font_block.status_font);
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
          gfx_draw_unit(&status_wnd.window,unit,4,text_x,text_y2,zoom - 8,0);
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
    gfx_set_draw_target(&status_wnd.window);
    src_y = 0;
    src_x = 0;
    height = rect_height((RECT *)&frame_texture_surface_block.status_upper_rect);
    width = rect_width((RECT *)&frame_texture_surface_block.status_upper_rect);
    gfx_tile_fill(&status_wnd.window,&panel_texture_surface,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect_top,width,height,src_x,src_y);
    if (status_font_block.status_lower_panel_height != 0) {
      src_y = 0;
      src_x = 0;
      height = rect_height((RECT *)&status_wnd.lower_rect);
      width = rect_width((RECT *)&status_wnd.lower_rect);
      gfx_tile_fill(&status_wnd.window,&panel_texture_surface,status_wnd.lower_rect,status_wnd.lower_rect_top,width,height,src_x,src_y);
    }
    status_calc_font_sizes();
    status_draw_unit_panel();
    if (blit != 0) {
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect((RECT *)&frame_texture_surface_block.status_upper_rect);
      rect2.left = status_wnd.lower_rect;
      rect2.top = status_wnd.lower_rect_top;
      rect2.right = status_wnd.lower_rect_right;
      rect2.bottom = frame_texture_surface_block.status_text_bottom;
      if (frame_texture_surface_block.status_text_bottom <= old_bottom) {
        rect2.bottom = old_bottom;
      }
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect(&rect2);
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
    gfx_set_draw_target(&status_wnd.window);
    rect_set_xywh((RECT *)&frame_texture_surface_block.status_upper_rect,(uint)(*(byte *)&status_font_block.status_panel_x),0,status_font_block.status_panel_width,status_wnd.upper_panel_height);
    rect_set_xywh((RECT *)&status_wnd.lower_rect,(uint)(*(byte *)&status_font_block.status_panel_x),0,status_font_block.status_panel_width,status_font_block.status_lower_panel_height);
    rect_offset((RECT *)&frame_texture_surface_block.status_upper_rect,status_wnd.client_x,status_wnd.client_y);
    rect_offset((RECT *)&status_wnd.lower_rect,status_wnd.client_x,(uint)(*(byte *)&status_wnd.panel_gap) + frame_texture_surface_block.status_upper_rect_bottom);
    status_redraw(0);
    ((FrameWindow *)((i8 *)&status_wnd.window))->FrameWindow::draw_caption();
    ((DibSurface *)((i8 *)&status_wnd.window))->reset_clip();
    if (status_font_block.status_lower_panel_height != 0) {
      rect_set(&bevel_rect,frame_texture_surface_block.status_upper_rect,frame_texture_surface_block.status_upper_rect_bottom,frame_texture_surface_block.status_upper_rect_right,status_wnd.lower_rect_top);
      rect_inflate(&bevel_rect,ui_metrics.bevel_width,0);
      inner_rect.left = bevel_rect.left;
      inner_rect.top = bevel_rect.top;
      inner_rect.right = bevel_rect.right;
      inner_rect.bottom = bevel_rect.bottom;
      rect_deflate(&inner_rect,0,ui_metrics.bevel_width);
      src_y = 0;
      src_x = 0;
      height = rect_height(&inner_rect);
      width = rect_width(&inner_rect);
      gfx_tile_fill(&status_wnd.window,&window_border_tile_surface,inner_rect.left,inner_rect.top,width,height,src_x,src_y);
      bevel_rect.bottom = bevel_rect.top + ui_metrics.bevel_width;
      gfx_fill_rect(&status_wnd.window,&bevel_rect,ui_metrics.highlight_color);
      inner_height = rect_height(&inner_rect);
      rect_offset(&bevel_rect,0,ui_metrics.bevel_width + inner_height);
      gfx_fill_rect(&status_wnd.window,&bevel_rect,ui_metrics.shadow_color);
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
  rect_set_xywh((RECT *)&minimap_wnd_rect_block.status_wnd_rect,left,top,width,(client_height - top) - 1 + ui_metrics.dlgframe_h2);
  return;
}

/* 0056aa7f auto */
void __cdecl status_wnd_on_size(void)

{
  ((WndBase *)((i8 *)&status_wnd.wnd))->get_rect_in_parent((RECT *)&minimap_wnd_rect_block.status_wnd_rect);
  return;
}

/* 0056aaa5 auto */
void __cdecl status_wnd_on_move(void)

{
  ((WndBase *)((i8 *)&status_wnd.wnd))->get_rect_in_parent((RECT *)&minimap_wnd_rect_block.status_wnd_rect);
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
      ((DibSurface *)((i8 *)&status_wnd.timer_bg_surface))->alloc_dib8(&bar_rect);
      height = rect_height(&bar_rect);
      bar_width = rect_width(&bar_rect);
      gfx_copy_rect_xywh(&status_wnd.window,&status_wnd.timer_bg_surface,bar_rect.left,bar_rect.top,0,0,bar_width,height);
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
      gfx_copy_rect_xywh(&status_wnd.timer_bg_surface,&status_wnd.window,0,0,bar_rect.left,bar_rect.top,bar_width,height);
      ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect(&bar_rect);
      if (-1 < color) {
        width = rect_width(&bar_rect);
        bar_rect.right = bar_rect.left + (percent * width) / 100;
        gfx_fill_rect(&status_wnd.window,&bar_rect,color);
        win_flush_pending_msgs();
        ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface_rect(&bar_rect);
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
