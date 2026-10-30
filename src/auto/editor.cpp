/* The game, the scenario editors: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00417fa0 auto */
EditorWindow *EditorWindow::construct()

{
  i32 eh_frame;
  i8 *eh_prev;
  
  this->FrameWindow::construct();
  crt_vector_ctor_iterator(&combos_u,0x38,6,combo_ctor,combo_dtor);
  crt_vector_ctor_iterator(edits_bytes,0x48,9,edit_ctor,
               edit_dtor);
  button_ctor(&ok_button);
  button_ctor(&help_button);
  button_ctor(&cancel_button);
  button_ctor(&rename_button);
  button_ctor(&icon_button);
  button_ctor(&misc_button);
  button_ctor(&coast_button);
  button_ctor((Button *)((u8 *)&terrain_misc_button_u));
  button_ctor(&unit_flags_button);
  button_ctor(&unit_sound_button);
  button_ctor(&add_trigger_button);
  button_ctor(&edit_trigger_button);
  button_ctor((Button *)((u8 *)&delete_trigger_button_u));
  button_ctor(&add_action_button);
  button_ctor(&edit_action_button);
  button_ctor(&delete_action_button);
  ((ListBox *)((u8 *)&list_box_uptr))->construct();
  trigger_listbox.construct();
  action_listbox.construct();
  vtable = (u32)((pointer *)ptr_fun_rest);
  return this;
}

/* 004183d0 auto */
void EditorWindow::destruct()

{
  i8 *eh_prev;
  int eh_state;
  
  eh_state = eh_state & 0xff;
  (*((char *)&eh_state + 0)) = 0x14;
  /* inlined unwind funclet 0041851f */ { ((ListBox *)(((int)this) + 0xb0c))->destruct(); }
  (*((char *)&eh_state + 0)) = 0x13;
  /* inlined unwind funclet 0041852e */ { ((ListBox *)(((int)this) + 0xad0))->destruct(); }
  (*((char *)&eh_state + 0)) = 0x12;
  /* inlined unwind funclet 0041853d */ { ((ListBox *)(((int)this) + 0xa94))->destruct(); }
  (*((char *)&eh_state + 0)) = 0x11;
  /* inlined unwind funclet 0041854c */ { ctrl_dtor((Button *)(((int)this) + 0xa58)); }
  (*((char *)&eh_state + 0)) = 0x10;
  /* inlined unwind funclet 0041855b */ { ctrl_dtor((Button *)(((int)this) + 0xa1c)); }
  (*((char *)&eh_state + 0)) = 0xf;
  /* inlined unwind funclet 0041856a */ { ctrl_dtor((Button *)(((int)this) + 0x9e0)); }
  (*((char *)&eh_state + 0)) = 0xe;
  /* inlined unwind funclet 00418579 */ { ctrl_dtor((Button *)(((int)this) + 0x9a4)); }
  (*((char *)&eh_state + 0)) = 0xd;
  /* inlined unwind funclet 00418588 */ { ctrl_dtor((Button *)(((int)this) + 0x968)); }
  (*((char *)&eh_state + 0)) = 0xc;
  /* inlined unwind funclet 00418597 */ { ctrl_dtor((Button *)(((int)this) + 0x92c)); }
  (*((char *)&eh_state + 0)) = 0xb;
  /* inlined unwind funclet 004185a6 */ { ctrl_dtor((Button *)(((int)this) + 0x8f0)); }
  (*((char *)&eh_state + 0)) = 10;
  /* inlined unwind funclet 004185b5 */ { ctrl_dtor((Button *)(((int)this) + 0x8b4)); }
  (*((char *)&eh_state + 0)) = 9;
  /* inlined unwind funclet 004185c4 */ { ctrl_dtor((Button *)(((int)this) + 0x878)); }
  (*((char *)&eh_state + 0)) = 8;
  /* inlined unwind funclet 004185d3 */ { ctrl_dtor((Button *)(((int)this) + 0x83c)); }
  (*((char *)&eh_state + 0)) = 7;
  /* inlined unwind funclet 004185e2 */ { ctrl_dtor((Button *)(((int)this) + 0x800)); }
  (*((char *)&eh_state + 0)) = 6;
  /* inlined unwind funclet 004185f1 */ { ctrl_dtor((Button *)(((int)this) + 0x7c4)); }
  (*((char *)&eh_state + 0)) = 5;
  /* inlined unwind funclet 00418600 */ { ctrl_dtor((Button *)(((int)this) + 0x788)); }
  (*((char *)&eh_state + 0)) = 4;
  /* inlined unwind funclet 0041860f */ { ctrl_dtor((Button *)(((int)this) + 0x74c)); }
  (*((char *)&eh_state + 0)) = 3;
  /* inlined unwind funclet 0041861e */ { ctrl_dtor((Button *)(((int)this) + 0x710)); }
  (*((char *)&eh_state + 0)) = 2;
  /* inlined unwind funclet 0041862d */ { ctrl_dtor((Button *)(((int)this) + 0x6d4)); }
  (*((char *)&eh_state + 0)) = 1;
  /* inlined unwind funclet 0041863c */ { crt_vector_dtor_iterator((u8 *)this + 0x44c,0x48,9,edit_dtor); }
  eh_state = eh_state & 0xffffff00;
  /* inlined unwind funclet 00418654 */ { crt_vector_dtor_iterator((u8 *)this + 0x2fc,0x38,6,combo_dtor); }
  eh_state = 0xffffffff;
  /* inlined unwind funclet 0041866c */ { FrameWindow::destruct(); }
  return;
}

/* 00447fc2 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl editor_unit_backdrop_sprite_global_init(void)

{
  editor_unit_backdrop_sprite_static_init();
  editor_unit_backdrop_sprite_reg_atexit();
  return;
}

/* 00447fdc auto */
void __cdecl editor_unit_backdrop_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&editor_unit_backdrop_sprite));
  return;
}

/* 00447ff6 auto */
void __cdecl editor_unit_backdrop_sprite_reg_atexit(void)

{
  _atexit(editor_unit_backdrop_sprite_static_dtor);
  return;
}

/* 00448013 auto */
void __cdecl editor_unit_backdrop_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&editor_unit_backdrop_sprite));
  return;
}

/* 0044802d auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl editor_tile_backdrop_sprite_global_init(void)

{
  editor_tile_backdrop_sprite_static_init();
  editor_tile_backdrop_sprite_reg_atexit();
  return;
}

/* 00448047 auto */
void __cdecl editor_tile_backdrop_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite));
  return;
}

/* 00448061 auto */
void __cdecl editor_tile_backdrop_sprite_reg_atexit(void)

{
  _atexit(editor_tile_backdrop_sprite_static_dtor);
  return;
}

/* 0044807e auto */
void __cdecl editor_tile_backdrop_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite));
  return;
}

/* 004a3060 auto */
void __cdecl tribe_editor_load_tables(void)

