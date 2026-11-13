/* The game, cities: the cheat menu's city editor and the improvements editor: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00428d00 auto */
void __cdecl city_editor_request_redraw(void)

{
  editor_active.wnd->city_editor_draw();
  return;
}

/* 00428d1b auto */
void __cdecl city_editor_on_ok(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  menu_update_states();
  return;
}

/* 00428d48 auto */
void __cdecl city_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_2,(const char *)&str_cities);
  dlg_set_parent_window(0);
  return;
}

/* 00428da7 auto */
void __cdecl city_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 00428dcf auto */
void __cdecl city_editor_on_edit_icon(void)

{
  i32 title_text;
  i32 sprite_count;
  SpriteSheetSaveFn save_callback;
  
  save_callback = sprite_sheet_save_cities;
  sprite_count = 1;
  title_text = (iptr)label_string(label_table.string_ids[0x1f6]);
  sprite_editor_run((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) +
                           editor_active.wnd->item_3 * 0x3c +
                           editor_active.wnd->item * 0x1e0 +
                           editor_active.wnd->item_2 * 0x78),(const char *)title_text,sprite_count,save_callback);
  city_editor_request_redraw();
  return;
}

/* 00428e50 auto */
void __cdecl city_editor_on_edit_misc(void)

{
  int choice;
  i32 title_text;
  SpriteSheetSaveFn save_callback;
  iptr parent_wnd2;
  i8 *sprites;
  i32 sprite_count2;
  
  sprite_count2 = 2;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_citymisc,1);
  /* BF-115 (Peter Johnson's ledger): a choice past the 20 sprites (a row
     added to a scenario's DEBUG.TXT) is no choice */
  if ((choice != -1) && (-1 < choice) && (choice < 0x14)) {
    switch(choice) {
    case 0:
      sprites = ((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites);
      break;
    case 1:
      sprites = ((i8 *)&airbase_sprites_block.airbase_sprites);
      break;
    default:
      if (choice < 10) {
        sprites = ((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + (choice * 8 - 0x10) * 0xf;
      }
      else {
        sprites = ((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + (choice * 8 - 0x60) * 0xf;
      }
      sprite_count2 = 3;
      break;
    case 10:
      sprites = ((i8 *)&fortify_overlay_sprites_block.fortress_sprite);
      break;
    case 0xb:
      sprites = ((i8 *)&airbase_sprites_block.airbase_occupied_sprite);
    }
    if (sprites != (i8 *)0x0) {
      save_callback = sprite_sheet_save_cities;
      title_text = (iptr)label_string(label_table.string_ids[0x1f6]);
      sprite_editor_run((Sprite *)sprites,(const char *)title_text,sprite_count2,save_callback);
    }
  }
  dlg_set_parent_window(0);
  return;
}

/* 00428fd2 auto */
void __cdecl city_editor_on_combo_change(i32 unused)

{
  i32 selection;
  
  selection = editor_active.wnd->combos[0].get_cur_sel();
  editor_active.wnd->item = selection;
  selection = editor_active.wnd->combos[1].get_cur_sel();
  editor_active.wnd->item_2 = selection;
  selection = editor_active.wnd->combos[2].get_cur_sel();
  editor_active.wnd->item_3 = selection;
  city_editor_request_redraw();
  return;
}

/* 0042903e auto */
void EditorWindow::city_editor_create_combo(int combo_index)

{
  int ctrl_id;
  iptr label;
  u8 *parent_wnd2;
  i8 rect[16];
  
  rect_set_xywh((RECT *)rect,(((i32 *)&city_editor_combo_x_table_block.city_editor_combo_x_table))[combo_index * 2] + client_x - 0x1e,
                     (((i32 *)&city_editor_combo_x_table_block.city_editor_combo_y_table))[combo_index * 2] + client_y,0xa0,font_height_uptr << 3);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->create((Wnd *)parent_wnd2,ctrl_id,
             (RECT *)rect);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_font(&editor_font_block.editor_font);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_callback(city_editor_on_combo_change);
  if (combo_index == 0) {
    label = (iptr)label_string(label_table.string_ids[0x20d]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x20e]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x20f]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x210]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x211]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x212]);
    ((ComboBox *)&combos_u)->add_string((const char *)label);
  }
  else if (combo_index == 1) {
    label = (iptr)label_string(label_table.string_ids[0x213]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x214]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x215]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x216]);
    ((ComboBox *)&field_334)->add_string((const char *)label);
  }
  else if (combo_index == 2) {
    label = (iptr)label_string(label_table.string_ids[0x217]);
    ((ComboBox *)combo_bytes_36c)->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x218]);
    ((ComboBox *)combo_bytes_36c)->add_string((const char *)label);
  }
  return;
}

