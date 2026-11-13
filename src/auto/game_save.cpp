/* The game, saving and loading: saved games, passwords, preferences: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00472f7b auto */
void __cdecl save_pack_layout_rects(void)

{
  int i;
  
  grey_palette_block.save_layout_record[0] = (i16)destroyed_civ_count_block.saved_layout_valid;
  rect_pack_to_short((RECT *)&saved_screen_rect_block.saved_screen_rect,(ShortRect *)&grey_palette_block.str_empty_189);
  rect_pack_to_short((RECT *)&saved_screen_rect_block.layout_rect_main,(ShortRect *)&grey_palette_block.saved_layout_rects);
  for (i = 0; i < 8; i++) {
    rect_pack_to_short((RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + i * 0x10),(ShortRect *)(i * 8 + (iptr)(((u8 *)&grey_palette_block.saved_layout_rects + 8))));
  }
  rect_pack_to_short((RECT *)&minimap_wnd_rect_block.minimap_wnd_rect,(ShortRect *)&grey_palette_block.str_empty_190);
  rect_pack_to_short((RECT *)&minimap_wnd_rect_block.status_wnd_rect,(ShortRect *)&grey_palette_block.str_empty_191);
  rect_pack_to_short((RECT *)&minimap_wnd_rect_block.city_wnd_rect,(ShortRect *)&grey_palette_block.saved_layout_rect2);
  return;
}

/* 00473064 auto */
void __cdecl save_unpack_layout_rects(void)

{
  int i;
  
  destroyed_civ_count_block.saved_layout_valid = (int)grey_palette_block.save_layout_record[0];
  rect_unpack_from_short((ShortRect *)&grey_palette_block.str_empty_189,(RECT *)&saved_screen_rect_block.saved_screen_rect);
  rect_unpack_from_short((ShortRect *)&grey_palette_block.saved_layout_rects,(RECT *)&saved_screen_rect_block.layout_rect_main);
  for (i = 0; i < 8; i++) {
    rect_unpack_from_short((ShortRect *)(i * 8 + (iptr)(((u8 *)&grey_palette_block.saved_layout_rects + 8))),(RECT *)(iptr)(((i8 *)&map_view_saved_rects_block.map_view_saved_rects) + i * 0x10));
  }
  rect_unpack_from_short((ShortRect *)&grey_palette_block.str_empty_190,(RECT *)&minimap_wnd_rect_block.minimap_wnd_rect);
  rect_unpack_from_short((ShortRect *)&grey_palette_block.str_empty_191,(RECT *)&minimap_wnd_rect_block.status_wnd_rect);
  rect_unpack_from_short((ShortRect *)&grey_palette_block.saved_layout_rect2,(RECT *)&minimap_wnd_rect_block.city_wnd_rect);
  return;
}

/* A NUL inside a saved text field of `size` bytes. */
static int save_text_terminated(const char *text,int size)
{
  return memchr(text,0,size) != (void *)0x0;
}

/* BF-093, BF-123..128, BF-193, BF-232, BF-233 (Peter Johnson's ledger): what
   indexes the game's tables in the units and cities a save brings, as the game
   writes them: an active unit's type, owner, stack links, home city (none or an
   active city), a caravan's commodity (0..15, none) and, with goto orders, its
   square on the map; an active city's owner, trade routes and their partners,
   commodities supplied and demanded (negative: in use), production and name.
   0 refuses the file as one that does not read. */
static int save_records_valid(void)
{
  int unit;
  int city;
  int i;
  Unit *u;
  City *c;

  for (unit = 0; unit < game.num_units; unit++) {
    u = units + unit;
    if (u->id == 0) continue;
    if ((0x3d < u->type) || ((uint)(int)u->owner > 7) ||
       (u->prev_in_stack < -1) || (0x7ff < u->prev_in_stack) ||
       (u->next_in_stack < -1) || (0x7ff < u->next_in_stack)) {
      return 0;
    }
    if ((byte)u->home_city != 0xff) {
      if ((game.num_cities <= (byte)u->home_city) || (cities[(byte)u->home_city].id == 0)) {
        return 0;
      }
    }
    if ((unit_types[u->type].role == ROLE_TRADE) && (0xf < u->counter) && (u->counter != 0xff)) {
      return 0;
    }
    if (((u->orders == ORDER_GOTO) || (u->orders == ORDER_AI_GOTO)) &&
       (map_in_bounds(u->x,u->y) == 0)) {
      return 0;
    }
  }
  for (city = 0; city < game.num_cities; city++) {
    c = cities + city;
    if (c->id == 0) continue;
    if (((uint)(int)c->owner > 7) || (3 < c->trade_routes) ||
       (c->building < -0x42) || (0x3d < c->building) ||
       (save_text_terminated(c->name,sizeof c->name) == 0)) {
      return 0;
    }
    for (i = 0; i < 3; i++) {
      if (((char)c->supplies[i] < -0xf) || (0xf < (char)c->supplies[i]) ||
         ((char)c->demands[i] < -0xf) || (0xf < (char)c->demands[i])) {
        return 0;
      }
    }
    for (i = 0; i < c->trade_routes; i++) {
      if ((c->route_partner[i] < 0) || (0xff < c->route_partner[i]) ||
         (0xf < (char)c->route_goods[i])) {
        return 0;
      }
    }
  }
  return 1;
}

/* 004732a6 auto */
i32 __cdecl save_read_units_cities(int version,FILE *file)

{
  size_t nread;
  iptr unit;
  
  /* BF-091: reject serialized counts before any record read or ID fixup.
   * Use this port's backing (2048 units, 256 cities), not another build's limits. */
  if (game.num_units < 0 || game.num_units > (int)(sizeof power_graph_history_block.units / sizeof(Unit)) ||
      game.num_cities < 0 || game.num_cities > (int)(sizeof cities / sizeof(City))) {
    return 0;
  }
  if (version < 0x2a) {
    if (version == 0x29) {
      for (unit = 0; unit < game.num_units; unit++) {
        nread = _fread(units + unit,0x1e,1,file);
        if (nread == 0) {
          return 0;
        }
      }
      if ((game.num_cities != 0) &&
         (nread = _fread(cities,game.num_cities * 0x58,1,file), nread == 0)) {
        return 0;
      }
      for (unit = 0; unit < game.num_units; unit++) {
        if (units[unit].id != 0) {
          units[unit].id = next_ids.unit;
          next_ids.unit++;
        }
      }
      for (unit = 0; unit < game.num_cities; unit++) {
        if (cities[unit].id != 0) {
          cities[unit].id = next_ids.city;
          next_ids.city++;
        }
      }
    }
    else {
      for (unit = 0; unit < game.num_units; unit++) {
        nread = _fread(units + unit,0x1a,1,file);
        if (nread == 0) {
          return 0;
        }
      }
      for (unit = 0; unit < game.num_cities; unit++) {
        nread = _fread(cities + unit,0x54,1,file);
        if (nread == 0) {
          return 0;
        }
      }
      for (unit = 0; unit < game.num_units; unit++) {
        units[unit].id = next_ids.unit;
        next_ids.unit++;
      }
      for (unit = 0; unit < game.num_cities; unit++) {
        cities[unit].id = next_ids.city;
        next_ids.city++;
      }
    }
  }
  else {
    if ((game.num_units != 0) &&
       (nread = _fread(units,(int)game.num_units << 5,1,file), nread == 0)) {
      return 0;
    }
    if ((game.num_cities != 0) &&
       (nread = _fread(cities,game.num_cities * 0x58,1,file), nread == 0)) {
      return 0;
    }
    for (unit = 0; unit < game.num_units; unit++) {
      if (units[unit].id != 0) {
        units[unit].id = next_ids.unit;
        next_ids.unit++;
      }
    }
    for (unit = 0; unit < game.num_cities; unit++) {
      if (cities[unit].id != 0) {
        cities[unit].id = next_ids.city;
        next_ids.city++;
      }
    }
  }
  return save_records_valid();
}

