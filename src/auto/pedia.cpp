/* The game, the Civilopedia: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00451af0 auto */
void PediaWindow::draw_category_body(i32 category,i32 flags)

{
  int xLeft;
  int yTop;
  int col_width;
  int line_height;
  int title_height;
  tagRECT body_rect;
  int body_height;
  
  gfx_set_text_color_index(10);
  xLeft = content_left + panel_layout.margin_x;
  yTop = content_top + panel_layout.margin_y;
  col_width = rect_width((RECT *)&content_left);
  col_width = (col_width + panel_layout.margin_x * -2) / 2 - panel_layout.margin_x;
  line_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  title_height = ((GameFont *)((i8 *)&font_size24_bold))->font_cached_height();
  body_height = line_height * 9 + title_height + panel_layout.margin_y * 0xd;
  SetRect(&body_rect,xLeft,yTop,col_width + xLeft,body_height + yTop);
  gfx_set_draw_target((iptr)&pedia_wnd.window);
  dispatch_category_draw(body_rect.left,body_rect.top,body_rect.right,body_rect.bottom,category);
  wnd_blit_surface_rect((iptr)pedia_body_blit_rect);
  return;
}

/* 00451bf0 auto */
void PediaWindow::update_controls()

{
  u32 view_mode;
  int scroll_height;
  Hotspot *new_ctrl;
  u8 *hotspot_parent2;
  u8 *hotspot_parent;
  Hotspot *hotspot2;
  Hotspot *hotspot;
  struct { u32 left; u32 top; u32 right; u32 bottom; } rect2;
  i32 seh_frame;
  
  ((Control *)list_describe_button)->hide();
  ((Control *)((u8 *)&list_close_button))->hide();
  ((Control *)((u8 *)&detail_back_button))->hide();
  ((Control *)detail_describe_button)->hide();
  ((Control *)&detail_close_button)->hide();
  ((Control *)text_back_button)->hide();
  ((Control *)&text_close_button)->hide();
  ((Control *)&list_scrollbar)->hide();
  ((Control *)(u8 *)&concept_list_back_button)->hide();
  ((Control *)advance_list_describe_button)->hide();
  ((Control *)advance_list_tech_tree_button)->hide();
  ((Control *)advance_list_close_button)->hide();
  ((Control *)&advance_detail_back_button)->hide();
  ((Control *)((u8 *)&advance_detail_tech_tree_button))->hide();
  ((Control *)advance_detail_describe_button)->hide();
  ((Control *)&advance_detail_close_button)->hide();
  ((Control *)&description_edit)->hide();
  ((Control *)&description_edit)->set_text((const char *)&str_empty_73);
  if (list_hotspot != 0) {
    if ((Hotspot *)list_hotspot != (Hotspot *)0x0) {
      ((Hotspot *)list_hotspot)->scalar_destruct(1);
    }
    list_hotspot = 0;
  }
  view_mode = border_width;
  if (view_mode == 0) {
    list_rect = content_left;
    list_rect_top = content_top;
    list_rect_right = content_right;
    list_rect_bottom = content_bottom;
    scroll_height = GetSystemMetrics(3);
    list_rect_bottom -= scroll_height;
    if (category == 8) {
      ((Control *)(u8 *)&concept_list_back_button)->show_and_redraw();
      ((Control *)((u8 *)&list_close_button))->show_and_redraw();
    }
    else if (category == 1) {
      ((Control *)advance_list_describe_button)->show_and_redraw();
      ((Control *)advance_list_tech_tree_button)->show_and_redraw();
      ((Control *)advance_list_close_button)->show_and_redraw();
      ((Control *)&list_scrollbar)->show_and_redraw();
      if (list_hotspot == 0) {
        rect2.left = content_left;
        rect2.top = content_top;
        rect2.right = content_right;
        rect2.bottom = content_bottom;
        scroll_height = GetSystemMetrics(3);
        rect2.bottom -= scroll_height;
        new_ctrl = (Hotspot *)operator_new(0x40);
        if (new_ctrl == (Hotspot *)0x0) {
          hotspot = 0;
        }
        else {
          hotspot = hotspot_ctor(new_ctrl);
        }
        list_hotspot = (u32)hotspot;
        if (this == (PediaWindow *)0x0) {
          hotspot_parent = (u8 *)0x0;
        }
        else {
          hotspot_parent = (u8 *)&window;
        }
        ((Hotspot *)list_hotspot)->create((iptr)hotspot_parent,0xfa9,(iptr)&rect2);
        ((Hotspot *)list_hotspot)->set_click_callback((CtrlNotifyFn)pedia_list_on_click);
        ((Hotspot *)list_hotspot)->set_dblclick_callback(pedia_toggle_describe);
        ((Control *)list_hotspot)->show_and_redraw();
      }
    }
    else {
      ((Control *)list_describe_button)->show_and_redraw();
      ((Control *)&list_scrollbar)->show_and_redraw();
      ((Control *)((u8 *)&list_close_button))->show_and_redraw();
      if (list_hotspot == 0) {
        rect2.left = content_left;
        rect2.top = content_top;
        rect2.right = content_right;
        rect2.bottom = content_bottom;
        scroll_height = GetSystemMetrics(3);
        rect2.bottom -= scroll_height;
        new_ctrl = (Hotspot *)operator_new(0x40);
        if (new_ctrl == (Hotspot *)0x0) {
          hotspot2 = 0;
        }
        else {
          hotspot2 = hotspot_ctor(new_ctrl);
        }
        list_hotspot = (u32)hotspot2;
        if (this == (PediaWindow *)0x0) {
          hotspot_parent2 = (u8 *)0x0;
        }
        else {
          hotspot_parent2 = (u8 *)&window;
        }
        ((Hotspot *)list_hotspot)->create((iptr)hotspot_parent2,0xfa9,(iptr)&rect2);
        ((Hotspot *)list_hotspot)->set_click_callback((CtrlNotifyFn)pedia_list_on_click);
        ((Hotspot *)list_hotspot)->set_dblclick_callback(pedia_toggle_describe);
        ((Control *)list_hotspot)->show_and_redraw();
      }
    }
  }
  else if (view_mode == 1) {
    if ((category == 6) || (category == 7)) {
      ((Control *)text_back_button)->show_and_redraw();
      ((Control *)&text_close_button)->show_and_redraw();
    }
    else if (category == 1) {
      ((Control *)&advance_detail_back_button)->show_and_redraw();
      ((Control *)((u8 *)&advance_detail_tech_tree_button))->show_and_redraw();
      ((Control *)advance_detail_describe_button)->show_and_redraw();
      ((Control *)&advance_detail_close_button)->show_and_redraw();
    }
    else {
      ((Control *)((u8 *)&detail_back_button))->show_and_redraw();
      ((Control *)detail_describe_button)->show_and_redraw();
      ((Control *)&detail_close_button)->show_and_redraw();
    }
  }
  else if (view_mode == 2) {
    ((Control *)text_back_button)->show_and_redraw();
    ((Control *)&text_close_button)->show_and_redraw();
  }
  ui_pump_messages();
  return;
}

/* 00452188 auto */
void PediaWindow::dispatch_category_draw(i32 left,i32 top,i32 right, i32 bottom,i32 category)

{
  i32 rect[4] = {left, top, right, bottom};  /* the rectangle passed as four arguments */
  switch(category) {
  case 0:
    break;
  case 1:
    if (border_width == 0) {
      init_advance_list((iptr)rect);
    }
    else {
      draw_advance_page();
    }
    break;
  case 2:
    if (border_width == 0) {
      init_improvement_list((iptr)rect);
    }
    else {
      draw_improvement_page();
    }
    break;
  case 3:
    if (border_width == 0) {
      init_wonder_list((iptr)rect);
    }
    else {
      draw_improvement();
    }
    break;
  case 4:
    if (border_width == 0) {
      show_unit_list((iptr)rect);
    }
    else {
      draw_unit_page();
    }
    break;
  case 5:
    if (border_width == 0) {
      init_terrain_list((iptr)rect);
    }
    break;
  case 6:
    if (border_width == 0) {
      init_government_list((iptr)rect);
    }
    break;
  case 7:
    if (border_width == 0) {
      init_concept_list((iptr)rect);
    }
    break;
  case 8:
    draw_advance_tree();
  }
  return;
}

/* 00452315: written by hand in src/game/retvals.cpp */

/* 00452768 auto */
void PediaWindow::update_describe_button(int list_index)

{
  int result;
  int topic_id;
  i8 path[260];
  
  if ((((game.game_flags & GAME_SCENARIO) != 0) && (category != 7)) &&
     (result = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), result != 0)) {
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcpy(path,str_describe_2);
    crt_strcat(path,((i8 *)&lang_file_ext));
    result = file_exists((const char *)path);
    if (result == 0) {
      ((Button *)detail_describe_button)->disable();
      __chdir(((i8 *)&game_dirs.home));
      return;
    }
    __chdir(((i8 *)&game_dirs.home));
  }
  switch(category) {
  case 1:
    topic_id = pedia_advance_keys[list_index];
    break;
  case 2:
    topic_id = pedia_improve_keys[list_index];
    break;
  case 3:
    topic_id = pedia_wonder_keys[list_index];
    break;
  case 4:
    topic_id = pedia_unit_keys[list_index];
    break;
  case 5:
    topic_id = pedia_terrain_keys[list_index];
    break;
  case 6:
    topic_id = pedia_government_keys[list_index];
    break;
  case 7:
    topic_id = pedia_concept_keys[list_index];
  }
  if (topic_id < 0) {
    if (category == 1) {
      ((Button *)advance_detail_describe_button)->disable();
    }
    else {
      ((Button *)detail_describe_button)->disable();
    }
  }
  else if (category == 1) {
    ((Button *)advance_detail_describe_button)->enable();
  }
  else {
    ((Button *)detail_describe_button)->enable();
  }
  return;
}

/* 004529df auto */
void __cdecl hyperlink_register_class(void)

{
  WNDCLASSA wnd_class;
  
  if (hyperlink_class_registered == 0) {
    hyperlink_class_registered = 1;
    wnd_class.style = 0x88;
    wnd_class.lpfnWndProc = ctrl_default_wndproc;
    wnd_class.cbClsExtra = 0;
    wnd_class.cbWndExtra = 0x10;
    wnd_class.hInstance = err_report_ctx_block.app_hinstance;
    wnd_class.hIcon = (HICON)0x0;
    wnd_class.hCursor = LoadCursorA(err_report_ctx_block.app_hinstance,(LPCSTR)0x212);
    wnd_class.hbrBackground = (HBRUSH)0x0;
    wnd_class.lpszMenuName = (LPCSTR)0x0;
    wnd_class.lpszClassName = str_mshypertextclass;
    RegisterClassA(&wnd_class);
  }
  return;
}

/* 00452a67 auto */
void Hyperlink::destruct()

{
  i32 seh_frame;
  i32 seh_state;
  
  seh_state = 0;
  if (hwnd != 0) {
    region_dispose(hwnd);
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00452ac1 */ { base_destruct(); }
  return;
}

/* 00452ae2 auto */
HWND __cdecl hyperlink_create_hwnd(int *rect,iptr control)

{
  WndBase *self;
  NativeWindow *parent_wnd;
  iptr height;
  int width;
  HWND link_hwnd;
  HMENU hMenu;
  HINSTANCE hInstance;
  LPVOID lpParam;
  ControlBlock *block;
  
  hyperlink_register_class();
  lpParam = (LPVOID)0x0;
  hMenu = (HMENU)0x0;
  hInstance = err_report_ctx_block.app_hinstance;
  self = (WndBase *)((Control *)control)->get_parent();
  parent_wnd = self->get_window();
  link_hwnd = parent_wnd->hwnd;
  height = rect_height((RECT *)rect);
  width = rect_width((RECT *)rect);
  link_hwnd = CreateWindowExA(4,str_mshypertextclass_2,((i8 *)&hyperlink_window_title_str),0x50000000,*rect,
                           rect[1],width,height,link_hwnd,hMenu,hInstance,lpParam);
  block = ctrl_init_block(link_hwnd,control);
  block->kind = 1;
  SetWindowLongA(link_hwnd,-4,(iptr)hotspot_wndproc);
  return link_hwnd;
}

/* 00452b89 auto */
void Hyperlink::create(iptr parent,i32 control_id,iptr rect)

{
  u32 hwnd;
  
  if (this->hwnd != 0) {
    destroy();
  }
  init(parent,1,control_id,(RECT *)rect);
  on_enter = 0;
  on_leave = 0;
  on_click = 0;
  on_press = 0;
  on_dblclick = 0;
  hwnd = hyperlink_create_hwnd((int *)rect,(iptr)this);
  this->hwnd = hwnd;
  return;
}

/* 00452c14 auto */
void PediaWindow::add_link(iptr item_id,int x,int y,i32 link_kind, i32 rect_left,i32 rect_top,i32 rect_right,i32 rect_bottom)

{
  i32 rect[4] = {rect_left, rect_top, rect_right, rect_bottom};  /* the rectangle passed as four arguments */
  int text_width;
  int max_width;
  uptr len;
  PediaLink *new_link;
  int underline_y;
  u8 *parent_wnd2;
  PediaLink *link;
  u32 list_index;
  int link_index;
  PediaLink *last_link;
  i32 seh_frame;
  
  switch(link_kind) {
  case 0:
    list_index = 0;
    while (((int)list_index < (int)pedia_advance_count &&
           (pedia_advance_ids[list_index] != item_id))) {
      list_index++;
    }
    if (pedia_advance_count == list_index) return;
    break;
  case 1:
    list_index = 0;
    while (((int)list_index < (int)pedia_improve_count &&
           (pedia_improve_ids[list_index] != item_id))) {
      list_index++;
    }
    if (pedia_improve_count == list_index) return;
    break;
  case 2:
    for (list_index = 0;
        ((int)list_index < pedia_wonder_count &&
        (pedia_wonder_ids[list_index] != item_id)); list_index++) {
    }
    if ((u32)pedia_wonder_count == list_index) return;
    break;
  case 3:
    list_index = 0;
    while (((int)list_index < pedia_unit_count &&
           (pedia_unit_ids[list_index] != item_id))) {
      list_index++;
    }
    if ((u32)pedia_unit_count == list_index) return;
    break;
  case 4:
    list_index = 0;
    while (((int)list_index < (int)pedia_terrain_count_2 &&
           (pedia_terrain_ids[list_index] != item_id))) {
      list_index++;
    }
    if (pedia_terrain_count_2 == list_index) return;
    break;
  default:
    return;
  }
  while( true ) {
    text_width = ((GameFont *)((i8 *)&font_size21))->text_width((const char *)&textbuf.buf);
    max_width = rect_width((RECT *)rect);
    if (text_width <= max_width) break;
    len = _strlen(((i8 *)&textbuf.buf));
    (((i8 *)&textbuf.before))[len] = 0;
  }
  if ((category == 8) && (text_width = civ_has_advance(human_civ[0],item_id), text_width != 0)) {
    gfx_set_text_color_index(0x6a);
  }
  else {
    gfx_set_text_color_index(0x55);
  }
  text_width = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  underline_y = y + text_width - 2;
  text_width = ((GameFont *)((i8 *)&font_size21))->text_width((const char *)&textbuf.buf);
  text_width = x + text_width;
  max_width = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  draw_line(x,y + max_width - 2,text_width,underline_y);
  gfx_set_text_color_index(10);
  draw_text((iptr)&font_size21,(const char *)&textbuf.buf,x + 1,y,5);
  if ((category == 8) && (text_width = civ_has_advance(human_civ[0],item_id), text_width != 0)) {
    gfx_set_text_color_index(0x6a);
  }
  else {
    gfx_set_text_color_index(0x55);
  }
  draw_text((iptr)&font_size21,(const char *)&textbuf.buf,x,y,5);
  gfx_set_text_color_index(10);
  new_link = (PediaLink *)operator_new(0x48);
  if (new_link == (PediaLink *)0x0) {
    link = (PediaLink *)0x0;
  }
  else {
    link = new_link->construct();
  }
  link->topic = list_index;
  link->next = 0;
  link_index = 0;
  if (link_list_head == (PediaLink *)0x0) {
    link_list_head = link;
  }
  else {
    link_index = 1;
    for (last_link = link_list_head; last_link->next != (PediaLink *)0x0;
        last_link = last_link->next) {
      link_index++;
    }
    last_link->next = link;
  }
  if (this == (PediaWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  link->control.create((iptr)parent_wnd2,link_index + 0xfb4,(iptr)rect);
  switch(link_kind) {
  case 0:
    link->control.set_click_callback(pedia_link_on_click_advance);
    link->control.set_dblclick_callback(pedia_link_on_click_advance);
    break;
  case 1:
    link->control.set_click_callback(pedia_link_on_click_improvement);
    link->control.set_dblclick_callback(pedia_link_on_click_improvement);
    break;
  case 2:
    link->control.set_click_callback(pedia_link_on_click_wonder);
    link->control.set_dblclick_callback(pedia_link_on_click_wonder);
    break;
  case 3:
    link->control.set_click_callback(pedia_link_on_click_unit);
    link->control.set_dblclick_callback(pedia_link_on_click_unit);
    break;
  case 4:
    link->control.set_click_callback(pedia_link_on_click_terrain);
    link->control.set_dblclick_callback(pedia_link_on_click_terrain);
  }
  ((Control *)&link->control)->show_and_redraw();
  return;
}

/* 004531b8 auto */
void __cdecl pedia_link_on_click_advance(int control_id)

{
  PediaLink *link;
  int link_index;
  
  link_index = control_id - 0xfb4;
  for (link = pedia_wnd.link_list; (link_index != 0 && (link != (PediaLink *)0x0));
      link = link->next) {
    link_index--;
  }
  if (link != (PediaLink *)0x0) {
    pedia_wnd.category = 1;
    pedia_wnd.page_mode = 0;
    pedia_wnd.selected_item = link->topic;
    pedia_toggle_describe(1);
  }
  return;
}

/* 0045323a auto */
void __cdecl pedia_link_on_click_improvement(int control_id)

{
  PediaLink *link;
  int link_index;
  
  link_index = control_id - 0xfb4;
  for (link = pedia_wnd.link_list; (link_index != 0 && (link != (PediaLink *)0x0));
      link = link->next) {
    link_index--;
  }
  if (link != (PediaLink *)0x0) {
    pedia_wnd.category = 2;
    pedia_wnd.page_mode = 0;
    pedia_wnd.selected_item = link->topic;
    pedia_toggle_describe(1);
  }
  return;
}

/* 004532bc auto */
void __cdecl pedia_link_on_click_wonder(int control_id)

{
  PediaLink *link;
  int link_index;
  
  link_index = control_id - 0xfb4;
  for (link = pedia_wnd.link_list; (link_index != 0 && (link != (PediaLink *)0x0));
      link = link->next) {
    link_index--;
  }
  if (link != (PediaLink *)0x0) {
    pedia_wnd.category = 3;
    pedia_wnd.page_mode = 0;
    pedia_wnd.selected_item = link->topic;
    pedia_toggle_describe(1);
  }
  return;
}

/* 0045333e auto */
void __cdecl pedia_link_on_click_unit(int control_id)

{
  PediaLink *link;
  int link_index;
  
  link_index = control_id - 0xfb4;
  for (link = pedia_wnd.link_list; (link_index != 0 && (link != (PediaLink *)0x0));
      link = link->next) {
    link_index--;
  }
  if (link != (PediaLink *)0x0) {
    pedia_wnd.category = 4;
    pedia_wnd.page_mode = 0;
    pedia_wnd.selected_item = link->topic;
    pedia_toggle_describe(1);
  }
  return;
}

/* 004533c0 auto */
void __cdecl pedia_link_on_click_terrain(int control_id)

{
  PediaLink *link;
  int link_index;
  
  link_index = control_id - 0xfb4;
  for (link = pedia_wnd.link_list; (link_index != 0 && (link != (PediaLink *)0x0));
      link = link->next) {
    link_index--;
  }
  if (link != (PediaLink *)0x0) {
    pedia_wnd.category = 5;
    pedia_wnd.page_mode = 0;
    pedia_wnd.selected_item = link->topic;
    pedia_toggle_describe(1);
  }
  return;
}

/* 00453b10 auto */
PediaLink *PediaLink::construct()

{
  i32 seh_frame;
  
  control.construct();
  return this;
}

/* 00453cc0 auto */
Hyperlink *Hyperlink::construct()

{
  i32 seh_frame;
  
  base_construct();
  return this;
}

/* 00453d40 auto */
void Hyperlink::set_click_callback(CtrlNotifyFn callback)

{
  on_click = callback;
  return;
}

/* 00453d70 auto */
/* Library Function - Single Match
    public: void __thiscall CDialog::SetHelpID(unsigned int)
   
   Library: Visual Studio 1998 Debug */

void Hyperlink::set_dblclick_callback(CtrlNotifyFn callback)

{
  on_dblclick = callback;
  return;
}

/* 00490590 auto */
void PediaWindow::init_terrain_list(iptr rect)

{
  int i;
  
  if (((int)resize_border < 0) || (pedia_terrain_count <= (int)resize_border)) {
    resize_border = 0;
  }
  list_count = pedia_terrain_count;
  list_selected_index = resize_border;
  list_top_index = resize_border;
  while (pedia_wnd.list_top % 9 != 0) {
    list_top_index = list_top_index - 1;
  }
  for (i = 0; i < pedia_terrain_count; i++) {
    list_items[i] =
         pedia_terrain_ids[i];
  }
  ((Scrollbar *)&list_scrollbar)->set_range(0,list_count / 9);
  ((Scrollbar *)&list_scrollbar)->set_pos(list_top_index / 9);
  if (list_count < 1) {
    list_top_index = -1;
  }
  update_controls();
  pedia_wnd.window.window.set_focus();
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* Part of PediaWindow::draw_terrain_page (004906fd): a special resource:
   what it adds to its base terrain's food, shields and trade. */
void PediaWindow::draw_terrain_special_bonus(GameFont *font2,iptr terrain2,iptr terrain3,int y2,
                                                       int line_height,int margin)
{
  u8 shields;
  int measured;
  iptr terrain_index;
  char *label;
  char *_Format;
  int icon_center_y;
  i8 *name_arg;
  char format_buf[256];
  int base_trade2;
  i8 base_name[0x100];  /* BF-043 (Peter Johnson's ledger): the original's 40,
                          less than a terrain's name */
  int base_shields;
  int special_trade;
  int base_food;
  int special_shields;
  int special_food;
  u32 x2;

  x2 = content_left + margin;
  y2 += line_height * 2;
  terrain_index = pedia_find_terrain_index(terrain3);
  label = pedia_lookup_text(pedia_wnd.terrain_names,terrain_index);
  str_copy_max(base_name,label,sizeof base_name);
  base_food = (int)(char)terrain_types[terrain3].food;
  if (terrain3 == TERRAIN_GRASSLAND) {
    shields = '\0';
  }
  else {
    shields = terrain_types[terrain3].shields;
  }
  base_shields = (int)(char)shields;
  base_trade2 = (int)(char)terrain_types[terrain3].trade;
  special_food = (int)(char)terrain_types[terrain2].food;
  special_shields = (int)(char)terrain_types[terrain2].shields;
  special_trade = (int)(char)terrain_types[terrain2].trade;
  text_clear();
  if (base_food < special_food) {
    name_arg = base_name;
    measured = base_food;
    icon_center_y = special_food;
    _Format = label_string(label_table.string_ids[0x2c4]);
    _sprintf(((i8 *)&textbuf.buf),_Format,name_arg,measured,icon_center_y);
  }
  else {
    label = label_string(label_table.string_ids[0x2c5]);
    crt_strcpy(((i8 *)&textbuf.buf),label);
  }
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  y2 += line_height;
  text_clear();
  str_clear((char *)format_buf);
  if (base_shields < special_shields) {
    label = label_string(label_table.string_ids[0x2c6]);
    crt_strcpy(format_buf,label);
    _sprintf(((i8 *)&textbuf.buf),format_buf,base_shields,special_shields);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    y2 += line_height;
    text_clear();
    str_clear((char *)format_buf);
    if (base_trade2 < special_trade) {
      label = label_string(label_table.string_ids[0x2c7]);
      crt_strcat(format_buf,label);
      _sprintf(((i8 *)&textbuf.buf),format_buf,base_trade2,special_trade);
    }
    else {
      label = label_string(label_table.string_ids[0x2c8]);
      crt_strcat(((i8 *)&textbuf.buf),label);
    }
  }
  else {
    label = label_string(label_table.string_ids[0x2c9]);
    crt_strcpy(((i8 *)&textbuf.buf),label);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    y2 += line_height;
    text_clear();
    str_clear((char *)format_buf);
    if (base_trade2 < special_trade) {
      label = label_string(label_table.string_ids[0x2c7]);
      crt_strcat(format_buf,label);
      _sprintf(((i8 *)&textbuf.buf),format_buf,base_trade2,special_trade);
    }
    else {
      label = label_string(label_table.string_ids[0x2ca]);
      crt_strcat(((i8 *)&textbuf.buf),label);
    }
  }
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
}

/* Part of PediaWindow::draw_terrain_page (004906fd): a base terrain's
   irrigation and mining rows: the bonus (or the terrain it turns into, as
   a link) and the turns it takes. */
void PediaWindow::draw_terrain_improve_rows(GameFont *font2,iptr terrain2,int y2,int line_height,
                                                      int margin,int value_x,int row_height)
{
  int measured;
  iptr terrain_index;
  char *label;
  int icon_center_y;
  i8 shield_sprite_ctx2[16];
  i8 food_sprite_ctx2[16];
  int link_height;
  iptr improve_kind;
  u32 x2;
  int link_width;
  tagRECT link_rect;

  for (improve_kind = 0; improve_kind < 2; improve_kind++) {
    x2 = content_left + margin;
    y2 += line_height;
    text_clear();
    if (improve_kind == 0) {
      text_append_label(0x297);
    }
    else {
      text_append_label(0x299);
    }
    text_append_colon();
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    text_clear();
    if (terrain_types[terrain2].improve_to[improve_kind] < '\0') {
      if ((char)terrain_types[terrain2].improve_bonus[improve_kind] < 1) {
        text_append_label(0x29b);
        draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
      }
      else {
        _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_terrain_improve_bonus_fmt),(int)(char)terrain_types[terrain2].improve_bonus[improve_kind]
                );
        draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
        measured = font2->text_width((const char *)&textbuf.buf);
        x2 = value_x + measured + 3;
        if (improve_kind == 0) {
          icon_center_y = y2 + row_height / 2;
          measured = ((Sprite *)((i8 *)&war_icon_sprite_block.resource_icon_sprites))->get_height();
          ((Sprite *)((i8 *)&war_icon_sprite_block.resource_icon_sprites))->draw((iptr)food_sprite_ctx2,(iptr)&pedia_wnd.window,x2,
                     (icon_center_y - 2) - measured / 2);
        }
        else {
          icon_center_y = y2 + row_height / 2;
          measured = ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->get_height();
          ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->draw((iptr)shield_sprite_ctx2,(iptr)&pedia_wnd.window,x2,
                     (icon_center_y - 2) - measured / 2);
        }
      }
    }
    else {
      terrain_index = pedia_find_terrain_index((int)terrain_types[terrain2].improve_to[improve_kind]);
      label = pedia_lookup_text(pedia_wnd.terrain_names,terrain_index);
      crt_strcpy(((i8 *)&textbuf.buf),label);
      link_width = font2->text_width((const char *)&textbuf.buf);
      link_height = font2->line_spacing();
      SetRect(&link_rect,value_x,y2,link_width + value_x + 5,y2 + link_height);
      add_link((int)terrain_types[terrain2].improve_to[improve_kind],value_x,
                         y2,4,link_rect.left,link_rect.top,link_rect.right,link_rect.bottom);
    }
    x2 = content_left + margin;
    y2 += line_height;
    text_clear();
    if (improve_kind == 0) {
      text_append_label(0x298);
    }
    else {
      text_append_label(0x29a);
    }
    text_append_colon();
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    text_clear();
    if ((char)terrain_types[terrain2].improve_turns[improve_kind] < 1) {
      text_append_label(0x29b);
    }
    else {
      _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_terrain_improve_turns_fmt),(int)(char)terrain_types[terrain2].improve_turns[improve_kind]);
    }
    draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  }
}