{
  char *text;
  size_t max_len;
  int adjective_id;
  int plural_id;
  iptr civ;
  iptr leader;
  
  for (leader = 0; leader < 0x15; leader++) {
    civ = 1;
    while ((civ < 8 && (civs[civ].tribe != leader))) {
      civ++;
    }
    if (civ < 8) {
      max_len = 0x18;
      text = civ_name_plural(civ);
      _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x2a],text,max_len);
      editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x2a][0x17] = 0;
      max_len = 0x18;
      text = (char *)civ_adjective(civ);
      _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x3f],text,max_len);
      editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x3f][0x17] = 0;
      editor_text_dlg_wnd_block.editor_item_recs[leader].values[0] = (int)civ_names[civ].city_style;
    }
    else {
      if (leaders[leader].plural < 1) {
        plural_id = ~(int)leaders[leader].plural + 1;
      }
      else {
        plural_id = (int)leaders[leader].plural;
      }
      max_len = 0x18;
      text = label_string(plural_id);
      _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x2a],text,max_len);
      editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x2a][0x17] = 0;
      if (leaders[leader].adjective < 1) {
        adjective_id = ~(int)leaders[leader].adjective + 1;
      }
      else {
        adjective_id = (int)leaders[leader].adjective;
      }
      max_len = 0x18;
      text = label_string(adjective_id);
      _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x3f],text,max_len);
      editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x3f][0x17] = 0;
      editor_text_dlg_wnd_block.editor_item_recs[leader].values[0] = (int)leaders[leader].city_style;
    }
    editor_text_dlg_wnd_block.editor_item_recs[leader].values[1] = leaders[leader].attack + 1;
    editor_text_dlg_wnd_block.editor_item_recs[leader].values[2] = leaders[leader].expand + 1;
    editor_text_dlg_wnd_block.editor_item_recs[leader].values[3] = leaders[leader].civilize + 1;
  }
  for (leader = 0; leader < 7; leader++) {
    max_len = 0xf;
    text = label_string(government_names[leader]);
    _strncpy(((i8 *)&editor_lists.govt_names) + leader * 0xf,text,max_len);
    (((i8 *)&editor_lists.govt_name_ends))[leader * 0xf] = 0;
    max_len = 0xf;
    text = label_string(government_titles[leader].male);
    _strncpy(((i8 *)&editor_lists.govt_names) + (leader * 3 + 0x15) * 5,text,max_len);
    (((i8 *)&editor_lists.govt_name_ends))[leader * 0xf] = 0;
    max_len = 0xf;
    text = label_string(government_titles[leader].female);
    _strncpy(((i8 *)&editor_lists.govt_names) + (leader * 3 + 0x2a) * 5,text,max_len);
    (((i8 *)&editor_lists.govt_name_ends))[leader * 0xf] = 0;
  }
  for (leader = 0; leader < 0x10; leader++) {
    max_len = 10;
    text = label_string(commodity_names[leader]);
    _strncpy(((i8 *)&editor_lists.commodity_names) + leader * 10,text,max_len);
    (((i8 *)&editor_lists.commodity_name_ends))[leader * 10] = 0;
  }
  return;
}

/* 004a3426 auto */
void __cdecl tribe_editor_apply_tables(void)

{
  char *src;
  char *dest;
  size_t max_len;
  iptr civ;
  iptr leader;
  
  for (leader = 0; leader < 0x15; leader++) {
    civ = 1;
    while ((civ < 8 && (civs[civ].tribe != leader))) {
      civ++;
    }
    if (civ < 8) {
      civ_names[civ].city_style = (i16)editor_text_dlg_wnd_block.editor_item_recs[leader].values[0];
    }
    leaders[leader].city_style = (i16)editor_text_dlg_wnd_block.editor_item_recs[leader].values[0];
    leaders[leader].attack = (char)editor_text_dlg_wnd_block.editor_item_recs[leader].values[1] - 1;
    leaders[leader].expand = (char)editor_text_dlg_wnd_block.editor_item_recs[leader].values[2] - 1;
    leaders[leader].civilize = (char)editor_text_dlg_wnd_block.editor_item_recs[leader].values[3] - 1;
  }
  for (leader = 0; leader < 7; leader++) {
    max_len = 0xf;
    src = ((i8 *)&editor_lists.govt_names) + leader * 0xf;
    dest = label_string(government_names[leader]);
    editor_copy_pool_name(dest,src,max_len);  /* B32 (docs/BUGS.md) */
    max_len = 0xf;
    src = ((i8 *)&editor_lists.govt_names) + (leader * 3 + 0x15) * 5;
    dest = label_string(government_titles[leader].male);
    editor_copy_pool_name(dest,src,max_len);  /* B32 (docs/BUGS.md) */
    max_len = 0xf;
    src = ((i8 *)&editor_lists.govt_names) + (leader * 3 + 0x2a) * 5;
    dest = label_string(government_titles[leader].female);
    editor_copy_pool_name(dest,src,max_len);  /* B32 (docs/BUGS.md) */
  }
  for (leader = 0; leader < 0x10; leader++) {
    max_len = 10;
    src = ((i8 *)&editor_lists.commodity_names) + leader * 10;
    dest = label_string(commodity_names[leader]);
    editor_copy_pool_name(dest,src,max_len);  /* B32 (docs/BUGS.md) */
  }
  return;
}

/* 004a3640 auto */
void __cdecl tribe_editor_load_controls(void)

{
  int ctrl_id;
  int i;
  
  for (i = 1; i < 5; i++) {
    if (tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].kind == 9) {
      ((Control *)
                 &editor_active.wnd->edits[tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot])->set_text(
                 (const char *)editor_text_dlg_wnd_block.editor_item_name_recs[editor_active.wnd->item + 0x3f]);
    }
    else if (tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].kind == 0xc) {
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot])->get_id();
      editor_active.wnd->combos[tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot].set_cur_sel(
                 *(iptr *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + ctrl_id * 4 + editor_active.wnd->item * 0x58));
    }
  }
  return;
}

/* 004a3757 auto */
void __cdecl tribe_editor_store_controls(void)

{
  int ctrl_id;
  int i;
  char text[24];
  i32 selection;
  
  for (i = 1; i < 5; i++) {
    if (tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].kind == 9) {
      ((Control *)
                 &editor_active.wnd->edits[tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot])->Control::get_text(
                 (char *)text);
      _strncpy(editor_text_dlg_wnd_block.editor_item_name_recs[editor_active.wnd->item + 0x3f],text,0x18);
    }
    else if (tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].kind == 0xc) {
      selection = editor_active.wnd->combos[tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot].get_cur_sel();
      ctrl_id = ((Control *)
                         &editor_active.wnd->combos[tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot])->get_id();
      *(i32 *)(((i8 *)&editor_text_dlg_wnd_block.editor_ctrl_values) + editor_active.wnd->item * 0x58 + ctrl_id * 4) = selection;
    }
  }
  return;
}

/* 004a3889 auto */
void __cdecl tribe_editor_redraw(void)

{
  editor_active.wnd->tribe_editor_paint();
  return;
}

/* 004a38a4 auto */
i32 __cdecl tribe_editor_write_governments(FILE *file)

{
  char *text;
  size_t len;
  size_t title_len;
  size_t name_len;
  iptr gov;
  
  for (gov = 0; gov < 7; gov++) {
    text_clear();
    text_append_string_id(government_names[gov]);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_gov_name_sep_str));
    text = label_string(government_names[gov]);
    len = _strlen(text);
    if (len < 0xf) {
      text = label_string(government_names[gov]);
      name_len = _strlen(text);
    }
    else {
      name_len = 0xf;
    }
    text_append_spaces(0xf - name_len);
    text_append_string_id(government_titles[gov].male);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_gov_male_title_sep_str));
    text = label_string(government_titles[gov].male);
    len = _strlen(text);
    if (len < 0xc) {
      text = label_string(government_titles[gov].male);
      title_len = _strlen(text);
    }
    else {
      title_len = 0xc;
    }
    text_append_spaces(0xc - title_len);
    text_append_string_id(government_titles[gov].female);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_gov_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 004a3a21 auto */
i32 __cdecl tribe_editor_write_leaders(FILE *file,i32 src_handle)