/* BF-134..136, BF-172, BF-176, BF-201, BF-230, BF-291 (Peter Johnson's
   ledger): what indexes the game's tables in the game record and the civs a
   save brings, as the game writes them: the difficulty, barbarian activity,
   the civs whose turn it is and who plays here (the local one in a game on
   this computer: hot seat and e-mail games), the power ranks, the AI
   opponents counted from the civs alive, and each civ's tribe, research,
   government and names (each with its NUL). 0 refuses the file. */
static int save_game_record_valid(void)
{
  int civ;
  int i;

  if ((5 < game.difficulty) || (3 < game.barbarian_activity) ||
     ((uint)(int)game.turn_civ > 7) || ((uint)(int)game.player_civ > 7) ||
     ((game.multiplayer < 3) && ((uint)(int)game.saved_current_civ > 7)) ||
     (7 < game.best_human_rank) || (7 < game.worst_human_rank) || (6 < game.num_enemies)) {
    return 0;
  }
  for (civ = 0; civ < 8; civ++) {
    if (((uint)(int)civs[civ].tribe > 0x14) || (civs[civ].researching < -1) ||
       (99 < civs[civ].researching) || (6 < civs[civ].government) ||
       (save_text_terminated(civ_names[civ].leader_name,0x18) == 0) ||
       (save_text_terminated(civ_names[civ].tribe_name,0x18) == 0) ||
       (save_text_terminated(civ_names[civ].adjective,0x18) == 0)) {
      return 0;
    }
    for (i = 0; i < 7; i++) {
      if (save_text_terminated(civ_names[civ].titles[i],0x18) == 0) {
        return 0;
      }
    }
  }
  return 1;
}

/* 00473660 auto */
i32 __cdecl save_load_gpk(int version,FILE *file)

{
  size_t nread;
  int alive_count;
  uint enemy_count;
  iptr civ;
  int civ2;
  i8 pad[4];
  
  if (version < 0x28) {
    if ((version != 0x26) && (version != 0x27)) {
      return 0;
    }
    for (civ2 = 0; civ2 < 8; civ2++) {
      for (civ = 0; civ < 0x3e; civ++) {
        *(i8 *)&civs[civ2].units_active[civ] = 0;
        *(i8 *)&civs[civ2].units_lost[civ] = 0;
        *(i8 *)&civs[civ2].units_in_production[civ] = 0;
      }
    }
    for (civ = 0; civ < 100; civ++) {
      game.first_discoverer[civ] = '\0';
      game.advance_known_by[civ] = '\0';
      for (civ2 = 0; civ2 < 8; civ2++) {
        *(i8 *)&civs[civ2].advance_source[civ] = 0;
      }
    }
    for (civ2 = 0; civ2 < 8; civ2++) {
      for (civ = 0; civ < 0xd; civ++) {
        *(i8 *)&civs[civ2].advances[civ] = 0;
      }
    }
    nread = _fread(&game,0x36,1,file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(game.first_discoverer,0x5d,1,file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(game.advance_known_by,0x5d,1,file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(game.wonder_city,0x4c,1,file);
    if (nread == 0) {
      return 0;
    }
    alive_count = bits_count8((uint)game.civs_alive);
    enemy_count = alive_count - 1;
    if ((game.civs_alive & 1) != 0) {
      enemy_count = alive_count - 2;
    }
    if (game.num_enemies != enemy_count) {
      dbg_logf_int2((const char *)&str_load_gpk_fixing_up_game_enemies_d_incons_2,(uint)game.num_enemies,(uint)game.civs_alive);
      game.num_enemies = (u8)enemy_count;
    }
    nread = _fread(civ_names,0x790,1,file);
    if (nread == 0) {
      return 0;
    }
    for (civ = 0; civ < 8; civ++) {
      nread = _fread(civs + civ,0x58,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].advances,0xc,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(&civs[civ].num_military_units,0xe,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].advance_source,0x5d,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].units_active,0x36,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].units_lost,0x36,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].units_in_production,0x36,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(pad,1,1,file);
      if (nread == 0) {
        return 0;
      }
      nread = _fread(civs[civ].continent_military,0x402,1,file);
      if (nread == 0) {
        return 0;
      }
    }
  }
  else {
    nread = _fread(&game,0x14a,1,file);
    if (nread == 0) {
      return 0;
    }
    alive_count = bits_count8((uint)game.civs_alive);
    enemy_count = alive_count - 1;
    if ((game.civs_alive & 1) != 0) {
      enemy_count = alive_count - 2;
    }
    if (game.num_enemies != enemy_count) {
      dbg_logf_int2((const char *)&str_load_gpk_fixing_up_game_enemies_d_incons,(uint)game.num_enemies,(uint)game.civs_alive);
      game.num_enemies = (u8)enemy_count;
    }
    nread = _fread(civ_names,0x790,1,file);
    if (nread == 0) {
      return 0;
    }
    nread = _fread(civs,0x2ca0,1,file);
    if (nread == 0) {
      return 0;
    }
  }
  return save_game_record_valid();
}

/* 00473c12 auto */
i32 __cdecl save_write_str(char *str,FILE *file)

{
  int rc;
  i32 result;
  
  rc = _fputs(str,file);
  if (rc == -1) {
    result = 0xffffffff;
  }
  else {
    rc = _fputc(0,file);
    if (rc == -1) {
      result = 0xffffffff;
    }
    else {
      result = 0;
    }
  }
  return result;
}

/* 00473c68 auto */
/* WARNING: Unable to use type for symbol next_len */

int __cdecl save_read_event_str(FILE *file)

{
  char *cur;
  int saved_ch;
  size_t str_len;
  int next_len;
  iptr len;
  char buf[260];
  int ch;
  int result;
  
  len = 0;
  do {
    ch = _fgetc(file);
    saved_ch = ch;
    if (ch == -1) {
      return 0;
    }
    if (0xfe < len) break;
    next_len = len + 1;
    buf[len] = (char)ch;
    ch = saved_ch;
    cur = buf + len;
    len = next_len;
  } while (*cur != '\0');
  buf[len] = '\0';
  str_len = _strlen(buf);
  result = memblock_alloc((MemBlock *)&unit_types_block.str_empty_188,str_len + 1);
  if (result == 0) {
    error_report(0xfffffff7,3,0,0,0);
    result = 0;
  }
  else {
    crt_strcpy((char *)((void *)result),buf);
  }
  return result;
}