/* Part of PediaWindow::draw_terrain_page (004906fd): the
   right column of a base terrain: what it transforms to, the road bonus,
   and links to its two special resources. */
void PediaWindow::draw_terrain_transform_column(GameFont *font2,iptr terrain2,int stats_top,
                                                          int line_height,int right_col_x,int row_height)
{
  int measured;
  iptr terrain_index;
  char *label;
  int icon_center_y;
  i8 trade_sprite_ctx2[16];
  int link_height;
  int y2;
  u32 x2;
  int link_width;
  tagRECT link_rect;

  x2 = right_col_x;
  text_clear();
  y2 = stats_top;
  text_append_label(0x29c);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  x2 += 0x14;
  y2 += line_height;
  text_clear();
  if (terrain_types[terrain2].transform_to < '\0') {
    text_append_label(0x29b);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  }
  else {
    terrain_index = pedia_find_terrain_index((int)terrain_types[terrain2].transform_to);
    label = pedia_lookup_text(pedia_wnd.terrain_names,terrain_index);
    crt_strcpy(((i8 *)&textbuf.buf),label);
    link_width = font2->text_width((const char *)&textbuf.buf);
    link_height = font2->line_spacing();
    SetRect(&link_rect,x2,y2,x2 + link_width + 5,y2 + link_height);
    add_link((int)terrain_types[terrain2].transform_to,x2,y2,4,
                       link_rect.left,link_rect.top,link_rect.right,link_rect.bottom);
  }
  if (terrain2 != TERRAIN_OCEAN) {
    x2 = right_col_x;
    y2 += line_height;
    text_clear();
    text_append_label(0x29d);
    text_append_colon();
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    text_clear();
    x2 += 0x14;
    y2 += line_height;
    text_append_label(0x293);
    text_append_colon();
    text_append_space();
    terrain_index = (iptr)label_string(label_table.string_ids[0x2c3]);
    text_append((const char *)terrain_index);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    if (((terrain2 == TERRAIN_DESERT) || (terrain2 == TERRAIN_GRASSLAND)) || (terrain2 == TERRAIN_PLAINS)) {
      y2 += line_height;
      text_clear();
      text_append_label(0x45);
      text_append_colon();
      text_append((const char *)&str_1_4);
      draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
      measured = font2->text_width((const char *)&textbuf.buf);
      x2 = x2 + measured + 3;
      icon_center_y = y2 + row_height / 2;
      measured = ((Sprite *)((i8 *)&war_icon_sprite_block.trade_icon_sprite))->get_height();
      ((Sprite *)((i8 *)&war_icon_sprite_block.trade_icon_sprite))->draw((iptr)trade_sprite_ctx2,(iptr)&pedia_wnd.window,x2,
                 (icon_center_y - 2) - measured / 2);
    }
  }
  x2 = right_col_x;
  y2 += line_height;
  text_clear();
  text_append_label(0x29e);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  x2 += 0x14;
  y2 += line_height;
  text_clear();
  terrain_index = pedia_find_terrain_index(terrain2 + 0xb);
  label = pedia_lookup_text(pedia_wnd.terrain_names,terrain_index);
  crt_strcpy(((i8 *)&textbuf.buf),label);
  link_width = font2->text_width((const char *)&textbuf.buf);
  link_height = font2->line_spacing();
  SetRect(&link_rect,x2,y2,x2 + link_width + 5,y2 + link_height);
  add_link(terrain2 + 0xb,x2,y2,4,link_rect.left,link_rect.top,
                     link_rect.right,link_rect.bottom);
  if (terrain2 != TERRAIN_GRASSLAND) {
    text_clear();
    y2 += line_height;
    terrain_index = pedia_find_terrain_index(terrain2 + 0x16);
    label = pedia_lookup_text(pedia_wnd.terrain_names,terrain_index);
    crt_strcpy(((i8 *)&textbuf.buf),label);
    link_width = font2->text_width((const char *)&textbuf.buf);
    link_height = font2->line_spacing();
    SetRect(&link_rect,x2,y2,x2 + link_width + 5,y2 + link_height);
    add_link(terrain2 + 0x16,x2,y2,4,link_rect.left,link_rect.top,
                       link_rect.right,link_rect.bottom);
  }
}

/* 004906fd auto */
void PediaWindow::draw_terrain_page()

{
  u8 shields;
  int measured;
  iptr terrain_index;
  char *label;
  int icon_center_y;
  i8 trade_sprite_ctx[16];
  i8 shield_sprite_ctx[16];
  i8 food_sprite_ctx[16];
  i8 resource_sprite_ctx[16];
  i8 hill_sprite_ctx2[16];
  i8 forest_sprite_ctx2[16];
  i8 grass_shield_ctx[16];
  i8 mountain_sprite_ctx[16];
  i8 hill_sprite_ctx[16];
  i8 forest_sprite_ctx[16];
  i8 base_sprite_ctx[16];
  int line_height;
  iptr terrain2;
  int half_width;
  int link_height;
  iptr terrain3;
  int sprite_area_width;
  int sprite_height2;
  int stats_top;
  int y2;
  u32 x2;
  int page_width;
  int value_x;
  uint resource_variant;
  int sprite_width2;
  int margin;
  int right_col_x;
  i32 text_color2;
  int link_width;
  tagRECT link_rect;
  i32 scale_den2;
  int scale_num2;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  get_clip((RECT *)saved_clip);
  set_clip((RECT *)&content_left);
  update_controls();
  fill_background((RECT *)&content_left,2);
  font2 = (GameFont *)((i8 *)&font_size21);
  x2 = content_left;
  margin = 10;
  page_width = rect_width((RECT *)&content_left);
  half_width = page_width / 2;
  row_height = rect_height((RECT *)&content_left);
  row_height = row_height / 0xc;
  line_height = font2->font_cached_height();
  y2 = content_top + 8;
  terrain2 = pedia_terrain_ids[resize_border];
  gfx_get_scale_ratio((iptr)&scale_num2,(iptr)&scale_den2);
  gfx_set_scale_ratio(scale_num2 * 2,scale_den2);
  sprite_height2 = ((Sprite *)((i8 *)&terrain_base_sprites_block.terrain_base_sprites))->get_height();
  sprite_width2 = ((Sprite *)((i8 *)&terrain_base_sprites_block.terrain_base_sprites))->get_width();
  x2 += margin;
  y2 += (row_height / 2 - sprite_height2 / 2);
  terrain3 = terrain2 % 0xb;
  sprite_area_width = sprite_width2 * 2 + panel_layout.margin_x * 2;
  ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain3 * 0x3c))->draw((iptr)base_sprite_ctx,(iptr)&pedia_wnd.window,x2,
             y2);
  if (terrain2 < 0xb) {
    if (terrain2 == TERRAIN_FOREST) {
      ((Sprite *)((i8 *)&forest_sprites))->draw((iptr)forest_sprite_ctx,(iptr)&pedia_wnd.window,x2,y2);
    }
    else if (terrain2 == TERRAIN_HILLS) {
      ((Sprite *)((i8 *)&hill_sprites_block.hill_sprites))->draw((iptr)hill_sprite_ctx,(iptr)&pedia_wnd.window,x2,y2);
    }
    else if (terrain2 == TERRAIN_MOUNTAINS) {
      ((Sprite *)((i8 *)&mountain_sprites))->draw((iptr)mountain_sprite_ctx,(iptr)&pedia_wnd.window,x2,y2);
    }
  }
  else {
    resource_variant = (uint)(0x15 < terrain2);
    if ((terrain2 == 0xd) || (terrain2 == 0x18)) {
      ((Sprite *)((i8 *)&grassland_shield_sprite))->draw((iptr)grass_shield_ctx,(iptr)&pedia_wnd.window,x2,y2);
    }
    else {
      switch(terrain2) {
      case 0xe:
      case 0x19:
        ((Sprite *)((i8 *)&forest_sprites))->draw((iptr)forest_sprite_ctx2,(iptr)&pedia_wnd.window,x2,y2);
        break;
      case 0xf:
      case 0x1a:
        ((Sprite *)((i8 *)&hill_sprites_block.hill_sprites))->draw((iptr)hill_sprite_ctx2,(iptr)&pedia_wnd.window,x2,y2);
        break;
      case 0x10:
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17: ;
      }
      ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain3 * 0x78 + resource_variant * 0x3c))->draw((iptr)resource_sprite_ctx,
                 (iptr)&pedia_wnd.window,x2,y2);
    }
  }
  gfx_set_scale_ratio(scale_num2,scale_den2);
  text_color2 = dlg_style.list_text;
  gfx_set_text_color_index(dlg_style.list_text);
  x2 = content_left + margin;
  right_col_x = rect_width((RECT *)&content_left);
  right_col_x = right_col_x / 2;
  value_x = right_col_x - 100;
  if ((terrain2 < 0) || (10 < terrain2)) {
    x2 = x2 + sprite_area_width + 0x1e;
    y2 += line_height;
    text_clear();
    text_append_label(0x295);
    text_append_colon();
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    measured = font2->text_width((const char *)&textbuf.buf);
    x2 = x2 + measured + 10;
    text_clear();
    terrain_index = pedia_find_terrain_index(terrain3);
    label = pedia_lookup_text(pedia_wnd.terrain_names,terrain_index);
    crt_strcpy(((i8 *)&textbuf.buf),label);
    link_width = font2->text_width((const char *)&textbuf.buf);
    link_height = font2->line_spacing();
    SetRect(&link_rect,x2,y2,x2 + link_width + 5,y2 + link_height);
    add_link(terrain3,x2,y2,4,link_rect.left,link_rect.top,link_rect.right,
                       link_rect.bottom);
    y2 += ((sprite_height2 * 2 + 10) - line_height);
  }
  else {
    stats_top = y2 + sprite_height2 * 2 + 10;
    y2 = stats_top;
    text_clear();
    text_append_label(0x293);
    text_append_colon();
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    text_clear();
    _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_terrain_move_cost_fmt),(int)(char)terrain_types[terrain2].move_cost);
    draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
    x2 = content_left + margin;
    y2 += line_height;
    text_clear();
    text_append_label(0x294);
    text_append_colon();
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
    text_clear();
    if (terrain_types[terrain2].defense == 6) {
      crt_strcpy(((i8 *)&textbuf.buf),str_200);
    }
    else if (terrain_types[terrain2].defense == 3) {
      crt_strcpy(((i8 *)&textbuf.buf),((i8 *)&pedia_terrain_defense_50_str));
    }
    else if (terrain_types[terrain2].defense == 4) {
      crt_strcpy(((i8 *)&textbuf.buf),str_100);
    }
    else if (terrain_types[terrain2].defense == 5) {
      crt_strcpy(((i8 *)&textbuf.buf),str_150);
    }
    else {
      terrain_index = (iptr)label_string(label_table.string_ids[0x2c2]);
      text_append((const char *)terrain_index);
    }
    draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  }
  x2 = content_left + margin;
  y2 += line_height;
  text_clear();
  text_append_label(0x40);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_terrain_food_fmt),(int)(char)terrain_types[terrain2].food);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  measured = font2->text_width((const char *)&textbuf.buf);
  x2 = value_x + measured + 3;
  icon_center_y = y2 + row_height / 2;
  measured = ((Sprite *)((i8 *)&war_icon_sprite_block.resource_icon_sprites))->get_height();
  ((Sprite *)((i8 *)&war_icon_sprite_block.resource_icon_sprites))->draw((iptr)food_sprite_ctx,(iptr)&pedia_wnd.window,x2,
             (icon_center_y - 2) - measured / 2);
  x2 = content_left + margin;
  y2 += line_height;
  text_clear();
  text_append_label(0x296);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  if (terrain2 == TERRAIN_GRASSLAND) {
    shields = '\0';
  }
  else {
    shields = terrain_types[terrain2].shields;
  }
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_terrain_shields_fmt),(int)(char)shields);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  measured = font2->text_width((const char *)&textbuf.buf);
  x2 = value_x + measured + 3;
  icon_center_y = y2 + row_height / 2;
  measured = ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->get_height();
  ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->draw((iptr)shield_sprite_ctx,(iptr)&pedia_wnd.window,x2,
             (icon_center_y - 2) - measured / 2);
  x2 = content_left + margin;
  y2 += line_height;
  text_clear();
  text_append_label(0x45);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_terrain_trade_fmt),(int)(char)terrain_types[terrain2].trade);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  measured = font2->text_width((const char *)&textbuf.buf);
  x2 = value_x + measured + 3;
  icon_center_y = y2 + row_height / 2;
  measured = ((Sprite *)((i8 *)&war_icon_sprite_block.trade_icon_sprite))->get_height();
  ((Sprite *)((i8 *)&war_icon_sprite_block.trade_icon_sprite))->draw((iptr)trade_sprite_ctx,(iptr)&pedia_wnd.window,x2,
             (icon_center_y - 2) - measured / 2);
  if ((terrain2 < 0) || (10 < terrain2)) {
    draw_terrain_special_bonus(font2,terrain2,terrain3,y2,line_height,margin);
  }
  else {
    draw_terrain_improve_rows(font2,terrain2,y2,line_height,margin,value_x,row_height);
    draw_terrain_transform_column(font2,terrain2,stats_top,line_height,right_col_x,row_height);
  }
  update_describe_button(resize_border);
  set_clip((RECT *)saved_clip);
  wnd_blit_surface_rect((iptr)&content_left);
  return;
}

/* 00491c20 auto */
void PediaWindow::show_terrain_modal(int terrain_id)

{
  char *name_text;
  uptr terrain_idx;
  
  if (pedia_wnd.closed != 0) {
    terrain_idx = 0;
    while (((int)terrain_idx < (int)pedia_terrain_count_2 &&
           (pedia_terrain_ids[terrain_idx] != terrain_id))) {
      terrain_idx++;
    }
    if (pedia_terrain_count_2 != terrain_idx) {
      open_category(5,1);
      resize_border = terrain_idx;
      list_selected_index = terrain_idx;
      client_x = 1;
      border_width = 1;
      clear_links();
      update_controls();
      name_text = pedia_lookup_text(pedia_wnd.terrain_names,terrain_idx);
      crt_strcpy((char *)pedia_title_text_2_618,name_text);
      draw_frame();
      draw_terrain_page();
      ((DibSurface *)((i8 *)&pedia_wnd.window))->wnd_blit_surface();
      pedia_wnd.window.window.activate();
      pedia_wnd.window.window.set_modal_idle_callback(pedia_modal_idle);
      pedia_wnd.window.window.run_modal_loop();
      pedia_wnd.window.window.set_modal_idle_callback(0);
    }
  }
  return;
}

/* 00491d61 auto */
i32 __cdecl pedia_find_terrain_index(iptr terrain_id)

{
  int index;
  
  index = 0;
  while( true ) {
    if (pedia_wnd.terrain_count <= index) {
      return 0xffffffff;
    }
    if (pedia_wnd.terrain_ids[index] == terrain_id) break;
    index++;
  }
  return index;
}

/* 004a69b0 auto */
void PediaWindow::init_concept_list(iptr unused_arg)

