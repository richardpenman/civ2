/* The game, graphics: the sprite sheets, the sprite editor and picker (cheat menu): functions translated from the original
 * (tools/gen_c.py, now edited directly), in address order. */
#include "auto_common.h"

/* 0056f910 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl sprite_sheet_surface_global_init(void)

{
  sprite_sheet_surface_static_init();
  sprite_sheet_surface_reg_atexit();
  return;
}

/* 0056f92a auto */
void __cdecl sprite_sheet_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface));
  return;
}

/* 0056f944 auto */
void __cdecl sprite_sheet_surface_reg_atexit(void)

{
  _atexit(sprite_sheet_surface_static_dtor);
  return;
}

/* 0056f961 auto */
void __cdecl sprite_sheet_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface));
  return;
}

/* 0056f97b auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl sprite_editor_orig_sprite_global_init(void)

{
  sprite_editor_orig_sprite_static_init();
  sprite_editor_orig_sprite_reg_atexit();
  return;
}

/* 0056f995 auto */
void __cdecl sprite_editor_orig_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&sprite_editor.orig_sprite));
  return;
}

/* 0056f9af auto */
void __cdecl sprite_editor_orig_sprite_reg_atexit(void)

{
  _atexit(sprite_editor_orig_sprite_static_dtor);
  return;
}

/* 0056f9cc auto */
void __cdecl sprite_editor_orig_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&sprite_editor.orig_sprite));
  return;
}

/* 0056f9e6 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl sprite_editor_result_global_init(void)

{
  sprite_editor_result_static_init();
  sprite_editor_result_reg_atexit();
  return;
}

/* 0056fa00 auto */
void __cdecl sprite_editor_result_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&sprite_editor.result_sprite));
  return;
}

/* 0056fa1a auto */
void __cdecl sprite_editor_result_reg_atexit(void)

{
  _atexit(sprite_editor_result_static_dtor);
  return;
}

/* 0056fa37 auto */
void __cdecl sprite_editor_result_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&sprite_editor.result_sprite));
  return;
}

/* 0056fa51 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl sprite_picker_sprite_global_init(void)

{
  sprite_picker_sprite_static_init();
  sprite_picker_sprite_reg_atexit();
  return;
}

/* 0056fa6b auto */
void __cdecl sprite_picker_sprite_static_init(void)

{
  sprite_init((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite));
  return;
}

/* 0056fa85 auto */
void __cdecl sprite_picker_sprite_reg_atexit(void)

{
  _atexit(sprite_picker_sprite_static_dtor);
  return;
}

/* 0056faa2 auto */
void __cdecl sprite_picker_sprite_static_dtor(void)

{
  sprite_destroy((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite));
  return;
}

/* 0056fabc auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl sprite_editor_surface_global_init(void)

{
  sprite_editor_surface_static_init();
  sprite_editor_surface_reg_atexit();
  return;
}

/* 0056fad6 auto */
void __cdecl sprite_editor_surface_static_init(void)

{
  surface_ctor((DibSurface *)((i8 *)&sprite_editor.surface));
  return;
}

/* 0056faf0 auto */
void __cdecl sprite_editor_surface_reg_atexit(void)

{
  _atexit(sprite_editor_surface_static_dtor);
  return;
}

/* 0056fb0d auto */
void __cdecl sprite_editor_surface_static_dtor(void)

{
  surface_dtor((DibSurface *)((i8 *)&sprite_editor.surface));
  return;
}

/* 0056fb27 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl sprite_editor_window_global_init(void)

{
  sprite_editor_window_static_init();
  sprite_editor_window_reg_atexit();
  return;
}

/* 0056fb41 auto */
void __cdecl sprite_editor_window_static_init(void)

{
  ((FrameWindow *)((i8 *)&sprite_editor.window))->FrameWindow::construct();
  return;
}

/* 0056fb5b auto */
void __cdecl sprite_editor_window_reg_atexit(void)

{
  _atexit(sprite_editor_window_static_dtor);
  return;
}

/* 0056fb78 auto */
void __cdecl sprite_editor_window_static_dtor(void)

{
  ((FrameWindow *)((i8 *)&sprite_editor.window))->FrameWindow::destruct();
  return;
}

/* 0056fb92 auto */
/* Library Function - Multiple Matches With Different Base Names
    _$E26
    _$E31
    _$E353
    _$E354
   
   Library: Visual Studio 1998 Debug */

void __cdecl sprite_picker_window_global_init(void)

{
  sprite_picker_window_static_init();
  sprite_picker_window_reg_atexit();
  return;
}

/* 0056fbac auto */
void __cdecl sprite_picker_window_static_init(void)

{
  new (&sprite_editor.sprite_picker_window, GameNewAt()) SpritePicker;
  return;
}

/* 0056fbc6 auto */
void __cdecl sprite_picker_window_reg_atexit(void)

{
  _atexit(sprite_picker_window_static_dtor);
  return;
}

/* 0056fbe3 auto */
void __cdecl sprite_picker_window_static_dtor(void)

{
  ((SpritePicker *)((i8 *)&sprite_editor.sprite_picker_window))->~SpritePicker();
  return;
}

/* 0056fbfd auto */
void __cdecl sprite_sheet_draw_shield_marks(int unit_type,int x,int y)

{
  int left;
  int top;
  int dy;
  int dx;
  
  left = x - 1;
  top = y - 1;
  for (dx = 0; dx < 0x40; dx++) {
    ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->set_pixel(left + dx + 1,top,8);
  }
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->set_pixel(left + road_sprites_block.unit_shield_x_offsets[unit_type] + 1,top,
             4);
  for (dy = 0; dy < 0x30; dy++) {
    ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->set_pixel(left,dy + top + 1,8);
  }
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->set_pixel(left,road_sprites_block.unit_shield_y_offsets[unit_type] + top + 1,
             4);
  return;
}

/* 0056fce4 auto */
void __cdecl sprite_sheet_save_units(void)

{
  byte key_color;
  int result;
  iptr height;
  i32 width;
  uint fill_color;
  i8 clip_unit[16];
  i8 clip_backdrop[16];
  int row;
  int col;
  int unit_type;
  int start_x2;
  int y2;
  GamePalette palette;
  char saved_dir[260];
  int x2;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __getcwd(saved_dir,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = gfx_load_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_units_bmp_2,10,0xc0,&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_units_gif_3,10,0xc0,&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_units_gif_4,10,0xc0,&palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  start_x2 = 1;
  row = 0;
  col = 0;
  y2 = 1;
  x2 = 1;
  for (unit_type = 0; unit_type < 0x3f; unit_type++) {
    key_color = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)clip_backdrop,&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + unit_type * 0x3c))->draw((RECT *)clip_unit,&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    sprite_sheet_draw_shield_marks(unit_type,x2,y2);
    col++;
    x2 += 0x41;
    if (8 < col) {
      col = 0;
      row++;
      y2 += 0x31;
      x2 = start_x2;
    }
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_units_bmp_3);
  if ((result == 0) || (result = crt_remove(str_units_bmp_4), result == 0)) {
    gfx_save_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_units_bmp_5,10,0xc0,&palette);
  }
  __chdir(saved_dir);
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->release();
  return;
}

/* 0056ffda auto */
void __cdecl sprite_sheet_save_cities(void)

{
  byte key_color;
  int result;
  iptr height;
  i32 width;
  uint fill_color;
  i8 clip_airbase_occ[16];
  i8 clip_airbase_occ_bg[16];
  i8 clip_airbase[16];
  i8 clip_airbase_bg[16];
  i8 clip_fortress[16];
  i8 clip_fortress_bg[16];
  i8 clip_fortify[16];
  i8 clip_fortify_bg[16];
  i8 clip_flag_dark[16];
  i8 clip_flag[16];
  i8 clip_walled[16];
  i8 clip_walled_bg[16];
  i8 clip_city[16];
  i8 clip_city_bg[16];
  int style2;
  int civ;
  int cell_x;
  i32 flag_dark_y;
  i32 flag_y;
  int walled_x;
  int y2;
  int x2;
  GamePalette palette;
  char saved_dir[260];
  int size_idx;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __getcwd(saved_dir,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = gfx_load_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_cities_bmp_2,10,0xc0,&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_cities_gif_3,10,0xc0,&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_cities_gif_4,10,0xc0,&palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  x2 = 1;
  y2 = 0x27;
  walled_x = 0x14e;
  for (style2 = 0; style2 < 6; style2++) {
    for (size_idx = 0; size_idx < 4; size_idx++) {
      cell_x = size_idx * 0x41;
      key_color = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + style2 * 0x1e0))->get_key_color();
      fill_color = (uint)key_color;
      height = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + style2 * 0x1e0))->height();
      width = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + style2 * 0x1e0))->width();
      gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,cell_x + x2,y2,width,height,fill_color);
      ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)clip_city_bg,&sprite_editor.sprite_sheet_surface,cell_x + x2,
                 y2);
      ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + size_idx * 0x78 + style2 * 0x1e0))->draw(
                 (RECT *)clip_city,&sprite_editor.sprite_sheet_surface,cell_x + x2,y2);
      key_color = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->get_key_color();
      fill_color = (uint)key_color;
      height = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->height();
      width = ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->width();
      gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,cell_x + walled_x,y2,width,height,fill_color);
      ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)clip_walled_bg,&sprite_editor.sprite_sheet_surface,cell_x + walled_x,
                 y2);
      ((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_walled_sprites) + size_idx * 0x78 + style2 * 0x1e0))->draw(
                 (RECT *)clip_walled,&sprite_editor.sprite_sheet_surface,cell_x + walled_x,y2);
    }
    y2 += 0x31;
  }
  x2 = 1;
  flag_y = 0x1a9;
  flag_dark_y = 0x1c0;
  for (civ = 0; civ < 8; civ++) {
    key_color = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->height();
    width = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,flag_y,width,height,fill_color);
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + civ * 0x78))->draw((RECT *)clip_flag,&sprite_editor.sprite_sheet_surface,
               x2,flag_y);
    key_color = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->height();
    width = ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,flag_dark_y,width,height,fill_color);
    ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + civ * 0x78))->draw((RECT *)clip_flag_dark,&sprite_editor.sprite_sheet_surface,
               x2,flag_dark_y);
    x2 += 0xf;
  }
  key_color = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->height();
  width = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,0x8f,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)clip_fortify_bg,&sprite_editor.sprite_sheet_surface,0x8f,0x1a7);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->draw((RECT *)clip_fortify,&sprite_editor.sprite_sheet_surface,0x8f,0x1a7);
  key_color = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->height();
  width = ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,0xd0,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)clip_fortress_bg,&sprite_editor.sprite_sheet_surface,0xd0,0x1a7);
  ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->draw((RECT *)clip_fortress,&sprite_editor.sprite_sheet_surface,0xd0,0x1a7);
  key_color = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->height();
  width = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,0x111,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)clip_airbase_bg,&sprite_editor.sprite_sheet_surface,0x111,0x1a7);
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->draw((RECT *)clip_airbase,&sprite_editor.sprite_sheet_surface,0x111,0x1a7);
  key_color = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->height();
  width = ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,0x152,0x1a7,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)clip_airbase_occ_bg,&sprite_editor.sprite_sheet_surface,0x152,0x1a7);
  ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->draw((RECT *)clip_airbase_occ,&sprite_editor.sprite_sheet_surface,0x152,0x1a7);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_cities_bmp_3);
  if ((result == 0) || (result = crt_remove(str_cities_bmp_4), result == 0)) {
    gfx_save_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_cities_bmp_5,10,0xc0,&palette);
  }
  __chdir(saved_dir);
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->release();
  return;
}

/* 00570780 auto */
void __cdecl sprite_sheet_save_terrain1(void)

