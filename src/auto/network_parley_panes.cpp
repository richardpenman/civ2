/* The game, multiplayer: the parley window panes and its lists of advances, units, cities and civs: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00450480 auto */
void ParleyWindow::create_advance_list_pane(RECT *rect,int pane,int their_side)

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
  
  can_see_other = 0;
  sb_rect.left = rect->left;
  sb_rect.top = rect->top;
  sb_rect.right = rect->right;
  sb_rect.bottom = rect->bottom;
  result = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS);
  if ((((result == 0) && (result = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), result == 0)) &&
      ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_EMBASSY) == 0)) &&
     (((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_ALLIANCE) == 0 &&
      (game.reveal_map == '\0')))) {
    can_see_other = 0;
  }
  else {
    can_see_other = 1;
  }
  list_counts[pane] = 0;
  civ = human_civ[0];
  if ((their_side != 0) && (can_see_other != 0)) {
    civ = (int)parley_wnd_block.other_civ;
  }
  for (advance = 0; (int)advance < ADV_COUNT; advance++) {
    if (their_side == 0) {
      result = civ_has_advance(human_civ[0],advance);
      if ((result != 0) &&
         ((can_see_other == 0 || (result = civ_has_advance(parley_wnd_block.other_civ,advance), result == 0)))) {
        list_items[pane][(u32)list_counts[pane]] = advance;
        list_selected[pane][(u32)list_counts[pane]] = 0;
        list_counts[pane]++;
      }
    }
    else if (((human_civ[0] == civ) ||
             (result = civ_has_advance(human_civ[0],advance), result != 0)) ||
            (result = civ_has_advance(civ,advance), result == 0)) {
      if (((human_civ[0] == civ) &&
          (result = civ_has_advance(human_civ[0],advance), result == 0)) &&
         (((int)advance < 0x59 && (advances[advance].enabled == 1)))) {
        list_items[pane][(u32)list_counts[pane]] = advance;
        list_selected[pane][(u32)list_counts[pane]] = 0;
        list_counts[pane]++;
      }
    }
    else {
      list_items[pane][(u32)list_counts[pane]] = advance;
      list_selected[pane][(u32)list_counts[pane]] = 0;
      list_counts[pane]++;
    }
  }
  if ((int)(u32)list_counts[pane] < 1) {
    list_first_row[pane] = 0xffffffff;
  }
  else {
    list_first_row[pane] = 0;
  }
  list_columns[pane] = 3;
  list_rects[pane].left = (u32)rect->left;
  list_rects[pane].top = (u32)rect->top;
  list_rects[pane].right = (u32)rect->right;
  list_rects[pane].bottom = (u32)rect->bottom;
  if ((int)(u32)list_rows[pane] < (int)(u32)list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    result = sb_rect.right;
    list_rects[pane].right -= scroll_width;
    sb_rect.left = GetSystemMetrics(2);
    sb_rect.left = result - sb_rect.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (u32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[pane] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      parent2 = (Wnd *)0x0;
    }
    else {
      parent2 = &window;
    }
    ((Scrollbar *)list_scrollbars[pane])->create(parent2,pane + 0x411,
               &sb_rect,1);
    ((Scrollbar *)list_scrollbars[pane])->set_range(0,
               ((u32)list_counts[pane] - 1) - (u32)list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_pos(0);
    ((Scrollbar *)list_scrollbars[pane])->set_page_size((u32)list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_callback(
               (pane == 0 ? parley_advance_list0_on_scroll : parley_advance_list1_on_scroll));
    ((Scrollbar *)list_scrollbars[pane])->set_track_callback(
               (pane == 0 ? parley_advance_list0_on_scroll : parley_advance_list1_on_scroll));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (u32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[pane] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    parent22 = (Wnd *)0x0;
  }
  else {
    parent22 = &window;
  }
  ((Hotspot *)list_hotspots[pane])->create(parent22,pane + 0x413,
             &list_rects[pane]);
  ((Hotspot *)list_hotspots[pane])->set_click_callback(parley_advance_list_on_click);
  ((Hotspot *)list_hotspots[pane])->set_dblclick_callback(parley_advance_list_on_click);
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
  window->window.activate();
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
  window->window.activate();
  pane = control_id - 0x413;
  window->window.get_cursor_pos(&cursor_x,&cursor_y);
  if (((window->list_counts[pane] != 0) &&
      (row = parley_advance_list_hit_test(cursor_x,cursor_y,pane), -1 < row)) &&
     (row < window->list_counts[pane])) {
    key_down = window->window.is_shift_down();
    if (key_down == 0) {
      key_down = window->window.is_ctrl_down();
      if (key_down == 0) {
        for (i = 0; i < window->list_counts[pane];
            i = i + 1) {
          if (i != row) {
            window->list_selected[pane][i] = 0;
          }
        }
        window->list_anchor_row[pane] = row;
        sel_flag = &window->list_selected[pane][row];
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        window->list_anchor_row[pane] = row;
        sel_flag = &window->list_selected[pane][row];
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < window->list_counts[pane]; i++)
      {
        window->list_selected[pane][i] = 0;
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
        window->list_selected[pane][i] = 1;
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
  if (py < (int)window->list_rects[pane].top) {
    result = -1;
  }
  else if (py < (int)window->list_rects[pane].bottom) {
    if (px < (int)window->list_rects[pane].left) {
      result = -3;
    }
    else if (px < (int)window->list_rects[pane].right) {
      result = (int)window->list_rects[pane].top;
      list_height = rect_height(&window->list_rects[pane]);
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
void __cdecl parley_draw_advance_list(int pane)

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
  window->get_clip((RECT *)saved_clip);
  window->set_clip(&window->list_rects[pane]);
  window->fill_rect(&window->list_rects[pane],dlg_style.list_bg);
  if (window->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  list_left = (int)window->list_rects[pane].left;
  list_width2 = rect_width(&window->list_rects[pane]);
  row_height = rect_height(&window->list_rects[pane]);
  row_height = row_height / (window->list_rows[pane] + 1);
  tmp = row_height / 2;
  tmp2 = font2->font_cached_height();
  text_y_offset = tmp - tmp2 / 2;
  has_sel = (u8 *)&window->side_selected[pane];
  has_sel[0] = '\0';
  has_sel[1] = '\0';
  has_sel[2] = '\0';
  has_sel[3] = '\0';
  for (row = 0; row < window->list_counts[pane]; row++) {
    if (window->list_selected[pane][row] != 0) {
      has_sel = (u8 *)&window->side_selected[pane];
      has_sel[0] = 1;
      has_sel[1] = '\0';
      has_sel[2] = '\0';
      has_sel[3] = '\0';
      break;
    }
  }
  for (row = 0; row < window->list_rows[pane] + 1;
      row = row + 1) {
    row_top = (int)window->list_rects[pane].top + row * row_height +
               panel_layout.margin_y;
    SetRect(&row_rect,list_left,row_top,list_left + list_width2,row_top + row_height);
    if (((row == 0) || (window->list_first_row[pane] != -1)) &&
       (window->list_first_row[pane] + row <
        window->list_counts[pane])) {
      selected2 = (int)window->list_selected[pane][window->list_first_row[pane] + row];
      if (selected2 == 0) {
        text_color2 = dlg_style.list_text;
        shadow_color2 = dlg_style.list_shadow;
      }
      else {
        text_color2 = dlg_style.sel_text_color;
        shadow_color2 = dlg_style.sel_text_shadow_color;
      }
      advance = (int)window->list_items[pane][window->list_first_row[pane] + row];
      icon_sprite = (Sprite *)
                 (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                 (char)advances[advance].category * 0xf0 +
                 (char)advances[advance].epoch * 0x3c);
      tmp = icon_sprite->get_width();
      text_x_offset = panel_layout.margin_x * 3 + tmp * 2;
      row_rect.left += text_x_offset;
      if (selected2 == 0) {
        bg_color2 = dlg_style.list_bg;
      }
      else {
        bg_color2 = dlg_style.list_sel_bg;
      }
      window->fill_rect(&row_rect,bg_color2);
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
        tmp = rect_width(&window->list_rects[pane]);
        tmp2 = font2->text_width((const char *)&textbuf.buf);
        text_x_offset = (tmp - tmp2) / 2;
      }
      else {
        icon_step = icon_sprite->get_width();
        icon_step += panel_layout.margin_x * 2;
        icon_x = icon_step;
        if ((window->list_first_row[pane] + row & 1U) == 0) {
          icon_x = panel_layout.margin_x;
        }
        tmp2 = row_top + row_height / 2;
        tmp = icon_sprite->get_height();
        icon_sprite->draw((RECT *)sprite_rect,&parley_wnd_block,list_left + icon_x,tmp2 - tmp / 2);
        text_clear();
        name_label = (label_string(advances[advance].name));
        text_append(name_label);
      }
      if (shadow_color2 != text_color2) {
        gfx_set_text_color_index(shadow_color2);
        window->draw_text(font2,(const char *)&textbuf.buf,list_left + text_x_offset + 2,
                   row_top + text_y_offset + 1,5);
        gfx_set_text_color_index(text_color2);
        window->draw_text(font2,(const char *)&textbuf.buf,list_left + text_x_offset + 1,
                   row_top + text_y_offset,5);
      }
      gfx_set_text_color_index(text_color2);
      window->draw_text(font2,(const char *)&textbuf.buf,list_left + text_x_offset,
                 row_top + text_y_offset,5);
    }
  }
  window->set_clip((RECT *)saved_clip);
  window->update_buttons();
  ((Control *)window->list_hotspots[pane])->show_and_redraw()
  ;
  return;
}

/* 004683f0 auto */
void ParleyWindow::create_unit_list_pane(RECT *rect,int pane,i32 own_side)

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
  SurfaceWindow *window;
  
  window = (SurfaceWindow *)wnd_get_active_child();
  if (window != 0) {
    /* the active window is a SurfaceWindow's Wnd */
    window = (SurfaceWindow *)((u8 *)window - offsetof(SurfaceWindow, window));
  }
  prefs_main_menu_choice_block.parley_list_own_side[pane] = own_side;
  sb_rect.left = rect->left;
  sb_rect.top = rect->top;
  sb_rect.right = rect->right;
  sb_rect.bottom = rect->bottom;
  prefs_main_menu_choice_block.parley_units_by_city = 0;
  build_unit_list(pane,0);
  if (list_counts[pane] < 1) {
    field = (u8 *)&list_first_row[pane];
    field[0] = 0xff;
    field[1] = 0xff;
    field[2] = 0xff;
    field[3] = 0xff;
  }
  else {
    field = (u8 *)&list_first_row[pane];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  field = (u8 *)&list_columns[pane];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  list_rects[pane].left = rect->left;
  list_rects[pane].top = rect->top;
  list_rects[pane].right = rect->right;
  list_rects[pane].bottom = rect->bottom;
  if (list_rows[pane] <
      list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    sb_right_orig = sb_rect.right;
    list_rects[pane].right =
         (int)list_rects[pane].right - scroll_width;
    sb_rect.left = GetSystemMetrics(2);
    sb_rect.left = sb_right_orig - sb_rect.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[pane] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      parent2 = (u8 *)0x0;
    }
    else {
      parent2 = ((u8 *)this + 0x48);
    }
    ((Scrollbar *)list_scrollbars[pane])->create((Wnd *)parent2,
               pane + 0x3fa,&sb_rect,1);
    ((Scrollbar *)list_scrollbars[pane])->set_range(0,
               list_counts[pane] -
               list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_pos(0);
    ((Scrollbar *)list_scrollbars[pane])->set_page_size(
               list_rows[pane] - 1);
    ((Scrollbar *)list_scrollbars[pane])->set_callback(
               (pane == 0 ? parley_unit_list_scroll_left : parley_unit_list_scroll_right));
    ((Scrollbar *)list_scrollbars[pane])->set_track_callback(
               (pane == 0 ? parley_unit_list_scroll_left : parley_unit_list_scroll_right));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[pane] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    parent22 = (u8 *)0x0;
  }
  else {
    parent22 = ((u8 *)this + 0x48);
  }
  ((Hotspot *)list_hotspots[pane])->create((Wnd *)parent22,
             pane + 0x3fc,&list_rects[pane]);
  ((Hotspot *)list_hotspots[pane])->set_click_callback(parley_unit_list_on_click);
  ((Hotspot *)list_hotspots[pane])->set_dblclick_callback(parley_unit_list_on_click);
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
  window->window.activate();
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
  window->window.activate();
  pane = control_id - 0x3fc;
  window->window.get_cursor_pos(&cursor_x,&cursor_y);
  if (((window->list_counts[pane] != 0) &&
      (row = parley_unit_list_hit_test(cursor_x,cursor_y,pane), -1 < row)) &&
     (row < window->list_counts[pane])) {
    key_down = window->window.is_shift_down();
    if (key_down == 0) {
      key_down = window->window.is_ctrl_down();
      if (key_down == 0) {
        for (i = 0; i < window->list_counts[pane];
            i = i + 1) {
          if (i != row) {
            window->list_selected[pane][i] = 0;
          }
        }
        window->list_anchor_row[pane] = row;
        sel_flag = &window->list_selected[pane][row];
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        window->list_anchor_row[pane] = row;
        sel_flag = &window->list_selected[pane][row];
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < window->list_counts[pane]; i++)
      {
        window->list_selected[pane][i] = 0;
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
        window->list_selected[pane][i] = 1;
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
  if (py < (int)window->list_rects[pane].top) {
    result = -1;
  }
  else if (py < (int)window->list_rects[pane].bottom) {
    if (px < (int)window->list_rects[pane].left) {
      result = -3;
    }
    else if (px < (int)window->list_rects[pane].right) {
      result = (int)window->list_rects[pane].top;
      list_height = rect_height(&window->list_rects[pane]);
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
void __cdecl parley_draw_unit_list(int pane)

{
  int stale;
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
  this2->get_clip((RECT *)saved_clip);
  this2->set_clip(&this2->list_rects[pane]);
  for (;;) {
    this2->fill_rect(&this2->list_rects[pane],dlg_style.list_bg);
    if (this2->large_layout == 0) {
      font2 = (GameFont *)((i8 *)&font_size14);
    }
    else {
      font2 = (GameFont *)((i8 *)&font_size21);
    }
    list_left = (int)this2->list_rects[pane].left;
    list_width2 = rect_width(&this2->list_rects[pane]);
    row_height = rect_height(&this2->list_rects[pane]);
    row_height = row_height / this2->list_rows[pane];
    tmp = row_height / 2;
    tmp2 = font2->font_cached_height();
    text_y_offset = tmp - tmp2 / 2;
    has_sel = (u8 *)&this2->side_selected[pane];
    has_sel[0] = '\0';
    has_sel[1] = '\0';
    has_sel[2] = '\0';
    has_sel[3] = '\0';
    for (row = 0; row < this2->list_counts[pane];
        row = row + 1) {
      if (this2->list_selected[pane][row] != 0) {
        has_sel = (u8 *)&this2->side_selected[pane];
        has_sel[0] = 1;
        has_sel[1] = '\0';
        has_sel[2] = '\0';
        has_sel[3] = '\0';
        break;
      }
    }
    stale = 0;
    for (row = 0; row < this2->list_rows[pane]; row++) {
      row_top = (int)this2->list_rects[pane].top + row_height * row;
      SetRect(&row_rect,list_left,row_top,list_width2 + list_left,row_height + row_top);
      if ((row != 0) && (this2->list_first_row[pane] == -1)) {
        break;
      }
      if (this2->list_first_row[pane] + row <
          this2->list_counts[pane]) {
        selected2 = (int)this2->list_selected[pane][this2->list_first_row[pane] + row];
        if (selected2 == 0) {
          text_color2 = dlg_style.list_text;
          shadow_color2 = dlg_style.list_shadow;
        }
        else {
          text_color2 = dlg_style.sel_text_color;
          shadow_color2 = dlg_style.sel_text_shadow_color;
        }
        unit = 0;
        if ((0 < this2->list_counts[pane]) &&
           (unit = this2->find_unit_by_id(
                                  (i32)this2->list_items_b[pane][this2->list_first_row[pane] + row]), unit < 0)) {
          stale = 1;  /* not found: the list is rebuilt and drawn again */
          break;
        }
        unit_sprite = (Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + (uint)units[unit].type * 0x3c);
        if (this2->large_layout == 0) {
          tmp = unit_sprite->get_width();
          icon_width = (tmp * 2) / 3;
        }
        else {
          icon_width = unit_sprite->get_width();
        }
        text_x_offset = (-(uint)(this2->large_layout == 0) & 0xfffffffc) + 10 + icon_width * 2;
        row_rect.left = row_rect.left + text_x_offset - 3;
        if (selected2 == 0) {
          bg_color2 = dlg_style.list_bg;
        }
        else {
          bg_color2 = dlg_style.list_sel_bg;
        }
        this2->fill_rect(&row_rect,bg_color2);
        if (this2->list_first_row[pane] == -1) {
          text_clear();
          str = (iptr)(label_string(label_table.string_ids[0x2df]));
          text_append((const char *)str);
          has_sel = (u8 *)&this2->side_selected[pane];
          has_sel[0] = '\0';
          has_sel[1] = '\0';
          has_sel[2] = '\0';
          has_sel[3] = '\0';
          tmp = rect_width(&this2->list_rects[pane]);
          tmp2 = font2->text_width((const char *)&textbuf.buf);
          text_x_offset = (tmp - tmp2) / 2;
        }
        else {
          denom = 0x30;
          tmp = font2->font_cached_height();
          gfx_set_scale_ratio(tmp * 2,denom);
          tmp = font2->font_cached_height();
          icon_step = (tmp << 7) / 0x30;
          icon_x = icon_step;
          if ((this2->list_first_row[pane] + row & 1U) == 0) {
            icon_x = 0;
          }
          row_mid = row_top + row_height / 2;
          tmp = unit_sprite->get_height();
          tmp2 = font2->font_cached_height();
          unit_sprite->draw((RECT *)sprite_rect,&parley_wnd_block,list_left + icon_x,
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
          this2->draw_text(font2,(const char *)&textbuf.buf,text_x_offset + list_left + 2,
                     text_y_offset + row_top + 1,5);
          gfx_set_text_color_index(text_color2);
          this2->draw_text(font2,(const char *)&textbuf.buf,text_x_offset + list_left + 1,
                     text_y_offset + row_top,5);
        }
        gfx_set_text_color_index(text_color2);
        this2->draw_text(font2,(const char *)&textbuf.buf,text_x_offset + list_left,
                   text_y_offset + row_top,5);
      }
    }
    if (stale == 0) {
      this2->set_clip((RECT *)saved_clip);
      this2->update_buttons();
      ((Control *)this2->list_hotspots[pane])->show_and_redraw();
      return;
    }
    this2->build_unit_list(pane,1);
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
  }
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
  window->window.activate();
  ((Control *)window->info_hide_buttons[0])->show_and_redraw();
  ((Control *)window->info_show_buttons[0])->hide();
  window->local_units_revealed = 1;
  ((Button *)window->sort_by_unit_buttons[0])->enable();
  ((Button *)window->sort_by_city_buttons[0])->enable();
  net_send_message(NM_REVEAL_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)window->other_civ]].id,
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
  window->window.activate();
  ((Control *)window->info_show_buttons[0])->show_and_redraw();
  ((Control *)window->info_hide_buttons[0])->hide();
  window->local_units_revealed = 0;
  ((Button *)window->sort_by_unit_buttons[0])->disable();
  ((Button *)window->sort_by_city_buttons[0])->disable();
  net_send_message(NM_HIDE_CITY_ORIGIN,net.net_slots[net.net_civ_slots[window->other_civ]].id,human_civ[0]
                     ,(int)(char)window->local_units_revealed,0,0,0,0,0,0);
  window->sort_unit_list(0,(u32)window->list_counts[0] - 1,0);
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
  window->window.activate();
  civ_plural = (civ_name_plural(human_civ[0]));
  text_set_string_arg(0,civ_plural);
  net_send_text_args(100,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,1,0,
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
void __cdecl parley_sort_by_city_clicked(int pane)

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
  window->window.activate();
  window->sort_units_by_city(0,window->list_counts[pane] - 1,pane);
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
void __cdecl parley_sort_by_unit_clicked(int pane)

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
  window->window.activate();
  window->sort_unit_list(0,(u32)window->list_counts[pane] - 1,pane);
  parley_draw_unit_list(pane);
  return;
}

/* 0046968b auto */
void ParleyWindow::sort_unit_list(int first,int last,int pane)

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
                                  (uint)units[(int)window->list_items[pane][i]].type * 4),
                        *(char **)(((i8 *)&prefs_main_menu_choice_block.unit_type_name_ptrs) +
                                  (uint)units[(int)window->list_items[pane][j]].type * 4));
        if (0 < cmp) {
          swap = (i32)window->list_items[pane][i];
          window->list_items[pane][i] =
               (i32)window->list_items[pane][j];
          window->list_items[pane][j] = swap;
          swap = (i32)window->list_items_b[pane][i];
          window->list_items_b[pane][i] =
               (i32)window->list_items_b[pane][j];
          window->list_items_b[pane][j] = swap;
          swap = (i32)window->list_selected[pane][i];
          window->list_selected[pane][i] =
               (i32)window->list_selected[pane][j];
          window->list_selected[pane][j] = swap;
        }
      }
    }
  }
  return;
}

/* 0046990a auto */
void ParleyWindow::sort_units_by_city(int first,int last,int pane)

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
  for (i = first; j = i, i < last; i++) {
    for (;;) {
      j++;
      if (last < j) break;
      if (units[(int)window->list_items[pane][i]].home_city != -1) {
        if (units[(int)window->list_items[pane][j]].home_city != -1) {
          _Str2 = (char *)unit_home_city_name(window->list_items[pane][j]);
          _Str1 = (char *)unit_home_city_name(window->list_items[pane][i]);
          cmp = _strcmp(_Str1,_Str2);
          if (cmp < 1) continue;
        }
        swap = (i32)window->list_items[pane][i];
        window->list_items[pane][i] =
             (i32)window->list_items[pane][j];
        window->list_items[pane][j] = swap;
        swap = (i32)window->list_items_b[pane][i];
        window->list_items_b[pane][i] =
             (i32)window->list_items_b[pane][j];
        window->list_items_b[pane][j] = swap;
        swap = (i32)window->list_selected[pane][i];
        window->list_selected[pane][i] =
             (i32)window->list_selected[pane][j];
        window->list_selected[pane][j] = swap;
      }
    }
  }
  return;
}

/* 00469bdc auto */
void ParleyWindow::build_unit_list(int pane,int keep_selection)

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
        if (window->list_selected[pane][unit] == 0) {
          *(i32 *)((int)selected_ids + unit * 4) = 0xffffffff;
        }
        else {
          *(i32 *)((int)selected_ids + unit * 4) =
               (i32)window->list_items_b[pane][unit];
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
        sort_unit_list(0,window->list_counts[pane] - 1,pane);
      }
      else {
        sort_units_by_city(0,window->list_counts[pane] - 1,pane);
      }
      return;
    }
    if ((units[unit].id != 0) &&
       (((prefs_main_menu_choice_block.parley_list_own_side[pane] != 0 && (units[unit].owner == (*(i8 *)&human_civ))) ||
        (((prefs_main_menu_choice_block.parley_list_own_side[pane] == 0 &&
          ((int)units[unit].owner == other_civ)) &&
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
        window->list_items[pane][window->list_counts[pane]] =
             unit;
        window->list_items_b[pane][window->list_counts[pane]] =
             units[unit].id;
        window->list_selected[pane][window->list_counts[pane]] = 0;
        if ((keep_selection == 1) && (selected_ids != (void *)0x0)) {
          for (k = 0; k < old_count; k++) {
            if (units[unit].id == *(int *)((int)selected_ids + k * 4)) {
              window->list_selected[pane][window->list_counts[pane]] =
                   1;
              break;
            }
          }
        }
        count_ptr = &window->list_counts[pane];
        *count_ptr = *count_ptr + 1;
      }
    }
    unit++;
  } while( true );
}

/* 004af3e0 auto */
void ParleyWindow::build_civ_list(RECT *rect,int side)

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
  
  rect2.left = rect->left;
  rect2.top = rect->top;
  rect2.right = rect->right;
  rect2.bottom = rect->bottom;
  if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder == 0)) &&
     (game.reveal_map == '\0')) {
    all_contact = 0;
  }
  else {
    all_contact = 1;
  }
  field = (u8 *)&list_counts[side];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  for (other_civ = 1; (int)other_civ < 8; other_civ++) {
    if (((human_civ[0] != other_civ) && (this->other_civ != other_civ)) &&
       ((all_contact != 0 || ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_CONTACT) != 0))))
    {
      list_items[side][list_counts[side]] = other_civ;
      list_selected[side][list_counts[side]] =
           0;
      list_counts[side] =
           list_counts[side] + 1;
    }
  }
  if (list_counts[side] < 1) {
    field = (u8 *)&list_first_row[side];
    field[0] = 0xff;
    field[1] = 0xff;
    field[2] = 0xff;
    field[3] = 0xff;
  }
  else {
    field = (u8 *)&list_first_row[side];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  field = (u8 *)&list_columns[side];
  field[0] = 2;
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  list_rects[side].left = rect->left;
  list_rects[side].top = rect->top;
  list_rects[side].right = rect->right;
  list_rects[side].bottom = rect->bottom;
  if (list_rows[side] <
      list_counts[side]) {
    scroll_width = GetSystemMetrics(2);
    has_wonder = rect2.right;
    list_rects[side].right =
         (int)list_rects[side].right - scroll_width;
    rect2.left = GetSystemMetrics(2);
    rect2.left = has_wonder - rect2.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[side] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      scroll_parent = (u8 *)0x0;
    }
    else {
      scroll_parent = ((u8 *)this + 0x48);
    }
    ((Scrollbar *)list_scrollbars[side])->create((Wnd *)scroll_parent,
               side + 0x40d,&rect2,1);
    ((Scrollbar *)list_scrollbars[side])->set_range(0,
               list_counts[side] -
               list_rows[side]);
    ((Scrollbar *)list_scrollbars[side])->set_pos(0);
    ((Scrollbar *)list_scrollbars[side])->set_page_size(
               list_rows[side]);
    ((Scrollbar *)list_scrollbars[side])->set_callback(
               (side == 0 ? parley_civ_list_scroll_local : parley_civ_list_scroll_remote));
    ((Scrollbar *)list_scrollbars[side])->set_track_callback(
               (side == 0 ? parley_civ_list_scroll_local : parley_civ_list_scroll_remote));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[side] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    hotspot_parent = (u8 *)0x0;
  }
  else {
    hotspot_parent = ((u8 *)this + 0x48);
  }
  ((Hotspot *)list_hotspots[side])->create((Wnd *)hotspot_parent,
             side + 0x40f,&list_rects[side]);
  ((Hotspot *)list_hotspots[side])->set_click_callback(parley_civ_list_click);
  ((Hotspot *)list_hotspots[side])->set_dblclick_callback(parley_civ_list_click);
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
  wnd2->window.activate();
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
  wnd2->window.activate();
  side = ctrl_id - 0x40f;
  wnd2->window.get_cursor_pos(&cursor_x,&cursor_y);
  if (((wnd2->list_counts[side] != 0) &&
      (row = parley_civ_list_hit_test(cursor_x,cursor_y,side), -1 < row)) &&
     (row < wnd2->list_counts[side])) {
    key_down = wnd2->window.is_shift_down();
    if (key_down == 0) {
      key_down = wnd2->window.is_ctrl_down();
      if (key_down == 0) {
        for (i = 0; i < wnd2->list_counts[side];
            i = i + 1) {
          if (i != row) {
            wnd2->list_selected[side][i] = 0;
          }
        }
        wnd2->list_anchor_row[side] = row;
        sel_flag = &wnd2->list_selected[side][row];
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        wnd2->list_anchor_row[side] = row;
        sel_flag = &wnd2->list_selected[side][row];
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (i = 0; i < wnd2->list_counts[side]; i++)
      {
        wnd2->list_selected[side][i] = 0;
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
        wnd2->list_selected[side][i] = 1;
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
  if (y < (int)wnd2->list_rects[side].top) {
    row = -1;
  }
  else if (y < (int)wnd2->list_rects[side].bottom) {
    if (x < (int)wnd2->list_rects[side].left) {
      row = -3;
    }
    else if (x < (int)wnd2->list_rects[side].right) {
      row = (int)wnd2->list_rects[side].top;
      list_height = rect_height(&wnd2->list_rects[side]);
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
  wnd2->get_clip((RECT *)saved_clip);
  wnd2->set_clip(&wnd2->list_rects[side]);
  wnd2->fill_rect(&wnd2->list_rects[side],dlg_style.list_bg);
  if (wnd2->large_layout == 0) {
    font2 = (GameFont *)((i8 *)&font_size14);
  }
  else {
    font2 = (GameFont *)((i8 *)&font_size21);
  }
  xLeft = (int)wnd2->list_rects[side].left;
  list_width = rect_width(&wnd2->list_rects[side]);
  row_height = rect_height(&wnd2->list_rects[side]);
  row_height = row_height / wnd2->list_rows[side];
  text_dy = row_height / 2;
  row_top = font2->font_cached_height();
  text_dy -= row_top / 2;
  field = (u8 *)&wnd2->side_selected[side];
  field[0] = '\0';
  field[1] = '\0';
  field[2] = '\0';
  field[3] = '\0';
  for (row = 0; row < wnd2->list_counts[side]; row++) {
    if (wnd2->list_selected[side][row] != 0) {
      field = (u8 *)&wnd2->side_selected[side];
      field[0] = 1;
      field[1] = '\0';
      field[2] = '\0';
      field[3] = '\0';
      break;
    }
  }
  for (row = 0; row < wnd2->list_rows[side];
      row = row + 1) {
    row_top = (int)wnd2->list_rects[side].top + row_height * row;
    SetRect(&row_rect,xLeft,row_top,list_width + xLeft,row_height + row_top);
    if (((row == 0) || (wnd2->list_first_row[side] != -1)) &&
       (wnd2->list_first_row[side] + row <
        wnd2->list_counts[side])) {
      selected = (int)wnd2->list_selected[side][wnd2->list_first_row[side] + row];
      if (selected == 0) {
        text_color2 = dlg_style.list_text;
        shadow_color2 = dlg_style.list_shadow;
      }
      else {
        text_color2 = dlg_style.sel_text_color;
        shadow_color2 = dlg_style.sel_text_shadow_color;
      }
      text_dx = (-(uint)(wnd2->large_layout == 0) & 0xffffffe2) + 0x5a + panel_layout.margin_x;
      row_rect.left = row_rect.left + text_dx - 3;
      if (selected == 0) {
        bg_color2 = dlg_style.list_bg;
      }
      else {
        bg_color2 = dlg_style.list_sel_bg;
      }
      wnd2->fill_rect(&row_rect,bg_color2);
      if (wnd2->list_first_row[side] == -1) {
        text_clear();
        text = (label_string(label_table.string_ids[0x2e1]));
        text_append(text);
        field = (u8 *)&wnd2->side_selected[side];
        field[0] = '\0';
        field[1] = '\0';
        field[2] = '\0';
        field[3] = '\0';
      }
      else {
        font_height = font2->font_cached_height();
        parley_draw_civ_capital((i32)wnd2->list_items[side][wnd2->list_first_row[side] + row],
                           wnd2->list_first_row[side] + row,
                           xLeft + 2,row_top,font_height);
        text_clear();
        text = civ_name_plural(wnd2->list_items[side][wnd2->list_first_row[side] + row]);
        text_append(text);
        text_append_space();
        text_append_lparen();
        text = civ_leader_name(wnd2->list_items[side][wnd2->list_first_row[side] + row]);
        text_append(text);
        text_append_rparen();
      }
      if (shadow_color2 != text_color2) {
        gfx_set_text_color_index(shadow_color2);
        wnd2->draw_text(font2,(const char *)&textbuf.buf,text_dx + xLeft + 2,
                   text_dy + row_top + 1,5);
        gfx_set_text_color_index(text_color2);
        wnd2->draw_text(font2,(const char *)&textbuf.buf,text_dx + xLeft + 1,
                   text_dy + row_top,5);
      }
      gfx_set_text_color_index(text_color2);
      wnd2->draw_text(font2,(const char *)&textbuf.buf,text_dx + xLeft,text_dy + row_top,5)
      ;
    }
  }
  wnd2->set_clip((RECT *)saved_clip);
  wnd2->update_buttons();
  ((Control *)wnd2->list_hotspots[side])->show_and_redraw()
  ;
  return;
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
      has_palace = city_has_improvement(city,IMP_PALACE);
      if (has_palace != 0) {
        score += 200;
      }
      if (cities[city].building == -IMP_PALACE) {
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
    gfx_draw_city(&parley_wnd_block,city2,0,draw_x,y - (has_palace - row_height) / 2,zoom2);
  }
  return;
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the description of an
   offer (side kind 0x13; received: 0x14, the offer under review) in a
   read-only edit control filling the pane. */
void ParleyWindow::show_description(int received)
{
  EditControl *new_input;
  uptr desc_edit;
  uptr cur_x;
  int col_width;
  int cur_y;
  int block_height;
  tagRECT ctrl_rect;
  i8 *desc_font;
  u8 *desc_parent;

  build_description(received_offer,received);
  new_input = (EditControl *)(operator_new(0x48));
  if (new_input == (EditControl *)0x0) {
    desc_edit = 0;
  }
  else {
    desc_edit = (uptr)(edit_ctor(new_input));
  }
  this->desc_edit = desc_edit;
  cur_x = panel_left + panel_layout.margin_x;
  col_width = rect_width((RECT *)&panel_left);
  col_width += panel_layout.margin_x * -2;
  cur_y = panel_top + panel_layout.margin_y;
  block_height = divider_y - cur_y;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (large_layout == 0) {
    desc_font = ((i8 *)&font_size14);
  }
  else {
    desc_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((GameFont *)desc_font);
  if (this == (ParleyWindow *)0x0) {
    desc_parent = (u8 *)0x0;
  }
  else {
    desc_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)this->desc_edit)->create_styled((Wnd *)desc_parent,0x426,&ctrl_rect,(const char *)&text_section_index_pool_block.parley_description,0x122
             ,0);
  ((EditControl *)this->desc_edit)->set_max_len(0x2000);
  ((EditControl *)this->desc_edit)->set_callback(parley_edit_escape_filter);
  ((Control *)this->desc_edit)->show_and_redraw();
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the chat pane (side kind
   1): the anonymous, mute and broadcast checkboxes and the send-to checkbox
   per civ. */
void ParleyWindow::add_chat_pane(u32 side_x)
{
  int tmp;
  iptr label;
  int header_x;
  Button *new_ctrl;
  uptr draw_x;
  i32 text_align;
  u8 *send_parent;
  u8 *mute_parent;
  u8 *broadcast_parent;
  u8 *mute_anon_parent;
  u8 *anon_parent;
  int chat_font_height;
  i32 send_checkbox;
  i32 mute_checkbox;
  i8 *right_header_font;
  i8 *left_header_font;
  uptr broadcast_checkbox;
  uptr mute_anon_checkbox;
  uptr anon_checkbox;
  i8 *chat_font;
  uptr left_col_x;
  uint shown_civ_count;
  int civ;
  u32 right_col_x;
  int has_full_intel;
  int checkbox_indent;
  int cur_y;
  uptr cur_x;
  int col_width;
  int block_height;
  
  if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (tmp = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), tmp == 0)
      ) && (game.reveal_map == '\0')) {
    has_full_intel = 0;
  }
  else {
    has_full_intel = 1;
  }
  right_col_x = side_x;
  left_col_x = panel_left + panel_layout.margin_x;
  cur_y = panel_top + panel_layout.margin_y;
  cur_x = left_col_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 3;
  if (large_layout == 0) {
    chat_font_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    chat_font_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 2 + chat_font_height;
  if (large_layout == 0) {
    chat_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    chat_font = ((i8 *)&font_size24_bold);
  }
  ui_set_ctrl_font((GameFont *)chat_font);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    anon_checkbox = 0;
  }
  else {
    anon_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
  }
  anonymous_checkbox = anon_checkbox;
  if (this == (ParleyWindow *)0x0) {
    anon_parent = (u8 *)0x0;
  }
  else {
    anon_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2e9]));
  ((Checkbox *)anonymous_checkbox)->create_fixed((Wnd *)anon_parent,0x415,cur_x,cur_y,col_width,
             (const char *)label);
  ((Checkbox *)anonymous_checkbox)->set_checked((i32)chat_wnd_block.remain_anonymous);
  ((Checkbox *)anonymous_checkbox)->enable();
  ((Checkbox *)anonymous_checkbox)->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    mute_anon_checkbox = 0;
  }
  else {
    mute_anon_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
  }
  mute_anonymous_checkbox = mute_anon_checkbox;
  if (this == (ParleyWindow *)0x0) {
    mute_anon_parent = (u8 *)0x0;
  }
  else {
    mute_anon_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x361]));
  ((Checkbox *)mute_anonymous_checkbox)->create_fixed((Wnd *)mute_anon_parent,0x430,col_width + cur_x,cur_y,
             col_width,(const char *)label);
  ((Checkbox *)mute_anonymous_checkbox)->set_checked((i32)chat_wnd_block.mute_anonymous);
  ((Checkbox *)mute_anonymous_checkbox)->enable();
  ((Checkbox *)mute_anonymous_checkbox)->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    broadcast_checkbox = 0;
  }
  else {
    broadcast_checkbox = (uptr)(checkbox_elem_ctor((Checkbox *)new_ctrl));
  }
  this->broadcast_checkbox = broadcast_checkbox;
  if (this == (ParleyWindow *)0x0) {
    broadcast_parent = (u8 *)0x0;
  }
  else {
    broadcast_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2ea]));
  ((Checkbox *)this->broadcast_checkbox)->create_fixed((Wnd *)broadcast_parent,0x416,col_width * 2 + cur_x,
             cur_y,col_width,(const char *)label);
  ((Checkbox *)this->broadcast_checkbox)->set_checked((i32)chat_wnd_block.broadcast_all);
  ((Checkbox *)this->broadcast_checkbox)->enable();
  ((Checkbox *)this->broadcast_checkbox)->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
  cur_y += block_height;
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    left_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    left_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = left_col_x;
  tmp = cur_y;
  label = (iptr)(label_string(label_table.string_ids[0x34b]));
  draw_text((GameFont *)left_header_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    right_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    right_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  header_x = right_col_x - ((large_layout == 0) - 1 & 0x17);
  tmp = cur_y;
  label = (iptr)(label_string(label_table.string_ids[0x34b]));
  draw_text((GameFont *)right_header_font,(const char *)label,header_x,tmp,text_align);
  cur_x = left_col_x;
  if (large_layout == 0) {
    checkbox_indent = 5;
  }
  else {
    checkbox_indent = 0x17;
  }
  cur_y += block_height;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  shown_civ_count = 0;
  for (civ = 1; civ < 8; civ++) {
    if (((human_civ[0] == civ) ||
        ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) ||
       (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
        (((has_full_intel == 0 && (mp_options.chat_from_start == 0)) &&
         ((*(byte *)&civs[civ].treaties[human_civ[0]] & TREATY_CONTACT) == 0)))))) {
      chat_wnd_block.send_to_civs[civ] = 0;
      chat_wnd_block.muted_civs[civ] = 0;
    }
    else {
      new_ctrl = (Button *)(operator_new(0x3c));
      if (new_ctrl == (Button *)0x0) {
        mute_checkbox = 0;
      }
      else {
        mute_checkbox = (i32)(checkbox_elem_ctor((Checkbox *)new_ctrl));
      }
      mute_checkboxes[civ] = mute_checkbox;
      if (this == (ParleyWindow *)0x0) {
        mute_parent = (u8 *)0x0;
      }
      else {
        mute_parent = ((u8 *)this + 0x48);
      }
      ((Checkbox *)mute_checkboxes[civ])->create_fixed((Wnd *)mute_parent,
                 civ + 0x428,cur_x + checkbox_indent,cur_y,0x23,(const char *)&str_empty_167);
      ((Checkbox *)mute_checkboxes[civ])->set_checked(
                 (i32)chat_wnd_block.muted_civs[civ]);
      ((Checkbox *)mute_checkboxes[civ])->enable();
      ((Checkbox *)mute_checkboxes[civ])->set_callback((CheckboxToggleFn)chat_on_mute_checkbox);
      new_ctrl = (Button *)(operator_new(0x3c));
      if (new_ctrl == (Button *)0x0) {
        send_checkbox = 0;
      }
      else {
        send_checkbox = (i32)(checkbox_elem_ctor((Checkbox *)new_ctrl));
      }
      send_checkboxes[civ] = send_checkbox;
      if (this == (ParleyWindow *)0x0) {
        send_parent = (u8 *)0x0;
      }
      else {
        send_parent = ((u8 *)this + 0x48);
      }
      label = civ_adjective(civ);
      ((Checkbox *)send_checkboxes[civ])->create_fixed((Wnd *)send_parent,
                 civ + 0x416,cur_x + checkbox_indent + 0x23,cur_y,
                 (col_width - 0x23) - checkbox_indent,(const char *)label);
      ((Checkbox *)send_checkboxes[civ])->set_checked(
                 (i32)chat_wnd_block.send_to_civs[civ]);
      ((Checkbox *)send_checkboxes[civ])->enable();
      ((Checkbox *)send_checkboxes[civ])->set_callback((CheckboxToggleFn)chat_on_send_checkbox);
      shown_civ_count++;
      if ((shown_civ_count & 1) == 0) {
        cur_x = left_col_x;
      }
      else {
        cur_x = right_col_x;
      }
      if ((shown_civ_count & 1) == 0) {
        if (large_layout == 0) {
          checkbox_indent = 5;
        }
        else {
          checkbox_indent = 0x17;
        }
      }
      else if (large_layout == 0) {
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
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the pane choosing what to
   do (side kind 2): the mode radio group under its title. */
void ParleyWindow::add_mode_pane(int side)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  uptr title_x;
  i32 text_align;
  u32 selected_mode;
  u8 *mode_radio_parent;
  int mode_title_height;
  i32 mode_radio;
  i8 *mode_radio_font;
  i8 *mode_title_font;
  int max_label_width;
  int label_width;
  iptr label_idx;
  iptr mode_labels[5];
  int cur_y;
  uptr cur_x;
  int col_width;
  u32 side_x;
  tagRECT ctrl_rect;
  int block_height;
  
  mode_labels[0] = (iptr)(label_string(label_table.string_ids[0x2e4]));
  mode_labels[1] = (iptr)(label_string(label_table.string_ids[0x2e5]));
  mode_labels[2] = (iptr)(label_string(label_table.string_ids[0x2e6]));
  mode_labels[3] = (iptr)(label_string(label_table.string_ids[0x2e7]));
  mode_labels[4] = (iptr)(label_string(label_table.string_ids[0x2e8]));
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    label = (iptr)(label_string(label_table.string_ids[0x2e3]));
    max_label_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)label);
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x2e3]));
    max_label_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)label);
  }
  title_x = (panel_left + (int)(panel_right - panel_left) / 2) - max_label_width / 2;
  tmp = panel_top + panel_layout.margin_y;
  if (large_layout == 0) {
    mode_title_font = ((i8 *)&font_size20);
  }
  else {
    mode_title_font = ((i8 *)&font_size30);
  }
  text_align = 5;
  cur_y = tmp;
  cur_x = title_x;
  label = (iptr)(label_string(label_table.string_ids[0x2e3]));
  draw_text((GameFont *)mode_title_font,(const char *)label,title_x,tmp,text_align);
  if (large_layout == 0) {
    max_label_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)mode_labels[0]);
  }
  else {
    max_label_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)mode_labels[0]);
  }
  for (label_idx = 1; label_idx < 5; label_idx++) {
    if (large_layout == 0) {
      label_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)mode_labels[label_idx]);
    }
    else {
      label_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)mode_labels[label_idx]);
    }
    if (max_label_width < label_width) {
      max_label_width = label_width;
    }
  }
  side_x = (panel_left + (int)(panel_right - panel_left) / 2) - max_label_width / 2;
  fill_left = side_x;
  if ((int)cur_x < (int)side_x) {
    side_x = cur_x;
    fill_left = cur_x;
  }
  if (large_layout == 0) {
    mode_radio_font = ((i8 *)&font_size20);
  }
  else {
    mode_radio_font = ((i8 *)&font_size30);
  }
  ctrl_set_pending_font((GameFont *)mode_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    mode_radio = 0;
  }
  else {
    mode_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = mode_radio;
  cur_x = side_x;
  col_width = max_label_width + max_label_width / 2;
  if (large_layout == 0) {
    mode_title_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    mode_title_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  cur_y = panel_top + panel_layout.margin_y * 2 + mode_title_height;
  block_height = divider_y - cur_y;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    mode_radio_parent = (u8 *)0x0;
  }
  else {
    mode_radio_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)mode_radio_parent,0x3f3,
             &ctrl_rect,1,5,(char **)mode_labels);
  if (selected_action == 0xffffffff) {
    selected_mode = 0;
  }
  else {
    selected_mode = selected_action;
  }
  ((RadioGroup *)side_radios[side])->set_selected(selected_mode);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the left side's exchange
   kind radio group (side kind 3). */