/* 004293a8 auto */
void EditorWindow::city_editor_draw()

{
  byte key_color;
  iptr label;
  i8 draw_rect[16];
  Sprite *sprite;
  int label_y;
  int label_x;
  int icon_y;
  int icon_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect(this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh(((DibSurface *)*(int *)&editor_ui.bg_surface),this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  icon_x = client_x + 0x20;
  icon_y = client_y + 0x20;
  sprite = (Sprite *)
             (((i8 *)&gfx_sheet_surface_block.city_sprites) +
             item_3_u * 0x3c + item_uptr * 0x1e0 + item_2_uptr * 0x78);
  key_color = sprite->get_key_color();
  gfx_fill_rect_xywh(this,icon_x,icon_y,0x40,0x40,(uint)key_color);
  sprite->draw_keyed((RECT *)draw_rect,this,0,icon_x,icon_y + 8);
  gfx_draw_tiled_frame(this,icon_x,icon_y,0x40,0x40,6);
  text_set_surface(this);
  text_set_font(&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  label_x = city_editor_combo_x_table_block.city_editor_combo_x_table[0] + client_x + 0x32;
  label_y = ((city_editor_combo_x_table_block.city_editor_combo_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x219]);
  text_append_str((const char *)label);
  text_draw_centered(this,textbuf.buf,label_x,label_y,0);
  label_x = city_editor_combo_x_table_block.city_editor_combo2_x + client_x + 0x32;
  label_y = ((city_editor_combo_x_table_block.city_editor_combo2_y + client_y) - font_height_uptr) - 2;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x21a]);
  text_append_str((const char *)label);
  text_draw_centered(this,textbuf.buf,label_x,label_y,0);
  label_x = city_editor_combo_x_table_block.city_editor_combo3_x + client_x + 0x32;
  label_y = ((city_editor_combo_x_table_block.city_editor_combo3_y + client_y) - font_height_uptr) - 2;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x21b]);
  text_append_str((const char *)label);
  text_draw_centered(this,textbuf.buf,label_x,label_y,0);
  wnd_blit_surface();
  return;
}

/* 00429671 auto */
void EditorWindow::city_editor_open_window()

