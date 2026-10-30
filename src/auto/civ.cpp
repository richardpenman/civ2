/* The game, civilizations: advances, research, government, the rules: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"
#include "lang.h"
#include "rules.h"

/* 0040bd10 auto */
i32 __cdecl civ_max_tax_rate(iptr civ)

{
  i32 max_rate;
  
  if (civs[civ].government < GOV_MONARCHY) {
    max_rate = 6;
  }
  else if (civs[civ].government == GOV_MONARCHY) {
    max_rate = 7;
  }
  else if (civs[civ].government < GOV_DEMOCRACY) {
    max_rate = 8;
  }
  else {
    max_rate = 10;
  }
  return max_rate;
}

/* 00410070 auto */
i32 __cdecl civ_get_adjective(iptr civ)

{
  i32 adjective;
  
  adjective = civ_adjective(civ);
  return adjective;
}

/* 004151e0 auto */
void __cdecl tech_editor_load_items(void)

{
  char *_Source;
  size_t _Count;
  iptr advance;
  
  for (advance = 0; advance < 100; advance++) {
    _Count = 0x28;
    _Source = label_string(advances[advance].name);
    _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[advance],_Source,_Count);
    editor_text_dlg_wnd_block.editor_item_name_recs[advance][0x27] = 0;
    editor_text_dlg_wnd_block.editor_item_recs[advance].values[0] = (int)advances[advance].preq[0];
    editor_text_dlg_wnd_block.editor_item_recs[advance].values[1] = (int)advances[advance].preq[1];
    editor_text_dlg_wnd_block.editor_item_recs[advance].values[2] = (int)(char)advances[advance].epoch;
    editor_text_dlg_wnd_block.editor_item_recs[advance].values[3] = (int)(char)advances[advance].category;
    editor_text_dlg_wnd_block.editor_item_recs[advance].values[4] = (int)advances[advance].ai_value;
    editor_text_dlg_wnd_block.editor_item_recs[advance].values[5] = (int)advances[advance].civilized_mod;
  }
  return;
}

/* 00415307 auto */
void __cdecl tech_editor_save_items(void)

{
  char *_Dest;
  char *_Source;
  size_t _Count;
  iptr advance;
  
  for (advance = 0; advance < 100; advance++) {
    _Count = 0x14;
    _Source = editor_text_dlg_wnd_block.editor_item_name_recs[advance];
    _Dest = label_string(advances[advance].name);
    editor_copy_pool_name(_Dest,_Source,_Count);  /* B32 (docs/BUGS.md) */
    advances[advance].preq[0] = (i8)editor_text_dlg_wnd_block.editor_item_recs[advance].values[0];
    advances[advance].preq[1] = (i8)editor_text_dlg_wnd_block.editor_item_recs[advance].values[1];
    advances[advance].epoch = (i8)editor_text_dlg_wnd_block.editor_item_recs[advance].values[2];
    advances[advance].category = (i8)editor_text_dlg_wnd_block.editor_item_recs[advance].values[3];
    advances[advance].ai_value = (i8)editor_text_dlg_wnd_block.editor_item_recs[advance].values[4];
    advances[advance].civilized_mod = (i8)editor_text_dlg_wnd_block.editor_item_recs[advance].values[5];
  }
  return;
}

/* 0041541a auto */
void __cdecl tech_editor_update_controls(void)

{
  int ctrl_id;
  int row;
  char num_text[8];
  iptr sel;
  
  for (row = 1; row < 7; row++) {
    if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 9) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot])->get_id();
      _sprintf(num_text,((i8 *)&tech_editor_ctrl_kinds_block.tech_editor_int_fmt),
               *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58));
      ((Control *)
                 &editor_active.wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot])->set_text(
                 (const char *)num_text);
    }
    else if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 0xc) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot])->get_id();
      sel = *(iptr *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58);
      if (row < 3) {
        sel += 2;
      }
      editor_active.wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot].set_cur_sel(sel);
    }
  }
  return;
}

/* 0041557b auto */
int __cdecl tech_editor_store_controls(void)

{
  int ctrl_id;
  i32 clamped;
  int row;
  int changed_count;
  char num_text[8];
  int value;
  
  changed_count = 0;
  for (row = 1; row < 7; row++) {
    if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 9) {
      ((Control *)
                 &editor_active.wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot])->Control::get_text(
                 (char *)num_text);
      value = _atoi(num_text);
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot])->get_id();
      ctrl_id -= 0xca;
      clamped = int_clamp(value,*(i32 *)(((i8 *)&tech_editor_ctrl_kinds_block.tech_editor_field_min_table) + ctrl_id * 4),
                                 *(i32 *)(((i8 *)&tech_editor_ctrl_kinds_block.tech_editor_field_max_table) + ctrl_id * 4));
      editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[ctrl_id] = clamped;
      ctrl_id = ((Control *)
                         &editor_active.wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot])->get_id();
      if (*(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58) != value) {
        changed_count++;
      }
    }
    else if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 0xc) {
      value = editor_active.wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot].get_cur_sel();
      if (row < 3) {
        value -= 2;
      }
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot])->get_id();
      *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_active.wnd->item * 0x58 + ctrl_id * 4) = value;
    }
  }
  return changed_count;
}

/* 00415765 auto */
void __cdecl tech_editor_redraw(void)

{
  editor_active.wnd->tech_editor_paint();
  return;
}

/* 00415780 auto */
i32 __cdecl tech_editor_write_civilize(FILE *file)

{
  char *name2;
  size_t len;
  size_t name_len;
  iptr advance;
  
  for (advance = 0; advance < 100; advance++) {
    text_clear();
    text_append_string_id(advances[advance].name);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_name_sep_str));
    name2 = label_string(advances[advance].name);
    len = _strlen(name2);
    if (len < 0x13) {
      name2 = label_string(advances[advance].name);
      name_len = _strlen(name2);
    }
    else {
      name_len = 0x13;
    }
    text_append_spaces(0x13 - name_len);
    text_append_int((int)advances[advance].ai_value);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_ai_sep_str));
    text_append_int_2d((int)advances[advance].civilized_mod);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_mod_sep_str));
    text_append_advance_abbrev((int)advances[advance].preq[0],1);
    text_append_space();
    text_append_advance_abbrev((int)advances[advance].preq[1],1);
    text_append_space();
    text_append_int((int)(char)advances[advance].epoch);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_epoch_sep_str));
    text_append_int((int)(char)advances[advance].category);
    crt_strcat(((i8 *)&textbuf.buf),str_empty_10);
    text_append_advance_abbrev(advance,0);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&civilize_rules_newline_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 00415952 auto */
void __cdecl tech_editor_relink_prereqs(void)

{
  bool found;
  iptr advance2;
  iptr advance;
  iptr preq_idx;
  
  game.num_advances = 0;
  for (advance = 0; advance < 100; advance++) {
    if (advances[advance].enabled != '\0') {
      game.num_advances++;
      for (preq_idx = 0; preq_idx < 2; preq_idx++) {
        advance2 = (iptr)advances[advance].preq[preq_idx];
        found = false;
        while ((-1 < advance2 && (!found))) {
          if (advances[advance2].enabled == '\0') {
            advance2 = (iptr)advances[advance2].preq[0];
          }
          else {
            found = true;
          }
        }
        advances[advance].preq[preq_idx] = (i8)advance2;
      }
    }
  }
  return;
}

/* 00415a40 auto */
void __cdecl tech_editor_on_ok(void)

{
  iptr errors;
  HWND hWnd;
  iptr ok;
  NativeWindow *window;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  iptr parent_wnd2;
  char msg[32];
  
  errors = tech_editor_store_controls();
  if (errors == 0) {
    tech_editor_save_items();
    tech_editor_relink_prereqs();
    rules_edit_set_section((iptr)&str_civilize,(RulesSectionWriterFn)tech_editor_write_civilize);
    ok = rules_write_sections();
    if (ok == 0) {
      _sprintf(msg,str_error_updating_rules_s,((i8 *)&lang_file_ext));
      uType = 0x10;
      lpCaption = str_file_i_o_error;
      lpText = msg;
      window = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(window->hwnd,lpText,lpCaption,uType);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    tech_editor_update_controls();
    tech_editor_redraw();
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice);
    dlg_set_parent_window(0);
    hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
    SetFocus(hWnd);
  }
  return;
}

/* 00415b52 auto */
void __cdecl tech_editor_fill_usage_list(void)

{
  bool units_header;
  bool advances_header;
  iptr label;
  i32 name2;
  i32 conj;
  i32 adv_name;
  iptr advance;
  iptr unit_type;
  char line[80];  /* BF-042 (Peter Johnson's ledger): longer names are cut */
  
  advances_header = false;
  units_header = false;
  editor_active.wnd->list_box.clear();
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if ((int)unit_types[unit_type].prerequisite == editor_active.wnd->item) {
      if (!units_header) {
        label = (iptr)label_string(label_table.string_ids[0x202]);
        editor_active.wnd->list_box.add_string((const char *)label);
        units_header = true;
      }
      label = (iptr)label_string(unit_types[unit_type].name);
      editor_active.wnd->list_box.add_string((const char *)label);
    }
  }
  for (advance = 0; advance < 100; advance++) {
    if ((int)advances[advance].preq[0] == editor_active.wnd->item) {
      if (!advances_header) {
        label = (iptr)label_string(label_table.string_ids[0x203]);
        editor_active.wnd->list_box.add_string((const char *)label);
        advances_header = true;
      }
      if (advances[advance].preq[1] < '\0') {
        name2 = (iptr)label_string(advances[advance].name);
        snprintf(line,sizeof line,((i8 *)&tech_editor_usage_preq1_fmt),name2);
      }
      else {
        name2 = (iptr)label_string(advances[advances[advance].preq[1]].name);
        conj = (iptr)label_string(label_table.string_ids[0x7b]);
        adv_name = (iptr)label_string(advances[advance].name);
        snprintf(line,sizeof line,str_s_s_s,adv_name,conj,name2);
      }
      editor_active.wnd->list_box.add_string((const char *)line);
    }
    else if ((int)advances[advance].preq[1] == editor_active.wnd->item) {
      if (!advances_header) {
        label = (iptr)label_string(label_table.string_ids[0x203]);
        editor_active.wnd->list_box.add_string((const char *)label);
        advances_header = true;
      }
      if (advances[advance].preq[0] < '\0') {
        name2 = (iptr)label_string(advances[advance].name);
        snprintf(line,sizeof line,((i8 *)&tech_editor_usage_preq2_fmt),name2);
      }
      else {
        name2 = (iptr)label_string(advances[advances[advance].preq[0]].name);
        conj = (iptr)label_string(label_table.string_ids[0x7b]);
        adv_name = (iptr)label_string(advances[advance].name);
        snprintf(line,sizeof line,str_s_s_s_2,adv_name,conj,name2);
      }
      editor_active.wnd->list_box.add_string((const char *)line);
    }
  }
  return;
}

/* 00415e53 auto */
void __cdecl tech_editor_on_rename(void)

{
  int item;
  size_t len;
  iptr label;
  iptr parent_wnd2;
  int i;
  char old_name[20];
  char new_name[260];
  iptr sel3;
  iptr sel2;
  iptr sel1;
  int result;
  
  item = editor_active.wnd->item;
  _strncpy(old_name,editor_text_dlg_wnd_block.editor_item_name_recs[item],0x14);
  old_name[19] = 0;
  do {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_push_parent_window(parent_wnd2);
    result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_techname,0x13,(iptr)old_name,(iptr)new_name);
    dlg_pop_parent_window();
    if (result == -1) break;
    len = _strlen(new_name);
  } while (len == 0);
  if (-1 < result) {
    crt_strcpy(editor_text_dlg_wnd_block.editor_item_name_recs[item],new_name);
    sel1 = editor_active.wnd->combos[0].get_cur_sel();
    sel2 = editor_active.wnd->combos[1].get_cur_sel();
    sel3 = editor_active.wnd->combos[2].get_cur_sel();
    editor_active.wnd->combos[0].clear();
    editor_active.wnd->combos[1].clear();
    editor_active.wnd->combos[2].clear();
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    editor_active.wnd->combos[1].add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    editor_active.wnd->combos[2].add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f1]);
    editor_active.wnd->combos[1].add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f1]);
    editor_active.wnd->combos[2].add_string((const char *)label);
    for (i = 0; i < 100; i++) {
      editor_active.wnd->combos[0].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[i]);
      editor_active.wnd->combos[1].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[i]);
      editor_active.wnd->combos[2].add_string((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[i]);
    }
    editor_active.wnd->combos[0].set_cur_sel(sel1);
    editor_active.wnd->combos[1].set_cur_sel(sel2);
    editor_active.wnd->combos[2].set_cur_sel(sel3);
    tech_editor_fill_usage_list();
    tech_editor_redraw();
  }
  return;
}

/* 0041612e auto */
void __cdecl tech_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help,(const char *)&str_advances);
  dlg_set_parent_window(0);
  return;
}

/* 0041618d auto */
void __cdecl tech_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 004161b5 auto */
void __cdecl tech_editor_on_edit_icon(void)

{
  i32 title;
  i32 flags;
  SpriteSheetSaveFn save_fn;
  
  save_fn = sprite_sheet_save_tech_icons;
  flags = 0;
  title = (iptr)label_string(label_table.string_ids[0x1f6]);
  sprite_editor_run((iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                           editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[3] * 0xf0 +
                           editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[2] * 0x3c),
                     (const char *)title,flags,save_fn);
  tech_editor_redraw();
  return;
}

/* 0041623d auto */
void __cdecl tech_editor_on_combo(int ctrl_id)

{
  int errors;
  HWND hWnd;
  i32 sel;
  i32 parent_wnd2;
  
  if (ctrl_id == 0xc9) {
    errors = tech_editor_store_controls();
    if (errors == 0) {
      sel = editor_active.wnd->combos[0].get_cur_sel();
      editor_active.wnd->item = sel;
      tech_editor_update_controls();
      tech_editor_fill_usage_list();
      tech_editor_redraw();
    }
    else {
      editor_active.wnd->combos[0].set_cur_sel(editor_active.wnd->item);
      tech_editor_update_controls();
      tech_editor_redraw();
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_debug_11,(const char *)&str_notice_2);
      dlg_set_parent_window(0);
      hWnd = ((Control *)&editor_active.wnd->combos[0])->get_hwnd();
      SetFocus(hWnd);
    }
  }
  else if ((ctrl_id == 0xcc) || (ctrl_id == 0xcd)) {
    tech_editor_store_controls();
    tech_editor_fill_usage_list();
    tech_editor_redraw();
  }
  return;
}

/* 00416354 auto */
void EditorWindow::tech_editor_create_combo(int slot)

{
  int ctrl_id;
  iptr label;
  u8 *parent_wnd2;
  iptr advance;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,tech_editor_combo_x_table_block.tech_editor_combo_pos[slot].x + client_x - 0x1e,
                     tech_editor_combo_x_table_block.tech_editor_combo_pos[slot].y + client_y,0xa0,
                     font_height_uptr << 3);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + slot * 0x38 + 0x28))->create((iptr)parent_wnd2,ctrl_id,
             (iptr)rect);
  ((ComboBox *)(text_font + slot * 0x38 + 0x28))->set_font((iptr)&editor_font_block.editor_font);
  ((ComboBox *)(text_font + slot * 0x38 + 0x28))->set_callback(tech_editor_on_combo);
  switch(slot) {
  case 0:
    for (advance = 0; advance < 100; advance++) {
      label = (iptr)label_string(advances[advance].name);
      ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 1:
  case 2:
    label = (iptr)label_string(label_table.string_ids[0x1f0]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x1f1]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    for (advance = 0; advance < 100; advance++) {
      label = (iptr)label_string(advances[advance].name);
      ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    }
    break;
  case 3:
    label = (iptr)label_string(label_table.string_ids[0x204]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x205]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x206]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x207]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    break;
  case 4:
    label = (iptr)label_string(label_table.string_ids[0x208]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x209]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x20a]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x20b]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)label_string(label_table.string_ids[0x20c]);
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
  }
  return;
}

/* 00416734 auto */
void EditorWindow::tech_editor_create_edit(int slot)

{
  int ctrl_id;
  u8 *parent_wnd2;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,(((i32 *)&tech_editor_edit_x_table_block.tech_editor_edit_x_table))[slot * 2] + client_x,
                     (((i32 *)&tech_editor_edit_x_table_block.tech_editor_edit_y_table))[slot * 2] + client_y,0x30,font_height_uptr + 6);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((EditControl *)(numeric_edit_controls_area + slot * 0x48 - 0x2c))->create((iptr)parent_wnd2,ctrl_id,
             (iptr)rect,(const char *)&str_empty_11);
  ((EditControl *)(numeric_edit_controls_area + slot * 0x48 - 0x2c))->set_max_len(4);
  ((EditControl *)(numeric_edit_controls_area + slot * 0x48 - 0x2c))->set_callback(editor_numeric_edit_filter);
  return;
}

/* 00416828 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void EditorWindow::tech_editor_paint()

{
  byte key_color;
  i8 sprite_rect[16];
  Sprite *icon;
  int label_y;
  int label_x;
  int icon_y;
  int icon_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((*(int *)&editor_ui.bg_surface),(iptr)this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  icon_x = client_x + 0x20;
  icon_y = client_y + 0x20;
  icon = (Sprite *)
             (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
             editor_text_dlg_wnd_block.editor_item_recs[item_uptr].values[3] * 0xf0 +
             editor_text_dlg_wnd_block.editor_item_recs[item_uptr].values[2] * 0x3c);
  key_color = icon->get_key_color();
  gfx_fill_rect_xywh((iptr)this,icon_x,icon_y,0x48,0x28,(uint)key_color);
  gfx_set_scale_ratio(2,1);
  icon->draw((iptr)sprite_rect,(iptr)this,icon_x,icon_y);
  gfx_set_scale_ratio(1,1);
  gfx_draw_tiled_frame((iptr)this,icon_x,icon_y,0x48,0x28,6);
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  label_x = tech_editor_combo_x_table_block.tech_editor_prereq1_ctrl_x + client_x + 0x32;
  label_y = ((tech_editor_combo_x_table_block.tech_editor_prereq1_ctrl_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tech_editor_prereq1_suffix_str));
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tech_editor_combo_x_table_block.tech_editor_prereq2_ctrl_x + client_x + 0x32;
  label_y = ((tech_editor_combo_x_table_block.tech_editor_prereq2_ctrl_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tech_editor_prereq2_suffix_str));
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tech_editor_combo_x_table_block.tech_editor_epoch_ctrl_x + client_x + 0x32;
  label_y = ((tech_editor_combo_x_table_block.tech_editor_epoch_ctrl_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1eb);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tech_editor_combo_x_table_block.tech_editor_category_ctrl_x + client_x + 0x32;
  label_y = ((client_y + tech_editor_combo_x_table_block.tech_editor_category_ctrl_y_i32) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1ec);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tech_editor_edit_x_table_block.tech_editor_edit_x_table[0] + client_x + 0x18;
  label_y = ((tech_editor_edit_x_table_block.tech_editor_edit_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1ed);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tech_editor_edit_x_table_block.tech_editor_modifier_edit_x + client_x + 0x18;
  label_y = ((tech_editor_edit_x_table_block.tech_editor_modifier_edit_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1ee);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = client_x + (int)wnd_width_u / 2;
  label_y = (client_y - font_height_uptr) + 0xe6;
  text_clear();
  text_append_label(0x1ea);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  wnd_blit_surface();
  return;
}

/* 00416c9e auto */
void EditorWindow::tech_editor_dialog_run()

{
  DibSurface *this_00;
  uptr font_height;
  i32 title;
  int pos;
  int btn_y;
  iptr label;
  i32 style;
  i32 left;
  i32 top;
  u32 width;
  u32 height;
  i32 extra_arg1;
  i32 extra_arg2;
  i32 extra_arg3;
  u8 *icon_parent;
  u8 *cancel_parent;
  u8 *help_parent;
  u8 *rename_parent;
  u8 *ok_parent;
  u8 *list_parent;
  DibSurface *surface;
  int row;
  i8 rect[16];
  GamePalette palette;
  i32 eh_next;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    surface = (DibSurface *)0x0;
  }
  else {
    surface = surface_ctor(this_00);
  }
  editor_ui.bg_surface = surface;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  wnd_width_u = 0x230;
  wnd_height_u = 0x17c;
  item_uptr = 0;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editorpt_gif,10,0xc0,(iptr)&palette);
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  extra_arg3 = 0;
  extra_arg2 = 0;
  extra_arg1 = 0;
  height = wnd_height_u;
  width = wnd_width_u;
  top = 0;
  left = 0;
  style = 0xd;
  title = (iptr)label_string(label_table.string_ids[0x1cd]);
  this->FrameWindow::create((const char *)title,style,left,top,width,height,extra_arg1,extra_arg2,extra_arg3);
  for (row = 0; row < 7; row++) {
    if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 9) {
      tech_editor_create_edit(tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot);
    }
    else if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 0xc) {
      tech_editor_create_combo(tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot);
    }
  }
  rect_set_xywh((iptr)rect,client_x + 0x32,client_y + 0xe7,0x1cb,0x6e);
  pos = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    list_parent = (u8 *)0x0;
  }
  else {
    list_parent = (u8 *)&window;
  }
  list_box.create((iptr)list_parent,pos,(iptr)rect);
  list_box.set_font((iptr)&editor_font_block.editor_font);
  pos = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = pos + 8;
  pos = client_width - 10;
  button_width_u = (int)(pos + (pos >> 0x1f & 3U)) >> 2;
  btn_y = (client_y + client_height) - (button_height_u + 2);
  pos = client_x + 2;
  rect_set_xywh((iptr)rect,pos,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)ok_parent,0x65,(iptr)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)tech_editor_on_ok);
  pos = pos + button_width_u + 2;
  rect_set_xywh((iptr)rect,pos,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    rename_parent = (u8 *)0x0;
  }
  else {
    rename_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x2a]);
  rename_button.create((iptr)rename_parent,0x65,(iptr)rect,(const char *)label);
  rename_button.set_callback((CtrlNotifyFn)tech_editor_on_rename);
  pos = pos + button_width_u + 2;
  rect_set_xywh((iptr)rect,pos,btn_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((iptr)help_parent,0x66,(iptr)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)tech_editor_on_help);
  rect_set_xywh((iptr)rect,pos + button_width_u + 2,btn_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)cancel_parent,0x66,(iptr)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)tech_editor_on_cancel);
  cancel_button.set_cancel_button();
  button_width_u = 0x55;
  pos = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = pos + 8;
  rect_set_xywh((iptr)rect,client_x + 0x19,client_y + 0x50,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    icon_parent = (u8 *)0x0;
  }
  else {
    icon_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x1f3]);
  icon_button.create((iptr)icon_parent,0x65,(iptr)rect,(const char *)label);
  icon_button.set_callback((CtrlNotifyFn)tech_editor_on_edit_icon);
  window.set_keyboard_nav(0);
  tech_editor_load_items();
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  tech_editor_update_controls();
  combo_count_u = 5;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(tech_editor_redraw);
  tech_editor_on_combo(0xc9);
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

/* 00417566 auto */
void __cdecl tech_editor_run(void)

{
  EditorWindow editor_wnd;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.tech_editor_dialog_run();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 00419cbb auto */
i8 __cdecl rules_next_line_byte_clamped(i32 min_value,i32 max_value)

{
  iptr raw_value;
  i32 clamped;
  
  text_read_line(1);
  raw_value = text_next_int();
  clamped = int_clamp(raw_value,min_value,max_value);
  return (char)clamped;
}

/* 00419cf4 auto */
i8 __cdecl rules_next_byte_clamped(i32 min_value,i32 max_value)

{
  iptr raw_value;
  i32 clamped;
  
  raw_value = text_next_int();
  clamped = int_clamp(raw_value,min_value,max_value);
  return (char)clamped;
}

/* 00419d23 auto */
void __cdecl rules_load_cosmic(void)

{
  if (rules_from_scenario() == 0) {
    /* (port) the game's rules (rules.h) */
    text_close_file();
    memcpy(&cosmic,rules_cosmic,RULES_COSMIC_SIZE);
    return;
  }
  text_open_section((const char *)&main_frame_wnd_block.rules_txt_path,(const char *)&str_cosmic);
  cosmic.road_move_mult = rules_next_line_byte_clamped(1,10);
  cosmic.trireme_loss_chance = rules_next_line_byte_clamped(1,100);
  cosmic.food_per_citizen = rules_next_line_byte_clamped(0,10);
  cosmic.food_box_rows = rules_next_line_byte_clamped(4,0x14);
  if ((cosmic.food_box_rows & 1) != 0) {
    cosmic.food_box_rows++;
  }
  cosmic.shield_box_rows = rules_next_line_byte_clamped(4,0x14);
  cosmic.settler_food_low = rules_next_line_byte_clamped(0,10);
  cosmic.settler_food_high = rules_next_line_byte_clamped(0,10);
  cosmic.unhappy_size = rules_next_line_byte_clamped(4,0xc);
  cosmic.riot_factor = rules_next_line_byte_clamped(10,100);
  cosmic.aqueduct_size = rules_next_line_byte_clamped(4,0x32);
  cosmic.sewer_size = rules_next_line_byte_clamped(4,0x32);
  cosmic.tech_paradigm = rules_next_line_byte_clamped(3,10);
  cosmic.transform_time = rules_next_line_byte_clamped(5,100);
  cosmic.monarchy_free_support = rules_next_line_byte_clamped(0,8);
  cosmic.communism_free_support = rules_next_line_byte_clamped(0,8);
  cosmic.fundamentalism_free_support = rules_next_line_byte_clamped(0,8);
  cosmic.communism_palace_dist = rules_next_line_byte_clamped(1,0x14);
  cosmic.fund_science_loss = rules_next_line_byte_clamped(0,100);
  cosmic.change_penalty = rules_next_line_byte_clamped(0,100);
  cosmic.paradrop_range = rules_next_line_byte_clamped(4,100);
  cosmic.mass_thrust_paradigm = rules_next_line_byte_clamped(0x19,200);
  cosmic.fund_max_science = rules_next_line_byte_clamped(0,10);
  return;
}

/* 0041a046 auto */
void __cdecl rules_load_advances(int keep_names)

{
  bool found_enabled;
  i8 preq_advance;
  size_t line_len;
  i32 value;
  char *token;
  i32 name_id;
  iptr advance2;
  iptr advance;
  iptr preq_index;
  int from_file;
  char key[32];
  
  /* (port) the game's rules (rules.h), or the scenario's RULES.TXT */
  from_file = rules_from_scenario();
  if (from_file != 0) {
    text_open_section((const char *)&main_frame_wnd_block.rules_txt_path,(const char *)&str_civilize_2);
  }
  else {
    text_close_file();
  }
  game.num_advances = 0;
  for (advance = 0; advance < 100; advance++) {
    advances[advance].preq[1] = -2;
    advances[advance].preq[0] = advances[advance].preq[1];
    if (advances[advance].preq[0] == -2) {
      advances[advance].enabled = '\0';
    }
    else {
      advances[advance].enabled = 1;
    }
  }
  for (advance = 0; advance < 100; advance++) {
    if (from_file == 0) {
      if (rules_advance_count <= advance) break;
      if (keep_names == 0) {
        snprintf(key,sizeof key,"rules.advance.%d",advance);
        advances[advance].name = rules_name(key,0x14);
      }
      advances[advance].ai_value = rules_advances[advance].ai_value;
      advances[advance].civilized_mod = rules_advances[advance].civilized_mod;
      advances[advance].preq[0] = rules_advances[advance].preq[0];
      advances[advance].preq[1] = rules_advances[advance].preq[1];
      advances[advance].epoch = rules_advances[advance].epoch;
      advances[advance].category = rules_advances[advance].category;
      advances[advance].enabled = advances[advance].preq[0] != -2;
      continue;
    }
    text_read_line(1);
    line_len = _strlen(((i8 *)&textbuf.buf));
    if ((line_len == 0) || (textbuf.buf[0] == ';')) break;
    if (keep_names == 0) {
      name_id = text_next_token_str_id(0x14);
      advances[advance].name = name_id;
    }
    else {
      text_next_token();
    }
    value = text_next_int();
    advances[advance].ai_value = (i8)value;
    value = text_next_int();
    advances[advance].civilized_mod = (i8)value;
    for (preq_index = 0; preq_index < 2; preq_index++) {
      token = text_next_token();
      preq_advance = rules_parse_advance_abbrev(token);
      advances[advance].preq[preq_index] = preq_advance;
      if (advances[advance].preq[preq_index] == -3) {
        error_report(0xfffffffc,3,(iptr)&textbuf.buf,advance,preq_index);
      }
    }
    value = text_next_int();
    advances[advance].epoch = (u8)value;
    value = text_next_int();
    advances[advance].category = (u8)value;
    /* BF-129 (Peter Johnson's ledger): an epoch past the four or a category
       past the five is a bad line, as a bad prerequisite (the original took
       them, and the Civilopedia drew sprites past its 20) */
    if ((3 < advances[advance].epoch) || (4 < advances[advance].category)) {
      error_report(0xfffffffc,3,(iptr)&textbuf.buf,advance,2);
    }
    if (advances[advance].preq[0] == -2) {
      advances[advance].enabled = '\0';
    }
    else {
      advances[advance].enabled = 1;
    }
  }
  for (; advance < 100; advance++) {
    if (keep_names == 0) {
      _sprintf(((i8 *)&text_parse.token_buf),str_advance_d,advance + 1);
      name_id = strpool_add_padded((const char *)&text_parse.token_buf,0x14);
      advances[advance].name = name_id;
    }
    advances[advance].ai_value = 3;
    advances[advance].civilized_mod = '\0';
    for (preq_index = 0; preq_index < 2; preq_index++) {
      advances[advance].preq[preq_index] = 0xfe;
    }
    advances[advance].epoch = '\0';
    advances[advance].category = '\0';
    if (advances[advance].preq[0] == -2) {
      advances[advance].enabled = '\0';
    }
    else {
      advances[advance].enabled = 1;
    }
  }
  for (advance = 0; advance < 100; advance++) {
    if (advances[advance].enabled != '\0') {
      game.num_advances++;
      for (preq_index = 0; preq_index < 2; preq_index++) {
        advance2 = (iptr)advances[advance].preq[preq_index];
        found_enabled = false;
        while ((-1 < advance2 && (!found_enabled))) {
          if (advances[advance2].enabled == '\0') {
            advance2 = (iptr)advances[advance2].preq[0];
          }
          else {
            found_enabled = true;
          }
        }
        advances[advance].preq[preq_index] = (i8)advance2;
      }
    }
  }
  return;
}

/* 0041a422 auto */
void __cdecl rules_load_improvements(int keep_names)

{
  i8 preq_advance;
  i32 name_id;
  i32 value;
  char *token;
  iptr improvement;
  int from_file;
  char key[32];
  
  /* (port) the game's rules (rules.h), or the scenario's RULES.TXT */
  from_file = rules_from_scenario();
  if (from_file == 0) {
    text_close_file();
    for (improvement = 0; improvement < 0x43; improvement++) {
      if (keep_names == 0) {
        snprintf(key,sizeof key,"rules.improvement.%d",improvement);
        improvements[improvement].name = rules_name(key,0x19);
      }
      improvements[improvement].cost = rules_improvements[improvement].cost;
      improvements[improvement].upkeep = rules_improvements[improvement].upkeep;
      improvements[improvement].preq = rules_improvements[improvement].preq;
      while ((-1 < improvements[improvement].preq &&
             (advances[improvements[improvement].preq].enabled == '\0'))) {
        improvements[improvement].preq = advances[improvements[improvement].preq].preq[0];
      }
    }
    for (improvement = 0; improvement < 0x1c; improvement++) {
      wonder_expires[improvement] = rules_wonder_expires[improvement];
    }
    return;
  }
  text_open_section((const char *)&main_frame_wnd_block.rules_txt_path,(const char *)&str_improve);
  for (improvement = 0; improvement < 0x43; improvement++) {
    text_read_line(1);
    if (keep_names == 0) {
      name_id = text_next_token_str_id(0x19);
      improvements[improvement].name = name_id;
    }
    else {
      text_next_token();
    }
    value = text_next_int();
    improvements[improvement].cost = (u8)value;
    value = text_next_int();
    improvements[improvement].upkeep = (u8)value;
    token = text_next_token();
    preq_advance = rules_parse_advance_abbrev(token);
    improvements[improvement].preq = preq_advance;
    if (improvements[improvement].preq == -3) {
      error_report(0xfffffffb,3,(iptr)&textbuf.buf,improvement,0);
    }
    while ((-1 < improvements[improvement].preq &&
           (advances[improvements[improvement].preq].enabled == '\0'))) {
      improvements[improvement].preq = advances[improvements[improvement].preq].preq[0];
    }
  }
  text_open_section(0,(const char *)&str_endwonder);
  for (improvement = 0; improvement < 0x1c; improvement++) {
    text_read_line(1);
    token = text_next_token();
    preq_advance = rules_parse_advance_abbrev(token);
    wonder_expires[improvement] = preq_advance;
    if (wonder_expires[improvement] == -3) {
      error_report(0xfffffffc,3,(iptr)&textbuf.buf,improvement,0);
    }
  }
  return;
}

/* 0041a5c4 auto */
void __cdecl rules_load_units(int keep_names)

{
  i8 advance_id;
  u8 cost;
  ushort flag_bits;
  size_t line_len;
  i32 value;
  char *token;
  i32 name_id;
  iptr unit_type;
  int from_file;
  char key[32];
  const RulesUnit *row;
  
  double_moves_applied = 0;
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    unit_types[unit_type].prerequisite = -2;
  }
  /* (port) the game's rules (rules.h), or the scenario's RULES.TXT */
  from_file = rules_from_scenario();
  if (from_file != 0) {
    text_open_section(0,(const char *)&str_units);
  }
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if (from_file == 0) {
      if (rules_unit_count <= unit_type) break;
      row = &rules_units[unit_type];
      if (keep_names == 0) {
        snprintf(key,sizeof key,"rules.unit.%d",unit_type);
        unit_types[unit_type].name = rules_name(key,0xf);
        prefs_main_menu_choice_block.unit_type_name_ptrs[unit_type] = minimap_window_block.strpool_last_string;
      }
      unit_types[unit_type].obsolete_by = row->obsolete_by;
      unit_types[unit_type].domain = row->domain;
      unit_types[unit_type].move = (char)row->move * cosmic.road_move_mult;
      unit_types[unit_type].range = row->range;
      unit_types[unit_type].attack = row->attack;
      unit_types[unit_type].defense = row->defense;
      unit_types[unit_type].hit_points = (char)row->hit_points * '\n';
      unit_types[unit_type].firepower = row->firepower;
      unit_types[unit_type].cost = row->cost;
      unit_types[unit_type].hold = row->hold;
      unit_types[unit_type].role = row->role;
      unit_types[unit_type].prerequisite = row->prerequisite;
      unit_types[unit_type].flags = (uint)row->flags;
      continue;
    }
    text_read_line(1);
    line_len = _strlen(((i8 *)&textbuf.buf));
    if ((line_len == 0) || (textbuf.buf[0] == ';')) break;
    if (keep_names == 0) {
      name_id = text_next_token_str_id(0xf);
      unit_types[unit_type].name = name_id;
      prefs_main_menu_choice_block.unit_type_name_ptrs[unit_type] = minimap_window_block.strpool_last_string;
    }
    else {
      text_next_token();
    }
    token = text_next_token();
    advance_id = rules_parse_advance_abbrev(token);
    unit_types[unit_type].obsolete_by = advance_id;
    value = text_next_int();
    unit_types[unit_type].domain = (u8)value;
    value = text_next_int();
    unit_types[unit_type].move = (char)value * cosmic.road_move_mult;
    value = text_next_int();
    unit_types[unit_type].range = (u8)value;
    value = text_next_int();
    unit_types[unit_type].attack = (u8)value;
    value = text_next_int();
    unit_types[unit_type].defense = (u8)value;
    value = text_next_int();
    unit_types[unit_type].hit_points = (char)value * '\n';
    value = text_next_int();
    unit_types[unit_type].firepower = (u8)value;
    cost = rules_next_byte_clamped(1,200);
    unit_types[unit_type].cost = cost;
    value = text_next_int();
    unit_types[unit_type].hold = (u8)value;
    value = text_next_int();
    unit_types[unit_type].role = (u8)value;
    token = text_next_token();
    advance_id = rules_parse_advance_abbrev(token);
    unit_types[unit_type].prerequisite = advance_id;
    flag_bits = text_next_binary();
    unit_types[unit_type].flags = (uint)flag_bits;
    if ((unit_types[unit_type].obsolete_by == -3) || (unit_types[unit_type].prerequisite == -3)) {
      error_report(0xfffffffa,3,(iptr)&textbuf.buf,unit_type,0);
    }
  }
  for (; unit_type < 0x3e; unit_type++) {
    if (keep_names == 0) {
      _sprintf(((i8 *)&text_parse.token_buf),str_unit_d,unit_type + 1);
      name_id = strpool_add_padded((const char *)&text_parse.token_buf,0xf);
      unit_types[unit_type].name = name_id;
    }
    unit_types[unit_type].obsolete_by = -2;
    unit_types[unit_type].domain = DOMAIN_LAND;
    unit_types[unit_type].move = '\0';
    unit_types[unit_type].range = '\0';
    unit_types[unit_type].attack = 1;
    unit_types[unit_type].defense = '\0';
    unit_types[unit_type].hit_points = '\0';
    unit_types[unit_type].firepower = '\0';
    unit_types[unit_type].cost = 1;
    unit_types[unit_type].hold = '\0';
    unit_types[unit_type].role = ROLE_ATTACK;
    unit_types[unit_type].prerequisite = -2;
    unit_types[unit_type].flags = 0;
    if ((unit_types[unit_type].obsolete_by == -3) || (unit_types[unit_type].prerequisite == -3)) {
      error_report(0xfffffffa,3,(iptr)&textbuf.buf,unit_type,0);
    }
  }
  return;
}