void ParleyWindow::add_exchange_kind_pane_left(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  u32 text_x;
  i32 text_align;
  u8 *exch_parent_l;
  int exch_title_height_l;
  i32 exch_radio_l;
  i8 *exch_radio_font_l;
  i8 *exch_title_font_l;
  i32 exch_labels_l[7];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  exch_labels_l[0] = (i32)(label_string(label_table.string_ids[0x2eb]));
  exch_labels_l[1] = (i32)(label_string(label_table.string_ids[0x2ec]));
  exch_labels_l[2] = (i32)(label_string(label_table.string_ids[0x2ed]));
  exch_labels_l[3] = (i32)(label_string(label_table.string_ids[750]));
  exch_labels_l[4] = (i32)(label_string(label_table.string_ids[0x2ef]));
  exch_labels_l[5] = (i32)(label_string(label_table.string_ids[0x2f0]));
  exch_labels_l[6] = (i32)(label_string(label_table.string_ids[0x342]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = panel_top + panel_layout.margin_y;
  if (large_layout == 0) {
    exch_title_font_l = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_title_font_l = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  text_x = side_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x303]));
  draw_text((GameFont *)exch_title_font_l,(const char *)label,text_x,tmp,text_align);
  if (large_layout == 0) {
    exch_radio_font_l = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_radio_font_l = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((GameFont *)exch_radio_font_l);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    exch_radio_l = 0;
  }
  else {
    exch_radio_l = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = exch_radio_l;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    exch_title_height_l = ((GameFont *)((i8 *)&font_size16))->font_cached_height();
  }
  else {
    exch_title_height_l = ((GameFont *)((i8 *)&font_size24))->font_cached_height();
  }
  cur_y = panel_top + panel_layout.margin_y + exch_title_height_l;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  if (this == (ParleyWindow *)0x0) {
    exch_parent_l = (u8 *)0x0;
  }
  else {
    exch_parent_l = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)exch_parent_l,0x3f4,
             &ctrl_rect,1,7,(char **)exch_labels_l);
  if (exchange_kind[1] == 0) {
    ((RadioGroup *)side_radios[side])->disable_item(0);
  }
  else if (exchange_kind[1] == 6) {
    ((RadioGroup *)side_radios[side])->disable_item(6);
  }
  ((RadioGroup *)side_radios[side])->set_selected(exchange_kind[0]);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_exchange_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the right side's exchange
   kind radio group (side kind 4). */
