/* The game, text: game text files, strings, resources and files: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"
#include "lang.h"

/* 0040bbb0 auto */
void __cdecl text_clear(void)

{
  str_clear((char *)&textbuf.buf);
  return;
}

/* 0040bbe0 auto */
void __cdecl text_append(const char *str)

{
  crt_strcat(((i8 *)&textbuf.buf),(const char *)((void *)str));
  return;
}

/* 0040bc10 auto */
void __cdecl text_append_label(i32 label_id)

{
  str_append_label((iptr)&textbuf.buf,label_id);
  return;
}

/* 0040fe10 auto */
void __cdecl text_append_space(void)

{
  str_append_space((iptr)&textbuf.buf);
  return;
}

/* 0040fe40 auto */
void __cdecl text_append_colon(void)

{
  str_append_colon((char *)&textbuf.buf);
  return;
}

/* 0040fe70 auto */
void __cdecl text_append_percent(void)

{
  str_append_percent((iptr)&textbuf.buf);
  return;
}

/* 0040fea0 auto */
void __cdecl text_append_lparen(void)

{
  str_append_lparen((iptr)&textbuf.buf);
  return;
}

/* 0040fed0 auto */
void __cdecl text_append_rparen(void)

{
  str_append_rparen((iptr)&textbuf.buf);
  return;
}

/* 0040ff00 auto */
void __cdecl text_append_string_id(i32 string_id)

{
  str_append_string_id((iptr)&textbuf.buf,string_id);
  return;
}

/* 0040ff30 auto */
void __cdecl text_append_int(iptr value)

{
  str_append_int((iptr)&textbuf.buf,value);
  return;
}

/* 0040ff60 auto */
i32 __cdecl text_set_string_arg(iptr index,const char *str)

{
  i32 result;
  
  result = (i32)crt_strcpy(((i8 *)&text_args.strings) + index * 0x104,(const char *)((void *)str));
  return result;
}

/* 00414d70 auto */
void __cdecl text_append_str(const char *str)

{
  str_concat((iptr)&textbuf.buf,(iptr)str);
  return;
}

/* 00415040 auto */
iptr __cdecl file_build_open_path(iptr dest,const char *name2)

{
  if (file_path_mode == '\0') {
    crt_strcpy((char *)((void *)dest),(const char *)((void *)name2));
  }
  else {
    path_join_upper((char *)dest,(iptr)&open_path_prefix,name2);
  }
  return dest;
}

/* 0041508c auto */
FILE * __cdecl file_open_path(const char *name2,iptr mode)

{
  FILE *file;
  char path[0x208];  /* BF-058, BF-296: the original's 136 bytes; a saved
                        game's folder outside the game's is a longer path */
  size_t len;
  
  file_build_open_path((iptr)path,name2);
  /* the original copied the whole path into its 80 bytes; the error message
     shows the first 79 characters of a longer one */
  len = _strlen(path);
  if (sizeof error_state.last_open_path - 1 < len) {
    len = sizeof error_state.last_open_path - 1;
  }
  crt_memcpy(((i8 *)&error_state.last_open_path),path,len);
  error_state.last_open_path[len] = '\0';
  file = _fopen(path,(char *)mode);
  (*(uint *)&error_state.last_open_failed) = (uint)(file == (FILE *)0x0);
  return file;
}

/* 00415133 auto */
uint __cdecl file_exists(const char *name2)

{
  FILE *_File;
  i32 exists;
  
  _File = file_open_path(name2,(iptr)&str_rb);
  if (_File != (FILE *)0x0) {
    _fclose(_File);
  }
  exists = (uint)(_File != (FILE *)0x0);
  return exists;
}

/* 004190a0 auto */
void __cdecl text_append_spaces(i32 count)

{
  str_append_spaces((iptr)&textbuf.buf,count);
  return;
}

/* 0041e7e3 auto */
void __cdecl labels_reload(void)

{
  int value;
  
  value = text_open_section((const char *)str_ptr_labels,(const char *)&str_stringheap_2);
  if (value == 0) {
    text_read_line(1);
    value = text_next_int();
    if (strpool_capacity < value) {
      strpool_capacity = value;
    }
  }
  strpool_init(strpool_capacity);
  strpool_add((const char *)&str_nil_2);
  labels_free();
  labels_load();
  dialog_load_button_labels();
  return;
}

/* 00421bf0 auto */
/* Library Function - Multiple Matches With Same Base Name
    protected: virtual unsigned char * __thiscall CHtmlStream::Realloc(unsigned char *,unsigned
   long)
    protected: virtual unsigned char * __thiscall CMemFile::Realloc(unsigned char *,unsigned long)
   
   Library: Visual Studio 1998 Debug */

i32 FileObject::create(const char *path)

{
  i32 result;
  
  result = file_obj_create_path(path,this);
  return result;
}

/* 00421c30 auto */
i32 FileObject::map_close()

{
  i32 result;
  
  result = file_obj_close_handle(this);
  return result;
}

/* 00421c60 auto */
void FileObject::write(iptr data,i32 size)

{
  file_obj_write_bytes(this,(char *)data,size);
  return;
}

/* 00421d30 auto */
void __cdecl text_append_comma(void)

{
  str_append_comma((iptr)&textbuf.buf);
  return;
}

/* 00421d60 auto */
i32 __cdecl text_set_string_arg_b(int index,const char *text)

{
  i32 result;
  
  result = (i32)crt_strcpy(((i8 *)&text_args.strings) + index * 0x104,(const char *)((void *)text));
  return result;
}

/* 00421da0 auto */
i32 __cdecl text_set_number_arg(int index,i32 value)

{
  text_args.numbers[index] = value;
  return value;
}

/* 00421f10 auto */
void __cdecl text_append_year(i32 year)

{
  str_append_year((iptr)&textbuf.buf,year);
  return;
}

/* (port) text_expand_vars within a destination of size bytes: BF-063, 064,
   199, 200 (Peter Johnson's ledger): the original expanded without a bound
   into its callers' fixed buffers (a line of a modified GAME.TXT, or string
   arguments from long names); the text is cut at the buffer's end */
void __cdecl text_expand_vars_max(iptr src,iptr dest,size_t size)

{
  char *percent_pos;
  int arg_index;
  size_t len;
  char *tag;
  uint pad_i;
  char num_buf[40];
  
  if (size == 0) {
    return;
  }
  *(i8 *)dest = 0;
  do {
    percent_pos = _strchr((char *)src,0x25);
    if (percent_pos != (char *)0x0) {
      *percent_pos = '\0';
    }
    if (*(char *)src != '\0') {
      str_append_max((char *)((void *)dest),(const char *)((void *)src),size);
    }
    tag = percent_pos;
    if (percent_pos != (char *)0x0) {
      tag = percent_pos + 1;
      arg_index = __strnicmp(tag,str_string,6);
      if (arg_index == 0) {
        arg_index = _atoi(percent_pos + 7);
        /* BF-197 (Peter Johnson's ledger): a string argument text_args has
           (24 of 0x104 bytes after the numbers; the game's text names 0..9);
           another is left out */
        if ((0 <= arg_index) &&
           (arg_index < (int)((sizeof(TextArgs) - offsetof(TextArgs,strings)) / 0x104))) {
          str_append_max((char *)((void *)dest),((i8 *)&text_args.strings) + arg_index * 0x104,size);
        }
        src = (iptr)(percent_pos + 8);
        if (9 < arg_index) {
          src = (iptr)(percent_pos + 9);
        }
      }
      else {
        arg_index = _strncmp(tag,str_number,6);
        if (arg_index == 0) {
          arg_index = _atoi(percent_pos + 7);
          if ((0 <= arg_index) && (arg_index < 6)) {  /* BF-197 */
            __ltoa(text_args.numbers[arg_index],num_buf,10);
            str_append_max((char *)((void *)dest),(const char *)((void *)num_buf),size);
          }
          src = (iptr)(percent_pos + 8);
        }
        else {
          arg_index = _strncmp(tag,((i8 *)&text_var_hex_tag_str),3);
          if (arg_index == 0) {
            arg_index = _atoi(percent_pos + 4);
            if ((0 <= arg_index) && (arg_index < 6)) {  /* BF-197 */
              __ltoa(text_args.numbers[arg_index],num_buf,0x10);
              /* BF-197: zeros up to 4 digits; the original's unsigned 4 - len
                 padded a longer number (above 0xffff) without end */
              for (pad_i = 0; len = _strlen(num_buf), pad_i + len < 4;
                  pad_i = pad_i + 1) {
                str_append_max((char *)((void *)dest),((i8 *)&text_var_hex_pad_str),size);
              }
              str_append_max((char *)((void *)dest),(const char *)((void *)num_buf),size);
            }
            src = (iptr)(percent_pos + 5);
          }
          else {
            src = (iptr)tag;
            if (*tag == '%') {
              str_append_max((char *)((void *)dest),((i8 *)&text_var_percent_str),size);
              src = (iptr)(percent_pos + 2);
            }
          }
        }
      }
    }
  } while (tag != (char *)0x0);
  return;
}