/* 00473d5e auto */
void __cdecl save_set_file_ext(int is_scenario)

{
  if (is_scenario == 0) {
    if (game.multiplayer == '\0') {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_sav_str));
    }
    else if (game.multiplayer == 1) {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_hot_str));
    }
    else if (game.multiplayer == 2) {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_eml_str));
    }
    else if ((((game.multiplayer == 3) || (game.multiplayer == 4)) ||
             (game.multiplayer == 5)) || (game.multiplayer == 6)) {
      crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_net_str));
    }
  }
  else {
    crt_strcpy(((i8 *)&grey_palette_block.save_file_path),((i8 *)&save_ext_scn_str));
  }
  return;
}

/* (port) BF-038 (Peter Johnson's ledger): an entry's description is its
   label, cut to 48 characters so that the four entries, their patterns and the
   list's end fit the 260 bytes save_file_filter clears (the original appended
   the whole label) */
static void save_filter_append_label(char *buf,int label_id)
{
  str_append_max(buf,label_string(label_table.string_ids[label_id]),0x31);
}

/* 00473e55 auto */
i8 * __cdecl save_file_filter(int save_only,int is_scenario,int is_mp)

{
  size_t len;
  char *cursor;
  
  if (is_scenario == 0) {
    if (is_mp == 0) {
      _memset(((i8 *)&grey_palette_block.save_file_filter_buf),0,0x104);
      cursor = save_filter_add_entry((uint)game.multiplayer,(char *)&grey_palette_block.save_file_filter_buf);
      if (save_only == 0) {
        if (game.multiplayer != '\0') {
          cursor = save_filter_add_entry(0,cursor);
        }
        if (game.multiplayer < 3) {
          cursor = save_filter_add_entry(3,cursor);
        }
        if (game.multiplayer != 1) {
          save_filter_add_entry(1,cursor);
        }
      }
    }
    else {
      _memset(((i8 *)&grey_palette_block.save_file_filter_buf),0,0x104);
      save_filter_append_label((char *)&grey_palette_block.save_file_filter_buf,0x105);
      crt_strcat(((i8 *)&grey_palette_block.save_file_filter_buf),str_mp);
      len = _strlen(((i8 *)&grey_palette_block.save_file_filter_buf));
      crt_strcat((char *)((void *)(len + (iptr)(((u8 *)&grey_palette_block.save_file_filter_buf + 1)))),((i8 *)&save_filter_mp_pattern_str));
    }
  }
  else {
    _memset(((i8 *)&grey_palette_block.save_file_filter_buf),0,0x104);
    save_filter_append_label((char *)&grey_palette_block.save_file_filter_buf,0x19f);
    crt_strcat(((i8 *)&grey_palette_block.save_file_filter_buf),str_scn);
    len = _strlen(((i8 *)&grey_palette_block.save_file_filter_buf));
    crt_strcat((char *)((void *)(len + (iptr)(((u8 *)&grey_palette_block.save_file_filter_buf + 1)))),str_scn_2);
  }
  return &grey_palette_block.save_file_filter_buf[0];
}

/* 00473ff2 auto */
char * __cdecl save_filter_add_entry(i32 kind,char *buf)

{
  size_t len;
  char *next;
  
  switch(kind) {
  case 0:
    save_filter_append_label(buf,0xee);
    crt_strcat((char *)((void *)buf),str_sav);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_sav_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  case 1:
    save_filter_append_label(buf,0x357);
    crt_strcat((char *)((void *)buf),str_hot);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_hot_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  case 2:
    save_filter_append_label(buf,0x35a);
    crt_strcat((char *)((void *)buf),str_eml);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_eml_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  case 3:
  case 4:
  case 5:
  case 6:
    save_filter_append_label(buf,0x35d);
    crt_strcat((char *)((void *)buf),str_net);
    len = _strlen(buf);
    next = (buf + len + 1);
    crt_strcat(next,str_net_2);
    len = _strlen(next);
    next = next + len + 1;
    break;
  default:
    next = (char *)0x0;
  }
  return next;
}

/* 004741be auto */
i32 __cdecl save_write_game(const char *file_name,int is_scenario)