{
  i32 saved_handle;
  iptr female_flag;
  char *text;
  size_t len;
  i32 min_val;
  i32 max_val;
  int adjective_id3;
  int adjective_id2;
  size_t adjective_len;
  int adjective_id;
  int plural_id3;
  int plural_id2;
  size_t plural_len;
  int plural_id;
  int female_id3;
  int female_id2;
  size_t female_len;
  int female_id;
  int male_id3;
  int male_id2;
  size_t male_len;
  int male_id;
  iptr leader;
  i8 line_tail[128];
  
  saved_handle = (*(i32 *)&text_file_handle);
  (*(i32 *)&text_file_handle) = src_handle;
  for (leader = 0; leader < 0x15; leader++) {
    text_read_line(1);
    text_next_token();
    text_next_token();
    max_val = 1;
    min_val = 0;
    female_flag = text_next_int();
    female_flag = int_clamp(female_flag,min_val,max_val);
    text_next_int();
    text_next_int();
    text_next_token();
    text_next_token();
    text_next_int();
    text_next_int();
    text_next_int();
    crt_strcpy(line_tail,(const char *)((const void *)text_parse.line_cursor));
    text_clear();
    if (leaders[leader].names[0] < 1) {
      male_id = ~(int)leaders[leader].names[0] + 1;
    }
    else {
      male_id = (int)leaders[leader].names[0];
    }
    text_append_string_id(male_id);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_male_name_sep_str));
    if (leaders[leader].names[0] < 1) {
      male_id2 = ~(int)leaders[leader].names[0] + 1;
    }
    else {
      male_id2 = (int)leaders[leader].names[0];
    }
    text = label_string(male_id2);
    len = _strlen(text);
    if (len < 0xd) {
      if (leaders[leader].names[0] < 1) {
        male_id3 = ~(int)leaders[leader].names[0] + 1;
      }
      else {
        male_id3 = (int)leaders[leader].names[0];
      }
      text = label_string(male_id3);
      male_len = _strlen(text);
    }
    else {
      male_len = 0xd;
    }
    text_append_spaces(0xd - male_len);
    if (leaders[leader].names[1] < 1) {
      female_id = ~(int)leaders[leader].names[1] + 1;
    }
    else {
      female_id = (int)leaders[leader].names[1];
    }
    text_append_string_id(female_id);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_female_name_sep_str));
    if (leaders[leader].names[1] < 1) {
      female_id2 = ~(int)leaders[leader].names[1] + 1;
    }
    else {
      female_id2 = (int)leaders[leader].names[1];
    }
    text = label_string(female_id2);
    len = _strlen(text);
    if (len < 0x14) {
      if (leaders[leader].names[1] < 1) {
        female_id3 = ~(int)leaders[leader].names[1] + 1;
      }
      else {
        female_id3 = (int)leaders[leader].names[1];
      }
      text = label_string(female_id3);
      female_len = _strlen(text);
    }
    else {
      female_len = 0x14;
    }
    text_append_spaces(0x14 - female_len);
    text_append_int(female_flag);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_female_flag_sep_str));
    text_append_int((int)leaders[leader].color);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_color_sep_str));
    text_append_int((int)leaders[leader].city_style);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_city_style_sep_str));
    if (leaders[leader].plural < 1) {
      plural_id = ~(int)leaders[leader].plural + 1;
    }
    else {
      plural_id = (int)leaders[leader].plural;
    }
    text_append_string_id(plural_id);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_plural_sep_str));
    if (leaders[leader].plural < 1) {
      plural_id2 = ~(int)leaders[leader].plural + 1;
    }
    else {
      plural_id2 = (int)leaders[leader].plural;
    }
    text = label_string(plural_id2);
    len = _strlen(text);
    if (len < 0xc) {
      if (leaders[leader].plural < 1) {
        plural_id3 = ~(int)leaders[leader].plural + 1;
      }
      else {
        plural_id3 = (int)leaders[leader].plural;
      }
      text = label_string(plural_id3);
      plural_len = _strlen(text);
    }
    else {
      plural_len = 0xc;
    }
    text_append_spaces(0xc - plural_len);
    if (leaders[leader].adjective < 1) {
      adjective_id = ~(int)leaders[leader].adjective + 1;
    }
    else {
      adjective_id = (int)leaders[leader].adjective;
    }
    text_append_string_id(adjective_id);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_adjective_sep_str));
    if (leaders[leader].adjective < 1) {
      adjective_id2 = ~(int)leaders[leader].adjective + 1;
    }
    else {
      adjective_id2 = (int)leaders[leader].adjective;
    }
    text = label_string(adjective_id2);
    len = _strlen(text);
    if (len < 0xb) {
      if (leaders[leader].adjective < 1) {
        adjective_id3 = ~(int)leaders[leader].adjective + 1;
      }
      else {
        adjective_id3 = (int)leaders[leader].adjective;
      }
      text = label_string(adjective_id3);
      adjective_len = _strlen(text);
    }
    else {
      adjective_len = 0xb;
    }
    text_append_spaces(0xb - adjective_len);
    text_append_int_2d((int)leaders[leader].attack);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_attack_sep_str));
    text_append_int_2d((int)leaders[leader].expand);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_expand_sep_str));
    text_append_int_2d((int)leaders[leader].civilize);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_civilize_sep_str));
    crt_strcat(((i8 *)&textbuf.buf),line_tail);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_leader_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  (*(i32 *)&text_file_handle) = saved_handle;
  return 1;
}

/* 004a41d8 auto */
i32 __cdecl tribe_editor_write_caravan(FILE *file)

{
  iptr commodity;
  
  for (commodity = 0; commodity < 0x10; commodity++) {
    text_clear();
    text_append_string_id(commodity_names[commodity]);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&tribe_editor_ctrl_kinds_block.rules_commodity_line_end_str));
    _fputs(((i8 *)&textbuf.buf),file);
  }
  return 1;
}

/* 004a4249 auto */
void __cdecl tribe_editor_on_ok(void)

{
  iptr ok;
  NativeWindow *wnd;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  char msg[32];
  
  tribe_editor_store_controls();
  tribe_editor_update_city_file();
  tribe_editor_apply_tables();
  rules_edit_set_section((iptr)&str_governments_2,(RulesSectionWriterFn)tribe_editor_write_governments);
  rules_edit_add_section((const char *)&str_leaders_2,(RulesSectionWriterFn)tribe_editor_write_leaders);
  rules_edit_add_section((const char *)&str_caravan_5,(RulesSectionWriterFn)tribe_editor_write_caravan);
  ok = rules_write_sections();
  if (ok == 0) {
    _sprintf(msg,str_error_updating_rules_s_2,((i8 *)&lang_file_ext));
    uType = 0x10;
    lpCaption = str_file_i_o_error_2;
    lpText = msg;
    wnd = ((WndBase *)&editor_active.wnd->window)->get_window();
    MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
  }
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  menu_update_states();
  return;
}

/* 004a4304 auto */
void __cdecl tribe_editor_rename_commodity(void)

{
  size_t len;
  iptr parent_wnd2;
  char input_buf[260];
  Dialog dialog2;
  int choice;
  int input_result;
  i8 *eh_prev;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  dialog2.load_text((const char *)&str_debug_11,(const char *)&str_renamecommo,1);
  dialog2.set_width(0xdc);
  dialog2.set_listbox(0x10,0xdc,0);
  for (choice = 0; choice < 0x10; choice++) {
    dialog2.add_option((const char *)(((i8 *)&editor_lists.commodity_names) + choice * 10),choice,0);
  }
  dialog2.option_width = 0xdc;
  choice = dialog2.show_modal(0);
  if (-1 < choice) {
    do {
      input_result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_commoname,9,(iptr)(((i8 *)&editor_lists.commodity_names) + choice * 10),
                                    (iptr)input_buf);
      if (input_result == -1) break;
      len = _strlen(input_buf);
    } while (len == 0);
    if (-1 < input_result) {
      _strncpy(((i8 *)&editor_lists.commodity_names) + choice * 10,input_buf,10);
    }
  }
  dlg_set_parent_window(0);
  return;
}

/* 004a44c6 auto */
void __cdecl tribe_editor_rename_govt(void)

