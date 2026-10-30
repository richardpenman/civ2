/* The game, windows, dialogs and menus: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"
#include "lang.h"

/* 00408010 auto */
void WndBase::set_cursor(i32 cursor)

{
  wnd_obj_set_cursor(native_wnd,cursor);
  return;
}

/* 00408050 auto */
void WndBase::set_icon(i32 icon)

{
  wnd_obj_set_icon(native_wnd,icon);
  return;
}

/* 00408090 auto */
void WndBase::minimize()

{
  wnd_obj_minimize(native_wnd);
  return;
}

/* 004080c0 auto */
i32 WndBase::client_width()

{
  i32 width;
  
  width = wnd_obj_client_width(native_wnd);
  return width;
}

/* 004080f0 auto */
void WndBase::get_rect_in_parent(iptr rect)

{
  wnd_obj_get_rect_in_parent(native_wnd,(LPRECT)rect);
  return;
}

/* 00408130 auto */
WndMouseFn WndHandlers::set_lbuttonup_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = on_lbutton_up;
  on_lbutton_up = handler;
  return old_handler;
}

/* 00408170 auto */
WndMouseFn WndHandlers::set_rbuttonup_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = on_rbutton_up;
  on_rbutton_up = handler;
  return old_handler;
}

/* 004081b0 auto */
/* Library Function - Single Match
    public: virtual int __thiscall COleControlSite::SetDlgCtrlID(int)
   
   Library: Visual Studio 1998 Debug */

WndKeyFn WndHandlers::set_keydown_handler(WndKeyFn handler)

{
  WndKeyFn old_handler;
  
  old_handler = on_key_down;
  on_key_down = handler;
  return old_handler;
}

/* 004081f0 auto */
/* Library Function - Multiple Matches With Same Base Name
    public: class basic_ostream<char,struct char_traits<char> > * __thiscall basic_ios<char,struct
   char_traits<char> >::tie(class basic_ostream<char,struct char_traits<char> > *)
    public: class basic_ostream<unsigned short,struct char_traits<unsigned short> > * __thiscall
   basic_ios<unsigned short,struct char_traits<unsigned short> >::tie(class basic_ostream<unsigned
   short,struct char_traits<unsigned short> > *)
   
   Library: Visual Studio 1998 Debug */

WndKeyFn WndHandlers::set_char_handler(WndKeyFn handler)

{
  WndKeyFn old_handler;
  
  old_handler = on_char;
  on_char = handler;
  return old_handler;
}

/* 00408230 auto */
WndCloseFn WndHandlers::set_close_handler(WndCloseFn handler)

{
  WndCloseFn old_handler;
  
  old_handler = on_close;
  on_close = handler;
  return old_handler;
}

/* 00408270 auto */
WndNotifyFn WndHandlers::set_minimize_handler(WndNotifyFn handler)

{
  WndNotifyFn old_handler;
  
  old_handler = on_minimize;
  on_minimize = handler;
  return old_handler;
}

/* 004082b0 auto */
WndNotifyFn WndHandlers::set_restore_handler(WndNotifyFn handler)

{
  WndNotifyFn old_handler;
  
  old_handler = on_restore;
  on_restore = handler;
  return old_handler;
}

/* 004082f0 auto */
WndSizeFn WndHandlers::set_size_handler(WndSizeFn handler)

{
  WndSizeFn old_handler;
  
  old_handler = on_size;
  on_size = handler;
  return old_handler;
}

/* 00408330 auto */
WndMouseFn WndHandlers::set_move_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = on_move;
  on_move = handler;
  return old_handler;
}

/* 00408370 auto */
void ModalMenuWindow::set_min_size(u32 width,u32 height)

{
  min_width = width;
  min_height = height;
  return;
}

/* 004083b0 auto */
void DibSurface::wnd_destroy_with_surface()

{
  ((WndBase *)(this + 1))->destroy_native();
  release();
  return;
}

/* 00408420 auto */
void WndBase::destroy_native()

{
  NativeWindow *native;
  
  native = (NativeWindow *)wnd_obj_destroy(native_wnd);
  native_wnd = native;
  return;
}

/* 00408460 auto */
void DibSurface::wnd_blit_surface()

{
  wnd_blit_surface_rect(0);
  return;
}

/* 00408490 auto */
void DibSurface::wnd_blit_surface_rect(iptr rect)

{
  DibSurface *view;
  DibSurface *view2;
  
  if (rect == 0) {
    if (this == (DibSurface *)0x0) {
      view = (DibSurface *)0x0;
    }
    else {
      view = this + 1;
    }
    blit_to_wnd((iptr)view,(RECT *)&bounds_rect,(iptr)&bounds_rect);
    ((WndBase *)(this + 1))->view_validate_rect((iptr)&bounds_rect);
  }
  else {
    if (this == (DibSurface *)0x0) {
      view2 = (DibSurface *)0x0;
    }
    else {
      view2 = this + 1;
    }
    blit_to_wnd((iptr)view2,(RECT *)rect,rect);
    ((WndBase *)(this + 1))->view_validate_rect(rect);
  }
  return;
}

/* 00408580 auto */
void WndBase::view_validate_rect(iptr rect)

{
  wnd_obj_validate_rect(native_wnd,(RECT *)rect);
  return;
}

/* 004085c0 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void SurfaceWindow::set_redraw_callback(WndNotifyFn callback)

{
  on_paint = callback;
  return;
}

/* 004085f0 auto */
void SurfaceWindow::show()

{
  ((Wnd *)((u8 *)this + 0x48))->show_focus_first();
  return;
}

/* 00408620 auto */
void Wnd::show_focus_first()

{
  handle_show_raise();
  focus_first_control();
  return;
}

/* 00408650 auto */
void WndBase::handle_show_raise()

{
  wnd_obj_show_raise(native_wnd);
  return;
}

/* 0040894c auto */
void __cdecl ui_pump_messages_net(void)

{
  win_flush_pending_msgs();
  if (2 < game.multiplayer) {
    net.process_messages(1,0);
  }
  return;
}

/* 0040bc40 auto */
void Dialog::reset(i32 flags)

{
  init(0,0,0,flags);
  return;
}

/* 0040bc80 auto */
i32 Dialog::show_modal(iptr parent)

{
  i32 result;
  
  result = run_modal(0,parent);
  return result;
}

/* 0040e017 auto */
void __cdecl menu_find_city(void)

{
  bool visible;
  iptr adjective;
  iptr objective;
  iptr city2;
  iptr civ2;
  byte civ_byte;
  iptr city;
  int count;
  Dialog dialog2;
  int civ;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  civ2 = human_civ[0];
  count = 0;
  dialog2.load_game_section((const char *)&str_findcity,0x800001);
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      visible = false;
      civ = (int)cities[city].owner;
      if ((game.reveal_map != '\0') ||
         (((game.game_flags & GAME_SCENARIO) != 0 && ((scenario.flags[0] & 8) != 0)))) {
        visible = true;
      }
      if (civ == civ2) {
        visible = true;
      }
      civ_byte = (byte)civ2;
      if (((1 << (civ_byte & 0x1f) & (int)(char)cities[city].known_to) != 0) ||
         ((int)cities[city].owner == (civ2 & 0xffU))) {
        visible = true;
      }
      if (visible) {
        text_clear();
        text_append((const char *)(city * 0x58 + (iptr)((u8 *)&cities + 32)));
        if (civ != civ2) {
          text_append_space();
          text_append_lparen();
          adjective = civ_adjective(civ);
          text_append((const char *)adjective);
          text_append_rparen();
        }
        if ((((game.game_flags & GAME_SCENARIO) != 0) && ((scenario.flags[0] & 2) != 0)) &&
           (objective = city_objective_value(city), objective != 0)) {
          text_append_space();
          text_append_lparen();
          text_append_label(0x1b0);
          if (1 < objective) {
            text_append((const char *)&str_x);
            text_append_int(objective);
          }
          text_append_rparen();
        }
        dialog2.add_option((const char *)&textbuf.buf,city,0);
        count++;
      }
    }
  }
  if (count != 0) {
    city2 = dialog2.show_modal(0);
    if (city2 >= 0) {
      ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)cities[city2].x,(int)cities[city2].y);
      if ((dialog2.result_extra != 0) && ((cities[city2].owner == civ2 || (game.reveal_map != '\0')))
         ) {
        ((CityWindow *)((i8 *)&city_wnd.window))->open(city2);
      }
    }
  }
  return;
}

/* 0040e3b1 auto */
void __cdecl menu_revolution(void)

{
  iptr text;
  int choice;
  iptr civ;
  
  civ = human_civ[0];
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 0x10) == 0)) {
    if ((civs[human_civ[0]].flags & CIVF_GOV_CHANGED) == 0) {
      if (civs[human_civ[0]].government != GOV_ANARCHY) {
        text = civ_get_adjective(human_civ[0]);
        text_set_string_arg(0,(const char *)text);
        text_set_label_arg(1,government_names[civs[civ].government]);
        choice = popup_game_with_sprite((const char *)&str_revolution,(iptr)(((i8 *)&government_sprites_block.government_sprites) +
                                                  (uint)civs[civ].government * 0x3c),
                                   (ui_metrics.large_screen == 0) - 1 & 8);
        if (choice == 0) {
          snd_play_effect(SND_REVOLT,1,0,0);
          text = (iptr)(civ_name_plural(civ));
          text_set_string_arg(0,(const char *)text);
          popup_game_with_sprite((const char *)&str_startrev,(iptr)&popup_info_icon_sprite,0);
          civ_set_government(civ,0);
          menu_update_states();
        }
      }
    }
    else {
      civ_change_government(human_civ[0],1);
    }
  }
  return;
}

/* 0040f320 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void ModalMenuWindow::set_modal_idle_callback(WndNotifyFn callback)

{
  on_modal_idle = callback;
  return;
}

/* 0040f350 auto */
void Wnd::set_keyboard_nav(i32 enabled)

{
  *(i8 *)&keyboard_nav = (i8)enabled;
  return;
}

/* 0040ffa0 auto */
i32 Dialog::load_game_section(const char *section,i32 flags)

{
  i32 result;
  
  result = load_text_ex((const char *)&str_game_7,section,0,flags);
  return result;
}

/* 0040ffe0 auto */
i32 Dialog::load_text_ex(const char *filename,const char *section,i32 arg,i32 flags)

{
  i32 result;
  
  result = load_text_section(filename,section,arg,0,0,0,0,flags);
  return result;
}

/* 00410030 auto */
i32 __cdecl popup_game_with_sprite(const char *section,iptr sprite,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_sprite((iptr)&str_game_7,section,0,sprite,sprite_flags);
  return result;
}

/* 00410e0a auto */
i32 __cdecl ui_paradrop_cursor_active(void)

{
  i32 active;
  
  if ((map_views_block.mapview_cursor_id[0] == 0x202) || (map_views_block.mapview_cursor_id[0] == 0x203)) {
    active = 1;
  }
  else {
    active = 0;
  }
  return active;
}

/* 00414b70 auto */
void WndBase::set_cursor_now(iptr cursor,i32 flag)

{
  wnd_obj_set_cursor_now(native_wnd,cursor,flag);
  return;
}

/* 00414bb0 auto */
i32 WndBase::client_height()

{
  i32 height;
  
  height = wnd_obj_client_height(native_wnd);
  return height;
}

/* 00414be0 auto */
WndMouseFn WndHandlers::set_mousemove_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = on_mouse_move;
  on_mouse_move = handler;
  return old_handler;
}

/* 00414c20 auto */
WndMouseFn WndHandlers::set_lbuttondown_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = on_lbutton_down;
  on_lbutton_down = handler;
  return old_handler;
}

/* 00414c60 auto */
WndMouseFn WndHandlers::set_lbuttonup_any_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = (this)->on_lbutton_up_raw;
  (this)->on_lbutton_up_raw = handler;
  return old_handler;
}

/* 00414ca0 auto */
WndMouseFn WndHandlers::set_dblclick_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = (this)->on_dblclick;
  (this)->on_dblclick = handler;
  return old_handler;
}

/* 00414ce0 auto */
void WndBase::set_capture()

{
  NativeWindow *hwnd;
  
  hwnd = get_window();
  wnd_obj_set_capture(hwnd);
  return;
}

/* 00414d10 auto */
NativeWindow *WndBase::get_window()

{
  return native_wnd;
}

/* 00414d40 auto */
void WndBase::release_capture()

{
  NativeWindow *hwnd;
  
  hwnd = get_window();
  wnd_obj_release_capture(hwnd);
  return;
}

/* 00414da0 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void FrameWindow::set_caption_button_handler(CaptionButtonFn handler)

{
  on_caption_button = handler;
  return;
}

/* 00414dd0 auto */
i32 __cdecl popup_game_with_city(const char *section,iptr city)

{
  i32 result;
  
  result = popup_text_with_city((const char *)&str_game_7,section,0,city);
  return result;
}

/* 004190d0 auto */
i32 __cdecl popup_text(const char *filename,const char *section)

{
  i32 result;
  
  result = popup_text_flags(filename,section,0);
  return result;
}

/* 00419100 auto */
i32 __cdecl popup_text_flags(const char *filename,const char *section,i32 flags)

{
  i32 result;
  
  result = popup_text_ex(filename,section,0,flags);
  return result;
}

/* 00419130 auto */
i32 __cdecl popup_text_ex(const char *filename,const char *section,i32 dlg_flags,i32 flags)

{
  i32 result;
  
  result = popup_text_run((iptr)filename,section,dlg_flags,0,0,0,flags);
  return result;
}

/* 00419b80 auto */
void __cdecl wnd_flush_paint(void)

{
  win_dispatch_all_paints();
  return;
}

/* 00419ba0 auto */
void WndBase::set_bg_color(i32 color)

{
  wnd_obj_set_bg_color(native_wnd,color);
  return;
}

/* 00419be0 auto */
void WndBase::set_bg_surface(DibSurface *surface)

{
  wnd_obj_set_bg_surface(native_wnd,surface);
  return;
}

/* 00421dd0 auto */
void Dialog::init_default()

{
  init(0,0,0,0);
  return;
}

/* 00421e00 auto */
/* Library Function - Single Match
    public: int __thiscall CSocket::Create(unsigned int,int,char const *)
   
   Library: Visual Studio 1998 Debug */

i32 Dialog::load_text(const char *filename,const char *section,i32 flags)

{
  i32 result;
  
  result = load_text_ex(filename,section,0,flags);
  return result;
}

/* 00421e40 auto */
void __cdecl dlg_set_scale(i32 numerator,i32 denominator)

{
  dlg_style.scale_num = numerator;
  dlg_style.scale_den = denominator;
  return;
}

/* 00421e70 auto */
i32 __cdecl popup_game_flags(const char *section,i32 flags)

{
  i32 result;
  
  result = popup_text_flags((const char *)&str_game_7,section,flags);
  return result;
}

/* 00421ea0 auto */
i32 __cdecl popup_game(const char *section)

{
  i32 result;
  
  result = popup_text((const char *)&str_game_7,section);
  return result;
}

/* 00421ed0 auto */
i32 __cdecl dlg_game_text_input(const char *section,i32 max_len,char *buf,iptr default_text)

{
  i32 result;
  
  result = dlg_text_input((iptr)&str_game_7,section,max_len,(iptr)buf,default_text);
  return result;
}

/* 00426f30 auto */
Dialog *Dialog::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 00426f80 auto */
void WndBase::maximize()

{
  wnd_obj_maximize(native_wnd);
  return;
}

/* 00426fb0 auto */
i32 __cdecl popup_game_sprite_flags(const char *section,i32 dlg_flags,iptr sprite,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_sprite((iptr)&str_game_7,section,dlg_flags,sprite,sprite_flags);
  return result;
}

/* 0042a768 auto */
void PictureWindow::close()

{
  if (-1 < (int)report_id) {
    report_id = 0xffffffff;
    window.remove_all_controls();
    surface.release();
    FrameWindow::close();
  }
  return;
}

/* 0042a7bc auto */
void PictureWindow::open(uint resource_id,u32 report,u32 style,u32 img_width,u32 img_height, u32 win_width,u32 win_height)

{
  u32 wnd_h;
  int border;
  int scr_height;
  u32 wnd_w;
  int nonclient_height;
  GamePalette palette;
  i8 *seh_scope;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ui_set_button_font((iptr)&advisor_button_font_block.advisor_button_font);
  controls_created = 0;
  frame_state = 0;
  page_count = 1;
  first_row = 0;
  report_civ = 1;
  report_param = 0;
  report_id = report;
  frame_style = style;
  wnd_w = GetSystemMetrics(2);
  vscroll_width = wnd_w;
  border = ui_metrics.border_width;
  if ((int)resource_id < 0) {
    resource_id = ~resource_id + 1;
  }
  else if (resource_id != 10000) {
    resource_id += 0x31;
  }
  image_width = img_width;
  image_height = img_height;
  win_flush_pending_msgs();
  if (resource_id == 10000) {
    surface.load_gif_file((const char *)&str_scredits_gif,10,0xc0,(iptr)&palette)
    ;
  }
  else {
    surface.load_gif_resource(resource_id,10,0xc0,(iptr)&palette);
  }
  win_flush_pending_msgs();
  if ((int)img_width < 1) {
    wnd_w = surface.surface_width();
    image_width = wnd_w;
  }
  if ((int)img_height < 1) {
    wnd_w = surface.surface_height();
    image_height = wnd_w;
  }
  window_width = win_width;
  window_height = win_height;
  if (window_width == 0) {
    window_width = image_width;
  }
  if ((int)window_width < 0) {
    window_width = image_width - window_width;
  }
  if (window_height == 0) {
    window_height = image_height;
  }
  if ((int)window_height < 0) {
    window_height = image_height - window_height;
  }
  scr_height = caption_font_block.screen_height;
  wnd_w = window_width;
  wnd_h = window_height;
  nonclient_height = main_frame_wnd_block.main_frame_wnd.nonclient_height();
  create(0,(frame_style == 0) - 1 & 8,
             (caption_font_block.screen_width >> 1) - ((int)(wnd_w + border * 2) >> 1),
             (scr_height - nonclient_height >> 1) - ((int)(wnd_h + border * 2) >> 1),image_width,image_height,
             0,border,border);
  return;
}

/* 0042abc1 auto */
void PictureWindow::blit_at(int x,int y,i32 width,i32 height)

{
  gfx_copy_rect_xywh((iptr)&surface_head,(iptr)this,x,y,client_x + x,
                     client_y + y,width,height);
  return;
}

/* 0042ac18 auto */
void PictureWindow::blit()

{
  blit_at(0,0,image_width,image_height);
  return;
}

/* 0043c110 auto */
/* Library Function - Single Match
    public: __thiscall CDaoFieldInfo::~CDaoFieldInfo(void)
   
   Library: Visual Studio 1998 Debug */

void PictureWindow::destruct()

{
  i8 *seh_scope;
  int eh_state;
  
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 5;
  /* inlined unwind funclet 0043c19c */ { ((Scrollbar *)(((int)this) + 0x410))->destruct(); }
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 0043c1ab */ { ctrl_dtor((Button *)(((int)this) + 0x3d4)); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0043c1ba */ { ctrl_dtor((Button *)(((int)this) + 0x398)); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0043c1c9 */ { ctrl_dtor((Button *)(((int)this) + 0x35c)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0043c1d8 */ { ctrl_dtor((Button *)(((int)this) + 800)); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0043c1e7 */ { surface_dtor((DibSurface *)(((int)this) + 0x2d8)); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0043c1f6 */ { FrameWindow::destruct(); }
  return;
}

/* 0043c260 auto */
PictureWindow *PictureWindow::construct()

{
  i8 *seh_scope;
  
  FrameWindow::construct();
  surface_ctor(&surface);
  button_ctor(&close_button);
  button_ctor(&button_2);
  button_ctor(&button_3);
  button_ctor(&button_4);
  scrollbar.construct();
  vtable = (u32)((pointer *)(ptr_fun_rest + 4));
  return this;
}

/* 0043c5c0 auto */
i32 WndBase::nonclient_height()

{
  i32 height;
  
  height = wnd_obj_frame_height(native_wnd);
  return height;
}

/* 0043c630 auto */
void WndBase::disable()

{
  wnd_obj_enable(native_wnd,0);
  return;
}

/* 0043c660 auto */
void WndBase::enable()

{
  wnd_obj_enable(native_wnd,1);
  return;
}

/* 0043c990 auto */
void Dialog::set_list_indent(uptr indent,iptr index)

{
  (this)->list_indents[index] = indent;
  return;
}

/* 0043c9d0 auto */
void Dialog::load_game_text(const char *section)

{
  load_text_noflags((const char *)&str_game_7,section);
  return;
}

/* 0043ca10 auto */
void Dialog::load_text_noflags(const char *filename,const char *section)

{
  load_text(filename,section,0);
  return;
}

/* 00444270 auto */
i32 __cdecl popup_debug_text(const char *section)

{
  i32 result;
  
  result = popup_text((const char *)&str_debug_11,section);
  return result;
}

/* 004442a0 auto */
i32 __cdecl dlg_game_txt_with_unit_flags(const char *section,iptr unit,i32 flags)

{
  i32 result;
  
  result = popup_text_with_unit_sprite((const char *)&str_game_7,section,0,unit,flags);
  return result;
}

/* 004442e0 auto */
i32 __cdecl dlg_game_txt_with_unit(const char *section,iptr unit)

{
  i32 result;
  
  result = popup_text_with_unit((const char *)&str_game_7,section,0,unit);
  return result;
}

/* 00447c3c auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl caption_close_button_sprite_global_init(void)

{
  caption_close_button_sprite_static_init();
  caption_close_button_sprite_reg_atexit();
  return;
}

/* 00447c56 auto */
void __cdecl caption_close_button_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite));
  return;
}

/* 00447c70 auto */
void __cdecl caption_close_button_sprite_reg_atexit(void)

{
  _atexit(caption_close_button_sprite_atexit_dtor);
  return;
}

/* 00447c8d auto */
void __cdecl caption_close_button_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite));
  return;
}

/* 00447ca7 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl caption_zoom_out_sprite_global_init(void)

{
  caption_zoom_out_sprite_static_init();
  caption_zoom_out_sprite_reg_atexit();
  return;
}

/* 00447cc1 auto */
void __cdecl caption_zoom_out_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&caption_zoom_out_sprite));
  return;
}

/* 00447cdb auto */
void __cdecl caption_zoom_out_sprite_reg_atexit(void)

{
  _atexit(caption_zoom_out_sprite_atexit_dtor);
  return;
}

/* 00447cf8 auto */
void __cdecl caption_zoom_out_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&caption_zoom_out_sprite));
  return;
}

/* 00447d12 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl caption_zoom_out_btn_sprite_global_init(void)

{
  caption_zoom_out_btn_sprite_static_init();
  caption_zoom_out_btn_sprite_reg_atexit();
  return;
}

/* 00447d2c auto */
void __cdecl caption_zoom_out_btn_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite));
  return;
}

/* 00447d46 auto */
void __cdecl caption_zoom_out_btn_sprite_reg_atexit(void)

{
  _atexit(caption_zoom_out_btn_sprite_atexit_dtor);
  return;
}

/* 00447d63 auto */
void __cdecl caption_zoom_out_btn_sprite_atexit_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite));
  return;
}

/* 00447d7d auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl caption_view_mode_btn_sprite_global_init(void)

{
  caption_view_mode_btn_sprite_static_init();
  caption_view_mode_btn_sprite_reg_atexit();
  return;
}

/* 00447d97 auto */
void __cdecl caption_view_mode_btn_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&caption_view_mode_button_sprite));
  return;
}

/* 00447db1 auto */
void __cdecl caption_view_mode_btn_sprite_reg_atexit(void)

{
  _atexit(caption_view_mode_button_sprite_static_dtor);
  return;
}

/* 00447dce auto */
void __cdecl caption_view_mode_button_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&caption_view_mode_button_sprite));
  return;
}

/* 00447de8 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl scroll_up_button_sprite_global_init(void)

{
  scroll_up_button_sprite_static_init();
  scroll_up_button_sprite_reg_atexit();
  return;
}

/* 00447e02 auto */
void __cdecl scroll_up_button_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&scroll_up_button_sprite));
  return;
}

/* 00447e1c auto */
void __cdecl scroll_up_button_sprite_reg_atexit(void)

{
  _atexit(scroll_up_button_sprite_static_dtor);
  return;
}

/* 00447e39 auto */
void __cdecl scroll_up_button_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&scroll_up_button_sprite));
  return;
}

/* 00448d7b auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl popup_info_icon_sprite_global_init(void)

{
  popup_info_icon_sprite_static_init();
  popup_info_icon_sprite_reg_atexit();
  return;
}

/* 00448d95 auto */
void __cdecl popup_info_icon_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&popup_info_icon_sprite));
  return;
}

/* 00448daf auto */
void __cdecl popup_info_icon_sprite_reg_atexit(void)

{
  _atexit(popup_info_icon_sprite_static_dtor);
  return;
}

/* 00448dcc auto */
void __cdecl popup_info_icon_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&popup_info_icon_sprite));
  return;
}

/* 00448e51 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl window_border_tile_surface_global_init(void)

{
  window_border_tile_surface_static_init();
  window_border_tile_surface_reg_atexit();
  return;
}

/* 00448e6b auto */
void __cdecl window_border_tile_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&window_border_tile_surface));
  return;
}

/* 00448e85 auto */
void __cdecl window_border_tile_surface_reg_atexit(void)

{
  _atexit(window_border_tile_surface_static_dtor);
  return;
}

/* 00448ea2 auto */
void __cdecl window_border_tile_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&window_border_tile_surface));
  return;
}

/* 00448ebc auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl panel_texture_surface_global_init(void)

{
  panel_texture_surface_static_init();
  panel_texture_surface_reg_atexit();
  return;
}

/* 00448ed6 auto */
void __cdecl panel_texture_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&panel_texture_surface));
  return;
}

/* 00448ef0 auto */
void __cdecl panel_texture_surface_reg_atexit(void)

{
  _atexit(panel_texture_surface_static_dtor);
  return;
}

/* 00448f0d auto */
void __cdecl panel_texture_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&panel_texture_surface));
  return;
}

/* 0044c5a0 auto */
SurfaceWindow *SurfaceWindow::construct()

{
  i8 *seh_scope;
  
  surface_ctor(this);
  window.construct();
  vtable = (u32)((pointer *)(ptr_fun_rest + 8));
  window.control_list = 0;
  on_paint = 0;
  return this;
}

/* 0044c670 auto */
Wnd *Wnd::construct()

{
  i8 *seh_scope;
  
  ModalMenuWindow::construct();
  control_list = 0;
  handler_slot_bc = 0;
  handler_slot_c0 = 0;
  *(i8 *)&keyboard_nav = 1;
  return this;
}

/* 0044c730 auto */
ModalMenuWindow *ModalMenuWindow::construct()

{
  i8 *seh_scope;
  
  init();
  wnd_handlers.construct();
  menu = 0;
  on_menu_pos_command = 0;
  on_command = 0;
  min_width = 0;
  min_height = 0;
  max_width = 0x4000;
  max_height = 0x4000;
  handler_slot_a8 = 0;
  handler_slot_ac = 0;
  modal_running = 0;
  hscroll_step = 1;
  vscroll_step = 1;
  on_menu_select = 0;
  on_modal_idle = 0;
  return this;
}

/* 0044c8b0 auto */
/* Library Function - Single Match
    public: __thiscall CString::CString(void)
   
   Library: Visual Studio 1998 Debug */

WndHandlers *WndHandlers::construct()

{
  clear_handlers();
  return this;
}

/* 0044c8e0 auto */
void WndHandlers::clear_handlers()

{
  on_mouse_move = 0;
  on_lbutton_down = 0;
  on_lbutton_up_raw = 0;
  on_lbutton_up = 0;
  on_rbutton_down = 0;
  on_rbutton_up_raw = 0;
  on_rbutton_up = 0;
  on_dblclick = 0;
  on_key_down_pre = 0;
  on_key_up = 0;
  on_key_down = 0;
  on_char = 0;
  on_close = 0;
  on_minimize = 0;
  on_restore = 0;
  on_set_focus = 0;
  on_size = 0;
  on_move = 0;
  on_msg_44 = 0;
  on_hscroll = 0;
  on_hscroll_track = 0;
  on_vscroll = 0;
  on_vscroll_track = 0;
  on_msg_4c8 = 0;
  on_enter_size_move = 0;
  on_exitsizemove = 0;
  return;
}

/* 0044ca40 auto */
void WndHandlers::destruct()

{
  return;
}

/* 0044ca60: written by hand in src/game/unwind.cpp */

/* 0044cb20 auto */
i32 Wnd::destruct()

{
  i32 seh_handler;
  i8 *seh_scope;
  i32 eh_state;
  
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0044cb5e */ { ModalMenuWindow::destruct(); }
  return seh_handler;
}

/* 0044cba0: written by hand in src/game/unwind.cpp */

/* 0044cc80: written by hand in src/game/throneroom.cpp */

/* 0044cd9b: written by hand in src/game/throneroom.cpp */

/* 00450390 auto */
void WndBase::set_palette(iptr palette)

{
  wnd_palette = palette;
  apply_palette_handle(palette);
  return;
}

/* 004503d0 auto */
void WndBase::hide()

{
  wnd_obj_hide(native_wnd);
  return;
}

/* 00450400 auto */
void WndBase::center()

{
  wnd_obj_center(native_wnd,0,0);
  return;
}

/* 00450440 auto */
void WndBase::apply_palette_handle(iptr palette)

{
  wnd_obj_select_palette(native_wnd,*(i32 *)(palette + 0x404));
  return;
}

/* 00451890 auto */
void WndBase::get_cursor_pos(iptr out_x,iptr out_y)

{
  NativeWindow *hwnd;
  
  hwnd = get_window();
  wnd_obj_get_cursor_pos(hwnd,(LONG *)out_x,(LONG *)out_y);
  return;
}

/* 004518d0 auto */
void Wnd::activate()

{
  focus_and_raise();
  focus_first_control();
  return;
}

/* 00451900 auto */
void WndBase::focus_and_raise()

{
  wnd_obj_focus_and_raise(native_wnd);
  return;
}

/* 00451930 auto */
Hotspot * __thiscall hotspot_ctor(Hotspot *self)

{
  i8 *seh_scope;
  
  self->base_construct();
  return self;
}

/* 004519b0 auto */
void Hotspot::create(iptr parent,iptr id,iptr rect)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  init(parent,1,id,(RECT *)rect);
  on_enter = 0;
  on_leave = 0;
  on_click = 0;
  on_press = 0;
  on_dblclick = 0;
  new_hwnd = hotspot_create_hwnd((int *)rect,(iptr)this);
  hwnd = new_hwnd;
  return;
}

/* 00451a60 auto */
void Hotspot::set_click_callback(CtrlNotifyFn callback)

{
  on_click = callback;
  return;
}

/* 00451a90 auto */
/* Library Function - Single Match
    public: void __thiscall CDialog::SetHelpID(unsigned int)
   
   Library: Visual Studio 1998 Debug */

void Hotspot::set_dblclick_callback(CtrlNotifyFn callback)

{
  on_dblclick = callback;
  return;
}

/* 00453aa0 auto */
Hotspot *Hotspot::scalar_destruct(uint flags)

