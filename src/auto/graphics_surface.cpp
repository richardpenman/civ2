/* The game, graphics: drawing surfaces, DIBs, sprites, palettes and fonts: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 005bb3f0 auto */
void DibSurface::wnd_create(const char *title,i32 style,i32 x,i32 y ,i32 width,i32 height,GamePalette *palette)

{
  NativeWindow *window;
  
  ((WndBase *)(this + 1))->create_framed(title,style,x,y,width,height,palette);
  create(width,height);
  set_palette(palette);
  window = ((WndBase *)(this + 1))->get_window();
  wnd_install_paint_proc((SurfaceWindow *)this,window);
  return;
}

/* 005bcaa7 auto */
void __cdecl gfx_get_screen_rect(RECT *rect)

{
  int yBottom;
  int xRight;
  
  yBottom = GetSystemMetrics(1);
  xRight = GetSystemMetrics(0);
  SetRect(rect,0,0,xRight,yBottom);
  return;
}

/* 005bcc11 auto */
void __cdecl
dib_blit_to_wnd_obj(GdiDib *src,int src_x,int src_y,int width,int height,NativeWindow *dest,int dest_x,
            int dest_y)

{
  if ((src != 0) && (dest != 0)) {
    if ((dest->palette != 0) && (display_palette.palettized == 1)) {
      RealizePalette(dest->hdc);
    }
    BitBlt(dest->hdc,dest_x,dest_y,width,height,src->hdc,src_x,
           src_y,0xcc0020);
  }
  return;
}

/* 005bd630 auto */
DibSurface * __thiscall surface_ctor(DibSurface *self)

{
  self->vtable = (u32)&leader_anim_frame_starts_block.ptr_fun_5;
  self->set_dims(0);
  return self;
}

/* 005bd65c auto */
void DibSurface::create(int width,int height)

{
  tagRECT rect;
  
  SetRect(&rect,0,0,width,height);
  alloc_dib8(&rect);
  return;
}

/* 005bd696 auto */
i32 DibSurface::alloc_dib8(RECT *rect)

{
  char locked;
  u32 new_dib;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    new_dib = dib_destroy(dib);
    dib = (GdiDib *)new_dib;
  }
  if (((rect != 0) && (dim = rect_width(rect), dim != 0)) &&
     (dim = rect_height(rect), dim != 0)) {
    set_dims((int *)rect);
    new_dib = (u32)dib_create((RECT *)&bounds_rect);
    dib = (GdiDib *)new_dib;
    if (dib != 0) {
      new_dib = dib_get_stride(dib);
      stride = new_dib;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(const char*)&str_err_portallocfailed,(const char*)&str_d_ss_smeds32_port_cpp,0x71);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005bd813 auto */
void DibSurface::set_dims(int *rect)

{
  bits = 0;
  dib = 0;
  palette_version = 0;
  stride = 0;
  signed_pitch = 0;
  bytes_per_pixel = 1;
  if (rect == (int *)0x0) {
    pixel_height = 0;
    pixel_width = 0;
    SetRect((LPRECT)&bounds_rect,0,0,0,0);
    SetRect((LPRECT)&clip_left,0,0,0,0);
  }
  else {
    pixel_height = rect[3] - rect[1];
    pixel_width = rect[2] - *rect;
    SetRect((LPRECT)&bounds_rect,0,0,pixel_width,pixel_height);
    SetRect((LPRECT)&clip_left,0,0,pixel_width,pixel_height);
  }
  return;
}

/* 005bd915 auto */
DibSurface * __thiscall surface_dtor(DibSurface *self)

{
  char locked;
  u32 new_dib;
  
  self->vtable = (u32)&leader_anim_frame_starts_block.ptr_fun_5;
  locked = self->is_locked();
  if (locked != '\0') {
    self->unlock();
  }
  new_dib = dib_destroy(self->dib);
  self->dib = (GdiDib *)new_dib;
  self->set_dims(0);
  if (draw_target.surface == self) {
    draw_target.surface = (DibSurface *)0x0;
  }
  return self;
}

/* 005bf071 auto */
i32 DibSurface::load_gif_file(const char *filename,uint first_color,uint color_count,GamePalette *palette)

{
  int result;
  i32 ret;
  uint table_size;
  tagRECT rect;
  int mem;
  iptr buffer;
  int height2;
  FileObject file;
  int width2;
  byte lzw_min_code;
  char *data;
  char *cursor;
  char *image_desc;
  char *image_data;
  
  file.init();
  ON_SCOPE_EXIT(file.close());
  result = file.open(filename);
  if (result == 0) {
    ret = 0;
  }
  else {
    ret = file.get_size();
    mem = mem_handle_alloc(ret);
    win_flush_pending_msgs();
    if (mem == 0) {
      ret = 0;
    }
    else {
      buffer = (iptr)mem_handle_lock(mem);
      ret = file.get_size();
      file.read((char *)buffer,ret);
      buffer = mem_handle_unlock(mem);
      win_flush_pending_msgs();
      if (mem == 0) {
        dbg_logf_str((const char *)&str_error_gif_resource_not_found,filename);
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(3,(const char*)&str_err_resourcenotfound,(const char*)&str_d_ss_smeds32_port_cpp_2,0x382);
        ret = 0;
      }
      else {
        data = (char *)mem_handle_lock(mem);
        result = _strncmp(data,((i8 *)&gif_sig_file),3);
        if (result == 0) {
          if ((data[10] & 0x80U) == 0) {
            dbg_logf_str((const char *)&str_error_gif_contains_no_global_color_map,filename);
            mem_handle_unlock(mem);
            mem_handle_free(mem);
            ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(const char*)&str_err_badpicformat_2,(const char*)&str_d_ss_smeds32_port_cpp_4,0x396);
            ret = 0;
          }
          else {
            table_size = 1 << (data[10] & 7U) + 1;
            cursor = data + 0xd;
            win_flush_pending_msgs();
            if (palette != 0) {
              win_flush_pending_msgs();
              if (0x100 < (int)(color_count + first_color)) {
                color_count = 0x100 - first_color;
              }
              if (table_size <= color_count) {
                color_count = table_size;
              }
              (palette)->set_rgb(first_color,color_count,(u8 *)cursor)
              ;
              win_flush_pending_msgs();
            }
            for (cursor = data + table_size * 3 + 0xd; *cursor == '\0'; cursor++)
            {
            }
            if ((*cursor == ',') || (*cursor == '!')) {
              image_desc = cursor + 1;
              width2 = endian_from_le((uint)*(ushort *)(cursor + 5));
              height2 = endian_from_le((uint)*(ushort *)(image_desc + 6));
              if ((image_desc[8] & 0x80U) != 0) {
                dbg_log((const char *)&str_warning_skipping_local_color_table);
              }
              SetRect(&rect,0,0,width2,height2);
              result = resize(&rect);
              if (result == 0) {
                mem_handle_unlock(mem);
                mem_handle_free(mem);
                ret = 0;
              }
              else {
                lzw_min_code = image_desc[9];
                image_data = image_desc + 10;
                win_flush_pending_msgs();
                gif_lzw_decode((byte *)image_data,first_color,
                             (ushort)lzw_min_code,stride
                             ,width2,height2,(i8 *)bits);
                win_flush_pending_msgs();
                set_palette(palette);
                result = dib_is_top_down(dib);
                if (result == 0) {
                  dib_flip_vertical(dib);
                }
                mem_handle_unlock(mem);
                mem_handle_free(mem);
                file.map_close();
                ret = 1;
              }
            }
            else {
              dbg_logf_str((const char *)&str_error_gif_image_block_not_found,filename);
              mem_handle_unlock(mem);
              mem_handle_free(mem);
              ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(const char*)&str_err_badpicformat_3,(const char*)&str_d_ss_smeds32_port_cpp_5,0x3b3);
              ret = 0;
            }
          }
        }
        else {
          dbg_logf_str((const char *)&str_error_resource_is_not_a_gif,filename);
          mem_handle_unlock(mem);
          mem_handle_free(mem);
          ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(const char*)&str_err_badpicformat,(const char*)&str_d_ss_smeds32_port_cpp_3,0x38d);
          ret = 0;
        }
      }
    }
  }
  return ret;
}

/* 005bf5e1 auto */
i32 DibSurface::load_gif_resource(int res_id,uint first_color,uint color_count,GamePalette *palette)

{
  i32 ret;
  int result;
  uint table_size;
  tagRECT rect;
  int res;
  char res_type[4];
  int height2;
  int width2;
  byte lzw_min_code;
  char *data;
  char *cursor;
  char *image_desc;
  char *image_data;
  
  res_type[0] = 0x47;
  res_type[1] = 0x49;
  res_type[2] = 0x46;
  res_type[3] = 0x53;
  res = res_load(res_type,res_id);
  if (res == 0) {
    dbg_logf((const char *)&str_error_gif_resource_not_found_2,res_id);
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(3,(const char*)&str_err_resourcenotfound_2,(const char*)&str_d_ss_smeds32_port_cpp_6,0x3f4);
    ret = 0;
  }
  else {
    data = (char *)res_lock(res);
    result = _strcmp(data,((i8 *)&gif_sig_resource));
    if (result == 0) {
      dbg_logf((const char *)&str_error_resource_is_not_a_gif_2,res_id);
      res_unlock(res);
      res_free(res);
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(const char*)&str_err_badpicformat_4,(const char*)&str_d_ss_smeds32_port_cpp_7,0x3ff);
      ret = 0;
    }
    else if ((data[10] & 0x80U) == 0) {
      dbg_logf((const char *)&str_error_gif_contains_no_global_color_map_2,res_id);
      res_unlock(res);
      res_free(res);
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(const char*)&str_err_badpicformat_5,(const char*)&str_d_ss_smeds32_port_cpp_8,0x408);
      ret = 0;
    }
    else {
      table_size = 1 << (data[10] & 7U) + 1;
      cursor = data + 0xd;
      if (palette != 0) {
        if (0x100 < (int)(color_count + first_color)) {
          color_count = 0x100 - first_color;
        }
        if (table_size <= color_count) {
          color_count = table_size;
        }
        (palette)->set_rgb(first_color,color_count,(u8 *)cursor);
      }
      win_flush_pending_msgs();
      for (cursor = data + table_size * 3 + 0xd; *cursor == '\0'; cursor++) {
      }
      if ((*cursor == ',') || (*cursor == '!')) {
        image_desc = cursor + 1;
        width2 = endian_from_le((uint)*(ushort *)(cursor + 5));
        height2 = endian_from_le((uint)*(ushort *)(image_desc + 6));
        if ((image_desc[8] & 0x80U) != 0) {
          dbg_log((const char *)&str_warning_skipping_local_color_table_2);
        }
        SetRect(&rect,0,0,width2,height2);
        resize(&rect);
        lzw_min_code = image_desc[9];
        image_data = image_desc + 10;
        win_flush_pending_msgs();
        gif_lzw_decode((byte *)image_data,first_color,(ushort)lzw_min_code
                     ,stride,width2,height2,(i8 *)bits);
        win_flush_pending_msgs();
        set_palette(palette);
        result = dib_is_top_down(dib);
        if (result == 0) {
          dib_flip_vertical(dib);
        }
        res_unlock(res);
        res_free(res);
        ret = 1;
      }
      else {
        dbg_logf((const char *)&str_error_gif_image_block_not_found_2,res_id);
        res_unlock(res);
        res_free(res);
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(4,(const char*)&str_err_badpicformat_6,(const char*)&str_d_ss_smeds32_port_cpp_9,0x424);
        ret = 0;
      }
    }
  }
  return ret;
}

/* 005c0034 auto */
void DibSurface::reset_clip()

{
  SetRect((LPRECT)&clip_left,bounds_rect,bounds_top,bounds_right,bounds_bottom);
  return;
}

/* 005c0073 auto */
void DibSurface::set_clip(RECT *rect)

{
  SetRect((LPRECT)&clip_left,rect->left,rect->top,rect->right,
          rect->bottom);
  IntersectRect((LPRECT)&clip_left,(RECT *)&clip_left,(RECT *)&bounds_rect);
  return;
}

/* 005c00ce auto */
void DibSurface::get_clip(RECT *out_rect)

{
  out_rect->left = clip_left;
  out_rect->top = clip_top;
  out_rect->right = clip_right;
  out_rect->bottom = clip_bottom;
  return;
}

/* 005c0105 auto */
i32 __thiscall surface_alloc_dib_by_bpp(DibSurface *self,RECT *rect)

{
  i32 result;
  
  switch(self->bytes_per_pixel) {
  case 1:
    result = self->alloc_dib8(rect);
    break;
  case 2:
    result = self->alloc_dib16(rect);
    break;
  case 3:
    result = self->alloc_dib24(rect);
    break;
  case 4:
    result = self->alloc_dib32(rect);
    break;
  default:
    result = 0;
  }
  return result;
}

/* 005c019d auto */
i32 DibSurface::resize(RECT *rect)

{
  i32 result;
  
  result = alloc_dib8(rect);
  return result;
}

/* 005c01c1 auto */
void DibSurface::lock()

{
  uptr new_bits;
  void *offsets;
  int top_down;
  int row;
  
  if (bits == 0) {
    new_bits = dib_get_bits(dib);
    bits = new_bits;
    if (bits != 0) {
      offsets = operator_new(pixel_height << 2);
      row_offsets = (uptr)offsets;
      top_down = dib_is_top_down(dib);
      if (top_down == 0) {
        *(u32 *)row_offsets = (pixel_height - 1) * stride;
        for (row = 1; row < (int)pixel_height; row++) {
          *(u32 *)(row_offsets + row * 4) =
               *(int *)((row_offsets - 4) + row * 4) - stride;
        }
      }
      else {
        *(i32 *)row_offsets = 0;
        for (row = 1; row < (int)pixel_height; row++) {
          *(u32 *)(row_offsets + row * 4) =
               *(int *)((row_offsets - 4) + row * 4) + stride;
        }
      }
    }
  }
  return;
}

/* 005c02e0 auto */
void DibSurface::unlock()

