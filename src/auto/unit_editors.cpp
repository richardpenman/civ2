/* The game, units: the cheat menu's unit editor and the sound editor: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0058a5b0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl unit_sound_editor_global_init(void)

{
  unit_sound_editor_static_init();
  unit_sound_editor_reg_atexit();
  return;
}

/* 0058a5ca auto */
void __cdecl unit_sound_editor_static_init(void)

{
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::construct();
  return;
}

/* 0058a5e4 auto */
void __cdecl unit_sound_editor_reg_atexit(void)

{
  _atexit(unit_sound_editor_static_dtor);
  return;
}

/* 0058a601 auto */
void __cdecl unit_sound_editor_static_dtor(void)

{
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::destruct();
  return;
}

/* 0058a61b auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
unit_sound_editor_create_wnd(const char *title,uint style,int x,int y,int width,int height,int resize_border,
            int caption_height,int border_width)

{
  iptr parent2;
  uint wnd_style;
  
  if (title == 0) {
    crt_strcpy(((i8 *)&unit_sound_editor.title),((i8 *)&unit_sound_editor_empty_title));
  }
  else {
    crt_strcpy(((i8 *)&unit_sound_editor.title),title);
  }
  unit_sound_editor.text_font = (iptr)&ui_text_font;
  unit_sound_editor.title_font = (iptr)&caption_font_block.caption_font;
  unit_sound_editor.title_small_font = (iptr)&caption_small_font_block.caption_small_font;
  unit_sound_editor.style_flags = style;
  unit_sound_editor.caption_height = caption_height;
  unit_sound_editor.border_width = border_width;
  unit_sound_editor.resize_border = resize_border;
  unit_sound_editor.caption_button_count = 0;
  unit_sound_editor.caption_button_cb = 0;
  if ((style & 4) != 0) {
    unit_sound_editor.caption_height = ui_metrics.caption_height;
    unit_sound_editor.border_width = ui_metrics.border_width;
  }
  if ((style & 8) == 0) {
    wnd_style = 0x202;
  }
  else {
    wnd_style = 0x802;
  }
  if (unit_sound_editor.caption_height != 0) {
    wnd_style = wnd_style | 0x400;
  }
  if (resize_border != 0) {
    wnd_style = wnd_style | 0x1000;
  }
  if ((style & 2) == 0) {
    width += unit_sound_editor.border_width * 2;
    height = height + unit_sound_editor.caption_height + unit_sound_editor.border_width;
  }
  if ((style & 1) != 0) {
    x = (caption_font_block.screen_width >> 1) - (width >> 1);
    y = (caption_font_block.screen_height >> 1) - (height >> 1);
  }
  if (editor_active.wnd == 0) {
    parent2 = 0;
  }
  else {
    parent2 = (iptr)&editor_active.wnd->window;
  }
  ((DibSurface *)((i8 *)&unit_sound_editor.window))->wnd_create_with_surface(0,wnd_style,x,y,width,height,(GamePalette *)&game_palette_block.game_palette,
             (WndBase *)parent2);
  if (unit_sound_editor.caption_height != 0) {
    ((WndBase *)((i8 *)&unit_sound_editor.wnd))->set_caption_height(unit_sound_editor.caption_height);
  }
  if (unit_sound_editor.resize_border != 0) {
    ((WndBase *)((i8 *)&unit_sound_editor.wnd))->set_resize_border(unit_sound_editor.resize_border);
  }
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->layout();
  return;
}

/* 0058a905 auto */
/* WARNING: Removing unreachable block (ram,0x0058a97f) */
/* WARNING: Removing unreachable block (ram,0x0058a9fb) */

void __cdecl unit_sound_editor_pick_wav(int slot)