{
  char *dot;
  size_t nwritten;
  int rc;
  int count;
  uint saved_civ;
  iptr civ;
  iptr advance;
  ScenarioEvent *event;
  i32 result;
  u8 saved_humans;
  i16 version[2];
  int leader;
  char path[260];
  
  result = 1;
  if (game.multiplayer == 1) {
    saved_humans = game.human_players;
    game.human_players = net.hotseat_human_players;
  }
  save_set_file_ext(is_scenario);
  crt_strcpy(path,(const char *)((const void *)file_name));
  dot = _strchr(path,0x2e);
  if (dot == (char *)0x0) {
    crt_strcat(path,((i8 *)&grey_palette_block.save_file_path));
  }
  ScopedFile file(_fopen(path,((i8 *)&save_write_mode)));
  ON_SCOPE_EXIT(if (game.multiplayer == 1) { game.human_players = saved_humans; });
  if (file != (FILE *)0x0) {
    save_write_cstr_eof((const char *)str_ptr_civilize,file);
    game.saved_current_civ = *(i8 *)&human_civ;
    version[0] = 0x2c;
    nwritten = _fwrite(version,2,1,file);
    if (nwritten != 0) {
      if (is_scenario == 0) {
        game.game_flags = game.game_flags & ~GAME_SAVED_AS_SCENARIO;
      }
      else {
        game.game_flags = game.game_flags | GAME_SAVED_AS_SCENARIO;
        for (advance = 0; advance < ADV_COUNT; advance++) {
          game.advance_known_by[advance] = '\0';
          for (civ = 1; civ < 8; civ++) {
            rc = civ_has_advance(civ,advance);
            if (rc != 0) {
              game.advance_known_by[advance] =
                   game.advance_known_by[advance] | (byte)(1 << ((byte)civ & 0x1f));
            }
          }
          if (game.advance_known_by[advance] != '\0') {
            rc = bits_count8((int)(char)game.advance_known_by[advance]);
            if (rc == 1) {
              for (civ = 1; civ < 8; civ++) {
                rc = civ_has_advance(civ,advance);
                if (rc != 0) {
                  game.first_discoverer[advance] = (byte)civ;
                }
              }
            }
            else {
              game.first_discoverer[advance] = 8;
            }
          }
        }
        for (civ = 1; civ < 8; civ++) {
          civs[civ].flags = civs[civ].flags & ~(CIVF_REVOLUTION | CIVF_GOV_CHANGED | CIVF_FREE_ADVANCE | CIVF_UNUSED_40);
        }
      }
      if ((is_scenario != 0) || ((game.game_flags & GAME_SCENARIO) != 0)) {
        for (civ = 1; civ < 8; civ++) {
          if (-1 < leaders[civs[civ].tribe].adjective) {
            str_clear((char *)(civ * 0xf2 + (iptr)((u8 *)&civ_names + 50)));
            str_append_string_id(civ_names[civ].adjective,
                               (int)leaders[civs[civ].tribe].adjective);
          }
          if (-1 < leaders[civs[civ].tribe].plural) {
            str_clear((char *)(civ * 0xf2 + (iptr)((u8 *)&civ_names + 26)));
            str_append_string_id(civ_names[civ].tribe_name,
                               (int)leaders[civs[civ].tribe].plural);
          }
          if (-1 < leaders[civs[civ].tribe].leader_name) {
            str_clear((char *)(civ * 0xf2 + (iptr)((u8 *)&civ_names + 2)));
            str_append_string_id(civ_names[civ].leader_name,
                               (int)leaders[civs[civ].tribe].leader_name);
          }
        }
      }
      nwritten = _fwrite(&game,0x14a,1,file);
      if (((((((nwritten != 0) && (nwritten = _fwrite(civ_names,0x790,1,file), nwritten != 0)) &&
             (nwritten = _fwrite(civs,0x2ca0,1,file), nwritten != 0)) &&
            ((rc = map_write(file,0), rc == 0 &&
             (nwritten = _fwrite(map_data.coarse[0],(int)map.quarter_w * (int)map.quarter_h,1,file),
             nwritten != 0)))) &&
           (nwritten = _fwrite(map_data.coarse[1],(int)map.quarter_w * (int)map.quarter_h,1,file),
           nwritten != 0)) &&
          ((nwritten = _fwrite(((i8 *)&landmass_table_block.landmass_table),0x400,1,file), nwritten != 0 &&
           ((game.num_units == 0 ||
            (nwritten = _fwrite(units,(int)game.num_units << 5,1,file), nwritten != 0)))))) &&
         ((game.num_cities == 0 ||
          (nwritten = _fwrite(cities,game.num_cities * 0x58,1,file), nwritten != 0)))) {
        for (leader = 0; leader < 0x15; leader++) {
          nwritten = _fwrite(&leaders[leader].times_used,1,1,file);
          if (((nwritten == 0) ||
              (nwritten = _fwrite(&leaders[leader].city_name_index,1,1,file), nwritten == 0)) ||
             (nwritten = _fwrite(&leaders[leader].female,1,1,file), nwritten == 0))
          return result;
        }
        nwritten = _fwrite(((i16 *)&map_cursor.x),2,1,file);
        if ((nwritten != 0) && (nwritten = _fwrite(((i16 *)&map_cursor.y),2,1,file), nwritten != 0)) {
          if (game.multiplayer == '\0') {
            nwritten = _fwrite(leaders + civs[game.player_civ].tribe,0x30,1,file);
            if (nwritten == 0) return result;
          }
          else {
            civ = (iptr)game.player_civ;
            for (leader = 0; leader < 8; leader++) {
              if ((1 << ((byte)leader & 0x1f) & (uint)game.human_players) != 0) {
                game.player_civ = (byte)leader;
                nwritten = _fwrite(leaders + civs[(char)(byte)leader].tribe,0x30,1,file);
                if (nwritten == 0) {
                  game.player_civ = (byte)civ;
                  return result;
                }
              }
            }
            game.player_civ = (byte)civ;
          }
          nwritten = _fwrite(((i8 *)&power_graph_history_block.power_graph_history),0x4b0,1,file);
          if (nwritten != 0) {
            saved_screen_rect_block.saved_screen_rect[0] = caption_small_font_block.screen_rect[0];
            saved_screen_rect_block.saved_screen_rect_top = caption_small_font_block.screen_rect_top;
            saved_screen_rect_block.saved_screen_rect_right = caption_small_font_block.ui_screen_rect_right;
            saved_screen_rect_block.saved_screen_rect_bottom = caption_small_font_block.ui_screen_rect_bottom;
            save_pack_layout_rects();
            nwritten = _fwrite(((i16 *)&grey_palette_block.save_layout_record),0x6a,1,file);
            if (nwritten != 0) {
              for (leader = 0; leader < 8; leader++) {
                nwritten = _fwrite(&map_views_block.map_view_recs[leader].open,2,1,file);
                if (((nwritten == 0) ||
                    (nwritten = _fwrite(&map_views_block.map_view_recs[leader].track_moves,2,1,file), nwritten == 0))
                   || ((nwritten = _fwrite(((i16 *)&map_views_block.map_view_center_x) + leader * 0x1f8,2,1,file), nwritten == 0
                       || ((nwritten = _fwrite(((i16 *)&map_views_block.mapview_center_y) + leader * 0x1f8,2,1,file),
                           nwritten == 0 ||
                           (nwritten = _fwrite(((i16 *)&map_views_block.mapview_zoom) + leader * 0x1f8,2,1,file),
                           nwritten == 0)))))) return result;
              }
              if ((((is_scenario == 0) && ((game.game_flags & GAME_SCENARIO) == 0)) ||
                  (nwritten = _fwrite(((i16 *)&scenario.flags),100,1,file), nwritten != 0)) &&
                 (nwritten = _fwrite(((i16 *)&destroyed_civs.count),0x152,1,file), nwritten != 0)) {
                if (game.multiplayer != '\0') {
                  saved_civ = (uint)(*(byte *)&saved_turn.civ);
                  (*(byte *)&saved_turn.civ) = (*(byte *)&net.mp_turn_civ);
                  nwritten = _fwrite(((i8 *)&civ_view.saved_cursor_x),0x494,1,file);
                  if (nwritten == 0) return result;
                  (*(byte *)&saved_turn.civ) = (byte)saved_civ;
                }
                if ((scenario_events_loaded != 0) && (is_scenario == 0)) {
                  rc = _fputs(((i8 *)&save_events_tag_str),file);
                  if (rc == -1) return result;
                  count = 0;
                  for (event = (ScenarioEvent *)(void *)unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
                      event = (ScenarioEvent *)(void *)event->next) {
                    count++;
                  }
                  nwritten = _fwrite(&count,4,1,file);
                  if (nwritten == 0) return result;
                  for (event = (ScenarioEvent *)(void *)unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
                      event = (ScenarioEvent *)(void *)event->next) {
                    nwritten = _fwrite(event,0x1bc,1,file);
                    if (nwritten == 0) return result;
                  }
                  for (event = (ScenarioEvent *)(void *)unit_types_block.events_list_head; event != (ScenarioEvent *)0x0;
                      event = (ScenarioEvent *)(void *)event->next) {
                    if (((((event->unit_name != 0) &&
                          (rc = save_write_str(event->unit_name,
                                                      file), rc == -1)) ||
                         ((event->city_name != 0 &&
                          (rc = save_write_str(event->city_name,
                                                      file), rc == -1)))) ||
                        ((event->who_name != 0 &&
                         (rc = save_write_str(event->who_name,
                                                     file), rc == -1)))) ||
                       ((event->whom_name != 0 &&
                        (rc = save_write_str(event->whom_name,
                                                    file), rc == -1))))
                    return result;
                    count = 0;
                    while ((count < 0x14 &&
                           (event->text[count] != 0))) {
                      rc = save_write_str(event->text[count],
                                                 file);
                      if (rc == -1) return result;
                      count++;
                    }
                    if ((((((event->move_owner_name != 0) &&
                           (rc = save_write_str(event->move_owner_name,
                                                       file), rc == -1)) ||
                          ((event->move_unit_name != 0 &&
                           (rc = save_write_str(event->move_unit_name,
                                                       file), rc == -1)))) ||
                         (((event->aggression_whom_name != 0 &&
                           (rc = save_write_str(event->aggression_whom_name,
                                                       file), rc == -1)) ||
                          ((event->aggression_who_name != 0 &&
                           (rc = save_write_str(event->aggression_who_name,
                                                       file), rc == -1)))))) ||
                        ((((event->create_owner_name != 0 &&
                           (rc = save_write_str(event->create_owner_name,
                                                       file), rc == -1)) ||
                          ((event->create_unit_name != 0 &&
                           (rc = save_write_str(event->create_unit_name,
                                                       file), rc == -1)))) ||
                         (((event->create_homecity != 0 &&
                           (rc = save_write_str(event->create_homecity,
                                                       file), rc == -1)) ||
                          ((event->field_140 != 0 &&
                           (rc = save_write_str(event->field_140,
                                                       file), rc == -1)))))))) ||
                       ((((event->field_148 != 0 &&
                          (rc = save_write_str(event->field_148,
                                                      file), rc == -1)) ||
                         ((event->money_receiver_name != 0 &&
                          (rc = save_write_str(event->money_receiver_name,
                                                      file), rc == -1)))) ||
                        ((event->wave_file != 0 &&
                         (rc = save_write_str(event->wave_file,
                                                     file), rc == -1))))))
                    return result;
                  }
                }
                result = 0;
                if ((file.close() != 0) && bugfix_ui()) {
                  /* B28: the end of the file could not be written (a full disk): a
                     failed save, not a saved game cut short (docs/BUGS.md) */
                  result = 1;
                }
              }
            }
          }
        }
      }
    }
  }
  return result;
}

