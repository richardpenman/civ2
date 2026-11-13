/* The game, scenario events: the events file read and written, triggers checked, actions run: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004675b0 auto */
void __cdecl scenario_events_global_init(void)

{
  scenario_events_static_init();
  scenario_events_reg_atexit();
  return;
}

/* 004675ca auto */
void __cdecl scenario_events_static_init(void)

{
  new (&unit_types_block.scenario_events, GameNewAt()) ScenarioEvents(50000);
  return;
}

/* 004675e9 auto */
void __cdecl scenario_events_reg_atexit(void)

{
  _atexit(scenario_events_static_dtor);
  return;
}

/* 00467606 auto */
void __cdecl scenario_events_static_dtor(void)

{
  ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->~ScenarioEvents();
  return;
}

/* 0048308f auto */
void __cdecl events_relink_pool(void)

{
  int event_count;
  size_t str_len;
  int len_184;
  int len_174;
  int len_148;
  int len_140;
  int len_13c;
  int len_dc;
  int len_d4;
  int len_cc;
  int len_c4;
  int len_90;
  int len_88;
  int len_slot;
  int len_20;
  int len_14;
  int len_10;
  int len_08;
  char *string_ptr;
  char *event;
  int i;
  
  event_count = unit_types_block.scenario_event_count;
  if (scenario_events_loaded != 0) {
    unit_types_block.events_list_head = (ScenarioEvent *)unit_types_block.events_pool_base;
    event = unit_types_block.events_pool_base;
    string_ptr = unit_types_block.events_pool_base + unit_types_block.scenario_event_count * 0x1c4;
    if ((string_ptr <= unit_types_block.events_pool_base) || (unit_types_block.events_pool_base + unit_types_block.events_pool_size <= string_ptr)) {
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_pstrings_charptr_pevents_pstrings_scenar,(const char*)&str_d_ss_franklinton_netmgr_poll_cpp_6,0x913);
    }
    for (i = 0; i < event_count; i++) {
      ((ScenarioEvent *)event)->next = 0;
      ((ScenarioEvent *)event)->prev = 0;
      if (0 < i) {
        ((ScenarioEvent *)event)->prev = (ScenarioEvent *)event - 1;
        ((ScenarioEvent *)event)->prev->next = (ScenarioEvent *)event;
      }
      event += sizeof(ScenarioEvent);
    }
    for (event = (char *)unit_types_block.events_list_head; event != (char *)0x0;
        event = (char *)((ScenarioEvent *)event)->next) {
      if ((int)((ScenarioEvent *)event)->unit_name != 0) {
        ((ScenarioEvent *)event)->unit_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_08 = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_08 = str_len + 1;
        }
        string_ptr += len_08;
      }
      if ((int)((ScenarioEvent *)event)->city_name != 0) {
        ((ScenarioEvent *)event)->city_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_10 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_10 = str_len + 1;
        }
        string_ptr += len_10;
      }
      if ((int)((ScenarioEvent *)event)->who_name != 0) {
        ((ScenarioEvent *)event)->who_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_14 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_14 = str_len + 1;
        }
        string_ptr += len_14;
      }
      if ((int)((ScenarioEvent *)event)->whom_name != 0) {
        ((ScenarioEvent *)event)->whom_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_20 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_20 = str_len + 1;
        }
        string_ptr += len_20;
      }
      for (i = 0; i < 0x14; i++) {
        if ((int)((ScenarioEvent *)event)->text[i] != 0) {
          ((ScenarioEvent *)event)->text[i] = string_ptr;
          str_len = _strlen(string_ptr);
          if (str_len + 1 < 2) {
            len_slot = 1;
          }
          else {
            str_len = _strlen(string_ptr);
            len_slot = str_len + 1;
          }
          string_ptr += len_slot;
        }
      }
      if ((int)((ScenarioEvent *)event)->move_owner_name != 0) {
        ((ScenarioEvent *)event)->move_owner_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_88 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_88 = str_len + 1;
        }
        string_ptr += len_88;
      }
      if ((int)((ScenarioEvent *)event)->move_unit_name != 0) {
        ((ScenarioEvent *)event)->move_unit_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_90 = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_90 = str_len + 1;
        }
        string_ptr += len_90;
      }
      if ((int)((ScenarioEvent *)event)->aggression_whom_name != 0) {
        ((ScenarioEvent *)event)->aggression_whom_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_c4 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_c4 = str_len + 1;
        }
        string_ptr += len_c4;
      }
      if ((int)((ScenarioEvent *)event)->aggression_who_name != 0) {
        ((ScenarioEvent *)event)->aggression_who_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_cc = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_cc = str_len + 1;
        }
        string_ptr += len_cc;
      }
      if ((int)((ScenarioEvent *)event)->create_owner_name != 0) {
        ((ScenarioEvent *)event)->create_owner_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_d4 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_d4 = str_len + 1;
        }
        string_ptr += len_d4;
      }
      if ((int)((ScenarioEvent *)event)->create_unit_name != 0) {
        ((ScenarioEvent *)event)->create_unit_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_dc = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_dc = str_len + 1;
        }
        string_ptr += len_dc;
      }
      if ((int)((ScenarioEvent *)event)->create_homecity != 0) {
        ((ScenarioEvent *)event)->create_homecity = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_13c = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_13c = str_len + 1;
        }
        string_ptr += len_13c;
      }
      if ((int)((ScenarioEvent *)event)->field_140 != 0) {
        ((ScenarioEvent *)event)->field_140 = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_140 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_140 = str_len + 1;
        }
        string_ptr += len_140;
      }
      if ((int)((ScenarioEvent *)event)->field_148 != 0) {
        ((ScenarioEvent *)event)->field_148 = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x10) {
          len_148 = 0xf;
        }
        else {
          str_len = _strlen(string_ptr);
          len_148 = str_len + 1;
        }
        string_ptr += len_148;
      }
      if ((int)((ScenarioEvent *)event)->money_receiver_name != 0) {
        ((ScenarioEvent *)event)->money_receiver_name = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 0x19) {
          len_174 = 0x18;
        }
        else {
          str_len = _strlen(string_ptr);
          len_174 = str_len + 1;
        }
        string_ptr += len_174;
      }
      if ((int)((ScenarioEvent *)event)->wave_file != 0) {
        ((ScenarioEvent *)event)->wave_file = string_ptr;
        str_len = _strlen(string_ptr);
        if (str_len + 1 < 2) {
          len_184 = 1;
        }
        else {
          str_len = _strlen(string_ptr);
          len_184 = str_len + 1;
        }
        string_ptr += len_184;
      }
    }
  }
  return;
}

/* 004ce903 auto */
int __cdecl events_alloc_string(char *text,ScenarioEvents *events,uint min_len)

{
  size_t len;
  int str;
  i32 alloc_len;
  
  len = _strlen(text);
  if ((int)min_len < (int)(len + 1)) {
    len = _strlen(text);
    alloc_len = len + 1;
  }
  else {
    alloc_len = min_len;
  }
  str = memblock_alloc((MemBlock *)events->events_string_pool,alloc_len);
  if (str == 0) {
    error_report(0xfffffff7,3,0,0,0);
  }
  crt_strcpy((char *)((void *)str),text);
  return str;
}

/* 004ce98e auto */
void __cdecl events_copy(ScenarioEvents *dst_events,ScenarioEvents *src_events)

{
  ScenarioEvent *new_event;
  i32 str;
  ScenarioEvent *dst_event;
  ScenarioEvent *src_event;
  int i;
  
  for (src_event = src_events->event_list; src_event != 0; src_event = src_event->next) {
    new_event = dst_events->alloc();
    if (new_event == 0) {
      error_report(0xfffffff7,3,0,0,0);
    }
    crt_memcpy((void *)new_event,(const void *)src_event,0x1bc);
  }
  src_event = src_events->event_list;
  dst_event = dst_events->event_list;
  do {
    if (src_event == 0) {
      return;
    }
    if (src_event->unit_name != 0) {
      str = events_alloc_string(src_event->unit_name,dst_events,0xf);
      dst_event->unit_name = (char *)str;
      if (dst_event->unit_name == 0) {
        return;
      }
    }
    if (src_event->city_name != 0) {
      str = events_alloc_string(src_event->city_name,dst_events,0x18);
      dst_event->city_name = (char *)str;
      if (dst_event->city_name == 0) {
        return;
      }
    }
    if (src_event->who_name != 0) {
      str = events_alloc_string(src_event->who_name,dst_events,0x18);
      dst_event->who_name = (char *)str;
      if (dst_event->who_name == 0) {
        return;
      }
    }
    if (src_event->whom_name != 0) {
      str = events_alloc_string(src_event->whom_name,dst_events,0x18);
      dst_event->whom_name = (char *)str;
      if (dst_event->whom_name == 0) {
        return;
      }
    }
    for (i = 0; (i < 0x14 && (src_event->text[i] != 0));
        i = i + 1) {
      str = events_alloc_string(src_event->text[i],dst_events,1);
      dst_event->text[i] = (char *)str;
      if (dst_event->text[i] == 0) {
        return;
      }
    }
    if (src_event->move_owner_name != 0) {
      str = events_alloc_string(src_event->move_owner_name,dst_events,0x18);
      dst_event->move_owner_name = (char *)str;
      if (dst_event->move_owner_name == 0) {
        return;
      }
    }
    if (src_event->move_unit_name != 0) {
      str = events_alloc_string(src_event->move_unit_name,dst_events,0xf);
      dst_event->move_unit_name = (char *)str;
      if (dst_event->move_unit_name == 0) {
        return;
      }
    }
    if (src_event->aggression_whom_name != 0) {
      str = events_alloc_string(src_event->aggression_whom_name,dst_events,0x18);
      dst_event->aggression_whom_name = (char *)str;
      if (dst_event->aggression_whom_name == 0) {
        return;
      }
    }
    if (src_event->aggression_who_name != 0) {
      str = events_alloc_string(src_event->aggression_who_name,dst_events,0x18);
      dst_event->aggression_who_name = (char *)str;
      if (dst_event->aggression_who_name == 0) {
        return;
      }
    }
    if (src_event->create_owner_name != 0) {
      str = events_alloc_string(src_event->create_owner_name,dst_events,0x18);
      dst_event->create_owner_name = (char *)str;
      if (dst_event->create_owner_name == 0) {
        return;
      }
    }
    if (src_event->create_unit_name != 0) {
      str = events_alloc_string(src_event->create_unit_name,dst_events,0xf);
      dst_event->create_unit_name = (char *)str;
      if (dst_event->create_unit_name == 0) {
        return;
      }
    }
    if (src_event->create_homecity != 0) {
      str = events_alloc_string(src_event->create_homecity,dst_events,0x18);
      dst_event->create_homecity = (char *)str;
      if (dst_event->create_homecity == 0) {
        return;
      }
    }
    if (src_event->field_140 != 0) {
      str = events_alloc_string(src_event->field_140,dst_events,0x18);
      dst_event->field_140 = (char *)str;
      if (dst_event->field_140 == 0) {
        return;
      }
    }
    if (src_event->field_148 != 0) {
      str = events_alloc_string(src_event->field_148,dst_events,0xf);
      dst_event->field_148 = (char *)str;
      if (dst_event->field_148 == 0) {
        return;
      }
    }
    if (src_event->money_receiver_name != 0) {
      str = events_alloc_string(src_event->money_receiver_name,dst_events,0x18);
      dst_event->money_receiver_name = (char *)str;
      if (dst_event->money_receiver_name == 0) {
        return;
      }
    }
    if (src_event->wave_file != 0) {
      str = events_alloc_string(src_event->wave_file,dst_events,1);
      dst_event->wave_file = (char *)str;
      if (dst_event->wave_file == 0) {
        return;
      }
    }
    src_event = src_event->next;
    dst_event = dst_event->next;
  } while( true );
}

