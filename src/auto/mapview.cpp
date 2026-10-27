/* The game, the map windows: drawing, scrolling, the mini map: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00406a80 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl minimap_window_global_init(void)

{
  minimap_window_static_init();
  minimap_window_reg_atexit();
  return;
}

/* 00406a9a auto */
void __cdecl minimap_window_static_init(void)

{
  frame_wnd_ctor((FrameWindow *)((i8 *)&minimap_window_block.minimap_window));
  return;
}

/* 00406ab4 auto */
void __cdecl minimap_window_reg_atexit(void)

{
  _atexit(minimap_window_atexit_dtor);
  return;
}

/* 00406ad1 auto */
void __cdecl minimap_window_atexit_dtor(void)

{
  frame_wnd_dtor((FrameWindow *)((i8 *)&minimap_window_block.minimap_window));
  return;
}

/* 00406aeb auto */
void __cdecl minimap_on_minimize(void)

{
  minimap_hidden = 1;
  return;
}

/* 00406b02 auto */
void __cdecl minimap_on_restore(void)

{
  city_window_bring_to_top((CityWindow *)((i8 *)&city_window_block.city_window));
  minimap_hidden = 0;
  minimap_redraw();
  return;
}

/* 00406b28 auto */
i32 __cdecl minimap_on_close(void)

{
  minimap_hidden = 1;
  wnd_minimize((WndBase *)((i8 *)&minimap_window_block.minimap_wnd));
  return 0;
}

/* 00406b4c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl minimap_calc_layout(void)

{
  int client_h;
  int tile_size;
  int tile_h;
  
  client_h = minimap_window_block.minimap_client_height;
  tile_size = minimap_window_block.minimap_client_width / (int)map.width;
  if (tile_size < 2) {
    tile_size = 1;
  }
  tile_h = minimap_window_block.minimap_client_height / (int)map.height;
  if (tile_h < 2) {
    tile_h = 1;
  }
  if (tile_h <= tile_size) {
    tile_size = tile_h;
  }
  minimap_tile_height = tile_size;
  if (tile_h <= tile_size) {
    minimap_tile_height = tile_h;
  }
  minimap_tile_width = tile_size * 2;
  tile_size = (minimap_window_block.minimap_client_width - minimap_tile_width / 2) / minimap_tile_width;
  minimap_view_cols = (int)map.width / 2;
  if (tile_size <= (int)map.width / 2) {
    minimap_view_cols = tile_size;
  }
  minimap_view_rows = (int)map.height;
  if (minimap_window_block.minimap_client_height / minimap_tile_height <= (int)map.height) {
    minimap_view_rows = minimap_window_block.minimap_client_height / minimap_tile_height;
  }
  tile_size = int_clamp(minimap_window_block.minimap_client_width - minimap_view_cols * minimap_tile_width,0,999);
  minimap_window_block.minimap_margin_y_i32 = int_clamp(client_h - minimap_view_rows * minimap_tile_height,0,999);
  minimap_window_block.minimap_margin_x = tile_size >> 1;
  minimap_window_block.minimap_margin_y_i32 = minimap_window_block.minimap_margin_y_i32 >> 1;
  if (minimap_view_rows < map.height) {
    (*(uint *)&minimap_origin_y) = (int)map_views_block.mapview_center_y[0] - (minimap_view_rows >> 1);
    (*(uint *)&minimap_origin_y) = int_clamp((*(uint *)&minimap_origin_y),0,map_views_block.mapview_layout_height - minimap_view_rows);
  }
  else {
    (*(uint *)&minimap_origin_y) = 0;
  }
  (*(uint *)&minimap_origin_x) = map_wrap_x(map_views_block.map_view_center_x[0] - minimap_view_cols);
  if ((game.rules & 0x8000) != 0) {
    if (minimap_view_cols < map.width) {
      (*(uint *)&minimap_origin_x) = int_clamp((*(uint *)&minimap_origin_x),0,map_views_block.mapview_layout_width - minimap_view_cols);
    }
    else {
      (*(uint *)&minimap_origin_x) = 0;
    }
  }
  if (((*(uint *)&minimap_origin_x) & 1) != 0) {
    (*(uint *)&minimap_origin_x)--;
  }
  if (((*(uint *)&minimap_origin_y) & 1) != 0) {
    (*(uint *)&minimap_origin_x)++;
  }
  return;
}

/* 00406db8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl minimap_tile_to_screen(int x,uint y,iptr out_x,iptr out_y)

{
  int dx;
  
  dx = x - minimap_origin_x;
  if (((game.rules & 0x8000) == 0) && (dx < 0)) {
    dx += map.width;
  }
  *(uint *)out_y = (y - minimap_origin_y) * minimap_tile_height + minimap_window_block.minimap_margin_y_i32;
  *(int *)out_x = (dx >> 1) * minimap_tile_width + minimap_window_block.minimap_margin_x;
  if ((y & 1) != 0) {
    *(int *)out_x = *(int *)out_x + (minimap_tile_width >> 1);
  }
  *(int *)out_x = *(int *)out_x + minimap_window_block.minimap_client_x;
  *(int *)out_y = *(int *)out_y + minimap_window_block.minimap_client_y;
  return;
}

/* 00406e61 auto */
i32 __cdecl minimap_tile_color(int x,int y)

{
  iptr in_bounds;
  iptr visible;
  int city;
  i32 color2;
  
  in_bounds = map_in_bounds(x,y);
  if (in_bounds == 0) {
    color2 = 10;
  }
  else {
    visible = tile_visible_to(x,y,(*(i8 *)&human_civ));
    if ((visible == 0) && (game.reveal_map == '\0')) {
      color2 = 10;
    }
    else {
      city = city_at(x,y);
      if ((city < 0) ||
         (((game.reveal_map == '\0' &&
           ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) == 0)) &&
          ((int)cities[city].owner != ((*(i8 *)&human_civ) & 0xffU))))) {
        if ((((unit_blink_active == '\0') || ((moving_units_mode == 0) != (bool)unit_blink_phase)) ||
            (map_cursor_x[0] != x)) || (map_cursor_y[0] != y)) {
          city = tile_is_ocean(x,y);
          if (city == 0) {
            color2 = 0x30;
          }
          else {
            color2 = 0x5d;
          }
        }
        else {
          color2 = 0x29;
        }
      }
      else {
        color2 = civ_primary_colors_block.civ_colors[cities[city].owner].secondary;
      }
    }
  }
  return color2;
}

/* 0040701e auto */
void __cdecl minimap_draw_view_rect(void)

{
  int bottom;
  i32 top;
  int right;
  int left;
  i8 rect[16];
  
  if ((map_views_block.mapview_margin_x == 0) && (map_views_block.mapview_margin_y == 0)) {
    minimap_tile_to_screen(map_views_block.mapview_origin_x[0],map_views_block.mapview_origin_y[0],(iptr)&left,(iptr)&top);
    minimap_tile_to_screen(map_views_block.mapview_end_x,map_views_block.mapview_end_y,(iptr)&right,(iptr)&bottom);
    if (left < right) {
      rect_set((iptr)rect,left,top,right + 1,bottom + 1);
      gfx_target_frame_rect((RECT *)rect,0x29);
    }
    else {
      rect_set((iptr)rect,right,top,left + 1,bottom + 1);
      gfx_target_frame_rect((RECT *)rect,0x29);
    }
  }
  return;
}

/* 0040733c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl minimap_redraw(void)

{
  uint y;
  int in_bounds;
  i32 color;
  int row_offset;
  int row;
  int col;
  uint x2;
  int screen_y;
  int screen_x;
  
  if ((minimap_hidden == '\0') && (game_loop_active != '\0')) {
    frame_wnd_layout((FrameWindow *)((i8 *)&minimap_window_block.minimap_window));
    gfx_set_draw_target((iptr)&minimap_window_block.minimap_window);
    minimap_calc_layout();
    gfx_fill_draw_target(0);
    screen_y = minimap_window_block.minimap_client_y + minimap_window_block.minimap_margin_y_i32;
    for (row = 0; row < minimap_view_rows; row++) {
      y = row + minimap_origin_y;
      if ((y & 1) == 0) {
        row_offset = 0;
      }
      else {
        row_offset = minimap_tile_width >> 1;
      }
      screen_x = minimap_window_block.minimap_client_x + minimap_window_block.minimap_margin_x + row_offset;
      for (col = 0; col < minimap_view_cols; col++) {
        x2 = map_wrap_x(col * 2 + minimap_origin_x);
        if ((x2 & 1) != 0) {
          x2--;
        }
        if ((y & 1) != 0) {
          x2++;
        }
        in_bounds = map_in_bounds(x2,y);
        if (in_bounds != 0) {
          color = minimap_tile_color(x2,y);
          gfx_target_fill_rect(screen_x,screen_y,minimap_tile_width,minimap_tile_height,color);
          screen_x += minimap_tile_width;
        }
      }
      screen_y += minimap_tile_height;
    }
    win_flush_pending_msgs();
    minimap_draw_view_rect();
    frame_wnd_draw_caption((FrameWindow *)((i8 *)&minimap_window_block.minimap_window));
    wnd_blit_surface((DibSurface *)((i8 *)&minimap_window_block.minimap_window));
    win_flush_pending_msgs();
  }
  return;
}

/* 004074dc auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl minimap_on_lbuttonup(int mouse_x,int mouse_y)

{
  uint tile_y;
  iptr col_x;
  int in_bounds;
  int row_offset;
  uint tile_x;
  
  mouse_x -= minimap_window_block.minimap_client_x;
  mouse_y -= minimap_window_block.minimap_client_y;
  if ((((-1 < mouse_x) && (-1 < mouse_y)) && (mouse_x < minimap_window_block.minimap_client_width)) && (mouse_y < minimap_window_block.minimap_client_height))
  {
    if (city_turn_processing == 0) {
      tile_y = int_clamp(minimap_origin_y + (mouse_y - minimap_window_block.minimap_margin_y_i32) / minimap_tile_height,
                                 minimap_origin_y,minimap_origin_y + minimap_view_rows - 1);
      if ((tile_y & 1) == 0) {
        row_offset = 0;
      }
      else {
        row_offset = minimap_tile_width >> 1;
      }
      col_x = int_clamp(((mouse_x - (minimap_window_block.minimap_margin_x + row_offset)) / minimap_tile_width) * 2 +
                                 minimap_origin_x,minimap_origin_x,minimap_view_cols * 2 - 2 + minimap_origin_x);
      tile_x = map_wrap_x(col_x);
      if ((tile_x & 1) != 0) {
        tile_x--;
      }
      if ((tile_y & 1) != 0) {
        tile_x++;
      }
      in_bounds = map_in_bounds(tile_x,tile_y);
      if (in_bounds != 0) {
        mapview_center_on((MapView *)((i8 *)&map_views_block.map_views),tile_x,tile_y);
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  return;
}

/* 00407658 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl minimap_on_rbuttonup(int mouse_x,int mouse_y)

{
  uint tile_y;
  iptr col_x;
  int in_bounds;
  int row_offset;
  uint tile_x;
  int free_view;
  int view;
  
  mouse_x -= minimap_window_block.minimap_client_x;
  mouse_y -= minimap_window_block.minimap_client_y;
  if ((((-1 < mouse_x) && (-1 < mouse_y)) && (mouse_x < minimap_window_block.minimap_client_width)) && (mouse_y < minimap_window_block.minimap_client_height))
  {
    if (city_turn_processing == 0) {
      tile_y = int_clamp(minimap_origin_y + (mouse_y - minimap_window_block.minimap_margin_y_i32) / minimap_tile_height,
                                 minimap_origin_y,minimap_origin_y + minimap_view_rows - 1);
      if ((tile_y & 1) == 0) {
        row_offset = 0;
      }
      else {
        row_offset = minimap_tile_width >> 1;
      }
      col_x = int_clamp(((mouse_x - (minimap_window_block.minimap_margin_x + row_offset)) / minimap_tile_width) * 2 +
                                 minimap_origin_x,minimap_origin_x,minimap_view_cols * 2 - 2 + minimap_origin_x);
      tile_x = map_wrap_x(col_x);
      if ((tile_x & 1) != 0) {
        tile_x--;
      }
      if ((tile_y & 1) != 0) {
        tile_x++;
      }
      in_bounds = map_in_bounds(tile_x,tile_y);
      if (in_bounds != 0) {
        free_view = -1;
        for (view = 1; view < 8; view++) {
          if (map_views_block.map_view_recs[view].open == 0) {
            free_view = view;
            break;
          }
        }
        if (0 < free_view) {
          mapview_create(&map_views_block.map_view_recs[free_view]);
          mapview_center_on
                    (&map_views_block.map_view_recs[free_view],tile_x,tile_y);
        }
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  return;
}

/* 0040785b auto */
void __cdecl minimap_calc_rect(void)