{
  byte key_color;
  int result;
  iptr height;
  i32 width;
  uint fill_color;
  i8 rail_rect[16];
  i8 rail_bg_rect[16];
  i8 road_rect[16];
  i8 road_bg_rect[16];
  i8 hut_rect[16];
  i8 hut_bg_rect[16];
  i8 shield_rect[16];
  i8 shield_bg_rect[16];
  i8 pollution_rect[16];
  i8 pollution_bg_rect[16];
  i8 impr_rect[16];
  i8 impr_bg_rect[16];
  i8 res2_rect[16];
  i8 res2_bg_rect[16];
  i8 res_rect[16];
  i8 res_bg_rect[16];
  i8 base_rect[16];
  i8 base_bg_rect[16];
  i32 res2_x;
  i32 res_x;
  int y2;
  int x2;
  GamePalette palette;
  char saved_cwd[260];
  int terrain2;
  int i;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = gfx_load_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_terrain1_bmp_2,10,0xc0,&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif_3,10,0xc0,&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain1_gif_4,10,0xc0,&palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  x2 = 1;
  res_x = 0x83;
  res2_x = 0xc4;
  y2 = 1;
  for (terrain2 = 0; terrain2 < 0xb; terrain2++) {
    key_color = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)base_bg_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + terrain2 * 0x3c))->draw((RECT *)base_rect,&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    key_color = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->height();
    width = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,res_x,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)res_bg_rect,&sprite_editor.sprite_sheet_surface,res_x,y2);
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + terrain2 * 0x78))->draw((RECT *)res_rect,&sprite_editor.sprite_sheet_surface,res_x
               ,y2);
    key_color = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->height();
    width = ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,res2_x,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)res2_bg_rect,&sprite_editor.sprite_sheet_surface,res2_x,y2);
    ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + terrain2 * 0x78))->draw((RECT *)res2_rect,&sprite_editor.sprite_sheet_surface,res2_x
               ,y2);
    y2 += 0x21;
  }
  x2 = 0x1c8;
  y2 = 100;
  for (i = 0; i < 3; i++) {
    key_color = ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    /* BF-013 (Peter Johnson's ledger): the original took the height of
       sprite terrain2 (11 after the loop above), 660 bytes into a block of
       184: the improvement's own */
    height = ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)impr_bg_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + i * 0x3c))->draw((RECT *)impr_rect,&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    y2 += 0x21;
  }
  key_color = ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->height();
  width = ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)pollution_bg_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
  ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->draw((RECT *)pollution_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
  y2 += 0x21;
  key_color = ((Sprite *)((i8 *)&grassland_shield_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&grassland_shield_sprite))->height();
  width = ((Sprite *)((i8 *)&grassland_shield_sprite))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)shield_bg_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
  ((Sprite *)((i8 *)&grassland_shield_sprite))->draw((RECT *)shield_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
  y2 += 0x21;
  key_color = ((Sprite *)((i8 *)&hut_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&hut_sprite))->height();
  width = ((Sprite *)((i8 *)&hut_sprite))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
  ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)hut_bg_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
  ((Sprite *)((i8 *)&hut_sprite))->draw((RECT *)hut_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
  y2 = 0x16c;
  x2 = 1;
  for (i = 0; i < 9; i++) {
    key_color = ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)road_bg_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
    ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + i * 0x3c))->draw((RECT *)road_rect,&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    key_color = ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2 + 0x21,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)rail_bg_rect,&sprite_editor.sprite_sheet_surface,x2,y2 + 0x21);
    ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + i * 0x3c))->draw((RECT *)rail_rect,&sprite_editor.sprite_sheet_surface,x2
               ,y2 + 0x21);
    x2 += 0x41;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_terrain1_bmp_3);
  if ((result == 0) || (result = crt_remove(str_terrain1_bmp_4), result == 0)) {
    gfx_save_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_terrain1_bmp_5,10,0xc0,&palette);
  }
  __chdir(saved_cwd);
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->release();
  return;
}

/* 00570fc6 auto */
void __cdecl sprite_sheet_save_terrain2(void)

{
  byte key_color;
  int result;
  iptr height;
  i32 width;
  uint fill_color;
  i8 coast_w_rect[16];
  i8 coast_s_rect[16];
  i8 coast_e_rect[16];
  i8 coast_n_rect[16];
  i8 mouth_rect[16];
  i8 mouth_bg_rect[16];
  i8 hill_rect[16];
  i8 hill_bg_rect[16];
  i8 mountain_rect[16];
  i8 mountain_bg_rect[16];
  i8 forest_rect[16];
  i8 forest_bg_rect[16];
  i8 river_rect[16];
  i8 river_bg_rect[16];
  int coast_idx;
  int base_y;
  int cell_y;
  int cell_x;
  int base_x;
  GamePalette palette;
  char saved_cwd[260];
  uint i;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = gfx_load_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_terrain2_bmp_2,10,0xc0,&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif_3,10,0xc0,&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_terrain2_gif_4,10,0xc0,&palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  base_x = 1;
  base_y = 0x43;
  for (i = 0; (int)i < 0x10; i++) {
    cell_x = (i & 7) * 0x41;
    cell_y = ((int)i >> 3) * 0x21;
    key_color = ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)river_bg_rect,&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y);
    ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + i * 0x3c))->draw((RECT *)river_rect,&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y);
    key_color = ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y + 0x42,width,height,fill_color
                      );
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)forest_bg_rect,&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y + 0x42);
    ((Sprite *)(((i8 *)&forest_sprites) + i * 0x3c))->draw((RECT *)forest_rect,&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y + 0x42);
    key_color = ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y + 0x84,width,height,fill_color
                      );
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)mountain_bg_rect,&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y + 0x84);
    ((Sprite *)(((i8 *)&mountain_sprites) + i * 0x3c))->draw((RECT *)mountain_rect,&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y + 0x84);
    key_color = ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x + cell_x,base_y + cell_y + 0xc6,width,height,fill_color
                      );
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)hill_bg_rect,&sprite_editor.sprite_sheet_surface,base_x + cell_x,
               base_y + cell_y + 0xc6);
    ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + i * 0x3c))->draw((RECT *)hill_rect,&sprite_editor.sprite_sheet_surface,
               base_x + cell_x,base_y + cell_y + 0xc6);
  }
  base_x = 1;
  base_y = 0x14b;
  for (i = 0; (int)i < 4; i++) {
    key_color = ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x,base_y,width,height,fill_color);
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)mouth_bg_rect,&sprite_editor.sprite_sheet_surface,base_x,base_y);
    ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + i * 0x3c))->draw((RECT *)mouth_rect,&sprite_editor.sprite_sheet_surface,base_x
               ,base_y);
    base_x += 0x41;
  }
  base_x = 1;
  base_y = 0x1ad;
  for (coast_idx = 0; coast_idx < 8; coast_idx++) {
    key_color = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0))->height();
    width = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x,base_y,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + coast_idx * 0xf0))->draw((RECT *)coast_n_rect,&sprite_editor.sprite_sheet_surface,
               base_x,base_y);
    key_color = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->height();
    width = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x + 0x21,base_y + 0x22,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + coast_idx * 0xf0))->draw((RECT *)coast_e_rect,&sprite_editor.sprite_sheet_surface,
               base_x + 0x21,base_y + 0x22);
    key_color = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->height();
    width = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x,base_y + 0x11,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + coast_idx * 0xf0))->draw((RECT *)coast_s_rect,&sprite_editor.sprite_sheet_surface,
               base_x,base_y + 0x11);
    key_color = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->height();
    width = ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,base_x,base_y + 0x22,width,height,fill_color);
    ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + coast_idx * 0xf0))->draw((RECT *)coast_w_rect,&sprite_editor.sprite_sheet_surface,
               base_x,base_y + 0x22);
    base_x += 0x42;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_terrain2_bmp_3);
  if ((result == 0) || (result = crt_remove(str_terrain2_bmp_4), result == 0)) {
    gfx_save_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_terrain2_bmp_5,10,0xc0,&palette);
  }
  __chdir(saved_cwd);
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->release();
  return;
}

/* 00571910 auto */
void __cdecl sprite_sheet_save_tech_icons(void)

{
  byte key_color;
  int result;
  iptr height;
  i32 width;
  uint fill_color;
  i8 icon_rect[16];
  int y2;
  GamePalette palette;
  int x2;
  char saved_cwd[260];
  int category2;
  int row;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = gfx_load_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_icons_bmp_2,10,0xc0,&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_3,10,0xc0,&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_4,10,0xc0,&palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  y2 = 0xd3;
  for (row = 0; row < 4; row++) {
    x2 = 0x157;
    for (category2 = 0; category2 < 5; category2++) {
      key_color = ((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + category2 * 0xf0 + row * 0x3c))->get_key_color();
      fill_color = (uint)key_color;
      height = ((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + category2 * 0xf0 + row * 0x3c))->height();
      width = ((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + category2 * 0xf0 + row * 0x3c))->width();
      gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
      ((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) + category2 * 0xf0 + row * 0x3c))->draw(
                 (RECT *)icon_rect,&sprite_editor.sprite_sheet_surface,x2,y2);
      x2 += 0x25;
    }
    y2 += 0x15;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_icons_bmp_3);
  if ((result == 0) || (result = crt_remove(str_icons_bmp_4), result == 0)) {
    gfx_save_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_icons_bmp_5,10,0xc0,&palette);
  }
  __chdir(saved_cwd);
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->release();
  return;
}

/* 00571bce auto */
void __cdecl sprite_sheet_save_building_icons(void)

{
  byte key_color;
  int result;
  iptr height;
  i32 width;
  uint fill_color;
  i8 explosion_rect[16];
  i8 cursor_rect[16];
  i8 wonder_rect[16];
  i8 improvement_rect[16];
  int column;
  int y2;
  GamePalette palette;
  int x2;
  char saved_cwd[260];
  int i;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  __getcwd(saved_cwd,0x104);
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = gfx_load_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_icons_bmp_6,10,0xc0,&palette);
  if (result == 0) {
    result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_5,10,0xc0,&palette);
    if (result == 0) {
      __chdir(((i8 *)&game_dirs.home));
      ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)&str_icons_gif_6,10,0xc0,&palette);
    }
    else {
      __chdir(((i8 *)&game_dirs.home));
    }
  }
  else {
    __chdir(((i8 *)&game_dirs.home));
  }
  x2 = 0x157;
  y2 = 1;
  column = 0;
  for (i = 1; i < 0x27; i++) {
    key_color = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + i * 0x3c))->draw((RECT *)improvement_rect,&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    x2 += 0x25;
    column++;
    if (7 < column) {
      column = 0;
      y2 += 0x15;
      x2 = 0x157;
    }
  }
  x2 = 0x157;
  y2 = 0x6a;
  column = 0;
  for (i = 0; i < 0x1c; i++) {
    key_color = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + i * 0x3c))->draw((RECT *)wonder_rect,&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    x2 += 0x25;
    column++;
    if (6 < column) {
      column = 0;
      y2 += 0x15;
      x2 = 0x157;
    }
  }
  key_color = ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->get_key_color();
  fill_color = (uint)key_color;
  height = ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->height();
  width = ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->width();
  gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,199,0x100,width,height,fill_color);
  ((Sprite *)((i8 *)&map_cursor_sprite_block.map_cursor_sprite))->draw((RECT *)cursor_rect,&sprite_editor.sprite_sheet_surface,199,0x100);
  x2 = 1;
  y2 = 0x164;
  for (i = 0; i < 8; i++) {
    key_color = ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->get_key_color();
    fill_color = (uint)key_color;
    height = ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->height();
    width = ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->width();
    gfx_fill_rect_xywh(&sprite_editor.sprite_sheet_surface,x2,y2,width,height,fill_color);
    ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + i * 0x3c))->draw((RECT *)explosion_rect,&sprite_editor.sprite_sheet_surface,x2
               ,y2);
    x2 += 0x21;
  }
  __chdir(((i8 *)&net_map_lock_slots_block.scenario_dir_path));
  result = file_exists((const char *)&str_icons_bmp_7);
  if ((result == 0) || (result = crt_remove(str_icons_bmp_8), result == 0)) {
    gfx_save_bmp_640x480(&sprite_editor.sprite_sheet_surface,str_icons_bmp_9,10,0xc0,&palette);
  }
  __chdir(saved_cwd);
  ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->release();
  return;
}