{
  hotspot_dtor(this);
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 00453af0 auto */
void __cdecl ui_pump_messages(void)

{
  win_dispatch_all_messages();
  return;
}

/* 00453ba0 auto */
void __thiscall hotspot_dtor(Hotspot *self)

{
  i8 *seh_scope;
  i32 eh_state;
  
  eh_state = 0;
  if (self->hwnd != 0) {
    region_dispose(self->hwnd);
  }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 00453bfa */ { self->base_destruct(); }
  return;
}

/* 00456ee0 auto */
/* Library Function - Single Match
    public: class ostream * __thiscall ios::tie(class ostream *)
   
   Library: Visual Studio 1998 Debug */

WndKeyFn WndHandlers::set_keydown_pre_handler(WndKeyFn handler)

{
  WndKeyFn old_handler;
  
  old_handler = on_key_down_pre;
  on_key_down_pre = handler;
  return old_handler;
}

/* 0046768b auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl menu_handle_654fd4_global_init(void)

{
  menu_handle_654fd4_static_init();
  menu_handle_654fd4_reg_atexit();
  return;
}

/* 004676a5 auto */
void __cdecl menu_handle_654fd4_static_init(void)

{
  ((GameMenu *)((i8 *)&civ_saved_cursor_x_block.secondary_menu_handle))->handle_construct();
  return;
}

/* 004676bf auto */
void __cdecl menu_handle_654fd4_reg_atexit(void)

{
  _atexit(menu_handle_654fd4_atexit_dtor);
  return;
}

/* 004676dc auto */
void __cdecl menu_handle_654fd4_atexit_dtor(void)

{
  ((GameMenu *)((i8 *)&civ_saved_cursor_x_block.secondary_menu_handle))->handle_destruct();
  return;
}

/* 0046e287 auto */
void __cdecl ui_delay_ticks(int ticks)

{
  DWORD start_tick;
  DWORD now_tick;
  
  start_tick = timeGetTime();
  do {
    win_flush_pending_msgs();
    now_tick = timeGetTime();
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
  } while ((int)(now_tick - start_tick) < (ticks * 0x32) / 3);
  return;
}

/* 0046f18f auto */
void __cdecl ui_blit_all_windows(void)

{
  int view;
  
  for (view = 0; view < 8; view++) {
    if ((view == 0) || (map_views_block.map_view_recs[view].open != 0)) {
      map_views_block.map_view_recs[view].wnd_blit_surface();
    }
  }
  ((DibSurface *)((i8 *)&minimap_state.window))->wnd_blit_surface();
  ((DibSurface *)((i8 *)&status_wnd.window))->wnd_blit_surface();
  ((DibSurface *)((i8 *)&city_wnd.window))->wnd_blit_surface();
  wnd_flush_paint();
  return;
}

/* 004731d2 auto */
uint __cdecl
ui_file_name_dialog(iptr parent,const char *title,iptr file_buf,iptr filter,iptr default_ext,int must_exist,
            int is_save)

{
  NativeWindow *wnd;
  BOOL dlg_ok;
  char success;
  OPENFILENAMEA ofn;
  
  _memset(&ofn,0,0x4c);
  ofn.lStructSize = 0x4c;
  ofn.hwndOwner = (HWND)0x0;
  if (parent != 0) {
    wnd = ((WndBase *)parent)->get_window();
    ofn.hwndOwner = wnd->hwnd;
  }
  ofn.lpstrFilter = (LPCSTR)filter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile = (LPSTR)file_buf;
  ofn.nMaxFile = 0x100;
  ofn.lpstrTitle = title;
  ofn.lpstrDefExt = (LPCSTR)default_ext;
  if (must_exist == 0) {
    ofn.Flags = 0x80e;
  }
  else {
    ofn.Flags = 0x180e;
  }
  if (is_save == 0) {
    dlg_ok = GetOpenFileNameA(&ofn);
    success = (char)dlg_ok;
  }
  else {
    dlg_ok = GetSaveFileNameA(&ofn);
    success = (char)dlg_ok;
  }
  if (success != 1) {
    CommDlgExtendedError();
  }
  return (uint)(success == 1);
}

/* 00484d52 auto */
void __cdecl ui_flush_input(void)

{
  if (net.mp_simultaneous_moves == '\0') {
    input_flush_events();
  }
  win_flush_pending_msgs();
  wnd_flush_paint();
  win_flush_pending_msgs();
  return;
}

/* 004897fa auto */
void __cdecl ui_redraw_all_maps(int force)

{
  if ((moving_units_mode != 0) || (force != 0)) {
    moving_units_mode = 0;
    blink.unit_phase = 1;
    blink.unit_skip_tick = 1;
    blink.unit_active = 0;
    status_redraw(1);
    mapviews_redraw_cursor();
    menu_update_states();
  }
  return;
}

/* 00489a0d auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl ui_focus_active_unit(int auto_next)

{
  iptr selectable;
  iptr unit;
  int unit_ok;
  bool start_blink;
  
  start_blink = moving_units_mode != 1;
  if (start_blink) {
    blink.unit_phase = 0;
    blink.unit_skip_tick = 1;
    mapviews_redraw_cursor();
    moving_units_mode = 1;
    blink.unit_active = 0;
  }
  selectable = unit_is_selectable((int)game.active_unit);
  if (selectable == 0) {
    unit_activate_next(auto_next);
  }
  start_blink = selectable != 0 && start_blink;
  unit = (iptr)game.active_unit;
  if ((((-1 < unit) && (unit < game.num_units)) && (units[unit].id != 0)) &&
     ((unit_ok = unit_is_selectable(unit), unit_ok != 0 &&
      (((game.multiplayer < 3 || (net.mp_simultaneous_moves != '\0')) ||
       ((auto_next == 0 || (net.mp_turn_civ == human_civ[0])))))))) {
    if (start_blink) {
      status_redraw(1);
    }
    map_cursor.x[0] = units[unit].x;
    map_cursor.y[0] = units[unit].y;
    if (start_blink) {
      mapviews_ensure_visible((int)map_cursor.x[0],(int)map_cursor.y[0],(int)units[unit].owner);
    }
    menu_update_states();
    map_views_block.unit_focus_done = 1;
    return;
  }
  blink.unit_active = 0;
  ui_redraw_all_maps(0);
  return;
}

/* 004904c0 auto */
void __cdecl popup_file_with_sprite(const char *filename,const char *section,iptr sprite,i32 sprite_flags)

{
  popup_text_with_sprite((iptr)filename,section,0,sprite,sprite_flags);
  return;
}

/* 00490500 auto */
void __cdecl popup_text_with_city_noflags(const char *filename,const char *section,i32 city)

{
  popup_text_with_city(filename,section,0,city);
  return;
}

/* 00490530 auto */
void __cdecl popup_text_with_unit_noflags(const char *filename,const char *section,iptr unit)

{
  popup_text_with_unit(filename,section,0,unit);
  return;
}

/* 004923c0 auto */
void WndBase::set_focus()

{
  wnd_obj_set_focus(native_wnd);
  return;
}

/* 00497cc0 auto */
void DiploLeaderWindow::wnd_set_bg_tile(DibSurface *tile)

{
  wnd_obj_set_bg_tile(native_wnd,tile);
  return;
}

/* 00497d00 auto */
void WndBase::set_caption_height(i32 height)

{
  wnd_obj_set_caption_height(native_wnd,height);
  return;
}

/* 004a6b80 auto */
void __cdecl
popup_text_with_advance(const char *filename,const char *section,i32 dlg_flags,iptr advance,i32 sprite_flags)

{
  popup_text_with_sprite((iptr)filename,section,dlg_flags,
                     (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                           (char)advances[advance].category * 0xf0 +
                           (char)advances[advance].epoch * 0x3c),sprite_flags);
  return;
}

/* 004a6bdc auto */
i32 __cdecl
popup_text_with_improvement(const char *filename,const char *section,i32 dlg_flags,int improvement,i32 sprite_flags)

{
  i32 result;
  i8 *sprite;
  
  if (improvement < 0x27) {
    sprite = ((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c;
  }
  else {
    sprite = ((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (improvement * 4 - 0x9c) * 0xf;
  }
  result = popup_text_with_sprite((iptr)filename,section,dlg_flags,(iptr)sprite,sprite_flags);
  return result;
}

/* 004a6c4b auto */
i32 __cdecl
popup_text_with_unit_sprite(const char *filename,const char *section,i32 dlg_flags,int unit_type,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_sprite((iptr)filename,section,dlg_flags,(iptr)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c),sprite_flags)
  ;
  return result;
}

/* 004a6c85 auto */
void __cdecl
popup_city_sprite_draw_cb(i32 dlg,iptr sprite,iptr city,i32 left,int x,
            i32 y)

{
  gfx_draw_city(sprite,city,0,x + 2,y,(ui_metrics.large_screen == 0) - 1 & 8);
  return;
}

/* 004a6cc5 auto */
i32 __cdecl popup_text_with_city(const char *filename,const char *section,i32 dlg_flags,iptr city)

{
  i32 result;
  Dialog dialog2;
  Sprite sprite;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  sprite.create_solid((2 - (uint)(ui_metrics.large_screen == 0)) * 0x40,(2 - (uint)(ui_metrics.large_screen == 0)) * 0x30
             ,0);
  dialog2.load_text(filename,section,dlg_flags);
  dialog2.set_sprite_draw_fn((DialogSpriteDrawFn)popup_city_sprite_draw_cb);
  dialog2.set_image_zoom(0);
  dialog2.add_sprite((iptr)&sprite,city,0);
  result = dialog2.show_modal(0);
  (*(u32 *)&dlg_input.help_pressed) = dialog2.result_extra;
  return result;
}

/* 004a6df7 auto */
void __cdecl
popup_unit_sprite_draw_cb(i32 dlg,iptr sprite,iptr unit,i32 left,int x,
            i32 y)

{
  gfx_draw_unit(sprite,unit,4,x + 2,y,(ui_metrics.large_screen == 0) - 1 & 8,0);
  return;
}

/* 004a6e39 auto */
i32 __cdecl popup_text_with_unit(const char *filename,const char *section,i32 dlg_flags,iptr unit)

{
  i32 result;
  Dialog dialog2;
  Sprite sprite;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  sprite.create_solid((2 - (uint)(ui_metrics.large_screen == 0)) * 0x40,(2 - (uint)(ui_metrics.large_screen == 0)) * 0x30
             ,0);
  dialog2.load_text(filename,section,dlg_flags);
  dialog2.set_sprite_draw_fn((DialogSpriteDrawFn)popup_unit_sprite_draw_cb);
  dialog2.set_image_zoom(0);
  dialog2.add_sprite((iptr)&sprite,unit,0);
  result = dialog2.show_modal(0);
  (*(u32 *)&dlg_input.help_pressed) = dialog2.result_extra;
  return result;
}

/* 004a93b3 auto */
void __cdecl accel_start_develop_city(iptr city,int num_tiles)

{
  byte civ;
  i8 terrain;
  char best_terrain;
  int x2;
  int y2;
  iptr x;
  int y;
  int result;
  int landmass;
  iptr best_tile;
  int i;
  iptr tile;
  uint best_score;
  uint score;
  
  x2 = (int)cities[city].x;
  y2 = (int)cities[city].y;
  net_batch_begin();
  for (i = 0; i < num_tiles; i++) {
    best_score = 0;
    best_tile = -1;
    for (tile = 0; tile < 0x14; tile++) {
      x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile] + x2);
      y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile] + y2;
      result = map_in_bounds(x,y);
      if ((result != 0) && (result = tile_improvements(x,y), result == 0)) {
        score = 0;
        terrain = tile_terrain(x,y);
        switch(terrain) {
        case TERRAIN_DESERT:
          score = 1;
          break;
        case TERRAIN_PLAINS:
          score = 4;
          break;
        case TERRAIN_GRASSLAND:
          result = tile_grassland_has_shield(x,y);
          if (result == 0) {
            score = 3;
          }
          else {
            score = 5;
          }
          break;
        case TERRAIN_HILLS:
          score = 2;
        }
        if (score != 0) {
          if (tile < 8) {
            score++;
          }
          y = tile_resource(x,y);
          if (y != 0) {
            score += 3;
          }
          if (best_score < score) {
            best_score = score;
            best_tile = tile;
          }
        }
      }
    }
    if (best_tile < 0) break;
    x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[best_tile] + x2);
    y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[best_tile] + y2;
    best_terrain = tile_terrain(x,y);
    tile_at(x,y);
    tile_set_improvements(x,y,0x10,1,1);
    if (best_terrain == TERRAIN_HILLS) {
      tile_set_improvements(x,y,8,1,1);
    }
    else {
      tile_set_improvements(x,y,4,1,1);
    }
    tile_update_known(x,y,(int)cities[city].owner);
  }
  civ = cities[city].owner;
  for (tile = 0; tile < 0x2d; tile++) {
    x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile] + x2);
    y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile] + y2;
    result = map_in_bounds(x,y);
    if ((result != 0) && (result = tile_is_ocean(x,y), result == 0)) {
      result = tile_landmass((int)cities[city].x,(int)cities[city].y);
      landmass = tile_landmass(x,y);
      if (result == landmass) {
        tile_set_visible(x,y,1 << (civ & 0x1f) & 0xff,1,1);
      }
    }
  }
  net_batch_flush();
  return;
}

/* 004bb570 auto */
void WndBase::move_to_rect(iptr rect)

{
  wnd_obj_move_to_rect(native_wnd,(int *)rect);
  return;
}

/* 004c4240 auto */
void __cdecl popup_game_with_advance(const char *section,iptr advance,i32 sprite_flags)

{
  popup_text_with_advance((const char *)&str_game_7,section,0,advance,sprite_flags);
  return;
}

/* 004cc870 auto */
i32 __cdecl popup_game_with_improvement(const char *section,iptr improvement,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_improvement((const char *)&str_game_7,section,0,improvement,sprite_flags);
  return result;
}

/* 004cc8b0 auto */
i32 __cdecl popup_game_with_unit(const char *section,i32 dlg_flags,iptr unit)

{
  i32 result;
  
  result = popup_text_with_unit((const char *)&str_game_7,section,dlg_flags,unit);
  return result;
}

/* 004cff70 auto */
void WndBase::set_resize_border(i32 width)

{
  wnd_obj_set_resize_border(native_wnd,width);
  return;
}

/* 004e01b0 auto */
void __cdecl menu_wonder_movies(void)

{
  iptr wonder_name;
  i32 flags;
  iptr wonder_id;
  Dialog dialog2;
  iptr wonder;
  iptr last_choice;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  last_choice = 0;
  while( true ) {
    dialog2.Dialog::reset(0x1001);
    dialog2.set_width(0x1b8);
    dialog2.set_listbox(0x10,0x1b8,0);
    for (wonder = 0; wonder < 0x1c; wonder++) {
      flags = 0;
      wonder_id = wonder;
      wonder_name = (iptr)label_string(improvements[wonder + 0x27].name);
      dialog2.add_option((const char *)wonder_name,wonder_id,flags);
    }
    dialog2.set_default_choice(last_choice);
    wonder = dialog2.show_modal(0);
    last_choice = wonder;
    if (wonder < 0) break;
    wonder_show_movie(wonder);
  }
  return;
}

/* 004e02ef auto */
void __cdecl ui_reset_window_layout(void)

{
  int left;
  int client_height;
  int chat_font_height;
  int parley_font_height;
  int height2;
  int width2;
  int view;
  
  main_frame_wnd_block.main_frame_wnd.maximize();
  city_window_init_size();
  ((WndBase *)((i8 *)&city_wnd.wnd))->show_restored();
  ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.city_wnd_rect);
  city_window_close();
  minimap_calc_rect();
  ((WndBase *)((i8 *)&minimap_state.wnd))->show_restored();
  minimap_hidden = 0;
  ((WndBase *)((i8 *)&minimap_state.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.minimap_wnd_rect);
  ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
  status_calc_wnd_rect();
  ((WndBase *)((i8 *)&status_wnd.wnd))->show_restored();
  status_bar.wnd_hidden = 0;
  ((WndBase *)((i8 *)&status_wnd.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.status_wnd_rect);
  ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
  if (parley_wnd_block.large_layout == 0) {
    width2 = 0x208;
    height2 = 0x14f;
    parley_font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    width2 = 800;
    height2 = 0x1cc;
    parley_font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  parley_wnd_block.caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + parley_font_height;
  width2 = width2 + (panel_layout.padding_x + panel_layout.margin_x) * 2 + 6;
  height2 = height2 + panel_layout.margin_y * 2 + (int)parley_wnd_block.caption_height * 2 + 0xe;
  left = main_frame_wnd_block.main_frame_wnd.client_width();
  left = (left >> 1) - (width2 >> 1);
  parley_wnd_block.wnd_x = left;
  client_height = main_frame_wnd_block.main_frame_wnd.client_height();
  parley_wnd_block.wnd_y = (client_height >> 1) - (height2 >> 1);
  SetRect((LPRECT)parley_wnd_block.init_rect,left,(int)parley_wnd_block.wnd_y,left + width2,(int)parley_wnd_block.wnd_y + height2);
  ((WndBase *)((i8 *)&parley_wnd_block.window))->move_to_rect((iptr)(LPRECT)parley_wnd_block.init_rect);
  if (chat_wnd_block.large_layout == 0) {
    width2 = 0x208;
    height2 = 0x14f;
    chat_font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    width2 = 800;
    height2 = 0x1cc;
    chat_font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  chat_wnd_block.caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + chat_font_height;
  width2 = width2 + (panel_layout.padding_x + panel_layout.margin_x) * 2 + 6;
  height2 = height2 + panel_layout.margin_y * 2 + (int)chat_wnd_block.caption_height * 2 + 0xe;
  left = main_frame_wnd_block.main_frame_wnd.client_width();
  left = (left >> 1) - (width2 >> 1);
  chat_wnd_block.wnd_x = left;
  client_height = main_frame_wnd_block.main_frame_wnd.client_height();
  chat_wnd_block.wnd_y = (client_height >> 1) - (height2 >> 1);
  SetRect((LPRECT)chat_wnd_block.init_rect,left,(int)chat_wnd_block.wnd_y,left + width2,(int)chat_wnd_block.wnd_y + height2);
  ((WndBase *)((i8 *)&chat_wnd_block.window))->move_to_rect((iptr)(LPRECT)chat_wnd_block.init_rect);
  for (view = 1; view < 8; view++) {
    if (map_views_block.map_view_recs[view].open != 0) {
      map_views_block.map_view_recs[view].open = 0;
      map_views_block.map_view_recs[view].wnd_destroy_with_surface();
    }
  }
  mapview_calc_rect();
  ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->show_restored();
  ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->move_to_rect((iptr)&map_view_saved_rects_block.map_view_saved_rects);
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  ui_flush_input();
  return;
}

/* 004e068d auto */
void __cdecl menu_load_game(void)

{
  byte music_was_on;
  int result;
  int view;
  
  mp_turn_timer_stop(1);
  city_window_close();
  parley_wnd_close();
  chat_wnd_close();
  pedia_close();
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  ((PictureWindow *)((i8 *)&casualty.window))->close();
  music_was_on = (byte)game.game_options & OPT_MUSIC;
  result = game_load_dialog(0,0,0);
  if (result == 0) {
    if (game.multiplayer != '\0') {
      load_game_pick_civ(1);
    }
    (*(byte *)&saved_turn.civ) = (byte)(*(i8 *)&human_civ);
    (*(byte *)&turn_loop.resume_civ) = (byte)(*(i8 *)&human_civ);
    game.multiplayer = '\0';
    game.human_players = (u8)(1 << ((byte)(*(i8 *)&human_civ) & 0x1f));
    if (((byte)game.game_options & OPT_MUSIC) == 0) {
      snd_cd_stop();
    }
    else {
      if (music_was_on == 0) {
        snd_cd_mci_stop();
        snd_cd_close();
        snd_cd_open();
        snd_cd_detect_layout();
      }
      snd_sync_music_option();
    }
    civ_check_password((*(i8 *)&human_civ));
    if (destroyed_civ_count_block.saved_layout_valid == 0) {
      ui_reset_window_layout();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->show_restored();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->move_to_rect((iptr)&map_view_saved_rects_block.map_view_saved_rects);
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
      for (view = 1; view < 8; view++) {
        if (map_views_block.map_view_recs[view].open != 0) {
          map_views_block.map_view_recs[view].create();
          ((WndBase *)&map_views_block.map_view_recs[view].window)->move_to_rect(
                     (iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + view * 0x10));
          map_views_block.map_view_recs[view].update_title();
          ((SurfaceWindow *)&map_views_block.map_view_recs[view])->repaint();
        }
      }
    }
    else {
      main_frame_wnd_block.main_frame_wnd.maximize();
      ((WndBase *)((i8 *)&city_wnd.wnd))->show_restored();
      ((WndBase *)((i8 *)&city_wnd.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.city_wnd_rect);
      city_window_close();
      ((WndBase *)((i8 *)&minimap_state.wnd))->show_restored();
      minimap_hidden = 0;
      ((WndBase *)((i8 *)&minimap_state.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.minimap_wnd_rect);
      ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
      ((WndBase *)((i8 *)&status_wnd.wnd))->show_restored();
      status_bar.wnd_hidden = 0;
      ((WndBase *)((i8 *)&status_wnd.wnd))->move_to_rect((iptr)&minimap_wnd_rect_block.status_wnd_rect);
      ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->show_restored();
      ((WndBase *)((i8 *)&map_views_block.map_view_wnds))->move_to_rect((iptr)&map_view_saved_rects_block.map_view_saved_rects);
      ((MapView *)((i8 *)&map_views_block.map_views))->update_title();
      ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
      for (view = 1; view < 8; view++) {
        if (map_views_block.map_view_recs[view].open != 0) {
          map_views_block.map_view_recs[view].create();
          ((WndBase *)&map_views_block.map_view_recs[view].window)->move_to_rect(
                     (iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + view * 0x10));
          map_views_block.map_view_recs[view].update_title();
          ((SurfaceWindow *)&map_views_block.map_view_recs[view])->repaint();
        }
      }
      ((SurfaceWindow *)((i8 *)&minimap_state.window))->repaint();
    }
    moving_units_mode = 0;
    ((SurfaceWindow *)((i8 *)&status_wnd.window))->repaint();
    ui_redraw_all_maps(1);
    ui_focus_active_unit(0);
    menu_update_states();
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->load_indexes();
    ui_flush_input();
    if ((game.multiplayer != '\0') && (civ_saved_cursor_x_block.mp_turn_time_limit_ms != 0)) {
      mp_turn_timer_start(1);
    }
  }
  else {
    game.multiplayer = '\0';
    mp_turn_timer_resume(1);
  }
  return;
}

/* 004e0a8c auto */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __cdecl dlg_check_set_bool(i32 index,int value)

{
  dlg_check_set(index,(uint)(value != 0));
  return;
}

/* 004e0ab0 auto */
void __cdecl menu_game_options(void)

{
  uint music_was_on;
  int result;
  
  music_was_on = game.game_options & OPT_MUSIC;
  dlg_checks_clear();
  dlg_check_set_bool(0,game.game_options & OPT_SOUND);
  dlg_check_set_bool(1,game.game_options & OPT_MUSIC);
  dlg_check_set_bool(2,game.game_options & OPT_ALWAYS_WAIT);
  dlg_check_set_bool(3,game.game_options & OPT_AUTOSAVE);
  dlg_check_set_bool(4,game.game_options & OPT_SHOW_ENEMY_MOVES);
  dlg_check_set_bool(5,game.game_options & OPT_NO_PAUSE_ENEMY);
  dlg_check_set_bool(6,game.game_options & OPT_FAST_PIECE_SLIDE);
  dlg_check_set_bool(7,game.game_options & OPT_INSTANT_ADVICE);
  dlg_check_set_bool(8,game.game_options & OPT_TUTORIAL);
  dlg_check_set_bool(9,game.game_options & OPT_MOVE_WITH_MOUSE);
  dlg_check_set_bool(10,game.game_options & OPT_ENTER_CLOSES_CITY);
  text_set_string_arg(0,(const char *)str_ptr_5_4_0f_multiplayer_26_march_99);
  crt_strcat(((i8 *)&text_args.strings),((i8 *)&game_options_version_sep_str));
  crt_strcat(((i8 *)&text_args.strings),(*(i8 * *)str_ptr_5_4_0f_multiplayer_26_march_99_rest));
  result = popup_game_flags((const char *)&str_gameoptions,1);
  if (result == 0) {
    game.game_options = game.game_options & 0xffff8027;
    result = dlg_check_get(0);
    if (result != 0) {
      game.game_options = game.game_options | OPT_SOUND;
    }
    result = dlg_check_get(1);
    if (result == 0) {
      snd_cd_stop();
    }
    else {
      game.game_options = game.game_options | OPT_MUSIC;
      if (music_was_on == 0) {
        snd_cd_mci_stop();
        snd_cd_close();
        snd_cd_open();
        snd_cd_detect_layout();
      }
      snd_sync_music_option();
    }
    result = dlg_check_get(2);
    if (result != 0) {
      game.game_options = game.game_options | OPT_ALWAYS_WAIT;
    }
    result = dlg_check_get(3);
    if (result != 0) {
      game.game_options = game.game_options | OPT_AUTOSAVE;
    }
    result = dlg_check_get(4);
    if (result != 0) {
      game.game_options = game.game_options | OPT_SHOW_ENEMY_MOVES;
    }
    result = dlg_check_get(5);
    if (result != 0) {
      game.game_options = game.game_options | OPT_NO_PAUSE_ENEMY;
    }
    result = dlg_check_get(6);
    if (result != 0) {
      game.game_options = game.game_options | OPT_FAST_PIECE_SLIDE;
    }
    result = dlg_check_get(7);
    if (result != 0) {
      game.game_options = game.game_options | OPT_INSTANT_ADVICE;
    }
    result = dlg_check_get(8);
    if (result != 0) {
      game.game_options = game.game_options | OPT_TUTORIAL;
    }
    result = dlg_check_get(9);
    if (result != 0) {
      game.game_options = game.game_options | OPT_MOVE_WITH_MOUSE;
    }
    result = dlg_check_get(10);
    if (result != 0) {
      game.game_options = game.game_options | OPT_ENTER_CLOSES_CITY;
    }
    prefs.game_options = game.game_options;
    prefs_save_civ2dat();
  }
  return;
}

/* 004e0d71 auto */
void __cdecl menu_graphic_options(void)

{
  uint hires_was_on;
  int result;
  
  hires_was_on = game.game_options & OPT_ANIMATED_HERALDS;
  dlg_checks_clear();
  dlg_check_set_bool(0,game.game_options & OPT_THRONE_ROOM);
  dlg_check_set_bool(1,game.game_options & OPT_DIPLOMACY_SCREEN);
  dlg_check_set_bool(2,game.game_options & OPT_ANIMATED_HERALDS);
  dlg_check_set_bool(3,game.game_options & OPT_PEDIA_ADVANCES);
  dlg_check_set_bool(4,game.game_options & OPT_HIGH_COUNCIL);
  dlg_check_set_bool(5,game.game_options & OPT_WONDER_MOVIES);
  result = popup_game_flags((const char *)&str_graphicoptions,1);
  if (result == 0) {
    game.game_options = game.game_options & 0xffc0ffff;
    result = dlg_check_get(0);
    if (result != 0) {
      game.game_options = game.game_options | OPT_THRONE_ROOM;
    }
    result = dlg_check_get(1);
    if (result != 0) {
      game.game_options = game.game_options | OPT_DIPLOMACY_SCREEN;
    }
    result = dlg_check_get(2);
    if (result != 0) {
      if (hires_was_on == 0) {
        result = mem_check_available(0x800000);
        if (result == 0) {
          popup_game((const char *)&str_lowmemory);
        }
      }
      game.game_options = game.game_options | OPT_ANIMATED_HERALDS;
    }
    result = dlg_check_get(3);
    if (result != 0) {
      game.game_options = game.game_options | OPT_PEDIA_ADVANCES;
    }
    result = dlg_check_get(4);
    if (result != 0) {
      game.game_options = game.game_options | OPT_HIGH_COUNCIL;
    }
    result = dlg_check_get(5);
    if (result != 0) {
      game.game_options = game.game_options | OPT_WONDER_MOVIES;
    }
    prefs.game_options = game.game_options;
    prefs_save_civ2dat();
  }
  return;
}

/* 004e0f18 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl menu_multiplayer_options(void)

{
  int result;
  int choice;
  
  dlg_checks_clear();
  dlg_check_set_bool(0,(int)(short)(*(ushort *)&menu_prefs.clear_chat_on_new_game));
  dlg_check_set_bool(1,(int)(short)(*(ushort *)&menu_prefs.clear_chat_on_boot));
  if (net.host_flag != '\0') {
    dlg_check_set_bool(2,(int)(short)(*(ushort *)&mp_doubles.production));
    dlg_check_set_bool(3,(int)(short)(*(ushort *)&mp_doubles.moves));
  }
  if (((game.multiplayer < 3) || (net.host_flag == '\0')) && (2 < game.multiplayer)) {
    choice = popup_game_flags((const char *)&str_multiplayeroptions2,1);
  }
  else {
    choice = popup_game_flags((const char *)&str_multiplayeroptions,1);
  }
  if (choice == 0) {
    result = dlg_check_get(0);
    (*(ushort *)&menu_prefs.clear_chat_on_new_game) = (ushort)(result != 0);
    result = dlg_check_get(1);
    (*(ushort *)&menu_prefs.clear_chat_on_boot) = (ushort)(result != 0);
    prefs_save_civ2dat();
    if (((2 < game.multiplayer) && (net.host_flag != '\0')) || (game.multiplayer < 3)) {
      result = dlg_check_get(2);
      (*(ushort *)&menu_prefs.mp_double_production) = (ushort)(result != 0);
      result = dlg_check_get(3);
      (*(ushort *)&menu_prefs.mp_double_moves) = (ushort)(result != 0);
      if (((*(ushort *)&mp_doubles.production) != (*(ushort *)&menu_prefs.mp_double_production)) || ((*(ushort *)&mp_doubles.moves) != (*(ushort *)&menu_prefs.mp_double_moves))) {
        if ((2 < game.multiplayer) && (1 < net.net_player_count[0])) {
          crt_strcpy(((i8 *)&text_args.strings),((i8 *)&net.local_player_name));
          str_clear((char *)&text_args.string1);
          str_append_label((iptr)&text_args.string1,0x350 - (uint)((*(ushort *)&menu_prefs.mp_double_production) == 0));
          crt_strcat(((i8 *)&text_args.string1),((i8 *)&mp_options_vote_sep_str));
          str_append_label((iptr)&text_args.string1,0x352 - (uint)((*(ushort *)&menu_prefs.mp_double_moves) == 0));
          net.vote_replies_pending = net.net_player_count[0] - 1;
          net.vote_status = 0;
          net_send_text_args(0x43,0xff,2,0,0,0);
          dlg_style.pending_idle_callback = net_vote_wait_idle_cb;
          dialog_state.timeout_start = timer_ticks();
          result = popup_game_sprite_flags((const char *)&str_pmchangeserver,0x2000001,(iptr)&popup_info_icon_sprite,0);
          if (((result < 0) || (net.vote_status == -1)) || (net.vote_replies_pending != 0)) {
            (*(ushort *)&menu_prefs.mp_double_production) = (*(ushort *)&mp_doubles.production);
            (*(ushort *)&menu_prefs.mp_double_moves) = (*(ushort *)&mp_doubles.moves);
            net_send_text_args(0x44,0xff,0,0,0,0);
            popup_game_with_sprite((const char *)&str_pmchangeno,(iptr)&popup_info_icon_sprite,0);
            return;
          }
        }
        (*(ushort *)&mp_doubles.production) = (*(ushort *)&menu_prefs.mp_double_production);
        (*(ushort *)&mp_doubles.moves) = (*(ushort *)&menu_prefs.mp_double_moves);
        rules_apply_double_moves();
        if ((2 < game.multiplayer) && (1 < net.net_player_count[0])) {
          net_send_message(NM_UNITS,0xff,0,0,0,0,0,0,0,0);
          net_send_message(NM_MULTIOPTIONS_CHANGE,0xff,(int)(short)(*(ushort *)&mp_doubles.production),(int)(short)(*(ushort *)&mp_doubles.moves),
                             (int)mp_options.simultaneous_setting,0,0,0,0,0);
          net_send_text_args(0x45,0xff,0,0,0,0);
          popup_game_with_sprite((const char *)&str_pmchangeyes,(iptr)&popup_info_icon_sprite,0);
        }
      }
    }
  }
  return;
}

/* 004e1452 auto */
void __cdecl menu_message_options(void)

{
  int result;
  
  dlg_checks_clear();
  dlg_check_set(0,~(uint)game.message_options & MSGOFF_GROWTH_HALTED);
  dlg_check_set(1,(uint)((game.message_options & MSGOFF_IMPROVEMENT_BUILT) == 0));
  dlg_check_set(2,(uint)((game.message_options & MSGOFF_UNIT_BUILT) == 0));
  dlg_check_set(3,(uint)((game.message_options & MSGOFF_INVALID_BUILD) == 0));
  dlg_check_set(4,(uint)((game.message_options & MSGOFF_DISORDER) == 0));
  dlg_check_set(5,(uint)((game.message_options & MSGOFF_ORDER_RESTORED) == 0));
  dlg_check_set(6,(uint)((game.message_options & MSGOFF_CELEBRATION) == 0));
  dlg_check_set(7,(uint)((game.message_options & MSGOFF_FOOD_LOW) == 0));
  dlg_check_set(8,(uint)((game.message_options & MSGOFF_POLLUTION) == 0));
  dlg_check_set(9,(uint)((game.message_options & MSGOFF_PRODUCTION_PENALTY) == 0));
  dlg_check_set(10,(uint)((game.message_options & MSGOFF_NO_ZOOM_TO_CITY) == 0));
  result = popup_game_flags((const char *)&str_messageoptions,1);
  if (result == 0) {
    game.message_options = 0;
    result = dlg_check_get(0);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_GROWTH_HALTED;
    }
    result = dlg_check_get(1);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_IMPROVEMENT_BUILT;
    }
    result = dlg_check_get(2);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_UNIT_BUILT;
    }
    result = dlg_check_get(3);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_INVALID_BUILD;
    }
    result = dlg_check_get(4);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_DISORDER;
    }
    result = dlg_check_get(5);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_ORDER_RESTORED;
    }
    result = dlg_check_get(6);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_CELEBRATION;
    }
    result = dlg_check_get(7);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_FOOD_LOW;
    }
    result = dlg_check_get(8);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_POLLUTION;
    }
    result = dlg_check_get(9);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_PRODUCTION_PENALTY;
    }
    result = dlg_check_get(10);
    if (result == 0) {
      game.message_options = game.message_options | MSGOFF_NO_ZOOM_TO_CITY;
    }
    prefs.message_options = game.message_options;
    prefs_save_civ2dat();
  }
  return;
}