{
  int client_w;
  int height2;
  int width2;
  int top;
  
  client_w = wnd_client_width(&main_frame_wnd_block.main_frame_wnd);
  width2 = 0xa0;
  height2 = 100;
  if ((1000 < main_frame_bg_surface_block.main_frame_nonclient_width + client_w) && (width2 = 0xf0, map_layout_wide != 0)) {
    height2 = 0x96;
  }
  if (height2 < map.height) {
    height2 = (int)map.height;
  }
  width2 += frame_border_width * 2;
  top = 0;
  if (map_layout_wide != 0) {
    top = minimap_margin + 1;
  }
  rect_set_xywh((iptr)&minimap_wnd_rect_block.minimap_wnd_rect,client_w - (sys_dlgframe_w2 + width2),top,width2 + sys_dlgframe_w2,
                     height2 + frame_border_width + frame_caption_height + sys_dlgframe_h2);
  return;
}

/* 0040795a auto */
void __cdecl minimap_on_size(void)

{
  wnd_get_rect_in_parent((WndBase *)((i8 *)&minimap_window_block.minimap_wnd),(iptr)&minimap_wnd_rect_block.minimap_wnd_rect);
  return;
}

/* 00407980 auto */
void __cdecl minimap_on_move(void)

{
  wnd_get_rect_in_parent((WndBase *)((i8 *)&minimap_window_block.minimap_wnd),(iptr)&minimap_wnd_rect_block.minimap_wnd_rect);
  return;
}

/* 004079a6 auto */
void __cdecl minimap_window_create(void)

{
  int wnd_height;
  int wnd_width;
  i32 title_text;
  i32 wnd_style;
  i32 wnd_x;
  i32 wnd_y;
  i32 create_arg7;
  i32 create_arg8;
  i32 create_arg9;
  
  minimap_hidden = 0;
  if ((turn_resume_civ == '\0') || (destroyed_civ_count_block.saved_layout_valid == 0)) {
    minimap_calc_rect();
  }
  create_arg9 = 0;
  create_arg8 = 0;
  create_arg7 = 6;
  wnd_height = rect_height((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  wnd_height -= sys_dlgframe_h2;
  wnd_width = rect_width((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  wnd_width -= sys_dlgframe_w2;
  wnd_style = 6;
  wnd_x = minimap_wnd_rect_block.minimap_wnd_rect;
  wnd_y = minimap_wnd_rect_block.minimap_wnd_rect_top;
  title_text = (i32)label_string(label_string_ids[0x6]);
  frame_wnd_create
            ((FrameWindow *)((i8 *)&minimap_window_block.minimap_window),(const char *)title_text,wnd_style,wnd_x,wnd_y,wnd_width,wnd_height,create_arg7,create_arg8,create_arg9);
  wnd_set_min_size((ModalMenuWindow *)((i8 *)&minimap_window_block.minimap_wnd),0x28,0x19);
  wnd_set_redraw_callback((SurfaceWindow *)((i8 *)&minimap_window_block.minimap_window),minimap_redraw);
  wnd_set_lbuttonup_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),minimap_on_lbuttonup);
  wnd_set_rbuttonup_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),minimap_on_rbuttonup);
  wnd_set_keydown_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),map_window_key);
  wnd_set_char_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),(WndKeyFn)map_window_ascii);
  wnd_set_cursor((WndBase *)((i8 *)&minimap_window_block.minimap_wnd),0x1ff);
  wnd_set_icon((WndBase *)((i8 *)&minimap_window_block.minimap_wnd),1);
  wnd_set_minimize_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),minimap_on_minimize);
  wnd_set_restore_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),minimap_on_restore);
  wnd_set_close_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),minimap_on_close);
  wnd_set_size_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),(WndSizeFn)minimap_on_size);
  wnd_set_move_handler((WndHandlers *)((i8 *)&minimap_window_block.minimap_wnd_handlers),(WndMouseFn)minimap_on_move);
  wnd_repaint((SurfaceWindow *)((i8 *)&minimap_window_block.minimap_window));
  if (game.multiplayer != 1) {
    window_show((SurfaceWindow *)((i8 *)&minimap_window_block.minimap_window));
  }
  return;
}

/* 00407b31 auto */
void __cdecl minimap_window_destroy(void)

{
  minimap_hidden = 1;
  wnd_destroy_with_surface((DibSurface *)((i8 *)&minimap_window_block.minimap_window));
  return;
}

/* 00410302 auto */
void __thiscall mapview_redraw_cursor_tile(MapView *self)

{
  mapview_redraw_area(self,(int)map_cursor_x[0],(int)map_cursor_y[0],0,human_civ[0],1);
  return;
}

/* 0041033a auto */
void __cdecl mapviews_redraw_cursor(void)

{
  int view;
  
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      mapview_redraw_cursor_tile(&map_views_block.map_view_recs[view]);
    }
  }
  return;
}

/* 004103ae auto */
void __cdecl mapview_on_redraw(void)

{
  int child_wnd;
  MapView *this2;
  
  city_window_bring_to_top((CityWindow *)((i8 *)&city_window_block.city_window));
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    this2 = (MapView *)0x0;
  }
  else {
    this2 = (MapView *)(child_wnd - 0x48);
  }
  mapview_redraw(this2,human_civ[0],1);
  return;
}

/* 00410402 auto */
void __thiscall mapview_center_on(MapView *self,iptr x,i32 y)

{
  i8 saved_blink;
  i32 wrapped_x;
  
  wrapped_x = map_wrap_x(x);
  *(short *)&self->center_x_u = (short)wrapped_x;
  *(i16 *)((int)&self->center_x_u + 2) = (i16)y;
  saved_blink = unit_blink_active;
  unit_blink_active = 0;
  mapview_redraw(self,human_civ[0],1);
  unit_blink_active = saved_blink;
  return;
}

/* 00410464 auto */
int __thiscall mapview_scroll_if_near_edge(MapView *self,int x,int y)

{
  uptr view_left;
  uptr view_top;
  u32 view_cols;
  u32 view_bottom;
  int need_scroll;
  
  need_scroll = 0;
  view_left = self->origin_x_uptr;
  view_top = self->origin_y_uptr;
  if ((((game.rules & 0x8000) == 0) && (x <= (int)(self->half_view_width * 2))) &&
     ((int)((int)map.width + self->half_view_width * -2) <= (int)view_left)) {
    x += map.width;
  }
  view_cols = self->tile_columns;
  view_bottom = self->end_y_u;
  if (self->margin_x_u == 0) {
    if ((game.rules & 0x8000) != 0) {
      x = int_clamp(x,2,map.width - 3);
    }
    if ((x <= (int)(view_left + 1)) || ((int)(view_cols * 2 + view_left - 3) <= x)) {
      need_scroll = 1;
    }
  }
  if ((self->margin_y_u == 0) &&
     ((y = int_clamp(y,2,map.height - 3), y <= (int)(view_top + 1) ||
      ((int)(view_bottom - 1) <= y)))) {
    need_scroll = 1;
  }
  if (need_scroll != 0) {
    unit_blink_skip_tick = 1;
    unit_blink_phase = moving_units_mode == 0;
    mapview_center_on(self,x,y);
  }
  return need_scroll;
}

/* 004105f8 auto */
uint __cdecl mapviews_ensure_visible(i32 x,i32 y,iptr civ)

{
  ushort track_mask;
  uint scrolled;
  uint result;
  int view;
  
  result = 1;
  net_session_block.map_redraw_busy = 1;
  if (civ < 0) {
    track_mask = 3;
  }
  else if (human_civ[0] == civ) {
    track_mask = 1;
  }
  else {
    track_mask = 2;
  }
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      if ((track_mask & map_views_block.map_view_recs[view].track_moves) == 0) {
        result = 0;
      }
      else {
        scrolled = mapview_scroll_if_near_edge
                          (&map_views_block.map_view_recs[view],x,y);
        result = result & scrolled;
      }
    }
  }
  net_session_block.map_redraw_busy = 0;
  return result;
}

/* 004106fd auto */
int __thiscall
mapview_point_unit_dir
          (MapView *self,int mouse_x,int mouse_y,i32 unit_x,i32 unit_y)

{
  short client_dx;
  short client_dy;
  int dir;
  int tile_sx[2];
  int off_y;
  int off_x;
  int tile_sy;
  
  mapview_tile_to_screen(self,(iptr)tile_sx,(iptr)&tile_sy,unit_x,unit_y);
  if (((short)mouse_x < tile_sx[0]) || ((short)mouse_y < tile_sy)) {
    dir = 8;
  }
  else if (((int)(short)mouse_x < (int)(self->tile_width_u + tile_sx[0])) &&
          ((int)(short)mouse_y < (int)(self->tile_height_u + tile_sy))) {
    off_x = (short)mouse_x - tile_sx[0];
    off_y = (short)mouse_y - tile_sy;
    client_dx = (short)mouse_x - (short)self->client_x;
    client_dy = (short)mouse_y - (short)self->client_y;
    if ((client_dx < 0) || (client_dy < 0)) {
      dir = 8;
    }
    else if (((int)client_dx < (int)self->client_width) && ((int)client_dy < (int)self->client_height)) {
      dir = surface_get_pixel(&self->overlay_surface,off_x,off_y);
      dir = dir - 10 >> 4;
    }
    else {
      dir = 8;
    }
  }
  else {
    dir = 8;
  }
  return dir;
}

/* 00410835 auto */
void __cdecl mapview_on_mousemove(int mouse_x,int mouse_y)