{
  int i;
  
  if (((int)resize_border < 0) || ((int)((uptr)pedia_concept_count) <= (int)resize_border)) {
    resize_border = 0;
  }
  list_count = ((uptr)pedia_concept_count);
  list_selected_index = resize_border;
  list_top_index = resize_border;
  while (pedia_wnd.list_top % 9 != 0) {
    list_top_index = list_top_index - 1;
  }
  for (i = 0; i < (int)((uptr)pedia_concept_count); i++) {
    list_items[i] =
         pedia_concept_ids[i];
  }
  ((Scrollbar *)&list_scrollbar)->set_range(0,list_count / 9);
  ((Scrollbar *)&list_scrollbar)->set_pos(list_top_index / 9);
  if (list_count < 1) {
    list_top_index = -1;
  }
  update_controls();
  pedia_wnd.window.window.set_focus();
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* 004f3f80 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl pedia_window_global_init(void)

{
  pedia_window_static_init();
  pedia_window_reg_atexit();
  return;
}

/* 004f3f9a auto */
void __cdecl pedia_window_static_init(void)

{
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->construct();
  return;
}

/* 004f3fb4 auto */
void __cdecl pedia_window_reg_atexit(void)

{
  _atexit(pedia_window_static_dtor);
  return;
}

/* 004f3fd1 auto */
void __cdecl pedia_window_static_dtor(void)

{
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->destruct();
  return;
}

/* 004f3feb auto */
PediaWindow *PediaWindow::construct()

{
  u8 *slot;
  int i;
  i32 exc_link;
  
  this->SurfaceWindow::construct();
  button_ctor((Button *)list_describe_button);
  button_ctor((Button *)((u8 *)&list_close_button));
  button_ctor((Button *)&detail_back_button);
  button_ctor((Button *)detail_describe_button);
  button_ctor((Button *)((u8 *)&detail_close_button));
  button_ctor((Button *)text_back_button);
  button_ctor((Button *)((u8 *)&text_close_button));
  button_ctor((Button *)&concept_list_back_button);
  button_ctor((Button *)advance_list_describe_button);
  button_ctor((Button *)advance_list_tech_tree_button);
  button_ctor((Button *)advance_list_close_button);
  button_ctor((Button *)((u8 *)&advance_detail_back_button));
  button_ctor((Button *)&advance_detail_tech_tree_button);
  button_ctor((Button *)advance_detail_describe_button);
  button_ctor((Button *)((u8 *)&advance_detail_close_button));
  edit_ctor((EditControl *)((u8 *)&description_edit));
  ((Scrollbar *)&list_scrollbar)->construct();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 4);
  closed = 1;
  border_width = 0;
  resize_border = 0;
  client_x = 0;
  close_box_hotspot = (Hotspot *)0x0;
  link_list_head = (PediaLink *)0x0;
  category = 0;
  for (i = 0; i < 100; i++) {
    slot = pedia_list_init_area + i * 4 + 0x38;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x27; i++) {
    slot = improvement_list_init_area + i * 4 - 0x28;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x1c; i++) {
    slot = improvement_key_slots + i * 4 - 0x18;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x3e; i++) {
    slot = unit_key_init_slots + i * 4 + 0x10;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 7; i++) {
    slot = pedia_key_init_slots + i * 4 + 0x68;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x21; i++) {
    slot = pedia_key_init_slots + i * 4 + 0xa4;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x100; i++) {
    slot = (u8 *)&advisor_opinions + i * 4 + 0x84;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  pedia_advance_names = (PediaListNode *)0x0;
  pedia_improve_names = (PediaListNode *)0x0;
  pedia_wonder_names = (PediaListNode *)0x0;
  pedia_unit_names = (PediaListNode *)0x0;
  pedia_government_names = (PediaListNode *)0x0;
  pedia_terrain_names = (PediaListNode *)0x0;
  pedia_concept_names = (PediaListNode *)0x0;
  list_hotspot = 0;
  client_y = 0;
  return this;
}

/* 004f44a7 auto */
void PediaWindow::destruct()

{
  i32 exc_link;
  int eh_state;
  
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 4);
  eh_state = 0x11;
  free_lists();
  clear_links();
  if (close_box_hotspot != (Hotspot *)0x0) {
    if (close_box_hotspot != (Hotspot *)0x0) {
      close_box_hotspot->scalar_destruct(1);
    }
    close_box_hotspot = (Hotspot *)0x0;
  }
  if (list_hotspot != 0) {
    if ((Hotspot *)list_hotspot != (Hotspot *)0x0) {
      ((Hotspot *)list_hotspot)->scalar_destruct(1);
    }
    list_hotspot = 0;
  }
  wnd_destroy_with_surface();
  (*((char *)&eh_state + 0)) = 0x10;
  /* inlined unwind funclet 004f4673 */ { ((Scrollbar *)(((int)this) + 0x1ae0))->destruct(); }
  (*((char *)&eh_state + 0)) = 0xf;
  /* inlined unwind funclet 004f4682 */ { edit_dtor((EditControl *)(((int)this) + 0x1a98)); }
  (*((char *)&eh_state + 0)) = 0xe;
  /* inlined unwind funclet 004f4691 */ { ctrl_dtor((Button *)(((int)this) + 0x1a5c)); }
  (*((char *)&eh_state + 0)) = 0xd;
  /* inlined unwind funclet 004f46a0 */ { ctrl_dtor((Button *)(((int)this) + 0x1a20)); }
  (*((char *)&eh_state + 0)) = 0xc;
  /* inlined unwind funclet 004f46af */ { ctrl_dtor((Button *)(((int)this) + 0x19e4)); }
  (*((char *)&eh_state + 0)) = 0xb;
  /* inlined unwind funclet 004f46be */ { ctrl_dtor((Button *)(((int)this) + 0x19a8)); }
  (*((char *)&eh_state + 0)) = 10;
  /* inlined unwind funclet 004f46cd */ { ctrl_dtor((Button *)(((int)this) + 0x196c)); }
  (*((char *)&eh_state + 0)) = 9;
  /* inlined unwind funclet 004f46dc */ { ctrl_dtor((Button *)(((int)this) + 0x1930)); }
  (*((char *)&eh_state + 0)) = 8;
  /* inlined unwind funclet 004f46eb */ { ctrl_dtor((Button *)(((int)this) + 0x18f4)); }
  (*((char *)&eh_state + 0)) = 7;
  /* inlined unwind funclet 004f46fa */ { ctrl_dtor((Button *)(((int)this) + 0x18b8)); }
  (*((char *)&eh_state + 0)) = 6;
  /* inlined unwind funclet 004f4709 */ { ctrl_dtor((Button *)(((int)this) + 0x187c)); }
  (*((char *)&eh_state + 0)) = 5;
  /* inlined unwind funclet 004f4718 */ { ctrl_dtor((Button *)(((int)this) + 0x1840)); }
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 004f4727 */ { ctrl_dtor((Button *)(((int)this) + 0x1804)); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 004f4736 */ { ctrl_dtor((Button *)(((int)this) + 0x17c8)); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 004f4745 */ { ctrl_dtor((Button *)(((int)this) + 0x178c)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 004f4754 */ { ctrl_dtor((Button *)(((int)this) + 0x1750)); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 004f4763 */ { ctrl_dtor((Button *)(((int)this) + 0x1714)); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004f4772 */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return;
}

/* 004f4793 auto */
void PediaWindow::clear_links()

{
  PediaLink *next_link;
  
  while (link_list_head != (PediaLink *)0x0) {
    next_link = link_list_head->next;
    if ((void *)link_list_head != (void *)0x0) {
      ((PediaLink *)link_list_head)->scalar_destruct(1);
    }
    link_list_head = next_link;
  }
  wnd_flush_paint();
  return;
}

/* 004f4809 auto */
void PediaWindow::free_lists()

{
  PediaListNode *next_node;
  
  while (pedia_advance_names != (PediaListNode *)0x0) {
    next_node = pedia_advance_names->next;
    if (pedia_advance_names->name != (char *)0x0) {
      operator_delete(pedia_advance_names->name);
    }
    operator_delete(pedia_advance_names);
    pedia_advance_names = next_node;
  }
  while (pedia_improve_names != (PediaListNode *)0x0) {
    next_node = pedia_improve_names->next;
    if (pedia_improve_names->name != (char *)0x0) {
      operator_delete(pedia_improve_names->name);
    }
    operator_delete(pedia_improve_names);
    pedia_improve_names = next_node;
  }
  while (pedia_wonder_names != (PediaListNode *)0x0) {
    next_node = pedia_wonder_names->next;
    if (pedia_wonder_names->name != (char *)0x0) {
      operator_delete(pedia_wonder_names->name);
    }
    operator_delete(pedia_wonder_names);
    pedia_wonder_names = next_node;
  }
  while (pedia_unit_names != (PediaListNode *)0x0) {
    next_node = pedia_unit_names->next;
    if (pedia_unit_names->name != (char *)0x0) {
      operator_delete(pedia_unit_names->name);
    }
    operator_delete(pedia_unit_names);
    pedia_unit_names = next_node;
  }
  while (pedia_government_names != (PediaListNode *)0x0) {
    next_node = pedia_government_names->next;
    if (pedia_government_names->name != (char *)0x0) {
      operator_delete(pedia_government_names->name);
    }
    operator_delete(pedia_government_names);
    pedia_government_names = next_node;
  }
  while (pedia_terrain_names != (PediaListNode *)0x0) {
    next_node = pedia_terrain_names->next;
    if (pedia_terrain_names->name != (char *)0x0) {
      operator_delete(pedia_terrain_names->name);
    }
    operator_delete(pedia_terrain_names);
    pedia_terrain_names = next_node;
  }
  while (pedia_concept_names != (PediaListNode *)0x0) {
    next_node = pedia_concept_names->next;
    if (pedia_concept_names->name != (char *)0x0) {
      operator_delete(pedia_concept_names->name);
    }
    operator_delete(pedia_concept_names);
    pedia_concept_names = next_node;
  }
  return;
}

/* Part of PediaWindow::create (004f4b9f): the buttons of the item
   lists: Describe and Close, and Back (concept list). */
void PediaWindow::create_list_buttons(int left_x,int yTop,int btn_height,int avail_width)
{
  int col_width;
  iptr label;
  u8 *parent_2;
  u8 *parent_3;
  u8 *parent_4;
  tagRECT rect;

  col_width = avail_width / 2;
  SetRect(&rect,left_x,yTop,col_width + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_2 = (u8 *)0x0;
  }
  else {
    parent_2 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x27]);
  ((Button *)list_describe_button)->create((iptr)parent_2,0xfa1,(iptr)&rect,(const char *)label);
  ((Button *)list_describe_button)->set_callback(pedia_toggle_describe);
  SetRect(&rect,col_width + left_x,yTop,col_width * 2 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_3 = (u8 *)0x0;
  }
  else {
    parent_3 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)&list_close_button)->create((iptr)parent_3,0xfa2,(iptr)&rect,(const char *)label);
  ((Button *)&list_close_button)->set_default_button();
  ((Button *)&list_close_button)->set_cancel_button();
  ((Button *)&list_close_button)->set_callback((CtrlNotifyFn)pedia_on_close);
  SetRect(&rect,left_x,yTop,avail_width / 2 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_4 = (u8 *)0x0;
  }
  else {
    parent_4 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x356]);
  ((Button *)((u8 *)&concept_list_back_button))->create((iptr)parent_4,0xfb2,(iptr)&rect,(const char *)label);
  ((Button *)((u8 *)&concept_list_back_button))->set_callback((CtrlNotifyFn)pedia_on_back);
}

/* Part of PediaWindow::create (004f4b9f): the buttons of the
   advance list (Describe, Tech tree, Close) and of an advance's page (Back,
   Tech tree, Describe, Close). */
void PediaWindow::create_advance_buttons(int left_x,int yTop,int btn_height,int avail_width)
{
  int col_width;
  iptr label;
  u8 *parent_5;
  u8 *parent_6;
  u8 *parent_7;
  u8 *parent_8;
  u8 *parent_9;
  u8 *parent_10;
  u8 *parent_11;
  tagRECT rect;

  col_width = avail_width / 3;
  SetRect(&rect,left_x,yTop,col_width + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_5 = (u8 *)0x0;
  }
  else {
    parent_5 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x27]);
  ((Button *)advance_list_describe_button)->create((iptr)parent_5,0xfab,(iptr)&rect,(const char *)label);
  ((Button *)advance_list_describe_button)->set_callback(pedia_toggle_describe);
  SetRect(&rect,col_width + left_x,yTop,col_width * 2 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_6 = (u8 *)0x0;
  }
  else {
    parent_6 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x360]);
  ((Button *)advance_list_tech_tree_button)->create((iptr)parent_6,0xfac,(iptr)&rect,(const char *)label);
  ((Button *)advance_list_tech_tree_button)->set_callback((CtrlNotifyFn)pedia_show_tech_tree);
  SetRect(&rect,col_width * 2 + left_x,yTop,col_width * 3 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_7 = (u8 *)0x0;
  }
  else {
    parent_7 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)advance_list_close_button)->create((iptr)parent_7,0xfad,(iptr)&rect,(const char *)label);
  ((Button *)advance_list_close_button)->set_default_button();
  ((Button *)advance_list_close_button)->set_cancel_button();
  ((Button *)advance_list_close_button)->set_callback((CtrlNotifyFn)pedia_on_close);
  col_width = (int)(avail_width + (avail_width >> 0x1f & 3U)) >> 2;
  SetRect(&rect,left_x,yTop,col_width + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_8 = (u8 *)0x0;
  }
  else {
    parent_8 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x356]);
  ((Button *)((u8 *)&advance_detail_back_button))->create((iptr)parent_8,0xfae,(iptr)&rect,(const char *)label);
  ((Button *)((u8 *)&advance_detail_back_button))->set_callback((CtrlNotifyFn)pedia_on_back);
  SetRect(&rect,col_width + left_x,yTop,col_width * 2 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_9 = (u8 *)0x0;
  }
  else {
    parent_9 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x360]);
  ((Button *)((u8 *)&advance_detail_tech_tree_button))->create((iptr)parent_9,0xfaf,(iptr)&rect,(const char *)label);
  ((Button *)((u8 *)&advance_detail_tech_tree_button))->set_callback((CtrlNotifyFn)pedia_show_tech_tree);
  SetRect(&rect,col_width * 2 + left_x,yTop,col_width * 3 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_10 = (u8 *)0x0;
  }
  else {
    parent_10 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x2c1]);
  ((Button *)advance_detail_describe_button)->create((iptr)parent_10,0xfb0,(iptr)&rect,(const char *)label);
  ((Button *)advance_detail_describe_button)->set_callback(pedia_toggle_describe);
  SetRect(&rect,col_width * 3 + left_x,yTop,col_width * 4 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_11 = (u8 *)0x0;
  }
  else {
    parent_11 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)((u8 *)&advance_detail_close_button))->create((iptr)parent_11,0xfb1,(iptr)&rect,(const char *)label);
  ((Button *)((u8 *)&advance_detail_close_button))->set_default_button();
  ((Button *)((u8 *)&advance_detail_close_button))->set_cancel_button();
  ((Button *)((u8 *)&advance_detail_close_button))->set_callback((CtrlNotifyFn)pedia_on_close);
}

/* Part of PediaWindow::create (004f4b9f): the buttons of the other
   pages (Back, Describe, Close) and of the description text (Back, Close). */
void PediaWindow::create_detail_buttons(int left_x,int yTop,int btn_height,int avail_width)
{
  int col_width;
  iptr label;
  u8 *parent_12;
  u8 *parent_13;
  u8 *parent_14;
  u8 *parent_15;
  u8 *parent_16;
  tagRECT rect;

  col_width = avail_width / 3;
  SetRect(&rect,left_x,yTop,col_width + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_12 = (u8 *)0x0;
  }
  else {
    parent_12 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x356]);
  ((Button *)&detail_back_button)->create((iptr)parent_12,0xfa3,(iptr)&rect,(const char *)label);
  ((Button *)&detail_back_button)->set_callback((CtrlNotifyFn)pedia_on_back);
  SetRect(&rect,col_width + left_x,yTop,col_width * 2 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_13 = (u8 *)0x0;
  }
  else {
    parent_13 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x2c1]);
  ((Button *)detail_describe_button)->create((iptr)parent_13,0xfa4,(iptr)&rect,(const char *)label);
  ((Button *)detail_describe_button)->set_callback(pedia_toggle_describe);
  SetRect(&rect,col_width * 2 + left_x,yTop,col_width * 3 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_14 = (u8 *)0x0;
  }
  else {
    parent_14 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)&detail_close_button)->create((iptr)parent_14,0xfa5,(iptr)&rect,(const char *)label);
  ((Button *)&detail_close_button)->set_callback((CtrlNotifyFn)pedia_on_close);
  avail_width = avail_width / 2;
  SetRect(&rect,left_x,yTop,avail_width + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_15 = (u8 *)0x0;
  }
  else {
    parent_15 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x356]);
  ((Button *)text_back_button)->create((iptr)parent_15,0xfa6,(iptr)&rect,(const char *)label);
  ((Button *)text_back_button)->set_callback((CtrlNotifyFn)pedia_on_back);
  SetRect(&rect,avail_width + left_x,yTop,avail_width * 2 + left_x,btn_height + yTop);
  if (this == (PediaWindow *)0x0) {
    parent_16 = (u8 *)0x0;
  }
  else {
    parent_16 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)((u8 *)&text_close_button))->create((iptr)parent_16,0xfa7,(iptr)&rect,(const char *)label);
  ((Button *)((u8 *)&text_close_button))->set_callback((CtrlNotifyFn)pedia_on_close);
}

/* 004f4b9f auto */
void PediaWindow::create()

{
  int left_x;
  int col_width;
  u32 panel_h;
  Hotspot *this_00;
  int avail_width;
  int yTop;
  u8 *scroll_parent;
  u8 *edit_parent;
  u8 *close_parent;
  Hotspot *close_hotspot;
  tagRECT rect;
  int btn_height;
  i32 exc_link;
  
  free_lists();
  clear_links();
  client_y = 0;
  if (close_box_hotspot != (Hotspot *)0x0) {
    if (close_box_hotspot != (Hotspot *)0x0) {
      close_box_hotspot->scalar_destruct(1);
    }
    close_box_hotspot = (Hotspot *)0x0;
  }
  panel_width = 0x280;
  panel_height = 400;
  left_x = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  header_height = left_x + panel_layout.bevel_width * 2 + panel_layout.margin_y * 2;
  SetRect((LPRECT)pedia_frame_rect,0,0,panel_width,panel_height);
  panel_h = panel_height;
  left_x = header_height + panel_layout.margin_y;
  col_width = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  SetRect((LPRECT)&content_left,0,0,panel_width + (panel_layout.padding_x + panel_layout.margin_x) * -2,
          (panel_h - left_x) + ((panel_layout.margin_y * -3 + panel_layout.padding_y * -2) - col_width) + panel_layout.bevel_width * -2);
  OffsetRect((LPRECT)&content_left,panel_layout.padding_x + panel_layout.margin_x,header_height + panel_layout.margin_y);
  left_x = main_frame_wnd_block.main_frame_wnd.client_width();
  col_width = main_frame_wnd_block.main_frame_wnd.client_height();
  crt_strcpy((char *)pedia_title_text_2_618,((i8 *)&pedia_window_empty_title_str));
  wnd_create_with_surface(0,0xe02,left_x / 2 - (int)panel_width / 2,
             col_width / 2 - (int)panel_height / 2,panel_width,panel_height,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd)
  ;
  window.wnd_handlers.set_close_handler(pedia_close);
  set_redraw_callback(pedia_redraw_page);
  ((WndBase *)&window)->set_caption_height(header_height);
  window.wnd_handlers.set_keydown_handler(pedia_list_on_key);
  window.set_keyboard_nav(0);
  left_x = panel_layout.padding_x;
  close_box_sprite = (u32)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite);
  has_close_box = 1;
  panel_h = ((Sprite *)close_box_sprite)->width();
  close_box_width = panel_h;
  panel_h = ((Sprite *)close_box_sprite)->height();
  close_box_height = panel_h;
  btn_height = (int)(close_box_height * header_height) / 0x18;
  col_width = ((int)header_height >> 1) - (btn_height >> 1);
  SetRect(&rect,left_x,col_width,(int)(close_box_width * header_height) / 0x18 + left_x,
          btn_height + col_width);
  this_00 = (Hotspot *)operator_new(0x40);
  if (this_00 == (Hotspot *)0x0) {
    close_hotspot = 0;
  }
  else {
    close_hotspot = hotspot_ctor(this_00);
  }
  close_box_hotspot = close_hotspot;
  if (this == (PediaWindow *)0x0) {
    close_parent = (u8 *)0x0;
  }
  else {
    close_parent = (u8 *)&window;
  }
  close_box_hotspot->create((iptr)close_parent,4000,(iptr)&rect);
  close_box_hotspot->set_click_callback((CtrlNotifyFn)pedia_on_close);
  left_x = panel_layout.bevel_width + panel_layout.margin_x + 2;
  panel_h = panel_height;
  col_width = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  yTop = panel_h - (col_width + (panel_layout.margin_y + panel_layout.padding_y) * 2);
  avail_width = panel_width - (panel_layout.margin_x * 2 + panel_layout.bevel_width * 2 + 5);
  btn_height = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  btn_height += panel_layout.padding_y * 2;
  create_list_buttons(left_x,yTop,btn_height,avail_width);
  create_advance_buttons(left_x,yTop,btn_height,avail_width);
  create_detail_buttons(left_x,yTop,btn_height,avail_width);
  ui_set_button_font((iptr)&font_size24);
  clear_links();
  ui_set_default_font((iptr)&font_size21);
  if (this == (PediaWindow *)0x0) {
    edit_parent = (u8 *)0x0;
  }
  else {
    edit_parent = (u8 *)&window;
  }
  ((EditControl *)((u8 *)&description_edit))->create_styled((iptr)edit_parent,0xfaa,(iptr)&content_left,
             (const char *)&str_empty_143,0x122,0);
  rect.left = content_left;
  rect.top = content_top;
  rect.right = content_right;
  panel_h = content_bottom;
  rect.bottom = panel_h;
  left_x = GetSystemMetrics(3);
  rect.top = panel_h - left_x;
  if (this == (PediaWindow *)0x0) {
    scroll_parent = (u8 *)0x0;
  }
  else {
    scroll_parent = (u8 *)&window;
  }
  ((Scrollbar *)((u8 *)&list_scrollbar))->create((iptr)scroll_parent,0xfa8,(iptr)&rect,0);
  ((Scrollbar *)((u8 *)&list_scrollbar))->set_callback(pedia_list_on_scroll);
  ((Scrollbar *)((u8 *)&list_scrollbar))->set_track_callback(pedia_list_on_scroll);
  ((Scrollbar *)((u8 *)&list_scrollbar))->set_page_size(2);
  load_indexes();
  return;
}

