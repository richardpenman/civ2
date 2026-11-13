/* The game, windows: dialogs (their fonts, lines, options, buttons, lists and keys): functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0059d190 auto */
void __cdecl dialog_text_font_global_init(void)

{
  dialog_text_font_static_init();
  dialog_text_font_reg_atexit();
  return;
}

/* 0059d1aa auto */
void __cdecl dialog_text_font_static_init(void)

{
  ((GameFont *)((i8 *)&dialog_text_font_obj_block.dialog_text_font_obj))->init_styled(0,0x10,1);
  return;
}

/* 0059d1ca auto */
void __cdecl dialog_text_font_reg_atexit(void)

{
  _atexit(dialog_text_font_atexit_dtor);
  return;
}

/* 0059d1e7 auto */
void __cdecl dialog_text_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&dialog_text_font_obj_block.dialog_text_font_obj))->destroy();
  return;
}

/* 0059d201 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl dialog_title_font_global_init(void)

{
  dialog_title_font_static_init();
  dialog_title_font_reg_atexit();
  return;
}

/* 0059d21b auto */
void __cdecl dialog_title_font_static_init(void)

{
  ((GameFont *)((i8 *)&dialog_title_font_obj_block.dialog_title_font_obj))->init(0,0x10);
  return;
}

/* 0059d239 auto */
void __cdecl dialog_title_font_reg_atexit(void)

{
  _atexit(dialog_title_font_atexit_dtor);
  return;
}

/* 0059d256 auto */
void __cdecl dialog_title_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&dialog_title_font_obj_block.dialog_title_font_obj))->destroy();
  return;
}

/* 0059d270 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl dialog_list_font_global_init(void)

{
  dialog_list_font_static_init();
  dialog_list_font_reg_atexit();
  return;
}

/* 0059d28a auto */
void __cdecl dialog_list_font_static_init(void)

{
  ((GameFont *)((i8 *)&dialog_list_font_obj))->init(1,0xe);
  return;
}

/* 0059d2a8 auto */
void __cdecl dialog_list_font_reg_atexit(void)

{
  _atexit(dialog_list_font_atexit_dtor);
  return;
}

/* 0059d2c5 auto */
void __cdecl dialog_list_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&dialog_list_font_obj))->destroy();
  return;
}

/* 0059d2df auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl dialog_plain_font_global_init(void)

{
  dialog_plain_font_static_init();
  dialog_plain_font_reg_atexit();
  return;
}

/* 0059d2f9 auto */
void __cdecl dialog_plain_font_static_init(void)

{
  ((GameFont *)((i8 *)&dialog_plain_font_obj_block.dialog_plain_font_obj))->init(0,0x10);
  return;
}

/* 0059d317 auto */
void __cdecl dialog_plain_font_reg_atexit(void)

{
  _atexit(dialog_plain_font_atexit_dtor);
  return;
}

/* 0059d334 auto */
void __cdecl dialog_plain_font_atexit_dtor(void)

{
  ((GameFont *)((i8 *)&dialog_plain_font_obj_block.dialog_plain_font_obj))->destroy();
  return;
}

/* 0059d34e auto */
void __cdecl active_dialog_global_init(void)

{
  active_dialog_static_init();
  return;
}

/* 0059d363 auto */
void __cdecl active_dialog_static_init(void)

{
  dialog_state.active = (Dialog *)dlg_style.stack[0];
  return;
}

/* 0059d37d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl dialog_edit_on_change(void)

{
  dialog_state.timeout_start = timer_ticks();
  return;
}

/* 0059d397 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl dialog_radio_on_select(void)

{
  dialog_state.timeout_start = timer_ticks();
  return;
}

/* 0059d3b1 auto */
void __cdecl dlg_set_palette(iptr palette)

{
  dlg_defaults.palette = palette;
  return;
}

/* 0059d3c9 auto */
void __cdecl dlg_set_parent_window(iptr window)

{
  dlg_defaults.parent_window = window;
  return;
}

/* 0059d3e1 auto */
void __cdecl dialog_set_default_offset(i32 x,i32 y)

{
  dlg_defaults.x = x;
  dlg_defaults.y = y;
  return;
}

/* 0059d401 auto */
void __cdecl dialog_load_button_labels(void)

{
  int status;
  i32 label_id;
  i8 *label;
  iptr i;
  
  status = text_open_section((const char *)&str_labels_6,(const char *)&str_popups);
  if (status == 0) {
    for (i = 0; i < 3; i++) {
      label_id = text_read_line_pooled();
      dialog_plain_font_obj_block.dialog_button_label_ids[i] = label_id;
      label = label_string(dialog_plain_font_obj_block.dialog_button_label_ids[i]);
      (((i8 * *)&dlg_style.button_labels))[i] = label;
    }
  }
  return;
}

/* 0059d487 auto */
void __cdecl
dialog_set_colors(i32 text_color,i32 text_shadow,i32 frame_color,i32 highlight_color,
            i32 bg_color,i32 bevel_light,i32 bevel_dark,i32 title_color,
            i32 title_shadow)

{
  dlg_style.text_color = (i8)text_color;
  dlg_style.text_shadow_color = (i8)text_shadow;
  dlg_style.frame_color = (i8)frame_color;
  (*(i8 *)&dlg_style.highlight_text_color) = (i8)highlight_color;
  (*(i8 *)&dlg_style.bg_color) = (i8)bg_color;
  (*(i8 *)&dlg_style.bevel_light_color) = (i8)bevel_light;
  (*(i8 *)&dlg_style.bevel_dark_color) = (i8)bevel_dark;
  dlg_style.title_text_color = (i8)title_color;
  dlg_style.title_shadow_color = (i8)title_shadow;
  return;
}

/* 0059d4df auto */
void __cdecl
dialog_set_list_colors(i32 bg_color,i32 text_color,i32 shadow_color,i32 sel_bg_color,
            i32 sel_text_color,i32 sel_shadow_color,i32 default_color)

{
  dlg_style.list_bg = bg_color;
  dlg_style.list_text = text_color;
  dlg_style.list_shadow = shadow_color;
  dlg_style.list_sel_bg = sel_bg_color;
  dlg_style.sel_text_color = sel_text_color;
  dlg_style.sel_text_shadow_color = sel_shadow_color;
  dlg_style.default_list_color = default_color;
  return;
}

/* 0059d59d auto */
void __cdecl dialog_set_frame_tile_surface(DibSurface *surface)

{
  dlg_style.frame_tile_surface = (int)surface;
  return;
}

/* 0059d5b5 auto */
void Dialog::reset_string_pool(uint size)

{
  memblock_free((MemBlock *)&(this)->string_pool);
  memblock_init((MemBlock *)&(this)->string_pool,9,size);
  return;
}

/* 0059d5f5 auto */
void Dialog::init_defaults()

{
  u8 *slot;
  int i;
  
  surf_wnd = 0;
  bg_surface = 0;
  pool_valid = '\0';
  list_font = 0;
  title_font = list_font;
  body_font = title_font;
  sprite_count = 0;
  option_count = sprite_count;
  edit_count = option_count;
  line_count = edit_count;
  extra_button_count = 0;
  std_button_count = extra_button_count;
  style_flags = 0;
  scale_num = (*(u32 *)&dlg_style.scale_num);
  scale_den = (*(u32 *)&dlg_style.scale_den);
  window_y = 0xfffffc19;
  window_x = window_y;
  origin_y = 0;
  origin_x = origin_y;
  wnd_height = 0;
  wnd_width = wnd_height;
  if (((*(u32 *)&dlg_defaults.x) == 0) && ((*(u32 *)&dlg_defaults.y) == 0)) {
    req_y = 0xfffffc19;
    req_x = req_y;
  }
  else {
    req_x = (*(u32 *)&dlg_defaults.x);
    req_y = (*(u32 *)&dlg_defaults.y);
  }
  min_height = 0;
  edit_label_width = 0;
  bg_color = (uint)dlg_style.bg_color;
  text_color = (uint)(*(byte *)&dlg_style.text_color);
  shadow_color = (uint)(*(byte *)&dlg_style.text_shadow_color);
  frame_color = (uint)(*(byte *)&dlg_style.frame_color);
  highlight_color = (uint)dlg_style.highlight_text_color;
  bevel_light_color = (uint)dlg_style.bevel_light_color;
  bevel_dark_color = (uint)dlg_style.bevel_dark_color;
  title_color = (uint)(*(byte *)&dlg_style.title_text_color);
  title_shadow_color = (uint)(*(byte *)&dlg_style.title_shadow_color);
  list_fill_color = (*(u32 *)&dlg_style.list_bg);
  list_fg_color = (*(u32 *)&dlg_style.list_text);
  list_fg_shadow_color = (*(u32 *)&dlg_style.list_shadow);
  list_sel_fill_color = (*(u32 *)&dlg_style.list_sel_bg);
  sel_text_color = (*(u32 *)&dlg_style.sel_text_color);
  sel_shadow_color = (*(u32 *)&dlg_style.sel_text_shadow_color);
  list_default_color = (*(u32 *)&dlg_style.default_list_color);
  for (i = 0; i < 2; i++) {
    slot = (u8 *)&list_row_height_override[i];
    slot[0] = '\0';
    slot[1] = '\0';
    slot[2] = '\0';
    slot[3] = '\0';
    slot = (u8 *)&list_indents[i];
    slot[0] = '\0';
    slot[1] = '\0';
    slot[2] = '\0';
    slot[3] = '\0';
    slot = (u8 *)&pane_drawn[i];
    slot[0] = '\0';
    slot[1] = '\0';
    slot[2] = '\0';
    slot[3] = '\0';
  }
  cur_pane = 0;
  edit_width = 0;
  item_width_value = edit_width;
  max_sprite_width = item_width_value;
  content_width = max_sprite_width;
  option_width = content_width;
  selected_option = 0;
  selected_sprite = 0;
  option_tail = 0;
  option_list = option_tail;
  line_list = 0;
  sprite_list = 0;
  edit_field_list = 0;
  option_spacing = 0;
  pad_y = 2;
  pad_x = 2;
  section_gap = 4;
  sprite_gap = 4;
  border_width = 8;
  bevel_width = 2;
  option_extra_width = 0;
  columns = 1;
  list_left_col[1] = 0;
  list_left_col[0] = list_left_col[1];
  sprite_zoom = 0;
  title_text = 0;
  radio_group = 0;
  checkboxes = 0;
  edit_controls = 0;
  button_controls = 0;
  listbox[1] = 0;
  listbox[0] = listbox[1];
  sprite_hotspots = 0;
  list_vscroll[1] = 0;
  list_vscroll[0] = list_vscroll[1];
  list_hscroll[1] = 0;
  list_hscroll[0] = list_hscroll[1];
  on_draw_sprite = 0;
  on_sprite_click = 0;
  on_sprite_dblclick = 0;
  on_draw_option = 0;
  on_select_change = 0;
  on_modal_poll = 0;
  result_id = 0;
  result_extra = 0;
  return;
}

/* 0059db08 auto */
Dialog::Dialog(i32 size)

{
  ((MemBlock *)string_pool)->construct();
  pool_ptr = 0;
  pool_handle = pool_ptr;
  init_defaults();
  pool_size = (u16)size;
}

/* 0059db65 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void Dialog::close()

{
  if ((*(WndBase * *)&dlg_defaults.parent_window) == (WndBase *)0x0) {
    if (main_frame_created != 0) {
      main_frame_wnd_block.main_frame_wnd.focus_and_raise();
    }
  }
  else {
    (*(WndBase * *)&dlg_defaults.parent_window)->focus_and_raise();
  }
  if (radio_group != 0) {
    if ((RadioGroup *)radio_group != (RadioGroup *)0x0) {
      ((RadioGroup *)radio_group)->scalar_destruct(1);
    }
    radio_group = 0;
  }
  if (checkboxes != 0) {
    if ((Checkbox *)checkboxes != (Checkbox *)0x0) {
      ((Checkbox *)checkboxes)->vector_destruct(3);
    }
    checkboxes = 0;
  }
  if (edit_controls != 0) {
    if ((Button *)edit_controls != (Button *)0x0) {
      ((EditControl *)edit_controls)->vector_destruct(3);
    }
    edit_controls = 0;
  }
  if (button_controls != 0) {
    if ((Button *)button_controls != (Button *)0x0) {
      ((Button *)button_controls)->vector_destruct(3);
    }
    button_controls = 0;
  }
  if (listbox[0] != 0) {
    if ((ListBox *)listbox[0] != (ListBox *)0x0) {
      ((ListBox *)listbox[0])->scalar_destruct(1);
    }
    listbox[0] = 0;
  }
  if (listbox[1] != 0) {
    if ((ListBox *)listbox[1] != (ListBox *)0x0) {
      ((ListBox *)listbox[1])->scalar_destruct(1);
    }
    listbox[1] = 0;
  }
  if (sprite_hotspots != 0) {
    if ((Hotspot *)sprite_hotspots != (Hotspot *)0x0) {
      ((Hotspot *)sprite_hotspots)->vector_destruct(3);
    }
    sprite_hotspots = 0;
  }
  if (list_vscroll[0] != 0) {
    if ((Button *)list_vscroll[0] != (Button *)0x0) {
      ((Scrollbar *)list_vscroll[0])->scalar_destruct(1);
    }
    list_vscroll[0] = 0;
  }
  if (list_vscroll[1] != 0) {
    if ((Button *)list_vscroll[1] != (Button *)0x0) {
      ((Scrollbar *)list_vscroll[1])->scalar_destruct(1);
    }
    list_vscroll[1] = 0;
  }
  if (list_hscroll[0] != 0) {
    if ((Button *)list_hscroll[0] != (Button *)0x0) {
      ((Scrollbar *)list_hscroll[0])->scalar_destruct(1);
    }
    list_hscroll[0] = 0;
  }
  if (list_hscroll[1] != 0) {
    if ((Button *)list_hscroll[1] != (Button *)0x0) {
      ((Scrollbar *)list_hscroll[1])->scalar_destruct(1);
    }
    list_hscroll[1] = 0;
  }
  if (((style_flags & 0x10) == 0) && (surf_wnd != 0)) {
    if (surf_wnd != (SurfaceWindow *)0x0) {
      surf_wnd->SurfaceWindow::scalar_destruct(1);
    }
    surf_wnd = 0;
    style_flags = style_flags | 0x10;
  }
  if (this == dialog_state.active) {
    if ((dlg_style.stack_depth < 1) || (0x10 < dlg_style.stack_depth)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_popupstackindex_0_popupstackindex_17,(const char*)&str_d_ss_franklinton_popup_1_cpp,0x19c);
    }
    dlg_style.stack_depth--;
    (*(iptr *)&net.dialog_stack_top) = (((i32 *)&dlg_style.stack))[dlg_style.stack_depth];
    dialog_state.active = (Dialog *)(*(iptr *)&net.dialog_stack_top);
    dialog_state.timeout_start = timer_ticks();
  }
  return;
}

/* 0059df8a auto */
Dialog::~Dialog()

{
  close();
  memblock_free((MemBlock *)string_pool);
}

/* 0059dfb9 auto */
void Dialog::init(SurfaceWindow *surface_wnd,DibSurface *background,RECT *rect,u32 flags)

{
  i32 size;
  
  if ((style_flags & 0x20) == 0) {
    memblock_init((MemBlock *)string_pool,9,(ushort)pool_size);
  }
  else {
    close();
    init_defaults();
    reset_string_pool((uint)pool_size);
  }
  set_font((GameFont *)dlg_style.default_text_font);
  set_title_font(((GameFont *)(*(i8 * *)dlg_style.default_text_font_rest)));
  set_list_font(((GameFont *)*(i8 * *)(dlg_style.default_text_font_rest + 4)));
  surf_wnd = surface_wnd;
  bg_surface = (u32)background;
  result_id = 0;
  style_flags = flags;
  if (rect == 0) {
    set_width(0x1b8);
  }
  else {
    origin_x = (u32)rect->left;
    origin_y = (u32)rect->top;
    size = rect_width(rect);
    set_width(size);
    size = rect_height(rect);
    set_height(size);
  }
  if (surface_wnd != 0) {
    style_flags = style_flags | 0x10;
  }
  return;
}

/* 0059e0eb auto */
i32 Dialog::set_item_text(int line_id,const char *text)

{
  size_t len;
  DialogLine *line;
  
  for (line = line_list; (line->number != line_id && (line != (DialogLine *)0x0));
      line = line->next) {
  }
  if (((line != (DialogLine *)0x0) && (-1 < line->capacity)) &&
     (len = _strlen((char *)text), (int)(len + 1) <= line->capacity)) {
    crt_strcpy((char *)((void *)line->text),(const char *)((void *)text));
    return 1;
  }
  return 0;
}

/* 0059e18b auto */
DialogLine *Dialog::add_line(const char *text,int x,int y,int max_len,int color)