/* 00572089 auto */
/* WARNING: Removing unreachable block (ram,0x0057228e) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
sprite_editor_create_window(const char *title,uint style_flags,int x,int y,int width,int height,int resize_border,
            int caption_height,int border_width)

{
  iptr parent_hwnd;
  uint wnd_style;
  
  if (title == 0) {
    crt_strcpy(((i8 *)&sprite_editor.title),((i8 *)&sprite_editor_default_title));
  }
  else {
    crt_strcpy(((i8 *)&sprite_editor.title),title);
  }
  sprite_editor.text_font = (iptr)&ui_text_font;
  sprite_editor.title_font = (iptr)&caption_font_block.caption_font;
  sprite_editor.title_small_font = (iptr)&caption_small_font_block.caption_small_font;
  sprite_editor.style_flags = style_flags;
  sprite_editor.caption_height = caption_height;
  sprite_editor.border_width = border_width;
  sprite_editor.resize_border = resize_border;
  sprite_editor.caption_button_count = 0;
  sprite_editor.caption_button_cb = 0;
  if ((style_flags & 4) != 0) {
    sprite_editor.caption_height = ui_metrics.caption_height;
    sprite_editor.border_width = ui_metrics.border_width;
  }
  if ((style_flags & 8) == 0) {
    wnd_style = 0x202;
  }
  else {
    wnd_style = 0x802;
  }
  if (sprite_editor.caption_height != 0) {
    wnd_style = wnd_style | 0x400;
  }
  if (resize_border != 0) {
    wnd_style = wnd_style | 0x1000;
  }
  if ((style_flags & 2) == 0) {
    width += sprite_editor.border_width * 2;
    height = height + sprite_editor.caption_height + sprite_editor.border_width;
  }
  if ((style_flags & 1) != 0) {
    x = (caption_font_block.screen_width >> 1) - (width >> 1);
    y = (caption_font_block.screen_height >> 1) - (height >> 1);
  }
  if (editor_active.wnd == 0) {
    parent_hwnd = 0;
  }
  else {
    parent_hwnd = (iptr)&editor_active.wnd->window;
  }
  ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_create_with_surface(0,wnd_style,x,y,width,height,(GamePalette *)&game_palette_block.game_palette,
             (WndBase *)parent_hwnd);
  if (sprite_editor.caption_height != 0) {
    ((WndBase *)((i8 *)&sprite_editor.wnd))->set_caption_height(sprite_editor.caption_height);
  }
  if (sprite_editor.resize_border != 0) {
    ((WndBase *)((i8 *)&sprite_editor.wnd))->set_resize_border(sprite_editor.resize_border);
  }
  ((FrameWindow *)((i8 *)&sprite_editor.window))->layout();
  dlg_set_parent_window((iptr)&sprite_editor.wnd);
  return;
}

/* 005722ab auto */
void __cdecl sprite_editor_ok_clicked(void)

{
  i8 rect[16];
  
  rect_set_xywh((RECT *)rect,0,0,sprite_editor.width,sprite_editor.height);
  ((DibSurface *)((i8 *)&sprite_editor.surface))->replace_color_clip(0x109,0x107);
  sprite_editor.target_sprite->grab_surface_rect(&sprite_editor.surface,(RECT *)rect);
  road_sprites_block.unit_shield_x_offsets[editor_active.wnd->item] = sprite_editor.flag_x;
  road_sprites_block.unit_shield_y_offsets[editor_active.wnd->item] = sprite_editor.flag_y;
  if (sprite_editor.ok_callback != 0) {
    sprite_editor.ok_callback();
  }
  sprite_editor.running = 0;
  ((ModalMenuWindow *)((i8 *)&sprite_editor.wnd))->end_modal_loop();
  ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  return;
}

/* 00572364 auto */
void __cdecl sprite_editor_cancel_clicked(void)

{
  sprite_editor.running = 0;
  ((ModalMenuWindow *)((i8 *)&sprite_editor.wnd))->end_modal_loop();
  return;
}

/* 00572389 auto */
void __cdecl sprite_editor_rebuild_sprite(void)

{
  i8 rect[16];
  
  ((Sprite *)((i8 *)&sprite_editor.result_sprite))->free();
  rect_set_xywh((RECT *)rect,0,0,sprite_editor.width,sprite_editor.height);
  ((Sprite *)((i8 *)&sprite_editor.result_sprite))->grab_rect(&sprite_editor.surface,0xfe,(RECT *)rect);
  sprite_editor.undo_flag_x = sprite_editor.flag_x;
  sprite_editor.undo_flag_y = sprite_editor.flag_y;
  return;
}

/* 005723ee auto */
void __cdecl sprite_editor_draw_swatches(void)

{
  int prev_right_color;
  int prev_left_color;
  int left_color;
  int right_color;
  i8 rect[16];
  
  if (sprite_editor_colors.right == 0) {
    right_color = (*(int *)&sprite_editor.key_color);
  }
  else {
    right_color = sprite_editor_colors.right;
  }
  gfx_fill_rect_xywh(&sprite_editor.window,sprite_editor.origin_x + 0x142,sprite_editor.origin_y + 0xac,0x20,0x20,right_color);
  gfx_draw_frame_rect(&sprite_editor.window,sprite_editor.origin_x + 0x142,sprite_editor.origin_y + 0xac,0x20,0x20,10);
  if (sprite_editor_colors.left == 0) {
    left_color = (*(int *)&sprite_editor.key_color);
  }
  else {
    left_color = sprite_editor_colors.left;
  }
  gfx_fill_rect_xywh(&sprite_editor.window,sprite_editor.origin_x + 0x14a,sprite_editor.origin_y + 0xb4,0x10,0x10,left_color);
  gfx_draw_frame_rect(&sprite_editor.window,sprite_editor.origin_x + 0x14a,sprite_editor.origin_y + 0xb4,0x10,0x10,10);
  if (sprite_editor_colors.prev_left == 0) {
    prev_left_color = (*(int *)&sprite_editor.key_color);
  }
  else {
    prev_left_color = sprite_editor_colors.prev_left;
  }
  gfx_fill_rect_xywh(&sprite_editor.window,sprite_editor.origin_x + 0x11e,sprite_editor.origin_y + 0xac,0x20,0x20,prev_left_color);
  gfx_draw_frame_rect(&sprite_editor.window,sprite_editor.origin_x + 0x11e,sprite_editor.origin_y + 0xac,0x20,0x20,10);
  if (sprite_picker_sprite_block.sprite_sheet_mode != 0xb) {
    if (sprite_editor_colors.prev_right == 0) {
      prev_right_color = (*(int *)&sprite_editor.key_color);
    }
    else {
      prev_right_color = sprite_editor_colors.prev_right;
    }
    gfx_fill_rect_xywh(&sprite_editor.window,sprite_editor.origin_x + 0x166,sprite_editor.origin_y + 0xac,0x20,0x20,prev_right_color);
    gfx_draw_frame_rect(&sprite_editor.window,sprite_editor.origin_x + 0x166,sprite_editor.origin_y + 0xac,0x20,0x20,10);
  }
  rect_set_xywh((RECT *)rect,sprite_editor.origin_x + 0x11e,sprite_editor.origin_y + 0xac,0x68,0x20);
  ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface_rect((RECT *)rect);
  return;
}

/* 0057261a auto */
void __cdecl sprite_editor_draw_palette_cell(DibSurface *surface,int origin_x,int origin_y,int color)

{
  int col;
  int row;
  uint sign;
  i8 rect[16];
  
  if ((9 < color) && (color < 0xca)) {
    sign = (int)(color - 10U) >> 0x1f;
    col = ((color - 10U ^ sign) - sign & 0x1f ^ sign) - sign;
    row = (int)(color - 10 + (color - 10 >> 0x1f & 0x1fU)) >> 5;
    rect_set_xywh((RECT *)rect,col * 0xc + origin_x,row * 0xc + origin_y,10,10);
    gfx_fill_rect(&sprite_editor.window,(RECT *)rect,color);
    gfx_draw_bevel_rect(&sprite_editor.window,(RECT *)rect,10,10);
    rect_set_xywh((RECT *)rect,col * 0xc + origin_x - 1,row * 0xc + origin_y - 1,0xc,0xc)
    ;
    if ((color == sprite_editor_colors.right) || (color == sprite_editor_colors.left)) {
      gfx_draw_bevel_rect(surface,(RECT *)rect,0x29,0x29);
    }
    else {
      gfx_draw_bevel_rect(surface,(RECT *)rect,0x1d,0x1d);
    }
    (surface)->wnd_blit_surface_rect((RECT *)rect);
  }
  return;
}

/* 00572740 auto */
int __cdecl sprite_editor_tool_hit_test(int x,int y)

{
  int rel_y;
  int rel_x;
  
  rel_x = x - 0x11e;
  rel_y = y - 0x14;
  if ((((rel_x < 0) || (0x6c < rel_x)) || (rel_y < 0)) ||
     (((0xbe < rel_y || (0x20 < rel_x % 0x24)) || (0x20 < rel_y % 0x26)))) {
    rel_x = -1;
  }
  else {
    rel_x = rel_x / 0x24 + (rel_y / 0x26) * 3;
  }
  return rel_x;
}

/* 005727d8 auto */
void __cdecl sprite_editor_draw_backdrop(void)

{
  i8 tile_rect[16];
  i8 unit_rect[16];
  
  ((DibSurface *)((i8 *)&sprite_editor.surface))->fill((*(i32 *)&sprite_editor.key_color));
  switch(sprite_picker_sprite_block.sprite_sheet_mode) {
  case 1:
  case 2:
  case 0xb:
    ((Sprite *)((i8 *)&editor_unit_backdrop_sprite))->draw((RECT *)unit_rect,&sprite_editor.surface,0,0);
    break;
  case 5:
  case 7:
  case 8:
  case 10:
    ((Sprite *)((i8 *)&editor_tile_backdrop_sprite_block.editor_tile_backdrop_sprite))->draw((RECT *)tile_rect,&sprite_editor.surface,0,0);
  case 3:
  case 4:
  case 6:
  case 9: ;
  }
  return;
}

/* 00572887 auto */
i8 * __cdecl sprite_editor_plot_pixel(int px,int py)