{
  uptr new_bits;
  
  if (bits != 0) {
    new_bits = dib_unlock_bits(dib);
    bits = new_bits;
    operator_delete((void *)row_offsets);
  }
  return;
}

/* 005c0333 auto */
void DibSurface::fill_rect(RECT *rect,i32 color)

{
  char locked;
  i32 surf_height;
  GdiDib *dib;
  int top_down;
  int pitch;
  tagRECT clipped;
  int height2;
  int width2;
  i32 was_locked;
  
  was_locked = 1;
  locked = is_locked();
  if (locked == '\0') {
    dbg_log((const char *)&str_warning_port_not_locked_in_fillcolor);
    was_locked = 0;
    lock();
  }
  IntersectRect(&clipped,rect,(RECT *)&clip_left);
  width2 = rect_width(&clipped);
  if ((width2 != 0) && (height2 = rect_height(&clipped), height2 != 0)) {
    surf_height = surface_height();
    dib = get_dib();
    top_down = dib_is_top_down(dib);
    pitch = get_pitch();
    dib_fill_rect8((u8 *)bits,color,clipped.left,clipped.top,width2,height2,
                 ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch,surf_height);
  }
  return;
}

/* 005c041f auto */
void DibSurface::fill(i32 color)

{
  fill_rect((RECT *)&clip_left,color);
  return;
}

/* 005c044a auto */
void DibSurface::replace_color_clip(i32 from_color,i32 to_color)

{
  replace_color((RECT *)&clip_left,from_color,to_color);
  return;
}

/* 005c0479 auto */
void DibSurface::replace_color(RECT *rect,i32 from_color,i32 to_color)

{
  char locked;
  iptr height;
  iptr row_width;
  int next_row;
  char *pixel;
  tagRECT clipped;
  int row;
  int col;
  iptr width2;
  int was_locked;
  
  was_locked = 1;
  locked = is_locked();
  if (locked == '\0') {
    dbg_log((const char *)&str_warning_port_not_locked_in_changecolor);
    was_locked = 0;
    lock();
  }
  IntersectRect(&clipped,rect,(RECT *)&clip_left);
  width2 = rect_width(&clipped);
  pixel = (char *)pixel_ptr(clipped.left,clipped.top);
  row = 0;
  while( true ) {
    height = rect_height(&clipped);
    if (height <= row) break;
    col = 0;
    while( true ) {
      row_width = rect_width(&clipped);
      if (row_width <= col) break;
      if (*pixel == (char)from_color) {
        *pixel = (char)to_color;
      }
      pixel++;
      col++;
    }
    next_row = (int)this->next_row((u8 *)pixel);
    pixel = (char *)(next_row - width2);
    row++;
  }
  if (was_locked == 0) {
    unlock();
  }
  return;
}

/* 005c0593 auto */
void DibSurface::blit(DibSurface *src,RECT *dest_rect,RECT *src_rect)

{
  GdiDib *dib;
  int top_down;
  int src_pitch;
  int dest_pitch;
  i32 src_height;
  i32 dest_height;
  int row_bytes;
  int src_x_bytes;
  int dest_x_bytes;
  i32 src_bits;
  i32 dest_bits;
  bool was_unlocked;
  tagRECT src_clip;
  tagRECT dest_clip;
  int src_was_locked;
  int height2;
  int width2;
  
  src_was_locked = 1;
  was_unlocked = bits == 0;
  if (was_unlocked) {
    dbg_log((const char *)&str_warning_source_port_not_locked_in_copyto);
    lock();
  }
  if (src->bits == 0) {
    dbg_log((const char *)&str_warning_destination_port_not_locked_in_c);
    src->lock();
    src_was_locked = 0;
  }
  IntersectRect(&dest_clip,dest_rect,(RECT *)&clip_left);
  IntersectRect(&src_clip,src_rect,(RECT *)&src->clip_left);
  width2 = src_clip.right - src_clip.left;
  if (dest_clip.right - dest_clip.left <= src_clip.right - src_clip.left) {
    width2 = dest_clip.right - dest_clip.left;
  }
  height2 = src_clip.bottom - src_clip.top;
  if (dest_clip.bottom - dest_clip.top <= src_clip.bottom - src_clip.top) {
    height2 = dest_clip.bottom - dest_clip.top;
  }
  if ((width2 != 0) && (height2 != 0)) {
    dib = src->get_dib();
    top_down = dib_is_top_down(dib);
    src_pitch = src->get_pitch();
    src_pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * src_pitch;
    dib = get_dib();
    top_down = dib_is_top_down(dib);
    dest_pitch = get_pitch();
    dest_pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * dest_pitch;
    src_height = src->surface_height();
    dest_height = surface_height();
    row_bytes = bytes_per_pixel * width2;
    src_x_bytes = bytes_per_pixel * src_clip.left;
    dest_x_bytes = bytes_per_pixel * dest_clip.left;
    top_down = height2;
    src_bits = src->get_bits();
    dest_bits = get_bits();
    gfx_copy_pixels(dest_bits,src_bits,dest_x_bytes,dest_clip.top,src_x_bytes,src_clip.top,row_bytes,top_down,dest_height,src_height,dest_pitch,
                 src_pitch);
  }
  if (was_unlocked) {
    unlock();
  }
  if (src_was_locked == 0) {
    src->unlock();
  }
  return;
}

/* 005c0979 auto */
void DibSurface::blit_to_wnd(WndBase *wnd,RECT *src_rect,RECT *dest_pt)

{
  char locked;
  i32 dest_y;
  NativeWindow *window;
  i32 height;
  i32 width;
  i32 dest_x;
  
  if (dib == 0) {
    dbg_log((const char *)&str_error_null_port_in_copytoscreen);
  }
  else {
    locked = is_locked();
    if (locked == '\0') {
      dbg_log((const char *)&str_warning_port_not_locked_in_copytoscreen);
      lock();
    }
    dest_y = (wnd)->get_palette_obj();
    apply_palette((GamePalette *)dest_y);
    dest_y = dest_pt->top;
    dest_x = dest_pt->left;
    window = (wnd)->get_window();
    height = rect_height(src_rect);
    width = rect_width(src_rect);
    dib_blit_to_wnd_obj(dib,src_rect->left,src_rect->top,width,height,
                 window,dest_x,dest_y);
    if (locked == '\0') {
      unlock();
    }
  }
  return;
}

/* 005c0bf2 auto */
uint DibSurface::get_pixel(i32 x,i32 y)

{
  char in_clip;
  uint color;
  byte *pixel;
  
  in_clip = this->in_clip(x,y);
  if (in_clip == '\0') {
    dbg_logf_int2((const char *)&str_warning_getpixel_out_of_clip_region,x,y);
    color = 0;
  }
  else {
    pixel = (byte *)pixel_ptr(x,y);
    color = (uint)*pixel;
  }
  return color;
}

/* 005c0c5d auto */
void DibSurface::set_pixel(i32 x,i32 y,i32 color)

{
  char in_clip;
  i8 *pixel;
  
  in_clip = this->in_clip(x,y);
  if (in_clip == '\0') {
    dbg_logf_int2((const char *)&str_warning_setpixel_out_of_clip_region,x,y);
  }
  else {
    pixel = (i8 *)pixel_ptr(x,y);
    *pixel = (i8)color;
  }
  return;
}

/* 005c0cc5 auto */
void DibSurface::set_palette(GamePalette *palette)

{
  u32 version;
  
  version = (palette)->get_version();
  if (version != palette_version) {
    dib_set_palette(dib,palette);
    version = (palette)->get_version();
    palette_version = version;
  }
  return;
}

/* 005c0d12 auto */
void DibSurface::apply_palette(GamePalette *palette)

{
  u32 version;
  
  if (palette != (GamePalette *)0x0) {
    version = palette->get_version();
    if (version != palette_version) {
      dib_set_palette(dib,palette);
      version = palette->get_version();
      palette_version = version;
    }
  }
  return;
}

/* 005c0d69 auto */
void DibSurface::draw_text_cur_font(const char *text,int x,int y,uint flags)

{
  i8 old_color;
  int font_height;
  int shadow_offset;
  i32 saved_color;
  
  if (draw_target.font == (i8 *)0x0) {
    dbg_log((const char *)&str_error_no_current_font_selected);
  }
  else if (dib != 0) {
    if ((flags & 0x10) != 0) {
      font_height = font_measure_height(*(i32 *)draw_target.font);
      font_height += (font_height >> 0x1f & 7U);
      shadow_offset = font_height >> 3;
      old_color = gfx_set_text_color_index(display_palette.black_index);
      saved_color = (saved_color & 0xffffff00 | (byte)(old_color));
      dib_draw_text_at(dib,*(i32 *)draw_target.font,(char *)text,shadow_offset + x,
                   shadow_offset + y,(RECT *)&clip_left,flags);
      gfx_set_text_color_index(saved_color);
    }
    dib_draw_text_at(dib,*(i32 *)draw_target.font,(char *)text,x,y,
                 (RECT *)&clip_left,flags);
  }
  return;
}

/* 005c0f57 auto */
void DibSurface::draw_text(GameFont *font,const char *text,int x,int y,uint flags)

{
  i8 old_color;
  int font_height;
  int shadow_offset;
  i32 saved_color;
  
  if (dib != 0) {
    if ((flags & 0x10) != 0) {
      font_height = font_measure_height(*(i32 *)font);
      font_height += (font_height >> 0x1f & 7U);
      shadow_offset = font_height >> 3;
      old_color = gfx_set_text_color_index(display_palette.black_index);
      saved_color = (saved_color & 0xffffff00 | (byte)(old_color));
      dib_draw_text_at(dib,*(i32 *)font,(char *)text,x + shadow_offset,y + shadow_offset,
                   (RECT *)&clip_left,flags);
      gfx_set_text_color_index(saved_color);
    }
    dib_draw_text_at(dib,*(i32 *)font,(char *)text,x,y,(RECT *)&clip_left
                 ,flags);
  }
  return;
}

/* 005c1020 auto */
void DibSurface::draw_text_in_rect(GameFont *font,const char *text,RECT *rect,uint flags)

{
  i8 old_color;
  int shadow_offset;
  BOOL offset_ok;
  i32 saved_color;
  tagRECT shadow_rect;
  
  if (dib != 0) {
    if ((flags & 0x10) != 0) {
      shadow_offset = font_measure_height(*(i32 *)font);
      shadow_offset = (int)(shadow_offset + (shadow_offset >> 0x1f & 7U)) >> 3;
      shadow_rect.left = rect->left;
      shadow_rect.top = rect->top;
      shadow_rect.right = rect->right;
      shadow_rect.bottom = rect->bottom;
      offset_ok = OffsetRect(&shadow_rect,shadow_offset,shadow_offset);
      old_color = gfx_set_text_color_index(display_palette.black_index);
      saved_color = (saved_color & 0xffffff00 | (byte)(old_color));
      dib_draw_text_rect(dib,*(i32 *)font,(char *)text,&shadow_rect,flags);
      gfx_set_text_color_index(saved_color);
    }
    dib_draw_text_rect(dib,*(i32 *)font,(char *)text,rect,flags);
  }
  return;
}

/* 005c1167 auto */
void DibSurface::draw_text_rect(GameFont *font,const char *text,RECT *rect,i32 flags)

{
  if (dib != 0) {
    dib_draw_text_wrap(dib,*(i32 *)font,(char *)text,rect,flags);
  }
  return;
}

/* 005c11b2 auto */
void DibSurface::draw_line(u32 x1,u32 y1,u32 x2,u32 y2)

{
  iptr handle;
  int top_down;
  int pitch;
  i32 height;
  i32 color;
  u32 start;
  
  if (((((((int)clip_left <= (int)x1) || ((int)clip_left <= (int)x2)) &&
        (((int)x2 < (int)clip_right || ((int)x1 < (int)clip_right)))) &&
       (((int)clip_top <= (int)y1 || ((int)clip_top <= (int)y2)))) &&
      (((int)y1 < (int)clip_bottom || ((int)y2 < (int)clip_bottom)))) &&
     (dib != 0)) {
    if (y2 == y1) {
      if ((int)x1 < (int)clip_left) {
        x1 = clip_left;
      }
      if ((int)clip_right <= (int)x1) {
        x1 = clip_right - 1;
      }
      if ((int)x2 < (int)clip_left) {
        x2 = clip_left;
      }
      if ((int)clip_right <= (int)x2) {
        x2 = clip_right - 1;
      }
      if (x2 != x1) {
        handle = (iptr)get_dib();
        top_down = dib_is_top_down((GdiDib *)handle);
        pitch = get_pitch();
        pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
        height = surface_height();
        start = x2;
        if ((int)x1 <= (int)x2) {
          start = x1;
        }
        top_down = crt_abs(x2 - x1);
        top_down++;
        color = *(u8 *)&ctrl_colors.gfx_text;
        handle = get_bits();
        dib_hline8((u8 *)handle,color,top_down,start,y1,height,pitch);
      }
    }
    else if (x2 == x1) {
      if ((int)y1 < (int)clip_top) {
        y1 = clip_top;
      }
      if ((int)clip_bottom <= (int)y1) {
        y1 = clip_bottom - 1;
      }
      if ((int)y2 < (int)clip_top) {
        y2 = clip_top;
      }
      if ((int)clip_bottom <= (int)y2) {
        y2 = clip_bottom - 1;
      }
      if (y2 != y1) {
        handle = (iptr)get_dib();
        top_down = dib_is_top_down((GdiDib *)handle);
        pitch = get_pitch();
        pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
        height = surface_height();
        start = y2;
        if ((int)y1 <= (int)y2) {
          start = y1;
        }
        top_down = crt_abs(y2 - y1);
        top_down++;
        color = *(u8 *)&ctrl_colors.gfx_text;
        handle = get_bits();
        dib_vline8((u8 *)handle,color,top_down,x1,start,height,pitch);
      }
    }
    else {
      dib_draw_line(dib,x1,y1,x2,y2);
    }
  }
  return;
}

/* 005c19ad auto */
i8 __cdecl gfx_set_text_color_index(i32 color_index)

