/* The game, the map windows: drawing, scrolling, the mini map: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"
#include "../platform/hd.h"

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
  ((FrameWindow *)((i8 *)&minimap_state.window))->FrameWindow::construct();
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
  ((FrameWindow *)((i8 *)&minimap_state.window))->FrameWindow::destruct();
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
  ((CityWindow *)((i8 *)&city_wnd.window))->bring_to_top();
  minimap_hidden = 0;
  minimap_redraw();
  return;
}

/* 00406b28 auto */
i32 __cdecl minimap_on_close(void)

{
  minimap_hidden = 1;
  ((WndBase *)((i8 *)&minimap_state.wnd))->minimize();
  return 0;
}

/* 00406b4c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl minimap_calc_layout(void)

{
  int client_h;
  int tile_size;
  int tile_h;
  
  client_h = minimap_state.client_height;
  tile_size = minimap_state.client_width / (int)map.width;
  if (tile_size < 2) {
    tile_size = 1;
  }
  tile_h = minimap_state.client_height / (int)map.height;
  if (tile_h < 2) {
    tile_h = 1;
  }
  if (tile_h <= tile_size) {
    tile_size = tile_h;
  }
  minimap_layout.tile_height = tile_size;
  if (tile_h <= tile_size) {
    minimap_layout.tile_height = tile_h;
  }
  minimap_layout.tile_width = tile_size * 2;
  tile_size = (minimap_state.client_width - minimap_layout.tile_width / 2) / minimap_layout.tile_width;
  minimap_layout.view_cols = (int)map.width / 2;
  if (tile_size <= (int)map.width / 2) {
    minimap_layout.view_cols = tile_size;
  }
  minimap_layout.view_rows = (int)map.height;
  if (minimap_state.client_height / minimap_layout.tile_height <= (int)map.height) {
    minimap_layout.view_rows = minimap_state.client_height / minimap_layout.tile_height;
  }
  tile_size = int_clamp(minimap_state.client_width - minimap_layout.view_cols * minimap_layout.tile_width,0,999);
  minimap_state.margin_y = int_clamp(client_h - minimap_layout.view_rows * minimap_layout.tile_height,0,999);
  minimap_state.margin_x = tile_size >> 1;
  minimap_state.margin_y = minimap_state.margin_y >> 1;
  if (minimap_layout.view_rows < map.height) {
    (*(uint *)&minimap_layout.origin_y) = (int)map_views_block.mapview_center_y[0] - (minimap_layout.view_rows >> 1);
    (*(uint *)&minimap_layout.origin_y) = int_clamp((*(uint *)&minimap_layout.origin_y),0,map_views_block.mapview_layout_height - minimap_layout.view_rows);
  }
  else {
    (*(uint *)&minimap_layout.origin_y) = 0;
  }
  (*(uint *)&minimap_layout.origin_x) = map_wrap_x(map_views_block.map_view_center_x[0] - minimap_layout.view_cols);
  if ((game.rules & RULE_FLAT_WORLD) != 0) {
    if (minimap_layout.view_cols < map.width) {
      (*(uint *)&minimap_layout.origin_x) = int_clamp((*(uint *)&minimap_layout.origin_x),0,map_views_block.mapview_layout_width - minimap_layout.view_cols);
    }
    else {
      (*(uint *)&minimap_layout.origin_x) = 0;
    }
  }
  if (((*(uint *)&minimap_layout.origin_x) & 1) != 0) {
    (*(uint *)&minimap_layout.origin_x)--;
  }
  if (((*(uint *)&minimap_layout.origin_y) & 1) != 0) {
    (*(uint *)&minimap_layout.origin_x)++;
  }
  return;
}

/* 00406db8 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl minimap_tile_to_screen(int x,uint y,iptr out_x,iptr out_y)

{
  int dx;
  
  dx = x - minimap_layout.origin_x;
  if (((game.rules & RULE_FLAT_WORLD) == 0) && (dx < 0)) {
    dx += map.width;
  }
  *(uint *)out_y = (y - minimap_layout.origin_y) * minimap_layout.tile_height + minimap_state.margin_y;
  *(int *)out_x = (dx >> 1) * minimap_layout.tile_width + minimap_state.margin_x;
  if ((y & 1) != 0) {
    *(int *)out_x = *(int *)out_x + (minimap_layout.tile_width >> 1);
  }
  *(int *)out_x = *(int *)out_x + minimap_state.client_x;
  *(int *)out_y = *(int *)out_y + minimap_state.client_y;
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
        if ((((blink.unit_active == '\0') || ((moving_units_mode == 0) != (bool)blink.unit_phase)) ||
            (map_cursor.x[0] != x)) || (map_cursor.y[0] != y)) {
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
  
  if ((minimap_hidden == '\0') && (turn_loop.active != '\0')) {
    ((FrameWindow *)((i8 *)&minimap_state.window))->layout();
    gfx_set_draw_target((iptr)&minimap_state.window);
    minimap_calc_layout();
    void *hd_bits=(void *)((DibSurface *)&minimap_state.window)->bits;
    hd_indexed_begin(hd_bits);
    gfx_fill_draw_target(0);
    screen_y = minimap_state.client_y + minimap_state.margin_y;
    for (row = 0; row < minimap_layout.view_rows; row++) {
      y = row + minimap_layout.origin_y;
      if ((y & 1) == 0) {
        row_offset = 0;
      }
      else {
        row_offset = minimap_layout.tile_width >> 1;
      }
      screen_x = minimap_state.client_x + minimap_state.margin_x + row_offset;
      for (col = 0; col < minimap_layout.view_cols; col++) {
        x2 = map_wrap_x(col * 2 + minimap_layout.origin_x);
        if ((x2 & 1) != 0) {
          x2--;
        }
        if ((y & 1) != 0) {
          x2++;
        }
        in_bounds = map_in_bounds(x2,y);
        if (in_bounds != 0) {
          color = minimap_tile_color(x2,y);
          gfx_target_fill_rect(screen_x,screen_y,minimap_layout.tile_width,minimap_layout.tile_height,color);
          screen_x += minimap_layout.tile_width;
        }
      }
      screen_y += minimap_layout.tile_height;
    }
    hd_indexed_end(hd_bits);
    win_flush_pending_msgs();
    minimap_draw_view_rect();
    ((FrameWindow *)((i8 *)&minimap_state.window))->FrameWindow::draw_caption();
    ((DibSurface *)((i8 *)&minimap_state.window))->wnd_blit_surface();
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
  
  mouse_x -= minimap_state.client_x;
  mouse_y -= minimap_state.client_y;
  if ((((-1 < mouse_x) && (-1 < mouse_y)) && (mouse_x < minimap_state.client_width)) && (mouse_y < minimap_state.client_height))
  {
    if (city_turn.processing == 0) {
      tile_y = int_clamp(minimap_layout.origin_y + (mouse_y - minimap_state.margin_y) / minimap_layout.tile_height,
                                 minimap_layout.origin_y,minimap_layout.origin_y + minimap_layout.view_rows - 1);
      if ((tile_y & 1) == 0) {
        row_offset = 0;
      }
      else {
        row_offset = minimap_layout.tile_width >> 1;
      }
      col_x = int_clamp(((mouse_x - (minimap_state.margin_x + row_offset)) / minimap_layout.tile_width) * 2 +
                                 minimap_layout.origin_x,minimap_layout.origin_x,minimap_layout.view_cols * 2 - 2 + minimap_layout.origin_x);
      tile_x = map_wrap_x(col_x);
      if ((tile_x & 1) != 0) {
        tile_x--;
      }
      if ((tile_y & 1) != 0) {
        tile_x++;
      }
      in_bounds = map_in_bounds(tile_x,tile_y);
      if (in_bounds != 0) {
        ((MapView *)((i8 *)&map_views_block.map_views))->center_on(tile_x,tile_y);
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
  
  mouse_x -= minimap_state.client_x;
  mouse_y -= minimap_state.client_y;
  if ((((-1 < mouse_x) && (-1 < mouse_y)) && (mouse_x < minimap_state.client_width)) && (mouse_y < minimap_state.client_height))
  {
    if (city_turn.processing == 0) {
      tile_y = int_clamp(minimap_layout.origin_y + (mouse_y - minimap_state.margin_y) / minimap_layout.tile_height,
                                 minimap_layout.origin_y,minimap_layout.origin_y + minimap_layout.view_rows - 1);
      if ((tile_y & 1) == 0) {
        row_offset = 0;
      }
      else {
        row_offset = minimap_layout.tile_width >> 1;
      }
      col_x = int_clamp(((mouse_x - (minimap_state.margin_x + row_offset)) / minimap_layout.tile_width) * 2 +
                                 minimap_layout.origin_x,minimap_layout.origin_x,minimap_layout.view_cols * 2 - 2 + minimap_layout.origin_x);
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
          map_views_block.map_view_recs[free_view].create();
          map_views_block.map_view_recs[free_view].center_on(tile_x,tile_y);
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
  
  client_w = main_frame_wnd_block.main_frame_wnd.client_width();
  width2 = 0xa0;
  height2 = 100;
  if ((1000 < main_frame_bg_surface_block.main_frame_nonclient_width + client_w) && (width2 = 0xf0, map_layout_wide != 0)) {
    height2 = 0x96;
  }
  if (height2 < map.height) {
    height2 = (int)map.height;
  }
  if (extra_ui(X_ARRANGE)) {
    width2 = extras_panel_width(width2);   /* X38: Arrange Windows S or L */
  }
  width2 += ui_metrics.border_width * 2;
  top = 0;
  if (map_layout_wide != 0) {
    top = minimap_margin + 1;
  }
  rect_set_xywh((iptr)&minimap_wnd_rect_block.minimap_wnd_rect,client_w - (ui_metrics.dlgframe_w2 + width2),top,width2 + ui_metrics.dlgframe_w2,
                     height2 + ui_metrics.border_width + ui_metrics.caption_height + ui_metrics.dlgframe_h2);
  return;
}