{
  int color8;
  int color7;
  int color6;
  int color5;
  int color4;
  int color3;
  int color2;
  int color1;
  int screen_color;
  int pixel_color;
  i8 saved_clip[16];
  
  if (sprite_editor.brush_size == 0) {
    if (sprite_editor.paint_color == 0) {
      pixel_color = (*(int *)&sprite_editor.key_color);
    }
    else {
      pixel_color = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->set_pixel(px,py,pixel_color);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0x10,sprite_editor.origin_y + py * 4 + 0x10,
                       4,4);
    if (sprite_editor.paint_color == 0) {
      screen_color = (*(int *)&sprite_editor.key_color);
    }
    else {
      screen_color = sprite_editor.paint_color;
    }
    gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,screen_color);
  }
  else if (sprite_editor.brush_size == 1) {
    ((DibSurface *)((i8 *)&sprite_editor.window))->get_clip((RECT *)saved_clip);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,sprite_editor.origin_x + 0x10,sprite_editor.origin_y + 0x10,sprite_editor.width << 2,
                       sprite_editor.height << 2);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)&sprite_editor.plot_rect);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px - 1,py,3,1);
    if (sprite_editor.paint_color == 0) {
      color1 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color1 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((RECT *)&sprite_editor.plot_rect,color1);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,sprite_editor.origin_y + py * 4 + 0x10,
                       0xc,4);
    if (sprite_editor.paint_color == 0) {
      color2 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color2 = sprite_editor.paint_color;
    }
    gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color2);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px,py - 1,1,3);
    if (sprite_editor.paint_color == 0) {
      color3 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color3 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((RECT *)&sprite_editor.plot_rect,color3);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0x10,py * 4 + sprite_editor.origin_y + 0xc,4
                       ,0xc);
    if (sprite_editor.paint_color == 0) {
      color4 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color4 = sprite_editor.paint_color;
    }
    gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color4);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)saved_clip);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,py * 4 + sprite_editor.origin_y + 0xc,
                       0xc,0xc);
  }
  else if (sprite_editor.brush_size == 2) {
    ((DibSurface *)((i8 *)&sprite_editor.window))->get_clip((RECT *)saved_clip);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,sprite_editor.origin_x + 0x10,sprite_editor.origin_y + 0x10,sprite_editor.width << 2,
                       sprite_editor.height << 2);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)&sprite_editor.plot_rect);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px - 1,py,4,2);
    if (sprite_editor.paint_color == 0) {
      color5 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color5 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((RECT *)&sprite_editor.plot_rect,color5);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,sprite_editor.origin_y + py * 4 + 0x10,
                       0x10,8);
    if (sprite_editor.paint_color == 0) {
      color6 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color6 = sprite_editor.paint_color;
    }
    gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color6);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px,py - 1,2,4);
    if (sprite_editor.paint_color == 0) {
      color7 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color7 = sprite_editor.paint_color;
    }
    ((DibSurface *)((i8 *)&sprite_editor.surface))->fill_rect((RECT *)&sprite_editor.plot_rect,color7);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0x10,py * 4 + sprite_editor.origin_y + 0xc,8
                       ,0x10);
    if (sprite_editor.paint_color == 0) {
      color8 = (*(int *)&sprite_editor.key_color);
    }
    else {
      color8 = sprite_editor.paint_color;
    }
    gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.plot_rect,color8);
    ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)saved_clip);
    rect_set_xywh((RECT *)&sprite_editor.plot_rect,px * 4 + sprite_editor.origin_x + 0xc,py * 4 + sprite_editor.origin_y + 0xc,
                       0x10,0x10);
  }
  return &sprite_editor.plot_rect;
}

/* 00572da0 auto */
void __cdecl sprite_editor_draw_flag_marker(void)

{
  int flag_y;
  i8 saved_clip[16];
  int flag_x;
  
  ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
  flag_y = sprite_editor.flag_y;
  flag_x = sprite_editor.flag_x;
  ((DibSurface *)((i8 *)&sprite_editor.window))->get_clip((RECT *)saved_clip);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,sprite_editor.origin_x + 0x10,sprite_editor.origin_y + 0x10,sprite_editor.width << 2,
                     sprite_editor.height << 2);
  ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)&sprite_editor.marker_rect);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,sprite_editor.origin_x + 0x20,sprite_editor.origin_y + 0x10,4,0x3c);
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x6a);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,sprite_editor.origin_x + 0x20,sprite_editor.origin_y + 0x4c,0xb4,4);
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x6a);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,sprite_editor.origin_x + 0xd0,sprite_editor.origin_y + 0x10,4,0x3c);
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x6a);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 8,sprite_editor.origin_y + flag_y * 4 + 0x10,0x14,4)
  ;
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,10);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 0x10,flag_y * 4 + sprite_editor.origin_y + 8,4,0x14)
  ;
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,10);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 9,sprite_editor.origin_y + flag_y * 4 + 0x11,0x12,2)
  ;
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x29);
  rect_set_xywh((RECT *)&sprite_editor.marker_rect,flag_x * 4 + sprite_editor.origin_x + 0x11,flag_y * 4 + sprite_editor.origin_y + 9,2,0x12)
  ;
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.marker_rect,0x29);
  ((DibSurface *)((i8 *)&sprite_editor.window))->set_clip((RECT *)saved_clip);
  ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface();
  return;
}

/* Part of sprite_editor_on_mouse_down (00572fff): a click
   on a tool button (tool_index): pick, pencil, brush, undo, the colour
   swaps, brush sizes, the flag marker. x/y are zeroed by the colour tools;
   old_color/new_color: the caller's (read stale by the swatch redraws). */
static void sprite_editor_tool_click(i32 tool_index,iptr old_flag_x,iptr old_flag_y,Sprite &temp_sprite,
                                                int &x,int &y,int &old_color,int &new_color)
{
  SHORT button_state;
  int temp_x;
  int temp_y;
  i8 draw_rect[16];
  iptr saved_flag_y;
  iptr saved_flag_x;
  i8 grab_rect[16];

  switch(tool_index) {
  case 0:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    button_state = GetKeyState(1);
    if (((int)button_state & 0x8000U) != 0) {
      sprite_editor_colors.prev_left = sprite_editor_colors.left;
      old_color = sprite_editor_colors.left;
      sprite_editor_colors.left = 0;
    }
    button_state = GetKeyState(2);
    if (((int)button_state & 0x8000U) != 0) {
      sprite_editor_colors.prev_right = sprite_editor_colors.right;
      old_color = sprite_editor_colors.right;
      sprite_editor_colors.right = 0;
    }
    sprite_editor_draw_swatches();
    sprite_editor_draw_palette_cell(&sprite_editor.window,sprite_editor.origin_x + 10,sprite_editor.origin_y + 0xe0,old_color);
    y = 0;
    x = 0;
    break;
  case 1:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = 1;
    sprite_editor.paint_tool = 1;
    break;
  case 2:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = 2;
    sprite_editor.paint_tool = 2;
    break;
  case 3:
    saved_flag_x = sprite_editor.flag_x;
    saved_flag_y = sprite_editor.flag_y;
    sprite_editor.flag_x = sprite_editor.undo_flag_x;
    sprite_editor.flag_y = sprite_editor.undo_flag_y;
    sprite_editor.undo_flag_x = old_flag_x;
    sprite_editor.undo_flag_y = old_flag_y;
    temp_sprite.free();
    rect_set_xywh((RECT *)grab_rect,0,0,sprite_editor.width,sprite_editor.height);
    temp_sprite.grab_rect(&sprite_editor.surface,0xfe,(RECT *)grab_rect);
    ((Sprite *)((i8 *)&sprite_editor.result_sprite))->draw_keyed((RECT *)draw_rect,&sprite_editor.surface,0xfe,0,0);
    ((Sprite *)((i8 *)&sprite_editor.result_sprite))->free();
    temp_sprite.merge_onto((Sprite *)&sprite_editor.result_sprite);
    sprite_editor_redraw();
    if (sprite_editor.tool == 3) {
      sprite_editor_draw_flag_marker();
    }
    break;
  case 4:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = 0;
    break;
  case 5:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor_rebuild_sprite();
    sprite_editor_draw_backdrop();
    sprite_editor_redraw();
    break;
  case 6:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor_rebuild_sprite();
    ((DibSurface *)((i8 *)&sprite_editor.surface))->replace_color_clip(sprite_editor_colors.right,sprite_editor_colors.left);
    sprite_editor_redraw();
    break;
  case 7:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor_rebuild_sprite();
    ((DibSurface *)((i8 *)&sprite_editor.surface))->replace_color_clip(sprite_editor_colors.left,1);
    ((DibSurface *)((i8 *)&sprite_editor.surface))->replace_color_clip(sprite_editor_colors.right,sprite_editor_colors.left);
    ((DibSurface *)((i8 *)&sprite_editor.surface))->replace_color_clip(1,sprite_editor_colors.right);
    sprite_editor_redraw();
    break;
  case 8:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor_import_sheet();
    break;
  case 9:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor.brush_size = 0;
    break;
  case 10:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor.brush_size = 1;
    break;
  case 0xb:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor.brush_size = 2;
    break;
  case 0xc:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    button_state = GetKeyState(1);
    temp_x = sprite_editor_colors.left;
    if (((int)button_state & 0x8000U) != 0) {
      old_color = sprite_editor_colors.left;
      sprite_editor_colors.left = sprite_editor_colors.prev_left;
      new_color = sprite_editor_colors.prev_left;
      sprite_editor_colors.prev_left = temp_x;
    }
    button_state = GetKeyState(2);
    temp_x = sprite_editor_colors.right;
    if (((int)button_state & 0x8000U) != 0) {
      old_color = sprite_editor_colors.right;
      sprite_editor_colors.right = sprite_editor_colors.prev_left;
      new_color = sprite_editor_colors.prev_left;
      sprite_editor_colors.prev_right = temp_x;
    }
    sprite_editor_draw_swatches();
    temp_x = sprite_editor.origin_x + 10;
    temp_y = sprite_editor.origin_y + 0xe0;
    sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,old_color);
    sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,new_color);
    y = 0;
    x = 0;
    break;
  case 0xd:
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    break;
  case 0xe:
    if (sprite_picker_sprite_block.sprite_sheet_mode == 0xb) {
      sprite_editor_rebuild_sprite();
      sprite_editor.tool = 3;
      sprite_editor.flag_x = int_clamp(sprite_editor.flag_x,5,0x2f);
      sprite_editor.flag_y = int_clamp(sprite_editor.flag_y,0,0xe);
      sprite_editor_draw_flag_marker();
    }
    else {
      if (sprite_editor.tool == 3) {
        ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
      }
      sprite_editor.tool = sprite_editor.paint_tool;
      button_state = GetKeyState(1);
      temp_x = sprite_editor_colors.left;
      if (((int)button_state & 0x8000U) != 0) {
        old_color = sprite_editor_colors.left;
        sprite_editor_colors.left = sprite_editor_colors.prev_right;
        new_color = sprite_editor_colors.prev_right;
        sprite_editor_colors.prev_left = temp_x;
      }
      button_state = GetKeyState(2);
      temp_x = sprite_editor_colors.right;
      if (((int)button_state & 0x8000U) != 0) {
        old_color = sprite_editor_colors.right;
        sprite_editor_colors.right = sprite_editor_colors.prev_right;
        new_color = sprite_editor_colors.prev_right;
        sprite_editor_colors.prev_right = temp_x;
      }
      sprite_editor_draw_swatches();
      temp_x = sprite_editor.origin_x + 10;
      temp_y = sprite_editor.origin_y + 0xe0;
      sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,old_color);
      sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,new_color);
      y = 0;
      x = 0;
    }
  }
}

/* Part of sprite_editor_on_mouse_down (00572fff): a click on the
   enlarged sprite with the current tool: pick a colour, paint, or move the
   flag. old_color/new_color: the caller's. */
