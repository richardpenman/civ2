/* The game, cities: the city view (the picture of a city's buildings) and the wonder movies: functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 00454260 auto */
void __cdecl cityview_open(uptr city)

{
  int loaded;
  CityViewWindow *this2;
  
  this2 = game_new<CityViewWindow>();
  if (this2 != (CityViewWindow *)0x0) {
    loaded = this2->load(city);
    if (loaded != 0) {
      this2->run_modal();
    }
    if (this2 != (CityViewWindow *)0x0) {
      this2->scalar_destruct(1);
    }
  }
  return;
}

/* 00454354 auto */
CityViewWindow::CityViewWindow()

{
  u8 *slot;
  int i;
  
  this->ModalMenuWindow::construct();
  surface_wnd.SurfaceWindow::construct();
  surface_ctor(&backbuffer);
  cityview_palette.init();
  surface_ctor(&improvement_art_surface);
  surface_ctor(&wonder_art_surface);
  resource_dll.init();
  crt_vector_ctor_iterator((u8 *)&slot_sprites,0x3c,0x3c,sprite_init,sprite_destroy);
  crt_vector_ctor_iterator(small_building_sprites_a,0x3c,4,sprite_init,sprite_destroy)
  ;
  crt_vector_ctor_iterator(small_building_sprites_b,0x3c,4,sprite_init,sprite_destroy
              );
  crt_vector_ctor_iterator(large_building_sprites_a,0x3c,4,sprite_init,
               sprite_destroy);
  crt_vector_ctor_iterator(large_building_sprites_b,0x3c,4,sprite_init,
               sprite_destroy);
  surface_ctor(&icon_surface);
  title_font.init_empty();
  gfx_get_screen_rect((RECT *)&tax_rate_icon_sprites_block.cityview_screen_rect);
  cityview.scroll_x = (0x500 - tax_rate_icon_sprites_block.screen_rect_right) / 2;
  mem_copy_src_dst(&cityview_slot_x_table_block.cityview_slot_tables_init,(improvement_slot_tables),0x174);
  mem_copy_src_dst(&str_empty,(wonder_slot_tables),0x120);
  cityview.wnd = this;
  for (i = 0; i < 0x3c; i++) {
    slot = (u8 *)&slot_sprite_ptrs[i];
    slot[0] = '\0';
    slot[1] = '\0';
    slot[2] = '\0';
    slot[3] = '\0';
  }
  title_font.set_styled(0,0x24,1);
  SetRect((LPRECT)(title_rect),0,0,tax_rate_icon_sprites_block.screen_rect_right,0x28);
}

/* 00454699 auto */
CityViewWindow::~CityViewWindow()