{
  DialogLine *line;
  size_t len;
  int buffer;
  DialogLine *last;
  DialogLine *cur;
  
  last = (DialogLine *)0x0;
  for (cur = line_list; cur != (DialogLine *)0x0; cur = cur->next) {
    last = cur;
  }
  line = (DialogLine *)memblock_alloc((MemBlock *)string_pool,0x20);
  if (last == (DialogLine *)0x0) {
    line_list = line;
    line->number = 1;
  }
  else {
    last->next = line;
    line->number = last->number + 1;
  }
  line->next = 0;
  line->flags = 0;
  if (*(char *)text == '^') {
    if (*(char *)(text + 1) == '^') {
      line->flags = (int)line->flags | 1;
      text += 2;
    }
    else {
      line->flags = (int)line->flags | 2;
      text++;
    }
  }
  if ((0 < max_len) && (len = _strlen((char *)text), (int)(len + 1) < max_len)) {
    buffer = memblock_alloc((MemBlock *)string_pool,max_len + 1);
    line->text = (char *)buffer;
    line->capacity = max_len + 1;
  }
  else {
    len = _strlen((char *)text);
    buffer = memblock_alloc((MemBlock *)string_pool,len + 1);
    line->text = (char *)buffer;
    len = _strlen((char *)text);
    line->capacity = len + 1;
  }
  crt_strcpy((char *)((void *)line->text),(const char *)((void *)text));
  line->x = x;
  line->y = y;
  line->color = color;
  line_count++;
  return line;
}

/* 0059e356 auto */
i32 Dialog::option_icon_width()

{
  return 0x20;
}

/* 0059e376 auto */
int Dialog::dialog_option_row_height()

{
  int spacing;
  int result;
  int icon_width;
  
  icon_width = option_icon_width();
  spacing = ((GameFont *)body_font)->line_spacing();
  if (spacing * 5 >> 2 <= icon_width) {
    spacing = ((GameFont *)body_font)->line_spacing();
    icon_width = spacing * 5 >> 2;
  }
  spacing = ((GameFont *)body_font)->line_spacing();
  if (spacing < icon_width) {
    result = icon_width;
  }
  else {
    result = ((GameFont *)body_font)->line_spacing();
  }
  return result;
}

/* 0059e3fa auto */
u32 Dialog::dialog_list_row_height()

{
  int font_height;
  u32 result;
  
  if ((this)->list_row_height_override[cur_pane] == 0) {
    font_height = ((GameFont *)list_font)->font_cached_height();
    result = font_height + 1;
  }
  else {
    result = (this)->list_row_height_override[cur_pane];
  }
  return result;
}

/* 0059e448 auto */
int Dialog::edit_row_height()

{
  int spacing;
  
  spacing = ((GameFont *)body_font)->line_spacing();
  return spacing + 10;
}

/* 0059e472 auto */
void Dialog::set_font(GameFont *font)

{
  u32 row_height;
  
  body_font = (uptr)font;
  row_height = dialog_option_row_height();
  option_row_height = row_height;
  return;
}

/* 0059e4a4 auto */
/* Library Function - Single Match
    public: void __thiscall CArchive::SetObjectSchema(unsigned int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_title_font(GameFont *font)

{
  title_font = (uptr)font;
  return;
}

/* 0059e4c5 auto */
void Dialog::set_list_font(GameFont *font)

{
  list_font = (uptr)font;
  return;
}

/* 0059e4e6 auto */
void Dialog::set_columns(u32 column_count)

{
  columns = column_count;
  return;
}

/* 0059e507 auto */
void Dialog::set_list_rows(u32 rows)

{
  int row_height;
  
  style_flags = style_flags | 0x1000;
  list_rows[cur_pane] = rows;
  if ((style_flags & 0x40000) == 0) {
    row_height = dialog_list_row_height();
    list_height[cur_pane] = row_height * rows + 2;
  }
  else {
    row_height = ((GameFont *)((i32 *)&default_draw_font_obj_block.default_draw_font_obj))->font_cached_height();
    list_height[cur_pane] = row_height * rows + 2;
  }
  return;
}

/* 0059e585 auto */
void Dialog::set_current_pane(u32 pane)

{
  if ((pane == 0) || (pane == 1)) {
    cur_pane = pane;
  }
  else {
    cur_pane = 0;
  }
  return;
}

/* 0059e5c9 auto */
void Dialog::set_listbox(int rows,u32 width,u32 list_flags)

{
  style_flags = style_flags | 0x1000;
  list_width[cur_pane] = width;
  (this)->list_x_offset[cur_pane] = list_flags;
  if (rows != 0) {
    set_list_rows(rows);
  }
  return;
}

/* 0059e648 auto */
int Dialog::line_height()

{
  int spacing;
  
  spacing = ((GameFont *)body_font)->line_spacing();
  return spacing + 4 + pad_y * 2;
}

/* 0059e676 auto */
int Dialog::button_width_for(const char *text)

{
  u32 padding;
  int text_width;
  
  padding = sprite_gap;
  text_width = ((GameFont *)((i8 *)&dialog_plain_font_obj_block.dialog_plain_font_obj))->text_width(text);
  return text_width + 4 + padding * 4;
}

/* 0059e6a9 auto */
void Dialog::set_title(const char *title)

{
  size_t len;
  uptr buffer;
  
  len = _strlen((char *)title);
  buffer = memblock_alloc((MemBlock *)&(this)->string_pool,len + 1);
  title_text = buffer;
  crt_strcpy((char *)((void *)title_text),(const char *)((void *)title));
  return;
}

/* 0059e6ff auto */
void Dialog::set_width(u32 width)

{
  if ((scale_num != scale_den) && ((style_flags & 0x80000) == 0)) {
    width = (int)(scale_num * width) / (int)scale_den;
  }
  content_width = width;
  return;
}

/* 0059e762 auto */
/* Library Function - Single Match
    public: void __thiscall ios::delbuf(int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_height(u32 height)

{
  min_height = height;
  return;
}

/* 0059e783 auto */
void Dialog::set_position(u32 x,u32 y)

{
  req_x = x;
  req_y = y;
  return;
}

/* 0059e7ad auto */
DialogOption *Dialog::find_option(int option_id)

{
  DialogOption *cur;
  DialogOption *found;
  
  found = 0;
  cur = option_list;
  while ((cur != 0 && (found == 0))) {
    if (cur->id == option_id) {
      found = cur;
    }
    cur = cur->next;
  }
  return found;
}

/* 0059e877 auto */
DialogSprite *Dialog::find_sprite(int sprite_id)

{
  DialogSprite *cur;
  DialogSprite *found;
  
  found = (DialogSprite *)0x0;
  cur = sprite_list;
  while ((cur != (DialogSprite *)0x0 && (found == (DialogSprite *)0x0))) {
    if (cur->id == sprite_id) {
      found = cur;
    }
    cur = cur->next;
  }
  return found;
}

/* 0059e8db auto */
void Dialog::set_option_disabled(int option_id,int disabled)

{
  DialogOption *option;
  
  option = find_option(option_id);
  if (option != (DialogOption *)0x0) {
    if (disabled == 0) {
      option->flags = option->flags & 0xfffffffe;
    }
    else {
      option->flags = option->flags | 1;
    }
  }
  return;
}

/* 0059e9f3 auto */
i32 Dialog::option_is_checked(i32 option_id)

{
  DialogOption *option;
  i32 checked;
  
  checked = 0;
  option = find_option(option_id);
  if (option != (DialogOption *)0x0) {
    if ((option->flags_byte & 4) == 0) {
      checked = 0;
    }
    else {
      checked = 1;
    }
  }
  return checked;
}

/* 0059ea4d auto */
void Dialog::set_option_checked(i32 option_id,int checked)

{
  DialogOption *option;
  
  option = find_option(option_id);
  if (option != (DialogOption *)0x0) {
    if (checked == 0) {
      option->flags = option->flags & 0xfffffffb;
    }
    else {
      option->flags = option->flags | 4;
    }
  }
  return;
}

/* 0059ea99 auto */
void Dialog::set_default_choice(int choice_id)

{
  uptr entry;
  
  if (option_count == 0) {
    entry = (uptr)find_sprite(choice_id);
    if (entry != 0) {
      selected_sprite = (DialogSprite *)entry;
    }
  }
  else {
    entry = (uptr)find_option(choice_id);
    if (entry != 0) {
      selected_option = (DialogOption *)entry;
    }
  }
  return;
}

/* 0059eb8c auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_sprite_click_fn(DialogSpriteFn callback)

{
  on_sprite_click = callback;
  return;
}

/* 0059ebb0 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_sprite_dblclick_fn(DialogSpriteFn callback)

{
  on_sprite_dblclick = callback;
  return;
}

/* 0059ebd4 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_sprite_draw_fn(DialogSpriteDrawFn callback)

{
  (this)->on_draw_sprite = callback;
  return;
}

/* 0059ebf8 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_option_draw_fn(DialogOptionDrawFn callback)

{
  on_draw_option = callback;
  return;
}

/* 0059ec1c auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_select_change_fn(DialogSelectFn callback)

{
  on_select_change = callback;
  return;
}

/* 0059ec64 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_image_zoom(u32 zoom)

{
  sprite_zoom = zoom;
  return;
}

/* 0059ec88 auto */
DialogSprite *Dialog::add_sprite(Sprite *sprite,int sprite_id,const char *label)

{
  DialogSprite *entry;
  size_t len;
  iptr label_buf;
  int label_width;
  u32 max_width;
  DialogSprite *last;
  DialogSprite *cur;
  u32 entry_width;
  
  last = 0;
  for (cur = sprite_list; cur != 0; cur = cur->next) {
    last = cur;
  }
  entry = (DialogSprite *)memblock_alloc((MemBlock *)string_pool,0x14);
  if (last == 0) {
    sprite_list = entry;
    if (selected_sprite == 0) {
      selected_sprite = entry;
    }
  }
  else {
    last->next = (DialogSprite *)((iptr *)entry);
  }
  entry->next = 0;
  entry->field_04 = 0;
  entry->sprite = sprite;
  entry_width = (sprite)->width();
  if (label == 0) {
    entry->label = 0;
  }
  else {
    len = _strlen((char *)label);
    label_buf = memblock_alloc((MemBlock *)string_pool,len + 1);
    entry->label = (char *)label_buf;
    crt_strcpy((char *)((void *)entry->label),(const char *)((void *)label));
    label_width = ((GameFont *)body_font)->text_width((const char *)entry->label);
    entry_width = entry_width + sprite_gap + label_width;
  }
  entry->id = sprite_id;
  max_width = max_sprite_width;
  if ((int)max_sprite_width <= (int)entry_width) {
    max_width = entry_width;
  }
  max_sprite_width = max_width;
  sprite_count++;
  return entry;
}

/* 0059edf0 auto */
DialogOption *Dialog::add_option(const char *text,int option_id,uint data)

{
  u32 width;
  int cmp;
  DialogOption *option;
  size_t len;
  uint text_buf;
  int icon_width;
  u32 max_width;
  DialogOption *prev;
  DialogOption *cur;
  
  prev = 0;
  cur = option_list;
  if (((style_flags & 0x800000) == 0) || (cur == 0)) {
    if (cur != 0) {
      for (; cur != 0; cur = cur->next) {
        prev = cur;
      }
    }
  }
  else {
    while ((cur != 0 && (cmp = __strcmpi((char *)text,cur->text), 0 < cmp))
          ) {
      prev = cur;
      cur = cur->next;
    }
  }
  option = (DialogOption *)memblock_alloc((MemBlock *)string_pool,0x18);
  if (prev == 0) {
    option->next = option_list;
    option->prev = 0;
    if (option_list == 0) {
      option_tail = option;
    }
    else {
      option_list->prev = option;
    }
    option_list = option;
    selected_option = option;
  }
  else {
    option->next = prev->next;
    option->prev = prev;
    if (option->next == 0) {
      option_tail = option;
    }
    else {
      *(uint **)&option->next->prev = (uint *)option;
    }
    prev->next = (DialogOption *)((uint *)option);
  }
  option->flags = 0;
  len = _strlen((char *)text);
  text_buf = memblock_alloc((MemBlock *)string_pool,len + 1);
  option->text = (char *)text_buf;
  crt_strcpy((char *)((void *)option->text),(const char *)((void *)text));
  if (*(char *)text == '\0') {
    option->flags = option->flags | 1;
  }
  option->id = option_id;
  option->group = data;
  cmp = ((GameFont *)body_font)->text_width((const char *)option->text);
  icon_width = option_icon_width();
  width = cmp + icon_width + option_extra_width + 5;
  max_width = option_width;
  if ((int)option_width <= (int)width) {
    max_width = width;
  }
  option_width = max_width;
  option_count++;
  return option;
}

/* 0059f026 auto */
DialogOption *Dialog::add_checkbox_option(const char *text,int option_id,int checked)

{
  DialogOption *option;
  
  style_flags = style_flags | 5;
  option = add_option(text,option_id,0);
  if (checked != 0) {
    option->flags = option->flags | 4;
  }
  return option;
}

/* 0059f06d auto */
DialogEditField *Dialog::add_edit_field(const char *label,const char *initial_text,size_t max_len)

{
  DialogEditField *field;
  size_t len;
  i32 buffer;
  u32 label_width;
  int char_width;
  u32 max_width;
  DialogEditField *last;
  DialogEditField *cur;
  u32 field_width;
  
  last = 0;
  for (cur = edit_field_list; cur != 0; cur = cur->next) {
    last = cur;
  }
  field = (DialogEditField *)memblock_alloc((MemBlock *)string_pool,0x1c);
  if (last == 0) {
    edit_field_list = field;
  }
  else {
    last->next = (DialogEditField *)((i32 *)field);
  }
  field->next = 0;
  field->field_00 = 0;
  len = _strlen((char *)label);
  buffer = memblock_alloc((MemBlock *)string_pool,len + 2);
  field->label = (char *)buffer;
  crt_strcpy((char *)((void *)field->label),(const char *)((void *)label));
  label_width = ((GameFont *)body_font)->text_width((const char *)field->label);
  max_width = edit_label_width;
  if ((int)edit_label_width <= (int)label_width) {
    max_width = label_width;
  }
  edit_label_width = max_width;
  /* BF-110 (Peter Johnson's ledger): a length that wrapped negative (a
     GAME.TXT @LENGTH) is the longest too (the original copied that many) */
  if ((0xfe < (int)max_len) || ((int)max_len < 0)) {
    max_len = 0xff;
  }
  field->max_len = max_len;
  buffer = memblock_alloc((MemBlock *)string_pool,0x100);
  field->buffer = (char *)buffer;
  char_width = ((GameFont *)(*(i8 * *)(draw_target.font_rest + 12)))->text_width((const char *)&str_w);
  field->width = char_width * max_len;
  field_width = field->width + edit_label_width;
  if ((content_width != 0) && ((int)content_width < (int)field_width)) {
    field->width -= ((field_width - content_width) - 3);
    field_width = field->width + edit_label_width + 3;
  }
  max_width = edit_width;
  if ((int)edit_width <= (int)field_width) {
    max_width = field_width;
  }
  edit_width = max_width;
  if (initial_text == 0) {
    *field->buffer = 0;
  }
  else {
    _memset((void *)field->buffer,0,0x100);
    _strncpy(field->buffer,(char *)initial_text,max_len);
  }
  field->index = edit_count;
  edit_count++;
  return field;
}

/* 0059f2a3 auto */
void Dialog::add_button(const char *label)

{
  size_t len;
  uptr buffer;
  
  if ((int)extra_button_count < 6) {
    len = _strlen((char *)label);
    buffer = memblock_alloc((MemBlock *)&(this)->string_pool,len + 1);
    extra_button_labels[extra_button_count] = (char *)buffer;
    crt_strcpy(extra_button_labels[extra_button_count],(const char *)((void *)label));
    extra_button_count++;
  }
  return;
}

/* 0059f31a auto */
void Dialog::set_text_color(int highlighted,int use_alt,int shadow)

{
  u32 normal_color;
  u32 alt_color;
  u32 hilite_color;
  
  if (highlighted == 0) {
    if (use_alt == 0) {
      if (shadow == 0) {
        normal_color = text_color;
      }
      else {
        normal_color = shadow_color;
      }
      gfx_set_text_color_index(normal_color);
    }
    else {
      if (shadow == 0) {
        alt_color = frame_color;
      }
      else {
        alt_color = shadow_color;
      }
      gfx_set_text_color_index(alt_color);
    }
  }
  else {
    if (shadow == 0) {
      hilite_color = highlight_color;
    }
    else {
      hilite_color = shadow_color;
    }
    gfx_set_text_color_index(hilite_color);
  }
  return;
}

/* 0059f3d7 auto */
int Dialog::draw_text_shadowed(GameFont *font,char *text,int x,int y,uint flags)

