/* The game, multiplayer: the parley and chat window (its controls, offers and the treaties made): functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004923f0 auto */
void __cdecl chat_insert_macro_text(EditControl *edit,i32 macro_id)

{
  bool all_contact;
  int civ;
  iptr name2;
  size_t text_len;
  int tries;
  
  text_clear();
  tries = 0;
  if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (civ = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), civ == 0))
     && (game.reveal_map == '\0')) {
    all_contact = false;
  }
  else {
    all_contact = true;
  }
  switch(macro_id) {
  case 0x2b0:
    while (((((civ = chat_macros.next_adj_civ, tries < 8 && (human_civ[0] != chat_macros.next_adj_civ)) && (!all_contact))
            && ((mp_options.chat_from_start == 0 ||
                ((1 << ((byte)chat_macros.next_adj_civ & 0x1f) & (uint)game.human_players) == 0)))) &&
           ((*(byte *)&civs[chat_macros.next_adj_civ].treaties[human_civ[0]] & TREATY_CONTACT) == 0))) {
      tries++;
      chat_macros.next_adj_civ++;
      if (chat_macros.next_adj_civ == 8) {
        chat_macros.next_adj_civ = 1;
      }
    }
    if (tries == 8) {
      return;
    }
    chat_macros.next_adj_civ++;
    name2 = civ_adjective(civ);
    text_append((const char *)name2);
    text_append_space();
    if (chat_macros.next_adj_civ == 8) {
      chat_macros.next_adj_civ = 1;
    }
    break;
  case 0x2b1:
    while ((((tries < 8 && (human_civ[0] != chat_macros.next_leader_civ)) &&
            ((!all_contact &&
             ((mp_options.chat_from_start == 0 ||
              ((1 << ((byte)chat_macros.next_leader_civ & 0x1f) & (uint)game.human_players) == 0)))))) &&
           ((*(byte *)&civs[chat_macros.next_leader_civ].treaties[human_civ[0]] & TREATY_CONTACT) == 0))) {
      tries++;
      chat_macros.next_leader_civ++;
      if (chat_macros.next_leader_civ == 8) {
        chat_macros.next_leader_civ = 1;
      }
    }
    if (tries == 8) {
      return;
    }
    name2 = civ_leader_title(chat_macros.next_leader_civ);
    text_append((const char *)name2);
    text_append_space();
    civ = chat_macros.next_leader_civ;
    chat_macros.next_leader_civ++;
    name2 = (iptr)(civ_leader_name(civ));
    text_append((const char *)name2);
    text_append_space();
    if (chat_macros.next_leader_civ == 8) {
      chat_macros.next_leader_civ = 1;
    }
    break;
  case 0x2b2:
    chat_read_macro_line((const char *)&str_chatmac1_txt,&chat_macros.macro1_offset);
    civ = chat_macros.macro1_offset;
    if (civ == 0) {
      return;
    }
    break;
  case 0x2b3:
    chat_read_macro_line((const char *)&str_chatmac2_txt,&chat_macros.macro2_offset);
    civ = chat_macros.macro2_offset;
    if (civ == 0) {
      return;
    }
    break;
  case 0x2b4:
    chat_read_macro_line((const char *)&str_chatmac3_txt,&chat_macros.macro3_offset);
    civ = chat_macros.macro3_offset;
    if (civ == 0) {
      return;
    }
    break;
  }
  edit->replace_sel((const char *)&textbuf.buf);
  civ = edit->get_sel_end();
  text_len = _strlen(((i8 *)&textbuf.buf));
  edit->set_sel(civ - text_len,civ);
  return;
}

/* 0049275a auto */
void __cdecl chat_read_macro_line(const char *path,i32 *file_offset)

{
  int ok;
  size_t line_len;
  i8 file[160];
  
  ((FileObject *)file)->init();
  ON_SCOPE_EXIT(((FileObject *)&file)->close());
  ok = ((FileObject *)file)->open(path);
  if (ok == 0) {
    *file_offset = 0;
  }
  else {
    ok = ((FileObject *)file)->get_size();
    if (ok <= *file_offset) {
      *file_offset = 0;
    }
    ok = ((FileObject *)file)->seek_cur(*file_offset);
    if (ok == 0) {
      *file_offset = 0;
      ((FileObject *)file)->map_close();
    }
    else {
      ((FileObject *)file)->read_line((char *)&textbuf.buf,0x100);
      ((FileObject *)file)->map_close();
      line_len = _strlen(((i8 *)&textbuf.buf));
      *(size_t *)file_offset = *file_offset + line_len + 2;
    }
  }
  return;
}

/* 004b401c auto */
void __cdecl parley_wnd_global_init(void)

{
  parley_wnd_static_init();
  parley_wnd_reg_atexit();
  return;
}

/* 004b4036 auto */
void __cdecl parley_wnd_static_init(void)

{
  parley_wnd_block.construct((RECT *)&menu_prefs.parley_wnd_rect);
  return;
}

/* 004b405b auto */
void __cdecl parley_wnd_reg_atexit(void)

{
  _atexit(parley_wnd_static_dtor);
  return;
}

/* 004b4078 auto */
void __cdecl parley_wnd_static_dtor(void)

{
  parley_wnd_block.destruct();
  return;
}

/* 004b4092 auto */
void __cdecl chat_wnd_global_init(void)

{
  chat_wnd_static_init();
  chat_wnd_reg_atexit();
  return;
}

/* 004b40ac auto */
void __cdecl chat_wnd_static_init(void)

{
  chat_wnd_block.construct((RECT *)&menu_prefs.chat_wnd_rect);
  return;
}

/* 004b40d1 auto */
void __cdecl chat_wnd_reg_atexit(void)

{
  _atexit(chat_wnd_static_dtor);
  return;
}

/* 004b40ee auto */
void __cdecl chat_wnd_static_dtor(void)

{
  chat_wnd_block.destruct();
  return;
}

/* 004b4108 auto */
ParleyWindow *ParleyWindow::construct(RECT *rect)