{
  int seh_state;
  
  seh_state = seh_state & 0xff;
  (*((char *)&seh_state + 0)) = 0xd;
  (*(i32 *)&cityview.wnd) = 0;
  resource_dll.dll_handle_free();
  (*((char *)&seh_state + 0)) = 0xc;
  /* inlined unwind funclet 00454798 */ { title_font.destroy(); }
  (*((char *)&seh_state + 0)) = 0xb;
  /* inlined unwind funclet 004547a7 */ { surface_dtor(&icon_surface); }
  (*((char *)&seh_state + 0)) = 10;
  /* inlined unwind funclet 004547b6 */ { crt_vector_dtor_iterator((void *)large_building_sprites_b,0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 9;
  /* inlined unwind funclet 004547ce */ { crt_vector_dtor_iterator((void *)large_building_sprites_a,0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 8;
  /* inlined unwind funclet 004547e6 */ { crt_vector_dtor_iterator((void *)small_building_sprites_b,0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 7;
  /* inlined unwind funclet 004547fe */ { crt_vector_dtor_iterator((void *)small_building_sprites_a,0x3c,4,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 6;
  /* inlined unwind funclet 00454816 */ { crt_vector_dtor_iterator((void *)slot_sprites,0x3c,0x3c,sprite_destroy); }
  (*((char *)&seh_state + 0)) = 5;
  /* inlined unwind funclet 0045482e */ { resource_dll.free(); }
  (*((char *)&seh_state + 0)) = 4;
  /* inlined unwind funclet 0045483d */ { surface_dtor(&wonder_art_surface); }
  (*((char *)&seh_state + 0)) = 3;
  /* inlined unwind funclet 0045484c */ { surface_dtor(&improvement_art_surface); }
  (*((char *)&seh_state + 0)) = 2;
  /* inlined unwind funclet 0045485b */ { cityview_palette.destroy(); }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 0045486a */ { surface_dtor(&backbuffer); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 00454879 */ { surface_wnd.SurfaceWindow::destruct(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 00454888 */ { ModalMenuWindow::destruct(); }
  return;
}

/* 004548a9 auto */
i32 CityViewWindow::load(uptr city)

{
  int ok;
  u32 art_era;
  i32 result;
  DibSurface art_surface;
  int src_y;
  int i;
  int src_x;
  i8 path[128];
  
  surface_ctor(&art_surface);
  ON_SCOPE_EXIT(surface_dtor(&art_surface));
  city_id = city;
  str_clear((char *)path);
  str_append((char *)path,(const char *)&str_cv_dll);
  ok = file_resolve_path((const char *)path);
  if (ok == 0) {
    result = 0;
  }
  else {
    resource_dll.load((const char *)path);
    classify_site();
    art_era = civ_city_art_era(cities[city].owner);
    this->art_era = art_era;
    if (tax_rate_icon_sprites_block.screen_rect_right < 0x500) {
      (&backbuffer)->DibSurface::create(tax_rate_icon_sprites_block.screen_rect_right,0x1e0);
    }
    else {
      (&backbuffer)->DibSurface::create(0x500,0x1e0);
    }
    ok = improvement_art_surface.load_gif_resource(300,10,0xec,
                       &cityview_palette);
    if (ok == 0) {
      result = 0;
    }
    else {
      ok = wonder_art_surface.load_gif_resource(0x131,10,0xec,
                         &cityview_palette);
      if (ok == 0) {
        result = 0;
      }
      else {
        ok = art_surface.load_gif_resource(0x136,10,0xec,&cityview_palette);
        if (ok == 0) {
          result = 0;
        }
        else {
          src_x = 1;
          src_y = 1;
          for (i = 0; i < 4; i++) {
            large_building_sprites_a[i].grab_from_surface(&art_surface,9
                       ,src_x,src_y,0x9e,0x72);
            large_building_sprites_a[i].replace_color(7,9);
            src_x += 0x9f;
          }
          src_x = 1;
          src_y = 0x74;
          ok = civ_city_style_era(cities[city].owner);
          if (1 < ok) {
            src_y += 0x73;
          }
          for (i = 0; i < 4; i++) {
            large_building_sprites_b[i].grab_from_surface(&art_surface,9
                       ,src_x,src_y,0x9e,0x72);
            large_building_sprites_b[i].replace_color(7,9);
            src_x += 0x9f;
          }
          src_x = 1;
          src_y = 0x15a;
          for (i = 0; i < 4; i++) {
            small_building_sprites_a[i].grab_from_surface(&art_surface,9
                       ,src_x,src_y,0x7b,0x52);
            small_building_sprites_a[i].replace_color(7,9);
            src_x += 0x7c;
          }
          src_x = 1;
          src_y = 0x1ad;
          ok = civ_city_style_era(cities[city].owner);
          if (1 < ok) {
            src_y += 0x53;
          }
          for (i = 0; i < 4; i++) {
            small_building_sprites_b[i].grab_from_surface(&art_surface,9
                       ,src_x,src_y,0x7b,0x52);
            small_building_sprites_b[i].replace_color(7,9);
            src_x += 0x7c;
          }
          ok = load_icon_surface();
          if (ok == 0) {
            result = 0;
          }
          else {
            ok = create_windows();
            if (ok == 0) {
              result = 0;
            }
            else {
              ok = layout_buildings();
              if (ok == 0) {
                result = 0;
              }
              else {
                draw_buildings();
                result = 1;
              }
            }
          }
        }
      }
    }
  }
  return result;
}

/* 00454eb2 auto */
void CityViewWindow::run_modal()

{
  handle_show_raise();
  wnd_flush_paint();
  set_palette(&cityview_palette);
  surface_wnd.SurfaceWindow::show();
  wnd_flush_paint();
  surface_wnd.window.set_capture();
  if (2 < game.multiplayer) {
    surface_wnd.window.set_modal_idle_callback(net_modal_idle_poll);
  }
  surface_wnd.window.run_modal_loop();
  surface_wnd.window.set_modal_idle_callback(0);
  surface_wnd.window.release_capture();
  surface_wnd.window.hide();
  wnd_flush_paint();
  set_palette((GamePalette *)&game_palette_block.game_palette);
  hide();
  wnd_flush_paint();
  return;
}

/* 00454f83 auto */
void CityViewWindow::classify_site()

{
  short x2;
  short y2;
  iptr x;
  int y;
  int in_bounds;
  byte *tile;
  iptr dir;
  
  x2 = cities[city_id].x;
  y2 = cities[city_id].y;
  if ((cities[city_id].flags & CITYF_COASTAL) == 0) {
    for (dir = 0; dir < 9; dir++) {
      x = map_wrap_x((int)(((i8 *)&dir_dx_table_block.dir_dx_table))[dir] + (int)x2);
      y = (int)(((i8 *)&dir_dy_table_block.dir_dy_table))[dir] + (int)y2;
      in_bounds = map_in_bounds(x,y);
      if ((in_bounds != 0) && (tile = (byte *)tile_at(x,y), (*tile & 0x80) != 0)) {
        slot_count = 0x39;
        terrain_kind = 1;
        wonder_slots_left = 0x18;
        improvement_slots_left = 0x1b;
        return;
      }
    }
    slot_count = 0x3c;
    terrain_kind = 2;
    wonder_slots_left = 0x18;
    improvement_slots_left = 0x1f;
  }
  else {
    slot_count = 0x3c;
    terrain_kind = 0;
    wonder_slots_left = 0x18;
    improvement_slots_left = 0x1b;
  }
  return;
}

/* 0045512b auto */
uint CityViewWindow::load_icon_surface()

{
  int loaded;
  
  icon_surface.DibSurface::create(0x20,0x20);
  loaded = icon_surface.load_gif_resource(399,10,0x20,(GamePalette *)&game_palette_block.game_palette);
  return (uint)(loaded != 0);
}

/* 00455183 auto */
uint CityViewWindow::create_windows()

{
  int ok;
  int art_era2;
  
  create_with_handlers((const char *)&str_empty_74,0x800,tax_rate_icon_sprites_block.cityview_screen_rect,tax_rate_icon_sprites_block.cityview_screen_rect_top,
             tax_rate_icon_sprites_block.screen_rect_right - tax_rate_icon_sprites_block.cityview_screen_rect,(tax_rate_icon_sprites_block.cityview_screen_rect_bottom - tax_rate_icon_sprites_block.cityview_screen_rect_top) + 5,(GamePalette *)&game_palette_block.game_palette,&main_frame_wnd_block.main_frame_wnd);
  set_bg_color(0);
  surface_wnd.wnd_create_with_surface((const char *)&str_empty_75,0x800,0,tax_rate_icon_sprites_block.cityview_screen_rect_bottom - 0x1e0 >> 1,
             tax_rate_icon_sprites_block.screen_rect_right - tax_rate_icon_sprites_block.cityview_screen_rect,0x1e0,&cityview_palette,this);
  ok = city_has_improvement(city_id,IMP_SUPERHIGHWAYS);
  if (ok == 0) {
    art_era2 = civ_city_art_era(cities[city_id].owner);
  }
  else {
    art_era2 = 3;
  }
  ok = (&backbuffer)->load_gif_resource(terrain_kind * 5 + art_era2 + 0x154,10,
                     0xec,&cityview_palette);
  if (ok != 0) {
    surface_wnd.window.wnd_handlers.set_keydown_handler(cityview_on_key);
    surface_wnd.window.wnd_handlers.set_lbuttonup_handler((WndMouseFn)cityview_on_lbuttonup);
    surface_wnd.window.wnd_handlers.set_keydown_pre_handler(cityview_on_key_pre);
    surface_wnd.window.wnd_handlers.set_mousemove_handler((WndMouseFn)cityview_on_mousemove);
    redraw_scrolled();
  }
  return (uint)(ok != 0);
}

/* 00455314 auto */
i32 CityViewWindow::layout_buildings()

{
  u32 terrain_kind;
  int spare_slot;
  iptr slot;
  iptr building2;
  
  houses_left = (int)cities[city_id].size + 2;
  rng_set_seed(cities[city_id].x * 0x100 + (int)cities[city_id].y +
                     (uint)map.seed);
  for (building2 = 1; building2 < 0x23; building2++) {
    fixed_slot_used = 0;
    terrain_kind = this->terrain_kind;
    if (terrain_kind == 0) {
      switch(building2) {
      case 8:
        fixed_slot_used = 1;
        slot = 0x35;
        break;
      default:
        slot = take_improvement_slot();
        if (slot == -1) continue;
        break;
      case 0x1c:
        slot = 0x23;
        break;
      case 0x1e:
        fixed_slot_used = 1;
        slot = 0x36;
        break;
      case 0x1f:
        fixed_slot_used = 1;
        slot = 0x39;
        break;
      case 0x22:
        fixed_slot_used = 1;
        slot = 0x37;
      }
    }
    else if (terrain_kind == 1) {
      switch(building2) {
      case 8:
        fixed_slot_used = 1;
        slot = 0x35;
        break;
      default:
        slot = take_improvement_slot();
        if (slot == -1) continue;
        break;
      case 0x1c:
      case 0x1e:
      case 0x1f:
      case 0x22:
        continue;
      }
    }
    else if (terrain_kind == 2) {
      switch(building2) {
      case 8:
        fixed_slot_used = 1;
        slot = 0x34;
        break;
      default:
        slot = take_improvement_slot();
        if (slot == -1) continue;
        break;
      case 0x1c:
      case 0x1e:
      case 0x1f:
      case 0x22:
        continue;
      }
    }
    spare_slot = city_has_improvement(city_id,building2);
    if (spare_slot == 0) {
      if (houses_left == 0) {
        place_empty_lot(slot);
      }
      else {
        place_small_house(slot);
      }
    }
    else {
      place_improvement(slot,building2);
      rand_range(0,0);
    }
  }
  while (spare_slot = take_improvement_slot(), spare_slot != -1) {
    if (houses_left == 0) {
      place_empty_lot(spare_slot);
    }
    else {
      place_small_house(spare_slot);
    }
  }
  houses_left = (int)cities[city_id].size + 2;
  building2 = 0;
  slot = -1;
  for (; building2 < 0x1c; building2++) {
    fixed_slot_used = 0;
    terrain_kind = this->terrain_kind;
    if (terrain_kind == 0) {
      switch(building2) {
      case 2:
        fixed_slot_used = 1;
        slot = 0x3a;
        break;
      case 3:
        fixed_slot_used = 1;
        slot = 0x3b;
        break;
      default:
        slot = take_wonder_slot();
        if (slot == -1) continue;
        break;
      case 6:
        fixed_slot_used = 1;
        slot = 1;
        break;
      case 0x13:
        fixed_slot_used = 1;
        slot = 0x38;
      }
    }
    else if (terrain_kind == 1) {
      switch(building2) {
      case 2:
        fixed_slot_used = 1;
        slot = 0x36;
        break;
      case 3:
        fixed_slot_used = 1;
        slot = 4;
        break;
      default:
        slot = take_wonder_slot();
        if (slot == -1) continue;
        break;
      case 6:
        fixed_slot_used = 1;
        slot = 1;
        break;
      case 0x13:
        fixed_slot_used = 1;
        slot = 0x37;
      }
    }
    else if (terrain_kind == 2) {
      switch(building2) {
      case 2:
        fixed_slot_used = 1;
        slot = 0x36;
        break;
      case 3:
        fixed_slot_used = 1;
        slot = 3;
        break;
      default:
        slot = take_wonder_slot();
        if (slot == -1) continue;
        break;
      case 6:
        fixed_slot_used = 1;
        slot = 0;
        break;
      case 0x13:
        fixed_slot_used = 1;
        slot = 0x39;
      }
    }
    if ((int)game.wonder_city[building2] == city_id) {
      place_wonder(slot,building2);
      rand_range(0,0);
    }
    else if (houses_left == 0) {
      place_large_empty_lot(slot);
    }
    else {
      place_large_house(slot);
    }
  }
  while (spare_slot = take_wonder_slot(), spare_slot != -1) {
    if (houses_left == 0) {
      place_large_empty_lot(spare_slot);
    }
    else {
      place_large_house(spare_slot);
    }
  }
  wonder_art_surface.release();
  improvement_art_surface.release();
  return 1;
}

/* 00455add auto */
void CityViewWindow::draw_buildings()

{
  i8 draw_rect[16];
  int i;
  
  for (i = 0; i < (int)slot_count; i++) {
    if (slot_sprite_ptrs[i] != 0) {
      slot_sprite_ptrs[i]->draw((RECT *)draw_rect,
                 &backbuffer,
                 *(i32 *)(((i8 *)&cityview_slot_x_table_block.cityview_slot_x_table) + i * 8 + terrain_kind * 0x1e0),
                 *(i32 *)(((i8 *)&cityview_slot_x_table_block.cityview_slot_y_table) + i * 8 + terrain_kind * 0x1e0));
    }
  }
  redraw_scrolled();
  return;
}

/* 00455b8e auto */
i32 CityViewWindow::take_wonder_slot()

{
  i32 slot;
  int pick;
  
  if (wonder_slots_left == 0) {
    slot = 0xffffffff;
  }
  else {
    pick = wonder_slots_left * 3 - 3;
    pick = rand_range((int)(pick + (pick >> 0x1f & 3U)) >> 2,wonder_slots_left - 1);
    slot = *(i32 *)(improvement_slot_tables + terrain_kind * 0x60 + pick * 4 + 0x174);
    wonder_slots_left--;
    *(i32 *)(improvement_slot_tables + terrain_kind * 0x60 + pick * 4 + 0x174) =
         *(i32 *)(improvement_slot_tables + terrain_kind * 0x60 + wonder_slots_left * 4 + 0x174);
  }
  return slot;
}

/* 00455c5d auto */
i32 CityViewWindow::take_improvement_slot()

{
  i32 slot;
  int pick;
  
  if (improvement_slots_left == 0) {
    slot = 0xffffffff;
  }
  else {
    pick = improvement_slots_left * 3 - 3;
    pick = rand_range((int)(pick + (pick >> 0x1f & 3U)) >> 2,improvement_slots_left - 1);
    slot = *(i32 *)(improvement_slot_tables + pick * 4 + terrain_kind * 0x7c);
    improvement_slots_left--;
    *(i32 *)(improvement_slot_tables + pick * 4 + terrain_kind * 0x7c) =
         *(i32 *)(improvement_slot_tables + terrain_kind * 0x7c + improvement_slots_left * 4);
  }
  return slot;
}

/* 00455d38 auto */
void CityViewWindow::place_improvement(int slot,int improvement)

{
  if (-1 < slot) {
    slot_sprites[slot].grab_from_surface(&improvement_art_surface,9,
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_x) + (improvement - 1) * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_y) + (improvement - 1) * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_w) + (improvement - 1) * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_improvement_art_h) + (improvement - 1) * 0x10));
    slot_sprites[slot].replace_color(7,9);
    slot_sprite_ptrs[slot] = &slot_sprites[slot];
  }
  return;
}

/* 00455dfd auto */
void CityViewWindow::place_wonder(int slot,int wonder)

{
  if (-1 < slot) {
    slot_sprites[slot].grab_from_surface(&wonder_art_surface,9,
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_x) + wonder * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_y) + wonder * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_w) + wonder * 0x10),
               *(i32 *)(((i8 *)&cityview_improvement_art_x_block.cityview_wonder_art_h) + wonder * 0x10));
    slot_sprites[slot].replace_color(7,9);
    slot_sprite_ptrs[slot] = &slot_sprites[slot];
  }
  return;
}

/* 00455ebe auto */
void CityViewWindow::place_large_house(int slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (houses_left = houses_left - 1, slot != -1)) {
    slot_sprite_ptrs[slot] = &large_building_sprites_b[variant];
  }
  return;
}

/* 00455f2e auto */
void CityViewWindow::place_small_house(int slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (houses_left = houses_left - 1, slot != -1)) {
    slot_sprite_ptrs[slot] = &small_building_sprites_b[variant];
  }
  return;
}