/* 00475666: written by hand in src/game/retvals.cpp */

/* 0047758c auto */
void __cdecl game_save_dialog(int is_scenario)

{
  uptr desc_len;
  uptr pattern_len;
  i32 leader_name;
  size_t name_len;
  char *title;
  int result;
  char *name_arg;
  i8 *filter_arg;
  i8 *ext_arg;
  i32 flag_arg;
  int abs_year_rem_neg;
  int abs_year_rem_pos;
  int abs_year_div;
  int abs_year;
  _SYSTEMTIME local_time;
  char num_buf[31];
  char file_name_buf[261];
  char filter_text[0x120];  /* BF-038 (Peter Johnson's ledger): the original's
                              260, less than its label and patterns */
  char *str_ptr;
  
  if (civ_saved_cursor_x_block.game_setup_complete != 0) {
    mp_turn_timer_stop(1);
    do {
      save_set_file_ext(is_scenario);
      str_clear((char *)filter_text);
      str_append_label(filter_text,(-(uint)(is_scenario == 0) & 0xffffff4f) + 0x19f);
      str_append_space(filter_text);
      str_append_lparen(filter_text);
      str_append((char *)filter_text,(const char *)&str_empty_94);
      str_append((char *)filter_text,(const char *)&grey_palette_block.save_file_ext_bare);
      str_append_rparen(filter_text);
      desc_len = _strlen(filter_text);
      str_ptr = (filter_text + 1) + desc_len;
      str_clear(str_ptr);
      str_append(str_ptr,(const char *)&str_empty_95);
      str_append(str_ptr,(const char *)&grey_palette_block.save_file_ext_bare);
      pattern_len = _strlen(str_ptr);
      str_ptr = str_ptr + pattern_len + 1;
      str_clear(str_ptr);
      str_clear((file_name_buf + 1));
      leader_name = (i32)(civ_leader_name(human_civ[0]));
      crt_strcat(file_name_buf + 1,(const char *)((const void *)leader_name));
      file_name_buf[3] = 0;
      str_append((file_name_buf + 1),(const char *)&str_empty_96);
      str_ptr = file_name_buf;
      while (str_ptr = str_ptr + 1, *str_ptr != '\0') {
        if ((31 < *str_ptr) && (*str_ptr < 'A')) {
          *str_ptr = '_';
        }
      }
      text_clear();
      if (game.year < 0) {
        text_append((const char *)&str_b);
      }
      else {
        text_append((const char *)&str_a);
      }
      str_append_space((file_name_buf + 1));
      name_len = _strlen(file_name_buf + 1);
      file_name_buf[name_len] = textbuf.buf[0];
      if (game.year < 1) {
        abs_year = ~(int)game.year + 1;
      }
      else {
        abs_year = (int)game.year;
      }
      str_append_int((file_name_buf + 1),abs_year);
      if ((game.multiplayer == 2) && (is_scenario == 0)) {
        GetLocalTime(&local_time);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_date_sep_str));
        __itoa((uint)(*(i32 *)((char *)&local_time + 0)) >> 0x10,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_month_day_sep_str));
        __itoa((uint)local_time.wDay,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_day_year_sep_str));
        __itoa((*(i32 *)((char *)&local_time + 0)) & 0xffff,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_time_sep_str));
        __itoa((*(i32 *)((char *)&local_time + 8)) & 0xffff,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_hour_min_sep_str));
        __itoa((uint)(*(i32 *)((char *)&local_time + 8)) >> 0x10,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
        crt_strcat(file_name_buf + 1,((i8 *)&save_name_min_sec_sep_str));
        __itoa((*(i32 *)((char *)&local_time + 12)) & 0xffff,num_buf,10);
        crt_strcat(file_name_buf + 1,num_buf);
      }
      crt_strcat(file_name_buf + 1,((i8 *)&save_name_ext_dot_str));
      crt_strcat(file_name_buf + 1,((i8 *)&grey_palette_block.save_file_ext_bare));
      __strlwr(file_name_buf + 1);
      if (((is_scenario != 0) || ((game.game_flags & GAME_SCENARIO) != 0)) && (scenario.year_increment[0] < 0)) {
        str_clear((file_name_buf + 1));
        if ((int)game.year / 0xc < 1) {
          abs_year_div = ~((int)game.year / 0xc) + 1;
        }
        else {
          abs_year_div = (int)game.year / 0xc;
        }
        str_append_int((file_name_buf + 1),abs_year_div);
        if (game.year < 0) {
          if ((int)game.year % 0xc < 1) {
            abs_year_rem_neg = ~((int)game.year % 0xc) + 1;
          }
          else {
            abs_year_rem_neg = (int)game.year % 0xc;
          }
          /* BF-038 (Peter Johnson's ledger): the month's label cut to fit
             the name's 256 bytes with its extension */
          str_append_max(file_name_buf + 1,
                         label_string(label_table.string_ids[0x1af - abs_year_rem_neg]),0xf0);
        }
        else {
          if ((int)game.year % 0xc < 1) {
            abs_year_rem_pos = ~((int)game.year % 0xc) + 1;
          }
          else {
            abs_year_rem_pos = (int)game.year % 0xc;
          }
          str_append_max(file_name_buf + 1,  /* BF-038 */
                         label_string(label_table.string_ids[abs_year_rem_pos + 0x1a4]),0xf0);
        }
        crt_strcat(file_name_buf + 1,((i8 *)&grey_palette_block.save_file_path));
        __strlwr(file_name_buf + 1);
      }
      __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
      save_file_filter(1,is_scenario,0);
      flag_arg = 1;
      leader_name = 0;
      ext_arg = ((i8 *)&grey_palette_block.save_file_ext_bare);
      filter_arg = ((i8 *)&grey_palette_block.save_file_filter_buf);
      name_arg = file_name_buf + 1;
      title = label_string(label_table.string_ids[0x1a0 + (-(uint)(is_scenario == 0) & 0xffffff4f)]
                                );
      result = ui_file_name_dialog(&main_frame_wnd_block.main_frame_wnd,(const char *)title,name_arg,filter_arg,ext_arg,leader_name,flag_arg);
      if (result != 0) {
        __chdir(((i8 *)&game_dirs.home));
        result = save_write_game((file_name_buf + 1),is_scenario);
        if (result == 0) {
          if (is_scenario == 0) {
            if (game.multiplayer == 2) {
              text_set_string_arg(0,(file_name_buf + 1));
              popup_game((const char *)&str_emailsaved);
            }
            else {
              title = (char *)civ_leader_title(human_civ[0]);
              text_set_string_arg(0,title);
              title = civ_leader_name(human_civ[0]);
              text_set_string_arg(1,title);
              title = civ_name_plural(human_civ[0]);
              text_set_string_arg(2,title);
              text_clear();
              text_append_year((int)game.year);
              text_set_string_arg(3,(const char *)&textbuf.buf);
              text_set_label_arg(4,difficulty_names[game.difficulty]);
              popup_game((const char *)&str_saveok);
            }
          }
          else {
            text_set_string_arg(0,(const char *)&scenario.name);
            popup_game((const char *)&str_scenok);
          }
        }
        else {
          text_set_string_arg_b(0,(const char *)(((i8 * *)&str_ptr_no_error))[crt_errno]);
          popup_game((const char *)&str_saveerror);
        }
        break;
      }
      __chdir(((i8 *)&game_dirs.home));
    } while (game.multiplayer == 2);
    mp_turn_timer_resume(1);
  }
  return;
}