static void sprite_editor_canvas_click(int x,int y,int &old_color,int &new_color)
{
  SHORT button_state;
  int temp_x;
  int temp_y;
  iptr dirty_rect;

  switch(sprite_editor.tool) {
  case 0:
    new_color = ((DibSurface *)((i8 *)&sprite_editor.surface))->get_pixel(x - 0x10 >> 2,
                          y - 0x10 >> 2);
    button_state = GetKeyState(1);
    if (((int)button_state & 0x8000U) == 0) {
      button_state = GetKeyState(2);
      if (((int)button_state & 0x8000U) != 0) {
        sprite_editor_colors.prev_right = sprite_editor_colors.right;
        old_color = sprite_editor_colors.right;
        sprite_editor_colors.right = new_color;
      }
    }
    else {
      sprite_editor_colors.prev_left = sprite_editor_colors.left;
      old_color = sprite_editor_colors.left;
      sprite_editor_colors.left = new_color;
    }
    sprite_editor_draw_swatches();
    temp_x = sprite_editor.origin_x + 10;
    temp_y = sprite_editor.origin_y + 0xe0;
    sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,old_color);
    sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,new_color);
    if (sprite_editor.tool == 3) {
      ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
    }
    sprite_editor.tool = sprite_editor.paint_tool;
    sprite_editor.cursor_id = 0x204;
    ((WndBase *)((i8 *)&sprite_editor.wnd))->set_cursor(0x204);
    ui_flush_input();
    break;
  case 1:
  case 2:
    sprite_editor_rebuild_sprite();
    sprite_editor.painting = 1;
    temp_x = (int)(x - 0x10 + (x - 0x10 >> 0x1f & 3U)) >> 2;
    temp_y = (int)(y - 0x10 + (y - 0x10 >> 0x1f & 3U)) >> 2;
    sprite_editor.last_px = temp_x;
    sprite_editor.last_py = temp_y;
    button_state = GetKeyState(1);
    if (((int)button_state & 0x8000U) == 0) {
      button_state = GetKeyState(2);
      if (((int)button_state & 0x8000U) != 0) {
        sprite_editor.paint_color = sprite_editor_colors.right;
      }
    }
    else {
      sprite_editor.paint_color = sprite_editor_colors.left;
    }
    dirty_rect = (iptr)sprite_editor_plot_pixel(temp_x,temp_y);
    ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface_rect((RECT *)dirty_rect);
    break;
  case 3:
    sprite_editor_rebuild_sprite();
    sprite_editor.flag_x = int_clamp((int)(x - 0x10 + (x - 0x10 >> 0x1f & 3U)) >>
                                      2,5,0x2f);
    sprite_editor.flag_y = int_clamp((int)(y - 0x10 + (y - 0x10 >> 0x1f & 3U)) >>
                                      2,0,0xe);
    sprite_editor_draw_flag_marker();
  }
}

/* Part of sprite_editor_on_mouse_down (00572fff): a click on the
   palette: the left or right colour set from the cell. old_color/new_color:
   the caller's. */
static void sprite_editor_palette_click(int x,int y,int &old_color,int &new_color)
{
  SHORT button_state;
  int temp_x;
  int temp_y;

  button_state = GetKeyState(1);
  if (((int)button_state & 0x8000U) == 0) {
    button_state = GetKeyState(2);
    if (((int)button_state & 0x8000U) != 0) {
      sprite_editor_colors.prev_right = sprite_editor_colors.right;
      old_color = sprite_editor_colors.right;
      sprite_editor_colors.right = ((y - 0xdf) / 0xc) * 0x20 + (x - 9) / 0xc + 10;
      new_color = sprite_editor_colors.right;
    }
  }
  else {
    sprite_editor_colors.prev_left = sprite_editor_colors.left;
    old_color = sprite_editor_colors.left;
    sprite_editor_colors.left = ((y - 0xdf) / 0xc) * 0x20 + (x - 9) / 0xc + 10;
    new_color = sprite_editor_colors.left;
  }
  sprite_editor_draw_swatches();
  temp_x = sprite_editor.origin_x + 10;
  temp_y = sprite_editor.origin_y + 0xe0;
  sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,old_color);
  sprite_editor_draw_palette_cell(&sprite_editor.window,temp_x,temp_y,new_color);
  if (sprite_editor.tool == 3) {
    ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
  }
  sprite_editor.tool = sprite_editor.paint_tool;
}

/* 00572fff auto */
void __cdecl sprite_editor_on_mouse_down(int x,int y)

{
  iptr old_flag_y;
  iptr old_flag_x;
  NativeWindow *window;
  i32 tool_index;
  int old_color;
  int new_color;
  Sprite temp_sprite;
  
  sprite_init(&temp_sprite);
  ON_SCOPE_EXIT(sprite_destroy(&temp_sprite));
  window = ((WndBase *)((i8 *)&sprite_editor.wnd))->get_window();
  SetCapture(window->hwnd);
  x -= sprite_editor.origin_x;
  y -= sprite_editor.origin_y;
  tool_index = sprite_editor_tool_hit_test(x,y);
  old_flag_x = sprite_editor.flag_x;
  old_flag_y = sprite_editor.flag_y;
  sprite_editor_tool_click(tool_index,old_flag_x,old_flag_y,temp_sprite,x,y,old_color,new_color);
  if ((((x < 9) || (0x187 < x)) || (y < 0xdf)) || (0x125 < y)) {
    if (((0xf < x) && (x < sprite_editor.width * 4 + 0x10)) &&
       ((0xf < y && (y < sprite_editor.height * 4 + 0x10)))) {
      sprite_editor_canvas_click(x,y,old_color,new_color);
    }
  }
  else {
    sprite_editor_palette_click(x,y,old_color,new_color);
  }
  return;
}

/* 00573afd auto */
void __cdecl sprite_editor_on_mouse_up(void)

{
  sprite_editor.painting = 0;
  ReleaseCapture();
  return;
}

/* 00573b1d auto */
void __cdecl sprite_editor_on_mouse_move(int x,int y)

{
  int px;
  int py;
  iptr dirty_rect;
  int dx;
  int dy;
  int cur_x;
  int cur_y;
  int prev_x;
  int fixed_y;
  int fixed_x;
  int prev_y;
  
  x -= sprite_editor.origin_x;
  y -= sprite_editor.origin_y;
  if ((((x < 9) || (0x187 < x)) || (y < 0xdf)) || (0x125 < y)) {
    if (((x < 0x10) || (sprite_editor.width * 4 + 0x10 <= x)) ||
       ((y < 0x10 || (sprite_editor.height * 4 + 0x10 <= y)))) {
      if (sprite_editor.cursor_id != 0x201) {
        sprite_editor.cursor_id = 0x201;
        ((WndBase *)((i8 *)&sprite_editor.wnd))->set_cursor(0x201);
        ui_flush_input();
      }
      sprite_editor.last_py = -1;
      sprite_editor.last_px = -1;
      px = sprite_editor.last_px;
      py = sprite_editor.last_py;
    }
    else {
      if (sprite_editor.tool == 0) {
        if (sprite_editor.cursor_id != 0x205) {
          sprite_editor.cursor_id = 0x205;
          ((WndBase *)((i8 *)&sprite_editor.wnd))->set_cursor(0x205);
          ui_flush_input();
        }
      }
      else if (((0 < sprite_editor.tool) && (sprite_editor.tool < 3)) && (sprite_editor.cursor_id != 0x204)) {
        sprite_editor.cursor_id = 0x204;
        ((WndBase *)((i8 *)&sprite_editor.wnd))->set_cursor(0x204);
        ui_flush_input();
      }
      px = sprite_editor.last_px;
      py = sprite_editor.last_py;
      if (sprite_editor.painting != 0) {
        px = (int)(x - 0x10 + (x - 0x10 >> 0x1f & 3U)) >> 2;
        py = (int)(y - 0x10 + (y - 0x10 >> 0x1f & 3U)) >> 2;
        if (sprite_editor.tool == 1) {
          if ((sprite_editor.last_px < 0) || (sprite_editor.last_py < 0)) {
            sprite_editor.last_px = px;
            sprite_editor.last_py = py;
          }
          dx = px - sprite_editor.last_px;
          dy = py - sprite_editor.last_py;
          prev_y = -1;
          prev_x = -1;
          fixed_x = sprite_editor.last_px << 8;
          fixed_y = sprite_editor.last_py << 8;
          while( true ) {
            cur_x = fixed_x >> 8;
            cur_y = fixed_y >> 8;
            if ((cur_x == px) && (cur_y == py)) break;
            if ((cur_x != prev_x) || (cur_y != prev_y)) {
              sprite_editor_plot_pixel(cur_x,cur_y);
              prev_x = cur_x;
              prev_y = cur_y;
            }
            fixed_x += dx;
            fixed_y += dy;
          }
          ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface();
        }
        else if (sprite_editor.tool == 2) {
          dirty_rect = (iptr)sprite_editor_plot_pixel(px,py);
          ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface_rect((RECT *)dirty_rect);
        }
      }
    }
  }
  else {
    px = sprite_editor.last_px;
    py = sprite_editor.last_py;
    if (sprite_editor.cursor_id != 0x205) {
      sprite_editor.cursor_id = 0x205;
      ((WndBase *)((i8 *)&sprite_editor.wnd))->set_cursor(0x205);
      ui_flush_input();
      px = sprite_editor.last_px;
      py = sprite_editor.last_py;
    }
  }
  sprite_editor.last_py = py;
  sprite_editor.last_px = px;
  return;
}

/* 00573e59 auto */
/* WARNING: Removing unreachable block (ram,0x0057405e) */
/* WARNING: Removing unreachable block (ram,0x005740e7) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl sprite_editor_run(Sprite *sprite,const char *title,i32 sheet_mode,SpriteSheetSaveFn ok_callback)

{
  byte key_color;
  int line_height;
  iptr label;
  i8 draw_rect[16];
  int button_y2;
  Button cancel_button;
  int button_height;
  int button_x;
  int button_width2;
  i8 button_rect[16];
  Button ok_button;
  
  button_ctor(&ok_button);
  ON_SCOPE_EXIT(ctrl_dtor(&ok_button));
  button_ctor(&cancel_button);
  ON_SCOPE_EXIT(ctrl_dtor(&cancel_button));
  sprite_picker_sprite_block.sprite_sheet_mode = sheet_mode;
  sprite_editor.target_sprite = sprite;
  sprite_editor.width = (sprite)->width();
  sprite_editor.height = sprite_editor.target_sprite->height();
  key_color = sprite_editor.target_sprite->get_key_color();
  sprite_editor.key_color = (uint)key_color;
  sprite_editor.ok_callback = ok_callback;
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->free();
  sprite_editor.target_sprite->merge_onto((Sprite *)&sprite_editor.orig_sprite);
  ((DibSurface *)((i8 *)&sprite_editor.surface))->DibSurface::create(sprite_editor.width,sprite_editor.height);
  sprite_editor_draw_backdrop();
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->draw((RECT *)draw_rect,&sprite_editor.surface,0,0);
  sprite_editor.undo_flag_x = road_sprites_block.unit_shield_x_offsets[editor_active.wnd->item];
  sprite_editor.undo_flag_y = road_sprites_block.unit_shield_y_offsets[editor_active.wnd->item];
  sprite_editor.flag_y = sprite_editor.undo_flag_y;
  sprite_editor.flag_x = sprite_editor.undo_flag_x;
  sprite_editor_rebuild_sprite();
  sprite_editor.tool = 1;
  sprite_editor.paint_tool = 1;
  sprite_editor.brush_size = 0;
  sprite_editor.cursor_id = 0x201;
  sprite_editor.running = 1;
  sprite_editor.painting = 0;
  sprite_editor_create_window(title,0xd,0,0,0x192,0x14e,0,0,0);
  line_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height = line_height + 8;
  button_width2 = (sprite_editor.client_width - 6) / 2;
  button_y2 = (sprite_editor.origin_y + sprite_editor.client_height) - (line_height + 10);
  button_x = sprite_editor.origin_x + 2;
  rect_set_xywh((RECT *)button_rect,button_x,button_y2,button_width2,button_height);
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  ok_button.create((Wnd *)&sprite_editor.wnd,0x65,(RECT *)button_rect,(const char *)label);
  ok_button.set_callback((CtrlNotifyFn)sprite_editor_ok_clicked);
  button_x = button_x + button_width2 + 2;
  rect_set_xywh((RECT *)button_rect,button_x,button_y2,button_width2,button_height);
  label = (iptr)label_string(label_table.string_ids[0xff]);
  cancel_button.create((Wnd *)&sprite_editor.wnd,0x66,(RECT *)button_rect,(const char *)label);
  cancel_button.set_callback((CtrlNotifyFn)sprite_editor_cancel_clicked);
  cancel_button.set_cancel_button();
  ((WndHandlers *)((i8 *)&sprite_editor.wnd_events))->set_lbuttondown_handler(sprite_editor_on_mouse_down);
  ((WndHandlers *)((i8 *)&sprite_editor.wnd_events))->set_rbuttondown_handler(sprite_editor_on_mouse_down);
  ((WndHandlers *)((i8 *)&sprite_editor.wnd_events))->set_dblclick_handler(sprite_editor_on_mouse_down);
  ((WndHandlers *)((i8 *)&sprite_editor.wnd_events))->set_lbuttonup_any_handler((WndMouseFn)sprite_editor_on_mouse_up);
  ((WndHandlers *)((i8 *)&sprite_editor.wnd_events))->set_rbuttonup_any_handler((WndMouseFn)sprite_editor_on_mouse_up);
  ((WndHandlers *)((i8 *)&sprite_editor.wnd_events))->set_mousemove_handler(sprite_editor_on_mouse_move);
  ((SurfaceWindow *)((i8 *)&sprite_editor.window))->set_redraw_callback(sprite_editor_redraw);
  ((SurfaceWindow *)((i8 *)&sprite_editor.window))->repaint();
  ((SurfaceWindow *)((i8 *)&sprite_editor.window))->SurfaceWindow::show();
  ((ModalMenuWindow *)((i8 *)&sprite_editor.wnd))->run_modal_loop();
  while (sprite_editor.running != 0) {
    win_pump_one_message();
  }
  dlg_set_parent_window(0);
  ((FrameWindow *)((i8 *)&sprite_editor.window))->FrameWindow::close();
  return;
}

/* 00574239 auto */
void __cdecl sprite_editor_redraw(void)