/* 00455f9e auto */
void CityViewWindow::place_large_empty_lot(int slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (slot != -1)) {
    slot_sprite_ptrs[slot] = &large_building_sprites_a[variant];
  }
  return;
}

/* 00456005 auto */
void CityViewWindow::place_empty_lot(int slot)

{
  int variant;
  
  variant = rand_range(0,3);
  if ((fixed_slot_used == 0) && (slot != -1)) {
    slot_sprite_ptrs[slot] = &small_building_sprites_a[variant];
  }
  return;
}

/* 0045606c auto */
void CityViewWindow::redraw_scrolled()

{
  tagRECT dst_rect;
  tagRECT src_rect;
  
  SetRect(&dst_rect,0,0,tax_rate_icon_sprites_block.screen_rect_right,0x1e0);
  SetRect(&src_rect,cityview.scroll_x,0,tax_rate_icon_sprites_block.screen_rect_right + cityview.scroll_x,0x1e0);
  (&backbuffer)->DibSurface::blit(&surface_wnd,&src_rect,&dst_rect
            );
  draw_title();
  surface_wnd.repaint();
  wnd_flush_paint();
  return;
}

/* 004560f8 auto */
void CityViewWindow::draw_title()

{
  text_clear();
  text_append(cities[city_id].name);
  text_append_colon();
  text_append_space();
  text_append_year((int)game.year);
  gfx_set_text_color_index(10);
  OffsetRect((LPRECT)title_rect,1,1);
  surface_wnd.draw_text_in_rect(&title_font,(const char *)&textbuf.buf,
             (RECT *)title_rect,0);
  gfx_set_text_color_index(0x1a);
  OffsetRect((LPRECT)title_rect,-1,-1);
  surface_wnd.draw_text_in_rect(&title_font,(const char *)&textbuf.buf,
             (RECT *)title_rect,0);
  return;
}