{
  char *underscore;
  i32 use_alt;
  int text_width;
  i32 shadow;
  
  while (underscore = _strchr(text,0x5f), underscore != (char *)0x0) {
    *underscore = ' ';
  }
  if (shadow_color != text_color) {
    shadow = 1;
    use_alt = uses_alt_text_color();
    set_text_color(flags & 1,use_alt,shadow);
    ((DibSurface *)surf_wnd)->draw_text(font,(const char *)text,x + 1,y + 1,5);
  }
  shadow = 0;
  use_alt = uses_alt_text_color();
  set_text_color(flags & 1,use_alt,shadow);
  ((DibSurface *)surf_wnd)->draw_text(font,(const char *)text,x,y,5);
  text_width = (font)->text_width((const char *)text);
  return x + text_width;
}

/* 0059f5ba auto */
void Dialog::draw_text_line(const char *text,int x_offset,i32 y)

{
  draw_text_default_font(text,text_x + x_offset,y,0);
  return;
}

/* 0059f5f7 auto */
i32 Dialog::has_text_lines()

{
  DialogLine *line;
  
  line = line_list;
  while( true ) {
    if (line == 0) {
      return 0;
    }
    if (line->x < 0) break;
    line = line->next;
  }
  return 1;
}

/* 0059f64a auto */
int Dialog::draw_wrapped_text(int draw)

{
  u32 max_width;
  int text_width;
  uptr remaining;
  char *space;
  uptr word_len;
  int buf_width;
  int center_x;
  bool has_shadow;
  u32 line_y;
  char *src;
  int total_height;
  u32 y2;
  int x2;
  char line_buf[256];
  char word_buf[0x100];  /* BF-052 (Peter Johnson's ledger): the original's 80,
                           less than a word of a line; longer words and lines
                           are cut */
  int line_width;
  DialogLine *line;
  int line_height;
  
  has_shadow = shadow_color != text_color;
  line = line_list;
  max_width = content_width;
  total_height = 0;
  y2 = text_y;
  line_height = ((GameFont *)body_font)->line_spacing();
  line_buf[0] = '\0';
  line_width = 0;
  for (; center_x = (int)max_width >> 1, line != 0; line = line->next) {
    if (line->x < 1) {
      src = line->text;
      if ((line->flags_byte & 3) != 0) {
        if (line_buf[0] != '\0') {
          if (draw != 0) {
            draw_text_line((const char *)line_buf,0,y2);
          }
          total_height += line_height;
          y2 += line_height;
          line_buf[0] = '\0';
          line_width = 0;
        }
        if (draw != 0) {
          if ((line->flags_byte & 1) == 0) {
            x2 = 0;
          }
          else if (line->color == 0) {
            text_width = ((GameFont *)body_font)->text_width((const char *)src);
            x2 = center_x - ((int)((uint)has_shadow + text_width) >> 1);
          }
          else {
            x2 = line->color;
            text_width = ((GameFont *)body_font)->text_width((const char *)src);
            x2 -= ((int)((uint)has_shadow + text_width) >> 1);
          }
          if (line->y == -1) {
            line_y = y2;
          }
          else {
            line_y = (u32)line->y;
          }
          draw_text_line((const char *)src,x2,line_y);
        }
        for (; *src != '\0'; src++) {
        }
        if (line->y < 0) {
          total_height += line_height;
          y2 += line_height;
        }
      }
      while (remaining = _strlen(src), remaining != 0) {
        for (; *src == ' '; src++) {
        }
        space = _strchr(src,0x20);
        if (space != (char *)0x0) {
          *space = '\0';
        }
        word_len = _strlen(src);
        word_buf[0] = '\0';
        if (line_buf[0] != '\0') {
          crt_strcat(word_buf,((i8 *)&dialog_wrap_space_str));
        }
        str_append_max(word_buf,src,sizeof word_buf);
        text_width = ((GameFont *)body_font)->text_width((const char *)word_buf);
        if ((int)max_width < (int)((uint)has_shadow + text_width + line_width)) {
          if (draw != 0) {
            if ((style_flags & 0x200000) == 0) {
              x2 = 0;
            }
            else {
              buf_width = ((GameFont *)body_font)->text_width((const char *)line_buf);
              x2 = center_x - ((int)((uint)has_shadow + buf_width) >> 1);
            }
            draw_text_line((const char *)line_buf,x2,y2);
          }
          total_height += line_height;
          y2 += line_height;
          while (word_buf[0] == ' ') {
            crt_strcpy(word_buf,(word_buf + 1));
          }
          line_buf[0] = '\0';
          line_width = 0;
        }
        str_append_max(line_buf,word_buf,sizeof line_buf);
        line_width += text_width;
        if (space != (char *)0x0) {
          *space = ' ';
        }
        src += word_len;
      }
    }
    else if (draw != 0) {
      draw_text_default_font((const char *)line->text,line->x,
                 line->y,(i32)line->flags);
    }
  }
  if (line_buf[0] != '\0') {
    if (draw != 0) {
      if ((style_flags & 0x200000) == 0) {
        x2 = 0;
      }
      else {
        text_width = ((GameFont *)body_font)->text_width((const char *)line_buf);
        x2 = center_x - ((int)((uint)has_shadow + text_width) >> 1);
      }
      draw_text_line((const char *)line_buf,x2,y2)
      ;
    }
    total_height += line_height;
  }
  return total_height;
}

/* 0059fb78 auto */
i32 Dialog::option_group_index(DialogOption *option)

{
  int index;
  DialogOption *cur;
  
  if (option_list != 0) {
    index = 0;
    for (cur = option_list; cur != 0; cur = cur->next) {
      if ((option->group == cur->group) && ((uptr)option == (uptr)cur)) {
        return index;
      }
      if (option->group == cur->group) {
        index++;
      }
    }
  }
  return 0;
}

/* 0059fc19 auto */
DialogOption *Dialog::option_at(int target_index)

{
  int index;
  DialogOption *cur;
  
  if (option_list != 0) {
    index = 0;
    for (cur = option_list; cur != 0; cur = cur->next) {
      if ((cur_pane == (u32)cur->group) && (index == target_index)) {
        return cur;
      }
      if (cur_pane == (u32)cur->group) {
        index++;
      }
    }
  }
  return 0;
}

/* 0059fcba auto */
int Dialog::column_of_option(DialogOption *option)

{
  int group_index;
  
  group_index = option_group_index(option);
  return group_index / (int)list_rows[cur_pane];
}

/* Part of Dialog::compute_layout (0059fd2a): the sprites' column: moves the text, list
   and edit fields right of it (or widens the content for labelled sprites);
   returns the sprites' total height. */
int Dialog::layout_sprites()
{
  i32 height;
  u32 tmp;
  int value;
  int sprites_height;
  DialogSprite *sprite;
  u32 sprite_width2;

  sprites_height = 0;
  if (sprite_list != 0) {
    sprite_width2 = gfx_zoom_scale(max_sprite_width,sprite_zoom);
    if (sprite_list->label == 0) {
      text_x += sprite_width2;
      list_x += sprite_width2;
      edit_x += sprite_width2;
      wnd_width += sprite_width2;
    }
    else {
      tmp = content_width;
      if ((int)content_width <= (int)sprite_width2) {
        tmp = sprite_width2;
      }
      content_width = tmp;
    }
    for (sprite = sprite_list; sprite != 0; sprite = sprite->next) {
      tmp = sprite_zoom;
      height = sprite->sprite->height();
      value = gfx_zoom_scale(height,tmp);
      sprites_height += value;
      if (sprite->next != 0) {
        sprites_height += section_gap;
      }
    }
  }
  return sprites_height;
}

/* Part of Dialog::compute_layout (0059fd2a): the geometry of a list pane (pane 0, or pane 1
   of a two-pane list): its scroll bar, client rect, column width, row height
   and the top option that shows the selection.  The two panes' code is the
   same but for the index and how lists_height follows a pane grown for a
   horizontal scroll bar (pane 0: its height, pane 1: the larger). */
void Dialog::layout_list_pane(int pane,u32 &lists_height)
{
  u32 tmp;
  int value;
  int top_index;
  DialogOption *first_option;

  if ((style_flags & 0x40000) == 0) {
    tmp = GetSystemMetrics(2);
    list_vscroll_width[pane] = tmp;
    tmp = GetSystemMetrics(3);
    list_hscroll_height[pane] = tmp;
    rect_set_xywh(&pane_rect[pane],list_x_offset[pane] + list_x,
                       list_y,list_width[pane],list_height[pane]);
    list_client_rect[pane].left = pane_rect[pane].left;
    list_client_rect[pane].top = pane_rect[pane].top;
    list_client_rect[pane].right = pane_rect[pane].right;
    list_client_rect[pane].bottom = pane_rect[pane].bottom;
    if (((int)(list_rows[pane] * columns) < (int)option_count) ||
       ((style_flags & 0x1000000) != 0)) {
      if (columns == 1) {
        rect_set_xywh(&list_vscroll_rect[pane],pane_rect[pane].right - list_vscroll_width[pane],
                           list_y,list_vscroll_width[pane],list_height[pane]);
        list_client_rect[pane].right -= list_vscroll_width[pane];
      }
      else {
        list_height[pane] += list_hscroll_height[pane];
        pane_rect[pane].bottom += list_hscroll_height[pane];
        list_client_rect[pane].bottom += list_hscroll_height[pane];
        if (pane == 0) {
          lists_height = list_height[0];
        }
        else if ((int)lists_height < (int)list_height[1]) {
          lists_height = list_height[1];
        }
        rect_set_xywh(&list_hscroll_rect[pane],list_x_offset[pane] + list_x,
                           pane_rect[pane].bottom - list_hscroll_height[pane],list_width[pane],list_hscroll_height[pane])
        ;
        list_client_rect[pane].bottom -= list_hscroll_height[pane];
      }
    }
    rect_deflate(&list_client_rect[pane],1,1);
    list_origin_x[pane] = list_client_rect[pane].left;
    list_origin_y[pane] = list_client_rect[pane].top;
    value = rect_width(&list_client_rect[pane]);
    list_column_width[pane] = value / (int)columns;
    tmp = dialog_list_row_height();
    list_row_height[pane] = tmp;
    first_option = option_at(0);
    top_index = option_group_index(first_option);
    value = option_group_index(selected_option);
    while ((int)(list_rows[pane] * columns + top_index) <= value) {
      if (columns == 1) {
        top_index++;
      }
      else {
        top_index += list_rows[pane];
      }
    }
    tmp = (u32)option_at(top_index);
    list_top_option[pane] = (DialogOption *)tmp;
  }
}

/* Part of Dialog::compute_layout (0059fd2a): the buttons: their width (the widest label,
   or an equal share of the window's width) and x positions, right to left,
   the remainder spread one pixel at a time. */
void Dialog::layout_buttons(int sprites_height,int content_height)
{
  u32 pos;
  int value;
  u32 tmp;
  int button_x;
  int remainder_acc;
  int extra_space;
  u32 avail_width;
  uptr i;

  button_x = wnd_width - (border_width - bevel_width);
  button_width = 0;
  for (i = 0; (int)i < 3; i++) {
    value = button_width_for((const char *)(((i8 * *)&dlg_style.button_labels))[i]);
    if ((int)button_width <= value) {
      tmp = button_width_for((const char *)(((i8 * *)&dlg_style.button_labels))[i]);
      button_width = tmp;
    }
  }
  for (i = 0; (int)i < (int)extra_button_count; i++) {
    value = button_width_for(extra_button_labels[i]);
    if ((int)button_width <= value) {
      tmp = button_width_for(extra_button_labels[i]);
      button_width = tmp;
    }
  }
  if (bugfix_ui()) {
    /* B20: buttons wider than the window: it grows to hold them, instead of
       the leftmost going past its left edge (docs/BUGS.md) */
    value = (std_button_count + extra_button_count) * button_width +
            (std_button_count + extra_button_count - 1) * pad_x + (border_width - bevel_width) * 2;
    if ((int)wnd_width < value) {
      wnd_width = value;
      button_x = wnd_width - (border_width - bevel_width);
    }
  }
  value = (wnd_width + (border_width - bevel_width) * -2) -
          (std_button_count + extra_button_count - 1) * pad_x;
  extra_space = value % (int)(std_button_count + extra_button_count);
  avail_width = value / (int)(std_button_count + extra_button_count);
  tmp = button_width;
  if ((int)button_width <= (int)avail_width) {
    tmp = avail_width;
  }
  button_width = tmp;
  if (button_width != avail_width) {
    extra_space = 0;
  }
  if ((style_flags & 0x100000) == 0) {
    if ((sprites_height != 0) || (content_height != 0)) {
      wnd_height += section_gap * 2;
    }
    value = line_height();
    wnd_height += value;
    if (border_width != 0) {
      if ((sprites_height != 0) || (content_height != 0)) {
        chrome_height += section_gap * 2;
      }
      value = line_height();
      chrome_height += value;
    }
  }
  remainder_acc = std_button_count + extra_button_count - 1;
  tmp = std_button_count;
  while (i = tmp - 1, -1 < (int)i) {
    pos = button_x - button_width;
    buttons[i].x = pos;
    button_x = pos - pad_x;
    for (remainder_acc = remainder_acc + extra_space; tmp = i,
        (int)(std_button_count + extra_button_count) <= remainder_acc;
        remainder_acc = remainder_acc - (std_button_count + extra_button_count)) {
      button_x--;
    }
  }
  tmp = extra_button_count;
  while (i = tmp - 1, -1 < (int)i) {
    pos = button_x - button_width;
    buttons[std_button_count + i].x = pos;
    button_x = pos - pad_x;
    for (remainder_acc = remainder_acc + extra_space; tmp = i,
        (int)(std_button_count + extra_button_count) <= remainder_acc;
        remainder_acc = remainder_acc - (std_button_count + extra_button_count)) {
      button_x--;
    }
  }
}

/* Part of Dialog::compute_layout (0059fd2a): places the window on the screen (centered, or
   negative positions counted from the right/bottom edge) and moves the
   text, list and edit positions by the origin. */
void Dialog::layout_place_window()
{
  int value;
  RECT frame_rect2;
  RECT screen_rect2;

  if ((style_flags & 0x100000) == 0) {
    gfx_get_screen_rect(&screen_rect2);
    main_frame_wnd_block.main_frame_wnd.get_screen_rect(((RECT *)(i8 *)&frame_rect2.left));
    if (window_x == 0xfffffc19) {
      value = rect_width((RECT *)(iptr)((i8 *)&screen_rect2.left));
      window_x = (value >> 1) - ((int)wnd_width >> 1);
    }
    else if ((((int)window_x < 0) &&
             (window_x = window_x + ((screen_rect2.right - 1) - wnd_width),
             main_frame_created != 0)) && ((style_flags & 0x10000) != 0)) {
      value = main_frame_wnd_block.main_frame_wnd.frame_width();
      window_x -= value;
    }
    if (window_y == 0xfffffc19) {
      value = rect_height((RECT *)(iptr)((i8 *)&screen_rect2.left));
      window_y = (value >> 1) - ((int)wnd_height >> 1);
    }
    else if ((int)window_y < 0) {
      if (-(screen_rect2.bottom - frame_rect2.bottom) < (int)window_y) {
        window_y -= (screen_rect2.bottom - frame_rect2.bottom);
      }
      window_y += ((screen_rect2.bottom - 1) - wnd_height);
      if ((main_frame_created != 0) && ((style_flags & 0x10000) != 0)) {
        value = main_frame_wnd_block.main_frame_wnd.nonclient_height();
        window_y -= value;
      }
    }
    if ((dialog_clamp_overflow == 0) &&
       (value = rect_width((RECT *)(iptr)((i8 *)&screen_rect2.left)), (int)wnd_width <= value)) {
      rect_height((RECT *)(iptr)((i8 *)&screen_rect2.left));
    }
    list_x += origin_x;
    list_y += origin_y;
    text_x += origin_x;
    text_y += origin_y;
    edit_x += origin_x;
    edit_y += origin_y;
  }
}

/* 0059fd2a auto */
i32 Dialog::compute_layout()