/* 004e2597 auto */
void __cdecl menu_show_hidden_terrain(void)

{
  blink.unit_active = 0;
  map_show_hidden_terrain = 1;
  map_redraw_all_views(human_civ[0],1);
  popup_game((const char *)&str_hiddenterrain);
  map_show_hidden_terrain = 0;
  map_redraw_all_views(human_civ[0],1);
  return;
}

/* 004e25ef auto */
void __cdecl menu_select_music(void)

{
  int choice;
  Dialog dialog2;
  int track_count;
  int tries;
  i8 *seh_scope;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  tries = 0;
  while (tries < 5) {
    tries++;
    track_count = snd_cd_get_track_count();
    if ((((track_count == 0x18) || (track_count == 0x12)) || (track_count == 0xc)) || (track_count == 10))
    break;
    if (track_count == -1) {
      snd_cd_close();
    }
    Sleep(200);
  }
  if (track_count == -1) {
    popup_game((const char *)&str_nopickmusicnew);
  }
  else {
    if (track_count == 0x18) {
      dialog2.load_game_section((const char *)&str_pickmusicfanworlds,1);
    }
    else if (track_count == 0x12) {
      dialog2.load_game_section((const char *)&str_pickmusicscenario,1);
    }
    else if (track_count == 0xc) {
      dialog2.load_game_section((const char *)&str_pickmusicgold,1);
    }
    else {
      if (track_count != 10) {
        popup_game((const char *)&str_nopickmusicnew_2);
        return;
      }
      dialog2.load_game_section((const char *)&str_pickmusic,1);
    }
    if (1 < (*(int *)&cd_music.current_track)) {
      dialog2.set_default_choice((*(int *)&cd_music.current_track) - 2);
    }
    choice = dialog2.show_modal(0);
    if (choice >= 0) {
      music_play_track(choice + 2,1);
    }
  }
  return;
}

/* Part of menu_command (004e2803): the Game menu's options dialogs (0x101-0x106). */
static void menu_cmd_game_options(int cmd)
{
  switch(cmd) {
  case CMD_GAME_OPTIONS:
    menu_game_options();
    break;
  case CMD_GRAPHIC_OPTIONS:
    menu_graphic_options();
    break;
  case CMD_CITY_REPORT_OPTIONS:
    menu_message_options();
    break;
  case CMD_MULTIPLAYER_OPTIONS:
    menu_multiplayer_options();
    break;
  case CMD_GAME_PROFILE:
    game_profile_popup(0);
    break;
  case CMD_PICK_MUSIC:
    menu_select_music();
    break;
  case CMD_OPENCIV2_OPTIONS:
    extras_options_dialog();
  }
}

/* Part of menu_command (004e2803): the View menu's zoom presets, the
   grid-style toggle and the window layout reset (0x321-0x328). */
static void menu_cmd_view_zoom(int cmd)
{
  switch(cmd) {
  case CMD_STANDARD_ZOOM:
    map_views_block.mapview_zoom[0] = 0;
    map_views_block.map_view_center_x[0] = map_cursor.x[0];
    map_views_block.mapview_center_y[0] = map_cursor.y[0];
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
    break;
  case CMD_MEDIUM_ZOOM_OUT:
    map_views_block.mapview_zoom[0] = -3;
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
    break;
  case CMD_MAX_ZOOM_OUT:
    map_views_block.mapview_zoom[0] = -7;
    ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
    break;
  case CMD_MAP_GRID:
    game.game_options = game.game_options ^ OPT_MAP_GRID;
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_MAP_GRID,(game.game_options & OPT_MAP_GRID) >> 5);
    map_redraw_all_views(human_civ[0],1);
    prefs.game_options = game.game_options;
    prefs_save_civ2dat();
    break;
  case CMD_ARRANGE_WINDOWS:
    extras_set_panel_size(0);
    ui_reset_window_layout();
    break;
  case CMD_ARRANGE_WINDOWS_S:
  case CMD_ARRANGE_WINDOWS_L:
    /* X38: Arrange Windows with a smaller or larger side panel */
    extras_set_panel_size(cmd == CMD_ARRANGE_WINDOWS_S ? -1 : 1);
    ui_reset_window_layout();
  }
}

/* Part of menu_command (004e2803): the Orders menu's terrain orders,
   automate and pillage (0x410-0x421). */
static void menu_cmd_terraform(int cmd)
{
  switch(cmd) {
  case CMD_BUILD_ROAD:
    unit_cmd_terraform(ORDER_BUILD_ROAD);
    break;
  case CMD_BUILD_IRRIGATION:
    unit_cmd_terraform(ORDER_IRRIGATE);
    break;
  case CMD_BUILD_MINES:
    unit_cmd_terraform(ORDER_MINE);
    break;
  case CMD_TRANSFORM:
    unit_cmd_terraform(ORDER_TRANSFORM);
    break;
  case CMD_BUILD_AIRBASE:
    unit_cmd_terraform(ORDER_BUILD_AIRBASE);
    break;
  case CMD_BUILD_FORTRESS:
    unit_cmd_terraform(ORDER_BUILD_FORTRESS);
    break;
  case CMD_AUTOMATE:
    unit_cmd_automate();
    break;
  case CMD_CLEAN_POLLUTION:
    unit_cmd_terraform(ORDER_CLEAN_POLLUTION);
    break;
  case CMD_PILLAGE:
    unit_cmd_pillage();
  }
}

/* Part of menu_command (004e2803): the Orders menu's go to, paradrop,
   airlift and home city (0x440-0x445). */
static void menu_cmd_unit_moves(int cmd)
{
  switch(cmd) {
  case CMD_GOTO:
    unit_cmd_goto_city();
    break;
  case CMD_PARADROP:
    unit_cmd_paradrop();
    break;
  case CMD_AIRLIFT:
    unit_cmd_airlift();
    break;
  case CMD_HOME_CITY:
    unit_cmd_home_city();
  }
}

/* Part of menu_command (004e2803): the Advisors menu (0x500-0x508). */
static void menu_cmd_advisors(int cmd)
{
  SHORT shift_state;

  switch(cmd) {
  case CMD_HIGH_COUNCIL:
    if ((((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) ||
       (shift_state = GetAsyncKeyState(0x10), (char)((ushort)shift_state >> 8) == '\0')) {
      council_show(human_civ[0],0);
    }
    else {
      council_show(human_civ[0],1);
    }
    break;
  case CMD_CITY_STATUS:
    city_status_show(human_civ[0]);
    break;
  case CMD_DEFENSE_MINISTER:
    defense_minister_show(human_civ[0]);
    break;
  case CMD_FOREIGN_MINISTER:
    foreign_minister_show(human_civ[0]);
    break;
  case CMD_ATTITUDE_ADVISOR:
    attitude_advisor_show(human_civ[0]);
    break;
  case CMD_TRADE_ADVISOR:
    trade_advisor_show(human_civ[0]);
    break;
  case CMD_SCIENCE_ADVISOR:
    science_advisor_show(human_civ[0]);
    break;
  case CMD_CHAT_WITH_KINGS:
    chat_wnd_block.open(0,4);
    break;
  case CMD_CASUALTY_TIMELINE:
    casualty_timeline_show(human_civ[0]);
  }
}

/* Part of menu_command (004e2803): the World menu's reports (0x602-0x606;
   with shift in cheat mode: the endgame movies, the destroyed civs). */
static void menu_cmd_world_reports(int cmd)
{
  SHORT shift_state;

  switch(cmd) {
  case CMD_TOP_CITIES:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      top_cities_show(human_civ[0]);
    }
    else {
      endgame_defeat_movie(human_civ[0]);
    }
    break;
  case CMD_SCORE:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      civ_score_window_show(human_civ[0]);
      if (game.reveal_map != '\0') {
        civ_rating_window_show(human_civ[0]);
        powergraph_show();
      }
    }
    else {
      destroyed_civs_show(human_civ[0]);
    }
    break;
  case CMD_DEMOGRAPHICS:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      demographics_show(human_civ[0]);
    }
    else {
      endgame_spaceship_arrival(-(human_civ[0] + 1U & 7));
    }
    break;
  case CMD_SPACESHIPS:
    shift_state = GetAsyncKeyState(0x10);
    if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
       (game.multiplayer != '\0')) {
      spaceship_select_view_dialog(human_civ[0]);
    }
    else {
      endgame_spaceship_arrival(human_civ[0]);
    }
  }
}

/* Part of menu_command (004e2803): the Cheat menu's scenario editors
   (0x801-0x809). */
static void menu_cmd_editors(int cmd)
{
  switch(cmd) {
  case CMD_SCENARIO_FLAG:
    editor_toggle_scenario_flag();
    break;
  case CMD_ADVANCES_EDITOR:
    tech_editor_run();
    break;
  case CMD_CITIES_EDITOR:
    city_editor_run();
    break;
  case CMD_EFFECTS_EDITOR:
    cosmic_editor_run();
    break;
  case CMD_IMPROVEMENTS_EDITOR:
    impr_editor_run();
    break;
  case CMD_TERRAIN_EDITOR:
    terrain_editor_open();
    break;
  case CMD_TRIBE_EDITOR:
    tribe_editor_run();
    break;
  case CMD_UNITS_EDITOR:
    unit_editor_run();
    break;
  case CMD_EVENTS_EDITOR:
    events_editor_open();
  }
}

/* Part of menu_command (004e2803): the Civilopedia pages and the credits
   (0x903-0x9f0). */
static void menu_cmd_pedia(int cmd)
{
  SHORT shift_state;
  int result;

  switch(cmd) {
  case CMD_PEDIA_IMPROVEMENTS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(2,1);
    break;
  case CMD_PEDIA_WONDERS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(3,1);
    break;
  case CMD_PEDIA_UNITS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(4,1);
    break;
  case CMD_PEDIA_GOVERNMENTS:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(6,1);
    break;
  case CMD_PEDIA_TERRAIN:
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(5,1);
    break;
  case CMD_PEDIA_CONCEPTS:
    if ((cmd == CMD_PEDIA_WONDERS) &&
       (shift_state = GetAsyncKeyState(0x10), (char)((ushort)shift_state >> 8) != '\0')) {
      menu_wonder_movies();
    }
    else {
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(7,1);
    }
    break;
  case CMD_ABOUT:
    if (game.multiplayer < 3) {
      result = popup_game_flags((const char *)&str_newcredits_2,1);
      if (-1 < result) {
        credits_show(result);
      }
      text_close_file();
    }
  }
}

/* 004e2803 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl menu_command(i32 menu,int cmd)

{
  SHORT shift_state;
  int result;
  
  result = XD_InFlushSendBuffer();
  if ((((((((result == 0) && (net.unit_create_busy == 0)) && (net.unit_delete_busy == 0)) &&
         ((net.city_create_busy == 0 && (net.city_delete_busy == 0)))) && (net.lock_map_busy == 0)) &&
       (((net.unlock_map_busy == 0 && (net.unit_stack_unlink_busy == 0)) &&
        ((net.unit_stack_link_busy == 0 && (((net.unit_stack_bottom_busy == 0 && (net.unit_relocate_busy == 0)) && (net.city_acquire_wonder_busy == 0)))
         ))))) && ((net.civ_eliminate_busy == 0 && (net.civ_create_busy == 0)))) &&
     ((net.civ_check_destroyed_busy == 0 &&
      (((net.unit_gather_cargo_busy == 0 && (net.unit_lift_off_map_busy == 0)) && ((net.unit_delete_cargo_busy == 0 && (net.unit_destroy_redraw_busy == 0)))))
      ))) {
    if (city_turn.processing == 0) {
      result = ui_paradrop_cursor_active();
      if ((result == 0) || (mapviews_reset_cursor(), cmd != CMD_PARADROP)) {
        if (cmd < 0x111) {
          if (cmd == CMD_SAVE_GAME) {
            game_save_dialog(0);
          }
          else {
            menu_cmd_game_options(cmd);
          }
        }
        else if (cmd < 0x131) {
          if (cmd == CMD_JOIN_GAME) {
            game_join_dialog();
          }
          else if (cmd == CMD_LOAD_GAME) {
            menu_load_game();
          }
        }
        else if (cmd < 0x1f1) {
          if (cmd == CMD_RETIRE) {
            game_quit_or_retire(1);
          }
          else if (cmd == CMD_SET_PASSWORD) {
            civ_change_password_dialog(human_civ[0]);
          }
          else if (cmd == CMD_CHANGE_TIMER) {
            mp_cmd_change_timer();
          }
        }
        else if (cmd < 0x202) {
          if (cmd == CMD_TAX_RATE) {
            taxrate_dialog_show(human_civ[0]);
          }
          else if (cmd == CMD_QUIT) {
            game_quit_or_retire(0);
          }
          else if ((cmd == 0x1f2) && (game.multiplayer == 1)) {
            result = popup_game_with_sprite((const char *)&str_reallyquit_4,(iptr)&popup_info_icon_sprite,0);
            if (result == 0) {
              return;
            }
            snd_cd_stop();
            map_display_disable();
            return;
          }
        }
        else if (cmd < 0x211) {
          if (cmd == CMD_FIND_CITY) {
            menu_find_city();
          }
          else if (cmd == CMD_THRONE_ROOM) {
            throneroom_show(human_civ[0]);
          }
        }
        else if (cmd < 0x302) {
          if (cmd == CMD_MOVE_PIECES) {
            ui_focus_active_unit(1);
          }
          else if (cmd == CMD_REVOLUTION) {
            menu_revolution();
          }
        }
        else if (cmd < 0x311) {
          if (cmd == CMD_ZOOM_IN) {
            if (map_views_block.mapview_zoom[0] < 8) {
              map_views_block.mapview_zoom[0]++;
              ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
            }
          }
          else if (cmd == CMD_VIEW_PIECES) {
            ui_redraw_all_maps(0);
          }
        }
        else if (cmd < 0x321) {
          if (cmd == CMD_MAX_ZOOM_IN) {
            map_views_block.mapview_zoom[0] = 8;
            ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
          }
          else if ((cmd == CMD_ZOOM_OUT) && (-7 < map_views_block.mapview_zoom[0])) {
            map_views_block.mapview_zoom[0]--;
            ((MapView *)((i8 *)&map_views_block.map_views))->redraw(human_civ[0],1);
          }
        }
        else if (cmd < 0x331) {
          if (cmd == CMD_SHOW_HIDDEN_TERRAIN) {
            menu_show_hidden_terrain();
          }
          else {
            menu_cmd_view_zoom(cmd);
          }
        }
        else if (cmd < 0x402) {
          if (cmd == CMD_BUILD_CITY) {
            unit_cmd_build_city();
          }
          else if (cmd == CMD_CENTER_VIEW) {
            ((MapView *)((i8 *)&map_views_block.map_views))->center_on((int)map_cursor.x[0],(int)map_cursor.y[0]);
          }
        }
        else if (cmd < 0x431) {
          if (cmd == CMD_UNLOAD) {
            unit_cmd_unload();
          }
          else {
            menu_cmd_terraform(cmd);
          }
        }
        else if (cmd < 0x451) {
          if (cmd == CMD_FORTIFY) {
            unit_cmd_fortify();
          }
          else {
            menu_cmd_unit_moves(cmd);
          }
        }
        else if (cmd < 0x461) {
          if (cmd == CMD_DISBAND) {
            unit_cmd_disband();
          }
          else if (cmd == CMD_SLEEP) {
            unit_cmd_sleep();
          }
        }
        else if (cmd < 0x471) {
          if (cmd == CMD_WAIT) {
            unit_cmd_wait();
          }
          else if (cmd == CMD_ACTIVATE_UNIT) {
            ui_pick_unit_from_stack();
          }
        }
        else if (cmd < 0x491) {
          if (cmd == CMD_END_TURN) {
            cmd_end_player_turn();
          }
          else if (cmd == CMD_SKIP_TURN) {
            cmd_unit_skip_turn();
          }
        }
        else if (cmd < 0x602) {
          if (cmd == CMD_WONDERS) {
            shift_state = GetAsyncKeyState(0x10);
            if ((((char)((ushort)shift_state >> 8) == '\0') || ((game.game_options & OPT_CHEAT_MODE) == 0)) ||
               (game.multiplayer != '\0')) {
              wonders_report_show(human_civ[0]);
            }
            else {
              menu_wonder_movies();
            }
          }
          else {
            menu_cmd_advisors(cmd);
          }
        }
        else if (cmd < 0x702) {
          if (cmd == CMD_CHEAT_MODE) {
            if ((game.multiplayer == '\0') || (mp_cheat_menu_allowed[0] != 0)) {
              cheat_toggle_mode();
            }
          }
          else {
            menu_cmd_world_reports(cmd);
          }
        }
        else if (cmd < 0x722) {
          if (cmd == CMD_CHEAT_SET_YEAR) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_set_game_year();
            }
          }
          else if (cmd == CMD_CHEAT_CREATE_UNIT) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_create_unit();
            }
          }
          else if (cmd == CMD_CHEAT_REVEAL_MAP) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_reveal_map();
            }
          }
          else if (((cmd == CMD_CHEAT_SET_HUMAN) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_set_human_player();
          }
        }
        else if (cmd < 0x732) {
          if (cmd == CMD_CHEAT_TECH_ADVANCE) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_tech_advance();
            }
          }
          else if (((cmd == CMD_CHEAT_KILL_CIV) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_kill_civ();
          }
        }
        else if (cmd < 0x741) {
          if (cmd == CMD_CHEAT_FORCE_GOVERNMENT) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_force_government();
            }
          }
          else if (((cmd == CMD_CHEAT_EDIT_TECHS) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_edit_technologies();
          }
        }
        else if (cmd < 0x751) {
          if (cmd == CMD_CHEAT_DESTROY_UNITS) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_destroy_units_at_cursor();
              ui_focus_active_unit(0);
            }
          }
          else if (((cmd == CMD_CHEAT_CHANGE_TERRAIN) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_terrain_saved_bits = 0xffffffff;
            cheat_change_terrain();
          }
        }
        else if (cmd < 0x756) {
          if (cmd == CMD_CHEAT_EDIT_UNIT) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_edit_unit();
            }
          }
          else if (((cmd == CMD_CHEAT_CHANGE_MONEY) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_change_money();
          }
        }
        else if (cmd < 0x766) {
          if (cmd == CMD_CHEAT_EDIT_KING) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_edit_king();
            }
          }
          else if (((cmd == CMD_CHEAT_EDIT_CITY) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_edit_city();
          }
        }
        else if (cmd < 0x771) {
          if (cmd == CMD_CHEAT_SAVE_SCENARIO) {
            if (((game.game_options & OPT_CHEAT_MODE) != 0) && (game.multiplayer == '\0')) {
              cheat_save_as_scenario();
            }
          }
          else if (((cmd == CMD_CHEAT_SCENARIO_PARAMS) && ((game.game_options & OPT_CHEAT_MODE) != 0)) &&
                  (game.multiplayer == '\0')) {
            cheat_scenario_params();
          }
        }
        else if (cmd < 0x902) {
          if (cmd == CMD_PEDIA_ADVANCES) {
            ((PediaWindow *)((i8 *)&pedia_wnd.window))->open_category(1,1);
          }
          else {
            menu_cmd_editors(cmd);
          }
        }
        else {
          menu_cmd_pedia(cmd);
        }
      }
    }
    else {
      city_window_modal_notice();
    }
  }
  else {
    dbg_log((const char *)&str_mainmenu_menu_exec_blocked_by_xd_flushse);
    net.blocked_input_kind = 5;
    net.blocked_input_arg1 = menu;
    net.blocked_input_arg2 = cmd;
  }
  return;
}

/* 004e3a86 auto */
void __cdecl menu_build_main(void)

{
  int status;
  i32 editor_disabled;
  i32 editor_checked;
  i32 cheat_checked;
  i32 cheat_disabled;
  int row;
  int options_added = 0;
  
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->reset(0x1bbc);
  /* (port) The menus' structure is menu_rows; their texts are looked up by
     key (lang.h), not read from MENU.TXT line by line.  An item without a
     text is left out (the CD's MENU.TXT has no Game Profile). */
  lang_load_menus();
  for (row = 0; row < menu_row_count; row++) {
    if (menu_rows[row].cmd == MENU_TITLE) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_menu(menu_rows[row].menu,
                    (char *)lang_text(menu_rows[row].key));
    }
    else if (menu_rows[row].cmd == 0) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,0,0,0);
    }
    else if (lang_has(menu_rows[row].key) != 0) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,menu_rows[row].cmd,
                    lang_text(menu_rows[row].key),menu_rows[row].room);
      if (menu_rows[row].cmd == CMD_ARRANGE_WINDOWS && plat_extras) {
        /* X38: Arrange Windows S and L (hidden while X38 is off: the options
           dialog can switch it) */
        static char label_s[64], label_l[64];
        const char *label = lang_text(menu_rows[row].key);
        int n = (int)strcspn(label, "|");
        snprintf(label_s, sizeof label_s, "%.*s S", n, label);
        snprintf(label_l, sizeof label_l, "%.*s L", n, label);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,CMD_ARRANGE_WINDOWS_S,
                      extras_text_translated("menu.arrange_s") ? extras_text("menu.arrange_s") : label_s,0);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(menu_rows[row].menu,CMD_ARRANGE_WINDOWS_L,
                      extras_text_translated("menu.arrange_l") ? extras_text("menu.arrange_l") : label_l,0);
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(CMD_ARRANGE_WINDOWS_S,!extra_ui(X_ARRANGE));
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(CMD_ARRANGE_WINDOWS_L,!extra_ui(X_ARRANGE));
      }
    }
    if (plat_extras && menu_rows[row].menu == 1 && menu_rows[row + 1].menu == 1 &&
        menu_rows[row + 1].cmd == 0 && !options_added) {
      /* OpenCiv2's options, after the Game menu's options dialogs */
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->add_item(1,CMD_OPENCIV2_OPTIONS,
                    extras_text("menu.options"),0);
      options_added = 1;
    }
  }
  if (((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) {
    cheat_disabled = 1;
  }
  else {
    cheat_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(7,cheat_disabled);
  if (((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) {
    cheat_checked = 0;
  }
  else {
    cheat_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_CHEAT_MODE,cheat_checked);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHEAT_MODE,
             (game.multiplayer != '\0' &((int *)&mp_cheat_menu_allowed) == 0));
  if (((((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) ||
      ((game.game_flags & GAME_SCENARIO) == 0)) ||
     (status = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), status == 0)) {
    editor_checked = 0;
  }
  else {
    editor_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_SCENARIO_FLAG,editor_checked);
  if ((((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) ||
     (((game.game_flags & GAME_SCENARIO) == 0 ||
      (status = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), status == 0)))) {
    editor_disabled = 1;
  }
  else {
    editor_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(8,editor_disabled);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SCENARIO_FLAG,(game.multiplayer != '\0'));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x441,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x413,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x442,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x417,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x418,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x41b,1);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->rebuild((iptr)&main_frame_wnd_block.main_frame_wnd);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_command_handler(menu_command);
  return;
}

/* 004e4c92 auto */
void __cdecl menu_set_item_label(i32 item_id,i32 label_id,i32 enabled)

{
  i8 label_text[80];
  
  text_clear();
  text_append_label(label_id);
  text_expand_vars((iptr)&textbuf.buf,(iptr)label_text);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_text(item_id,(const char *)label_text);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(item_id,enabled);
  return;
}

/* Part of menu_update_states (004e4ceb): the settlers' orders (build city / join city, road
   and railroad, irrigation and farmland, mine, transform, fortress, airbase,
   pillage) for the unit's tile. */
static void menu_update_settler_orders(int unit,int x,int y,int civ,uptr terrain,bool is_ocean)
{
  char dy;
  bool no_adjacent_city;
  int city;
  iptr x2;
  uint improvements2;
  i32 improvements22;
  iptr airbase_owner;
  iptr airbase_owner2;
  int terrain2;
  byte *tile;
  i32 mine_disabled;
  i32 irrigate_disabled;
  i32 farmland_disabled;
  i32 road_disabled;
  i32 railroad_disabled;
  i32 build_city_disabled;
  iptr dir;

  no_adjacent_city = true;
  for (dir = 0; dir < 8; dir++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + x);
    dy = (((i8 *)&dir_dy_table_block.dir_dy_table))[dir];
    city = map_in_bounds(x2,dy + y);
    if ((city != 0) && (city = tile_city_owner(x2,dy + y), -1 < city)) {
      no_adjacent_city = false;
    }
  }
  city = city_at(x,y);
  if (city < 0) {
    terrain2 = tile_is_ocean(x,y);
    if ((terrain2 == TERRAIN_DESERT) && (no_adjacent_city)) {
      build_city_disabled = 0;
    }
    else {
      build_city_disabled = 1;
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_CITY,build_city_disabled);
  }
  else {
    menu_set_item_label(CMD_BUILD_CITY,0x33,(uint)('\t' < cities[city].size));
  }
  improvements2 = tile_improvements(x,y);
  if ((improvements2 & TILE_POLLUTION) == 0) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CLEAN_POLLUTION,1);
  }
  improvements22 = tile_improvements(x,y);
  if ((((((byte)improvements22 & TILE_AIRBASE) == 0x40) ||
       (airbase_owner = tile_airbase_owner(x,y), -1 < airbase_owner)) || (is_ocean)) || (-1 < city)) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_FORTRESS,1);
  }
  improvements22 = tile_improvements(x,y);
  if (((((byte)improvements22 & TILE_AIRBASE) == 0x40) ||
      (airbase_owner2 = tile_airbase_owner(x,y), -1 < airbase_owner2)) || ((is_ocean || (-1 < city)))) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_AIRBASE,1);
  }
  improvements2 = tile_improvements(x,y);
  if (((improvements2 & TILE_ROAD) == 0) || (terrain2 = civ_has_advance(civ,ADV_RAILROAD), terrain2 == TERRAIN_DESERT)) {
    if (((is_ocean) ||
        ((-1 < city || (improvements2 = tile_improvements(x,y), (improvements2 & TILE_ROAD) != 0)))) ||
       ((tile = (byte *)tile_at(x,y), (*tile & 0x80) != 0 &&
        (terrain2 = civ_has_advance(civ,ADV_BRIDGE_BUILDING), terrain2 == TERRAIN_DESERT)))) {
      road_disabled = 1;
    }
    else {
      road_disabled = 0;
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_ROAD,road_disabled);
  }
  else {
    improvements2 = tile_improvements(x,y);
    if ((((improvements2 & TILE_RAILROAD) != 0) || (-1 < city)) || (is_ocean)) {
      railroad_disabled = 1;
    }
    else {
      railroad_disabled = 0;
    }
    menu_set_item_label(CMD_BUILD_ROAD,0x35,railroad_disabled);
  }
  terrain2 = (int)terrain_types[terrain].irrigate_to;
  if (terrain2 < 0) {
    improvements2 = tile_improvements(x,y);
    if ((improvements2 & TILE_IRRIGATION) == 0) {
      if (((terrain2 == -1) || (is_ocean)) || (-1 < city)) {
        irrigate_disabled = 1;
      }
      else {
        irrigate_disabled = 0;
      }
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_IRRIGATION,irrigate_disabled);
    }
    else {
      terrain2 = civ_has_advance(civ,ADV_REFRIGERATION);
      if (((terrain2 == TERRAIN_DESERT) || (is_ocean)) || (-1 < city)) {
        farmland_disabled = 1;
      }
      else {
        farmland_disabled = 0;
      }
      menu_set_item_label(CMD_BUILD_IRRIGATION,0x3d,farmland_disabled);
    }
  }
  else {
    text_set_label_arg(0,terrain_types[terrain2].name);
    menu_set_item_label(CMD_BUILD_IRRIGATION,0x37,0);
  }
  terrain2 = (int)terrain_types[terrain].mine_to;
  if (terrain2 < 0) {
    if (((terrain2 == -1) || (is_ocean)) || (-1 < city)) {
      mine_disabled = 1;
    }
    else {
      mine_disabled = 0;
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_MINES,mine_disabled);
  }
  else {
    text_set_label_arg(0,terrain_types[terrain2].name);
    menu_set_item_label(CMD_BUILD_MINES,0x39,0);
  }
  civ = civ_can_build_unit(civ,-1,UNIT_ENGINEERS);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x413,(uint)(civ == 0));
  if (terrain_types[terrain].transform_to < 0) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_TRANSFORM,1);
  }
  else {
    text_set_label_arg(0,terrain_types[terrain_types[terrain].transform_to].name);
    menu_set_item_label(CMD_TRANSFORM,0x3c,(uint)(units[unit].type != UNIT_ENGINEERS));
  }
}

/* Part of menu_update_states (004e4ceb): the Game and Kingdom items that depend on single
   or multiplayer (join, load, chat, timer, save, quit, retire). */
static void menu_update_multiplayer_items(void)
{
  if (game.multiplayer == '\0') {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_JOIN_GAME,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHAT_WITH_KINGS,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_MULTIPLAYER_OPTIONS,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_GAME_PROFILE,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_LOAD_GAME,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_ABOUT,0);
  }
  else {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_ABOUT,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_MULTIPLAYER_OPTIONS,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_GAME_PROFILE,0);
    if (mp_cheat_menu_allowed[0] == 0) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHEAT_MODE,1);
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_LOAD_GAME,1);
    if ((mp_options.open_game == 0) || (game.multiplayer != 1)) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_JOIN_GAME,1);
    }
    else {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_JOIN_GAME,0);
    }
    if (game.multiplayer == 1) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_THRONE_ROOM,1);
    }
    else {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_THRONE_ROOM,0);
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHAT_WITH_KINGS,(game.multiplayer < 3));
    if (game.multiplayer == 2) {
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,1);
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SAVE_GAME,1);
    }
    else {
      if (net.host_flag == '\0') {
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,1);
      }
      else {
        ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHANGE_TIMER,0);
      }
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SAVE_GAME,0);
    }
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_QUIT,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_RETIRE,0);
  }
}

