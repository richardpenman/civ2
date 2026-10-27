/* The game, civilizations: advances, research, government, the rules: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"
#include "lang.h"
#include "rules.h"

/* 0040bd10 auto */
i32 __cdecl civ_max_tax_rate(iptr civ)

{
  i32 max_rate;
  
  if (civs[civ].government < 2) {
    max_rate = 6;
  }
  else if (civs[civ].government == 2) {
    max_rate = 7;
  }
  else if (civs[civ].government < 6) {
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
    _strncpy(_Dest,_Source,_Count);
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
      ctrl_id = ctrl_get_id
                        ((Control *)
                         &editor_text_dlg_wnd_block.active_editor_wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot]);
      _sprintf(num_text,((i8 *)&tech_editor_ctrl_kinds_block.tech_editor_int_fmt),
               *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_text_dlg_wnd_block.active_editor_wnd->item * 0x58));
      ctrl_set_text
                ((Control *)
                 &editor_text_dlg_wnd_block.active_editor_wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot],
                 (const char *)num_text);
    }
    else if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 0xc) {
      ctrl_id = ctrl_get_id
                        ((Control *)
                         &editor_text_dlg_wnd_block.active_editor_wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot]);
      sel = *(iptr *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_text_dlg_wnd_block.active_editor_wnd->item * 0x58);
      if (row < 3) {
        sel += 2;
      }
      combo_set_cur_sel
                (
                 &editor_text_dlg_wnd_block.active_editor_wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot],sel);
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
      ctrl_get_text
                ((Control *)
                 &editor_text_dlg_wnd_block.active_editor_wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot],
                 (char *)num_text);
      value = _atoi(num_text);
      ctrl_id = ctrl_get_id
                        ((Control *)
                         &editor_text_dlg_wnd_block.active_editor_wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot]);
      ctrl_id -= 0xca;
      clamped = int_clamp(value,*(i32 *)(((i8 *)&tech_editor_ctrl_kinds_block.tech_editor_field_min_table) + ctrl_id * 4),
                                 *(i32 *)(((i8 *)&tech_editor_ctrl_kinds_block.tech_editor_field_max_table) + ctrl_id * 4));
      editor_text_dlg_wnd_block.editor_item_recs[editor_text_dlg_wnd_block.active_editor_wnd->item].values[ctrl_id] = clamped;
      ctrl_id = ctrl_get_id
                        ((Control *)
                         &editor_text_dlg_wnd_block.active_editor_wnd->edits[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot]);
      if (*(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_text_dlg_wnd_block.active_editor_wnd->item * 0x58) != value) {
        changed_count++;
      }
    }
    else if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 0xc) {
      value = combo_get_cur_sel
                          (
                           &editor_text_dlg_wnd_block.active_editor_wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot]);
      if (row < 3) {
        value -= 2;
      }
      ctrl_id = ctrl_get_id
                        ((Control *)
                         &editor_text_dlg_wnd_block.active_editor_wnd->combos[tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot]);
      *(int *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_text_dlg_wnd_block.active_editor_wnd->item * 0x58 + ctrl_id * 4) = value;
    }
  }
  return changed_count;
}

/* 00415765 auto */
void __cdecl tech_editor_redraw(void)

{
  tech_editor_paint(editor_text_dlg_wnd_block.active_editor_wnd);
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
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_name_sep_str));
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
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_ai_sep_str));
    text_append_int_2d((int)advances[advance].civilized_mod);
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_mod_sep_str));
    text_append_advance_abbrev((int)advances[advance].preq[0],1);
    text_append_space();
    text_append_advance_abbrev((int)advances[advance].preq[1],1);
    text_append_space();
    text_append_int((int)(char)advances[advance].epoch);
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&tech_editor_ctrl_kinds_block.civilize_rules_epoch_sep_str));
    text_append_int((int)(char)advances[advance].category);
    crt_strcat(((i8 *)&map_views_block.text_buffer),str_empty_10);
    text_append_advance_abbrev(advance,0);
    crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&civilize_rules_newline_str));
    _fputs(((i8 *)&map_views_block.text_buffer),file);
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
  char *lpCaption;
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
      window = wnd_get_window((WndBase *)&editor_text_dlg_wnd_block.active_editor_wnd->window);
      MessageBoxA(window->hwnd,lpText,lpCaption,uType);
    }
    editor_text_dlg_wnd_block.editor_running = 0;
    wnd_end_modal_loop(&editor_text_dlg_wnd_block.active_editor_wnd->window);
    menu_update_states();
  }
  else {
    tech_editor_update_controls();
    tech_editor_redraw();
    if (editor_text_dlg_wnd_block.active_editor_wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_text_dlg_wnd_block.active_editor_wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice);
    dlg_set_parent_window(0);
    hWnd = control_get_hwnd((Control *)&editor_text_dlg_wnd_block.active_editor_wnd->combos[0]);
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
  char line[80];
  
  advances_header = false;
  units_header = false;
  listbox_clear(&editor_text_dlg_wnd_block.active_editor_wnd->list_box);
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if ((int)unit_types[unit_type].prerequisite == editor_text_dlg_wnd_block.active_editor_wnd->item) {
      if (!units_header) {
        label = (iptr)label_string(label_string_ids[0x202]);
        listbox_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,(const char *)label);
        units_header = true;
      }
      label = (iptr)label_string(unit_types[unit_type].name);
      listbox_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,(const char *)label);
    }
  }
  for (advance = 0; advance < 100; advance++) {
    if ((int)advances[advance].preq[0] == editor_text_dlg_wnd_block.active_editor_wnd->item) {
      if (!advances_header) {
        label = (iptr)label_string(label_string_ids[0x203]);
        listbox_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,(const char *)label);
        advances_header = true;
      }
      if (advances[advance].preq[1] < '\0') {
        name2 = (iptr)label_string(advances[advance].name);
        _sprintf(line,((i8 *)&tech_editor_usage_preq1_fmt),name2);
      }
      else {
        name2 = (iptr)label_string(advances[advances[advance].preq[1]].name);
        conj = (iptr)label_string(label_string_ids[0x7b]);
        adv_name = (iptr)label_string(advances[advance].name);
        _sprintf(line,str_s_s_s,adv_name,conj,name2);
      }
      listbox_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,(const char *)line);
    }
    else if ((int)advances[advance].preq[1] == editor_text_dlg_wnd_block.active_editor_wnd->item) {
      if (!advances_header) {
        label = (iptr)label_string(label_string_ids[0x203]);
        listbox_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,(const char *)label);
        advances_header = true;
      }
      if (advances[advance].preq[0] < '\0') {
        name2 = (iptr)label_string(advances[advance].name);
        _sprintf(line,((i8 *)&tech_editor_usage_preq2_fmt),name2);
      }
      else {
        name2 = (iptr)label_string(advances[advances[advance].preq[0]].name);
        conj = (iptr)label_string(label_string_ids[0x7b]);
        adv_name = (iptr)label_string(advances[advance].name);
        _sprintf(line,str_s_s_s_2,adv_name,conj,name2);
      }
      listbox_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,(const char *)line);
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
  
  item = editor_text_dlg_wnd_block.active_editor_wnd->item;
  _strncpy(old_name,editor_text_dlg_wnd_block.editor_item_name_recs[item],0x14);
  old_name[19] = 0;
  do {
    if (editor_text_dlg_wnd_block.active_editor_wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_text_dlg_wnd_block.active_editor_wnd->window;
    }
    dlg_push_parent_window(parent_wnd2);
    result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_techname,0x13,(iptr)old_name,(iptr)new_name);
    dlg_pop_parent_window();
    if (result == -1) break;
    len = _strlen(new_name);
  } while (len == 0);
  if (-1 < result) {
    crt_strcpy(editor_text_dlg_wnd_block.editor_item_name_recs[item],new_name);
    sel1 = combo_get_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->combos[0]);
    sel2 = combo_get_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->combos[1]);
    sel3 = combo_get_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->combos[2]);
    combo_clear(&editor_text_dlg_wnd_block.active_editor_wnd->combos[0]);
    combo_clear(&editor_text_dlg_wnd_block.active_editor_wnd->combos[1]);
    combo_clear(&editor_text_dlg_wnd_block.active_editor_wnd->combos[2]);
    label = (iptr)label_string(label_string_ids[0x1f0]);
    combo_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->combos[1],(const char *)label);
    label = (iptr)label_string(label_string_ids[0x1f0]);
    combo_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->combos[2],(const char *)label);
    label = (iptr)label_string(label_string_ids[0x1f1]);
    combo_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->combos[1],(const char *)label);
    label = (iptr)label_string(label_string_ids[0x1f1]);
    combo_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->combos[2],(const char *)label);
    for (i = 0; i < 100; i++) {
      combo_add_string
                (&editor_text_dlg_wnd_block.active_editor_wnd->combos[0],(const char *)editor_text_dlg_wnd_block.editor_item_name_recs[i]);
      combo_add_string
                (&editor_text_dlg_wnd_block.active_editor_wnd->combos[1],(const char *)editor_text_dlg_wnd_block.editor_item_name_recs[i]);
      combo_add_string
                (&editor_text_dlg_wnd_block.active_editor_wnd->combos[2],(const char *)editor_text_dlg_wnd_block.editor_item_name_recs[i]);
    }
    combo_set_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->combos[0],sel1);
    combo_set_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->combos[1],sel2);
    combo_set_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->combos[2],sel3);
    tech_editor_fill_usage_list();
    tech_editor_redraw();
  }
  return;
}

/* 0041612e auto */
void __cdecl tech_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_text_dlg_wnd_block.active_editor_wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_text_dlg_wnd_block.active_editor_wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help,(const char *)&str_advances);
  dlg_set_parent_window(0);
  return;
}

/* 0041618d auto */
void __cdecl tech_editor_on_cancel(void)

{
  editor_text_dlg_wnd_block.editor_running = 0;
  wnd_end_modal_loop(&editor_text_dlg_wnd_block.active_editor_wnd->window);
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
  title = (iptr)label_string(label_string_ids[0x1f6]);
  sprite_editor_run((iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                           editor_text_dlg_wnd_block.editor_item_recs[editor_text_dlg_wnd_block.active_editor_wnd->item].values[3] * 0xf0 +
                           editor_text_dlg_wnd_block.editor_item_recs[editor_text_dlg_wnd_block.active_editor_wnd->item].values[2] * 0x3c),
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
      sel = combo_get_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->combos[0]);
      editor_text_dlg_wnd_block.active_editor_wnd->item = sel;
      tech_editor_update_controls();
      tech_editor_fill_usage_list();
      tech_editor_redraw();
    }
    else {
      combo_set_cur_sel
                (&editor_text_dlg_wnd_block.active_editor_wnd->combos[0],editor_text_dlg_wnd_block.active_editor_wnd->item);
      tech_editor_update_controls();
      tech_editor_redraw();
      if (editor_text_dlg_wnd_block.active_editor_wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_text_dlg_wnd_block.active_editor_wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_debug_11,(const char *)&str_notice_2);
      dlg_set_parent_window(0);
      hWnd = control_get_hwnd((Control *)&editor_text_dlg_wnd_block.active_editor_wnd->combos[0]);
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
void __thiscall tech_editor_create_combo(EditorWindow *self,int slot)

{
  int ctrl_id;
  iptr label;
  u8 *parent_wnd2;
  iptr advance;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,tech_editor_combo_x_table_block.tech_editor_combo_pos[slot].x + self->client_x - 0x1e,
                     tech_editor_combo_x_table_block.tech_editor_combo_pos[slot].y + self->client_y,0xa0,
                     self->font_height_uptr << 3);
  ctrl_id = editor_text_dlg_wnd_block.editor_next_ctrl_id;
  editor_text_dlg_wnd_block.editor_next_ctrl_id++;
  if (self == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&self->window;
  }
  combo_create
            ((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(iptr)parent_wnd2,ctrl_id,
             (iptr)rect);
  combo_set_font((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(iptr)&editor_font_block.editor_font);
  combo_set_callback((ComboBox *)(self->text_font + slot * 0x38 + 0x28),tech_editor_on_combo);
  switch(slot) {
  case 0:
    for (advance = 0; advance < 100; advance++) {
      label = (iptr)label_string(advances[advance].name);
      combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    }
    break;
  case 1:
  case 2:
    label = (iptr)label_string(label_string_ids[0x1f0]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x1f1]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    for (advance = 0; advance < 100; advance++) {
      label = (iptr)label_string(advances[advance].name);
      combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    }
    break;
  case 3:
    label = (iptr)label_string(label_string_ids[0x204]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x205]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x206]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x207]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    break;
  case 4:
    label = (iptr)label_string(label_string_ids[0x208]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x209]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x20a]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x20b]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
    label = (iptr)label_string(label_string_ids[0x20c]);
    combo_add_string((ComboBox *)(self->text_font + slot * 0x38 + 0x28),(const char *)label);
  }
  return;
}

/* 00416734 auto */
void __thiscall tech_editor_create_edit(EditorWindow *self,int slot)

{
  int ctrl_id;
  u8 *parent_wnd2;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,(((i32 *)&tech_editor_edit_x_table_block.tech_editor_edit_x_table))[slot * 2] + self->client_x,
                     (((i32 *)&tech_editor_edit_x_table_block.tech_editor_edit_y_table))[slot * 2] + self->client_y,0x30,self->font_height_uptr + 6);
  ctrl_id = editor_text_dlg_wnd_block.editor_next_ctrl_id;
  editor_text_dlg_wnd_block.editor_next_ctrl_id++;
  if (self == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&self->window;
  }
  edit_create
            ((EditControl *)(self->numeric_edit_controls_area + slot * 0x48 - 0x2c),(iptr)parent_wnd2,ctrl_id,
             (iptr)rect,(const char *)&str_empty_11);
  edit_set_max_len((EditControl *)(self->numeric_edit_controls_area + slot * 0x48 - 0x2c),4);
  edit_set_callback((EditControl *)(self->numeric_edit_controls_area + slot * 0x48 - 0x2c),editor_numeric_edit_filter);
  return;
}

