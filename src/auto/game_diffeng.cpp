/* The game, the difference engine: the game state as images and differences sent to the other players of a network game: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 004b08e0 auto */
/* Library Function - Single Match
    _$E2
   
   Library: Visual Studio 1998 Debug */

void __cdecl diffeng_global_init(void)

{
  diffeng_static_init_stub();
  return;
}

/* 004b08f5 auto */
/* Library Function - Single Match
    _$E1
   
   Library: Visual Studio 1998 Debug */

void __cdecl diffeng_static_init_stub(void)

{
  return;
}

/* 004b0905 auto */
i32 __cdecl diffeng_alloc_mirror(void)

{
  size_t mirror_len;
  i32 result;
  int section;
  
  if (diffeng.mirror_buf != 0) {
    diffeng_free_mirror();
  }
  diffeng_scan.mirror_size[0] = 0;
  for (section = 0; section < 0x17; section++) {
    diffeng_scan.mirror_size[0] += (i32)diffeng_section_table[section].padded_size;
  }
  if ((diffeng_scan.mirror_size[0] & 3) != 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_mirrorlength_sizeof_long_0,(const char*)&str_d_ss_franklinton_difference_engine_cpp,0x22);
  }
  diffeng.mirror_buf = (iptr)operator_new(diffeng_scan.mirror_size[0]);
  if ((void *)diffeng.mirror_buf == (void *)0x0) {
    result = 0;
  }
  else {
    _memset((void *)diffeng.mirror_buf,0,diffeng_scan.mirror_size[0]);
    mirror_len = diffeng_map_known1_section_block.diffeng_client_mirror_size;
    if (net.host_flag != '\0') {
      mirror_len = diffeng_scan.mirror_size[0];
    }
    diffeng_scan.mirror_end = mirror_len + diffeng.mirror_buf;
    diffeng_copy_to_mirror();
    result = 1;
  }
  return result;
}

/* 004b0a0a auto */
void __cdecl diffeng_free_mirror(void)

{
  operator_delete((void *)diffeng.mirror_buf);
  diffeng.mirror_buf = 0;
  return;
}

/* 004b0a41 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl diffeng_copy_to_mirror(void)

{
  int section;
  u8 *dst;
  
  dst = (u8 *)diffeng.mirror_buf;
  for (section = 0; section < 0x17; section++) {
    crt_memcpy(dst,(const void *)diffeng_section_table[section].data_ptr,
                 (i32)diffeng_section_table[section].data_size);
    dst += (i32)diffeng_section_table[section].padded_size;
  }
  diffeng_scan.scan_ptr = diffeng.mirror_buf;
  diffeng_scan.scan_section = 0;
  diffeng_scan.scan_offset = 0;
  return;
}

/* 004b0ad0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl diffeng_invalidate_mirror(void)

{
  uint word_count;
  uint i;
  
  diffeng_copy_to_mirror();
  (*(uint * *)&diffeng_scan.scan_ptr) = (uint *)diffeng.mirror_buf;
  word_count = diffeng_scan.mirror_size[0] >> 2;
  for (i = 0; i < word_count; i++) {
    *(*(uint * *)&diffeng_scan.scan_ptr) = ~*(*(uint * *)&diffeng_scan.scan_ptr);
    (*(uint * *)&diffeng_scan.scan_ptr)++;
  }
  (*(uint * *)&diffeng_scan.scan_ptr) = (uint *)diffeng.mirror_buf;
  diffeng_scan.scan_section = 0;
  diffeng_scan.scan_offset = 0;
  return;
}

/* 004b14a4 auto */
int __cdecl diffeng_total_size(void)

{
  int section;
  int total;
  
  total = 0;
  for (section = 0; section < 0x18; section++) {
    if (section == 5) {
      total = game.num_units * 0x20 + total;
    }
    else if (section == 6) {
      total = game.num_cities * 0x58 + total;
    }
    else {
      total += (int)diffeng_section_table[section].data_size;
    }
  }
  return total + 0x1e0;
}