/* 004f5b24 auto */
void __cdecl pedia_on_close(void)

{
  if (pedia_wnd.modal_active == 1) {
    pedia_wnd.window.window.end_modal_loop();
    pedia_wnd.modal_active = 0;
  }
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->clear_links();
  pedia_wnd.history_count = 0;
  pedia_close();
  return;
}

/* 004f5b6f auto */
void __cdecl pedia_on_back(void)

{
  if (pedia_wnd.history_count != 0) {
    do {
      pedia_wnd.history_count--;
      if (((pedia_wnd.history_count == 0) || (pedia_wnd.history_categories[pedia_wnd.history_count] != pedia_wnd.category)) ||
         (pedia_wnd.history_page_modes[pedia_wnd.history_count] != pedia_wnd.page_mode)) break;
    } while (pedia_wnd.history_topics[pedia_wnd.history_count] == pedia_wnd.topic);
    if (pedia_wnd.history_count != 0) {
      pedia.going_back = 1;
      pedia.back_topic = pedia_wnd.history_topics[pedia_wnd.history_count];
      pedia_wnd.popped_page_mode = pedia_wnd.history_page_modes[pedia_wnd.history_count];
      pedia_wnd.popped_category = pedia_wnd.history_categories[pedia_wnd.history_count];
    }
    if (pedia_wnd.history_page_modes[pedia_wnd.history_count] != 0) {
      pedia_wnd.category = pedia_wnd.history_categories[pedia_wnd.history_count];
      pedia_wnd.page_mode = pedia_wnd.history_page_modes[pedia_wnd.history_count] - 1;
      pedia_wnd.topic = pedia_wnd.history_topics[pedia_wnd.history_count];
      pedia_wnd.selected_item = pedia_wnd.topic;
      pedia_toggle_describe(1);
      return;
    }
    pedia_wnd.category = pedia_wnd.history_categories[pedia_wnd.history_count];
    pedia_wnd.page_mode = 1;
    pedia_wnd.topic = pedia_wnd.history_topics[pedia_wnd.history_count];
    pedia_wnd.selected_item = pedia_wnd.topic;
  }
  if (pedia_wnd.page_mode == 0) {
    if (pedia_wnd.modal_active == 1) {
      pedia_wnd.window.window.end_modal_loop();
      pedia_wnd.modal_active = 0;
    }
    pedia_close();
  }
  else {
    if (pedia_wnd.page_mode == 1) {
      pedia_wnd.page_mode = 0;
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->update_controls();
      pedia_redraw_page();
      pedia_wnd.selected_item = pedia_wnd.topic;
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
    }
    else if ((pedia_wnd.category == 6) || (pedia_wnd.category == 7)) {
      pedia_wnd.page_mode = 0;
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->update_controls();
      pedia_redraw_page();
      pedia_wnd.selected_item = pedia_wnd.topic;
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
    }
    else {
      pedia_wnd.page_mode = 0;
      pedia_toggle_describe(1);
    }
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->history_push();
  }
  return;
}

/* 004f5dd1 auto */
i32 __cdecl pedia_close(void)

{
  pedia_wnd.closed = 1;
  pedia_wnd.history_count = 0;
  if (pedia_wnd.close_hotspot != (Hotspot *)0x0) {
    if (pedia_wnd.close_hotspot != (Hotspot *)0x0) {
      pedia_wnd.close_hotspot->scalar_destruct(1);
    }
    pedia_wnd.close_hotspot = (Hotspot *)0x0;
  }
  pedia_wnd.window.window.hide();
  main_frame_wnd_block.main_frame_wnd.focus_and_raise();
  ui_flush_input();
  return 0;
}

/* 004f5e52 auto */
void __cdecl pedia_redraw_page(void)

{
  if (pedia_wnd.closed == 0) {
    if (pedia_wnd.page_mode == 0) {
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->set_category_title(pedia_wnd.category);
    }
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->clear_links();
    gfx_set_draw_target((iptr)&pedia_wnd.window);
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_category_body(pedia_wnd.category,pedia_wnd.page_mode);
    ((DibSurface *)((i8 *)&pedia_wnd.window))->wnd_blit_surface();
  }
  return;
}

/* 004f5ed2 auto */
void __cdecl pedia_show_tech_tree(void)

{
  pedia_wnd.page_mode = 0;
  pedia_wnd.category = 8;
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->update_controls();
  pedia_redraw_page();
  pedia_wnd.selected_item = pedia_wnd.topic;
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->history_push();
  return;
}

/* 004f5f23 auto */
void __cdecl pedia_toggle_describe(i32 unused_arg)

{
  char *title;
  uptr item;
  
  item = (*(uptr *)&pedia_wnd.selected_item);
  if (pedia_wnd.page_mode == 0) {
    (*(uptr *)&pedia_wnd.topic) = (*(uptr *)&pedia_wnd.selected_item);
    pedia_wnd.page_mode = 1;
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->clear_links();
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->update_controls();
    switch(pedia_wnd.category) {
    case 1:
    case 8:
      title = pedia_lookup_text(pedia_wnd.advance_names,item);
      crt_strcpy(((i8 *)&pedia_wnd.title_text),title);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_advance_page();
      break;
    case 2:
      title = pedia_lookup_text(pedia_wnd.improvement_names,item);
      crt_strcpy(((i8 *)&pedia_wnd.title_text),title);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_improvement_page();
      break;
    case 3:
      title = pedia_lookup_text(pedia_wnd.wonder_names,item);
      crt_strcpy(((i8 *)&pedia_wnd.title_text),title);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_improvement();
      break;
    case 4:
      title = pedia_lookup_text(pedia_wnd.unit_names,item);
      crt_strcpy(((i8 *)&pedia_wnd.title_text),title);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_unit_page();
      break;
    case 5:
      title = pedia_lookup_text(pedia_wnd.terrain_names,item);
      crt_strcpy(((i8 *)&pedia_wnd.title_text),title);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_terrain_page();
      break;
    case 6:
      pedia_wnd.page_mode = 2;
      title = pedia_lookup_text(pedia_wnd.government_names,item);
      crt_strcpy(((i8 *)&pedia_wnd.title_text),title);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->fill_background((RECT *)&pedia_wnd.describe_rect,1);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->load_description(item);
      break;
    case 7:
      pedia_wnd.page_mode = 2;
      title = pedia_lookup_text(pedia_wnd.concept_names,item);
      crt_strcpy(((i8 *)&pedia_wnd.title_text),title);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->fill_background((RECT *)&pedia_wnd.describe_rect,1);
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->load_description(item);
      break;
    default:
      text_append((const char *)&str_this_should_not_happen);
    }
    ((DibSurface *)((i8 *)&pedia_wnd.window))->wnd_blit_surface();
  }
  else if (pedia_wnd.page_mode == 1) {
    pedia_wnd.page_mode = 2;
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->clear_links();
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->update_controls();
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_frame();
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->fill_background((RECT *)&pedia_wnd.describe_rect,0);
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->load_description(item);
  }
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->history_push();
  return;
}

/* 004f6244 auto */
void PediaWindow::draw_frame()

{
  int tmp_a;
  int tmp_b;
  i8 sprite_ctx[16];
  int y2;
  int x2;
  i8 *font2;
  int i;
  tagRECT rect;
  
  reset_clip();
  crt_memcpy(&rect,pedia_frame_rect,0x10);
  for (i = 0; i < panel_layout.bevel_width; i++) {
    gfx_draw_bevel_rect((iptr)this,&rect,(uint)dlg_style.bevel_light_color,(uint)dlg_style.bevel_dark_color);
    rect_deflate((iptr)&rect,1,1);
  }
  fill_background(&rect,1);
  rect_deflate((iptr)&rect,panel_layout.padding_x + panel_layout.bevel_width * -2,0);
  rect.top = rect.top + header_height + panel_layout.bevel_width * -2;
  tmp_a = rect_height((RECT *)pedia_frame_rect);
  tmp_b = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  rect.bottom = tmp_a + ((panel_layout.margin_y * -3 + panel_layout.padding_y * -2) - tmp_b);
  for (i = 0; i < panel_layout.bevel_width; i++) {
    gfx_draw_bevel_rect((iptr)this,&rect,(uint)dlg_style.bevel_dark_color,(uint)dlg_style.bevel_light_color);
    rect_deflate((iptr)&rect,1,1);
  }
  fill_background(&rect,2);
  tmp_a = panel_layout.padding_x;
  x2 = panel_layout.padding_x;
  panel_width = (int)((int)close_box_width * header_height) / 0x18;
  panel_height = (int)((int)close_box_height * header_height) / 0x18;
  y2 = ((int)header_height >> 1) - ((int)panel_height >> 1);
  SetRect(&rect,tmp_a,y2,panel_width + tmp_a,panel_height + y2);
  gfx_set_scale_ratio(header_height,0x18);
  ((Sprite *)close_box_sprite)->draw((iptr)sprite_ctx,(iptr)this,x2,y2
            );
  gfx_reset_zoom_scale();
  x2 += panel_width;
  font2 = ((i8 *)&font_size30);
  tmp_a = (frame_rect_right - x2) - panel_layout.padding_x;
  tmp_b = ((GameFont *)((i8 *)&font_size30))->text_width((const char *)pedia_title_text_2);
  x2 += (tmp_a / 2 - (tmp_b >> 1));
  y2 = panel_layout.bevel_width + panel_layout.margin_y;
  gfx_set_text_color_index(10);
  draw_text((iptr)font2,(const char *)pedia_title_text_2,x2 + 2,
             y2 + 1,5);
  gfx_set_text_color_index(0x1a);
  draw_text((iptr)font2,(const char *)pedia_title_text_2,x2 + 1,y2
             ,5);
  draw_text((iptr)font2,(const char *)pedia_title_text_2,x2,y2,5);
  set_clip((RECT *)&content_left);
  return;
}

/* 004f6564 auto */
void PediaWindow::fill_background(RECT *rect,int style)

{
  i32 height;
  i32 width;
  i32 offset_x;
  i32 offset_y;
  
  if ((dlg_style.frame_tile_surface == 0) || (style != 1)) {
    if ((dlg_style.client_tile_surface == 0) || (style != 2)) {
      gfx_fill_rect((iptr)this,rect,(uint)dlg_style.bg_color);
    }
    else {
      offset_y = 0;
      offset_x = 0;
      height = rect_height(rect);
      width = rect_width(rect);
      gfx_tile_fill((iptr)this,dlg_style.client_tile_surface,rect->left,rect->top
                         ,width,height,offset_x,offset_y);
    }
  }
  else {
    offset_y = 0;
    offset_x = 0;
    height = rect_height(rect);
    width = rect_width(rect);
    gfx_tile_fill((iptr)this,dlg_style.frame_tile_surface,rect->left,rect->top,
                       width,height,offset_x,offset_y);
  }
  return;
}

/* 004f6646 auto */
void PediaWindow::redraw()

{
  u32 page_mode;
  
  page_mode = border_width;
  if (page_mode == 0) {
    if (category != 8) {
      draw_item_list();
    }
  }
  else if ((page_mode != 1) && (page_mode != 2)) {
    pedia_close();
  }
  return;
}

/* Part of PediaWindow::draw_item_list (004f66c6): a terrain
   row's picture (base terrain with its forest, hills, mountains or
   resource), its background filled from text_x_off. */
void PediaWindow::draw_list_terrain_icon(iptr advance,uint is_selected,int col_base,int row,int col_x,
                                                   int row_y,tagRECT &row_rect,int &text_x_off)
{
  int dim_a;
  i32 terrain_fill_color;
  i8 resource_ctx[16];
  i8 hill_ctx2[16];
  i8 forest_ctx2[16];
  i8 grass_ctx[16];
  i8 mountain_ctx[16];
  i8 hill_ctx[16];
  i8 forest_ctx[16];
  i8 base_ctx[16];
  int base_terrain;
  int icon_x;
  uint resource_variant;

  base_terrain = advance % 0xb;
  if ((((u32)list_top_index) + col_base + row & 1) == 0) {
    icon_x = panel_layout.margin_x;
  }
  else {
    dim_a = ((Sprite *)((i8 *)&forest_sprites))->width();
    icon_x = panel_layout.margin_x + dim_a / 2;
  }
  dim_a = ((Sprite *)((i8 *)&forest_sprites))->width();
  text_x_off = panel_layout.margin_x + (dim_a * 3) / 2 + 2;
  row_rect.left += text_x_off;
  if (is_selected == 0) {
    terrain_fill_color = dlg_style.list_bg;
  }
  else {
    terrain_fill_color = dlg_style.list_sel_bg;
  }
  fill_rect((iptr)&row_rect,terrain_fill_color);
  ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + base_terrain * 0x3c))->draw((iptr)base_ctx,(iptr)&pedia_wnd.window,
             icon_x + col_x,row_y);
  if (advance < 0xb) {
    if (advance == TERRAIN_FOREST) {
      ((Sprite *)((i8 *)&forest_sprites))->draw((iptr)forest_ctx,(iptr)&pedia_wnd.window,icon_x + col_x,
                 row_y);
    }
    else if (advance == TERRAIN_HILLS) {
      ((Sprite *)((i8 *)&hill_sprites_block.hill_sprites))->draw((iptr)hill_ctx,(iptr)&pedia_wnd.window,icon_x + col_x,
                 row_y);
    }
    else if (advance == TERRAIN_MOUNTAINS) {
      ((Sprite *)((i8 *)&mountain_sprites))->draw((iptr)mountain_ctx,(iptr)&pedia_wnd.window,icon_x + col_x,
                 row_y);
    }
  }
  else {
    resource_variant = (uint)(0x15 < advance);
    if ((advance == 0xd) || (advance == 0x18)) {
      ((Sprite *)((i8 *)&grassland_shield_sprite))->draw((iptr)grass_ctx,(iptr)&pedia_wnd.window,icon_x + col_x,
                 row_y);
    }
    else {
      switch(advance) {
      case 0xe:
      case 0x19:
        ((Sprite *)((i8 *)&forest_sprites))->draw((iptr)forest_ctx2,(iptr)&pedia_wnd.window,icon_x + col_x,
                   row_y);
        break;
      case 0xf:
      case 0x1a:
        ((Sprite *)((i8 *)&hill_sprites_block.hill_sprites))->draw((iptr)hill_ctx2,(iptr)&pedia_wnd.window,icon_x + col_x,
                   row_y);
        break;
      case 0x10:
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17: ;
      }
      ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + base_terrain * 0x78 + resource_variant * 0x3c))->draw(
                 (iptr)resource_ctx,(iptr)&pedia_wnd.window,icon_x + col_x,row_y);
    }
  }
}

/* Part of PediaWindow::draw_item_list (004f66c6): a row's icon by icon_mode
   (3: advance or improvement, 0: unit, 2: shield) and its background.
   icon_sprite: the caller's, changed by mode 2 and kept for the next row. */
void PediaWindow::draw_list_icon(int icon_mode,uint is_selected,int col_base,int row,int col_x,int row_y,
                                           int row_height,Sprite *&icon_sprite,tagRECT &row_rect,int &text_x_off)
{
  int dim_a;
  int dim_b;
  int dim_c;
  i32 fill_color_e;
  i32 fill_color_d;
  int shield_x;
  i32 fill_color_c;
  int icon_x2;
  i32 fill_color_b;
  i32 fill_color_a;
  i8 shield_ctx[16];
  i8 icon_ctx2[16];
  i8 icon_ctx[16];
  int icon_x;

  if (icon_mode == 3) {
    dim_a = icon_sprite->width();
    text_x_off = (dim_a * 3) / 2 + panel_layout.margin_x * 2;
    row_rect.left += text_x_off;
    if (is_selected == 0) {
      fill_color_a = dlg_style.list_bg;
    }
    else {
      fill_color_a = dlg_style.list_sel_bg;
    }
    fill_rect((iptr)&row_rect,fill_color_a);
    icon_sprite->draw((iptr)icon_ctx,(iptr)&pedia_wnd.window,panel_layout.margin_x + col_x,row_rect.top + 1);
  }
  else if (icon_mode == 0) {
    dim_a = icon_sprite->width();
    text_x_off = panel_layout.margin_x * 3 + dim_a * 2;
    row_rect.left += text_x_off;
    if (is_selected == 0) {
      fill_color_b = dlg_style.list_bg;
    }
    else {
      fill_color_b = dlg_style.list_sel_bg;
    }
    fill_rect((iptr)&row_rect,fill_color_b);
    icon_x = icon_sprite->width();
    icon_x += panel_layout.margin_x * 2;
    icon_x2 = icon_x;
    if ((((u32)list_top_index) + col_base + row & 1) == 0) {
      icon_x2 = panel_layout.margin_x;
    }
    dim_b = row_y + row_height / 2;
    dim_a = icon_sprite->get_height();
    icon_sprite->draw((iptr)icon_ctx2,(iptr)&pedia_wnd.window,col_x + icon_x2,dim_b - dim_a / 2);
  }
  else if (icon_mode == 2) {
    dim_a = icon_sprite->width();
    text_x_off = panel_layout.margin_x * 3 + dim_a * 2;
    row_rect.left += text_x_off;
    if (is_selected == 0) {
      fill_color_c = dlg_style.list_bg;
    }
    else {
      fill_color_c = dlg_style.list_sel_bg;
    }
    fill_rect((iptr)&row_rect,fill_color_c);
    dim_a = icon_sprite->width();
    dim_b = icon_sprite->width();
    dim_c = icon_sprite->width();
    icon_x = ((int)(dim_a + (dim_a >> 0x1f & 3U)) >> 2) + dim_b / 6 + dim_c +
               panel_layout.margin_x * 2;
    icon_sprite = (Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite);
    shield_x = icon_x;
    if ((((u32)list_top_index) + col_base + row & 1) == 0) {
      shield_x = panel_layout.margin_x;
    }
    dim_b = row_y + row_height / 2;
    dim_a = ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->get_height();
    icon_sprite->draw((iptr)shield_ctx,(iptr)&pedia_wnd.window,col_x + shield_x,dim_b - dim_a / 2);
  }
  else if (icon_mode == -1) {
    dim_a = icon_sprite->width();
    text_x_off = panel_layout.margin_x * 3 + dim_a * 2;
    row_rect.left += text_x_off;
    if (is_selected == 0) {
      fill_color_d = dlg_style.list_bg;
    }
    else {
      fill_color_d = dlg_style.list_sel_bg;
    }
    fill_rect((iptr)&row_rect,fill_color_d);
  }
  else if (icon_mode != -5) {
    text_x_off = panel_layout.margin_x * 5 + 2;
    row_rect.left += text_x_off;
    if (is_selected == 0) {
      fill_color_e = dlg_style.list_bg;
    }
    else {
      fill_color_e = dlg_style.list_sel_bg;
    }
    fill_rect((iptr)&row_rect,fill_color_e);
  }
}

/* Part of PediaWindow::draw_item_list (004f66c6): the name of item number advance of
   the category's list into the text buffer. */
void PediaWindow::copy_list_item_name(iptr advance)
{
  char *name_text;

  switch(category) {
  case 1:
    name_text = pedia_lookup_text(pedia_wnd.advance_names,advance);
    crt_strcpy(((i8 *)&textbuf.buf),name_text);
    break;
  case 2:
    name_text = pedia_lookup_text(pedia_wnd.improvement_names,advance);
    crt_strcpy(((i8 *)&textbuf.buf),name_text);
    break;
  case 3:
    name_text = pedia_lookup_text(pedia_wnd.wonder_names,advance);
    crt_strcpy(((i8 *)&textbuf.buf),name_text);
    break;
  case 4:
    name_text = pedia_lookup_text(pedia_wnd.unit_names,advance);
    crt_strcpy(((i8 *)&textbuf.buf),name_text);
    break;
  case 5:
    name_text = pedia_lookup_text(pedia_wnd.terrain_names,advance);
    crt_strcpy(((i8 *)&textbuf.buf),name_text);
    break;
  case 6:
    name_text = pedia_lookup_text(pedia_wnd.government_names,advance);
    crt_strcpy(((i8 *)&textbuf.buf),name_text);
    break;
  case 7:
    name_text = pedia_lookup_text(pedia_wnd.concept_names,advance);
    crt_strcpy(((i8 *)&textbuf.buf),name_text);
    break;
  default:
    text_append((const char *)&str_this_should_never_be_seen);
  }
}

/* 004f66c6 auto */
void PediaWindow::draw_item_list()