/* 00416828 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall tech_editor_paint(EditorWindow *self)

{
  byte key_color;
  i8 sprite_rect[16];
  Sprite *icon;
  int label_y;
  int label_x;
  int icon_y;
  int icon_x;
  
  frame_wnd_draw_caption(self);
  if (((*(int *)&editor_bg_surface) == 0) || (editor_text_dlg_wnd_block.editor_running == 0)) {
    gfx_fill_rect((iptr)self,(RECT *)&self->client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((*(int *)&editor_bg_surface),(iptr)self,0,0,self->client_x,self->client_y,self->wnd_width_u,
                       self->wnd_height_u);
  }
  icon_x = self->client_x + 0x20;
  icon_y = self->client_y + 0x20;
  icon = (Sprite *)
             (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
             editor_text_dlg_wnd_block.editor_item_recs[self->item_uptr].values[3] * 0xf0 +
             editor_text_dlg_wnd_block.editor_item_recs[self->item_uptr].values[2] * 0x3c);
  key_color = sprite_get_key_color(icon);
  gfx_fill_rect_xywh((iptr)self,icon_x,icon_y,0x48,0x28,(uint)key_color);
  gfx_set_scale_ratio(2,1);
  sprite_draw(icon,(iptr)sprite_rect,(iptr)self,icon_x,icon_y);
  gfx_set_scale_ratio(1,1);
  gfx_draw_tiled_frame((iptr)self,icon_x,icon_y,0x48,0x28,6);
  text_set_surface((iptr)self);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  label_x = tech_editor_combo_x_table_block.tech_editor_prereq1_ctrl_x + self->client_x + 0x32;
  label_y = ((tech_editor_combo_x_table_block.tech_editor_prereq1_ctrl_y + self->client_y) - self->font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&tech_editor_prereq1_suffix_str));
  text_draw_centered((iptr)self,(const char *)&map_views_block.text_buffer,label_x,label_y,0);
  label_x = tech_editor_combo_x_table_block.tech_editor_prereq2_ctrl_x + self->client_x + 0x32;
  label_y = ((tech_editor_combo_x_table_block.tech_editor_prereq2_ctrl_y + self->client_y) - self->font_height_uptr) - 2;
  text_clear();
  text_append_label(0x7e);
  crt_strcat(((i8 *)&map_views_block.text_buffer),((i8 *)&tech_editor_prereq2_suffix_str));
  text_draw_centered((iptr)self,(const char *)&map_views_block.text_buffer,label_x,label_y,0);
  label_x = tech_editor_combo_x_table_block.tech_editor_epoch_ctrl_x + self->client_x + 0x32;
  label_y = ((tech_editor_combo_x_table_block.tech_editor_epoch_ctrl_y + self->client_y) - self->font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1eb);
  text_draw_centered((iptr)self,(const char *)&map_views_block.text_buffer,label_x,label_y,0);
  label_x = tech_editor_combo_x_table_block.tech_editor_category_ctrl_x + self->client_x + 0x32;
  label_y = ((self->client_y + tech_editor_combo_x_table_block.tech_editor_category_ctrl_y_i32) - self->font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1ec);
  text_draw_centered((iptr)self,(const char *)&map_views_block.text_buffer,label_x,label_y,0);
  label_x = tech_editor_edit_x_table_block.tech_editor_edit_x_table[0] + self->client_x + 0x18;
  label_y = ((tech_editor_edit_x_table_block.tech_editor_edit_y_table[0] + self->client_y) - self->font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1ed);
  text_draw_centered((iptr)self,(const char *)&map_views_block.text_buffer,label_x,label_y,0);
  label_x = tech_editor_edit_x_table_block.tech_editor_modifier_edit_x + self->client_x + 0x18;
  label_y = ((tech_editor_edit_x_table_block.tech_editor_modifier_edit_y + self->client_y) - self->font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1ee);
  text_draw_centered((iptr)self,(const char *)&map_views_block.text_buffer,label_x,label_y,0);
  label_x = self->client_x + (int)self->wnd_width_u / 2;
  label_y = (self->client_y - self->font_height_uptr) + 0xe6;
  text_clear();
  text_append_label(0x1ea);
  text_draw_centered((iptr)self,(const char *)&map_views_block.text_buffer,label_x,label_y,0);
  wnd_blit_surface(self);
  return;
}

/* 00416c9e auto */
void __thiscall tech_editor_dialog_run(EditorWindow *self)

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
  i8 *eh_handler;
  int eh_state;
  
  eh_state = 0xffffffff;
  palette_init(&palette);
  eh_state = 0;
  editor_text_dlg_wnd_block.editor_running = 1;
  editor_text_dlg_wnd_block.active_editor_wnd = self;
  this_00 = (DibSurface *)operator_new(0x48);
  (*((char *)&eh_state + 0)) = 1;
  if (this_00 == (DibSurface *)0x0) {
    surface = (DibSurface *)0x0;
  }
  else {
    surface = surface_ctor(this_00);
  }
  eh_state = eh_state & 0xffffff00;
  editor_bg_surface = surface;
  font_set((GameFont *)((i8 *)&editor_font_block.editor_font),0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  self->wnd_width_u = 0x230;
  self->wnd_height_u = 0x17c;
  self->item_uptr = 0;
  editor_text_dlg_wnd_block.editor_next_ctrl_id = 0xc9;
  surface_load_gif_file(editor_bg_surface,(const char *)&str_editorpt_gif,10,0xc0,(iptr)&palette);
  font_height = font_cached_height((GameFont *)((i8 *)&editor_font_block.editor_font));
  self->font_height_uptr = font_height;
  extra_arg3 = 0;
  extra_arg2 = 0;
  extra_arg1 = 0;
  height = self->wnd_height_u;
  width = self->wnd_width_u;
  top = 0;
  left = 0;
  style = 0xd;
  title = (iptr)label_string(label_string_ids[0x1cd]);
  frame_wnd_create
            (self,(const char *)title,style,left,top,width,height,extra_arg1,extra_arg2,extra_arg3);
  for (row = 0; row < 7; row++) {
    if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 9) {
      tech_editor_create_edit(self,tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot);
    }
    else if (tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].kind == 0xc) {
      tech_editor_create_combo(self,tech_editor_ctrl_kinds_block.tech_editor_ctrl_rows[row].slot);
    }
  }
  rect_set_xywh((iptr)rect,self->client_x + 0x32,self->client_y + 0xe7,0x1cb,0x6e);
  pos = editor_text_dlg_wnd_block.editor_next_ctrl_id;
  editor_text_dlg_wnd_block.editor_next_ctrl_id++;
  if (self == (EditorWindow *)0x0) {
    list_parent = (u8 *)0x0;
  }
  else {
    list_parent = (u8 *)&self->window;
  }
  listbox_create
            (&self->list_box,(iptr)list_parent,pos,(iptr)rect);
  listbox_set_font(&self->list_box,(iptr)&editor_font_block.editor_font);
  pos = font_line_spacing((GameFont *)(*(i8 * *)(dialog_default_text_font_rest + 8)));
  self->button_height_u = pos + 8;
  pos = self->client_width - 10;
  self->button_width_u = (int)(pos + (pos >> 0x1f & 3U)) >> 2;
  btn_y = (self->client_y + self->client_height) - (self->button_height_u + 2);
  pos = self->client_x + 2;
  rect_set_xywh((iptr)rect,pos,btn_y,self->button_width_u,self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0xfe]);
  button_create
            (&self->ok_button,(iptr)ok_parent,0x65,(iptr)rect,(const char *)label);
  button_set_callback(&self->ok_button,(CtrlNotifyFn)tech_editor_on_ok);
  pos = pos + self->button_width_u + 2;
  rect_set_xywh((iptr)rect,pos,btn_y,self->button_width_u,self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    rename_parent = (u8 *)0x0;
  }
  else {
    rename_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0x2a]);
  button_create
            (&self->rename_button,(iptr)rename_parent,0x65,(iptr)rect,(const char *)label);
  button_set_callback(&self->rename_button,(CtrlNotifyFn)tech_editor_on_rename);
  pos = pos + self->button_width_u + 2;
  rect_set_xywh((iptr)rect,pos,btn_y,self->button_width_u,self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0x23b]);
  button_create
            (&self->help_button,(iptr)help_parent,0x66,(iptr)rect,(const char *)label);
  button_set_callback(&self->help_button,(CtrlNotifyFn)tech_editor_on_help);
  rect_set_xywh((iptr)rect,pos + self->button_width_u + 2,btn_y,self->button_width_u,
                     self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0xff]);
  button_create
            (&self->cancel_button,(iptr)cancel_parent,0x66,(iptr)rect,(const char *)label);
  button_set_callback(&self->cancel_button,(CtrlNotifyFn)tech_editor_on_cancel);
  ctrl_set_cancel_button(&self->cancel_button);
  self->button_width_u = 0x55;
  pos = font_line_spacing((GameFont *)(*(i8 * *)(dialog_default_text_font_rest + 8)));
  self->button_height_u = pos + 8;
  rect_set_xywh((iptr)rect,self->client_x + 0x19,self->client_y + 0x50,self->button_width_u,
                     self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    icon_parent = (u8 *)0x0;
  }
  else {
    icon_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0x1f3]);
  button_create
            (&self->icon_button,(iptr)icon_parent,0x65,(iptr)rect,(const char *)label);
  button_set_callback(&self->icon_button,(CtrlNotifyFn)tech_editor_on_edit_icon);
  wnd_set_keyboard_nav(&self->window,0);
  tech_editor_load_items();
  combo_set_cur_sel((ComboBox *)&self->combos_u,self->item_uptr);
  tech_editor_update_controls();
  self->combo_count_u = 5;
  wnd_set_move_handler(&self->window.wnd_handlers,(WndMouseFn)editor_wnd_on_move_close_dropdowns);
  wnd_set_redraw_callback(self,tech_editor_redraw);
  tech_editor_on_combo(0xc9);
  wnd_repaint(self);
  window_show(self);
  wnd_run_modal_loop((ModalMenuWindow *)&self->window);
  while (editor_text_dlg_wnd_block.editor_running != 0) {
    win_pump_one_message();
  }
  if (editor_bg_surface != (DibSurface *)0x0) {
    surface_delete(editor_bg_surface,1);
  }
  editor_bg_surface = (DibSurface *)0x0;
  self->combo_count_u = 0;
  eh_state = 0xffffffff;
  /* inlined unwind funclet 00417542 */ { palette_destroy(&palette); }
  return;
}

/* 00417566 auto */
void __cdecl tech_editor_run(void)