{
  u32 pos;
  int value;
  size_t len;
  u32 tmp;
  uptr column_count;
  int text_height;
  u32 text_width;
  i32 result;
  int rows;
  u32 left;
  u32 top;
  int sprites_height;
  int edit_height;
  DialogSprite *sprite;
  uptr i;
  DialogLine *line;
  u32 lists_height;
  int content_height;
  
  result = 1;
  text_height = 0;
  lists_height = 0;
  edit_height = 0;
  if ((style_flags & 0x100000) == 0) {
    for (line = line_list; line != 0; line = line->next) {
      if ((line->flags_byte & 3) != 0) {
        tmp = shadow_color;
        pos = text_color;
        value = ((GameFont *)body_font)->text_width((const char *)line->text);
        text_width = (uint)(tmp != pos) + value;
        tmp = content_width;
        if ((int)content_width <= (int)text_width) {
          tmp = text_width;
        }
        content_width = tmp;
      }
    }
  }
  if (((title_text != 0) && (title_font != 0)) &&
     (len = _strlen((char *)title_text), len != 0)) {
    value = ((GameFont *)title_font)->text_width((const char *)title_text);
    text_width = value + 4;
    tmp = content_width;
    if ((int)content_width <= (int)text_width) {
      tmp = text_width;
    }
    content_width = tmp;
  }
  for (sprite = sprite_list; sprite != 0; sprite = sprite->next) {
  }
  if ((edit_count != 0) && (option_count != 0)) {
    edit_count = 0;
    edit_field_list = 0;
  }
  if ((style_flags & 0x10000) == 0) {
    value = ((GameFont *)title_font)->font_cached_height();
    title_height = value + bevel_width * 2 + pad_y * 2;
  }
  else {
    border_width = 0;
    bevel_width = 0;
    title_height = 0;
  }
  chrome_height = title_height;
  window_x = req_x;
  window_y = req_y;
  left = border_width + pad_x;
  top = title_height + pad_y;
  if ((style_flags & 0x100000) == 0) {
    wnd_width = left * 2;
    wnd_height = pad_y * 2 + title_height;
    text_x = left;
    text_y = top;
    list_x = left;
    list_y = top;
    edit_x = left;
    edit_y = top;
    sprite_x = left;
    sprite_y = top;
    tmp = option_width;
    if ((int)option_width <= (int)content_width) {
      tmp = content_width;
    }
    if ((int)tmp <= (int)edit_width) {
      tmp = edit_width;
    }
    content_width = tmp;
    edit_width = content_width;
    option_width = edit_width;
  }
  std_button_count = 0;
  if ((style_flags & 0x40) != 0) {
    buttons[std_button_count].kind = DLG_BUTTON_HELP;
    std_button_count++;
  }
  if (((style_flags & 8) == 0) && ((style_flags & 0x2000000) == 0)) {
    buttons[std_button_count].kind = DLG_BUTTON_OK;
    std_button_count++;
  }
  if ((style_flags & 1) != 0) {
    buttons[std_button_count].kind = DLG_BUTTON_CANCEL;
    std_button_count++;
  }
  if (edit_count + option_count + sprite_count + std_button_count + line_count == 0) {
    if (surf_wnd != 0) {
      text_x += origin_x;
      text_y += origin_y;
    }
  }
  else if ((option_count != 0) || ((style_flags & 0x1004) == 0)) {
    sprites_height = layout_sprites();
    if ((line_list != 0) && (value = has_text_lines(), value != 0)) {
      text_height = draw_wrapped_text(0);
      list_y = list_y + section_gap + text_height;
      edit_y = edit_y + section_gap + text_height;
    }
    if (option_list != 0) {
      column_count = columns;
      if ((int)column_count < 2) {
        column_count = 1;
      }
      rows = (int)((columns - 1) + option_count) / (int)column_count;
      lists_height = option_row_height * rows;
      if ((style_flags & 0x1000) != 0) {
        cur_pane = 0;
        lists_height = list_height[0];
        layout_list_pane(0,lists_height);
        if ((style_flags & 0x400000) != 0) {
          cur_pane = 1;
          if ((int)lists_height < (int)list_height[1]) {
            lists_height = list_height[1];
          }
          layout_list_pane(1,lists_height);
          cur_pane = 0;
        }
      }
    }
    if (edit_field_list != 0) {
      edit_height = edit_row_height();
      edit_height = edit_height * edit_count;
    }
    content_height = edit_height + lists_height + text_height;
    if (((content_height != 0) && (text_height != 0)) && (edit_height + lists_height != 0)) {
      content_height += section_gap;
    }
    if ((style_flags & 0x100000) == 0) {
      if ((sprites_height != 0) && (content_height != 0)) {
        text_x += sprite_gap;
        list_x += sprite_gap;
        edit_x += sprite_gap;
      }
      value = sprites_height;
      if (sprites_height <= content_height) {
        value = content_height;
      }
      wnd_height += value;
      wnd_width += content_width;
    }
    if (std_button_count + extra_button_count == 0) {
      wnd_height += border_width;
      chrome_height += border_width;
    }
    else {
      layout_buttons(sprites_height,content_height);
    }
    tmp = min_height;
    if ((int)min_height <= (int)wnd_height) {
      tmp = wnd_height;
    }
    wnd_height = tmp;
    tmp = wnd_height;
    value = line_height();
    button_y = tmp - (bevel_width + value + pad_y);
    layout_place_window();
    button_y += origin_y;
    for (i = 0; (int)i < (int)(std_button_count + extra_button_count);
        i = i + 1) {
      buttons[i].x = buttons[i].x + origin_x;
    }
    result = 0;
  }
  return result;
}

/* 005a0fea auto */
void Dialog::draw_option_cell(const char *text,int x,int y,int selected,int disabled)

{
  int col_width;
  int text_width;
  uptr text_len;
  u32 shadow_color2;
  u32 fg_color;
  
  if (plat_extras && strchr(text, EXTRAS_ICON)) {
    /* X30, X37: the text with icons, part by part */
    char part[256];
    while (*text != '\0') {
      size_t n = strcspn(text, EXTRAS_ICON_S);
      if (n >= sizeof part) n = sizeof part - 1;
      memcpy(part, text, n);
      part[n] = 0;
      if (n != 0) {
        draw_option_cell(part, x, y, selected, disabled);
        x += ((GameFont *)list_font)->text_width(part);
      }
      text += n;
      if (*text == EXTRAS_ICON && text[1] != '\0') {
        Sprite *icon = extras_icon(text[1]);
        if (icon != 0) {
          i8 icon_rect[16];
          int dy = ((int)list_row_height[cur_pane] - icon->get_height()) / 2;
          icon->draw((RECT *)icon_rect, (DibSurface *)surf_wnd, x, y + (dy > 0 ? dy : 0));
          x += icon->get_width() + 1;
        }
        text += 2;
      }
      else if (*text != '\0') text++;
    }
    return;
  }
  while (*(char *)text != '\0') {
    col_width = (int)list_column_width[cur_pane];
    text_width = ((GameFont *)list_font)->text_width(text);
    if (text_width <= col_width - 6) break;
    text_len = _strlen((char *)text);
    *(i8 *)((text_len - 1) + text) = 0;
  }
  if (selected == 0) {
    fg_color = list_fg_color;
    shadow_color2 = list_fg_shadow_color;
  }
  else {
    fg_color = sel_text_color;
    shadow_color2 = sel_shadow_color;
  }
  if (shadow_color2 != fg_color) {
    gfx_set_text_color_index(shadow_color2);
    ((DibSurface *)surf_wnd)->draw_text((GameFont *)list_font,text,x + 2,y + 1,5);
    gfx_set_text_color_index(fg_color);
    ((DibSurface *)surf_wnd)->draw_text((GameFont *)list_font,text,x + 1,y,5);
  }
  if (disabled == 0) {
    gfx_set_text_color_index(fg_color);
  }
  else {
    gfx_set_text_color_index(highlight_color);
  }
  ((DibSurface *)surf_wnd)->draw_text((GameFont *)list_font,text,x,y,5);
  return;
}

/* 005a1148 auto */
void Dialog::draw_option_text(char *text,int x,i32 y,i32 selected, i32 disabled)

{
  int col_width;
  char *separator;
  int right_width;
  char *right_text;
  
  right_text = (char *)0x0;
  separator = _strchr(text,0x7c);
  if (separator != (char *)0x0) {
    right_text = separator + 1;
    *separator = '\0';
  }
  draw_option_cell((const char *)text,(int)list_indents[cur_pane] + x
                     ,y,selected,disabled);
  if (right_text != (char *)0x0) {
    col_width = (int)list_column_width[cur_pane];
    if (plat_extras && strchr(right_text, EXTRAS_ICON))
      right_width = extras_rich_width(list_font, right_text);   /* X30: with icons */
    else
    right_width = ((GameFont *)list_font)->text_width((const char *)right_text);
    draw_option_cell((const char *)right_text,(col_width + x - 4) - right_width,y,selected,disabled);
    *separator = '|';
  }
  return;
}

/* 005a120b auto */
uint Dialog::draw_option(DialogOption *option,int blit)

{
  int rel_index;
  int group_index;
  int visible_count;
  RECT inner_rect;
  int row;
  int cell_y;
  int col;
  int cell_x;
  u32 fill_color;
  uint is_selected;
  RECT cell_rect;
  
  if (option == 0) {
    is_selected = 0;
  }
  else if (option_count == 0) {
    is_selected = 0;
  }
  else {
    rel_index = option_group_index(list_top_option[option->group]);
    group_index = option_group_index(option);
    rel_index = group_index - rel_index;
    if ((rel_index < 0) ||
       (visible_count = list_rows[option->group] * columns,
       visible_count - rel_index == 0 || visible_count < rel_index)) {
      is_selected = 0;
    }
    else {
      col = rel_index / (int)list_rows[option->group];
      row = rel_index % (int)list_rows[option->group];
      cell_x = (int)list_column_width[option->group] * col +
                 (int)list_origin_x[option->group];
      cell_y = (int)list_row_height[option->group] * row +
                 (int)list_origin_y[option->group];
      rect_set_xywh(&cell_rect,cell_x,cell_y,
                         list_column_width[option->group],
                         list_row_height[option->group]);
      is_selected = (uint)((uptr)selected_option == (iptr)option);
      if (is_selected == 0) {
        fill_color = list_fill_color;
      }
      else {
        fill_color = list_sel_fill_color;
      }
      inner_rect.left = cell_rect.left;
      inner_rect.top = cell_rect.top;
      inner_rect.right = cell_rect.right;
      inner_rect.bottom = cell_rect.bottom;
      if (this != (Dialog *)0xfffffdf8) {
        inner_rect.left = cell_rect.left + (int)list_indents[option->group];
      }
      if ((on_draw_option == 0) ||
         (group_index = on_draw_option(this,surf_wnd,option->id,group_index,cell_x,cell_y
                             ,(i32)list_row_height[option->group]),
         group_index == 0)) {
        if (is_selected != 0) {
          gfx_draw_bevel_rect((DibSurface *)surf_wnd,&inner_rect,bevel_light_color,bevel_dark_color);
          rect_deflate(&inner_rect,1,1);
        }
        gfx_fill_rect((DibSurface *)surf_wnd,&inner_rect,fill_color);
        draw_option_text(option->text,cell_x + 2,cell_y,is_selected,
                   option->flags & 1);
      }
      if (blit != 0) {
        ((DibSurface *)surf_wnd)->wnd_blit_surface_rect(&cell_rect);
      }
    }
  }
  return is_selected;
}

/* 005a14d2 auto */
void Dialog::draw_option_list()

{
  u8 *pane_drawn;
  u32 saved_pane;
  u32 pane;
  int first_index;
  DialogOption *option;
  uint drawn;
  i32 no_blit;
  int group_pos;
  u32 pane_idx;
  DialogOption *node2;
  int i;
  RECT clip_rect2;
  
  saved_pane = cur_pane;
  for (pane_idx = 0; (int)pane_idx < 2; pane_idx++) {
    if ((pane_idx != 1) || ((style_flags & 0x400000) != 0)) {
      cur_pane = pane_idx;
      pane_drawn = (u8 *)&this->pane_drawn[cur_pane];
      pane_drawn[0] = '\0';
      pane_drawn[1] = '\0';
      pane_drawn[2] = '\0';
      pane_drawn[3] = '\0';
      pane = cur_pane;
      clip_rect2.left = list_client_rect[pane].left;
      clip_rect2.top = list_client_rect[pane].top;
      clip_rect2.right = list_client_rect[pane].right;
      clip_rect2.bottom = list_client_rect[pane].bottom;
      if ((1 < (int)columns) || ((option_count == 0 || (on_draw_option != 0)))) {
        gfx_fill_rect((DibSurface *)surf_wnd,&list_client_rect[cur_pane],
                           list_fill_color);
      }
      gfx_draw_rect_frame((DibSurface *)surf_wnd,&pane_rect[cur_pane],
                         bevel_dark_color);
      ((DibSurface *)surf_wnd)->set_clip(&clip_rect2);
      if (option_count != 0) {
        node2 = dialog_state.active->option_list;
        first_index = option_group_index(list_top_option[cur_pane]);
        group_pos = 0;
        for (i = 0; (node2 != 0 && (i < (int)option_count));
            i = i + 1) {
          if (cur_pane == (u32)node2->group) {
            if ((first_index <= group_pos) &&
               (group_pos < (int)(list_rows[cur_pane] * columns + first_index))) {
              no_blit = 0;
              option = option_at(group_pos);
              drawn = draw_option(option,no_blit);
              this->pane_drawn[cur_pane] =
                   this->pane_drawn[cur_pane] | drawn;
              clip_rect2.top += (int)list_row_height[cur_pane];
            }
            group_pos++;
          }
          node2 = node2->next;
        }
        if (((columns == 1) && (on_draw_option == 0)) &&
           (first_index = rect_height(&clip_rect2), 0 < first_index)) {
          gfx_fill_rect((DibSurface *)surf_wnd,&clip_rect2,list_fill_color);
        }
      }
      ((DibSurface *)surf_wnd)->reset_clip();
      ((DibSurface *)surf_wnd)->wnd_blit_surface_rect(
                 &pane_rect[cur_pane]);
    }
  }
  cur_pane = saved_pane;
  return;
}

/* 005a1766 auto */
int Dialog::column_of_index(int index)

{
  if (columns != 1) {
    index = index / (int)list_rows[cur_pane];
  }
  return index;
}

/* 005a17a4 auto */
int Dialog::column_start_index(int index)

{
  if (columns != 1) {
    index -= index % (int)list_rows[cur_pane];
  }
  return index;
}

/* 005a17e9 auto */
void Dialog::select_option(DialogOption *option,int redraw)

{
  DialogOption *prev_option;
  bool scrolled;
  int option_index;
  i32 top_option;
  iptr scroll_pos;
  int top_index;
  
  scrolled = false;
  if ((option != 0) && ((uptr)selected_option != (uptr)option)) {
    top_index = option_group_index(list_top_option[cur_pane]);
    option_index = option_group_index(option);
    if (option_index < top_index) {
      scrolled = true;
      top_index = option_index;
    }
    while ((int)(list_rows[cur_pane] * columns + top_index) <= option_index) {
      if (columns == 1) {
        top_index++;
      }
      else {
        top_index += list_rows[cur_pane];
      }
      scrolled = true;
    }
    top_option = column_start_index(option_count - 1);
    top_option = int_clamp_b(top_index,0,top_option);
    top_option = (i32)option_at(top_option);
    list_top_option[cur_pane] = (DialogOption *)top_option;
    prev_option = selected_option;
    selected_option = option;
    if ((scrolled) || (redraw == 0)) {
      if ((this)->list_vscroll[cur_pane] != 0) {
        scroll_pos = option_group_index(list_top_option[cur_pane]);
        ((Scrollbar *)(this)->list_vscroll[cur_pane])->set_pos(scroll_pos);
      }
      if ((this)->list_hscroll[cur_pane] != 0) {
        scroll_pos = column_of_option(list_top_option[cur_pane]);
        ((Scrollbar *)(this)->list_hscroll[cur_pane])->set_pos(scroll_pos);
      }
    }
    if (redraw != 0) {
      if (scrolled) {
        draw_option_list();
        if (on_select_change != 0) {
          on_select_change(this,option->id);
        }
      }
      else {
        draw_option(prev_option,1);
        draw_option(option,1);
        if (on_select_change != 0) {
          on_select_change(this,option->id);
        }
      }
    }
  }
  return;
}

/* 005a1a7d auto */
i32 Dialog::option_at_point(int x,int y)

{
  i32 result;
  int top_index;
  
  pick_pane_at(x,y);
  if ((this)->list_hscroll[cur_pane] != 0) {
    if (x < list_client_rect[cur_pane].left) {
      return 0xfffffffd;
    }
    if (list_client_rect[cur_pane].right <= x) {
      return 0xfffffffc;
    }
  }
  if (y < list_client_rect[cur_pane].top) {
    result = 0xffffffff;
  }
  else if (y < list_client_rect[cur_pane].bottom) {
    if (x < list_client_rect[cur_pane].left) {
      result = 0xfffffffd;
    }
    else if (x < list_client_rect[cur_pane].right) {
      top_index = option_group_index(list_top_option[cur_pane]);
      result = int_clamp_b(list_rows[cur_pane] *
                           ((x - list_client_rect[cur_pane].left) /
                           (int)list_column_width[cur_pane]) +
                           (y - list_client_rect[cur_pane].top) /
                           (int)list_row_height[cur_pane] + top_index,0,
                           option_count - 1);
    }
    else {
      result = 0xfffffffc;
    }
  }
  else {
    result = 0xfffffffe;
  }
  return result;
}

/* 005a1c52 auto */
int Dialog::count_group_options(int group)

{
  DialogOption *node2;
  int count;
  
  count = 0;
  for (node2 = option_list; node2 != 0; node2 = node2->next) {
    if (node2->group == group) {
      count++;
    }
  }
  return count;
}

/* 005a1caf auto */
void Dialog::pick_pane_at(int x,int y)

