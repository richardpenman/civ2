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
void WndBase::get_rect_in_parent(RECT *rect)

{
  wnd_obj_get_rect_in_parent(native_wnd,rect);
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
void DibSurface::wnd_blit_surface_rect(RECT *rect)

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
    blit_to_wnd((WndBase *)view,(RECT *)&bounds_rect,(RECT *)&bounds_rect);
    ((WndBase *)(this + 1))->view_validate_rect((RECT *)&bounds_rect);
  }
  else {
    if (this == (DibSurface *)0x0) {
      view2 = (DibSurface *)0x0;
    }
    else {
      view2 = this + 1;
    }
    blit_to_wnd((WndBase *)view2,rect,rect);
    ((WndBase *)(this + 1))->view_validate_rect(rect);
  }
  return;
}

/* 00408580 auto */
void WndBase::view_validate_rect(RECT *rect)

{
  wnd_obj_validate_rect(native_wnd,rect);
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
  window.show_focus_first();
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
i32 Dialog::show_modal(int timeout_secs)

{
  i32 result;
  
  result = run_modal(0,timeout_secs);
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
    /* BF-101 (Peter Johnson's ledger): a choice past the 256 cities (the 257th
       row of a GAME.TXT @FINDCITY section) is no choice */
    if ((city2 >= 0) && (city2 < 0x100)) {
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
        choice = popup_game_with_sprite(str_revolution,(Sprite *)(((i8 *)&government_sprites_block.government_sprites) +
                                                  (uint)civs[civ].government * 0x3c),
                                   (ui_metrics.large_screen == 0) - 1 & 8);
        if (choice == 0) {
          snd_play_effect(SND_REVOLT,1,0,0);
          text = (iptr)(civ_name_plural(civ));
          text_set_string_arg(0,(const char *)text);
          popup_game_with_sprite(str_startrev,(Sprite *)&popup_info_icon_sprite,0);
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
i32 __cdecl popup_game_with_sprite(const char *section,Sprite *sprite,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_sprite(str_game_7,section,0,sprite,sprite_flags);
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
void WndBase::set_cursor_now(int cursor,i32 flag)

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
i32 __cdecl popup_game_with_city(const char *section,int city)

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
  
  result = popup_text_run(filename,section,dlg_flags,0,0,0,flags);
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
i32 __cdecl dlg_game_text_input(const char *section,i32 max_len,char *buf,const char *default_text)

{
  i32 result;
  
  result = dlg_text_input(str_game_7,section,max_len,buf,(char *)default_text);
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
i32 __cdecl popup_game_sprite_flags(const char *section,i32 dlg_flags,Sprite *sprite,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_sprite(str_game_7,section,dlg_flags,sprite,sprite_flags);
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
  ui_set_button_font(&advisor_button_font_block.advisor_button_font);
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
    surface.load_gif_file((const char *)&str_scredits_gif,10,0xc0,&palette)
    ;
  }
  else {
    surface.load_gif_resource(resource_id,10,0xc0,&palette);
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
  gfx_copy_rect_xywh((DibSurface *)&surface_head,this,x,y,client_x + x,
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
  /* inlined unwind funclet 0043c19c */ { scrollbar.destruct(); }
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 0043c1ab */ { ctrl_dtor(&button_4); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0043c1ba */ { ctrl_dtor(&button_3); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0043c1c9 */ { ctrl_dtor(&button_2); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0043c1d8 */ { ctrl_dtor(&close_button); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 0043c1e7 */ { surface_dtor(&surface); }
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
void Dialog::set_list_indent(uptr indent,int index)

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
i32 __cdecl dlg_game_txt_with_unit_flags(const char *section,int unit,i32 flags)

{
  i32 result;
  
  result = popup_text_with_unit_sprite((const char *)&str_game_7,section,0,unit,flags);
  return result;
}

/* 004442e0 auto */
i32 __cdecl dlg_game_txt_with_unit(const char *section,int unit)

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
void WndBase::set_palette(GamePalette *palette)

{
  wnd_palette = (u32)palette;
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
void WndBase::apply_palette_handle(GamePalette *palette)

{
  wnd_obj_select_palette(native_wnd,(i32)(palette)->hpalette);
  return;
}

/* 00451890 auto */
void WndBase::get_cursor_pos(i32 *out_x,i32 *out_y)

{
  NativeWindow *hwnd;
  
  hwnd = get_window();
  wnd_obj_get_cursor_pos(hwnd,out_x,out_y);
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
void Hotspot::create(Wnd *parent,int id,RECT *rect)

{
  u32 new_hwnd;
  
  if (hwnd != 0) {
    destroy();
  }
  init(parent,1,id,rect);
  on_enter = 0;
  on_leave = 0;
  on_click = 0;
  on_press = 0;
  on_dblclick = 0;
  new_hwnd = hotspot_create_hwnd((int *)rect,this);
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
ui_file_name_dialog(WndBase *parent,const char *title,char *file_buf,const char *filter,const char *default_ext,int must_exist,
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
    wnd = (parent)->get_window();
    ofn.hwndOwner = wnd->hwnd;
  }
  ofn.lpstrFilter = filter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile = file_buf;
  ofn.nMaxFile = 0x100;
  ofn.lpstrTitle = title;
  ofn.lpstrDefExt = default_ext;
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
  selectable = unit_is_selectable(game.active_unit);
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
      mapviews_ensure_visible((int)map_cursor.x[0],(int)map_cursor.y[0],units[unit].owner);
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
void __cdecl popup_file_with_sprite(const char *filename,const char *section,Sprite *sprite,i32 sprite_flags)

{
  popup_text_with_sprite(filename,section,0,sprite,sprite_flags);
  return;
}

/* 00490500 auto */
void __cdecl popup_text_with_city_noflags(const char *filename,const char *section,i32 city)

{
  popup_text_with_city(filename,section,0,city);
  return;
}

/* 00490530 auto */
void __cdecl popup_text_with_unit_noflags(const char *filename,const char *section,int unit)

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
popup_text_with_advance(const char *filename,const char *section,i32 dlg_flags,int advance,i32 sprite_flags)

{
  popup_text_with_sprite(filename,section,dlg_flags,
                     (Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
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
  result = popup_text_with_sprite(filename,section,dlg_flags,(Sprite *)sprite,sprite_flags);
  return result;
}

/* 004a6c4b auto */
i32 __cdecl
popup_text_with_unit_sprite(const char *filename,const char *section,i32 dlg_flags,int unit_type,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_sprite(filename,section,dlg_flags,((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c)),sprite_flags)
  ;
  return result;
}

/* 004a6c85 auto */
void __cdecl
popup_city_sprite_draw_cb(i32 dlg,DibSurface *surface,int city,i32 left,int x,
            i32 y)

{
  gfx_draw_city(surface,city,0,x + 2,y,(ui_metrics.large_screen == 0) - 1 & 8);
  return;
}

/* 004a6cc5 auto */
i32 __cdecl popup_text_with_city(const char *filename,const char *section,i32 dlg_flags,int city)

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
  dialog2.add_sprite(&sprite,city,0);
  result = dialog2.show_modal(0);
  (*(u32 *)&dlg_input.help_pressed) = dialog2.result_extra;
  return result;
}

/* 004a6df7 auto */
void __cdecl
popup_unit_sprite_draw_cb(i32 dlg,DibSurface *surface,int unit,i32 left,int x,
            i32 y)

{
  gfx_draw_unit(surface,unit,4,x + 2,y,(ui_metrics.large_screen == 0) - 1 & 8,0);
  return;
}

/* 004a6e39 auto */
i32 __cdecl popup_text_with_unit(const char *filename,const char *section,i32 dlg_flags,int unit)

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
  dialog2.add_sprite(&sprite,unit,0);
  result = dialog2.show_modal(0);
  (*(u32 *)&dlg_input.help_pressed) = dialog2.result_extra;
  return result;
}

/* 004a93b3 auto */
void __cdecl accel_start_develop_city(int city,int num_tiles)

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
    tile_update_known(x,y,cities[city].owner);
  }
  civ = cities[city].owner;
  for (tile = 0; tile < 0x2d; tile++) {
    x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[tile] + x2);
    y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[tile] + y2;
    result = map_in_bounds(x,y);
    if ((result != 0) && (result = tile_is_ocean(x,y), result == 0)) {
      result = tile_landmass(cities[city].x,cities[city].y);
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
void WndBase::move_to_rect(RECT *rect)

{
  wnd_obj_move_to_rect(native_wnd,(int *)rect);
  return;
}

/* 004c4240 auto */
void __cdecl popup_game_with_advance(const char *section,int advance,i32 sprite_flags)

{
  popup_text_with_advance((const char *)&str_game_7,section,0,advance,sprite_flags);
  return;
}

/* 004cc870 auto */
i32 __cdecl popup_game_with_improvement(const char *section,int improvement,i32 sprite_flags)

{
  i32 result;
  
  result = popup_text_with_improvement((const char *)&str_game_7,section,0,improvement,sprite_flags);
  return result;
}

/* 004cc8b0 auto */
i32 __cdecl popup_game_with_unit(const char *section,i32 dlg_flags,int unit)

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

/* 004e7240 auto */
void WndBase::show_restored()

{
  wnd_obj_show_restored(native_wnd);
  return;
}

/* 004eb80a auto */
int __cdecl popup_city_picture_zoom(const char *section,int city,uint picture,int silent,int civ)

{
  int result;
  i32 height;
  i32 width;
  iptr option_text;
  iptr option_id;
  uint picture_id;
  Sprite sprite;
  CityPicturePopup *popup;
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
    popup = game_new<CityPicturePopup>();
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
    popup->surface.load_gif_resource(picture_id,10,0xc0,&palette);
    height = popup->surface.surface_height();
    width = popup->surface.surface_width();
    sprite.grab_from_surface(&popup->surface,7,0,0,width,height);
    popup->dialog.load_game_text(section);
    popup->dialog.add_sprite(&sprite,0,0);
    if ((silent == 0) && (city_calc.zoom_requested == 0)) {
      height = 0;
      option_id = 1;
      option_text = (iptr)label_string(label_table.string_ids[0x20]);
      popup->dialog.add_option((const char *)option_text,option_id,height);
      height = 0;
      option_id = 0;
      option_text = (iptr)label_string(label_table.string_ids[0x21]);
      popup->dialog.add_option((const char *)option_text,option_id,height);
      if ((game.message_options & MSGOFF_NO_ZOOM_TO_CITY) == 0) {
        popup->dialog.set_default_choice(0);
      }
      else {
        popup->dialog.set_default_choice(1);
      }
    }
    result = popup->dialog.show_modal(0);
    if ((silent == 0) && (0 < result)) {
      city_calc.zoom_requested = 1;
    }
    if (popup != (CityPicturePopup *)0x0) {
      popup->scalar_destruct(1);
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
void ClickMap::hotspot_add_rect(int id,i32 cursor,RECT *rect)

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

void FrameWindow::set_title_font(GameFont *font)

{
  title_font = (uptr)font;
  return;
}

/* 00511850 auto */
/* Library Function - Single Match
    public: void __thiscall CPropertySheet::EnableStackedTabs(int)
   
   Library: Visual Studio 1998 Debug */

void FrameWindow::set_title_small_font(GameFont *font)

{
  title_small_font = (uptr)font;
  return;
}

/* 00518e80 auto */
void WndBase::move_xy(i32 x,i32 y)

{
  wnd_obj_move_xy(native_wnd,x,y);
  return;
}

/* 00518ec0 auto */
i32 __cdecl dlg_input_number_game(const char *section,i32 initial,long *out_value)

{
  i32 result;
  
  result = dlg_input_number(str_game_7,section,initial,out_value);
  return result;
}

/* 0051d3e0 auto */
i32 __cdecl
popup_text_run(const char *file,const char *section,i32 result,i32 arg4,i32 arg5,
            i32 arg6,i32 arg7)

{
  int status;
  Dialog dialog2;
  int i;
  i32 eh_prev;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  status = dialog2.load_text_section(file,section,result,0,(SurfaceWindow *)arg4,(DibSurface *)arg5,(RECT *)arg6,arg7);
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
popup_text_with_sprite(const char *file,const char *section,i32 flags,Sprite *sprite,i32 zoom)

{
  i32 result;
  Dialog dialog2;
  i32 eh_prev;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  dialog2.load_text(file,section,flags);
  dialog2.add_sprite(sprite,0,0);
  dialog2.set_image_zoom(zoom);
  result = dialog2.show_modal(0);
  (*(u32 *)&dlg_input.help_pressed) = dialog2.result_extra;
  return result;
}

/* 0051d63b auto */
i32 __cdecl
dlg_text_input(const char *file,const char *section,i32 max_len,const char *initial_text,char *out_text)

{
  int status;
  i32 result;
  Dialog dialog2;
  char *p;
  i32 eh_prev;
  
  result = 0;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  str_clear(out_text);
  status = dialog2.load_text_section(file,section,max_len,initial_text,0,0,0,1);
  if (status == 0) {
    result = dialog2.run_modal(out_text,0);
    for (p = out_text;
        (*p != '\0' && ((*p == ' ' || (*p == '\t')))); p++)
    {
      *p = '_';
    }
  }
  return result;
}

/* 0051d75d auto */
i32 __cdecl dlg_input_number(const char *file,const char *section,int initial,long *out_value)

{
  long value;
  char input_text[260];
  char initial_text[20];
  i32 result;
  
  __itoa(initial,initial_text,10);
  result = dlg_text_input(file,section,5,initial_text,input_text);
  value = _atol(input_text);
  *out_value = value;
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
void __cdecl dialog_prepend_title_label(Dialog *dialog,i32 label_id)

{
  size_t len;
  i32 buffer;
  char title[0x200];  /* BF-061 (Peter Johnson's ledger): the original's 128,
                         less than a label and the dialog's title; a longer
                         title is cut */
  
  title[0] = '\0';
  str_append_label(title,label_id);
  if (dialog->title_text != 0) {
    str_append_max(title,((i8 *)&dialog_title_label_space_str),sizeof title);
    str_append_max((char *)((void *)title),(const char *)((void *)dialog->title_text),sizeof title);
  }
  len = _strlen(title);
  buffer = memblock_alloc((MemBlock *)dialog->string_pool,len + 1);
  dialog->title_text = buffer;
  crt_strcpy((char *)((void *)dialog->title_text),(const char *)((void *)title));
  return;
}

/* 00548b70 auto */
int __cdecl ui_calc_item_spacing(int count,int item_size,int total_size,int *out_count,int *out_remainder)

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
      *out_remainder = 0;
    }
    if (out_count != 0) {
      *out_count = 1;
    }
  }
  else {
    free_space = total_size - item_size;
    spacing = free_space / (count - 1);
    if (out_remainder != 0) {
      *out_remainder = free_space % (count - 1);
    }
    if (out_count != 0) {
      *out_count = count;
    }
    if ((spacing < item_size) && (result = spacing, spacing < 1)) {
      if (out_remainder != 0) {
        *out_remainder = 0;
      }
      if (out_count != 0) {
        *out_count = (free_space - item_size) + 1;
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
  
  gfx_get_screen_rect((RECT *)&caption_small_font_block.screen_rect);
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
void __cdecl frame_set_border_tile(DibSurface *tile)

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
  gfx_tile_fill(this,(DibSurface *)ui_metrics.border_tile,0,bevel,width2,height2,0,bevel);
  height2 = border_width + bevel * -2;
  offset = rect_height((RECT *)&frame_rect);
  offset -= (bevel + height2);
  gfx_tile_fill(this,(DibSurface *)ui_metrics.border_tile,0,offset,width2,height2,0,offset);
  width2 = border_width + bevel * -2;
  height2 = rect_height((RECT *)&frame_rect);
  gfx_tile_fill(this,(DibSurface *)ui_metrics.border_tile,bevel,0,width2,height2,bevel,0);
  offset = rect_width((RECT *)&frame_rect);
  offset -= (bevel + width2);
  gfx_tile_fill(this,(DibSurface *)ui_metrics.border_tile,offset,0,width2,height2,offset,0);
  rect2.left = frame_rect;
  rect2.top = frame_top_u;
  rect2.right = frame_right_u;
  rect2.bottom = frame_bottom;
  for (i = 0; i < bevel; i++) {
    gfx_draw_bevel_rect(this,(RECT *)&rect2,highlight_color2,shadow_color2);
    rect_deflate((RECT *)&rect2,1,1);
  }
  rect_deflate((RECT *)&rect2,border_width + bevel * -2,0);
  if (caption_height == 0) {
    caption_h2 = border_width;
  }
  else {
    caption_h2 = caption_height;
  }
  rect2.top = rect2.top + caption_h2 + bevel * -2;
  rect2.bottom -= (border_width + bevel * -2);
  for (i = 0; i < bevel; i++) {
    gfx_draw_bevel_rect(this,(RECT *)&rect2,shadow_color2,highlight_color2);
    rect_deflate((RECT *)&rect2,1,1);
  }
  button_x = ui_metrics.border_width;
  for (i = 0; i < (int)caption_button_count; i++) {
    width2 = (int)(caption_buttons[i].width * caption_height) / 0x18;
    height2 = (int)(caption_buttons[i].height * caption_height) / 0x18;
    button_y2 = ((int)caption_height >> 1) - (height2 >> 1);
    rect_set_xywh((RECT *)&rect2,button_x,button_y2,width2,height2);
    gfx_set_scale_ratio(caption_height,0x18);
    caption_buttons[i].sprite->draw((RECT *)sprite_rect,
               this,button_x,button_y2);
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
        str_append_label(caption,0x369);
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
    str_append_label(caption,0x369);
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
  char caption[0x240];  /* BF-204 (Peter Johnson's ledger): the original's 256,
                           less than the title, a player's name and a label */
  
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
  draw_text(font2,(const char *)caption,text_x2 + 2,text_y2 + 1,5);
  gfx_set_text_color_index(0x1a);
  draw_text(font2,(const char *)caption,text_x2 + 1,text_y2,5);
  draw_text(font2,(const char *)caption,text_x2,text_y2,5);
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
    ((FrameWindow *)frame_wnd)->on_caption_button(((FrameWindow *)frame_wnd)->caption_buttons[button_id - 1000].id);
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
  rect_set_xywh((RECT *)&frame_rect,0,0,width,height);
  rect_set_xywh((RECT *)&client_rect,0,0,width + border_width * -2,
                     height - (caption_height + border_width));
  rect_offset((RECT *)&client_rect,border_width,caption_height);
  client_x = client_rect;
  client_y = client_top_u;
  size = rect_width((RECT *)&client_rect);
  client_width = size;
  size = rect_height((RECT *)&client_rect);
  client_height = size;
  destroy_caption_buttons();
  button_x = ui_metrics.border_width;
  for (i = 0; i < (int)caption_button_count; i++) {
    width = (int)(caption_buttons[i].width * caption_height) / 0x18;
    height = (int)(caption_buttons[i].height * caption_height) / 0x18;
    rect_set_xywh((RECT *)button_rect,button_x,((int)caption_height >> 1) - (height >> 1),width,
                       height);
    this_00 = (Hotspot *)operator_new(0x40);
    if (this_00 == (Hotspot *)0x0) {
      hotspot = 0;
    }
    else {
      hotspot = hotspot_ctor(this_00);
    }
    caption_buttons[i].hotspot = (u32)hotspot;
    if (this == (FrameWindow *)0x0) {
      native_wnd2 = (u8 *)0x0;
    }
    else {
      native_wnd2 = (u8 *)&window;
    }
    ((Hotspot *)caption_buttons[i].hotspot)->create((Wnd *)native_wnd2,
               i + 1000,(RECT *)button_rect);
    ((Hotspot *)caption_buttons[i].hotspot)->set_click_callback(frame_wnd_on_caption_button_click);
    button_x = button_x + width + ui_metrics.padding;
  }
  return;
}

/* 0055318c auto */
void FrameWindow::add_caption_button(Sprite *sprite,i32 callback)

{
  u32 index;
  i32 dim;
  
  if ((int)caption_button_count < 6) {
    index = caption_button_count;
    caption_button_count++;
    caption_buttons[index].sprite = sprite;
    caption_buttons[index].id = callback;
    dim = (sprite)->width();
    caption_buttons[index].width = dim;
    dim = (sprite)->height();
    caption_buttons[index].height = dim;
    caption_buttons[index].hotspot = 0;
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
  wnd_blit_surface_rect((RECT *)&rect2);
  return;
}

/* 005532d7 auto */
void FrameWindow::destroy_caption_buttons()

{
  int i;
  
  for (i = 0; i < 6; i++) {
    if (caption_buttons[i].hotspot != 0) {
      if ((Hotspot *)caption_buttons[i].hotspot != (Hotspot *)0x0) {
        ((Hotspot *)caption_buttons[i].hotspot)->scalar_destruct(1);
      }
      caption_buttons[i].hotspot = 0;
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
  int i;
  i32 eh_prev;
  
  SurfaceWindow::construct();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 7);
  for (i = 0; i < 6; i++) {
    caption_buttons[i].hotspot = 0;
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
  wnd_create_with_surface(0,wnd_style,x,y,width,height,(GamePalette *)&game_palette_block.game_palette,&main_frame_wnd_block.main_frame_wnd);
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
    dll.resource_dll_load(path);
    ON_SCOPE_EXIT(dll.free());
    ok = surface.load_gif_resource(999,10,0xec,(GamePalette *)&art_palette);
    if (ok == 0) {
      MessageBoxA((HWND)0x0,str_failed_to_load_civ2art_gif_2,str_notice_15,0x40);
    }
    else {
      sprite.grab_from_surface(&surface,2,1,1,0x40,0x20);
      ((DibSurface *)((i8 *)&frame_texture_surface_block.frame_texture_surface))->DibSurface::create(0x40,0x20);
      sprite.draw((RECT *)sprite_rect,(DibSurface *)&frame_texture_surface_block.frame_texture_surface,0,0);
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
  MenuNode *cur;
  
  index = 1;
  for (cur = item->link->link; (cur != (MenuNode *)0x0 && (item != cur));
      cur = cur->next) {
    if ((cur->flags_byte & 2) == 0) {
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
      item->field_0c = (i32)(buf_len + 1);   /* (port) its size, for set_item_text */
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
    window->attach_menu(this);
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
void GameMenu::rebuild(ModalMenuWindow *window)

{
  MenuNode *menu2;
  int menu_index;
  int item_index;
  MenuNode *item;
  int i;
  i8 spec[256];
  
  if ((menu_flags & 0x8000) == 0) {
    spec[0] = 0;
    menu2 = menus;
    menu_index = 1;
    for (; menu2 != (MenuNode *)0x0; menu2 = menu2->next) {
      if ((menu2->flags_byte & 2) == 0) {
        crt_strcat((char *)((void *)spec),(const char *)((void *)menu2->text));
        crt_strcat(spec,((i8 *)&menu_title_sep_str));
      }
      if (window == 0) {
        for (i = 0x1e; 0 < i; i--) {
          native_delete(menu_index,i);
        }
        menu2->field_0c = 0;
      }
      menu_index++;
    }
    if (window != 0) {
      native_build(spec);
    }
    menu2 = menus;
    menu_index = 1;
    for (; menu2 != (MenuNode *)0x0; menu2 = menu2->next) {
      if ((menu2->flags_byte & 2) == 0) {
        if (window != 0) {
          native_delete(menu_index,1);
        }
        item = menu2->link;
        item_index = 1;
        for (; item != (MenuNode *)0x0; item = item->next) {
          if ((item->flags_byte & 2) == 0) {
            menu2->field_0c = menu2->field_0c + 1;
            native_insert(menu_index,item_index,(const char *)item->text);
            if ((item->flags_byte & 1) != 0) {
              apply_item_enabled(item->id);
            }
            if ((item->flags_byte & 4) != 0) {
              apply_item_checked(item->id);
            }
            item_index++;
          }
        }
        menu_index++;
      }
    }
    menu_flags = menu_flags | 0x8000;
    attach_to_window(window);
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
    *(i32 *)&item->prev->next = (i32)item->next;
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
  /* (port) the text within the buffer the item was made with (add_item:
     its text or the room asked for; a longer one is cut), and none into an
     item made without a text (the original copied it whole: the menu's
     only caller, menu_set_item_label, cuts its labels to the room its items
     are made with, so no menu of the game's is affected) */
  if ((item != (MenuNode *)0x0) && (item->text != 0)) {
    str_copy_max((char *)((void *)item->text),(const char *)((void *)text),(size_t)item->field_0c);
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
void GameMenu::native_build(char *spec)

{
  u32 menu_handle;
  
  menu_handle = menu_hmenu_build_from_spec(spec);
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
void ModalMenuWindow::attach_menu(GameMenu *menu_obj)

{
  i32 hmenu;
  NativeWindow *hwnd;
  
  hmenu = (menu_obj)->get_hmenu();
  hwnd = get_window();
  wnd_obj_attach_menu(hwnd,hmenu);
  menu = (u32)menu_obj;
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
  unit_move_dir(game.active_unit,0xffffffff,3);
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
void WndBase::get_screen_rect(RECT *rect)

{
  wnd_obj_get_screen_rect(native_wnd,rect);
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
  draw_text_shadowed((GameFont *)body_font,(char *)text,x,y,flags);
  return;
}

/* 005bb4ae auto */
void DibSurface::wnd_create_with_surface(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,GamePalette *palette,WndBase *parent)

{
  NativeWindow *hwnd;
  
  ((WndBase *)(this + 1))->create_framed_child(title,style,x,y,width,height,palette,
             parent);
  create(width,height);
  set_palette(palette);
  hwnd = ((WndBase *)(this + 1))->get_window();
  wnd_install_paint_proc((SurfaceWindow *)this,hwnd);
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
bool __thiscall surfwnd_resize_dib(DibSurface *self,RECT *rect)

{
  int result;
  i32 height;
  i32 width;
  
  result = surface_alloc_dib_by_bpp(self,rect);
  if (result == 1) {
    height = rect_height(rect);
    width = rect_width(rect);
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
  
  window.get_min_size(&min_width2,&min_height2);
  window.get_max_size(&max_width2,&max_height2);
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
  window.set_client_size(width,height);
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
void ModalMenuWindow::get_min_size(i32 *out_width,i32 *out_height)

{
  *(u32 *)out_width = min_width;
  *(u32 *)out_height = min_height;
  return;
}

/* 005bb950 auto */
void ModalMenuWindow::get_max_size(i32 *out_width,i32 *out_height)

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
    self->get_entry(color_index & 0xff,red,green,blue);
    brush = CreateSolidBrush(RGB(red[0],green[0],blue[0]));
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
void WndBase::create_framed(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,GamePalette *palette)

{
  NativeWindow *hwnd;
  
  create_with_cmd_proc(title,style,x,y,width,height,palette);
  hwnd = get_window();
  wnd_install_frame_proc(this,hwnd);
  return;
}

/* 005c57f9 auto */
void WndBase::create_framed_child(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,GamePalette *palette,WndBase *parent)

{
  NativeWindow *hwnd;
  
  create_with_handlers(title,style,x,y,width,height,palette,parent);
  hwnd = get_window();
  wnd_install_frame_proc(this,hwnd);
  return;
}

/* 005c59c4 auto */
void Wnd::append_control(Control *ctrl)

{
  Control *last;
  
  last = (Control *)control_list;
  if (last == (Control *)0x0) {
    control_list = (uptr)ctrl;
  }
  else {
    for (; last->next_ctrl != 0; last = (Control *)last->next_ctrl) {
    }
    last->next_ctrl = (u32)ctrl;
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
        wnd_focus_control(this,(Control *)control_list);
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
  wnd_focus_control(this,ctrl);
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
void WndBase::create_with_cmd_proc(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,GamePalette *palette)

{
  NativeWindow *hwnd;
  
  create((i32)title,style,x,y,width,height,palette);
  hwnd = get_window();
  wnd_install_command_proc(this,hwnd);
  ((WndHandlers *)(this + 1))->clear_handlers();
  return;
}

/* 005c5fc4 auto */
void WndBase::create_with_handlers(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,GamePalette *palette,WndBase *parent)

{
  NativeWindow *hwnd;
  
  create_child((i32)title,style,x,y,width,height,palette,parent);
  hwnd = get_window();
  wnd_install_command_proc(this,hwnd);
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
HWND __cdecl hotspot_create_hwnd(int *rect,Control *ctrl)

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
  self = (WndBase *)(ctrl)->get_parent();
  parent_wnd = self->get_window();
  hotspot_hwnd = parent_wnd->hwnd;
  height = rect_height((RECT *)rect);
  width = rect_width((RECT *)rect);
  hotspot_hwnd = CreateWindowExA(4,str_mscontrolclass,((i8 *)&hotspot_wnd_title),0x50000000,*rect,rect[1],
                           width,height,hotspot_hwnd,hMenu,hInstance,lpParam);
  ctrl_init_block(hotspot_hwnd,(int)ctrl)->kind = 1;
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
void __cdecl ui_set_ctrl_font(GameFont *font)

{
  (*(i8 * *)(draw_target.font_rest + 8)) = (i8 *)font;
  return;
}

/* 005d268e auto */
void __cdecl ui_set_default_font(GameFont *font)

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
void __cdecl ui_set_button_font(GameFont *font)

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
void WndBase::create(i32 wnd_kind,u32 style,i32 x,i32 y, i32 width,i32 height,GamePalette *palette)

{
  NativeWindow *native;
  
  this->style = style;
  if (native_wnd != 0) {
    native = (NativeWindow *)wnd_obj_destroy(native_wnd);
    native_wnd = native;
  }
  native = wnd_obj_create((LPCSTR)wnd_kind,style,x,y,width,height,0);
  native_wnd = native;
  wnd_set_owner_ptr(this,native_wnd);
  set_palette(palette);
  return;
}

/* 005db893 auto */
void WndBase::create_child(i32 wnd_kind,u32 style,i32 x,i32 y, i32 width,i32 height,GamePalette *palette,WndBase *parent)

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
  wnd_set_owner_ptr(this,native_wnd);
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
    err_report_ctx_block.choice_dlg_window->window.end_modal_loop();
  }
  else if (button_id == 0x66) {
    err_report_ctx_block.choice_dlg_selection = 0xffffffff;
    err_report_ctx_block.choice_dlg_window->window.end_modal_loop();
  }
  return;
}

/* 005e0863: written by hand in src/game/unwind.cpp */

/* 005e0ce0 auto */
char * __cdecl menu_hmenu_parse_items(HMENU hmenu,char *spec)

{
  bool done;
  char *_Source;
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
  uBytes = _strlen(spec);
  hMem = LocalAlloc(2,uBytes);
  _Dest = (char *)LocalLock(hMem);
  *_Dest = '\0';
  for (; *spec == ' '; spec++) {
  }
  while (!done) {
    if (*spec == '!') {
      flags2 = 3;
      spec++;
    }
    else {
      flags2 = 0;
    }
    if (*spec == '*') {
      flags2 = flags2 | 8;
      spec++;
    }
    _Source = spec;
    label_len = 0;
    for (; ((*spec != '}' && (*spec != '_')) && (*spec != '{'));
        spec = spec + 1) {
      label_len++;
    }
    _strncat(_Dest,_Source,label_len);
    if ((*spec == '_') || (*spec == '}')) {
      if (*spec == '}') {
        done = true;
      }
      item_id = menu_next_item_id;
      menu_next_item_id = (HMENU)((iptr)menu_next_item_id + 1);
      do {
        spec++;
      } while (*spec == ' ');
    }
    else if (*spec == '{') {
      item_id = CreateMenu();
      spec = menu_hmenu_parse_items(item_id,spec + 1);
      flags2 = flags2 | 0x10;
      for (; *spec == ' '; spec++) {
      }
      if (*spec == '}') {
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
    cursor = menu_hmenu_parse_items(uIDNewItem,cursor);
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
void __cdecl wnd_install_paint_proc(SurfaceWindow *surface,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,0xc,(LONG_PTR)surface);
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
        self->blit_to_wnd((WndBase *)owner_ptr,&paint_rect,&paint_rect);
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
      hdcSrc = self->get_dib()->hdc;
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
void __cdecl wnd_install_frame_proc(WndBase *owner,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,8,(LONG_PTR)owner);
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
void __cdecl wnd_cycle_control_focus(WndBase *wnd,HWND hwnd,int direction)

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
    frame_win = (wnd)->get_window();
    is_child = IsChild(frame_win->hwnd,start_hwnd);
    if (is_child == 0) {
      hwnd = (HWND)0x0;
    }
  }
  if (hwnd == (HWND)0x0) {
    if (((Wnd *)wnd)->control_list == 0) {
      return;
    }
    if (direction == 2) {
      first_cmd = 5;
      frame_win2 = (wnd)->get_window();
      hwnd = GetWindow(frame_win2->hwnd,first_cmd);
      start_hwnd = hwnd;
    }
    else {
      uCmd = 1;
      first_cmd = 5;
      result = (int)(wnd)->get_window();
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
      self = (Checkbox *)ctrl_list_find_by_hwnd(hwnd,(Control *)((Wnd *)wnd)->control_list);
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
void __cdecl wnd_focus_control(Wnd *wnd,Control *control)

{
  char nav_enabled;
  int ctrl_type;
  i32 *items;
  i32 start_hwnd;
  
  nav_enabled = (wnd)->keyboard_nav_enabled();
  if (nav_enabled != '\0') {
    ctrl_type = (control)->get_type();
    if (ctrl_type == 3) {
      items = (i32 *)((RadioGroup *)control)->get_items();
      start_hwnd = hwnd_prev_sibling_wrap(*items);
      wnd_cycle_control_focus(wnd,start_hwnd,2);
    }
    else {
      start_hwnd = (control)->get_hwnd();
      start_hwnd = hwnd_prev_sibling_wrap(start_hwnd);
      wnd_cycle_control_focus(wnd,start_hwnd,2);
    }
  }
  return;
}

/* 005eb370 auto */
void __cdecl wnd_set_owner_ptr(WndBase *owner,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,0,(LONG_PTR)owner);
  return;
}

/* 005eb393 auto */
void __cdecl wnd_install_command_proc(WndBase *owner,NativeWindow *wnd)

{
  SetWindowLongA(wnd->hwnd,4,(LONG_PTR)owner);
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
void WndHandlers::call_lbuttondown(int x,int y)

{
  if (on_lbutton_down != 0) {
    on_lbutton_down(x,y);
  }
  return;
}

/* 005ecfd0 auto */
void WndHandlers::call_lbuttonup_raw(int x,int y)

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
void WndHandlers::call_rbuttondown(int x,int y)

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
void WndHandlers::call_msg44(int notify_arg)

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
    can_close = self->handlers.call_close();
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
        self->handlers.call_mousemove(lparam & 0xffff,lparam >> 0x10);
      }
    }
    else if (msg == 0x100) {
      zero_arg = 0;
      key = input_map_vkey(wparam);
      handled = self->dispatch_ctrl_msg(0x100,key,zero_arg);
      if (handled == '\0') {
        key = input_map_vkey(wparam);
        self->handlers.call_keydown_pre(key);
        key = input_map_vkey(wparam);
        self->handlers.call_keydown(key);
      }
    }
    else if (msg == 0x101) {
      key = input_map_vkey(wparam);
      self->handlers.call_keyup(key);
    }
    else {
      if (msg != 0x102) {
        def_result = DefWindowProcA(hwnd,msg,wparam,lparam);
        return def_result;
      }
      handled = self->dispatch_ctrl_msg(0x102,wparam & 0xff,0);
      if (handled == '\0') {
        self->handlers.call_char(wparam & 0xff);
      }
    }
  }
  else if (msg < 0x211) {
    if (msg == 0x210) {
      if ((wparam & 0xffff) == 0x201) {
        self->handlers.call_lbuttondown(lparam & 0xffff,lparam >> 0x10);
      }
      else if ((wparam & 0xffff) == 0x204) {
        self->handlers.call_rbuttondown(lparam & 0xffff,lparam >> 0x10);
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
          self->handlers.call_lbuttondown(lparam & 0xffff,lparam >> 0x10);
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
            self->handlers.call_lbuttonup(lparam & 0xffff,lparam >> 0x10);
          }
          self->handlers.call_lbuttonup_raw(lparam & 0xffff,lparam >> 0x10);
        }
        break;
      case 0x203:
        handled = self->dispatch_ctrl_msg(msg,lparam & 0xffff,lparam >> 0x10);
        if (handled == '\0') {
          self->handlers.call_dblclick(lparam & 0xffff,lparam >> 0x10);
        }
        break;
      case 0x204:
        handled = self->dispatch_ctrl_msg(msg,lparam & 0xffff,lparam >> 0x10);
        if (handled == '\0') {
          ((DDWindowExt *)self)->rbutton_held[0] = 1;
          ((DDWindowExt *)self)->rbutton_held_byte1[0] = '\0';
          ((DDWindowExt *)self)->rbutton_held_byte2[0] = '\0';
          ((DDWindowExt *)self)->rbutton_held_byte3[0] = '\0';
          self->handlers.call_rbuttondown(lparam & 0xffff,lparam >> 0x10);
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
            self->handlers.call_rbuttonup(lparam & 0xffff,lparam >> 0x10);
          }
          self->handlers.call_rbuttonup_raw(lparam & 0xffff,lparam >> 0x10);
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
      self->handlers.call_msg4c8(0);
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
  
  ctrl = child_controls.first();
  while (ctrl != 0) {
    child_controls.remove_current();
    ctrl = child_controls.first();
  }
  return;
}

/* 005f0342 auto */
void DDWindow::deactivate_controls()

{
  i32 ctrl;
  
  ctrl = child_controls.first();
  while (ctrl != 0) {
    ((DDControl *)ctrl)->deactivate();
    ctrl = child_controls.next();
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
  ctrl = (DDControl *)child_controls.first();
  while( true ) {
    if (ctrl == (DDControl *)0x0) {
      return 0;
    }
    handled = ctrl->dispatch_msg(msg,x,y);
    if (handled != 0) break;
    ctrl = (DDControl *)child_controls.next();
  }
  if (msg == 0x200) {
    hover_control = (uptr)ctrl;
  }
  return 1;
}