{
  int dim_a;
  int dim_b;
  int text_y_off;
  iptr advance;
  int col_width;
  i32 scale_num2;
  int col;
  int col_base;
  int row;
  int row_y;
  i32 scale_den2;
  int col_x;
  int list_width2;
  int shadow_color2;
  int text_x_off;
  int icon_mode;
  Sprite *icon_sprite;
  int text_color2;
  uint is_selected;
  tagRECT row_rect;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  icon_mode = 0;
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->history_push();
  get_clip((RECT *)saved_clip);
  set_clip((RECT *)(iptr)((u8 *)&list_rect));
  fill_rect((iptr)((u8 *)&list_rect),dlg_style.list_bg);
  font2 = (GameFont *)((i8 *)&font_size21);
  col_x = (int)list_rect;
  list_width2 = rect_width((RECT *)(iptr)((u8 *)&list_rect));
  col_width = list_width2 / 2;
  row_height = rect_height((RECT *)(iptr)((u8 *)&list_rect));
  row_height = row_height / 9;
  dim_a = row_height / 2;
  dim_b = font2->font_cached_height();
  text_y_off = dim_a - dim_b / 2;
  col = 0;
  do {
    if (1 < col) {
      set_clip((RECT *)saved_clip);
      ((Control *)list_hotspot)->show_and_redraw();
      return;
    }
    col_base = col * 9;
    for (row = 0; row < 9; row++) {
      row_y = (int)list_rect_top + row_height * row;
      SetRect(&row_rect,col_x,row_y,col_width + col_x - 2,row_height + row_y);
      if ((((u32)list_top_index) != 0xffffffff) &&
         ((int)(((u32)list_top_index) + col_base + row) < list_count)) {
        advance = list_items[(((u32)list_top_index) + col_base + row)];
        is_selected = (uint)(((u32)list_top_index) + col_base + row == pedia_wnd.selected_item);
        if (is_selected == 0) {
          text_color2 = dlg_style.list_text;
          shadow_color2 = dlg_style.list_shadow;
        }
        else {
          text_color2 = dlg_style.sel_text_color;
          shadow_color2 = dlg_style.sel_text_shadow_color;
        }
        gfx_get_scale_ratio((iptr)&scale_num2,(iptr)&scale_den2);
        switch(category) {
        case 1:
          icon_sprite = (Sprite *)
                     (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                     (char)advances[advance].epoch * 0x3c +
                     (char)advances[advance].category * 0xf0);
          gfx_set_scale_ratio(3,2);
          icon_mode = 3;
          break;
        case 2:
          icon_sprite = (Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + advance * 0x3c);
          gfx_set_scale_ratio(3,2);
          icon_mode = 3;
          break;
        case 3:
          icon_sprite = (Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + advance * 0x3c);
          gfx_set_scale_ratio(3,2);
          icon_mode = 3;
          break;
        case 4:
          icon_sprite = (Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + advance * 0x3c);
          break;
        case 5:
          icon_mode = -5;
          draw_list_terrain_icon(advance,is_selected,col_base,row,col_x,row_y,row_rect,text_x_off);
          break;
        case 6:
        case 7:
          icon_mode = -2;
          break;
        default:
          text_append((const char *)&str_this_should_not_happen_2);
        }
        draw_list_icon(icon_mode,is_selected,col_base,row,col_x,row_y,row_height,icon_sprite,row_rect,
                       text_x_off);
        icon_mode = 0;
        gfx_set_scale_ratio(scale_num2,scale_den2);
        text_clear();
        advance = ((u32)list_top_index) + col_base + row;
        copy_list_item_name(advance);
        set_clip(&row_rect);
        if (shadow_color2 != text_color2) {
          gfx_set_text_color_index(shadow_color2);
          draw_text((iptr)font2,(const char *)&textbuf.buf,text_x_off + col_x + 2,
                     text_y_off + row_y + 1,5);
          gfx_set_text_color_index(text_color2);
          draw_text((iptr)font2,(const char *)&textbuf.buf,text_x_off + col_x + 1,
                     text_y_off + row_y,5);
        }
        gfx_set_text_color_index(text_color2);
        draw_text((iptr)font2,(const char *)&textbuf.buf,text_x_off + col_x,
                   text_y_off + row_y,5);
        set_clip((RECT *)(iptr)((u8 *)&list_rect));
      }
    }
    col_x = (int)list_rect + (col + 1) * col_width;
    col++;
  } while( true );
}

/* 004f7313 auto */
void __cdecl pedia_list_on_scroll(int scroll_pos)

{
  if (pedia_wnd.list_top != scroll_pos * 9) {
    pedia_wnd.list_top = scroll_pos * 9;
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->draw_item_list();
  }
  return;
}

/* 004f734a auto */
int __cdecl pedia_list_hit_test(int x,int y)