/* 0040795a auto */
void __cdecl minimap_on_size(void)

{
  ((WndBase *)((i8 *)&minimap_state.wnd))->get_rect_in_parent((iptr)&minimap_wnd_rect_block.minimap_wnd_rect);
  return;
}

/* 00407980 auto */
void __cdecl minimap_on_move(void)

{
  ((WndBase *)((i8 *)&minimap_state.wnd))->get_rect_in_parent((iptr)&minimap_wnd_rect_block.minimap_wnd_rect);
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
  if ((turn_loop.resume_civ == '\0') || (destroyed_civ_count_block.saved_layout_valid == 0)) {
    minimap_calc_rect();
  }
  create_arg9 = 0;
  create_arg8 = 0;
  create_arg7 = 6;
  wnd_height = rect_height((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  wnd_height -= ui_metrics.dlgframe_h2;
  wnd_width = rect_width((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  wnd_width -= ui_metrics.dlgframe_w2;
  wnd_style = 6;
  wnd_x = minimap_wnd_rect_block.minimap_wnd_rect;
  wnd_y = minimap_wnd_rect_block.minimap_wnd_rect_top;
  title_text = (i32)label_string(label_table.string_ids[0x6]);
  ((FrameWindow *)((i8 *)&minimap_state.window))->FrameWindow::create((const char *)title_text,wnd_style,wnd_x,wnd_y,wnd_width,wnd_height,create_arg7,create_arg8,create_arg9);
  ((ModalMenuWindow *)((i8 *)&minimap_state.wnd))->set_min_size(0x28,0x19);
  ((SurfaceWindow *)((i8 *)&minimap_state.window))->set_redraw_callback(minimap_redraw);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_lbuttonup_handler(minimap_on_lbuttonup);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_rbuttonup_handler(minimap_on_rbuttonup);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_keydown_handler(map_window_key);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_char_handler((WndKeyFn)map_window_ascii);
  ((WndBase *)((i8 *)&minimap_state.wnd))->set_cursor(0x1ff);
  ((WndBase *)((i8 *)&minimap_state.wnd))->set_icon(1);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_minimize_handler(minimap_on_minimize);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_restore_handler(minimap_on_restore);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_close_handler(minimap_on_close);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_size_handler((WndSizeFn)minimap_on_size);
  ((WndHandlers *)((i8 *)&minimap_state.wnd_handlers))->set_move_handler((WndMouseFn)minimap_on_move);
  ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
  if (game.multiplayer != 1) {
    ((SurfaceWindow *)((i8 *)&minimap_state.window))->SurfaceWindow::show();
  }
  return;
}

/* 00407b31 auto */
void __cdecl minimap_window_destroy(void)

{
  minimap_hidden = 1;
  ((DibSurface *)((i8 *)&minimap_state.window))->wnd_destroy_with_surface();
  return;
}

/* 00410302 auto */
void MapView::redraw_cursor_tile()

{
  redraw_area((int)map_cursor.x[0],(int)map_cursor.y[0],0,human_civ[0],1);
  return;
}

/* 0041033a auto */
void __cdecl mapviews_redraw_cursor(void)

{
  int view;
  
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      map_views_block.map_view_recs[view].redraw_cursor_tile();
    }
  }
  return;
}

/* 004103ae auto */
void __cdecl mapview_on_redraw(void)

{
  int child_wnd;
  MapView *this2;
  
  ((CityWindow *)((i8 *)&city_wnd.window))->bring_to_top();
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    this2 = (MapView *)0x0;
  }
  else {
    this2 = (MapView *)(child_wnd - 0x48);
  }
  this2->redraw(human_civ[0],1);
  return;
}

/* 00410402 auto */
void MapView::center_on(iptr x,i32 y)

{
  i8 saved_blink;
  i32 wrapped_x;
  
  wrapped_x = map_wrap_x(x);
  *(short *)&center_x_u = (short)wrapped_x;
  *(i16 *)((int)&center_x_u + 2) = (i16)y;
  saved_blink = blink.unit_active;
  blink.unit_active = 0;
  redraw(human_civ[0],1);
  blink.unit_active = saved_blink;
  return;
}

/* 00410464 auto */
int MapView::scroll_if_near_edge(int x,int y)

{
  uptr view_left;
  uptr view_top;
  u32 view_cols;
  u32 view_bottom;
  int need_scroll;
  
  need_scroll = 0;
  view_left = origin_x_uptr;
  view_top = origin_y_uptr;
  if ((((game.rules & RULE_FLAT_WORLD) == 0) && (x <= (int)(half_view_width * 2))) &&
     ((int)((int)map.width + half_view_width * -2) <= (int)view_left)) {
    x += map.width;
  }
  view_cols = tile_columns;
  view_bottom = end_y_u;
  if (margin_x_u == 0) {
    if ((game.rules & RULE_FLAT_WORLD) != 0) {
      x = int_clamp(x,2,map.width - 3);
    }
    if ((x <= (int)(view_left + 1)) || ((int)(view_cols * 2 + view_left - 3) <= x)) {
      need_scroll = 1;
    }
  }
  if ((margin_y_u == 0) &&
     ((y = int_clamp(y,2,map.height - 3), y <= (int)(view_top + 1) ||
      ((int)(view_bottom - 1) <= y)))) {
    need_scroll = 1;
  }
  if (need_scroll != 0) {
    blink.unit_skip_tick = 1;
    blink.unit_phase = moving_units_mode == 0;
    center_on(x,y);
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
  net.map_redraw_busy = 1;
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
        scrolled = map_views_block.map_view_recs[view].scroll_if_near_edge(x,y);
        result = result & scrolled;
      }
    }
  }
  net.map_redraw_busy = 0;
  return result;
}

/* 004106fd auto */
int MapView::point_unit_dir(int mouse_x,int mouse_y,i32 unit_x,i32 unit_y)