/* 00477d8c auto */
int __cdecl game_load_dialog(int is_scenario,i32 unused_arg,int show_title)

{
  size_t str_len;
  char *str_arg;
  int result;
  char *ext_match;
  HANDLE find_handle;
  BOOL found_next;
  i8 *name_arg;
  i8 *filter_arg;
  i8 *ext_arg;
  i32 flag_arg;
  i32 flag_arg2;
  i32 cheat_checked;
  uint pick_index;
  _WIN32_FIND_DATAA find_data;
  CHAR alt_pattern[260];
  int alt_count;
  char *slash_ptr;
  iptr unit;
  i8 file_path[260];
  int failed;
  char filter_text[0x120];  /* BF-038 (Peter Johnson's ledger): the original's
                              260, less than its label and patterns */
  GamePalette palette;
  i8 events_path[260];
  char *str_ptr;
  int view_index;
  char upper_path[260];
  int load_error;
  
  failed = 1;
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  if (is_scenario == 0) {
    save_set_file_ext(0);
    net.loaded_file_kind = 1;
  }
  else {
    save_set_file_ext(1);
    net.loaded_file_kind = 2;
  }
  str_clear((char *)filter_text);
  str_append_label(filter_text,(-(uint)(is_scenario == 0) & 0xffffff4f) + 0x19f);
  str_append_space(filter_text);
  str_append_lparen(filter_text);
  str_append((char *)filter_text,(const char *)&str_empty_97);
  str_append((char *)filter_text,(const char *)&grey_palette_block.save_file_ext_bare);
  str_append_rparen(filter_text);
  str_len = _strlen(filter_text);
  str_ptr = (filter_text + 1) + str_len;
  str_clear(str_ptr);
  str_append(str_ptr,(const char *)&str_empty_98);
  str_append(str_ptr,(const char *)&grey_palette_block.save_file_ext_bare);
  str_len = _strlen(str_ptr);
  str_ptr = str_ptr + str_len + 1;
  str_clear(str_ptr);
  crt_strcpy(file_path,((i8 *)&load_dialog_wildcard_str));
  crt_strcat(file_path,((i8 *)&grey_palette_block.save_file_ext_bare));
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  save_file_filter(0,is_scenario,0);
  flag_arg2 = 0;
  flag_arg = 1;
  ext_arg = ((i8 *)&grey_palette_block.save_file_ext_bare);
  filter_arg = ((i8 *)&grey_palette_block.save_file_filter_buf);
  name_arg = file_path;
  str_arg = label_string(label_table.string_ids[0x1a1 + (-(uint)(is_scenario == 0) & 0xffffff4f)]);
  result = ui_file_name_dialog(&main_frame_wnd_block.main_frame_wnd,(const char *)str_arg,name_arg,filter_arg,ext_arg,flag_arg,flag_arg2);
  if (result == 0) {
    __chdir(((i8 *)&game_dirs.home));
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
    for (view_index = 1; view_index < 8; view_index++) {
      if (map_views_block.map_view_recs[view_index].open != 0) {
        map_views_block.map_view_recs[view_index].open = 0;
        map_views_block.map_view_recs[view_index].wnd_destroy_with_surface();
      }
    }
    crt_strcpy(upper_path,file_path);
    __strupr(upper_path);
    ext_match = _strstr(upper_path,((i8 *)&game_load_scn_ext_str));
    if (ext_match != (char *)0x0) {
      crt_strcpy(upper_path,file_path);
      slash_ptr = _strrchr(upper_path,0x5c);
      slash_ptr++;
      *slash_ptr = '\0';
      crt_strcpy(alt_pattern,upper_path);
      crt_strcat(alt_pattern,str_alt);
      alt_count = 0;
      find_handle = FindFirstFileA(alt_pattern,&find_data);
      if (find_handle != 0xffffffff) {
        do {
          alt_count++;
          found_next = FindNextFileA(find_handle,&find_data);
        } while (found_next == 1);
        FindClose(find_handle);  /* (port) the original kept both searches open */
      }
      if (alt_count != 0) {
        result = _rand();
        pick_index = (uint)(result * (alt_count + 1)) >> 0xf;
        if (pick_index != 0) {
          find_handle = FindFirstFileA(alt_pattern,&find_data);
          while (pick_index = pick_index - 1, pick_index != 0) {
            FindNextFileA(find_handle,&find_data);
          }
          if (find_handle != 0xffffffff) {
            FindClose(find_handle);
          }
          crt_strcat(upper_path,find_data.cFileName);
          crt_strcpy(file_path,upper_path);
        }
      }
    }
    snd_play_effect(-0x6b,0,0,0);
    snd_play_effect(-0x6c,0,0,0);
    snd_play_effect(SND_MENUOK,0,0,0);
    load_error = save_read_game(file_path);
    if (load_error == 0) {
      ui_flush_input();
      if ((scenario_events_loaded == 0) && ((game.game_flags & GAME_SCENARIO) != 0)) {
        scenario_events_loaded = 0;
        __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
        crt_strcpy(events_path,str_events);
        crt_strcat(events_path,((i8 *)&lang_file_ext));
        result = file_exists((const char *)events_path);
        if (result != 0) {
          crt_strcpy(events_path,((i8 *)&textbuf.buf));
          ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
          result = ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->parse_file((const char *)&str_events_2,(const char *)&str_beginevents);
          if (result == 0) {
            events_compact();
            scenario_events_loaded = 1;
          }
          crt_strcpy(((i8 *)&textbuf.buf),events_path);
        }
      }
      if (scenario_events_loaded == 0) {
        ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->reset(50000);
      }
      game.game_options = prefs.game_options;
      snd_wave_set_buffer_count(8);
      snd_cd_detect_layout();
      if ((game.game_options & OPT_MUSIC) != 0) {
        if (scenario_events_loaded == 0) {
          snd_sync_music_option();
        }
        else {
          ((ScenarioEvents *)((i8 *)&unit_types_block.scenario_events))->check_scenario_loaded();
          snd_sync_music_option();
        }
      }
      if (show_title != 0) {
        ui_flush_input();
        result = file_exists((const char *)&str_title_gif);
        if ((result != 0) &&
           (result = ((DibSurface *)((i8 *)&scenario_title_surface_block.scenario_title_surface))->load_gif_file((const char *)&str_title_gif_2,10,0xc0,&palette),
           result != 0)) {
          main_frame_wnd_block.main_frame_wnd.set_bg_surface((DibSurface *)&scenario_title_surface_block.scenario_title_surface);
          main_frame_wnd_block.main_frame_wnd.set_bg_color(0x9e);
          main_frame_wnd_block.main_frame_wnd.maximize();
          wnd_flush_paint();
          input_flush_events();
        }
      }
      __chdir(((i8 *)&game_dirs.home));
      failed = 0;
      str_arg = (char *)civ_leader_title(human_civ[0]);
      text_set_string_arg(0,str_arg);
      str_arg = civ_leader_name(human_civ[0]);
      text_set_string_arg(1,str_arg);
      str_arg = civ_name_plural(human_civ[0]);
      text_set_string_arg(2,str_arg);
      text_clear();
      text_append_year((int)game.year);
      text_set_string_arg(3,(const char *)&textbuf.buf);
      text_set_label_arg(4,difficulty_names[game.difficulty]);
      if (((game.game_options & OPT_CHEAT_MODE) == 0) || (game.multiplayer != '\0')) {
        cheat_checked = 0;
      }
      else {
        cheat_checked = 1;
      }
      ((GameMenu *)((i8 *)&main_menu_block.main_menu))->set_item_checked(CMD_CHEAT_MODE,cheat_checked);
      if ((is_scenario == 0) && (game.multiplayer == '\0')) {
        popup_game((const char *)&str_loadok);
      }
      net.host_flag = 1;
      for (unit = 0; unit < game.num_units; unit++) {
        if (units[unit].id == 0) {
          units[unit].next_in_stack = -1;
          units[unit].prev_in_stack = -1;
        }
        else {
          unit_stack_validate(unit);
          if ((units[unit].x < 0) || (units[unit].y < 0)) {
            dbg_logf3((const char *)&str_stacked_unit_in_save_unit_d_type_d_whose,unit,(uint)units[unit].type,(int)units[unit].owner)
            ;
            unit_delete(unit,0);
          }
        }
      }
      text_clear();
      text_append((const char *)file_path);
    }
    else {
      switch(load_error) {
      case 1:
        popup_game((const char *)&str_loadnotsave);
        break;
      case 2:
        popup_game((const char *)&str_loadoldsave);
        break;
      case 3:
        text_set_string_arg_b(0,(const char *)(((i8 * *)&str_ptr_no_error))[crt_errno]);
        popup_game((const char *)&str_loadbadsave);
        break;
      case 4:
        text_set_string_arg_b(0,(const char *)(((i8 * *)&str_ptr_no_error))[crt_errno]);
        popup_game((const char *)&str_loadnewsave);
      }
    }
  }
  result = failed;
  if (failed != 0) {
    net.loaded_file_kind = 0;
  }
  return result;
}