/* 004561d9 auto */
void __cdecl cityview_on_mousemove(int mouse_x)

{
  if (tax_rate_icon_sprites_block.screen_rect_right * 7 >> 3 < mouse_x) {
    cityview.wnd->scroll_right();
  }
  if (mouse_x < tax_rate_icon_sprites_block.screen_rect_right >> 3) {
    cityview.wnd->scroll_left();
  }
  return;
}

/* 00456228 auto */
void __cdecl cityview_on_key(int key)

{
  if (key == 0xd2) {
    (*(ModalMenuWindow * *)&cityview.wnd)->end_modal_loop();
  }
  return;
}

/* 0045626d auto */
void __cdecl cityview_on_lbuttonup(void)

{
  if ((*(int *)&cityview.wnd) != 0) {
    ((ModalMenuWindow *)((*(int *)&cityview.wnd) + 0x100))->end_modal_loop();
  }
  return;
}

/* 0045629b auto */
void __cdecl cityview_on_key_pre(i32 key)

{
  switch(key) {
  case 0xa4:
  case 0xc2:
    if (tax_rate_icon_sprites_block.screen_rect_right < 0x500) {
      cityview.wnd->scroll_left();
    }
    break;
  case 0xa6:
  case 0xc3:
    if (tax_rate_icon_sprites_block.screen_rect_right < 0x500) {
      cityview.wnd->scroll_right();
    }
    break;
  case 0xd0:
  case 0xd1:
  case 0xd2:
    if (cityview.wnd != (CityViewWindow *)0x0) {
      cityview.wnd->surface_wnd.window.end_modal_loop();
    }
  }
  return;
}