/* Part of menu_update_states (004e4ceb): the Orders menu for the active unit. */
static void menu_update_unit_orders(void)
{
  u8 unit_role;
  byte terrain_byte;
  int unit;
  int x;
  int y;
  int civ;
  iptr has_airbase_tech;
  iptr has_fortress_tech;
  int city;
  uptr terrain;
  bool is_ocean;
  i32 fortress_hidden;
  i32 airbase_hidden;
  i32 pillage_hidden;

  unit = (int)game.active_unit;
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  civ = (int)units[unit].owner;
  terrain_byte = tile_terrain(x,y);
  terrain = (uptr)terrain_byte;
  is_ocean = terrain == TERRAIN_OCEAN;
  unit_role = unit_types[units[unit].type].role;
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(4,0);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x401,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_CITY,0x32,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_ROAD,0x34,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_IRRIGATION,0x36,(uint)(unit_role != ROLE_SETTLE));
  menu_set_item_label(CMD_BUILD_MINES,0x38,(uint)(unit_role != ROLE_SETTLE));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x450,(uint)(unit_role == ROLE_SETTLE));
  if ((unit_role == ROLE_SETTLE) && (has_airbase_tech = civ_has_advance(civ,ADV_CONSTRUCTION), has_airbase_tech != 0)) {
    pillage_hidden = 0;
  }
  else {
    pillage_hidden = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x418,pillage_hidden);
  if ((unit_role == ROLE_SETTLE) && (has_fortress_tech = civ_has_advance(civ,ADV_RADIO), has_fortress_tech != 0)) {
    airbase_hidden = 0;
  }
  else {
    airbase_hidden = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x417,airbase_hidden);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x430,(uint)(unit_role == ROLE_SETTLE));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x41b,(uint)(unit_role != ROLE_SETTLE));
  city = civ_has_advance(civ,ADV_COMBINED_ARMS);
  if ((city == 0) && ((unit_types[units[unit].type].flags & UTYPE_PARADROP) == 0)) {
    fortress_hidden = 1;
  }
  else {
    fortress_hidden = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x441,fortress_hidden);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_PARADROP,
             ((unit_types[units[unit].type].flags & UTYPE_PARADROP) == 0));
  city = civ_has_advance(civ,ADV_RADIO);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x442,(uint)(city == 0));
  city = tile_city_owner(x,y);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_AIRLIFT,(city < 0));
  if (unit_role == ROLE_SETTLE) {
    menu_update_settler_orders(unit,x,y,civ,terrain,is_ocean);
  }
  else {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_TRANSFORM,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_BUILD_FORTRESS,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CLEAN_POLLUTION,1);
  }
  civ = tile_city_owner(x,y);
  if (civ < 0) {
    menu_set_item_label(CMD_HOME_CITY,0x3b,0);
  }
  else {
    x = tile_city_owner(x,y);
    menu_set_item_label(CMD_HOME_CITY,0x3a,(uint)(x < 0));
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_UNLOAD,(unit_types[units[unit].type].hold == '\0'))
  ;
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_hidden(0x421,(uint)(unit_role == ROLE_SETTLE));
  if ((unit_types[units[unit].type].attack == '\0') ||
     (unit_types[units[unit].type].domain != DOMAIN_LAND)) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_PILLAGE,1);
  }
}

/* Part of menu_update_states (004e4ceb): the Cheat and Scenario menus. */
static void menu_update_cheat_items(void)
{
  iptr path_cmp;
  iptr path_cmp2;
  i32 scenario_menu_disabled;
  i32 scenario_checked;
  i32 cheat_checked;
  i32 cheat_menu_disabled;

  if ((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) {
    cheat_menu_disabled = 1;
  }
  else {
    cheat_menu_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(7,cheat_menu_disabled);
  if ((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) {
    cheat_checked = 0;
  }
  else {
    cheat_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_CHEAT_MODE,cheat_checked);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_CHEAT_MODE,(uint)(game.multiplayer != '\0' &((int *)&mp_cheat_menu_allowed) == 0)
            );
  if ((2 < game.multiplayer) && (net.mp_turn_civ != human_civ[0])) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(4,1);
  }
  if ((((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) ||
      ((game.game_flags & GAME_SCENARIO) == 0)) ||
     (path_cmp = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), path_cmp == 0)) {
    scenario_checked = 0;
  }
  else {
    scenario_checked = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_SCENARIO_FLAG,scenario_checked);
  if (((((*((char *)&game.game_options + 1)) & (OPT_CHEAT_MODE >> 8)) == 0) || (game.multiplayer != '\0')) ||
     (((game.game_flags & GAME_SCENARIO) == 0 || (path_cmp2 = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), path_cmp2 == 0)
      ))) {
    scenario_menu_disabled = 1;
  }
  else {
    scenario_menu_disabled = 0;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(8,scenario_menu_disabled);
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SCENARIO_FLAG,(game.multiplayer != '\0'))
  ;
}

/* 004e4ceb auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl menu_update_states(void)

{
  i32 revolution_disabled;
  
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_SPACESHIPS,game.rules & RULE_BLOODLUST);
  menu_update_multiplayer_items();
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_VIEW_PIECES,(moving_units_mode == 0));
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_MOVE_PIECES,(moving_units_mode == 1));
  if ((((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 0x10) == 0)) &&
     (civs[human_civ[0]].government != GOV_ANARCHY)) {
    revolution_disabled = 0;
  }
  else {
    revolution_disabled = 1;
  }
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_REVOLUTION,revolution_disabled);
  if (((moving_units_mode == 0) || (game.num_units == 0)) ||
     ((game.active_unit < 0 ||
      ((game.num_units < game.active_unit || (units[game.active_unit].id == 0)))))) {
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_menu_enabled(4,1);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_DISBAND,0);
    ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_enabled(CMD_ACTIVATE_UNIT,0);
  }
  else {
    menu_update_unit_orders();
  }
  menu_update_cheat_items();
  ((GameMenu *)((i8 *)&main_menu_block.main_menu))->rebuild(0);
  return;
}

/* 004e7240 auto */
void WndBase::show_restored()

{
  wnd_obj_show_restored(native_wnd);
  return;
}

/* 004eb80a auto */
int __cdecl popup_city_picture_zoom(const char *section,iptr city,uint picture,int silent,iptr civ)

{
  CityPicturePopup *self;
  int result;
  i32 height;
  i32 width;
  iptr option_text;
  iptr option_id;
  uint picture_id;
  void *popup_new;
  Sprite sprite;
  void *popup;
  GamePalette palette;
  i32 eh_prev;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  if (autoplay.active == 0) {
    if (silent == 0) {
      if (city_calc.notify_enabled == 0) {
        result = 0;
        return result;
      }
      civ = (iptr)cities[city].owner;
      mapviews_ensure_visible((int)cities[city].x,(int)cities[city].y,civ);
      text_set_string_arg_b(0,(const char *)&cities[city].name);
    }
    self = (CityPicturePopup *)operator_new(0x798);
    if (self == (CityPicturePopup *)0x0) {
      popup_new = (void *)0x0;
    }
    else {
      popup_new = (void *)self->construct();
    }
    popup = popup_new;
    result = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
    if (result != 0) {
      picture++;
    }
    if ((int)picture < 1) {
      picture_id = ~picture + 1;
    }
    else {
      picture_id = picture;
    }
    ((DibSurface *)((int)popup + 0x2d8))->load_gif_resource(picture_id,10,0xc0,(iptr)&palette);
    height = ((DibSurface *)((int)popup + 0x2d8))->surface_height();
    width = ((DibSurface *)((int)popup + 0x2d8))->surface_width();
    sprite.grab_from_surface((int)popup + 0x2d8,7,0,0,width,height);
    ((Dialog *)((int)popup + 0x4a4))->load_game_text(section);
    ((Dialog *)((int)popup + 0x4a4))->add_sprite((iptr)&sprite,0,0);
    if ((silent == 0) && (city_calc.zoom_requested == 0)) {
      height = 0;
      option_id = 1;
      option_text = (iptr)label_string(label_table.string_ids[0x20]);
      ((Dialog *)((int)popup + 0x4a4))->add_option((const char *)option_text,option_id,height);
      height = 0;
      option_id = 0;
      option_text = (iptr)label_string(label_table.string_ids[0x21]);
      ((Dialog *)((int)popup + 0x4a4))->add_option((const char *)option_text,option_id,height);
      if ((game.message_options & MSGOFF_NO_ZOOM_TO_CITY) == 0) {
        ((Dialog *)((int)popup + 0x4a4))->set_default_choice(0);
      }
      else {
        ((Dialog *)((int)popup + 0x4a4))->set_default_choice(1);
      }
    }
    result = ((Dialog *)((int)popup + 0x4a4))->show_modal(0);
    if ((silent == 0) && (0 < result)) {
      city_calc.zoom_requested = 1;
    }
    if (popup != (void *)0x0) {
      ((CityPicturePopup *)popup)->scalar_destruct(1);
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 00511320 auto */
u32 WndBase::get_palette()

{
  return wnd_palette;
}

/* 00511460 auto */
void ClickMap::hotspot_add_rect(iptr id,i32 cursor,RECT *rect)

{
  add(id,cursor,rect->left,rect->top,
                     rect->right - rect->left,
                     rect->bottom - rect->top);
  return;
}

/* 00511520 auto */
i32 __cdecl popup_game_with_city_flags(const char *section,i32 city,i32 flags)

{
  i32 result;
  
  result = popup_text_with_city((const char *)&str_game_7,section,city,flags);
  return result;
}

/* 00511820 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void FrameWindow::set_title_font(iptr font)

{
  title_font = font;
  return;
}

/* 00511850 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void FrameWindow::set_title_small_font(iptr font)

{
  title_small_font = font;
  return;
}

/* 00518e80 auto */
void WndBase::move_xy(i32 x,i32 y)

{
  wnd_obj_move_xy(native_wnd,x,y);
  return;
}

/* 00518ec0 auto */
i32 __cdecl dlg_input_number_game(const char *section,i32 initial,iptr out_value)

{
  i32 result;
  
  result = dlg_input_number((iptr)&str_game_7,section,initial,out_value);
  return result;
}

/* 0051d3e0 auto */
i32 __cdecl
popup_text_run(iptr file,const char *section,i32 result,i32 arg4,i32 arg5,
            i32 arg6,i32 arg7)

{
  int status;
  Dialog dialog2;
  int i;
  i32 eh_prev;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  status = dialog2.load_text_section((const char *)file,section,result,0,arg4,arg5,arg6,arg7);
  if (status == 0) {
    if (((byte)dialog2.style_flags & 4) != 0) {
      for (i = 0; i < (int)dialog2.option_count; i++) {
        dialog2.set_option_checked(i,1 << ((byte)i & 0x1f) & dlg_input.check_bits
                  );
      }
    }
    result = dialog2.show_modal(0);
    (*(u32 *)&dlg_input.help_pressed) = dialog2.result_extra;
    if (((byte)dialog2.style_flags & 4) != 0) {
      dlg_input.check_bits = 0;
      for (i = 0; i < (int)dialog2.option_count; i++) {
        status = dialog2.option_is_checked(i);
        if (status != 0) {
          dlg_input.check_bits = dlg_input.check_bits | 1 << ((byte)i & 0x1f);
        }
      }
    }
  }
  return result;
}

/* 0051d564 auto */
i32 __cdecl
popup_text_with_sprite(iptr file,const char *section,i32 flags,iptr sprite,i32 zoom)

{
  i32 result;
  Dialog dialog2;
  i32 eh_prev;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  dialog2.load_text((const char *)file,section,flags);
  dialog2.add_sprite(sprite,0,0);
  dialog2.set_image_zoom(zoom);
  result = dialog2.show_modal(0);
  (*(u32 *)&dlg_input.help_pressed) = dialog2.result_extra;
  return result;
}

/* 0051d63b auto */
i32 __cdecl
dlg_text_input(iptr file,const char *section,i32 max_len,iptr initial_text,iptr out_text)

{
  int status;
  i32 result;
  Dialog dialog2;
  char *p;
  i32 eh_prev;
  
  result = 0;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  str_clear((char *)out_text);
  status = dialog2.load_text_section((const char *)file,section,max_len,initial_text,0,0,0,1);
  if (status == 0) {
    result = dialog2.run_modal(out_text,0);
    for (p = (char *)out_text;
        (*p != '\0' && ((*p == ' ' || (*p == '\t')))); p++)
    {
      *p = '_';
    }
  }
  return result;
}

/* 0051d75d auto */
i32 __cdecl dlg_input_number(iptr file,const char *section,int initial,iptr out_value)

{
  long value;
  char input_text[260];
  char initial_text[20];
  i32 result;
  
  __itoa(initial,initial_text,10);
  result = dlg_text_input(file,section,5,(iptr)initial_text,(iptr)input_text);
  value = _atol(input_text);
  *(long *)out_value = value;
  return result;
}

/* 0051d7bc auto */
void __cdecl dlg_checks_clear(void)

{
  (*(i32 *)&dlg_input.check_bits) = 0;
  return;
}

/* 0051d7d6 auto */
void __cdecl dlg_check_set(i32 index,int checked)

{
  if (checked == 0) {
    dlg_input.check_bits = dlg_input.check_bits & ~(1 << ((byte)index & 0x1f));
  }
  else {
    dlg_input.check_bits = dlg_input.check_bits | 1 << ((byte)index & 0x1f);
  }
  return;
}

/* 0051d817 auto */
uint __cdecl dlg_check_get(i32 index)

{
  return 1 << ((byte)index & 0x1f) & dlg_input.check_bits;
}

/* 0052182c auto */
void __cdecl dialog_prepend_title_label(iptr dialog,i32 label_id)

{
  size_t len;
  i32 buffer;
  char title[128];
  
  title[0] = '\0';
  str_append_label((iptr)title,label_id);
  if (*(int *)(dialog + 0x134) != 0) {
    crt_strcat(title,((i8 *)&dialog_title_label_space_str));
    crt_strcat((char *)((void *)title),(const char *)((void *)(*(i32 *)(dialog + 0x134))));
  }
  len = _strlen(title);
  buffer = memblock_alloc((MemBlock *)(dialog + 0x254),len + 1);
  *(i32 *)(dialog + 0x134) = buffer;
  crt_strcpy((char *)((void *)(*(i32 *)(dialog + 0x134))),(const char *)((void *)title));
  return;
}

/* 00548b70 auto */
int __cdecl ui_calc_item_spacing(int count,int item_size,int total_size,iptr out_count,iptr out_remainder)

{
  int free_space;
  int spacing;
  int result;
  
  if (item_size < 2) {
    item_size = 1;
  }
  if (count < 2) {
    count = 1;
  }
  result = item_size;
  if (count == 1) {
    if (out_remainder != 0) {
      *(i32 *)out_remainder = 0;
    }
    if (out_count != 0) {
      *(i32 *)out_count = 1;
    }
  }
  else {
    free_space = total_size - item_size;
    spacing = free_space / (count - 1);
    if (out_remainder != 0) {
      *(int *)out_remainder = free_space % (count - 1);
    }
    if (out_count != 0) {
      *(int *)out_count = count;
    }
    if ((spacing < item_size) && (result = spacing, spacing < 1)) {
      if (out_remainder != 0) {
        *(i32 *)out_remainder = 0;
      }
      if (out_count != 0) {
        *(int *)out_count = (free_space - item_size) + 1;
      }
      result = 1;
    }
  }
  return result;
}

/* 00551e20 auto */
i32 __cdecl popup_game_ex(const char *section,i32 arg2,i32 arg3)

{
  i32 result;
  
  result = popup_text_ex((const char *)&str_game_7,section,arg2,arg3);
  return result;
}

/* 00551e60 auto */
i32 __cdecl popup_debug_ex(const char *section,i32 arg2,i32 arg3)

{
  i32 result;
  
  result = popup_text_ex((const char *)&str_debug_11,section,arg2,arg3);
  return result;
}

/* 00551ea0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl caption_font_global_init(void)

{
  caption_font_static_init();
  caption_font_reg_atexit();
  return;
}

/* 00551eba auto */
void __cdecl caption_font_static_init(void)

{
  ((GameFont *)((i8 *)&caption_font_block.caption_font))->init(0,0x10);
  return;
}

/* 00551ed8 auto */
void __cdecl caption_font_reg_atexit(void)

{
  _atexit(caption_font_static_dtor);
  return;
}

/* 00551ef5 auto */
void __cdecl caption_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&caption_font_block.caption_font))->destroy();
  return;
}

/* 00551f0f auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl caption_small_font_global_init(void)

{
  caption_small_font_static_init();
  caption_small_font_reg_atexit();
  return;
}

/* 00551f29 auto */
void __cdecl caption_small_font_static_init(void)

{
  ((GameFont *)((i8 *)&caption_small_font_block.caption_small_font))->init(0,10);
  return;
}

/* 00551f47 auto */
void __cdecl caption_small_font_reg_atexit(void)

{
  _atexit(caption_small_font_static_dtor);
  return;
}

/* 00551f64 auto */
void __cdecl caption_small_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&caption_small_font_block.caption_small_font))->destroy();
  return;
}

/* 00551f7e auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl ui_text_font_global_init(void)

{
  ui_text_font_static_init();
  ui_text_font_reg_atexit();
  return;
}

/* 00551f98 auto */
void __cdecl ui_text_font_static_init(void)

{
  ((GameFont *)((i8 *)&ui_text_font))->init(0,0x10);
  return;
}

/* 00551fb6 auto */
void __cdecl ui_text_font_reg_atexit(void)

{
  _atexit(ui_text_font_static_dtor);
  return;
}

/* 00551fd3 auto */
void __cdecl ui_text_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&ui_text_font))->destroy();
  return;
}

/* 00551fed auto */
void __cdecl ui_init_screen_metrics(void)

{
  int metric;
  
  gfx_get_screen_rect((iptr)&caption_small_font_block.screen_rect);
  caption_font_block.screen_width = rect_width((RECT *)&caption_small_font_block.screen_rect);
  caption_font_block.screen_height = rect_height((RECT *)&caption_small_font_block.screen_rect);
  ui_metrics.font_height = 0x10;
  if (999 < caption_font_block.screen_width) {
    ui_metrics.font_height = 0x18;
  }
  (*(uint *)&ui_metrics.large_screen) = (uint)(999 < caption_font_block.screen_width);
  ((GameFont *)((i8 *)&caption_font_block.caption_font))->set(0,ui_metrics.font_height);
  ((GameFont *)((i8 *)&caption_small_font_block.caption_small_font))->set(0,(ui_metrics.font_height * 2) / 3);
  ((GameFont *)((i8 *)&ui_text_font))->set(0,ui_metrics.font_height);
  metric = ((GameFont *)((i8 *)&caption_font_block.caption_font))->font_cached_height();
  ui_metrics.caption_height = metric + ui_metrics.padding * 2 + ui_metrics.bevel_width * 2;
  ui_metrics.border_width = ui_metrics.bevel_width * 2 + ui_metrics.padding * 2;
  metric = GetSystemMetrics(7);
  ui_metrics.dlgframe_w2 = metric * 2;
  metric = GetSystemMetrics(8);
  ui_metrics.dlgframe_h2 = metric * 2;
  return;
}

/* 005520fa auto */
void __cdecl frame_set_border_tile(iptr tile)

{
  ui_metrics.border_tile = tile;
  return;
}

/* Part of FrameWindow::draw_caption (00552112): the border tiles, the outer and inner bevels
   and the caption buttons; returns the x after the buttons. */
int FrameWindow::draw_frame_borders(int bevel,i32 highlight_color2,i32 shadow_color2)
{
  int button_x;
  u32 caption_h2;
  u32 caption_h;
  i8 sprite_rect[16];
  int height2;
  int width2;
  int button_y2;
  int offset;
  int i;
  struct { u32 left; u32 top; u32 right; u32 bottom; } rect2;

  if (caption_height == 0) {
    caption_h = border_width;
  }
  else {
    caption_h = caption_height;
  }
  height2 = caption_h + ui_metrics.bevel_width * -2;
  width2 = rect_width((RECT *)&frame_rect);
  gfx_tile_fill((iptr)this,ui_metrics.border_tile,0,bevel,width2,height2,0,bevel);
  height2 = border_width + bevel * -2;
  offset = rect_height((RECT *)&frame_rect);
  offset -= (bevel + height2);
  gfx_tile_fill((iptr)this,ui_metrics.border_tile,0,offset,width2,height2,0,offset);
  width2 = border_width + bevel * -2;
  height2 = rect_height((RECT *)&frame_rect);
  gfx_tile_fill((iptr)this,ui_metrics.border_tile,bevel,0,width2,height2,bevel,0);
  offset = rect_width((RECT *)&frame_rect);
  offset -= (bevel + width2);
  gfx_tile_fill((iptr)this,ui_metrics.border_tile,offset,0,width2,height2,offset,0);
  rect2.left = frame_rect;
  rect2.top = frame_top_u;
  rect2.right = frame_right_u;
  rect2.bottom = frame_bottom;
  for (i = 0; i < bevel; i++) {
    gfx_draw_bevel_rect((iptr)this,(RECT *)&rect2,highlight_color2,shadow_color2);
    rect_deflate((iptr)&rect2,1,1);
  }
  rect_deflate((iptr)&rect2,border_width + bevel * -2,0);
  if (caption_height == 0) {
    caption_h2 = border_width;
  }
  else {
    caption_h2 = caption_height;
  }
  rect2.top = rect2.top + caption_h2 + bevel * -2;
  rect2.bottom -= (border_width + bevel * -2);
  for (i = 0; i < bevel; i++) {
    gfx_draw_bevel_rect((iptr)this,(RECT *)&rect2,shadow_color2,highlight_color2);
    rect_deflate((iptr)&rect2,1,1);
  }
  button_x = ui_metrics.border_width;
  for (i = 0; i < (int)caption_button_count; i++) {
    width2 = (int)(*(int *)(caption_button_area + i * 0x1c + 0xc) * caption_height) / 0x18;
    height2 = (int)(*(int *)(caption_button_area + i * 0x1c + 0x10) * caption_height) / 0x18;
    button_y2 = ((int)caption_height >> 1) - (height2 >> 1);
    rect_set_xywh((iptr)&rect2,button_x,button_y2,width2,height2);
    gfx_set_scale_ratio(caption_height,0x18);
    (*(Sprite **)(caption_button_area + i * 0x1c + 0x14))->draw((iptr)sprite_rect,
               (iptr)this,button_x,button_y2);
    gfx_reset_zoom_scale();
    button_x = button_x + width2 + ui_metrics.padding;
  }
  return button_x;
}

/* Part of FrameWindow::draw_caption (00552112): the turn timer text (m:ss) of a multiplayer
   turn, or the autoplay turns left. */
void FrameWindow::update_timer_text()
{
  int minutes;
  int seconds;

  if ((autoplay.active == 0) || (show_turn_status == 0)) {
    if ((((uint)game.human_players & 1 << (game.turn_civ & 0x1fU)) == 0) || (show_turn_timer == 0))
    {
      timer_text = '\0';
    }
    else if (timer_last_value != (*(u32 *)&mp_timer.seconds_left)) {
      if (timer_last_value == 0xffffffff) {
        timer_last_value = civ_saved_cursor_x_block.mp_turn_time_limit_ms / 1000;
      }
      else {
        timer_last_value = (*(u32 *)&mp_timer.seconds_left);
      }
      if ((int)timer_last_value < 0) {
        timer_last_value = 0;
      }
      minutes = (int)(timer_last_value - 1) / 0x3c;
      seconds = (timer_last_value - 1) + minutes * -0x3c;
      if (minutes < 10) {
        timer_text = '0';
        __itoa(minutes,(char *)timer_minutes_text,10);
      }
      else {
        __itoa(minutes,(char *)&timer_text,10);
      }
      timer_colon = ':';
      if (seconds < 10) {
        if (seconds < 0) {
          seconds = 0;
        }
        timer_seconds_text = '0';
        __itoa(seconds,(char *)timer_seconds_digits,10);
      }
      else {
        __itoa(seconds,(char *)&timer_seconds_text,10);
      }
    }
  }
  else if ((int)autoplay.turns_left != timer_last_value) {
    timer_last_value = (int)autoplay.turns_left;
    __itoa((int)autoplay.turns_left,(char *)&timer_text,10);
  }
}

/* Part of FrameWindow::draw_caption (00552112): in multiplayer, appends whose turn it is (and
   the timer) to the caption. */
void FrameWindow::append_turn_status(char *caption)
{
  i32 adjective;

  if (((show_turn_status == 0) || (game.multiplayer == '\0')) || (net.mp_simultaneous_moves != '\0')) {
    if (((show_turn_status != 0) && (game.multiplayer != '\0')) && (net.mp_simultaneous_moves != '\0')) {
      if ((1 << ((*(byte *)&net.mp_turn_civ) & 0x1f) & (uint)game.human_players) == 0) {
        crt_strcat(caption,((i8 *)&caption_simul_open_paren_str));
        if (game_language == 1) {
          crt_strcat(caption,((i8 *)&caption_simul_ai_fr_str));
        }
        else {
          crt_strcat(caption,((i8 *)&caption_simul_ai_str));
        }
        crt_strcat(caption,((i8 *)&caption_simul_space_str));
        str_append_label((iptr)caption,0x369);
        if (timer_text != '\0') {
          crt_strcat(caption,((i8 *)&caption_simul_timer_sep_str));
          crt_strcat(caption,(const char *)(&timer_text));
        }
        crt_strcat(caption,((i8 *)&caption_simul_close_paren_str));
      }
      else if (timer_text != '\0') {
        crt_strcat(caption,((i8 *)&caption_timer_open_paren_str));
        crt_strcat(caption,(const char *)(&timer_text));
        crt_strcat(caption,((i8 *)&caption_timer_close_paren_str));
      }
    }
  }
  else {
    crt_strcat(caption,((i8 *)&caption_status_open_paren_str));
    if ((((*(byte *)&civs[human_civ[0]].treaties[net.mp_turn_civ] & TREATY_CONTACT) == 0) &&
        (net.mp_turn_civ != human_civ[0])) &&
       ((game.multiplayer != 1 || ((1 << ((*(byte *)&net.mp_turn_civ) & 0x1f) & (uint)net.hotseat_human_players) == 0)))) {
      if ((1 << ((*(byte *)&net.mp_turn_civ) & 0x1f) & (uint)game.human_players) == 0) {
        if (game_language == 1) {
          crt_strcat(caption,((i8 *)&caption_ai_fr_str));
        }
        else {
          crt_strcat(caption,((i8 *)&caption_ai_str));
        }
        if (autoplay.active == 0) {
          timer_text = '\0';
        }
      }
      else if ((uint)net.net_civ_slots[net.mp_turn_civ] < 7) {   /* (BF-203) -1: its player left */
        crt_strcat(caption,((i8 *)&net.slot_player_names) + net.net_civ_slots[net.mp_turn_civ] * 0x54);
      }
    }
    else {
      adjective = civ_adjective(net.mp_turn_civ);
      crt_strcat((char *)((void *)caption),(const char *)((void *)adjective));
      if (((1 << ((*(byte *)&net.mp_turn_civ) & 0x1f) & (uint)game.human_players) == 0) && (autoplay.active == 0)) {
        timer_text = '\0';
      }
    }
    crt_strcat(caption,((i8 *)&caption_moves_space_str));
    str_append_label((iptr)caption,0x369);
    if (timer_text != '\0') {
      crt_strcat(caption,((i8 *)&caption_timer_sep_str));
      crt_strcat(caption,(const char *)(&timer_text));
    }
    crt_strcat(caption,((i8 *)&caption_close_paren_str));
  }
}

/* 00552112 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FrameWindow::draw_caption()

{
  int title_height;
  int small_height;
  uptr len;
  uptr len2;
  int bevel;
  i32 shadow_color2;
  int text_width;
  int width2;
  int button_x;
  GameFont *font2;
  int text_y2;
  i32 highlight_color2;
  int text_x2;
  char caption[256];
  
  reset_clip();
  highlight_color2 = ui_metrics.highlight_color;
  shadow_color2 = ui_metrics.shadow_color;
  bevel = ui_metrics.bevel_width;
  button_x = draw_frame_borders(bevel,highlight_color2,shadow_color2);
  update_timer_text();
  crt_strcpy(caption,(const char *)frame_title);
  append_turn_status(caption);
  width2 = rect_width((RECT *)&frame_rect);
  text_width = ((GameFont *)title_font)->text_width((const char *)caption);
  text_width += 4;
  text_x2 = (width2 >> 1) - (text_width >> 1);
  text_y2 = bevel + 1;
  font2 = (GameFont *)title_font;
  if (text_x2 < button_x) {
    text_x2 = button_x;
  }
  if (width2 < text_width + text_x2) {
    font2 = (GameFont *)title_small_font;
    text_width = font2->text_width((const char *)caption);
    text_width += 4;
    text_x2 = (width2 >> 1) - (text_width >> 1);
    if (text_x2 < button_x) {
      text_x2 = button_x;
    }
    title_height = ((GameFont *)title_font)->font_cached_height();
    small_height = font2->font_cached_height();
    text_y2 += (title_height - small_height >> 1);
    while ((len = _strlen(caption), len != 0 &&
           (title_height = font2->text_width((const char *)caption),
           width2 < text_x2 + title_height + 4))) {
      len2 = _strlen(caption);
      caption[len2 - 1] = '\0';
    }
  }
  gfx_set_text_color_index(10);
  draw_text((iptr)font2,(const char *)caption,text_x2 + 2,text_y2 + 1,5);
  gfx_set_text_color_index(0x1a);
  draw_text((iptr)font2,(const char *)caption,text_x2 + 1,text_y2,5);
  draw_text((iptr)font2,(const char *)caption,text_x2,text_y2,5);
  set_clip((RECT *)&client_rect);
  return;
}

/* 00552e5b auto */
void __cdecl frame_wnd_on_caption_button_click(int button_id)

{
  iptr child;
  iptr frame_wnd;
  
  child = wnd_get_active_child();
  if (child == 0) {
    frame_wnd = 0;
  }
  else {
    frame_wnd = child - 0x48;
  }
  if (((FrameWindow *)frame_wnd)->on_caption_button != 0) {
    ((FrameWindow *)frame_wnd)->on_caption_button(*(i32 *)(frame_wnd + 0x200 + (button_id - 1000) * 0x1c));
  }
  return;
}

/* 00552ed2 auto */
void FrameWindow::layout()

{
  int width;
  int height;
  u32 size;
  Hotspot *this_00;
  u8 *native_wnd2;
  Hotspot *hotspot;
  int button_x;
  int i;
  i8 button_rect[16];
  i32 eh_prev;
  
  width = get_width();
  height = get_height();
  rect_set_xywh((iptr)&frame_rect,0,0,width,height);
  rect_set_xywh((iptr)&client_rect,0,0,width + border_width * -2,
                     height - (caption_height + border_width));
  rect_offset((iptr)&client_rect,border_width,caption_height);
  client_x = client_rect;
  client_y = client_top_u;
  size = rect_width((RECT *)&client_rect);
  client_width = size;
  size = rect_height((RECT *)&client_rect);
  client_height = size;
  destroy_caption_buttons();
  button_x = ui_metrics.border_width;
  for (i = 0; i < (int)caption_button_count; i++) {
    width = (int)(*(int *)(caption_button_area + i * 0x1c + 0xc) * caption_height) / 0x18;
    height = (int)(*(int *)(caption_button_area + i * 0x1c + 0x10) * caption_height) / 0x18;
    rect_set_xywh((iptr)button_rect,button_x,((int)caption_height >> 1) - (height >> 1),width,
                       height);
    this_00 = (Hotspot *)operator_new(0x40);
    if (this_00 == (Hotspot *)0x0) {
      hotspot = 0;
    }
    else {
      hotspot = hotspot_ctor(this_00);
    }
    *(i32 *)(caption_button_area + i * 0x1c + 0x18) = (i32)hotspot;
    if (this == (FrameWindow *)0x0) {
      native_wnd2 = (u8 *)0x0;
    }
    else {
      native_wnd2 = (u8 *)&window;
    }
    (*(Hotspot **)(caption_button_area + i * 0x1c + 0x18))->create((iptr)native_wnd2,
               i + 1000,(iptr)button_rect);
    (*(Hotspot **)(caption_button_area + i * 0x1c + 0x18))->set_click_callback(frame_wnd_on_caption_button_click);
    button_x = button_x + width + ui_metrics.padding;
  }
  return;
}

/* 0055318c auto */
void FrameWindow::add_caption_button(iptr sprite,i32 callback)

{
  u8 *slot;
  u32 index;
  i32 dim;
  
  if ((int)caption_button_count < 6) {
    index = caption_button_count;
    caption_button_count++;
    *(iptr *)(close_caption_sprite_slot + index * 0x1c - 0x3c) = sprite;
    *(i32 *)(caption_button_area + index * 0x1c) = callback;
    dim = ((Sprite *)sprite)->width();
    *(i32 *)(close_caption_sprite_slot + index * 0x1c - 0x44) = dim;
    dim = ((Sprite *)sprite)->height();
    *(i32 *)(close_caption_sprite_slot + index * 0x1c - 0x40) = dim;
    slot = close_caption_sprite_slot + index * 0x1c - 0x38;
    slot[0] = '\0';
    slot[1] = '\0';
    slot[2] = '\0';
    slot[3] = '\0';
  }
  return;
}