/* 00497da0 auto */
char * __cdecl save_read_cstr(char *buf,FILE *file)

{
  int ch;
  i8 *dst;
  int i;
  
  dst = buf;
  ch = 1;
  for (i = 0; (ch != 0 && (i < 0x4f)); i++) {
    ch = _fgetc(file);
    *dst = (i8)ch;
    dst++;
  }
  _fgetc(file);
  return buf;
}

/* 00497e0f auto */
void __cdecl save_write_cstr_eof(const char *str,FILE *file)

{
  i32 ch;
  
  ch = 1;
  while (ch != 0) {
    ch = (int)*(char *)str;
    str++;
    _fputc(ch,file);
  }
  _fputc(0x1a,file);
  return;
}

/* 00498784 auto */
void __cdecl passwords_reset(void)

{
  byte sign;
  int rnd;
  int civ;
  iptr i;
  
  for (i = 0; i < 0xff; i++) {
    rnd = _rand();
    sign = (byte)(rnd >> 0x1f);
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] = (((byte)rnd ^ sign) - sign ^ sign) - sign;
  }
  for (civ = 0; civ < 8; civ++) {
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[civ * 0x20] = 0;
    map_views_block.civ_password_prompt_pending[civ] = 0;
    map_views_block.civ_has_password[civ] = 0;
  }
  passwords_scramble();
  return;
}

/* 0049882b auto */
void __cdecl passwords_refresh_flags(void)

{
  int civ;
  
  passwords_unscramble();
  for (civ = 0; civ < 8; civ++) {
    if ((((i8 *)&civ_saved_cursor_x_block.civ_passwords))[civ * 0x20] == '\0') {
      map_views_block.civ_password_prompt_pending[civ] = 0;
      map_views_block.civ_has_password[civ] = 0;
    }
    else {
      map_views_block.civ_password_prompt_pending[civ] = 1;
      map_views_block.civ_has_password[civ] = 1;
    }
  }
  passwords_scramble();
  return;
}