{
  size_t len;
  iptr parent_wnd2;
  char input_buf[260];
  Dialog dialog2;
  int choice;
  int input_result;
  i8 *eh_prev;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  dialog2.load_text((const char *)&str_debug_11,(const char *)&str_renamegovt,1);
  dialog2.set_width(0xdc);
  dialog2.set_listbox(7,0xdc,0);
  for (choice = 0; choice < 7; choice++) {
    dialog2.add_option((const char *)(((i8 *)&editor_lists.govt_names) + choice * 0xf),choice,0);
  }
  dialog2.option_width = 0xdc;
  choice = dialog2.show_modal(0);
  if (-1 < choice) {
    do {
      input_result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_govtname,0xe,(iptr)(((i8 *)&editor_lists.govt_names) + choice * 0xf),
                                    (iptr)input_buf);
      if (input_result == -1) break;
      len = _strlen(input_buf);
    } while (len == 0);
    if (-1 < input_result) {
      _strncpy(((i8 *)&editor_lists.govt_names) + choice * 0xf,input_buf,0xf);
    }
  }
  dlg_set_parent_window(0);
  return;
}

/* 004a468b auto */
void __cdecl tribe_editor_rename_title(void)

{
  int title_slot;
  size_t len;
  char *dest_title;
  i8 *current_title;
  i32 gender_label;
  i8 *title_name;
  iptr parent_wnd2;
  char input_buf[260];
  char option_text[64];
  Dialog dialog2;
  uint choice;
  int input_result;
  i8 *eh_prev;
  
  dialog2.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dialog2.Dialog::destruct());
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  dialog2.load_text((const char *)&str_debug_11,(const char *)&str_renametitle,1);
  dialog2.set_width(400);
  dialog2.set_listbox(0xe,400,0);
  for (choice = 0; (int)choice < 0xe; choice++) {
    if ((choice & 1) == 0) {
      title_slot = ((int)choice >> 1) * 3 + 0x15;
    }
    else {
      title_slot = ((int)choice >> 1) * 3 + 0x2a;
    }
    title_name = ((i8 *)&editor_lists.govt_names) + title_slot * 5;
    if ((choice & 1) == 0) {
      gender_label = (i32)(label_string(label_table.string_ids[0x23f]));
    }
    else {
      gender_label = (i32)(label_string(label_table.string_ids[0x240]));
    }
    _sprintf(option_text,str_s_s_s_3,((i8 *)&editor_lists.govt_names) + ((int)choice >> 1) * 0xf,gender_label,
             title_name);
    dialog2.add_option((const char *)option_text,choice,0);
  }
  dialog2.option_width = 400;
  choice = dialog2.show_modal(0);
  if (-1 < (int)choice) {
    do {
      if ((choice & 1) == 0) {
        title_slot = ((int)choice >> 1) * 3 + 0x15;
      }
      else {
        title_slot = ((int)choice >> 1) * 3 + 0x2a;
      }
      current_title = ((i8 *)&editor_lists.govt_names) + title_slot * 5;
      input_result = dlg_text_input((iptr)&str_debug_11,(const char *)&str_titlename,0xe,(iptr)current_title,(iptr)input_buf);
      if (input_result == -1) break;
      len = _strlen(input_buf);
    } while (len == 0);
    if (-1 < input_result) {
      if ((choice & 1) == 0) {
        title_slot = ((int)choice >> 1) * 3 + 0x15;
      }
      else {
        title_slot = ((int)choice >> 1) * 3 + 0x2a;
      }
      dest_title = ((i8 *)&editor_lists.govt_names) + title_slot * 5;
      _strncpy(dest_title,input_buf,0xf);
    }
  }
  dlg_set_parent_window(0);
  return;
}

/* 004a496c auto */
void __cdecl tribe_editor_on_help(void)

{
  i32 parent_wnd2;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_3,(const char *)&str_tribes);
  dlg_set_parent_window(0);
  return;
}

/* 004a49cb auto */
void __cdecl tribe_editor_on_cancel(void)