/* 0045638b auto */
void CityViewWindow::scroll_right()

{
  SHORT key_state;
  tagPOINT cursor;
  tagRECT dst_rect;
  tagRECT src_rect;
  int step;
  
  step = 4;
  SetRect(&dst_rect,0,0,tax_rate_icon_sprites_block.screen_rect_right,0x1e0);
  SetRect(&src_rect,cityview.scroll_x,0,tax_rate_icon_sprites_block.screen_rect_right + cityview.scroll_x,0x1e0);
  GetCursorPos(&cursor);
  while( true ) {
    if (0x500 - tax_rate_icon_sprites_block.screen_rect_right < cityview.scroll_x) {
      return;
    }
    key_state = GetAsyncKeyState(0x66);
    if ((((char)((ushort)key_state >> 8) == '\0') &&
        (key_state = GetAsyncKeyState(0x27), (char)((ushort)key_state >> 8) == '\0')) &&
       (cursor.x <= tax_rate_icon_sprites_block.screen_rect_right * 7 >> 3)) break;
    (&backbuffer)->DibSurface::blit(&surface_wnd,&src_rect,
               &dst_rect);
    draw_title();
    surface_wnd.wnd_blit_surface();
    cityview.scroll_x += step;
    OffsetRect(&src_rect,step,0);
    GetCursorPos(&cursor);
  }
  return;
}