/* 004b153c auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl diffeng_build_civ_image(void **out_buf)

{
  void *alloc;
  i32 result;
  i32 write_pos[2];
  uint capacity2;
  int buf;
  
  write_pos[0] = 0;
  capacity2 = diffeng_game_section_block.diffeng_game_section[0] + diffeng_leaders_section_block.diffeng_leaders_section[0] + diffeng_civs_section_block.diffeng_civs_section[0] + diffeng_civ_names_section_block.diffeng_civ_names_section[0] + diffeng_scenario_section_block.diffeng_scenario_section[0] + diffeng_civ_view_section_block.diffeng_civ_view_section[0]
            + diffeng_events_section_block.diffeng_events_section[0] + 0x8c;
  if ((int)capacity2 < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_size_0,(const char*)&str_d_ss_franklinton_difference_engine_cpp_4,0x226);
  }
  alloc = operator_new(capacity2);
  *out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocated_buffer_for_game_stat);
    result = 0;
  }
  else {
    diffeng_game_section_block.diffeng_game_checksum_i32 = mem_checksum((const void *)diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_game_section_block.diffeng_game_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_leaders_section_block.diffeng_leaders_checksum_i32 = mem_checksum((const void *)diffeng_leaders_section_block.diffeng_leaders_data,diffeng_leaders_section_block.diffeng_leaders_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_leaders_section_block.diffeng_leaders_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_leaders_section_block.diffeng_leaders_data,diffeng_leaders_section_block.diffeng_leaders_section[0]);
    diffeng_civs_section_block.diffeng_civs_checksum_i32 = mem_checksum((const void *)diffeng_civs_section_block.diffeng_civs_data,diffeng_civs_section_block.diffeng_civs_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_civs_section_block.diffeng_civs_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_civs_section_block.diffeng_civs_data,diffeng_civs_section_block.diffeng_civs_section[0]);
    diffeng_civ_names_section_block.diffeng_civ_names_checksum_i32 = mem_checksum((const void *)diffeng_civ_names_section_block.diffeng_civ_names_data,diffeng_civ_names_section_block.diffeng_civ_names_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_civ_names_section_block.diffeng_civ_names_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_civ_names_section_block.diffeng_civ_names_data,diffeng_civ_names_section_block.diffeng_civ_names_section[0]);
    diffeng_scenario_section_block.diffeng_scenario_checksum_i32 = mem_checksum((const void *)diffeng_scenario_section_block.diffeng_scenario_data,diffeng_scenario_section_block.diffeng_scenario_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_scenario_section_block.diffeng_scenario_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_scenario_section_block.diffeng_scenario_data,diffeng_scenario_section_block.diffeng_scenario_section[0]);
    diffeng_civ_view_section_block.diffeng_civ_view_checksum_i32 = mem_checksum((const void *)diffeng_civ_view_section_block.diffeng_civ_view_data,diffeng_civ_view_section_block.diffeng_civ_view_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_civ_view_section_block.diffeng_civ_view_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_civ_view_section_block.diffeng_civ_view_data,diffeng_civ_view_section_block.diffeng_civ_view_section[0]);
    diffeng_events_section_block.diffeng_events_checksum_i32 = mem_checksum((const void *)diffeng_events_section_block.diffeng_events_data,diffeng_events_section_block.diffeng_events_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_events_section_block.diffeng_events_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_events_section_block.diffeng_events_data,diffeng_events_section_block.diffeng_events_section[0]);
    result = mem_rle_compress(out_buf,write_pos[0]);
  }
  return result;
}

/* 004b187f auto */
void __cdecl diffeng_buf_append(char *buf,int *pos_ptr,int capacity,const void *src,int len)

{
  if (capacity < *pos_ptr + len) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_cursize_datsize_bufsize,(const char*)&str_d_ss_franklinton_difference_engine_cpp_5,0x20d);
    return;   /* (BF-083) the original stopped at its assertion: the copy would overrun */
  }
  crt_memcpy((void *)(*pos_ptr + buf),src,len);
  *pos_ptr = *pos_ptr + len;
  return;
}