{
  u32 prev_pane;
  
  prev_pane = cur_pane;
  if ((style_flags & 0x400000) == 0) {
    cur_pane = 0;
  }
  else {
    cur_pane = 0;
    if (((((y < pane_rect[0].top) || (pane_rect[0].bottom < y)) ||
         (x < pane_rect[0].left)) || (pane_rect[0].right < x)) &&
       (((cur_pane = 1, y < pane_rect[1].top || (pane_rect[1].bottom < y)) ||
        ((x < pane_rect[1].left || (pane_rect[1].right < x)))))) {
      cur_pane = prev_pane;
    }
  }
  return;
}

/* 005a1da3 auto */
void __cdecl dialog_list_on_vscroll(i32 scroll_pos)

{
  DialogOption *top_option;
  
  if (dialog_state.active != 0) {
    dialog_state.active->cur_pane = 0;
    top_option = dialog_state.active->option_at(scroll_pos);
    if ((int)dialog_state.active->list_top_option[dialog_state.active->cur_pane] != (iptr)top_option) {
      dialog_state.active->list_top_option[dialog_state.active->cur_pane] = top_option;
      dialog_state.active->draw_option_list();
    }
  }
  return;
}

/* 005a1e28 auto */
i32 Dialog::create_window()

{
  i32 result;
  SurfaceWindow *this_00;
  int surf_width;
  u32 new_size;
  SurfaceWindow *surf_wnd2;
  i32 wnd_style;
  i8 *palette;
  i8 *parent2;
  i8 *title;
  i8 empty_title[4];
  i32 saved_seh;
  
  if (surf_wnd == 0) {
    this_00 = (SurfaceWindow *)operator_new(0x114);
    if (this_00 == (SurfaceWindow *)0x0) {
      surf_wnd2 = 0;
    }
    else {
      surf_wnd2 = this_00->SurfaceWindow::construct();
    }
    surf_wnd = (SurfaceWindow *)surf_wnd2;
    empty_title[0] = 0;
    title = (i8 *)title_text;
    if (title == (i8 *)0x0) {
      title = empty_title;
    }
    if ((style_flags & 0x10000) == 0) {
      wnd_style = 0xc02;
    }
    else {
      wnd_style = 0x842;
    }
    if ((*(i8 * *)&dlg_defaults.parent_window) == (i8 *)0x0) {
      if (main_frame_created == 0) {
        parent2 = (i8 *)0x0;
      }
      else {
        parent2 = ((i8 *)&main_frame_wnd_block.main_frame_wnd);
      }
    }
    else {
      parent2 = (*(i8 * *)&dlg_defaults.parent_window);
    }
    if ((*(i8 * *)&dlg_defaults.palette) == (i8 *)0x0) {
      palette = ((i8 *)&game_palette_block.game_palette);
    }
    else {
      palette = (*(i8 * *)&dlg_defaults.palette);
    }
    if (parent2 == (i8 *)0x0) {
      ((DibSurface *)surf_wnd)->wnd_create(title,wnd_style,window_x,
                 window_y,wnd_width,wnd_height,(GamePalette *)&game_palette_block.game_palette);
    }
    else {
      ((DibSurface *)surf_wnd)->wnd_create_with_surface((const char *)title,wnd_style,window_x,
                 window_y,wnd_width,wnd_height,(GamePalette *)palette,(WndBase *)parent2);
    }
    if ((style_flags & 0x10000) == 0) {
      surf_wnd->window.set_caption_height(title_height);
    }
    surf_width = ((DibSurface *)surf_wnd)->get_width();
    if ((int)wnd_width <= surf_width) {
      new_size = ((DibSurface *)surf_wnd)->get_width();
      wnd_width = new_size;
    }
    surf_width = ((DibSurface *)surf_wnd)->get_height();
    if ((int)wnd_height <= surf_width) {
      new_size = ((DibSurface *)surf_wnd)->get_height();
      wnd_height = new_size;
    }
    surf_wnd->set_redraw_callback(dialog_on_redraw);
    if (dlg_style.pending_idle_callback == 0) {
      if (dlg_style.default_idle_callback != 0) {
        surf_wnd->window.set_modal_idle_callback(dlg_style.default_idle_callback);
      }
    }
    else {
      surf_wnd->window.set_modal_idle_callback(dlg_style.pending_idle_callback);
      dlg_style.pending_idle_callback = 0;
    }
    if (dlg_style.pending_move_handler != 0) {
      surf_wnd->window.wnd_handlers.set_move_handler(dlg_style.pending_move_handler);
      dlg_style.pending_move_handler = 0;
    }
    result = 1;
  }
  else {
    result = 0;
  }
  return result;
}

/* 005a20f4 auto */
void __cdecl dialog_on_redraw(void)

{
  if (dialog_state.active != 0) {
    dialog_state.active->render();
  }
  return;
}

/* Part of Dialog::create_controls (005a211c): the radio buttons of the options. */
void Dialog::create_radio_group(uint *labels)
{
  RadioGroup *this_00;
  u32 x;
  iptr parent_18c;
  RadioGroup *radio_group2;
  int item_index;
  i8 rect[16];
  DialogOption *option;

  this_00 = (RadioGroup *)operator_new(0x50);
  if (this_00 == (RadioGroup *)0x0) {
    radio_group2 = 0;
  }
  else {
    radio_group2 = this_00->construct();
  }
  radio_group = (uptr)radio_group2;
  x = columns;
  if ((int)x < 2) {
    x = 1;
  }
  rect_set_xywh((RECT *)rect,list_x,list_y,option_width,
                     option_row_height *
                     ((int)((columns - 1) + option_count) / (int)x));
  if (surf_wnd == 0) {
    parent_18c = 0;
  }
  else {
    parent_18c = (uptr)&surf_wnd->window;
  }
  ((RadioGroup *)radio_group)->create((Wnd *)parent_18c,200,(RECT *)rect,columns,
             option_count,(char **)labels);
  option = option_list;
  item_index = 0;
  for (; option != (DialogOption *)0x0; option = option->next) {
    if ((uint *)selected_option == ((uint *)option)) {
      ((RadioGroup *)radio_group)->set_selected(item_index);
    }
    if ((option->flags & 1) != 0) {
      ((RadioGroup *)radio_group)->disable_item(item_index);
    }
    item_index++;
  }
  ((RadioGroup *)radio_group)->set_select_callback((RadioNotifyFn)dialog_radio_on_select);
  ((RadioGroup *)radio_group)->set_dblclick_callback(dialog_radio_on_dblclick);
}

/* Part of Dialog::create_controls (005a211c): the checkboxes of the options. */
void Dialog::create_checkboxes()
{
  u32 x;
  iptr parent_188;
  u32 *checkboxes2;
  u32 y2;
  DialogOption *option;
  int i;

  x = option_count;
  checkboxes2 = (u32 *)operator_new(x * 0x3c + 4);
  if (checkboxes2 == (u32 *)0x0) {
    checkboxes2 = (u32 *)0x0;
  }
  else {
    *checkboxes2 = x;
    crt_vector_ctor_iterator(checkboxes2 + 1,0x3c,*checkboxes2,checkbox_elem_ctor,
                 checkbox_dtor);
    checkboxes2++;
  }
  checkboxes = (uptr)checkboxes2;
  x = list_x;
  y2 = list_y;
  i = 0;
  for (option = option_list; option != (DialogOption *)0x0;
      option = option->next) {
    if ((1 < (int)columns) && (i != 0) && (i % (int)((columns - 1 + option_count) / columns) == 0)) {
      /* (OpenCiv2) in columns, as compute_layout made room for them (the
         options dialog; the game's checkboxes are in one column) */
      x += option_width / columns;
      y2 = list_y;
    }
    if (surf_wnd == 0) {
      parent_188 = 0;
    }
    else {
      parent_188 = (uptr)&surf_wnd->window;
    }
    ((Checkbox *)(checkboxes + i * 0x3c))->create((Wnd *)parent_188,i + 200
               ,x,y2,(const char *)option->text);
    ((Checkbox *)(checkboxes + i * 0x3c))->set_checked(option->flags & 4);
    if ((option->flags & 1) != 0) {
      ((Checkbox *)(checkboxes + i * 0x3c))->disable();
    }
    y2 += option_row_height;
    i++;
  }
}

/* Part of Dialog::create_controls (005a211c): the scroll bar of the current pane's list
   (vertical with one column, else horizontal), when the options do not fit.
   Pane 0 and pane 1 differ only in the callbacks (the original's dead
   exception-state stores, 1 2 and 4 5, are left out). */
void Dialog::create_list_scrollbar(ScrollFn vscroll_cb,ScrollFn hscroll_cb)
{
  Button *ctrl_mem;
  Scrollbar *new_vscroll;
  Scrollbar *new_hscroll;
  iptr parent;
  int count;
  i32 value;
  iptr pos;

  if (((int)(list_rows[cur_pane] * columns) < (int)option_count) ||
     ((style_flags & 0x1000000) != 0)) {
    if (columns == 1) {
      ctrl_mem = (Button *)operator_new(0x40);
      if (ctrl_mem == (Button *)0x0) {
        new_vscroll = 0;
      }
      else {
        new_vscroll = ((Scrollbar *)ctrl_mem)->construct();
      }
      list_vscroll[cur_pane] = (i32)new_vscroll;
      if (surf_wnd == 0) {
        parent = 0;
      }
      else {
        parent = (uptr)&surf_wnd->window;
      }
      ((Scrollbar *)list_vscroll[cur_pane])->create((Wnd *)parent,
                 0xb,&list_vscroll_rect[cur_pane],1);
      count = count_group_options(cur_pane);
      ((Scrollbar *)list_vscroll[cur_pane])->set_range(0,
                 count - 1);
      pos = option_group_index(
                         list_top_option[cur_pane]);
      ((Scrollbar *)list_vscroll[cur_pane])->set_pos(pos);
      ((Scrollbar *)list_vscroll[cur_pane])->set_page_size(
                 list_rows[cur_pane] - 1);
      ((Scrollbar *)list_vscroll[cur_pane])->set_callback(vscroll_cb);
      ((Scrollbar *)list_vscroll[cur_pane])->set_track_callback(vscroll_cb);
    }
    else {
      ctrl_mem = (Button *)operator_new(0x40);
      if (ctrl_mem == (Button *)0x0) {
        new_hscroll = 0;
      }
      else {
        new_hscroll = ((Scrollbar *)ctrl_mem)->construct();
      }
      list_hscroll[cur_pane] = (i32)new_hscroll;
      if (surf_wnd == 0) {
        parent = 0;
      }
      else {
        parent = (uptr)&surf_wnd->window;
      }
      ((Scrollbar *)list_hscroll[cur_pane])->create((Wnd *)parent,0xc,
                 &list_hscroll_rect[cur_pane],0);
      count = count_group_options(cur_pane);
      value = column_of_index(count - 1);
      ((Scrollbar *)list_hscroll[cur_pane])->set_range(0,value);
      value = option_group_index(
                         list_top_option[cur_pane]);
      pos = column_of_index(value);
      ((Scrollbar *)list_hscroll[cur_pane])->set_pos(pos);
      ((Scrollbar *)list_hscroll[cur_pane])->set_page_size(columns
                );
      ((Scrollbar *)list_hscroll[cur_pane])->set_callback(hscroll_cb);
      ((Scrollbar *)list_hscroll[cur_pane])->set_track_callback(hscroll_cb);
    }
  }
}

/* Part of Dialog::create_controls (005a211c): the current pane's list as a list box control
   (the original's dead exception-state stores, 0 and 3, are left out). */
void Dialog::create_list_listbox()
{
  Button *ctrl_mem;
  ListBox *new_listbox;
  iptr parent;
  i8 rect[16];
  DialogOption *option;
  int item_index;

  ctrl_mem = (Button *)operator_new(0x3c);
  if (ctrl_mem == (Button *)0x0) {
    new_listbox = 0;
  }
  else {
    new_listbox = ((ListBox *)ctrl_mem)->construct();
  }
  listbox[cur_pane] = (i32)new_listbox;
  rect_set_xywh((RECT *)rect,list_x,list_y,
                     list_width[cur_pane],list_height[cur_pane]);
  if (surf_wnd == 0) {
    parent = 0;
  }
  else {
    parent = (uptr)&surf_wnd->window;
  }
  ((ListBox *)listbox[cur_pane])->create((Wnd *)parent,10,
             (RECT *)rect);
  option = option_list;
  item_index = 0;
  for (; option != (DialogOption *)0x0; option = option->next) {
    if (cur_pane == (uint)option->group) {
      ((ListBox *)listbox[cur_pane])->add_string((const char *)option->text);
      if ((uint *)selected_option == ((uint *)option)) {
        ((ListBox *)listbox[cur_pane])->set_cur_sel(item_index);
      }
      item_index++;
    }
  }
  ((ListBox *)listbox[cur_pane])->set_dblclick_callback((CtrlNotifyFn)dialog_listbox_on_dblclick);
}

/* Part of Dialog::create_controls (005a211c): the hotspots of the sprites (a picture
   choice). */
void Dialog::create_sprite_hotspots()
{
  i32 value;
  int count;
  u32 zoom;
  iptr parent_190;
  i8 rect[16];
  iptr width2;
  u32 x;
  u32 *hotspots;
  u32 y2;
  DialogSprite *sprite;
  int i;

  if ((1 < (int)sprite_count) && (option_count == 0)) {
    x = sprite_count;
    hotspots = (u32 *)operator_new(x * 0x40 + 4);
    if (hotspots == (u32 *)0x0) {
      hotspots = (u32 *)0x0;
    }
    else {
      *hotspots = x;
      crt_vector_ctor_iterator(hotspots + 1,0x40,*hotspots,hotspot_ctor,
                   hotspot_dtor);
      hotspots++;
    }
    sprite_hotspots = (uptr)hotspots;
    x = sprite_x;
    y2 = sprite_y;
    sprite = sprite_list;
    i = 0;
    for (; sprite != 0; sprite = sprite->next) {
      width2 = gfx_zoom_scale(max_sprite_width,sprite_zoom);
      zoom = sprite_zoom;
      value = sprite->sprite->height();
      count = gfx_zoom_scale(value,zoom);
      rect_set_xywh((RECT *)rect,x,y2,width2,count);
      if (surf_wnd == 0) {
        parent_190 = 0;
      }
      else {
        parent_190 = (uptr)&surf_wnd->window;
      }
      ((Hotspot *)(sprite_hotspots + i * 0x40))->create((Wnd *)parent_190,i + 300,
                 (RECT *)rect);
      ((Hotspot *)(sprite_hotspots + i * 0x40))->set_click_callback(dialog_on_sprite_click);
      ((Hotspot *)(sprite_hotspots + i * 0x40))->set_dblclick_callback((CtrlNotifyFn)dialog_sprite_on_dblclick);
      y2 = y2 + section_gap + count;
      i++;
    }
  }
}

/* Part of Dialog::create_controls (005a211c): the edit fields. */
void Dialog::create_edit_fields()
{
  i32 value;
  int count;
  i32 text;
  int parent_194;
  DialogEditField *edit_field;
  u32 x;
  u32 *edits;
  int i;

  if (edit_field_list != 0) {
    edit_field = edit_field_list;
    x = edit_count;
    edits = (u32 *)operator_new(x * 0x48 + 4);
    if (edits == (u32 *)0x0) {
      edits = (u32 *)0x0;
    }
    else {
      *edits = x;
      crt_vector_ctor_iterator(edits + 1,0x48,*edits,edit_ctor,
                   edit_dtor);
      edits++;
    }
    edit_controls = (uptr)edits;
    i = 0;
    for (; edit_field != 0; edit_field = edit_field->next) {
      if (surf_wnd == 0) {
        parent_194 = 0;
      }
      else {
        parent_194 = (uptr)&surf_wnd->window;
      }
      value = (i32)edit_field->buffer;
      text = edit_field->width;
      count = edit_row_height();
      ((EditControl *)(edit_controls + i * 0x48))->create_at((Wnd *)parent_194,0x14,
                 edit_x + edit_label_width,count * i + edit_y,text,(const char *)value)
      ;
      if (2 < game.multiplayer) {
        ((EditControl *)(edit_controls + i * 0x48))->set_change_callback((CtrlNotifyFn)dialog_edit_on_change)
        ;
      }
      ((EditControl *)(edit_controls + i * 0x48))->set_max_len(edit_field->max_len);
      i++;
    }
  }
}

/* Part of Dialog::create_controls (005a211c): the buttons (the standard ones, then the
   extra ones). */