/* 00426ff0 auto */
void __cdecl text_expand_vars(iptr src,iptr dest)

{
  text_expand_vars_max(src,dest,(size_t)-1);
  return;
}

/* 004271e8 auto */
i32 __cdecl text_set_label_arg(i32 index,i32 label_id)

{
  iptr label_text;
  i32 result;
  
  label_text = (iptr)label_string(label_id);
  result = text_set_string_arg_b(index,(const char *)label_text);
  return result;
}

/* 004289e0 auto */
void __cdecl strpool_block_global_init(void)

{
  strpool_block_static_init();
  return;
}

/* 004289f5 auto */
void __cdecl strpool_block_static_init(void)

{
  ((MemBlock *)((i8 *)&strpool_block_block.strpool_block))->construct();
  return;
}

/* 00428a0f auto */
void __cdecl strpool_init(uint pool_size)

{
  if (strpool.initialized == 0) {
    strpool.initialized = 1;
    memblock_init((MemBlock *)&strpool_block_block.strpool_block,2,pool_size);
  }
  else {
    memblock_free((MemBlock *)&strpool_block_block.strpool_block);
    memblock_init((MemBlock *)&strpool_block_block.strpool_block,2,pool_size);
  }
  strpool.count = 0;
  return;
}

/* 00428a78 auto */
void __cdecl strpool_free(void)

{
  memblock_free((MemBlock *)&strpool_block_block.strpool_block);
  return;
}

/* 00428a95 auto */
int __cdecl strpool_add(const char *text)

{
  size_t len;
  int string_id;
  
  len = _strlen((char *)text);
  if (len == 0) {
    string_id = 0;
  }
  else {
    len = _strlen((char *)text);
    minimap_window_block.strpool_last_string = memblock_alloc((MemBlock *)&strpool_block_block.strpool_block,len + 1);
    crt_strcpy((char *)((void *)minimap_window_block.strpool_last_string),(const char *)((void *)text));
    string_id = strpool.count;
    strpool.count++;
  }
  return string_id;
}

/* 00428b0c auto */
char * __cdecl label_string(int index)

{
  char *cursor;
  
  /* (port) a translated label (lang.h) */
  cursor = lang_pool_text(index);
  if (cursor != 0) {
    return cursor;
  }
  cursor = strpool_block_block.strpool_data;
  for (; index != 0; index--) {
    for (; *cursor != '\0'; cursor++) {
    }
    for (; *cursor == '\0'; cursor++) {
    }
  }
  return cursor;
}

/* 00428b68 auto */
int __cdecl strpool_add_padded(const char *text,uint min_size)

{
  int string_id;
  size_t len;
  i32 alloc_size;
  
  len = _strlen((char *)text);
  if ((int)min_size < (int)(len + 1)) {
    len = _strlen((char *)text);
    alloc_size = len + 1;
  }
  else {
    alloc_size = min_size;
  }
  (*(char * *)&minimap_window_block.strpool_last_string) = (char *)memblock_alloc((MemBlock *)&strpool_block_block.strpool_block,alloc_size);
  _memset((*(char * *)&minimap_window_block.strpool_last_string),0,alloc_size);
  _strncpy((*(char * *)&minimap_window_block.strpool_last_string),(char *)text,alloc_size);
  len = _strlen((char *)text);
  if (len == 0) {
    *(*(char * *)&minimap_window_block.strpool_last_string) = ' ';
  }
  string_id = strpool.count;
  strpool.count++;
  return string_id;
}

/* 0043c430 auto */
/* Library Function - Single Match
    public: __thiscall _Timevec::~_Timevec(void)
   
   Library: Visual Studio 1998 Debug */

void DllHandle::free()

{
  dll_unload(hmodule);
  return;
}

/* 0043c810 auto */
void __cdecl text_append_period(void)

{
  str_append_period((iptr)&textbuf.buf);
  return;
}

/* 0043c840 auto */
void __cdecl str_append(char *dest,const char *src)

{
  crt_strcat((char *)((void *)dest),(const char *)((void *)src));
  return;
}

/* (port) Copies into the game's fixed buffers bounded by the buffer's size
   (Peter Johnson's ledger: text from modified RULES, GAME, PEDIA, LABELS or
   EVENTS files longer than the buffer the original copied it into). A text
   that fits is copied as before; a longer one is cut at the buffer's end,
   as it would be cut on screen. */
char * __cdecl str_copy_max(char *dest,const char *src,size_t size)

{
  size_t len;

  if (size != 0) {
    len = _strlen(src);
    if (size - 1 < len) {
      len = size - 1;
    }
    crt_memcpy(dest,src,len);
    dest[len] = '\0';
  }
  return dest;
}

/* (port) str_append within a buffer of size bytes (see str_copy_max). */
char * __cdecl str_append_max(char *dest,const char *src,size_t size)

{
  size_t len;

  len = _strlen(dest);
  if (len < size) {
    str_copy_max(dest + len,src,size - len);
  }
  return dest;
}

/* 0043c870 auto */
void __cdecl text_append_long(i32 value)

{
  str_append_long((iptr)&textbuf.buf,value);
  return;
}

/* 0043c8a0 auto */
void __cdecl text_append_gold(i32 gold)

{
  str_append_gold((iptr)&textbuf.buf,gold);
  return;
}

/* 0043ca50 auto */
void __cdecl text_append_population(iptr civ,i32 population)

{
  str_append_population((iptr)&textbuf.buf,civ,population);
  return;
}

/* 0043ca80 auto */
void __cdecl text_append_city_name(iptr city)

{
  str_append_city_name((iptr)&textbuf.buf,city);
  return;
}

/* 0043cda6 auto */
void __cdecl str_append_population(iptr dest,iptr civ,int population)

{
  int pop;
  
  if (population < 0) {
    pop = civ_total_population(civ);
  }
  else {
    pop = population;
  }
  if (extra_rule(X_POP_LIMIT) && (99999 < pop)) {
    /* X51: past 999 million, a separator in the millions too */
    str_append_int(dest,pop / 100000);
    crt_strcat((char *)((void *)dest),((i8 *)&pop_thousands_sep_str));
    if ((pop / 100) % 1000 < 100) {
      crt_strcat((char *)((void *)dest),((i8 *)&pop_zero_pad_str));
    }
    if ((pop / 100) % 1000 < 10) {
      crt_strcat((char *)((void *)dest),((i8 *)&pop_zero_pad_str));
    }
    str_append_int(dest,(pop / 100) % 1000);
    crt_strcat((char *)((void *)dest),((i8 *)&pop_thousands_sep_str));
    pop = pop % 100;
    if (pop < 10) {
      crt_strcat((char *)((void *)dest),((i8 *)&pop_zero_pad_str));
    }
  }
  else if (99 < pop) {
    str_append_int(dest,pop / 100);
    crt_strcat((char *)((void *)dest),((i8 *)&pop_thousands_sep_str));
    pop = pop % 100;
    if (pop < 10) {
      crt_strcat((char *)((void *)dest),((i8 *)&pop_zero_pad_str));
    }
  }
  str_append_int(dest,pop);
  crt_strcat((char *)((void *)dest),(const char *)((void *)str_0_000));
  return;
}

/* 0043ce5a auto */
void __cdecl str_append_city_population(iptr dest,iptr city)

{
  int pop;
  
  pop = city_population_units(city);
  if (99 < pop) {
    str_append_int(dest,pop / 100);
    crt_strcat((char *)((void *)dest),((i8 *)&city_pop_thousands_sep_str));
    pop = pop % 100;
    if (pop < 10) {
      crt_strcat((char *)((void *)dest),((i8 *)&city_pop_zero_pad_str));
    }
  }
  str_append_int(dest,pop);
  crt_strcat((char *)((void *)dest),(const char *)((void *)str_0_000_2));
  return;
}