{
  u8 *field;
  int width;
  int i;
  
  this->SurfaceWindow::construct();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 1);
  init_rect = (u32)rect;
  closed = 1;
  prefs_load_civ2dat();
  width = rect_width((RECT *)init_rect);
  if (width < 1) {
    wnd_x = 0;
    wnd_y = 0;
  }
  else {
    wnd_x = *(u32 *)init_rect;
    wnd_y = (u32)((RECT *)init_rect)->top;
    if (width < 0x321) {
      large_layout = 0;
    }
    else {
      large_layout = 1;
    }
  }
  close_button = 0;
  send_button = 0;
  clear_button = 0;
  conceal_button = 0;
  reveal_button = 0;
  mode_button = 0;
  intel_button = 0;
  mode2_button = 0;
  never_mind_button = 0;
  accept_button = 0;
  no_thanks_button = 0;
  counter_button = 0;
  for (i = 0; i < 8; i++) {
    field = (u8 *)&send_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&mute_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&send_to_civs[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&muted_civs[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  remain_anonymous = 0;
  mute_anonymous = 0;
  broadcast_all = 0;
  broadcast_checkbox = 0;
  anonymous_checkbox = 0;
  mute_anonymous_checkbox = 0;
  desc_edit = 0;
  other_civ = 0xffffffff;
  received_offer = 0;
  exchange_kind[0] = 0;
  exchange_kind[1] = 1;
  demand_kind = 0;
  selected_action = 0xffffffff;
  gift_kind = 0;
  map_choice[1] = 0;
  map_choice[0] = map_choice[1];
  gold_amount[1] = 0;
  gold_amount[0] = gold_amount[1];
  proposal_kind = 0;
  treaty_kind = 0;
  side_selected[1] = 0;
  side_selected[0] = side_selected[1];
  caption_buttons[0].hotspot = 0;
  caption_buttons[1].hotspot = 0;
  caption_buttons[2].hotspot = 0;
  chat_input_edit = 0;
  chat_log_edit = 0;
  (*(i32 *)&chat.input_buffer) = 0;
  (*(i32 *)&chat.log_buffer) = 0;
  font_size16_bold_block.chat_log_capacity = 0;
  for (i = 0; i < 2; i++) {
    field = (u8 *)&side_checkboxes[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&side_radios[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&gold_edits[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&list_scrollbars[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&list_hotspots[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&sort_by_unit_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&sort_by_city_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&info_show_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
    field = (u8 *)&info_hide_buttons[i];
    field[0] = '\0';
    field[1] = '\0';
    field[2] = '\0';
    field[3] = '\0';
  }
  remote_units_revealed = '\0';
  local_units_revealed = '\0';
  local_cities_revealed = '\0';
  remote_cities_revealed = '\0';
  return this;
}

/* 004b4593 auto */
void ParleyWindow::destruct()

{
  int created;
  size_t log_len;
  FileObject log_file;
  uint unwind_state;
  
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 1);
  unwind_state = 0;
  if (mode == 4) {
    log_file.init();
    unwind_state = (unwind_state & 0xffffff00 | (byte)(1));
    if ((menu_prefs.clear_chat_on_boot != 0) && (chat.log_buffer != (char *)0x0)) {
      *chat.log_buffer = '\0';
      created = log_file.create((const char *)&str_chatlog_txt_2);
      if (created != 0) {
        log_len = _strlen(chat.log_buffer);
        log_file.write(chat.log_buffer,log_len);
        log_file.map_close();
      }
    }
    if (chat.log_buffer != (char *)0x0) {
      operator_delete(chat.log_buffer);
      chat.log_buffer = (char *)0x0;
    }
    if (chat.input_buffer != (void *)0x0) {
      operator_delete(chat.input_buffer);
      chat.input_buffer = (void *)0x0;
    }
    unwind_state = unwind_state & 0xffffff00;
    /* inlined unwind funclet 004b4705 */ { log_file.close(); }
  }
  destroy_controls();
  wnd_destroy_with_surface();
  unwind_state = 0xffffffff;
  /* inlined unwind funclet 004b4711 */ { ((SurfaceWindow *)this)->SurfaceWindow::destruct(); }
  return;
}

/* 004b4735 auto */
void ParleyWindow::create(u32 mode)

{
  int file_size;
  i32 height;
  i32 width;
  i8 *palette;
  i8 *parent;
  u8 *wnd2;
  i8 log_file[160];
  
  closed = 1;
  if (this == (ParleyWindow *)0x0) {
    wnd2 = (u8 *)0x0;
  }
  else {
    wnd2 = ((u8 *)this + 0x48);
  }
  wnd_set_active_child((iptr)wnd2);
  this->mode = mode;
  if (this->mode == 4) {
    (*(UINT *)&font_size16_bold_block.chat_log_capacity) = GetPrivateProfileIntA
                             (str_civilization_gold_7,str_chatshowsize,-1,
                              str_civ_ini_7);
    if (((*(UINT *)&font_size16_bold_block.chat_log_capacity) == 0xffffffff) || ((int)(*(UINT *)&font_size16_bold_block.chat_log_capacity) < 0x2000)) {
      WritePrivateProfileStringA
                (str_civilization_gold_8,str_chatshowsize_2,((i8 *)&chat_show_size_default_str),str_civ_ini_8
                );
      (*(UINT *)&font_size16_bold_block.chat_log_capacity) = 0x2000;
    }
    else if (0xdfff < (int)(*(UINT *)&font_size16_bold_block.chat_log_capacity)) {
      WritePrivateProfileStringA
                (str_civilization_gold_9,str_chatshowsize_3,str_57344,
                 str_civ_ini_9);
      (*(UINT *)&font_size16_bold_block.chat_log_capacity) = 0xe000;
    }
    if ((*&chat.log_buffer) == (i8 *)0x0) {
      (*&chat.log_buffer) = (i8 *)(operator_new((*(UINT *)&font_size16_bold_block.chat_log_capacity) + 1));
    }
    _memset((*&chat.log_buffer),0,(*(UINT *)&font_size16_bold_block.chat_log_capacity) + 1);
    if (chat.input_buffer == (void *)0x0) {
      chat.input_buffer = operator_new(0x101);
    }
    _memset(chat.input_buffer,0,0x101);
    ((FileObject *)log_file)->init();
    ON_SCOPE_EXIT(((FileObject *)&log_file)->close());
    if (menu_prefs.clear_chat_on_new_game == 0) {
      file_size = ((FileObject *)log_file)->open((const char *)&str_chatlog_txt_3);
      if (file_size != 0) {
        file_size = ((FileObject *)log_file)->get_size();
        if ((int)(*(UINT *)&font_size16_bold_block.chat_log_capacity) < file_size) {
          ((FileObject *)log_file)->seek_cur(file_size - (*(UINT *)&font_size16_bold_block.chat_log_capacity));
        }
        ((FileObject *)log_file)->read(*&chat.log_buffer,(*(UINT *)&font_size16_bold_block.chat_log_capacity));
        ((FileObject *)log_file)->map_close();
      }
      else {
        *(*&chat.log_buffer) = 0;
      }
    }
    else {
      *(*&chat.log_buffer) = 0;
    }
  }
  if ((wnd_x == 0) && (wnd_y == 0)) {
    pick_size_mode();
  }
  crt_strcpy(&title[0],((i8 *)&parley_wnd_empty_title_str));
  if (this->mode == 4) {
    parent = ((i8 *)&main_frame_wnd_block.main_frame_wnd);
    palette = ((i8 *)&game_palette_block.game_palette);
    height = rect_height((RECT *)init_rect);
    width = rect_width((RECT *)init_rect);
    wnd_create_with_surface(0,0x602,*(i32 *)init_rect,
               ((RECT *)init_rect)->top,width,height,(GamePalette *)palette,(WndBase *)parent);
  }
  else {
    parent = ((i8 *)&main_frame_wnd_block.main_frame_wnd);
    palette = ((i8 *)&game_palette_block.game_palette);
    height = rect_height((RECT *)init_rect);
    width = rect_width((RECT *)init_rect);
    wnd_create_with_surface(0,0x602,*(i32 *)init_rect,
               ((RECT *)init_rect)->top,width,height,(GamePalette *)palette,(WndBase *)parent);
  }
  calc_rect();
  window.move_to_rect((RECT *)init_rect);
  built_layout = 0xffffffff;
  left_kind = parley_panes.mode3_left_kind;
  right_kind = parley_panes.mode3_right_kind;
  shown_left_kind = 0x15;
  shown_right_kind = 0x15;
  window.wnd_handlers.set_close_handler(diplo_wnd_close_active);
  set_redraw_callback(parley_wnd_redraw);
  window.wnd_handlers.set_move_handler((WndMouseFn)parley_wnd_on_move);
  window.wnd_handlers.set_keydown_handler(parley_wnd_on_key);
  calc_layout();
  sync_sides();
  if (((this->mode == 3) || (this->mode == 2)) || (this->mode == 1)) {
    ((Button *)never_mind_button)->disable();
  }
  update_buttons();
  return;
}

/* 004b4c09 auto */
void __cdecl parley_wnd_on_move(void)

{
  iptr active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  wnd2->window.get_rect_in_parent((RECT *)wnd2->init_rect);
  wnd2->wnd_x = *(i32 *)wnd2->init_rect;
  wnd2->wnd_y = (i32)((RECT *)wnd2->init_rect)->top;
  return;
}

/* 004b4c81 auto */
void ParleyWindow::pick_size_mode()

{
  iptr active;
  ParleyWindow *wnd2;
  
  active = wnd_get_active_child();
  if (active == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(active - 0x48);
  }
  wnd2->large_layout = 0;
  if (999 < caption_font_block.screen_width) {
    wnd2->large_layout = 1;
  }
  calc_rect();
  return;
}

/* 004b4cf0 auto */
void ParleyWindow::calc_rect()

{
  int size;
  int font_height;
  int height2;
  int width2;
  u32 top;
  u32 left;
  
  if (large_layout == 0) {
    width2 = 0x208;
  }
  else {
    width2 = 800;
  }
  if (large_layout == 0) {
    height2 = 0x14f;
  }
  else {
    height2 = 0x1cc;
  }
  if (large_layout == 0) {
    font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + font_height;
  width2 = width2 + (panel_layout.padding_x + panel_layout.margin_x) * 2 + 6;
  height2 = height2 + panel_layout.margin_y * 2 + caption_height * 2 + 0xe;
  size = rect_width((RECT *)init_rect);
  if (size == 0) {
    size = main_frame_wnd_block.main_frame_wnd.client_width();
    wnd_x = (size >> 1) - (width2 >> 1);
    left = wnd_x;
    size = main_frame_wnd_block.main_frame_wnd.client_height();
    wnd_y = (size >> 1) - (height2 >> 1);
    top = wnd_y;
  }
  else {
    left = wnd_x;
    top = wnd_y;
  }
  SetRect((LPRECT)init_rect,left,top,width2 + left,height2 + top);
  window.set_caption_height(caption_height);
  return;
}

/* 004b4e8a auto */
void ParleyWindow::calc_layout()

{
  u32 dim;
  int font_height;
  
  dim = get_width();
  width = dim;
  dim = get_height();
  height = dim;
  if (large_layout == 0) {
    font_height = ((GameFont *)((i8 *)&font_size20))->font_cached_height();
  }
  else {
    font_height = ((GameFont *)((i8 *)&font_size30))->font_cached_height();
  }
  caption_height = panel_layout.margin_y * 2 + panel_layout.bevel_width * 2 + font_height;
  SetRect((LPRECT)&client_left,0,0,width,height);
  SetRect((LPRECT)&panel_left,0,0,width + (panel_layout.padding_x + panel_layout.margin_x) * -2,
          height - (panel_layout.margin_y * 2 + caption_height * 2 + panel_layout.padding_x));
  OffsetRect((LPRECT)&panel_left,panel_layout.padding_x + panel_layout.margin_x,caption_height + panel_layout.margin_y);
  return;
}

/* 004b4fb2 auto */
void ParleyWindow::sync_sides()

{
  if (built_layout == large_layout) {
    if (shown_left_kind != left_kind) {
      add_side_dialog(left_kind,0);
      shown_left_kind = left_kind;
    }
    if (shown_right_kind != right_kind) {
      add_side_dialog(right_kind,1);
      shown_right_kind = right_kind;
    }
  }
  else {
    destroy_controls();
    create_controls();
    add_side_dialog(left_kind,0);
    add_side_dialog(right_kind,1);
    shown_left_kind = left_kind;
    shown_right_kind = right_kind;
    built_layout = large_layout;
  }
  return;
}

/* 004b50cf auto */
void ParleyWindow::destroy_controls()

{
  u8 *slot;
  int i;
  
  if (received_offer != 0) {
    operator_delete((void *)received_offer);
    received_offer = 0;
  }
  for (i = 0; i < 3; i++) {
    if (caption_buttons[i].hotspot != 0) {
      if ((Hotspot *)caption_buttons[i].hotspot != (Hotspot *)0x0) {
        ((Hotspot *)caption_buttons[i].hotspot)->scalar_destruct(1);
      }
      slot = (u8 *)&caption_buttons[i].hotspot;
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  if (close_button != 0) {
    if ((Button *)close_button != (Button *)0x0) {
      ((Button *)close_button)->scalar_destruct(1);
    }
    close_button = 0;
  }
  if (send_button != 0) {
    if ((Button *)send_button != (Button *)0x0) {
      ((Button *)send_button)->scalar_destruct(1);
    }
    send_button = 0;
  }
  if (clear_button != 0) {
    if ((Button *)clear_button != (Button *)0x0) {
      ((Button *)clear_button)->scalar_destruct(1);
    }
    clear_button = 0;
  }
  if (reveal_button != 0) {
    if ((Button *)reveal_button != (Button *)0x0) {
      ((Button *)reveal_button)->scalar_destruct(1);
    }
    reveal_button = 0;
  }
  if (conceal_button != 0) {
    if ((Button *)conceal_button != (Button *)0x0) {
      ((Button *)conceal_button)->scalar_destruct(1);
    }
    conceal_button = 0;
  }
  if (mode_button != 0) {
    if ((Button *)mode_button != (Button *)0x0) {
      ((Button *)mode_button)->scalar_destruct(1);
    }
    mode_button = 0;
  }
  if (intel_button != 0) {
    if ((Button *)intel_button != (Button *)0x0) {
      ((Button *)intel_button)->scalar_destruct(1);
    }
    intel_button = 0;
  }
  if (mode2_button != 0) {
    if ((Button *)mode2_button != (Button *)0x0) {
      ((Button *)mode2_button)->scalar_destruct(1);
    }
    mode2_button = 0;
  }
  if (never_mind_button != 0) {
    if ((Button *)never_mind_button != (Button *)0x0) {
      ((Button *)never_mind_button)->scalar_destruct(1);
    }
    never_mind_button = 0;
  }
  if (accept_button != 0) {
    if ((Button *)accept_button != (Button *)0x0) {
      ((Button *)accept_button)->scalar_destruct(1);
    }
    accept_button = 0;
  }
  if (no_thanks_button != 0) {
    if ((Button *)no_thanks_button != (Button *)0x0) {
      ((Button *)no_thanks_button)->scalar_destruct(1);
    }
    no_thanks_button = 0;
  }
  if (counter_button != 0) {
    if ((Button *)counter_button != (Button *)0x0) {
      ((Button *)counter_button)->scalar_destruct(1);
    }
    counter_button = 0;
  }
  if (chat_log_edit != 0) {
    if ((Button *)chat_log_edit != (Button *)0x0) {
      ((EditControl *)chat_log_edit)->scalar_destruct(1);
    }
    chat_log_edit = 0;
  }
  if (chat_input_edit != 0) {
    if ((Button *)chat_input_edit != (Button *)0x0) {
      ((EditControl *)chat_input_edit)->scalar_destruct(1);
    }
    chat_input_edit = 0;
  }
  if (anonymous_checkbox != 0) {
    if ((Checkbox *)anonymous_checkbox != (Checkbox *)0x0) {
      ((Checkbox *)anonymous_checkbox)->scalar_destruct(1);
    }
    anonymous_checkbox = 0;
  }
  if (mute_anonymous_checkbox != 0) {
    if ((Checkbox *)mute_anonymous_checkbox != (Checkbox *)0x0) {
      ((Checkbox *)mute_anonymous_checkbox)->scalar_destruct(1);
    }
    mute_anonymous_checkbox = 0;
  }
  if (desc_edit != 0) {
    if ((Button *)desc_edit != (Button *)0x0) {
      ((EditControl *)desc_edit)->scalar_destruct(1);
    }
    desc_edit = 0;
  }
  if (broadcast_checkbox != 0) {
    if ((Checkbox *)broadcast_checkbox != (Checkbox *)0x0) {
      ((Checkbox *)broadcast_checkbox)->scalar_destruct(1);
    }
    broadcast_checkbox = 0;
  }
  for (i = 0; i < 8; i++) {
    if (send_checkboxes[i] != 0) {
      if ((Checkbox *)send_checkboxes[i] != (Checkbox *)0x0) {
        ((Checkbox *)send_checkboxes[i])->scalar_destruct(1);
      }
      slot = (u8 *)&send_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (mute_checkboxes[i] != 0) {
      if ((Checkbox *)mute_checkboxes[i] != (Checkbox *)0x0) {
        ((Checkbox *)mute_checkboxes[i])->scalar_destruct(1);
      }
      slot = (u8 *)&mute_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  for (i = 0; i < 2; i++) {
    if (side_checkboxes[i] != 0) {
      if ((Checkbox *)side_checkboxes[i] != (Checkbox *)0x0) {
        ((Checkbox *)side_checkboxes[i])->scalar_destruct(1);
      }
      slot = (u8 *)&side_checkboxes[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (side_radios[i] != 0) {
      if ((RadioGroup *)side_radios[i] != (RadioGroup *)0x0) {
        ((RadioGroup *)side_radios[i])->scalar_destruct(1);
      }
      slot = (u8 *)&side_radios[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (gold_edits[i] != 0) {
      if ((Button *)gold_edits[i] != (Button *)0x0) {
        ((EditControl *)gold_edits[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&gold_edits[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (list_scrollbars[i] != 0) {
      if ((Button *)list_scrollbars[i] != (Button *)0x0) {
        ((Scrollbar *)list_scrollbars[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&list_scrollbars[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (list_hotspots[i] != 0) {
      if ((Hotspot *)list_hotspots[i] != (Hotspot *)0x0) {
        ((Hotspot *)list_hotspots[i])->scalar_destruct(1);
      }
      slot = (u8 *)&list_hotspots[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (sort_by_unit_buttons[i] != 0) {
      if ((Button *)sort_by_unit_buttons[i] != (Button *)0x0) {
        ((Button *)sort_by_unit_buttons[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&sort_by_unit_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (sort_by_city_buttons[i] != 0) {
      if ((Button *)sort_by_city_buttons[i] != (Button *)0x0) {
        ((Button *)sort_by_city_buttons[i])->scalar_destruct(1)
        ;
      }
      slot = (u8 *)&sort_by_city_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (info_show_buttons[i] != 0) {
      if ((Button *)info_show_buttons[i] != (Button *)0x0) {
        ((Button *)info_show_buttons[i])->scalar_destruct(1);
      }
      slot = (u8 *)&info_show_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
    if (info_hide_buttons[i] != 0) {
      if ((Button *)info_hide_buttons[i] != (Button *)0x0) {
        ((Button *)info_hide_buttons[i])->scalar_destruct(1);
      }
      slot = (u8 *)&info_hide_buttons[i];
      slot[0] = '\0';
      slot[1] = '\0';
      slot[2] = '\0';
      slot[3] = '\0';
    }
  }
  return;
}

/* Part of ParleyWindow::create_controls (004b5c93): the three caption
   buttons: the sprite, size and place of each. */
void ParleyWindow::create_caption_buttons()
{
  u8 *slot_ptr;
  i32 sprite_dim;
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  u8 *caption_parent;
  i32 hotspot;
  int caption_x;
  int i;
  tagRECT rect;
  int ctrl_height;
  
  caption_x = panel_layout.padding_x;
  for (i = 0; i < 3; i++) {
    if (i == 0) {
      caption_buttons[0].sprite = (Sprite *)((i8 *)&caption_close_button_sprite_block.caption_close_button_sprite);
    }
    else if (i == 1) {
      caption_buttons[1].sprite = (Sprite *)((i8 *)&caption_zoom_out_sprite);
    }
    else {
      caption_buttons[i].sprite = (Sprite *)((i8 *)&caption_zoom_out_button_sprite_block.caption_zoom_out_button_sprite);
    }
    caption_buttons[i].id = i + 1;
    sprite_dim = caption_buttons[i].sprite->width();
    caption_buttons[i].width = sprite_dim;
    sprite_dim = caption_buttons[i].sprite->height();
    caption_buttons[i].height = sprite_dim;
    slot_ptr = (u8 *)&caption_buttons[i].hotspot;
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
    ctrl_width = (int)(caption_buttons[i].width * caption_height) / 0x18;
    ctrl_height = (int)(caption_buttons[i].height * caption_height) / 0x18;
    ctrl_left = ((int)caption_height >> 1) - (ctrl_height >> 1);
    SetRect(&rect,caption_x,ctrl_left,ctrl_width + caption_x,ctrl_left + ctrl_height);
    new_ctrl = (Button *)(operator_new(0x40));
    if (new_ctrl == (Button *)0x0) {
      hotspot = 0;
    }
    else {
      hotspot = (i32)(hotspot_ctor((Hotspot *)new_ctrl));
    }
    caption_buttons[i].hotspot = hotspot;
    if (this == (ParleyWindow *)0x0) {
      caption_parent = (u8 *)0x0;
    }
    else {
      caption_parent = ((u8 *)this + 0x48);
    }
    ((Hotspot *)caption_buttons[i].hotspot)->create((Wnd *)caption_parent,
               caption_buttons[i].id + 1000,&rect);
    ((Hotspot *)caption_buttons[i].hotspot)->set_click_callback(parley_wnd_on_caption_click);
    caption_x = caption_x + ctrl_width + panel_layout.margin_x;
  }
}

/* Part of ParleyWindow::create_controls (004b5c93): the Close button across
   the bottom. */
void ParleyWindow::create_close_button()
{
  u32 wnd_width;
  int y_delta;
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  int yTop;
  u8 *close_parent;
  iptr close_label;
  int close_line_h2;
  int close_line_h;
  uptr close_btn;
  i8 *button_font;
  tagRECT rect;
  int ctrl_height;
  
  if (large_layout == 0) {
    button_font = ((i8 *)&font_size16);
  }
  else {
    button_font = ((i8 *)&font_size24);
  }
  ui_set_button_font((GameFont *)button_font);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    close_btn = 0;
  }
  else {
    close_btn = (uptr)(button_ctor(new_ctrl));
  }
  close_button = close_btn;
  ctrl_width = panel_layout.bevel_width + panel_layout.margin_x + 2;
  if (large_layout == 0) {
    close_line_h = ((GameFont *)((i8 *)&font_size16))->line_spacing();
  }
  else {
    close_line_h = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  }
  yTop = height - ((panel_layout.margin_y + panel_layout.padding_y) * 2 + close_line_h);
  wnd_width = width;
  y_delta = panel_layout.margin_x * 2;
  ctrl_left = panel_layout.bevel_width * 2;
  if (large_layout == 0) {
    close_line_h2 = ((GameFont *)((i8 *)&font_size16))->line_spacing();
  }
  else {
    close_line_h2 = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  }
  ctrl_height = panel_layout.padding_y * 2 + close_line_h2;
  SetRect(&rect,ctrl_width,yTop,(wnd_width - (y_delta + ctrl_left + 5)) + ctrl_width,yTop + ctrl_height);
  if (mode == 4) {
    close_label = (iptr)(label_string(label_table.string_ids[0x2cb]));
  }
  else {
    close_label = (iptr)(label_string(label_table.string_ids[0x2cc]));
  }
  if (this == (ParleyWindow *)0x0) {
    close_parent = (u8 *)0x0;
  }
  else {
    close_parent = ((u8 *)this + 0x48);
  }
  ((Button *)close_button)->create((Wnd *)close_parent,0x3ed,&rect,(const char *)close_label);
  ((Button *)close_button)->set_callback(diplo_wnd_escape_close);
}

/* Part of ParleyWindow::create_controls (004b5c93): the chat history and
   input edits with the Send and Clear buttons; leaves ctrl_top and
   ctrl_height for the row of buttons above them. */
void ParleyWindow::create_chat_controls(int &ctrl_top,int &ctrl_height)
{
  int y_delta;
  LONG xLeft;
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  EditControl *new_edit;
  iptr label;
  int footer_line_h;
  u8 *clear_parent;
  u8 *send_parent;
  u8 *input_parent;
  int input_font_h4;
  int input_font_h3;
  int input_font_h2;
  int input_font_h1;
  u8 *history_parent;
  int hist_font_h4;
  int hist_font_h3;
  int hist_font_h2;
  int hist_font_h1;
  uptr clear_button;
  uptr send_button;
  i8 *input_font;
  uptr input_edit;
  i8 *history_font;
  uptr history_edit;
  tagRECT rect;
  i32 input_style;
  
  new_edit = (EditControl *)(operator_new(0x48));
  if (new_edit == (EditControl *)0x0) {
    history_edit = 0;
  }
  else {
    history_edit = (uptr)(edit_ctor(new_edit));
  }
  chat_log_edit = history_edit;
  ctrl_left = panel_left + panel_layout.margin_x;
  ctrl_width = rect_width((RECT *)&panel_left);
  ctrl_width += panel_layout.margin_x * -2;
  if (mode == 4) {
    if (large_layout == 0) {
      hist_font_h3 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h3 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_top = panel_bottom + (panel_layout.margin_y + hist_font_h3) * -9;
    if (large_layout == 0) {
      hist_font_h4 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h4 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = hist_font_h4 * 9 + panel_layout.margin_y * 8;
  }
  else {
    if (large_layout == 0) {
      hist_font_h1 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h1 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_top = panel_bottom + (panel_layout.margin_y + hist_font_h1) * -6;
    if (large_layout == 0) {
      hist_font_h2 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      hist_font_h2 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = panel_layout.margin_y * 5 + hist_font_h2 * 6;
  }
  SetRect(&rect,ctrl_left,ctrl_top,ctrl_width + ctrl_left,ctrl_top + ctrl_height);
  if (large_layout == 0) {
    history_font = ((i8 *)&font_size14);
  }
  else {
    history_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((GameFont *)history_font);
  if (this == (ParleyWindow *)0x0) {
    history_parent = (u8 *)0x0;
  }
  else {
    history_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)chat_log_edit)->create_styled((Wnd *)history_parent,0x3ee,&rect,((const char *)(*(iptr *)&chat.log_buffer)),
             0x122,0);
  ((EditControl *)chat_log_edit)->set_max_len(0x2000);
  ((EditControl *)chat_log_edit)->set_callback(parley_edit_escape_filter);
  ((Control *)chat_log_edit)->show_and_redraw();
  new_edit = (EditControl *)(operator_new(0x48));
  if (new_edit == (EditControl *)0x0) {
    input_edit = 0;
  }
  else {
    input_edit = (uptr)(edit_ctor(new_edit));
  }
  chat_input_edit = input_edit;
  ctrl_width -= ctrl_width / 10;
  if (mode == 4) {
    if (large_layout == 0) {
      input_font_h3 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h3 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    y_delta = panel_layout.margin_y * -4 + input_font_h3 * -3;
    if (large_layout == 0) {
      input_font_h4 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h4 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = input_font_h4 * 3 + panel_layout.margin_y * 3;
  }
  else {
    if (large_layout == 0) {
      input_font_h1 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h1 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    y_delta = panel_layout.margin_y * -3 + input_font_h1 * -2;
    if (large_layout == 0) {
      input_font_h2 = ((GameFont *)((i8 *)&font_size14))->font_cached_height();
    }
    else {
      input_font_h2 = ((GameFont *)((i8 *)&font_size21))->font_cached_height();
    }
    ctrl_height = panel_layout.margin_y * 2 + input_font_h2 * 2;
  }
  ctrl_top += y_delta;
  SetRect(&rect,ctrl_left,ctrl_top,ctrl_width + ctrl_left,ctrl_top + ctrl_height);
  if (large_layout == 0) {
    input_font = ((i8 *)&font_size14);
  }
  else {
    input_font = ((i8 *)&font_size21);
  }
  ui_set_default_font((GameFont *)input_font);
  if (mode == 4) {
    input_style = 0x200;
  }
  else {
    input_style = 0x220;
  }
  if (this == (ParleyWindow *)0x0) {
    input_parent = (u8 *)0x0;
  }
  else {
    input_parent = ((u8 *)this + 0x48);
  }
  ((EditControl *)chat_input_edit)->create_styled((Wnd *)input_parent,0x3ef,&rect,((const char *)(*(iptr *)&chat.input_buffer)),
             input_style,0);
  ((EditControl *)chat_input_edit)->set_max_len(0x100);
  ((EditControl *)chat_input_edit)->set_callback(chat_input_on_key);
  ((Control *)chat_input_edit)->set_initial_focus();
  ((Control *)chat_input_edit)->show_and_redraw();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    send_button = 0;
  }
  else {
    send_button = (uptr)(button_ctor(new_ctrl));
  }
  xLeft = rect.right;
  this->send_button = send_button;
  ctrl_width = ctrl_width / 9;
  ctrl_height = ctrl_height / 2;
  SetRect(&rect,rect.right,ctrl_top,ctrl_width + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    send_parent = (u8 *)0x0;
  }
  else {
    send_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2cd]));
  ((Button *)this->send_button)->create((Wnd *)send_parent,0x3f0,&rect,(const char *)label);
  ((Button *)this->send_button)->set_callback((CtrlNotifyFn)chat_send_button_on_click);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    clear_button = 0;
  }
  else {
    clear_button = (uptr)(button_ctor(new_ctrl));
  }
  this->clear_button = clear_button;
  ctrl_top += ctrl_height;
  SetRect(&rect,xLeft,ctrl_top,ctrl_width + xLeft,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    clear_parent = (u8 *)0x0;
  }
  else {
    clear_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2ce]));
  ((Button *)this->clear_button)->create((Wnd *)clear_parent,0x41e,&rect,(const char *)label);
  ((Button *)this->clear_button)->set_callback((CtrlNotifyFn)chat_clear_button_on_click);
  ctrl_top -= ctrl_height;
  if (large_layout == 0) {
    footer_line_h = ((GameFont *)((i8 *)&font_size16))->line_spacing();
  }
  else {
    footer_line_h = ((GameFont *)((i8 *)&font_size24))->line_spacing();
  }
  ctrl_height = panel_layout.padding_y * 2 + footer_line_h;
  ctrl_top -= (panel_layout.margin_y + ctrl_height);
  divider_y = ctrl_top - panel_layout.margin_y;
}

/* Part of ParleyWindow::create_controls (004b5c93): the parley buttons
   (conceal, reveal, intelligence, the two mode buttons, never mind, accept,
   no thanks, counter) in a row above the chat. */
void ParleyWindow::create_parley_buttons(int ctrl_top,int ctrl_height)
{
  int ctrl_width;
  int ctrl_left;
  Button *new_ctrl;
  iptr label;
  u8 *counter_parent;
  u8 *no_thanks_parent;
  u8 *accept_parent;
  u8 *never_mind_parent;
  u8 *mode2_parent;
  u8 *mode_parent;
  u8 *intel_parent;
  u8 *reveal_parent;
  u8 *conceal_parent;
  uptr counter_button;
  uptr no_thanks_button;
  uptr accept_button;
  uptr never_mind_button;
  uptr mode2_button;
  uptr mode_button;
  uptr intel_button;
  uptr reveal_button;
  uptr conceal_button;
  tagRECT rect;
  
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    conceal_button = 0;
  }
  else {
    conceal_button = (uptr)(button_ctor(new_ctrl));
  }
  this->conceal_button = conceal_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  ctrl_left = rect_width((RECT *)&panel_left);
  ctrl_left += panel_layout.margin_x * -2;
  SetRect(&rect,ctrl_width,ctrl_top - ctrl_height,ctrl_left + ctrl_width,ctrl_top);
  if (this == (ParleyWindow *)0x0) {
    conceal_parent = (u8 *)0x0;
  }
  else {
    conceal_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x36d]));
  ((Button *)this->conceal_button)->create((Wnd *)conceal_parent,0x431,&rect,(const char *)label);
  ((Button *)this->conceal_button)->set_callback((CtrlNotifyFn)parley_on_conceal_info);
  ((Control *)this->conceal_button)->hide();
  ((Button *)this->conceal_button)->disable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    reveal_button = 0;
  }
  else {
    reveal_button = (uptr)(button_ctor(new_ctrl));
  }
  this->reveal_button = reveal_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  SetRect(&rect,ctrl_width,ctrl_top - ctrl_height,ctrl_left + ctrl_width,ctrl_top);
  if (this == (ParleyWindow *)0x0) {
    reveal_parent = (u8 *)0x0;
  }
  else {
    reveal_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x36e]));
  ((Button *)this->reveal_button)->create((Wnd *)reveal_parent,0x432,&rect,(const char *)label);
  ((Button *)this->reveal_button)->set_callback((CtrlNotifyFn)parley_on_reveal_info);
  ((Control *)this->reveal_button)->hide();
  ((Button *)this->reveal_button)->disable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    intel_button = 0;
  }
  else {
    intel_button = (uptr)(button_ctor(new_ctrl));
  }
  this->intel_button = intel_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  ctrl_left = rect_width((RECT *)&panel_left);
  ctrl_left = (ctrl_left + panel_layout.margin_x * -2) / 3;
  SetRect(&rect,ctrl_width,ctrl_top,ctrl_left + ctrl_width,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    intel_parent = (u8 *)0x0;
  }
  else {
    intel_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x68]));
  ((Button *)this->intel_button)->create((Wnd *)intel_parent,0x428,&rect,(const char *)label);
  ((Button *)this->intel_button)->set_callback((CtrlNotifyFn)parley_on_check_intel);
  ((Button *)this->intel_button)->enable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    mode_button = 0;
  }
  else {
    mode_button = (uptr)(button_ctor(new_ctrl));
  }
  this->mode_button = mode_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    mode_parent = (u8 *)0x0;
  }
  else {
    mode_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2cf]));
  ((Button *)this->mode_button)->create((Wnd *)mode_parent,0x3f1,&rect,(const char *)label);
  ((Button *)this->mode_button)->set_callback(parley_on_mode_chosen);
  ((Button *)this->mode_button)->enable();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    mode2_button = 0;
  }
  else {
    mode2_button = (uptr)(button_ctor(new_ctrl));
  }
  this->mode2_button = mode2_button;
  if (this == (ParleyWindow *)0x0) {
    mode2_parent = (u8 *)0x0;
  }
  else {
    mode2_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x344]));
  ((Button *)this->mode2_button)->create((Wnd *)mode2_parent,0x427,&rect,(const char *)label);
  ((Button *)this->mode2_button)->set_callback(parley_on_mode_chosen);
  ((Button *)this->mode2_button)->disable();
  ((Control *)this->mode2_button)->hide();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    never_mind_button = 0;
  }
  else {
    never_mind_button = (uptr)(button_ctor(new_ctrl));
  }
  this->never_mind_button = never_mind_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    never_mind_parent = (u8 *)0x0;
  }
  else {
    never_mind_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d0]));
  ((Button *)this->never_mind_button)->create((Wnd *)never_mind_parent,0x3f2,&rect,(const char *)label);
  ((Button *)this->never_mind_button)->set_callback((CtrlNotifyFn)parley_on_never_mind);
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    accept_button = 0;
  }
  else {
    accept_button = (uptr)(button_ctor(new_ctrl));
  }
  this->accept_button = accept_button;
  ctrl_width = panel_left + panel_layout.margin_x;
  ctrl_left = rect_width((RECT *)&panel_left);
  ctrl_left = (ctrl_left + panel_layout.margin_x * -2) / 3;
  SetRect(&rect,ctrl_width,ctrl_top,ctrl_left + ctrl_width,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    accept_parent = (u8 *)0x0;
  }
  else {
    accept_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d1]));
  ((Button *)this->accept_button)->create((Wnd *)accept_parent,0x3f2,&rect,(const char *)label);
  ((Button *)this->accept_button)->set_callback((CtrlNotifyFn)parley_on_accept_offer);
  ((Control *)this->accept_button)->hide();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    no_thanks_button = 0;
  }
  else {
    no_thanks_button = (uptr)(button_ctor(new_ctrl));
  }
  this->no_thanks_button = no_thanks_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    no_thanks_parent = (u8 *)0x0;
  }
  else {
    no_thanks_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d2]));
  ((Button *)this->no_thanks_button)->create((Wnd *)no_thanks_parent,0x3f2,&rect,(const char *)label);
  ((Button *)this->no_thanks_button)->set_callback((CtrlNotifyFn)parley_on_no_thanks);
  ((Control *)this->no_thanks_button)->hide();
  new_ctrl = (Button *)(operator_new(0x3c));
  if (new_ctrl == (Button *)0x0) {
    counter_button = 0;
  }
  else {
    counter_button = (uptr)(button_ctor(new_ctrl));
  }
  this->counter_button = counter_button;
  SetRect(&rect,rect.right,ctrl_top,ctrl_left + rect.right,ctrl_top + ctrl_height);
  if (this == (ParleyWindow *)0x0) {
    counter_parent = (u8 *)0x0;
  }
  else {
    counter_parent = ((u8 *)this + 0x48);
  }
  label = (iptr)(label_string(label_table.string_ids[0x2d3]));
  ((Button *)this->counter_button)->create((Wnd *)counter_parent,0x3f2,&rect,(const char *)label);
  ((Button *)this->counter_button)->set_callback((CtrlNotifyFn)parley_on_counter_offer);
  ((Control *)this->counter_button)->hide();
}

/* 004b5c93 auto */
void ParleyWindow::create_controls()

{
  int ctrl_top;
  int ctrl_height;
  
  create_caption_buttons();
  create_close_button();
  create_chat_controls(ctrl_top,ctrl_height);
  if (mode != 4) {
    create_parley_buttons(ctrl_top,ctrl_height);
  }
  return;
}

/* 004b74c4 auto */
void __cdecl parley_wnd_on_caption_click(int control_id)

{
  iptr child_wnd;
  ParleyWindow *parley_wnd_ptr;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    parley_wnd_ptr = (ParleyWindow *)0x0;
  }
  else {
    parley_wnd_ptr = (ParleyWindow *)(child_wnd - 0x48);
  }
  parley_wnd_ptr->window.activate();
  if (control_id == 0x3e9) {
    diplo_wnd_close_active();
  }
  else if (control_id == 0x3ea) {
    if (0 < (int)parley_wnd_ptr->large_layout) {
      parley_wnd_ptr->window.hide();
      parley_wnd_ptr->large_layout--;
      parley_wnd_ptr->calc_rect();
      parley_wnd_ptr->window.move_to_rect((RECT *)parley_wnd_ptr->init_rect);
      parley_wnd_ptr->SurfaceWindow::show();
    }
  }
  else if (((control_id == 0x3eb) && ((int)parley_wnd_ptr->large_layout < 1)) && (999 < caption_font_block.screen_width)) {
    parley_wnd_ptr->window.hide();
    parley_wnd_ptr->large_layout++;
    parley_wnd_ptr->calc_rect();
    parley_wnd_ptr->window.move_to_rect((RECT *)parley_wnd_ptr->init_rect);
    parley_wnd_ptr->SurfaceWindow::show();
  }
  return;
}

/* 004b7645 auto */
/* WARNING: Removing unreachable block (ram,0x004b766b) */

void __cdecl parley_wnd_close(void)

{
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  diplo_wnd_close_active();
  return;
}

/* 004b768d auto */
/* WARNING: Removing unreachable block (ram,0x004b76b3) */

void __cdecl chat_wnd_close(void)

{
  wnd_set_active_child((iptr)&chat_wnd_block.window);
  diplo_wnd_close_active();
  return;
}

/* 004b7885 auto */
void __cdecl parley_wnd_redraw(void)

{
  iptr child_wnd;
  i32 caret_pos;
  ParleyWindow *wnd2;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (wnd2->closed == 0) {
    wnd2->calc_layout();
    wnd2->build_title();
    gfx_set_draw_target(wnd2);
    wnd2->draw_frame();
    wnd2->sync_sides();
    wnd2->wnd_blit_surface();
    wnd2->SurfaceWindow::show();
    caret_pos = ((EditControl *)wnd2->chat_log_edit)->get_text_end();
    ((EditControl *)wnd2->chat_log_edit)->set_caret(caret_pos);
    caret_pos = ((EditControl *)wnd2->chat_input_edit)->get_sel_end();
    ((EditControl *)wnd2->chat_input_edit)->set_caret(caret_pos);
  }
  return;
}

/* 004b794a auto */
void ParleyWindow::draw_frame()

{
  int free_width;
  int title_width;
  i8 sprite_rect[16];
  int icon_height;
  int icon_width;
  int y2;
  int x2;
  GameFont *title_font2;
  int i;
  tagRECT rect;
  
  reset_clip();
  crt_memcpy(&rect,&client_left,0x10);
  for (i = 0; i < panel_layout.bevel_width; i++) {
    gfx_draw_bevel_rect(this,&rect,(uint)dlg_style.bevel_light_color,(uint)dlg_style.bevel_dark_color);
    rect_deflate(&rect,1,1);
  }
  fill_background(&rect,1);
  rect_deflate(&rect,panel_layout.padding_x + panel_layout.bevel_width * -2,0);
  rect.top = rect.top + caption_height + panel_layout.bevel_width * -2;
  free_width = rect_height((RECT *)&client_left);
  rect.bottom = (free_width - caption_height) + panel_layout.bevel_width * -2;
  for (i = 0; i < panel_layout.bevel_width; i++) {
    gfx_draw_bevel_rect(this,&rect,(uint)dlg_style.bevel_dark_color,(uint)dlg_style.bevel_light_color);
    rect_deflate(&rect,1,1);
  }
  fill_background(&rect,2);
  x2 = panel_layout.padding_x;
  for (i = 0; i < 3; i++) {
    icon_width = (int)(caption_buttons[i].width * caption_height) / 0x18;
    icon_height = (int)(caption_buttons[i].height * caption_height) / 0x18;
    y2 = ((int)caption_height >> 1) - (icon_height >> 1);
    SetRect(&rect,x2,y2,icon_width + x2,icon_height + y2);
    gfx_set_scale_ratio(caption_height,0x18);
    caption_buttons[i].sprite->draw((RECT *)sprite_rect,
               this,x2,y2);
    gfx_reset_zoom_scale();
    x2 = x2 + panel_layout.margin_x + icon_width;
  }
  if (large_layout == 0) {
    title_font2 = (GameFont *)((i8 *)&font_size20);
  }
  else {
    title_font2 = (GameFont *)((i8 *)&font_size30);
  }
  x2 -= panel_layout.margin_x;
  free_width = (client_right - x2) - panel_layout.padding_x;
  title_width = title_font2->text_width(title);
  x2 += (free_width / 2 - (title_width >> 1));
  y2 = panel_layout.bevel_width + panel_layout.margin_y;
  gfx_set_text_color_index(10);
  draw_text(title_font2,title,x2 + 2,y2 + 1,5);
  gfx_set_text_color_index(0x1a);
  draw_text(title_font2,title,x2 + 1,y2,5);
  draw_text(title_font2,title,x2,y2,5);
  set_clip((RECT *)&panel_left);
  return;
}

/* 004b7c90 auto */
void ParleyWindow::fill_background(RECT *rect,int area_kind)

{
  i32 height;
  i32 width;
  i32 tile_x;
  i32 tile_y;
  
  if ((dlg_style.frame_tile_surface == 0) || (area_kind != 1)) {
    if ((dlg_style.client_tile_surface == 0) || (area_kind != 2)) {
      gfx_fill_rect(this,rect,(uint)dlg_style.bg_color);
    }
    else {
      tile_y = 0;
      tile_x = 0;
      height = rect_height(rect);
      width = rect_width(rect);
      gfx_tile_fill(this,(DibSurface *)dlg_style.client_tile_surface,rect->left,rect->top
                         ,width,height,tile_x,tile_y);
    }
  }
  else {
    tile_y = 0;
    tile_x = 0;
    height = rect_height(rect);
    width = rect_width(rect);
    gfx_tile_fill(this,(DibSurface *)dlg_style.frame_tile_surface,rect->left,rect->top,
                       width,height,tile_x,tile_y);
  }
  return;
}

/* 004b7d72 auto */
void ParleyWindow::build_title()

{
  const char *text;
  iptr attitude_level;
  
  text_clear();
  if (mode == 4) {
    text = (label_string(label_table.string_ids[0x2d5]));
    text_append(text);
    crt_strcpy(&title[0],((i8 *)&textbuf.buf));
  }
  else {
    text = (label_string(label_table.string_ids[0x2d4]));
    text_append(text);
    text_append_space();
    text = (civ_name_plural(other_civ));
    text_append(text);
    text_append_space();
    text_append_lparen();
    if (*(char *)&civs[other_civ].attitude[human_civ[0]] == '\0') {
      attitude_level = 0;
    }
    else {
      attitude_level = diplo_attitude_level((uint)civs[other_civ].attitude[human_civ[0]]);
    }
    text_append_string_id(attitude_names[attitude_level]);
    text_append_rparen();
    crt_strcpy(&title[0],((i8 *)&textbuf.buf));
  }
  return;
}

/* 004b7eb6 auto */
/* WARNING: Removing unreachable block (ram,0x004b8008) */
/* WARNING: Removing unreachable block (ram,0x004b8051) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void ParleyWindow::open(int other_civ,u32 mode)

{
  bool can_see_all;
  bool has_contact;
  iptr has_wonder;
  iptr child_wnd;
  i32 caret_pos;
  int civ;
  ParleyWindow *wnd2;
  
  if (mode == 4) {
    has_contact = false;
    if (((game.wonder_city[WONDER_UNITED_NATIONS] == -1) && (has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO), has_wonder == 0)
        ) && (game.reveal_map == '\0')) {
      can_see_all = false;
    }
    else {
      can_see_all = true;
    }
    for (civ = 1; civ < 8; civ++) {
      if ((((human_civ[0] != civ) &&
           ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
          ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0)) &&
         (((can_see_all || (mp_options.chat_from_start != 0)) ||
          ((*(byte *)&civs[civ].treaties[human_civ[0]] & TREATY_CONTACT) != 0)))) {
        has_contact = true;
        break;
      }
    }
    if (!has_contact) {
      popup_game((const char *)&str_noforeignhuman);
      return;
    }
    wnd_set_active_child((iptr)&chat_wnd_block.window);
    net.chat_unread_count = 0;
    net.chat_missed_message_count = 0;
  }
  else {
    wnd_set_active_child((iptr)&parley_wnd_block.window);
    parley_wnd_block.selected_action = 0xffffffff;
    remote_units_revealed = '\0';
    local_units_revealed = '\0';
    local_cities_revealed = '\0';
    remote_cities_revealed = '\0';
    if (mode == 3) {
      parley_wnd_block.set_mode_sides(0);
    }
  }
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = (ParleyWindow *)0x0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (wnd2->closed == 0) {
    wnd2->SurfaceWindow::show();
  }
  else {
    ((u8 *)wnd2 + 0x0)[0x114] = '\0';
    ((u8 *)wnd2 + 0x0)[0x115] = '\0';
    ((u8 *)wnd2 + 0x0)[0x116] = '\0';
    ((u8 *)wnd2 + 0x0)[0x117] = '\0';
    wnd2->other_civ = other_civ;
    if (mode != 3) {
      wnd2->mode = mode;
      wnd2->left_kind = (((i32 *)&parley_panes.mode_left_kinds))[mode * 2];
      wnd2->right_kind = (((i32 *)&parley_panes.mode_right_kinds))[mode * 2];
    }
    wnd2->build_title();
    ((u8 *)wnd2 + 0x1f8)[0] = 21;
    ((u8 *)wnd2 + 0x1f9)[0] = '\0';
    ((u8 *)wnd2 + 0x1fa)[0] = '\0';
    ((u8 *)wnd2 + 0x1fb)[0] = '\0';
    ((u8 *)wnd2 + 0x1fc)[0] = 21;
    ((u8 *)wnd2 + 0x1fd)[0] = '\0';
    ((u8 *)wnd2 + 0x1fe)[0] = '\0';
    ((u8 *)wnd2 + 0x1ff)[0] = '\0';
    wnd2->repaint();
    wnd2->SurfaceWindow::show();
    caret_pos = ((EditControl *)wnd2->chat_log_edit)->get_text_end();
    ((EditControl *)wnd2->chat_log_edit)->set_caret(caret_pos);
    caret_pos = ((EditControl *)wnd2->chat_input_edit)->get_sel_end();
    ((EditControl *)wnd2->chat_input_edit)->set_caret(caret_pos);
  }
  snd_play_effect(((uint)map.seed + other_civ & 7) + 0x53,1,0,0);
  return;
}

/* 004b81dd auto */
/* WARNING: Removing unreachable block (ram,0x004b8211) */

void __cdecl parley_poll_net_replies(void)

{
  const char *civ_name;
  
  net.process_messages(1,0);
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  if ((net.nm_parley_transaction_count == 0) || ((int)parley_wnd_block.mode != 2)) {
    if ((net.nm_parley_accept_offer_count == 0) || ((int)parley_wnd_block.mode != 1)) {
      if ((net.nm_parley_no_thanks_count == 0) || ((int)parley_wnd_block.mode != 1)) {
        if ((net.nm_parley_counter_offer_count != 0) && ((int)parley_wnd_block.mode == 1)) {
          net.nm_parley_counter_offer_count = 0;
          parley_wnd_block.mode = 2;
          parley_wnd_block.left_kind = parley_panes.mode2_left_kind;
          parley_wnd_block.right_kind = parley_panes.mode2_right_kind;
          parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
          parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
          parley_wnd_block.add_side_dialog(parley_panes.mode2_left_kind,0);
          parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
          civ_name = (civ_name_plural(parley_wnd_block.other_civ));
          text_set_string_arg(0,civ_name);
          dlg_style.pending_idle_callback = parley_poll_net_replies;
          popup_game_with_sprite(str_parleycounteroffer,(Sprite *)&popup_info_icon_sprite,0);
        }
      }
      else {
        net.nm_parley_no_thanks_count = 0;
        parley_wnd_block.mode = 3;
        parley_wnd_block.selected_action = 0xffffffff;
        parley_wnd_block.set_mode_sides(1);
        civ_name = (civ_name_plural(parley_wnd_block.other_civ));
        text_set_string_arg(0,civ_name);
        dlg_style.pending_idle_callback = parley_poll_net_replies;
        popup_game_with_sprite(str_parleynothanks,(Sprite *)&popup_info_icon_sprite,0);
      }
    }
    else {
      net.nm_parley_accept_offer_count = 0;
      if ((void *)parley_wnd_block.received_offer != (void *)0x0) {
        parley_wnd_block.received_offer->hdr.msg_type = NM_PARLEY_EXECUTE;
        if (net.host_flag == '\0') {
          net_send_message(NM_PARLEY_EXECUTE,0,0,0,0,0,0,0,0,(NetMsg *)(void *)parley_wnd_block.received_offer);
        }
        else {
          parley_execute_transaction(parley_wnd_block.received_offer);
        }
        operator_delete((void *)parley_wnd_block.received_offer);
        parley_wnd_block.received_offer = 0;
      }
      parley_wnd_block.mode = 2;
      parley_wnd_block.left_kind = parley_panes.mode2_left_kind;
      parley_wnd_block.right_kind = parley_panes.mode2_right_kind;
      parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
      parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
      parley_wnd_block.add_side_dialog(parley_panes.mode2_left_kind,0);
      parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
      dlg_style.pending_idle_callback = parley_poll_net_replies;
      popup_game_with_sprite(str_parleyaccept,(Sprite *)&popup_info_icon_sprite,0);
    }
  }
  else {
    net.nm_parley_transaction_count = 0;
    if ((parley_wnd_block.received_offer->mode == 6) && (parley_wnd_block.received_offer->item.value == 3)) {
      civ_name = (civ_name_plural(parley_wnd_block.other_civ));
      text_set_string_arg(0,civ_name);
      civ_name = (civ_name_plural(human_civ[0]));
      text_set_string_arg(1,civ_name);
      if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_ALLIANCE) == 0) {
        if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_PEACE) == 0) {
          if ((*(byte *)&civs[human_civ[0]].treaties[(int)parley_wnd_block.other_civ] & TREATY_CEASEFIRE) != 0) {
            diplo_clear_treaty_flags(human_civ[0],parley_wnd_block.other_civ,2);
            popup_game((const char *)&str_breakcease);
          }
        }
        else {
          diplo_clear_treaty_flags(human_civ[0],parley_wnd_block.other_civ,4);
          popup_game((const char *)&str_cancelpeace);
        }
      }
      else {
        diplo_clear_treaty_flags(human_civ[0],parley_wnd_block.other_civ,8);
        popup_game((const char *)&str_cancelallied);
      }
    }
    else {
      parley_wnd_block.mode = 0;
      parley_wnd_block.left_kind = parley_panes.mode_left_kinds[0];
      parley_wnd_block.right_kind = parley_panes.mode_right_kinds[0];
      parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
      parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
      parley_wnd_block.add_side_dialog(parley_panes.mode_left_kinds[0],0);
      parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
    }
  }
  return;
}

/* 004b8676 auto */
/* WARNING: Removing unreachable block (ram,0x004b869f) */

void ParleyWindow::set_mode_sides(int refresh)

{
  wnd_set_active_child((iptr)&parley_wnd_block.window);
  if ((i32)parley_wnd_block.selected_action == -1) {
    parley_wnd_block.mode = 3;
    ((Button *)parley_wnd_block.never_mind_button)->disable();
  }
  else if (parley_wnd_block.selected_action == 0) {
    parley_wnd_block.mode = 6;
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 1) {
    parley_wnd_block.mode = 0xc;
    if (parley_wnd_block.proposal_kind != 0) {
      parley_wnd_block.mode = 0xd;
    }
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 2) {
    if (parley_wnd_block.gift_kind == 0) {
      parley_wnd_block.mode = 7;
    }
    else if ((int)parley_wnd_block.gift_kind == 1) {
      parley_wnd_block.mode = 8;
    }
    else if ((int)parley_wnd_block.gift_kind == 2) {
      parley_wnd_block.mode = 9;
    }
    else if ((int)parley_wnd_block.gift_kind == 3) {
      parley_wnd_block.mode = 10;
    }
    else {
      parley_wnd_block.mode = 0xb;
    }
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 3) {
    parley_wnd_block.mode = 5;
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  else if ((i32)parley_wnd_block.selected_action == 4) {
    parley_wnd_block.mode = 0xf;
    ((Button *)parley_wnd_block.never_mind_button)->enable();
  }
  parley_wnd_block.left_kind = (((i32 *)&parley_panes.mode_left_kinds))[(int)parley_wnd_block.mode * 2];
  parley_wnd_block.right_kind = (((i32 *)&parley_panes.mode_right_kinds))[(int)parley_wnd_block.mode * 2];
  parley_wnd_block.shown_left_kind = (i32)parley_wnd_block.left_kind;
  parley_wnd_block.shown_right_kind = (i32)parley_wnd_block.right_kind;
  if (refresh != 0) {
    parley_wnd_block.add_side_dialog((i32)parley_wnd_block.left_kind,0);
    parley_wnd_block.add_side_dialog((i32)parley_wnd_block.right_kind,1);
  }
  return;
}

/* 004b888e auto */
void ParleyWindow::clear_side_controls(int side)

{
  u8 *slot_ptr;
  int half_width;
  int i;
  
  if ((side != 0) && (side != 1)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_leftright_left_leftright_right,(const char*)&str_d_ss_franklinton_parleywin_cpp,0x4ea);
  }
  if (side == 1) {
    if (desc_edit != 0) {
      if ((Button *)desc_edit != (Button *)0x0) {
        ((EditControl *)desc_edit)->scalar_destruct(1);
      }
      desc_edit = 0;
    }
    if (anonymous_checkbox != 0) {
      if ((Checkbox *)anonymous_checkbox != (Checkbox *)0x0) {
        ((Checkbox *)anonymous_checkbox)->scalar_destruct(1);
      }
      anonymous_checkbox = 0;
    }
    if (mute_anonymous_checkbox != 0) {
      if ((Checkbox *)mute_anonymous_checkbox != (Checkbox *)0x0) {
        ((Checkbox *)mute_anonymous_checkbox)->scalar_destruct(1);
      }
      mute_anonymous_checkbox = 0;
    }
    if (broadcast_checkbox != 0) {
      if ((Checkbox *)broadcast_checkbox != (Checkbox *)0x0) {
        ((Checkbox *)broadcast_checkbox)->scalar_destruct(1);
      }
      broadcast_checkbox = 0;
    }
    for (i = 0; i < 8; i++) {
      if (send_checkboxes[i] != 0) {
        if ((Checkbox *)send_checkboxes[i] != (Checkbox *)0x0) {
          ((Checkbox *)send_checkboxes[i])->scalar_destruct(1);
        }
        slot_ptr = (u8 *)&send_checkboxes[i];
        slot_ptr[0] = '\0';
        slot_ptr[1] = '\0';
        slot_ptr[2] = '\0';
        slot_ptr[3] = '\0';
      }
      if (mute_checkboxes[i] != 0) {
        if ((Checkbox *)mute_checkboxes[i] != (Checkbox *)0x0) {
          ((Checkbox *)mute_checkboxes[i])->scalar_destruct(1);
        }
        slot_ptr = (u8 *)&mute_checkboxes[i];
        slot_ptr[0] = '\0';
        slot_ptr[1] = '\0';
        slot_ptr[2] = '\0';
        slot_ptr[3] = '\0';
      }
    }
  }
  if (side_checkboxes[side] != 0) {
    if ((Checkbox *)side_checkboxes[side] != (Checkbox *)0x0) {
      ((Checkbox *)side_checkboxes[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&side_checkboxes[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (side_radios[side] != 0) {
    if ((RadioGroup *)side_radios[side] != (RadioGroup *)0x0) {
      ((RadioGroup *)side_radios[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&side_radios[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (gold_edits[side] != 0) {
    if ((Button *)gold_edits[side] != (Button *)0x0) {
      ((EditControl *)gold_edits[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&gold_edits[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (list_scrollbars[side] != 0) {
    if ((Button *)list_scrollbars[side] != (Button *)0x0) {
      ((Scrollbar *)list_scrollbars[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&list_scrollbars[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (list_hotspots[side] != 0) {
    if ((Hotspot *)list_hotspots[side] != (Hotspot *)0x0) {
      ((Hotspot *)list_hotspots[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&list_hotspots[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (sort_by_unit_buttons[side] != 0) {
    if ((Button *)sort_by_unit_buttons[side] != (Button *)0x0) {
      ((Button *)sort_by_unit_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&sort_by_unit_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (sort_by_city_buttons[side] != 0) {
    if ((Button *)sort_by_city_buttons[side] != (Button *)0x0) {
      ((Button *)sort_by_city_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&sort_by_city_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (info_show_buttons[side] != 0) {
    if ((Button *)info_show_buttons[side] != (Button *)0x0) {
      ((Button *)info_show_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&info_show_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  if (info_hide_buttons[side] != 0) {
    if ((Button *)info_hide_buttons[side] != (Button *)0x0) {
      ((Button *)info_hide_buttons[side])->scalar_destruct(1);
    }
    slot_ptr = (u8 *)&info_hide_buttons[side];
    slot_ptr[0] = '\0';
    slot_ptr[1] = '\0';
    slot_ptr[2] = '\0';
    slot_ptr[3] = '\0';
  }
  fill_left = panel_left;
  fill_top = panel_top;
  fill_right = panel_right;
  fill_bottom = panel_bottom;
  fill_bottom = divider_y;
  if (side == 0) {
    half_width = rect_width((RECT *)&panel_left);
    fill_right = half_width / 2;
  }
  else {
    half_width = rect_width((RECT *)&panel_left);
    fill_left = half_width / 2;
  }
  fill_background((RECT *)&fill_left,2);
  return;
}

/* 004b8e5c auto */
void __cdecl parley_gold_edit_on_change(int control_id)

{
  char *src_ptr;
  size_t len;
  int amount;
  size_t text_len;
  uptr i;
  char gold_text[32];
  uint edit_index;
  i32 modified;
  char text[32];
  uint caret;
  
  modified = 0;
  _memset(text,0,0x20);
  _memset(gold_text,0,0x20);
  edit_index = (uint)(control_id != 0x3f8);
  caret = ((EditControl *)parley_wnd_block.gold_edits[edit_index])->get_sel_end();
  ((Control *)parley_wnd_block.gold_edits[edit_index])->Control::get_text((char *)text);
  __itoa(civs[human_civ[0]].gold,gold_text,10);
  for (i = 0; len = _strlen(text), i < len; i++) {
    if ((text[i] < '0') || ('9' < text[i])) {
      modified = 1;
      crt_strcpy(text + i,text + i + 1);
    }
  }
  amount = _atoi(text);
  if ((((int)parley_wnd_block.mode == 0xe) && (edit_index == 0)) || (((int)parley_wnd_block.mode != 0xf && ((int)parley_wnd_block.mode != 0xe)))
     ) {
    len = _strlen(gold_text);
    text_len = _strlen(text);
    if ((len < text_len) || (civs[human_civ[0]].gold < amount)) {
      modified = 1;
      src_ptr = text + caret;
      amount = caret - 1;
      caret--;
      crt_strcpy(text + amount,src_ptr);
    }
  }
  amount = _atoi(text);
  if (amount < 1) {
    parley_wnd_block.side_selected[edit_index] = 0;
  }
  else {
    parley_wnd_block.side_selected[edit_index] = 1;
  }
  parley_wnd_block.update_buttons();
  if (modified != 0) {
    len = _strlen(text);
    if (len < caret) {
      caret = _strlen(text);
    }
    ((Control *)parley_wnd_block.gold_edits[edit_index])->set_text((const char *)text);
    ((EditControl *)parley_wnd_block.gold_edits[edit_index])->set_caret(caret);
  }
  return;
}

/* 004b90ad auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void ParleyWindow::chat_receive_message(int civ,int civ2,int is_private,const char *message)

{
  u32 muted;
  size_t log_len;
  const void *label;
  int listener;
  
  ((Control *)chat_wnd_block.chat_log_edit)->Control::get_text(chat.log_buffer);
  text_clear();
  log_len = _strlen(chat.log_buffer);
  if (log_len != 0) {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_newline_str));
  }
  if ((char)is_private == '\0') {
    label = (label_string(label_table.string_ids[0x2d8]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_year_open_str));
    text_append_year((int)game.year);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_year_close_str));
  }
  else {
    label = (label_string(label_table.string_ids[0x2d6]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ);
    label = (label_string(label_table.string_ids[0x2d7]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
    text_append_space();
    chat_append_sender_name(civ2);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_private_year_open_str));
    text_append_year((int)game.year);
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_recv_private_year_close_str));
  }
  crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)message));
  if ((civ == 0) && (chat_wnd_block.mute_anonymous != 0)) {
    return;
  }
  /* a public message: dropped if its sender is muted, unless the parley with him is open */
  if ((char)is_private == '\0') {
    if ((parley_wnd_block.closed != 0) || ((int)parley_wnd_block.other_civ != civ)) {
      muted = muted_civs[civ];
      if (muted != 0) {
        return;
      }
    }
  }
  for (listener = 1; listener < 8; listener++) {
    if ((((listener != civ) && (human_civ[0] != listener)) &&
        ((1 << ((byte)listener & 0x1f) & (uint)game.human_players) != 0)) &&
       ((*(byte *)(listener * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)) {
      net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[listener]].id,
                         civ,civ2,1,0,0,0,message,0);
    }
  }
  chat_log_append(textbuf.buf);
  if (chat_wnd_block.closed != 0) {
    net.chat_unread_count++;
    net.chat_missed_message_count++;
  }
  if ((parley_wnd_block.closed != 0) && (chat_wnd_block.closed != 0)) {
    chat_wnd_block.open(0,4);
  }
  return;
}

/* 004b93a2 auto */
void __cdecl chat_log_append(const char *line)

{
  int capacity;
  size_t log_len;
  size_t line_len;
  i32 caret_pos;
  uptr trim_len;
  
  capacity = font_size16_bold_block.chat_log_capacity;
  trim_len = 0;
  log_len = _strlen(chat.log_buffer);
  line_len = _strlen((char *)line);
  if (capacity - (log_len + 1) < line_len + 1) {
    trim_len = _strlen((char *)line);
    do {
      trim_len++;
      if ((chat.log_buffer[trim_len] == '\r') || (chat.log_buffer[trim_len] == '\n')) break;
    } while (chat.log_buffer[trim_len] != '\0');
    if (chat.log_buffer[trim_len] != '\0') {
      for (; (chat.log_buffer[trim_len] == '\r' || (chat.log_buffer[trim_len] == '\n'));
          trim_len = trim_len + 1) {
      }
    }
  }
  crt_strcpy(chat.log_buffer,chat.log_buffer + trim_len);
  crt_strcat(chat.log_buffer,(const char *)((const void *)line));
  ((Control *)chat_wnd_block.chat_log_edit)->set_text((const char *)chat.log_buffer);
  ((Control *)parley_wnd_block.chat_log_edit)->set_text((const char *)chat.log_buffer);
  caret_pos = ((EditControl *)chat_wnd_block.chat_log_edit)->get_text_end();
  ((EditControl *)chat_wnd_block.chat_log_edit)->set_caret(caret_pos);
  caret_pos = ((EditControl *)chat_wnd_block.chat_log_edit)->get_text_end();
  ((EditControl *)parley_wnd_block.chat_log_edit)->set_caret(caret_pos);
  return;
}

/* 004b9504 auto */
void __cdecl chat_append_sender_name(int civ)

{
  i32 name_text;
  
  if (civ == 0) {
    name_text = (i32)(label_string(label_table.string_ids[0x2d9]));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)name_text));
  }
  else {
    name_text = civ_leader_title(civ);
    crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)name_text));
    text_append_space();
    name_text = (i32)(civ_leader_name(civ));
    crt_strcat(((i8 *)&textbuf.buf),(const char *)((const void *)name_text));
  }
  return;
}

/* 004b957e auto */
void __cdecl parley_wnd_on_key(int key)

{
  if (key == 0xd2) {
    diplo_wnd_escape_close(0);
  }
  return;
}

/* 004b95c2 auto */
void __cdecl chat_send_button_on_click(void)

{
  chat_input_on_key(0,0xd0);
  return;
}

/* 004b95e1 auto */
void __cdecl chat_clear_button_on_click(void)

{
  _memset((*(void * *)&chat.log_buffer),0,font_size16_bold_block.chat_log_capacity + 1);
  ((Control *)chat_wnd_block.chat_log_edit)->set_text((const char *)(*(void * *)&chat.log_buffer));
  ((Control *)parley_wnd_block.chat_log_edit)->set_text((const char *)(*(void * *)&chat.log_buffer));
  ((Control *)parley_wnd_block.chat_input_edit)->set_focus();
  return;
}

/* 004b9635 auto */
i32 __cdecl parley_edit_escape_filter(i32 control,int key)

{
  i32 result;
  
  if ((key == 0x1b) || (key == 0xd2)) {
    diplo_wnd_escape_close(0);
    result = 0;
  }
  else {
    result = 1;
  }
  return result;
}

/* 004b968a auto */
i32 __cdecl chat_input_on_key(i32 control,int key)

{
  iptr child_wnd;
  size_t len;
  const void *label;
  iptr relay_sender;
  iptr sender;
  int count;
  int civ;
  ParleyWindow *wnd2;
  
  child_wnd = wnd_get_active_child();
  if (child_wnd == 0) {
    wnd2 = 0;
  }
  else {
    wnd2 = (ParleyWindow *)(child_wnd - 0x48);
  }
  if (key < 0x2b5) {
    if (0x2af < key) {
      chat_insert_macro_text((EditControl *)wnd2->chat_input_edit,key);
      return 0;
    }
    if (key == 0xd0) {
      ((Control *)wnd2->chat_input_edit)->Control::get_text((*(char * *)&chat.input_buffer));
      len = _strlen((*(char * *)&chat.input_buffer));
      if (len == 0) {
        return 0;
      }
      ((Control *)chat_wnd_block.chat_log_edit)->Control::get_text(chat.log_buffer);
      text_clear();
      len = _strlen(chat.log_buffer);
      if (len != 0) {
        crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_newline_str));
      }
      label = (label_string(label_table.string_ids[0x2da]));
      crt_strcat(((i8 *)&textbuf.buf),(const char *)label);
      text_append_space();
      if ((int)wnd2->mode == 4) {
        count = 0;
        for (civ = 1; civ < 8; civ++) {
          if (chat_wnd_block.send_to_civs[civ] != 0) {
            if (count != 0) {
              crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_recipient_sep_str));
            }
            chat_append_sender_name(civ);
            count++;
          }
        }
      }
      else {
        chat_append_sender_name((i32)wnd2->other_civ);
        count = 1;
      }
      if (count == 0) {
        return 0;
      }
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_year_open_str));
      text_append_year((int)game.year);
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&chat_send_year_close_str));
      crt_strcat(((i8 *)&textbuf.buf),(*(char * *)&chat.input_buffer));
      chat_log_append(textbuf.buf);
      if ((int)wnd2->mode == 4) {
        for (civ = 1; civ < 8; civ++) {
          if ((((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
              (human_civ[0] != civ)) && (chat_wnd_block.send_to_civs[civ] != 0)) {
            if (chat_wnd_block.remain_anonymous == 0) {
              sender = human_civ[0];
            }
            else {
              sender = 0;
            }
            net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[civ]].id,
                               sender,civ,0,0,0,0,(const char *)(*(char * *)&chat.input_buffer),0);
            for (count = 1; count < 8; count++) {
              if ((((1 << ((byte)count & 0x1f) & (uint)game.human_players) != 0) &&
                  (human_civ[0] != count)) &&
                 ((count != civ &&
                  ((*(byte *)(count * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)))) {
                if (chat_wnd_block.remain_anonymous == 0) {
                  relay_sender = human_civ[0];
                }
                else {
                  relay_sender = 0;
                }
                net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[count]].id,relay_sender,
                                   civ,1,0,0,0,(const char *)(*(char * *)&chat.input_buffer),0);
              }
            }
          }
        }
      }
      else {
        net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[(int)wnd2->other_civ]].id,
                           human_civ[0],wnd2->other_civ,0,0,0,0,(const char *)(*(char * *)&chat.input_buffer),0);
        for (count = 1; count < 8; count++) {
          if ((((1 << ((byte)count & 0x1f) & (uint)game.human_players) != 0) &&
              (human_civ[0] != count)) &&
             (((int)wnd2->other_civ != count &&
              ((*(byte *)(count * 0x594 + (iptr)((u8 *)&civs + 35) + human_civ[0] * 4) & 1) != 0)))) {
            net_send_message(NM_CHAT,net.net_slots[net.net_civ_slots[count]].id
                               ,human_civ[0],wnd2->other_civ,1,0,0,0,(const char *)(*(char * *)&chat.input_buffer),
                               0);
          }
        }
      }
      *(*(char * *)&chat.input_buffer) = '\0';
      ((Control *)wnd2->chat_input_edit)->set_text((const char *)(*(char * *)&chat.input_buffer));
      ((Control *)wnd2->chat_input_edit)->set_focus();
      return 0;
    }
    if (key == 0xd2) {
      diplo_wnd_escape_close(0);
      return 0;
    }
  }
  return 1;
}

/* 004db690 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 *ParleyWindow::send_offer(int mode)

{
  i32 offer_kind;
  i32 *msg;
  int left_items;
  int i;
  uint total_size;
  int right_size;
  int left_size;
  
  offer_kind = parley_offer_kinds[mode];
  parley_wnd_block.offer_counts[0] = 0;
  parley_wnd_block.offer_counts[1] = 0;
  right_size = parley_item_header_sizes[side_kind[1]];
  switch(side_kind[1]) {
  case 8:
  case 0xc:
    for (i = 0; i < parley_wnd_block.list_counts[1]; i++) {
      if ((int)parley_wnd_block.list_selected[i + 2049] == 1) {
        parley_wnd_block.offer_counts[1]++;
      }
    }
    right_size = offer_counts[1] * 4 + right_size;
    break;
  case 10:
  case 0x11:
    for (i = 0; i < parley_wnd_block.list_counts[1]; i++) {
      if ((int)parley_wnd_block.list_selected[i + 2049] == 1) {
        parley_wnd_block.offer_counts[1]++;
      }
    }
    right_size = offer_counts[1] * 4 + right_size;
    if (side_kind[1] == 0x11) {
      right_size = offer_counts[1] * 4 + right_size;
    }
  }
  total_size = right_size + 0x20;
  if (mode == 0xe) {
    left_size = parley_item_header_sizes[side_kind[0]];
    switch(side_kind[0]) {
    case 8:
    case 0xc:
      for (i = 0; i < parley_wnd_block.list_counts[0]; i++) {
        if ((int)parley_wnd_block.list_selected[i] == 1) {
          parley_wnd_block.offer_counts[0]++;
        }
      }
      left_size = offer_counts[0] * 4 + left_size;
      break;
    case 10:
    case 0x11:
      for (i = 0; i < parley_wnd_block.list_counts[0]; i++) {
        if ((int)parley_wnd_block.list_selected[i] == 1) {
          parley_wnd_block.offer_counts[0]++;
        }
      }
      left_size = offer_counts[0] * 4 + left_size;
      if (side_kind[0] == 0x11) {
        left_size = offer_counts[0] * 4 + left_size;
      }
    }
    total_size += left_size;
  }
  msg = (i32 *)(operator_new(total_size));
  if (msg == (i32 *)0x0) {
    dbg_log((const char *)&str_failed_to_allocate_buffer_for_parley_tra);
    msg = (i32 *)0x0;
  }
  else {
    *msg = 0x66606660;
    msg[1] = 0x82;
    msg[2] = total_size;
    msg[4] = this->mode;
    msg[5] = human_civ[0];
    msg[6] = other_civ;
    msg[7] = offer_kind;
    if (total_size + (int)msg < (uint)(right_size + (int)(msg + 8))) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_charptr_pchunk_sizechunk0_charptr_pmessa,(const char*)&str_d_ss_franklinton_parleywin_transaction_c,0x9f);
    }
    pack_offer_items((ParleyOfferItem *)(iptr)(msg + 8),side_kind[1],right_size,1);
    if (mode == 0xe) {
      left_items = (int)msg + right_size + 0x20;
      if (left_size + left_items != total_size + (int)msg) {
        ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_charptr_pchunk_sizechunk1_charptr_pmessa,(const char*)&str_d_ss_franklinton_parleywin_transaction_c_2,0xa4);
      }
      pack_offer_items((ParleyOfferItem *)left_items,side_kind[0],left_size,0);
    }
    net_send_message(NM_PARLEY_TRANSACTION,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0
                       ,0,0,0,0,0,(NetMsg *)msg);
  }
  return msg;
}

/* 004dbab4 auto */
void ParleyWindow::pack_offer_items(ParleyOfferItem *dest,i32 item_type,i32 item_size,int side)

{
  int gold;
  i32 *item_ptr2;
  int packed_count;
  int i;
  i32 *pair_ptr;
  char gold_text[32];
  
  switch(item_type) {
  case 5:
  case 6:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = treaty_kind;
    break;
  case 8:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    item_ptr2 = (i32 *)(dest + 1);   /* the ids after the item */
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *item_ptr2 = (i32)list_items[side * 0x801 + i];
        item_ptr2++;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 9:
    dest->type = item_type;
    dest->size = item_size;
    ((Control *)parley_wnd_block.gold_edits[side])->Control::get_text((char *)gold_text);
    gold = _atoi(gold_text);
    dest->value = gold;
    break;
  case 10:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    pair_ptr = (i32 *)(dest + 1);   /* the ids after the item */
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *pair_ptr = (i32)list_items_b[side * 0x801 + i];
        pair_ptr++;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 0xc:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    item_ptr2 = (i32 *)(dest + 1);   /* the ids after the item */
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *item_ptr2 = (i32)list_items[side * 0x801 + i];
        item_ptr2++;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
    break;
  case 0xd:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)map_choice[side];
    break;
  case 0x11:
    dest->type = item_type;
    dest->size = item_size;
    dest->value = (i32)offer_counts[side];
    pair_ptr = (i32 *)(dest + 1);   /* the ids after the item */
    packed_count = 0;
    for (i = 0; i < parley_wnd_block.list_counts[side]; i++) {
      if ((int)parley_wnd_block.list_selected[i + side * 0x801] == 1) {
        *pair_ptr = (i32)list_items_b[side * 0x801 + i];
        pair_ptr[1] = (i32)list_selected_b[side * 0x801 + i];
        pair_ptr += 2;
        packed_count++;
        if ((int)offer_counts[side] == packed_count) {
          return;
        }
      }
    }
  }
  return;
}

/* (port) BF-082 (Peter Johnson's ledger): the description of an offer within
   the 2048 characters build_description checks it for (of its 2464 bytes);
   longer names (a scenario's rules) or many items are cut there (the original
   appended without a bound, and its check stopped only a debug build) */
static void parley_desc_append(const char *text)
{
  str_append_max((char *)&text_section_index_pool_block.parley_description,text,0x800);
}

/* Part of ParleyWindow::build_description (004dbee6): an exchange (mode 0xe):
   the item given (the second item, the GIVE texts), by kind. */
void ParleyWindow::describe_barter_give(ParleyOfferMsg *second_offer,i32 *second_items)
{
  iptr label;
  
  switch(*second_items) {
  case 5:
  case 6:
    label = (iptr)(label_string(label_table.string_ids[0x347]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_give_treaty_crlf_str));
    describe_treaty(second_offer->item.value,1);
    break;
  case 8:
    label = (iptr)(label_string(label_table.string_ids[0x32e]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_give_adv_crlf_str));
    parley_describe_advances((ParleyOfferItem *)second_items);
    break;
  case 9:
    label = (iptr)(label_string(label_table.string_ids[0x32f]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_give_gold_crlf_str));
    parley_desc_append((const char *)&str_empty_140);
    parley_describe_gold(second_offer->item.value);
    break;
  case 10:
    label = (iptr)(label_string(label_table.string_ids[0x330]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_give_units_crlf_str));
    parley_describe_units((ParleyOfferItem *)second_items);
    break;
  case 0xc:
    label = (iptr)(label_string(label_table.string_ids[0x333]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_give_war_crlf_str));
    parley_describe_war_targets((ParleyOfferItem *)second_items);
    break;
  case 0xd:
    label = (iptr)(label_string(label_table.string_ids[0x332]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_give_maps_crlf_str));
    parley_describe_maps(second_offer->item.value,0);
    break;
  case 0x11:
    label = (iptr)(label_string(label_table.string_ids[0x331]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_give_cities_crlf_str));
    parley_describe_cities((ParleyOfferItem *)second_items);
  }
}

/* Part of ParleyWindow::build_description (004dbee6): an exchange (mode 0xe):
   the item wanted for it (the first item, the WANT/FOR texts), by kind. */
void ParleyWindow::describe_barter_want(ParleyOfferMsg *offer,int *first_items)
{
  iptr label;
  
  switch(*first_items) {
  case 5:
  case 6:
    label = (iptr)(label_string(label_table.string_ids[0x348]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_exch_for_treaty_crlf_str));
    describe_treaty(offer->item.value,1);
    break;
  case 8:
    label = (iptr)(label_string(label_table.string_ids[0x334]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_want_adv_crlf_str));
    parley_describe_advances((ParleyOfferItem *)first_items);
    break;
  case 9:
    label = (iptr)(label_string(label_table.string_ids[0x335]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_want_gold_crlf_str));
    parley_desc_append((const char *)&str_empty_141);
    parley_describe_gold(offer->item.value);
    break;
  case 10:
    label = (iptr)(label_string(label_table.string_ids[0x336]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_desc_want_units_crlf_str));
    parley_describe_units((ParleyOfferItem *)first_items);
    break;
  case 0xc:
    label = (iptr)(label_string(label_table.string_ids[0x339]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_exch_for_war_targets_crlf_str));
    parley_describe_war_targets((ParleyOfferItem *)first_items);
    break;
  case 0xd:
    label = (iptr)(label_string(label_table.string_ids[0x338]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_exch_for_maps_crlf_str));
    parley_describe_maps(offer->item.value,1);
    break;
  case 0x11:
    label = (iptr)(label_string(label_table.string_ids[0x337]));
    parley_desc_append((const char *)label);
    parley_desc_append((const char *)((i8 *)&parley_exch_for_cities_crlf_str));
    parley_describe_cities((ParleyOfferItem *)first_items);
  }
}

/* Part of ParleyWindow::build_description (004dbee6): a demand (mode 0xf):
   its title by kind, then what is demanded. */
void ParleyWindow::describe_threat(ParleyOfferMsg *offer,int *first_items)
{
  iptr label;
  
  label = (iptr)(label_string(label_table.string_ids[0x32b]));
  parley_desc_append((const char *)label);
  parley_desc_append((const char *)append_space_str);
  if (*first_items == 0xc) {
    label = (iptr)(label_string(label_table.string_ids[0x346]));
    parley_desc_append((const char *)label);
  }
  else if ((*first_items == 5) || (*first_items == 6)) {
    label = (iptr)(label_string(label_table.string_ids[0x345]));
    parley_desc_append((const char *)label);
  }
  else {
    label = (iptr)(label_string(label_table.string_ids[0x32c]));
    parley_desc_append((const char *)label);
  }
  parley_desc_append((const char *)((i8 *)&parley_desc_threat_crlf_str));
  switch(*first_items) {
  case 5:
  case 6:
    describe_treaty(offer->item.value,1);
    break;
  case 8:
    parley_describe_advances((ParleyOfferItem *)first_items);
    break;
  case 9:
    parley_describe_gold(offer->item.value);
    break;
  case 10:
    parley_describe_units((ParleyOfferItem *)first_items);
    break;
  case 0xc:
    parley_describe_war_targets((ParleyOfferItem *)first_items);
    break;
  case 0xd:
    parley_describe_maps(offer->item.value,1);
    break;
  case 0x11:
    parley_describe_cities((ParleyOfferItem *)first_items);
  }
}

/* 004dbee6 auto */
void ParleyWindow::build_description(ParleyOfferMsg *offer,int received)

{
  int maps_kind;
  int *first_items;
  ParleyOfferMsg *second_offer;
  i32 *second_items;
  size_t desc_len;
  iptr label;
  
  if (offer != 0) {
    first_items = &offer->item.type;
    second_offer = (ParleyOfferMsg *)(offer->item.size + (iptr)offer);
    second_items = (i32 *)&second_offer->item.type;
    str_clear((char *)&text_section_index_pool_block.parley_description);
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (desc_len != 0) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_strlen_parleydescription_0,(const char*)&str_d_ss_franklinton_parleywin_transaction_c_3,0x120);
    }
    if (received == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x31c]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_transmitted_crlf_str));
    }
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (0x7ff < desc_len) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_strlen_parleydescription_2048,(const char*)&str_d_ss_franklinton_parleywin_transaction_c_4,0x126);
    }
    switch(offer->mode) {
    case 6:
      describe_treaty(offer->item.value,0);
      break;
    case 7:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_adv_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x327]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_adv_head_crlf_str));
      parley_describe_advances((ParleyOfferItem *)first_items);
      break;
    case 8:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_gold_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x329]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_gold_head_crlf_str));
      parley_desc_append((const char *)&str_empty_139);
      parley_describe_gold(offer->item.value);
      break;
    case 9:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_units_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x328]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_units_head_crlf_str));
      parley_describe_units((ParleyOfferItem *)first_items);
      break;
    case 10:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_cities_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x32a]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_cities_head_crlf_str));
      parley_describe_cities((ParleyOfferItem *)first_items);
      break;
    case 0xb:
      label = (iptr)(label_string(label_table.string_ids[0x326]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_gift_maps_crlf_str));
      parley_describe_maps(offer->item.value,0);
      break;
    case 0xc:
      label = (iptr)(label_string(label_table.string_ids[0x321]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_war_offer_crlf_str));
      label = (iptr)(label_string(label_table.string_ids[0x325]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_war_head_crlf_str));
      parley_describe_war_targets((ParleyOfferItem *)first_items);
      break;
    case 0xd:
      label = (iptr)(label_string(label_table.string_ids[0x321]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_maps_offer_crlf_str));
      maps_kind = offer->item.value;
      if (maps_kind == 0) {
        label = (iptr)(label_string(label_table.string_ids[0x322]));
        parley_desc_append((const char *)label);
      }
      else if (maps_kind == 1) {
        label = (iptr)(label_string(label_table.string_ids[0x323]));
        parley_desc_append((const char *)label);
      }
      else if (maps_kind == 2) {
        label = (iptr)(label_string(label_table.string_ids[0x324]));
        parley_desc_append((const char *)label);
      }
      parley_desc_append((const char *)((i8 *)&parley_desc_maps_kind_crlf_str));
      break;
    case 0xe:
      label = (iptr)(label_string(label_table.string_ids[0x32d]));
      parley_desc_append((const char *)label);
      parley_desc_append((const char *)((i8 *)&parley_desc_barter_crlf_str));
      describe_barter_give(second_offer,second_items);
      parley_desc_append((const char *)((i8 *)&parley_desc_barter_sep_crlf_str));
      describe_barter_want(offer,first_items);
      break;
    case 0xf:
      describe_threat(offer,first_items);
    }
    desc_len = _strlen(((i8 *)&text_section_index_pool_block.parley_description));
    if (0x7ff < desc_len) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_strlen_parleydescription_2048_2,(const char*)&str_d_ss_franklinton_parleywin_transaction_c_5,0x1f1);
    }
  }
  return;
}