void Dialog::create_buttons()
{
  int count;
  int *controls;
  int i;
  iptr pos;
  i8 rect[16];
  iptr parent_198;
  iptr button_id;
  i8 *label;

  if (std_button_count + extra_button_count != 0) {
    count = std_button_count + extra_button_count;
    controls = (int *)operator_new(count * 0x3c + 4);
    if (controls == (int *)0x0) {
      controls = (int *)0x0;
    }
    else {
      *controls = count;
      crt_vector_ctor_iterator(controls + 1,0x3c,*controls,button_ctor,
                   ctrl_dtor);
      controls++;
    }
    button_controls = (uptr)controls;
  }
  for (i = 0; i < (int)(std_button_count + extra_button_count); i++) {
    if (i < (int)std_button_count) {
      button_id = buttons[i].kind;
      label = (((i8 * *)&dlg_style.button_labels))[button_id];
    }
    else {
      button_id = -1;
      label = extra_button_labels[i - std_button_count];
    }
    pos = line_height();
    rect_set_xywh((RECT *)rect,buttons[i].x,
                       button_y,button_width,pos);
    if (surf_wnd == 0) {
      parent_198 = 0;
    }
    else {
      parent_198 = (uptr)&surf_wnd->window;
    }
    ((Button *)(button_controls + i * 0x3c))->create((Wnd *)parent_198,i + 100,
               (RECT *)rect,(const char *)label);
    if ((button_id == DLG_BUTTON_CANCEL) || ((button_id == DLG_BUTTON_OK && ((style_flags & 1) == 0)))) {
      ((Button *)(button_controls + i * 0x3c))->set_cancel_button();
    }
    if (button_id == DLG_BUTTON_OK) {
      ((Button *)(button_controls + i * 0x3c))->set_default_button();
    }
    ((Button *)(button_controls + i * 0x3c))->set_callback(dialog_on_button_click);
  }
}

/* 005a211c auto */
void Dialog::create_controls()

{
  uint is_checkbox;
  iptr label_count2;
  uint is_listbox;
  DialogOption *option;
  i8 unused_flag;
  uint labels[41];
  i32 saved_seh;
  
  label_count2 = 0;
  if (this != dialog_state.active) {
    if ((dlg_style.stack_depth < 0) || (0xf < dlg_style.stack_depth)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_popupstackindex_0_popupstackindex_16,(const char*)&str_d_ss_franklinton_popup_1_cpp_2,0xc75);
    }
    dlg_style.stack_depth++;
    (*(Dialog * *)&net.dialog_stack_top) = this;
    dialog_state.active = this;
    (((i32 *)&dlg_style.stack))[dlg_style.stack_depth] = (i32)this;
  }
  if ((style_flags & 0x200) == 0) {
    unused_flag = 0;
    ctrl_set_pending_font((GameFont *)dialog_state.active->body_font);
    ui_set_button_font(&dialog_plain_font_obj_block.dialog_plain_font_obj);
    ui_set_ctrl_font((GameFont *)dialog_state.active->body_font);
    ui_set_default_font(&dialog_plain_font_obj_block.dialog_plain_font_obj);
    is_checkbox = style_flags & 4;
    is_listbox = style_flags & 0x1000;
    if ((is_checkbox == 0) && (is_listbox == 0)) {
      if (0x28 < (int)option_count) {
        if (dialog_clamp_overflow == 0) {
          error_report(0xfffffff2,9,0,0x28,option_count);
        }
        else {
          option_count = 0x28;
        }
      }
      /* BF-185, BF-187 (Peter Johnson's ledger): the labels of the first 40
         options (the original walked every option into its 41 labels after
         taking 40 of them) */
      for (option = option_list; (option != (DialogOption *)0x0) && (label_count2 < 0x28);
          option = option->next) {
        labels[label_count2] = (uint)option->text;
        label_count2++;
      }
    }
    if (option_count != 0) {
      if (is_listbox == 0) {
        if (is_checkbox == 0) {
          create_radio_group(labels);
        }
        else {
          create_checkboxes();
        }
      }
      else {
        cur_pane = 0;
        if ((style_flags & 0x40000) == 0) {
          create_list_scrollbar(dialog_list_on_vscroll,dialog_list_on_hscroll);
          surf_wnd->window.wnd_handlers.set_lbuttondown_handler(dialog_list_on_lbuttondown);
          surf_wnd->window.wnd_handlers.set_lbuttonup_any_handler(dialog_list_on_lbuttonup);
          surf_wnd->window.wnd_handlers.set_mousemove_handler(dialog_list_on_mousemove);
          surf_wnd->window.wnd_handlers.set_dblclick_handler(dialog_list_on_dblclick);
        }
        else {
          create_list_listbox();
        }
        if ((style_flags & 0x400000) != 0) {
          cur_pane = 1;
          if ((style_flags & 0x40000) == 0) {
            create_list_scrollbar(dialog_list2_on_vscroll,dialog_list2_on_hscroll);
          }
          else {
            create_list_listbox();
          }
          cur_pane = 0;
        }
      }
    }
    create_sprite_hotspots();
    create_edit_fields();
    create_buttons();
    surf_wnd->window.wnd_handlers.set_keydown_handler(dialog_on_keydown);
    surf_wnd->window.wnd_handlers.set_char_handler((WndKeyFn)dialog_on_char);
    style_flags = style_flags | 0x200;
  }
  return;
}

/* 005a3bae auto */
void __cdecl dialog_radio_on_dblclick(i32 ctrl_id,int item)

{
  DialogOption *found;
  i32 index;
  DialogOption *node2;
  
  found = 0;
  if (dialog_state.active != 0) {
    index = 0;
    for (node2 = dialog_state.active->option_list; node2 != 0;
        node2 = node2->next) {
      if (item == index) {
        found = node2;
      }
      index++;
    }
    dialog_state.active->result_id = 0xffffffff;
    if (found != 0) {
      dialog_state.active->result_id = found->id;
    }
    dialog_state.active->style_flags = dialog_state.active->style_flags | 0x2000;
    dialog_end_current();
  }
  return;
}

/* 005a3c58 auto */
void __cdecl dialog_end_current(void)

{
  dialog_state.active->style_flags = dialog_state.active->style_flags | 0x400;
  dialog_state.active->surf_wnd->window.end_modal_loop();
  return;
}

/* 005a3c83 auto */
void __cdecl dialog_listbox_on_dblclick(void)

{
  if ((dialog_state.active != 0) && ((((byte *)&dialog_state.active->style_flags)[1] & 4) == 0)) {
    dialog_state.active->style_flags = dialog_state.active->style_flags | 0x2000;
    dialog_end_current();
  }
  return;
}

/* 005a3cca auto */
void __cdecl dialog_on_sprite_click(int ctrl_id)

{
  bool changed;
  int done;
  i32 *sprite;
  int i;
  
  changed = false;
  sprite = (i32 *)dialog_state.active->sprite_list;
  if (sprite != (i32 *)0x0) {
    for (i = 0; i < ctrl_id - 300; i++) {
      sprite = (i32 *)sprite[4];
      if (sprite == (i32 *)0x0) {
        return;
      }
    }
    dialog_state.active->result_id = *sprite;
    if (((i32 *)dialog_state.active->selected_sprite != sprite) &&
       (dialog_state.active->selected_sprite = (DialogSprite *)sprite,
       (((byte *)&dialog_state.active->style_flags)[2] & 2) != 0)) {
      changed = true;
    }
    if (dialog_state.active->on_sprite_click == 0) {
      if (changed) {
        dialog_state.active->render();
      }
    }
    else {
      done = dialog_state.active->on_sprite_click(*sprite);
      dialog_state.active->render();
      if (done != 0) {
        dialog_state.active->style_flags = dialog_state.active->style_flags | 0x2000;
        dialog_end_current();
      }
    }
  }
  return;
}

/* 005a3df3 auto */
void __cdecl dialog_sprite_on_dblclick(void)

{
  int done;
  
  if (dialog_state.active->on_sprite_dblclick != 0) {
    done = dialog_state.active->on_sprite_dblclick(0);
    if (done == 0) {
      dialog_state.active->render();
    }
    else {
      dialog_state.active->style_flags = dialog_state.active->style_flags | 0x2000;
      dialog_end_current();
    }
  }
  return;
}

/* 005a3e56 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl dialog_on_button_click(int ctrl_id)

{
  int button_index;
  int sprite_index;
  DialogOption *found;
  int button_id;
  int index;
  DialogOption *node2;
  DialogSprite *sprite;
  
  if (dialog_state.active != 0) {
    dialog_state.timeout_start = timer_ticks();
    button_index = ctrl_id - 100;
    if (button_index < (int)dialog_state.active->std_button_count) {
      button_id = dialog_state.active->buttons[button_index].kind;
    }
    else {
      dialog_state.active->result_extra = (button_index - (int)dialog_state.active->std_button_count) + 1;
      button_id = DLG_BUTTON_OK;
    }
    if (button_id == DLG_BUTTON_HELP) {
      dialog_state.active->style_flags = dialog_state.active->style_flags | 0x80;
    }
    if (((button_id == DLG_BUTTON_OK) && ((((byte *)&dialog_state.active->style_flags)[2] & 2) != 0)) &&
       (dialog_state.active->selected_sprite != 0)) {
      sprite = dialog_state.active->sprite_list;
      sprite_index = 0;
      for (; (sprite != 0 && ((int)dialog_state.active->selected_sprite != (iptr)sprite));
          sprite = sprite->next) {
        sprite_index++;
      }
      if (sprite != 0) {
        dialog_on_sprite_click(sprite_index + 300);
      }
    }
    if (button_id == DLG_BUTTON_CANCEL) {
      dialog_state.active->result_id = 0xffffffff;
    }
    else {
      if (((int)dialog_state.active->sprite_count < 2) || (dialog_state.active->option_count != 0)) {
        dialog_state.active->result_id = 0;
      }
      if ((dialog_state.active->option_count != 0) && ((dialog_state.active->style_flags & 0x1004) == 0)
         ) {
        index = 0;
        found = 0;
        node2 = dialog_state.active->option_list;
        button_index = ((RadioGroup *)dialog_state.active->radio_group)->get_selected();
        for (; node2 != 0; node2 = node2->next) {
          if (button_index == index) {
            found = node2;
          }
          index++;
        }
        if (found != 0) {
          dialog_state.active->result_id = found->id;
        }
      }
    }
    dialog_end_current();
  }
  return;
}

/* 005a407f auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl dialog_on_keydown(i32 key)

{
  int count;
  DialogOption *option;
  int index;
  int pos;
  i32 redraw;
  DialogSprite *sprite;
  DialogOption *node2;
  
  if (dialog_state.active == 0) {
    return;
  }
  if (((((byte *)&dialog_state.active->style_flags)[1] & 0x10) == 0) || (dialog_state.active->edit_count != 0)) {
    dialog_state.timeout_start = timer_ticks();
  }
  if ((((byte *)&dialog_state.active->style_flags)[2] & 2) != 0) {
    switch(key) {
    case 0xa2:
    case 0xc1:
      dialog_state.active->selected_sprite =
           (DialogSprite *)*(i32 *)&dialog_state.active->selected_sprite->next;
      if (dialog_state.active->selected_sprite == 0) {
        dialog_state.active->selected_sprite = dialog_state.active->sprite_list;
      }
      dialog_state.active->render();
      break;
    case 0xa8:
    case 0xc0:
      for (sprite = dialog_state.active->sprite_list;
          ((sprite != 0 && ((int)sprite->next != (int)dialog_state.active->selected_sprite)) &&
          (sprite->next != 0)); sprite = sprite->next) {
      }
      dialog_state.active->selected_sprite = sprite;
      dialog_state.active->render();
      break;
    case 0xd1:
      if (dialog_state.active->selected_sprite != 0) {
        dialog_on_sprite_click(*(i32 *)dialog_state.active->selected_sprite);
      }
    }
  }
  if ((dialog_state.active->style_flags & 0x41000) != 0x1000) {
    return;
  }
  if (dialog_state.active->option_count == 0) {
    key = 0;
  }
  do {
    switch(key) {
    case 0xa1:
    case 199:
      index = dialog_state.active->option_group_index(dialog_state.active->selected_option);
      pos = dialog_state.active->count_group_options((i32)dialog_state.active->cur_pane);
      if (pos - 1 <= index) {
        return;
      }
      index = dialog_state.active->count_group_options((i32)dialog_state.active->cur_pane);
      redraw = (i32)dialog_state.active->option_at(index - 1);
      dialog_state.active->list_top_option[dialog_state.active->cur_pane] = (DialogOption *)redraw;
      dialog_state.active->select_option(
                 dialog_state.active->list_top_option[dialog_state.active->cur_pane],0);
      dialog_state.active->draw_option_list();
      return;
    case 0xa2:
    case 0xc1:
      node2 = dialog_state.active->selected_option;
      while (node2->next != 0) {
        node2 = node2->next;
        if ((int)dialog_state.active->cur_pane == node2->group) break;
      }
      if (((int)dialog_state.active->cur_pane == node2->group) &&
         (dialog_state.active->selected_option != node2)) {
        dialog_state.active->select_option(node2,1);
      }
      return;
    case 0xa3:
    case 0xc6:
      if ((int)dialog_state.active->columns < 2) {
        pos = dialog_state.active->option_group_index(dialog_state.active->selected_option);
        index = (int)dialog_state.active->list_rows[dialog_state.active->cur_pane];
        count = dialog_state.active->count_group_options((i32)dialog_state.active->cur_pane);
        if (index + pos < count) {
          redraw = 1;
          option = (dialog_state.active->option_at(
                             pos + (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]));
          dialog_state.active->select_option(option,redraw);
          return;
        }
        index = dialog_state.active->count_group_options((i32)dialog_state.active->cur_pane);
        redraw = 1;
        option = dialog_state.active->option_at(index - 1);
        dialog_state.active->select_option(option,redraw);
        return;
      }
      key = 0xc3;
      break;
    case 0xa4:
    case 0xc2:
      if ((int)dialog_state.active->columns != 1) {
        index = dialog_state.active->option_group_index(dialog_state.active->selected_option);
        pos = dialog_state.active->option_group_index(
                           dialog_state.active->list_top_option[dialog_state.active->cur_pane]);
        if ((int)dialog_state.active->list_rows[dialog_state.active->cur_pane] + pos <= index) {
          redraw = 1;
          option = (dialog_state.active->option_at(
                             index - (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]));
          dialog_state.active->select_option(option,redraw);
          return;
        }
        if (index < (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]) {
          return;
        }
        index = dialog_state.active->option_group_index(
                           dialog_state.active->list_top_option[dialog_state.active->cur_pane]);
        redraw = (i32)dialog_state.active->option_at(
                           index - (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]);
        dialog_state.active->list_top_option[dialog_state.active->cur_pane] = (DialogOption *)redraw;
        index = dialog_state.active->option_group_index(dialog_state.active->selected_option);
        redraw = 0;
        option = (dialog_state.active->option_at(
                           index - (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]));
        dialog_state.active->select_option(option,redraw);
        dialog_state.active->draw_option_list();
        return;
      }
      key = 0xc0;
      break;
    default:
      return;
    case 0xa6:
    case 0xc3:
      if ((int)dialog_state.active->columns != 1) {
        pos = dialog_state.active->option_group_index(dialog_state.active->selected_option);
        index = (int)dialog_state.active->list_rows[dialog_state.active->cur_pane];
        count = dialog_state.active->count_group_options((i32)dialog_state.active->cur_pane);
        if (count <= index + pos) {
          return;
        }
        redraw = 1;
        option = (dialog_state.active->option_at(
                           pos + (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]));
        dialog_state.active->select_option(option,redraw);
        return;
      }
      key = 0xc1;
      break;
    case 0xa7:
    case 0xc4:
      index = dialog_state.active->option_group_index(dialog_state.active->selected_option);
      if (index < 1) {
        return;
      }
      redraw = (i32)dialog_state.active->option_at(0);
      dialog_state.active->list_top_option[dialog_state.active->cur_pane] = (DialogOption *)redraw;
      dialog_state.active->select_option(
                 dialog_state.active->list_top_option[dialog_state.active->cur_pane],0);
      dialog_state.active->draw_option_list();
      return;
    case 0xa8:
    case 0xc0:
      node2 = dialog_state.active->selected_option;
      while (node2->prev != 0) {
        node2 = node2->prev;
        if ((int)dialog_state.active->cur_pane == node2->group) break;
      }
      if ((int)dialog_state.active->cur_pane != node2->group) {
        return;
      }
      if (dialog_state.active->selected_option == node2) {
        return;
      }
      if ((int)dialog_state.active->columns != 1) {
        index = dialog_state.active->option_group_index(
                           dialog_state.active->list_top_option[dialog_state.active->cur_pane]);
        pos = dialog_state.active->option_group_index(node2);
        if (pos < index) {
          index = dialog_state.active->option_group_index(
                             dialog_state.active->list_top_option[dialog_state.active->cur_pane]);
          redraw = (i32)dialog_state.active->option_at(
                             index - (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]
                            );
          dialog_state.active->list_top_option[dialog_state.active->cur_pane] = (DialogOption *)redraw;
          dialog_state.active->select_option(node2,0);
          dialog_state.active->draw_option_list();
          return;
        }
      }
      dialog_state.active->select_option(node2,1);
      return;
    case 0xa9:
    case 0xc5:
      if ((int)dialog_state.active->columns < 2) {
        index = dialog_state.active->option_group_index(dialog_state.active->selected_option);
        if ((int)dialog_state.active->list_rows[dialog_state.active->cur_pane] <= index) {
          redraw = 1;
          option = (dialog_state.active->option_at(
                             index - (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]));
          dialog_state.active->select_option(option,redraw);
          return;
        }
        redraw = 1;
        option = dialog_state.active->option_at(0);
        dialog_state.active->select_option(option,redraw);
        return;
      }
      key = 0xc2;
    }
  } while( true );
}

/* 005a49c1 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl dialog_on_char(byte ch)

{
  int target_index;
  uint upper_ch;
  uint first_ch;
  int top_index;
  int bottom_index;
  DialogOption *top_option;
  iptr match_index;
  DialogOption *match;
  DialogOption *node2;
  iptr counter2;
  
  if ((dialog_state.active != 0) && (target_index = _isalpha((uint)ch), target_index != 0)) {
    dialog_state.timeout_start = timer_ticks();
    upper_ch = crt_toupper((uint)ch);
    if ((dialog_state.active->style_flags & 0x41000) == 0x1000) {
      node2 = dialog_state.active->selected_option;
      match = 0;
      counter2 = 0;
      do {
        do {
          node2 = node2->next;
          if (node2 == 0) {
            node2 = dialog_state.active->option_list;
          }
        } while ((int)dialog_state.active->cur_pane != node2->group);
        first_ch = crt_toupper((int)*node2->text);
        if (first_ch == (upper_ch & 0xff)) {
          match = node2;
        }
      } while ((match == 0) && (counter2 = counter2 + 1, counter2 < 2000));
      if (match != 0) {
        target_index = dialog_state.active->option_group_index(match);
        counter2 = 0;
        while( true ) {
          top_index = dialog_state.active->option_group_index(
                             dialog_state.active->list_top_option[dialog_state.active->cur_pane]);
          bottom_index = (int)dialog_state.active->list_rows[dialog_state.active->cur_pane] *
                  (int)dialog_state.active->columns + top_index;
          if (((int)dialog_state.active->columns == 1) || ((top_index <= target_index && (target_index < bottom_index)))) break;
          if (target_index < top_index) {
            top_option = (dialog_state.active->option_at(
                               top_index - (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]));
            dialog_state.active->list_top_option[dialog_state.active->cur_pane] = top_option;
            counter2++;
          }
          else {
            if (target_index < bottom_index) {
              return;
            }
            top_option = (dialog_state.active->option_at(
                               top_index + (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]));
            dialog_state.active->list_top_option[dialog_state.active->cur_pane] = top_option;
            counter2++;
          }
        }
        if (counter2 == 0) {
          dialog_state.active->select_option(match,1);
        }
        else {
          dialog_state.active->select_option(match,0);
          dialog_state.active->draw_option_list();
        }
      }
    }
    else if ((((byte *)&dialog_state.active->style_flags)[2] & 2) == 0) {
      if (dialog_state.active->extra_button_count != 0) {
        match_index = -1;
        for (counter2 = 0; counter2 < (int)dialog_state.active->extra_button_count; counter2++) {
          first_ch = crt_toupper((int)*dialog_state.active->extra_button_labels[counter2]);
          if (first_ch == (upper_ch & 0xff)) {
            match_index = counter2;
            break;
          }
        }
        if (-1 < match_index) {
          ((Control *)
                     (((int)dialog_state.active->std_button_count + counter2) * 0x3c +
                     (int)dialog_state.active->button_controls))->set_focus();
          dialog_on_button_click((int)dialog_state.active->std_button_count + counter2 + 100);
          return;
        }
      }
      if (dialog_state.active->option_count != 0) {
        match = 0;
        match_index = 0;
        for (node2 = dialog_state.active->option_list; node2 != 0;
            node2 = node2->next) {
          first_ch = crt_toupper((int)*node2->text);
          if (first_ch == (upper_ch & 0xff)) {
            match = node2;
            break;
          }
          match_index++;
        }
        if (match != 0) {
          if ((((byte *)&dialog_state.active->style_flags)[1] & 0x10) == 0) {
            if ((((byte *)&dialog_state.active->style_flags)[0] & 4) == 0) {
              ((RadioGroup *)dialog_state.active->radio_group)->set_selected(match_index);
            }
          }
          else {
            ((ListBox *)dialog_state.active->listbox[dialog_state.active->cur_pane])->set_cur_sel(
                       match_index);
          }
        }
      }
    }
    else {
      dialog_end_current();
    }
  }
  return;
}

/* 005a4df3 auto */
void __cdecl dialog_list_on_lbuttondown(i32 x,i32 y)