{
  byte key_color;
  i8 tool_rect[16];
  i8 sprite_rect[16];
  int row;
  int col;
  int i;
  int y2;
  int x2;
  int height2;
  int width2;
  i8 grab_rect[16];
  
  ((FrameWindow *)((i8 *)&sprite_editor.window))->FrameWindow::draw_caption();
  gfx_fill_rect(&sprite_editor.window,(RECT *)&sprite_editor.fill_rect,0x1d);
  x2 = sprite_editor.origin_x + 0x10;
  y2 = sprite_editor.origin_y + 0x10;
  width2 = sprite_editor.width << 2;
  height2 = sprite_editor.height << 2;
  rect_set_xywh((RECT *)grab_rect,0,0,sprite_editor.width,sprite_editor.height);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->grab_surface_rect(&sprite_editor.surface,(RECT *)grab_rect);
  key_color = ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->get_key_color();
  sprite_editor.key_color = (uint)key_color;
  gfx_fill_rect_xywh(&sprite_editor.window,x2,y2,width2,height2,sprite_editor.key_color);
  gfx_set_scale_ratio(4,1);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->draw_keyed((RECT *)sprite_rect,&sprite_editor.window,0,x2,y2);
  gfx_set_scale_ratio(1,1);
  gfx_draw_tiled_frame(&sprite_editor.window,x2,y2,width2,height2,6);
  x2 = sprite_editor.origin_x + 10;
  y2 = sprite_editor.origin_y + 0xe0;
  for (i = 0; i < 0xc0; i++) {
    sprite_editor_draw_palette_cell(&sprite_editor.window,x2,y2,i + 10);
  }
  x2 = sprite_editor.origin_x + 0x11e;
  y2 = sprite_editor.origin_y + 0x14;
  for (col = 0; col < 3; col++) {
    for (row = 0; row < 5; row++) {
      if (row == 0 && col == 0) {
        gfx_fill_rect_xywh(&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2,0x20,0x20,
                           sprite_editor.key_color);
      }
      else {
        gfx_fill_rect_xywh(&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2,0x20,0x20,
                           0x29);
        ((Sprite *)(((i8 *)&caption_zoom_out_button_sprite_block.sprite_editor_tool_sprites) + (row * 3 + col) * 0x3c))->draw(
                   (RECT *)tool_rect,&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2);
      }
      gfx_draw_frame_rect(&sprite_editor.window,col * 0x24 + x2,row * 0x26 + y2,0x20,0x20,10
                        );
    }
  }
  sprite_editor_draw_swatches();
  text_set_surface(&sprite_editor.window);
  text_set_font(&editor_font_block.editor_font);
  text_set_colors(0x29,0x12,1,1);
  ((DibSurface *)((i8 *)&sprite_editor.window))->wnd_blit_surface();
  return;
}

/* 00574522 auto */
void SpritePicker::draw()

{
  byte key_color;
  i8 draw_rect[16];
  int button_height;
  int y2;
  int x2;
  int height2;
  int width2;
  int hscroll_height;
  
  draw_caption();
  gfx_fill_rect(this,(RECT *)&client_rect,0x1d);
  text_set_surface(this);
  text_set_font((GameFont *)&ui_text_font);
  key_color = ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->get_key_color();
  sprite_editor.key_color = (uint)key_color;
  width2 = ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->width();
  width2 = width2 * 2;
  height2 = ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->height();
  height2 = height2 * 2;
  button_height = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  button_height += 8;
  hscroll_height = GetSystemMetrics(3);
  hscroll_height += 2;
  x2 = client_x + (int)(client_width - width2) / 2;
  y2 = client_y + (int)((((client_height - height2) - button_height) - hscroll_height) - 4) / 2
  ;
  gfx_fill_rect_xywh(this,x2,y2,width2,height2,sprite_editor.key_color);
  gfx_set_scale_ratio(2,1);
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->draw_keyed((RECT *)draw_rect,this,0,x2,y2);
  gfx_set_scale_ratio(1,1);
  gfx_draw_tiled_frame(this,x2,y2,width2,height2,6);
  wnd_blit_surface();
  return;
}

/* 00574686 auto */
void __cdecl sprite_picker_redraw_cb(void)

{
  sprite_editor.sprite_picker_dialog->draw();
  return;
}

/* 005746a1 auto */
i32 __cdecl sprite_editor_cell_rect(uint cell,int *out_x,int *out_y,i32 *out_width,i32 *out_height)