/* 0043f444 auto */
void __cdecl str_append_city_name(iptr dest,int city)

{
  if (city < 0) {
    str_append_label(dest,0xe);
  }
  else {
    str_concat(dest,(iptr)&cities[city].name);
  }
  return;
}

/* 004502b0 auto */
DllHandle *DllHandle::init()

{
  hmodule = 0;
  return this;
}

/* 004502e0 auto */
void DllHandle::load(const char *path)

{
  u32 module;
  
  if (hmodule != 0) {
    dll_unload(hmodule);
  }
  module = res_dll_load_library(path);
  hmodule = module;
  return;
}

/* 00450340 auto */
void DllHandle::dll_handle_free()

{
  if (hmodule != 0) {
    dll_unload(hmodule);
  }
  hmodule = 0;
  return;
}

/* 00467580 auto */
void __cdecl text_set_number_arg_v(int index,i32 value)

{
  text_args.numbers[index] = value;
  return;
}

/* 0046a740 auto */
void __cdecl labels_free(void)

{
  if (label_table.ids_handle != 0) {
    mem_unlock_handle(label_table.ids_handle);
    mem_free_handle(label_table.ids_handle);
    label_table.ids_handle = 0;
  }
  label_table.count = 0;
  return;
}

/* 0046a78d auto */
int __cdecl labels_load(void)

{
  int status;
  i32 string_id;
  i8 saved_dir[260];
  int i;
  int result;
  
  result = -1;
  status = text_open_section((const char *)&str_labels_3,(const char *)&str_labels_2);
  if (status == 0) {
    text_read_line(1);
    label_table.count = text_next_int();
    /* BF-096 (Peter Johnson's ledger): at most the 0x378 labels allocated
       (a longer count wrote past the block) */
    if (0x378 < label_table.count) {
      label_table.count = 0x378;
    }
    label_table.ids_handle = mem_handle_alloc_tracked(0xde0);
    if (label_table.ids_handle == 0) {
      error_report(0xfffffff7,4,0,0x378,4);
    }
    else {
      label_table.string_ids = (i32 *)mem_lock_handle(label_table.ids_handle);
      if (label_table.string_ids == 0) {
        error_report(0xfffffff6,4,0,label_table.ids_handle,0x378);
      }
      else {
        for (i = 0; i < label_table.count; i++) {
          string_id = text_read_line_pooled();
          label_table.string_ids[i] = string_id;
        }
        if (i < 0x378) {
          crt_strcpy(saved_dir,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
          crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
          status = text_open_section((const char *)&str_labels_5,(const char *)&str_labels_4);
          if (status == 0) {
            text_read_line(1);
            status = text_next_int();
            if (0x378 < status) {  /* BF-096 */
              status = 0x378;
            }
            for (i = 0; i < label_table.count; i++) {
              text_read_line(1);
            }
            for (; i < status; i++) {
              string_id = text_read_line_pooled();
              label_table.string_ids[i] = string_id;
            }
          }
          crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),saved_dir);
          if (i < 0x378) {
            string_id = strpool_add((const char *)&str_error_labels_txt);
            for (; i < 0x378; i++) {
              label_table.string_ids[i] = string_id;
            }
          }
        }
        result = i;
      }
    }
  }
  else {
    error_report(0xfffffff8,4,0,0,0);
  }
  text_close_file();
  if (result < 1) {
    labels_free();
  }
  else {
    /* (port) the labels by key: English recorded, a translation's put in */
    lang_add_labels(result);
  }
  return result;
}

/* 0046e8f0 auto */
int __cdecl str_parse_hex(const char *text)

{
  char ch;
  int _C;
  int is_digit;
  i32 value;
  
  value = 0;
  while (*(char *)text != '\0') {
    ch = *(char *)text;
    text++;
    _C = crt_toupper((int)ch);
    is_digit = _isdigit(_C);
    if (is_digit == 0) {
      if ((_C < 0x41) || (0x46 < _C)) {
        for (; *(char *)text != '\0'; text++) {
        }
      }
      else {
        value = value * 0x10 + _C - 0x37;
      }
    }
    else {
      value = value * 0x10 + _C - 0x30;
    }
  }
  return value;
}

/* 00485208 auto */
void __cdecl str_append_year(iptr dest,uint year)

{
  uint abs_year;
  int abs_years;
  int month_neg;
  int month_pos;
  
  if (((game.game_flags & GAME_SCENARIO) == 0) || (-1 < scenario.year_increment[0])) {
    if ((-1 < (int)year) && (game_language == 0)) {
      str_append_label(dest,1);
      str_append_space(dest);
    }
    if ((int)year < 1) {
      abs_year = ~year + 1;
    }
    else {
      abs_year = year;
    }
    str_append_int(dest,abs_year);
    if ((-1 < (int)year) && (game_language != 0)) {
      str_append_space(dest);
      str_append_label(dest,1);
    }
    if ((int)year < 0) {
      str_append_space(dest);
      str_append_label(dest,0);
    }
  }
  else {
    if ((int)year < 1) {
      if ((int)year % 0xc < 1) {
        month_neg = ~((int)year % 0xc) + 1;
      }
      else {
        month_neg = (int)year % 0xc;
      }
      text_append_label(0x1af - month_neg);
    }
    else {
      if ((int)year % 0xc < 1) {
        month_pos = ~((int)year % 0xc) + 1;
      }
      else {
        month_pos = (int)year % 0xc;
      }
      text_append_label(month_pos + 0x1a4);
    }
    text_append_space();
    if ((int)year / 0xc < 1) {
      abs_years = ~((int)year / 0xc) + 1;
    }
    else {
      abs_years = (int)year / 0xc;
    }
    str_append_int(dest,abs_years);
  }
  return;
}

/* 004929c0 auto */
/* Library Function - Multiple Matches With Same Base Name
    protected: virtual unsigned char * __thiscall CHtmlStream::Realloc(unsigned char *,unsigned
   long)
    protected: virtual unsigned char * __thiscall CMemFile::Realloc(unsigned char *,unsigned long)
   
   Library: Visual Studio 1998 Debug */

i32 FileObject::open(const char *path)

{
  i32 result;
  
  result = file_obj_open_path((char *)path,this);
  return result;
}

/* 00492a00 auto */
/* Library Function - Multiple Matches With Same Base Name
    protected: virtual unsigned char * __thiscall CHtmlStream::Realloc(unsigned char *,unsigned
   long)
    protected: virtual unsigned char * __thiscall CMemFile::Realloc(unsigned char *,unsigned long)
   
   Library: Visual Studio 1998 Debug */

i32 FileObject::seek_cur(i32 offset)

{
  i32 result;
  
  result = file_obj_seek_rel(this,offset);
  return result;
}

/* 00492a40 auto */
void FileObject::read_line(char *buf,i32 max_len)

{
  file_obj_read_line_impl(this,buf,max_len);
  return;
}

/* 00492a80 auto */
i32 FileObject::get_size()

{
  i32 size;
  
  size = file_obj_get_size_impl(this);
  return size;
}

/* 004a2020 auto */
void __cdecl text_close_file(void)

{
  if (text_file_handle != (FILE *)0x0) {
    _fclose(text_file_handle);
    text_file_handle = (FILE *)0x0;
  }
  lang_section_closed();
  return;
}

/* (port) whether the text file open is a scenario's (lang_section_opened) */
static int text_file_in_scenario;

/* 004a2055 auto */
int __cdecl text_open_file_section(const char *filename,const char *section)