{
  int hit_index;
  DialogOption *option;
  i32 redraw;
  
  if (dialog_state.active != 0) {
    dialog_state.active->pick_pane_at(x,y);
    dialog_state.active->list_left_col[dialog_state.active->cur_pane] = 0;
    if ((((dialog_state.active->style_flags & 0x41000) == 0x1000) &&
        (dialog_state.active->option_count != 0)) &&
       (hit_index = dialog_state.active->option_at_point(x,y),
       -1 < hit_index)) {
      dialog_state.active->list_left_col[dialog_state.active->cur_pane] = 1;
      dialog_state.active->surf_wnd->window.set_capture();
      redraw = 1;
      option = dialog_state.active->option_at(hit_index);
      dialog_state.active->select_option(option,redraw);
    }
  }
  return;
}

/* 005a4edf auto */
void __cdecl dialog_list_on_mousemove(i32 x,i32 y)

{
  int rows;
  int hit;
  DialogOption *option;
  int top_index;
  int last_col;
  i32 value;
  int sel_index;
  
  if (((dialog_state.active != 0) && ((dialog_state.active->style_flags & 0x41000) == 0x1000)) &&
     (dialog_state.active->list_left_col[dialog_state.active->cur_pane] != 0)) {
    hit = dialog_state.active->option_at_point(x,y);
    if (hit < 0) {
      if ((int)dialog_state.active->list_rows[dialog_state.active->cur_pane] *
          (int)dialog_state.active->columns < (int)dialog_state.active->option_count) {
        top_index = dialog_state.active->option_group_index(
                           dialog_state.active->list_top_option[dialog_state.active->cur_pane]);
        switch(hit) {
        case -4:
          if ((1 < (int)dialog_state.active->columns) &&
             (hit = dialog_state.active->column_start_index((int)dialog_state.active->option_count - 1)
             , top_index < hit)) {
            hit = (int)dialog_state.active->list_rows[dialog_state.active->cur_pane];
            rows = (int)dialog_state.active->list_rows[dialog_state.active->cur_pane];
            last_col = dialog_state.active->column_of_index((int)dialog_state.active->option_count - 1);
            hit = int_clamp_b(top_index + hit,0,rows * last_col);
            top_index = dialog_state.active->option_group_index(dialog_state.active->selected_option);
            for (sel_index = top_index % (int)dialog_state.active->list_rows[dialog_state.active->cur_pane] + hit;
                (int)dialog_state.active->list_rows[dialog_state.active->cur_pane] + sel_index <
                (int)dialog_state.active->list_rows[dialog_state.active->cur_pane] *
                (int)dialog_state.active->columns + hit;
                sel_index = sel_index + (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]) {
            }
            value = int_clamp_b(sel_index,0,(int)dialog_state.active->option_count - 1);
            value = (i32)dialog_state.active->option_at(value);
            dialog_state.active->selected_option = (DialogOption *)value;
            value = (i32)dialog_state.active->option_at(hit);
            dialog_state.active->list_top_option[dialog_state.active->cur_pane] = (DialogOption *)value;
            dialog_state.active->draw_option_list();
          }
          break;
        case -3:
          if ((1 < (int)dialog_state.active->columns) && (0 < top_index)) {
            top_index -= (int)dialog_state.active->list_rows[dialog_state.active->cur_pane];
            if (top_index < 1) {
              top_index = 0;
            }
            value = (i32)dialog_state.active->option_at(top_index);
            dialog_state.active->list_top_option[dialog_state.active->cur_pane] = (DialogOption *)value;
            hit = dialog_state.active->option_group_index(dialog_state.active->selected_option);
            value = (i32)dialog_state.active->option_at(
                               top_index + hit % (int)dialog_state.active->list_rows[dialog_state.active->cur_pane]);
            dialog_state.active->selected_option = (DialogOption *)value;
            dialog_state.active->draw_option_list();
          }
          break;
        case -2:
          if ((top_index < (int)dialog_state.active->option_count - 1) && ((int)dialog_state.active->columns == 1)
             ) {
            value = (i32)dialog_state.active->option_at(top_index + 1);
            dialog_state.active->list_top_option[dialog_state.active->cur_pane] = (DialogOption *)value;
            value = int_clamp_b((int)dialog_state.active->list_rows[dialog_state.active->cur_pane]
                                 * (int)dialog_state.active->columns + top_index + 1 - 1,0,
                                 (int)dialog_state.active->option_count - 1);
            value = (i32)dialog_state.active->option_at(value);
            dialog_state.active->selected_option = (DialogOption *)value;
            dialog_state.active->draw_option_list();
          }
          break;
        case -1:
          if ((0 < top_index) && ((int)dialog_state.active->columns == 1)) {
            value = (i32)dialog_state.active->option_at(top_index - 1);
            dialog_state.active->selected_option = (DialogOption *)value;
            dialog_state.active->list_top_option[dialog_state.active->cur_pane] = dialog_state.active->selected_option;
            dialog_state.active->draw_option_list();
          }
        }
      }
    }
    else {
      value = 1;
      option = dialog_state.active->option_at(hit);
      dialog_state.active->select_option(option,value);
    }
  }
  return;
}

/* 005a535e auto */
void __cdecl dialog_list_on_lbuttonup(i32 x,i32 y)

{
  if (dialog_state.active != 0) {
    dialog_list_on_mousemove(x,y);
    dialog_state.active->list_left_col[dialog_state.active->cur_pane] = 0;
    dialog_state.active->surf_wnd->window.release_capture();
  }
  return;
}

/* 005a53b8 auto */
void __cdecl dialog_list_on_dblclick(i32 x,i32 y)

{
  int hit_index;
  
  if ((((dialog_state.active != 0) &&
       (dialog_state.active->pick_pane_at(x,y),
       (dialog_state.active->style_flags & 0x41000) == 0x1000)) &&
      (dialog_state.active->option_count != 0)) &&
     (((((byte *)&dialog_state.active->style_flags)[1] & 4) == 0 &&
      (hit_index = dialog_state.active->option_at_point(x,y),
      -1 < hit_index)))) {
    dialog_state.active->list_left_col[dialog_state.active->cur_pane] = 1;
    dialog_list_on_lbuttonup(x,y);
    dialog_state.active->style_flags = dialog_state.active->style_flags | 0x2000;
    dialog_end_current();
  }
  return;
}

/* 005a5494 auto */
void __cdecl dialog_list_on_hscroll(int scroll_pos)

{
  DialogOption *top_option;
  
  if (dialog_state.active != 0) {
    dialog_state.active->cur_pane = 0;
    top_option = (dialog_state.active->option_at(
                       (int)dialog_state.active->list_rows[dialog_state.active->cur_pane] * scroll_pos
                      ));
    if ((int)dialog_state.active->list_top_option[dialog_state.active->cur_pane] != (iptr)top_option) {
      dialog_state.active->list_top_option[dialog_state.active->cur_pane] = top_option;
      dialog_state.active->draw_option_list();
    }
  }
  return;
}

/* 005a552c auto */
void __cdecl dialog_list2_on_vscroll(i32 scroll_pos)

{
  DialogOption *top_option;
  
  if (dialog_state.active != 0) {
    dialog_state.active->cur_pane = 1;
    top_option = dialog_state.active->option_at(scroll_pos);
    if ((int)dialog_state.active->list_top_option[dialog_state.active->cur_pane] != (iptr)top_option) {
      dialog_state.active->list_top_option[dialog_state.active->cur_pane] = top_option;
      dialog_state.active->draw_option_list();
    }
  }
  return;
}

/* 005a55b1 auto */
void __cdecl dialog_list2_on_hscroll(int scroll_pos)

{
  DialogOption *top_option;
  
  if (dialog_state.active != 0) {
    dialog_state.active->cur_pane = 1;
    top_option = (dialog_state.active->option_at(
                       (int)dialog_state.active->list_rows[dialog_state.active->cur_pane] * scroll_pos
                      ));
    if ((int)dialog_state.active->list_top_option[dialog_state.active->cur_pane] != (iptr)top_option) {
      dialog_state.active->list_top_option[dialog_state.active->cur_pane] = top_option;
      dialog_state.active->draw_option_list();
    }
  }
  return;
}

/* 005a5649 auto */
void Dialog::fill_rect_bg(RECT *rect,int area)

{
  i32 height;
  i32 width;
  i32 src_x;
  i32 src_y;
  
  if (bg_surface == 0) {
    if ((dlg_style.frame_tile_surface == 0) || (area != 1)) {
      if ((dlg_style.client_tile_surface == 0) || (area != 2)) {
        gfx_fill_rect((DibSurface *)surf_wnd,rect,bg_color);
      }
      else {
        src_y = 0;
        src_x = 0;
        height = rect_height(rect);
        width = rect_width(rect);
        gfx_tile_fill((DibSurface *)surf_wnd,(DibSurface *)dlg_style.client_tile_surface,rect->left,
                           rect->top,width,height,src_x,src_y);
      }
    }
    else {
      src_y = 0;
      src_x = 0;
      height = rect_height(rect);
      width = rect_width(rect);
      gfx_tile_fill((DibSurface *)surf_wnd,(DibSurface *)dlg_style.frame_tile_surface,rect->left,
                         rect->top,width,height,src_x,src_y);
    }
  }
  else {
    height = rect_height(rect);
    width = rect_width(rect);
    gfx_copy_rect_same_pos((DibSurface *)bg_surface,(DibSurface *)surf_wnd,rect->left,
                       rect->top,width,height);
  }
  return;
}

/* 005a577e auto */
void Dialog::render()