/* 004dcafa auto */
void __cdecl parley_describe_advances(ParleyOfferItem *items)

{
  int count;
  iptr adv_name;
  int i;
  int *adv_ptr;
  
  count = items->value;
  adv_ptr = (int *)(items + 1);   /* the ids after the item */
  for (i = 0; i < count; i++) {
    adv_name = (iptr)(label_string(advances[*adv_ptr].name));
    parley_desc_append((const char *)adv_name);
    if (1 < count) {
      if (i < count - 2) {
        parley_desc_append((const char *)((i8 *)&parley_advances_comma_str));
      }
      else if (count - 2 == i) {
        parley_desc_append((const char *)append_space_str);
        adv_name = (iptr)(label_string(label_table.string_ids[0x33a]));
        parley_desc_append((const char *)adv_name);
        parley_desc_append((const char *)append_space_str);
      }
      else {
        parley_desc_append((const char *)((i8 *)&parley_advances_period_str));
      }
    }
    adv_ptr++;
  }
  parley_desc_append((const char *)((i8 *)&parley_advances_crlf_str));
  return;
}

/* 004dcc0c auto */
void __cdecl parley_describe_gold(int gold)

{
  const void *label;
  char gold_text[16];
  
  __itoa(gold,gold_text,10);
  parley_desc_append((const char *)gold_text);
  parley_desc_append((const char *)append_space_str);
  label = (label_string(label_table.string_ids[0x33b]));
  parley_desc_append((const char *)(const char *)label);
  parley_desc_append((const char *)((i8 *)&parley_gold_crlf_str));
  return;
}