{
  iptr child_wnd;
  int check;
  iptr x2;
  iptr unit;
  iptr y2;
  int civ;
  uptr cursor_id;
  MapView *view;
  int dir;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    view = (MapView *)0x0;
  }
  else {
    view = (MapView *)(child_wnd - 0x48);
  }
  if ((city_turn_processing == 0) && (view->cursor_id_uptr != 0x1fe)) {
    check = ui_paradrop_cursor_active();
    if (check == 0) {
      cursor_id = 0x201;
      if ((((((byte)game.game_options & 0x80) != 0) && (moving_units_mode == 1)) &&
          (unit = (iptr)game.active_unit, -1 < unit)) &&
         ((dir = mapview_point_unit_dir
                               (view,mouse_x,mouse_y,(int)units[unit].x,
                                (int)units[unit].y), -1 < dir && (dir < 8)))) {
        cursor_id = (dir + 1U & 7) + 500;
      }
    }
    else {
      cursor_id = 0x202;
      unit = (iptr)game.active_unit;
      check = mapview_screen_to_tile
                        (view,(iptr)&x2,(iptr)&y2,mouse_x,mouse_y);
      if (check != 0) {
        return;
      }
      check = map_in_bounds(x2,y2);
      if (check == 0) {
        return;
      }
      check = map_distance(x2,y2,(int)units[unit].x,(int)units[unit].y);
      if ((int)(uint)cosmic.paradrop_range < check) {
        cursor_id = 0x203;
      }
      else {
        check = tile_is_ocean(x2,y2);
        if (check == 0) {
          civ = tile_unit_owner(x2,y2);
          if ((-1 < civ) && (units[unit].owner != civ)) {
            cursor_id = 0x203;
          }
        }
        else {
          cursor_id = 0x203;
        }
      }
    }
    if (view->cursor_id_uptr != cursor_id) {
      view->cursor_id_uptr = cursor_id;
      wnd_set_cursor_now((WndBase *)&view->window,cursor_id,1);
    }
  }
  return;
}

/* 00410a64 auto */
void __cdecl mapview_cancel_drag(void)

{
  if (-1 < mapview_drag_hold_timer) {
    game_timer_destroy(mapview_drag_hold_timer);
    mapview_drag_hold_timer = -1;
  }
  if ((-1 < mapview_drag_view_index) && ((((i32 *)&map_views_block.mapview_cursor_id))[mapview_drag_view_index * 0xfc] == 0x1fe)) {
    (((i32 *)&map_views_block.mapview_cursor_id))[mapview_drag_view_index * 0xfc] = 0x201;
    wnd_set_cursor_now((WndBase *)&map_views_block.map_view_recs[mapview_drag_view_index].window,0x201,1);
    wnd_release_capture((WndBase *)&map_views_block.map_view_recs[mapview_drag_view_index].window);
  }
  mapview_drag_view_index = 0xffffffff;
  return;
}

/* 00410b23 auto */
void __cdecl mapview_drag_hold_expired(void)

{
  if (-1 < mapview_drag_hold_timer) {
    game_timer_destroy(mapview_drag_hold_timer);
    mapview_drag_hold_timer = -1;
    if (-1 < mapview_drag_view_index) {
      (((i32 *)&map_views_block.mapview_cursor_id))[mapview_drag_view_index * 0xfc] = 0x1fe;
      wnd_set_cursor_now((WndBase *)&map_views_block.map_view_recs[mapview_drag_view_index].window,0x1fe,1);
      wnd_set_capture((WndBase *)&map_views_block.map_view_recs[mapview_drag_view_index].window);
    }
  }
  return;
}

/* 00410bc3 auto */
void __cdecl mapview_on_lbuttondown(void)

{
  SHORT shift_state;
  iptr child_wnd;
  int paradrop;
  MapView *view;
  
  if ((city_turn_processing == 0) && ((game.multiplayer == '\0' || (net_session_block.ui_input_busy == 0)))) {
    net_session_block.ui_input_busy = 1;
    child_wnd = wnd_get_active_child();
    if (child_wnd == 0) {
      view = 0;
    }
    else {
      view = (MapView *)(child_wnd - 0x48);
    }
    mapview_cancel_drag();
    if ((moving_units_mode == 1) && (paradrop = ui_paradrop_cursor_active(), paradrop == 0)) {
      mapview_drag_view_index = (int)view->map_view_index;
      shift_state = GetAsyncKeyState(0x10);
      if ((char)((ushort)shift_state >> 8) == '\0') {
        mapview_drag_hold_timer = game_timer_create((TimerFn)mapview_drag_hold_expired,400,1);
      }
      else {
        (((i32 *)&map_views_block.mapview_cursor_id))[mapview_drag_view_index * 0xfc] = 0x1fe;
        wnd_set_cursor_now((WndBase *)&map_views_block.map_view_recs[mapview_drag_view_index].window,0x1fe,1)
        ;
        wnd_set_capture((WndBase *)&map_views_block.map_view_recs[mapview_drag_view_index].window);
      }
    }
    net_session_block.ui_input_busy = 0;
  }
  return;
}

/* 00410cfa auto */
void __cdecl mapview_on_lbuttonup_any(void)

{
  int view;
  
  if ((city_turn_processing == 0) && ((game.multiplayer == '\0' || (net_session_block.ui_input_busy == 0)))) {
    net_session_block.ui_input_busy = 1;
    view = wnd_get_active_child();
    if (view == 0) {
      view = 0;
    }
    else {
      view -= 0x48;
    }
    if (-1 < mapview_drag_hold_timer) {
      mapview_cancel_drag();
    }
    wnd_release_capture((WndBase *)(view + 0x48));
    net_session_block.ui_input_busy = 0;
  }
  return;
}

/* 00410e46 auto */
void __cdecl mapviews_set_paradrop_cursor(void)

{
  int view;
  
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      (((i32 *)&map_views_block.mapview_cursor_id))[view * 0xfc] = 0x202;
      wnd_set_cursor_now((WndBase *)&map_views_block.map_view_recs[view].window,0x202,1);
    }
  }
  return;
}

/* 00410ed8 auto */
void __cdecl mapviews_reset_cursor(void)

{
  int paradrop;
  int view;
  
  paradrop = ui_paradrop_cursor_active();
  if (paradrop != 0) {
    for (view = 0; view < 8; view++) {
      if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
        (((i32 *)&map_views_block.mapview_cursor_id))[view * 0xfc] = 0x201;
        wnd_set_cursor_now((WndBase *)&map_views_block.map_view_recs[view].window,0x201,1);
      }
    }
  }
  return;
}

/* 004116c1 auto */
void __cdecl mapview_on_lbuttonup(i32 x,i32 y)

{
  map_window_click(x,y,0);
  return;
}

/* 004116e3 auto */
void __cdecl mapview_on_rbuttonup(i32 x,i32 y)

{
  map_window_click(x,y,1);
  return;
}

/* 004132b7 auto */
void __cdecl mapview_calc_rect(void)

{
  int client_w;
  int minimap_w;
  iptr height2;
  int top;
  
  client_w = wnd_client_width(&main_frame_wnd_block.main_frame_wnd);
  minimap_w = rect_width((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  height2 = wnd_client_height(&main_frame_wnd_block.main_frame_wnd);
  height2++;
  top = 0;
  if (map_layout_wide != 0) {
    top = minimap_margin + 1;
    height2 -= (minimap_margin + 1);
  }
  rect_set_xywh((iptr)&map_view_saved_rects_block.map_view_saved_rects,0,top,client_w - minimap_w,height2);
  return;
}

/* 00413350 auto */
bool __cdecl mapview_on_close(void)

{
  iptr child_wnd;
  bool is_main;
  MapView *view;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    view = 0;
  }
  else {
    view = (MapView *)(child_wnd - 0x48);
  }
  is_main = (int)view->map_view_index == 0;
  if (is_main) {
    wnd_minimize((WndBase *)((iptr)view + 0x48));
  }
  else {
    *(i16 *)((iptr)view + 0x2dc) = 0;
  }
  return !is_main;
}

/* 004133c2 auto */
void __cdecl mapview_on_size(void)

{
  iptr child_wnd;
  MapView *view;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    view = 0;
  }
  else {
    view = (MapView *)(child_wnd - 0x48);
  }
  wnd_get_rect_in_parent
            ((WndBase *)((iptr)view + 0x48),
             (iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + (int)view->map_view_index * 0x10));
  return;
}

/* 0041341c auto */
void __cdecl mapview_on_move(void)

{
  iptr child_wnd;
  MapView *view;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    view = 0;
  }
  else {
    view = (MapView *)(child_wnd - 0x48);
  }
  wnd_get_rect_in_parent
            ((WndBase *)((iptr)view + 0x48),
             (iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + (int)view->map_view_index * 0x10));
  return;
}

/* 00413476 auto */
void __thiscall mapview_update_title(MapView *self)

{
  bool show_mode;
  iptr adjective;
  int view;
  uint mode;
  
  mode = (uint)((self->open_u & 0x10000) != 0);
  if ((self->open_u & 0x20000) != 0) {
    mode += 2;
  }
  text_clear();
  if (self->map_view_index == 0) {
    if (game_language != 1) {
      adjective = civ_get_adjective(human_civ[0]);
      text_append_str((const char *)adjective);
      if (game_language == 2) {
        text_append((const char *)&str_e);
      }
      text_append_space();
    }
    show_mode = false;
    for (view = 1; view < 8; view++) {
      if (map_views_block.map_view_recs[view].open != 0) {
        show_mode = true;
        break;
      }
    }
  }
  else {
    show_mode = true;
  }
  text_append_label(4);
  if (show_mode) {
    text_append_colon();
    text_append_label(mode + 300);
  }
  frame_wnd_set_caption(self,(const char *)&map_views_block.text_buffer);
  return;
}

/* 004135ab auto */
void __cdecl mapview_on_caption_button(i32 button)

{
  iptr child_wnd;
  MapView *this2;
  
  if ((game.multiplayer == '\0') || (net_session_block.ui_input_busy == 0)) {
    net_session_block.ui_input_busy = 1;
    child_wnd = wnd_get_active_child();
    if (child_wnd == 0) {
      this2 = (MapView *)0x0;
    }
    else {
      this2 = (MapView *)(child_wnd - 0x48);
    }
    switch(button) {
    case 1:
      *(i16 *)&this2->open_u = 0;
      wnd_destroy_with_surface(this2);
      mapview_update_title((MapView *)((i8 *)&map_views_block.map_views));
      wnd_repaint((SurfaceWindow *)((i8 *)&map_views_block.map_views));
      break;
    case 2:
      if ((short)this2->zoom_u < 8) {
        *(short *)&this2->zoom_u = (short)this2->zoom_u + 1;
        mapview_redraw(this2,human_civ[0],1);
      }
      break;
    case 3:
      if (-7 < (short)this2->zoom_u) {
        *(short *)&this2->zoom_u = (short)this2->zoom_u - 1;
        mapview_redraw(this2,human_civ[0],1);
      }
      break;
    case 4:
      *(ushort *)((int)&this2->open_u + 2) = *(short *)((int)&this2->open_u + 2) + 1U & 3;
      mapview_update_title(this2);
      mapview_redraw(this2,human_civ[0],1);
    }
    net_session_block.ui_input_busy = 0;
  }
  return;
}