/* 004b18e1 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

i32 __cdecl diffeng_build_map_header_image(void **out_buf)

{
  void *alloc;
  i32 result;
  i32 write_pos[2];
  uint capacity2;
  int buf;
  
  write_pos[0] = 0;
  capacity2 = diffeng_game_section_block.diffeng_game_section[0] + 0x28 + diffeng_map_header_section_block.diffeng_map_header_section[0];
  alloc = operator_new(capacity2);
  *out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocated_buffer_for_game_stat_2);
    result = 0;
  }
  else {
    diffeng_game_section_block.diffeng_game_checksum_i32 = mem_checksum((const void *)diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_game_section_block.diffeng_game_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_game_section_block.diffeng_game_data,diffeng_game_section_block.diffeng_game_section[0]);
    diffeng_map_header_section_block.diffeng_map_header_checksum_i32 = mem_checksum((const void *)diffeng_map_header_section_block.diffeng_map_header_data,diffeng_map_header_section_block.diffeng_map_header_section[0]);
    diffeng_buf_append((char *)buf,write_pos,capacity2,&diffeng_map_header_section_block.diffeng_map_header_section,0x14);
    diffeng_buf_append((char *)buf,write_pos,capacity2,(const void *)diffeng_map_header_section_block.diffeng_map_header_data,diffeng_map_header_section_block.diffeng_map_header_section[0]);
    result = mem_rle_compress(out_buf,write_pos[0]);
  }
  return result;
}

/* 004b1a15 auto */
i32 __cdecl diffeng_build_full_image(void **out_buf)

{
  uint capacity;
  void *alloc;
  i32 checksum;
  int encoded;
  RleStream rle;  /* (the original's locals in a row) */
  i32 result;
  int section;
  int header[5];
  i32 error_code;
  uint write_pos;
  int buf;
  
  capacity = diffeng_total_size();
  if ((int)capacity < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_sz_0,(const char*)&str_d_ss_franklinton_difference_engine_cpp_6,0x27f);
  }
  alloc = operator_new(capacity);
  *out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocated_buffer_for_game_stat_3);
    result = 0;
  }
  else {
    write_pos = 0;
    for (section = 0; section < 0x18; section++) {
      crt_memcpy(header,(i8 *)&diffeng_section_table[section],0x14);
      if (section == 5) {
        header[0] = (int)game.num_units << 5;
      }
      else if (section == 6) {
        header[0] = game.num_cities * 0x58;
      }
      checksum = mem_checksum((const void *)diffeng_section_table[section].data_ptr,header[0]);
      diffeng_section_table[section].checksum = checksum;
      diffeng_buf_append((char *)buf,(int *)&write_pos,capacity,header,0x14);
      diffeng_buf_append((char *)buf,(int *)&write_pos,capacity,(const void *)diffeng_section_table[section].data_ptr,header[0]);
    }
    rle.src = (u8 *)operator_new(write_pos);
    if (rle.src == 0) {
      dbg_log((const char *)&str_failed_to_allocated_buffer_2_for_game_st);
      result = 0;
    }
    else {
      rle.src_end = rle.src + write_pos;
      crt_memcpy(rle.src,(const void *)buf,write_pos);
      rle.dst = *(u8 **)out_buf;
      rle.dst_end = *(u8 **)out_buf + write_pos;
      encoded = diffeng_rle_encode(&rle);
      result = rle.out_len;
      if (encoded == 0) {
        error_code = 0xffffffff;
        result = error_code;
      }
      else {
        checksum = (i32)crt_realloc((void *)*(i32 *)out_buf,result);
        *(i32 *)out_buf = checksum;
      }
    }
  }
  return result;
}

/* 004b1c11 auto */
i32 __cdecl diffeng_build_delta_image(void **out_buf)