/* 0041a95f auto */
void __cdecl rules_load_terrain(void)

{
  i8 result_terrain;
  i8 transform_terrain;
  i32 name_id;
  i32 value;
  char *token;
  iptr terrain2;
  iptr slot;
  char key[32];
  
  if (rules_from_scenario() == 0) {
    /* (port) the game's rules (rules.h) */
    for (terrain2 = 0; terrain2 < 0x21; terrain2++) {
      snprintf(key,sizeof key,"rules.terrain.%d",terrain2);
      terrain_types[terrain2].name = rules_name(key,0xf);
      terrain_types[terrain2].move_cost = rules_terrain[terrain2].move_cost;
      terrain_types[terrain2].defense = rules_terrain[terrain2].defense;
      for (slot = 0; slot < 3; slot++) {
        terrain_types[terrain2].yields[slot] = rules_terrain[terrain2].yields[slot];
      }
      if (terrain2 < 0xb) {
        for (slot = 0; slot < 2; slot++) {
          terrain_types[terrain2].improve_to[slot] = rules_terrain[terrain2].improve_to[slot];
          terrain_types[terrain2].improve_bonus[slot] = rules_terrain[terrain2].improve_bonus[slot];
          terrain_types[terrain2].improve_turns[slot] = rules_terrain[terrain2].improve_turns[slot];
          terrain_types[terrain2].improve_ai_govt[slot] = rules_terrain[terrain2].improve_ai_govt[slot];
        }
        terrain_types[terrain2].transform_to = rules_terrain[terrain2].transform_to;
      }
    }
    return;
  }
  text_open_section(0,(const char *)&str_terrain);
  for (terrain2 = 0; terrain2 < 0x21; terrain2++) {
    text_read_line(1);
    name_id = text_next_token_str_id(0xf);
    terrain_types[terrain2].name = name_id;
    value = text_next_int();
    terrain_types[terrain2].move_cost = (u8)value;
    value = text_next_int();
    terrain_types[terrain2].defense = (u8)value;
    for (slot = 0; slot < 3; slot++) {
      value = text_next_int();
      terrain_types[terrain2].yields[slot] = (char)value;
    }
    if (terrain2 < 0xb) {
      for (slot = 0; slot < 2; slot++) {
        token = text_next_token();
        result_terrain = rules_parse_terrain_abbrev(token);
        terrain_types[terrain2].improve_to[slot] = result_terrain;
        value = text_next_int();
        terrain_types[terrain2].improve_bonus[slot] = (char)value;
        value = text_next_int();
        terrain_types[terrain2].improve_turns[slot] = (char)value;
        value = text_next_int();
        terrain_types[terrain2].improve_ai_govt[slot] = (char)value;
        if (terrain_types[terrain2].improve_to[slot] == -3) {
          error_report(0xfffffff9,3,(iptr)&textbuf.buf,terrain2,slot);
        }
      }
      token = text_next_token();
      transform_terrain = rules_parse_terrain_abbrev(token);
      terrain_types[terrain2].transform_to = transform_terrain;
      if (terrain_types[terrain2].transform_to == -3) {
        error_report(0xfffffff9,3,(iptr)&textbuf.buf,terrain2,99);
      }
    }
  }
  return;
}

/* (port) rules_load_govts_leaders's reading of one leader, from the game's
   rules (rules.h) in place of a line of @LEADERS: the same values set, the
   same names added to the string pool, in the same order. */
static void rules_load_leader_from_table(iptr leader,int load_civ_data)
{
  const RulesLeader *row = &rules_leaders[leader];
  i16 old_value;
  i16 old_value2;
  i16 old_male_title;
  i16 old_female_title;
  u8 old_female;
  i8 old_trait;
  i32 value;
  iptr govt;
  iptr gender;
  int i;
  int title_govt;
  char key[48];
  
  old_value = leaders[leader].names[0];
  old_value2 = leaders[leader].names[1];
  snprintf(key,sizeof key,"rules.leader.%d.male",leader);
  leaders[leader].names[0] = (i16)rules_name(key,0x18);
  snprintf(key,sizeof key,"rules.leader.%d.female",leader);
  leaders[leader].names[1] = (i16)rules_name(key,0x18);
  if (load_civ_data == 0) {
    leaders[leader].names[0] = old_value;
    leaders[leader].names[1] = old_value2;
  }
  old_female = leaders[leader].female;
  leaders[leader].female = row->female;
  if (load_civ_data == 0) {
    leaders[leader].female = old_female;
  }
  else {
    leaders[leader].leader_name = leaders[leader].names[((uint)leaders[leader].female)];
  }
  old_value = leaders[leader].color;
  leaders[leader].color = row->color;
  if (load_civ_data == 0) {
    leaders[leader].color = old_value;
  }
  old_value = leaders[leader].city_style;
  leaders[leader].city_style = row->city_style;
  if (load_civ_data == 0) {
    leaders[leader].city_style = old_value;
  }
  old_value = leaders[leader].plural;
  old_value2 = leaders[leader].adjective;
  snprintf(key,sizeof key,"rules.leader.%d.plural",leader);
  leaders[leader].plural = (i16)rules_name(key,0x18);
  snprintf(key,sizeof key,"rules.leader.%d.adjective",leader);
  leaders[leader].adjective = (i16)rules_name(key,0x18);
  if (load_civ_data == 0) {
    leaders[leader].plural = old_value;
    leaders[leader].adjective = old_value2;
  }
  old_trait = leaders[leader].attack;
  leaders[leader].attack = row->attack;
  if (load_civ_data == 0) {
    leaders[leader].attack = old_trait;
  }
  old_trait = leaders[leader].expand;
  leaders[leader].expand = row->expand;
  if (load_civ_data == 0) {
    leaders[leader].expand = old_trait;
  }
  old_trait = leaders[leader].civilize;
  leaders[leader].civilize = row->civilize;
  if (load_civ_data == 0) {
    leaders[leader].civilize = old_trait;
  }
  for (govt = 0; govt < 7; govt++) {
    for (gender = 0; gender < 2; gender++) {
      if (load_civ_data != 0) {
        leaders[leader].titles[govt][gender] = (short)(&government_titles[govt].male)[gender];
      }
    }
  }
  for (i = 0; i < row->title_count; i++) {
    title_govt = row->title_govts[i];
    old_male_title = leaders[leader].titles[title_govt][0];
    old_female_title = leaders[leader].titles[title_govt][1];
    snprintf(key,sizeof key,"rules.leader.%d.title.%d.male",leader,title_govt);
    value = rules_name_pooled(key);
    leaders[leader].titles[title_govt][0] = (short)value;
    snprintf(key,sizeof key,"rules.leader.%d.title.%d.female",leader,title_govt);
    value = rules_name_pooled(key);
    leaders[leader].titles[title_govt][1] = (short)value;
    if (load_civ_data == 0) {
      leaders[leader].titles[title_govt][0] = old_male_title;
      leaders[leader].titles[title_govt][1] = old_female_title;
    }
  }
}

/* 0041ab18 auto */
void __cdecl rules_load_govts_leaders(int load_civ_data)

{
  u8 old_female;
  i8 old_trait;
  i16 old_value;
  i16 old_value2;
  i16 old_male_title;
  i16 old_female_title;
  i32 name_id;
  i32 value;
  char *token;
  iptr raw_value;
  int title_govt;
  i32 max_value;
  iptr gender;
  iptr govt;
  iptr leader;
  int from_file;
  int title;
  char key[48];
  
  /* (port) the game's rules (rules.h), or the scenario's RULES.TXT */
  from_file = rules_from_scenario();
  if (from_file != 0) {
    text_open_section(0,(const char *)&str_governments);
  }
  for (leader = 0; leader < 7; leader++) {
    if (from_file == 0) {
      snprintf(key,sizeof key,"rules.government.%d",leader);
      government_names[leader] = rules_name(key,0xf);
      snprintf(key,sizeof key,"rules.government.%d.male",leader);
      government_titles[leader].male = rules_name(key,0xf);
      snprintf(key,sizeof key,"rules.government.%d.female",leader);
      government_titles[leader].female = rules_name(key,0xf);
      continue;
    }
    text_read_line(1);
    name_id = text_next_token_str_id(0xf);
    government_names[leader] = name_id;
    name_id = text_next_token_str_id(0xf);
    government_titles[leader].male = name_id;
    name_id = text_next_token_str_id(0xf);
    government_titles[leader].female = name_id;
  }
  if (from_file == 0) {
    for (leader = 0; leader < 0x15; leader++) {
      rules_load_leader_from_table(leader,load_civ_data);
    }
    return;
  }
  text_open_section(0,(const char *)&str_leaders);
  for (leader = 0; leader < 0x15; leader++) {
    text_read_line(1);
    old_value = leaders[leader].names[0];
    old_value2 = leaders[leader].names[1];
    value = text_next_token_str_id(0x18);
    leaders[leader].names[0] = (i16)value;
    value = text_next_token_str_id(0x18);
    leaders[leader].names[1] = (i16)value;
    if (load_civ_data == 0) {
      leaders[leader].names[0] = old_value;
      leaders[leader].names[1] = old_value2;
    }
    old_female = leaders[leader].female;
    max_value = 1;
    value = 0;
    raw_value = text_next_int();
    value = int_clamp(raw_value,value,max_value);
    leaders[leader].female = (u8)value;
    if (load_civ_data == 0) {
      leaders[leader].female = old_female;
    }
    else {
      leaders[leader].leader_name =
           leaders[leader].names[((uint)leaders[leader].female)];
    }
    old_value = leaders[leader].color;
    value = text_next_int();
    leaders[leader].color = (i16)value;
    if (load_civ_data == 0) {
      leaders[leader].color = old_value;
    }
    else {
      value = int_clamp((int)leaders[leader].color,1,7);
      leaders[leader].color = (i16)value;
    }
    old_value = leaders[leader].city_style;
    value = text_next_int();
    leaders[leader].city_style = (i16)value;
    if (load_civ_data == 0) {
      leaders[leader].city_style = old_value;
    }
    else {
      value = int_clamp((int)leaders[leader].city_style,0,3);
      leaders[leader].city_style = (i16)value;
    }
    old_value = leaders[leader].plural;
    old_value2 = leaders[leader].adjective;
    value = text_next_token_str_id(0x18);
    leaders[leader].plural = (i16)value;
    value = text_next_token_str_id(0x18);
    leaders[leader].adjective = (i16)value;
    if (load_civ_data == 0) {
      leaders[leader].plural = old_value;
      leaders[leader].adjective = old_value2;
    }
    old_trait = leaders[leader].attack;
    value = text_next_int();
    leaders[leader].attack = (i8)value;
    if (load_civ_data == 0) {
      leaders[leader].attack = old_trait;
    }
    old_trait = leaders[leader].expand;
    value = text_next_int();
    leaders[leader].expand = (i8)value;
    if (load_civ_data == 0) {
      leaders[leader].expand = old_trait;
    }
    old_trait = leaders[leader].civilize;
    value = text_next_int();
    leaders[leader].civilize = (i8)value;
    if (load_civ_data == 0) {
      leaders[leader].civilize = old_trait;
    }
    for (govt = 0; govt < 7; govt++) {
      for (gender = 0; gender < 2; gender++) {
        if (load_civ_data != 0) {
          leaders[leader].titles[govt][gender] =
               (short)(&government_titles[govt].male)[gender];
        }
      }
    }
    do {
      title_govt = text_next_int();
      if (0 < title_govt) {
        old_male_title = leaders[leader].titles[title_govt][0];
        old_female_title = leaders[leader].titles[title_govt][1];
        value = text_next_token_pooled();
        leaders[leader].titles[title_govt][0] = (short)value;
        value = text_next_token_pooled();
        leaders[leader].titles[title_govt][1] = (short)value;
        if (load_civ_data == 0) {
          leaders[leader].titles[title_govt][0] = old_male_title;
          leaders[leader].titles[title_govt][1] = old_female_title;
        }
      }
    } while (0 < title_govt);
  }
  return;
}

/* 0041b00e auto */
void __cdecl rules_parse_sections(i32 load_civ_data)

{
  i32 string_id;
  iptr i;
  char key[32];
  
  rules_load_cosmic();
  rules_load_advances(0);
  rules_load_improvements(0);
  rules_load_units(0);
  rules_load_terrain();
  rules_load_govts_leaders(load_civ_data);
  if (rules_from_scenario() == 0) {
    /* (port) the game's rules (rules.h) */
    for (i = 0; i < 0x10; i++) {
      snprintf(key,sizeof key,"rules.commodity.%d",i);
      commodity_names[i] = rules_name(key,10);
    }
    for (i = 1; i < 0xd; i++) {
      snprintf(key,sizeof key,"rules.order.%d",i);
      order_names[i].name = rules_name_pooled(key);
      snprintf(key,sizeof key,"rules.order.%d.key",i);
      order_names[i].key = *lang_text(key);
    }
    for (i = 0; i < 6; i++) {
      snprintf(key,sizeof key,"rules.difficulty.%d",i);
      difficulty_names[i] = rules_name_pooled(key);
    }
    for (i = 0; i < 9; i++) {
      snprintf(key,sizeof key,"rules.attitude.%d",i);
      attitude_names[i] = rules_name_pooled(key);
    }
    text_close_file();
    return;
  }
  text_open_section(0,(const char *)&str_caravan);
  for (i = 0; i < 0x10; i++) {
    text_read_line(1);
    string_id = text_next_token_str_id(10);
    commodity_names[i] = string_id;
  }
  text_open_section(0,(const char *)&str_orders);
  for (i = 1; i < 0xd; i++) {
    text_read_line(1);
    string_id = text_next_token_pooled();
    order_names[i].name = string_id;
    text_next_token();
    order_names[i].key = text_parse.token_buf[0];
  }
  text_open_section(0,(const char *)&str_difficulty);
  for (i = 0; i < 6; i++) {
    string_id = text_read_line_pooled();
    difficulty_names[i] = string_id;
  }
  text_open_section(0,(const char *)&str_attitudes);
  for (i = 0; i < 9; i++) {
    string_id = text_read_line_pooled();
    attitude_names[i] = string_id;
  }
  text_close_file();
  return;
}

/* 0041b8ff auto */
void __cdecl civ_intro_popup(iptr civ)

{
  int has_advance;
  iptr name_text;
  iptr advance;
  iptr known_count;
  iptr known_advances[12];
  
  advance = 100;
  known_count = 0;
  while ((0 < advance && (known_count < 0xc))) {
    /* advances[100] is one past the table: the original read the terrain
       table that follows it there (irrigate_to, mine_to of the first terrain) */
    if (((advance == 100) ? ((terrain_types[0].irrigate_to != -2) || (terrain_types[0].mine_to != -2))
                          : ((advances[advance].preq[0] != -2) || (advances[advance].preq[1] != -2))) &&
       (has_advance = civ_has_advance(civ,advance), has_advance != 0)) {
      known_advances[known_count] = advance;
      known_count++;
    }
    advance--;
  }
  name_text = (iptr)(civ_leader_name(civ));
  text_set_string_arg(0,(const char *)name_text);
  name_text = (iptr)(civ_name_plural(civ));
  text_set_string_arg(1,(const char *)name_text);
  text_clear();
  advance = known_count;
  if (0 < known_count) {
    while (advance = advance - 1, -1 < advance) {
      text_append_string_id(advances[known_advances[advance]].name);
      text_append_comma();
      text_append_space();
    }
  }
  text_set_string_arg(2,(const char *)&textbuf.buf);
  if (2 < game.multiplayer) {
    dlg_style.pending_idle_callback = turn_popup_timeout_idle;
  }
  popup_game_with_sprite((const char *)&str_init,(iptr)&popup_info_icon_sprite,0);
  return;
}

/* 0041ba52: written by hand in src/game/bigframes.cpp */

/* 0041e864 auto */
void __cdecl rules_load(i32 load_civ_data)

{
  int exists;
  i8 rules_path[260];
  
  labels_reload();
  gfx_load_game_sprites();
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  crt_strcpy(rules_path,str_rules_3);
  crt_strcat(rules_path,((i8 *)&lang_file_ext));
  exists = file_exists((const char *)rules_path);
  if (exists == 0) {
    __chdir(((i8 *)&game_dirs.home));
  }
  rules_parse_sections(load_civ_data);
  __chdir(((i8 *)&game_dirs.home));
  input_flush_events();
  return;
}

/* 0042a542 auto */
void __cdecl civ_rating_title_font_global_init(void)

{
  civ_rating_title_font_static_init();
  civ_rating_title_font_reg_atexit();
  return;
}

/* 0042a55c auto */
void __cdecl civ_rating_title_font_static_init(void)

{
  ((GameFont *)((i8 *)&civ_rating_title_font_block.civ_rating_title_font))->init_styled(0,0xc,1);
  return;
}

/* 0042a57c auto */
void __cdecl civ_rating_title_font_reg_atexit(void)

{
  _atexit(civ_rating_title_font_static_dtor);
  return;
}

/* 0042a599 auto */
void __cdecl civ_rating_title_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&civ_rating_title_font_block.civ_rating_title_font))->destroy();
  return;
}

/* 0042ad8f auto */
/* WARNING: Removing unreachable block (ram,0x0042b48e) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl science_advisor_draw(void)

{
  iptr text;
  int value;
  i32 max_value;
  int has_advance;
  i32 bar_height;
  i32 bar_flag;
  i8 sprite_rect[16];
  iptr civ;
  int draw_y;
  int total_science2;
  int list_bottom;
  int govt_index;
  iptr first_item;
  int item_x;
  int box_bottom;
  int cur_y;
  int item_index;
  iptr city;
  int box_top;
  iptr advance;
  int scrollbar_height;
  int list_right;
  int research_cost;
  int list_left;
  int known_count;
  int text_x2;
  int column;
  int row;
  int line_height;
  i8 frame_rect2[16];
  int rows_per_page;
  
  civ = advisor_state.report_civ;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  cur_y = advisor_state.client_y + 2;
  list_bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_table.string_ids[0x153]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
  cur_y += line_height;
  text_clear();
  govt_index = int_clamp(civs[civ].government - 1,0,4);
  text_append_label(govt_index + 0x148);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  text = (iptr)(civ_name_plural(civ));
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
  cur_y += line_height;
  text_clear();
  text = civ_leader_title(civ);
  text_append((const char *)text);
  text_append_space();
  text = (iptr)(civ_leader_name(civ));
  text_append((const char *)text);
  text_append_colon();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
  cur_y = cur_y + line_height + 6;
  if (-1 < civs[civ].researching) {
    box_top = cur_y;
    text_clear();
    text_append_label(0xec);
    text_append_colon();
    text_append_string_id(advances[civs[civ].researching].name);
    if (extra_ui(X_RESEARCH)) {
      /* X6: the bulbs so far, the advance's cost and the bulbs per turn */
      int per_turn = 0;
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city].owner == civ)) {
          per_turn += cities[city].science;
        }
      }
      text_append_space();
      text_append_lparen();
      text_append_int((int)civs[civ].research_progress);
      text_append("/");
      text_append_int(civ_research_cost(civ));
      text_append(", +");
      text_append_int(per_turn);
      text_append_rparen();
    }
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
    cur_y += line_height;
    draw_y = cur_y + 9;
    bar_flag = 1;
    value = rect_width((RECT *)&advisor_state.client_rect);
    value -= 10;
    bar_height = 0xe;
    max_value = civ_research_cost(civ);
    sprite_draw_spaced_row((iptr)&advisor_state.window,(iptr)&tax_rate_icon_sprites_block.science_icon_sprite,advisor_state.client_rect + 5,draw_y,
                       (int)civs[civ].research_progress,max_value,bar_height,value,bar_flag);
    box_bottom = cur_y + 0x20;
    cur_y = box_bottom;
    rect_set((iptr)frame_rect2,advisor_state.client_rect + 2,box_top,advisor_state.client_right - 2,box_bottom);
    gfx_draw_rect_frame((iptr)&advisor_state.window,(RECT *)frame_rect2,0x29);
    cur_y += 6;
  }
  research_cost = civ_research_cost(civ);
  total_science2 = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      total_science2 += cities[city].science;
    }
  }
  if (total_science2 != 0) {
    text_clear();
    text_append_label(0x170);
    text_append_space();
    text_append_int((total_science2 - 1 + research_cost) / total_science2);
    text_append_space();
    text_append_label(0x171);
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
    cur_y = cur_y + line_height + 6;
  }
  scrollbar_height = GetSystemMetrics(3);
  list_bottom -= (scrollbar_height + 2);
  line_height = 0x16;
  advisor_state.row_height = 0x16;
  advisor_state.list_top = cur_y;
  advisor_state.list_height = list_bottom - cur_y;
  advisor_state.rows_per_page = int_clamp((list_bottom - cur_y) / 0x16,1,99);
  advisor_state.list_left = advisor_state.client_x + 2;
  list_right = advisor_state.client_width + advisor_state.client_x - 2;
  advisor_state.list_width = list_right - advisor_state.list_left;
  advisor_state.column_width = advisor_state.list_width / 3;
  advisor_state.items_per_page = advisor_state.rows_per_page * 3;
  known_count = 0;
  list_left = advisor_state.list_left;
  rows_per_page = advisor_state.rows_per_page;
  for (advance = 0; advance < 100; advance++) {
    value = civ_has_advance(civ,advance);
    if (value != 0) {
      known_count++;
    }
  }
  advisor_state.page_count = int_clamp((rows_per_page - 1 + known_count) / rows_per_page,1,99);
  max_value = int_clamp(known_count - 1,0,999);
  advisor_state.first_row = int_clamp(advisor_state.first_row,0,max_value);
  item_index = 0;
  column = 0;
  row = 0;
  draw_y = cur_y;
  first_item = advisor_state.first_row;
  text_set_font((iptr)&advisor_bold_font);
  for (advance = 0; advance < 100; advance++) {
    has_advance = civ_has_advance(civ,advance);
    value = item_index;
    if (((has_advance != 0) && (value = item_index + 1, first_item <= item_index)) &&
       (item_index < advisor_state.items_per_page + first_item)) {
      draw_y = row * line_height + cur_y;
      item_x = advisor_state.column_width * column + list_left;
      item_index = value;
      ((Sprite *)
                 (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                 (char)advances[advance].epoch * 0x3c + (char)advances[advance].category * 0xf0))->draw(
                 (iptr)sprite_rect,(iptr)&advisor_state.window,item_x,draw_y);
      text_x2 = item_x + 0x26;
      text_set_colors((-(uint)((char)game.first_discoverer[advance] == civ) & 0xffffffcb) +
                         0x5e,0x12,1,1);
      has_advance = draw_y + 2;
      value = text_x2;
      text = (iptr)label_string(advances[advance].name);
      gfx_draw_text((const char *)text,value,has_advance);
      row++;
      value = item_index;
      if (rows_per_page <= row) {
        row = 0;
        column++;
        if (2 < column) break;
      }
    }
    item_index = value;
  }
  if (advisor_state.controls_created == 0) {
    rect_set((iptr)frame_rect2,list_left,list_bottom,list_right,list_bottom + scrollbar_height);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->create((iptr)&advisor_state.wnd,200,(iptr)frame_rect2,0);
    max_value = int_clamp((rows_per_page - 1 + known_count) / rows_per_page - 1,0,999);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_range(0,max_value);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_pos(advisor_state.first_row);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_page_size(1);
    ((Scrollbar *)((i8 *)&advisor_state.scrollbar))->set_callback(science_advisor_on_scroll);
    ((Control *)((i8 *)&advisor_state.scrollbar))->show_and_redraw();
    ((WndHandlers *)((i8 *)&advisor_state.wnd_events))->set_lbuttonup_handler(science_advisor_on_click);
    advisor_state.controls_created = 1;
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 0042b540 auto */
void __cdecl science_advisor_on_scroll(int scroll_pos)

{
  advisor_state.first_row = advisor_state.rows_per_page * scroll_pos;
  science_advisor_draw();
  return;
}

/* 0042b563 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl science_advisor_on_click(int click_x,int click_y)

{
  int row;
  int column;
  int has_advance;
  bool is_match;
  int advance;
  int known_index;
  
  if ((((-1 < click_y - advisor_state.list_top) &&
       (row = (click_y - advisor_state.list_top) / advisor_state.row_height, row < advisor_state.rows_per_page)) &&
      (-1 < click_x - 2)) && (column = (click_x - 2) / advisor_state.column_width, column < 3)) {
    column = advisor_state.rows_per_page * column + advisor_state.first_row;
    known_index = 0;
    for (advance = 0; advance < 100; advance++) {
      has_advance = civ_has_advance(advisor_state.report_civ,advance);
      if ((has_advance != 0) && (is_match = known_index == column + row, known_index = known_index + 1, is_match)) {
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(advance);
        return;
      }
    }
  }
  return;
}

/* 0042b65b auto */
void __cdecl science_advisor_on_goal_clicked(void)

{
  civ_research_goal_dialog(advisor_state.report_civ,1,0);
  return;
}

/* 0042b67d auto */
/* WARNING: Removing unreachable block (ram,0x0042b72b) */
/* WARNING: Removing unreachable block (ram,0x0042b7b6) */

void __cdecl science_advisor_show(iptr civ)

{
  int client_y;
  int rect_h;
  iptr label;
  i8 button_rect[16];
  
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(6,6,0,600,400,0,0);
  advisor_state.report_civ = civ;
  rect_set_xywh((iptr)button_rect,2,0,0x129,0x18);
  client_y = advisor_state.client_y;
  rect_h = rect_height((RECT *)button_rect);
  rect_offset((iptr)button_rect,advisor_state.client_x,(client_y - rect_h) + 0x18e);
  rect_offset((iptr)button_rect,299,0);
  label = (iptr)label_string(label_table.string_ids[0x147]);
  ((Button *)((i8 *)&advisor_state.close_button))->create((iptr)&advisor_state.wnd,100,(iptr)button_rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.close_button))->set_callback((CtrlNotifyFn)advisor_on_close_clicked);
  ((Button *)((i8 *)&advisor_state.close_button))->set_default_button();
  ((Button *)((i8 *)&advisor_state.close_button))->set_cancel_button();
  rect_offset((iptr)button_rect,0xfffffed5,0);
  label = (iptr)label_string(label_table.string_ids[0x173]);
  ((Button *)((i8 *)&advisor_state.toggle_button))->create((iptr)&advisor_state.wnd,0x65,(iptr)button_rect,(const char *)label);
  ((Button *)((i8 *)&advisor_state.toggle_button))->set_callback((CtrlNotifyFn)science_advisor_on_goal_clicked);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(science_advisor_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  return;
}

/* Part of civ_score_window_draw (00434e39): the citizens of all the civ's
   cities, by mood, in rows from text_y3 (at most 7 rows). Returns the y of
   the line under them. */
static int civ_score_draw_citizens(iptr civ,int left_x,int text_y3)
{
  int value;
  i8 citizen_rect[16];
  int content_count;
  uint sprite_index;
  iptr city;
  int column;
  int happy_count;
  uint citizen_count;
  int draw_y;
  int draw_x;
  int citizen;
  int unhappy_count;
  uint row;
  int row_height;
  int row_wrapped;
  int era;
  int icon_width;
  int full;

  era = civ_city_style_era(civ);
  draw_y = text_y3;
  draw_x = left_x;
  citizen_count = 0;
  row = 0;
  column = 0;
  row_height = 0x12;
  icon_width = 0x14;
  row_wrapped = 0;
  full = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      city_recalc(city,1);
      happy_count = city_calc.happy_count;
      unhappy_count = city_calc.unhappy_count;
      content_count = (((cities[city].size - city_calc.happy_count) - city_calc.unhappy_count) - city_calc.specialist_count) -
                 city_calc.angry_count;
      for (citizen = 0; citizen < cities[city].size; citizen++) {
        if (citizen < happy_count) {
          sprite_index = citizen_count & 1;
        }
        else if (citizen - happy_count < content_count) {
          sprite_index = (citizen_count & 1) + 2;
        }
        else if ((citizen - happy_count) - content_count < unhappy_count) {
          sprite_index = (citizen_count & 1) + 4;
        }
        else if (((citizen - happy_count) - content_count) - unhappy_count < city_calc.angry_count) {
          sprite_index = (citizen_count & 1) + 6;
        }
        else {
          value = city_specialist_type(city,(((citizen - happy_count) - content_count) - unhappy_count) -
                                              city_calc.angry_count);
          sprite_index = value + 7;
        }
        citizen_count++;
        ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + era * 0x294 + sprite_index * 0x3c))->draw(
                   (iptr)citizen_rect,(iptr)&advisor_state.window,draw_x,draw_y);
        row_wrapped = 0;
        draw_x += icon_width;
        column++;
        if (advisor_state.client_width + advisor_state.client_x - 0x1d < draw_x) {
          column = 0;
          row++;
          row_wrapped = 1;
          draw_x = (icon_width / 2) * (row & 1) + left_x;
          draw_y += row_height;
          if ((6 < (int)row) || (0xf0 < draw_y)) {
            full = 1;  /* no room for more rows */
            break;
          }
        }
      }
      if (full != 0) break;
    }
  }
  if (row_wrapped != 0) {
    draw_y -= row_height;
  }
  text_y3 = draw_y + 0x22;
  return text_y3;
}

/* Part of civ_score_window_draw (00434e39): the civ's wonders, three to a row
   from text_y3. Returns the y of the line under them. */