/* 00413717 auto */
void __thiscall mapview_create(MapView *self)

{
  i32 title_text;
  i32 wnd_style;
  i32 create_arg7;
  i32 create_arg8;
  i32 create_arg9;
  int height2;
  int width2;
  int top;
  int left;
  
  mapview_init_state(self,self->map_view_index);
  if ((turn_resume_civ == '\0') || (destroyed_civ_count_block.saved_layout_valid == 0)) {
    *(i16 *)&self->zoom_u = 0;
    if (999 < caption_font_block.screen_width) {
      *(i16 *)&self->zoom_u = 2;
    }
    if (self->map_view_index == 0) {
      mapview_calc_rect();
    }
  }
  if (self->map_view_index == 0) {
    *(i16 *)((int)&self->open_u + 2) = 3;
  }
  else {
    *(i16 *)((int)&self->open_u + 2) = 2;
  }
  if ((self->map_view_index == 0) || ((short)self->open_u != 0)) {
    left = *(int *)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + self->map_view_index * 0x10);
    top = *(int *)(((i8 *)&map_view_saved_rects_block.map_view_saved_rect_tops) + self->map_view_index * 0x10);
    width2 = rect_width((RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + self->map_view_index * 0x10));
    width2 -= sys_dlgframe_w2;
    height2 = rect_height((RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + self->map_view_index * 0x10));
    height2 -= sys_dlgframe_h2;
  }
  else {
    left = self->map_view_index * 0x28;
    top = self->map_view_index * 0x28;
    width2 = 500;
    height2 = 300;
    rect_set_xywh((iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + self->map_view_index * 0x10),left,top,
                       sys_dlgframe_w2 + 500,sys_dlgframe_h2 + 300);
  }
  *(i16 *)&self->open_u = 1;
  create_arg9 = 0;
  create_arg8 = 0;
  create_arg7 = 6;
  wnd_style = 6;
  title_text = (i32)label_string(label_string_ids[0x4]);
  frame_wnd_create
            (self,(const char *)title_text,wnd_style,left,top,width2,height2,create_arg7,create_arg8,create_arg9)
  ;
  wnd_set_min_size((ModalMenuWindow *)&self->window,0x100,0x80);
  wnd_set_redraw_callback(self,mapview_on_redraw);
  wnd_set_lbuttonup_handler(&self->window.wnd_handlers,mapview_on_lbuttonup);
  wnd_set_rbuttonup_handler(&self->window.wnd_handlers,mapview_on_rbuttonup);
  wnd_set_lbuttondown_handler(&self->window.wnd_handlers,(WndMouseFn)mapview_on_lbuttondown);
  wnd_set_lbuttonup_any_handler(&self->window.wnd_handlers,(WndMouseFn)mapview_on_lbuttonup_any);
  wnd_set_mousemove_handler(&self->window.wnd_handlers,mapview_on_mousemove);
  wnd_set_dblclick_handler(&self->window.wnd_handlers,map_window_double_click);
  wnd_set_char_handler(&self->window.wnd_handlers,(WndKeyFn)map_window_ascii);
  wnd_set_keydown_handler(&self->window.wnd_handlers,map_window_key);
  wnd_set_close_handler(&self->window.wnd_handlers,(WndCloseFn)mapview_on_close);
  wnd_set_size_handler(&self->window.wnd_handlers,(WndSizeFn)mapview_on_size);
  wnd_set_move_handler(&self->window.wnd_handlers,(WndMouseFn)mapview_on_move);
  mapview_update_title(self);
  if (self->map_view_index != 0) {
    frame_wnd_add_caption_button(self,(iptr)&caption_close_button_sprite_block.caption_close_button_sprite,1);
  }
  frame_wnd_add_caption_button(self,(iptr)&caption_zoom_out_sprite,2);
  frame_wnd_add_caption_button(self,(iptr)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite,3);
  if (self->map_view_index != 0) {
    frame_wnd_add_caption_button(self,(iptr)&caption_view_mode_button_sprite,4);
  }
  wnd_set_caption_button_handler(self,mapview_on_caption_button);
  wnd_repaint(self);
  if (game.multiplayer != 1) {
    window_show(self);
  }
  if (self->map_view_index != 0) {
    mapview_update_title((MapView *)((i8 *)&map_views_block.map_views));
    wnd_repaint((SurfaceWindow *)((i8 *)&map_views_block.map_views));
  }
  return;
}

/* 00413a90 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mapviews_create_all(void)

{
  int view;
  
  for (view = 0; view < 8; view++) {
    mapview_init_state(&map_views_block.map_view_recs[view],view);
    if (turn_resume_civ == '\0') {
      if (view == 0) {
        (*(i32 *)&map_views_block.map_view_open_flags) = 1;
      }
      else {
        map_views_block.map_view_recs[view].open = 0;
      }
    }
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      mapview_create(&map_views_block.map_view_recs[view]);
    }
  }
  map_views_block.main_view_status_shown_i32 = 1;
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
    map_views_block.main_view_timer_shown[0] = 1;
  }
  wnd_modal_stack_below_block.timer_alt_kind_request = 1;
  minimap_window_block.unit_blink_timer = game_timer_create((TimerFn)unit_blink_timer_tick,0x96,0xffffffff);
  minimap_window_block.eot_blink_timer = game_timer_create((TimerFn)eot_blink_timer_tick,500,0xffffffff);
  wnd_modal_stack_below_block.timer_alt_kind_request = 0;
  return;
}

/* 00413bd1 auto */
void __cdecl mapviews_destroy_all(void)

{
  int view;
  
  wnd_modal_stack_below_block.timer_alt_kind_request = 1;
  game_timer_destroy(minimap_window_block.unit_blink_timer);
  game_timer_destroy(minimap_window_block.eot_blink_timer);
  wnd_modal_stack_below_block.timer_alt_kind_request = 0;
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      wnd_destroy_with_surface(&map_views_block.map_view_recs[view]);
    }
  }
  return;
}

/* 0046ab30 auto */
void * __thiscall clickmap_ctor(void *self)

{
  return self;
}

/* 0046ab49 auto */
void __thiscall clickmap_dtor(void *self)

{
  return;
}

/* 0046ab5f auto */
/* Library Function - Single Match
    public: void __thiscall CRichEditDoc::InvalidateObjectCache(void)
   
   Library: Visual Studio 1998 Debug */

void __thiscall clickmap_clear(ClickMapClearView *self)

{
  self->region_count = 0;
  return;
}

/* 0046ab82 auto */
void __thiscall clickmap_remove_at(ClickMapRemoveView *self,int index)

{
  int i;
  
  for (i = index; i < (int)(self->entry_count - 1); i++) {
    self->hit_regions[i] = self->hit_regions[i + 1];
  }
  self->entry_count--;
  return;
}

/* 0046ac89 auto */
void __thiscall clickmap_remove_by_id(ClickMapRemoveView *self,int id)

{
  u32 count;
  u32 idx;
  
  count = self->entry_count;
  while (idx = count - 1, -1 < (int)idx) {
    count = idx;
    if (self->hit_regions[idx].value == id) {
      clickmap_remove_at(self,idx);
    }
  }
  return;
}

/* 0046ace7 auto */
u32 __thiscall
clickmap_add
          (ClickMap *self,iptr id,i32 value,i32 x,i32 y,
          iptr width,iptr height)

{
  u32 index;
  
  index = 0xffffffff;
  if ((int)self->entry_count < 200) {
    index = self->entry_count;
    self->entry_count++;
    rect_set_xywh((iptr)&self->hit_regions[index],x,y,width,height);
    self->hit_regions[index].id = id;
    self->hit_regions[index].value = value;
  }
  return index;
}

/* 0046ad85 auto */
int __thiscall
clickmap_hit_test(ClickMapHitTestView *self,int x,int y,iptr out_id,iptr out_value)

{
  int hit;
  int i;
  
  hit = -1;
  i = 0;
  do {
    if ((int)self->region_count <= i) {
report_hit:
      if (-1 < hit) {
        if (out_id != 0) {
          *(i32 *)out_id = self->hit_regions[hit].id;
        }
        if (out_value != 0) {
          *(i32 *)out_value = self->hit_regions[hit].value;
        }
      }
      return hit;
    }
    if ((((self->hit_regions[i].left <= x) &&
         (x < self->hit_regions[i].right)) &&
        (self->hit_regions[i].top <= y)) &&
       (y < self->hit_regions[i].bottom)) {
      hit = i;
      goto report_hit;
    }
    i++;
  } while( true );
}

/* 00472b0a auto */
void __cdecl mapview_draw_tile_number(i32 x,i32 y,int number,i32 color)