{
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 004a49f3 auto */
void __cdecl tribe_editor_on_combo(int ctrl_id)

{
  i32 selection;
  
  if (ctrl_id == 0xc9) {
    tribe_editor_store_controls();
    selection = editor_active.wnd->combos[0].get_cur_sel();
    editor_active.wnd->item = selection;
    tribe_editor_load_controls();
    tribe_editor_redraw();
  }
  else if (ctrl_id == 0xca) {
    tribe_editor_store_controls();
    tribe_editor_redraw();
  }
  return;
}

/* 004a4a58 auto */
void EditorWindow::tribe_editor_create_combo(int slot)

{
  int ctrl_x;
  int ctrl_y;
  iptr label;
  u8 *parent_wnd2;
  int i;
  i8 rect[16];
  
  ctrl_x = tribe_editor_combo_x_table_block.tribe_editor_combo_pos[slot].x + client_x;
  ctrl_y = tribe_editor_combo_x_table_block.tribe_editor_combo_pos[slot].y + client_y;
  if (slot == 0) {
    rect_set_xywh((iptr)rect,ctrl_x - 0x32,ctrl_y,200,font_height_uptr << 3);
  }
  else if (slot < 2) {
    rect_set_xywh((iptr)rect,ctrl_x - 0xf,ctrl_y,0x82,font_height_uptr << 3);
  }
  else {
    rect_set_xywh((iptr)rect,ctrl_x,ctrl_y,100,font_height_uptr << 3);
  }
  ctrl_x = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((ComboBox *)(text_font + slot * 0x38 + 0x28))->create((iptr)parent_wnd2,ctrl_x,
             (iptr)rect);
  ((ComboBox *)(text_font + slot * 0x38 + 0x28))->set_font((iptr)&editor_font_block.editor_font);
  ((ComboBox *)(text_font + slot * 0x38 + 0x28))->set_callback(tribe_editor_on_combo);
  switch(slot) {
  case 0:
    for (i = 0; i < 0x15; i++) {
      ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string(
                 (const char *)editor_text_dlg_wnd_block.editor_item_name_recs[i + 0x2a]);
    }
    break;
  case 1:
    label = (iptr)(label_string(label_table.string_ids[0x20d]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x20e]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x20f]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x210]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    break;
  case 2:
    label = (iptr)(label_string(label_table.string_ids[0x241]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x242]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x243]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    break;
  case 3:
    label = (iptr)(label_string(label_table.string_ids[0x244]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x245]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x246]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    break;
  case 4:
    label = (iptr)(label_string(label_table.string_ids[0x247]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x245]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
    label = (iptr)(label_string(label_table.string_ids[0x248]));
    ((ComboBox *)(text_font + slot * 0x38 + 0x28))->add_string((const char *)label);
  }
  return;
}

/* 004a4eb2 auto */
void EditorWindow::tribe_editor_create_edit(int slot)

{
  int ctrl_id;
  u8 *parent_wnd2;
  i8 rect[16];
  
  rect_set_xywh((iptr)rect,(((i32 *)&tribe_editor_combo_x_table_block.tribe_editor_edit_x_table))[slot * 2] + client_x,
                     (((i32 *)&tribe_editor_combo_x_table_block.tribe_editor_edit_y_table))[slot * 2] + client_y,100,font_height_uptr + 6);
  ctrl_id = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  ((EditControl *)(numeric_edit_controls_area + slot * 0x48 - 0x2c))->create((iptr)parent_wnd2,ctrl_id,
             (iptr)rect,(const char *)&str_empty_114);
  ((EditControl *)(numeric_edit_controls_area + slot * 0x48 - 0x2c))->set_max_len(0x17);
  return;
}

/* 004a4f89 auto */
void EditorWindow::tribe_editor_paint()

{
  byte key_color;
  i8 draw_ctx[16];
  Sprite *sprite;
  int label_y;
  int label_x;
  int box_y;
  int box_x;
  
  this->FrameWindow::draw_caption();
  if (((*(int *)&editor_ui.bg_surface) == 0) || (editor_lists.running == 0)) {
    gfx_fill_rect((iptr)this,(RECT *)&client_rect,0x1d);
  }
  else {
    gfx_copy_rect_xywh((*(int *)&editor_ui.bg_surface),(iptr)this,0,0,client_x,client_y,wnd_width_u,
                       wnd_height_u);
  }
  box_x = client_x + 0x20;
  box_y = client_y + 0x20;
  sprite = (Sprite *)
             (editor_text_dlg_wnd_block.editor_item_recs[editor_active.wnd->item].values[0] * 0x1e0 + (iptr)(((u8 *)&gfx_sheet_surface_block.city_walled_sprites + 300)));
  key_color = sprite->get_key_color();
  gfx_fill_rect_xywh((iptr)this,box_x,box_y,0x40,0x40,(uint)key_color);
  sprite->draw_keyed((iptr)draw_ctx,(iptr)this,0,box_x,box_y + 8);
  gfx_draw_tiled_frame((iptr)this,box_x,box_y,0x40,0x40,6);
  box_x = client_width + client_x - 0x60;
  box_y = client_y + 0x20;
  gfx_fill_rect_xywh((iptr)this,box_x,box_y,0x40,0x40,
                     civ_primary_colors_block.civ_colors[leaders[item_uptr].color].mid);
  gfx_fill_rect_xywh((iptr)this,box_x + 0x10,box_y + 0x10,0x20,0x20,
                     civ_primary_colors_block.civ_colors[leaders[item_uptr].color].primary);
  gfx_draw_tiled_frame((iptr)this,box_x,box_y,0x40,0x40,6);
  text_set_surface((iptr)this);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(10,10,0,0);
  box_x = tribe_editor_combo_x_table_block.tribe_editor_edit_x_table[0] + client_x;
  box_y = tribe_editor_combo_x_table_block.tribe_editor_edit_y_table[0] + client_y;
  gfx_fill_rect_xywh((iptr)this,box_x,box_y,100,font_height_uptr + 6,0x29);
  gfx_draw_frame_rect((iptr)this,box_x,box_y,100,font_height_uptr + 6,10);
  text_clear();
  text_append_str((const char *)editor_text_dlg_wnd_block.editor_item_name_recs[editor_active.wnd->item + 0x3f]);
  text_draw_shadowed((iptr)this,(const char *)&textbuf.buf,box_x + 3,box_y + 2);
  text_set_colors(0x29,0x12,1,1);
  box_x = client_x + 0x59;
  box_y = client_y + 0x109;
  gfx_draw_tiled_frame((iptr)this,box_x,box_y,0x17c,0x45,6);
  label_x = client_x + (int)client_width / 2;
  label_y = client_y + 0x10d;
  text_clear();
  text_append_label(0x25d);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tribe_editor_combo_x_table_block.tribe_editor_city_style_x + client_x + 0x32;
  label_y = ((tribe_editor_combo_x_table_block.tribe_editor_city_style_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x219);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tribe_editor_combo_x_table_block.tribe_editor_edit_x_table[0] + client_x + 0x32;
  label_y = ((tribe_editor_combo_x_table_block.tribe_editor_edit_y_table[0] + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x249);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tribe_editor_combo_x_table_block.tribe_editor_attack_x + client_x + 0x32;
  label_y = ((tribe_editor_combo_x_table_block.tribe_editor_attack_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x1e5);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tribe_editor_combo_x_table_block.tribe_editor_expand_x + client_x + 0x32;
  label_y = ((tribe_editor_combo_x_table_block.tribe_editor_expand_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x24a);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  label_x = tribe_editor_combo_x_table_block.tribe_editor_civilize_x + client_x + 0x32;
  label_y = ((tribe_editor_combo_x_table_block.tribe_editor_civilize_y + client_y) - font_height_uptr) - 2;
  text_clear();
  text_append_label(0x24b);
  text_draw_centered((iptr)this,(const char *)&textbuf.buf,label_x,label_y,0);
  wnd_blit_surface();
  return;
}

/* 004a54d9 auto */
void EditorWindow::tribe_editor_main()

{
  DibSurface *this_00;
  uptr font_height;
  i32 title;
  int pos;
  iptr label;
  int button_y;
  i32 style;
  i32 pos_x;
  i32 pos_y;
  u32 width;
  u32 height;
  i32 arg7;
  i32 arg8;
  i32 arg9;
  u8 *parent_wnd6;
  u8 *parent_wnd5;
  u8 *parent_wnd4;
  u8 *parent_wnd3;
  u8 *parent_wnd2;
  u8 *parent_wnd1;
  DibSurface *bg_surface2;
  int i;
  i8 rect[16];
  GamePalette palette;
  i8 *eh_prev;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (DibSurface *)(operator_new(0x48));
  if (this_00 == (DibSurface *)0x0) {
    bg_surface2 = (DibSurface *)0x0;
  }
  else {
    bg_surface2 = surface_ctor(this_00);
  }
  editor_ui.bg_surface = bg_surface2;
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->set(0,editor_font_size);
  ui_set_default_font((iptr)&editor_font_block.editor_font);
  wnd_width_u = 0x230;
  wnd_height_u = 0x17c;
  item_uptr = 0;
  editor_lists.next_ctrl_id = 0xc9;
  editor_ui.bg_surface->load_gif_file((const char *)&str_editorsa_gif,10,0xc0,(iptr)&palette);
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  arg9 = 0;
  arg8 = 0;
  arg7 = 0;
  height = wnd_height_u;
  width = wnd_width_u;
  pos_y = 0;
  pos_x = 0;
  style = 0xd;
  title = (i32)(label_string(label_table.string_ids[0x24c]));
  this->FrameWindow::create((const char *)title,style,pos_x,pos_y,width,height,arg7,arg8,arg9);
  tribe_editor_load_tables();
  for (i = 0; i < 5; i++) {
    if (tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].kind == 9) {
      tribe_editor_create_edit(tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot);
    }
    else if (tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].kind == 0xc) {
      tribe_editor_create_combo(tribe_editor_ctrl_kinds_block.tribe_editor_ctrl_rows[i].slot);
    }
  }
  pos = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + item_uptr * 0x3c))->width();
  button_width_u = pos + 0x2d;
  pos = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = pos + 8;
  pos = client_y + (int)(button_height_u * 3) / 2 + 0x101;
  height = client_x;
  rect_set_xywh((iptr)rect,height + 0x69,pos,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd1 = (u8 *)0x0;
  }
  else {
    parent_wnd1 = (u8 *)&window;
  }
  label = (iptr)(label_string(label_table.string_ids[0x250]));
  add_trigger_button.create((iptr)parent_wnd1,0x65,(iptr)rect,(const char *)label);
  add_trigger_button.set_callback((CtrlNotifyFn)tribe_editor_rename_commodity);
  rect_set_xywh((iptr)rect,height + 0xe1,pos,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd2 = (u8 *)0x0;
  }
  else {
    parent_wnd2 = (u8 *)&window;
  }
  label = (iptr)(label_string(label_table.string_ids[0x251]));
  edit_trigger_button.create((iptr)parent_wnd2,0x65,(iptr)rect,(const char *)label);
  edit_trigger_button.set_callback((CtrlNotifyFn)tribe_editor_rename_govt);
  rect_set_xywh((iptr)rect,height + 0x159,pos,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd3 = (u8 *)0x0;
  }
  else {
    parent_wnd3 = (u8 *)&window;
  }
  label = (iptr)(label_string(label_table.string_ids[0x252]));
  delete_trigger_button.create((iptr)parent_wnd3,0x65,(iptr)rect,(const char *)label);
  delete_trigger_button.set_callback((CtrlNotifyFn)tribe_editor_rename_title);
  pos = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = pos + 8;
  button_width_u = (int)(client_width - 8) / 3;
  button_y = (client_y + client_height) - (button_height_u + 2);
  pos = client_x + 2;
  rect_set_xywh((iptr)rect,pos,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd4 = (u8 *)0x0;
  }
  else {
    parent_wnd4 = (u8 *)&window;
  }
  label = (iptr)(label_string(label_table.string_ids[0xfe]));
  ok_button.create((iptr)parent_wnd4,0x65,(iptr)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)tribe_editor_on_ok);
  pos = pos + button_width_u + 2;
  rect_set_xywh((iptr)rect,pos,button_y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd5 = (u8 *)0x0;
  }
  else {
    parent_wnd5 = (u8 *)&window;
  }
  label = (iptr)(label_string(label_table.string_ids[0x23b]));
  help_button.create((iptr)parent_wnd5,0x66,(iptr)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)tribe_editor_on_help);
  rect_set_xywh((iptr)rect,pos + button_width_u + 2,button_y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent_wnd6 = (u8 *)0x0;
  }
  else {
    parent_wnd6 = (u8 *)&window;
  }
  label = (iptr)(label_string(label_table.string_ids[0xff]));
  cancel_button.create((iptr)parent_wnd6,0x66,(iptr)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)tribe_editor_on_cancel);
  cancel_button.set_cancel_button();
  window.set_keyboard_nav(0);
  ((ComboBox *)&combos_u)->set_cur_sel(item_uptr);
  tribe_editor_load_controls();
  combo_count_u = 5;
  window.wnd_handlers.set_move_handler((WndMouseFn)editor_wnd_on_move_close_dropdowns);
  set_redraw_callback(tribe_editor_redraw);
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