static int civ_score_draw_wonders(iptr civ,int left_x,int text_y3)
{
  iptr text;
  int text_y2;
  int value;
  i8 wonder_rect[16];
  iptr city;
  int column;
  int text_x2;
  int draw_y;
  int draw_x;
  iptr wonder;
  int row_wrapped;

  draw_x = left_x;
  column = 0;
  row_wrapped = 0;
  draw_y = text_y3;
  for (wonder = 0; wonder < 0x1c; wonder++) {
    if ((((-1 < game.wonder_city[wonder]) && (game.wonder_city[wonder] < game.num_cities))
        && (cities[game.wonder_city[wonder]].id != 0)) &&
       (city = (iptr)game.wonder_city[wonder], cities[city].owner == civ)) {
      ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c))->draw((iptr)wonder_rect,(iptr)&advisor_state.window,
                 draw_x,draw_y);
      text_x2 = draw_x + 0x26;
      text_set_colors(0x5e,0x12,0xffffffff,0xffffffff);
      text_y2 = draw_y + 3;
      value = text_x2;
      text = (iptr)label_string(improvements[wonder + 0x27].name);
      gfx_draw_text((const char *)text,value,text_y2);
      row_wrapped = 0;
      column++;
      draw_x += 0xc4;
      if (2 < column) {
        column = 0;
        draw_x = left_x;
        draw_y += 0x16;
        row_wrapped = 1;
        if (0x122 < draw_y) break;
      }
    }
  }
  if (row_wrapped == 0) {
    draw_y += 0x16;
  }
  text_y3 = draw_y + 4;
  return text_y3;
}

/* Part of civ_score_window_draw (00434e39): a scenario with objectives:
   the objective points of the civs and the victory thresholds. Returns the
   y of the line under them. */
static int civ_score_draw_objectives(iptr civ,int header_y,int line_height)
{
  iptr text;
  int text_y3;
  iptr threshold;
  iptr civ2;

  text_y3 = header_y + line_height + 0x2a;
  header_y = advisor_state.list_top;
  civ_calc_score(civ);
  text_clear();
  text_append_label(0x1b1);
  text_append_colon();
  text_append_int(civ_score.total_objective_value);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,text_y3,advisor_state.client_width);
  text_y3 += line_height;
  if (scenario.objective_civ != 0) {
    text_clear();
    text = civ_get_adjective((int)scenario.objective_civ);
    text_append((const char *)text);
    text_append_space();
    text_append_label(0x1b2);
    text_append_colon();
    text_append_int(civ_score.objective_civ_value);
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,text_y3,advisor_state.client_width);
    text_y3 += line_height;
  }
  if ((scenario.objective_civ == 0) || (scenario.objective_civ != civ)) {
    text_clear();
    text = civ_get_adjective(civ);
    text_append((const char *)text);
    text_append_space();
    text_append_label(0x1b2);
    text_append_colon();
    text_append_int(civ_score.civ_objective_value);
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,text_y3,advisor_state.client_width);
    text_y3 += line_height;
  }
  if (scenario.objective_civ == 0) {
    civ2 = civ;
  }
  else {
    civ2 = (iptr)scenario.objective_civ;
  }
  text_y3 += 6;
  text_set_colors(0x5e,0x12,1,1);
  for (threshold = 0; threshold < 4; threshold++) {
    text_clear();
    text = civ_get_adjective(civ2);
    text_append((const char *)text);
    text_append_space();
    if (threshold == 0) {
      text_append_label(0x1b3);
    }
    else if (threshold == 1) {
      text_append_label(0x1b4);
    }
    else if (threshold == 2) {
      text_append_label(0x1b6);
    }
    else if (threshold == 3) {
      text_append_label(0x1b7);
    }
    text_append_colon();
    text_append_int((int)(((i16 *)&scenario.victory_thresholds))[threshold]);
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,text_y3,advisor_state.client_width);
    text_y3 += line_height;
  }
  text_y3 += 6;
  text_set_colors(0x7a,0x12,1,1);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_clear();
  text = civ_get_adjective(civ2);
  text_append((const char *)text);
  text_append_space();
  text_append_label(civ_score.objective_outcome + 0x1b3);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,text_y3,advisor_state.client_width);
  text_y3 = text_y3 + line_height + 6;
  return text_y3;
}

/* Part of civ_score_window_draw (00434e39): the total score line (or the
   scenario points, centred: measured first), with the "cheated" and
   "conquest" notes. draw_x: the caller's, read stale by the measuring pass. */
static void civ_score_draw_total(int left_x,int text_y3,int &draw_x)
{
  int value;
  int total_width;
  int measure_pass;

  measure_pass = 0;
  total_width = 0;
  if (((game.game_flags & GAME_SCENARIO) != 0) && ((scenario.flags[0] & 2) != 0)) {
    civ_score.conquest_bonus = -1;
    civ_score.total = civ_score.scenario_points;
    measure_pass = 1;
  }
  while( true ) {
    text_set_font((iptr)&advisor_font_block.advisor_font);
    text_clear();
    if (civ_score.total < civ_score.conquest_bonus) {
      text_append_label(0x198);
      text_append_colon();
      text_append_int(civ_score.conquest_bonus);
    }
    else {
      text_append_label(0x197);
      text_append_colon();
      text_append_int(civ_score.total);
    }
    if (measure_pass == 1) {
      value = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->text_width((const char *)&textbuf.buf);
      total_width += value;
    }
    else {
      text_set_colors(0x25,0x12,1,1);
      draw_x = gfx_draw_text((const char *)&textbuf.buf,left_x,text_y3);
    }
    if ((game.game_flags & GAME_SCENARIO) != 0) {
      draw_x += 0x19;
      text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
      text_clear();
      text_append_label(0x1a2);
      if (measure_pass == 1) {
        value = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->text_width((const char *)&textbuf.buf);
        total_width = total_width + value + 0x19;
      }
      else {
        draw_x = gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y3);
      }
    }
    draw_x += 0x19;
    if ((game.game_flags & GAME_CHEATED) != 0) {
      text_set_colors(0x6a,0x12,0xffffffff,0xffffffff);
      text_clear();
      text_append_label(0x19a);
      if (measure_pass == 1) {
        value = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->text_width((const char *)&textbuf.buf);
        total_width = total_width + value + 0x19;
      }
      else {
        gfx_draw_text((const char *)&textbuf.buf,draw_x,text_y3);
      }
    }
    if (measure_pass != 1) break;
    left_x = ((advisor_state.client_width >> 1) + advisor_state.client_x) - (total_width >> 1);
    measure_pass = 2;
  }
}

/* 00434e39 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl civ_score_window_draw(void)

{
  iptr text;
  iptr civ;
  int text_y3;
  int list_bottom;
  int left_x;
  int header_y;
  int draw_x;
  int row_wrapped;
  int line_height;
  
  civ = advisor_state.report_civ;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  header_y = advisor_state.client_y + 2;
  list_bottom = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_table.string_ids[0x157]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,header_y,advisor_state.client_width);
  header_y += line_height;
  text_set_font((iptr)&advisor_bold_font);
  line_height = ((GameFont *)((i8 *)&advisor_bold_font))->line_spacing();
  text_clear();
  text = civ_leader_title(civ);
  text_append((const char *)text);
  text_append_space();
  text = (iptr)(civ_leader_name(civ));
  text_append((const char *)text);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  text = (iptr)(civ_name_plural(civ));
  text_append((const char *)text);
  text_append_comma();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,header_y,advisor_state.client_width);
  advisor_state.list_top = header_y + line_height + 4;
  advisor_state.row_height = line_height;
  advisor_state.list_height = list_bottom - advisor_state.list_top;
  left_x = advisor_state.client_x + 2;
  text_y3 = advisor_state.list_top;
  if ((game.game_flags & GAME_OVER_SCORED) == 0) {
    if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
      header_y = advisor_state.list_top;
      text_y3 = civ_score_draw_citizens(civ,left_x,text_y3);
      civ_calc_score(civ);
      text_clear();
      if (game_language != 1) {
        text = civ_get_adjective(civ);
        text_append((const char *)text);
        text_append_space();
      }
      text_append_label(0x193);
      text_append_space();
      text_append_lparen();
      text_append_int(civ_score.citizens);
      text_append_rparen();
      gfx_draw_text((const char *)&textbuf.buf,left_x,text_y3);
      text_y3 = text_y3 + line_height + 4;
      text_y3 = civ_score_draw_wonders(civ,left_x,text_y3);
      text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
      text_clear();
      if (game_language != 1) {
        text = civ_get_adjective(civ);
        text_append((const char *)text);
        text_append_space();
      }
      text_append_label(0x194);
      text_append_space();
      text_append_lparen();
      text_append_int(civ_score.wonders);
      text_append_rparen();
      gfx_draw_text((const char *)&textbuf.buf,left_x,text_y3);
      text_y3 += line_height;
      if ((civ_score.spaceship_arrived != 0) || (civ_score.spaceship_in_flight != 0)) {
        text_clear();
        text_append_label(0x195);
        text_append_colon();
        text_append_lparen();
        if (civ_score.spaceship_arrived == 0) {
          text_append_int(civ_score.spaceship_in_flight);
          text_append_space();
          text_append_label(0x199);
        }
        else {
          text_append_int(civ_score.spaceship_arrived);
        }
        text_append_rparen();
        gfx_draw_text((const char *)&textbuf.buf,left_x,text_y3);
        text_y3 += line_height;
      }
      row_wrapped = 0;
      text_clear();
      if (civ_score.pollution != 0) {
        text_append_label(0x17c);
        text_append_space();
        text_append_lparen();
        text_append_int(civ_score.pollution);
        text_append_rparen();
        text_append((const char *)&str_empty_52);
        row_wrapped = 1;
      }
      if ((199 < game.turn) && (civ_score.peace != 0)) {
        text_append_label(0x196);
        text_append_colon();
        text_append_lparen();
        text_append((const char *)&str_empty_53);
        text_append_int(civ_score.peace);
        text_append_rparen();
        text_append((const char *)&str_empty_54);
        row_wrapped = 1;
      }
      if (civ_score.future_techs != 0) {
        text_append_string_id(advances[ADV_FUTURE_TECH].name);
        text_append_colon();
        text_append_lparen();
        text_append((const char *)&str_empty_55);
        text_append_int(civ_score.future_techs);
        text_append_rparen();
        text_append((const char *)&str_empty_56);
        row_wrapped = 1;
      }
      text_append_label(0xf);
      text_append_colon();
      text_append_lparen();
      if (-1 < civ_score.barbarian_bonus) {
        text_append((const char *)&str_empty_57);
      }
      text_append_int(civ_score.barbarian_bonus);
      text_append_rparen();
      row_wrapped = 1;
      gfx_draw_text((const char *)&textbuf.buf,left_x,text_y3);
      text_y3 += line_height;
    }
    else {
      text_y3 = civ_score_draw_objectives(civ,header_y,line_height);
    }
    civ_score_draw_total(left_x,text_y3,draw_x);
  }
  else {
    header_y = advisor_state.list_top;
    text_clear();
    text_append_label(0x1c9);
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,header_y,advisor_state.client_width);
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 00435d15 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl civ_score_window_show(iptr civ)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(10,10,1,600,400,0,0);
  advisor_state.report_civ = civ;
  ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(civ_score_window_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(advisor_mp_idle);
  }
  advisor_modal_start_ticks_i32 = timer_ticks();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->run_modal_loop();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(0);
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  return;
}

/* 00435dc4 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl civ_rating_window_draw(void)

{
  int cur_y;
  int rating_percent;
  iptr text;
  iptr civ;
  int bottom_y;
  int title_index;
  int difficulty_factor;
  int i;
  i8 line_buf[0x100];  /* BF-050 (Peter Johnson's ledger): the original's 80,
                         less than a line of GAME.TXT */
  int line_height;
  int open_failed;
  int best_score;
  
  civ = advisor_state.report_civ;
  ((FrameWindow *)((i8 *)&advisor_state.window))->layout();
  ((FrameWindow *)((i8 *)&advisor_state.window))->FrameWindow::draw_caption();
  ((PictureWindow *)((i8 *)&advisor_state.window))->blit();
  cur_y = advisor_state.client_y + 1;
  bottom_y = advisor_state.client_y + 0x174;
  line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
  line_height -= 3;
  difficulty_factor = game.difficulty + 4;
  if (DIFF_PRINCE < game.difficulty) {
    difficulty_factor = game.difficulty + 5;
  }
  if (DIFF_KING < game.difficulty) {
    difficulty_factor++;
  }
  if (DIFF_EMPEROR < game.difficulty) {
    difficulty_factor += 2;
  }
  best_score = civ_score.total;
  if (civ_score.total <= civ_score.conquest_bonus) {
    best_score = civ_score.conquest_bonus;
  }
  rating_percent = (difficulty_factor * best_score) / 100;
  title_index = 0;
  for (i = 1; i < 0x19; i++) {
    if ((i * i) / 3 <= rating_percent) {
      title_index = i - 1;
    }
  }
  if (0x17 < title_index) {
    title_index = 0x17;
  }
  civ_rating_title_font_block.civ_rating_title_index = title_index;
  casualty_window_block.civ_rating_percent = rating_percent;
  text_set_surface((iptr)&advisor_state.window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_table.string_ids[0x19c]);
  text_append((const char *)text);
  text_append_colon();
  text_append_int(rating_percent);
  text_append((const char *)&str_empty_58);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
  cur_y += line_height;
  text_clear();
  text_append_label(0x19d);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
  cur_y += line_height;
  text_clear();
  text_append_label(0x19e);
  gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,cur_y,advisor_state.client_width);
  advisor_state.list_top = cur_y + line_height + 4;
  advisor_state.row_height = line_height;
  advisor_state.list_height = bottom_y - advisor_state.list_top;
  if (leaders[civs[civ].tribe].female == '\0') {
    crt_strcpy(line_buf,str_malefame);
  }
  else {
    crt_strcpy(line_buf,str_femalefame);
  }
  open_failed = text_open_section((const char *)&main_frame_wnd_block.game_txt_path,(const char *)line_buf);
  text = (iptr)(civ_leader_name(civ));
  text_set_string_arg(0,(const char *)text);
  for (i = 0; i <= title_index; i++) {
    if (open_failed == 0) {
      text_read_line(1);
      str_copy_max(line_buf,((i8 *)&textbuf.buf),sizeof line_buf);
    }
    else if (title_index == i) {
      crt_strcpy(line_buf,str_string0);
    }
    else {
      crt_strcpy(line_buf,str_error_2);
    }
    /* BF-064: expanded within the text buffer */
    text_expand_vars_max((iptr)line_buf,(iptr)&textbuf.buf,TEXTBUF_SIZE);
    if (i < title_index) {
      text_set_font((iptr)&civ_rating_title_font_block.civ_rating_title_font);
      text_set_colors(0x1d,0x12,0xffffffff,0xffffffff);
      line_height = ((GameFont *)((i8 *)&civ_rating_title_font_block.civ_rating_title_font))->line_spacing();
      line_height -= 3;
    }
    else {
      text_set_font((iptr)&advisor_font_block.advisor_font);
      line_height = ((GameFont *)((i8 *)&advisor_font_block.advisor_font))->line_spacing();
      line_height -= 4;
      text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
    }
    bottom_y -= line_height;
    gfx_draw_text_centered((const char *)&textbuf.buf,advisor_state.client_x,bottom_y,advisor_state.client_width);
  }
  ((DibSurface *)((i8 *)&advisor_state.window))->wnd_blit_surface();
  return;
}

/* 004361cc auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl civ_rating_window_show(iptr civ)

{
  ((PictureWindow *)((i8 *)&advisor_state.window))->open(10,10,1,600,400,0,0);
  advisor_state.report_civ = civ;
  ((PictureWindow *)((i8 *)&advisor_state.window))->advisor_add_close_button();
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->set_redraw_callback(civ_rating_window_draw);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->repaint();
  music_play_track(3,0);
  ((SurfaceWindow *)((i8 *)&advisor_state.window))->SurfaceWindow::show();
  if (2 < game.multiplayer) {
    ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(advisor_mp_idle);
  }
  advisor_modal_start_ticks_i32 = timer_ticks();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->run_modal_loop();
  ((ModalMenuWindow *)((i8 *)&advisor_state.wnd))->set_modal_idle_callback(0);
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  return;
}

/* 0043cab0 auto */
i32 __cdecl civ_color_primary(iptr civ)

{
  int color_index;
  
  if (civ == 0) {
    color_index = 0;
  }
  else {
    color_index = (int)leaders[civs[civ].tribe].color;
  }
  return civ_primary_colors_block.civ_colors[color_index].primary;
}

/* 0043cb30 auto */
i32 __cdecl civ_color_secondary(iptr civ)

{
  int color_index;
  
  if (civ == 0) {
    color_index = 0;
  }
  else {
    color_index = (int)leaders[civs[civ].tribe].color;
  }
  return civ_primary_colors_block.civ_colors[color_index].secondary;
}

/* 0043cce5 auto */
int __cdecl civ_total_population(iptr civ)

{
  int city_pop;
  iptr city;
  int total;
  
  total = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      city_pop = city_population_units(city);
      total += city_pop;
    }
  }
  if (extra_rule(X_POP_LIMIT)) {
    if ((total < 0) || (extras_population_limit() < total)) {
      total = extras_population_limit();  /* X51 */
    }
  }
  else if ((total < 0) || (32000 < total)) {
    total = 32000;
  }
  if (total < 2) {
    total = 1;
  }
  return total;
}

/* 00442541 auto */
void __cdecl civ_reset_city_production(uptr civ,iptr landmass)

{
  int city_landmass;
  iptr city;
  
  if ((game.multiplayer != 1) || ((1 << ((byte)civ & 0x1f) & (uint)net.hotseat_human_players) == 0)) {
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && ((int)cities[city].owner == (civ & 0xff))) &&
         ((landmass < 0 ||
          (city_landmass = tile_landmass((int)cities[city].x,(int)cities[city].y), city_landmass == landmass)))
         ) {
        city_set_production(city,99);
      }
    }
  }
  return;
}

/* 004481f0 auto */
void __cdecl civ_flag_sprites_global_init(void)

{
  civ_flag_sprites_static_init();
  civ_flag_sprites_reg_atexit();
  return;
}

/* 0044820a auto */
void __cdecl civ_flag_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites),0x3c,0x10,sprite_init,sprite_destroy);
  return;
}

/* 00448232 auto */
void __cdecl civ_flag_sprites_reg_atexit(void)

{
  _atexit(civ_flag_sprites_static_dtor);
  return;
}

/* 0044824f auto */
void __cdecl civ_flag_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites),0x3c,0x10,sprite_destroy);
  return;
}

/* 00448758 auto */
void __cdecl research_flask_sprites_global_init(void)

{
  research_flask_sprites_static_init();
  research_flask_sprites_reg_atexit();
  return;
}

/* 00448772 auto */
void __cdecl research_flask_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites),0x3c,4,sprite_init,sprite_destroy);
  return;
}

/* 0044879a auto */
void __cdecl research_flask_sprites_reg_atexit(void)

{
  _atexit(research_flask_sprites_static_dtor);
  return;
}

/* 004487b7 auto */
void __cdecl research_flask_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&city_down_arrow_sprite_block.research_flask_sprites),0x3c,4,sprite_destroy);
  return;
}

/* 004487da auto */
void __cdecl global_warming_sprites_global_init(void)

{
  global_warming_sprites_static_init();
  global_warming_sprites_reg_atexit();
  return;
}

/* 004487f4 auto */
void __cdecl global_warming_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites),0x3c,4,sprite_init,sprite_destroy);
  return;
}

/* 0044881c auto */
void __cdecl global_warming_sprites_reg_atexit(void)

{
  _atexit(global_warming_sprites_static_dtor);
  return;
}

/* 00448839 auto */
void __cdecl global_warming_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&city_down_arrow_sprite_block.global_warming_sprites),0x3c,4,sprite_destroy);
  return;
}

/* 004489cb auto */
void __cdecl advance_cat_icon_sprites_global_init(void)

{
  advance_category_icon_sprites_static_init();
  advance_category_icon_sprites_reg_atexit();
  return;
}

/* 004489e5 auto */
void __cdecl advance_category_icon_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&government_sprites_block.advance_category_icon_sprites),0x3c,0x14,sprite_init,sprite_destroy);
  return;
}

/* 00448a0d auto */
void __cdecl advance_category_icon_sprites_reg_atexit(void)

{
  _atexit(advance_cat_icon_sprites_static_dtor);
  return;
}

/* 00448a2a auto */
void __cdecl advance_cat_icon_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&government_sprites_block.advance_category_icon_sprites),0x3c,0x14,sprite_destroy);
  return;
}

/* 00448bbc auto */
void __cdecl government_sprites_global_init(void)

{
  government_sprites_static_init();
  government_sprites_reg_atexit();
  return;
}

/* 00448bd6 auto */
void __cdecl government_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&government_sprites_block.government_sprites),0x3c,7,sprite_init,sprite_destroy);
  return;
}

/* 00448bfe auto */
void __cdecl government_sprites_reg_atexit(void)

{
  _atexit(government_sprites_static_dtor);
  return;
}

/* 00448c1b auto */
void __cdecl government_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&government_sprites_block.government_sprites),0x3c,7,sprite_destroy);
  return;
}

/* 00448c3e auto */
void __cdecl civ_portrait_sprites_global_init(void)

{
  civ_portrait_sprites_static_init();
  civ_portrait_sprites_reg_atexit();
  return;
}

/* 00448c58 auto */
void __cdecl civ_portrait_sprites_static_init(void)

{
  crt_vector_ctor_iterator(((i8 *)&road_sprites_block.civ_portrait_sprites),0x3c,8,sprite_init,sprite_destroy);
  return;
}

/* 00448c80 auto */
void __cdecl civ_portrait_sprites_reg_atexit(void)

{
  _atexit(civ_portrait_sprites_static_dtor);
  return;
}

/* 00448c9d auto */
void __cdecl civ_portrait_sprites_static_dtor(void)

{
  crt_vector_dtor_iterator(((i8 *)&road_sprites_block.civ_portrait_sprites),0x3c,8,sprite_destroy);
  return;
}

/* 00448f92 auto */
i32 __cdecl civ_city_style_era(iptr civ)

{
  int has_advance;
  i32 era;
  
  has_advance = civ_has_advance(civ,ADV_ELECTRONICS);
  if ((has_advance != 0) && (has_advance = civ_has_advance(civ,ADV_AUTOMOBILE), has_advance != 0)) {
    return 3;
  }
  has_advance = civ_has_advance(civ,ADV_INDUSTRIALIZATION);
  if (has_advance == 0) {
    has_advance = civ_has_advance(civ,ADV_INVENTION);
    if ((has_advance != 0) && (has_advance = civ_has_advance(civ,ADV_PHILOSOPHY), has_advance != 0)) {
      return 1;
    }
    era = 0;
  }
  else {
    era = 2;
  }
  return era;
}

/* 00453e51 auto */
i32 __cdecl civ_has_wonder(iptr civ,iptr wonder)

{
  i32 result;
  iptr city;
  
  if (((wonder == WONDER_EIFFEL_TOWER) && ((game.game_flags & GAME_SCENARIO) != 0)) && ((scenario.flags[0] & 1) != 0)) {
    result = 0;
  }
  else {
    city = wonder_get_city(wonder);
    if (city < 0) {
      result = 0;
    }
    else if (cities[city].owner == (char)civ) {
      result = 1;
    }
    else {
      result = 0;
    }
  }
  return result;
}

/* 00456f20 auto */
void __cdecl civ_adjust_attitude(iptr civ,iptr other_civ,int delta)

{
  int attitude;
  
  attitude = civ_get_attitude(civ,other_civ);
  civ_set_attitude(civ,other_civ,delta + attitude);
  if ((civ == diplo_eval.attitude_civ) && (other_civ == diplo_eval.attitude_target)) {
    diplo_eval.cur_attitude += delta;
  }
  civ_get_attitude(civ,other_civ);
  return;
}

/* 00467904 auto */
uint __cdecl civ_get_attitude(iptr civ,iptr other_civ)

{
  return (uint)civs[civ].attitude[other_civ];
}

/* 00467933 auto */
void __cdecl civ_set_attitude(iptr civ,iptr other_civ,iptr attitude)

{
  i32 clamped;
  
  if (((game.multiplayer != 1) || ((1 << ((byte)civ & 0x1f) & (uint)net.hotseat_human_players) == 0)) ||
     (human_civ[0] == civ)) {
    clamped = int_clamp(attitude,0,100);
    *(char *)&civs[civ].attitude[other_civ] = (char)clamped;
  }
  return;
}

/* 004853e7 auto */
void __cdecl civ_update_power_ranks(void)

{
  uint *treaty_ptr;
  int result;
  i32 clamped;
  uint rand_value;
  uint sign;
  uptr leader_civ;
  uptr civ;
  int unit_strength;
  int power[8];
  int rank;
  iptr city;
  iptr unit_type;
  int alert_ai;
  int hp_factor;
  int ally_count;
  uptr civ2;
  int military_power;
  int best_power;
  
  for (civ = 1; (int)civ < 8; civ++) {
    if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
      power[civ] = 0;
    }
    else if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
      power[civ] =
           (uint)civs[civ].num_advances * 3 + civs[civ].total_city_size * 8 +
           (civs[civ].gold >> 5);
      military_power = 0;
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        if ((char)unit_types[unit_type].role < 5) {
          if (unit_types[unit_type].domain == DOMAIN_LAND) {
            hp_factor = ((char)unit_types[unit_type].hit_points + 1 +
                       (int)(char)unit_types[unit_type].firepower) / 2;
          }
          else {
            hp_factor = 1;
          }
          unit_strength = (char)unit_types[unit_type].attack * hp_factor +
                     (int)(char)unit_types[unit_type].defense >> 1;
        }
        else {
          unit_strength = ((char)unit_types[unit_type].cost + 1) / 2;
        }
        military_power += (uint)civs[civ].units_active[unit_type] * unit_strength;
      }
    }
    else {
      power[civ] = 0;
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && ((int)cities[city].owner == civ)) {
          result = city_objective_value(city);
          power[civ] += result * 2;
        }
      }
    }
    if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
      if (game.turn < 600) {
        clamped = int_clamp(power[civ] >> 3,0,0xff);
        (((i8 *)&power_graph_history_block.power_graph_history))
        [(int)(CONCAT44((int)game.turn >> 0x1f,(int)game.turn >> 2) % 0x96) * 8 + civ] =
             (char)clamped;
      }
    }
    else if (game.turn < 0x4b) {
      clamped = int_clamp(power[civ],0,0xff);
      (((i8 *)&power_graph_history_block.power_graph_history))
      [(int)(CONCAT44((int)game.turn >> 0x1f,(int)game.turn >> 1) % 0x96) * 8 + civ] =
           (char)clamped;
    }
  }
  for (rank = 1; rank < 8; rank++) {
    best_power = -1;
    civ = 0;
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (best_power < power[civ2]) {
        civ = civ2;
        best_power = power[civ2];
      }
    }
    game.power_rank[civ] = 8 - (char)rank;
    power[civ] = -1;
  }
  game.game_flags = game.game_flags & ~GAME_AI_GANG_UP;
  for (civ = 0; (int)civ < 8; civ++) {
    game.civ_by_rank[civ] = '\0';
  }
  for (civ = 1; (int)civ < 8; civ++) {
    if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) {
      game.civ_by_rank[game.power_rank[civ]] = (byte)civ;
    }
  }
  game.best_human_rank = '\0';
  game.worst_human_rank = '\0';
  for (civ = 7; -1 < (int)civ; civ--) {
    if (((uint)game.human_players & 1 << (game.civ_by_rank[civ] & 0x1f)) != 0) {
      game.best_human_rank = (byte)civ;
      break;
    }
  }
  for (civ = 0; (int)civ < 8; civ++) {
    if (((uint)game.human_players & 1 << (game.civ_by_rank[civ] & 0x1f)) != 0) {
      game.worst_human_rank = (byte)civ;
      break;
    }
  }
  alert_ai = 0;
  civ = (uptr)game.civ_by_rank[7];
  if ((((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0) &&
       (4 < civs[game.civ_by_rank[7]].num_cities)) &&
      (civs[game.civ_by_rank[7]].units_active[UNIT_NUCLEAR_MISSILE] == '\0')) &&
     ((200 < game.turn && (game.difficulty != DIFF_CHIEFTAIN)))) {
    alert_ai = 1;
  }
  if (DIFF_KING < game.difficulty) {
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
         (result = civ_spaceship_launched(civ2), result != 0)) {
        civ = civ2;
        alert_ai = 1;
      }
    }
  }
  leader_civ = civ;
  if (alert_ai != 0) {
    game.game_flags = game.game_flags | GAME_AI_GANG_UP;
    civ2 = civ;
    ally_count = 0;
    for (civ = 1; (int)civ < 8; civ++) {
      if (((byte)(civs[leader_civ].treaties[civ] >> 8) & (TREATY_WAR >> 8)) != 0) {
        ally_count++;
      }
    }
    for (civ = 1; (int)civ < 8; civ++) {
      if (((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
           ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) &&
          (1 < civs[civ].num_cities)) &&
         (((ally_count * 3 + 3 < (int)(uint)civs[civ2].reputation ||
           ((*(byte *)&civs[civ].treaties[civ2] & TREATY_VENDETTA) != 0)) ||
          (result = spaceship_race_active(), result != 0)))) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
          if ((*(byte *)&civs[civ].treaties[civ2] & (TREATY_CEASEFIRE | TREATY_PEACE)) == 0) {
            civ_adjust_attitude(civ,civ2,1);
            treaty_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
            *treaty_ptr = *treaty_ptr | 1;
          }
          else {
            rand_value = _rand();
            sign = (int)rand_value >> 0x1f;
            if (((int)(((rand_value ^ sign) - sign & 0x1f ^ sign) - sign) <=
                 (int)(uint)game.difficulty) ||
               (result = civ_spaceship_launched(civ), result != 0)) {
              treaty_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
              *treaty_ptr = *treaty_ptr | 0x20;
            }
          }
        }
        else {
          result = civ_spaceship_launched(civ);
          if ((result != 0) && (DIFF_KING < game.difficulty)) {
            treaty_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
            *treaty_ptr = *treaty_ptr | 0x20;
          }
        }
      }
    }
  }
  return;
}

/* 00486e15 auto */
int __cdecl civ_throne_upgrade_threshold(int level)

{
  int i;
  int sum;
  
  sum = 0;
  for (i = 0; i <= level; i++) {
    sum += (7 - (uint)game.difficulty) * i;
  }
  return sum / 2 + 1;
}

/* 00486e6f auto */
void __cdecl civ_check_throne_room_upgrade(void)

{
  bool has_palace;
  int score;
  int threshold;
  iptr civ;
  iptr city;
  
  civ = human_civ[0];
  if (((game.game_flags & GAME_SCENARIO) == 0) && (game.multiplayer != 1)) {
    has_palace = false;
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
         (score = city_has_improvement(city,IMP_PALACE), score != 0)) {
        has_palace = true;
        break;
      }
    }
    if ((has_palace) && (prefs.accelerated_start * 0x14 + 1 < (int)game.turn)) {
      score = civ_calc_score(civ);
      threshold = civ_throne_upgrade_threshold(civs[civ].throne_room_upgrades + 1);
      if ((threshold <= score) &&
         ((civs[civ].throne_room_upgrades = civs[civ].throne_room_upgrades + 1,
          civs[civ].throne_room_upgrades < 0x26 && (((*((char *)&game.game_options + 2)) & (OPT_THRONE_ROOM >> 16)) != 0)))) {
        throneroom_show_upgrade(human_civ[0]);
      }
    }
  }
  return;
}

/* 00487a41 auto */
void __cdecl civ_process_cities_turn(uptr civ)