/* 0055324c auto */
void FrameWindow::set_caption(const char *title)

{
  struct { u32 left; u32 top; u32 right; u32 bottom; } rect2;
  
  _memset(frame_title,0,0x84);
  _strncpy((char *)frame_title,(char *)title,0x83);
  draw_caption();
  rect2.left = frame_rect;
  rect2.top = frame_top_u;
  rect2.right = frame_right_u;
  rect2.bottom = client_top_u;
  wnd_blit_surface_rect((iptr)&rect2);
  return;
}

/* 005532d7 auto */
void FrameWindow::destroy_caption_buttons()

{
  int i;
  
  for (i = 0; i < 6; i++) {
    if (((u32 *)&caption_button_slots)[i * 7] != 0) {
      if ((Hotspot *)((u32 *)&caption_button_slots)[i * 7] != (Hotspot *)0x0) {
        ((Hotspot *)((u32 *)&caption_button_slots)[i * 7])->scalar_destruct(1);
      }
      ((u32 *)&caption_button_slots)[i * 7] = 0;
    }
  }
  return;
}

/* 00553379 auto */
void FrameWindow::close()

{
  destroy_caption_buttons();
  wnd_destroy_with_surface();
  return;
}

/* 0055339f auto */
FrameWindow *FrameWindow::construct()

{
  u8 *slot;
  int i;
  i32 eh_prev;
  
  SurfaceWindow::construct();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 7);
  for (i = 0; i < 6; i++) {
    slot = (u8 *)&caption_button_slots + i * 0x1c;
    slot[0] = '\0';
    slot[1] = '\0';
    slot[2] = '\0';
    slot[3] = '\0';
  }
  return this;
}

/* 00553444 auto */
/* Library Function - Single Match
    public: virtual __thiscall COleCntrFrameWnd::~COleCntrFrameWnd(void)
   
   Library: Visual Studio 1998 Debug */

void FrameWindow::destruct()

{
  i32 eh_prev;
  i32 eh_state;
  
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 7);
  eh_state = 0;
  destroy_caption_buttons();
  wnd_destroy_with_surface();
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0055349b */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return;
}

/* 005534bc auto */
void FrameWindow::create(const char *title,uint flags,int x,int y,int width, int height,u32 resize_width,u32 caption_h,u32 border_w)

{
  uint wnd_style;
  
  if (title == 0) {
    crt_strcpy((char *)frame_title,((i8 *)&frame_default_title_str));
  }
  else {
    crt_strcpy((char *)((void *)frame_title),(const char *)((void *)title));
  }
  *(i8 **)text_font = ((i8 *)&ui_text_font);
  title_font = (uptr)((i8 *)&caption_font_block.caption_font);
  title_small_font = (uptr)((i8 *)&caption_small_font_block.caption_small_font);
  show_turn_timer = 0;
  show_turn_status = 0;
  style_flags = flags;
  caption_height = caption_h;
  border_width = border_w;
  resize_border = resize_width;
  caption_button_count = 0;
  on_caption_button = 0;
  if ((flags & 4) != 0) {
    caption_height = (*(u32 *)&ui_metrics.caption_height);
    border_width = (*(u32 *)&ui_metrics.border_width);
  }
  if ((flags & 8) == 0) {
    wnd_style = 0x202;
  }
  else {
    wnd_style = 0x802;
  }
  if (caption_height != 0) {
    wnd_style = wnd_style | 0x400;
  }
  if (resize_border != 0) {
    wnd_style = wnd_style | 0x1000;
  }
  if ((flags & 2) == 0) {
    width += border_width * 2;
    height = height + caption_height + border_width;
  }
  if ((flags & 1) != 0) {
    x = (caption_font_block.screen_width >> 1) - (width >> 1);
    y = (caption_font_block.screen_height >> 1) - (height >> 1);
  }
  wnd_create_with_surface(0,wnd_style,x,y,width,height,(iptr)&game_palette_block.game_palette,(iptr)&main_frame_wnd_block.main_frame_wnd);
  if (caption_height != 0) {
    ((WndBase *)&window)->set_caption_height(caption_height);
  }
  if (resize_border != 0) {
    ((WndBase *)&window)->set_resize_border(resize_border);
  }
  layout();
  window.wnd_handlers.set_drag_begin_callback(mp_window_drag_begin);
  window.wnd_handlers.set_drag_end_callback(mp_window_drag_end);
  return;
}

/* 00553d30 auto */
WndNotifyFn WndHandlers::set_drag_begin_callback(WndNotifyFn callback)

{
  WndNotifyFn old_callback;
  
  old_callback = on_enter_size_move;
  on_enter_size_move = callback;
  return old_callback;
}

/* 00553d70 auto */
WndNotifyFn WndHandlers::set_drag_end_callback(WndNotifyFn callback)

{
  WndNotifyFn old_callback;
  
  old_callback = on_exitsizemove;
  on_exitsizemove = callback;
  return old_callback;
}

/* 0056810b auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl frame_texture_surface_global_init(void)

{
  frame_texture_surface_static_init();
  frame_texture_surface_reg_atexit();
  return;
}

/* 00568125 auto */
void __cdecl frame_texture_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&frame_texture_surface_block.frame_texture_surface));
  return;
}

/* 0056813f auto */
void __cdecl frame_texture_surface_reg_atexit(void)

{
  _atexit(frame_texture_surface_static_dtor);
  return;
}

/* 0056815c auto */
void __cdecl frame_texture_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&frame_texture_surface_block.frame_texture_surface));
  return;
}

/* 005681c9 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl frame_texture_load(void)

{
  int ok;
  i8 sprite_rect[16];
  DllHandle dll;
  DibSurface surface;
  i8 path[128];
  Sprite sprite;
  i32 eh_prev;
  
  gfx_build_grey_remap();
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  frame_texture_loaded_i32 = 1;
  str_clear((char *)path);
  str_append((char *)path,(const char *)&str_civ2_civ2art_dll_4);
  ok = file_resolve_path((const char *)path);
  if (ok == 0) {
    ((DibSurface *)((i8 *)&frame_texture_surface_block.frame_texture_surface))->DibSurface::create(0x40,0x20);
    ((DibSurface *)((i8 *)&frame_texture_surface_block.frame_texture_surface))->fill(0x21);
  }
  else {
    dll.resource_dll_load((iptr)path);
    ON_SCOPE_EXIT(dll.free());
    ok = surface.load_gif_resource(999,10,0xec,(iptr)&art_palette);
    if (ok == 0) {
      MessageBoxA((HWND)0x0,str_failed_to_load_civ2art_gif_2,str_notice_15,0x40);
    }
    else {
      sprite.grab_from_surface((iptr)&surface,2,1,1,0x40,0x20);
      ((DibSurface *)((i8 *)&frame_texture_surface_block.frame_texture_surface))->DibSurface::create(0x40,0x20);
      sprite.draw((iptr)sprite_rect,(iptr)&frame_texture_surface_block.frame_texture_surface,0,0);
      sprite.free();
      surface.release();
    }
    dll.dll_handle_free();
  }
  frame_texture_load_hook_nop();
  return;
}

/* 00568381 auto */
void __cdecl frame_texture_free(void)

{
  ((DibSurface *)((i8 *)&frame_texture_surface_block.frame_texture_surface))->release();
  gfx_free_grey_remap();
  frame_texture_free_hook_nop();
  return;
}

/* 005683a5 auto */
void __cdecl frame_texture_load_hook_nop(void)

{
  return;
}

/* 005683b5 auto */
void __cdecl frame_texture_free_hook_nop(void)

{
  return;
}

/* 005785d0 auto */
/* Library Function - Single Match
    public: int __thiscall ios_base::precision(int)
   
   Library: Visual Studio 1998 Debug */

WndMouseFn WndHandlers::set_rbuttondown_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = (this)->on_rbutton_down;
  (this)->on_rbutton_down = handler;
  return old_handler;
}

/* 00578610 auto */
/* Library Function - Single Match
    public: int __thiscall ios_base::width(int)
   
   Library: Visual Studio 1998 Debug */

WndMouseFn WndHandlers::set_rbuttonup_any_handler(WndMouseFn handler)

{
  WndMouseFn old_handler;
  
  old_handler = (this)->on_rbutton_up_raw;
  (this)->on_rbutton_up_raw = handler;
  return old_handler;
}

/* 00578650 auto */
void GameMenu::storage_reinit(uint size)

{
  memblock_free((MemBlock *)item_storage);
  memblock_init((MemBlock *)item_storage,0xb,size);
  return;
}

/* 0057868c auto */
/* Library Function - Single Match
    public: virtual void __thiscall CHtmlStream::Reset(void)
   
   Library: Visual Studio 1998 Debug */

void GameMenu::clear_lists()

{
  menus = 0;
  menu_flags = 0;
  return;
}

/* 005786b6 auto */
void GameMenu::reset(i32 size)

{
  memblock_detach((MemBlock *)item_storage);
  clear_lists();
  storage_reinit(size);
  return;
}

/* 005786f1 auto */
GameMenu *GameMenu::construct(i32 size)

{
  i32 eh_prev;
  
  handle_construct();
  ((MemBlock *)item_storage)->construct();
  reset(size);
  return this;
}

/* 00578770 auto */
void GameMenu::destruct()

{
  i32 eh_prev;
  i32 eh_state;
  
  eh_state = 0;
  memblock_free((MemBlock *)item_storage);
  eh_state = 0xffffffff;
  /* inlined unwind funclet 005787bd */ { handle_destruct(); }
  return;
}

/* 005787de auto */
MenuNode *GameMenu::find_menu(int menu_id)

{
  MenuNode *menu2;
  
  menu2 = menus;
  while( true ) {
    if (menu2 == 0) {
      return 0;
    }
    if (menu2->id == menu_id) break;
    menu2 = menu2->next;
  }
  return menu2;
}

/* 00578840 auto */
int GameMenu::visible_index(MenuNode *menu)

{
  int index;
  MenuNode *cur;
  
  index = 1;
  for (cur = menus; (cur != 0 && ((uptr)menu != (uptr)cur));
      cur = cur->next) {
    if ((cur->flags_byte & 2) == 0) {
      index++;
    }
  }
  return index;
}

/* 005788a9 auto */
i32 GameMenu::id_at_visible_index(int visible_index)

{
  int index;
  MenuNode *menu2;
  
  index = 1;
  menu2 = menus;
  do {
    if (menu2 == 0) {
      return 0xffffffff;
    }
    if ((menu2->flags_byte & 2) == 0) {
      if (index == visible_index) {
        return menu2->id;
      }
      index++;
    }
    menu2 = menu2->next;
  } while( true );
}

/* 00578922 auto */
MenuNode *GameMenu::find_item(int item_id)

{
  MenuNode *menu2;
  MenuNode *item;
  MenuNode *found;
  
  found = 0;
  menu2 = menus;
  do {
    if (menu2 == 0) {
      return found;
    }
    for (item = menu2->link; item != 0; item = item->next) {
      if (item->id == item_id) {
        found = item;
        break;
      }
    }
    menu2 = menu2->next;
  } while( true );
}

/* 005789aa auto */
int GameMenu::item_visible_index(MenuNode *item)

{
  int index;
  iptr cur;
  
  index = 1;
  for (cur = *(iptr *)((int)item->link + 0x18); (cur != 0 && ((iptr)item != cur));
      cur = *(iptr *)(cur + 0x10)) {
    if ((*(byte *)(cur + 8) & 2) == 0) {
      index++;
    }
  }
  return index;
}

/* 00578a1c auto */
i32 GameMenu::item_id_at_visible_index(i32 menu_id,int visible_index)

{
  MenuNode *menu;
  int index;
  MenuNode *item;
  
  index = 1;
  menu = find_menu(menu_id);
  if (menu != 0) {
    for (item = menu->link; item != 0; item = item->next) {
      if ((item->flags_byte & 2) == 0) {
        if (index == visible_index) {
          return item->id;
        }
        index++;
      }
    }
  }
  return 0xffffffff;
}

/* 00578b06 auto */
MenuNode *GameMenu::add_menu(i32 menu_id,char *title)

{
  MenuNode *menu;
  size_t len;
  i32 text;
  MenuNode *last;
  
  menu = (MenuNode *)memblock_alloc((MemBlock *)item_storage,0x1c);
  if (menus == 0) {
    menus = menu;
    menu->prev = 0;
  }
  else {
    for (last = menus; (int)last->next != 0;
        last = last->next) {
    }
    last->next = menu;
    menu->prev = last;
  }
  menu->next = 0;
  menu->link = 0;
  menu->flags = 0;
  menu->id = menu_id;
  menu->field_0c = 0;
  len = _strlen(title);
  text = memblock_alloc((MemBlock *)item_storage,len + 1);
  menu->text = (char *)text;
  crt_strcpy((char *)((void *)menu->text),(const char *)((void *)title));
  str_pipes_to_tabs(menu->text);
  menu_flags = menu_flags & 0xffff7fff;
  return menu;
}

/* 00578c12 auto */
MenuNode *GameMenu::add_item(i32 menu_id,i32 item_id,const char *text,size_t min_len)

{
  MenuNode *menu;
  size_t len;
  i32 buffer;
  size_t buf_len;
  MenuNode *item;
  MenuNode *last;
  
  item = (MenuNode *)0x0;
  menu = find_menu(menu_id);
  if (menu != 0) {
    item = (MenuNode *)memblock_alloc((MemBlock *)item_storage,0x1c);
    if (menu->link == 0) {
      menu->link = item;
      item->prev = 0;
    }
    else {
      for (last = menu->link; (int)last->next != 0;
          last = last->next) {
      }
      last->next = item;
      item->prev = last;
    }
    item->link = menu;
    item->next = 0;
    item->flags = 0;
    item->id = item_id;
    item->field_0c = 0;
    if (text == 0) {
      item->text = 0;
    }
    else {
      len = _strlen((char *)text);
      if ((int)len < (int)min_len) {
        buf_len = min_len;
      }
      else {
        buf_len = _strlen((char *)text);
      }
      buffer = memblock_alloc((MemBlock *)item_storage,buf_len + 1);
      item->text = (char *)buffer;
      crt_strcpy((char *)((void *)item->text),(const char *)((void *)text));
      str_pipes_to_tabs(item->text);
    }
    menu_flags = menu_flags & 0xffff7fff;
  }
  return item;
}

/* 00578d8a auto */
void __cdecl menu_dispatch_command(i32 menu_index,i32 item_index)

{
  i32 menu_id;
  i32 item_id;
  
  if (menu_state.command_callback != 0) {
    menu_id = menu_state.active_object->id_at_visible_index(menu_index);
    item_id = menu_state.active_object->item_id_at_visible_index(menu_id,item_index);
    menu_state.command_callback(menu_id,item_id);
  }
  return;
}

/* 00578de8 auto */
void GameMenu::attach_to_window(ModalMenuWindow *window)

{
  if (window != (ModalMenuWindow *)0x0) {
    menu_state.owner_window = window;
    menu_state.active_object = this;
    window->attach_menu((iptr)this);
    window->set_menu_callback(menu_dispatch_command);
  }
  return;
}

/* 00578e38 auto */
void GameMenu::set_command_handler(MenuCommandFn handler)

{
  menu_state.active_object = this;
  menu_state.command_callback = handler;
  return;
}

/* 00578e60 auto */
i32 GameMenu::apply_item_enabled(i32 item_id)

{
  MenuNode *item;
  uint enabled;
  i32 result;
  i32 menu_index;
  
  item = find_item(item_id);
  if (item == 0) {
    result = 0;
  }
  else {
    enabled = ~item->flags & 1;
    result = item_visible_index(item);
    menu_index = visible_index(item->link);
    result = native_enable(menu_index,result,enabled);
  }
  return result;
}

/* 00578ec7 auto */
void GameMenu::apply_item_checked(i32 item_id)

{
  MenuNode *item;
  uint checked;
  i32 item_index;
  i32 menu_index;
  
  item = find_item(item_id);
  if (item != 0) {
    checked = item->flags & 4;
    item_index = item_visible_index(item);
    menu_index = visible_index(item->link);
    native_check(menu_index,item_index,checked);
  }
  return;
}

/* 00578f2c auto */
void GameMenu::rebuild(iptr window)

{
  i32 *menu2;
  int menu_index;
  int item_index;
  i32 *item;
  int i;
  i8 spec[256];
  
  if ((menu_flags & 0x8000) == 0) {
    spec[0] = 0;
    menu2 = (i32 *)menus;
    menu_index = 1;
    for (; menu2 != (i32 *)0x0; menu2 = (i32 *)menu2[4]) {
      if ((*(byte *)(menu2 + 2) & 2) == 0) {
        crt_strcat((char *)((void *)spec),(const char *)((void *)*menu2));
        crt_strcat(spec,((i8 *)&menu_title_sep_str));
      }
      if (window == 0) {
        for (i = 0x1e; 0 < i; i--) {
          native_delete(menu_index,i);
        }
        menu2[3] = 0;
      }
      menu_index++;
    }
    if (window != 0) {
      native_build((iptr)spec);
    }
    menu2 = (i32 *)menus;
    menu_index = 1;
    for (; menu2 != (i32 *)0x0; menu2 = *(i32 **)((int)menu2 + 0x10)) {
      if ((*(byte *)((int)menu2 + 8) & 2) == 0) {
        if (window != 0) {
          native_delete(menu_index,1);
        }
        item = *(i32 **)((int)menu2 + 0x18);
        item_index = 1;
        for (; item != (i32 *)0x0; item = (i32 *)item[4]) {
          if ((*(byte *)(item + 2) & 2) == 0) {
            *(int *)((int)menu2 + 0xcU) = *(int *)((int)menu2 + 0xcU) + 1;
            native_insert(menu_index,item_index,(const char *)(*item));
            if ((*(byte *)(item + 2) & 1) != 0) {
              apply_item_enabled(item[1]);
            }
            if ((*(byte *)(item + 2) & 4) != 0) {
              apply_item_checked(item[1]);
            }
            item_index++;
          }
        }
        menu_index++;
      }
    }
    menu_flags = menu_flags | 0x8000;
    attach_to_window((ModalMenuWindow *)window);
  }
  if (menu_state.owner_window != (ModalMenuWindow *)0x0) {
    menu_state.owner_window->redraw_menu_bar();
  }
  return;
}

/* 00579260 auto */
void GameMenu::set_item_hidden(i32 item_id,int hidden)

{
  MenuNode *item;
  
  item = find_item(item_id);
  if (item != 0) {
    if (hidden == 0) {
      if ((item->flags_byte & 2) != 0) {
        menu_flags = menu_flags & 0xffff7fff;
      }
      item->flags = item->flags & 0xfffffffd;
    }
    else {
      if ((item->flags_byte & 2) == 0) {
        menu_flags = menu_flags & 0xffff7fff;
      }
      item->flags = item->flags | 2;
    }
  }
  return;
}

/* 005792e1 auto */
GameMenu *GameMenu::set_item_enabled(i32 item_id,int enabled)

{
  GameMenu *item;
  GameMenu *ret_val;
  int was_enabled;
  
  item = (GameMenu *)find_item(item_id);
  ret_val = item;
  if (item != (GameMenu *)0x0) {
    enabled = enabled != 0;
    was_enabled = (item->item_storage[4] & 1) != 0;
    if (enabled == 0) {
      *(uint *)item->item_flags = *(uint *)(item->item_flags) & 0xfffffffe;
    }
    else {
      *(uint *)item->item_flags = *(uint *)item->item_flags | 1;
    }
    ret_val = this;
    if ((((menu_flags & 0x8000) != 0) && (ret_val = item, (item->item_storage[4] & 2) == 0)) &&
       (ret_val = (GameMenu *)enabled, was_enabled != enabled)) {
      ret_val = (GameMenu *)apply_item_enabled(item_id);
    }
  }
  return ret_val;
}

/* 005793a3 auto */
void GameMenu::remove_item(i32 item_id)

{
  MenuNode *item;
  i32 item_index;
  i32 menu_index;
  
  item = find_item(item_id);
  if (item != 0) {
    item_index = item_visible_index(item);
    menu_index = visible_index(item->link);
    native_delete(menu_index,item_index);
    *(i32 *)((int)item->prev + 0x10) = (i32)item->next;
  }
  return;
}

/* 0057940d auto */
void GameMenu::set_item_checked(i32 item_id,int checked)

{
  byte old_flags;
  MenuNode *item;
  
  item = find_item(item_id);
  if (item != 0) {
    old_flags = item->flags_byte;
    if (checked != 0) {
      item->flags = item->flags | 4;
    }
    else {
      item->flags = item->flags & 0xfffffffb;
    }
    if ((((menu_flags & 0x8000) != 0) && ((item->flags_byte & 2) == 0)) &&
       (((old_flags & 4) != 0) != (checked != 0))) {
      apply_item_checked(item_id);
    }
  }
  return;
}

/* 005794cf auto */
void GameMenu::set_menu_enabled(i32 menu_id,i32 enabled)

{
  MenuNode *menu;
  MenuNode *item;
  
  menu = find_menu(menu_id);
  if (menu != 0) {
    for (item = menu->link; item != 0; item = item->next) {
      set_item_enabled(item->id,enabled);
    }
  }
  return;
}

/* 0057953e auto */
void GameMenu::set_item_text(i32 item_id,const char *text)

{
  MenuNode *item;
  i32 item_index;
  i32 menu_index;
  i32 item_text;
  
  item = find_item(item_id);
  if (item != (MenuNode *)0x0) {
    crt_strcpy((char *)((void *)item->text),(const char *)((void *)text));
    str_pipes_to_tabs(item->text);
    if (((menu_flags & 0x8000) != 0) && ((item->flags_byte & 2) == 0)) {
      item_text = (i32)item->text;
      item_index = item_visible_index(item);
      menu_index = visible_index(item->link);
      native_set_text(menu_index,item_index,(const char *)item_text);
    }
  }
  return;
}

/* 005799c0 auto */
void GameMenu::native_build(iptr spec)

{
  u32 menu_handle;
  
  menu_handle = menu_hmenu_build_from_spec((char *)spec);
  hmenu = menu_handle;
  return;
}

/* 00579a00 auto */
void GameMenu::native_insert(i32 menu_index,i32 item_index,const char *text)

{
  menu_hmenu_insert_item(hmenu,menu_index,item_index,text);
  return;
}

/* 00579a40 auto */
void GameMenu::native_delete(i32 menu_index,i32 item_index)

{
  menu_hmenu_delete_item(hmenu,menu_index,item_index);
  return;
}

/* 00579a80 auto */
void GameMenu::native_set_text(i32 menu_index,i32 item_index,const char *text)

{
  menu_hmenu_set_item_text(hmenu,menu_index,item_index,text);
  return;
}

/* 00579ac0 auto */
i32 GameMenu::native_enable(i32 menu_index,i32 item_index,i32 enabled)

{
  i32 result;
  
  result = menu_hmenu_enable_item(hmenu,menu_index,item_index,enabled);
  return result;
}

/* 00579b00 auto */
void GameMenu::native_check(i32 menu_index,i32 item_index,i32 checked)

{
  menu_hmenu_check_item(hmenu,menu_index,item_index,checked);
  return;
}

/* 00579b40 auto */
void ModalMenuWindow::attach_menu(iptr menu_obj)

{
  i32 hmenu;
  NativeWindow *hwnd;
  
  hmenu = ((GameMenu *)menu_obj)->get_hmenu();
  hwnd = get_window();
  wnd_obj_attach_menu(hwnd,hmenu);
  menu = menu_obj;
  return;
}

/* 00579b90 auto */
u32 GameMenu::get_hmenu()

{
  return hmenu;
}

/* 00579bc0 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void ModalMenuWindow::set_menu_callback(MenuPosCommandFn callback)

{
  on_menu_pos_command = callback;
  return;
}

/* 00579bf0 auto */
void ModalMenuWindow::redraw_menu_bar()

{
  NativeWindow *hwnd;
  
  if (menu != 0) {
    hwnd = get_window();
    menu_redraw_bar(hwnd);
  }
  return;
}

/* 00589d50 auto */
i32 WndBase::frame_width()

{
  i32 width;
  
  width = wnd_obj_frame_width(native_wnd);
  return width;
}

/* 0058bd60 auto */
void __cdecl cmd_unit_skip_turn(void)

{
  unit_move_dir((int)game.active_unit,0xffffffff,3);
  return;
}

/* 0058bd84 auto */
void __cdecl cmd_end_player_turn(void)

{
  iptr unit;
  
  for (unit = 0; unit < game.num_units; unit++) {
    if ((units[unit].id != 0) && (units[unit].owner == human_civ[0])) {
      unit_exhaust_moves(unit);
    }
  }
  return;
}

/* 0058d442 auto */
void __cdecl ui_pick_unit_from_stack(void)

{
  int stack_count;
  iptr prompt;
  i32 dialog_flags;
  iptr unit;
  
  blink.unit_active = 0;
  if (moving_units_mode == 1) {
    unit = (iptr)game.active_unit;
    if (units[unit].id == 0) {
      blink.unit_active = 0;
      return;
    }
    if (units[unit].owner != human_civ[0]) {
      blink.unit_active = 0;
      return;
    }
    stack_count = unit_stack_sum(unit,2);
    if (stack_count == 1) {
      return;
    }
  }
  else {
    unit = tile_first_unit((int)map_cursor.x[0],(int)map_cursor.y[0]);
    if (unit < 0) {
      return;
    }
    if (units[unit].owner != human_civ[0]) {
      return;
    }
    if ((units[unit].owner != game.turn_civ) && (game.reveal_map == '\0')) {
      return;
    }
    stack_count = unit_stack_sum(unit,2);
  }
  /* a stack: choose the unit */
  if (stack_count != 1) {
    dialog_flags = 1;
    prompt = (iptr)label_string(label_table.string_ids[0x3e]);
    unit = unit_stack_select_dialog(unit,(const char *)prompt,dialog_flags);
    if (unit < 0) {
      return;
    }
  }
  game.active_unit = (i16)unit;
  units[unit].orders = ORDER_NONE;
  units[unit].flags = units[unit].flags & ~UNITF_AUTOMATED;
  moving_units_mode = 0;
  if (extra_rule(X_ROTATION)) {
    extras_rotation_activated(unit);  /* X19: this stack's units next, the others wait */
  }
  ui_focus_active_unit(0);
  if (units[unit].moves_used == '\0') {
    unit_reveal_surroundings(unit,1);
  }
  return;
}

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
void __cdecl dialog_set_frame_tile_surface(iptr surface)

{
  dlg_style.frame_tile_surface = surface;
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
Dialog *Dialog::construct(i32 size)

{
  ((MemBlock *)string_pool)->construct();
  pool_ptr = 0;
  pool_handle = pool_ptr;
  init_defaults();
  pool_size = (u16)size;
  return this;
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
    if ((SurfaceWindow *)surf_wnd != (SurfaceWindow *)0x0) {
      ((SurfaceWindow *)surf_wnd)->SurfaceWindow::scalar_destruct(1);
    }
    surf_wnd = 0;
    style_flags = style_flags | 0x10;
  }
  if (this == dialog_state.active) {
    if ((dlg_style.stack_depth < 1) || (0x10 < dlg_style.stack_depth)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_popupstackindex_0_popupstackindex_17,(iptr)&str_d_ss_franklinton_popup_1_cpp,0x19c);
    }
    dlg_style.stack_depth--;
    (*(iptr *)&net.dialog_stack_top) = (((i32 *)&dlg_style.stack))[dlg_style.stack_depth];
    dialog_state.active = (Dialog *)(*(iptr *)&net.dialog_stack_top);
    dialog_state.timeout_start = timer_ticks();
  }
  return;
}

/* 0059df8a auto */
i32 Dialog::destruct()

{
  i32 result;
  
  close();
  result = memblock_free((MemBlock *)string_pool);
  return result;
}

/* 0059dfb9 auto */
void Dialog::init(iptr surface_wnd,iptr background,RECT *rect,u32 flags)