{
  if (!map_camera_pick(this,&mouse_x,&mouse_y)) return 8;
  short client_dx;
  short client_dy;
  int dir;
  int tile_sx[2];
  int off_y;
  int off_x;
  int tile_sy;
  
  tile_to_screen((iptr)tile_sx,(iptr)&tile_sy,unit_x,unit_y);
  if (((short)mouse_x < tile_sx[0]) || ((short)mouse_y < tile_sy)) {
    dir = 8;
  }
  else if (((int)(short)mouse_x < (int)(tile_width_u + tile_sx[0])) &&
          ((int)(short)mouse_y < (int)(tile_height_u + tile_sy))) {
    off_x = (short)mouse_x - tile_sx[0];
    off_y = (short)mouse_y - tile_sy;
    client_dx = (short)mouse_x - (short)client_x;
    client_dy = (short)mouse_y - (short)client_y;
    if ((client_dx < 0) || (client_dy < 0)) {
      dir = 8;
    }
    else if (((int)client_dx < (int)client_width) && ((int)client_dy < (int)client_height)) {
      dir = overlay_surface.get_pixel(off_x,off_y);
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
  if ((city_turn.processing == 0) && (view->cursor_id_uptr != 0x1fe)) {
    check = ui_paradrop_cursor_active();
    if (check == 0) {
      cursor_id = 0x201;
      if ((((((byte)game.game_options & OPT_MOVE_WITH_MOUSE) != 0) && (moving_units_mode == 1)) &&
          (unit = (iptr)game.active_unit, -1 < unit)) &&
         ((dir = view->point_unit_dir(mouse_x,mouse_y,(int)units[unit].x,
                                (int)units[unit].y), -1 < dir && (dir < 8)))) {
        cursor_id = (dir + 1U & 7) + 500;
      }
    }
    else {
      cursor_id = 0x202;
      unit = (iptr)game.active_unit;
      check = view->screen_to_tile((iptr)&x2,(iptr)&y2,mouse_x,mouse_y);
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
      ((WndBase *)&view->window)->set_cursor_now(cursor_id,1);
    }
  }
  return;
}

/* 00410a64 auto */
void __cdecl mapview_cancel_drag(void)

{
  if (-1 < map_click.drag_hold_timer) {
    game_timer_destroy(map_click.drag_hold_timer);
    map_click.drag_hold_timer = -1;
  }
  if ((-1 < map_click.drag_view_index) && ((((i32 *)&map_views_block.mapview_cursor_id))[map_click.drag_view_index * 0xfc] == 0x1fe)) {
    (((i32 *)&map_views_block.mapview_cursor_id))[map_click.drag_view_index * 0xfc] = 0x201;
    ((WndBase *)&map_views_block.map_view_recs[map_click.drag_view_index].window)->set_cursor_now(0x201,1);
    ((WndBase *)&map_views_block.map_view_recs[map_click.drag_view_index].window)->release_capture();
  }
  map_click.drag_view_index = 0xffffffff;
  return;
}

/* 00410b23 auto */
void __cdecl mapview_drag_hold_expired(void)

{
  if (-1 < map_click.drag_hold_timer) {
    game_timer_destroy(map_click.drag_hold_timer);
    map_click.drag_hold_timer = -1;
    if (-1 < map_click.drag_view_index) {
      (((i32 *)&map_views_block.mapview_cursor_id))[map_click.drag_view_index * 0xfc] = 0x1fe;
      ((WndBase *)&map_views_block.map_view_recs[map_click.drag_view_index].window)->set_cursor_now(0x1fe,1);
      ((WndBase *)&map_views_block.map_view_recs[map_click.drag_view_index].window)->set_capture();
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
  
  if ((city_turn.processing == 0) && ((game.multiplayer == '\0' || (net.ui_input_busy == 0)))) {
    net.ui_input_busy = 1;
    child_wnd = wnd_get_active_child();
    if (child_wnd == 0) {
      view = 0;
    }
    else {
      view = (MapView *)(child_wnd - 0x48);
    }
    mapview_cancel_drag();
    if ((moving_units_mode == 1) && (paradrop = ui_paradrop_cursor_active(), paradrop == 0)) {
      map_click.drag_view_index = (int)view->map_view_index;
      shift_state = GetAsyncKeyState(0x10);
      if ((char)((ushort)shift_state >> 8) == '\0') {
        map_click.drag_hold_timer = game_timer_create((TimerFn)mapview_drag_hold_expired,400,1);
      }
      else {
        (((i32 *)&map_views_block.mapview_cursor_id))[map_click.drag_view_index * 0xfc] = 0x1fe;
        ((WndBase *)&map_views_block.map_view_recs[map_click.drag_view_index].window)->set_cursor_now(0x1fe,1)
        ;
        ((WndBase *)&map_views_block.map_view_recs[map_click.drag_view_index].window)->set_capture();
      }
    }
    net.ui_input_busy = 0;
  }
  return;
}

/* 00410cfa auto */
void __cdecl mapview_on_lbuttonup_any(void)

{
  int view;
  
  if ((city_turn.processing == 0) && ((game.multiplayer == '\0' || (net.ui_input_busy == 0)))) {
    net.ui_input_busy = 1;
    view = wnd_get_active_child();
    if (view == 0) {
      view = 0;
    }
    else {
      view -= 0x48;
    }
    if (-1 < map_click.drag_hold_timer) {
      mapview_cancel_drag();
    }
    ((WndBase *)(view + 0x48))->release_capture();
    net.ui_input_busy = 0;
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
      ((WndBase *)&map_views_block.map_view_recs[view].window)->set_cursor_now(0x202,1);
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
        ((WndBase *)&map_views_block.map_view_recs[view].window)->set_cursor_now(0x201,1);
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
  
  client_w = main_frame_wnd_block.main_frame_wnd.client_width();
  minimap_w = rect_width((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  height2 = main_frame_wnd_block.main_frame_wnd.client_height();
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
    ((WndBase *)((iptr)view + 0x48))->minimize();
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
  ((WndBase *)((iptr)view + 0x48))->get_rect_in_parent(
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
  ((WndBase *)((iptr)view + 0x48))->get_rect_in_parent(
             (iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + (int)view->map_view_index * 0x10));
  return;
}

/* 00413476 auto */
void MapView::update_title()

{
  bool show_mode;
  iptr adjective;
  int view;
  uint mode;
  
  mode = (uint)((open_u & 0x10000) != 0);
  if ((open_u & 0x20000) != 0) {
    mode += 2;
  }
  text_clear();
  if (map_view_index == 0) {
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
  set_caption((const char *)&textbuf.buf);
  return;
}

/* 004135ab auto */
void __cdecl mapview_on_caption_button(i32 button)

{
  iptr child_wnd;
  MapView *this2;
  
  if ((game.multiplayer == '\0') || (net.ui_input_busy == 0)) {
    net.ui_input_busy = 1;
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
      this2->wnd_destroy_with_surface();
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
      break;
    case 2:
      if (!map_camera_zoom_step(this2,1) && (short)this2->zoom_u < 8) {
        *(short *)&this2->zoom_u = (short)this2->zoom_u + 1;
        this2->redraw(human_civ[0],1);
      }
      break;
    case 3:
      if (!map_camera_zoom_step(this2,-1) && -7 < (short)this2->zoom_u) {
        *(short *)&this2->zoom_u = (short)this2->zoom_u - 1;
        this2->redraw(human_civ[0],1);
      }
      break;
    case 4:
      *(ushort *)((int)&this2->open_u + 2) = *(short *)((int)&this2->open_u + 2) + 1U & 3;
      this2->update_title();
      this2->redraw(human_civ[0],1);
    }
    net.ui_input_busy = 0;
  }
  return;
}

/* 00413717 auto */
void MapView::create()

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
  
  init_state(map_view_index);
  if ((turn_loop.resume_civ == '\0') || (destroyed_civ_count_block.saved_layout_valid == 0)) {
    *(i16 *)&zoom_u = 0;
    if (999 < caption_font_block.screen_width && !extra_ui(X_ZOOM_1)) {
      *(i16 *)&zoom_u = 2;    /* X43: 1:1 (zoom 0) whatever the screen */
    }
    if (map_view_index == 0) {
      mapview_calc_rect();
    }
  }
  if (map_view_index == 0) {
    *(i16 *)((int)&open_u + 2) = 3;
  }
  else {
    *(i16 *)((int)&open_u + 2) = 2;
  }
  if ((map_view_index == 0) || ((short)open_u != 0)) {
    left = *(int *)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + map_view_index * 0x10);
    top = *(int *)(((i8 *)&map_view_saved_rects_block.map_view_saved_rect_tops) + map_view_index * 0x10);
    width2 = rect_width((RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + map_view_index * 0x10));
    width2 -= ui_metrics.dlgframe_w2;
    height2 = rect_height((RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + map_view_index * 0x10));
    height2 -= ui_metrics.dlgframe_h2;
  }
  else {
    left = map_view_index * 0x28;
    top = map_view_index * 0x28;
    width2 = 500;
    height2 = 300;
    rect_set_xywh((iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + map_view_index * 0x10),left,top,
                       ui_metrics.dlgframe_w2 + 500,ui_metrics.dlgframe_h2 + 300);
  }
  *(i16 *)&open_u = 1;
  create_arg9 = 0;
  create_arg8 = 0;
  create_arg7 = 6;
  wnd_style = 6;
  title_text = (i32)label_string(label_table.string_ids[0x4]);
  this->FrameWindow::create((const char *)title_text,wnd_style,left,top,width2,height2,create_arg7,create_arg8,create_arg9)
  ;
  ((ModalMenuWindow *)&window)->set_min_size(0x100,0x80);
  set_redraw_callback(mapview_on_redraw);
  window.wnd_handlers.set_lbuttonup_handler(mapview_on_lbuttonup);
  window.wnd_handlers.set_rbuttonup_handler(mapview_on_rbuttonup);
  window.wnd_handlers.set_lbuttondown_handler((WndMouseFn)mapview_on_lbuttondown);
  window.wnd_handlers.set_lbuttonup_any_handler((WndMouseFn)mapview_on_lbuttonup_any);
  window.wnd_handlers.set_mousemove_handler(mapview_on_mousemove);
  window.wnd_handlers.set_dblclick_handler(map_window_double_click);
  window.wnd_handlers.set_char_handler((WndKeyFn)map_window_ascii);
  window.wnd_handlers.set_keydown_handler(map_window_key);
  window.wnd_handlers.set_close_handler((WndCloseFn)mapview_on_close);
  window.wnd_handlers.set_size_handler((WndSizeFn)mapview_on_size);
  window.wnd_handlers.set_move_handler((WndMouseFn)mapview_on_move);
  update_title();
  if (map_view_index != 0) {
    add_caption_button((iptr)&caption_close_button_sprite_block.caption_close_button_sprite,1);
  }
  add_caption_button((iptr)&caption_zoom_out_sprite,2);
  add_caption_button((iptr)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite,3);
  if (map_view_index != 0) {
    add_caption_button((iptr)&caption_view_mode_button_sprite,4);
  }
  set_caption_button_handler(mapview_on_caption_button);
  repaint();
  if (game.multiplayer != 1) {
    this->SurfaceWindow::show();
  }
  if (map_view_index != 0) {
    ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
    ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  }
  return;
}

/* 00413a90 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl mapviews_create_all(void)

{
  int view;
  
  for (view = 0; view < 8; view++) {
    map_views_block.map_view_recs[view].init_state(view);
    if (turn_loop.resume_civ == '\0') {
      if (view == 0) {
        (*(i32 *)&map_views_block.map_view_open_flags) = 1;
      }
      else {
        map_views_block.map_view_recs[view].open = 0;
      }
    }
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      map_views_block.map_view_recs[view].create();
    }
  }
  map_views_block.main_view_status_shown_i32 = 1;
  if (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0) {
    map_views_block.main_view_timer_shown[0] = 1;
  }
  wnd_modal_stack_below_block.timer_alt_kind_request = 1;
  blink_timers.unit = game_timer_create((TimerFn)unit_blink_timer_tick,0x96,0xffffffff);
  blink_timers.end_of_turn = game_timer_create((TimerFn)eot_blink_timer_tick,500,0xffffffff);
  wnd_modal_stack_below_block.timer_alt_kind_request = 0;
  return;
}

/* 00413bd1 auto */
void __cdecl mapviews_destroy_all(void)

{
  int view;
  
  wnd_modal_stack_below_block.timer_alt_kind_request = 1;
  game_timer_destroy(blink_timers.unit);
  game_timer_destroy(blink_timers.end_of_turn);
  wnd_modal_stack_below_block.timer_alt_kind_request = 0;
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      map_views_block.map_view_recs[view].wnd_destroy_with_surface();
    }
  }
  return;
}

/* 0046ab30 auto */
ClickMap *ClickMap::construct()

{
  return this;
}

/* 0046ab49 auto */
void ClickMap::destruct()

{
  return;
}

/* 0046ab5f auto */
/* Library Function - Single Match
    public: void __thiscall CRichEditDoc::InvalidateObjectCache(void)
   
   Library: Visual Studio 1998 Debug */

void ClickMap::clear()

{
  entry_count = 0;
  return;
}

/* 0046ab82 auto */
void ClickMap::remove_at(int index)

{
  int i;
  
  for (i = index; i < (int)(entry_count - 1); i++) {
    hit_regions[i] = hit_regions[i + 1];
  }
  entry_count--;
  return;
}

/* 0046ac89 auto */
void ClickMap::remove_by_id(int id)

{
  u32 count;
  u32 idx;
  
  count = entry_count;
  while (idx = count - 1, -1 < (int)idx) {
    count = idx;
    if (hit_regions[idx].value == id) {
      remove_at(idx);
    }
  }
  return;
}

/* 0046ace7 auto */
u32 ClickMap::add(iptr id,i32 value,i32 x,i32 y, iptr width,iptr height)

{
  u32 index;
  
  index = 0xffffffff;
  if ((int)entry_count < 200) {
    index = entry_count;
    entry_count++;
    rect_set_xywh((iptr)&hit_regions[index],x,y,width,height);
    hit_regions[index].id = id;
    hit_regions[index].value = value;
  }
  return index;
}

/* 0046ad85 auto */
int ClickMap::hit_test(int x,int y,iptr out_id,iptr out_value)

{
  int hit;
  int i;
  
  hit = -1;
  for (i = 0; i < (int)entry_count; i++) {
    if ((((hit_regions[i].left <= x) &&
         (x < hit_regions[i].right)) &&
        (hit_regions[i].top <= y)) &&
       (y < hit_regions[i].bottom)) {
      hit = i;
      break;
    }
  }
  if (-1 < hit) {
    if (out_id != 0) {
      *(i32 *)out_id = hit_regions[hit].id;
    }
    if (out_value != 0) {
      *(i32 *)out_value = hit_regions[hit].value;
    }
  }
  return hit;
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
  
  in_view = ((MapView *)((i8 *)&map_views_block.map_views))->tile_in_view(x,y);
  if (in_view != 0) {
    ((MapView *)((i8 *)&map_views_block.map_views))->tile_to_screen((iptr)&screen_x,(iptr)&screen_y,x,y);
    width = gfx_zoom_scale(0x40,(int)map_views_block.mapview_zoom[0]);
    height = gfx_zoom_scale(0x20,(int)map_views_block.mapview_zoom[0]);
    rect_set_xywh((iptr)rect,screen_x,screen_y,width,height);
    __itoa(number,num_text,10);
    gfx_set_text_color_index(10);
    rect_offset((iptr)rect,0xffffffff,0xffffffff);
    ((DibSurface *)((i8 *)&map_views_block.map_views))->draw_text_in_rect((iptr)&map_views_block.tile_number_font,(const char *)num_text,(RECT *)rect,0);
    rect_offset((iptr)rect,3,3);
    ((DibSurface *)((i8 *)&map_views_block.map_views))->draw_text_in_rect((iptr)&map_views_block.tile_number_font,(const char *)num_text,(RECT *)rect,0);
    rect_offset((iptr)rect,0xfffffffe,0xfffffffe);
    gfx_set_text_color_index(color);
    ((DibSurface *)((i8 *)&map_views_block.map_views))->draw_text_in_rect((iptr)&map_views_block.tile_number_font,(const char *)num_text,(RECT *)rect,0);
    ((DibSurface *)((i8 *)&map_views_block.map_views))->wnd_blit_surface_rect((iptr)rect);
  }
  return;
}

/* 00479ede auto */
void MapView::init_state(u32 view_index)

{
  map_camera_forget(this);
  map_view_index = view_index;
  if (view_index == 0) {
    *(i16 *)((int)&open_u + 2) = 1;
  }
  else {
    *(i16 *)((int)&open_u + 2) = 0;
  }
  cursor_id_uptr = 0xffffffff;
  if (turn_loop.resume_civ == '\0') {
    *(i16 *)&zoom_u = 0;
    *(i16 *)&center_x_u = map.width >> 1;
    *(i16 *)((int)&center_x_u + 2) = map.height >> 1;
    if ((center_x_u & 1) != 0) {
      *(short *)&center_x_u = (short)center_x_u - 1;
    }
    if ((center_x_u & 0x10000) != 0) {
      *(short *)&center_x_u = (short)center_x_u + 1;
    }
  }
  cached_zoom = 0xfffffc19;
  return;
}

/* 00479fbe auto */
void MapView::update_layout()

{
  u32 scaled;
  i32 wrapped_x;
  uptr wrapped;
  uint clamped;
  i8 dir_mask_rect[16];
  i8 pick_mask_rect[16];
  uptr font_size;
  
  layout_width_u = client_width;
  layout_height_u = client_height;
  scaled = zoom_scale(0x40);
  tile_width_u = scaled;
  scaled = zoom_scale(0x20);
  tile_height_u = scaled;
  half_tile_width_u = (int)tile_width_u / 2;
  half_tile_height_u = (int)tile_height_u / 2;
  quarter_tile_width_u = (int)(tile_width_u + ((int)tile_width_u >> 0x1f & 3U)) >> 2;
  quarter_tile_height = (int)(tile_height_u + ((int)tile_height_u >> 0x1f & 3U)) >> 2;
  field_320 = tile_width_u;
  unit_sprite_height_u = tile_height_u + half_tile_height_u;
  half_view_width = (int)((layout_width_u - 1) + tile_width_u) / (int)tile_width_u;
  field_2fc = (int)((layout_height_u - 1) + tile_height_u) / (int)tile_height_u;
  tile_columns = (int)(half_tile_width_u * 3 - 1 + layout_width_u) / (int)tile_width_u;
  field_304 = (int)(half_tile_height_u * 3 - 1 + layout_height_u) / (int)tile_height_u;
  margin_x_u = 0;
  margin_y_u = 0;
  if ((int)map.width / 2 < (int)half_view_width) {
    half_view_width = (int)map.width / 2;
    margin_x_u = (int)(layout_width_u - tile_width_u * half_view_width) >> 1;
    tile_columns = half_view_width + 1;
  }
  if ((int)map.height < (int)field_2fc) {
    field_2fc = (int)map.height / 2;
    margin_y_u = (int)(layout_height_u - tile_height_u * field_2fc) >> 1;
    field_304 = (int)map.height + 1;
  }
  wrapped_x = map_wrap_x((int)(short)center_x_u);
  *(short *)&center_x_u = (short)wrapped_x;
  origin_y_uptr = (int)*(short *)((int)&center_x_u + 2) - field_2fc;
  wrapped = map_wrap_x((int)(short)center_x_u - half_view_width);
  origin_x_uptr = wrapped;
  if (((game.rules & RULE_FLAT_WORLD) != 0) && (map.width + 1 < (int)(half_view_width * 2 + origin_x_uptr))
     ) {
    origin_x_uptr = map.width + 1 + half_view_width * -2;
  }
  if (map.height + 1 < (int)(field_2fc * 2 + origin_y_uptr)) {
    origin_y_uptr = map.height + 1 + field_2fc * -2;
  }
  clamped = origin_x_uptr;
  if (0x7fffffff < clamped) {
    clamped = 0xffffffff;
  }
  origin_x_uptr = clamped;
  clamped = origin_y_uptr;
  if (0x7fffffff < clamped) {
    clamped = 0xffffffff;
  }
  origin_y_uptr = clamped;
  if ((origin_x_uptr & 1) != 0) {
    origin_x_uptr--;
  }
  if ((origin_y_uptr & 1) != 0) {
    origin_x_uptr++;
  }
  wrapped = map_wrap_x(tile_columns * 2 - 2 + origin_x_uptr);
  end_x_uptr = wrapped;
  end_y_u = field_304 * 2 - 2 + origin_y_uptr;
  if ((int)(short)zoom_u != cached_zoom) {
    map_surface.DibSurface::create(tile_width_u,tile_height_u);
    overlay_surface.DibSurface::create(tile_width_u,tile_height_u);
    cached_zoom = (int)(short)zoom_u;
    apply_zoom();
    ((Sprite *)((i8 *)&tile_pick_mask_sprite_block.tile_pick_mask_sprite))->draw((iptr)pick_mask_rect,(iptr)&map_surface_head,0,0);
    ((Sprite *)((i8 *)&tile_direction_mask_sprite))->draw((iptr)dir_mask_rect,(iptr)&overlay_surface_head,0,0);
    gfx_reset_zoom_scale();
  }
  font_size = (int)(tile_height_u * 3) / 5;
  if (tile_font_size != font_size) {
    tile_font.set(0,font_size);
    tile_font_size = font_size;
  }
  font_size = (int)(tile_height_u + ((int)tile_height_u >> 0x1f & 3U)) >> 2;
  if (tile_small_font_size != font_size) {
    tile_small_font.set(1,font_size);
    tile_small_font_size = font_size;
  }
  map_camera_layout(this);
  return;
}

/* 0047a540 auto */
i32 MapView::screen_to_tile(iptr out_x,iptr out_y,i32 mouse_x,i32 mouse_y)

{
  if (!map_camera_pick(this,&mouse_x,&mouse_y)) return 1;
  short px;
  short py;
  i32 result;
  iptr pixel;
  iptr quadrant;
  iptr tile_x;
  short tile_y;
  
  px = (short)mouse_x - (short)client_x;
  py = (short)mouse_y - (short)client_y;
  if ((px < 0) || (py < 0)) {
    result = 1;
  }
  else if (((int)px < (int)client_width) && ((int)py < (int)client_height)) {
    px -= (short)margin_x_u;
    py -= (short)margin_y_u;
    tile_x = ((int)px / (int)tile_width_u) * 2 + origin_x_u + 1;
    tile_y = (short)((int)py / (int)tile_height_u) * 2 + (short)origin_y_u + 1;
    pixel = map_surface.get_pixel(
                       (int)(short)((int)px % (int)tile_width_u),
                       (int)(short)((int)py % (int)tile_height_u));
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
void MapView::tile_to_screen(iptr out_x,iptr out_y,int x,int y)

{
  uptr top;
  int dx;
  
  dx = map_wrap_x(x - origin_x_uptr);
  top = origin_y_uptr;
  *(u32 *)out_x = half_tile_width_u * (dx - 1) + margin_x_u;
  *(u32 *)out_y = half_tile_height_u * ((y - top) - 1) + margin_y_u;
  *(u32 *)out_x = *(int *)out_x + client_x;
  *(u32 *)out_y = *(int *)out_y + client_y;
  return;
}

/* Part of MapView::draw_tile_terrain (0047a8c9): an ocean tile: the plain ocean when zoomed
   far out, else the coast quarters and the river mouths. */
void MapView::draw_tile_ocean(iptr surface,int screen_x,int screen_y,iptr x,int y,int zoom,u32 half_w,u32 half_h)
{
  const int detail_zoom = map_camera_topology_zoom((void *)surface,zoom);
  int check;
  byte *known;
  i8 mouth_rect[16];
  i8 coast_rect[16];
  i8 ocean_rect[16];
  iptr x2;
  int variant;
  iptr y2;
  iptr dir;
  int quarter_y;
  int quarter_x;
  iptr quarter;

  if (detail_zoom < -3) {
    ((Sprite *)((i8 *)&terrain_base_sprites_block.terrain_ocean_sprite))->draw((iptr)ocean_rect,surface,screen_x,screen_y);
  }
  else {
    variant = tile_calc_coast_masks(x,y);
    for (quarter = 0; quarter < 4; quarter++) {
      quarter_x = (coast_quarter_dx_table[quarter] + 1) * ((int)half_w >> 1) + screen_x;
      quarter_y = coast_quarter_dy_table[quarter] * ((int)half_h >> 1) + screen_y;
      ((Sprite *)
                 (((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + quarter * 0x3c + (uint)(byte)(((i8 *)&scenario_title_surface_block.coast_quarter_land_masks))[quarter] * 0xf0))->draw(
                 (iptr)coast_rect,surface,quarter_x,quarter_y);
    }
    for (dir = 0; dir < 4; dir++) {
      x2 = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + x);
      y2 = (((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + y;
      check = map_in_bounds(x2,y2);
      if ((check != 0) && (known = (byte *)tile_at(x2,y2), (*known & 0x80) != 0)) {
        ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + dir * 0x3c))->draw((iptr)mouth_rect,surface,
                   screen_x,screen_y);
      }
    }
  }
}

/* Part of MapView::draw_tile_terrain (0047a8c9): a land tile: the base terrain dithered with
   its neighbours, forest/mountain/hill shapes joined to like neighbours, the
   river, irrigation and farmland, the grassland shield. */
void MapView::draw_tile_land(iptr surface,int screen_x,int screen_y,iptr x,int y,int zoom,uint terrain2,uint improvements2,uint has_river,u32 half_w,u32 half_h,uint *dither_terrain)
{
  const int detail_zoom = map_camera_detail_zoom((void *)surface,zoom);
  const int topology_zoom = map_camera_topology_zoom((void *)surface,zoom);
  byte nbr_raw;
  uint new_flags;
  int check;
  u32 dither_dy;
  u32 dither_dx;
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
  iptr x2;
  uint nbr_terrain;
  int variant;
  iptr quad;
  int river_mask;
  byte *nbr_tile;
  uint nbr_improvements;
  iptr y2;
  int draw_y;
  iptr dir;
  int draw_x;
  uint nbr_byte;
  uint irrig_flags;

  variant = 0;
  river_mask = 0;
  if (topology_zoom < -4) {
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
  ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->draw((iptr)base_rect,surface,screen_x,
             screen_y);
  if (-5 < detail_zoom) {
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
        ((Sprite *)
                   (((i8 *)&map_cursor_sprite_block.terrain_dither_sprites) + quad * 0x3c + dither_terrain[quad] * 0xf0))->draw((iptr)dither_rect
                   ,surface,draw_x,draw_y);
      }
    }
  }
  if (terrain2 == TERRAIN_FOREST) {
    ((Sprite *)(((i8 *)&forest_sprites) + variant * 0x3c))->draw((iptr)forest_rect,surface,screen_x,
               screen_y);
  }
  else if (terrain2 == TERRAIN_MOUNTAINS) {
    ((Sprite *)(((i8 *)&mountain_sprites) + variant * 0x3c))->draw((iptr)mountain_rect,surface,screen_x,
               screen_y);
  }
  else if (terrain2 == TERRAIN_HILLS) {
    ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + variant * 0x3c))->draw((iptr)hill_rect,surface,screen_x,
               screen_y);
  }
  if (((-4 < detail_zoom) && (map_show_hidden_terrain == 0)) && (((byte)improvements2 & 0xc) == 0xc)) {
    ((Sprite *)((i8 *)&land_improvement_sprites_block.farmland_sprite))->draw((iptr)farmland_rect,surface,screen_x,screen_y);
  }
  if (((-7 < topology_zoom) && (has_river != 0)) && (map_show_hidden_terrain == 0)) {
    ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + river_mask * 0x3c))->draw((iptr)river_rect,surface,screen_x,
               screen_y);
  }
  if (-4 < detail_zoom) {
    irrig_flags = 0;
    if (((byte)improvements2 & 0x42) == 2) {
      for (dir = 2; dir < 5; dir += 2) {
        x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
        y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + y;
        check = map_in_bounds(x2,y2);
        new_flags = irrig_flags;
        if (((check != 0) &&
            (nbr_improvements = tile_improvements(x2,y2), new_flags = irrig_flags,
            (nbr_improvements & TILE_IRRIGATION) != 0)) && (new_flags = irrig_flags | 1, (nbr_improvements & TILE_MINE) != 0)) {
          new_flags = irrig_flags | 3;
        }
        irrig_flags = new_flags;
      }
    }
    if (map_show_hidden_terrain == 0) {
      if ((((byte)improvements2 & 0xc) == 4) || (irrig_flags == 1)) {
        ((Sprite *)((i8 *)&land_improvement_sprites_block.land_improvement_sprites))->draw((iptr)irrig_rect,surface,screen_x,screen_y);
      }
      else if ((irrig_flags & 2) != 0) {
        ((Sprite *)((i8 *)&land_improvement_sprites_block.farmland_sprite))->draw((iptr)farm_rect,surface,screen_x,screen_y);
      }
    }
  }
  if (((-4 < detail_zoom) && (terrain2 == TERRAIN_GRASSLAND)) &&
     (check = tile_grassland_has_shield(x,y), check != 0)) {
    ((Sprite *)((i8 *)&grassland_shield_sprite))->draw((iptr)shield_rect,surface,screen_x,screen_y);
  }
}

/* Part of MapView::draw_tile_terrain (0047a8c9): the road and railroad pieces toward the
   neighbours that have one (a dot when none). */
void MapView::draw_tile_roads(iptr surface,int screen_x,int screen_y,iptr x,int y,int viewer_civ,uint improvements2)
{
  int check;
  byte *known;
  i8 road_dot_rect[16];
  i8 rail_dot_rect[16];
  i8 rail_rect[16];
  i8 road_rect[16];
  iptr x2;
  int rail_count;
  int road_count;
  uint nbr_improvements;
  iptr y2;
  iptr dir;
  int has_rail;

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
        ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + (dir * 4 + 4) * 0xf))->draw((iptr)road_rect,
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
          ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + (dir * 4 + 4) * 0xf))->draw((iptr)rail_rect,
                     surface,screen_x,screen_y);
        }
      }
    }
  }
  check = tile_city_owner(x,y);
  if (check < 0) {
    if (has_rail == 0) {
      if (road_count == 0) {
        ((Sprite *)((i8 *)&road_sprites_block.road_sprites))->draw((iptr)road_dot_rect,surface,screen_x,screen_y);
      }
    }
    else if (rail_count == 0) {
      ((Sprite *)((i8 *)&road_sprites_block.railroad_sprites))->draw((iptr)rail_dot_rect,surface,screen_x,screen_y);
    }
  }
}