{
  char attack_trait;
  bool needs_expansion;
  int value;
  i32 clamped;
  int result;
  int govt;
  iptr other_civ;
  iptr city;
  iptr continent;
  uint city_status;
  int lux_slack;
  iptr wonder;
  int max_rate;
  int tech_bonus;
  
  city_status = 0;
  civs[civ].best_shield_surplus = 0;
  for (govt = 0; govt < 7; govt++) {
    civs[civ].government_score[govt] = 0;
  }
  for (wonder = 0; wonder < 0x1c; wonder++) {
    (((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] = 0;
  }
  game.transient_flags = game.transient_flags & ~TRANSIENT_ATTITUDE_DIRTY;
  ((CityWindow *)((i8 *)&city_wnd.window))->begin_notice();
  city = (iptr)game.num_cities;
  while( true ) {
    city--;
    if (city < 0) break;
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
    if ((cities[city].id != 0) && ((int)cities[city].owner == (civ & 0xff))) {
      cities[city].flags = cities[city].flags & ~CITYF_AIRLIFTED;
      value = city_process_turn(city);
      if (value != -999) {
        if ((value == 0) && (5 < cities[city].size)) {
          city_status = city_status | 1;
        }
        if ((cities[city].flags & CITYF_DISORDER) != 0) {
          city_status = city_status | 2;
        }
        for (other_civ = 1; other_civ < 8; other_civ++) {
          if (((*(byte *)&civs[civ].treaties[other_civ] & TREATY_ALLIANCE) != 0) ||
             (game.wonder_city[WONDER_APOLLO] != -1)) {
            *(i8 *)&cities[city].revealed_size[other_civ] = cities[city].size;
          }
        }
        value = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
        if ((value == city) && (city_calc.zoom_requested == 0)) {
          ((CityWindow *)((i8 *)&city_wnd.window))->refresh();
        }
      }
    }
  }
  ((CityWindow *)((i8 *)&city_wnd.window))->end_notice();
  civs[civ].government_score[GOV_ANARCHY] =
       civs[civ].government_score[GOV_ANARCHY] + civs[civ].government_score[GOV_DESPOTISM] * 2;
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    value = -((uint)civs[civ].science_rate + (uint)civs[civ].tax_rate);
    lux_slack = value + 10;
    if (((city_status & 2) != 0) && (lux_slack < 4)) {
      lux_slack = value + 0xb;
    }
    if ((((game.turn & 3U) == 0) && (city_status == 0)) && (2 < lux_slack)) {
      lux_slack--;
    }
    value = int_clamp(lux_slack,0,4);
    civs[civ].science_rate = leaders[civs[civ].tribe].civilize + (char)(10 - value >> 1);
    if (game.turn + 100 < civs[civ].gold) {
      civs[civ].science_rate++;
    }
    if (2000 < civs[civ].gold) {
      civs[civ].science_rate++;
    }
    if (8000 < civs[civ].gold) {
      civs[civ].science_rate = '\n' - (char)value;
    }
    needs_expansion = false;
    for (continent = 1; continent < 0x40; continent++) {
      if (*(char *)&civs[civ].continent_cities[continent] != '\0') {
        if ((*(char *)&civs[civ].continent_policy[continent] == '\0') ||
           (*(char *)&civs[civ].continent_policy[continent] == 1)) {
          needs_expansion = true;
        }
        if ((*(char *)&civs[civ].continent_policy[continent] == 5) &&
           (*(char *)&civs[civ].continent_flags[continent] != '\0')) {
          needs_expansion = true;
        }
      }
    }
    if ((!needs_expansion) && (civs[civ].science_rate < 7)) {
      civs[civ].science_rate++;
    }
    max_rate = 6;
    if (GOV_DESPOTISM < civs[civ].government) {
      max_rate = 7;
    }
    if (GOV_FUNDAMENTALISM < civs[civ].government) {
      max_rate = 8;
    }
    if (GOV_REPUBLIC < civs[civ].government) {
      max_rate = 10;
    }
    result = 10 - value;
    if (max_rate <= 10 - value) {
      result = max_rate;
    }
    clamped = int_clamp((uint)civs[civ].science_rate,0,result);
    civs[civ].science_rate = (u8)clamped;
    result = civ_has_advance(civ,ADV_FUSION_POWER);
    if ((((result != 0) && (result = civ_has_advance(civ,ADV_LASER), result != 0)) &&
        (result = civ_has_advance(civ,(int)improvements[IMP_SS_STRUCTURAL].preq), result != 0)) &&
       ((result = civ_has_advance(civ,(int)improvements[IMP_SS_COMPONENT].preq), result != 0 &&
        (result = civ_has_advance(civ,(int)improvements[IMP_SS_MODULE].preq), result != 0)))) {
      civs[civ].science_rate = '\0';
    }
    civs[civ].tax_rate = '\n' - (civs[civ].science_rate + (char)value);
  }
  tech_bonus = 0;
  value = civ_has_advance(civ,ADV_DEMOCRACY);
  if (((value != 0) || (value = civ_has_wonder(civ,WONDER_STATUE_OF_LIBERTY), value != 0)) &&
     (civs[game.best_human_rank].government == GOV_FUNDAMENTALISM)) {
    tech_bonus = civs[game.best_human_rank].subversion_count + 1;
  }
  civs[civ].government_score[GOV_DEMOCRACY] =
       ((ushort)civs[game.best_human_rank].units_active[UNIT_DIPLOMAT] +
        (ushort)civs[game.best_human_rank].units_active[UNIT_SPY] + civs[game.best_human_rank].subversion_count) *
       (civs[game.best_human_rank].subversion_count + (short)tech_bonus) + civs[civ].government_score[GOV_DEMOCRACY];
  if (civs[civ].gold < civs[game.best_human_rank].gold) {
    civs[civ].government_score[GOV_DEMOCRACY] += 10;
  }
  if ((tech_bonus == 0) &&
     ((((value = civ_has_advance(civ,ADV_COMMUNISM), value != 0 ||
        (value = civ_has_advance(civ,ADV_FUNDAMENTALISM), value != 0)) ||
       (value = civ_has_wonder(civ,WONDER_STATUE_OF_LIBERTY), value != 0)) &&
      ((attack_trait = leaders[civs[civ].tribe].attack, value = spaceship_race_active(),
       (int)(uint)game.power_rank[civ] <= (int)((int)attack_trait + (4 - (uint)(value == 0))) &&
       (civs[civ].num_advances < civs[game.best_human_rank].num_advances)))))) {
    civs[civ].government_score[GOV_DEMOCRACY] = 0;
    civs[civ].government_score[GOV_REPUBLIC] = 0;
  }
  if (-1 < leaders[civs[civ].tribe].attack) {
    if ((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) == 0) &&
       ((leaders[civs[civ].tribe].attack == 1 ||
        ((leaders[civs[civ].tribe].attack == '\0' &&
         (leaders[civs[civ].tribe].civilize < '\0')))))) {
      value = civs[civ].government_score[GOV_DEMOCRACY] * 3;
      civs[civ].government_score[GOV_DEMOCRACY] = (i16)((int)(value + (value >> 0x1f & 3U)) >> 2);
      value = civs[civ].government_score[GOV_REPUBLIC] * 3;
      civs[civ].government_score[GOV_REPUBLIC] = (i16)((int)(value + (value >> 0x1f & 3U)) >> 2);
    }
    if ((leaders[civs[civ].tribe].attack != '\0') ||
       ((leaders[civs[civ].tribe].civilize < '\0' && (tech_bonus == 0)))) {
      value = leaders[civs[civ].tribe].attack * 2 - (int)leaders[civs[civ].tribe].civilize;
      if (((game.power_rank[game.best_human_rank] - 1) <= (int)(uint)game.power_rank[civ]) ||
         (civs[game.best_human_rank].num_advances <= civs[civ].num_advances)) {
        if ((int)((uint)civs[civ].num_advances + value * 4) <
            (int)(uint)civs[game.best_human_rank].num_advances) {
          if ((int)(uint)civs[game.best_human_rank].num_advances <=
              (int)((uint)civs[civ].num_advances + value * 8)) {
            civs[civ].government_score[GOV_DEMOCRACY] = civs[civ].government_score[GOV_DEMOCRACY] / 2;
            civs[civ].government_score[GOV_REPUBLIC] = civs[civ].government_score[GOV_REPUBLIC] / 2;
          }
        }
        else {
          civs[civ].government_score[GOV_DEMOCRACY] = 0;
          civs[civ].government_score[GOV_REPUBLIC] = 0;
        }
      }
    }
  }
  value = civ_spaceship_launched(civ);
  if ((value != 0) && (DIFF_KING < game.difficulty)) {
    civs[civ].government_score[GOV_DEMOCRACY] = civs[civ].government_score[GOV_DEMOCRACY] / 2;
    civs[civ].government_score[GOV_REPUBLIC] = civs[civ].government_score[GOV_REPUBLIC] / 2;
  }
  if ((((game.rules & RULE_BLOODLUST) != 0) && (civs[civ].science_rate == '\0')) &&
     (civs[civ].government_score[GOV_DEMOCRACY] = 0, -1 < leaders[civs[civ].tribe].attack)) {
    civs[civ].government_score[GOV_REPUBLIC] = 0;
  }
  for (wonder = 0; wonder < 0x1c; wonder++) {
    (((i8 *)&advisor_window_block.wonder_notice_flags))[civ * 0x1c + wonder] = 0;
  }
  return;
}

/* 00488cef auto */
void __cdecl civ_heal_units(iptr civ)

{
  u8 domain;
  ushort old_flags;
  bool full_heal;
  uint heal;
  iptr max_hp;
  iptr max_hp2;
  i32 value;
  int city;
  int has_barracks;
  iptr civ2;
  iptr unit;
  iptr improvement;
  uint heal_amount;
  
  /* BF-028 (Peter Johnson's ledger): a unit type of another domain than land,
     air or sea (a modified RULES.TXT) left the facility unset for the first
     unit; 0 is none (later units keep the one before, as in the original) */
  improvement = 0;
  for (unit = 0; unit < game.num_units; unit++) {
    if (2 < game.multiplayer) {
      net.process_messages(1,0);
    }
    if ((units[unit].id != 0) && (units[unit].owner == civ)) {
      old_flags = units[unit].flags;
      units[unit].flags = units[unit].flags & ~(UNITF_PARADROPPED | UNITF_MOVED);
      if ((old_flags & UNITF_MOVED) == 0) {
        if (units[unit].damage != '\0') {
          heal_amount = 1;
          civ2 = -1;
          value = tile_landmass((int)units[unit].x,(int)units[unit].y);
          city = city_find_nearest((int)units[unit].x,(int)units[unit].y,civ,value,
                                     civ2);
          if (-1 < city) {
            advisor_window_block.nearest_city_dist = map_distance((int)units[unit].x,(int)units[unit].y,
                                              (int)cities[city].x,(int)cities[city].y);
          }
          value = tile_improvements((int)units[unit].x,(int)units[unit].y);
          if (((byte)value & 0x42) == 0x40) {
            heal_amount = 2;
          }
          full_heal = false;
          heal = heal_amount;
          if ((advisor_window_block.nearest_city_dist < 4) && (unit_types[units[unit].type].domain == DOMAIN_LAND)) {
            has_barracks = city_has_improvement(city,IMP_BARRACKS);
            heal = heal_amount + 1;
            if (has_barracks != 0) {
              heal = heal_amount + 2;
            }
          }
          heal_amount = heal;
          if (advisor_window_block.nearest_city_dist == 0) {
            domain = unit_types[units[unit].type].domain;
            if (domain == DOMAIN_LAND) {
              improvement = 2;
            }
            else if (domain == DOMAIN_AIR) {
              improvement = 0x20;
            }
            else if (domain == DOMAIN_SEA) {
              improvement = 0x22;
            }
            city = city_has_improvement(city,improvement);
            if ((city != 0) &&
               ((unit_types[units[unit].type].domain == DOMAIN_LAND || (advisor_window_block.nearest_city_dist == 0)))) {
              full_heal = true;
              heal_amount = heal_amount << 1;
            }
          }
          if (advisor_window_block.nearest_city_dist == 0) {
            heal_amount = heal_amount << 1;
          }
          city = unit_max_hp(unit);
          heal_amount = (city / 10) * heal_amount;
          if ((full_heal) && (advisor_window_block.nearest_city_dist == 0)) {
            heal_amount = (uint)units[unit].damage;
          }
          if ((unit_types[units[unit].type].domain == DOMAIN_AIR) &&
             (city = tile_airbase_owner((int)units[unit].x,(int)units[unit].y), 0 < city))
          {
            heal_amount = (uint)units[unit].damage;
          }
          heal = (uint)units[unit].damage;
          if ((int)heal_amount <= (int)(uint)units[unit].damage) {
            heal = heal_amount;
          }
          units[unit].damage -= (char)heal;
          if (((units[unit].damage == '\0') && (units[unit].orders == ORDER_SLEEP)) &&
             ((unit_types[units[unit].type].domain != DOMAIN_LAND ||
              (city = tile_is_ocean((int)units[unit].x,(int)units[unit].y), city == 0))))
          {
            units[unit].orders = ORDER_NONE;
          }
          if ((((*(i8 *)&human_civ) == civ) ||
              ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) != 0)) ||
             ((int)units[unit].owner == ((*(i8 *)&human_civ) & 0xffU))) {
            map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
          }
        }
      }
      else if (((unit_types[units[unit].type].domain == DOMAIN_AIR) &&
               (unit_types[units[unit].type].range == '\0')) &&
              (heal = tile_improvements((int)units[unit].x,(int)units[unit].y),
              (heal & 2) == 0)) {
        max_hp = unit_max_hp(unit);
        max_hp2 = unit_max_hp(unit);
        value = int_clamp((uint)units[unit].damage + max_hp / 10,0,max_hp2 - 1);
        units[unit].damage = (u8)value;
      }
    }
  }
  return;
}

/* 00489292 auto */
void __cdecl civ_check_pop_and_funds(iptr civ,int prev_gold)

{
  short old_milestone;
  int total_pop;
  iptr adjective;
  int milestone_pop;
  
  milestone_pop = -1;
  total_pop = civ_total_population(civ);
  old_milestone = civs[civ].population_milestone;
  if (total_pop < 100) {
    if (civs[civ].population_milestone * 10 < total_pop) {
      milestone_pop = (total_pop / 10) * 10;
      civs[civ].population_milestone = (i16)(total_pop / 10);
    }
  }
  else if ((civs[civ].population_milestone - 9) * 100 < total_pop) {
    milestone_pop = (total_pop / 100) * 100;
    civs[civ].population_milestone = (short)(total_pop / 100) + 9;
  }
  if (((old_milestone < civs[civ].population_milestone) &&
      ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) && (autoplay.active == 0)) {
    adjective = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)adjective);
    text_clear();
    text_append_population(civ,milestone_pop);
    text_set_string_arg(2,(const char *)&textbuf.buf);
    popup_game_with_sprite((const char *)&str_fertile,(iptr)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c),0);
  }
  if ((((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (autoplay.active == 0)) &&
       ((turn_loop.active != '\0' && ((1 < civs[civ].num_cities && (civs[civ].gold < 100))))))
      && ((civs[civ].gold - prev_gold) * 10 + civs[civ].gold < 0)) &&
     (total_pop = popup_game_with_sprite((const char *)&str_taxes,(iptr)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c),0), total_pop != 0)) {
    taxrate_dialog_show(civ);
  }
  return;
}

/* 00489553 auto */
void __cdecl civ_process_turn(uptr civ)

{
  i32 start_gold;
  int result;
  int part;
  int parts_delta;
  
  parts_delta = 0;
  turn_flags.civ_turn_in_progress = 1;
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    result = civ_spaceship_exists(civ);
    if (result != 0) {
      for (part = 0; part < 6; part++) {
        parts_delta += civs[civ].spaceship_parts[part];
      }
    }
  }
  spaceship_wnd_load_parts(civ);
  ((PictureWindow *)((i8 *)&advisor_state.window))->close();
  if (extra_rule(X_GOLD_LIMIT)) {
    if (extras_gold_limit() < civs[civ].gold) {
      civs[civ].gold = extras_gold_limit();  /* X52 */
    }
  }
  else if (30000 < civs[civ].gold) {
    civs[civ].gold = 30000;
  }
  if (civs[civ].gold < -0x4000) {
    civs[civ].gold = extra_rule(X_GOLD_LIMIT) ? extras_gold_limit() : 30000;
  }
  if (civs[civ].gold < 0) {
    civs[civ].gold = 0;
  }
  start_gold = civs[civ].gold;
  civ_heal_units(civ);
  civ_process_cities_turn(civ);
  civ_turn_update_diplomacy(civ);
  ai_civ_turn_assess(civ);
  civ_check_pop_and_funds(civ,start_gold);
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    result = civ_spaceship_exists(civ);
    if (result != 0) {
      for (part = 0; part < 6; part++) {
        parts_delta -= civs[civ].spaceship_parts[part];
      }
    }
  }
  if (parts_delta != 0) {
    result = spaceship_wnd_show_for_civ(civ);
    if (result == 0) {
      spaceship_status_dialog(civ,(uint)game.human_players & 1 << ((byte)civ & 0x1f));
    }
  }
  turn_flags.civ_turn_in_progress = 0;
  if (((*((char *)&game.game_options + 1)) & (OPT_AUTOSAVE >> 8)) != 0) {
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    game_autosave(civ);
    __chdir(((i8 *)&game_dirs.home));
  }
  return;
}

/* 00492b60 auto */
void __cdecl civ_ai_goal_negate_priority(int civ,int goal)

{
  char abs_priority;
  
  if (civs[civ].ai_goals[goal].priority < 1) {
    abs_priority = ~*(byte *)&civs[civ].ai_goals[goal].priority + 1;
  }
  else {
    abs_priority = civs[civ].ai_goals[goal].priority;
  }
  civs[civ].ai_goals[goal].priority = -abs_priority;
  return;
}

/* 00492c15 auto */
void __cdecl
civ_clear_standing_goals_near(iptr civ,i32 role,i32 x,i32 y,int max_dist)

{
  int dist;
  int goal;
  
  for (goal = 0; goal < 0x10; goal++) {
    if (civs[civ].ai_standing_goals[goal].role == (char)role) {
      dist = map_distance_diag(x,y,
                                 (int)civs[civ].ai_standing_goals[goal].x,
                                 (int)civs[civ].ai_standing_goals[goal].y);
      if (dist <= max_dist) {
        civs[civ].ai_standing_goals[goal].role = 0xff;
        civs[civ].ai_standing_goals[goal].priority = 0;
      }
    }
  }
  return;
}

/* 00492d18 auto */
void __cdecl civ_ai_goal_insert_gap(iptr civ,iptr goal)

{
  i16 goal_y;
  i8 priority;
  
  if ((goal < 0x2f) && (civs[civ].ai_goals[goal].role != -1)) {
    civ_ai_goal_insert_gap(civ,goal + 1);
    goal_y = civs[civ].ai_goals[goal].y;
    civs[civ].ai_goals[goal + 1].x = civs[civ].ai_goals[goal].x;
    civs[civ].ai_goals[goal + 1].y = goal_y;
    priority = civs[civ].ai_goals[goal].priority;
    civs[civ].ai_goals[goal + 1].role = civs[civ].ai_goals[goal].role;
    civs[civ].ai_goals[goal + 1].priority = priority;
  }
  return;
}

/* 00492dd0 auto */
void __cdecl civ_standing_goal_insert_gap(iptr civ,int start)

{
  i16 goal_y;
  i8 priority;
  iptr goal;
  
  for (goal = 0xe; start <= goal; goal--) {
    goal_y = civs[civ].ai_standing_goals[goal].y;
    civs[civ].ai_standing_goals[goal + 1].x = civs[civ].ai_standing_goals[goal].x;
    civs[civ].ai_standing_goals[goal + 1].y = goal_y;
    priority = civs[civ].ai_standing_goals[goal].priority;
    civs[civ].ai_standing_goals[goal + 1].role =
         civs[civ].ai_standing_goals[goal].role;
    civs[civ].ai_standing_goals[goal + 1].priority = priority;
  }
  return;
}

/* 00492e60 auto */
int __cdecl civ_ai_goal_get_priority(iptr civ,int x,int y,i32 role)

{
  int abs_priority;
  i32 best_priority;
  int goal;
  
  best_priority = 0;
  for (goal = 0; goal < 0x30; goal++) {
    if (((civs[civ].ai_goals[goal].x == x) &&
        (civs[civ].ai_goals[goal].y == y)) &&
       (civs[civ].ai_goals[goal].role == (char)role)) {
      if (civs[civ].ai_goals[goal].priority < 1) {
        abs_priority = ~(int)civs[civ].ai_goals[goal].priority + 1;
      }
      else {
        abs_priority = (int)civs[civ].ai_goals[goal].priority;
      }
      if ((char)best_priority <= abs_priority) {
        if (civs[civ].ai_goals[goal].priority < 1) {
          best_priority = ~(int)civs[civ].ai_goals[goal].priority + 1;
        }
        else {
          best_priority = (int)civs[civ].ai_goals[goal].priority;
        }
      }
    }
  }
  return best_priority;
}

/* 0049301b auto */
void __cdecl civ_add_ai_goal(iptr civ,int x,int y,int role,int priority)

{
  int value;
  int value2;
  iptr unit;
  int goal;
  
  goal = 0;
  while( true ) {
    if (0x2f < goal) {
      if (((game.turn_civ == civ) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) &&
         ((role == 2 || (role == 3)))) {
        for (unit = 0; unit < game.num_units; unit++) {
          if ((((units[unit].id != 0) && (units[unit].owner == civ)) &&
              (value = unit_is_awake_with_moves(unit), value != 0)) &&
             ((char)unit_types[units[unit].type].role == role)) {
            value = tile_landmass((int)units[unit].x,(int)units[unit].y);
            value2 = tile_landmass(x,y);
            if (value == value2) {
              value = map_distance_diag(x,y,(int)units[unit].x,
                                         (int)units[unit].y);
              value2 = unit_moves_left(unit);
              if (value * 2 <= value2) {
                units[unit].orders = ORDER_GOTO;
                units[unit].goto_x = (i16)x;
                units[unit].goto_y = (i16)y;
              }
            }
          }
        }
      }
      goal = 0;
      while( true ) {
        if (0x2f < goal) {
          return;
        }
        if ((civs[civ].ai_goals[goal].priority < (char)priority) ||
           (civs[civ].ai_goals[goal].role == -1)) break;
        goal++;
      }
      civ_ai_goal_insert_gap(civ,goal);
      civs[civ].ai_goals[goal].x = (i16)x;
      civs[civ].ai_goals[goal].y = (i16)y;
      civs[civ].ai_goals[goal].role = (char)role;
      civs[civ].ai_goals[goal].priority = (char)priority;
      return;
    }
    if (((civs[civ].ai_goals[goal].x == x) &&
        (civs[civ].ai_goals[goal].y == y)) &&
       ((civs[civ].ai_goals[goal].role == (char)role &&
        ((char)priority <= civs[civ].ai_goals[goal].priority)))) break;
    goal++;
  }
  return;
}

/* 004933f2 auto */
void __cdecl civ_add_standing_goal(iptr civ,int x,int y,i32 role,int priority)

{
  int goal;
  
  goal = 0;
  while( true ) {
    if (0xf < goal) {
      goal = 0;
      while( true ) {
        if (0xf < goal) {
          return;
        }
        if ((civs[civ].ai_standing_goals[goal].priority < (char)priority) ||
           (civs[civ].ai_standing_goals[goal].role == -1)) break;
        goal++;
      }
      civ_standing_goal_insert_gap(civ,goal);
      civs[civ].ai_standing_goals[goal].x = (short)x;
      civs[civ].ai_standing_goals[goal].y = (short)y;
      civs[civ].ai_standing_goals[goal].role = (char)role;
      civs[civ].ai_standing_goals[goal].priority = (char)priority;
      return;
    }
    if ((((civs[civ].ai_standing_goals[goal].x == x) &&
         (civs[civ].ai_standing_goals[goal].y == y)) &&
        (civs[civ].ai_standing_goals[goal].role == (char)role)) &&
       ((char)priority <= civs[civ].ai_standing_goals[goal].priority)) break;
    goal++;
  }
  return;
}

/* 00493602 auto */
void __cdecl civ_refresh_ai_goals(iptr civ)

{
  int goal;
  
  for (goal = 0; goal < 0x30; goal++) {
    if (civs[civ].ai_goals[goal].priority < '\0') {
      civs[civ].ai_goals[goal].role = 0xff;
    }
    civ_ai_goal_negate_priority(civ,goal);
  }
  for (goal = 0; goal < 0x10; goal++) {
    if (-1 < civs[civ].ai_standing_goals[goal].role) {
      civ_add_ai_goal(civ,(int)civs[civ].ai_standing_goals[goal].x,
                         (int)civs[civ].ai_standing_goals[goal].y,
                         (int)civs[civ].ai_standing_goals[goal].role,
                         (int)civs[civ].ai_standing_goals[goal].priority);
    }
  }
  return;
}

/* 0049376f auto */
void __cdecl civ_reset_standing_goals(iptr civ)

{
  int goal;
  
  for (goal = 0; goal < 0x10; goal++) {
    civs[civ].ai_standing_goals[goal].role = 0xff;
    civs[civ].ai_standing_goals[goal].priority = 0;
  }
  return;
}

/* 00493b10 auto */
char * __cdecl civ_leader_name(iptr civ)

{
  int name2;
  
  if (civ < 1) {
    name2 = (iptr)label_string(label_table.string_ids[0x11]);
  }
  else if (leaders[civs[civ].tribe].leader_name < 0) {
    name2 = civ * 0xf2 + (iptr)((u8 *)&civ_names + 2);
  }
  else {
    name2 = (iptr)label_string((int)leaders[civs[civ].tribe].leader_name);
  }
  return (char *)name2;
}

/* 00493ba6 auto */
int __cdecl civ_leader_title(iptr civ)

{
  int title;
  
  if (civ < 1) {
    title = (iptr)label_string(government_titles[2].male);
  }
  else {
    title = (int)leaders[civs[civ].tribe].titles[((uint)civs[civ].government)][((uint)leaders[civs[civ].tribe].female)];
    if (title < 0) {
      title = civ * 0xf2 + (iptr)((u8 *)&civ_names + 74) + (uint)civs[civ].government * 0x18;
    }
    else {
      title = (iptr)label_string(title);
    }
  }
  return title;
}

/* 00493c7d auto */
char * __cdecl civ_name_plural(iptr civ)

{
  int name2;
  
  if (civ < 1) {
    name2 = (iptr)label_string(label_table.string_ids[0xf]);
  }
  else if (leaders[civs[civ].tribe].plural < 0) {
    name2 = civ * 0xf2 + (iptr)((u8 *)&civ_names + 26);
  }
  else {
    name2 = (iptr)label_string((int)leaders[civs[civ].tribe].plural);
  }
  return (char *)name2;
}

/* 00493d13 auto */
int __cdecl civ_adjective(iptr civ)

{
  int adjective;
  
  if (civ < 1) {
    adjective = (iptr)label_string(label_table.string_ids[0x10]);
  }
  else if (leaders[civs[civ].tribe].adjective < 0) {
    adjective = civ * 0xf2 + (iptr)((u8 *)&civ_names + 50);
  }
  else {
    adjective = (iptr)label_string((int)leaders[civs[civ].tribe].adjective);
  }
  return adjective;
}

/* 00494dae auto */
void __cdecl advance_count_prereq_depth(iptr advance)

{
  iptr preq_index;
  
  if (tech_prereq_walk_count < 0x65) {
    for (preq_index = 0; preq_index < 2; preq_index++) {
      if ('\0' < advances[advance].preq[preq_index]) {
        tech_prereq_walk_count++;
        advance_count_prereq_depth((int)advances[advance].preq[preq_index]);
      }
    }
  }
  return;
}

/* 00498310 auto */
void __cdecl civ_password_ensure_set(int civ)

{
  char password[32];
  
  mp_turn_timer_stop(1);
  passwords_unscramble();
  crt_strcpy(password,((i8 *)&civ_saved_cursor_x_block.civ_passwords) + civ * 0x20);
  passwords_scramble();
  if (password[0] == '\0') {
    civ_change_password_dialog(civ);
  }
  return;
}

/* 0049836a auto */
void __cdecl civ_change_password_dialog(iptr civ)

{
  char entered[32];
  Dialog dialog2;
  char current_password[32];
  int result;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  mp_turn_timer_stop(1);
  ON_SCOPE_EXIT(
    passwords_refresh_flags();
    map_views_block.civ_password_prompt_pending[civ] = 0;
    mp_turn_timer_resume(1));
  passwords_unscramble();
  crt_strcpy(current_password,((i8 *)&civ_saved_cursor_x_block.civ_passwords) + civ * 0x20);
  passwords_scramble();
  if (current_password[0] == '\0') {
    dialog2.load_text_section((const char *)&str_game_7,(const char *)&str_password1_3,0x1f,(iptr)&str_empty_112,0,0,0,1);
    password_dialog_set_title(&dialog2,civ,0x26c);
    result = dialog2.run_modal((iptr)entered,0);
  }
  else {
    dialog2.load_text_section((const char *)&str_game_7,(const char *)&str_password1,0x1f,(iptr)&str_empty_110,0,0,0,1);
    password_dialog_set_title(&dialog2,civ,0x26d);
    result = dialog2.run_modal((iptr)entered,0);
    if (result < 0) return;
    result = _strcmp(entered,current_password);
    if (result != 0) {
      popup_game((const char *)&str_password0);
      return;
    }
    dialog2.load_text_section((const char *)&str_game_7,(const char *)&str_password1_2,0x1f,(iptr)&str_empty_111,0,0,0,1);
    password_dialog_set_title(&dialog2,civ,0x26c);
    result = dialog2.run_modal((iptr)entered,0);
  }
  if (-1 < result) {
    passwords_unscramble();
    crt_strcpy(((i8 *)&civ_saved_cursor_x_block.civ_passwords) + civ * 0x20,entered);
    crt_strcpy(current_password,entered);
    passwords_scramble();
    if (current_password[0] != '\0') {
      popup_game((const char *)&str_password4);
    }
  }
  return;
}

/* 004985f4 auto */
i32 __cdecl civ_password_prompt(int civ)

{
  i32 status;
  char entered[32];
  Dialog dialog2;
  int result;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  dialog2.load_text_section((const char *)&str_game_7,(const char *)&str_password1_4,0x1f,(iptr)&str_empty_113,0,0,0,1);
  password_dialog_set_title(&dialog2,civ,0x26b);
  entered[0] = '\0';
  dlg_style.pending_idle_callback = net_pump_idle_cb;
  result = dialog2.run_modal((iptr)entered,0);
  if (result < 0) {
    status = 2;
  }
  else {
    passwords_unscramble();
    result = _strcmp(entered,((i8 *)&civ_saved_cursor_x_block.civ_passwords) + civ * 0x20);
    passwords_scramble();
    if (result == 0) {
      status = 0;
    }
    else {
      dlg_style.pending_idle_callback = net_pump_idle_cb;
      popup_game((const char *)&str_password0_2);
      status = 1;
    }
  }
  return status;
}

/* 00498a5c auto */
i32 __cdecl civ_check_password(iptr civ)

{
  int prompt_result;
  
  if (map_views_block.civ_password_prompt_pending[civ] == 0) {
    if (game.multiplayer == 2) {
      civ_password_ensure_set(civ);
    }
    else {
      turn_loop.active = 1;
    }
  }
  else {
    do {
      turn_loop.active = 1;
      prompt_result = civ_password_prompt(civ);
      if (prompt_result == 0) {
        map_views_block.civ_password_prompt_pending[civ] = 0;
        return 1;
      }
    } while (prompt_result == 1);
    if (prompt_result == 2) {
      turn_loop.active = 0;
      return 0;
    }
  }
  return 1;
}

/* 004a28b0 auto */
int __cdecl civ_calc_score(iptr civ)

{
  int value;
  int end_turn;
  iptr city;
  iptr wonder;
  int outcome;
  
  civ_score.citizens = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      civ_score.citizens = civ_score.citizens +
                     (((int)cities[city].size + (int)(char)cities[city].happy) -
                     (int)(char)cities[city].unhappy);
    }
  }
  civ_score.wonders = 0;
  for (wonder = 0; wonder < 0x1c; wonder++) {
    if ((-1 < game.wonder_city[wonder]) && (cities[game.wonder_city[wonder]].owner == civ)) {
      civ_score.wonders += 0x14;
    }
  }
  civ_score.spaceship_in_flight = 0;
  civ_score.spaceship_arrived = 0;
  spaceship_calc_stats(civ,1);
  if (((spaceship_stats.success_pct != 0) && (civs[civ].spaceship_population != 0)) &&
     (value = civs[civ].spaceship_population * spaceship_stats.success_pct, civ_score.spaceship_in_flight = value,
     (civs[civ].spaceship_flags & SHIP_WON) != 0)) {
    civ_score.spaceship_in_flight = 0;
    civ_score.spaceship_arrived = value;
  }
  civ_score.pollution = ((int)game.polluted_tiles - (int)game.warming_pollution) * -10;
  civ_score.peace = 0;
  if (199 < game.turn) {
    civ_score.peace = int_clamp(game.peace_turns * 3,0,100);
  }
  civ_score.future_techs = (uint)civs[civ].future_techs * 5;
  civ_score.barbarian_bonus = (uint)game.barbarian_activity * 0x19 - 0x32;
  civ_score.total = civ_score.peace + civ_score.pollution + civ_score.spaceship_arrived + civ_score.wonders + civ_score.citizens +
                 civ_score.barbarian_bonus + civ_score.future_techs;
  if (civ_score.total < 0) {
    civ_score.total = 0;
  }
  civ_score.conquest_bonus = 0;
  if ((game.civs_alive & 0xfffffffe) == 1 << ((byte)civ & 0x1f)) {
    end_turn = (int)game.turn;
    if (((game.rules & RULE_NO_RESTART_ELIMINATED) != 0) && (end_turn < 0xfb)) {
      end_turn = 0xfa;
    }
    civ_score.conquest_bonus = (uint)game.num_enemies * 100 + (0x23a - end_turn) * 2 + 400;
    if ((game.game_flags & GAME_LARGE_MAP) != 0) {
      civ_score.conquest_bonus = (int)(civ_score.conquest_bonus * 5 + (civ_score.conquest_bonus * 5 >> 0x1f & 3U)) >> 2;
    }
    if ((game.game_flags & GAME_SMALL_MAP) != 0) {
      civ_score.conquest_bonus = (civ_score.conquest_bonus << 2) / 5;
    }
  }
  civ_score.scenario_points = 0;
  if (((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 2) == 0)) {
    value = civ_score.total;
    if (civ_score.total <= civ_score.conquest_bonus) {
      value = civ_score.conquest_bonus;
    }
  }
  else {
    civ_score.total_objective_value = 0;
    civ_score.objective_civ_value = 0;
    civ_score.civ_objective_value = 0;
    for (city = 0; city < game.num_cities; city++) {
      if (cities[city].id != 0) {
        value = city_objective_value(city);
        civ_score.total_objective_value += value;
        if ((int)cities[city].owner == (int)scenario.objective_civ) {
          civ_score.objective_civ_value += value;
        }
        if (cities[city].owner == civ) {
          civ_score.civ_objective_value += value;
        }
      }
    }
    if (scenario.objective_civ == 0) {
      civ_score.objective_civ_value = civ_score.civ_objective_value;
    }
    civ_score.objective_outcome = 2;
    if (civ_score.objective_civ_value < scenario.victory_thresholds[0]) {
      if (civ_score.objective_civ_value < scenario.marginal_victory_score) {
        if (scenario.decisive_defeat_score < civ_score.objective_civ_value) {
          if (civ_score.objective_civ_value <= scenario.marginal_defeat_score) {
            civ_score.objective_outcome = 3;
          }
        }
        else {
          civ_score.objective_outcome = 4;
        }
      }
      else {
        civ_score.objective_outcome = 1;
      }
    }
    else {
      civ_score.objective_outcome = 0;
    }
    outcome = civ_score.objective_outcome;
    if ((scenario.objective_civ != 0) && (scenario.objective_civ != civ)) {
      outcome = 4 - civ_score.objective_outcome;
    }
    switch(outcome) {
    case 0:
      civ_score.scenario_points = 1000;
      break;
    case 1:
      civ_score.scenario_points = 0x2ee;
      break;
    case 2:
      civ_score.scenario_points = 500;
      break;
    case 3:
      civ_score.scenario_points = 0xfa;
      break;
    case 4:
      civ_score.scenario_points = 0;
    }
    civ_score.scenario_points += civ_score.civ_objective_value * 10;
    if (0 < civ_score.conquest_bonus) {
      civ_score.scenario_points = civ_score.scenario_points * 2;
    }
    civ_score.conquest_bonus = -1;
    civ_score.total = civ_score.scenario_points;
    value = civ_score.scenario_points;
  }
  return value;
}