{
  bool found;
  bool allow_rewind;
  size_t len;
  char *line;
  int cmp;
  char section_tag[80];
  int result;
  char name2[260];
  char saved_cwd[260];
  int section_offset;
  
  result = 1;
  allow_rewind = false;
  section_offset = -1;
  str_clear((char *)name2);
  str_clear((char *)section_tag);
  if (section != 0) {
    section_tag[0] = '@';
    section_tag[1] = 0;
    /* BF-051 (Peter Johnson's ledger): a longer section name is cut (and
       not found) */
    str_append_max((char *)((void *)section_tag),(const char *)((void *)section),sizeof section_tag);
    __strupr(section_tag);
  }
  if (filename == 0) {
    allow_rewind = true;
  }
  else {
    text_close_file();
    crt_strcpy((char *)((void *)name2),(const char *)((void *)filename));
    path_default_extension(name2,(char *)(&lang_file_ext));
    __getcwd(saved_cwd,0x104);
    __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    text_file_handle = file_open_path((const char *)name2,(iptr)&str_rt);
    /* (port) the scenario's copy of the file, which keeps its own text (lang.h) */
    text_file_in_scenario =
         text_file_handle != (FILE *)0x0 &&
         __strcmpi(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home)) != 0;
    if ((text_file_handle != (FILE *)0x0) && (len = _strlen(name2), len != 0)) {
      section_offset = text_section_index_lookup((const char *)name2,(const char *)section_tag);
    }
    __chdir(((i8 *)&game_dirs.home));
    if (((text_file_handle == (FILE *)0x0) &&
        (text_file_handle = file_open_path((const char *)name2,(iptr)&str_rt_2),
        text_file_handle != (FILE *)0x0)) && (len = _strlen(name2), len != 0)) {
      section_offset = text_section_index_lookup((const char *)name2,(const char *)section_tag);
    }
    __chdir(saved_cwd);
    if (text_file_handle == (FILE *)0x0) {
      text_close_file();
      return result;
    }
  }
  if (section == 0) {
    result = 0;
  }
  else {
    if (0 < section_offset) {
      _fseek(text_file_handle,section_offset,0);
    }
    found = false;
    while (!found) {
      if ((CRT_FEOF(text_file_handle)) != 0) {
        if (!allow_rewind) {
          text_close_file();
          return result;
        }
        allow_rewind = false;
        _rewind(text_file_handle);
      }
      line = _fgets(((i8 *)&textbuf.buf),0xff,text_file_handle);
      if (line != (char *)0x0) {
        str_strip_newline((char *)(&textbuf.buf));
        str_trim((char *)&textbuf.buf);
        cmp = __strcmpi(((i8 *)&textbuf.buf),section_tag);
        if (cmp == 0) {
          found = true;
        }
      }
    }
    len = _strlen(((i8 *)&textbuf.buf));
    (*(i8 * *)&text_parse.line_cursor) = ((i8 *)&textbuf.buf) + len;
    result = 0;
  }
  if (result != 0) {
    text_close_file();
  }
  return result;
}

/* 004a2379 auto */
int __cdecl text_open_section(const char *filename,const char *section)

{
  i8 saved_dir[260];
  int result;
  
  result = text_open_file_section(filename,section);
  if (result != 0) {
    crt_strcpy(saved_dir,((i8 *)&net_map_lock_slots_block.scenario_dir_path));
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),((i8 *)&game_dirs.home));
    result = text_open_file_section(filename,section);
    crt_strcpy(((i8 *)&net_map_lock_slots_block.scenario_dir_path),saved_dir);
  }
  if (result == 0) {
    lang_section_opened(filename,section,text_file_in_scenario);
  }
  return result;
}

/* 004a23fc auto */
i8 * __cdecl text_read_line(int trim)

{
  char *line;
  i8 *result;
  
  result = (i8 *)0x0;
  /* (port) a translated section's line, in place of the file's (lang.h) */
  if (lang_section_line((char *)&textbuf.buf) != 0) {
    if (trim == 1) {
      str_trim((char *)&textbuf.buf);
    }
    (*(i8 * *)&text_parse.line_cursor) = ((i8 *)&textbuf.buf);
    return ((i8 *)&textbuf.buf);
  }
  if (((CRT_FEOF(text_file_handle)) == 0) &&
     (line = _fgets(((i8 *)&textbuf.buf),0xff,text_file_handle), line != (char *)0x0)) {
    str_strip_newline((char *)(&textbuf.buf));
    if (trim == 1) {
      str_trim((char *)&textbuf.buf);
    }
    (*(i8 * *)&text_parse.line_cursor) = ((i8 *)&textbuf.buf);
    result = ((i8 *)&textbuf.buf);
  }
  if (result == (i8 *)0x0) {
    textbuf.buf[0] = 0;
    text_close_file();
  }
  return result;
}

/* BF-121 (Peter Johnson's ledger): text_read_line for the readers that use
   the line without checking it, most of them up to the section's closing @
   line: at the end of the file (a section with no @ line after it), "@" in
   place of no line, as the end of the section. */
i8 * __cdecl text_read_line_or_end(int trim)

{
  i8 *line;

  line = text_read_line(trim);
  if (line == (i8 *)0x0) {
    textbuf.buf[0] = '@';
    textbuf.buf[1] = '\0';
    (*(i8 * *)&text_parse.line_cursor) = ((i8 *)&textbuf.buf);
    line = ((i8 *)&textbuf.buf);
  }
  return line;
}

/* 004a24b1 auto */
i8 * __cdecl text_next_token(void)

{
  char *dest;
  
  dest = ((i8 *)&text_parse.token_buf);
  for (; (*(*(char * *)&text_parse.line_cursor) != '\0' && (*(*(char * *)&text_parse.line_cursor) != ',')); (*(char * *)&text_parse.line_cursor)++) {
    *dest = *(*(char * *)&text_parse.line_cursor);
    dest++;
  }
  if (*(*(char * *)&text_parse.line_cursor) != '\0') {
    (*(char * *)&text_parse.line_cursor)++;
  }
  *dest = '\0';
  str_trim((char *)&text_parse.token_buf);
  return &text_parse.token_buf[0];
}

/* 004a2534 auto */
i32 __cdecl text_next_int(void)

{
  iptr token;
  i32 value;
  
  token = (iptr)text_next_token();
  value = str_parse_int_prefixed((const char *)token);
  return value;
}

/* 004a257a auto */
i32 __cdecl text_read_line_pooled(void)

{
  i32 string_id;
  
  text_read_line(1);
  string_id = strpool_add((const char *)&textbuf.buf);
  return string_id;
}

/* 004a25aa auto */
i32 __cdecl text_next_token_pooled(void)

{
  i32 string_id;
  
  text_next_token();
  string_id = strpool_add((const char *)&text_parse.token_buf);
  return string_id;
}

/* 004a25d5 auto */
short __cdecl text_next_binary(void)

{
  short value;
  char *cursor;
  
  value = 0;
  text_next_token();
  for (cursor = ((i8 *)&text_parse.token_buf); (*cursor == '0' || (*cursor == '1')); cursor++) {
    value = value * 2;
    if (*cursor == '1') {
      value++;
    }
  }
  return value;
}

/* 004a2645 auto */
i32 __cdecl text_read_section_line(const char *filename,const char *section,iptr line_index)

{
  int status;
  i32 result;
  int i;
  
  result = 1;
  status = text_open_section(filename,section);
  if (status == 0) {
    for (i = 0; i <= line_index; i++) {
      text_read_line(1);
    }
    result = 0;
  }
  text_close_file();
  return result;
}

/* 004a26bf auto */
i32 __cdecl text_next_token_str_id(i32 min_size)

{
  i32 string_id;
  
  text_next_token();
  string_id = strpool_add_padded((const char *)&text_parse.token_buf,min_size);
  return string_id;
}

/* 004aef20 auto */
iptr __cdecl str_clear(char *str)

{
  *str = 0;
  return (iptr)str;
}

/* 004aef36 auto */
void __cdecl str_append_space(iptr dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_space_str));
  return;
}

/* 004aef57 auto */
void __cdecl str_append_spaces(iptr dest,int count)

{
  int i;
  
  for (i = 0; i < count; i++) {
    str_append_space(dest);
  }
  return;
}

/* 004aef96 auto */
void __cdecl str_append_comma(iptr dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_comma_str));
  return;
}

/* 004aefb7 auto */
void __cdecl str_append_colon(char *dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_colon_str));
  return;
}

/* 004aefd8 auto */
void __cdecl str_append_period(iptr dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_period_str));
  return;
}

/* 004aeff9 auto */
void __cdecl str_append_percent(iptr dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_percent_str));
  return;
}

/* 004af01a auto */
void __cdecl str_append_lparen(iptr dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_lparen_str));
  return;
}

/* 004af03b auto */
void __cdecl str_append_rparen(iptr dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_rparen_str));
  return;
}