void ParleyWindow::add_exchange_kind_pane_right(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  u32 text_x;
  i32 text_align;
  u8 *exch_parent_r;
  int exch_title_height_r;
  i32 exch_radio_r;
  i8 *exch_radio_font_r;
  i8 *exch_title_font_r;
  i32 exch_labels_r[7];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  exch_labels_r[0] = (i32)(label_string(label_table.string_ids[0x2eb]));
  exch_labels_r[1] = (i32)(label_string(label_table.string_ids[0x2ec]));
  exch_labels_r[2] = (i32)(label_string(label_table.string_ids[0x2ed]));
  exch_labels_r[3] = (i32)(label_string(label_table.string_ids[750]));
  exch_labels_r[4] = (i32)(label_string(label_table.string_ids[0x2ef]));
  exch_labels_r[5] = (i32)(label_string(label_table.string_ids[0x2f1]));
  exch_labels_r[6] = (i32)(label_string(label_table.string_ids[0x342]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = panel_top + panel_layout.margin_y;
  if (large_layout == 0) {
    exch_title_font_r = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_title_font_r = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  text_x = side_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x304]));
  draw_text((GameFont *)exch_title_font_r,(const char *)label,text_x,tmp,text_align);
  if (large_layout == 0) {
    exch_radio_font_r = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    exch_radio_font_r = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((GameFont *)exch_radio_font_r);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    exch_radio_r = 0;
  }
  else {
    exch_radio_r = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = exch_radio_r;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  cur_x = side_x;
  if (large_layout == 0) {
    exch_title_height_r = ((GameFont *)((i8 *)&font_size16))->font_cached_height();
  }
  else {
    exch_title_height_r = ((GameFont *)((i8 *)&font_size24))->font_cached_height();
  }
  cur_y = panel_top + panel_layout.margin_y + exch_title_height_r;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  if (this == (ParleyWindow *)0x0) {
    exch_parent_r = (u8 *)0x0;
  }
  else {
    exch_parent_r = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)exch_parent_r,0x3f5,
             &ctrl_rect,1,7,(char **)exch_labels_r);
  if (exchange_kind[0] == 0) {
    ((RadioGroup *)side_radios[side])->disable_item(0);
  }
  else if (exchange_kind[0] == 6) {
    ((RadioGroup *)side_radios[side])->disable_item(6);
  }
  ((RadioGroup *)side_radios[side])->set_selected(exchange_kind[1]);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_exchange_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the treaty radio group
   (side kind 5). */
void ParleyWindow::add_treaty_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  u8 *treaty_parent;
  int treaty_row_height;
  int treaty_row_height_alt;
  int treaty_title_height;
  iptr treaty_title;
  int treaty_line_height;
  int treaty_line_height_alt;
  int treaty_side_height;
  iptr treaty_side_title;
  i32 treaty_radio;
  i8 *treaty_radio_font;
  i8 *treaty_title_font;
  i8 *treaty_side_font;
  i32 treaty_labels[4];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  treaty_labels[0] = (i32)(label_string(label_table.string_ids[0x2f3]));
  treaty_labels[1] = (i32)(label_string(label_table.string_ids[0x2f4]));
  treaty_labels[2] = (i32)(label_string(label_table.string_ids[0x2f5]));
  treaty_labels[3] = (i32)(label_string(label_table.string_ids[0x2f6]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if ((is_exchange_side != '\0') || (is_demand_side != '\0')) {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      treaty_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      treaty_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)treaty_side_title);
    if (large_layout == 0) {
      treaty_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      treaty_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)treaty_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      treaty_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + treaty_side_height;
  }
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    if (large_layout == 0) {
      treaty_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = treaty_line_height * 5 + panel_layout.margin_y * 4;
  }
  else {
    if (large_layout == 0) {
      treaty_line_height_alt = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_line_height_alt = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = panel_layout.margin_y * 3 + treaty_line_height_alt * 4;
  }
  cur_y = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (is_demand_side == '\0' && is_exchange_side == '\0') {
    treaty_title = (iptr)(label_string(label_table.string_ids[0x2e4]));
  }
  else {
    treaty_title = (iptr)(label_string(label_table.string_ids[0x342]));
  }
  if (large_layout == 0) {
    treaty_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    treaty_title_font = ((i8 *)&font_size24_bold);
  }
  draw_text((GameFont *)treaty_title_font,(const char *)treaty_title,cur_x,cur_y,5)
  ;
  if (large_layout == 0) {
    treaty_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    treaty_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((GameFont *)treaty_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    treaty_radio = 0;
  }
  else {
    treaty_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = treaty_radio;
  cur_x = side_x;
  if (large_layout == 0) {
    treaty_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    treaty_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + treaty_title_height;
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    if (large_layout == 0) {
      treaty_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = panel_layout.margin_y * 3 + treaty_row_height * 4;
  }
  else {
    if (large_layout == 0) {
      treaty_row_height_alt = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      treaty_row_height_alt = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    block_height = treaty_row_height_alt * 3 + panel_layout.margin_y * 2;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    treaty_parent = (u8 *)0x0;
  }
  else {
    treaty_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)treaty_parent,0x3f6,
             &ctrl_rect,1,(is_demand_side == '\0' && is_exchange_side == '\0') + 3,(char **)treaty_labels);
  if (((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_CEASEFIRE) != 0) &&
     (((RadioGroup *)side_radios[side])->disable_item(0),
     treaty_kind == 0)) {
    treaty_kind++;
  }
  if (((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_PEACE) != 0) &&
     (((RadioGroup *)side_radios[side])->disable_item(1),
     treaty_kind == 1)) {
    treaty_kind++;
  }
  if (((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_ALLIANCE) != 0) &&
     (((RadioGroup *)side_radios[side])->disable_item(2),
     treaty_kind == 2)) {
    if (is_demand_side == '\0' && is_exchange_side == '\0') {
      treaty_kind = 3;
    }
    else {
      treaty_kind = 0;
    }
  }
  if ((((is_exchange_side == '\0') && (is_demand_side == '\0')) &&
      ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & (TREATY_CEASEFIRE | TREATY_PEACE | TREATY_ALLIANCE)) == 0)) &&
     (((RadioGroup *)side_radios[side])->disable_item(3),
     treaty_kind == 3)) {
    treaty_kind = 0;
  }
  ((RadioGroup *)side_radios[side])->set_selected(treaty_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_treaty_type_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the framed status box
   (side kind 6). */
void ParleyWindow::add_status_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  iptr label;
  uptr draw_x;
  i32 text_align;
  int status_box_offset;
  int status_header_height;
  int status_title_height;
  int status_line_height;
  i8 *status_text_font;
  i8 *status_header_font;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 - panel_layout.margin_x;
  if (large_layout == 0) {
    status_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  if (large_layout == 0) {
    status_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 2 + status_line_height * 4 + status_title_height;
  cur_y = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target(&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    status_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + panel_layout.margin_y + status_header_height / 2;
  if (large_layout == 0) {
    status_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    status_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 0;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x2f7]));
  draw_text((GameFont *)status_header_font,(const char *)label,draw_x,tmp,text_align);
  ctrl_rect.bottom += panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  if (large_layout == 0) {
    status_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    status_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + status_box_offset;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  fill_rect(&ctrl_rect,dlg_style.list_bg);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  tmp = rect_height(&ctrl_rect);
  cur_y = ctrl_rect.top + tmp / 2;
  text_clear();
  if ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_ALLIANCE) == 0) {
    if ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_PEACE) == 0) {
      if ((*(byte *)&civs[human_civ[0]].treaties[other_civ] & TREATY_CEASEFIRE) == 0) {
        if ((*(byte *)(human_civ[0] * 0x594 + (iptr)((u8 *)&civs + 33) + other_civ * 4) & 0x20) == 0) {
          label = (iptr)(label_string(label_table.string_ids[0x2fb]));
          text_append((const char *)label);
        }
        else {
          label = (iptr)(label_string(label_table.string_ids[0x2fa]));
          text_append((const char *)label);
        }
      }
      else {
        label = (iptr)(label_string(label_table.string_ids[0x2f3]));
        text_append((const char *)label);
      }
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x2f9]));
      text_append((const char *)label);
    }
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x2f8]));
    text_append((const char *)label);
  }
  if (large_layout == 0) {
    status_text_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    status_text_font = ((i8 *)&font_size24_bold);
  }
  draw_text((GameFont *)status_text_font,(const char *)&textbuf.buf,cur_x,cur_y,0);
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the gift kind radio group
   (side kind 7). */