{
  int tmp;
  i32 size;
  int row_height;
  int line_spacing;
  u32 zoom;
  i8 sprite_rect[16];
  u32 pane;
  u32 saved_pane;
  int label_x;
  int sprite_h;
  DialogEditField *edit_field;
  u32 sprite_y2;
  u32 sprite_x2;
  int created;
  int title_y;
  int label_y;
  int title_x;
  DialogSprite *sprite;
  int i;
  int sprite_w;
  RECT rect;
  
  created = 0;
  if ((style_flags & 0x20) == 0) {
    tmp = compute_layout();
    if (tmp != 0) {
      return;
    }
    created = create_window();
    create_controls();
  }
  if ((style_flags & 0x4000) == 0) {
    rect_set_xywh(&rect,origin_x,origin_y,wnd_width,wnd_height);
    if (border_width != 0) {
      for (i = 0; i < (int)bevel_width; i++) {
        gfx_draw_bevel_rect((DibSurface *)surf_wnd,(RECT *)(iptr)((i8 *)&rect.left),bevel_light_color,bevel_dark_color);
        rect_deflate(&rect,1,1);
      }
      fill_rect_bg((RECT *)(iptr)((i8 *)&rect.left),1);
      rect_deflate(&rect,border_width + bevel_width * -2,0);
      rect.top = rect.top + title_height + bevel_width * -2;
      rect.bottom -= ((chrome_height - title_height) + bevel_width * -2);
      for (i = 0; i < (int)bevel_width; i++) {
        gfx_draw_bevel_rect((DibSurface *)surf_wnd,(RECT *)(iptr)((i8 *)&rect.left),bevel_dark_color,bevel_light_color);
        rect_deflate(&rect,1,1);
      }
    }
    fill_rect_bg((RECT *)(iptr)((i8 *)&rect.left),2);
  }
  if ((title_text != 0) && ((style_flags & 0x10000) == 0)) {
    zoom = wnd_width;
    tmp = ((GameFont *)title_font)->text_width((const char *)title_text);
    title_x = ((int)zoom >> 1) - (tmp + 4 >> 1);
    title_y = bevel_width + pad_y;
    gfx_set_text_color_index(title_shadow_color);
    ((DibSurface *)surf_wnd)->draw_text((GameFont *)title_font,(const char *)title_text,title_x + 2,
               title_y + 1,5);
    gfx_set_text_color_index(title_color);
    ((DibSurface *)surf_wnd)->draw_text((GameFont *)title_font,(const char *)title_text,title_x + 1,title_y,5)
    ;
    ((DibSurface *)surf_wnd)->draw_text((GameFont *)title_font,(const char *)title_text,title_x,title_y,5);
  }
  draw_wrapped_text(1);
  if (sprite_count != 0) {
    gfx_set_zoom_scale(sprite_zoom);
    sprite_x2 = sprite_x;
    sprite_y2 = sprite_y;
    for (sprite = sprite_list; sprite != (DialogSprite *)0x0;
        sprite = sprite->next) {
      zoom = sprite_zoom;
      size = sprite->sprite->width();
      sprite_w = gfx_zoom_scale(size,zoom);
      zoom = sprite_zoom;
      size = sprite->sprite->height();
      sprite_h = gfx_zoom_scale(size,zoom);
      if (on_draw_sprite == 0) {
        sprite->sprite->draw((RECT *)sprite_rect,(DibSurface *)surf_wnd,sprite_x2,sprite_y2);
      }
      else {
        on_draw_sprite((i32)sprite->sprite,surf_wnd,sprite->id,sprite->field_04,sprite_x2,sprite_y2);
      }
      if (((style_flags & 0x20000) != 0) && ((i32 *)selected_sprite == ((i32 *)sprite))) {
        rect_set(&rect,sprite_x2 - 1,sprite_y2 - 1,sprite_w + sprite_x2,
                           sprite_h + sprite_y2);
        gfx_draw_rect_frame((DibSurface *)surf_wnd,(RECT *)(iptr)((i8 *)&rect.left),frame_color);
      }
      tmp = sprite_h;
      if (sprite->label != 0) {
        label_x = sprite_gap + sprite_w + sprite_x2;
        row_height = ((GameFont *)body_font)->line_spacing();
        label_y = (tmp - row_height >> 1) + sprite_y2;
        draw_text_default_font((const char *)sprite->label,label_x,label_y,0);
      }
      sprite_y2 = sprite_y2 + sprite_h + section_gap;
    }
    gfx_reset_zoom_scale();
  }
  if (edit_count != 0) {
    edit_field = edit_field_list;
    i = 0;
    for (; edit_field != 0; edit_field = edit_field->next) {
      gfx_set_text_color_index(text_color);
      size = 5;
      tmp = edit_row_height();
      tmp = tmp * i;
      row_height = edit_row_height();
      zoom = edit_y;
      line_spacing = ((GameFont *)body_font)->line_spacing();
      ((DibSurface *)surf_wnd)->draw_text_cur_font((const char *)edit_field->label,edit_x,
                 (tmp + (row_height >> 1) + zoom) - (line_spacing >> 1),size);
      i++;
    }
  }
  if ((style_flags & 0x41000) == 0x1000) {
    draw_option_list();
  }
  if (created == 0) {
    ((DibSurface *)surf_wnd)->wnd_blit_surface();
    if (option_count != 0) {
      if ((style_flags & 0x1000) == 0) {
        if ((style_flags & 4) == 0) {
          ((RadioGroup *)radio_group)->show_all();
        }
        else {
          for (i = 0; i < (int)option_count; i++) {
            ((Control *)(checkboxes + i * 0x3c))->show_and_redraw();
          }
        }
      }
      else if ((style_flags & 0x40000) == 0) {
        saved_pane = cur_pane;
        for (pane = 0; (int)pane < 2; pane++) {
          if ((pane != 1) || ((style_flags & 0x400000) != 0)) {
            cur_pane = pane;
            if (list_vscroll[cur_pane] != 0) {
              ((Control *)list_vscroll[cur_pane])->show_and_redraw();
            }
            if (list_hscroll[cur_pane] != 0) {
              ((Control *)list_hscroll[cur_pane])->show_and_redraw();
            }
          }
        }
        cur_pane = saved_pane;
      }
    }
    if (edit_count != 0) {
      for (i = 0; i < (int)edit_count; i++) {
        ((Control *)(i * 0x48 + edit_controls))->show_and_redraw();
      }
    }
    if (std_button_count + extra_button_count != 0) {
      for (i = 0; i < (int)(std_button_count + extra_button_count); i++)
      {
        ((Control *)(button_controls + i * 0x3c))->show_and_redraw();
      }
    }
    if ((1 < (int)sprite_count) && (option_count == 0)) {
      for (i = 0; i < (int)sprite_count; i++) {
        ((Control *)(sprite_hotspots + i * 0x40))->show_and_redraw();
      }
    }
  }
  else {
    surf_wnd->SurfaceWindow::show();
  }
  style_flags = style_flags | 0x20;
  return;
}

/* (port) BF-184 (Peter Johnson's ledger): whether list item index (-1: the
   selected one) has a text that fits size bytes (the original copied it whole
   into 132; a longer label comes only from a custom resource) */
static int dialog_list_text_fits(uptr list,int index,size_t size)
{
  HWND hwnd;
  LRESULT len;

  hwnd = (HWND)(uptr)((ListBox *)list)->hwnd;
  if (index < 0) {
    index = (int)SendMessageA(hwnd,0x188,0,0);
    if (index == -1) {
      return 1;  /* no selection: the short text the list gives for none */
    }
  }
  len = SendMessageA(hwnd,0x18a,(WPARAM)index,0);
  return (0 <= len) && ((size_t)len < size);
}

/* 005a5f34 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

u32 Dialog::run_modal(char *out_text,int timeout_secs)

{
  NativeWindow *main_hwnd;
  BOOL is_iconic;
  int result;
  char item_text[132];
  char sel_text[132];
  DialogOption *option;
  int i;
  
  if (extra_ui(X_SUPPRESS_POPUPS) && extras_popup_suppressed(this)) {
    /* X33: a simple popup of the list: shown on the map instead */
    result_extra = 0;
    return result_id;
  }
  if (dlg_style.stack_depth != 0x10) {
    if (timeout_secs == 0) {
      dialog_state.timeout_start = timer_ticks();
    }
    else {
      dialog_state.timeout_start = timer_ticks();
      dialog_state.timeout_start = timeout_secs * 0x3c - 0x4b0 + dialog_state.timeout_start;
    }
    if (game_session_active != 0) {
      main_hwnd = main_frame_wnd_block.main_frame_wnd.get_window();
      is_iconic = IsIconic(main_hwnd->hwnd);
      if (is_iconic != 0) {
        main_frame_wnd_block.main_frame_wnd.show_restored();
      }
    }
    style_flags = style_flags & 0xffffdfff;
    if ((style_flags & 0x4000) != 0) {
      style_flags = style_flags | 0x10000;
    }
    render();
    if (((style_flags & 0x20) != 0) && ((style_flags & 8) == 0)) {
      if (on_modal_poll == 0) {
        surf_wnd->window.run_modal_loop();
        while ((style_flags & 0x400) == 0) {
          win_pump_one_message();
        }
      }
      else {
        while ((style_flags & 0x400) == 0) {
          result = on_modal_poll(this);
          if (result != 0) {
            dialog_end_current();
          }
        }
      }
      if ((out_text != 0) && (edit_controls != 0)) {
        ((Control *)edit_controls)->Control::get_text(out_text);
      }
      if (1 < (int)edit_count) {
        for (i = 0; i < (int)edit_count; i++) {
          ((Control *)(edit_controls + i * 0x48))->Control::get_text(
                     (((i8 *)&text_args.strings) + i * 0x104));
        }
      }
      if ((style_flags & 0x1000) == 0) {
        if ((style_flags & 4) != 0) {
          option = option_list;
          i = 0;
          for (; option != (DialogOption *)0x0; option = option->next) {
            option->flags = option->flags & 0xfffffffb;
            result = ((Checkbox *)(checkboxes + i * 0x3c))->is_checked();
            if (result != 0) {
              option->flags = option->flags | 4;
            }
            i++;
          }
        }
      }
      else if (result_id == 0) {
        if ((style_flags & 0x40000) == 0) {
          result_id = (u32)selected_option->id;
        }
        else {
          result_id = (u32)option_list->id;
          if (dialog_list_text_fits(listbox[cur_pane],-1,sizeof sel_text)) {  /* BF-184 */
            ((ListBox *)listbox[cur_pane])->get_sel_text((char *)sel_text);
            option = option_list;
            i = 0;
            for (; option != (DialogOption *)0x0; option = option->next) {
              if (dialog_list_text_fits(listbox[cur_pane],i,sizeof item_text)) {
                ((ListBox *)listbox[cur_pane])->get_text(i,
                           (char *)item_text);
                result = _strcmp(sel_text,item_text);
                if (result == 0) {
                  result_id = (uint)option->id;
                }
              }
              i++;
            }
          }
        }
      }
      if ((style_flags & 0x800) == 0) {
        close();
        input_flush_events();
        win_flush_pending_msgs();
        wnd_flush_paint();
        win_flush_pending_msgs();
      }
    }
  }
  return result_id;
}

/* Part of Dialog::load_text_section (005a632a): an @ line: @OPTIONS/@PROMPT
   (options follow), @TEXT, @TITLE=, @BUTTON=, @COLUMNS, @HEIGHT, @SMALLFONT, @X,
   @Y, @WIDTH, @LENGTH, @CHECKBOX, @LISTBOX, @SYSTEM, @DEFAULT; another @ word
   ends the section. */
void Dialog::load_section_keyword(char *line,i8 *expanded,int &mode,int &default_value,uint has_edit,const char *edit_default,i8 *edit_text)
{
  int cmp;
  size_t len;
  u32 coord;
  int list_rows2;
  char *key2;
  int number;

  key2 = line + 1;
  cmp = __strcmpi(key2,str_options);
  if ((cmp == 0) || (cmp = _strcmp(key2,str_prompt), cmp == 0)) {
    mode = 2;
  }
  else {
    cmp = __strcmpi(key2,((i8 *)&dlg_key_text_str));
    if (cmp == 0) {
      mode = 1;
    }
    else {
      cmp = __strnicmp(key2,str_title,5);
      if (cmp == 0) {
        for (; (*key2 != '\0' && (*key2 != '=')); key2++) {
        }
        if (*key2 != '\0') {
          key2++;
        }
        /* BF-059 (Peter Johnson's ledger): expanded is load_text_section's
           256 bytes; a longer text is cut */
        text_expand_vars_max(key2,expanded,0x100);
        set_title((const char *)expanded);
      }
      else {
        cmp = __strnicmp(key2,str_button,6);
        if (cmp == 0) {
          for (; (*key2 != '\0' && (*key2 != '=')); key2++) {
          }
          if (*key2 != '\0') {
            key2++;
          }
          text_expand_vars_max(key2,expanded,0x100);  /* BF-059 */
          add_button((const char *)expanded);
        }
        else {
          cmp = __strnicmp(key2,str_columns,7);
          if (cmp == 0) {
            while ((*key2 != '\0' && (cmp = _isdigit((int)*key2), cmp == 0))) {
              key2++;
            }
            number = _atoi(key2);
            set_columns(number);
          }
          else {
            cmp = __strnicmp(key2,str_height,6);
            if (cmp == 0) {
              while ((*key2 != '\0' && (cmp = _isdigit((int)*key2), cmp == 0)))
              {
                key2++;
              }
              number = _atoi(key2);
              set_height(number);
            }
            else {
              cmp = __strcmpi(key2,str_smallfont);
              if (cmp != 0) {
                cmp = __strnicmp(key2,((i8 *)&dlg_key_y_prefix),1);
                if ((cmp == 0) && (cmp = _isalpha((int)key2[1]), cmp == 0)) {
                  while (((*key2 != '\0' &&
                          (cmp = _isdigit((int)*key2), cmp == 0)) &&
                         (*key2 != '-'))) {
                    key2++;
                  }
                  coord = _atoi(key2);
                  req_y = coord;
                }
                else {
                  cmp = __strnicmp(key2,((i8 *)&dlg_key_x_prefix),1);
                  if ((cmp == 0) && (cmp = _isalpha((int)key2[1]), cmp == 0)) {
                    while (((*key2 != '\0' &&
                            (cmp = _isdigit((int)*key2), cmp == 0)) &&
                           (*key2 != '-'))) {
                      key2++;
                    }
                    coord = _atoi(key2);
                    req_x = coord;
                  }
                  else {
                    cmp = __strnicmp(key2,str_width,5);
                    if (cmp == 0) {
                      while ((*key2 != '\0' &&
                             (cmp = _isdigit((int)*key2), cmp == 0))) {
                        key2++;
                      }
                      number = _atoi(key2);
                      set_width(number);
                    }
                    else {
                      cmp = __strnicmp(key2,str_length,6);
                      if (cmp == 0) {
                        while ((*key2 != '\0' &&
                               (cmp = _isdigit((int)*key2), cmp == 0))) {
                          key2++;
                        }
                        if (default_value == 0) {
                          default_value = _atoi(key2);
                        }
                      }
                      else {
                        cmp = __strnicmp(key2,str_checkbox,7);
                        if (cmp == 0) {
                          style_flags = style_flags | 5;
                        }
                        else {
                          cmp = __strnicmp(key2,str_listbox,7);
                          if (cmp == 0) {
                            while ((*key2 != '\0' &&
                                   (cmp = _isdigit((int)*key2), cmp == 0))) {
                              key2++;
                            }
                            list_rows2 = _atoi(key2);
                            if (list_rows2 == 0) {
                              list_rows2 = 0x10;
                            }
                            set_listbox(list_rows2,content_width,0);
                          }
                          else {
                            cmp = __strnicmp(key2,str_system,6);
                            if (cmp == 0) {
                              style_flags = style_flags | 0x40000;
                            }
                            else {
                              cmp = __strnicmp(key2,str_default_3,7);
                              if (cmp == 0) {
                                if (has_edit == 0) {
                                  while ((*key2 != '\0' &&
                                         (cmp = _isdigit((int)*key2), cmp == 0))) {
                                    key2++;
                                  }
                                  if (default_value == 0) {
                                    default_value = _atoi(key2);
                                  }
                                }
                                else {
                                  for (; (*key2 != '\0' && (*key2 != '='));
                                      key2 = key2 + 1) {
                                  }
                                  if (*key2 != '\0') {
                                    key2++;
                                  }
                                  len = _strlen((char *)edit_default);
                                  if (len == 0) {
                                    /* BF-059: edit_text is load_text_section's 80 bytes */
                                    str_copy_max(edit_text,key2,0x50);
                                  }
                                }
                              }
                              else if (*key2 != '@') {
                                mode = 3;
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/* 005a632a auto */
i32 Dialog::load_text_section(const char *file_name,const char *section,int default_value,const char *edit_default,SurfaceWindow *init_arg1, DibSurface *init_arg2,RECT *init_arg3,uint flags)

{
  int cmp;
  size_t len;
  i8 expanded[256];
  int mode;
  i32 result;
  uint has_edit;
  char *line;
  i8 edit_text[80];
  DialogOption *option;
  int option_index;
  i8 *edit_buf;
  
  result = 1;
  mode = 1;
  option_index = 0;
  if (plat_extras) {
    extras_note_dialog(this,file_name,section);   /* X33 */
  }
  /* BF-051 (Peter Johnson's ledger): within its 112 bytes */
  str_copy_max((char *)((void *)(((i8 *)&dialog_text_font_obj_block.dialog_section_name))),(const char *)((void *)section),
               sizeof dialog_text_font_obj_block.dialog_section_name);
  cmp = text_open_section(file_name,section);
  if (cmp == 0) {
    if ((flags & 0x8000) == 0) {
      init(init_arg1,init_arg2,init_arg3,flags);
    }
    else {
      style_flags = style_flags | flags;
    }
    has_edit = (uint)(edit_default != 0);
    if (has_edit != 0) {
      /* BF-186 (Peter Johnson's ledger): a longer default is cut at 79
         characters (the original copied it whole) */
      str_copy_max((char *)((void *)edit_text),(const char *)((void *)edit_default),sizeof edit_text);
    }
    do {
      line = text_read_line(1);
      if (line == (char *)0x0) {
        mode = 3;
      }
      else {
        len = _strlen(line);
        if (len == 0) {
          if ((flags & 0x4000000) == 0) {
            mode++;
          }
          else {
            add_line((const char *)&str_empty_185,0xffffffff,0xffffffff,0xffffffff,0);
          }
        }
        else if (*line == '@') {
          load_section_keyword(line,expanded,mode,default_value,has_edit,edit_default,edit_text);
        }
        else if (mode == 1) {
          text_expand_vars_max(line,expanded,sizeof expanded);  /* BF-059 */
          add_line((const char *)expanded,0xffffffff,0xffffffff,0xffffffff,0);
        }
        else if (mode == 2) {
          text_expand_vars_max(line,expanded,sizeof expanded);  /* BF-059 */
          if (has_edit == 0) {
            if ((style_flags & 4) == 0) {
              option = add_option((const char *)expanded,option_index,0);
            }
            else {
              option = add_checkbox_option((const char *)expanded,option_index,0);
            }
            if (default_value == option_index) {
              selected_option = option;
            }
            option_index++;
          }
          else {
            if (default_value == 0) {
              default_value = 5;
            }
            edit_buf = edit_text;
            if (edit_count != 0) {
              edit_buf = ((i8 *)&text_args.strings) + edit_count * 0x104;
            }
            add_edit_field((const char *)expanded,edit_buf,default_value);
          }
        }
      }
    } while (mode < 3);
    result = 0;
  }
  else if ((flags & 0x8000) == 0) {
    init(init_arg1,init_arg2,init_arg3,flags);
  }
  return result;
}

/* 005a6c23 auto */
void __cdecl dlg_push_parent_window(iptr parent)

{
  dlg_defaults.saved_parent_window = dlg_defaults.parent_window;
  dlg_defaults.parent_window = parent;
  return;
}

/* 005a6c45 auto */
void __cdecl dlg_pop_parent_window(void)

{
  dlg_defaults.parent_window = dlg_defaults.saved_parent_window;
  return;
}