{
  DibSurface *this_00;
  uptr font_height;
  i32 title_text;
  int button_x;
  int button_y;
  iptr label;
  i32 style;
  i32 arg_x;
  i32 arg_y;
  u32 width;
  u32 height;
  i32 arg_7;
  i32 arg_8;
  i32 arg_9;
  u8 *parent_wnd_misc;
  u8 *parent_wnd_icon;
  u8 *parent_wnd_cancel;
  u8 *parent_wnd_help;
  u8 *parent_wnd_ok;
  DibSurface *bg_surface2;
  int i;
  i8 rect[16];
  GamePalette palette;
  
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
  editor_ui.bg_surface->load_gif_file((const char *)&str_editorsq_gif,10,0xc0,&palette);
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  arg_9 = 0;
  arg_8 = 0;
  arg_7 = 0;
  height = wnd_height_u;
  width = wnd_width_u;
  arg_y = 0;
  arg_x = 0;
  style = 0xd;
  title_text = (iptr)label_string(label_table.string_ids[0x1ce]);
  this->FrameWindow::create((const char *)title_text,style,arg_x,arg_y,width,height,arg_7,arg_8,arg_9);
  for (i = 0; i < 3; i++) {
    city_editor_create_combo(i);
  }
  button_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = button_x + 8;
  button_width_u = (int)(client_width - 8) / 3;
  button_y = (client_y + client_height) - (button_height_u + 2);
  button_x = client_x + 2;
  rect_set_xywh((RECT *)rect,button_x,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_ok = (u8 *)0x0;
  }
  else {
    parent_wnd_ok = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((Wnd *)parent_wnd_ok,0x65,(RECT *)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)city_editor_on_ok);
  button_x = button_x + button_width_u + 2;
  rect_set_xywh((RECT *)rect,button_x,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_help = (u8 *)0x0;
  }
  else {
    parent_wnd_help = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((Wnd *)parent_wnd_help,0x66,(RECT *)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)city_editor_on_help);
  rect_set_xywh((RECT *)rect,button_x + button_width_u + 2,button_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_cancel = (u8 *)0x0;
  }
  else {
    parent_wnd_cancel = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((Wnd *)parent_wnd_cancel,0x66,(RECT *)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)city_editor_on_cancel);
  cancel_button.set_cancel_button();
  button_width_u = 0x4d;
  button_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = button_x + 8;
  button_x = client_x + 0x19;
  button_y = client_y + 0x68;
  rect_set_xywh((RECT *)rect,button_x,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_icon = (u8 *)0x0;
  }
  else {
    parent_wnd_icon = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((Wnd *)parent_wnd_icon,0x65,(RECT *)rect,(const char *)label);
  icon_button.set_callback((CtrlNotifyFn)city_editor_on_edit_icon);
  rect_set_xywh((RECT *)rect,button_x,button_y + button_height_u + 2,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd_misc = (u8 *)0x0;
  }
  else {
    parent_wnd_misc = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23c]);
  misc_button.create((Wnd *)parent_wnd_misc,0x65,(RECT *)rect,(const char *)label);
  misc_button.set_callback((CtrlNotifyFn)city_editor_on_edit_misc);
  window.set_keyboard_nav(0);
  item_3_u = 0;
  item_2_uptr = item_3_u;
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  ((ComboBox *)&field_334)->set_cur_sel(item_2_uptr);
  ((ComboBox *)combo_bytes_36c)->set_cur_sel(item_3_u);
  combo_count_u = 3;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(city_editor_request_redraw);
  city_editor_on_combo_change(0xc9);
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

/* 00429e77 auto */
void __cdecl city_editor_run(void)

{
  
  EditorWindow editor_wnd;
  editor_wnd.city_editor_open_window();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 004d8bc0 auto */
void __cdecl impr_editor_load_rules(void)

{
  char *_Source;
  size_t _Count;
  iptr improvement;
  
  for (improvement = 0; improvement < IMP_COUNT; improvement++) {
    _Count = 0x28;
    _Source = label_string(improvements[improvement].name);
    _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[improvement],_Source,_Count);
    editor_text_dlg_wnd_block.editor_item_name_recs[improvement][0x27] = 0;
    editor_text_dlg_wnd_block.editor_item_recs[improvement].values[0] = (uint)improvements[improvement].cost;
    editor_text_dlg_wnd_block.editor_item_recs[improvement].values[1] = (uint)improvements[improvement].upkeep;
    editor_text_dlg_wnd_block.editor_item_recs[improvement].values[2] = (int)improvements[improvement].preq;
    if (IMP_FIRST_WONDER < improvement) {
      editor_text_dlg_wnd_block.editor_item_recs[improvement].values[3] = (int)wonder_expires[improvement - IMP_FIRST_WONDER];
    }
  }
  return;
}

/* 004d8caa auto */
void __cdecl impr_editor_store_rules(void)

{
  char *_Dest;
  char *_Source;
  size_t _Count;
  iptr improvement;
  
  for (improvement = 0; improvement < IMP_COUNT; improvement++) {
    _Count = 0x19;
    _Source = editor_text_dlg_wnd_block.editor_item_name_recs[improvement];
    _Dest = label_string(improvements[improvement].name);
    editor_copy_pool_name(_Dest,_Source,_Count);  /* B32 (docs/BUGS.md) */
    improvements[improvement].cost = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[0];
    improvements[improvement].upkeep = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[1];
    improvements[improvement].preq = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[2];
    if (IMP_FIRST_WONDER < improvement) {
      wonder_expires[improvement - IMP_FIRST_WONDER] = (i8)editor_text_dlg_wnd_block.editor_item_recs[improvement].values[3];
    }
  }
  return;
}

/* 004d8d80 auto */
void __cdecl impr_editor_load_controls(void)

{
  int ctrl_id;
  int ctrl_index;
  char value_text[8];
  int sel_index;
  
  for (ctrl_index = 1; ctrl_index < 5; ctrl_index++) {
    if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 9) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      _sprintf(value_text,((i8 *)&impr_editor_ctrl_kinds_block.impr_editor_value_fmt),
               *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58));
      ((Control *)
                 &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->set_text(
                 (const char *)value_text);
    }
    else if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 0xc) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      sel_index = *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58) + 2;
      editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot].set_cur_sel(sel_index);
    }
  }
  return;
}