/* 004af09e auto */
void __cdecl str_append_plus(iptr dest)

{
  crt_strcat((char *)((void *)dest),((i8 *)&append_plus_str));
  return;
}

/* 004af122 auto */
void __cdecl str_append_string_id(iptr dest,i32 string_id)

{
  i32 text;
  
  text = (iptr)label_string(string_id);
  crt_strcat((char *)((void *)dest),(const char *)((void *)text));
  return;
}

/* 004af14b auto */
void __cdecl str_append_label(iptr dest,int label_id)

{
  str_append_string_id(dest,label_table.string_ids[label_id]);
  return;
}

/* 004af174 auto */
void __cdecl str_concat(iptr dest,iptr src)

{
  crt_strcat((char *)((void *)dest),(const char *)((void *)src));
  return;
}

/* 004af1d5 auto */
i32 __cdecl str_append_int(iptr dest,int value)

{
  i32 result;
  char num_buf[20];
  
  __itoa(value,num_buf,10);
  result = (i32)crt_strcat((char *)((void *)dest),num_buf);
  return result;
}

/* 004af284 auto */
void __cdecl str_append_long(iptr dest,long value)

{
  char num_buf[20];
  
  __ltoa(value,num_buf,10);
  crt_strcat((char *)((void *)dest),(const char *)((void *)num_buf));
  return;
}

/* 004af2b9 auto */
void __cdecl str_append_gold(iptr dest,i32 gold)

{
  str_append_long(dest,gold);
  text_append_space();
  text_append_label(0x143);
  return;
}

/* 004bb370 auto */
void FileObject::read(char *buf,i32 size)

{
  file_obj_read_bytes(this,(LPVOID)buf,size);
  return;
}

/* 004befd1 auto */
void __cdecl text_list_append_wrapped(iptr dialog,i32 string_id,iptr item_count,iptr line_count)

{
  if (*(int *)item_count == 0) {
    text_append_lparen();
  }
  else {
    text_append((const char *)&str_empty_117);
    if (*(int *)line_count < 4) {
      text_append((const char *)&str_empty_118);
    }
    else {
      *(i32 *)line_count = 0;
      ((Dialog *)dialog)->add_line((const char *)&textbuf.buf,0xffffffff,0xffffffff,0xffffffff,0);
      text_clear();
    }
  }
  *(int *)line_count = *(int *)line_count + 1;
  *(int *)item_count = *(int *)item_count + 1;
  text_append_string_id(string_id);
  return;
}

/* 004c4210 auto */
void __cdecl text_set_number_arg_u8(int index,uint value)

{
  text_args.numbers[index] = value & 0xff;
  return;
}

/* 004ccdb6 auto */
void __cdecl text_append_int_2d(i32 value)

{
  char num_buf[20];
  
  _sprintf(num_buf,((i8 *)&int_2d_fmt),value);
  crt_strcat(((i8 *)&textbuf.buf),num_buf);
  return;
}

/* 004ccdef auto */
void __cdecl text_append_advance_abbrev(iptr advance,int with_comma)

{
  size_t abbrev_len;
  size_t len;
  
  if (advance == -2) {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&advance_abbrev_no_strs) + ((with_comma != 0) - 1 & 8));
  }
  else if (advance == -1) {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&advance_abbrev_nil_strs) + ((with_comma != 0) - 1 & 8));
  }
  else if (advance < 100) {
    crt_strcat(((i8 *)&textbuf.buf),(const char *)(advances + advance));
    if (with_comma != 0) {
      crt_strcat(((i8 *)&textbuf.buf),((i8 *)&advance_abbrev_comma_str));
    }
    abbrev_len = _strlen(advances[advance].abbrev);
    if (abbrev_len < 3) {
      len = _strlen(advances[advance].abbrev);
    }
    else {
      len = 3;
    }
    text_append_spaces(3 - len);
  }
  else {
    crt_strcat(((i8 *)&textbuf.buf),((i8 *)&advance_abbrev_err_strs) + ((with_comma != 0) - 1 & 8));
  }
  return;
}

/* 004cfff0 auto */
void __cdecl str_trim_trailing(char *str)

{
  uptr len;
  char *cursor;
  
  len = _strlen(str);
  if (len != 0) {
    for (cursor = ((len - 1) + str);
        (str <= cursor &&
        (((*cursor == ' ' || (*cursor == '\t')) && (*cursor = '\0', cursor != str)))
        ); cursor--) {
    }
  }
  return;
}

/* 004d007e auto */
void __cdecl str_trim(char *str)

{
  char *start;
  i8 buf[256];
  
  str_trim_trailing(str);
  for (start = str;
      (*start != '\0' && ((*start == ' ' || (*start == '\t'))));
      start = start + 1) {
  }
  crt_strcpy(buf,start);
  crt_strcpy((char *)((void *)str),(const char *)((void *)buf));
  return;
}

/* 004db210 auto */
void __cdecl text_section_index_global_init(void)

{
  text_section_index_static_init();
  return;
}

/* 004db225 auto */
void __cdecl text_section_index_static_init(void)

{
  ((MemBlock *)((i8 *)&text_section_index_pool_block.text_section_index_pool))->construct();
  return;
}

/* 004db23f auto */
void __cdecl text_section_index_build(const char *filename,uint pool_size)

{
  FILE *_File;
  long line_offset;
  char *line;
  size_t len;
  long *entry;
  char line_buf[256];
  uint entry_size;
  
  memblock_free((MemBlock *)&text_section_index_pool_block.text_section_index_pool);
  memblock_init((MemBlock *)&text_section_index_pool_block.text_section_index_pool,1,pool_size);
  text_index.valid = 0;
  text_index.count = 0;
  __getcwd(((i8 *)&text_index.path),0x104);
  crt_strcat(((i8 *)&text_index.path),((i8 *)&text_index_path_sep_str));
  crt_strcat(((i8 *)&text_index.path),(const char *)((void *)filename));
  path_default_extension((char *)(&text_index.path),lang_file_ext);
  _File = file_open_path((const char *)&text_index.path,(iptr)&str_rt_6);
  if (_File == (FILE *)0x0) {
    text_index.path[0] = 0;
  }
  else {
    while ((CRT_FEOF(_File)) == 0) {
      line_offset = _ftell(_File);
      line = _fgets(line_buf,0xff,_File);
      if (line == (char *)0x0) break;
      str_strip_newline(line_buf);
      str_trim((char *)line_buf);
      if (((line_buf[0] == '@') && ('@' < line_buf[1])) && (line_buf[1] < '[')) {
        len = _strlen(line_buf);
        entry_size = len + 5;
        if ((int)(uint)text_section_index_pool_block.text_section_pool_free_bytes < (int)entry_size) break;
        entry = (long *)memblock_alloc((MemBlock *)&text_section_index_pool_block.text_section_index_pool,entry_size);
        *entry = line_offset;
        crt_strcpy((char *)(entry + 1),line_buf);
        text_index.count++;
      }
    }
    text_index.valid = 1;
    _fclose(_File);
  }
  return;
}

/* 004db450 auto */
void __cdecl text_section_index_free(void)

{
  text_index.valid = 0;
  text_index.count = 0;
  memblock_free((MemBlock *)&text_section_index_pool_block.text_section_index_pool);
  return;
}

/* 004db481 auto */
i32 __cdecl text_section_index_lookup(const char *filename,const char *section)

{
  char *next;
  int cmp;
  int i;
  char *entry;
  char full_path[260];
  
  if (text_index.valid != 0) {
    __getcwd(full_path,0x104);
    crt_strcat(full_path,((i8 *)&text_lookup_path_sep_str));
    crt_strcat((char *)((void *)full_path),(const char *)((void *)filename));
    cmp = __strcmpi(full_path,((i8 *)&text_index.path));
    if (cmp == 0) {
      entry = text_section_index_pool_block.text_section_index_base;
      for (i = 0; i < text_index.count; i++) {
        cmp = __strcmpi(entry + 4,(char *)section);
        next = entry + 4;
        if (cmp == 0) {
          return *(i32 *)entry;
        }
        while (entry = next, *entry != '\0') {
          next = entry + 1;
        }
        entry++;
      }
    }
  }
  return 0xffffffff;
}

/* 00506448 auto */
void __cdecl text_set_unit_info_args(iptr unit)