/* (port) ScenarioEvents::reset (004fa5d9) for events_compact: the pool
   frees its block and allocates one of the same size, which under Wine is
   the block just freed, at the same address (seen in the events replay:
   the editor's text lines go there).  The original relies on it: its
   callers keep event pointers across events_compact (editor_text_dlg_on_ok
   writes the text lines through editor_lists.text_lines_out,
   events_editor_edit_action reads event->actions), and they stay valid
   while compacting does not move the event.  So the pool keeps its block
   here, cleared as GMEM_ZEROINIT clears a new one, and unlocked as
   memblock_free leaves it. */
static void events_reset_keep_block(ScenarioEvents *events,uint pool_size)
{
  MemBlock *pool;

  pool = (MemBlock *)events->events_string_pool;
  if ((char)pool->owns_handle == '\0' || pool->mem_handle == 0 || pool->capacity != (pool_size & 0xffff)) {
    events->reset(pool_size);
    return;
  }
  events->event_list = 0;
  events->event_count = 0;
  memblock_ensure_locked(pool);
  _memset((void *)pool->base_ptr,0,pool->capacity);
  memblock_unlock(pool);
  pool->block_id = 0xc;
  pool->used = 0;
  pool->available = (u16)pool_size;
  pool->capacity = (u16)pool_size;
}

/* 004cef35 auto */
void __cdecl events_compact(void)

{
  ScenarioEvents temp_events(50000);
  
  temp_events.reset(50000);
  events_copy(&temp_events,(ScenarioEvents *)&unit_types_block.scenario_events);
  events_reset_keep_block((ScenarioEvents *)&unit_types_block.scenario_events,50000);
  events_copy((ScenarioEvents *)&unit_types_block.scenario_events,&temp_events);
  return;
}

/* 004cefe9 auto */
int __cdecl events_rename_unit_refs(char *old_name,const char *new_name)

{
  int cmp;
  ScenarioEvent *event;
  i32 count;
  
  count = 0;
  for (event = unit_types_block.events_list_head; event != 0; event = event->next) {
    if (event->unit_name != 0) {
      cmp = _strcmp(event->unit_name,old_name);
      if (cmp == 0) {
        _strncpy(event->unit_name,(char *)new_name,0xf);
        count++;
      }
    }
    if (event->move_unit_name != 0) {
      cmp = _strcmp(event->move_unit_name,old_name);
      if (cmp == 0) {
        _strncpy(event->move_unit_name,(char *)new_name,0xf);
        count++;
      }
    }
    if (event->create_unit_name != 0) {
      cmp = _strcmp(event->create_unit_name,old_name);
      if (cmp == 0) {
        _strncpy(event->create_unit_name,(char *)new_name,0xf);
        count++;
      }
    }
    if (event->field_148 != 0) {
      cmp = _strcmp(event->field_148,old_name);
      if (cmp == 0) {
        _strncpy(event->field_148,(char *)new_name,0xf);
        count++;
      }
    }
  }
  return count;
}

/* 004fa250 auto */
i32 __cdecl events_parse_civ(const char *name2)

{
  int cmp;
  i32 result;
  char *_Str2;
  int civ;
  
  cmp = __strcmpi((char *)name2,str_anybody);
  if (cmp == 0) {
    result = 0xfffffffe;
  }
  else {
    cmp = __strcmpi((char *)name2,str_triggerattacker);
    if (cmp == 0) {
      result = 0xfffffffd;
    }
    else {
      cmp = __strcmpi((char *)name2,str_triggerdefender);
      if (cmp == 0) {
        result = 0xfffffffc;
      }
      else {
        cmp = __strcmpi((char *)name2,str_triggerreceiver);
        if (cmp == 0) {
          result = 0xfffffffc;
        }
        else {
          for (civ = 0; civ < 8; civ++) {
            _Str2 = civ_name_plural(civ);
            cmp = __strcmpi((char *)name2,_Str2);
            if (cmp == 0) {
              return civ;
            }
          }
          if (events_parse_verbose != 0) {
            _printf(str_could_not_find_s_check_spelling,name2);
          }
          result = 0xffffffff;
        }
      }
    }
  }
  return result;
}

/* 004fa359 auto */
i32 __cdecl events_parse_unit_type(const char *name2)

{
  int cmp;
  i32 result;
  char *_Str2;
  iptr unit_type;
  
  cmp = __strcmpi((char *)name2,str_anyunit);
  if (cmp == 0) {
    result = 0xfffffffe;
  }
  else {
    for (unit_type = 0; unit_type < 0x3e; unit_type++) {
      _Str2 = label_string(unit_types[unit_type].name);
      cmp = __strcmpi((char *)name2,_Str2);
      if (cmp == 0) {
        return unit_type;
      }
    }
    if (events_parse_verbose != 0) {
      _printf(str_could_not_find_unit_s_check_spelling,name2);
    }
    result = 0xffffffff;
  }
  return result;
}

/* 004fa47e auto */
void ScenarioEvents::init_string_pool(uint pool_size)

{
  memblock_free((MemBlock *)events_string_pool);
  memblock_init((MemBlock *)events_string_pool,0xc,pool_size);
  return;
}

/* 004fa4be auto */
ScenarioEvents::ScenarioEvents(i32 pool_size) : Dialog(0x4000)

{
  ((MemBlock *)events_string_pool)->construct();
  events_pool_valid = '\0';
  event_list = 0;
  event_count = 0;
  reset(pool_size);
}

/* 004fa569 auto */
ScenarioEvents::~ScenarioEvents()

{
  memblock_free((MemBlock *)events_string_pool);
  /* then the Dialog base's destructor (the unwind funclet 004fa5b8) */
}

/* 004fa5d9 auto */
void ScenarioEvents::reset(i32 pool_size)

{
  event_list = 0;
  event_count = 0;
  init_string_pool(pool_size);
  return;
}

/* 004fa617 auto */
ScenarioEvent *ScenarioEvents::alloc()

{
  ScenarioEvent *_Dst;
  ScenarioEvent *last;
  
  last = event_list;
  if (last != 0) {
    for (; (int)last->next != 0; last = last->next) {
    }
  }
  _Dst = (ScenarioEvent *)(void *)memblock_alloc((MemBlock *)events_string_pool,0x1c4);
  if (_Dst == (void *)0x0) {
    _Dst = (ScenarioEvent *)(void *)0x0;
  }
  else {
    _memset(_Dst,0,0x1c4);
    if (last == 0) {
      event_list = _Dst;
      _Dst->next = 0;
      _Dst->prev = 0;
    }
    else {
      last->next = _Dst;
      _Dst->prev = last;
      _Dst->next = 0;
    }
    event_count++;
  }
  return _Dst;
}

/* 004fa707 auto */
i32 ScenarioEvents::action_play_wave(ScenarioEvent *event)

{
  i32 result;
  int exists;
  i8 path[0x208];  /* (port) 240 in the original: scenario_dir_path (264), SOUND\
                      and the event's file name (up to 255, BF-296) */
  
  if (((byte)game.game_options & OPT_SOUND) == 0) {
    result = 0;
  }
  else {
    if (event->wave_file != 0) {
      crt_strcpy(path,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
      crt_strcat(path,str_sound_2);
      /* BF-081 (Peter Johnson's ledger): a longer name is cut (and not found) */
      str_append_max(path,(const char *)((const void *)event->wave_file),sizeof path);
      exists = file_exists((const char *)path);
      if (exists == 0) {
        crt_strcpy(path,((i8 *)&game_dirs.home));
        crt_strcat(path,str_sound_3);
        str_append_max(path,(const char *)((const void *)event->wave_file),sizeof path);
        exists = file_exists((const char *)path);
        if (exists != 0) {
          snd_wave_play((const char *)path,0,0,0);
        }
      }
      else {
        snd_wave_play((const char *)path,0,0,0);
      }
    }
    result = 1;
  }
  return result;
}

/* 004fa82d auto */
i32 ScenarioEvents::action_dont_play_wonders(ScenarioEvent *event)

{
  game_palette_block.event_dont_play_wonders = 1;
  return 1;
}

/* 004fa854 auto */
i32 ScenarioEvents::action_play_cd(ScenarioEvent *event)

{
  i32 result;
  int track_count;
  int attempts;
  
  attempts = 0;
  while (attempts < 10) {
    attempts++;
    track_count = snd_cd_get_track_count();
    if ((((track_count == 0x18) || (track_count == 0x12)) || (track_count == 0xc)) || (track_count == 10)) break;
    snd_cd_close();
  }
  if (((track_count == 10) || (track_count == 0x12)) || ((track_count == 0x18 || (track_count == 0xc)))) {
    if ((event->cd_track < 2) || (track_count < event->cd_track)) {
      result = 0;
    }
    else {
      music_play_track(event->cd_track,1);
      result = 1;
    }
  }
  else {
    result = 0;
  }
  return result;
}

/* 004fa944 auto */
i32 ScenarioEvents::action_change_money(ScenarioEvent *event)

{
  i32 result;
  int new_gold;
  iptr civ;
  
  if (event->money_receiver_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->money_receiver_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->money_receiver_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->money_receiver_civ;
  }
  if ((civ < 1) || (7 < civ)) {
    result = 0;
  }
  else if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
    result = 0;
  }
  else {
    new_gold = event->money_amount + civs[civ].gold;
    if ((new_gold < 0) || (extras_gold_limit() < new_gold)) {  /* X52: 30000 unless set */
      if (event->money_amount < 0) {
        civs[civ].gold = 0;
      }
      else {
        civs[civ].gold = extras_gold_limit();
      }
    }
    else {
      civs[civ].gold = new_gold;
    }
    result = 1;
  }
  return result;
}

/* 004faab0 auto */
i32 ScenarioEvents::action_text(ScenarioEvent *event)

{
  int line;
  int arg_count;
  
  arg_count = 0;
  init_default();
  for (line = 0; line < 0x14; line++) {
    if (event->text[line] != 0) {
      add_line((const char *)event->text[line],0xffffffff,0xffffffff,
                 0xffffffff,0);
      if (2 < game.multiplayer) {
        crt_strcpy(((i8 *)&text_args.strings) + arg_count * 0x104,(const char *)event->text[line]);
        arg_count++;
      }
    }
  }
  if (autoplay.active == 0) {
    if (2 < game.multiplayer) {
      net_send_text_args(0x4f,0xff,arg_count,0,0,0);
    }
    show_modal(0);
  }
  return 1;
}

/* 004faba6 auto */
i32 ScenarioEvents::action_make_aggression(ScenarioEvent *event)

