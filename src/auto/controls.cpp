/* The game, controls: buttons, lists, combo and check boxes, edit fields, scroll bars: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0040f380 auto */
void Control::show_and_redraw()

{
  if (hwnd != 0) {
    hwnd_show(hwnd);
    wnd_invalidate(hwnd);
  }
  return;
}

/* 0040f3e0 auto */
Button * __thiscall button_ctor(Button *self)

{
  
  self->base_construct();
  self->hotkey_char = '\0';
  self->on_select = 0xffffffff;
  return self;
}

/* 0040f480 auto */
Control *Control::base_construct()

{
  ctrl_id = 0;
  parent = 0;
  SetRect((LPRECT)&rect_left,0,0,0,0);
  hwnd = 0;
  next_ctrl = 0;
  wants_initial_focus = '\0';
  hotkey_char = '\0';
  hotkey_pos = 0xffffffff;
  return this;
}

/* 0040f510 auto */
i32 Control::base_destruct()

{
  i32 result;
  
  if (parent != 0) {
    ((Wnd *)parent)->remove_control(ctrl_id);
  }
  next_ctrl = 0;
  result = ctrl_destroy_hwnd((iptr)this);
  return result;
}

/* 0040f570 auto */
i32 __thiscall ctrl_dtor(Button *self)

{
  i32 seh_prev;
  i32 seh_state;
  
  seh_state = 0;
  if (self->hwnd != 0) {
    self->destroy();
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0040f5c3 */ { self->base_destruct(); }
  return seh_prev;
}

/* 0040f610 auto */
void Control::destroy()

{
  if (hwnd != 0) {
    if (parent != 0) {
      ((Wnd *)parent)->remove_control(ctrl_id);
    }
    next_ctrl = 0;
    ctrl_destroy_hwnd((iptr)this);
  }
  return;
}

/* 0040f680 auto */
void Button::create(iptr parent,i32 ctrl_id,iptr rect,const char *label)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  font = (u32)(*(i8 * *)draw_target.font_rest);
  init(parent,6,ctrl_id,(RECT *)rect);
  on_click = 0;
  new_hwnd = button_create_hwnd((int *)rect,(iptr)this,label,1,(GameFont *)font);
  hwnd = new_hwnd;
  is_enabled = 1;
  return;
}

/* 0040f730 auto */
void Control::init(iptr parent_wnd,u32 type_code,u32 id,RECT *rect)

{
  parent = parent_wnd;
  ctrl_type = type_code;
  ctrl_id = id;
  rect_left = (u32)rect->left;
  rect_top = (u32)rect->top;
  rect_right = (u32)rect->right;
  rect_bottom = (u32)rect->bottom;
  next_ctrl = 0;
  hwnd = 0;
  wants_initial_focus = '\0';
  ((Wnd *)parent_wnd)->append_control((iptr)this);
  return;
}

/* 0040f7d0 auto */
void Button::set_default_button()

{
  iptr parent;
  
  parent = get_parent();
  *(Button **)(parent + 0xbc) = this;
  return;
}

/* 0040f810 auto */
uptr Control::get_parent()

{
  return parent;
}

/* 0040f840 auto */
void Button::set_cancel_button()

{
  iptr parent;
  
  parent = get_parent();
  *(Button **)(parent + 0xc0) = this;
  return;
}

/* 0040f880 auto */
void Button::set_callback(CtrlNotifyFn callback)

{
  on_click = callback;
  return;
}

/* 0040f8b0 auto */
Checkbox * __thiscall checkbox_elem_ctor(Checkbox *self)

{
  
  self->base_construct();
  return self;
}

/* 0040f930 auto */
void __thiscall checkbox_dtor(Checkbox *self)