{
  i8 prev_index;
  
  prev_index = (*(i8 *)&ctrl_colors.gfx_text);
  (*(i8 *)&ctrl_colors.gfx_text) = (i8)color_index;
  return prev_index;
}

/* 005c19d3 auto */
int DibSurface::pixel_ptr(int x,int y)

{
  return *(int *)(row_offsets + y * 4) + bits + x;
}

/* 005c1a00 auto */
i8 DibSurface::in_clip(int x,int y)

{
  i8 inside;
  
  if ((((x < (int)clip_left) || ((int)clip_right <= x)) ||
      (y < (int)clip_top)) || ((int)clip_bottom <= y)) {
    inside = 0;
  }
  else {
    inside = 1;
  }
  return inside;
}

/* 005c1a62 auto */
void DibSurface::remap_colors(u8 *remap_table,int base_index,i32 unused)

{
  byte *pixel;
  int col;
  int row;
  
  if (bits != 0) {
    pixel = (byte *)bits;
    for (row = 0; row < (int)pixel_height; row++) {
      for (col = 0; col < (int)pixel_width; col++) {
        if (-1 < (int)((uint)*pixel - base_index)) {
          *pixel = *(((uint)*pixel - base_index) + remap_table);
        }
        pixel++;
      }
      pixel += (stride - pixel_width);
    }
  }
  return;
}

/* 005c1b47 auto */
i32 DibSurface::alloc_dib16(RECT *rect)