void ParleyWindow::add_gift_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  uptr draw_x;
  i32 text_align;
  u8 *gift_parent;
  int gift_row_height;
  int gift_header_height;
  int gift_title_height;
  int gift_line_height;
  i32 gift_radio;
  i8 *gift_radio_font;
  i8 *gift_title_font;
  i32 gift_labels[5];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gift_labels[0] = (i32)(label_string(label_table.string_ids[0x2fd]));
  gift_labels[1] = (i32)(label_string(label_table.string_ids[0x2fe]));
  gift_labels[2] = (i32)(label_string(label_table.string_ids[0x2ff]));
  gift_labels[3] = (i32)(label_string(label_table.string_ids[0x300]));
  gift_labels[4] = (i32)(label_string(label_table.string_ids[0x301]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  if (large_layout == 0) {
    gift_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  if (large_layout == 0) {
    gift_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 5 + gift_line_height * 5 + gift_title_height;
  tmp = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (large_layout == 0) {
    gift_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gift_title_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x2fc]));
  draw_text((GameFont *)gift_title_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    gift_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gift_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((GameFont *)gift_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    gift_radio = 0;
  }
  else {
    gift_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = gift_radio;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    gift_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + gift_header_height;
  if (large_layout == 0) {
    gift_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gift_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = gift_row_height * 5 + panel_layout.margin_y * 4;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    gift_parent = (u8 *)0x0;
  }
  else {
    gift_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)gift_parent,0x3f7,
             &ctrl_rect,1,5,(char **)gift_labels);
  ((RadioGroup *)side_radios[side])->set_selected(gift_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback((RadioNotifyFn)parley_on_gift_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the advance list (side
   kind 8). */
void ParleyWindow::add_tech_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  iptr label;
  uptr draw_x;
  i32 text_align;
  int tech_header_height;
  int tech_side_height;
  iptr tech_side_title;
  i8 *tech_header_font;
  i8 *tech_side_font;
  i32 tech_list_pick_mode;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  if (is_exchange_side == '\0') {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 7;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  else {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 6;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      tech_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      tech_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)tech_side_title);
    if (large_layout == 0) {
      tech_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      tech_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)tech_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      tech_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      tech_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + tech_side_height;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  gfx_set_draw_target(&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    tech_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    tech_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + tech_header_height / 2;
  if (large_layout == 0) {
    tech_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    tech_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 0;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x302]));
  draw_text((GameFont *)tech_header_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    tmp = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  else {
    tmp = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  ctrl_rect.bottom = divider_y + panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  fill_rect(&ctrl_rect,dlg_style.list_bg);
  tech_list_pick_mode = 0;
  if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
    tech_list_pick_mode = 1;
  }
  create_advance_list_pane(&ctrl_rect,side,tech_list_pick_mode);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the gold amount edit
   (side kind 9). */
void ParleyWindow::add_gold_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  int tmp;
  EditControl *new_input;
  iptr label;
  u8 *gold_edit_parent;
  int gold_prompt_width;
  int gold_edit_text_width;
  int gold_box_offset;
  int gold_header_height;
  iptr gold_side_title;
  int gold_title_height;
  int gold_line_height;
  i8 *gold_edit_font;
  i32 gold_edit;
  i8 *gold_prompt_font;
  i8 *gold_header_font;
  i8 *gold_side_font;
  char gold_text[32];
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gfx_set_text_color_index(10);
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (large_layout == 0) {
    gold_line_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    gold_line_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  if (large_layout == 0) {
    gold_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gold_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 2 + gold_line_height * 4 + gold_title_height;
  cur_y = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      gold_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      gold_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)gold_side_title);
    if (large_layout == 0) {
      gold_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      gold_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)gold_side_font,(const char *)&textbuf.buf,cur_x,
               panel_top + panel_layout.margin_y,5);
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target(&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    gold_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gold_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + gold_header_height / 2;
  str_clear((char *)&textbuf.buf);
  if (is_demand_side == '\0') {
    label = (iptr)(label_string(label_table.string_ids[0x306]));
    text_append_str((const char *)label);
    text_append_space();
    text_append_int(civs[human_civ[0]].gold);
    text_append_space();
    label = (iptr)(label_string(label_table.string_ids[0x307]));
    text_append_str((const char *)label);
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x305]));
    text_append_str((const char *)label);
  }
  if (large_layout == 0) {
    gold_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gold_header_font = ((i8 *)&font_size24_bold);
  }
  draw_text((GameFont *)gold_header_font,(const char *)&textbuf.buf,cur_x,cur_y,0);
  ctrl_rect.bottom += panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  if (large_layout == 0) {
    gold_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    gold_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + gold_box_offset;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  fill_rect(&ctrl_rect,dlg_style.list_bg);
  str_clear((char *)&textbuf.buf);
  text_append_str((const char *)&str_000000);
  if (large_layout == 0) {
    gold_edit_text_width = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->text_width((const char *)&textbuf.buf);
  }
  else {
    gold_edit_text_width = ((GameFont *)((i8 *)&font_size24_bold))->text_width((const char *)&textbuf.buf);
  }
  col_width = (gold_edit_text_width * 3) / 2;
  str_clear((char *)&textbuf.buf);
  label = (iptr)(label_string(label_table.string_ids[0x308]));
  text_append_str((const char *)label);
  text_append_space();
  if (large_layout == 0) {
    gold_prompt_width = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->text_width((const char *)&textbuf.buf);
  }
  else {
    gold_prompt_width = ((GameFont *)((i8 *)&font_size24_bold))->text_width((const char *)&textbuf.buf);
  }
  tmp = rect_width(&ctrl_rect);
  cur_x = (ctrl_rect.left + tmp / 2) - (col_width + gold_prompt_width) / 2;
  tmp = rect_height(&ctrl_rect);
  cur_y = ctrl_rect.top + tmp / 2;
  if (large_layout == 0) {
    gold_prompt_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gold_prompt_font = ((i8 *)&font_size24_bold);
  }
  draw_text((GameFont *)gold_prompt_font,(const char *)&textbuf.buf,cur_x,cur_y,1);
  new_input = (EditControl *)(operator_new(0x48));
  if (new_input == (EditControl *)0x0) {
    gold_edit = 0;
  }
  else {
    gold_edit = (i32)(edit_ctor(new_input));
  }
  gold_edits[side] = gold_edit;
  if (large_layout == 0) {
    gold_edit_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    gold_edit_font = ((i8 *)&font_size24_bold);
  }
  ui_set_default_font((GameFont *)gold_edit_font);
  if (large_layout == 0) {
    tmp = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->text_width((const char *)&textbuf.buf);
    cur_x += tmp;
  }
  else {
    tmp = ((GameFont *)((i8 *)&font_size24_bold))->text_width((const char *)&textbuf.buf);
    cur_x += tmp;
  }
  if (large_layout == 0) {
    block_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    block_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y -= block_height / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  __itoa((int)gold_amount[side],gold_text,10);
  if (this == (ParleyWindow *)0x0) {
    gold_edit_parent = (u8 *)0x0;
  }
  else {
    gold_edit_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)gold_edits[side])->create((Wnd *)gold_edit_parent,
             side + 0x3f8,&ctrl_rect,(const char *)gold_text);
  ((EditControl *)gold_edits[side])->set_max_len(6);
  ((EditControl *)gold_edits[side])->set_change_callback(parley_gold_edit_on_change);
  ((Control *)gold_edits[side])->set_focus();
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the unit list with its
   sort, hide and reveal buttons (side kind 10). */
void ParleyWindow::add_unit_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  iptr label;
  Button *new_ctrl;
  uptr draw_x;
  i32 text_align;
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
  i32 sort_city_button;
  i32 sort_unit_button;
  i32 hide_button;
  i32 reveal_button;
  i8 *unit_header_font;
  i8 *unit_side_font;
  tagRECT btn_rect;
  int btn_area_bottom;
  int btn_width;
  int btn_top;
  int btn_height;
  i32 unit_list_mode;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  if (is_exchange_side == '\0') {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 6;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  else {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 5;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      unit_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      unit_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)unit_side_title);
    if (large_layout == 0) {
      unit_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      unit_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)unit_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      unit_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      unit_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + unit_side_height;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  gfx_set_draw_target(&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    unit_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    unit_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + unit_header_height / 2;
  cur_y = tmp;
  if (is_exchange_side == '\0') {
    if (large_layout == 0) {
      unit_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      unit_header_font = ((i8 *)&font_size24_bold);
    }
    text_align = 0;
    draw_x = cur_x;
    label = (iptr)(label_string(label_table.string_ids[0x309]));
    draw_text((GameFont *)unit_header_font,(const char *)label,draw_x,tmp,text_align);
  }
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    if (large_layout == 0) {
      unit_btn_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      unit_btn_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    btn_top = ctrl_rect.top + unit_btn_offset;
    btn_width = col_width / 2;
    if (large_layout == 0) {
      unit_line_spacing = ((GameFont *)((i8 *)&font_size16))->line_spacing();
    }
    else {
      unit_line_spacing = ((GameFont *)((i8 *)&font_size24))->line_spacing();
    }
    btn_height = panel_layout.padding_y * 2 + unit_line_spacing;
    SetRect(&btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
            btn_top + btn_height);
  }
  else {
    btn_top = panel_layout.margin_y * 2 + ctrl_rect.top - 1;
    if (large_layout == 0) {
      unit_btn_offset_alt = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      unit_btn_offset_alt = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    if (large_layout == 0) {
      unit_line_spacing_alt = ((GameFont *)((i8 *)&font_size16))->line_spacing();
    }
    else {
      unit_line_spacing_alt = ((GameFont *)((i8 *)&font_size24))->line_spacing();
    }
    btn_area_bottom = ctrl_rect.top + panel_layout.padding_y * 2 + unit_btn_offset_alt + unit_line_spacing_alt;
    btn_width = col_width + panel_layout.margin_x * -3;
    btn_height = (btn_area_bottom - btn_top) / 2;
    SetRect(&btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
            btn_top + btn_height);
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      reveal_button = 0;
    }
    else {
      reveal_button = (i32)(button_ctor(new_ctrl));
    }
    info_show_buttons[side] = reveal_button;
    if (side == 0) {
      reveal_label = (iptr)(label_string(label_table.string_ids[0x36f]));
    }
    else {
      reveal_label = (iptr)(label_string(label_table.string_ids[0x370]));
    }
    if (this == (ParleyWindow *)0x0) {
      reveal_parent = (u8 *)0x0;
    }
    else {
      reveal_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_show_buttons[side])->create((Wnd *)reveal_parent,
               side + 0x404,&btn_rect,(const char *)reveal_label);
    ((Button *)info_show_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_reveal_city_origin_clicked : parley_request_city_origin_clicked));
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      hide_button = 0;
    }
    else {
      hide_button = (i32)(button_ctor(new_ctrl));
    }
    info_hide_buttons[side] = hide_button;
    if (side == 0) {
      hide_label = (iptr)(label_string(label_table.string_ids[0x371]));
    }
    else {
      hide_label = (iptr)(label_string(label_table.string_ids[0x370]));
    }
    if (this == (ParleyWindow *)0x0) {
      hide_parent = (u8 *)0x0;
    }
    else {
      hide_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_hide_buttons[side])->create((Wnd *)hide_parent,
               side + 0x406,&btn_rect,(const char *)hide_label);
    ((Button *)info_hide_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_hide_city_origin_clicked : parley_request_city_origin_clicked));
    if ((side == 0) && (local_units_revealed != '\0')) {
      ((Control *)info_show_buttons[0])->hide();
      ((Control *)info_hide_buttons[0])->show_and_redraw();
    }
    else if (side == 0) {
      ((Control *)info_show_buttons[0])->show_and_redraw();
      ((Control *)info_hide_buttons[0])->hide();
    }
    if ((side == 1) && (remote_units_revealed != '\0')) {
      ((Control *)info_show_buttons[1])->hide();
      ((Control *)info_hide_buttons[1])->show_and_redraw();
      ((Button *)info_hide_buttons[1])->disable();
    }
    else if (side == 1) {
      ((Control *)info_show_buttons[1])->show_and_redraw();
      ((Control *)info_hide_buttons[1])->hide();
    }
    btn_top += btn_height;
    btn_width = col_width / 2;
    SetRect(&btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,btn_top,ctrl_rect.left + btn_width,
            btn_top + btn_height);
  }
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    sort_unit_button = 0;
  }
  else {
    sort_unit_button = (i32)(button_ctor(new_ctrl));
  }
  sort_by_unit_buttons[side] = sort_unit_button;
  if (this == (ParleyWindow *)0x0) {
    sort_unit_parent = (u8 *)0x0;
  }
  else {
    sort_unit_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x30a]));
  ((Button *)sort_by_unit_buttons[side])->create((Wnd *)sort_unit_parent,
             side + 0x400,&btn_rect,(const char *)label);
  ((Button *)sort_by_unit_buttons[side])->set_callback(
             (CtrlNotifyFn)(side == 0 ? parley_sort_by_unit_left_clicked : parley_sort_by_unit_right_clicked));
  if (((int)parley_wnd_block.mode != 9) &&
     (((side == 0 && (local_units_revealed == '\0')) ||
      ((side == 1 && (remote_units_revealed == '\0')))))) {
    ((Button *)sort_by_unit_buttons[side])->disable();
  }
  SetRect(&btn_rect,btn_rect.right,btn_top,btn_rect.right + btn_width + panel_layout.margin_x * -3,
          btn_top + btn_height);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    sort_city_button = 0;
  }
  else {
    sort_city_button = (i32)(button_ctor(new_ctrl));
  }
  sort_by_city_buttons[side] = sort_city_button;
  if (this == (ParleyWindow *)0x0) {
    sort_city_parent = (u8 *)0x0;
  }
  else {
    sort_city_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x30b]));
  ((Button *)sort_by_city_buttons[side])->create((Wnd *)sort_city_parent,
             side + 0x402,&btn_rect,(const char *)label);
  ((Button *)sort_by_city_buttons[side])->set_callback(
             (CtrlNotifyFn)(side == 0 ? parley_sort_by_city_left_clicked : parley_sort_by_city_right_clicked));
  if (((int)parley_wnd_block.mode != 9) &&
     (((side == 0 && (local_units_revealed == '\0')) ||
      ((side == 1 && (remote_units_revealed == '\0')))))) {
    ((Button *)sort_by_city_buttons[side])->disable();
  }
  ctrl_rect.top = btn_rect.bottom;
  ctrl_rect.bottom = divider_y + panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  fill_rect(&ctrl_rect,dlg_style.list_bg);
  unit_list_mode = 1;
  if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
    unit_list_mode = 0;
  }
  create_unit_list_pane(&ctrl_rect,side,unit_list_mode);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the proposal radio group
   (side kind 0xb). */