/* Part of MapView::draw_tile_terrain (0047a8c9): pollution, the hut, the fortress, the airbase
   (with a plane when one is there, in the owner's color). */
void MapView::draw_tile_specials(iptr surface,int screen_x,int screen_y,iptr x,int y,uint improvements2,u32 tile_w,u32 tile_h,u32 half_h)
{
  int check;
  iptr civ;
  i32 color;
  i8 airbase_rect[16];
  i8 fortress_rect[16];
  i8 hut_rect[16];
  i8 pollution_rect[16];
  iptr unit;
  int variant;
  i8 airbase_area[16];

  if ((improvements2 & 0x80) != 0) {
    ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->draw((iptr)pollution_rect,surface,screen_x,screen_y);
  }
  check = tile_has_hut(x,y);
  if ((check != 0) && (map_show_hidden_terrain == 0)) {
    ((Sprite *)((i8 *)&hut_sprite))->draw((iptr)hut_rect,surface,screen_x,screen_y);
  }
  if ((((byte)improvements2 & 0x42) == 0x40) && (map_show_hidden_terrain == 0)) {
    ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->draw((iptr)fortress_rect,surface,screen_x,screen_y - half_h);
  }
  if ((((byte)improvements2 & 0x42) == 0x42) && (map_show_hidden_terrain == 0)) {
    variant = 0;
    for (unit = tile_first_unit(x,y); -1 < unit;
        unit = unit_next_in_stack(unit)) {
      if (unit_types[units[unit].type].domain == DOMAIN_AIR) {
        variant = 1;
        break;
      }
    }
    if (hd_enabled && variant) {
      hd_sprite_tint((Sprite *)&airbase_sprites_block.airbase_sprites + variant,8,
                     civ_color_primary(tile_owner(x,y)));
    }
    ((Sprite *)(((i8 *)&airbase_sprites_block.airbase_sprites) + variant * 0x3c))->draw((iptr)airbase_rect,surface,screen_x,
               screen_y - half_h);
    rect_set_xywh((iptr)airbase_area,screen_x,screen_y - half_h,tile_w,half_h + tile_h);
    civ = tile_owner(x,y);
    color = civ_color_primary(civ);
    if (!hd_enabled) ((DibSurface *)surface)->replace_color((iptr)airbase_area,0x106,color);
  }
}