/* 004dcc83 auto */
void __cdecl parley_describe_units(ParleyOfferItem *items)

{
  iptr unit;
  iptr label;
  int type_counts[62];
  int listed;
  iptr unit_ids;
  iptr unit_type;
  int distinct_types;
  int unit_count;
  char count_text[16];
  
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    type_counts[unit_type] = 0;
  }
  unit_count = items->value;
  unit_ids = (iptr)items + 0xc;
  distinct_types = 0;
  for (unit_type = 0; unit_type < unit_count; unit_type++) {
    unit = parley_wnd_block.find_unit_by_id(*(i32 *)(unit_ids + unit_type * 4));
    if (-1 < unit) {
      if (type_counts[units[unit].type] == 0) {
        distinct_types++;
      }
      type_counts[units[unit].type] = type_counts[units[unit].type] + 1;
    }
  }
  listed = 0;
  for (unit_type = 0; unit_type < 0x3e; unit_type++) {
    if (type_counts[unit_type] != 0) {
      parley_desc_append(label_string(unit_types[unit_type].name));
      parley_desc_append((const char *)((i8 *)&parley_units_open_paren_str));
      __itoa(type_counts[unit_type],count_text,10);
      parley_desc_append((const char *)count_text);
      parley_desc_append((const char *)((i8 *)&parley_units_close_paren_str));
      if (1 < unit_count) {
        if (listed < distinct_types - 2) {
          parley_desc_append((const char *)((i8 *)&parley_units_comma_str));
        }
        else if (distinct_types - 2 == listed) {
          parley_desc_append((const char *)append_space_str);
          label = (iptr)(label_string(label_table.string_ids[0x33a]));
          parley_desc_append((const char *)label);
          parley_desc_append((const char *)append_space_str);
        }
        else {
          parley_desc_append((const char *)((i8 *)&parley_units_period_str));
        }
      }
      listed++;
    }
  }
  parley_desc_append((const char *)((i8 *)&parley_units_crlf_str));
  return;
}