{
  i32 seh_state;
  
  seh_state = 0;
  if (self->hwnd != 0) {
    checkbox_destroy_hwnd(self->hwnd);
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0040f98a */ { self->base_destruct(); }
  return;
}

/* 0040f9d0 auto */
void Checkbox::create(iptr parent,i32 ctrl_id,i32 x,i32 y ,const char *label)

{
  u32 new_hwnd;
  i8 rect[16];
  
  font = (u32)(*(i8 * *)(draw_target.font_rest + 8));
  checkbox_calc_rect((char *)label,x,y,(iptr)rect,(GameFont *)font);
  if (hwnd != 0) {
    destroy();
  }
  init(parent,2,ctrl_id,(RECT *)rect);
  on_toggle = 0;
  is_enabled = 1;
  checked = 0;
  new_hwnd = checkbox_create_hwnd((RECT *)rect,(iptr)this,(char *)label,1);
  hwnd = new_hwnd;
  return;
}

/* 0040faa0 auto */
void Checkbox::set_callback(CheckboxToggleFn callback)

{
  on_toggle = callback;
  return;
}

/* 0040fad0 auto */
void Checkbox::set_checked(u32 checked)

{
  this->checked = checked;
  return;
}

/* 0040fb00 auto */
Scrollbar *Scrollbar::construct()

{
  
  base_construct();
  page_size = 1;
  on_change = 0;
  on_track = 0;
  return this;
}

/* 0040fbb0 auto */
void Scrollbar::destruct()

{
  i32 seh_state;
  
  seh_state = 0;
  if (hwnd != 0) {
    scrollbar_destroy_hwnd(hwnd);
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0040fc0a */ { base_destruct(); }
  return;
}

/* 0040fc50 auto */
void Scrollbar::create(iptr parent,iptr ctrl_id,iptr rect,i32 style)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  init(parent,8,ctrl_id,(RECT *)rect);
  new_hwnd = scrollbar_create_hwnd((int *)rect,(iptr)this,style,1);
  hwnd = new_hwnd;
  scrollbar_hwnd_set_range(hwnd,1,100);
  field_38 = 1;
  return;
}

/* 0040fcf0 auto */
void Scrollbar::set_pos(iptr pos)

{
  cur_pos = pos;
  scrollbar_hwnd_set_pos(hwnd,pos);
  return;
}

/* 0040fd40 auto */
void Scrollbar::set_range(i32 min_value,i32 max_value)

{
  scrollbar_hwnd_set_range(hwnd,min_value,max_value);
  return;
}

/* 0040fd80 auto */
void Scrollbar::set_callback(ScrollFn callback)

{
  on_change = callback;
  return;
}

/* 00418740 auto */
u32 Control::get_id()

{
  return ctrl_id;
}

/* 00418770 auto */
u32 Control::get_hwnd()

{
  return hwnd;
}

/* 004187a0 auto */
EditControl * __thiscall edit_ctor(EditControl *self)

{
  
  self->base_construct();
  self->on_maxtext = 0;
  self->on_change = 0;
  self->field_44 = 0;
  self->on_key = 0;
  self->keep_sel = 0;
  self->field_39 = 0;
  return self;
}

/* 00418870 auto */
void __thiscall edit_dtor(EditControl *self)

{
  i32 seh_state;
  
  seh_state = 0;
  if (self->hwnd != 0) {
    edit_hwnd_cleanup_stub(self->hwnd);
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 004188ca */ { self->base_destruct(); }
  return;
}

/* 00418910 auto */
void EditControl::create(iptr parent,i32 ctrl_id,iptr rect,const char *text)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  edit_font = (u32)(*(i8 * *)(draw_target.font_rest + 12));
  init(parent,4,ctrl_id,(RECT *)rect);
  new_hwnd = edit_create_window((int *)rect,(iptr)this,1,1,(GameFont *)edit_font);
  hwnd = new_hwnd;
  hwnd_send_settext(hwnd,(LPARAM)(text));
  return;
}

/* 004189c0 auto */
void EditControl::set_max_len(i32 max_len)

{
  edit_hwnd_limit_text(hwnd,max_len);
  return;
}

/* 00418a00 auto */
void EditControl::set_callback(EditKeyFn callback)

{
  on_key = callback;
  return;
}

/* 00418a30 auto */
void Control::set_text(const char *text)

{
  hwnd_send_settext(hwnd,(LPARAM)(text));
  return;
}

/* 00418a70 auto */
void Control::get_text(char *buffer)

{
  hwnd_get_text(hwnd,(LPARAM)(buffer));
  return;
}

/* 00418ab0 auto */
ComboBox * __thiscall combo_ctor(ComboBox *self)

{
  
  self->base_construct();
  self->on_dblclick = 0;
  self->on_select = 0;
  return self;
}

/* 00418b50 auto */
void __thiscall combo_dtor(ComboBox *self)

{
  i32 seh_state;
  
  seh_state = 0;
  if (self->hwnd != 0) {
    combo_hwnd_cleanup_stub(self->hwnd);
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00418baa */ { self->base_destruct(); }
  return;
}

/* 00418bf0 auto */
void ComboBox::create(iptr parent,i32 ctrl_id,iptr rect)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  init(parent,10,ctrl_id,(RECT *)rect);
  new_hwnd = combo_create_window((int *)rect,(iptr)this,1);
  hwnd = new_hwnd;
  item_count = 0;
  return;
}

/* 00418c70 auto */
void ComboBox::set_font(iptr font)

{
  i32 hfont;
  
  hfont = ((GameFont *)font)->get_hfont();
  combo_hwnd_set_font(hwnd,hfont);
  return;
}

/* 00418ce0 auto */
void ComboBox::add_string(const char *text)

{
  combo_hwnd_add_string(hwnd,(LPARAM)(text));
  item_count++;
  return;
}

/* 00418d20 auto */
void ComboBox::clear()

{
  combo_hwnd_clear(hwnd);
  item_count = 0;
  return;
}

/* 00418d60 auto */
i32 ComboBox::get_cur_sel()

{
  i32 sel;
  
  sel = combo_hwnd_get_cur_sel(hwnd);
  return sel;
}

/* 00418d90 auto */
void ComboBox::set_cur_sel(iptr index)

{
  combo_hwnd_set_cur_sel(hwnd,index);
  return;
}

/* 00418dd0 auto */
void ComboBox::set_callback(CtrlNotifyFn callback)

{
  on_select = callback;
  return;
}

/* 00418e00 auto */
ListBox *ListBox::construct()

{
  
  base_construct();
  on_dblclick = 0;
  on_select = 0;
  return this;
}

/* 00418ea0 auto */
void ListBox::destruct()

{
  i32 seh_state;
  
  seh_state = 0;
  if (hwnd != 0) {
    listbox_hwnd_cleanup_stub(hwnd);
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00418efa */ { base_destruct(); }
  return;
}

/* 00418f40 auto */
void ListBox::create(iptr parent,i32 ctrl_id,iptr rect)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  font = (u32)(*(i8 * *)(draw_target.font_rest + 16));
  init(parent,7,ctrl_id,(RECT *)rect);
  new_hwnd = listbox_create_hwnd((int *)rect,(iptr)this,1,0,(GameFont *)font);
  hwnd = new_hwnd;
  item_count = 0;
  return;
}

/* 00418fe0 auto */
void ListBox::set_font(iptr font)

{
  i32 hfont;
  
  hfont = ((GameFont *)font)->get_hfont();
  listbox_hwnd_set_font(hwnd,hfont);
  return;
}

/* 00419020 auto */
void ListBox::add_string(const char *text)

{
  listbox_hwnd_add_string(hwnd,(LPARAM)(text));
  item_count++;
  return;
}

/* 00419060 auto */
void ListBox::clear()

{
  listbox_hwnd_clear(hwnd);
  item_count = 0;
  return;
}

/* 00421bd0 auto */
void __cdecl input_flush_events(void)

{
  win_drain_input_msgs();
  return;
}

/* 00421ca0 auto */
void RadioGroup::enable_item(iptr index)

{
  if ((index < (int)item_count) && (-1 < index)) {
    ((RadioItem *)items)[index].enabled = 1;
    radiogrp_hwnd_invalidate((i32)((RadioItem *)items)[index].hwnd);
  }
  return;
}

/* 0043c5f0 auto */
void Control::hide()

{
  if (hwnd != 0) {
    hwnd_hide(hwnd);
  }
  return;
}

/* 00447210 auto */
void RadioGroup::disable_item(iptr index)

{
  if ((index < (int)item_count) && (-1 < index)) {
    if (selected == index) {
      selected = 0xffffffff;
    }
    ((RadioItem *)items)[index].enabled = 0;
    radiogrp_hwnd_invalidate((i32)((RadioItem *)items)[index].hwnd);
  }
  return;
}

/* 004472c0 auto */
/* Library Function - Single Match
    public: unsigned int __thiscall CCheckListBox::GetCheckStyle(void)
   
   Library: Visual Studio 1998 Debug */

u32 RadioGroup::get_selected()

{
  return selected;
}

/* 004472f0 auto */
void RadioGroup::set_selected(u32 value)

{
  selected = value;
  return;
}

/* 00451ac0 auto */
void Scrollbar::set_track_callback(ScrollFn callback)

{
  on_track = callback;
  return;
}

/* 00453c40 auto */
void Button::disable()

{
  is_enabled = 0;
  wnd_invalidate(hwnd);
  return;
}

/* 00453c80 auto */
void Button::enable()

{
  is_enabled = 1;
  wnd_invalidate(hwnd);
  return;
}

/* 00479da8 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl scrollbar_66c728_global_init(void)

{
  scrollbar_66c728_static_init();
  scrollbar_66c728_reg_atexit();
  return;
}

/* 00479dc2 auto */
void __cdecl scrollbar_66c728_static_init(void)

{
  ((Scrollbar *)((i8 *)&static_scrollbar_1))->construct();
  return;
}

/* 00479ddc auto */
void __cdecl scrollbar_66c728_reg_atexit(void)

{
  _atexit(scrollbar_66c728_atexit_dtor);
  return;
}

/* 00479df9 auto */
void __cdecl scrollbar_66c728_atexit_dtor(void)

{
  ((Scrollbar *)((i8 *)&static_scrollbar_1))->destruct();
  return;
}

/* 00479e13 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl scrollbar_66c768_global_init(void)

{
  scrollbar_66c768_static_init();
  scrollbar_66c768_reg_atexit();
  return;
}

/* 00479e2d auto */
void __cdecl scrollbar_66c768_static_init(void)

{
  ((Scrollbar *)((i8 *)&static_scrollbar_2))->construct();
  return;
}

/* 00479e47 auto */
void __cdecl scrollbar_66c768_reg_atexit(void)

{
  _atexit(scrollbar_66c768_atexit_dtor);
  return;
}

/* 00479e64 auto */
void __cdecl scrollbar_66c768_atexit_dtor(void)

{
  ((Scrollbar *)((i8 *)&static_scrollbar_2))->destruct();
  return;
}

/* 00492ab0 auto */
i32 EditControl::get_sel_end()

{
  i32 sel_end;
  
  sel_end = edit_ctl_get_sel_end(hwnd);
  return sel_end;
}

/* 00492ae0 auto */
void EditControl::replace_sel(const char *text)

{
  edit_hwnd_replace_sel(hwnd,(LPARAM)(text));
  return;
}

/* 00492b20 auto */
void EditControl::set_sel(i32 sel_start,i32 sel_end)

{
  edit_hwnd_set_sel(hwnd,sel_start,sel_end);
  return;
}

/* 00497d40 auto */
void Button::create_with_callback(iptr parent,i32 ctrl_id,iptr rect,u32 callback,const char *label)

{
  on_select = callback;
  font = (u32)(*(i8 * *)draw_target.font_rest);
  create(parent,ctrl_id,rect,label);
  return;
}

/* 004bb3b0 auto */
Button *Button::scalar_destruct(uint flags)

{
  ctrl_dtor(this);
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004bb400 auto */
EditControl *EditControl::scalar_destruct(uint flags)

{
  edit_dtor(this);
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004bb450 auto */
Checkbox *Checkbox::scalar_destruct(uint flags)

{
  checkbox_dtor(this);
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004bb4a0 auto */
RadioGroup *RadioGroup::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004bb4f0 auto */
Scrollbar *Scrollbar::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004bb5b0 auto */
void Control::set_initial_focus()

{
  wants_initial_focus = 1;
  return;
}

/* 004bb5e0 auto */
void Control::set_focus()

{
  if (hwnd != 0) {
    ctrl_hwnd_set_focus(hwnd);
  }
  return;
}

/* 004bb620 auto */
void EditControl::create_styled(iptr parent,i32 ctrl_id,iptr rect,const char *text, i32 multiline,i32 style)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  edit_font = (u32)(*(i8 * *)(draw_target.font_rest + 12));
  init(parent,4,ctrl_id,(RECT *)rect);
  new_hwnd = edit_create_window((int *)rect,(iptr)this,multiline,style,(GameFont *)edit_font);
  hwnd = new_hwnd;
  hwnd_send_settext(hwnd,(LPARAM)(text));
  return;
}

/* 004bb6d0 auto */
void EditControl::set_caret(i32 pos)

{
  edit_hwnd_set_caret(hwnd,pos);
  return;
}

/* 004bb710 auto */
i32 EditControl::get_text_end()

{
  i32 text_end;
  
  text_end = edit_hwnd_get_text_end(hwnd);
  return text_end;
}

/* 004bb740 auto */
void RadioGroup::destruct()

{
  i32 seh_state;
  
  seh_state = 0;
  if (items_handle != 0) {
    mem_handle_unlock(items_handle);
    radiogrp_destroy_items(item_count,items_handle);
  }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 004bb7b0 */ { base_destruct(); }
  return;
}

/* 004cffb0 auto */
i32 EditControl::get_line(i32 line,char *buffer,i32 max_len)

{
  i32 length;
  
  length = edit_hwnd_get_line(hwnd,line,(short *)buffer,max_len);
  return length;
}

/* 005011b4 auto */
void __cdecl
spritebtn_create_icon_button(iptr parent,i32 ctrl_id,SpriteButton *button,iptr sprite,RECT *rect)

{
  int *surf_rect;
  int size;
  WndBase *parent_wnd;
  iptr palette;
  int center;
  i8 shadow_ctx[16];
  i8 draw_ctx[16];
  i32 height2;
  RECT rect2;
  RECT inner_rect;
  i32 width2;
  int icon_y;
  int icon_x;
  
  width2 = rect_width(rect);
  height2 = rect_height(rect);
  button->init_from_sprite((WndBase *)parent,ctrl_id,rect->left,rect->top,width2,
             height2,sprite,0x17);
  surf_rect = (int *)((DibSurface *)&button->on_select)->get_rect();
  rect2.left = *surf_rect;
  rect2.top = surf_rect[1];
  rect2.right = surf_rect[2];
  rect2.bottom = surf_rect[3];
  size = rect_width(&rect2);
  center = rect2.left + size / 2;
  size = ((Sprite *)sprite)->width();
  icon_x = center - size / 2;
  size = rect_height(&rect2);
  center = rect2.top + size / 2;
  size = ((Sprite *)sprite)->height();
  icon_y = center - size / 2;
  ((Sprite *)sprite)->draw((iptr)draw_ctx,(iptr)&button->on_select,icon_x,icon_y);
  ((Sprite *)sprite)->draw((iptr)shadow_ctx,(iptr)(button->button_surface),icon_x + 2,
             icon_y + 2);
  gfx_draw_rect_frame((iptr)&button->on_select,&rect2,10);
  gfx_draw_rect_frame((iptr)(button->button_surface),&rect2,10);
  inner_rect.left = rect2.left;
  inner_rect.top = rect2.top;
  inner_rect.right = rect2.right;
  inner_rect.bottom = rect2.bottom;
  rect_deflate((iptr)&inner_rect,1,1);
  gfx_draw_bevel_rect((iptr)&button->on_select,&inner_rect,0x1f,0xf);
  gfx_draw_bevel_rect((iptr)(button->button_surface),&inner_rect,0xf,10);
  rect_deflate((iptr)&inner_rect,1,1);
  gfx_draw_bevel_rect((iptr)&button->on_select,&inner_rect,0x1f,0xf);
  gfx_draw_bevel_rect((iptr)(button->button_surface),&inner_rect,0xf,10);
  parent_wnd = (WndBase *)button->get_parent();
  palette = parent_wnd->get_palette();
  ((DibSurface *)button->button_surface)->set_palette(palette);
  parent_wnd = (WndBase *)button->get_parent();
  palette = parent_wnd->get_palette();
  ((DibSurface *)&button->on_select)->set_palette(palette);
  return;
}

/* 00511560 auto */
SpriteButton *SpriteButton::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 005115b0 auto */
void SpriteButton::destruct()

{
  int seh_state;
  
  seh_state = seh_state & 0xff;
  (*((char *)&seh_state + 0)) = 2;
  if (hwnd != 0) {
    spritebtn_hwnd_cleanup_stub(hwnd);
  }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 00511624 */ { surface_dtor((DibSurface *)(((int)this) + 0x7c)); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 00511630 */ { surface_dtor((DibSurface *)(((int)this) + 0x34)); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0051163c */ { base_destruct(); }
  return;
}

/* 00511720 auto */
SpriteButton *SpriteButton::construct()

{
  
  base_construct();
  surface_ctor((DibSurface *)&on_select);
  surface_ctor((DibSurface *)button_surface);
  pressed_state = '\0';
  return this;
}

/* 005117f0 auto */
void SpriteButton::set_click_handler(CtrlNotifyFn callback)

{
  on_click = callback;
  return;
}

/* 00530e80 auto */
/* Library Function - Single Match
    protected: char * __thiscall streambuf::egptr(void)const 
   
   Library: Visual Studio 1998 Debug */

u32 Button::ctrl_is_enabled()

{
  return is_enabled;
}

/* 00530eb0 auto */
void EditControl::set_change_callback(CtrlNotifyFn callback)

{
  on_change = callback;
  return;
}

/* 00530ee0 auto */
void Checkbox::create_fixed(iptr parent,i32 ctrl_id,i32 x,i32 y ,i32 width,const char *label)

{
  u32 hwnd;
  i8 rect[16];
  
  font = (u32)(*(i8 * *)(draw_target.font_rest + 8));
  checkbox_calc_rect_fixed(x,y,width,(iptr)rect,(GameFont *)font);
  if (this->hwnd != 0) {
    destroy();
  }
  init(parent,2,ctrl_id,(RECT *)rect);
  on_toggle = 0;
  is_enabled = 1;
  checked = 0;
  hwnd = checkbox_create_hwnd((RECT *)rect,(iptr)this,(char *)label,1);
  this->hwnd = hwnd;
  return;
}

/* 00530fb0 auto */
void Checkbox::enable()

{
  is_enabled = 1;
  return;
}

/* 00530fe0 auto */
/* Library Function - Single Match
    public: int __thiscall ios::width(void)const 
   
   Library: Visual Studio 1998 Debug */

u32 Checkbox::is_checked()

{
  return checked;
}

/* 00531010 auto */
RadioGroup *RadioGroup::construct()

{
  
  base_construct();
  items_handle = 0;
  return this;
}

/* 005310a0 auto */
void RadioGroup::create(iptr parent,i32 ctrl_id,iptr rect,u32 columns,u32 item_count, iptr labels)

{
  u32 handle;
  
  if (items_handle != 0) {
    destroy();
  }
  radio_font = (u32)(*(i8 * *)(draw_target.font_rest + 4));
  init(parent,3,ctrl_id,(RECT *)rect);
  on_select = 0;
  on_dblclick = 0;
  on_rclick = 0;
  this->item_count = item_count;
  this->columns = columns;
  selected = 0;
  handle = radiogrp_create_items((iptr)&this->item_count,(int *)rect,columns,(iptr)this,item_count,labels,1,
                       radio_font);
  items_handle = handle;
  handle = (u32)mem_handle_lock(items_handle);
  items = handle;
  if (extra_ui(X_RADIO_KEYS)) {
    extras_radio_hotkeys((RadioItem *)items,(int)item_count);   /* X24 */
  }
  return;
}

/* 005311b0 auto */
void RadioGroup::set_select_callback(RadioNotifyFn callback)

{
  on_select = callback;
  return;
}

/* 005311e0 auto */
void RadioGroup::set_dblclick_callback(RadioNotifyFn callback)

{
  on_dblclick = callback;
  return;
}

/* 00551d20 auto */
/* Library Function - Single Match
    protected: char * __thiscall streambuf::egptr(void)const 
   
   Library: Visual Studio 1998 Debug */

u32 ListBox::get_count()

{
  return item_count;
}

/* 00551d50 auto */
i32 ListBox::get_cur_sel()

{
  i32 cur_sel;
  
  cur_sel = listbox_hwnd_get_cur_sel(hwnd);
  return cur_sel;
}

/* 00551d80 auto */
void ListBox::set_cur_sel(i32 index)

{
  listbox_hwnd_set_cur_sel(hwnd,index);
  return;
}

/* 00551dc0 auto */
void ListBox::set_dblclick_callback(CtrlNotifyFn callback)

{
  on_dblclick = callback;
  return;
}

/* 00551df0 auto */
void ListBox::set_select_callback(CtrlNotifyFn callback)

{
  on_select = callback;
  return;
}

/* 005a9320 auto */
/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall CDaoIndexFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CDaoRelationFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CODBCFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CString::`vector deleting destructor'(unsigned int)
   
   Library: Visual Studio 1998 Debug */

Checkbox *Checkbox::vector_destruct(uint flags)

{
  if ((flags & 2) == 0) {
    checkbox_dtor(this);
    if ((flags & 1) != 0) {
      operator_delete(this);
    }
  }
  else {
    crt_vector_dtor_iterator(this,0x3c,VECTOR_COUNT(this),checkbox_dtor);
    operator_delete(&VECTOR_COUNT(this));
  }
  return this;
}

/* 005a93b0 auto */
/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall CDaoIndexFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CDaoRelationFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CODBCFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CString::`vector deleting destructor'(unsigned int)
   
   Library: Visual Studio 1998 Debug */

EditControl *EditControl::vector_destruct(uint flags)

{
  if ((flags & 2) == 0) {
    edit_dtor(this);
    if ((flags & 1) != 0) {
      operator_delete(this);
    }
  }
  else {
    crt_vector_dtor_iterator(this,0x48,VECTOR_COUNT(this),edit_dtor);
    operator_delete(&VECTOR_COUNT(this));
  }
  return this;
}

/* 005a9440 auto */
/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall CDaoIndexFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CDaoRelationFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CODBCFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CString::`vector deleting destructor'(unsigned int)
   
   Library: Visual Studio 1998 Debug */

Button *Button::vector_destruct(uint flags)

{
  if ((flags & 2) == 0) {
    ctrl_dtor(this);
    if ((flags & 1) != 0) {
      operator_delete(this);
    }
  }
  else {
    crt_vector_dtor_iterator(this,0x3c,VECTOR_COUNT(this),ctrl_dtor);
    operator_delete(&VECTOR_COUNT(this));
  }
  return this;
}

/* 005a94d0 auto */
ListBox *ListBox::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 005a9640 auto */
void Checkbox::disable()

{
  is_enabled = 0;
  return;
}

/* 005a9670 auto */
void ListBox::get_text(i32 index,char *buffer)

{
  listbox_hwnd_get_text(hwnd,index,(LPARAM)(buffer));
  return;
}

/* 005a96b0 auto */
void ListBox::get_sel_text(char *buffer)

{
  listbox_hwnd_get_sel_text(hwnd,buffer);
  return;
}

/* 005bd39e auto */
int __cdecl input_poll_modifier(int modifier)

{
  SHORT key_state;
  BOOL has_msg;
  tagMSG msg;
  char pressed;
  
  pressed = '\0';
  if (modifier == 0x200) {
    key_state = GetKeyState(0x11);
    pressed = ((int)key_state & 0x8000U) != 0;
  }
  else if (modifier == 0x100) {
    key_state = GetKeyState(0x10);
    pressed = ((int)key_state & 0x8000U) != 0;
  }
  else if (modifier == 0x400) {
    while (has_msg = PeekMessageA(&msg,(HWND)0x0,0,0,1), has_msg != 0) {
      if (((msg.message == 0x106) || (msg.message == 0x104)) ||
         (msg.message == 0x105)) {
        pressed = 1;
      }
      else {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
      }
    }
  }
  return (int)pressed;
}

/* 005bd4cd auto */
uint __cdecl input_lbutton_down(void)

{
  SHORT key_state;
  
  key_state = GetAsyncKeyState(1);
  return (uint)((char)((ushort)key_state >> 8) != '\0');
}

/* 005c5e60 auto */
u32 Control::get_type()

{
  return ctrl_type;
}

/* 005c5e80 auto */
u32 Control::get_next()

{
  return next_ctrl;
}

/* 005c5ea0 auto */
u8 Control::get_initial_focus()

{
  return wants_initial_focus;
}

/* 005c5ec0 auto */
u32 Checkbox::checkbox_is_enabled()

{
  return is_enabled;
}

/* 005c5ee0 auto */
u32 RadioGroup::get_items()

{
  return items;
}

/* 005c5f00 auto */
u32 RadioGroup::radiogrp_item_count()

{
  return item_count;
}

/* 005c8f50 auto */
int Control::get_rect()

{
  return (int)&rect_left;
}

/* 005c90b0 auto */
void __cdecl ctrl_hwnd_set_focus(HWND hwnd)

{
  SetFocus(hwnd);
  return;
}

/* 005c90ca auto */
void __cdecl ctrl_paint_parent_bkgnd(iptr paint,HWND hwnd)

{
  LONG own_data;
  DibSurface *self;
  WndBase *parent_wnd;
  NativeWindow *window;
  iptr height;
  int value;
  HDC hdcSrc;
  LONG y1;
  DWORD rop;
  tagPOINT origin;
  GdiDib *dib2;
  HWND parent_hwnd;
  
  parent_hwnd = GetParent(hwnd);
  own_data = GetWindowLongA(parent_hwnd,0xc);
  if (own_data == 0) {
    parent_wnd = (WndBase *)GetWindowLongA(parent_hwnd,0);
    value = (int)parent_wnd->get_window();
    if ((int)((NativeWindow *)value)->bg_brush != 0) {
      FillRect(*(HDC *)paint,(RECT *)(paint + 8),((NativeWindow *)value)->bg_brush);
    }
  }
  else {
    self = (DibSurface *)GetWindowLongA(parent_hwnd,0xc);
    parent_wnd = (WndBase *)GetWindowLongA(parent_hwnd,0);
    window = parent_wnd->get_window();
    dib2 = self->get_dib();
    origin.x = *(int *)(paint + 8);
    origin.y = *(int *)(paint + 0xc);
    MapWindowPoints(hwnd,window->hwnd,&origin,1);
    if ((window->palette != 0) && (display_palette.palettized == 1)) {
      SelectPalette(*(HDC *)paint,window->palette,0);
      RealizePalette(*(HDC *)paint);
    }
    rop = 0xcc0020;
    hdcSrc = dib2->hdc;
    own_data = origin.x;
    y1 = origin.y;
    height = rect_height((RECT *)(paint + 8));
    value = rect_width((RECT *)(paint + 8));
    BitBlt(*(HDC *)paint,*(int *)(paint + 8),*(int *)(paint + 0xc),value,height,hdcSrc,own_data,y1
           ,rop);
  }
  return;
}

/* 005c9222 auto */
void __cdecl ctrl_parse_hotkey(iptr ctrl,const char *text)

{
  size_t len;
  uptr pos;
  char *cursor;
  char hotkey[4];
  
  cursor = (char *)text;
  *(i32 *)(ctrl + 0x28) = 0;
  for (; *cursor != '\0'; cursor++) {
    if (*cursor == '~') {
      /* remove the '~' */
      for (pos = *(uptr *)(ctrl + 0x28); len = _strlen((char *)text), pos < len;
          pos = pos + 1) {
        *(i8 *)(pos + text) = *(i8 *)(pos + 1 + text);
      }
      break;
    }
    *(int *)(ctrl + 0x28) = *(int *)(ctrl + 0x28) + 1;
  }
  len = _strlen((char *)text);
  if (*(uint *)(ctrl + 0x28) < len) {
    hotkey[0] = *(char *)(*(int *)(ctrl + 0x28) + text);
    hotkey[1] = 0;
    __strlwr(hotkey);
    *(char *)(ctrl + 0x25) = hotkey[0];
  }
  else {
    *(i32 *)(ctrl + 0x28) = 0xffffffff;
  }
  return;
}

/* 005c9307 auto */
LRESULT __stdcall ctrl_default_wndproc(HWND hwnd,uint msg,WPARAM wparam,LPARAM lparam)

{
  HWND hWnd;
  ControlBlock *block;
  LRESULT result;
  tagPAINTSTRUCT ps;
  
  if (msg < 0x10) {
    if (msg == 0xf) {
      BeginPaint(hwnd,&ps);
      ctrl_paint_parent_bkgnd((iptr)&ps,hwnd);
      EndPaint(hwnd,&ps);
      return 0;
    }
    if (msg == 2) {
      block = wnd_from_hwnd(hwnd);
      if (block != 0) {
        if (block->kind == 3) {
          *(i32 *)block->data = 0;
        }
        else {
          ((Control *)block->owner)->set_hwnd(0);
        }
        ctrl_hwnd_free_block(hwnd);
      }
      result = DefWindowProcA(hwnd,2,wparam,lparam);
      return result;
    }
  }
  else if ((0xff < msg) && (msg < 0x103)) {
    hWnd = GetParent(hwnd);
    SendMessageA(hWnd,msg,wparam,lparam);
    return 0;
  }
  result = DefWindowProcA(hwnd,msg,wparam,lparam);
  return result;
}

/* 005c944b auto */
int __cdecl ctrl_destroy_hwnd(iptr ctrl)

{
  int result;
  int hwnd_val;
  HWND hWnd;
  
  result = ((Control *)ctrl)->get_type();
  if (result != 3) {
    hwnd_val = ((Control *)ctrl)->get_hwnd();
    result = 0;
    if (hwnd_val != 0) {
      hWnd = ((Control *)ctrl)->get_hwnd();
      result = DestroyWindow(hWnd);
    }
  }
  return result;
}

/* 005c9499 auto */
ControlBlock * __cdecl ctrl_init_block(HWND hwnd,iptr owner)

{
  i32 handle;
  ControlBlock *block;
  
  block = (ControlBlock *)0x0;
  if (hwnd == (HWND)0x0) {
    dbg_log((const char *)&str_error_tried_to_initialize_null_control_i);
  }
  else {
    handle = mem_handle_alloc(0x30);
    block = (ControlBlock *)mem_handle_lock(handle);
    block->shared_bitmaps = 0;
    block->bitmaps[2] = 0;
    block->bitmaps[1] = (i32)block->bitmaps[2];
    block->bitmaps[0] = (i32)block->bitmaps[1];
    block->mem_handle = handle;
    block->owner = owner;
    block->pressed = 0;
    block->tracking = 0;
    block->field_20 = 0;
    block->kind = 0;
    SetWindowLongA(hwnd,0,(LONG)block);
  }
  return block;
}

/* 005c9595 auto */
void __cdecl ctrl_hwnd_free_block(HWND hwnd)

{
  i32 handle;
  ControlBlock *block;
  
  if (hwnd == (HWND)0x0) {
    dbg_log((const char *)&str_error_tried_to_dispose_null_control_in_d);
  }
  else {
    block = (ControlBlock *)GetWindowLongA(hwnd,0);
    if (block != (ControlBlock *)0x0) {
      if (block->shared_bitmaps == 0) {
        if (block->bitmaps[0] != 0) {
          DeleteObject((HGDIOBJ)(i32)block->bitmaps[0]);
          block->bitmaps[0] = 0;
        }
        if (block->bitmaps[1] != 0) {
          DeleteObject((HGDIOBJ)(i32)block->bitmaps[1]);
          block->bitmaps[1] = 0;
        }
        if (block->bitmaps[2] != 0) {
          DeleteObject((HGDIOBJ)(i32)block->bitmaps[2]);
          block->bitmaps[2] = 0;
        }
      }
      if (block->kind == 2) {
        operator_delete((void *)block->data);
        block->data = 0;
      }
      handle = block->mem_handle;
      mem_handle_unlock(handle);
      mem_handle_free(handle);
      SetWindowLongA(hwnd,-4,(iptr)ctrl_default_wndproc);
      SetWindowLongA(hwnd,0,0);
    }
  }
  return;
}

/* 005c9710 auto */
/* Library Function - Single Match
    public: void __thiscall ios::delbuf(int)
   
   Library: Visual Studio 1998 Debug */

void Control::set_hwnd(u32 hwnd)

{
  this->hwnd = hwnd;
  return;
}

/* Part of button_create_hwnd (005c9740): the face color: the window color, or a pen of
   the palette entry on_select names. */
static HPEN button_face_pen(HDC mem_dc,Button *button)
{
  WndBase *parent_wnd;
  GamePalette *this_00;
  HPALETTE hPal;
  GamePalette *this_01;
  GamePalette *logpal;
  u32 color_index;
  i8 *red_out;
  i8 *green_out;
  BOOL bForceBkgd;
  i8 *blue_out;
  i8 green[4];
  i8 red[4];
  i8 blue[4];
  HPEN bg_pen;

  if (button->on_select == 0xffffffff) {
    bg_pen = GetStockObject(1);
  }
  else {
    if (display_palette.palettized == 1) {
      bForceBkgd = 0;
      parent_wnd = (WndBase *)button->get_parent();
      this_00 = (GamePalette *)parent_wnd->get_palette();
      hPal = this_00->get_hpalette();
      SelectPalette(mem_dc,hPal,bForceBkgd);
      RealizePalette(mem_dc);
    }
    blue_out = blue;
    green_out = green;
    red_out = red;
    color_index = button->on_select;
    parent_wnd = (WndBase *)button->get_parent();
    this_01 = (GamePalette *)parent_wnd->get_palette();
    logpal = this_01->get_logpalette();
    palette_get_entry_rgb((GameLogPalette *)logpal,color_index,(iptr)red_out,(iptr)green_out,(iptr)blue_out);
    bg_pen = CreatePen(0,1,CONCAT12(blue[0],CONCAT11(green[0],red[0])))
    ;
  }
  return bg_pen;
}

/* Part of button_create_hwnd (005c9740): fills a state's bitmap with the face color
   (line by line with a palette pen). */
static void button_fill_face(HDC mem_dc,Button *button,tagRECT *bounds,HPEN bg_pen)
{
  HGDIOBJ old_obj;
  int line_y;

  if (button->on_select == 0xffffffff) {
    FillRect(mem_dc,bounds,bg_pen);
  }
  else {
    old_obj = SelectObject(mem_dc,bg_pen);
    for (line_y = bounds->top; line_y < bounds->bottom; line_y++) {
      MoveToEx(mem_dc,bounds->left,line_y,(LPPOINT)0x0);
      LineTo(mem_dc,bounds->right,line_y);
    }
    SelectObject(mem_dc,old_obj);
  }
}

/* Part of button_create_hwnd (005c9740): the raised bevel (white top-left, grey
   bottom-right); old_obj is the caller's: the hotkey underline after it
   restores the object it holds. */
static void button_draw_raised_bevel(HDC mem_dc,tagRECT *bounds,HPEN h_01,HPEN h_02,HGDIOBJ &old_obj)
{
  old_obj = SelectObject(mem_dc,h_01);
  MoveToEx(mem_dc,bounds->left,bounds->bottom,(LPPOINT)0x0);
  LineTo(mem_dc,bounds->left,bounds->top);
  LineTo(mem_dc,bounds->right,bounds->top);
  MoveToEx(mem_dc,bounds->left + 1,bounds->bottom - 1,(LPPOINT)0x0);
  LineTo(mem_dc,bounds->left + 1,bounds->top + 1);
  LineTo(mem_dc,bounds->right - 1,bounds->top + 1);
  SelectObject(mem_dc,h_02);
  MoveToEx(mem_dc,bounds->left + 1,bounds->bottom - 1,(LPPOINT)0x0);
  LineTo(mem_dc,bounds->right - 1,bounds->bottom - 1);
  LineTo(mem_dc,bounds->right - 1,bounds->top);
  MoveToEx(mem_dc,bounds->left + 2,bounds->bottom - 2,(LPPOINT)0x0);
  LineTo(mem_dc,bounds->right - 2,bounds->bottom - 2);
  LineTo(mem_dc,bounds->right - 2,bounds->top + 1);
  SelectObject(mem_dc,old_obj);
}

/* Part of button_create_hwnd (005c9740): underlines the hotkey letter of the label
   (text_rect: the label as drawn, saved_*: the same before; old_obj: the
   object the bevel drawing left selected before it). */
static void button_underline_hotkey(HDC mem_dc,Button *button,char *label_text,tagRECT *text_rect,int saved_left,int saved_top,int saved_right,int saved_bottom,HPEN h_02,HGDIOBJ old_obj)
{
  size_t len;
  tagRECT *draw_rect;
  UINT draw_flags;
  int text_width;
  int underline_end;
  char prefix[256];
  int underline_start;

  /* BF-150 (Peter Johnson's ledger): a label of 256 characters or more is
     cut, and a hotkey past the 255th not underlined (the original copied the
     whole label into the 256 bytes) */
  if ((-1 < (int)button->hotkey_pos) && ((int)button->hotkey_pos < (int)sizeof prefix - 1)) {
    str_copy_max(prefix,label_text,sizeof prefix);
    prefix[button->hotkey_pos] = '\0';
    len = _strlen(prefix);
    if (len == 0) {
      underline_start = text_rect->left;
    }
    else {
      text_rect->left = saved_left;
      text_rect->top = saved_top;
      text_rect->right = saved_right;
      text_rect->bottom = saved_bottom;
      draw_flags = 0x424;
      draw_rect = text_rect;
      len = _strlen(prefix);
      DrawTextA(mem_dc,prefix,len,draw_rect,draw_flags);
      underline_start = text_rect->right - 1;
    }
    str_copy_max(prefix,label_text,sizeof prefix);
    prefix[button->hotkey_pos + 1] = '\0';
    text_rect->left = saved_left;
    text_rect->top = saved_top;
    text_rect->right = saved_right;
    text_rect->bottom = saved_bottom;
    draw_flags = 0x424;
    draw_rect = text_rect;
    len = _strlen(prefix);
    DrawTextA(mem_dc,prefix,len,draw_rect,draw_flags);
    text_width = (text_rect->right + 1) - underline_start;
    underline_start = underline_start + (text_width >> 1) - 5;
    underline_end = underline_start + 10;
    SelectObject(mem_dc,h_02);
    MoveToEx(mem_dc,underline_start,text_rect->bottom - 2,(LPPOINT)0x0);
    LineTo(mem_dc,underline_end,text_rect->bottom - 2);
    MoveToEx(mem_dc,underline_start,text_rect->bottom - 1,(LPPOINT)0x0);
    LineTo(mem_dc,underline_end,text_rect->bottom - 1);
    SelectObject(mem_dc,old_obj);
  }
}

/* 005c9740 auto */
HWND __cdecl
button_create_hwnd(int *rect,iptr ctrl,const char *label,int visible,GameFont *font)

{
  WndBase *self;
  NativeWindow *window;
  iptr height;
  int value;
  HDC hdc;
  int value2;
  HBITMAP h;
  HBITMAP h_00;
  HPEN h_01;
  HPEN h_02;
  i32 font_handle;
  i32 *font_data;
  HGDIOBJ h_03;
  HGDIOBJ h_04;
  size_t len;
  HWND hWndParent;
  HMENU hMenu;
  LONG text_top;
  HINSTANCE hInstance;
  tagRECT *draw_rect;
  LPVOID lpParam;
  UINT draw_flags;
  LONG text_bottom;
  DWORD style2;
  char label_text[256];
  Button *button;
  int text_width;
  HBITMAP bmp_disabled;
  HDC mem_dc;
  HWND hwnd2;
  tagRECT bounds;
  HGDIOBJ old_obj;
  int saved_left;
  int saved_top;
  int saved_right;
  int saved_bottom;
  tagRECT text_rect;
  HPEN bg_pen;
  int orig_left;
  int orig_top;
  int orig_right;
  int orig_bottom;
  
  crt_strcpy((char *)((void *)label_text),(const char *)((void *)label));
  button = (Button *)ctrl;
  style2 = 0x40810000;
  if (visible != 0) {
    style2 = 0x50810000;
  }
  lpParam = (LPVOID)0x0;
  hMenu = (HMENU)0x0;
  hInstance = err_report_ctx_block.app_hinstance;
  self = (WndBase *)((Control *)ctrl)->get_parent();
  window = self->get_window();
  hWndParent = window->hwnd;
  height = rect_height((RECT *)rect);
  value = rect_width((RECT *)rect);
  hwnd2 = CreateWindowExA(4,str_mscontrolclass_2,((i8 *)&button_wnd_title),style2,*rect,
                              rect[1],value,height,hWndParent,hMenu,hInstance,lpParam);
  ctrl_parse_hotkey((iptr)button,(const char *)label_text);
  bounds.left = *rect;
  bounds.top = rect[1];
  bounds.right = rect[2];
  bounds.bottom = rect[3];
  OffsetRect(&bounds,-bounds.left,-bounds.top);
  bounds.right -= 2;
  bounds.bottom -= 2;
  hdc = GetDC(hwnd2);
  value = rect_height(&bounds);
  value2 = rect_width(&bounds);
  h = CreateCompatibleBitmap(hdc,value2,value);
  if (h == (HBITMAP)0x0) {
    dbg_log((const char *)&str_could_not_create_bitmap_in_msinitbutton);
    DestroyWindow(hwnd2);
    hwnd2 = (HWND)0x0;
  }
  else {
    value = rect_height(&bounds);
    value2 = rect_width(&bounds);
    h_00 = CreateCompatibleBitmap(hdc,value2,value);
    if (h_00 == (HBITMAP)0x0) {
      dbg_log((const char *)&str_could_not_create_bitmap_in_msinitbutton_2);
      DestroyWindow(hwnd2);
      hwnd2 = (HWND)0x0;
    }
    else {
      value = rect_height(&bounds);
      value2 = rect_width(&bounds);
      bmp_disabled = CreateCompatibleBitmap(hdc,value2,value);
      if (bmp_disabled == (HBITMAP)0x0) {
        dbg_log((const char *)&str_could_not_create_bitmap_in_msinitbutton_3);
        DestroyWindow(hwnd2);
        hwnd2 = (HWND)0x0;
      }
      else {
        mem_dc = CreateCompatibleDC(hdc);
        ReleaseDC(hwnd2,hdc);
        SetBkMode(mem_dc,1);
        bg_pen = button_face_pen(mem_dc,button);
        h_01 = CreatePen(0,1,0xffffff);
        h_02 = CreatePen(0,1,0x808080);
        font_handle = font->get_handle();
        font_data = (i32 *)mem_handle_lock(font_handle);
        h_03 = SelectObject(mem_dc,(HGDIOBJ)*font_data);
        font_handle = font->get_handle();
        mem_handle_unlock(font_handle);
        h_04 = SelectObject(mem_dc,h);
        orig_left = bounds.left;
        orig_top = bounds.top;
        orig_right = bounds.right;
        orig_bottom = bounds.bottom;
        button_fill_face(mem_dc,button,&bounds,bg_pen);
        button_draw_raised_bevel(mem_dc,&bounds,h_01,h_02,old_obj);
        text_rect.left = bounds.left;
        text_rect.top = bounds.top;
        text_rect.right = bounds.right;
        text_rect.bottom = bounds.bottom;
        draw_flags = 0x424;
        draw_rect = &text_rect;
        len = _strlen(label_text);
        DrawTextA(mem_dc,label_text,len,draw_rect,draw_flags);
        text_width = rect_width(&text_rect);
        text_bottom = bounds.bottom;
        value = rect_width(&bounds);
        value2 = ((bounds.left + value / 2) - text_width / 2) + text_width;
        text_top = bounds.top;
        value = rect_width(&bounds);
        SetRect(&text_rect,(bounds.left + value / 2) - text_width / 2,text_top,value2,text_bottom);
        draw_flags = 0x24;
        draw_rect = &text_rect;
        len = _strlen(label_text);
        DrawTextA(mem_dc,label_text,len,draw_rect,draw_flags);
        saved_left = text_rect.left;
        saved_top = text_rect.top;
        saved_right = text_rect.right;
        saved_bottom = text_rect.bottom;
        button_underline_hotkey(mem_dc,button,label_text,&text_rect,saved_left,saved_top,saved_right,saved_bottom,h_02,old_obj);
        SelectObject(mem_dc,h_00);
        button_fill_face(mem_dc,button,&bounds,bg_pen);
        old_obj = SelectObject(mem_dc,h_02);
        MoveToEx(mem_dc,bounds.left,bounds.bottom,(LPPOINT)0x0);
        LineTo(mem_dc,bounds.left,bounds.top);
        LineTo(mem_dc,bounds.right,bounds.top);
        MoveToEx(mem_dc,bounds.left + 1,bounds.bottom - 1,(LPPOINT)0x0);
        LineTo(mem_dc,bounds.left + 1,bounds.top + 1);
        LineTo(mem_dc,bounds.right - 1,bounds.top + 1);
        SelectObject(mem_dc,old_obj);
        bounds.left += 2;
        bounds.top += 2;
        text_rect.right = bounds.right;
        text_rect.bottom = bounds.bottom;
        draw_flags = 0x424;
        draw_rect = &text_rect;
        text_rect.left = bounds.left;
        text_rect.top = bounds.top;
        len = _strlen(label_text);
        DrawTextA(mem_dc,label_text,len,draw_rect,draw_flags);
        text_width = rect_width(&text_rect);
        text_bottom = bounds.bottom;
        value = rect_width(&bounds);
        value = (((value >> 1) + bounds.left) - (text_width >> 1)) + text_width;
        text_top = bounds.top;
        value2 = rect_width(&bounds);
        SetRect(&text_rect,((value2 >> 1) + bounds.left) - (text_width >> 1),text_top,value,text_bottom);
        draw_flags = 0x24;
        draw_rect = &text_rect;
        len = _strlen(label_text);
        DrawTextA(mem_dc,label_text,len,draw_rect,draw_flags);
        saved_left = text_rect.left;
        saved_top = text_rect.top;
        saved_right = text_rect.right;
        saved_bottom = text_rect.bottom;
        button_underline_hotkey(mem_dc,button,label_text,&text_rect,saved_left,saved_top,saved_right,saved_bottom,h_02,old_obj);
        SelectObject(mem_dc,bmp_disabled);
        bounds.left = orig_left;
        bounds.top = orig_top;
        bounds.right = orig_right;
        bounds.bottom = orig_bottom;
        button_fill_face(mem_dc,button,&bounds,bg_pen);
        button_draw_raised_bevel(mem_dc,&bounds,h_01,h_02,old_obj);
        text_rect.left = bounds.left;
        text_rect.top = bounds.top;
        text_rect.right = bounds.right;
        text_rect.bottom = bounds.bottom;
        draw_flags = 0x424;
        draw_rect = &text_rect;
        len = _strlen(label_text);
        DrawTextA(mem_dc,label_text,len,draw_rect,draw_flags);
        text_width = rect_width(&text_rect);
        text_bottom = bounds.bottom;
        value = rect_width(&bounds);
        value = (((value >> 1) + bounds.left) - (text_width >> 1)) + text_width;
        text_top = bounds.top;
        value2 = rect_width(&bounds);
        SetRect(&text_rect,((value2 >> 1) + bounds.left) - (text_width >> 1),text_top,value,text_bottom);
        SetTextColor(mem_dc,0xffffff);
        draw_flags = 0x24;
        draw_rect = &text_rect;
        len = _strlen(label_text);
        DrawTextA(mem_dc,label_text,len,draw_rect,draw_flags);
        OffsetRect(&text_rect,-1,-1);
        SetTextColor(mem_dc,0x808080);
        draw_flags = 0x24;
        draw_rect = &text_rect;
        len = _strlen(label_text);
        DrawTextA(mem_dc,label_text,len,draw_rect,draw_flags);
        OffsetRect(&text_rect,1,1);
        saved_left = text_rect.left;
        saved_top = text_rect.top;
        saved_right = text_rect.right;
        saved_bottom = text_rect.bottom;
        button_underline_hotkey(mem_dc,button,label_text,&text_rect,saved_left,saved_top,saved_right,saved_bottom,h_02,old_obj);
        SelectObject(mem_dc,h_04);
        SelectObject(mem_dc,h_03);
        DeleteObject(h_01);
        if (button->on_select != 0xffffffff) {
          DeleteObject(bg_pen);
        }
        DeleteObject(h_02);
        DeleteDC(mem_dc);
        value = (iptr)ctrl_init_block(hwnd2,ctrl);
        *(HBITMAP *)(value + 0xc) = h;
        *(HBITMAP *)(value + 0x10) = h_00;
        *(HBITMAP *)(value + 0x14) = bmp_disabled;
        *(i32 *)(value + 0x2c) = 6;
        SetWindowLongA(hwnd2,-4,(iptr)button_wndproc);
      }
    }
  }
  return hwnd2;
}

/* 005cac22 auto */
void __cdecl button_press_animated(HWND hwnd)

{
  Button *self;
  ControlBlock *block;
  int enabled;
  i32 ctrl_id;
  
  block = wnd_from_hwnd(hwnd);
  if (block != 0) {
    self = (Button *)block->owner;
    enabled = self->ctrl_is_enabled();
    if (enabled != 0) {
      SetFocus(hwnd);
      block->pressed = 1;
      wnd_redraw_client(hwnd);
      block->pressed = 0;
      ui_pump_messages();
      ui_wait_ticks(8);
      wnd_redraw_client(hwnd);
      modal_wnd.active_child = self->get_parent();
      ctrl_id = self->get_id();
      self->fire_callback(ctrl_id);
    }
  }
  return;
}

/* 005cacd4: written by hand in src/game/fixups.cpp */

/* 005cb601 auto */
HWND __cdecl spritebtn_create_hwnd(int x,int y,int width,int height,iptr ctrl,int visible)

{
  WndBase *self;
  NativeWindow *window;
  iptr rect_h;
  int value;
  HWND hWndParent;
  HMENU hMenu;
  HINSTANCE hInstance;
  LPVOID lpParam;
  DWORD style2;
  tagRECT rect;
  iptr owner2;
  HWND hwnd2;
  
  style2 = 0x40810000;
  if (visible != 0) {
    style2 = 0x50810000;
  }
  owner2 = ctrl;
  SetRect(&rect,x,y,x + width,y + height);
  lpParam = (LPVOID)0x0;
  hMenu = (HMENU)0x0;
  hInstance = err_report_ctx_block.app_hinstance;
  self = (WndBase *)((Control *)ctrl)->get_parent();
  window = self->get_window();
  hWndParent = window->hwnd;
  rect_h = rect_height(&rect);
  value = rect_width(&rect);
  hwnd2 = CreateWindowExA(4,str_mscontrolclass_3,((i8 *)&spritebtn_wnd_title),style2,x,y,value
                            ,rect_h,hWndParent,hMenu,hInstance,lpParam);
  value = (iptr)ctrl_init_block(hwnd2,ctrl);
  *(i32 *)(value + 0x2c) = 5;
  SetWindowLongA(hwnd2,-4,(iptr)spritebtn_wndproc);
  return hwnd2;
}

/* 005cb6db auto */
void __cdecl spritebtn_hwnd_cleanup_stub(i32 hwnd)

{
  return;
}

/* 005cb6eb: written by hand in src/game/fixups.cpp */

/* 005cbdf0 auto */
void Button::fire_callback(i32 ctrl_id)

{
  if (on_click != 0) {
    on_click(ctrl_id);
  }
  return;
}

/* 005cbe30 auto */
void SpriteButton::fire_click(i32 ctrl_id)

{
  if (on_click != 0) {
    on_click(ctrl_id);
  }
  return;
}

/* 005cbe70 auto */
void SpriteButton::fire_rclick(i32 ctrl_id)

{
  if (on_rclick != 0) {
    on_rclick(ctrl_id);
  }
  return;
}

/* 005cbf40 auto */
void __cdecl checkbox_calc_rect(char *label,int x,int y,iptr out_rect,GameFont *font)

{
  HDC hdc;
  HDC hdc_00;
  i32 font_handle;
  HGDIOBJ h;
  size_t c;
  tagSIZE *lpsz;
  int height2;
  tagSIZE text_size;
  tagTEXTMETRICA metrics;
  int width2;
  i32 *font_data;
  
  hdc = GetDC((HWND)0x0);
  hdc_00 = CreateCompatibleDC(hdc);
  ReleaseDC((HWND)0x0,hdc);
  font_handle = font->get_handle();
  font_data = (i32 *)mem_handle_lock(font_handle);
  h = SelectObject(hdc_00,(HGDIOBJ)*font_data);
  font_handle = font->get_handle();
  mem_handle_unlock(font_handle);
  GetTextMetricsA(hdc_00,&metrics);
  height2 = metrics.tmHeight;
  lpsz = &text_size;
  c = _strlen(label);
  GetTextExtentPointA(hdc_00,label,c,lpsz);
  width2 = text_size.cx;
  SelectObject(hdc_00,h);
  DeleteDC(hdc_00);
  width2 += 0x25;
  if (metrics.tmHeight < 0x14) {
    height2 = 0x14;
  }
  SetRect((LPRECT)out_rect,x,y,width2 + x,height2 + y);
  return;
}

/* 005cc035 auto */
void __cdecl checkbox_calc_rect_fixed(int x,int y,int width,iptr out_rect,GameFont *font)

{
  HDC hdc;
  HDC hdc_00;
  i32 font_handle;
  int height2;
  tagTEXTMETRICA metrics;
  i32 *font_data;
  
  hdc = GetDC((HWND)0x0);
  hdc_00 = CreateCompatibleDC(hdc);
  ReleaseDC((HWND)0x0,hdc);
  font_handle = font->get_handle();
  font_data = (i32 *)mem_handle_lock(font_handle);
  SelectObject(hdc_00,(HGDIOBJ)*font_data);
  font_handle = font->get_handle();
  mem_handle_unlock(font_handle);
  GetTextMetricsA(hdc_00,&metrics);
  height2 = metrics.tmHeight;
  DeleteDC(hdc_00);
  if (metrics.tmHeight < 0x14) {
    height2 = 0x14;
  }
  SetRect((LPRECT)out_rect,x,y,width + x,height2 + y);
  return;
}

/* 005cc0f0 auto */
HWND __cdecl checkbox_create_hwnd(RECT *rect,iptr ctrl,char *label,int visible)

{
  size_t len;
  void *label_copy;
  WndBase *self;
  NativeWindow *window;
  iptr height;
  int value;
  HWND check_hwnd;
  HBITMAP bmp_unchecked;
  HBITMAP bmp_checked;
  HBITMAP bmp_focus;
  HMENU hMenu;
  HINSTANCE hInstance;
  LPVOID lpParam;
  DWORD style2;
  
  len = _strlen(label);
  label_copy = operator_new(len + 1);
  crt_strcpy((char *)label_copy,label);
  style2 = 0x40010000;
  if (visible != 0) {
    style2 = 0x50010000;
  }
  lpParam = (LPVOID)0x0;
  hMenu = (HMENU)0x0;
  hInstance = err_report_ctx_block.app_hinstance;
  self = (WndBase *)((Control *)ctrl)->get_parent();
  window = self->get_window();
  check_hwnd = window->hwnd;
  height = rect_height(rect);
  value = rect_width(rect);
  check_hwnd = CreateWindowExA(4,str_mscontrolclass_4,((i8 *)&wnd_modal_stack_below_block.checkbox_wnd_title),style2,rect->left,
                           rect->top,value,height,check_hwnd,hMenu,hInstance,lpParam);
  ctrl_parse_hotkey(ctrl,(const char *)label_copy);
  bmp_unchecked = LoadBitmapA(err_report_ctx_block.app_hinstance,(LPCSTR)0x132);
  bmp_checked = LoadBitmapA(err_report_ctx_block.app_hinstance,(LPCSTR)0x131);
  bmp_focus = LoadBitmapA(err_report_ctx_block.app_hinstance,(LPCSTR)0x130);
  value = (iptr)ctrl_init_block(check_hwnd,ctrl);
  *(HBITMAP *)(value + 0xc) = bmp_unchecked;
  *(HBITMAP *)(value + 0x10) = bmp_checked;
  *(HBITMAP *)(value + 0x14) = bmp_focus;
  *(i32 *)(value + 0x2c) = 2;
  *(void **)(value + 0x24) = label_copy;
  SetWindowLongA(check_hwnd,-4,(iptr)checkbox_wndproc);
  return check_hwnd;
}

/* 005cc248 auto */
void __cdecl checkbox_destroy_hwnd(int hwnd)

{
  if (hwnd == 0) {
    dbg_log((const char *)&str_error_tried_to_dispose_null_checkbox_in);
  }
  return;
}

/* 005cc274 auto */
void __cdecl checkbox_toggle_by_hotkey(HWND hwnd)

{
  Button *self;
  ControlBlock *block;
  int checked;
  i32 is_checked;
  i32 ctrl_id;
  
  block = wnd_from_hwnd(hwnd);
  if (block != 0) {
    self = (Button *)block->owner;
    SetFocus(hwnd);
    checked = ((Checkbox *)self)->is_checked();
    if (checked == 0) {
      ((Checkbox *)self)->set_checked(1);
    }
    else {
      ((Checkbox *)self)->set_checked(0);
    }
    SetCapture(hwnd);
    InvalidateRect(hwnd,(RECT *)0x0,0);
    modal_wnd.active_child = self->get_parent();
    is_checked = ((Checkbox *)self)->is_checked();
    ctrl_id = self->get_id();
    ((Checkbox *)self)->fire_callback(ctrl_id,is_checked);
  }
  return;
}

/* Part of checkbox_wndproc (005cc320): the label's text color, enabled or disabled
   (a palette entry in a palettized window, else the set RGB or black/grey). */
static void checkbox_text_color(HDC hdc,Checkbox *checkbox)
{
  int value;
  uint color_index;
  WndBase *parent_wnd;
  GamePalette *game_palette2;
  WndBase *parent_base;
  iptr logpal;
  i8 *red_out;
  i8 *green_out;
  i8 *blue_out;
  i8 green[4];
  i8 red[4];
  i8 blue[4];

  value = checkbox->checkbox_is_enabled();
  if (value == 1) {
    parent_base = (WndBase *)checkbox->get_parent();
    value = (int)parent_base->get_window();
    if (((NativeWindow *)value)->field_40 == 8) {
      if (ctrl_colors.text == 0) {
        SetTextColor(hdc,0);
      }
      else {
        blue_out = blue;
        green_out = green;
        red_out = red;
        color_index = (uint)ctrl_colors.text;
        parent_wnd = (WndBase *)checkbox->get_parent();
        game_palette2 = (GamePalette *)parent_wnd->get_palette();
        logpal = (iptr)game_palette2->get_logpalette();
        palette_get_entry_rgb((GameLogPalette *)logpal,color_index,(iptr)red_out,(iptr)green_out,(iptr)blue_out);
        SetTextColor(hdc,CONCAT12(blue[0],CONCAT11(green[0],red[0])))
        ;
      }
    }
    else if (ctrl_rgb.text_rgb_set == '\0') {
      SetTextColor(hdc,0);
    }
    else {
      SetTextColor(hdc,CONCAT12(ctrl_rgb.text_rgb_b,CONCAT11(ctrl_rgb.text_rgb_g,ctrl_rgb.text_rgb_r)));
    }
  }
  else {
    value = checkbox->checkbox_is_enabled();
    if (value == 0) {
      parent_base = (WndBase *)checkbox->get_parent();
      value = (int)parent_base->get_window();
      if (((NativeWindow *)value)->field_40 == 8) {
        if (ctrl_colors.disabled == 0) {
          SetTextColor(hdc,0x404040);
        }
        else {
          blue_out = blue;
          green_out = green;
          red_out = red;
          color_index = (uint)ctrl_colors.disabled;
          parent_wnd = (WndBase *)checkbox->get_parent();
          game_palette2 = (GamePalette *)parent_wnd->get_palette();
          logpal = (iptr)game_palette2->get_logpalette();
          palette_get_entry_rgb((GameLogPalette *)logpal,color_index,(iptr)red_out,(iptr)green_out,(iptr)blue_out);
          SetTextColor(hdc,
                       CONCAT12(blue[0],CONCAT11(green[0],red[0])));
        }
      }
      else if (ctrl_rgb.disabled_rgb_set == '\0') {
        SetTextColor(hdc,0x404040);
      }
      else {
        SetTextColor(hdc,CONCAT12(ctrl_rgb.disabled_rgb_b,CONCAT11(ctrl_rgb.disabled_rgb_g,ctrl_rgb.disabled_rgb_r)))
        ;
      }
    }
  }
}

/* Part of checkbox_wndproc (005cc320): underlines the hotkey letter of the label. */
static void checkbox_underline_hotkey(HDC hdc,Checkbox *checkbox,char *label,tagRECT *rect)
{
  size_t len;
  tagRECT *draw_rect;
  UINT draw_flags;
  int underline_end;
  int underline_start;
  HBRUSH brush;
  char prefix[256];
  tagRECT text_rect;

  /* BF-150 (Peter Johnson's ledger): as button_underline_hotkey */
  if ((checkbox->hotkey_pos != 0xffffffff) && ((int)checkbox->hotkey_pos < (int)sizeof prefix - 1)) {
    str_copy_max(prefix,label,sizeof prefix);
    prefix[checkbox->hotkey_pos] = '\0';
    text_rect.left = rect->left;
    text_rect.top = rect->top;
    text_rect.right = rect->right;
    text_rect.bottom = rect->bottom;
    len = _strlen(prefix);
    if (len == 0) {
      underline_start = text_rect.left;
    }
    else {
      draw_flags = 0x424;
      draw_rect = &text_rect;
      len = _strlen(prefix);
      DrawTextA(hdc,prefix,len,draw_rect,draw_flags);
      underline_start = text_rect.right - 1;
    }
    str_copy_max(prefix,label,sizeof prefix);
    prefix[checkbox->hotkey_pos + 1] = '\0';
    text_rect.left = rect->left;
    text_rect.top = rect->top;
    text_rect.right = rect->right;
    text_rect.bottom = rect->bottom;
    draw_flags = 0x424;
    draw_rect = &text_rect;
    len = _strlen(prefix);
    DrawTextA(hdc,prefix,len,draw_rect,draw_flags);
    underline_start = underline_start + ((text_rect.right + 1) - underline_start >> 1) - 5;
    underline_end = underline_start + 10;
    brush = GetStockObject(3);
    SelectObject(hdc,brush);
    MoveToEx(hdc,underline_start,rect->bottom - 3,(LPPOINT)0x0);
    LineTo(hdc,underline_end,rect->bottom - 3);
    MoveToEx(hdc,underline_start,rect->bottom - 2,(LPPOINT)0x0);
    LineTo(hdc,underline_end,rect->bottom - 2);
  }
}

/* Part of checkbox_wndproc (005cc320): WM_PAINT: the box bitmap (off, on or disabled)
   and the label with its shadow and hotkey underline. */
static void checkbox_paint(HWND hwnd,ControlBlock *block,Checkbox *checkbox)
{
  iptr enabled;
  iptr checked;
  int *ctrl_rect;
  int value;
  uint color_index;
  WndBase *parent_wnd;
  GamePalette *game_palette2;
  size_t len;
  HWND hwnd_tmp;
  i32 result;
  i8 *red_out;
  i8 *green_out;
  tagRECT *draw_rect;
  i8 *blue_out;
  UINT draw_flags;
  tagRECT rect;
  HBITMAP bitmap;
  tagPAINTSTRUCT ps;
  HGDIOBJ old_font;
  tagSIZE bmp_size;
  i8 green[4];
  HDC hdc;
  char *label;
  int box_left;
  int box_top;
  int box_right;
  int box_bottom;
  i8 red[4];
  i8 blue[4];
  i32 *font_data;

  enabled = checkbox->checkbox_is_enabled();
  if (enabled == 0) {
    hwnd_tmp = GetFocus();
    if (hwnd_tmp == hwnd) {
      SetFocus((HWND)0x0);
    }
    bitmap = block->bitmaps[0];
  }
  else {
    checked = checkbox->is_checked();
    if (checked == 0) {
      bitmap = block->bitmaps[1];
    }
    else {
      bitmap = block->bitmaps[2];
    }
  }
  GetBitmapDimensionEx(bitmap,&bmp_size);
  ctrl_rect = (int *)checkbox->get_rect();
  rect.left = *ctrl_rect;
  rect.top = ctrl_rect[1];
  rect.right = ctrl_rect[2];
  rect.bottom = ctrl_rect[3];
  box_left = rect.left;
  box_top = rect.top;
  box_right = rect.right;
  box_bottom = rect.bottom;
  if (wnd_modal_stack_below_block.checkbox_mouse_pressed != 0) {
    box_right = rect.left + 0x20;
    value = rect_height(&rect);
    box_top = value - 0x14 + rect.top >> 1;
    box_bottom = rect.top + 0x14;
  }
  OffsetRect(&rect,-rect.left,-rect.top);
  hdc = BeginPaint(hwnd,&ps);
  ctrl_paint_parent_bkgnd((iptr)&ps,hwnd);
  result = ((GameFont *)checkbox->font)->get_handle();
  font_data = (i32 *)mem_handle_lock(result);
  old_font = SelectObject(hdc,(HGDIOBJ)*font_data);
  result = ((GameFont *)checkbox->font)->get_handle();
  mem_handle_unlock(result);
  SetBkMode(hdc,1);
  gdi_blit_transparent(hdc,bitmap,rect.left,rect.bottom - 0x14 >> 1,0x20,0x14,0,7);
  rect.left += 0x25;
  label = (char *)block->data;
  if ((ctrl_colors.shadow_dx != 0) || (ctrl_colors.shadow_dy != 0)) {
    OffsetRect(&rect,(uint)ctrl_colors.shadow_dx,(uint)ctrl_colors.shadow_dy);
    blue_out = blue;
    green_out = green;
    red_out = red;
    color_index = (uint)ctrl_colors.shadow;
    parent_wnd = (WndBase *)checkbox->get_parent();
    game_palette2 = (GamePalette *)parent_wnd->get_palette();
    game_palette2->get_entry(color_index,(iptr)red_out,(iptr)green_out,(iptr)blue_out);
    SetTextColor(hdc,CONCAT12(blue[0],CONCAT11(green[0],red[0])));
    draw_flags = 0x24;
    draw_rect = &rect;
    len = _strlen(label);
    DrawTextA(hdc,label,len,draw_rect,draw_flags);
    OffsetRect(&rect,-(uint)ctrl_colors.shadow_dx,-(uint)ctrl_colors.shadow_dy);
  }
  checkbox_text_color(hdc,checkbox);
  draw_flags = 0x24;
  draw_rect = &rect;
  len = _strlen(label);
  DrawTextA(hdc,label,len,draw_rect,draw_flags);
  checkbox_underline_hotkey(hdc,checkbox,label,&rect);
  SelectObject(hdc,old_font);
  EndPaint(hwnd,&ps);
}

/* 005cc320 auto */
i32 __stdcall checkbox_wndproc(HWND hwnd,uint msg,WPARAM wparam,LPARAM lparam)

{
  char nav_enabled;
  i32 ctrl_id;
  int value;
  iptr logpal;
  HWND hwnd_tmp;
  Wnd *nav_wnd;
  i32 result;
  tagRECT rect;
  ControlBlock *block;
  Checkbox *checkbox;
  HDC hdc;
  HBRUSH brush;
  
  block = wnd_from_hwnd(hwnd);
  checkbox = (Checkbox *)block->owner;
  if (msg < 0x10) {
    if (msg == 0xf) {
      checkbox_paint(hwnd,block,checkbox);
      hwnd_tmp = GetFocus();
      if (hwnd_tmp != hwnd) {
        return 0;
      }
    }
    else if (msg != 7) {
      if (msg == 8) {
        wnd_redraw_client(hwnd);
        return 0;
      }
      result = ctrl_default_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    nav_wnd = (Wnd *)checkbox->get_parent();
    nav_enabled = nav_wnd->keyboard_nav_enabled();
    if (nav_enabled == '\0') {
      result = ctrl_default_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    hdc = GetDC(hwnd);
    brush = GetStockObject(3);
    GetClientRect(hwnd,&rect);
    SelectObject(hdc,brush);
    rect.left += 0x20;
    FrameRect(hdc,&rect,brush);
    DeleteObject(brush);
    ReleaseDC(hwnd,hdc);
  }
  else if (msg == 0x100) {
    if (wparam == 9) {
      hwnd_tmp = GetParent(hwnd);
      SendMessageA(hwnd_tmp,msg,wparam,lparam);
    }
    else {
      if (wparam == 0x20) {
        nav_wnd = (Wnd *)checkbox->get_parent();
        nav_enabled = nav_wnd->keyboard_nav_enabled();
        if (nav_enabled != '\0') {
          SendMessageA(hwnd,0x201,0,0);
          SendMessageA(hwnd,0x202,0,0);
          return 0;
        }
      }
      hwnd_tmp = GetParent(hwnd);
      SendMessageA(hwnd_tmp,msg,wparam,lparam);
    }
  }
  else if (msg == 0x201) {
    value = checkbox->checkbox_is_enabled();
    if ((value != 0) && (wnd_modal_stack_below_block.checkbox_mouse_pressed == 0)) {
      SetFocus(hwnd);
      value = checkbox->is_checked();
      if (value == 0) {
        checkbox->set_checked(1);
      }
      else {
        checkbox->set_checked(0);
      }
      wnd_modal_stack_below_block.checkbox_mouse_pressed = 1;
      SetCapture(hwnd);
      InvalidateRect(hwnd,(RECT *)0x0,0);
      logpal = checkbox->get_parent();
      wnd_set_active_child(logpal);
      result = checkbox->is_checked();
      ctrl_id = checkbox->get_id();
      checkbox->fire_callback(ctrl_id,result);
    }
  }
  else {
    if (msg != 0x202) {
      result = ctrl_default_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    value = checkbox->checkbox_is_enabled();
    if (value != 0) {
      ReleaseCapture();
      wnd_modal_stack_below_block.checkbox_mouse_pressed = 0;
    }
  }
  return 0;
}

/* 005cce40 auto */
void Checkbox::fire_callback(i32 ctrl_id,i32 checked)

{
  if (on_toggle != 0) {
    on_toggle(ctrl_id,checked);
  }
  return;
}

/* 005cce80 auto */
void __stdcall control_subclass_wndproc(HWND hwnd,int msg,i32 wparam,i32 lparam)

{
  DWORD class_extra;
  WNDPROC base_proc;
  Control *self;
  
  class_extra = GetClassLongA(hwnd,-0x12);
  base_proc = (WNDPROC)GetWindowLongA(hwnd,class_extra - 4);
  if ((msg == 2) &&
     (self = (Control *)GetWindowLongA(hwnd,class_extra - 8), self != (Control *)0x0)) {
    self->set_hwnd(0);
  }
  (*base_proc)(hwnd,msg,wparam,lparam);
  return;
}

/* 005ccf17 auto */
HWND __cdecl scrollbar_create_hwnd(int *rect,iptr ctrl,uint style_flags,int visible)

{
  WndBase *parent_wnd;
  NativeWindow *window;
  iptr height;
  int value;
  int nWidth;
  HWND parent_hwnd;
  HMENU menu;
  HINSTANCE hinst;
  LPVOID create_param;
  uint style2;
  WNDCLASSA wc;
  HWND hwnd2;
  
  style2 = 0x40000000;
  if (visible != 0) {
    style2 = 0x50000000;
  }
  if (style_flags != 0) {
    style2 = style2 | 1;
  }
  if (visible != 0) {
    style_flags = style_flags | 0x10000000;
  }
  if (scrollbar_class_registered == 0) {
    create_param = (LPVOID)0x0;
    menu = (HMENU)0x0;
    hinst = err_report_ctx_block.app_hinstance;
    parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
    window = parent_wnd->get_window();
    parent_hwnd = window->hwnd;
    height = rect_height((RECT *)rect);
    value = rect_width((RECT *)rect);
    hwnd2 = CreateWindowExA(4,str_scrollbar,((i8 *)&scrollbar_probe_wnd_title),style_flags | 0x40000000,*rect,
                              rect[1],value,height,parent_hwnd,menu,hinst,create_param);
    default_draw_font_obj_block.scrollbar_base_wndproc = (WNDPROC)GetWindowLongA(hwnd2,-4);
    style2 = GetWindowLongA(hwnd2,-0x10);
    wc.style = 0x8b;
    wc.lpfnWndProc = default_draw_font_obj_block.scrollbar_base_wndproc;
    wc.cbClsExtra = 0;
    default_draw_font_obj_block.scrollbar_base_wnd_extra = GetClassLongA(hwnd2,-0x12);
    wc.cbWndExtra = default_draw_font_obj_block.scrollbar_base_wnd_extra + 8;
    wc.hInstance = err_report_ctx_block.app_hinstance;
    wc.hIcon = (HICON)0x0;
    wc.hCursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f00);
    wc.hbrBackground = GetStockObject(1);
    wc.lpszMenuName = (LPCSTR)0x0;
    wc.lpszClassName = str_msscrollbarclass;
    RegisterClassA(&wc);
    scrollbar_class_registered = 1;
    DestroyWindow(hwnd2);
  }
  create_param = (LPVOID)0x0;
  menu = (HMENU)0x0;
  hinst = err_report_ctx_block.app_hinstance;
  parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
  value = (int)parent_wnd->get_window();
  parent_hwnd = ((NativeWindow *)value)->hwnd;
  value = rect_height((RECT *)rect);
  nWidth = rect_width((RECT *)rect);
  hwnd2 = CreateWindowExA(4,str_msscrollbarclass_2,((i8 *)&scrollbar_wnd_title),style2,*rect,rect[1]
                            ,nWidth,value,parent_hwnd,menu,hinst,create_param);
  SetWindowLongA(hwnd2,-4,(iptr)control_subclass_wndproc);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.scrollbar_base_wnd_extra,ctrl);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.scrollbar_base_wnd_extra + 4,(LONG)default_draw_font_obj_block.scrollbar_base_wndproc);
  ShowScrollBar(hwnd2,2,1);
  scrollbar_hwnd_set_range(hwnd2,1,100);
  return hwnd2;
}

/* 005cd139 auto */
void __cdecl scrollbar_destroy_hwnd(i32 hwnd)

{
  return;
}

/* 005cd149 auto */
void __cdecl scrollbar_hwnd_handle_msg(HWND hwnd,uint msg,uint wparam,int lparam)

{
  short thumb_pos;
  DWORD class_extra;
  Scrollbar *self;
  iptr cur_pos;
  iptr parent;
  WPARAM *rect_ptr;
  iptr page_size;
  int value;
  u32 new_pos;
  LPARAM pos_lparam;
  
  class_extra = GetClassLongA(hwnd,-0x12);
  self = (Scrollbar *)GetWindowLongA(hwnd,class_extra - 8);
  cur_pos = self->get_cur_pos();
  if (msg == 0x111) {
    if (wparam == 0x7f) {
      SetScrollPos(hwnd,2,lparam,1);
      new_pos = GetScrollPos(hwnd,2);
      self->cur_pos = new_pos;
    }
  }
  else if ((0x113 < msg) && (msg < 0x116)) {
    thumb_pos = (short)(wparam >> 0x10);
    switch(wparam & 0xffff) {
    case 0:
      value = input_lbutton_down();
      if (value == 0) {
        value = self->get_rect();
        pos_lparam = *(LPARAM *)(value + 4);
        rect_ptr = (WPARAM *)self->get_rect();
        PostMessageA(hwnd,0x202,*rect_ptr,pos_lparam);
      }
      else {
        SetScrollPos(hwnd,2,cur_pos - 1,1);
        new_pos = GetScrollPos(hwnd,2);
        parent = self->get_parent();
        wnd_set_active_child(parent);
        self->cur_pos = new_pos;
        self->fire_change(new_pos);
      }
      break;
    case 1:
      value = input_lbutton_down();
      if (value == 0) {
        value = self->get_rect();
        pos_lparam = *(LPARAM *)(value + 4);
        rect_ptr = (WPARAM *)self->get_rect();
        PostMessageA(hwnd,0x202,*rect_ptr,pos_lparam);
      }
      else {
        SetScrollPos(hwnd,2,cur_pos + 1,1);
        new_pos = GetScrollPos(hwnd,2);
        parent = self->get_parent();
        wnd_set_active_child(parent);
        self->cur_pos = new_pos;
        self->fire_change(new_pos);
      }
      break;
    case 2:
      value = self->get_page_size();
      SetScrollPos(hwnd,2,cur_pos - value,1);
      new_pos = GetScrollPos(hwnd,2);
      parent = self->get_parent();
      wnd_set_active_child(parent);
      self->cur_pos = new_pos;
      self->fire_change(new_pos);
      break;
    case 3:
      page_size = self->get_page_size();
      SetScrollPos(hwnd,2,cur_pos + page_size,1);
      new_pos = GetScrollPos(hwnd,2);
      parent = self->get_parent();
      wnd_set_active_child(parent);
      self->cur_pos = new_pos;
      self->fire_change(new_pos);
      break;
    case 4:
      SetScrollPos(hwnd,2,(int)thumb_pos,1);
      parent = self->get_parent();
      wnd_set_active_child(parent);
      self->cur_pos = (int)thumb_pos;
      self->fire_change(self->cur_pos);
      break;
    case 5:
      parent = self->get_parent();
      wnd_set_active_child(parent);
      self->fire_track((int)thumb_pos);
    }
  }
  return;
}

/* 005cd4c7 auto */
void __cdecl scrollbar_hwnd_set_range(HWND hwnd,int min_pos,int max_pos)

{
  SetScrollRange(hwnd,2,min_pos,max_pos,1);
  scrollbar_hwnd_set_pos(hwnd,min_pos);
  return;
}

/* 005cd535 auto */
void __cdecl scrollbar_hwnd_get_range(HWND hwnd,LPINT min_out,LPINT max_out)

{
  GetScrollRange(hwnd,2,min_out,max_out);
  return;
}

/* 005cd559 auto */
void __cdecl scrollbar_hwnd_set_pos(i32 hwnd,i32 pos)

{
  scrollbar_hwnd_handle_msg(hwnd,0x111,0x7f,pos);
  return;
}

/* 005cd620 auto */
u32 Scrollbar::get_cur_pos()

{
  return cur_pos;
}

/* 005cd640 auto */
void Scrollbar::fire_change(i32 pos)

{
  if (on_change != 0) {
    on_change(pos);
  }
  return;
}

/* 005cd680 auto */
void Scrollbar::fire_track(i32 pos)

{
  if (on_track != 0) {
    on_track(pos);
  }
  return;
}

/* 005cd6c0 auto */
u32 Scrollbar::get_page_size()

{
  return page_size;
}

/* 005d25c0 auto */
void EditControl::create_at(iptr parent,i32 ctrl_id,int x,int y,int width, const char *text)

{
  tagRECT rect;
  
  edit_font = (u32)(*(i8 * *)(draw_target.font_rest + 12));
  on_change = 0;
  SetRect(&rect,x,y,width + x,y + 0x1e);
  create(parent,ctrl_id,(iptr)&rect,text);
  return;
}

/* 005d2740 auto */
HWND __cdecl edit_create_window(int *rect,iptr ctrl,uint flags,int visible,GameFont *font)

{
  WndBase *parent_wnd;
  NativeWindow *window;
  iptr height;
  int value;
  int nWidth;
  i32 font_handle;
  WPARAM *font_ptr;
  HWND parent_hwnd;
  HMENU menu;
  HINSTANCE hinst;
  LPVOID create_param;
  WNDCLASSA wc;
  HWND hwnd2;
  uint style2;
  
  style2 = 0x40800000;
  if (visible != 0) {
    style2 = 0x50800000;
  }
  if ((flags & 1) != 0) {
    style2 = style2 | 0x80;
  }
  if (((flags & 2) != 0) || ((flags & 0x200) != 0)) {
    style2 = style2 | 4;
    *(i8 *)(ctrl + 0x38) = 1;
    if ((flags & 0x200) != 0) {
      *(i8 *)(ctrl + 0x39) = 1;
    }
  }
  if ((flags & 4) != 0) {
    style2 = style2 | 0xa0;
  }
  if ((flags & 8) != 0) {
    style2 = style2 | 8;
  }
  if ((flags & 0x10) != 0) {
    style2 = style2 | 0x10;
  }
  if ((flags & 0x20) != 0) {
    style2 = style2 | 0x200040;
  }
  if ((flags & 0x40) != 0) {
    style2 = style2 | 0x100080;
  }
  if ((flags & 0x80) != 0) {
    style2 = style2 | 0x1000;
  }
  if ((flags & 0x100) != 0) {
    style2 = style2 | 0x800;
  }
  *(uint *)(ctrl + 0x3c) = style2;
  if (editbox_class_registered == 0) {
    create_param = (LPVOID)0x0;
    menu = (HMENU)0x0;
    hinst = err_report_ctx_block.app_hinstance;
    parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
    window = parent_wnd->get_window();
    parent_hwnd = window->hwnd;
    height = rect_height((RECT *)rect);
    value = rect_width((RECT *)rect);
    hwnd2 = CreateWindowExA(4,((i8 *)&editbox_native_class_name),((i8 *)&editbox_probe_wnd_title),style2,*rect,rect[1],value,height,
                              parent_hwnd,menu,hinst,create_param);
    default_draw_font_obj_block.edit_base_wndproc = (WNDPROC)GetWindowLongA(hwnd2,-4);
    wc.style = 0x88;
    wc.cbClsExtra = 0;
    wc.lpfnWndProc = default_draw_font_obj_block.edit_base_wndproc;
    default_draw_font_obj_block.edit_base_wnd_extra = GetClassLongA(hwnd2,-0x12);
    wc.cbWndExtra = default_draw_font_obj_block.edit_base_wnd_extra + 8;
    wc.hInstance = err_report_ctx_block.app_hinstance;
    wc.hIcon = (HICON)0x0;
    wc.hCursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f01);
    wc.hbrBackground = (HBRUSH)0x0;
    wc.lpszMenuName = (LPCSTR)0x0;
    wc.lpszClassName = str_mseditboxclass;
    RegisterClassA(&wc);
    editbox_class_registered = 1;
    DestroyWindow(hwnd2);
  }
  create_param = (LPVOID)0x0;
  menu = (HMENU)0x0;
  hinst = err_report_ctx_block.app_hinstance;
  parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
  value = (int)parent_wnd->get_window();
  parent_hwnd = ((NativeWindow *)value)->hwnd;
  value = rect_height((RECT *)rect);
  nWidth = rect_width((RECT *)rect);
  hwnd2 = CreateWindowExA(4,str_mseditboxclass_2,((i8 *)&editbox_wnd_title),style2 | 0x40800000,*rect,
                            rect[1],nWidth,value,parent_hwnd,menu,hinst,create_param);
  font_handle = font->get_handle();
  font_ptr = (WPARAM *)mem_handle_lock(font_handle);
  SendMessageA(hwnd2,0x30,*font_ptr,0);
  font_handle = font->get_handle();
  mem_handle_unlock(font_handle);
  SetWindowLongA(hwnd2,-4,(iptr)edit_subclass_wndproc);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.edit_base_wnd_extra,(LONG)default_draw_font_obj_block.edit_base_wndproc);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.edit_base_wnd_extra + 4,ctrl);
  return hwnd2;
}

/* 005d2a01 auto */
LRESULT __stdcall edit_subclass_wndproc(HWND hwnd,uint msg,WPARAM wparam,LPARAM lparam)

{
  Button *self;
  DWORD class_extra;
  WNDPROC lpPrevWndFunc;
  iptr key;
  HWND parent_hwnd;
  iptr parent;
  WndBase *this_00;
  NativeWindow *window;
  LRESULT result;
  iptr long_index;
  int pass_through;
  
  self = (Button *)GetWindowLongA(hwnd,10);
  class_extra = GetClassLongA(hwnd,-0x12);
  lpPrevWndFunc = (WNDPROC)GetWindowLongA(hwnd,class_extra - 8);
  if (msg < 8) {
    if (msg == 7) {
      parent = self->get_parent();
      wnd_set_active_child(parent);
      if ((char)self->font == '\0') {
        edit_hwnd_set_sel(hwnd,0,0xffffffff);
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,7,wparam,lparam);
      return result;
    }
    if (msg == 2) {
      if (self != (Button *)0x0) {
        self->set_hwnd(0);
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,2,wparam,lparam);
      return result;
    }
  }
  else if ((msg == 0x201) || (msg == 0x204)) {
    long_index = 0;
    parent_hwnd = GetParent(hwnd);
    this_00 = (WndBase *)GetWindowLongA(parent_hwnd,long_index);
    window = this_00->get_window();
    if ((window->flags_bytes[1] & 2) != 0) {
      parent_hwnd = GetParent(hwnd);
      SetFocus(parent_hwnd);
      parent_hwnd = GetParent(hwnd);
      BringWindowToTop(parent_hwnd);
    }
    result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
    return result;
  }
  else if (msg < 0x202) {
    if (msg == 0x100) {
      if (((wparam != 9) && (wparam != 0xd)) && (wparam != 0x1b)) {
        result = CallWindowProcA(lpPrevWndFunc,hwnd,0x100,wparam,lparam);
        return result;
      }
      parent_hwnd = GetParent(hwnd);
      SendMessageA(parent_hwnd,msg,wparam,lparam);
      return 1;
    }
    if (msg == 0x101) {
      parent = self->get_parent();
      wnd_set_active_child(parent);
      key = input_map_vkey(wparam);
      if ((0x2af < key) && (key < 0x2b5)) {
        ((EditControl *)self)->fire_key_callback(key);
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,0x101,wparam,lparam);
      return result;
    }
    if (msg == 0x102) {
      parent = self->get_parent();
      wnd_set_active_child(parent);
      pass_through = 0;
      if (wparam != 9) {
        parent = input_map_vkey(wparam);
        pass_through = ((EditControl *)self)->fire_key_callback(parent);
      }
      if (pass_through == 0) {
        return 1;
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,0x102,wparam,lparam);
      return result;
    }
  }
  result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
  return result;
}

/* 005d2d3d auto */
void __cdecl edit_hwnd_cleanup_stub(i32 hwnd)

{
  return;
}

/* 005d2da1 auto */
void __cdecl edit_hwnd_limit_text(HWND hwnd,WPARAM max_len)

{
  SendMessageA(hwnd,0xc5,max_len,0);
  return;
}

/* 005d2dc6 auto */
void __cdecl edit_hwnd_set_sel(HWND hwnd,WPARAM sel_start,LPARAM sel_end)

{
  SendMessageA(hwnd,0xb1,sel_start,sel_end);
  return;
}

/* 005d2ded auto */
iptr __cdecl edit_hwnd_get_line(HWND hwnd,WPARAM line,short *buffer,i32 buf_size)

{
  iptr copied_len;
  
  *buffer = (short)buf_size - 1;
  copied_len = SendMessageA(hwnd,0xc4,line,(LPARAM)buffer);
  *(i8 *)(copied_len + (int)buffer) = 0;
  return copied_len;
}

/* 005d2e31 auto */
LRESULT __cdecl edit_hwnd_get_line_count(HWND hwnd)

{
  LRESULT line_count;
  
  line_count = SendMessageA(hwnd,0xba,0,0);
  return line_count;
}

/* 005d2e9c auto */
LRESULT __cdecl edit_line_index(HWND hwnd,WPARAM line)

{
  LRESULT line_index;
  
  line_index = SendMessageA(hwnd,0xbb,line,0);
  return line_index;
}

/* 005d2ec1 auto */
LRESULT __cdecl edit_line_length(HWND hwnd,WPARAM char_index)

{
  LRESULT line_start;
  
  line_start = SendMessageA(hwnd,0xc1,char_index,0);
  return line_start;
}

/* 005d2f0d auto */
void __cdecl edit_hwnd_set_caret(HWND hwnd,WPARAM pos)

{
  SendMessageA(hwnd,0xb1,pos,pos);
  SendMessageA(hwnd,0xb7,0,0);
  return;
}

/* 005d2f47 auto */
uint __cdecl edit_ctl_get_sel_end(HWND hwnd)

{
  uint sel;
  
  sel = SendMessageA(hwnd,0xb0,0,0);
  return sel >> 0x10;
}

/* 005d2f7e auto */
int __cdecl edit_hwnd_get_text_end(i32 hwnd)

{
  int line_start;
  int line_len;
  
  line_start = edit_hwnd_get_line_count(hwnd);
  line_start = edit_line_index(hwnd,line_start - 1);
  line_len = edit_line_length(hwnd,line_start);
  return line_start + line_len;
}

/* 005d3035 auto */
void __cdecl edit_hwnd_replace_sel(HWND hwnd,LPARAM text)

{
  SendMessageA(hwnd,0xc2,0,text);
  return;
}

/* 005d30e0 auto */
i32 EditControl::fire_key_callback(iptr key)

{
  i32 result;
  
  if (on_key == 0) {
    result = 1;
  }
  else {
    result = on_key(ctrl_id,key);
  }
  return result;
}

/* 005d3130 auto */
HWND __cdecl combo_create_window(int *rect,iptr ctrl,int visible)

{
  WndBase *parent_wnd;
  NativeWindow *window;
  iptr height;
  int value;
  int nWidth;
  HWND parent_hwnd;
  HMENU menu;
  HINSTANCE hinst;
  LPVOID create_param;
  DWORD style2;
  WNDCLASSA wc;
  HWND hwnd2;
  
  style2 = 0x40a10003;
  if (visible != 0) {
    style2 = 0x50a10003;
  }
  if (combo_class_registered == 0) {
    create_param = (LPVOID)0x0;
    menu = (HMENU)0x0;
    hinst = err_report_ctx_block.app_hinstance;
    parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
    window = parent_wnd->get_window();
    parent_hwnd = window->hwnd;
    height = rect_height((RECT *)rect);
    value = rect_width((RECT *)rect);
    hwnd2 = CreateWindowExA(4,str_combobox,((i8 *)&combo_probe_wnd_title),style2,*rect,rect[1],value
                              ,height,parent_hwnd,menu,hinst,create_param);
    default_draw_font_obj_block.combo_base_wndproc = (WNDPROC)GetWindowLongA(hwnd2,-4);
    wc.style = 0x88;
    wc.cbClsExtra = 0;
    wc.lpfnWndProc = default_draw_font_obj_block.combo_base_wndproc;
    default_draw_font_obj_block.combo_base_wnd_extra = GetClassLongA(hwnd2,-0x12);
    wc.cbWndExtra = default_draw_font_obj_block.combo_base_wnd_extra + 8;
    wc.hInstance = err_report_ctx_block.app_hinstance;
    wc.hIcon = (HICON)0x0;
    wc.hCursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f00);
    wc.hbrBackground = (HBRUSH)0x0;
    wc.lpszMenuName = (LPCSTR)0x0;
    wc.lpszClassName = str_mscomboboxclass;
    RegisterClassA(&wc);
    combo_class_registered = 1;
    DestroyWindow(hwnd2);
  }
  style2 = 0x40a10203;
  if (visible != 0) {
    style2 = 0x50a10203;
  }
  create_param = (LPVOID)0x0;
  menu = (HMENU)0x0;
  hinst = err_report_ctx_block.app_hinstance;
  parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
  value = (int)parent_wnd->get_window();
  parent_hwnd = ((NativeWindow *)value)->hwnd;
  value = rect_height((RECT *)rect);
  nWidth = rect_width((RECT *)rect);
  hwnd2 = CreateWindowExA(4,str_mscomboboxclass_2,((i8 *)&combo_wnd_title),style2,*rect,rect[1],
                            nWidth,value,parent_hwnd,menu,hinst,create_param);
  SetWindowLongA(hwnd2,-4,(iptr)combo_subclass_wndproc);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.combo_base_wnd_extra,(LONG)default_draw_font_obj_block.combo_base_wndproc);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.combo_base_wnd_extra + 4,ctrl);
  return hwnd2;
}

/* 005d3310 auto */
LRESULT __stdcall combo_subclass_wndproc(HWND hwnd,uint msg,uint wparam,LPARAM lparam)

{
  DWORD class_extra;
  WNDPROC lpPrevWndFunc;
  Button *self;
  iptr parent;
  HWND parent_hwnd;
  WndBase *this_00;
  NativeWindow *window;
  LRESULT result;
  iptr long_index;
  
  class_extra = GetClassLongA(hwnd,-0x12);
  lpPrevWndFunc = (WNDPROC)GetWindowLongA(hwnd,class_extra - 8);
  self = (Button *)GetWindowLongA(hwnd,class_extra - 4);
  if (self == (Button *)0x0) {
    return 1;
  }
  parent = self->get_parent();
  wnd_set_active_child(parent);
  if (msg < 0x103) {
    if (0xff < msg) {
      if (((wparam != 9) && (wparam != 0xd)) && (wparam != 0x1b)) {
        result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
        return result;
      }
      parent_hwnd = GetParent(hwnd);
      SendMessageA(parent_hwnd,msg,wparam,lparam);
      return 1;
    }
    if (msg == 2) {
      if (self != (Button *)0x0) {
        self->set_hwnd(0);
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,2,wparam,lparam);
      return result;
    }
  }
  else {
    if ((msg == 0x201) || (msg == 0x204)) {
      long_index = 0;
      parent_hwnd = GetParent(hwnd);
      this_00 = (WndBase *)GetWindowLongA(parent_hwnd,long_index);
      window = this_00->get_window();
      if ((window->flags_bytes[1] & 2) != 0) {
        parent_hwnd = GetParent(hwnd);
        SetFocus(parent_hwnd);
        parent_hwnd = GetParent(hwnd);
        BringWindowToTop(parent_hwnd);
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
      return result;
    }
    if (msg == 0x4c8) {
      if (wparam >> 0x10 == 1) {
        if (self == (Button *)0x0) {
          return 1;
        }
        ((ComboBox *)self)->fire_selchange();
        return 1;
      }
      if (wparam >> 0x10 != 2) {
        return 1;
      }
      ((ComboBox *)self)->fire_dblclick();
      return 1;
    }
  }
  result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
  return result;
}

/* 005d356e auto */
void __cdecl combo_hwnd_cleanup_stub(i32 hwnd)

{
  return;
}

/* 005d357e auto */
void __cdecl combo_hwnd_add_string(HWND hwnd,LPARAM text)

{
  SendMessageA(hwnd,0x143,0,text);
  return;
}

/* 005d35c8 auto */
void __cdecl combo_hwnd_set_font(HWND hwnd,i32 font_handle)

{
  WPARAM *font_ptr;
  
  font_ptr = (WPARAM *)mem_handle_lock(font_handle);
  SendMessageA(hwnd,0x30,*font_ptr,0);
  mem_handle_unlock(font_handle);
  return;
}

/* 005d360a auto */
void __cdecl combo_hwnd_clear(HWND hwnd)

{
  SendMessageA(hwnd,0x14b,0,0);
  return;
}

/* 005d36b1 auto */
LRESULT __cdecl combo_hwnd_get_cur_sel(HWND hwnd)

{
  LRESULT cur_sel;
  
  cur_sel = SendMessageA(hwnd,0x147,0,0);
  if (cur_sel == -1) {
    cur_sel = -1;
  }
  return cur_sel;
}

/* 005d36f6 auto */
void __cdecl combo_hwnd_set_cur_sel(HWND hwnd,WPARAM index)

{
  SendMessageA(hwnd,0x14e,index,0);
  return;
}

/* 005d3720 auto */
void ComboBox::fire_dblclick()

{
  if (on_dblclick != 0) {
    on_dblclick(ctrl_id);
  }
  return;
}

/* 005d3760 auto */
void ComboBox::fire_selchange()

{
  if (on_select != 0) {
    on_select(ctrl_id);
  }
  return;
}

/* 005d37a0 auto */
HWND __cdecl listbox_create_hwnd(int *rect,iptr ctrl,int visible,int multi_column,GameFont *font)

{
  uint style_bits;
  WndBase *parent_wnd;
  NativeWindow *window;
  iptr height;
  int value;
  int nWidth;
  i32 font_handle;
  WPARAM *font_ptr;
  HWND parent_hwnd;
  HMENU menu;
  HINSTANCE hinst;
  LPVOID create_param;
  uint style2;
  WNDCLASSA wc;
  HWND hwnd2;
  
  style2 = 0;
  if (multi_column == 1) {
    style2 = 0x800;
  }
  style_bits = style2 | 0x40a10000;
  if (visible != 0) {
    style_bits = style2 | 0x50a10000;
  }
  style2 = style_bits;
  if (listbox_class_registered == 0) {
    create_param = (LPVOID)0x0;
    menu = (HMENU)0x0;
    hinst = err_report_ctx_block.app_hinstance;
    parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
    window = parent_wnd->get_window();
    parent_hwnd = window->hwnd;
    height = rect_height((RECT *)rect);
    value = rect_width((RECT *)rect);
    hwnd2 = CreateWindowExA(4,str_listbox_2,((i8 *)&listbox_probe_wnd_title),style2,*rect,rect[1],value,
                              height,parent_hwnd,menu,hinst,create_param);
    default_draw_font_obj_block.listbox_base_wndproc = (WNDPROC)GetWindowLongA(hwnd2,-4);
    wc.style = 0x88;
    wc.cbClsExtra = 0;
    wc.lpfnWndProc = default_draw_font_obj_block.listbox_base_wndproc;
    default_draw_font_obj_block.listbox_base_wnd_extra = GetClassLongA(hwnd2,-0x12);
    wc.cbWndExtra = default_draw_font_obj_block.listbox_base_wnd_extra + 8;
    wc.hInstance = err_report_ctx_block.app_hinstance;
    wc.hIcon = (HICON)0x0;
    wc.hCursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f00);
    wc.hbrBackground = (HBRUSH)0x0;
    wc.lpszMenuName = (LPCSTR)0x0;
    wc.lpszClassName = str_mslistboxclass;
    RegisterClassA(&wc);
    listbox_class_registered = 1;
    DestroyWindow(hwnd2);
  }
  style2 = 0x40a10001;
  if (visible != 0) {
    style2 = 0x50a10001;
  }
  if (multi_column == 1) {
    style2 = style2 | 0x800;
  }
  create_param = (LPVOID)0x0;
  menu = (HMENU)0x0;
  hinst = err_report_ctx_block.app_hinstance;
  parent_wnd = (WndBase *)((Control *)ctrl)->get_parent();
  value = (int)parent_wnd->get_window();
  parent_hwnd = ((NativeWindow *)value)->hwnd;
  value = rect_height((RECT *)rect);
  nWidth = rect_width((RECT *)rect);
  hwnd2 = CreateWindowExA(4,str_mslistboxclass_2,((i8 *)&listbox_wnd_title),style2,*rect,rect[1],
                            nWidth,value,parent_hwnd,menu,hinst,create_param);
  font_handle = font->get_handle();
  font_ptr = (WPARAM *)mem_handle_lock(font_handle);
  SendMessageA(hwnd2,0x30,*font_ptr,0);
  font_handle = font->get_handle();
  mem_handle_unlock(font_handle);
  SetWindowLongA(hwnd2,-4,(iptr)listbox_subclass_wndproc);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.listbox_base_wnd_extra,(LONG)default_draw_font_obj_block.listbox_base_wndproc);
  SetWindowLongA(hwnd2,default_draw_font_obj_block.listbox_base_wnd_extra + 4,ctrl);
  return hwnd2;
}

/* 005d39e2 auto */
LRESULT __stdcall listbox_subclass_wndproc(HWND hwnd,uint msg,uint wparam,LPARAM lparam)

{
  DWORD class_extra;
  WNDPROC lpPrevWndFunc;
  Button *self;
  iptr parent;
  HWND parent_hwnd;
  WndBase *this_00;
  NativeWindow *window;
  LRESULT result;
  iptr long_index;
  
  class_extra = GetClassLongA(hwnd,-0x12);
  lpPrevWndFunc = (WNDPROC)GetWindowLongA(hwnd,class_extra - 8);
  self = (Button *)GetWindowLongA(hwnd,class_extra - 4);
  if (self == (Button *)0x0) {
    return 1;
  }
  parent = self->get_parent();
  wnd_set_active_child(parent);
  if (msg < 0x103) {
    if (0xff < msg) {
      if (((wparam != 9) && (wparam != 0xd)) && (wparam != 0x1b)) {
        result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
        return result;
      }
      parent_hwnd = GetParent(hwnd);
      SendMessageA(parent_hwnd,msg,wparam,lparam);
      return 1;
    }
    if (msg == 2) {
      if (self != (Button *)0x0) {
        self->set_hwnd(0);
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,2,wparam,lparam);
      return result;
    }
  }
  else {
    if ((msg == 0x201) || (msg == 0x204)) {
      long_index = 0;
      parent_hwnd = GetParent(hwnd);
      this_00 = (WndBase *)GetWindowLongA(parent_hwnd,long_index);
      window = this_00->get_window();
      if ((window->flags_bytes[1] & 2) != 0) {
        parent_hwnd = GetParent(hwnd);
        SetFocus(parent_hwnd);
        parent_hwnd = GetParent(hwnd);
        BringWindowToTop(parent_hwnd);
      }
      result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
      return result;
    }
    if (msg == 0x4c8) {
      if (wparam >> 0x10 == 1) {
        if (self == (Button *)0x0) {
          return 1;
        }
        ((ListBox *)self)->fire_selchange();
        return 1;
      }
      if (wparam >> 0x10 != 2) {
        return 1;
      }
      ((ListBox *)self)->fire_dblclick();
      return 1;
    }
  }
  result = CallWindowProcA(lpPrevWndFunc,hwnd,msg,wparam,lparam);
  return result;
}

/* 005d3c40 auto */
void __cdecl listbox_hwnd_cleanup_stub(i32 hwnd)

{
  return;
}

/* 005d3c50 auto */
void __cdecl listbox_hwnd_add_string(HWND hwnd,LPARAM text)

{
  SendMessageA(hwnd,0x180,0,text);
  return;
}

/* 005d3c9a auto */
void __cdecl listbox_hwnd_set_font(HWND hwnd,i32 font_handle)

{
  WPARAM *font_ptr;
  
  font_ptr = (WPARAM *)mem_handle_lock(font_handle);
  SendMessageA(hwnd,0x30,*font_ptr,0);
  mem_handle_unlock(font_handle);
  return;
}

/* 005d3cdc auto */
void __cdecl listbox_hwnd_clear(HWND hwnd)

{
  SendMessageA(hwnd,0x184,0,0);
  return;
}

/* 005d3cff auto */
void __cdecl listbox_hwnd_get_text(HWND hwnd,WPARAM index,LPARAM buffer)

{
  SendMessageA(hwnd,0x189,index,buffer);
  return;
}

/* 005d3d62 auto */
void __cdecl listbox_hwnd_get_sel_text(HWND hwnd,char *buffer)

{
  LRESULT cur_sel;
  
  cur_sel = SendMessageA(hwnd,0x188,0,0);
  if (cur_sel == -1) {
    crt_strcpy((char *)((void *)buffer),((i8 *)&listbox_no_sel_text));
  }
  else {
    listbox_hwnd_get_text(hwnd,cur_sel,(LPARAM)(buffer));
  }
  return;
}

/* 005d3dbf auto */
LRESULT __cdecl listbox_hwnd_get_cur_sel(HWND hwnd)

{
  LRESULT cur_sel;
  
  cur_sel = SendMessageA(hwnd,0x188,0,0);
  if (cur_sel == -1) {
    cur_sel = -1;
  }
  return cur_sel;
}

/* 005d3e92 auto */
void __cdecl listbox_hwnd_set_cur_sel(HWND hwnd,WPARAM index)

{
  SendMessageA(hwnd,0x186,index,0);
  return;
}

/* 005d3f30 auto */
void ListBox::fire_dblclick()

{
  if (on_dblclick != 0) {
    on_dblclick(ctrl_id);
  }
  return;
}

/* 005d3f70 auto */
void ListBox::fire_selchange()

{
  if (on_select != 0) {
    on_select(ctrl_id);
  }
  return;
}

/* 005d4014 auto */
void RadioGroup::show_all()

{
  int i;
  
  for (i = 0; i < (int)item_count; i++) {
    hwnd_show((i32)((RadioItem *)items)[i].hwnd);
    wnd_invalidate((i32)((RadioItem *)items)[i].hwnd);
  }
  return;
}

/* 005d4167 auto */
void __cdecl ctrl_set_pending_font(iptr font)

{
  (*(i8 * *)(draw_target.font_rest + 4)) = (i8 *)font;
  return;
}

/* 005d7cb0 auto */
void SpriteButton::init_from_sprite(WndBase *parent,i32 ctrl_id,int x,int y, int width,int height,iptr sprite,uint bg_color)

{
  u32 value;
  int size;
  WndBase *parent_wnd;
  i32 palette;
  int yBottom;
  i8 draw_ctx_b[16];
  i8 draw_ctx_a[16];
  i32 origin_y2;
  i32 origin_x2;
  i32 scale_den2;
  i32 scale_num2;
  int center_y;
  int center_x;
  tagRECT rect;
  int half_h;
  int half_w;
  tagRECT dst_rect;
  RECT src_rect;
  
  pressed_state = '\0';
  value = parent->get_palette();
  ctrl_palette = value;
  SetRect(&rect,x,y,width + x,y + height);
  init((iptr)parent,6,ctrl_id,&rect);
  on_click = 0;
  on_rclick = 0;
  gfx_get_scale_ratio((iptr)&scale_num2,(iptr)&scale_den2);
  gfx_set_scale_ratio(1,1);
  ((DibSurface *)button_surface)->DibSurface::create(width,height);
  ((DibSurface *)&on_select)->DibSurface::create(width,height);
  ((DibSurface *)button_surface)->fill(bg_color & 0xff);
  ((DibSurface *)&on_select)->fill(bg_color & 0xff);
  center_x = width >> 1;
  center_y = height >> 1;
  ((Sprite *)sprite)->get_origin((iptr)&origin_x2,(iptr)&origin_y2);
  half_w = ((Sprite *)sprite)->get_width();
  half_w = half_w >> 1;
  half_h = ((Sprite *)sprite)->get_height();
  half_h = half_h >> 1;
  ((Sprite *)sprite)->set_origin(-half_w,-half_h);
  ((Sprite *)sprite)->draw((iptr)draw_ctx_a,(iptr)(button_surface),center_x,center_y);
  ((Sprite *)sprite)->draw((iptr)draw_ctx_b,(iptr)&on_select,center_x,center_y);
  ((Sprite *)sprite)->set_origin(origin_x2,origin_y2);
  gfx_set_scale_ratio(scale_num2,scale_den2);
  size = ((DibSurface *)button_surface)->surface_height();
  yBottom = y + size - 1;
  size = ((DibSurface *)button_surface)->surface_width();
  SetRect(&rect,x,y,x + size - 1,yBottom);
  OffsetRect(&rect,-rect.left,-rect.top);
  dst_rect.left = rect.left;
  dst_rect.top = rect.top;
  dst_rect.right = rect.right;
  dst_rect.bottom = rect.bottom;
  SetRect(&dst_rect,rect.left,rect.top,rect.right - 2,rect.bottom - 2);
  src_rect.left = dst_rect.left;
  src_rect.top = dst_rect.top;
  src_rect.right = dst_rect.right;
  src_rect.bottom = dst_rect.bottom;
  OffsetRect(&dst_rect,2,2);
  ((DibSurface *)button_surface)->fill((uint)(*(byte *)&display_palette.black_index));
  ((DibSurface *)&on_select)->DibSurface::blit((DibSurface *)(iptr)(button_surface),&src_rect,
             &dst_rect);
  parent_wnd = (WndBase *)get_parent();
  palette = parent_wnd->get_palette();
  ((DibSurface *)button_surface)->apply_palette((GamePalette *)palette);
  parent_wnd = (WndBase *)get_parent();
  palette = parent_wnd->get_palette();
  ((DibSurface *)&on_select)->apply_palette((GamePalette *)palette);
  value = spritebtn_create_hwnd(x,y,width,height,(iptr)this,1);
  hwnd = value;
  return;
}

/* 005d8da1 auto */
int __cdecl radiogrp_item_height(GameFont *font)

{
  HDC hdc;
  HDC hdc_00;
  i32 font_handle;
  int height2;
  tagTEXTMETRICA metrics;
  i32 *font_data;
  
  hdc = GetDC((HWND)0x0);
  hdc_00 = CreateCompatibleDC(hdc);
  ReleaseDC((HWND)0x0,hdc);
  font_handle = font->get_handle();
  font_data = (i32 *)mem_handle_lock(font_handle);
  SelectObject(hdc_00,(HGDIOBJ)*font_data);
  font_handle = font->get_handle();
  mem_handle_unlock(font_handle);
  GetTextMetricsA(hdc_00,&metrics);
  height2 = metrics.tmHeight;
  DeleteDC(hdc_00);
  if (metrics.tmHeight < 0x14) {
    height2 = 0x14;
  }
  return height2;
}

/* 005d8e3f auto */
i32 __cdecl
radiogrp_create_items(iptr count_out,int *rect,int columns,iptr group,int item_count,iptr labels,int visible
            ,i32 font)

{
  int *hotkey_pos;
  HBITMAP bmp_unchecked;
  HBITMAP bmp_checked;
  HBITMAP bmp_focus;
  i32 handle;
  iptr items;
  int col_width;
  int row_height;
  int nHeight;
  WndBase *self;
  NativeWindow *window;
  int half_row;
  HWND item_hwnd;
  ControlBlock *block;
  size_t len;
  int half_item;
  HMENU hMenu;
  HINSTANCE hInstance;
  LPVOID lpParam;
  int item_x;
  DWORD style2;
  uint index;
  char *cursor;
  char *label;
  char hotkey[4];
  int item_y;
  
  bmp_unchecked = LoadBitmapA(err_report_ctx_block.app_hinstance,(LPCSTR)0x12d);
  bmp_checked = LoadBitmapA(err_report_ctx_block.app_hinstance,(LPCSTR)0x12f);
  bmp_focus = LoadBitmapA(err_report_ctx_block.app_hinstance,(LPCSTR)0x12e);
  handle = mem_handle_alloc(item_count * 0xa4);
  items = (iptr)mem_handle_lock((HGLOBAL)handle);
  col_width = rect_width((RECT *)rect);
  col_width = col_width / columns;
  row_height = rect_height((RECT *)rect);
  row_height = (row_height * columns) / item_count;
  nHeight = radiogrp_item_height((GameFont *)font);
  item_x = *rect;
  item_y = rect[1];
  style2 = 0x40010000;
  if (visible != 0) {
    style2 = 0x50010000;
  }
  index = 0;
  do {
    if (item_count <= (int)index) {
      *(int *)count_out = item_count;
      mem_handle_unlock(handle);
      return handle;
    }
    lpParam = (LPVOID)0x0;
    hMenu = (HMENU)0x0;
    hInstance = err_report_ctx_block.app_hinstance;
    self = (WndBase *)((Control *)group)->get_parent();
    window = self->get_window();
    half_row = row_height >> 1;
    half_item = nHeight >> 1;
    item_hwnd = CreateWindowExA(4,str_mscontrolclass_5,((i8 *)&radiogrp_item_wnd_title),style2,item_x,
                              (half_row - half_item) + item_y,col_width,nHeight,window->hwnd,hMenu,
                              hInstance,lpParam);
    ((RadioItem *)items)[index].hwnd = item_hwnd;
    block = ctrl_init_block((i32)((RadioItem *)items)[index].hwnd,group);
    if (index == 0) {
      block->shared_bitmaps = 0;
    }
    else {
      block->shared_bitmaps = 1;
    }
    block->bitmaps[0] = bmp_unchecked;
    block->bitmaps[1] = bmp_checked;
    block->bitmaps[2] = bmp_focus;
    block->data = (iptr)&((RadioItem *)items)[index];
    block->kind = 3;
    SetWindowLongA(((RadioItem *)items)[index].hwnd,-4,(iptr)radiogrp_item_wndproc);
    SetRect(&((RadioItem *)items)[index].rect,item_x,(half_row - half_item) + item_y,
            item_x + col_width,(half_row - half_item) + item_y + nHeight);
    ((RadioItem *)items)[index].index = index;
    label = *(char **)(labels + index * 4);
    if (*label == '!') {
      ((RadioItem *)items)[index].enabled = 0;
      label++;
    }
    else if (*label == '^') {
      ((RadioItem *)items)[index].enabled = 1;
      ((RadioGroup *)group)->set_selected(index);
      label++;
    }
    else {
      ((RadioItem *)items)[index].enabled = 1;
    }
    /* BF-151 (Peter Johnson's ledger): a label cut at 127 characters (the
       original copied a longer one over the next item) */
    str_copy_max(((RadioItem *)items)[index].label,(const char *)((void *)label),
                 sizeof ((RadioItem *)items)[index].label);
    cursor = ((RadioItem *)items)[index].label;
    ((RadioItem *)items)[index].hotkey_pos = 0;
    for (; *cursor != '\0'; cursor++) {
      if (*cursor == '~') {
        /* BF-152 (Peter Johnson's ledger): the original's walk, which takes
           the hotkey position for the item, within the items (it went on
           past them with a position past their number) */
        for (index = (uint)((RadioItem *)items)[index].hotkey_pos;
            (index < (uint)item_count) &&
            (len = _strlen(((RadioItem *)items)[index].label), index < len);
            index = index + 1) {
          *(i8 *)(index * 0xa5 + 0x1c + items) =
               *(i8 *)(index * 0xa5 + 0x1d + items);
        }
        break;
      }
      hotkey_pos = &((RadioItem *)items)[index].hotkey_pos;
      *hotkey_pos = *hotkey_pos + 1;
    }
    if (item_count <= (int)index) continue;  /* BF-152: no item there */
    len = _strlen(((RadioItem *)items)[index].label);
    if ((uint)((RadioItem *)items)[index].hotkey_pos < len) {
      hotkey[0] = ((RadioItem *)items)[index].label[((RadioItem *)items)[index].hotkey_pos];
      hotkey[1] = 0;
      __strlwr(hotkey);
      ((RadioItem *)items)[index].hotkey = hotkey[0];
    }
    else {
      ((RadioItem *)items)[index].hotkey_pos = 0xffffffff;
    }
    item_y += row_height;
    if (rect[3] <= item_y) {
      item_x += col_width;
      item_y = rect[1];
    }
    index++;
  } while( true );
}

/* 005d99f4 auto */
void __cdecl radiogrp_destroy_items(int count,int handle)

{
  iptr items;
  i32 i;
  
  if (handle == 0) {
    dbg_log((const char *)&str_error_tried_to_dismantle_null_radioset_i);
  }
  else {
    items = (iptr)mem_handle_lock((HGLOBAL)handle);
    for (i = 0; i < count; i++) {
      if ((int)((RadioItem *)items)[i].hwnd != 0) {
        DestroyWindow(((RadioItem *)items)[i].hwnd);
      }
    }
    mem_handle_unlock(handle);
    mem_handle_free(handle);
  }
  return;
}

/* 005d9a9a auto */
void __cdecl radiogrp_item_activate(HWND hwnd)

{
  RadioGroup *self;
  ControlBlock *block;
  iptr selected;
  iptr old_sel;
  iptr items;
  iptr new_sel;
  RadioItem *item;
  i32 sel_index;
  i32 ctrl_id;
  RECT *null_rect;
  BOOL erase;
  
  block = wnd_from_hwnd(hwnd);
  if (block != 0) {
    self = (RadioGroup *)block->owner;
    item = (RadioItem *)block->data;
    SetFocus(hwnd);
    selected = self->get_selected();
    if ((selected != item->index) && (item->enabled == 1)) {
      erase = 0;
      null_rect = (RECT *)0x0;
      old_sel = self->get_selected();
      items = self->get_items();
      InvalidateRect(((RadioItem *)items)[old_sel].hwnd,null_rect,erase);
      self->set_selected(item->index);
      erase = 0;
      null_rect = (RECT *)0x0;
      new_sel = self->get_selected();
      item = (RadioItem *)self->get_items();
      InvalidateRect(item[new_sel].hwnd,null_rect,erase);
    }
    modal_wnd.active_child = self->get_parent();
    sel_index = self->get_selected();
    ctrl_id = self->get_id();
    self->fire_callback(ctrl_id,sel_index);
  }
  return;
}

/* Part of radiogrp_item_wndproc (005d9b86): the label's text color, enabled or disabled
   (a palette entry in a palettized window, else the set RGB or black/grey). */
static void radiogrp_item_text_color(HDC hdc,RadioGroup *group,RadioItem *item)
{
  uint color_index;
  WndBase *parent_wnd;
  GamePalette *game_palette2;
  WndBase *parent_base;
  int value;
  iptr parent;
  i8 *red_out;
  i8 *green_out;
  i8 *blue_out;
  i8 green[4];
  i8 red[4];
  i8 blue[4];

  if (item->enabled == 1) {
    parent_base = (WndBase *)group->get_parent();
    value = (int)parent_base->get_window();
    if (((NativeWindow *)value)->field_40 == 8) {
      if (ctrl_colors.radio_text == 0) {
        SetTextColor(hdc,0);
      }
      else {
        blue_out = blue;
        green_out = green;
        red_out = red;
        color_index = (uint)ctrl_colors.radio_text;
        parent_wnd = (WndBase *)group->get_parent();
        game_palette2 = (GamePalette *)parent_wnd->get_palette();
        parent = (iptr)game_palette2->get_logpalette();
        palette_get_entry_rgb((GameLogPalette *)parent,color_index,(iptr)red_out,(iptr)green_out,(iptr)blue_out);
        SetTextColor(hdc,CONCAT12(blue[0],CONCAT11(green[0],red[0])));
      }
    }
    else if (radio_state.text_set == '\0') {
      SetTextColor(hdc,0);
    }
    else {
      SetTextColor(hdc,CONCAT12(radio_state.text_b,CONCAT11(radio_state.text_g,radio_state.text_r)));
    }
  }
  else if (item->enabled == 0) {
    parent_base = (WndBase *)group->get_parent();
    value = (int)parent_base->get_window();
    if (((NativeWindow *)value)->field_40 == 8) {
      if (ctrl_colors.radio_disabled == 0) {
        SetTextColor(hdc,0x404040);
      }
      else {
        blue_out = blue;
        green_out = green;
        red_out = red;
        color_index = (uint)ctrl_colors.radio_disabled;
        parent_wnd = (WndBase *)group->get_parent();
        game_palette2 = (GamePalette *)parent_wnd->get_palette();
        parent = (iptr)game_palette2->get_logpalette();
        palette_get_entry_rgb((GameLogPalette *)parent,color_index,(iptr)red_out,(iptr)green_out,(iptr)blue_out);
        SetTextColor(hdc,CONCAT12(blue[0],CONCAT11(green[0],red[0])));
      }
    }
    else if (radio_state.disabled_set == '\0') {
      SetTextColor(hdc,0x404040);
    }
    else {
      SetTextColor(hdc,CONCAT12(radio_state.disabled_b,CONCAT11(radio_state.disabled_g,radio_state.disabled_r)));
    }
  }
}

/* Part of radiogrp_item_wndproc (005d9b86): underlines the hotkey letter of the label. */
static void radiogrp_item_underline_hotkey(HDC hdc,RadioItem *item,tagRECT *rect)
{
  size_t len;
  tagRECT *draw_rect;
  UINT draw_flags;
  int underline_end;
  int underline_start;
  HBRUSH brush;
  tagRECT text_rect;
  char prefix[128];

  if (item->hotkey_pos != -1) {
    crt_strcpy(prefix,item->label);
    prefix[item->hotkey_pos] = '\0';
    text_rect.left = rect->left;
    text_rect.top = rect->top;
    text_rect.right = rect->right;
    text_rect.bottom = rect->bottom;
    len = _strlen(prefix);
    if (len == 0) {
      underline_start = text_rect.left;
    }
    else {
      draw_flags = 0x424;
      draw_rect = &text_rect;
      len = _strlen(prefix);
      DrawTextA(hdc,prefix,len,draw_rect,draw_flags);
      underline_start = text_rect.right - 1;
    }
    crt_strcpy(prefix,item->label);
    prefix[item->hotkey_pos + 1] = '\0';
    text_rect.left = rect->left;
    text_rect.top = rect->top;
    text_rect.right = rect->right;
    text_rect.bottom = rect->bottom;
    draw_flags = 0x424;
    draw_rect = &text_rect;
    len = _strlen(prefix);
    DrawTextA(hdc,prefix,len,draw_rect,draw_flags);
    underline_start = underline_start + ((text_rect.right + 1) - underline_start >> 1) - 5;
    underline_end = underline_start + 10;
    brush = GetStockObject(3);
    SelectObject(hdc,brush);
    MoveToEx(hdc,underline_start,rect->bottom - 3,(LPPOINT)0x0);
    LineTo(hdc,underline_end,rect->bottom - 3);
    MoveToEx(hdc,underline_start,rect->bottom - 2,(LPPOINT)0x0);
    LineTo(hdc,underline_end,rect->bottom - 2);
  }
}

/* Part of radiogrp_item_wndproc (005d9b86): focuses and selects item
   next_index (the arrow keys), redraws the old and new selection, notifies. */
static void radiogrp_focus_select(RadioGroup *group,int next_index)
{
  int value;
  iptr parent;
  iptr items2;
  i32 ctrl_id;
  i32 result;
  RECT *null_rect;
  BOOL erase;

  value = group->get_items();
  SetFocus(((RadioItem *)value)[next_index].hwnd);
  erase = 0;
  null_rect = (RECT *)0x0;
  value = group->get_selected();
  items2 = group->get_items();
  InvalidateRect(((RadioItem *)items2)[value].hwnd,null_rect,erase);
  group->set_selected(next_index);
  erase = 0;
  null_rect = (RECT *)0x0;
  value = group->get_items();
  InvalidateRect(((RadioItem *)value)[next_index].hwnd,null_rect,erase);
  parent = group->get_parent();
  wnd_set_active_child(parent);
  result = group->get_selected();
  ctrl_id = group->get_id();
  group->fire_callback(ctrl_id,result);
}

/* Part of radiogrp_item_wndproc (005d9b86): right/down: selects the next enabled item
   (wrapping around), focuses it and notifies. */
static void radiogrp_select_next(RadioGroup *group,RadioItem *item)
{
  int value;
  int remaining;
  int next_index;

  remaining = group->radiogrp_item_count();
  next_index = item->index + 1;
  value = group->radiogrp_item_count();
  if (value <= next_index) {
    next_index = 0;
  }
  while ((0 < remaining &&
         (value = group->get_items(),
         ((RadioItem *)value)[next_index].enabled == 0))) {
    remaining--;
    next_index++;
    value = group->radiogrp_item_count();
    if (value <= next_index) {
      next_index = 0;
    }
  }
  if (remaining < 1) {
    return;
  }
  radiogrp_focus_select(group,next_index);
}

/* Part of radiogrp_item_wndproc (005d9b86): left/up: selects the previous enabled item
   (wrapping around), focuses it and notifies. */
static void radiogrp_select_prev(RadioGroup *group,RadioItem *item)
{
  int value;
  int remaining;
  int next_index;

  remaining = group->radiogrp_item_count();
  next_index = item->index - 1;
  if (next_index < 0) {
    next_index = group->radiogrp_item_count();
    next_index--;
  }
  while ((0 < remaining &&
         (value = group->get_items(),
         ((RadioItem *)value)[next_index].enabled == 0))) {
    remaining--;
    next_index--;
    if (next_index < 0) {
      next_index = group->radiogrp_item_count();
      next_index--;
    }
  }
  if (remaining < 1) {
    return;
  }
  radiogrp_focus_select(group,next_index);
}

/* Part of radiogrp_item_wndproc (005d9b86): WM_PAINT: the button bitmap (off, on or
   disabled) and the label with its shadow and hotkey underline. */
static void radiogrp_item_paint(HWND hwnd,ControlBlock *block,RadioGroup *group,RadioItem *item)
{
  iptr selected;
  iptr height;
  uint color_index;
  WndBase *parent_wnd;
  GamePalette *game_palette2;
  size_t len;
  i32 result;
  i8 *red_out;
  i8 *green_out;
  tagRECT *draw_rect;
  i8 *blue_out;
  UINT draw_flags;
  tagRECT rect;
  HBITMAP bitmap;
  tagPAINTSTRUCT ps;
  HGDIOBJ old_font;
  tagSIZE bmp_size;
  HDC hdc;
  i8 green[4];
  int box_left;
  int box_top;
  int box_right;
  int box_bottom;
  i8 red[4];
  i8 blue[4];
  i32 *font_data;

  if (item->enabled == 1) {
    selected = group->get_selected();
    if (selected == item->index) {
      bitmap = block->bitmaps[2];
    }
    else {
      bitmap = block->bitmaps[1];
    }
  }
  else {
    bitmap = block->bitmaps[0];
  }
  GetBitmapDimensionEx(bitmap,&bmp_size);
  rect.left = item->rect.left;
  rect.top = item->rect.top;
  rect.right = item->rect.right;
  rect.bottom = item->rect.bottom;
  box_left = rect.left;
  box_top = rect.top;
  box_right = rect.right;
  box_bottom = rect.bottom;
  if (radio_state.lbutton_captured != 0) {
    box_right = rect.left + 0x20;
    height = rect_height(&rect);
    box_top = height - 0x14 + rect.top >> 1;
    box_bottom = rect.top + 0x14;
  }
  InvalidateRect(hwnd,(RECT *)0x0,1);
  OffsetRect(&rect,-rect.left,-rect.top);
  hdc = BeginPaint(hwnd,&ps);
  ctrl_paint_parent_bkgnd((iptr)&ps,hwnd);
  result = ((GameFont *)(group)->radio_font)->get_handle();
  font_data = (i32 *)mem_handle_lock(result);
  old_font = SelectObject(hdc,(HGDIOBJ)*font_data);
  result = ((GameFont *)(group)->radio_font)->get_handle();
  mem_handle_unlock(result);
  SetBkMode(hdc,1);
  gdi_blit_transparent(hdc,bitmap,rect.left,rect.bottom - 0x14 >> 1,0x20,0x14,0,7);
  rect.left += 0x25;
  if ((ctrl_colors.radio_dx != 0) || (ctrl_colors.radio_dy != 0)) {
    OffsetRect(&rect,(uint)ctrl_colors.radio_dx,(uint)ctrl_colors.radio_dy);
    blue_out = blue;
    green_out = green;
    red_out = red;
    color_index = (uint)ctrl_colors.radio_shadow;
    parent_wnd = (WndBase *)group->get_parent();
    game_palette2 = (GamePalette *)parent_wnd->get_palette();
    game_palette2->get_entry(color_index,(iptr)red_out,(iptr)green_out,(iptr)blue_out);
    SetTextColor(hdc,CONCAT12(blue[0],CONCAT11(green[0],red[0])));
    draw_flags = 0x24;
    draw_rect = &rect;
    len = _strlen(item->label);
    DrawTextA(hdc,item->label,len,draw_rect,draw_flags);
    OffsetRect(&rect,-(uint)ctrl_colors.radio_dx,-(uint)ctrl_colors.radio_dy);
  }
  radiogrp_item_text_color(hdc,group,item);
  draw_flags = 0x24;
  draw_rect = &rect;
  len = _strlen(item->label);
  DrawTextA(hdc,item->label,len,draw_rect,draw_flags);
  radiogrp_item_underline_hotkey(hdc,item,&rect);
  SelectObject(hdc,old_font);
  EndPaint(hwnd,&ps);
}

/* 005d9b86 auto */
i32 __stdcall radiogrp_item_wndproc(HWND hwnd,uint msg,WPARAM wparam,LPARAM lparam)

{
  char nav_enabled;
  int items;
  int value;
  iptr parent;
  HWND hwnd_tmp;
  Wnd *nav_wnd;
  i32 ctrl_id;
  i32 result;
  RECT *null_rect;
  BOOL erase;
  tagRECT rect;
  ControlBlock *block;
  HDC hdc;
  RadioItem *item;
  HBRUSH brush;
  RadioGroup *group;
  
  block = wnd_from_hwnd(hwnd);
  group = (RadioGroup *)block->owner;
  item = (RadioItem *)block->data;
  if (msg < 0x10) {
    if (msg == 0xf) {
      radiogrp_item_paint(hwnd,block,group,item);
      hwnd_tmp = GetFocus();
      if (hwnd_tmp != hwnd) {
        return 0;
      }
    }
    else if (msg != 7) {
      if (msg == 8) {
        wnd_redraw_client(hwnd);
        return 0;
      }
      result = ctrl_default_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    nav_wnd = (Wnd *)group->get_parent();
    nav_enabled = nav_wnd->keyboard_nav_enabled();
    if (nav_enabled == '\0') {
      result = ctrl_default_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
    hdc = GetDC(hwnd);
    brush = GetStockObject(3);
    GetClientRect(hwnd,&rect);
    SelectObject(hdc,brush);
    rect.left += 0x20;
    FrameRect(hdc,&rect,brush);
    DeleteObject(brush);
    ReleaseDC(hwnd,hdc);
  }
  else if (msg < 0x202) {
    if (msg == 0x201) {
      if (radio_state.lbutton_captured == 0) {
        value = group->get_selected();
        if ((value != item->index) && (item->enabled == 1)) {
          value = group->get_selected();
          if (value != -1) {
            erase = 0;
            null_rect = (RECT *)0x0;
            value = group->get_selected();
            items = group->get_items();
            InvalidateRect(((RadioItem *)items)[value].hwnd,null_rect,erase);
          }
          group->set_selected(item->index);
          SetFocus(hwnd);
          InvalidateRect(hwnd,(RECT *)0x0,0);
          parent = group->get_parent();
          wnd_set_active_child(parent);
          result = group->get_selected();
          ctrl_id = group->get_id();
          group->fire_callback(ctrl_id,result);
        }
        radio_state.lbutton_captured = 1;
        SetCapture(hwnd);
      }
    }
    else {
      if (msg != 0x100) {
        result = ctrl_default_wndproc(hwnd,msg,wparam,lparam);
        return result;
      }
      if (wparam == 9) {
        hwnd_tmp = GetParent(hwnd);
        SendMessageA(hwnd_tmp,msg,wparam,lparam);
      }
      else {
        if (wparam == 0x20) {
          nav_wnd = (Wnd *)group->get_parent();
          nav_enabled = nav_wnd->keyboard_nav_enabled();
          if (nav_enabled != '\0') {
            SendMessageA(hwnd,0x201,0,0);
            SendMessageA(hwnd,0x202,0,0);
            return 0;
          }
        }
        if ((((wparam == 0x27) || (wparam == 0x28)) || (wparam == 0x66)) || (wparam == 0x62)) {
          nav_wnd = (Wnd *)group->get_parent();
          nav_enabled = nav_wnd->keyboard_nav_enabled();
          if (nav_enabled != '\0') {
            radiogrp_select_next(group,item);
            return 0;
          }
        }
        if (((wparam == 0x26) || (wparam == 0x25)) || ((wparam == 0x68 || (wparam == 100)))) {
          nav_wnd = (Wnd *)group->get_parent();
          nav_enabled = nav_wnd->keyboard_nav_enabled();
          if (nav_enabled != '\0') {
            radiogrp_select_prev(group,item);
            return 0;
          }
        }
        hwnd_tmp = GetParent(hwnd);
        SendMessageA(hwnd_tmp,msg,wparam,lparam);
      }
    }
  }
  else {
    switch(msg) {
    case 0x202:
      ReleaseCapture();
      radio_state.lbutton_captured = 0;
      break;
    case 0x203:
      if (item->enabled == 1) {
        parent = group->get_parent();
        wnd_set_active_child(parent);
        value = group->get_selected();
        if (value != item->index) {
          value = group->get_selected();
          if (value != -1) {
            erase = 0;
            null_rect = (RECT *)0x0;
            value = group->get_selected();
            items = group->get_items();
            InvalidateRect(((RadioItem *)items)[value].hwnd,null_rect,erase);
          }
          group->set_selected(item->index);
        }
        result = item->index;
        ctrl_id = group->get_id();
        group->fire_dblclick(ctrl_id,result);
      }
      break;
    case 0x204:
      if (radio_state.rbutton_captured == 0) {
        radio_state.rbutton_captured = 1;
        SetCapture(hwnd);
        parent = group->get_parent();
        wnd_set_active_child(parent);
        result = item->index;
        ctrl_id = group->get_id();
        group->fire_rclick(ctrl_id,result);
      }
      break;
    case 0x205:
      ReleaseCapture();
      radio_state.rbutton_captured = 0;
      break;
    default:
      result = ctrl_default_wndproc(hwnd,msg,wparam,lparam);
      return result;
    }
  }
  return 0;
}

/* 005dab5a auto */
i32 __cdecl ctrl_hwnd_is_radio(HWND hwnd)

{
  char *_Str1;
  iptr cmp;
  iptr block;
  const char *_Str2;
  CHAR class_name[52];
  
  GetClassNameA(hwnd,class_name,0x32);
  _Str2 = str_mscontrolclass_6;
  _Str1 = __strupr(class_name);
  cmp = _strcmp(_Str1,_Str2);
  if ((cmp == 0) && (block = (iptr)wnd_from_hwnd(hwnd), *(int *)(block + 0x2c) == 3)) {
    return 1;
  }
  return 0;
}

/* 005dabc7 auto */
void __cdecl radiogrp_hwnd_invalidate(HWND hwnd)

{
  InvalidateRect(hwnd,(RECT *)0x0,0);
  return;
}

/* 005dabe5 auto */
uint __cdecl radiogrp_hwnd_is_selected(i32 hwnd)

{
  ControlBlock *block;
  int selected;
  RadioItem *item;
  
  block = wnd_from_hwnd(hwnd);
  item = (RadioItem *)block->data;
  selected = ((RadioGroup *)block->owner)->get_selected();
  return (uint)(selected == item->index);
}

/* 005dac39 auto */
i32 __cdecl radiogrp_hwnds_same_group(int hwnd_a,int hwnd_b)

{
  iptr is_radio_a;
  iptr is_radio_b;
  ControlBlock *block_a;
  ControlBlock *block_b;
  
  is_radio_a = ctrl_hwnd_is_radio(hwnd_a);
  if ((is_radio_a == 0) || (is_radio_b = ctrl_hwnd_is_radio(hwnd_b), is_radio_b == 0)) {
    return 0;
  }
  if (hwnd_a == hwnd_b) {
    return 1;
  }
  block_a = wnd_from_hwnd(hwnd_a);
  block_b = wnd_from_hwnd(hwnd_b);
  if (block_b->owner == block_a->owner) {
    return 1;
  }
  return 0;
}

/* 005dad40 auto */
void RadioGroup::fire_callback(i32 ctrl_id,i32 index)

{
  if (on_select != 0) {
    on_select(ctrl_id,index);
  }
  return;
}

/* 005dad80 auto */
void RadioGroup::fire_dblclick(i32 ctrl_id,i32 index)

{
  if (on_dblclick != 0) {
    on_dblclick(ctrl_id,index);
  }
  return;
}

/* 005dadc0 auto */
void RadioGroup::fire_rclick(i32 ctrl_id,i32 index)

{
  if (on_rclick != 0) {
    on_rclick(ctrl_id,index);
  }
  return;
}

/* 005db0d0 auto */
void Scrollbar::set_page_size(u32 page_size)

{
  int min_pos;
  int max_pos;
  
  get_ctl_range((iptr)&min_pos,(iptr)&max_pos);
  if ((int)page_size <= max_pos - min_pos) {
    this->page_size = page_size;
  }
  return;
}

/* 005db110 auto */
void Scrollbar::get_ctl_range(iptr min_out,iptr max_out)

{
  scrollbar_hwnd_get_range(hwnd,(LPINT)min_out,(LPINT)max_out);
  return;
}

/* 005ea7d7 auto */
i32 __cdecl ctrl_list_find_by_hwnd(int hwnd,Control *list)

{
  int ctrl_hwnd;
  Control *ctrl;
  
  ctrl = list;
  while( true ) {
    if (ctrl == (Control *)0x0) {
      return 0;
    }
    ctrl_hwnd = ctrl->get_hwnd();
    if (ctrl_hwnd == hwnd) break;
    ctrl = (Control *)ctrl->get_next();
  }
  return (i32)ctrl;
}

/* 005eac6d auto */
i32 __cdecl ctrl_list_find_type11_hwnd(Control *list)

{
  int ctrl_type;
  i32 hwnd;
  Control *ctrl;
  
  ctrl = list;
  while( true ) {
    if (ctrl == (Control *)0x0) {
      return 0;
    }
    ctrl_type = ctrl->get_type();
    if (ctrl_type == 0xb) break;
    ctrl = (Control *)ctrl->get_next();
  }
  hwnd = ctrl->get_hwnd();
  return hwnd;
}

/* 005eacc0: written by hand in src/game/retvals.cpp */

/* 005eb2f0 auto */
void EditControl::fire_maxtext_callback()

{
  if (on_maxtext != 0) {
    on_maxtext(ctrl_id);
  }
  return;
}

/* 005eb330 auto */
void EditControl::fire_change_callback()

{
  if (on_change != 0) {
    on_change(ctrl_id);
  }
  return;
}

/* 005eb3ed auto */
uint __cdecl input_map_vkey(uint vkey)

{
  int key_down;
  uint key_code;
  
  key_code = (uint)input_vkey_map_table[vkey & 0xff];
  if (key_code != 0) {
    key_down = input_ctrl_key_down();
    if (key_down != 0) {
      key_code = key_code | 0x200;
    }
    key_down = input_shift_key_down();
    if (key_down != 0) {
      key_code = key_code | 0x100;
    }
  }
  return key_code;
}

/* 005eb447: written by hand in src/game/fixups.cpp */

/* 005ecef0 auto */
uint __cdecl input_shift_key_down(void)

{
  SHORT key_state;
  
  key_state = GetKeyState(0x10);
  return (uint)(((int)key_state & 0x8000U) != 0);
}

/* 005ecf20 auto */
uint __cdecl input_ctrl_key_down(void)

{
  SHORT key_state;
  
  key_state = GetKeyState(0x11);
  return (uint)(((int)key_state & 0x8000U) != 0);
}

/* 005f0620 auto */
/* Library Function - Single Match
    protected: struct _CRT_CRITICAL_SECTION * __thiscall ios::lockptr(void)
   
   Library: Visual Studio 1998 Debug */

int DDControl::get_rect()

{
  return (int)&rect_left;
}

/* 005f0f05 auto */
void __cdecl ddctrl_set_msg_target(iptr target)

{
  ddctrl_current_target = target;
  return;
}

/* 005f130f auto */
i32 DDControl::dispatch_msg(int msg,int x,int y)

{
  char handled;
  i32 result;
  
  handled = dispatch_to_children(msg,x,y);
  if (handled == '\0') {
    ddctrl_set_msg_target((iptr)this);
    if ((msg == 0x100) && (is_active != '\0')) {
      call_untyped(((code **)vtable)[6], x);
      result = 1;
    }
    else if ((msg == 0x102) && (is_active != '\0')) {
      call_untyped(((code **)vtable)[7], x);
      result = 1;
    }
    else if (msg == 0xfff0) {
      call_untyped(((code **)vtable)[9], x,y);
      result = 1;
    }
    else if ((((x < (int)rect_left) || (y < (int)rect_top)) ||
             ((int)rect_right < x)) || ((int)rect_bottom < y)) {
      result = 0;
    }
    else {
      switch(msg) {
      case 0x200:
        call_untyped(((code **)vtable)[4], x,y);
        break;
      case 0x201:
        call_untyped(((code **)vtable)[0], this,x,y);
        break;
      case 0x202:
        call_untyped(((code **)vtable)[2], x,y);
        break;
      case 0x203:
        call_untyped(((code **)vtable)[5], x,y);
        break;
      case 0x204:
        call_untyped(((code **)vtable)[1], x,y);
        break;
      case 0x205:
        call_untyped(((code **)vtable)[3], x,y);
      }
      result = 1;
    }
  }
  else {
    result = 1;
  }
  return result;
}

/* 005f1514 auto */
uptr DDControl::get_parent_window()

{
  uptr wnd;
  uptr result;
  
  if (parent_wnd == 0) {
    if (parent_control == 0) {
      result = 0;
    }
    else {
      wnd = ((DDControl *)parent_control)->get_parent_window();
      parent_wnd = wnd;
      result = parent_wnd;
    }
  }
  else {
    result = parent_wnd;
  }
  return result;
}

/* 005f15a9 auto */
void __thiscall ddctrl_on_paint_default(DDControl *self,i32 paint_arg)

{
  int parent_wnd;
  
  parent_wnd = self->get_parent_window();
  if (parent_wnd == 0) {
    __assert(str_getparentgamewin,str_d_ss_smeds32_ddcntrl_cpp,0xd8);
  }
  return;
}

/* 005f1622 auto */
void DDControl::activate()

{
  if (is_active == '\0') {
    if (parent_control == 0) {
      ((DDWindow *)(void *)parent_wnd)->deactivate_controls();
    }
    else {
      ((DDControl *)(void *)parent_control)->deactivate_children();
    }
    is_active = 1;
    call_untyped(((code **)vtable)[8], 1);
  }
  return;
}

/* 005f1683 auto */
void DDControl::deactivate()

{
  if ((char)is_active != '\0') {
    deactivate_children();
    is_active = 0;
    call_untyped(((code **)((int *)vtable))[8], this,0);
  }
  return;
}

/* 005f179c auto */
void __thiscall ddctrl_on_lbuttondown(DDControl *self,i32 x,i32 y)

{
  self->activate();
  return;
}

/* 005f17bc auto */
void __thiscall ddctrl_on_rbuttondown_nop(void *self,i32 x,i32 y)

{
  return;
}

/* 005f17d4 auto */
void __thiscall ddctrl_on_lbuttonup(DdctrlOnLbuttonupView *self,i32 x,i32 y)

{
  if (self->on_lbutton_up != 0) {
    self->on_lbutton_up(self->control_id,x,y);
  }
  return;
}

/* 005f1811 auto */
void __thiscall ddctrl_on_rbuttonup(DdctrlOnRbuttonupView *self,i32 x,i32 y)

{
  if (self->on_rbutton_up != 0) {
    self->on_rbutton_up(self->control_id,x,y);
  }
  return;
}

/* 005f184e auto */
void __thiscall ddctrl_on_mousemove(DdctrlOnMousemoveView *self,i32 x,i32 y)

{
  if (self->on_mouse_move != 0) {
    self->on_mouse_move(self->control_id,x,y);
  }
  return;
}

/* 005f188b auto */
void __thiscall ddctrl_on_dblclick(DdctrlOnDblclickView *self,i32 x,i32 y)

{
  if (self->on_dblclick != 0) {
    self->on_dblclick(self->control_id,x,y);
  }
  return;
}

/* 005f18c8 auto */
void __thiscall ddctrl_on_msg_fff0_nop(void *self,i32 x,i32 y)

{
  return;
}

/* 005f18e0 auto */
void __thiscall ddctrl_on_keydown_nop(void *self,i32 key)

{
  return;
}

/* 005f18f8 auto */
void __thiscall ddctrl_on_char_nop(void *self,i32 ch)

{
  return;
}

/* 005f1910 auto */
void __thiscall ddctrl_on_set_active_nop(void *self,i32 active)

{
  return;
}

/* 005f19a6 auto */
void DDControl::deactivate_children()

{
  i32 child;
  
  child = ((PtrList *)child_controls)->first();
  while (child != 0) {
    ((DDControl *)child)->deactivate();
    child = ((PtrList *)child_controls)->next();
  }
  return;
}

/* 005f1a40 auto */
i8 DDControl::dispatch_to_children(int msg,int x,int y)

{
  int *child_rect;
  int handled;
  DDControl *child;
  
  if ((hover_child != 0) && (msg == 0x200)) {
    child_rect = (int *)((DDControl *)hover_child)->get_rect();
    if ((x < *child_rect) ||
       (((child_rect[2] < x || (y < child_rect[1])) || (child_rect[3] < y)))) {
      ((DDControl *)hover_child)->dispatch_msg(0xfff0,x,y);
      hover_child = 0;
    }
  }
  child = (DDControl *)((PtrList *)child_controls)->first();
  while( true ) {
    if (child == (DDControl *)0x0) {
      return 0;
    }
    handled = child->dispatch_msg(msg,x,y);
    if (handled != 0) break;
    child = (DDControl *)((PtrList *)child_controls)->next();
  }
  if (msg == 0x200) {
    hover_child = (uptr)child;
  }
  return 1;
}

/* 00601300: written by hand in src/crt/locale.c */

/* 00601560: written by hand in src/crt/locale.c */

/* 00606330: written by hand in src/crt/locale.c */

/* 00606690: written by hand in src/crt/locale.c */