{
  i32 size;
  
  if ((style_flags & 0x20) == 0) {
    memblock_init((MemBlock *)string_pool,9,CONCAT22((short)((uint)this >> 0x10),pool_size));
  }
  else {
    close();
    init_defaults();
    reset_string_pool((uint)pool_size);
  }
  set_font((iptr)dlg_style.default_text_font);
  set_title_font((iptr)(*(i8 * *)dlg_style.default_text_font_rest));
  set_list_font((iptr)(*(i8 * *)(dlg_style.default_text_font_rest + 4)));
  surf_wnd = surface_wnd;
  bg_surface = background;
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
void Dialog::set_font(iptr font)

{
  u32 row_height;
  
  body_font = font;
  row_height = dialog_option_row_height();
  option_row_height = row_height;
  return;
}

/* 0059e4a4 auto */
/* Library Function - Single Match
    public: void __thiscall CArchive::SetObjectSchema(unsigned int)
   
   Library: Visual Studio 1998 Debug */

void Dialog::set_title_font(iptr font)

{
  title_font = font;
  return;
}

/* 0059e4c5 auto */
void Dialog::set_list_font(iptr font)

{
  list_font = font;
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
void Dialog::set_option_disabled(iptr option_id,int disabled)

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
void Dialog::set_default_choice(iptr choice_id)

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
DialogSprite *Dialog::add_sprite(iptr sprite,iptr sprite_id,const char *label)

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
  entry->sprite = (Sprite *)sprite;
  entry_width = ((Sprite *)sprite)->width();
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
DialogOption *Dialog::add_option(const char *text,iptr option_id,uint data)

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
      *(uint **)((uint)option->next + 0x14) = (uint *)option;
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
DialogOption *Dialog::add_checkbox_option(const char *text,iptr option_id,int checked)

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
DialogEditField *Dialog::add_edit_field(const char *label,iptr initial_text,size_t max_len)

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
  if (0xfe < (int)max_len) {
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
    (&(this)->extra_button_labels)[extra_button_count] = buffer;
    crt_strcpy((char *)((void *)((&(this)->extra_button_labels)[extra_button_count])),(const char *)((void *)label));
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
int Dialog::draw_text_shadowed(iptr font,char *text,int x,int y,uint flags)

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
  text_width = ((GameFont *)font)->text_width((const char *)text);
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
  char word_buf[80];
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
        crt_strcat(word_buf,src);
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
        crt_strcat(line_buf,word_buf);
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
    rect_set_xywh((iptr)&pane_rect[pane].left,list_x_offset[pane] + list_x,
                       list_y,list_width[pane],list_height[pane]);
    list_client_rect[pane].left = pane_rect[pane].left;
    list_client_rect[pane].top = pane_rect[pane].top;
    list_client_rect[pane].right = pane_rect[pane].right;
    list_client_rect[pane].bottom = pane_rect[pane].bottom;
    if (((int)(list_rows[pane] * columns) < (int)option_count) ||
       ((style_flags & 0x1000000) != 0)) {
      if (columns == 1) {
        rect_set_xywh((iptr)&list_vscroll_rect[pane],pane_rect[pane].right - list_vscroll_width[pane],
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
        rect_set_xywh((iptr)&list_hscroll_rect[pane],list_x_offset[pane] + list_x,
                           pane_rect[pane].bottom - list_hscroll_height[pane],list_width[pane],list_hscroll_height[pane])
        ;
        list_client_rect[pane].bottom -= list_hscroll_height[pane];
      }
    }
    rect_deflate((iptr)&list_client_rect[pane].left,1,1);
    list_origin_x[pane] = list_client_rect[pane].left;
    list_origin_y[pane] = list_client_rect[pane].top;
    value = rect_width((RECT *)&list_client_rect[pane].left);
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
    value = button_width_for((const char *)((&(*&(this)->extra_button_labels))[i]));
    if ((int)button_width <= value) {
      tmp = button_width_for((const char *)((&(*&(this)->extra_button_labels))[i]));
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
    (&(this)->button_layout)[tmp * 2] = pos;
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
    (&(this)->button_layout)[tmp * 2 + std_button_count * 2] = pos;
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
    gfx_get_screen_rect((iptr)((i8 *)&screen_rect2.left));
    main_frame_wnd_block.main_frame_wnd.get_screen_rect((iptr)((i8 *)&frame_rect2.left));
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
    (&(this)->button_alignment)[std_button_count * 2] = 1;
    std_button_count++;
  }
  if (((style_flags & 8) == 0) && ((style_flags & 0x2000000) == 0)) {
    (&(this)->button_alignment)[std_button_count * 2] = 0;
    std_button_count++;
  }
  if ((style_flags & 1) != 0) {
    (&(this)->button_alignment)[std_button_count * 2] = 2;
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
      (&(this)->button_x_coords)[i * 2] = (&(this)->button_x_coords)[i * 2] + origin_x;
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
          icon->draw((iptr)icon_rect, surf_wnd, x, y + (dy > 0 ? dy : 0));
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
    ((DibSurface *)surf_wnd)->draw_text(list_font,text,x + 2,y + 1,5);
    gfx_set_text_color_index(fg_color);
    ((DibSurface *)surf_wnd)->draw_text(list_font,text,x + 1,y,5);
  }
  if (disabled == 0) {
    gfx_set_text_color_index(fg_color);
  }
  else {
    gfx_set_text_color_index(highlight_color);
  }
  ((DibSurface *)surf_wnd)->draw_text(list_font,text,x,y,5);
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
      rect_set_xywh((iptr)&cell_rect,cell_x,cell_y,
                         (iptr)list_column_width[option->group],
                         (iptr)list_row_height[option->group]);
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
          gfx_draw_bevel_rect(surf_wnd,&inner_rect,bevel_light_color,bevel_dark_color);
          rect_deflate((iptr)&inner_rect,1,1);
        }
        gfx_fill_rect(surf_wnd,&inner_rect,fill_color);
        draw_option_text(option->text,cell_x + 2,cell_y,is_selected,
                   option->flags & 1);
      }
      if (blit != 0) {
        ((DibSurface *)surf_wnd)->wnd_blit_surface_rect((iptr)&cell_rect);
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
        gfx_fill_rect(surf_wnd,&list_client_rect[cur_pane],
                           list_fill_color);
      }
      gfx_draw_rect_frame(surf_wnd,&pane_rect[cur_pane],
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
          gfx_fill_rect(surf_wnd,&clip_rect2,list_fill_color);
        }
      }
      ((DibSurface *)surf_wnd)->reset_clip();
      ((DibSurface *)surf_wnd)->wnd_blit_surface_rect(
                 (iptr)&pane_rect[cur_pane]);
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
    surf_wnd = (uptr)surf_wnd2;
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
      ((DibSurface *)surf_wnd)->wnd_create((iptr)title,wnd_style,window_x,
                 window_y,wnd_width,wnd_height,(iptr)&game_palette_block.game_palette);
    }
    else {
      ((DibSurface *)surf_wnd)->wnd_create_with_surface((const char *)title,wnd_style,window_x,
                 window_y,wnd_width,wnd_height,(iptr)palette,(iptr)parent2);
    }
    if ((style_flags & 0x10000) == 0) {
      ((WndBase *)(surf_wnd + 0x48))->set_caption_height(title_height);
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
    ((SurfaceWindow *)surf_wnd)->set_redraw_callback(dialog_on_redraw);
    if (dlg_style.pending_idle_callback == 0) {
      if (dlg_style.default_idle_callback != 0) {
        ((ModalMenuWindow *)(surf_wnd + 0x48))->set_modal_idle_callback(dlg_style.default_idle_callback);
      }
    }
    else {
      ((ModalMenuWindow *)(surf_wnd + 0x48))->set_modal_idle_callback(dlg_style.pending_idle_callback);
      dlg_style.pending_idle_callback = 0;
    }
    if (dlg_style.pending_move_handler != 0) {
      ((WndHandlers *)(surf_wnd + 0x58))->set_move_handler(dlg_style.pending_move_handler);
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
  rect_set_xywh((iptr)rect,list_x,list_y,option_width,
                     option_row_height *
                     ((int)((columns - 1) + option_count) / (int)x));
  if (surf_wnd == 0) {
    parent_18c = 0;
  }
  else {
    parent_18c = surf_wnd + 0x48;
  }
  ((RadioGroup *)radio_group)->create(parent_18c,200,(iptr)rect,columns,
             option_count,(iptr)labels);
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
      parent_188 = surf_wnd + 0x48;
    }
    ((Checkbox *)(checkboxes + i * 0x3c))->create(parent_188,i + 200
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
        parent = surf_wnd + 0x48;
      }
      ((Scrollbar *)list_vscroll[cur_pane])->create(parent,
                 0xb,(iptr)&list_vscroll_rect[cur_pane],1);
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
        parent = surf_wnd + 0x48;
      }
      ((Scrollbar *)list_hscroll[cur_pane])->create(parent,0xc,
                 (iptr)&list_hscroll_rect[cur_pane],0);
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
  rect_set_xywh((iptr)rect,list_x,list_y,
                     list_width[cur_pane],list_height[cur_pane]);
  if (surf_wnd == 0) {
    parent = 0;
  }
  else {
    parent = surf_wnd + 0x48;
  }
  ((ListBox *)listbox[cur_pane])->create(parent,10,
             (iptr)rect);
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
      rect_set_xywh((iptr)rect,x,y2,width2,count);
      if (surf_wnd == 0) {
        parent_190 = 0;
      }
      else {
        parent_190 = surf_wnd + 0x48;
      }
      ((Hotspot *)(sprite_hotspots + i * 0x40))->create(parent_190,i + 300,
                 (iptr)rect);
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
        parent_194 = surf_wnd + 0x48;
      }
      value = (i32)edit_field->buffer;
      text = edit_field->width;
      count = edit_row_height();
      ((EditControl *)(edit_controls + i * 0x48))->create_at(parent_194,0x14,
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
  int *buttons;
  int i;
  iptr pos;
  i8 rect[16];
  iptr parent_198;
  iptr button_id;
  i8 *label;

  if (std_button_count + extra_button_count != 0) {
    count = std_button_count + extra_button_count;
    buttons = (int *)operator_new(count * 0x3c + 4);
    if (buttons == (int *)0x0) {
      buttons = (int *)0x0;
    }
    else {
      *buttons = count;
      crt_vector_ctor_iterator(buttons + 1,0x3c,*buttons,button_ctor,
                   ctrl_dtor);
      buttons++;
    }
    button_controls = (uptr)buttons;
  }
  for (i = 0; i < (int)(std_button_count + extra_button_count); i++) {
    if (i < (int)std_button_count) {
      button_id = *(iptr *)((u8 *)&extra_button_labels + i * 8 + 0x18);
      label = (((i8 * *)&dlg_style.button_labels))[button_id];
    }
    else {
      button_id = -1;
      label = *(i8 **)((int)this + (i - std_button_count) * 4 + 0x294);
    }
    pos = line_height();
    rect_set_xywh((iptr)rect,*(i32 *)((u8 *)&extra_button_labels + i * 8 + 0x1c),
                       button_y,button_width,pos);
    if (surf_wnd == 0) {
      parent_198 = 0;
    }
    else {
      parent_198 = surf_wnd + 0x48;
    }
    ((Button *)(button_controls + i * 0x3c))->create(parent_198,i + 100,
               (iptr)rect,(const char *)label);
    if ((button_id == 2) || ((button_id == 0 && ((style_flags & 1) == 0)))) {
      ((Button *)(button_controls + i * 0x3c))->set_cancel_button();
    }
    if (button_id == 0) {
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
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(iptr)&str_popupstackindex_0_popupstackindex_16,(iptr)&str_d_ss_franklinton_popup_1_cpp_2,0xc75);
    }
    dlg_style.stack_depth++;
    (*(Dialog * *)&net.dialog_stack_top) = this;
    dialog_state.active = this;
    (((i32 *)&dlg_style.stack))[dlg_style.stack_depth] = (i32)this;
  }
  if ((style_flags & 0x200) == 0) {
    unused_flag = 0;
    ctrl_set_pending_font((iptr)dialog_state.active->body_font);
    ui_set_button_font((iptr)&dialog_plain_font_obj_block.dialog_plain_font_obj);
    ui_set_ctrl_font((iptr)dialog_state.active->body_font);
    ui_set_default_font((iptr)&dialog_plain_font_obj_block.dialog_plain_font_obj);
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
      for (option = option_list; option != (DialogOption *)0x0;
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
          ((WndHandlers *)(surf_wnd + 0x58))->set_lbuttondown_handler(dialog_list_on_lbuttondown);
          ((WndHandlers *)(surf_wnd + 0x58))->set_lbuttonup_any_handler(dialog_list_on_lbuttonup);
          ((WndHandlers *)(surf_wnd + 0x58))->set_mousemove_handler(dialog_list_on_mousemove);
          ((WndHandlers *)(surf_wnd + 0x58))->set_dblclick_handler(dialog_list_on_dblclick);
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
    ((WndHandlers *)(surf_wnd + 0x58))->set_keydown_handler(dialog_on_keydown);
    ((WndHandlers *)(surf_wnd + 0x58))->set_char_handler((WndKeyFn)dialog_on_char);
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
  ((ModalMenuWindow *)((int)dialog_state.active->surf_wnd + 0x48))->end_modal_loop();
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
      button_id = *(int *)((iptr)dialog_state.active + 0x2ac + button_index * 8);
    }
    else {
      dialog_state.active->result_extra = (button_index - (int)dialog_state.active->std_button_count) + 1;
      button_id = 0;
    }
    if (button_id == 1) {
      dialog_state.active->style_flags = dialog_state.active->style_flags | 0x80;
    }
    if (((button_id == 0) && ((((byte *)&dialog_state.active->style_flags)[2] & 2) != 0)) &&
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
    if (button_id == 2) {
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
           (DialogSprite *)*(i32 *)((int)dialog_state.active->selected_sprite + 0x10);
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
          first_ch = crt_toupper((int)**(char **)((iptr)dialog_state.active + 0x294 + counter2 * 4));
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
      ((WndBase *)((int)dialog_state.active->surf_wnd + 0x48))->set_capture();
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
    ((WndBase *)((int)dialog_state.active->surf_wnd + 0x48))->release_capture();
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
        gfx_fill_rect(surf_wnd,rect,bg_color);
      }
      else {
        src_y = 0;
        src_x = 0;
        height = rect_height(rect);
        width = rect_width(rect);
        gfx_tile_fill(surf_wnd,dlg_style.client_tile_surface,rect->left,
                           rect->top,width,height,src_x,src_y);
      }
    }
    else {
      src_y = 0;
      src_x = 0;
      height = rect_height(rect);
      width = rect_width(rect);
      gfx_tile_fill(surf_wnd,dlg_style.frame_tile_surface,rect->left,
                         rect->top,width,height,src_x,src_y);
    }
  }
  else {
    height = rect_height(rect);
    width = rect_width(rect);
    gfx_copy_rect_same_pos(bg_surface,surf_wnd,rect->left,
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
    rect_set_xywh((iptr)((i8 *)&rect.left),origin_x,origin_y,wnd_width,wnd_height);
    if (border_width != 0) {
      for (i = 0; i < (int)bevel_width; i++) {
        gfx_draw_bevel_rect(surf_wnd,(RECT *)(iptr)((i8 *)&rect.left),bevel_light_color,bevel_dark_color);
        rect_deflate((iptr)((i8 *)&rect.left),1,1);
      }
      fill_rect_bg((RECT *)(iptr)((i8 *)&rect.left),1);
      rect_deflate((iptr)((i8 *)&rect.left),border_width + bevel_width * -2,0);
      rect.top = rect.top + title_height + bevel_width * -2;
      rect.bottom -= ((chrome_height - title_height) + bevel_width * -2);
      for (i = 0; i < (int)bevel_width; i++) {
        gfx_draw_bevel_rect(surf_wnd,(RECT *)(iptr)((i8 *)&rect.left),bevel_dark_color,bevel_light_color);
        rect_deflate((iptr)((i8 *)&rect.left),1,1);
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
    ((DibSurface *)surf_wnd)->draw_text(title_font,(const char *)title_text,title_x + 2,
               title_y + 1,5);
    gfx_set_text_color_index(title_color);
    ((DibSurface *)surf_wnd)->draw_text(title_font,(const char *)title_text,title_x + 1,title_y,5)
    ;
    ((DibSurface *)surf_wnd)->draw_text(title_font,(const char *)title_text,title_x,title_y,5);
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
        sprite->sprite->draw((iptr)sprite_rect,surf_wnd,sprite_x2,sprite_y2);
      }
      else {
        on_draw_sprite((i32)sprite->sprite,surf_wnd,sprite->id,sprite->field_04,sprite_x2,sprite_y2);
      }
      if (((style_flags & 0x20000) != 0) && ((i32 *)selected_sprite == ((i32 *)sprite))) {
        rect_set((iptr)((i8 *)&rect.left),sprite_x2 - 1,sprite_y2 - 1,sprite_w + sprite_x2,
                           sprite_h + sprite_y2);
        gfx_draw_rect_frame(surf_wnd,(RECT *)(iptr)((i8 *)&rect.left),frame_color);
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
    ((SurfaceWindow *)surf_wnd)->SurfaceWindow::show();
  }
  style_flags = style_flags | 0x20;
  return;
}

/* 005a5f34 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

u32 Dialog::run_modal(iptr out_text,iptr timeout_secs)

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
        ((ModalMenuWindow *)(surf_wnd + 0x48))->run_modal_loop();
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
        ((Control *)edit_controls)->Control::get_text((char *)out_text);
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
          ((ListBox *)listbox[cur_pane])->get_sel_text((char *)sel_text);
          result_id = (u32)option_list->id;
          option = option_list;
          i = 0;
          for (; option != (DialogOption *)0x0; option = option->next) {
            ((ListBox *)listbox[cur_pane])->get_text(i,
                       (char *)item_text);
            result = _strcmp(sel_text,item_text);
            if (result == 0) {
              result_id = (uint)option->id;
            }
            i++;
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
void Dialog::load_section_keyword(char *line,i8 *expanded,int &mode,int &default_value,uint has_edit,iptr edit_default,i8 *edit_text)
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
        text_expand_vars((iptr)key2,(iptr)expanded);
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
          text_expand_vars((iptr)key2,(iptr)expanded);
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
                                    crt_strcpy(edit_text,key2);
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
i32 Dialog::load_text_section(const char *file_name,const char *section,int default_value,iptr edit_default,iptr init_arg1, iptr init_arg2,iptr init_arg3,uint flags)

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
  crt_strcpy((char *)((void *)(((i8 *)&dialog_text_font_obj_block.dialog_section_name))),(const char *)((void *)section));
  cmp = text_open_section(file_name,section);
  if (cmp == 0) {
    if ((flags & 0x8000) == 0) {
      init(init_arg1,init_arg2,(RECT *)init_arg3,flags);
    }
    else {
      style_flags = style_flags | flags;
    }
    has_edit = (uint)(edit_default != 0);
    if (has_edit != 0) {
      crt_strcpy((char *)((void *)edit_text),(const char *)((void *)edit_default));
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
          text_expand_vars((iptr)line,(iptr)expanded);
          add_line((const char *)expanded,0xffffffff,0xffffffff,0xffffffff,0);
        }
        else if (mode == 2) {
          text_expand_vars((iptr)line,(iptr)expanded);
          if (has_edit == 0) {
            if ((style_flags & 4) == 0) {
              option = (DialogOption *)((uptr)add_option((const char *)expanded,option_index,0));
            }
            else {
              option = (DialogOption *)((uptr)add_checkbox_option((const char *)expanded,option_index,0));
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
            add_edit_field((const char *)expanded,(iptr)edit_buf,default_value);
          }
        }
      }
    } while (mode < 3);
    result = 0;
  }
  else if ((flags & 0x8000) == 0) {
    init(init_arg1,init_arg2,(RECT *)init_arg3,flags);
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

/* 005a9520 auto */
/* Library Function - Multiple Matches With Different Base Names
    public: void * __thiscall CDaoIndexFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CDaoRelationFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CODBCFieldInfo::`vector deleting destructor'(unsigned int)
    public: void * __thiscall CString::`vector deleting destructor'(unsigned int)
   
   Library: Visual Studio 1998 Debug */

Hotspot *Hotspot::vector_destruct(uint flags)

{
  if ((flags & 2) == 0) {
    hotspot_dtor(this);
    if ((flags & 1) != 0) {
      operator_delete(this);
    }
  }
  else {
    crt_vector_dtor_iterator(this,0x40,VECTOR_COUNT(this),hotspot_dtor);
    operator_delete(&VECTOR_COUNT(this));
  }
  return this;
}

/* 005a95b0 auto */
SurfaceWindow *SurfaceWindow::scalar_destruct(uint flags)

{
  destruct();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 005a9600 auto */
void WndBase::get_screen_rect(iptr rect)

{
  wnd_obj_get_screen_rect(native_wnd,(LPRECT)rect);
  return;
}

/* 005a96f0 auto */
uint Dialog::uses_alt_text_color()

{
  return (uint)((style_flags & 0x100) != 0);
}

/* 005a9730 auto */
void Dialog::draw_text_default_font(const char *text,i32 x,i32 y, i32 flags)

{
  draw_text_shadowed(body_font,(char *)text,x,y,flags);
  return;
}

/* 005bb4ae auto */
void DibSurface::wnd_create_with_surface(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,iptr palette,iptr parent)

{
  NativeWindow *hwnd;
  
  ((WndBase *)(this + 1))->create_framed_child(title,style,x,y,width,height,palette,
             parent);
  create(width,height);
  set_palette(palette);
  hwnd = ((WndBase *)(this + 1))->get_window();
  wnd_install_paint_proc((iptr)this,hwnd);
  return;
}

/* 005bb574 auto */
void SurfaceWindow::repaint()

{
  if (this == (SurfaceWindow *)0x0) {
    (*(u8 * *)&modal_wnd.active_child) = (u8 *)0x0;
  }
  else {
    (*(u8 * *)&modal_wnd.active_child) = (u8 *)&window;
  }
  call_paint_cb();
  wnd_blit_surface();
  return;
}

/* 005bb5be auto */
bool __thiscall surfwnd_resize_dib(DibSurface *self,iptr rect)

{
  int result;
  i32 height;
  i32 width;
  
  result = surface_alloc_dib_by_bpp(self,rect);
  if (result == 1) {
    height = rect_height((RECT *)rect);
    width = rect_width((RECT *)rect);
    ((WndBase *)(self + 1))->set_client_size(width,height);
  }
  return result == 1;
}

/* 005bb621 auto */
void SurfaceWindow::on_resize(int width,int height)

{
  int cur_size;
  i32 palette;
  tagRECT rect;
  
  cur_size = get_width();
  if ((cur_size == width) &&
     (cur_size = get_height(), cur_size == height)) {
    return;
  }
  SetRect(&rect,0,0,width,height);
  call_untyped(((code **)vtable)[0], this,&rect);
  palette = ((WndBase *)&window)->get_palette_obj();
  apply_palette((GamePalette *)palette);
  if (this == (SurfaceWindow *)0x0) {
    (*(u8 * *)&modal_wnd.active_child) = (u8 *)0x0;
  }
  else {
    (*(u8 * *)&modal_wnd.active_child) = (u8 *)&window;
  }
  call_paint_cb();
  wnd_blit_surface();
  return;
}

/* 005bb6c7 auto */
void SurfaceWindow::resize_clamped(int width,int height)

{
  int min_height2;
  int min_width2;
  int max_height2;
  int max_width2;
  
  ((ModalMenuWindow *)((u8 *)this + 0x48))->get_min_size((iptr)&min_width2,(iptr)&min_height2);
  ((ModalMenuWindow *)((u8 *)this + 0x48))->get_max_size((iptr)&max_width2,(iptr)&max_height2);
  if (width < min_width2) {
    width = min_width2;
  }
  if (max_width2 < width) {
    width = max_width2;
  }
  if (height < min_height2) {
    height = min_height2;
  }
  if (max_height2 < height) {
    height = max_height2;
  }
  ((WndBase *)((u8 *)this + 0x48))->set_client_size(width,height);
  return;
}

/* 005bb8c0 auto */
u32 WndBase::get_palette_obj()

{
  return wnd_palette;
}

/* 005bb8e0 auto */
void WndBase::set_client_size(i32 width,i32 height)

{
  wnd_obj_set_client_size(native_wnd,width,height);
  return;
}

/* 005bb910 auto */
void ModalMenuWindow::get_min_size(iptr out_width,iptr out_height)

{
  *(u32 *)out_width = min_width;
  *(u32 *)out_height = min_height;
  return;
}

/* 005bb950 auto */
void ModalMenuWindow::get_max_size(iptr out_width,iptr out_height)

{
  *(u32 *)out_width = max_width;
  *(u32 *)out_height = max_height;
  return;
}

/* 005bb990 auto */
void SurfaceWindow::call_paint_cb()

{
  if (on_paint != 0) {
    on_paint();
  }
  return;
}

/* 005bbc10 auto */
NativeWindow * __cdecl
wnd_obj_create(LPCSTR title,uint flags,int x,int y,int width,int height,
            NativeWindow *parent)

{
  i32 handle;
  NativeWindow *wnd;
  int metric;
  HWND new_hwnd;
  HDC hdc;
  HCURSOR cursor;
  int pos_x2;
  int pos_y2;
  int outer_width;
  int outer_height;
  HWND parent_hwnd;
  DWORD ex_style;
  uint style2;
  int caption_height2;
  int frame_height;
  int frame_width;
  
  ex_style = 0;
  handle = mem_handle_alloc(0x4c);
  wnd = (NativeWindow *)mem_handle_lock(handle);
  wnd->mem_handle = handle;
  frame_width = 0;
  frame_height = 0;
  caption_height2 = GetSystemMetrics(4);
  style2 = 0x2000000;
  if ((flags & 4) == 0) {
    if (((flags & 2) == 0) || ((flags & 0x40) != 0)) {
      if (((flags & 2) == 0) || ((flags & 0x40) == 0)) {
        if ((flags & 1) != 0) {
          style2 = 0x2800000;
          frame_width = GetSystemMetrics(7);
          frame_width = frame_width * 2;
          frame_height = GetSystemMetrics(8);
          frame_height = frame_height * 2;
          caption_height2++;
        }
      }
      else {
        style2 = 0x2800000;
        frame_width = GetSystemMetrics(7);
        frame_width = frame_width * 2;
        frame_height = GetSystemMetrics(8);
        frame_height = frame_height * 2;
      }
    }
    else {
      style2 = 0x2400000;
      frame_width = GetSystemMetrics(7);
      frame_width = frame_width * 2;
      frame_height = GetSystemMetrics(8);
      frame_height = frame_height * 2;
    }
  }
  else {
    style2 = 0x2040000;
    frame_width = GetSystemMetrics(0x20);
    frame_width = frame_width * 2;
    frame_height = GetSystemMetrics(0x21);
    frame_height = frame_height * 2;
  }
  if ((flags & 8) != 0) {
    style2 = style2 | 0x20000;
    flags = flags | 0x40;
  }
  if ((flags & 0x10) != 0) {
    style2 = style2 | 0x10000;
    flags = flags | 0x40;
  }
  if ((flags & 0x20) != 0) {
    style2 = style2 | 0x80000;
    flags = flags | 0x40;
  }
  if ((flags & 0x40) == 0) {
    caption_height2 = 0;
  }
  else {
    style2 = style2 | 0xc00000;
  }
  if ((flags & 0x80) != 0) {
    style2 = style2 | 0x200000;
    metric = GetSystemMetrics(2);
    frame_width = frame_width + metric - 1;
  }
  if ((flags & 0x100) != 0) {
    style2 = style2 | 0x100000;
    metric = GetSystemMetrics(3);
    frame_height = frame_height + metric - 1;
  }
  if (((flags & 0x200) != 0) && (parent != 0)) {
    style2 = style2 | 0x44000000;
    ex_style = 4;
  }
  if ((flags & 0x800) != 0) {
    style2 = style2 & 0xbfffffff | 0x80000000;
  }
  if ((flags & 0x400) == 0) {
    wnd->flag_400 = 0;
  }
  else {
    wnd->flag_400 = 1;
  }
  if ((flags & 0x1000) == 0) {
    wnd->flag_1000 = 0;
  }
  else {
    wnd->flag_1000 = 1;
  }
  if (parent == 0) {
    parent_hwnd = (HWND)0x0;
  }
  else {
    parent_hwnd = parent->hwnd;
  }
  if (height == -1) {
    outer_height = -0x80000000;
  }
  else {
    outer_height = frame_height + caption_height2 + height - 1;
  }
  if (width == -1) {
    outer_width = -0x80000000;
  }
  else {
    outer_width = frame_width + width;
  }
  if (y == -1) {
    pos_y2 = -0x80000000;
  }
  else {
    pos_y2 = y;
  }
  if (x == -1) {
    pos_x2 = -0x80000000;
  }
  else {
    pos_x2 = x;
  }
  new_hwnd = CreateWindowExA(ex_style,str_mswindowclass_2,title,style2,pos_x2,pos_y2,
                           outer_width,outer_height,parent_hwnd,(HMENU)0x0,err_report_ctx_block.app_hinstance,(LPVOID)0x0);
  wnd->hwnd = new_hwnd;
  wnd_list_add_hwnd(wnd->hwnd);
  hdc = GetDC(wnd->hwnd);
  wnd->hdc = hdc;
  cursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f00);
  wnd->cursor = cursor;
  wnd->palette = 0;
  wnd->icon = 0;
  wnd->destroyed = 0;
  wnd->bg_brush = 0;
  wnd->active_child = 0;
  wnd->bg_tile = 0;
  wnd->field_28 = 0;
  wnd->bg_surface = 0;
  wnd->flags = flags;
  wnd->caption_height = 0;
  wnd->resize_border = 0;
  wnd->topmost_child = 0;
  wnd->field_40 = 8;
  return wnd;
}

/* 005bbfee auto */
void __cdecl wnd_obj_enable(NativeWindow *wnd,BOOL enable)

{
  if (wnd != 0) {
    EnableWindow(wnd->hwnd,enable);
  }
  return;
}

/* 005bc0ab auto */
i32 __cdecl wnd_obj_destroy(NativeWindow *wnd)

{
  i32 handle;
  
  if (wnd != (NativeWindow *)0x0) {
    wnd_stack_remove(wnd->hwnd);
    if (wnd->destroyed == 0) {
      if (wnd->hwnd != 0) {
        SetWindowLongA(wnd->hwnd,4,0);
        ShowWindow(wnd->hwnd,0);
        SetMenu(wnd->hwnd,(HMENU)0x0);
        DestroyWindow(wnd->hwnd);
      }
      wnd->hwnd = 0;
    }
    if (wnd->bg_brush != 0) {
      DeleteObject(wnd->bg_brush);
    }
    handle = wnd->mem_handle;
    mem_handle_unlock(handle);
    mem_handle_free(handle);
  }
  return 0;
}

/* 005bc173 auto */
void __cdecl wnd_stack_check_leaks_nop(void)

{
  int i;
  
  if (ms_window_stack_count != 0) {
    for (i = 0; i < ms_window_stack_count; i++) {
    }
  }
  return;
}

/* 005bc1b5 auto */
void __cdecl wnd_list_add_hwnd(i32 hwnd)

{
  ms_window_stack_block.ms_window_stack_entries[ms_window_stack_count] = hwnd;
  ms_window_stack_count++;
  return;
}

/* 005bc1db auto */
void __cdecl wnd_stack_remove(int hwnd)

{
  int i;
  
  for (i = 0; (i < ms_window_stack_count && (ms_window_stack_block.ms_window_stack_entries[i] != hwnd));
      i = i + 1) {
  }
  if (i < ms_window_stack_count - 1) {
    for (; i < ms_window_stack_count - 1; i++) {
      ms_window_stack_block.ms_window_stack_entries[i] = ms_window_stack_block.ms_window_stack_entries[i + 1];
    }
  }
  ms_window_stack_count--;
  if (ms_window_stack_count < 0) {
    dbg_log((const char *)&str_error_ms_window_stack_inaccurate);
  }
  return;
}

/* 005bc280 auto */
void __cdecl wnd_obj_set_bg_color(NativeWindow *wnd,uint color_index)

{
  HBRUSH brush;
  GamePalette *self;
  i8 green[4];
  WndBase *wnd_view;
  i8 red[4];
  i8 blue[4];
  
  if (wnd->bg_brush != 0) {
    DeleteObject(wnd->bg_brush);
  }
  if (display_palette.palettized == 0) {
    wnd_view = (WndBase *)GetWindowLongA(wnd->hwnd,0);
    self = (GamePalette *)wnd_view->get_palette();
    self->get_entry(color_index & 0xff,(iptr)red,(iptr)green,(iptr)blue);
    brush = CreateSolidBrush(CONCAT12(blue[0],CONCAT11(green[0],red[0])));
    wnd->bg_brush = brush;
  }
  else {
    if (wnd->palette != 0) {
      RealizePalette(wnd->hdc);
    }
    brush = CreateSolidBrush(color_index & 0xff | 0x1000000);
    wnd->bg_brush = brush;
  }
  InvalidateRect(wnd->hwnd,(RECT *)0x0,0);
  return;
}

/* 005bc3bf auto */
void __cdecl wnd_obj_set_bg_tile(NativeWindow *wnd,DibSurface *tile)

{
  wnd->bg_tile = tile;
  return;
}

/* 005bc3f1 auto */
void __cdecl wnd_obj_set_bg_surface(NativeWindow *wnd,DibSurface *surface)

{
  wnd->bg_surface = surface;
  return;
}

/* 005bc40a auto */
void __cdecl wnd_obj_show_raise(NativeWindow *wnd)

{
  if (wnd != 0) {
    ShowWindow(wnd->hwnd,5);
    if ((wnd->flags_bytes[1] & 2) != 0) {
      BringWindowToTop(wnd->hwnd);
    }
  }
  return;
}

/* 005bc44d auto */
void __cdecl wnd_obj_hide(NativeWindow *wnd)

{
  if (wnd != 0) {
    ShowWindow(wnd->hwnd,0);
  }
  return;
}

/* 005bc4a1 auto */
void __cdecl wnd_obj_move_xy(NativeWindow *wnd,int x,int y)

{
  int nHeight;
  int nWidth;
  BOOL bRepaint;
  tagRECT rect;
  
  if (wnd != 0) {
    GetWindowRect(wnd->hwnd,&rect);
    bRepaint = 1;
    nHeight = rect_height(&rect);
    nWidth = rect_width(&rect);
    MoveWindow(wnd->hwnd,x,y,nWidth,nHeight,bRepaint);
  }
  return;
}

/* 005bc505 auto */
void __cdecl wnd_obj_set_client_size(NativeWindow *wnd,int width,int height)

{
  int frame_w;
  int frame_h;
  tagPOINT origin;
  HWND parent_hwnd;
  tagRECT rect;
  
  if (wnd != 0) {
    GetWindowRect(wnd->hwnd,&rect);
    frame_w = wnd_obj_frame_width(wnd);
    frame_h = wnd_obj_frame_height(wnd);
    parent_hwnd = GetParent(wnd->hwnd);
    if ((parent_hwnd != (HWND)0x0) && ((wnd->flags_bytes[1] & 2) != 0)) {
      origin.x = 0;
      origin.y = 0;
      MapWindowPoints(parent_hwnd,(HWND)0x0,&origin,1);
      rect.left -= origin.x;
      rect.top -= origin.y;
    }
    MoveWindow(wnd->hwnd,rect.left,rect.top,width + frame_w,height + frame_h,1);
  }
  return;
}

/* 005bc5da auto */
void __cdecl wnd_obj_maximize(NativeWindow *wnd)

{
  if (wnd != 0) {
    ShowWindow(wnd->hwnd,3);
  }
  return;
}

/* 005bc636 auto */
void __cdecl wnd_obj_minimize(NativeWindow *wnd)

{
  if (wnd != 0) {
    ShowWindow(wnd->hwnd,6);
  }
  return;
}

/* 005bc692 auto */
void __cdecl wnd_obj_show_restored(NativeWindow *wnd)

{
  if (wnd != 0) {
    ShowWindow(wnd->hwnd,9);
  }
  return;
}

/* 005bc6bb auto */
void __cdecl wnd_obj_validate_rect(NativeWindow *wnd,RECT *rect)

{
  if (wnd != 0) {
    ValidateRect(wnd->hwnd,rect);
  }
  return;
}

/* 005bc713 auto */
void __cdecl wnd_obj_move_to_rect(NativeWindow *wnd,int *rect)

{
  int nHeight;
  int nWidth;
  BOOL bRepaint;
  
  if (wnd != 0) {
    bRepaint = 1;
    nHeight = rect_height((RECT *)rect);
    nWidth = rect_width((RECT *)rect);
    MoveWindow(wnd->hwnd,*rect,rect[1],nWidth,nHeight,bRepaint);
  }
  return;
}

/* 005bc763 auto */
void __cdecl wnd_obj_center(NativeWindow *wnd,int offset_x,int offset_y)

{
  HWND hWnd;
  int outer_w;
  int outer_h;
  int wnd_w;
  BOOL bRepaint;
  tagRECT wnd_rect;
  int pos_y2;
  int pos_x2;
  tagPOINT origin;
  tagRECT parent_rect;
  
  if (wnd != 0) {
    GetWindowRect(wnd->hwnd,&wnd_rect);
    hWnd = GetParent(wnd->hwnd);
    if (hWnd == (HWND)0x0) {
      outer_w = GetSystemMetrics(0);
      outer_h = GetSystemMetrics(1);
      wnd_w = rect_width(&wnd_rect);
      pos_x2 = outer_w - wnd_w >> 1;
      outer_w = rect_height(&wnd_rect);
      pos_y2 = outer_h - outer_w >> 1;
    }
    else if ((wnd->flags_bytes[1] & 2) == 0) {
      GetWindowRect(hWnd,&parent_rect);
      outer_w = rect_width(&parent_rect);
      outer_h = rect_height(&parent_rect);
      wnd_w = rect_width(&wnd_rect);
      pos_x2 = outer_w - wnd_w >> 1;
      outer_w = rect_height(&wnd_rect);
      pos_y2 = outer_h - outer_w >> 1;
      origin.x = 0;
      origin.y = 0;
      MapWindowPoints(hWnd,(HWND)0x0,&origin,1);
      pos_x2 += parent_rect.left;
      pos_y2 += parent_rect.top;
    }
    else {
      GetClientRect(hWnd,&parent_rect);
      outer_w = rect_width(&parent_rect);
      outer_h = rect_height(&parent_rect);
      wnd_w = rect_width(&wnd_rect);
      pos_x2 = outer_w - wnd_w >> 1;
      outer_w = rect_height(&wnd_rect);
      pos_y2 = outer_h - outer_w >> 1;
      if (pos_y2 < 0) {
        pos_y2 = 0;
      }
    }
    bRepaint = 1;
    outer_w = rect_height(&wnd_rect);
    outer_h = rect_width(&wnd_rect);
    MoveWindow(wnd->hwnd,pos_x2 + offset_x,pos_y2 + offset_y,outer_h,outer_w,bRepaint);
  }
  return;
}

/* 005bc933 auto */
LONG __cdecl wnd_obj_client_width(NativeWindow *wnd)

{
  tagRECT rect;
  
  if (wnd == 0) {
    rect.right = 0;
  }
  else {
    GetClientRect(wnd->hwnd,&rect);
  }
  return rect.right;
}

/* 005bc96b auto */
LONG __cdecl wnd_obj_client_height(NativeWindow *wnd)

{
  tagRECT rect;
  
  if (wnd == 0) {
    rect.bottom = 0;
  }
  else {
    GetClientRect(wnd->hwnd,&rect);
  }
  return rect.bottom;
}

/* 005bc9a3 auto */
void __cdecl wnd_obj_get_screen_rect(NativeWindow *wnd,LPRECT rect)

{
  if (wnd != 0) {
    GetWindowRect(wnd->hwnd,rect);
  }
  return;
}

/* 005bc9d3 auto */
int __cdecl wnd_obj_frame_width(NativeWindow *wnd)

{
  int result;
  int client_w;
  tagRECT rect;
  
  if (wnd == 0) {
    result = 0;
  }
  else {
    GetWindowRect(wnd->hwnd,&rect);
    result = rect_width(&rect);
    GetClientRect(wnd->hwnd,&rect);
    client_w = rect_width(&rect);
    result -= client_w;
  }
  return result;
}

/* 005bca3d auto */
int __cdecl wnd_obj_frame_height(NativeWindow *wnd)

{
  int result;
  int client_h;
  tagRECT rect;
  
  if (wnd == 0) {
    result = 0;
  }
  else {
    GetWindowRect(wnd->hwnd,&rect);
    result = rect_height(&rect);
    GetClientRect(wnd->hwnd,&rect);
    client_h = rect_height(&rect);
    result -= client_h;
  }
  return result;
}

/* 005bcb85 auto */
void __cdecl wnd_obj_get_rect_in_parent(NativeWindow *wnd,LPRECT rect)

{
  HWND hWndTo;
  tagPOINT *lpPoints;
  UINT cPoints;
  tagPOINT corners[2];  /* top left, bottom right */
  
  if (wnd != 0) {
    GetWindowRect(wnd->hwnd,rect);
    corners[0].x = rect->left;
    corners[0].y = rect->top;
    corners[1].x = rect->right;
    corners[1].y = rect->bottom;
    cPoints = 2;
    lpPoints = corners;
    hWndTo = GetParent(wnd->hwnd);
    MapWindowPoints((HWND)0x0,hWndTo,lpPoints,cPoints);
    SetRect(rect,corners[0].x,corners[0].y,corners[1].x,corners[1].y);
  }
  return;
}

/* 005bcdc3 auto */
void __cdecl wnd_obj_select_palette(NativeWindow *wnd,HPALETTE palette)

{
  if (display_palette.palettized == 1) {
    wnd->palette = palette;
    SelectPalette(wnd->hdc,palette,0);
  }
  return;
}

/* 005bcdfc auto */
void __cdecl wnd_obj_attach_menu(NativeWindow *wnd,HMENU menu)

{
  int new_height;
  int menu_height;
  i32 width;
  
  if (wnd != 0) {
    SetMenu(wnd->hwnd,menu);
    new_height = wnd_obj_client_height(wnd);
    menu_height = GetSystemMetrics(0xf);
    new_height = new_height + menu_height + 1;
    width = wnd_obj_client_width(wnd);
    wnd_obj_set_client_size(wnd,width,new_height);
  }
  return;
}

/* 005bce5f auto */
void __cdecl wnd_obj_set_icon(NativeWindow *wnd,uint icon_id)

{
  HICON icon;
  BOOL is_window;
  
  if (wnd != 0) {
    if (wnd->icon != 0) {
      DestroyIcon(wnd->icon);
    }
    icon = LoadIconA(err_report_ctx_block.app_hinstance,(LPCSTR)(icon_id & 0xffff));
    wnd->icon = icon;
    is_window = IsWindow(wnd->hwnd);
    if ((is_window != 0) && (is_window = IsIconic(wnd->hwnd), is_window != 0)) {
      InvalidateRect(wnd->hwnd,(RECT *)0x0,0);
    }
  }
  return;
}

/* 005bcfdd auto */
void __cdecl wnd_obj_set_cursor_now(NativeWindow *wnd,i32 cursor_id,int apply_now)

{
  if ((wnd != 0) && (wnd_obj_set_cursor(wnd,cursor_id), apply_now != 0)) {
    SetCursor(wnd->cursor);
  }
  return;
}

/* 005bd05f auto */
void __cdecl wnd_obj_set_cursor(NativeWindow *wnd,uint cursor_id)

{
  HCURSOR cursor;
  
  if (wnd != 0) {
    if ((int)cursor_id < 0) {
      if (cursor_id == 0xffffffff) {
        cursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f00);
        wnd->cursor = cursor;
      }
      else if (cursor_id == 0xfffffffe) {
        cursor = LoadCursorA((HINSTANCE)0x0,(LPCSTR)0x7f02);
        wnd->cursor = cursor;
      }
    }
    else {
      cursor = LoadCursorA(err_report_ctx_block.app_hinstance,(LPCSTR)(cursor_id & 0xffff));
      wnd->cursor = cursor;
    }
  }
  return;
}

/* 005bd0e7 auto */
void __cdecl wnd_obj_focus_and_raise(NativeWindow *wnd)

{
  if (wnd != 0) {
    SetFocus(wnd->hwnd);
    BringWindowToTop(wnd->hwnd);
  }
  return;
}

/* 005bd120 auto */
void __cdecl wnd_obj_set_focus(NativeWindow *wnd)

{
  if (wnd != 0) {
    SetFocus(wnd->hwnd);
  }
  return;
}

/* 005bd14c auto */
void __cdecl wnd_obj_disable_owner(NativeWindow *wnd,NativeWindow *owner)

{
  HWND parent_hwnd;
  BOOL is_window;
  
  if ((owner == 0) && (wnd != 0)) {
    parent_hwnd = GetParent(wnd->hwnd);
    is_window = IsWindow(parent_hwnd);
    if (is_window != 0) {
      is_window = 0;
      parent_hwnd = GetParent(wnd->hwnd);
      EnableWindow(parent_hwnd,is_window);
    }
  }
  else if (owner != 0) {
    EnableWindow(owner->hwnd,0);
  }
  return;
}

/* 005bd1c5 auto */
void __cdecl wnd_obj_enable_owner(NativeWindow *wnd,NativeWindow *owner)

{
  HWND parent_hwnd;
  BOOL is_window;
  
  if ((owner == 0) && (wnd != 0)) {
    parent_hwnd = GetParent(wnd->hwnd);
    is_window = IsWindow(parent_hwnd);
    if (is_window != 0) {
      is_window = 1;
      parent_hwnd = GetParent(wnd->hwnd);
      EnableWindow(parent_hwnd,is_window);
    }
  }
  else if (owner != 0) {
    EnableWindow(owner->hwnd,1);
  }
  return;
}

/* 005bd248 auto */
void __cdecl wnd_obj_set_caption_height(NativeWindow *wnd,i32 height)

{
  if (wnd != 0) {
    wnd->caption_height = height;
  }
  return;
}

/* 005bd270 auto */
void __cdecl wnd_obj_set_resize_border(NativeWindow *wnd,i32 border)

{
  if (wnd != 0) {
    wnd->resize_border = border;
  }
  return;
}

/* 005bd48f auto */
void __cdecl wnd_obj_get_cursor_pos(NativeWindow *wnd,LONG *out_x,LONG *out_y)

{
  tagPOINT pt;
  
  GetCursorPos(&pt);
  ScreenToClient(wnd->hwnd,&pt);
  *out_x = pt.x;
  *out_y = pt.y;
  return;
}

/* 005bd533 auto */
void __cdecl wnd_obj_set_capture(NativeWindow *wnd)

{
  SetCapture(wnd->hwnd);
  return;
}

/* 005bd550 auto */
void __cdecl wnd_obj_release_capture(NativeWindow *wnd)

{
  ReleaseCapture();
  return;
}

/* 005bd566: written by hand in src/game/retvals.cpp */

/* 005bd610 auto */
i16 WndBase::get_style_flags()

{
  return (short)style;
}

/* 005c5760 auto */
void WndBase::create_framed(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,iptr palette)

{
  NativeWindow *hwnd;
  
  create_with_cmd_proc(title,style,x,y,width,height,palette);
  hwnd = get_window();
  wnd_install_frame_proc((iptr)this,hwnd);
  return;
}

/* 005c57f9 auto */
void WndBase::create_framed_child(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,iptr palette,iptr parent)

{
  NativeWindow *hwnd;
  
  create_with_handlers(title,style,x,y,width,height,palette,parent);
  hwnd = get_window();
  wnd_install_frame_proc((iptr)this,hwnd);
  return;
}

/* 005c59c4 auto */
void Wnd::append_control(iptr ctrl)

{
  uptr last;
  
  last = control_list;
  if (last == 0) {
    control_list = ctrl;
  }
  else {
    for (; *(int *)(last + 0x20) != 0; last = *(uptr *)(last + 0x20)) {
    }
    *(iptr *)(last + 0x20) = ctrl;
  }
  return;
}

/* 005c5a27 auto */
void Wnd::remove_control(int ctrl_id)

{
  int id;
  Control *this_00;
  Control *this_01;
  u32 next_next;
  Control *ctrl;
  
  if (control_list != 0) {
    ctrl = (Control *)control_list;
    id = ctrl->get_id();
    if (id == ctrl_id) {
      control_list = ctrl->next_ctrl;
    }
    else {
      while( true ) {
        if (ctrl == (Control *)0x0) {
          return;
        }
        id = ctrl->get_next();
        if (id == 0) {
          return;
        }
        this_00 = (Control *)ctrl->get_next();
        id = this_00->get_id();
        if (id == ctrl_id) break;
        ctrl = (Control *)ctrl->get_next();
      }
      this_01 = (Control *)ctrl->get_next();
      next_next = this_01->get_next();
      ctrl->next_ctrl = next_next;
    }
  }
  return;
}

/* 005c5aeb auto */
void Wnd::remove_all_controls()

{
  i32 ctrl_id;
  Control *ctrl;
  
  for (ctrl = (Control *)control_list; ctrl != (Control *)0x0;
      ctrl = (Control *)ctrl->next_ctrl) {
    ctrl_id = ctrl->get_id();
    remove_control(ctrl_id);
  }
  return;
}

/* 005c5b7f auto */
void Wnd::focus_first_control()

{
  char wants_focus;
  Control *ctrl;
  
  ctrl = (Control *)control_list;
  while( true ) {
    if (ctrl == (Control *)0x0) {
      if ((control_list != 0) && ((char)keyboard_nav != '\0')) {
        wnd_focus_control((iptr)this,control_list);
      }
      return;
    }
    wants_focus = ctrl->get_initial_focus();
    if (wants_focus != '\0') break;
    ctrl = (Control *)ctrl->get_next();
  }
  if ((char)keyboard_nav == '\0') {
    return;
  }
  wnd_focus_control((iptr)this,(iptr)ctrl);
  return;
}

/* 005c5c86 auto */
i32 Wnd::dispatch_hotkey_char(i32 ch)

{
  u8 key;
  iptr ctrl_type;
  iptr enabled;
  i32 hwnd;
  int value;
  int item_count;
  int i;
  i32 handled;
  Control *ctrl;
  u8 key_str[4];
  char done;
  
  done = '\0';
  handled = 0;
  key_str[0] = (u8)ch;
  key_str[1] = 0;
  __strlwr((char *)key_str);
  key = key_str[0];
  ctrl = (Control *)control_list;
  do {
    if (ctrl == (Control *)0x0) {
      return handled;
    }
    if (done != '\0') {
      return handled;
    }
    if ((int)ctrl->hotkey_pos < 0) {
      value = ctrl->get_type();
      if (value == 3) {
        value = ((RadioGroup *)ctrl)->get_items();
        item_count = ((RadioGroup *)ctrl)->radiogrp_item_count();
        for (i = 0; i < item_count; i++) {
          if ((-1 < *(int *)(value + 0xa0 + i * 0xa4)) &&
             (*(u8 *)(value + 0x9c + i * 0xa4) == key)) {
            radiogrp_item_activate(*(i32 *)(value + i * 0xa4));
            handled = 1;
            done = 1;
            break;
          }
        }
      }
    }
    else if (ctrl->hotkey_char == key) {
      ctrl_type = ctrl->get_type();
      if ((ctrl_type == 6) &&
         (enabled = ((Button *)ctrl)->ctrl_is_enabled(), enabled != 0)) {
        hwnd = ctrl->get_hwnd();
        button_press_animated(hwnd);
        return 1;
      }
      value = ctrl->get_type();
      if ((value == 2) && (value = ((Checkbox *)ctrl)->checkbox_is_enabled(), value != 0)) {
        hwnd = ctrl->get_hwnd();
        checkbox_toggle_by_hotkey(hwnd);
        return 1;
      }
    }
    ctrl = (Control *)ctrl->next_ctrl;
  } while( true );
}

/* 005c5f20 auto */
void WndBase::create_with_cmd_proc(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,iptr palette)

{
  NativeWindow *hwnd;
  
  create((i32)title,style,x,y,width,height,palette);
  hwnd = get_window();
  wnd_install_command_proc((iptr)this,hwnd);
  ((WndHandlers *)(this + 1))->clear_handlers();
  return;
}

/* 005c5fc4 auto */
void WndBase::create_with_handlers(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,iptr palette,iptr parent)

{
  NativeWindow *hwnd;
  
  create_child((i32)title,style,x,y,width,height,palette,(WndBase *)parent);
  hwnd = get_window();
  wnd_install_command_proc((iptr)this,hwnd);
  ((WndHandlers *)(this + 1))->clear_handlers();
  return;
}

/* 005c61b0 auto */
void ModalMenuWindow::run_modal_loop()

{
  NativeWindow *wnd;
  NativeWindow *owner_after;
  NativeWindow *owner2;
  
  if (modal_wnd.depth != 0xf) {
    modal_running = 1;
    modal_wnd.depth++;
    (((i32 *)&wnd_modal_stack_below_block.wnd_modal_stack))[modal_wnd.depth] = (i32)this;
    if (modal_wnd.depth == 0) {
      owner2 = 0;
    }
    else {
      owner2 = ((WndBase *)(((i32 *)&wnd_modal_stack_below_block.wnd_modal_stack_below))[modal_wnd.depth])->get_window();
    }
    wnd = get_window();
    wnd_obj_disable_owner(wnd,owner2);
    while (modal_running != 0) {
      win_pump_one_message();
      if (on_modal_idle != 0) {
        on_modal_idle();
      }
    }
    if (modal_wnd.depth == 0) {
      owner_after = 0;
    }
    else {
      owner_after = ((WndBase *)(((i32 *)&wnd_modal_stack_below_block.wnd_modal_stack_below))[modal_wnd.depth])->get_window();
    }
    wnd = get_window();
    wnd_obj_enable_owner(wnd,owner_after);
    modal_wnd.depth--;
    if (-1 < modal_wnd.depth) {
      modal_running = 1;
    }
  }
  return;
}

/* 005c62cb auto */
/* Library Function - Single Match
    public: void __thiscall CRichEditDoc::InvalidateObjectCache(void)
   
   Library: Visual Studio 1998 Debug */

void ModalMenuWindow::end_modal_loop()

{
  modal_running = 0;
  return;
}

/* 005c62ee auto */
i32 __cdecl wnd_get_active_child(void)

{
  return modal_wnd.active_child;
}

/* 005c6303 auto */
i32 __cdecl wnd_set_active_child(iptr child)

{
  i32 prev;
  
  prev = modal_wnd.active_child;
  modal_wnd.active_child = child;
  return prev;
}

/* 005c63af auto */
bool ModalMenuWindow::fire_menu_select(i32 item_id)

{
  bool has_handler;
  
  has_handler = on_menu_select != 0;
  if (has_handler) {
    on_menu_select(item_id);
  }
  return has_handler;
}

/* 005c8b00 auto */
void __cdecl wnd_invalidate(HWND hwnd)

{
  if (hwnd != (HWND)0x0) {
    InvalidateRect(hwnd,(RECT *)0x0,0);
  }
  return;
}

/* 005c8b2d auto */
void __cdecl hwnd_show(HWND hwnd)

{
  if (hwnd != (HWND)0x0) {
    ShowWindow(hwnd,5);
  }
  return;
}

/* 005c8b58 auto */
void __cdecl hwnd_hide(HWND hwnd)

{
  if (hwnd != (HWND)0x0) {
    ShowWindow(hwnd,0);
  }
  return;
}

/* 005c8be1 auto */
HWND __cdecl hotspot_create_hwnd(int *rect,iptr ctrl)

{
  WndBase *self;
  NativeWindow *parent_wnd;
  iptr height;
  int width;
  HWND hotspot_hwnd;
  HMENU hMenu;
  HINSTANCE hInstance;
  LPVOID lpParam;
  
  lpParam = (LPVOID)0x0;
  hMenu = (HMENU)0x0;
  hInstance = err_report_ctx_block.app_hinstance;
  self = (WndBase *)((Control *)ctrl)->get_parent();
  parent_wnd = self->get_window();
  hotspot_hwnd = parent_wnd->hwnd;
  height = rect_height((RECT *)rect);
  width = rect_width((RECT *)rect);
  hotspot_hwnd = CreateWindowExA(4,str_mscontrolclass,((i8 *)&hotspot_wnd_title),0x50000000,*rect,rect[1],
                           width,height,hotspot_hwnd,hMenu,hInstance,lpParam);
  width = (int)ctrl_init_block(hotspot_hwnd,ctrl);
  *(i32 *)(width + 0x2c) = 1;
  SetWindowLongA(hotspot_hwnd,-4,(iptr)hotspot_wndproc);
  return hotspot_hwnd;
}

/* 005c8f70 auto */
void Hotspot::fire_enter(i32 arg)

{
  if (on_enter != 0) {
    on_enter(arg);
  }
  return;
}

/* 005c8fb0 auto */
void Hotspot::fire_leave(i32 event_arg)

{
  if (on_leave != 0) {
    on_leave(event_arg);
  }
  return;
}

/* 005c8ff0 auto */
void Hotspot::fire_click(i32 event_arg)

{
  if (on_click != 0) {
    on_click(event_arg);
  }
  return;
}

/* 005c9030 auto */
void Hotspot::fire_press(i32 event_arg)

{
  if (on_press != 0) {
    on_press(event_arg);
  }
  return;
}

/* 005c9070 auto */
void Hotspot::fire_dblclick(i32 event_arg)

{
  if (on_dblclick != 0) {
    on_dblclick(event_arg);
  }
  return;
}

/* 005c9563 auto */
ControlBlock * __cdecl wnd_from_hwnd(HWND hwnd)

{
  ControlBlock *wnd_data;
  
  if (hwnd == (HWND)0x0) {
    wnd_data = 0;
  }
  else {
    wnd_data = (ControlBlock *)GetWindowLongA(hwnd,0);
  }
  return wnd_data;
}

/* 005c96cc auto */
void __cdecl wnd_redraw_client(HWND hwnd)

{
  tagRECT client_rect2;
  
  GetClientRect(hwnd,&client_rect2);
  InvalidateRect(hwnd,&client_rect2,1);
  UpdateWindow(hwnd);
  return;
}

/* 005cbdd0 auto */
u8 Wnd::keyboard_nav_enabled()

{
  return keyboard_nav;
}

/* 005cbeb0 auto */
void __cdecl ui_wait_ticks(i32 ticks)

{
  timer_busy_wait_ticks(ticks);
  return;
}

/* 005cbed0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl default_draw_font_static_init(void)

{
  default_draw_font_ctor();
  default_draw_font_reg_dtor();
  return;
}

/* 005cbeea auto */
void __cdecl default_draw_font_ctor(void)

{
  ((GameFont *)((i32 *)&default_draw_font_obj_block.default_draw_font_obj))->init_empty();
  return;
}

/* 005cbf04 auto */
void __cdecl default_draw_font_reg_dtor(void)

{
  _atexit(default_draw_font_static_dtor);
  return;
}

/* 005cbf21 auto */
void __cdecl default_draw_font_static_dtor(void)

{
  ((GameFont *)((i32 *)&default_draw_font_obj_block.default_draw_font_obj))->destroy();
  return;
}

/* 005d2550 auto */
void __cdecl ui_set_ctrl_text_color(i32 color_index)

{
  (*(i8 *)&ctrl_colors.text) = (i8)color_index;
  return;
}

/* 005d2568 auto */
void __cdecl ui_set_ctrl_text_shadow(i32 color_index,i32 dx,i32 dy)

{
  (*(i8 *)&ctrl_colors.shadow) = (i8)color_index;
  (*(i8 *)&ctrl_colors.shadow_dx) = (i8)dx;
  (*(i8 *)&ctrl_colors.shadow_dy) = (i8)dy;
  return;
}

/* 005d2590 auto */
void __cdecl ui_set_ctrl_disabled_color(i32 color_index)

{
  (*(i8 *)&ctrl_colors.disabled) = (i8)color_index;
  return;
}

/* 005d25a8 auto */
void __cdecl ui_set_ctrl_font(iptr font)

{
  (*(i8 * *)(draw_target.font_rest + 8)) = (i8 *)font;
  return;
}

/* 005d268e auto */
void __cdecl ui_set_default_font(iptr font)

{
  (*(i8 * *)(draw_target.font_rest + 12)) = (i8 *)font;
  return;
}

/* 005d2d4d auto */
void __cdecl hwnd_get_text(HWND hwnd,LPARAM buffer)

{
  LRESULT text_len;
  
  text_len = SendMessageA(hwnd,0xe,0,0);
  SendMessageA(hwnd,0xd,text_len + 1,buffer);
  return;
}

/* 005d2d7f auto */
void __cdecl hwnd_send_settext(HWND hwnd,LPARAM text)

{
  SendMessageA(hwnd,0xc,0,text);
  return;
}

/* 005d417f auto */
void __cdecl ui_set_radio_text_color(i32 color_index)

{
  (*(i8 *)&ctrl_colors.radio_text) = (i8)color_index;
  return;
}

/* 005d4197 auto */
void __cdecl ui_set_radio_disabled_color(i32 color_index)

{
  (*(i8 *)&ctrl_colors.radio_disabled) = (i8)color_index;
  return;
}

/* 005d41af auto */
void __cdecl ui_set_radio_text_shadow(i32 color_index,i32 dx,i32 dy)

{
  (*(i8 *)&ctrl_colors.radio_shadow) = (i8)color_index;
  (*(i8 *)&ctrl_colors.radio_dx) = (i8)dx;
  (*(i8 *)&ctrl_colors.radio_dy) = (i8)dy;
  return;
}

/* 005d8236 auto */
void __cdecl ui_set_button_font(iptr font)

{
  (*(i8 * *)draw_target.font_rest) = (i8 *)font;
  return;
}

/* 005db610 auto */
WndBase *WndBase::init()

{
  native_wnd = 0;
  wnd_palette = 0;
  wnd_record = 0;
  style = 0;
  return this;
}

/* 005db650 auto */
void WndBase::destroy()

{
  NativeWindow *destroyed_obj;
  
  destroyed_obj = (NativeWindow *)wnd_obj_destroy(native_wnd);
  native_wnd = destroyed_obj;
  return;
}

/* 005db67b auto */
void WndBase::create(i32 wnd_kind,u32 style,i32 x,i32 y, i32 width,i32 height,iptr palette)

{
  NativeWindow *native;
  
  this->style = style;
  if (native_wnd != 0) {
    native = (NativeWindow *)wnd_obj_destroy(native_wnd);
    native_wnd = native;
  }
  native = wnd_obj_create((LPCSTR)wnd_kind,style,x,y,width,height,0);
  native_wnd = native;
  wnd_set_owner_ptr((iptr)this,native_wnd);
  set_palette(palette);
  return;
}

/* 005db893 auto */
void WndBase::create_child(i32 wnd_kind,u32 style,i32 x,i32 y, i32 width,i32 height,iptr palette,WndBase *parent)

{
  NativeWindow *native;
  NativeWindow *parent_wnd;
  
  this->style = style;
  if (native_wnd != 0) {
    native = (NativeWindow *)wnd_obj_destroy(native_wnd);
    native_wnd = native;
  }
  parent_wnd = parent->get_window();
  native = wnd_obj_create((LPCSTR)wnd_kind,style,x,y,width,height,parent_wnd);
  native_wnd = native;
  wnd_set_owner_ptr((iptr)this,native_wnd);
  set_palette(palette);
  return;
}

/* 005dba95 auto */
i32 Wnd::is_shift_down()

{
  i32 is_down;
  
  is_down = input_poll_modifier(0x100);
  return is_down;
}

/* 005dbab8 auto */
i32 Wnd::is_ctrl_down()

{
  i32 is_down;
  
  is_down = input_poll_modifier(0x200);
  return is_down;
}

/* 005dcc10 auto */
GameMenu *GameMenu::handle_construct()

{
  hmenu = 0;
  return this;
}

/* 005dcc32 auto */
/* Library Function - Single Match
    public: __thiscall _Timevec::~_Timevec(void)
   
   Library: Visual Studio 1998 Debug */

void GameMenu::handle_destruct()

{
  menu_hmenu_destroy(hmenu);
  return;
}

/* 005e00bb auto */
void __cdecl dlg_radio_choice_button_cb(int button_id)

{
  if (button_id == 0x65) {
    ((ModalMenuWindow *)(err_report_ctx_block.choice_dlg_window + 0x48))->end_modal_loop();
  }
  else if (button_id == 0x66) {
    err_report_ctx_block.choice_dlg_selection = 0xffffffff;
    ((ModalMenuWindow *)(err_report_ctx_block.choice_dlg_window + 0x48))->end_modal_loop();
  }
  return;
}

/* 005e0863: written by hand in src/game/unwind.cpp */

/* 005e0ce0 auto */
iptr __cdecl menu_hmenu_parse_items(HMENU hmenu,iptr spec)

{
  bool done;
  iptr _Source;
  size_t uBytes;
  HLOCAL hMem;
  char *_Dest;
  int cmp;
  int item_count2;
  HMENU item_id;
  uint flags2;
  size_t label_len;
  
  done = false;
  item_count2 = 0;
  uBytes = _strlen((char *)spec);
  hMem = LocalAlloc(2,uBytes);
  _Dest = (char *)LocalLock(hMem);
  *_Dest = '\0';
  for (; *(char *)spec == ' '; spec++) {
  }
  while (!done) {
    if (*(char *)spec == '!') {
      flags2 = 3;
      spec++;
    }
    else {
      flags2 = 0;
    }
    if (*(char *)spec == '*') {
      flags2 = flags2 | 8;
      spec++;
    }
    _Source = spec;
    label_len = 0;
    for (; ((*(char *)spec != '}' && (*(char *)spec != '_')) && (*(char *)spec != '{'));
        spec = spec + 1) {
      label_len++;
    }
    _strncat(_Dest,(char *)_Source,label_len);
    if ((*(char *)spec == '_') || (*(char *)spec == '}')) {
      if (*(char *)spec == '}') {
        done = true;
      }
      item_id = menu_next_item_id;
      menu_next_item_id = (HMENU)((iptr)menu_next_item_id + 1);
      do {
        spec++;
      } while (*(char *)spec == ' ');
    }
    else if (*(char *)spec == '{') {
      item_id = CreateMenu();
      spec = menu_hmenu_parse_items(item_id,spec + 1);
      flags2 = flags2 | 0x10;
      for (; *(char *)spec == ' '; spec++) {
      }
      if (*(char *)spec == '}') {
        done = true;
        spec++;
      }
    }
    item_count2++;
    if (0x16 < item_count2) {
      item_count2 = 0;
      flags2 = flags2 | 0x40;
    }
    cmp = _strcmp(_Dest,((i8 *)&menu_separator_tag));
    if (cmp == 0) {
      flags2 = 0x800;
      *_Dest = '\0';
      item_id = (HMENU)0x0;
    }
    AppendMenuA(hmenu,flags2,item_id,_Dest);
    *_Dest = '\0';
  }
  LocalUnlock(hMem);
  LocalFree(hMem);
  return spec;
}

/* 005e0f2a auto */
HMENU __cdecl menu_hmenu_build_from_spec(char *spec)

{
  char prefix_char;
  int base_item_id;
  char *_Source;
  HMENU hMenu;
  size_t uBytes;
  HLOCAL hMem;
  char *_Dest;
  HMENU uIDNewItem;
  size_t label_len;
  char *cursor;
  
  cursor = spec;
  hMenu = CreateMenu();
  base_item_id = (*(int *)&menu_next_item_id);
  uBytes = _strlen(spec);
  hMem = LocalAlloc(2,uBytes);
  _Dest = (char *)LocalLock(hMem);
  *_Dest = '\0';
  while (*cursor != '\0') {
    for (; *cursor == ' '; cursor++) {
    }
    prefix_char = *cursor;
    if (prefix_char == '!') {
      cursor++;
    }
    _Source = cursor;
    label_len = 0;
    for (; *cursor != '{'; cursor++) {
      label_len++;
    }
    _strncat(_Dest,_Source,label_len);
    do {
      cursor++;
    } while (*cursor == ' ');
    uIDNewItem = CreateMenu();
    cursor = (char *)menu_hmenu_parse_items(uIDNewItem,(iptr)cursor);
    if (prefix_char != '!') {
      AppendMenuA(hMenu,0x10,uIDNewItem,_Dest);
    }
    else {
      AppendMenuA(hMenu,0x13,uIDNewItem,_Dest);
    }
    *_Dest = '\0';
  }
  (*(int *)&menu_next_item_id) = base_item_id + 100;
  LocalUnlock(hMem);
  LocalFree(hMem);
  return hMenu;
}

/* 005e10c7 auto */
void __cdecl menu_hmenu_destroy(HMENU hmenu)

{
  if (hmenu != (HMENU)0x0) {
    DestroyMenu(hmenu);
  }
  return;
}

/* 005e10fb auto */
void __cdecl menu_redraw_bar(NativeWindow *wnd)

{
  DrawMenuBar(wnd->hwnd);
  return;
}

/* 005e1118 auto */
BOOL __cdecl menu_hmenu_enable_item(HMENU hmenu,int menu_pos,int item_pos,i32 enable)

{
  BOOL enable_ret;
  HMENU hMenu;
  
  if (item_pos == 0) {
    if ((char)enable == '\0') {
      enable_ret = EnableMenuItem(hmenu,menu_pos - 1,0x403);
    }
    else {
      enable_ret = EnableMenuItem(hmenu,menu_pos - 1,0x400);
    }
  }
  else {
    hMenu = GetSubMenu(hmenu,menu_pos - 1);
    if ((char)enable == '\0') {
      enable_ret = EnableMenuItem(hMenu,item_pos - 1,0x403);
    }
    else {
      enable_ret = EnableMenuItem(hMenu,item_pos - 1,0x400);
    }
  }
  return enable_ret;
}

/* 005e11be auto */
void __cdecl menu_hmenu_check_item(HMENU hmenu,int menu_pos,int item_pos,i32 checked)

{
  HMENU hMenu;
  
  if (hmenu != (HMENU)0x0) {
    hMenu = GetSubMenu(hmenu,menu_pos - 1);
    if ((char)checked == '\0') {
      CheckMenuItem(hMenu,item_pos - 1,0x400);
    }
    else {
      CheckMenuItem(hMenu,item_pos - 1,0x408);
    }
  }
  return;
}

/* 005e1226 auto */
void __cdecl menu_hmenu_delete_item(HMENU hmenu,int menu_pos,int item_pos)

{
  HMENU hMenu;
  
  if (hmenu != (HMENU)0x0) {
    hMenu = GetSubMenu(hmenu,menu_pos - 1);
    if (item_pos == -1) {
      RemoveMenu(hmenu,menu_pos - 1,0x400);
    }
    else {
      DeleteMenu(hMenu,item_pos - 1,0x400);
    }
  }
  return;
}

/* 005e128c auto */
void __cdecl menu_hmenu_insert_item(HMENU hmenu,int menu_pos,int item_pos,LPCSTR text)

{
  UINT_PTR uIDNewItem;
  HMENU submenu;
  int item_count2;
  uint flags2;
  
  if (hmenu != (HMENU)0x0) {
    submenu = GetSubMenu(hmenu,menu_pos - 1);
    item_count2 = 0;
    if (((submenu != (HMENU)0x0) && (item_pos != -1)) && (menu_pos != -1)) {
      item_count2 = GetMenuItemCount(submenu);
    }
    uIDNewItem = (*&menu_next_item_id);
    if ((item_count2 % 0x16 == 0) && (item_count2 != 0)) {
      flags2 = 0x440;
    }
    else {
      flags2 = 0x400;
    }
    if (item_pos == -1) {
      submenu = CreatePopupMenu();
      if (menu_pos == -1) {
        AppendMenuA(hmenu,flags2 | 0x10,submenu,text);
      }
      else {
        InsertMenuA(hmenu,menu_pos - 1,flags2 | 0x10,submenu,text);
      }
    }
    else {
      (*&menu_next_item_id)++;
      InsertMenuA(submenu,item_pos - 1,flags2,uIDNewItem,text);
    }
  }
  return;
}

/* 005e14c8 auto */
void __cdecl menu_hmenu_set_item_text(HMENU hmenu,int menu_pos,int item_pos,LPCSTR text)

{
  HMENU hMenu;
  UINT uIDNewItem;
  
  if (hmenu != (HMENU)0x0) {
    hMenu = GetSubMenu(hmenu,menu_pos - 1);
    if (item_pos == -1) {
      ModifyMenuA(hmenu,menu_pos - 1,0x410,hMenu,text);
    }
    else {
      uIDNewItem = GetMenuItemID(hMenu,item_pos - 1);
      ModifyMenuA(hMenu,item_pos - 1,0x400,uIDNewItem,text);
    }
  }
  return;
}

/* 005e1880 auto */
void __cdecl wnd_install_paint_proc(iptr surface,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,0xc,surface);
  SetWindowLongA(wnd->hwnd,-4,(iptr)surfwnd_subclass_wndproc);
  return;
}