/* 004564a8 auto */
void CityViewWindow::scroll_left()

{
  SHORT key_state;
  tagPOINT cursor;
  tagRECT dst_rect;
  tagRECT src_rect;
  int step;
  
  step = 4;
  SetRect(&dst_rect,0,0,tax_rate_icon_sprites_block.screen_rect_right,0x1e0);
  SetRect(&src_rect,cityview.scroll_x,0,tax_rate_icon_sprites_block.screen_rect_right + cityview.scroll_x,0x1e0);
  GetCursorPos(&cursor);
  while( true ) {
    if (cityview.scroll_x < 0) {
      return;
    }
    key_state = GetAsyncKeyState(100);
    if ((((char)((ushort)key_state >> 8) == '\0') &&
        (key_state = GetAsyncKeyState(0x25), (char)((ushort)key_state >> 8) == '\0')) &&
       (tax_rate_icon_sprites_block.screen_rect_right >> 3 <= cursor.x)) break;
    (&backbuffer)->DibSurface::blit(&surface_wnd,&src_rect,
               &dst_rect);
    draw_title();
    surface_wnd.wnd_blit_surface();
    cityview.scroll_x -= step;
    OffsetRect(&src_rect,-step,0);
    GetCursorPos(&cursor);
  }
  return;
}