/* 004988b8 auto */
void __cdecl passwords_scramble(void)

{
  char prev_byte;
  iptr i;
  byte carry;
  
  prev_byte = civ_saved_cursor_x_block.civ_passwords_last_byte;
  for (i = 0; carry = prev_byte << 5, i < 0x100; i++) {
    prev_byte = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i];
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] =
         ((byte)((uint)(int)(((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] >> 3) & 0x1f | carry) ^ (byte)i
    ;
  }
  return;
}

/* 00498943 auto */
void __cdecl passwords_unscramble(void)

{
  uint carry;
  iptr i;
  char prev_byte;
  
  prev_byte = civ_saved_cursor_x_block.civ_passwords[0];
  for (i = 0xff; -1 < i; i--) {
    carry = (uint)prev_byte;
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] ^ (byte)i;
    prev_byte = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i];
    (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] = (((i8 *)&civ_saved_cursor_x_block.civ_passwords))[i] << 3 | (byte)(carry >> 5) & 7;
  }
  return;
}

/* 004989d3 auto */
void __cdecl password_dialog_set_title(Dialog *dialog,int civ,i32 label_id)

{
  size_t title_len;
  i32 title_mem;
  char title[0x120];  /* BF-040 (Peter Johnson's ledger): the original's 128,
                         less than a 24-byte name and a label */
  
  crt_strcpy(title,(const char *)(&civ_names[civ].leader_name));
  str_append_label(title,label_id);
  title_len = _strlen(title);
  title_mem = memblock_alloc((MemBlock *)dialog->string_pool,title_len + 1);
  dialog->title_text = title_mem;
  crt_strcpy((char *)((void *)dialog->title_text),title);
  return;
}

/* 004a70b0 auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl prefs_set_defaults(void)

{
  int has_memory;
  iptr i;
  
  (*(i16 *)&prefs.game_rules) = 0x3f;
  prefs.default_level = 0;
  if (999 < caption_font_block.screen_width) {
    prefs.default_level = 2;
  }
  prefs.difficulty = 0;
  prefs.tribe = 0;
  prefs.gender = 0;
  prefs.science_rate = 6;
  prefs.tax_rate = 4;
  prefs.enemies_choice = 0;
  prefs.map_size = 1;
  prefs.game_options = 0x3f3258;
  has_memory = mem_check_available(10000000);
  if (has_memory == 0) {
    prefs.game_options = prefs.game_options & 0xffdfffff;
  }
  (*(i16 *)&prefs.message_options) = 0;
  prefs.barbarity = 2;
  for (i = 0; i < 5; i++) {
    (((i16 *)&prefs.world_params))[i] = 0;
  }
  crt_strcpy(((i8 *)&prefs.text_field),((i8 *)&prefs_defaults_empty_str));
  prefs.rules_choice = 0;
  prefs.accelerated_start = 0;
  return;
}

/* 004a71bb auto */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl prefs_set_mp_defaults(void)

{
  iptr j;
  int i;
  
  menu_prefs.main_menu_choice[0] = 0;
  menu_prefs.multiplayer_type = 0;
  menu_prefs.hotseat_menu_choice = 0;
  menu_prefs.lan_menu_choice = 0;
  menu_prefs.mp_prefs_setting_1 = 0;
  menu_prefs.pbem_menu_choice = 0;
  (*(i16 *)&menu_prefs.net_protocol) = 0;
  menu_prefs.mp_open_game = 0;
  menu_prefs.mp_kill_on_retire = 0;
  menu_prefs.mp_show_human_starts = 0;
  menu_prefs.mp_chat_from_start = 0;
  menu_prefs.mp_simultaneous_moves = 0;
  menu_prefs.net_game_name[0] = 0;
  menu_prefs.net_player_name[0] = 0;
  menu_prefs.email_address[0] = 0;
  menu_prefs.net_ip_address[0] = 0;
  menu_prefs.turn_timer_choice = 0;
  crt_strcpy(((i8 *)&menu_prefs.turn_timer_custom),((i8 *)&prefs_turn_timer_default_str));
  menu_prefs.hotseat_players_choice = 0;
  _memset(((i8 *)&menu_prefs.parley_wnd_rect),0,0x10);
  _memset(((i8 *)&menu_prefs.chat_wnd_rect),0,0x10);
  menu_prefs.clear_chat_on_new_game = 0;
  menu_prefs.clear_chat_on_boot = 0;
  menu_prefs.mp_double_production = 0;
  menu_prefs.mp_double_moves = 0;
  menu_prefs.mp_prefs_setting_2 = 0;
  menu_prefs.mp_prefs_setting_3 = 0;
  menu_prefs.mp_prefs_setting_4 = 0;
  for (i = 0; i < 7; i++) {
    for (j = 0; j < 0x18; j++) {
      (((i8 *)&menu_prefs.mp_prefs_settings_table))[i * 0x18 + j] = 0;
    }
  }
  menu_prefs.mp_prefs_setting_5 = 0;
  return;
}

/* 004a733d auto */
void __cdecl prefs_load_civ2dat(void)

{
  FILE *_File;
  size_t read_count;
  
  _File = _fopen(str_ptr_civ2_dat,((i8 *)&civ2dat_read_mode_str));
  if (_File == (FILE *)0x0) {
    prefs_set_defaults();
    prefs_set_mp_defaults();
  }
  else {
    read_count = _fread(((i16 *)&prefs.game_rules),0x48,1,_File);
    if (read_count == 0) {
      prefs_set_defaults();
    }
    read_count = _fread(((i16 *)&menu_prefs.main_menu_choice),0x1bc,1,_File);
    /* BF-226 (Peter Johnson's ledger): the game, player, e-mail and address
       names end within their 32 bytes, else the defaults as for a short file */
    if ((read_count == 0) || (memchr(menu_prefs.net_game_name,0,0x20) == (void *)0x0) ||
       (memchr(menu_prefs.net_player_name,0,0x20) == (void *)0x0) ||
       (memchr(menu_prefs.email_address,0,0x20) == (void *)0x0) ||
       (memchr(menu_prefs.net_ip_address,0,0x20) == (void *)0x0)) {
      prefs_set_mp_defaults();
    }
    _fclose(_File);
  }
  return;
}

/* 004a73d9 auto */
int __cdecl prefs_save_civ2dat(void)

{
  FILE *_File;
  size_t write_count;
  int failed;
  
  failed = 1;
  _File = _fopen(str_ptr_civ2_dat,((i8 *)&civ2dat_write_mode_str));
  if (((_File != (FILE *)0x0) && (write_count = _fwrite(((i16 *)&prefs.game_rules),0x48,1,_File), write_count != 0)) &&
     (write_count = _fwrite(((i16 *)&menu_prefs.main_menu_choice),0x1bc,1,_File), write_count != 0)) {
    failed = 0;
  }
  if ((_File != (FILE *)0x0) && (_fclose(_File), failed != 0)) {
    crt_remove(str_ptr_civ2_dat);
  }
  return failed;
}