/* 005e18ff auto */
i32 __stdcall surfwnd_subclass_wndproc(HWND hwnd,int msg,i32 wparam,uint lparam)

{
  SurfaceWindow *self;
  i32 result;
  BOOL is_iconic;
  NativeWindow *owner_wnd;
  iptr surf_width;
  int src_dim;
  int dim_b;
  int paint_extent;
  int dest_extent;
  int yDest;
  LONG wDest;
  LONG hDest;
  HDC hdcSrc;
  int xSrc;
  int ySrc;
  DWORD rop;
  u8 *owner_ptr;
  tagPAINTSTRUCT ps;
  HDC hdc;
  tagRECT dest_rect;
  RECT paint_rect;
  
  self = (SurfaceWindow *)GetWindowLongA(hwnd,0xc);
  if (self == (SurfaceWindow *)0x0) {
    result = wnd_frame_wndproc(hwnd,msg,wparam,lparam);
  }
  else if (msg == 2) {
    self->DibSurface::create(0,0);
    SetWindowLongA(hwnd,0xc,0);
    result = wnd_frame_wndproc(hwnd,2,wparam,lparam);
  }
  else if (msg == 5) {
    if (self == (SurfaceWindow *)0x0) {
      (*(u8 * *)&modal_wnd.active_child) = (u8 *)0x0;
    }
    else {
      (*(u8 * *)&modal_wnd.active_child) = (u8 *)&self->window;
    }
    is_iconic = IsIconic(hwnd);
    if (is_iconic == 0) {
      self->on_resize(lparam & 0xffff,lparam >> 0x10);
    }
    result = wnd_frame_wndproc(hwnd,5,wparam,lparam);
  }
  else if (msg == 0xf) {
    is_iconic = IsIconic(hwnd);
    if (is_iconic == 0) {
      is_iconic = IsWindowVisible(hwnd);
      if (is_iconic != 0) {
        BeginPaint(hwnd,&ps);
        paint_rect.left = ps.rcPaint.left;
        paint_rect.top = ps.rcPaint.top;
        paint_rect.right = ps.rcPaint.right;
        paint_rect.bottom = ps.rcPaint.bottom;
        if (self == (SurfaceWindow *)0x0) {
          owner_ptr = (u8 *)0x0;
        }
        else {
          owner_ptr = (u8 *)&self->window;
        }
        self->blit_to_wnd((iptr)owner_ptr,&paint_rect,(iptr)&paint_rect);
        EndPaint(hwnd,&ps);
      }
    }
    else {
      owner_wnd = ((WndBase *)&self->window)->get_window();
      if (owner_wnd->icon != 0) {
        result = wnd_base_wndproc(hwnd,0xf,wparam,lparam);
        return result;
      }
      hdc = BeginPaint(hwnd,&ps);
      paint_rect.left = ps.rcPaint.left;
      paint_rect.top = ps.rcPaint.top;
      paint_rect.right = ps.rcPaint.right;
      paint_rect.bottom = ps.rcPaint.bottom;
      SetRect(&dest_rect,0,0,0,0);
      surf_width = self->get_width();
      src_dim = self->get_height();
      if (surf_width < src_dim) {
        src_dim = self->get_width();
        dim_b = rect_width(&paint_rect);
        dest_rect.right = self->get_height();
        dest_rect.right = (src_dim * dim_b) / dest_rect.right;
        dest_rect.bottom = rect_height(&paint_rect);
      }
      else {
        dest_rect.right = rect_width(&paint_rect);
        src_dim = self->get_height();
        dim_b = rect_height(&paint_rect);
        dest_rect.bottom = self->get_width();
        dest_rect.bottom = (src_dim * dim_b) / dest_rect.bottom;
      }
      src_dim = (int)self->get_dib();
      hdcSrc = *(HDC *)(src_dim + 4);
      rop = 0xcc0020;
      src_dim = self->get_height();
      dim_b = self->get_width();
      ySrc = 0;
      xSrc = 0;
      wDest = dest_rect.right;
      hDest = dest_rect.bottom;
      paint_extent = rect_height(&paint_rect);
      dest_extent = rect_height(&dest_rect);
      yDest = paint_extent - dest_extent >> 1;
      paint_extent = rect_width(&paint_rect);
      dest_extent = rect_width(&dest_rect);
      StretchBlt(hdc,paint_extent - dest_extent >> 1,yDest,wDest,hDest,hdcSrc,xSrc,ySrc,dim_b,src_dim,rop);
      EndPaint(hwnd,&ps);
    }
    result = 0;
  }
  else {
    result = wnd_frame_wndproc(hwnd,msg,wparam,lparam);
  }
  return result;
}