{
  uint capacity;
  void *alloc;
  i32 result;
  int section;
  int header[5];
  int checksum2;
  i32 write_pos[2];
  int buf;
  
  capacity = diffeng_total_size();
  if ((int)capacity < 0) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_sz_0_2,(const char*)&str_d_ss_franklinton_difference_engine_cpp_7,0x2c6);
  }
  alloc = operator_new(capacity);
  *out_buf = alloc;
  buf = *(int *)out_buf;
  if (buf == 0) {
    dbg_log((const char *)&str_failed_to_allocate_buffer_for_game_state);
    result = 0;
  }
  else {
    write_pos[0] = 0;
    for (section = 0; section < 0x18; section++) {
      crt_memcpy(header,(i8 *)&diffeng_section_table[section],0x14);
      if (section == 5) {
        header[0] = (int)game.num_units << 5;
      }
      else if (section == 6) {
        header[0] = game.num_cities * 0x58;
      }
      checksum2 = mem_checksum((const void *)diffeng_section_table[section].data_ptr,header[0]);
      if (((int)diffeng_section_table[section].checksum != checksum2) && (header[0] != 0)) {
        diffeng_section_table[section].checksum = checksum2;
        diffeng_buf_append((char *)buf,write_pos,capacity,header,0x14);
        diffeng_buf_append((char *)buf,write_pos,capacity,(const void *)diffeng_section_table[section].data_ptr,header[0]);
      }
    }
    result = (i32)crt_realloc((void *)*(i32 *)out_buf,write_pos[0]);
    *(i32 *)out_buf = result;
    if (*(int *)out_buf == 0) {
      dbg_log((const char *)&str_failed_to_re_allocate_buffer_for_game_st);
      result = 0;
    }
    else {
      result = mem_rle_compress(out_buf,write_pos[0]);
    }
  }
  return result;
}

/* 004b1de3 auto */
void __cdecl diffeng_apply_image(int *image_buf,char compressed)

{
  int *section_id_ptr;
  int *payload;
  size_t src_size;
  int decoded;
  i32 checksum;
  RleStream rle;  /* (the original's locals in a row) */
  u32 saved_game_options;
  u16 saved_msg_options;
  int *header;
  int offset;
  uint total_size;
  int *cursor;
  
  if ((image_buf == (int *)0x0) || (*image_buf == 0)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_phdata_phdata,(const char*)&str_d_ss_franklinton_difference_engine_cpp_8,0x2fc);
  }
  saved_game_options = game.game_options;
  saved_msg_options = game.message_options;
  if (diffeng_sections_block.diffeng_section_ptrs[0] == 0) {
    diffeng_register_blocks();
  }
  if (net.nm_full_gamestate_count == 0) {
    if (compressed != '\0') {
      total_size = mem_rle_decompress((void **)image_buf);
    }
    cursor = (int *)*image_buf;
    total_size = __msize((void *)*image_buf);
  }
  else {
    rle.src = (u8 *)*image_buf;
    src_size = __msize((void *)*image_buf);
    rle.src_end = rle.src + src_size;
    total_size = diffeng_rle_decoded_size(&rle);
    rle.dst = (u8 *)operator_new(total_size);
    if (rle.dst == 0) {
      dbg_log((const char *)&str_failed_to_allocate_buffer_2_for_game_sta);
      return;
    }
    rle.dst_end = rle.dst + total_size;
    decoded = diffeng_rle_decode(&rle);
    if (decoded == 0) {
      dbg_log((const char *)&str_failed_to_rllbufferdecode_in_setgamestat);
      return;
    }
    operator_delete((void *)*image_buf);
    *image_buf = (int)rle.dst;
    cursor = (int *)*image_buf;
  }
  offset = 0;
  while (offset < (int)total_size) {
    header = cursor;
    payload = cursor + 5;
    offset = *cursor + offset + 0x14;
    if ((cursor[2] < 0) || (section_id_ptr = cursor + 2, cursor = payload, 0x17 < *section_id_ptr)) {
      cursor = payload;
      ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_pnode_type_0_pnode_type_num_block_types,(const char*)&str_d_ss_franklinton_difference_engine_cpp_9,0x338);
    }
    crt_memcpy((void *)(iptr)diffeng_section_table[header[2]].data_ptr,cursor,*header);
    checksum = mem_checksum(cursor,*header);
    diffeng_section_table[header[2]].checksum = checksum;
    cursor = (int *)((int)cursor + *header);
  }
  game.message_options = saved_msg_options;
  game.game_options = saved_game_options;
  return;
}