/* 004dcea5 auto */
void __cdecl parley_describe_cities(ParleyOfferItem *items)

{
  int count;
  int city;
  iptr label;
  int i;
  char size_text[16];
  
  count = items->value;
  for (i = 0; i < count; i++) {
    city = parley_wnd_block.find_city_by_id(*(i32 *)((iptr)items + 0xc + i * 8));
    parley_desc_append((const char *)&cities[city].name);
    parley_desc_append((const char *)((i8 *)&parley_cities_open_paren_str));
    __itoa(*(int *)((iptr)items + 0x10 + i * 8),size_text,10);
    parley_desc_append((const char *)size_text);
    parley_desc_append((const char *)((i8 *)&parley_cities_close_paren_str));
    if (1 < count) {
      if (i < count - 2) {
        parley_desc_append((const char *)((i8 *)&parley_cities_comma_str));
      }
      else if (count - 2 == i) {
        parley_desc_append((const char *)append_space_str);
        label = (iptr)(label_string(label_table.string_ids[0x33a]));
        parley_desc_append((const char *)label);
        parley_desc_append((const char *)append_space_str);
      }
      else {
        parley_desc_append((const char *)((i8 *)&parley_cities_period_str));
      }
    }
  }
  parley_desc_append((const char *)((i8 *)&parley_cities_crlf_str));
  return;
}

