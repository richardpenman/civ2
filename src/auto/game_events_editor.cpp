/* The game, the events editor (cheat menu): triggers and actions edited: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00548df0 auto */
void __cdecl events_editor_focus_redraw(void)

{
  HWND hWnd;
  
  hWnd = ((Control *)&editor_active.wnd->trigger_listbox)->get_hwnd();
  SetFocus(hWnd);
  editor_active.wnd->events_editor_draw();
  return;
}

/* 0054a912 auto */
i32 __cdecl events_editor_pick_item(int default_choice,const char *title,i32 kind)

{
  iptr name_str;
  iptr item;
  i32 result;
  iptr terrain_item;
  iptr parent_wnd2;
  iptr advance;
  iptr civ;
  Dialog dlg;
  iptr terrain2;
  
  dlg.Dialog::construct(0x4000);
  ON_SCOPE_EXIT(dlg.Dialog::destruct());
  dlg.Dialog::reset(0x1001);
  dlg.set_title(title);
  dlg.set_width(0xdc);
  dlg.set_listbox(8,0xdc,0);
  switch(kind) {
  case 0:
    if (default_choice == 0) {
      default_choice = 1;
    }
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 1:
    for (civ = 0; civ < 0x3e; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)label_string(unit_types[civ].name);
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 2:
    for (advance = 0; advance < 100; advance++) {
      result = 0;
      item = advance;
      name_str = (iptr)label_string(advances[advance].name);
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 3:
    ((uint *)dlg.add_option((const char *)&str_anyunit_2,-2,0));
    for (civ = 0; civ < 0x3e; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)label_string(unit_types[civ].name);
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 4:
    if (default_choice == 0) {
      default_choice = -2;
    }
    dlg.add_option((const char *)&str_anybody_2,-2,0);
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 5:
    if (default_choice == 0) {
      default_choice = -3;
    }
    dlg.add_option((const char *)&str_triggerattacker_2,-3,0);
    dlg.add_option((const char *)&str_triggerdefender_2,-4,0);
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 6:
    if (default_choice == 0) {
      default_choice = -4;
    }
    dlg.add_option((const char *)&str_triggerreceiver_2,-4,0);
    for (civ = 1; civ < 8; civ++) {
      result = 0;
      item = civ;
      name_str = (iptr)(civ_name_plural(civ));
      dlg.add_option((const char *)name_str,item,result);
    }
    break;
  case 7:
    for (terrain2 = 0; terrain2 < 0xb; terrain2++) {
      result = 0;
      terrain_item = terrain2;
      item = (iptr)label_string(terrain_types[terrain2].name);
      dlg.add_option((const char *)item,terrain_item,result);
    }
  }
  dlg.set_default_choice(default_choice);
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  result = dlg.show_modal(0);
  dlg_set_parent_window(0);
  return result;
}

/* 0054adc6 auto */
int __cdecl events_editor_pick_action_civ(int civ,const char *title,int action,const char **out_name)

{
  i32 name2;
  
  if (action == 0x100) {
    civ = events_editor_pick_item(civ,title,6);
    if (civ != -1) {
      if (civ < 0) {
        *out_name = str_triggerreceiver_3;
      }
      else {
        name2 = (i32)(civ_name_plural(civ));
        *(i32 *)out_name = name2;
      }
    }
  }
  else {
    civ = events_editor_pick_item(civ,title,5);
    if (civ != -1) {
      if (civ < 0) {
        if (civ == -3) {
          *out_name = str_triggerattacker_3;
        }
        else {
          *out_name = str_triggerdefender_3;
        }
      }
      else {
        name2 = (i32)(civ_name_plural(civ));
        *(i32 *)out_name = name2;
      }
    }
  }
  return civ;
}

/* 0054ae93 auto */
void __cdecl events_editor_update_buttons(i32 unused)

{
  int action_sel;
  uint action_bit;
  ScenarioEvent *event;
  int action_count;
  int sel;
  
  editor_active.wnd->add_trigger_button.enable();
  sel = editor_active.wnd->trigger_listbox.get_cur_sel();
  if (sel < 0) {
    editor_active.wnd->edit_trigger_button.disable();
    editor_active.wnd->delete_trigger_button.disable();
    editor_active.wnd->add_action_button.disable();
    editor_active.wnd->edit_action_button.disable();
    editor_active.wnd->delete_action_button.disable();
  }
  else {
    editor_active.wnd->delete_trigger_button.enable();
    event = unit_types_block.events_list_head;
    while ((event != (ScenarioEvent *)0x0 && (sel != 0))) {
      event = (ScenarioEvent *)(byte *)event->next;
      sel--;
    }
    if ((*((byte *)event) & 0x20) == 0) {
      editor_active.wnd->edit_trigger_button.enable();
    }
    else {
      editor_active.wnd->edit_trigger_button.disable();
    }
    if (event->trigger == 0x20) {
      action_count = 0;
      while ((1 << ((byte)action_count & 0x1f) & (event->actions | 0xfffffe6f)) != 0) {
        action_count++;
      }
    }
    else {
      action_count = 0;
      while ((event->actions & 1 << ((byte)action_count & 0x1f)) != 0) {
        action_count++;
      }
    }
    if ((action_count < 0xc) && ((*((byte *)event) & 0x90) == 0)) {
      editor_active.wnd->add_action_button.enable();
    }
    else {
      editor_active.wnd->add_action_button.disable();
    }
    action_sel = editor_active.wnd->action_listbox.get_cur_sel();
    if (action_sel < 0) {
      editor_active.wnd->edit_action_button.disable();
      editor_active.wnd->delete_action_button.disable();
    }
    else {
      editor_active.wnd->delete_action_button.enable();
      action_bit = bits_nth_set(action_sel,(i32)event->actions);
      if ((action_bit & 0x3140) == 0) {
        editor_active.wnd->edit_action_button.enable();
      }
      else {
        editor_active.wnd->edit_action_button.disable();
      }
    }
  }
  return;
}

/* 0054b0ed auto */
void __cdecl events_editor_on_ok(void)

{
  iptr ok;
  NativeWindow *wnd;
  char *lpText;
  const char *lpCaption;
  UINT uType;
  iptr parent_wnd2;
  char msg[32];
  
  if (editor_ui.value_clamped == 0) {
    events_compact();
    ok = events_write_file();
    if (ok == 0) {
      _sprintf(msg,str_error_updating_events_s,((i8 *)&lang_file_ext));
      uType = 0x10;
      lpCaption = str_file_i_o_error_5;
      lpText = msg;
      wnd = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(wnd->hwnd,lpText,lpCaption,uType);
    }
    editor_lists.running = 0;
    editor_active.wnd->window.end_modal_loop();
    menu_update_states();
  }
  else {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    popup_text((const char *)&str_debug_11,(const char *)&str_notice_14);
    dlg_set_parent_window(0);
    editor_ui.value_clamped = 0;
    events_editor_focus_redraw();
  }
  return;
}

/* 0054b1d5 auto */
void __cdecl events_editor_help_trigger(i32 trigger_idx)

{
  iptr parent_wnd2;
  const char *help_key;
  
  switch(trigger_idx) {
  case 0:
    help_key = str_helpunitkilled;
    break;
  case 1:
    help_key = str_helpcitytaken;
    break;
  case 2:
    help_key = str_helpturn;
    break;
  case 3:
    help_key = str_helpturninterval;
    break;
  case 4:
    help_key = str_helpnegotiation;
    break;
  case 5:
    help_key = str_helpscenarioloaded;
    break;
  case 6:
    help_key = str_helprandomturn;
    break;
  case 7:
    help_key = str_helpnoschism;
    break;
  case 8:
    help_key = str_helpreceivedtechnology;
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_12,(const char *)help_key);
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* 0054b2ec auto */
void __cdecl events_editor_help_action(i32 action_idx)

{
  iptr parent_wnd2;
  const char *help_key;
  
  switch(action_idx) {
  case 0:
    help_key = str_helptext;
    break;
  case 1:
    help_key = str_helpmoveunit;
    break;
  case 2:
    help_key = str_helpcreateunit;
    break;
  case 3:
    help_key = str_helpchangemoney;
    break;
  case 4:
    help_key = str_helpplaywavefile;
    break;
  case 5:
    help_key = str_helpmakeaggression;
    break;
  case 6:
    help_key = str_helpjustonce;
    break;
  case 7:
    help_key = str_helpplaycdtrack;
    break;
  case 8:
    help_key = str_helpdontplaywonders;
    break;
  case 9:
    help_key = str_helpchangeterrain;
    break;
  case 10:
    help_key = str_helpdestroyacivilization;
    break;
  case 0xb:
    help_key = str_helpgivetechnology;
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  popup_text((const char *)&str_help_13,(const char *)help_key);
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* 0054b433 auto */
void __cdecl events_editor_on_help(void)

{
  int action_sel;
  iptr parent_wnd2;
  ScenarioEvent *event;
  uint bits2;
  int idx;
  
  action_sel = editor_active.wnd->action_listbox.get_cur_sel();
  if (action_sel < 0) {
    idx = editor_active.wnd->trigger_listbox.get_cur_sel();
    if (idx < 0) {
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      popup_text((const char *)&str_help_14,(const char *)&str_events_4);
      dlg_set_parent_window(0);
      events_editor_focus_redraw();
    }
    else {
      event = unit_types_block.events_list_head;
      while ((event != (ScenarioEvent *)0x0 && (idx != 0))) {
        event = event->next;
        idx--;
      }
      bits2 = (uint)event->trigger;
      idx = 0;
      /* BF-077 (Peter Johnson's ledger): an event with no trigger has no
         help (the original's search for its bit never ended) */
      for (; (bits2 != 0) && ((bits2 & 1) == 0); bits2 = bits2 >> 1) {
        idx++;
      }
      if ((event != (ScenarioEvent *)0x0) && (bits2 != 0)) {
        events_editor_help_trigger(idx);
      }
    }
  }
  else {
    idx = editor_active.wnd->trigger_listbox.get_cur_sel();
    event = unit_types_block.events_list_head;
    while ((event != (ScenarioEvent *)0x0 && (idx != 0))) {
      event = (ScenarioEvent *)(uint *)event->next;
      idx--;
    }
    bits2 = bits_nth_set(action_sel,(i32)event->actions);
    idx = 0;
    for (; (bits2 & 1) == 0; bits2 = bits2 >> 1) {
      idx++;
    }
    if (event != (ScenarioEvent *)0x0) {
      events_editor_help_action(idx);
    }
  }
  return;
}

/* 0054b5eb auto */
void __cdecl events_editor_on_cancel(void)

{
  ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
  events_copy((ScenarioEvents *)&unit_types_block.scenario_events,(ScenarioEvents *)events_editor_backup);
  editor_lists.running = 0;
  editor_active.wnd->window.end_modal_loop();
  return;
}

/* 0054b635 auto */
void __cdecl events_editor_fill_triggers(void)

{
  i32 tech_name;
  uint trigger;
  const char *talker_type;
  const char *listener_type;
  ScenarioEvent *event;
  int num;
  char line[256];
  
  num = 0;
  editor_active.wnd->trigger_listbox.clear();
  for (event = unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
      event = event->next) {
    num++;
    /* BF-077 (Peter Johnson's ledger): an event with no trigger (an empty @IF
       in the events file) is listed by its number (the original listed the
       line unset); BF-089: lines longer than 255 characters are cut */
    snprintf(line,sizeof line,"%d)",num);
    trigger = (uint)event->trigger;
    if (trigger < 9) {
      if (trigger == 8) {
        snprintf(line,sizeof line,str_d_turninterval_d,num,(uint)event->turn);
      }
      else if (trigger == 1) {
        if (((event->unit_name == 0) || (event->who_name == 0)) || (event->whom_name == 0)) {
          snprintf(line,sizeof line,str_d_unitkilled,num);
        }
        else {
          snprintf(line,sizeof line,str_d_unitkilled_s_attacker_s_defender_s,num,(uint)event->unit_name,
                   (uint)event->who_name,(uint)event->whom_name);
        }
      }
      else if (trigger == 2) {
        if (((event->city_name == 0) || (event->who_name == 0)) || (event->whom_name == 0)) {
          snprintf(line,sizeof line,str_d_citytaken,num);
        }
        else {
          snprintf(line,sizeof line,str_d_citytaken_s_attacker_s_defender_s,num,(uint)event->city_name,
                   (uint)event->who_name,(uint)event->whom_name);
        }
      }
      else if (trigger == 4) {
        snprintf(line,sizeof line,str_d_turn_d,num,(uint)event->turn);
      }
    }
    else {
      switch(trigger) {
      case 0x10:
        if ((event->who_name == 0) || (event->whom_name == 0)) {
          snprintf(line,sizeof line,str_d_negotiation,num);
        }
        else {
          if ((uint)event->listener_type == 1) {
            listener_type = str_human_5;
          }
          else if ((uint)event->listener_type == 2) {
            listener_type = str_computer_5;
          }
          else {
            listener_type = str_humanorcomputer_5;
          }
          if ((uint)event->talker_type == 1) {
            talker_type = str_human_6;
          }
          else if ((uint)event->talker_type == 2) {
            talker_type = str_computer_6;
          }
          else {
            talker_type = str_humanorcomputer_6;
          }
          snprintf(line,sizeof line,str_d_negotiation_talker_s_s_listener_s_s,num,(uint)event->who_name,
                   talker_type,(uint)event->whom_name,listener_type);
        }
        break;
      case 0x20:
        snprintf(line,sizeof line,str_d_scenarioloaded,num);
        break;
      case 0x40:
        if (event->denominator == 0) {
          snprintf(line,sizeof line,str_d_randomturn,num);
        }
        else {
          snprintf(line,sizeof line,str_d_randomturn_denominator_d,num,(uint)event->denominator);
        }
        break;
      case 0x80:
        if (event->whom_name == 0) {
          snprintf(line,sizeof line,str_d_noschism,num);
        }
        else {
          snprintf(line,sizeof line,str_d_noschism_s,num,(uint)event->whom_name);
        }
        break;
      case 0x100:
        if (event->whom_name == 0) {
          snprintf(line,sizeof line,str_d_receivedtechnology,num);
        }
        else {
          trigger = (uint)event->whom_name;
          tech_name = (i32)label_string(advances[(uint)event->technology].name);
          snprintf(line,sizeof line,str_d_receivedtechnology_s_whom_s,num,tech_name,trigger);
        }
      }
    }
    editor_active.wnd->trigger_listbox.add_string((const char *)line);
  }
  events_editor_update_buttons(0);
  return;
}

/* 0054bc1a auto */
void __cdecl events_editor_fill_actions(i32 unused)

{
  int next_idx;
  uint action;
  i32 name_str;
  i32 civ_name;
  bool more;
  char who_str[16];
  ScenarioEvent *event;
  uint action_bit;
  char line[256];
  
  action_bit = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  while ((event != 0 &&
         (next_idx = action_bit - 1, more = action_bit != 0, action_bit = next_idx, more))) {
    event = event->next;
  }
  editor_active.wnd->action_listbox.clear();
  action_bit = 1;
  if (event != 0) {
    do {
      line[0] = '\0';
      action = event->actions & action_bit;
      if (action < 9) {
        if (action == 8) {
          if (event->money_receiver_name == 0) {
            snprintf(line,sizeof line,str_changemoney_3);
          }
          else {
            snprintf(line,sizeof line,str_changemoney_receiver_s_amount_d,
                     (i32)event->money_receiver_name,event->money_amount);
          }
        }
        else if (action == 1) {
          if (event->text[0] == 0) {
            snprintf(line,sizeof line,((i8 *)&events_editor_text_caption));
          }
          else {
            snprintf(line,sizeof line,str_text_s,(i32)event->text[0]);
          }
        }
        else if (action == 2) {
          if ((event->move_unit_name == 0) || (event->move_owner_name == 0)) {
            snprintf(line,sizeof line,str_moveunit_3);
          }
          else {
            snprintf(line,sizeof line,str_moveunit_s_owner_s,(i32)event->move_unit_name,
                     (i32)event->move_owner_name);
          }
        }
        else if (action == 4) {
          if (((event->create_unit_name == 0) || (event->create_owner_name == 0)) ||
             (event->create_homecity == 0)) {
            snprintf(line,sizeof line,str_createunit_3);
          }
          else {
            snprintf(line,sizeof line,str_createunit_s_owner_s_veteran_s_homecity,
                     (i32)event->create_unit_name,(i32)event->create_owner_name,
                     ((i8 *)&events_editor_yes_no_strs) + ((event->create_veteran != 0) - 1 & 4),
                     (i32)event->create_homecity);
          }
        }
      }
      else if (action < 0x21) {
        if (action == 0x20) {
          if ((event->aggression_who_name == 0) || (event->aggression_whom_name == 0)) {
            snprintf(line,sizeof line,str_makeaggression_3);
          }
          else {
            snprintf(line,sizeof line,str_makeaggression_aggressor_s_victim_s,
                     (i32)event->aggression_who_name,(i32)event->aggression_whom_name);
          }
        }
        else if (action == 0x10) {
          if (event->wave_file == 0) {
            snprintf(line,sizeof line,str_playwavefile_3);
          }
          else {
            snprintf(line,sizeof line,str_playwavefile_s,(i32)event->wave_file);
          }
        }
      }
      else if (action < 0x81) {
        if (action == 0x80) {
          if (event->cd_track == 0) {
            snprintf(line,sizeof line,str_playcdtrack_3);
          }
          else {
            snprintf(line,sizeof line,str_playcdtrack_d,event->cd_track);
          }
        }
        else if (action == 0x40) {
          snprintf(line,sizeof line,str_justonce_3);
        }
      }
      else if (action < 0x201) {
        if (action == 0x200) {
          name_str = (i32)label_string(terrain_types[event->terrain_type].name);
          snprintf(line,sizeof line,str_changeterrain_s,name_str);
        }
        else if (action == 0x100) {
          snprintf(line,sizeof line,str_dontplaywonders_3);
        }
      }
      else if (action == 0x400) {
        if (event->destroy_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->destroy_civ == -3) {
              snprintf(line,sizeof line,str_destroyacivilization_triggerattacker);
            }
            else {
              snprintf(line,sizeof line,str_destroyacivilization_triggerdefender);
            }
          }
          else {
            snprintf(line,sizeof line,str_destroyacivilization_triggerreceiver);
          }
        }
        else {
          name_str = (i32)(civ_name_plural(event->destroy_civ));
          snprintf(line,sizeof line,str_destroyacivilization_s,name_str);
        }
      }
      else if (action == 0x800) {
        if (event->give_tech_civ == 0) {
          snprintf(line,sizeof line,str_givetechnology_3);
        }
        else if (event->give_tech_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->give_tech_civ == -3) {
              _sprintf(who_str,str_triggerattacker_4);
            }
            else {
              _sprintf(who_str,str_triggerdefender_4);
            }
          }
          else {
            _sprintf(who_str,str_triggerreceiver_4);
          }
          name_str = (i32)label_string(advances[event->give_tech].name);
          snprintf(line,sizeof line,str_givetechnology_receiver_s_technology_s_2,who_str,name_str);
        }
        else {
          name_str = (i32)label_string(advances[event->give_tech].name);
          civ_name = (i32)(civ_name_plural(event->give_tech_civ));
          snprintf(line,sizeof line,str_givetechnology_receiver_s_technology_s,civ_name,name_str);
        }
      }
      else if (action == 0x2000) {
        snprintf(line,sizeof line,str_hastriggered_2);
      }
      if (line[0] != '\0') {
        editor_active.wnd->action_listbox.add_string((const char *)line);
      }
      action_bit = action_bit << 1;
    } while (action_bit != 0x8000);
  }
  events_editor_update_buttons(0);
  return;
}