{
  i32 result;
  int civ2;
  int civ;
  
  if (event->aggression_who_civ == -3) {
    civ2 = event->who_civ;
  }
  else if (event->aggression_who_civ == -4) {
    civ2 = event->whom_civ;
  }
  else if (event->aggression_who_civ == -4) {
    civ2 = event->whom_civ;
  }
  else {
    civ2 = event->aggression_who_civ;
  }
  if (event->aggression_whom_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->aggression_whom_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->aggression_whom_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->aggression_whom_civ;
  }
  if (((civ < 1) || (7 < civ)) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else if (((civ2 < 1) || (7 < civ2)) ||
          ((1 << ((byte)civ2 & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else {
    result = diplo_force_war(civ2,civ);
  }
  return result;
}

/* 004fad02 auto */
i32 ScenarioEvents::action_destroy_civ(ScenarioEvent *event)

{
  i32 result;
  int civ;
  
  if (event->destroy_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->destroy_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->destroy_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->destroy_civ;
  }
  if (((civ < 1) || (7 < civ)) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else {
    if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
       (human_civ[0] == civ)) {
      game_over_reason = 4;
    }
    civ_eliminate(civ,1,1);
    result = 1;
  }
  return result;
}

/* 004fadfb auto */
i32 ScenarioEvents::action_give_tech(ScenarioEvent *event)

{
  i32 result;
  int civ;
  
  if (event->give_tech_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->give_tech_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->give_tech_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->give_tech_civ;
  }
  if (((civ < 1) || (7 < civ)) ||
     ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0)) {
    result = 0;
  }
  else {
    civ_give_advance(civ,event->give_tech,0,0,0);
    result = 1;
  }
  return result;
}

/* 004faed4 auto */
i32 ScenarioEvents::action_create_unit(ScenarioEvent *event)

{
  iptr x;
  iptr y;
  i32 result;
  iptr in_bounds;
  iptr is_ocean;
  uptr owner;
  iptr unit;
  int city2;
  int loc;
  int y2;
  int x2;
  uptr civ;
  iptr city;
  
  x2 = 10000;
  y2 = 10000;
  if (event->create_owner_civ == -3) {
    civ = (uptr)event->who_civ;
  }
  else if (event->create_owner_civ == -4) {
    civ = (uptr)event->whom_civ;
  }
  else if (event->create_owner_civ == -4) {
    civ = (uptr)event->whom_civ;
  }
  else {
    civ = (uptr)event->create_owner_civ;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
    result = 0;
  }
  else {
    for (loc = 0; y = y2, x = x2, loc < event->create_location_count;
        loc = loc + 1) {
      x = event->create_locations[loc * 2];
      y = event->create_locations[loc * 2 + 1];
      in_bounds = map_in_bounds(x,y);
      if (in_bounds != 0) {
        if (unit_types[event->create_unit_type].domain == DOMAIN_SEA) {
          is_ocean = tile_is_ocean(x,y);
          if (is_ocean != 0) break;
          city2 = city_at(x,y);
          if ((city2 != -1) &&
             (city2 = city_at(x,y), (cities[city2].flags & CITYF_COASTAL) != 0)) {
            owner = tile_city_owner(x,y);
            if (owner == civ) break;
          }
        }
        else {
          city2 = tile_is_ocean(x,y);
          if (((city2 == 0) || (unit_types[event->create_unit_type].domain == DOMAIN_AIR)) &&
             ((city2 = city_at(x,y), city2 == -1 ||
              (owner = tile_city_owner(x,y), owner == civ)))) {
            city2 = tile_unit_owner(x,y);
            if (city2 == -1) break;
            owner = tile_unit_owner(x,y);
            if (owner == civ) break;
          }
        }
      }
    }
    x2 = x;
    y2 = y;
    if ((x2 == 10000) && (y2 == 10000)) {
      result = 0;
    }
    else {
      unit = unit_create(event->create_unit_type,civ,x2,y2);
      if (unit == -1) {
        result = 0;
      }
      else {
        if (event->create_veteran != 0) {
          units[unit].flags = units[unit].flags | UNITF_VETERAN;
        }
        units[unit].home_city = -1;
        if (event->create_homecity != 0) {
          for (city = 0; city < game.num_cities; city++) {
            if ((cities[city].id != 0) &&
               (city2 = __strcmpi(cities[city].name,event->create_homecity), city2 == 0)) {
              if ((int)cities[city].owner == civ) {
                units[unit].home_city = (i8)city;
              }
              break;
            }
          }
        }
        result = 1;
      }
    }
  }
  return result;
}

/* 004fb29f auto */
int ScenarioEvents::action_move_unit(ScenarioEvent *event)

{
  int x;
  int y;
  int in_bounds;
  iptr unit;
  int moved;
  iptr i;
  int civ;
  int rect_y[4];
  int rect_x[4];
  
  if (event->move_owner_civ == -3) {
    civ = event->who_civ;
  }
  else if (event->move_owner_civ == -4) {
    civ = event->whom_civ;
  }
  else if (event->move_owner_civ == -4) {
    civ = event->whom_civ;
  }
  else {
    civ = event->move_owner_civ;
  }
  if ((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) == 0) {
    moved = 0;
  }
  else if ((1 << ((byte)civ & 0x1f) & (uint)game.human_players) == 0) {
    for (i = 0; i < 4; i++) {
      rect_x[i] = event->move_rect[i * 2];
      rect_y[i] = event->move_rect[i * 2 + 1];
    }
    moved = 0;
    unit = (iptr)game.num_units;
    do {
      do {
        do {
          unit--;
          if (unit < 0) {
            return moved;
          }
        } while ((((units[unit].id == 0) ||
                  ((((event->move_unit_type != (bugfix_state() ? (iptr)units[unit].type : unit) &&  /* B23: the unit's type, not its slot (docs/BUGS.md) */
                     (event->move_unit_type != -2)) ||
                    (units[unit].owner != civ)) ||
                   (('b' < (char)unit_types[units[unit].type].attack ||
                    (units[unit].orders == ORDER_AI_IDLE)))))) || (3 < units[unit].orders)) ||
                (units[unit].orders == ORDER_SLEEP));
        x = (int)units[unit].x;
        y = (int)units[unit].y;
        in_bounds = map_in_bounds(x,y);
      } while ((((in_bounds == 0) || (x < rect_x[0])) ||
               ((rect_x[1] < x ||
                (((rect_x[2] < x || (x < rect_x[3])) || (y < rect_y[0])))))) ||
              (((y < rect_y[1] || (rect_y[2] < y)) || (rect_y[3] < y))));
      units[unit].orders = ORDER_GOTO;
      units[unit].ai_task = '7';
      units[unit].goto_x = (i16)event->move_to_x;
      units[unit].goto_y = (i16)event->move_to_y;
      moved++;
    } while ((moved < event->move_count) || (event->move_count == -2));
  }
  else {
    moved = 0;
  }
  return moved;
}

/* 004fb5b2 auto */
int ScenarioEvents::action_change_terrain(ScenarioEvent *event)

{
  bool has_city;
  iptr in_bounds;
  uptr city2;
  iptr in_bounds2;
  i8 *terrain;
  Tile *tile;
  int civ;
  iptr unit;
  iptr city;
  iptr i;
  int y2;
  int changed;
  int x2;
  int rect_y[4];
  int rect_x[4];
  
  changed = 0;
  for (i = 0; i < 4; i++) {
    rect_x[i] = event->terrain_rect[i * 2];
    rect_y[i] = event->terrain_rect[i * 2 + 1];
  }
  for (x2 = rect_x[0]; x2 <= rect_x[1]; x2++) {
    for (y2 = rect_y[0]; y2 <= rect_y[3]; y2++) {
      in_bounds = map_in_bounds(x2,y2);
      if (in_bounds != 0) {
        city2 = city_at(x2,y2);
        if (-1 < (int)city2) {
          city_delete(city2,0);
        }
        for (unit = tile_first_unit(x2,y2); -1 < unit;
            unit = unit_next_in_stack(unit)) {
          unit_delete(unit,1);
        }
      }
    }
  }
  for (x2 = rect_x[0]; x2 <= rect_x[1]; x2++) {
    for (y2 = rect_y[0]; y2 <= rect_y[3]; y2++) {
      in_bounds2 = map_in_bounds(x2,y2);
      if (in_bounds2 != 0) {
        terrain = (i8 *)tile_at(x2,y2);
        *terrain = (i8)event->terrain_type;
        tile = tile_at(x2,y2);
        tile->improvements = 0;
        tile = tile_at(x2,y2);
        tile->radius = 0;
        tile = tile_at(x2,y2);
        tile->landmass = 0;
        for (civ = 0; civ < 8; civ++) {
          tile_update_known(x2,y2,civ);
        }
        map_redraw_tile_all_views(x2,y2);
        changed++;
      }
    }
  }
  for (city = 0; city < game.num_cities; city++) {
    if (cities[city].id != 0) {
      city_refresh_area_tiles(city);
    }
  }
  for (civ = 0; civ < 8; civ++) {
    has_city = false;
    for (city = 0; city < game.num_cities; city++) {
      if ((cities[city].id != 0) && (cities[city].owner == civ)) {
        has_city = true;
      }
    }
    if (!has_city) {
      unit = (iptr)game.num_units;
      while (unit = unit - 1, -1 < unit) {
        if ((units[unit].id != 0) && (units[unit].owner == civ)) {
          unit_delete(unit,1);
        }
      }
      if (((1 << ((byte)civ & 0x1f) & (uint)game.human_players) != 0) &&
         (human_civ[0] == civ)) {
        game_over_reason = 4;
      }
      for (x2 = 0; x2 < map.width; x2++) {
        for (y2 = 0; y2 < map.height; y2++) {
          tile = tile_at(x2,y2);
          tile->visible = tile->visible & ~(byte)(1 << ((byte)civ & 0x1f));
        }
      }
      civ_create(civ);
      map_redraw_all_views(human_civ[0],1);
    }
  }
  return changed;
}

/* 004fba0c auto */
i32 ScenarioEvents::check_turn(int turn)

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((event->trigger == 4) && ((event->turn == -1 || (event->turn == turn)))) {
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fba9c auto */
i32 ScenarioEvents::check_turn_interval(int turn)

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((event->trigger == 8) && ((event->turn == 0 || (turn % event->turn == 0)))) {
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbb2f auto */
i32 ScenarioEvents::check_random_turn(i32 turn)

{
  int rnd;
  int roll;
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if (event->trigger == 0x40) {
        if (event->denominator < 2) {
          roll = 1;
        }
        else {
          rnd = crt_rand_at("events_check_random_turn");
          roll = rnd % event->denominator + 1;
        }
        if (event->denominator == roll) {
          triggered = 1;
          run_actions(event);
        }
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbbdd auto */
i32 ScenarioEvents::check_received_tech()

{
  int has_tech;
  int civ;
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if (event->trigger == 0x100) {
        if (event->whom_civ == -2) {
          for (civ = 0; civ < 8; civ++) {
            if (((1 << ((byte)civ & 0x1f) & (uint)game.civs_alive) != 0) &&
               (has_tech = civ_has_advance(civ,event->technology), has_tech != 0)) {
              event->whom_civ = civ;
              triggered = 1;
              run_actions(event);
            }
          }
        }
        else if ((((0 < event->whom_civ) || (event->whom_civ < 8)) &&
                 ((1 << ((byte)event->whom_civ & 0x1f) & (uint)game.civs_alive) != 0)) &&
                (has_tech = civ_has_advance(event->whom_civ,event->technology), has_tech != 0)) {
          triggered = 1;
          run_actions(event);
        }
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbd2b auto */
i32 ScenarioEvents::check_scenario_loaded()

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if (event->trigger == 0x20) {
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbd9d auto */
i32 ScenarioEvents::check_unit_killed(int unit_type,int attacker_civ,int defender_civ)

{
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((((event->trigger == 1) && (event->unit_type == unit_type)) &&
          ((event->whom_civ == defender_civ || (event->whom_civ == -2)))) &&
         ((event->who_civ == attacker_civ || (event->who_civ == -2)))) {
        triggered = 1;
        if (event->who_civ == -2) {
          event->who_civ = attacker_civ;
        }
        if (event->whom_civ == -2) {
          event->whom_civ = defender_civ;
        }
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fbe84 auto */
i32 ScenarioEvents::check_negotiation(int talker_civ,int listener_civ)

{
  ScenarioEvent *event;
  bool talker_match;
  bool listener_match;
  
  for (event = event_list; ; event = event->next) {
    if (event == (ScenarioEvent *)0x0) {
      return 1;
    }
    if (event->trigger == 0x10) {
      if (((event->who_civ == talker_civ) || (event->who_civ == -2)) &&
         ((((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) != 0 &&
           ((event->talker_type == 1 || (event->talker_type == 4)))) ||
          (((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) == 0 &&
           ((event->talker_type == 2 || (event->talker_type == 4)))))))) {
        talker_match = true;
      }
      else {
        talker_match = false;
      }
      if (((event->whom_civ == listener_civ) || (event->whom_civ == -2)) &&
         (((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) != 0 &&
           ((event->listener_type == 1 || (event->listener_type == 4)))) ||
          ((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) == 0 &&
           ((event->listener_type == 2 || (event->listener_type == 4)))))))) {
        listener_match = true;
      }
      else {
        listener_match = false;
      }
      if (!((talker_match) && (listener_match))) {
        if (!((2 < game.multiplayer) &&
           (((1 << ((byte)talker_civ & 0x1f) & (uint)game.human_players) != 0 &&
            ((1 << ((byte)listener_civ & 0x1f) & (uint)game.human_players) != 0))))) continue;
        if (((event->who_civ == listener_civ) || (event->who_civ == -2)) &&
           ((((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) != 0 &&
             ((event->talker_type == 1 || (event->talker_type == 4)))) ||
            (((1 << ((byte)event->who_civ & 0x1f) & (uint)game.human_players) == 0 &&
             ((event->talker_type == 2 || (event->talker_type == 4)))))))) {
          talker_match = true;
        }
        else {
          talker_match = false;
        }
        if (((event->whom_civ == talker_civ) || (event->whom_civ == -2)) &&
           (((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) != 0 &&
             ((event->listener_type == 1 || (event->listener_type == 4)))) ||
            ((((uint)game.human_players & 1 << ((byte)event->whom_civ & 0x1f)) == 0 &&
             ((event->listener_type == 2 || (event->listener_type == 4)))))))) {
          listener_match = true;
        }
        else {
          listener_match = false;
        }
        if (!((talker_match) && (listener_match))) continue;
      }
      if (event->negotiation_flag == 0x1000) {
        run_actions(event);
        return 0;
      }
      run_actions(event);
      return 1;
    }
  }
}

/* 004fc20d auto */
i32 ScenarioEvents::check_no_schism(int civ)

{
  bool triggered;
  i32 result;
  ScenarioEvent *event;
  
  triggered = false;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    result = 1;
  }
  else {
    do {
      if ((event->trigger == 0x80) && ((event->whom_civ == civ || (event->whom_civ == -2)))) {
        run_actions(event);
        triggered = true;
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
    if (triggered) {
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
}

/* 004fc2bb auto */
i32 ScenarioEvents::check_city_taken(const char *city_name,int attacker_civ,int defender_civ)

{
  int cmp;
  i32 triggered;
  ScenarioEvent *event;
  
  triggered = 0;
  event = event_list;
  if (event == (ScenarioEvent *)0x0) {
    triggered = 0;
  }
  else {
    do {
      if ((((event->trigger == 2) && (cmp = __strcmpi(event->city_name,(char *)city_name), cmp == 0))
          && ((event->who_civ == attacker_civ || (event->who_civ == -2)))) &&
         ((event->whom_civ == defender_civ || (event->whom_civ == -2)))) {
        if (event->who_civ == -2) {
          event->who_civ = attacker_civ;
        }
        if (event->whom_civ == -2) {
          event->whom_civ = defender_civ;
        }
        triggered = 1;
        run_actions(event);
      }
      event = event->next;
    } while (event != (ScenarioEvent *)0x0);
  }
  return triggered;
}

/* 004fc3ae auto */
i32 ScenarioEvents::run_actions(ScenarioEvent *event)

{
  i32 result;
  
  if (((event->actions_bytes[0] & 0x40) == 0) || ((event->actions_bytes[1] & 0x20) == 0)) {
    if ((event->actions_bytes[0] & 0x10) != 0) {
      action_play_wave(event);
    }
    if ((event->actions_bytes[0] & 0x80) != 0) {
      action_play_cd(event);
    }
    if ((event->actions_bytes[0] & 4) != 0) {
      action_create_unit(event);
    }
    if ((event->actions_bytes[0] & 2) != 0) {
      action_move_unit(event);
    }
    if ((event->actions_bytes[1] & 2) != 0) {
      action_change_terrain(event);
    }
    if ((event->actions_bytes[0] & 0x20) != 0) {
      action_make_aggression(event);
    }
    if ((event->actions_bytes[0] & 8) != 0) {
      action_change_money(event);
    }
    if ((event->actions_bytes[1] & 4) != 0) {
      action_destroy_civ(event);
    }
    if ((event->actions_bytes[1] & 8) != 0) {
      action_give_tech(event);
    }
    if ((event->actions_bytes[0] & 1) != 0) {
      action_text(event);
    }
    if ((event->actions_bytes[1] & 1) != 0) {
      action_dont_play_wonders(event);
    }
    if ((event->actions_bytes[0] & 0x40) != 0) {
      event->actions = event->actions | 0x2000;
    }
    result = 1;
  }
  else {
    result = 0;
  }
  return result;
}

/* The state of ScenarioEvents::parse_file (004fc516) shared by its parts:
   the event being read, the line just read and which of the event's fields
   were found (reset at each @IF). */
struct EventsParseState {
  int have_owner;
  int have_homecity;
  int have_city;
  int have_unit;
  int have_move_count;
  int have_tech;
  ScenarioEvent *event;
  int have_listener_type;
  int have_unit_killed;
  int have_moveto;
  int have_receiver;
  int have_veteran;
  int have_tech_receiver;
  int have_tech_id;
  char *line;
  int have_defender;
  int have_talker_type;
  int have_destroy_whom;
  /* the last civ read: the verbose messages of Receiver= (RECEIVEDTECHNOLOGY)
     and Attacker= / City= (CITYTAKEN) print it before it is set */
  int parsed_civ;
  int have_terrain;
  int have_amount;
  int have_who;
  int have_attacker;
  int have_whom;
};

/* Part of ScenarioEvents::parse_file (004fc516): the trigger UNITKILLED (its
   Unit=, Attacker= and Defender= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_unit_killed(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;
  int str_buf;

  if (events_parse_verbose != 0) {
    _printf(str_if_unitkilled_found_looking_for_unit_att);
  }
  while (((ps->have_unit_killed == 0 || (ps->have_defender == 0)) || (ps->have_attacker == 0))) {
    ps->line = text_read_line(1);
    if (ps->line == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file);
      }
      return 0;
    }
    if (*ps->line == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_unitkilled_statement);
      }
      return 0;
    }
    cmp = __strnicmp(ps->line,str_unit,5);
    if (cmp == 0) {
      arg = ps->line + 5;
      cmp = events_parse_unit_type((const char *)arg);
      if ((cmp == -1) || (cmp == -2)) {
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        str_buf = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->unit_name = (char *)str_buf;
        if (ps->event->unit_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->unit_name),arg);
        ps->event->unit_type = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_id_d,arg,cmp);
        }
        ps->have_unit_killed = 1;
      }
    }
    else {
      cmp = __strnicmp(ps->line,str_attacker,9);
      if (cmp == 0) {
        arg = ps->line + 9;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_attacker_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->who_name = (char *)cmp;
          if (ps->event->who_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->who_name),arg);
          ps->event->who_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_attacker_s_king_id_d,arg,ps->parsed_civ);
          }
          ps->have_attacker = 1;
        }
      }
      else {
        cmp = __strnicmp(ps->line,str_defender,9);
        if (cmp == 0) {
          arg = ps->line + 9;
          ps->parsed_civ = events_parse_civ((const char *)arg);
          if (ps->parsed_civ == -1) {
            if (events_parse_verbose != 0) {
              _printf(str_found_defender_s_but_s_is_not_valid,arg,arg);
            }
          }
          else {
            len = _strlen(arg);
            cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
            ps->event->whom_name = (char *)cmp;
            if (ps->event->whom_name == 0) return 0;
            crt_strcpy((char *)((void *)ps->event->whom_name),arg);
            ps->event->whom_civ = ps->parsed_civ;
            if (events_parse_verbose != 0) {
              _printf(str_found_defender_s_king_id_d,arg,ps->parsed_civ);
            }
            ps->have_defender = 1;
          }
        }
      }
    }
  }
  ps->event->trigger = 1;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger NEGOTIATION
   (its Talker=, TalkerType=, Listener= and ListenerType= lines). Returns 0
   when the file is bad. */
int ScenarioEvents::parse_trigger_negotiation(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_negotiation_found_looking_for_talker);
  }
  while (((ps->have_defender == 0 || (ps->have_attacker == 0)) || ((ps->have_talker_type == 0 || (ps->have_listener_type == 0))))) {
    ps->line = text_read_line(1);
    if (ps->line == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_2);
      }
      return 0;
    }
    if (*ps->line == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_negotiation_statement);
      }
      return 0;
    }
    cmp = __strnicmp(ps->line,str_talker,7);
    if (cmp == 0) {
      arg = ps->line + 7;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_talker_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->who_name = (char *)cmp;
        if (ps->event->who_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->who_name),arg);
        ps->event->who_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_talker_s_king_id_d,arg,ps->parsed_civ);
        }
        ps->have_attacker = 1;
      }
    }
    else {
      cmp = __strnicmp(ps->line,str_talkertype,0xb);
      if (cmp == 0) {
        arg = ps->line + 0xb;
        cmp = __strcmpi(arg,str_human);
        if (cmp == 0) {
          ps->event->talker_type = 1;
          ps->have_talker_type = 1;
        }
        else {
          cmp = __strcmpi(arg,str_computer);
          if (cmp == 0) {
            ps->event->talker_type = 2;
            ps->have_talker_type = 1;
          }
          else {
            cmp = __strcmpi(arg,str_humanorcomputer);
            if (cmp == 0) {
              ps->event->talker_type = 4;
              ps->have_talker_type = 1;
            }
          }
        }
        if (events_parse_verbose != 0) {
          if (ps->have_talker_type == 0) {
            _printf(str_found_talkertype_s_but_s_is_not_valid,arg,arg);
          }
          else {
            _printf(str_found_talkertype_s,arg);
          }
        }
      }
      else {
        cmp = __strnicmp(ps->line,str_listener,9);
        if (cmp == 0) {
          arg = ps->line + 9;
          ps->parsed_civ = events_parse_civ((const char *)arg);
          if (ps->parsed_civ == -1) {
            if (events_parse_verbose != 0) {
              _printf(str_found_listener_s_but_s_is_not_valid,arg,arg);
            }
          }
          else {
            len = _strlen(arg);
            cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
            ps->event->whom_name = (char *)cmp;
            if (ps->event->whom_name == 0) return 0;
            crt_strcpy((char *)((void *)ps->event->whom_name),arg);
            ps->event->whom_civ = ps->parsed_civ;
            if (events_parse_verbose != 0) {
              _printf(str_found_listener_s_king_id_d,arg,ps->parsed_civ);
            }
            ps->have_defender = 1;
          }
        }
        else {
          cmp = __strnicmp(ps->line,str_listenertype,0xd);
          if (cmp == 0) {
            arg = ps->line + 0xd;
            cmp = __strcmpi(arg,str_human_2);
            if (cmp == 0) {
              ps->event->listener_type = 1;
              ps->have_listener_type = 1;
            }
            else {
              cmp = __strcmpi(arg,str_computer_2);
              if (cmp == 0) {
                ps->event->listener_type = 2;
                ps->have_listener_type = 1;
              }
              else {
                cmp = __strcmpi(arg,str_humanorcomputer_2);
                if (cmp == 0) {
                  ps->event->listener_type = 4;
                  ps->have_listener_type = 1;
                }
              }
            }
            if (events_parse_verbose != 0) {
              if (ps->have_talker_type == 0) {
                _printf(str_found_listenertype_s_but_s_is_not_valid,arg,arg);
              }
              else {
                _printf(str_found_listenertype_s,arg);
              }
            }
          }
        }
      }
    }
  }
  ps->event->trigger = 0x10;
  ps->event->negotiation_flag = 0x1000;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger NOSCHISM (its
   Defender= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_no_schism(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_noschism_found_looking_for_whom);
  }
  while (ps->have_defender == 0) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_3);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_noschism_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_defender_2,9);
    if (cmp == 0) {
      arg += 9;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_defender_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->whom_name = (char *)cmp;
        if (ps->event->whom_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->whom_name),arg);
        ps->event->whom_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_defender_s_king_id_d_2,arg,ps->parsed_civ);
        }
        ps->have_defender = 1;
      }
    }
  }
  ps->event->trigger = 0x80;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger
   RECEIVEDTECHNOLOGY (its Receiver= and Technology= lines). Returns 0 when
   the file is bad. */