/* Part of MapView::draw_tile_terrain (0047a8c9): darkens the quarters toward unexplored
   neighbours (in a city window: also those outside the city radius). */
void MapView::draw_tile_unexplored_edges(iptr surface,int screen_x,int screen_y,iptr x,int y,int viewer_civ,iptr city_tile_idx,u32 half_w,u32 half_h)
{
  int check;
  u32 unexp_dy;
  u32 unexp_dx;
  i8 unexp_rect[16];
  iptr x2;
  int abs_dy;
  int abs_dx;
  int nbr_hidden;
  iptr y2;
  int draw_y;
  iptr dir;
  int draw_x;

  for (dir = 0; dir < 4; dir++) {
    x2 = map_wrap_x((((i8 *)&diag_neighbor_bits_block.diag_dx_table))[dir] + x);
    y2 = (((i8 *)&diag_neighbor_bits_block.diag_dy_table))[dir] + y;
    nbr_hidden = 0;
    if (city_tile_idx < 0) {
      check = map_in_bounds(x2,y2);
      if (check == 0) continue;
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
    }
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
      ((Sprite *)(((i8 *)&map_cursor_sprite_block.unexplored_dither_sprites) + dir * 0x3c))->draw((iptr)unexp_rect,surface,
                 draw_x,draw_y);
    }
  }
}