{
  int dim;
  int result;
  uint column;
  
  if (y < pedia_wnd.list_rect_top) {
    result = -1;
  }
  else if (y < pedia_wnd.list_rect_bottom) {
    if (x < pedia_wnd.list_rect) {
      result = -3;
    }
    else if (x < pedia_wnd.list_rect_right) {
      dim = rect_width((RECT *)&pedia_wnd.list_rect);
      column = (uint)(pedia_wnd.list_rect + dim / 2 <= x);
      dim = rect_height((RECT *)&pedia_wnd.list_rect);
      result = (y - pedia_wnd.list_rect_top) / (dim / 9) + column * 9 + pedia_wnd.list_top;
      if (pedia_wnd.list_count <= result) {
        result = -5;
      }
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

/* 004f7454 auto */
void PediaWindow::scroll_to_item(int item)

{
  if (item < pedia_wnd.list_top) {
    for (pedia_wnd.list_top = item; pedia_wnd.list_top % 9 != 0; pedia_wnd.list_top--) {
    }
  }
  else if (pedia_wnd.list_top + 0x12 <= item) {
    for (pedia_wnd.list_top = item; pedia_wnd.list_top % 9 != 0; pedia_wnd.list_top--) {
    }
  }
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* 004f74eb auto */
int __cdecl pedia_find_item_by_key(int key)

{
  int first_char;
  int remaining;
  PediaListNode *list_head;
  int index;
  PediaListNode *node2;
  
  switch(pedia_wnd.category) {
  case 1:
    list_head = pedia_wnd.advance_names;
    remaining = pedia_wnd.advance_entry_count;
    break;
  case 2:
    list_head = pedia_wnd.improvement_names;
    remaining = pedia_wnd.improvement_entry_count;
    break;
  case 3:
    list_head = pedia_wnd.wonder_names;
    remaining = pedia_wnd.wonder_entry_count;
    break;
  case 4:
    list_head = pedia_wnd.unit_names;
    remaining = pedia_wnd.unit_entry_count;
    break;
  case 5:
    list_head = pedia_wnd.terrain_names;
    remaining = pedia_wnd.terrain_count;
    break;
  case 6:
    list_head = pedia_wnd.government_names;
    remaining = pedia_wnd.government_entry_count;
    break;
  case 7:
    list_head = pedia_wnd.concept_names;
    remaining = pedia_wnd.concept_entry_count;
    break;
  default:
    return 0xffffffff;
  }
  node2 = list_head;
  index = pedia_wnd.selected_item + 1;
  if (pedia_wnd.list_count <= index) {
    index = 0;
  }
  for (; index != 0; index--) {
    if (node2->next == 0) {
      node2 = list_head;
    }
    else {
      node2 = node2->next;
    }
  }
  index = pedia_wnd.selected_item + 1;
  if (pedia_wnd.list_count <= index) {
    index = 0;
  }
  while ((remaining != 0 && (first_char = crt_toupper((int)*node2->name), first_char != key))) {
    if (node2->next == 0) {
      node2 = list_head;
      index = 0;
    }
    else {
      node2 = node2->next;
      index++;
    }
    remaining--;
  }
  if (remaining == 0) {
    index = -1;
  }
  return index;
}

/* 004f76ce auto */
void __cdecl pedia_list_on_key(i32 key)

{
  int key_upper;
  int item;
  
  item = pedia_wnd.selected_item;
  if (pedia_wnd.page_mode == 0) {
    key_upper = crt_toupper(key);
    if ((key_upper < 0x41) || (0x5a < key_upper)) {
      switch(key_upper) {
      case 0xa1:
      case 199:
        pedia_wnd.topic = pedia_wnd.list_count - 1;
        for (pedia_wnd.list_top = pedia_wnd.topic; pedia_wnd.list_top % 9 != 0; pedia_wnd.list_top--) {
        }
        pedia_wnd.selected_item = pedia_wnd.topic;
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
        ((Scrollbar *)((i8 *)&pedia_wnd.list_scrollbar))->set_pos(pedia_wnd.list_top / 9);
        break;
      case 0xa2:
      case 0xc1:
        item++;
        if (item < pedia_wnd.list_count) {
          pedia_wnd.topic = item;
          pedia_wnd.selected_item = item;
          ((PediaWindow *)((i8 *)&pedia_wnd.window))->scroll_to_item(item);
          ((Scrollbar *)((i8 *)&pedia_wnd.list_scrollbar))->set_pos(pedia_wnd.list_top / 9);
        }
        break;
      case 0xa3:
      case 0xa6:
      case 0xc3:
      case 0xc6:
        item += 9;
        if (item < pedia_wnd.list_count) {
          pedia_wnd.topic = item;
          pedia_wnd.selected_item = item;
          ((PediaWindow *)((i8 *)&pedia_wnd.window))->scroll_to_item(item);
          ((Scrollbar *)((i8 *)&pedia_wnd.list_scrollbar))->set_pos(pedia_wnd.list_top / 9);
        }
        break;
      case 0xa4:
      case 0xa9:
      case 0xc2:
      case 0xc5:
        item -= 9;
        if (-1 < item) {
          pedia_wnd.topic = item;
          pedia_wnd.selected_item = item;
          ((PediaWindow *)((i8 *)&pedia_wnd.window))->scroll_to_item(item);
          ((Scrollbar *)((i8 *)&pedia_wnd.list_scrollbar))->set_pos(pedia_wnd.list_top / 9);
        }
        break;
      case 0xa7:
      case 0xc4:
        pedia_wnd.topic = 0;
        pedia_wnd.selected_item = 0;
        pedia_wnd.list_top = 0;
        ((Scrollbar *)((i8 *)&pedia_wnd.list_scrollbar))->set_pos(0);
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
        break;
      case 0xa8:
      case 0xc0:
        item--;
        if (-1 < item) {
          pedia_wnd.topic = item;
          pedia_wnd.selected_item = item;
          ((PediaWindow *)((i8 *)&pedia_wnd.window))->scroll_to_item(item);
          ((Scrollbar *)((i8 *)&pedia_wnd.list_scrollbar))->set_pos(pedia_wnd.list_top / 9);
        }
        break;
      case 0xd0: ;
      }
    }
    else {
      item = pedia_find_item_by_key(key_upper);
      if (-1 < item) {
        pedia_wnd.topic = item;
        pedia_wnd.selected_item = item;
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->scroll_to_item(item);
        ((Scrollbar *)((i8 *)&pedia_wnd.list_scrollbar))->set_pos(pedia_wnd.list_top / 9);
      }
    }
  }
  return;
}

/* 004f7a30 auto */
void __cdecl pedia_list_on_click(void)

{
  int hit_item;
  i32 cursor_y;
  i32 cursor_x;
  
  if ((((pedia_wnd.page_mode == 0) &&
       (pedia_wnd.window.window.get_cursor_pos((iptr)&cursor_x,(iptr)&cursor_y), pedia_wnd.list_count != 0)
       ) && (hit_item = pedia_list_hit_test(cursor_x,cursor_y), -1 < hit_item)) && (hit_item < pedia_wnd.list_count)) {
    pedia_wnd.topic = hit_item;
    pedia_wnd.selected_item = hit_item;
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  }
  return;
}

/* 004f7ac7 auto */
void PediaWindow::set_category_title(i32 category)

{
  text_clear();
  switch(category) {
  case 1:
  case 8:
    text_append_label(0x29f);
    break;
  case 2:
    text_append_label(0x2a0);
    break;
  case 3:
    text_append_label(0x2a1);
    break;
  case 4:
    text_append_label(0x2a2);
    break;
  case 5:
    text_append_label(0x2a3);
    break;
  case 6:
    text_append_label(0x2a4);
    break;
  case 7:
    text_append_label(0x2a5);
    break;
  default:
    text_append((const char *)&str_this_should_not_happen_3);
  }
  crt_strcpy((char *)((u8 *)this + 0x618),((i8 *)&textbuf.buf));
  return;
}

/* 004f7bd1 auto */
void PediaWindow::open_category(u32 category,int show)

{
  closed = 0;
  border_width = 0;
  resize_border = 0;
  this->category = category;
  client_y = 0;
  pedia.back_topic = 0xffffffff;
  pedia.going_back = 0;
  set_category_title(category);
  update_controls();
  this->category = category;
  repaint();
  if (show != 0) {
    this->SurfaceWindow::show();
  }
  pedia_wnd.window.window.set_keyboard_nav(0);
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->history_push();
  return;
}

/* Part of PediaWindow::load_indexes (004f7c99): every sort key slot of the seven
   lists set to -1. */
void PediaWindow::clear_index_keys()
{
  u8 *slot;
  int i;

  for (i = 0; i < 100; i++) {
    slot = advance_sort_keys + i * 4;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x27; i++) {
    slot = improvement_sort_keys + i * 4;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x1c; i++) {
    slot = wonder_sort_keys_view + i * 4 + 0x4c;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x3e; i++) {
    slot = unit_key_slots + i * 4 + 0x1c;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 7; i++) {
    slot = government_key_slots + i * 4 + 8;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x21; i++) {
    slot = terrain_key_slots + i * 4 - 0x18;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
  for (i = 0; i < 0x100; i++) {
    slot = concept_key_slots + i * 4 + 0x10;
    slot[0] = 0xff;
    slot[1] = 0xff;
    slot[2] = 0xff;
    slot[3] = 0xff;
  }
}

/* Part of PediaWindow::load_indexes (004f7c99): the advances (@ADVANCE_INDEX of DESCRIBE.TXT): the enabled ones, sorted
   into their list. prev_node2: the caller's, the last node added (carried
   from list to list). */
void PediaWindow::load_advance_index(PediaListNode *&prev_node2)
{
  int status;
  char *name_str;
  size_t name_len;
  void *name_buf;
  char *name_text;
  iptr improvement;
  int sort_key;
  PediaListNode *node2;

  status = text_open_section((const char *)&str_describe_3,(const char *)&str_advance_index_2);
  if (status == 0) {
    text_read_line(1);
    sort_key = text_next_int();
    pedia_advance_count = 0;
    improvement = 0;
    /* BF-217 (Peter Johnson's ledger): at most the game's advances (a longer
       DESCRIBE.TXT index filled the lists past their ends) */
    while ((sort_key != -2) && (improvement < 100)) {
      if (advances[improvement].enabled != '\0') {
        pedia_advance_keys[pedia_advance_count] = sort_key;
        pedia_advance_ids[pedia_advance_count] = improvement;
        node2 = (PediaListNode *)operator_new(0xc);
        node2->index = pedia_advance_count;
        if (pedia_advance_names == (PediaListNode *)0x0) {
          pedia_advance_names = node2;
        }
        else {
          prev_node2->next = node2;
        }
        node2->next = 0;
        name_str = label_string(advances[improvement].name);
        name_len = _strlen(name_str);
        name_buf = operator_new(name_len + 1);
        node2->name = (char *)name_buf;
        name_text = label_string(advances[improvement].name);
        crt_strcpy((char *)((void *)node2->name),name_text);
        prev_node2 = node2;
        pedia_advance_count++;
      }
      improvement++;
      text_read_line(1);
      sort_key = text_next_int();
    }
    text_close_file();
    if (pedia_advance_names != (PediaListNode *)0x0) {
      sort_list(pedia_advance_names,pedia_advance_keys,pedia_advance_ids,
                         pedia_advance_count);
    }
  }
}

/* Part of PediaWindow::load_indexes (004f7c99): the improvements (@IMPROVEMENT_INDEX), sorted
   into their list. prev_node2: the caller's, the last node added (carried
   from list to list). */
void PediaWindow::load_improvement_index(PediaListNode *&prev_node2)
{
  int status;
  char *name_str;
  size_t name_len;
  void *name_buf;
  char *name_text;
  iptr improvement;
  int sort_key;
  PediaListNode *node2;

  status = text_open_section((const char *)&str_describe_3,(const char *)&str_improvement_index_2);
  if (status == 0) {
    text_read_line(1);
    text_read_line(1);
    sort_key = text_next_int();
    pedia_improve_count = 0;
    improvement = 1;
    /* BF-140, BF-214 (Peter Johnson's ledger): at most the game's improvements (1..38) (a longer
       DESCRIBE.TXT index filled the lists past their ends) */
    while ((sort_key != -2) && (improvement < 0x27)) {
      if (-2 < improvements[improvement].preq) {
        pedia_improve_keys[pedia_improve_count] = sort_key;
        pedia_improve_ids[pedia_improve_count] = improvement;
        node2 = (PediaListNode *)operator_new(0xc);
        node2->index = pedia_improve_count;
        if (pedia_improve_names == (PediaListNode *)0x0) {
          pedia_improve_names = node2;
        }
        else {
          prev_node2->next = node2;
        }
        node2->next = 0;
        name_str = label_string(improvements[improvement].name);
        name_len = _strlen(name_str);
        name_buf = operator_new(name_len + 1);
        node2->name = (char *)name_buf;
        name_text = label_string(improvements[improvement].name);
        crt_strcpy((char *)((void *)node2->name),name_text);
        prev_node2 = node2;
        pedia_improve_count++;
      }
      improvement++;
      text_read_line(1);
      sort_key = text_next_int();
    }
    text_close_file();
    if (pedia_improve_names != (PediaListNode *)0x0) {
      sort_list(pedia_improve_names,pedia_improve_keys,pedia_improve_ids,
                         pedia_improve_count);
    }
  }
}

/* Part of PediaWindow::load_indexes (004f7c99): the wonders (@WONDER_INDEX), sorted
   into their list. prev_node2: the caller's, the last node added (carried
   from list to list). */
void PediaWindow::load_wonder_index(PediaListNode *&prev_node2)
{
  int status;
  char *name_str;
  size_t name_len;
  void *name_buf;
  char *name_text;
  iptr improvement;
  int sort_key;
  PediaListNode *node2;

  status = text_open_section((const char *)&str_describe_3,(const char *)&str_wonder_index_2);
  if (status == 0) {
    text_read_line(1);
    sort_key = text_next_int();
    pedia_wonder_count = 0;
    improvement = 0x27;
    /* BF-212 (Peter Johnson's ledger): at most the game's wonders (39..66) (a longer
       DESCRIBE.TXT index filled the lists past their ends) */
    while ((sort_key != -2) && (improvement < 0x43)) {
      if (-2 < improvements[improvement].preq) {
        pedia_wonder_keys[pedia_wonder_count] = sort_key;
        pedia_wonder_ids[pedia_wonder_count] = improvement;
        node2 = (PediaListNode *)operator_new(0xc);
        node2->index = pedia_wonder_count;
        if (pedia_wonder_names == (PediaListNode *)0x0) {
          pedia_wonder_names = node2;
        }
        else {
          prev_node2->next = node2;
        }
        node2->next = 0;
        name_str = label_string(improvements[improvement].name);
        name_len = _strlen(name_str);
        name_buf = operator_new(name_len + 1);
        node2->name = (char *)name_buf;
        name_text = label_string(improvements[improvement].name);
        crt_strcpy((char *)((void *)node2->name),name_text);
        prev_node2 = node2;
        pedia_wonder_count = pedia_wonder_count + 1;
      }
      improvement++;
      text_read_line(1);
      sort_key = text_next_int();
    }
    text_close_file();
    if (pedia_wonder_names != (PediaListNode *)0x0) {
      sort_list(pedia_wonder_names,pedia_wonder_keys,pedia_wonder_ids,
                         pedia_wonder_count);
    }
  }
}

/* Part of PediaWindow::load_indexes (004f7c99): the unit types (@UNIT_INDEX), sorted
   into their list. prev_node2: the caller's, the last node added (carried
   from list to list). */
void PediaWindow::load_unit_index(PediaListNode *&prev_node2)
{
  int status;
  char *name_str;
  size_t name_len;
  void *name_buf;
  char *name_text;
  iptr improvement;
  int sort_key;
  PediaListNode *node2;

  status = text_open_section((const char *)&str_describe_3,(const char *)&str_unit_index_2);
  if (status == 0) {
    text_read_line(1);
    sort_key = text_next_int();
    pedia_unit_count = 0;
    improvement = 0;
    /* BF-215 (Peter Johnson's ledger): at most the game's unit types (a longer
       DESCRIBE.TXT index filled the lists past their ends) */
    while ((sort_key != -2) && (improvement < 0x3e)) {
      if (-2 < unit_types[improvement].prerequisite) {
        pedia_unit_keys[pedia_unit_count] = sort_key;
        pedia_unit_ids[pedia_unit_count] = improvement;
        node2 = (PediaListNode *)operator_new(0xc);
        node2->index = pedia_unit_count;
        if (pedia_unit_names == (PediaListNode *)0x0) {
          pedia_unit_names = node2;
        }
        else {
          prev_node2->next = node2;
        }
        node2->next = 0;
        name_str = label_string(unit_types[improvement].name);
        name_len = _strlen(name_str);
        name_buf = operator_new(name_len + 1);
        node2->name = (char *)name_buf;
        name_text = label_string(unit_types[improvement].name);
        crt_strcpy((char *)((void *)node2->name),name_text);
        prev_node2 = node2;
        pedia_unit_count = pedia_unit_count + 1;
      }
      improvement++;
      text_read_line(1);
      sort_key = text_next_int();
    }
    text_close_file();
    if (pedia_unit_names != (PediaListNode *)0x0) {
      sort_list(pedia_unit_names,pedia_unit_keys,pedia_unit_ids,pedia_unit_count);
    }
  }
}

/* Part of PediaWindow::load_indexes (004f7c99): the governments (@GOVERNMENT_INDEX), sorted
   into their list. prev_node2: the caller's, the last node added (carried
   from list to list). */
void PediaWindow::load_government_index(PediaListNode *&prev_node2)
{
  int status;
  char *name_str;
  size_t name_len;
  void *name_buf;
  char *name_text;
  iptr improvement;
  int sort_key;
  PediaListNode *node2;

  status = text_open_section((const char *)&str_describe_3,(const char *)&str_government_index_2);
  if (status == 0) {
    text_read_line(1);
    sort_key = text_next_int();
    pedia_government_count = 0;
    improvement = 0;
    /* BF-140 (Peter Johnson's ledger): at most the game's governments (a longer
       DESCRIBE.TXT index filled the lists past their ends) */
    while ((sort_key != -2) && (improvement < 7)) {
      pedia_government_keys[pedia_government_count] = sort_key;
      pedia_government_ids[pedia_government_count] = improvement;
      node2 = (PediaListNode *)operator_new(0xc);
      node2->index = pedia_government_count;
      if (pedia_government_names == (PediaListNode *)0x0) {
        pedia_government_names = node2;
      }
      else {
        prev_node2->next = node2;
      }
      node2->next = 0;
      name_str = label_string(government_names[improvement]);
      name_len = _strlen(name_str);
      name_buf = operator_new(name_len + 1);
      node2->name = (char *)name_buf;
      name_text = label_string(government_names[improvement]);
      crt_strcpy((char *)((void *)node2->name),name_text);
      prev_node2 = node2;
      pedia_government_count = pedia_government_count + 1;
      improvement++;
      text_read_line(1);
      sort_key = text_next_int();
    }
    text_close_file();
    if (pedia_government_names != (PediaListNode *)0x0) {
      sort_list(pedia_government_names,pedia_government_keys,pedia_government_ids,pedia_government_count);
    }
  }
}

/* Part of PediaWindow::load_indexes (004f7c99): the terrain types and resources (@TERRAIN_INDEX), "(shield)" after
   grassland with shields, sorted
   into their list. prev_node2: the caller's, the last node added (carried
   from list to list). */
void PediaWindow::load_terrain_index(PediaListNode *&prev_node2)
{
  int status;
  size_t name_len;
  void *name_buf;
  char *name_text;
  iptr terrain_label;
  iptr improvement;
  int sort_key;
  PediaListNode *node2;

  status = text_open_section((const char *)&str_describe_3,(const char *)&str_terrain_index_2);
  if (status == 0) {
    text_read_line(1);
    sort_key = text_next_int();
    pedia_terrain_count_2 = 0;
    improvement = 0;
    /* BF-220 (Peter Johnson's ledger): at most the game's terrain types (a longer
       DESCRIBE.TXT index filled the lists past their ends) */
    while ((sort_key != -2) && (improvement < 0x21)) {
      if (improvement != IMP_SUPERMARKET) {
        pedia_terrain_keys[pedia_terrain_count_2] = sort_key;
        pedia_terrain_ids[pedia_terrain_count_2] = improvement;
        node2 = (PediaListNode *)operator_new(0xc);
        node2->index = pedia_terrain_count_2;
        if (pedia_terrain_names == (PediaListNode *)0x0) {
          pedia_terrain_names = node2;
        }
        else {
          prev_node2->next = node2;
        }
        node2->next = 0;
        text_clear();
        terrain_label = (iptr)label_string(terrain_types[improvement].name);
        text_append((const char *)terrain_label);
        if ((0xc < improvement) &&
           (status = _strcmp(terrain_types[improvement].abbrev,((i8 *)&pedia_grassland_shield_abbrev_str)), status == 0)) {
          text_append_space();
          name_text = label_string(label_table.string_ids[0x2c0]);
          crt_strcat(((i8 *)&textbuf.buf),name_text);
        }
        name_len = _strlen(((i8 *)&textbuf.buf));
        name_buf = operator_new(name_len + 1);
        node2->name = (char *)name_buf;
        crt_strcpy((char *)((void *)node2->name),((i8 *)&textbuf.buf));
        prev_node2 = node2;
        pedia_terrain_count_2++;
      }
      improvement++;
      text_read_line(1);
      sort_key = text_next_int();
    }
    text_close_file();
    if (pedia_terrain_names != (PediaListNode *)0x0) {
      sort_list(pedia_terrain_names,pedia_terrain_keys,pedia_terrain_ids,pedia_terrain_count_2);
    }
  }
}

/* Part of PediaWindow::load_indexes (004f7c99): the game concepts (@CONCEPT_DESCRIPTIONS, the ^^ lines), in file order, sorted
   into their list. prev_node2: the caller's, the last node added (carried
   from list to list). */
void PediaWindow::load_concept_index(PediaListNode *&prev_node2)
{
  bool reading;
  int status;
  size_t name_len;
  void *name_buf;
  iptr improvement;
  char line_prefix[3];
  PediaListNode *node2;

  status = text_open_section((const char *)&str_describe_3,(const char *)&str_concept_descriptions_2);
  if (status == 0) {
    reading = true;
    pedia_concept_count = 0;
    line_prefix[2] = 0;
    improvement = 0;
    /* BF-140 (Peter Johnson's ledger): at most the 256 concepts the list
       holds; BF-121: the end of the file ends the section (the original read
       on without end) */
    while (reading && (pedia_concept_count < 0x100)) {
      text_read_line_or_end(1);
      _strncpy(line_prefix,((i8 *)&textbuf.buf),2);
      status = _strcmp(line_prefix,((i8 *)&pedia_index_marker_str));
      if (status == 0) {
        pedia_concept_keys[pedia_concept_count] =
             pedia_concept_count;
        pedia_concept_ids[pedia_concept_count] = improvement;
        node2 = (PediaListNode *)operator_new(0xc);
        node2->index = pedia_concept_count;
        if (pedia_concept_names == (PediaListNode *)0x0) {
          pedia_concept_names = node2;
        }
        else {
          prev_node2->next = node2;
        }
        node2->next = 0;
        name_len = _strlen(((i8 *)&textbuf.plus2));
        name_buf = operator_new(name_len + 1);
        node2->name = (char *)name_buf;
        crt_strcpy((char *)((void *)node2->name),((i8 *)&textbuf.plus2));
        prev_node2 = node2;
        improvement++;
        pedia_concept_count = pedia_concept_count + 1;
      }
      else if (line_prefix[0] == '@') {
        reading = false;
      }
    }
    if (pedia_concept_names != (PediaListNode *)0x0) {
      sort_list(pedia_concept_names,pedia_concept_keys,pedia_concept_ids,pedia_concept_count);
    }
  }
}

/* 004f7c99 auto */
void PediaWindow::load_indexes()

{
  PediaListNode *prev_node2;
  
  clear_index_keys();
  free_lists();
  load_advance_index(prev_node2);
  load_improvement_index(prev_node2);
  load_wonder_index(prev_node2);
  load_unit_index(prev_node2);
  load_government_index(prev_node2);
  load_terrain_index(prev_node2);
  load_concept_index(prev_node2);
  return;
}

/* 004f896a auto */
void PediaWindow::sort_list(PediaListNode *list_head,i32 *keys,i32 *ids,iptr count)

{
  i32 swap_tmp;
  int cmp;
  int j;
  int i;
  PediaListNode *other_node;
  PediaListNode *node2;
  
  other_node = list_head;
  node2 = list_head;
  for (i = 0; i < count - 1; i++) {
    j = i;
    if (node2->next != 0) {
      other_node = node2->next;
    }
    while (j = j + 1, j < count) {
      cmp = _strcmp(node2->name,other_node->name);
      if (0 < cmp) {
        swap_tmp = keys[i];
        keys[i] = keys[j];
        keys[j] = swap_tmp;
        swap_tmp = ids[i];
        ids[i] = ids[j];
        ids[j] = swap_tmp;
        swap_tmp = (i32)node2->name;
        node2->name = other_node->name;
        other_node->name = (char *)swap_tmp;
      }
      if (other_node->next != 0) {
        other_node = other_node->next;
      }
    }
    if (node2->next != 0) {
      node2 = node2->next;
    }
  }
  return;
}

/* 004f8a9b auto */
i8 * __cdecl pedia_lookup_text(PediaListNode *list_head,iptr item_id)

{
  i8 *text;
  PediaListNode *node2;
  
  for (node2 = list_head; (node2 != (PediaListNode *)0x0 && (node2->index != item_id));
      node2 = node2->next) {
  }
  text = pedia.missing_text;
  if (node2 != (PediaListNode *)0x0) {
    text = node2->name;
  }
  return text;
}

/* 004f8af9 auto */
void PediaWindow::history_push()

{
  if (((int)client_y < 99) && (pedia.going_back == 0)) {
    if (((client_y == 0) ||
        ((((u32)pedia_history_page_modes[client_y - 1] != border_width ||
          (pedia_history_page_modes[client_y - 1] != 0)) ||
         (pedia_history_categories[client_y - 1] == 8)))) || (category == 8))
    {
      if (((client_y == 0) ||
          ((u32)pedia_history_categories[client_y - 1] != category)) ||
         (((u32)pedia_history_page_modes[client_y - 1] != border_width ||
          ((u32)pedia_history_topics[pedia_wnd.history_count - 1] != resize_border)))) {
        if (-1 < pedia.back_topic) {
          pedia_history_topics[client_y] = pedia.back_topic;
          pedia_history_page_modes[client_y] = pedia_wnd.popped_page_mode;
          pedia_history_categories[client_y] = pedia_wnd.popped_category;
          client_y++;
        }
        pedia_history_topics[client_y] = resize_border;
        pedia_history_page_modes[client_y] = border_width;
        pedia_history_categories[client_y] = category;
        client_y++;
      }
    }
    else {
      pedia_history_topics[client_y - 1] = resize_border;
    }
  }
  pedia.back_topic = 0xffffffff;
  pedia.going_back = 0;
  return;
}

/* 004f8d04 auto */
void __cdecl pedia_modal_idle(void)

{
  net.process_messages(1,0);
  if ((pedia_wnd.modal_active == 0) || (pedia_wnd.closed == 1)) {
    pedia_wnd.window.window.end_modal_loop();
    pedia_wnd.modal_active = 0;
  }
  return;
}

/* 004f8d51 auto */
void __cdecl pedia_end_modal(void)

{
  pedia_wnd.window.window.end_modal_loop();
  return;
}

/* 004fa0f0 auto */
/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall CControlBarInfo::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall __non_rtti_object::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall bad_cast::`scalar deleting destructor'(unsigned int)
    public: virtual void * __thiscall bad_typeid::`scalar deleting destructor'(unsigned int)
     9 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void *PediaLink::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004fa140 auto */
/* Library Function - Single Match
    public: __thiscall CControlBarInfo::~CControlBarInfo(void)
   
   Library: Visual Studio 1998 Debug */

void PediaLink::destruct()

{
  i32 exc_link;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  /* inlined unwind funclet 004fa17e */ { ((Hyperlink *)(((int)this) + 4))->destruct(); }
  return;
}

/* 00518f00 auto */
void PediaWindow::init_government_list(iptr unused_arg)

{
  int i;
  
  if (((int)resize_border < 0) || ((int)((u32)pedia_government_count) <= (int)resize_border)) {
    resize_border = 0;
  }
  list_count = ((u32)pedia_government_count);
  list_selected_index = resize_border;
  list_top_index = resize_border;
  while (pedia_wnd.list_top % 9 != 0) {
    list_top_index = list_top_index - 1;
  }
  for (i = 0; i < (int)((u32)pedia_government_count); i++) {
    list_items[i] =
         pedia_government_ids[i];
  }
  ((Scrollbar *)&list_scrollbar)->set_range(0,list_count / 9);
  ((Scrollbar *)&list_scrollbar)->set_pos(list_top_index / 9);
  if (list_count < 1) {
    list_top_index = -1;
  }
  update_controls();
  pedia_wnd.window.window.set_focus();
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* 005190d0 auto */
void __cdecl notice_dlg_global_init(void)

{
  notice_dlg_static_init();
  notice_dlg_reg_atexit();
  return;
}

/* 005190ea auto */
void __cdecl notice_dlg_static_init(void)

{
  ((Dialog *)((i8 *)&game_section_dialog_block.game_section_dialog))->Dialog::construct(0x4000);
  return;
}

/* 00519109 auto */
void __cdecl notice_dlg_reg_atexit(void)

{
  _atexit(notice_dlg_static_dtor);
  return;
}

/* 00519126 auto */
void __cdecl notice_dlg_static_dtor(void)

{
  ((Dialog *)((i8 *)&game_section_dialog_block.game_section_dialog))->Dialog::destruct();
  return;
}

/* 005261a0 auto */
void PediaWindow::init_wonder_list(iptr unused_arg)

{
  int i;
  
  if (((int)resize_border < 0) || ((int)((u32)pedia_wonder_count) <= (int)resize_border)) {
    resize_border = 0;
  }
  list_count = ((u32)pedia_wonder_count);
  list_selected_index = resize_border;
  list_top_index = resize_border;
  while (pedia_wnd.list_top % 9 != 0) {
    list_top_index = list_top_index - 1;
  }
  for (i = 0; i < (int)((u32)pedia_wonder_count); i++) {
    list_items[i] =
         pedia_wonder_ids[i];
  }
  ((Scrollbar *)&list_scrollbar)->set_range(0,list_count / 9);
  ((Scrollbar *)&list_scrollbar)->set_pos(list_top_index / 9);
  if (list_count < 1) {
    list_top_index = -1;
  }
  update_controls();
  pedia_wnd.window.window.set_focus();
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* 0052630d auto */
/* WARNING: Removing unreachable block (ram,0x0052661a) */

void PediaWindow::draw_improvement()

{
  int measure;
  iptr label;
  size_t text_len;
  int center_y;
  u32 yBottom;
  i8 shield_ctx[16];
  i8 icon_ctx[16];
  iptr improvement;
  tagRECT text_rect;
  int line_height;
  int icon_width;
  i32 scale_num2;
  int y2;
  i32 scale_den2;
  int x2;
  int content_width2;
  int value_x;
  i32 unused_flag;
  tagRECT link_rect;
  int text_width;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  unused_flag = 0;
  get_clip((RECT *)saved_clip);
  set_clip((RECT *)&content_left);
  update_controls();
  fill_background((RECT *)&content_left,2);
  font2 = (GameFont *)((i8 *)&font_size21);
  x2 = content_left + 10;
  content_width2 = rect_width((RECT *)&content_left);
  row_height = font2->font_cached_height();
  y2 = content_top + 8;
  value_x = content_width2 / 2 - 100;
  improvement = pedia_wonder_ids[resize_border];
  gfx_get_scale_ratio((iptr)&scale_num2,(iptr)&scale_den2);
  gfx_set_scale_ratio(2,1);
  icon_width = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->get_width();
  icon_width += panel_layout.margin_x * 2;
  ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((iptr)icon_ctx,(iptr)&pedia_wnd.window,x2,
             y2);
  gfx_set_scale_ratio(scale_num2,scale_den2);
  gfx_set_text_color_index(dlg_style.list_text);
  x2 = content_left + 10;
  measure = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->height();
  y2 = y2 + measure * 2 + 5;
  text_clear();
  text_append_label(0x79);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  measure = font2->text_width((const char *)&textbuf.buf);
  x2 = x2 + measure + 5;
  text_clear();
  if (improvements[improvement].preq < '\0') {
    text_append_label(0xe);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  }
  else {
    text_append_string_id(advances[improvements[improvement].preq].name);
    text_width = font2->text_width((const char *)&textbuf.buf);
    line_height = font2->line_spacing();
    SetRect(&link_rect,x2,y2,x2 + text_width + 5,y2 + line_height);
    add_link((int)improvements[improvement].preq,x2,y2,0,link_rect.left,
                       link_rect.top,link_rect.right,link_rect.bottom);
  }
  value_x = x2;
  x2 = content_left + 10;
  y2 += row_height;
  text_clear();
  text_append_label(0x84);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_improvement_cost_fmt),(uint)improvements[improvement].cost * 10);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  measure = font2->text_width((const char *)&textbuf.buf);
  x2 = value_x + measure + 3;
  center_y = y2 + row_height / 2;
  measure = ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->get_height();
  ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->draw((iptr)shield_ctx,(iptr)&pedia_wnd.window,x2,center_y - measure / 2);
  x2 = content_left + 10;
  y2 += row_height;
  text_clear();
  text_append_label(0x9f);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  if (improvement < 0x17) {
    text_append_label(0xe);
    draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  }
  else if (wonder_expires[improvement - 0x27] < 1) {
    text_append_label(0xe);
    draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  }
  else {
    label = (iptr)label_string(advances[wonder_expires[improvement - 0x27]].name);
    text_append((const char *)label);
    text_width = font2->text_width((const char *)&textbuf.buf);
    line_height = font2->line_spacing();
    SetRect(&link_rect,value_x,y2,value_x + text_width + 5,y2 + line_height);
    add_link((int)wonder_expires[improvement - 0x27],value_x,y2,0,link_rect.left,
                       link_rect.top,link_rect.right,link_rect.bottom);
  }
  x2 = content_left + 10;
  y2 += row_height * 2;
  load_improvement_text(improvement);
  text_len = _strlen(((i8 *)&textbuf.buf));
  if (text_len != 0) {
    yBottom = content_bottom;
    measure = rect_width((RECT *)&content_left);
    SetRect(&text_rect,x2,y2,measure - 10 + x2,yBottom);
    draw_text_rect((iptr)font2,(const char *)&textbuf.buf,(iptr)&text_rect,5);
  }
  update_describe_button(resize_border);
  set_clip((RECT *)saved_clip);
  wnd_blit_surface_rect((iptr)&content_left);
  return;
}

/* 00526913 auto */
void PediaWindow::show_wonder_modal(int wonder_id)

{
  char *name_text;
  u32 wonder_idx;
  
  if ((pedia_wnd.closed != 0) && (net.map_redraw_busy == 0)) {
    for (wonder_idx = 0;
        ((int)wonder_idx < pedia_wonder_count &&
        (pedia_wonder_ids[wonder_idx] != wonder_id)); wonder_idx++) {
    }
    if ((u32)pedia_wonder_count != wonder_idx) {
      open_category(3,1);
      resize_border = wonder_idx;
      list_selected_index = wonder_idx;
      client_x = 1;
      border_width = 1;
      clear_links();
      update_controls();
      name_text = pedia_lookup_text(pedia_wnd.wonder_names,wonder_idx);
      crt_strcpy((char *)pedia_title_text_2_618,name_text);
      draw_frame();
      draw_improvement();
      ((DibSurface *)((i8 *)&pedia_wnd.window))->wnd_blit_surface();
      pedia_wnd.window.window.activate();
      pedia_wnd.window.window.set_modal_idle_callback(pedia_modal_idle);
      pedia_wnd.window.window.run_modal_loop();
      pedia_wnd.window.window.set_modal_idle_callback(0);
    }
  }
  return;
}

/* 00564d00 auto */
void PediaWindow::init_advance_list(iptr unused_arg)

{
  int i;
  
  if (((int)resize_border < 0) || ((int)pedia_advance_count <= (int)resize_border)) {
    resize_border = 0;
  }
  list_count = pedia_advance_count;
  list_selected_index = resize_border;
  list_top_index = resize_border;
  while (pedia_wnd.list_top % 9 != 0) {
    list_top_index = list_top_index - 1;
  }
  for (i = 0; i < (int)pedia_advance_count; i++) {
    list_items[i] =
         pedia_advance_ids[i];
  }
  ((Scrollbar *)((u8 *)&list_scrollbar))->set_range(0,list_count / 9);
  ((Scrollbar *)((u8 *)&list_scrollbar))->set_pos(list_top_index / 9);
  if (list_count < 1) {
    list_top_index = -1;
  }
  update_controls();
  pedia_wnd.window.window.set_focus();
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* Part of PediaWindow::draw_advance_page (00564e6d): the "Requires" line: the
   prerequisites as links (or "None"). */
void PediaWindow::draw_advance_requires(GameFont *font2,iptr advance,int y2)
{
  int measure;
  int line_height;
  u32 x2;
  tagRECT link_rect;
  int text_width;

  x2 = content_left + 0x32;
  text_clear();
  text_append_label(0x79);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  measure = font2->text_width((const char *)&textbuf.buf);
  x2 = x2 + measure + 5;
  text_clear();
  if ((advances[advance].preq[0] < '\0') && (advances[advance].preq[1] < '\0')) {
    text_append_label(0xe);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  }
  else {
    if (-1 < advances[advance].preq[0]) {
      text_append_string_id(advances[advances[advance].preq[0]].name);
      text_width = font2->text_width((const char *)&textbuf.buf);
      line_height = font2->line_spacing();
      SetRect(&link_rect,x2,y2,text_width + x2 + 5,line_height + y2);
      add_link((int)advances[advance].preq[0],x2,y2,0,link_rect.left,
                         link_rect.top,link_rect.right,link_rect.bottom);
    }
    if (-1 < advances[advance].preq[1]) {
      if (-1 < advances[advance].preq[0]) {
        x2 = x2 + text_width + 3;
        text_clear();
        text_append_comma();
        draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
        text_width = font2->text_width((const char *)&textbuf.buf);
        text_width += 4;
        x2 += text_width;
      }
      text_clear();
      text_append_string_id(advances[advances[advance].preq[1]].name);
      text_width = font2->text_width((const char *)&textbuf.buf);
      line_height = font2->line_spacing();
      SetRect(&link_rect,x2,y2,text_width + x2 + 5,line_height + y2);
      add_link((int)advances[advance].preq[1],x2,y2,0,link_rect.left,
                         link_rect.top,link_rect.right,link_rect.bottom);
    }
  }
}

/* Part of PediaWindow::draw_advance_page (00564e6d): the "Allows" list:
   each advance this one leads to, with its other prerequisite. */
void PediaWindow::draw_advance_children(GameFont *font2,iptr advance,int list_x2,int row_height,
                                                  int &y2)
{
  int measure;
  int is_child;
  iptr advance3;
  int line_height;
  u32 x2;
  iptr advance2;
  tagRECT link_rect;
  int text_width;

  for (advance2 = 0; advance2 < 100; advance2++) {
    is_child = 0;
    advance3 = -1;
    if (advances[advance2].preq[0] == advance) {
      is_child = 1;
      advance3 = (iptr)advances[advance2].preq[1];
    }
    else if (advances[advance2].preq[1] == advance) {
      is_child = 1;
      advance3 = (iptr)advances[advance2].preq[0];
    }
    if (is_child != 0) {
      x2 = list_x2;
      text_clear();
      text_append_string_id(advances[advance2].name);
      text_width = font2->text_width((const char *)&textbuf.buf);
      line_height = font2->line_spacing();
      SetRect(&link_rect,x2,y2,text_width + x2 + 2,line_height + y2);
      add_link(advance2,x2,y2,0,link_rect.left,link_rect.top,
                         link_rect.right,link_rect.bottom);
      measure = font2->text_width((const char *)&textbuf.buf);
      x2 = x2 + measure + 2;
      if (-1 < advance3) {
        text_clear();
        text_append_space();
        text_append_lparen();
        text_append_label(0x7b);
        text_append_space();
        draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
        measure = font2->text_width((const char *)&textbuf.buf);
        x2 = x2 + measure + 2;
        text_clear();
        text_append_string_id(advances[advance3].name);
        text_width = font2->text_width((const char *)&textbuf.buf);
        line_height = font2->line_spacing();
        SetRect(&link_rect,x2,y2,text_width + x2 + 2,line_height + y2);
        add_link(advance3,x2,y2,0,link_rect.left,link_rect.top,
                           link_rect.right,link_rect.bottom);
        measure = font2->text_width((const char *)&textbuf.buf);
        x2 = x2 + measure + 2;
        text_clear();
        text_append_rparen();
        draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
      }
      y2 += row_height;
    }
  }
}

/* Part of PediaWindow::draw_advance_page (00564e6d): the improvements
   and wonders this advance allows, two to a row. */
void PediaWindow::draw_advance_improvements(GameFont *font2,iptr advance,int list_x2,int row_height,
                                                      u32 &x2,int &y2)
{
  int measure;
  bool was_right;
  i8 impr_ctx[16];
  iptr improvement;
  int line_height;
  tagRECT link_rect;
  int text_width;
  uint in_right_col;

  in_right_col = 0;
  x2 = list_x2;
  for (improvement = 1; improvement < 0x43; improvement++) {
    if (improvements[improvement].preq == advance) {
      ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((iptr)impr_ctx,(iptr)&pedia_wnd.window,
                 x2,y2);
      measure = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->get_width()
      ;
      x2 = x2 + measure + 5;
      text_clear();
      text_append_string_id(improvements[improvement].name);
      text_width = font2->text_width((const char *)&textbuf.buf);
      line_height = font2->line_spacing();
      if ((in_right_col == 0) &&
         (measure = rect_width((RECT *)&content_left),
         measure / 2 <= (int)(text_width + x2 + 5))) {
        measure = rect_width((RECT *)&content_left);
        text_width = (measure / 2 - 5) - x2;
      }
      SetRect(&link_rect,x2,y2,text_width + x2 + 5,line_height + y2);
      if (improvement < 0x27) {
        add_link(improvement,x2,y2,1,link_rect.left,link_rect.top,
                           link_rect.right,link_rect.bottom);
      }
      else {
        add_link(improvement,x2,y2,2,link_rect.left,link_rect.top,
                           link_rect.right,link_rect.bottom);
      }
      was_right = in_right_col != 0;
      if (was_right) {
        y2 += row_height;
        x2 = list_x2;
      }
      else {
        measure = rect_width((RECT *)&content_left);
        x2 = measure / 2;
      }
      in_right_col = (uint)!was_right;
    }
  }
  x2 = list_x2;
  if (in_right_col == 1) {
    y2 += row_height;
  }
}

/* Part of PediaWindow::draw_advance_page (00564e6d): the units this
   advance allows, two to a row (with their pictures when the page has
   room: fewer than 11 items). */
void PediaWindow::draw_advance_units(GameFont *font2,iptr advance,int related_count,int list_x2,
                                               int row_height,u32 &x2,int &y2)
{
  int measure;
  iptr label;
  int center_y;
  i8 unit_ctx[16];
  int prev_unit_height;
  int line_height;
  iptr unit_type;
  int unit_text_y;
  tagRECT link_rect;
  int text_width;
  uint in_right_col;

  in_right_col = 0;
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if (unit_types[unit_type].prerequisite == advance) {
      if (related_count < 0xb) {
        measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->height();
        center_y = y2 + measure / 2;
        measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
        ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->draw((iptr)unit_ctx,(iptr)&pedia_wnd.window,
                   x2,center_y - measure / 2);
        measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->width();
        x2 = x2 + measure + 5;
        measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->height();
        center_y = font2->line_spacing();
        unit_text_y = (measure / 2 - center_y / 2) + y2;
      }
      else {
        unit_text_y = y2;
      }
      text_clear();
      label = (iptr)label_string(unit_types[unit_type].name);
      text_append((const char *)label);
      text_width = font2->text_width((const char *)&textbuf.buf);
      line_height = font2->line_spacing();
      SetRect(&link_rect,x2,unit_text_y,text_width + x2 + 5,line_height + unit_text_y);
      add_link(unit_type,x2,unit_text_y,3,link_rect.left,link_rect.top,
                         link_rect.right,link_rect.bottom);
      if (in_right_col == 0) {
        measure = rect_width((RECT *)&content_left);
        x2 = measure / 2;
        in_right_col = 1;
        prev_unit_height = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
      }
      else {
        x2 = list_x2;
        in_right_col = 0;
        if (related_count < 0xb) {
          measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
          if (measure < prev_unit_height) {
            y2 += prev_unit_height;
          }
          else {
            measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
            y2 += measure;
          }
        }
        else {
          y2 += row_height;
        }
      }
    }
  }
  x2 = list_x2;
  if (in_right_col == 1) {
    measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
    if (measure < prev_unit_height) {
      y2 += prev_unit_height;
    }
    else {
      measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height()
      ;
      y2 += measure;
    }
  }
}

/* Part of PediaWindow::draw_advance_page (00564e6d): the
   facts of the @PEDIACIVFACTS section: the special notes of some advances
   (when the scenario has no Pedia of its own) and the wonders this advance
   makes obsolete. */
void PediaWindow::draw_advance_civ_facts(GameFont *font2,iptr advance,int has_pedia,const char *pedia_file,
                                                   int list_x2,int row_height,u32 x2,int y2)
{
  int measure;
  size_t rest_len;
  iptr wonder;
  int line_height;
  char *line;
  tagRECT link_rect;
  int text_width;
  char line_buf[260];

  measure = text_open_section((const char *)pedia_file,(const char *)&str_pediacivfacts);
  if (measure == 0) {
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) &&
       ((((advance == ADV_MONARCHY || (advance == ADV_COMMUNISM)) || (advance == ADV_FUNDAMENTALISM)) ||
        ((advance == ADV_REPUBLIC || (advance == ADV_DEMOCRACY)))))) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_CONSTRUCTION)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_RADIO)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_RAILROAD)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_REFRIGERATION)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_MYSTICISM)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_ELECTRONICS)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_FUTURE_TECH)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_NUCLEAR_POWER)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_FUSION_POWER)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_PHILOSOPHY)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    if ((has_pedia == 0) && (advance == ADV_COMMUNISM)) {
      draw_text((iptr)font2,(const char *)(line + (*line == '^')),
                 x2,y2,5);
      y2 += row_height;
    }
    line = text_read_line_or_end(1);
    str_append_space((iptr)line);
    crt_strcpy(line_buf,line);
    if (line_buf[0] == '^') {
      rest_len = _strlen((line_buf + 1));
      crt_memmove(line_buf,(line_buf + 1),rest_len);
    }
    for (wonder = 0; wonder < 0x1c; wonder++) {
      if (wonder_expires[wonder] == advance) {
        draw_text((iptr)font2,(const char *)line_buf,x2,y2,5);
        measure = font2->text_width((const char *)line_buf);
        x2 += measure;
        text_clear();
        text_append_string_id(improvements[wonder + 0x27].name);
        text_width = font2->text_width((const char *)&textbuf.buf);
        line_height = font2->line_spacing();
        SetRect(&link_rect,x2,y2,text_width + x2,line_height + y2);
        add_link(wonder + 0x27,x2,y2,2,link_rect.left,link_rect.top,
                           link_rect.right,link_rect.bottom);
        measure = font2->text_width((const char *)&textbuf.buf);
        x2 = x2 + measure + 2;
        text_clear();
        text_append_period();
        draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
        x2 = list_x2;
        y2 += row_height;
      }
    }
  }
}