/* 004a74bc auto */
void __cdecl civ_reset_spaceship(iptr civ)

{
  int part;
  
  civs[civ].spaceship_flags = '\0';
  civs[civ].spaceship_population = 0;
  civs[civ].spaceship_launch_year = 0;
  civs[civ].spaceship_arrival_year = 0;
  for (part = 0; part < 6; part++) {
    civs[civ].spaceship_parts[part] = 0;
  }
  return;
}

/* 004a7577 auto */
uint __cdecl civ_spaceship_launched(iptr civ)

{
  return civs[civ].spaceship_flags & SHIP_LAUNCHED;
}

/* 004a75a6 auto */
uint __cdecl civ_spaceship_exists(iptr civ)

{
  return civs[civ].spaceship_flags & SHIP_EXISTS;
}

/* 004a75d5 auto */
i32 __cdecl civ_spaceship_arrived(iptr civ)

{
  int launched;
  i32 result;
  
  launched = civ_spaceship_launched(civ);
  if ((launched == 0) || (game.year < civs[civ].spaceship_arrival_year)) {
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 004a762d auto */
void __cdecl civ_spaceship_lost_notify(iptr civ)

{
  iptr adjective;
  int status;
  
  adjective = civ_get_adjective(civ);
  text_set_string_arg(0,(const char *)adjective);
  status = civ_spaceship_launched(civ);
  if (status == 0) {
    status = civ_spaceship_exists(civ);
    if (status != 0) {
      if (2 < game.multiplayer) {
        net_send_text_args(0xd,0xff,1,0,0,0);
      }
      popup_game((const char *)&str_spacedestroyed);
    }
  }
  else {
    if (2 < game.multiplayer) {
      net_send_text_args(0xc,0xff,1,0,0,0);
    }
    popup_game((const char *)&str_spacereturns);
  }
  civ_reset_spaceship(civ);
  return;
}

/* The parts below call rand() as crt_rand_at("civ_create"): the
   rand trace names the original function, as the original's does. */
/* Part of civ_create (004a7ce9): a civilization created during the game
   may get each advance the human players know (and that is not the
   discovery of a single human player), at a chance growing with the
   difficulty. */
static void civ_create_grant_advances(iptr civ)
{
  int result;
  int grant_roll;
  uint was_alive;
  iptr leader2;

  was_alive = (uint)game.civs_alive & 1 << ((byte)civ & 0x1f);
  game.civs_alive = game.civs_alive & ~(byte)(1 << ((byte)civ & 0x1f));
  for (leader2 = 0; leader2 < 99; leader2++) {
    if (((((uint)game.human_players & 1 << (game.first_discoverer[leader2] & 0x1f)) == 0) ||
        (result = bits_count8((int)(char)game.advance_known_by[leader2]), 2 < result))
       && ((game.human_players & game.advance_known_by[leader2]) != 0)) {
      if (game.difficulty == DIFF_CHIEFTAIN) {
        grant_roll = 0;
      }
      else {
        grant_roll = crt_rand_at("civ_create");
        grant_roll = grant_roll % (game.difficulty + 1);
      }
      if (grant_roll != 0) {
        civ_give_advance(civ,leader2,civ,0,0);
      }
    }
  }
  if (was_alive != 0) {
    game.civs_alive = game.civs_alive | (byte)(1 << ((byte)civ & 0x1f));
  }
}

/* Part of civ_create (004a7ce9): an AI civilization's tribe: a leader of
   the civ's colour not yet used (any of them after the first pass), or the
   one chosen at setup. No_start_units: whether a reused tribe starts
   without units. True when there is none: the civ is not created. */
static bool civ_create_pick_leader(iptr civ,int &no_start_units)
{
  short color;
  byte color_bit;
  int result;
  iptr pick;
  iptr candidates[21];
  int pass;
  int leader_color;
  iptr leader;
  iptr leader2;
  iptr candidate_count;

  candidate_count = 0;
  for (pass = 0; (candidate_count == 0 && (pass < 3)); pass++) {
    for (leader2 = 0; leader2 < 0x15; leader2++) {
      color = leaders[leader2].color;
      leader_color = (int)color;
      result = leader_color;
      (*((char *)&leader_color + 0)) = (byte)color;
      color_bit = (byte)leader_color & 0x1f;
      leader_color = result;
      if ((((1 << color_bit & (uint)game.human_players) == 0) && (civ == color)) &&
         ((leaders[leader2].times_used == '\0' || (0 < pass)))) {
        candidates[candidate_count] = leader2;
        candidate_count++;
      }
    }
  }
  if (candidate_count == 0) {
    game.civs_alive = game.civs_alive & ~(byte)(1 << ((byte)civ & 0x1f));
    net.civ_create_busy = 0;
    return true;
  }
  if (candidate_count == 1 || candidate_count - 1 < 0) {
    pick = 0;
  }
  else {
    result = crt_rand_at("civ_create");
    pick = result % candidate_count;
  }
  leader = candidates[pick];
  if ((game.turn == 0) && (-1 < (((i8 *)&setup_chosen_leaders))[civ])) {
    leader = (iptr)(((i8 *)&setup_chosen_leaders))[civ];
    (((i8 *)&setup_chosen_leaders))[civ] = 0xff;
  }
  if (leaders[leader].times_used == '\0') {
    no_start_units = 0;
  }
  else {
    no_start_units = crt_rand_at("civ_create");
    no_start_units = no_start_units % (leaders[leader].times_used + 1);
  }
  civs[civ].tribe = (i16)leader;
  leaders[leader].times_used++;
  leaders[leader].city_name_index = '\0';
  result = crt_rand_at("civ_create");
  if (result % 3 == 0) {
    if (leaders[leader].female == '\0') {
      leaders[leader].female = 1;
    }
    else {
      leaders[leader].female = '\0';
    }
  }
  leaders[leader].leader_name =
       leaders[leader].names[((uint)leaders[leader].female)];
  civs[civ].leader_portrait = (byte)leader;
  civs[civ].flags = civs[civ].flags & ~CIVF_FEMALE_LEADER;
  if (leaders[leader].female != '\0') {
    civs[civ].leader_portrait += 21;
    civs[civ].flags = civs[civ].flags | CIVF_FEMALE_LEADER;
  }
  return false;
}

/* Part of civ_create (004a7ce9): the search for a start position:
   random land tiles until one is fertile enough, far enough from cities and
   units, without hut or owner (the demands relax as attempts grow). */
static void civ_create_find_start(iptr civ,int *continent_city_counts,uint &x2,uint &y2,int &attempts)
{
  int result;
  int rand_y;
  int rand_x;
  iptr unit;
  int landmass_fertility;
  iptr landmass2;
  int unit_dist;
  int fertility;
  int nearest_city;

  attempts = 0;
  do {
    if ((((game.rules & RULE_FLAT_WORLD) == 0) || (map.width < 0x51)) || (0x5db < attempts)) {
      if (map.width == 1 || map.width - 1 < 0) {
        x2 = 0;
      }
      else {
        result = crt_rand_at("civ_create");
        x2 = result % (int)map.width;
      }
    }
    else {
      if (map.width == 0x11 || map.width - 0x11 < 0) {
        rand_x = 0;
      }
      else {
        rand_x = crt_rand_at("civ_create");
        rand_x = rand_x % (map.width - 0x10);
      }
      x2 = rand_x + 8;
    }
    if ((((game.rules & RULE_FLAT_WORLD) == 0) || (map.height < 0x28)) || (0x5db < attempts)) {
      if (map.height == 1 || map.height - 1 < 0) {
        y2 = 0;
      }
      else {
        result = crt_rand_at("civ_create");
        y2 = result % (int)map.height;
      }
    }
    else {
      if (map.height == 0x11 || map.height - 0x11 < 0) {
        rand_y = 0;
      }
      else {
        rand_y = crt_rand_at("civ_create");
        rand_y = rand_y % (map.height - 0x10);
      }
      y2 = rand_y + 8;
    }
    if ((x2 & 1) != 0) {
      x2--;
    }
    if ((y2 & 1) != 0) {
      x2++;
    }
    nearest_city = city_find_nearest(x2,y2,-1,0xffffffff,-1);
    if (nearest_city < 0) {
      advisor_window_block.nearest_city_dist = 0x1e;
    }
    if (game.turn == 0) {
      for (unit = 0; unit < game.num_units; unit++) {
        if ((units[unit].id != 0) &&
           (unit_dist = map_distance_diag(x2,y2,(int)units[unit].x,
                                          (int)units[unit].y), unit_dist < advisor_window_block.nearest_city_dist)
           ) {
          advisor_window_block.nearest_city_dist = unit_dist;
        }
      }
    }
    fertility = tile_owner_or_fertility(x2,y2);
    landmass2 = tile_landmass(x2,y2);
    landmass_fertility = (int)landmass_table_block.landmass_recs[landmass2].fertile_tiles;
    attempts++;
  } while (((attempts < 0x7d1) ||
           (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0 &&
            (game.turn == 0)))) &&
          (((((result = tile_is_ocean(x2,y2), result != 0 ||
              ((fertility < 0xc - (attempts >> 5) ||
               (advisor_window_block.nearest_city_dist < 0x10 - (attempts >> 7))))) ||
             (((advisor_window_block.nearest_city_dist < 2 && (attempts < 0x7d1)) ||
              ((advisor_window_block.nearest_city_dist < 1 ||
               (landmass_fertility < (0x20 - ((int)game.turn >> 4)) - (attempts >> 6))))))) ||
            ((200 < game.turn && (continent_city_counts[landmass2] != 0)))) ||
           (((result = tile_has_hut(x2,y2), result != 0 && (attempts < 0x7d1))
            || (result = tile_city_or_unit_owner(x2,y2), -1 < result))))));
}

/* Part of civ_create (004a7ce9): the start: settlers at (x2,y2) (units
   of the barbarians nearby removed), a second settlers and an explorer
   later in the game, the tiles around made visible. */
static void civ_create_place_units(iptr civ,uint x2,uint y2,int no_start_units)
{
  int result;
  Tile *map_tile;
  iptr x3;
  iptr unit;
  iptr leader2;
  iptr y3;

  civs[civ].num_advances = 1;
  unit = (iptr)game.num_units;
  while (unit = unit - 1, -1 < unit) {
    if (((units[unit].id != 0) && (units[unit].owner == '\0')) &&
       (result = map_distance_diag(x2,y2,(int)units[unit].x,
                                   (int)units[unit].y), result < 9)) {
      unit_delete(unit,1);
    }
  }
  if (human_civ[0] == civ) {
    map_views_block.map_view_center_x[0] = (i16)x2;
    map_views_block.mapview_center_y[0] = (i16)y2;
  }
  unit = unit_create(UNIT_SETTLERS,civ,x2,y2);
  civs[civ].capital_x = (i16)x2;
  if (((0x14 < game.turn) && (no_start_units == 0)) &&
     ((map_tile = tile_at(x2,y2),
      (map_tile->visible & game.human_players) == 0 &&
      ((unit_create(UNIT_SETTLERS,civ,x2,y2), 0x28 < game.turn &&
       (result = civ_can_build_unit(civ,-1,UNIT_PHALANX), result != 0)))))) {
    unit_create(UNIT_PHALANX,civ,x2,y2);
  }
  net_batch_begin();
  for (leader2 = 0; leader2 < 0x15; leader2++) {
    x3 = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[leader2] + x2);
    y3 = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[leader2] + y2;
    result = map_in_bounds(x3,y3);
    if (result != 0) {
      tile_set_visible(x3,y3,1 << ((byte)civ & 0x1f),1,1);
    }
  }
  net_batch_flush();
}

/* Part of civ_create (004a7ce9): a multiplayer client asks the host to
   create the civ and waits for its answer (the result). */
static int civ_create_request_host(iptr civ)
{
  iptr now_ticks;
  int result;
  int start_ticks;

  net.civ_create_busy = 0;
  net.nm_new_civ_ack_result = -0x1b;
  net_send_message(NM_NEW_CIV,0,civ,0,0,0,0,0,0,0);
  start_ticks = timer_ticks();
  while ((net.nm_new_civ_ack_result == -0x1b && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
    net.process_messages(1,1);
  }
  if (net.nm_new_civ_ack_result == -0x1b) {
    dbg_log((const char *)&str_new_civ_connection_to_server_timed_out);
    popup_game_with_sprite((const char *)&str_serverconnecttime_3,(iptr)&popup_info_icon_sprite,0);
    turn_loop.active = 0;
  }
  else {
    dbg_log((const char *)&str_new_civ_received_nm_new_civ_ack_from_ser);
  }
  while ((net.msgq_priority_count != 0 || (result = net.nm_new_civ_ack_result, net.msgq_normal_count != 0))) {
    net.process_messages(1,0);
  }
  return result;
}

/* Part of civ_create (004a7ce9): the civ's state reset: treasury,
   government, throne room, attitudes (random towards the AIs), goals,
   treaties, advances (with the catch-up ones during the game) and unit
   counts; continent_city_counts: the cities of every civ on each
   continent. */
static void civ_create_reset_civ(iptr civ,int *continent_city_counts)
{
  i16 last_city_turn;
  iptr rand_value;
  int result;
  i32 attitude;
  i32 max_attitude;
  uint clear_mask;
  iptr leader;
  iptr leader2;

  civs[civ].gold = 0;
  civs[civ].research_progress = 0;
  civs[civ].num_advances = '\0';
  civs[civ].research_cost_advances = '\0';
  last_city_turn = game.turn;
  if (game.turn < 0xb) {
    last_city_turn = 10;
  }
  civs[civ].last_city_turn = last_city_turn;
  civs[civ].government = GOV_DESPOTISM;
  civs[civ].tax_rate = 4;
  civs[civ].science_rate = 4;
  civs[civ].subversion_count = 0;
  civs[civ].reputation = '\0';
  civs[civ].diplo_request_count = '\0';
  civs[civ].best_shield_surplus = 0;
  for (leader2 = 0; leader2 < 10; leader2++) {
    *(i8 *)&civs[civ].throne_room[leader2] = 0;
  }
  civs[civ].throne_room[0] = 1;
  civs[civ].throne_room[1] = 1;
  civs[civ].throne_room[3] = 1;
  civs[civ].throne_room[6] = 1;
  civs[civ].throne_room_upgrades = 0;
  for (leader = 0; leader < 8; leader++) {
    civs[civ].reputation_credit[leader] = 0;
    civs[leader].reputation_credit[civ] = 0;
    *(i8 *)&civs[civ].aggression_counter[leader] = 0;
    *(i8 *)&civs[leader].aggression_counter[civ] = 0;
  }
  civs[civ].attitude[0] = 'd';
  for (leader = 1; leader < 8; leader++) {
    if (civ == 0) {
      civs[0].attitude[leader] = 'd';
    }
    else if ((1 << ((byte)leader & 0x1f) & (uint)game.human_players) == 0) {
      rand_value = crt_rand_at("civ_create");
      *(char *)&civs[leader].attitude[civ] = (char)(rand_value % 0x50) + '\n';
      *(i8 *)&civs[civ].attitude[leader] =
           *(i8 *)&civs[leader].attitude[civ];
    }
    else {
      max_attitude = 0x4b;
      attitude = 10;
      result = crt_rand_at("civ_create");
      attitude = int_clamp((uint)game.difficulty * 5 + result % 0x50 + 10,attitude,max_attitude);
      *(char *)&civs[leader].attitude[civ] = (char)attitude;
      *(i8 *)&civs[civ].attitude[leader] =
           *(i8 *)&civs[leader].attitude[civ];
    }
  }
  civ_refresh_ai_goals(civ);
  civ_reset_standing_goals(civ);
  for (leader2 = 0; leader2 < 0x40; leader2++) {
    *(i8 *)&civs[civ].continent_policy[leader2] = 5;
    continent_city_counts[leader2] = 0;
    for (leader = 1; leader < 8; leader++) {
      continent_city_counts[leader2] =
           continent_city_counts[leader2] + (uint)civs[leader].continent_cities[leader2];
    }
  }
  civs[civ].num_cities = 0;
  civ_unit_counts_block.civ_unit_totals[civ] = 0;
  civs[civ].num_military_units = 0;
  civs[civ].military_strength = 0;
  civs[civ].total_attack = 0;
  civs[civ].total_city_size = 0;
  civs[civ].num_ships = 0;
  civs[civ].population_milestone = 0;
  for (leader2 = 0; leader2 < 4; leader2++) {
    *(i8 *)&civs[civ].wonders_by_group[leader2] = 0;
  }
  for (leader = 0; leader < 8; leader++) {
    civs[leader].last_contact_turn[civ] = 0xffff;
  }
  clear_mask = ~(1 << ((byte)civ & 0x1f));
  for (leader2 = 0; leader2 < 100; leader2++) {
    *(i8 *)&civs[civ].advance_source[leader2] = 0xff;
    game.advance_known_by[leader2] = game.advance_known_by[leader2] & (byte)clear_mask;
  }
  for (leader = 0; leader < 8; leader++) {
    *(i32 *)&civs[civ].treaties[leader] = 0;
    *(i32 *)&civs[leader].treaties[civ] = 0;
  }
  for (leader2 = 0; leader2 < 0xd; leader2++) {
    *(i8 *)&civs[civ].advances[leader2] = 0;
  }
  if (game.turn != 0) {
    civ_create_grant_advances(civ);
  }
  for (leader2 = 0; leader2 < 0x3e; leader2++) {
    *(i8 *)&civs[civ].units_active[leader2] = 0;
    *(i8 *)&civs[civ].units_in_production[leader2] = 0;
    *(i8 *)&civs[civ].units_lost[leader2] = 0;
  }
}

/* 004a7ce9 auto */
int __cdecl civ_create(iptr civ)

{
  int result;
  int no_start_units;
  int attempts;
  uint y2;
  int continent_city_counts[64];
  uint x2;
  int old_tribe;
  iptr leader2;
  int alive_count;
  
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) || (game.turn == 0)) {
    net.civ_create_busy = 1;
    if ((game.multiplayer < 3) || (net.host_flag != '\0')) {
      old_tribe = (int)civs[civ].tribe;
      civ_create_reset_civ(civ,continent_city_counts);
      if (civ == 0) {
        net.civ_create_busy = 0;
        result = 1;
      }
      else {
        no_start_units = 0;
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
          if (civ_create_pick_leader(civ,no_start_units)) {
            return 1;
          }
        }
        if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
          net.civ_create_busy = 0;
          result = 1;
        }
        else {
          civ_create_find_start(civ,continent_city_counts,x2,y2,attempts);
          if ((game.turn == 0) &&
             (result = map_in_bounds((int)start_positions.x[civs[civ].tribe],
                                    (int)start_positions.y[civs[civ].tribe]), result != 0)) {
            x2 = (uint)start_positions.x[civs[civ].tribe];
            y2 = (uint)start_positions.y[civs[civ].tribe];
            attempts = 0;
          }
          if (2000 < attempts) {
            no_start_units = 1;
          }
          if (((int)y2 < 3) || (map.height - 3 <= (int)y2)) {
            no_start_units = 1;
          }
          if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
            civs[civ].science_rate = leaders[civs[civ].tribe].civilize + 3;
            civs[civ].tax_rate = '\t' - civs[civ].science_rate;
          }
          if (((no_start_units == 0) &&
              (((game.turn == 0 || ((game.rules & RULE_NO_RESTART_ELIMINATED) == 0)) &&
               ((int)game.turn <= (int)((uint)game.difficulty * -0x23 + 0x15e))))) ||
             ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
            civ_create_place_units(civ,x2,y2,no_start_units);
            net.civ_create_busy = 0;
            result = 1;
          }
          else {
            game.civs_alive = game.civs_alive & ~(byte)(1 << ((byte)civ & 0x1f));
            civs[civ].tribe = (i16)old_tribe;
            alive_count = 0;
            for (leader2 = 1; leader2 < 8; leader2++) {
              if ((1 << ((byte)leader2 & 0x1f) & (uint)game.civs_alive) != 0) {
                alive_count++;
              }
            }
            if (alive_count < 2) {
              if ((game.game_flags & GAME_WON) == 0) {
                game_over_reason = 3;
                game.game_flags = game.game_flags | GAME_WON;
              }
              net.civ_create_busy = 0;
              result = -1;
            }
            else {
              net.civ_create_busy = 0;
              result = 0;
            }
          }
        }
      }
    }
    else {
      result = civ_create_request_host(civ);
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004aa378 auto */
int __cdecl civ_check_destroyed(iptr civ,iptr civ2)

{
  uint *treaty_ptr;
  iptr start_ticks;
  iptr now_ticks;
  iptr name_text;
  int result;
  i32 plural_name;
  iptr unit;
  iptr city;
  int y2;
  int x2;
  
  if (civ == 0) {
    result = 0;
  }
  else {
    net.civ_check_destroyed_busy = 1;
    if ((game.multiplayer < 3) || (net.host_flag != '\0')) {
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city].owner == civ)) {
          net.civ_check_destroyed_busy = 0;
          return 0;
        }
      }
      if (civ2 == -1) {
        civ_reset_spaceship(civ);
      }
      else {
        if (autoplay.active == 0) {
          name_text = civ_adjective(civ);
          text_set_string_arg_b(0,(const char *)name_text);
          name_text = (iptr)(civ_name_plural(civ2));
          text_set_string_arg_b(1,(const char *)name_text);
          if (2 < game.multiplayer) {
            net_send_text_args(0xe,0xff,2,0,civ,0);
          }
          music_play_track(2,1);
          popup_game_with_sprite((const char *)&str_destroyed,(iptr)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c),0);
        }
        if (destroyed_civs.count[0] < 0xc) {
          result = (int)destroyed_civs.count[0];
          destroyed_civs.count[0]++;
          *(i16 *)(((i8 *)&destroyed_civs.turns) + result * 2) = game.turn;
          (((i8 *)&destroyed_civs.killers))[result] = (i8)civ2;
          (((i8 *)&destroyed_civs.portraits))[result] = civs[civ].leader_portrait;
          plural_name = (i32)(civ_name_plural(civ));
          crt_strcpy(((i8 *)&destroyed_civs.names) + result * 0x18,(const char *)plural_name);
        }
        civ_spaceship_lost_notify(civ);
      }
      unit = (iptr)game.num_units;
      while (unit = unit - 1, -1 < unit) {
        if (units[unit].id != 0) {
          if (((units[unit].owner == civ) || ((units[unit].flags & UNITF_SPY_ON_MISSION) == 0)) ||
             ((char)units[unit].counter != civ)) {
            if (units[unit].owner == civ) {
              unit_delete(unit,1);
            }
          }
          else {
            units[unit].flags = units[unit].flags & ~UNITF_SPY_ON_MISSION;
            treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + (char)units[unit].counter * 4);
            *treaty_ptr = *treaty_ptr & 0xfeffffff;
            spy_try_escape(unit,0,(int)(char)units[unit].counter);
          }
        }
      }
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
        if ((*(i8 *)&human_civ) == civ) {
          game_over_reason = 4;
        }
        else if ((2 < game.multiplayer) && ((*(i8 *)&human_civ) != civ)) {
          net_send_message(NM_END_SEQUENCE,net.net_slots[net.net_civ_slots[civ]].id,
                             (uint)game.game_flags,4,0,0,0,0,0,0);
        }
        passwords_unscramble();
        (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[civ * 0x20] = 0;
        passwords_scramble();
        if ((~(1 << ((byte)civ & 0x1f)) & (uint)game.human_players) == 0) {
          net.civ_check_destroyed_busy = 0;
          return 0;
        }
      }
      net_batch_begin();
      for (x2 = 0; x2 < map.width; x2++) {
        for (y2 = 0; y2 < map.height; y2++) {
          tile_set_visible(x2,y2,1 << ((byte)civ & 0x1f),0,1);
        }
      }
      net_batch_flush();
      if (2 < game.multiplayer) {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
        net_send_message(NM_UNLOCK_NATION,0,0,civ,0,0,0,0,0,0);
      }
      if (game.multiplayer == 1) {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
      }
      civ_create(civ);
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
        net_send_message(NM_DRAW_MAP,0xff,0,0,0,0,0,0,0,0);
      }
      if ((game.multiplayer != 1) ||
         ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)game.civs_alive) != 0)) {
        map_redraw_all_views((*(i8 *)&human_civ),1);
      }
      net.civ_check_destroyed_busy = 0;
      result = 1;
    }
    else {
      net.civ_check_destroyed_busy = 0;
      net.nm_kill_civ_ack_result = -0x1b;
      net_send_message(NM_KILL_CIV,0,civ,civ2,0,0,0,0,0,0);
      start_ticks = timer_ticks();
      while ((net.nm_kill_civ_ack_result == -0x1b && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
        net.process_messages(1,1);
      }
      if (net.nm_kill_civ_ack_result == -0x1b) {
        dbg_log((const char *)&str_kill_civ_connection_to_server_timed_out);
        popup_game_with_sprite((const char *)&str_serverconnecttime_4,(iptr)&popup_info_icon_sprite,0);
        turn_loop.active = 0;
      }
      else {
        dbg_log((const char *)&str_kill_civ_received_nm_new_civ_ack_from_se);
      }
      while ((net.msgq_priority_count != 0 || (result = net.nm_kill_civ_ack_result, net.msgq_normal_count != 0))) {
        net.process_messages(1,0);
      }
    }
  }
  return result;
}

/* 004b0720 auto */
i8 __cdecl rules_parse_advance_abbrev(char *abbrev)

{
  i8 result;
  int cmp;
  int found;
  int advance;
  
  cmp = _strcmp(abbrev,((i8 *)&rules_advance_no_str));
  if (cmp == 0) {
    result = 0xfe;
  }
  else {
    cmp = _strcmp(abbrev,((i8 *)&rules_advance_nil_str));
    if (cmp == 0) {
      result = 0xff;
    }
    else {
      found = -3;
      for (advance = 0; (found < 0 && (advance < 100)); advance++) {
        cmp = _strcmp(abbrev,advances[advance].abbrev);
        if (cmp == 0) {
          found = advance;
        }
      }
      result = (i8)found;
    }
  }
  return result;
}

/* 004b07d1 auto */
i8 __cdecl rules_parse_terrain_abbrev(char *abbrev)

{
  i8 result;
  int cmp;
  int found;
  int terrain2;
  
  cmp = __strcmpi(abbrev,((i8 *)&rules_terrain_no_str));
  if (cmp == 0) {
    result = 0xff;
  }
  else {
    cmp = __strcmpi(abbrev,((i8 *)&rules_terrain_yes_str));
    if (cmp == 0) {
      result = 0xfe;
    }
    else {
      found = -3;
      for (terrain2 = 0; (found < 0 && (terrain2 < 0xb)); terrain2++) {
        cmp = _strcmp(abbrev,terrain_types[terrain2].abbrev);
        if (cmp == 0) {
          found = terrain2;
        }
      }
      result = (i8)found;
    }
  }
  return result;
}

/* 004bd9f0 auto */
i32 __cdecl civ_has_advance(iptr civ,iptr advance_id)

{
  i32 result;
  iptr advance;
  byte bit_mask[4];
  
  if (advance_id == -2) {
    result = 0;
  }
  else if (advance_id < 0) {
    result = 1;
  }
  else if (advance_id == ADV_FUTURE_TECH) {
    result = 0;
  }
  else if (advance_id < 100) {
    if (civ < 1) {
      result = 0;
    }
    else {
      bit_index_split(advance_id,(iptr)&advance,(iptr)bit_mask);
      if ((bit_mask[0] & civs[civ].advances[advance]) == 0) {
        result = 0;
      }
      else {
        result = 1;
      }
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* BF-173 (Peter Johnson's ledger): advance_has_prereq's search, which goes
   to each advance once.  A scenario's RULES.TXT can make advances each
   other's prerequisites (the original takes them, Bonaparte's rules have
   two); the original's search went round such a cycle until the stack ran
   out.  The answer is the same where it had one: whether prereq is among
   the advance's prerequisites, at any distance. */
static i32 advance_has_prereq_seen(iptr advance,iptr prereq,u8 (&seen)[100])
{
  if (advance < 0) {
    return 0;
  }
  if (advance == prereq) {
    return 1;
  }
  if ((99 < advance) || (seen[advance] != 0)) {
    return 0;
  }
  seen[advance] = 1;
  return ((advance_has_prereq_seen((int)advances[advance].preq[0],prereq,seen) != 0) ||
          (advance_has_prereq_seen((int)advances[advance].preq[1],prereq,seen) != 0));
}

/* 004bdaa5 auto */
i32 __cdecl advance_has_prereq(iptr advance,iptr prereq)

{
  u8 seen[100];

  _memset(seen,0,sizeof seen);
  return advance_has_prereq_seen(advance,prereq,seen);
}

/* 004be6ba auto */
void __cdecl civ_leonardo_upgrade(iptr civ)

{
  byte unit_type;
  int result;
  iptr unit;
  int upgrade_advance;
  iptr unit_type3;
  iptr unit_type2;
  char notified[64];
  
  result = civ_has_wonder(civ,WONDER_LEONARDO);
  if (result != 0) {
    for (unit_type2 = 0; unit_type2 < 0x3e; unit_type2++) {
      notified[unit_type2] = '\0';
    }
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) && (units[unit].owner == civ)) {
        unit_type = units[unit].type;
        upgrade_advance = (int)unit_types[unit_type].obsolete_by;
        if ((unit_types[unit_type].role == ROLE_DEFEND) &&
           (((char)unit_types[unit_type].defense < (char)unit_types[UNIT_MUSKETEERS].defense &&
            (result = civ_has_advance(civ,ADV_GUNPOWDER), result != 0)))) {
          upgrade_advance = 0x23;
        }
        if ((-1 < upgrade_advance) && (result = civ_has_advance(civ,upgrade_advance), result != 0)) {
          unit_type3 = -1;
          for (unit_type2 = 0; unit_type2 < 0x3e; unit_type2++) {
            if (((unit_types[unit_type2].prerequisite == upgrade_advance) &&
                (unit_types[units[unit].type].role == unit_types[unit_type2].role)) &&
               ((char)unit_types[units[unit].type].hold <= (char)unit_types[unit_type2].hold)) {
              unit_type3 = unit_type2;
            }
          }
          if (-1 < unit_type3) {
            if ((notified[units[unit].type] == '\0') &&
               ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
              notified[units[unit].type] = 1;
              text_set_label_arg(0,unit_types[units[unit].type].name);
              text_set_label_arg(1,unit_types[unit_type3].name);
              text_set_label_arg(2,improvements[IMP_LEONARDO].name);
              if ((game.multiplayer < 3) || (human_civ[0] == civ)) {
                dlg_game_txt_with_unit_flags((const char *)&str_upgrade,unit_type3,(ui_metrics.large_screen == 0) - 1 & 8);
              }
              else if ((uint)net.net_civ_slots[civ] < 7) {   /* (BF-194) -1: its player left */
                net_send_text_args(0x3e,net.net_slots[net.net_civ_slots[civ]].id,3,0,unit_type3,
                                   ui_metrics.large_screen);
              }
            }
            units[unit].type = (u8)unit_type3;
            units[unit].flags = units[unit].flags & ~UNITF_VETERAN;
            map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
            if (2 < game.multiplayer) {
              net_send_state_diff(0xff,2,0,0,0);
              net_send_message(NM_DRAW_SQUARE,0xff,(int)units[unit].x,(int)units[unit].y,0,0,0,0,0,0
                                );
            }
          }
        }
      }
    }
  }
  return;
}

/* 004bea84 auto */
void __cdecl civ_advance_gov_prompts(iptr civ,iptr advance)