int ScenarioEvents::parse_trigger_received_tech(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_receivedtechnology_found_looking_for);
  }
  while ((ps->have_tech == 0 || (ps->have_defender == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_4);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_receivedtechnology_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_receiver,9);
    if (cmp == 0) {
      arg += 9;
      cmp = events_parse_civ((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->whom_name = (char *)cmp;
        if (ps->event->whom_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->whom_name),arg);
        cmp = events_parse_civ((const char *)arg);
        ps->event->whom_civ = cmp;
        if (events_parse_verbose != 0) {
          /* B33: the civilization just read, not the one of an earlier line (docs/BUGS.md) */
          _printf(str_found_receiver_s_king_id_d,arg,bugfix_ui() ? cmp : ps->parsed_civ);
        }
        ps->have_defender = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_technology,0xb);
      if (cmp == 0) {
        arg += 0xb;
        if ((*arg < '0') || ('9' < *arg)) {
          if (events_parse_verbose != 0) {
            _printf(str_found_technology_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          cmp = str_parse_int_prefixed((const char *)arg);
          ps->event->technology = cmp;
          ps->have_tech = 1;
          if (events_parse_verbose != 0) {
            _printf(str_found_technology_s_technology_id_d,arg,ps->event->technology);
          }
        }
      }
    }
  }
  ps->event->trigger = 0x100;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger CITYTAKEN (its
   City=, Attacker= and Defender= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_city_taken(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_citytaken_found_looking_for_city_atta);
  }
  while (((ps->have_city == 0 || (ps->have_attacker == 0)) || (ps->have_defender == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_5);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_citytaken_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_attacker_2,9);
    if (cmp == 0) {
      arg += 9;
      cmp = events_parse_civ((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_attacker_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->who_name = (char *)cmp;
        if (ps->event->who_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->who_name),arg);
        cmp = events_parse_civ((const char *)arg);
        ps->event->who_civ = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_attacker_s_king_id_d_2,arg,bugfix_ui() ? cmp : ps->parsed_civ);  /* B33 */
        }
        ps->have_attacker = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_defender_3,9);
      if (cmp == 0) {
        arg += 9;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_defender_s_but_s_is_not_valid_3,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->whom_name = (char *)cmp;
          if (ps->event->whom_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->whom_name),arg);
          ps->event->whom_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_defender_s_king_id_d_3,arg,ps->parsed_civ);
          }
          ps->have_defender = 1;
        }
      }
      else {
        cmp = __strnicmp(arg,str_city_5,5);
        if (cmp == 0) {
          arg += 5;
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->city_name = (char *)cmp;
          if (ps->event->city_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->city_name),arg);
          if (events_parse_verbose != 0) {
            _printf(str_found_city_s,arg,ps->parsed_civ);
          }
          ps->have_city = 1;
        }
      }
    }
  }
  ps->event->trigger = 2;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger TURN (its Turn=
   line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_turn(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_turn_found_looking_for_turn);
  }
  do {
    do {
      arg = text_read_line(1);
      if (arg == (char *)0x0) {
        if (events_parse_verbose != 0) {
          _printf(str_got_early_end_of_file_6);
        }
        return 0;
      }
      if (*arg == '@') {
        if (events_parse_verbose != 0) {
          _printf(str_illegal_turn_statement);
        }
        return 0;
      }
      cmp = __strnicmp(arg,str_turn,5);
    } while (cmp != 0);
    arg += 5;
    cmp = __strcmpi(arg,str_every);
    if (cmp == 0) {
      ps->event->turn = -1;
      ps->event->trigger = 4;
    }
    else if (((('/' < *arg) && (*arg < ':')) || (*arg == '-')) ||
            (*arg == '+')) {
      cmp = str_parse_int_prefixed((const char *)arg);
      ps->event->turn = cmp;
      ps->event->trigger = 4;
    }
    if (events_parse_verbose != 0) {
      if (ps->event->trigger == 4) {
        _printf(str_found_turn_s,arg);
      }
      else {
        _printf(str_found_turn_s_but_s_is_not_valid,arg,arg);
      }
    }
  } while (ps->event->trigger != 4);
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger TURNINTERVAL
   (its Interval= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_turn_interval(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_turninterval_found_looking_for_interv);
  }
  do {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_7);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_turninterval_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_interval,9);
    if (cmp == 0) {
      arg += 9;
      if ((((*arg < '0') || ('9' < *arg)) && (*arg != '-')) &&
         (*arg != '+')) {
        if (events_parse_verbose != 0) {
          _printf(str_found_interval_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->turn = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_interval_s,arg);
        }
        ps->event->trigger = 8;
      }
    }
  } while (ps->event->trigger != 8);
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the trigger RANDOMTURN (its
   Denominator= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger_random_turn(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_if_randomturn_found_looking_for_denomina);
  }
  do {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_8);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_randomturn_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_denominator,0xc);
    if (cmp == 0) {
      arg += 0xc;
      if ((*arg < '0') || ('9' < *arg)) {
        if (events_parse_verbose != 0) {
          _printf(str_found_denominator_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->denominator = cmp;
        if ((ps->event->denominator < 1) || (1000 < ps->event->denominator)) {
          _printf(str_found_denominator_s_but_s_is_not_valid,arg,arg);
          return 0;
        }
        if (events_parse_verbose != 0) {
          _printf(str_found_denominator_s,arg);
        }
        ps->event->trigger = 0x40;
      }
    }
  } while (ps->event->trigger != 0x40);
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): a line of the trigger after
   @IF (state 2). Returns 0 when the file is bad. */