{
  iptr adjective;
  int x;
  int y;
  int city_here;
  uint city;
  
  text_clear();
  adjective = civ_get_adjective((int)units[unit].owner);
  text_append((const char *)adjective);
  if ((units[unit].flags & UNITF_VETERAN) != 0) {
    text_append_space();
    text_append_label(0xd);
  }
  text_append_space();
  text_append_string_id(unit_types[units[unit].type].name);
  text_set_string_arg(0,(const char *)&textbuf.buf);
  text_clear();
  x = (int)units[unit].x;
  y = (int)units[unit].y;
  city_here = city_at(x,y);
  if (-1 < city_here) {
    text_append((const char *)(city_here * 0x58 + (iptr)((u8 *)&cities + 32)));
    text_append_space();
  }
  text_append_lparen();
  text_append_int(x);
  text_append_comma();
  text_append_int(y);
  text_append_rparen();
  if (city_here < 0) {
    x = city_find_nearest(x,y,-1,0xffffffff,-1);
    if (-1 < x) {
      text_append_space();
      text_append_lparen();
      text_append_label(0xb2);
      text_append_space();
      text_append((const char *)(x * 0x58 + (iptr)((u8 *)&cities + 32)));
      text_append_rparen();
    }
  }
  text_set_string_arg(1,(const char *)&textbuf.buf);
  text_clear();
  if (units[unit].home_city == -1) {
    city = 0xffffffff;
  }
  else {
    city = (uint)(byte)units[unit].home_city;
  }
  text_append_city_name(city);
  text_set_string_arg(2,(const char *)&textbuf.buf);
  return;
}

/* 00511430 auto */
void __cdecl text_append_plus(void)

{
  str_append_plus((iptr)&textbuf.buf);
  return;
}

/* 005116f0 auto */
void __cdecl text_append_city_population(i32 city)

{
  str_append_city_population((iptr)&textbuf.buf,city);
  return;
}

/* 00553db0 auto */
int __cdecl str_find_bad_filename_char(const char *filename)

{
  __strupr((char *)filename);
  while ((*(char *)filename != '\0' && ((((i8 *)&filename_valid_chars))[*(char *)filename] != '\0'))) {
    filename++;
  }
  return (int)*(char *)filename;
}

/* 00564470 auto */
i32 __cdecl file_require_cdrom(const char *filename)

{
  iptr len;
  int found;
  int attempt;
  char *cursor;
  
  GetModuleFileNameA((*&err_report_ctx_block.app_hinstance),((i8 *)&setup_picwnd_block.exe_dir_path),0x80);
  len = lstrlenA(((i8 *)&setup_picwnd_block.exe_dir_path));
  cursor = ((i8 *)&setup_picwnd_block.exe_dir_path) + len;
  do {
    if (cursor == (char *)0x0) break;
    cursor--;
  } while (*cursor != '\\');
  cursor[1] = '\0';
  do {
    for (attempt = 0; attempt < 2; attempt++) {
      found = (iptr)file_find_fallback_drive(filename);
      if (found != 0) {
        return 1;
      }
    }
    popup_game_flags((const char *)&str_cdromnotfound2,0x2000001);
  } while (dlg_input.help_pressed != 0);
  return 0;
}

/* 00564574 auto */
i8 * __cdecl file_get_fallback_drive(void)

{
  i8 *result;
  
  if (setup_picwnd_block.fallback_drive_root[0] == '\0') {
    result = (i8 *)0x0;
  }
  else {
    result = ((i8 *)&setup_picwnd_block.fallback_drive_root);
  }
  return result;
}

/* 0056459f auto */
i8 * __cdecl file_find_fallback_drive(const char *filename)

{
  UINT uMode;
  DWORD drive_mask;
  i8 *result;
  int drive;
  _OFSTRUCT of_struct;
  UINT drive_type;
  CHAR drive_strings[256];
  uint drive_bit;
  char *drive_root;
  HFILE hfile;
  
  uMode = SetErrorMode(1);
  drive_mask = GetLogicalDrives();
  GetLogicalDriveStringsA(0x100,drive_strings);
  drive_root = drive_strings;
  for (drive = 0; drive <= 0x19; drive++) {
    drive_bit = 1 << ((byte)drive & 0x1f);
    if ((drive_mask & drive_bit) != 0) {
      drive_type = GetDriveTypeA(drive_root);
      if (drive_type == 3) {
        text_clear();
        text_append((const char *)drive_root);
        text_append(filename);
        hfile = OpenFile(((i8 *)&textbuf.buf),&of_struct,0x4000);
        str_clear((char *)&setup_picwnd_block.fallback_drive_root);
        setup_picwnd_block.fallback_drive_root[0] = *drive_root;
        setup_picwnd_block.fallback_drive_root_end = 0;
        str_append((char *)&setup_picwnd_block.fallback_drive_root,(const char *)&str_empty_173);
        break;
      }
      for (; *drive_root != '\0'; drive_root++) {
      }
      drive_root++;
    }
  }
  SetErrorMode(uMode);
  if (setup_picwnd_block.fallback_drive_root[0] == '\0') {
    result = (i8 *)0x0;
  }
  else {
    result = ((i8 *)&setup_picwnd_block.fallback_drive_root);
  }
  return result;
}

/* 00564713 auto */
i32 __cdecl file_resolve_path(const char *path)

{
  char *next;
  char ch;
  iptr len;
  UINT uMode;
  CHAR alt_path[128];
  i8 base_name[16];
  _OFSTRUCT of_struct;
  CHAR try_path[128];
  int name_offset;
  char *cursor;
  HFILE result;
  
  replay_log_video(path);
  len = lstrlenA(path);
  cursor = (char *)(path + len);
  name_offset = lstrlenA(path);
  name_offset++;
  for (; (name_offset != 0 && (*cursor != '\\')); cursor--) {
    name_offset--;
  }
  cursor++;
  str_clear((char *)base_name);
  str_clear((char *)base_name);
  str_append((char *)base_name,(name_offset + path));
  uMode = SetErrorMode(1);
  str_clear((char *)try_path);
  str_append((char *)try_path,(const char *)&setup_picwnd_block.exe_dir_path);
  str_append((char *)try_path,(const char *)base_name);
  result = OpenFile(try_path,&of_struct,0x4000);
  if ((result == 0) || (result == -1)) {
    cursor = (char *)path;
    do {
      if (*cursor == '\0') break;
      next = cursor + 1;
      ch = *cursor;
      cursor = next;
    } while (ch != '\\');
    str_clear((char *)alt_path);
    str_append((char *)alt_path,(const char *)&setup_picwnd_block.exe_dir_path);
    if (*cursor == '\0') {
      str_append((char *)alt_path,path);
    }
    else {
      str_append((char *)alt_path,(const char *)cursor);
    }
    result = OpenFile(alt_path,&of_struct,0x4000);
    if ((result == 0) || (result == -1)) {
      if (setup_picwnd_block.fallback_drive_root[0] == '\0') {
        result = 0;
      }
      else {
        str_clear((char *)try_path);
        str_append((char *)try_path,(const char *)&setup_picwnd_block.fallback_drive_root);
        str_append((char *)try_path,path);
        result = OpenFile(try_path,&of_struct,0x4000);
        if ((result == 0) || (result == -1)) {
          result = 0;
        }
        else {
          str_clear((char *)alt_path);
          str_append((char *)alt_path,(const char *)&setup_picwnd_block.fallback_drive_root);
          str_append((char *)alt_path,path);
          str_clear((char *)path);
          str_append((char *)path,(const char *)alt_path);
          result = 1;
        }
      }
    }
    else {
      str_clear((char *)path);
      str_append((char *)path,(const char *)alt_path);
      result = 1;
    }
  }
  else {
    str_clear((char *)path);
    str_append((char *)path,(const char *)&setup_picwnd_block.exe_dir_path);
    str_append((char *)path,(const char *)base_name);
    result = 1;
  }
  SetErrorMode(uMode);
  return result;
}

/* 00564bf0 auto */
int __cdecl str_parse_int_prefixed(const char *text)

{
  char prefix;
  i32 upper;
  i32 value;
  
  if (*(char *)text == '0') {
    upper = crt_toupper((uint)*(byte *)(text + 1));
    prefix = (char)upper;
    if (prefix == 'X') {
      value = str_parse_hex(text + 2);
    }
    else if (prefix == 'B') {
      value = str_parse_binary(text + 2);
    }
    else if (prefix == 'D') {
      value = _atoi((char *)(text + 2));
    }
    else {
      value = _atoi((char *)text);
    }
  }
  else {
    value = _atoi((char *)text);
  }
  return value;
}