{
  i32 filter_arg;
  iptr title;
  int ret;
  DWORD attrs;
  i8 *file_buf;
  char *filter;
  i32 dlg_arg;
  i8 *default_ext;
  i32 fmt_arg2;
  i32 fmt_arg3;
  i8 wav_path[256];
  char dest_path[0x140];  /* (port) 256 in the original: scenario_dir_path (264),
                             SOUND\ and the name (BF-296) */
  Dialog dlg;
  int choice;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if (sprite_picker_sprite_block.unit_sound_effects_by_ctrl[slot] >= 0) {
    dlg_set_parent_window((iptr)&unit_sound_editor.wnd);
    do {
      fmt_arg3 = 0;
      fmt_arg2 = 0;
      dlg_arg = 0;
      filter_arg = (iptr)label_string(label_table.string_ids[0x222]);
      _sprintf(dest_path,str_s_wav_c_wav_c_c,filter_arg,dlg_arg,fmt_arg2,fmt_arg3);
      crt_strcpy(wav_path,str_wav);
      dlg_arg = 0;
      filter_arg = 1;
      default_ext = ((i8 *)&unit_sound_wav_default_ext);
      filter = dest_path;
      file_buf = wav_path;
      title = (iptr)label_string(label_table.string_ids[0x223]);
      ret = ui_file_name_dialog(&unit_sound_editor.wnd,(const char *)title,file_buf,filter,default_ext,filter_arg,dlg_arg);
      if (ret == 0) {
        return;
      }
      dlg.load_text((const char *)&str_debug_10,(const char *)&str_playsound,0x801);
      do {
        dlg.result_extra = 0;
        choice = dlg.show_modal(0);
        if (dlg.result_extra != 0) {
          snd_wave_play((const char *)wav_path,0,0,0);
        }
      } while (dlg.result_extra != 0);
      dlg.close();
      dlg.init_defaults();
    } while (choice != 0);
    crt_strcpy(dest_path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(dest_path,str_sound_5);
    attrs = GetFileAttributesA(dest_path);
    if (attrs != 0xffffffff) {
      crt_strcat(dest_path,((i8 *)&unit_sound_path_sep_str));
      crt_strcat(dest_path,((char *)&str_aircombt) + sprite_picker_sprite_block.unit_sound_effects_by_ctrl[slot] * 9);
      crt_strcat(dest_path,((i8 *)&unit_sound_wav_suffix_str));
      ret = file_exists((const char *)dest_path);
      if ((ret == 0) || (ret = crt_remove(dest_path), ret == 0)) {
        file_copy(wav_path,dest_path);
      }
    }
  }
  return;
}

/* 0058abee auto */
void __cdecl unit_sound_editor_ok_clicked(void)

{
  main_frame_bg_surface_block.unit_sound_editor_running = 0;
  ((ModalMenuWindow *)((i8 *)&unit_sound_editor.wnd))->end_modal_loop();
  return;
}

/* 0058ac13 auto */
void __cdecl unit_sound_editor_cancel_clicked(void)

{
  main_frame_bg_surface_block.unit_sound_editor_running = 0;
  ((ModalMenuWindow *)((i8 *)&unit_sound_editor.wnd))->end_modal_loop();
  return;
}

/* 0058ac38 auto */
void __cdecl unit_sound_editor_redraw(void)

{
  int row_height;
  int width_adj;
  int col_width;
  int top;
  int left;
  int slot;
  i8 rect[16];
  
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::draw_caption();
  gfx_fill_rect(&unit_sound_editor.window,(RECT *)&unit_sound_editor.fill_rect,0x1d);
  text_set_surface(&unit_sound_editor.window);
  text_set_font(&editor_font_block.editor_font);
  text_set_colors(10,10,0,0);
  row_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  row_height += 8;
  width_adj = unit_sound_editor.client_width + (unit_sound_editor.client_width >> 0x1f & 3U);
  col_width = width_adj >> 2;
  for (slot = 0; slot < 6; slot++) {
    top = (row_height * 3) / 2 + (slot / 3) * row_height * 4 + unit_sound_editor.client_y;
    left = (slot % 3) * col_width * 5;
    left = ((int)(left + (left >> 0x1f & 3U)) >> 2) + ((int)(col_width + (width_adj >> 0x1f & 3U)) >> 2)
            + unit_sound_editor.client_x;
    rect_set_xywh((RECT *)rect,left,top,col_width,row_height);
    gfx_draw_bevel_rect(&unit_sound_editor.window,(RECT *)rect,10,10);
    if (-1 < main_frame_bg_surface_block.unit_sound_effects[slot]) {
      text_clear();
      text_append_str((const char *)(((char *)&str_aircombt) + main_frame_bg_surface_block.unit_sound_effects[slot] * 9));
      text_append_str((const char *)&str_wav_2);
      text_draw_centered(&unit_sound_editor.window,textbuf.buf,left + col_width / 2,top + 4,0);
    }
  }
  text_clear();
  text_append_label(0x25b);
  text_draw_centered(&unit_sound_editor.window,textbuf.buf,unit_sound_editor.client_x + unit_sound_editor.client_width / 2,unit_sound_editor.client_y + 10,0);
  ((DibSurface *)((i8 *)&unit_sound_editor.window))->wnd_blit_surface();
  return;
}

/* 0058ae6c auto */
void __cdecl unit_sound_editor_on_dblclick(int x,int y)

{
  int row_height;
  int width_adj;
  int col_width;
  int hit;
  int slot;
  RECT rect2;
  u32 saved_options;
  
  y -= unit_sound_editor.client_y;
  x -= unit_sound_editor.client_x;
  row_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  row_height += 8;
  width_adj = unit_sound_editor.client_width + (unit_sound_editor.client_width >> 0x1f & 3U);
  col_width = width_adj >> 2;
  slot = 0;
  while( true ) {
    if (5 < slot) {
      return;
    }
    hit = (slot % 3) * col_width * 5;
    rect_set_xywh(&rect2,
                       ((int)(hit + (hit >> 0x1f & 3U)) >> 2) +
                       ((int)(col_width + (width_adj >> 0x1f & 3U)) >> 2),
                       (row_height * 3) / 2 + (slot / 3) * row_height * 4,col_width,row_height);
    if ((-1 < main_frame_bg_surface_block.unit_sound_effects[slot]) &&
       (hit = point_in_rect_ltrb(x,y,rect2.left,rect2.top,rect2.right,rect2.bottom), hit != 0))
    break;
    slot++;
  }
  saved_options = game.game_options;
  game.game_options = game.game_options | OPT_SOUND;
  snd_play_effect(main_frame_bg_surface_block.unit_sound_effects[slot],0,0,0);
  game.game_options = saved_options;
  return;
}

/* 0058afb6 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl unit_sound_editor_set_slots(int unit_type,Button *buttons)

{
  int orders_effect;
  int i;
  
  orders_effect = -1;
  for (i = 0; i < 6; i++) {
    main_frame_bg_surface_block.unit_sound_effects[i] = 0xffffffff;
  }
  if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8] != 0) {
    if (unit_type == UNIT_TEST_1) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x7d;
    }
    else if (unit_type == UNIT_TEST_2) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x7e;
    }
    else if (unit_type == UNIT_TEST_3) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x7f;
    }
    else if (unit_type == UNIT_TEST_4) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x80;
    }
    else if (unit_type == UNIT_TEST_5) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x81;
    }
    else if (unit_type == UNIT_TEST_6) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x82;
    }
    else if (unit_type == UNIT_TEST_7) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x83;
    }
    else if (unit_type == UNIT_TEST_8) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x84;
    }
    else if (unit_type == UNIT_EXTRA_LAND) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x65;
    }
    else if (unit_type == UNIT_EXTRA_SHIP) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x66;
    }
    else if (unit_type == UNIT_EXTRA_AIR) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x67;
    }
    else if (((((i8 *)&editor_text_dlg_wnd_block.editor_item_flags_byte1))[unit_type * 0x58] & 0x10) == 0) {
      if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3] == 1) {
        if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[7] == 0) {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x21;
          if (unit_type < 0x1e) {
            main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x17;
          }
          else {
            main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x4f;
          }
        }
        else if (unit_type < 0x1e) {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0;
          main_frame_bg_surface_block.unit_sound_bomb_effect_i32 = 0x18;
          main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x17;
          main_frame_bg_surface_block.unit_sound_sputter_effect_i32 = 0x1a;
        }
        else {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x52;
          main_frame_bg_surface_block.unit_sound_bomb_effect_i32 = 0x50;
          main_frame_bg_surface_block.unit_sound_crash_effect_i32 = 0x4f;
          main_frame_bg_surface_block.unit_sound_sputter_effect_i32 = 0x4e;
        }
      }
      else if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3] == 2) {
        if (((i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[12] & 8) == 0) {
          orders_effect = 6;
          if (((unit_type == UNIT_BATTLESHIP) || (unit_type == UNIT_CRUISER)) || ((unit_type == UNIT_AEGIS_CRUISER || (unit_type == UNIT_DESTROYER))))
          {
            orders_effect = 0x2e;
          }
        }
        else {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x4d;
        }
      }
      else if (unit_type == UNIT_ELEPHANT) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x19;
      }
      else if ((((unit_type == UNIT_HORSEMEN) || (unit_type == UNIT_CHARIOT)) || (unit_type == UNIT_KNIGHTS)) || (unit_type == UNIT_CRUSADERS))
      {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x4a;
      }
      else if ((unit_type == UNIT_DRAGOONS) || (unit_type == UNIT_CAVALRY)) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0xc;
      }
      else if (((unit_type == UNIT_MUSKETEERS) || (unit_type == UNIT_RIFLEMEN)) || ((unit_type == UNIT_ALPINE_TROOPS || (unit_type == UNIT_PARTISANS)))) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x22;
      }
      else if ((((unit_type == UNIT_FANATICS) || (unit_type == UNIT_PARATROOPERS)) || (unit_type == UNIT_MARINES)) || (unit_type == UNIT_MECH_INF)) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x26;
      }
      else if ((unit_type < 0x16) || (0x1a < unit_type)) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x49;
      }
      else if (unit_type == UNIT_CATAPULT) {
        main_frame_bg_surface_block.unit_sound_slot_effects[0] = 10;
      }
      else {
        orders_effect = 0x28;
        if (0x17 < unit_type) {
          main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x1c;
        }
      }
    }
    else if (editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8] < 99) {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x29;
    }
    else {
      main_frame_bg_surface_block.unit_sound_slot_effects[0] = 0x32;
    }
  }
  main_frame_bg_surface_block.unit_sound_orders_effect = orders_effect;
  if (-1 < orders_effect) {
    main_frame_bg_surface_block.unit_sound_explosion_effect_i32 = 0x23;
  }
  for (i = 0; i < 6; i++) {
    if (main_frame_bg_surface_block.unit_sound_effects[i] < 0) {
      buttons[i].disable();
    }
    else {
      buttons[i].enable();
    }
  }
  return;
}

/* 0058b47e auto */
/* WARNING: Removing unreachable block (ram,0x0058b6f2) */
/* WARNING: Removing unreachable block (ram,0x0058b77b) */
/* WARNING: Removing unreachable block (ram,0x0058b5ea) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl unit_sound_editor_run(const char *title,i32 unit_type)

{
  int col_offset;
  iptr label;
  int i;
  Button cancel_btn;
  int btn_y;
  Button slot_btns[6];
  int btn_height;
  int btn_x;
  int btn_width;
  i8 rect[16];
  Button ok_btn;
  
  button_ctor(&ok_btn);
  ON_SCOPE_EXIT(ctrl_dtor(&ok_btn));
  button_ctor(&cancel_btn);
  ON_SCOPE_EXIT(ctrl_dtor(&cancel_btn));
  crt_vector_ctor_iterator(slot_btns,0x3c,6,button_ctor,ctrl_dtor);
  ON_SCOPE_EXIT(crt_vector_dtor_iterator(&slot_btns,0x3c,6,ctrl_dtor));
  main_frame_bg_surface_block.unit_sound_editor_running = 1;
  unit_sound_editor_create_wnd(title,0xd,0,0,0x21c,0x118,0,0,0);
  btn_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  btn_height += 8;
  btn_width = (int)(unit_sound_editor.client_width + (unit_sound_editor.client_width >> 0x1f & 3U)) >> 2;
  for (i = 0; i < 6; i++) {
    btn_y = btn_height * 3 + (i / 3) * btn_height * 4 + unit_sound_editor.client_y;
    col_offset = (i % 3) * btn_width * 5;
    btn_x = ((int)(col_offset + (col_offset >> 0x1f & 3U)) >> 2) +
               ((int)(btn_width + (btn_width >> 0x1f & 3U)) >> 2) + unit_sound_editor.client_x;
    rect_set_xywh((RECT *)rect,btn_x,btn_y,btn_width,btn_height);
    label = (iptr)label_string(label_table.string_ids[unit_sound_button_label_ids[i]]);
    (slot_btns + i)->create((Wnd *)&unit_sound_editor.wnd,i + 0xc9,(RECT *)rect,(const char *)label);
    (slot_btns + i)->set_callback(unit_sound_editor_pick_wav);
  }
  unit_sound_editor_set_slots(unit_type,slot_btns);
  btn_width = (unit_sound_editor.client_width - 6) / 2;
  btn_y = (unit_sound_editor.client_y + unit_sound_editor.client_height) - (btn_height + 2);
  btn_x = unit_sound_editor.client_x + 2;
  rect_set_xywh((RECT *)rect,btn_x,btn_y,btn_width,btn_height);
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_btn.create((Wnd *)&unit_sound_editor.wnd,0x65,(RECT *)rect,(const char *)label);
  ok_btn.set_callback((CtrlNotifyFn)unit_sound_editor_ok_clicked);
  btn_x = btn_x + btn_width + 2;
  rect_set_xywh((RECT *)rect,btn_x,btn_y,btn_width,btn_height);
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_btn.create((Wnd *)&unit_sound_editor.wnd,0x66,(RECT *)rect,(const char *)label);
  cancel_btn.set_callback((CtrlNotifyFn)unit_sound_editor_cancel_clicked);
  cancel_btn.set_cancel_button();
  ((WndHandlers *)((i8 *)&unit_sound_editor.wnd_handlers))->set_dblclick_handler(unit_sound_editor_on_dblclick);
  ((SurfaceWindow *)((i8 *)&unit_sound_editor.window))->set_redraw_callback(unit_sound_editor_redraw);
  ((SurfaceWindow *)((i8 *)&unit_sound_editor.window))->repaint();
  ((SurfaceWindow *)((i8 *)&unit_sound_editor.window))->SurfaceWindow::show();
  ((ModalMenuWindow *)((i8 *)&unit_sound_editor.wnd))->run_modal_loop();
  while (main_frame_bg_surface_block.unit_sound_editor_running != 0) {
    win_pump_one_message();
  }
  dlg_set_parent_window(0);
  ((FrameWindow *)((i8 *)&unit_sound_editor.window))->FrameWindow::close();
  return;
}

/* 005aef20 auto */
void __cdecl unit_editor_load_rules(void)

{
  char *_Source;
  size_t _Count;
  iptr unit_type;
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    _Count = 0x28;
    _Source = label_string(unit_types[unit_type].name);
    _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[unit_type],_Source,_Count);
    editor_text_dlg_wnd_block.editor_item_name_recs[unit_type][0x27] = 0;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[0] = (int)unit_types[unit_type].prerequisite;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[1] = (int)unit_types[unit_type].obsolete_by;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[2] = (int)(char)unit_types[unit_type].role;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3] = (int)(char)unit_types[unit_type].domain;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[4] = (int)(char)unit_types[unit_type].cost;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[5] = (int)(char)unit_types[unit_type].hold;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[6] =
         (int)(char)unit_types[unit_type].move / (int)(uint)cosmic.road_move_mult;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[7] = (int)(char)unit_types[unit_type].range;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8] = (int)(char)unit_types[unit_type].attack;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[9] = (int)(char)unit_types[unit_type].defense;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[10] = (int)(char)unit_types[unit_type].hit_points / 10;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[11] = (int)(char)unit_types[unit_type].firepower;
    editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[12] = unit_types[unit_type].flags;
  }
  return;
}

