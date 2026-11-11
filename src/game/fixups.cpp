/*
 * Assorted window procedures and helpers whose machine translation did not
 * compile (struct values treated as scalars, wrong Win32 argument lists,
 * a switch on a handle).  Rewritten following the original machine code.
 */
#include "game.h"

/* ------------------------------------------------------------------------ */
/* 0x005c8791: unload a font resource: delete its cached GDI fonts, remove
 * the font file, then free the resource block. */
void __cdecl font_file_unload(i32 font_handle)
{
    char *font;
    int slot;

    font = (char *)mem_handle_lock(font_handle);
    for (slot = 0; slot < *(int *)(font + 0x100); slot = slot + 1) {
        if (*(int *)(font + slot * 4 + 0x108) != 0) {
            DeleteObject(*(HGDIOBJ *)(font + slot * 4 + 0x108));
            *(int *)(font + slot * 4 + 0x108) = 0;
        }
    }
    RemoveFontResourceA(font);   /* file name at the start of the block */
    mem_handle_unlock(font_handle);
    mem_handle_free(font_handle);
}

/* ------------------------------------------------------------------------ */

/* 0x005c8caf: hover-tracking button window procedure. */
i32 __stdcall hotspot_wndproc(HWND hwnd,i32 msg,i32 wparam,uint lparam)
{
    void *self;
    ControlBlock *wnd_obj;
    int *rect_ptr;
    BOOL inside;
    i32 ctrl_arg;
    POINT pt;
    tagRECT rect;

    wnd_obj = wnd_from_hwnd(hwnd);
    self = (void *)wnd_obj->owner;
    switch (msg) {
    case 0x200:     /* WM_MOUSEMOVE */
        pt.x = lparam & 0xffff;
        pt.y = (lparam >> 0x10) & 0xffff;
        if (button_mouse.hover == 0) {
            if (button_mouse.pressed == 0) {
                button_mouse.hover = 1;
                SetCapture(hwnd);
                if (self != (void *)0x0) {
                    ctrl_arg = ((Control *)self)->get_parent();
                    wnd_set_active_child(ctrl_arg);
                    ctrl_arg = ((Control *)self)->get_id();
                    ((Hotspot *)self)->fire_enter( ctrl_arg);
                }
            }
        }
        else {
            rect_ptr = (int *)((Control *)self)->get_rect();
            rect.left = rect_ptr[0];
            rect.top = rect_ptr[1];
            rect.right = rect_ptr[2];
            rect.bottom = rect_ptr[3];
            OffsetRect(&rect, -rect.left, -rect.top);
            inside = PtInRect(&rect, pt);
            if (inside == 0) {
                button_mouse.hover = 0;
                if (button_mouse.pressed == 0) {
                    ReleaseCapture();
                    if (self != (void *)0x0) {
                        ctrl_arg = ((Control *)self)->get_parent();
                        wnd_set_active_child(ctrl_arg);
                        ctrl_arg = ((Control *)self)->get_id();
                        ((Hotspot *)self)->fire_leave( ctrl_arg);
                    }
                }
            }
        }
        break;
    case 0x201:     /* WM_LBUTTONDOWN */
        if (button_mouse.hover != 0) {
            button_mouse.pressed = 1;
            ctrl_arg = ((Control *)self)->get_parent();
            wnd_set_active_child(ctrl_arg);
            ctrl_arg = ((Control *)self)->get_id();
            ((Hotspot *)self)->fire_press( ctrl_arg);
        }
        break;
    case 0x202:     /* WM_LBUTTONUP */
        if (button_mouse.pressed != 0) {
            button_mouse.pressed = 0;
            if (button_mouse.hover != 0) {
                if (self != (void *)0x0) {
                    ctrl_arg = ((Control *)self)->get_parent();
                    wnd_set_active_child(ctrl_arg);
                    ctrl_arg = ((Control *)self)->get_id();
                    ((Hotspot *)self)->fire_click( ctrl_arg);
                }
            }
            else {
                ReleaseCapture();
                if (self != (void *)0x0) {
                    ctrl_arg = ((Control *)self)->get_parent();
                    wnd_set_active_child(ctrl_arg);
                    ctrl_arg = ((Control *)self)->get_id();
                    ((Hotspot *)self)->fire_leave( ctrl_arg);
                }
            }
        }
        break;
    case 0x203:     /* WM_LBUTTONDBLCLK */
        if (button_mouse.hover != 0) {
            ctrl_arg = ((Control *)self)->get_parent();
            wnd_set_active_child(ctrl_arg);
            ctrl_arg = ((Control *)self)->get_id();
            ((Hotspot *)self)->fire_dblclick( ctrl_arg);
        }
        break;
    default:
        return ctrl_default_wndproc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

/* ------------------------------------------------------------------------ */
/* Part of button_wndproc (005cacd4): WM_PAINT: blits the normal, pressed or
   disabled bitmap. */
static void button_on_paint(HWND hwnd,ControlBlock *wnd_obj2,void *ctrl)
{
  char nav_enabled;
  int *rect_ptr;
  HWND focus_wnd;
  int tmp;
  HGDIOBJ h;
  int cx;
  void *parent;
  tagPAINTSTRUCT ps;
  HDC hdc;
  HGDIOBJ bitmap2;
  tagRECT rect;
  HDC mem_dc;

  hdc = BeginPaint(hwnd, &ps);
  rect_ptr = (int *)((Control *)ctrl)->get_rect();
  rect.left = rect_ptr[0];
  rect.top = rect_ptr[1];
  rect.right = rect_ptr[2];
  rect.bottom = rect_ptr[3];
  ValidateRect(GetParent(hwnd), &rect);
  tmp = ((Button *)ctrl)->ctrl_is_enabled();
  if (tmp == 0) {
      focus_wnd = GetFocus();
      if (focus_wnd == hwnd) {
          parent = (void *)((Control *)ctrl)->get_parent();
          nav_enabled = ((Wnd *)parent)->keyboard_nav_enabled();
          if (nav_enabled == '\0') {
              parent = (void *)((Control *)ctrl)->get_parent();
              tmp = (int)((WndBase *)parent)->get_window();
              SetFocus(((NativeWindow *)tmp)->hwnd);
          }
          else {
              SetFocus((HWND)0x0);
          }
      }
      bitmap2 = wnd_obj2->bitmaps[2];
  }
  else {
      if (wnd_obj2->pressed == 0)
          bitmap2 = wnd_obj2->bitmaps[0];
      if (wnd_obj2->pressed == 1)
          bitmap2 = wnd_obj2->bitmaps[1];
  }
  mem_dc = CreateCompatibleDC(hdc);
  RealizePalette(mem_dc);
  h = SelectObject(mem_dc, bitmap2);
  tmp = rect_height(&rect);      /* height */
  cx = rect_width(&rect);         /* width */
  BitBlt(hdc, 0, 0, cx, tmp, mem_dc, 0, 0, 0xcc0020);
  SelectObject(mem_dc, h);
  DeleteDC(mem_dc);
  EndPaint(hwnd, &ps);
}

/* 0x005cacd4: bitmap push-button window procedure (normal / pressed /
 * disabled bitmaps, focus frame, space bar activation). */
i32 __stdcall button_wndproc(HWND hwnd,uint msg,WPARAM wparam,uint lparam)
{
    char nav_enabled;
    SHORT key_state;
    int *rect_ptr;
    BOOL inside;
    int tmp;
    void *parent;
    i32 ctrl_arg;
    POINT pt;
    ControlBlock *wnd_obj2;
    HDC hdc;
    tagRECT rect;
    void *ctrl;
    uint inside_flag;
    HBRUSH brush;
    tagRECT focus_rect;
    i32 key_word;

    wnd_obj2 = wnd_from_hwnd(hwnd);
    if (wnd_obj2 == (ControlBlock *)0x0)
        return 0;
    ctrl = (void *)wnd_obj2->owner;
    switch (msg) {
    case 0x201:     /* WM_LBUTTONDOWN */
        tmp = ((Button *)ctrl)->ctrl_is_enabled();
        if (tmp == 0)
            break;
        wnd_obj2->tracking = 1;
        SetFocus(hwnd);
        SetCapture(hwnd);
        /* fall through */
    case 0x200:     /* WM_MOUSEMOVE */
        if (wnd_obj2->tracking != 0) {
            pt.x = lparam & 0xffff;
            pt.y = (lparam >> 0x10) & 0xffff;
            rect_ptr = (int *)((Control *)ctrl)->get_rect();
            rect.left = rect_ptr[0];
            rect.top = rect_ptr[1];
            rect.right = rect_ptr[2];
            rect.bottom = rect_ptr[3];
            OffsetRect(&rect, -rect.left, -rect.top);
            inside = PtInRect(&rect, pt);
            inside_flag = (uint)(inside != 0);
            if ((uint)wnd_obj2->pressed != inside_flag) {
                wnd_obj2->pressed = inside_flag;
                InvalidateRect(hwnd, &rect, 0);
            }
        }
        break;
    case 0x202:     /* WM_LBUTTONUP */
        if (wnd_obj2->tracking == 0)
            break;
        ReleaseCapture();
        wnd_obj2->tracking = 0;
        pt.x = lparam & 0xffff;
        pt.y = (lparam >> 0x10) & 0xffff;
        rect_ptr = (int *)((Control *)ctrl)->get_rect();
        rect.left = rect_ptr[0];
        rect.top = rect_ptr[1];
        rect.right = rect_ptr[2];
        rect.bottom = rect_ptr[3];
        OffsetRect(&rect, -rect.left, -rect.top);
        InvalidateRect(hwnd, &rect, 0);
        wnd_obj2->pressed = 0;
        inside = PtInRect(&rect, pt);
        if (inside != 0) {
            button_wndproc(hwnd, 0xf, 0, 0);
            ctrl_arg = ((Control *)ctrl)->get_parent();
            wnd_set_active_child(ctrl_arg);
            ctrl_arg = ((Control *)ctrl)->get_id();
            ((Button *)ctrl)->fire_callback( ctrl_arg);
        }
        break;
    case 0xf:       /* WM_PAINT */
        button_on_paint(hwnd,wnd_obj2,ctrl);
        if (GetFocus() == hwnd)
            button_wndproc(hwnd, 7, 0, 0);
        break;
    case 0x100:     /* WM_KEYDOWN */
        if (wparam == 9) {
            SendMessageA(GetParent(hwnd), msg, wparam, lparam);
        }
        else if (wparam == 0x20 && (tmp = ((Button *)ctrl)->ctrl_is_enabled(), tmp != 0) &&
                 (parent = (void *)((Control *)ctrl)->get_parent(),
                  nav_enabled = ((Wnd *)parent)->keyboard_nav_enabled(), nav_enabled != '\0')) {
            wnd_obj2->pressed = 1;
            wnd_redraw_client(hwnd);
            wnd_obj2->pressed = 0;
            key_state = GetAsyncKeyState(0x20);
            key_word = (int)key_state;
            ui_pump_messages();
            while (*((char *)&key_word + 1) != '\0') {
                key_state = GetAsyncKeyState(0x20);
                key_word = (int)key_state;
            }
            wnd_redraw_client(hwnd);
            ctrl_arg = ((Control *)ctrl)->get_parent();
            wnd_set_active_child(ctrl_arg);
            ctrl_arg = ((Control *)ctrl)->get_id();
            ((Button *)ctrl)->fire_callback( ctrl_arg);
        }
        else {
            SendMessageA(GetParent(hwnd), msg, wparam, lparam);
        }
        break;
    case 7:         /* WM_SETFOCUS: draw the focus frame */
        parent = (void *)((Control *)ctrl)->get_parent();
        nav_enabled = ((Wnd *)parent)->keyboard_nav_enabled();
        if (nav_enabled == '\0')
            return ctrl_default_wndproc(hwnd, msg, wparam, lparam);
        hdc = GetDC(hwnd);
        brush = GetStockObject(3);
        GetClientRect(hwnd, &focus_rect);
        SelectObject(hdc, brush);
        FrameRect(hdc, &focus_rect, brush);
        SetRect(&focus_rect, focus_rect.left + 4, focus_rect.top + 4, focus_rect.right - 4,
                focus_rect.bottom - 4);
        FrameRect(hdc, &focus_rect, brush);
        DeleteObject(brush);
        ReleaseDC(hwnd, hdc);
        break;
    case 8:         /* WM_KILLFOCUS */
        wnd_redraw_client(hwnd);
        break;
    default:
        return ctrl_default_wndproc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

/* ------------------------------------------------------------------------ */

/* 0x005cb6eb: sprite push-button window procedure (left and right click). */
i32 __stdcall spritebtn_wndproc(HWND hwnd,uint msg,WPARAM wparam,uint lparam)
{
    char nav_enabled;
    ControlBlock *wnd_obj;
    int *rect_ptr;
    HPALETTE hPal;
    int height;
    int width;
    void *parent;
    i32 ctrl_arg;
    HDC hdcSrc;
    BOOL inside;
    POINT pt;
    tagPAINTSTRUCT ps;
    HDC hdc;
    tagRECT rect;
    void *ctrl;
    uint inside_flag;
    i32 key_word;

    wnd_obj = wnd_from_hwnd(hwnd);
    if (wnd_obj == (ControlBlock *)0x0)
        return 1;
    ctrl = (void *)wnd_obj->owner;
    switch (msg) {
    case 0x201:     /* WM_LBUTTONDOWN */
        wnd_obj->tracking = 1;
        SetFocus(hwnd);
        SetCapture(hwnd);
        /* fall through */
    case 0x200:     /* WM_MOUSEMOVE */
        if (wnd_obj->tracking != 0) {
            pt.x = lparam & 0xffff;
            pt.y = (lparam >> 0x10) & 0xffff;
            rect_ptr = (int *)((Control *)ctrl)->get_rect();
            rect.left = rect_ptr[0];
            rect.top = rect_ptr[1];
            rect.right = rect_ptr[2];
            rect.bottom = rect_ptr[3];
            OffsetRect(&rect, -rect.left, -rect.top);
            inside = PtInRect(&rect, pt);
            inside_flag = (uint)(inside != 0);
            if ((uint)wnd_obj->pressed != inside_flag) {
                wnd_obj->pressed = inside_flag;
                InvalidateRect(hwnd, &rect, 0);
            }
        }
        break;
    case 0x202:     /* WM_LBUTTONUP */
    case 0x205:     /* WM_RBUTTONUP */
        if (wnd_obj->tracking != 0) {
            ReleaseCapture();
            wnd_obj->tracking = 0;
            pt.x = lparam & 0xffff;
            pt.y = (lparam >> 0x10) & 0xffff;
            rect_ptr = (int *)((Control *)ctrl)->get_rect();
            rect.left = rect_ptr[0];
            rect.top = rect_ptr[1];
            rect.right = rect_ptr[2];
            rect.bottom = rect_ptr[3];
            OffsetRect(&rect, -rect.left, -rect.top);
            InvalidateRect(hwnd, &rect, 0);
            wnd_obj->pressed = 0;
            inside = PtInRect(&rect, pt);
            if (inside != 0) {
                spritebtn_wndproc(hwnd, 0xf, 0, 0);
                ctrl_arg = ((Control *)ctrl)->get_parent();
                wnd_set_active_child(ctrl_arg);
                ctrl_arg = ((Control *)ctrl)->get_id();
                if (msg == 0x202)
                    ((SpriteButton *)ctrl)->fire_click( ctrl_arg);
                else
                    ((SpriteButton *)ctrl)->fire_rclick( ctrl_arg);
            }
        }
        break;
    case 0x204:     /* WM_RBUTTONDOWN */
        wnd_obj->tracking = 1;
        SetFocus(hwnd);
        SetCapture(hwnd);
        break;
    case 0xf:       /* WM_PAINT */
        hdc = BeginPaint(hwnd, &ps);
        rect_ptr = (int *)((Control *)ctrl)->get_rect();
        rect.left = rect_ptr[0];
        rect.top = rect_ptr[1];
        rect.right = rect_ptr[2];
        rect.bottom = rect_ptr[3];
        ValidateRect(GetParent(hwnd), &rect);
        OffsetRect(&rect, -rect.left, -rect.top);
        ValidateRect(hwnd, &rect);
        if (display_palette.palettized == 1) {
            parent = (void *)((Control *)ctrl)->get_parent();
            parent = (void *)((WndBase *)parent)->get_palette();
            hPal = ((GamePalette *)parent)->get_hpalette();
            SelectPalette(hdc, hPal, 0);
        }
        if (wnd_obj->pressed == 0) {
            ((DibSurface *)((int)ctrl + 0x34))->lock();
            parent = (void *)((Control *)ctrl)->get_parent();
            ctrl_arg = ((WndBase *)parent)->get_palette();
            ((DibSurface *)((int)ctrl + 0x34))->apply_palette( (GamePalette *)ctrl_arg);
            if (display_palette.palettized == 1)
                RealizePalette(hdc);
            height = (int)((DibSurface *)((int)ctrl + 0x34))->get_dib();
            hdcSrc = ((GdiDib *)height)->hdc;
            height = ((DibSurface *)((int)ctrl + 0x34))->surface_height();
            width = ((DibSurface *)((int)ctrl + 0x34))->surface_width();
            BitBlt(hdc, 0, 0, width, height, hdcSrc, 0, 0, 0xcc0020);
            ((DibSurface *)((int)ctrl + 0x34))->unlock();
        }
        if (wnd_obj->pressed == 1) {
            ((DibSurface *)((int)ctrl + 0x7c))->lock();
            parent = (void *)((Control *)ctrl)->get_parent();
            ctrl_arg = ((WndBase *)parent)->get_palette();
            ((DibSurface *)((int)ctrl + 0x7c))->apply_palette( (GamePalette *)ctrl_arg);
            RealizePalette(hdc);
            height = (int)((DibSurface *)((int)ctrl + 0x7c))->get_dib();
            hdcSrc = ((GdiDib *)height)->hdc;
            /* The original uses the same accessor for both dimensions here. */
            height = ((DibSurface *)((int)ctrl + 0x7c))->surface_width();
            width = ((DibSurface *)((int)ctrl + 0x7c))->surface_width();
            BitBlt(hdc, 0, 0, width, height, hdcSrc, 0, 0, 0xcc0020);
            ((DibSurface *)((int)ctrl + 0x7c))->unlock();
        }
        EndPaint(hwnd, &ps);
        if (GetFocus() == hwnd)
            spritebtn_wndproc(hwnd, 7, 0, 0);
        break;
    case 0x100:     /* WM_KEYDOWN */
        if (wparam == 9) {
            SendMessageA(GetParent(hwnd), msg, wparam, lparam);
        }
        else if (wparam == 0x20 && (parent = (void *)((Control *)ctrl)->get_parent(),
                                     nav_enabled = ((Wnd *)parent)->keyboard_nav_enabled(), nav_enabled != '\0')) {
            wnd_obj->pressed = 1;
            wnd_redraw_client(hwnd);
            wnd_obj->pressed = 0;
            key_word = (int)GetAsyncKeyState(0x20);
            while (*((char *)&key_word + 1) != '\0')
                key_word = (int)GetAsyncKeyState(0x20);
            wnd_redraw_client(hwnd);
            ctrl_arg = ((Control *)ctrl)->get_parent();
            wnd_set_active_child(ctrl_arg);
            ctrl_arg = ((Control *)ctrl)->get_id();
            ((SpriteButton *)ctrl)->fire_click( ctrl_arg);
        }
        else {
            SendMessageA(GetParent(hwnd), msg, wparam, lparam);
        }
        break;
    case 7:         /* WM_SETFOCUS */
        parent = (void *)((Control *)ctrl)->get_parent();
        nav_enabled = ((Wnd *)parent)->keyboard_nav_enabled();
        if (nav_enabled == '\0')
            return 0;
        return ctrl_default_wndproc(hwnd, 7, wparam, lparam);
    case 8:         /* WM_KILLFOCUS */
        wnd_redraw_client(hwnd);
        break;
    default:
        return ctrl_default_wndproc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

/* ------------------------------------------------------------------------ */

/* 0x005e22ed: start playing a video (decompressor setup, audio sync). */
void __cdecl avi_play_start(AviPlayer *avi)
{
    LONG t_end, t_start;

    if (avi == 0 || avi_play.file_loaded == 0)
        return;
    if (avi_play.playing != 0)
        avi_play_pause(avi);
    if (avi->double_size != 0) {
        ICSendMessage((*(HIC *)&avi_play.decompressor), 0x403f, 0, 0);
        (*(uint *)&indeo.video_flags_a) = 8;
        if (avi->transparent != 0)
            (*(uint *)&indeo.video_flags_a) |= 0x80000004;
        (*(uint *)&indeo.video_flags_b) = 0x80000008;
        ICSendMessage((*(HIC *)&avi_play.decompressor), 0x5001, (DWORD_PTR)(iptr)&indeo.seq_opts, 0x2c);
        ICSendMessage((*(HIC *)&avi_play.decompressor), 0x5001, (DWORD_PTR)(iptr)&indeo.frame_opts, 0x70);
        avi->dst_format.height <<= 1;
        avi->dst_format.width <<= 1;
        avi_ic_decompress_ex_begin((i32)(*(HIC *)&avi_play.decompressor), 0, (BITMAPINFOHEADER *)&avi->src_format, 0, 0, 0,
                     avi->src_format.width, avi->src_format.height,
                     (BITMAPINFOHEADER *)&avi->dst_format, 0, 0, 0,
                     avi->src_format.width * 2, avi->src_format.height * 2);
        avi->dst_format.height >>= 1;
        avi->dst_format.width >>= 1;
    }
    if (avi->has_audio != 0) {
        t_end = AVIStreamSampleToTime((*(PAVISTREAM *)&avi_play.video_stream), avi->end_frame);
        t_start = AVIStreamSampleToTime((*(PAVISTREAM *)&avi_play.video_stream), avi->cur_frame);
        avi->audio.prepare_range( t_start, t_end);
    }
    if (avi_play.file_loaded != 0) {
        if (avi->transparent == 0)
            avi_update_frame(avi);
        wnd_modal_stack_below_block.avi_idle_update_enabled = 1;
        avi_play.playing = 1;
        avi_blit_union_left_block.g_video_frame_count = 0;
        if (avi->has_audio == 0) {
            (*(DWORD *)&avi_play.start_time) = timeGetTime();
        }
        else {
            avi->audio.start();
            avi->audio_playing = 1;
        }
    }
}

/* ------------------------------------------------------------------------ */

typedef HRESULT (__stdcall *dd_getcaps_fn)(void *dd, void *driver_caps, void *hel_caps);

/* 0x005e89d0: register the DirectDraw window class, create DirectDraw and
 * check the hardware capabilities. */
i32 __cdecl ddraw_init(void)
{
    HRESULT hr;
    WNDCLASSA wc;

    wc.style = 0xb;
    wc.lpfnWndProc = (WNDPROC)ddwin_native_wndproc;
    wc.cbClsExtra = 8;
    wc.cbWndExtra = 0;
    wc.hInstance = err_report_ctx_block.app_hinstance;
    wc.hIcon = (HICON)0x0;
    wc.hCursor = LoadCursorA((HINSTANCE)0x0, (LPCSTR)0x7f00);
    wc.hbrBackground = (HBRUSH)0x0;
    wc.lpszMenuName = (LPCSTR)0x0;
    wc.lpszClassName = str_msdirectwindow;
    RegisterClassA(&wc);
    hr = DirectDrawCreate((GUID *)0x0, (LPDIRECTDRAW *)&g_ddraw, (IUnknown *)0x0);
    if (hr != 0) {
        dbg_log((const char *)str_error_directdrawinitialize_failed);
        return 0;
    }
    err_report_ctx_block.g_ddcaps_driver = 0x13c;
    err_report_ctx_block.g_ddcaps_hel = 0x13c;
    /* IDirectDraw::GetCaps (vtable slot 11) */
    ((dd_getcaps_fn)(*(void ***)g_ddraw)[11])(g_ddraw, (void *)&err_report_ctx_block.g_ddcaps_driver, (void *)&err_report_ctx_block.g_ddcaps_hel);
    if ((err_report_ctx_block.ddraw_hw_caps_byte0 & 0x40) == 0)
        dbg_log((const char *)str_direct_draw_doesn_t_supports_hardware_bl);
    if ((err_report_ctx_block.ddraw_hw_caps_byte2 & 0x40) == 0)
        dbg_log((const char *)str_direct_draw_doesn_t_supports_colorkeys);
    if ((err_report_ctx_block.ddraw_hw_caps_byte3 & 4) == 0)
        dbg_log((const char *)str_direct_draw_doesn_t_supports_color_fills);
    if ((err_report_ctx_block.ddraw_hw_caps_byte3 & 8) != 0)
        dbg_log((const char *)str_direct_draw_device_is_bank_switched);
    return 1;
}

/* ------------------------------------------------------------------------ */

/* Part of wnd_child_wndproc (005eb447): WM_HSCROLL: moves the horizontal scroll bar
   (line, page, thumb) and tells the window. */
static void wnd_child_on_hscroll(HWND hwnd,Wnd *child2,void *view,WPARAM wp)
{
  int page;
  int scroll_pos2;

  scroll_pos2 = child2->get_hscroll_pos();
  switch (wp) {
  case 4:     /* SB_THUMBPOSITION */
      scroll_pos2 = (int)(short)(wp >> 0x10);
      SetScrollPos(hwnd, 0, scroll_pos2, 1);
      (*(void * *)&modal_wnd.active_child) = child2;
      child2->hscroll_pos = scroll_pos2;
      ((WndHandlers *)view)->call_hscroll( child2->hscroll_pos);
      break;
  case 5:     /* SB_THUMBTRACK */
      (*(void * *)&modal_wnd.active_child) = child2;
      ((WndHandlers *)view)->call_hscroll_track( (int)(short)(wp >> 0x10));
      break;
  case 0:
  case 1:
  case 2:
  case 3:
      switch (wp) {
      case 1:     /* SB_LINEDOWN */
          scroll_pos2 = scroll_pos2 + 1;
          break;
      case 0:     /* SB_LINEUP */
          scroll_pos2 = scroll_pos2 - 1;
          break;
      case 3:     /* SB_PAGEDOWN */
          page = child2->get_hscroll_page();
          scroll_pos2 = scroll_pos2 + page;
          break;
      case 2:     /* SB_PAGEUP */
          page = child2->get_hscroll_page();
          scroll_pos2 = scroll_pos2 - page;
          break;
      }
      SetScrollPos(hwnd, 0, scroll_pos2, 1);
      scroll_pos2 = GetScrollPos(hwnd, 0);
      (*(void * *)&modal_wnd.active_child) = child2;
      child2->hscroll_pos = scroll_pos2;
      ((WndHandlers *)view)->call_hscroll( scroll_pos2);
      break;
  }
}

/* Part of wnd_child_wndproc (005eb447): WM_VSCROLL: moves the vertical scroll bar
   (line, page, thumb) and tells the window. */
static void wnd_child_on_vscroll(HWND hwnd,Wnd *child2,void *view,WPARAM wp)
{
  int page;
  int scroll_pos2;

  scroll_pos2 = child2->get_vscroll_pos();
  switch (wp) {
  case 4:     /* SB_THUMBPOSITION */
      scroll_pos2 = (int)(short)(wp >> 0x10);
      SetScrollPos(hwnd, 1, scroll_pos2, 1);
      (*(void * *)&modal_wnd.active_child) = child2;
      child2->vscroll_pos = scroll_pos2;
      ((WndHandlers *)view)->call_vscroll( child2->vscroll_pos);
      break;
  case 5:     /* SB_THUMBTRACK */
      (*(void * *)&modal_wnd.active_child) = child2;
      ((WndHandlers *)view)->call_vscroll_track( (int)(short)(wp >> 0x10));
      break;
  case 0:
  case 1:
  case 2:
  case 3:
      switch (wp) {
      case 1:
          scroll_pos2 = scroll_pos2 + 1;
          break;
      case 0:
          scroll_pos2 = scroll_pos2 - 1;
          break;
      case 3:
          page = child2->get_vscroll_page();
          scroll_pos2 = scroll_pos2 + page;
          break;
      case 2:
          page = child2->get_vscroll_page();
          scroll_pos2 = scroll_pos2 - page;
          break;
      }
      SetScrollPos(hwnd, 1, scroll_pos2, 1);
      scroll_pos2 = GetScrollPos(hwnd, 1);
      (*(void * *)&modal_wnd.active_child) = child2;
      child2->vscroll_pos = scroll_pos2;
      ((WndHandlers *)view)->call_vscroll( scroll_pos2);
      break;
  }
}

/* 0x005eb447: MDI-style child window procedure: forwards input, sizing,
 * scrolling and palette messages to the window object (GWL 4). */
uint __stdcall wnd_child_wndproc(HWND hwnd,uint msg,HWND wparam_hwnd,uint lparam)
{
    int wnd_obj;
    NativeWindow *owner;
    void *view;
    WPARAM wp = wparam_hwnd;
    short x = (short)lparam;
    short y = (short)(lparam >> 0x10);
    int min_h;
    int min_w;
    HDC hdc;
    Wnd *child2;
    UINT realized;
    int max_h;
    int max_w;

    child2 = (Wnd *)GetWindowLongA(hwnd, 4);
    if (child2 == (Wnd *)0x0)
        return wnd_base_wndproc(hwnd, msg, wp, lparam);
    view = (void *)((int)child2 + 0x10);

    switch (msg) {
    case 0x210:     /* WM_PARENTNOTIFY */
        switch (wp & 0xffff) {
        case 0x201:
            (*(void * *)&modal_wnd.active_child) = child2;
            ((WndHandlers *)view)->call_lbuttondown( lparam & 0xffff, lparam >> 0x10);
            break;
        case 0x204:
            (*(void * *)&modal_wnd.active_child) = child2;
            ((WndHandlers *)view)->call_rbuttondown( lparam & 0xffff, lparam >> 0x10);
            break;
        }
        break;

    case 0x4c8:
        if (lparam == 0x20d)
            ((WndHandlers *)view)->call_msg4c8( 0);
        break;

    case 0x24: {    /* WM_GETMINMAXINFO */
        int *mmi = (int *)lparam;
        int border_y, border_x;
        border_y = wnd_obj_frame_height(child2->get_window());
        border_x = wnd_obj_frame_width(child2->get_window());
        child2->get_min_size( (iptr)&min_w, (iptr)&min_h);
        child2->get_max_size( (iptr)&max_w, (iptr)&max_h);
        mmi[8] = border_x + max_w;        /* ptMaxTrackSize */
        mmi[9] = border_y + max_h;
        mmi[6] = border_x + min_w;       /* ptMinTrackSize */
        mmi[7] = border_y + min_h;
        break;
    }

    case 0x201:     /* WM_LBUTTONDOWN */
        owner = ((WndBase *)GetWindowLongA(hwnd, 0))->get_window();
        (*(void * *)&modal_wnd.active_child) = child2;
        if ((owner->flags_bytes[1] & 2) != 0) {
            SetFocus(owner->hwnd);
            BringWindowToTop(hwnd);
        }
        child2->lbutton_down = 1;
        ((WndHandlers *)view)->call_lbuttondown( (int)x, (int)y);
        break;
    case 0x202:     /* WM_LBUTTONUP */
        (*(void * *)&modal_wnd.active_child) = child2;
        if ((int)child2->lbutton_down == 1) {
            child2->lbutton_down = 0;
            ((WndHandlers *)view)->call_lbuttonup( (int)x, (int)y);
        }
        ((WndHandlers *)view)->call_lbuttonup_raw( lparam & 0xffff, lparam >> 0x10);
        break;
    case 0x204:     /* WM_RBUTTONDOWN */
        owner = ((WndBase *)GetWindowLongA(hwnd, 0))->get_window();
        if ((owner->flags_bytes[1] & 2) != 0) {
            SetFocus(owner->hwnd);
            BringWindowToTop(hwnd);
        }
        (*(void * *)&modal_wnd.active_child) = child2;
        child2->rbutton_down = 1;
        ((WndHandlers *)view)->call_rbuttondown( (int)x, (int)y);
        break;
    case 0x205:     /* WM_RBUTTONUP */
        (*(void * *)&modal_wnd.active_child) = child2;
        if ((int)child2->rbutton_down == 1) {
            (*(void * *)&modal_wnd.active_child) = child2;
            child2->rbutton_down = 0;
            ((WndHandlers *)view)->call_rbuttonup( (int)x, (int)y);
        }
        ((WndHandlers *)view)->call_rbuttonup_raw( (int)x, (int)y);
        break;
    case 0x200:     /* WM_MOUSEMOVE */
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_mousemove( (int)x, (int)y);
        break;
    case 0x203:     /* WM_LBUTTONDBLCLK */
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_dblclick( (int)x, (int)y);
        break;

    case 0x100:     /* WM_KEYDOWN */
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_keydown_pre( input_map_vkey(wp));
        ((WndHandlers *)view)->call_keydown( input_map_vkey(wp));
        break;
    case 0x102:     /* WM_CHAR */
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_char( wp & 0xff);
        break;
    case 0x101:     /* WM_KEYUP */
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_keyup( input_map_vkey(wp));
        break;

    case 7:         /* WM_SETFOCUS */
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_setfocus();
        break;

    case 0x311:     /* WM_PALETTECHANGED */
        if (wparam_hwnd == hwnd)
            return wnd_base_wndproc(hwnd, msg, wp, lparam);
        /* fall through */
    case 0x30f:     /* WM_QUERYNEWPALETTE */
        hdc = GetDC(hwnd);
        wnd_obj = (int)child2->get_window();
        if ((int)((NativeWindow *)wnd_obj)->palette != 0) {
            wnd_obj = (int)child2->get_window();
            SelectPalette(hdc, ((NativeWindow *)wnd_obj)->palette, 0);
            realized = RealizePalette(hdc);
            InvalidateRect(hwnd, (RECT *)0x0, 1);
        }
        ReleaseDC(hwnd, hdc);
        /* realized is left unset when there is no palette (as in the original). */
        return (uint)(realized != 0);

    case 5:         /* WM_SIZE */
        (*(void * *)&modal_wnd.active_child) = child2;
        if (IsIconic(hwnd) == 0) {
            child2->on_size_nop( (int)x, (int)y);
            ((WndHandlers *)view)->call_size( wp);
        }
        return wnd_base_wndproc(hwnd, msg, wp, lparam);
    case 3:         /* WM_MOVE */
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_move( (int)x, (int)y);
        return wnd_base_wndproc(hwnd, msg, wp, lparam);

    case 0x111:     /* WM_COMMAND */
        (*(void * *)&modal_wnd.active_child) = child2;
        if ((short)lparam == 0)
            wnd_on_menu_command(hwnd, wparam_hwnd);
        break;
    case 0x3bd:
        snd_wave_on_buffer_done(wparam_hwnd, (WaveHeader *)lparam);
        return 0;

    case 0x11f:     /* WM_MENUSELECT */
        (*(void * *)&modal_wnd.active_child) = child2;
        if ((wp >> 0x10 & 0xffff) == 0xffff && lparam == 0)
            child2->fire_menu_select( 0xffffffff);
        else
            child2->fire_menu_select( wp & 0xffff);
        break;

    case 0x112:     /* WM_SYSCOMMAND */
        (*(void * *)&modal_wnd.active_child) = child2;
        switch (wp) {
        case 0xf020:    /* SC_MINIMIZE */
            ((WndHandlers *)view)->call_minimize();
            break;
        case 0xf120:    /* SC_RESTORE */
            ((WndHandlers *)view)->call_restore();
            break;
        }
        return wnd_base_wndproc(hwnd, msg, wp, lparam);

    case 0x10:      /* WM_CLOSE */
        (*(void * *)&modal_wnd.active_child) = child2;
        wnd_obj = ((WndHandlers *)view)->call_close();
        if (wnd_obj == 0)
            break;
        return wnd_base_wndproc(hwnd, msg, wp, lparam);

    case 0x44:
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_msg44( lparam & 0xffff);
        return 1;

    case 0x114:     /* WM_HSCROLL */
        wnd_child_on_hscroll(hwnd,child2,view,wp);
        break;

    case 0x115:     /* WM_VSCROLL */
        wnd_child_on_vscroll(hwnd,child2,view,wp);
        break;

    case 2:         /* WM_DESTROY */
        SetWindowLongA(hwnd, 4, 0);
        return wnd_base_wndproc(hwnd, msg, wp, lparam);

    case 0x231:     /* WM_ENTERSIZEMOVE */
        owner = ((WndBase *)GetWindowLongA(hwnd, 0))->get_window();
        (void)owner;
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_entersizemove();
        return wnd_base_wndproc(hwnd, msg, wp, lparam);
    case 0x232:     /* WM_EXITSIZEMOVE */
        owner = ((WndBase *)GetWindowLongA(hwnd, 0))->get_window();
        (void)owner;
        (*(void * *)&modal_wnd.active_child) = child2;
        ((WndHandlers *)view)->call_exitsizemove();
        return wnd_base_wndproc(hwnd, msg, wp, lparam);

    default:
        return wnd_base_wndproc(hwnd, msg, wp, lparam);
    }
    return 0;
}