/* 004d8ed6 auto */
int __cdecl impr_editor_save_controls(void)

{
  int field;
  i32 clamped;
  int ctrl_index;
  int clamp_count;
  char value_text[8];
  int value;
  
  clamp_count = 0;
  for (ctrl_index = 1; ctrl_index < 5; ctrl_index++) {
    if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 9) {
      ((Control *)
                 &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->Control::get_text(
                 (char *)value_text);
      field = ((Control *)
                         &editor_active.wnd->edits[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      field -= 0xca;
      value = _atoi(value_text);
      clamped = int_clamp(value,*(i32 *)(((i8 *)&impr_editor_ctrl_kinds_block.impr_editor_field_min_table) + field * 4),
                                 *(i32 *)(((i8 *)&impr_editor_ctrl_kinds_block.impr_editor_field_max_table) + field * 4));
      editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] = clamped;
      if (editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[field] != value) {
        clamp_count++;
      }
    }
    else if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 0xc) {
      value = editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot].get_cur_sel();
      value -= 2;
      field = ((Control *)
                         &editor_active.wnd->combos[impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot])->get_id();
      *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_active.wnd->item * 0x58 + field * 4) = value;
    }
  }
  return clamp_count;
}

/* 004d9095 auto */
void __cdecl impr_editor_redraw(void)

{
  editor_active.wnd->impr_editor_paint();
  return;
}

/* 004d90b0 auto */
i32 __cdecl impr_editor_write_section(FILE *file)

{
  char *name_str;
  size_t len;
  size_t name_len;
  iptr improvement;
  
  for (improvement = 0; improvement < IMP_COUNT; improvement++) {
    text_clear();
    text_append_string_id(improvements[improvement].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&impr_editor_ctrl_kinds_block.rules_improve_name_sep_str));
    name_str = label_string(improvements[improvement].name);
    len = _strlen(name_str);
    if (len < 0x19) {
      name_str = label_string(improvements[improvement].name);
      name_len = _strlen(name_str);
    }
    else {
      name_len = 0x19;
    }
    text_append_spaces(0x19 - name_len);
    text_append_int_2d((uint)improvements[improvement].cost);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&impr_editor_ctrl_kinds_block.rules_improve_cost_sep_str));
    text_append_int_2d((uint)improvements[improvement].upkeep);
    crt_strcat(((i8 *)&textbuf.buf),str_empty_137);
    text_append_advance_abbrev(improvements[improvement].preq,1);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&rules_improve_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 004d91f8 auto */
i32 __cdecl impr_editor_write_endwonder(FILE *file)