/* 005af140 auto */
void __cdecl unit_editor_store_rules(void)

{
  char *_Dest;
  char *_Source;
  size_t _Count;
  iptr unit_type;
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    _Count = 0xf;
    _Source = editor_text_dlg_wnd_block.editor_item_name_recs[unit_type];
    _Dest = label_string(unit_types[unit_type].name);
    editor_copy_pool_name(_Dest,_Source,_Count);  /* B32 (docs/BUGS.md) */
    unit_types[unit_type].prerequisite = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[0];
    unit_types[unit_type].obsolete_by = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[1];
    unit_types[unit_type].role = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[2];
    unit_types[unit_type].domain = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[3];
    unit_types[unit_type].cost = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[4];
    unit_types[unit_type].hold = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[5];
    unit_types[unit_type].move =
         (char)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[6] * cosmic.road_move_mult;
    unit_types[unit_type].range = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[7];
    unit_types[unit_type].attack = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[8];
    unit_types[unit_type].defense = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[9];
    unit_types[unit_type].hit_points = (char)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[10] * '\n';
    unit_types[unit_type].firepower = (i8)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[11];
    unit_types[unit_type].flags = (u32)editor_text_dlg_wnd_block.editor_item_recs[unit_type].values[12];
  }
  return;
}

/* 005af343 auto */
void __cdecl unit_editor_load_controls(void)