/* 0047a8c9 auto */
void MapView::draw_tile_terrain(iptr surface,int screen_x,int screen_y,iptr x,int y, int viewer_civ,int zoom,iptr city_tile_idx)

{
  const int detail_zoom = map_camera_detail_zoom((void *)surface,zoom);
  int check;
  byte *known;
  i8 mine_rect[16];
  i8 resource_rect[16];
  i8 hidden_rect[16];
  uint terrain_byte;
  uint hidden;
  uint improvements2;
  byte *tile;
  u32 half_w;
  uint terrain2;
  u32 half_h;
  iptr dir;
  u32 tile_w;
  int resource;
  uint has_river;
  u32 tile_h;
  uint dither_terrain[4];
  
  if ((short)zoom_u == zoom) {
    tile_w = tile_width_u;
    tile_h = tile_height_u;
    half_w = half_tile_width_u;
    half_h = half_tile_height_u;
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
      ((Sprite *)((i8 *)&unexplored_tile_sprite))->draw((iptr)hidden_rect,surface,screen_x,screen_y);
      gfx_reset_zoom_scale();
      return;
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
  if (terrain2 == TERRAIN_OCEAN) {
    draw_tile_ocean(surface,screen_x,screen_y,x,y,zoom,half_w,half_h);
  }
  else {
    draw_tile_land(surface,screen_x,screen_y,x,y,zoom,terrain2,improvements2,has_river,half_w,half_h,dither_terrain);
  }
  if (((-4 < detail_zoom) && (hidden == 0)) &&
     (resource = tile_resource(x,y), resource != 0)) {
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78 + (resource * 4 - 4) * 0xf))->draw(
               (iptr)resource_rect,surface,screen_x,screen_y);
  }
  if (((-4 < detail_zoom) && (map_show_hidden_terrain == 0)) &&
     (((improvements2 & 0x10) != 0 || (check = tile_city_owner(x,y), -1 < check)))) {
    draw_tile_roads(surface,screen_x,screen_y,x,y,viewer_civ,improvements2);
  }
  if (((-4 < detail_zoom) && (map_show_hidden_terrain == 0)) && (((byte)improvements2 & 0xc) == 8)) {
    ((Sprite *)((i8 *)&land_improvement_sprites_block.mine_sprite))->draw((iptr)mine_rect,surface,screen_x,screen_y);
  }
  if ((-5 < detail_zoom) && (hidden == 0)) {
    draw_tile_specials(surface,screen_x,screen_y,x,y,improvements2,tile_w,tile_h,half_h);
  }
  if ((-4 < map_camera_topology_zoom((void *)surface,zoom)) && (-1 < viewer_civ)) {
    draw_tile_unexplored_edges(surface,screen_x,screen_y,x,y,viewer_civ,city_tile_idx,half_w,half_h);
  }
  gfx_reset_zoom_scale();
  return;
}