{
  iptr improvement;
  
  for (improvement = IMP_FIRST_WONDER; improvement < IMP_COUNT; improvement++) {
    text_clear();
    text_append_advance_abbrev(wonder_expires[improvement - IMP_FIRST_WONDER],1);
    text_append_spaces(8);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&rules_endwonder_comment_sep_str));
    text_append_string_id(improvements[improvement].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&rules_endwonder_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 004d929a auto */
void __cdecl impr_editor_on_ok(void)

{
  iptr clamp_count;
  HWND hWnd;
  iptr write_ok;
  NativeWindow *wnd;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  iptr parent_wnd2;
  char msg_text[32];
  
  clamp_count = impr_editor_save_controls();
  if (clamp_count == 0) {
    impr_editor_store_rules();
    rules_edit_set_section(str_improve_2,(RulesSectionWriterFn)impr_editor_write_section);
    rules_edit_add_section((const char *)&str_endwonder_3,(RulesSectionWriterFn)impr_editor_write_endwonder);
    write_ok = rules_write_sections();
    if (write_ok == 0) {
      _sprintf(msg_text,str_error_updating_rules_s_3,((i8 *)&lang_file_ext));
      uType = 0x10;
      lpCaption = str_file_i_o_error_3;
      lpText = msg_text;
      wnd = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    impr_editor_load_controls();
    impr_editor_redraw();
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice_10);
    dlg_set_parent_window(0);
    hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
    SetFocus(hWnd);
  }
  return;
}

/* 004d93b9 auto */
void __cdecl impr_editor_on_rename(void)

{
  int item_index;
  size_t len;
  iptr parent_wnd2;
  int item;
  char old_name[25];
  char new_name[260];
  iptr cur_sel;
  int dlg_result;
  
  item_index = editor_active.wnd->item;
  _strncpy(old_name,editor_text_dlg_wnd_block.editor_item_name_recs[item_index],0x19);
  old_name[24] = 0;
  do {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_push_parent_window(parent_wnd2);
    dlg_result = dlg_text_input(str_debug_11,str_imprname,0x18,old_name,new_name);
    dlg_pop_parent_window();
    if (dlg_result == -1) break;
    len = _strlen(new_name);
  } while (len == 0);
  if (-1 < dlg_result) {
    crt_strcpy(editor_text_dlg_wnd_block.editor_item_name_recs[item_index],new_name);
    cur_sel = editor_active.wnd->combos[0].get_cur_sel();
    editor_active.wnd->combos[0].clear();
    for (item = 0; item < 0x43; item++) {
      editor_active.wnd->combos[0].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[item]);
    }
    editor_active.wnd->combos[0].set_cur_sel(cur_sel);
    impr_editor_redraw();
  }
  return;
}

/* 004d953f auto */
void __cdecl impr_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_4,(const char *)&str_improvements);
  dlg_set_parent_window(0);
  return;
}

/* 004d959e auto */
void __cdecl impr_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 004d95c6 auto */
void __cdecl impr_editor_edit_icon(void)

{
  i32 title;
  i32 sprite_count;
  SpriteSheetSaveFn save_callback;
  
  save_callback = sprite_sheet_save_building_icons;
  sprite_count = 4;
  title = (iptr)label_string(label_table.string_ids[0x1f6]);
  sprite_editor_run(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + editor_active.wnd->item * 0x3c)),(const char *)title,sprite_count,
                     save_callback);
  impr_editor_redraw();
  return;
}

/* 004d9619 auto */
void __cdecl impr_editor_edit_misc_icon(void)

{
  int choice;
  i32 title;
  SpriteSheetSaveFn save_callback;
  iptr parent_wnd2;
  i8 *sprites;
  i32 sprite_count2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  choice = popup_text_flags((const char *)&str_debug_11,(const char *)&str_imprmisc,1);
  /* BF-114 (Peter Johnson's ledger): a choice past the cursor and the eight
     explosions (a row added to a scenario's DEBUG.TXT) is no choice */
  if ((choice != -1) && (-1 < choice) && (choice < 9)) {
    if (choice == 0) {
      sprites = ((i8 *)&map_cursor_sprite_block.map_cursor_sprite);
      sprite_count2 = 5;
    }
    else {
      sprites = ((i8 *)&government_sprites_block.explosion_sprites) + (choice * 4 - 4) * 0xf;
      sprite_count2 = 6;
    }
    if (sprites != (i8 *)0x0) {
      save_callback = sprite_sheet_save_building_icons;
      title = (iptr)label_string(label_table.string_ids[0x1f6]);
      sprite_editor_run((Sprite *)sprites,(const char *)title,sprite_count2,save_callback);
    }
  }
  dlg_set_parent_window(0);
  return;
}