{
  int in_view;
  iptr width;
  iptr height;
  i32 screen_y;
  i32 screen_x;
  char num_text[80];
  i8 rect[16];
  
  in_view = mapview_tile_in_view((MapView *)((i8 *)&map_views_block.map_views),x,y);
  if (in_view != 0) {
    mapview_tile_to_screen
              ((MapView *)((i8 *)&map_views_block.map_views),(iptr)&screen_x,(iptr)&screen_y,x,y);
    width = gfx_zoom_scale(0x40,(int)map_views_block.mapview_zoom[0]);
    height = gfx_zoom_scale(0x20,(int)map_views_block.mapview_zoom[0]);
    rect_set_xywh((iptr)rect,screen_x,screen_y,width,height);
    __itoa(number,num_text,10);
    gfx_set_text_color_index(10);
    rect_offset((iptr)rect,0xffffffff,0xffffffff);
    surface_draw_text_in_rect
              ((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&map_views_block.tile_number_font,(const char *)num_text,(RECT *)rect,0);
    rect_offset((iptr)rect,3,3);
    surface_draw_text_in_rect
              ((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&map_views_block.tile_number_font,(const char *)num_text,(RECT *)rect,0);
    rect_offset((iptr)rect,0xfffffffe,0xfffffffe);
    gfx_set_text_color_index(color);
    surface_draw_text_in_rect
              ((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)&map_views_block.tile_number_font,(const char *)num_text,(RECT *)rect,0);
    wnd_blit_surface_rect((DibSurface *)((i8 *)&map_views_block.map_views),(iptr)rect);
  }
  return;
}

/* 00479ede auto */
void __thiscall mapview_init_state(MapView *self,u32 view_index)

{
  self->map_view_index = view_index;
  if (view_index == 0) {
    *(i16 *)((int)&self->open_u + 2) = 1;
  }
  else {
    *(i16 *)((int)&self->open_u + 2) = 0;
  }
  self->cursor_id_uptr = 0xffffffff;
  if (turn_resume_civ == '\0') {
    *(i16 *)&self->zoom_u = 0;
    *(i16 *)&self->center_x_u = map.width >> 1;
    *(i16 *)((int)&self->center_x_u + 2) = map.height >> 1;
    if ((self->center_x_u & 1) != 0) {
      *(short *)&self->center_x_u = (short)self->center_x_u - 1;
    }
    if ((self->center_x_u & 0x10000) != 0) {
      *(short *)&self->center_x_u = (short)self->center_x_u + 1;
    }
  }
  self->cached_zoom = 0xfffffc19;
  return;
}

/* 00479fbe auto */
void __thiscall mapview_update_layout(MapView *self)

{
  u32 scaled;
  i32 wrapped_x;
  uptr wrapped;
  uint clamped;
  i8 dir_mask_rect[16];
  i8 pick_mask_rect[16];
  uptr font_size;
  
  self->layout_width_u = self->client_width;
  self->layout_height_u = self->client_height;
  scaled = mapview_zoom_scale(self,0x40);
  self->tile_width_u = scaled;
  scaled = mapview_zoom_scale(self,0x20);
  self->tile_height_u = scaled;
  self->half_tile_width_u = (int)self->tile_width_u / 2;
  self->half_tile_height_u = (int)self->tile_height_u / 2;
  self->quarter_tile_width_u = (int)(self->tile_width_u + ((int)self->tile_width_u >> 0x1f & 3U)) >> 2;
  self->quarter_tile_height = (int)(self->tile_height_u + ((int)self->tile_height_u >> 0x1f & 3U)) >> 2;
  self->field_320 = self->tile_width_u;
  self->unit_sprite_height_u = self->tile_height_u + self->half_tile_height_u;
  self->half_view_width = (int)((self->layout_width_u - 1) + self->tile_width_u) / (int)self->tile_width_u;
  self->field_2fc = (int)((self->layout_height_u - 1) + self->tile_height_u) / (int)self->tile_height_u;
  self->tile_columns = (int)(self->half_tile_width_u * 3 - 1 + self->layout_width_u) / (int)self->tile_width_u;
  self->field_304 = (int)(self->half_tile_height_u * 3 - 1 + self->layout_height_u) / (int)self->tile_height_u;
  self->margin_x_u = 0;
  self->margin_y_u = 0;
  if ((int)map.width / 2 < (int)self->half_view_width) {
    self->half_view_width = (int)map.width / 2;
    self->margin_x_u = (int)(self->layout_width_u - self->tile_width_u * self->half_view_width) >> 1;
    self->tile_columns = self->half_view_width + 1;
  }
  if ((int)map.height < (int)self->field_2fc) {
    self->field_2fc = (int)map.height / 2;
    self->margin_y_u = (int)(self->layout_height_u - self->tile_height_u * self->field_2fc) >> 1;
    self->field_304 = (int)map.height + 1;
  }
  wrapped_x = map_wrap_x((int)(short)self->center_x_u);
  *(short *)&self->center_x_u = (short)wrapped_x;
  self->origin_y_uptr = (int)*(short *)((int)&self->center_x_u + 2) - self->field_2fc;
  wrapped = map_wrap_x((int)(short)self->center_x_u - self->half_view_width);
  self->origin_x_uptr = wrapped;
  if (((game.rules & 0x8000) != 0) && (map.width + 1 < (int)(self->half_view_width * 2 + self->origin_x_uptr))
     ) {
    self->origin_x_uptr = map.width + 1 + self->half_view_width * -2;
  }
  if (map.height + 1 < (int)(self->field_2fc * 2 + self->origin_y_uptr)) {
    self->origin_y_uptr = map.height + 1 + self->field_2fc * -2;
  }
  clamped = self->origin_x_uptr;
  if (0x7fffffff < clamped) {
    clamped = 0xffffffff;
  }
  self->origin_x_uptr = clamped;
  clamped = self->origin_y_uptr;
  if (0x7fffffff < clamped) {
    clamped = 0xffffffff;
  }
  self->origin_y_uptr = clamped;
  if ((self->origin_x_uptr & 1) != 0) {
    self->origin_x_uptr--;
  }
  if ((self->origin_y_uptr & 1) != 0) {
    self->origin_x_uptr++;
  }
  wrapped = map_wrap_x(self->tile_columns * 2 - 2 + self->origin_x_uptr);
  self->end_x_uptr = wrapped;
  self->end_y_u = self->field_304 * 2 - 2 + self->origin_y_uptr;
  if ((int)(short)self->zoom_u != self->cached_zoom) {
    surface_create(&self->map_surface,self->tile_width_u,self->tile_height_u);
    surface_create(&self->overlay_surface,self->tile_width_u,self->tile_height_u);
    self->cached_zoom = (int)(short)self->zoom_u;
    mapview_apply_zoom(self);
    sprite_draw
              ((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite),(iptr)pick_mask_rect,(iptr)&self->map_surface_head,0,0);
    sprite_draw
              ((Sprite *)((i8 *)&tile_direction_mask_sprite),(iptr)dir_mask_rect,(iptr)&self->overlay_surface_head,0,0);
    gfx_reset_zoom_scale();
  }
  font_size = (int)(self->tile_height_u * 3) / 5;
  if (self->tile_font_size != font_size) {
    font_set(&self->tile_font,0,font_size);
    self->tile_font_size = font_size;
  }
  font_size = (int)(self->tile_height_u + ((int)self->tile_height_u >> 0x1f & 3U)) >> 2;
  if (self->tile_small_font_size != font_size) {
    font_set(&self->tile_small_font,1,font_size);
    self->tile_small_font_size = font_size;
  }
  return;
}

/* 0047a540 auto */
i32 __thiscall
mapview_screen_to_tile
          (MapView *self,iptr out_x,iptr out_y,i32 mouse_x,i32 mouse_y)

{
  short px;
  short py;
  i32 result;
  iptr pixel;
  iptr quadrant;
  iptr tile_x;
  short tile_y;
  
  px = (short)mouse_x - (short)self->client_x;
  py = (short)mouse_y - (short)self->client_y;
  if ((px < 0) || (py < 0)) {
    result = 1;
  }
  else if (((int)px < (int)self->client_width) && ((int)py < (int)self->client_height)) {
    px -= (short)self->margin_x_u;
    py -= (short)self->margin_y_u;
    tile_x = ((int)px / (int)self->tile_width_u) * 2 + self->origin_x_u + 1;
    tile_y = (short)((int)py / (int)self->tile_height_u) * 2 + (short)self->origin_y_u + 1;
    pixel = surface_get_pixel
                      (&self->map_surface,
                       (int)(short)((int)px % (int)self->tile_width_u),
                       (int)(short)((int)py % (int)self->tile_height_u));
    quadrant = pixel - 10 >> 4;
    if (quadrant != 0) {
      tile_x += (((i8 *)&diag_neighbor_bits_block.pick_quadrant_dx_table))[quadrant];
      tile_y += (((i8 *)&diag_neighbor_bits_block.pick_quadrant_dy_table))[quadrant];
    }
    result = map_wrap_x(tile_x);
    *(int *)out_x = (int)(short)result;
    *(int *)out_y = (int)tile_y;
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 0047a6b0 auto */
void __thiscall
mapview_tile_to_screen(MapView *self,iptr out_x,iptr out_y,int x,int y)

{
  uptr top;
  int dx;
  
  dx = map_wrap_x(x - self->origin_x_uptr);
  top = self->origin_y_uptr;
  *(u32 *)out_x = self->half_tile_width_u * (dx - 1) + self->margin_x_u;
  *(u32 *)out_y = self->half_tile_height_u * ((y - top) - 1) + self->margin_y_u;
  *(u32 *)out_x = *(int *)out_x + self->client_x;
  *(u32 *)out_y = *(int *)out_y + self->client_y;
  return;
}

/* 0047a8c9 auto */
void __thiscall
mapview_draw_tile_terrain
          (MapView *self,iptr surface,int screen_x,int screen_y,iptr x,int y,
          int viewer_civ,int zoom,iptr city_tile_idx)

{
  byte nbr_raw;
  uint new_flags;
  int check;
  byte *known;
  iptr civ;
  i32 color;
  u32 unexp_dy;
  u32 unexp_dx;
  u32 dither_dy;
  u32 dither_dx;
  i8 unexp_rect[16];
  i8 airbase_rect[16];
  i8 fortress_rect[16];
  i8 hut_rect[16];
  i8 pollution_rect[16];
  i8 mine_rect[16];
  i8 road_dot_rect[16];
  i8 rail_dot_rect[16];
  i8 rail_rect[16];
  i8 road_rect[16];
  i8 resource_rect[16];
  i8 shield_rect[16];
  i8 farm_rect[16];
  i8 irrig_rect[16];
  i8 river_rect[16];
  i8 farmland_rect[16];
  i8 hill_rect[16];
  i8 mountain_rect[16];
  i8 forest_rect[16];
  i8 dither_rect[16];
  i8 base_rect[16];
  i8 mouth_rect[16];
  i8 coast_rect[16];
  i8 ocean_rect[16];
  i8 hidden_rect[16];
  iptr x2;
  int abs_dy;
  uint terrain_byte;
  uint nbr_terrain;
  uint hidden;
  iptr unit;
  int rail_count;
  int road_count;
  int variant;
  uint improvements2;
  iptr quad;
  byte *tile;
  int abs_dx;
  int river_mask;
  byte *nbr_tile;
  u32 half_w;
  uint terrain2;
  uint nbr_improvements;
  int nbr_hidden;
  iptr y2;
  u32 half_h;
  int draw_y;
  iptr dir;
  u32 tile_w;
  int draw_x;
  uint nbr_byte;
  int quarter_y;
  int has_rail;
  uint irrig_flags;
  int resource;
  i8 airbase_area[16];
  int quarter_x;
  uint has_river;
  iptr quarter;
  u32 tile_h;
  uint dither_terrain[4];
  
  if ((short)self->zoom_u == zoom) {
    tile_w = self->tile_width_u;
    tile_h = self->tile_height_u;
    half_w = self->half_tile_width_u;
    half_h = self->half_tile_height_u;
  }
  else {
    tile_w = gfx_zoom_scale(0x40,zoom);
    tile_h = gfx_zoom_scale(0x20,zoom);
    half_w = (int)tile_w >> 1;
    half_h = (int)tile_h >> 1;
  }
  if (city_tile_idx < 0) {
    check = map_in_bounds(x,y);
    hidden = (uint)(check == 0);
  }
  else {
    hidden = (uint)(0x14 < city_tile_idx);
    if (hidden == 0) {
      check = map_in_bounds(x,y);
      if (check == 0) {
        hidden = 1;
      }
      else {
        hidden = 0;
      }
    }
  }
  gfx_set_zoom_scale(zoom);
  if ((-1 < viewer_civ) || (hidden != 0)) {
    if (hidden == 0) {
      check = tile_visible_to(x,y,viewer_civ);
      if (check == 0) {
        hidden = 1;
      }
      else {
        hidden = 0;
      }
    }
    if (hidden != 0) {
      sprite_draw
                ((Sprite *)((i8 *)&unexplored_tile_sprite),(iptr)hidden_rect,surface,screen_x,screen_y);
      goto reset_zoom_done;
    }
  }
  check = map_in_bounds(x,y);
  if (check == 0) {
    hidden = 1;
    terrain_byte = 7;
    terrain2 = 7;
    improvements2 = 0;
    has_river = 0;
    tile = (byte *)0x0;
  }
  else {
    hidden = 0;
    tile = (byte *)tile_at(x,y);
    terrain_byte = (uint)*tile;
    terrain2 = terrain_byte & 0xf;
    has_river = terrain_byte & 0x80;
    if (viewer_civ < 1) {
      improvements2 = (uint)tile[1];
    }
    else {
      known = known_at(x,y,viewer_civ);
      improvements2 = (uint)*known;
    }
  }
  for (dir = 0; dir < 4; dir++) {
    dither_terrain[dir] = 0xffffffff;
  }
  if (terrain2 == 10) {
    if (zoom < -3) {
      sprite_draw
                ((Sprite *)((i8 *)&terrain_base_sprites_block.terrain_ocean_sprite),(iptr)ocean_rect,surface,screen_x,screen_y);
    }
    else {
      variant = tile_calc_coast_masks(x,y);
      for (quarter = 0; quarter < 4; quarter++) {
        quarter_x = (coast_quarter_dx_table[quarter] + 1) * ((int)half_w >> 1) + screen_x;
        quarter_y = coast_quarter_dy_table[quarter] * ((int)half_h >> 1) + screen_y;
        sprite_draw
                  ((Sprite *)
                   (((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + quarter * 0x3c + (uint)(byte)(((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter] * 0xf0),
                   (iptr)coast_rect,surface,quarter_x,quarter_y);
      }
      for (dir = 0; dir < 4; dir++) {
        x2 = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + x);
        y2 = (((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + y;
        check = map_in_bounds(x2,y2);
        if ((check != 0) && (known = (byte *)tile_at(x2,y2), (*known & 0x80) != 0)) {
          sprite_draw
                    ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + dir * 0x3c),(iptr)mouth_rect,surface,
                     screen_x,screen_y);
        }
      }
    }
  }
  else {
    variant = 0;
    river_mask = 0;
    if (zoom < -4) {
      river_mask = 0xf;
    }
    else {
      for (dir = 0; dir < 4; dir++) {
        x2 = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + x);
        y2 = (((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + y;
        check = map_in_bounds(x2,y2);
        if (check != 0) {
          nbr_tile = (byte *)tile_at(x2,y2);
          nbr_raw = *nbr_tile;
          nbr_byte = (uint)nbr_raw;
          nbr_terrain = nbr_byte & 0xf;
          if (nbr_terrain == terrain2) {
            variant += (((i8 *)&diag_neighbor_bits_block.diag_neighbor_bits))[dir];
          }
          else if (nbr_terrain == 10) {
            dither_terrain[dir] = 0xc;
          }
          else {
            dither_terrain[dir] = nbr_terrain;
          }
          if (((nbr_raw & 0x80) != 0) || (nbr_terrain == 10)) {
            river_mask += (((i8 *)&diag_neighbor_bits_block.diag_neighbor_bits))[dir];
          }
        }
      }
    }
    sprite_draw
              ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c),(iptr)base_rect,surface,screen_x,
               screen_y);
    if (-5 < zoom) {
      for (quad = 0; quad < 4; quad++) {
        if (-1 < (int)dither_terrain[quad]) {
          if (quad < 2) {
            dither_dx = half_w;
          }
          else {
            dither_dx = 0;
          }
          draw_x = dither_dx + screen_x;
          if (((char)quad + 1U & 3) < 2) {
            dither_dy = 0;
          }
          else {
            dither_dy = half_h;
          }
          draw_y = dither_dy + screen_y;
          sprite_draw
                    ((Sprite *)
                     (((i8 *)&map_cursor_sprite_block.terrain_dither_sprites) + quad * 0x3c + dither_terrain[quad] * 0xf0),(iptr)dither_rect
                     ,surface,draw_x,draw_y);
        }
      }
    }
    if (terrain2 == 3) {
      sprite_draw
                ((Sprite *)(((i8 *)&forest_sprites) + variant * 0x3c),(iptr)forest_rect,surface,screen_x,
                 screen_y);
    }
    else if (terrain2 == 5) {
      sprite_draw
                ((Sprite *)(((i8 *)&mountain_sprites) + variant * 0x3c),(iptr)mountain_rect,surface,screen_x,
                 screen_y);
    }
    else if (terrain2 == 4) {
      sprite_draw
                ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + variant * 0x3c),(iptr)hill_rect,surface,screen_x,
                 screen_y);
    }
    if (((-4 < zoom) && (map_show_hidden_terrain == 0)) && (((byte)improvements2 & 0xc) == 0xc)) {
      sprite_draw
                ((Sprite *)((i8 *)&land_improvement_sprites_block.farmland_sprite),(iptr)farmland_rect,surface,screen_x,screen_y);
    }
    if (((-7 < zoom) && (has_river != 0)) && (map_show_hidden_terrain == 0)) {
      sprite_draw
                ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + river_mask * 0x3c),(iptr)river_rect,surface,screen_x,
                 screen_y);
    }
    if (-4 < zoom) {
      irrig_flags = 0;
      if (((byte)improvements2 & 0x42) == 2) {
        for (dir = 2; dir < 5; dir += 2) {
          x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
          y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
          check = map_in_bounds(x2,y2);
          new_flags = irrig_flags;
          if (((check != 0) &&
              (nbr_improvements = tile_improvements(x2,y2), new_flags = irrig_flags,
              (nbr_improvements & 4) != 0)) && (new_flags = irrig_flags | 1, (nbr_improvements & 8) != 0)) {
            new_flags = irrig_flags | 3;
          }
          irrig_flags = new_flags;
        }
      }
      if (map_show_hidden_terrain == 0) {
        if ((((byte)improvements2 & 0xc) == 4) || (irrig_flags == 1)) {
          sprite_draw
                    ((Sprite *)((i8 *)&land_improvement_sprites_block.land_improvement_sprites),(iptr)irrig_rect,surface,screen_x,screen_y);
        }
        else if ((irrig_flags & 2) != 0) {
          sprite_draw
                    ((Sprite *)((i8 *)&land_improvement_sprites_block.farmland_sprite),(iptr)farm_rect,surface,screen_x,screen_y);
        }
      }
    }
    if (((-4 < zoom) && (terrain2 == 2)) &&
       (check = tile_grassland_has_shield(x,y), check != 0)) {
      sprite_draw
                ((Sprite *)((i8 *)&grassland_shield_sprite),(iptr)shield_rect,surface,screen_x,screen_y);
    }
  }
  if (((-4 < zoom) && (hidden == 0)) &&
     (resource = tile_resource(x,y), resource != 0)) {
    sprite_draw
              ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78 + (resource * 4 - 4) * 0xf),
               (iptr)resource_rect,surface,screen_x,screen_y);
  }
  if (((-4 < zoom) && (map_show_hidden_terrain == 0)) &&
     (((improvements2 & 0x10) != 0 || (check = tile_city_owner(x,y), -1 < check)))) {
    road_count = 0;
    rail_count = 0;
    if (((improvements2 & 0x20) == 0) && (check = tile_city_owner(x,y), check < 0)) {
      has_rail = 0;
    }
    else {
      has_rail = 1;
    }
    for (dir = 0; dir < 8; dir++) {
      x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
      y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
      check = map_in_bounds(x2,y2);
      if (check != 0) {
        if (viewer_civ < 1) {
          nbr_improvements = tile_improvements(x2,y2);
        }
        else {
          known = known_at(x2,y2,viewer_civ);
          nbr_improvements = (uint)*known;
        }
        if (((nbr_improvements & 0x12) != 0) &&
           ((road_count = road_count + 1, has_rail == 0 || ((nbr_improvements & 0x22) == 0)))) {
          sprite_draw
                    ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + (dir * 4 + 4) * 0xf),(iptr)road_rect,
                     surface,screen_x,screen_y);
        }
      }
    }
    if (has_rail != 0) {
      for (dir = 0; dir < 8; dir++) {
        x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
        y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
        check = map_in_bounds(x2,y2);
        if (check != 0) {
          if (viewer_civ < 1) {
            nbr_improvements = tile_improvements(x2,y2);
          }
          else {
            known = known_at(x2,y2,viewer_civ);
            nbr_improvements = (uint)*known;
          }
          if ((nbr_improvements & 0x22) != 0) {
            rail_count++;
            sprite_draw
                      ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + (dir * 4 + 4) * 0xf),(iptr)rail_rect,
                       surface,screen_x,screen_y);
          }
        }
      }
    }
    check = tile_city_owner(x,y);
    if (check < 0) {
      if (has_rail == 0) {
        if (road_count == 0) {
          sprite_draw
                    ((Sprite *)((i8 *)&road_sprites_block.road_sprites),(iptr)road_dot_rect,surface,screen_x,screen_y);
        }
      }
      else if (rail_count == 0) {
        sprite_draw
                  ((Sprite *)((i8 *)&road_sprites_block.railroad_sprites),(iptr)rail_dot_rect,surface,screen_x,screen_y);
      }
    }
  }
  if (((-4 < zoom) && (map_show_hidden_terrain == 0)) && (((byte)improvements2 & 0xc) == 8)) {
    sprite_draw
              ((Sprite *)((i8 *)&land_improvement_sprites_block.mine_sprite),(iptr)mine_rect,surface,screen_x,screen_y);
  }
  if ((-5 < zoom) && (hidden == 0)) {
    if ((improvements2 & 0x80) != 0) {
      sprite_draw
                ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite),(iptr)pollution_rect,surface,screen_x,screen_y);
    }
    check = tile_has_hut(x,y);
    if ((check != 0) && (map_show_hidden_terrain == 0)) {
      sprite_draw
                ((Sprite *)((i8 *)&hut_sprite),(iptr)hut_rect,surface,screen_x,screen_y);
    }
    if ((((byte)improvements2 & 0x42) == 0x40) && (map_show_hidden_terrain == 0)) {
      sprite_draw
                ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite),(iptr)fortress_rect,surface,screen_x,screen_y - half_h);
    }
    if ((((byte)improvements2 & 0x42) == 0x42) && (map_show_hidden_terrain == 0)) {
      variant = 0;
      for (unit = tile_first_unit(x,y); -1 < unit;
          unit = unit_next_in_stack(unit)) {
        if (unit_types[units[unit].type].domain == 1) {
          variant = 1;
          break;
        }
      }
      sprite_draw
                ((Sprite *)(((i8 *)&airbase_sprites_block.airbase_sprites) + variant * 0x3c),(iptr)airbase_rect,surface,screen_x,
                 screen_y - half_h);
      rect_set_xywh((iptr)airbase_area,screen_x,screen_y - half_h,tile_w,half_h + tile_h);
      civ = tile_owner(x,y);
      color = civ_color_primary(civ);
      surface_replace_color((DibSurface *)surface,(iptr)airbase_area,0x106,color);
    }
  }
  if ((-4 < zoom) && (-1 < viewer_civ)) {
    for (dir = 0; dir < 4; dir++) {
      x2 = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + x);
      y2 = (((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + y;
      nbr_hidden = 0;
      if (city_tile_idx < 0) {
        check = map_in_bounds(x2,y2);
        if (check != 0) goto check_neighbor_visible;
      }
      else {
        if (city_tile_idx < 0x15) {
          check = map_in_bounds(x2,y2);
          if (check == 0) {
            nbr_hidden = 1;
          }
          if ((int)(((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[city_tile_idx] < 1) {
            abs_dx = ~((int)(((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[city_tile_idx])
                       + 1;
          }
          else {
            abs_dx = (int)(((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + (int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[city_tile_idx];
          }
          if ((int)(((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[city_tile_idx] < 1) {
            abs_dy = ~((int)(((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[city_tile_idx])
                       + 1;
          }
          else {
            abs_dy = (int)(((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[city_tile_idx];
          }
          check = dist_from_deltas(abs_dx,abs_dy);
          if (2 < check) {
            nbr_hidden = 1;
          }
        }
        else {
          nbr_hidden = 1;
        }
check_neighbor_visible:
        if (nbr_hidden == 0) {
          check = tile_visible_to(x2,y2,viewer_civ);
          if (check == 0) {
            nbr_hidden = 1;
          }
          else {
            nbr_hidden = 0;
          }
        }
        if (nbr_hidden != 0) {
          if (dir < 2) {
            unexp_dx = half_w;
          }
          else {
            unexp_dx = 0;
          }
          draw_x = unexp_dx + screen_x;
          if (((char)dir + 1U & 3) < 2) {
            unexp_dy = 0;
          }
          else {
            unexp_dy = half_h;
          }
          draw_y = unexp_dy + screen_y;
          sprite_draw
                    ((Sprite *)(((i8 *)&map_cursor_sprite_block.unexplored_dither_sprites) + dir * 0x3c),(iptr)unexp_rect,surface,
                     draw_x,draw_y);
        }
      }
    }
  }
reset_zoom_done:
  gfx_reset_zoom_scale();
  return;
}

/* 0047ba1d auto */
void __thiscall
mapview_draw_tile_city
          (MapView *self,i32 screen_x,int screen_y,iptr x,iptr y)

{
  iptr city;
  int in_bounds;
  int visible2;
  
  city = city_at(x,y);
  if ((-1 < city) && (in_bounds = map_in_bounds(x,y), in_bounds != 0)) {
    visible2 = tile_visible_to(x,y,(*(i8 *)&human_civ));
    if ((int)cities[city].owner != ((*(i8 *)&human_civ) & 0xffU)) {
      if ((visible2 == 0) ||
         ((((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) == 0 &&
           ((int)cities[city].owner != ((*(i8 *)&human_civ) & 0xffU))) ||
          (*(char *)&cities[city].revealed_size[(*(i8 *)&human_civ)] == '\0')))) {
        visible2 = 0;
      }
      else {
        visible2 = 1;
      }
    }
    if (((visible2 != 0) || (game.reveal_map != '\0')) ||
       (((game.game_flags & 0x80) != 0 && ((prefs_game_rules_block.scenario_flags[0] & 8) != 0)))) {
      gfx_draw_city((iptr)self,city,0,screen_x,screen_y - self->half_tile_height_u,
                         (int)(short)self->zoom_u);
    }
  }
  return;
}

/* 0047bba5 auto */
void __thiscall
mapview_draw_unit_stacked
          (MapView *self,iptr unit,uint draw_flags,i32 screen_x,i32 screen_y)

{
  gfx_draw_unit((iptr)self,unit,draw_flags | 3,screen_x,screen_y,(int)(short)self->zoom_u,0);
  return;
}

/* 0047bbea auto */
void __thiscall
mapview_draw_unit_blinking
          (MapView *self,iptr unit,i32 screen_x,i32 screen_y)

{
  if (((unit_blink_active == '\0') || (unit_blink_phase == '\0')) || (moving_units_mode != 1)) {
    gfx_draw_unit((iptr)self,unit,5,screen_x,screen_y,(int)(short)self->zoom_u,0);
  }
  return;
}

/* 0047bc59 auto */
void __thiscall mapview_draw_unit_at(MapView *self,iptr unit)

{
  int screen_y;
  i32 screen_x;
  
  if (((unit_blink_active == '\0') || (unit_blink_phase == '\0')) || (moving_units_mode != 1)) {
    mapview_tile_to_screen(self,(iptr)&screen_x,(iptr)&screen_y,(int)units[unit].x,
                       (int)units[unit].y);
    screen_y -= self->half_tile_height_u;
    gfx_draw_unit((iptr)self,unit,5,screen_x,screen_y,(int)(short)self->zoom_u,0);
  }
  return;
}

/* 0047bd04 auto */
void __thiscall
mapview_draw_stack_unit
          (MapView *self,iptr unit,i32 screen_x,i32 screen_y)

{
  int selectable;
  
  if ((anim_combat_defender < 0) || (anim_combat_defender != unit)) {
    if ((anim_origin_stack_unit < 0) || (unit != anim_origin_stack_unit)) {
      if ((((units[game.active_unit].x == units[unit].x) &&
           (units[game.active_unit].y == units[unit].y)) &&
          (((uint)game.human_players & 1 << (units[game.active_unit].owner & 0x1fU)) != 0)) &&
         (selectable = unit_is_selectable((int)game.active_unit), selectable != 0)) {
        mapview_draw_unit_blinking(self,(int)game.active_unit,screen_x,screen_y);
        return;
      }
      mapview_draw_unit_stacked(self,unit,4,screen_x,screen_y);
    }
    else {
      gfx_draw_unit((iptr)self,unit,5,screen_x,screen_y,(int)(short)self->zoom_u,0);
    }
  }
  else {
    mapview_draw_unit_blinking(self,unit,screen_x,screen_y);
  }
  return;
}

/* 0047be63 auto */
void __thiscall
mapview_draw_tile_units
          (MapView *self,i32 screen_x,int screen_y,int x,int y)

{
  int draw_y;
  int result;
  iptr unit;
  int visible2;
  
  draw_y = screen_y - self->half_tile_height_u;
  result = map_in_bounds(x,y);
  if (result != 0) {
    unit = tile_first_unit(x,y);
    if (((anim_combat_defender < 0) || (anim_combat_defender_x != x)) || (anim_combat_defender_y != y)) {
      if (((anim_origin_stack_unit < 0) || (x != anim_origin_x)) || (y != anim_origin_y)) {
        result = tile_city_owner(x,y);
        if (-1 < result) {
          if (moving_units_mode != 1) {
            return;
          }
          if (unit_blink_active == '\0') {
            return;
          }
          for (; (game.active_unit != unit && (-1 < unit));
              unit = unit_next_in_stack(unit)) {
          }
          if (unit < 0) {
            return;
          }
          result = unit_is_selectable(unit);
          if (result == 0) {
            return;
          }
          if (units[unit].orders == 1) {
            return;
          }
          if (units[unit].orders == 2) {
            return;
          }
        }
        visible2 = tile_visible_to(x,y,(*(i8 *)&human_civ));
        if ((int)units[unit].owner != ((*(i8 *)&human_civ) & 0xffU)) {
          if ((visible2 == 0) ||
             (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) == 0 &&
              ((int)units[unit].owner != ((*(i8 *)&human_civ) & 0xffU))))) {
            visible2 = 0;
          }
          else {
            visible2 = 1;
          }
        }
        if ((visible2 != 0) || (game.reveal_map != '\0')) {
          mapview_draw_stack_unit(self,unit,screen_x,draw_y);
        }
      }
      else {
        mapview_draw_stack_unit(self,anim_origin_stack_unit,screen_x,draw_y);
      }
    }
    else {
      for (; (unit != anim_combat_defender && (-1 < unit)); unit = unit_next_in_stack(unit)) {
      }
      mapview_draw_stack_unit(self,unit,screen_x,draw_y);
    }
  }
  return;
}

/* 0047c103 auto */
void __thiscall
mapview_draw_tile(MapView *self,iptr x,iptr y,int civ)

{
  int result;
  i8 grid_rect[16];
  i8 radius_rect[16];
  iptr city;
  i32 screen_y;
  i32 screen_x;
  int near_city;
  
  mapview_tile_to_screen(self,(iptr)&screen_x,(iptr)&screen_y,x,y);
  mapview_draw_tile_terrain(self,(iptr)self,screen_x,screen_y,x,y,civ,
                     (int)(short)self->zoom_u,-1);
  win_flush_pending_msgs();
  if (2 < game.multiplayer) {
    net_process_messages(&net_session_block,1,0);
  }
  result = map_in_bounds(x,y);
  if (result != 0) {
    if (map_show_hidden_terrain == 0) {
      mapview_draw_tile_city(self,screen_x,screen_y,x,y);
      mapview_draw_tile_units(self,screen_x,screen_y,x,y);
    }
    if (((byte)game.game_options & 0x20) != 0) {
      near_city = 0;
      mapview_apply_zoom(self);
      for (city = 0; city < game.num_cities; city++) {
        if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
           (result = map_distance_diag(x,y,(int)cities[city].x,
                                       (int)cities[city].y), result < 3)) {
          near_city = 1;
          break;
        }
      }
      if (near_city == 0) {
        sprite_draw
                  ((Sprite *)((i8 *)&map_grid_sprite),(iptr)grid_rect,(iptr)self,screen_x,screen_y);
      }
      else {
        sprite_draw
                  ((Sprite *)((i8 *)&icons_pair_sprites_block.city_radius_grid_sprite),(iptr)radius_rect,(iptr)self,screen_x,screen_y);
      }
      gfx_reset_zoom_scale();
    }
  }
  return;
}

/* 0047c3e0 auto */
i32 __thiscall
mapview_tile_in_view(MapView *self,i32 tile_x,i32 tile_y)

{
  i32 in_view;
  
  in_view = map_tile_in_box(tile_x,tile_y,self->origin_x_uptr,self->origin_y_uptr,
                             self->half_view_width + self->tile_columns,self->field_2fc + self->field_304 + 1
                            );
  return in_view;
}

/* 0047c443 auto */
void __thiscall
mapview_draw_city_names(MapView *self,i32 center_x,i32 center_y,int radius)

{
  int label_x;
  int text_width;
  i32 shadow_color;
  int name_width;
  i32 text_color;
  i32 shadow_depth;
  i32 shadow_offset;
  int shadow_y;
  int screen_y;
  iptr city;
  int screen_x;
  int y2;
  int x2;
  int visible2;
  int label_y;
  
  if (map_show_hidden_terrain == 0) {
    if (-1 < radius) {
      radius += 3;
    }
    text_set_font((iptr)&self->tile_font_head);
    for (city = 0; city < game.num_cities; city++) {
      if (cities[city].id != 0) {
        x2 = (int)cities[city].x;
        y2 = (int)cities[city].y;
        if (((radius < 0) ||
            (label_x = map_distance(x2,y2,center_x,center_y), label_x <= radius)) &&
           (label_x = mapview_tile_in_view(self,x2,y2), label_x != 0)) {
          visible2 = tile_visible_to(x2,y2,(*(i8 *)&human_civ));
          if ((int)cities[city].owner != ((*(i8 *)&human_civ) & 0xffU)) {
            if (((visible2 == 0) ||
                (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) == 0 &&
                 ((int)cities[city].owner != ((*(i8 *)&human_civ) & 0xffU))))) ||
               (*(char *)&cities[city].revealed_size[(*(i8 *)&human_civ)] == '\0')) {
              visible2 = 0;
            }
            else {
              visible2 = 1;
            }
          }
          if (game.reveal_map != '\0') {
            visible2 = 1;
          }
          if (visible2 != 0) {
            mapview_tile_to_screen(self,(iptr)&screen_x,(iptr)&screen_y,x2,y2);
            label_x = self->half_tile_width_u + screen_x;
            label_y = ((int)(self->tile_height_u * 3) >> 2) + screen_y;
            text_set_colors(10,0xffffffff,0xffffffff,0xffffffff);
            text_width = mapview_zoom_scale(self,2);
            if (text_width < 3) {
              shadow_y = mapview_zoom_scale(self,2);
            }
            else {
              shadow_y = 2;
            }
            shadow_y = label_y + shadow_y;
            text_width = font_text_width
                              (&self->tile_font,(const char *)&cities[city].name);
            text_draw_shadowed((iptr)self,(const char *)&cities[city].name,label_x - (text_width >> 1),shadow_y);
            text_width = mapview_zoom_scale(self,2);
            if (text_width < 3) {
              shadow_offset = mapview_zoom_scale(self,2);
            }
            else {
              shadow_offset = 2;
            }
            shadow_depth = 0;
            text_color = 10;
            shadow_color = civ_color_secondary((int)cities[city].owner);
            text_set_colors(shadow_color,text_color,shadow_offset,shadow_depth);
            text_width = label_y;
            name_width = font_text_width
                              (&self->tile_font,(const char *)&cities[city].name);
            text_draw_shadowed((iptr)self,(const char *)&cities[city].name,label_x - (name_width >> 1),text_width);
          }
        }
      }
    }
  }
  return;
}

/* 0047c7aa auto */
void __thiscall
mapview_tile_radius_rect
          (MapView *self,i32 tile_x,i32 tile_y,int radius,iptr out_rect)

{
  i8 rect[16];
  int width2;
  int top;
  int left;
  
  mapview_tile_to_screen(self,(iptr)&left,(iptr)&top,tile_x,tile_y);
  top -= (radius * 2 + 1) * self->half_tile_height_u;
  left += self->half_tile_width_u * radius * -2;
  width2 = (radius * 2 + 1) * self->tile_width_u;
  rect_set_xywh((iptr)rect,left,top,width2,(radius * 4 + 3) * self->half_tile_height_u);
  rect_intersect_into((LPRECT)out_rect,(iptr)rect,(iptr)&self->client_rect);
  return;
}

/* 0047c869 auto */
void __thiscall
mapview_draw_tile_radius
          (MapView *self,int center_x,int center_y,int radius,i32 civ)

{
  uint extent;
  uint row_abs;
  i32 x;
  int in_view;
  uint dx_abs;
  uint dy;
  uint dx;
  int row_extent;
  i8 clip_rect[16];
  
  mapview_tile_radius_rect(self,center_x,center_y,radius,(iptr)clip_rect);
  surface_set_clip(self,(RECT *)clip_rect);
  extent = radius * 2 + 2;
  for (dy = -extent; (int)dy <= (int)extent; dy++) {
    if ((int)dy < 1) {
      row_abs = ~dy + 1;
    }
    else {
      row_abs = dy & 1;
    }
    row_extent = extent - row_abs;
    for (dx = -row_extent; (int)dx <= row_extent; dx += 2) {
      if ((int)dx < 1) {
        dx_abs = ~dx + 1;
      }
      else {
        dx_abs = dx;
      }
      if (dx_abs != extent) {
        x = map_wrap_x(dx + center_x);
        in_view = mapview_tile_in_view(self,x,center_y + dy);
        if (in_view != 0) {
          mapview_draw_tile(self,x,center_y + dy,civ);
        }
      }
    }
  }
  mapview_draw_city_names(self,center_x,center_y,radius);
  surface_set_clip(self,(RECT *)&self->client_rect);
  return;
}

/* 0047c9d4 auto */
void __thiscall mapview_draw_all_tiles(MapView *self,i32 civ)

{
  uptr origin_y;
  uptr origin_x;
  i32 x;
  uint row;
  int col;
  int row_width;
  
  if ((self->margin_x_u != 0) || (self->margin_y_u != 0)) {
    gfx_fill_surface((iptr)self,10);
  }
  for (row = 0; (int)row < (int)(self->field_2fc + self->field_304 + 1);
      row = row + 1) {
    origin_y = self->origin_y_uptr;
    origin_x = self->origin_x_uptr;
    if ((row & 1) == 0) {
      row_width = self->tile_columns;
    }
    else {
      row_width = self->half_view_width;
    }
    for (col = 0; col < row_width; col++) {
      x = map_wrap_x(col * 2 + origin_x + (row & 1));
      mapview_draw_tile(self,x,origin_y + row,civ);
    }
  }
  mapview_draw_city_names(self,0,0,0xffffffff);
  return;
}

/* 0047caea auto */
void __thiscall
mapview_blit_tile_radius
          (MapView *self,i32 tile_x,i32 tile_y,i32 radius)

{
  i8 rect[16];
  
  mapview_tile_radius_rect(self,tile_x,tile_y,radius,(iptr)rect);
  wnd_blit_surface_rect(self,(iptr)rect);
  return;
}

/* 0047cb26 auto */
void __thiscall
mapview_blit_tile(MapView *self,i32 tile_x,i32 tile_y)

{
  mapview_blit_tile_radius(self,tile_x,tile_y,0);
  return;
}

/* 0047cb50 auto */
void __thiscall mapview_blit_with_net_pump(DibSurface *self)

{
  win_flush_pending_msgs();
  if (2 < game.multiplayer) {
    net_process_messages(&net_session_block,1,0);
  }
  wnd_blit_surface(self);
  win_flush_pending_msgs();
  if (2 < game.multiplayer) {
    net_process_messages(&net_session_block,1,0);
  }
  return;
}

/* 0047cbb4 auto */
void __thiscall
mapview_redraw_area
          (MapView *self,i32 tile_x,i32 tile_y,i32 radius,
          iptr viewer_civ,int do_blit)

{
  int in_view;
  iptr civ;
  i8 cursor_rect[16];
  i32 cursor_x;
  i32 cursor_y;
  
  if (game_loop_active != '\0') {
    net_session_block.map_redraw_busy = 1;
    if (game.reveal_map == '\0') {
      civ = viewer_civ;
    }
    else {
      civ = -1;
    }
    mapview_draw_tile_radius(self,tile_x,tile_y,radius,civ);
    if ((((moving_units_mode == 0) && (unit_blink_phase != '\0')) && (unit_blink_active != '\0')) &&
       ((self->map_view_index == 0 &&
        (in_view = mapview_tile_in_view(self,(int)map_cursor_x[0],(int)map_cursor_y[0]), in_view != 0)))) {
      mapview_tile_to_screen(self,(iptr)&cursor_x,(iptr)&cursor_y,(int)map_cursor_x[0],(int)map_cursor_y[0]);
      mapview_apply_zoom(self);
      sprite_draw
                ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite),(iptr)cursor_rect,(iptr)self,cursor_x,cursor_y);
      gfx_reset_zoom_scale();
    }
    if (do_blit != 0) {
      mapview_blit_tile_radius(self,tile_x,tile_y,radius);
    }
    net_session_block.map_redraw_busy = 0;
  }
  return;
}

/* 0047cced auto */
void __thiscall
mapview_redraw_tile(MapView *self,i32 tile_x,i32 tile_y)

{
  mapview_redraw_area(self,tile_x,tile_y,0,human_civ[0],1);
  return;
}

/* 0047cd1f auto */
void __thiscall
mapview_redraw_tile_neighbors(MapView *self,i32 tile_x,i32 tile_y)

{
  mapview_redraw_area(self,tile_x,tile_y,1,human_civ[0],1);
  return;
}

/* 0047cd51 auto */
void __thiscall mapview_redraw(MapView *self,iptr viewer_civ,int do_blit)

{
  iptr civ;
  
  if (game_loop_active != '\0') {
    net_session_block.map_redraw_busy = 1;
    frame_wnd_layout(self);
    mapview_update_layout(self);
    frame_wnd_draw_caption(self);
    if (game.reveal_map == '\0') {
      civ = viewer_civ;
    }
    else {
      civ = -1;
    }
    mapview_draw_all_tiles(self,civ);
    if ((do_blit != 0) &&
       (mapview_blit_with_net_pump(self), self->map_view_index == 0)) {
      minimap_redraw();
    }
    if ((map_click_in_progress == 0) && (net_session_block.mp_simultaneous_moves == '\0')) {
      input_flush_events();
    }
    net_session_block.map_redraw_busy = 0;
  }
  return;
}

/* 0047dce0 auto */
MapView * __thiscall mapview_ctor(MapView *self)

{
  i32 seh_prev;
  i8 *seh_handler;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  frame_wnd_ctor(self);
  eh_state = 0;
  font_init_empty(&self->tile_font);
  (*((char *)&eh_state + 0)) = 1;
  font_init_empty(&self->tile_small_font);
  (*((char *)&eh_state + 0)) = 2;
  surface_ctor(&self->map_surface);
  eh_state = (eh_state & 0xffffff00 | (byte)(3));
  surface_ctor(&self->overlay_surface);
  self->vtable = (u32)&ptr_fun_2;
  return self;
}

/* 0047de10 auto */
/* Library Function - Single Match
    public: virtual __thiscall CBitmapButton::~CBitmapButton(void)
   
   Library: Visual Studio 1998 Debug */

void __thiscall mapview_dtor(void *self)

{
  i32 seh_prev;
  i8 *seh_handler;
  int eh_state;
  
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0047de82 */ { surface_dtor((DibSurface *)(*(int *)&self + 0x3a8)); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0047de91 */ { surface_dtor((DibSurface *)(*(int *)&self + 0x360)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0047dea0 */ { font_destroy((GameFont *)(*(int *)&self + 0x340)); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0047deaf */ { font_destroy((GameFont *)(*(int *)&self + 0x338)); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0047debe */ { frame_wnd_dtor(*(FrameWindow **)&self); }
  return;
}

/* 0047dfb0 auto */
i32 __thiscall mapview_zoom_scale(MapView *self,i32 value)

{
  i32 scaled;
  
  scaled = gfx_zoom_scale(value,(int)(short)self->zoom_u);
  return scaled;
}

/* 0047dff0 auto */
void __thiscall mapview_apply_zoom(MapView *self)

{
  gfx_set_zoom_scale((int)(short)self->zoom_u);
  return;
}

/* 0047e030 auto */
i32 __cdecl mapviews_tile_visible(iptr x,iptr y)

{
  int visible;
  int view_index;
  
  visible = tile_visible_to(x,y,human_civ[0]);
  if (visible != 0) {
    for (view_index = 0; view_index < 8; view_index++) {
      if (((view_index == 0) || (map_views_block.map_view_recs[view_index].open != 0)) &&
         (visible = mapview_tile_in_view
                            (&map_views_block.map_view_recs[view_index],x,y),
         visible != 0)) {
        return 1;
      }
    }
  }
  return 0;
}

/* 00489b9b auto */
void __cdecl mapview_center_if_moved(int center_x,int center_y)

{
  if ((map_views_block.map_view_center_x[0] != center_x) || (map_views_block.mapview_center_y[0] != center_y)) {
    mapview_center_on((MapView *)((i8 *)&map_views_block.map_views),center_x,center_y);
  }
  return;
}

/* 004d8b70 auto */
void __thiscall
clickmap_add_xywh
          (ClickMap *self,iptr id,i32 left,i32 top,i32 width,
          i32 height)

{
  clickmap_add(self,id,0,left,top,width,height);
  return;
}