/* 00564e6d auto */
void PediaWindow::draw_advance_page()

{
  int measure;
  i8 icon_ctx[16];
  iptr advance;
  const char *pedia_file;
  iptr improvement;
  iptr unit_type;
  int y2;
  u32 x2;
  iptr advance2;
  int has_pedia;
  int related_count;
  char line_buf[260];
  int row_height;
  int list_x2;
  i8 saved_clip[16];
  GameFont *font2;
  
  get_clip((RECT *)saved_clip);
  set_clip((RECT *)&content_left);
  update_controls();
  fill_background((RECT *)&content_left,2);
  font2 = (GameFont *)((i8 *)&font_size21);
  x2 = content_left;
  row_height = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
  y2 = content_top + 5;
  advance = pedia_advance_ids[resize_border];
  ((Sprite *)
             (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
             (char)advances[advance].epoch * 0x3c + (char)advances[advance].category * 0xf0))->draw(
             (iptr)icon_ctx,(iptr)&pedia_wnd.window,panel_layout.margin_x + x2,y2);
  gfx_set_text_color_index(dlg_style.list_text);
  related_count = 0;
  for (advance2 = 0; advance2 < 100; advance2++) {
    if ((advances[advance2].preq[0] == advance) || (advances[advance2].preq[1] == advance)) {
      related_count++;
    }
  }
  for (improvement = 1; improvement < 0x43; improvement++) {
    if (improvements[improvement].preq == advance) {
      related_count++;
    }
  }
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if (unit_types[unit_type].prerequisite == advance) {
      related_count++;
    }
  }
  draw_advance_requires(font2,advance,y2);
  y2 += row_height;
  x2 = content_left + 2;
  text_clear();
  text_append_label(0x7a);
  text_append_colon();
  measure = font2->text_width((const char *)&textbuf.buf);
  list_x2 = x2 + measure + 2;
  if (related_count != 0) {
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  }
  text_clear();
  draw_advance_children(font2,advance,list_x2,row_height,y2);
  draw_advance_improvements(font2,advance,list_x2,row_height,x2,y2);
  draw_advance_units(font2,advance,related_count,list_x2,row_height,x2,y2);
  pedia_file = str_pedia_3;
  measure = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  if (measure != 0) {
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcpy(line_buf,str_describe_4);
    crt_strcat(line_buf,((i8 *)&lang_file_ext));
    measure = file_exists((const char *)line_buf);
    if (measure == 0) {
      ((Button *)advance_detail_describe_button)->disable();
      __chdir(((i8 *)&game_dirs.home));
      return;
    }
    __chdir(((i8 *)&game_dirs.home));
  }
  has_pedia = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  if (has_pedia != 0) {
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcpy(line_buf,str_pedia_4);
    crt_strcat(line_buf,((i8 *)&lang_file_ext));
    measure = file_exists((const char *)line_buf);
    if (measure == 0) {
      has_pedia = 0;
    }
    __chdir(((i8 *)&game_dirs.home));
  }
  str_clear((char *)line_buf);
  draw_advance_civ_facts(font2,advance,has_pedia,pedia_file,list_x2,row_height,x2,y2);
  update_describe_button(resize_border);
  set_clip((RECT *)saved_clip);
  wnd_blit_surface_rect((iptr)&content_left);
  return;
}

/* 00566584 auto */
void PediaWindow::show_topic_modal(iptr advance_id)

{
  char *name_text;
  u32 advance_idx;
  
  if ((pedia_wnd.closed != 0) && (net.map_redraw_busy == 0)) {
    advance_idx = 0;
    while (((int)advance_idx < (int)pedia_advance_count &&
           (pedia_advance_ids[advance_idx] != advance_id))) {
      advance_idx++;
    }
    if (pedia_advance_count != advance_idx) {
      open_category(1,0);
      resize_border = advance_idx;
      list_selected_index = advance_idx;
      client_x = 1;
      border_width = 1;
      clear_links();
      update_controls();
      name_text = pedia_lookup_text(pedia_wnd.advance_names,advance_idx);
      crt_strcpy((char *)pedia_title_text_2_618,name_text);
      draw_frame();
      draw_advance_page();
      this->SurfaceWindow::show();
      ((DibSurface *)((i8 *)&pedia_wnd.window))->wnd_blit_surface();
      pedia_wnd.window.window.activate();
      pedia_wnd.window.window.set_modal_idle_callback(pedia_modal_idle);
      pedia_wnd.window.window.run_modal_loop();
      pedia_wnd.window.window.set_modal_idle_callback(0);
    }
  }
  return;
}

/* 005666da auto */
void PediaWindow::draw_advance_tree()

{
  int measure;
  int *icon_rect;
  int measure2;
  int icon_w;
  int gp_line_y;
  int grand_line_y;
  int parent_line_y;
  i8 gp_ctx[16];
  i8 grand_ctx[16];
  i8 parent_ctx[16];
  i8 root_ctx[16];
  iptr advance;
  int line_height;
  int tier2_anchor_y[8];
  Sprite *icon;
  int tier2_anchor_x[8];
  int tier3_step;
  iptr j;
  int y2;
  iptr i;
  int tier3_ids[8];
  int x2;
  int icon_width;
  int tier1_anchors[4];  /* y of the two tier-1 boxes' anchors */
  int root_anchor_y[2];
  int tier1_anchor_x[4];
  RECT icon_box;
  int root_anchor_x[3];
  tagRECT link_rect;
  int tier2_ids[4];
  i8 saved_clip[16];
  GameFont *font2;
  int parent_ids[2];
  
  get_clip((RECT *)saved_clip);
  set_clip((RECT *)&content_left);
  clear_links();
  update_controls();
  fill_background((RECT *)&content_left,2);
  font2 = (GameFont *)((i8 *)&font_size21);
  advance = pedia_advance_ids[resize_border];
  text_clear();
  text_append_string_id(advances[advance].name);
  root_anchor_x[2] = font2->text_width((const char *)&textbuf.buf);
  line_height = font2->line_spacing();
  icon = (Sprite *)
             (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
             (char)advances[advance].category * 0xf0 + (char)advances[advance].epoch * 0x3c);
  icon_width = icon->get_width();
  icon_width += 3;
  x2 = (content_right - 3) - (icon_width + root_anchor_x[2]);
  measure = rect_height((RECT *)&content_left);
  y2 = (content_top + measure / 2) - line_height / 2;
  icon_rect = (int *)icon->draw((iptr)root_ctx,(iptr)&pedia_wnd.window,x2,y2);
  icon_box.left = *icon_rect;
  icon_box.top = icon_rect[1];
  icon_box.right = icon_rect[2];
  icon_box.bottom = icon_rect[3];
  x2 += icon_width;
  SetRect(&link_rect,x2 - icon_width,y2,root_anchor_x[2] + x2 + 5,line_height + y2);
  add_link(advance,x2,y2,0,link_rect.left,link_rect.top,link_rect.right,
                     link_rect.bottom);
  parent_ids[0] = (int)advances[advance].preq[0];
  parent_ids[1] = (int)advances[advance].preq[1];
  root_anchor_x[1] = icon_box.left;
  root_anchor_x[0] = icon_box.left;
  measure = rect_height(&icon_box);
  root_anchor_y[0] = icon_box.top + measure / 2;
  root_anchor_y[1] = root_anchor_y[0];
  line_height = font2->line_spacing();
  measure = rect_height((RECT *)&content_left);
  y2 = (content_top + ((int)(measure + (measure >> 0x1f & 3U)) >> 2)) - line_height / 2;
  for (i = 0; i < 2; i++) {
    tier2_ids[i * 2] = -1;
    tier2_ids[i * 2 + 1] = -1;
    if (-1 < parent_ids[i]) {
      text_clear();
      text_append_string_id(advances[parent_ids[i]].name);
      root_anchor_x[2] = font2->text_width((const char *)&textbuf.buf);
      icon = (Sprite *)
                 (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                 (char)advances[parent_ids[i]].category * 0xf0 +
                 (char)advances[parent_ids[i]].epoch * 0x3c);
      icon_width = icon->get_width();
      icon_width += 3;
      measure = rect_width((RECT *)&content_left);
      measure2 = rect_width((RECT *)&content_left);
      x2 = ((int)(measure * 2 + (measure * 2 >> 0x1f & 3U)) >> 2) +
                 ((int)(measure2 + (measure2 >> 0x1f & 7U)) >> 3) + content_left;
      icon_rect = (int *)icon->draw((iptr)parent_ctx,(iptr)&pedia_wnd.window,x2,y2);
      icon_box.left = *icon_rect;
      icon_box.top = icon_rect[1];
      icon_box.right = icon_rect[2];
      parent_line_y = icon_rect[3];
      icon_box.bottom = parent_line_y;
      if (i != 0) {
        parent_line_y = icon_rect[1] - 1;
      }
      measure = root_anchor_y[i];
      measure2 = root_anchor_x[i];
      icon_w = rect_width(&icon_box);
      pedia_draw_tree_connector(icon_box.left + icon_w / 2,parent_line_y,measure2,measure);
      x2 += icon_width;
      tier2_ids[i * 2] = (int)advances[parent_ids[i]].preq[0];
      tier2_ids[i * 2 + 1] = (int)advances[parent_ids[i]].preq[1];
      tier1_anchor_x[i * 2 + 1] = icon_box.left;
      tier1_anchor_x[i * 2] = tier1_anchor_x[i * 2 + 1];
      measure = rect_height(&icon_box);
      tier1_anchors[i * 2 + 1] = icon_box.top + measure / 2;
      tier1_anchors[i * 2] = tier1_anchors[i * 2 + 1];
      SetRect(&link_rect,x2 - icon_width,y2,root_anchor_x[2] + x2 + 5,line_height + y2
             );
      add_link(parent_ids[i],x2,y2,0,link_rect.left,link_rect.top,
                         link_rect.right,link_rect.bottom);
    }
    measure = rect_height((RECT *)&content_left);
    y2 += measure / 2;
  }
  line_height = font2->line_spacing();
  measure = rect_height((RECT *)&content_left);
  y2 = (content_top + ((int)(measure + (measure >> 0x1f & 7U)) >> 3)) - line_height / 2;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      tier3_ids[(i * 2 + j) * 2] = -1;
      tier3_ids[(i * 2 + j) * 2 + 1] = -1;
      if (-1 < tier2_ids[i * 2 + j]) {
        text_clear();
        text_append_string_id(advances[tier2_ids[i * 2 + j]].name);
        root_anchor_x[2] = font2->text_width((const char *)&textbuf.buf);
        icon = (Sprite *)
                   (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                   (char)advances[tier2_ids[i * 2 + j]].epoch * 0x3c +
                   (char)advances[tier2_ids[i * 2 + j]].category * 0xf0);
        icon_width = icon->get_width();
        icon_width += 3;
        measure = rect_width((RECT *)&content_left);
        measure2 = rect_width((RECT *)&content_left);
        x2 = ((int)(measure + (measure >> 0x1f & 3U)) >> 2) +
                   ((int)(measure2 + (measure2 >> 0x1f & 7U)) >> 3) + content_left;
        icon_rect = (int *)icon->draw((iptr)grand_ctx,(iptr)&pedia_wnd.window,x2,y2);
        icon_box.left = *icon_rect;
        icon_box.top = icon_rect[1];
        icon_box.right = icon_rect[2];
        grand_line_y = icon_rect[3];
        icon_box.bottom = grand_line_y;
        if (j != 0) {
          grand_line_y = icon_rect[1] - 1;
        }
        measure = tier1_anchors[i * 2 + j];
        measure2 = tier1_anchor_x[i * 2 + j];
        icon_w = rect_width(&icon_box);
        pedia_draw_tree_connector(icon_box.left + icon_w / 2,grand_line_y,measure2,measure);
        x2 += icon_width;
        tier3_ids[(i * 2 + j) * 2] =
             (int)advances[tier2_ids[i * 2 + j]].preq[0];
        tier3_ids[(i * 2 + j) * 2 + 1] =
             (int)advances[tier2_ids[i * 2 + j]].preq[1];
        tier2_anchor_x[(i * 2 + j) * 2 + 1] = icon_box.left;
        tier2_anchor_x[(i * 2 + j) * 2] = tier2_anchor_x[(i * 2 + j) * 2 + 1];
        measure = rect_height(&icon_box);
        tier2_anchor_y[(i * 2 + j) * 2 + 1] = icon_box.top + measure / 2;
        tier2_anchor_y[(i * 2 + j) * 2] = tier2_anchor_y[(i * 2 + j) * 2 + 1];
        SetRect(&link_rect,x2 - icon_width,y2,root_anchor_x[2] + x2 + 5,
                line_height + y2);
        add_link(tier2_ids[i * 2 + j],x2,y2,0,
                           link_rect.left,link_rect.top,link_rect.right,link_rect.bottom);
      }
      measure = rect_height((RECT *)&content_left);
      y2 += ((int)(measure + (measure >> 0x1f & 3U)) >> 2);
    }
  }
  measure = rect_height((RECT *)&content_left);
  tier3_step = (int)(measure + (measure >> 0x1f & 7U)) >> 3;
  line_height = font2->line_spacing();
  x2 = content_left + 2;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 2; j++) {
      y2 = ((i * 2 + j) * tier3_step + content_top + tier3_step / 2) -
                 line_height / 2;
      if (-1 < tier3_ids[i * 2 + j]) {
        text_clear();
        text_append_string_id(advances[tier3_ids[i * 2 + j]].name);
        root_anchor_x[2] = font2->text_width((const char *)&textbuf.buf);
        icon = (Sprite *)
                   (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                   (char)advances[tier3_ids[i * 2 + j]].category * 0xf0 +
                   (char)advances[tier3_ids[i * 2 + j]].epoch * 0x3c);
        icon_width = icon->get_width();
        icon_width += 3;
        icon_rect = (int *)icon->draw((iptr)gp_ctx,(iptr)&pedia_wnd.window,x2,y2);
        icon_box.left = *icon_rect;
        icon_box.top = icon_rect[1];
        icon_box.right = icon_rect[2];
        gp_line_y = icon_rect[3];
        icon_box.bottom = gp_line_y;
        if (j != 0) {
          gp_line_y = icon_rect[1] - 1;
        }
        measure = tier2_anchor_y[i * 2 + j];
        measure2 = tier2_anchor_x[i * 2 + j];
        icon_w = rect_width(&icon_box);
        pedia_draw_tree_connector(icon_box.left + icon_w / 2,gp_line_y,measure2,measure);
        if ((-1 < advances[tier3_ids[i * 2 + j]].preq[0]) ||
           (-1 < advances[tier3_ids[i * 2 + j]].preq[1])) {
          measure = rect_height(&icon_box);
          draw_tree_stub(icon_box.left,icon_box.top + measure / 2);
        }
        SetRect(&link_rect,x2,y2,icon_width + root_anchor_x[2] + x2 + 5,
                line_height + y2);
        add_link(tier3_ids[i * 2 + j],icon_width + x2,y2,0,
                           link_rect.left,link_rect.top,link_rect.right,link_rect.bottom);
      }
      measure = rect_height((RECT *)&content_left);
      y2 += ((int)(measure + (measure >> 0x1f & 7U)) >> 3);
    }
  }
  set_clip((RECT *)saved_clip);
  wnd_blit_surface_rect((iptr)&content_left);
  return;
}