/* 004b2010 auto */
void __cdecl diffeng_apply_map_header_image(int *image_buf,char compressed)

{
  u32 saved_game_options;
  u16 saved_msg_options;
  int section_id;
  int cursor;
  
  if ((image_buf == (int *)0x0) || (*image_buf == 0)) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_phdata_phdata_2,(const char*)&str_d_ss_franklinton_difference_engine_cpp_10,0x34b);
  }
  saved_msg_options = game.message_options;
  saved_game_options = game.game_options;
  if (compressed != '\0') {
    mem_rle_decompress((void **)image_buf);
  }
  cursor = *image_buf;
  map_free();
  section_id = diffeng_read_block((int **)&cursor);
  if (section_id != 1) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_blocktype_btgame,(const char*)&str_d_ss_franklinton_difference_engine_cpp_11,0x35a);
  }
  section_id = diffeng_read_block((int **)&cursor);
  if (section_id != 0xc) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_blocktype_btmapstruct,(const char*)&str_d_ss_franklinton_difference_engine_cpp_12,0x35d);
  }
  game.game_options = saved_game_options;
  game.message_options = saved_msg_options;
  map_alloc();
  diffeng_register_blocks();
  diffeng_alloc_mirror();
  return;
}

/* 004b2123 auto */
int __cdecl diffeng_read_block(int **cursor_ptr)

{
  int *header;
  i32 checksum;
  
  header = *cursor_ptr;
  *(int *)cursor_ptr = *(int *)cursor_ptr + 0x14;
  if ((header[2] < 0) || (0x17 < header[2])) {
    ((ErrorContext *)((i8 *)&err_report_ctx_block.err_report_ctx))->report(7,(const char*)&str_pnode_type_0_pnode_type_num_block_types_2,(const char*)&str_d_ss_franklinton_difference_engine_cpp_13,0x36d);
  }
  crt_memcpy((void *)(iptr)diffeng_section_table[header[2]].data_ptr,(const void *)*(i32 *)cursor_ptr,*header);
  checksum = mem_checksum((const void *)*(iptr *)cursor_ptr,*header);
  diffeng_section_table[header[2]].checksum = checksum;
  *(int *)cursor_ptr = *(int *)cursor_ptr + *header;
  return header[2];
}

/* 004b21d7 auto */
void __cdecl diffeng_register_blocks(void)