/* 0054c36e auto */
void __cdecl events_editor_on_delete_trigger(void)

{
  int sel;
  i32 parent_wnd2;
  ScenarioEvent *event;
  i32 idx;
  
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  sel = popup_game((const char *)&str_deleteitem);
  if (sel != 0) {
    sel = editor_active.wnd->trigger_listbox.get_cur_sel();
    idx = sel;
    for (event = unit_types_block.events_list_head; event != 0; event = event->next) {
      if (idx == 0) break;
      idx--;
    }
    if (event->prev != 0) {
      event->prev->next = event->next;
    }
    if (event->next != 0) {
      event->next->prev = event->prev;
    }
    if (sel == 0) {
      unit_types_block.events_list_head = event->next;
    }
    events_compact();
    events_editor_fill_triggers();
    events_editor_fill_actions(0);
  }
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* Part of events_editor_edit_trigger (0054c4a1): asks for the civ of a
   trigger (the list titled by the label title_label) into *civ, its name (or
   any_name, for any civ) into *civ_name. Returns 0 when cancelled (or out of
   memory). */
static int edit_trigger_pick_civ(int title_label,i32 *civ,char **civ_name,const char *any_name)
{
  size_t len;
  i32 kind;
  i32 title;
  uint value;
  uint choice;
  const char *name2;

  kind = 4;
  title = (i32)label_string(label_table.string_ids[title_label]);
  choice = events_editor_pick_item((uint)*civ,(const char *)title,kind);
  if (choice != 0xffffffff) {
    if ((int)choice < 0) {
      *civ = 0xfffffffe;
      name2 = any_name;
    }
    else {
      *civ = choice;
      name2 = civ_name_plural(*civ);
    }
    len = _strlen(name2);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    *civ_name = (char *)value;
    if (*civ_name != 0) {
      crt_strcpy((char *)((void *)*civ_name),name2);
      return 1;
    }
  }
  return 0;
}

/* Part of events_editor_edit_trigger (0054c4a1): asks whether the talker or
   the listener of a negotiation is a human, the computer or either, into
   *party_type (1, 2, 4). Returns 0 when cancelled. */
static int edit_trigger_pick_party_type(i32 *party_type,const char *title)
{
  iptr parent_wnd;
  uint choice;

  if ((*party_type == 0) || ((uint)*party_type == 1)) {
    choice = 0;
  }
  else if ((uint)*party_type == 2) {
    choice = 1;
  }
  else {
    choice = 2;
  }
  if (editor_active.wnd == 0) {
    parent_wnd = 0;
  }
  else {
    parent_wnd = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd);
  choice = popup_text_run(str_debug_11,title,choice,0,0,0,1);
  dlg_set_parent_window(0);
  if (-1 < (int)choice) {
    if (choice == 0) {
      *party_type = 1;
    }
    else if (choice == 1) {
      *party_type = 2;
    }
    else {
      *party_type = 4;
    }
    return 1;
  }
  return 0;
}

/* 0054c4a1 auto */
i32 __cdecl events_editor_edit_trigger(i32 unused)

{
  size_t len;
  int check;
  i32 title;
  uint value;
  bool more;
  i32 kind;
  iptr parent_wnd_h;
  iptr parent_wnd_b;
  iptr parent_wnd_a;
  iptr parent_wnd_d;
  iptr parent_wnd_c;
  iptr parent_wnd2;
  i32 changed;
  ScenarioEvent *event;
  char city_name[260];
  int skip;
  i32 number;
  int sel;
  uint choice;
  const char *name2;
  
  changed = 0;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  sel = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  skip = sel;
  while ((event != (ScenarioEvent *)0x0 &&
         (check = skip - 1, more = skip != 0, skip = check, more))) {
    event = event->next;
  }
  value = (uint)event->trigger;
  if (value < 9) {
    if (value == 8) {
      if (editor_active.wnd == 0) {
        parent_wnd_a = 0;
      }
      else {
        parent_wnd_a = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_a);
      choice = dlg_input_number(str_debug_11,str_turninterval_3,(uint)event->turn,&number);
      if (editor_active.wnd == 0) {
        parent_wnd_b = 0;
      }
      else {
        parent_wnd_b = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_b);
      if (choice == 0) {
        value = editor_clamp_value(number,0,0x7fff);
        event->turn = value;
        changed = 1;
      }
    }
    else if (value == 1) {
      kind = 1;
      title = (i32)label_string(label_table.string_ids[0x224]);
      choice = events_editor_pick_item((uint)event->unit_type,(const char *)title,kind);
      if (choice != 0xffffffff) {
        if ((int)choice < 0) {
          event->unit_type = 0xfffffffe;
          name2 = str_anyunit_3;
        }
        else {
          event->unit_type = choice;
          name2 = label_string(unit_types[(uint)event->unit_type].name);
        }
        len = _strlen(name2);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->unit_name = (char *)value;
        if (event->unit_name != 0) {
          crt_strcpy((char *)((void *)event->unit_name),name2);
          if (edit_trigger_pick_civ(0x225,&event->who_civ,&event->who_name,str_anybody_3) != 0) {
            if (edit_trigger_pick_civ(0x226,&event->whom_civ,&event->whom_name,str_anybody_4) != 0) {
              changed = 1;
            }
          }
        }
      }
    }
    else if (value == 2) {
      do {
        if (event->city_name == 0) {
          event->city_name = &events_city_default_str;
        }
        if (editor_active.wnd == 0) {
          parent_wnd_c = 0;
        }
        else {
          parent_wnd_c = (iptr)&editor_active.wnd->window;
        }
        dlg_set_parent_window(parent_wnd_c);
        choice = dlg_text_input(str_debug_11,str_cityname,0x17,event->city_name,city_name);
        dlg_set_parent_window(0);
      } while ((-1 < (int)choice) &&
              ((len = _strlen(city_name), len == 0 ||
               (check = editor_check_city_name((const char *)city_name), check == 0))));
      if ((-1 < (int)choice) && (len = _strlen(city_name), len != 0)) {
        len = _strlen(city_name);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->city_name = (char *)value;
        if (event->city_name != 0) {
          crt_strcpy((char *)((void *)event->city_name),city_name);
          if (edit_trigger_pick_civ(0x225,&event->who_civ,&event->who_name,str_anybody_5) != 0) {
            if (edit_trigger_pick_civ(0x226,&event->whom_civ,&event->whom_name,str_anybody_6) != 0) {
              changed = 1;
            }
          }
        }
      }
    }
    else if (value == 4) {
      if (editor_active.wnd == 0) {
        parent_wnd_d = 0;
      }
      else {
        parent_wnd_d = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_d);
      choice = dlg_input_number(str_debug_11,str_turncount,(uint)event->turn,&number);
      dlg_set_parent_window(0);
      if (choice == 0) {
        value = editor_clamp_value(number,0xffffffff,0x7fff);
        event->turn = value;
        changed = 1;
      }
    }
  }
  else {
    switch(value) {
    case 0x10:
      if (edit_trigger_pick_civ(0x227,&event->who_civ,&event->who_name,str_anybody_7) != 0) {
        if (edit_trigger_pick_party_type(&event->talker_type,(const char *)&str_talkertype_3) != 0) {
          if (edit_trigger_pick_civ(0x228,&event->whom_civ,&event->whom_name,str_anybody_8) != 0) {
            if (edit_trigger_pick_party_type(&event->listener_type,(const char *)&str_talkertype_4) != 0) {
              changed = 1;
            }
          }
        }
      }
      break;
    case 0x20:
      changed = 1;
      break;
    case 0x40:
      value = editor_clamp_value((uint)event->denominator,1,1000);
      event->denominator = value;
      if (editor_active.wnd == 0) {
        parent_wnd_h = 0;
      }
      else {
        parent_wnd_h = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd_h);
      choice = dlg_input_number(str_debug_11,str_randomdenom,(uint)event->denominator,&number);
      dlg_set_parent_window(0);
      if (choice == 0) {
        value = editor_clamp_value(number,1,1000);
        event->denominator = value;
        changed = 1;
      }
      break;
    case 0x80:
      if (edit_trigger_pick_civ(0x229,&event->whom_civ,&event->whom_name,str_anybody_9) != 0) {
        changed = 1;
      }
      break;
    case 0x100:
      if (edit_trigger_pick_civ(0x22a,&event->whom_civ,&event->whom_name,str_anybody_10) != 0) {
        kind = 2;
        title = (i32)label_string(label_table.string_ids[0x22b]);
        choice = events_editor_pick_item((uint)event->technology,(const char *)title,kind);
        if (choice != 0xffffffff) {
          event->technology = choice;
          changed = 1;
        }
      }
    }
  }
  events_compact();
  events_editor_fill_triggers();
  editor_active.wnd->trigger_listbox.set_cur_sel(sel);
  events_editor_update_buttons(0);
  events_editor_focus_redraw();
  dlg_set_parent_window(0);
  return changed;
}

/* 0054d4ca auto */
void __cdecl events_editor_on_edit_trigger(i32 mode)

{
  events_editor_edit_trigger(mode);
  return;
}

/* 0054d4e6 auto */
void __cdecl events_editor_on_add_trigger(void)

{
  int *new_event;
  LPCSTR lpCaption;
  LPCSTR lpText;
  NativeWindow *window;
  int result;
  UINT uType;
  iptr parent_wnd2;
  ScenarioEvent *event;
  int trigger_type;
  
  trigger_type = 0;
  while( true ) {
    if (editor_active.wnd == 0) {
      parent_wnd2 = 0;
    }
    else {
      parent_wnd2 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd2);
    dlg_input.help_pressed = 0;
    trigger_type = popup_game_ex((const char *)&str_triggers,trigger_type,1);
    dlg_set_parent_window(0);
    if (dlg_input.help_pressed == 0) break;
    events_editor_help_trigger(trigger_type);
  }
  /* BF-234 (Peter Johnson's ledger): one of the nine triggers (the list is
     from the game's text, which another can make longer); another adds none */
  if ((0 <= trigger_type) && (trigger_type < 9)) {
    if ((unit_types_block.events_pool_bytes_free < 0x1c4) ||
       (new_event = (int *)((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->alloc(),
       new_event == (int *)0x0)) {
      uType = 0;
      lpCaption = (LPCSTR)label_string(label_table.string_ids[0x234]);
      lpText = (LPCSTR)label_string(label_table.string_ids[0x23a]);
      window = ((WndBase *)&editor_active.wnd->window)->get_window();
      MessageBoxA(window->hwnd,lpText,lpCaption,uType);
    }
    else {
      *new_event = 1 << ((byte)trigger_type & 0x1f);
      events_editor_fill_triggers();
      events_editor_fill_actions(0);
      result = editor_active.wnd->trigger_listbox.get_count();
      editor_active.wnd->trigger_listbox.set_cur_sel(result - 1);
      result = events_editor_edit_trigger(0);
      if (result == 0) {
        for (event = unit_types_block.events_list_head; (int)event->next != 0;
            event = event->next) {
        }
        if (event->prev == 0) {
          unit_types_block.events_list_head = event->next;
        }
        else {
          event->prev->next = event->next;
        }
        events_compact();
        events_editor_fill_triggers();
        events_editor_fill_actions(0);
      }
      events_editor_update_buttons(0);
    }
  }
  events_editor_focus_redraw();
  return;
}

/* 0054d6da auto */
void __cdecl events_editor_on_delete_action(void)

{
  int result;
  i32 action_sel;
  uint action_bit;
  i32 parent_wnd2;
  ScenarioEvent *event;
  i32 sel_index;
  
  sel_index = editor_active.wnd->trigger_listbox.get_cur_sel();
  for (event = unit_types_block.events_list_head; event != 0; event = event->next) {
    if (sel_index == 0) break;
    sel_index--;
  }
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  result = popup_game((const char *)&str_deleteitem_2);
  if (result != 0) {
    action_sel = editor_active.wnd->action_listbox.get_cur_sel();
    result = bits_nth_set(action_sel,(i32)event->actions);
    if (result == 1) {
      event->text[0] = 0;
    }
    action_bit = bits_nth_set(action_sel,(i32)event->actions);
    event->actions = event->actions & ~action_bit;
    events_editor_fill_actions(0);
    events_compact();
  }
  dlg_set_parent_window(0);
  events_editor_focus_redraw();
  return;
}

/* Part of events_editor_edit_action (0054d7ef): the next number of the line
   being read (text_next_token), clamped to 0..0x7fff: a coordinate. */
static i32 edit_action_read_coord(void)
{
  i32 arg2;
  i32 value;
  char *token;
  long parsed;

  arg2 = 0x7fff;
  value = 0;
  token = text_next_token();
  parsed = _atol(token);
  value = editor_clamp_value(parsed,value,arg2);
  return value;
}

/* Part of events_editor_edit_action (0054d7ef): the action CHANGEMONEY: the
   receiver and the amount. */
static void edit_action_change_money(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  char **name_out;
  iptr parent_wnd22;
  int number_value;
  int result;
  char *name_str;

  name_out = &name_str;
  value = event->trigger;
  arg2 = (i32)label_string(label_table.string_ids[0x22a]);
  result = events_editor_pick_action_civ(event->money_receiver_civ,(const char *)arg2,value,(const char **)name_out);
  if (result != -1) {
    event->money_receiver_civ = result;
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->money_receiver_name = (char *)value;
    if (event->money_receiver_name != 0) {
      crt_strcpy((char *)((void *)event->money_receiver_name),name_str);
      if (editor_active.wnd == 0) {
        parent_wnd22 = 0;
      }
      else {
        parent_wnd22 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd22);
      result = editor_input_long((const char *)&str_debug_11,(const char *)&str_amount_2,event->money_amount,&number_value);
      dlg_set_parent_window(0);
      if (result == 0) {
        value = editor_clamp_value(number_value,0xffff8ad0,30000);
        event->money_amount = value;
        *changed = 1;
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action MOVEUNIT: the
   unit, its owner, the map rectangle, the destination and the number of
   units. */
static void edit_action_move_unit(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  const char **name_out;
  i32 default_value;
  iptr parent_wnd6;
  iptr parent_wnd5;
  iptr parent_wnd4;
  iptr parent_wnd3;
  char input_buf[260];
  int idx;
  int number_value;
  char default_text[64];  /* BF-036 (Peter Johnson's ledger): eight numbers of
                             the events file cut at 63 characters */
  int result;
  const char *name_str;

  arg2 = 3;
  value = (i32)label_string(label_table.string_ids[0x22d]);
  result = events_editor_pick_item(event->move_unit_type,(const char *)value,arg2);
  if (result != -1) {
    if (result < 0) {
      event->move_unit_type = 0xfffffffe;
      name_str = str_anyunit_4;
    }
    else {
      event->move_unit_type = result;
      name_str = label_string(unit_types[event->move_unit_type].name);
    }
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->move_unit_name = (char *)value;
    if (event->move_unit_name != 0) {
      crt_strcpy((char *)((void *)event->move_unit_name),name_str);
      name_out = &name_str;
      value = event->trigger;
      arg2 = (i32)label_string(label_table.string_ids[0x22e]);
      result = events_editor_pick_action_civ(event->move_owner_civ,(const char *)arg2,value,name_out);
      if (result != -1) {
        event->move_owner_civ = result;
        len = _strlen(name_str);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->move_owner_name = (char *)value;
        if (event->move_owner_name != 0) {
          crt_strcpy((char *)((void *)event->move_owner_name),name_str);
          snprintf(default_text,sizeof default_text,str_d_d_d_d_d_d_d_d,event->move_rect[0],event->move_rect[1],
                   event->move_rect[2],event->move_rect[3],event->move_rect[4],event->move_rect[5],
                   event->move_rect[6],event->move_rect[7]);
          if (editor_active.wnd == 0) {
            parent_wnd3 = 0;
          }
          else {
            parent_wnd3 = (iptr)&editor_active.wnd->window;
          }
          dlg_set_parent_window(parent_wnd3);
          do {
            result = dlg_text_input(str_debug_11,str_maprect_5,0x1f,default_text,input_buf)
            ;
            if (result == -1) break;
            len = _strlen(input_buf);
          } while (len == 0);
          dlg_set_parent_window(0);
          if (result != -1) {
            len = _strlen(input_buf);
            if (len != 0) {
              crt_strcpy(((i8 *)&textbuf.buf),input_buf);
              for (idx = 0; idx < 4; idx++) {
                event->move_rect[idx * 2] = edit_action_read_coord();
                event->move_rect[idx * 2 + 1] = edit_action_read_coord();
              }
            }
            snprintf(default_text,sizeof default_text,str_d_d_5,event->move_to_x,event->move_to_y);
            if (editor_active.wnd == 0) {
              parent_wnd4 = 0;
            }
            else {
              parent_wnd4 = (iptr)&editor_active.wnd->window;
            }
            dlg_set_parent_window(parent_wnd4);
            result = dlg_text_input(str_debug_11,str_location,8,default_text,input_buf);
            if (editor_active.wnd == 0) {
              parent_wnd5 = 0;
            }
            else {
              parent_wnd5 = (iptr)&editor_active.wnd->window;
            }
            dlg_set_parent_window(parent_wnd5);
            if (result != -1) {
              len = _strlen(input_buf);
              if (len != 0) {
                crt_strcpy(((i8 *)&textbuf.buf),input_buf);
                event->move_to_x = edit_action_read_coord();
                event->move_to_y = edit_action_read_coord();
              }
              if (editor_active.wnd == 0) {
                parent_wnd6 = 0;
              }
              else {
                parent_wnd6 = (iptr)&editor_active.wnd->window;
              }
              dlg_set_parent_window(parent_wnd6);
              if (event->move_count == -2) {
                default_value = 0xffffffff;
              }
              else {
                default_value = event->move_count;
              }
              result = dlg_input_number(str_debug_11,str_numunits,default_value,&number_value);
              dlg_set_parent_window(0);
              if (result == 0) {
                if (number_value == -1) {
                  event->move_count = 0xfffffffe;
                }
                else {
                  value = editor_clamp_value(number_value,0,0x7fff);
                  event->move_count = value;
                }
                *changed = 1;
              }
            }
          }
        }
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action CREATEUNIT: the
   unit, its owner, veteran or not, its home city and up to 10 locations. */
static void edit_action_create_unit(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  int prev_count;
  char **name_out;
  iptr parent_wnd9;
  iptr parent_wnd8;
  iptr parent_wnd7;
  char input_buf[260];
  int idx;
  int choice;
  char default_text[64];  /* BF-036 (Peter Johnson's ledger): eight numbers of
                             the events file cut at 63 characters */
  int result;
  char *name_str;

  arg2 = 1;
  value = (i32)label_string(label_table.string_ids[0x22f]);
  result = events_editor_pick_item(event->create_unit_type,(const char *)value,arg2);
  if (result != -1) {
    event->create_unit_type = result;
    name_str = label_string(unit_types[event->create_unit_type].name);
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->create_unit_name = (char *)value;
    if (event->create_unit_name != 0) {
      crt_strcpy((char *)((void *)event->create_unit_name),name_str);
      name_out = &name_str;
      value = event->trigger;
      arg2 = (i32)label_string(label_table.string_ids[0x22e]);
      result = events_editor_pick_action_civ(event->create_owner_civ,(const char *)arg2,value,(const char **)name_out);
      if (result != -1) {
        event->create_owner_civ = result;
        len = _strlen(name_str);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->create_owner_name = (char *)value;
        if (event->create_owner_name != 0) {
          crt_strcpy((char *)((void *)event->create_owner_name),name_str);
          if (editor_active.wnd == 0) {
            parent_wnd7 = 0;
          }
          else {
            parent_wnd7 = (iptr)&editor_active.wnd->window;
          }
          dlg_set_parent_window(parent_wnd7);
          choice = popup_debug_ex((const char *)&str_veteran_2,event->create_veteran,0);
          dlg_set_parent_window(0);
          if (-1 < choice) {
            event->create_veteran = choice;
            do {
              if (event->create_homecity == 0) {
                event->create_homecity = (char *)(((i32)(i8 *)&events_homecity_default_str));
              }
              if (editor_active.wnd == 0) {
                parent_wnd8 = 0;
              }
              else {
                parent_wnd8 = (iptr)&editor_active.wnd->window;
              }
              dlg_set_parent_window(parent_wnd8);
              result = dlg_text_input(str_debug_11,str_homecityname,0x17,event->create_homecity,
                                            input_buf);
              dlg_set_parent_window(0);
            } while ((-1 < result) &&
                    ((len = _strlen(input_buf), len == 0 ||
                     (prev_count = editor_check_city_name((const char *)input_buf), prev_count == 0))));
            if ((-1 < result) && (len = _strlen(input_buf), len != 0)) {
              len = _strlen(input_buf);
              value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
              event->create_homecity = (char *)value;
              if (event->create_homecity != 0) {
                crt_strcpy((char *)((void *)event->create_homecity),input_buf);
                for (idx = 0; idx < 10; idx++) {
                  event->create_location_count = idx;
                  snprintf(default_text,sizeof default_text,str_d_d_6,event->create_locations[idx * 2],
                           event->create_locations[idx * 2 + 1]);
                  if (editor_active.wnd == 0) {
                    parent_wnd9 = 0;
                  }
                  else {
                    parent_wnd9 = (iptr)&editor_active.wnd->window;
                  }
                  dlg_set_parent_window(parent_wnd9);
                  text_set_number_arg(0,idx + 1);
                  result = dlg_text_input(str_debug_11,str_locationnum,8,default_text,
                                                input_buf);
                  dlg_set_parent_window(0);
                  if (result == -1) {
                    if (idx == 0) break;
                    do {
                      event->create_locations[idx * 2] = 0;
                      event->create_locations[idx * 2 + 1] = 0;
                      idx++;
                    } while (idx < 10);
                  }
                  else {
                    len = _strlen(input_buf);
                    if (len != 0) {
                      crt_strcpy(((i8 *)&textbuf.buf),input_buf);
                      event->create_locations[idx * 2] = edit_action_read_coord();
                      event->create_locations[idx * 2 + 1] = edit_action_read_coord();
                    }
                  }
                }
                if (event->create_location_count != 0) {
                  *changed = 1;
                }
              }
            }
          }
        }
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action MAKEAGGRESSION:
   who and whom. */
static void edit_action_make_aggression(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  int result;
  char *name_str;

  arg2 = 0;
  value = (i32)label_string(label_table.string_ids[0x230]);
  result = events_editor_pick_item(event->aggression_who_civ,(const char *)value,arg2);
  if (result != -1) {
    event->aggression_who_civ = result;
    name_str = civ_name_plural(event->aggression_who_civ);
    len = _strlen(name_str);
    value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
    event->aggression_who_name = (char *)value;
    if (event->aggression_who_name != 0) {
      crt_strcpy((char *)((void *)event->aggression_who_name),name_str);
      arg2 = 0;
      value = (i32)label_string(label_table.string_ids[0x231]);
      result = events_editor_pick_item(event->aggression_whom_civ,(const char *)value,arg2);
      if (result != -1) {
        event->aggression_whom_civ = result;
        name_str = civ_name_plural(event->aggression_whom_civ);
        len = _strlen(name_str);
        value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
        event->aggression_whom_name = (char *)value;
        if (event->aggression_whom_name != 0) {
          crt_strcpy((char *)((void *)event->aggression_whom_name),name_str);
          *changed = 1;
        }
      }
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action PLAYWAVEFILE: the
   file (.WAV added when it has no extension). */
static void edit_action_play_wave(ScenarioEvent *event,uint *changed)
{
  i32 value;
  size_t len;
  char *token;
  iptr parent_wnd10;
  char input_buf[260];
  int result;

  if (event->wave_file == 0) {
    event->wave_file = (char *)(((i32)(i8 *)&events_wavfile_default_str));
  }
  if (editor_active.wnd == 0) {
    parent_wnd10 = 0;
  }
  else {
    parent_wnd10 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd10);
  do {
    result = dlg_text_input(str_debug_11,str_wavfile,10,event->wave_file,input_buf);
    if (result == -1) break;
    len = _strlen(input_buf);
  } while (len == 0);
  dlg_set_parent_window(0);
  if (-1 < result) {
    len = _strlen(input_buf);
    if (len != 0) {
      token = _strchr(input_buf,0x2e);
      if (token == (char *)0x0) {
        crt_strcat(input_buf,((i8 *)&events_wav_ext_str));
      }
      __strupr(input_buf);
      len = _strlen(input_buf);
      value = memblock_alloc_checked((MemBlock *)&unit_types_block.str_empty_188,len + 1);
      event->wave_file = (char *)value;
      if (event->wave_file != 0) {
        crt_strcpy((char *)((void *)event->wave_file),input_buf);
        *changed = 1;
      }
    }
    else {
      *changed = 1;
    }
  }
}

/* Part of events_editor_edit_action (0054d7ef): the action CHANGETERRAIN:
   the terrain type and the map rectangle. */
static void edit_action_change_terrain(ScenarioEvent *event,uint *changed)
{
  i32 arg2;
  i32 value;
  size_t len;
  iptr parent_wnd12;
  char input_buf[260];
  int idx;
  char default_text[64];  /* BF-036 (Peter Johnson's ledger): eight numbers of
                             the events file cut at 63 characters */
  int result;

  arg2 = 7;
  value = (i32)label_string(label_table.string_ids[0x25c]);
  result = events_editor_pick_item(event->terrain_type,(const char *)value,arg2);
  if (result == -1) {
    result = -1;
  }
  else {
    event->terrain_type = result;
    snprintf(default_text,sizeof default_text,str_d_d_d_d_d_d_d_d_2,event->terrain_rect[0],event->terrain_rect[1],
             event->terrain_rect[2],event->terrain_rect[3],event->terrain_rect[4],event->terrain_rect[5],event->terrain_rect[6],
             event->terrain_rect[7]);
    if (editor_active.wnd == 0) {
      parent_wnd12 = 0;
    }
    else {
      parent_wnd12 = (iptr)&editor_active.wnd->window;
    }
    dlg_set_parent_window(parent_wnd12);
    do {
      result = dlg_text_input(str_debug_11,str_maprect_6,0x1f,default_text,input_buf);
      if (result == -1) break;
      len = _strlen(input_buf);
    } while (len == 0);
    dlg_set_parent_window(0);
    if (result != -1) {
      len = _strlen(input_buf);
      if (len != 0) {
        crt_strcpy(((i8 *)&textbuf.buf),input_buf);
        for (idx = 0; idx < 4; idx++) {
          event->terrain_rect[idx * 2] = edit_action_read_coord();
          event->terrain_rect[idx * 2 + 1] = edit_action_read_coord();
        }
      }
      *changed = 1;
    }
  }
}

/* 0054d7ef auto */
uint __cdecl events_editor_edit_action(i32 mode)

{
  i32 arg2;
  uint action_bit;
  i32 value;
  int prev_count;
  bool nonzero;
  char **name_out;
  iptr parent_wnd11;
  iptr parent_wnd2;
  uint changed;
  ScenarioEvent *event;
  int idx;
  EditControl edit_ctl;
  int number_value;
  int choice;
  int result;
  char *name_str;
  i8 *seh_scope;
  
  edit_ctor(&edit_ctl);
  ON_SCOPE_EXIT(edit_dtor(&edit_ctl));
  changed = 0;
  if (editor_active.wnd == 0) {
    parent_wnd2 = 0;
  }
  else {
    parent_wnd2 = (iptr)&editor_active.wnd->window;
  }
  dlg_set_parent_window(parent_wnd2);
  idx = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  while ((event != (ScenarioEvent *)0x0 &&
         (prev_count = idx - 1, nonzero = idx != 0, idx = prev_count, nonzero))) {
    event = event->next;
  }
  value = (i32)event->actions;
  arg2 = editor_active.wnd->action_listbox.get_cur_sel();
  action_bit = bits_nth_set(arg2,value);
  if (action_bit < 9) {
    if (action_bit == 8) {
      edit_action_change_money(event,&changed);
    }
    else if (action_bit == 1) {
      value = (i32)label_string(label_table.string_ids[0x22c]);
      prev_count = editor_text_edit_dialog((((i32 *)event) + 0xe),(const char *)value);
      changed = (uint)(prev_count != 0);
    }
    else if (action_bit == 2) {
      edit_action_move_unit(event,&changed);
    }
    else if (action_bit == 4) {
      edit_action_create_unit(event,&changed);
    }
  }
  else if (action_bit < 0x21) {
    if (action_bit == 0x20) {
      edit_action_make_aggression(event,&changed);
    }
    else if (action_bit == 0x10) {
      edit_action_play_wave(event,&changed);
    }
  }
  else if (action_bit < 0x81) {
    if (action_bit == 0x80) {
      value = editor_clamp_value(event->cd_track,2,0x18);
      event->cd_track = value;
      text_set_number_arg(0,2);
      text_set_number_arg(1,0x18);
      if (editor_active.wnd == 0) {
        parent_wnd11 = 0;
      }
      else {
        parent_wnd11 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd11);
      result = dlg_input_number(str_debug_11,str_cdtrack,event->cd_track,&number_value);
      dlg_set_parent_window(0);
      if (result == 0) {
        value = editor_clamp_value(number_value,2,0x18);
        event->cd_track = value;
        changed = 1;
      }
    }
    else if (action_bit == 0x40) {
      changed = 1;
    }
  }
  else if (action_bit < 0x201) {
    if (action_bit == 0x200) {
      edit_action_change_terrain(event,&changed);
    }
    else if (action_bit == 0x100) {
      changed = 1;
    }
  }
  else if (action_bit == 0x400) {
    name_out = &name_str;
    value = event->trigger;
    arg2 = (i32)label_string(label_table.string_ids[0x232]);
    result = events_editor_pick_action_civ(event->destroy_civ,(const char *)arg2,value,(const char **)name_out);
    if (result != -1) {
      event->destroy_civ = result;
      changed = 1;
    }
  }
  else if (action_bit == 0x800) {
    name_out = &name_str;
    value = event->trigger;
    arg2 = (i32)label_string(label_table.string_ids[0x22a]);
    result = events_editor_pick_action_civ(event->give_tech_civ,(const char *)arg2,value,(const char **)name_out);
    if (result == -1) {
      result = -1;
    }
    else {
      event->give_tech_civ = result;
      arg2 = 2;
      value = (i32)label_string(label_table.string_ids[0x22b]);
      result = events_editor_pick_item(event->give_tech,(const char *)value,arg2);
      if (result != -1) {
        event->give_tech = result;
        changed = 1;
      }
    }
  }
  else if (action_bit == 0x2000) {
    changed = 1;
  }
  events_compact();
  events_editor_fill_actions(0);
  choice = bits_index_of(action_bit,(i32)event->actions);
  editor_active.wnd->action_listbox.set_cur_sel(choice);
  events_editor_update_buttons(0);
  events_editor_focus_redraw();
  dlg_set_parent_window(0);
  return changed;
}

/* 0054eeb4 auto */
void __cdecl events_editor_on_edit_action(i32 mode)

{
  events_editor_edit_action(mode);
  return;
}

/* 0054eed0 auto */
void __cdecl events_editor_on_add_action(void)

{
  LPCSTR lpCaption;
  LPCSTR lpText;
  i32 list_pos;
  int result;
  UINT uType;
  iptr parent_wnd2;
  ScenarioEvent *event;
  int action_index;
  int sel_index;
  
  action_index = editor_active.wnd->trigger_listbox.get_cur_sel();
  event = unit_types_block.events_list_head;
  while ((event != (ScenarioEvent *)0x0 && (action_index != 0))) {
    event = event->next;
    action_index--;
  }
  if (event->trigger == 0x20) {
    action_index = 0;
    while ((1 << ((byte)action_index & 0x1f) & ((int)event->actions | 0xfffffe6fU)) != 0) {
      action_index++;
    }
  }
  else {
    action_index = 0;
    while (((int)event->actions & 1 << ((byte)action_index & 0x1f)) != 0) {
      action_index++;
    }
  }
  if (action_index < 0xd) {
    while( true ) {
      if (editor_active.wnd == 0) {
        parent_wnd2 = 0;
      }
      else {
        parent_wnd2 = (iptr)&editor_active.wnd->window;
      }
      dlg_set_parent_window(parent_wnd2);
      dlg_input.help_pressed = 0;
      if (event->trigger == 0x20) {
        action_index = editor_options_dialog((const char *)&str_actions,(const char *)action_index,(int)event->actions | 0xfffffe6f,1);
      }
      else {
        action_index = editor_options_dialog((const char *)&str_actions_2,(const char *)action_index,(int)event->actions,1);
      }
      dlg_set_parent_window(0);
      if (dlg_input.help_pressed == 0) break;
      events_editor_help_action(action_index);
    }
    if (action_index != -1) {
      event->actions = (int)event->actions | 1 << ((byte)action_index & 0x1f);
      list_pos = bits_index_of(1 << ((byte)action_index & 0x1f),(int)event->actions);
      events_editor_fill_actions(0);
      editor_active.wnd->action_listbox.set_cur_sel(list_pos);
      result = events_editor_edit_action(0);
      if (result == 0) {
        sel_index = editor_active.wnd->trigger_listbox.get_cur_sel();
        event = unit_types_block.events_list_head;
        while ((event != (ScenarioEvent *)0x0 && (sel_index != 0))) {
          event = (ScenarioEvent *)(int *)event->next;
          sel_index--;
        }
        event->actions =
             event->actions & ~(1 << ((byte)action_index & 0x1f));
        events_editor_fill_actions(0);
      }
    }
  }
  else {
    uType = 0;
    lpCaption = (LPCSTR)label_string(label_table.string_ids[0x234]);
    lpText = (LPCSTR)label_string(label_table.string_ids[0x233]);
    MessageBoxA((HWND)0x0,lpText,lpCaption,uType);
  }
  events_editor_focus_redraw();
  return;
}

/* 0054f16b auto */
void EditorWindow::events_editor_draw()

{
  u32 top;
  uptr line_height;
  iptr label;
  int bar_y;
  uint bar_height;
  int x;
  
  this->FrameWindow::draw_caption();
  gfx_fill_rect(this,(RECT *)&client_rect,0x1a);
  text_set_surface(this);
  text_set_font(&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  x = client_x + (int)client_width / 2;
  top = client_y;
  line_height = font_height_uptr;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x235]);
  text_append_str((const char *)label);
  text_draw_centered(this,textbuf.buf,x,(top - line_height) + 0x19,0);
  top = client_y;
  line_height = font_height_uptr;
  text_clear();
  label = (iptr)label_string(label_table.string_ids[0x236]);
  text_append_str((const char *)label);
  text_draw_centered(this,textbuf.buf,x,(top - line_height) + 0xc3,0);
  top = client_x;
  x = top + 0x211;
  bar_y = client_y + 0x1a;
  text_clear();
  _sprintf(((i8 *)&textbuf.buf),((i8 *)&events_pool_free_fmt),(uint)unit_types_block.events_pool_bytes_free);
  text_draw_centered(this,textbuf.buf,top + 0x216,(bar_y - font_height_uptr) - 2,0);
  bar_height = ((uint)unit_types_block.events_pool_bytes_free * 0x13b) / (uint)unit_types_block.events_pool_size;
  gfx_fill_rect_xywh(this,x,bar_y,10,bar_height,0x2a);
  gfx_fill_rect_xywh(this,x,bar_height + bar_y,10,0x13b - bar_height,0x6a);
  gfx_draw_frame_rect(this,x,bar_y,10,0x13b,10);
  gfx_draw_frame_rect(this,x,bar_y,10,bar_height,10);
  wnd_blit_surface();
  return;
}

/* 0054f3b9 auto */
void EditorWindow::events_editor_run()

{
  ScenarioEvents *this_00;
  uptr font_height;
  i32 title;
  int x;
  int y;
  iptr label;
  i32 style;
  i32 arg_x;
  i32 arg_y;
  u32 width;
  u32 height;
  i32 arg_a;
  i32 arg_b;
  i32 arg_c;
  u8 *parent6;
  u8 *parent5;
  u8 *parent4;
  u8 *list_parent2;
  u8 *list_parent;
  u8 *parent9;
  u8 *parent8;
  u8 *parent7;
  u8 *parent3;
  u8 *parent22;
  u8 *parent2;
  ScenarioEvents *backup;
  i8 rect[16];
  GamePalette palette;
  i8 *seh_scope;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  editor_lists.running = 1;
  editor_active.wnd = this;
  this_00 = (ScenarioEvents *)operator_new(0x310);
  if (this_00 == (ScenarioEvents *)0x0) {
    backup = (ScenarioEvents *)0x0;
  }
  else {
    backup = this_00->construct(50000);
  }
  (*(ScenarioEvents * *)&events_editor_backup) = backup;
  backup->reset(50000);
  events_copy((ScenarioEvents *)(iptr)(*(ScenarioEvents * *)&events_editor_backup),(ScenarioEvents *)&unit_types_block.scenario_events);
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
  font_height = ((GameFont *)((i8 *)&editor_font_block.editor_font))->font_cached_height();
  font_height_uptr = font_height;
  arg_c = 0;
  arg_b = 0;
  arg_a = 0;
  height = wnd_height_u;
  width = wnd_width_u;
  arg_y = 0;
  arg_x = 0;
  style = 0xd;
  title = (i32)label_string(label_table.string_ids[0x1cf]);
  this->FrameWindow::create((const char *)title,style,arg_x,arg_y,width,height,arg_a,arg_b,arg_c);
  x = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height_u = x + 8;
  button_width_u = 0x97;
  y = client_y + 0x90;
  x = client_x + 0x32;
  rect_set_xywh((RECT *)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent2 = (u8 *)0x0;
  }
  else {
    parent2 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x237]);
  add_trigger_button.create((Wnd *)parent2,0x65,(RECT *)rect,(const char *)label);
  add_trigger_button.set_callback((CtrlNotifyFn)events_editor_on_add_trigger);
  x = x + button_width_u + 3;
  rect_set_xywh((RECT *)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent22 = (u8 *)0x0;
  }
  else {
    parent22 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x238]);
  edit_trigger_button.create((Wnd *)parent22,0x65,(RECT *)rect,(const char *)label);
  edit_trigger_button.set_callback(events_editor_on_edit_trigger);
  rect_set_xywh((RECT *)rect,x + button_width_u + 3,y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent3 = (u8 *)0x0;
  }
  else {
    parent3 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x239]);
  ((Button *)((u8 *)&delete_trigger_button_u))->create((Wnd *)parent3,0x65,(RECT *)rect,(const char *)label);
  ((Button *)((u8 *)&delete_trigger_button_u))->set_callback((CtrlNotifyFn)events_editor_on_delete_trigger);
  y = client_y + 0x13a;
  x = client_x + 0x32;
  rect_set_xywh((RECT *)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent7 = (u8 *)0x0;
  }
  else {
    parent7 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x237]);
  add_action_button.create((Wnd *)parent7,0x65,(RECT *)rect,(const char *)label);
  add_action_button.set_callback((CtrlNotifyFn)events_editor_on_add_action);
  x = x + button_width_u + 3;
  rect_set_xywh((RECT *)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent8 = (u8 *)0x0;
  }
  else {
    parent8 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x238]);
  edit_action_button.create((Wnd *)parent8,0x65,(RECT *)rect,(const char *)label);
  edit_action_button.set_callback(events_editor_on_edit_action);
  rect_set_xywh((RECT *)rect,x + button_width_u + 3,y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent9 = (u8 *)0x0;
  }
  else {
    parent9 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x239]);
  delete_action_button.create((Wnd *)parent9,0x65,(RECT *)rect,(const char *)label);
  delete_action_button.set_callback((CtrlNotifyFn)events_editor_on_delete_action);
  rect_set_xywh((RECT *)rect,client_x + 0x32,client_y + 0x1b,0x1cb,0x6e);
  x = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    list_parent = (u8 *)0x0;
  }
  else {
    list_parent = (u8 *)&window;
  }
  trigger_listbox.create((Wnd *)list_parent,x,(RECT *)rect);
  trigger_listbox.set_font(&editor_font_block.editor_font);
  trigger_listbox.set_select_callback(events_editor_fill_actions);
  trigger_listbox.set_dblclick_callback(events_editor_on_edit_trigger);
  events_editor_fill_triggers();
  trigger_listbox.set_cur_sel(0);
  rect_set_xywh((RECT *)rect,client_x + 0x32,client_y + 0xc5,0x1cb,0x6e);
  x = editor_lists.next_ctrl_id;
  editor_lists.next_ctrl_id++;
  if (this == (EditorWindow *)0x0) {
    list_parent2 = (u8 *)0x0;
  }
  else {
    list_parent2 = (u8 *)&window;
  }
  action_listbox.create((Wnd *)list_parent2,x,(RECT *)rect);
  action_listbox.set_font(&editor_font_block.editor_font);
  action_listbox.set_select_callback(events_editor_update_buttons);
  action_listbox.set_dblclick_callback(events_editor_on_edit_action);
  events_editor_fill_actions(0);
  events_editor_update_buttons(0);
  button_width_u = (int)(client_width - 8) / 3;
  y = (client_y + client_height) - (button_height_u + 2);
  x = client_x + 2;
  rect_set_xywh((RECT *)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent4 = (u8 *)0x0;
  }
  else {
    parent4 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((Wnd *)parent4,0x65,(RECT *)rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)events_editor_on_ok);
  x = x + button_width_u + 2;
  rect_set_xywh((RECT *)rect,x,y,button_width_u,button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent5 = (u8 *)0x0;
  }
  else {
    parent5 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x23b]);
  help_button.create((Wnd *)parent5,0x66,(RECT *)rect,(const char *)label);
  help_button.set_callback((CtrlNotifyFn)events_editor_on_help);
  rect_set_xywh((RECT *)rect,x + button_width_u + 2,y,button_width_u,
                     button_height_u);
  if (this == (EditorWindow *)0x0) {
    parent6 = (u8 *)0x0;
  }
  else {
    parent6 = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((Wnd *)parent6,0x66,(RECT *)rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)events_editor_on_cancel);
  cancel_button.set_cancel_button();
  window.set_keyboard_nav(0);
  editor_ui.value_clamped = 0;
  set_redraw_callback(events_editor_focus_redraw);
  repaint();
  this->SurfaceWindow::show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (editor_lists.running != 0) {
    win_pump_one_message();
  }
  if ((*(ScenarioEvents * *)&events_editor_backup) != (ScenarioEvents *)0x0) {
    (*(ScenarioEvents * *)&events_editor_backup)->events_delete(1);
  }
  (*(ScenarioEvents * *)&events_editor_backup) = (ScenarioEvents *)0x0;
  return;
}

/* 0054ffc8 auto */
void __cdecl events_editor_open(void)

{
  i8 *seh_scope;
  
  EditorWindow editor_wnd;
  editor_wnd.events_editor_run();
  return;
}