/* 005674b9 auto */
void DibSurface::pedia_draw_tree_connector(int x,int y_from,int x_to,int y_to)

{
  if (y_to < y_from) {
    y_from++;
  }
  else {
    y_from--;
  }
  gfx_set_text_color_index(10);
  draw_line(x - 1,y_from,x - 1,y_to);
  draw_line(x + 1,y_from,x + 1,y_to);
  gfx_set_text_color_index(0x7a);
  draw_line(x,y_from,x,y_to);
  if (x < x_to) {
    gfx_set_text_color_index(10);
    draw_line(x + 1,y_to - 1,x_to,y_to - 1);
    draw_line(x + 1,y_to + 1,x_to,y_to + 1);
    gfx_set_text_color_index(0x7a);
    draw_line(x + 1,y_to,x_to,y_to);
  }
  return;
}

/* 005675b7 auto */
void PediaWindow::draw_tree_stub(i32 x,int y)

{
  gfx_set_text_color_index(10);
  draw_line(x,y - 1,content_left,y - 1);
  draw_line(x,y + 1,content_left,y + 1)
  ;
  gfx_set_text_color_index(0x7a);
  draw_line(x,y,content_left,y);
  return;
}

/* 005680a0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl shared_palette_global_init(void)

{
  shared_palette_static_init();
  shared_palette_reg_atexit();
  return;
}

/* 005680ba auto */
void __cdecl shared_palette_static_init(void)

{
  ((GamePalette *)((i8 *)&art_palette))->init();
  return;
}

/* 005680d4 auto */
void __cdecl shared_palette_reg_atexit(void)

{
  _atexit(shared_palette_static_dtor);
  return;
}

/* 005680f1 auto */
void __cdecl shared_palette_static_dtor(void)

{
  ((GamePalette *)((i8 *)&art_palette))->destroy();
  return;
}

/* 00599a20 auto */
void PediaWindow::init_improvement_list(iptr unused_arg)

{
  int i;
  
  if (((int)resize_border < 0) || ((int)pedia_improve_count <= (int)resize_border)) {
    resize_border = 0;
  }
  list_count = pedia_improve_count;
  list_selected_index = resize_border;
  list_top_index = resize_border;
  while (pedia_wnd.list_top % 9 != 0) {
    list_top_index = list_top_index - 1;
  }
  for (i = 0; i < (int)pedia_improve_count; i++) {
    list_items[i] =
         pedia_improve_ids[i];
  }
  ((Scrollbar *)((u8 *)&list_scrollbar))->set_range(0,list_count / 9);
  ((Scrollbar *)((u8 *)&list_scrollbar))->set_pos(list_top_index / 9);
  if (list_count < 1) {
    list_top_index = -1;
  }
  update_controls();
  pedia_wnd.window.window.set_focus();
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* 00599b8d auto */
/* WARNING: Removing unreachable block (ram,0x00599e9a) */
/* WARNING: Removing unreachable block (ram,0x00599fc4) */

void PediaWindow::draw_improvement_page()

{
  int measure;
  size_t text_len;
  int center_y;
  u32 yBottom;
  i8 coin_ctx[16];
  i8 shield_ctx[16];
  i8 icon_ctx[16];
  iptr improvement;
  tagRECT text_rect;
  int line_height;
  int icon_width;
  i32 scale_num2;
  int y2;
  i32 scale_den2;
  int x2;
  int content_width2;
  int value_x;
  i32 unused_flag;
  tagRECT link_rect;
  int text_width;
  int row_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  unused_flag = 0;
  get_clip((RECT *)saved_clip);
  set_clip((RECT *)&content_left);
  update_controls();
  fill_background((RECT *)&content_left,2);
  font2 = (GameFont *)((i8 *)&font_size21);
  x2 = content_left + 10;
  content_width2 = rect_width((RECT *)&content_left);
  row_height = font2->font_cached_height();
  y2 = content_top + 8;
  value_x = content_width2 / 2 - 100;
  improvement = pedia_improve_ids[resize_border];
  gfx_get_scale_ratio((iptr)&scale_num2,(iptr)&scale_den2);
  gfx_set_scale_ratio(2,1);
  icon_width = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->get_width();
  icon_width += panel_layout.margin_x * 2;
  ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((iptr)icon_ctx,(iptr)&pedia_wnd.window,x2,
             y2);
  gfx_set_scale_ratio(scale_num2,scale_den2);
  gfx_set_text_color_index(dlg_style.list_text);
  x2 = content_left + 10;
  measure = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->height();
  y2 = y2 + measure * 2 + 5;
  text_clear();
  text_append_label(0x79);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  measure = font2->text_width((const char *)&textbuf.buf);
  x2 = x2 + measure + 5;
  text_clear();
  if (improvements[improvement].preq < '\0') {
    text_append_label(0xe);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  }
  else {
    text_append_string_id(advances[improvements[improvement].preq].name);
    text_width = font2->text_width((const char *)&textbuf.buf);
    line_height = font2->line_spacing();
    SetRect(&link_rect,x2,y2,x2 + text_width + 5,y2 + line_height);
    add_link((int)improvements[improvement].preq,x2,y2,0,link_rect.left,
                       link_rect.top,link_rect.right,link_rect.bottom);
  }
  value_x = x2;
  x2 = content_left + 10;
  y2 += row_height;
  text_clear();
  text_append_label(0x84);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_improve_cost_fmt),(uint)improvements[improvement].cost * 10);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  measure = font2->text_width((const char *)&textbuf.buf);
  x2 = value_x + measure + 3;
  center_y = y2 + row_height / 2;
  measure = ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->get_height();
  ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->draw((iptr)shield_ctx,(iptr)&pedia_wnd.window,x2,center_y - measure / 2);
  x2 = content_left + 10;
  y2 += row_height;
  text_clear();
  text_append_label(0x9e);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_improve_upkeep_fmt),(uint)improvements[improvement].upkeep);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  measure = font2->text_width((const char *)&textbuf.buf);
  x2 = value_x + measure + 3;
  center_y = y2 + row_height / 2;
  measure = ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->get_height();
  ((Sprite *)((i8 *)&tax_rate_icon_sprites_block.gold_coin_icon_sprite))->draw((iptr)coin_ctx,(iptr)&pedia_wnd.window,x2,center_y - measure / 2);
  x2 = content_left + 10;
  y2 += row_height * 2;
  load_improvement_text(improvement);
  text_len = _strlen(((i8 *)&textbuf.buf));
  if (text_len != 0) {
    yBottom = content_bottom;
    measure = rect_width((RECT *)&content_left);
    SetRect(&text_rect,x2,y2,measure - 10 + x2,yBottom);
    draw_text_rect((iptr)font2,(const char *)&textbuf.buf,(iptr)&text_rect,5);
  }
  update_describe_button(resize_border);
  set_clip((RECT *)saved_clip);
  wnd_blit_surface_rect((iptr)&content_left);
  return;
}

/* 0059a15d auto */
void PediaWindow::load_improvement_text(iptr improvement_id)

{
  int status;
  char *_Str;
  size_t len;
  i32 ret_addr;
  char desc_buf[4096];
  i8 line_buf[256];
  i8 section_name[32];
  str_clear((char *)desc_buf);
  ret_addr = 0x59a190;
  crt_strcpy(section_name,str_pediaimprove_2);
  ret_addr = 0x59a1a0;
  str_append_int((iptr)section_name,improvement_id);
  ret_addr = 0x59a1b1;
  status = text_open_section((const char *)&str_pedia_5,(const char *)section_name);
  if (status == 0) {
    text_clear();
    while (_Str = text_read_line_or_end(1), *_Str != '@') {
      if ((*_Str != ';') && (len = _strlen(_Str), len != 0)) {
        while ((*_Str == '^' || (*_Str == '*'))) {
          ret_addr = 0x59a253;
          crt_strcpy(line_buf,_Str + 1);
          ret_addr = 0x59a269;
          crt_strcpy(_Str,line_buf);
        }
        str_trim(_Str);
        len = _strlen(desc_buf);
        /* BF-069 (Peter Johnson's ledger): the text cut where the text
           buffer it is copied into ends (the original appended without a
           bound and copied the whole of it) */
        if (len != 0) {
          ret_addr = 0x59a2a8;
          str_append_max(desc_buf,((i8 *)&pedia_improve_text_space_str),TEXTBUF_SIZE);
        }
        ret_addr = 0x59a2be;
        str_append_max(desc_buf,_Str,TEXTBUF_SIZE);
      }
    }
  }
  ret_addr = 0x59a2d7;
  crt_strcpy(((i8 *)&textbuf.buf),desc_buf);
  return;
}

/* 0059a2e6 auto */
void PediaWindow::show_improvement_modal(iptr improvement_id)

{
  char *name_text;
  u32 improve_idx;
  
  if ((pedia_wnd.closed != 0) && (net.map_redraw_busy == 0)) {
    if (improvement_id < 0x27) {
      improve_idx = 0;
      while (((int)improve_idx < (int)pedia_improve_count &&
             (pedia_improve_ids[improve_idx] != improvement_id))) {
        improve_idx++;
      }
      if (pedia_improve_count != improve_idx) {
        open_category(2,0);
        resize_border = improve_idx;
        list_selected_index = improve_idx;
        client_x = 1;
        border_width = 1;
        clear_links();
        update_controls();
        name_text = pedia_lookup_text(pedia_wnd.improvement_names,improve_idx);
        crt_strcpy((char *)pedia_title_text_2_618,name_text);
        draw_frame();
        draw_improvement_page();
        this->SurfaceWindow::show();
        ((DibSurface *)((i8 *)&pedia_wnd.window))->wnd_blit_surface();
        pedia_wnd.window.window.activate();
        pedia_wnd.window.window.set_modal_idle_callback(pedia_modal_idle);
        pedia_wnd.window.window.run_modal_loop();
        pedia_wnd.window.window.set_modal_idle_callback(0);
      }
    }
    else {
      show_wonder_modal(improvement_id);
    }
  }
  return;
}

/* 005ac840 auto */
void PediaWindow::show_unit_list(iptr unused_arg)

{
  int i;
  
  if (((int)resize_border < 0) || (pedia_unit_count <= (int)resize_border)) {
    resize_border = 0;
  }
  list_count = pedia_unit_count;
  list_selected_index = resize_border;
  list_top_index = resize_border;
  while (pedia_wnd.list_top % 9 != 0) {
    list_top_index = list_top_index - 1;
  }
  for (i = 0; i < pedia_unit_count; i++) {
    list_items[i] =
         pedia_unit_ids[i];
  }
  ((Scrollbar *)&list_scrollbar)->set_range(0,list_count / 9);
  ((Scrollbar *)&list_scrollbar)->set_pos(list_top_index / 9);
  if (list_count < 1) {
    list_top_index = -1;
  }
  update_controls();
  pedia_wnd.window.window.set_focus();
  pedia_wnd.window.window.activate();
  ((PediaWindow *)((i8 *)&pedia_wnd.window))->redraw();
  return;
}

/* Part of PediaWindow::draw_unit_page (005ac9ad): one line of the
   @PEDIAUNITFACTS section, read for each of the 15 unit flags in turn and
   added to the description when the unit has the flag (mask). */
void PediaWindow::append_unit_fact(iptr unit_type,u32 mask,char *desc_buf,int &has_text)
{
  int line;

  line = (int)text_read_line_or_end(1);
  /* BF-053 (Peter Johnson's ledger): within draw_unit_page's 1000 bytes (the
     original appended without a bound); a longer text is cut */
  if ((unit_types[unit_type].flags & mask) != 0) {
    if (has_text == 0) {
      str_copy_max(desc_buf,(char *)(line + 1),1000);
      has_text = 1;
    }
    else {
      str_append_max(desc_buf,(char *)(line + 1),1000);
    }
    str_append_max(desc_buf,((i8 *)&append_space_str),1000);
  }
}

/* 005ac9ad auto */
void PediaWindow::draw_unit_page()

{
  int measure;
  size_t text_len;
  int center_y;
  u32 yBottom;
  i8 shield_ctx[16];
  i8 unit_ctx[16];
  tagRECT desc_rect;
  int has_text;
  int line_height;
  tagRECT link_rect;
  int text_width;
  int row_height;
  iptr unit_type;
  const char *pedia_file;
  int half_width;
  i32 unused_a;
  int icon_width;
  char desc_buf[1000];
  i32 unused_d;
  int y2;
  u32 x2;
  int content_width2;
  int value_x;
  i32 unused_b;
  int margin;
  i32 text_color2;
  i32 unused_c;
  i32 scale_den2;
  i32 scale_num2;
  int slot_height;
  i8 saved_clip[16];
  GameFont *font2;
  
  get_clip((RECT *)saved_clip);
  set_clip((RECT *)&content_left);
  update_controls();
  fill_background((RECT *)&content_left,2);
  font2 = (GameFont *)((i8 *)&font_size21);
  x2 = content_left;
  margin = 10;
  content_width2 = rect_width((RECT *)&content_left);
  half_width = content_width2 / 2;
  slot_height = rect_height((RECT *)&content_left);
  slot_height = slot_height / 0xc;
  row_height = font2->font_cached_height();
  y2 = content_top + 0xf;
  unit_type = pedia_unit_ids[resize_border];
  x2 += margin;
  gfx_get_scale_ratio((iptr)&scale_num2,(iptr)&scale_den2);
  gfx_set_scale_ratio(2,1);
  icon_width = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_width()
  ;
  icon_width += panel_layout.margin_x * 2;
  center_y = y2 + slot_height / 2;
  measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
  ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->draw((iptr)unit_ctx,(iptr)&pedia_wnd.window,x2,
             center_y - measure / 2);
  gfx_set_scale_ratio(scale_num2,scale_den2);
  text_color2 = dlg_style.list_text;
  gfx_set_text_color_index(dlg_style.list_text);
  measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_width();
  x2 = x2 + icon_width + measure + 0x32;
  y2 = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
  y2 = content_top + y2;
  text_clear();
  text_append_label(0x79);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  measure = font2->text_width((const char *)&textbuf.buf);
  x2 = x2 + measure + 5;
  text_clear();
  if (unit_types[unit_type].prerequisite < '\0') {
    text_append_label(0xe);
    draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  }
  else {
    text_append_string_id(advances[unit_types[unit_type].prerequisite].name);
    text_width = font2->text_width((const char *)&textbuf.buf);
    line_height = font2->line_spacing();
    SetRect(&link_rect,x2,y2,text_width + x2 + 5,line_height + y2);
    add_link((int)unit_types[unit_type].prerequisite,x2,y2,0,
                       link_rect.left,link_rect.top,link_rect.right,link_rect.bottom);
  }
  value_x = half_width - 0x4b;
  x2 = content_left + margin;
  measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
  y2 = measure * 2 + content_top + row_height;
  text_clear();
  text_append_label(0x84);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  if ((char)unit_types[unit_type].cost < '\0') {
    text_append_label(0xe);
    draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  }
  else {
    _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_unit_cost_fmt),(char)unit_types[unit_type].cost * 10);
    draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
    measure = font2->text_width((const char *)&textbuf.buf);
    x2 = value_x + measure + 5;
    center_y = y2 + slot_height / 2;
    measure = ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->get_height();
    ((Sprite *)((i8 *)&war_icon_sprite_block.shield_icon_sprite))->draw((iptr)shield_ctx,(iptr)&pedia_wnd.window,x2,
               (center_y - 2) - measure / 2);
  }
  x2 = content_left + margin;
  y2 += row_height;
  text_clear();
  text_append_label(0x7f);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_unit_attack_fmt),(int)(char)unit_types[unit_type].attack);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  y2 += row_height;
  text_clear();
  text_append_label(0x80);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_unit_defense_fmt),(int)(char)unit_types[unit_type].defense);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  y2 += row_height;
  text_clear();
  text_append_label(0x82);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_unit_hp_fmt),(int)(char)unit_types[unit_type].hit_points / 10);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  y2 += row_height;
  text_clear();
  text_append_label(0x83);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_unit_firepower_fmt),(int)(char)unit_types[unit_type].firepower);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  y2 += row_height;
  text_clear();
  text_append_label(0x81);
  text_append_colon();
  draw_text((iptr)font2,(const char *)&textbuf.buf,x2,y2,5);
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&pedia_unit_move_fmt),(int)(char)unit_types[unit_type].move / 3);
  draw_text((iptr)font2,(const char *)&textbuf.buf,value_x,y2,5);
  pedia_file = str_pedia_6;
  text_clear();
  text_append_int(unit_type);
  unused_a = 0;
  unused_b = 0;
  unused_c = 7;
  unused_d = 0;
  measure = text_open_section((const char *)pedia_file,(const char *)&str_pediaunitfacts);
  if (measure == 0) {
    has_text = 0;
    str_clear((char *)desc_buf);
    append_unit_fact(unit_type,1,desc_buf,has_text);
    append_unit_fact(unit_type,2,desc_buf,has_text);
    append_unit_fact(unit_type,4,desc_buf,has_text);
    append_unit_fact(unit_type,8,desc_buf,has_text);
    append_unit_fact(unit_type,0x10,desc_buf,has_text);
    append_unit_fact(unit_type,0x20,desc_buf,has_text);
    append_unit_fact(unit_type,0x40,desc_buf,has_text);
    append_unit_fact(unit_type,0x80,desc_buf,has_text);
    append_unit_fact(unit_type,0x100,desc_buf,has_text);
    append_unit_fact(unit_type,0x200,desc_buf,has_text);
    append_unit_fact(unit_type,0x400,desc_buf,has_text);
    append_unit_fact(unit_type,0x800,desc_buf,has_text);
    append_unit_fact(unit_type,0x1000,desc_buf,has_text);
    append_unit_fact(unit_type,0x2000,desc_buf,has_text);
    append_unit_fact(unit_type,0x4000,desc_buf,has_text);
    text_close_file();
    x2 = content_left + half_width;
    measure = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_height();
    y2 = measure * 2 + content_top + row_height;
    text_len = _strlen(desc_buf);
    if (text_len != 0) {
      yBottom = content_bottom;
      measure = rect_width((RECT *)&content_left);
      SetRect(&desc_rect,x2,y2,x2 + measure / 2,yBottom);
      draw_text_rect((iptr)font2,(const char *)desc_buf,(iptr)&desc_rect,5);
    }
  }
  update_describe_button(resize_border);
  set_clip((RECT *)saved_clip);
  wnd_blit_surface_rect((iptr)&content_left);
  return;
}

/* 005ad998 auto */
void PediaWindow::show_unit_modal(iptr unit_type)

{
  char *name_text;
  u32 unit_index;
  
  if ((pedia_wnd.closed != 0) && (net.map_redraw_busy == 0)) {
    unit_index = 0;
    while (((int)unit_index < pedia_unit_count &&
           (pedia_unit_ids[unit_index] != unit_type))) {
      unit_index++;
    }
    if ((u32)pedia_unit_count != unit_index) {
      open_category(4,0);
      resize_border = unit_index;
      list_selected_index = unit_index;
      client_x = 1;
      border_width = 1;
      clear_links();
      update_controls();
      name_text = pedia_lookup_text(pedia_wnd.unit_names,unit_index);
      crt_strcpy((char *)pedia_title_text_2_618,name_text);
      draw_frame();
      draw_unit_page();
      this->SurfaceWindow::show();
      ((DibSurface *)((i8 *)&pedia_wnd.window))->wnd_blit_surface();
      pedia_wnd.window.window.activate();
      pedia_wnd.window.window.set_modal_idle_callback(pedia_modal_idle);
      pedia_wnd.window.window.run_modal_loop();
      pedia_wnd.window.window.set_modal_idle_callback(0);
    }
  }
  return;
}