/* 00456e90 auto */
void *CityViewWindow::scalar_destruct(uint flags)

{
  this->~CityViewWindow();
  if ((flags & 1) != 0) {
    operator_delete(this);
  }
  return this;
}

/* 004bb8e0 auto */
void __cdecl wonder_show_movie(int wonder)

{
  i32 ret_addr;
  
  snd_wave_stop_all();
  WonderMovieWindow movie_wnd2;
  blink.unit_active = 0;
  snd_cd_stop();
  ret_addr = 0x4bb937;
  movie_wnd2.create(wonder);
  movie_wnd2.play();
  snd_wave_stop_all();
  snd_sync_music_option();
  if (game_language != 0) {
    ret_addr = 0x4bb96a;
    ((PediaWindow *)((i8 *)&pedia_wnd.window))->show_improvement_modal(wonder + IMP_FIRST_WONDER);
  }
  return;
}

/* 004bb99f auto */
WonderMovieWindow::WonderMovieWindow()

{
  
  this->FrameWindow::construct();
  player.construct();
  surface_ctor(&frame_surface);
  palette.init();
  vtable = (u32)((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 2);
  font_size16_bold_block.wonder_movie_wnd = this;
}

/* 004bba79 auto */
WonderMovieWindow::~WonderMovieWindow()

{
  int seh_state;
  
  *(pointer *)&vtable = ((pointer *)&leader_anim_frame_starts_block.ptr_fun_3 + 2);
  seh_state = seh_state & 0xff;
  font_size16_bold_block.wonder_movie_wnd = 0;
  (*((char *)&seh_state + 0)) = 2;
  /* inlined unwind funclet 004bbaf1 */ { palette.destroy(); }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 004bbb00 */ { surface_dtor(&frame_surface); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 004bbb0f */ { player.destruct(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 004bbb1e */ { (((FrameWindow *)this))->FrameWindow::destruct(); }
  return;
}

/* 004bbb3f auto */
void WonderMovieWindow::create(int wonder)

{
  int ok;
  i32 title;
  iptr rect;
  i32 style;
  i32 pos_x;
  i32 pos_y;
  i32 wnd_height;
  i32 arg7;
  i32 arg8;
  i32 arg9;
  i32 wnd_width;
  i8 draw_rect[16];
  DllHandle art_dll;
  DibSurface art_surface;
  i32 height2;
  SIZE_T compacted;
  Sprite sprite;
  i8 path[128];
  i32 width2;
  
  width2 = 0x140;
  height2 = 0xf0;
  sprite_init(&sprite);
  ON_SCOPE_EXIT(sprite_destroy(&sprite));
  surface_ctor(&art_surface);
  ON_SCOPE_EXIT(surface_dtor(&art_surface));
  compacted = GlobalCompact(0x100000);
  str_clear((char *)path);
  str_append((char *)path,(const char *)&str_civ2_wonder_dll);
  ok = file_resolve_path((const char *)path);
  if (ok == 0) {
    frame_surface.DibSurface::create(0x40,0x20);
    frame_surface.fill(0x21);
  }
  else {
    art_dll.resource_dll_load(path);
    ON_SCOPE_EXIT(art_dll.free());
    ok = art_surface.load_gif_resource(wonder + 20000,10,0xec,(GamePalette *)palette_bytes);
    if (ok == 0) {
      MessageBoxA((HWND)0x0,str_failed_to_load_civ2art_gif,str_notice_9,0x40);
    }
    else {
      sprite.grab_from_surface(&art_surface,2,1,1,0x40,0x20);
      frame_surface.DibSurface::create(0x40,0x20);
      sprite.draw((RECT *)draw_rect,((DibSurface *)frame_surface_bytes),0,0);
      sprite.free();
      art_surface.release();
    }
    art_dll.dll_handle_free();
  }
  wonder_index = wonder;
  arg9 = 0;
  arg8 = 0;
  arg7 = 0;
  pos_y = 0;
  pos_x = 0;
  style = 0xc;
  wnd_width = width2;
  wnd_height = height2;
  title = (iptr)label_string(improvements[wonder_index + IMP_FIRST_WONDER].name);
  this->FrameWindow::create((const char *)title,style,pos_x,pos_y,wnd_width,wnd_height,arg7,arg8,arg9);
  ((WndBase *)&window)->set_palette((GamePalette *)palette_bytes);
  wnd_width = 0;
  rect = (iptr)get_rect();
  fill_rect((RECT *)rect,wnd_width);
  draw_caption();
  repaint();
  ((WndBase *)&window)->center();
  window.wnd_handlers.set_lbuttonup_handler((WndMouseFn)wonder_movie_on_lbuttonup);
  return;
}

/* 004bbdfb auto */
void WonderMovieWindow::play()

{
  int found;
  i8 path[128];
  int open_result;
  
  resize();
  if (((game.game_flags & GAME_SAVED_AS_SCENARIO) == 0) && ((game.game_flags & GAME_SCENARIO) == 0)) {
    str_clear((char *)path);
    str_append((char *)path,(const char *)&str_civ2_video_wonder);
    if (wonder_index < 10) {
      str_append_int(path,0);
    }
    str_append_int(path,wonder_index);
    str_concat(path,str_avi_2);
    found = file_resolve_path((const char *)path);
    if (found != 0) {
      window.wnd_handlers.set_lbuttonup_handler((WndMouseFn)wonder_movie_on_lbuttonup);
      window.wnd_handlers.set_keydown_handler(wonder_movie_on_key);
      player.create_child((const char*)&str_empty_116,0x200,border_width,caption_height,
                 this);
      open_result = ((AviPlayer *)&player_head)->open((const char *)path);
      if (open_result == 0) {
        ((AviPlayer *)&player_head)->set_end_callback(wonder_movie_on_video_end);
        ((WndHandlers *)&player_field_58)->set_lbuttonup_handler((WndMouseFn)wonder_movie_on_lbuttonup);
        ((DibSurface *)&player_head)->avi_player_set_palette(((GamePalette *)palette_bytes))
        ;
        ((WndBase *)&player_field_48)->set_palette(((GamePalette *)palette_bytes));
        screen_fade_out();
        main_frame_wnd_block.main_frame_wnd.set_palette(((GamePalette *)palette_bytes));
        this->SurfaceWindow::show();
        ((SurfaceWindow *)&player_head)->SurfaceWindow::show();
        wnd_flush_paint();
        ((AviPlayer *)&player_head)->play();
        if (2 < game.multiplayer) {
          ((ModalMenuWindow *)&window)->set_modal_idle_callback(net_modal_idle_poll);
        }
        ((ModalMenuWindow *)&window)->run_modal_loop();
        ((ModalMenuWindow *)&window)->set_modal_idle_callback(0);
        snd_wave_stop_all();
        ((WndBase *)&player_field_48)->hide();
        wnd_flush_paint();
        main_frame_wnd_block.main_frame_wnd.set_palette((GamePalette *)&game_palette_block.game_palette);
        ((WndBase *)&window)->hide();
        wnd_flush_paint();
        screen_fade_in();
      }
      else if (open_result == -0x7ffbfeac) {
        popup_game((const char *)&str_vfwnotregistered_5);
      }
    }
  }
  return;
}

/* 004bc0d3 auto */
void WonderMovieWindow::draw_caption()

{
  frame_set_border_tile(&frame_surface);
  this->FrameWindow::draw_caption();
  frame_set_border_tile(&window_border_tile_surface);
  return;
}

/* 004bc10f auto */
void WonderMovieWindow::resize()

{
  iptr rect;
  i32 color;
  
  resize_clamped(border_width * 2 + 0x140,
             caption_height + border_width + 0xf0);
  layout();
  color = 0;
  rect = (iptr)get_rect();
  fill_rect((RECT *)rect,color);
  repaint();
  ((WndBase *)&window)->center();
  wnd_flush_paint();
  return;
}

/* 004bc193 auto */
void __cdecl wonder_movie_on_video_end(void)

{
  font_size16_bold_block.wonder_movie_wnd->window.end_modal_loop();
  return;
}

/* 004bc1b1 auto */
void __cdecl wonder_movie_on_lbuttonup(void)

{
  font_size16_bold_block.wonder_movie_wnd->window.end_modal_loop();
  return;
}

/* 004bc1cf auto */
void __cdecl wonder_movie_on_key(int key)

{
  if ((0xcf < key) && (key < 0xd3)) {
    font_size16_bold_block.wonder_movie_wnd->window.end_modal_loop();
  }
  return;
}