/* 004dd016 auto */
void __cdecl parley_describe_maps(int map_kind,int exchange)

{
  iptr label;
  
  if (map_kind == 0) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x33c]));
      parley_desc_append((const char *)label);
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x33d]));
      parley_desc_append((const char *)label);
    }
  }
  else if (map_kind == 1) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x33e]));
      parley_desc_append((const char *)label);
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x33f]));
      parley_desc_append((const char *)label);
    }
  }
  else if (map_kind == 2) {
    if (exchange == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x340]));
      parley_desc_append((const char *)label);
    }
    else {
      label = (iptr)(label_string(label_table.string_ids[0x341]));
      parley_desc_append((const char *)label);
    }
  }
  parley_desc_append((const char *)((i8 *)&parley_maps_crlf_str));
  return;
}

/* 004dd176 auto */
void __cdecl parley_describe_war_targets(ParleyOfferItem *items)

{
  int count;
  iptr text;
  iptr *civ_ptr;
  int i;
  
  count = items->value;
  civ_ptr = (iptr *)(items + 1);   /* the ids after the item */
  for (i = 0; i < count; i++) {
    text = (iptr)(civ_name_plural(*civ_ptr));
    parley_desc_append((const char *)text);
    if (1 < count) {
      if (i < count - 2) {
        parley_desc_append((const char *)((i8 *)&parley_war_targets_comma_str));
      }
      else if (count - 2 == i) {
        parley_desc_append((const char *)append_space_str);
        text = (iptr)(label_string(label_table.string_ids[0x33a]));
        parley_desc_append((const char *)text);
        parley_desc_append((const char *)append_space_str);
      }
      else {
        parley_desc_append((const char *)((i8 *)&parley_war_targets_period_str));
      }
    }
    civ_ptr++;
  }
  parley_desc_append((const char *)((i8 *)&parley_war_targets_crlf_str));
  return;
}

/* 004dd285 auto */
void __cdecl parley_execute_transaction(ParleyOfferMsg *offer)