{
  int ctrl_id;
  int i;
  char num_buf[8];
  iptr sel;
  
  for (i = 1; i < 0xd; i++) {
    if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 9) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      _sprintf(num_buf,((i8 *)&unit_editor_ctrl_kinds_block.unit_editor_int_fmt),
               *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58));
      ((Control *)
                 &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->set_text(
                 (const char *)num_buf);
    }
    else if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 0xc) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      sel = *(iptr *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58);
      if ((i == 1) || (i == 2)) {
        sel += 2;
      }
      editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot].set_cur_sel(sel);
    }
  }
  return;
}

/* 005af4ae auto */
int __cdecl unit_editor_store_controls(void)

{
  int field;
  i32 clamped;
  int i;
  int changed_count;
  char num_buf[8];
  int value;
  
  changed_count = 0;
  for (i = 1; i < 0xd; i++) {
    if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 9) {
      field = ((Control *)
                         &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      field -= 0xca;
      ((Control *)
                 &editor_active.wnd->edits[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->Control::get_text(
                 (char *)num_buf);
      value = _atoi(num_buf);
      clamped = int_clamp(value,*(i32 *)(((i8 *)&unit_editor_ctrl_kinds_block.unit_editor_field_min_table) + field * 4),
                                 *(i32 *)(((i8 *)&unit_editor_ctrl_kinds_block.unit_editor_field_max_table) + field * 4));
      editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] = clamped;
      if (editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] != value) {
        changed_count++;
      }
    }
    else if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 0xc) {
      value = editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot].get_cur_sel();
      if ((i == 1) || (i == 2)) {
        value -= 2;
      }
      field = ((Control *)
                         &editor_active.wnd->combos[unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot])->get_id();
      *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_active.wnd->item * 0x58 + field * 4) = value;
    }
  }
  return changed_count;
}