{
  int offset;
  int section;
  
  memblock_ensure_locked((MemBlock *)&unit_types_block.str_empty_188);
  ((DiffEngSection *)((i8 *)&diffeng_sections_block.diffeng_sections))->init((void *)&str_empty_115,4,0);
  ((DiffEngSection *)((i32 *)&diffeng_game_section_block.diffeng_game_section))->init(&game,0x14c,1);
  ((DiffEngSection *)((i32 *)&diffeng_civ_names_section_block.diffeng_civ_names_section))->init(&civ_names,0x790,2);
  ((DiffEngSection *)((i32 *)&diffeng_civs_section_block.diffeng_civs_section))->init(&civs,0x2ca0,3);
  ((DiffEngSection *)((i8 *)&diffeng_landmass_section))->init(&landmass_table_block.landmass_table,0x400,4);
  ((DiffEngSection *)((i8 *)&diffeng_units_section))->init(((u8 *)units),0x10000,5);
  ((DiffEngSection *)((i8 *)&diffeng_cities_section))->init(&cities,0x5800,6);
  ((DiffEngSection *)((i32 *)&diffeng_leaders_section_block.diffeng_leaders_section))->init(((u8 *)leaders),0x3f0,7);
  ((DiffEngSection *)((i8 *)&diffeng_power_graph_section))->init(&power_graph_history_block.power_graph_history,0x4b0,8);
  ((DiffEngSection *)((i32 *)&diffeng_scenario_section_block.diffeng_scenario_section))->init(&scenario.flags,100,9);
  ((DiffEngSection *)((i8 *)&diffeng_destroyed_civs_section))->init(&destroyed_civs.count,0x154,10);
  ((DiffEngSection *)((i32 *)&diffeng_civ_view_section_block.diffeng_civ_view_section))->init(&civ_view.saved_cursor_x,0x494,0xb);
  ((DiffEngSection *)((i32 *)&diffeng_map_header_section_block.diffeng_map_header_section))->init(&map,0x10,0xc);
  ((DiffEngSection *)((i8 *)&diffeng_map_coarse0_section))->init(map_data.coarse[0],(int)map.quarter_w * (int)map.quarter_h
             ,0xd);
  ((DiffEngSection *)((i8 *)&diffeng_map_coarse1_section))->init(map_data.coarse[1],(int)map.quarter_w * (int)map.quarter_h
             ,0xe);
  ((DiffEngSection *)((i8 *)&diffeng_map_known1_section_block.diffeng_map_known1_section))->init(map_data.known[1],(int)map.area,0xf);
  ((DiffEngSection *)((i8 *)&diffeng_map_known2_section))->init(map_data.known[2],(int)map.area,0x10);
  ((DiffEngSection *)((i8 *)&diffeng_map_known3_section))->init(map_data.known[3],(int)map.area,0x11);
  ((DiffEngSection *)((i8 *)&diffeng_map_known4_section))->init(map_data.known[4],(int)map.area,0x12);
  ((DiffEngSection *)((i8 *)&diffeng_map_known5_section))->init(map_data.known[5],(int)map.area,0x13);
  ((DiffEngSection *)((i8 *)&diffeng_map_known6_section))->init(map_data.known[6],(int)map.area,0x14);
  ((DiffEngSection *)((i8 *)&diffeng_map_known7_section))->init(map_data.known[7],(int)map.area,0x15);
  ((DiffEngSection *)((i8 *)&diffeng_map_tiles_section))->init(map_data.tiles,map.area * 6,0x16);
  ((DiffEngSection *)((i32 *)&diffeng_events_section_block.diffeng_events_section))->init(((void *)*(iptr *)&unit_types_block.events_pool_base),50000,0x17);
  offset = 0;
  for (section = 0; section < 0x17; section++) {
    diffeng_section_table[section].mirror_offset = offset;
    offset += (i32)diffeng_section_table[section].padded_size;
  }
  return;
}

/* 004b24a2 auto */
/* The decoded size of a run-length coded stream (diffeng_rle_encode): a code
   below 0x8000 is a run of that many copies of the one byte after it, else
   a literal of (code & 0x7fff) bytes. (BF-092) A code cut short at the end
   is not counted: the original read past the stream for it. */
int __cdecl diffeng_rle_decoded_size(RleStream *stream)

{
  uint code;
  int total;
  ushort *src;
  
  src = (ushort *)stream->src;
  total = 0;
  while (2 <= (int)stream->src_end - (int)src) {
    code = (uint)*src;
    if (code < 0x8000) {
      src = (ushort *)((int)src + 3);
    }
    else {
      src = (ushort *)((int)src + (code & 0x7fff) + 2);
    }
    total += (code & 0x7fff);
  }
  return total;
}

/* 004b251a auto */
/* (BF-092) The original took code 0x7fff, the encoder's run of 32767 bytes,
   for a 65535-byte literal, past what the size above counted, and checked no
   code against the stream's ends: decoded here as the size counts it, and a
   code that does not fit the stream or the output fails (0). Valid streams
   decode as before: the encoder writes no literal code 0x7fff (0xffff is the
   32767-byte literal). */
i32 __cdecl diffeng_rle_decode(RleStream *stream)