{
  char locked;
  u32 value;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    value = dib_destroy(dib);
    dib = (GdiDib *)value;
  }
  if (((rect != 0) && (dim = rect_width(rect), dim != 0)) &&
     (dim = rect_height(rect), dim != 0)) {
    set_dims((int *)rect);
    value = (u32)dib_create16((RECT *)&bounds_rect);
    dib = (GdiDib *)value;
    if (dib != 0) {
      value = dib_get_stride(dib);
      stride = value;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      bytes_per_pixel = 2;
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(const char*)&str_err_portallocfailed_2,(const char*)&str_d_ss_smeds32_port_cpp_10,0x86c);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005c1cf7 auto */
i32 DibSurface::alloc_dib24(RECT *rect)

{
  char locked;
  u32 value;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    value = dib_destroy(dib);
    dib = (GdiDib *)value;
  }
  if (((rect != 0) && (dim = rect_width(rect), dim != 0)) &&
     (dim = rect_height(rect), dim != 0)) {
    set_dims((int *)rect);
    value = (u32)dib_create24((RECT *)&bounds_rect);
    dib = (GdiDib *)value;
    if (dib != 0) {
      value = dib_get_stride(dib);
      stride = value;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      bytes_per_pixel = 3;
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(const char*)&str_err_portallocfailed_3,(const char*)&str_d_ss_smeds32_port_cpp_11,0x8a1);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005c1ea7 auto */
i32 DibSurface::alloc_dib32(RECT *rect)

{
  char locked;
  u32 value;
  int dim;
  
  if (dib != 0) {
    locked = is_locked();
    if (locked != '\0') {
      unlock();
    }
    value = dib_destroy(dib);
    dib = (GdiDib *)value;
  }
  if (((rect != 0) && (dim = rect_width(rect), dim != 0)) &&
     (dim = rect_height(rect), dim != 0)) {
    set_dims((int *)rect);
    value = (u32)dib_create32((RECT *)&bounds_rect);
    dib = (GdiDib *)value;
    if (dib != 0) {
      value = dib_get_stride(dib);
      stride = value;
      dim = dib_is_top_down(dib);
      if (dim == 0) {
        signed_pitch = -stride;
      }
      else {
        signed_pitch = stride;
      }
      bytes_per_pixel = 4;
      lock();
      return 1;
    }
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(2,(const char*)&str_err_portallocfailed_4,(const char*)&str_d_ss_smeds32_port_cpp_12,0x8d6);
    return 0;
  }
  set_dims(0);
  return 1;
}

/* 005c54f0 auto */
bool DibSurface::is_locked()

{
  return bits != 0;
}

/* 005c55a0 auto */
int __cdecl dib_row_align4(int width)

{
  return ((int)(width + 3 + (width + 3 >> 0x1f & 3U)) >> 2) << 2;
}

/* 005c55d0 auto */
u32 DibSurface::get_pitch()

{
  return stride;
}

/* 005c55f0 auto */
/* Library Function - Single Match
    public: unsigned int __thiscall CCheckListBox::GetCheckStyle(void)
   
   Library: Visual Studio 1998 Debug */

GdiDib *DibSurface::get_dib()

{
  return dib;
}

/* 005c5610 auto */
u8 *DibSurface::next_row(u8 *row_ptr)

{
  return signed_pitch + row_ptr;
}

/* 005c5640 auto */
uptr DibSurface::get_bits()

{
  return bits;
}

/* 005c56f0 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 GamePalette::get_version()

{
  return pal_version;
}

/* 005c5710 auto */
u8 *DibSurface::prev_row(u8 *row_ptr)

{
  return row_ptr - signed_pitch;
}

/* 005c6480 auto */
void GamePalette::commit_range(int first,int count)

{
  if (0x100 < count + first) {
    count = 0x100 - first;
  }
  logpal_apply_range((GameLogPalette *)this,hpalette,first,count);
  new_version();
  return;
}

/* 005c64da auto */
GamePalette *GamePalette::init()

{
  u32 hpal;
  
  logpal_init_from_system((GameLogPalette *)this);
  hpal = logpal_create_hpalette((GameLogPalette *)this);
  hpalette = hpal;
  new_version();
  tint_red = '\0';
  tint_green = '\0';
  tint_blue = '\0';
  tint_saved_rgb = 0;
  fade_dst_rgb = tint_saved_rgb;
  fade_src_rgb = fade_dst_rgb;
  return this;
}

/* 005c656b auto */
GamePalette *GamePalette::destroy()

{
  GamePalette *result;

  result = this;
  logpal_delete_hpalette(hpalette);
  if (fade_src_rgb != 0) {
    mem_handle_free(fade_src_rgb);
  }
  if (fade_dst_rgb != 0) {
    mem_handle_free(fade_dst_rgb);
  }
  if (tint_saved_rgb != 0) {
    result = (GamePalette *)mem_handle_free(tint_saved_rgb);
  }
  return result;
}

/* 005c65f9 auto */
void GamePalette::load_ctab_res(i32 res_id)

{
  int res;
  i32 *data;
  i32 first;
  i32 count;
  char res_type[4];
  
  res_type[0] = 0x43;
  res_type[1] = 0x54;
  res_type[2] = 0x41;
  res_type[3] = 0x42;
  res = res_load(res_type,res_id);
  if (res == 0) {
    dbg_logf_noret((const char *)&str_error_color_resource_not_found,res_id);
  }
  else {
    data = (i32 *)res_lock(res);
    first = endian_from_le(*data);
    count = endian_from_le(data[1]);
    set_rgb(first,count,((u8 *)(data + 2)));
    res_unlock(res);
    res_free(res);
  }
  return;
}

/* 005c6a42 auto */
void GamePalette::new_version()

{
  uint rnd;
  
  pal_version = 0;
  while (pal_version == 0) {
    rnd = crt_rand_at("palette_new_version");
    pal_version = rnd & 0x7fff;
  }
  return;
}

/* 005c6a8d auto */
void GamePalette::reserve_range(i32 first,i32 count)

{
  logpal_reserve_range((GameLogPalette *)this,first,count);
  commit_range(first,count);
  return;
}

/* 005c6b63 auto */
void GamePalette::get_rgb(u8 *dest,i32 first,i32 count)

{
  palette_copy_rgb_range((GameLogPalette *)this,dest,first,count);
  return;
}

/* 005c6b93 auto */
void GamePalette::set_entry(i32 index,i32 red,i32 green, i32 blue)

{
  palette_set_entry_rgb((GameLogPalette *)this,index,red,green,blue);
  commit_range(index,1);
  return;
}

/* 005c6da8 auto */
void GamePalette::set_rgb(int first,int count,u8 *rgb)

{
  i32 hi_bits;
  i32 index;
  
  if (0x100 < count + first) {
    count = 0x100 - first;
  }
  for (index = first; index < count + first; index++) {
    hi_bits = (i32)((uint)rgb >> 8);
    palette_set_entry_rgb((GameLogPalette *)this,index,*rgb,*(rgb + 1),*(rgb + 2));
    rgb += 3;
  }
  commit_range(first,count);
  return;
}

/* 005c71f3 auto */
void GamePalette::fade_begin(u32 first,u32 count,u32 steps,GamePalette *target_pal)

{
  u32 handle;
  iptr dst_buf;
  GamePalette *target_logpal;
  u32 count_arg;
  
  fade_steps = steps;
  fade_first = first;
  fade_count = count;
  handle = mem_handle_alloc(count * 3);
  fade_src_rgb = handle;
  handle = mem_handle_alloc(count * 3);
  fade_dst_rgb = handle;
  dst_buf = (iptr)mem_handle_lock(fade_src_rgb);
  palette_copy_rgb_range((GameLogPalette *)this,(u8 *)dst_buf,first,count);
  mem_handle_unlock(fade_src_rgb);
  dst_buf = (iptr)mem_handle_lock(fade_dst_rgb);
  handle = first;
  count_arg = count;
  target_logpal = (target_pal)->get_logpalette();
  palette_copy_rgb_range((GameLogPalette *)target_logpal,(u8 *)dst_buf,handle,count_arg);
  mem_handle_unlock(fade_dst_rgb);
  reserve_range(first,count);
  return;
}

/* 005c72f8 auto */
void GamePalette::fade_end()

{
  iptr src_buf;
  u32 handle;
  
  src_buf = (iptr)mem_handle_lock(fade_src_rgb);
  set_rgb(fade_first,fade_count,(u8 *)src_buf);
  mem_handle_unlock(fade_src_rgb);
  handle = mem_handle_free(fade_src_rgb);
  fade_src_rgb = handle;
  handle = mem_handle_free(fade_dst_rgb);
  fade_dst_rgb = handle;
  return;
}

/* 005c738e auto */
void GamePalette::fade_step(int step)

{
  char dst_weight;
  char src_weight;
  iptr src_buf;
  iptr dst_buf;
  iptr offset;
  i32 green;
  i32 red;
  i32 blue;
  
  if ((step < 0) || ((int)fade_steps < step)) {
    dbg_logf((const char *)&str_color_scale_factor_out_of_range,step);
  }
  else {
    src_weight = (char)step;
    dst_weight = (char)fade_steps - src_weight;
    src_buf = (iptr)mem_handle_lock(fade_src_rgb);
    dst_buf = (iptr)mem_handle_lock(fade_dst_rgb);
    for (offset = 0; offset < (int)(fade_count * 3); offset += 3) {
      red = (red & 0xffffff00 | (byte)(
                          (char)((longlong)(ulonglong)*(byte *)(offset + dst_buf) /
                                (longlong)(int)fade_steps) * dst_weight +
                          (char)((longlong)(ulonglong)*(byte *)(offset + src_buf) /
                                (longlong)(int)fade_steps) * src_weight));
      green = (green & 0xffffff00 | (byte)(
                          (char)((longlong)(ulonglong)*(byte *)(offset + 1 + src_buf) /
                                (longlong)(int)fade_steps) * src_weight +
                          (char)((longlong)(ulonglong)*(byte *)(offset + 1 + dst_buf) /
                                (longlong)(int)fade_steps) * dst_weight));
      blue = (blue & 0xffffff00 | (byte)(
                         (char)((longlong)(ulonglong)*(byte *)(offset + 2 + dst_buf) /
                               (longlong)(int)fade_steps) * dst_weight +
                         (char)((longlong)(ulonglong)*(byte *)(offset + 2 + src_buf) /
                               (longlong)(int)fade_steps) * src_weight));
      logpal_set_entry((GameLogPalette *)this,fade_first + offset / 3,red,green,blue);
    }
    palette_animate((GameLogPalette *)this,hpalette,fade_first,fade_count);
    mem_handle_unlock(fade_src_rgb);
    mem_handle_unlock(fade_dst_rgb);
  }
  return;
}

/* 005c8200 auto */
i32 __cdecl font_handle_create(i32 face,int size,i32 style)

{
  LOGFONTA logfont;
  i32 handle;
  HFONT hfont2;
  i32 *data;
  
  logfont.lfHeight = -size;
  logfont.lfWidth = 0;
  logfont.lfEscapement = 0;
  logfont.lfOrientation = 0;
  logfont.lfCharSet = 1;
  logfont.lfOutPrecision = 4;
  logfont.lfClipPrecision = 1;
  logfont.lfQuality = '\0';
  logfont.lfPitchAndFamily = '\0';
  if (((byte)style & 1) == 0) {
    logfont.lfWeight = 400;
  }
  else {
    logfont.lfWeight = 700;
  }
  logfont.lfItalic = (byte)style & 2;
  logfont.lfUnderline = (byte)style & 4;
  logfont.lfStrikeOut = (byte)style & 8;
  switch(face) {
  case 0:
    logfont.lfPitchAndFamily = 16;
    crt_strcpy(logfont.lfFaceName,str_times_new_roman);
    break;
  case 1:
    logfont.lfPitchAndFamily = ' ';
    crt_strcpy(logfont.lfFaceName,str_arial);
    break;
  case 2:
    crt_strcpy(logfont.lfFaceName,str_system_2);
    break;
  case 3:
    logfont.lfPitchAndFamily = '0';
    crt_strcpy(logfont.lfFaceName,str_courier);
  }
  handle = 0;
  hfont2 = CreateFontIndirectA(&logfont);
  if (hfont2 != (HFONT)0x0) {
    handle = mem_handle_alloc(8);
    data = (i32 *)mem_handle_lock(handle);
    *data = hfont2;
    data[1] = 1;
    mem_handle_unlock(handle);
  }
  return handle;
}

/* 005c841d auto */
i32 __cdecl font_handle_destroy(i32 handle)

{
  i32 *data;
  i32 result;
  
  data = (i32 *)mem_handle_lock(handle);
  if (data == (i32 *)0x0) {
    result = 0;
  }
  else {
    if (data[1] == 1) {
      DeleteObject((HGDIOBJ)*data);
    }
    mem_handle_unlock(handle);
    result = mem_handle_free(handle);
  }
  return result;
}

/* 005c847f auto */
LONG __cdecl font_measure_height(i32 handle)

{
  int size;
  HDC hdc;
  HGDIOBJ h;
  tagTEXTMETRICA metrics;
  i32 *data;
  
  size = mem_handle_size(handle);
  if (size == 0) {
    metrics.tmHeight = 1;
  }
  else {
    data = (i32 *)mem_handle_lock(handle);
    hdc = GetDC((HWND)0x0);
    h = SelectObject(hdc,(HGDIOBJ)*data);
    GetTextMetricsA(hdc,&metrics);
    SelectObject(hdc,h);
    ReleaseDC((HWND)0x0,hdc);
    mem_handle_unlock(handle);
  }
  return metrics.tmHeight;
}

/* 005c8514 auto */
int __cdecl font_handle_line_spacing(i32 handle)

{
  HDC hdc;
  HGDIOBJ h;
  tagTEXTMETRICA metrics;
  i32 *data;
  
  data = (i32 *)mem_handle_lock(handle);
  hdc = GetDC((HWND)0x0);
  h = SelectObject(hdc,(HGDIOBJ)*data);
  GetTextMetricsA(hdc,&metrics);
  SelectObject(hdc,h);
  ReleaseDC((HWND)0x0,hdc);
  mem_handle_unlock(handle);
  return metrics.tmExternalLeading + metrics.tmHeight;
}

/* 005c858e auto */
LONG __cdecl font_measure_width(i32 handle,const char *text)

{
  HDC hdc;
  HGDIOBJ h;
  size_t c;
  tagSIZE *lpsz;
  tagSIZE extent;
  i32 *data;
  
  data = (i32 *)mem_handle_lock(handle);
  hdc = GetDC((HWND)0x0);
  h = SelectObject(hdc,(HGDIOBJ)*data);
  lpsz = &extent;
  c = _strlen((char *)text);
  GetTextExtentPointA(hdc,text,c,lpsz);
  SelectObject(hdc,h);
  ReleaseDC((HWND)0x0,hdc);
  mem_handle_unlock(handle);
  return extent.cx;
}

/* 005c8791: written by hand in src/game/fixups.cpp */

/* 005c8c83 auto */
void __cdecl region_dispose(int region)

{
  if (region == 0) {
    dbg_log((const char *)&str_error_tried_to_dispose_null_region_in_ms);
  }
  return;
}

/* 005c8caf: written by hand in src/game/fixups.cpp */

/* 005cbdb0 auto */
/* Library Function - Single Match
    public: int __thiscall CSplitterWnd::IsTracking(void)
   
   Library: Visual Studio 1998 Debug */

u32 GamePalette::get_hpalette()

{
  return hpalette;
}

/* 005cd6e0 auto */
void __cdecl gfx_scale_init(void)

{
  int i;
  
  for (i = 0; i < 0x10; i++) {
    default_draw_font_obj_block.gfx_scale_caches[i].last_used = 0;
    default_draw_font_obj_block.gfx_scale_caches[i].num = 0;
    default_draw_font_obj_block.gfx_scale_caches[i].den = 0;
  }
  gfx_set_scale_ratio(1,1);
  gfx_set_scale_tables(1,1,1,1);
  return;
}

/* 005cd775 auto */
void __cdecl gfx_set_scale_ratio(int num,int den)

{
  i32 now;
  int slot;
  int index;
  int oldest_tick;
  int accum;
  int value;
  
  if (num < 2) {
    num = 1;
  }
  if (den < 2) {
    den = 1;
  }
  gfx_scale.num = num;
  gfx_scale.den = den;
  oldest_tick = timer_ticks();
  slot = -1;
  for (index = 0; index < 0x10; index++) {
    if ((default_draw_font_obj_block.gfx_scale_caches[index].num == gfx_scale.num) &&
       (default_draw_font_obj_block.gfx_scale_caches[index].den == gfx_scale.den)) {
      now = timer_ticks();
      default_draw_font_obj_block.gfx_scale_caches[index].last_used = now;
      slot = index;
      break;
    }
    if (default_draw_font_obj_block.gfx_scale_caches[index].last_used <= oldest_tick) {
      oldest_tick = default_draw_font_obj_block.gfx_scale_caches[index].last_used;
      slot = index;
    }
  }
  if (0xf < index) {
    default_draw_font_obj_block.gfx_scale_caches[slot].num = gfx_scale.num;
    default_draw_font_obj_block.gfx_scale_caches[slot].den = gfx_scale.den;
    now = timer_ticks();
    default_draw_font_obj_block.gfx_scale_caches[slot].last_used = now;
    value = 0;
    if (gfx_scale.num < gfx_scale.den) {
      accum = 0;
      index = 0;
      while (index < 0x400) {
        for (; (-1 < accum && (index < 0x400)); index++) {
          default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
          accum -= gfx_scale.den;
        }
        accum += gfx_scale.num;
        value++;
      }
    }
    else {
      accum = gfx_scale.num;
      index = 0;
      while (index < 0x400) {
        for (; (gfx_scale.den <= accum && (index < 0x400)); index++) {
          default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
          accum -= gfx_scale.den;
        }
        accum += gfx_scale.num;
        value++;
      }
      }
  }
  gfx_scale_tables.table = (i32 *)default_draw_font_obj_block.gfx_scale_caches[slot].table;
  return;
}

/* 005cda06 auto */
void __cdecl gfx_get_scale_ratio(i32 *out_num,i32 *out_den)

{
  *out_num = gfx_scale.num;
  *out_den = gfx_scale.den;
  return;
}

/* 005cda2a auto */
void __cdecl gfx_set_scale_tables(int x_num,int x_den,int y_num,int y_den)

{
  i32 now;
  int index;
  int oldest_tick;
  int y_slot;
  int x_slot;
  
  if (x_num < 2) {
    x_num = 1;
  }
  if (x_den < 2) {
    x_den = 1;
  }
  if (y_num < 2) {
    y_num = 1;
  }
  if (y_den < 2) {
    y_den = 1;
  }
  gfx_scale.x_num = x_num;
  gfx_scale.x_den = x_den;
  gfx_scale.y_num = y_num;
  gfx_scale.y_den = y_den;
  oldest_tick = timer_ticks();
  x_slot = -1;
  for (index = 0; index < 0x10; index++) {
    if ((default_draw_font_obj_block.gfx_scale_caches[index].num == gfx_scale.x_num) &&
       (default_draw_font_obj_block.gfx_scale_caches[index].den == gfx_scale.x_den)) {
      now = timer_ticks();
      default_draw_font_obj_block.gfx_scale_caches[index].last_used = now;
      x_slot = index;
      break;
    }
    if (default_draw_font_obj_block.gfx_scale_caches[index].last_used <= oldest_tick) {
      oldest_tick = default_draw_font_obj_block.gfx_scale_caches[index].last_used;
      x_slot = index;
    }
  }
  if (0xf < index) {
    gfx_scale_build_table(x_slot,gfx_scale.x_num,gfx_scale.x_den);
  }
  gfx_scale_tables.x_table = (i8 *)default_draw_font_obj_block.gfx_scale_caches[x_slot].table;
  oldest_tick = timer_ticks();
  y_slot = -1;
  for (index = 0; index < 0x10; index++) {
    if ((default_draw_font_obj_block.gfx_scale_caches[index].num == gfx_scale.y_num) &&
       (default_draw_font_obj_block.gfx_scale_caches[index].den == gfx_scale.y_den)) {
      now = timer_ticks();
      default_draw_font_obj_block.gfx_scale_caches[index].last_used = now;
      break;
    }
    if ((default_draw_font_obj_block.gfx_scale_caches[index].last_used <= oldest_tick) && (x_slot != index)) {
      oldest_tick = default_draw_font_obj_block.gfx_scale_caches[index].last_used;
      y_slot = index;
    }
  }
  if (0xf < index) {
    gfx_scale_build_table(y_slot,gfx_scale.y_num,gfx_scale.y_den);
    index = y_slot;
  }
  gfx_scale_tables.y_table = (i8 *)default_draw_font_obj_block.gfx_scale_caches[index].table;
  return;
}

/* 005cdcdb auto */
void __cdecl gfx_scale_build_table(int slot,int num,int den)

{
  i32 now;
  int index;
  int accum;
  int value;
  
  default_draw_font_obj_block.gfx_scale_caches[slot].num = num;
  default_draw_font_obj_block.gfx_scale_caches[slot].den = den;
  now = timer_ticks();
  default_draw_font_obj_block.gfx_scale_caches[slot].last_used = now;
  value = 0;
  if (num < den) {
    accum = 0;
    index = 0;
    while (index < 0x400) {
      for (; (-1 < accum && (index < 0x400)); index++) {
        default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
        accum -= den;
      }
      accum += num;
      value++;
    }
  }
  else {
    accum = num;
    index = 0;
    while (index < 0x400) {
      for (; (den <= accum && (index < 0x400)); index++) {
        default_draw_font_obj_block.gfx_scale_caches[slot].table[index] = value;
        accum -= den;
      }
      accum += num;
      value++;
    }
  }
  return;
}

/* 005cde2c auto */
/* Library Function - Single Match
    public: __thiscall CString::CString(void)
   
   Library: Visual Studio 1998 Debug */

Sprite * __thiscall sprite_init(Sprite *self)

{
  self->clear_fields();
  return self;
}

/* 005cde4d auto */
Sprite * __thiscall sprite_destroy(Sprite *self)

{
  uptr ptr;
  
  if (self->data_handle != 0) {
    if (self->data_ptr != 0) {
      ptr = mem_handle_unlock(self->data_handle);
      self->data_ptr = ptr;
    }
    self = (Sprite *)mem_handle_free(self->data_handle);
  }
  return self;
}

/* 005cdea1 auto */
i32 Sprite::create_solid(i32 width,i32 height,uint color)

{
  DibSurface surface;
  i32 result;
  
  surface_ctor(&surface);
  ON_SCOPE_EXIT(surface_dtor(&surface));
  surface.DibSurface::create(width,height);
  surface.fill(color & 0xff);
  encode_rect(&surface,0,0,width,height);
  SetRect((LPRECT)&bounds_rect,0,0,0,0);
  return result;
}

/* 005cdf50 auto */
void Sprite::free()

{
  uptr ptr;
  u32 handle;
  
  if (data_ptr != 0) {
    ptr = mem_handle_unlock(data_handle);
    data_ptr = ptr;
  }
  if (data_handle != 0) {
    handle = mem_handle_free(data_handle);
    data_handle = handle;
  }
  clear_fields();
  return;
}

/* 005cebb4 auto */
void Sprite::grab_surface_rect(DibSurface *surface,RECT *rect)

{
  u32 handle;
  
  handle = encode_from_surface(surface,0xffffffff,rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cebec auto */
void Sprite::encode_rect(DibSurface *surface,int x,int y,int width,int height)

{
  u32 handle;
  tagRECT rect;
  
  SetRect(&rect,x,y,x + width,height + y);
  handle = encode_from_surface(surface,0xffffffff,&rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cec44 auto */
void Sprite::grab_rect(DibSurface *surface,uint key_color,RECT *rect)

{
  u32 handle;
  
  handle = encode_from_surface(surface,key_color & 0xff,rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cedad auto */
void Sprite::grab_from_surface(DibSurface *surface,uint key_color,int x,int y,int width, int height)

{
  u32 handle;
  tagRECT rect;
  
  SetRect(&rect,x,y,width + x,height + y);
  handle = encode_from_surface(surface,key_color & 0xff,&rect);
  data_handle = handle;
  lock();
  return;
}

/* 005cef31 auto */
RECT *Sprite::draw(RECT *dest,DibSurface *surface,i32 x,i32 y)

{
  blit_to_surface((int *)dest,surface,0xffffffff,x,y);
  return dest;
}

/* 005cef66 auto */
RECT *Sprite::draw_keyed(RECT *dest,DibSurface *surface,uint key_color,i32 x, i32 y)

{
  blit_to_surface((int *)dest,surface,key_color & 0xff,x,y);
  return dest;
}

/* 005cf126 auto */
RECT *Sprite::draw_silhouette(RECT *dest,DibSurface *surface,i32 x,i32 y, i32 color)

{
  blit_silhouette_to_surface((int *)dest,surface,0xffffffff,x,y,color);
  return dest;
}

/* 005cf23f auto */
void Sprite::merge_onto(Sprite *dest)

{
  merge_at(this,dest,0,0,0);
  return;
}

/* 005cf2ff auto */
void Sprite::lock()

{
  uptr ptr;
  
  if (data_ptr == 0) {
    ptr = (uptr)mem_handle_lock(data_handle);
    data_ptr = ptr;
  }
  return;
}

/* 005cf337 auto */
void Sprite::unlock()

{
  uptr ptr;
  
  if (data_ptr != 0) {
    ptr = mem_handle_unlock(data_handle);
    data_ptr = ptr;
  }
  return;
}

/* 005cf36f auto */
bool Sprite::has_data()

{
  return data_ptr != 0;
}

/* 005cf39b auto */
void Sprite::set_origin(u32 x,u32 y)

{
  offset_x = x;
  offset_y = y;
  return;
}

/* 005cf3c5 auto */
void Sprite::get_origin(i32 *out_x,i32 *out_y)

{
  *(u32 *)out_x = offset_x;
  *(u32 *)out_y = offset_y;
  return;
}

/* 005cf467 auto */
void Sprite::replace_color(i32 old_color,i32 new_color)

{
  int rows_left;
  int *len_ptr;
  char had_data;
  char *cursor;
  int run_len;
  int rows;
  
  had_data = has_data();
  if (had_data == '\0') {
    lock();
    dbg_log((const char *)&str_warning_sprite_not_locked_in_changecolor);
  }
  rows = rect_height((RECT *)&bounds_rect);
  cursor = (char *)data_ptr;
  while (rows_left = rows - 1, rows != 0) {
    len_ptr = (int *)(cursor + 4);
    cursor += 8;
    run_len = *len_ptr;
    while (rows = rows_left, run_len != 0) {
      if (*cursor == (char)old_color) {
        *cursor = (char)new_color;
      }
      cursor++;
      run_len--;
    }
  }
  if (had_data == '\0') {
    unlock();
  }
  return;
}

/* 005cf64c auto */
int Sprite::encode_from_surface(DibSurface *surface,int key_mode,RECT *src_rect)

{
  char locked;
  int rect_h;
  int rect_w;
  u32 freed;
  tagRECT rect;
  u8 *scan;
  int height2;
  i32 *out;
  int all_empty;
  int row;
  u8 key2;
  int col;
  i32 *run_lens;
  int was_locked;
  int handle;
  u8 *top_row;
  u8 *bottom_row;
  iptr width2;
  i32 *run_starts;
  int min_left;
  int first_row2;
  int max_right;
  int last_row;
  int total_size;
  u8 **run_ptrs;
  
  was_locked = 1;
  all_empty = 1;
  SetRect(&rect,src_rect->left,src_rect->top,src_rect->right,
          src_rect->bottom);
  IntersectRect(&rect,&rect,(RECT *)&surface->clip_left);
  min_left = 10000;
  max_right = 0;
  total_size = 0;
  width2 = rect.right - rect.left;
  height2 = rect.bottom - rect.top;
  locked = surface->is_locked();
  if (locked == '\0') {
    dbg_log((const char *)&str_warning_port_not_locked_on_sprite_extrac);
    was_locked = 0;
    surface->lock();
  }
  top_row = (u8 *)surface->pixel_ptr(rect.left,rect.top);
  if (top_row == (u8 *)0x0) {
    dbg_logf_int2((const char *)&str_error_sprite_extract_illegal_pixel_mappi,rect.left,rect.top);
    if (was_locked == 0) {
      surface->unlock();
    }
    handle = 0;
  }
  else {
    if (key_mode == -1) {
      key2 = *top_row;
    }
    else if (key_mode != -2) {
      key2 = (u8)key_mode;
    }
    row = 0;
    while (row < height2) {
      scan = top_row;
      for (col = 0; col < width2; col++) {
        if ((*scan != key2) || (key_mode == -2)) {
          first_row2 = row;
          scan++;
          top_row = surface->prev_row(top_row);
          row = height2;
          all_empty = 0;
          break;
        }
        scan++;
      }
      row++;
      top_row = surface->next_row(top_row);
    }
    if (all_empty == 0) {
      bottom_row = (u8 *)surface->pixel_ptr(rect.left,
                                  rect.top + height2 - 1);
      row = 0;
      while (row < height2) {
        scan = bottom_row;
        for (col = 0; col < width2; col++) {
          if ((*scan != key2) || (key_mode == -2)) {
            last_row = height2 - row;
            row = height2;
            scan++;
            break;
          }
          scan++;
        }
        row++;
        bottom_row = surface->prev_row(bottom_row);
      }
      run_ptrs = (u8 **)operator_new(0xa00);
      run_starts = (i32 *)operator_new(0xa00);
      run_lens = (i32 *)operator_new(0xa00);
      if (((run_ptrs == (u8 **)0x0) || (run_starts == (i32 *)0x0)) || (run_lens == (i32 *)0x0)) {
        dbg_log((const char *)&str_error_unable_to_allocate_scan_line_table);
        handle = 0;
      }
      else {
        row = 0;
        while (row < last_row - first_row2) {
          total_size += 8;
          run_starts[row] = 0;
          run_lens[row] = 0;
          scan = top_row;
          for (col = 0; col < width2; col++) {
            if ((*scan != key2) || (key_mode == -2)) {
              run_ptrs[row] = scan;
              run_starts[row] = col;
              break;
            }
            scan++;
          }
          if ((width2 != col) && (run_starts[row] < min_left)) {
            min_left = run_starts[row];
          }
          scan = top_row + width2;
          for (col = 0; scan = scan - 1, col < width2; col++)
          {
            if ((*scan != key2) || (key_mode == -2)) {
              run_lens[row] =
                   (width2 - col) - run_starts[row];
              total_size += run_lens[row];
              break;
            }
          }
          if ((width2 != col) &&
             (max_right <
              run_lens[row] + run_starts[row])) {
            max_right = run_lens[row] +
                       run_starts[row];
          }
          row++;
          top_row = surface->next_row(top_row);
        }
        if (data_ptr != 0) {
          unlock();
        }
        if (data_handle != 0) {
          freed = mem_handle_free(data_handle);
          data_handle = freed;
        }
        total_size += 8;
        handle = mem_handle_alloc(total_size);
        if (handle == 0) {
          dbg_logf((const char *)&str_error_couldn_t_allocate_memory_for_sprit,total_size);
          if (was_locked == 0) {
            surface->unlock();
          }
          operator_delete(run_ptrs);
          operator_delete(run_starts);
          operator_delete(run_lens);
        }
        else {
          SetRect((LPRECT)&bounds_rect,min_left,first_row2,max_right,last_row);
          rect_h = rect_height(&rect);
          rect_w = rect_width(&rect);
          SetRect((LPRECT)this,0,0,rect_w,rect_h);
          if (key_mode == -2) {
            key_color = '\0';
          }
          else {
            key_color = key2;
          }
          offset_x = 0;
          offset_y = 0;
          out = (i32 *)mem_handle_lock(handle);
          for (row = 0; row < last_row - first_row2; row++) {
            *out = run_starts[row];
            out[1] = run_lens[row];
            out += 2;
            mem_move_src_dst(run_ptrs[row],out,
                         run_lens[row]);
            out = (i32 *)((int)out + run_lens[row]);
          }
          *out = 0;
          out = (i32 *)mem_handle_unlock(handle);
          if (was_locked == 0) {
            surface->unlock();
          }
          operator_delete(run_ptrs);
          operator_delete(run_starts);
          operator_delete(run_lens);
        }
      }
    }
    else {
      SetRect((LPRECT)&bounds_rect,0,0,0,0);
      rect_h = rect_height(&rect);
      rect_w = rect_width(&rect);
      SetRect((LPRECT)this,0,0,rect_w,rect_h);
      if (key_mode == -2) {
        key_color = '\0';
      }
      else {
        key_color = key2;
      }
      offset_x = 0;
      offset_y = 0;
      if (was_locked == 0) {
        surface->unlock();
      }
      data_ptr = 0;
      handle = 0;
    }
  }
  return handle;
}

/* 005d056c auto */
int *Sprite::blit_to_surface(int *out_rect,DibSurface *surface,int key_arg,int x, int y)

{
  int trim_y;
  int tmp;
  int scaled_trim_x;
  int scaled_trim_y;
  i32 clip_w;
  int scaled_h;
  BOOL intersects;
  i32 surf_height;
  GdiDib *dib;
  int top_down;
  int pitch;
  i32 bits;
  iptr scale_tbl;
  i32 clip_h;
  int skip_x;
  int dst_y;
  int dst_x;
  i32 key2;
  tagRECT clip_rect;
  int skip_y;
  int trim_x;
  
  if (key_arg == -1) {
    (*((char *)&key_arg + 0)) = key_color;
  }
  key2 = (key2 & 0xffffff00 | ((u8)key_arg));
  trim_x = bounds_rect - frame_rect;
  trim_y = bounds_top - frame_top;
  tmp = width();
  if (((tmp * gfx_scale.num) / gfx_scale.den < 0x401) &&
     (tmp = height(), (tmp * gfx_scale.num) / gfx_scale.den < 0x401)) {
    scaled_trim_x = gfx_scale_length(trim_x,0,1);
    scaled_trim_y = gfx_scale_length(trim_y,0,1);
    tmp = gfx_scale_length(offset_x,0,0);
    dst_x = scaled_trim_x + tmp + x;
    tmp = gfx_scale_length(offset_y,0,0);
    dst_y = scaled_trim_y + tmp + y;
    clip_h = 0;
    tmp = scaled_trim_y;
    clip_w = get_height();
    scaled_h = gfx_scale_length(clip_w,tmp,clip_h);
    clip_h = 0;
    tmp = scaled_trim_x;
    clip_w = get_width();
    tmp = gfx_scale_length(clip_w,tmp,clip_h);
    SetRect(&clip_rect,0,0,tmp,scaled_h);
    OffsetRect(&clip_rect,dst_x,dst_y);
    intersects = IntersectRect(&clip_rect,&clip_rect,(RECT *)&surface->clip_left);
    if (intersects != 0) {
      skip_x = 0;
      clip_w = rect_width(&clip_rect);
      if (dst_x < clip_rect.left) {
        skip_x = clip_rect.left - dst_x;
        dst_x = clip_rect.left;
      }
      skip_y = 0;
      clip_h = rect_height(&clip_rect);
      if (dst_y < clip_rect.top) {
        skip_y = clip_rect.top - dst_y;
        dst_y = clip_rect.top;
      }
      tmp = skip_y;
      scaled_h = trim_x;
      surf_height = surface->surface_height();
      dib = surface->get_dib();
      top_down = dib_is_top_down(dib);
      pitch = surface->get_pitch();
      pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
      scale_tbl = (iptr)gfx_scale_tables.table;
      bits = surface->get_bits();
      sprite_blit_rows_keyed((u8 *)data_ptr,bits,(u8 *)scale_tbl,key2,dst_x,dst_y,pitch,surf_height,clip_w,clip_h
                   ,skip_x,tmp,scaled_h,trim_y,scaled_trim_x,scaled_trim_y);
    }
    *out_rect = clip_rect.left;
    out_rect[1] = clip_rect.top;
    out_rect[2] = clip_rect.right;
    out_rect[3] = clip_rect.bottom;
    return out_rect;
  }
  dbg_log((const char *)&str_error_sprite_size_exceeds_scale_table_si);
  SetRect(&clip_rect,0,0,0,0);
  *out_rect = clip_rect.left;
  out_rect[1] = clip_rect.top;
  out_rect[2] = clip_rect.right;
  out_rect[3] = clip_rect.bottom;
  return out_rect;
}

/* 005d10cd auto */
int *Sprite::blit_silhouette_to_surface(int *out_rect,DibSurface *surface,int key_arg,int x, int y,i32 color)

{
  int trim_y;
  int tmp;
  int scaled_trim_x;
  int scaled_trim_y;
  i32 clip_w;
  int scaled_h;
  BOOL intersects;
  i32 surf_height;
  GdiDib *dib;
  int top_down;
  int pitch;
  i32 bits;
  iptr scale_tbl;
  i32 clip_h;
  int skip_x;
  int dst_y;
  int dst_x;
  i32 key2;
  tagRECT clip_rect;
  int skip_y;
  int trim_x;
  
  if (key_arg == -1) {
    (*((char *)&key_arg + 0)) = key_color;
  }
  key2 = (key2 & 0xffffff00 | ((u8)key_arg));
  trim_x = bounds_rect - frame_rect;
  trim_y = bounds_top - frame_top;
  tmp = width();
  if (((tmp * gfx_scale.num) / gfx_scale.den < 0x401) &&
     (tmp = height(), (tmp * gfx_scale.num) / gfx_scale.den < 0x401)) {
    scaled_trim_x = gfx_scale_length(trim_x,0,1);
    scaled_trim_y = gfx_scale_length(trim_y,0,1);
    tmp = gfx_scale_length(offset_x,0,0);
    dst_x = x + tmp + scaled_trim_x;
    tmp = gfx_scale_length(offset_y,0,0);
    dst_y = y + tmp + scaled_trim_y;
    clip_h = 0;
    tmp = scaled_trim_y;
    clip_w = get_height();
    scaled_h = gfx_scale_length(clip_w,tmp,clip_h);
    clip_h = 0;
    tmp = scaled_trim_x;
    clip_w = get_width();
    tmp = gfx_scale_length(clip_w,tmp,clip_h);
    SetRect(&clip_rect,0,0,tmp,scaled_h);
    OffsetRect(&clip_rect,dst_x,dst_y);
    intersects = IntersectRect(&clip_rect,&clip_rect,(RECT *)&surface->clip_left);
    if (intersects != 0) {
      skip_x = 0;
      clip_w = rect_width(&clip_rect);
      if (dst_x < clip_rect.left) {
        skip_x = clip_rect.left - dst_x;
        dst_x = clip_rect.left;
      }
      skip_y = 0;
      clip_h = rect_height(&clip_rect);
      if (dst_y < clip_rect.top) {
        skip_y = clip_rect.top - dst_y;
        dst_y = clip_rect.top;
      }
      tmp = skip_y;
      scaled_h = trim_x;
      surf_height = surface->surface_height();
      dib = surface->get_dib();
      top_down = dib_is_top_down(dib);
      pitch = surface->get_pitch();
      pitch = ((-(uint)(top_down == 0) & 0xfffffffe) + 1) * pitch;
      scale_tbl = (iptr)gfx_scale_tables.table;
      bits = surface->get_bits();
      sprite_blit_rows_solid((u8 *)data_ptr,bits,(u8 *)scale_tbl,key2,dst_x,dst_y,pitch,surf_height,clip_w,clip_h
                   ,skip_x,tmp,scaled_h,trim_y,scaled_trim_x,scaled_trim_y,color);
    }
    *out_rect = clip_rect.left;
    out_rect[1] = clip_rect.top;
    out_rect[2] = clip_rect.right;
    out_rect[3] = clip_rect.bottom;
    return out_rect;
  }
  dbg_log((const char *)&str_error_sprite_size_exceeds_scale_table_si_2);
  SetRect(&clip_rect,0,0,0,0);
  *out_rect = clip_rect.left;
  out_rect[1] = clip_rect.top;
  out_rect[2] = clip_rect.right;
  out_rect[3] = clip_rect.bottom;
  return out_rect;
}

/* 005d1612 auto */
void Sprite::merge_at(Sprite *src,Sprite *dst,i32 unused,int dx, int dy)

{
  byte row_mask;
  LONG width;
  LONG height;
  int src_top;
  iptr src_w;
  int src_h;
  int merged;
  int dst_h;
  iptr new_h;
  int value;
  u32 new_handle;
  int dst_end;
  int src_start;
  int src_end;
  u32 freed;
  int written;
  int *dst_cursor;
  int *src_cursor;
  int row;
  int col;
  int cols_left;
  int *out;
  tagRECT rect;
  
  if (src->data_handle != 0) {
    if (dst->data_handle == 0) {
      SetRect((LPRECT)dst,(int)src->frame_rect,(int)src->frame_top,*(int *)src->frame_right + dx,
              *(int *)src->frame_bottom + dy);
      dst->bounds_rect = src->bounds_rect;
      dst->bounds_top = src->bounds_top;
      *(i32 *)dst->bounds_right = *(i32 *)src->bounds_right;
      *(i32 *)dst->bounds_bottom = *(i32 *)src->bounds_bottom;
      OffsetRect((LPRECT)&dst->bounds_rect,dx,dy);
      dst->offset_x = (int)src->offset_x + dx;
      dst->offset_y = (int)src->offset_y + dy;
      dst->load_scale_num = src->load_scale_num;
      dst->load_scale_den = src->load_scale_den;
      dst->key_color = src->key_color;
      dst->data_handle = src->data_handle;
      mem_handle_clone(&dst->data_handle);
      dst->lock();
    }
    else {
      src_top = ((int)src->bounds_top + dy) - dst->bounds_top;
      src_w = rect_width((RECT *)&src->bounds_rect);
      value = (int)src->bounds_rect;
      src_h = rect_height((RECT *)&src->bounds_rect);
      merged = rect_width((RECT *)&dst->bounds_rect);
      merged = dst->bounds_rect + merged;
      dst_h = rect_height((RECT *)&dst->bounds_rect);
      new_h = src_h + src_top;
      if (src_h + src_top <= dst_h) {
        new_h = dst_h;
      }
      value = value + src_w + dx;
      if (value <= merged) {
        value = merged;
      }
      SetRect(&rect,0,0,value,new_h);
      height = rect.bottom;
      width = rect.right;
      src->lock();
      dst->lock();
      src_cursor = (int *)src->data_ptr;
      dst_cursor = (int *)dst->data_ptr;
      new_handle = mem_handle_alloc(width * height + height * 8);
      out = (int *)mem_handle_lock(new_handle);
      for (row = 0; row < height; row++) {
        row_mask = row < dst_h;
        if ((src_top <= row) && (row < src_h + src_top)) {
          row_mask = row_mask | 2;
        }
        switch(row_mask) {
        case 0:
          *out = 0;
          out[1] = 0;
          out += 2;
          break;
        case 1:
          *out = *dst_cursor;
          out[1] = dst_cursor[1];
          value = out[1];
          mem_move_src_dst((dst_cursor + 2),(out + 2),value);
          dst_cursor = (int *)((int)(dst_cursor + 2) + value);
          out = (int *)((int)(out + 2) + value);
          break;
        case 2:
          *out = *src_cursor + dx;
          out[1] = src_cursor[1];
          value = out[1];
          mem_move_src_dst((src_cursor + 2),(out + 2),value);
          src_cursor = (int *)((int)(src_cursor + 2) + value);
          out = (int *)((int)(out + 2) + value);
          break;
        case 3:
          value = *dst_cursor;
          dst_end = dst_cursor[1] + value;
          dst_cursor += 2;
          src_start = *src_cursor + dx;
          src_end = src_cursor[1] + src_start;
          src_cursor += 2;
          merged = value;
          if (src_start <= value) {
            merged = src_start;
          }
          *out = merged;
          merged = dst_end;
          if (dst_end <= src_end) {
            merged = src_end;
          }
          out[1] = merged - *out;
          out += 2;
          cols_left = width;
          col = 0;
          while (cols_left != 0) {
            written = 0;
            if ((value <= col) && (col < dst_end)) {
              *(char *)out = (char)*dst_cursor;
              dst_cursor = (int *)((int)dst_cursor + 1);
              written = 1;
            }
            if ((src_start <= col) && (col < src_end)) {
              if ((uint)src->key_color == (int)(char)*src_cursor) {
                if (written == 0) {
                  *(u8 *)out = dst->key_color;
                }
              }
              else {
                *(char *)out = (char)*src_cursor;
              }
              src_cursor = (int *)((int)src_cursor + 1);
              written++;
            }
            if (written != 0) {
              out = (int *)((int)out + 1);
            }
            col++;
            cols_left--;
          }
        }
      }
      OffsetRect(&rect,dst->bounds_rect,dst->bounds_top);
      dst->bounds_rect = rect.left;
      dst->bounds_top = rect.top;
      *(LONG *)dst->bounds_right = rect.right;
      *(LONG *)dst->bounds_bottom = rect.bottom;
      dst->frame_rect = rect.left;
      dst->frame_top = rect.top;
      *(LONG *)dst->frame_right = rect.right;
      *(LONG *)dst->frame_bottom = rect.bottom;
      if (dst->data_ptr != 0) {
        dst->unlock();
      }
      if (dst->data_handle != 0) {
        freed = mem_handle_free(dst->data_handle);
        dst->data_handle = freed;
      }
      mem_handle_unlock(new_handle);
      dst->data_handle = new_handle;
      dst->lock();
    }
  }
  return;
}

/* 005d1b38 auto */
void Sprite::clear_fields()

{
  SetRect((LPRECT)this,0,0,0,0);
  SetRect((LPRECT)&bounds_rect,0,0,0,0);
  offset_x = 0;
  offset_y = 0;
  load_scale_num = 1;
  load_scale_den = 1;
  key_color = '\0';
  data_handle = 0;
  data_ptr = 0;
  return;
}

/* 005d1d00 auto */
int __cdecl gfx_scale_length(int length,int start,int round_up)

{
  int result;
  bool positive;
  int index;
  
  if (length == 0) {
    result = 0;
  }
  else {
    positive = -1 < length;
    if (!positive) {
      length = -length;
    }
    if (gfx_scale.num < gfx_scale.den) {
      index = start;
      result = gfx_scale_tables.table[start] + length;
      for (; gfx_scale_tables.table[index] < result; index++) {
      }
      if ((round_up == 0) && (gfx_scale_tables.table[index] != result)) {
        index--;
      }
    }
    else {
      index = start;
      for (; gfx_scale_tables.table[index] <
             gfx_scale_tables.table[start] + length; index++) {
      }
    }
    result = ((-(uint)positive & 2) - 1) * (index - start);
  }
  return result;
}

/* 005de780 auto */
void __cdecl logpal_init_from_system(GameLogPalette *logpal)

{
  HDC hdc;
  uint raster_caps;
  int num_reserved;
  i8 *sys_color;
  int i;
  
  logpal->version = 0x300;
  logpal->count = 0x100;
  hdc = GetDC((HWND)0x0);
  raster_caps = GetDeviceCaps(hdc,0x26);
  if ((raster_caps & 0x100) == 0) {
    err_report_ctx_block.palette_static_half_count = 0;
    display_palette.palettized = 0;
  }
  else {
    num_reserved = GetDeviceCaps(hdc,0x18);
    err_report_ctx_block.palette_static_half_count = num_reserved / 2;
    GetSystemPaletteEntries(hdc,0,0x100,(LPPALETTEENTRY)logpal->entries);
  }
  for (i = 0; i < err_report_ctx_block.palette_static_half_count; i++) {
    logpal->entries[i].flags = 0;
  }
  for (; i < 0x100 - err_report_ctx_block.palette_static_half_count; i++) {
    logpal->entries[i].red = 0;
    logpal->entries[i].green = 0;
    logpal->entries[i].blue = 0;
    logpal->entries[i].flags = 4;
  }
  for (; i < 0x100; i++) {
    logpal->entries[i].flags = 0;
  }
  if (display_palette.palettized == 0) {
    sys_color = ((i8 *)&logpal_static_system_colors);
    for (i = 0; i < 10; i++) {
      logpal->entries[i].red = *sys_color;
      logpal->entries[i].green = sys_color[1];
      logpal->entries[i].blue = sys_color[2];
      sys_color += 3;
    }
    for (i = 0xf6; i < 0x100; i++) {
      logpal->entries[i].red = *sys_color;
      logpal->entries[i].green = sys_color[1];
      logpal->entries[i].blue = sys_color[2];
      sys_color += 3;
    }
  }
  ReleaseDC((HWND)0x0,hdc);
  return;
}

/* 005de984 auto */
void __cdecl logpal_reserve_range(GameLogPalette *logpal,int first,int count)

{
  int i;
  
  if (0x100 < count + first) {
    count = 0x100 - first;
  }
  for (i = first; i < count + first; i++) {
    logpal->entries[i].flags = 1;
  }
  return;
}

/* 005dea62 auto */
void __cdecl palette_animate(GameLogPalette *logpal,HPALETTE hpal,UINT first,UINT count)

{
  if (display_palette.palettized == 1) {
    AnimatePalette(hpal,first,count,(PALETTEENTRY *)&logpal->entries[first]);
  }
  return;
}

/* 005dea9e auto */
void __cdecl palette_get_entry_rgb(GameLogPalette *logpal,int index,i8 *out_red,i8 *out_green,i8 *out_blue)

{
  *out_red = logpal->entries[index].red;
  *out_green = logpal->entries[index].green;
  *out_blue = logpal->entries[index].blue;
  return;
}

/* 005deadb auto */
void __cdecl
logpal_set_entry(GameLogPalette *logpal,int index,i32 red,i32 green,i32 blue)

{
  logpal->entries[index].red = (i8)red;
  logpal->entries[index].green = (i8)green;
  logpal->entries[index].blue = (i8)blue;
  return;
}

/* 005deb12 auto */
void __cdecl
palette_set_entry_rgb(GameLogPalette *logpal,int index,i32 red,i32 green,i32 blue)

{
  int i;
  
  logpal->entries[index].red = (char)red;
  logpal->entries[index].green = (char)green;
  logpal->entries[index].blue = (char)blue;
  logpal->entries[index].flags = 4;
  for (i = 0; i < err_report_ctx_block.palette_static_half_count; i++) {
    if (((logpal->entries[i].red == (char)red) &&
        (logpal->entries[i].green == (char)green)) &&
       (logpal->entries[i].blue == (char)blue)) {
      logpal->entries[index].flags = 1;
    }
  }
  for (i = 0x100 - err_report_ctx_block.palette_static_half_count; i < 0x100; i++) {
    if (((logpal->entries[i].red == (char)red) &&
        (logpal->entries[i].green == (char)green)) &&
       (logpal->entries[i].blue == (char)blue)) {
      logpal->entries[index].flags = 1;
    }
  }
  return;
}

/* 005dec4e auto */
HPALETTE __cdecl logpal_create_hpalette(GameLogPalette *logpal)

{
  i32 hpal;
  
  if (display_palette.palettized == 1) {
    hpal = CreatePalette((LOGPALETTE *)logpal);
  }
  else {
    hpal = (HPALETTE)0x0;
  }
  return hpal;
}

/* 005dec8a auto */
void __cdecl logpal_delete_hpalette(HGDIOBJ hpal)

{
  if (display_palette.palettized == 1) {
    DeleteObject(hpal);
  }
  return;
}

/* 005decb1 auto */
void __cdecl logpal_apply_range(GameLogPalette *logpal,HPALETTE hpal,UINT first,UINT count)

{
  if (display_palette.palettized == 1) {
    SetPaletteEntries(hpal,first,count,(PALETTEENTRY *)&logpal->entries[first]);
  }
  return;
}

/* 005ded12 auto */
void __cdecl palette_copy_rgb_range(GameLogPalette *logpal,u8 *dest,int first,int count)

{
  int index;
  iptr offset;
  
  offset = 0;
  for (index = first; index < count + first; index++) {
    *(i8 *)(offset + dest) = logpal->entries[index].red;
    *(i8 *)(offset + 1 + dest) = logpal->entries[index].green;
    *(i8 *)(offset + 2 + dest) = logpal->entries[index].blue;
    offset += 3;
  }
  return;
}

/* 005e35b0 auto */
GdiDib * __cdecl dib_create(RECT *rect)

{
  int width;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  struct { BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[256]; } bmi;  /* 8-bit, 256 colours */
  int height2;
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height2 = rect->bottom - rect->top;
  if ((width < 1) || (height2 < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf);
    }
    else {
      _memset(&bmi,0,0x28);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 8;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      if (0 < bmi.bmiHeader.biHeight) {
        if (dib_rows.bottom_up_logged == 0) {
          dbg_log((const char *)&str_bottom_up_orientation_recommended);
        }
        dib_rows.bottom_up_logged = 1;
      }
      if (dib_rows.order == -1) {
        bmi.bmiHeader.biHeight = height2;
      }
      else if (dib_rows.order == 1) {
        bmi.bmiHeader.biHeight = -height2;
      }
      else {
        bmi.bmiHeader.biHeight = bmi.bmiHeader.biHeight * height2;
      }
      _memset(bmi.bmiColors,0,0x400);
      h = CreateDIBSection(hdc,(BITMAPINFO *)&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth;
        dib2->width = width;
        dib2->height = height2;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e388f auto */
i32 __cdecl dib_destroy(GdiDib *dib)

{
  i32 handle;
  
  if (dib != 0) {
    if (dib->hdc != 0) {
      SelectObject(dib->hdc,dib->old_bitmap);
      DeleteObject(dib->bitmap);
      DeleteDC(dib->hdc);
    }
    if (dib->palette != 0) {
      DeleteObject(dib->palette);
    }
    handle = dib->mem_handle;
    mem_handle_unlock(handle);
    mem_handle_free(handle);
  }
  return 0;
}

/* 005e392a auto */
i32 __cdecl dib_get_stride(GdiDib *dib)

{
  i32 stride;
  
  if (dib == 0) {
    stride = 0;
  }
  else {
    stride = dib->stride;
  }
  return stride;
}

/* 005e395a auto */
uint __cdecl dib_is_top_down(GdiDib *dib)

{
  return (uint)(dib->top_down == 1);
}

/* 005e3988 auto */
void __cdecl dib_flip_vertical(GdiDib *dib)

{
  i32 tmp_handle;
  byte *tmp_row;
  int row;
  byte *bottom_row;
  byte *top_row;
  
  top_row = (byte *)dib_get_bits(dib);
  tmp_handle = mem_handle_alloc(dib->stride);
  tmp_row = (byte *)mem_handle_lock(tmp_handle);
  bottom_row = (dib->height - 1) * dib->stride + top_row;
  for (row = 0; row < dib->height / 2; row++) {
    crt_memmove(tmp_row,bottom_row,dib->stride);
    crt_memmove(bottom_row,top_row,dib->stride);
    crt_memmove(top_row,tmp_row,dib->stride);
    top_row += dib->stride;
    bottom_row -= dib->stride;
  }
  mem_handle_unlock(tmp_handle);
  mem_handle_free(tmp_handle);
  return;
}

/* 005e3a81 auto */
i32 __cdecl dib_get_bits(GdiDib *dib)

{
  i32 bits;
  
  if (dib == 0) {
    bits = 0;
  }
  else {
    bits = dib->bits;
  }
  return bits;
}

/* 005e3aa8 auto */
i32 __cdecl dib_unlock_bits(GdiDib *dib)

{
  return 0;
}

/* 005e3b4c auto */
void __cdecl dib_set_color_table_range(GdiDib *dib,GamePalette *palette,UINT first,UINT count)

{
  RGBQUAD colors[256];
  uptr index;
  
  for (index = first; (int)index < (int)(count + first); index++) {
    palette->get_entry(index,(i8 *)&colors[index].rgbRed,(i8 *)&colors[index].rgbGreen,
               ((i8 *)(colors + index)));
    colors[index].rgbReserved = '\0';
  }
  SetDIBColorTable(dib->hdc,first,count,colors);
  return;
}

/* 005e3bdc auto */
void __cdecl dib_set_palette(GdiDib *dib,GamePalette *palette)

{
  dib_set_color_table_range(dib,palette,0,0x100);
  return;
}

/* 005e3cb4 auto */
void __cdecl
dib_draw_text_at(GdiDib *dib,int font,char *text,int x,int y,RECT *clip_rect,
            uint align)

{
  int half_h;
  int half_w;
  size_t cchText;
  tagRECT *lprc;
  UINT format;
  RGBQUAD color2;
  HGDIOBJ old_font;
  UINT old_align;
  tagRECT text_rect;
  i32 *font_data;
  
  if (font == 0) {
    dbg_log((const char *)&str_error_null_font_in_msdrawstring);
  }
  else {
    font_data = (i32 *)mem_handle_lock(font);
    half_h = font_measure_height(font);
    half_h = y + half_h;
    half_w = font_measure_width(font,(const char *)text);
    SetRect(&text_rect,x,y,x + half_w,half_h);
    old_font = SelectObject(dib->hdc,(HGDIOBJ)*font_data);
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    SetTextColor(dib->hdc,
                 RGB(color2.rgbRed,color2.rgbGreen,color2.rgbBlue));
    half_h = rect_height(&text_rect);
    half_h = half_h / 2;
    half_w = rect_width(&text_rect);
    half_w = half_w / 2;
    OffsetRect(&text_rect,-half_w,-half_h);
    if ((align & 1) != 0) {
      OffsetRect(&text_rect,half_w,0);
    }
    if ((align & 4) != 0) {
      OffsetRect(&text_rect,0,half_h);
    }
    if ((align & 8) != 0) {
      OffsetRect(&text_rect,0,-half_h);
    }
    if ((align & 2) != 0) {
      OffsetRect(&text_rect,-half_w,0);
    }
    old_align = GetTextAlign(dib->hdc);
    SetTextAlign(dib->hdc,1);
    MoveToEx(dib->hdc,text_rect.left,text_rect.top,(LPPOINT)0x0);
    IntersectRect(&text_rect,&text_rect,clip_rect);
    format = 0;
    lprc = &text_rect;
    cchText = _strlen(text);
    DrawTextA(dib->hdc,text,cchText,lprc,format);
    SetTextAlign(dib->hdc,old_align);
    SelectObject(dib->hdc,old_font);
    mem_handle_unlock(font);
  }
  return;
}

/* 005e3eca auto */
void __cdecl dib_draw_text_rect(GdiDib *dib,int font,char *text,RECT *rect,uint align)

{
  size_t cchText;
  uint format;
  RGBQUAD color2;
  HGDIOBJ old_font;
  i32 *font_data;
  
  if (font == 0) {
    dbg_log((const char *)&str_error_null_font_in_msdrawstring_2);
  }
  else if (dib != 0) {
    font_data = (i32 *)mem_handle_lock(font);
    old_font = SelectObject(dib->hdc,(HGDIOBJ)*font_data);
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    SetTextColor(dib->hdc,
                 RGB(color2.rgbRed,color2.rgbGreen,color2.rgbBlue));
    format = 0;
    if (align == 0) {
      format = 0x25;
    }
    else {
      if ((align & 8) != 0) {
        format = 8;
      }
      if ((align & 2) != 0) {
        format = format | 2;
      }
    }
    cchText = _strlen(text);
    DrawTextA(dib->hdc,text,cchText,rect,format);
    SelectObject(dib->hdc,old_font);
    mem_handle_unlock(font);
  }
  return;
}

/* 005e3feb auto */
void __cdecl dib_draw_text_wrap(GdiDib *dib,int font,char *text,LPRECT rect,uint align)

{
  size_t cchText;
  UINT format;
  RGBQUAD color2;
  HGDIOBJ old_font;
  i32 *font_data;
  
  if (font == 0) {
    dbg_log((const char *)&str_error_null_font_in_msdrawtext);
  }
  else if (dib != 0) {
    font_data = (i32 *)mem_handle_lock(font);
    old_font = SelectObject(dib->hdc,(HGDIOBJ)*font_data);
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    SetTextColor(dib->hdc,
                 RGB(color2.rgbRed,color2.rgbGreen,color2.rgbBlue));
    format = 0x10;
    if (align == 0) {
      format = 0x11;
    }
    else if ((align & 2) != 0) {
      format = 0x12;
    }
    cchText = _strlen(text);
    DrawTextA(dib->hdc,text,cchText,rect,format);
    SelectObject(dib->hdc,old_font);
    mem_handle_unlock(font);
  }
  return;
}

/* 005e40fb auto */
void __cdecl dib_draw_line(GdiDib *dib,int x1,int y1,int x2,int y2)

{
  RGBQUAD color2;
  HPEN pen;
  HGDIOBJ old_pen;
  
  if (dib != 0) {
    GetDIBColorTable(dib->hdc,(uint)ctrl_colors.gfx_text,1,&color2);
    pen = CreatePen(0,1,RGB(color2.rgbRed,color2.rgbGreen,color2.rgbBlue));
    old_pen = SelectObject(dib->hdc,pen);
    MoveToEx(dib->hdc,x1,y1,(LPPOINT)0x0);
    LineTo(dib->hdc,x2,y2);
    SelectObject(dib->hdc,old_pen);
    DeleteObject(pen);
  }
  return;
}

/* 005e41ba auto */
GdiDib * __cdecl dib_create16(RECT *rect)

{
  int bmp_height;
  int width;
  int height;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  struct { BITMAPINFOHEADER bmiHeader; DWORD masks[3]; } bmi;  /* 16-bit BI_BITFIELDS */
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height = rect->bottom - rect->top;
  if ((width < 1) || (height < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf_2);
    }
    else {
      _memset(&bmi,0,0x34);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 0x10;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biCompression = 3;
      bmi.masks[0] = 0x7c00;
      bmi.masks[1] = 0x3e0;
      bmi.masks[2] = 0x1f;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      bmp_height = height;
      if (dib_rows.order != -1) {
        if (dib_rows.order == 1) {
          bmp_height = -height;
        }
        else {
          bmp_height = bmi.bmiHeader.biHeight * height;
        }
      }
      bmi.bmiHeader.biHeight = bmp_height;
      h = CreateDIBSection(hdc,(BITMAPINFO *)&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg_2);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth * 2;
        dib2->width = width;
        dib2->height = height;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e43c5 auto */
GdiDib * __cdecl dib_create24(RECT *rect)

{
  int bmp_height;
  int width;
  int height;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  BITMAPINFO bmi;
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height = rect->bottom - rect->top;
  if ((width < 1) || (height < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf_3);
    }
    else {
      _memset(&bmi,0,0x2c);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 0x18;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      bmp_height = height;
      if (dib_rows.order != -1) {
        if (dib_rows.order == 1) {
          bmp_height = -height;
        }
        else {
          bmp_height = bmi.bmiHeader.biHeight * height;
        }
      }
      bmi.bmiHeader.biHeight = bmp_height;
      h = CreateDIBSection(hdc,&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg_3);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth * 3;
        dib2->width = width;
        dib2->height = height;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e45b5 auto */
GdiDib * __cdecl dib_create32(RECT *rect)

{
  int bmp_height;
  int width;
  int height;
  HDC hdc;
  HBITMAP h;
  HGLOBAL hMem;
  HGDIOBJ old_bitmap;
  GdiDib *dib2;
  BITMAPINFO bmi;
  void *bits2;
  
  dib2 = (GdiDib *)0x0;
  width = rect->right - rect->left;
  height = rect->bottom - rect->top;
  if ((width < 1) || (height < 1)) {
    dib2 = (GdiDib *)0x0;
  }
  else {
    hdc = CreateCompatibleDC((HDC)0x0);
    if (hdc == (HDC)0x0) {
      dbg_log((const char *)&str_error_unable_to_create_dc_in_msinitgraf_4);
    }
    else {
      _memset(&bmi,0,0x2c);
      bmi.bmiHeader.biSize = 0x28;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 0x20;
      bmi.bmiHeader.biHeight = 1;
      bmi.bmiHeader.biWidth = dib_row_align4(width);
      bmp_height = height;
      if (dib_rows.order != -1) {
        if (dib_rows.order == 1) {
          bmp_height = -height;
        }
        else {
          bmp_height = bmi.bmiHeader.biHeight * height;
        }
      }
      bmi.bmiHeader.biHeight = bmp_height;
      h = CreateDIBSection(hdc,&bmi,0,&bits2,(HANDLE)0x0,0);
      if (h == (HBITMAP)0x0) {
        dbg_log((const char *)&str_error_unable_to_create_bitmap_in_msinitg_4);
        DeleteDC(hdc);
      }
      else {
        hMem = mem_handle_alloc(0x28);
        dib2 = (GdiDib *)GlobalLock(hMem);
        dib2->mem_handle = hMem;
        dib2->hdc = hdc;
        dib2->palette = 0;
        dib2->bitmap = h;
        dib2->stride = bmi.bmiHeader.biWidth << 2;
        dib2->width = width;
        dib2->height = height;
        old_bitmap = SelectObject(hdc,h);
        dib2->old_bitmap = old_bitmap;
        dib2->bits = (iptr)bits2;
        if (bmi.bmiHeader.biHeight < 0) {
          dib2->top_down = 1;
        }
        else {
          dib2->top_down = 0;
        }
        SetBkMode(hdc,1);
      }
    }
  }
  return dib2;
}

/* 005e4d60 auto */
i32 __cdecl
gif_lzw_decode(byte *src,uint code_base,i32 min_code_size,uint pitch,int width,uint height,
            i8 *dest)

{
  int table_handle;
  i32 result;
  iptr table;
  i16 code;
  
  table_handle = mem_handle_alloc(0x6000);
  if (table_handle == 0) {
    dbg_log((const char *)&str_error_cannot_allocate_decompression_tabl);
    result = 0;
  }
  else {
    table = (iptr)mem_handle_lock(table_handle);
    for (code = 0; code < 0x1000; code++) {
      *(i16 *)(table + code * 2) = 0xffff;
      *(ushort *)(table + 0x2000 + code * 2) = (ushort)(byte)code_base + code;
      *(i16 *)(table + 0x4000 + code * 2) = 1;
    }
    gif_lzw_decode_stream(src,dest,(u8 *)table,width,height,pitch,min_code_size);
    mem_handle_unlock(table_handle);
    mem_handle_free(table_handle);
    result = 1;
  }
  return result;
}

/* 005e4e60 auto */
void __cdecl
dib_fill_rect8(u8 *bits,uint color,int x,int y,uint width,int height,int pitch,
            int surf_height)

{
  uint color16;
  uint dwords;
  int abs_pitch;
  uint *dst;
  
  abs_pitch = pitch;
  if (pitch < 1) {
    y = (surf_height - 1) - y;
    abs_pitch = -pitch;
  }
  dst = (uint *)(bits + y * abs_pitch + x);
  color16 = color | color << 8;
  do {
    dwords = width >> 2;
    if (dwords != 0) {
      for (; dwords != 0; dwords--) {
        *dst = color16 | color << 0x10 | color << 0x18;
        dst++;
      }
    }
    if ((width & 2) != 0) {
      *(short *)dst = (short)color16;
      dst = (uint *)((int)dst + 2);
    }
    if ((width & 1) != 0) {
      *(char *)dst = (char)color;
      dst = (uint *)((int)dst + 1);
    }
    dst = (uint *)((int)dst + (pitch - width));
    height--;
  } while (height != 0);
  return;
}

/* 005e4f9b auto */
void __cdecl
gfx_copy_pixels(int src_bits,int dst_bits,int src_x,int src_y,int dst_x,int dst_y,uint width,
            int height,int src_height,int dst_height,int src_pitch,int dst_pitch)

{
  i8 pixel;
  uint count;
  int abs_pitch;
  i32 *src;
  i32 *dst;
  int rows_left;
  
  abs_pitch = src_pitch;
  if (src_pitch < 1) {
    src_y = (src_height - 1) - src_y;
    abs_pitch = -src_pitch;
  }
  src = (i32 *)(src_bits + src_y * abs_pitch + src_x);
  abs_pitch = dst_pitch;
  if (dst_pitch < 1) {
    dst_y = (dst_height - 1) - dst_y;
    abs_pitch = -dst_pitch;
  }
  dst = (i32 *)(dst_bits + dst_y * abs_pitch + dst_x);
  rows_left = height;
  do {
    count = width >> 2;
    if (count != 0) {
      for (; count != 0; count--) {
        *dst = *src;
        src++;
        dst++;
      }
    }
    for (count = width & 3; count != 0; count--) {
      pixel = *(i8 *)src;
      src = (i32 *)((int)src + 1);
      *(i8 *)dst = pixel;
      dst = (i32 *)((int)dst + 1);
    }
    src = (i32 *)((int)src + (src_pitch - width));
    dst = (i32 *)((int)dst + (dst_pitch - width));
    rows_left--;
  } while (rows_left != 0);
  return;
}

/* 005e518e auto */
/* WARNING: Unable to use type for symbol run_start */

void __cdecl
sprite_blit_rows_keyed(u8 *sprite_data,int dst_bits,u8 *scale_tbl,i32 key,int dst_x,int dst_y,
            int pitch,int surf_height,int width,int height,int skip_x,int skip_y,
            i32 trim_x,int src_row,int scaled_trim_x,int scaled_trim_y)

{
  int *col_map;
  int cols_left;
  int run_len;
  iptr offset;
  iptr row_data;
  char *dst_row;
  char *dst;
  int run_start;
  int *row_map;
  
  run_len = pitch;
  if (pitch < 1) {
    dst_y = (surf_height - 1) - dst_y;
    run_len = -pitch;
  }
  dst_row = (char *)(dst_bits + dst_y * run_len + dst_x);
  row_map = (int *)((skip_y + scaled_trim_y) * 4 + scale_tbl);
  row_data = (iptr)(sprite_data + 8);
  do {
    for (; src_row < *row_map; src_row++) {
      row_data = row_data + *(int *)(row_data - 4) + 8;
    }
    run_start = *(int *)(row_data - 8);
    run_len = *(int *)(row_data - 4);
    if (run_len != 0) {
      cols_left = width;
      dst = dst_row;
      for (col_map = (int *)((skip_x + scaled_trim_x) * 4 + scale_tbl); *col_map < run_start;
          col_map = col_map + 1) {
        dst++;
        cols_left--;
      }
      if (0 < cols_left) {
        do {
          offset = *col_map - run_start;
          if (run_len <= offset) break;
          if (*(char *)(offset + row_data) != (char)key) {
            *dst = *(char *)(offset + row_data);
          }
          col_map++;
          dst++;
          cols_left--;
        } while (cols_left != 0);
      }
    }
    dst_row += pitch;
    row_map++;
    height--;
    if (height < 1) {
      return;
    }
  } while( true );
}

/* 005e52bf auto */
/* WARNING: Unable to use type for symbol run_start */

void __cdecl
sprite_blit_rows_solid(u8 *sprite_data,int dst_bits,u8 *scale_tbl,i32 key,int dst_x,int dst_y,
            int pitch,int surf_height,int width,int height,int skip_x,int skip_y,
            i32 trim_x,int src_row,int scaled_trim_x,int scaled_trim_y,i32 color)

{
  int *col_map;
  int cols_left;
  int run_len;
  iptr offset;
  iptr row_data;
  i8 *dst_row;
  i8 *dst;
  int run_start;
  int *row_map;
  
  run_len = pitch;
  if (pitch < 1) {
    dst_y = (surf_height - 1) - dst_y;
    run_len = -pitch;
  }
  dst_row = (i8 *)(dst_bits + dst_y * run_len + dst_x);
  row_map = (int *)((skip_y + scaled_trim_y) * 4 + scale_tbl);
  row_data = (iptr)(sprite_data + 8);
  do {
    for (; src_row < *row_map; src_row++) {
      row_data = row_data + *(int *)(row_data - 4) + 8;
    }
    run_start = *(int *)(row_data - 8);
    run_len = *(int *)(row_data - 4);
    if (run_len != 0) {
      cols_left = width;
      dst = dst_row;
      for (col_map = (int *)((skip_x + scaled_trim_x) * 4 + scale_tbl); *col_map < run_start;
          col_map = col_map + 1) {
        dst++;
        cols_left--;
      }
      if (0 < cols_left) {
        do {
          offset = *col_map - run_start;
          if (run_len <= offset) break;
          if (*(char *)(offset + row_data) != (char)key) {
            *dst = (i8)color;
          }
          col_map++;
          dst++;
          cols_left--;
        } while (cols_left != 0);
      }
    }
    dst_row += pitch;
    row_map++;
    height--;
    if (height < 1) {
      return;
    }
  } while( true );
}

/* 005e53f3 auto */
void __cdecl
gif_lzw_decode_stream(byte *src,i8 *dest,u8 *prefix_tbl,int pitch,uint width,uint height,
            i32 min_code_size)

{
  byte *next_src;
  ushort prefix;
  ushort clear_code;
  ushort init_bits;
  ushort init_mask;
  ushort code;
  i8 *dst_end;
  uptr n;
  iptr len;
  uptr slot;
  iptr suffix_tbl;
  iptr length_tbl;
  i8 *from;
  i8 *row_next;
  i8 *to;
  byte *src_ptr;
  i8 *out_ptr;
  ushort next_code;
  ushort code_bits;
  ushort cur_code;
  uint bit_buf;
  ushort row_pos;
  i8 *dst;
  ushort code_mask;
  ushort bit_count;
  ushort prev_code;
  byte *block_end;
  
  suffix_tbl = (iptr)(prefix_tbl + 0x2000);
  length_tbl = (iptr)(prefix_tbl + 0x4000);
  row_pos = 0;
  src_ptr = src;
  block_end = src;
  dst = dest;
  dst_end = dest + (width & 0xffff) * (height & 0xffff) + 1;
  clear_code = 2 << ((byte)min_code_size & 0x1f);
  next_code = clear_code + 4;
  init_bits = (short)min_code_size + 1;
  init_mask = (2 << ((byte)init_bits & 0x1f)) - 2;
  bit_buf = 0;
  bit_count = 0;
  code_bits = init_bits;
  code_mask = init_mask;
  prev_code = clear_code;
  do {
    while( true ) {
      if (((short)pitch <= (short)row_pos) &&
         (row_pos = row_pos - (short)pitch, (ushort)((short)height - (short)pitch) != 0)) {
        row_next = dst + (ushort)((short)height - (short)pitch);
        to = row_next;
        from = dst;
        for (n = (uptr)row_pos; dst = row_next, n != 0; n--) {
          from--;
          to--;
          *to = *from;
        }
      }
      next_src = src_ptr;
      for (; src_ptr = next_src, bit_count < code_bits; bit_count += 8) {
        if (block_end == next_src) {
          src_ptr = next_src + 1;
          block_end = next_src + *next_src + 1;
        }
        next_src = src_ptr + 1;
        bit_buf = (uint)*src_ptr << ((byte)bit_count & 0x1f) | bit_buf;
      }
      code = (short)bit_buf << 1 & code_mask;
      if (next_code < code) {
        return;
      }
      bit_buf = bit_buf >> ((byte)code_bits & 0x1f);
      bit_count -= code_bits;
      if (code == (ushort)(clear_code + 2)) {
        return;
      }
      if (code != clear_code) break;
      next_code = clear_code + 4;
      code_bits = init_bits;
      code_mask = init_mask;
      prev_code = clear_code;
    }
    cur_code = code;
    if (prev_code != clear_code) {
      slot = (uptr)next_code;
      *(ushort *)(prefix_tbl + slot) = prev_code;
      *(i16 *)(length_tbl + slot) = *(i16 *)(length_tbl + (uint)prev_code);
      *(short *)(length_tbl + slot) = *(short *)(length_tbl + slot) + 1;
      if (code < next_code) {
        dst += *(ushort *)(length_tbl + (uint)code);
        row_pos += *(ushort *)(length_tbl + (uint)code);
        out_ptr = dst;
        if ((int)dst_end <= (int)dst) {
          return;
        }
        do {
          to = out_ptr - 1;
          *to = *(i8 *)(suffix_tbl + (uint)cur_code);
          prefix = *(ushort *)(prefix_tbl + (uint)cur_code);
          if (prefix == 0xffff) break;
          to = out_ptr - 2;
          *to = *(i8 *)(suffix_tbl + (uint)prefix);
          cur_code = *(ushort *)(prefix_tbl + (uint)prefix);
          out_ptr = to;
        } while (cur_code != 0xffff);
      }
      else {
        len = *(ushort *)(length_tbl + (uint)prev_code) + 1;
        dst += len;
        out_ptr = dst - 1;
        row_pos += (short)len;
        if ((int)dst_end <= (int)dst) {
          return;
        }
        do {
          to = out_ptr - 1;
          *to = *(i8 *)(suffix_tbl + (uint)prev_code);
          prefix = *(ushort *)(prefix_tbl + (uint)prev_code);
          if (prefix == 0xffff) break;
          to = out_ptr - 2;
          *to = *(i8 *)(suffix_tbl + (uint)prefix);
          prev_code = *(ushort *)(prefix_tbl + (uint)prefix);
          out_ptr = to;
        } while (prev_code != 0xffff);
        dst[-1] = *to;
      }
      *(i8 *)(suffix_tbl + (uint)next_code) = *to;
      next_code += 2;
      prev_code = code;
      if ((code_mask < next_code) && (code_bits != 0xc)) {
        code_bits++;
        code_mask = code_mask * 2 + 2;
      }
      continue;
    }
    dst += *(ushort *)(length_tbl + (uint)code);
    row_pos += *(ushort *)(length_tbl + (uint)code);
    out_ptr = dst;
    if ((int)dst_end <= (int)dst) {
      return;
    }
    do {
      out_ptr[-1] = *(i8 *)(suffix_tbl + (uint)cur_code);
      prefix = *(ushort *)(prefix_tbl + (uint)cur_code);
      prev_code = code;
      if (prefix == 0xffff) break;
      out_ptr[-2] = *(i8 *)(suffix_tbl + (uint)prefix);
      cur_code = *(ushort *)(prefix_tbl + (uint)prefix);
      out_ptr -= 2;
    } while (cur_code != 0xffff);
  } while( true );
}

/* 005e5869 auto */
void __cdecl
dib_hline8(u8 *bits,i32 color,uint length,int x,int y,int surf_height,
            int pitch)

{
  ushort dwords;
  int count;
  i32 *dst;
  
  if (pitch < 1) {
    pitch = -pitch;
    y = (surf_height - 1) - y;
  }
  dst = (i32 *)(bits + y * pitch + x);
  dwords = (ushort)length >> 2;
  count = length & 0xffff0000 | dwords;   /* the original's count: length's high half kept */
  if (dwords != 0) {
    for (; count != 0; count--) {
      *dst = (u8)color * 0x01010101;
      dst++;
    }
  }
  if ((length & 2) != 0) {
    *(ushort *)dst = (ushort)((u8)color * 0x0101);
    dst = (i32 *)((int)dst + 2);
  }
  if ((length & 1) != 0) {
    *(i8 *)dst = (i8)color;
  }
  return;
}

/* 005e58e7 auto */
void __cdecl
dib_vline8(u8 *bits,i32 color,int length,int x,int y,int surf_height,
            int pitch)

{
  iptr abs_pitch;
  i8 *dst;
  
  abs_pitch = pitch;
  if (pitch < 1) {
    abs_pitch = -pitch;
    y = (surf_height - 1) - y;
  }
  dst = (i8 *)(bits + y * abs_pitch + x);
  do {
    *dst = (i8)color;
    dst += pitch;
    length--;
  } while (length != 0);
  return;
}

/* 005e5ee0 auto */
void DDSurface::destruct()

{
  return;
}

/* 005e89d0: written by hand in src/game/fixups.cpp */

/* 005ea578: written by hand in src/game/regargs.cpp */

/* 005ea677: written by hand in src/game/regargs.cpp */

/* 005ed710 auto */
void __cdecl
gdi_blit_transparent(HDC dst_dc,HGDIOBJ bitmap,int dst_x,int dst_y,int width,int height,int src_x
            ,int src_y)

{
  HDC hdc;
  HGDIOBJ h;
  HDC hdc_00;
  HDC hdc_01;
  HBITMAP h_00;
  HGDIOBJ h_01;
  HBITMAP h_02;
  HGDIOBJ h_03;
  
  hdc = CreateCompatibleDC(dst_dc);
  h = SelectObject(hdc,bitmap);
  hdc_00 = CreateCompatibleDC(dst_dc);
  hdc_01 = CreateCompatibleDC(dst_dc);
  h_00 = CreateCompatibleBitmap(dst_dc,width,height);
  h_01 = SelectObject(hdc_01,h_00);
  h_02 = CreateBitmap(width,height,1,1,(void *)0x0);
  h_03 = SelectObject(hdc_00,h_02);
  SetBkColor(hdc_01,0xffffff);
  SetTextColor(hdc_01,0);
  SetBkColor(hdc,0x808000);
  BitBlt(hdc_00,0,0,width,height,hdc,src_x,src_y,0xcc0020);
  BitBlt(hdc_01,0,0,width,height,dst_dc,dst_x,dst_y,0xcc0020);
  BitBlt(hdc_01,0,0,width,height,hdc,src_x,src_y,0x660046);
  BitBlt(hdc_01,0,0,width,height,hdc_00,0,0,0x8800c6);
  BitBlt(hdc_01,0,0,width,height,hdc,src_x,src_y,0x660046);
  BitBlt(dst_dc,dst_x,dst_y,width,height,hdc_01,0,0,0xcc0020);
  SelectObject(hdc_00,h_03);
  DeleteObject(h_02);
  SelectObject(hdc_01,h_01);
  DeleteObject(h_00);
  DeleteDC(hdc_00);
  DeleteDC(hdc_01);
  SelectObject(hdc,h);
  DeleteDC(hdc);
  return;
}