{
  i32 *first_items;
  ParleyOfferMsg *second_offer;
  i32 *second_items;
  iptr civ;
  iptr civ2;
  
  if (offer != 0) {
    dbg_logf((const char *)&str_start_executeparleytransaction_message_d,net_send_sequence);
    civ = offer->civ;
    civ2 = offer->other_civ;
    first_items = (i32 *)&offer->item.type;
    second_offer = (ParleyOfferMsg *)(offer->item.size + (iptr)offer);
    second_items = (i32 *)&second_offer->item.type;
    switch(offer->mode) {
    case 6:
      parley_apply_treaty(offer->item.value,civ,civ2);
      break;
    case 7:
      parley_transfer_advances(civ2,civ,(ParleyOfferItem *)first_items);
      break;
    case 8:
      parley_transfer_gold(civ2,civ,offer->item.value);
      break;
    case 9:
      parley_transfer_units(civ2,(ParleyOfferItem *)first_items);
      break;
    case 10:
      parley_transfer_cities(civ2,(ParleyOfferItem *)first_items);
      break;
    case 0xb:
      parley_give_maps(civ2,civ,offer->item.value);
      break;
    case 0xc:
      parley_apply_war_pact(civ,(ParleyOfferItem *)first_items);
      parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
      break;
    case 0xd:
      parley_give_maps(civ,civ2,offer->item.value);
      parley_give_maps(civ2,civ,offer->item.value);
      break;
    case 0xe:
      switch(*second_items) {
      case 5:
      case 6:
        parley_apply_treaty(second_offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ2,civ,(ParleyOfferItem *)second_items);
        break;
      case 9:
        parley_transfer_gold(civ2,civ,second_offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ2,(ParleyOfferItem *)second_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ,(ParleyOfferItem *)second_items);
        break;
      case 0xd:
        parley_give_maps(civ2,civ,second_offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ2,(ParleyOfferItem *)second_items);
      }
      switch(*first_items) {
      case 5:
      case 6:
        parley_apply_treaty(offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ,civ2,(ParleyOfferItem *)first_items);
        break;
      case 9:
        parley_transfer_gold(civ,civ2,offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ,(ParleyOfferItem *)first_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
        break;
      case 0xd:
        parley_give_maps(civ,civ2,offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ,(ParleyOfferItem *)first_items);
      }
      break;
    case 0xf:
      switch(*first_items) {
      case 5:
      case 6:
        parley_apply_treaty(offer->item.value,civ,civ2);
        break;
      case 8:
        parley_transfer_advances(civ,civ2,(ParleyOfferItem *)first_items);
        break;
      case 9:
        parley_transfer_gold(civ,civ2,offer->item.value);
        break;
      case 10:
        parley_transfer_units(civ,(ParleyOfferItem *)first_items);
        break;
      case 0xc:
        parley_apply_war_pact(civ2,(ParleyOfferItem *)first_items);
        break;
      case 0xd:
        parley_give_maps(civ,civ2,offer->item.value);
        break;
      case 0x11:
        parley_transfer_cities(civ,(ParleyOfferItem *)first_items);
      }
    }
    net_send_state_diff(0xff,2,0,0,0);
    if (human_civ[0] == civ2) {
      status_redraw(1);
    }
    else {
      net_send_message(NM_INFO_UPDATE,net.net_slots[net.net_civ_slots[civ2]].id,0,0,0,0,
                         0,0,0,0);
    }
    if (human_civ[0] == civ) {
      status_redraw(1);
    }
    else {
      net_send_message(NM_INFO_UPDATE,net.net_slots[net.net_civ_slots[civ]].id,0,0,0,0,
                         0,0,0,0);
    }
    if ((civ_unit_counts_block.civ_unit_totals[civ2] == 0) && (civs[civ2].num_cities == 0)) {
      civ_check_destroyed(civ2,civ);
    }
    if ((civ_unit_counts_block.civ_unit_totals[civ] == 0) && (civs[civ].num_cities == 0)) {
      civ_check_destroyed(civ,civ2);
    }
    dbg_logf((const char *)&str_end_executeparleytransaction_message_d,net_send_sequence);
  }
  return;
}

/* 004dd8ad auto */
void __cdecl parley_give_maps(int receiver,int civ,int map_kind)

{
  byte recv_bits;
  byte giver_bits;
  uint recv_mask;
  uint giver_mask;
  Tile *tile;
  byte *recv_known;
  byte *giver_known;
  Tile *tile2;
  Tile *tile3;
  int y;
  iptr x;
  int in_bounds;
  i32 set_flag;
  i32 redraw_flag;
  iptr target_civ;
  iptr unit;
  byte known_bits;
  byte recv_bit;
  iptr city;
  uint y2;
  uint x2;
  iptr i;
  
  recv_mask = 1 << ((byte)receiver & 0x1f);
  giver_mask = 1 << ((byte)civ & 0x1f);
  y2 = 0;
  x2 = 0;
  net_batch_begin();
  for (i = 0; i < map.area; i++) {
    tile = tile_at(x2,y2);
    recv_known = known_at(x2,y2,receiver);
    giver_known = known_at(x2,y2,civ);
    if ((giver_mask & tile->visible) != 0) {
      if ((recv_mask & tile->visible) == 0) {
        known_set(x2,y2,0,receiver,0,1);
        tile_set_visible(x2,y2,recv_mask,1,1);
      }
      if (map_kind == 0) {
        if (((*recv_known & 0x80) == 0) || ((*giver_known & 0x80) == 0)) {
          giver_bits = *giver_known;
          recv_bits = *recv_known;
          tile2 = tile_at(x2,y2);
          known_bits = (giver_bits | recv_bits) & tile2->improvements & TILE_POLLUTION;
        }
        else {
          known_bits = 0x80;
        }
        redraw_flag = 1;
        set_flag = 0;
        target_civ = receiver;
        recv_known = known_at(x2,y2,receiver);
        known_set(x2,y2,*recv_known & 0xffffff7f | (uint)known_bits,target_civ,set_flag,redraw_flag);
        redraw_flag = 1;
        set_flag = 1;
        x = receiver;
        tile3 = tile_at(x2,y2);
        giver_bits = tile3->improvements;
        recv_known = known_at(x2,y2,civ);
        known_set(x2,y2,(uint)(giver_bits & *recv_known),x,set_flag,redraw_flag);
      }
    }
    x2 += 2;
    if ((int)map.width <= (int)x2) {
      y2++;
      x2 = y2 & 1;
    }
  }
  recv_bit = (byte)recv_mask;
  if (map_kind == 0) {
    for (unit = 0; unit < game.num_units; unit++) {
      if ((units[unit].id != 0) &&
         (((giver_mask & units[unit].visibility) != 0 || (units[unit].owner == civ)))) {
        units[unit].visibility = units[unit].visibility | recv_bit;
        tile = tile_at(units[unit].x,units[unit].y);
        if (((recv_mask & tile->visible) == 0) || ((giver_mask & tile->visible) == 0)) {
          for (i = 0; i < 0x15; i++) {
            x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
            y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
            in_bounds = map_in_bounds(x,y);
            if (in_bounds != 0) {
              tile_set_visible(x,y,recv_mask,1,1);
              tile_set_visible(x,y,giver_mask,1,1);
            }
          }
        }
      }
    }
  }
  if ((map_kind == 0) || (map_kind == 1)) {
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) &&
         (((giver_mask & (int)(char)cities[city].known_to) != 0 ||
          (cities[city].owner == civ)))) {
        if (cities[city].owner == civ) {
          *(i8 *)&cities[city].revealed_size[receiver] = cities[city].size;
        }
        else {
          *(i8 *)&cities[city].revealed_size[receiver] =
               *(i8 *)&cities[city].revealed_size[civ];
        }
        cities[city].known_to = cities[city].known_to | recv_bit;
        tile = tile_at(cities[city].x,cities[city].y);
        if (((recv_mask & tile->visible) == 0) || ((giver_mask & tile->visible) == 0)) {
          for (i = 0; i < 0x15; i++) {
            x = map_wrap_x((int)(((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
            y = (int)(((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
            in_bounds = map_in_bounds(x,y);
            if (in_bounds != 0) {
              tile_set_visible(x,y,recv_mask,1,1);
              tile_set_visible(x,y,giver_mask,1,1);
            }
          }
        }
      }
    }
  }
  net_batch_flush();
  net_send_state_diff(0xff,2,0,0,0);
  if (human_civ[0] == receiver) {
    map_redraw_all_views(human_civ[0],1);
  }
  else {
    net_send_message(NM_DRAW_MAP,net.net_slots[net.net_civ_slots[receiver]].id,0,0,0,0,
                       0,0,0,0);
  }
  return;
}

/* 004dde9e auto */
void __cdecl parley_apply_war_pact(int civ,ParleyOfferItem *items)

{
  int count;
  iptr *civ_ptr;
  int i;
  
  count = items->value;
  civ_ptr = (iptr *)(items + 1);   /* the ids after the item */
  for (i = 0; i < count; i++) {
    diplo_set_treaty_flags(civ,*civ_ptr,0x2000);
    civ_ptr++;
  }
  return;
}

/* 004ddf04 auto */
void __cdecl parley_transfer_gold(int civ2,int civ,i32 amount)

{
  int attitude_value;
  
  if (civs[civ].gold < amount) {
    amount = civs[civ].gold;
  }
  civs[civ].gold -= amount;
  civs[civ2].gold += amount;
  attitude_value = diplo_gold_attitude_value(amount);
  civ_adjust_attitude(civ2,civ,-((attitude_value * 3) / 2));
  return;
}

/* 004ddfb2 auto */
void __cdecl parley_transfer_advances(int civ,int civ2,ParleyOfferItem *items)

{
  int count;
  int has_adv;
  iptr *adv_ptr;
  int i;
  
  count = items->value;
  adv_ptr = (iptr *)(items + 1);   /* the ids after the item */
  for (i = 0; i < count; i++) {
    has_adv = civ_has_advance(civ2,*adv_ptr);
    if (has_adv != 0) {
      has_adv = civ_has_advance(civ,*adv_ptr);
      if (has_adv == 0) {
        civ_give_advance(civ,*adv_ptr,civ2,1,0);
      }
    }
    adv_ptr++;
  }
  return;
}

/* 004de049 auto */
void __cdecl parley_transfer_cities(int civ,ParleyOfferItem *items)

{
  int count;
  iptr city;
  i32 *city_id_ptr;
  int i;
  
  count = items->value;
  city_id_ptr = (i32 *)(items + 1);   /* the ids after the item */
  for (i = 0; i < count; i++) {
    city = parley_wnd_block.find_city_by_id(*city_id_ptr);
    if ((-1 < city) && (cities[city].id != 0)) {
      parley_cede_city(city,civ);
    }
    city_id_ptr += 2;
  }
  return;
}

/* 004de0e2 auto */
void __cdecl parley_cede_city(uptr city,int civ)

{
  char *count_ptr;
  char civ3;
  iptr civ2;
  int x;
  int y;
  iptr x2;
  iptr y3;
  iptr in_bounds;
  int y2;
  iptr in_bounds2;
  Tile *tile;
  uptr leonardo_city;
  iptr improvement;
  iptr unit;
  iptr unit_type;
  iptr i;
  int defense_ratio;
  
  civ3 = cities[city].owner;
  civ2 = (iptr)civ3;
  x = (int)cities[city].x;
  y = (int)cities[city].y;
  city_set_improvement(city,1,0);
  city_set_improvement(city,4,0);
  city_set_improvement(city,0xb,0);
  city_set_improvement(city,7,0);
  civs[civ].last_city_turn = game.turn;
  civs[civ].num_cities++;
  if (civs[civ2].num_cities != 0) {
    civs[civ2].num_cities--;
  }
  net_batch_begin();
  for (i = 0; i < 0x15; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x);
    y3 = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y;
    in_bounds = map_in_bounds(x2,y3);
    if (in_bounds != 0) {
      tile_set_radius_civ(x2,y3,civ,1);
      tile_set_visible(x2,y3,1 << ((byte)civ & 0x1f),1,1);
    }
  }
  cities[city].owner = (byte)civ;
  city_reveal_to_civ(city,civ2);
  if (scenario_events_loaded != 0) {
    ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_city_taken((const char*)&cities[city].name,civ,civ2);
  }
  tile_set_owner(x,y,civ,1);
  unit = (iptr)game.num_units;
  while (unit = unit - 1, -1 < unit) {
    if ((units[unit].id != 0) && (units[unit].owner == civ2)) {
      if (((byte)units[unit].home_city == city) &&
         ((cities[city].x != units[unit].x || (cities[city].y != units[unit].y)))) {
        unit_delete_with_cargo(unit,1);
      }
      else if ((cities[city].x == units[unit].x) && (cities[city].y == units[unit].y))
      {
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *count_ptr = *count_ptr + 1;
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *count_ptr = *count_ptr - 1;
        units[unit].owner = (byte)civ;
        units[unit].home_city = -1;
        units[unit].moves_used = '\0';
        units[unit].orders = ORDER_NONE;
        city = city_find_nearest(units[unit].x,units[unit].y,civ,
                                     0xffffffff,-1);
        if ((-1 < (int)city) && (cities[city].owner == civ)) {
          units[unit].home_city = (i8)city;
        }
        unit_set_seen_by(unit,civ);
        if ((char)unit_types[units[unit].type].role < 5) {
          civs[civ].num_military_units++;
        }
        civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
        if ((civs[civ2].num_military_units != 0) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          civs[civ2].num_military_units--;
        }
        if (civ_unit_counts_block.civ_unit_totals[civ2] != 0) {
          civ_unit_counts_block.civ_unit_totals[civ2] = civ_unit_counts_block.civ_unit_totals[civ2] - 1;
        }
      }
    }
  }
  if ((cities[city].building < '\0') ||
     (y2 = civ_can_build_unit(civ,city,cities[city].building), y2 != 0)) {
    if (!(-1 < cities[city].building)) {
      if (cities[city].building < 1) {
        improvement = ~(int)cities[city].building + 1;
      }
      else {
        improvement = (iptr)cities[city].building;
      }
      y2 = city_can_build_improvement(civ,city,improvement);
    }
  }
  /* (y2: 0 if the new owner cannot build the city's production) */
  if (y2 == 0) {
    cities[city].building = UNIT_SETTLERS;
    for (unit_type = 0x3e; -1 < unit_type; unit_type--) {
      if ((unit_types[unit_type].role == ROLE_DEFEND) &&
         (y2 = civ_can_build_unit(civ,city,unit_type), y2 != 0)) {
        defense_ratio = ((int)(char)unit_types[unit_type].defense << 3) /
                  (int)(char)unit_types[unit_type].cost;
        if ((unit_types[unit_type].flags & UTYPE_X2_VS_HORSE) != 0) {
          defense_ratio++;
        }
        if (0 < defense_ratio) {
          cities[city].building = (i8)unit_type;
        }
        break;
      }
    }
  }
  map_redraw_neighbors_all_views((int)cities[city].x,cities[city].y);
  net_send_state_diff(0xff,2,0,0,0);
  net_send_message(NM_DRAW_AREA,0xff,cities[city].x,cities[city].y,0,0,0,0,0,0);
  tile = tile_at(x,y);
  if ((1 << ((byte)civ & 0x1f) & (uint)tile->visible) == 0) {
    tile_set_visible(x,y,1 << ((byte)civ & 0x1f),1,1);
    map_redraw_tile_all_views(x,y);
  }
  for (i = 0; i < 8; i++) {
    x2 = map_wrap_x((((i8 *)&dir_dx_table_block.dir_dx_table))[i] + x);
    y2 = (((i8 *)&dir_dy_table_block.dir_dy_table))[i] + y;
    in_bounds2 = map_in_bounds(x2,y2);
    if ((in_bounds2 != 0) &&
       (tile = tile_at(x2,y2),
       (1 << ((byte)civ & 0x1f) & (uint)tile->visible) == 0)) {
      tile_set_visible(x2,y2,1 << ((byte)civ & 0x1f),1,1);
      map_redraw_tile_all_views(x2,y2);
    }
  }
  net_batch_flush();
  net_send_message(NM_DRAW_AREA,0xff,x,y,0,0,0,0,0,0);
  cities[city].founder = civ3;
  cities[city].turns_captured = (u8)game.turn;
  leonardo_city = wonder_get_city(WONDER_LEONARDO);
  if (leonardo_city == city) {
    civ_leonardo_upgrade(civ);
  }
  return;
}

/* 004de990 auto */
void __cdecl parley_transfer_units(int civ,ParleyOfferItem *items)

{
  char *count_ptr;
  i32 unit_id;
  int result;
  int x3;
  iptr unit;
  iptr city;
  iptr unit2;
  int i;
  int y2;
  int x2;
  int count;
  iptr civ2;
  int y3;
  i32 *id_ptr;
  
  count = items->value;
  id_ptr = (i32 *)(items + 1);   /* the ids after the item */
  for (i = 0; i < count; i++) {
    unit_id = *id_ptr;
    id_ptr++;
    unit = parley_wnd_block.find_unit_by_id(unit_id);
    if ((-1 < unit) && (civ2 = (iptr)units[unit].owner, civ != civ2)) {
      if ((unit_types[units[unit].type].domain == DOMAIN_SEA) &&
         (result = unit_stack_sum(unit,2), 1 < result)) {
        while (result = unit_stack_sum(unit,2), 1 < result) {
          unit2 = unit_stack_first(unit);
          if (unit2 == unit) {
            unit2 = unit_next_in_stack(unit2);
          }
          result = parley_find_unit_drop_tile(unit2,&x3,&y3,
                                     units[unit2].owner,1);
          if (result == 0) {
            unit_delete_with_cargo(unit2,1);
          }
          else {
            unit_relocate(unit2,x3,y3,1);
          }
        }
      }
      result = parley_find_unit_drop_tile(unit,&x2,&y2,civ,0);
      if (result != 0) {
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ * 0x594);
        *count_ptr = *count_ptr + 1;
        count_ptr = (char *)(units[unit].type + (iptr)((u8 *)&civs + 216) + civ2 * 0x594);
        *count_ptr = *count_ptr - 1;
        units[unit].owner = (i8)civ;
        units[unit].home_city = -1;
        units[unit].moves_used = '\0';
        units[unit].orders = ORDER_NONE;
        city = city_find_nearest(units[unit].x,units[unit].y,civ,
                                      0xffffffff,-1);
        if ((-1 < city) && (cities[city].owner == civ)) {
          units[unit].home_city = (i8)city;
        }
        unit_set_seen_by(unit,civ);
        if ((char)unit_types[units[unit].type].role < 5) {
          civs[civ].num_military_units++;
        }
        civ_unit_counts_block.civ_unit_totals[civ] = civ_unit_counts_block.civ_unit_totals[civ] + 1;
        if ((civs[civ2].num_military_units != 0) &&
           ((char)unit_types[units[unit].type].role < 5)) {
          civs[civ2].num_military_units--;
        }
        if (civ_unit_counts_block.civ_unit_totals[civ2] != 0) {
          civ_unit_counts_block.civ_unit_totals[civ2] = civ_unit_counts_block.civ_unit_totals[civ2] - 1;
        }
        unit_relocate(unit,x2,y2,1);
        unit_reveal_surroundings(unit,0);
      }
    }
  }
  XD_FlushSendBuffer(5000);
  return;
}

/* 004ded07 auto */
i32 __cdecl parley_find_unit_drop_tile(int unit,i32 *x,i32 *y,int civ,int must_move)

{
  iptr x2;
  int y2;
  int result;
  i32 wrapped_x;
  iptr i;
  i32 found;
  
  *x = (int)units[unit].x;
  x2 = *x;
  *y = (int)units[unit].y;
  y2 = *y;
  found = 1;
  if (((must_move != 0) || (result = tile_city_owner(x2,y2), result != -1)) ||
     (result = unit_stack_sum(unit,2), 1 < result)) {
    found = 0;
    for (i = 0; i < 0x2d; i++) {
      wrapped_x = map_wrap_x((((i8 *)&dir_dy_table_block.city_tile_dx_table))[i] + x2);
      *x = wrapped_x;
      *y = (((i8 *)&dir_dy_table_block.city_tile_dy_table))[i] + y2;
      if ((((((must_move == 0) || (*x != x2)) || (*y != y2)) &&
           (result = map_in_bounds(*x,*y), result != 0)) &&
          (((result = tile_city_owner(*x,*y), result == civ ||
            (result = tile_unit_owner(*x,*y),
            result == civ)) ||
           ((result = tile_city_owner(*x,*y), result == -1 &&
            (result = tile_unit_owner(*x,*y), result == -1))))
          )) && ((((unit_types[units[unit].type].domain == DOMAIN_LAND &&
                   (result = tile_is_ocean(*x,*y), result == 0
                   )) || ((unit_types[units[unit].type].domain == DOMAIN_SEA &&
                          (result = tile_is_ocean(*x,*y),
                          result != 0)))) || (unit_types[units[unit].type].domain == DOMAIN_AIR)))) {
        return 1;
      }
    }
  }
  return found;
}

/* 004def54 auto */
void ParleyWindow::describe_treaty(int treaty_kind,int exchange)

{
  iptr label;
  
  if (exchange == 0) {
    switch(treaty_kind) {
    case 0:
      label = (iptr)(label_string(label_table.string_ids[0x31d]));
      parley_desc_append((const char *)label);
      break;
    case 1:
      label = (iptr)(label_string(label_table.string_ids[0x31e]));
      parley_desc_append((const char *)label);
      break;
    case 2:
      label = (iptr)(label_string(label_table.string_ids[0x31f]));
      parley_desc_append((const char *)label);
      break;
    case 3:
      label = (iptr)(label_string(label_table.string_ids[0x320]));
      parley_desc_append((const char *)label);
    }
  }
  else if (exchange == 1) {
    if (treaty_kind == 0) {
      label = (iptr)(label_string(label_table.string_ids[0x2f3]));
      parley_desc_append((const char *)label);
    }
    else if (treaty_kind == 1) {
      label = (iptr)(label_string(label_table.string_ids[0x2f4]));
      parley_desc_append((const char *)label);
    }
    else if (treaty_kind == 2) {
      label = (iptr)(label_string(label_table.string_ids[0x2f5]));
      parley_desc_append((const char *)label);
    }
  }
  parley_desc_append((const char *)((i8 *)&parley_treaty_crlf_str));
  return;
}

/* 004df10f auto */
void __cdecl parley_apply_treaty(i32 treaty_kind,int civ,int other_civ)

{
  switch(treaty_kind) {
  case 0:
    diplo_set_treaty_flags(civ,other_civ,2);
    break;
  case 1:
    diplo_set_treaty_flags(civ,other_civ,4);
    break;
  case 2:
    diplo_set_treaty_flags(civ,other_civ,8);
    break;
  case 3:
    if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_CEASEFIRE) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,2);
    }
    if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_PEACE) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,4);
    }
    if ((*(byte *)&civs[civ].treaties[other_civ] & TREATY_ALLIANCE) != 0) {
      diplo_clear_treaty_flags(civ,other_civ,8);
    }
  }
  return;
}

/* 0052d4d2 auto */
void __cdecl parley_on_treaty_type_select(i32 ctrl_id,i32 treaty_type)

{
  iptr active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->window.activate();
  parley->treaty_kind = treaty_type;
  return;
}

/* 0052d523 auto */
void __cdecl parley_on_demand_kind_select(i32 ctrl_id,u32 demand_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  this2->window.activate();
  this2->demand_kind = demand_kind;
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052d588 auto */
void __cdecl parley_on_map_choice_select(int side,i32 map_choice)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->window.activate();
  *(i32 *)((iptr)parley - 0xe6c + side * 4) = map_choice;
  return;
}