/* 00578abd auto */
void __cdecl str_pipes_to_tabs(char *str)

{
  char *cursor;
  
  for (cursor = str; *cursor != '\0'; cursor++) {
    if (*cursor == '|') {
      *cursor = '\t';
    }
  }
  return;
}

/* 00589fc9 auto */
i32 __cdecl file_read_nth_line(iptr dest,const char *filename,int line_num)

{
  char *line;
  size_t len;
  i32 result;
  iptr i;
  char line_buf[80];
  
  result = 1;
  ScopedFile file(file_open_path(filename,(iptr)&str_rt_9));
  if (file != (FILE *)0x0) {
    for (i = 1; i <= line_num; i++) {
      if (((CRT_FEOF(file)) != 0) ||
         (line = _fgets(line_buf,0x4c,file), line == (char *)0x0)) {
        return result;
      }
    }
    for (i = 0; len = _strlen(line_buf), i < (int)len; i++) {
      if (line_buf[i] < ' ') {
        line_buf[i] = '\0';
      }
    }
    crt_strcpy((char *)((void *)dest),(const char *)((void *)line_buf));
    result = 0;
  }
  return result;
}

/* 0058a80d auto */
void __cdecl file_copy(iptr src_path,iptr dest_path)

{
  int cmp;
  i8 buf[1024];
  FILE *src_file;
  size_t bytes_read;
  FILE *dest_file;
  
  cmp = _strcmp((char *)src_path,(char *)dest_path);
  if (cmp != 0) {
    src_file = _fopen((char *)src_path,((i8 *)&file_copy_read_mode));
    if ((src_file == (FILE *)0x0) ||
       (dest_file = _fopen((char *)dest_path,((i8 *)&file_copy_write_mode)), dest_file == (FILE *)0x0)) {
      MessageBoxA((HWND)0x0,str_error_copying_file,(LPCSTR)0x0,0x10);
      if (src_file != (FILE *)0x0) {
        _fclose(src_file);
      }
    }
    else {
      while (bytes_read = _fread(buf,1,0x400,src_file), bytes_read != 0) {
        _fwrite(buf,1,bytes_read,dest_file);
      }
      _fclose(src_file);
      _fclose(dest_file);
    }
  }
  return;
}

/* 005bad40 auto */
int __cdecl str_parse_binary(const char *text)

{
  char ch;
  int upper;
  i32 value;
  
  value = 0;
  while (*(char *)text != '\0') {
    ch = *(char *)text;
    text++;
    upper = crt_toupper((int)ch);
    if ((upper == 0x30) || (upper == 0x31)) {
      value = value * 2 + upper - 0x30;
    }
    else {
      for (; *(char *)text != '\0'; text++) {
      }
    }
  }
  return value;
}

/* 005baeb0 auto */
void __cdecl text_set_surface(iptr surface)

{
  text_style.surface = surface;
  return;
}

/* 005baec8 auto */
void __cdecl text_set_font(iptr font)

{
  (*(iptr *)&text_style.font) = font;
  return;
}

/* 005baee0 auto */
void __cdecl text_set_colors(i32 fg_color,i32 shadow_color,int shadow_dx,int shadow_dy)

{
  text_style.fg_color = fg_color;
  text_style.shadow_color = shadow_color;
  if (-1 < shadow_dx) {
    text_style.shadow_dx = shadow_dx;
  }
  if (-1 < shadow_dy) {
    text_style.shadow_dy = shadow_dy;
  }
  return;
}

/* 005baf57 auto */
int __cdecl text_draw_shadowed(iptr surface,const char *text,int x,int y)

{
  int width;
  
  if (-1 < text_style.shadow_color) {
    gfx_set_text_color_index(text_style.shadow_color);
    ((DibSurface *)surface)->draw_text((iptr)text_style.font,text,
               text_style.shadow_dx + text_style.fake_bold + x,text_style.shadow_dy + y,5);
  }
  if (-1 < text_style.fg_color) {
    gfx_set_text_color_index(text_style.fg_color);
    if (text_style.fake_bold != 0) {
      ((DibSurface *)surface)->draw_text((iptr)text_style.font,text,x + 1,y,5);
    }
    ((DibSurface *)surface)->draw_text((iptr)text_style.font,text,x,y,5);
  }
  width = text_style.font->text_width(text);
  return x + width;
}

/* 005bb024 auto */
int __cdecl text_draw_centered(iptr surface,const char *text,int x,iptr y,int box_width)

{
  int total_width;
  int width2;
  
  width2 = text_style.font->text_width(text);
  if (-1 < text_style.shadow_color) {
    if ((int)(*(uint *)&text_style.shadow_dx) < 1) {
      width2 = width2 + ~(*(uint *)&text_style.shadow_dx) + 1;
    }
    else {
      width2 += (*(uint *)&text_style.shadow_dx);
    }
  }
  total_width = width2 + text_style.fake_bold;
  text_draw_shadowed(surface,text,x + ((box_width >> 1) - (total_width >> 1)),y);
  return total_width;
}

/* 005bb0af auto */
int __cdecl text_draw_right_aligned(iptr surface,const char *text,int x,i32 y,int box_width)

{
  int draw_x;
  i32 width2;
  
  width2 = text_style.font->text_width(text);
  if (-1 < text_style.shadow_color) {
    if ((int)(*(uint *)&text_style.shadow_dx) < 1) {
      width2 = width2 + ~(*(uint *)&text_style.shadow_dx) + 1;
    }
    else {
      width2 += (*(uint *)&text_style.shadow_dx);
    }
  }
  draw_x = x + (box_width - (width2 + text_style.fake_bold));
  text_draw_shadowed(surface,text,draw_x,y);
  return draw_x;
}

/* 005c5520 auto */
void __cdecl res_free(i32 handle)

{
  res_free_impl(handle);
  return;
}

/* 005c5540 auto */
i32 __cdecl res_load(iptr res_type,i32 res_name)

{
  i32 handle;
  
  handle = (i32)res_load_impl((CHAR *)res_type,(LPCSTR)res_name);
  return handle;
}

/* 005c5560 auto */
i32 __cdecl res_lock(i32 handle)

{
  i32 data;
  
  data = (i32)res_lock_impl((HGLOBAL)handle);
  return data;
}

/* 005c5580 auto */
void __cdecl res_unlock(i32 handle)

{
  res_unlock_impl(handle);
  return;
}

/* 005d7c00 auto */
FileObject *FileObject::init()

{
  _memset(this,0,0x98);
  return this;
}

/* 005d7c6e auto */
i32 FileObject::close()

{
  i32 result;
  
  result = map_close();
  return result;
}

/* 005d8270 auto */
i32 __cdecl file_obj_open_path(char *filename,FileObject *file_obj)

{
  char *sep;
  HFILE file_handle;
  i32 result;
  char full_path[260];
  
  sep = _strchr(filename,0x5c);
  if (sep == (char *)0x0) {
    __getcwd(full_path,0x104);
    crt_strcat(full_path,((i8 *)&file_path_backslash_str));
    crt_strcat(full_path,filename);
  }
  else {
    crt_strcpy(full_path,filename);
  }
  file_handle = OpenFile(full_path,(LPOFSTRUCT)file_obj->open_info,0x4000);
  if (file_handle == -1) {
    result = 0;
  }
  else {
    file_handle = OpenFile(full_path,(LPOFSTRUCT)file_obj->open_info,2);
    file_obj->handle = file_handle;
    if (file_obj->handle == -1) {
      *(i32 *)file_obj = 0;
      file_handle = OpenFile(full_path,(LPOFSTRUCT)file_obj->open_info,0);
      file_obj->handle = file_handle;
      file_obj->map_handle = 0;
      file_obj->map_view = 0;
      if (file_obj->handle == -1) {
        file_obj->handle = 0;
        return 0;
      }
    }
    else {
      *(i32 *)file_obj = 1;
    }
    result = 1;
  }
  return result;
}

/* 005d83d6 auto */
uint __cdecl file_obj_create_path(LPCSTR filename,FileObject *file_obj)