{
  uint sign;
  i32 cell_count;
  
  *out_height = 0;
  *out_width = *out_height;
  *out_y = *out_width;
  *out_x = *out_y;
  cell_count = *out_x;
  if ((*(uint *)&sprite_picker_sprite_block.sprite_sheet_mode) < 0xc) {
    sign = (int)cell >> 0x1f;
    switch((*(uint *)&sprite_picker_sprite_block.sprite_sheet_mode)) {
    case 0:
      cell_count = 0x14;
      *out_x = ((int)(cell + (sign & 3)) >> 2) * 0x25 + 0x157;
      *(uint *)out_y = (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x15 + 0xd3;
      *out_width = 0x24;
      *out_height = 0x14;
      break;
    case 1:
      if ((int)cell < 0x18) {
        *out_x = 1;
      }
      else {
        *out_x = 0x14e;
        cell -= 0x18;
      }
      sign = (int)cell >> 0x1f;
      *(uint *)out_x = *out_x + (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x41;
      *out_y = ((int)(cell + (sign & 3)) >> 2) * 0x31 + 0x27;
      *out_width = 0x40;
      *out_height = 0x30;
      cell_count = 0x30;
      break;
    case 2:
      cell_count = 4;
      *(uint *)out_x = cell * 0x41 + 0x8f;
      *out_y = 0x1a7;
      *out_width = 0x40;
      *out_height = 0x30;
      break;
    case 3:
      cell_count = 0x12;
      *out_width = 0xe;
      *out_height = 0x16;
      *out_x = (((int)cell >> 1) % 9) * (*out_width + 1) + 1;
      *(uint *)out_y = (*out_height + 1) * (cell & 1) + 0x1a9;
      break;
    case 4:
      cell_count = 0x42;
      *out_width = 0x24;
      *out_height = 0x14;
      if ((int)cell < 0x26) {
        *(uint *)out_x = (((cell ^ sign) - sign & 7 ^ sign) - sign) * 0x25 + 0x157;
        *out_y = ((int)(cell + (sign & 7)) >> 3) * 0x15 + 1;
      }
      else {
        *out_x = ((int)(cell - 0x26) % 7) * 0x25 + 0x157;
        *out_y = ((int)(cell - 0x26) / 7) * 0x15 + 0x6a;
      }
      break;
    case 5:
      cell_count = 1;
      *out_width = 0x40;
      *out_height = 0x20;
      *out_x = 199;
      *out_y = 0x100;
      break;
    case 6:
      cell_count = 8;
      *out_width = 0x20;
      *out_height = 0x20;
      *(uint *)out_x = (*out_width + 1) * cell + 1;
      *out_y = 0x164;
      break;
    case 7:
      cell_count = 0x2c;
      *out_width = 0x40;
      *out_height = 0x20;
      *out_x = ((int)cell / 0xb) * 0x41 + 1;
      *out_y = ((int)cell % 0xb) * 0x21 + 1;
      break;
    case 8:
      *out_width = 0x40;
      *out_height = 0x20;
      if ((int)cell < 0x40) {
        *(uint *)out_x = (((cell ^ sign) - sign & 7 ^ sign) - sign) * 0x41 + 1;
        *(uint *)out_y =
             (((int)(((cell ^ sign) - sign & 0xf ^ sign) - sign) < 8) - 1 & 0x21) +
             ((int)(cell + (sign & 0xf)) >> 4) * 0x42 + 0x43;
      }
      else {
        *(uint *)out_x = (cell - 0x40) * 0x41 + 1;
        *out_y = 0x14b;
      }
      cell_count = 0x44;
      break;
    case 9:
      *out_width = 0x20;
      *out_height = 0x10;
      *out_x = 1;
      *out_y = 0x1ad;
      switch(((cell ^ sign) - sign & 3 ^ sign) - sign) {
      case 0:
        break;
      case 1:
        *out_y = *out_y + 0x22;
        *out_x = *out_x + 0x21;
        break;
      case 2:
        *out_y = *out_y + 0x11;
        break;
      case 3:
        *out_y = *out_y + 0x22;
      }
      *out_x = *out_x + ((int)(cell + (sign & 3)) >> 2) * 0x42;
      cell_count = 0x20;
      break;
    case 10:
      *out_width = 0x40;
      *out_height = 0x20;
      *out_x = 0x1c8;
      if ((int)cell < 6) {
        *(uint *)out_y = cell * 0x21 + 100;
      }
      else {
        *out_x = ((int)(cell - 6) % 9) * 0x41 + 1;
        *out_y = ((int)(cell - 6) / 9) * 0x21 + 0x16c;
      }
      cell_count = 0x18;
      break;
    case 0xb:
      cell_count = 0x3f;
      *out_y = ((int)cell / 9) * 0x31 + 1;
      *out_x = ((int)cell % 9) * 0x41 + 1;
      *out_width = 0x40;
      *out_height = 0x30;
    }
  }
  return cell_count;
}

/* 00574c47 auto */
void __cdecl sprite_picker_select_cell(i32 cell)

{
  i32 y2;
  i32 x2;
  i32 height2;
  i32 width2;
  
  sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2);
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->free();
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->grab_from_surface(&sprite_editor.sprite_sheet_surface,7,x2,y2,width2,height2);
  sprite_picker_redraw_cb();
  return;
}

/* 00574ca6 auto */
/* WARNING: Removing unreachable block (ram,0x00574e18) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
sprite_picker_create_window(const char *title,uint style_flags,int x,int y,int width,int height,int resize_border,
            int caption_height,int border_width)

{
  uint wnd_style;
  
  if (title == 0) {
    crt_strcpy(((i8 *)&sprite_editor.sprite_picker_title),((i8 *)&sprite_picker_default_title));
  }
  else {
    crt_strcpy(((i8 *)&sprite_editor.sprite_picker_title),title);
  }
  sprite_editor.sprite_picker_text_font = (iptr)&ui_text_font;
  sprite_editor.sprite_picker_title_font = (iptr)&caption_font_block.caption_font;
  sprite_editor.sprite_picker_title_small_font = (iptr)&caption_small_font_block.caption_small_font;
  sprite_editor.sprite_picker_style_flags = style_flags;
  sprite_editor.sprite_picker_caption_height = caption_height;
  sprite_editor.sprite_picker_border_width = border_width;
  sprite_editor.sprite_picker_resize_border = resize_border;
  sprite_editor.sprite_picker_caption_button_count = 0;
  sprite_editor.sprite_picker_caption_button_cb = 0;
  if ((style_flags & 4) != 0) {
    sprite_editor.sprite_picker_caption_height = ui_metrics.caption_height;
    sprite_editor.sprite_picker_border_width = ui_metrics.border_width;
  }
  if ((style_flags & 8) == 0) {
    wnd_style = 0x202;
  }
  else {
    wnd_style = 0x802;
  }
  if (sprite_editor.sprite_picker_caption_height != 0) {
    wnd_style = wnd_style | 0x400;
  }
  if (resize_border != 0) {
    wnd_style = wnd_style | 0x1000;
  }
  if ((style_flags & 2) == 0) {
    width += sprite_editor.sprite_picker_border_width * 2;
    height = height + sprite_editor.sprite_picker_caption_height + sprite_editor.sprite_picker_border_width;
  }
  if ((style_flags & 1) != 0) {
    x = (caption_font_block.screen_width >> 1) - (width >> 1);
    y = (caption_font_block.screen_height >> 1) - (height >> 1);
  }
  ((DibSurface *)((i8 *)&sprite_editor.sprite_picker_window))->wnd_create_with_surface(0,wnd_style,x,y,width,height,(GamePalette *)&game_palette_block.game_palette,
             &sprite_editor.wnd);
  if (sprite_editor.sprite_picker_caption_height != 0) {
    ((WndBase *)((i8 *)&sprite_editor.sprite_picker_wnd))->set_caption_height(sprite_editor.sprite_picker_caption_height);
  }
  if (sprite_editor.sprite_picker_resize_border != 0) {
    ((WndBase *)((i8 *)&sprite_editor.sprite_picker_wnd))->set_resize_border(sprite_editor.sprite_picker_resize_border);
  }
  ((FrameWindow *)((i8 *)&sprite_editor.sprite_picker_window))->layout();
  return;
}

/* 00574e98 auto */
void __cdecl sprite_picker_ok_clicked(void)

{
  byte key_color;
  i8 draw_rect[16];
  iptr height2;
  iptr width2;
  i8 grab_rect[16];
  
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->replace_color(9,7);
  width2 = ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->width();
  height2 = ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->height();
  key_color = ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->get_key_color();
  sprite_editor.key_color = (uint)key_color;
  sprite_editor_draw_backdrop();
  ((Sprite *)((i8 *)&sprite_picker_sprite_block.sprite_picker_sprite))->draw((RECT *)draw_rect,&sprite_editor.surface,0,0);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->free();
  rect_set_xywh((RECT *)grab_rect,0,0,width2,height2);
  ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->grab_surface_rect(&sprite_editor.surface,(RECT *)grab_rect);
  sprite_editor.sprite_picker_running = 0;
  sprite_editor.sprite_picker_dialog->window.end_modal_loop();
  return;
}

/* 00574f50 auto */
/* WARNING: Removing unreachable block (ram,0x00574f76) */

void __cdecl sprite_picker_import_all_clicked(void)

{
  uint sign;
  i8 draw_rect[16];
  int sheet_half;
  int size_col;
  int era_row;
  uint cell;
  i32 y2;
  i32 x2;
  i32 height2;
  i32 width2;
  uint half_cell;
  int answer;
  
  dlg_push_parent_window((iptr)&sprite_editor.sprite_picker_wnd);
  answer = popup_text_flags((const char *)&str_debug_11,(const char *)&str_rusure,1);
  dlg_pop_parent_window();
  if (answer == 0) {
    switch(sprite_picker_sprite_block.sprite_sheet_mode) {
    case 0:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        sign = (int)cell >> 0x1f;
        ((Sprite *)
                   (((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                   (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x3c +
                   ((int)(cell + (sign & 3)) >> 2) * 0xf0))->free();
        sign = (int)cell >> 0x1f;
        sprite_editor_grab_from_sheet((Sprite *)(((i8 *)&government_sprites_block.advance_category_icon_sprites) +
                           ((int)(cell + (sign & 3)) >> 2) * 0xf0 +
                           (((cell ^ sign) - sign & 3 ^ sign) - sign) * 0x3c),x2,
                     y2,width2,height2);
      }
      break;
    case 1:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        half_cell = (int)cell % 0x18;
        sign = (int)half_cell >> 0x1f;
        era_row = (int)(half_cell + (sign & 3)) >> 2;
        size_col = ((half_cell ^ sign) - sign & 3 ^ sign) - sign;
        sheet_half = (int)cell / 0x18;
        ((Sprite *)
                   (((i8 *)&gfx_sheet_surface_block.city_sprites) + size_col * 0x78 + era_row * 0x1e0 + sheet_half * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&gfx_sheet_surface_block.city_sprites) + era_row * 0x1e0 + sheet_half * 0x3c + size_col * 0x78)),
                     x2,y2,width2,height2);
      }
      break;
    case 2:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        switch(cell) {
        case 0:
          ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortify_overlay_sprites))->free();
          sprite_editor_grab_from_sheet(&fortify_overlay_sprites_block.fortify_overlay_sprites,x2,y2,width2,height2);
          break;
        case 1:
          ((Sprite *)((i8 *)&fortify_overlay_sprites_block.fortress_sprite))->free();
          sprite_editor_grab_from_sheet(&fortify_overlay_sprites_block.fortress_sprite,x2,y2,width2,height2);
          break;
        case 2:
          ((Sprite *)((i8 *)&airbase_sprites_block.airbase_sprites))->free();
          sprite_editor_grab_from_sheet(&airbase_sprites_block.airbase_sprites,x2,y2,width2,height2);
          break;
        case 3:
          ((Sprite *)((i8 *)&airbase_sprites_block.airbase_occupied_sprite))->free();
          sprite_editor_grab_from_sheet(&airbase_sprites_block.airbase_occupied_sprite,x2,y2,width2,height2);
        }
      }
      break;
    case 3:
      for (cell = 0; (int)cell < 8; cell++) {
        sprite_editor_cell_rect(cell * 2,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_sprites) + cell * 0x78)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 2 + 1,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&editor_tile_backdrop_sprite_block.civ_flag_dark_sprites) + cell * 0x78)),x2,y2,width2,height2);
      }
      break;
    case 4:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        if ((int)cell < 0x26) {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + (cell * 4 + 4) * 0xf))->free();
          sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&caption_close_button_sprite_block.improvement_icon_sprites) + (cell * 4 + 4) * 0xf)),x2,y2,width2,
                       height2);
        }
        else {
          ((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (cell * 4 - 0x98) * 0xf))->free();
          sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&caption_close_button_sprite_block.wonder_icon_sprites) + (cell * 4 - 0x98) * 0xf)),x2,y2,
                       width2,height2);
        }
      }
      break;
    case 5:
      break;
    case 6:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        ((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&government_sprites_block.explosion_sprites) + cell * 0x3c)),x2,y2,width2,height2);
      }
      break;
    case 7:
      for (cell = 0; (int)cell < 0xb; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        ((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&terrain_base_sprites_block.terrain_base_sprites) + cell * 0x3c)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x16,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource_sprites) + cell * 0x78)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x21,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + cell * 0x78))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&city_down_arrow_sprite_block.terrain_resource2_sprites) + cell * 0x78)),x2,y2,width2,height2);
      }
      break;
    case 8:
      for (cell = 0; (int)cell < 0x10; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        ((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&icon_misc_sprites_block.river_sprites) + cell * 0x3c)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x10,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&forest_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&forest_sprites) + cell * 0x3c)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x20,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&mountain_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&mountain_sprites) + cell * 0x3c)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0x30,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&hill_sprites_block.hill_sprites) + cell * 0x3c)),x2,y2,width2,height2);
      }
      for (cell = 0; (int)cell < 4; cell++) {
        sprite_editor_cell_rect(cell + 0x40,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&tile_pick_mask_sprite_block.river_mouth_sprites) + cell * 0x3c)),x2,y2,width2,height2);
      }
      break;
    case 9:
      for (cell = 0; (int)cell < 8; cell++) {
        sprite_editor_cell_rect(cell << 2,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_sprites) + cell * 0xf0)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 4 + 1,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_east_sprites) + cell * 0xf0)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 4 + 2,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_south_sprites) + cell * 0xf0)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell * 4 + 3,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + cell * 0xf0))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&tutorial_icon_sprite_block.coast_quarter_west_sprites) + cell * 0xf0)),x2,y2,width2,height2);
      }
      break;
    case 10:
      for (cell = 0; (int)cell < 3; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        ((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&land_improvement_sprites_block.land_improvement_sprites) + cell * 0x3c)),x2,y2,width2,height2);
      }
      sprite_editor_cell_rect(3,&x2,&y2,&width2,&height2);
      ((Sprite *)((i8 *)&pollution_sprite_block.pollution_sprite))->free();
      sprite_editor_grab_from_sheet((Sprite *)&pollution_sprite_block.pollution_sprite,x2,y2,width2,height2);
      sprite_editor_cell_rect(4,&x2,&y2,&width2,&height2);
      ((Sprite *)((i8 *)&grassland_shield_sprite))->free();
      sprite_editor_grab_from_sheet((Sprite *)&grassland_shield_sprite,x2,y2,width2,height2);
      sprite_editor_cell_rect(5,&x2,&y2,&width2,&height2);
      ((Sprite *)((i8 *)&hut_sprite))->free();
      sprite_editor_grab_from_sheet((Sprite *)&hut_sprite,x2,y2,width2,height2);
      for (cell = 0; (int)cell < 9; cell++) {
        sprite_editor_cell_rect(cell + 6,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&road_sprites_block.road_sprites) + cell * 0x3c)),x2,y2,width2,height2);
        sprite_editor_cell_rect(cell + 0xf,&x2,&y2,&width2,
                           &height2);
        ((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&road_sprites_block.railroad_sprites) + cell * 0x3c)),x2,y2,width2,height2);
      }
      break;
    case 0xb:
      for (cell = 0; (int)cell < sprite_editor.sprite_sheet_cell_count; cell++) {
        sprite_editor_cell_rect(cell,&x2,&y2,&width2,&height2)
        ;
        ((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + cell * 0x3c))->free();
        sprite_editor_grab_from_sheet(((Sprite *)(((i8 *)&pollution_sprite_block.unit_sprites) + cell * 0x3c)),x2,y2,width2,height2);
      }
    }
    ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->free();
    sprite_editor.target_sprite->merge_onto((Sprite *)&sprite_editor.orig_sprite);
    ((DibSurface *)((i8 *)&sprite_editor.surface))->DibSurface::create(sprite_editor.width,sprite_editor.height);
    sprite_editor_draw_backdrop();
    ((Sprite *)((i8 *)&sprite_editor.orig_sprite))->draw((RECT *)draw_rect,&sprite_editor.surface,0,0);
    ((Sprite *)((i8 *)&sprite_editor.result_sprite))->free();
    sprite_editor.target_sprite->merge_onto((Sprite *)&sprite_editor.result_sprite);
    sprite_editor.sprite_picker_running = 0;
    sprite_editor.sprite_picker_dialog->window.end_modal_loop();
    ((SurfaceWindow *)((i8 *)&map_views_block.map_views))->repaint();
  }
  return;
}

/* 00575d89 auto */
void __cdecl
sprite_editor_grab_from_sheet(Sprite *sprite,i32 x,i32 y,i32 width,
            i32 height)

{
  (sprite)->grab_from_surface(&sprite_editor.sprite_sheet_surface,7,x,y,width,height);
  (sprite)->replace_color(9,7);
  return;
}

/* 00575dc4 auto */
void __cdecl sprite_picker_cancel_clicked(void)