/* 005af682 auto */
void __cdecl unit_editor_redraw(void)

{
  editor_active.wnd->unit_editor_paint();
  return;
}

/* 005af69d auto */
i32 __cdecl unit_editor_write_units(FILE *file)

{
  char *type_name;
  size_t name_len;
  size_t pad_len;
  iptr unit_type;
  int bit;
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    text_clear();
    text_append_string_id(unit_types[unit_type].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_name_sep));
    type_name = label_string(unit_types[unit_type].name);
    name_len = _strlen(type_name);
    if (name_len < 0xd) {
      type_name = label_string(unit_types[unit_type].name);
      pad_len = _strlen(type_name);
    }
    else {
      pad_len = 0xd;
    }
    text_append_spaces(0xd - pad_len);
    text_append_advance_abbrev(unit_types[unit_type].obsolete_by,1);
    text_append_int_2d((int)(char)unit_types[unit_type].domain);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_domain_sep));
    text_append_int_2d((int)(char)unit_types[unit_type].move / (int)(uint)cosmic.road_move_mult);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_move_suffix));
    text_append_int((int)(char)unit_types[unit_type].range);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_range_sep));
    text_append_int_2d((int)(char)unit_types[unit_type].attack);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_attack_suffix));
    text_append_int((int)(char)unit_types[unit_type].defense);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_defense_suffix));
    if ((char)unit_types[unit_type].defense < '\n') {
      text_append_space();
    }
    text_append_int_2d((int)(char)unit_types[unit_type].hit_points / 10);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_hp_suffix));
    text_append_int((int)(char)unit_types[unit_type].firepower);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_firepower_suffix));
    text_append_int_2d((int)(char)unit_types[unit_type].cost);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_cost_sep));
    text_append_int((int)(char)unit_types[unit_type].hold);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_hold_sep));
    text_append_int_2d((int)(char)unit_types[unit_type].role);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_role_sep));
    text_append_advance_abbrev(unit_types[unit_type].prerequisite,1);
    text_append_space();
    for (bit = 0xe; -1 < bit; bit--) {
      crt_strcat(((i8 *)&textbuf.buf),
                   ((i8 *)&unit_editor_ctrl_kinds_block.units_txt_flag_digits) +
                   (((unit_types[unit_type].flags & 1 << ((byte)bit & 0x1f)) != 0) - 1 & 4));
    }
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&unit_editor_ctrl_kinds_block.units_txt_line_end));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 005af9e3 auto */
void __cdecl unit_editor_on_ok(void)

{
  iptr invalid_count;
  HWND hWnd;
  const char *str;
  iptr cmp;
  i32 old_name;
  int result;
  char *new_name;
  i8 *edited_name;
  UINT box_flags;
  iptr parent_wnd2;
  char rules_err_msg[32];
  char events_err_msg[32];
  iptr unit_type;
  int renamed_count;
  
  invalid_count = unit_editor_store_controls();
  if (invalid_count == 0) {
    renamed_count = 0;
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      new_name = editor_text_dlg_wnd_block.editor_item_name_recs[unit_type];
      str = label_string(unit_types[unit_type].name);
      cmp = _strcmp(str,new_name);
      if (cmp != 0) {
        edited_name = editor_text_dlg_wnd_block.editor_item_name_recs[unit_type];
        old_name = (iptr)label_string(unit_types[unit_type].name);
        result = events_rename_unit_refs((char *)old_name,edited_name);
        renamed_count += result;
      }
    }
    if (renamed_count != 0) {
      result = events_write_file();
      if (result == 0) {
        _sprintf(events_err_msg,str_error_updating_events_s_2,((i8 *)&lang_file_ext));
        box_flags = 0x10;
        str = str_file_i_o_error_7;
        new_name = events_err_msg;
        result = (int)((WndBase *)&editor_active.wnd->window)->get_window();
        MessageBoxA(((NativeWindow *)result)->hwnd,new_name,str,box_flags);
      }
    }
    unit_editor_store_rules();
    rules_edit_set_section(str_units_3,(RulesSectionWriterFn)unit_editor_write_units);
    result = rules_write_sections();
    if (result == 0) {
      _sprintf(rules_err_msg,str_error_updating_rules_s_6,((i8 *)&lang_file_ext));
      box_flags = 0x10;
      str = str_file_i_o_error_8;
      new_name = rules_err_msg;
      result = (int)((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(((NativeWindow *)result)->hwnd,new_name,str,box_flags);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    unit_editor_load_controls();
    unit_editor_redraw();
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice_16);
    dlg_set_parent_window(0);
    hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
    SetFocus(hWnd);
  }
  return;
}

/* 005afbca auto */
void __cdecl unit_editor_on_rename(void)

{
  size_t input_len;
  iptr parent_wnd2;
  char old_name[15];
  char new_name[260];
  int item;
  iptr sel;
  int result;
  
  item = editor_active.wnd->item;
  _strncpy(old_name,editor_text_dlg_wnd_block.editor_item_name_recs[item],0xf);
  old_name[14] = 0;
  do {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_push_parent_window(parent_wnd2);
    result = dlg_text_input(str_debug_11,str_unitname,0xe,old_name,new_name);
    dlg_pop_parent_window();
    if (result == -1) break;
    input_len = _strlen(new_name);
  } while (input_len == 0);
  if (-1 < result) {
    crt_strcpy(editor_text_dlg_wnd_block.editor_item_name_recs[item],new_name);
    sel = editor_active.wnd->combos[0].get_cur_sel();
    editor_active.wnd->combos[0].clear();
    for (item = 0; item < 0x3e; item++) {
      editor_active.wnd->combos[0].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[item]);
    }
    editor_active.wnd->combos[0].set_cur_sel(sel);
    unit_editor_redraw();
  }
  return;
}