{
  int choice;
  iptr civ_plural;
  int gov_sprite_index;
  
  if (advance == ADV_MONARCHY) {
    if ((civs[civ].government == GOV_DESPOTISM) &&
       (choice = popup_game_with_sprite((const char *)&str_automonarchy,(iptr)&government_sprites_block.government_monarchy_sprite,0), choice == 1)) {
      civ_plural = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)civ_plural);
      popup_game_with_sprite((const char *)&str_startrev_2,(iptr)&popup_info_icon_sprite,0);
      civ_set_government(civ,0);
    }
  }
  else if ((((advance == ADV_REPUBLIC) || (advance == ADV_DEMOCRACY)) || (advance == ADV_COMMUNISM)) || (advance == ADV_FUNDAMENTALISM)) {
    if (((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) && (advance == ADV_REPUBLIC)) &&
       (civs[civ].government < GOV_COMMUNISM)) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_republic,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      return;
    }
    if (advance == ADV_COMMUNISM) {
      gov_sprite_index = 3;
    }
    else if (advance == ADV_FUNDAMENTALISM) {
      gov_sprite_index = 4;
    }
    else if (advance == ADV_REPUBLIC) {
      gov_sprite_index = 5;
    }
    else {
      gov_sprite_index = 6;
    }
    if ((advance == ADV_REPUBLIC) && (civs[civ].government == GOV_REPUBLIC)) {
      return;
    }
    if ((advance == ADV_COMMUNISM) && (civs[civ].government == GOV_COMMUNISM)) {
      return;
    }
    if ((advance == ADV_FUNDAMENTALISM) && (civs[civ].government == GOV_FUNDAMENTALISM)) {
      return;
    }
    if ((civs[civ].government != GOV_DEMOCRACY) && (civs[civ].government != GOV_ANARCHY)) {
      text_set_label_arg(0,advances[advance].name);
      text_set_label_arg(1,government_names[civs[civ].government]);
      choice = popup_game_with_sprite((const char *)&str_autorev,(iptr)(((i8 *)&government_sprites_block.government_sprites) + gov_sprite_index * 0x3c),0);
      if (choice == 1) {
        civ_plural = (iptr)(civ_name_plural(civ));
        text_set_string_arg(0,(const char *)civ_plural);
        popup_game_with_sprite((const char *)&str_startrev_3,(iptr)&popup_info_icon_sprite,0);
        civ_set_government(civ,0);
      }
    }
  }
  if (((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) {
    if (advance == ADV_RAILROAD) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_railroads,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == ADV_REFRIGERATION) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_farmland,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == ADV_TRADE) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_trade,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == ADV_SEAFARING) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_seafaring,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == ADV_WRITING) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_writing,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
  }
  return;
}

/* 004bee56 auto */
void __cdecl civ_golden_age_popup(iptr civ)

{
  iptr adjective;
  int has_temple;
  int roll;
  iptr city2;
  iptr city;
  int best_roll;
  int weight;
  
  adjective = civ_get_adjective(civ);
  text_set_string_arg(0,(const char *)adjective);
  city2 = -1;
  best_roll = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      weight = (int)cities[city].size;
      has_temple = city_has_improvement(city,IMP_PALACE);
      if (has_temple != 0) {
        weight = weight << 1;
      }
      if (weight == 1 || weight - 1 < 0) {
        roll = 0;
      }
      else {
        roll = _rand();
        roll = roll % weight;
      }
      if (best_roll <= roll + 1) {
        city2 = city;
        best_roll = roll + 1;
      }
    }
  }
  if (-1 < city2) {
    text_set_string_arg(1,(const char *)&cities[city2].name);
    if (2 < game.multiplayer) {
      net_send_text_args(0x54,0xff,2,0,0,0);
    }
    popup_game_with_advance((const char *)&str_goldenage,ADV_PHILOSOPHY,8);
  }
  return;
}

/* Part of civ_give_advance (004bf05b): the dialog announcing the advance:
   who acquired it (from whom), its picture, and for others' advances what
   it allows. dialog2: the caller's. */
static void civ_advance_announce(uptr civ,iptr advance,iptr civ2,Dialog &dialog2)
{
  iptr civ_plural;
  iptr improvement;
  int list_count;
  iptr unit_type;
  i32 line_width;

  dialog2.init_default();
  dialog2.set_width(0x140);
  text_clear();
  civ_plural = (iptr)(civ_name_plural(civ));
  text_append((const char *)civ_plural);
  text_append_space();
  if ((civ2 == 0) || (game_language != 2)) {
    text_append_label(0x5f - (uint)(civ2 == 0));
  }
  else {
    text_append((const char *)&str_erhalten);
  }
  text_append_space();
  text_append_string_id(advances[advance].name);
  if ((civ2 != 0) && (civ != civ2)) {
    text_append_space();
    if (game_language == 2) {
      text_append((const char *)&str_von_den);
    }
    else {
      text_append_label(0x60);
    }
    text_append_space();
    if (civ2 < 0) {
      text_append_string_id(improvements[IMP_GREAT_LIBRARY].name);
    }
    else {
      civ_plural = (iptr)(civ_name_plural(civ2));
      text_append((const char *)civ_plural);
    }
  }
  text_append((const char *)&str_empty_119);
  dialog2.add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
  if (diplo_meeting.session_active == 0) {
    dialog2.add_sprite(
               (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                     (char)advances[advance].epoch * 0x3c +
                     (char)advances[advance].category * 0xf0),0,0);
    dialog2.set_image_zoom(8);
  }
  line_width = 0;
  list_count = 0;
  text_clear();
  if (((*(i8 *)&human_civ) != civ) ||
     ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)game.human_players) == 0)) {
    for (improvement = 0; improvement < 0x43; improvement++) {
      if (improvements[improvement].preq == advance) {
        text_list_append_wrapped((iptr)&dialog2,improvements[improvement].name,(iptr)&list_count,
                           (iptr)&line_width);
      }
    }
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      if (unit_types[unit_type].prerequisite == advance) {
        text_list_append_wrapped((iptr)&dialog2,unit_types[unit_type].name,(iptr)&list_count,
                           (iptr)&line_width);
      }
    }
    if (list_count != 0) {
      text_append_rparen();
      dialog2.add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
    }
  }
  dialog2.show_modal(0);
}

/* Part of civ_give_advance (004bf05b): an advance making barracks
   obsolete: the civ's barracks are sold (with a dialog for a human). */
static void civ_advance_sell_barracks(uptr civ,iptr advance,Dialog &dialog2)
{
  int improvement2;
  iptr city;
  int count;

  count = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && ((int)cities[city].owner == (civ & 0xff))) {
      improvement2 = city_has_improvement(city,IMP_BARRACKS);
      if (improvement2 != IMP_NOTHING) {
        count++;
      }
      city_set_improvement(city,2,0);
    }
  }
  if (count != 0) {
    count = (uint)improvements[IMP_BARRACKS].cost * (uint)cosmic.shield_box_rows * count;
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
      text_set_label_arg(0,advances[advance].name);
      text_set_number_arg(0,count);
      dialog2.load_game_text((const char *)&str_barracks);
      if (diplo_meeting.session_active == 0) {
        dialog2.add_sprite(
                   (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                         (char)advances[advance].epoch * 0x3c +
                         (char)advances[advance].category * 0xf0),0,0);
        dialog2.add_sprite((iptr)&caption_close_button_sprite_block.notify_sprites,0,0);
        dialog2.set_image_zoom(8);
      }
      dialog2.show_modal(0);
    }
    civs[civ].gold += count;
    if ((*(i8 *)&human_civ) == civ) {
      status_draw_info(1);
    }
  }
}

/* Part of civ_give_advance (004bf05b): the first civ to discover an
   advance: Philosophy's golden age, the first discoverer recorded, and the
   wonders it makes obsolete announced. */
static void civ_advance_first_discovery(uptr civ,iptr advance,iptr civ2,Dialog &dialog2)
{
  int improvement2;
  iptr city;
  iptr wonder;

  if (((advance == ADV_PHILOSOPHY) &&
      (civs[civ].flags = civs[civ].flags | CIVF_FREE_ADVANCE, autoplay.active == 0)) &&
     (((*(i8 *)&human_civ) != civ || (civ2 != 0)))) {
    civ_golden_age_popup(civ);
  }
  game.first_discoverer[advance] = game.first_discoverer[advance] | (byte)civ;
  if (autoplay.active == 0) {
    for (wonder = 0; wonder < 0x1c; wonder++) {
      if (((wonder_expires[wonder] == advance) &&
          (city = (iptr)game.wonder_city[wonder], -1 < city)) &&
         (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0)) {
        improvement2 = wonder + 0x27;
        text_set_label_arg(0,advances[advance].name);
        text_set_label_arg(1,improvements[improvement2].name);
        dialog2.load_game_text((const char *)&str_endwonder_2);
        if (diplo_meeting.session_active == 0) {
          dialog2.add_sprite(
                     (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                           (char)advances[advance].epoch * 0x3c +
                           (char)advances[advance].category * 0xf0),0,0);
          dialog2.add_sprite((iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c),0,0);
          dialog2.set_image_zoom(8);
        }
        if (2 < game.multiplayer) {
          net_send_text_args(0x4d,0xff,2,0,advance,improvement2);
        }
        dialog2.show_modal(0);
      }
    }
  }
}

/* 004bf05b auto */
void __cdecl civ_give_advance(uptr civ,iptr advance,iptr civ2,iptr skip_pedia,int from_network)

{
  iptr has_wonder_a;
  iptr has_wonder_b;
  iptr civ_plural;
  int improvement2;
  uptr civ3;
  iptr advance3;
  byte bit_mask[4];
  Dialog dialog2;
  iptr advance2;
  int civ4;
  int first_discovery;
  int count;
  
  first_discovery = 0;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if ((((game.multiplayer < 3) || (from_network != 0)) ||
      ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) ||
     (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 || ((*(i8 *)&human_civ) == civ)))
     ) {
    if (civs[civ].researching == advance) {
      civs[civ].researching = -1;
    }
    if ((advance == ADV_FUTURE_TECH) || (99 < advance)) {
      civs[civ].future_techs++;
    }
    else {
      bit_index_split(advance,(iptr)&advance3,(iptr)bit_mask);
      civs[civ].advances[advance3] =
           civs[civ].advances[advance3] | bit_mask[0];
      *(i8 *)&civs[civ].advance_source[advance] = (i8)civ2;
      if (game.advance_known_by[advance] == '\0') {
        first_discovery = 1;
      }
      game.advance_known_by[advance] =
           game.advance_known_by[advance] | (byte)(1 << ((byte)civ & 0x1f));
    }
    if (game.turn == 0) {
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
      }
    }
    else {
      civs[civ].num_advances++;
      if ((autoplay.active == 0) &&
         (((((*(i8 *)&human_civ) == civ && (civ2 < 0)) ||
           (((*(i8 *)&human_civ) == civ &&
            ((civ == civ2 && (((*((char *)&game.game_options + 2)) & (OPT_PEDIA_ADVANCES >> 16)) == 0)))))) ||
          (((*(i8 *)&human_civ) != civ &&
           ((((((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & TREATY_EMBASSY) != 0 ||
               (has_wonder_a = civ_has_wonder((*(i8 *)&human_civ),WONDER_UNITED_NATIONS), has_wonder_a != 0)) ||
              (has_wonder_b = civ_has_wonder((*(i8 *)&human_civ),WONDER_MARCO_POLO), has_wonder_b != 0)) ||
             (((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & TREATY_ALLIANCE) != 0 ||
              (game.reveal_map != '\0')))) &&
            ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)))))))) {
        civ_advance_announce(civ,advance,civ2,dialog2);
      }
      else if (((autoplay.active == 0) &&
               ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) &&
              (((first_discovery != 0 &&
                (((improvement2 = rand_range(0,2), improvement2 == IMP_NOTHING &&
                  ((*(byte *)&civs[civ].treaties[(*(i8 *)&human_civ)] & TREATY_CONTACT) != 0)) &&
                 ((*(byte *)&civs[civ].treaties[(*(i8 *)&human_civ)] & TREATY_ALLIANCE) == 0)))) &&
               ((((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & TREATY_EMBASSY) == 0 &&
                 (improvement2 = civ_has_wonder((*(i8 *)&human_civ),WONDER_UNITED_NATIONS), improvement2 == IMP_NOTHING)) &&
                (improvement2 = civ_has_wonder((*(i8 *)&human_civ),WONDER_MARCO_POLO), improvement2 == IMP_NOTHING)))))) {
        improvement2 = diplo_ask_receive_emissary((*(i8 *)&human_civ),civ,0xffffffff,0xffffffff);
        if (improvement2 != IMP_NOTHING) {
          civ_plural = (iptr)(civ_name_plural(civ));
          text_set_string_arg(1,(const char *)civ_plural);
          text_set_label_arg(2,advances[advance].name);
          popup_game((const char *)&str_gape);
        }
        diplo_session_reset();
      }
      if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
           ((*(i8 *)&human_civ) == civ)) && (civ2 != 0)) && (autoplay.active == 0)) {
        if (((diplo_meeting.session_active == 0) && (((*((char *)&game.game_options + 2)) & (OPT_PEDIA_ADVANCES >> 16)) != 0)) && (skip_pedia == 0)) {
          ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(advance);
        }
        civ_advance_gov_prompts(civ,advance);
      }
      for (advance2 = 0x35; (-1 < advance2 && (advances[advance2].enabled == '\0'));
          advance2 = (iptr)advances[advance2].preq[0]) {
      }
      if ((advance == ADV_GUNPOWDER) || (advance2 == advance)) {
        civ_advance_sell_barracks(civ,advance,dialog2);
      }
      if (first_discovery != 0) {
        civ_advance_first_discovery(civ,advance,civ2,dialog2);
      }
      civ_leonardo_upgrade(civ);
      civ3 = wonder_get_owner(WONDER_GREAT_LIBRARY);
      if ((0 < (int)civ3) && (improvement2 = civ_has_advance(civ3,advance), improvement2 == IMP_NOTHING)) {
        count = 0;
        for (civ4 = 1; civ4 < 8; civ4++) {
          improvement2 = civ_has_advance(civ4,advance);
          if (improvement2 != IMP_NOTHING) {
            count++;
          }
        }
        if (1 < count) {
          civ_give_advance(civ3,advance,-2,0,0);
        }
      }
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
      }
    }
  }
  else {
    net_send_message(NM_SET_CIV,net.net_slots[net.net_civ_slots[civ]].id,civ,
                       advance,civ2,skip_pedia,0,0,0,0);
  }
  return;
}

/* 004bfdbe auto */
i32 __cdecl civ_can_research_advance(iptr civ,iptr advance)

{
  int known;
  
  if ((((advances[advance].enabled != '\0') &&
       (known = civ_has_advance(civ,advance), known == 0)) &&
      (known = civ_has_advance(civ,(int)advances[advance].preq[0]), known != 0)) &&
     (known = civ_has_advance(civ,(int)advances[advance].preq[1]), known != 0)) {
    return 1;
  }
  return 0;
}

/* 004bfe5a auto */
i32 __cdecl civ_can_build_unit(iptr civ,iptr city,iptr unit_type)

{
  char prereq;
  int known;
  i32 result;
  iptr unit_type2;
  
  result = 0;
  prereq = unit_types[unit_type].prerequisite;
  if ((((-2 < prereq) &&
       ((prereq < '\0' || (known = civ_has_advance(civ,(int)prereq), known != 0)))) &&
      (((unit_types[unit_type].obsolete_by < '\0' ||
        (known = civ_has_advance(civ,(int)unit_types[unit_type].obsolete_by), known == 0)) &&
       ((((unit_types[unit_type].role != ROLE_DEFEND ||
          ((char)unit_types[UNIT_MUSKETEERS].defense <= (char)unit_types[unit_type].defense)) ||
         (known = civ_has_advance(civ,ADV_GUNPOWDER), known == 0)) &&
        (((unit_types[unit_type].role != ROLE_ATTACK ||
          ((int)(char)unit_types[unit_type].move != (uint)cosmic.road_move_mult * 2)) ||
         (((char)unit_types[UNIT_KNIGHTS].attack < (char)unit_types[unit_type].attack ||
          (((char)unit_types[UNIT_KNIGHTS].defense <= (char)unit_types[unit_type].defense ||
           (known = civ_has_advance(civ,(int)unit_types[UNIT_KNIGHTS].prerequisite), known == 0))))))
        )))))) &&
     ((((char)unit_types[unit_type].attack < 'c' ||
       ((game.wonder_city[WONDER_MANHATTAN] != -1 && (known = civ_has_advance(civ,ADV_NUCLEAR_FISSION), known != 0))))
      && (((city < 0 || (unit_types[unit_type].domain != DOMAIN_SEA)) ||
          (((cities[city].flags & CITYF_OCEAN_ACCESS) != 0 && ((cities[city].flags & CITYF_COASTAL) != 0))))))))
  {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
       (((int)(uint)cosmic.road_move_mult < (int)(char)unit_types[unit_type].move &&
        (unit_types[unit_type].domain == DOMAIN_LAND)))) {
      for (unit_type2 = 0; unit_type2 < 0x3e; unit_type2++) {
        if (((((unit_type2 != unit_type) && (unit_types[unit_type2].role == unit_types[unit_type].role)) &&
             (unit_types[unit_type2].domain == unit_types[unit_type].domain)) &&
            (((int)(uint)cosmic.road_move_mult < (int)(char)unit_types[unit_type2].move &&
             (known = civ_has_advance(civ,(int)unit_types[unit_type2].prerequisite), known != 0
             )))) && (((char)unit_types[unit_type].attack <= (char)unit_types[unit_type2].attack &&
                      ((char)unit_types[unit_type].defense <= (char)unit_types[unit_type2].defense)))) {
          if (unit_types[unit_type2].attack != unit_types[unit_type].attack) {
            return 0;
          }
          if (unit_types[unit_type2].defense != unit_types[unit_type].defense) {
            return 0;
          }
        }
      }
    }
    if ((unit_type != UNIT_FANATICS) || (civs[civ].government == GOV_FUNDAMENTALISM)) {
      result = 1;
    }
  }
  return result;
}

/* 004c02d8 auto */
i32 __cdecl civ_can_build_wonder(iptr civ,iptr wonder)

{
  int known;
  i32 result;
  
  result = 0;
  if ((((((game.game_flags & GAME_SCENARIO) == 0) || ((scenario.flags[0] & 4) == 0)) || (game.reveal_map != '\0'))
      || ((((int)(short)scenario.flags[0] & 0x8000U) != 0 && (wonder == WONDER_MANHATTAN)))) &&
     ((game.wonder_city[wonder] == -1 &&
      (known = civ_has_advance(civ,(int)improvements[wonder + 0x27].preq), known != 0)))) {
    result = 1;
  }
  return result;
}

/* 004c09b0 auto */
iptr __cdecl civ_pick_research_advance(iptr civ)

{
  int value;
  int ai_value;
  iptr best_advance;
  int candidates;
  iptr advance;
  int best_score;
  int score;
  
  best_score = -1;
  best_advance = -1;
  candidates = 0;
  for (advance = 0; advance < 100; advance++) {
    value = civ_can_research_advance(civ,advance);
    if (((value != 0) && ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2))))
       && (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
           (((candidates == 0 || (game.difficulty == DIFF_CHIEFTAIN)) ||
            ((int)(advance - (uint)civs[civ].num_advances) % 3 != 0)))))) {
      candidates++;
      if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
        value = ai_advance_value(civ,advance);
        if (value == 1 || value - 1 < 0) {
          score = 0;
        }
        else {
          score = _rand();
          value = ai_advance_value(civ,advance);
          score = score % value;
        }
      }
      else {
        value = _rand();
        ai_value = ai_advance_value(civ,advance);
        score = value % 3 + ai_value - 1;
      }
      if (best_score < score) {
        best_score = score;
        best_advance = advance;
      }
    }
  }
  return best_advance;
}

/* 004c0b51 auto */
i32 __cdecl
research_option_draw_advance(i32 dialog,iptr surface,iptr advance,byte flags,int x,int y,
            int height)

{
  i8 draw_ctx[16];
  int draw_y;
  int draw_x;
  int y_offset;
  
  draw_x = x;
  if ((flags & 1) != 0) {
    draw_x = x + 0x26;
  }
  y_offset = (0x14 - height) / 2;
  draw_y = y - y_offset;
  ((Sprite *)
             (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
             (char)advances[advance].category * 0xf0 + (char)advances[advance].epoch * 0x3c))->draw(
             (iptr)draw_ctx,surface,draw_x,draw_y);
  return 0;
}

/* 004c0be8 auto */
i32 __cdecl
research_option_draw_unit(i32 dialog,iptr surface,int unit_type,byte flags,int x,int y,
            int height)

{
  i8 draw_ctx[16];
  i32 zoom2;
  int draw_y;
  int draw_x;
  int y_offset;
  int sprite_height2;
  
  zoom2 = 0xfffffffe;
  draw_x = x;
  if ((flags & 1) != 0) {
    draw_x = x + 0x26;
  }
  draw_y = y;
  sprite_height2 = gfx_zoom_scale(0x30,0xfffffffe);
  y_offset = (sprite_height2 - height) / 2;
  draw_y -= y_offset;
  gfx_set_zoom_scale(zoom2);
  ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->draw((iptr)draw_ctx,surface,draw_x,
             draw_y);
  gfx_reset_zoom_scale();
  return 0;
}

/* 004c0c83 auto */
i32 __cdecl
research_option_draw_improvement(i32 dialog,iptr surface,int improvement,byte flags,int x,int y,
            int height)

{
  i8 draw_ctx[16];
  int draw_y;
  int draw_x;
  int y_offset;
  
  draw_x = x;
  if ((flags & 1) != 0) {
    draw_x = x + 0x26;
  }
  y_offset = (0x14 - height) / 2;
  draw_y = y - y_offset;
  ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c))->draw((iptr)draw_ctx,surface,draw_x,
             draw_y);
  return 0;
}

/* Part of civ_research_goal_dialog (004c0cf7): the options of the
   goal list by mode: the advances not known (0), the units (1) or the
   improvements (2) they allow; option_count2 counts them. */
static void research_goal_add_options(iptr civ,int exclude_current,int mode,Dialog &dialog2,
                                                 int &option_count2)
{
  int known;
  iptr improvement;
  iptr advance;
  iptr unit_type;

  if (mode == 0) {
    dialog2.set_columns(2);
    if (diplo_meeting.session_active == 0) {
      dialog2.set_list_indent(0x4b,0);
      dialog2.set_option_draw_fn((DialogOptionDrawFn)research_option_draw_advance);
    }
    for (advance = 0; advance < 100; advance++) {
      if (((advances[advance].enabled != '\0') &&
          (known = civ_has_advance(civ,advance), known == 0)) &&
         ((exclude_current == 0 ||
          ((civs[civ].researching != advance &&
           ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))))))) {
        text_clear();
        text_append_string_id(advances[advance].name);
        if (advance == ADV_FUTURE_TECH) {
          text_append_space();
          text_append_int(civs[civ].future_techs + 1);
        }
        dialog2.add_option((const char *)&textbuf.buf,advance,0);
        option_count2++;
      }
    }
  }
  else if (mode == 1) {
    if (diplo_meeting.session_active == 0) {
      dialog2.set_list_indent(0x4b,0);
      dialog2.set_option_draw_fn((DialogOptionDrawFn)research_option_draw_unit);
    }
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      advance = (iptr)unit_types[unit_type].prerequisite;
      if (((-1 < advance) && (known = civ_has_advance(civ,advance), known == 0)) &&
         ((exclude_current == 0 || (civs[civ].researching != advance)))) {
        text_clear();
        text_append_string_id(unit_types[unit_type].name);
        text_append_space();
        text_append((const char *)&str_empty_120);
        text_append_lparen();
        text_append_string_id(advances[advance].name);
        text_append_rparen();
        dialog2.add_option((const char *)&textbuf.buf,unit_type,0);
        option_count2++;
      }
    }
  }
  else if (mode == 2) {
    if (diplo_meeting.session_active == 0) {
      dialog2.set_list_indent(0x4b,0);
      dialog2.set_option_draw_fn((DialogOptionDrawFn)research_option_draw_improvement);
    }
    for (improvement = 0; improvement < 0x43; improvement++) {
      advance = (iptr)improvements[improvement].preq;
      if (((-1 < advance) && (known = civ_has_advance(civ,advance), known == 0)) &&
         ((exclude_current == 0 || (civs[civ].researching != advance)))) {
        text_clear();
        text_append_string_id(improvements[improvement].name);
        text_append_space();
        text_append((const char *)&str_empty_121);
        text_append_lparen();
        text_append_string_id(advances[advance].name);
        text_append_rparen();
        dialog2.add_option((const char *)&textbuf.buf,improvement,0);
        option_count2++;
      }
    }
  }
}

/* Part of civ_research_goal_dialog (004c0cf7): the help on a choice of the
   list (mode as above): the Pedia page of the advance, or a choice between
   the unit or improvement and its advance. */
static void research_goal_help(int mode,iptr choice,Dialog &dialog2)
{
  iptr label;
  iptr option_id;
  i32 option_flags;
  iptr improvement;
  iptr advance;
  iptr unit_type;

  if (mode == 0) {
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(choice);
  }
  else if (mode == 1) {
    advance = (iptr)unit_types[choice].prerequisite;
    unit_type = choice;
    dialog2.load_game_section((const char *)&str_helpon,1);
    option_flags = 0;
    option_id = 0;
    label = (iptr)label_string(unit_types[unit_type].name);
    dialog2.add_option((const char *)label,option_id,option_flags);
    option_flags = 0;
    option_id = 1;
    label = (iptr)label_string(advances[advance].name);
    dialog2.add_option((const char *)label,option_id,option_flags);
    choice = dialog2.show_modal(0);
    if (choice == 0) {
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_unit_modal(unit_type);
    }
    if (choice == 1) {
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(advance);
    }
  }
  else if (mode == 2) {
    advance = (iptr)improvements[choice].preq;
    improvement = choice;
    dialog2.load_game_section((const char *)&str_helpon_2,1);
    option_flags = 0;
    option_id = 0;
    label = (iptr)label_string(improvements[improvement].name);
    dialog2.add_option((const char *)label,option_id,option_flags);
    option_flags = 0;
    option_id = 1;
    label = (iptr)label_string(advances[advance].name);
    dialog2.add_option((const char *)label,option_id,option_flags);
    choice = dialog2.show_modal(0);
    if (choice == 0) {
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_improvement_modal(improvement);
    }
    if (choice == 1) {
      ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(advance);
    }
  }
}

/* 004c0cf7 auto */
void __cdecl civ_research_goal_dialog(iptr civ,int exclude_current,int civ2)

{
  iptr label;
  int known;
  iptr path_advances[99];
  int mode_state[4];
  iptr choice;
  iptr advance;
  int option_count2;
  iptr path_count;
  iptr button_count;
  Dialog dialog2;
  iptr i;
  
  dialog2.Dialog::construct(0x2000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  mode_state[1] = 0;
  while( true ) {
    while( true ) {
      option_count2 = 0;
      dialog2.load_game_section((const char *)&str_researchgoal,1);
      research_goal_add_options(civ,exclude_current,mode_state[1],dialog2,option_count2);
      if (option_count2 == 0) {
        return;
      }
      button_count = 0;
      if (mode_state[1] != 0) {
        label = (iptr)label_string(label_table.string_ids[0xc3]);
        dialog2.add_button((const char *)label);
        mode_state[button_count + 2] = 0;
        button_count++;
      }
      if (mode_state[1] != 1) {
        label = (iptr)label_string(label_table.string_ids[0xc4]);
        dialog2.add_button((const char *)label);
        mode_state[button_count + 2] = 1;
        button_count++;
      }
      if (mode_state[1] != 2) {
        label = (iptr)label_string(label_table.string_ids[0xc5]);
        dialog2.add_button((const char *)label);
        mode_state[button_count + 2] = 2;
        button_count++;
      }
      choice = dialog2.show_modal(0);
      if (choice < 0) {
        return;
      }
      if (dialog2.result_extra != 1) break;
      if (diplo_meeting.session_active == 0) {
        research_goal_help(mode_state[1],choice,dialog2);
      }
    }
    if (((int)dialog2.result_extra < 2) || (3 < (int)dialog2.result_extra)) break;
    mode_state[1] = mode_state[dialog2.result_extra];
  }
  if (mode_state[1] == 1) {
    choice = (iptr)unit_types[choice].prerequisite;
  }
  else if (mode_state[1] == 2) {
    choice = (iptr)improvements[choice].preq;
  }
  option_count2 = 0;
  path_count = 0;
  for (advance = 0; advance <= 99; advance++) {
    if ((advances[advance].enabled != '\0') &&
       (known = civ_has_advance(civ,advance), known == 0)) {
      if (civ2 == 0) {
        known = civ_has_advance(civ,(int)advances[advance].preq[0]);
        if (!(((known != 0) &&
              (known = civ_has_advance(civ,(int)advances[advance].preq[1]), known != 0)) &&
             ((exclude_current == 0 || (civs[civ].researching != advance))))) continue;
        if (((option_count2 != 0) && (game.difficulty != DIFF_CHIEFTAIN)) && ((game.game_flags & GAME_SCENARIO) == 0)) {
          known = (int)(advance - ((uint)civs[civ].num_advances + exclude_current)) % 3;
          if (known == 0) continue;
        }
      }
      else {
        known = civ_has_advance(civ2,advance);
        if (known == 0) continue;
      }
      if ((advances[advance].preq[0] != -2) || (advances[advance].preq[1] != -2)) {
        option_count2++;
        known = advance_has_prereq(choice,advance);
        if (known != 0) {
          path_advances[path_count] = advance;
          path_count++;
        }
      }
    }
  }
  text_set_label_arg(0,advances[choice].name);
  if (path_count == 0) {
    if (civ2 == 0) {
      popup_game((const char *)&str_researchnone);
    }
    else {
      popup_game((const char *)&str_stealnone);
    }
  }
  else {
    if (civ2 == 0) {
      dialog2.load_game_text((const char *)&str_researchthese);
    }
    else {
      dialog2.load_game_text((const char *)&str_stealthese);
    }
    for (i = 0; i < path_count; i++) {
      advance = path_advances[i];
      text_clear();
      text_append_string_id(advances[advance].name);
      if ((i < path_count - 1) && (2 < path_count)) {
        text_append_comma();
      }
      else if (path_count - 2 == i) {
        text_append_space();
      }
      if (path_count - 2 == i) {
        text_append_label(0xb5);
      }
      if (path_count - 1 == i) {
        text_append_period();
      }
      dialog2.add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
    }
    dialog2.show_modal(0);
  }
  return;
}

/* 004c195e auto */
iptr __cdecl civ_choose_research_dialog(iptr civ,int forced)

{
  iptr advance2;
  iptr ticks;
  int known;
  iptr selected;
  int hint_kind;
  iptr improvement;
  iptr advance;
  iptr count;
  iptr hint_items[5];
  iptr unit_type;
  iptr chosen;
  Dialog choice_dialog;
  iptr advance3;
  Dialog info_dialog;
  iptr i;
  
  choice_dialog.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(choice_dialog.Dialog::destruct());
  info_dialog.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(info_dialog.Dialog::destruct());
  chosen = civ_pick_research_advance(civ);
  if ((forced != 0) ||
     (((advance2 = chosen, game.turn != 0 &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) && (autoplay.active == 0)))) {
    while( true ) {
      advance2 = chosen;
      if ((((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) && (forced == 0)) {
        text_clear();
        text_append_string_id(advances[advance2].name);
        if (advance2 == ADV_FUTURE_TECH) {
          text_append_space();
          text_append_int(civs[civ].future_techs + 1);
        }
        text_set_string_arg(0,(const char *)&textbuf.buf);
        hint_kind = 0;
        count = 0;
        for (unit_type = 0; unit_type < 0x3e; unit_type++) {
          if ((unit_types[unit_type].prerequisite == advance2) && (count < 5)) {
            hint_items[count] = unit_type;
            count++;
          }
        }
        for (improvement = 1; improvement < 0x43; improvement++) {
          if ((improvements[improvement].preq == advance2) && (count < 5)) {
            hint_items[count] = -improvement;
            count++;
          }
        }
        if (count == 0) {
          for (advance3 = 0; advance3 < 100; advance3++) {
            if ((advances[advance3].enabled != '\0') &&
               (((advances[advance3].preq[0] == advance2 || (advances[advance3].preq[1] == advance2)) &&
                (count < 5)))) {
              hint_items[count] = advance3;
              count++;
            }
          }
          if (count != 0) {
            hint_kind = 2;
          }
        }
        else {
          hint_kind = 1;
        }
        text_clear();
        text_append((const char *)&str_science);
        text_append_int(hint_kind);
        info_dialog.load_text((const char *)&str_advice,(const char *)&textbuf.buf,8);
        info_dialog.add_sprite(
                   (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                         (char)advances[advance2].epoch * 0x3c + (char)advances[advance2].category * 0xf0)
                   ,(ui_metrics.large_screen == 0) - 1 & 8,0);
        if (hint_kind != 0) {
          text_clear();
          for (i = 0; i < count; i++) {
            if (hint_kind == 1) {
              unit_type = hint_items[i];
              if (unit_type < 0) {
                text_append_string_id(improvements[-unit_type].name);
              }
              else {
                text_append_string_id(unit_types[unit_type].name);
              }
            }
            else {
              text_append_string_id(advances[hint_items[i]].name);
            }
            if (i < count - 1) {
              if (count < 3) {
                text_append_space();
              }
              else {
                text_append_comma();
              }
              if (count - 2 == i) {
                text_append_label(0xdb);
                text_append_space();
              }
            }
            else {
              text_append_period();
            }
          }
          info_dialog.add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
        }
        ticks = timer_ticks();
        info_dialog.run_modal(0,ticks - 300);
      }
      known = civ_has_advance(civ,ADV_INVENTION);
      text_set_label_arg(0,label_table.string_ids[0x31 - (uint)(known == 0)]);
      choice_dialog.load_game_text((const char *)&str_research);
      if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
        choice_dialog.set_position(5,0xfffffc19);
      }
      choice_dialog.set_option_draw_fn((DialogOptionDrawFn)research_option_draw_advance);
      choice_dialog.set_list_indent(0x4b,0);
      count = 0;
      for (advance = 0; advance < 100; advance++) {
        if (((((advances[advance].enabled != '\0') &&
              (known = civ_has_advance(civ,advance), known == 0)) &&
             ((known = civ_has_advance(civ,(int)advances[advance].preq[0]), known != 0 &&
              (known = civ_has_advance(civ,(int)advances[advance].preq[1]), known != 0))))
            && ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))) &&
           ((count == 0 ||
            ((((game.difficulty == DIFF_CHIEFTAIN || (forced != 0)) || ((game.game_flags & GAME_SCENARIO) != 0)) ||
             ((int)(advance - (uint)civs[civ].num_advances) % 3 != 0)))))) {
          text_clear();
          text_append_string_id(advances[advance].name);
          if (advance == ADV_FUTURE_TECH) {
            text_append_space();
            text_append_int(civs[civ].future_techs + 1);
          }
          choice_dialog.add_option((const char *)&textbuf.buf,advance,0);
          count++;
        }
      }
      advance2 = chosen;
      if (count == 0) break;
      selected = choice_dialog.run_modal(0,300);
      if (((*((char *)&game.game_options + 1)) & (OPT_INSTANT_ADVICE >> 8)) != 0) {
        info_dialog.close();
        ui_flush_input();
      }
      if (choice_dialog.result_extra == 2) {
        civ_research_goal_dialog(civ,0,0);
      }
      else {
        advance2 = selected;
        if (choice_dialog.result_extra == 0) break;
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(selected);
      }
    }
  }
  chosen = advance2;
  advance2 = chosen;
  civs[civ].researching = (i16)chosen;
  return advance2;
}