/* 004a5d92 auto */
void __cdecl tribe_editor_run(void)

{
  EditorWindow editor_wnd;
  
  editor_wnd.construct();
  ON_SCOPE_EXIT(editor_wnd.destruct());
  editor_wnd.tribe_editor_main();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 004cc8f0 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl editor_text_dlg_wnd_global_init(void)

{
  editor_text_dlg_wnd_static_init();
  editor_text_dlg_wnd_reg_atexit();
  return;
}

/* 004cc90a auto */
void __cdecl editor_text_dlg_wnd_static_init(void)

{
  ((FrameWindow *)((i8 *)&editor_text_dlg.wnd))->FrameWindow::construct();
  return;
}

/* 004cc924 auto */
void __cdecl editor_text_dlg_wnd_reg_atexit(void)

{
  _atexit(editor_text_dlg_wnd_static_dtor);
  return;
}

/* 004cc941 auto */
void __cdecl editor_text_dlg_wnd_static_dtor(void)

{
  ((FrameWindow *)((i8 *)&editor_text_dlg.wnd))->FrameWindow::destruct();
  return;
}

/* 004cc95b auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl editor_text_edit_global_init(void)

{
  editor_text_edit_static_init();
  editor_text_edit_reg_atexit();
  return;
}

/* 004cc975 auto */
void __cdecl editor_text_edit_static_init(void)

{
  edit_ctor((EditControl *)((i8 *)&editor_text_edit_block.editor_text_edit));
  return;
}

/* 004cc98f auto */
void __cdecl editor_text_edit_reg_atexit(void)

{
  _atexit(editor_text_edit_static_dtor);
  return;
}

/* 004cc9ac auto */
void __cdecl editor_text_edit_static_dtor(void)

{
  edit_dtor((EditControl *)((i8 *)&editor_text_edit_block.editor_text_edit));
  return;
}

/* 004cc9c6 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E41
    _$E46
    _$E51
    _$E56
     6 names - too many to list
   
   Library: Visual Studio 1998 Debug */

void __cdecl editor_font_global_init(void)

{
  editor_font_static_init();
  editor_font_reg_atexit();
  return;
}

/* 004cc9e0 auto */
void __cdecl editor_font_static_init(void)

{
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->init(0,0x10);
  return;
}

/* 004cc9fe auto */
void __cdecl editor_font_reg_atexit(void)

{
  _atexit(editor_font_static_dtor);
  return;
}

/* 004cca1b auto */
void __cdecl editor_font_static_dtor(void)

{
  ((GameFont *)((i8 *)&editor_font_block.editor_font))->destroy();
  return;
}

/* 004cd8a6 auto */
i32 __cdecl tribe_editor_update_city_file(void)

{
  int rc;
  char *read_ptr;
  size_t _MaxCount;
  int string_id;
  i32 result;
  iptr civ;
  FILE *out_file;
  FILE *in_file;
  char saved_cwd[260];
  iptr leader;
  char line[256];
  i8 city_file_name[16];
  char *tribe_str;
  
  result = 0;
  out_file = (FILE *)0x0;
  in_file = (FILE *)0x0;
  crt_strcpy(city_file_name,str_city_3);
  crt_strcat(city_file_name,((i8 *)&lang_file_ext));
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  rc = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  if (rc != 0) {
    rc = file_exists((const char *)city_file_name);
    if (rc == 0) {
      result = 1;
    }
    else {
      rc = file_exists((const char *)&str_city_tmp);
      if (((rc == 0) || (rc = crt_remove(str_city_tmp_2), rc == 0)) &&
         (rc = crt_rename(city_file_name,str_city_tmp_3), rc == 0)) {
        in_file = file_open_path((const char *)&str_city_tmp_4,(iptr)&str_rt_5);
        out_file = file_open_path((const char *)city_file_name,(iptr)&str_wt_4);
        if ((in_file != (FILE *)0x0) && (out_file != (FILE *)0x0)) {
          for (;;) {
            read_ptr = _fgets(line,0x100,in_file);
            if (read_ptr == (char *)0x0) {
              result = 1;
              break;
            }
            if ((line[0] == '@') && (rc = __strnicmp((line + 1),((i8 *)&city_file_stop_str),4), rc != 0)) {
              for (leader = 0; leader < 0x15; leader++) {
                civ = 1;
                while ((civ < 8 && (civs[civ].tribe != leader))) {
                  civ++;
                }
                if (civ < 8) {
                  tribe_str = civ_names[civ].tribe_name;
                }
                else {
                  if (leaders[leader].plural < 1) {
                    string_id = ~(int)leaders[leader].plural + 1;
                  }
                  else {
                    string_id = (int)leaders[leader].plural;
                  }
                  tribe_str = label_string(string_id);
                }
                _MaxCount = _strlen(tribe_str);
                rc = __strnicmp((line + 1),tribe_str,_MaxCount);
                if (rc == 0) {
                  _sprintf(line,str_c_s,0x40,
                           editor_text_dlg_wnd_block.editor_item_name_recs[leader + 0x2a]);
                  __strupr(line);
                  break;
                }
              }
            }
            /* the line as read, or the tribe's line renamed */
            rc = _fputs(line,out_file);
            if (rc == -1) break;
          }
        }
      }
    }
  }
  if (in_file != (FILE *)0x0) {
    _fclose(in_file);
  }
  if (out_file != (FILE *)0x0) {
    _fclose(out_file);
  }
  rc = file_exists((const char *)&str_city_tmp_5);
  if (rc != 0) {
    crt_remove(str_city_tmp_6);
  }
  __chdir(saved_cwd);
  return result;
}

/* 004cdcf6 auto */
i32 __cdecl editor_numeric_edit_filter(i32 ctrl,int key)

{
  i32 accept;
  
  if ((((key < 0x30) || (0x39 < key)) && (key != 0x2d)) && (key != 0xd6)) {
    accept = 0;
  }
  else {
    accept = 1;
  }
  return accept;
}

/* 004cdd3d auto */
i32 __cdecl editor_options_dialog(const char *file_name,const char *section,uint disabled_mask,i32 flags)

{
  int rc;
  iptr parent_wnd2;
  i32 choice;
  Dialog dlg;
  int i;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  rc = dlg.load_text_section((const char *)&str_game_7,file_name,(int)section,0,0,0,0,flags);
  if (rc == 0) {
    if (((byte)dlg.style_flags & 4) != 0) {
      for (i = 0; i < (int)dlg.option_count; i++) {
        dlg.set_option_checked(i,1 << ((byte)i & 0x1f) & dlg_input.check_bits
                  );
      }
    }
    for (i = 0; i < (int)dlg.option_count; i++) {
      dlg.set_option_disabled(i,
                 (uint)((1 << ((byte)i & 0x1f) & disabled_mask) != 0));
    }
    choice = dlg.show_modal(0);
    (*(u32 *)&dlg_input.help_pressed) = dlg.result_extra;
    if (((byte)dlg.style_flags & 4) != 0) {
      dlg_input.check_bits = 0;
      for (i = 0; i < (int)dlg.option_count; i++) {
        rc = dlg.option_is_checked(i);
        if (rc != 0) {
          dlg_input.check_bits = dlg_input.check_bits | 1 << ((byte)i & 0x1f);
        }
      }
    }
  }
  dlg_set_parent_window(0);
  return choice;
}

/* 004cdf4b auto */
int __cdecl editor_clamp_value(int value,int min_value,int max_value)

{
  if (value < min_value) {
    editor_ui.value_clamped = 1;
  }
  else {
    min_value = value;
    if (max_value < value) {
      editor_ui.value_clamped = 1;
      min_value = max_value;
    }
  }
  return min_value;
}

/* 004cdfa4 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
editor_text_dlg_create(const char *title,uint style,int x,int y,int width,int height,int resize_border,
            int caption_height,int border_width)

{
  iptr parent_wnd2;
  uint wnd_style;
  
  if (title == 0) {
    crt_strcpy(((i8 *)&editor_text_dlg.title),((i8 *)&editor_text_dlg_empty_title));
  }
  else {
    crt_strcpy(((i8 *)&editor_text_dlg.title),(const char *)((const void *)title));
  }
  editor_text_dlg.text_font = ((i32)((i8 *)&ui_text_font));
  editor_text_dlg.title_font = ((i32)((i8 *)&caption_font_block.caption_font));
  editor_text_dlg.title_small_font = ((i32)((i8 *)&caption_small_font_block.caption_small_font));
  editor_text_dlg.style = style;
  editor_text_dlg.caption_height = caption_height;
  editor_text_dlg.border_width = border_width;
  editor_text_dlg.resize_border = resize_border;
  editor_text_dlg.caption_buttons = 0;
  editor_text_dlg.caption_handler = 0;
  if ((style & 4) != 0) {
    editor_text_dlg.caption_height = ui_metrics.caption_height;
    editor_text_dlg.border_width = ui_metrics.border_width;
  }
  if ((style & 8) == 0) {
    wnd_style = 0x202;
  }
  else {
    wnd_style = 0x802;
  }
  if (editor_text_dlg.caption_height != 0) {
    wnd_style = wnd_style | 0x400;
  }
  if (resize_border != 0) {
    wnd_style = wnd_style | 0x1000;
  }
  if ((style & 2) == 0) {
    width += editor_text_dlg.border_width * 2;
    height = height + editor_text_dlg.caption_height + editor_text_dlg.border_width;
  }
  if ((style & 1) != 0) {
    x = (caption_font_block.screen_width >> 1) - (width >> 1);
    y = (caption_font_block.screen_height >> 1) - (height >> 1);
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  ((DibSurface *)((i8 *)&editor_text_dlg.wnd))->wnd_create_with_surface(0,wnd_style,x,y,width,height,(iptr)&game_palette_block.game_palette,
             parent_wnd2);
  if (editor_text_dlg.caption_height != 0) {
    ((WndBase *)((i8 *)&editor_text_dlg.inner))->set_caption_height(editor_text_dlg.caption_height);
  }
  if (editor_text_dlg.resize_border != 0) {
    ((WndBase *)((i8 *)&editor_text_dlg.inner))->set_resize_border(editor_text_dlg.resize_border);
  }
  ((FrameWindow *)((i8 *)&editor_text_dlg.wnd))->layout();
  return;
}

/* 004ce196 auto */
void __cdecl editor_text_dlg_on_ok(void)

{
  int has_line;
  size_t len;
  i32 line_copy;
  int line_idx;
  char line_buf[256];
  
  for (line_idx = 0; line_idx < 0x14; line_idx++) {
    editor_lists.text_lines_out[line_idx] = 0;
  }
  events_compact();
  for (line_idx = 0; line_idx <= 0x13; line_idx++) {
    has_line = ((EditControl *)((i8 *)&editor_text_edit_block.editor_text_edit))->get_line(line_idx,(char *)line_buf,0x100);
    if (has_line == 0) {
      editor_lists.text_lines_out[line_idx] = 0;
    }
    else {
      len = _strlen(line_buf);
      line_copy = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
      editor_lists.text_lines_out[line_idx] = line_copy;
      if (editor_lists.text_lines_out[line_idx] == 0) {
        editor_lists.text_lines_out[line_idx] = 0;
        break;
      }
      crt_strcpy((char *)((void *)editor_lists.text_lines_out[line_idx]),line_buf);
    }
  }
  editor_font_block.editor_text_result = 1;
  editor_font_block.editor_text_active = 0;
  ((ModalMenuWindow *)((i8 *)&editor_text_dlg.inner))->end_modal_loop();
  return;
}

/* 004ce2f3 auto */
void __cdecl editor_text_dlg_on_cancel(void)

{
  editor_font_block.editor_text_result = 0;
  editor_font_block.editor_text_active = 0;
  ((ModalMenuWindow *)((i8 *)&editor_text_dlg.inner))->end_modal_loop();
  return;
}

/* 004ce322 auto */
void __cdecl editor_text_dlg_redraw(void)

{
  ((FrameWindow *)((i8 *)&editor_text_dlg.wnd))->FrameWindow::draw_caption();
  gfx_fill_rect((iptr)&editor_text_dlg.wnd,(RECT *)&editor_text_dlg.fill_rect,0x1d);
  text_set_surface((iptr)&editor_text_dlg.wnd);
  text_set_font((iptr)&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  ((DibSurface *)((i8 *)&editor_text_dlg.wnd))->wnd_blit_surface();
  return;
}

/* 004ce38a auto */
/* WARNING: Removing unreachable block (ram,0x004ce598) */
/* WARNING: Removing unreachable block (ram,0x004ce49e) */
/* WARNING: Removing unreachable block (ram,0x004ce621) */

i32 __cdecl editor_text_edit_dialog(i32 *lines,const char *title)

{
  i32 result;
  int button_y;
  iptr label;
  int line_idx;
  Button cancel_btn;
  int button_height;
  int button_x;
  int button_width2;
  i8 rect[16];
  Button ok_btn;
  
  button_ctor(&ok_btn);
  ON_SCOPE_EXIT(ctrl_dtor(&ok_btn));
  button_ctor(&cancel_btn);
  ON_SCOPE_EXIT(ctrl_dtor(&cancel_btn));
  editor_font_block.editor_text_active = 1;
  editor_lists.text_lines_out = lines;
  button_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height += 8;
  button_y = ((GameFont *)((i8 *)&editor_font_block.editor_font))->line_spacing();
  editor_text_dlg_create(title,0xd,0,0x14,600,button_y * 0x14 + button_height,0,0,0);
  button_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height += 8;
  button_width2 = (editor_text_dlg.client_width - 6) / 2;
  rect_set_xywh((iptr)rect,editor_text_dlg.client_x + 4,editor_text_dlg.client_y + 4,editor_text_dlg.client_width - 9,
                     (editor_text_dlg.client_height - button_height) - 9);
  ((EditControl *)((i8 *)&editor_text_edit_block.editor_text_edit))->create_styled((iptr)&editor_text_dlg.inner,0xc9,(iptr)rect,0,2,1);
  for (line_idx = 0; (line_idx < 0x14 && (lines[line_idx] != 0));
      line_idx = line_idx + 1) {
    if (line_idx != 0) {
      ((EditControl *)((i8 *)&editor_text_edit_block.editor_text_edit))->replace_sel((const char *)&str_empty_122);
    }
    ((EditControl *)((i8 *)&editor_text_edit_block.editor_text_edit))->replace_sel((const char *)lines[line_idx]);
  }
  button_y = (editor_text_dlg.client_y + editor_text_dlg.client_height) - (button_height + 2);
  button_x = editor_text_dlg.client_x + 2;
  rect_set_xywh((iptr)rect,button_x,button_y,button_width2,button_height);
  label = (iptr)(label_string(label_table.string_ids[0xfe]));
  ok_btn.create((iptr)&editor_text_dlg.inner,0x65,(iptr)rect,(const char *)label);
  ok_btn.set_callback((CtrlNotifyFn)editor_text_dlg_on_ok);
  button_x = button_x + button_width2 + 2;
  rect_set_xywh((iptr)rect,button_x,button_y,button_width2,button_height);
  label = (iptr)(label_string(label_table.string_ids[0xff]));
  cancel_btn.create((iptr)&editor_text_dlg.inner,0x66,(iptr)rect,(const char *)label);
  cancel_btn.set_callback((CtrlNotifyFn)editor_text_dlg_on_cancel);
  cancel_btn.set_cancel_button();
  ((SurfaceWindow *)((i8 *)&editor_text_dlg.wnd))->set_redraw_callback(editor_text_dlg_redraw);
  ((SurfaceWindow *)((i8 *)&editor_text_dlg.wnd))->repaint();
  ((SurfaceWindow *)((i8 *)&editor_text_dlg.wnd))->SurfaceWindow::show();
  ((ModalMenuWindow *)((i8 *)&editor_text_dlg.inner))->run_modal_loop();
  while (editor_font_block.editor_text_active != 0) {
    win_pump_one_message();
  }
  ((FrameWindow *)((i8 *)&editor_text_dlg.wnd))->FrameWindow::close();
  result = editor_font_block.editor_text_result;
  return result;
}

/* 004ce71b auto */
uint __cdecl editor_check_city_name(const char *name2)

{
  int rc;
  size_t name_len;
  size_t line_len;
  uint result;
  iptr parent_wnd2;
  size_t cmp_len;
  
  rc = text_open_section((const char *)&str_city_4,0);
  if (rc == 0) {
    for (;;) {
      rc = (int)(text_read_line(1));
      if (rc == 0) break;
      name_len = _strlen((char *)name2);
      line_len = _strlen(((i8 *)&textbuf.buf));
      if (line_len < name_len) {
        cmp_len = _strlen((char *)name2);
      }
      else {
        cmp_len = _strlen(((i8 *)&textbuf.buf));
      }
      rc = __strnicmp((char *)name2,((i8 *)&textbuf.buf),cmp_len);
      if (rc == 0) {
        text_close_file();
        result = 1;
        return result;
      }
    }
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  text_set_string_arg(0,name2);
  rc = popup_debug_text((const char *)&str_nocity);
  dlg_set_parent_window(0);
  result = (uint)(rc == 0);
  return result;
}

/* 004ce83d auto */
void __cdecl editor_wnd_on_move_close_dropdowns(void)

{
  HWND hWnd;
  UINT Msg;
  WPARAM wParam;
  LPARAM lParam;
  int i;
  
  for (i = 0; i < editor_active.wnd->combo_count; i++) {
    lParam = 0;
    wParam = 0;
    Msg = 0x14f;
    hWnd = ((Control *)&editor_active.wnd->combos[i])->get_hwnd();
    SendMessageA(hWnd,Msg,wParam,lParam);
  }
  return;
}

/* 004ce8a4 auto */
i32 __cdecl editor_input_long(const char *file_name,const char *section,int default_value,iptr out_value)

{
  long value;
  char input_buf[260];
  char default_buf[20];
  i32 result;
  
  __itoa(default_value,default_buf,10);
  result = dlg_text_input((iptr)file_name,section,6,(iptr)default_buf,(iptr)input_buf);
  value = _atol(input_buf);
  *(long *)out_value = value;
  return result;
}

/* 00553dfd auto */
i32 __cdecl editor_create_scenario_folder(void)

{
  int rc;
  size_t len;
  DWORD attrs;
  char folder_name[260];
  CHAR path[260];
  int retry;
  
  rc = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
  if (rc == 0) {
    do {
      retry = 0;
      do {
        rc = dlg_text_input((iptr)&str_debug_11,(const char *)&str_newfolder,8,(iptr)&str_empty_168,(iptr)folder_name);
        if (rc < 0) {
          return 0;
        }
        len = _strlen(folder_name);
      } while (len == 0);
      rc = str_find_bad_filename_char((const char *)folder_name);
      if (rc == 0) {
        crt_strcpy(path,((i8 *)&game_dirs.home));
        crt_strcat(path,str_scenario_4);
        crt_strcat(path,folder_name);
        attrs = GetFileAttributesA(path);
        if (attrs != 0xffffffff) {
          text_set_string_arg(0,(const char *)path);
          popup_text((const char *)&str_debug_11,(const char *)&str_direxists);
          retry = 1;
        }
      }
      else {
        text_set_string_arg(0,(const char *)folder_name);
        popup_text((const char *)&str_debug_11,(const char *)&str_badname);
        retry = 1;
      }
    } while (retry != 0);
    crt_mkdir(path);
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),path);
    crt_strcat(path,str_sound_4);
    crt_mkdir(path);
    crt_strcpy(path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcat(path,((i8 *)&scenario_path_sep_str));
    crt_strcat(path,folder_name);
    crt_strcat(path,((i8 *)&scenario_file_ext_str));
    save_write_game((const char *)path,1);
  }
  return 1;
}

/* 00553ff6 auto */
void __cdecl editor_toggle_scenario_flag(void)

{
  int rc;
  
  if (((game.game_options & OPT_CHEAT_MODE) == 0) && ((game.game_flags & GAME_CHEATED) == 0)) {
    rc = popup_game_with_sprite((const char *)&str_reallycheat,(iptr)&popup_info_icon_sprite,0);
    if (rc == 0) {
      return;
    }
    game.game_flags = game.game_flags | GAME_CHEATED;
    game.game_options = game.game_options | OPT_CHEAT_MODE;
    rc = editor_create_scenario_folder();
    if (rc != 0) {
      popup_text((const char *)&str_help_15,(const char *)&str_warning_3);
      game.game_flags = game.game_flags | GAME_SCENARIO;
    }
  }
  else if (((game.game_options & OPT_CHEAT_MODE) == 0) ||
          (((game.game_flags & GAME_SCENARIO) == 0 ||
           (rc = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)), rc == 0)))) {
    rc = _strcmp(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
    if (rc == 0) {
      popup_text((const char *)&str_help_16,(const char *)&str_warning_4);
    }
    rc = editor_create_scenario_folder();
    if (rc == 0) {
      return;
    }
    game.game_options = game.game_options | OPT_CHEAT_MODE;
    game.game_flags = game.game_flags | GAME_SCENARIO;
  }
  else {
    game.game_flags = game.game_flags & ~GAME_SCENARIO;
  }
  menu_update_states();
  return;
}