{
  EditorWindow editor_wnd;
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  editor_wnd_ctor(&editor_wnd);
  seh_state = 0;
  tech_editor_dialog_run(&editor_wnd);
  wnd_repaint((SurfaceWindow *)((i8 *)&map_views_block.map_views));
  seh_state = 0xffffffff;
  /* inlined unwind funclet 004175bf */ { editor_wnd_dtor(&editor_wnd); }
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
    line_len = _strlen(((i8 *)&map_views_block.text_buffer));
    if ((line_len == 0) || (map_views_block.text_buffer[0] == ';')) break;
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
        error_report(0xfffffffc,3,(iptr)&map_views_block.text_buffer,advance,preq_index);
      }
    }
    value = text_next_int();
    advances[advance].epoch = (u8)value;
    value = text_next_int();
    advances[advance].category = (u8)value;
    if (advances[advance].preq[0] == -2) {
      advances[advance].enabled = '\0';
    }
    else {
      advances[advance].enabled = 1;
    }
  }
  for (; advance < 100; advance++) {
    if (keep_names == 0) {
      _sprintf(((i8 *)&map_views_block.text_token_buf),str_advance_d,advance + 1);
      name_id = strpool_add_padded((const char *)&map_views_block.text_token_buf,0x14);
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
      error_report(0xfffffffb,3,(iptr)&map_views_block.text_buffer,improvement,0);
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
      error_report(0xfffffffc,3,(iptr)&map_views_block.text_buffer,improvement,0);
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
    line_len = _strlen(((i8 *)&map_views_block.text_buffer));
    if ((line_len == 0) || (map_views_block.text_buffer[0] == ';')) break;
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
      error_report(0xfffffffa,3,(iptr)&map_views_block.text_buffer,unit_type,0);
    }
  }
  for (; unit_type < 0x3e; unit_type++) {
    if (keep_names == 0) {
      _sprintf(((i8 *)&map_views_block.text_token_buf),str_unit_d,unit_type + 1);
      name_id = strpool_add_padded((const char *)&map_views_block.text_token_buf,0xf);
      unit_types[unit_type].name = name_id;
    }
    unit_types[unit_type].obsolete_by = -2;
    unit_types[unit_type].domain = '\0';
    unit_types[unit_type].move = '\0';
    unit_types[unit_type].range = '\0';
    unit_types[unit_type].attack = 1;
    unit_types[unit_type].defense = '\0';
    unit_types[unit_type].hit_points = '\0';
    unit_types[unit_type].firepower = '\0';
    unit_types[unit_type].cost = 1;
    unit_types[unit_type].hold = '\0';
    unit_types[unit_type].role = '\0';
    unit_types[unit_type].prerequisite = -2;
    unit_types[unit_type].flags = 0;
    if ((unit_types[unit_type].obsolete_by == -3) || (unit_types[unit_type].prerequisite == -3)) {
      error_report(0xfffffffa,3,(iptr)&map_views_block.text_buffer,unit_type,0);
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
          error_report(0xfffffff9,3,(iptr)&map_views_block.text_buffer,terrain2,slot);
        }
      }
      token = text_next_token();
      transform_terrain = rules_parse_terrain_abbrev(token);
      terrain_types[terrain2].transform_to = transform_terrain;
      if (terrain_types[terrain2].transform_to == -3) {
        error_report(0xfffffff9,3,(iptr)&map_views_block.text_buffer,terrain2,99);
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
    order_names[i].key = map_views_block.text_token_buf[0];
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
  text_set_string_arg(2,(const char *)&map_views_block.text_buffer);
  if (2 < game.multiplayer) {
    dlg_pending_idle_callback = turn_popup_timeout_idle;
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
    __chdir(((i8 *)&civ_saved_cursor_x_block.game_home_dir));
  }
  rules_parse_sections(load_civ_data);
  __chdir(((i8 *)&civ_saved_cursor_x_block.game_home_dir));
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
  font_init_styled((GameFont *)((i8 *)&civ_rating_title_font_block.civ_rating_title_font),0,0xc,1);
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
  font_destroy((GameFont *)((i8 *)&civ_rating_title_font_block.civ_rating_title_font));
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
  
  civ = advisor_window_block.advisor_report_civ;
  frame_wnd_layout((FrameWindow *)((i8 *)&advisor_window_block.advisor_window));
  frame_wnd_draw_caption((FrameWindow *)((i8 *)&advisor_window_block.advisor_window));
  picwnd_blit((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
  cur_y = advisor_window_block.advisor_client_y + 2;
  list_bottom = advisor_window_block.advisor_client_y + 0x174;
  line_height = font_line_spacing((GameFont *)((i8 *)&advisor_font_block.advisor_font));
  text_set_surface((iptr)&advisor_window_block.advisor_window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_string_ids[0x153]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
  cur_y += line_height;
  text_clear();
  govt_index = int_clamp(civs[civ].government - 1,0,4);
  text_append_label(govt_index + 0x148);
  text_append_space();
  text_append_label(0x8c);
  text_append_space();
  text = (iptr)(civ_name_plural(civ));
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
  cur_y += line_height;
  text_clear();
  text = civ_leader_title(civ);
  text_append((const char *)text);
  text_append_space();
  text = (iptr)(civ_leader_name(civ));
  text_append((const char *)text);
  text_append_colon();
  text_append_year((int)game.year);
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
  cur_y = cur_y + line_height + 6;
  if (-1 < civs[civ].researching) {
    box_top = cur_y;
    text_clear();
    text_append_label(0xec);
    text_append_colon();
    text_append_string_id(advances[civs[civ].researching].name);
    gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
    cur_y += line_height;
    draw_y = cur_y + 9;
    bar_flag = 1;
    value = rect_width((RECT *)&advisor_window_block.advisor_client_rect);
    value -= 10;
    bar_height = 0xe;
    max_value = civ_research_cost(civ);
    sprite_draw_spaced_row((iptr)&advisor_window_block.advisor_window,(iptr)&tax_rate_icon_sprites_block.science_icon_sprite,advisor_window_block.advisor_client_rect + 5,draw_y,
                       (int)civs[civ].research_progress,max_value,bar_height,value,bar_flag);
    box_bottom = cur_y + 0x20;
    cur_y = box_bottom;
    rect_set((iptr)frame_rect2,advisor_window_block.advisor_client_rect + 2,box_top,advisor_window_block.advisor_client_right - 2,box_bottom);
    gfx_draw_rect_frame((iptr)&advisor_window_block.advisor_window,(RECT *)frame_rect2,0x29);
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
    gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
    cur_y = cur_y + line_height + 6;
  }
  scrollbar_height = GetSystemMetrics(3);
  list_bottom -= (scrollbar_height + 2);
  line_height = 0x16;
  advisor_window_block.advisor_row_height = 0x16;
  advisor_window_block.advisor_list_top_i32 = cur_y;
  advisor_window_block.advisor_list_height_i32 = list_bottom - cur_y;
  advisor_window_block.advisor_rows_per_page = int_clamp((list_bottom - cur_y) / 0x16,1,99);
  advisor_window_block.advisor_list_left_i32 = advisor_window_block.advisor_client_x + 2;
  list_right = advisor_window_block.advisor_client_width + advisor_window_block.advisor_client_x - 2;
  advisor_window_block.advisor_list_width = list_right - advisor_window_block.advisor_list_left_i32;
  advisor_window_block.advisor_column_width = advisor_window_block.advisor_list_width / 3;
  advisor_window_block.advisor_items_per_page = advisor_window_block.advisor_rows_per_page * 3;
  known_count = 0;
  list_left = advisor_window_block.advisor_list_left_i32;
  rows_per_page = advisor_window_block.advisor_rows_per_page;
  for (advance = 0; advance < 100; advance++) {
    value = civ_has_advance(civ,advance);
    if (value != 0) {
      known_count++;
    }
  }
  advisor_window_block.advisor_page_count_i32 = int_clamp((rows_per_page - 1 + known_count) / rows_per_page,1,99);
  max_value = int_clamp(known_count - 1,0,999);
  advisor_window_block.advisor_first_row = int_clamp(advisor_window_block.advisor_first_row,0,max_value);
  item_index = 0;
  column = 0;
  row = 0;
  draw_y = cur_y;
  first_item = advisor_window_block.advisor_first_row;
  text_set_font((iptr)&advisor_bold_font);
  for (advance = 0; advance < 100; advance++) {
    has_advance = civ_has_advance(civ,advance);
    value = item_index;
    if (((has_advance != 0) && (value = item_index + 1, first_item <= item_index)) &&
       (item_index < advisor_window_block.advisor_items_per_page + first_item)) {
      draw_y = row * line_height + cur_y;
      item_x = advisor_window_block.advisor_column_width * column + list_left;
      item_index = value;
      sprite_draw
                ((Sprite *)
                 (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                 (char)advances[advance].epoch * 0x3c + (char)advances[advance].category * 0xf0),
                 (iptr)sprite_rect,(iptr)&advisor_window_block.advisor_window,item_x,draw_y);
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
  if (advisor_window_block.advisor_controls_created == 0) {
    rect_set((iptr)frame_rect2,list_left,list_bottom,list_right,list_bottom + scrollbar_height);
    scrollbar_create((Scrollbar *)((i8 *)&advisor_window_block.advisor_scrollbar),(iptr)&advisor_window_block.advisor_wnd,200,(iptr)frame_rect2,0);
    max_value = int_clamp((rows_per_page - 1 + known_count) / rows_per_page - 1,0,999);
    scrollbar_set_range((Scrollbar *)((i8 *)&advisor_window_block.advisor_scrollbar),0,max_value);
    scrollbar_set_pos((Scrollbar *)((i8 *)&advisor_window_block.advisor_scrollbar),advisor_window_block.advisor_first_row);
    scrollbar_set_page_size((Scrollbar *)((i8 *)&advisor_window_block.advisor_scrollbar),1);
    scrollbar_set_callback((Scrollbar *)((i8 *)&advisor_window_block.advisor_scrollbar),science_advisor_on_scroll);
    ctrl_show_and_redraw((Control *)((i8 *)&advisor_window_block.advisor_scrollbar));
    wnd_set_lbuttonup_handler((WndHandlers *)((i8 *)&advisor_window_block.advisor_wnd_events),science_advisor_on_click);
    advisor_window_block.advisor_controls_created = 1;
  }
  wnd_blit_surface((DibSurface *)((i8 *)&advisor_window_block.advisor_window));
  return;
}

/* 0042b540 auto */
void __cdecl science_advisor_on_scroll(int scroll_pos)

{
  advisor_window_block.advisor_first_row = advisor_window_block.advisor_rows_per_page * scroll_pos;
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
  
  if ((((-1 < click_y - advisor_window_block.advisor_list_top_i32) &&
       (row = (click_y - advisor_window_block.advisor_list_top_i32) / advisor_window_block.advisor_row_height, row < advisor_window_block.advisor_rows_per_page)) &&
      (-1 < click_x - 2)) && (column = (click_x - 2) / advisor_window_block.advisor_column_width, column < 3)) {
    column = advisor_window_block.advisor_rows_per_page * column + advisor_window_block.advisor_first_row;
    known_index = 0;
    for (advance = 0; advance < 100; advance++) {
      has_advance = civ_has_advance(advisor_window_block.advisor_report_civ,advance);
      if ((has_advance != 0) && (is_match = known_index == column + row, known_index = known_index + 1, is_match)) {
        pedia_show_topic_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),advance);
        return;
      }
    }
  }
  return;
}

/* 0042b65b auto */
void __cdecl science_advisor_on_goal_clicked(void)

{
  civ_research_goal_dialog(advisor_window_block.advisor_report_civ,1,0);
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
  
  picwnd_open((PictureWindow *)((i8 *)&advisor_window_block.advisor_window),6,6,0,600,400,0,0);
  advisor_window_block.advisor_report_civ = civ;
  rect_set_xywh((iptr)button_rect,2,0,0x129,0x18);
  client_y = advisor_window_block.advisor_client_y;
  rect_h = rect_height((RECT *)button_rect);
  rect_offset((iptr)button_rect,advisor_window_block.advisor_client_x,(client_y - rect_h) + 0x18e);
  rect_offset((iptr)button_rect,299,0);
  label = (iptr)label_string(label_string_ids[0x147]);
  button_create
            ((Button *)((i8 *)&advisor_window_block.advisor_close_button),(iptr)&advisor_window_block.advisor_wnd,100,(iptr)button_rect,(const char *)label);
  button_set_callback((Button *)((i8 *)&advisor_window_block.advisor_close_button),(CtrlNotifyFn)advisor_on_close_clicked);
  ctrl_set_default_button((Button *)((i8 *)&advisor_window_block.advisor_close_button));
  ctrl_set_cancel_button((Button *)((i8 *)&advisor_window_block.advisor_close_button));
  rect_offset((iptr)button_rect,0xfffffed5,0);
  label = (iptr)label_string(label_string_ids[0x173]);
  button_create
            ((Button *)((i8 *)&advisor_window_block.advisor_toggle_button),(iptr)&advisor_window_block.advisor_wnd,0x65,(iptr)button_rect,(const char *)label);
  button_set_callback((Button *)((i8 *)&advisor_window_block.advisor_toggle_button),(CtrlNotifyFn)science_advisor_on_goal_clicked);
  wnd_set_redraw_callback((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window),science_advisor_draw);
  wnd_repaint((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window));
  window_show((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window));
  return;
}

/* 00434e39 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl civ_score_window_draw(void)

{
  iptr text;
  int text_y2;
  int value;
  i8 wonder_rect[16];
  i8 citizen_rect[16];
  iptr civ;
  int text_y3;
  int content_count;
  int list_bottom;
  int left_x;
  int header_y;
  uint sprite_index;
  iptr city;
  int column;
  int happy_count;
  int total_width;
  uint citizen_count;
  int text_x2;
  iptr threshold;
  int draw_y;
  int draw_x;
  int citizen;
  int unhappy_count;
  iptr wonder;
  iptr civ2;
  uint row;
  int measure_pass;
  int row_height;
  int row_wrapped;
  int line_height;
  int era;
  int icon_width;
  
  civ = advisor_window_block.advisor_report_civ;
  frame_wnd_layout((FrameWindow *)((i8 *)&advisor_window_block.advisor_window));
  frame_wnd_draw_caption((FrameWindow *)((i8 *)&advisor_window_block.advisor_window));
  picwnd_blit((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
  header_y = advisor_window_block.advisor_client_y + 2;
  list_bottom = advisor_window_block.advisor_client_y + 0x174;
  line_height = font_line_spacing((GameFont *)((i8 *)&advisor_font_block.advisor_font));
  text_set_surface((iptr)&advisor_window_block.advisor_window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_string_ids[0x157]);
  text_append((const char *)text);
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,header_y,advisor_window_block.advisor_client_width);
  header_y += line_height;
  text_set_font((iptr)&advisor_bold_font);
  line_height = font_line_spacing((GameFont *)((i8 *)&advisor_bold_font));
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
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,header_y,advisor_window_block.advisor_client_width);
  advisor_window_block.advisor_list_top_i32 = header_y + line_height + 4;
  advisor_window_block.advisor_row_height = line_height;
  advisor_window_block.advisor_list_height_i32 = list_bottom - advisor_window_block.advisor_list_top_i32;
  left_x = advisor_window_block.advisor_client_x + 2;
  text_y3 = advisor_window_block.advisor_list_top_i32;
  if ((game.game_flags & 0x20) == 0) {
    if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 2) == 0)) {
      header_y = advisor_window_block.advisor_list_top_i32;
      era = civ_city_style_era(civ);
      draw_y = text_y3;
      draw_x = left_x;
      citizen_count = 0;
      row = 0;
      column = 0;
      row_height = 0x12;
      icon_width = 0x14;
      row_wrapped = 0;
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city].owner == civ)) {
          city_recalc(city,1);
          happy_count = main_menu_block.city_happy_count;
          unhappy_count = main_menu_block.city_unhappy_count;
          content_count = (((cities[city].size - main_menu_block.city_happy_count) - main_menu_block.city_unhappy_count) - main_menu_block.city_specialist_count) -
                     main_menu_block.city_angry_count;
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
            else if (((citizen - happy_count) - content_count) - unhappy_count < main_menu_block.city_angry_count) {
              sprite_index = (citizen_count & 1) + 6;
            }
            else {
              value = city_specialist_type(city,(((citizen - happy_count) - content_count) - unhappy_count) -
                                                  main_menu_block.city_angry_count);
              sprite_index = value + 7;
            }
            citizen_count++;
            sprite_draw
                      ((Sprite *)(((i8 *)&road_sprites_block.citizen_sprites) + era * 0x294 + sprite_index * 0x3c),
                       (iptr)citizen_rect,(iptr)&advisor_window_block.advisor_window,draw_x,draw_y);
            row_wrapped = 0;
            draw_x += icon_width;
            column++;
            if (advisor_window_block.advisor_client_width + advisor_window_block.advisor_client_x - 0x1d < draw_x) {
              column = 0;
              row++;
              row_wrapped = 1;
              draw_x = (icon_width / 2) * (row & 1) + left_x;
              draw_y += row_height;
              if ((6 < (int)row) || (0xf0 < draw_y)) goto draw_civ_row;
            }
          }
        }
      }
draw_civ_row:
      if (row_wrapped != 0) {
        draw_y -= row_height;
      }
      text_y3 = draw_y + 0x22;
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
      text_append_int(map_views_block.score_citizens);
      text_append_rparen();
      gfx_draw_text((const char *)&map_views_block.text_buffer,left_x,text_y3);
      text_y3 = text_y3 + line_height + 4;
      draw_x = left_x;
      column = 0;
      row_wrapped = 0;
      draw_y = text_y3;
      for (wonder = 0; wonder < 0x1c; wonder++) {
        if ((((-1 < game.wonder_city[wonder]) && (game.wonder_city[wonder] < game.num_cities))
            && (cities[game.wonder_city[wonder]].id != 0)) &&
           (city = (iptr)game.wonder_city[wonder], cities[city].owner == civ)) {
          sprite_draw
                    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + wonder * 0x3c),(iptr)wonder_rect,(iptr)&advisor_window_block.advisor_window,
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
      text_append_int(map_views_block.score_wonders);
      text_append_rparen();
      gfx_draw_text((const char *)&map_views_block.text_buffer,left_x,text_y3);
      text_y3 += line_height;
      if ((map_views_block.score_spaceship_arrived != 0) || (map_views_block.score_spaceship_in_flight != 0)) {
        text_clear();
        text_append_label(0x195);
        text_append_colon();
        text_append_lparen();
        if (map_views_block.score_spaceship_arrived == 0) {
          text_append_int(map_views_block.score_spaceship_in_flight);
          text_append_space();
          text_append_label(0x199);
        }
        else {
          text_append_int(map_views_block.score_spaceship_arrived);
        }
        text_append_rparen();
        gfx_draw_text((const char *)&map_views_block.text_buffer,left_x,text_y3);
        text_y3 += line_height;
      }
      row_wrapped = 0;
      text_clear();
      if (map_views_block.score_pollution != 0) {
        text_append_label(0x17c);
        text_append_space();
        text_append_lparen();
        text_append_int(map_views_block.score_pollution);
        text_append_rparen();
        text_append((const char *)&str_empty_52);
        row_wrapped = 1;
      }
      if ((199 < game.turn) && (map_views_block.score_peace != 0)) {
        text_append_label(0x196);
        text_append_colon();
        text_append_lparen();
        text_append((const char *)&str_empty_53);
        text_append_int(map_views_block.score_peace);
        text_append_rparen();
        text_append((const char *)&str_empty_54);
        row_wrapped = 1;
      }
      if (map_views_block.score_future_techs != 0) {
        text_append_string_id(advances[0x59].name);
        text_append_colon();
        text_append_lparen();
        text_append((const char *)&str_empty_55);
        text_append_int(map_views_block.score_future_techs);
        text_append_rparen();
        text_append((const char *)&str_empty_56);
        row_wrapped = 1;
      }
      text_append_label(0xf);
      text_append_colon();
      text_append_lparen();
      if (-1 < map_views_block.score_barbarian_bonus) {
        text_append((const char *)&str_empty_57);
      }
      text_append_int(map_views_block.score_barbarian_bonus);
      text_append_rparen();
      row_wrapped = 1;
      gfx_draw_text((const char *)&map_views_block.text_buffer,left_x,text_y3);
      text_y3 += line_height;
    }
    else {
      text_y3 = header_y + line_height + 0x2a;
      header_y = advisor_window_block.advisor_list_top_i32;
      civ_calc_score(civ);
      text_clear();
      text_append_label(0x1b1);
      text_append_colon();
      text_append_int(map_views_block.score_total_objective_value);
      gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,text_y3,advisor_window_block.advisor_client_width);
      text_y3 += line_height;
      if (prefs_game_rules_block.scenario_objective_civ != 0) {
        text_clear();
        text = civ_get_adjective((int)prefs_game_rules_block.scenario_objective_civ);
        text_append((const char *)text);
        text_append_space();
        text_append_label(0x1b2);
        text_append_colon();
        text_append_int(map_views_block.score_objective_civ_value);
        gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,text_y3,advisor_window_block.advisor_client_width);
        text_y3 += line_height;
      }
      if ((prefs_game_rules_block.scenario_objective_civ == 0) || (prefs_game_rules_block.scenario_objective_civ != civ)) {
        text_clear();
        text = civ_get_adjective(civ);
        text_append((const char *)text);
        text_append_space();
        text_append_label(0x1b2);
        text_append_colon();
        text_append_int(map_views_block.score_civ_objective_value);
        gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,text_y3,advisor_window_block.advisor_client_width);
        text_y3 += line_height;
      }
      if (prefs_game_rules_block.scenario_objective_civ == 0) {
        civ2 = civ;
      }
      else {
        civ2 = (iptr)prefs_game_rules_block.scenario_objective_civ;
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
        text_append_int((int)(((i16 *)&prefs_game_rules_block.scenario_victory_thresholds))[threshold]);
        gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,text_y3,advisor_window_block.advisor_client_width);
        text_y3 += line_height;
      }
      text_y3 += 6;
      text_set_colors(0x7a,0x12,1,1);
      text_set_font((iptr)&advisor_font_block.advisor_font);
      line_height = font_line_spacing((GameFont *)((i8 *)&advisor_font_block.advisor_font));
      text_clear();
      text = civ_get_adjective(civ2);
      text_append((const char *)text);
      text_append_space();
      text_append_label(map_views_block.score_objective_outcome + 0x1b3);
      gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,text_y3,advisor_window_block.advisor_client_width);
      text_y3 = text_y3 + line_height + 6;
    }
    measure_pass = 0;
    total_width = 0;
    if (((game.game_flags & 0x80) != 0) && ((prefs_game_rules_block.scenario_flags[0] & 2) != 0)) {
      map_views_block.score_conquest_bonus = -1;
      map_views_block.score_total = map_views_block.score_scenario_points;
      measure_pass = 1;
    }
    while( true ) {
      text_set_font((iptr)&advisor_font_block.advisor_font);
      text_clear();
      if (map_views_block.score_total < map_views_block.score_conquest_bonus) {
        text_append_label(0x198);
        text_append_colon();
        text_append_int(map_views_block.score_conquest_bonus);
      }
      else {
        text_append_label(0x197);
        text_append_colon();
        text_append_int(map_views_block.score_total);
      }
      if (measure_pass == 1) {
        value = font_text_width((GameFont *)((i8 *)&advisor_font_block.advisor_font),(const char *)&map_views_block.text_buffer);
        total_width += value;
      }
      else {
        text_set_colors(0x25,0x12,1,1);
        draw_x = gfx_draw_text((const char *)&map_views_block.text_buffer,left_x,text_y3);
      }
      if ((game.game_flags & 0x80) != 0) {
        draw_x += 0x19;
        text_set_colors(0x7a,0x12,0xffffffff,0xffffffff);
        text_clear();
        text_append_label(0x1a2);
        if (measure_pass == 1) {
          value = font_text_width((GameFont *)((i8 *)&advisor_font_block.advisor_font),(const char *)&map_views_block.text_buffer);
          total_width = total_width + value + 0x19;
        }
        else {
          draw_x = gfx_draw_text((const char *)&map_views_block.text_buffer,draw_x,text_y3);
        }
      }
      draw_x += 0x19;
      if ((game.game_flags & 0x10) != 0) {
        text_set_colors(0x6a,0x12,0xffffffff,0xffffffff);
        text_clear();
        text_append_label(0x19a);
        if (measure_pass == 1) {
          value = font_text_width((GameFont *)((i8 *)&advisor_font_block.advisor_font),(const char *)&map_views_block.text_buffer);
          total_width = total_width + value + 0x19;
        }
        else {
          gfx_draw_text((const char *)&map_views_block.text_buffer,draw_x,text_y3);
        }
      }
      if (measure_pass != 1) break;
      left_x = ((advisor_window_block.advisor_client_width >> 1) + advisor_window_block.advisor_client_x) - (total_width >> 1);
      measure_pass = 2;
    }
  }
  else {
    header_y = advisor_window_block.advisor_list_top_i32;
    text_clear();
    text_append_label(0x1c9);
    gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,header_y,advisor_window_block.advisor_client_width);
  }
  wnd_blit_surface((DibSurface *)((i8 *)&advisor_window_block.advisor_window));
  return;
}

/* 00435d15 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl civ_score_window_show(iptr civ)

{
  picwnd_open((PictureWindow *)((i8 *)&advisor_window_block.advisor_window),10,10,1,600,400,0,0);
  advisor_window_block.advisor_report_civ = civ;
  advisor_add_close_button((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
  wnd_set_redraw_callback((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window),civ_score_window_draw);
  wnd_repaint((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window));
  window_show((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window));
  if (2 < game.multiplayer) {
    wnd_set_modal_idle_callback((ModalMenuWindow *)((i8 *)&advisor_window_block.advisor_wnd),advisor_mp_idle);
  }
  advisor_modal_start_ticks_i32 = timer_ticks();
  wnd_run_modal_loop((ModalMenuWindow *)((i8 *)&advisor_window_block.advisor_wnd));
  wnd_set_modal_idle_callback((ModalMenuWindow *)((i8 *)&advisor_window_block.advisor_wnd),0);
  picwnd_close((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
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
  i8 line_buf[80];
  int line_height;
  int open_failed;
  int best_score;
  
  civ = advisor_window_block.advisor_report_civ;
  frame_wnd_layout((FrameWindow *)((i8 *)&advisor_window_block.advisor_window));
  frame_wnd_draw_caption((FrameWindow *)((i8 *)&advisor_window_block.advisor_window));
  picwnd_blit((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
  cur_y = advisor_window_block.advisor_client_y + 1;
  bottom_y = advisor_window_block.advisor_client_y + 0x174;
  line_height = font_line_spacing((GameFont *)((i8 *)&advisor_font_block.advisor_font));
  line_height -= 3;
  difficulty_factor = game.difficulty + 4;
  if (2 < game.difficulty) {
    difficulty_factor = game.difficulty + 5;
  }
  if (3 < game.difficulty) {
    difficulty_factor++;
  }
  if (4 < game.difficulty) {
    difficulty_factor += 2;
  }
  best_score = map_views_block.score_total;
  if (map_views_block.score_total <= map_views_block.score_conquest_bonus) {
    best_score = map_views_block.score_conquest_bonus;
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
  text_set_surface((iptr)&advisor_window_block.advisor_window);
  text_set_font((iptr)&advisor_font_block.advisor_font);
  text_set_colors(0x25,0x12,2,1);
  text_clear();
  text = (iptr)label_string(label_string_ids[0x19c]);
  text_append((const char *)text);
  text_append_colon();
  text_append_int(rating_percent);
  text_append((const char *)&str_empty_58);
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
  cur_y += line_height;
  text_clear();
  text_append_label(0x19d);
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
  cur_y += line_height;
  text_clear();
  text_append_label(0x19e);
  gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,cur_y,advisor_window_block.advisor_client_width);
  advisor_window_block.advisor_list_top_i32 = cur_y + line_height + 4;
  advisor_window_block.advisor_row_height = line_height;
  advisor_window_block.advisor_list_height_i32 = bottom_y - advisor_window_block.advisor_list_top_i32;
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
      crt_strcpy(line_buf,((i8 *)&map_views_block.text_buffer));
    }
    else if (title_index == i) {
      crt_strcpy(line_buf,str_string0);
    }
    else {
      crt_strcpy(line_buf,str_error_2);
    }
    text_expand_vars((iptr)line_buf,(iptr)&map_views_block.text_buffer);
    if (i < title_index) {
      text_set_font((iptr)&civ_rating_title_font_block.civ_rating_title_font);
      text_set_colors(0x1d,0x12,0xffffffff,0xffffffff);
      line_height = font_line_spacing((GameFont *)((i8 *)&civ_rating_title_font_block.civ_rating_title_font));
      line_height -= 3;
    }
    else {
      text_set_font((iptr)&advisor_font_block.advisor_font);
      line_height = font_line_spacing((GameFont *)((i8 *)&advisor_font_block.advisor_font));
      line_height -= 4;
      text_set_colors(0x25,0x12,0xffffffff,0xffffffff);
    }
    bottom_y -= line_height;
    gfx_draw_text_centered((const char *)&map_views_block.text_buffer,advisor_window_block.advisor_client_x,bottom_y,advisor_window_block.advisor_client_width);
  }
  wnd_blit_surface((DibSurface *)((i8 *)&advisor_window_block.advisor_window));
  return;
}

/* 004361cc auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl civ_rating_window_show(iptr civ)

{
  picwnd_open((PictureWindow *)((i8 *)&advisor_window_block.advisor_window),10,10,1,600,400,0,0);
  advisor_window_block.advisor_report_civ = civ;
  advisor_add_close_button((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
  wnd_set_redraw_callback((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window),civ_rating_window_draw);
  wnd_repaint((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window));
  music_play_track(3,0);
  window_show((SurfaceWindow *)((i8 *)&advisor_window_block.advisor_window));
  if (2 < game.multiplayer) {
    wnd_set_modal_idle_callback((ModalMenuWindow *)((i8 *)&advisor_window_block.advisor_wnd),advisor_mp_idle);
  }
  advisor_modal_start_ticks_i32 = timer_ticks();
  wnd_run_modal_loop((ModalMenuWindow *)((i8 *)&advisor_window_block.advisor_wnd));
  wnd_set_modal_idle_callback((ModalMenuWindow *)((i8 *)&advisor_window_block.advisor_wnd),0);
  picwnd_close((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
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
  if ((total < 0) || (32000 < total)) {
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
  
  if ((game.multiplayer != 1) || ((1 << ((byte)civ & 0x1f) & (uint)net_session_block.hotseat_human_players) == 0)) {
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
  
  has_advance = civ_has_advance(civ,0x18);
  if ((has_advance != 0) && (has_advance = civ_has_advance(civ,5), has_advance != 0)) {
    return 3;
  }
  has_advance = civ_has_advance(civ,0x25);
  if (has_advance == 0) {
    has_advance = civ_has_advance(civ,0x26);
    if ((has_advance != 0) && (has_advance = civ_has_advance(civ,0x3c), has_advance != 0)) {
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
  
  if (((wonder == 0x14) && ((game.game_flags & 0x80) != 0)) && ((prefs_game_rules_block.scenario_flags[0] & 1) != 0)) {
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
  if ((civ == tax_rate_icon_sprites_block.diplo_attitude_civ) && (other_civ == tax_rate_icon_sprites_block.diplo_attitude_target)) {
    tax_rate_icon_sprites_block.diplo_cur_attitude += delta;
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
  
  if (((game.multiplayer != 1) || ((1 << ((byte)civ & 0x1f) & (uint)net_session_block.hotseat_human_players) == 0)) ||
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
    else if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 2) == 0)) {
      power[civ] =
           (uint)civs[civ].num_advances * 3 + civs[civ].total_city_size * 8 +
           (civs[civ].gold >> 5);
      military_power = 0;
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        if ((char)unit_types[unit_type].role < 5) {
          if (unit_types[unit_type].domain == '\0') {
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
    if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 2) == 0)) {
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
  game.game_flags = game.game_flags & 0xfffe;
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
  civ = 0;
  do {
    if (7 < (int)civ) {
check_alert_ai:
      alert_ai = 0;
      civ = (uptr)game.civ_by_rank[7];
      if ((((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) != 0) &&
           (4 < civs[game.civ_by_rank[7]].num_cities)) &&
          (civs[game.civ_by_rank[7]].units_active[0x2d] == '\0')) &&
         ((200 < game.turn && (game.difficulty != '\0')))) {
        alert_ai = 1;
      }
      if (3 < game.difficulty) {
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
        game.game_flags = game.game_flags | 1;
        civ2 = civ;
        ally_count = 0;
        for (civ = 1; (int)civ < 8; civ++) {
          if (((byte)(civs[leader_civ].treaties[civ] >> 8) & 0x20) != 0) {
            ally_count++;
          }
        }
        for (civ = 1; (int)civ < 8; civ++) {
          if (((((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
               ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) &&
              (1 < civs[civ].num_cities)) &&
             (((ally_count * 3 + 3 < (int)(uint)civs[civ2].reputation ||
               ((*(byte *)&civs[civ].treaties[civ2] & 0x10) != 0)) ||
              (result = spaceship_race_active(), result != 0)))) {
            if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
              if ((*(byte *)&civs[civ].treaties[civ2] & 6) == 0) {
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
              if ((result != 0) && (3 < game.difficulty)) {
                treaty_ptr = (uint *)(civ2 * 4 + (iptr)((u8 *)&civs + 32) + civ * 0x594);
                *treaty_ptr = *treaty_ptr | 0x20;
              }
            }
          }
        }
      }
      return;
    }
    if (((uint)game.human_players & 1 << (game.civ_by_rank[civ] & 0x1f)) != 0) {
      game.worst_human_rank = (byte)civ;
      goto check_alert_ai;
    }
    civ++;
  } while( true );
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
  if (((game.game_flags & 0x80) == 0) && (game.multiplayer != 1)) {
    has_palace = false;
    for (city = 0; city < game.num_cities; city++) {
      if (((cities[city].id != 0) && (cities[city].owner == civ)) &&
         (score = city_has_improvement(city,1), score != 0)) {
        has_palace = true;
        break;
      }
    }
    if ((has_palace) && (prefs_game_rules_block.prefs_accelerated_start * 0x14 + 1 < (int)game.turn)) {
      score = civ_calc_score(civ);
      threshold = civ_throne_upgrade_threshold(civs[civ].throne_room_upgrades + 1);
      if ((threshold <= score) &&
         ((civs[civ].throne_room_upgrades = civs[civ].throne_room_upgrades + 1,
          civs[civ].throne_room_upgrades < 0x26 && (((*((char *)&game.game_options + 2)) & 2) != 0)))) {
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
  game.transient_flags = game.transient_flags & 0xfffb;
  city_window_begin_notice((CityWindow *)((i8 *)&city_window_block.city_window));
  city = (iptr)game.num_cities;
  while( true ) {
    city--;
    if (city < 0) break;
    if (2 < game.multiplayer) {
      net_process_messages(&net_session_block,1,0);
    }
    if ((cities[city].id != 0) && ((int)cities[city].owner == (civ & 0xff))) {
      cities[city].flags = cities[city].flags & 0xfffeffff;
      value = city_process_turn(city);
      if (value != -999) {
        if ((value == 0) && (5 < cities[city].size)) {
          city_status = city_status | 1;
        }
        if ((cities[city].flags & 1) != 0) {
          city_status = city_status | 2;
        }
        for (other_civ = 1; other_civ < 8; other_civ++) {
          if (((*(byte *)&civs[civ].treaties[other_civ] & 8) != 0) ||
             (game.wonder_city[0x19] != -1)) {
            *(i8 *)&cities[city].revealed_size[other_civ] = cities[city].size;
          }
        }
        value = citywnd_get_city((CityWindow *)((i8 *)&city_window_block.city_window));
        if ((value == city) && (main_menu_block.city_zoom_requested == 0)) {
          city_window_refresh((CityWindow *)((i8 *)&city_window_block.city_window));
        }
      }
    }
  }
  city_window_end_notice((CityWindow *)((i8 *)&city_window_block.city_window));
  civs[civ].government_score[0] =
       civs[civ].government_score[0] + civs[civ].government_score[1] * 2;
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
    if (1 < civs[civ].government) {
      max_rate = 7;
    }
    if (4 < civs[civ].government) {
      max_rate = 8;
    }
    if (5 < civs[civ].government) {
      max_rate = 10;
    }
    result = 10 - value;
    if (max_rate <= 10 - value) {
      result = max_rate;
    }
    clamped = int_clamp((uint)civs[civ].science_rate,0,result);
    civs[civ].science_rate = (u8)clamped;
    result = civ_has_advance(civ,0x20);
    if ((((result != 0) && (result = civ_has_advance(civ,0x29), result != 0)) &&
        (result = civ_has_advance(civ,(int)improvements[0x23].preq), result != 0)) &&
       ((result = civ_has_advance(civ,(int)improvements[0x24].preq), result != 0 &&
        (result = civ_has_advance(civ,(int)improvements[0x25].preq), result != 0)))) {
      civs[civ].science_rate = '\0';
    }
    civs[civ].tax_rate = '\n' - (civs[civ].science_rate + (char)value);
  }
  tech_bonus = 0;
  value = civ_has_advance(civ,0x15);
  if (((value != 0) || (value = civ_has_wonder(civ,0x13), value != 0)) &&
     (civs[game.best_human_rank].government == 4)) {
    tech_bonus = civs[game.best_human_rank].subversion_count + 1;
  }
  civs[civ].government_score[6] =
       ((ushort)civs[game.best_human_rank].units_active[0x2e] +
        (ushort)civs[game.best_human_rank].units_active[0x2f] + civs[game.best_human_rank].subversion_count) *
       (civs[game.best_human_rank].subversion_count + (short)tech_bonus) + civs[civ].government_score[6];
  if (civs[civ].gold < civs[game.best_human_rank].gold) {
    civs[civ].government_score[6] += 10;
  }
  if ((tech_bonus == 0) &&
     ((((value = civ_has_advance(civ,0xf), value != 0 ||
        (value = civ_has_advance(civ,0x1f), value != 0)) ||
       (value = civ_has_wonder(civ,0x13), value != 0)) &&
      ((attack_trait = leaders[civs[civ].tribe].attack, value = spaceship_race_active(),
       (int)(uint)game.power_rank[civ] <= (int)((int)attack_trait + (4 - (uint)(value == 0))) &&
       (civs[civ].num_advances < civs[game.best_human_rank].num_advances)))))) {
    civs[civ].government_score[6] = 0;
    civs[civ].government_score[5] = 0;
  }
  if (-1 < leaders[civs[civ].tribe].attack) {
    if ((((uint)game.human_players & 1 << (game.civ_by_rank[7] & 0x1f)) == 0) &&
       ((leaders[civs[civ].tribe].attack == 1 ||
        ((leaders[civs[civ].tribe].attack == '\0' &&
         (leaders[civs[civ].tribe].civilize < '\0')))))) {
      value = civs[civ].government_score[6] * 3;
      civs[civ].government_score[6] = (i16)((int)(value + (value >> 0x1f & 3U)) >> 2);
      value = civs[civ].government_score[5] * 3;
      civs[civ].government_score[5] = (i16)((int)(value + (value >> 0x1f & 3U)) >> 2);
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
            civs[civ].government_score[6] = civs[civ].government_score[6] / 2;
            civs[civ].government_score[5] = civs[civ].government_score[5] / 2;
          }
        }
        else {
          civs[civ].government_score[6] = 0;
          civs[civ].government_score[5] = 0;
        }
      }
    }
  }
  value = civ_spaceship_launched(civ);
  if ((value != 0) && (3 < game.difficulty)) {
    civs[civ].government_score[6] = civs[civ].government_score[6] / 2;
    civs[civ].government_score[5] = civs[civ].government_score[5] / 2;
  }
  if ((((game.rules & 0x80) != 0) && (civs[civ].science_rate == '\0')) &&
     (civs[civ].government_score[6] = 0, -1 < leaders[civs[civ].tribe].attack)) {
    civs[civ].government_score[5] = 0;
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
  
  for (unit = 0; unit < game.num_units; unit++) {
    if (2 < game.multiplayer) {
      net_process_messages(&net_session_block,1,0);
    }
    if ((units[unit].id != 0) && (units[unit].owner == civ)) {
      old_flags = units[unit].flags;
      units[unit].flags = units[unit].flags & 0xffaf;
      if ((old_flags & 0x40) == 0) {
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
          if ((advisor_window_block.nearest_city_dist < 4) && (unit_types[units[unit].type].domain == '\0')) {
            has_barracks = city_has_improvement(city,2);
            heal = heal_amount + 1;
            if (has_barracks != 0) {
              heal = heal_amount + 2;
            }
          }
          heal_amount = heal;
          if (advisor_window_block.nearest_city_dist == 0) {
            domain = unit_types[units[unit].type].domain;
            if (domain == '\0') {
              improvement = 2;
            }
            else if (domain == 1) {
              improvement = 0x20;
            }
            else if (domain == 2) {
              improvement = 0x22;
            }
            city = city_has_improvement(city,improvement);
            if ((city != 0) &&
               ((unit_types[units[unit].type].domain == '\0' || (advisor_window_block.nearest_city_dist == 0)))) {
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
          if ((unit_types[units[unit].type].domain == 1) &&
             (city = tile_airbase_owner((int)units[unit].x,(int)units[unit].y), 0 < city))
          {
            heal_amount = (uint)units[unit].damage;
          }
          heal = (uint)units[unit].damage;
          if ((int)heal_amount <= (int)(uint)units[unit].damage) {
            heal = heal_amount;
          }
          units[unit].damage -= (char)heal;
          if (((units[unit].damage == '\0') && (units[unit].orders == 3)) &&
             ((unit_types[units[unit].type].domain != '\0' ||
              (city = tile_is_ocean((int)units[unit].x,(int)units[unit].y), city == 0))))
          {
            units[unit].orders = -1;
          }
          if ((((*(i8 *)&human_civ) == civ) ||
              ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)units[unit].visibility) != 0)) ||
             ((int)units[unit].owner == ((*(i8 *)&human_civ) & 0xffU))) {
            map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
          }
        }
      }
      else if (((unit_types[units[unit].type].domain == 1) &&
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
      ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) && (civ_saved_cursor_x_block.autoplay_active == 0)) {
    adjective = civ_get_adjective(civ);
    text_set_string_arg(1,(const char *)adjective);
    text_clear();
    text_append_population(civ,milestone_pop);
    text_set_string_arg(2,(const char *)&map_views_block.text_buffer);
    popup_game_with_sprite((const char *)&str_fertile,(iptr)(((i8 *)&road_sprites_block.civ_portrait_sprites) + civ * 0x3c),0);
  }
  if ((((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (civ_saved_cursor_x_block.autoplay_active == 0)) &&
       ((game_loop_active != '\0' && ((1 < civs[civ].num_cities && (civs[civ].gold < 100))))))
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
  year_table_start_years_block.civ_turn_in_progress = 1;
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
    result = civ_spaceship_exists(civ);
    if (result != 0) {
      for (part = 0; part < 6; part++) {
        parts_delta += civs[civ].spaceship_parts[part];
      }
    }
  }
  spaceship_wnd_load_parts(civ);
  picwnd_close((PictureWindow *)((i8 *)&advisor_window_block.advisor_window));
  if (30000 < civs[civ].gold) {
    civs[civ].gold = 30000;
  }
  if (civs[civ].gold < -0x4000) {
    civs[civ].gold = 30000;
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
  year_table_start_years_block.civ_turn_in_progress = 0;
  if (((*((char *)&game.game_options + 1)) & 0x20) != 0) {
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    game_autosave(civ);
    __chdir(((i8 *)&civ_saved_cursor_x_block.game_home_dir));
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
                units[unit].orders = 11;
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
    name2 = (iptr)label_string(label_string_ids[0x11]);
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
    name2 = (iptr)label_string(label_string_ids[0xf]);
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
    adjective = (iptr)label_string(label_string_ids[0x10]);
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
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dialog2,0x4000);
  seh_state = 0;
  mp_turn_timer_stop(1);
  passwords_unscramble();
  crt_strcpy(current_password,((i8 *)&civ_saved_cursor_x_block.civ_passwords) + civ * 0x20);
  passwords_scramble();
  if (current_password[0] == '\0') {
    dialog_load_text_section(&dialog2,(const char *)&str_game_7,(const char *)&str_password1_3,0x1f,(iptr)&str_empty_112,0,0,0,1);
    password_dialog_set_title(&dialog2,civ,0x26c);
    result = dialog_run_modal(&dialog2,(iptr)entered,0);
  }
  else {
    dialog_load_text_section(&dialog2,(const char *)&str_game_7,(const char *)&str_password1,0x1f,(iptr)&str_empty_110,0,0,0,1);
    password_dialog_set_title(&dialog2,civ,0x26d);
    result = dialog_run_modal(&dialog2,(iptr)entered,0);
    if (result < 0) goto cleanup;
    result = _strcmp(entered,current_password);
    if (result != 0) {
      popup_game((const char *)&str_password0);
      goto cleanup;
    }
    dialog_load_text_section(&dialog2,(const char *)&str_game_7,(const char *)&str_password1_2,0x1f,(iptr)&str_empty_111,0,0,0,1);
    password_dialog_set_title(&dialog2,civ,0x26c);
    result = dialog_run_modal(&dialog2,(iptr)entered,0);
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
cleanup:
  passwords_refresh_flags();
  map_views_block.civ_password_prompt_pending[civ] = 0;
  mp_turn_timer_resume(1);
  seh_state = 0xffffffff;
  /* inlined unwind funclet 004985d0 */ { dialog_dtor(&dialog2); }
  return;
}

/* 004985f4 auto */
i32 __cdecl civ_password_prompt(int civ)

{
  i32 status;
  char entered[32];
  Dialog dialog2;
  int result;
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dialog2,0x4000);
  seh_state = 0;
  dialog_load_text_section(&dialog2,(const char *)&str_game_7,(const char *)&str_password1_4,0x1f,(iptr)&str_empty_113,0,0,0,1);
  password_dialog_set_title(&dialog2,civ,0x26b);
  entered[0] = '\0';
  dlg_pending_idle_callback = net_pump_idle_cb;
  result = dialog_run_modal(&dialog2,(iptr)entered,0);
  if (result < 0) {
    seh_state = 0xffffffff;
    /* inlined unwind funclet 0049875f */ { dialog_dtor(&dialog2); }
    status = 2;
  }
  else {
    passwords_unscramble();
    result = _strcmp(entered,((i8 *)&civ_saved_cursor_x_block.civ_passwords) + civ * 0x20);
    passwords_scramble();
    if (result == 0) {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0049875f */ { dialog_dtor(&dialog2); }
      status = 0;
    }
    else {
      dlg_pending_idle_callback = net_pump_idle_cb;
      popup_game((const char *)&str_password0_2);
      seh_state = 0xffffffff;
      /* inlined unwind funclet 0049875f */ { dialog_dtor(&dialog2); }
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
      game_loop_active = 1;
    }
  }
  else {
    do {
      game_loop_active = 1;
      prompt_result = civ_password_prompt(civ);
      if (prompt_result == 0) {
        map_views_block.civ_password_prompt_pending[civ] = 0;
        return 1;
      }
    } while (prompt_result == 1);
    if (prompt_result == 2) {
      game_loop_active = 0;
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
  
  map_views_block.score_citizens = 0;
  for (city = 0; city < game.num_cities; city++) {
    if ((cities[city].id != 0) && (cities[city].owner == civ)) {
      map_views_block.score_citizens = map_views_block.score_citizens +
                     (((int)cities[city].size + (int)(char)cities[city].happy) -
                     (int)(char)cities[city].unhappy);
    }
  }
  map_views_block.score_wonders = 0;
  for (wonder = 0; wonder < 0x1c; wonder++) {
    if ((-1 < game.wonder_city[wonder]) && (cities[game.wonder_city[wonder]].owner == civ)) {
      map_views_block.score_wonders += 0x14;
    }
  }
  map_views_block.score_spaceship_in_flight = 0;
  map_views_block.score_spaceship_arrived = 0;
  spaceship_calc_stats(civ,1);
  if (((unit_sound_editor_window_block.spaceship_success_pct != 0) && (civs[civ].spaceship_population != 0)) &&
     (value = civs[civ].spaceship_population * unit_sound_editor_window_block.spaceship_success_pct, map_views_block.score_spaceship_in_flight = value,
     (civs[civ].spaceship_flags & 0x10) != 0)) {
    map_views_block.score_spaceship_in_flight = 0;
    map_views_block.score_spaceship_arrived = value;
  }
  map_views_block.score_pollution = ((int)game.polluted_tiles - (int)game.warming_pollution) * -10;
  map_views_block.score_peace = 0;
  if (199 < game.turn) {
    map_views_block.score_peace = int_clamp(game.peace_turns * 3,0,100);
  }
  map_views_block.score_future_techs = (uint)civs[civ].future_techs * 5;
  map_views_block.score_barbarian_bonus = (uint)game.barbarian_activity * 0x19 - 0x32;
  map_views_block.score_total = map_views_block.score_peace + map_views_block.score_pollution + map_views_block.score_spaceship_arrived + map_views_block.score_wonders + map_views_block.score_citizens +
                 map_views_block.score_barbarian_bonus + map_views_block.score_future_techs;
  if (map_views_block.score_total < 0) {
    map_views_block.score_total = 0;
  }
  map_views_block.score_conquest_bonus = 0;
  if ((game.civs_alive & 0xfffffffe) == 1 << ((byte)civ & 0x1f)) {
    end_turn = (int)game.turn;
    if (((game.rules & 0x100) != 0) && (end_turn < 0xfb)) {
      end_turn = 0xfa;
    }
    map_views_block.score_conquest_bonus = (uint)game.num_enemies * 100 + (0x23a - end_turn) * 2 + 400;
    if ((game.game_flags & 4) != 0) {
      map_views_block.score_conquest_bonus = (int)(map_views_block.score_conquest_bonus * 5 + (map_views_block.score_conquest_bonus * 5 >> 0x1f & 3U)) >> 2;
    }
    if ((game.game_flags & 8) != 0) {
      map_views_block.score_conquest_bonus = (map_views_block.score_conquest_bonus << 2) / 5;
    }
  }
  map_views_block.score_scenario_points = 0;
  if (((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 2) == 0)) {
    value = map_views_block.score_total;
    if (map_views_block.score_total <= map_views_block.score_conquest_bonus) {
      value = map_views_block.score_conquest_bonus;
    }
  }
  else {
    map_views_block.score_total_objective_value = 0;
    map_views_block.score_objective_civ_value = 0;
    map_views_block.score_civ_objective_value = 0;
    for (city = 0; city < game.num_cities; city++) {
      if (cities[city].id != 0) {
        value = city_objective_value(city);
        map_views_block.score_total_objective_value += value;
        if ((int)cities[city].owner == (int)prefs_game_rules_block.scenario_objective_civ) {
          map_views_block.score_objective_civ_value += value;
        }
        if (cities[city].owner == civ) {
          map_views_block.score_civ_objective_value += value;
        }
      }
    }
    if (prefs_game_rules_block.scenario_objective_civ == 0) {
      map_views_block.score_objective_civ_value = map_views_block.score_civ_objective_value;
    }
    map_views_block.score_objective_outcome = 2;
    if (map_views_block.score_objective_civ_value < prefs_game_rules_block.scenario_victory_thresholds[0]) {
      if (map_views_block.score_objective_civ_value < prefs_game_rules_block.scenario_marginal_victory_score) {
        if (prefs_game_rules_block.scenario_decisive_defeat_score < map_views_block.score_objective_civ_value) {
          if (map_views_block.score_objective_civ_value <= prefs_game_rules_block.scenario_marginal_defeat_score) {
            map_views_block.score_objective_outcome = 3;
          }
        }
        else {
          map_views_block.score_objective_outcome = 4;
        }
      }
      else {
        map_views_block.score_objective_outcome = 1;
      }
    }
    else {
      map_views_block.score_objective_outcome = 0;
    }
    outcome = map_views_block.score_objective_outcome;
    if ((prefs_game_rules_block.scenario_objective_civ != 0) && (prefs_game_rules_block.scenario_objective_civ != civ)) {
      outcome = 4 - map_views_block.score_objective_outcome;
    }
    switch(outcome) {
    case 0:
      map_views_block.score_scenario_points = 1000;
      break;
    case 1:
      map_views_block.score_scenario_points = 0x2ee;
      break;
    case 2:
      map_views_block.score_scenario_points = 500;
      break;
    case 3:
      map_views_block.score_scenario_points = 0xfa;
      break;
    case 4:
      map_views_block.score_scenario_points = 0;
    }
    map_views_block.score_scenario_points += map_views_block.score_civ_objective_value * 10;
    if (0 < map_views_block.score_conquest_bonus) {
      map_views_block.score_scenario_points = map_views_block.score_scenario_points * 2;
    }
    map_views_block.score_conquest_bonus = -1;
    map_views_block.score_total = map_views_block.score_scenario_points;
    value = map_views_block.score_scenario_points;
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
  return civs[civ].spaceship_flags & 2;
}

/* 004a75a6 auto */
uint __cdecl civ_spaceship_exists(iptr civ)

{
  return civs[civ].spaceship_flags & 1;
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

/* 004a7ce9 auto */
int __cdecl civ_create(iptr civ)

{
  short color;
  byte color_bit;
  i16 last_city_turn;
  iptr now_ticks;
  iptr rand_value;
  int result;
  Tile *map_tile;
  i32 attitude;
  i32 max_attitude;
  int rand_y;
  int rand_x;
  iptr pick;
  int grant_roll;
  iptr candidates[21];
  iptr x3;
  int no_start_units;
  iptr unit;
  int landmass_fertility;
  int attempts;
  uint clear_mask;
  iptr landmass2;
  uint y2;
  int continent_city_counts[64];
  uint x2;
  int unit_dist;
  int pass;
  int old_tribe;
  int leader_color;
  uint was_alive;
  iptr leader;
  iptr y3;
  iptr leader2;
  int fertility;
  int nearest_city;
  int alive_count;
  iptr candidate_count;
  int start_ticks;
  
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) || (game.turn == 0)) {
    net_session_block.civ_create_busy = 1;
    if ((game.multiplayer < 3) || (net_session_block.net_host_flag != '\0')) {
      old_tribe = (int)civs[civ].tribe;
      civs[civ].gold = 0;
      civs[civ].research_progress = 0;
      civs[civ].num_advances = '\0';
      civs[civ].research_cost_advances = '\0';
      last_city_turn = game.turn;
      if (game.turn < 0xb) {
        last_city_turn = 10;
      }
      civs[civ].last_city_turn = last_city_turn;
      civs[civ].government = 1;
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
          rand_value = _rand();
          *(char *)&civs[leader].attitude[civ] = (char)(rand_value % 0x50) + '\n';
          *(i8 *)&civs[civ].attitude[leader] =
               *(i8 *)&civs[leader].attitude[civ];
        }
        else {
          max_attitude = 0x4b;
          attitude = 10;
          result = _rand();
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
        was_alive = (uint)game.civs_alive & 1 << ((byte)civ & 0x1f);
        game.civs_alive = game.civs_alive & ~(byte)(1 << ((byte)civ & 0x1f));
        for (leader2 = 0; leader2 < 99; leader2++) {
          if (((((uint)game.human_players & 1 << (game.first_discoverer[leader2] & 0x1f)) == 0) ||
              (result = bits_count8((int)(char)game.advance_known_by[leader2]), 2 < result))
             && ((game.human_players & game.advance_known_by[leader2]) != 0)) {
            if (game.difficulty == '\0') {
              grant_roll = 0;
            }
            else {
              grant_roll = _rand();
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
      for (leader2 = 0; leader2 < 0x3e; leader2++) {
        *(i8 *)&civs[civ].units_active[leader2] = 0;
        *(i8 *)&civs[civ].units_in_production[leader2] = 0;
        *(i8 *)&civs[civ].units_lost[leader2] = 0;
      }
      if (civ == 0) {
        net_session_block.civ_create_busy = 0;
        result = 1;
      }
      else {
        no_start_units = 0;
        if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
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
            net_session_block.civ_create_busy = 0;
            return 1;
          }
          if (candidate_count == 1 || candidate_count - 1 < 0) {
            pick = 0;
          }
          else {
            result = _rand();
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
            no_start_units = _rand();
            no_start_units = no_start_units % (leaders[leader].times_used + 1);
          }
          civs[civ].tribe = (i16)leader;
          leaders[leader].times_used++;
          leaders[leader].city_name_index = '\0';
          result = _rand();
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
          civs[civ].flags = civs[civ].flags & 0xfdff;
          if (leaders[leader].female != '\0') {
            civs[civ].leader_portrait += 21;
            civs[civ].flags = civs[civ].flags | 0x200;
          }
        }
        if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
          net_session_block.civ_create_busy = 0;
          result = 1;
        }
        else {
          attempts = 0;
          do {
            if ((((game.rules & 0x8000) == 0) || (map.width < 0x51)) || (0x5db < attempts)) {
              if (map.width == 1 || map.width - 1 < 0) {
                x2 = 0;
              }
              else {
                result = _rand();
                x2 = result % (int)map.width;
              }
            }
            else {
              if (map.width == 0x11 || map.width - 0x11 < 0) {
                rand_x = 0;
              }
              else {
                rand_x = _rand();
                rand_x = rand_x % (map.width - 0x10);
              }
              x2 = rand_x + 8;
            }
            if ((((game.rules & 0x8000) == 0) || (map.height < 0x28)) || (0x5db < attempts)) {
              if (map.height == 1 || map.height - 1 < 0) {
                y2 = 0;
              }
              else {
                result = _rand();
                y2 = result % (int)map.height;
              }
            }
            else {
              if (map.height == 0x11 || map.height - 0x11 < 0) {
                rand_y = 0;
              }
              else {
                rand_y = _rand();
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
          if ((game.turn == 0) &&
             (result = map_in_bounds((int)start_x[civs[civ].tribe],
                                    (int)start_y[civs[civ].tribe]), result != 0)) {
            x2 = (uint)start_x[civs[civ].tribe];
            y2 = (uint)start_y[civs[civ].tribe];
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
              (((game.turn == 0 || ((game.rules & 0x100) == 0)) &&
               ((int)game.turn <= (int)((uint)game.difficulty * -0x23 + 0x15e))))) ||
             ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
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
            unit = unit_create(0,civ,x2,y2);
            civs[civ].capital_x = (i16)x2;
            if (((0x14 < game.turn) && (no_start_units == 0)) &&
               ((map_tile = tile_at(x2,y2),
                (map_tile->visible & game.human_players) == 0 &&
                ((unit_create(0,civ,x2,y2), 0x28 < game.turn &&
                 (result = civ_can_build_unit(civ,-1,3), result != 0)))))) {
              unit_create(3,civ,x2,y2);
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
            net_session_block.civ_create_busy = 0;
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
              if ((game.game_flags & 2) == 0) {
                game_over_reason = 3;
                game.game_flags = game.game_flags | 2;
              }
              net_session_block.civ_create_busy = 0;
              result = -1;
            }
            else {
              net_session_block.civ_create_busy = 0;
              result = 0;
            }
          }
        }
      }
    }
    else {
      net_session_block.civ_create_busy = 0;
      net_session_block.nm_new_civ_ack_result = -0x1b;
      net_send_message(0x5f,0,civ,0,0,0,0,0,0,0);
      start_ticks = timer_ticks();
      while ((net_session_block.nm_new_civ_ack_result == -0x1b && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
        net_process_messages(&net_session_block,1,1);
      }
      if (net_session_block.nm_new_civ_ack_result == -0x1b) {
        dbg_log((const char *)&str_new_civ_connection_to_server_timed_out);
        popup_game_with_sprite((const char *)&str_serverconnecttime_3,(iptr)&popup_info_icon_sprite,0);
        game_loop_active = 0;
      }
      else {
        dbg_log((const char *)&str_new_civ_received_nm_new_civ_ack_from_ser);
      }
      while ((net_session_block.net_msgq_priority_count != 0 || (result = net_session_block.nm_new_civ_ack_result, net_session_block.net_msgq_normal_count != 0))) {
        net_process_messages(&net_session_block,1,0);
      }
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
    net_session_block.civ_check_destroyed_busy = 1;
    if ((game.multiplayer < 3) || (net_session_block.net_host_flag != '\0')) {
      for (city = 0; city < game.num_cities; city++) {
        if ((cities[city].id != 0) && (cities[city].owner == civ)) {
          net_session_block.civ_check_destroyed_busy = 0;
          return 0;
        }
      }
      if (civ2 == -1) {
        civ_reset_spaceship(civ);
      }
      else {
        if (civ_saved_cursor_x_block.autoplay_active == 0) {
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
        if (destroyed_civ_count_block.destroyed_civ_count[0] < 0xc) {
          result = (int)destroyed_civ_count_block.destroyed_civ_count[0];
          destroyed_civ_count_block.destroyed_civ_count[0]++;
          *(i16 *)(((i8 *)&destroyed_civ_count_block.destroyed_civ_turns) + result * 2) = game.turn;
          (((i8 *)&destroyed_civ_count_block.destroyed_civ_killers))[result] = (i8)civ2;
          (((i8 *)&destroyed_civ_count_block.destroyed_civ_portraits))[result] = civs[civ].leader_portrait;
          plural_name = (i32)(civ_name_plural(civ));
          crt_strcpy(((i8 *)&destroyed_civ_count_block.destroyed_civ_names) + result * 0x18,(const char *)plural_name);
        }
        civ_spaceship_lost_notify(civ);
      }
      unit = (iptr)game.num_units;
      while (unit = unit - 1, -1 < unit) {
        if (units[unit].id != 0) {
          if (((units[unit].owner == civ) || ((units[unit].flags & 2) == 0)) ||
             ((char)units[unit].counter != civ)) {
            if (units[unit].owner == civ) {
              unit_delete(unit,1);
            }
          }
          else {
            units[unit].flags = units[unit].flags & 0xfffd;
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
          net_send_message(0x6b,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,
                             (uint)game.game_flags,4,0,0,0,0,0,0);
        }
        passwords_unscramble();
        (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[civ * 0x20] = 0;
        passwords_scramble();
        if ((~(1 << ((byte)civ & 0x1f)) & (uint)game.human_players) == 0) {
          net_session_block.civ_check_destroyed_busy = 0;
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
        net_send_message(0x31,0,0,civ,0,0,0,0,0,0);
      }
      if (game.multiplayer == 1) {
        game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
      }
      civ_create(civ);
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
        net_send_message(0x74,0xff,0,0,0,0,0,0,0,0);
      }
      if ((game.multiplayer != 1) ||
         ((1 << ((byte)(*(i8 *)&human_civ) & 0x1f) & (uint)game.civs_alive) != 0)) {
        map_redraw_all_views((*(i8 *)&human_civ),1);
      }
      net_session_block.civ_check_destroyed_busy = 0;
      result = 1;
    }
    else {
      net_session_block.civ_check_destroyed_busy = 0;
      net_session_block.nm_kill_civ_ack_result = -0x1b;
      net_send_message(0x60,0,civ,civ2,0,0,0,0,0,0);
      start_ticks = timer_ticks();
      while ((net_session_block.nm_kill_civ_ack_result == -0x1b && (now_ticks = timer_ticks(), now_ticks - start_ticks < 0xe10))) {
        net_process_messages(&net_session_block,1,1);
      }
      if (net_session_block.nm_kill_civ_ack_result == -0x1b) {
        dbg_log((const char *)&str_kill_civ_connection_to_server_timed_out);
        popup_game_with_sprite((const char *)&str_serverconnecttime_4,(iptr)&popup_info_icon_sprite,0);
        game_loop_active = 0;
      }
      else {
        dbg_log((const char *)&str_kill_civ_received_nm_new_civ_ack_from_se);
      }
      while ((net_session_block.net_msgq_priority_count != 0 || (result = net_session_block.nm_kill_civ_ack_result, net_session_block.net_msgq_normal_count != 0))) {
        net_process_messages(&net_session_block,1,0);
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
  else if (advance_id == 0x59) {
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

/* 004bdaa5 auto */
i32 __cdecl advance_has_prereq(iptr advance,iptr prereq)

{
  i32 result;
  int found;
  
  if (advance < 0) {
    result = 0;
  }
  else if (advance == prereq) {
    result = 1;
  }
  else {
    found = advance_has_prereq((int)advances[advance].preq[0],prereq);
    if ((found == 0) &&
       (found = advance_has_prereq((int)advances[advance].preq[1],prereq), found == 0)) {
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
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
  
  result = civ_has_wonder(civ,0xe);
  if (result != 0) {
    for (unit_type2 = 0; unit_type2 < 0x3e; unit_type2++) {
      notified[unit_type2] = '\0';
    }
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) && (units[unit].owner == civ)) {
        unit_type = units[unit].type;
        upgrade_advance = (int)unit_types[unit_type].obsolete_by;
        if ((unit_types[unit_type].role == 1) &&
           (((char)unit_types[unit_type].defense < (char)unit_types[7].defense &&
            (result = civ_has_advance(civ,0x23), result != 0)))) {
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
              text_set_label_arg(2,improvements[0x35].name);
              if ((game.multiplayer < 3) || (human_civ[0] == civ)) {
                dlg_game_txt_with_unit_flags((const char *)&str_upgrade,unit_type3,(ui_large_screen == 0) - 1 & 8);
              }
              else {
                net_send_text_args(0x3e,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,3,0,unit_type3,
                                   ui_large_screen);
              }
            }
            units[unit].type = (u8)unit_type3;
            units[unit].flags = units[unit].flags & 0xdfff;
            map_redraw_tile_all_views((int)units[unit].x,(int)units[unit].y);
            if (2 < game.multiplayer) {
              net_send_state_diff(0xff,2,0,0,0);
              net_send_message(0x72,0xff,(int)units[unit].x,(int)units[unit].y,0,0,0,0,0,0
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
  
  if (advance == 0x36) {
    if ((civs[civ].government == 1) &&
       (choice = popup_game_with_sprite((const char *)&str_automonarchy,(iptr)&government_sprites_block.government_monarchy_sprite,0), choice == 1)) {
      civ_plural = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)civ_plural);
      popup_game_with_sprite((const char *)&str_startrev_2,(iptr)&popup_info_icon_sprite,0);
      civ_set_government(civ,0);
    }
  }
  else if ((((advance == 0x47) || (advance == 0x15)) || (advance == 0xf)) || (advance == 0x1f)) {
    if (((((*((char *)&game.game_options + 1)) & 1) != 0) && (advance == 0x47)) &&
       (civs[civ].government < 3)) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_republic,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
      return;
    }
    if (advance == 0xf) {
      gov_sprite_index = 3;
    }
    else if (advance == 0x1f) {
      gov_sprite_index = 4;
    }
    else if (advance == 0x47) {
      gov_sprite_index = 5;
    }
    else {
      gov_sprite_index = 6;
    }
    if ((advance == 0x47) && (civs[civ].government == 5)) {
      return;
    }
    if ((advance == 0xf) && (civs[civ].government == 3)) {
      return;
    }
    if ((advance == 0x1f) && (civs[civ].government == 4)) {
      return;
    }
    if ((civs[civ].government != 6) && (civs[civ].government != '\0')) {
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
  if (((*((char *)&game.game_options + 1)) & 1) != 0) {
    if (advance == 0x43) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_railroads,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == 0x46) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_farmland,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == 0x54) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_trade,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == 0x4b) {
      popup_file_with_sprite((const char *)(*(i8 * *)str_ptr_labels_rest),(const char *)&str_seafaring,(iptr)&tutorial_icon_sprite_block.tutorial_icon_sprite,0);
    }
    if (advance == 0x58) {
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
      has_temple = city_has_improvement(city,1);
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
    popup_game_with_advance((const char *)&str_goldenage,0x3c,8);
  }
  return;
}

/* 004bf05b auto */
void __cdecl civ_give_advance(uptr civ,iptr advance,iptr civ2,iptr skip_pedia,int from_network)

{
  iptr has_wonder_a;
  iptr has_wonder_b;
  iptr civ_plural;
  int improvement2;
  uptr civ3;
  iptr improvement;
  iptr advance3;
  iptr city;
  int list_count;
  byte bit_mask[4];
  iptr unit_type;
  Dialog dialog2;
  iptr advance2;
  iptr wonder;
  int civ4;
  int first_discovery;
  int count;
  i32 line_width;
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  first_discovery = 0;
  dialog_ctor(&dialog2,0x4000);
  seh_state = 0;
  if ((((game.multiplayer < 3) || (from_network != 0)) ||
      ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) ||
     (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 || ((*(i8 *)&human_civ) == civ)))
     ) {
    if (civs[civ].researching == advance) {
      civs[civ].researching = -1;
    }
    if ((advance == 0x59) || (99 < advance)) {
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
      seh_state = 0xffffffff;
      /* inlined unwind funclet 004bfd9a */ { dialog_dtor(&dialog2); }
    }
    else {
      civs[civ].num_advances++;
      if ((civ_saved_cursor_x_block.autoplay_active == 0) &&
         (((((*(i8 *)&human_civ) == civ && (civ2 < 0)) ||
           (((*(i8 *)&human_civ) == civ &&
            ((civ == civ2 && (((*((char *)&game.game_options + 2)) & 8) == 0)))))) ||
          (((*(i8 *)&human_civ) != civ &&
           ((((((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & 0x80) != 0 ||
               (has_wonder_a = civ_has_wonder((*(i8 *)&human_civ),0x18), has_wonder_a != 0)) ||
              (has_wonder_b = civ_has_wonder((*(i8 *)&human_civ),9), has_wonder_b != 0)) ||
             (((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & 8) != 0 ||
              (game.reveal_map != '\0')))) &&
            ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)))))))) {
        dialog_init_default(&dialog2);
        dialog_set_width(&dialog2,0x140);
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
            text_append_string_id(improvements[0x2b].name);
          }
          else {
            civ_plural = (iptr)(civ_name_plural(civ2));
            text_append((const char *)civ_plural);
          }
        }
        text_append((const char *)&str_empty_119);
        dialog_add_line
                  (&dialog2,(const char *)&map_views_block.text_buffer,0xffffffff,0xffffffff,0xffffffff,0);
        if (diplo_session_active == 0) {
          dialog_add_sprite
                    (&dialog2,
                     (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                           (char)advances[advance].epoch * 0x3c +
                           (char)advances[advance].category * 0xf0),0,0);
          dialog_set_image_zoom(&dialog2,8);
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
            dialog_add_line
                      (&dialog2,(const char *)&map_views_block.text_buffer,0xffffffff,0xffffffff,0xffffffff,0);
          }
        }
        dlg_show_modal(&dialog2,0);
      }
      else if (((civ_saved_cursor_x_block.autoplay_active == 0) &&
               ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) &&
              (((first_discovery != 0 &&
                (((improvement2 = rand_range(0,2), improvement2 == 0 &&
                  ((*(byte *)&civs[civ].treaties[(*(i8 *)&human_civ)] & 1) != 0)) &&
                 ((*(byte *)&civs[civ].treaties[(*(i8 *)&human_civ)] & 8) == 0)))) &&
               ((((*(byte *)&civs[(*(i8 *)&human_civ)].treaties[civ] & 0x80) == 0 &&
                 (improvement2 = civ_has_wonder((*(i8 *)&human_civ),0x18), improvement2 == 0)) &&
                (improvement2 = civ_has_wonder((*(i8 *)&human_civ),9), improvement2 == 0)))))) {
        improvement2 = diplo_ask_receive_emissary((*(i8 *)&human_civ),civ,0xffffffff,0xffffffff);
        if (improvement2 != 0) {
          civ_plural = (iptr)(civ_name_plural(civ));
          text_set_string_arg(1,(const char *)civ_plural);
          text_set_label_arg(2,advances[advance].name);
          popup_game((const char *)&str_gape);
        }
        diplo_session_reset();
      }
      if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
           ((*(i8 *)&human_civ) == civ)) && (civ2 != 0)) && (civ_saved_cursor_x_block.autoplay_active == 0)) {
        if (((diplo_session_active == 0) && (((*((char *)&game.game_options + 2)) & 8) != 0)) && (skip_pedia == 0)) {
          pedia_show_topic_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),advance);
        }
        civ_advance_gov_prompts(civ,advance);
      }
      for (advance2 = 0x35; (-1 < advance2 && (advances[advance2].enabled == '\0'));
          advance2 = (iptr)advances[advance2].preq[0]) {
      }
      if ((advance == 0x23) || (advance2 == advance)) {
        count = 0;
        for (city = 0; city < game.num_cities; city++) {
          if ((cities[city].id != 0) && ((int)cities[city].owner == (civ & 0xff))) {
            improvement2 = city_has_improvement(city,2);
            if (improvement2 != 0) {
              count++;
            }
            city_set_improvement(city,2,0);
          }
        }
        if (count != 0) {
          count = (uint)improvements[2].cost * (uint)cosmic.shield_box_rows * count;
          if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
            text_set_label_arg(0,advances[advance].name);
            text_set_number_arg(0,count);
            dialog_load_game_text(&dialog2,(const char *)&str_barracks);
            if (diplo_session_active == 0) {
              dialog_add_sprite
                        (&dialog2,
                         (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                               (char)advances[advance].epoch * 0x3c +
                               (char)advances[advance].category * 0xf0),0,0);
              dialog_add_sprite(&dialog2,(iptr)&caption_close_button_sprite_block.notify_sprites,0,0);
              dialog_set_image_zoom(&dialog2,8);
            }
            dlg_show_modal(&dialog2,0);
          }
          civs[civ].gold += count;
          if ((*(i8 *)&human_civ) == civ) {
            status_draw_info(1);
          }
        }
      }
      if (first_discovery != 0) {
        if (((advance == 0x3c) &&
            (civs[civ].flags = civs[civ].flags | 0x20, civ_saved_cursor_x_block.autoplay_active == 0)) &&
           (((*(i8 *)&human_civ) != civ || (civ2 != 0)))) {
          civ_golden_age_popup(civ);
        }
        game.first_discoverer[advance] = game.first_discoverer[advance] | (byte)civ;
        if (civ_saved_cursor_x_block.autoplay_active == 0) {
          for (wonder = 0; wonder < 0x1c; wonder++) {
            if (((wonder_expires[wonder] == advance) &&
                (city = (iptr)game.wonder_city[wonder], -1 < city)) &&
               (((uint)game.human_players & 1 << (cities[city].owner & 0x1fU)) != 0)) {
              improvement2 = wonder + 0x27;
              text_set_label_arg(0,advances[advance].name);
              text_set_label_arg(1,improvements[improvement2].name);
              dialog_load_game_text(&dialog2,(const char *)&str_endwonder_2);
              if (diplo_session_active == 0) {
                dialog_add_sprite
                          (&dialog2,
                           (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                                 (char)advances[advance].epoch * 0x3c +
                                 (char)advances[advance].category * 0xf0),0,0);
                dialog_add_sprite
                          (&dialog2,(iptr)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement2 * 0x3c),0,0);
                dialog_set_image_zoom(&dialog2,8);
              }
              if (2 < game.multiplayer) {
                net_send_text_args(0x4d,0xff,2,0,advance,improvement2);
              }
              dlg_show_modal(&dialog2,0);
            }
          }
        }
      }
      civ_leonardo_upgrade(civ);
      civ3 = wonder_get_owner(4);
      if ((0 < (int)civ3) && (improvement2 = civ_has_advance(civ3,advance), improvement2 == 0)) {
        count = 0;
        for (civ4 = 1; civ4 < 8; civ4++) {
          improvement2 = civ_has_advance(civ4,advance);
          if (improvement2 != 0) {
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
      seh_state = 0xffffffff;
      /* inlined unwind funclet 004bfd9a */ { dialog_dtor(&dialog2); }
    }
  }
  else {
    net_send_message(0x9b,net_session_block.net_slots[net_session_block.net_civ_slots[civ]].id,civ,
                       advance,civ2,skip_pedia,0,0,0,0);
    seh_state = 0xffffffff;
    /* inlined unwind funclet 004bfd9a */ { dialog_dtor(&dialog2); }
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
       ((((unit_types[unit_type].role != 1 ||
          ((char)unit_types[7].defense <= (char)unit_types[unit_type].defense)) ||
         (known = civ_has_advance(civ,0x23), known == 0)) &&
        (((unit_types[unit_type].role != '\0' ||
          ((int)(char)unit_types[unit_type].move != (uint)cosmic.road_move_mult * 2)) ||
         (((char)unit_types[0x13].attack < (char)unit_types[unit_type].attack ||
          (((char)unit_types[0x13].defense <= (char)unit_types[unit_type].defense ||
           (known = civ_has_advance(civ,(int)unit_types[0x13].prerequisite), known == 0))))))
        )))))) &&
     ((((char)unit_types[unit_type].attack < 'c' ||
       ((game.wonder_city[0x17] != -1 && (known = civ_has_advance(civ,0x3a), known != 0))))
      && (((city < 0 || (unit_types[unit_type].domain != 2)) ||
          (((cities[city].flags & 0x200000) != 0 && ((cities[city].flags & 0x80) != 0))))))))
  {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
       (((int)(uint)cosmic.road_move_mult < (int)(char)unit_types[unit_type].move &&
        (unit_types[unit_type].domain == '\0')))) {
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
    if ((unit_type != 8) || (civs[civ].government == 4)) {
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
  if ((((((game.game_flags & 0x80) == 0) || ((prefs_game_rules_block.scenario_flags[0] & 4) == 0)) || (game.reveal_map != '\0'))
      || ((((int)(short)prefs_game_rules_block.scenario_flags[0] & 0x8000U) != 0 && (wonder == 0x17)))) &&
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
           (((candidates == 0 || (game.difficulty == '\0')) ||
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
  sprite_draw
            ((Sprite *)
             (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
             (char)advances[advance].category * 0xf0 + (char)advances[advance].epoch * 0x3c),
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
  sprite_draw
            ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c),(iptr)draw_ctx,surface,draw_x,
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
  sprite_draw
            ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + improvement * 0x3c),(iptr)draw_ctx,surface,draw_x,
             draw_y);
  return 0;
}

/* 004c0cf7 auto */
void __cdecl civ_research_goal_dialog(iptr civ,int exclude_current,int civ2)

{
  iptr label;
  int known;
  iptr option_id;
  i32 option_flags;
  iptr path_advances[99];
  int mode_state[4];
  iptr choice;
  iptr improvement;
  iptr advance;
  int option_count2;
  iptr path_count;
  iptr button_count;
  iptr unit_type;
  Dialog dialog2;
  iptr i;
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dialog2,0x2000);
  seh_state = 0;
  mode_state[1] = 0;
  while( true ) {
    while( true ) {
      option_count2 = 0;
      dlg_load_game_section(&dialog2,(const char *)&str_researchgoal,1);
      if (mode_state[1] == 0) {
        dialog_set_columns(&dialog2,2);
        if (diplo_session_active == 0) {
          dialog_set_list_indent(&dialog2,0x4b,0);
          dialog_set_option_draw_fn(&dialog2,(DialogOptionDrawFn)research_option_draw_advance);
        }
        for (advance = 0; advance < 100; advance++) {
          if (((advances[advance].enabled != '\0') &&
              (known = civ_has_advance(civ,advance), known == 0)) &&
             ((exclude_current == 0 ||
              ((civs[civ].researching != advance &&
               ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))))))) {
            text_clear();
            text_append_string_id(advances[advance].name);
            if (advance == 0x59) {
              text_append_space();
              text_append_int(civs[civ].future_techs + 1);
            }
            dialog_add_option(&dialog2,(const char *)&map_views_block.text_buffer,advance,0);
            option_count2++;
          }
        }
      }
      else if (mode_state[1] == 1) {
        if (diplo_session_active == 0) {
          dialog_set_list_indent(&dialog2,0x4b,0);
          dialog_set_option_draw_fn(&dialog2,(DialogOptionDrawFn)research_option_draw_unit);
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
            dialog_add_option(&dialog2,(const char *)&map_views_block.text_buffer,unit_type,0);
            option_count2++;
          }
        }
      }
      else if (mode_state[1] == 2) {
        if (diplo_session_active == 0) {
          dialog_set_list_indent(&dialog2,0x4b,0);
          dialog_set_option_draw_fn(&dialog2,(DialogOptionDrawFn)research_option_draw_improvement);
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
            dialog_add_option(&dialog2,(const char *)&map_views_block.text_buffer,improvement,0);
            option_count2++;
          }
        }
      }
      if (option_count2 == 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 004c193a */ { dialog_dtor(&dialog2); }
        goto done;
      }
      button_count = 0;
      if (mode_state[1] != 0) {
        label = (iptr)label_string(label_string_ids[0xc3]);
        dialog_add_button(&dialog2,(const char *)label);
        mode_state[button_count + 2] = 0;
        button_count++;
      }
      if (mode_state[1] != 1) {
        label = (iptr)label_string(label_string_ids[0xc4]);
        dialog_add_button(&dialog2,(const char *)label);
        mode_state[button_count + 2] = 1;
        button_count++;
      }
      if (mode_state[1] != 2) {
        label = (iptr)label_string(label_string_ids[0xc5]);
        dialog_add_button(&dialog2,(const char *)label);
        mode_state[button_count + 2] = 2;
        button_count++;
      }
      choice = dlg_show_modal(&dialog2,0);
      if (choice < 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 004c193a */ { dialog_dtor(&dialog2); }
        goto done;
      }
      if (dialog2.result_extra != 1) break;
      if (diplo_session_active == 0) {
        if (mode_state[1] == 0) {
          pedia_show_topic_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),choice);
        }
        else if (mode_state[1] == 1) {
          advance = (iptr)unit_types[choice].prerequisite;
          unit_type = choice;
          dlg_load_game_section(&dialog2,(const char *)&str_helpon,1);
          option_flags = 0;
          option_id = 0;
          label = (iptr)label_string(unit_types[unit_type].name);
          dialog_add_option(&dialog2,(const char *)label,option_id,option_flags);
          option_flags = 0;
          option_id = 1;
          label = (iptr)label_string(advances[advance].name);
          dialog_add_option(&dialog2,(const char *)label,option_id,option_flags);
          choice = dlg_show_modal(&dialog2,0);
          if (choice == 0) {
            pedia_show_unit_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),unit_type);
          }
          if (choice == 1) {
            pedia_show_topic_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),advance);
          }
        }
        else if (mode_state[1] == 2) {
          advance = (iptr)improvements[choice].preq;
          improvement = choice;
          dlg_load_game_section(&dialog2,(const char *)&str_helpon_2,1);
          option_flags = 0;
          option_id = 0;
          label = (iptr)label_string(improvements[improvement].name);
          dialog_add_option(&dialog2,(const char *)label,option_id,option_flags);
          option_flags = 0;
          option_id = 1;
          label = (iptr)label_string(advances[advance].name);
          dialog_add_option(&dialog2,(const char *)label,option_id,option_flags);
          choice = dlg_show_modal(&dialog2,0);
          if (choice == 0) {
            pedia_show_improvement_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),improvement);
          }
          if (choice == 1) {
            pedia_show_topic_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),advance);
          }
        }
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
  advance = 0;
  do {
    if (99 < advance) {
      text_set_label_arg(0,advances[choice].name);
      if (path_count == 0) {
        if (civ2 == 0) {
          popup_game((const char *)&str_researchnone);
        }
        else {
          popup_game((const char *)&str_stealnone);
        }
        seh_state = 0xffffffff;
        /* inlined unwind funclet 004c193a */ { dialog_dtor(&dialog2); }
      }
      else {
        if (civ2 == 0) {
          dialog_load_game_text(&dialog2,(const char *)&str_researchthese);
        }
        else {
          dialog_load_game_text(&dialog2,(const char *)&str_stealthese);
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
          dialog_add_line
                    (&dialog2,(const char *)&map_views_block.text_buffer,0xffffffff,0xffffffff,0xffffffff,0);
        }
        dlg_show_modal(&dialog2,0);
        seh_state = 0xffffffff;
        /* inlined unwind funclet 004c193a */ { dialog_dtor(&dialog2); }
      }
done:
      return;
    }
    if ((advances[advance].enabled != '\0') &&
       (known = civ_has_advance(civ,advance), known == 0)) {
      if (civ2 == 0) {
        known = civ_has_advance(civ,(int)advances[advance].preq[0]);
        if (((known != 0) &&
            (known = civ_has_advance(civ,(int)advances[advance].preq[1]), known != 0)) &&
           ((exclude_current == 0 || (civs[civ].researching != advance)))) {
          if (((option_count2 != 0) && (game.difficulty != '\0')) && ((game.game_flags & 0x80) == 0)) {
            known = (int)(advance - ((uint)civs[civ].num_advances + exclude_current)) % 3;
            goto joined_r0x004c1729;
          }
          goto add_path_option;
        }
      }
      else {
        known = civ_has_advance(civ2,advance);
joined_r0x004c1729:
        if (known != 0) {
add_path_option:
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
    }
    advance++;
  } while( true );
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
  i32 seh_prev;
  i8 *seh_handler;
  uint seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&choice_dialog,0x4000);
  seh_state = 0;
  dialog_ctor(&info_dialog,0x4000);
  seh_state = (seh_state & 0xffffff00 | (byte)(1));
  chosen = civ_pick_research_advance(civ);
  if ((forced != 0) ||
     (((advance2 = chosen, game.turn != 0 &&
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) && (civ_saved_cursor_x_block.autoplay_active == 0)))) {
    while( true ) {
      advance2 = chosen;
      if ((((*((char *)&game.game_options + 1)) & 2) != 0) && (forced == 0)) {
        text_clear();
        text_append_string_id(advances[advance2].name);
        if (advance2 == 0x59) {
          text_append_space();
          text_append_int(civs[civ].future_techs + 1);
        }
        text_set_string_arg(0,(const char *)&map_views_block.text_buffer);
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
        dialog_load_text(&info_dialog,(const char *)&str_advice,(const char *)&map_views_block.text_buffer,8);
        dialog_add_sprite
                  (&info_dialog,
                   (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                         (char)advances[advance2].epoch * 0x3c + (char)advances[advance2].category * 0xf0)
                   ,(ui_large_screen == 0) - 1 & 8,0);
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
          dialog_add_line
                    (&info_dialog,(const char *)&map_views_block.text_buffer,0xffffffff,0xffffffff,0xffffffff,0);
        }
        ticks = timer_ticks();
        dialog_run_modal(&info_dialog,0,ticks - 300);
      }
      known = civ_has_advance(civ,0x26);
      text_set_label_arg(0,label_string_ids[0x31 - (uint)(known == 0)]);
      dialog_load_game_text(&choice_dialog,(const char *)&str_research);
      if (((*((char *)&game.game_options + 1)) & 2) != 0) {
        dialog_set_position(&choice_dialog,5,0xfffffc19);
      }
      dialog_set_option_draw_fn(&choice_dialog,(DialogOptionDrawFn)research_option_draw_advance);
      dialog_set_list_indent(&choice_dialog,0x4b,0);
      count = 0;
      for (advance = 0; advance < 100; advance++) {
        if (((((advances[advance].enabled != '\0') &&
              (known = civ_has_advance(civ,advance), known == 0)) &&
             ((known = civ_has_advance(civ,(int)advances[advance].preq[0]), known != 0 &&
              (known = civ_has_advance(civ,(int)advances[advance].preq[1]), known != 0))))
            && ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))) &&
           ((count == 0 ||
            ((((game.difficulty == '\0' || (forced != 0)) || ((game.game_flags & 0x80) != 0)) ||
             ((int)(advance - (uint)civs[civ].num_advances) % 3 != 0)))))) {
          text_clear();
          text_append_string_id(advances[advance].name);
          if (advance == 0x59) {
            text_append_space();
            text_append_int(civs[civ].future_techs + 1);
          }
          dialog_add_option(&choice_dialog,(const char *)&map_views_block.text_buffer,advance,0);
          count++;
        }
      }
      advance2 = chosen;
      if (count == 0) break;
      selected = dialog_run_modal(&choice_dialog,0,300);
      if (((*((char *)&game.game_options + 1)) & 2) != 0) {
        dialog_close(&info_dialog);
        ui_flush_input();
      }
      if (choice_dialog.result_extra == 2) {
        civ_research_goal_dialog(civ,0,0);
      }
      else {
        advance2 = selected;
        if (choice_dialog.result_extra == 0) break;
        pedia_show_topic_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),selected);
      }
    }
  }
  chosen = advance2;
  advance2 = chosen;
  civs[civ].researching = (i16)chosen;
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 004c217c */ { dialog_dtor(&info_dialog); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 004c2188 */ { dialog_dtor(&choice_dialog); }
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
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  dialog_ctor(&dialog2,0x4000);
  seh_state = 0;
  current_research = civs[civ].researching;
  if (civs[civ].researching < 0) {
    civ_choose_research(civ);
    current_research = civs[civ].researching;
  }
  advance = (iptr)current_research;
  if (advance < 0) {
    seh_state = 0xffffffff;
    /* inlined unwind funclet 004c2763 */ { dialog_dtor(&dialog2); }
  }
  else {
    if (research_notice_mode != 2) {
      civs[civ].research_progress = 0;
    }
    civs[civ].researching = -1;
    civ_give_advance(civ,advance,civ2,0,0);
    if ((game.turn == 0) || ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
      city = citywnd_get_city((CityWindow *)((i8 *)&city_window_block.city_window));
      civ_reset_city_production(civ,-1);
      if (((game.turn != 0) && (-1 < city)) &&
         ((city < game.num_cities &&
          ((cities[city].id != 0 && ((int)cities[city].owner == civ)))))) {
        city_recalc(city,0);
      }
      seh_state = 0xffffffff;
      /* inlined unwind funclet 004c2763 */ { dialog_dtor(&dialog2); }
    }
    else if ((human_civ[0] == civ) && (civ_saved_cursor_x_block.autoplay_active == 0)) {
      adjective = civ_adjective(civ);
      text_set_string_arg_b(0,(const char *)adjective);
      has_philosophy = civ_has_advance(civ,0x26);
      text_set_label_arg(1,label_string_ids[0x31 - (uint)(has_philosophy == 0)]);
      text_clear();
      text_append_string_id(advances[advance].name);
      if (advance == 0x59) {
        text_append_space();
        text_append_int((uint)civs[civ].future_techs);
      }
      text_set_string_arg_b(2,(const char *)&map_views_block.text_buffer);
      dialog_load_game_text(&dialog2,(const char *)&str_civadvance);
      dialog_add_sprite
                (&dialog2,
                 (iptr)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                       (char)advances[advance].category * 0xf0 +
                       (char)advances[advance].epoch * 0x3c),0,0);
      dialog_set_image_zoom(&dialog2,8);
      dlg_show_modal(&dialog2,0);
      if (((*((char *)&game.game_options + 2)) & 8) != 0) {
        pedia_show_topic_modal((PediaWindow *)((i8 *)&pedia_window_block.pedia_window),advance);
      }
      if ((((advance == 0x3c) && ((civs[civ].flags & 0x20) != 0)) &&
          (human_civ[0] == civ)) && (civ_saved_cursor_x_block.autoplay_active == 0)) {
        civ_golden_age_popup(civ);
      }
      if (unit_types[1].prerequisite == advance) {
        popup_game_with_sprite((const char *)&str_newxform,(iptr)&pollution_sprite_block.research_popup_sprite,8);
      }
      if (advance == 0x12) {
        popup_game_with_sprite((const char *)&str_newfortress,(iptr)&popup_info_icon_sprite,0);
      }
      if (improvements[0x20].preq == advance) {
        popup_game_with_sprite((const char *)&str_newairlift,(iptr)&caption_close_button_sprite_block.research_popup_sprite3,8);
      }
      if (advance == 0x43) {
        popup_game_with_sprite((const char *)&str_newrailroad,(iptr)&road_sprites_block.railroad_sprites,8);
      }
      if (improvements[0x18].preq == advance) {
        dialog_load_game_text(&dialog2,(const char *)&str_newfarmland);
        dialog_add_sprite(&dialog2,(iptr)&caption_close_button_sprite_block.research_dialog_sprite,0,0);
        dialog_add_sprite(&dialog2,(iptr)&land_improvement_sprites_block.farmland_sprite,0,0);
        dlg_show_modal(&dialog2,0);
      }
      if (unit_types[0xd].prerequisite == advance) {
        text_set_number_arg_u8(0,CONCAT31(unit_types[0xd].prerequisite >> 7,cosmic.paradrop_range));
        popup_game_with_sprite((const char *)&str_newparadrop,(iptr)&pollution_sprite_block.research_popup_sprite2,8);
      }
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (game.turn != 0)) {
        civ_choose_research(civ);
        civ_advance_gov_prompts(civ,advance);
      }
      seh_state = 0xffffffff;
      /* inlined unwind funclet 004c2763 */ { dialog_dtor(&dialog2); }
    }
    else {
      seh_state = 0xffffffff;
      /* inlined unwind funclet 004c2763 */ { dialog_dtor(&dialog2); }
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
  if (((game.game_flags & 0x80) == 0) || (prefs_game_rules_block.scenario_year_increment[0] == 0)) {
    if (tech_count < (uint)civs[game.best_human_rank].num_advances + (uint)civs[game.best_human_rank].research_cost_advances) {
      if (game.difficulty != '\0') {
        multiplier--;
      }
      if (((game.difficulty == 5) &&
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
  if ((game.game_flags & 0x80) == 0) {
    if (cosmic.tech_paradigm != '\n') {
      multiplier = (int)((uint)cosmic.tech_paradigm * multiplier) / 10;
    }
  }
  else if (prefs_game_rules_block.scenario_tech_paradigm != 10) {
    multiplier = (prefs_game_rules_block.scenario_tech_paradigm * multiplier) / 10;
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
  if ((game.game_flags & 4) != 0) {
    multiplier = (int)(multiplier * 5 + (multiplier * 5 >> 0x1f & 3U)) >> 2;
  }
  if ((game.game_flags & 8) != 0) {
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
      if ((civs[civ].flags & 0x20) != 0) {
        civs[civ].flags = civs[civ].flags & 0xffdf;
        civ_research_complete(civ,0);
      }
    }
    if ((civs[civ].research_progress != 0) && (civs[civ].researching < 0)) {
      if ((((*((char *)&game.game_options + 1)) & 1) != 0) &&
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
        (dist = city_has_improvement(city,1), dist != 0)) &&
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
  char *line_ptr;
  size_t line_len;
  char *lpCaption;
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
  cmp_result = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&civ_saved_cursor_x_block.game_home_dir));
  if ((cmp_result == 0) || (exists = file_exists((const char *)rules_name), exists == 0)) {
    __chdir(((i8 *)&civ_saved_cursor_x_block.game_home_dir));
    in_file = file_open_path((const char *)rules_name,(iptr)&str_rt_4);
    rc = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&civ_saved_cursor_x_block.game_home_dir));
    if (rc == 0) {
      rc = file_exists((const char *)&str_rules_bak_5);
      if ((rc != 0) && (rc = crt_remove(str_rules_bak_6), rc != 0))
      goto cleanup;
      out_file = file_open_path((const char *)&str_rules_bak_7,(iptr)&str_wt_2);
      uType = 0x40;
      lpCaption = str_warning_2;
      line_ptr = str_saving_changes_in_file_rules_bak;
      rc = (int)wnd_get_window((WndBase *)&editor_text_dlg_wnd_block.active_editor_wnd->window);
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
       (rc = crt_rename(rules_name,str_rules_bak_3), rc != 0)) goto cleanup;
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
        goto cleanup;
        str_strip_newline(s);
        str_trim((char *)s);
        rc = __strcmpi(s,header);
      } while (rc != 0);
      if (font_size16_bold_block.rules_edit_sections[section].writer != 0) {
        font_size16_bold_block.rules_edit_sections[section].writer(out_file,in_file);
      }
      do {
        line_ptr = _fgets(s,0x100,in_file);
        if (line_ptr == (char *)0x0) goto cleanup;
        line_len = _strlen(s);
      } while (1 < line_len);
      rc = _fputs(s,out_file);
      if (rc == -1) goto cleanup;
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
cleanup:
  if (in_file != (FILE *)0x0) {
    _fclose(in_file);
  }
  if (out_file != (FILE *)0x0) {
    _fclose(out_file);
  }
  __chdir(saved_cwd);
  return success;
}

/* 004e1314 auto */
void __cdecl rules_apply_double_moves(void)

{
  iptr unit_type;
  
  if (((game.multiplayer == '\0') || (civ_saved_cursor_x_block.mp_double_moves == 0)) || (double_moves_applied != 0)) {
    if (((game.multiplayer != '\0') && (civ_saved_cursor_x_block.mp_double_moves == 0)) && (double_moves_applied == 1)) {
      double_moves_applied = 0;
      for (unit_type = 0; unit_type < 0x3e; unit_type++) {
        if (unit_types[unit_type].domain == '\0') {
          unit_types[unit_type].move = (char)unit_types[unit_type].move / 2;
        }
      }
      status_redraw(1);
    }
  }
  else {
    double_moves_applied = 1;
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      if (unit_types[unit_type].domain == '\0') {
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
  net_session_block.civ_eliminate_busy = 1;
  if ((game.multiplayer < 3) || (net_session_block.net_host_flag != '\0')) {
    game.human_players = game.human_players & ~(byte)(1 << ((byte)civ & 0x1f));
    if (2 < game.multiplayer) {
      net_send_message(0x31,0,0,civ,0,0,0,0,0,0);
    }
    if ((civ_saved_cursor_x_block.mp_kill_civ_on_retire != 0) || ((char)kill_units != '\0')) {
      net_batch_begin();
      for (unit = 0; unit < game.num_units; unit++) {
        if (units[unit].id != 0) {
          if (((units[unit].owner == civ) || ((units[unit].flags & 2) == 0)) ||
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
                if ((units[unit2].orders == 3) && (units[unit2].goto_x == unit)) {
                  units[unit2].orders = -1;
                }
              }
            }
          }
          else {
            units[unit].flags = units[unit].flags & 0xfffd;
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
      net_session_block.civ_eliminate_busy = 0;
    }
    else {
      if ((char)recreate != '\0') {
        civ_create(civ);
      }
      if (2 < game.multiplayer) {
        net_send_state_diff(0xff,2,0,0,0);
        net_send_message(0x74,0xff,0,0,0,0,0,0,0,0);
      }
      map_redraw_all_views(human_civ[0],1);
      net_session_block.civ_eliminate_busy = 0;
    }
  }
  else {
    net_session_block.civ_eliminate_busy = 0;
    net_session_block.net_kill_or_quit_ack = -2;
    net_send_message(0x34,0,civ,0,0,0,0,0,0,0);
    x4 = timer_ticks();
    while ((net_session_block.net_kill_or_quit_ack == -2 && (y2 = timer_ticks(), y2 - x4 < 0xe10))) {
      net_process_messages(&net_session_block,1,1);
    }
    if (net_session_block.net_kill_or_quit_ack == -2) {
      dbg_log((const char *)&str_killorretire_connection_to_server_timed);
      popup_game_with_sprite((const char *)&str_serverconnecttime_6,(iptr)&popup_info_icon_sprite,0);
      game_loop_active = 0;
    }
    else {
      dbg_log((const char *)&str_killorretire_received_nm_kill_or_quit_ac);
    }
    while ((net_session_block.net_msgq_priority_count != 0 || (net_session_block.net_msgq_normal_count != 0))) {
      net_process_messages(&net_session_block,1,0);
    }
  }
  return;
}

/* 004ec312 auto */
void __cdecl civ_apply_eiffel_tower(iptr civ)

{
  uint *treaty_ptr;
  int civ2;
  
  if ((civs[civ].flags & 0x10) == 0) {
    civs[civ].reputation = civs[civ].reputation >> 1;
    civs[civ].flags = civs[civ].flags | 0x10;
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
  if (improvement == 2) {
    if ((game.difficulty < 2) && (cost2 != 0)) {
      cost2--;
    }
    has = civ_has_advance(civ,0x23);
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
    has = civ_has_wonder(civ,0x11);
    if (has != 0) {
      cost2 = 0;
    }
  }
  if (((cost2 != 0) && (civs[civ].government == 4)) &&
     ((improvement == 4 || ((improvement == 0xe || (improvement == 0xb)))))) {
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
    if ((civs[civ].government == '\0') ||
       ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
      for (civ2 = 0; civ2 < 8; civ2++) {
        treaty_ptr = (uint *)(civ * 4 + (iptr)((u8 *)&civs + 32) + civ2 * 0x594);
        *treaty_ptr = *treaty_ptr & 0xffffffef;
      }
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) {
      game.transient_flags = game.transient_flags & 0xfffb;
      for (city = 0; city < game.num_cities; city++) {
        if ((((cities[city].id != 0) && (cities[city].owner == civ)) &&
            (city_recalc(city,1), cities[city].building == 8)) &&
           (civs[civ].government != 4)) {
          cities[city].building = 11;
        }
      }
    }
  }
  if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) && (0 < new_gov)) &&
     (civ_saved_cursor_x_block.autoplay_active == 0)) {
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
  has_statue = civ_has_wonder(civ,0x13);
  switch(government) {
  case 2:
    known = civ_has_advance(civ,0x36);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    break;
  case 3:
    known = civ_has_advance(civ,0xf);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    break;
  case 4:
    known = civ_has_advance(civ,0x1f);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    if (advances[0x1f].enabled == '\0') {
      result = 0;
    }
    break;
  case 5:
    known = civ_has_advance(civ,0x47);
    if ((known == 0) && (has_statue == 0)) {
      result = 0;
    }
    break;
  case 6:
    known = civ_has_advance(civ,0x15);
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
    civs[civ].flags = civs[civ].flags | 8;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    if ((old_government != new_gov) || ((civs[civ].flags & 1) != 0)) {
      text = civ_adjective(civ);
      text_set_string_arg_b(0,(const char *)text);
      text_set_label_arg(1,government_names[new_gov]);
      text = (iptr)(civ_leader_name(civ));
      text_set_string_arg(2,(const char *)text);
      text = civ_leader_title(civ);
      text_set_string_arg(3,(const char *)text);
      notified = 0;
      if (((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) != 0) ||
         (((has_wonder = civ_has_wonder(human_civ[0],0x18), has_wonder != 0 ||
           (has_wonder = civ_has_wonder(human_civ[0],9), has_wonder != 0)) || (game.reveal_map != '\0')))
         ) {
        if (((civs[civ].flags & 1) == 0) || (civ_saved_cursor_x_block.autoplay_active != 0)) {
          if (civ_saved_cursor_x_block.autoplay_active == 0) {
            popup_game_with_sprite((const char *)&str_overthrown_2,(iptr)(((i8 *)&government_sprites_block.government_sprites) + old_government * 0x3c),
                               (ui_large_screen == 0) - 1 & 8);
            civs[civ].flags = civs[civ].flags | 1;
          }
        }
        else {
          popup_game_with_sprite((const char *)&str_changed_2,(iptr)(((i8 *)&government_sprites_block.government_sprites) + new_gov * 0x3c),
                             (ui_large_screen == 0) - 1 & 8);
          civs[civ].flags = civs[civ].flags & 0xfffe;
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
              ((((*(byte *)&civs[civ2].treaties[civ] & 0x80) != 0 ||
                (has_wonder = civ_has_wonder(civ2,0x18), has_wonder != 0)) ||
               (has_wonder = civ_has_wonder(civ2,9), has_wonder != 0)))))) {
            if (((civs[civ].flags & 1) == 0) || (civ_saved_cursor_x_block.autoplay_active != 0)) {
              net_send_text_args(0x2c,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,1,0,old_government,(ui_large_screen == 0) - 1 & 8);
              civs[civ].flags = civs[civ].flags | 1;
              notified++;
            }
            else {
              net_send_text_args(0x2b,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,4,0,new_gov,(ui_large_screen == 0) - 1 & 8);
              civs[civ].flags = civs[civ].flags & 0xfffe;
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
  civs[civ].flags = civs[civ].flags & 0xffb7;
  if (cur_gov == '\0') {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) || ((game.turn & 3U) != 0))
    {
      if ((((game.turn & 3U) == 0) || (value = civ_has_wonder(civ,0x13), value != 0)) &&
         (civ_change_government(civ,1),
         (1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) {
        civs[civ].flags = civs[civ].flags & 0xfffe;
      }
    }
    else if ((civs[civ].flags & 1) == 0) {
      civ_change_government(civ,1);
    }
    else {
      civs[civ].flags = civs[civ].flags & 0xfffe;
    }
  }
  if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
     ((civs[civ].flags & 1) != 0)) {
    civ_change_government(civ,(uint)civs[civ].government);
  }
  if (civ != 0) {
    value = _rand();
    civs[civ].diplo_random = (u8)(value % 100);
    value = _rand();
    if (value % 3 == 0) {
      civs[civ].flags = civs[civ].flags ^ 4;
    }
    if (((int)game.turn % 3 == 0) && (civs[civ].diplo_request_count != '\0')) {
      civs[civ].diplo_request_count--;
    }
    for (civ2 = 1; (int)civ2 < 8; civ2++) {
      if (2 < game.multiplayer) {
        net_process_messages(&net_session_block,1,0);
      }
      if (((byte)(civs[civ].treaties[civ2] >> 8) & 0x40) == 0) {
        setup_picwnd_block.diplo_recent_peace_table[civ2] = 0;
      }
      else {
        setup_picwnd_block.diplo_recent_peace_table[civ2] = 1;
      }
      treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
      *treaty_ptr = *treaty_ptr & 0xff5fbfff;
      if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) != 0) && (civ2 != civ))
         && ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0)) {
        if ((*(byte *)&civs[civ].treaties[civ2] & 0x20) != 0) {
          value = int_clamp(3 - ((int)(uint)civs[civ2].reputation >> 2),1,3);
          if (value - 1 < 1) {
            roll = 0;
          }
          else {
            roll = _rand();
            roll = roll % value;
          }
          if (roll == 0) {
            if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
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
            if (((byte)(civs[civ].treaties[civ2] >> 16) & 8) == 0) {
              treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
              *treaty_ptr = *treaty_ptr & 0xfffff7ff;
            }
            treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
            *treaty_ptr = *treaty_ptr & 0xfff7ffff;
          }
          if ((game.turn & 0xfU) == 0) {
            if (((byte)(civs[civ].treaties[civ2] >> 8) & 4) == 0) {
              if ((*(byte *)&civs[civ].treaties[civ2] & 8) == 0) {
                if (((*(byte *)&civs[civ].treaties[civ2] & 4) == 0) ||
                   ((game.game_flags & 1) == 0)) {
                  if (((*(byte *)&civs[civ].treaties[civ2] & 1) != 0) &&
                     (((((byte)(civs[civ].treaties[civ2] >> 8) & 0x20) != 0 &&
                       (((byte)(civs[civ].treaties[civ2] >> 8) & 8) == 0)) &&
                      (((byte)(civs[civ2].treaties[civ] >> 8) & 8) == 0)))) {
                    if (((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) &&
                         ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)) &&
                        (((*(byte *)&civs[human_civ[0]].treaties[civ] & 0x80) != 0 ||
                         (((value = civ_has_wonder(human_civ[0],0x18), value != 0 ||
                           (value = civ_has_wonder(human_civ[0],9), value != 0)) ||
                          (((*(byte *)&civs[human_civ[0]].treaties[civ2] & 0x80) != 0
                           || (((value = civ_has_wonder(human_civ[0],0x18), value != 0 ||
                                (value = civ_has_wonder(human_civ[0],9), value != 0)) ||
                               (game.reveal_map != '\0')))))))))) && (civ_saved_cursor_x_block.autoplay_active == 0)) {
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
                             (((*(byte *)&civs[civ3].treaties[civ] & 0x80) != 0 ||
                              ((*(byte *)&civs[civ3].treaties[civ2] & 0x80) != 0)))
                             ) || ((value = civ_has_wonder(civ3,0x18), value != 0 ||
                                   (value = civ_has_wonder(civ3,9), value != 0)))))) {
                          civ_plural = (iptr)(civ_name_plural(civ));
                          text_set_string_arg(0,(const char *)civ_plural);
                          civ_plural = (iptr)(civ_name_plural(civ2));
                          text_set_string_arg(1,(const char *)civ_plural);
                          net_send_text_args(0x50,net_session_block.net_slots[net_session_block.net_civ_slots[civ3]].id,2,0
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
              if (((((*(byte *)&civs[civ].treaties[civ2] & 1) != 0) &&
                   (((byte)(civs[civ].treaties[civ2] >> 8) & 8) == 0)) &&
                  (((byte)(civs[civ2].treaties[civ] >> 8) & 8) == 0)) &&
                 (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0 ||
                  ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) == 0)))) {
                diplo_clear_treaty_flags(civ,civ2,1);
              }
            }
            treaty_ptr = (uint *)(civ * 0x594 + (iptr)((u8 *)&civs + 32) + civ2 * 4);
            *treaty_ptr = *treaty_ptr & 0xfffffbff;
          }
          else if ((((game.turn & 7U) == 0) &&
                   ((*(byte *)&civs[civ].treaties[civ2] & 8) != 0)) &&
                  (((byte)(civs[civ].treaties[civ2] >> 8) & 4) == 0)) {
            diplo_ai_swap_advances(civ,civ2);
          }
        }
      }
    }
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      for (civ2 = 1; (int)civ2 < 8; civ2++) {
        if (2 < game.multiplayer) {
          net_process_messages(&net_session_block,1,0);
        }
        if ((((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) && (civ_saved_cursor_x_block.autoplay_active == 0)
            ) && ((*(byte *)&civs[civ].treaties[civ2] & 1) != 0)) {
          if (human_civ[0] == civ2) {
            diplo_ai_review_relations(civ,civ2);
          }
          else if (2 < game.multiplayer) {
            net_send_message(0xa0,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id
                               ,civ,civ2,0,0,0,0,0,0);
          }
        }
      }
      diplo_ai_military_aid(civ);
      for (civ4 = 1; (int)civ4 < 8; civ4++) {
        if (2 < game.multiplayer) {
          net_process_messages(&net_session_block,1,0);
        }
        if ((1 << ((byte)civ4 & 0x1f) & (uint)game.human_players) != 0) {
          if (human_civ[0] == civ4) {
            diplo_ai_seek_war_ally(civ,civ4);
          }
          else if (2 < game.multiplayer) {
            net_send_message(0xa1,net_session_block.net_slots[net_session_block.net_civ_slots[civ4]].id
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
  
  known = civ_has_advance(civ,5);
  if ((known != 0) && (known = civ_has_advance(civ,0x18), known != 0)) {
    return 2;
  }
  known = civ_has_advance(civ,0x3c);
  if ((known != 0) && (known = civ_has_advance(civ,0x26), known != 0)) {
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
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  best_advance = -1;
  best_value = -999;
  dialog_ctor(&dialog2,0x4000);
  seh_state = 0;
  if ((civ == 0) || (civ2 == 0)) {
    seh_state = 0xffffffff;
    /* inlined unwind funclet 0057a661 */ { dialog_dtor(&dialog2); }
  }
  else {
    interactive = human_civ[0] == civ;
    if (civ_saved_cursor_x_block.autoplay_active != 0) {
      interactive = false;
    }
    while( true ) {
      if (interactive) {
        dialog_load_game_text(&dialog2,(const char *)&str_takeciv_2);
      }
      for (advance = 0; advance < 100; advance++) {
        known = civ_has_advance(civ,advance);
        if ((((known == 0) && (known = civ_has_advance(civ2,advance), known != 0)) &&
            ((((game.game_flags & 0x80) == 0 || ((prefs_game_rules_block.scenario_flags[0] & 0x10) == 0)) ||
             ((((advance != 0x1f && (advance != 0x36)) && (advance != 0xf)) &&
              ((advance != 0x47 && (advance != 0x15)))))))) &&
           ((advances[advance].preq[0] != -2 || (advances[advance].preq[1] != -2)))) {
          if (interactive) {
            option_flags = 0;
            option_id = advance;
            text = (iptr)label_string(advances[advance].name);
            dialog_add_option(&dialog2,(const char *)text,option_id,option_flags);
          }
          value = ai_advance_value(civ,advance);
          if (best_value <= value) {
            best_advance = advance;
            best_value = value;
          }
        }
      }
      if (best_advance < 0) {
        seh_state = 0xffffffff;
        /* inlined unwind funclet 0057a661 */ { dialog_dtor(&dialog2); }
        goto done;
      }
      advance = best_advance;
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) ||
         (advance = dlg_show_modal(&dialog2,0), dialog2.result_extra == 0))
      break;
      civ_research_goal_dialog(civ,0,civ2);
    }
    if ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0) {
      text = (iptr)(civ_name_plural(civ));
      text_set_string_arg(0,(const char *)text);
      text_set_label_arg(1,advances[advance].name);
      if ((civ_saved_cursor_x_block.autoplay_active == 0) && ((1 << ((byte)civ2 & 0x1f) & (uint)game.human_players) != 0)) {
        if (human_civ[0] == civ2) {
          popup_game_with_advance((const char *)&str_tookciv_2,advance,0);
        }
        else if (2 < game.multiplayer) {
          net_send_text_args(99,net_session_block.net_slots[net_session_block.net_civ_slots[civ2]].id,2,0,
                             advance,0);
        }
      }
    }
    civ_give_advance(civ,advance,civ2,0,0);
    if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
      civ_reset_city_production(civ,-1);
    }
    seh_state = 0xffffffff;
    /* inlined unwind funclet 0057a661 */ { dialog_dtor(&dialog2); }
  }
done:
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
  
  allowed = events_check_no_schism((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events),civ);
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
          has_palace = city_has_improvement(city,1);
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
          tiles = map_tiles;
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
            net_send_message(0x74,0xff,0,0,0,0,0,0,0,0);
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
  
  hWnd = control_get_hwnd((Control *)&editor_text_dlg_wnd_block.active_editor_wnd->list_box);
  SetFocus(hWnd);
  cosmic_editor_draw_bg(editor_text_dlg_wnd_block.active_editor_wnd);
  return;
}

/* 005866d3 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl cosmic_editor_capture_defaults(void)

{
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_values) = (uint)cosmic.road_move_mult;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_trireme_loss) = (uint)cosmic.trireme_loss_chance;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_food_per_citizen) = (uint)cosmic.food_per_citizen;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_food_box_rows) = (uint)cosmic.food_box_rows;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_shield_box_rows) = (uint)cosmic.shield_box_rows;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_settler_food_low) = (uint)cosmic.settler_food_low;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_settler_food_high) = (uint)cosmic.settler_food_high;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_unhappy_size) = (uint)cosmic.unhappy_size;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_riot_factor) = (uint)cosmic.riot_factor;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_aqueduct_size) = (uint)cosmic.aqueduct_size;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_sewer_size) = (uint)cosmic.sewer_size;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_tech_paradigm) = (uint)cosmic.tech_paradigm;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_transform_time) = (uint)cosmic.transform_time;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_monarchy_support) = (uint)cosmic.monarchy_free_support;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_communism_support) = (uint)cosmic.communism_free_support;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_fundamentalism_support) = (uint)cosmic.fundamentalism_free_support;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_communism_palace_dist) = (uint)cosmic.communism_palace_dist;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_fund_science_loss) = (uint)cosmic.fund_science_loss;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_change_penalty) = (uint)cosmic.change_penalty;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_paradrop_range) = (uint)cosmic.paradrop_range;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_mass_thrust_paradigm) = (uint)cosmic.mass_thrust_paradigm;
  (*(i32 *)&editor_text_dlg_wnd_block.cosmic_edit_fund_max_science) = (uint)cosmic.fund_max_science;
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
  cosmic.road_move_mult = editor_text_dlg_wnd_block.cosmic_edit_values[0];
  cosmic.trireme_loss_chance = editor_text_dlg_wnd_block.cosmic_edit_trireme_loss;
  cosmic.food_per_citizen = editor_text_dlg_wnd_block.cosmic_edit_food_per_citizen;
  cosmic.food_box_rows = editor_text_dlg_wnd_block.cosmic_edit_food_box_rows;
  cosmic.shield_box_rows = editor_text_dlg_wnd_block.cosmic_edit_shield_box_rows;
  cosmic.settler_food_low = editor_text_dlg_wnd_block.cosmic_edit_settler_food_low;
  cosmic.settler_food_high = editor_text_dlg_wnd_block.cosmic_edit_settler_food_high;
  cosmic.unhappy_size = editor_text_dlg_wnd_block.cosmic_edit_unhappy_size;
  cosmic.riot_factor = editor_text_dlg_wnd_block.cosmic_edit_riot_factor;
  cosmic.aqueduct_size = editor_text_dlg_wnd_block.cosmic_edit_aqueduct_size;
  cosmic.sewer_size = editor_text_dlg_wnd_block.cosmic_edit_sewer_size;
  cosmic.tech_paradigm = editor_text_dlg_wnd_block.cosmic_edit_tech_paradigm;
  cosmic.transform_time = editor_text_dlg_wnd_block.cosmic_edit_transform_time;
  cosmic.monarchy_free_support = editor_text_dlg_wnd_block.cosmic_edit_monarchy_support;
  cosmic.communism_free_support = editor_text_dlg_wnd_block.cosmic_edit_communism_support;
  cosmic.fundamentalism_free_support = editor_text_dlg_wnd_block.cosmic_edit_fundamentalism_support;
  cosmic.communism_palace_dist = editor_text_dlg_wnd_block.cosmic_edit_communism_palace_dist;
  cosmic.fund_science_loss = editor_text_dlg_wnd_block.cosmic_edit_fund_science_loss;
  cosmic.change_penalty = editor_text_dlg_wnd_block.cosmic_edit_change_penalty;
  cosmic.paradrop_range = editor_text_dlg_wnd_block.cosmic_edit_paradrop_range;
  cosmic.mass_thrust_paradigm = editor_text_dlg_wnd_block.cosmic_edit_mass_thrust_paradigm;
  cosmic.fund_max_science = editor_text_dlg_wnd_block.cosmic_edit_fund_max_science;
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
  listbox_clear(&editor_text_dlg_wnd_block.active_editor_wnd->list_box);
  text_open_section((const char *)&str_debug_11,(const char *)&str_editcosmic);
  while( true ) {
    _Str = text_read_line(1);
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
    crt_strcat(line,((i8 *)&map_views_block.text_buffer));
    listbox_add_string(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,(const char *)line);
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
  
  sel = listbox_get_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->list_box);
  if (editor_text_dlg_wnd_block.active_editor_wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_text_dlg_wnd_block.active_editor_wnd->window;
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
    listbox_set_cur_sel(&editor_text_dlg_wnd_block.active_editor_wnd->list_box,sel);
  }
  ctrl_set_focus((Control *)&editor_text_dlg_wnd_block.active_editor_wnd->list_box);
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
    _fgets(line,0x80,in_file);
    comment = _strchr(line,0x3b);
    _sprintf(((i8 *)&map_views_block.text_buffer),str_8d_s,editor_text_dlg_wnd_block.editor_item_recs[0].values[i],comment);
    _fputs(((i8 *)&map_views_block.text_buffer),out_file);
  }
  return 1;
}

/* 00586da1 auto */
void __cdecl cosmic_editor_ok_clicked(void)

{
  iptr ok;
  NativeWindow *wnd;
  char *lpText;
  char *lpCaption;
  UINT uType;
  char message[32];
  
  rules_edit_set_section((iptr)&str_cosmic_2,cosmic_editor_write_section);
  ok = rules_write_sections();
  if (ok == 0) {
    _sprintf(message,str_error_updating_rules_s_5,((i8 *)&lang_file_ext));
    uType = 0x10;
    lpCaption = str_file_i_o_error_6;
    lpText = message;
    wnd = wnd_get_window((WndBase *)&editor_text_dlg_wnd_block.active_editor_wnd->window);
    MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
  }
  editor_text_dlg_wnd_block.editor_running = 0;
  wnd_end_modal_loop(&editor_text_dlg_wnd_block.active_editor_wnd->window);
  return;
}

/* 00586e24 auto */
void __cdecl cosmic_editor_help_clicked(void)

{
  i32 parent_wnd2;
  
  if (editor_text_dlg_wnd_block.active_editor_wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_text_dlg_wnd_block.active_editor_wnd->window;
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
  editor_text_dlg_wnd_block.editor_running = 0;
  wnd_end_modal_loop(&editor_text_dlg_wnd_block.active_editor_wnd->window);
  return;
}

/* 00586eb0 auto */
void __thiscall cosmic_editor_draw_bg(EditorWindow *self)

{
  frame_wnd_draw_caption(self);
  gfx_fill_rect((iptr)self,(RECT *)&self->client_rect,0x1a);
  text_set_surface((iptr)self);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  wnd_blit_surface(self);
  return;
}

/* 00586f16 auto */
void __thiscall cosmic_editor_dialog(EditorWindow *self)

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
  i32 seh_prev;
  i8 *seh_handler;
  int seh_state;
  
  seh_state = 0xffffffff;
  palette_init(&palette);
  seh_state = 0;
  editor_text_dlg_wnd_block.editor_running = 1;
  editor_text_dlg_wnd_block.active_editor_wnd = self;
  this_00 = (DibSurface *)operator_new(0x48);
  (*((char *)&seh_state + 0)) = 1;
  if (this_00 == (DibSurface *)0x0) {
    surface = (DibSurface *)0x0;
  }
  else {
    surface = surface_ctor(this_00);
  }
  seh_state = seh_state & 0xffffff00;
  editor_bg_surface = surface;
  font_set((GameFont *)((i8 *)&editor_font_block.editor_font),0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_font((iptr)&editor_font_block.editor_font);
  ui_set_ctrl_text_color(0x29);
  ui_set_ctrl_text_shadow(0x12,1,1);
  ui_set_ctrl_disabled_color(0x25);
  self->wnd_width_u = 0x230;
  self->wnd_height_u = 0x1c6;
  self->item_uptr = 0;
  editor_text_dlg_wnd_block.editor_next_ctrl_id = 0xc9;
  font_height = font_cached_height((GameFont *)((i8 *)&editor_font_block.editor_font));
  self->font_height_uptr = font_height;
  reserved_3 = 0;
  reserved_2 = 0;
  reserved_1 = 0;
  height = self->wnd_height_u;
  width = self->wnd_width_u;
  pos_y = 0;
  pos_x = 0;
  style = 0xd;
  title = (iptr)label_string(label_string_ids[0x253]);
  frame_wnd_create
            (self,(const char *)title,style,pos_x,pos_y,width,height,reserved_1,reserved_2,reserved_3);
  rect_set_xywh((iptr)rect,self->client_x + 2,self->client_y + 2,self->client_width - 5,
                     self->client_height - 0x14);
  x_pos = editor_text_dlg_wnd_block.editor_next_ctrl_id;
  editor_text_dlg_wnd_block.editor_next_ctrl_id++;
  if (self == (EditorWindow *)0x0) {
    list_parent = (u8 *)0x0;
  }
  else {
    list_parent = (u8 *)&self->window;
  }
  listbox_create
            (&self->list_box,(iptr)list_parent,x_pos,(iptr)rect);
  listbox_set_font(&self->list_box,(iptr)&editor_font_block.editor_font);
  listbox_set_dblclick_callback(&self->list_box,(CtrlNotifyFn)cosmic_editor_edit_value);
  cosmic_editor_capture_defaults();
  cosmic_editor_fill_list();
  listbox_set_cur_sel(&self->list_box,0);
  x_pos = font_line_spacing((GameFont *)(*(i8 * *)(dialog_default_text_font_rest + 8)));
  self->button_height_u = x_pos + 8;
  x_pos = self->client_width - 10;
  self->button_width_u = (int)(x_pos + (x_pos >> 0x1f & 3U)) >> 2;
  y_pos = (self->client_y + self->client_height) - (self->button_height_u + 2);
  x_pos = self->client_x + 2;
  rect_set_xywh((iptr)rect,x_pos,y_pos,self->button_width_u,self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0xfe]);
  button_create
            (&self->ok_button,(iptr)ok_parent,0x65,(iptr)rect,(const char *)label);
  button_set_callback(&self->ok_button,(CtrlNotifyFn)cosmic_editor_ok_clicked);
  x_pos = x_pos + self->button_width_u + 2;
  rect_set_xywh((iptr)rect,x_pos,y_pos,self->button_width_u,self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    edit_parent = (u8 *)0x0;
  }
  else {
    edit_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0x238]);
  button_create
            (&self->rename_button,(iptr)edit_parent,0x66,(iptr)rect,(const char *)label);
  button_set_callback(&self->rename_button,(CtrlNotifyFn)cosmic_editor_edit_value);
  ctrl_set_default_button(&self->rename_button);
  x_pos = x_pos + self->button_width_u + 2;
  rect_set_xywh((iptr)rect,x_pos,y_pos,self->button_width_u,self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    help_parent = (u8 *)0x0;
  }
  else {
    help_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0x23b]);
  button_create
            (&self->help_button,(iptr)help_parent,0x66,(iptr)rect,(const char *)label);
  button_set_callback(&self->help_button,(CtrlNotifyFn)cosmic_editor_help_clicked);
  rect_set_xywh((iptr)rect,x_pos + self->button_width_u + 2,y_pos,self->button_width_u,
                     self->button_height_u);
  if (self == (EditorWindow *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&self->window;
  }
  label = (iptr)label_string(label_string_ids[0xff]);
  button_create
            (&self->cancel_button,(iptr)cancel_parent,0x66,(iptr)rect,(const char *)label);
  button_set_callback(&self->cancel_button,(CtrlNotifyFn)cosmic_editor_cancel_clicked);
  ctrl_set_cancel_button(&self->cancel_button);
  wnd_set_keyboard_nav(&self->window,0);
  wnd_set_redraw_callback(self,cosmic_editor_focus_redraw);
  wnd_repaint(self);
  window_show(self);
  wnd_run_modal_loop((ModalMenuWindow *)&self->window);
  while (editor_text_dlg_wnd_block.editor_running != 0) {
    win_pump_one_message();
  }
  if (editor_bg_surface != (DibSurface *)0x0) {
    surface_delete(editor_bg_surface,1);
  }
  editor_bg_surface = (DibSurface *)0x0;
  seh_state = 0xffffffff;
  /* inlined unwind funclet 005875e9 */ { palette_destroy(&palette); }
  return;
}

/* 0058760d auto */
void __cdecl cosmic_editor_run(void)

{
  EditorWindow editor_wnd;
  i32 seh_prev;
  i8 *seh_handler;
  i32 seh_state;
  
  seh_state = 0xffffffff;
  editor_wnd_ctor(&editor_wnd);
  seh_state = 0;
  cosmic_editor_dialog(&editor_wnd);
  wnd_repaint((SurfaceWindow *)((i8 *)&map_views_block.map_views));
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00587666 */ { editor_wnd_dtor(&editor_wnd); }
  return;
}

/* 00598a05 auto */
i32 __cdecl civ_spaceship_part_maxed(iptr civ,iptr improvement)

{
  if (improvement == 0x23) {
    if (spaceship_part_table_block.spaceship_part_max_counts[0] <= civs[civ].spaceship_parts[0]) {
      return 1;
    }
  }
  else if (improvement == 0x24) {
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