int ScenarioEvents::parse_trigger(EventsParseState *ps)
{
  int cmp;

  cmp = __strcmpi(ps->line,str_unitkilled);
  if (cmp == 0) {
    if (parse_trigger_unit_killed(ps) == 0) return 0;
  }
  cmp = __strcmpi(ps->line,str_negotiation);
  if (cmp == 0) {
    if (parse_trigger_negotiation(ps) == 0) return 0;
  }
  cmp = __strcmpi(ps->line,str_noschism);
  if (cmp == 0) {
    if (parse_trigger_no_schism(ps) == 0) return 0;
  }
  else {
    cmp = __strcmpi(ps->line,str_receivedtechnology);
    if (cmp == 0) {
      if (parse_trigger_received_tech(ps) == 0) return 0;
    }
    else {
      cmp = __strcmpi(ps->line,str_citytaken);
      if (cmp == 0) {
        if (parse_trigger_city_taken(ps) == 0) return 0;
      }
      else {
        cmp = __strcmpi(ps->line,((i8 *)&events_kw_turn_str));
        if (cmp == 0) {
          if (parse_trigger_turn(ps) == 0) return 0;
        }
        else {
          cmp = __strcmpi(ps->line,str_turninterval);
          if (cmp == 0) {
            if (parse_trigger_turn_interval(ps) == 0) return 0;
          }
          else {
            cmp = __strcmpi(ps->line,str_randomturn);
            if (cmp == 0) {
              if (parse_trigger_random_turn(ps) == 0) return 0;
            }
            else {
              cmp = __strcmpi(ps->line,str_scenarioloaded_3);
              if (cmp == 0) {
                if (events_parse_verbose != 0) {
                  _printf(str_if_scenarioloaded);
                }
                ps->event->trigger = 0x20;
              }
            }
          }
        }
      }
    }
  }
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action TEXT (its lines
   up to ENDTEXT). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_text(EventsParseState *ps)
{
  int cmp;
  size_t len;
  iptr i;

  if (events_parse_verbose != 0) {
    _printf(str_then_text_found_looking_for_endtext);
  }
  i = 0;
  ps->line = text_read_line(1);
  if (ps->line == (char *)0x0) return 0;
  while (cmp = __strcmpi(ps->line,str_endtext), cmp != 0) {
    if (i != 0x14) {
      len = _strlen(ps->line);
      cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
      ps->event->text[i] = (char *)cmp;
      if (ps->event->text[i] == 0) return 0;
      crt_strcpy(ps->event->text[i],ps->line);
      i++;
    }
    ps->line = text_read_line(1);
    if (ps->line == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_9);
      }
      return 0;
    }
    if (*ps->line == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_text_statement);
      }
      return 0;
    }
  }
  if (events_parse_verbose != 0) {
    _printf(str_found_endtext);
  }
  ps->event->actions = (int)ps->event->actions | 1;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action CHANGETERRAIN
   (its TerrainType= line and MapRect). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_change_terrain(EventsParseState *ps)
{
  int cmp;
  char *arg;
  iptr i;
  int loc_count;

  if (events_parse_verbose != 0) {
    _printf(str_then_changeterrain_found_looking_for_ter);
  }
  loc_count = 0;
  while ((ps->have_terrain == 0 || (loc_count != 4))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_10);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_changeterrain_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_terraintype,0xc);
    if (cmp == 0) {
      arg += 0xc;
      if ((*arg < '0') || ('9' < *arg)) {
        if (events_parse_verbose != 0) {
          _printf(str_found_terraintype_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        cmp = str_parse_int_prefixed((const char *)arg);
        if ((-1 < cmp) || (cmp < 0xb)) {
          ps->event->terrain_type = cmp;
          if (events_parse_verbose != 0) {
            _printf(str_found_terraintype_s_terrainid_d,arg,cmp);
          }
          ps->have_terrain = 1;
        }
      }
    }
    else {
      cmp = __strcmpi(arg,str_maprect);
      if (cmp == 0) {
        if (events_parse_verbose != 0) {
          _printf(str_found_maprect);
        }
        cmp = (int)text_read_line(1);
        if (cmp == 0) {
          if (events_parse_verbose != 0) {
            _printf(str_got_early_end_of_file_11);
          }
          return 0;
        }
        for (i = 0; i < 4; i++) {
          cmp = text_next_int();
          ps->event->terrain_rect[i * 2] = cmp;
          cmp = text_next_int();
          ps->event->terrain_rect[i * 2 + 1] = cmp;
          if (events_parse_verbose != 0) {
            _printf(str_found_location_d_d_d,loc_count,
                    ps->event->terrain_rect[i * 2],ps->event->terrain_rect[i * 2 + 1]);
          }
          loc_count++;
        }
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x200;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action CREATEUNIT (its
   Unit=, Owner=, Veteran=, HomeCity= lines and Locations). Returns 0 when
   the file is bad. */
int ScenarioEvents::parse_action_create_unit(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;
  int str_buf;
  iptr i;
  int loc_count;

  if (events_parse_verbose != 0) {
    _printf(str_then_createunit_found_looking_for_unit_o);
  }
  loc_count = 0;
  while (((ps->have_unit == 0) || (ps->have_owner == 0)) ||
        ((ps->have_homecity == 0 || ((ps->have_veteran == 0 || (loc_count == 0)))))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_12);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_createunit_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_unit_2,5);
    if (cmp == 0) {
      arg += 5;
      cmp = events_parse_unit_type((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        str_buf = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->create_unit_name = (char *)str_buf;
        if (ps->event->create_unit_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->create_unit_name),arg);
        ps->event->create_unit_type = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_unit_id_d,arg,cmp);
        }
        ps->have_unit = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_owner,6);
      if (cmp == 0) {
        arg += 6;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->create_owner_name = (char *)cmp;
          if (ps->event->create_owner_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->create_owner_name),arg);
          ps->event->create_owner_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_king_id_d,arg,ps->parsed_civ);
          }
          ps->have_owner = 1;
        }
      }
      else {
        cmp = __strnicmp(arg,str_veteran,8);
        if (cmp == 0) {
          arg += 8;
          cmp = __strcmpi(arg,((i8 *)&events_bool_yes_str));
          if ((cmp == 0) || (cmp = __strcmpi(arg,((i8 *)&events_bool_true_str)), cmp == 0)) {
            ps->event->create_veteran = 1;
            ps->have_veteran = 1;
          }
          else {
            cmp = __strcmpi(arg,((i8 *)&events_bool_no_str));
            if ((cmp == 0) || (cmp = __strcmpi(arg,str_false), cmp == 0)
               ) {
              ps->event->create_veteran = 0;
              ps->have_veteran = 1;
            }
            else if (events_parse_verbose != 0) {
              _printf(str_found_veteran_s_but_s_is_not_valid,arg,arg);
            }
          }
        }
        else {
          cmp = __strnicmp(arg,str_homecity,9);
          if (cmp == 0) {
            arg += 9;
            len = _strlen(arg);
            cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
            ps->event->create_homecity = (char *)cmp;
            if (ps->event->create_homecity == 0) return 0;
            crt_strcpy((char *)((void *)ps->event->create_homecity),arg);
            if (events_parse_verbose != 0) {
              _printf(str_found_homecity_s,arg);
            }
            ps->have_homecity = 1;
          }
          else {
            cmp = __strcmpi(arg,str_locations);
            if (cmp == 0) {
              if (events_parse_verbose != 0) {
                _printf(str_found_locations);
              }
              for (i = 0; i < 10; i++) {
                arg = text_read_line(1);
                if (arg == (char *)0x0) {
                  if (events_parse_verbose != 0) {
                    _printf(str_got_early_end_of_file_13);
                  }
                  return 0;
                }
                if ((((*arg < '0') || ('9' < *arg)) && (*arg != '-')) &&
                   (*arg != '+')) {
                  cmp = __strcmpi(arg,str_endlocations);
                  if (cmp == 0) {
                    if (events_parse_verbose != 0) {
                      _printf(str_found_endlocations,arg,arg);
                    }
                    break;
                  }
                }
                else {
                  cmp = text_next_int();
                  ps->event->create_locations[i * 2] = cmp;
                  cmp = text_next_int();
                  ps->event->create_locations[i * 2 + 1] = cmp;
                  if (events_parse_verbose != 0) {
                    _printf(str_found_location_d_d_d_2,loc_count,
                            ps->event->create_locations[i * 2],ps->event->create_locations[i * 2 + 1])
                    ;
                  }
                  loc_count++;
                }
              }
            }
          }
        }
      }
    }
  }
  ps->event->create_location_count = loc_count;
  ps->event->actions = (int)ps->event->actions | 4;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action CHANGEMONEY (its
   Receiver= and Amount= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_change_money(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_changemoney_found_looking_for_recei);
  }
  while ((ps->have_receiver == 0 || (ps->have_amount == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_14);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_changemoney_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_receiver_2,9);
    if (cmp == 0) {
      arg += 9;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->money_receiver_name = (char *)cmp;
        if (ps->event->money_receiver_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->money_receiver_name),arg);
        ps->event->money_receiver_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_king_id_d_2,arg,ps->parsed_civ);
        }
        ps->have_receiver = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_amount,7);
      if ((cmp == 0) &&
         ((((arg = arg + 7, '/' < *arg && (*arg < ':')) ||
           (*arg == '-')) || (*arg == '+')))) {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->money_amount = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_amount_s_integer_d,arg,ps->event->money_amount);
        }
        ps->have_amount = 1;
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 8;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action MAKEAGGRESSION
   (its Who= and Whom= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_make_aggression(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_makeaggression_found_looking_for_wh);
  }
  while ((ps->have_who == 0 || (ps->have_whom == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_15);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_makeaggression_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,((i8 *)&events_who_prefix_str),4);
    if (cmp == 0) {
      arg += 4;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_who_s_but_s_is_not_valid,arg,arg);
        }
      }
      else {
        len = _strlen(arg);
        cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
        ps->event->aggression_who_name = (char *)cmp;
        if (ps->event->aggression_who_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->aggression_who_name),arg);
        ps->event->aggression_who_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_who_s_king_id_d,arg,ps->parsed_civ);
        }
        ps->have_who = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_whom,5);
      if (cmp == 0) {
        arg += 5;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_whom_s_but_s_is_not_valid,arg,arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
          ps->event->aggression_whom_name = (char *)cmp;
          if (ps->event->aggression_whom_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->aggression_whom_name),arg);
          ps->event->aggression_whom_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_whom_s_king_id_d,arg,ps->parsed_civ);
          }
          ps->have_whom = 1;
        }
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x20;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action
   DESTROYACIVILIZATION (its Whom= line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_destroy_civ(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_destroyacivilization_found_looking);
  }
  while (ps->have_destroy_whom == 0) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_16);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_destroyacivilization_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_whom_2,5);
    if (cmp == 0) {
      arg += 5;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_whom_s_but_s_is_not_valid_2,arg,arg);
        }
      }
      else {
        ps->event->destroy_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_whom_s_king_id_d_2,arg,ps->parsed_civ);
        }
        ps->have_destroy_whom = 1;
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x400;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action GIVETECHNOLOGY
   (its Technology= and Receiver= lines). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_give_tech(EventsParseState *ps)
{
  int cmp;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_givetechnology_found_looking_for_wh);
  }
  while ((ps->have_tech_id == 0 || (ps->have_tech_receiver == 0))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_17);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_givetechnology_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_receiver_3,9);
    if (cmp == 0) {
      arg += 9;
      ps->parsed_civ = events_parse_civ((const char *)arg);
      if (ps->parsed_civ == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_but_s_is_not_valid_3,arg,arg)
          ;
        }
      }
      else {
        ps->event->give_tech_civ = ps->parsed_civ;
        if (events_parse_verbose != 0) {
          _printf(str_found_receiver_s_king_id_d_3,arg,ps->parsed_civ)
          ;
        }
        ps->have_tech_receiver = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_technology_2,0xb);
      if (((cmp == 0) && (arg = arg + 0xb, '/' < *arg)) &&
         (*arg < ':')) {
        cmp = str_parse_int_prefixed((const char *)arg);
        ps->event->give_tech = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_technology_s_integer_d,arg,
                  ps->event->give_tech);
        }
        ps->have_tech_id = 1;
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 0x800;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action MOVEUNIT (its
   Unit=, Owner=, MapRect, MoveTo and NumberToMove= lines). Returns 0 when
   the file is bad. */