/* 0047ba1d auto */
void MapView::draw_tile_city(i32 screen_x,int screen_y,iptr x,iptr y)

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
       (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))) {
      gfx_draw_city((iptr)this,city,0,screen_x,screen_y - half_tile_height_u,
                         (int)(short)zoom_u);
    }
  }
  return;
}

/* 0047bba5 auto */
void MapView::draw_unit_stacked(iptr unit,uint draw_flags,i32 screen_x,i32 screen_y)

{
  gfx_draw_unit((iptr)this,unit,draw_flags | 3,screen_x,screen_y,(int)(short)zoom_u,0);
  return;
}

/* 0047bbea auto */
void MapView::draw_unit_blinking(iptr unit,i32 screen_x,i32 screen_y)

{
  if (((blink.unit_active == '\0') || (blink.unit_phase == '\0')) || (moving_units_mode != 1)) {
    gfx_draw_unit((iptr)this,unit,5,screen_x,screen_y,(int)(short)zoom_u,0);
  }
  return;
}

/* 0047bc59 auto */
void MapView::draw_unit_at(iptr unit)

{
  int screen_y;
  i32 screen_x;
  
  if (((blink.unit_active == '\0') || (blink.unit_phase == '\0')) || (moving_units_mode != 1)) {
    tile_to_screen((iptr)&screen_x,(iptr)&screen_y,(int)units[unit].x,
                       (int)units[unit].y);
    screen_y -= half_tile_height_u;
    gfx_draw_unit((iptr)this,unit,5,screen_x,screen_y,(int)(short)zoom_u,0);
  }
  return;
}

/* Shared read-only selection keeps overlay text synchronized with visibility,
 * stacked units, active-unit blinking and combat. */
int map_stack_unit(int unit,uint *flags)
{
  int selectable;
  if(unit<0)return -1;
  *flags=5;
  
  if ((combat_anim.defender < 0) || (combat_anim.defender != unit)) {
    if ((combat_anim.origin_stack_unit < 0) || (unit != combat_anim.origin_stack_unit)) {
      if ((((units[game.active_unit].x == units[unit].x) &&
           (units[game.active_unit].y == units[unit].y)) &&
          (((uint)game.human_players & 1 << (units[game.active_unit].owner & 0x1fU)) != 0)) &&
         (selectable = unit_is_selectable((int)game.active_unit), selectable != 0)) {
        unit=(int)game.active_unit;
        return (!blink.unit_active || !blink.unit_phase || moving_units_mode!=1) ? unit : -1;
      }
      *flags=7;
      return unit;
    }
    else {
      return unit;
    }
  }
  else {
    return (!blink.unit_active || !blink.unit_phase || moving_units_mode!=1) ? unit : -1;
  }
  return -1;
}

int map_tile_unit(int x,int y,int first)
{
  int result;
  iptr unit;
  int visible2;
  
  result = map_in_bounds(x,y);
  if (result != 0) {
    /* The camera already has each stack head; avoid a unit-table search per
     * label on every frame. Legacy callers request the normal lookup with -2. */
    unit = first==-2 ? tile_first_unit(x,y) : first;
    if(first!=-2 && unit_move_anim_active && tile_unit_owner(x,y)<0)unit=-1;
    if (((combat_anim.defender < 0) || (combat_anim.defender_x != x)) || (combat_anim.defender_y != y)) {
      if (((combat_anim.origin_stack_unit < 0) || (x != combat_anim.origin_x)) || (y != combat_anim.origin_y)) {
        if(unit<0)return -1;
        result = tile_city_owner(x,y);
        if (-1 < result) {
          if (moving_units_mode != 1) {
            return -1;
          }
          if (blink.unit_active == '\0') {
            return -1;
          }
          for (; (game.active_unit != unit && (-1 < unit));
              unit = unit_next_in_stack(unit)) {
          }
          if (unit < 0) {
            return -1;
          }
          result = unit_is_selectable(unit);
          if (result == 0) {
            return -1;
          }
          if (units[unit].orders == ORDER_FORTIFYING) {
            return -1;
          }
          if (units[unit].orders == ORDER_FORTIFIED) {
            return -1;
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
          return (int)unit;
        }
      }
      else {
        return combat_anim.origin_stack_unit;
      }
    }
    else {
      for (; (unit != combat_anim.defender && (-1 < unit)); unit = unit_next_in_stack(unit)) {
      }
      return (int)unit;
    }
  }
  return -1;
}

/* 0047bd04 auto */
void MapView::draw_stack_unit(iptr unit,i32 screen_x,i32 screen_y)
{
  uint flags;
  int shown=map_stack_unit((int)unit,&flags);
  if(shown>=0)gfx_draw_unit((iptr)this,shown,flags,screen_x,screen_y,(int)(short)zoom_u,0);
}

/* 0047be63 auto */
void MapView::draw_tile_units(i32 screen_x,int screen_y,int x,int y)
{
  int unit=map_tile_unit(x,y,-2);
  if(unit>=0)draw_stack_unit(unit,screen_x,screen_y-half_tile_height_u);
}

/* 0047c103 auto */
void MapView::draw_tile(iptr x,iptr y,int civ)

{
  if (!net.map_redraw_busy) map_view_cache_invalidate(this,x,y,0);
  int result;
  i8 grid_rect[16];
  i8 radius_rect[16];
  iptr city;
  i32 screen_y;
  i32 screen_x;
  int near_city;
  
  tile_to_screen((iptr)&screen_x,(iptr)&screen_y,x,y);
  draw_tile_terrain((iptr)this,screen_x,screen_y,x,y,civ,
                     (int)(short)zoom_u,-1);
  if (!map_view_cache_rendering() && hd_redraw_poll_due()) win_flush_pending_msgs();
  if (!map_view_cache_rendering() && 2 < game.multiplayer) {
    net.process_messages(1,0);
  }
  result = map_in_bounds(x,y);
  if (result != 0) {
    if (map_show_hidden_terrain == 0) {
      draw_tile_city(screen_x,screen_y,x,y);
      draw_tile_units(screen_x,screen_y,x,y);
    }
    if (((byte)game.game_options & OPT_MAP_GRID) != 0) {
      near_city = 0;
      apply_zoom();
      for (city = 0; city < game.num_cities; city++) {
        if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
           (result = map_distance_diag(x,y,(int)cities[city].x,
                                       (int)cities[city].y), result < 3)) {
          near_city = 1;
          break;
        }
      }
      if (near_city == 0) {
        ((Sprite *)((i8 *)&map_grid_sprite))->draw((iptr)grid_rect,(iptr)this,screen_x,screen_y);
      }
      else {
        ((Sprite *)((i8 *)&icons_pair_sprites_block.city_radius_grid_sprite))->draw((iptr)radius_rect,(iptr)this,screen_x,screen_y);
      }
      gfx_reset_zoom_scale();
    }
  }
  return;
}

/* 0047c3e0 auto */
i32 MapView::tile_in_view(i32 tile_x,i32 tile_y)

{
  i32 in_view;
  
  in_view = map_tile_in_box(tile_x,tile_y,origin_x_uptr,origin_y_uptr,
                             half_view_width + tile_columns,field_2fc + field_304 + 1
                            );
  return in_view;
}

/* 0047c443 auto */
void MapView::draw_city_names(i32 center_x,i32 center_y,int radius)

{
  /* Camera labels are drawn at final display resolution, after map scaling. */
  if (map_camera_surface(this)) return;
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
    text_set_font((iptr)&tile_font_head);
    for (city = 0; city < game.num_cities; city++) {
      if (cities[city].id != 0) {
        x2 = (int)cities[city].x;
        y2 = (int)cities[city].y;
        if (((radius < 0) ||
            (label_x = map_distance(x2,y2,center_x,center_y), label_x <= radius)) &&
           (label_x = tile_in_view(x2,y2), label_x != 0)) {
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
            tile_to_screen((iptr)&screen_x,(iptr)&screen_y,x2,y2);
            label_x = half_tile_width_u + screen_x;
            label_y = ((int)(tile_height_u * 3) >> 2) + screen_y;
            text_set_colors(10,0xffffffff,0xffffffff,0xffffffff);
            text_width = zoom_scale(2);
            if (text_width < 3) {
              shadow_y = zoom_scale(2);
            }
            else {
              shadow_y = 2;
            }
            shadow_y = label_y + shadow_y;
            text_width = tile_font.text_width((const char *)&cities[city].name);
            text_draw_shadowed((iptr)this,(const char *)&cities[city].name,label_x - (text_width >> 1),shadow_y);
            text_width = zoom_scale(2);
            if (text_width < 3) {
              shadow_offset = zoom_scale(2);
            }
            else {
              shadow_offset = 2;
            }
            shadow_depth = 0;
            text_color = 10;
            shadow_color = civ_color_secondary((int)cities[city].owner);
            text_set_colors(shadow_color,text_color,shadow_offset,shadow_depth);
            text_width = label_y;
            name_width = tile_font.text_width((const char *)&cities[city].name);
            text_draw_shadowed((iptr)this,(const char *)&cities[city].name,label_x - (name_width >> 1),text_width);
          }
        }
      }
    }
  }
  return;
}