void ParleyWindow::add_proposal_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  u32 text_x;
  i32 text_align;
  u8 *proposal_parent;
  int proposal_row_height;
  int proposal_title_height;
  int proposal_line_height;
  i32 proposal_radio;
  i8 *proposal_radio_font;
  i8 *proposal_title_font;
  i32 proposal_labels[2];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    proposal_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    proposal_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = proposal_line_height * 3 + panel_layout.margin_y * 2;
  tmp = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  cur_x = side_x;
  if (large_layout == 0) {
    proposal_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    proposal_title_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  text_x = side_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x30c]));
  draw_text((GameFont *)proposal_title_font,(const char *)label,text_x,tmp,text_align);
  proposal_labels[0] = (i32)(label_string(label_table.string_ids[0x30d]));
  proposal_labels[1] = (i32)(label_string(label_table.string_ids[0x30e]));
  if (large_layout == 0) {
    proposal_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    proposal_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((GameFont *)proposal_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    proposal_radio = 0;
  }
  else {
    proposal_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = proposal_radio;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    proposal_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    proposal_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + proposal_title_height;
  if (large_layout == 0) {
    proposal_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    proposal_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = proposal_row_height * 2 + panel_layout.margin_y;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    proposal_parent = (u8 *)0x0;
  }
  else {
    proposal_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)proposal_parent,0x40c,
             &ctrl_rect,1,2,(char **)proposal_labels);
  ((RadioGroup *)side_radios[side])->set_selected(proposal_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback((RadioNotifyFn)parley_on_proposal_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the civ list (side kind
   0xc). */
void ParleyWindow::add_civ_list_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  int civlist_item_height;
  int civlist_box_offset;
  int civlist_header_height;
  int civlist_title_height;
  int civlist_row_height;
  int civlist_side_height;
  iptr civlist_side_title;
  i8 *civlist_header_font;
  i8 *civlist_side_font;
  iptr civlist_title_idx;
  iptr civlist_titles[4];
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  civlist_titles[0] = (iptr)(label_string(label_table.string_ids[0x314]));
  civlist_titles[1] = (iptr)(label_string(label_table.string_ids[0x315]));
  civlist_titles[2] = (iptr)(label_string(label_table.string_ids[0x316]));
  civlist_titles[3] = (iptr)(label_string(label_table.string_ids[0x343]));
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
  flag_ptr = (u8 *)&list_rows[side];
  flag_ptr[0] = 5;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      civlist_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      civlist_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)civlist_side_title);
    if (large_layout == 0) {
      civlist_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      civlist_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)civlist_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      civlist_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      civlist_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + civlist_side_height;
  }
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (large_layout == 0) {
    civlist_row_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    civlist_row_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  if (large_layout == 0) {
    civlist_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    civlist_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = list_rows[side] * civlist_row_height +
             list_rows[side] * panel_layout.margin_y + panel_layout.margin_y * 4 +
             civlist_title_height;
  cur_y += (int)((divider_y - cur_y) - block_height) / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target(&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    civlist_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    civlist_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + civlist_header_height / 2;
  if (large_layout == 0) {
    civlist_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    civlist_header_font = ((i8 *)&font_size24_bold);
  }
  draw_text((GameFont *)civlist_header_font,(const char *)civlist_titles[civlist_title_idx],cur_x,cur_y,0);
  if (large_layout == 0) {
    civlist_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    civlist_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + civlist_box_offset;
  if (large_layout == 0) {
    civlist_item_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    civlist_item_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  ctrl_rect.bottom =
       list_rows[side] * civlist_item_height +
       list_rows[side] * panel_layout.margin_y + ctrl_rect.top + 2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  fill_rect(&ctrl_rect,dlg_style.list_bg);
  build_civ_list(&ctrl_rect,side);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the map radio group (side
   kind 0xd). */
void ParleyWindow::add_map_pane(int side,u32 side_x,char is_exchange_side)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  u8 *map_parent;
  int map_box_offset;
  int map_header_height;
  int map_title_height;
  int map_row_height;
  int map_side_height;
  iptr map_side_title;
  i32 map_radio;
  i8 *map_radio_font;
  i8 *map_header_font;
  i8 *map_side_font;
  uptr map_title_idx;
  iptr map_labels[5];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  map_labels[0] = (iptr)(label_string(label_table.string_ids[0x30f]));
  map_labels[1] = (iptr)(label_string(label_table.string_ids[0x310]));
  map_title_idx = 0;
  map_labels[2] = (iptr)(label_string(label_table.string_ids[0x311]));
  map_labels[3] = (iptr)(label_string(label_table.string_ids[0x312]));
  map_labels[4] = (iptr)(label_string(label_table.string_ids[0x313]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  if (is_exchange_side != '\0') {
    map_title_idx = (uptr)(side != 0);
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      map_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      map_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)map_side_title);
    if (large_layout == 0) {
      map_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      map_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)map_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      map_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      map_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + map_side_height;
  }
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (large_layout == 0) {
    map_row_height = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
  }
  else {
    map_row_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  }
  if (large_layout == 0) {
    map_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    map_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = map_row_height * 3 + panel_layout.margin_y * 9 + map_title_height;
  cur_y += (int)((divider_y - cur_y) - block_height) / 2;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  gfx_set_draw_target(&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    map_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    map_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + map_header_height / 2;
  if (large_layout == 0) {
    map_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    map_header_font = ((i8 *)&font_size24_bold);
  }
  draw_text((GameFont *)map_header_font,(const char *)map_labels[map_title_idx],cur_x,cur_y,0);
  if (large_layout == 0) {
    map_box_offset = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    map_box_offset = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  ctrl_rect.top = ctrl_rect.top + panel_layout.margin_y + map_box_offset;
  ctrl_rect.bottom += panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  fill_rect(&ctrl_rect,dlg_style.list_bg);
  if (large_layout == 0) {
    map_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    map_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((GameFont *)map_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    map_radio = 0;
  }
  else {
    map_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = map_radio;
  if (this == (ParleyWindow *)0x0) {
    map_parent = (u8 *)0x0;
  }
  else {
    map_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)map_parent,side + 0x424,
             &ctrl_rect,1,3,((char **)(map_labels + 2)));
  ((RadioGroup *)side_radios[side])->set_selected(
             (i32)map_choice[side]);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback(parley_on_map_choice_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the offer radio group
   (side kind 0xf). */
void ParleyWindow::add_offer_pane(int side,u32 side_x)
{
  u8 *flag_ptr;
  int tmp;
  EditControl *new_input;
  iptr label;
  uptr draw_x;
  i32 text_align;
  u8 *offer_parent;
  int offer_row_height;
  int offer_header_height;
  int offer_title_height;
  int offer_line_height;
  i32 offer_radio;
  i8 *offer_radio_font;
  i8 *offer_header_font;
  i8 *offer_title_font;
  i32 offer_labels[7];   /* (the original's locals in a row: the radio group's labels) */
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  int block_height;
  
  offer_labels[0] = (i32)(label_string(label_table.string_ids[0x2eb]));
  offer_labels[1] = (i32)(label_string(label_table.string_ids[0x2ec]));
  offer_labels[2] = (i32)(label_string(label_table.string_ids[0x2ed]));
  offer_labels[3] = (i32)(label_string(label_table.string_ids[750]));
  offer_labels[4] = (i32)(label_string(label_table.string_ids[0x2ef]));
  offer_labels[5] = (i32)(label_string(label_table.string_ids[0x2f1]));
  offer_labels[6] = (i32)(label_string(label_table.string_ids[0x342]));
  gfx_set_text_color_index(10);
  cur_x = side_x;
  if (large_layout == 0) {
    offer_line_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_line_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 7 + offer_line_height * 8;
  tmp = panel_top + (int)((divider_y - panel_top) - block_height) / 2;
  if (large_layout == 0) {
    offer_title_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    offer_title_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x317]));
  draw_text((GameFont *)offer_title_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    offer_title_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  tmp = cur_y + panel_layout.margin_y * 2 + offer_title_height;
  if (large_layout == 0) {
    offer_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    offer_header_font = ((i8 *)&font_size24_bold);
  }
  text_align = 5;
  draw_x = cur_x;
  cur_y = tmp;
  label = (iptr)(label_string(label_table.string_ids[0x318]));
  draw_text((GameFont *)offer_header_font,(const char *)label,draw_x,tmp,text_align);
  if (large_layout == 0) {
    offer_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y = cur_y + panel_layout.margin_y + offer_header_height;
  if (large_layout == 0) {
    offer_radio_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
  }
  else {
    offer_radio_font = ((i8 *)&font_size24_bold);
  }
  ctrl_set_pending_font((GameFont *)offer_radio_font);
  new_input = (EditControl *)(operator_new(0x50));
  if (new_input == (EditControl *)0x0) {
    offer_radio = 0;
  }
  else {
    offer_radio = (i32)(((RadioGroup *)new_input)->construct());
  }
  side_radios[side] = offer_radio;
  cur_x = side_x;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2;
  if (large_layout == 0) {
    offer_row_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    offer_row_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  block_height = panel_layout.margin_y * 5 + offer_row_height * 6;
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,block_height + cur_y);
  if (this == (ParleyWindow *)0x0) {
    offer_parent = (u8 *)0x0;
  }
  else {
    offer_parent = ((u8 *)this + 0x48);
  }
  ((RadioGroup *)side_radios[side])->create((Wnd *)offer_parent,0x41f,
             &ctrl_rect,1,7,(char **)offer_labels);
  ((RadioGroup *)side_radios[side])->set_selected(demand_kind);
  ((RadioGroup *)side_radios[side])->set_dblclick_callback((RadioNotifyFn)parley_on_radio_dblclick)
  ;
  ((RadioGroup *)side_radios[side])->set_select_callback((RadioNotifyFn)parley_on_demand_kind_select)
  ;
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = 1;
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the city list with its
   hide and reveal buttons (side kind 0x11). */
void ParleyWindow::add_city_pane(int side,u32 side_x,char is_exchange_side,char is_demand_side)
{
  u8 *flag_ptr;
  int tmp;
  Button *new_ctrl;
  u8 *city_hide_parent;
  iptr city_hide_label;
  u8 *city_reveal_parent;
  iptr city_reveal_label;
  int city_btn_font_height;
  int city_header_height;
  int city_side_height;
  iptr city_side_title;
  i8 *city_header_font;
  i32 city_hide_button;
  i32 city_reveal_button;
  i8 *city_side_font;
  tagRECT city_btn_rect;
  int city_btn_width;
  int city_btn_top;
  iptr city_header_label;
  int city_btn_height;
  i32 city_list_mode;
  int cur_y;
  uptr cur_x;
  int col_width;
  tagRECT ctrl_rect;
  
  if (is_exchange_side == '\0') {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 7;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  else {
    flag_ptr = (u8 *)&list_rows[side];
    flag_ptr[0] = 6;
    flag_ptr[1] = '\0';
    flag_ptr[2] = '\0';
    flag_ptr[3] = '\0';
  }
  gfx_set_text_color_index(10);
  cur_x = side_x;
  cur_y = panel_top + panel_layout.margin_y;
  tmp = rect_width((RECT *)&panel_left);
  col_width = (tmp + panel_layout.margin_x * -2) / 2 + panel_layout.margin_x * -5;
  if (is_exchange_side != '\0') {
    str_clear((char *)&textbuf.buf);
    if (side == 0) {
      city_side_title = (iptr)(label_string(label_table.string_ids[0x303]));
    }
    else {
      city_side_title = (iptr)(label_string(label_table.string_ids[0x304]));
    }
    text_append_str((const char *)city_side_title);
    if (large_layout == 0) {
      city_side_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      city_side_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)city_side_font,(const char *)&textbuf.buf,cur_x,cur_y,5);
    if (large_layout == 0) {
      city_side_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      city_side_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    cur_y = cur_y + panel_layout.margin_y + city_side_height;
  }
  SetRect(&ctrl_rect,cur_x,cur_y,col_width + cur_x,divider_y);
  gfx_set_draw_target(&parley_wnd_block);
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  gfx_target_frame_rect(&ctrl_rect,10);
  tmp = rect_width(&ctrl_rect);
  cur_x = ctrl_rect.left + tmp / 2;
  if (large_layout == 0) {
    city_header_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
  }
  else {
    city_header_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  }
  cur_y += city_header_height / 2;
  if ((is_exchange_side == '\0') && (is_demand_side == '\0')) {
    city_header_label = (iptr)(label_string(label_table.string_ids[0x31a]));
    if (large_layout == 0) {
      city_header_font = ((i8 *)&font_size16_bold_block.font_size16_bold);
    }
    else {
      city_header_font = ((i8 *)&font_size24_bold);
    }
    draw_text((GameFont *)city_header_font,(const char *)city_header_label,cur_x,cur_y,0);
  }
  else {
    city_btn_top = panel_layout.margin_y * 2 + ctrl_rect.top - 1;
    city_btn_width = col_width + panel_layout.margin_x * -3;
    if (large_layout == 0) {
      city_btn_font_height = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    }
    else {
      city_btn_font_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    }
    city_btn_height = city_btn_font_height - (panel_layout.margin_y * 2 - 1);
    SetRect(&city_btn_rect,panel_layout.margin_x * 2 + ctrl_rect.left,city_btn_top,ctrl_rect.left + city_btn_width,
            city_btn_height + city_btn_top);
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      city_reveal_button = 0;
    }
    else {
      city_reveal_button = (i32)(button_ctor(new_ctrl));
    }
    info_show_buttons[side] = city_reveal_button;
    if (side == 0) {
      city_reveal_label = (iptr)(label_string(label_table.string_ids[0x372]));
    }
    else {
      city_reveal_label = (iptr)(label_string(label_table.string_ids[0x373]));
    }
    if (this == (ParleyWindow *)0x0) {
      city_reveal_parent = (u8 *)0x0;
    }
    else {
      city_reveal_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_show_buttons[side])->create((Wnd *)city_reveal_parent,
               side + 0x408,&city_btn_rect,(const char *)city_reveal_label);
    ((Button *)info_show_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_reveal_city_info : parley_request_city_info));
    new_ctrl = (Button *)(operator_new(0x3c));
    if (new_ctrl == (Button *)0x0) {
      city_hide_button = 0;
    }
    else {
      city_hide_button = (i32)(button_ctor(new_ctrl));
    }
    info_hide_buttons[side] = city_hide_button;
    if (side == 0) {
      city_hide_label = (iptr)(label_string(label_table.string_ids[0x374]));
    }
    else {
      city_hide_label = (iptr)(label_string(label_table.string_ids[0x373]));
    }
    if (this == (ParleyWindow *)0x0) {
      city_hide_parent = (u8 *)0x0;
    }
    else {
      city_hide_parent = ((u8 *)this + 0x48);
    }
    ((Button *)info_hide_buttons[side])->create((Wnd *)city_hide_parent,
               side + 0x40a,&city_btn_rect,(const char *)city_hide_label);
    ((Button *)info_hide_buttons[side])->set_callback(
               (CtrlNotifyFn)(side == 0 ? parley_hide_city_info : parley_request_city_info));
    if ((side == 0) && (local_cities_revealed != '\0')) {
      ((Control *)info_show_buttons[0])->hide();
      ((Control *)info_hide_buttons[0])->show_and_redraw();
    }
    else if (side == 0) {
      ((Control *)info_show_buttons[0])->show_and_redraw();
      ((Control *)info_hide_buttons[0])->hide();
    }
    if ((side == 1) && (remote_cities_revealed != '\0')) {
      ((Control *)info_show_buttons[1])->hide();
      ((Control *)info_hide_buttons[1])->show_and_redraw();
      ((Button *)info_hide_buttons[1])->disable();
    }
    else if (side == 1) {
      ((Control *)info_show_buttons[1])->show_and_redraw();
      ((Control *)info_hide_buttons[1])->hide();
    }
  }
  if (large_layout == 0) {
    tmp = ((GameFont *)((i8 *)&font_size16_bold_block.font_size16_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  else {
    tmp = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
    ctrl_rect.top += tmp;
  }
  ctrl_rect.bottom = divider_y + panel_layout.margin_y * -2;
  ctrl_rect.left += panel_layout.margin_x * 2;
  ctrl_rect.right += panel_layout.margin_x * -2;
  gfx_target_frame_rect(&ctrl_rect,10);
  rect_deflate(&ctrl_rect,1,1);
  fill_rect(&ctrl_rect,dlg_style.list_bg);
  city_list_mode = 1;
  if ((is_demand_side != '\0') || ((is_exchange_side != '\0' && (side == 1)))) {
    city_list_mode = 0;
  }
  create_city_list_pane(&ctrl_rect,side,city_list_mode);
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the
   waiting-for-the-other-civ text (side kind 0x12). */
void ParleyWindow::add_waiting_pane()
{
  iptr label;
  int waiting_font_height;
  i8 *waiting_font;
  int waiting_text_width;
  i8 waiting_text[80];
  int cur_y;
  uptr cur_x;
  
  label = civ_get_adjective(other_civ);
  text_set_string_arg_b(0,(const char *)label);
  text_clear();
  text_append_label(0x2e2);
  /* BF-200 (Peter Johnson's ledger): cut at 79 characters */
  text_expand_vars_max(textbuf.buf,waiting_text,sizeof waiting_text);
  gfx_set_text_color_index(10);
  if (large_layout == 0) {
    waiting_text_width = ((GameFont *)((i8 *)&font_size20))->text_width((const char *)waiting_text);
  }
  else {
    waiting_text_width = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)waiting_text);
  }
  cur_x = (panel_left + (int)(panel_right - panel_left) / 2) - waiting_text_width / 2;
  if (large_layout == 0) {
    waiting_font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    waiting_font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  cur_y = (panel_top + (int)(divider_y - panel_top) / 2 + panel_layout.margin_y) -
             waiting_font_height / 2;
  if (large_layout == 0) {
    waiting_font = ((i8 *)&font_size20);
  }
  else {
    waiting_font = ((i8 *)&font_size30);
  }
  draw_text((GameFont *)waiting_font,(const char *)waiting_text,cur_x,cur_y,5);
  ((Control *)mode2_button)->hide();
  ((Button *)mode2_button)->disable();
  ((Control *)mode_button)->show_and_redraw();
  ((Button *)mode_button)->disable();
  ((Button *)never_mind_button)->disable();
  if ((local_units_revealed == '\0') && (local_cities_revealed == '\0')) {
    ((Control *)conceal_button)->hide();
    ((Button *)conceal_button)->disable();
    ((Control *)this->reveal_button)->show_and_redraw();
    ((Button *)this->reveal_button)->enable();
  }
  else {
    ((Control *)conceal_button)->show_and_redraw();
    ((Button *)conceal_button)->enable();
    ((Control *)this->reveal_button)->hide();
    ((Button *)this->reveal_button)->disable();
  }
}

/* Part of ParleyWindow::add_side_dialog (00526ca0): the offer under review
   (side kind 0x14): its description, and Accept enabled only if we have every
   advance it asks for. */
void ParleyWindow::add_review_pane()
{
  int tmp;
  iptr label;
  ParleyOfferMsg *item_buf;
  int *item_kind_ptr;
  iptr *tech_ptr;
  int tech_idx;
  int tech_count;
  
  ((SurfaceWindow *)((i8 *)&parley_wnd_block))->SurfaceWindow::show();
  show_description(1);
  parley_wnd_block.offer_acceptable = 1;
  if (received_offer != 0) {
    item_buf = received_offer;
    item_kind_ptr = &item_buf->item.type;
    if (((received_offer->mode == 0xf) && (*item_kind_ptr == 8)) ||
       ((received_offer->mode == 0xe && (*item_kind_ptr == 8)))) {
      tech_count = item_buf->item.value;
      tech_ptr = (iptr *)(item_buf + 1);   /* the ids after the item */
      for (tech_idx = 0; tech_idx < tech_count; tech_idx++) {
        label = *tech_ptr;
        tech_ptr++;
        tmp = civ_has_advance(human_civ[0],label);
        if (tmp == 0) {
          offer_acceptable = 0;
        }
      }
    }
  }
  if (offer_acceptable == 0) {
    ((Button *)accept_button)->disable();
  }
  else {
    ((Button *)accept_button)->enable();
  }
}

/* 00526ca0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void ParleyWindow::add_side_dialog(int side_kind,int side)

{
  u8 *flag_ptr;
  int tmp;
  char is_demand_side;
  u32 side_x;
  char is_exchange_side;
  
  if ((side != 0) && (side != 1)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_leftright_left_leftright_right_2,(const char*)&str_d_ss_franklinton_parleywin_add_dialog_cp,0xc);
  }
  if (side_kind < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_thedialog_pdt_null_pdt_null_pdt_count,(const char*)&str_d_ss_franklinton_parleywin_add_dialog_cp_2,0xd);
  }
  is_exchange_side = '\0';
  is_demand_side = '\0';
  flag_ptr = (u8 *)&side_selected[side];
  flag_ptr[0] = '\0';
  flag_ptr[1] = '\0';
  flag_ptr[2] = '\0';
  flag_ptr[3] = '\0';
  if (side == 0) {
    side_x = panel_left + panel_layout.margin_x;
  }
  else if (side == 1) {
    tmp = rect_width((RECT *)&panel_left);
    side_x = panel_left + tmp / 2 + panel_layout.margin_x;
  }
  if (side_kind == 0xe) {
    is_exchange_side = 1;
    switch((i32)exchange_kind[side]) {
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
    switch(demand_kind) {
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
  clear_side_controls(side);
  this->side_kind[side] = side_kind;
  if (((parley_wnd_block.closed == 0) && (side_kind != 1)) && (side_kind != 0)) {
    if (side_kind == 0x14) {
      ((Control *)mode_button)->hide();
      ((Button *)mode_button)->disable();
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)never_mind_button)->hide();
      ((Control *)accept_button)->show_and_redraw();
      ((Control *)intel_button)->hide();
      ((Control *)no_thanks_button)->show_and_redraw();
      ((Control *)counter_button)->show_and_redraw();
    }
    else {
      ((Control *)mode_button)->show_and_redraw();
      ((Button *)mode_button)->enable();
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)never_mind_button)->show_and_redraw();
      ((Control *)accept_button)->hide();
      ((Control *)intel_button)->show_and_redraw();
      ((Control *)no_thanks_button)->hide();
      ((Control *)counter_button)->hide();
    }
  }
  switch(side_kind) {
  case 0:
    break;
  case 1:
    add_chat_pane(side_x);
    break;
  case 2:
    add_mode_pane(side);
    break;
  case 3:
    add_exchange_kind_pane_left(side,side_x);
    break;
  case 4:
    add_exchange_kind_pane_right(side,side_x);
    break;
  case 5:
    add_treaty_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 6:
    add_status_pane(side,side_x);
    break;
  case 7:
    add_gift_pane(side,side_x);
    break;
  case 8:
    add_tech_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 9:
    add_gold_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 10:
    add_unit_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 0xb:
    add_proposal_pane(side,side_x);
    break;
  case 0xc:
    add_civ_list_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 0xd:
    add_map_pane(side,side_x,is_exchange_side);
    break;
  case 0xf:
    add_offer_pane(side,side_x);
    break;
  case 0x11:
    add_city_pane(side,side_x,is_exchange_side,is_demand_side);
    break;
  case 0x12:
    add_waiting_pane();
    break;
  case 0x13:
    show_description(0);
    break;
  case 0x14:
    add_review_pane();
  }
  update_buttons();
  wnd_blit_surface_rect((RECT *)&fill_left);
  return;
}

/* 00587a90 auto */
void ParleyWindow::create_city_list_pane(RECT *rect,int pane,i32 own_cities)

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
  
  rect2.left = rect->left;
  rect2.top = rect->top;
  rect2.right = rect->right;
  rect2.bottom = rect->bottom;
  sprite_picker_sprite_block.parley_pane_own_cities[pane] = own_cities;
  build_city_list(pane,0);
  if (list_counts[pane] < 1) {
    field_ptr = (u8 *)&list_first_row[pane];
    field_ptr[0] = 0xff;
    field_ptr[1] = 0xff;
    field_ptr[2] = 0xff;
    field_ptr[3] = 0xff;
  }
  else {
    field_ptr = (u8 *)&list_first_row[pane];
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
  }
  field_ptr = (u8 *)&list_columns[pane];
  field_ptr[0] = 4;
  field_ptr[1] = '\0';
  field_ptr[2] = '\0';
  field_ptr[3] = '\0';
  list_rects[pane].left = rect->left;
  list_rects[pane].top = rect->top;
  list_rects[pane].right = rect->right;
  list_rects[pane].bottom = rect->bottom;
  if (list_rows[pane] <
      list_counts[pane]) {
    scroll_width = GetSystemMetrics(2);
    right = rect2.right;
    list_rects[pane].right =
         (int)list_rects[pane].right - scroll_width;
    rect2.left = GetSystemMetrics(2);
    rect2.left = right - rect2.left;
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      scrollbar = 0;
    }
    else {
      scrollbar = (i32)(((Scrollbar *)new_ctrl)->construct());
    }
    list_scrollbars[pane] = scrollbar;
    if (this == (ParleyWindow *)0x0) {
      scroll_parent = (u8 *)0x0;
    }
    else {
      scroll_parent = ((u8 *)this + 0x48);
    }
    ((Scrollbar *)list_scrollbars[pane])->create((Wnd *)scroll_parent,
               pane + 0x420,&rect2,1);
    ((Scrollbar *)list_scrollbars[pane])->set_range(0,
               list_counts[pane] -
               list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_pos(0);
    ((Scrollbar *)list_scrollbars[pane])->set_page_size(
               list_rows[pane]);
    ((Scrollbar *)list_scrollbars[pane])->set_callback(
               (pane == 0 ? parley_city_list_scroll_local : parley_city_list_scroll_remote));
    ((Scrollbar *)list_scrollbars[pane])->set_track_callback(
               (pane == 0 ? parley_city_list_scroll_local : parley_city_list_scroll_remote));
  }
  new_ctrl = (Button *)(operator_new(0x40));
  if (new_ctrl == (Button *)0x0) {
    hotspot = 0;
  }
  else {
    hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
  }
  list_hotspots[pane] = hotspot;
  if (this == (ParleyWindow *)0x0) {
    hotspot_parent = (u8 *)0x0;
  }
  else {
    hotspot_parent = ((u8 *)this + 0x48);
  }
  ((Hotspot *)list_hotspots[pane])->create((Wnd *)hotspot_parent,
             pane + 0x422,&list_rects[pane]);
  ((Hotspot *)list_hotspots[pane])->set_click_callback(parley_city_list_click);
  ((Hotspot *)list_hotspots[pane])->set_dblclick_callback(parley_city_list_click);
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
  parley_wnd2->window.activate();
  ((Control *)parley_wnd2->info_hide_buttons[0])->show_and_redraw();
  ((Control *)parley_wnd2->info_show_buttons[0])->hide();
  parley_wnd2->local_cities_revealed = 1;
  net_send_message(NM_REVEAL_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley_wnd2->other_civ]].id,
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
  parley_wnd2->window.activate();
  ((Control *)parley_wnd2->info_show_buttons[0])->show_and_redraw();
  ((Control *)parley_wnd2->info_hide_buttons[0])->hide();
  parley_wnd2->local_cities_revealed = 0;
  net_send_message(NM_HIDE_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley_wnd2->other_civ]].id,
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
  window->window.activate();
  civ_plural = (civ_name_plural(human_civ[0]));
  text_set_string_arg(0,civ_plural);
  net_send_text_args(0x65,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,1,0,
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
  parley_wnd2->window.activate();
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
  parley_wnd2->window.activate();
  pane = control_id - 0x422;
  parley_wnd2->window.get_cursor_pos(&cursor_x,&cursor_y);
  if (((parley_wnd2->list_counts[pane] != 0) &&
      (hit_row = parley_city_list_hit_test(cursor_x,cursor_y,pane), 0 < hit_row)) &&
     (hit_row < parley_wnd2->list_counts[pane])) {
    key_down = parley_wnd2->window.is_shift_down();
    if (key_down == 0) {
      key_down = parley_wnd2->window.is_ctrl_down();
      if (key_down == 0) {
        for (row = 0; row < parley_wnd2->list_counts[pane];
            row = row + 1) {
          if (row != hit_row) {
            parley_wnd2->list_selected[pane][row] = 0;
          }
        }
        parley_wnd2->list_anchor_row[pane] = hit_row;
        sel_flag = &parley_wnd2->list_selected[pane][hit_row];
        *sel_flag = *sel_flag ^ 1;
      }
      else {
        parley_wnd2->list_anchor_row[pane] = hit_row;
        sel_flag = &parley_wnd2->list_selected[pane][hit_row];
        *sel_flag = *sel_flag ^ 1;
      }
    }
    else {
      for (row = 0; row < parley_wnd2->list_counts[pane]; row++)
      {
        parley_wnd2->list_selected[pane][row] = 0;
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
          parley_wnd2->list_selected[pane][row] = 1;
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
  if (y < (int)parley_wnd2->list_rects[pane].top) {
    result = -1;
  }
  else if (y < (int)parley_wnd2->list_rects[pane].bottom) {
    if (x < (int)parley_wnd2->list_rects[pane].left) {
      result = -3;
    }
    else if (x < (int)parley_wnd2->list_rects[pane].right) {
      result = (int)parley_wnd2->list_rects[pane].top;
      height = rect_height(&parley_wnd2->list_rects[pane]);
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
void ParleyWindow::sort_city_list(int first,int last,int pane)

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
        cmp = city_has_improvement(parley_wnd2->list_items[pane][j],IMP_PALACE);
        if ((cmp != 0) ||
           ((cmp = city_has_improvement(parley_wnd2->list_items[pane][i]
                                        ,IMP_PALACE), cmp == 0 &&
            (cmp = _strcmp(cities[(int)parley_wnd2->list_items[pane][i]].
                             name,cities[(int)parley_wnd2->list_items[pane][j]
                                        ].name), 0 < cmp)))) {
          tmp = (i32)parley_wnd2->list_items[pane][i];
          parley_wnd2->list_items[pane][i] =
               (i32)parley_wnd2->list_items[pane][j];
          parley_wnd2->list_items[pane][j] = tmp;
          tmp = (i32)parley_wnd2->list_selected[pane][i];
          parley_wnd2->list_selected[pane][i] =
               (i32)parley_wnd2->list_selected[pane][j];
          parley_wnd2->list_selected[pane][j] = tmp;
          tmp = (i32)parley_wnd2->list_selected_b[pane][i];
          parley_wnd2->list_selected_b[pane][i] =
               (i32)parley_wnd2->list_selected_b[pane][j];
          parley_wnd2->list_selected_b[pane][j] = tmp;
          tmp = (i32)parley_wnd2->list_items_b[pane][i];
          parley_wnd2->list_items_b[pane][i] =
               (i32)parley_wnd2->list_items_b[pane][j];
          parley_wnd2->list_items_b[pane][j] = tmp;
        }
      }
    }
  }
  return;
}

/* 0058878e auto */
void __cdecl parley_draw_city_list(int pane)

{
  int stale;
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
  this2->get_clip((RECT *)saved_clip);
  this2->set_clip(&this2->list_rects[pane]);
  for (;;) {
    this2->fill_rect(&this2->list_rects[pane],dlg_style.list_bg);
    if (this2->large_layout == 0) {
      font2 = (GameFont *)((i8 *)&font_size14);
    }
    else {
      font2 = (GameFont *)((i8 *)&font_size21);
    }
    xLeft = (int)this2->list_rects[pane].left;
    width = rect_width(&this2->list_rects[pane]);
    row_height = rect_height(&this2->list_rects[pane]);
    row_height = row_height / this2->list_rows[pane];
    text_offset_y = row_height / 2;
    row_y = font2->font_cached_height();
    text_offset_y -= row_y / 2;
    field_ptr = (u8 *)&this2->side_selected[pane];
    field_ptr[0] = '\0';
    field_ptr[1] = '\0';
    field_ptr[2] = '\0';
    field_ptr[3] = '\0';
    for (row = 0; row < this2->list_counts[pane];
        row = row + 1) {
      if (this2->list_selected[pane][row] != 0) {
        field_ptr = (u8 *)&this2->side_selected[pane];
        field_ptr[0] = 1;
        field_ptr[1] = '\0';
        field_ptr[2] = '\0';
        field_ptr[3] = '\0';
        break;
      }
    }
    stale = 0;
    for (row = 0; row < this2->list_rows[pane]; row++) {
      row_y = (int)this2->list_rects[pane].top + row * row_height;
      SetRect(&row_rect,xLeft,row_y,width + xLeft,row_y + row_height);
      if ((row != 0) && (this2->list_first_row[pane] == -1)) {
        break;
      }
      if (this2->list_first_row[pane] + row <
          this2->list_counts[pane]) {
        selected = (int)this2->list_selected[pane][this2->list_first_row[pane] + row];
        if (selected == 0) {
          if (this2->list_first_row[pane] + row == 0) {
            text_color2 = (uint)dlg_style.highlight_text_color;
            shadow_color2 = (*(uint *)&dlg_style.sel_text_shadow_color);
          }
          else {
            text_color2 = (*(uint *)&dlg_style.list_text);
            shadow_color2 = (*(uint *)&dlg_style.list_shadow);
          }
        }
        else {
          text_color2 = (*(uint *)&dlg_style.sel_text_color);
          shadow_color2 = (*(uint *)&dlg_style.sel_text_shadow_color);
        }
        city = 0;
        if ((0 < this2->list_counts[pane]) &&
           (city = this2->find_city_by_id(
                                  (i32)this2->list_items_b[pane][this2->list_first_row[pane] + row]), city == -1)) {
          stale = 1;  /* not found: the list is rebuilt and drawn again */
          break;
        }
        text_offset_x = (-(uint)(this2->large_layout == 0) & 0xffffffe2) + 0x5a + panel_layout.margin_x;
        row_rect.left = row_rect.left + text_offset_x - 3;
        if (selected == 0) {
          bg_color2 = dlg_style.list_bg;
        }
        else {
          bg_color2 = dlg_style.list_sel_bg;
        }
        this2->fill_rect(&row_rect,bg_color2);
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
          label = (label_string(label_table.string_ids[0x2de]));
          text_append(label);
          field_ptr = (u8 *)&this2->side_selected[pane];
          field_ptr[0] = '\0';
          field_ptr[1] = '\0';
          field_ptr[2] = '\0';
          field_ptr[3] = '\0';
        }
        else {
          selected = sprite_style;
          font_height = font2->font_cached_height();
          parley_draw_city_sprite(city,this2->list_first_row[pane] + row,
                             xLeft + 2,row_y,font_height,selected);
          text_clear();
          text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
          text_append_space();
          selected = city_has_improvement(city,IMP_PALACE);
          if ((selected == 0) && (sprite_style == 0)) {
            text_append_lparen();
            text_append_int((iptr)this2->list_selected_b[pane][this2->list_first_row[pane] + row]);
            text_append_space();
            if ((int)this2->list_selected_b[pane][this2->list_first_row[pane] + row] < 2) {
              label = (label_string(label_table.string_ids[0x2dc]));
              text_append(label);
            }
            else {
              label = (label_string(label_table.string_ids[0x2db]));
              text_append(label);
            }
          }
          else {
            selected = city_has_improvement(city,IMP_PALACE);
            if (selected != 0) {
              label = (label_string(label_table.string_ids[0x2dd]));
              text_append(label);
            }
          }
        }
        if (shadow_color2 != text_color2) {
          gfx_set_text_color_index(shadow_color2);
          this2->draw_text(font2,(const char *)&textbuf.buf,xLeft + text_offset_x + 2,
                     row_y + text_offset_y + 1,5);
          gfx_set_text_color_index(text_color2);
          this2->draw_text(font2,(const char *)&textbuf.buf,xLeft + text_offset_x + 1,row_y + text_offset_y,
                     5);
        }
        gfx_set_text_color_index(text_color2);
        this2->draw_text(font2,(const char *)&textbuf.buf,xLeft + text_offset_x,row_y + text_offset_y,5);
      }
    }
    if (stale == 0) {
      this2->set_clip((RECT *)saved_clip);
      this2->update_buttons();
      ((Control *)this2->list_hotspots[pane])->show_and_redraw();
      return;
    }
    this2->build_city_list(pane,1);
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
  }
}

/* 00588e47 auto */
void __cdecl
parley_draw_city_sprite(int city,uint row_index,int x,int y,int row_height,i32 style)

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
  gfx_draw_city(&parley_wnd_block,city,style,draw_x,y - (sprite_height2 - row_height) / 2,zoom2);
  return;
}

/* 00588f36 auto */
void ParleyWindow::build_city_list(int pane,int keep_selection)

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
        if (parley_wnd2->list_selected[pane][city] == 0) {
          *(i32 *)((int)old_selection + city * 4) = 0xffffffff;
        }
        else {
          *(i32 *)((int)old_selection + city * 4) =
               (i32)parley_wnd2->list_items_b[pane][city];
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
      sort_city_list(0,parley_wnd2->list_counts[pane] - 1,pane);
      return;
    }
    if ((cities[city].id != 0) &&
       (((sprite_picker_sprite_block.parley_pane_own_cities[pane] != 0 && (cities[city].owner == (*(i8 *)&human_civ)))
        || (((sprite_picker_sprite_block.parley_pane_own_cities[pane] == 0 &&
             ((int)cities[city].owner == other_civ)) &&
            (((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (int)(char)cities[city].known_to) != 0 ||
             ((int)cities[city].owner == ((*(i8 *)&human_civ) & 0xffU))))))))) {
      parley_wnd2->list_items[pane][parley_wnd2->list_counts[pane]] =
           city;
      parley_wnd2->list_items_b[pane][parley_wnd2->list_counts[pane]] =
           cities[city].id;
      if (keep_selection == 0) {
        parley_wnd2->list_selected[pane][parley_wnd2->list_counts[pane]] = 0;
      }
      else {
        parley_wnd2->list_selected[pane][parley_wnd2->list_counts[pane]] = 0;
        for (unit = 0; unit < old_count; unit++) {
          if (cities[city].id == *(int *)((int)old_selection + unit * 4)) {
            parley_wnd2->list_selected[pane][parley_wnd2->list_counts[pane]] = 1;
            break;
          }
        }
      }
      parley_wnd2->list_selected_b[pane][parley_wnd2->list_counts[pane]] = 0;
      for (unit = 0; unit < game.num_units; unit++) {
        if (((units[unit].id != 0) && (cities[city].x == units[unit].x)) &&
           (cities[city].y == units[unit].y)) {
          count_ptr = (int *)&parley_wnd2->list_selected_b[pane][parley_wnd2->list_counts[pane]];
          *count_ptr = *count_ptr + 1;
        }
      }
      count_ptr = &parley_wnd2->list_counts[pane];
      *count_ptr = *count_ptr + 1;
    }
    city++;
  } while( true );
}