{
  sprite_editor.sprite_picker_running = 0;
  sprite_editor.sprite_picker_dialog->window.end_modal_loop();
  return;
}

/* 00575dec auto */
void SpritePicker::run()

{
  i32 title;
  int metric;
  iptr label;
  i32 style_flags;
  i32 win_x;
  i32 win_y;
  u32 win_width;
  u32 win_height;
  i32 resize_border;
  i32 caption_height;
  i32 border_width;
  u8 *cancel_parent;
  u8 *import_parent;
  u8 *ok_parent;
  u8 *scroll_parent;
  i8 dummy_out[4];
  int y2;
  int x2;
  i8 rect[16];
  iptr scroll_height;
  
  sprite_editor.sprite_picker_dialog = this;
  rates_civ = 0xf0;
  max_rate_value = 0xf0;
  border_width = 0;
  caption_height = 0;
  resize_border = 0;
  win_height = max_rate_value;
  win_width = rates_civ;
  win_y = 0;
  win_x = 0;
  style_flags = 0xd;
  title = (iptr)label_string(label_table.string_ids[0x254]);
  sprite_picker_create_window((const char *)title,style_flags,win_x,win_y,win_width,win_height,resize_border,caption_height,border_width);
  metric = ((GameFont *)(*(i8 * *)(dlg_style.default_text_font_rest + 8)))->line_spacing();
  init_sci_rate = metric + 8;
  init_lux_rate = client_width - 4;
  metric = GetSystemMetrics(3);
  scroll_height = metric + 2;
  sci_rate_sel = client_width - 4;
  y2 = ((client_y + client_height) - (init_sci_rate + 2)) - (metric + 4);
  x2 = client_x + 2;
  sprite_editor.sprite_sheet_cell_count = sprite_editor_cell_rect(0,(int *)dummy_out,(int *)dummy_out,(i32 *)dummy_out,(i32 *)dummy_out);
  rect_set_xywh((RECT *)rect,x2,y2,sci_rate_sel,scroll_height);
  if (this == (SpritePicker *)0x0) {
    scroll_parent = (u8 *)0x0;
  }
  else {
    scroll_parent = (u8 *)&window;
  }
  scrollbar.create((Wnd *)scroll_parent,0x65,(RECT *)rect,0);
  scrollbar.set_range(0,sprite_editor.sprite_sheet_cell_count - 1);
  scrollbar.set_pos(0);
  scrollbar.set_callback(sprite_picker_select_cell);
  sprite_picker_select_cell(0);
  ((Control *)&scrollbar_head)->show_and_redraw();
  init_lux_rate = (int)(client_width - 8) / 3;
  y2 = (client_y + client_height) - (init_sci_rate + 2);
  x2 = client_x + 2;
  rect_set_xywh((RECT *)rect,x2,y2,init_lux_rate,init_sci_rate);
  if (this == (SpritePicker *)0x0) {
    ok_parent = (u8 *)0x0;
  }
  else {
    ok_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xfe]);
  button_1.create((Wnd *)ok_parent,0xc9,(RECT *)rect,(const char *)label);
  button_1.set_callback((CtrlNotifyFn)sprite_picker_ok_clicked);
  button_1.set_default_button();
  x2 = x2 + init_lux_rate + 2;
  rect_set_xywh((RECT *)rect,x2,y2,init_lux_rate,init_sci_rate);
  if (this == (SpritePicker *)0x0) {
    import_parent = (u8 *)0x0;
  }
  else {
    import_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0x255]);
  button_2.create((Wnd *)import_parent,0xc9,(RECT *)rect,(const char *)label);
  button_2.set_callback((CtrlNotifyFn)sprite_picker_import_all_clicked);
  button_2.set_default_button();
  if (sprite_picker_sprite_block.sprite_sheet_mode == 5) {
    button_2.disable();
  }
  x2 = x2 + init_lux_rate + 2;
  rect_set_xywh((RECT *)rect,x2,y2,init_lux_rate,init_sci_rate);
  if (this == (SpritePicker *)0x0) {
    cancel_parent = (u8 *)0x0;
  }
  else {
    cancel_parent = (u8 *)&window;
  }
  label = (iptr)label_string(label_table.string_ids[0xff]);
  button_3.create((Wnd *)cancel_parent,0xca,(RECT *)rect,(const char *)label);
  button_3.set_callback((CtrlNotifyFn)sprite_picker_cancel_clicked);
  button_3.set_cancel_button();
  set_redraw_callback(sprite_picker_redraw_cb);
  repaint();
  show();
  ((ModalMenuWindow *)&window)->run_modal_loop();
  while (sprite_editor.sprite_picker_running != 0) {
    win_pump_one_message();
  }
  close();
  return;
}

/* 0057624d auto */
void __cdecl sprite_picker_show(void)

{
  ((SpritePicker *)((i8 *)&sprite_editor.sprite_picker_window))->run();
  return;
}

/* 00576267 auto */
/* WARNING: Removing unreachable block (ram,0x0057656d) */
/* WARNING: Removing unreachable block (ram,0x005763fb) */
/* WARNING: Removing unreachable block (ram,0x00576601) */

void __cdecl sprite_editor_import_sheet(void)

{
  i32 label;
  iptr ok;
  size_t _MaxCount;
  iptr cmp;
  int result;
  const char *str;
  i32 arg_a;
  const char *arg_name;
  i32 arg_b;
  LPCSTR caption_text;
  LPCSTR title_text;
  i32 arg_c;
  UINT mb_flags;
  char prompt[132];
  int retry;
  char file_path[260];
  const char *sheet_name;
  char filter[260];
  GamePalette palette;
  char *file_name;
  
  palette.init();
  ON_SCOPE_EXIT(palette.destroy());
  Dialog dialog2(0x4000);
  ON_SCOPE_EXIT(((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->release());
  switch(sprite_picker_sprite_block.sprite_sheet_mode) {
  case 0:
  case 4:
  case 5:
  case 6:
    sheet_name = str_icons;
    break;
  case 1:
  case 2:
  case 3:
    sheet_name = str_cities_2;
    break;
  case 7:
  case 8:
  case 9:
  case 10:
    sheet_name = str_terrain_5;
    break;
  case 0xb:
    sheet_name = str_units_2;
  }
  do {
    arg_c = 0;
    arg_b = 0;
    arg_a = 0;
    str = sheet_name;
    arg_name = sheet_name;
    label = (iptr)label_string(label_table.string_ids[0x256]);
    _sprintf(filter,str_s_s_c_s_bmp_s_gif_c_c,sheet_name,label,arg_a,str,arg_name,
             arg_b,arg_c);
    _sprintf(file_path,str_s_bmp_s_gif,sheet_name,sheet_name);
    str = sheet_name;
    label = (iptr)label_string(label_table.string_ids[0x257]);
    _sprintf(prompt,str_s_s_2,label,str);
    ok = ui_file_name_dialog(&sprite_editor.wnd,(const char *)prompt,file_path,filter,str_bmp_gif,1,0
                              );
    if (ok == 0) return;
    __strupr(file_path);
    retry = 1;
    file_name = _strrchr(file_path,0x5c);
    if (file_name == (char *)0x0) {
      file_name = file_path;
    }
    else {
      file_name++;
    }
    _MaxCount = _strlen(sheet_name);
    cmp = _strncmp(file_name,sheet_name,_MaxCount);
    if (cmp == 0) {
      if (((sprite_picker_sprite_block.sprite_sheet_mode == 7) || (sprite_picker_sprite_block.sprite_sheet_mode == 10)) &&
          ((result = _isdigit((int)file_name[7]), result == 0 || ((file_name[7] & 1U) == 0)))) {
        dlg_push_parent_window((iptr)&sprite_editor.sprite_picker_wnd);
        popup_debug_text((const char *)&str_oddterrain);
        dlg_pop_parent_window();
      }
      else if (((sprite_picker_sprite_block.sprite_sheet_mode == 8) || (sprite_picker_sprite_block.sprite_sheet_mode == 9)) &&
              ((result = _isdigit((int)file_name[7]), result == 0 || ((file_name[7] & 1U) != 0)))) {
        dlg_push_parent_window((iptr)&sprite_editor.sprite_picker_wnd);
        popup_debug_text((const char *)&str_eventerrain);
        dlg_pop_parent_window();
      }
      else {
        str = _strstr(file_path,((i8 *)&sprite_import_bmp_ext_str));
        if (str == (char *)0x0) {
          str = _strstr(file_path,((i8 *)&sprite_import_gif_ext_str));
          if (str == (char *)0x0) {
            mb_flags = 0x10;
            title_text = (LPCSTR)0x0;
            caption_text = (LPCSTR)label_string(label_table.string_ids[0x25a]);
            result = (int)((WndBase *)((i8 *)&sprite_editor.wnd))->get_window();
            MessageBoxA(((NativeWindow *)result)->hwnd,caption_text,title_text,mb_flags);
          }
          else {
            result = ((DibSurface *)((i8 *)&sprite_editor.sprite_sheet_surface))->load_gif_file((const char *)file_path,10,0xc0,
                               &palette);
            if (result == 0) {
              mb_flags = 0x10;
              title_text = (LPCSTR)0x0;
              caption_text = (LPCSTR)label_string(label_table.string_ids[0x259]);
              result = (int)((WndBase *)((i8 *)&sprite_editor.wnd))->get_window();
              MessageBoxA(((NativeWindow *)result)->hwnd,caption_text,title_text,mb_flags);
            }
            else {
              retry = 0;
            }
          }
        }
        else {
          result = gfx_load_bmp_640x480(&sprite_editor.sprite_sheet_surface,(const char *)file_path,10,0xc0,&palette);
          if (result == 0) {
            mb_flags = 0x10;
            title_text = (LPCSTR)0x0;
            caption_text = (LPCSTR)label_string(label_table.string_ids[0x259]);
            result = (int)((WndBase *)((i8 *)&sprite_editor.wnd))->get_window();
            MessageBoxA(((NativeWindow *)result)->hwnd,caption_text,title_text,mb_flags);
          }
          else {
            retry = 0;
          }
        }
      }
    }
    else {
      str = sheet_name;
      label = (iptr)label_string(label_table.string_ids[0x258]);
      _sprintf(prompt,str_s_s_3,label,str);
      mb_flags = 0x40;
      caption_text = (LPCSTR)0x0;
      str = prompt;
      result = (int)((WndBase *)((i8 *)&sprite_editor.wnd))->get_window();
      MessageBoxA(((NativeWindow *)result)->hwnd,str,caption_text,mb_flags);
    }
  } while (retry != 0);
  sprite_picker_show();
  sprite_editor_redraw();
  return;
}

/* 00578390 auto */
/* Library Function - Single Match
    public: virtual __thiscall CBitmapButton::~CBitmapButton(void)
   
   Library: Visual Studio 1998 Debug */

SpritePicker::~SpritePicker()

{
  int seh_state;
  
  seh_state = seh_state & 0xff;
  (*((char *)&seh_state + 0)) = 3;
  /* inlined unwind funclet 00578402 */ { ctrl_dtor(&button_3); }
  (*((char *)&seh_state + 0)) = 2;
  /* inlined unwind funclet 00578411 */ { ctrl_dtor(&button_2); }
  (*((char *)&seh_state + 0)) = 1;
  /* inlined unwind funclet 00578420 */ { ctrl_dtor(&button_1); }
  seh_state = seh_state & 0xffffff00;
  /* inlined unwind funclet 0057842f */ { scrollbar.destruct(); }
  seh_state = 0xffffffff;
  /* inlined unwind funclet 0057843e */ { FrameWindow::destruct(); }
  return;
}

/* 005784a0 auto */
SpritePicker::SpritePicker()

{
  
  FrameWindow::construct();
  scrollbar.construct();
  button_ctor(&button_1);
  button_ctor(&button_2);
  button_ctor(&button_3);
  vtable = (u32)&leader_anim_frame_starts_block.ptr_fun_4;
}