{
  ushort *payload;
  ushort run_len;
  uint decoded_size;
  i32 result;
  ushort *src;
  void *dst;
  uint src_left;
  
  decoded_size = diffeng_rle_decoded_size(stream);
  if ((uint)((int)stream->dst_end - (int)stream->dst) < decoded_size) {
    result = 0;
  }
  else {
    src = (ushort *)stream->src;
    dst = (void *)stream->dst;
    while (src < (ushort *)stream->src_end) {
      src_left = (uint)((int)stream->src_end - (int)src);
      if (src_left < 2) {
        return 0;
      }
      run_len = *src;
      payload = src + 1;
      if (run_len < 0x8000) {
        if (src_left < 3) {
          return 0;
        }
      }
      else {
        run_len &= 0x7fff;
        if (src_left - 2 < (uint)run_len) {
          return 0;
        }
      }
      if ((uint)((int)stream->dst_end - (int)dst) < (uint)run_len) {
        return 0;
      }
      if (*src < 0x8000) {
        src = (ushort *)((int)src + 3);
        _memset(dst,(int)*(byte *)payload,(uint)run_len);   /* (the run's byte: one byte, not a ushort) */
      }
      else {
        crt_memcpy(dst,payload,(uint)run_len);
        src = (ushort *)((int)payload + (uint)run_len);
      }
      dst = (void *)((int)dst + (uint)run_len);
    }
    stream->out_len = (int)dst - (int)stream->dst;
    result = 1;
  }
  return result;
}

/* 004b263e auto */
i32 __cdecl diffeng_rle_encode(RleStream *stream)

{
  short *code_ptr;
  uint src_len;
  uint dst_capacity;
  char *scan;
  char *run_start;
  short *dst;
  uptr chunk_len;
  uint run_len;
  uint out_len;
  
  dst = (short *)stream->dst;
  run_start = (char *)stream->src;
  scan = run_start + 1;
  src_len = (int)stream->src_end - (int)stream->src;
  out_len = 0;
  dst_capacity = (int)stream->dst_end - (int)stream->dst;
  do {
    if ((char *)stream->src_end <= run_start) {
      stream->out_len = out_len;
      return 1;
    }
    for (; (scan < (char *)stream->src_end && (*scan == *run_start)); scan = scan + 1
        ) {
    }
    run_len = (int)scan - (int)run_start;
    if (3 < run_len) {
      while (code_ptr = dst, run_len != 0) {
        if ((dst_capacity < out_len + 2) || (src_len <= out_len + 2)) {
          return 0;
        }
        if (run_len < 0x8000) {
          *dst = (short)run_len;
          run_len = 0;
        }
        else {
          *dst = 0x7fff;
          run_len -= 0x7fff;
        }
        dst++;
        out_len += 3;
        if ((dst_capacity < out_len) || (src_len <= out_len)) {
          return 0;
        }
        *(char *)dst = *run_start;
        dst = (short *)((int)code_ptr + 3);
      }
      run_start = scan;
    }
    /* (the original compared the last byte with the one after the input;
       the end ends the literal here instead: the same decoded bytes) */
    for (; (scan < (char *)stream->src_end && (scan + 1 == (char *)stream->src_end || scan[1] != *scan));
        scan = scan + 1) {
    }
    run_len = (int)scan - (int)run_start;
    while (run_len != 0) {
      out_len += 2;
      if ((dst_capacity < out_len) || (src_len <= out_len)) {
        return 0;
      }
      if (run_len < 0x8000) {
        *dst = (short)run_len - 0x8000;
        chunk_len = run_len;
        run_len = 0;
      }
      else {
        *dst = -1;
        chunk_len = 0x7fff;
        run_len -= 0x7fff;
      }
      dst++;
      out_len += chunk_len;
      if ((dst_capacity < out_len) || (src_len <= out_len)) {
        return 0;
      }
      crt_memcpy(dst,run_start,chunk_len);
      dst = (short *)((int)dst + chunk_len);
      run_start += chunk_len;
    }
  } while( true );
}

/* 004b3080 auto */
void DiffEngSection::init(void *data_ptr,u32 data_size,u32 section_id)

{
  this->data_ptr = (u32)data_ptr;
  this->data_size = data_size;
  this->section_id = section_id;
  checksum = 0;
  padded_size = this->data_size;
  if ((this->data_size & 3) != 0) {
    padded_size = (4 - (this->data_size & 3)) + padded_size;
  }
  return;
}