/* 004c21ad auto */
void __cdecl civ_choose_research(iptr civ)

{
  civ_choose_research_dialog(civ,0);
  return;
}

/* 004c21d5 auto */
iptr __cdecl civ_research_complete(uptr civ,iptr civ2)

{
  short current_research;
  iptr city;
  iptr adjective;
  int has_philosophy;
  iptr advance;
  Dialog dialog2;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  current_research = civs[civ].researching;
  if (civs[civ].researching < 0) {
    civ_choose_research(civ);
    current_research = civs[civ].researching;
  }
  advance = (iptr)current_research;
  if (advance >= 0) {
    if (research_notice_mode != 2) {
      civs[civ].research_progress = 0;
    }
    civs[civ].researching = -1;
    civ_give_advance(civ,advance,civ2,0,0);
    if ((game.turn == 0) || ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
      city = ((CityWindow *)((i8 *)&city_wnd.window))->get_city();
      civ_reset_city_production(civ,-1);
      if (((game.turn != 0) && (-1 < city)) &&
         ((city < game.num_cities &&
          ((cities[city].id != 0 && ((int)cities[city].owner == civ)))))) {
        city_recalc(city,0);
      }
    }
    else if ((human_civ[0] == civ) && (autoplay.active == 0)) {
      adjective = civ_adjective(civ);
      text_set_string_arg_b(0,(const char *)adjective);
      has_philosophy = civ_has_advance(civ,ADV_INVENTION);
      text_set_label_arg(1,label_table.string_ids[0x31 - (uint)(has_philosophy == 0)]);
      text_clear();
      text_append_string_id(advances[advance].name);
      if (advance == ADV_FUTURE_TECH) {
        text_append_space();
        text_append_int((uint)civs[civ].future_techs);
      }
      text_set_string_arg_b(2,(const char *)&textbuf.buf);
      dialog2.load_game_text((const char *)&str_civadvance);
      dialog2.add_sprite(
                 (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                       (char)advances[advance].category * 0xf0 +
                       (char)advances[advance].epoch * 0x3c),0,0);
      dialog2.set_image_zoom(8);
      dialog2.show_modal(0);
      if (((*((char *)&game.game_options + 2)) & (OPT_PEDIA_ADVANCES >> 16)) != 0) {
        ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_topic_modal(advance);
      }
      if ((((advance == ADV_PHILOSOPHY) && ((civs[civ].flags & CIVF_FREE_ADVANCE) != 0)) &&
          (human_civ[0] == civ)) && (autoplay.active == 0)) {
        civ_golden_age_popup(civ);
      }
      if (unit_types[UNIT_ENGINEERS].prerequisite == advance) {
        popup_game_with_sprite((const char *)&str_newxform,(iptr)&pollution_sprite_block.research_popup_sprite,8);
      }
      if (advance == ADV_CONSTRUCTION) {
        popup_game_with_sprite((const char *)&str_newfortress,(iptr)&popup_info_icon_sprite,0);
      }
      if (improvements[IMP_AIRPORT].preq == advance) {
        popup_game_with_sprite((const char *)&str_newairlift,(iptr)&caption_close_button_sprite_block.research_popup_sprite3,8);
      }
      if (advance == ADV_RAILROAD) {
        popup_game_with_sprite((const char *)&str_newrailroad,(iptr)&road_sprites_block.railroad_sprites,8);
      }
      if (improvements[IMP_SUPERMARKET].preq == advance) {
        dialog2.load_game_text((const char *)&str_newfarmland);
        dialog2.add_sprite((iptr)&caption_close_button_sprite_block.research_dialog_sprite,0,0);
        dialog2.add_sprite((iptr)&land_improvement_sprites_block.farmland_sprite,0,0);
        dialog2.show_modal(0);
      }
      if (unit_types[UNIT_PARATROOPERS].prerequisite == advance) {
        text_set_number_arg_u8(0,CONCAT31(unit_types[UNIT_PARATROOPERS].prerequisite >> 7,cosmic.paradrop_range));
        popup_game_with_sprite((const char *)&str_newparadrop,(iptr)&pollution_sprite_block.research_popup_sprite2,8);
      }
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (game.turn != 0)) {
        civ_choose_research(civ);
        civ_advance_gov_prompts(civ,advance);
      }
    }
  }
  return advance;
}

/* 004c2788 auto */
int __cdecl civ_research_cost(iptr civ)

{
  uint tech_count;
  int extra;
  int cost2;
  int multiplier;
  int turn_penalty;
  
  tech_count = (uint)civs[civ].num_advances + (uint)civs[civ].research_cost_advances;
  if (tech_count < 2) {
    tech_count = 1;
  }
  multiplier = int_clamp((uint)game.difficulty,0,4);
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    multiplier = 0xe - multiplier;
  }
  else {
    multiplier = multiplier * 2 + 6;
  }
  if (((game.game_flags & GAME_SCENARIO) == 0) || (scenario.year_increment[0] == 0)) {
    if (tech_count < (uint)civs[game.best_human_rank].num_advances + (uint)civs[game.best_human_rank].research_cost_advances) {
      if (game.difficulty != DIFF_CHIEFTAIN) {
        multiplier--;
      }
      if (((game.difficulty == DIFF_DEITY) &&
          (tech_count + 4 < (uint)civs[game.best_human_rank].num_advances + (uint)civs[game.best_human_rank].research_cost_advances
          )) && (0x96 < game.turn)) {
        multiplier--;
      }
    }
    else {
      multiplier = multiplier +
                 (int)(tech_count - ((uint)civs[game.best_human_rank].num_advances +
                               (uint)civs[game.best_human_rank].research_cost_advances)) / 3;
    }
    turn_penalty = 0;
    if (0x13 < tech_count) {
      turn_penalty = int_clamp(tech_count - ((int)((int)game.turn + ((int)game.turn >> 0x1f & 7U))
                                            >> 3),0,6);
    }
    multiplier += turn_penalty;
  }
  if ((game.game_flags & GAME_SCENARIO) == 0) {
    if (cosmic.tech_paradigm != '\n') {
      multiplier = (int)((uint)cosmic.tech_paradigm * multiplier) / 10;
    }
  }
  else if (scenario.tech_paradigm != 10) {
    multiplier = (scenario.tech_paradigm * multiplier) / 10;
  }
  extra = multiplier * 3 >> 2;
  if ((uint)civs[civ].num_advances + (uint)civs[civ].research_cost_advances < 0x14) {
    extra = (int)(((uint)civs[civ].num_advances + (uint)civs[civ].research_cost_advances) * extra) /
               0x14;
  }
  multiplier += extra;
  if (0x43 < game.num_advances) {
    multiplier = (multiplier * 0x43) / (int)game.num_advances;
  }
  if ((game.game_flags & GAME_LARGE_MAP) != 0) {
    multiplier = (int)(multiplier * 5 + (multiplier * 5 >> 0x1f & 3U)) >> 2;
  }
  if ((game.game_flags & GAME_SMALL_MAP) != 0) {
    multiplier = (multiplier << 2) / 5;
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
     (multiplier < (int)(0xb - tech_count))) {
    multiplier = 0xb - tech_count;
  }
  cost2 = multiplier * tech_count;
  if ((cost2 < 1) || (32000 < cost2)) {
    cost2 = 32000;
  }
  return cost2;
}

/* 004c2b73 auto */
void __cdecl civ_add_research_points(uptr civ,int points)

{
  short progress;
  int cost;
  
  if (0 < points) {
    civs[civ].research_progress += (short)points;
    if (-1 < civs[civ].researching) {
      progress = civs[civ].research_progress;
      cost = civ_research_cost(civ);
      if (cost <= progress) {
        civ_research_complete(civ,0);
      }
      if ((civs[civ].flags & CIVF_FREE_ADVANCE) != 0) {
        civs[civ].flags = civs[civ].flags & ~CIVF_FREE_ADVANCE;
        civ_research_complete(civ,0);
      }
    }
    if ((civs[civ].research_progress != 0) && (civs[civ].researching < 0)) {
      if ((((*((char *)&game.game_options + 1)) & (OPT_TUTORIAL >> 8)) != 0) &&
         ((civs[civ].num_advances < 2 &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)))) {
        popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_firstciv,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      }
      civ_choose_research(civ);
    }
  }
  return;
}

/* 004c65d2 auto */
int __cdecl civ_distance_to_capital(iptr civ,i32 target_x,i32 target_y)

{
  int dist;
  iptr city;
  int min_dist;
  
  min_dist = 0x10;
  for (city = 0; city < game.num_cities; city++) {
    if ((((cities[city].id != 0) && (cities[city].owner == civ)) &&
        (dist = city_has_improvement(city,IMP_PALACE), dist != 0)) &&
       (dist = map_distance_diag((int)cities[city].x,(int)cities[city].y,target_x,target_y),
       dist < min_dist)) {
      min_dist = dist;
    }
  }
  return min_dist;
}

/* 004ccab9 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl rules_edit_set_section(iptr names,RulesSectionWriterFn writers)

{
  font_size16_bold_block.rules_edit_sections[0].name = names;
  font_size16_bold_block.rules_edit_sections[0].writer = writers;
  font_size16_bold_block.rules_edit_sections[1].name = 0;
  font_size16_bold_block.rules_edit_sections[1].writer = 0;
  return;
}

/* 004ccaed auto */
void __cdecl rules_edit_add_section(const char *name2,RulesSectionWriterFn writer)

{
  int slot;
  
  for (slot = 0; (font_size16_bold_block.rules_edit_sections[slot].name != 0 && (slot < 7));
      slot = slot + 1) {
  }
  if (slot < 7) {
    font_size16_bold_block.rules_edit_sections[slot].name = (iptr)name2;
    font_size16_bold_block.rules_edit_sections[slot].writer = writer;
    font_size16_bold_block.rules_edit_sections[slot + 1].name = 0;
    font_size16_bold_block.rules_edit_sections[slot + 1].writer = 0;
  }
  return;
}

/* 004ccf2d auto */
i32 __cdecl rules_write_sections(void)

{
  iptr cmp_result;
  iptr exists;
  int rc;
  const char *line_ptr;
  size_t line_len;
  const char *lpCaption;
  UINT uType;
  i32 success;
  char header[80];
  FILE *out_file;
  FILE *in_file;
  char saved_cwd[260];
  i8 rules_name[16];
  int section;
  char s[256];
  
  section = 0;
  success = 0;
  out_file = (FILE *)0x0;
  in_file = (FILE *)0x0;
  crt_strcpy(rules_name,str_rules_5);
  crt_strcat(rules_name,((i8 *)&lang_file_ext));
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  ON_SCOPE_EXIT(
    if (in_file != (FILE *)0x0) {
      _fclose(in_file);
    }
    if (out_file != (FILE *)0x0) {
      _fclose(out_file);
    }
    __chdir(saved_cwd));
  cmp_result = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  if ((cmp_result == 0) || (exists = file_exists((const char *)rules_name), exists == 0)) {
    __chdir(((i8 *)&game_dirs.home));
    in_file = file_open_path((const char *)rules_name,(iptr)&str_rt_4);
    rc = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
    if (rc == 0) {
      rc = file_exists((const char *)&str_rules_bak_5);
      if ((rc != 0) && (rc = crt_remove(str_rules_bak_6), rc != 0))
      return success;
      out_file = file_open_path((const char *)&str_rules_bak_7,(iptr)&str_wt_2);
      uType = 0x40;
      lpCaption = str_warning_2;
      line_ptr = str_saving_changes_in_file_rules_bak;
      rc = (int)((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(((NativeWindow *)rc)->hwnd,line_ptr,lpCaption,uType);
    }
    else {
      __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
      out_file = file_open_path((const char *)rules_name,(iptr)&str_wt_3);
    }
  }
  else {
    rc = file_exists((const char *)&str_rules_bak);
    if (((rc != 0) && (rc = crt_remove(str_rules_bak_2), rc != 0)) ||
       (rc = crt_rename(rules_name,str_rules_bak_3), rc != 0)) return success;
    in_file = file_open_path((const char *)&str_rules_bak_4,(iptr)&str_rt_3);
    out_file = file_open_path((const char *)rules_name,(iptr)&str_wt);
  }
  if ((in_file != (FILE *)0x0) && (out_file != (FILE *)0x0)) {
    for (; font_size16_bold_block.rules_edit_sections[section].name != 0; section++) {
      header[0] = '@';
      header[1] = 0;
      crt_strcat(header,(char *)font_size16_bold_block.rules_edit_sections[section].name);
      __strupr(header);
      do {
        line_ptr = _fgets(s,0x100,in_file);
        if ((line_ptr == (char *)0x0) || (rc = _fputs(s,out_file), rc == -1))
        return success;
        str_strip_newline(s);
        str_trim((char *)s);
        rc = __strcmpi(s,header);
      } while (rc != 0);
      if (font_size16_bold_block.rules_edit_sections[section].writer != 0) {
        font_size16_bold_block.rules_edit_sections[section].writer(out_file,in_file);
      }
      do {
        line_ptr = _fgets(s,0x100,in_file);
        if (line_ptr == (char *)0x0) return success;
        line_len = _strlen(s);
      } while (1 < line_len);
      rc = _fputs(s,out_file);
      if (rc == -1) return success;
    }
    do {
      line_ptr = _fgets(s,0x100,in_file);
      if (line_ptr == (char *)0x0) {
        success = 1;
        break;
      }
      rc = _fputs(s,out_file);
    } while (rc != -1);
  }
  return success;
}

/* 004e1314 auto */
void __cdecl rules_apply_double_moves(void)

{
  iptr unit_type;
  
  if (((game.multiplayer == '\0') || (mp_doubles.moves == 0)) || (double_moves_applied != 0)) {
    if (((game.multiplayer != '\0') && (mp_doubles.moves == 0)) && (double_moves_applied == 1)) {
      double_moves_applied = 0;
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        if (unit_types[unit_type].domain == DOMAIN_LAND) {
          unit_types[unit_type].move = (char)unit_types[unit_type].move / 2;
        }
      }
      status_redraw(1);
    }
  }
  else {
    double_moves_applied = 1;
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      if (unit_types[unit_type].domain == DOMAIN_LAND) {
        unit_types[unit_type].move = unit_types[unit_type].move * 2;
      }
    }
    status_redraw(1);
  }
  return;
}

/* 004e1763 auto */
void __cdecl civ_eliminate(iptr civ,i32 kill_units,i32 recreate)

{
  uint *treaty_ptr;
  char *count_ptr;
  short x3;
  short y4;
  bool relinked;
  int x4;
  int y2;
  iptr x;
  int y;
  int status;
  uint fertility;
  iptr net_flag;
  iptr unit;
  iptr city;
  uint seen_mask;
  int y3;
  int x2;
  iptr unit2;
  byte owner_bit;
  iptr city2;
  
  relinked = false;
  net.civ_eliminate_busy = 1;
  if ((game.multiplayer < 3) || (net.host_flag != '\0')) {
    game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
    if (2 < game.multiplayer) {
      net_send_message(NM_UNLOCK_NATION,0,0,civ,0,0,0,0,0,0);
    }
    if ((mp_options.kill_civ_on_retire != 0) || ((char)kill_units != '\0')) {
      net_batch_begin();
      for (unit = 0; unit < game.num_units; unit++) {
        if (units[unit].id != 0) {
          if (((units[unit].owner == civ) || ((units[unit].flags & UNITF_SPY_ON_MISSION) == 0)) ||
             ((char)units[unit].counter != civ)) {
            if (units[unit].owner == civ) {
              if ((-1 < civ) && (unit < 0x800)) {
                if ((civs[civ].num_military_units != 0) &&
                   ((char)unit_types[units[unit].type].role < 5)) {
                  civs[civ].num_military_units--;
                }
                if (*(char *)&civs[civ].units_active[units[unit].type] != '\0') {
                  count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
                  *count_ptr = *count_ptr - 1;
                }
                if (civ_unit_counts_block.civ_unit_totals[civ] != 0) {
                  civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] - 1
                  ;
                }
              }
              if (-1 < units[unit].prev_in_stack) {
                units[units[unit].prev_in_stack].next_in_stack = units[unit].next_in_stack;
                relinked = true;
              }
              if (-1 < units[unit].next_in_stack) {
                units[units[unit].next_in_stack].prev_in_stack = units[unit].prev_in_stack;
                relinked = true;
              }
              if ((!relinked) &&
                 (x4 = map_in_bounds((int)units[unit].x,(int)units[unit].y), x4 != 0))
              {
                tile_set_improvements((int)units[unit].x,(int)units[unit].y,1,0,1);
              }
              units[unit].prev_in_stack = -1;
              units[unit].next_in_stack = -1;
              units[unit].x = (units[unit].owner * 4 + 4) * -0x19;
              units[unit].y = (units[unit].owner * 4 + 4) * -0x19;
              units[unit].id = 0;
              if (game.num_units - 1 == unit) {
                game.num_units--;
              }
              for (unit2 = 0; unit2 < game.num_units; unit2++) {
                if ((units[unit2].orders == ORDER_SLEEP) && (units[unit2].goto_x == unit)) {
                  units[unit2].orders = ORDER_NONE;
                }
              }
            }
          }
          else {
            units[unit].flags = units[unit].flags & ~UNITF_SPY_ON_MISSION;
            treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + (char)units[unit].counter * 4);
            *treaty_ptr = *treaty_ptr & 0xfeffffff;
            spy_try_escape(unit,0,(int)(char)units[unit].counter);
          }
        }
      }
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city].owner == civ)) {
          x4 = (int)cities[city].x;
          y2 = (int)cities[city].y;
          civ = (iptr)cities[city].owner;
          civs[civ].num_cities--;
          cities[city].id = 0;
          if (game.num_cities - 1 == city) {
            game.num_cities--;
          }
          for (city2 = 0; city2 < game.num_cities; city2++) {
            if (cities[city2].id != 0) {
              unit = (iptr)(char)cities[city2].trade_routes;
              while (unit = unit - 1, -1 < unit) {
                if (cities[city2].route_partner[unit] == city) {
                  city_remove_trade_route(city2,unit);
                }
              }
            }
          }
          for (city2 = 0; city2 < 0x1c; city2++) {
            if (game.wonder_city[city2] == city) {
              game.wonder_city[city2] = -2;
            }
          }
          seen_mask = 0;
          tile_set_improvements(x4,y2,2,0,1);
          for (city2 = 0; city2 < 0x2d; city2++) {
            x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[city2] + x4);
            y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[city2] + y2;
            status = map_in_bounds(x,y);
            if ((status != 0) && (status = tile_is_ocean(x,y), status == 0)) {
              net_flag = 1;
              fertility = tile_get_fertility(x,y);
              tile_set_fertility(x,y,fertility | 8,net_flag);
              if (city2 < 0x15) {
                status = tile_radius_civ(x,y);
                if (status == civ) {
                  tile_set_radius_civ(x,y,0,1);
                }
                y = tile_city_or_unit_owner(x,y);
                if ((0 < y) && (y != civ)) {
                  owner_bit = (byte)y;
                  seen_mask = seen_mask | 1 << (owner_bit & 0x1f);
                }
              }
            }
          }
          if (seen_mask != 0) {
            for (unit = tile_first_unit(x4,y2); -1 < unit;
                unit = unit_next_in_stack(unit)) {
              units[unit].visibility = units[unit].visibility | (byte)seen_mask;
            }
          }
        }
      }
      for (city = 0; city < game.num_cities; city++) {
        x3 = cities[city].x;
        y4 = cities[city].y;
        for (city2 = 0; city2 < 0x2d; city2++) {
          x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[city2] + (int)x3);
          x4 = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[city2] + (int)y4;
          y2 = map_in_bounds(x,x4);
          if (y2 != 0) {
            net_flag = 1;
            fertility = tile_get_fertility(x,x4);
            tile_set_fertility(x,x4,fertility & 7,net_flag);
            if (city2 < 0x15) {
              tile_set_radius_civ(x,x4,civ,1);
            }
          }
        }
      }
      civ_reset_spaceship(civ);
      game.civs_alive = game.civs_alive & ~(byte)(1 << ((byte)civ & 0x1f));
      for (x2 = 0; x2 < map.width; x2++) {
        for (y3 = 0; y3 < map.height; y3++) {
          tile_set_visible(x2,y3,1 << ((byte)civ & 0x1f),0,1);
        }
      }
      net_batch_flush();
    }
    passwords_unscramble();
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[civ * 0x20] = 0;
    passwords_scramble();
    if (2 < game.multiplayer) {
      snd_cd_stop();
      net_send_state_diff(0xff,2,0,0,0);
      XD_FlushSendBuffer(5000);
    }
    if ((~(1 << ((byte)civ & 0x1f)) & (uint)game.human_players) == 0) {
      net.civ_eliminate_busy = 0;
    }
    else {
      if ((char)recreate != '\0') {
        civ_create(civ);
      }
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
        net_send_message(NM_DRAW_MAP,0xff,0,0,0,0,0,0,0,0);
      }
      map_redraw_all_views(human_civ[0],1);
      net.civ_eliminate_busy = 0;
    }
  }
  else {
    net.civ_eliminate_busy = 0;
    net.kill_or_quit_ack = -2;
    net_send_message(NM_KILL_OR_QUIT,0,civ,0,0,0,0,0,0,0);
    x4 = timer_ticks();
    while ((net.kill_or_quit_ack == -2 && (y2 = timer_ticks(), y2 - x4 < 0xe10))) {
      net.process_messages(1,1);
    }
    if (net.kill_or_quit_ack == -2) {
      dbg_log((const char *)&str_killorretire_connection_to_server_timed);
      popup_game_with_sprite((const char *)&str_serverconnecttime_6,(iptr)&popup_info_icon_sprite,0);
      turn_loop.active = 0;
    }
    else {
      dbg_log((const char *)&str_killorretire_received_nm_kill_or_quit_ac);
    }
    while ((net.msgq_priority_count != 0 || (net.msgq_normal_count != 0))) {
      net.process_messages(1,0);
    }
  }
  return;
}

/* 004ec312 auto */
void __cdecl civ_apply_eiffel_tower(iptr civ)

{
  uint *treaty_ptr;
  int civ2;
  
  if ((civs[civ].flags & CIVF_EIFFEL_APPLIED) == 0) {
    civs[civ].reputation = civs[civ].reputation >> 1;
    civs[civ].flags = civs[civ].flags | CIVF_EIFFEL_APPLIED;
    for (civ2 = 1; civ2 < 8; civ2++) {
      if (civ2 != civ) {
        civ_adjust_attitude(civ2,civ,0xffffffe7);
        treaty_ptr = (uint *)(civ2 * 0x594 + (iptr)((u8 *)&civs + 32) + civ * 4);
        *treaty_ptr = *treaty_ptr & 0xffffffef;
      }
    }
  }
  return;
}

/* 004f00f0 auto */
uint __cdecl civ_improvement_upkeep(iptr civ,iptr improvement)

{
  int has;
  iptr advance;
  uint cost2;
  
  cost2 = (uint)improvements[improvement].upkeep;
  if (improvement == IMP_BARRACKS) {
    if ((game.difficulty < DIFF_PRINCE) && (cost2 != 0)) {
      cost2--;
    }
    has = civ_has_advance(civ,ADV_GUNPOWDER);
    if (has != 0) {
      cost2++;
    }
    for (advance = 0x35; (-1 < advance && (advances[advance].enabled == '\0'));
        advance = (iptr)advances[advance].preq[0]) {
    }
    has = civ_has_advance(civ,advance);
    if (has != 0) {
      cost2++;
    }
  }
  if (cost2 == 1) {
    has = civ_has_wonder(civ,WONDER_ADAM_SMITH);
    if (has != 0) {
      cost2 = 0;
    }
  }
  if (((cost2 != 0) && (civs[civ].government == GOV_FUNDAMENTALISM)) &&
     ((improvement == IMP_TEMPLE || ((improvement == IMP_COLOSSEUM || (improvement == IMP_CATHEDRAL)))))) {
    cost2 = 0;
  }
  return cost2;
}

/* 0055c066 auto */
void __cdecl civ_set_government(iptr civ,iptr new_gov)

{
  uint *treaty_ptr;
  u8 old_government;
  iptr city;
  int civ2;
  
  old_government = civs[civ].government;
  civs[civ].government = (u8)new_gov;
  if (civs[civ].government != old_government) {
    if ((civs[civ].government == GOV_ANARCHY) ||
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
      for (civ2 = 0; civ2 < 8; civ2++) {
        treaty_ptr = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
        *treaty_ptr = *treaty_ptr & 0xffffffef;
      }
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
      game.transient_flags = game.transient_flags & ~TRANSIENT_ATTITUDE_DIRTY;
      for (city = 0; city < game.num_cities; city++) {
        if ((((cities[city].id != 0) && (cities[city].owner == civ)) &&
            (city_recalc(city,1), cities[city].building == UNIT_FANATICS)) &&
           (civs[civ].government != GOV_FUNDAMENTALISM)) {
          cities[city].building = UNIT_RIFLEMEN;
        }
      }
    }
  }
  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (0 < new_gov)) &&
     (autoplay.active == 0)) {
    taxrate_dialog_show(civ);
  }
  return;
}

/* 0055c277 auto */
i32 __cdecl
civ_can_choose_government(iptr civ,iptr government,i32 unused_3,i32 unused_4,i32 unused_5,
            iptr unused_6,iptr unused_7,i32 unused_8,iptr unused_9,iptr unused_10,iptr unused_11,
            i32 unused_12,iptr unused_13,i32 unused_14,i32 unused_15,
            i32 unused_16,i32 unused_17,i32 unused_18,i32 unused_19,
            i32 unused_20,i32 unused_21)

{
  int has_statue;
  int known;
  i32 result;
  
  result = 1;
  has_statue = civ_has_wonder(civ,WONDER_STATUE_OF_LIBERTY);
  switch(government) {
  case GOV_MONARCHY:
    known = civ_has_advance(civ,ADV_MONARCHY);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    break;
  case GOV_COMMUNISM:
    known = civ_has_advance(civ,ADV_COMMUNISM);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    break;
  case GOV_FUNDAMENTALISM:
    known = civ_has_advance(civ,ADV_FUNDAMENTALISM);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    if (advances[ADV_FUNDAMENTALISM].enabled == '\0') {
      result = 0;
    }
    break;
  case GOV_REPUBLIC:
    known = civ_has_advance(civ,ADV_REPUBLIC);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    break;
  case GOV_DEMOCRACY:
    known = civ_has_advance(civ,ADV_DEMOCRACY);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
  }
  return result;
}

/* 0055c3d3: written by hand in src/game/unwind.cpp */

/* 0055c69d auto */
void __cdecl civ_change_government(iptr civ,uptr new_gov)

{
  iptr text;
  int has_wonder;
  uint old_government;
  int notified;
  int civ2;
  uptr gov_rank;
  
  old_government = (uint)civs[civ].government;
  if (new_gov != 0) {
    civs[civ].flags = civs[civ].flags | CIVF_GOV_CHANGED;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    if ((old_government != new_gov) || ((civs[civ].flags & CIVF_REVOLUTION) != 0)) {
      text = civ_adjective(civ);
      text_set_string_arg_b(0,(const char *)text);
      text_set_label_arg(1,government_names[new_gov]);
      text = (iptr)(civ_leader_name(civ));
      text_set_string_arg(2,(const char *)text);
      text = civ_leader_title(civ);
      text_set_string_arg(3,(const char *)text);
      notified = 0;
      if (((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) != 0) ||
         (((has_wonder = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), has_wonder != 0 ||
           (has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder != 0)) || (game.reveal_map != '\0')))
         ) {
        if (((civs[civ].flags & CIVF_REVOLUTION) == 0) || (autoplay.active != 0)) {
          if (autoplay.active == 0) {
            popup_game_with_sprite((const char *)&str_overthrown_2,(iptr)(((i8 *)&government_sprites_block.government_sprites) + old_government * 0x3c),
                               (ui_metrics.large_screen == 0) - 1 & 8);
            civs[civ].flags = civs[civ].flags | CIVF_REVOLUTION;
          }
        }
        else {
          popup_game_with_sprite((const char *)&str_changed_2,(iptr)(((i8 *)&government_sprites_block.government_sprites) + new_gov * 0x3c),
                             (ui_metrics.large_screen == 0) - 1 & 8);
          civs[civ].flags = civs[civ].flags & ~CIVF_REVOLUTION;
        }
        if ((int)new_gov < 4) {
          gov_rank = new_gov;
        }
        else {
          gov_rank = new_gov - 1;
        }
        civs[civ].tax_rate = 4 - (char)((int)gov_rank >> 1);
      }
      if (2 < game.multiplayer) {
        for (civ2 = 1; civ2 < 8; civ2++) {
          if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) &&
              (human_civ[0] != civ2)) &&
             ((game.reveal_map != '\0' ||
              ((((*(byte *)&civs[civ2].treaties[civ] & TREATY_EMBASSY) != 0 ||
                (has_wonder = civ_has_wonder(civ2,WONDER_UNITED_NATIONS), has_wonder != 0)) ||
               (has_wonder = civ_has_wonder(civ2,WONDER_MARCO_POLO), has_wonder != 0)))))) {
            if (((civs[civ].flags & CIVF_REVOLUTION) == 0) || (autoplay.active != 0)) {
              net_send_text_args(0x2c,net.net_slots[net.net_civ_slots[civ2]].id,1,0,old_government,(ui_metrics.large_screen == 0) - 1 & 8);
              civs[civ].flags = civs[civ].flags | CIVF_REVOLUTION;
              notified++;
            }
            else {
              net_send_text_args(0x2b,net.net_slots[net.net_civ_slots[civ2]].id,4,0,new_gov,(ui_metrics.large_screen == 0) - 1 & 8);
              civs[civ].flags = civs[civ].flags & ~CIVF_REVOLUTION;
              notified++;
            }
          }
        }
      }
      if (notified != 0) {
        if ((int)new_gov < 4) {
          gov_rank = new_gov;
        }
        else {
          gov_rank = new_gov - 1;
        }
        civs[civ].tax_rate = 4 - (char)((int)gov_rank >> 1);
      }
    }
  }
  else if (new_gov != 0) {
    civ_select_government_dialog(civ,0);
    return;
  }
  civ_set_government(civ,new_gov);
  return;
}

/* 00560084 auto */
void __cdecl civ_turn_update_diplomacy(uptr civ)