/* 005afd3b auto */
void __cdecl unit_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_19,(const char *)&str_units_4);
  dlg_set_parent_window(0);
  return;
}

/* 005afd9a auto */
void __cdecl unit_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 005afdc2 auto */
void __cdecl unit_editor_edit_sprite(void)

{
  i32 title;
  i32 count;
  SpriteSheetSaveFn save_cb;
  
  save_cb = sprite_sheet_save_units;
  count = 0xb;
  title = (iptr)label_string(label_table.string_ids[0x1f6]);
  sprite_editor_run(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + editor_active.wnd->item * 0x3c)),(const char *)title,count,
                     save_cb);
  unit_editor_redraw();
  return;
}

/* 005afe15 auto */
void __cdecl unit_editor_edit_sound(void)

{
  i32 label;
  i8 *item_name;
  char title[128];
  
  unit_editor_store_controls();
  item_name = editor_text_dlg_wnd_block.editor_item_name_recs[editor_active.wnd->item];
  label = (iptr)label_string(label_table.string_ids[0x1f7]);
  /* BF-049 (Peter Johnson's ledger): a longer label is cut at 127 characters */
  snprintf(title,sizeof title,str_s_s_4,(char *)label,item_name);
  unit_sound_editor_run((const char *)title,editor_active.wnd->item);
  return;
}

/* 005afe84 auto */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __cdecl unit_editor_set_flag_check(i32 index,int checked)

{
  dlg_check_set(index,(uint)(checked != 0));
  return;
}

/* 005afea8 auto */
void __cdecl unit_editor_edit_flags(void)

{
  int item;
  int result;
  iptr parent_wnd2;
  uint flags2;
  
  item = editor_active.wnd->item;
  dlg_checks_clear();
  unit_editor_set_flag_check(0,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 1);
  unit_editor_set_flag_check(1,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 2);
  unit_editor_set_flag_check(2,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 4);
  unit_editor_set_flag_check(3,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 8);
  unit_editor_set_flag_check(4,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x10);
  unit_editor_set_flag_check(5,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x20);
  unit_editor_set_flag_check(6,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x40);
  unit_editor_set_flag_check(7,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x80);
  unit_editor_set_flag_check(8,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x100);
  unit_editor_set_flag_check(9,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x200);
  unit_editor_set_flag_check(10,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x400);
  unit_editor_set_flag_check(0xb,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x800);
  unit_editor_set_flag_check(0xc,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x1000);
  unit_editor_set_flag_check(0xd,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x2000);
  unit_editor_set_flag_check(0xe,(uint)editor_text_dlg_wnd_block.editor_item_recs[item].values[12] & 0x4000);
  text_set_string_arg(0,(const char *)editor_text_dlg_wnd_block.editor_item_name_recs[item]);
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  result = popup_game_flags((const char *)&str_abilities,1);
  if (result == 0) {
    result = dlg_check_get(0);
    flags2 = (uint)(result != 0);
    result = dlg_check_get(1);
    if (result != 0) {
      flags2 = flags2 | 2;
    }
    result = dlg_check_get(2);
    if (result != 0) {
      flags2 = flags2 | 4;
    }
    result = dlg_check_get(3);
    if (result != 0) {
      flags2 = flags2 | 8;
    }
    result = dlg_check_get(4);
    if (result != 0) {
      flags2 = flags2 | 0x10;
    }
    result = dlg_check_get(5);
    if (result != 0) {
      flags2 = flags2 | 0x20;
    }
    result = dlg_check_get(6);
    if (result != 0) {
      flags2 = flags2 | 0x40;
    }
    result = dlg_check_get(7);
    if (result != 0) {
      flags2 = flags2 | 0x80;
    }
    result = dlg_check_get(8);
    if (result != 0) {
      flags2 = flags2 | 0x100;
    }
    result = dlg_check_get(9);
    if (result != 0) {
      flags2 = flags2 | 0x200;
    }
    result = dlg_check_get(10);
    if (result != 0) {
      flags2 = flags2 | 0x400;
    }
    result = dlg_check_get(0xb);
    if (result != 0) {
      flags2 = flags2 | 0x800;
    }
    result = dlg_check_get(0xc);
    if (result != 0) {
      flags2 = flags2 | 0x1000;
    }
    result = dlg_check_get(0xd);
    if (result != 0) {
      flags2 = flags2 | 0x2000;
    }
    result = dlg_check_get(0xe);
    if (result != 0) {
      flags2 = flags2 | 0x4000;
    }
    editor_text_dlg_wnd_block.editor_item_recs[item].values[12] = flags2;
  }
  dlg_set_parent_window(0);
  return;
}

/* 005b02a5 auto */
void __cdecl unit_editor_update_domain_ctrls(void)

{
  int domain;
  
  domain = editor_active.wnd->combos[4].get_cur_sel();
  if (domain == 0) {
    ((Control *)&editor_active.wnd->edits[1])->hide();
    ((Control *)&editor_active.wnd->edits[3])->hide();
  }
  else if (domain == 1) {
    ((Control *)&editor_active.wnd->edits[1])->hide();
    ((Control *)&editor_active.wnd->edits[3])->show_and_redraw();
  }
  else if (domain == 2) {
    ((Control *)&editor_active.wnd->edits[1])->show_and_redraw();
    ((Control *)&editor_active.wnd->edits[3])->hide();
  }
  return;
}