{
  HANDLE handle;
  bool ok;
  
  handle = CreateFileA(filename,0xc0000000,0,(LPSECURITY_ATTRIBUTES)0x0,2,0x80,(HANDLE)0x0);
  file_obj->handle = handle;
  ok = file_obj->handle != -1;
  if (ok) {
    *(i32 *)file_obj = 1;
    file_obj->map_handle = 0;
    file_obj->map_view = 0;
  }
  else {
    file_obj->handle = 0;
    *(i32 *)file_obj = 0;
    file_obj->map_handle = 0;
    file_obj->map_view = 0;
  }
  return (uint)ok;
}

/* 005d8476 auto */
i32 __cdecl file_obj_close_handle(FileObject *file_obj)

{
  i32 result;
  BOOL closed;
  
  if (file_obj->handle == 0) {
    result = 0;
  }
  else {
    if ((file_obj->map_handle != 0) || (file_obj->map_view != 0)) {
      file_obj_close_mapping(file_obj);
    }
    closed = CloseHandle((HANDLE)file_obj->handle);
    if (closed == 0) {
      result = 0;
    }
    else {
      file_obj->handle = 0;
      result = 1;
    }
  }
  return result;
}

/* 005d84f6 auto */
i32 __cdecl file_obj_read_bytes(FileObject *file_obj,LPVOID buf,DWORD size)

{
  i32 result;
  BOOL read_ok;
  DWORD bytes_read;
  
  if (file_obj->handle == 0) {
    result = 0;
  }
  else {
    read_ok = ReadFile((HANDLE)file_obj->handle,buf,size,&bytes_read,(LPOVERLAPPED)0x0);
    if (read_ok == 0) {
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
}

/* 005d8622 auto */
i32 __cdecl file_obj_seek_rel(FileObject *file_obj,LONG offset)

{
  i32 result;
  DWORD new_pos;
  
  if (file_obj->handle == 0) {
    result = 0;
  }
  else {
    new_pos = SetFilePointer((HANDLE)file_obj->handle,offset,(PLONG)0x0,1);
    if (new_pos == 0xffffffff) {
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
}

/* 005d8721 auto */
i32 __cdecl file_obj_write_bytes(FileObject *file_obj,char *buf,DWORD size)

{
  i32 result;
  BOOL write_ok;
  DWORD bytes_written;
  
  if (file_obj->handle == 0) {
    result = 0;
  }
  else if (*(int *)file_obj == 0) {
    dbg_log((const char *)&str_error_tried_to_write_to_a_read_only_file);
    result = 0;
  }
  else {
    write_ok = WriteFile((HANDLE)file_obj->handle,(LPCVOID)buf,size,&bytes_written,(LPOVERLAPPED)0x0);
    if (write_ok == 0) {
      result = 0;
    }
    else {
      result = 1;
    }
  }
  return result;
}

/* 005d881c auto */
int __cdecl file_obj_read_line_impl(FileObject *file_obj,char *buf,int max_len)

{
  int ok;
  int len;
  DWORD bytes_read;
  char *dest;
  char ch[4];
  
  len = 0;
  dest = buf;
  *buf = '\0';
  if (file_obj->handle == 0) {
    ok = 0;
  }
  else {
    ok = ReadFile((HANDLE)file_obj->handle,ch,1,&bytes_read,(LPOVERLAPPED)0x0);
    if (bytes_read == 0) {
      ok = 0;
    }
    else {
      while (((((ok != 0 && (bytes_read != 0)) && (ch[0] != '\n')) &&
              ((ch[0] != '\r' && (ch[0] != '\0')))) && (len < max_len))) {
        if (((ch[0] != '\r') && (ch[0] != '\n')) && (ch[0] != '\0')) {
          *dest = ch[0];
          dest++;
          len++;
        }
        ok = ReadFile((HANDLE)file_obj->handle,ch,1,&bytes_read,(LPOVERLAPPED)0x0);
      }
      *dest = '\0';
      while (((ok != 0 && (bytes_read != 0)) && ((ch[0] == '\n' || (ch[0] == '\r')))))
      {
        ok = ReadFile((HANDLE)file_obj->handle,ch,1,&bytes_read,(LPOVERLAPPED)0x0);
      }
      if (bytes_read != 0) {
        file_obj_seek_rel(file_obj,0xffffffff);
      }
    }
  }
  return ok;
}

/* 005d898e auto */
DWORD __cdecl file_obj_get_size_impl(FileObject *file_obj)

{
  DWORD file_size;
  
  if (file_obj->handle == 0) {
    file_size = 0;
  }
  else {
    file_size = GetFileSize((HANDLE)file_obj->handle,(LPDWORD)0x0);
    if (file_size == 0xffffffff) {
      dbg_log((const char *)&str_bad_file_handle_in_msfilesize);
      file_size = 0;
    }
  }
  return file_size;
}

/* 005d8ab8 auto */
i32 __cdecl file_obj_close_mapping(FileObject *file_obj)

{
  if ((file_obj->map_handle != 0) && (file_obj->map_view != 0)) {
    UnmapViewOfFile((LPCVOID)file_obj->map_view);
    file_obj->map_view = 0;
  }
  if (file_obj->map_handle != 0) {
    CloseHandle((HANDLE)file_obj->map_handle);
    file_obj->map_handle = 0;
  }
  return 0;
}

/* 005db140 auto */
HMODULE __cdecl res_dll_load_library(LPCSTR filename)

{
  HMODULE module;
  i8 msg[128];
  
  module = LoadLibraryExA(filename,(HANDLE)0x0,2);
  if (module == (HMODULE)0x0) {
    crt_strcpy(msg,str_error_resource_file);
    crt_strcat(msg,filename);
    crt_strcat(msg,str_not_found);
    dbg_log((const char *)msg);
    module = (HMODULE)0x0;
  }
  else {
    err_report_ctx_block.res_dll_modules[(*(int *)&res_dll_module_count)] = module;
    (*(int *)&res_dll_module_count)++;
  }
  return module;
}

/* 005db1e0 auto */
void __cdecl res_free_impl(HGLOBAL handle)

{
  FreeResource(handle);
  return;
}

/* 005db1fa auto */
HGLOBAL __cdecl res_load_impl(CHAR *res_type,LPCSTR res_name)

{
  HGLOBAL handle;
  HMODULE hmodule2;
  uint dll_index;
  HRSRC hrsrc;
  CHAR type_buf[5];
  
  type_buf[0] = *res_type;
  type_buf[1] = res_type[1];
  type_buf[2] = res_type[2];
  type_buf[3] = res_type[3];
  type_buf[4] = 0;
  dll_index = 0xffffffff;
  hrsrc = FindResourceA((*&err_report_ctx_block.app_hinstance),res_name,type_buf);
  if (hrsrc == (HRSRC)0x0) {
    dll_index = 0;
    while ((dll_index < res_dll_module_count &&
           (hrsrc = FindResourceA(err_report_ctx_block.res_dll_modules[dll_index],res_name,type_buf),
           hrsrc == (HRSRC)0x0))) {
      dll_index++;
    }
  }
  if (hrsrc == (HRSRC)0x0) {
    handle = (HGLOBAL)0x0;
  }
  else {
    if (dll_index == 0xffffffff) {
      hmodule2 = (*&err_report_ctx_block.app_hinstance);
    }
    else {
      hmodule2 = err_report_ctx_block.res_dll_modules[dll_index];
    }
    handle = LoadResource(hmodule2,hrsrc);
  }
  return handle;
}

/* 005db531 auto */
LPVOID __cdecl res_lock_impl(HGLOBAL handle)

{
  LPVOID data;
  
  data = LockResource(handle);
  return data;
}

/* 005db54b auto */
void __cdecl res_unlock_impl(i32 handle)

{
  return;
}

/* 005db55b auto */
void __cdecl dll_unload(HMODULE hmodule)

{
  uint i;
  
  for (i = 0;
      (err_report_ctx_block.res_dll_modules[i] != hmodule && (i < res_dll_module_count));
      i = i + 1) {
  }
  if (i < res_dll_module_count) {
    for (; i < res_dll_module_count; i++) {
      err_report_ctx_block.res_dll_modules[i] = err_report_ctx_block.res_dll_modules[i + 1];
    }
    res_dll_module_count--;
    FreeLibrary(hmodule);
  }
  return;
}