/* 0047c7aa auto */
void MapView::tile_radius_rect(i32 tile_x,i32 tile_y,int radius,iptr out_rect)

{
  i8 rect[16];
  int width2;
  int top;
  int left;
  
  tile_to_screen((iptr)&left,(iptr)&top,tile_x,tile_y);
  top -= (radius * 2 + 1) * half_tile_height_u;
  left += half_tile_width_u * radius * -2;
  width2 = (radius * 2 + 1) * tile_width_u;
  rect_set_xywh((iptr)rect,left,top,width2,(radius * 4 + 3) * half_tile_height_u);
  rect_intersect_into((LPRECT)out_rect,(iptr)rect,(iptr)&client_rect);
  return;
}

/* 0047c869 auto */
void MapView::draw_tile_radius(int center_x,int center_y,int radius,i32 civ)

{
  if (!net.map_redraw_busy) map_view_cache_invalidate(this,center_x,center_y,radius);
  uint extent;
  uint row_abs;
  i32 x;
  int in_view;
  uint dx_abs;
  uint dy;
  uint dx;
  int row_extent;
  i8 clip_rect[16];
  
  tile_radius_rect(center_x,center_y,radius,(iptr)clip_rect);
  set_clip((RECT *)clip_rect);
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
        in_view = tile_in_view(x,center_y + dy);
        if (in_view != 0) {
          draw_tile(x,center_y + dy,civ);
        }
      }
    }
  }
  draw_city_names(center_x,center_y,radius);
  set_clip((RECT *)&client_rect);
  return;
}

/* 0047c9d4 auto */
void MapView::draw_all_tiles(i32 civ)

{
  if (map_view_cache_draw(this,civ)) return;
  map_view_cache_invalidate(this,0,0,-1);
  uptr origin_y;
  uptr origin_x;
  i32 x;
  uint row;
  int col;
  int row_width;
  
  if ((margin_x_u != 0) || (margin_y_u != 0)) {
    gfx_fill_surface((iptr)this,10);
  }
  for (row = 0; (int)row < (int)(field_2fc + field_304 + 1);
      row = row + 1) {
    origin_y = origin_y_uptr;
    origin_x = origin_x_uptr;
    if ((row & 1) == 0) {
      row_width = tile_columns;
    }
    else {
      row_width = half_view_width;
    }
    for (col = 0; col < row_width; col++) {
      x = map_wrap_x(col * 2 + origin_x + (row & 1));
      draw_tile(x,origin_y + row,civ);
    }
  }
  draw_city_names(0,0,0xffffffff);
  return;
}

/* 0047caea auto */
void MapView::blit_tile_radius(i32 tile_x,i32 tile_y,i32 radius)

{
  i8 rect[16];
  
  tile_radius_rect(tile_x,tile_y,radius,(iptr)rect);
  wnd_blit_surface_rect((iptr)rect);
  return;
}

/* 0047cb26 auto */
void MapView::blit_tile(i32 tile_x,i32 tile_y)

{
  blit_tile_radius(tile_x,tile_y,0);
  return;
}

/* 0047cb50 auto */
void DibSurface::mapview_blit_with_net_pump()

{
  win_flush_pending_msgs();
  if (2 < game.multiplayer) {
    net.process_messages(1,0);
  }
  wnd_blit_surface();
  win_flush_pending_msgs();
  if (2 < game.multiplayer) {
    net.process_messages(1,0);
  }
  return;
}

/* 0047cbb4 auto */
void MapView::redraw_area(i32 tile_x,i32 tile_y,i32 radius, iptr viewer_civ,int do_blit)

{
  map_view_cache_invalidate(this,tile_x,tile_y,radius);
  int in_view;
  iptr civ;
  i8 cursor_rect[16];
  i32 cursor_x;
  i32 cursor_y;
  
  if (turn_loop.active != '\0') {
    /* Blink/partial redraws poll messages once per tile too. Present the
     * finished patch once, as for full redraws, instead of repeatedly
     * composing all camera labels while the map is still being painted. */
    const bool batch_hd = hd_gpu_active && game.multiplayer < 3;
    if (batch_hd) hd_redraw_begin();
    ON_SCOPE_EXIT(if (batch_hd) hd_redraw_end());
    net.map_redraw_busy = 1;
    if (game.reveal_map == '\0') {
      civ = viewer_civ;
    }
    else {
      civ = -1;
    }
    draw_tile_radius(tile_x,tile_y,radius,civ);
    if ((((moving_units_mode == 0) && (blink.unit_phase != '\0')) && (blink.unit_active != '\0')) &&
       ((map_view_index == 0 &&
        (in_view = tile_in_view((int)map_cursor.x[0],(int)map_cursor.y[0]), in_view != 0)))) {
      tile_to_screen((iptr)&cursor_x,(iptr)&cursor_y,(int)map_cursor.x[0],(int)map_cursor.y[0]);
      apply_zoom();
      ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->draw((iptr)cursor_rect,(iptr)this,cursor_x,cursor_y);
      gfx_reset_zoom_scale();
    }
    if (do_blit != 0) {
      blit_tile_radius(tile_x,tile_y,radius);
    }
    net.map_redraw_busy = 0;
  }
  return;
}

/* 0047cced auto */
void MapView::redraw_tile(i32 tile_x,i32 tile_y)

{
  redraw_area(tile_x,tile_y,0,human_civ[0],1);
  return;
}

/* 0047cd1f auto */
void MapView::redraw_tile_neighbors(i32 tile_x,i32 tile_y)

{
  redraw_area(tile_x,tile_y,1,human_civ[0],1);
  return;
}

/* 0047cd51 auto */
void MapView::redraw(iptr viewer_civ,int do_blit)

{
  iptr civ;
  
  if (turn_loop.active != '\0') {
    const bool batch_hd = hd_gpu_active && game.multiplayer < 3;
    if (batch_hd) hd_redraw_begin();
    ON_SCOPE_EXIT(if (batch_hd) hd_redraw_end());
    net.map_redraw_busy = 1;
    layout();
    update_layout();
    this->FrameWindow::draw_caption();
    if (game.reveal_map == '\0') {
      civ = viewer_civ;
    }
    else {
      civ = -1;
    }
    draw_all_tiles(civ);
    if (extra_ui(X_SUPPRESS_POPUPS) && map_view_index == 0) {
      extras_draw_map_messages((iptr)this,0);   /* X33 */
    }
    if ((do_blit != 0) &&
       (mapview_blit_with_net_pump(), map_view_index == 0)) {
      minimap_redraw();
    }
    if ((map_click_in_progress == 0) && (net.mp_simultaneous_moves == '\0')) {
      input_flush_events();
    }
    net.map_redraw_busy = 0;
  }
  return;
}

/* 0047dce0 auto */
MapView * __thiscall mapview_ctor(MapView *self)

{
  
  self->FrameWindow::construct();
  self->tile_font.init_empty();
  self->tile_small_font.init_empty();
  surface_ctor(&self->map_surface);
  surface_ctor(&self->overlay_surface);
  self->vtable = (u32)&ptr_fun_2;
  return self;
}

/* 0047de10 auto */
/* Library Function - Single Match
    public: virtual __thiscall CBitmapButton::~CBitmapButton(void)
   
   Library: Visual Studio 1998 Debug */

void __thiscall mapview_dtor(MapView *self)

{
  map_camera_forget(self);
  int eh_state;
  
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0047de82 */ { surface_dtor((DibSurface *)(*(int *)&self + 0x3a8)); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0047de91 */ { surface_dtor((DibSurface *)(*(int *)&self + 0x360)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0047dea0 */ { ((GameFont *)(*(int *)&self + 0x340))->destroy(); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0047deaf */ { ((GameFont *)(*(int *)&self + 0x338))->destroy(); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0047debe */ { (*(FrameWindow **)&self)->FrameWindow::destruct(); }
  return;
}

/* 0047dfb0 auto */
i32 MapView::zoom_scale(i32 value)

{
  i32 scaled;
  
  scaled = gfx_zoom_scale(value,(int)(short)zoom_u);
  return scaled;
}

/* 0047dff0 auto */
void MapView::apply_zoom()

{
  gfx_set_zoom_scale((int)(short)zoom_u);
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
         (visible = map_views_block.map_view_recs[view_index].tile_in_view(x,y),
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
    ((MapView *)((i8 *)&map_views_block.map_views))->center_on(center_x,center_y);
  }
  return;
}

/* 004d8b70 auto */
void ClickMap::add_xywh(iptr id,i32 left,i32 top,i32 width, i32 height)

{
  add(id,0,left,top,width,height);
  return;
}