/* 004d9718 auto */
void __cdecl impr_editor_on_select(int ctrl_id)

{
  int clamp_count;
  HWND hWnd;
  i32 cur_sel;
  i32 parent_wnd2;
  
  if (ctrl_id == 0xc9) {
    clamp_count = impr_editor_save_controls();
    if (clamp_count == 0) {
      cur_sel = editor_active.wnd->combos[0].get_cur_sel();
      editor_active.wnd->item = cur_sel;
      if (editor_active.wnd->item == 0) {
        editor_active.wnd->icon_button.disable();
      }
      else {
        editor_active.wnd->icon_button.enable();
      }
      if (editor_active.wnd->item < IMP_FIRST_WONDER) {
        ((Control *)&editor_active.wnd->combos[2])->hide();
      }
      else {
        ((Control *)&editor_active.wnd->combos[2])->show_and_redraw();
      }
      impr_editor_load_controls();
      impr_editor_redraw();
    }
    else {
      editor_active.wnd->combos[0].set_cur_sel(editor_active.wnd->item);
      impr_editor_load_controls();
      impr_editor_redraw();
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_debug_11,(const char *)&str_notice_11);
      dlg_set_parent_window(0);
      hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
      SetFocus(hWnd);
    }
  }
  return;
}

/* 004d986e auto */
void EditorWindow::impr_editor_create_combo(int combo_index)

{
  int ctrl_id;
  iptr label;
  u8 *parent_wnd2;
  iptr improvement;
  iptr advance;
  i8 rect[16];
  
  rect_set_xywh((RECT *)rect,impr_editor_combo_x_table_block.impr_editor_combo_pos[combo_index].x + client_x - 0x32,
                     impr_editor_combo_x_table_block.impr_editor_combo_pos[combo_index].y + client_y,200,
                     font_height_uptr << 3);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->create((Wnd *)parent_wnd2,ctrl_id,
             (RECT *)rect);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_font(&editor_font_block.editor_font);
  ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->set_callback(impr_editor_on_select);
  if (combo_index == 0) {
    for (improvement = 0; improvement < IMP_COUNT; improvement++) {
      label = (iptr)label_string(improvements[improvement].name);
      ((ComboBox *)&combos_u)->add_string((const char *)label);
    }
  }
  else if ((0 < combo_index) && (combo_index < 3)) {
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f1]);
    ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->add_string((const char *)label);
    for (advance = 0; advance < ADV_COUNT; advance++) {
      label = (iptr)label_string(advances[advance].name);
      ((ComboBox *)(text_font + combo_index * 0x38 + 0x28))->add_string((const char *)label);
    }
  }
  return;
}

/* 004d9a9f auto */
void EditorWindow::impr_editor_create_edit(int edit_index)

{
  int ctrl_id;
  u8 *parent_wnd2;
  i8 rect[16];
  
  rect_set_xywh((RECT *)rect,(((i32 *)&impr_editor_edit_x_table_block.impr_editor_edit_x_table))[edit_index * 2] + client_x,
                     (((i32 *)&impr_editor_edit_x_table_block.impr_editor_edit_y_table))[edit_index * 2] + client_y,0x30,font_height_uptr + 6);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((EditControl *)(numeric_edit_controls_area + edit_index * 0x48 - 0x2c))->create((Wnd *)parent_wnd2,ctrl_id,
             (RECT *)rect,(const char *)&str_empty_138);
  ((EditControl *)(numeric_edit_controls_area + edit_index * 0x48 - 0x2c))->set_max_len(3);
  ((EditControl *)(numeric_edit_controls_area + edit_index * 0x48 - 0x2c))->set_callback(editor_numeric_edit_filter);
  return;
}