/* 005b0373 auto */
void __cdecl unit_editor_on_combo(int ctrl_id)

{
  int invalid_count;
  HWND hWnd;
  i32 sel;
  i32 parent_wnd2;
  
  if (ctrl_id == 0xc9) {
    invalid_count = unit_editor_store_controls();
    if (invalid_count == 0) {
      sel = editor_active.wnd->combos[0].get_cur_sel();
      editor_active.wnd->item = sel;
      unit_editor_load_controls();
      unit_editor_update_domain_ctrls();
      unit_editor_redraw();
    }
    else {
      editor_active.wnd->combos[0].set_cur_sel(editor_active.wnd->item);
      unit_editor_load_controls();
      unit_editor_redraw();
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_debug_11,(const char *)&str_notice_17);
      dlg_set_parent_window(0);
      hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
      SetFocus(hWnd);
    }
  }
  else if (ctrl_id == 0xcd) {
    unit_editor_update_domain_ctrls();
  }
  return;
}

/* 005b0473 auto */
void EditorWindow::unit_editor_create_combo(int index)

{
  int x;
  int y;
  iptr label;
  u8 *parent2;
  iptr advance;
  iptr unit_type;
  i8 rect[16];
  
  x = unit_editor_ctrl_positions_block.unit_editor_combo_pos[index].x + client_x;
  y = unit_editor_ctrl_positions_block.unit_editor_combo_pos[index].y + client_y;
  if (index == 0) {
    rect_set_xywh((RECT *)rect,x - 0xf,y,0x82,font_height_uptr << 3);
  }
  else {
    rect_set_xywh((RECT *)rect,x - 0x1e,y,0xa0,font_height_uptr << 3);
  }
  x = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + index * 0x38 + 0x28))->create((Wnd *)parent2,x,
             (RECT *)rect);
  ((ComboBox *)(text_font + index * 0x38 + 0x28))->set_font(&editor_font_block.editor_font);
  ((ComboBox *)(text_font + index * 0x38 + 0x28))->set_callback(unit_editor_on_combo);
  switch(index) {
  case 0:
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      label = (iptr)label_string(unit_types[unit_type].name);
      ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 1:
  case 2:
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f1]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    for (advance = 0; advance < 100; advance++) {
      label = (iptr)label_string(advances[advance].name);
      ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 3:
    label = (iptr)label_string(label_table.string_ids[0x1e5]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1e6]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f8]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f9]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fa]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fb]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fc]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1fd]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    break;
  case 4:
    label = (iptr)label_string(label_table.string_ids[0x1fe]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1ff]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x200]);
    ((ComboBox *)(text_font + index * 0x38 + 0x28))->add_string((const char *)label);
  }
  return;
}

/* 005b08e8 auto */
void EditorWindow::unit_editor_create_edit(int index)

{
  int ctrl_id;
  u8 *parent2;
  i8 rect[16];
  
  rect_set_xywh((RECT *)rect,(((i32 *)&unit_editor_edit_x_table_block.unit_editor_edit_x_table))[index * 2] + client_x,
                     (((i32 *)&unit_editor_edit_x_table_block.unit_editor_edit_y_table))[index * 2] + client_y,0x30,font_height_uptr + 6);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  ((EditControl *)(numeric_edit_controls_area + index * 0x48 - 0x2c))->create((Wnd *)parent2,ctrl_id,
             (RECT *)rect,(const char *)&str_empty_186);
  ((EditControl *)(numeric_edit_controls_area + index * 0x48 - 0x2c))->set_max_len(3);
  ((EditControl *)(numeric_edit_controls_area + index * 0x48 - 0x2c))->set_callback(editor_numeric_edit_filter);
  return;
}