/* 005ea7a0 auto */
void __cdecl wnd_install_frame_proc(iptr owner,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,8,owner);
  SetWindowLongA(wnd->hwnd,-4,(iptr)wnd_frame_wndproc);
  return;
}

/* 005ea825 auto */
HWND __cdecl hwnd_next_sibling_wrap(HWND hwnd)

{
  BOOL is_window;
  HWND sibling;
  
  is_window = IsWindow(hwnd);
  if (is_window == 0) {
    sibling = (HWND)0x0;
  }
  else {
    sibling = GetWindow(hwnd,2);
    if (sibling == (HWND)0x0) {
      sibling = GetWindow(hwnd,0);
    }
  }
  return sibling;
}

/* 005ea87c auto */
HWND __cdecl hwnd_prev_sibling_wrap(HWND hwnd)

{
  BOOL is_window;
  HWND sibling;
  
  is_window = IsWindow(hwnd);
  if (is_window == 0) {
    sibling = (HWND)0x0;
  }
  else {
    sibling = GetWindow(hwnd,3);
    if (sibling == (HWND)0x0) {
      sibling = GetWindow(hwnd,1);
    }
  }
  return sibling;
}

/* 005ea8d3 auto */
void __cdecl wnd_cycle_control_focus(iptr wnd,HWND hwnd,int direction)

{
  NativeWindow *frame_win;
  BOOL is_child;
  NativeWindow *frame_win2;
  HWND start_hwnd;
  int result;
  Checkbox *self;
  UINT first_cmd;
  UINT uCmd;
  HWND group_hwnd;
  
  if (hwnd == (HWND)0x0) {
    hwnd = GetFocus();
    start_hwnd = hwnd;
    frame_win = ((WndBase *)wnd)->get_window();
    is_child = IsChild(frame_win->hwnd,start_hwnd);
    if (is_child == 0) {
      hwnd = (HWND)0x0;
    }
  }
  if (hwnd == (HWND)0x0) {
    if (*(int *)(wnd + 0xb8) == 0) {
      return;
    }
    if (direction == 2) {
      first_cmd = 5;
      frame_win2 = ((WndBase *)wnd)->get_window();
      hwnd = GetWindow(frame_win2->hwnd,first_cmd);
      start_hwnd = hwnd;
    }
    else {
      uCmd = 1;
      first_cmd = 5;
      result = (int)((WndBase *)wnd)->get_window();
      start_hwnd = GetWindow(((NativeWindow *)result)->hwnd,first_cmd);
      hwnd = GetWindow(start_hwnd,uCmd);
      start_hwnd = hwnd;
    }
  }
  else if (direction == 2) {
    hwnd = hwnd_next_sibling_wrap(hwnd);
    start_hwnd = hwnd;
  }
  else {
    hwnd = hwnd_prev_sibling_wrap(hwnd);
    start_hwnd = hwnd;
  }
  do {
    is_child = IsWindow(hwnd);
    if (is_child == 0) {
      return;
    }
    is_child = IsWindowVisible(hwnd);
    if (is_child == 0) {
      if (direction == 2) {
        hwnd = hwnd_next_sibling_wrap(hwnd);
      }
      else {
        hwnd = hwnd_prev_sibling_wrap(hwnd);
      }
    }
    else {
      result = ctrl_hwnd_is_radio(hwnd);
      if (result != 0) {
        group_hwnd = hwnd;
        while( true ) {
          is_child = IsWindow(hwnd);
          if (is_child == 0) {
            return;
          }
          result = radiogrp_hwnds_same_group(group_hwnd,hwnd);
          if (result == 0) break;
          result = radiogrp_hwnd_is_selected(hwnd);
          if (result != 0) {
            SetFocus(hwnd);
            return;
          }
          group_hwnd = hwnd;
          if (direction == 2) {
            hwnd = hwnd_next_sibling_wrap(hwnd);
          }
          else {
            hwnd = hwnd_prev_sibling_wrap(hwnd);
          }
        }
        wnd_cycle_control_focus(wnd,group_hwnd,direction);
        return;
      }
      self = (Checkbox *)ctrl_list_find_by_hwnd(hwnd,(Control *)*(i32 *)(wnd + 0xb8));
      if ((self != (Checkbox *)0x0) &&
         ((((result = self->get_type(), result == 6 &&
            (result = ((Button *)self)->ctrl_is_enabled(), result != 0)) ||
           ((result = self->get_type(), result == 2 &&
            (result = self->checkbox_is_enabled(), result != 0)))) ||
          (((result = self->get_type(), result == 4 ||
            (result = self->get_type(), result == 7)) ||
           (result = self->get_type(), result == 10)))))) {
        SetFocus(hwnd);
        return;
      }
      if (direction == 2) {
        hwnd = hwnd_next_sibling_wrap(hwnd);
      }
      else {
        hwnd = hwnd_prev_sibling_wrap(hwnd);
      }
    }
    if (hwnd == start_hwnd) {
      return;
    }
  } while( true );
}

/* 005eabcc auto */
void __cdecl wnd_focus_control(iptr wnd,iptr control)

{
  char nav_enabled;
  int ctrl_type;
  i32 *items;
  i32 start_hwnd;
  
  nav_enabled = ((Wnd *)wnd)->keyboard_nav_enabled();
  if (nav_enabled != '\0') {
    ctrl_type = ((Control *)control)->get_type();
    if (ctrl_type == 3) {
      items = (i32 *)((RadioGroup *)control)->get_items();
      start_hwnd = hwnd_prev_sibling_wrap(*items);
      wnd_cycle_control_focus(wnd,start_hwnd,2);
    }
    else {
      start_hwnd = ((Control *)control)->get_hwnd();
      start_hwnd = hwnd_prev_sibling_wrap(start_hwnd);
      wnd_cycle_control_focus(wnd,start_hwnd,2);
    }
  }
  return;
}

/* 005eb370 auto */
void __cdecl wnd_set_owner_ptr(iptr owner,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,0,owner);
  return;
}

/* 005eb393 auto */
void __cdecl wnd_install_command_proc(iptr owner,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,4,owner);
  SetWindowLongA(wnd->hwnd,-4,(iptr)wnd_child_wndproc);
  return;
}

/* 005ec1a1 auto */
void __cdecl wnd_on_menu_command(HWND hwnd,i32 cmd_id)

{
  char handled;
  ModalMenuWindow *self;
  HMENU hmenu;
  uint pos;
  
  self = (ModalMenuWindow *)GetWindowLongA(hwnd,4);
  handled = self->call_command(cmd_id);
  if (handled == '\0') {
    err_report_ctx_block.menu_search_pos = 0;
    hmenu = GetMenu(hwnd);
    if (hmenu != (HMENU)0x0) {
      pos = menu_find_item_pos(hmenu,cmd_id);
      self->call_menu_pos_command(pos & 0xffff,pos >> 0x10);
    }
  }
  return;
}

/* 005ec23a auto */
uint __cdecl menu_find_item_pos(HMENU hmenu,UINT item_id)

{
  int item_count;
  UINT sub_id;
  HMENU submenu;
  i32 result;
  int i;
  
  result = 0;
  err_report_ctx_block.menu_search_pos++;
  item_count = GetMenuItemCount(hmenu);
  for (i = 0; i < item_count; i++) {
    sub_id = GetMenuItemID(hmenu,i);
    if (sub_id == 0xffffffff) {
      submenu = GetSubMenu(hmenu,i);
      result = menu_find_item_pos(submenu,item_id);
      if (result != 0) {
        i = item_count + 1;
      }
    }
    else {
      sub_id = GetMenuItemID(hmenu,i);
      if (item_id == sub_id) {
        result = (i + 1) * 0x10000 | err_report_ctx_block.menu_search_pos - 1U;
        i = item_count + 1;
      }
    }
  }
  return result;
}

/* 005ecf50 auto */
void WndHandlers::call_mousemove(i32 x,i32 y)

{
  if (on_mouse_move != 0) {
    on_mouse_move(x,y);
  }
  return;
}

/* 005ecf90 auto */
void WndHandlers::call_lbuttondown(iptr x,iptr y)

{
  if (on_lbutton_down != 0) {
    on_lbutton_down(x,y);
  }
  return;
}

/* 005ecfd0 auto */
void WndHandlers::call_lbuttonup_raw(iptr x,iptr y)

{
  if (on_lbutton_up_raw != 0) {
    on_lbutton_up_raw(x,y);
  }
  return;
}

/* 005ed010 auto */
void WndHandlers::call_lbuttonup(i32 x,i32 y)

{
  if (on_lbutton_up != 0) {
    on_lbutton_up(x,y);
  }
  return;
}

/* 005ed050 auto */
void WndHandlers::call_rbuttondown(iptr x,iptr y)

{
  if (on_rbutton_down != 0) {
    on_rbutton_down(x,y);
  }
  return;
}

/* 005ed090 auto */
void WndHandlers::call_rbuttonup_raw(i32 x,i32 y)

{
  if (on_rbutton_up_raw != 0) {
    on_rbutton_up_raw(x,y);
  }
  return;
}

/* 005ed0d0 auto */
void WndHandlers::call_rbuttonup(i32 x,i32 y)

{
  if (on_rbutton_up != 0) {
    on_rbutton_up(x,y);
  }
  return;
}

/* 005ed110 auto */
void WndHandlers::call_dblclick(i32 x,i32 y)

{
  if (on_dblclick != 0) {
    on_dblclick(x,y);
  }
  return;
}

/* 005ed150 auto */
void WndHandlers::call_keydown_pre(i32 key)

{
  if (on_key_down_pre != 0) {
    on_key_down_pre(key);
  }
  return;
}

/* 005ed190 auto */
void WndHandlers::call_keyup(i32 key)

{
  if (on_key_up != 0) {
    on_key_up(key);
  }
  return;
}

/* 005ed1d0 auto */
void WndHandlers::call_keydown(i32 key)

{
  if (on_key_down != 0) {
    on_key_down(key);
  }
  return;
}

/* 005ed210 auto */
void WndHandlers::call_char(i32 ch)

{
  if (on_char != 0) {
    on_char(ch);
  }
  return;
}

/* 005ed250 auto */
i32 WndHandlers::call_close()

{
  i32 result;
  
  if (on_close == 0) {
    result = 0;
  }
  else {
    result = on_close();
  }
  return result;
}

/* 005ed290 auto */
void WndHandlers::call_minimize()

{
  if (on_minimize != 0) {
    on_minimize();
  }
  return;
}

/* 005ed2c0 auto */
void WndHandlers::call_restore()

{
  if (on_restore != 0) {
    on_restore();
  }
  return;
}

/* 005ed2f0 auto */
void WndHandlers::call_setfocus()

{
  if (on_set_focus != 0) {
    on_set_focus();
  }
  return;
}

/* 005ed320 auto */
void WndHandlers::call_size(i32 size_type)

{
  if (on_size != 0) {
    on_size(size_type);
  }
  return;
}

/* 005ed360 auto */
void WndHandlers::call_move(i32 x,i32 y)

{
  if (on_move != 0) {
    on_move(x,y);
  }
  return;
}

/* 005ed3a0 auto */
void WndHandlers::call_msg44(iptr notify_arg)

{
  if (on_msg_44 != 0) {
    on_msg_44(notify_arg);
  }
  return;
}

/* 005ed3e0 auto */
void WndHandlers::call_hscroll(i32 scroll_pos)

{
  if (on_hscroll != 0) {
    on_hscroll(scroll_pos);
  }
  return;
}

/* 005ed420 auto */
void WndHandlers::call_hscroll_track(i32 scroll_pos)

{
  if (on_hscroll_track != 0) {
    on_hscroll_track(scroll_pos);
  }
  return;
}

/* 005ed460 auto */
void WndHandlers::call_vscroll(i32 scroll_pos)

{
  if (on_vscroll != 0) {
    on_vscroll(scroll_pos);
  }
  return;
}

/* 005ed4a0 auto */
void WndHandlers::call_vscroll_track(i32 scroll_pos)

{
  if (on_vscroll_track != 0) {
    on_vscroll_track(scroll_pos);
  }
  return;
}

/* 005ed4e0 auto */
void WndHandlers::call_msg4c8(i32 msg_arg)

{
  if (on_msg_4c8 != 0) {
    on_msg_4c8(msg_arg);
  }
  return;
}

/* 005ed520 auto */
void WndHandlers::call_entersizemove()

{
  if (on_enter_size_move != 0) {
    on_enter_size_move();
  }
  return;
}

/* 005ed550 auto */
void WndHandlers::call_exitsizemove()

{
  if (on_exitsizemove != 0) {
    on_exitsizemove();
  }
  return;
}

/* 005ed580 auto */
void ModalMenuWindow::on_size_nop(i32 width,i32 height)

{
  return;
}

/* 005ed5a0 auto */
bool ModalMenuWindow::call_menu_pos_command(i32 menu_pos,i32 item_pos)

{
  bool has_handler;
  
  has_handler = on_menu_pos_command != 0;
  if (has_handler) {
    on_menu_pos_command(menu_pos,item_pos);
  }
  return has_handler;
}

/* 005ed5f0 auto */
bool ModalMenuWindow::call_command(i32 cmd_id)

{
  bool has_handler;
  
  has_handler = on_command != 0;
  if (has_handler) {
    on_command(cmd_id);
  }
  return has_handler;
}

/* 005ed640 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 ModalMenuWindow::get_hscroll_pos()

{
  return hscroll_pos;
}

/* 005ed660 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 ModalMenuWindow::get_vscroll_pos()

{
  return vscroll_pos;
}

/* 005ed680 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 ModalMenuWindow::get_hscroll_page()

{
  return hscroll_step;
}

/* 005ed6a0 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 ModalMenuWindow::get_vscroll_page()

{
  return vscroll_step;
}

/* 005ed6c0 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 __thiscall wnd_get_mci_device(WndMciDeviceView *self)

{
  return self->mci_device;
}

/* 005ed6e0 auto */
void __thiscall wnd_call_mci_done(WndOnMciDoneView *self)

{
  if (self->on_mci_done != 0) {
    self->on_mci_done();
  }
  return;
}

/* 005ef5db auto */
i32 __cdecl ddwin_destroy_native(i32 *ddwin)

{
  i32 mem_handle;
  
  if (ddwin != (i32 *)0x0) {
    if (ddwin[5] == 0) {
      if (ddwin[1] != 0) {
        ShowWindow((HWND)ddwin[1],0);
        DestroyWindow((HWND)ddwin[1]);
      }
      ddwin[1] = 0;
    }
    mem_handle = *ddwin;
    mem_handle_unlock(mem_handle);
    mem_handle_free(mem_handle);
  }
  return 0;
}

/* 005ef6cb auto */
LRESULT __stdcall ddwin_native_wndproc(HWND hwnd,uint msg,uint wparam,uint lparam)

{
  char handled;
  DDWindow *self;
  LRESULT def_result;
  i32 key;
  int can_close;
  uptr wnd_obj;
  i32 zero_arg;
  
  self = (DDWindow *)GetClassLongA(hwnd,4);
  if (self == (DDWindow *)0x0) {
    def_result = DefWindowProcA(hwnd,msg,wparam,lparam);
    return def_result;
  }
  if (msg < 0x11) {
    if (msg != 0x10) {
      if (msg == 2) {
        wnd_obj = GetClassLongA(hwnd,0);
        *(i32 *)(wnd_obj + 0x14) = 1;
        def_result = DefWindowProcA(hwnd,2,wparam,lparam);
        return def_result;
      }
      def_result = DefWindowProcA(hwnd,msg,wparam,lparam);
      return def_result;
    }
    can_close = ((WndHandlers *)self->handlers)->call_close();
    if (can_close != 0) {
      def_result = DefWindowProcA(hwnd,0x10,wparam,lparam);
      return def_result;
    }
  }
  else if (msg < 0x45) {
    if (msg != 0x44) {
      if (msg != 0x1c) {
        def_result = DefWindowProcA(hwnd,msg,wparam,lparam);
        return def_result;
      }
      if (wparam != ddwin_last_activate_app) {
        self->call_activate_app(wparam);
        ddwin_last_activate_app = wparam;
      }
    }
  }
  else if (msg < 0x201) {
    if (msg == 0x200) {
      handled = self->dispatch_ctrl_msg(0x200,lparam & 0xffff,lparam >> 0x10);
      if (handled == '\0') {
        ((WndHandlers *)self->handlers)->call_mousemove(lparam & 0xffff,lparam >> 0x10);
      }
    }
    else if (msg == 0x100) {
      zero_arg = 0;
      key = input_map_vkey(wparam);
      handled = self->dispatch_ctrl_msg(0x100,key,zero_arg);
      if (handled == '\0') {
        key = input_map_vkey(wparam);
        ((WndHandlers *)self->handlers)->call_keydown_pre(key);
        key = input_map_vkey(wparam);
        ((WndHandlers *)self->handlers)->call_keydown(key);
      }
    }
    else if (msg == 0x101) {
      key = input_map_vkey(wparam);
      ((WndHandlers *)self->handlers)->call_keyup(key);
    }
    else {
      if (msg != 0x102) {
        def_result = DefWindowProcA(hwnd,msg,wparam,lparam);
        return def_result;
      }
      handled = self->dispatch_ctrl_msg(0x102,wparam & 0xff,0);
      if (handled == '\0') {
        ((WndHandlers *)self->handlers)->call_char(wparam & 0xff);
      }
    }
  }
  else if (msg < 0x211) {
    if (msg == 0x210) {
      if ((wparam & 0xffff) == 0x201) {
        ((WndHandlers *)self->handlers)->call_lbuttondown(lparam & 0xffff,lparam >> 0x10);
      }
      else if ((wparam & 0xffff) == 0x204) {
        ((WndHandlers *)self->handlers)->call_rbuttondown(lparam & 0xffff,lparam >> 0x10);
      }
    }
    else {
      switch(msg) {
      case 0x201:
        handled = self->dispatch_ctrl_msg(msg,lparam & 0xffff,lparam >> 0x10);
        if (handled == '\0') {
          ((DDWindowExt *)self)->lbutton_held[0] = 1;
          ((DDWindowExt *)self)->lbutton_held_byte1[0] = '\0';
          ((DDWindowExt *)self)->lbutton_held_byte2[0] = '\0';
          ((DDWindowExt *)self)->lbutton_held_byte3[0] = '\0';
          ((WndHandlers *)self->handlers)->call_lbuttondown(lparam & 0xffff,lparam >> 0x10);
        }
        break;
      case 0x202:
        handled = self->dispatch_ctrl_msg(msg,lparam & 0xffff,lparam >> 0x10);
        if (handled == '\0') {
          if (*(int *)((DDWindowExt *)self)->lbutton_held == 1) {
            ((DDWindowExt *)self)->lbutton_held[0] = '\0';
            ((DDWindowExt *)self)->lbutton_held_byte1[0] = '\0';
            ((DDWindowExt *)self)->lbutton_held_byte2[0] = '\0';
            ((DDWindowExt *)self)->lbutton_held_byte3[0] = '\0';
            ((WndHandlers *)self->handlers)->call_lbuttonup(lparam & 0xffff,lparam >> 0x10);
          }
          ((WndHandlers *)self->handlers)->call_lbuttonup_raw(lparam & 0xffff,lparam >> 0x10);
        }
        break;
      case 0x203:
        handled = self->dispatch_ctrl_msg(msg,lparam & 0xffff,lparam >> 0x10);
        if (handled == '\0') {
          ((WndHandlers *)self->handlers)->call_dblclick(lparam & 0xffff,lparam >> 0x10);
        }
        break;
      case 0x204:
        handled = self->dispatch_ctrl_msg(msg,lparam & 0xffff,lparam >> 0x10);
        if (handled == '\0') {
          ((DDWindowExt *)self)->rbutton_held[0] = 1;
          ((DDWindowExt *)self)->rbutton_held_byte1[0] = '\0';
          ((DDWindowExt *)self)->rbutton_held_byte2[0] = '\0';
          ((DDWindowExt *)self)->rbutton_held_byte3[0] = '\0';
          ((WndHandlers *)self->handlers)->call_rbuttondown(lparam & 0xffff,lparam >> 0x10);
        }
        break;
      case 0x205:
        handled = self->dispatch_ctrl_msg(msg,lparam & 0xffff,lparam >> 0x10);
        if (handled == '\0') {
          if (*(int *)(((DDWindowExt *)self)->rbutton_held) == 1) {
            ((DDWindowExt *)self)->rbutton_held[0] = '\0';
            ((DDWindowExt *)self)->rbutton_held_byte1[0] = '\0';
            ((DDWindowExt *)self)->rbutton_held_byte2[0] = '\0';
            ((DDWindowExt *)self)->rbutton_held_byte3[0] = '\0';
            ((WndHandlers *)self->handlers)->call_rbuttonup(lparam & 0xffff,lparam >> 0x10);
          }
          ((WndHandlers *)self->handlers)->call_rbuttonup_raw(lparam & 0xffff,lparam >> 0x10);
        }
        break;
      default:
        def_result = DefWindowProcA(hwnd,msg,wparam,lparam);
        return def_result;
      }
    }
  }
  else {
    if (msg != 0x4c8) {
      def_result = DefWindowProcA(hwnd,msg,wparam,lparam);
      return def_result;
    }
    if (lparam == 0x20d) {
      ((WndHandlers *)self->handlers)->call_msg4c8(0);
    }
  }
  return 0;
}

/* 005efd70 auto */
void DDWindow::call_activate_app(i32 active)

{
  if (on_activate_app != 0) {
    on_activate_app(active);
  }
  return;
}

/* 005efeb0: written by hand in src/game/unwind.cpp */

/* 005f029e auto */
void DDWindow::remove_all_controls()

{
  i32 ctrl;
  
  ctrl = ((PtrList *)child_controls)->first();
  while (ctrl != 0) {
    ((PtrList *)child_controls)->remove_current();
    ctrl = ((PtrList *)child_controls)->first();
  }
  return;
}

/* 005f0342 auto */
void DDWindow::deactivate_controls()

{
  i32 ctrl;
  
  ctrl = ((PtrList *)child_controls)->first();
  while (ctrl != 0) {
    ((DDControl *)ctrl)->deactivate();
    ctrl = ((PtrList *)child_controls)->next();
  }
  return;
}

/* 005f0391 auto */
i8 DDWindow::dispatch_ctrl_msg(int msg,int x,int y)

{
  int *rect;
  int handled;
  DDControl *ctrl;
  
  if ((hover_control != 0) && (msg == 0x200)) {
    rect = (int *)((DDControl *)hover_control)->get_rect();
    if ((x < *rect) ||
       (((rect[2] < x || (y < rect[1])) || (rect[3] < y)))) {
      ((DDControl *)hover_control)->dispatch_msg(0xfff0,x,y);
      hover_control = 0;
    }
  }
  ctrl = (DDControl *)((PtrList *)child_controls)->first();
  while( true ) {
    if (ctrl == (DDControl *)0x0) {
      return 0;
    }
    handled = ctrl->dispatch_msg(msg,x,y);
    if (handled != 0) break;
    ctrl = (DDControl *)((PtrList *)child_controls)->next();
  }
  if (msg == 0x200) {
    hover_control = (uptr)ctrl;
  }
  return 1;
}