{
  uint *treaty_ptr;
  u8 cur_gov;
  int value;
  iptr civ_plural;
  int roll;
  uptr civ4;
  uptr civ2;
  int civ3;
  
  cur_gov = civs[civ].government;
  civs[civ].flags = civs[civ].flags & ~(CIVF_GOV_CHANGED | CIVF_UNUSED_40);
  if (cur_gov == GOV_ANARCHY) {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) || ((game.turn & 3U) != 0))
    {
      if ((((game.turn & 3U) == 0) || (value = civ_has_wonder(civ,WONDER_STATUE_OF_LIBERTY), value != 0)) &&
         (civ_change_government(civ,1),
         (1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        civs[civ].flags = civs[civ].flags & ~CIVF_REVOLUTION;
      }
    }
    else if ((civs[civ].flags & CIVF_REVOLUTION) == 0) {
      civ_change_government(civ,1);
    }
    else {
      civs[civ].flags = civs[civ].flags & ~CIVF_REVOLUTION;
    }
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
     ((civs[civ].flags & CIVF_REVOLUTION) != 0)) {
    civ_change_government(civ,(uint)civs[civ].government);
  }
  if (civ != 0) {
    value = _rand();
    civs[civ].diplo_random = (u8)(value % 100);
    value = _rand();
    if (value % 3 == 0) {
      civs[civ].flags = civs[civ].flags ^ CIVF_SENATE_ACTIVE;
    }
    if (((int)game.turn % 3 == 0) && (civs[civ].diplo_request_count != '\0')) {
      civs[civ].diplo_request_count--;
    }
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (2 < game.multiplayer) {
        net.process_messages(1,0);
      }
      if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_CHANGED_THIS_TURN >> 8)) == 0) {
        setup_picwnd_block.diplo_recent_peace_table[civ2] = 0;
      }
      else {
        setup_picwnd_block.diplo_recent_peace_table[civ2] = 1;
      }
      treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
      *treaty_ptr = *treaty_ptr & 0xff5fbfff;
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0) && (civ2 != civ))
         && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
        if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_HOSTILE_INTENT) != 0) {
          value = int_clamp(3 - ((int)(uint)civs[civ2].reputation >> 2),1,3);
          if (value - 1 < 1) {
            roll = 0;
          }
          else {
            roll = _rand();
            roll = roll % value;
          }
          if (roll == 0) {
            if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
              treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
              *treaty_ptr = *treaty_ptr & 0xffffffd9;
              treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
              *treaty_ptr = *treaty_ptr | 0x80840;
              value = (int)civs[civ2].last_contact_turn[civ];
              if (value <= game.turn - 8) {
                value = game.turn - 8;
              }
              civs[civ2].last_contact_turn[civ] = (short)value;
            }
            else {
              civ_adjust_attitude(civ,civ2,100);
              civs[civ2].last_contact_turn[civ] = 0xffff;
              treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
              *treaty_ptr = *treaty_ptr & 0xffffffdf;
            }
          }
        }
        if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0) {
          treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
          *treaty_ptr = *treaty_ptr & 0xfffbffff;
          if ((game.turn & 0x1fU) == 0) {
            if (((byte)(civs[civ].treaties[civ2] >> 16) & (TREATY_AGGRESSOR_LONG >> 16)) == 0) {
              treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
              *treaty_ptr = *treaty_ptr & 0xfffff7ff;
            }
            treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
            *treaty_ptr = *treaty_ptr & 0xfff7ffff;
          }
          if ((game.turn & 0xfU) == 0) {
            if (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_RECENT_CONTACT >> 8)) == 0) {
              if ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) == 0) {
                if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_PEACE) == 0) ||
                   ((game.game_flags & GAME_AI_GANG_UP) == 0)) {
                  if (((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0) &&
                     (((((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_WAR >> 8)) != 0 &&
                       (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)) &&
                      (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)))) {
                    if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
                         ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) &&
                        (((*(byte *)&civs[human_civ[0]].treaties[civ] & TREATY_EMBASSY) != 0 ||
                         (((value = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), value != 0 ||
                           (value = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), value != 0)) ||
                          (((*(byte *)&civs[human_civ[0]].treaties[civ2] & TREATY_EMBASSY) != 0
                           || (((value = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS), value != 0 ||
                                (value = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), value != 0)) ||
                               (game.reveal_map != '\0')))))))))) && (autoplay.active == 0)) {
                      civ_plural = (iptr)(civ_name_plural(civ));
                      text_set_string_arg(0,(const char *)civ_plural);
                      civ_plural = (iptr)(civ_name_plural(civ2));
                      text_set_string_arg(1,(const char *)civ_plural);
                      popup_game_with_sprite((const char *)&str_warends_2,(iptr)&ceasefire_icon_sprite,0);
                    }
                    if (2 < game.multiplayer) {
                      for (civ3 = 1; civ3 < 8; civ3++) {
                        if ((((1 << ((byte)civ3 & 0x1f) & (uint)game.human_players) != 0) &&
                            (human_civ[0] != civ3)) &&
                           (((game.reveal_map != '\0' ||
                             (((*(byte *)&civs[civ3].treaties[civ] & TREATY_EMBASSY) != 0 ||
                              ((*(byte *)&civs[civ3].treaties[civ2] & TREATY_EMBASSY) != 0)))
                             ) || ((value = civ_has_wonder(civ3,WONDER_UNITED_NATIONS), value != 0 ||
                                   (value = civ_has_wonder(civ3,WONDER_MARCO_POLO), value != 0)))))) {
                          civ_plural = (iptr)(civ_name_plural(civ));
                          text_set_string_arg(0,(const char *)civ_plural);
                          civ_plural = (iptr)(civ_name_plural(civ2));
                          text_set_string_arg(1,(const char *)civ_plural);
                          net_send_text_args(0x50,net.net_slots[net.net_civ_slots[civ3]].id,2,0
                                             ,0,0);
                        }
                      }
                    }
                  }
                }
                else {
                  diplo_civs_meet(civ,civ2,0,0);
                }
              }
              else {
                diplo_ai_swap_advances(civ,civ2);
              }
              if (((((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0) &&
                   (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)) &&
                  (((byte)(civs[civ2].treaties[civ] >> 8) & (TREATY_AGGRESSOR >> 8)) == 0)) &&
                 (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
                  ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)))) {
                diplo_clear_treaty_flags(civ,civ2,1);
              }
            }
            treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
            *treaty_ptr = *treaty_ptr & 0xfffffbff;
          }
          else if ((((game.turn & 7U) == 0) &&
                   ((*(byte *)&civs[civ].treaties[civ2] & TREATY_ALLIANCE) != 0)) &&
                  (((byte)(civs[civ].treaties[civ2] >> 8) & (TREATY_RECENT_CONTACT >> 8)) == 0)) {
            diplo_ai_swap_advances(civ,civ2);
          }
        }
      }
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      for (civ2 = 1; (int)civ2 < 8; civ2++) {
        if (2 < game.multiplayer) {
          net.process_messages(1,0);
        }
        if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) && (autoplay.active == 0)
            ) && ((*(byte *)&civs[civ].treaties[civ2] & TREATY_CONTACT) != 0)) {
          if (human_civ[0] == civ2) {
            diplo_ai_review_relations(civ,civ2);
          }
          else if (2 < game.multiplayer) {
            net_send_message(NM_POLITICAL_VIOLATIONS,net.net_slots[net.net_civ_slots[civ2]].id
                               ,civ,civ2,0,0,0,0,0,0);
          }
        }
      }
      diplo_ai_military_aid(civ);
      for (civ4 = 1; (int)civ4 < 8; civ4++) {
        if (2 < game.multiplayer) {
          net.process_messages(1,0);
        }
        if ((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) {
          if (human_civ[0] == civ4) {
            diplo_ai_seek_war_ally(civ,civ4);
          }
          else if (2 < game.multiplayer) {
            net_send_message(NM_POLITICAL_ATTITUDE,net.net_slots[net.net_civ_slots[civ4]].id
                               ,civ,civ4,0,0,0,0,0,0);
          }
        }
      }
    }
  }
  return;
}

/* 00568861 auto */
i32 __cdecl civ_city_art_era(iptr civ)

{
  int known;
  
  known = civ_has_advance(civ,ADV_AUTOMOBILE);
  if ((known != 0) && (known = civ_has_advance(civ,ADV_ELECTRONICS), known != 0)) {
    return 2;
  }
  known = civ_has_advance(civ,ADV_PHILOSOPHY);
  if ((known != 0) && (known = civ_has_advance(civ,ADV_INVENTION), known != 0)) {
    return 1;
  }
  return 0;
}

/* 0056e1f0 auto */
i32 __cdecl civ_color_mid(iptr civ)

{
  int color_index;
  
  if (civ == 0) {
    color_index = 0;
  }
  else {
    color_index = (int)leaders[civs[civ].tribe].color;
  }
  return civ_primary_colors_block.civ_colors[color_index].mid;
}

/* 0057a27a auto */
void __cdecl civ_steal_advance(iptr civ,iptr civ2)

{
  int known;
  iptr text;
  bool interactive;
  i32 option_flags;
  iptr option_id;
  iptr best_advance;
  iptr advance;
  Dialog dialog2;
  int best_value;
  int value;
  
  best_advance = -1;
  best_value = -999;
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (!((civ == 0) || (civ2 == 0))) {
    interactive = human_civ[0] == civ;
    if (autoplay.active != 0) {
      interactive = false;
    }
    while( true ) {
      if (interactive) {
        dialog2.load_game_text((const char *)&str_takeciv_2);
      }
      for (advance = 0; advance < 100; advance++) {
        known = civ_has_advance(civ,advance);
        if ((((known == 0) && (known = civ_has_advance(civ2,advance), known != 0)) &&
            ((((game.game_flags & GAME_SCENARIO) == 0 || ((scenario.flags[0] & 0x10) == 0)) ||
             ((((advance != ADV_FUNDAMENTALISM && (advance != ADV_MONARCHY)) && (advance != ADV_COMMUNISM)) &&
              ((advance != ADV_REPUBLIC && (advance != ADV_DEMOCRACY)))))))) &&
           ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))) {
          if (interactive) {
            option_flags = 0;
            option_id = advance;
            text = (iptr)label_string(advances[advance].name);
            dialog2.add_option((const char *)text,option_id,option_flags);
          }
          value = ai_advance_value(civ,advance);
          if (best_value <= value) {
            best_advance = advance;
            best_value = value;
          }
        }
      }
      if (best_advance < 0) {
        return;
      }
      advance = best_advance;
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
         (advance = dialog2.show_modal(0), dialog2.result_extra == 0))
      break;
      civ_research_goal_dialog(civ,0,civ2);
    }
    if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
      text = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)text);
      text_set_label_arg(1,advances[advance].name);
      if ((autoplay.active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
        if (human_civ[0] == civ2) {
          popup_game_with_advance((const char *)&str_tookciv_2,advance,0);
        }
        else if (2 < game.multiplayer) {
          net_send_text_args(99,net.net_slots[net.net_civ_slots[civ2]].id,2,0,
                             advance,0);
        }
      }
    }
    civ_give_advance(civ,advance,civ2,0,0);
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      civ_reset_city_production(civ,-1);
    }
  }
  return;
}

/* 0057a685 auto */
iptr __cdecl civ_find_central_city(iptr civ)

{
  int dist;
  iptr best_city;
  iptr city;
  int best_total;
  iptr city2;
  int total_dist;
  
  best_total = 0x7fff;
  best_city = -1;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      total_dist = 0;
      for (city2 = 0; city2 < game.num_cities; city2++) {
        if ((cities[city2].id != 0) && (cities[city2].owner == civ)) {
          dist = map_distance_diag((int)cities[city].x,(int)cities[city].y,
                                     (int)cities[city2].x,(int)cities[city2].y);
          total_dist += dist;
        }
      }
      if (total_dist < best_total) {
        best_total = total_dist;
        best_city = city;
      }
    }
  }
  return best_city;
}

/* 0057a904 auto */
i32 __cdecl civ_civil_war(iptr civ)

{
  char civ4;
  iptr allowed;
  i32 result;
  iptr has_palace;
  int city2;
  Tile *map_tile;
  iptr adjective;
  int x3;
  uint y2;
  iptr capital_landmass;
  iptr unit;
  iptr city4;
  iptr civ3;
  iptr city3;
  uptr city;
  int landmass_sizes[64];
  iptr landmass2;
  byte rebel_bit;
  uint x2;
  byte civ_bit;
  iptr saved_unit;
  Tile *tiles;
  iptr civ2;
  int rebel_size;
  int dist;
  iptr advance;
  int loyal_size;
  int total_size;
  int max_dist;
  int y3;
  
  allowed = ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_no_schism(civ);
  if (allowed == 0) {
    result = 0;
  }
  else {
    civ2 = -1;
    for (civ3 = 1; civ3 < 8; civ3++) {
      if ((civs[civ3].num_military_units == 0) && (civs[civ3].num_cities == 0)) {
        civ2 = civ3;
      }
    }
    if (civ2 < 1) {
      result = 0;
    }
    else {
      for (landmass2 = 0; landmass2 < 0x40; landmass2++) {
        landmass_sizes[landmass2] = 0;
      }
      capital_landmass = -1;
      for (city = 0; (int)city < (int)game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city].owner == civ)) {
          landmass2 = tile_landmass((int)cities[city].x,(int)cities[city].y);
          landmass_sizes[landmass2] += (int)cities[city].size;
          has_palace = city_has_improvement(city,IMP_PALACE);
          if ((has_palace != 0) && ((capital_landmass < 0 || (civs[civ].capital_x == cities[city].x))))
          {
            capital_landmass = landmass2;
            x3 = (int)cities[city].x;
            y3 = (int)cities[city].y;
            city3 = city;
          }
        }
      }
      if (capital_landmass < 0) {
        result = 0;
      }
      else {
        game.civs_alive = game.civs_alive & ~(byte)(1 << ((byte)civ2 & 0x1f));
        city2 = civ_create(civ2);
        if (city2 == 0) {
          result = 0;
        }
        else {
          game.civs_alive = game.civs_alive | (byte)(1 << ((byte)civ2 & 0x1f));
          adjective = civ_get_adjective(civ);
          text_set_string_arg(0,(const char *)adjective);
          adjective = civ_get_adjective(civ2);
          text_set_string_arg(1,(const char *)adjective);
          popup_game((const char *)&str_schism_3);
          if (2 < game.multiplayer) {
            net_send_text_args(0x16,0xff,2,0,0,0);
          }
          civs[civ2].gold = civs[civ].gold / 2;
          civs[civ].gold -= civs[civ2].gold;
          civs[civ2].military_strength = (u16)((int)(uint)civs[civ].military_strength >> 1);
          civs[civ2].government = civs[civ].government;
          civs[civ2].research_progress = civs[civ].research_progress;
          civs[civ2].num_advances = civs[civ].num_advances;
          for (advance = 0; advance < 0xd; advance++) {
            *(i8 *)&civs[civ2].advances[advance] =
                 *(i8 *)&civs[civ].advances[advance];
          }
          for (civ3 = 1; civ3 < 8; civ3++) {
            civs[civ3].last_contact_turn[civ2] = game.turn - 8;
          }
          *(i32 *)&civs[civ2].treaties[civ] = 0x2001;
          *(i32 *)&civs[civ].treaties[civ2] = 0x82801;
          tiles = map_data.tiles;
          civ_bit = (byte)(1 << ((byte)civ & 0x1f));
          rebel_bit = (byte)(1 << ((byte)civ2 & 0x1f));
          y2 = 0;
          x2 = 0;
          net_batch_begin();
          for (advance = 0; advance < map.area; advance++) {
            map_tile = tile_at(x2,y2);
            if ((civ_bit & map_tile->visible) != 0) {
              tile_set_visible(x2,y2,(uint)rebel_bit,1,1);
            }
            x2 += 2;
            if ((int)map.width <= (int)x2) {
              y2++;
              x2 = y2 & 1;
            }
          }
          net_batch_flush();
          loyal_size = landmass_sizes[capital_landmass];
          rebel_size = 0;
          for (landmass2 = 0; landmass2 < 0x40; landmass2++) {
            if (landmass_sizes[landmass2] != 0) {
              if ((loyal_size < rebel_size) || (capital_landmass == landmass2)) {
                if (capital_landmass != landmass2) {
                  loyal_size += landmass_sizes[landmass2];
                }
                landmass_sizes[landmass2] = 1;
              }
              else {
                rebel_size += landmass_sizes[landmass2];
                landmass_sizes[landmass2] = 2;
              }
            }
          }
          if ((loyal_size < rebel_size * 2) && (rebel_size <= loyal_size)) {
            for (city = 0; (int)city < (int)game.num_cities; city++) {
              if ((cities[city].id != 0) && (cities[city].owner == civ)) {
                landmass2 = tile_landmass((int)cities[city].x,(int)cities[city].y);
                if (landmass_sizes[landmass2] == 2) {
                  city_reveal_to_civ(city,civ);
                  city_transfer_for_civil_war(city,civ,civ2);
                }
                else {
                  city_reveal_to_civ(city,civ2);
                }
              }
            }
          }
          else {
            rebel_size = 0;
            total_size = (int)civs[civ].total_city_size;
            while (rebel_size * 3 < total_size) {
              max_dist = 1;
              city4 = -1;
              for (city = 0; (int)city < (int)game.num_cities; city++)
              {
                if ((cities[city].id != 0) && (cities[city].owner == civ)) {
                  city_reveal_to_civ(city,civ2);
                  city_reveal_to_civ(city,civ);
                  dist = map_distance_diag(x3,y3,(int)cities[city].x,
                                                (int)cities[city].y);
                  if (max_dist < dist) {
                    city4 = city;
                    max_dist = dist;
                  }
                }
              }
              if (city4 < 0) break;
              rebel_size += cities[city4].size;
              city_transfer_for_civil_war(city4,civ,civ2);
            }
          }
          for (unit = 0; unit < game.num_units; unit++) {
            if ((units[unit].id != 0) && (units[unit].owner == civ)) {
              city = city_at((int)units[unit].x,(int)units[unit].y);
              if (((int)city < 0) && (units[unit].home_city != -1)) {
                city = (uptr)(byte)units[unit].home_city;
              }
              if (-1 < (int)city) {
                civ4 = cities[city].owner;
                city2 = (int)civ4;
                if ((city2 == civ) || (city2 == civ2)) {
                  tile_set_owner((int)units[unit].x,(int)units[unit].y,city2,1);
                  saved_unit = unit;
                  for (unit = unit_stack_first(unit); -1 < unit;
                      unit = unit_next_in_stack(unit)) {
                    units[unit].owner = civ4;
                    if ((units[unit].home_city != -1) &&
                       (cities[(byte)units[unit].home_city].owner != city2)) {
                      units[unit].home_city = (i8)city;
                    }
                  }
                  unit = saved_unit;
                }
              }
            }
          }
          city2 = civ_find_central_city(civ2);
          if (-1 < city2) {
            city_set_improvement(city2,1,1);
            civs[civ2].capital_x = cities[city2].x;
          }
          cities[city3].owner = -1;
          city2 = civ_find_central_city(civ);
          if (-1 < city2) {
            city_set_improvement(city2,1,1);
            civs[civ].capital_x = cities[city2].x;
          }
          cities[city3].owner = (byte)civ;
          if (2 < game.multiplayer) {
            net_send_state_diff(0xff,2,0,0,0);
            net_send_message(NM_DRAW_MAP,0xff,0,0,0,0,0,0,0,0);
          }
          map_redraw_all_views(human_civ[0],1);
          result = 1;
        }
      }
    }
  }
  return result;
}

/* 005866a0 auto */
void __cdecl cosmic_editor_focus_redraw(void)

{
  HWND hWnd;
  
  hWnd = ((Control *)&editor_active.wnd->list_box)->get_hwnd();
  SetFocus(hWnd);
  editor_active.wnd->cosmic_editor_draw_bg();
  return;
}

/* 005866d3 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl cosmic_editor_capture_defaults(void)

{
  (*(i32 *)&cosmic_edit.values) = (uint)cosmic.road_move_mult;
  (*(i32 *)&cosmic_edit.trireme_loss) = (uint)cosmic.trireme_loss_chance;
  (*(i32 *)&cosmic_edit.food_per_citizen) = (uint)cosmic.food_per_citizen;
  (*(i32 *)&cosmic_edit.food_box_rows) = (uint)cosmic.food_box_rows;
  (*(i32 *)&cosmic_edit.shield_box_rows) = (uint)cosmic.shield_box_rows;
  (*(i32 *)&cosmic_edit.settler_food_low) = (uint)cosmic.settler_food_low;
  (*(i32 *)&cosmic_edit.settler_food_high) = (uint)cosmic.settler_food_high;
  (*(i32 *)&cosmic_edit.unhappy_size) = (uint)cosmic.unhappy_size;
  (*(i32 *)&cosmic_edit.riot_factor) = (uint)cosmic.riot_factor;
  (*(i32 *)&cosmic_edit.aqueduct_size) = (uint)cosmic.aqueduct_size;
  (*(i32 *)&cosmic_edit.sewer_size) = (uint)cosmic.sewer_size;
  (*(i32 *)&cosmic_edit.tech_paradigm) = (uint)cosmic.tech_paradigm;
  (*(i32 *)&cosmic_edit.transform_time) = (uint)cosmic.transform_time;
  (*(i32 *)&cosmic_edit.monarchy_support) = (uint)cosmic.monarchy_free_support;
  (*(i32 *)&cosmic_edit.communism_support) = (uint)cosmic.communism_free_support;
  (*(i32 *)&cosmic_edit.fundamentalism_support) = (uint)cosmic.fundamentalism_free_support;
  (*(i32 *)&cosmic_edit.communism_palace_dist) = (uint)cosmic.communism_palace_dist;
  (*(i32 *)&cosmic_edit.fund_science_loss) = (uint)cosmic.fund_science_loss;
  (*(i32 *)&cosmic_edit.change_penalty) = (uint)cosmic.change_penalty;
  (*(i32 *)&cosmic_edit.paradrop_range) = (uint)cosmic.paradrop_range;
  (*(i32 *)&cosmic_edit.mass_thrust_paradigm) = (uint)cosmic.mass_thrust_paradigm;
  (*(i32 *)&cosmic_edit.fund_max_science) = (uint)cosmic.fund_max_science;
  rules_load_cosmic();
  editor_text_dlg_wnd_block.editor_items[0] = (uint)cosmic.road_move_mult;
  editor_text_dlg_wnd_block.editor_item_field1[0] = (uint)cosmic.trireme_loss_chance;
  editor_text_dlg_wnd_block.editor_item_field2[0] = (uint)cosmic.food_per_citizen;
  editor_text_dlg_wnd_block.editor_item_field3[0] = (uint)cosmic.food_box_rows;
  editor_text_dlg_wnd_block.editor_item_field4[0] = (uint)cosmic.shield_box_rows;
  editor_text_dlg_wnd_block.editor_item_field5[0] = (uint)cosmic.settler_food_low;
  editor_text_dlg_wnd_block.editor_item_field6[0] = (uint)cosmic.settler_food_high;
  editor_text_dlg_wnd_block.editor_item_field7[0] = (uint)cosmic.unhappy_size;
  editor_text_dlg_wnd_block.editor_item_field8[0] = (uint)cosmic.riot_factor;
  editor_text_dlg_wnd_block.editor_item_field9[0] = (uint)cosmic.aqueduct_size;
  editor_text_dlg_wnd_block.editor_item_field10[0] = (uint)cosmic.sewer_size;
  editor_text_dlg_wnd_block.editor_item_field11[0] = (uint)cosmic.tech_paradigm;
  (*(i32 *)&editor_text_dlg_wnd_block.editor_item_flags) = (uint)cosmic.transform_time;
  editor_text_dlg_wnd_block.editor_item_field13[0] = (uint)cosmic.monarchy_free_support;
  editor_text_dlg_wnd_block.editor_item_field14_i32 = (uint)cosmic.communism_free_support;
  editor_text_dlg_wnd_block.editor_item_field15_i32 = (uint)cosmic.fundamentalism_free_support;
  editor_text_dlg_wnd_block.editor_item_field16_i32 = (uint)cosmic.communism_palace_dist;
  editor_text_dlg_wnd_block.editor_item_field17_i32 = (uint)cosmic.fund_science_loss;
  editor_text_dlg_wnd_block.editor_item_field18_i32 = (uint)cosmic.change_penalty;
  editor_text_dlg_wnd_block.editor_item_field19_i32 = (uint)cosmic.paradrop_range;
  editor_text_dlg_wnd_block.editor_item_field20_i32 = (uint)cosmic.mass_thrust_paradigm;
  editor_text_dlg_wnd_block.editor_item_field21_i32 = (uint)cosmic.fund_max_science;
  cosmic.road_move_mult = cosmic_edit.values[0];
  cosmic.trireme_loss_chance = cosmic_edit.trireme_loss;
  cosmic.food_per_citizen = cosmic_edit.food_per_citizen;
  cosmic.food_box_rows = cosmic_edit.food_box_rows;
  cosmic.shield_box_rows = cosmic_edit.shield_box_rows;
  cosmic.settler_food_low = cosmic_edit.settler_food_low;
  cosmic.settler_food_high = cosmic_edit.settler_food_high;
  cosmic.unhappy_size = cosmic_edit.unhappy_size;
  cosmic.riot_factor = cosmic_edit.riot_factor;
  cosmic.aqueduct_size = cosmic_edit.aqueduct_size;
  cosmic.sewer_size = cosmic_edit.sewer_size;
  cosmic.tech_paradigm = cosmic_edit.tech_paradigm;
  cosmic.transform_time = cosmic_edit.transform_time;
  cosmic.monarchy_free_support = cosmic_edit.monarchy_support;
  cosmic.communism_free_support = cosmic_edit.communism_support;
  cosmic.fundamentalism_free_support = cosmic_edit.fundamentalism_support;
  cosmic.communism_palace_dist = cosmic_edit.communism_palace_dist;
  cosmic.fund_science_loss = cosmic_edit.fund_science_loss;
  cosmic.change_penalty = cosmic_edit.change_penalty;
  cosmic.paradrop_range = cosmic_edit.paradrop_range;
  cosmic.mass_thrust_paradigm = cosmic_edit.mass_thrust_paradigm;
  cosmic.fund_max_science = cosmic_edit.fund_max_science;
  return;
}

/* 005869d4 auto */
void __cdecl cosmic_editor_fill_list(void)

{
  char *_Str;
  size_t len;
  int index;
  char line[128];
  char suffix[32];
  
  index = 0;
  editor_active.wnd->list_box.clear();
  text_open_section((const char *)&str_debug_11,(const char *)&str_editcosmic);
  while( true ) {
    _Str = text_read_line_or_end(1);
    len = _strlen(_Str);
    if ((len < 3) || (0x15 < index)) break;
    if (editor_text_dlg_wnd_block.editor_item_recs[1].values[index] < 10) {
      _sprintf(line,str_d,editor_text_dlg_wnd_block.editor_item_recs[1].values[index]);
    }
    else if (editor_text_dlg_wnd_block.editor_item_recs[1].values[index] < 100) {
      _sprintf(line,str_d_2,editor_text_dlg_wnd_block.editor_item_recs[1].values[index]);
    }
    else {
      _sprintf(line,((i8 *)&cosmic_value_fmt_3digit),editor_text_dlg_wnd_block.editor_item_recs[1].values[index]);
    }
    if (editor_text_dlg_wnd_block.editor_item_recs[0].values[index] < 10) {
      _sprintf(suffix,str_d_3,editor_text_dlg_wnd_block.editor_item_recs[0].values[index]);
    }
    else if (editor_text_dlg_wnd_block.editor_item_recs[0].values[index] < 100) {
      _sprintf(suffix,str_d_4,editor_text_dlg_wnd_block.editor_item_recs[0].values[index]);
    }
    else {
      _sprintf(suffix,str_d_5,editor_text_dlg_wnd_block.editor_item_recs[0].values[index]);
    }
    crt_strcat(line,suffix);
    crt_strcat(line,((i8 *)&textbuf.buf));
    editor_active.wnd->list_box.add_string((const char *)line);
    index++;
  }
  text_close_file();
  return;
}

/* 00586bb6 auto */
void __cdecl cosmic_editor_edit_value(void)

{
  int sel;
  char *default_text;
  int rc;
  i32 max_value;
  i32 min_value;
  char *out_buf;
  iptr parent_wnd2;
  char input[260];
  char num_buf[16];
  
  sel = editor_active.wnd->list_box.get_cur_sel();
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  text_set_number_arg(0,*(i32 *)(((i8 *)&cosmic_edit_min_values) + sel * 4));
  text_set_number_arg(1,*(i32 *)(((i8 *)&cosmic_edit_max_values) + sel * 4));
  out_buf = input;
  default_text = __itoa(editor_text_dlg_wnd_block.editor_item_recs[0].values[sel],num_buf,10);
  rc = dlg_text_input((iptr)&str_debug_11,(const char *)&str_cpedit,3,(iptr)default_text,(iptr)out_buf);
  if (-1 < rc) {
    max_value = *(i32 *)(((i8 *)&cosmic_edit_max_values) + sel * 4);
    min_value = *(i32 *)(((i8 *)&cosmic_edit_min_values) + sel * 4);
    rc = _atoi(input);
    max_value = int_clamp(rc,min_value,max_value);
    editor_text_dlg_wnd_block.editor_item_recs[0].values[sel] = max_value;
    cosmic_editor_fill_list();
    editor_active.wnd->list_box.set_cur_sel(sel);
  }
  ((Control *)&editor_active.wnd->list_box)->set_focus();
  dlg_set_parent_window(0);
  cosmic_editor_focus_redraw();
  return;
}

/* 00586d0a auto */
i32 __cdecl cosmic_editor_write_section(FILE *out_file,FILE *in_file)

{
  char *comment;
  int i;
  char line[128];
  
  for (i = 0; i < 0x16; i++) {
    /* BF-029 (Peter Johnson's ledger): a section cut short leaves no line
       (the original used the buffer unset) and a line without a comment no
       comment (the original printed a null pointer); the value goes on its
       own line */
    if (_fgets(line,0x80,in_file) == (char *)0x0) {
      line[0] = '\0';
    }
    comment = _strchr(line,0x3b);
    if (comment == (char *)0x0) {
      comment = (char *)"\n";
    }
    _sprintf(((i8 *)&textbuf.buf),str_8d_s,editor_text_dlg_wnd_block.editor_item_recs[0].values[i],comment);
    _fputs(((i8 *)&textbuf.buf),out_file);
  }
  return 1;
}

/* 00586da1 auto */
void __cdecl cosmic_editor_ok_clicked(void)

{
  iptr ok;
  NativeWindow *wnd;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  char message[32];
  
  rules_edit_set_section((iptr)&str_cosmic_2,cosmic_editor_write_section);
  ok = rules_write_sections();
  if (ok == 0) {
    _sprintf(message,str_error_updating_rules_s_5,((i8 *)&lang_file_ext));
    uType = 0x10;
    lpCaption = str_file_i_o_error_6;
    lpText = message;
    wnd = ((WndBase *)&editor_active.wnd->window)->get_window();
    MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
  }
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 00586e24 auto */
void __cdecl cosmic_editor_help_clicked(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_18,(const char *)&str_effects);
  dlg_set_parent_window(0);
  cosmic_editor_focus_redraw();
  return;
}

/* 00586e88 auto */
void __cdecl cosmic_editor_cancel_clicked(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 00586eb0 auto */
void EditorWindow::cosmic_editor_draw_bg()

{
  this->FrameWindow::draw_caption();
  gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1a);
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  wnd_blit_surface();
  return;
}

/* 00586f16 auto */
void EditorWindow::cosmic_editor_dialog()

{
  DibSurface *this_00;
  uptr font_height;
  i32 title;
  int x_pos;
  int y_pos;
  iptr label;
  i32 style;
  i32 pos_x;
  i32 pos_y;
  u32 width;
  u32 height;
  i32 reserved_1;
  i32 reserved_2;
  i32 reserved_3;
  u8 *cancel_parent;
  u8 *help_parent;
  u8 *edit_parent;
  u8 *ok_parent;
  u8 *list_parent;
  DibSurface *surface;
  i8 rect[16];
  GamePalette palette;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)operator_new(0x48);
  if (this_00 == (DibSurface *)0x0) {
    surface = (DibSurface *)0x0;
  }
  else {
    surface = surface_ctor(this_00);
  }
  editor_ui.bg_surface = surface;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  wnd_width_u = 0x230;
  wnd_height_u = 0x1c6;
  item_uptr = 0;
  editor_lists.next_ctrl_id = 0xc9;
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  reserved_3 = 0;
  reserved_2 = 0;
  reserved_1 = 0;
  height = wnd_height_u;
  width = wnd_width_u;
  pos_y = 0;
  pos_x = 0;
  style = 0xd;
  title = (iptr)label_string(label_table.string_ids[0x253]);
  this->FrameWindow::create((const char *)title,style,pos_x,pos_y,width,height,reserved_1,reserved_2,reserved_3);
  rect_set_xywh((iptr)rect,client_x + 2,client_y + 2,client_width - 5,
                     client_height - 0x14);
  x_pos = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    list_parent = (u8 *)0x0;
  }
  else {
    list_parent = (u8 *)&window;
  }
  list_box.create((iptr)list_parent,x_pos,(iptr)rect);
  list_box.set_font((iptr)&editor_font_block.editor_font);
  list_box.set_dblclick_callback((CtrlNotifyFn)cosmic_editor_edit_value);
  cosmic_editor_capture_defaults();
  cosmic_editor_fill_list();
  list_box.set_cur_sel(0);
  x_pos = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = x_pos + 8;
  x_pos = client_width - 10;
  button_width_u = (int)(x_pos + (x_pos >> 0x1f & 3U)) >> 2;
  y_pos = (client_y + client_height) - (button_height_u + 2);
  x_pos = client_x + 2;
  rect_set_xywh((iptr)rect,x_pos,y_pos,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((iptr)ok_parent,0x65,(iptr)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)cosmic_editor_ok_clicked);
  x_pos = x_pos + button_width_u + 2;
  rect_set_xywh((iptr)rect,x_pos,y_pos,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    edit_parent = (u8 *)0x0;
  }
  else {
    edit_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x238]);
  rename_button.create((iptr)edit_parent,0x66,(iptr)rect,(const char *)label);
  rename_button.set_callback((CtrlNotifyFn)cosmic_editor_edit_value);
  rename_button.set_default_button();
  x_pos = x_pos + button_width_u + 2;
  rect_set_xywh((iptr)rect,x_pos,y_pos,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((iptr)help_parent,0x66,(iptr)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)cosmic_editor_help_clicked);
  rect_set_xywh((iptr)rect,x_pos + button_width_u + 2,y_pos,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((iptr)cancel_parent,0x66,(iptr)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)cosmic_editor_cancel_clicked);
  cancel_button.set_cancel_button();
  window.set_keyboard_nav(0);
  set_redraw_callback(cosmic_editor_focus_redraw);
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
  return;
}

/* 0058760d auto */
void __cdecl cosmic_editor_run(void)

{
  EditorWindow editor_wnd;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.cosmic_editor_dialog();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 00598a05 auto */
i32 __cdecl civ_spaceship_part_maxed(iptr civ,iptr improvement)

{
  if (improvement == IMP_SS_STRUCTURAL) {
    if (spaceship_part_table_block.spaceship_part_max_counts[0] <= civs[civ].spaceship_parts[0]) {
      return 1;
    }
  }
  else if (improvement == IMP_SS_COMPONENT) {
    if ((spaceship_part_table_block.spaceship_propulsion_max <= civs[civ].spaceship_parts[1]) &&
       (spaceship_part_table_block.spaceship_fuel_max <= civs[civ].spaceship_parts[2])) {
      return 1;
    }
  }
  else if (((spaceship_part_table_block.spaceship_habitation_max <= civs[civ].spaceship_parts[3]) &&
           (spaceship_part_table_block.spaceship_life_support_max <= civs[civ].spaceship_parts[4])) &&
          (spaceship_part_table_block.spaceship_solar_panel_max <= civs[civ].spaceship_parts[5])) {
    return 1;
  }
  return 0;
}