/* 004d9b93 auto */
void EditorWindow::impr_editor_paint()

{
  byte key_color;
  int open_err;
  char *percent_pos;
  size_t line_len;
  i8 sprite_rect[16];
  Sprite *icon_sprite;
  int text_y2;
  char *line;
  int text_x2;
  int icon_y;
  int icon_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect(this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh(((DibSurface *)*(int *)&editor_ui.bg_surface),this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  icon_x = client_x + 0x20;
  icon_y = client_y + 0x20;
  if ((int)item_uptr < IMP_FIRST_WONDER) {
    icon_sprite = (Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + item_uptr * 0x3c);
  }
  else {
    icon_sprite = (Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (item_uptr * 4 - 0x9c) * 0xf);
  }
  if (item_uptr == 0) {
    gfx_fill_rect_xywh(this,icon_x,icon_y,0x48,0x28,10);
  }
  else {
    key_color = icon_sprite->get_key_color();
    gfx_fill_rect_xywh(this,icon_x,icon_y,0x48,0x28,(uint)key_color);
    gfx_set_scale_ratio(2,1);
    icon_sprite->draw((RECT *)sprite_rect,this,icon_x,icon_y);
    gfx_set_scale_ratio(1,1);
  }
  gfx_draw_tiled_frame(this,icon_x,icon_y,0x48,0x28,6);
  text_set_surface(this);
  text_set_font(&editor_font_block.editor_font);
  text_set_colors(10,10,0,0);
  gfx_fill_rect_xywh(this,client_x + 0x32,client_y + 0xe8,0x1cc,0x6e,0x29);
  gfx_draw_frame_rect(this,client_x + 0x32,client_y + 0xe8,0x1cc,0x6e,10);
  text_x2 = client_x + 0x40;
  text_y2 = client_y + 0xee;
  text_clear();
  text_append((const char *)&str_pediaimprove);
  text_append_int(item_uptr);
  open_err = text_open_section((const char *)&str_pedia,(const char *)&textbuf.buf);
  if (open_err == 0) {
    do {
      line = text_read_line_or_end(1);
      if (*line == '^') {
        line++;
      }
      percent_pos = _strstr(line,((i8 *)&impr_editor_help_percent_str));
      if (percent_pos != (char *)0x0) {
        percent_pos = _strstr(line,((i8 *)&impr_editor_help_percent2_str));
        percent_pos[1] = ' ';
      }
      text_draw_shadowed(this,(const char *)line,text_x2,text_y2);
      text_y2 += font_height_uptr;
      line_len = _strlen(line);
    } while (1 < line_len);
  }
  text_set_colors(0x29,0x12,1,1);
  text_x2 = client_x + (int)wnd_width_u / 2;
  text_y2 = (client_y - font_height_uptr) + 0xe6;
  text_clear();
  text_append_label(0x1ea);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = client_x + (int)wnd_width_u / 2;
  text_y2 = impr_editor_combo_x_table_block.impr_editor_combo1_y + client_y + font_height_uptr * -2 - 2;
  text_clear();
  text_append_label(0x1e9);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_combo_x_table_block.impr_editor_combo1_x + client_x + 0x32;
  text_y2 = ((impr_editor_combo_x_table_block.impr_editor_combo1_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_combo_x_table_block.impr_editor_combo2_x + client_x + 0x32;
  text_y2 = ((impr_editor_combo_x_table_block.impr_editor_combo2_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1de);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_edit_x_table_block.impr_editor_edit_x_table[0] + client_x + 0x18;
  text_y2 = ((impr_editor_edit_x_table_block.impr_editor_edit_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e1);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  text_x2 = impr_editor_edit_x_table_block.impr_editor_edit1_x + client_x + 0x18;
  text_y2 = ((impr_editor_edit_x_table_block.impr_editor_edit1_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x9e);
  text_draw_centered(this,textbuf.buf,text_x2,text_y2,0);
  wnd_blit_surface();
  return;
}

/* 004da107 auto */
void EditorWindow::impr_editor_window_run()

{
  DibSurface *this_00;
  uptr line_height;
  i32 title;
  int btn_x;
  int btn_y;
  iptr label;
  i32 style;
  i32 wnd_x;
  i32 wnd_y;
  u32 wnd_width;
  u32 wnd_height;
  i32 arg7;
  i32 arg8;
  i32 arg9;
  u8 *misc_icon_parent;
  u8 *icon_parent;
  u8 *cancel_parent;
  u8 *help_parent;
  u8 *rename_parent;
  u8 *ok_parent;
  DibSurface *bg_surface2;
  int ctrl_index;
  i8 button_rect[16];
  GamePalette palette;
  
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
  item_uptr = 1;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editoras_gif,10,0xc0,&palette);
  line_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = line_height;
  arg9 = 0;
  arg8 = 0;
  arg7 = 0;
  wnd_height = wnd_height_u;
  wnd_width = wnd_width_u;
  wnd_y = 0;
  wnd_x = 0;
  style = 0xd;
  title = (iptr)label_string(label_table.string_ids[0x1cc]);
  this->FrameWindow::create((const char *)title,style,wnd_x,wnd_y,wnd_width,wnd_height,arg7,arg8,arg9);
  for (ctrl_index = 0; ctrl_index < 5; ctrl_index++) {
    if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 9) {
      impr_editor_create_edit(impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot);
    }
    else if (impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].kind == 0xc) {
      impr_editor_create_combo(impr_editor_ctrl_kinds_block.impr_editor_ctrl_rows[ctrl_index].slot);
    }
  }
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_x = client_width - 10;
  button_width_u = (int)(btn_x + (btn_x >> 0x1f & 3U)) >> 2;
  btn_y = (client_y + client_height) - (button_height_u + 2);
  btn_x = client_x + 2;
  rect_set_xywh((RECT *)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((Wnd *)ok_parent,0x65,(RECT *)button_rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)impr_editor_on_ok);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((RECT *)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    rename_parent = (u8 *)0x0;
  }
  else {
    rename_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x2a]);
  rename_button.create((Wnd *)rename_parent,0x66,(RECT *)button_rect,(const char *)label);
  rename_button.set_callback((CtrlNotifyFn)impr_editor_on_rename);
  btn_x = btn_x + button_width_u + 2;
  rect_set_xywh((RECT *)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((Wnd *)help_parent,0x66,(RECT *)button_rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)impr_editor_on_help);
  rect_set_xywh((RECT *)button_rect,btn_x + button_width_u + 2,btn_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((Wnd *)cancel_parent,0x66,(RECT *)button_rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)impr_editor_on_cancel);
  cancel_button.set_cancel_button();
  button_width_u = 0x55;
  btn_x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = btn_x + 8;
  btn_x = client_x + 0x19;
  btn_y = client_y + 0x50;
  rect_set_xywh((RECT *)button_rect,btn_x,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    icon_parent = (u8 *)0x0;
  }
  else {
    icon_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((Wnd *)icon_parent,0x65,(RECT *)button_rect,(const char *)label);
  icon_button.set_callback((CtrlNotifyFn)impr_editor_edit_icon);
  rect_set_xywh((RECT *)button_rect,btn_x,btn_y + button_height_u + 2,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    misc_icon_parent = (u8 *)0x0;
  }
  else {
    misc_icon_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23c]);
  misc_button.create((Wnd *)misc_icon_parent,0x65,(RECT *)button_rect,(const char *)label);
  misc_button.set_callback((CtrlNotifyFn)impr_editor_edit_misc_icon);
  window.set_keyboard_nav(0);
  impr_editor_load_rules();
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  impr_editor_load_controls();
  combo_count_u = 3;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(impr_editor_redraw);
  impr_editor_on_select(0xc9);
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

/* 004da9e2 auto */
void __cdecl impr_editor_run(void)

{
  
  EditorWindow editor_wnd;
  editor_wnd.impr_editor_window_run();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}