int ScenarioEvents::parse_action_move_unit(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;
  int str_buf;
  iptr i;
  int loc_count;

  if (events_parse_verbose != 0) {
    _printf(str_then_moveunit_found_looking_for_unit_own);
  }
  loc_count = 0;
  while ((((ps->have_unit == 0 || (ps->have_owner == 0)) || (loc_count != 4)) ||
         ((ps->have_moveto == 0 || (ps->have_move_count == 0))))) {
    arg = text_read_line(1);
    if (arg == (char *)0x0) {
      if (events_parse_verbose != 0) {
        _printf(str_got_early_end_of_file_18);
      }
      return 0;
    }
    if (*arg == '@') {
      if (events_parse_verbose != 0) {
        _printf(str_illegal_moveunit_statement);
      }
      return 0;
    }
    cmp = __strnicmp(arg,str_unit_3,5);
    if (cmp == 0) {
      arg += 5;
      cmp = events_parse_unit_type((const char *)arg);
      if (cmp == -1) {
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_but_s_is_not_valid_3,arg,
                  arg);
        }
      }
      else {
        len = _strlen(arg);
        str_buf = memblock_alloc((MemBlock *)events_string_pool,len + 1)
        ;
        ps->event->move_unit_name = (char *)str_buf;
        if (ps->event->move_unit_name == 0) return 0;
        crt_strcpy((char *)((void *)ps->event->move_unit_name),arg);
        ps->event->move_unit_type = cmp;
        if (events_parse_verbose != 0) {
          _printf(str_found_unit_s_unit_id_d_2,arg,cmp);
        }
        ps->have_unit = 1;
      }
    }
    else {
      cmp = __strnicmp(arg,str_owner_2,6);
      if (cmp == 0) {
        arg += 6;
        ps->parsed_civ = events_parse_civ((const char *)arg);
        if (ps->parsed_civ == -1) {
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_but_s_is_not_valid_2,arg,
                    arg);
          }
        }
        else {
          len = _strlen(arg);
          cmp = memblock_alloc((MemBlock *)events_string_pool,
                                     len + 1);
          ps->event->move_owner_name = (char *)cmp;
          if (ps->event->move_owner_name == 0) return 0;
          crt_strcpy((char *)((void *)ps->event->move_owner_name),arg);
          ps->event->move_owner_civ = ps->parsed_civ;
          if (events_parse_verbose != 0) {
            _printf(str_found_owner_s_king_id_d_2,arg,ps->parsed_civ
                   );
          }
          ps->have_owner = 1;
        }
      }
      else {
        cmp = __strcmpi(arg,str_maprect_2);
        if (cmp == 0) {
          if (events_parse_verbose != 0) {
            _printf(str_found_maprect_2);
          }
          cmp = (int)text_read_line(1);
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_got_early_end_of_file_19);
            }
            return 0;
          }
          for (i = 0; i < 4; i++) {
            cmp = text_next_int();
            ps->event->move_rect[i * 2] = cmp;
            cmp = text_next_int();
            ps->event->move_rect[i * 2 + 1] = cmp;
            if (events_parse_verbose != 0) {
              _printf(str_found_location_d_d_d_3,loc_count,
                      ps->event->move_rect[i * 2],
                      ps->event->move_rect[i * 2 + 1]);
            }
            loc_count++;
          }
        }
        else {
          cmp = __strcmpi(arg,str_moveto);
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_found_moveto);
            }
            arg = text_read_line(1);
            if (arg == (char *)0x0) {
              if (events_parse_verbose != 0) {
                _printf(str_got_early_end_of_file_20);
              }
              return 0;
            }
            if (((('/' < *arg) && (*arg < ':')) || (*arg == '-'))
               || (*arg == '+')) {
              cmp = text_next_int();
              ps->event->move_to_x = cmp;
              cmp = text_next_int();
              ps->event->move_to_y = cmp;
              if (events_parse_verbose != 0) {
                _printf(str_found_moveto_d_d,ps->event->move_to_x,
                        ps->event->move_to_y);
              }
              ps->have_moveto = 1;
            }
          }
          else {
            cmp = __strnicmp(arg,str_numbertomove,0xd);
            if (cmp == 0) {
              arg += 0xd;
              if ((*arg < '0') || ('9' < *arg)) {
                cmp = __strcmpi(arg,((i8 *)&events_numbertomove_all_str));
                if (cmp == 0) {
                  ps->event->move_count = -2;
                  if (events_parse_verbose != 0) {
                    _printf(str_found_numbertomove_s,arg);
                  }
                  ps->have_move_count = 1;
                }
                else if (events_parse_verbose != 0) {
                  _printf(str_found_numbertomove_s_but_s_is_not_valid,
                          arg,arg);
                }
              }
              else {
                cmp = str_parse_int_prefixed((const char *)arg);
                ps->event->move_count = cmp;
                if (events_parse_verbose != 0) {
                  _printf(str_found_numbertomove_s_integer_d,
                          arg,ps->event->move_count);
                }
                ps->have_move_count = 1;
              }
            }
          }
        }
      }
    }
  }
  ps->event->actions = (int)ps->event->actions | 2;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action PLAYCDTRACK (its
   track line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_play_cd(EventsParseState *ps)
{
  int cmp;

  if (events_parse_verbose != 0) {
    _printf(str_then_playcdtrack_found_looking_for_track);
  }
  cmp = (int)text_read_line(1);
  if (cmp == 0) {
    if (events_parse_verbose != 0) {
      _printf(str_got_early_end_of_file_21);
    }
    return 0;
  }
  cmp = text_next_int();
  ps->event->cd_track = cmp;
  if (0 < ps->event->cd_track) {
    if (events_parse_verbose != 0) {
      _printf(str_found_cd_track_d,ps->event->cd_track);
    }
    ps->event->actions = (int)ps->event->actions | 0x80;
  }
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): the action PLAYWAVEFILE
   (its file line). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action_play_wave(EventsParseState *ps)
{
  int cmp;
  size_t len;
  char *arg;

  if (events_parse_verbose != 0) {
    _printf(str_then_playwavefile_found_looking_for_file);
  }
  arg = text_read_line(1);
  if (arg == (char *)0x0) {
    if (events_parse_verbose != 0) {
      _printf(str_got_early_end_of_file_22);
    }
    return 0;
  }
  len = _strlen(arg);
  cmp = memblock_alloc((MemBlock *)events_string_pool,len + 1);
  ps->event->wave_file = (char *)cmp;
  if (ps->event->wave_file == 0) return 0;
  crt_strcpy((char *)((void *)ps->event->wave_file),arg);
  if (events_parse_verbose != 0) {
    _printf(str_found_wave_file_name_s,(int)ps->event->wave_file);
  }
  ps->event->actions = (int)ps->event->actions | 0x10;
  return 1;
}

/* Part of ScenarioEvents::parse_file (004fc516): a line of an action after
   @THEN (state 3). Returns 0 when the file is bad. */
int ScenarioEvents::parse_action(EventsParseState *ps)
{
  int cmp;

  cmp = __strcmpi(ps->line,((i8 *)&events_kw_text_str));
  if (cmp == 0) {
    if (parse_action_text(ps) == 0) return 0;
  }
  else {
    cmp = __strcmpi(ps->line,str_changeterrain);
    if (cmp == 0) {
      if (parse_action_change_terrain(ps) == 0) return 0;
    }
    else {
      cmp = __strcmpi(ps->line,str_createunit);
      if (cmp == 0) {
        if (parse_action_create_unit(ps) == 0) return 0;
      }
      else {
        cmp = __strcmpi(ps->line,str_changemoney);
        if (cmp == 0) {
          if (parse_action_change_money(ps) == 0) return 0;
        }
        else {
          cmp = __strcmpi(ps->line,str_justonce);
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_then_justonce_found_not_looking_for_anyt);
            }
            ps->event->actions = (int)ps->event->actions | 0x40;
          }
          else {
            cmp = __strcmpi(ps->line,str_dontplaywonders);
            if (cmp == 0) {
              if (events_parse_verbose != 0) {
                _printf(str_then_dontplaywonders_found_not_looking_f);
              }
              ps->event->actions = (int)ps->event->actions | 0x100;
            }
            else {
              cmp = __strcmpi(ps->line,str_makeaggression);
              if (cmp == 0) {
                if (parse_action_make_aggression(ps) == 0) return 0;
              }
              else {
                cmp = __strcmpi(ps->line,str_destroyacivilization);
                if (cmp == 0) {
                  if (parse_action_destroy_civ(ps) == 0) return 0;
                }
                else {
                  cmp = __strcmpi(ps->line,str_givetechnology);
                  if (cmp == 0) {
                    if (parse_action_give_tech(ps) == 0) return 0;
                  }
                  else {
                    cmp = __strcmpi(ps->line,str_moveunit);
                    if (cmp == 0) {
                      if (parse_action_move_unit(ps) == 0) return 0;
                    }
                    else {
                      cmp = __strcmpi(ps->line,str_playcdtrack);
                      if (cmp == 0) {
                        if (parse_action_play_cd(ps) == 0) return 0;
                      }
                      else {
                        cmp = __strcmpi(ps->line,str_playwavefile);
                        if (cmp == 0) {
                          if (parse_action_play_wave(ps) == 0) return 0;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 1;
}

/* 004fc516 auto */
int ScenarioEvents::parse_file(const char *filename,const char *section)

{
  int cmp;
  uint state;
  int failed;
  EventsParseState ps = {};  /* BF-030: no event before the first @IF */
  
  failed = 1;
  state = 1;
  events_parse_verbose = 0;
  cmp = text_open_section(filename,section);
  if (cmp == 0) {
    do {
      ps.line = text_read_line(1);
      if (ps.line == (char *)0x0) {
        state = 10;
      }
      else if (*ps.line == '@') {
        ps.line++;
        cmp = __strcmpi(ps.line,((i8 *)&events_kw_if_str));
        if (cmp == 0) {
          if (events_parse_verbose != 0) {
            _printf(str_if_found_creating_new_event_structure);
          }
          ps.event = alloc();
          if (ps.event == (ScenarioEvent *)0x0) {
            if (events_parse_verbose != 0) {
              _printf(str_failed_to_create_new_event_structure);
            }
            break;
          }
          if (events_parse_verbose != 0) {
            _printf(str_new_event_structure_created);
          }
          ps.have_city = 0;
          ps.have_defender = 0;
          ps.have_attacker = 0;
          ps.have_unit_killed = 0;
          ps.have_terrain = 0;
          ps.have_moveto = 0;
          ps.have_unit = 0;
          ps.have_owner = 0;
          ps.have_move_count = 0;
          ps.have_amount = 0;
          ps.have_receiver = 0;
          ps.have_destroy_whom = 0;
          ps.have_whom = 0;
          ps.have_who = 0;
          ps.have_tech = 0;
          ps.have_listener_type = 0;
          ps.have_talker_type = 0;
          ps.have_veteran = 0;
          ps.have_homecity = 0;
          ps.have_tech_receiver = 0;
          ps.have_tech_id = 0;
          state = 2;
        }
        else if (state != 6) {
          cmp = __strcmpi(ps.line,((i8 *)&events_kw_then_str));
          if (cmp == 0) {
            if (events_parse_verbose != 0) {
              _printf(str_then_found);
            }
            state = 3;
          }
          else {
            cmp = __strcmpi(ps.line,str_endif);
            if (cmp == 0) {
              if (events_parse_verbose != 0) {
                _printf(str_endif_found);
              }
              state = 5;
            }
            else {
              cmp = __strcmpi(ps.line,str_debug);
              if (cmp == 0) {
                events_parse_verbose = 1;
                _printf(str_debugging_filename_s,filename);
              }
              else {
                cmp = __strcmpi(ps.line,str_endevents);
                if (cmp == 0) {
                  if (events_parse_verbose != 0) {
                    _printf(str_endevents_found);
                  }
                  state = 10;
                }
              }
            }
          }
        }
      }
      else if (state == 2) {
        if (parse_trigger(&ps) == 0) break;
      }
      else if (state == 3) {
        /* BF-030 (Peter Johnson's ledger): an action after an @THEN with no
           @IF before it had no event to go to (the original wrote through
           an unset pointer): a bad events file */
        if ((ps.event == (ScenarioEvent *)0x0) || (parse_action(&ps) == 0)) break;
      }
    } while (state < 10);
    if (state == 10) {
      failed = 0;  /* read to the end (an error leaves the loop earlier) */
    }
  }
  if (failed != 0) {
    __chdir(((i8 *)&game_dirs.home));
    popup_game((const char *)&str_badeventsfile);
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  }
  return failed;
}

/* 00548e23 auto */
/* WARNING: Type propagation algorithm not settling */

void __cdecl events_write_actions(FILE *fp,ScenarioEvent *event)

{
  int rc;
  uint action;
  i32 civ_name;
  uint action_bit;
  int i;
  char line[0x120];  /* BF-048 (Peter Johnson's ledger): the original's 80,
                        less than a name read from a line of the file */
  
  rc = _fputs(str_then,fp);
  if (((rc == -1) || (action_bit = 1, event == 0)) || (event->actions == 0)) {
    return;
  }
  do {
    action = event->actions & action_bit;
    if (action < 9) {
      if (action == 8) {
        rc = _fputs(str_changemoney_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_receiver_s,(i32)event->money_receiver_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_amount_d,event->money_amount);
        rc = _fputs(line,fp);
      }
      else if (action == 1) {
        rc = _fputs(str_text,fp);
        if (rc == -1) {
          return;
        }
        for (i = 0; (i < 0x14 && (event->text[i] != 0));
            i = i + 1) {
          rc = _fputs(event->text[i],fp);
          if (rc == -1) {
            return;
          }
          rc = _fputs(((i8 *)&events_text_line_newline_str),fp);
          if (rc == -1) {
            return;
          }
        }
        rc = _fputs(str_endtext_2,fp);
      }
      else if (action == 2) {
        rc = _fputs(str_moveunit_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_unit_s,(i32)event->move_unit_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_owner_s,(i32)event->move_owner_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        rc = _fputs(str_maprect_3,fp);
        if (rc == -1) {
          return;
        }
        for (i = 0; i < 4; i++) {
          _sprintf(line,str_d_d,event->move_rect[i * 2],
                   event->move_rect[i * 2 + 1]);
          if (i < 3) {
            crt_strcat(line,((i8 *)&events_moveunit_rect_sep_str));
          }
          rc = _fputs(line,fp);
          if (rc == -1) {
            return;
          }
        }
        rc = _fputs(str_moveto_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_d_d_2,event->move_to_x,
                 event->move_to_y);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        if (event->move_count == -2) {
          _sprintf(line,str_numbertomove_all);
        }
        else {
          _sprintf(line,str_numbertomove_d,event->move_count);
        }
        rc = _fputs(line,fp);
      }
      else {
        if (action != 4) continue;
        rc = _fputs(str_createunit_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_unit_s_2,(i32)event->create_unit_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_owner_s_2,(i32)event->create_owner_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_veteran_s,
                 ((i8 *)&events_veteran_yes_no_strs) + ((event->create_veteran != 0) - 1 & 4));
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_homecity_s,(i32)event->create_homecity);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        rc = _fputs(str_locations_2,fp);
        if (rc == -1) {
          return;
        }
        for (i = 0; i < event->create_location_count; i++) {
          _sprintf(line,str_d_d_3,event->create_locations[i * 2],
                   event->create_locations[i * 2 + 1]);
          rc = _fputs(line,fp);
          if (rc == -1) {
            return;
          }
        }
        rc = _fputs(str_endlocations_2,fp);
      }
    }
    else if (0x20 < action) {
      if (action < 0x81) {
        if (action == 0x80) {
          rc = _fputs(str_playcdtrack_2,fp);
          if (rc == -1) {
            return;
          }
          _sprintf(line,((i8 *)&events_cd_track_fmt),event->cd_track);
          rc = _fputs(line,fp);
        }
        else {
          if (action != 0x40) continue;
          rc = _fputs(str_justonce_2,fp);
        }
      }
      else if (action < 0x201) {
        if (action == 0x200) {
          rc = _fputs(str_changeterrain_2,fp);
          if (rc == -1) {
            return;
          }
          _sprintf(line,str_terraintype_d,event->terrain_type);
          rc = _fputs(line,fp);
          if (rc == -1) {
            return;
          }
          rc = _fputs(str_maprect_4,fp);
          if (rc == -1) {
            return;
          }
          for (i = 0; i < 4; i++) {
            _sprintf(line,str_d_d_4,event->terrain_rect[i * 2],
                     event->terrain_rect[i * 2 + 1]);
            if (i < 3) {
              crt_strcat(line,((i8 *)&events_terrain_rect_sep_str));
            }
            rc = _fputs(line,fp);
            if (rc == -1) {
              return;
            }
          }
          rc = _fputs(((i8 *)&events_terrain_rect_end_str),fp);
        }
        else {
          if (action != 0x100) continue;
          rc = _fputs(str_dontplaywonders_2,fp);
        }
      }
      else if (action == 0x400) {
        rc = _fputs(str_destroyacivilization_2,fp);
        if (rc == -1) {
          return;
        }
        if (event->destroy_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->destroy_civ == -3) {
              _sprintf(line,str_whom_triggerattacker);
            }
            else {
              _sprintf(line,str_whom_triggerdefender);
            }
          }
          else {
            _sprintf(line,str_whom_triggerreceiver);
          }
        }
        else {
          civ_name = (i32)(civ_name_plural(event->destroy_civ));
          _sprintf(line,str_whom_s_2,civ_name);
        }
        rc = _fputs(line,fp);
      }
      else if (action == 0x800) {
        rc = _fputs(str_givetechnology_2,fp);
        if (rc == -1) {
          return;
        }
        if (event->give_tech_civ < 0) {
          if ((event->trigger_bytes[1] & 1) == 0) {
            if (event->give_tech_civ == -3) {
              _sprintf(line,str_receiver_triggerattacker);
            }
            else {
              _sprintf(line,str_receiver_triggerdefender);
            }
          }
          else {
            _sprintf(line,str_receiver_triggerreceiver);
          }
        }
        else {
          civ_name = (i32)(civ_name_plural(event->give_tech_civ));
          _sprintf(line,str_receiver_s_2,civ_name);
        }
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_technology_d,event->give_tech);
        rc = _fputs(line,fp);
      }
      else {
        if (action != 0x2000) continue;
        rc = _fputs(str_hastriggered,fp);
      }
    }
    else if (action == 0x20) {
      rc = _fputs(str_makeaggression_2,fp);
      if (rc == -1) {
        return;
      }
      _sprintf(line,str_who_s,(i32)event->aggression_who_name);
      rc = _fputs(line,fp);
      if (rc == -1) {
        return;
      }
      _sprintf(line,str_whom_s,(i32)event->aggression_whom_name);
      rc = _fputs(line,fp);
    }
    else if (action == 0x10) {
      rc = _fputs(str_playwavefile_2,fp);
      if (rc == -1) {
        return;
      }
      _sprintf(line,((i8 *)&events_wave_file_fmt),(i32)event->wave_file);
      rc = _fputs(line,fp);
    }
    else {
      continue;
    }
    if (rc == -1) {
      return;
    }
  } while ((action_bit = action_bit << 1) != 0x8000);
}

/* 00549aee auto */
void __cdecl events_write_list(FILE *fp)

{
  uint trigger;
  int rc;
  ScenarioEvent *event;
  char line[256];
  
  event = unit_types_block.events_list_head;
  do {
    if (event == (ScenarioEvent *)0x0) {
      _fputs(((i8 *)&events_list_end_str),fp);
      return;
    }
    rc = _fputs(str_if,fp);
    if (rc == -1) {
      return;
    }
    trigger = (uint)event->trigger;
    if (trigger < 9) {
      if (trigger == 8) {
        rc = _fputs(str_turninterval_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_interval_d,(uint)event->turn);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
      else if (trigger == 1) {
        rc = _fputs(str_unitkilled_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_unit_s_3,(uint)event->unit_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_attacker_s,(uint)event->who_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_defender_s,(uint)event->whom_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
      else if (trigger == 2) {
        rc = _fputs(str_citytaken_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_city_s,(uint)event->city_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_attacker_s_2,(uint)event->who_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_defender_s_2,(uint)event->whom_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
      else if (trigger == 4) {
        rc = _fputs(str_turn_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_turn_d,(uint)event->turn);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
    }
    else {
      switch(trigger) {
      case 0x10:
        rc = _fputs(str_negotiation_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_talker_s,(uint)event->who_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_talkertype_2);
        if ((uint)event->talker_type == 1) {
          crt_strcat(line,str_human_3);
        }
        if ((uint)event->talker_type == 2) {
          crt_strcat(line,str_computer_3);
        }
        if ((uint)event->talker_type == 4) {
          crt_strcat(line,str_humanorcomputer_3);
        }
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_listener_s,(uint)event->whom_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_listenertype_2);
        if ((uint)event->listener_type == 1) {
          crt_strcat(line,str_human_4);
        }
        if ((uint)event->listener_type == 2) {
          crt_strcat(line,str_computer_4);
        }
        if ((uint)event->listener_type == 4) {
          crt_strcat(line,str_humanorcomputer_4);
        }
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x20:
        rc = _fputs(str_scenarioloaded_5,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x40:
        rc = _fputs(str_randomturn_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_denominator_d,(uint)event->denominator);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x80:
        rc = _fputs(str_noschism_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_defender_s_3,(uint)event->whom_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
        break;
      case 0x100:
        rc = _fputs(str_receivedtechnology_2,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_technology_d_2,(uint)event->technology);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        _sprintf(line,str_receiver_s_3,(uint)event->whom_name);
        rc = _fputs(line,fp);
        if (rc == -1) {
          return;
        }
        events_write_actions(fp,event);
      }
    }
    rc = _fputs(str_endif_2,fp);
    if (rc == -1) {
      return;
    }
    event = event->next;
  } while( true );
}

/* 0054a4c4 auto */
i32 __cdecl events_write_file(void)

{
  int rc;
  char *got;
  i32 ok;
  FILE *src_fp;
  FILE *dst_fp;
  char saved_cwd[260];
  i8 filename[16];
  char s[260];
  
  src_fp = (FILE *)0x0;
  dst_fp = (FILE *)0x0;
  crt_strcpy(filename,str_events_3);
  crt_strcat(filename,((i8 *)&lang_file_ext));
  ok = 0;
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  ON_SCOPE_EXIT(
    if (src_fp != (FILE *)0x0) {
      _fclose(src_fp);
    }
    if (dst_fp != (FILE *)0x0) {
      _fclose(dst_fp);
    }
    __chdir(saved_cwd));
  rc = file_exists((const char *)&str_events_bak);
  if (((rc == 0) || (rc = crt_remove(str_events_bak_2), rc == 0)) &&
     ((rc = file_exists((const char *)filename), rc == 0 ||
      (rc = crt_rename(filename,str_events_bak_3), rc == 0)))) {
    src_fp = file_open_path(str_events_bak_4,str_rt_7);
    dst_fp = file_open_path((const char *)filename,str_wt_5);
    if (dst_fp != (FILE *)0x0) {
      if (src_fp == (FILE *)0x0) {
        rc = _fputs(str_beginevents_3,dst_fp);
        if (rc == -1) return ok;
      }
      else {
        do {
          got = _fgets(s,0x100,src_fp);
          if ((got == (char *)0x0) || (rc = _fputs(s,dst_fp), rc == -1))
          return ok;
          str_strip_newline(s);
          str_trim((char *)s);
          rc = __strcmpi(s,str_beginevents_2);
        } while (rc != 0);
      }
      events_write_list(dst_fp);
      if (src_fp == (FILE *)0x0) {
        rc = _fputs(str_endevents_4,dst_fp);
        if (rc != -1) {
          ok = 1;
        }
      }
      else {
        do {
          got = _fgets(s,0x100,src_fp);
          if (got == (char *)0x0) return ok;
          str_strip_newline(s);
          str_trim((char *)s);
          rc = __strcmpi(s,str_endevents_2);
        } while (rc != 0);
        rc = _fputs(str_endevents_3,dst_fp);
        while (rc != -1) {
          got = _fgets(s,0x100,src_fp);
          if (got == (char *)0x0) {
            ok = 1;
            break;
          }
          rc = _fputs(s,dst_fp);
        }
      }
    }
  }
  return ok;
}

/* 00551cd0 auto */
ScenarioEvents *ScenarioEvents::events_delete(uint flags)

{
  this->~ScenarioEvents();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}