/* 0052d5dd auto */
void __cdecl parley_on_gift_kind_select(i32 ctrl_id,u32 gift_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  this2->window.activate();
  this2->gift_kind = gift_kind;
  switch(gift_kind) {
  case 0:
    this2->mode = 7;
    break;
  case 1:
    this2->mode = 8;
    break;
  case 2:
    this2->mode = 9;
    break;
  case 3:
    this2->mode = 10;
    break;
  case 4:
    this2->mode = 0xb;
  }
  this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052d6ff auto */
void __cdecl parley_on_proposal_kind_select(i32 ctrl_id,u32 proposal_kind)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  this2->window.activate();
  this2->proposal_kind = proposal_kind;
  if (proposal_kind == 0) {
    this2->mode = 0xc;
  }
  else if (proposal_kind == 1) {
    this2->mode = 0xd;
  }
  this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052d7dc auto */
void __cdecl parley_on_exchange_kind_select(int ctrl_id,i32 exchange_kind)

{
  iptr active_child;
  int selected;
  ParleyWindow *parley;
  uint is_right_side;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->window.activate();
  is_right_side = (uint)(ctrl_id == 0x3f4);
  switch(exchange_kind) {
  case 0:
    selected = ((RadioGroup *)parley->side_radios[is_right_side])->get_selected();
    ((RadioGroup *)parley->side_radios[is_right_side])->disable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(6);
    ((RadioGroup *)parley->side_radios[0])->enable_item(6);
    if (selected == 0) {
      ((RadioGroup *)parley->side_radios[is_right_side])->set_selected(1);
    }
    break;
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
    ((RadioGroup *)parley->side_radios[0])->enable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(6);
    ((RadioGroup *)parley->side_radios[0])->enable_item(6);
    break;
  case 6:
    selected = ((RadioGroup *)parley->side_radios[is_right_side])->get_selected();
    ((RadioGroup *)parley->side_radios[0])->enable_item(0);
    ((RadioGroup *)parley->side_radios[1])->enable_item(0);
    ((RadioGroup *)parley->side_radios[is_right_side])->disable_item(6);
    if (selected == 6) {
      ((RadioGroup *)parley->side_radios[is_right_side])->set_selected(0);
    }
  }
  return;
}

/* 0052d9a1 auto */
void __cdecl parley_on_radio_dblclick(i32 ctrl_id)

{
  iptr active_child;
  int enabled;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  enabled = ((Button *)parley->mode_button)->ctrl_is_enabled();
  if ((enabled != 0) ||
     (enabled = ((Button *)parley->mode2_button)->ctrl_is_enabled(), enabled != 0)) {
    parley->window.activate();
    parley_on_mode_chosen(ctrl_id);
  }
  return;
}

/* 0052da23 auto */
void __cdecl parley_on_conceal_info(void)

{
  iptr active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->local_units_revealed = 0;
  parley->local_cities_revealed = 0;
  net_send_message(NM_HIDE_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_cities_revealed,0,0,0,0,0,0);
  net_send_message(NM_HIDE_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_units_revealed,0,0,0,0,0,0);
  ((Control *)parley->conceal_button)->hide();
  ((Button *)parley->conceal_button)->disable();
  ((Control *)parley->reveal_button)->show_and_redraw();
  ((Button *)parley->reveal_button)->enable();
  return;
}

/* 0052db3d auto */
void __cdecl parley_on_reveal_info(void)

{
  iptr active_child;
  ParleyWindow *parley;
  
  wnd_get_active_child();
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->local_units_revealed = 1;
  parley->local_cities_revealed = 1;
  net_send_message(NM_REVEAL_CITY_INFO,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_cities_revealed,0,0,0,0,0,0);
  net_send_message(NM_REVEAL_CITY_ORIGIN,net.net_slots[net.net_civ_slots[(int)parley->other_civ]].id,
                     human_civ[0],(int)(char)parley->local_units_revealed,0,0,0,0,0,0);
  ((Control *)parley->conceal_button)->show_and_redraw();
  ((Button *)parley->conceal_button)->enable();
  ((Control *)parley->reveal_button)->hide();
  ((Button *)parley->reveal_button)->disable();
  return;
}

/* 0052dc7e auto */
void __cdecl parley_on_check_intel(void)

{
  bool has_intel_wonder;
  iptr active_child;
  int has_wonder;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = 0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  has_wonder = civ_has_wonder(human_civ[0],WONDER_UNITED_NATIONS);
  has_intel_wonder = true;
  if (has_wonder == 0) {
    has_wonder = civ_has_wonder(human_civ[0],WONDER_MARCO_POLO);
    if (has_wonder == 0) {
      has_intel_wonder = false;
    }
  }
  if ((((*(byte *)&civs[human_civ[0]].treaties[(int)parley->other_civ] & TREATY_EMBASSY) != 0)
      || (has_intel_wonder)) || (game.reveal_map != '\0')) {
    intel_report_show(human_civ[0],parley->other_civ);
  }
  else {
    popup_game_with_sprite(str_nointel_2,(Sprite *)&popup_info_icon_sprite,0);
  }
  return;
}

/* 0052dd73 auto */
void __cdecl parley_on_mode_chosen(i32 ctrl_id)

{
  iptr active_child;
  i32 value;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  this2->window.activate();
  switch(this2->mode) {
  case 3:
    value = ((RadioGroup *)this2->side_radios[1])->get_selected();
    this2->selected_action = value;
    if (this2->selected_action == 0) {
      this2->mode = 6;
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 1) {
      this2->mode = 0xc;
      if (this2->proposal_kind != 0) {
        this2->mode = 0xd;
      }
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 2) {
      if (this2->gift_kind == 0) {
        this2->mode = 7;
      }
      else if ((int)this2->gift_kind == 1) {
        this2->mode = 8;
      }
      else if ((int)this2->gift_kind == 2) {
        this2->mode = 9;
      }
      else if ((int)this2->gift_kind == 3) {
        this2->mode = 10;
      }
      else {
        this2->mode = 0xb;
      }
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 3) {
      this2->mode = 5;
      ((Button *)this2->never_mind_button)->enable();
    }
    else if ((int)this2->selected_action == 4) {
      this2->mode = 0xf;
      ((Button *)this2->never_mind_button)->enable();
    }
    this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
    this2->left_kind = (i32)this2->shown_left_kind;
    this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
    this2->right_kind = (i32)this2->shown_right_kind;
    this2->add_side_dialog((i32)this2->left_kind,0);
    this2->add_side_dialog((i32)this2->right_kind,1);
    break;
  case 5:
    value = ((RadioGroup *)this2->side_radios[0])->get_selected();
    this2->exchange_kind[0] = value;
    value = ((RadioGroup *)this2->side_radios[1])->get_selected();
    this2->exchange_kind[1] = value;
    this2->mode = 0xe;
    this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
    this2->left_kind = (i32)this2->shown_left_kind;
    this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
    this2->right_kind = (i32)this2->shown_right_kind;
    this2->add_side_dialog((i32)this2->left_kind,0);
    this2->add_side_dialog((i32)this2->right_kind,1);
    break;
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    if (this2->received_offer != 0) {
      operator_delete((void *)this2->received_offer);
      ((u8 *)this2 + 0x21c)[0x18] = '\0';
      ((u8 *)this2 + 0x21c)[0x19] = '\0';
      ((u8 *)this2 + 0x21c)[0x1a] = '\0';
      ((u8 *)this2 + 0x21c)[0x1b] = '\0';
    }
    value = (i32)(this2->send_offer(this2->mode));
    this2->received_offer = (ParleyOfferMsg *)value;
    if ((this2->mode == 6) && (this2->treaty_kind == 3)) {
      if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & TREATY_ALLIANCE) == 0) {
        if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & TREATY_PEACE) == 0) {
          if ((*(byte *)&civs[human_civ[0]].treaties[this2->other_civ] & TREATY_CEASEFIRE) != 0) {
            diplo_clear_treaty_flags(human_civ[0],this2->other_civ,2);
          }
        }
        else {
          diplo_clear_treaty_flags(human_civ[0],this2->other_civ,4);
        }
      }
      else {
        diplo_clear_treaty_flags(human_civ[0],this2->other_civ,8);
      }
      this2->add_side_dialog((i32)this2->left_kind,0);
      this2->add_side_dialog((i32)this2->right_kind,1);
    }
    else if (this2->received_offer != 0) {
      this2->mode = 1;
      this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
      this2->left_kind = (i32)this2->shown_left_kind;
      this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
      this2->right_kind = (i32)this2->shown_right_kind;
      this2->add_side_dialog((i32)this2->left_kind,0);
      this2->add_side_dialog((i32)this2->right_kind,1);
      ((Button *)this2->never_mind_button)->disable();
    }
  }
  return;
}

/* 0052e326 auto */
void __cdecl parley_on_never_mind(void)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  this2->window.activate();
  switch(this2->mode) {
  case 3:
    break;
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xf:
    this2->mode = 3;
    ((Button *)this2->never_mind_button)->disable();
    this2->shown_left_kind = parley_panes.mode3_left_kind;
    this2->left_kind = this2->shown_left_kind;
    this2->shown_right_kind = parley_panes.mode3_right_kind;
    this2->right_kind = this2->shown_right_kind;
    this2->add_side_dialog(this2->left_kind,0);
    this2->add_side_dialog(this2->right_kind,1);
    break;
  case 0xe:
    this2->mode = 5;
    this2->shown_left_kind = parley_panes.mode5_left_kind;
    this2->left_kind = this2->shown_left_kind;
    this2->shown_right_kind = parley_panes.mode5_right_kind;
    this2->right_kind = this2->shown_right_kind;
    this2->add_side_dialog(this2->left_kind,0);
    this2->add_side_dialog(this2->right_kind,1);
  }
  return;
}

/* 0052e4c9 auto */
void __cdecl parley_on_accept_offer(void)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->window.activate();
  net_send_message(NM_PARLEY_ACCEPT_OFFER,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  dlg_style.pending_idle_callback = turn_popup_timeout_idle;
  popup_game_with_sprite(str_parleyaccept2,(Sprite *)&popup_info_icon_sprite,0);
  parley->mode = 3;
  parley->set_mode_sides(1);
  return;
}

/* 0052e57c auto */
void __cdecl parley_on_no_thanks(void)

{
  iptr active_child;
  ParleyWindow *this2;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    this2 = (ParleyWindow *)0x0;
  }
  else {
    this2 = (ParleyWindow *)(active_child - 0x48);
  }
  this2->window.activate();
  net_send_message(NM_PARLEY_NO_THANKS,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  this2->mode = 2;
  this2->shown_left_kind = (((i32 *)&parley_panes.mode_left_kinds))[this2->mode * 2];
  this2->left_kind = this2->shown_left_kind;
  this2->shown_right_kind = (((i32 *)&parley_panes.mode_right_kinds))[this2->mode * 2];
  this2->right_kind = this2->shown_right_kind;
  this2->add_side_dialog(this2->left_kind,0);
  this2->add_side_dialog(this2->right_kind,1);
  return;
}

/* 0052e685 auto */
void __cdecl parley_on_counter_offer(void)

{
  int active_child;
  ParleyWindow *parley;
  
  active_child = wnd_get_active_child();
  if (active_child == 0) {
    parley = (ParleyWindow *)0x0;
  }
  else {
    parley = (ParleyWindow *)(active_child - 0x48);
  }
  parley->window.activate();
  net_send_message(NM_PARLEY_COUNTER_OFFER,net.net_slots[net.net_civ_slots[(int)parley_wnd_block.other_civ]].id,0,0,0
                     ,0,0,0,0,0);
  parley->mode = 3;
  parley->set_mode_sides(1);
  return;
}

/* 0052e71a auto */
void __cdecl chat_on_mute_checkbox(int ctrl_id)

{
  i32 checked;
  SurfaceWindow *chat_wnd2;
  
  chat_wnd2 = (SurfaceWindow *)wnd_get_active_child();
  if (chat_wnd2 != 0) {
    /* the active window is a SurfaceWindow's Wnd */
    chat_wnd2 = (SurfaceWindow *)((u8 *)chat_wnd2 - offsetof(SurfaceWindow, window));
  }
  chat_wnd2->window.activate();
  if ((0x428 < ctrl_id) && (ctrl_id < 0x430)) {
    checked = ((Checkbox *)chat_wnd_block.mute_checkboxes[(ctrl_id - 0x428)])->is_checked();
    chat_wnd_block.muted_civs[(ctrl_id - 0x428)] = checked;
  }
  return;
}

/* 0052e7b7 auto */
void __cdecl chat_on_send_checkbox(int ctrl_id)

{
  i32 checked;
  int slot;
  SurfaceWindow *chat_wnd2;
  
  chat_wnd2 = (SurfaceWindow *)wnd_get_active_child();
  if (chat_wnd2 != 0) {
    /* the active window is a SurfaceWindow's Wnd */
    chat_wnd2 = (SurfaceWindow *)((u8 *)chat_wnd2 - offsetof(SurfaceWindow, window));
  }
  chat_wnd2->window.activate();
  switch(ctrl_id) {
  case 0x415:
    chat_wnd_block.remain_anonymous = ((Checkbox *)chat_wnd_block.anonymous_checkbox)->is_checked();
    break;
  case 0x416:
    chat_wnd_block.broadcast_all = ((Checkbox *)chat_wnd_block.broadcast_checkbox)->is_checked();
    if (chat_wnd_block.broadcast_all != 0) {
      for (slot = 1; slot < 8; slot++) {
        if (chat_wnd_block.send_checkboxes[slot] != 0) {
          ((Checkbox *)chat_wnd_block.send_checkboxes[slot])->set_checked(1);
          ((Control *)chat_wnd_block.send_checkboxes[slot])->show_and_redraw();
          chat_wnd_block.send_to_civs[slot] = 1;
        }
      }
    }
    break;
  case 0x417:
  case 0x418:
  case 0x419:
  case 0x41a:
  case 0x41b:
  case 0x41c:
  case 0x41d:
    ctrl_id -= 0x416;
    checked = ((Checkbox *)chat_wnd_block.send_checkboxes[ctrl_id])->is_checked();
    chat_wnd_block.send_to_civs[ctrl_id] = checked;
    if ((chat_wnd_block.send_to_civs[ctrl_id] == 0) && (chat_wnd_block.broadcast_all != 0)) {
      ((Checkbox *)chat_wnd_block.broadcast_checkbox)->set_checked(0);
      ((Control *)chat_wnd_block.broadcast_checkbox)->show_and_redraw();
      chat_wnd_block.broadcast_all = 0;
    }
    break;
  case 0x430:
    chat_wnd_block.mute_anonymous = ((Checkbox *)chat_wnd_block.mute_anonymous_checkbox)->is_checked();
  }
  return;
}

/* 0052e971 auto */
void ParleyWindow::update_buttons()

{
  if ((mode_button != 0) && (mode2_button != 0)) {
    if (mode != 2) {
      ((Control *)conceal_button)->hide();
      ((Button *)conceal_button)->disable();
      ((Control *)reveal_button)->hide();
      ((Button *)reveal_button)->disable();
    }
    switch(mode) {
    case 1:
    case 2:
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)mode_button)->show_and_redraw();
      ((Button *)mode_button)->disable();
      if (mode == 2) {
        if ((local_units_revealed == '\0') && (local_cities_revealed == '\0')) {
          ((Control *)conceal_button)->hide();
          ((Button *)conceal_button)->disable();
          ((Control *)reveal_button)->show_and_redraw();
          ((Button *)reveal_button)->enable();
        }
        else {
          ((Control *)conceal_button)->show_and_redraw();
          ((Button *)conceal_button)->enable();
          ((Control *)reveal_button)->hide();
          ((Button *)reveal_button)->disable();
        }
      }
      break;
    case 3:
    case 5:
      ((Control *)mode2_button)->hide();
      ((Button *)mode2_button)->disable();
      ((Control *)mode_button)->show_and_redraw();
      ((Button *)mode_button)->enable();
      break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xf:
      ((Control *)mode2_button)->show_and_redraw();
      ((Control *)mode_button)->hide();
      ((Button *)mode_button)->disable();
      if (side_selected[1] == 0) {
        ((Button *)mode2_button)->disable();
      }
      else {
        ((Button *)mode2_button)->enable();
      }
      break;
    case 0xe:
      ((Control *)mode2_button)->show_and_redraw();
      ((Control *)mode_button)->hide();
      ((Button *)mode_button)->disable();
      if ((side_selected[0] == 0) || (side_selected[1] == 0)) {
        ((Button *)mode2_button)->disable();
      }
      else {
        ((Button *)mode2_button)->enable();
      }
    }
  }
  return;
}

/* 0052ec47 auto */
int ParleyWindow::find_unit_by_id(int unit_id)

{
  short x2;
  short y2;
  bool near_land;
  iptr x;
  int y;
  int in_bounds;
  iptr unit;
  iptr dir;
  
  unit = (iptr)game.num_units;
  do {
    unit--;
    if (unit < 0) {
      return 0xffffffff;
    }
  } while ((units[unit].id == 0) || (units[unit].id != unit_id));
  x2 = units[unit].x;
  y2 = units[unit].y;
  near_land = false;
  for (dir = 0; dir < 8; dir++) {
    x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
    y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
    in_bounds = map_in_bounds(x,y);
    if ((in_bounds != 0) && (y = tile_is_ocean(x,y), y == 0)) {
      near_land = true;
      break;
    }
  }
  if ((!near_land) && (y = unit_stack_sum(unit,2), y != 1)) {
    unit = -2;
  }
  return unit;
}

/* 0052ed95 auto */
int ParleyWindow::find_city_by_id(int city_id)

{
  iptr city;
  
  city = (iptr)game.num_cities;
  do {
    city--;
    if (city < 0) {
      return 0xffffffff;
    }
  } while ((cities[city].id == 0) || (cities[city].id != city_id));
  return city;
}