/* 005b09dc auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void EditorWindow::unit_editor_paint()

{
  byte key_color;
  i8 sprite_rect[16];
  Sprite *sprite;
  int text_y2;
  int text_x2;
  int pic_y;
  int pic_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect(this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh(((DibSurface *)*(int *)&editor_ui.bg_surface),this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  pic_x = client_x + 0x20;
  pic_y = client_y + 0x20;
  sprite = (Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + item_uptr * 0x3c);
  key_color = sprite->get_key_color();
  gfx_fill_rect_xywh(this,pic_x,pic_y,0x40,0x40,(uint)key_color);
  sprite->draw_keyed((RECT *)sprite_rect,this,0,pic_x,pic_y + 8);
  gfx_draw_tiled_frame(this,pic_x,pic_y,0x40,0x40,6);
  gfx_draw_tiled_frame(this,client_x + 0x79,client_y + 0x4f,0x14d,0x47,6);
  text_set_surface(this);
  text_set_font(&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  text_x2 = client_x + (int)wnd_width_u / 2 + 5;
  text_y2 = unit_editor_ctrl_positions_block.unit_editor_prereq_y + client_y + font_height_uptr * -2 - 2;
  text_clear();
  text_append_label(0x1e9);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_prereq_x + client_x + 0x32;
  text_y2 = ((unit_editor_ctrl_positions_block.unit_editor_prereq_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_obsolete_x + client_x + 0x32;
  text_y2 = ((unit_editor_ctrl_positions_block.unit_editor_obsolete_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1de);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_role_x + client_x + 0x32;
  text_y2 = ((unit_editor_ctrl_positions_block.unit_editor_role_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1df);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_ctrl_positions_block.unit_editor_domain_x + client_x + 0x32;
  text_y2 = ((client_y + unit_editor_ctrl_positions_block.unit_editor_domain_y_i32) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e0);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit_x_table[0] + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e1);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_holds_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_holds_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e2);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_move_x + client_x + 0x18;
  text_y2 = ((client_y + unit_editor_edit_x_table_block.unit_editor_edit2_y_i32) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e3);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit3_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit3_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e4);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit4_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit4_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e5);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit5_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit5_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e6);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit6_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit6_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e7);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = unit_editor_edit_x_table_block.unit_editor_edit7_x + client_x + 0x18;
  text_y2 = ((unit_editor_edit_x_table_block.unit_editor_edit7_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e8);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  wnd_blit_surface();
  return;
}

/* 005b1037 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void EditorWindow::unit_editor_window_run()

{
  DibSurface *this_00;
  uptr font_height;
  i32 title;
  int btn_x;
  int btn_y;
  iptr label;
  i32 wnd_style;
  i32 wnd_x;
  i32 wnd_y;
  u32 wnd_width;
  u32 wnd_height;
  i32 wnd_arg7;
  i32 wnd_arg8;
  i32 wnd_arg9;
  u8 *flags_parent;
  u8 *sound_parent;
  u8 *sprite_parent;
  u8 *cancel_parent;
  u8 *help_parent;
  u8 *rename_parent;
  u8 *ok_parent;
  DibSurface *bg_surface2;
  int i;
  i8 rect[16];
  GamePalette palette;
  i32 eh_next;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    bg_surface2 = (DibSurface *)0x0;
  }
  else {
    bg_surface2 = surface_ctor(this_00);
  }
  editor_ui.bg_surface = bg_surface2;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font(&editor_font_block.editor_font);
  ui_set_ctrl_font(&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  wnd_width_u = 0x230;
  wnd_height_u = 0x17c;
  item_uptr = 0;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editorsa_gif_2,10,0xc0,&palette);
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  wnd_arg9 = 0;
  wnd_arg8 = 0;
  wnd_arg7 = 0;
  wnd_height = wnd_height_u;
  wnd_width = wnd_width_u;
  wnd_y = 0;
  wnd_x = 0;
  wnd_style = 0xd;
  title = (iptr)label_string(label_table.string_ids[0x1cb]);
  this->FrameWindow::create((const char *)title,wnd_style,wnd_x,wnd_y,wnd_width,wnd_height,wnd_arg7,wnd_arg8,wnd_arg9);
  for (i = 0; i < 0xd; i++) {
    if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 9) {
      unit_editor_create_edit(unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot);
    }
    else if (unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].kind == 0xc) {
      unit_editor_create_combo(unit_editor_ctrl_kinds_block.unit_editor_ctrl_rows[i].slot);
    }
  }
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_x = client_width - 10;
  button_width_u = (int)(btn_x + (btn_x >> 0x1f & 3U)) >> 2;
  btn_y = (client_y + client_height) - (button_height_u + 2);
  btn_x = client_x + 2;
  rect_set_xywh((RECT *)rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((Wnd *)ok_parent,0x65,(RECT *)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)unit_editor_on_ok);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((RECT *)rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    rename_parent = (u8 *)0x0;
  }
  else {
    rename_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x2a]);
  rename_button.create((Wnd *)rename_parent,0x66,(RECT *)rect,(const char *)label);
  rename_button.set_callback((CtrlNotifyFn)unit_editor_on_rename);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((RECT *)rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((Wnd *)help_parent,0x66,(RECT *)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)unit_editor_on_help);
  rect_set_xywh((RECT *)rect,btn_x + button_width_u + 2,btn_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((Wnd *)cancel_parent,0x66,(RECT *)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)unit_editor_on_cancel);
  cancel_button.set_cancel_button();
  btn_x = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c))->width();
  button_width_u = btn_x + 0xd;
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_y = client_x + 0x19;
  btn_x = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c))->height();
  btn_x = btn_x * 2 + client_y + 0x28;
  rect_set_xywh((RECT *)rect,btn_y,btn_x,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    sprite_parent = (u8 *)0x0;
  }
  else {
    sprite_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((Wnd *)sprite_parent,0x65,(RECT *)rect,(const char *)label);
  icon_button.set_callback((CtrlNotifyFn)unit_editor_edit_sprite);
  btn_x = btn_x + button_height_u + 2;
  rect_set_xywh((RECT *)rect,btn_y,btn_x,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    sound_parent = (u8 *)0x0;
  }
  else {
    sound_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[500]);
  unit_sound_button.create((Wnd *)sound_parent,0x65,(RECT *)rect,(const char *)label);
  unit_sound_button.set_callback((CtrlNotifyFn)unit_editor_edit_sound);
  rect_set_xywh((RECT *)rect,btn_y,btn_x + button_height_u + 2,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    flags_parent = (u8 *)0x0;
  }
  else {
    flags_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f5]);
  unit_flags_button.create((Wnd *)flags_parent,0x65,(RECT *)rect,(const char *)label);
  unit_flags_button.set_callback((CtrlNotifyFn)unit_editor_edit_flags);
  window.set_keyboard_nav(0);
  unit_editor_ctrl_kinds_block.unit_editor_move_max_i32 = (i32)(0x7f / (ulonglong)(longlong)(int)(uint)cosmic.road_move_mult);
  unit_editor_ctrl_kinds_block.unit_editor_hp_max_i32 = 0xc;
  unit_editor_load_rules();
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  unit_editor_load_controls();
  unit_editor_update_domain_ctrls();
  combo_count_u = 5;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(unit_editor_redraw);
  repaint();
  this->SurfaceWindow::show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (editor_lists.running != 0) {
    win_pump_one_message();
  }
  if (editor_ui.bg_surface != (DibSurface *)0x0) {
    editor_ui.bg_surface->surface_delete(1);
  }
  editor_ui.bg_surface = (DibSurface *)0x0;
  combo_count_u = 0;
  return;
}

/* 005b1a29 auto */
void __cdecl unit_editor_run(void)

{
  EditorWindow editor_wnd;
  i32 eh_next;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.unit_editor_window_run();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}
